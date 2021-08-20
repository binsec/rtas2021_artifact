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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_VALUE_BASE_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_VALUE_BASE_H__

#include <data_types/fwd.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_value_base
{
	friend class sc_concatref;
	
private:
	virtual void concat_clear_data(bool to_ones = false);
	virtual bool concat_get_ctrl(/*implementation-defined*/ unsigned int *dst_p, int low_i) const;
	virtual bool concat_get_data(/*implementation-defined*/ unsigned int *dst_p, int low_i) const;
	virtual uint64 concat_get_uint64() const;
	virtual int concat_length(bool *xz_present_p = 0) const;
	virtual void concat_set(int64 src, int low_i);
	virtual void concat_set(const sc_signed& src, int low_i);
	virtual void concat_set(const sc_unsigned& src, int low_i);
	virtual void concat_set(uint64 src, int low_i);
};

} // end of namespace sc_dt

#endif
