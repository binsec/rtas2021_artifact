/***************************************************************************
                                 product.hh
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

#ifndef __PRODUCT_HH__
#define __PRODUCT_HH__

#include <fwd.hh>
#include <iosfwd>
#include <conststr.hh>
#include <errtools.hh>
#include <strtools.hh>
#include <vector>
#include <string>

struct Product
{
  ConstStr            m_filename;
  std::string         m_line;
  unsigned int        m_lineno;
  std::vector<int>    m_indentations;
  bool                m_sourcelines;
  
  Product( ConstStr _filename, bool _sourcelines );
  virtual ~Product() {};
  
  Product&          usercode( FileLoc const& _fileloc, char const* _format, ... );
  Product&          usercode( SourceCode const& _source );
  Product&          usercode( SourceCode const& _source, char const* _fmt );
  Product&          code( char const* _format, ... );
  Product&          template_signature( Vector<CodePair> const& _tparams );
  Product&          template_abbrev( Vector<CodePair> const& _tparams );
  Product&          ns_enter( std::vector<ConstStr> const& _namespace );
  Product&          ns_leave( std::vector<ConstStr> const& _namespace );
  Product&          require_newline();
  Product&          write( char const* _chars );
  void                flush();
  Product&          flatten_indentation();
  
  virtual void        xwrite( char const* chrs ) = 0;
};

struct FProduct : public Product
{
  std::ostream*       m_sink;
  
  FProduct( char const* prefix, char const* suffix, bool sourcelines );
  ~FProduct();
  
  bool                good() const;
  void                xwrite( char const* chrs );
};

struct SProduct : public Product
{
  std::string         m_content;
  
  SProduct( ConstStr _prefix, bool _sourcelines );
  
  void                xwrite( char const* chrs );
};

#endif // __PRODUCT_HH__
