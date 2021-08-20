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

#ifndef __LIBIEEE1666_DATA_TYPES_FWD_H__
#define __LIBIEEE1666_DATA_TYPES_FWD_H__

#include <data_types/native.h>

namespace sc_dt {

enum sc_numrep
{
	SC_NOBASE = 0,
	SC_BIN = 2,
	SC_OCT = 8,
	SC_DEC = 10,
	SC_HEX = 16,
	SC_BIN_US,
	SC_BIN_SM,
	SC_OCT_US,
	SC_OCT_SM,
	SC_HEX_US,
	SC_HEX_SM,
	SC_CSD
};

enum sc_logic_value_t
{
	Log_0 = 0,
	Log_1,
	Log_Z,
	Log_X
};

enum sc_o_mode
{
	SC_SAT,       // Saturation
	SC_SAT_ZERO,  // Saturation to zero
	SC_SAT_SYM,   // Symmetrical saturation
	SC_WRAP,      // Wrap-around (*)
	SC_WRAP_SM    // Sign magnitude wrap-around (*)
};

enum sc_q_mode
{
	SC_RND,          // Rounding to plus infinity
	SC_RND_ZERO,     // Rounding to zero
	SC_RND_MIN_INF,  // Rounding to minus infinity
	SC_RND_INF,      // Rounding to infinity
	SC_RND_CONV,     // Convergent rounding
	SC_TRN,          // Truncation
	SC_TRN_ZERO      // Truncation to zero
};

enum sc_fmt
{
	SC_F,
	SC_E
};

enum sc_switch
{
    SC_OFF,
    SC_ON
};

enum sc_context_begin
{
	SC_NOW,
	SC_LATER
};

const sc_q_mode SC_DEFAULT_Q_MODE_ = SC_TRN;
const sc_o_mode SC_DEFAULT_O_MODE_ = SC_WRAP;
const int SC_DEFAULT_N_BITS_ = 0;

template <class T1, class T2> class sc_concref;
class sc_logic;
template <int W> class sc_bv;
template <class T> class sc_subref;
template <class T> class sc_subref_r;
template <int W> class sc_lv;
class sc_lv_base;
template <class T> class sc_bitref;
template <class T1, class T2> class sc_concref_r; 
class sc_bv_base;
template <class T> class sc_bitref_r;
class sc_uint_bitref;
class sc_unsigned_subref;
class sc_signed;
class sc_signed_bitref_r;
class sc_value_base;
class sc_unsigned_subref_r;
class sc_uint_subref;
template <int W> class sc_uint;
template <class T> class sc_generic_base;
class sc_int_subref;
class sc_unsigned_bitref;
class sc_unsigned_bitref_r;
class sc_signed_subref;
class sc_uint_bitref_r;
class sc_int_bitref;
template <int W> class sc_int;
class sc_unsigned;
class sc_int_bitref_r;
class sc_signed_bitref;
class sc_concatref;
class sc_int_subref_r;
class sc_uint_base;
class sc_uint_subref_r;
template <int W> class sc_bigint;
class sc_int_base;
class sc_signed_subref_r;
template <int W> class sc_biguint;
class sc_fxnum_fast;
class sc_fix;
class sc_fix_fast;
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> class sc_ufixed;
class sc_fxval;
class sc_ufix;
class sc_fxnum_subref;
class sc_fxval_fast;
class sc_ufix_fast;
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> class sc_fixed_fast;
class sc_fxnum_bitref;
class sc_fxnum_fast_subref;
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> class sc_fixed;
template <int W, int I, sc_q_mode Q, sc_o_mode O, int N> class sc_ufixed_fast;
class sc_fxnum_fast_bitref;
class sc_fxnum;
class sc_fxtype_context;
class sc_fxcast_switch;
class sc_length_context;
class sc_fxcast_switch;
class sc_fxtype_params;
class sc_fxcast_context;

template <class T> const T sc_abs(const T& v);
template <class T> const T sc_max(const T& a, const T& b);
template <class T> const T sc_min(const T& a, const T& b);

} // end of namespace sc_dt

#endif
