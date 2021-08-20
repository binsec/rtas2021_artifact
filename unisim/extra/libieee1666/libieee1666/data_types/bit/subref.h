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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_SUBREF_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_SUBREF_H__

#include <data_types/fwd.h>
#include <data_types/bit/subref_r.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <class T>
class sc_subref : public sc_subref_r<T>
{
public:
	// Copy constructor
	sc_subref(const sc_subref<T>& a);

	// Assignment operators
	template <class T1> sc_subref<T>& operator = (const sc_subref_r<T1>& a);
	template <class T1, class T2> sc_subref<T>& operator = (const sc_concref_r<T1,T2>& a);
	sc_subref<T>& operator = (const sc_bv_base& a);
	sc_subref<T>& operator = (const sc_lv_base& a);
	sc_subref<T>& operator = (const sc_subref_r<T>& a);
	sc_subref<T>& operator = (const sc_subref<T>& a);
	sc_subref<T>& operator = (const char *a);
	sc_subref<T>& operator = (const bool *a);
	sc_subref<T>& operator = (const sc_logic *a);
	sc_subref<T>& operator = (const sc_unsigned& a);
	sc_subref<T>& operator = (const sc_signed& a);
	sc_subref<T>& operator = (const sc_uint_base& a);
	sc_subref<T>& operator = (const sc_int_base& a);
	sc_subref<T>& operator = (unsigned long a);
	sc_subref<T>& operator = (long a);
	sc_subref<T>& operator = (unsigned int a);
	sc_subref<T>& operator = (int a);
	sc_subref<T>& operator = (uint64 a);
	sc_subref<T>& operator = (int64 a);

	// Bitwise rotations
	sc_subref<T>& lrotate(int n);
	sc_subref<T>& rrotate(int n);

	// Bitwise reverse
	sc_subref<T>& reverse();

	// Bit selection
	sc_bitref<sc_subref<T> > operator [] (int i);

	// Part selection
	sc_subref<sc_subref<T> > operator () (int hi, int lo);
	sc_subref<sc_subref<T> > range(int hi, int lo);

	// Other methods
	void scan(std::istream& = std::cin);

private:
	// Disabled
	sc_subref();
};

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
template <class T>
sc_subref<T>::sc_subref(const sc_subref<T>& a)
{
}

// Assignment operators
template <class T>
template <class T1> sc_subref<T>& sc_subref<T>::operator = (const sc_subref_r<T1>& a)
{
}

template <class T>
template <class T1, class T2> sc_subref<T>& sc_subref<T>::operator = (const sc_concref_r<T1,T2>& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_bv_base& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_lv_base& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_subref_r<T>& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_subref<T>& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const char *a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const bool *a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_logic *a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_unsigned& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_signed& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_uint_base& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (const sc_int_base& a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (unsigned long a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (long a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (unsigned int a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (int a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (uint64 a)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::operator = (int64 a)
{
}

// Bitwise rotations
template <class T>
sc_subref<T>& sc_subref<T>::lrotate(int n)
{
}

template <class T>
sc_subref<T>& sc_subref<T>::rrotate(int n)
{
}

// Bitwise reverse
template <class T>
sc_subref<T>& sc_subref<T>::reverse()
{
}

// Bit selection
template <class T>
sc_bitref<sc_subref<T> > sc_subref<T>::operator [] (int i)
{
}

// Part selection
template <class T>
sc_subref<sc_subref<T> > sc_subref<T>::operator () (int hi, int lo)
{
}

template <class T>
sc_subref<sc_subref<T> > sc_subref<T>::range(int hi, int lo)
{
}

// Other methods
template <class T>
void sc_subref<T>::scan(std::istream& is)
{
}

// Disabled
template <class T>
sc_subref<T>::sc_subref()
{
}

} // end of namespace sc_dt

#endif
