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

#ifndef __UNISIM_COMPONENT_CXX_PCI_VIDEO_DISPLAY_TCC__
#define __UNISIM_COMPONENT_CXX_PCI_VIDEO_DISPLAY_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace video {

using namespace std;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
	
template <class ADDRESS>
Display<ADDRESS>::Display(const char *name, Object *parent) :
	Object(name, parent, "PCI Video frame buffer display"),
	Service<Memory<ADDRESS> >(name, parent),
	Client<Video<ADDRESS> >(name, parent),
	memory_export("memory-export", this),
	video_import("video-import", this),
	frame_buffer(0),
	bytesize(0),
	width(0),
	height(0),
	depth(0),
	bytes_per_pixels(0),
	bytes_per_line(0),
	logger(*this),
	verbose(false),

	// PCI configuration registers initialization
	pci_conf_device_id("pci_conf_device_id", "PCI config Device ID", 0x0, 0x4758), // Mach64 GT
	pci_conf_vendor_id("pci_conf_vendor_id", "PCI config Vendor ID", 0x0, 0x1002), // ATI Technologies Inc.
	pci_conf_status("pci_conf_status", "PCI Config Status", 0x0, 0x0),
	pci_conf_command("pci_conf_command", "PCI Config Command", 0x007f, 0x0),
	pci_conf_class_code("pci_conf_class_code", "PCI Config Class Code", 0x0, 0x0),
	pci_conf_revision_id("pci_conf_revision_id", "PCI Config Revision ID", 0x0, 0x0),
	pci_conf_bist("pci_conf_bist", "PCI Config BIST", 0x0, 0x0),
	pci_conf_header_type("pci_conf_header_type", "PCI Config Header Type", 0x0, 0x0),
	pci_conf_latency_timer("pci_conf_latency_timer", "PCI Config Latency Timer", 0x0, 0x0),
	pci_conf_cache_line_size("pci_conf_cache_line_size", "PCI Config Cache Line Size", 0x0, 0x0),

	pci_conf_carbus_cis_pointer("pci_conf_carbus_cis_pointer", "PCI Config Carbus CIS Pointer", 0x0, 0x0),
	pci_conf_subsystem_id("pci_conf_subsystem_id", "PCI Config Subsystem ID", 0x0, 0x0),
	pci_conf_subsystem_vendor_id("pci_conf_subsystem_vendor_id", "PCI Config Subsystem Vendor ID", 0x0, 0x0),

	pci_device_number(0),
	initial_base_addr(0),
	pci_bus_frequency(33),

	// Parameters initialization
	param_verbose("verbose", this, verbose, "enable/disable verbosity"),
	param_width("width", this, width, "screen width in pixels"),
	param_height("height", this, height, "screen height in pixels"),
	param_depth("depth", this, depth, "screen depth in bits per pixel"),
	param_bytesize("bytesize", this, bytesize, "frame buffer size in bytes"),
	param_initial_base_addr("initial-base-addr", this, initial_base_addr, "initial base address of memory space"),
	param_pci_device_number("pci-device-number", this, pci_device_number, "PCI device number"),
	param_pci_bus_frequency("pci-bus-frequency", this, pci_bus_frequency, "PCI bus frequency")
{
	param_bytesize.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_width.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_height.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_depth.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
}

template <class ADDRESS>
Display<ADDRESS>::~Display()
{
	if(frame_buffer)
	{
		delete[] frame_buffer;
		frame_buffer = 0;
	}
}

template <class ADDRESS>
bool Display<ADDRESS>::BeginSetup()
{
	if(frame_buffer)
	{
		delete[] frame_buffer;
		frame_buffer = 0;
	}
	
	return true;
}

template <class ADDRESS>
bool Display<ADDRESS>::SetupFrameBuffer()
{
	if(frame_buffer) return true;

	bytes_per_pixels = (depth + 7) / 8;
	bytes_per_line = width * bytes_per_pixels;
	
	if(bytes_per_line * height > bytesize)
	{
		logger << DebugError << ": Frame buffer is too small to handle video mode " << width << " pixels x " << height << " pixels x " << depth << " bits per pixel..." << EndDebugError;
		return false;
	}
	
	frame_buffer = new uint8_t[bytesize];
	memset(frame_buffer, 0, bytesize);
	
	return frame_buffer != 0;
}

template <class ADDRESS>
bool Display<ADDRESS>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &memory_export) return SetupFrameBuffer();
	
	logger << DebugError << "Internal error" << EndDebugError;
	return false;
}

template <class ADDRESS>
bool Display<ADDRESS>::EndSetup()
{
	// PCI configuration registers initialization	
	pci_conf_base_addr.Initialize("pci_conf_base_addr", "PCI Config Base Address", 0xfffffff0UL, initial_base_addr);

	pci_conf_command = pci_conf_command | 0x2; // active memory space

	if(video_import)
	{
		if(!video_import->SetVideoMode(pci_conf_base_addr, width, height, depth, bytes_per_line))
		{
			logger << DebugError << "Can't set video mode" << EndDebugError;
			return false;
		}
	}
	else
	{
		logger << DebugWarning << "No video output on host machine is available" << EndDebugWarning;
	}
	
	return true;
}

template <class ADDRESS>
void Display<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void Display<ADDRESS>::Reset()
{
}

template <class ADDRESS>
void Display<ADDRESS>::ResetMemory()
{
	this->Reset();
}

template <class ADDRESS>
bool Display<ADDRESS>::WriteMemory(ADDRESS physical_addr, const void *buffer, uint32_t size)
{
	if(physical_addr < pci_conf_base_addr || (physical_addr + (size - 1)) >= (pci_conf_base_addr + bytesize) || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)

	memcpy(frame_buffer + physical_addr - pci_conf_base_addr, buffer, size);
	if(video_import) video_import->RefreshDisplay();
	return true;
}

template <class ADDRESS>
bool Display<ADDRESS>::ReadMemory(ADDRESS physical_addr, void *buffer, uint32_t size)
{
	if(physical_addr < pci_conf_base_addr || (physical_addr + (size - 1)) >= (pci_conf_base_addr + bytesize) || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)
	memcpy(buffer, frame_buffer + physical_addr - pci_conf_base_addr, size);
	return true;
}

template <class ADDRESS>
uint8_t Display<ADDRESS>::ReadConfigByte(unsigned int offset)
{
	switch(offset)
	{
		case 0x00:
			return pci_conf_vendor_id;
			break;
		case 0x01:
			return pci_conf_vendor_id >> 8;
			break;
		case 0x02:
			return pci_conf_device_id;
			break;
		case 0x03:
			return pci_conf_device_id >> 8;
			break;
		case 0x04:
			return pci_conf_command;
			break;
		case 0x05:
			return pci_conf_command >> 8;
			break;
		case 0x06:
			return pci_conf_status;
			break;
		case 0x07:
			return pci_conf_status >> 8;
			break;
		case 0x08:
			return pci_conf_revision_id;
			break;
		case 0x09:
			return pci_conf_class_code;
			break;
		case 0x0a:
			return pci_conf_class_code >> 8;
			break;
		case 0x0b:
			return pci_conf_class_code >> 16;
			break;
		case 0x0c:
			return pci_conf_cache_line_size;
			break;
		case 0x0d:
			return pci_conf_latency_timer;
			break;
		case 0x0e:
			return pci_conf_header_type;
			break;
		case 0x0f:
			return pci_conf_bist;
			break;
		case 0x10:
			return pci_conf_base_addr;
			break;
		case 0x11:
			return pci_conf_base_addr >> 8;
			break;
		case 0x12:
			return pci_conf_base_addr >> 16;
			break;
		case 0x13:
			return pci_conf_base_addr >> 24;
			break;
		case 0x28:
			return 0xf1;
		case 0x29:
			return 0xff;
		case 0x2a:
			return 0xff;
		case 0x2b:
			return 0xff;
	}
	return 0x00;
}

template <class ADDRESS>
void Display<ADDRESS>::WriteConfigByte(unsigned int offset, uint8_t value)
{
	switch(offset)
	{
		case 0x04:
			pci_conf_command = (pci_conf_command & 0xff00) | value;
			break;
		case 0x05:
			pci_conf_command = (pci_conf_command & 0x00ff) | ((uint16_t) value << 8);
			break;
		case 0x0f:
			pci_conf_bist = value;
			break;
		case 0x10:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xffffff00UL) == 0xffffff00UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0xffffff00UL) | value;
			break;
		case 0x11:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xffff00f0UL) == 0xffff00f0UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0xffff00f0UL) | ((uint16_t) value << 8);
			break;
		case 0x12:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xff00fff0UL) == 0xff00fff0UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0xff00fff0UL) | ((uint16_t) value << 16);
			break;
		case 0x13:
			pci_conf_base_addr = ((pci_conf_base_addr & 0x00fffff0UL) == 0x00fffff0UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0x00fffff0UL) | ((uint16_t) value << 24);
			break;
	}
}

template <class ADDRESS>
void Display<ADDRESS>::Read(ADDRESS addr, void *buffer, uint32_t size)
{
	ReadMemory(addr, buffer, size);
}

template <class ADDRESS>
void Display<ADDRESS>::Write(ADDRESS addr, const void *buffer, uint32_t size)
{
	WriteMemory(addr, buffer, size);
}

} // end of namespace video
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
