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
 
#ifndef __UNISIM_SERVICE_LOADER_PMAC_BOOTX_PMAC_BOOTX_HH__
#define __UNISIM_SERVICE_LOADER_PMAC_BOOTX_PMAC_BOOTX_HH__

#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/blob.hh>

#include <unisim/util/xml/xml.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <list>
#include <string>
#include <inttypes.h>

namespace unisim {
namespace service {
namespace loader {
namespace pmac_bootx {

using std::list;
using std::string;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;



struct DeviceProperty
{
	uint32_t name; // char	*name;
	int32_t	length;
	uint32_t value; //unsigned char *value;
	uint32_t next; // struct property *next;
};

struct DeviceNode
{
	uint32_t name; // char *name
	uint32_t type; // char *type
	uint32_t node; // phandle node
	int32_t n_addrs;
	uint32_t addrs; // struct address_range *addrs
	int32_t n_intrs;
	uint32_t intrs; // struct interrupt_info *intrs
	uint32_t full_name; // char *full_name
	uint32_t properties; // struct	property *properties
	uint32_t parent; // struct	device_node *parent
	uint32_t child; // struct	device_node *child;
	uint32_t sibling; // struct	device_node *sibling;
	uint32_t next; // struct	device_node *next /* next device of same type */
	uint32_t allnext; // struct	device_node *allnext;	/* next in list of all nodes */
};

class BootInfos;

class DeviceTree
{
public:
	DeviceTree(BootInfos *boot_infos, unisim::kernel::logger::Logger& logger, bool verbose);
	virtual ~DeviceTree();
	bool Load(const string& filename);
	uint32_t GetBase();
	uint32_t GetSize();
	DeviceNode *FindDevice(const char *prop_name, const char *prop_string_value);
	uint8_t *GetProperty(DeviceNode *reloc_node, const char *name, int32_t& len);

private:
	unisim::kernel::logger::Logger& logger;
	BootInfos *boot_infos;
	DeviceNode *root_node;
	DeviceNode *last_reloc_node;
	uint32_t base;
	uint32_t size;
	bool verbose;

	uint32_t Malloc(uint32_t size);
	void *Relocate(uint32_t offset);
	uint32_t UnRelocate(void *p);

	DeviceProperty *CreateDeviceProperty(const char *name, const uint8_t *value, int32_t len);
	DeviceProperty *CreateDeviceProperty(const unisim::util::xml::Node *xml_node);
	DeviceNode *CreateDeviceNode(DeviceNode *reloc_parent, const unisim::util::xml::Node *xml_node);
	void DumpDeviceProperty(DeviceProperty *reloc_prop);
	void DumpDeviceNode(DeviceNode *reloc_node);
	unsigned int ExpandEscapeSequences(const string& s, char *buffer);
};

class BootInfos
{
public:
	static const uint32_t MAX_BOOT_INFOS_IMAGE_SIZE = 104 * 1024 * 1024; // 104 MB
	
	BootInfos(unisim::kernel::logger::Logger& logger, bool verbose);
	virtual ~BootInfos();

	uint32_t Malloc(uint32_t size);
	void *Relocate(uint32_t offset);
	uint32_t UnRelocate(void *p);

	bool Load(uint32_t boot_infos_addr, const string& device_tree_filename, const string& kernel_parms, const string& ramdisk_filename, unsigned int screen_width, unsigned int screen_height);
	const unisim::util::blob::Blob<uint32_t> *GetBlob() const;
private:
	unisim::kernel::logger::Logger& logger;
	bool verbose;
	uint32_t size;
	uint32_t max_size;
	uint8_t *image;
	unisim::util::blob::Blob<uint32_t> *blob;

	/* On kernel entry:
	*
	* r3 = 0x426f6f58    ('BooX')
	* r4 = pointer to boot_infos
	* r5 = NULL
	*
	* Data and instruction translation disabled, interrupts
	* disabled, kernel loaded at physical 0x00000000 on PCI
	* machines (will be different on NuBus).
	*/
	
	static const uint32_t BOOT_INFO_VERSION = 5;
	static const uint32_t BOOT_INFO_COMPATIBLE_VERSION = 1;
	
	/* Bit in the architecture flag mask. More to be defined in
	future versions. Note that either BOOT_ARCH_PCI or
	BOOT_ARCH_NUBUS is set. The other BOOT_ARCH_NUBUS_xxx are
	set additionally when BOOT_ARCH_NUBUS is set.
	*/
	static const uint32_t BOOT_ARCH_PCI = 0x00000001UL;
	static const uint32_t BOOT_ARCH_NUBUS = 0x00000002UL;
	static const uint32_t BOOT_ARCH_NUBUS_PDM = 0x00000010UL;
	static const uint32_t BOOT_ARCH_NUBUS_PERFORMA = 0x00000020UL;
	static const uint32_t BOOT_ARCH_NUBUS_POWERBOOK = 0x00000040UL;
	
	/*  Maximum number of ranges in phys memory map */
	static const unsigned int MAX_MEM_MAP_SIZE = 26;

	/* This is the format of an element in the physical memory map. Note that
	the map is optional and current BootX will only build it for pre-PCI
	machines */
	typedef struct
	{
		uint32_t       physAddr;                /* Physical starting address */
		uint32_t       size;                    /* Size in bytes */
	} BootInfoMapEntry;

	/* Here are the boot informations that are passed to the bootstrap
	* Note that the kernel arguments and the device tree are appended
	* at the end of this structure. */
	typedef struct
	{
		/* Version of this structure */
		uint32_t       version;
		/* backward compatible down to version: */
		uint32_t       compatible_version;
		
		/* NEW (vers. 2) this holds the current _logical_ base addr of
		the frame buffer (for use by early boot message) */
		uint32_t      logicalDisplayBase;
	
		/* NEW (vers. 4) Apple's machine identification */
		uint32_t       machineID;
	
		/* NEW (vers. 4) Detected hw architecture */
		uint32_t       architecture;
		
		/* The device tree (internal addresses relative to the beginning of the tree,
		* device tree offset relative to the beginning of this structure).
		* On pre-PCI macintosh (BOOT_ARCH_PCI bit set to 0 in architecture), this
		* field is 0.
		*/
		uint32_t       deviceTreeOffset;        /* Device tree offset */
		uint32_t       deviceTreeSize;          /* Size of the device tree */
			
		/* Some infos about the current MacOS display */
		uint32_t       dispDeviceRect[4];       /* left,top,right,bottom */
		uint32_t       dispDeviceDepth;         /* (8, 16 or 32) */
		uint32_t       dispDeviceBase;          /* base address (physical) */
		uint32_t       dispDeviceRowBytes;      /* rowbytes (in bytes) */
		uint32_t       dispDeviceColorsOffset;  /* Colormap (8 bits only) or 0 (*) */
		/* Optional offset in the registry to the current
		* MacOS display. (Can be 0 when not detected) */
		uint32_t      dispDeviceRegEntryOffset;
	
		/* Optional pointer to boot ramdisk (offset from this structure) */
		uint32_t       ramDisk;
		uint32_t       ramDiskSize;             /* size of ramdisk image */
		
		/* Kernel command line arguments (offset from this structure) */
		uint32_t       kernelParamsOffset;
		
		/* ALL BELOW NEW (vers. 4) */
		
		/* This defines the physical memory. Valid with BOOT_ARCH_NUBUS flag
		(non-PCI) only. On PCI, memory is contiguous and it's size is in the
		device-tree. */
		BootInfoMapEntry physMemoryMap[MAX_MEM_MAP_SIZE]; /* Where the phys memory is */
		uint32_t       physMemoryMapSize;               /* How many entries in map */
		
					
		/* The framebuffer size (optional, currently 0) */
		uint32_t       frameBufferSize;         /* Represents a max size, can be 0. */
		
		/* NEW (vers. 5) */
	
		/* Total params size (args + colormap + device tree + ramdisk) */
		uint32_t       totalParamsSize;
	
	} BootInfosImage;
};

class PMACBootX :
	public Service<Loader>,
	public Service<Blob<uint32_t> >,
	public Client<Loader>,
	public Client<Blob<uint32_t> >,
	public Client<Memory<uint32_t> >,
	public Client<Registers>
{
public:
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<uint32_t> > blob_export;

	ServiceImport<Loader> loader_import;
	ServiceImport<Blob<uint32_t> > blob_import;
	ServiceImport<Memory<uint32_t> > memory_import;
	ServiceImport<Registers> registers_import;
	
	PMACBootX(const char *name, Object *parent = 0);
	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();
	virtual const unisim::util::blob::Blob<uint32_t> *GetBlob() const;
	virtual bool Load();

private:
	unisim::kernel::logger::Logger logger;
	string device_tree_filename;
	string kernel_parms;
	string ramdisk_filename;
	unsigned int screen_width;
	unsigned int screen_height;
	bool verbose;
	unisim::util::blob::Blob<uint32_t> *blob;
	
	Parameter<string> param_device_tree_filename;
	Parameter<string> param_kernel_parms;
	Parameter<string> param_ramdisk_filename;
	Parameter<unsigned int> param_screen_width;
	Parameter<unsigned int> param_screen_height;
	Parameter<bool> param_verbose;

	bool SetupBlob();
	bool SetupLoad();
	bool LoadBootInfosAndRegisters();
};

} // end of namesapce pmac_bootx
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
