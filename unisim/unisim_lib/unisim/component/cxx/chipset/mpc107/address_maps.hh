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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ADDRESSMAPS_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ADDRESSMAPS_HH__

#include <inttypes.h>

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/cxx/chipset/mpc107/address_map_entry.hh"
#include "unisim/component/cxx/chipset/mpc107/config_regs.hh"
#include "unisim/component/cxx/chipset/mpc107/atu/atu.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {

using unisim::kernel::ServiceImport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::component::cxx::chipset::mpc107::ConfigurationRegisters;
using unisim::component::cxx::chipset::mpc107::atu::ATU;

template <class ADDRESS_TYPE, class PCI_ADDRESS_TYPE, bool DEBUG = false>
class AddressMap :
	virtual public Object {
private:
	typedef uint32_t address_t;

	class AddressMapEntryNode {
	public:
		AddressMapEntry *entry;
		AddressMapEntryNode *next;
		AddressMapEntryNode *prev;

		AddressMapEntryNode() :
			entry(NULL), next(NULL), prev(NULL) {
		}

		AddressMapEntryNode(AddressMapEntry *_entry) :
			entry(_entry), next(NULL), prev(NULL) {
		}
	};

	AddressMapEntryNode *proc_list;
	AddressMapEntryNode *pci_list;

	void DeleteEntryList(AddressMapEntryNode **list);	
	void AddEntryNode(AddressMapEntryNode **list, AddressMapEntryNode *entry);
	void CreateAddressMapEntry(AddressMapEntryNode **list,
		const char *name,
		address_t orig,
		address_t end,
		AddressMapEntry::Type type);
	AddressMapEntry *GetEntry(AddressMapEntryNode *list, 
		address_t addr, const char *list_name);
	bool SetAddressMapA();
	bool SetAddressMapB();
	void CreateProcessorPCIView();
	void CreateMapBPCIViewHostMode();
	void CreateMapBPCIViewAgentMode();
//	bool SetEmbeddedUtilities();
							
	ConfigurationRegisters *config_regs;
	ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> *atu;

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
public:
	virtual bool BeginSetup();
	virtual void OnDisconnect();
	
	AddressMap(ConfigurationRegisters &_config_regs,
		ATU<ADDRESS_TYPE, PCI_ADDRESS_TYPE, DEBUG> &_atu,
		const char *name, Object *parent = NULL);
	~AddressMap();

	bool Reset();
	
	AddressMapEntry *GetEntryProc(address_t addr);
	AddressMapEntry *GetEntryPCI(address_t addr);
};

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
	
#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_ADDRESSMAPS_HH__
