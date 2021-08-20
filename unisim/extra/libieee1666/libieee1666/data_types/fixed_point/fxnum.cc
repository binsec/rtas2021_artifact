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

#include <data_types/fixed_point/fxnum.h>
#include <data_types/fixed_point/fxval.h>
#include <data_types/fixed_point/fxnum_bitref.h>
#include <data_types/fixed_point/fxnum_subref.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Unary operators
const sc_fxval sc_fxnum::operator - () const
{
}

const sc_fxval sc_fxnum::operator + () const
{
}

// Binary operators
#define DEF_BIN_OP_T(op, tp) \
const sc_fxval operator op (const sc_fxnum&, tp) \
{ \
} \
  \
const sc_fxval operator op (tp, const sc_fxnum&) \
{ \
}

#define DEF_BIN_OP_OTHER(op) \
DEF_BIN_OP_T(op, int64) \
DEF_BIN_OP_T(op, uint64) \
DEF_BIN_OP_T(op, const sc_int_base&) \
DEF_BIN_OP_T(op, const sc_uint_base&) \
DEF_BIN_OP_T(op, const sc_signed&) \
DEF_BIN_OP_T(op, const sc_unsigned&)

#define DEF_BIN_OP(op, dummy) \
const sc_fxval operator op (const sc_fxnum&, const sc_fxnum&) \
{ \
} \
  \
DEF_BIN_OP_T(op, int) \
DEF_BIN_OP_T(op, unsigned int) \
DEF_BIN_OP_T(op, long) \
DEF_BIN_OP_T(op, unsigned long) \
DEF_BIN_OP_T(op, float) \
DEF_BIN_OP_T(op, double) \
DEF_BIN_OP_T(op, const char *) \
DEF_BIN_OP_T(op, const sc_fxval&) \
DEF_BIN_OP_T(op, const sc_fxval_fast&) \
DEF_BIN_OP_T(op, const sc_fxnum_fast&) \
DEF_BIN_OP_OTHER(op)

DEF_BIN_OP(*, mult)
DEF_BIN_OP(+, add)
DEF_BIN_OP(-, sub)
DEF_BIN_OP(/, div)

#undef DEF_BIN_OP_T
#undef DEF_BIN_OP_OTHER
#undef DEF_BIN_OP

const sc_fxval operator<< (const sc_fxnum&, int)
{
}

const sc_fxval operator>> (const sc_fxnum&, int)
{
}

// Relational (including equality) operators
#define DEF_REL_OP_T(op, tp) \
bool operator op (const sc_fxnum&, tp) \
{ \
} \
  \
bool operator op (tp, const sc_fxnum&) \
{ \
}

#define DEF_REL_OP_OTHER(op) \
DEF_REL_OP_T(op, int64) \
DEF_REL_OP_T(op, uint64) \
DEF_REL_OP_T(op, const sc_int_base&) \
DEF_REL_OP_T(op, const sc_uint_base&) \
DEF_REL_OP_T(op, const sc_signed&) \
DEF_REL_OP_T(op, const sc_unsigned&)

#define DEF_REL_OP(op) \
bool operator op (const sc_fxnum&, const sc_fxnum&) \
{ \
} \
  \
DEF_REL_OP_T(op, int) \
DEF_REL_OP_T(op, unsigned int) \
DEF_REL_OP_T(op, long) \
DEF_REL_OP_T(op, unsigned long) \
DEF_REL_OP_T(op, float) \
DEF_REL_OP_T(op, double) \
DEF_REL_OP_T(op, const char *) \
DEF_REL_OP_T(op, const sc_fxval&) \
DEF_REL_OP_T(op, const sc_fxval_fast&) \
DEF_REL_OP_T(op, const sc_fxnum_fast&) \
DEF_REL_OP_OTHER(op)

DEF_REL_OP(<)
DEF_REL_OP(<=)
DEF_REL_OP(>)
DEF_REL_OP(>=)
DEF_REL_OP(==)
DEF_REL_OP(!=)

#undef DEF_REL_OP_T
#undef DEF_REL_OP_OTHER
#undef DEF_REL_OP

// Assignment operators
#define DEF_ASN_OP_T(op, tp) \
sc_fxnum& sc_fxnum::operator op(tp) \
{ \
}

#define DEF_ASN_OP_OTHER(op) \
DEF_ASN_OP_T(op, int64) \
DEF_ASN_OP_T(op, uint64) \
DEF_ASN_OP_T(op, const sc_int_base&) \
DEF_ASN_OP_T(op, const sc_uint_base&) \
DEF_ASN_OP_T(op, const sc_signed&) \
DEF_ASN_OP_T(op, const sc_unsigned&)

#define DEF_ASN_OP(op) \
DEF_ASN_OP_T(op, int) \
DEF_ASN_OP_T(op, unsigned int) \
DEF_ASN_OP_T(op, long) \
DEF_ASN_OP_T(op, unsigned long) \
DEF_ASN_OP_T(op, float) \
DEF_ASN_OP_T(op, double) \
DEF_ASN_OP_T(op, const char *) \
DEF_ASN_OP_T(op, const sc_fxval&) \
DEF_ASN_OP_T(op, const sc_fxval_fast&) \
DEF_ASN_OP_T(op, const sc_fxnum&) \
DEF_ASN_OP_T(op, const sc_fxnum_fast&) \
DEF_ASN_OP_OTHER(op)

DEF_ASN_OP(=)
DEF_ASN_OP(*=)
DEF_ASN_OP(/=)
DEF_ASN_OP(+=)
DEF_ASN_OP(-=)
DEF_ASN_OP_T(<<=, int)
DEF_ASN_OP_T(>>=, int)

#undef DEF_ASN_OP_T
#undef DEF_ASN_OP_OTHER
#undef DEF_ASN_OP

// Auto-increment and auto-decrement
const sc_fxval sc_fxnum::operator ++ (int)
{
}

const sc_fxval sc_fxnum::operator -- (int)
{
}

sc_fxnum& sc_fxnum::operator ++ ()
{
}

sc_fxnum& sc_fxnum::operator -- ()
{
}

// Bit selection
const sc_fxnum_bitref sc_fxnum::operator [] (int) const
{
}

sc_fxnum_bitref sc_fxnum::operator [] (int)
{
}

// Part selection
const sc_fxnum_subref sc_fxnum::operator () (int, int) const
{
}

sc_fxnum_subref sc_fxnum::operator () (int, int)
{
}

const sc_fxnum_subref sc_fxnum::range(int, int) const
{
}

sc_fxnum_subref sc_fxnum::range(int, int)
{
}

const sc_fxnum_subref sc_fxnum::operator () () const
{
}

sc_fxnum_subref sc_fxnum::operator () ()
{
}

const sc_fxnum_subref sc_fxnum::range() const
{
}

sc_fxnum_subref sc_fxnum::range()
{
}

// Implicit conversion
sc_fxnum::operator double() const
{
}

// Explicit conversion to primitive types
short sc_fxnum::to_short() const
{
}

unsigned short sc_fxnum::to_ushort() const
{
}

int sc_fxnum::to_int() const
{
}

unsigned int sc_fxnum::to_uint() const
{
}

long sc_fxnum::to_long() const
{
}

unsigned long sc_fxnum::to_ulong() const
{
}

int64 sc_fxnum::to_int64() const
{
}

uint64 sc_fxnum::to_uint64() const
{
}

float sc_fxnum::to_float() const
{
}

double sc_fxnum::to_double() const
{
}

// Explicit conversion to character string
const std::string sc_fxnum::to_string() const
{
}

const std::string sc_fxnum::to_string(sc_numrep) const
{
}

const std::string sc_fxnum::to_string(sc_numrep, bool) const
{
}

const std::string sc_fxnum::to_string(sc_fmt) const
{
}

const std::string sc_fxnum::to_string(sc_numrep, sc_fmt) const
{
}

const std::string sc_fxnum::to_string(sc_numrep, bool, sc_fmt) const
{
}

const std::string sc_fxnum::to_dec() const
{
}

const std::string sc_fxnum::to_bin() const
{
}

const std::string sc_fxnum::to_oct() const
{
}

const std::string sc_fxnum::to_hex() const
{
}

// Query value
bool sc_fxnum::is_neg() const
{
}

bool sc_fxnum::is_zero() const
{
}

bool sc_fxnum::quantization_flag() const
{
}

bool sc_fxnum::overflow_flag() const
{
}

const sc_fxval sc_fxnum::value() const
{
}

// Query parameters
int sc_fxnum::wl() const
{
}

int sc_fxnum::iwl() const
{
}

sc_q_mode sc_fxnum::q_mode() const
{
}

sc_o_mode sc_fxnum::o_mode() const
{
}

int sc_fxnum::n_bits() const
{
}

const sc_fxtype_params& sc_fxnum::type_params() const
{
}

const sc_fxcast_switch& sc_fxnum::cast_switch() const
{
}

// Print or dump content
void sc_fxnum::print(std::ostream& os) const
{
}

void sc_fxnum::scan(std::istream& is)
{
}

void sc_fxnum::dump(std::ostream& os) const
{
}

// Disabled
sc_fxnum::sc_fxnum()
{
}

sc_fxnum::sc_fxnum(const sc_fxnum&)
{
}

} // end of namespace sc_dt
