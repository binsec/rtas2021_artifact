/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <arch.hh>

void
Processor::FTop::Repr( std::ostream& sink ) const
{
  sink << "FpuStackTop";
}

unisim::util::symbolic::Expr& Processor::fpaccess( unsigned reg, bool write ) { throw Unimplemented();}

Processor::Expr
Processor::eregread( unsigned reg, unsigned size, unsigned pos )
{
  using unisim::util::symbolic::ExprNode;
  using unisim::util::symbolic::make_const;
    
  struct
  {
    Expr ui( unsigned sz, Expr const& src ) const
    {
      switch (sz) {
      default: throw 0;
      case 1: return new unisim::util::symbolic::CastNode<uint8_t,uint64_t>( src );
      case 2: return new unisim::util::symbolic::CastNode<uint16_t,uint64_t>( src );
      case 4: return new unisim::util::symbolic::CastNode<uint32_t,uint64_t>( src );
      case 8: return new unisim::util::symbolic::CastNode<uint64_t,uint64_t>( src );
      }
      return 0;
    }
  } cast;
    
  if (not regvalues[reg][pos].node)
    {
      // requested read is in the middle of a larger value
      unsigned src = pos;
      do { src = src & (src-1); } while (not regvalues[reg][src].node);
      unsigned shift = 8*(pos - src);
      return cast.ui( size, make_operation( "Lsr", regvalues[reg][src], make_const( shift ) ) );
    }
  else if (not regvalues[reg][(pos|size)&(GREGSIZE-1)].node)
    {
      // requested read is in lower bits of a larger value
      return cast.ui( size, regvalues[reg][pos] );
    }
  else if ((size > 1) and (regvalues[reg][pos|(size >> 1)].node))
    {
      // requested read is a concatenation of multiple source values
      Expr concat = cast.ui( size, regvalues[reg][pos] );
      for (unsigned idx = 0; ++idx < size;)
        {
          if (not regvalues[reg][pos+idx].node)
            continue;
          concat = make_operation( "Or", make_operation( "Lsl", cast.ui( size, regvalues[reg][idx] ), make_const( 8*idx ) ), concat );
        }
      return concat;
    }
    
  // requested read is directly available
  return regvalues[reg][pos];
}
  
void
Processor::eregwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr )
{
  Expr nxt[GREGSIZE];
    
  for (unsigned ipos = pos, isize = size, cpos;
       cpos = (ipos^isize) & (GREGSIZE-1), (not regvalues[reg][ipos].node) or (not regvalues[reg][cpos].node);
       isize *= 2, ipos &= -isize
       )
    {
      nxt[cpos] = eregread( reg, isize, cpos );
    }
    
  for (unsigned ipos = 0; ipos < GREGSIZE; ++ipos)
    {
      if (nxt[ipos].node)
        regvalues[reg][ipos] = nxt[ipos];
    }
    
  regvalues[reg][pos] = xpr;
    
  for (unsigned rem = 1; rem < size; ++rem)
    {
      regvalues[reg][pos+rem] = 0;
    }
}

void
Processor::eregsinks( RIRegID reg )
{
  using unisim::util::symbolic::binsec::BitFilter;
  // using unisim::util::symbolic::make_const;
    
  { // Check for constant values
    Expr dr = unisim::util::symbolic::binsec::ASExprNode::Simplify( eregread( reg.idx(), 8, 0 ) );
    if (dr.ConstSimplify())
      {
        path->add_sink( newRegWrite( reg, dr ) );
        return;
      }
  }

  // Check for monolithic value
  if (not regvalues[reg.idx()][GREGSIZE/2].node)
    {
      path->add_sink( newRegWrite( reg, eregread(reg.idx(),GREGSIZE,0) ) );
      return;
    }

  throw Unimplemented(); // Yet
  // // Requested read is a concatenation of multiple source values
  // struct _
  // {
  //   _( Processor& _core, unsigned _reg ) : core(_core), reg(_reg) { Process( 0, GREGSIZE ); } Processor& core; unsigned reg;
  //   void Process( unsigned pos, unsigned size )
  //   {
  //     unsigned half = size / 2, mid = pos+half;
  //     if (size > 1 and core.regvalues[reg][mid].node)
  //       {
  //         Process( pos, half );
  //         Process( mid, half );
  //       }
  //     else
  //       {
  //         unsigned begin = pos*8, end = begin+size*8;
  //         Expr value( new BitFilter( core.eregread(reg,size,pos), 64, size*8, size*8, false ) );
  //         core.path->add_sink( new GregPartialWrite( reg, begin, end, value ) );
  //       }
  //   }
  // } concat( *this, reg );
}

// Operation*
// AMD64::decode( uint64_t addr, MemCode& ct, std::string& disasm )
// {
//   try
//     {
//       unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 ); // x86_64
//       unisim::component::cxx::processor::intel::InputCode<ut::Processor> ic( mode, &ct.bytes[0], addr );
//       Operation* op = getoperation( ic );
//       if (not op)
//         {
//           uint8_t const* oc = ic.opcode();
//           unsigned opcode_length = ic.vex() ? 5 - (oc[0] & 1) : oc[0] == 0x0f ? (oc[1] & ~2) == 0x38 ? 4 : 3 : 2;
//           ct.length = ic.opcode_offset + opcode_length;
//           throw ut::Untestable("#UD");
//         }
//       ct.length = op->length;
//       std::ostringstream buf;
//       op->disasm(buf);
//       disasm = buf.str();
//       return op;
//     }
//   catch (unisim::component::cxx::processor::intel::CodeBase::PrefixError const& perr)
//     {
//       ct.length = perr.len;
//       throw ut::Untestable("#UD");
//     }
//   return 0;
// }

Processor::Processor()
  : path(0)
  , next_insn_addr()
  , next_insn_mode(ipjmp)
{
  for (FLAG reg; reg.next();)
    flagvalues[reg.idx()] = newRegRead( reg );
    
  for (RIRegID reg; reg.next();)
    regvalues[reg.idx()][0] = newRegRead( reg );

  for (unsigned reg = 0; reg < VREGCOUNT; ++reg)
    {
      VmmRegister v( new VRegRead( reg ) );
      *(umms[reg].GetStorage( &vmm_storage[reg][0], v, VUConfig::BYTECOUNT )) = v;
    }
}

bool
Processor::close( Processor const& ref, uint32_t linear_nia )
{
  bool complete = path->close();
  
  if (next_insn_mode == ipcall)
    path->add_sink( new Call( next_insn_addr.expr, return_address ) );
  else
    path->add_sink( new Goto( next_insn_addr.expr ) );
    
  for (RIRegID reg; reg.next();)
    {
      if (regvalues[reg.idx()][0] != ref.regvalues[reg.idx()][0])
        eregsinks(reg);
    }
    
  // Flags
  for (FLAG reg; reg.next();)
    if (flagvalues[reg.idx()] != ref.flagvalues[reg.idx()])
      path->add_sink( newRegWrite( reg, flagvalues[reg.idx()] ) );

  for (Expr const& store : stores)
    path->add_sink( store );
    
  return complete;
}
  
void
Processor::step( Operation const& op )
{
  uint64_t insn_addr = op.address, nia = insn_addr + op.length;
  return_address = nia;
  next_insn_addr = addr_t(nia);
  op.execute( *this );
}
  
void
Processor::noexec( Operation const& op )
{
  std::cerr
    << "error: no execute method in `" << typeid(op).name() << "'\n"
    << std::hex << op.address << ":\t";
  op.disasm( std::cerr );
  std::cerr << '\n';
  throw Unimplemented();
}
