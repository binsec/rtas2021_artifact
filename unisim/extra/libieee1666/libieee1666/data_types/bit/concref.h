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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_CONCREF_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_CONCREF_H__

#include <data_types/fwd.h>
#include <data_types/bit/concref_r.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <class T1, class T2>
class sc_concref : public sc_concref_r<T1,T2>
{
public:
	// Copy constructor
	sc_concref(const sc_concref<T1,T2>& a);

	// Assignment operators
	template <class T> sc_concref <T1,T2>& operator = (const sc_subref_r<T>& a);
	template <class U1, class U2> sc_concref <T1,T2>& operator = (const sc_concref_r<U1,U2>& a);
	sc_concref <T1,T2>& operator = (const sc_bv_base& a);
	sc_concref <T1,T2>& operator = (const sc_lv_base& a);
	sc_concref <T1,T2>& operator = (const sc_concref<T1,T2>& a);
	sc_concref <T1,T2>& operator = (const char *a);
	sc_concref <T1,T2>& operator = (const bool* a);
	sc_concref <T1,T2>& operator = (const sc_logic* a);
	sc_concref <T1,T2>& operator = (const sc_unsigned& a);
	sc_concref <T1,T2>& operator = (const sc_signed& a);
	sc_concref <T1,T2>& operator = (const sc_uint_base& a);
	sc_concref <T1,T2>& operator = (const sc_int_base& a);
	sc_concref <T1,T2>& operator = (unsigned long a);
	sc_concref <T1,T2>& operator = (long a);
	sc_concref <T1,T2>& operator = (unsigned int a);
	sc_concref <T1,T2>& operator = (int a);
	sc_concref <T1,T2>& operator = (uint64 a);
	sc_concref <T1,T2>& operator = (int64 a);

	// Bitwise rotations
	sc_concref <T1,T2>& lrotate(int n);
	sc_concref <T1,T2>& rrotate(int n);

	// Bitwise reverse
	sc_concref <T1,T2>& reverse();

	// Bit selection
	sc_bitref<sc_concref<T1,T2> > operator [] (int i);

	// Part selection
	sc_subref<sc_concref<T1,T2> > operator () (int hi, int lo);
	sc_subref<sc_concref<T1,T2> > range(int hi, int lo);

	// Other methods
	void scan(std::istream& = std::cin);

private:
	// Disabled
	sc_concref();
};

// r-value concatenation operators and functions
template <typename C1, typename C2> sc_concref_r<C1,C2> operator , (C1, C2);
template <typename C1, typename C2> sc_concref_r<C1,C2> concat(C1, C2);

// l-value concatenation operators and functions
template <typename C1, typename C2> sc_concref<C1,C2> operator , (C1, C2);
template <typename C1, typename C2> sc_concref<C1,C2> concat(C1, C2);

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
template <class T1, class T2>
sc_concref<T1,T2>::sc_concref(const sc_concref<T1,T2>& a)
{
}

// Assignment operators
template <class T1, class T2>
template <class T> sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_subref_r<T>& a)
{
}

template <class T1, class T2>
template <class U1, class U2> sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_concref_r<U1,U2>& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_bv_base& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_lv_base& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_concref<T1,T2>& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const char *a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const bool* a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_logic* a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_unsigned& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_signed& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_uint_base& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (const sc_int_base& a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (unsigned long a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (long a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (unsigned int a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (int a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (uint64 a)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::operator = (int64 a)
{
}

// Bitwise rotations
template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::lrotate(int n)
{
}

template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::rrotate(int n)
{
}


// Bitwise reverse
template <class T1, class T2>
sc_concref <T1,T2>& sc_concref<T1,T2>::reverse()
{
}


// Bit selection
template <class T1, class T2>
sc_bitref<sc_concref<T1,T2> > sc_concref<T1,T2>::operator [] (int i)
{
}

// Part selection
template <class T1, class T2>
sc_subref<sc_concref<T1,T2> > sc_concref<T1,T2>::operator () (int hi, int lo)
{
}

template <class T1, class T2>
sc_subref<sc_concref<T1,T2> > sc_concref<T1,T2>::range(int hi, int lo)
{
}

// Other methods
template <class T1, class T2>
void sc_concref<T1,T2>::scan(std::istream& is)
{
}

// Disabled
template <class T1, class T2>
sc_concref<T1,T2>::sc_concref()
{
}

// r-value concatenation operators and functions
template <typename C1, typename C2> sc_concref_r<C1,C2> operator , (C1, C2)
{
}

template <typename C1, typename C2> sc_concref_r<C1,C2> concat(C1, C2)
{
}

// l-value concatenation operators and functions
template <typename C1, typename C2> sc_concref<C1,C2> operator , (C1, C2)
{
}

template <typename C1, typename C2> sc_concref<C1,C2> concat(C1, C2)
{
}

} // end of namespace sc_dt

#endif
