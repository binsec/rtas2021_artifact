/***************************************************************************
                                 specialization.cc
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

#include <specialization.hh>
#include <operation.hh>
#include <strtools.hh>
#include <action.hh>
#include <variable.hh>
#include <bitfield.hh>
#include <iostream>

/** 
 *  @brief constraint object constructor
 *
 *  @param _symbol the variable symbol concerned about the specialization
 *  @param _value the specialization value for the target variable.
 *
 */

Constraint::Constraint( ConstStr _symbol, unsigned int _value )
  : m_symbol( _symbol ), m_value( _value )
{}

/**
 *  @brief constraint object destructor
 *
 */

Constraint::~Constraint() {}

/** Dump the specialization object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Constraint const& _var ) {
  return (_sink << _var.m_symbol << " = " << _var.m_value);
}

/** 
 *  @brief specialization object constructor
 *
 *  @param _operation the operation being specialized
 *  @param _variables the specialization variables
 *
 */

Specialization::Specialization( Operation* _operation, Vector<Constraint>& _constraints )
  : m_operation( _operation ), m_constraints( _constraints )
{}

/** 
 *  @brief specialization object destructor
 *
 */

Specialization::~Specialization() {};

/** Dump the specialization object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& _sink, Specialization const& _var ) {
  _sink << _var.m_operation->symbol << " ( ";
  char const* sep = "";
  for( Vector<Constraint>::const_iterator node = _var.m_constraints.begin(); node < _var.m_constraints.end(); ++node, sep = ", " )
    _sink << sep << (**node);
  _sink << " )";
  return _sink;
}

Operation*
Specialization::newop()
{
  ConstStr symbol; // The symbol of the operation
  {
    std::string buffer;
    buffer.append( "__spec__" ).append( m_operation->symbol.str() );
    for( Vector<Constraint>::const_iterator expr = m_constraints.begin(); expr < m_constraints.end(); ++ expr )
      buffer+= Str::fmt( "_%s_%x", (**expr).m_symbol.str(), (**expr).m_value ).str();
    symbol = Str::tokenize( buffer.c_str() );
  }

  //  Actions, comments, variables, conditions, and fileloc are
  //  duplicated.
  Operation* res = new Operation( symbol, Vector<BitField>(), m_operation->comments,
                                      m_operation->condition, m_operation->fileloc );
  
  res->variables = m_operation->variables;
  res->actions = m_operation->actions;
  // Generating new bitfield.
  Vector<BitField>& bflist = m_operation->bitfields;
  
  for (Vector<BitField>::const_iterator bf = bflist.begin(); bf < bflist.end(); ++ bf )
    {
      OperandBitField const* opbf;
      Constraint* expr;
      if ((opbf = dynamic_cast<OperandBitField const*>( &**bf )) and (expr = constraint( (**bf).getsymbol() )) ) {
        res->bitfields.push_back( new SpOperandBitField( *opbf, expr->m_value ) );
      } else {
        res->bitfields.push_back( *bf );
      }
    }
  
  return res;
}

Constraint*
Specialization::constraint( ConstStr _symbol ) {
  for( Vector<Constraint>::iterator expr = m_constraints.begin(); expr < m_constraints.end(); ++ expr )
    if( (**expr).m_symbol == _symbol ) return *expr;
  return 0;
}
