/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string first_line, element;
   getline (is, first_line);
   while (getline(is, element))
   {
      int first_double_quote = element.find_first_of ('"', 0);
      if (first_double_quote == -1) break;
      int sec_double_quote = element.find_last_of ('"');
      string key;
      key = element.substr (first_double_quote + 1, sec_double_quote - first_double_quote -1);
      int column_pos = element.find_first_of (':', 0);
      int value_begin = element.find_first_of ("-0123456789", column_pos + 1);
      int value_end = element.find_last_of ("0123456789");
      int value;
      value = stoi (element.substr (value_begin, value_end - value_begin + 1));
      DBJsonElem new_element(key, value);
      j._obj.push_back(new_element);
   }
   j._read_in = true;

   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << '{' << endl;
   if (j.size() != 0)
   {
      for (int i = 0; i < j.size() -1; ++i)
      {
         os << "  " << j[i] << ',' << endl;
      }
      os << "  " << j[j.size()-1] << endl;
   }
   os << '}' << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
   _obj.clear();
   _read_in = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   for (int i = 0; i < size(); ++i)
   {
      if (elm.key() == _obj[i].key())
      {
         return false;
      }
   }
   _obj.push_back(elm);
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   if (size() > 0)
   {
      return float(sum()) / size();
   }
   else
   {
      return NAN;
   }
   //return 0.0;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   idx = size();
   if (size() > 0)
   {
      for (int i = 0; i < size(); ++i)
      {
         if (_obj[i].value() > maxN)
         {
            maxN = _obj[i].value();
            idx = i;
         }
      }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   idx = size();
   if (size() > 0)
   {
      for (int i = 0; i < size(); ++i)
      {
         if (_obj[i].value() < minN)
         {
            minN = _obj[i].value();
            idx = i;
         }
      }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   //int s = 0;
   if (size() > 0)
   {
      int s = _obj[0].value();
      for (int i = 1; i < size(); i++)
      {
         s += _obj[i].value();
      }
      return s;
   }
   else
   {
     return 0;
   }
   //return s;
}
