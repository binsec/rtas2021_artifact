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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_UINT_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_UINT_H__

#include <data_types/fwd.h>
#include <data_types/integer/uint_base.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <int W>
class sc_uint : public sc_uint_base
{
public:
	// Constructors
	sc_uint();
	sc_uint(uint_type v);
	sc_uint(const sc_uint<W>& a);
	sc_uint(const sc_uint_base& a);
	sc_uint(const sc_uint_subref_r & a);
	template <class T> sc_uint(const sc_generic_base<T>& a);
	sc_uint(const sc_signed& a);
	sc_uint(const sc_unsigned& a);
	explicit sc_uint(const sc_fxval& a);
	explicit sc_uint(const sc_fxval_fast& a);
	explicit sc_uint(const sc_fxnum& a);
	explicit sc_uint(const sc_fxnum_fast& a);
	sc_uint(const sc_bv_base& a);
	sc_uint(const sc_lv_base& a);
	sc_uint(const char *a);
	sc_uint(unsigned long a);
	sc_uint(long a);
	sc_uint(unsigned int a);
	sc_uint(int a);
	sc_uint(int64 a);
	sc_uint(double a);

	// Assignment operators
	sc_uint<W>& operator = (uint_type v);
	sc_uint<W>& operator = (const sc_uint_base& a);
	sc_uint<W>& operator = (const sc_uint_subref_r & a);
	sc_uint<W>& operator = (const sc_uint<W>& a);
	template <class T> sc_uint<W>& operator = (const sc_generic_base<T>& a);
	sc_uint<W>& operator = (const sc_signed& a);
	sc_uint<W>& operator = (const sc_unsigned& a);
	sc_uint<W>& operator = (const sc_fxval& a);
	sc_uint<W>& operator = (const sc_fxval_fast& a);
	sc_uint<W>& operator = (const sc_fxnum& a);
	sc_uint<W>& operator = (const sc_fxnum_fast& a);
	sc_uint<W>& operator = (const sc_bv_base& a);
	sc_uint<W>& operator = (const sc_lv_base& a);
	sc_uint<W>& operator = (const char *a);
	sc_uint<W>& operator = (unsigned long a);
	sc_uint<W>& operator = (long a);
	sc_uint<W>& operator = (unsigned int a);
	sc_uint<W>& operator = (int a);
	sc_uint<W>& operator = (int64 a);
	sc_uint<W>& operator = (double a);

	// Prefix and postfix increment and decrement operators
	sc_uint<W>& operator ++ ();         // Prefix
	const sc_uint<W> operator ++ (int); // Postfix
	sc_uint<W>& operator -- ();         // Prefix
	const sc_uint<W> operator -- (int); // Postfix
};

///////////////////////////////// definition //////////////////////////////////

// Constructors
template <int W>
sc_uint<W>::sc_uint()
{
}

template <int W>
sc_uint<W>::sc_uint(uint_type v)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_uint<W>& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_uint_base& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_uint_subref_r & a)
{
}

template <int W>
template <class T> sc_uint<W>::sc_uint(const sc_generic_base<T>& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_signed& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_unsigned& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_fxval& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_fxval_fast& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_fxnum& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_fxnum_fast& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_bv_base& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const sc_lv_base& a)
{
}

template <int W>
sc_uint<W>::sc_uint(const char *a)
{
}

template <int W>
sc_uint<W>::sc_uint(unsigned long a)
{
}

template <int W>
sc_uint<W>::sc_uint(long a)
{
}

template <int W>
sc_uint<W>::sc_uint(unsigned int a)
{
}

template <int W>
sc_uint<W>::sc_uint(int a)
{
}

template <int W>
sc_uint<W>::sc_uint(int64 a)
{
}

template <int W>
sc_uint<W>::sc_uint(double a)
{
}

// Assignment operators
template <int W>
sc_uint<W>& sc_uint<W>::operator = (uint_type v)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_uint_base& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_uint_subref_r & a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_uint<W>& a)
{
}

template <int W>
template <class T> sc_uint<W>& sc_uint<W>::operator = (const sc_generic_base<T>& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_signed& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_unsigned& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_fxval& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_fxval_fast& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_fxnum& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_fxnum_fast& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_bv_base& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const sc_lv_base& a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (const char *a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (unsigned long a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (long a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (unsigned int a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (int a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (int64 a)
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator = (double a)
{
}

// Prefix and postfix increment and decrement operators
template <int W>
sc_uint<W>& sc_uint<W>::operator ++ ()          // Prefix
{
}

template <int W>
const sc_uint<W> sc_uint<W>::operator ++ (int)  // Postfix
{
}

template <int W>
sc_uint<W>& sc_uint<W>::operator -- ()          // Prefix
{
}

template <int W>
const sc_uint<W> sc_uint<W>::operator -- (int)  // Postfix
{
}

} // end of namespace sc_dt

#endif
