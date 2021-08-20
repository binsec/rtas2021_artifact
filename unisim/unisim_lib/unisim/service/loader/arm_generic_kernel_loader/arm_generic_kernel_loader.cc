/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include "unisim/service/loader/arm_generic_kernel_loader/arm_generic_kernel_loader.hh"
#include <iostream>

namespace unisim {
namespace service {
namespace loader {
namespace arm_generic_kernel_loader {

	using namespace std;

  ARMGenericKernelLoader::
  ARMGenericKernelLoader(const char *name, Object *parent) :
    Object(name, parent),
    Service<Resetable>(name, parent),
    Client<Registers>(name, parent),
    Client<Loader<address_t> >(name, parent),
    loader_import("loader-import", this),
    reset_export("reset-export", this),
    registers_import("registers-import", this) {
      SetupDependsOn(loader_import);
      SetupDependsOn(cpu_registers_import);
  }
  
  ARMGenericKernelLoader::
  ~ARMGenericKernelLoader() {
  }
  
  /* Service methods */
  void 
  ARMGenericKernelLoader::
  OnDisconnect() {
  }

//   bool 
//   ARMGenericKernelLoader::
//   ClientIndependentSetup() {
//     /* The loader interface is required */
//     if(!loader) return false;
// 		return true;
//   }

  bool 
  ARMGenericKernelLoader::
  Setup() {
    /* The service needs to be connected to the client to work correctly */
    if(!registers_import || !loader_import) return false;

    reg_t entry_point = loader_import->GetEntryPoint();
    RegisterInterface *pc_reg = registers_import->GetRegister("pc");
		cerr << "ARMGenericKernelLoader: Setting entry point to 0x" << hex << entry_point << dec << endl;
    pc_reg->SetValue(&entry_point);
    
    return true;
  }
  
  /* Reset method */
  void 
  ARMGenericKernelLoader::
  Reset() {
  }

} // end of namespace arm_generic_kernel_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim
