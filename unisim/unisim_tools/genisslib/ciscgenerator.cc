/***************************************************************************
                              ciscgenerator.cc
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

#include <ciscgenerator.hh>
#include <product.hh>
#include <isa.hh>
#include <scanner.hh>
#include <operation.hh>
#include <variable.hh>
#include <sourcecode.hh>
#include <action.hh>
#include <strtools.hh>
#include <bitfield.hh>
#include <comment.hh>
#include <subdecoder.hh>
#include <conststr.hh>
#include <iostream>
#include <cassert>
#include <cstring>
#include <limits>

/**
 *  @brief  Default constructor, create a CiscGenerator.
 */
CiscGenerator::CiscGenerator( Isa& _source, Opts const& _options )
  : Generator( _source, _options ), m_code_capacity( 0 )
{};

CiscGenerator::~CiscGenerator()
{
  for (OpCodeMap::iterator current = m_opcodes.begin(), stop = m_opcodes.end(); current != stop; ++current)
    {
      delete current->second;
    }
}

/** Base constructor, sets the size of the OpCode and allocates mask and bits buffers
*/
CiscOpCode::CiscOpCode( ConstStr _symbol, unsigned int _prefixsize, unsigned int _fullsize, bool _vlen )
  : OpCode( _symbol ), m_mask( 0 ), m_bits( 0 ), m_prefixsize( _prefixsize ), m_fullsize( _fullsize ), m_vlen( _vlen )
{
  uintptr_t mbsz = maskbytesize();
  m_mask = new uint8_t[mbsz*2];
  m_bits = &m_mask[mbsz];
  memset( m_mask, 0, mbsz*2 );
}

/** Optimize the size of OpCode object: upper zero-mask bits are stripped. */
void
CiscOpCode::optimize( bool is_little_endian ) {
  unsigned int stripped_size = m_prefixsize;
  if (is_little_endian) {
    for (; stripped_size > 0; stripped_size -= 1) {
      unsigned int byte = (stripped_size-1)/8, bit = (stripped_size-1)%8;
      if (((m_mask[byte] >> bit) & 1) != 0) break;
    }
  } else {
    for (; stripped_size > 0; stripped_size -= 1) {
      unsigned int byte = (stripped_size-1)/8, bit = (-stripped_size)%8;
      if (((m_mask[byte] >> bit) & 1) != 0) break;
    }
  }
  m_prefixsize = stripped_size;
}

/**
 *  @brief  Locate argument opcode from the subject opcode point of view.
 *  @param  _that the opcode to locate
 *  @return the argument object location
 *
 *  This function will return the argument opcode location according
 *  to the subject opcode location. Relative locations may be:
 *    - Outside
 *    - Overlaps
 *    - Inside
 *    - Contains
 *    - Equal
 */

OpCode::location_t
CiscOpCode::locate( OpCode const& _oc ) const
{
  CiscOpCode const& _that = dynamic_cast<CiscOpCode const&>( _oc );
  /* this is named A and _that is named B */
  unsigned int abytesize = maskbytesize(), bbytesize = _that.maskbytesize(), maxsize = std::max( abytesize, bbytesize );
  bool some_a_outside_B = false, some_b_outside_A = false;

  for( unsigned int idx = 0; idx < maxsize; ++ idx ) {
    uint8_t amask, abits, bmask, bbits;
    if (idx < abytesize) { amask = this->m_mask[idx]; abits = this->m_bits[idx]; } else { amask = 0; abits = 0; }
    if (idx < bbytesize) { bmask = _that.m_mask[idx]; bbits = _that.m_bits[idx]; } else { bmask = 0; bbits = 0; }
    
    if( (((amask) & (bmask)) & ((abits) ^ (bbits))) != 0 ) return Outside;
    if( ~(amask) & (bmask) ) some_a_outside_B = true;
    if( (amask) & ~(bmask) ) some_b_outside_A = true;
  }
  
  location_t table[] = {Equal, Inside, Contains, Overlaps};
  
  return table[(some_a_outside_B ? 1 : 0) | (some_b_outside_A ? 2 : 0)];
}

/**  @brief matches (or not) OpCode bytes according to the OpCode instance
 *   @param _oc the opcode which bytes are to be matched by the OpCode instance
 *   @return true on successful match
 */
bool
CiscOpCode::match( CiscOpCode const& _oc ) const {
  unsigned int bytesize = std::min( maskbytesize(), _oc.maskbytesize() );
  for( unsigned int byte = 0; byte < bytesize; ++byte ) {
    if( (_oc.m_bits[byte] & m_mask[byte]) != m_bits[byte] )
      return false;
  }
  return true;
}

/** Logs the content of the OpCode to an ostream
    @param _sink the ostream where the OpCode is logged.
    @return a reference to the ostream
*/
std::ostream&
CiscOpCode::details( std::ostream& _sink ) const
{
  char hex[] = "0123456789abcdef";
  uint32_t byte;
  _sink << "{size: " << this->maskbytesize() << ", mask: 0x";
  for( unsigned int idx = 0; idx < this->maskbytesize(); ++ idx ) { byte = this->m_mask[idx]; _sink << hex[byte/16] << hex[byte%16]; }
  _sink << ", bits: 0x";
  for( unsigned int idx = 0; idx < this->maskbytesize(); ++ idx ) { byte = this->m_bits[idx]; _sink << hex[byte/16] << hex[byte%16]; }
  _sink << "}";
  return _sink;
}

/**
 *  @brief  A word iterator Object.
 *
 *  This class implements an iterator on words in a bitfield list
 *  (bitfield separated by separators). It computes bitfield bounds by
 *  mean of iterators, and size for current word.
 *  
 */

struct BFWordIterator {
  Vector<BitField>::const_iterator m_left, m_right, m_end;
  unsigned int                       m_count, m_minsize, m_maxsize;
  bool                               m_rewind, m_has_operand, m_has_subop;
  
  BFWordIterator( Vector<BitField> const& _bitfields )
    : m_right( _bitfields.begin() - 1 ), m_end( _bitfields.end() ),
      m_count( 0 ), m_minsize( 0 ), m_maxsize( 0 ), m_rewind( false ),
      m_has_operand( false ), m_has_subop( false )
  {}
  
  bool
  next() {
    m_left = m_right + 1;
    if (m_left >= m_end) return false;
    m_count = 0; m_minsize = 0; m_maxsize = 0;
    m_has_operand = false; m_has_subop = false;
    for (m_right = m_left; (m_right < m_end) and (not dynamic_cast<SeparatorBitField const*>( &**m_right )); ++m_right)
      {
        m_count += 1;
        m_minsize += (**m_right).minsize();
        m_maxsize += (**m_right).maxsize();
        if      (dynamic_cast<OperandBitField const*>(&**m_right)) m_has_operand = true;
        else if (dynamic_cast<SubOpBitField const*>(&**m_right))   m_has_subop = true;
      }
    SeparatorBitField const* sepbf = dynamic_cast<SeparatorBitField const*>( &**m_right );
    if ((m_right < m_end) and sepbf) {
      m_rewind = sepbf->rewind;
    } else
      m_rewind = false;
        
    return true;
  }
  
};

/** Process the isa structure and computes CISC specific data 
*/
void
CiscGenerator::finalize()
{
  // Finalize size information
  m_code_capacity = ((*m_insnsizes.rbegin()) + 7) / 8;
  
  Vector<Operation> const& operations = source.m_operations;
  
  // Process the opcodes needed by the decoder
  for (Vector<Operation>::const_iterator op = operations.begin(); op < operations.end(); ++ op) {
    // compute prefix size
    unsigned int prefixsize = 0, insn_size = 0;
    bool vlen = false, outprefix = false, vword = false;
    
    for (FieldIterator fi( source.m_little_endian, (**op).bitfields, (*m_insnsizes.rbegin()) ); fi.next(); ) {
      if (SeparatorBitField const* sbf = dynamic_cast<SeparatorBitField const*>( &fi.item() )) {
        if (sbf->rewind and vword) {
          (**op).fileloc.err( "error: operation `%s' rewinds a variable length word.", (**op).symbol.str() );
          throw GenerationError;
        }
        vword = false;
      }
      
      if (fi.item().sizes() != 1) {
        vword = true;
        outprefix = true;
        vlen = true;
      }
      
      insn_size = std::max( insn_size, fi.insn_size() );
      if (outprefix) continue;
      if (prefixsize < fi.insn_size()) prefixsize = fi.insn_size();
    }
    
    m_opcodes[*op] = new CiscOpCode( (**op).symbol, prefixsize, insn_size, vlen );
    CiscOpCode& oc = ciscopcode( *op );
    
    // compute opcode
    for (FieldIterator fi( source.m_little_endian, (**op).bitfields, (*m_insnsizes.rbegin()) ); fi.next(); ) {
      if (fi.item().sizes() != 1) break;
      if (not fi.item().hasopcode()) continue;
      uint64_t mask = fi.item().mask(), bits = fi.item().bits();
      bool little_endian = source.m_little_endian;
      unsigned int pos = little_endian ? fi.pos() : fi.insn_size();
      while (mask) {
        unsigned int shift = little_endian ? (pos % 8) : (-pos % 8);
        unsigned int offset = little_endian ? (pos / 8) : ((pos-1) / 8);
        uint8_t partial_mask = mask << shift;
        uint8_t partial_bits = bits << shift;
        oc.m_mask[offset] |= partial_mask;
        oc.m_bits[offset] = (oc.m_bits[offset] & ~partial_mask) | (partial_bits & partial_mask);
        shift = 8 - shift;
        mask >>= shift;
        bits >>= shift;
        pos = little_endian ? pos + shift : pos - shift;
      }
    }
    oc.optimize( source.m_little_endian );
  }
  
  this->toposort();
}

void
CiscGenerator::additional_decl_includes( Product& _product ) const {
  _product.code( "#include <iosfwd>\n" );
}

void
CiscGenerator::additional_impl_includes( Product& _product ) const {
  _product.code( "#include <ostream>\n" );
  _product.code( "#include <cstring>\n" );
}

void
CiscGenerator::codetype_decl( Product& _product ) const {
  bool little_endian = source.m_little_endian;
  char const* dsh = little_endian ? ">>" : "<<";
  char const* ash = little_endian ? "<<" : ">>";
  _product.code( "struct CodeType {\n" );
  _product.code( " enum { capacity = %d };\n", m_code_capacity );
  _product.code( " unsigned int              size;\n" );
  _product.code( " uint8_t                   str[capacity];\n" );
  _product.code( " enum Exception_t { NotEnoughBytes };\n" );
  _product.code( " CodeType() : size( 0 ) { for (int idx = capacity; (--idx) >= 0;) str[idx] = 0; };\n" );
  _product.code( " CodeType( unsigned int sz )\n" );
  _product.code( " : size( std::min( sz, capacity*8u ) )\n{\n" );
  _product.code( "   for (int idx = capacity; (--idx) >= 0;) str[idx] = 0;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType( uint8_t const* src, unsigned int sz )\n" );
  _product.code( " : size( std::min( sz, capacity*8u ) )\n{\n" );
  _product.code( "   for (int idx = capacity; (--idx) >= 0;) str[idx] = 0;\n" );
  _product.code( "   for (int idx = (size+7)/8; (--idx) >= 0;) str[idx] = src[idx];\n" );
  _product.code( " }\n" );
  _product.code( " CodeType( CodeType const& ct )\n" );
  _product.code( " : size( ct.size )\n{\n" );
  for (unsigned int idx = 0; idx < m_code_capacity; ++idx)
    _product.code( " str[%u] = ct.str[%u];", idx, idx );
  _product.code( "\n }\n" );
  _product.code( " bool match( CodeType const& bits, CodeType const& mask ) const {\n" );
  _product.code( "  unsigned int maskbound = (mask.size+7)/8, codebound = (size+7)/8;\n" );
  _product.code( "  for (unsigned int idx = 0; idx < maskbound; ++idx) {\n" );
  _product.code( "   if (idx >= codebound) throw NotEnoughBytes;\n" );
  _product.code( "   if ((str[idx] & mask.str[idx]) != bits.str[idx]) return false;\n" );
  _product.code( "  };\n" );
  _product.code( "  return true;\n" );
  _product.code( " }\n" );
  _product.code( " bool match( CodeType const& bits ) const {\n" );
  _product.code( "  if (size < bits.size) throw NotEnoughBytes;\n" );
  _product.code( "  unsigned int end = bits.size/8;\n" );
  _product.code( "  for (unsigned int idx = 0; idx < end; idx += 1)\n" );
  _product.code( "   if (str[idx] != bits.str[idx]) return false;\n" );
  _product.code( "  unsigned int tail = (bits.size % 8);\n" );
  _product.code( "  if (tail == 0) return true;" );
  _product.code( "  uint8_t tailmask = 0xff %s (8-tail);\n", source.m_little_endian ? ">>" : "<<" );
  _product.code( "  return ((str[end] ^ bits.str[end]) & tailmask) == 0;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& stretch_front( unsigned int shift ) {\n" );
  _product.code( "  int hish = shift / 8, losh = shift %% 8; \n" );
  _product.code( "  for (int dst = %u, src = dst - hish; dst >= 0; dst-=1, src-=1) {\n",
                 m_code_capacity - 1 );
  _product.code( "   if (src > 0) str[dst] = (str[src] %s losh) | (str[src-1] %s (8-losh));\n",
                 ash, dsh );
  _product.code( "   else if (src == 0) str[dst] = (str[src] %s losh);\n", ash );
  _product.code( "   else str[dst] = 0;\n" );
  _product.code( "  }\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& shrink_front( unsigned int shift ) {\n" );
  _product.code( "  int hish = shift / 8, losh = shift %% 8; \n" );
  _product.code( "  for (unsigned int dst = 0, src = dst + hish; dst < %u; dst+=1, src+=1) {\n",
                 m_code_capacity);
  _product.code( "   if (src < %u) str[dst] = (str[src] %s losh) | (str[src-1] %s (8-losh));\n",
                 m_code_capacity - 1, dsh, ash );
  _product.code( "   else if (src == %u) str[dst] = (str[src] %s losh);\n",
                 m_code_capacity - 1, dsh );
  _product.code( "   else str[dst] = 0;\n" );
  _product.code( "  }\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " CodeType& extend( uint8_t* src, unsigned int sz ) {\n" );
  _product.code( "  CodeType tail( src, sz );\n" );
  _product.code( "  unsigned int mod = this->size %% 8; \n" );
  _product.code( "  if (mod) {\n" );
  _product.code( "   tail.size = std::min( tail.size + mod, capacity*8u );\n" );
  _product.code( "   tail.stretch_front( mod );\n" );
  _product.code( "   tail.str[0] = (tail.str[0] & (0xff %s mod)) | "
                 "(this->str[this->size/8] & (0xff %s (8-mod)));\n", ash, dsh );
  _product.code( "   this->size -= mod;\n" );
  _product.code( "  }\n" );
  _product.code( "  for (unsigned int src = 0, dst = this->size/8; "
                 "(src < capacity*1u) and (dst < capacity*1u); src+=1, dst+=1) {\n" );
  _product.code( "   this->str[dst] = tail.str[src];\n" );
  _product.code( "  }\n" );
  _product.code( "  this->size = std::min( this->size + tail.size, capacity*8u );\n" );
  _product.code( "  return *this;\n" );
  _product.code( " }\n" );
  _product.code( " friend std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct );\n" );
  _product.code( "};\n" );
}

void
CiscGenerator::codetype_impl( Product& _product ) const
{
  if (not source.m_tparams.empty())
    _product.code( "inline\n" );
  _product.code( "std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {\n" );
  _product.code( " if (_ct.size % 8) {\n" );
  if (source.m_little_endian) {
    _product.code( "  for (int idx = _ct.size; (--idx) >= 0; ) {\n" );
    _product.code( "   _sink << (((_ct.str[idx/8] >> (idx%8)) & 1) ? '1' : '0');\n" );
    _product.code( "  }\n" );
  } else {
    _product.code( "  for (unsigned int idx = 0; idx < _ct.size; ++idx ) {\n" );
    _product.code( "   _sink << (((_ct.str[idx/8] >> ((7-idx)%8)) & 1) ? '1' : '0');" );
    _product.code( "  }\n" );
  }
  _product.code( " } else {\n" );
  _product.code( "  char const* xrepr = \"0123456789abcdef\";\n" );
  if (source.m_little_endian) {
    _product.code( "  for (int idx = _ct.size/8; (--idx) >= 0; ) {\n" );
  } else {
    _product.code( "  for (unsigned int idx = 0; idx < _ct.size/8; ++idx ) {\n" );
  }
  _product.code( "    uint8_t byte = _ct.str[idx];\n" );
  _product.code( "   _sink << xrepr[(byte >> 4) & 0xf] << xrepr[(byte >> 0) & 0xf];\n" );
  _product.code( "  }\n" );
  _product.code( " }\n" );
  _product.code( " return _sink;\n" );
  _product.code( "}\n" );
}

void
CiscGenerator::insn_bits_code( Product& _product, Operation const& _op ) const
{
  CiscOpCode const& oc = ciscopcode( &_op );
  _product.code( "CodeType( (uint8_t*)( \"" );
  char const* hex = "0123456789abcdef";
  for( unsigned int idx = 0; idx < oc.maskbytesize(); ++idx )
    _product.code( "\\x%c%c", hex[oc.m_bits[idx]/16], hex[oc.m_bits[idx]%16] );
  _product.code( "\" ), %u )", oc.m_prefixsize );
}

void
CiscGenerator::insn_mask_code( Product& _product, Operation const& _op ) const
{
  CiscOpCode const& oc = ciscopcode( &_op );
  _product.code( "CodeType( (uint8_t*)( \"" );
  char const* hex = "0123456789abcdef";
  for( unsigned int idx = 0; idx < oc.maskbytesize(); ++idx )
    _product.code( "\\x%c%c", hex[oc.m_mask[idx]/16], hex[oc.m_mask[idx]%16] );
  _product.code( "\" ), %u )", oc.m_prefixsize );
}

void
CiscGenerator::insn_match_ifexpr( Product& _product, char const* _code, char const* _mask, char const* _bits ) const
{
  _product.code( "if( %s.match( %s, %s) )\n", _code, _bits, _mask );
}

void
CiscGenerator::insn_encode_impl( Product& _product, Operation const& _op, char const* _codename ) const
{
  CiscOpCode const& oc = ciscopcode( &_op );
  
  if (oc.m_vlen) {
    _product.code( "assert( \"Encode method does not work with variable length operations.\" and false );\n" );
  }
  
  
  { /* writing  common operation bits */
    _product.code( "%s = CodeType( (uint8_t*)( \"", _codename );
    char const* const hex = "0123456789abcdef";
    unsigned int idx = 0;
    for (;idx < oc.maskbytesize(); ++idx)
      _product.code( "\\x%c%c", hex[oc.m_bits[idx]/16], hex[oc.m_bits[idx]%16] );
    for (;idx < oc.fullbytesize(); ++idx)
      _product.code( "\\0" );
    _product.code( "\" ), %u );\n", oc.m_fullsize );
  }
  
  bool little_endian = source.m_little_endian;
  
  for (FieldIterator fi( little_endian, _op.bitfields, (*m_insnsizes.rbegin()) ); fi.next(); )
    {
    
      if (dynamic_cast<SubOpBitField const*>( &fi.item() ))
        {
          _product.code( "assert( \"Encode method does not work with sub-operations.\" and false );\n" );
        }
    
      else if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &fi.item() ))
        {
          unsigned int opsize = membersize( *opbf );
          ConstStr shiftedop;
          if      (opbf->shift > 0)
            shiftedop = Str::fmt( "(uint%u_t( %s ) << %u)", opsize, opbf->symbol.str(), +opbf->shift );
          else if (opbf->shift < 0)
            shiftedop = Str::fmt( "(uint%u_t( %s ) >> %u)", opsize, opbf->symbol.str(), -opbf->shift );
          else
            shiftedop = Str::fmt( "uint%u_t( %s )", opsize, opbf->symbol.str() );
      
          for (unsigned int end = fi.insn_size(), start = end - fi.m_size,
                 substart = start, subend; substart < end; substart = subend)
            {
              subend = std::min( (substart + 8) & -8, end);
              unsigned int
                bound = 1 << (subend - substart),
                pos = little_endian ? (substart % 8) : ((-subend) % 8),
                bytepos = substart / 8,
                srcpos = little_endian ? (substart - start) : (end - subend);
          
              _product.code( "%s.str[%u] |= (((%s >> %u) %% %u) << %u);\n",
                             _codename, bytepos, shiftedop.str(), srcpos, bound, pos );
            }
        }
    }
}

void
CiscGenerator::insn_decode_impl( Product& _product, Operation const& _op, char const* _codename, char const* _addrname ) const
{
  CiscOpCode const& oc = ciscopcode( &_op );
  _product.code( "if (this->encoding.size < %u) throw CodeType::NotEnoughBytes;\n", oc.m_fullsize );
  _product.code( "this->encoding.size = %u;\n", oc.m_fullsize );
  if (oc.m_vlen) {
    char const* ncodename = "_code_";
    _product.code( "CodeType %s( %s );\n", ncodename, _codename );
    _codename = ncodename;
  }
  
  bool little_endian = source.m_little_endian;
  char const* dsh = little_endian ? ">>" : "<<";
  char const* ash = little_endian ? "<<" : ">>";
  
  for (FieldIterator fi( little_endian, _op.bitfields, (*m_insnsizes.rbegin()) ); fi.next(); )
    {
      if (SubOpBitField const* sobf = dynamic_cast<SubOpBitField const*>( &fi.item() ))
        {
          SDInstance const* sdinstance = sobf->sdinstance;
          SDClass const* sdclass = sdinstance->m_sdclass;
          SourceCode const* tpscheme =  sdinstance->m_template_scheme;
      
          _product.code( "{\n" );
          _product.code( "%s::CodeType _subcode_;\n", sdclass->qd_namespace().str() );
          _product.code( "_subcode_.size = %u;\n", sdclass->maxsize() );
      
          unsigned int shift = fi.insn_size() - fi.m_size;
          unsigned int byteshift = shift / 8;
          shift = shift % 8;
          unsigned int subbytes = (sdclass->maxsize() + 7) / 8;
      
          for (unsigned int idx = 0; idx < subbytes; ++idx) {
            unsigned int didx = idx + byteshift;
            _product.code( "_subcode_.str[%u] = ", idx );
            _product.code( "uint8_t( %s.str[%u] %s %u )", _codename, didx, dsh, shift );
            if (shift > 0 and (didx + 1) < m_code_capacity)
              _product.code( "| uint8_t( %s.str[%u] %s %u )", _codename, didx + 1, ash, 8 - shift );
            _product.code( ";\n" );
          }
      
          _product.code( "%s = %s::sub_decode", sobf->symbol.str(), sdclass->qd_namespace().str() );
          if (tpscheme)
            _product.usercode( tpscheme->fileloc, "< %s >", tpscheme->content.str() );
          _product.code( "( %s, _subcode_ );\n", _addrname );
          _product.code( "unsigned int shortening = %u - %s->GetLength();\n",
                         sdclass->maxsize(), sobf->symbol.str() );
          _product.code( "this->encoding.size -= shortening;\n" );
          _product.code( "%s.stretch_front( shortening );\n", _codename );
          _product.code( "}\n" );
        }
    
      else if (OperandBitField const* opbf = dynamic_cast<OperandBitField const*>( &fi.item() ))
        {
          _product.code( "%s = ", opbf->symbol.str() );
          unsigned int opsize = membersize( *opbf );
          char const* sep = "";
      
          for (unsigned int end = fi.insn_size(), start = end - fi.m_size,
                 substart = start, subend; substart < end; substart = subend)
            {
              subend = std::min( (substart + 8) & -8, end);
              unsigned int
                bound = 1 << (subend - substart),
                pos = little_endian ? (substart % 8) : ((-subend) % 8),
                bytepos = substart / 8,
                dstpos = little_endian ? (substart - start) : (end - subend),
                mask = bound - 1;
          
              if (mask) {
                if(pos) {
                  if(dstpos) {
                    _product.code( "%s(((uint%u_t( %s.str[%u] ) >> %u) & 0x%x) << %u)", sep, opsize, _codename, bytepos, pos, mask, dstpos );
                  }
                  else {
                    _product.code( "%s((uint%u_t( %s.str[%u] ) >> %u) & 0x%x)", sep, opsize, _codename, bytepos, pos, mask );
                  }
                }
                else {
                  if(dstpos) {
                    _product.code( "%s((uint%u_t( %s.str[%u] ) & 0x%x) << %u)", sep, opsize, _codename, bytepos, mask, dstpos );
                  }
                  else {
                    _product.code( "%s(uint%u_t( %s.str[%u] ) & 0x%x)", sep, opsize, _codename, bytepos, mask );
                  }
                }
              }
              else {
                _product.code( "%s(uint%u_t( 0x0 ))", sep, opsize);
              }
              sep = " | ";
            }
          _product.code( ";\n" );
      
          if (opbf->sext) {
            int sext_shift = opsize - opbf->size;
            _product.code( "%s = (int%u_t)(%s << %u) >> %u;\n", opbf->symbol.str(), opsize, opbf->symbol.str(), sext_shift, sext_shift );
          }
    
          if (opbf->shift > 0)
            _product.code( "%s >>= %u;\n", opbf->symbol.str(), +opbf->shift );
          if (opbf->shift < 0)
            _product.code( "%s <<= %u;\n", opbf->symbol.str(), -opbf->shift );
        }
    }
}

void
CiscGenerator::insn_unchanged_expr( Product& _product, char const* _ref, char const* _bytes ) const
{
  _product.code( "%s.match( %s )", _bytes, _ref );
}

void
CiscGenerator::insn_destructor_decl( Product& _product, Operation const& _op ) const
{
  bool subops = false;
  Vector<BitField> const& bfs = _op.bitfields;
  
  for( Vector<BitField>::const_iterator bf = bfs.begin(); bf < bfs.end(); ++bf ) {
    if (dynamic_cast<SubOpBitField const*>( &**bf )) { subops = true; break; }
  }
  
  if (not subops) return;
  _product.code( "~Op%s();\n", Str::capitalize( _op.symbol.str() ).str() );
}

void
CiscGenerator::insn_destructor_impl( Product& _product, Operation const& _op ) const
{
  std::vector<ConstStr> subops;
  Vector<BitField> const& bfs = _op.bitfields;
  
  for( Vector<BitField>::const_iterator bf = bfs.begin(); bf < bfs.end(); ++bf ) {
    if (SubOpBitField const* sobf = dynamic_cast<SubOpBitField const*>( &**bf ))
      subops.push_back( sobf->symbol );
  }
  
  if( subops.size() == 0 ) return;
  
  _product.template_signature( source.m_tparams );
  _product.code( "Op%s", Str::capitalize( _op.symbol.str() ).str() );
  _product.template_abbrev( source.m_tparams );
  _product.code( "::~Op%s()\n", Str::capitalize( _op.symbol.str() ).str() );
  
  _product.code( "{\n" );
  for( std::vector<ConstStr>::const_iterator name = subops.begin(); name != subops.end(); ++name ) {
    _product.code( "delete %s;\n", name->str() );
  }
  _product.code( "}\n\n" );
}

void
CiscGenerator::op_getlen_decl( Product& _product ) const {
  _product.code( "inline unsigned int GetLength() const { return this->encoding.size; }\n" );
}
