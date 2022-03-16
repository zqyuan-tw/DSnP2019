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

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
   string gate = getTypeStr() + "(" + to_string(this - cirMgr -> getGate(0)) + ")";
   if (!symbol.empty()) gate += "\"" + symbol + "\"";
   gate += ", line " + to_string(getLineNo());
   cout << "==================================================" << endl;
   cout << "= " << left << setw(47) << gate << "=" << endl;
   cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   ++cirMgr -> global_ref;
   faninDFS(level, 2);
}

void
CirGate::reportFanout(int level) const
{
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
      default:
         return "UNDEF";
   }
}

void CirGate::po_in(int a)
{
   fanin1 = a / 2;
   inv1 = a % 2;
}

void CirGate::aig_in(int a, int b)
{
   fanin1 = a / 2;
   inv1 = a % 2;
   fanin2 = b / 2;
   inv2 = b % 2;
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
      if (inv1) cout << "!";
      if (cirMgr -> getGate(fanin1) != NULL)
      {
         cirMgr -> getGate(fanin1) ->faninDFS(l - 1, s + 2);
      }
      else
      {
         cout << cirMgr -> getGate(fanin1) -> getTypeStr() << " " << fanin1 << endl;
      }      
   }
   else if (type == AIG_GATE)
   {
      for (int i = 0; i < s; ++i)
      {
         cout << " ";
      }
      if (inv1) cout << "!";
      if (cirMgr -> getGate(fanin1) != NULL)
      {
         cirMgr -> getGate(fanin1) ->faninDFS(l - 1, s + 2);
      }
      else
      {
         cout << cirMgr -> getGate(fanin1) -> getTypeStr() << " " << fanin1 << endl;
      }
      for (int i = 0; i < s; ++i)
      {
         cout << " ";
      }
      if (inv2) cout << "!";
      if (cirMgr -> getGate(fanin2) != NULL)
      {
         cirMgr -> getGate(fanin2) ->faninDFS(l - 1, s + 2);
      }
      else
      {
         cout << cirMgr -> getGate(fanin2) -> getTypeStr() << " " << fanin2 << endl;
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