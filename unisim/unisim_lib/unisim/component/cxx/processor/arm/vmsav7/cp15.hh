/*
 *  Copyright (c) 2010-2016,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier Perez (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV7_CP15_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV7_CP15_HH__


#include <unisim/component/cxx/processor/arm/register_field.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav7 {

  /** SCTLR Manips
   *
   * Base Register Fields to ease access to the SCTLR System Control
   * Register.  This class factorizes all common SCTLR register
   * fields, though some of them may not be present in a particular
   * ARM processor implementation
   */

  namespace sctlr
  {
    RegisterField<29,1> const AFE;     // Access flag enable
    RegisterField<28,1> const TRE;     // TEX remap enable
    RegisterField<20,1> const UWXN;    // Unprivileged write permission implies PL1 XN (Virtualization Extensions)
    RegisterField<19,1> const WXN;     // Write permission implies XN (Virtualization Extensions)
    RegisterField<17,1> const HA;      // Hardware Access flag enable
  }
  
  /** TTBCR Manips
   *
   *  TTBCR, Translation Table Base Control Register
   */

  namespace TTBCR
  {
    
    RegisterField<31,1> const EAE;   // Extended Address Enable
    RegisterField< 5,1> const PD1;   // Translation table walk disable for TTBR1
    RegisterField< 4,1> const PD0;   // Translation table walk disable for TTBR0
    RegisterField< 0,3> const N;     // Bit considered for TTBR selection
  };


} // end of namespace vmsav7
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV7_CP15_HH__
