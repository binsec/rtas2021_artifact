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
 
#ifndef __UNISIM_SERVICE_LOADER_ARM_GENRIC_KERNEL_LOADER_ARM_GENERIC_KERNEL_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_ARM_GENRIC_KERNEL_LOADER_ARM_GENERIC_KERNEL_LOADER_HH__

#include "unisim/service/interfaces/resetable.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/component/processor/arm/armtypes.hh"

namespace unisim {
namespace service {
namespace loader {
namespace arm_generic_kernel_loader {
  
  using unisim::component::processor::arm::reg_t;
  using unisim::component::processor::arm::address_t;
  using unisim::service::interfaces::Resetable;
  using unisim::kernel::Service;
  using unisim::kernel::ServiceImport;
  using unisim::kernel::ServiceExport;
  using unisim::kernel::Client;
  using unisim::kernel::Object;
  using unisim::service::interfaces::Registers;
  using unisim::service::interfaces::Loader;

  class ARMGenericKernelLoader :
    public Service<Resetable>,
	public Client<Registers>,
    public Client<Loader<address_t> > {
  public:
    /** Import to access the loader. */
    ServiceImport<Loader<address_t> > loader_import;
    /** 
     * Export of the service.
     * It provides a single method (Reset)
    */
    ServiceExport<Resetable> reset_export;

    /**
       Loader requires an interface
     *   with the CPU registers.
     */
    ServiceImport<Registers> registers_import;


    /** Constructor */
    ARMGenericKernelLoader(const char *name, Object *parent = 0);
    /** Destructor */
    ~ARMGenericKernelLoader();

    /* Service methods */
    /**
     * Method to execute when the service is disconnected.
     */
    virtual void OnDisconnect();
    /**
     * Setup method called at the end of setup
     *
     * @return True if the setup succeded.
     */
    virtual bool Setup();
    
    /**
     * Resets the arm generic kernel loader.
     */
    virtual void Reset();
  };


} // end of namespace arm_generic_kernel_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
