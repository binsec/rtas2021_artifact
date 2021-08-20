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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_SUBREF_R_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_SUBREF_R_H__

#include <data_types/fwd.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

template <class T>
class sc_subref_r
{
public:
	// Copy constructor
	sc_subref_r(const sc_subref_r<T>& a);

	// Bit selection
	sc_bitref_r<sc_subref_r<T> > operator [] (int i) const;

	// Part selection
	sc_subref_r<sc_subref_r<T> > operator () (int hi, int lo) const;
	sc_subref_r<sc_subref_r<T> > range(int hi, int lo) const;

	// Reduce functions
	sc_logic_value_t and_reduce() const;
	sc_logic_value_t nand_reduce() const;
	sc_logic_value_t or_reduce() const;
	sc_logic_value_t nor_reduce() const;
	sc_logic_value_t xor_reduce() const;
	sc_logic_value_t xnor_reduce() const;

	// Common methods
	int length() const;

	// Explicit conversions to character string
	const std::string to_string() const;
	const std::string to_string(sc_numrep) const;
	const std::string to_string(sc_numrep, bool) const;

	// Explicit conversions
	int to_int() const;
	unsigned int to_uint() const;
	long to_long() const;
	unsigned long to_ulong() const;
	int64 to_int64() const;
	uint64 to_uint64() const;
	bool is_01() const;

	// Other methods
	void print(std::ostream& os = std::cout) const;
	bool reversed() const;

private:
	// Disabled
	sc_subref_r();
	sc_subref_r<T>& operator = (const sc_subref_r<T>&);
};

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
template <class T>
sc_subref_r<T>::sc_subref_r(const sc_subref_r<T>& a)
{
}

// Bit selection
template <class T>
sc_bitref_r<sc_subref_r<T> > sc_subref_r<T>::operator [] (int i) const
{
}

// Part selection
template <class T>
sc_subref_r<sc_subref_r<T> > sc_subref_r<T>::operator () (int hi, int lo) const
{
}

template <class T>
sc_subref_r<sc_subref_r<T> > sc_subref_r<T>::range(int hi, int lo) const
{
}

// Reduce functions
template <class T>
sc_logic_value_t sc_subref_r<T>::and_reduce() const
{
}

template <class T>
sc_logic_value_t sc_subref_r<T>::nand_reduce() const
{
}

template <class T>
sc_logic_value_t sc_subref_r<T>::or_reduce() const
{
}

template <class T>
sc_logic_value_t sc_subref_r<T>::nor_reduce() const
{
}

template <class T>
sc_logic_value_t sc_subref_r<T>::xor_reduce() const
{
}

template <class T>
sc_logic_value_t sc_subref_r<T>::xnor_reduce() const
{
}

// Common methods
template <class T>
int sc_subref_r<T>::length() const
{
}

// Explicit conversions to character string
template <class T>
const std::string sc_subref_r<T>::to_string() const
{
}

template <class T>
const std::string sc_subref_r<T>::to_string(sc_numrep) const
{
}

template <class T>
const std::string sc_subref_r<T>::to_string(sc_numrep, bool) const
{
}

// Explicit conversions

template <class T>
int sc_subref_r<T>::to_int() const
{
}

template <class T>
unsigned int sc_subref_r<T>::to_uint() const
{
}

template <class T>
long sc_subref_r<T>::to_long() const
{
}

template <class T>
unsigned long sc_subref_r<T>::to_ulong() const
{
}

template <class T>
int64 sc_subref_r<T>::to_int64() const
{
}

template <class T>
uint64 sc_subref_r<T>::to_uint64() const
{
}

template <class T>
bool sc_subref_r<T>::is_01() const
{
}

// Other methods
template <class T>
void sc_subref_r<T>::print(std::ostream& os) const
{
}

template <class T>
bool sc_subref_r<T>::reversed() const
{
}

// Disabled
template <class T>
sc_subref_r<T>::sc_subref_r()
{
}

template <class T>
sc_subref_r<T>& sc_subref_r<T>::operator = (const sc_subref_r<T>&)
{
}

} // end of namespace sc_dt

#endif
