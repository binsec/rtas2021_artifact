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

#ifndef __LIBIEEE1666_DATA_TYPES_CONTEXT_FXTYPE_PARAMS_H__
#define __LIBIEEE1666_DATA_TYPES_CONTEXT_FXTYPE_PARAMS_H__

#include <data_types/fwd.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_fxtype_params
{
public:
	// Constructors and destructor
	sc_fxtype_params();
	sc_fxtype_params(int, int);
	sc_fxtype_params(sc_q_mode, sc_o_mode, int = 0);
	sc_fxtype_params(int, int, sc_q_mode, sc_o_mode, int = 0);
	sc_fxtype_params(const sc_fxtype_params&);
	sc_fxtype_params(const sc_fxtype_params&, int, int);
	sc_fxtype_params(const sc_fxtype_params&, sc_q_mode, sc_o_mode, int = 0);

	// Operators
	sc_fxtype_params& operator = (const sc_fxtype_params&);
	friend bool operator == (const sc_fxtype_params&, const sc_fxtype_params&);
	friend bool operator != (const sc_fxtype_params&, const sc_fxtype_params&);

	// Methods
	int wl() const;
	void wl(int);
	int iwl() const;
	void iwl(int);
	sc_q_mode q_mode() const;
	void q_mode(sc_q_mode);
	sc_o_mode o_mode() const;
	void o_mode(sc_o_mode);
	int n_bits() const;
	void n_bits(int);
	const std::string to_string() const;
	void print(std::ostream& = std::cout) const;
	void dump(std::ostream& = std::cout) const;
};

} // end of namespace sc_dt

#endif
