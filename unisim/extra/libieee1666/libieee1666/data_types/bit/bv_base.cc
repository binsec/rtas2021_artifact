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

#include <data_types/bit/bv_base.h>
#include <data_types/bit/bitref_r.h>
#include <data_types/bit/bitref.h>
#include <data_types/bit/subref.h>

namespace sc_dt {

// Constructors
sc_bv_base::sc_bv_base(int nb)
{
}

sc_bv_base::sc_bv_base(bool a, int nb)
{
}

sc_bv_base::sc_bv_base(const char *a)
{
}

sc_bv_base::sc_bv_base(const char *a, int nb)
{
}

sc_bv_base::sc_bv_base(const sc_lv_base& a)
{
}

sc_bv_base::sc_bv_base(const sc_bv_base& a)
{
}

// Destructor
sc_bv_base::~sc_bv_base()
{
}

// Assignment operators
sc_bv_base& sc_bv_base::operator = (const sc_bv_base& a)
{
}

sc_bv_base& sc_bv_base::operator = (const sc_lv_base& a)
{
}

sc_bv_base& sc_bv_base::operator = (const char *a)
{
}

sc_bv_base& sc_bv_base::operator = (const bool *a)
{
}

sc_bv_base& sc_bv_base::operator = (const sc_logic *a)
{
}

sc_bv_base& sc_bv_base::operator = (const sc_unsigned& a)
{
}

sc_bv_base& sc_bv_base::operator = (const sc_signed& a)
{
}

sc_bv_base& sc_bv_base::operator = (const sc_uint_base& a)
{
}

sc_bv_base& sc_bv_base::operator = (const sc_int_base& a)
{
}

sc_bv_base& sc_bv_base::operator = (unsigned long a)
{
}

sc_bv_base& sc_bv_base::operator = (long a)
{
}

sc_bv_base& sc_bv_base::operator = (unsigned int a)
{
}

sc_bv_base& sc_bv_base::operator = (int a)
{
}

sc_bv_base& sc_bv_base::operator = (uint64 a)
{
}

sc_bv_base& sc_bv_base::operator = (int64 a)
{
}

// Bitwise rotations
sc_bv_base& sc_bv_base::lrotate(int n)
{
}

sc_bv_base& sc_bv_base::rrotate(int n)
{
}

// Bitwise reverse
sc_bv_base& sc_bv_base::reverse()
{
}

// Bit selection
sc_bitref<sc_bv_base> sc_bv_base::operator [] (int i)
{
}

sc_bitref_r<sc_bv_base> sc_bv_base::operator [] (int i) const
{
}

// Part selection
sc_subref<sc_bv_base> sc_bv_base::operator () (int hi, int lo)
{
}

sc_subref_r<sc_bv_base> sc_bv_base::operator () (int hi, int lo) const
{
}

sc_subref<sc_bv_base> sc_bv_base::range(int hi, int lo)
{
}

sc_subref_r<sc_bv_base> sc_bv_base::range(int hi, int lo) const
{
}

// Reduce functions
sc_logic_value_t sc_bv_base::and_reduce() const
{
}

sc_logic_value_t sc_bv_base::nand_reduce() const
{
}

sc_logic_value_t sc_bv_base::or_reduce() const
{
}

sc_logic_value_t sc_bv_base::nor_reduce() const
{
}

sc_logic_value_t sc_bv_base::xor_reduce() const
{
}

sc_logic_value_t sc_bv_base::xnor_reduce() const
{
}

// Common methods
int sc_bv_base::length() const
{
}

// Explicit conversions to character string
const std::string sc_bv_base::to_string() const
{
}

const std::string sc_bv_base::to_string(sc_numrep) const
{
}

const std::string sc_bv_base::to_string(sc_numrep, bool) const
{
}

// Explicit conversions
int sc_bv_base::to_int() const
{
}

unsigned int sc_bv_base::to_uint() const
{
}

long sc_bv_base::to_long() const
{
}

unsigned long sc_bv_base::to_ulong() const
{
}

int64 sc_bv_base::to_int64() const
{
}

uint64 sc_bv_base::to_uint64() const
{
}

bool sc_bv_base::is_01() const
{
}

// Other methods
void sc_bv_base::print(std::ostream& os) const
{
}

void sc_bv_base::scan(std::istream& is)
{
}

} // end of namespace sc_dt
