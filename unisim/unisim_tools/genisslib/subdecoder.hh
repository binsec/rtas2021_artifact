/***************************************************************************
                                 subdecoder.hh
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

#ifndef __SUBDECODER_HH__
#define __SUBDECODER_HH__

#include <inttypes.h>
#include <fwd.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <referencecounting.hh>
#include <vect.hh>
#include <set>

/** A Sub Decoder class object */
struct SDClass :  virtual ReferenceCounter
{
  std::vector<ConstStr>       m_namespace;         /**< The namespace in which the decoder is defined */
  std::set<unsigned int>        m_insnsizes;         /**< instructions size set (in bytes) of the decoder's operations */
  FileLoc                     m_fileloc;           /**< The file location where subdecoder was declared */

  template<typename _InputIterator>
  SDClass( std::vector<ConstStr>& _namespace, _InputIterator szbeg, _InputIterator szend, FileLoc const& _fileloc );
  ~SDClass();

  ConstStr                    qd_namespace() const;
  
  unsigned int                  maxsize() const { return *m_insnsizes.rbegin(); }
  unsigned int                  minsize() const { return *m_insnsizes.begin(); }
};

/** A Sub Decoder instance object */
struct SDInstance : virtual ReferenceCounter {
  ConstStr                    m_symbol;            /**< The name of the subdecoder instance */
  ConstPtr<SourceCode>      m_template_scheme;   /**< The template scheme associated with the instance */
  ConstPtr<SDClass>         m_sdclass;           /**< The subdecoder class associated of the instance */
  FileLoc                     m_fileloc;           /**< The file location where subdecoder was instanciated */
  
  SDInstance( ConstStr _symbol, SourceCode const* _template_scheme, SDClass const* _sdclass, FileLoc const& _fileloc );
  ~SDInstance();
  
};

/** Create a subdecoder class object
    @param _namespace a namespace in which the decoder is defined
    @param _minsize a minimum bit size for the decoder operations
    @param _maxsize a maximum bit size for the decoder operations
    @param _fileloc a fileloc pointing at the subdecoder declaration
*/
template<typename _InputIterator>
SDClass::SDClass( std::vector<ConstStr>& _namespace, _InputIterator szbeg, _InputIterator szend, FileLoc const& _fileloc )
  : m_namespace( _namespace ), m_insnsizes( szbeg, szend ), m_fileloc( _fileloc )
{}


#endif // __SUBDECODER_HH__
