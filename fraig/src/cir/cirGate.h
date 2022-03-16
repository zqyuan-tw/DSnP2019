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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
  friend class CirMgr;
public:
   CirGate(): type(UNDEF_GATE), line(0), fanin1(4294967295), fanin2(4294967295), ref(0), symbol(""), sim(0), buc(4294967295) {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const;
   unsigned getLineNo() const { return line; }
   bool isAig() const {
     if (type == AIG_GATE)
       return true;
     else
       return false;
   }

   // Printing functions
   virtual void printGate() const {}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   
private:
   GateType type;
   unsigned line;
   unsigned fanin1; //*2
   unsigned fanin2; //*2
   IdList fanout; //*2
   mutable unsigned ref;
   string symbol;
   size_t sim;
   unsigned buc;

   void faninDFS(int, int) const;
   void fanoutDFS(int, int) const;
};

#endif // CIR_GATE_H
