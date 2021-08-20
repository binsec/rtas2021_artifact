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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PCI_VIDEO_DISPLAY_HH__
#define __UNISIM_COMPONENT_CXX_PCI_VIDEO_DISPLAY_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/video.hh>
#include <unisim/service/interfaces/time.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/device/register.hh>
#include <unisim/kernel/logger/logger.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace video {

using namespace unisim::util::endian;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Time;
using unisim::util::device::Register;
using unisim::service::interfaces::Video;

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;

template <class ADDRESS>
class Display :
	public Service<Memory<ADDRESS> >,
	public Client<Video<ADDRESS> >
{
public:
	typedef Register<uint32_t> reg32_t;
	typedef Register<uint32_t> reg24_t;
	typedef Register<uint16_t> reg16_t;
	typedef Register<uint8_t> reg8_t;
	
	ServiceExport<Memory<ADDRESS> > memory_export;
	ServiceImport<Video<ADDRESS> > video_import;
	
	Display(const char *name, Object *parent = 0);
	virtual ~Display();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();
	virtual void Reset();
	virtual void ResetMemory();
	virtual bool WriteMemory(ADDRESS physical_addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(ADDRESS physical_addr, void *buffer, uint32_t size);
	uint8_t ReadConfigByte(unsigned int offset);
	void WriteConfigByte(unsigned int offset, uint8_t value);
	void Read(ADDRESS addr, void *buffer, uint32_t size);
	void Write(ADDRESS addr, const void *buffer, uint32_t size);

private:
	uint8_t *frame_buffer;
	uint32_t bytesize; // WARNING! must be a power of two
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t bytes_per_pixels;
	uint32_t bytes_per_line;

protected:
	// debug stuff
	unisim::kernel::logger::Logger logger;
	bool verbose;
	
	// PCI configuration registers
	reg16_t pci_conf_device_id;
	reg16_t pci_conf_vendor_id;
	reg16_t pci_conf_status;
	reg16_t pci_conf_command;
	reg24_t pci_conf_class_code;
	reg8_t pci_conf_revision_id;
	reg8_t pci_conf_bist;
	reg8_t pci_conf_header_type;
	reg8_t pci_conf_latency_timer;
	reg8_t pci_conf_cache_line_size;
	reg32_t pci_conf_base_addr;

	reg32_t pci_conf_carbus_cis_pointer;
	reg16_t pci_conf_subsystem_id;
	reg16_t pci_conf_subsystem_vendor_id;
	
	uint32_t pci_device_number;
	ADDRESS initial_base_addr;
	unsigned int pci_bus_frequency; // in Mhz
private:
	Parameter<bool> param_verbose;
	Parameter<uint32_t> param_width;
	Parameter<uint32_t> param_height;
	Parameter<uint32_t> param_depth;
	Parameter<uint32_t> param_bytesize;
	Parameter<ADDRESS> param_initial_base_addr;
	Parameter<uint32_t> param_pci_device_number;
	Parameter<unsigned int> param_pci_bus_frequency;
	
	bool SetupFrameBuffer();
};

} // end of namespace video
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
