/***************************************************************************
                                 bitfield.cc
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

#include <bitfield.hh>
#include <ostream>
#include <strtools.hh>
#include <subdecoder.hh>
#include <scanner.hh>
#include <cassert>

/** Create an opcode bitfield object
    @param _size size in bits
    @param _value value of the opcode
*/
OpcodeBitField::OpcodeBitField( unsigned int _size, unsigned int _value )
  : FixedSizeBitField( _size ), value( _value )
{}

/** Create an opcode bitfield object (copy constructor)
    @param _src the bitfield object to copy
*/
OpcodeBitField::OpcodeBitField( OpcodeBitField const& _src )
  : FixedSizeBitField( _src.size ), value( _src.value )
{}

/** Dump an opcode bitfield object into a stream
    @param sink a stream
*/
void
OpcodeBitField::fills( std::ostream& sink ) const
{
  sink << Str::fmt( "0x%x[%u]", value, size );
}

/** Create an operand bitfield object
    @param _size size in bits
    @param _symbol a symbol object representing the operand
    @param _shift a shift amount to do on the bitfield
    @param _size_modifier minimum bit size for holding the operand bitfield
    @param _sext true if a sign extension is required
*/
OperandBitField::OperandBitField( unsigned int _size, ConstStr _symbol, int _shift, unsigned int _size_modifier, bool _sext )
  : FixedSizeBitField( _size ), symbol( _symbol ), shift( _shift ), size_modifier( _size_modifier ), sext( _sext )
{}

/** Create an operand bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
OperandBitField::OperandBitField( OperandBitField const& _src )
  : FixedSizeBitField( _src.size ),
    symbol( _src.symbol ), shift( _src.shift ), size_modifier( _src.size_modifier ), sext( _src.sext )
{}

/** Dump an operand bitfield object into a stream
    @param sink a stream
*/
void
OperandBitField::fills( std::ostream& sink ) const
{
  if      (shift < 0)
    sink << "shl<" << (-shift) << "> ";
  else if (shift > 0)
    sink << "shr<" << (+shift) << "> ";
  
  if (sext)
    sink << "sext ";
  
  if (size_modifier)
    sink << "<" << size_modifier << "> ";

  sink << Str::fmt( "%s[%u]", symbol.str(), size );
}

/** Return the size (in bits) of the target word encoded by this field.
    @return the size (in bits) of the target word encoded by this field.
*/
unsigned int
OperandBitField::dstsize() const
{
  return std::max( (unsigned int)(size + ((shift < 0) ? -shift : 0)), size_modifier );
}

/** Create an unused bitfield object
    @param _size size in bits
*/
UnusedBitField::UnusedBitField( unsigned int _size )
  : FixedSizeBitField( _size )
{}

/** Create an unused bitfield object (copy constructor)
    @param _src the source bitfield object to copy
*/
UnusedBitField::UnusedBitField( UnusedBitField const& _src )
  : FixedSizeBitField( _src.size )
{}

/** Dump an unused bitfield object into a stream
    @param sink a stream
*/
void
UnusedBitField::fills( std::ostream& sink ) const
{
  sink << "?[" << size << "]";
}

SeparatorBitField::SeparatorBitField( bool _rewind )
  : rewind( _rewind )
{}

SeparatorBitField::SeparatorBitField( SeparatorBitField const& _src )
  : BitField(), rewind(false)
{}

void
SeparatorBitField::fills( std::ostream& sink ) const
{
  sink << (rewind ? "> rewind <" : "> <" );
}

/** Dump an bitfield object into a stream
    @param sink a stream
*/
std::ostream&
operator << ( std::ostream& sink, BitField const& bf )
{
  bf.fills( sink );
  return sink;
}

SubOpBitField::SubOpBitField( ConstStr _symbol, SDInstance const* _sdinstance )
  : symbol( _symbol ), sdinstance( _sdinstance )
{}

SubOpBitField::SubOpBitField( SubOpBitField const& _src )
  : symbol( _src.symbol ), sdinstance( _src.sdinstance )
{}

/** Dump an subop bitfield object into a stream
    @param sink a stream
*/
void
SubOpBitField::fills( std::ostream& sink ) const
{
  sink << symbol << '[' << sdinstance->m_symbol << ']';
}

uintptr_t
SubOpBitField::sizes() const { return sdinstance->m_sdclass->m_insnsizes.size(); }

void
SubOpBitField::sizes( unsigned int* _sizes ) const
{
  std::copy( sdinstance->m_sdclass->m_insnsizes.begin(), sdinstance->m_sdclass->m_insnsizes.end(), _sizes );
}

unsigned int SubOpBitField::minsize() const { return sdinstance->m_sdclass->minsize(); }
unsigned int SubOpBitField::maxsize() const { return sdinstance->m_sdclass->maxsize(); }


/**
 *  Create a specialized operand bitfield object
 *  @param _src the operand bitfield object to be specialized.
 *  @param _value
 *  @param _shift a shift amount to do on the bitfield
 *  @param _size_modifier minimum bit size for holding the operand bitfield
 *  @param _sext true if a sign extension is required
 */
SpOperandBitField::SpOperandBitField( OperandBitField const& _src, unsigned int _value )
  : FixedSizeBitField( _src.size ), symbol( _src.symbol ), shift( _src.shift ),
    size_modifier( _src.size_modifier ), sext( _src.sext ), value( _value )
{}

/** Create an operand bitfield object (copy constructor)
 *  @param _src the source bitfield object to copy
 */
SpOperandBitField::SpOperandBitField( SpOperandBitField const& _src )
  : FixedSizeBitField( _src.size ), symbol( _src.symbol ), shift( _src.shift ),
    size_modifier( _src.size_modifier ), sext( _src.sext ), value( _src.value )
{}

/** Return the size (in bits) of the target word encoded by this field.
    @return the size (in bits) of the target word encoded by this field.
*/
unsigned int
SpOperandBitField::dstsize() const
{
  return std::max( (unsigned int)(size + ((shift < 0) ? -shift : 0)), size_modifier );
}

/**
 *  Return the c constant string corresponding to the value encoded.
 */
ConstStr
SpOperandBitField::constval() const
{
  if (not sext)
    return Str::fmt( "%#x", value );
  
  assert( size <= 32 );
  int32_t val = (int32_t( value ) << (32-size)) >> (32-size);
  if (val < 0)
    return Str::fmt( "-%#x", uint32_t( -val ) );
  
  return Str::fmt( "%#x", uint32_t( value ) );
}
