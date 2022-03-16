/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include<fstream>
#include <iomanip>

using namespace std;

// Implement member functions of class Row and Table here
bool
Json::read(const string& jsonFile)
{
   ifstream input_file (jsonFile);
   if (input_file.is_open())
   {
      string first_line, element;
      getline (input_file, first_line);
      while (getline(input_file, element))
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
         JsonElem new_element(key, value);
         _obj.push_back(new_element);
      }
      input_file.close();
      return true; // TODO
   }
   else
   {
      return false; // cannot open
   }
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void Json::print()
{
   cout << '{' << endl;
   if (_obj.size() != 0)
   {
      for (int i = 0; i < _obj.size() -1; i++)
      {
         cout << "  " << _obj[i] << ',' << endl;
      }
      cout << "  " << _obj[_obj.size()-1] << endl;
   }
   cout << '}' << endl;
}

void Json::add()
{
   string key_name;
   int value;
   cin >> key_name >> value;
   _obj.push_back(JsonElem(key_name, value));
}

void Json::sum()
{
   if (_obj.size() > 0)
   {
      int sum = _obj[0].get_value();
      for (int i = 1; i < _obj.size(); i++)
      {
         sum += _obj[i].get_value();
      }
      cout << "The summation of the values is: " << sum << '.' << endl;
   }
   else
   {
      cout << "Error: No element found!!" << endl;
   }
}

void Json::ave()
{
   if (_obj.size() > 0)
   {
      int sum = _obj[0].get_value();
      for (int i = 1; i < _obj.size(); i++)
      {
         sum += _obj[i].get_value();
      }
      cout << "The average of the values is: " ;
      cout << fixed << setprecision(1) << sum/float(_obj.size());
      cout << '.' << endl;
   }
   else
   {
      cout << "Error: No element found!!" << endl;
   }
}

void Json::max()
{
   if (_obj.size() > 0)
   {
      JsonElem max_elem = _obj[0];
       for (int i = 1; i < _obj.size(); i++)
      {
         if (_obj[i].get_value() > max_elem.get_value())
         {
            max_elem = _obj[i];
         }
      }
      cout << "The maximum element is: { " <<  max_elem << " }." << endl;
   }
   else
   {
      cout << "Error: No element found!!" << endl;
   }
}

void Json::min()
{
   if (_obj.size() > 0)
   {
      JsonElem min_elem = _obj[0];
       for (int i = 1; i < _obj.size(); i++)
      {
         if (_obj[i].get_value ()< min_elem.get_value())
         {
            min_elem = _obj[i];
         }
      }
      cout << "The minimum element is: { " <<  min_elem << " }." << endl;
   }
   else
   {
      cout << "Error: No element found!!" << endl;
   }
}

/*int Json::exit()
{
   return 0;
}*/