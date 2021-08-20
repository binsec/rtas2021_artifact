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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_BIGINT_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_BIGINT_H__

#include <data_types/fwd.h>
#include <data_types/integer/signed.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <int W>
class sc_bigint : public sc_signed
{
public:
	// Constructors
	sc_bigint();
	sc_bigint(const sc_bigint<W>& v);
	sc_bigint(const sc_signed& v);
	sc_bigint(const sc_signed_subref& v);
	template <class T> sc_bigint(const sc_generic_base<T>& a);
	sc_bigint(const sc_unsigned& v);
	sc_bigint(const sc_unsigned_subref& v);
	sc_bigint(const char *v);
	sc_bigint(int64 v);
	sc_bigint(uint64 v);
	sc_bigint(long v);
	sc_bigint(unsigned long v);
	sc_bigint(int v);
	sc_bigint(unsigned int v);
	sc_bigint(double v);
	sc_bigint(const sc_bv_base& v);
	sc_bigint(const sc_lv_base& v);
	explicit sc_bigint(const sc_fxval& v);
	explicit sc_bigint(const sc_fxval_fast& v);
	explicit sc_bigint(const sc_fxnum& v);
	explicit sc_bigint(const sc_fxnum_fast& v);

	// Destructor
	~sc_bigint();

	// Assignment operators
	sc_bigint<W>& operator = (const sc_bigint<W>& v);
	sc_bigint<W>& operator = (const sc_signed& v);
	sc_bigint<W>& operator = (const sc_signed_subref& v);
	template <class T> sc_bigint<W>& operator = (const sc_generic_base<T>& a);
	sc_bigint<W>& operator = (const sc_unsigned& v);
	sc_bigint<W>& operator = (const sc_unsigned_subref& v);
	sc_bigint<W>& operator = (const char *v);
	sc_bigint<W>& operator = (int64 v);
	sc_bigint<W>& operator = (uint64 v);
	sc_bigint<W>& operator = (long v);
	sc_bigint<W>& operator = (unsigned long v);
	sc_bigint<W>& operator = (int v);
	sc_bigint<W>& operator = (unsigned int v);
	sc_bigint<W>& operator = (double v);
	sc_bigint<W>& operator = (const sc_bv_base& v);
	sc_bigint<W>& operator = (const sc_lv_base& v);
	sc_bigint<W>& operator = (const sc_int_base& v);
	sc_bigint<W>& operator = (const sc_uint_base& v);
	sc_bigint<W>& operator = (const sc_fxval& v);
	sc_bigint<W>& operator = (const sc_fxval_fast& v);
	sc_bigint<W>& operator = (const sc_fxnum& v);
	sc_bigint<W>& operator = (const sc_fxnum_fast& v);
};

///////////////////////////////// definition //////////////////////////////////

// Constructors
template <int W>
sc_bigint<W>::sc_bigint()
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_bigint<W>& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_signed& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_signed_subref& v)
{
}

template <int W>
template <class T> sc_bigint<W>::sc_bigint(const sc_generic_base<T>& a)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_unsigned& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_unsigned_subref& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const char *v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(int64 v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(uint64 v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(long v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(unsigned long v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(int v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(unsigned int v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(double v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_bv_base& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_lv_base& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_fxval& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_fxval_fast& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_fxnum& v)
{
}

template <int W>
sc_bigint<W>::sc_bigint(const sc_fxnum_fast& v)
{
}

// Destructor
template <int W>
sc_bigint<W>::~sc_bigint()
{
}

// Assignment operators
template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_bigint<W>& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_signed& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_signed_subref& v)
{
}

template <int W>
template <class T> sc_bigint<W>& sc_bigint<W>::operator = (const sc_generic_base<T>& a)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_unsigned& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_unsigned_subref& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const char *v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (int64 v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (uint64 v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (long v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (unsigned long v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (int v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (unsigned int v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (double v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_bv_base& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_lv_base& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_int_base& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_uint_base& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_fxval& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_fxval_fast& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_fxnum& v)
{
}

template <int W>
sc_bigint<W>& sc_bigint<W>::operator = (const sc_fxnum_fast& v)
{
}

} // end of namespace sc_dt

#endif
