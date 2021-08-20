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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_CONCATREF_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_CONCATREF_H__

#include <data_types/fwd.h>
#include <data_types/integer/value_base.h>
#include <data_types/integer/generic_base.h>
#include <iostream>

namespace sc_dt {

class sc_concatref : public sc_generic_base<sc_concatref>, public sc_value_base 
{
public:
	// Destructor
	virtual ~sc_concatref();

	// Capacity
	unsigned int length() const;

	// Explicit conversions
	int to_int() const;
	unsigned int to_uint() const;
	long to_long() const;
	unsigned long to_ulong() const;
	int64 to_int64() const;
	uint64 to_uint64() const;
	double to_double() const;
	void to_sc_signed(sc_signed& target) const;
	void to_sc_unsigned(sc_unsigned& target) const;

	// Implicit conversions
	operator uint64() const;
	operator const sc_unsigned&() const;

	// Unary operators
	sc_unsigned operator + () const;
	sc_unsigned operator - () const;
	sc_unsigned operator ~ () const;

	// Explicit conversion to character string
	const std::string to_string(sc_numrep numrep = SC_DEC) const;
	const std::string to_string(sc_numrep numrep, bool w_prefix) const;

	// Assignment operators
	const sc_concatref& operator = (int v);
	const sc_concatref& operator = (unsigned int v);
	const sc_concatref& operator = (long v);
	const sc_concatref& operator = (unsigned long v);
	const sc_concatref& operator = (int64 v);
	const sc_concatref& operator = (uint64 v);
	const sc_concatref& operator = (const sc_concatref& v);
	const sc_concatref& operator = (const sc_signed& v);
	const sc_concatref& operator = (const sc_unsigned& v);
	const sc_concatref& operator = (const char *v_p);
	const sc_concatref& operator = (const sc_bv_base& v);
	const sc_concatref& operator = (const sc_lv_base& v);

	// Reduce methods
	bool and_reduce() const;
	bool nand_reduce() const;
	bool or_reduce() const;
	bool nor_reduce() const;
	bool xor_reduce() const;
	bool xnor_reduce() const;

	// Other methods
	void print(std::ostream& os = std::cout) const;
	void scan(std::istream& is);

private:
	sc_concatref(const sc_concatref&);
	sc_concatref();
};

sc_concatref& concat(sc_value_base & a, sc_value_base& b);
const sc_concatref& concat(const sc_value_base& a, const sc_value_base& b);
const sc_concatref& concat(const sc_value_base& a, bool b);
const sc_concatref& concat(bool a, const sc_value_base& b);
sc_concatref& operator , (sc_value_base& a, sc_value_base& b);
const sc_concatref& operator , (const sc_value_base& a, const sc_value_base& b);
const sc_concatref& operator , (const sc_value_base& a, bool b);
const sc_concatref& operator , (bool a, const sc_value_base& b);

} // end of namespace sc_dt

#endif
