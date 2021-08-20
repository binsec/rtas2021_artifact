/***************************************************************************
                                 errtools.hh
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

#ifndef __ERRTOOLS_HH__
#define __ERRTOOLS_HH__

/*** Classes an method for handling error log, file locations...
 ***/
#include <iosfwd>
#include <inttypes.h>
#include <conststr.hh>

struct FileLoc
{
  FileLoc() : m_line( 0 ), m_nxtcol( 0 ), m_column( 0 ) {}
  FileLoc( ConstStr _name, intptr_t _line, intptr_t _column )
    : m_name( _name ), m_line( _line ), m_nxtcol( 0 ), m_column( _column )
  {}
  FileLoc( FileLoc const& _fl )
    : m_name( _fl.m_name ), m_line( _fl.m_line ), m_nxtcol( _fl.m_nxtcol ), m_column( _fl.m_column )
  {}
  
  FileLoc& operator=( FileLoc const& _fl )
  { m_name = _fl.m_name; m_line = _fl.m_line; m_nxtcol = _fl.m_nxtcol; m_column = _fl.m_column; return *this; }
  FileLoc& assign( ConstStr _name, intptr_t _line, intptr_t _nxtcol )
  { m_name = _name; m_line = _line; m_nxtcol = _nxtcol; m_column = _nxtcol; return *this; }
  
  void       err( char const* _fmt, ... ) const;
  std::ostream& loc( std::ostream& _sink ) const;
  void       newline() { m_line+=1; m_nxtcol = 1; m_column = 1; };
  void       newtoken( intptr_t len ) { m_column = m_nxtcol; m_nxtcol += len; };
  
  intptr_t   getline() const { return m_line; }
  intptr_t   getcolumn() const { return m_column; }
  ConstStr const& getname() const { return m_name; };

private:
  ConstStr    m_name;
  intptr_t    m_line;
  intptr_t    m_nxtcol;
  intptr_t    m_column;
};


#endif // __ERRTOOLS_HH__
