/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <stack>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d,  BSTreeNode<T>* p = NULL, BSTreeNode<T>* l = NULL, BSTreeNode<T>* r = NULL):
      _data(d), _parent(p), _left(l), _right(r) {}

   T  _data;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree() 
   {
      _dum = new BSTreeNode<T>(T());
      _dum -> _parent = _dum -> _left = _dum -> _right = NULL;
      _root = NULL;
      _size = 0;
   }
   ~BSTree() {clear(); delete _dum;}

   class iterator
   {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n = NULL) : _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {}

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ ()
      {
         if (_node -> _right != NULL)
         {
            _node = _node -> _right;
            while (_node -> _left != NULL)
            {
               _node = _node -> _left;
            }
         }
         else
         {
            while (_node == _node -> _parent -> _right)
            {
               _node = _node -> _parent;
            }
            _node = _node -> _parent;
         }
         return *this;
      }
      iterator operator ++ (int)
      {
         iterator it(_node);
         if (_node -> _right != NULL)
         {
            _node = _node -> _right;
            while (_node -> _left != NULL)
            {
               _node = _node -> _left;
            }
         }
         else
         {
            while (_node == _node -> _parent -> _right)
            {
               _node = _node -> _parent;
            }
            _node = _node -> _parent;
         }
         return it;
      }
      iterator& operator -- ()
      {
         if (_node -> _left != NULL)
         {
            _node = _node -> _left;
            while (_node -> _right != NULL)
            {
               _node = _node -> _right;
            }
         }
         else
         {
            while (_node == _node -> _parent -> _left)
            {
               _node = _node -> _parent;
               if (_node -> _parent == NULL) return *this;
            }
            _node = _node -> _parent;
         }
         return *this;
      }
      iterator operator -- (int)
      {
         iterator it(_node);
         if (_node -> _left != NULL)
         {
            _node = _node -> _left;
            while (_node -> _right != NULL)
            {
               _node = _node -> _right;
            }
         }
         else
         {
            while (_node == _node -> _parent -> _left)
            {
               _node = _node -> _parent;
               if (_node -> _parent == NULL) return it;
            }
            _node = _node -> _parent;
         }
         return it;
      }
      iterator& operator = (const iterator& i)
      {
         _node = i._node;
         return *(this); 
      }
      bool operator != (const iterator& i) const
      {
         if (_node != i._node) return true;
         return false;
      }
      bool operator == (const iterator& i) const
      {
         if (_node == i._node) return true;
         return false; 
      }

   private:
      BSTreeNode<T>* _node;
   };

   iterator begin() const 
   {
      //cout << "call begin()" << endl;
      if (!empty())
      {
         BSTreeNode<T>* begin = min(_root);
         return begin;
      }
      else
      {
         return _dum;
      }      
   }
   iterator end() const  //_dum
   {
      return _dum;
   }
   bool empty() const
   {
      if (_size == 0) return true;
      return false;
   }
   size_t size() const 
   {
      return _size; 
   }
   void insert(const T& x)
   {
      if (empty())
      {
         _root = new BSTreeNode<T>(x);
         _root -> _right = _dum;
         _dum -> _parent = _root;
      }
      else
      {
         BSTreeNode<T>* temp = _root;
         BSTreeNode<T>* max_node = max(_root);
         max_node -> _right = NULL;
         while (1)
         {
            if (x < temp -> _data)
            {
               if (temp -> _left == NULL)
               {
                  temp -> _left = new BSTreeNode<T>(x, temp);
                  break;
               }
               else
               {
                  temp = temp -> _left;
               }
            }
            else
            {
               if (temp -> _right == NULL)
               {
                  temp -> _right = new BSTreeNode<T>(x, temp);
                  break;
               }
               else
               {
                  temp = temp -> _right;
               }
            }            
         }
         max_node = max(_root);
         max_node -> _right = _dum;
         _dum -> _parent = max_node;
      }
      ++_size;
   }
   void pop_front()
   {
      erase(begin());
   }
   void pop_back()
   {
      erase(_dum -> _parent);
   }
   bool erase(iterator pos)
   {
      if (!empty())
      {
         BSTreeNode<T>* max_node = max(_root);
         max_node -> _right = NULL;
         if (size() == 1)
         {
            delete _root;
            _root = NULL;
            _dum -> _parent = NULL;
         }
         else
         {
            BSTreeNode<T>* del = NULL;
            BSTreeNode<T>* del_child = NULL;
            if (pos._node -> _left == NULL ||  pos._node -> _right == NULL)
            {
               del = pos._node;
            }
            else //two children
            {
               del = successor(pos._node);
               swap(del, pos._node);
            }
            if (del -> _left != NULL)
            {
               del_child = del -> _left;
            }
            else
            {
               del_child = del -> _right;
            }
            if (del_child != NULL)
            {
               del_child -> _parent = del -> _parent;
            }
            if (del -> _parent == NULL)
            {
               _root = del_child;
            }
            else
            {
               if (del == del -> _parent -> _left)
               {
                  del -> _parent -> _left = del_child;
               }
               else
               {
                  del -> _parent -> _right = del_child;
               }
            }
            delete del;
            max_node = max(_root);
            max_node -> _right = _dum;
            _dum -> _parent = max_node;
         }
         --_size;
         return true;
      }
      else
      {
         return false;
      }      
   }
   bool erase(const T& x)
   {
      iterator it = find(x);
      if (it != end())
      {
         erase(it);
         return true;
      }
      else
      {
         return false;
      }      
   }
   iterator find(const T& x)
   {
      BSTreeNode<T>* temp = _root;
      while (temp != NULL)
      {
         if (x > temp -> _data)
         {
            temp = temp -> _right; 
         }
         else if (x < temp -> _data)
         {
            temp = temp -> _left;
         }
         else
         {
            return temp;
         }
      }
      return end();
   }
   void clear()
   {
      while (_size != 0)
      {
         pop_front();
      }
   }

void sort() {}
void print() const
{
   if (!empty())
   {
      visualize(_root, 0);
   }
}

private:
   BSTreeNode<T>* _root;
   BSTreeNode<T>* _dum;
   size_t _size;
   BSTreeNode<T>* successor(BSTreeNode<T>* b)
   {
      if (b -> _right != NULL && b -> _right != _dum)
      {
         b = min(b -> _right);
         return b;
      }
      else
      {
         return NULL;
      }
   }
   BSTreeNode<T>* max(BSTreeNode<T>* b)
   {
      while (b -> _right != NULL && b -> _right != _dum)
      {
         b = b -> _right;
      }
      return b;
   }
   BSTreeNode<T>* min(BSTreeNode<T>* b) const
   {
      while (b -> _left != NULL)
      {
         b = b -> _left;
      }
      return b;
   }
   void swap(BSTreeNode<T>* a, BSTreeNode<T>* b)
   {
      T temp = a -> _data;
      a -> _data = b -> _data;
      b -> _data = temp;
   }
   void visualize(BSTreeNode<T>* n, int s) const
   {
      for (int i = 0; i < s; ++i)
      {
         cout << " ";
      }
      if (n == NULL || n == _dum)
      {
         cout << "[0]" << endl;
      }
      else
      {
         cout << n -> _data << endl;
         visualize(n -> _left, s + 2);
         visualize(n -> _right, s + 2);
      } 
   }
};

#endif // BST_H
