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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_UINT_SUBREF_R_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_UINT_SUBREF_R_H__

#include <data_types/fwd.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_uint_subref_r
{
	friend class sc_uint_base;
	friend class sc_uint_subref;

public:
	// Copy constructor
	sc_uint_subref_r(const sc_uint_subref_r& a);

	// Destructor
	virtual ~sc_uint_subref_r();

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

protected:
	sc_uint_subref_r();

private:
	// Disabled
	sc_uint_subref_r& operator = (const sc_uint_subref_r&);
};

} // end of namespace sc_dt

#endif
