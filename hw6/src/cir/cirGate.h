/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
   friend class CirMgr;
public:
   CirGate(): type(UNDEF_GATE), line(0), fanin1(-1), fanin2(-1), ref(0), symbol("") {}
   ~CirGate() {}

   // Basic access methods
   string getTypeStr() const;
   unsigned getLineNo() const { return line; }
   void po_in(int);
   void po_out(int);
   void aig_in(int, int);

   // Printing functions
   void printGate() const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void faninDFS(int, int) const;
   void fanoutDFS(int, int) const;

protected:
   GateType type;
   int line;
   int fanin1;
   bool inv1;
   int fanin2;
   bool inv2;
   IdList fanout;
   mutable int ref;
   string symbol;
};

#endif // CIR_GATE_H
