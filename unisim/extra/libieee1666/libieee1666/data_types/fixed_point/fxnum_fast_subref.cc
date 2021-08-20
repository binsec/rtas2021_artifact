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

#include <data_types/fixed_point/fxnum_fast_subref.h>
#include <data_types/bit/bv_base.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
sc_fxnum_fast_subref::sc_fxnum_fast_subref(const sc_fxnum_fast_subref&)
{
}

// Destructor
sc_fxnum_fast_subref::~sc_fxnum_fast_subref()
{
}

// Assignment operators
#define DEF_ASN_OP_T(tp) \
sc_fxnum_fast_subref& sc_fxnum_fast_subref::operator = (tp) \
{ \
}

DEF_ASN_OP_T(const sc_fxnum_subref&)
DEF_ASN_OP_T(const sc_fxnum_fast_subref&)
DEF_ASN_OP_T(const sc_bv_base&)
DEF_ASN_OP_T(const sc_lv_base&)
DEF_ASN_OP_T(const char *)
DEF_ASN_OP_T(const bool *)
DEF_ASN_OP_T(const sc_signed&)
DEF_ASN_OP_T(const sc_unsigned&)
DEF_ASN_OP_T(const sc_int_base&)
DEF_ASN_OP_T(const sc_uint_base&)
DEF_ASN_OP_T(int64)
DEF_ASN_OP_T(uint64)
DEF_ASN_OP_T(int)
DEF_ASN_OP_T(unsigned int)
DEF_ASN_OP_T(long)
DEF_ASN_OP_T(unsigned long)
DEF_ASN_OP_T(char)

#undef DEF_ASN_OP_T

#define DEF_ASN_OP_T_A(op, tp) \
sc_fxnum_fast_subref& sc_fxnum_fast_subref::operator op ## = (tp) \
{ \
}

#define DEF_ASN_OP_A(op) \
DEF_ASN_OP_T_A(op, const sc_fxnum_subref &) \
DEF_ASN_OP_T_A(op, const sc_fxnum_fast_subref&) \
DEF_ASN_OP_T_A(op, const sc_bv_base&) \
DEF_ASN_OP_T_A(op, const sc_lv_base&)

DEF_ASN_OP_A(&)
DEF_ASN_OP_A(|)
DEF_ASN_OP_A(^)

#undef DEF_ASN_OP_T_A
#undef DEF_ASN_OP_A

// Relational operators
#define DEF_REL_OP_T(op, tp) \
bool operator op (const sc_fxnum_fast_subref&, tp) \
{ \
} \
  \
bool operator op (tp, const sc_fxnum_fast_subref&) \
{ \
}

#define DEF_REL_OP(op) \
bool operator op (const sc_fxnum_fast_subref&, const sc_fxnum_fast_subref&) \
{ \
} \
  \
bool operator op (const sc_fxnum_fast_subref&, const sc_fxnum_subref&) \
{ \
} \
DEF_REL_OP_T(op, const sc_bv_base&) \
DEF_REL_OP_T(op, const sc_lv_base&) \
DEF_REL_OP_T(op, const char *) \
DEF_REL_OP_T(op, const bool *) \
DEF_REL_OP_T(op, const sc_signed&) \
DEF_REL_OP_T(op, const sc_unsigned&) \
DEF_REL_OP_T(op, int) \
DEF_REL_OP_T(op, unsigned int) \
DEF_REL_OP_T(op, long) \
DEF_REL_OP_T(op, unsigned long)

DEF_REL_OP(==)
DEF_REL_OP(!=)

#undef DEF_REL_OP_T
#undef DEF_REL_OP

// Reduce functions
bool sc_fxnum_fast_subref::and_reduce() const
{
}

bool sc_fxnum_fast_subref::nand_reduce() const
{
}

bool sc_fxnum_fast_subref::or_reduce() const
{
}

bool sc_fxnum_fast_subref::nor_reduce() const
{
}

bool sc_fxnum_fast_subref::xor_reduce() const
{
}

bool sc_fxnum_fast_subref::xnor_reduce() const
{
}

// Query parameter
int sc_fxnum_fast_subref::length() const
{
}

// Explicit conversions
int sc_fxnum_fast_subref::to_int() const
{
}

unsigned int sc_fxnum_fast_subref::to_uint() const
{
}

long sc_fxnum_fast_subref::to_long() const
{
}

unsigned long sc_fxnum_fast_subref::to_ulong() const
{
}

int64 sc_fxnum_fast_subref::to_int64() const
{
}

uint64 sc_fxnum_fast_subref::to_uint64() const
{
}

const std::string sc_fxnum_fast_subref::to_string() const
{
}

const std::string sc_fxnum_fast_subref::to_string(sc_numrep) const
{
}

const std::string sc_fxnum_fast_subref::to_string(sc_numrep, bool) const
{
}

// Implicit conversion
sc_fxnum_fast_subref::operator sc_bv_base() const
{
}

// Print or dump content
void sc_fxnum_fast_subref::print(std::ostream& os) const
{
}

void sc_fxnum_fast_subref::scan(std::istream& is)
{
}

void sc_fxnum_fast_subref::dump(std::ostream& os) const
{
}

// Disabled
// Constructor
sc_fxnum_fast_subref::sc_fxnum_fast_subref(sc_fxnum_fast&, int, int)
{
}

sc_fxnum_fast_subref::sc_fxnum_fast_subref()
{
}

} // end of namespace sc_dt
