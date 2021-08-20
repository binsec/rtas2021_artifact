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

#include <data_types/integer/concatref.h>
#include <data_types/context/length_param.h>
#include <data_types/integer/unsigned.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Destructor
sc_concatref::~sc_concatref()
{
}

// Capacity
unsigned int sc_concatref::length() const
{
}

// Explicit conversions
int sc_concatref::to_int() const
{
}

unsigned int sc_concatref::to_uint() const
{
}

long sc_concatref::to_long() const
{
}

unsigned long sc_concatref::to_ulong() const
{
}

int64 sc_concatref::to_int64() const
{
}

uint64 sc_concatref::to_uint64() const
{
}

double sc_concatref::to_double() const
{
}

void sc_concatref::to_sc_signed(sc_signed& target) const
{
}

void sc_concatref::to_sc_unsigned(sc_unsigned& target) const
{
}

// Implicit conversions
sc_concatref::operator uint64() const
{
}

sc_concatref::operator const sc_unsigned&() const
{
}


// Unary operators
sc_unsigned sc_concatref::operator + () const
{
}

sc_unsigned sc_concatref::operator - () const
{
}

sc_unsigned sc_concatref::operator ~ () const
{
}

// Explicit conversion to character string
const std::string sc_concatref::to_string(sc_numrep numrep) const
{
}

const std::string sc_concatref::to_string(sc_numrep numrep, bool w_prefix) const
{
}

// Assignment operators
const sc_concatref& sc_concatref::operator = (int v)
{
}

const sc_concatref& sc_concatref::operator = (unsigned int v)
{
}

const sc_concatref& sc_concatref::operator = (long v)
{
}

const sc_concatref& sc_concatref::operator = (unsigned long v)
{
}

const sc_concatref& sc_concatref::operator = (int64 v)
{
}

const sc_concatref& sc_concatref::operator = (uint64 v)
{
}

const sc_concatref& sc_concatref::operator = (const sc_concatref& v)
{
}

const sc_concatref& sc_concatref::operator = (const sc_signed& v)
{
}

const sc_concatref& sc_concatref::operator = (const sc_unsigned& v)
{
}

const sc_concatref& sc_concatref::operator = (const char *v_p)
{
}

const sc_concatref& sc_concatref::operator = (const sc_bv_base& v)
{
}

const sc_concatref& sc_concatref::operator = (const sc_lv_base& v)
{
}

// Reduce methods
bool sc_concatref::and_reduce() const
{
}

bool sc_concatref::nand_reduce() const
{
}

bool sc_concatref::or_reduce() const
{
}

bool sc_concatref::nor_reduce() const
{
}

bool sc_concatref::xor_reduce() const
{
}

bool sc_concatref::xnor_reduce() const
{
}

// Other methods
void sc_concatref::print(std::ostream& os) const
{
}

void sc_concatref::scan(std::istream& is)
{
}


sc_concatref::sc_concatref(const sc_concatref&)
{
}

sc_concatref::sc_concatref()
{
}

sc_concatref& concat(sc_value_base & a, sc_value_base& b)
{
}

const sc_concatref& concat(const sc_value_base& a, const sc_value_base& b)
{
}

const sc_concatref& concat(const sc_value_base& a, bool b)
{
}

const sc_concatref& concat(bool a, const sc_value_base& b)
{
}

sc_concatref& operator , (sc_value_base& a, sc_value_base& b)
{
}

const sc_concatref& operator , (const sc_value_base& a, const sc_value_base& b)
{
}

const sc_concatref& operator , (const sc_value_base& a, bool b)
{
}

const sc_concatref& operator , (bool a, const sc_value_base& b)
{
}

} // end of namespace sc_dt
