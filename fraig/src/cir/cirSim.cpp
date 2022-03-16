/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <random>
#include <stdlib.h>

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
  resetSim();
  random_device rd;
  default_random_engine generator(rd());
  uniform_int_distribution<long long unsigned> distribution(0, 0xFFFFFFFFFFFFFFFF);
  int count = rand() % 64 + 1;
  for (int i = 0; i < 64 * count; ++i)
  {
    for (unsigned j = 0; j < PIList.size(); ++j)
    {
      _gateList[PIList[j]].sim = distribution(generator);
    }
    simprocess();
    if (_simLog)
    {
      for (unsigned m = 0; m < 64; ++m)
      {
        for (unsigned n = 0; n < PIList.size(); ++n)
        {
          *_simLog << ((_gateList[PIList[n]].sim >> m) & 1);
        }
        *_simLog << " ";
        for (unsigned n = M + 1; n < M + O + 1; ++n)
        {
          *_simLog << ((_gateList[n].sim >> m) & 1);
        }
        *_simLog << endl;
      }
    }
  }
  cout << 64 * count << " patterns simulated." << endl;
  //FEC();
}

void
CirMgr::fileSim(ifstream& patternFile)
{
  string p;  
  unsigned count = 0;
  while (patternFile >> p)
  {
    if (!(count % 64)) resetSim();
    if (p.size() != PIList.size())
    {
      cerr << "Error: Pattern(" << p << ") length(" << p.length() << ") does not match the number of inputs(" << PIList.size() << ") in a circuit!!" << endl;
      return;
    }
    for (unsigned i = 0; i < PIList.size(); ++i)
    {
      if (p[i] != '0' && p[i] != '1')
      {
        cerr << "Error: Pattern(" << p << ") contains a non-0/1 character(\'" << p[i] << "\')." << endl;
        cout << "0 patterns simulated." << endl;
        return;
      }
      _gateList[PIList[i]].sim = _gateList[PIList[i]].sim + (size_t(p[i] - '0') << (count % 64));
    }
    ++count;
    if (!(count % 64))
    {
      simprocess();
      if (_simLog)
      {
        for (unsigned i = 0; i < 64; ++i)
        {
          for (unsigned j = 0; j < PIList.size(); ++j)
          {
            *_simLog << ((_gateList[PIList[j]].sim >> i) & 1);
          }
          *_simLog << " ";
          for (unsigned j = M + 1; j < M + O + 1; ++j)
          {
            *_simLog << ((_gateList[j].sim >> i) & 1);
          }
          *_simLog << endl;
        }
      }
    }
  }
  if (count % 64)
  {
    simprocess();
    if (_simLog)
      {
        for (unsigned i = 0; i < (count % 64); ++i)
      {
        for (unsigned j = 0; j < PIList.size(); ++j)
        {
          *_simLog << ((_gateList[PIList[j]].sim >> i) & 1);
        }
        *_simLog << " ";
        for (unsigned j = M + 1; j < M + O + 1; ++j)
        {
          *_simLog << ((_gateList[j].sim >> i) & 1);
        }
        *_simLog << endl;
      }
    }
  }
  cout << count << " patterns simulated." << endl;
  //FEC();
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
