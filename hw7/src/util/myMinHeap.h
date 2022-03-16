/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) 
   {
      _data.push_back(d);
      size_t pos = size() - 1;
      while(pos > 0 && d < _data[(pos - 1) / 2])
      {
         _data[pos] = _data[(pos - 1) / 2];
         pos = (pos - 1) / 2;
      }
      _data[pos] = d;
   }
   void delMin() {delData(0); }
   void delData(size_t i) 
   {
      _data[i] = _data.back();
      _data.pop_back();
      Data temp = _data[i];
      while (i > 0 && _data[i] < _data[(i - 1) / 2])
      {
         _data[i] = _data[(i - 1) / 2];
         i = (i - 1) / 2;
         _data[i] = temp;
      }
      while (2 * i + 1 < size())
      {
         if (2 * i + 2 < size())
         {
            if (_data[2 * i + 2] < _data[2 * i + 1])
            {
               if (_data[2 * i + 2] < _data[i])
               {
                  _data[i] = _data[2 * i + 2];
                  i = 2 * i + 2;
                  _data[i] = temp;
                  continue;
               }
               else
               {
                  break;
               }               
            }
         }
         if (_data[2 * i + 1] < _data[i])
         {
            _data[i] = _data[2 * i + 1];
            i = 2 * i + 1;
            _data[i] = temp;
         }
         else
         {
            break;
         }         
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
