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

#ifndef __LIBIEEE1666_DATA_TYPES_FIXED_POINT_FXVAL_H__
#define __LIBIEEE1666_DATA_TYPES_FIXED_POINT_FXVAL_H__

#include <data_types/fwd.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_fxval
{
public:
	// Constructors and destructor
	sc_fxval();
	explicit sc_fxval(int);
	explicit sc_fxval(unsigned int);
	explicit sc_fxval(long);
	explicit sc_fxval(unsigned long);
	explicit sc_fxval(float);
	explicit sc_fxval(double);
	explicit sc_fxval(const char*);
	sc_fxval(const sc_fxval&);
	sc_fxval(const sc_fxval_fast&);
	sc_fxval(const sc_fxnum&);
	sc_fxval(const sc_fxnum_fast&);
	explicit sc_fxval(int64);
	explicit sc_fxval(uint64);
	explicit sc_fxval(const sc_int_base&);
	explicit sc_fxval(const sc_uint_base&);
	explicit sc_fxval(const sc_signed&);
	explicit sc_fxval(const sc_unsigned&);
	~sc_fxval();

	// Unary operators
	const sc_fxval operator - () const;
	const sc_fxval& operator + () const;
	friend void neg(sc_fxval&, const sc_fxval&);

	// Binary operators
#define DECL_BIN_OP_T(op, tp) \
	friend const sc_fxval operator op (const sc_fxval&, tp); \
	friend const sc_fxval operator op (tp, const sc_fxval&);

#define DECL_BIN_OP_OTHER(op) \
	DECL_BIN_OP_T(op, int64) \
	DECL_BIN_OP_T(op, uint64) \
	DECL_BIN_OP_T(op, const sc_int_base&) \
	DECL_BIN_OP_T(op, const sc_uint_base&) \
	DECL_BIN_OP_T(op, const sc_signed&) \
	DECL_BIN_OP_T(op, const sc_unsigned&)

#define DECL_BIN_OP(op, dummy) \
	friend const sc_fxval operator op (const sc_fxval&, const sc_fxval&); \
	DECL_BIN_OP_T(op, int) \
	DECL_BIN_OP_T(op, unsigned int) \
	DECL_BIN_OP_T(op, long) \
	DECL_BIN_OP_T(op, unsigned long) \
	DECL_BIN_OP_T(op, float) \
	DECL_BIN_OP_T(op, double) \
	DECL_BIN_OP_T(op, const char *) \
	DECL_BIN_OP_T(op, const sc_fxval_fast&) \
	DECL_BIN_OP_T(op, const sc_fxnum_fast&) \
	DECL_BIN_OP_OTHER(op)

	DECL_BIN_OP(*, mult)
	DECL_BIN_OP(+, add)
	DECL_BIN_OP(-, sub)
	DECL_BIN_OP(/, div)

#undef DECL_BIN_OP_T
#undef DECL_BIN_OP_OTHER
#undef DECL_BIN_OP
	
	friend const sc_fxval operator << (const sc_fxval&, int);
	friend const sc_fxval operator >> (const sc_fxval&, int);

	// Relational (including equality) operators
#define DECL_REL_OP_T(op, tp) \
	friend bool operator op (const sc_fxval&, tp); \
	friend bool operator op (tp, const sc_fxval&);

#define DECL_REL_OP_OTHER(op) \
	DECL_REL_OP_T(op, int64) \
	DECL_REL_OP_T(op, uint64) \
	DECL_REL_OP_T(op, const sc_int_base&) \
	DECL_REL_OP_T(op, const sc_uint_base&) \
	DECL_REL_OP_T(op, const sc_signed&) \
	DECL_REL_OP_T(op, const sc_unsigned&)

#define DECL_REL_OP(op) \
	friend bool operator op (const sc_fxval&, const sc_fxval&); \
	DECL_REL_OP_T(op, int) \
	DECL_REL_OP_T(op, unsigned int) \
	DECL_REL_OP_T(op, long) \
	DECL_REL_OP_T(op, unsigned long) \
	DECL_REL_OP_T(op, float) \
	DECL_REL_OP_T(op, double) \
	DECL_REL_OP_T(op, const char *) \
	DECL_REL_OP_T(op, const sc_fxval_fast&) \
	DECL_REL_OP_T(op, const sc_fxnum_fast&) \
	DECL_REL_OP_OTHER(op)

	DECL_REL_OP(<)
	DECL_REL_OP(<=)
	DECL_REL_OP(>)
	DECL_REL_OP(>=)
	DECL_REL_OP(==)
	DECL_REL_OP(!=)

#undef DECL_REL_OP_T
#undef DECL_REL_OP_OTHER
#undef DECL_REL_OP
	
	// Assignment operators
#define DECL_ASN_OP_T(op, tp) \
	sc_fxval& operator op(tp);

#define DECL_ASN_OP_OTHER(op) \
	DECL_ASN_OP_T(op, int64) \
	DECL_ASN_OP_T(op, uint64) \
	DECL_ASN_OP_T(op, const sc_int_base&) \
	DECL_ASN_OP_T(op, const sc_uint_base&) \
	DECL_ASN_OP_T(op, const sc_signed&) \
	DECL_ASN_OP_T(op, const sc_unsigned&)

#define DECL_ASN_OP(op) \
	DECL_ASN_OP_T(op, int) \
	DECL_ASN_OP_T(op, unsigned int) \
	DECL_ASN_OP_T(op, long) \
	DECL_ASN_OP_T(op, unsigned long) \
	DECL_ASN_OP_T(op, float) \
	DECL_ASN_OP_T(op, double) \
	DECL_ASN_OP_T(op, const char *) \
	DECL_ASN_OP_T(op, const sc_fxval&) \
	DECL_ASN_OP_T(op, const sc_fxval_fast&) \
	DECL_ASN_OP_T(op, const sc_fxnum&) \
	DECL_ASN_OP_T(op, const sc_fxnum_fast&) \
	DECL_ASN_OP_OTHER(op)

	DECL_ASN_OP(=)
	DECL_ASN_OP(*=)
	DECL_ASN_OP(/=)
	DECL_ASN_OP(+=)
	DECL_ASN_OP(-=)
	DECL_ASN_OP_T(<<=, int)
	DECL_ASN_OP_T(>>=, int)

#undef DECL_ASN_OP_T
#undef DECL_ASN_OP_OTHER
#undef DECL_ASN_OP
	
	// Auto-increment and auto-decrement
	const sc_fxval operator ++ (int);
	const sc_fxval operator -- (int);
	sc_fxval& operator ++ ();
	sc_fxval& operator -- ();

	// Implicit conversion
	operator double() const;

	// Explicit conversion to primitive types
	short to_short() const;
	unsigned short to_ushort() const;
	int to_int() const;
	unsigned int to_uint() const;
	long to_long() const;
	unsigned long to_ulong() const;
	int64 to_int64() const;
	uint64 to_uint64() const;
	float to_float() const;
	double to_double() const;

	// Explicit conversion to character string
	const std::string to_string() const;
	const std::string to_string(sc_numrep) const;
	const std::string to_string(sc_numrep, bool) const;
	const std::string to_string(sc_fmt) const;
	const std::string to_string(sc_numrep, sc_fmt) const;
	const std::string to_string(sc_numrep, bool, sc_fmt) const;
	const std::string to_dec() const;
	const std::string to_bin() const;
	const std::string to_oct() const;
	const std::string to_hex() const;

	// Methods
	bool is_neg() const;
	bool is_zero() const;
	void print(std::ostream& = std::cout) const;
	void scan(std::istream& = std::cin);
	void dump(std::ostream& = std::cout) const;
};

} // end of namespace sc_dt

#endif
