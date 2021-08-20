/***************************************************************************
                                   riscgenerator.hh
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

#ifndef __RISCGENERATOR_HH__
#define __RISCGENERATOR_HH__

#include <fwd.hh>
#include <generator.hh>
#include <conststr.hh>
#include <map>

struct RiscOpCode : public OpCode
{
  uint64_t                    m_mask;
  uint64_t                    m_bits;
  unsigned int                m_size;
  bool                        m_vlen;
    
  // RiscOpCode()
  //   : OpCode( ConstStr() ), m_mask( 0 ), m_bits( 0 ), m_size( 0 ), m_vlen( false )
  // {}
  RiscOpCode( ConstStr _symbol, uint64_t mask, uint64_t bits, unsigned int size, bool vlen )
    : OpCode( _symbol ), m_mask( mask ), m_bits( bits ), m_size( size ), m_vlen( vlen )
  {}

  virtual ~RiscOpCode() {}

  // Topology methods
  location_t                  locate( OpCode const& _oc ) const;
  
  std::ostream&               details( std::ostream& _sink ) const;
};

struct RiscGenerator : public Generator
{
  unsigned int                  m_insn_ctypesize;
  ConstStr                      m_insn_ctype;
  ConstStr                      m_insn_cpostfix;
  
  RiscGenerator( Isa& _source, Opts const& _options );
  ~RiscGenerator();
  
  RiscOpCode const&             riscopcode( Operation const* _op ) const { return dynamic_cast<RiscOpCode const&>( opcode( _op ) ); }
  RiscOpCode&                   riscopcode( Operation const* _op ) { return dynamic_cast<RiscOpCode&>( opcode( _op ) ); };
  
  /* Risc specific Utilities */
  void                          finalize();
  void                          codetype_decl( Product& _product ) const;
  void                          codetype_impl( Product& _product ) const {}
  ConstStr                      codetype_name() const { return "CodeType"; }
  ConstStr                      codetype_ref() const { return "CodeType&"; }
  ConstStr                      codetype_constref() const { return "CodeType"; }
  void                          insn_bits_code( Product& _product, Operation const& _op ) const;
  void                          insn_mask_code( Product& _product, Operation const& _op ) const;
  ConstStr                      insn_id_expr( char const* _addrname ) const;
  void                          insn_match_ifexpr( Product& _product, char const* _code, char const* _mask, char const* _bits ) const;
  void                          insn_unchanged_expr( Product& _product, char const* _old, char const* _new ) const;
  void                          insn_decode_impl( Product& _product, Operation const& _op, char const* _codename, char const* _addrname ) const;
  void                          insn_encode_impl( Product& _product, Operation const& _op, char const* _codename ) const;
  void                          additional_impl_includes( Product& _product ) const {}
  void                          additional_decl_includes( Product& _product ) const {}
  
  void                          insn_destructor_decl( Product& _product, Operation const& _op ) const {};
  void                          insn_destructor_impl( Product& _product, Operation const& _op ) const {};
  
  void                          op_getlen_decl( Product& _product ) const;
  void                          insn_getlen_decl( Product& _product, Operation const& _op ) const;
};

#endif // __RISCGENERATOR_HH__
