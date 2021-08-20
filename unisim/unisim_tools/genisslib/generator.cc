/***************************************************************************
                              generator.cc
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

#include <generator.hh>
#include <scanner.hh>
#include <isa.hh>
#include <action.hh>
#include <bitfield.hh>
#include <comment.hh>
#include <operation.hh>
#include <sourcecode.hh>
#include <subdecoder.hh>
#include <variable.hh>
#include <product.hh>
#include <strtools.hh>
#include <vect.hh>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <cassert>

namespace
{
  struct OpProperties
  {
    std::set<unsigned int> m_insnsizes;
    typedef std::vector<unsigned int> insnsizes_t;
    typedef Vector<BitField>::const_iterator bfiter_t;
  
  
    void
    dig( unsigned int cursize, unsigned int rwdsize, unsigned int minsize, bfiter_t bfitr, bfiter_t bfend )
    {
      for (;; ++bfitr) {
        if (bfitr == bfend) {
          m_insnsizes.insert( std::max( cursize, minsize ) );
          return;
        }
      
        BitField const& bitfield = **bfitr;
        if (SeparatorBitField const* sbf = dynamic_cast<SeparatorBitField const*>( &bitfield ))
          {
            if (not sbf->rewind) { rwdsize = cursize; continue; }
            if (minsize < cursize) minsize = cursize;
            cursize = rwdsize;
          }
        uintptr_t count = bitfield.sizes();
        if (count < 1) throw std::logic_error( "empty instruction sizes list" );
        unsigned int bfsizes[count];
        bitfield.sizes( &bfsizes[0] );
      
        while (--count > 0)
          this->dig( cursize + bfsizes[count], rwdsize, minsize, bfitr + 1, bfend );
      
        cursize += bfsizes[0];
      }
    }
  
    OpProperties( Operation const& op )
    {
      Vector<BitField> const& bitfields = op.bitfields;
      dig( 0, 0, 0, bitfields.begin(), bitfields.end() );
    }
  };

}

Generator::Generator( Isa& _source, Opts const& _options )
  : source( _source ), options( _options ), m_minwordsize()
{
  // Actual minimal word size is determined by minimal word size
  // requirements from both command line and source code
  m_minwordsize = least_ctype_size( std::max( _options.minwordsize, source.m_minwordsize ) );
  
  { // change bitfield ordering if 
    bool rev_forder = source.m_asc_forder xor source.m_little_endian;
  
    if (source.m_asc_worder xor source.m_little_endian) {
      for( Vector<Operation>::iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op ) {
        Vector<BitField>& bitfields = (**op).bitfields;
        uintptr_t lo = 0, hi = bitfields.size();
        if (hi == 0) continue;
      
        for (hi -= 1; lo < hi; lo ++, hi-- ) {
          std::swap( bitfields[lo], bitfields[hi] );
        }
      }
      // this operation has invert fields order
      rev_forder = not rev_forder;
    }
  
    if (rev_forder) {
      for( Vector<Operation>::iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op ) {
        Vector<BitField>& bitfields = (**op).bitfields;
      
        uintptr_t fbeg = 0, fend = 0, fmax = bitfields.size();
        for ( ; fbeg < fmax; fbeg = fend = fend + 1) {
          while ((fend < fmax) and (not dynamic_cast<SeparatorBitField const*>( &*bitfields[fend] ))) fend += 1;
          if ((fend - fbeg) < 2) continue;
          uintptr_t lo = fbeg, hi = fend - 1;
        
          for ( ; lo < hi; lo ++, hi-- ) {
            std::swap( bitfields[lo], bitfields[hi] );
          }
        }
      }
    }
  }
  
  // Compute min/max sizes
  m_insnsizes.clear();
  for( Vector<Operation>::const_iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op ) {
    OpProperties opprops( **op );
    m_insnsizes.insert( opprops.m_insnsizes.begin(), opprops.m_insnsizes.end() );
  }
}

/**
 *  @brief computes the greatest common divisor of instruction lengths (in bits).
 */
unsigned int
Generator::gcd() const
{
  unsigned int res = *m_insnsizes.begin();
  for (std::set<unsigned int>::const_iterator itr = m_insnsizes.begin(); itr != m_insnsizes.end(); ++itr) {
    unsigned int cur = *itr;
    for (;;) {
      unsigned int rem = cur % res;
      if (rem == 0) break;
      cur = res;
      res = rem;
    }
  }
  return res;
}

/**
 *  @brief a null ouput stream useful for ignoring too verbose logs
 */
struct onullstream: public std::ostream
{
  struct nullstreambuf: public std::streambuf
  {
    int_type overflow( int_type c ) { return traits_type::not_eof(c); }
  };
  onullstream() : std::ostream(0) { rdbuf(&m_sbuf); }
  nullstreambuf m_sbuf;
};

/**
 *  @brief returns the output stream for a given level of verbosity
 */
std::ostream&
Generator::log( unsigned int level ) const
{
  if (level > options.verbosity) {
    // Return a null ostream
    static onullstream ons;
    return ons;
  }
  return std::cerr;
}

namespace {
  struct BelowScanner
  {
    typedef OpCode::BelowList BelowList;
    BelowList seen;
    OpCode* target;
    BelowScanner( OpCode* _target ) : target( _target ) {}
  
    bool findfrom( OpCode* start )
    {
      for (BelowList::const_iterator itr = start->m_belowlist.begin(), end = start->m_belowlist.end(); itr != end; ++itr) {
        if (not seen.insert( *itr ).second) continue; /* already seen */
        if ((*itr == target) or findfrom( *itr )) return true;
      }
      return false;
    }
  };
};

bool
OpCode::above( OpCode* below )
{
  return BelowScanner( below ).findfrom( this );
}


/*
 *  @brief update the topology; linking subject to argument opcode
 *  @param _below the object being linked to
 */
void
OpCode::setbelow( OpCode* _below )
{
  for (std::set<OpCode*>::iterator itr = m_belowlist.begin(), end = m_belowlist.end(); itr != end; ++itr) {
    if (*itr == _below) return;
    OpCode::location_t loc = _below->locate( **itr );
    if (loc == OpCode::Inside) return;
    if (loc != OpCode::Contains) continue;
    // Redundant  edge ... removing
    (**itr).m_abovecount -= 1;
    m_belowlist.erase( itr );
    // Normally no more than one redundant edge ... stopping here.
    break;
  }
  m_belowlist.insert( _below );
  _below->m_abovecount += 1;
}
  
/*
 *  @brief update the topology; linking subject to argument opcode
 *  @param _below the object being linked to
 */
void
OpCode::forcebelow( OpCode* _below )
{
  m_belowlist.insert( _below );
  _below->m_abovecount += 1;
}
  
/*
 *  @brief update the topology; unlink subject opcode
 */
void
OpCode::unsetbelow()
{
  for (std::set<OpCode*>::iterator itr = m_belowlist.begin(), end = m_belowlist.end(); itr != end; ++itr)
    (**itr).m_abovecount -= 1;
  m_belowlist.clear();
}

std::ostream& operator<<( std::ostream& _sink, OpCode const& _oc ) { return _oc.details( _sink ); }
  
/* Generates the topological graph of operations, checks for conflicts (overlapping encodings), and sort operations accordingly.
 */
void
Generator::toposort()
{
  Vector<Operation>& operations = source.m_operations;
  
  // Finalizing ordering graph
  
  // Adding implicit edges.
  for (OpCodeMap::iterator oitr1 = m_opcodes.begin(), oend1 = m_opcodes.end(); oitr1 != oend1; ++oitr1) {
    OpCode& opcode1( *oitr1->second );
    for (OpCodeMap::iterator oitr2 = m_opcodes.begin(); oitr2 != oitr1; ++oitr2) {
      OpCode& opcode2( *oitr2->second );
      switch (opcode1.locate( opcode2 )) {
      default: break;
      case OpCode::Equal:
        oitr1->first->fileloc.err( "error: operation `%s' duplicates operation `%s'", oitr1->first->symbol.str(), oitr2->first->symbol.str() );
        oitr2->first->fileloc.err( "operation `%s' was declared here", oitr2->first->symbol.str() );
        std::cerr << oitr1->first->symbol.str() << ": " << opcode1 << std::endl;
        std::cerr << oitr2->first->symbol.str() << ": " << opcode2 << std::endl;
        throw GenerationError;
        break;
      case OpCode::Contains: opcode1.setbelow( &opcode2 ); break;
      case OpCode::Inside:   opcode2.setbelow( &opcode1 ); break;
      }
    }
  }
  // Informing about implicit specialization 
  if (options.verbosity >= 2) {
    for (OpCodeMap::iterator oitr = m_opcodes.begin(), oend = m_opcodes.end(); oitr != oend; ++oitr) {
      OpCode& opc( *oitr->second );
      uintptr_t count = opc.m_belowlist.size();
      if (count == 0) continue;
      log(2) << "operation `" << opc.m_symbol.str() << "' is a specialization of operation" << (count>1?"s ":" ");
      char const* sep = "";
      for (OpCode::BelowList::iterator bitr = opc.m_belowlist.begin(), bend = opc.m_belowlist.end(); bitr != bend; ++bitr) {
        log(2) << sep << "`" << (**bitr).m_symbol.str() << "'";
        sep = ", ";
      }
      log(2) << std::endl;
    }
  }
  // Adding explicit edges (user specified)
  for (Isa::Orderings::iterator itr = source.m_user_orderings.begin(), end = source.m_user_orderings.end(); itr != end; ++itr) {
    // Unrolling specialization relations
    typedef Vector<Operation> OpV;
    
    struct : Isa::OOG { void with( Operation& operation ) { ops.append( &operation ); } OpV ops; } above, below;
    
    if (not source.for_ops( itr->top_op, above ))
      {
        itr->fileloc.loc( std::cerr ) << "error: no such operation or group `" << itr->top_op.str() << "'" << std::endl;
        throw GenerationError;
      }
    
    for (std::vector<ConstStr>::const_iterator symitr = itr->under_ops.begin(), symend = itr->under_ops.end(); symitr != symend; ++symitr)
      {
        if (not source.for_ops( *symitr, below ))
          {
            itr->fileloc.loc( std::cerr ) << "error: no such operation or group `" << symitr->str() << "'" << std::endl;
            throw GenerationError;
          }
      }
    
    // Check each user specialization and insert when valid
    for (OpV::iterator aoitr = above.ops.begin(), aoend = above.ops.end(); aoitr != aoend; ++aoitr) {
      OpCode& opcode1( opcode( *aoitr ) );
      for (OpV::iterator boitr = below.ops.begin(), boend = below.ops.end(); boitr != boend; ++boitr) {
        OpCode& opcode2( opcode( *boitr ) );
        switch (opcode1.locate( opcode2 )) {
        default: break;
        case OpCode::Outside:
          itr->fileloc.loc( log(2) ) << "warning: specializing `" << (**boitr).symbol.str() << "'"
                                     << " with `" << (**aoitr).symbol.str() << "' as no effect (no relationship).\n";
          break;
        case OpCode::Contains:
          itr->fileloc.loc( log(2) ) << "warning: specializing `" << (**boitr).symbol.str() << "'"
                                     << " with `" << (**aoitr).symbol.str() << "' as no effect (implicit specialization).\n";
          break;
        case OpCode::Inside:
          itr->fileloc.loc( std::cerr ) << "error: cant specialize `" << (**boitr).symbol.str() << "'"
                                        << " with `" << (**aoitr).symbol.str() << "' (would hide the former).\n";
          throw GenerationError;
          break;
        case OpCode::Overlaps:
          opcode1.forcebelow( &opcode2 );
          log(2) << "operation `" << (**aoitr).symbol.str() << "' is a forced specialization of operation `" << (**boitr).symbol.str() << "'" << std::endl;
          break;
        }          
      }
    }
  }
  
  // Finally check if overlaps are resolved
  for (OpCodeMap::iterator oitr1 = m_opcodes.begin(), oend1 = m_opcodes.end(); oitr1 != oend1; ++oitr1) {
    for (OpCodeMap::iterator oitr2 = m_opcodes.begin(); oitr2 != oitr1; ++oitr2) {
      if (oitr1->second->locate( *oitr2->second ) != OpCode::Overlaps) continue;
      if (oitr1->second->above( oitr2->second )) continue;
      if (oitr2->second->above( oitr1->second )) continue;
      oitr1->first->fileloc.err( "error: operation `%s' conflicts with operation `%s'", oitr1->first->symbol.str(), oitr2->first->symbol.str() );
      oitr2->first->fileloc.err( "operation `%s' was declared here", oitr2->first->symbol.str() );
      std::cerr << oitr1->first->symbol.str() << ": " << (*oitr1->second) << std::endl;
      std::cerr << oitr2->first->symbol.str() << ": " << (*oitr2->second) << std::endl;
      throw GenerationError;
    }
  }

  // Topological sort to fix potential precedence problems
  {
    intptr_t opcount = operations.size();
    Vector<Operation> noperations( opcount );
    intptr_t
      sopidx = opcount, // operation source table index
      dopidx = opcount, // operation destination table index
      inf_loop_tracker = opcount; // counter tracking infinite loop
    
    while( dopidx > 0 ) {
      sopidx = (sopidx + opcount - 1) % opcount;
      Operation* op = operations[sopidx];
      if (not op) continue;
      
      OpCode& oc = opcode( op );
      if (oc.m_abovecount > 0)
        {
          // There is some operations to be placed before this one 
          --inf_loop_tracker;
          assert( inf_loop_tracker >= 0 );
          continue;
        }
      inf_loop_tracker = opcount;
      noperations[--dopidx] = op;
      operations[sopidx] = 0;
      oc.unsetbelow();
    }
    operations = noperations;
  }
}

/** Dumps ISA statistics
    @param _sink a std::ostream reference where to dump statistics
    @param verbosity the level of verbosity
 */
void
Generator::isastats()
{
  log(1) << "Instruction Size: ";
  if (m_insnsizes.size() == 1)
    log(1) << (*m_insnsizes.begin());
  else
    {
      char const* sep = "[";
      for (std::set<unsigned int>::const_iterator itr = m_insnsizes.begin(); itr != m_insnsizes.end(); ++itr) {
        log(1) << sep << *itr;
        sep = ",";
      }
      log(1) << "] (gcd=" << this->gcd() << ")";
    }
  
  log(1) << std::endl;
  log(1) << "Instruction Set Encoding: " << (source.m_little_endian ? "little-endian" : "big-endian") << "\n";
  /* Statistics about operation and actions */
  log(1) << "Operation count: " << source.m_operations.size() << "\n";
  {
    log(3) << "Operations (actions details):\n";
    typedef std::map<ActionProto const*,uint64_t> ActionCount;
    ActionCount actioncount;
    for (Vector<Operation>::const_iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op) {
      log(3) << "  " << (**op).symbol.str() << ':';
      for (Vector<Action>::const_iterator action = (**op).actions.begin(); action < (**op).actions.end(); ++ action) {
        ActionProto const* ap = (**action).m_actionproto;
        log(3) << " ." << ap->m_symbol.str();
        actioncount[ap] += 1;
      }
      log(3) << '\n';
    }
    log(1) << "Action count:\n";
    for (ActionCount::const_iterator itr = actioncount.begin(); itr != actioncount.end(); ++itr) {
      log(1) << "   ." << itr->first->m_symbol.str() << ": " << itr->second << '\n';
    }
  }
}

/** Generates one C source file and one C header
    @param output a C string containing the name of the output filenames without the file name extension
    @param word_size define the minimum word size to hold the operand bit field,
    if zero uses the smallest type which hold the operand bit field
*/
void
Generator::iss() const
{
  /*******************/
  /*** Header file ***/
  /*******************/
  {
    FProduct sink( options.outputprefix, ".hh", options.sourcelines );
    if (not sink.good()) {
      std::cerr << options.appname() << ": can't open header file '" << sink.m_filename.str() << "'.\n";
      throw GenerationError;
    }
    
    sink.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", options.appname(), options.appversion() );
    
    ConstStr headerid;
    
    {
      std::string ns_header_str;
      std::string sep = "";
      for( std::vector<ConstStr>::const_iterator piece = source.m_namespace.begin(); piece < source.m_namespace.end(); ++ piece ) {
        ns_header_str += sep + (*piece).str();
        sep = "__";
      }
      headerid = Str::fmt( "__%s_%s_HH__", Str::tokenize( options.outputprefix ).str(), ns_header_str.c_str() );
    }

    sink.code( "#ifndef %s\n", headerid.str() );
    sink.code( "#define %s\n", headerid.str() );
  
    sink.code( "#ifndef __STDC_FORMAT_MACROS\n"
                   "#define __STDC_FORMAT_MACROS\n"
                   "#endif\n\n" );
  
    sink.code( "#include <vector>\n" );
    sink.code( "#include <inttypes.h>\n" );
    sink.code( "#include <cstring>\n" );
  
    //   sink.code( "#ifndef GCC_VERSION\n" );
    //   sink.code( "#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)\n" );
    //   sink.code( "#endif\n" );

    additional_decl_includes( sink );
  
    sink.ns_enter( source.m_namespace );
  
    codetype_decl( sink );
    decoder_decl( sink );
  
    sink.ns_leave( source.m_namespace );

    for( Vector<SourceCode>::const_iterator srccode = source.m_decl_srccodes.begin(); srccode < source.m_decl_srccodes.end(); ++ srccode ) {
      sink.usercode( **srccode );
    }
  
    sink.ns_enter( source.m_namespace );
  
    operation_decl( sink );
  
    sink.ns_leave( source.m_namespace );
  
    sink.code( "#endif\n" );
  
    sink.flush();
  }
  
  /*******************/
  /*** Source file ***/
  /*******************/
  {
    FProduct sink( options.outputprefix, source.m_tparams.empty() ? ".cc" : ".tcc", options.sourcelines );
    if (not sink.good()) {
      std::cerr << options.appname() << ": can't open header file '" << sink.m_filename.str() << "'.\n";
      throw GenerationError;
    }
  
    //  sink.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", options.appname(), options.appversion() );
  
    sink.code( "#include \"%s.hh\"\n", options.outputprefix );
    //   sink.code( "#include <cstdlib>\n"
    //                  "#include <cstring>\n\n\n\n" );
    additional_impl_includes( sink );
  
    for( Vector<SourceCode>::const_iterator srccode = source.m_impl_srccodes.begin(); srccode < source.m_impl_srccodes.end(); ++ srccode ) {
      sink.usercode( **srccode );
    }
  
    sink.ns_enter( source.m_namespace );
  
    codetype_impl( sink );
    operation_impl( sink );
  
    isa_operations_decl( sink );
    isa_operations_methods( sink );
    isa_operations_ctors( sink );
    if (source.m_withencode)
      isa_operations_encoders( sink );
  
    decoder_impl( sink );
  
    sink.ns_leave( source.m_namespace );
  
    sink.flush();
  }
  
  /******************************/
  /*** Subdecoder header file ***/
  /******************************/
  if (source.m_is_subdecoder) {
    FProduct sink( options.outputprefix, "_sub.isa", options.sourcelines );
    if (not sink.good()) {
      std::cerr << options.appname() << ": can't open header file '" << sink.m_filename.str() << "'.\n";
      throw GenerationError;
    }
    
    sink.code( "subdecoder " );
    
    std::vector<ConstStr> const& nmspc = source.m_namespace;
    char const* sep = "";
    for( std::vector<ConstStr>::const_iterator ns = nmspc.begin(); ns < nmspc.end(); sep = "::", ++ ns )
      sink.code( "%s%s", sep, (*ns).str() );
    
    sink.code( " [" );
    sep = "";
    for (std::set<unsigned int>::const_iterator itr = m_insnsizes.begin(); itr != m_insnsizes.end(); sep = ",", ++itr)
      sink.code( "%s%u", sep, *itr );
    
    sink.code( "]\n" );
    
    sink.flush();
  }
}

/** Returns a type format of the good bit size
    @param size a size in bits
    @param is_signed_type non-zero if the type is signed
    @return a C string containing the format for printf functions
*/
char const*
get_type_format( int size, int is_signed_type ) {
  if (is_signed_type) {
    if (size <= 8)  return "%\" PRIi8 \"";
    if (size <= 16) return "%\" PRIi16 \"";
    if (size <= 32) return "%\" PRIi32 \"";
    if (size <= 64) return "%\" PRIi64 \"";
  }
  else {
    if (size <= 8)  return "%\" PRIu8 \"";
    if (size <= 16) return "%\" PRIu16 \"";
    if (size <= 32) return "%\" PRIu32 \"";
    if (size <= 64) return "%\" PRIu64 \"";
  }
  return 0;
}

void
Generator::decoder_decl( Product& _product ) const {
  _product.template_signature( source.m_tparams );
  _product.code( "class Operation;\n" );
  
  if (source.m_withcache) {
    _product.code( "const unsigned int NUM_OPERATIONS_PER_PAGE = 4096;\n" );
    _product.template_signature( source.m_tparams );
    _product.code( "class DecodeMapPage\n" );
    _product.code( "{\n" );
    _product.code( "public:\n" );
    _product.code( " DecodeMapPage(%s key);\n", source.m_addrtype.str() );
    _product.code( " ~DecodeMapPage();\n" );
    _product.code( " %s key;\n", source.m_addrtype.str() );
    _product.code( " DecodeMapPage *next;\n" );
    _product.code( " Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *operation[NUM_OPERATIONS_PER_PAGE];\n" );
    _product.code( "};\n\n" );
  }

  _product.template_signature( source.m_tparams );
  _product.code( "class DecodeTableEntry\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( " DecodeTableEntry(%s opcode, %s opcode_mask, Operation", codetype_constref().str(), codetype_constref().str() );
  _product.template_abbrev( source.m_tparams );
  _product.code( " *(*decode)(%s, %s));\n", codetype_constref().str(), source.m_addrtype.str() );
  _product.code( " %s opcode;\n", codetype_name().str() );
  _product.code( " %s opcode_mask;\n", codetype_name().str() );
  _product.code( " Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( " *(*decode)(%s code, %s addr);\n", codetype_constref().str(), source.m_addrtype.str() );
  _product.code( "};\n" );

  _product.code( "const unsigned int NUM_DECODE_HASH_TABLE_ENTRIES = 4096;\n" );

  _product.template_signature( source.m_tparams );
  _product.code( "class Decoder\n" );
  _product.code( "{\n" );
  _product.code( "public:\n" );
  _product.code( " Decoder();\n" );
  _product.code( " virtual ~Decoder();\n" );
  _product.code( "\n" );
  if (not source.m_is_subdecoder) {
    _product.code( " Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "*NCDecode(%s addr);\n", source.m_addrtype.str() );
  }
  _product.code( " Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( " *NCDecode(%s addr, %s code);\n", source.m_addrtype.str(), codetype_constref().str() );
  if (source.m_withcache) {
    _product.code( " Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *Decode(%s addr, %s insn);\n", source.m_addrtype.str(), codetype_constref().str() );
    _product.code( " std::vector<DecodeTableEntry" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " > const& GetDecodeTable() const { return decode_table; };\n" );
    _product.code( " void InvalidateDecodingCacheEntry(%s addr);\n", source.m_addrtype.str() );
    _product.code( " void InvalidateDecodingCache();\n\n" );
  }
  _product.code( " void SetLittleEndian();\n" );
  _product.code( " void SetBigEndian();\n" );
  _product.code( options.privatemembers ? "private:\n" : "public:\n" );
  _product.code( " std::vector<DecodeTableEntry" );
  _product.template_abbrev( source.m_tparams );
  _product.code( " > decode_table;\n" );
  if (source.m_withcache) {
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *mru_page;\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *decode_hash_table[NUM_DECODE_HASH_TABLE_ENTRIES];\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *FindPage(%s page_key)\n", source.m_addrtype.str() );
    _product.code( "#if defined(__GNUC__) && (__GNUC__ >= 3 && (__GNUC__ != 3 || __GNUC_MINOR__ != 4 || __GNUC_PATCHLEVEL__ != 6))\n" );
    _product.code( " __attribute__((always_inline))\n" );
    _product.code( "#endif\n" );
    _product.code( " ;\n" );
  }
  _product.code( "};\n\n" );
  if (source.m_is_subdecoder) {
    if( not source.m_tparams.empty() ) {
      _product.template_signature( source.m_tparams );
    } else {
      _product.code( "inline\n" );
    }
    _product.code( "Operation *sub_decode(%s addr, %s code) {\n", source.m_addrtype.str(), codetype_constref().str() );
    _product.code( " static Decoder" );
    _product.template_abbrev( source.m_tparams );
    _product.code(" decoder;\n" );
    _product.code( " return decoder.NCDecode( addr, code );\n" );
    _product.code( "}\n" );
  }
}

void
Generator::operation_decl( Product& _product ) const {
  _product.template_signature( source.m_tparams );
  _product.code( "class Operation\n" );
  if( source.m_inheritances.size() > 0 ) {
    char const* sep = ": ";
    for( Vector<Inheritance>::const_iterator inh = source.m_inheritances.begin(); inh != source.m_inheritances.end(); ++inh ) {
      _product.code( sep ).usercode( *(**inh).modifier ).code( " " ).usercode( *(**inh).ctypename ).code( "\n" );
      sep = ", ";
    }
  }
  _product.code( "{\n" );
  _product.code( "public:\n" );

  _product.code( " Operation(%s code, %s addr, const char *name);\n", codetype_constref().str(), source.m_addrtype.str() );
  _product.code( " virtual ~Operation();\n" );
  if (source.m_withencode)
    _product.code( " virtual void Encode(%s code) const {}\n", codetype_ref().str() );
  _product.code( " inline %s GetAddr() const { return addr; }\n", source.m_addrtype.str() );
  _product.code( " inline void SetAddr(%s _addr) { this->addr = _addr; }\n", source.m_addrtype.str() );
  _product.code( " inline %s GetEncoding() const { return encoding; }\n", codetype_constref().str() );
  op_getlen_decl( _product );
  _product.code( " inline const char *GetName() const { return name; }\n" );
  
  _product.code( " static unsigned int const minsize = %d;\n", (*m_insnsizes.begin()) );
  _product.code( " static unsigned int const maxsize = %d;\n", (*m_insnsizes.rbegin()) );
  
  for( Vector<Variable>::const_iterator var = source.m_vars.begin(); var < source.m_vars.end(); ++ var ) {
    _product.usercode( (**var).ctype->fileloc, " %s %s;", (**var).ctype->content.str(), (**var).symbol.str() );
  }
  
  for( intptr_t idx = source.m_actionprotos.size(); (--idx) >= 0; ) {
    if (source.m_actionprotos[idx]->m_type != ActionProto::Static or not source.m_actionprotos[idx]->m_returns) continue;
    _product.code( " %s %s_result;\n", source.m_actionprotos[idx]->m_returns->content.str(), source.m_actionprotos[idx]->m_symbol.str() );
  }
  
  if (source.m_withsource) {
    /* base declaration for internal encoding and decoding code
     * introspection methods */
    if (source.m_withencode)
      _product.code( "virtual char const* Encode_text() const;\n" );
    _product.code( "virtual char const* Decode_text() const;\n" );
  }
  
  for( intptr_t idx = source.m_actionprotos.size(); (--idx) >= 0; ) {
    if (source.m_withsource) {
      /* base declaration for user-defined code introspection
       * methods */
      _product.code( " virtual char const* %s_text() const;\n", source.m_actionprotos[idx]->m_symbol.str() );
    }
    _product.code( " virtual " );
    if (source.m_actionprotos[idx]->m_returns) {
      _product.usercode( *(source.m_actionprotos[idx]->m_returns) );
    } else {
      _product.code( "void" );
    }

    _product.code( " %s(", source.m_actionprotos[idx]->m_symbol.str() );

    if( not source.m_actionprotos[idx]->m_params.empty() ) {
      char const* sep = " ";
      for( Vector<CodePair>::const_iterator param = source.m_actionprotos[idx]->m_params.begin();
           param < source.m_actionprotos[idx]->m_params.end(); ++ param, sep = ",\n" )
        {
          _product.code( sep ).usercode( *(**param).ctype ).code( " " ).usercode( *(**param).csymbol );
        }
    }

    _product.code( " )%s;\n", (source.m_actionprotos[idx]->m_constness ? " const" : "") );
  }

  _product.code( "protected:\n" );

  _product.code( " %s encoding;\n", codetype_name().str() );
  _product.code( " %s addr;\n", source.m_addrtype.str() );
  _product.code( " const char *name;\n" );

  _product.code( "};\n\n" );
}

unsigned
Generator::membersize( unsigned size ) const
{
  // Operand's C type size is determined by 1/ the least C type
  // required by the ISA operand itself and 2/ the minimal C type
  // required for all operands.
  return std::max( least_ctype_size( size ), m_minwordsize );
}

void
Generator::isa_operations_decl( Product& _product ) const
{
  for( Vector<Operation>::const_iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op ) {
    _product.template_signature( source.m_tparams );
    _product.code( "class Op%s : public Operation", Str::capitalize( (**op).symbol.str() ).str() );
    _product.template_abbrev( source.m_tparams );
    _product.code( "\n" );

    _product.code( "{\n" );
    _product.code( "public:\n" );
    _product.code( " Op%s(%s code, %s addr);\n", Str::capitalize( (**op).symbol.str() ).str(),
                   codetype_constref().str(), source.m_addrtype.str() );
    insn_destructor_decl( _product, **op );
    insn_getlen_decl( _product, **op );
    if (source.m_withencode)
      _product.code( " void Encode(%s code) const;\n", codetype_ref().str() );
    
    for( Vector<BitField>::const_iterator bf = (**op).bitfields.begin(); bf < (**op).bitfields.end(); ++ bf )
      {
        if      (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &**bf ))
          {
            _product.code( "%sint%d_t %s;\n", (opbf->sext ? "" : "u"), membersize( *opbf ), opbf->symbol.str() );
          }
        else if (SpOperandBitField const* sopbf = dynamic_cast<SpOperandBitField const*>( &**bf ))
          {
            _product.code( "static %sint%d_t const %s = %s;\n",
                           (sopbf->sext ? "" : "u"), membersize( *sopbf ), sopbf->symbol.str(), sopbf->constval().str() );
          }
        else if (SubOpBitField const* sobf = dynamic_cast<SubOpBitField const*>( &**bf ))
          {
            SDInstance const* sdinstance = sobf->sdinstance;
            SDClass const* sdclass = sdinstance->m_sdclass;
            SourceCode const* tpscheme =  sdinstance->m_template_scheme;
        
            _product.usercode( sdclass->m_fileloc, " %s::Operation", sdclass->qd_namespace().str() );
            if (tpscheme)
              _product.usercode( *tpscheme, "< %s >" );
            _product.code( "* %s;\n", sobf->symbol.str() );
          }
      }

    if (not (**op).variables.empty())
      {
        for (Vector<Variable>::const_iterator var = (**op).variables.begin(); var < (**op).variables.end(); ++ var)
          _product.usercode( (**var).ctype->fileloc, "   %s %s;", (**var).ctype->content.str(), (**var).symbol.str() );
      }
    
    if (source.m_withsource) {
      /* insn declaration for internal encoding and decoding code
       * introspection methods */
      if (source.m_withencode)
        _product.code( "char const* Encode_text() const;\n" );
      _product.code( "char const* Decode_text() const;\n" );
    }


    for( Vector<Action>::const_iterator action = (**op).actions.begin(); action < (**op).actions.end(); ++ action ) {
      ActionProto const* actionproto = (**action).m_actionproto;

      if (options.comments)
      {
        for (Vector<Comment>::const_iterator comm = actionproto->m_comments.begin(); comm < actionproto->m_comments.end(); ++ comm)
          _product.code( " %s\n", (**comm).content.str() );
      }
      
      if (source.m_withsource) {
        // for cstring version of the action
        _product.code( " virtual char const* %s_text() const;\n", actionproto->m_symbol.str() );
      }

      _product.code( " virtual\n " );

      if (actionproto->m_returns) {
        _product.usercode( *actionproto->m_returns );
      } else {
        _product.code( "void\n" );
      }

      _product.code( " %s(", actionproto->m_symbol.str() );

      if( not actionproto->m_params.empty() ) {
        char const* sep = " ";
        for( Vector<CodePair>::const_iterator param = actionproto->m_params.begin();
             param < actionproto->m_params.end(); ++ param, sep = ",\n" )
          {
            _product.code( sep ).usercode( *(**param).ctype ).code( " " ).usercode( *(**param).csymbol );
          }
        _product.code( " " );
      }

      _product.code( ")%s;\n", (actionproto->m_constness ? " const" : "") );
    }

    _product.code( options.privatemembers ? "private:\n" : "public:\n" );

    _product.code( "};\n\n" );
  }
}

void
Generator::operation_impl( Product& _product ) const {
  _product.template_signature( source.m_tparams );
  _product.code( "Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::Operation(%s _code, %s _addr, const char *_name)\n", codetype_constref().str(), source.m_addrtype.str() );
  _product.code( ": \n");
  
  for( Vector<Inheritance>::const_iterator inh = source.m_inheritances.begin(); inh != source.m_inheritances.end(); ++ inh ) {
    if (not (**inh).initargs) continue;
    _product.usercode( *(**inh).ctypename ).code( "( " ).usercode( *(**inh).initargs ).code( " ),\n" );
  }

  for( Vector<Variable>::const_iterator var = source.m_vars.begin(); var < source.m_vars.end(); ++ var ) {
    if (not (**var).cinit) continue;
    _product.code( " %s(", (**var).symbol.str() ).usercode( *(**var).cinit ).code( "),\n" );
  }

  _product.code( " encoding(_code),\n" );
  _product.code( " addr(_addr),\n" );
  _product.code( " name(_name)\n" );
  _product.code( "{\n" );
  _product.code( "}\n\n" );
  _product.template_signature( source.m_tparams );
  _product.code( "Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::~Operation()\n" );
  _product.code( "{\n" );
  
  for( intptr_t idx = source.m_actionprotos.size(); (--idx) >= 0; ) {
    if (source.m_actionprotos[idx]->m_type != ActionProto::Destructor) continue;
    _product.code( " %s();\n", source.m_actionprotos[idx]->m_symbol.str() );
  }
  _product.code( "}\n\n" );

  if (source.m_withsource) {
    /* base implemantation for internal encoding and decoding code
     * introspection methods */
    char const* xxcode[2] = {"Encode", "Decode"};
    for (int step = 0; step < 2; ++step) {
      if (step == 0 and not source.m_withencode) continue;
      _product.template_signature( source.m_tparams );
      _product.code( " char const* Operation" );
      _product.template_abbrev( source.m_tparams );
      _product.code( "::%s_text() const { return\"\"; }\n", xxcode[step] );
    }
  }
  
  for( intptr_t idx = source.m_actionprotos.size(); (--idx) >= 0; ) {
    if (source.m_withsource) {
      // for cstring version of the method
      _product.template_signature( source.m_tparams );
      _product.code( " char const* Operation" );
      _product.template_abbrev( source.m_tparams );
      _product.code( "::%s_text() const\n", source.m_actionprotos[idx]->m_symbol.str() );
      _product.code( " { return %s; }", Str::dqcstring( source.m_actionprotos[idx]->m_defaultcode->content.str() ).str() );
    }
    
    _product.template_signature( source.m_tparams );

    if(source.m_actionprotos[idx]->m_returns) {
      _product.usercode( *(source.m_actionprotos[idx]->m_returns) );
    }
    else {
      _product.code( "\nvoid\n" );
    }

    _product.code( " Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::%s(", source.m_actionprotos[idx]->m_symbol.str() );

    if( not source.m_actionprotos[idx]->m_params.empty() ) {
      char const* sep = " ";
      for( Vector<CodePair>::const_iterator param = source.m_actionprotos[idx]->m_params.begin();
           param < source.m_actionprotos[idx]->m_params.end(); ++ param, sep = ",\n" )
        {
          _product.code( sep ).usercode( *(**param).ctype ).code( " " ).usercode( *(**param).csymbol );
        }
    }

    _product.code( ")%s\n{\n", (source.m_actionprotos[idx]->m_constness ? " const" : "") );
    _product.usercode( *source.m_actionprotos[idx]->m_defaultcode, "{%s}\n" );
    _product.code( "}\n" );
  }
}

void
Generator::isa_operations_methods( Product& _product ) const
{
  for (Vector<Operation>::const_iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op)
  {
    if (options.comments)
    {
      for (Vector<Comment>::const_iterator comm = (**op).comments.begin(); comm < (**op).comments.end(); ++ comm)
        _product.code( "%s\n", (**comm).content.str() );
    }
    
    if (source.m_withsource) {
      /* insn implementation for internal encoding and decoding code
       * introspection methods */
      char const* xxcode[2] = {"Encode", "Decode"};
      for (int step = 0; step < 2; ++step) {
        if (step == 0 and not source.m_withencode) continue;
        SProduct xxcode_text( _product.m_filename, options.sourcelines );
        switch (step) {
        case 0: insn_decode_impl( xxcode_text, **op, "code", "addr" ); break;
        case 1: insn_encode_impl( xxcode_text, **op, "code" ); break;
        }
        _product.template_signature( source.m_tparams );
        _product.code( " char const* Op%s", Str::capitalize( (**op).symbol.str() ).str() );
        _product.template_abbrev( source.m_tparams );
        _product.code( "::%s_text() const\n", xxcode[step] );
        _product.code( " { return %s; }\n", Str::dqcstring( xxcode_text.m_content.c_str() ).str() );
      }
    }
    
    for (Vector<Action>::const_iterator action = (**op).actions.begin(); action < (**op).actions.end(); ++ action) {
      ActionProto const* actionproto = (**action).m_actionproto;

      if (options.comments)
      {
        for (Vector<Comment>::const_iterator comm = (**action).m_comments.begin(); comm < (**action).m_comments.end(); ++ comm)
          _product.code( "%s\n", (**comm).content.str() );
      }

      if (source.m_withsource) {
        // for cstring version of the method
        _product.template_signature( source.m_tparams );
        _product.code( " char const* Op%s", Str::capitalize( (**op).symbol.str() ).str() );
        _product.template_abbrev( source.m_tparams );
        _product.code( "::%s_text() const\n", actionproto->m_symbol.str() );
        _product.code( " { return %s; }", Str::dqcstring( (**action).m_source_code->content.str() ).str() );
      }

      _product.template_signature( source.m_tparams );

      if (actionproto->m_returns) {
        _product.usercode( *actionproto->m_returns );
      }
      else {
        _product.code( "\nvoid\n" );
      }

      _product.code( " Op%s", Str::capitalize( (**op).symbol.str() ).str() );
      _product.template_abbrev( source.m_tparams );
      _product.code( "::%s(", actionproto->m_symbol.str() );

      if( not actionproto->m_params.empty() ) {
        char const* sep = " ";
        for( Vector<CodePair>::const_iterator param = actionproto->m_params.begin();
             param < actionproto->m_params.end(); ++ param, sep = ",\n" )
          {
            _product.code( sep ).usercode( *(**param).ctype ).code( " " ).usercode( *(**param).csymbol );
          }
      }

      _product.code( ")%s\n{\n", (actionproto->m_constness ? " const" : "") );
      _product.usercode( *(**action).m_source_code, "{%s}\n" );
      _product.code( "}\n" );
    }

    _product.code( "\n" );

    _product.template_signature( source.m_tparams );
    _product.code( "static Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *DecodeOp%s(", Str::capitalize( (**op).symbol.str() ).str() );
    _product.code( "%s code, %s addr)\n", codetype_constref().str(), source.m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " return new Op%s", Str::capitalize( (**op).symbol.str() ).str() );
    _product.template_abbrev( source.m_tparams );
    _product.code( "(code, addr);\n" );
    _product.code( "}\n\n" );
  }
}

void
Generator::isa_operations_ctors( Product& _product ) const {
  for( Vector<Operation>::const_iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op ) {
    _product.template_signature( source.m_tparams );
    _product.code( "Op%s", Str::capitalize( (**op).symbol.str() ).str() );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::Op%s(%s code, %s addr) : Operation", Str::capitalize( (**op).symbol.str() ).str(),
                   codetype_constref().str(), source.m_addrtype.str() );
    _product.template_abbrev( source.m_tparams );
    _product.code( "(code, addr, \"%s\")\n",
                   (**op).symbol.str() );

    _product.code( "{\n" );
    
    insn_decode_impl( _product, **op, "code", "addr" );

    if (not (**op).variables.empty()) {
      for( Vector<Variable>::const_iterator var = (**op).variables.begin(); var < (**op).variables.end(); ++ var ) {
        if ((**var).cinit) {
          _product.code( "%s = ", (**var).symbol.str() ).usercode( *(**var).cinit ).code( ";\n" );
        }
      }
    }

    _product.code( "}\n\n" );
    
    insn_destructor_impl( _product, **op );
  }
}

void
Generator::isa_operations_encoders( Product& _product ) const {
  for( Vector<Operation>::const_iterator op = source.m_operations.begin(); op < source.m_operations.end(); ++ op ) {
    _product.template_signature( source.m_tparams );
    _product.code( "void Op%s", Str::capitalize( (**op).symbol.str() ).str() );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::Encode(%s code) const\n", codetype_ref().str() );

    _product.code( "{\n" );
    
    insn_encode_impl( _product, **op, "code" );

    _product.code( "}\n\n" );
  }
}

void
Generator::decoder_impl( Product& _product ) const {
  if (source.m_withcache) {
    _product.template_signature( source.m_tparams );
    _product.code( "DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::DecodeMapPage(%s key)\n", source.m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " this->key = key;\n" );
    _product.code( " memset(operation, 0, sizeof(operation));\n" );
    _product.code( " next = 0;\n" );
    _product.code( "}\n\n" );
    
    _product.template_signature( source.m_tparams );
    _product.code( "DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::~DecodeMapPage()\n" );
    _product.code( "{\n" );
    _product.code( " unsigned int idx;\n" );
    _product.code( " for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)\n" );
    _product.code( "  delete operation[idx];\n" );
    _product.code( "}\n\n" );
  }
  _product.template_signature( source.m_tparams );
  _product.code( " DecodeTableEntry" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::DecodeTableEntry(%s opcode, %s opcode_mask, Operation", codetype_constref().str(), codetype_constref().str() );
  _product.template_abbrev( source.m_tparams );
  _product.code( " *(*decode)(%s, %s))\n", codetype_constref().str(), source.m_addrtype.str() );
  _product.code( " {\n" );
  _product.code( "  this->opcode = opcode;\n" );
  _product.code( "  this->opcode_mask = opcode_mask;\n" );
  _product.code( "  this->decode = decode;\n" );
  _product.code( " }\n\n" );
  
  
  _product.template_signature( source.m_tparams );
  _product.code( "Decoder" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::Decoder()\n" );
  char const* member_init_separator = ": ";
  if (source.m_withcache)
    {
      _product.code( "%smru_page( 0 )", member_init_separator );
      member_init_separator = ", ";
    }
  _product.code( "\n{\n" );
  if (source.m_withcache)
    _product.code( " memset(decode_hash_table, 0, sizeof(decode_hash_table));\n" );
  
  for( Vector<Operation>::const_reverse_iterator op = source.m_operations.rbegin(); op < source.m_operations.rend(); ++ op ) {
    if( (**op).condition ) {
      _product.code( "if(" ).usercode( *(**op).condition ).code( ")" );
    }
    _product.code( " decode_table.push_back(DecodeTableEntry" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "(" );
    insn_bits_code( _product, **op );
    _product.code( ", " );
    insn_mask_code( _product, **op );
    _product.code( ", DecodeOp%s", Str::capitalize( (**op).symbol.str() ).str() );
    _product.template_abbrev( source.m_tparams );
    _product.code( "));\n" );
  }
  _product.code( "}\n\n" );

  _product.template_signature( source.m_tparams );
  _product.code( "Decoder" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::~Decoder()\n" );
  _product.code( "{\n" );
  if (source.m_withcache) {
    _product.code( " InvalidateDecodingCache();\n" );
  }
  _product.code( "}\n\n" );
  
  /*** NCDecode( Address_t addr, CodeType_t code ) ***/
  _product.template_signature( source.m_tparams );
  _product.code( "Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( " *Decoder" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::NCDecode(%s addr, %s code)\n", source.m_addrtype.str(), codetype_constref().str() );
  _product.code( "{\n" );
  _product.code( " Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( " *operation;\n" );
  _product.code( " unsigned int count = decode_table.size();\n" );
  _product.code( " unsigned int idx;\n" );
  _product.code( " for(idx = 0; idx < count; idx++)\n" );
  _product.code( " {\n" );
  insn_match_ifexpr( _product, "code", "decode_table[idx].opcode_mask", "decode_table[idx].opcode" );
  _product.code( "  {\n" );
  _product.code( "   operation = decode_table[idx].decode(code, addr);\n" );

  for( intptr_t idx = source.m_actionprotos.size();  (--idx) >= 0; ) {
    switch( source.m_actionprotos[idx]->m_type ) {
    case ActionProto::Constructor:
      _product.code( "   operation->%s();\n", source.m_actionprotos[idx]->m_symbol.str() );
      break;
        
    case ActionProto::Static:
      _product.code( "   " );
      if (source.m_actionprotos[idx]->m_returns)
        _product.code( "operation->%s_result = ", source.m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", source.m_actionprotos[idx]->m_symbol.str() );
      break;
        
    default: break;
    }
  }
    
  _product.code( "   return operation;\n" );
  _product.code( "  }\n" );
  _product.code( " }\n" );
  
  _product.code( " operation = new Operation" );
  _product.template_abbrev( source.m_tparams );
  _product.code( "(code, addr, \"???\");\n" );
    
    
  for( intptr_t idx = source.m_actionprotos.size();  (--idx) >= 0; ) {
    switch( source.m_actionprotos[idx]->m_type ) {
    case ActionProto::Constructor:
      _product.code( " operation->%s();\n", source.m_actionprotos[idx]->m_symbol.str() );
      break;
    case ActionProto::Static:
      _product.code( " " );
      if (source.m_actionprotos[idx]->m_returns)
        _product.code( "operation->%s_result = ", source.m_actionprotos[idx]->m_symbol.str() );
      _product.code( "operation->%s();\n", source.m_actionprotos[idx]->m_symbol.str() );
      break;
    default: break;
    }
  }
    
  _product.code( " return operation;\n" );
  _product.code( "}\n\n" );
  
  if (source.m_withcache) {
    /*** InvalidateDecodingCache() ***/
    _product.template_signature( source.m_tparams );
    _product.code( "void Decoder" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::InvalidateDecodingCache()\n" );
    _product.code( "{\n" );
    _product.code( " uint32_t index;\n" );
    _product.code( " mru_page = 0;\n" );
    _product.code( " for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)\n" );
    _product.code( " {\n" );
    _product.code( "  DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *page, *next_page;\n" );

    _product.code( "  page = decode_hash_table[index];\n" );
    _product.code( "  if(page)\n" );
    _product.code( "  {\n" );
    _product.code( "   do\n" );
    _product.code( "   {\n" );
    _product.code( "   next_page = page->next;\n" );
    _product.code( "   delete page;\n" );
    _product.code( "   page = next_page;\n" );
    _product.code( "   } while(page);\n" );
    _product.code( "  decode_hash_table[index] = 0;\n" );
    _product.code( "  }\n" );
    _product.code( " }\n" );
    _product.code( "}\n\n" );
  
    ConstStr instid = insn_id_expr( "addr" );

    /*** InvalidateDecodingCacheEntry( Address_t addr ) ***/
    _product.template_signature( source.m_tparams );
    _product.code( "void Decoder" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::InvalidateDecodingCacheEntry(%s addr)\n", source.m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " %s page_key = %s / NUM_OPERATIONS_PER_PAGE;\n", source.m_addrtype.str(), instid.str() );
    _product.code( " if(mru_page && mru_page->key == page_key) mru_page = 0;\n" );
    _product.code( " uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *prev, *cur;\n" );
    _product.code( " cur = decode_hash_table[index];\n" );
    _product.code( " if(cur)\n" );
    _product.code( " {\n" );
    _product.code( "  if(cur->key == page_key)\n" );
    _product.code( "  {\n" );
    _product.code( "   decode_hash_table[index] = cur->next;\n" );
    _product.code( "   delete cur;\n" );
    _product.code( "   return;\n" );
    _product.code( "  }\n" );
    _product.code( "  prev = cur;\n" );
    _product.code( "  cur = cur->next;\n" );
    _product.code( "  if(cur)\n" );
    _product.code( "  {\n" );
    _product.code( "   do\n" );
    _product.code( "   {\n" );
    _product.code( "    if(cur->key == page_key)\n" );
    _product.code( "    {\n" );
    _product.code( "     prev->next = cur->next;\n" );
    _product.code( "     cur->next = 0;\n" );
    _product.code( "     delete cur;\n" );
    _product.code( "     return;\n" );
    _product.code( "    }\n" );
    _product.code( "    prev = cur;\n" );
    _product.code( "   } while((cur = cur->next) != 0);\n" );
    _product.code( "  }\n" );
    _product.code( " }\n" );
    _product.code( "}\n\n" );

    /*** FindPage( Address_t page_key ) ***/
    _product.template_signature( source.m_tparams );
    _product.code( "inline DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *Decoder" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::FindPage(%s page_key)\n", source.m_addrtype.str() );
    _product.code( "{\n" );
    _product.code( " if(mru_page && mru_page->key == page_key) return mru_page;\n" );
    _product.code( " uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *prev, *cur;\n" );
    _product.code( " cur = decode_hash_table[index];\n" );
    _product.code( " if(cur)\n" );
    _product.code( " {\n" );
    _product.code( "  if(cur->key == page_key)\n" );
    _product.code( "  {\n" );
    _product.code( "   mru_page = cur;\n" );
    _product.code( "   return cur;\n" );
    _product.code( "  }\n" );
    _product.code( "  prev = cur;\n" );
    _product.code( "  cur = cur->next;\n" );
    _product.code( "  if(cur)\n" );
    _product.code( "  {\n" );
    _product.code( "   do\n" );
    _product.code( "   {\n" );
    _product.code( "    if(cur->key == page_key)\n" );
    _product.code( "    {\n" );
    _product.code( "     prev->next = cur->next;\n" );
    _product.code( "     cur->next= decode_hash_table[index];\n" );
    _product.code( "     decode_hash_table[index] = cur;\n" );
    _product.code( "     mru_page = cur;\n" );
    _product.code( "     return cur;\n" );
    _product.code( "    }\n" );
    _product.code( "    prev = cur;\n" );
    _product.code( "   } while((cur = cur->next) != 0);\n" );
    _product.code( "  }\n" );
    _product.code( " }\n" );
    _product.code( " return 0;\n" );
    _product.code( "}\n\n" );

    /*** Decode( Address_t addr, CodeType_t const& insn ) ***/
    _product.template_signature( source.m_tparams );
    _product.code( "Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *Decoder" );
    _product.template_abbrev( source.m_tparams );
    _product.code( "::Decode(%s addr, %s insn)\n", source.m_addrtype.str(), codetype_constref().str() );
    _product.code( "{\n" );
    _product.code( " Operation" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *operation;\n" );
    _product.code( " %s page_key = %s / NUM_OPERATIONS_PER_PAGE;\n", source.m_addrtype.str(), instid.str() );
    _product.code( " DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " *page;\n" );
    _product.code( " page = FindPage(page_key);\n" );
    _product.code( " if(!page)\n" );
    _product.code( " {\n" );
    // _product.code( "   fprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n" );
    _product.code( "  page = new DecodeMapPage" );
    _product.template_abbrev( source.m_tparams );
    _product.code( " (page_key);\n" );
    _product.code( "  uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" );
    _product.code( "  page->next = decode_hash_table[index];\n" );
    _product.code( "  decode_hash_table[index] = page;\n" );
    _product.code( "  mru_page = page;\n" );
    _product.code( " }\n" );
    _product.code( " operation = page->operation[(%s) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", instid.str() );
    _product.code( " if(operation)\n" );
    _product.code( " {\n" );
    // _product.code( "  fprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n" );
    _product.code( "  if(");
    insn_unchanged_expr( _product, "operation->GetEncoding()", "insn" );
    _product.code( " && operation->GetAddr() == addr)\n" );
    _product.code( "   return operation;\n" );
    _product.code( "  delete operation;\n" );
    _product.code( " }\n" );
    // _product.code( " fprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n" );
    _product.code( " operation = NCDecode(addr, insn);\n" );
    _product.code( " page->operation[(%s) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", instid.str() );
    _product.code( " return operation;\n" );
    _product.code( "}\n\n" );
  }
}

unsigned int
Generator::least_ctype_size( unsigned int bits ) {
  if (bits > 64) throw GenerationError;
  unsigned int size = 8;
  while (size < bits) size *= 2;
  return size;
}

FieldIterator::FieldIterator( bool little_endian, Vector<BitField> const& bitfields, unsigned int maxsize )
  : m_bitfields( bitfields ), m_idx( (unsigned int)(-1) ),
    m_ref( little_endian ? 0 : maxsize ),
    m_pos( m_ref ), m_size( 0 ),
    m_chkpt_pos( m_pos ), m_chkpt_size( m_size )
{}
  
BitField const& FieldIterator::item() { return *(m_bitfields[m_idx]); }

bool
FieldIterator::next() {
  if ((m_idx+1) >= m_bitfields.size()) return false;
  m_idx += 1;
  BitField const& field = *(m_bitfields[m_idx]);
  if (m_ref == 0) m_pos += m_size;
  else            m_pos -= field.maxsize();
  m_size = field.maxsize();
  if (SeparatorBitField const* sbf = dynamic_cast<SeparatorBitField const*>( &field )) {
    if (sbf->rewind) { m_pos = m_chkpt_pos; m_size = m_chkpt_size; }
    else             { m_chkpt_pos = m_pos; m_chkpt_size = m_size; }
  }
  
  return true;
}

OpCode const&
Generator::opcode( Operation const* _op ) const
{
  OpCodeMap::const_iterator res = m_opcodes.find( _op );
  assert( res != m_opcodes.end() );
  return *res->second;
}

OpCode&
Generator::opcode( Operation const* _op )
{
  OpCodeMap::iterator res = m_opcodes.find( _op );
  assert( res != m_opcodes.end() );
  return *res->second;
}

