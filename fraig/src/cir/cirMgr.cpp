/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{   
   lineNo = 0;
   ifstream fin(fileName);
   if (fin.is_open())
   {
      string aag;
      fin >> aag >> M >> I >> L >> O >> A;
      _gateList = new CirGate[M + O + 1];
      _gateList[0].type = CONST_GATE;
      ++lineNo; //lineNo = 1
      for (unsigned i = 0; i < I; ++i)
      {
         size_t in;
         fin >> in;
         _gateList[in / 2].type = PI_GATE;
         _gateList[in / 2].line = ++lineNo;
         PIList.push_back(in / 2);
      }
      for (unsigned i = 0; i < L; ++i)
      {}
      for (unsigned i = 0; i < O; ++i)
      {
         size_t out;
         fin >> out;
         _gateList[M + 1 + i].type = PO_GATE;
         _gateList[M + 1 + i].line = ++lineNo;
         _gateList[M + 1 + i].fanin1 = out;
         _gateList[out / 2].fanout.push_back(2 * (M + 1 + i) + out % 2);
      }
      for (unsigned i = 0; i < A; ++i)
      {
         size_t ag, in1, in2;
         fin >> ag >> in1 >> in2;
         _gateList[ag / 2].type = AIG_GATE;
         _gateList[ag / 2].line = ++lineNo;
         _gateList[ag / 2].fanin1 = in1;
         _gateList[ag / 2].fanin2 = in2;
         _gateList[in1 / 2].fanout.push_back(ag + in1 % 2);
         _gateList[in2 / 2].fanout.push_back(ag + in2 % 2);
         AIGList.push_back(ag / 2);
      }
      char c;
      while (fin >> c)
      {
         if (c == 'i')
         {
            size_t pos;
            fin >> pos;
            fin >> _gateList[PIList[pos]].symbol;
         }
         else if (c == 'o')
         {
            size_t pos;
            fin >> pos;
            fin >> _gateList[M + 1 + pos].symbol;
         }
         else
         {
            break;
         }         
      }
      fin.close();
      DFS();
      fecg.reserve(M + O);
      return true;
   }
   else
   {
      return false;
   }
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI " << right << setw(11) << I << endl;
   cout << "  PO " << right << setw(11) << O << endl;
   cout << "  AIG" << right << setw(11) << AIGList.size() << endl;
   cout << "------------------" << endl;
   cout << "  Total" << right << setw(9) << I + O + AIGList.size() << endl;
}

void
CirMgr::printNetlist() const
{
/*
   cout << endl;
   for (unsigned i = 0, n = DFSList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      DFSList[i]->printGate();
   }
*/
   cout << endl;
   for (unsigned i = 0; i < DFSList.size(); ++i)
   {
      cout << "[" << i << "] " << left << setw(4) << _gateList[DFSList[i]].getTypeStr() << DFSList[i];
      if (_gateList[DFSList[i]].type == PI_GATE || _gateList[DFSList[i]].type == CONST_GATE)
      {
         if (!_gateList[DFSList[i]].symbol.empty()) cout << " (" << _gateList[DFSList[i]].symbol << ")";
         cout << endl;
      }
      else if (_gateList[DFSList[i]].type == AIG_GATE)
      {
         cout << " ";
         if (_gateList[DFSList[i]].fanin1 % 2) cout << "!";
         cout << _gateList[DFSList[i]].fanin1 / 2 << " ";
         if (_gateList[DFSList[i]].fanin2 % 2) cout << "!";
         cout << _gateList[DFSList[i]].fanin2 / 2 << endl;
      }
      else if (_gateList[DFSList[i]].type == PO_GATE)
      {
         cout << " ";
         if (_gateList[DFSList[i]].fanin1 % 2) cout << "!";
         cout << _gateList[DFSList[i]].fanin1 / 2;
         if (!_gateList[DFSList[i]].symbol.empty()) cout << " (" << _gateList[DFSList[i]].symbol << ")";
         cout << endl;
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned i = 0; i < PIList.size(); ++i)
   {
      cout << " " << PIList[i];
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i = M + 1; i < M + O + 1; ++i)
   {
        cout << " " << i;
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   bool found = 0;
   for(int i = 0; i < M + O + 1; ++i)
   {
      if ((_gateList[i].type == PO_GATE && _gateList[_gateList[i].fanin1 / 2].type == UNDEF_GATE)
      || (_gateList[i].type == AIG_GATE && (_gateList[_gateList[i].fanin1 / 2].type == UNDEF_GATE || _gateList[_gateList[i].fanin2 / 2].type == UNDEF_GATE)))
      {
         if (!found)
         {
            cout << "Gates with floating fanins(s):";
            found =1;
         }
         cout << " " << i;
      }
   }
   if (found) cout << endl;
   found = 0;
   for (int i = 0; i < M + 1; ++i)
   {
      if (_gateList[i].type == PI_GATE || _gateList[i].type == AIG_GATE)
      {
         if (_gateList[i].fanout.empty())
         {
            if (!found)
            {
               cout << "Gates defined but not used   :";
               found = 1;
            }
            cout << " " << i;
         }
      }
   }
   if (found) cout << endl;
}

void
CirMgr::printFECPairs() const
{
   vector<IdList> pfec = fecg;
   sort(pfec.begin(), pfec.end(), [](const vector<unsigned>& v1, const vector<unsigned>& v2) {return v1[0] < v2[0];});
   for (unsigned i = 0; i < pfec.size(); ++i)
   {      
      if (pfec[i].size() == 1)
      {
         pfec.erase(pfec.begin() + i);
         --i;
         continue;
      }
      cout << "[" << i << "] ";
      for (unsigned j = 0; j < pfec[i].size(); ++j)
      {
         if (_gateList[pfec[i][j]].sim != _gateList[pfec[i][0]].sim) cout << "!";
         cout << pfec[i][j] << " ";
      }
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile << "aag " << M << " " << I << " " << L << " " << O << " " << AIGList.size() << endl;
   for (unsigned i = 0; i < I; ++i)
   {
      outfile << 2 * PIList[i] << endl;
   }
   for (unsigned i = 0; i < O; ++i)
   {
      outfile << _gateList[M + 1 + i].fanin1 << endl;    
   }
   for (int i = 0; i < AIGList.size(); ++i)
   {
      outfile << 2 * AIGList[i] << " " << _gateList[AIGList[i]].fanin1 << " " << _gateList[AIGList[i]].fanin2 << endl;
   }
   for (int i = 0; i < I; ++i)
   {
      if (!_gateList[PIList[i]].symbol.empty())
      {
         outfile << "i" << i << " " << _gateList[PIList[i]].symbol << endl;
      }
   }
   for (int i = 0; i < O; ++i)
   {
      if (!_gateList[M + 1 + i].symbol.empty())
      {
         outfile << "o" << i << " " << _gateList[M + 1 + i].symbol << endl;
      }
   }

   outfile << "c" << endl;
   outfile << "AAG output by Zhao-Qian Yuan" << endl;
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   //outfile << "aag " << g - getGate(0);
   if (g -> type != AIG_GATE)
   {
      cerr << "Error: Gate(" << g - getGate(0) << ") is NOT an AIG!!" << endl;
      return;
   }
   ++global_ref;
   vector<unsigned> i, a;
   checkGate(g - getGate(0), i, a);
   outfile << "aag " << g - getGate(0) << " " << i.size() << " 0 1 " << a.size() << endl;
   for (unsigned m = 0; m < i.size(); ++m)
   {
      outfile << 2 * i[m] << endl;
   }
   outfile << 2 * (g - getGate(0)) << endl;
   for (unsigned m = 0; m < a.size(); ++m)
   {
      outfile << 2 * a[m] << " " << _gateList[a[m]].fanin1 << " " << _gateList[a[m]].fanin2 << endl;
   }
   for (unsigned m = 0; m < i.size(); ++m)
   {
      if (!_gateList[i[m]].symbol.empty())
      {
         outfile << "i" << m << " " << _gateList[i[m]].symbol << endl;
      }
   }
   outfile << "o0 " << g - getGate(0) << endl;
   outfile << "c" << endl;
   outfile << "Write gate (" << g - getGate(0) << ") by Zhao-Qian Yuan" << endl;
}



void CirMgr::genDFSList(size_t a)
{
   _gateList[a].ref = global_ref;
   if (_gateList[a].type == UNDEF_GATE) return;
   if (_gateList[a].type == AIG_GATE)
   {
      if (_gateList[_gateList[a].fanin1 / 2].ref != global_ref)
      {
         genDFSList(_gateList[a].fanin1 / 2);
      }
      if (_gateList[_gateList[a].fanin2 / 2].ref != global_ref)
      {
         genDFSList(_gateList[a].fanin2 / 2);
      }
   }
   else if (_gateList[a].type == PO_GATE)
   {
      if (_gateList[_gateList[a].fanin1 / 2].ref != global_ref)
      {
         genDFSList(_gateList[a].fanin1 / 2);
      }
   }
   DFSList.push_back(a);
}

void CirMgr::DFS()
{
   DFSList.clear();
   ++global_ref;
   for (unsigned i = M + 1; i < M + O + 1; ++i)
   {
      genDFSList(i);
   }
}

void CirMgr::FEC()
{
   if (fecg.empty())
   {   
      //cout << "hi" << endl;
      for (unsigned i = 0; i < DFSList.size(); ++i)
      {
         if (_gateList[DFSList[i]].type != AIG_GATE) continue;
         unsigned j = 0;
         for (; j < fecg.size(); ++j)
         {
            if ((_gateList[fecg[j][0]].sim == _gateList[DFSList[i]].sim) || (_gateList[fecg[j][0]].sim == ~(_gateList[DFSList[i]].sim)))
            {
               //cout << fecg[j][0] << " " << _gateList[fecg[j][0]].sim << " " << DFSList[i] << " " << _gateList[DFSList[i]].sim << endl;
               fecg[j].push_back(DFSList[i]);
               _gateList[DFSList[i]].buc = j;
               break;
            }
         }
         if (j == fecg.size())
         {
            _gateList[DFSList[i]].buc = j;
            IdList id;
            id.push_back(DFSList[i]);
            fecg.push_back(id);
         }
      }
   }
   else
   {
      unsigned n = fecg.size();
      for (unsigned i = 0; i < n; ++i)
      {
         vector<vector<unsigned> > temp;
         for (unsigned j = 1; j < fecg[i].size(); ++j)
         {
            if (_gateList[fecg[i][j]].sim != _gateList[fecg[i][0]].sim && _gateList[fecg[i][j]].sim != ~(_gateList[fecg[i][0]].sim))
            {
               //cout << "hi" << endl;
               unsigned k = 0;
               for (; k < temp.size(); ++k)
               {
                  if (_gateList[fecg[i][j]].sim == _gateList[temp[k][0]].sim || _gateList[fecg[i][j]].sim == ~(_gateList[temp[k][0]].sim))
                  {
                     _gateList[fecg[i][j]].buc = fecg.size() + k;
                     temp[k].push_back(fecg[i][j]);
                     fecg[i].erase(fecg[i].begin() + j);
                     --j;
                     break;
                  }
               }
               if (k == temp.size())
               {
                  _gateList[fecg[i][j]].buc = fecg.size() + k;
                  IdList m;
                  m.push_back(fecg[i][j]);
                  temp.push_back(m);
                  fecg[i].erase(fecg[i].begin() + j);
                  --j;
               }
            }
         }
         if (!temp.empty())
         {
            fecg.insert(fecg.end(), temp.begin(), temp.end());
         }
      }
   }
   
   for (unsigned i = 0; i < fecg.size(); ++i)
   {
      sort(fecg[i].begin(), fecg[i].end());
   }
}
