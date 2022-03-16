/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   if (type == UNDEF_GATE && fanout.empty())
   {
      cerr << "Error: Gate(" << this - cirMgr -> getGate(0) << ") not found!!" << endl;
      return;
   }
   string gate = getTypeStr() + "(" + to_string(this - cirMgr -> getGate(0)) + ")";
   if (!symbol.empty()) gate += "\"" + symbol + "\"";
   gate += ", line " + to_string(getLineNo());
   cout << "================================================================================" << endl;
   cout << "= " << left << setw(47) << gate << endl;
   cout << "= FECs: ";
   if (type == AIG_GATE)
   {
      if (buc != 4294967295)
      {
         for (unsigned i = 0; i < cirMgr -> fecg[buc].size(); ++i)
         {
            if (cirMgr -> fecg[buc][i] == this - cirMgr -> getGate(0)) continue;
            if (cirMgr -> _gateList[cirMgr -> fecg[buc][i]].sim != sim) cout << "!";
            cout << cirMgr -> fecg[buc][i] << " ";
         }
      }
   }
   cout << endl << "= Value: ";
   for (unsigned i = 0; i < 64; ++i)
   {
      if (i && !(i % 8)) cout << "_";
      cout << ((sim >> (63 - i)) & 1);
   }
   cout << endl;
   cout << "================================================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   if (type == UNDEF_GATE && fanout.empty())
   {
      cerr << "Error: Gate(" << this - cirMgr -> getGate(0) << ") not found!!" << endl;
      return;
   }
   assert (level >= 0);
   ++cirMgr -> global_ref;
   faninDFS(level, 2);
}

void
CirGate::reportFanout(int level) const
{
   if (type == UNDEF_GATE && fanout.empty())
   {
      cerr << "Error: Gate(" << this - cirMgr -> getGate(0) << ") not found!!" << endl;
      return;
   }
   assert (level >= 0);
   ++cirMgr -> global_ref;
   fanoutDFS(level, 2);
}

string CirGate::getTypeStr() const
{
   switch(type)
   {
      case UNDEF_GATE:
         return "UNDEF";
      case PI_GATE:
         return "PI";
      case PO_GATE:
         return "PO";
      case AIG_GATE:
         return "AIG";
      case CONST_GATE:
         return "CONST";
      case LATCH:
         return "LATCH";
      default:
         return "UNDEF";
   }
}

void CirGate::faninDFS(int l, int s) const
{
   cout << getTypeStr() << " " << this - cirMgr -> getGate(0);
   if (l == 0)
   {
      cout << endl;
      return;
   }
   if (ref == cirMgr -> global_ref)
   {
      cout << " (*)" << endl;
      return;
   }
   cout << endl;
   ref = cirMgr -> global_ref;
   if (type == PO_GATE)
   {
      for (int i = 0; i < s; ++i)
      {
         cout << " ";
      }
      if (fanin1 % 2) cout << "!";
      if (cirMgr -> getGate(fanin1 / 2) != NULL)
      {
         cirMgr -> getGate(fanin1 / 2) ->faninDFS(l - 1, s + 2);
      }
      else
      {
         cout << "UNDEF " << fanin1 / 2 << endl;
      }      
   }
   else if (type == AIG_GATE)
   {
      for (int i = 0; i < s; ++i)
      {
         cout << " ";
      }
      if (fanin1 % 2) cout << "!";
      if (cirMgr -> getGate(fanin1 / 2) != NULL)
      {
         cirMgr -> getGate(fanin1 / 2) ->faninDFS(l - 1, s + 2);
      }
      else
      {
         cout << "UNDEF " << fanin1 / 2 << endl;
      }
      for (int i = 0; i < s; ++i)
      {
         cout << " ";
      }
      if (fanin2 % 2) cout << "!";
      if (cirMgr -> getGate(fanin2 / 2) != NULL)
      {
         cirMgr -> getGate(fanin2 / 2) ->faninDFS(l - 1, s + 2);
      }
      else
      {
         cout << "UNDEF " << fanin2 / 2 << endl;
      }
   }
   else
   {
      return;
   }   
}

void CirGate::fanoutDFS(int l, int s) const
{
   cout << getTypeStr() << " " << this - cirMgr -> getGate(0);
   if (l == 0)
   {
      cout << endl;
      return;
   }
   if (ref == cirMgr -> global_ref)
   {
      cout << " (*)" << endl;
      return;
   }
   cout << endl;
   ref = cirMgr -> global_ref;
   for (int i = 0; i < fanout.size(); ++i)
   {
      for (int j = 0; j < s; ++j)
      {
         cout << " ";
      }
      if (fanout[i] % 2) cout << "!";
      cirMgr -> getGate(fanout[i] / 2) -> fanoutDFS(l - 1, s + 2);
   }
}