/***************************************************************************
                                 variable.cc
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

#include <variable.hh>
#include <sourcecode.hh>
#include <strtools.hh>
#include <iostream>

Variable::Variable( ConstStr _symbol, SourceCode* _ctype, SourceCode* _cinit )
  : symbol( _symbol ), ctype( _ctype ), cinit( _cinit )
{}

Variable::Variable( Variable const& _variable )
  : symbol( _variable.symbol ), ctype( _variable.ctype ), cinit( _variable.cinit )
{}

Variable::~Variable() {};

/** Dump the variable object into a stream
    @param stream a stream
*/
std::ostream&
operator<<( std::ostream& sink, Variable const& var )
{
  sink << var.symbol << " : " << *var.ctype;
  
  if (var.cinit)
    sink << " = " << *var.cinit;
  
  return sink;
}

Inheritance::Inheritance( SourceCode* _modifier, SourceCode* _typename, SourceCode* _initargs )
  : modifier( _modifier ), ctypename( _typename ), initargs( _initargs )
{}

Inheritance::Inheritance( Inheritance const& _inh )
  : modifier( _inh.modifier ), ctypename( _inh.ctypename ), initargs( _inh.initargs )
{}

Inheritance::~Inheritance() {};
