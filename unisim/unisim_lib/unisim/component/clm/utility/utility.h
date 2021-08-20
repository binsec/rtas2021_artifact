/***************************************************************************
   utility.h  -  Defines base architecture classes (Queue, Pipeline, ...)
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
Foundings: Partly founded with HiPEAC foundings
All rights reserved.  
  
Redistribution and use in source and binary forms, with or without modification,   
are permitted provided that the following conditions are met:  
  
 - Redistributions of source code must retain the above copyright notice, this   
   list of conditions and the following disclaimer.   
  
 - Redistributions in binary form must reproduce the above copyright notice,   
   this list of conditions and the following disclaimer in the documentation   
   and/or other materials provided with the distribution.   
   
 - Neither the name of the UNISIM nor the names of its contributors may be   
   used to endorse or promote products derived from this software without   
   specific prior written permission.   
     
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE   
DISCLAIMED.   
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,   
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,   
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY   
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING   
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,   
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
***************************************************************************** */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>

#if defined(__GNUC__) && defined(EXTENSIVE_INLINING)
#define GCC_INLINE __attribute__((always_inline))
#define INLINE inline
#else
#define GCC_INLINE
#define INLINE inline
#endif

/*
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
*/
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


//#define DD_DEBUG_OoOQUEUE

/**
 * \brief Embbed pointers in a class to have a smart garbage collector
 */
template <class T>
class Pointer
{public:
  /**
   * \brief Creates a new NULL Pointer
   */
  Pointer()
  { data = 0;
  }

  /**
   * \brief Creates a new Pointer to an existing data
   */
  Pointer(T& data)
  { this->data = &data;
  }

  /**
   * \brief return the C pointer corresponding to the Pointer (a pointer to the data)
   */
  T *operator * () const
  { return data;
  }

  /**
   * \brief return the C pointer corresponding to the Pointer (a pointer to the data)
   */
  T *operator -> () const
  { return data;
  }

 private:
  T *data;
};

/**
 * \brief Node of an out-of-order queue
 */
template <class T>
class OoOQueueNode
{public:
  /**
   * \brief Creates a new node
   */
  OoOQueueNode();

  T data;                  ///< The node data
  OoOQueueNode<T> *prev;   ///< Pointer to the previous node in the queue
  OoOQueueNode<T> *next;   ///< Pointer to the next node in the queue
};

/**
 * \brief Creates a new OoOQueueNode
 */
template <class T>
OoOQueueNode<T>::OoOQueueNode()
{ prev = next = 0;
}

template <class T, int MaxSize>
class OoOQueuePointer;
template <class T, int MaxSize>
class OoOQueue;

/**
 * \brief Out-of-order queue
 */
template <class T, int MaxSize>
class OoOQueue
{public:
  OoOQueue();
  ~OoOQueue();
  OoOQueue<T, MaxSize>& operator << (const T& v);
  OoOQueue<T, MaxSize>& operator >> (T& v);
  bool Empty() GCC_INLINE;
  bool Full() GCC_INLINE;
  OoOQueuePointer<T, MaxSize> SeekAtHead() const GCC_INLINE;
  OoOQueuePointer<T, MaxSize> SeekAtTail() const GCC_INLINE;
  int Size() GCC_INLINE;
  int FreeSpace() GCC_INLINE;
  T *GetTail() GCC_INLINE;
  T *GetHead() GCC_INLINE;
  void RemoveHead() GCC_INLINE;
  void RemoveHead(int n);
  void RemoveTail();
  T *NewAfter(const OoOQueuePointer<T, MaxSize>& p);
  T *New() GCC_INLINE;
  void Flush();
  void Reset();
  void FlushAfter(const OoOQueuePointer<T, MaxSize>& p);
  void Remove(OoOQueuePointer<T, MaxSize>& p) GCC_INLINE;
  bool Check();
  int GetIdent(OoOQueuePointer<T, MaxSize>& p);
  int GetTailIdent();
  OoOQueuePointer<T, MaxSize> Retrieve(int ident);

  /**
   * \brief Pretty printer of the class
   */
  friend ostream& operator << /*<T, MaxSize>*/(ostream& os, const OoOQueue& queue)
  {
#ifdef DD_DEBUG_OoOQUEUE
	  cerr << "[DD_DEBUG_OoOQUEUE] <Debug Queue> Head : " << queue.head << endl;
	  cerr << "[DD_DEBUG_OoOQUEUE] <Debug Queue> Tail : " << queue.tail << endl;
	  {
	    OoOQueueNode<T> *node = queue.tail;
	    while(node)
	      {
		os << "[DD_DEBUG_OoOQUEUE] <Debug Queue> node->prev : " << "(" << node->prev << ")" << endl;
		os << "[DD_DEBUG_OoOQUEUE] <Debug Queue> node->next : " << "(" << node->next << ")" << endl;

		node = node->prev;
	      }
	  }
#endif

    OoOQueueNode<T> *node = queue.tail;
    while(node)
    {
      os << "(" << node->data << ")" << endl;
      node = node->prev;
    }
    return os;
  }


private:
  OoOQueueNode<T> *Allocate() GCC_INLINE;
  OoOQueueNode<T> *AllocateAfter(const OoOQueuePointer<T, MaxSize>& p);
  void Free(OoOQueueNode<T> *node) GCC_INLINE;

  int size;
  OoOQueueNode<T> *tail;
  OoOQueueNode<T> *head;
  OoOQueueNode<T> *freeNode;
  OoOQueueNode<T> nodes[MaxSize];
  friend class OoOQueuePointer<T, MaxSize>;
};

template <class T, int MaxSize>
class OoOQueuePointer
{public:
  OoOQueuePointer() GCC_INLINE;
  OoOQueuePointer(OoOQueueNode<T> *node) GCC_INLINE;

  operator T * () GCC_INLINE;
  operator bool () GCC_INLINE;

  T * operator -> () GCC_INLINE;

  bool operator ++ () GCC_INLINE;
  bool operator ++ (int) GCC_INLINE;
  bool operator -- () GCC_INLINE;
  bool operator -- (int) GCC_INLINE;
private:

  OoOQueueNode<T> *current;
  friend class OoOQueue<T, MaxSize>;
};

template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize>::OoOQueuePointer()
{}

template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize>::OoOQueuePointer(OoOQueueNode<T> *node)
{ current = node;
}

template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize>::operator bool ()
{ return current;
}

template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize>::operator T * ()
{ return current ? &current->data : 0;
}

template <class T, int MaxSize>
INLINE bool OoOQueuePointer<T, MaxSize>::operator ++ ()
{ if(current) current = current->next;
  return current;
}

template <class T, int MaxSize>
INLINE bool OoOQueuePointer<T, MaxSize>::operator ++ (int)
{ if(current) current = current->next;
  return current;
}

template <class T, int MaxSize>
INLINE bool OoOQueuePointer<T, MaxSize>::operator -- ()
{ if(current) current = current->prev;
  return current;
}

template <class T, int MaxSize>
INLINE bool OoOQueuePointer<T, MaxSize>::operator -- (int)
{ if(current) current = current->prev;
  return current;
}

template <class T, int MaxSize>
INLINE T *OoOQueuePointer<T, MaxSize>::operator -> ()
{ return current ? &current->data : 0;
}

#ifdef DEBUG_OOOQUEUE
#define OOOQUEUE_PRE { if(!Check()) cerr << "pre assertion failed in " << __FUNCTION__ << endl; }
#define OOOQUEUE_POST { if(!Check()) cerr << "post assertion failed in " << __FUNCTION__ << endl; }
#else
#define OOOQUEUE_PRE
#define OOOQUEUE_POST
#endif

template <class T, int MaxSize>
INLINE OoOQueue<T, MaxSize>::OoOQueue()
{ int i;

  freeNode = &nodes[0];
  for(i = 0; i < MaxSize - 1; i++)
  {
    nodes[i].prev = 0;
    nodes[i].next = &nodes[i + 1];
  }
  tail = head = 0;
  size = 0;
  OOOQUEUE_POST;
}

/**
 * \brief Destructor of the OoOQueue class
 */
template <class T, int MaxSize>
OoOQueue<T, MaxSize>::~OoOQueue()
{
  Reset();
}

/**
 * \brief Add a new item on the left on the queue
 */
template <class T, int MaxSize>
INLINE OoOQueue<T, MaxSize>& OoOQueue<T, MaxSize>::operator << (const T& v)
{
  OOOQUEUE_PRE;
  OoOQueueNode<T> *node = Allocate();

  if(node)
    node->data = v;

  OOOQUEUE_POST;
  return *this;
}

template <class T, int MaxSize>
INLINE T *OoOQueue<T, MaxSize>::NewAfter(const OoOQueuePointer<T, MaxSize>& p)
{
  OOOQUEUE_PRE;
  OoOQueueNode<T> *node = AllocateAfter(p);
  OOOQUEUE_POST;
  return node ? &node->data : 0;
}


/**
 * \brief Add a new item on the right on the queue
 */
template <class T, int MaxSize>
INLINE OoOQueue<T, MaxSize>& OoOQueue<T, MaxSize>::operator >> (T& v)
{
  OOOQUEUE_PRE;
  if(head)
  {
    v = head->data;
    Free(head);
  }
  OOOQUEUE_POST;

  return *this;
}

/**
 * \brief Returns true if the queue is empty
 */
template <class T, int MaxSize>
INLINE bool OoOQueue<T, MaxSize>::Empty()
{ return !head;
}

/**
 * \brief Returns true if the queue is full
 */
template <class T, int MaxSize>
INLINE bool OoOQueue<T, MaxSize>::Full()
{ return !freeNode;
}

template <class T, int MaxSize>
INLINE OoOQueueNode<T> *OoOQueue<T, MaxSize>::Allocate()
{ if(freeNode)
  { OOOQUEUE_PRE;
    OoOQueueNode<T> *node = freeNode;
    freeNode = freeNode->next;
    node->prev = tail;
    node->next = 0;
    if(tail)
      tail->next = node;
    tail = node;
    if(!head)
      head = tail;
    size++;
    OOOQUEUE_POST;
    return node;
  }
  return 0;
}

template <class T, int MaxSize>
INLINE OoOQueueNode<T> *OoOQueue<T, MaxSize>::AllocateAfter(const OoOQueuePointer<T, MaxSize>& p)
{ if(freeNode)
  { OOOQUEUE_PRE;
    OoOQueueNode<T> *node = freeNode;
    freeNode = freeNode->next;
    node->prev = p.current;
    node->next = p.current->next;
    if(p.current->next)
      p.current->next->prev = node;
    if(p.current == tail)
      tail = node;
    //if(p.current == head)
    //	head = node;
    p.current->next = node;
    size++;
    OOOQUEUE_POST;
    return node;
  }
  return 0;
}

/**
 * \brief Free a node of the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::Free(OoOQueueNode<T> *node)
{
  OOOQUEUE_PRE;
  if(node->next)
  {
    node->next->prev = node->prev;
  }
  else
  {
    // node is the tail
    tail = tail->prev;
  }
  if(node->prev)
  {
    node->prev->next = node->next;
  }
  else
  {
    // node is the head
    head = head->next;
  }
  node->next = freeNode;
  freeNode = node;
  size--;
  OOOQUEUE_POST;
}

/**
 * \brief Returns a pointer to the first item of the queue
 */
template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize> OoOQueue<T, MaxSize>::SeekAtHead() const
{
  OOOQUEUE_PRE;
  OoOQueuePointer<T, MaxSize> p(head);
  OOOQUEUE_POST;
  //return OoOQueuePointer<T, MaxSize>(head);
  return p;
}

/**
 * \brief Returns a pointer to the last item of the queue
 */
template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize> OoOQueue<T, MaxSize>::SeekAtTail() const
{
  OOOQUEUE_PRE;
  OoOQueuePointer<T, MaxSize> p(tail);
  OOOQUEUE_POST;
  //return OoOQueuePointer<T, MaxSize>(tail);
  return p;
}

/**
 * \brief Returns the remaining space in the queue
 */
template <class T, int MaxSize>
INLINE int OoOQueue<T, MaxSize>::FreeSpace()
{
  return MaxSize - size;
}

/**
 * \brief Returns the size of the queue
 */
template <class T, int MaxSize>
INLINE int OoOQueue<T, MaxSize>::Size()
{
  return size;
}

/**
 * \brief Remove the first item of the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::RemoveHead()
{
  OOOQUEUE_PRE;
  if(head)
    Free(head);

  OOOQUEUE_POST;
}

/**
 * \brief Remove the n first items of the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::RemoveHead(int n)
{
  OOOQUEUE_PRE;
  if(n > 0)
  {
    do
    {
      if(head)
        Free(head);
    }
    while(--n);
  }
  OOOQUEUE_POST;
}

/**
 * \brief Remove the tail of the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::RemoveTail()
{
  OOOQUEUE_PRE;
  if(tail)
    Free(tail);
  OOOQUEUE_POST;
}

/**
 * \brief Returns the first element of the queue
 */
template <class T, int MaxSize>
INLINE T *OoOQueue<T, MaxSize>::GetHead()
{
  return head ? &head->data : 0;
}

/**
 * \brief Returns the tail of the queue
 */
template <class T, int MaxSize>
INLINE T *OoOQueue<T, MaxSize>::GetTail()
{
  return tail ? &tail->data : 0;
}

/**
 * \brief Allocates and returns an element in the queue
 */
template <class T, int MaxSize>
INLINE T *OoOQueue<T, MaxSize>::New()
{
  OOOQUEUE_PRE;
  OoOQueueNode<T> *node = Allocate();
  OOOQUEUE_POST;
  return node ? &node->data : 0;
}

/**
 * \brief Flush the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::Flush()
{
  OOOQUEUE_PRE;
  while(head)
    Free(head);
  OOOQUEUE_POST;
}

/**
 * \brief Resets the queue, emptying the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::Reset()
{
  int i;
  freeNode = &nodes[0];
  for(i = 0; i < MaxSize - 1; i++)
  {
    nodes[i].prev = 0;
    nodes[i].next = &nodes[i + 1];
  }
  tail = head = 0;
  size = 0;
}

/**
 * \brief Flush the queue after an element
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::FlushAfter(const OoOQueuePointer<T, MaxSize>& p)
{
  OOOQUEUE_PRE;
  if(tail != p.current)
  {
    do
    {
      if(tail)
        Free(tail);
    }
    while(tail != p.current);
  }
  OOOQUEUE_POST;
}

/**
 * \brief Remove an element from the queue
 */
template <class T, int MaxSize>
INLINE void OoOQueue<T, MaxSize>::Remove(OoOQueuePointer<T, MaxSize>& p)
{
  OOOQUEUE_PRE;
  if(p.current)
  {
    OoOQueueNode<T> *node = p.current;
    p++;
    Free(node);
  }
  OOOQUEUE_POST;
}

/**
 * \brief Perform an integrity check on the queue
 */
template <class T, int MaxSize>
INLINE bool OoOQueue<T, MaxSize>::Check()
{
  /*
  OoOQueueNode<T> *node1 = head;
  while(node1)
  {
  	OoOQueueNode<T> *node2 = head;
  	while(node2)
  	{
  		if(node1 != node2 && node1->data == node2->data)
  		{
  			cerr << "integrity check of queue failed" << endl;
  			return false;
  		}
  		node2 = node2->next;
  	}
  	node1 = node1->next;
  }
  */

  OoOQueueNode<T> *node = head;
  if(node && node->prev)
  { cerr << "head->prev !NULL" << endl;
    return false;
  }
  int sz = size;
  while(node && sz)
  {
    node = node->next;
    sz--;
  }
  if(node)
  { cerr << "integrity check of queue failed" << endl;
    return false;
  }
  node = tail;
  if(node && node->next)
  { cerr << "tail->next !NULL" << endl;
    return false;
  }
  sz = size;
  while(node && sz)
  {
    node = node->prev;
    sz--;
  }
  if(node)
  { cerr << "integrity check of queue failed" << endl;
    return false;
  }
  return true;
}

/**
 * \brief Get the index of a pointed element
 */
template <class T, int MaxSize>
INLINE int OoOQueue<T, MaxSize>::GetIdent(OoOQueuePointer<T, MaxSize>& p)
{
  return p.current - nodes;
}

/**
 * \brief Returns an identifier to the last element of the queue
 */
template <class T, int MaxSize>
INLINE int OoOQueue<T, MaxSize>::GetTailIdent()
{
  return tail - nodes;
}

/**
 * \brief Return an indexed element of the queue
 */
template <class T, int MaxSize>
INLINE OoOQueuePointer<T, MaxSize> OoOQueue<T, MaxSize>::Retrieve(int ident)
{
  return nodes + ident;
}


template <class T, int MaxSize>
class QueuePointer;
template <class T, int MaxSize>
class Queue;

/**
 * \brief Generic Queue class
 */
template <class T, int MaxSize>
class Queue
{
public:
  Queue();
  ~Queue();
  Queue<T, MaxSize>& operator << (const T& v);
  Queue<T, MaxSize>& operator >> (T& v);
  bool Empty() GCC_INLINE;
  bool Full() GCC_INLINE;
  QueuePointer<T, MaxSize> SeekAtHead() GCC_INLINE;
  QueuePointer<T, MaxSize> SeekAtTail() GCC_INLINE;
  int Size() GCC_INLINE;
  int GetMaxSize();
  int FreeSpace() GCC_INLINE;
  T *GetHead() GCC_INLINE;
  T *GetTail() GCC_INLINE;
  void RemoveHead() GCC_INLINE;
  void RemoveHead(int n);
  void RemoveTail() GCC_INLINE;
  T& operator [] (int index);
  T& operator () (int index);
  T *New() GCC_INLINE;
  void Flush();
  void Reset();
  void FlushAfter(const QueuePointer<T, MaxSize>& qp);
  int GetIndex(int index);
  bool Check();

  /**
   * \brief Pretty pinter of the queue class
   */
  friend ostream& operator << /*<T, MaxSize>*/(ostream& os, const Queue& queue)
  { int i, j;
    os << "queue size is " << queue.size << endl;
    for(i = 0, j = queue.tail; i < queue.size; i++, j = (j + MaxSize - 1) % MaxSize)
    { os << "(" << queue.storage[j] << ")" << endl;
    }
    return os;
  }


private:
  int size;
  int tail;
  int head;
  T storage[MaxSize];
  friend class QueuePointer<T, MaxSize>;
};

template <class T, int MaxSize>
class QueuePointer
{
public:
  QueuePointer() GCC_INLINE;
  QueuePointer(void *) GCC_INLINE;
  QueuePointer(Queue<T, MaxSize> *queue, int index) GCC_INLINE;
  //QueuePointer(const QueuePointer<T, MaxSize>& p) GCC_INLINE;

  operator T * () GCC_INLINE;
  operator bool () GCC_INLINE;

  T * operator -> () GCC_INLINE;

  bool operator ++ () GCC_INLINE;
  bool operator ++ (int) GCC_INLINE;
  bool operator -- () GCC_INLINE;
  bool operator -- (int) GCC_INLINE;

  //QueuePointer<T, MaxSize>& operator = (const QueuePointer<T, MaxSize>& p) GCC_INLINE;
private:
  int current;
  Queue<T, MaxSize> *queue;
  friend class Queue<T, MaxSize>;
};

/**
 * \brief Creates a NULL new QueuePointer
 */
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>::QueuePointer(void *)
{
  queue = 0;
  current = -1;
}

/**
 * \brief Creates a NULL new QueuePointer
 */
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>::QueuePointer()
{
  queue = 0;
  current = -1;
}

/**
 * \brief Creates a QueuePointer to a queue item
 */
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>::QueuePointer(Queue<T, MaxSize> *queue, int index)
{
  this->queue = queue;
  current = index;
}

/*
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>::QueuePointer(const QueuePointer<T, MaxSize>& p)
{
	queue = p.queue;
    current = p.current;
}
*/


template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>::operator bool ()
{
  return current >= 0;
}

template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>::operator T * ()
{
  return current >= 0 ? &queue->storage[current] : 0;
}

template <class T, int MaxSize>
INLINE bool QueuePointer<T, MaxSize>::operator ++ ()
{
  if(current >= 0)
  {
    if(current == queue->tail)
    {
      current = -1;
      return false;
    }
    else
    {
      current = (current + 1) % MaxSize;
      return true;
    }
  }
  return false;
}

template <class T, int MaxSize>
INLINE bool QueuePointer<T, MaxSize>::operator ++ (int)
{
  if(current >= 0)
  {
    if(current == queue->tail)
    {
      current = -1;
      return false;
    }
    else
    {
      current = (current + 1) % MaxSize;
      return true;
    }
  }
  return false;
}

template <class T, int MaxSize>
INLINE bool QueuePointer<T, MaxSize>::operator -- ()
{
  if(current >= 0)
  {
    if(current == queue->head)
    {
      current = -1;
      return false;
    }
    else
    {
      current = (current + MaxSize - 1) % MaxSize;
      return true;
    }
  }
  return false;
}

template <class T, int MaxSize>
INLINE bool QueuePointer<T, MaxSize>::operator -- (int)
{
  if(current >= 0)
  {
    if(current == queue->head)
    {
      current = -1;
      return false;
    }
    else
    {
      current = (current + MaxSize - 1) % MaxSize;
      return true;
    }
  }
  return false;
}

template <class T, int MaxSize>
INLINE T *QueuePointer<T, MaxSize>::operator -> ()
{
  return current >= 0 ? &queue->storage[current] : 0;
}

/*
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize>& QueuePointer<T, MaxSize>::operator = (const QueuePointer<T, MaxSize>& p)
{
	queue = p.queue;
	current = p.current;
	return *this;
}
*/


/**
 * \brief Creates a new empty queue
 */
template <class T, int MaxSize>
INLINE Queue<T, MaxSize>::Queue()
{ size = 0;
  tail = MaxSize - 1;
  head = 0;
}

/**
 * \brief Destructor of the Queue class
 */
template <class T, int MaxSize>
INLINE Queue<T, MaxSize>::~Queue()
{ Reset();
}

/**
 * \brief Enqueue an item to the right
 */
template <class T, int MaxSize>
INLINE Queue<T, MaxSize>& Queue<T, MaxSize>::operator << (const T& v)
{
#ifdef SAFE_MODE
  if(size >= MaxSize)
  { cerr << "Error: queue overflow" << endl;
    exit(-1);
  }
#endif
  tail = (tail + 1) % MaxSize;
  storage[tail] = v;
  size++;
  return *this;
}

/**
 * \brief Enqueue an item to the left
 */
template <class T, int MaxSize>
INLINE Queue<T, MaxSize>& Queue<T, MaxSize>::operator >> (T& v)
{
#ifdef SAFE_MODE
  if(size <= 0)
  {
    cerr << "Error: queue underflow" << endl;
    exit(-1);
  }
#endif
  v = storage[head];
  head = (head + 1) % MaxSize;
  size--;
  return *this;
}

/**
 * \brief Returns true if the queue is empty
 */
template <class T, int MaxSize>
INLINE bool Queue<T, MaxSize>::Empty()
{
  return size == 0;
}

/**
 * \brief Returns true if the queue is full
 */
template <class T, int MaxSize>
INLINE bool Queue<T, MaxSize>::Full()
{
  return size == MaxSize;
}

/**
 * \brief Return a pointer to the head of the queue
 */
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize> Queue<T, MaxSize>::SeekAtHead()
{
  return QueuePointer<T, MaxSize>(this, size > 0 ? head : -1);
}

/**
 * \brief Return a pointer to the tail of the queue
 */
template <class T, int MaxSize>
INLINE QueuePointer<T, MaxSize> Queue<T, MaxSize>::SeekAtTail()
{
  return QueuePointer<T, MaxSize>(this, size > 0 ? tail : -1);
}

/**
 * \brief Return the size of the queue
 */
template <class T, int MaxSize>
INLINE int Queue<T, MaxSize>::Size()
{
  return size;
}

/**
 * \brief Return the maximum size of the queue
 */
template <class T, int MaxSize>
INLINE int Queue<T, MaxSize>::GetMaxSize()
{
  return MaxSize;
}

/**
 * \brief Return the remaining space in the queue
 */
template <class T, int MaxSize>
INLINE int Queue<T, MaxSize>::FreeSpace()
{
  return MaxSize - size;
}

/**
 * \brief Returns an item of the queue
 */
template <class T, int MaxSize>
INLINE T& Queue<T, MaxSize>::operator [] (int index)
{
#ifdef SAFE_MODE
  if(index >= size)
  {
    cerr << "Error: queue subscript out of range" << endl;
    exit(-1);
  }
#endif
  return storage[(head + index) % MaxSize];
}

/**
 * \brief Returns an item of the queue
 */
template <class T, int MaxSize>
INLINE T& Queue<T, MaxSize>::operator () (int index)
{
#ifdef SAFE_MODE
  if(index < 0 || index >= MaxSize)
  {
    cerr << "Error: queue subscript out of range" << endl;
    exit(-1);
  }
#endif
  return storage[index];
}

/**
 * \brief Remove the head of the queue
 */
template <class T, int MaxSize>
INLINE void Queue<T, MaxSize>::RemoveHead()
{
#ifdef SAFE_MODE
  if(size <= 0)
  {
    cerr << "Error: queue underflow" << endl;
    exit(-1);
  }
#endif
  head = (head + 1) % MaxSize;
  size--;
}

/**
 * \brief Remove the n first items of the queue
 */
template <class T, int MaxSize>
INLINE void Queue<T, MaxSize>::RemoveHead(int n)
{
#ifdef SAFE_MODE
  if(size < n)
  {
    cerr << "Error: queue underflow" << endl;
    exit(-1);
  }
#endif
  head = (head + n) % MaxSize;
  size -= n;
}

/**
 * \brief Remove the tail of the queue
 */
template <class T, int MaxSize>
INLINE void Queue<T, MaxSize>::RemoveTail()
{
#ifdef SAFE_MODE
  if(size <= 0)
  {
    cerr << "Error: queue underflow" << endl;
    exit(-1);
  }
#endif
  tail = (tail + MaxSize - 1) % MaxSize;
  size--;
}

/**
 * \brief Returns the head of the queue
 */
template <class T, int MaxSize>
INLINE T *Queue<T, MaxSize>::GetHead()
{
  if(size > 0)
    return storage + head;
  return 0;
}

/**
 * \brief Returns the tail of the queue
 */
template <class T, int MaxSize>
INLINE T *Queue<T, MaxSize>::GetTail()
{
  if(size > 0)
    return storage + tail;
  return 0;
}

/**
 * \brief Add and return a new item to the queue
 */
template <class T, int MaxSize>
INLINE T *Queue<T, MaxSize>::New()
{
  if(size >= MaxSize)
  {
    return 0;
  }
  tail = (tail + 1) % MaxSize;
  T *entry = storage + tail;
  size++;
  return entry;
}

/**
 * \brief Flush the queue
 */
template <class T, int MaxSize>
INLINE void Queue<T, MaxSize>::Flush()
{ head = 0;
  tail = MaxSize - 1;
  size = 0;
}

/**
 * \brief Reset the queue
 */
template <class T, int MaxSize>
INLINE void Queue<T, MaxSize>::Reset()
{ head = 0;
  tail = MaxSize - 1;
  size = 0;
}

/**
 * \brief Flush the queue after one element
 */
template <class T, int MaxSize>
INLINE void Queue<T, MaxSize>::FlushAfter(const QueuePointer<T, MaxSize>& qp)
{ if(qp.queue == this)
  { while(tail != qp.current)
    { tail = (tail + MaxSize - 1) % MaxSize;
      size--;
    }
  }
}

/**
 * \brief Return an indexed element from the queue
 */
template <class T, int MaxSize>
INLINE int Queue<T, MaxSize>::GetIndex(int index)
{ return (tail + index) % MaxSize;
}

/**
 * \brief Perform an integrity check on the queue
 */
template <class T, int MaxSize>
INLINE bool Queue<T, MaxSize>::Check()
{ int i, j, n, m;
  for(i = head, n = 0; n < size; n++, i = (i + 1) % MaxSize)
  { for(j = head, m = 0; m < size; m++, j = (j + 1) % MaxSize)
    { if(i != j && storage[i] == storage[j])
      { cerr << "integrity check of queue failed" << endl;
        return false;
      }
    }
  }
  return true;
}


/**
 * \brief Generic Stack class
 */
template <class T, int MaxSize>
class Stack
{public:
  Stack();

  void Push(const T& value);
  T Pop();
  T Peek();
  bool Empty();
  int Size();
private:
  int top;
  T storage[MaxSize];
};

/**
 * \brief Create a new empty Stack
 */
template <class T, int MaxSize>
Stack<T, MaxSize>::Stack()
{ top = -1;
}

/**
 * \brief Push a new value to the stack
 */
template <class T, int MaxSize>
void Stack<T, MaxSize>::Push(const T& value)
{ storage[++top] = value;
}

/**
 * \brief Pop the top value of the stack
 */
template <class T, int MaxSize>
T Stack<T, MaxSize>::Pop()
{ return storage[top--];
}

/**
 * \brief Peek the top value of the stack (without poping)
 */
template <class T, int MaxSize>
T Stack<T, MaxSize>::Peek()
{ return storage[top];
}

/**
 * \brief Returns true if the stack is empty
 */
template <class T, int MaxSize>
bool Stack<T, MaxSize>::Empty()
{ return top < 0;
}

/**
 * \brief Returns the size of the queue
 */
template <class T, int MaxSize>
int Stack<T, MaxSize>::Size()
{ return top + 1;
}

/**
 * \brief Generic circular stack
 */
template <class T, int MaxSize>
class CircularStack
{public:
  CircularStack();

  void Push(const T& value);
  T Pop();
  T Peek();
  void Flush();

  bool Empty();
  int Size();

  friend ostream& operator << (ostream& os, const CircularStack<T, MaxSize>& stack)
  { int i, j;
    for(i = 0, j = stack.top; i < stack.size; i++, j = j < 0 ? MaxSize - 1 : j - 1)
    { os << stack.storage[j] << endl;
    }
    return os;
  }
private:
  int top;
  int size;
  T storage[MaxSize];
};

/**
 * \brief Creates a new empty circular stack
 */
template <class T, int MaxSize>
CircularStack<T, MaxSize>::CircularStack()
{ top = -1;
  size = 0;
}

/**
 * \brief Push a new value to the stack
 */
template <class T, int MaxSize>
void CircularStack<T, MaxSize>::Push(const T& value)
{ top = (top + 1) % MaxSize;
  if(size <= MaxSize) size++;
  storage[top] = value;
}

/**
 * \brief Pop the top value of the stack
 */
template <class T, int MaxSize>
T CircularStack<T, MaxSize>::Pop()
{ T value = storage[top];
  top = (top + MaxSize - 1) % MaxSize;
  if(size > 0)
    size--;
  return value;
}

/**
 * \brief Returns the top item of the queue
 */
template <class T, int MaxSize>
T CircularStack<T, MaxSize>::Peek()
{ return storage[top];
}

/**
 * \brief Returns true if the stack is empty
 */
template <class T, int MaxSize>
bool CircularStack<T, MaxSize>::Empty()
{ return size == 0;
}

/**
 * \brief Returns the size of the queue
 */
template <class T, int MaxSize>
int CircularStack<T, MaxSize>::Size()
{ return size;
}

/**
 * \brief Flushes the queue
 */
template <class T, int MaxSize>
void CircularStack<T, MaxSize>::Flush()
{ top = -1;
  size = 0;
}

/**
 * \brief Pipeline Entry class
 */
template <class T>
class PipelineEntry
{public:
  int delay;   ///< Delay of the entry
  T data;      ///< Data associated with the entry

  /**
   * \brief Compares two pipeline entry (based on data)
   */
  int operator == (const PipelineEntry& entry) const
  {
    return entry.data == data;
  }
};

template <class T, int nStages>
class Pipeline
{public:
  Pipeline()
  {}

  virtual bool HasAccept(const T *value) = 0;
  virtual bool HasValid() = 0;
  virtual bool HasEnable() = 0;
  virtual bool Enabled(const T *value) = 0;
  virtual void ReadInput(T& value) = 0;
  virtual void WriteOutput(const T& value) = 0;
  virtual void DisableOutput() = 0;

  /**
   * \brief Flush the pipeline
   */
  void Flush()
  { queue.Flush();
  }

  bool Accept()
  {
    //		if(enable) {
    //			if(full) {
    //				return inAccept;
    //			} else {
    //				return true;
    //			}
    //	} else {
    //		return false;
    //	}
    PipelineEntry<T> *entry = queue.GetHead();
    bool enabled = Enabled(entry ? &entry->data : 0);
    return HasValid() && (enabled || !queue.Full());
  }

  bool Run()
  {
    bool changed = false;
    QueuePointer<PipelineEntry<T>, nStages> cur;
    PipelineEntry<T> *entry;
    if(nStages > 1)
    {
      entry = queue.GetHead();
      if(HasAccept(entry ? &entry->data : 0))
      {
        queue.RemoveHead();
        changed = true;
      }

      /* Run */
      for(cur = queue.SeekAtHead(); cur; cur++)
      {
        if(cur->delay > 0)
        {
          cur->delay--;
        }
      }

      /* Output */
      entry = queue.GetHead();
      if(entry && entry->delay == 0)
      {
        WriteOutput(entry->data);
      }
      else
      {
        DisableOutput();
      }

      /* Add */
      if(HasEnable())
      {
        entry = queue.New();
        if(entry)
        {
          ReadInput(entry->data);
          entry->delay = nStages - 1;
          changed = true;
        }
      }
    }
    else
    {
      //			printf("->>>1\n");
      //			fflush(stdout);
      entry = queue.GetHead();
      if(HasAccept(entry ? &entry->data : 0))
      {
        queue.RemoveHead();
        changed = true;
      }

      //			printf("->>>2\n");
      //			fflush(stdout);
      /* Add */
      if(HasEnable())
      {
        entry = queue.New();
        if(entry)
        {
          ReadInput(entry->data);
          entry->delay = nStages - 1; /* probably not useful */
          changed = true;
        }
      }

      //			printf("->>>3\n");
      //			fflush(stdout);
      /* Output */
      entry = queue.GetHead();
      if(entry && entry->delay == 0)
      {
        WriteOutput(entry->data);
      }
      else
      {
        DisableOutput();
      }
      //			printf("->>>4\n");
      //			fflush(stdout);

    }
    return changed;
  }


  /**
   * \brief Pretty printer of the pipeline class
   */
  friend ostream& operator << (ostream& os, /* const */ Pipeline<T, nStages>& pipeline)
  { QueuePointer<PipelineEntry<T>, nStages> entry;

    //		os << "queue size = " << pipeline.queue.Size() << endl;
    for(entry = pipeline.queue.SeekAtTail(); entry; entry--)
    {
      os << "data = " << entry->data << ", delay = " << entry->delay << endl;
    }
    return os;
  }

  bool Check()
  {
    return queue.Check();
  }

private:
  Queue<PipelineEntry<T>, nStages> queue;
};


template <class T, int Width, int nStages>
class WidePipeline
{public:
  WidePipeline()
  {
    nOutputs = 0;
  }

  virtual int HasAccept(int nOutputs) = 0;
  virtual int Enabled(int nOutputs) = 0;
  virtual int HasValid() = 0;
  virtual int HasEnable() = 0;
  virtual void ReadInput(T& value) = 0;
  virtual int WriteOutput(int nValids) = 0;

  int Size()
  { return queue.Size();
  }

  void Flush()
  { nOutputs = 0;
    queue.Flush();
  }

  T& operator [] (int index)
  { return queue[index].data;
  }

  int Accept()
  {
    int nValids = HasValid();
    int enabled = Enabled(nOutputs);
    int freeSpace = queue.FreeSpace();

    return MIN(freeSpace + enabled, nValids);
  }

  bool Run()
  {
    QueuePointer<PipelineEntry<T>, Width * nStages> cur;
    PipelineEntry<T> *entry;
    bool changed = false;
    if(nStages > 1)
    {
      int nAccepts = HasAccept(nOutputs);
      if(nAccepts)
      {
        queue.RemoveHead(nAccepts);
        changed = true;
      }

      /* Run */
      for(cur = queue.SeekAtHead(); cur; cur++)
      {
        if(cur->delay > 0)
        {
          cur->delay--;
        }
      }

      /* Output */
      nOutputs = 0;
      if(!queue.Empty())
      {
        for(cur = queue.SeekAtHead(); cur && nOutputs < Width; cur++)
        {
          if(cur->delay > 0)
            break;
          nOutputs++;
        }
      }
      nOutputs = WriteOutput(nOutputs);

      /* Add */
      int nEnables = HasEnable();
      if(nEnables > 0)
      {
        do
        {
          entry = queue.New();
          if(entry)
          {
            ReadInput(entry->data);
            entry->delay = nStages - 1;
            changed = true;
          }
          else
          {
            exit(-1);
          }
        }
        while(--nEnables > 0);
      }
    }
    else
    {
      int nAccepts = HasAccept(nOutputs);
      if(nAccepts > 0)
      {
        queue.RemoveHead(nAccepts);
        changed = true;
      }

      /* Add */
      int nEnables = HasEnable();
      if(nEnables > 0)
      {
        do
        {
          entry = queue.New();
          if(entry)
          {
            ReadInput(entry->data);
            entry->delay = nStages - 1; /* probably not useful */
            changed = true;
          }
        }
        while(--nEnables > 0);
      }

      /* Output */

      nOutputs = 0;
      if(!queue.Empty())
      {
        for(cur = queue.SeekAtHead(); cur && nOutputs < Width; cur++)
        {
          if(cur->delay > 0)
            break;
          nOutputs++;
        }
      }
      nOutputs = WriteOutput(nOutputs);
    }
    return changed;
  }

  friend ostream& operator << (ostream& os, /* const */ WidePipeline<T, Width, nStages>& pipeline)
  {
    QueuePointer<PipelineEntry<T>, nStages * Width> entry;

    os << "queue size = " << pipeline.queue.Size() << endl;
    for(entry = pipeline.queue.SeekAtTail(); entry; entry--)
    {
      os << entry->data << ",delay=" << entry->delay << endl;
    }
    return os;
  }

  bool Check()
  {
    return queue.Check();
  }

private:
  int nOutputs;
  Queue<PipelineEntry<T>, nStages * Width> queue;
};




template <class T, int nStages>
class EnhancedPipelineEntry
{public:
  int stage;
  int delay[nStages];
  T data;

  int operator == (const EnhancedPipelineEntry& entry) const
  { return entry.data == data;
  }
};

template <class T, int nStages>
class EnhancedPipeline
{public:
  virtual bool HasAccept() = 0;
  virtual bool HasValid() = 0;
  virtual bool HasEnable() = 0;
  virtual bool Enabled() = 0;
  virtual void ReadInput(T& value, int latencies[nStages]) = 0;
  //	virtual void ReadInput(T& value) = 0;
  virtual void WriteOutput(const T& value) = 0;
  virtual void DisableOutput() = 0;

  EnhancedPipeline()
  {
  }

  EnhancedPipeline(const int latencies[nStages])
  {
  }

  void Flush()
  {
    queue.Flush();
  }

  bool Run()
  {
    bool changed = false;
    EnhancedPipelineEntry<T, nStages> *entry;

    if(nStages > 1)
    {
      if(HasAccept())
      {
        queue.RemoveHead();
        changed = true;
      }

      QueuePointer<EnhancedPipelineEntry<T, nStages>, nStages> cur, prev;

      /* For each entry into the pipeline */
      for(cur = queue.SeekAtHead(); cur; cur++)
      {
        /* if the pipe stage work is finished */
        if(cur->delay[cur->stage] == 0)
        {
          if(prev)
          {
            /* and if this is not the last entry into the pipeline, then make the queue entry become the next pipe stage */
            if(prev->stage > cur->stage + 1)
              cur->stage++;
          }
          else
          {
            /* if this is the last entry into the pipeline and this is not the last pipe stage then make the queue entry entry become the next pipe stage */
            if(cur->stage < nStages - 1)
            {
              cur->stage++;
              if(cur->stage == 1)
                changed = true;
            }
          }
        }
        /* Remember the previous entry (which is older than the current) */
        prev = cur;
      }

      /* For each entry into the pipeline */
      for(cur = queue.SeekAtHead(); cur; cur++)
      {
        /* if the pipe stage work is not finished */
        if(cur->delay[cur->stage] > 0)
        {
          /* then we decrement the delay (to make it work for one clock cycle) */
          --cur->delay[cur->stage];
        }
      }

      entry = queue.GetHead();
      if(entry && entry->stage == nStages - 1 && entry->delay[nStages - 1] == 0)
      {
        WriteOutput(entry->data);
      }
      else
      {
        DisableOutput();
      }

      if(HasEnable())
      {
        entry = queue.New();
        if(!entry)
        {
          fprintf(stderr, "Panic");
          exit(1);
        }
        changed = true;
        ReadInput(entry->data, entry->delay);
        //fprintf(stdout, "#### entry = %p, data = %d\n", entry, entry->data);
        entry->stage = 0;
        if(entry->delay[entry->stage] > 0)
          entry->delay[entry->stage]--;
      }
    }
    else
    {
      if(HasAccept())
      {
        queue.RemoveHead();
        changed = true;
      }

      /* For each entry into the pipeline */
      EnhancedPipelineEntry<T, nStages> *cur;
      cur = queue.GetHead();
      if(cur)
      {
        /* if the pipe stage work is not finished */
        if(cur->delay[0] > 0)
        {
          if(cur->delay[0] == 1)
            changed = true;
          /* then we decrement the delay (to make it work for one clock cycle) */
          --cur->delay[0];
        }
      }

      if(HasEnable())
      {
        if(queue.Full())
        {
          fprintf(stderr, "Panic\n");
          exit(-1);
        }
        changed = true;
        entry = queue.New();
        ReadInput(entry->data, entry->delay);
        entry->stage = 0;
        if(entry->delay[0] > 0)
          entry->delay[0]--;
      }

      entry = queue.GetHead();
      if(entry && entry->delay[0] == 0)
      {
        WriteOutput(entry->data);
      }
      else
      {
        DisableOutput();
      }
    }
    return changed;
  }

  bool Accept()
  {
    QueuePointer<EnhancedPipelineEntry<T, nStages>, nStages> cur;
    //		EnhancedPipelineEntry<T, nStages> *cur;
    int stage;
    bool enabled = Enabled();

    if(HasValid())
    {
      /* if queue is empty then the pipeline is ready */
      if(!queue.Empty())
      {
        /* Walk across the queue from the newest to the oldest entry */
        for(stage = 0, cur = queue.SeekAtTail(); cur; cur--, stage++)
        {
          /* if there is a bubble into the pipeline then the pipeline is ready */
          if(cur->stage > stage)
          {
            return true;
          }

          /* There is a special case for the last pipe stage */
          if(stage == nStages - 1)
          {
            /* it is ready if the last pipe stage has an instruction to issue and the next module is ready */
            return cur->delay[stage] == 0 && enabled;
          }
          else
          {
            /* If a pipe stage (which is not the last) has not finished its work then functional unit is not ready */
            if(cur->delay[stage] > 0)
            {
              return false;
            }
          }
        }
      }
      /* At this point, we know that the first stage will be empty in the next clock cycle, so the pipeline is ready to get a new data */
      return true;
    }
    return false;
  }

  friend ostream& operator << (ostream& os, /* const */ EnhancedPipeline<T, nStages>& pipeline)
  {
    QueuePointer<EnhancedPipelineEntry<T, nStages>, nStages> entry;

    os << "queue size = " << pipeline.queue.Size() << endl;
    for(entry = pipeline.queue.SeekAtTail(); entry; entry--)
    {
      os << "data=" << entry->data << ",stage=" << entry->stage << ",delay=";
      int i;
      for(i = 0; i < nStages; i++)
        os << entry->delay[i] << " ";
      os << endl;
    }
    return os;
  }

  bool Check()
  {
    return queue.Check();
  }

private:
  Queue<EnhancedPipelineEntry<T, nStages>, nStages> queue;
};

template <class T, int latency>
class Delay
{public:
  Delay()
  {}

  virtual bool HasAccept() = 0;
  virtual bool HasValid() = 0;
  virtual bool HasEnable() = 0;
  virtual void ReadInput(T& value) = 0;
  virtual void WriteOutput(const T& value) = 0;
  virtual void DisableOutput() = 0;

  void Flush()
  { full = false;
    delay = 0;
  }

  void Run()
  {
    if(latency > 1)
    {
      if(HasAccept())
      {
        full = false;
      }

      if(delay > 0)
        delay--;

      if(full && delay == 0)
      {
        WriteOutput(data);
      }
      else
      {
        DisableOutput();
      }

      if(HasEnable())
      {
        if(full)
        {
          fprintf(stderr, "Panic\n");
          exit(-1);
        }
        ReadInput(data);
        full = true;
        delay = latency - 1;
      }
    }
    else
    {
      if(HasAccept())
      {
        full = false;
      }

      if(HasEnable())
      {
        ReadInput(data);
        full = true;
      }

      if(full)
      {
        WriteOutput(data);
      }
      else
      {
        DisableOutput();
      }
    }
  }

  bool Accept()
  {
    return !full || HasAccept();
  }
private:
  bool full;
  int delay;
  T data;
};


#undef GCC_INLINE

#endif
