/*
 *  Copyright (c) 2015-2016,
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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
  
  /** struct Exception
   *  
   * Base class used to abort normal execution of an instruction and
   * take processor to related handler (using a throw).
   */
  struct Exception : public std::exception { Exception() {} virtual const char* what() const throw() { return "Exception"; } };
  
  struct UndefInstrException : Exception { UndefInstrException() {} virtual const char* what() const throw() { return "UndefInstrException"; } };
  struct HypTrapException : Exception { HypTrapException() {} virtual const char* what() const throw() { return "HypTrapException"; } };
  struct SVCException : Exception  { SVCException() {} virtual const char* what() const throw() { return "SVCException"; } };
  struct SMCException : Exception  { SMCException() {} virtual const char* what() const throw() { return "SMCException"; } };
  struct HVCException : Exception  { HVCException() {} virtual const char* what() const throw() { return "HVCException"; } };
  struct PrefetchAbortException : Exception { PrefetchAbortException() {} virtual const char* what() const throw() { return "PrefetchAbortException"; } };
  struct DataAbortException : Exception { DataAbortException() {} virtual const char* what() const throw() { return "DataAbortException"; } };
  struct VirtualAbortException : Exception { VirtualAbortException() {} virtual const char* what() const throw() { return "VirtualAbortException"; } };
  
  // Data Abort Types
  enum DAbort {
    DAbort_AccessFlag,
    DAbort_Alignment,
    DAbort_Background,
    DAbort_Domain,
    DAbort_Permission,
    DAbort_Translation,
    DAbort_SyncExternal,
    DAbort_SyncExternalonWalk,
    DAbort_SyncParity,
    DAbort_SyncParityonWalk,
    DAbort_AsyncParity,
    DAbort_AsyncExternal,
    DAbort_DebugEvent,
    DAbort_TLBConflict,
    DAbort_Lockdown,
    DAbort_Coproc,
    DAbort_ICacheMaint
  };

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_EXCEPTION_HH__
