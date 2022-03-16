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
#include <algorithm>
#include "cirGate.h"
#include "../util/myHashMap.h"
#include "../util/myHashSet.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
   friend class CirGate;
public:
   CirMgr(): _gateList(NULL), global_ref(0) {}
   ~CirMgr() {
      if (_gateList)
      {
         delete[] _gateList;
         _gateList = NULL;
      }
   } 

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if (_gateList[gid].type == UNDEF_GATE) return NULL;
      return _gateList + gid; 
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   void genDFSList(size_t);
   void DFS();
   void FEC();

   class HashKey
   {
   public:
      HashKey(unsigned g): gateId(g) {}
      ~HashKey() {}

      unsigned operator() () const
      {
         return 65536 * (cirMgr -> getGate(gateId) -> fanin1 + cirMgr -> getGate(gateId) -> fanin2) + cirMgr -> getGate(gateId) -> fanin1 * cirMgr -> getGate(gateId) -> fanin2;
      }
      bool operator== (const HashKey& k) const
      {
         if ((cirMgr -> getGate(gateId) ->  fanin1 == cirMgr -> getGate(k.gateId) ->  fanin1 && cirMgr -> getGate(gateId) ->  fanin2 == cirMgr -> getGate(k.gateId) ->  fanin2) || 
              (cirMgr -> getGate(gateId) ->  fanin1 == cirMgr -> getGate(k.gateId) ->  fanin2 && cirMgr -> getGate(gateId) ->  fanin2 == cirMgr -> getGate(k.gateId) ->  fanin1))
         {
            return true;
         }
         else
         {
            return false;
         }         
      }
      unsigned getId() {return gateId;}
   private:
      unsigned gateId;
   };

private:
   ofstream           *_simLog;
   unsigned M, I, L, O, A;
   mutable int global_ref;
   CirGate* _gateList;
   IdList PIList;
   IdList AIGList;
   IdList DFSList;
   //HashSet<FECkey> fecg;
   vector<IdList> fecg;
   
   bool find(unsigned u, IdList& idl) const
   {
      for (unsigned i = 0; i < idl.size(); ++i)
      {
         if (u == idl[i]) return true;
      }
      return false;
   }

   void existErase(unsigned u, IdList& idl)
   {
      idl.erase(remove(idl.begin(), idl.end(), u), idl.end());
   }

   void simprocess()
   {
      for (unsigned i = 0; i < DFSList.size(); ++i)
      {
         if (_gateList[DFSList[i]].type == PO_GATE) 
         {
            if (_gateList[DFSList[i]].fanin1 % 2) _gateList[DFSList[i]].sim = ~(_gateList[_gateList[DFSList[i]].fanin1 / 2].sim);
            else _gateList[DFSList[i]].sim = _gateList[_gateList[DFSList[i]].fanin1 / 2].sim;
         }
         else if (_gateList[DFSList[i]].type == AIG_GATE)
         {
            size_t fs1, fs2;
            if (_gateList[DFSList[i]].fanin1 % 2) fs1 = ~(_gateList[_gateList[DFSList[i]].fanin1 / 2].sim);
            else fs1 = _gateList[_gateList[DFSList[i]].fanin1 / 2].sim;
            if (_gateList[DFSList[i]].fanin2 % 2) fs2 = ~(_gateList[_gateList[DFSList[i]].fanin2 / 2].sim);
            else fs2 = _gateList[_gateList[DFSList[i]].fanin2 / 2].sim;
            //cout << fs1 << " " << fs2 << endl;
            _gateList[DFSList[i]].sim = (fs1 & fs2);
         }
      }
      FEC();
   }

   void resetSim()
   {
      for (unsigned i = 0; i < DFSList.size(); ++i)
      {
         _gateList[DFSList[i]].sim = 0;
      }
   }

   void checkGate(unsigned g, vector<unsigned>& i, vector<unsigned>& a) const
   {
      _gateList[g].ref = global_ref;
      if (_gateList[g].type == UNDEF_GATE) return;
      if (_gateList[g].type == AIG_GATE)
      {
         a.push_back(g);
         if (_gateList[_gateList[g].fanin1 / 2].ref != global_ref)
         {
            checkGate(_gateList[g].fanin1 / 2, i, a);
         }
         if (_gateList[_gateList[g].fanin2 / 2].ref != global_ref)
         {
            checkGate(_gateList[g].fanin2 / 2, i, a);
         }
      }
      else if (_gateList[g].type == PI_GATE)
      {
         i.push_back(g);
         return;
      }
   }
};

#endif // CIR_MGR_H
