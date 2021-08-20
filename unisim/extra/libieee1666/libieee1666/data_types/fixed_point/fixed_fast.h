/*
 *  Copyright (c) 2017,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __LIBIEEE1666_DATA_TYPES_FIXED_POINT_FIXED_FAST_H__
#define __LIBIEEE1666_DATA_TYPES_FIXED_POINT_FIXED_FAST_H__

#include <data_types/fwd.h>
#include <data_types/fixed_point/fix_fast.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <int W, int I,
          sc_q_mode Q = SC_DEFAULT_Q_MODE_,
          sc_o_mode O = SC_DEFAULT_O_MODE_, int N = SC_DEFAULT_N_BITS_>
class sc_fixed_fast : public sc_fix_fast
{
public:
	// Constructors
	sc_fixed_fast();
	sc_fixed_fast(const sc_fxcast_switch&);

#define DECL_CTORS_T_A(tp) \
	sc_fixed_fast(tp); \
	sc_fixed_fast(tp, const sc_fxcast_switch&);

#define DECL_CTORS_T_B(tp) \
	sc_fixed_fast(tp); \
	sc_fixed_fast(tp, const sc_fxcast_switch&);

	DECL_CTORS_T_A(int)
	DECL_CTORS_T_A(unsigned int)
	DECL_CTORS_T_A(long)
	DECL_CTORS_T_A(unsigned long)
	DECL_CTORS_T_A(float)
	DECL_CTORS_T_A(double)
	DECL_CTORS_T_A(const char *)
	DECL_CTORS_T_A(const sc_fxval&)
	DECL_CTORS_T_A(const sc_fxval_fast&)
	DECL_CTORS_T_A(const sc_fxnum&)
	DECL_CTORS_T_A(const sc_fxnum_fast&)
	DECL_CTORS_T_B(int64)
	DECL_CTORS_T_B(uint64)
	DECL_CTORS_T_B(const sc_int_base&)
	DECL_CTORS_T_B(const sc_uint_base&)
	DECL_CTORS_T_B(const sc_signed&)
	DECL_CTORS_T_B(const sc_unsigned&)

#undef DECL_CTORS_T_A
#undef DECL_CTORS_T_B

	sc_fixed_fast(const sc_fixed_fast<W, I, Q, O, N>&);

	// Operators
	sc_fixed_fast& operator = (const sc_fixed_fast<W, I, Q, O, N>&);

#define DECL_ASN_OP_T(op, tp) \
	sc_fixed_fast& operator op (tp);

#define DECL_ASN_OP_OTHER(op) \
	DECL_ASN_OP_T(op, int64) \
	DECL_ASN_OP_T(op, uint64) \
	DECL_ASN_OP_T(op, const sc_int_base&) \
	DECL_ASN_OP_T(op, const sc_uint_base&) \
	DECL_ASN_OP_T(op, const sc_signed&) \
	DECL_ASN_OP_T(op, const sc_unsigned&)

#define DECL_ASN_OP(op) \
	DECL_ASN_OP_T(op, int) \
	DECL_ASN_OP_T(op, unsigned int) \
	DECL_ASN_OP_T(op, long) \
	DECL_ASN_OP_T(op, unsigned long) \
	DECL_ASN_OP_T(op, float) \
	DECL_ASN_OP_T(op, double) \
	DECL_ASN_OP_T(op, const char *) \
	DECL_ASN_OP_T(op, const sc_fxval&) \
	DECL_ASN_OP_T(op, const sc_fxval_fast&) \
	DECL_ASN_OP_T(op, const sc_fxnum&) \
	DECL_ASN_OP_T(op, const sc_fxnum_fast&) \
	DECL_ASN_OP_OTHER(op)

	DECL_ASN_OP(=)
	DECL_ASN_OP(*=)
	DECL_ASN_OP(/=)
	DECL_ASN_OP(+=)
	DECL_ASN_OP(-=)
	DECL_ASN_OP_T(<<=, int)
	DECL_ASN_OP_T(>>=, int)
	DECL_ASN_OP_T(&=, const sc_fix&)
	DECL_ASN_OP_T(&=, const sc_fix_fast&)
	DECL_ASN_OP_T(|=, const sc_fix&)
	DECL_ASN_OP_T(|=, const sc_fix_fast&)
	DECL_ASN_OP_T(^=, const sc_fix&)
	DECL_ASN_OP_T(^=, const sc_fix_fast&)

#undef DECL_ASN_OP_T
#undef DECL_ASN_OP_OTHER
#undef DECL_ASN_OP

	const sc_fxval_fast operator ++ (int);
	const sc_fxval_fast operator -- (int);
	sc_fixed_fast& operator ++ ();
	sc_fixed_fast& operator -- ();

};

///////////////////////////////// definition //////////////////////////////////

// Constructors
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast()
{
}

template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast(const sc_fxcast_switch&)
{
}

#define DEF_CTORS_T_A(tp) \
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> \
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast(tp) \
{ \
} \
  \
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> \
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast(tp, const sc_fxcast_switch&) \
{ \
}

#define DEF_CTORS_T_B(tp) \
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> \
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast(tp) \
{ \
} \
  \
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> \
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast(tp, const sc_fxcast_switch&) \
{ \
}

DEF_CTORS_T_A(int)
DEF_CTORS_T_A(unsigned int)
DEF_CTORS_T_A(long)
DEF_CTORS_T_A(unsigned long)
DEF_CTORS_T_A(float)
DEF_CTORS_T_A(double)
DEF_CTORS_T_A(const char *)
DEF_CTORS_T_A(const sc_fxval&)
DEF_CTORS_T_A(const sc_fxval_fast&)
DEF_CTORS_T_A(const sc_fxnum&)
DEF_CTORS_T_A(const sc_fxnum_fast&)
DEF_CTORS_T_B(int64)
DEF_CTORS_T_B(uint64)
DEF_CTORS_T_B(const sc_int_base&)
DEF_CTORS_T_B(const sc_uint_base&)
DEF_CTORS_T_B(const sc_signed&)
DEF_CTORS_T_B(const sc_unsigned&)

#undef DEF_CTORS_T_A
#undef DEF_CTORS_T_B

template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
sc_fixed_fast<W, I, Q, O, N>::sc_fixed_fast(const sc_fixed_fast<W, I, Q, O, N>&)
{
}

// Operators
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
sc_fixed_fast<W, I, Q, O, N>& sc_fixed_fast<W, I, Q, O, N>::operator = (const sc_fixed_fast<W, I, Q, O, N>&)
{
}

#define DEF_ASN_OP_T(op, tp) \
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> \
sc_fixed_fast<W, I, Q, O, N>& sc_fixed_fast<W, I, Q, O, N>::operator op (tp) \
{ \
}

#define DEF_ASN_OP_OTHER(op) \
DEF_ASN_OP_T(op, int64) \
DEF_ASN_OP_T(op, uint64) \
DEF_ASN_OP_T(op, const sc_int_base&) \
DEF_ASN_OP_T(op, const sc_uint_base&) \
DEF_ASN_OP_T(op, const sc_signed&) \
DEF_ASN_OP_T(op, const sc_unsigned&)

#define DEF_ASN_OP(op) \
DEF_ASN_OP_T(op, int) \
DEF_ASN_OP_T(op, unsigned int) \
DEF_ASN_OP_T(op, long) \
DEF_ASN_OP_T(op, unsigned long) \
DEF_ASN_OP_T(op, float) \
DEF_ASN_OP_T(op, double) \
DEF_ASN_OP_T(op, const char *) \
DEF_ASN_OP_T(op, const sc_fxval&) \
DEF_ASN_OP_T(op, const sc_fxval_fast&) \
DEF_ASN_OP_T(op, const sc_fxnum&) \
DEF_ASN_OP_T(op, const sc_fxnum_fast&) \
DEF_ASN_OP_OTHER(op)

DEF_ASN_OP(=)
DEF_ASN_OP(*=)
DEF_ASN_OP(/=)
DEF_ASN_OP(+=)
DEF_ASN_OP(-=)
DEF_ASN_OP_T(<<=, int)
DEF_ASN_OP_T(>>=, int)
DEF_ASN_OP_T(&=, const sc_fix&)
DEF_ASN_OP_T(&=, const sc_fix_fast&)
DEF_ASN_OP_T(|=, const sc_fix&)
DEF_ASN_OP_T(|=, const sc_fix_fast&)
DEF_ASN_OP_T(^=, const sc_fix&)
DEF_ASN_OP_T(^=, const sc_fix_fast&)

#undef DEF_ASN_OP_T
#undef DEF_ASN_OP_OTHER
#undef DEF_ASN_OP

template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
const sc_fxval_fast sc_fixed_fast<W, I, Q, O, N>::operator ++ (int)
{
}

template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
const sc_fxval_fast sc_fixed_fast<W, I, Q, O, N>::operator -- (int)
{
}

template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
sc_fixed_fast<W, I, Q, O, N>& sc_fixed_fast<W, I, Q, O, N>::operator ++ ()
{
}

template <int W, int I, sc_q_mode Q, sc_o_mode O, int N>
sc_fixed_fast<W, I, Q, O, N>& sc_fixed_fast<W, I, Q, O, N>::operator -- ()
{
}

} // end of namespace sc_dt

#endif
