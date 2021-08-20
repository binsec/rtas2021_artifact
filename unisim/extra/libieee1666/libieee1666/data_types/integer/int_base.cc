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

#include <data_types/integer/int_base.h>
#include <data_types/integer/int_bitref.h>
#include <data_types/integer/int_subref.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Constructors
sc_int_base::sc_int_base(int w)
{
}

sc_int_base::sc_int_base(int_type v, int w)
{
}

sc_int_base::sc_int_base(const sc_int_base& a)
{
}

sc_int_base::sc_int_base(const sc_int_subref_r& a)
{
}

sc_int_base::sc_int_base(const sc_signed& a)
{
}

sc_int_base::sc_int_base(const sc_unsigned& a)
{
}

sc_int_base::sc_int_base(const sc_bv_base& v)
{
}

sc_int_base::sc_int_base(const sc_lv_base& v)
{
}

sc_int_base::sc_int_base(const sc_uint_subref_r& v)
{
}

sc_int_base::sc_int_base(const sc_signed_subref_r& v)
{
}

sc_int_base::sc_int_base(const sc_unsigned_subref_r& v)
{
}

// Destructor
sc_int_base::~sc_int_base()
{
}


// Assignment operators
sc_int_base& sc_int_base::operator = (int_type v)
{
}

sc_int_base& sc_int_base::operator = (const sc_int_base& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_int_subref_r & a)
{
}

sc_int_base& sc_int_base::operator = (const sc_signed& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_unsigned& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxval& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxval_fast& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxnum& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxnum_fast& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_bv_base& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_lv_base& a)
{
}

sc_int_base& sc_int_base::operator = (const char *a)
{
}

sc_int_base& sc_int_base::operator = (unsigned long a)
{
}

sc_int_base& sc_int_base::operator = (long a)
{
}

sc_int_base& sc_int_base::operator = (unsigned int a)
{
}

sc_int_base& sc_int_base::operator = (int a)
{
}

sc_int_base& sc_int_base::operator = (uint64 a)
{
}

sc_int_base& sc_int_base::operator = (double a)
{
}


// Prefix and postfix increment and decrement operators
sc_int_base& sc_int_base::operator ++ ()          // Prefix
{
}

const sc_int_base sc_int_base::operator ++ (int)  // Postfix
{
}

sc_int_base& sc_int_base::operator -- ()          // Prefix
{
}

const sc_int_base sc_int_base::operator -- (int)  // Postfix
{
}

// Bit selection
sc_int_bitref sc_int_base::operator [] (int i)
{
}

sc_int_bitref_r sc_int_base::operator [] (int i) const
{
}


// Part selection
sc_int_subref sc_int_base::operator () (int left, int right)
{
}

sc_int_subref_r sc_int_base::operator () (int left, int right) const
{
}

sc_int_subref sc_int_base::range(int left, int right)
{
}

sc_int_subref_r sc_int_base::range(int left, int right) const
{
}

// Capacity
int sc_int_base::length() const
{
}

// Reduce methods
bool sc_int_base::and_reduce() const
{
}

bool sc_int_base::nand_reduce() const
{
}

bool sc_int_base::or_reduce() const
{
}

bool sc_int_base::nor_reduce() const
{
}

bool sc_int_base::xor_reduce() const
{
}

bool sc_int_base::xnor_reduce() const
{
}

// Implicit conversion to int_type
sc_int_base::operator int_type() const
{
}

// Explicit conversions
int sc_int_base::to_int() const
{
}

unsigned int sc_int_base::to_uint() const
{
}

long sc_int_base::to_long() const
{
}

unsigned long sc_int_base::to_ulong() const
{
}

int64 sc_int_base::to_int64() const
{
}

uint64 sc_int_base::to_uint64() const
{
}

double sc_int_base::to_double() const
{
}

// Explicit conversion to character string
const std::string sc_int_base::to_string(sc_numrep numrep) const
{
}

const std::string sc_int_base::to_string(sc_numrep numrep, bool w_prefix) const
{
}


// Other methods
void sc_int_base::print(std::ostream& os) const
{
}

void sc_int_base::scan(std::istream& is)
{
}

} // end of namespace sc_dt
