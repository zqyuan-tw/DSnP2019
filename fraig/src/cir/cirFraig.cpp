/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
  HashSet<HashKey> strh(getHashSize(DFSList.size())); 
  for (unsigned i = 0; i < DFSList.size(); ++i)
  {
    if (_gateList[DFSList[i]].type != AIG_GATE) continue;
    HashKey hk(DFSList[i]);
    if (!strh.insert(hk))
    {
      strh.query(hk);
      cout << "Strashing: " << hk.getId() << " merging " << DFSList[i] << "..." << endl;
      for (unsigned j = 0; j < _gateList[DFSList[i]].fanout.size(); ++j)
      {
        if (_gateList[_gateList[DFSList[i]].fanout[j] / 2].type == PO_GATE)
        {
          _gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin1 = 2 * hk.getId() + _gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin1 % DFSList[i];
          _gateList[hk.getId()].fanout.push_back(_gateList[DFSList[i]].fanout[j]);
        }
        else if (_gateList[_gateList[DFSList[i]].fanout[j] / 2].type == AIG_GATE)
        {
          if (_gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin1 / 2 == DFSList[i])
          {
            _gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin1 = 2 * hk.getId() + _gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin1 % DFSList[i];
            _gateList[hk.getId()].fanout.push_back(_gateList[DFSList[i]].fanout[j]);
          }
          if (_gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin2 / 2 == DFSList[i])
          {
            _gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin2 = 2 * hk.getId() + _gateList[_gateList[DFSList[i]].fanout[j] / 2].fanin2 % DFSList[i];
            _gateList[hk.getId()].fanout.push_back(_gateList[DFSList[i]].fanout[j]);
          }
        }
      }
      existErase(DFSList[i], AIGList);
      if (_gateList[DFSList[i]].fanin1 != 4294967295) existErase(2 * DFSList[i] + _gateList[DFSList[i]].fanin1 % 2, _gateList[_gateList[DFSList[i]].fanin1 / 2].fanout);
      if (_gateList[DFSList[i]].fanin2 != 4294967295) existErase(2 * DFSList[i] + _gateList[DFSList[i]].fanin2 % 2, _gateList[_gateList[DFSList[i]].fanin2 / 2].fanout);
      _gateList[DFSList[i]].fanout.clear();
      _gateList[DFSList[i]].type = UNDEF_GATE;
      existErase(DFSList[i], DFSList);
      --i;
    }
  }
  DFS();
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
