/***************************************************************************
                            referencecounting.hh
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef __REFERENCECOUNTING_HH__
#define __REFERENCECOUNTING_HH__

#include <inttypes.h>

struct ReferenceCounter
{
  intptr_t              m_count;
  
  ReferenceCounter() : m_count( 0 ) {}
  
  void                  const_check() const;
};
  
template <class Object_t>
class Ptr
{
  Object_t*             m_object;
  
  void                  retain() { ++ (m_object->ReferenceCounter::m_count); }
  intptr_t              release() { return -- (m_object->ReferenceCounter::m_count); }
  
public:
  Ptr() : m_object( 0 ) {}
  Ptr( Object_t* _object ) : m_object( _object ) { if (m_object) retain(); }
  Ptr( Ptr const& _ref ) : m_object( _ref.m_object ) { if (m_object) retain(); }
  
  Ptr&                operator=( Ptr const& _ref ) {
    if (m_object) release();
    if( (m_object = _ref.m_object) ) retain();
    return *this;
  }
    
  ~Ptr() { if( not m_object or release() > 0 ) return; delete m_object; }
    
  operator Object_t const* () const { if (not m_object) return 0; return m_object; }
  operator Object_t* ()
  {
    // if (m_object) m_object->ReferenceCounter::const_check();
    return m_object;
  }
    
  Object_t const*       operator->() const { if (not m_object) return 0; return m_object; }
  Object_t*             operator->()
  {
    // if (m_object) m_object->ReferenceCounter::const_check();
    return m_object;
  }
};

template <class Object_t>
class ConstPtr {
  Object_t const*       m_object;
  
  typedef ReferenceCounter const* ConstRCP_t;
  typedef ReferenceCounter* RCP_t;
  
  ReferenceCounter*     refcounter() { return const_cast<ReferenceCounter*>( static_cast<ReferenceCounter const*>( m_object ) ); }
  void                  retain() { ++ (refcounter()->m_count); }
  intptr_t              release() { return -- (refcounter()->m_count); }
  
public:
  ConstPtr() : m_object( 0 ) {}
  ConstPtr( Object_t const* _object ) : m_object( _object ) { if (m_object) retain(); }
  ConstPtr( ConstPtr const& _ref ) : m_object( _ref.m_object ) { if (m_object) retain(); }
  ConstPtr( Ptr<Object_t> const& _ref ) : m_object( _ref.m_object ) { if (m_object) retain(); }
  
  ConstPtr&                operator=( ConstPtr const& _ref ) {
    if (m_object) release();
    if( (m_object = _ref.m_object) ) retain();
    return *this;
  }
  
  ConstPtr&                operator=( Ptr<Object_t> const& _ref ) {
    if (m_object) release();
    if( (m_object = _ref.m_object) ) retain();
    return *this;
  }
  
  ~ConstPtr() { if( not m_object or release() > 0 ) return; delete m_object; }
    
  operator Object_t const* () const { if (not m_object) return 0; return m_object; }
  Object_t const*       operator->() const { if (not m_object) return 0; return m_object; }
};

#endif // __REFERENCECOUNTING_HH__
