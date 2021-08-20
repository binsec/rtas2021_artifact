/***************************************************************************
                                operation.cc
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

#include <operation.hh>
#include <scanner.hh>
#include <comment.hh>
#include <variable.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <bitfield.hh>
#include <action.hh>
#include <isa.hh>
#include <cassert>
#include <ostream>

/** Create an operation object
    @param symbol a symbol object representing the operation
    @param bitfield_list a bit field list object containing the bit fields of the operation
    @param comment_list a C/C++ comment list object containing C/C++ comments relative to the operation
    @param variable_list a variable list object
    @param filename a filename object where the operation was found
    @param lineno a line number where the operation was found
    @return an operation object
*/
Operation::Operation( ConstStr _symbol, Vector<BitField> const& _bitfields, Vector<Comment> const& _comments,
                          SourceCode* _op_condition, FileLoc const& _fileloc )
  : symbol( _symbol ), bitfields( _bitfields ), comments( _comments ),
    condition( _op_condition ), fileloc( _fileloc )
{
}

/** Delete an operation object
    @param operation an operation object to delete
*/
Operation::~Operation() {}

/** Create a group object
    @param operation_list the list of the operation of the group
    @returns a group list object
*/
Group::Group( ConstStr _symbol, Vector<Operation>& _oplist, FileLoc const& _fileloc )
  : symbol( _symbol ), operations( _oplist ), fileloc( _fileloc )
{}

/** Create a group object
    @param _symbol the symbol naming the group
    @param _fileloc the source file location of the group declaration
    @returns a group list object
*/
Group::Group( ConstStr _symbol, FileLoc const& _fileloc )
  : symbol( _symbol ), fileloc( _fileloc )
{}

/** Search the operation for an action implementing an action prototype
    @param actionproto an action prototype object
    @return the matching action object, null if no action object matches
*/
Action const*
Operation::action( ActionProto const* proto ) const
{
  for (Vector<Action>::const_iterator iter = actions.begin(); iter < actions.end(); ++ iter)
    if ((**iter).m_actionproto == proto) return *iter;
  
  return 0;
}

void
Operation::add( Action* _action )
{
  actions.push_back( _action );
  assert( not _action->m_operation );
  _action->m_operation = this;
}

/** Dump an operation object into a stream
    @param operation an operation object to dump
    @param sink a stream
*/

std::ostream&
operator<<( std::ostream& sink, Operation const& _op )
{
  sink << "op " << _op.symbol << "( ";
  
  char const* sep = "";
  for (Vector<BitField>::const_iterator bf = _op.bitfields.begin(); bf < _op.bitfields.end(); sep = " : ", ++ bf)
    sink << sep << (**bf);
  
  sink << " )\n";
  
  if (not _op.variables.empty())
    {
      sink << _op.symbol << ".var ";
      sep = "";
      for( Vector<Variable>::const_iterator var = _op.variables.begin(); var < _op.variables.end(); sep = ", ", ++ var )
        sink << sep << *(*var);
    }
  sink << "\n\n";
  
  for( Vector<Action>::const_iterator action = _op.actions.begin(); action < _op.actions.end(); ++ action )
    sink << *(*action) << "\n\n";
  
  return sink;
}

