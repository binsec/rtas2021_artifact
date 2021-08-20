/***************************************************************************
                                 specialization.hh
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

#ifndef __SPECIALIZATION_HH__
#define __SPECIALIZATION_HH__

#include <fwd.hh>
#include <iosfwd>

#include <conststr.hh>
#include <referencecounting.hh>
#include <vect.hh>

struct Constraint : virtual ReferenceCounter
{
  ConstStr               m_symbol; /**< The specialization symbol */
  unsigned int           m_value;  /**< The specialization value */
  
  Constraint( ConstStr _symbol, unsigned int _value );
  ~Constraint();
};

std::ostream& operator<<( std::ostream& _sink, Constraint const& _var );

/** A specialization object */
struct Specialization : virtual ReferenceCounter
{
  Ptr<Operation>         m_operation; /**< The operation wich is specialized */
  Vector<Constraint>   m_constraints; /**< The list of variables associated with the specialization */

  Specialization( Operation* _operation, Vector<Constraint>& _variables );
  ~Specialization();
  
  Operation*             newop();
  
  Constraint*          constraint( ConstStr _symbol );
};

std::ostream& operator<<( std::ostream& _sink, Specialization const& _var );

#endif // __SPECIALIZATION_HH__
