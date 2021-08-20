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

#include <data_types/integer/signed.h>
#include <data_types/integer/signed_bitref.h>
#include <data_types/integer/signed_subref.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Constructors
sc_signed::sc_signed(int nb)
{
}

sc_signed::sc_signed(const sc_signed& v)
{
}

sc_signed::sc_signed(const sc_unsigned& v)
{
}

sc_signed::sc_signed(const sc_bv_base& v)
{
}

sc_signed::sc_signed(const sc_lv_base& v)
{
}

sc_signed::sc_signed(const sc_int_subref_r& v)
{
}

sc_signed::sc_signed(const sc_uint_subref_r& v)
{
}

sc_signed::sc_signed(const sc_signed_subref_r& v)
{
}

sc_signed::sc_signed(const sc_unsigned_subref_r& v)
{
}

// Assignment operators
sc_signed& sc_signed::operator = (const sc_signed& v)
{
}

sc_signed& sc_signed::operator = (const sc_signed_subref_r& a)
{
}

sc_signed& sc_signed::operator = (const sc_unsigned& v)
{
}

sc_signed& sc_signed::operator = (const sc_unsigned_subref_r& a)
{
}

sc_signed& sc_signed::operator = (const char *v)
{
}

sc_signed& sc_signed::operator = (int64 v)
{
}

sc_signed& sc_signed::operator = (uint64 v)
{
}

sc_signed& sc_signed::operator = (long v)
{
}

sc_signed& sc_signed::operator = (unsigned long v)
{
}

sc_signed& sc_signed::operator = (int v)
{
}

sc_signed& sc_signed::operator = (unsigned int v)
{
}

sc_signed& sc_signed::operator = (double v)
{
}

sc_signed& sc_signed::operator = (const sc_int_base& v)
{
}

sc_signed& sc_signed::operator = (const sc_uint_base& v)
{
}

sc_signed& sc_signed::operator = (const sc_bv_base&)
{
}

sc_signed& sc_signed::operator = (const sc_lv_base&)
{
}

sc_signed& sc_signed::operator = (const sc_fxval&)
{
}

sc_signed& sc_signed::operator = (const sc_fxval_fast&)
{
}

sc_signed& sc_signed::operator = (const sc_fxnum&)
{
}

sc_signed& sc_signed::operator = (const sc_fxnum_fast&)
{
}

// Destructor
sc_signed::~sc_signed()
{
}

// Increment operators.
sc_signed& sc_signed::operator ++ ()
{
}

const sc_signed sc_signed::operator ++ (int)
{
}

// Decrement operators.
sc_signed& sc_signed::operator -- ()
{
}

const sc_signed sc_signed::operator -- (int)
{
}

// Bit selection
sc_signed_bitref sc_signed::operator[] (int i)
{
}

sc_signed_bitref_r sc_signed::operator[] (int i) const
{
}

// Part selection
sc_signed_subref sc_signed::range(int i, int j)
{
}

sc_signed_subref_r sc_signed::range(int i, int j) const
{
}

sc_signed_subref sc_signed::operator() (int i, int j)
{
}

sc_signed_subref_r sc_signed::operator() (int i, int j) const
{
}

// Explicit conversions
int sc_signed::to_int() const
{
}

unsigned int sc_signed::to_uint() const
{
}

long sc_signed::to_long() const
{
}

unsigned long sc_signed::to_ulong() const
{
}

int64 sc_signed::to_int64() const
{
}

uint64 sc_signed::to_uint64() const
{
}

double sc_signed::to_double() const
{
}

// Explicit conversion to character string
const std::string sc_signed::to_string(sc_numrep numrep) const
{
}

const std::string sc_signed::to_string(sc_numrep numrep, bool w_prefix) const
{
}

// Print functions
void sc_signed::print(std::ostream& os) const
{
}

void sc_signed::scan(std::istream& is)
{
}

// Capacity
int sc_signed::length() const
{
}


// Reduce methods
bool sc_signed::and_reduce() const
{
}

bool sc_signed::nand_reduce() const
{
}

bool sc_signed::or_reduce() const
{
}

bool sc_signed::nor_reduce() const
{
}

bool sc_signed::xor_reduce() const
{
}

bool sc_signed::xnor_reduce() const
{
}


// Overloaded operators

} // end of namespace sc_dt
