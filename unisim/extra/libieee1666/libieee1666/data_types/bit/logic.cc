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

#include <data_types/bit/logic.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Constructors
sc_logic::sc_logic()
{
}

sc_logic::sc_logic(const sc_logic& a)
{
}

sc_logic::sc_logic(sc_logic_value_t v)
{
}

sc_logic::sc_logic(bool a)
{
}

sc_logic::sc_logic(char a)
{
}

sc_logic::sc_logic(int a)
{
}

// Destructor
sc_logic::~sc_logic()
{
}

// Assignment operators
sc_logic& sc_logic::operator = (const sc_logic& a)
{
}

sc_logic& sc_logic::operator = (sc_logic_value_t v)
{
}

sc_logic& sc_logic::operator = (bool a)
{
}

sc_logic& sc_logic::operator = (char a)
{
}

sc_logic& sc_logic::operator = (int a)
{
}

// Explicit conversions
sc_logic_value_t sc_logic::value() const
{
}

char sc_logic::to_char() const
{
}

bool sc_logic::to_bool() const
{
}

bool sc_logic::is_01() const
{
}

// Other methods
void sc_logic::print(std::ostream& os) const
{
}

void sc_logic::scan(std::istream& is)
{
}

// Disabled
sc_logic::sc_logic(const char *)
{
}

sc_logic& sc_logic::operator = (const char *)
{
}

} // end of namespace sc_dt
