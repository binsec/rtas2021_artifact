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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_LV_BASE_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_LV_BASE_H__

#include <data_types/fwd.h>
#include <data_types/context/length_param.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_lv_base
{
	friend class sc_bv_base;

public:
	// Constructors
	explicit sc_lv_base(int length_ = sc_length_param().len());
	explicit sc_lv_base(const sc_logic& a, int length_ = sc_length_param().len());
	sc_lv_base(const char *a);
	sc_lv_base(const char *a, int length_);
	template <class X> sc_lv_base(const sc_subref_r<X>& a);
	template <class T1, class T2> sc_lv_base(const sc_concref_r<T1,T2>& a);
	sc_lv_base(const sc_bv_base& a);
	sc_lv_base(const sc_lv_base& a);

	// Destructor
	virtual ~sc_lv_base();

	// Assignment operators
	template <class X> sc_lv_base& operator = (const sc_subref_r<X>& a);
	template <class T1, class T2> sc_lv_base& operator = (const sc_concref_r<T1,T2>& a);
	sc_lv_base& operator = (const sc_bv_base& a);
	sc_lv_base& operator = (const sc_lv_base& a);
	sc_lv_base& operator = (const char *a);
	sc_lv_base& operator = (const bool *a);
	sc_lv_base& operator = (const sc_logic *a);
	sc_lv_base& operator = (const sc_unsigned& a);
	sc_lv_base& operator = (const sc_signed& a);
	sc_lv_base& operator = (const sc_uint_base& a);
	sc_lv_base& operator = (const sc_int_base& a);
	sc_lv_base& operator = (unsigned long a);
	sc_lv_base& operator = (long a);
	sc_lv_base& operator = (unsigned int a);
	sc_lv_base& operator = (int a);
	sc_lv_base& operator = (uint64 a);
	sc_lv_base& operator = (int64 a);

	// Bitwise rotations
	sc_lv_base& lrotate(int n);
	sc_lv_base& rrotate(int n);

	// Bitwise reverse
	sc_lv_base& reverse();

	// Bit selection
	sc_bitref<sc_bv_base> operator [] (int i);
	sc_bitref_r<sc_bv_base> operator [] (int i) const;

	// Part selection
	sc_subref<sc_lv_base> operator () (int hi, int lo);
	sc_subref_r<sc_lv_base> operator () (int hi, int lo) const;
	sc_subref<sc_lv_base> range(int hi, int lo);
	sc_subref_r<sc_lv_base> range(int hi, int lo) const;

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
	void scan(std::istream& is = std::cin);
};

///////////////////////////////// definition //////////////////////////////////

// Constructors
template <class X> sc_lv_base::sc_lv_base(const sc_subref_r<X>& a)
{
}

template <class T1, class T2> sc_lv_base::sc_lv_base(const sc_concref_r<T1,T2>& a)
{
}

// Assignment operators
template <class X> sc_lv_base& sc_lv_base::operator = (const sc_subref_r<X>& a)
{
}

template <class T1, class T2> sc_lv_base& sc_lv_base::operator = (const sc_concref_r<T1,T2>& a)
{
}

} // end of namespace sc_dt

#endif
