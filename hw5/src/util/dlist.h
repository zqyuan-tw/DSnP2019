/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = false;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = _node -> _next;
         return *(this);
      }
      iterator operator ++ (int) { 
         iterator it(_node);
         _node = _node -> _next;
         return it; 
      }
      iterator& operator -- () { 
         _node = _node -> _prev;
         return *(this); 
      }
      iterator operator -- (int) {
         iterator it(_node);
         _node = _node -> _prev;
         return it; 
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return *(this); 
      }

      bool operator != (const iterator& i) const {
         if (_node != i._node) return true;
         return false;
      }
      bool operator == (const iterator& i) const {
         if (_node == i._node) return true;
         return false; 
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head; }
   iterator end() const { return _head -> _prev; }
   bool empty() const {
      if (_head == _head -> _prev) return true;
      return false; 
   }
   size_t size() const {  
      size_t count = 0;
      for (iterator it = begin(); it != end(); ++it)
      {
         ++count;
      }
      return count; 
   }

   void push_back(const T& x) {
      DListNode<T>* new_node = new DListNode<T>(x, _head -> _prev -> _prev, _head -> _prev);
      _head -> _prev -> _prev -> _next = new_node;
      _head -> _prev -> _prev = new_node;
      if (size() == 1) _head = new_node;
      _isSorted = false;
   }
   void pop_front() {
      if (!empty())
      {
         _head -> _prev -> _next = _head -> _next;
         _head -> _next -> _prev = _head -> _prev;
         delete _head;
         _head = _head -> _next;
      }
   }
   void pop_back() {
      if (!empty())
      {
         if (size() == 1)
         {
            delete _head;
            _head = _head -> _next;
            _head -> _prev = _head;
            _head -> _next = _head;
         }
         else
         {
            _head -> _prev -> _prev = _head -> _prev -> _prev -> _prev;
            _head -> _prev -> _prev -> _next = _head -> _prev;
            delete _head -> _prev -> _prev;
         }
      }
    }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if (!empty())
      {
         if (pos == begin())
         {
            pop_front();
         }
         else
         {
            delete pos._node;
            pos._node -> _prev -> _next = pos._node -> _next;
            pos._node -> _next -> _prev = pos._node -> _prev;
         }
         return true;
      }
      else
      {
         return false;
      }
   }
   bool erase(const T& x) {
      for (iterator i = begin(); i != end(); ++i)
      {
         if (i._node -> _data == x)
         {
            erase(i);
            return true;
         }
      }
      return false;
   }

   iterator find(const T& x) { 
      for (iterator i = begin(); i != end(); ++i)
      {
         if (i._node -> _data == x)
         {
            return i;
         }
      }
      return end();
   }

   void clear() {
      for (iterator i = begin(); i != end(); ++i)
      {
         pop_front();
      }
   }  // delete all nodes except for the dummy node

   void sort() const {
      if (!empty() && _isSorted == false)
      {
         for (DListNode<T>*  i = _head -> _next; i -> _next != _head; i = i -> _next)
         {
            T key = i -> _data;
            DListNode<T>* j = i -> _prev;
            while (key < j -> _data && j != _head -> _prev)
            {
               j -> _next -> _data = j -> _data;
               j = j -> _prev;
            }
            j -> _next -> _data = key;
         }
         _isSorted = true;
      }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   /*void swap(DListNode<T>* a, DListNode<T>* b)
   {
      T temp = a -> _data;
      a -> _data = b -> _data;
      b -> _data = temp;
   }*/
};

#endif // DLIST_H
