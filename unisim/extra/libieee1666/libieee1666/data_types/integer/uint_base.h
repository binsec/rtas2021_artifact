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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_UINT_BASE_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_UINT_BASE_H__

#include <data_types/fwd.h>
#include <data_types/context/length_param.h>
#include <data_types/integer/value_base.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_uint_base : public sc_value_base
{
	friend class sc_uint_bitref_r;
	friend class sc_uint_bitref;
	friend class sc_uint_subref_r;
	friend class sc_uint_subref;
	
public:
	// Constructors
	explicit sc_uint_base(int w = sc_length_param().len());
	sc_uint_base(uint_type v, int w);
	sc_uint_base(const sc_uint_base& a);
	explicit sc_uint_base(const sc_uint_subref_r& a);
	template <class T> explicit sc_uint_base(const sc_generic_base<T>& a);
	explicit sc_uint_base(const sc_bv_base& v);
	explicit sc_uint_base(const sc_lv_base& v);
	explicit sc_uint_base(const sc_int_subref_r& v);
	explicit sc_uint_base(const sc_signed_subref_r& v);
	explicit sc_uint_base(const sc_unsigned_subref_r& v);
	explicit sc_uint_base(const sc_signed& a);
	explicit sc_uint_base(const sc_unsigned& a);
	
	// Destructor
	~sc_uint_base();
	
	// Assignment operators
	sc_uint_base& operator = (uint_type v);
	sc_uint_base& operator = (const sc_uint_base& a);
	sc_uint_base& operator = (const sc_uint_subref_r& a);
	template <class T> sc_uint_base& operator = (const sc_generic_base<T>& a);
	sc_uint_base& operator = (const sc_signed& a);
	sc_uint_base& operator = (const sc_unsigned& a);
	sc_uint_base& operator = (const sc_fxval& a);
	sc_uint_base& operator = (const sc_fxval_fast& a);
	sc_uint_base& operator = (const sc_fxnum& a);
	sc_uint_base& operator = (const sc_fxnum_fast& a);
	sc_uint_base& operator = (const sc_bv_base& a);
	sc_uint_base& operator = (const sc_lv_base& a);
	sc_uint_base& operator = (const char *a);
	sc_uint_base& operator = (unsigned long a);
	sc_uint_base& operator = (long a);
	sc_uint_base& operator = (unsigned int a);
	sc_uint_base& operator = (int a);
	sc_uint_base& operator = (int64 a);
	sc_uint_base& operator = (double a);
	
	// Prefix and postfix increment and decrement operators
	sc_uint_base& operator ++ ();         // Prefix
	const sc_uint_base operator ++ (int); // Postfix
	sc_uint_base& operator -- ();         // Prefix
	const sc_uint_base operator -- (int); // Postfix
	
	// Bit selection
	sc_uint_bitref operator [] (int i);
	sc_uint_bitref_r operator [] (int i) const;
	
	// Part selection
	sc_uint_subref operator () (int left, int right);
	sc_uint_subref_r operator () (int left, int right) const;
	sc_uint_subref range(int left, int right);
	sc_uint_subref_r range(int left, int right) const;
	
	// Capacity
	int length() const;
	
	// Reduce methods
	bool and_reduce() const;
	bool nand_reduce() const;
	bool or_reduce() const;
	bool nor_reduce() const;
	bool xor_reduce() const;
	bool xnor_reduce() const;
	
	// Implicit conversion to uint_type
	operator uint_type() const;
	
	// Explicit conversions
	int to_int() const;
	unsigned int to_uint() const;
	long to_long() const;
	unsigned long to_ulong() const;
	int64 to_int64() const;
	uint64 to_uint64() const;
	double to_double() const;
	
	// Explicit conversion to character string
	const std::string to_string(sc_numrep numrep = SC_DEC) const;
	const std::string to_string(sc_numrep numrep, bool w_prefix) const;
	
	// Other methods
	void print(std::ostream& os = std::cout) const;
	void scan(std::istream& is = std::cin);
};

///////////////////////////////// definition //////////////////////////////////

template <class T> sc_uint_base& sc_uint_base::operator = (const sc_generic_base<T>& a)
{
}

} // end of namespace sc_dt

#endif
