/***************************************************************************
                                 variable.hh
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

#ifndef __VARIABLE_HH__
#define __VARIABLE_HH__

#include <fwd.hh>
#include <iosfwd>

#include <conststr.hh>
#include <referencecounting.hh>
#include <vector>

/** A variable object */
struct Variable
  : virtual ReferenceCounter
{
  Variable( ConstStr _symbol, SourceCode* _ctype, SourceCode* _cinit );
  Variable( Variable const& _variable );
  ~Variable();

  ConstStr               symbol; /**< the symbol object representing the variable */
  Ptr<SourceCode>        ctype; /**< the C type of the variable */
  Ptr<SourceCode>        cinit; /**< the C expression used to initialized the variable */
};

std::ostream& operator<<( std::ostream& sink, Variable const& _var );

struct Inheritance
  : virtual ReferenceCounter
{
  Inheritance( SourceCode* _modifier, SourceCode* _typename, SourceCode* _initargs );
  Inheritance( Inheritance const& _variable );
  ~Inheritance();
  
  Ptr<SourceCode>           modifier;
  Ptr<SourceCode>           ctypename;
  Ptr<SourceCode>           initargs;
};

#endif // __VARIABLE_HH__
