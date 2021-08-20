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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_BITREF_R_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_BITREF_R_H__

#include <data_types/fwd.h>
#include <data_types/bit/logic.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <class T>
class sc_bitref_r
{
	friend class sc_bv_base;
	friend class sc_lv_base;

public:
	// Copy constructor
	sc_bitref_r(const sc_bitref_r<T>& a);

	// Bitwise complement
	const sc_logic operator ~ () const;

	// Implicit conversion to sc_logic
	operator const sc_logic() const;

	// Explicit conversions
	bool is_01() const;
	bool to_bool() const;
	char to_char() const;

	// Common methods
	int length() const;

	// Other methods
	void print(std::ostream& os = std::cout) const;

private:
	// Disabled
	sc_bitref_r();
	sc_bitref_r<T>& operator = (const sc_bitref_r<T>&);
};

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
template <class T>
sc_bitref_r<T>::sc_bitref_r(const sc_bitref_r<T>& a)
{
}

// Bitwise complement
template <class T>
const sc_logic sc_bitref_r<T>::operator ~ () const
{
}

// Implicit conversion to sc_logic
template <class T>
sc_bitref_r<T>::operator const sc_logic() const
{
}

// Explicit conversions
template <class T>
bool sc_bitref_r<T>::is_01() const
{
}
template <class T>
bool sc_bitref_r<T>::to_bool() const
{
}
template <class T>
char sc_bitref_r<T>::to_char() const
{
}

// Common methods
template <class T>
int sc_bitref_r<T>::length() const
{
}

// Other methods
template <class T>
void sc_bitref_r<T>::print(std::ostream& os) const
{
}

// Disabled
template <class T>
sc_bitref_r<T>::sc_bitref_r()
{
}
template <class T>
sc_bitref_r<T>& sc_bitref_r<T>::operator = (const sc_bitref_r<T>&)
{
}

} // end of namespace sc_dt

#endif
