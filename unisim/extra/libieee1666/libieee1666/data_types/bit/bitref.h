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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_BITREF_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_BITREF_H__

#include <data_types/fwd.h>
#include <data_types/bit/bitref_r.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <class T>
class sc_bitref : public sc_bitref_r<T>
{
	friend class sc_bv_base;
	friend class sc_lv_base;

public:
	// Copy constructor
	sc_bitref(const sc_bitref<T>& a);

	// Assignment operators
	sc_bitref<T>& operator = (const sc_bitref_r<T>& a);
	sc_bitref<T>& operator = (const sc_bitref<T>& a);
	sc_bitref<T>& operator = (const sc_logic& a);
	sc_bitref<T>& operator = (sc_logic_value_t v);
	sc_bitref<T>& operator = (bool a);
	sc_bitref<T>& operator = (char a);
	sc_bitref<T>& operator = (int a);

	// Bitwise assignment operators
	sc_bitref<T>& operator &= (const sc_bitref_r<T>& a);
	sc_bitref<T>& operator &= (const sc_logic& a);
	sc_bitref<T>& operator &= (sc_logic_value_t v);
	sc_bitref<T>& operator &= (bool a);
	sc_bitref<T>& operator &= (char a);
	sc_bitref<T>& operator &= (int a);
	sc_bitref<T>& operator |= (const sc_bitref_r<T>& a);
	sc_bitref<T>& operator |= (const sc_logic& a);
	sc_bitref<T>& operator |= (sc_logic_value_t v);
	sc_bitref<T>& operator |= (bool a);
	sc_bitref<T>& operator |= (char a);
	sc_bitref<T>& operator |= (int a);
	sc_bitref<T>& operator ^= (const sc_bitref_r<T>& a);
	sc_bitref<T>& operator ^= (const sc_logic& a);
	sc_bitref<T>& operator ^= (sc_logic_value_t v);
	sc_bitref<T>& operator ^= (bool a);
	sc_bitref<T>& operator ^= (char a);
	sc_bitref<T>& operator ^= (int a);

	// Other methods
	void scan(std::istream& is = std::cin);

private:
	// Disabled
	sc_bitref();
};

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
template <class T>
sc_bitref<T>::sc_bitref(const sc_bitref<T>& a)
{
}

// Assignment operators
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (const sc_bitref_r<T>& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (const sc_bitref<T>& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (const sc_logic& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (sc_logic_value_t v)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (bool a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (char a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator = (int a)
{
}

// Bitwise assignment operators
template <class T>
sc_bitref<T>& sc_bitref<T>::operator &= (const sc_bitref_r<T>& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator &= (const sc_logic& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator &= (sc_logic_value_t v)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator &= (bool a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator &= (char a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator &= (int a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator |= (const sc_bitref_r<T>& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator |= (const sc_logic& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator |= (sc_logic_value_t v)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator |= (bool a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator |= (char a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator |= (int a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator ^= (const sc_bitref_r<T>& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator ^= (const sc_logic& a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator ^= (sc_logic_value_t v)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator ^= (bool a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator ^= (char a)
{
}
template <class T>
sc_bitref<T>& sc_bitref<T>::operator ^= (int a)
{
}

// Other methods
template <class T>
void sc_bitref<T>::scan(std::istream& is)
{
}

// Disabled
template <class T>
sc_bitref<T>::sc_bitref()
{
}

} // end of namespace sc_dt

#endif
