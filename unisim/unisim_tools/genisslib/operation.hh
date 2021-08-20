/***************************************************************************
                                 operation.hh
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

#ifndef __OPERATION_HH__
#define __OPERATION_HH__

#include <inttypes.h>
#include <fwd.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <referencecounting.hh>
#include <vect.hh>
#include <iosfwd>

/** An operation object */
struct Operation : virtual ReferenceCounter
{
  Operation( ConstStr _symbol,
             Vector<BitField> const& _bitfields, Vector<Comment> const& _comments,
             SourceCode* _op_condition, FileLoc const& _fileloc );
  ~Operation();
  
  void                        add( Action* _action );
  Action const*               action( ActionProto const* _actionproto ) const;

  ConstStr                    symbol;            /**< The associated symbol */
  Vector<BitField>            bitfields;         /**< The bit field list of the operation */
  Vector<Action>              actions;           /**< The list of actions of the operation */
  Vector<Comment>             comments;          /**< The list of the C comment associated with the operation */
  Vector<Variable>            variables;         /**< The list of variables associated with the operation */
  Ptr<SourceCode>             condition;         /**< The condition associated with the operation */
  FileLoc                   fileloc;           /**< The file location where the operation was declared */
};

std::ostream& operator<<( std::ostream& _sink, Operation const& _op );

/** A group object */
struct Group : virtual ReferenceCounter
{
  Group( ConstStr _symbol, Vector<Operation>& _operations, FileLoc const& _fileloc );
  Group( ConstStr _symbol, FileLoc const& _fileloc );

  ConstStr                    symbol;         /**< The associated symbol */
  Vector<Operation>           operations;     /**< an operation node list object containing the operations of the group */
  FileLoc                   fileloc;        /**< The file location where the group was declared */
};

#endif // __OPERATION_HH__
