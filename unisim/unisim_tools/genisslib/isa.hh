/***************************************************************************
                                   isa.hh
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

#ifndef __ISA_HH__
#define __ISA_HH__

#include <fwd.hh>
#include <vect.hh>
#include <conststr.hh>
#include <errtools.hh>
#include <map>
#include <memory>
#include <iosfwd>
#include <referencecounting.hh>

#include <conststr.hh>
#include <vector>

// external options (environ, command-line...)
struct Opts
{
  Opts(char const*);
  
  char const*             outputprefix;
  unsigned int            verbosity;
  char const*             expandname;
  char const*             depfilename;
  unsigned int            minwordsize;
  bool                    sourcelines;
  bool                    privatemembers;
  bool                    specialization;
  bool                    comments;
  
  virtual ConstStr        locate( char const* _name ) const = 0;
  virtual char const*     appname() const = 0;
  virtual char const*     appversion() const = 0;
};

struct Isa
{
  enum DecoderType_t { RiscDecoder = 0, CiscDecoder, VliwDecoder };
  DecoderType_t                 m_decoder;         /**< Decoder Type */
  bool                          m_is_subdecoder;   /**< Subdecoder or full decoder */
  bool                          m_withcache;       /**< Generate cache structure */
  bool                          m_withsource;      /**< Action source code accessible or not */
  bool                          m_withencode;      /**< Action source code accessible or not */
  bool                          m_little_endian;   /**< Endianness of isa (false: big endian, true: little endian) */
  bool                          m_asc_forder;      /**< bitfields ordering (false: descending)*/
  bool                          m_asc_worder;      /**< words ordering (false: descending)*/
  unsigned int                  m_minwordsize;     /**< minimum C type size for operand fields */
  std::vector<ConstStr>         m_namespace;       /**< Encapsulating namespace of the iss */
  Vector<CodePair>              m_tparams;         /**< Template parameters of the iss */
  Vector<Variable>              m_vars;            /**< Global variables used by the iss */
  Vector<ActionProto>           m_actionprotos;    /**< Action prototypes of operations */
  Vector<Operation>             m_operations;      /**< Defined instructions */
  Vector<Group>                 m_groups;          /**< Defined groups */
  Vector<SDClass>               m_sdclasses;       /**< Defined subdecoder classes */
  Vector<SDInstance>            m_sdinstances;     /**< Defined subdecoder instances */
  Vector<SourceCode>            m_decl_srccodes;   /**< Code to insert in header file */
  Vector<SourceCode>            m_impl_srccodes;   /**< Code to insert in source file */
  ConstStr                      m_addrtype;        /**< C type for instructions addresses */
  std::vector<ConstStr>         m_includes;        /**< files included by the isa main file */
  Vector<Specialization>        m_specializations; /**< Requested specializations */
  Vector<Inheritance>           m_inheritances;    /**< Defined inheritances for operation class */
  
  typedef std::map<ConstStr,Group*> GroupAccumulators;
  GroupAccumulators             m_group_accs;      /**< Active group accumulators */
  
  struct Ordering
  {
    FileLoc fileloc;
    ConstStr top_op;
    std::vector<ConstStr> under_ops;
  };
  typedef std::vector<Ordering> Orderings;
  Orderings                     m_user_orderings;

  Isa();
  ~Isa();
  
  void                          remove( Operation* _op );
  void                          add( Operation* _op );
  void                          remove( ActionProto const* _ap );
  Operation*                    operation( ConstStr _symbol );
  bool                          group_extend( Group* _group, ConstStr _symbol );
  struct OOG
  {
    virtual ~OOG() {}
    virtual void with( Operation& operation ) = 0;
  };
  bool                          for_ops( ConstStr _symbol, OOG& oog );
  Group*                        group( ConstStr _symbol );
  ActionProto const*            actionproto( ConstStr _symbol ) const;
  SDClass const*                sdclass( std::vector<ConstStr>& _namespace ) const;
  SDInstance const*             sdinstance( ConstStr _symbol ) const;
                                
  Generator*                    generator( Isa& _source, Opts const& _options ) const;
                                
  void                          expand( std::ostream& _sink ) const;
  void                          deps( std::ostream& _sink, char const* _prefix ) const;
  bool                          sanity_checks() const;
  void                          specialize();
  
  struct UnknownIdent
  {
    ConstStr m_ident;
    UnknownIdent( ConstStr ident ) : m_ident( ident ) {}
  };
  
  void                          setparam( ConstStr key, ConstStr value );
  void                          setparam( ConstStr key, SourceCode const& value );
  void                          setparam( ConstStr key, unsigned int value );
  
  struct ParseError {};
  
  void                          group_command( ConstStr _symbol, ConstStr _command, FileLoc const& fl );
};

#endif // __ISA_HH__
