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

#include <arch.hh>

inline void eval_div( Processor& arch, Processor::u64_t& hi, Processor::u64_t& lo, Processor::u64_t const& divisor )    { throw Processor::Unimplemented(); }
inline void eval_div( Processor& arch, Processor::s64_t& hi, Processor::s64_t& lo, Processor::s64_t const& divisor )    { throw Processor::Unimplemented(); }
inline void eval_mul( Processor& arch, Processor::u64_t& hi, Processor::u64_t& lo, Processor::u64_t const& multiplier ) { throw Processor::Unimplemented(); }
inline void eval_mul( Processor& arch, Processor::s64_t& hi, Processor::s64_t& lo, Processor::s64_t const& multiplier ) { throw Processor::Unimplemented(); }
  
inline Processor::f64_t eval_fprem ( Processor& arch, Processor::f64_t const& dividend, Processor::f64_t const& modulus ) { throw Processor::Unimplemented(); }
inline Processor::f64_t eval_fprem1( Processor& arch, Processor::f64_t const& dividend, Processor::f64_t const& modulus ) { throw Processor::Unimplemented(); }

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {
          template <typename FPT> FPT firound( FPT const& src, int x87frnd_mode )
          {
            throw Processor::Unimplemented();
            return FPT();
          }
        } /* namespace unisim */ } /* namespace component */ } /* namespace cxx */ } /* namespace processor */ } /* namespace intel */


#include <unisim/component/cxx/processor/intel/isa/intel.tcc>

Processor::Operation*
Processor::Decode(uint64_t address, uint8_t const* bytes)
{
  typedef unisim::component::cxx::processor::intel::InputCode<Processor> InputCode;
  unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 ); /* intel64 mode */
  return getoperation( InputCode(mode, bytes, Processor::OpHeader(address) ) );
}

