/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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
   ifstream fin(fileName);
   if (fin.is_open())
   {
      //++lineNo;
      string aag, ms, is, ls, os, as;
      //fin >> aag >> M >> I >> L >> O >> A;
      fin >> aag >> ms >> is >> ls >> os >> as;
      if (aag != "aag") 
      {
         errMsg = aag; 
         return parseError(ILLEGAL_IDENTIFIER);
      }
      if (!myStr2Int(ms, M)) 
      {
         errMsg = "number of variables(" + ms + ")"; 
         return parseError(ILLEGAL_NUM);
      }
      if (!myStr2Int(is, I)) 
      {
         errMsg = "number of PIs(" + is + ")"; 
         return parseError(ILLEGAL_NUM);
      }
      if (!myStr2Int(ls, L)) 
      {
         errMsg = "number of latches(" + ls + ")"; 
         return parseError(ILLEGAL_NUM);
      }
      if (!myStr2Int(os, O)) 
      {
         errMsg = "number of POs(" + os + ")"; 
         return parseError(ILLEGAL_NUM);
      }
      if (!myStr2Int(as, A)) 
      {
         errMsg = "number of AIGs(" + as + ")"; 
         return parseError(ILLEGAL_NUM);
      }
      if (M < I + L + A)
      {
         errMsg = "Number of variables";
         errInt = M;
         return parseError(NUM_TOO_SMALL);
      }
      if (L > 0)
      {
         errMsg = "latches";
         return parseError(ILLEGAL_NUM);
      }
      _gateList = new CirGate[M + O + 1];
      _gateList[0].type = CONST_GATE;
      ++lineNo;
      for (int i = 0; i < I; ++i)
      {
         int in;
         string ins;
         fin >> ins;
         if (!myStr2Int(ins, in))
         {
            errMsg = "PI literal ID(" + ins + ")";
            return parseError(ILLEGAL_NUM);
         }
         errInt = in;
         if (!(in / 2))
         {
            return parseError(REDEF_CONST);
         }
         if (in / 2 > M)
         {
            return parseError(MAX_LIT_ID);
         }
         if (in % 2)
         {
            errMsg = "PI";
            return parseError(CANNOT_INVERTED);
         }
         if (_gateList[in / 2].type != UNDEF_GATE)
         {
            errGate = getGate(in / 2);
            return parseError(REDEF_GATE);
         }
         ++lineNo;
         _gateList[in / 2].type = PI_GATE;
         _gateList[in / 2].line = lineNo;
         _piList.push_back(in / 2);         
      }
      int m = M;
      for (int i = 0; i < O; ++i)
      {
         int out;
         string outs;
         fin >> outs;
         if (!myStr2Int(outs, out))
         {
            errMsg = "PO literal ID(" + outs + ")";
            return parseError(ILLEGAL_NUM);
         }
         errInt = out;
         if (out / 2 > M)
         {
            return parseError(MAX_LIT_ID);
         }
         ++lineNo;
         _gateList[++m].type = PO_GATE;
         _gateList[m].line = lineNo;
         _gateList[m].po_in(out);
         _gateList[_gateList[m].fanin1].fanout.push_back(2 * m + out % 2);         
      }
      for (int i = 0; i < A; ++i)
      {
         int ag, in1, in2;
         string ags, in1s, in2s;
         fin >> ags >> in1s >> in2s;
         if (!myStr2Int(ags, ag))
         {
            errMsg = "AIG literal ID(" + ags + ")";
            return parseError(ILLEGAL_NUM);
         }
         errInt = ag;
         if (!(ag / 2))
         {
            return parseError(REDEF_CONST);
         }
         if (ag / 2 > M)
         {
            return parseError(MAX_LIT_ID);
         }
         if (ag % 2)
         {
            errMsg = "AIG";
            return parseError(CANNOT_INVERTED);
         }
         if (_gateList[ag / 2].type != UNDEF_GATE)
         {
            errGate = getGate(ag / 2);
            return parseError(REDEF_GATE);
         }
         if (!myStr2Int(in1s, in1))
         {
            errMsg = "AIG literal ID(" + in1s + ")";
            return parseError(ILLEGAL_NUM);
         }
         errInt = in1;
         if (in1 / 2 > M)
         {
            return parseError(MAX_LIT_ID);
         }
         if (!myStr2Int(in2s, in2))
         {
            errMsg = "AIG literal ID(" + in2s + ")";
            return parseError(ILLEGAL_NUM);
         }
         errInt = in2;
         if (in2 / 2 > M)
         {
            return parseError(MAX_LIT_ID);
         }
         ++lineNo;
         _gateList[ag / 2].type = AIG_GATE;
         _gateList[ag / 2].line = lineNo;
         _gateList[ag / 2].aig_in(in1, in2);
         _gateList[_gateList[ag / 2].fanin1].fanout.push_back(ag + in1 % 2);
         _gateList[_gateList[ag / 2].fanin2].fanout.push_back(ag + in2 % 2);
      }
      string iloc;
      int pos;
      //string poss;
      while (fin >> iloc)
      {
         ++lineNo;
         if (iloc[0] == 'i')
         {
            //fin >> poss;
            if (!myStr2Int(iloc.substr(1), pos))
            {
               errMsg = "symbol index(" + iloc.substr(1) + ")";
               return parseError(ILLEGAL_NUM);
            }
            if (pos >= I)
            {
               errMsg = "PI index";
               errInt = pos;
               return parseError(NUM_TOO_BIG);
            }
            if (!_gateList[_piList[pos]].symbol.empty())
            {
               errMsg = "i";
               errInt = pos;
               return parseError(REDEF_SYMBOLIC_NAME);
            }
            fin >> _gateList[_piList[pos]].symbol;
         }
         else if (iloc[0] == 'o')
         {
            //fin >> poss;
            if (!myStr2Int(iloc.substr(1), pos))
            {
               errMsg = "symbol index(" + iloc.substr(1) + ")";
               return parseError(ILLEGAL_NUM);
            }
            if (pos >= O)
            {
               errMsg = "PO index";
               errInt = pos;
               return parseError(NUM_TOO_BIG);
            }
            if (!_gateList[M + 1 + pos].symbol.empty())
            {
               errMsg = "o";
               errInt = pos;
               return parseError(REDEF_SYMBOLIC_NAME);
            }
            fin >> _gateList[M + 1 + pos].symbol;
         }
         else if (iloc[0] == 'c')
         {
            if (iloc.size() > 1)
            {
               return parseError(MISSING_NEWLINE);
            }
            string c;
            while (getline(fin, c))
            {
               comment.push_back(c);
            }
            break;
         }
         else
         {
            return parseError(ILLEGAL_SYMBOL_TYPE);
         }         
      }
      fin.close();
      //_read = true;
      ++global_ref;
      dfsList();
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
   cout << "  AIG" << right << setw(11) << A << endl;
   cout << "------------------" << endl;
   cout << "  Total" << right << setw(9) << I + O + A << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for (int i = 0; i < _dfsList.size(); ++i)
   {
      cout << "[" << i << "] " << left << setw(4) << _gateList[_dfsList[i]].getTypeStr() << _dfsList[i];
      if (_gateList[_dfsList[i]].type == PI_GATE || _gateList[_dfsList[i]].type == CONST_GATE)
      {
         if (!_gateList[_dfsList[i]].symbol.empty()) cout << " (" << _gateList[_dfsList[i]].symbol << ")";
         cout << endl;
      }
      else if (_gateList[_dfsList[i]].type == AIG_GATE)
      {
         cout << " ";
         if (_gateList[_dfsList[i]].inv1) cout << "!";
         cout << _gateList[_dfsList[i]].fanin1 << " ";
         if (_gateList[_dfsList[i]].inv2) cout << "!";
         cout << _gateList[_dfsList[i]].fanin2 << endl;
      }
      else if (_gateList[_dfsList[i]].type == PO_GATE)
      {
         cout << " ";
         if (_gateList[_dfsList[i]].inv1) cout << "!";
         cout << _gateList[_dfsList[i]].fanin1;
         if (!_gateList[_dfsList[i]].symbol.empty()) cout << " (" << _gateList[_dfsList[i]].symbol << ")";
         cout << endl;
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(int i = 0; i < _piList.size(); ++i)
   {
      cout << " " << _piList[i];    
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(int i = M + 1; i < M + O + 1; ++i)
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
      if ((_gateList[i].type == PO_GATE && _gateList[_gateList[i].fanin1].type == UNDEF_GATE) 
      || (_gateList[i].type == AIG_GATE && (_gateList[_gateList[i].fanin1].type == UNDEF_GATE || _gateList[_gateList[i].fanin2].type == UNDEF_GATE)))
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
               cout << "Gates defined but not used  :";
               found = 1;
            }
            cout << " " << i;
         }
      }
   }
   if (found) cout << endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
   IdList new_AList;
   for (int i = 0; i < _dfsList.size(); ++i)
   {
      if (_gateList[_dfsList[i]].type == AIG_GATE)
      {
         new_AList.push_back(_dfsList[i]);
      }
   }
   outfile << "aag " << M << " " << I << " " << L << " " << O << " " << new_AList.size() << endl;
   for (int i = 0; i < I; ++i)
   {
      outfile << 2 * _piList[i] << endl;
   }
   for (int i = 0; i < O; ++i)
   {
      if (_gateList[M + 1 + i].inv1) 
      {
         outfile << 2 * _gateList[M + 1 + i].fanin1 + 1 << endl;
      }
      else
      {
         outfile << 2 * _gateList[M + 1 + i].fanin1 << endl;
      }      
   }
   for (int i = 0; i < new_AList.size(); ++i)
   {
      outfile << 2 * new_AList[i] << " ";
      if (_gateList[new_AList[i]].inv1)
      {
         outfile << 2 * _gateList[new_AList[i]].fanin1 + 1 << " ";
      }
      else
      {
         outfile << 2 * _gateList[new_AList[i]].fanin1 << " ";
      }
      if (_gateList[new_AList[i]].inv2)
      {
         outfile << 2 * _gateList[new_AList[i]].fanin2 + 1 << endl;
      }
      else
      {
         outfile << 2 * _gateList[new_AList[i]].fanin2 << endl;
      }
   }
   for (int i = 0; i < I; ++i)
      {
         if (!_gateList[_piList[i]].symbol.empty())
         {
            outfile << "i" << i << " " << _gateList[_piList[i]].symbol << endl;
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


void CirMgr::genDFSList(int a)
{
   _gateList[a].ref = global_ref;
   if (_gateList[a].type == UNDEF_GATE) return;
   if (_gateList[a].type == AIG_GATE)
   {
      if (_gateList[_gateList[a].fanin1].ref != global_ref)
      {
         genDFSList(_gateList[a].fanin1);
      }
      if (_gateList[_gateList[a].fanin2].ref != global_ref)
      {
         genDFSList(_gateList[a].fanin2);
      }
   }
   else if (_gateList[a].type == PO_GATE)
   {
      if (_gateList[_gateList[a].fanin1].ref != global_ref)
      {
         genDFSList(_gateList[a].fanin1);
      }
   }
   _dfsList.push_back(a);
}

void CirMgr::dfsList()
{
   for (int i = M + 1; i < M + O + 1; ++i)
   {
      genDFSList(i);
   }
}