/******************************************************************************* 
 *                                   BSD LICENSE 
 *******************************************************************************
 *  Copyright (c) 2006, CEA, INRIA and Universite Paris Sud
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without modification,  
 *  are permitted provided that the following conditions are met: 
 *  
 *   - Redistributions of source code must retain the above copyright notice, this  
 *     list of conditions and the following disclaimer.  
 *  
 *   - Redistributions in binary form must reproduce the above copyright notice,  
 *     this list of conditions and the following disclaimer in the documentation  
 *     and/or other materials provided with the distribution.  
 *  
 *   - Neither the name of CEA, INRIA and Universite Paris Sud nor the names of its 
 *     contributors may be used to endorse or promote products derived from this 
 *     software without specific prior written permission.  
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 *  DISCLAIMED.  
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY  
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *  
 ****************************************************************************** */ 

/***************************************************************************
                       fsc.h  -  FraternitéSysC header
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#ifndef UNISIM_LIST
#define UNISIM_LIST

/**  
 * @file unisim_list.h  
 * @brief Defiles the List and ListPointer classes
 */


template <class T> class ListPointer;


/**
 * \brief a node of the List class
 */
template <class T>
class ListNode
{public:
  T *data;                ///< Data stored in the node
  ListNode<T> *next;      ///< Pointer to the next node
  ListNode<T> *prev;      ///< Pointer to the previous node
};

template <class T> class List;

template <class T>
List<T>& operator >> (T *data, List<T>& l);

/**
 * \brief Templated list class
 */
template <class T>
class List
{public:
  List();
  List(const List<T>& l);
  ~List();

  List<T>& operator << (T *data);
  friend List<T>& operator >> <> (T *data, List<T>& l);
  ListPointer<T> Begin();
  ListPointer<T> End();
  int Size() const;
  bool Empty();
  ListPointer<T> Search(T *data);
  T *operator [] (int index);
  void Remove(ListPointer<T>& p);
  /**
   * \brief Pretty printer of the List class
   */
  friend ostream& operator << (ostream& os, const List<T>& l)
  { ListNode<T> *node;
    for(node = l.first; node; node = node->next)
    { os << *node->data;
      if(node->next) os << " ";
    }
    os << endl;
    return os;
  }
private:
  int size;                      ///< Size of the List
  ListNode<T> *first, *last;     ///< First and Last node of the list
  friend class ListPointer<T>;
};

/**
 * \brief Iterator on the List class
 */
template <class T>
class ListPointer
{public:
  ListPointer();
  ListPointer(ListNode<T> *node);
  ListPointer(const ListPointer<T>& lp);

  T *operator -> ();
  operator T * ();

  bool operator ++ ();
  bool operator ++ (int);
  bool operator -- ();
  bool operator -- (int);

  bool IsFirst();
  bool IsLast();

  ListPointer<T>& operator = (const ListPointer<T>& lp);
private:
  ListNode<T> *current;  ///< Node corresponding to the pointer
  friend class List<T>;
};

/**
 * \brief Create a new empty List
 */
template <class T>
List<T>::List()
{ first = last = 0;
  size = 0;
}

/**
 * \brief Copy constructor of the List class
 */
template <class T>
List<T>::List(const List<T>& l)
{ ListNode<T> *node = l.first;
  first = last = 0;
  size = 0;
  if(node)
  { do
    { ListNode<T> *copy_node = new ListNode<T>();
      copy_node->data = node->data;
      copy_node->next = 0;
      copy_node->prev = last;
      if(last) last->next = copy_node;
      last = copy_node;
      if(!first) first = last;
      size++;
    } while(node = node->next);
  }
}

/**
 * \brief Destructor of the List class
 */
template <class T>
List<T>::~List<T>()
{ ListNode<T> *node = first;
  if(node)
  { ListNode<T> *next_node;
    do
    { next_node = node->next;
      delete node;
    } while((node = next_node));
  }
}

/**
 * \brief Enqueue on the right a new element to the List
 */
template <class T>
List<T>& List<T>::operator << (T *data)
{ ListNode<T> *node = new ListNode<T>();
  if(!data)
  { cerr << "data is " << data << endl;
    abort();
  }
  node->data = data;
  node->next = 0;
  node->prev = last;
  if(last) last->next = node;
  last = node;
  if(!first) first = last;
  size++;
  return *this;
}

/**
 * \brief Enqueue on the left a new element to the List
 */
template <class T>
List<T>& operator >> (T *data, List<T>& l)
{ ListNode<T> *node = new ListNode<T>();
  if(!data) abort();
  node->data = data;
  node->next = l.first;
  node->prev = 0;
  if(l.first) l.first->prev = node;
  l.first = node;
  if(!l.last) l.last = l.first;
  l.size++;
  return l;
}

/**
 * \brief Returns a pointer to the first element of the List
 */
template <class T>
ListPointer<T> List<T>::Begin()
{ return ListPointer<T>(first);
}

/**
 * \brief Returns a pointer to the last element of the List
 */
template <class T>
ListPointer<T> List<T>::End()
{ return ListPointer<T>(last);
}

/**
 * \brief Returns the size of the List.
 */
template <class T>
int List<T>::Size() const
{ return size;
}

/**
 * \brief Returns true if the List is empty
 */
template <class T>
bool List<T>::Empty()
{ return size == 0;
}

/**
 * \brief Returns the pointer to a matching element in the list
 */
template <class T>
ListPointer<T> List<T>::Search(T *data)
{ ListNode<T> *node = first;
  if(node)
  { do
    { if(node->data == data)
      { return ListPointer<T>(node);
      }
    } while((node = node->next));
  }
  return 0;
}

/**
 * \brief Return the indexed element from the List
 */
template <class T>
T *List<T>::operator [] (int index)
{ ListNode<T> *node = first;
  if(node)
  { do
    { if(index == 0) return node->data;
    } while(node = node->next, --index >= 0);
  }
  return 0;
}

/**
 * \brief Remove the pointed element from the List
 */
template <class T>
void List<T>::Remove(ListPointer<T>& p)
{ if(p.current->next)
  { p.current->next->prev = p.current->prev;
  }
  else
  { // node is the last
    last = last->prev;
  }
  if(p.current->prev)
  { p.current->prev->next = p.current->next;
  }
  else
  { // node is the first
    first = first->next;
  }
  ListNode<T> *next_node = p.current->next;
  p.current->prev = 0;
  p.current->next = 0;
  delete p.current;
  p.current = next_node;
}

/**
 * \brief Create a null ListPointer
 */
template <class T>
ListPointer<T>::ListPointer()
{ current = 0;
}

/**
 * \brief Creates a pointer to an existing node
 */
template <class T>
ListPointer<T>::ListPointer(ListNode<T> *node)
{ current = node;
}

/**
 * \brief copy constructor of the ListPointer class
 */
template <class T>
ListPointer<T>::ListPointer(const ListPointer<T>& lp)
{ current = lp.current;
}

/**
 * \brief copy constructor of the ListPointer class
 */
template <class T>
ListPointer<T>& ListPointer<T>::operator = (const ListPointer<T>& lp)
{ current = lp.current;
  return *this;
}

/**
 * Direct access to the data of a node pointer
 */
template <class T>
T *ListPointer<T>::operator -> ()
{ return current ? current->data : 0;
}

/**
 * Direct access to the data of a node pointer
 */
template <class T>
ListPointer<T>::operator T * ()
{ return current ? current->data : 0;
}

/**
 * \brief Points to the next element of the list
 */
template <class T>
bool ListPointer<T>::operator ++ ()
{ current = current->next;
  return current ? true : false;
}

/**
 * \brief Points to the next element of the list
 */
template <class T>
bool ListPointer<T>::operator ++ (int)
{ current = current->next;
  return current ? true : false;
}

/**
 * \brief Points to the previous element of the list
 */
template <class T>
bool ListPointer<T>::operator -- ()
{ current = current->prev;
  return current ? true : false;
}


/**
 * \brief Points to the previous element of the list
 */
template <class T>
bool ListPointer<T>::operator -- (int)
{ current = current->prev;
  return current ? true : false;
}

/**
 * \brief Returns true if the pointer point to the last element of the list.
 */
template <class T>
bool ListPointer<T>::IsLast()
{ return !current->next;
}

/**
 * \brief Returns true if the pointer point to the first element of the list.
 */
template <class T>
bool ListPointer<T>::IsFirst()
{ return !current->prev;
}

#endif
