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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_BV_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_BV_H__

#include <data_types/fwd.h>
#include <data_types/bit/bv_base.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <int W>
class sc_bv : public sc_bv_base
{
public:
	// Constructors
	sc_bv();
	explicit sc_bv(bool init_value);
	explicit sc_bv(char init_value);
	sc_bv(const char *a);
	sc_bv(const bool *a);
	sc_bv(const sc_logic *a);
	sc_bv(const sc_unsigned& a);
	sc_bv(const sc_signed& a);
	sc_bv(const sc_uint_base& a);
	sc_bv(const sc_int_base& a);
	sc_bv(unsigned long a);
	sc_bv(long a);
	sc_bv(unsigned int a);
	sc_bv(int a);
	sc_bv(uint64 a);
	sc_bv(int64 a);
	template <class X> sc_bv(const sc_subref_r<X>& a);
	template <class T1, class T2> sc_bv(const sc_concref_r<T1,T2>& a);
	sc_bv(const sc_bv_base& a);
	sc_bv(const sc_lv_base& a);
	sc_bv(const sc_bv<W>& a);

	// Assignment operators
	template <class X> sc_bv<W>& operator = (const sc_subref_r<X>& a);
	template <class T1, class T2> sc_bv<W>& operator = (const sc_concref_r <T1,T2>& a);
	sc_bv<W>& operator = (const sc_bv_base& a);
	sc_bv<W>& operator = (const sc_lv_base& a);
	sc_bv<W>& operator = (const sc_bv<W>& a);
	sc_bv<W>& operator = (const char *a);
	sc_bv<W>& operator = (const bool *a);
	sc_bv<W>& operator = (const sc_logic *a);
	sc_bv<W>& operator = (const sc_unsigned& a);
	sc_bv<W>& operator = (const sc_signed& a);
	sc_bv<W>& operator = (const sc_uint_base& a);
	sc_bv<W>& operator = (const sc_int_base& a);
	sc_bv<W>& operator = (unsigned long a);
	sc_bv<W>& operator = (long a);
	sc_bv<W>& operator = (unsigned int a);
	sc_bv<W>& operator = (int a);
	sc_bv<W>& operator = (uint64 a);
	sc_bv<W>& operator = (int64 a);
};

///////////////////////////////// definition //////////////////////////////////

// Constructors
template <int W>
sc_bv<W>::sc_bv()
{
}

template <int W>
sc_bv<W>::sc_bv(bool init_value)
{
}

template <int W>
sc_bv<W>::sc_bv(char init_value)
{
}

template <int W>
sc_bv<W>::sc_bv(const char *a)
{
}

template <int W>
sc_bv<W>::sc_bv(const bool *a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_logic *a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_unsigned& a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_signed& a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_uint_base& a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_int_base& a)
{
}

template <int W>
sc_bv<W>::sc_bv(unsigned long a)
{
}

template <int W>
sc_bv<W>::sc_bv(long a)
{
}

template <int W>
sc_bv<W>::sc_bv(unsigned int a)
{
}

template <int W>
sc_bv<W>::sc_bv(int a)
{
}

template <int W>
sc_bv<W>::sc_bv(uint64 a)
{
}

template <int W>
sc_bv<W>::sc_bv(int64 a)
{
}

template <int W>
template <class X> sc_bv<W>::sc_bv(const sc_subref_r<X>& a)
{
}

template <int W>
template <class T1, class T2> sc_bv<W>::sc_bv(const sc_concref_r<T1,T2>& a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_bv_base& a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_lv_base& a)
{
}

template <int W>
sc_bv<W>::sc_bv(const sc_bv<W>& a)
{
}


// Assignment operators
template <int W>
template <class X> sc_bv<W>& sc_bv<W>::operator = (const sc_subref_r<X>& a)
{
}

template <int W>
template <class T1, class T2> sc_bv<W>& sc_bv<W>::operator = (const sc_concref_r <T1,T2>& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_bv_base& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_lv_base& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_bv<W>& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const char *a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const bool *a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_logic *a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_unsigned& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_signed& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_uint_base& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (const sc_int_base& a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (unsigned long a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (long a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (unsigned int a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (int a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (uint64 a)
{
}

template <int W>
sc_bv<W>& sc_bv<W>::operator = (int64 a)
{
}

} // end of namespace sc_dt

#endif
