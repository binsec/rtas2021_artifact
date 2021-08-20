/*
 *  Copyright (c) 2018,
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

#include <core.hh>

Core::Core()
  : xer(0)
  , cr(0)
  , msr(0)
  , fpscr(0,*this)
{
}

U64
UnsignedMultiplyHigh( U64 lop, U64 rop )
{
  U64 lhi = U64(lop >> 32), llo = U64(U32(lop)), rhi = U64(rop >> 32), rlo = U64(U32(rop));
  U64 hihi( lhi*rhi ), hilo( lhi*rlo), lohi( llo*rhi ), lolo( llo*rlo );
  return (((lolo >> 32) + U64(U32(hilo)) + U64(U32(lohi))) >> 32) + (hilo >> 32) + (lohi >> 32) + hihi;
}

S64
SignedMultiplyHigh( S64 lop, S64 rop )
{
  bool lsign = (lop < S64(0)), rsign = (rop < S64(0));
  U64 ulop = lsign ? -lop : lop, urop = rsign ? -rop : rop;
  
  if (lop < S64(0)) { ulop = -lop; lsign = true; } else { ulop = lop; lsign = false; }
  if (rop < S64(0)) { urop = -rop; rsign = true; } else { urop = rop; rsign = false; }
  
  U64 uhi = UnsignedMultiplyHigh(ulop, urop), ulo = ulop*urop;
  
  return S64( (lsign xor rsign) ? (ulo == 0) ? -uhi : ~uhi : uhi );
}

UINT
FPSCR::GetDispatch( FPSCR::RN const& rn ) const
{
  return core.fp_shuffler.GetRN( core.cia, FPSCR::RN::Get(value) );
}

