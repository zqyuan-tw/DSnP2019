/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
  for (unsigned i = 0; i < M + 1; ++i)
  {
    if (_gateList[i].type == PI_GATE || _gateList[i].type == CONST_GATE) continue;
    if (!find(i, DFSList))
    {
      if (_gateList[i].type == UNDEF_GATE && _gateList[i].fanout.empty()) continue;
      cout << "Sweeping: " << _gateList[i].getTypeStr() << "(" << i << ") removed..." << endl;
      existErase(i, AIGList); //AIGList.erase(remove(AIGList.begin(), AIGList.end(), i), AIGList.end());
      _gateList[i].fanout.clear();
      _gateList[i].type = UNDEF_GATE;
      if (_gateList[i].fanin1 != 4294967295) existErase(2 * i + _gateList[i].fanin1 % 2, _gateList[_gateList[i].fanin1 / 2].fanout);
      if (_gateList[i].fanin2 != 4294967295) existErase(2 * i + _gateList[i].fanin2 % 2, _gateList[_gateList[i].fanin2 / 2].fanout);
    }
  }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
  unsigned i = 1;
  while (i != 0)
  {
    i = 0;
    for (unsigned j = 0; j < DFSList.size(); ++j)
    {
      if (_gateList[DFSList[j]].type != AIG_GATE) continue;
      if ((!_gateList[DFSList[j]].fanin1) || (!_gateList[DFSList[j]].fanin2)) //one fanin is const 0
      {
        ++i;
        cout << "Simplifying: " << 0 << " merging ";
        cout <<  DFSList[j] << "..." << endl;
        existErase(DFSList[j], AIGList);
        for (unsigned k = 0; k < _gateList[DFSList[j]].fanout.size(); ++k) //check fanout
        {
          if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == PO_GATE)
          {
            if (_gateList[DFSList[j]].fanout[k] % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 1;
            else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 0;
          }
          else if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == AIG_GATE)
          {
            if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 / 2 == DFSList[j])
            {
              if (_gateList[DFSList[j]].fanout[k] % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 0;
            }
            else //if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 / 2 == DFSList[j])
            {
              if (_gateList[DFSList[j]].fanout[k] % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = 0;
            }
          }
          _gateList[0].fanout.push_back(_gateList[DFSList[j]].fanout[k]);
        }
        if (_gateList[DFSList[j]].fanin1 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin1 % 2, _gateList[_gateList[DFSList[j]].fanin1 / 2].fanout);
        if (_gateList[DFSList[j]].fanin2 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin2 % 2, _gateList[_gateList[DFSList[j]].fanin2 / 2].fanout);
        _gateList[DFSList[j]].fanout.clear();
        _gateList[DFSList[j]].type = UNDEF_GATE;
      }
      else if ((_gateList[DFSList[j]].fanin1 == 1) || (_gateList[DFSList[j]].fanin2 == 1)) //one fanin is const 1
      {
        ++i;
        if (_gateList[DFSList[j]].fanin1 == 1)
        {
          cout << "Simplifying: " << _gateList[DFSList[j]].fanin2 / 2 << " merging ";
          if (_gateList[DFSList[j]].fanin2 % 2) cout << "!";
          for (unsigned k = 0; k < _gateList[DFSList[j]].fanout.size(); ++k)
          {
            if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == PO_GATE)            
            {
              if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2 + 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2;
            }
            else if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == AIG_GATE)
            {
              if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 / 2 == DFSList[j]) //fanin1 connect to this gate
              {
                if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2 + 1;
                else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2;
              }
              else
              {
                if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = _gateList[DFSList[j]].fanin2 / 2 * 2 + 1;
                else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = _gateList[DFSList[j]].fanin2 / 2 * 2;
              }
            }
            _gateList[_gateList[DFSList[j]].fanin2 / 2].fanout.push_back(_gateList[DFSList[j]].fanout[k] / 2 * 2 + (_gateList[DFSList[j]].fanin2 + _gateList[DFSList[j]].fanout[k]) % 2);
          }
        }
        else
        {
          cout << "Simplifying: " << _gateList[DFSList[j]].fanin1 / 2 << " merging ";
          if (_gateList[DFSList[j]].fanin1 % 2) cout << "!";
          for (unsigned k = 0; k < _gateList[DFSList[j]].fanout.size(); ++k)
          {
            if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == PO_GATE)            
            {
              if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin1) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin1 / 2 * 2 + 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin1 / 2 * 2;
            }
            else if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == AIG_GATE)
            {
              if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 / 2 == DFSList[j]) //fanin1 connect to this gate
              {
                if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin1) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin1 / 2 * 2 + 1;
                else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin1 / 2 * 2;
              }
              else
              {
                if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin1) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = _gateList[DFSList[j]].fanin1 / 2 * 2 + 1;
                else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = _gateList[DFSList[j]].fanin1 / 2 * 2;
              }
            }
            _gateList[_gateList[DFSList[j]].fanin1 / 2].fanout.push_back(_gateList[DFSList[j]].fanout[k] / 2 * 2 + (_gateList[DFSList[j]].fanin1 + _gateList[DFSList[j]].fanout[k]) % 2);
          }
        }
        cout <<  DFSList[j] << "..." << endl;
        existErase(DFSList[j], AIGList);
        if (_gateList[DFSList[j]].fanin1 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin1 % 2, _gateList[_gateList[DFSList[j]].fanin1 / 2].fanout);
        if (_gateList[DFSList[j]].fanin2 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin2 % 2, _gateList[_gateList[DFSList[j]].fanin2 / 2].fanout);
        _gateList[DFSList[j]].fanout.clear();
        _gateList[DFSList[j]].type = UNDEF_GATE;
        
      }
      else if (_gateList[DFSList[j]].fanin1 == _gateList[DFSList[j]].fanin2) // same fanin
      {
        ++i;
        cout << "Simplifying: " << _gateList[DFSList[j]].fanin1 / 2 << " merging ";
        if (_gateList[DFSList[j]].fanin1 % 2) cout << "!";
        cout <<  DFSList[j] << "..." << endl;
        existErase(DFSList[j], AIGList);
        for (unsigned k = 0; k < _gateList[DFSList[j]].fanout.size(); ++k) //check fanout
        {
          if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == PO_GATE)
          {
            if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2 + 1;
            else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2;
          }
          else if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == AIG_GATE)
          {
            if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 / 2 == DFSList[j]) //fanin1 connect to this gate
            {
              if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2 + 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = _gateList[DFSList[j]].fanin2 / 2 * 2;
            }
            else
            {
              if ((_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = _gateList[DFSList[j]].fanin2 / 2 * 2 + 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = _gateList[DFSList[j]].fanin2 / 2 * 2;
            }
          }
          _gateList[_gateList[DFSList[j]].fanin2 / 2].fanout.push_back(_gateList[DFSList[j]].fanout[k] / 2 * 2 + (_gateList[DFSList[j]].fanout[k] + _gateList[DFSList[j]].fanin2) % 2);
        }
        if (_gateList[DFSList[j]].fanin1 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin1 % 2, _gateList[_gateList[DFSList[j]].fanin1 / 2].fanout);
        if (_gateList[DFSList[j]].fanin2 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin2 % 2, _gateList[_gateList[DFSList[j]].fanin2 / 2].fanout);
        _gateList[DFSList[j]].fanout.clear();
        _gateList[DFSList[j]].type = UNDEF_GATE;
      }
      else if ((_gateList[DFSList[j]].fanin1 + _gateList[DFSList[j]].fanin2) % 2 && _gateList[DFSList[j]].fanin1 / 2 == _gateList[DFSList[j]].fanin2 / 2) //opposite fanin
      {
        ++i;
        cout << "Simplifying: " << 0 << " merging ";
        cout <<  DFSList[j] << "..." << endl;
        existErase(DFSList[j], AIGList);
        for (unsigned k = 0; k < _gateList[DFSList[j]].fanout.size(); ++k) //check fanout
        {
          if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == PO_GATE)
          {
            if (_gateList[DFSList[j]].fanout[k] % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 1;
            else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 0;
          }
          else if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].type == AIG_GATE)
          {
            if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 / 2 == DFSList[j])
            {
              if (_gateList[DFSList[j]].fanout[k] % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin1 = 0;
            }
            else //if (_gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 / 2 == DFSList[j])
            {
              if (_gateList[DFSList[j]].fanout[k] % 2) _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = 1;
              else _gateList[_gateList[DFSList[j]].fanout[k] / 2].fanin2 = 0;
            }
          }
          _gateList[0].fanout.push_back(_gateList[DFSList[j]].fanout[k]);
        }
        if (_gateList[DFSList[j]].fanin1 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin1 % 2, _gateList[_gateList[DFSList[j]].fanin1 / 2].fanout);
        if (_gateList[DFSList[j]].fanin2 != 4294967295) existErase(2 * DFSList[j] + _gateList[DFSList[j]].fanin2 % 2, _gateList[_gateList[DFSList[j]].fanin2 / 2].fanout);
        _gateList[DFSList[j]].fanout.clear();
        _gateList[DFSList[j]].type = UNDEF_GATE;
      }
    }
    DFS();
  }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
