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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_LV_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_LV_H__

#include <data_types/fwd.h>
#include <data_types/bit/lv_base.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <int W>
class sc_lv : public sc_lv_base
{
public:
	// Constructors
	sc_lv();
	explicit sc_lv(const sc_logic& init_value);
	explicit sc_lv(bool init_value);
	explicit sc_lv(char init_value);
	sc_lv(const char *a);
	sc_lv(const bool *a);
	sc_lv(const sc_logic *a);
	sc_lv(const sc_unsigned& a);
	sc_lv(const sc_signed& a);
	sc_lv(const sc_uint_base& a);
	sc_lv(const sc_int_base& a);
	sc_lv(unsigned long a);
	sc_lv(long a);
	sc_lv(unsigned int a);
	sc_lv(int a);
	sc_lv(uint64 a);
	sc_lv(int64 a);
	template <class X> sc_lv(const sc_subref_r<X>& a);
	template <class T1, class T2> sc_lv(const sc_concref_r<T1,T2>& a);
	sc_lv(const sc_bv_base& a);
	sc_lv(const sc_lv_base& a);
	sc_lv(const sc_lv<W>& a);
	
	// Assignment operators
	template <class X> sc_lv<W>& operator = (const sc_subref_r<X>& a);
	template <class T1, class T2> sc_lv<W>& operator = (const sc_concref_r<T1,T2>& a);
	sc_lv<W>& operator = (const sc_bv_base& a);
	sc_lv<W>& operator = (const sc_lv_base& a);
	sc_lv<W>& operator = (const sc_lv<W>& a);
	sc_lv<W>& operator = (const char *a);
	sc_lv<W>& operator = (const bool *a);
	sc_lv<W>& operator = (const sc_logic *a);
	sc_lv<W>& operator = (const sc_unsigned& a);
	sc_lv<W>& operator = (const sc_signed& a);
	sc_lv<W>& operator = (const sc_uint_base& a);
	sc_lv<W>& operator = (const sc_int_base& a);
	sc_lv<W>& operator = (unsigned long a);
	sc_lv<W>& operator = (long a);
	sc_lv<W>& operator = (unsigned int a);
	sc_lv<W>& operator = (int a);
	sc_lv<W>& operator = (uint64 a);
	sc_lv<W>& operator = (int64 a);
};

///////////////////////////////// definition //////////////////////////////////

// Constructors

template <int W>
sc_lv<W>::sc_lv()
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_logic& init_value)
{
}

template <int W>
sc_lv<W>::sc_lv(bool init_value)
{
}

template <int W>
sc_lv<W>::sc_lv(char init_value)
{
}

template <int W>
sc_lv<W>::sc_lv(const char *a)
{
}

template <int W>
sc_lv<W>::sc_lv(const bool *a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_logic *a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_unsigned& a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_signed& a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_uint_base& a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_int_base& a)
{
}

template <int W>
sc_lv<W>::sc_lv(unsigned long a)
{
}

template <int W>
sc_lv<W>::sc_lv(long a)
{
}

template <int W>
sc_lv<W>::sc_lv(unsigned int a)
{
}

template <int W>
sc_lv<W>::sc_lv(int a)
{
}

template <int W>
sc_lv<W>::sc_lv(uint64 a)
{
}

template <int W>
sc_lv<W>::sc_lv(int64 a)
{
}

template <int W>
template <class X> sc_lv<W>::sc_lv(const sc_subref_r<X>& a)
{
}

template <int W>
template <class T1, class T2> sc_lv<W>::sc_lv(const sc_concref_r<T1,T2>& a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_bv_base& a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_lv_base& a)
{
}

template <int W>
sc_lv<W>::sc_lv(const sc_lv<W>& a)
{
}

// Assignment operators

template <int W>
template <class X> sc_lv<W>& sc_lv<W>::operator = (const sc_subref_r<X>& a)
{
}

template <int W>
template <class T1, class T2> sc_lv<W>& sc_lv<W>::operator = (const sc_concref_r<T1,T2>& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_bv_base& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_lv_base& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_lv<W>& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const char *a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const bool *a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_logic *a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_unsigned& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_signed& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_uint_base& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (const sc_int_base& a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (unsigned long a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (long a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (unsigned int a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (int a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (uint64 a)
{
}

template <int W>
sc_lv<W>& sc_lv<W>::operator = (int64 a)
{
}

} // end of namespace sc_dt

#endif
