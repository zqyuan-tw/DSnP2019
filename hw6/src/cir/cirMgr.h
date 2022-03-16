/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
  friend class CirGate;
public:
   CirMgr(): _gateList(NULL), global_ref(0){}
   ~CirMgr() 
  {
    if (_gateList)
    {
      delete[] _gateList;
      _gateList = NULL;
    }
  }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const 
   {
     if (_gateList[gid].type == UNDEF_GATE) return NULL;
     return _gateList + gid; 
    }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void genDFSList(int);
   void dfsList();

private:
  int M;
  int I;
  int L;
  int O;
  int A;
  mutable int global_ref;
  CirGate* _gateList;
  IdList _piList;
  //bool _read;
  vector<string> comment;
  IdList _dfsList;
};

#endif // CIR_MGR_H
