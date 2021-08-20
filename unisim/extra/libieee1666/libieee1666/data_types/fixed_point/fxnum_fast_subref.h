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

#ifndef __LIBIEEE1666_DATA_TYPES_FIXED_POINT_FXNUM_FAST_SUBREF_H__
#define __LIBIEEE1666_DATA_TYPES_FIXED_POINT_FXNUM_FAST_SUBREF_H__

#include <data_types/fwd.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_fxnum_fast_subref
{
	friend class sc_fxnum_fast;
	friend class sc_fxnum_subref;

public:
	// Copy constructor
	sc_fxnum_fast_subref(const sc_fxnum_fast_subref&);

	// Destructor
	~sc_fxnum_fast_subref();

	// Assignment operators
#define DECL_ASN_OP_T(tp) \
	sc_fxnum_fast_subref& operator = (tp);

	DECL_ASN_OP_T(const sc_fxnum_subref&)
	DECL_ASN_OP_T(const sc_fxnum_fast_subref&)
	DECL_ASN_OP_T(const sc_bv_base&)
	DECL_ASN_OP_T(const sc_lv_base&)
	DECL_ASN_OP_T(const char *)
	DECL_ASN_OP_T(const bool *)
	DECL_ASN_OP_T(const sc_signed&)
	DECL_ASN_OP_T(const sc_unsigned&)
	DECL_ASN_OP_T(const sc_int_base&)
	DECL_ASN_OP_T(const sc_uint_base&)
	DECL_ASN_OP_T(int64)
	DECL_ASN_OP_T(uint64)
	DECL_ASN_OP_T(int)
	DECL_ASN_OP_T(unsigned int)
	DECL_ASN_OP_T(long)
	DECL_ASN_OP_T(unsigned long)
	DECL_ASN_OP_T(char)

#undef DECL_ASN_OP_T
	
#define DECL_ASN_OP_T_A(op, tp) \
	sc_fxnum_fast_subref& operator op ## = (tp);

#define DECL_ASN_OP_A(op) \
	DECL_ASN_OP_T_A(op, const sc_fxnum_subref &) \
	DECL_ASN_OP_T_A(op, const sc_fxnum_fast_subref&) \
	DECL_ASN_OP_T_A(op, const sc_bv_base&) \
	DECL_ASN_OP_T_A(op, const sc_lv_base&)
	
	DECL_ASN_OP_A(&)
	DECL_ASN_OP_A(|)
	DECL_ASN_OP_A(^)

#undef DECL_ASN_OP_T_A
#undef DECL_ASN_OP_A

	// Relational operators
#define DECL_REL_OP_T(op, tp) \
	friend bool operator op (const sc_fxnum_fast_subref&, tp); \
	friend bool operator op (tp, const sc_fxnum_fast_subref&);

#define DECL_REL_OP(op) \
	friend bool operator op (const sc_fxnum_fast_subref&, const sc_fxnum_fast_subref&); \
	friend bool operator op (const sc_fxnum_fast_subref&, const sc_fxnum_subref&); \
	DECL_REL_OP_T(op, const sc_bv_base&) \
	DECL_REL_OP_T(op, const sc_lv_base&) \
	DECL_REL_OP_T(op, const char *) \
	DECL_REL_OP_T(op, const bool *) \
	DECL_REL_OP_T(op, const sc_signed&) \
	DECL_REL_OP_T(op, const sc_unsigned&) \
	DECL_REL_OP_T(op, int) \
	DECL_REL_OP_T(op, unsigned int) \
	DECL_REL_OP_T(op, long) \
	DECL_REL_OP_T(op, unsigned long)

	DECL_REL_OP(==)
	DECL_REL_OP(!=)

#undef DECL_REL_OP_T
#undef DECL_REL_OP

	// Reduce functions
	bool and_reduce() const;
	bool nand_reduce() const;
	bool or_reduce() const;
	bool nor_reduce() const;
	bool xor_reduce() const;
	bool xnor_reduce() const;

	// Query parameter
	int length() const;

	// Explicit conversions
	int to_int() const;
	unsigned int to_uint() const;
	long to_long() const;
	unsigned long to_ulong() const;
	int64 to_int64() const;
	uint64 to_uint64() const;
	const std::string to_string() const;
	const std::string to_string(sc_numrep) const;
	const std::string to_string(sc_numrep, bool) const;

	// Implicit conversion
	operator sc_bv_base() const;

	// Print or dump content
	void print(std::ostream& = std::cout) const;
	void scan(std::istream& = std::cin);
	void dump(std::ostream& = std::cout) const;

private:
	// Disabled
	// Constructor
	sc_fxnum_fast_subref(sc_fxnum_fast&, int, int);
	sc_fxnum_fast_subref();
};

} // end of namespace sc_dt

#endif
