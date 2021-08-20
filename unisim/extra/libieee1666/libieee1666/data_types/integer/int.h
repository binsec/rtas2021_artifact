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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_INT_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_INT_H__

#include <data_types/fwd.h>
#include <data_types/integer/int_base.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <int W>
class sc_int : public sc_int_base
{
public:
	// Constructors
	sc_int();
	sc_int(int_type v);
	sc_int(const sc_int<W>& a);
	sc_int(const sc_int_base& a);
	sc_int(const sc_int_subref_r& a);
	template <class T> sc_int(const sc_generic_base<T>& a);
	sc_int(const sc_signed& a);
	sc_int(const sc_unsigned& a);
	explicit sc_int(const sc_fxval& a);
	explicit sc_int(const sc_fxval_fast& a);
	explicit sc_int(const sc_fxnum& a);
	explicit sc_int(const sc_fxnum_fast& a);
	sc_int(const sc_bv_base& a);
	sc_int(const sc_lv_base& a);
	sc_int(const char *a);
	sc_int(unsigned long a);
	sc_int(long a);
	sc_int(unsigned int a);
	sc_int(int a);
	sc_int(uint64 a);
	sc_int(double a);

	// Assignment operators
	sc_int<W>& operator = (int_type v);
	sc_int<W>& operator = (const sc_int_base& a);
	sc_int<W>& operator = (const sc_int_subref_r& a);
	sc_int<W>& operator = (const sc_int<W>& a);
	template <class T> sc_int<W>& operator = (const sc_generic_base<T>& a);
	sc_int<W>& operator = (const sc_signed& a);
	sc_int<W>& operator = (const sc_unsigned& a);
	sc_int<W>& operator = (const sc_fxval& a);
	sc_int<W>& operator = (const sc_fxval_fast& a);
	sc_int<W>& operator = (const sc_fxnum& a);
	sc_int<W>& operator = (const sc_fxnum_fast& a);
	sc_int<W>& operator = (const sc_bv_base& a);
	sc_int<W>& operator = (const sc_lv_base& a);
	sc_int<W>& operator = (const char *a);
	sc_int<W>& operator = (unsigned long a);
	sc_int<W>& operator = (long a);
	sc_int<W>& operator = (unsigned int a);
	sc_int<W>& operator = (int a);
	sc_int<W>& operator = (uint64 a);
	sc_int<W>& operator = (double a);

	// Prefix and postfix increment and decrement operators
	sc_int<W>& operator ++ ();         // Prefix
	const sc_int<W> operator ++ (int); // Postfix
	sc_int<W>& operator -- ();         // Prefix
	const sc_int<W> operator -- (int); // Postfix
};

///////////////////////////////// definition //////////////////////////////////

// Constructors

template <int W>
sc_int<W>::sc_int()
{
}

template <int W>
sc_int<W>::sc_int(int_type v)
{
}

template <int W>
sc_int<W>::sc_int(const sc_int<W>& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_int_base& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_int_subref_r& a)
{
}

template <int W>
template <class T> sc_int<W>::sc_int(const sc_generic_base<T>& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_signed& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_unsigned& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_fxval& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_fxval_fast& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_fxnum& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_fxnum_fast& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_bv_base& a)
{
}

template <int W>
sc_int<W>::sc_int(const sc_lv_base& a)
{
}

template <int W>
sc_int<W>::sc_int(const char *a)
{
}

template <int W>
sc_int<W>::sc_int(unsigned long a)
{
}

template <int W>
sc_int<W>::sc_int(long a)
{
}

template <int W>
sc_int<W>::sc_int(unsigned int a)
{
}

template <int W>
sc_int<W>::sc_int(int a)
{
}

template <int W>
sc_int<W>::sc_int(uint64 a)
{
}

template <int W>
sc_int<W>::sc_int(double a)
{
}

// Assignment operators
template <int W>
sc_int<W>& sc_int<W>::operator = (int_type v)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_int_base& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_int_subref_r& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_int<W>& a)
{
}

template <int W>
template <class T> sc_int<W>& sc_int<W>::operator = (const sc_generic_base<T>& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_signed& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_unsigned& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_fxval& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_fxval_fast& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_fxnum& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_fxnum_fast& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_bv_base& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const sc_lv_base& a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (const char *a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (unsigned long a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (long a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (unsigned int a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (int a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (uint64 a)
{
}

template <int W>
sc_int<W>& sc_int<W>::operator = (double a)
{
}

// Prefix and postfix increment and decrement operators

template <int W>
sc_int<W>& sc_int<W>::operator ++ ()          // Prefix
{
}

template <int W>
const sc_int<W> sc_int<W>::operator ++ (int)  // Postfix
{
}

template <int W>
sc_int<W>& sc_int<W>::operator -- ()          // Prefix
{
}

template <int W>
const sc_int<W> sc_int<W>::operator -- (int)  // Postfix
{
}

} // end of namespace sc_dt

#endif
