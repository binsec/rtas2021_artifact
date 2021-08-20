/*
 *  Copyright (c) 2019-2020,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/component/cxx/processor/mips/isa/disasm.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace mips {
namespace isa {

std::ostream& operator << (std::ostream& sink, Ostreamable const& os)
{
  os(sink);
  return sink;
}

void
PrintGPR::operator () (std::ostream& sink) const
{
  sink << std::dec;
  if      (idx == 0)  sink << "zero";
  else if (idx == 1)  sink << "at";
  else if (idx < 4)   sink << "v" << (idx-2);
  else if (idx < 8)   sink << "a" << (idx-4);
  else if (idx < 16)  sink << "t" << (idx-8);
  else if (idx < 24)  sink << "s" << (idx-16);
  else if (idx < 26)  sink << "t" << (idx-24+8);
  else if (idx < 28)  sink << "k" << (idx-26);
  else if (idx == 28) sink << "gp";
  else if (idx == 29) sink << "sp";
  else if (idx == 30) sink << "fp";
  else if (idx == 31) sink << "ra";
}

} /* end of namespace isa */
} /* end of namespace mips */
} /* end of namespace processor */
} /* end of namespace cxx */
} /* end of namespace component */
} /* end of namespace unisim */

