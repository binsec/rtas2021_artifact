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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_MIPS_ISA_DISASM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_MIPS_ISA_DISASM_HH__

#include <iostream>
#include <iomanip>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace mips {
namespace isa {

struct Ostreamable
{
  virtual void operator() ( std::ostream& sink ) const = 0;
  virtual ~Ostreamable() {};
  friend std::ostream& operator << (std::ostream& sink, Ostreamable const& os);
};
 
struct PrintGPR : public Ostreamable
{
  PrintGPR( unsigned _idx ) : idx(_idx) {} unsigned idx;
  void operator () (std::ostream& sink) const;
};

struct PrintR : public Ostreamable
{
  PrintR( char const* _prefix, unsigned _num ) : prefix(_prefix), num(_num) {} char const* prefix; unsigned num;
  void operator () ( std::ostream& sink ) const { sink << prefix << std::dec << num; }
};

struct PrintHex : public Ostreamable
{
  PrintHex( uint32_t _num ) : num(_num) {} uint32_t num;
  void operator () ( std::ostream& sink ) const { sink << "0x" << std::hex << num; }
};

struct PrintI : public Ostreamable
{
  PrintI( int32_t _num ) : num(_num) {} int32_t num;
  void operator () ( std::ostream& sink ) const { sink << std::dec << num; }
};

struct PrintCond : public Ostreamable
{
  PrintCond( unsigned _rid ) : rid(_rid) {} unsigned rid;
  void operator () ( std::ostream& sink ) const
  {
    char const* condnames[] = {"f",    "un",   "eq",   "ueq",  "olt",  "ult",  "ole",  "ule",  "sf",
                               "ngle", "seq",  "ngl",  "lt",   "nge",  "le",   "ngt"};
    sink << condnames[rid];
  }
};



} /* end of namespace isa */
} /* end of namespace mips */
} /* end of namespace processor */
} /* end of namespace cxx */
} /* end of namespace component */
} /* end of namespace unisim */

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_MIPS_ISA_DISASM_HH__

