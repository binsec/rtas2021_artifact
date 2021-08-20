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

#include <data_types/fixed_point/fix_fast.h>
#include <data_types/fixed_point/fxval_fast.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Constructors
sc_fix_fast::sc_fix_fast()
{
}

sc_fix_fast::sc_fix_fast(int, int)
{
}

sc_fix_fast::sc_fix_fast(sc_q_mode, sc_o_mode)
{
}

sc_fix_fast::sc_fix_fast(sc_q_mode, sc_o_mode, int)
{
}

sc_fix_fast::sc_fix_fast(int, int, sc_q_mode, sc_o_mode)
{
}

sc_fix_fast::sc_fix_fast(int, int, sc_q_mode, sc_o_mode, int)
{
}

sc_fix_fast::sc_fix_fast(const sc_fxcast_switch&)
{
}

sc_fix_fast::sc_fix_fast(int, int, const sc_fxcast_switch&)
{
}

sc_fix_fast::sc_fix_fast(sc_q_mode, sc_o_mode, const sc_fxcast_switch&)
{
}

sc_fix_fast::sc_fix_fast(sc_q_mode, sc_o_mode, int, const sc_fxcast_switch&)
{
}

sc_fix_fast::sc_fix_fast(int, int, sc_q_mode, sc_o_mode, const sc_fxcast_switch&)
{
}

sc_fix_fast::sc_fix_fast(int, int, sc_q_mode, sc_o_mode, int, const sc_fxcast_switch&)
{
}

sc_fix_fast::sc_fix_fast(const sc_fxtype_params&)
{
}

sc_fix_fast::sc_fix_fast(const sc_fxtype_params&, const sc_fxcast_switch&)
{
}

#define DEF_CTORS_T(tp) \
sc_fix_fast::sc_fix_fast(tp, int, int) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, sc_q_mode, sc_o_mode) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, sc_q_mode, sc_o_mode, int) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, int, int, sc_q_mode, sc_o_mode) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, int, int, sc_q_mode, sc_o_mode, int) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, const sc_fxcast_switch&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, int, int, const sc_fxcast_switch&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, sc_q_mode, sc_o_mode, const sc_fxcast_switch&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, sc_q_mode, sc_o_mode, int, const sc_fxcast_switch&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, int, int, sc_q_mode, sc_o_mode, const sc_fxcast_switch&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, int, int, sc_q_mode, sc_o_mode, int, const sc_fxcast_switch&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, const sc_fxtype_params&) \
{ \
} \
  \
sc_fix_fast::sc_fix_fast(tp, const sc_fxtype_params&, const sc_fxcast_switch&) \
{ \
}

#define DEF_CTORS_T_A(tp) \
sc_fix_fast::sc_fix_fast(tp) \
{ \
} \
  \
DEF_CTORS_T(tp)

#define DEF_CTORS_T_B(tp) \
sc_fix_fast::sc_fix_fast(tp) \
{ \
} \
  \
DEF_CTORS_T(tp)

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

#undef DEF_CTORS_T
#undef DEF_CTORS_T_A
#undef DEF_CTORS_T_B

// Copy constructor
sc_fix_fast::sc_fix_fast(const sc_fix_fast&)
{
}

// Operators
const sc_fix_fast sc_fix_fast::operator ~ () const
{
}

const sc_fix_fast operator & (const sc_fix_fast&, const sc_fix_fast&)
{
}

const sc_fix_fast operator ^ (const sc_fix_fast&, const sc_fix_fast&)
{
}

const sc_fix_fast operator | (const sc_fix_fast&, const sc_fix_fast&)
{
}

sc_fix_fast& sc_fix_fast::operator = (const sc_fix_fast&)
{
}

#define DEF_ASN_OP_T(op, tp) \
sc_fix_fast& sc_fix_fast::operator op (tp) \
{ \
}

#define DEF_ASN_OP_OTHER(op) \
DEF_ASN_OP_T(op, int64) \
DEF_ASN_OP_T(op, uint64) \
DEF_ASN_OP_T(op, const sc_int_base&)\
DEF_ASN_OP_T(op, const sc_uint_base&)\
DEF_ASN_OP_T(op, const sc_signed&)\
DEF_ASN_OP_T(op, const sc_unsigned&)

#define DEF_ASN_OP(op) \
DEF_ASN_OP_T(op, int) \
DEF_ASN_OP_T(op, unsigned int) \
DEF_ASN_OP_T(op, long) \
DEF_ASN_OP_T(op, unsigned long) \
DEF_ASN_OP_T(op, float) \
DEF_ASN_OP_T(op, double) \
DEF_ASN_OP_T(op, const char *)\
DEF_ASN_OP_T(op, const sc_fxval&)\
DEF_ASN_OP_T(op, const sc_fxval_fast&)\
DEF_ASN_OP_T(op, const sc_fxnum&)\
DEF_ASN_OP_T(op, const sc_fxnum_fast&)\
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

const sc_fxval_fast sc_fix_fast::operator ++ (int)
{
}

const sc_fxval_fast sc_fix_fast::operator -- (int)
{
}

sc_fix_fast& sc_fix_fast::operator ++ ()
{
}

sc_fix_fast& sc_fix_fast::operator -- ()
{
}

} // end of namespace sc_dt
