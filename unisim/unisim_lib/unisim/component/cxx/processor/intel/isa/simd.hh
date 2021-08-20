/*
 *  Copyright (c) 2007-2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

// /* ADDSUBPD -- Packed Single- or Double-FP Add/Subtract */
// op addsubpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsubpd_vdq_wdq.disasm = { _sink << "addsubpd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
//  
// op addsubps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xd0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsubps_vdq_wdq.disasm = { _sink << "addsubps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* BLENDP -- Blend Packed Single- or Double-Precision Floating-Point Values */
// op blendps_vdq_wdq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// blendps_vdq_wdq_ib.disasm = { _sink << "blendps " << DisasmI( imm ) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op blendpd_vdq_wdq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// blendpd_vdq_wdq_ib.disasm = { _sink << "blendpd " << DisasmI( imm ) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* BLENDVPD -- Variable Blend Packed Double Precision Floating-Point Values */
// op blendvps_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// blendvps_vdq_wdq.disasm = { _sink << "blendvps %xmm0," << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op blendvpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// blendvpd_vdq_wdq.disasm = { _sink << "blendvpd %xmm0," << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* CMPP -- Compare Packed Single- or Double-Precision Floating-Point Values */
// op cmpps_vdq_wdq_ib( 0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpps_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpps " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "ps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
// };
// 
// op cmppd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmppd_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmppd " << DisasmI( imm ) << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "pd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
// };
// 
// /* CMPSD -- Compare Scalar Single- Double-Precision Floating-Point Values */
// op cmpss_vdq_wdq_ib( 0xf3[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpss_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpss " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "ss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
// };
// 
// op cmpsd_vdq_wdq_ib( 0xf2[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpsd_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpsd " << DisasmI( imm ) << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "sd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
// };
// 
// /* COMISD -- Compare Scalar Ordered Single- or Double-Precision Floating-Point Values and Set EFLAGS */
// op comisd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// comisd_vdq_wdq.disasm = { _sink << "comisd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op comiss_vdq_wdq( 0x0f[8]:> <:0x2f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// comiss_vdq_wdq.disasm = { _sink << "comiss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };

/*********************/
/* SIMD CVT Functions*/
/*********************/

template <class ARCH, unsigned DOPSZ, unsigned SOPSZ>
struct CvtI2F : public Operation<ARCH>
{
  CvtI2F( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {}
  RMOp<ARCH> rm; uint8_t gn;
  char const* mnemonic() const { switch (DOPSZ) { case 32: return "cvtdq2ps"; case 64: return "cvtdq2pd"; } return "cvt<bad>"; }
  void disasm( std::ostream& sink ) const { sink << mnemonic() << ' ' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,DOPSZ>::f fdst_type;
    typedef typename TypeFor<ARCH,SOPSZ>::u usrc_type;
    for (unsigned idx = 0, end = 128/DOPSZ; idx < end; ++idx)
      {
        usrc_type src = arch.vmm_read( SSE(), rm, idx, usrc_type() );
        fdst_type dst = fdst_type(src);
        arch.vmm_write( SSE(), gn, idx, dst );
      }
  }
};

template <class ARCH, unsigned DOPSZ, class SOP>
struct CvtsI2F : public Operation<ARCH>
{
  CvtsI2F( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {}
  RMOp<ARCH> rm; uint8_t gn;
  char const* mnemonic() const { switch (DOPSZ) { case 32: return "cvtsi2ss"; case 64: return "cvtsi2sd"; } return "cvt<bad>"; }
  void disasm( std::ostream& sink ) const { sink << mnemonic() << ' ' << DisasmE( SOP(), rm ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,DOPSZ>::f fdst_type;
    typename TypeFor<ARCH,SOP::SIZE>::u src = arch.rmread( SOP(), rm );
    fdst_type dst = fdst_type(src);
    arch.vmm_write( SSE(), gn, 0, dst );
  }
};

template <class ARCH, unsigned DOPSZ, unsigned SOPSZ>
struct CvtsF2F : public Operation<ARCH>
{
  CvtsF2F( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const
  {
    sink << "cvts" << SizeID<SOPSZ>::fid() << "2s" << SizeID<DOPSZ>::fid() << " " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,SOPSZ>::f fsrc_type;
    typedef typename TypeFor<ARCH,DOPSZ>::f fdst_type;

    fsrc_type src = arch.vmm_read( SSE(), rm, 0, fsrc_type() );
    //FPToFP( res, op, cpu, cpu.FPSCR );
    fdst_type dst = fdst_type(src);
    
    arch.vmm_write( SSE(), gn, 0, dst );
  }
};

template <class ARCH, unsigned SOPSZ, class DOP>
struct CvttF2I : public Operation<ARCH>
{
  CvttF2I( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {}
  RMOp<ARCH> rm; uint8_t gn;
  char const* mnemonic() const { switch (SOPSZ) { case 32: return "cvttss2si"; case 64: return "cvttsd2si"; } return "cvt<bad>"; }
  void disasm( std::ostream& sink ) const { sink << mnemonic() << ' ' << DisasmW( SSE(), rm ) << ',' << DisasmG( DOP(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,DOP::SIZE>::u udst_type;
    typedef typename TypeFor<ARCH,SOPSZ>::f fsrc_type;
    fsrc_type src = arch.vmm_read( SSE(), rm, 0, fsrc_type() );
    udst_type dst = udst_type(src);
    arch.regwrite( DOP(), gn, dst );
  }
};

template <class ARCH> struct DC<ARCH,CVT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, simdF2() & opcode( "\x0f\x5a" ) & RM() ))

    return new CvtsF2F<ARCH,32,64>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simdF3() & opcode( "\x0f\x5a" ) & RM() ))

    return new CvtsF2F<ARCH,64,32>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simdF3() & opcode( "\x0f\xe6" ) & RM() ))

    return new CvtI2F<ARCH,64,32>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd__() & opcode( "\x0f\x5b" ) & RM() ))

    return new CvtI2F<ARCH,32,32>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\x0f\x2a" ) & RM() ))
    {
      if (ic.f3())
        {
          if (ic.w()) return new CvtsI2F<ARCH,32,GOq>( _.opbase(), _.rmop(), _.greg() );
          /*else*/    return new CvtsI2F<ARCH,32,GOd>( _.opbase(), _.rmop(), _.greg() );
        }
      if (ic.f2())
        {
          if (ic.w()) return new CvtsI2F<ARCH,64,GOq>( _.opbase(), _.rmop(), _.greg() );
          /*else*/    return new CvtsI2F<ARCH,64,GOd>( _.opbase(), _.rmop(), _.greg() );
        }
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\x2c" ) & RM() ))
    {
      if (ic.f3())
        {
          if (ic.w()) return new CvttF2I<ARCH,32,GOq>( _.opbase(), _.rmop(), _.greg() );
          /*else*/    return new CvttF2I<ARCH,32,GOd>( _.opbase(), _.rmop(), _.greg() );
        }
      if (ic.f2())
        {
          if (ic.w()) return new CvttF2I<ARCH,64,GOq>( _.opbase(), _.rmop(), _.greg() );
          /*else*/    return new CvttF2I<ARCH,64,GOd>( _.opbase(), _.rmop(), _.greg() );
        }
      return 0;
    }
  
  return 0;
}};

template <class ARCH>
struct LSMxcsr : public Operation<ARCH>
{
  LSMxcsr( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, bool _st, bool _vex ) : Operation<ARCH>( opbase ), rm( _rm ), st(_st), vex(_vex) {} RMOp<ARCH> rm; bool st, vex;
  void disasm( std::ostream& sink ) const { sink << (vex ? "v" : "") << (st ? "st" : "ld") << "mxcsr " << DisasmE( GOd(), rm ); }
  void execute( ARCH& arch ) const
  {
    if (st) arch.rmwrite( GOd(), rm, arch.mxcsread() );
    else    arch.mxcswrite( arch.rmread( GOd(), rm ) );
  }
};

template <class ARCH> struct DC<ARCH,MXCSR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "\x0f\xae" ) & RM_mem() ))
    {
      bool vex = ic.vex();
      if (vex and (ic.vlen() != 128 or _.vreg())) return 0;
      unsigned rmopc = _.greg();
      if ((rmopc|1) != 3) return 0;
      return new LSMxcsr<ARCH>( _.opbase(), _.rmop(), rmopc&1, vex );
    }
      
  return 0;
}};

/* CVTDQ2PS -- Convert Packed Dword Integers to Packed Single- or Double-Precision FP Values */
// op cvtdq2ps_vdq_wdq( 0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtdq2ps_vdq_wdq.disasm = { _sink << "cvtdq2ps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// op cvtdq2pd_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtdq2pd_vdq_wdq.disasm = { _sink << "cvtdq2pd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSI2SD -- Convert Dword Integer to Scalar Double-Precision FP Value */
// op cvtsi2sd_vdq_ed( 0xf2[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtsi2sd_vdq_ed.disasm = { _sink << "cvtsi2sd " << DisasmEd( rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSI2SS -- Convert Dword Integer to Scalar Single-Precision FP Value */
// op cvtsi2ss_vdq_ed( 0xf3[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtsi2ss_vdq_ed.disasm = { _sink << "cvtsi2ss " << DisasmEd( rm ) << ',' << DisasmV( SSE(), gn ); };


/* CVTSD2SS -- Convert Scalar Double-Precision FP Value to Scalar Single-Precision FP Value */
// op cvtsd2ss_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
/* CVTPD2DQ -- Convert Packed Double-Precision FP Values to Packed Dword Integers */
// op cvtpd2dq_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpd2dq_vdq_wdq.disasm = { _sink << "cvtpd2dq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPD2PI -- Convert Packed Double-Precision FP Values to Packed Dword Integers */
// op cvtpd2pi_pq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpd2pi_pq_wdq.disasm = { _sink << "cvtpd2pi " << DisasmW( SSE(), rm ) << ',' << DisasmPq( gn ); };
/* CVTPD2PS -- Convert Packed Double-Precision FP Values to Packed Single-Precision FP Values */
// op cvtpd2ps_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpd2ps_vdq_wdq.disasm = { _sink << "cvtpd2ps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPI2P -- Convert Packed Dword Integers to Packed Single- or Double-Precision FP Values */
// op cvtpi2ps_vdq_qq( 0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpi2ps_vdq_qq.disasm = { _sink << "cvtpi2ps " << DisasmQq( rm ) << ',' << DisasmV( SSE(), gn ); };
// op cvtpi2pd_vdq_qq( 0x66[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpi2pd_vdq_qq.disasm = { _sink << "cvtpi2pd " << DisasmQq( rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPS2DQ -- Convert Packed Single-Precision FP Values to Packed Dword Integers */
// op cvtps2dq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtps2dq_vdq_wdq.disasm = { _sink << "cvtps2dq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPS2PD -- Convert Packed Single-Precision FP Values to Packed Double-Precision FP Values */
// op cvtps2pd_vdq_wdq( 0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtps2pd_vdq_wdq.disasm = { _sink << "cvtps2pd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPS2PI -- Convert Packed Single-Precision FP Values to Packed Dword Integers */
// op cvtps2pi_pq_wdq( 0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtps2pi_pq_wdq.disasm = { _sink << "cvtps2pi " << DisasmW( SSE(), rm ) << ',' << DisasmPq( gn ); };
/* CVTSD2SI -- Convert Scalar Double-Precision FP Value to Integer */
// op cvtsd2si_gd_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtsd2si_gd_wdq.disasm = { _sink << "cvtsd2si " << DisasmW( SSE(), rm ) << ',' << DisasmGd( gn ); };
// cvtsd2ss_vdq_wdq.disasm = { _sink << "cvtsd2ss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSS2SD -- Convert Scalar Single-Precision FP Value to Scalar Double-Precision FP Value */
// op cvtss2sd_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtss2sd_vdq_wdq.disasm = { _sink << "cvtss2sd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSS2SI -- Convert Scalar Single-Precision FP Value to Dword Integer */
// op cvtss2si_gd_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtss2si_gd_wdq.disasm = { _sink << "cvtss2si " << DisasmW( SSE(), rm ) << ',' << DisasmGd( gn ); };
/* CVTTPD2DQ -- Convert with Truncation Packed Double-Precision FP Values to Packed Dword Integers */
// op cvttpd2dq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttpd2dq_vdq_wdq.disasm = { _sink << "cvttpd2dq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTTPD2PI -- Convert with Truncation Packed Double-Precision FP Values to Packed Dword Integers */
// op cvttpd2pi_pq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttpd2pi_pq_wdq.disasm = { _sink << "cvttpd2pi " << DisasmW( SSE(), rm ) << ',' << DisasmPq( gn ); };
/* CVTTPS2DQ -- Convert with Truncation Packed Single-Precision FP Values to Packed Dword Integers */
// op cvttps2dq_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttps2dq_vdq_wdq.disasm = { _sink << "cvttps2dq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
/* CVTTPS2PI -- Convert with Truncation Packed Single-Precision FP Values to Packed Dword Integers */
// op cvttps2pi_pq_wdq( 0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttps2pi_pq_wdq.disasm = { _sink << "cvttps2pi " << DisasmW( SSE(), rm ) << ',' << DisasmPq( gn ); };
/* CVTTSD2SI -- Convert with Truncation Scalar Double-Precision FP Value to Integer */
// op cvttsd2si_gd_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttsd2si_gd_wdq.disasm = { _sink << "cvttsd2si " << DisasmW( SSE(), rm ) << ',' << DisasmGd( gn ); };
/* CVTTSS2SI -- Convert with Truncation Scalar Single-Precision FP Value to Dword Integer */
// op cvttss2si_gd_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttss2si_gd_wdq.disasm = { _sink << "cvttss2si " << DisasmW( SSE(), rm ) << ',' << DisasmGd( gn ); };


// /* DPPD -- Dot Product of Packed Double Precision Floating-Point Values */
// op dpps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x40[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// dpps_vdq_wdq_ib.disasm = { _sink << "dpps " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op dppd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x41[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// dppd_vdq_wdq_ib.disasm = { _sink << "dppd " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* EMMS -- Empty MMX Technology State */
// op emms( 0x0f[8]:> <:0x77[8] );
// 
// emms.disasm = { _sink << "emms"; };
// 
// /* EXTRACTPS -- Extract Packed Single Precision Floating-Point Value */
// op extractps_rd_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// extractps_rd_wdq_ib.disasm = { _sink << "extractps " << DisasmI(imm) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEd( rm ); };
// 
// /* FXSAVE/FXRSTOR -- Save/Restore x87 FPU, MMX Technology, and SSE State */
// op fxrstor( 0x0f[8]:> <:0xae[8]:> <:?[2]:1[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// fxrstor.disasm = { _sink << "fxrstor " << DisasmM( rm ); };
// 
// op fxsave( 0x0f[8]:> <:0xae[8]:> <:?[2]:0[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// fxsave.disasm = { _sink << "fxsave " << DisasmM( rm ); };
// 
// /* HADDPD -- Packed Single- or Double-FP Horizontal Add */
// op haddps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x7c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// haddps_vdq_wdq.disasm = { _sink << "haddps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op haddpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x7c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// haddpd_vdq_wdq.disasm = { _sink << "haddpd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* HSUBPD -- Packed Single- or Double-FP Horizontal Substract */
// op hsubps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x7d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// hsubps_vdq_wdq.disasm = { _sink << "hsubps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op hsubpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x7d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// hsubpd_vdq_wdq.disasm = { _sink << "hsubpd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* INSERTPS -- Insert Packed Single Precision Floating-Point Value */
// op insertps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x21[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// insertps_vdq_wdq_ib.disasm = { _sink << "insertps " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* LDDQU -- Load Unaligned Integer 128 Bits */
// op lddqu_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xf0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lddqu_vdq_wdq.disasm = { _sink << "lddqu " << DisasmM( rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MASKMOVDQU -- Store Selected Bytes of Double Quadword */
// op maskmovdqu_vdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xf7[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// maskmovdqu_vdq_vdq.disasm = { _sink << "maskmovdqu " << DisasmV( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MASKMOVQ */
// op maskmovq_pq_nq( 0x0f[8]:> <:0xf7[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// maskmovq_pq_nq.disasm = { _sink << "maskmovq " << DisasmPq( rm ) << ',' << DisasmPq( gn ); };
// 
// /* MOVD/MOVQ -- Move Doubleword/Move Quadword */
template <class ARCH, class GOP>
struct MovEP : public Operation<ARCH>
{
  MovEP( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::SIZE==64?'q':'d') << ' ' << DisasmPq( gn ) << ',' << DisasmE( GOP(), rm ); }
};

template <class ARCH, class GOP>
struct MovPE : public Operation<ARCH>
{
  MovPE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::SIZE==64?'q':'d') << ' ' << DisasmE( GOP(), rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH, class VR, class GOP>
struct MovEV : public Operation<ARCH>
{
  MovEV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex()?"v":"") << "mov" << SizeID<GOP::SIZE>::iid() << ' ' << DisasmV( VR(), gn ) << ',' << DisasmE( GOP(), rm ); }
  void execute( ARCH& arch ) const
  {
    arch.rmwrite( GOP(), rm, arch.vmm_read( VR(), gn, 0, typename TypeFor<ARCH,GOP::SIZE>::u() ) );
  }
};

template <class ARCH, class VR, class GOP>
struct MovVE : public Operation<ARCH>
{
  MovVE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex()?"v":"") << "mov" << SizeID<GOP::SIZE>::iid() << ' ' << DisasmE( GOP(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), gn, 0, arch.rmread( GOP(), rm ) );
    for (unsigned idx = 1, end = VR::size() / GOP::SIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, typename TypeFor<ARCH,GOP::SIZE>::u(0) );
  }
};

template <class ARCH, class VR>
struct MovQ : public Operation<ARCH>
{
  MovQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex()?"v":"") << "movq" << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), gn, 0, arch.vmm_read( VR(), rm, 0, typename ARCH::u64_t() ) );
    for (unsigned idx = 1, end = VR::size() / 64; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, typename ARCH::u64_t(0) );
  }
};

template <class ARCH> struct DC<ARCH,MOVQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex("\xf3\x0f\x7e") & RM() ))
    
    return newMovQ( ic, true, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex("\x66\x0f\xd6") & RM() ))

    return newMovQ( ic, false, _.opbase(), _.rmop(), _.greg() );

  return 0;
}
Operation<ARCH>* newMovQ( InputCode<ARCH> const& ic, bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn )
{
  if (not ic.vex())     return newMovQ<SSE>( load, opbase, rm, gn );
  if (ic.vreg()) return 0;
  if (ic.vlen() == 128) return newMovQ<XMM>( load, opbase, rm, gn );
  if (ic.vlen() == 256) return newMovQ<YMM>( load, opbase, rm, gn );
  return 0;
}
template <class VR>
Operation<ARCH>* newMovQ( bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn )
{
  if (load)
    return new MovQ<ARCH,VR>( opbase, rm, gn );
  
  RMOp<ARCH> _rm(rm);
  Operation<ARCH>* res = _rm.is_memory_operand()
    ? static_cast<Operation<ARCH>*>( new MovEV<ARCH,VR,GOq>( opbase, rm, gn ) )
    : new MovQ<ARCH,VR>( opbase, make_rop<ARCH>(gn), _rm.ereg() );
  _rm.release();
  return res;
}
};

template <class ARCH> struct DC<ARCH,MOVGV> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  bool vex_p = ic.vex(), quad_p = ic.w();
  if (not vex_p and not quad_p) return getMOVGV<SSE,GOd>( ic );
  if (not vex_p and     quad_p) return getMOVGV<SSE,GOq>( ic );
  if (    vex_p and (ic.vlen() != 128 or ic.vreg())) return 0;
  if (    vex_p and not quad_p) return getMOVGV<XMM,GOd>( ic );
  if (    vex_p and     quad_p) return getMOVGV<XMM,GOq>( ic );
  return 0;
}
template <typename VR, typename GOP> Operation<ARCH>* getMOVGV( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, simd__() & opcode( "\x0f\x6e" ) & RM() ))
  
    return ic.vex() ? 0 : new MovPE<ARCH,GOP>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd__() & opcode( "\x0f\x7e" ) & RM() ))
  
    return ic.vex() ? 0 : new MovEP<ARCH,GOP>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x6e" ) & RM() ))
  
    return new MovVE<ARCH,VR,GOP>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x7e" ) & RM() ))
  
    return new MovEV<ARCH,VR,GOP>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
};

struct   MovDQU { char const* name() { return   "movdqu"; } bool aligned() { return false; } };
struct   MovDQA { char const* name() { return   "movdqa"; } bool aligned() { return  true; } };
struct    LdDQU { char const* name() { return    "lddqu"; } bool aligned() { return false; } };
struct MovNTDQA { char const* name() { return "movntdqa"; } bool aligned() { return  true; } };
struct  MovNTDQ { char const* name() { return  "movntdq"; } bool aligned() { return  true; } };

template <class ARCH, class IMPL, class VR>
struct VLd : public Operation<ARCH>
{
  VLd( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; char const* mn; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << IMPL().name() << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size() / 64; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx,  arch.vmm_read( VR(), rm, idx, typename ARCH::u64_t() ) );
  }
};

template <class ARCH, class IMPL, class VR>
struct VSt : public Operation<ARCH>
{
  VSt( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << IMPL().name() << ' ' << DisasmV( VR(), gn ) << ',' << DisasmW( VR(), rm ); }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size() / 64; idx < end; ++idx)
      arch.vmm_write( VR(), rm, idx, arch.vmm_read( VR(), gn, idx, typename ARCH::u64_t() ) );
  }
};

/* MOVDQA/MOVDQU -- Move Aligned Unaligned Double Quadword */
/* LDDQU -- Load Unaligned Integer 128 Bits */
/* MOVNTDQA -- Load Double Quadword Non-Temporal Aligned Hint */
/* MOVNTDQ -- Store Double Quadword Using Non-Temporal Hint */
template <class ARCH> struct DC<ARCH,MOVDQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "\xf3\x0f\x6f" ) & RM() ))
    
    return newMov<MovDQU>( ic, true, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf3\x0f\x7f" ) & RM() ))
    
    return newMov<MovDQU>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x6f" ) & RM() ))
    
    return newMov<MovDQA>( ic, true, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x7f" ) & RM() ))
    
    return newMov<MovDQA>( ic, false, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\xf2\x0f\xf0" ) & RM() ))

    return newMov<LdDQU>( ic, false, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x2a" ) & RM() ))

    return newMov<MovNTDQA>( ic, true, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\xe7" ) & RM() ))

    return newMov<MovNTDQ>( ic, false, _.opbase(), _.rmop(), _.greg() );

  return 0;
}
template <class IMPL>
Operation<ARCH>* newMov( InputCode<ARCH> const& ic, bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn )
{
  if (not ic.vex())     return newMov2<IMPL,SSE>( load, opbase, rm, gn );
  if (ic.vreg()) return 0;
  if (ic.vlen() == 128) return newMov2<IMPL,XMM>( load, opbase, rm, gn );
  if (ic.vlen() == 256) return newMov2<IMPL,YMM>( load, opbase, rm, gn );
  return 0;
}
  template <class IMPL, class VR>
Operation<ARCH>* newMov2( bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn )
{
  if (load) return new VLd<ARCH,IMPL,VR>( opbase, rm, gn );
  else      return new VSt<ARCH,IMPL,VR>( opbase, rm, gn );
}
};

template <class ARCH, class VR, unsigned OPSIZE, class IMPL>
struct MovfpVW : public Operation<ARCH>
{
  MovfpVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex() ? "v" : "") << IMPL::name() << SizeID<OPSIZE>::fid() << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::f f_type;
    for (unsigned idx = 0, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx,  arch.vmm_read( VR(), rm, idx, f_type() ) );
  }
};

template <class ARCH, class VR, unsigned OPSIZE, class IMPL>
struct MovfpWV : public Operation<ARCH>
{
  MovfpWV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex() ? "v" : "") << IMPL::name() << SizeID<OPSIZE>::fid() << ' ' << DisasmV( VR(), gn ) << ',' << DisasmW( VR(), rm ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::f f_type;
    for (unsigned idx = 0, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), rm, idx,  arch.vmm_read( VR(), gn, idx, f_type() ) );
  }
};

struct MovAFP { static char const* name() { return "movap"; } static bool aligned() { return true; } };
struct MovUFP { static char const* name() { return "movup"; } static bool aligned() { return false; } };
struct StNTFP { static char const* name() { return "movntp"; } static bool aligned() { return true; } };

/* MOVAP -- Move Aligned Packed Single- or Double-Precision Floating-Point Values */
/* MOVUP -- Move Unaligned Packed Single- or Double-Precision Floating-Point Values */
/* MOVNTPD -- Store Packed Single- or Double-Precision Floating-Point Values Using Non-Temporal Hint */
template <class ARCH> struct DC<ARCH,MOVFP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "\x0f\x28" ) & RM() ))

    return newMovfp<32,MovAFP>( ic, true, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x0f\x29" ) & RM() ))

    return newMovfp<32,MovAFP>( ic, false, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x28" ) & RM() ))

    return newMovfp<64,MovAFP>( ic, true, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x29" ) & RM() ))

    return newMovfp<64,MovAFP>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x0f\x10" ) & RM() ))

    return newMovfp<32,MovUFP>( ic, true, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x0f\x11" ) & RM() ))

    return newMovfp<32,MovUFP>( ic, false, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x10" ) & RM() ))

    return newMovfp<64,MovUFP>( ic, true, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x11" ) & RM() ))

    return newMovfp<64,MovUFP>( ic, false, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x0f\x2b" ) & RM() ))

    return newMovfp<32,StNTFP>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x2b" ) & RM() ))

    return newMovfp<64,StNTFP>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  
  return 0;
}
template <unsigned OPSIZE, class IMPL>
Operation<ARCH>* newMovfp( InputCode<ARCH> const& ic, bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned gn )
{
  if (not ic.vex())     return newMovfp<SSE,OPSIZE,IMPL>( load, opbase, rm, gn );
  if (ic.vreg())         return 0;
  if (ic.vlen() == 128) return newMovfp<XMM,OPSIZE,IMPL>( load, opbase, rm, gn );
  if (ic.vlen() == 256) return newMovfp<YMM,OPSIZE,IMPL>( load, opbase, rm, gn );
  return 0;
}
template <class VR, unsigned OPSIZE, class IMPL>
Operation<ARCH>* newMovfp( bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned gn )
{
  if (load) return new MovfpVW<ARCH,VR,OPSIZE,IMPL>( opbase, rm, gn );
  /*else*/  return new MovfpWV<ARCH,VR,OPSIZE,IMPL>( opbase, rm, gn );
}  
};

template <class ARCH, class VR, unsigned OPSIZE>
struct MovfpcVW : public Operation<ARCH>
{
  MovfpcVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn, bool _hi ) : Operation<ARCH>(opbase), rm(_rm), vn(_vn), gn(_gn), hi(_hi) {} RMOp<ARCH> rm; uint8_t vn, gn; bool hi;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << (hi ? "movhp" : "movlp") << SizeID<OPSIZE>::fid() << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::f f_type;
    for (unsigned idx = 0, end = VR::size()/2/OPSIZE, withrm = hi*end, withvn = end-withrm; idx < end; ++idx)
      {
        arch.vmm_write( VR(), gn, idx + withrm, arch.vmm_read( VR(), rm, idx, f_type() ) );
        arch.vmm_write( VR(), gn, idx + withvn, arch.vmm_read( VR(), vn, idx, f_type() ) );
      }
  }
};

template <class ARCH, class VR, unsigned OPSIZE>
struct MovfpcWV : public Operation<ARCH>
{
  MovfpcWV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, bool _hi ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn), hi(_hi) {} RMOp<ARCH> rm; uint8_t gn; bool hi;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << (hi ? "movhp" : "movlp") << SizeID<OPSIZE>::fid() << ' ' << DisasmV( VR(), gn ) << ',' << DisasmW( VR(), rm );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::f f_type;
    for (unsigned idx = 0, end = VR::size()/2/OPSIZE, mux = hi*end; idx < end; ++idx)
      arch.vmm_write( VR(), rm, idx, arch.vmm_read( VR(), gn, idx + mux, f_type() ) );
  }
};

// /* MOVLPD -- Move Low Packed Double-Precision Floating-Point Value */
// /* MOVLPS -- Move Low Packed Single-Precision Floating-Point Value */
// /* MOVHPD -- Move High Packed Double-Precision Floating-Point Value */
// /* MOVHPS -- Move High Packed Single-Precision Floating-Point Value */
template <class ARCH> struct DC<ARCH,MOVFPC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x0f\x12" ) & RM() ))

    return newMovFPC<32>( ic, true, _.opbase(), _.rmop(), _.greg(), 0 );

  if (auto _ = match( ic, vex( "\x0f\x13" ) & RM() ))

    return newMovFPC<32>( ic, false, _.opbase(), _.rmop(), _.greg(), 0 );

  if (auto _ = match( ic, vex( "\x66\x0f\x12" ) & RM() ))

    return newMovFPC<64>( ic, true, _.opbase(), _.rmop(), _.greg(), 0 );

  if (auto _ = match( ic, vex( "\x66\x0f\x13" ) & RM() ))

    return newMovFPC<64>( ic, false, _.opbase(), _.rmop(), _.greg(), 0 );

  if (auto _ = match( ic, vex( "\x0f\x16" ) & RM() ))

    return newMovFPC<32>( ic, true, _.opbase(), _.rmop(), _.greg(), 1 );

  if (auto _ = match( ic, vex( "\x0f\x17" ) & RM() ))

    return newMovFPC<32>( ic, false, _.opbase(), _.rmop(), _.greg(), 1 );

  if (auto _ = match( ic, vex( "\x66\x0f\x16" ) & RM() ))

    return newMovFPC<64>( ic, true, _.opbase(), _.rmop(), _.greg(), 1 );

  if (auto _ = match( ic, vex( "\x66\x0f\x17" ) & RM() ))

    return newMovFPC<64>( ic, false, _.opbase(), _.rmop(), _.greg(), 1 );

  return 0;
}
template <unsigned OPSIZE>
Operation<ARCH>* newMovFPC( InputCode<ARCH> const& ic, bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned gn, unsigned lohi )
{
  unsigned vn = 0;
  if (not ic.vex())     return newMovFPC<SSE,OPSIZE>( load, opbase, rm, vn, gn, lohi );
  if ((vn = ic.vreg()) and not load) return 0;
  if (ic.vlen() == 128) return newMovFPC<XMM,OPSIZE>( load, opbase, rm, vn, gn, lohi );
  return 0;
}
template <class VR, unsigned OPSIZE>
Operation<ARCH>* newMovFPC( bool load, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned vn, unsigned gn, unsigned lohi )
{
  if (load) return new MovfpcVW<ARCH,VR,OPSIZE>( opbase, rm, vn, gn, lohi );
  else      return new MovfpcWV<ARCH,VR,OPSIZE>( opbase, rm, gn, lohi );
}
};

// /* MOVDQ2Q -- Move Quadword from XMM to MMX Technology Register */
// op movdq2q_pq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x2d[8]:> <:0b11[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM] );
// 
// movdq2q_pq_vdq.disasm = { _sink << "movdq2q " << DisasmV( SSE(), rm ) << ',' << DisasmPq( gn ); };
// 
// /* MOVHLPS -- Move Packed Single-Precision Floating-Point Values High to Low */
// op movhlps_vdq_vdq( 0x0f[8]:> <:0x12[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movhlps_vdq_vdq.disasm = { _sink << "movhlps " << DisasmV( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVLHPS -- Move Packed Single-Precision Floating-Point Values Low to High */
// op movlhps_vdq_wdq( 0x0f[8]:> <:0x16[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movlhps_vdq_wdq.disasm = { _sink << "movlhps " << DisasmV( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVMSKPD -- Extract Packed Single- or Double-Precision Floating-Point Sign Mask */
// op movmskpd_gd_vdq( 0x66[8]:> <:0x0f[8]:> <:0x50[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movmskpd_gd_vdq.disasm = { _sink << "movmskpd " << DisasmV( SSE(), rm ) << ',' << DisasmGd( gn ); };
// 
// op movmskps_gd_vdq( 0x0f[8]:> <:0x50[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movmskps_gd_vdq.disasm = { _sink << "movmskps " << DisasmV( SSE(), rm ) << ',' << DisasmGd( gn ); };
// 
// /* MOVNTPD -- Store Packed Single- or Double-Precision Floating-Point Values Using Non-Temporal Hint */
// op movntpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntpd_mdq_vdq.disasm = { _sink << "movntpd " << DisasmV( SSE(), gn ) << ',' << DisasmM( rm ); };
// 
// op movntps_mdq_vdq( 0x0f[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntps_mdq_vdq.disasm = { _sink << "movntps " << DisasmV( SSE(), gn ) << ',' << DisasmM( rm ); };
// 
// /* MOVNTQ -- Store Double Quadword Using Non-Temporal Hint */
// op movntq_mq_pq( 0x0f[8]:> <:0xe7[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntq_mq_pq.disasm = { _sink << "movntq " << DisasmPq( gn ) << ',' << DisasmM( rm ); };
// 
// /* MOVQ -- Move Quadword (MMX) */
// op movq_pq_mq( 0x0f[8]:> <:0x6f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_pq_mq.disasm = { _sink << "movq " << DisasmM( rm ) << ',' << DisasmPq( gn ); };
// 
// op movq_mq_pq( 0x0f[8]:> <:0x7f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_mq_pq.disasm = { _sink << "movq " << DisasmPq( gn ) << ',' << DisasmM( rm ); };
// 
// op movq_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x7e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_vdq_wdq.disasm = { _sink << "movq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op movq_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xd6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_wdq_vdq.disasm = { _sink << "movq " << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rm ); };
// 
// /* MOVQ2DQ -- Move Quadword from MMX Technology to XMM Register */
// op movq2dq_vdq_pq( 0xf3[8]:> <:0x0f[8]:> <:0xd6[8]:> <:0b11[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM] );
// 
// movq2dq_vdq_pq.disasm = { _sink << "movq2dq " << DisasmV( SSE(), gn ) << ',' << DisasmPq( rm ); };
//

// /* MOVSS -- Move Scalar Single-Precision Floating-Point Values */
// /* MOVSD -- Move Scalar Double-Precision Floating-Point Value */

template <class VR, unsigned OPSIZE>
struct LSMnemo
{
  typedef LSMnemo<VR,OPSIZE> this_type;
  friend std::ostream& operator << ( std::ostream& sink, this_type const& ) { sink << (VR::vex() ? "v" : "") << "movs" << SizeID<OPSIZE>::fid(); return sink; };
  this_type const& mnemonic() const { return *this; }
};

template <class ARCH, class VR, unsigned OPSIZE>
struct LSFPS : public Operation<ARCH>
{
  LSFPS( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {}
  RMOp<ARCH> rm; unsigned gn;
};

template <class ARCH, class VR, unsigned OPSIZE>
struct StoreFPS : public LSFPS<ARCH,VR,OPSIZE>, public LSMnemo<VR,OPSIZE>
{
  typedef LSFPS<ARCH,VR,OPSIZE> LSBASE; using LSBASE::LSFPS; using LSBASE::rm; using LSBASE::gn;
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' ' << DisasmV( VR(), gn ) << ',' << DisasmW( VR(), rm ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), rm, 0, arch.vmm_read( VR(), gn, 0, typename TypeFor<ARCH,OPSIZE>::f() ) );
  }
};

template <class ARCH, class VR, unsigned OPSIZE>
struct LoadFPS : public LSFPS<ARCH,VR,OPSIZE>, public LSMnemo<VR,OPSIZE>
{
  typedef LSFPS<ARCH,VR,OPSIZE> LSBASE; using LSBASE::LSFPS; using LSBASE::rm; using LSBASE::gn;
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), gn, 0, arch.vmm_read( VR(), rm, 0, typename TypeFor<ARCH,OPSIZE>::f() ) );
    for (unsigned idx = 1, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, typename TypeFor<ARCH,OPSIZE>::f(0) );
  }
};

template <class ARCH, unsigned OPSIZE>
struct MergeFPS : public Operation<ARCH>, public LSMnemo<XMM,OPSIZE>
{
  MergeFPS(OpBase<ARCH> const& opbase, unsigned _dst, unsigned _src1, unsigned _src2)
    : Operation<ARCH>(opbase), dst(_dst), src1(_src1), src2(_src2)
  {}
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' ' << DisasmV( XMM(), src1 ) << ',' << DisasmV( XMM(), src2 ) << ',' << DisasmV( XMM(), dst ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( XMM(), dst, 0, arch.vmm_read( XMM(), src2, 0, typename TypeFor<ARCH,OPSIZE>::f() ) );
    for (unsigned idx = 1, end = XMM::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( XMM(), dst, idx, arch.vmm_read( XMM(), src1, idx, typename TypeFor<ARCH,OPSIZE>::f() ) );
  }
  uint8_t dst, src1, src2;
};


template <class ARCH> struct DC<ARCH,VMOVSD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\xf3\x0f\x10" ) & RM() ))
    
    return newMovs<32>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf3\x0f\x11" ) & RM() ))
    
    return newMovs<32>( ic,  true, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf2\x0f\x10" ) & RM() ))
    
    return newMovs<64>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf2\x0f\x11" ) & RM() ))
    
    return newMovs<64>( ic,  true, _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
template <unsigned OPSIZE>
Operation<ARCH>* newMovs( InputCode<ARCH> const& ic, bool store, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned gn )
{
  bool is_mem; { RMOp<ARCH> _rm(rm); is_mem = _rm.is_memory_operand(); _rm.release(); }

  if (is_mem ? store : not ic.vex())
    {
      // Store
      if      (not ic.vex())  return new StoreFPS<ARCH,SSE,OPSIZE>( opbase, rm, gn );
      else if (not ic.vreg()) return new StoreFPS<ARCH,XMM,OPSIZE>( opbase, rm, gn );
      else                    return 0;
    }

  if (is_mem)
    {
      // Load
      if      (not ic.vex())  return new LoadFPS<ARCH,SSE,OPSIZE>( opbase, rm, gn );
      else if (not ic.vreg()) return new LoadFPS<ARCH,XMM,OPSIZE>( opbase, rm, gn );
      else                    return 0;
    }

  // Merger (AVX only)
  unsigned dst = gn, src1 = ic.vreg(), src2 = unsigned(uintptr_t(rm));
  if (store) std::swap(dst,src2);
  return new MergeFPS<ARCH,OPSIZE>( opbase, dst, src1, src2 );
}
};

// /* MOVSHDUP -- Move Packed Single-FP High and Duplicate */
// op movshdup_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movshdup_vdq_wdq.disasm = { _sink << "movshdup " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVSLDUP -- Move Packed Single-FP Low and Duplicate */
// op movsldup_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movsldup_vdq_wdq.disasm = { _sink << "movsldup " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MPSADBW -- Compute Multiple Packed Sums of Absolute Difference */
// op mpsadbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x42[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mpsadbw_vdq_wdq.disasm = { _sink << "mpsadbw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };

struct VADD { char const* n() { return "add"; } }; struct VSUB { char const* n() { return "sub"; } };
struct VMUL { char const* n() { return "mul"; } }; struct VDIV { char const* n() { return "div"; } };
struct VMIN { char const* n() { return "min"; } }; struct VMAX { char const* n() { return "max"; } };

template <class ARCH, class OPERATION, class VR, unsigned OPSZ, bool PACKED>
struct VArithmeticVVW : public Operation<ARCH>
{
  typedef typename TypeFor<ARCH,OPSZ>::f valtype;
  valtype eval ( VADD const&, valtype const& src1, valtype const& src2 ) const { return src1 + src2; }
  valtype eval ( VDIV const&, valtype const& src1, valtype const& src2 ) const { return src1 / src2; }
  valtype eval ( VMAX const&, valtype const& src1, valtype const& src2 ) const { return Maximum(src1, src2); }
  valtype eval ( VMIN const&, valtype const& src1, valtype const& src2 ) const { return Minimum(src1, src2); }
  valtype eval ( VMUL const&, valtype const& src1, valtype const& src2 ) const { return src1 * src2; }
  valtype eval ( VSUB const&, valtype const& src1, valtype const& src2 ) const { return src1 - src2; }
  

  VArithmeticVVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _vn, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), vn(_vn), gn(_gn) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << OPERATION().n() << (PACKED ? "p" : "s") << (OPSZ == 32 ? "s" : "d")
         <<                ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = PACKED ? VR::size()/OPSZ : 1; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, eval( OPERATION(), arch.vmm_read( VR(), vn, idx, valtype() ), arch.vmm_read( VR(), rm, idx, valtype() ) ) );
    for (unsigned idx = PACKED ? VR::size()/OPSZ : 1, end = VR::size()/OPSZ; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), vn, idx, valtype() ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* ADD -- Add Scalar or Packed Single- or Double-Precision Floating-Point Values */
/* MUL -- Multiply Scalar or Packed Single- or Double-Precision Floating-Point Values */
/* SUB -- Substract Scalar or Packed Single- or Double-Precision Floating-Point Values */
/* MIN -- Return Minimum Scalar or Packed Single- or Double-Precision Floating-Point Values */
/* DIV -- Divide Scalar or Packed Single- or Double-Precision Floating-Point Values */
/* MAX -- Return Maximum Scalar or Packed Single- or Double-Precision Floating-Point Values */

template <class ARCH> struct DC<ARCH,VFP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x58" ) & RM() ))

    return newVFP<VADD>(ic, _.opbase(), _.greg(), _.rmop());
  
  if (auto _ = match( ic, vex( "*\x0f\x59" ) & RM() ))

    return newVFP<VMUL>(ic, _.opbase(), _.greg(), _.rmop());
  
  if (auto _ = match( ic, vex( "*\x0f\x5c" ) & RM() ))

    return newVFP<VSUB>(ic, _.opbase(), _.greg(), _.rmop());
    
  if (auto _ = match( ic, vex( "*\x0f\x5d" ) & RM() ))

    return newVFP<VMIN>(ic, _.opbase(), _.greg(), _.rmop());
      
  if (auto _ = match( ic, vex( "*\x0f\x5e" ) & RM() ))

    return newVFP<VDIV>(ic, _.opbase(), _.greg(), _.rmop());

  if (auto _ = match( ic, vex( "*\x0f\x5f" ) & RM() ))

    return newVFP<VMAX>(ic, _.opbase(), _.greg(), _.rmop());
  
  return 0;
}
template <class OPERATION> Operation<ARCH>* newVFP( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, unsigned gn, MOp<ARCH> const* rm )
{
  if (not ic.vex())     return newVArithmeticVVW<OPERATION,SSE>( ic, opbase, gn, gn, rm );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return newVArithmeticVVW<OPERATION,XMM>( ic, opbase, gn, vn, rm );
  if (ic.vlen() == 256) return newVArithmeticVVW<OPERATION,YMM>( ic, opbase, gn, vn, rm );
  return 0;
}
template <class OPERATION, class VR> Operation<ARCH>*
newVArithmeticVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, unsigned gn, unsigned vn, MOp<ARCH> const* rm )
{
  if (match( ic, simd__() )) return new VArithmeticVVW<ARCH,OPERATION,VR,32, true>( opbase, rm, vn, gn );
  if (match( ic, simd66() )) return new VArithmeticVVW<ARCH,OPERATION,VR,64, true>( opbase, rm, vn, gn );
  if (match( ic, simdF3() )) return newScalarVArithmeticVVW<OPERATION,VR,32>( opbase, gn, vn, rm );
  if (match( ic, simdF2() )) return newScalarVArithmeticVVW<OPERATION,VR,64>( opbase, gn, vn, rm );
  return 0;
}
template <class OPERATION, class VR, unsigned OPSZ> Operation<ARCH>*
newScalarVArithmeticVVW( OpBase<ARCH> const& opbase, unsigned gn, unsigned vn, MOp<ARCH> const* rm )
{
  if (VR::vex()) return new VArithmeticVVW<ARCH,OPERATION,XMM,OPSZ,false>( opbase, rm, vn, gn );
  return                new VArithmeticVVW<ARCH,OPERATION,SSE,OPSZ,false>( opbase, rm, vn, gn );
}
};

template <class ARCH, class VR, unsigned SOPSZ, unsigned DOPSZ>
struct VFPCvtp : public Operation<ARCH>
{
  typedef typename TypeFor<ARCH,SOPSZ>::f srctype;
  typedef typename TypeFor<ARCH,DOPSZ>::f dsttype;
  
  VFPCvtp( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "vcvt" : "cvt") << (SOPSZ == 32 ? "ps" : "pd") << '2' << (DOPSZ == 32 ? "ps" : "pd")
         << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    unsigned const dcount = VR::size()/DOPSZ;
    unsigned const scount = VR::size()/SOPSZ;

    dsttype res[dcount];
    unsigned idx = 0;
    for (unsigned end = std::min(scount,dcount); idx < end; ++idx)
      res[idx] = dsttype( arch.vmm_read( VR(), rm, idx, srctype() ) );
    for (; idx < dcount; ++ idx)
      res[idx] = dsttype(0);
    for (idx = 0; idx < dcount; ++ idx)
      arch.vmm_write( VR(), gn, idx, res[idx] );
  }
  RMOp<ARCH> rm; uint8_t gn;
};

template <class ARCH, class VR, unsigned SOPSZ, unsigned DOPSZ>
struct VFPCvts : public Operation<ARCH>
{
  typedef typename TypeFor<ARCH,SOPSZ>::f srctype;
  typedef typename TypeFor<ARCH,DOPSZ>::f dsttype;
  
  VFPCvts( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _vn, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), vn(_vn), gn(_gn) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "vcvt" : "cvt") << (SOPSZ == 32 ? "ss" : "sd") << '2' << (DOPSZ == 32 ? "ss" : "sd")
         <<                ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), gn, 0, dsttype( arch.vmm_read( VR(), rm, 0, srctype() ) ) );
    for (unsigned idx = 1, end = VR::size()/DOPSZ; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), vn, idx, dsttype() ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* FP conversions */

template <class ARCH> struct DC<ARCH,VFPCVT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x5a" ) & RM() ))
    {
      unsigned gn = _.greg();
      if (not ic.vex())     return newVFPCvt<SSE>( ic, _.opbase(), _.rmop(), gn, gn );
      unsigned vn = ic.vreg();
      if (ic.vlen() == 128) return newVFPCvt<XMM>( ic, _.opbase(), _.rmop(), vn, gn );
      if (ic.vlen() == 256) return newVFPCvt<YMM>( ic, _.opbase(), _.rmop(), vn, gn );
      return 0;
      return 0;
    }
  
  return 0;
}
template <class VR>
Operation<ARCH>*
newVFPCvt( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t vn, uint8_t gn )
{
  if (match( ic, simdF3() )) return new VFPCvts<ARCH,VR,32,64>( opbase, rm, vn, gn );
  if (match( ic, simdF2() )) return new VFPCvts<ARCH,VR,64,32>( opbase, rm, vn, gn );
  if (vn and ic.vex()) return 0; // No vreg for packed operation
  if (match( ic, simd__() )) return new VFPCvtp<ARCH,VR,32,64>( opbase, rm, gn );
  if (match( ic, simd66() )) return new VFPCvtp<ARCH,VR,64,32>( opbase, rm, gn );
  return 0;
}
};

// Todo: Fusion all binary operation (FP and Int)
template <class ARCH, class VR, unsigned OPSZ, class OPERATION>
struct VIntBin : public Operation<ARCH>
{
  typedef typename TypeFor<ARCH,OPSZ>::u valtype;
  valtype eval ( VADD const&, valtype const& src1, valtype const& src2 ) const { return src1 + src2; }
  // valtype eval ( VDIV const&, valtype const& src1, valtype const& src2 ) const { return src1 / src2; }
  // valtype eval ( VMAX const&, valtype const& src1, valtype const& src2 ) const { return std::max(src1, src2); }
  // valtype eval ( VMIN const&, valtype const& src1, valtype const& src2 ) const { return std::min(src1, src2); }
  // valtype eval ( VMUL const&, valtype const& src1, valtype const& src2 ) const { return src1 * src2; }
  valtype eval ( VSUB const&, valtype const& src1, valtype const& src2 ) const { return src1 - src2; }

  VIntBin( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _vn, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), vn(_vn), gn(_gn) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "vp" : "p") << OPERATION().n() << SizeID<OPSZ>::iid()
         <<                ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size()/OPSZ; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, eval( OPERATION(), arch.vmm_read( VR(), vn, idx, valtype() ), arch.vmm_read( VR(), rm, idx, valtype() ) ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

template <class ARCH> struct DC<ARCH,VINTBIN> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\xfc" ) & RM() ))

    return newVIntBin<VADD, 8>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xfd" ) & RM() ))

    return newVIntBin<VADD,16>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xfe" ) & RM() ))

    return newVIntBin<VADD,32>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xd4" ) & RM() ))

    return newVIntBin<VADD,64>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xf8" ) & RM() ))

    return newVIntBin<VSUB, 8>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xf9" ) & RM() ))

    return newVIntBin<VSUB,16>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xfa" ) & RM() ))

    return newVIntBin<VSUB,32>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\xfb" ) & RM() ))

    return newVIntBin<VSUB,64>( ic, _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
template <class OPERATION, unsigned OPSIZE>
Operation<ARCH>* newVIntBin( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned gn )
{
  if (not ic.vex())     return new VIntBin<ARCH,SSE,OPSIZE,OPERATION>( opbase, rm, gn, gn );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new VIntBin<ARCH,XMM,OPSIZE,OPERATION>( opbase, rm, vn, gn );
  if (ic.vlen() == 256) return new VIntBin<ARCH,YMM,OPSIZE,OPERATION>( opbase, rm, vn, gn );
  return 0;
}
};

// /* PABSB/PABSW/PABSD -- Packed Absolute Value */
// op pabsb_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x1c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsb_pq_qq.disasm = { _sink << "pabsb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pabsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x1d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsw_pq_qq.disasm = { _sink << "pabsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pabsd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x1e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsd_pq_qq.disasm = { _sink << "pabsd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pabsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsb_vdq_wdq.disasm = { _sink << "pabsb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pabsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsw_vdq_wdq.disasm = { _sink << "pabsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pabsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsd_vdq_wdq.disasm = { _sink << "pabsd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PACKSSWB/PACKSSDW -- Pack with Signed Saturation */
// op packsswb_pq_qq( 0x0f[8]:> <:0x63[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packsswb_pq_qq.disasm = { _sink << "packsswb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op packssdw_pq_qq( 0x0f[8]:> <:0x6b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packssdw_pq_qq.disasm = { _sink << "packssdw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op packsswb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x63[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packsswb_vdq_wdq.disasm = { _sink << "packsswb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op packssdw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x6b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packssdw_vdq_wdq.disasm = { _sink << "packssdw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PACKUSWB/PACKUSDW -- Pack with Unsigned Saturation */
// op packuswb_pq_qq( 0x0f[8]:> <:0x67[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packuswb_pq_qq.disasm = { _sink << "packuswb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op packuswb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x67[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packuswb_vdq_wdq.disasm = { _sink << "packuswb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op packusdw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packusdw_vdq_wdq.disasm = { _sink << "packusdw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PADDB/PADDW/PADDD/PADDQ -- Add Packed Integers */
// op paddb_pq_qq( 0x0f[8]:> <:0xfc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddb_pq_qq.disasm = { _sink << "paddb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddw_pq_qq( 0x0f[8]:> <:0xfd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddw_pq_qq.disasm = { _sink << "paddw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddd_pq_qq( 0x0f[8]:> <:0xfe[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddd_pq_qq.disasm = { _sink << "paddd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddq_pq_qq( 0x0f[8]:> <:0xd4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddq_pq_qq.disasm = { _sink << "paddq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddb_vdq_wdq.disasm = { _sink << "paddb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddw_vdq_wdq.disasm = { _sink << "paddw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfe[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddd_vdq_wdq.disasm = { _sink << "paddd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddq_vdq_wdq.disasm = { _sink << "paddq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PADDSB/PADDSW -- Add Packed Signed Integers with Signed Saturation */
// op paddsb_pq_qq( 0x0f[8]:> <:0xec[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsb_pq_qq.disasm = { _sink << "paddsb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddsw_pq_qq( 0x0f[8]:> <:0xed[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsw_pq_qq.disasm = { _sink << "paddsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xec[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsb_vdq_wdq.disasm = { _sink << "paddsb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xed[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsw_vdq_wdq.disasm = { _sink << "paddsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PADDUSB/PADDUSW -- Add Packed Unsigned Integers with Unsigned Saturation */
// op paddusb_pq_qq( 0x0f[8]:> <:0xdc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusb_pq_qq.disasm = { _sink << "paddusb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddusw_pq_qq( 0x0f[8]:> <:0xdd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusw_pq_qq.disasm = { _sink << "paddusw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op paddusb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusb_vdq_wdq.disasm = { _sink << "paddusb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddusw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusw_vdq_wdq.disasm = { _sink << "paddusw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PAVG -- Average Packed Integers */
// op pavgb_pq_qq( 0x0f[8]:> <:0xe0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgb_pq_qq.disasm = { _sink << "pavgb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pavgw_pq_qq( 0x0f[8]:> <:0xe3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgw_pq_qq.disasm = { _sink << "pavgw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pavgb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgb_vdq_wdq.disasm = { _sink << "pavgb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pavgw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgw_vdq_wdq.disasm = { _sink << "pavgw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PBLENDVB -- Variable Blend Packed Bytes */
// op pblenvb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pblenvb_vdq_wdq.disasm = { _sink << "pblenvb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PBLENDW -- Blend Packed Words */
// op pblendw_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pblendw_vdq_wdq_ib.disasm = { _sink << "pblendw " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCLMULQDQ - Carry-Less Multiplication Quadword */
// op pclmulqdq_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x44[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pclmulqdq_vdq_wdq_ib.disasm = { _sink << "pclmulqdq " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };

namespace PCmpStrUtil
{
  template <typename ARCH, unsigned COUNT>
  void explicit_validity( typename ARCH::bit_t(&result)[COUNT], ARCH& arch, unsigned reg )
  {
    typename ARCH::gr_type count = arch.regread( typename ARCH::GR(), reg );
    if (arch.Test(count < typename ARCH::gr_type(0))) count = -count;
    for (unsigned idx = 0; idx < COUNT; ++idx)
      result[idx] = typename ARCH::gr_type(idx) < count;
  }
}

template <class ARCH, class VR, class TYPE>
struct PCmpStr : public Operation<ARCH>
{
  enum { is_signed = atpinfo<ARCH,TYPE>::is_signed, bitsize = atpinfo<ARCH,TYPE>::bitsize, count = VR::SIZE / bitsize };
  
  PCmpStr( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, uint8_t _im, bool _implicit, bool _index )
    : Operation<ARCH>( opbase ), rm(_rm), gn(_gn), im(_im), implicit(_implicit), index(_index)
  {}

  RMOp<ARCH> rm;
  uint8_t    gn;
  uint8_t    im;
  bool       implicit;
  bool       index;
  
  enum Cmp { EqualAny = 0, Ranges = 0b01, EqualEach = 0b10, EqualOrdered = 0b11 }; Cmp comparison() const { return Cmp((im >> 2) & 3); }
  enum Neg { NegateNothing = 0, NegateAll = 0b01, NegateValid = 0b11 };            Neg negation()   const { return Neg((im&0x20) ? NegateValid : (im&0x10) ? NegateAll : NegateNothing); }
  enum Res { BitMask = 0, CharMask = 0b01, MaskLSB = 0b10, MaskMSB = 0b11 };       Res result()     const { return Res((index << 1) | ((im >> 6) & 1)); }

  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "pcmp" << (implicit ? 'i' : 'e') << "str" << (index ? 'i' : 'm') << ' ' << DisasmI(im) << ',' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::bit_t bit_t;
    TYPE const null_char(0);
    bit_t gnv[count], rmv[count], intres[count];
    if (implicit)
      {
        bit_t rmv_b(true), gnv_b(true);
        for (unsigned idx = 0; idx < count; ++idx)
          {
            gnv_b = gnv[idx] = gnv_b and (arch.vmm_read( VR(), gn, idx, null_char ) != null_char);
            rmv_b = rmv[idx] = rmv_b and (arch.vmm_read( VR(), rm, idx, null_char ) != null_char);
          }
      }
    else
      {
        PCmpStrUtil::explicit_validity( gnv, arch, 0 );
        PCmpStrUtil::explicit_validity( rmv, arch, 2 );
      }

    switch (comparison())
      {
      case EqualAny: // find characters from a set
        for (unsigned rmi = 0; rmi < count; ++rmi)
          {
            bit_t res( false );
            TYPE rmval = arch.vmm_read( VR(), rm, rmi, null_char );
            for (unsigned gni = 0; gni < count; ++gni)
              res = res or ((arch.vmm_read( VR(), gn, gni, null_char ) == rmval) and gnv[gni]);
            intres[rmi] = res and rmv[rmi];
          }
        break;
        
      case Ranges: // find characters from ranges
        for (unsigned rmi = 0; rmi < count; ++rmi) // Reg/Mem
          {
            bit_t res( false );
            TYPE rmval = arch.vmm_read( VR(), rm, rmi, null_char );
            for (unsigned gni = 0; gni < count; gni += 2) // Reg
              {
                bit_t lower = (arch.vmm_read( VR(), gn, gni+0, null_char ) >= rmval);
                bit_t upper = (arch.vmm_read( VR(), gn, gni+1, null_char ) <= rmval);
                res = res or (lower and gnv[gni+0] and upper and gnv[gni+1]);
              }
            intres[rmi] = res and rmv[rmi];
          }
        break;
        
      case EqualEach: // string compare
        for (unsigned idx = 0; idx < count; ++idx) // Reg/Mem
          intres[idx] = ((arch.vmm_read( VR(), gn, idx, null_char ) == arch.vmm_read( VR(), rm, idx, null_char )) and gnv[idx] and rmv[idx]) or (not gnv[idx] and not rmv[idx]);
        break;
        
      case EqualOrdered: // substring search
        for (unsigned idx = 0; idx < count; ++idx)
          {
            bit_t res( true );
            for (unsigned rmi = idx, gni = 0; rmi < count; ++rmi, ++gni)
              res = res and (((arch.vmm_read( VR(), gn, gni, null_char ) == arch.vmm_read( VR(), rm, rmi, null_char )) and gnv[gni] and rmv[rmi]) or (not gnv[idx] and not rmv[idx]));
            intres[idx] = res;
          }
        break;
      }

    switch (negation())
      {
      case NegateAll:   for (unsigned idx = 0; idx < count; ++idx) intres[idx] = not intres[idx]; break;
      case NegateValid: for (unsigned idx = 0; idx < count; ++idx) intres[idx] = intres[idx] xor rmv[idx]; break;
      case NegateNothing: break;
      }

    switch (result())
      {
      case BitMask:
        {
          typedef typename TypeFor<ARCH,count>::u bf_type;
          bf_type bf(0);
          for (unsigned idx = 0; idx < count; ++idx)
            bf |= bf_type(intres[idx]) << idx;
          arch.vmm_write( VR(), 0, 0, bf );
          for (unsigned idx = 1; idx < bitsize; ++idx)
            arch.vmm_write( VR(), 0, 0, bf_type(0) );
        }
        break;
      case CharMask:
        for (unsigned idx = 0; idx < count; ++idx)
          arch.vmm_write( VR(), 0, idx, TYPE(TYPE(not intres[idx]) - TYPE(1)) );
        break;
      case MaskLSB:
        for (unsigned idx = 0; idx < count; ++idx)
          if (arch.Test( intres[idx] ))
            arch.regwrite( typename ARCH::GR(), 1, typename ARCH::gr_type(idx) );
        break;
      case MaskMSB:
        for (int idx = count; --idx >= 0;)
          if (arch.Test( intres[idx] ))
            arch.regwrite( typename ARCH::GR(), 1, typename ARCH::gr_type(idx) );
        break;
      }
  }
};

// /* PCMPESTRM -- Packed Compare Explicit Length Strings, Return Mask */
// /* PCMPESTRI -- Packed Compare Explicit Length Strings, Return Index */
// /* PCMPISTRM -- Packed Compare Implicit Length Strings, Return Mask */
// /* PCMPISTRI -- Packed Compare Implicit Length Strings, Return Index */

template <class ARCH> struct DC<ARCH,PCMPSTR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x60" ) & RM() & Imm<8>() ))

    return newPCmpStr( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()), false, false );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x61" ) & RM() & Imm<8>() ))

    return newPCmpStr( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()), false, true );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x62" ) & RM() & Imm<8>() ))

    return newPCmpStr( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()),  true, false );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x63" ) & RM() & Imm<8>() ))
    
    return newPCmpStr( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()),  true, true );

  return 0;
}
Operation<ARCH>* newPCmpStr( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn, uint8_t im, bool implicit, bool index )
{
  if (not ic.vex())     return newPCmpStr<SSE>( opbase, rm, gn, im, implicit, index );
  if (ic.vreg()) return 0;
  if (ic.vlen() == 128) return newPCmpStr<XMM>( opbase, rm, gn, im, implicit, index );
  return 0;
}
template <class VR>
Operation<ARCH>* newPCmpStr( OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn, uint8_t im, bool implicit, bool index )
{
  switch (im&3)
    {
    case 0b00: return new PCmpStr<ARCH,VR,typename ARCH:: u8_t>( opbase, rm, gn, im, implicit, index );
    case 0b01: return new PCmpStr<ARCH,VR,typename ARCH::u16_t>( opbase, rm, gn, im, implicit, index );
    case 0b10: return new PCmpStr<ARCH,VR,typename ARCH:: s8_t>( opbase, rm, gn, im, implicit, index );
    case 0b11: return new PCmpStr<ARCH,VR,typename ARCH::s16_t>( opbase, rm, gn, im, implicit, index );
    }
  return 0;
}
};

struct PCmpEQ { static char const* name() { return "pcmpeq"; } };
struct PCmpGT { static char const* name() { return "pcmpgt"; } };

template <class ARCH, class VR, class TYPE, class OPERATION>
struct PCmpVW : public Operation<ARCH>
{
  typedef TYPE valtype;
  enum { bitsize = atpinfo<ARCH,TYPE>::bitsize };
  valtype eval( PCmpEQ const&, valtype const& a, valtype const& b ) const { return valtype(a != b) - valtype(1); }
  valtype eval( PCmpGT const&, valtype const& a, valtype const& b ) const { return valtype(a <= b) - valtype(1); }
  
  PCmpVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn ) {}
  
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << OPERATION::name() << SizeID<bitsize>::iid()
         <<                ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    for (unsigned sub = 0; sub < (VR::size()/bitsize); ++sub)
      arch.vmm_write( VR(), gn, sub, eval( OPERATION(), arch.vmm_read( VR(), vn, sub, valtype() ), arch.vmm_read( VR(), rm, sub, valtype() ) ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* PCMPGTB/PCMPGTW/PCMPGTD/PCMPGTQ -- Compare Packed Signed Integers for Greater Than */
/* PCMPEQB/PCMPEQW/PCMPEQD/PCMPEQQ -- Compare Packed Data for Equal */

template <class ARCH> struct DC<ARCH,PCMPEQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "\x66\x0f\x74" ) & RM() ))
    
    return newPCmp<PCmpEQ,  typename ARCH::u8_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x75" ) & RM() ))
    
    return newPCmp<PCmpEQ, typename ARCH::u16_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x76" ) & RM() ))
    
    return newPCmp<PCmpEQ, typename ARCH::u32_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x38\x29" ) & RM() ))
    
    return newPCmp<PCmpEQ, typename ARCH::u64_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x64" ) & RM() ))
    
    return newPCmp<PCmpGT,  typename ARCH::s8_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x65" ) & RM() ))
    
    return newPCmp<PCmpGT, typename ARCH::s16_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x66" ) & RM() ))
    
    return newPCmp<PCmpGT, typename ARCH::s32_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x38\x37" ) & RM() ))
    
    return newPCmp<PCmpGT, typename ARCH::s64_t>( ic, _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
template <class OPERATION, class TYPE>
Operation<ARCH>* newPCmp( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn )
{
  if (not ic.vex())     return new PCmpVW<ARCH,SSE,TYPE,OPERATION>( opbase, rm, gn, gn );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new PCmpVW<ARCH,XMM,TYPE,OPERATION>( opbase, rm, vn, gn );
  if (ic.vlen() == 256) return new PCmpVW<ARCH,YMM,TYPE,OPERATION>( opbase, rm, vn, gn );
  return 0;
}
};

// op pcmpeqb_pq_qq( 0x0f[8]:> <:0x74[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqb_pq_qq.disasm = { _sink << "pcmpeqb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pcmpeqw_pq_qq( 0x0f[8]:> <:0x75[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqw_pq_qq.disasm = { _sink << "pcmpeqw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pcmpeqd_pq_qq( 0x0f[8]:> <:0x76[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqd_pq_qq.disasm = { _sink << "pcmpeqd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pcmpgtb_pq_qq( 0x0f[8]:> <:0x64[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtb_pq_qq.disasm = { _sink << "pcmpgtb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pcmpgtw_pq_qq( 0x0f[8]:> <:0x65[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtw_pq_qq.disasm = { _sink << "pcmpgtw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pcmpgtd_pq_qq( 0x0f[8]:> <:0x66[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtd_pq_qq.disasm = { _sink << "pcmpgtd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };

// /* PEXTRB/PEXTRW/PEXTRD/PEXTRQ -- Extract Byte/Dword/Qword */
// op pextrw_rd_pq_ib( 0x0f[8]:> <:0xc5[8]:> <:0b11[2]:gn[3]:rm[3]:> <:imm[8] );
// 
// pextrw_rd_pq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmPq( rm ) << ',' << DisasmGd( gn ); };
// 
// op pextrw_rd_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc5[8]:> <:0b11[2]:gn[3]:rm[3]:> <:imm[8] );
// 
// pextrw_rd_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmV( SSE(), rm ) << ',' << DisasmGd( gn ); };
// 
// op pextrb_eb_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x14[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrb_eb_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEb( rm ); };
// 
// op pextrw_ew_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x15[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrw_ew_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEw( rm ); };
// 
// op pextrd_ed_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x16[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrd_ed_vdq_ib.disasm = { _sink << "pextrd " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEd( rm ); };
// 
// // op pextrq_eb_vdq_ib( 0x66[8]:> <:0x48[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x16[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// // pextrq_eb_vdq_ib.disasm = { _sink << "pextrq " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEq( rm ); };
// 
// /* PHADDW/PHADDD -- Packed Horizontal Add */
// op phaddw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x01[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddw_pq_qq.disasm = { _sink << "phaddw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op phaddd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x02[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddd_pq_qq.disasm = { _sink << "phaddd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op phaddw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x01[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddw_vdq_wdq.disasm = { _sink << "phaddw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op phaddd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x02[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddd_vdq_wdq.disasm = { _sink << "phaddd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PHADDSW -- Packed Horizontal Add and Saturate */
// op phaddsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddsw_pq_qq.disasm = { _sink << "phaddsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op phaddsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddsw_vdq_wdq.disasm = { _sink << "phaddsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PHMINPOSUW -- Packed Horizontal Word Minimum */
// op phminposuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x41[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phminposuw_vdq_wdq.disasm = { _sink << "phminposuw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// 
// /* PHSUBW/PHSUBD -- Packed Horizontal Subtract */
// op phsubw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x05[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubw_pq_qq.disasm = { _sink << "phsubw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op phsubd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x06[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubd_pq_qq.disasm = { _sink << "phsubd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op phsubw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x05[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubw_vdq_wdq.disasm = { _sink << "phsubw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op phsubd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x06[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubd_vdq_wdq.disasm = { _sink << "phsubd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PHSUBSW -- Packed Horizontal Subtract and Saturate */
// op phsubsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x07[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubsw_pq_qq.disasm = { _sink << "phsubsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op phsubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x07[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubsw_vdq_wdq.disasm = { _sink << "phsubsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMADDUBSW --  */
// op pmaddubsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x04[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddubsw_pq_qq.disasm = { _sink << "pmaddubsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmaddubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x04[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddubsw_vdq_wdq.disasm = { _sink << "pmaddubsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMADDWD -- Multiply and Add Packed Integers */
// op pmaddwd_pq_qq( 0x0f[8]:> <:0xf5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddwd_pq_qq.disasm = { _sink << "pmaddwd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmaddwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddwd_vdq_wdq.disasm = { _sink << "pmaddwd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 

template <class ARCH, class VR, class GR>
struct PInsr : public Operation<ARCH>
{
  PInsr( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn, uint8_t _im ) : Operation<ARCH>( opbase ), rm(_rm), vn(_vn), gn(_gn), im(_im) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "pinsr" << SizeID<GR::SIZE>::iid()
         <<                ' ' << DisasmI( im )
         <<                ',' << DisasmE( GR(), rm ); // TODO: for register, stays disassembled as doubleword below doubleword
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,GR::SIZE>::u u_type;
    for (unsigned idx = 0, end = VR::size() / GR::SIZE; idx < end; ++idx )
      {
        if (((idx ^ im) % end) == 0)
          arch.vmm_write( VR(), gn, idx, arch.rmread( GR(), rm ) );
        else if (gn != vn)
          arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), vn, idx, u_type() ) );
      }
  }
  RMOp<ARCH> rm; uint8_t vn, gn, im;
};

// /* PINSRB/PINSRW/PINSRD/PINSRQ -- Insert Byte/Word/Dword/Qword */
// op pinsrw_pq_ed_ib( 0x0f[8]:> <:0xc4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// op pinsrb_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x20[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// op pinsrw_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0xc4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// op pinsrd_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x22[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );

template <class ARCH> struct DC<ARCH,PINSR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\xc4" ) & RM() & Imm<8>() ))

    return newPInsr<GOw>( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );

  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x20" ) & RM() & Imm<8>() ))

    return newPInsr<GOb>( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );

  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x22" ) & RM() & Imm<8>() ))
    {
      if (ic.w()) return newPInsr<GOq>( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );
      /* else */  return newPInsr<GOd>( ic, _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );
    }

  return 0;
}
template <class GR>
Operation<ARCH>* newPInsr( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn, uint8_t im )
{
  if (not ic.vex())     return new PInsr<ARCH,SSE,GR>( opbase, rm, gn, gn, im );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new PInsr<ARCH,XMM,GR>( opbase, rm, vn, gn, im );
  return 0;
}
};

struct PMax { static char const* name() { return "pmax"; } }; struct PMin { static char const* name() { return "pmin"; } };

template <class ARCH, class VR, class TYPE, class OPERATION>
struct PMinMax : public Operation<ARCH>
{
  typedef TYPE valtype;
  enum { is_signed = atpinfo<ARCH,TYPE>::is_signed, bitsize = atpinfo<ARCH,TYPE>::bitsize };
  valtype eval( PMax const&, valtype const& a, valtype const& b ) const { return Maximum(a, b); }
  valtype eval( PMin const&, valtype const& a, valtype const& b ) const { return Minimum(a, b); }
  
  PMinMax( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn ) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << OPERATION::name()
         << (is_signed ? "s" : "u") << SizeID<bitsize>::iid()
         <<                ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size()/bitsize; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, eval( OPERATION(), arch.vmm_read( VR(), vn, idx, valtype() ), arch.vmm_read( VR(), rm, idx, valtype() ) ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

// op pmaxsw_pq_qq( 0x0f[8]:> <:0xee[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// op pmaxub_pq_qq( 0x0f[8]:> <:0xde[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// op pminsw_pq_qq( 0x0f[8]:> <:0xea[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// op pminub_pq_qq( 0x0f[8]:> <:0xda[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );

// /* P{MAX|MIN}[US][BWD],  -- Maximum/Minimum of Packed Unsigned/Signed Byte/Word/DWord Integers */

template <class ARCH> struct DC<ARCH,PMINMAX> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x3c" ) & RM() ))

    return newMinMax<typename ARCH::s8_t,PMax>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\xee" ) & RM() ))
  
    return newMinMax<typename ARCH::s16_t,PMax>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x3d" ) & RM() ))

    return newMinMax<typename ARCH::s32_t,PMax>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\xde" ) & RM() ))

    return newMinMax<typename ARCH::u8_t,PMax>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x3e" ) & RM() ))

    return newMinMax<typename ARCH::u16_t,PMax>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x38\x3f" ) & RM() ))

    return newMinMax<typename ARCH::u32_t,PMax>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x38\x38" ) & RM() ))

    return newMinMax<typename ARCH::s8_t,PMin>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\xea" ) & RM() ))
  
    return newMinMax<typename ARCH::s16_t,PMin>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x39" ) & RM() ))

    return newMinMax<typename ARCH::s32_t,PMin>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\xda" ) & RM() ))

    return newMinMax<typename ARCH::u8_t,PMin>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x3a" ) & RM() ))

    return newMinMax<typename ARCH::u16_t,PMin>( ic, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x38\x3b" ) & RM() ))

    return newMinMax<typename ARCH::u32_t,PMin>( ic, _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
template <class TYPE, class OPERATION>
Operation<ARCH>* newMinMax( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, uint8_t gn )
{
  if (not ic.vex())     return new PMinMax<ARCH,SSE,TYPE,OPERATION>( opbase, rm, gn, gn );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new PMinMax<ARCH,XMM,TYPE,OPERATION>( opbase, rm, vn, gn );
  if (ic.vlen() == 256) return new PMinMax<ARCH,YMM,TYPE,OPERATION>( opbase, rm, vn, gn );
  return 0;
}
};

// /* PMOVMSKB -- Move Byte Mask */
template <class ARCH>
struct PMovMskBRP : public Operation<ARCH>
{
  PMovMskBRP( OpBase<ARCH> const& opbase, uint8_t _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} uint8_t rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pmovmskb " << DisasmPq( rm ) << ',' << DisasmG( GOd(), gn ); }
};
 
template <class ARCH, class VR>
struct PMovMskBRV : public Operation<ARCH>
{
  PMovMskBRV( OpBase<ARCH> const& opbase, uint8_t _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} uint8_t rm; uint8_t gn;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "pmovmskb " << DisasmV( VR(), rm ) << ',' << DisasmG( GOd(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    typedef typename ARCH::u8_t  u8_t;
    
    u32_t res = u32_t(0);
    
    for (unsigned sub = 0, end = VR::size() / 8; sub < end; ++sub)
      res |= u32_t(arch.vmm_read( VR(), rm, sub, u8_t() ) >> 7) << sub;
    
    arch.regwrite( GOd(), gn, res );
  }
};

template <class ARCH> struct DC<ARCH,PMOVMSKB> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xd7" ) & RM_reg() ))
    
    return new PMovMskBRP<ARCH>( _.opbase(), _.ereg(), _.greg() );
  
  if (auto _ = match( ic, vex("\x66\x0f\xd7" ) & RM_reg() ))
    
    return newPMovMsk( ic, _.opbase(), _.ereg(), _.greg() );
  
  return 0;
}
Operation<ARCH>* newPMovMsk( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, uint8_t rn, uint8_t gn )
{
  if (not ic.vex())     return new PMovMskBRV<ARCH,SSE>( opbase, rn, gn );
  if (ic.vreg()) return 0;
  if (ic.vlen() == 128) return new PMovMskBRV<ARCH,XMM>( opbase, rn, gn );
  if (ic.vlen() == 256) return new PMovMskBRV<ARCH,YMM>( opbase, rn, gn );
  return 0;
}
};

// 
// /* PMOVSX -- Packed Move with Sign Extend */
// op pmovsxbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x20[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbw_vdq_wdq.disasm = { _sink << "pmovsxbw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxbd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x21[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbd_vdq_wdq.disasm = { _sink << "pmovsxbd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxbq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x22[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbq_vdq_wdq.disasm = { _sink << "pmovsxbq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x23[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxwd_vdq_wdq.disasm = { _sink << "pmovsxwd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxwq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x24[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxwq_vdq_wdq.disasm = { _sink << "pmovsxwq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxdq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x25[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxdq_vdq_wdq.disasm = { _sink << "pmovsxdq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMOVZX -- Packed Move with Sign Extend */
// op pmovzxbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x30[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbw_vdq_wdq.disasm = { _sink << "pmovzxbw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxbd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x31[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbd_vdq_wdq.disasm = { _sink << "pmovzxbd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxbq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x32[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbq_vdq_wdq.disasm = { _sink << "pmovzxbq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x33[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxwd_vdq_wdq.disasm = { _sink << "pmovzxwd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxwq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x34[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxwq_vdq_wdq.disasm = { _sink << "pmovzxwq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxdq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x35[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxdq_vdq_wdq.disasm = { _sink << "pmovzxdq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULDQ -- Multiply Packed Signed Dword Integers */
// op pmuldq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuldq_vdq_wdq.disasm = { _sink << "pmuldq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULHRSW -- Packed Multiply High with Round and Scale */
// op pmulhrsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhrsw_pq_qq.disasm = { _sink << "pmulhrsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmulhrsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhrsw_vdq_wdq.disasm = { _sink << "pmulhrsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULHUW -- Multiply Packed Unsigned Integers and Store High Result */
// op pmulhuw_pq_qq( 0x0f[8]:> <:0xe4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhuw_pq_qq.disasm = { _sink << "pmulhuw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmulhuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhuw_vdq_wdq.disasm = { _sink << "pmulhuw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULHW -- Multiply Packed Signed Integers and Store High Result */
// op pmulhw_pq_qq( 0x0f[8]:> <:0xe5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhw_pq_qq.disasm = { _sink << "pmulhw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmulhw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhw_vdq_wdq.disasm = { _sink << "pmulhw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULLD -- Multiply Packed Signed Dword Integers and Store Low Result */
// op pmulld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x40[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulld_vdq_wdq.disasm = { _sink << "pmulld " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULLW -- Multiply Packed Signed Integers and Store Low Result */
// op pmullw_pq_qq( 0x0f[8]:> <:0xd5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmullw_pq_qq.disasm = { _sink << "pmullw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmullw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmullw_vdq_wdq.disasm = { _sink << "pmullw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULUDQ -- Multiply Packed Unsigned Doubleword Integers */
// op pmuludq_pq_qq( 0x0f[8]:> <:0xf4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuludq_pq_qq.disasm = { _sink << "pmuludq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pmuludq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuludq_vdq_wdq.disasm = { _sink << "pmuludq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSADBW -- Compute Sum of Absolute Differences */
// op psadbw_pq_qq( 0x0f[8]:> <:0xf6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psadbw_pq_qq.disasm = { _sink << "psadbw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psadbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psadbw_vdq_wdq.disasm = { _sink << "psadbw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 

template <class ARCH, class VR>
struct Pshufd : public Operation<ARCH>
{
  Pshufd( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, uint8_t _oo ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn), oo(_oo) {} RMOp<ARCH> rm; uint8_t gn, oo;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "pshufd " << DisasmI(oo) << ',' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;


    for (unsigned chunk = 0, cend = VR::size() / 128; chunk < cend; ++ chunk)
      {
        for (unsigned idx = 0, end = 128 / 32; idx < end; ++idx)
          {
            unsigned part = (oo >> 2*idx) % 4;
            arch.vmm_write( VR(), gn, idx + chunk*end, arch.vmm_read( VR(), rm, part + chunk*end, u32_t() ) );
          }
      }
  }
};

/* PSHUFD -- Shuffle Packed Doublewords */
template <class ARCH> struct DC<ARCH,PSHUFD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x70" ) & RM() & Imm<8>() ))
    {
      if (not ic.vex())     return new Pshufd<ARCH,SSE>( _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );
      if (ic.vreg()) return 0;
      if (ic.vlen() == 128) return new Pshufd<ARCH,XMM>( _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );
      if (ic.vlen() == 256) return new Pshufd<ARCH,YMM>( _.opbase(), _.rmop(), _.greg(), _.i(uint8_t()) );
    }
    
  return 0;
}};

template <class ARCH, class VR>
struct Pshufb : public Operation<ARCH>
{
  Pshufb( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), vn(_vn), gn(_gn) {} RMOp<ARCH> rm; uint8_t vn; uint8_t gn;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "pshufd " << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), vn ) << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u8_t u8_t;
    typedef typename ARCH::s8_t s8_t;
    
    unsigned const size = 128 / 8;
    
    for (unsigned chunk = 0, cend = VR::size(); chunk < cend; chunk += size)
      {
        u8_t res[size];
        
        for (unsigned idx = 0; idx < size; ++idx)
          {
            u8_t sidx = arch.vmm_read( VR(), rm, chunk + idx, u8_t() );
            u8_t mask = ~u8_t(s8_t(sidx) >> 7);
            sidx = (sidx & u8_t(0xf)) + u8_t(chunk);
            res[idx] = arch.vmm_read( VR(), vn, sidx, u8_t() ) & mask;
          }
        
        for (unsigned idx = 0; idx < size; ++idx)
          arch.vmm_write( VR(), gn, idx+chunk, res[idx] );
      }
  }
};

/* PSHUFB -- Packed Shuffle Bytes */
template <class ARCH> struct DC<ARCH,PSHUFB> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x00" ) & RM() ))
    {
      unsigned gn = _.greg();
      if (not ic.vex())     return new Pshufb<ARCH,SSE>( _.opbase(), _.rmop(), gn, gn );
      unsigned vn = ic.vreg();
      if (ic.vlen() == 128) return new Pshufb<ARCH,XMM>( _.opbase(), _.rmop(), vn, gn );
      if (ic.vlen() == 256) return new Pshufb<ARCH,YMM>( _.opbase(), _.rmop(), vn, gn );
    }
    
  return 0;
}};

template <class ARCH, class VR>
struct Pshdq : public Operation<ARCH>
{
  Pshdq( OpBase<ARCH> const& opbase, uint8_t _rn, uint8_t _vn, uint8_t _im, bool _sr ) : Operation<ARCH>(opbase), rn(_rn), vn(_vn), im(_im), sr(_sr) {} uint8_t rn, vn, im; bool sr;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << (sr ? "psrldq" : "pslldq")
         <<                ' ' << DisasmI( im )
         <<                ',' << DisasmV( VR(), rn );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u8_t u8_t;
    
    unsigned const size = 128 / 8;
    for (unsigned chunk = 0, cend = VR::size() / size; chunk < cend; ++chunk )
      {
        u8_t res[size];
        for (unsigned idx = 0; idx < size; ++idx)
          {
            unsigned sidx = idx + (sr ? -im : im);
            res [idx] = sidx < size ? arch.vmm_read( VR(), rn, sidx + chunk*size, u8_t() ) : u8_t(0);
          }
        for (unsigned idx = 0; idx < size; ++idx)
          arch.vmm_write( VR(), vn, idx + chunk*size, res[idx] );
      }
  }
};

/* PSLLDQ/PSRLDQ -- Shift Double Quadword Left/Right Logical */
template <class ARCH> struct DC<ARCH,PSHDQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x73" ) /7 & RM_reg() & Imm<8>() ))

    return newPshdq( ic, false, _.opbase(), _.ereg(), _.i(uint8_t()) );

  if (auto _ = match( ic, vex( "\x66\x0f\x73" ) /3 & RM_reg() & Imm<8>() ))

    return newPshdq( ic, true, _.opbase(), _.ereg(), _.i(uint8_t()) );
    
  return 0;
}
Operation<ARCH>* newPshdq( InputCode<ARCH> const& ic, bool right, OpBase<ARCH> const& opbase, uint8_t rn, uint8_t im )
{
  if (not ic.vex())     return new Pshdq<ARCH,SSE>( opbase, rn, rn, im, right );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new Pshdq<ARCH,XMM>( opbase, rn, vn, im, right );
  if (ic.vlen() == 256) return new Pshdq<ARCH,YMM>( opbase, rn, vn, im, right );
  return 0;
}
};

// /* PSHUFHW -- Shuffle Packed High Words */
// op pshufhw_vdq_wdq_ib( 0xf3[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshufhw_vdq_wdq_ib.disasm = { _sink << "pshufhw " << DisasmI(imm) << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSHUFLW -- Shuffle Packed Low Words */
// op pshuflw_vdq_wdq_ib( 0xf2[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshuflw_vdq_wdq_ib.disasm = { _sink << "pshuflw " << DisasmI(imm) << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSHUFW -- Shuffle Packed Words */
// op pshufw_pq_qq( 0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pshufw_pq_qq.disasm = { _sink << "pshufw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// /* PSIGNB/PSIGNW/PSIGND -- Packed SIGN */
// op psignb_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignb_pq_qq.disasm = { _sink << "psignb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psignw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignw_pq_qq.disasm = { _sink << "psignw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psignd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignd_pq_qq.disasm = { _sink << "psignd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psignb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignb_vdq_wdq.disasm = { _sink << "psignb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psignw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignw_vdq_wdq.disasm = { _sink << "psignw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psignd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignd_vdq_wdq.disasm = { _sink << "psignd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSLLW/PSLLD/PSLLQ -- Shift Packed Data Left Logical */
// op psllw_pq_qq( 0x0f[8]:> <:0xf1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllw_pq_qq.disasm = { _sink << "psllw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psllw_pq_ib( 0x0f[8]:> <:0x71[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllw_pq_ib.disasm = { _sink << "psllw " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op pslld_pq_qq( 0x0f[8]:> <:0xf2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pslld_pq_qq.disasm = { _sink << "pslld " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op pslld_pq_ib( 0x0f[8]:> <:0x72[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// pslld_pq_ib.disasm = { _sink << "pslld " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psllq_pq_qq( 0x0f[8]:> <:0xf3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllq_pq_qq.disasm = { _sink << "psllq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psllq_pq_ib( 0x0f[8]:> <:0x73[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllq_pq_ib.disasm = { _sink << "psllq " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psllw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllw_vdq_wdq.disasm = { _sink << "psllw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psllw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllw_vdq_ib.disasm = { _sink << "psllw " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op pslld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pslld_vdq_wdq.disasm = { _sink << "pslld " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pslld_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// pslld_vdq_ib.disasm = { _sink << "pslld " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psllq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllq_vdq_wdq.disasm = { _sink << "psllq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psllq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllq_vdq_ib.disasm = { _sink << "psllq " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSRAW/PSRAD -- Shift Packed Data Right Arithmetic */
// op psraw_pq_qq( 0x0f[8]:> <:0xe1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psraw_pq_qq.disasm = { _sink << "psraw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psraw_pq_ib( 0x0f[8]:> <:0x71[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psraw_pq_ib.disasm = { _sink << "psraw " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrad_pq_qq( 0x0f[8]:> <:0xe2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrad_pq_qq.disasm = { _sink << "psrad " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psrad_pq_ib( 0x0f[8]:> <:0x72[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psrad_pq_ib.disasm = { _sink << "psrad " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psraw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psraw_vdq_wdq.disasm = { _sink << "psraw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psraw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psraw_vdq_ib.disasm = { _sink << "psraw " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psrad_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrad_vdq_wdq.disasm = { _sink << "psrad " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrad_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psrad_vdq_ib.disasm = { _sink << "psrad " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSRLW/PSRLD/PSRLQ -- Shift Packed Data Right Logical */
// op psrlw_pq_qq( 0x0f[8]:> <:0xd1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlw_pq_qq.disasm = { _sink << "psrlw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psrlw_pq_ib( 0x0f[8]:> <:0x71[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlw_pq_ib.disasm = { _sink << "psrlw " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrld_pq_qq( 0x0f[8]:> <:0xd2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrld_pq_qq.disasm = { _sink << "psrld " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psrld_pq_ib( 0x0f[8]:> <:0x72[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrld_pq_ib.disasm = { _sink << "psrld " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrlq_pq_qq( 0x0f[8]:> <:0xd3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlq_pq_qq.disasm = { _sink << "psrlq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psrlq_pq_ib( 0x0f[8]:> <:0x73[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlq_pq_ib.disasm = { _sink << "psrlq " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrlw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlw_vdq_wdq.disasm = { _sink << "psrlw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrlw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlw_vdq_ib.disasm = { _sink << "psrlw " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psrld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrld_vdq_wdq.disasm = { _sink << "psrld " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrld_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrld_vdq_ib.disasm = { _sink << "psrld " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psrlq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlq_vdq_wdq.disasm = { _sink << "psrlq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrlq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlq_vdq_ib.disasm = { _sink << "psrlq " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSUBB/PSUBW/PSUBD/PSUBQ -- Subtract Packed Integers */
// op psubb_pq_qq( 0x0f[8]:> <:0xf8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubb_pq_qq.disasm = { _sink << "psubb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubw_pq_qq( 0x0f[8]:> <:0xf9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubw_pq_qq.disasm = { _sink << "psubw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubd_pq_qq( 0x0f[8]:> <:0xfa[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubd_pq_qq.disasm = { _sink << "psubd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubq_pq_qq( 0x0f[8]:> <:0xfb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubq_pq_qq.disasm = { _sink << "psubq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubb_vdq_wdq.disasm = { _sink << "psubb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubw_vdq_wdq.disasm = { _sink << "psubw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfa[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubd_vdq_wdq.disasm = { _sink << "psubd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubq_vdq_wdq.disasm = { _sink << "psubq " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSUBSB/PSUBSW -- Subtract Packed Signed Integers with Signed Saturation */
// op psubsb_pq_qq( 0x0f[8]:> <:0xe8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsb_pq_qq.disasm = { _sink << "psubsb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubsw_pq_qq( 0x0f[8]:> <:0xe9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsw_pq_qq.disasm = { _sink << "psubsw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsb_vdq_wdq.disasm = { _sink << "psubsb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsw_vdq_wdq.disasm = { _sink << "psubsw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSUBUSB/PSUBUSW -- Subtract Packed Unsigned Integers with Unsigned Saturation */
// op psubusb_pq_qq( 0x0f[8]:> <:0xd8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusb_pq_qq.disasm = { _sink << "psubusb " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubusw_pq_qq( 0x0f[8]:> <:0xd9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusw_pq_qq.disasm = { _sink << "psubusw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// 
// op psubusb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusb_vdq_wdq.disasm = { _sink << "psubusb " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubusw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusw_vdq_wdq.disasm = { _sink << "psubusw " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
//

template <class ARCH, class VR>
struct PTest : public Operation<ARCH>
{
  PTest( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {}
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "ptest " << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {  
    typedef typename ARCH::u64_t u64_t;
    typedef typename ARCH::bit_t bit_t;
    bit_t zf(true), cf( true );
    for (unsigned idx = 0, end = VR::size()/64; idx < end; ++idx)
      {
        u64_t lhs = arch.vmm_read( VR(), rm, idx, u64_t() );
        u64_t rhs = arch.vmm_read( VR(), gn, idx, u64_t() );
        zf &= (lhs &  rhs) == u64_t(0);
        cf &= (lhs & ~rhs) == u64_t(0);
      }
    arch.flagwrite( ARCH::FLAG::ZF, zf );
    arch.flagwrite( ARCH::FLAG::CF, cf );
    arch.flagwrite( ARCH::FLAG::PF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::OF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::SF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::AF, bit_t(false) );
  }
  RMOp<ARCH> rm; uint8_t gn;
};

template <class ARCH> struct DC<ARCH,PTEST> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x38\x17" ) & RM() ))
    {
      if (not ic.vex())     return new PTest<ARCH,SSE>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vreg()) return 0;
      if (ic.vlen() == 128) return new PTest<ARCH,XMM>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new PTest<ARCH,YMM>( _.opbase(), _.rmop(), _.greg() );
    }

  return 0;
}};
// op punpckhbw_pq_qq( 0x0f[8]:> <:0x68[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckhbw_pq_qq.disasm = { _sink << "punpckhbw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// op punpckhwd_pq_qq( 0x0f[8]:> <:0x69[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckhwd_pq_qq.disasm = { _sink << "punpckhwd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// op punpckhdq_pq_qq( 0x0f[8]:> <:0x6a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckhdq_pq_qq.disasm = { _sink << "punpckhdq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// op punpcklbw_pq_qq( 0x0f[8]:> <:0x60[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpcklbw_pq_qq.disasm = { _sink << "punpcklbw " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// op punpcklwd_pq_qq( 0x0f[8]:> <:0x61[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpcklwd_pq_qq.disasm = { _sink << "punpcklwd " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };
// op punpckldq_pq_qq( 0x0f[8]:> <:0x62[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckldq_pq_qq.disasm = { _sink << "punpckldq " << DisasmQq( rm ) << ',' << DisasmPq( gn ); };

template <class ARCH, class VR, class TYPE, bool HI>
struct Unpack : public Operation<ARCH>
{
  typedef TYPE val_type;
  typedef atpinfo<ARCH,TYPE> CFG;
  Unpack( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t vn, gn;
  template <typename T, typename std::enable_if<T::is_integral == 1, int>::type = 0>
  std::ostream& mnemonic( T const&, std::ostream& sink ) const { sink << 'p' << "unpck" << "lh"[HI] << SizeID<T::bitsize>::iid() << SizeID<2*T::bitsize>::iid(); return sink; }
  template <typename T, typename std::enable_if<T::is_integral == 0, int>::type = 0>
  std::ostream& mnemonic( T const&, std::ostream& sink ) const { sink << "unpck" << "lh"[HI] << 'p' << SizeID<T::bitsize>::fid(); return sink; }
  void disasm( std::ostream& sink ) const { this->mnemonic(CFG(), sink << (VR::vex() ? "v" : "") ) << ' ' << DisasmW(VR(),rm) << ',' << DisasmV(VR(),gn); }
  void execute( ARCH& arch ) const
  {
    unsigned const COUNT = 128/CFG::bitsize/2;
    unsigned const HALF = HI ? COUNT : 0;

    for (unsigned chunk = 0, cend = VR::size()/128; chunk < cend; ++chunk)
      {
        val_type res[2*COUNT];
        for (unsigned idx = 0; idx < COUNT; ++idx)
          {
            unsigned sidx = idx + HALF + chunk*2*COUNT, didx = 2*idx;
            res[didx+0] = arch.vmm_read( VR(), vn, sidx, val_type() );
            res[didx+1] = arch.vmm_read( VR(), rm, sidx, val_type() );
          }
        for (unsigned idx = 0; idx < 2*COUNT; ++idx)
          arch.vmm_write( VR(), gn, idx, res[idx] );
      }
  }
};

/* PUNPCKHBW/PUNPCKHWD/PUNPCKHDQ/PUNPCKHQDQ/PUNPCKLBW/PUNPCKLWD/PUNPCKLDQ/PUNPCKLQDQ/UNPCKHPS/UNPCKHPD/UNPCKLPS/UNPCKLPD -- Unpack and Interleave Data */
template <class ARCH> struct DC<ARCH,PUNPCK> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x60" ) & RM() ))

    return newUnpack<typename ARCH::u8_t,false>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x61" ) & RM() ))

    return newUnpack<typename ARCH::u16_t,false>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x62" ) & RM() ))

    return newUnpack<typename ARCH::u32_t,false>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x6c" ) & RM() ))

    return newUnpack<typename ARCH::u64_t,false>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x68" ) & RM() ))

    return newUnpack<typename ARCH::u8_t,true>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x69" ) & RM() ))

    return newUnpack<typename ARCH::u16_t,true>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x6a" ) & RM() ))

    return newUnpack<typename ARCH::u32_t,true>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x6d" ) & RM() ))

    return newUnpack<typename ARCH::u64_t,true>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x0f\x14" ) & RM() ))

    return newUnpack<typename ARCH::f32_t,false>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x14" ) & RM() ))

    return newUnpack<typename ARCH::f64_t,false>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x0f\x15" ) & RM() ))

    return newUnpack<typename ARCH::f32_t,true>( ic, _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x15" ) & RM() ))

    return newUnpack<typename ARCH::f64_t,true>( ic, _.opbase(), _.rmop(), _.greg() );

  return 0;
}
template <class TYPE,bool LOHI>
Operation<ARCH>* newUnpack( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn )
{
  if (not ic.vex())     return new Unpack<ARCH,SSE,TYPE,LOHI>( opbase, _rm, _gn, _gn );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new Unpack<ARCH,XMM,TYPE,LOHI>( opbase, _rm,  vn, _gn );
  if (ic.vlen() == 256) return new Unpack<ARCH,YMM,TYPE,LOHI>( opbase, _rm,  vn, _gn );
  return 0;
}
};

template <class ARCH, class VR, unsigned OPSIZE>
struct Palignr : public Operation<ARCH>
{
  Palignr( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _vn, uint8_t _gn, uint8_t _im ) : Operation<ARCH>(opbase), rm(_rm), vn(_vn), gn(_gn), im(_im) {}
  RMOp<ARCH> rm; uint8_t vn, gn, im;
  void disasm( std::ostream& sink ) const
  {
    sink << (VR::vex() ? "v" : "") << "palignr"
         <<                ' ' << DisasmI(im)
         <<                ',' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink <<                ',' << DisasmV( VR(), gn );
  }
  
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u uint_t;
    unsigned const size =  128 / OPSIZE;
    
    for (unsigned step = 0, end = VR::size() / 128; step < end; ++step)
      {
        uint_t res[size];
    
        {
          unsigned sidx, ridx = 0;
          // SRC2 part
          sidx = im*8/OPSIZE;
          for (; sidx < size; ++sidx, ++ridx)
            res[ridx] = arch.vmm_read( VR(), rm, sidx+step*size, uint_t() );
          // SRC1 part
          sidx -= size;
          for (unsigned end = size - (ridx > sidx ? ridx - sidx : 0); sidx < end; ++sidx, ++ridx)
            res[ridx] = arch.vmm_read( VR(), vn, sidx+step*size, uint_t() );
          // Zero part
          for (; ridx < size; ++ridx)
            res[ridx] = uint_t(0u);
        }
    
        for (unsigned idx = 0; idx < size; ++idx)
          arch.vmm_write( VR(), gn, idx+step*size, res[idx] );
      }
  }
};

// op palignr_pq_qq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// op palignr_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );

/* PALIGNR -- Packed Align Right */
template <class ARCH> struct DC<ARCH,PALIGNR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\x66\x0f\x3a\x0f" ) & RM() & Imm<8>() ))
    {
      uint8_t imm = _.i(int8_t());
      switch ((imm^(imm-1)) & 15)
        {
        case 1:  return newPalignr <8>( ic, _.opbase(), _.rmop(), _.greg(), imm );
        case 3:  return newPalignr<16>( ic, _.opbase(), _.rmop(), _.greg(), imm );
        case 7:  return newPalignr<32>( ic, _.opbase(), _.rmop(), _.greg(), imm );
        case 15: return newPalignr<64>( ic, _.opbase(), _.rmop(), _.greg(), imm );
        }
    }

  return 0;
}
template <unsigned OPSIZE>
Operation<ARCH>* newPalignr( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, uint8_t _im )
{
  if (not ic.vex())     return new Palignr<ARCH,SSE,OPSIZE>( opbase, _rm, _gn, _gn, _im );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new Palignr<ARCH,XMM,OPSIZE>( opbase, _rm,  vn, _gn, _im );
  if (ic.vlen() == 256) return new Palignr<ARCH,YMM,OPSIZE>( opbase, _rm,  vn, _gn, _im );
  return 0;
}
};

struct VAND {}; struct VANDN {}; struct VOR {}; struct VXOR {};

/* TODO: this operation will always blow the vector typing, but is this avoidable ? */
template <class ARCH, class VR, class OPERATION, unsigned OPSIZE>
struct PBitManipVVW : public Operation<ARCH>
{
  typedef typename TypeFor<ARCH,OPSIZE>::u valtype;
  valtype eval (  VXOR const&, valtype const& src1, valtype const& src2 ) const { return src1 ^ src2; }
  valtype eval (  VAND const&, valtype const& src1, valtype const& src2 ) const { return src1 & src2; }
  valtype eval ( VANDN const&, valtype const& src1, valtype const& src2 ) const { return src1 &~ src2; }
  valtype eval (   VOR const&, valtype const& src1, valtype const& src2 ) const { return src1 | src2; }
  
  PBitManipVVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _vn, unsigned _gn, char const* _mn ) : Operation<ARCH>(opbase), rm(_rm), mn(_mn), vn(_vn), gn(_gn) {}
  RMOp<ARCH> rm; char const* mn; uint8_t vn, gn; 
  void disasm( std::ostream& sink ) const
  {
    sink << mn << ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, eval( OPERATION(), arch.vmm_read( VR(), vn, idx, valtype() ), arch.vmm_read( VR(), rm, idx, valtype() ) ) );
  }
};

template <class ARCH>
struct PAndnPQ : public Operation<ARCH>
{
  PAndnPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pandn " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct PAndPQ : public Operation<ARCH>
{
  PAndPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pand " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct POrPQ : public Operation<ARCH>
{
  POrPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "por " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct PXorPQ : public Operation<ARCH>
{
  PXorPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pxor " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

/**** Packed Bit Manipulations ****/
/* PAND -- Bitwise Logical AND */
/* ANDPD -- Bitwise Logical AND of Packed Double-Precision Floating-Point Values */
/* ANDPS -- Bitwise Logical AND of Packed Single-Precision Floating-Point Values */
/* PANDN -- Bitwise Logical AND NOT */
/* ANDNPD -- Bitwise Logical AND NOT of Packed Double-Precision Floating-Point Values */
/* ANDNPS -- Bitwise Logical AND NOT of Packed Single-Precision Floating-Point Values */
/* POR -- Bitwise Logical OR */
/* ORPD -- Bitwise Logical OR of Double-Precision Floating-Point Values */
/* ORPS -- Bitwise Logical OR of Single-Precision Floating-Point Values */
/* PXOR -- Logical Exclusive OR */
/* XORPD -- Bitwise Logical XOR for Double-Precision Floating-Point Values */
/* XORPS -- Bitwise Logical XOR for Single-Precision Floating-Point Values */
template <class ARCH> struct DC<ARCH,PBM> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  /* MMX */
  
  if (auto _ = match( ic, simd__() & opcode( "\x0f\xeb" ) & RM() ))

    return new POrPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xdb" ) & RM() ))

    return new PAndPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xdb" ) & RM() ))

    return new PAndnPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xef" ) & RM() ))

    return new PXorPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  /* SSE/AVX Integer */
  
  if (auto _ = match( ic, vex( "\x66\x0f\xdb" ) & RM() ))
    
    return newPBitManipVVW<VAND,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpand" : "pand" );

  if (auto _ = match( ic, vex( "\x66\x0f\xdf" ) & RM() ))

    return newPBitManipVVW<VANDN,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpandn" : "pandn" );

  if (auto _ = match( ic, vex( "\x66\x0f\xeb" ) & RM() ))

    return newPBitManipVVW<VOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpor" : "por" );

  if (auto _ = match( ic, vex( "\x66\x0f\xef" ) & RM() ))
    
    return newPBitManipVVW<VXOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpxor" : "pxor" );

  /* SSE/AVX Floating Point */
  
  if (auto _ = match( ic, vex( "\x0f\x54" ) & RM() ))

    return newPBitManipVVW<VAND,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandps" : "andps" );

  if (auto _ = match( ic, vex( "\x66\x0f\x54" ) & RM() ))

    return newPBitManipVVW<VAND,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandpd" : "andpd" );

  if (auto _ = match( ic, vex( "\x0f\x55" ) & RM() ))

    return newPBitManipVVW<VANDN,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandnps" : "andnps" );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x55" ) & RM() ))

    return newPBitManipVVW<VANDN,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandnpd" : "andnpd" );
  
  if (auto _ = match( ic, vex( "\x0f\x56" ) & RM() ))

    return newPBitManipVVW<VOR,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vorps" : "orps" );

  if (auto _ = match( ic, vex( "\x66\x0f\x56" ) & RM() ))

    return newPBitManipVVW<VOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vorpd" : "orpd" );

  if (auto _ = match( ic, vex( "\x0f\x57" ) & RM() ))

    return newPBitManipVVW<VXOR,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vxorps" : "xorps" );

  if (auto _ = match( ic, vex( "\x66\x0f\x57" ) & RM() ))

    return newPBitManipVVW<VXOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vxorpd" : "xorpd" );

  return 0;
}
template <class OPERATION, unsigned OPSIZE>
Operation<ARCH>* newPBitManipVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, char const* _mn )
{
  if (not ic.vex())     return new PBitManipVVW<ARCH,SSE,OPERATION,OPSIZE>( opbase, _rm, _gn, _gn, _mn );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new PBitManipVVW<ARCH,XMM,OPERATION,OPSIZE>( opbase, _rm,  vn, _gn, _mn );
  if (ic.vlen() == 256) return new PBitManipVVW<ARCH,YMM,OPERATION,OPSIZE>( opbase, _rm,  vn, _gn, _mn );
  return 0;
}
};

template <class ARCH, unsigned OPSZ>
struct Ucomis : public Operation<ARCH>
{
  Ucomis( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, bool _v ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn), v(_v) {} RMOp<ARCH> rm; uint8_t gn; bool v;
  void disasm( std::ostream& sink ) const { sink << (v?"v":"") << "ucomis" << SizeID<OPSZ>::fid() << ' ' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSZ>::f f_type;
    typedef typename ARCH::bit_t bit_t;
    
    f_type a = arch.vmm_read( SSE(), gn, 0, f_type() );
    f_type b = arch.vmm_read( SSE(), rm, 0, f_type() );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( ARCH::FLAG::ZF, notge == notle );
    arch.flagwrite( ARCH::FLAG::PF, notge and notle );
    arch.flagwrite( ARCH::FLAG::CF, notge );
    arch.flagwrite( ARCH::FLAG::OF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::SF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::AF, bit_t(false) );
  }
};

/* UCOMISD -- Unordered Compare Scalar Double-Precision Floating-Point Values and Set EFLAGS */
/* UCOMISS -- Unordered Compare Scalar Single-Precision Floating-Point Values and Set EFLAGS */
template <class ARCH> struct DC<ARCH,UCOMIS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x2e" ) & RM() ))
    {
      if (ic.vex() and ic.vreg()) return 0;
      if (match( ic, simd__() )) return new Ucomis<ARCH,32>( _.opbase(), _.rmop(), _.greg(), ic.vex() );
      if (match( ic, simd66() )) return new Ucomis<ARCH,64>( _.opbase(), _.rmop(), _.greg(), ic.vex() );
    }

  return 0;
}};

// /* RCPPS -- Compute Reciprocals of Packed Single-Precision Floating-Point Values */
// op rcpps_vdq_wdq( 0x0f[8]:> <:0x53[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rcpps_vdq_wdq.disasm = { _sink << "rcpps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* RCPSS -- Compute Reciprocal of Scalar Single-Precision Floating-Point Values */
// op rcpss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x53[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rcpss_vdq_wdq.disasm = { _sink << "rcpss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDPS -- Round Packed Single-Precision Floating-Point Values */
// op roundps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundps_vdq_wdq_ib.disasm = { _sink << "roundps " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDPD -- Round Packed Double-Precision Floating-Point Values */
// op roundpd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundpd_vdq_wdq_ib.disasm = { _sink << "roundpd " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDSS -- Round Scalar Single-Precision Floating-Point Values */
// op roundss_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundss_vdq_wdq_ib.disasm = { _sink << "roundss " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDSD -- Round Scalar Double-Precision Floating-Point Values */
// op roundsd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundsd_vdq_wdq_ib.disasm = { _sink << "roundsd " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* RSQRTPS -- Compute Reciprocals of Square Roots of Packed Single-Precision Floating-Point Values */
// op rsqrtps_vdq_wdq( 0x0f[8]:> <:0x52[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rsqrtps_vdq_wdq.disasm = { _sink << "rsqrtps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* RSQRTSS -- Compute Reciprocal of Square Root of Scalar Single-Precision Floating-Point Value */
// op rsqrtss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x52[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rsqrtss_vdq_wdq.disasm = { _sink << "rsqrtss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SHUFPS -- Shuffle Packed Single-Precision Floating-Point Values */
// op shufps_vdq_wdq_ib( 0x0f[8]:> <:0xc6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// shufps_vdq_wdq_ib.disasm = { _sink << "shufps " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SHUFPD -- Shuffle Packed Double-Precision Floating-Point Values */
// op shufpd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// shufpd_vdq_wdq_ib.disasm = { _sink << "shufpd " << DisasmI(imm) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTPS -- Compute Square Roots of Packed Single-Precision Floating-Point Values */
// op sqrtps_vdq_wdq( 0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtps_vdq_wdq.disasm = { _sink << "sqrtps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTPD -- Compute Square Roots of Packed Double-Precision Floating-Point Values */
// op sqrtpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtpd_vdq_wdq.disasm = { _sink << "sqrtpd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTSS -- Compute Square Roots of Packed Single-Precision Floating-Point Values */
// op sqrtss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtss_vdq_wdq.disasm = { _sink << "sqrtss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTSD -- Compute Square Roots of Packed Double-Precision Floating-Point Values */
// op sqrtsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtsd_vdq_wdq.disasm = { _sink << "sqrtsd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBPS -- Subtract Packed Single-Precision Floating-Point Values */
// op subps_vdq_wdq( 0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subps_vdq_wdq.disasm = { _sink << "subps " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBPD -- Subtract Packed Double-Precision Floating-Point Values */
// op subpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subpd_vdq_wdq.disasm = { _sink << "subpd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBSS -- Subtract Scalar Single-Precision Floating-Point Values */
// op subss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subss_vdq_wdq.disasm = { _sink << "subss " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBSD -- Subtract Scalar Double-Precision Floating-Point Values */
// op subsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subsd_vdq_wdq.disasm = { _sink << "subsd " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// 
// /* MOVDDUP -- Move One Double-FP and Duplicate */
// op movddup_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movddup_vdq_wdq.disasm = { _sink << "movddup " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };

