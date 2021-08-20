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

#include <data_types/integer/int_subref_r.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
sc_int_subref_r::sc_int_subref_r(const sc_int_subref_r& a)
{
}

// Destructor
sc_int_subref_r::~sc_int_subref_r()
{
}

// Capacity
int sc_int_subref_r::length() const
{
}

// Reduce methods
bool sc_int_subref_r::and_reduce() const
{
}

bool sc_int_subref_r::nand_reduce() const
{
}

bool sc_int_subref_r::or_reduce() const
{
}

bool sc_int_subref_r::nor_reduce() const
{
}

bool sc_int_subref_r::xor_reduce() const
{
}

bool sc_int_subref_r::xnor_reduce() const
{
}

// Implicit conversion to uint_type
sc_int_subref_r::operator uint_type() const
{
}

// Explicit conversions
int sc_int_subref_r::to_int() const
{
}

unsigned int sc_int_subref_r::to_uint() const
{
}

long sc_int_subref_r::to_long() const
{
}

unsigned long sc_int_subref_r::to_ulong() const
{
}

int64 sc_int_subref_r::to_int64() const
{
}

uint64 sc_int_subref_r::to_uint64() const
{
}

double sc_int_subref_r::to_double() const
{
}

// Explicit conversion to character string
const std::string sc_int_subref_r::to_string(sc_numrep numrep) const
{
}

const std::string sc_int_subref_r::to_string(sc_numrep numrep, bool w_prefix) const
{
}

// Other methods
void sc_int_subref_r::print(std::ostream& os) const
{
}

sc_int_subref_r::sc_int_subref_r()
{
}

// Disabled
sc_int_subref_r& sc_int_subref_r::operator = (const sc_int_subref_r&)
{
}

} // end of namespace sc_dt
