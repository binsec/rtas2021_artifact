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
 
#include <unisim/service/loader/pmac_bootx/pmac_bootx.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/register.hh>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

namespace unisim {
namespace service {
namespace loader {
namespace pmac_bootx {

using namespace std;
using namespace unisim::util::endian;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Register;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

static uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

static bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

DeviceProperty *DeviceTree::CreateDeviceProperty(const char *name, const uint8_t *value, int32_t len)
{
	DeviceProperty *reloc_prop = (DeviceProperty *) Relocate(Malloc(sizeof(DeviceProperty)));
	reloc_prop->name = Host2BigEndian(Malloc(strlen(name) + 1));
	strcpy((char *) Relocate(BigEndian2Host(reloc_prop->name)), name);
	reloc_prop->value = len > 0 ? Host2BigEndian(Malloc(len)) : 0;
	if(value) memcpy(Relocate(BigEndian2Host(reloc_prop->value)), value, len);
	reloc_prop->length = Host2BigEndian(len);
	reloc_prop->next = 0;
	return reloc_prop;
}

unsigned int DeviceTree::ExpandEscapeSequences(const string& s, char *buffer)
{
	unsigned int i, len;
	unsigned int state = 0;
	for(i = 0, len = 0; i <= s.length(); i++)
	{
		char c = s[i];
		
		switch(state)
		{
			case 0:
				if(c == '\\')
				{
					state = 1;
					break;
				}
				buffer[len++] = c;
				break;
			case 1:
				if(c == '0')
				{
					buffer[len++] = 0;
					state = 0;
					break;
				}
				if(c == 'n')
				{
					buffer[len++] = '\n';
					state = 0;
					break;
				}
				if(c == 't')
				{
					buffer[len++] = '\t';
					state = 0;
					break;
				}
				if(c == 'r')
				{
					buffer[len++] = '\r';
					state = 0;
					break;
				}
				state = 0;
				break;
		}
	}
	buffer[len++] = 0;
	return len;
}

DeviceProperty *DeviceTree::CreateDeviceProperty(const unisim::util::xml::Node *xml_node)
{
	if(xml_node->Name() == string("string_property"))
	{
		string name;
		string value;

		const list<unisim::util::xml::Property *> *xml_props = xml_node->Properties();
		list<unisim::util::xml::Property *>::const_iterator xml_prop;

		for(xml_prop = xml_props->begin(); xml_prop != xml_props->end(); xml_prop++)
		{
			if((*xml_prop)->Name() == string("name"))
			{
				name = (*xml_prop)->Value();
			}
			else
			{
				if((*xml_prop)->Name() == string("value"))
				{
					value = (*xml_prop)->Value();
				}
				else
				{
					logger << DebugWarning << (*xml_prop)->Filename() << ":" << (*xml_prop)->LineNo() << ":ignoring property " << (*xml_prop)->Name() << " of tag " << xml_node->Name() << EndDebugWarning;
				}
			}
		}

		char buffer[value.length() + 1];
		unsigned int len = ExpandEscapeSequences(value, buffer);
		
		return CreateDeviceProperty(name.c_str(), (uint8_t *) buffer, len);
	}
	else
	{
		if(xml_node->Name() == string("hex_property"))
		{
			string name;
			string value;

			const list<unisim::util::xml::Property *> *xml_props = xml_node->Properties();
			list<unisim::util::xml::Property *>::const_iterator xml_prop;
	
			for(xml_prop = xml_props->begin(); xml_prop != xml_props->end(); xml_prop++)
			{
				if((*xml_prop)->Name() == string("name"))
				{
					name = (*xml_prop)->Value();
				}
				else
				{
					if((*xml_prop)->Name() == string("value"))
					{
						string hex_value = (*xml_prop)->Value();
						uint32_t number_start = 0;
						uint32_t bitcount = 0;
						uint32_t i;
						char c;
						uint32_t offset;

						for(i = 0; i < hex_value.length();)
						{
							c = hex_value[i];
			
							// ignoring something not hexadecimal character
							if(IsHexChar(c)) 
							{
								bitcount += 4;
								if(bitcount == 8) bitcount = 0;
								i++;
							}
							else
							{
								if(bitcount)
								{
									// inserting a leading 0
									hex_value.insert(number_start, "0");
									// so move 'i' from one position to right
									i++;
									bitcount = 0;
								}
								// remove character at position 'i'
								hex_value.erase(i,1);
								number_start = i;
							}
						}

						if(bitcount)
						{
							// inserting a leading 0
							hex_value.insert(number_start, "0");
						}
						// at this point hex_value.length() must be an even number

						uint32_t value_length = hex_value.length() / 2;
						uint8_t value[value_length];
					
						for(offset = 0, i = 0; offset < value_length; i += 2, offset++)
						{
							value[offset] = (HexChar2Nibble(hex_value[i]) << 4) | HexChar2Nibble(hex_value[i+1]);
						}

						return CreateDeviceProperty(name.c_str(), value, value_length);
					}
					else
					{
						logger << DebugWarning << (*xml_prop)->Filename() << ":" << (*xml_prop)->LineNo() << ":ignoring property " << (*xml_prop)->Name() << " of tag " << xml_node->Name() << EndDebugWarning;
					}
				}
			}
		}
	}
	return 0;
}

void DeviceTree::DumpDeviceProperty(DeviceProperty *reloc_prop)
{
	int32_t length, i, j;
	uint8_t *value;
	fprintf(stderr, "\"%s\"\n", (char *) Relocate(BigEndian2Host(reloc_prop->name)));
	length = BigEndian2Host(reloc_prop->length);
	value = (uint8_t *) Relocate(BigEndian2Host(reloc_prop->value));
	if(!value)
	{
		fprintf(stderr, "?\n");
		return;
	}
	for(i = 0; i < length; i += 16)
	{
		for(j = 0; j < 16 && (i + j < length); j++)
		{
			fprintf(stderr, "%02x", value[i + j]);
			if(j < 15) fprintf(stderr, " ");
		}
		for(; j < 16; j++)
		{
			fprintf(stderr, "  ");
			if(j < 15) fprintf(stderr, " ");
		}
		fprintf(stderr, "    ");
		for(j = 0; j < 16 && (i + j < length); j++)
		{
			uint8_t c = value[i + j];
			fprintf(stderr, "%c", (c >= 32 && c < 128) ? c : '.');
		}
		fprintf(stderr, "\n");
	}
}

DeviceNode *DeviceTree::CreateDeviceNode(DeviceNode *reloc_parent, const unisim::util::xml::Node *xml_node)
{
	DeviceNode *reloc_node = 0;

	if(xml_node->Name() == string("devnode"))
	{
		reloc_node = (DeviceNode *) Relocate(Malloc(sizeof(DeviceNode)));
		reloc_node->name = 0;
		reloc_node->type = 0;
		reloc_node->node = 0;
		reloc_node->n_addrs = 0;
		reloc_node->addrs = 0;
		reloc_node->n_intrs = 0;
		reloc_node->intrs = 0;
		reloc_node->full_name = 0;
		reloc_node->properties = 0;
		reloc_node->parent = Host2BigEndian(UnRelocate(reloc_parent));
		reloc_node->child = 0;
		reloc_node->sibling = 0;
		reloc_node->next = 0;	/* next device of same type */
		if(last_reloc_node)
		{
			last_reloc_node->allnext = Host2BigEndian(UnRelocate(reloc_node));
		}
		reloc_node->allnext = 0; /* next in list of all nodes */
	
		last_reloc_node = reloc_node;

		if(!reloc_parent)
		{
			const char *name = "/";
			reloc_node->full_name = Host2BigEndian(Malloc(strlen(name) + 1));
			strcpy((char *) Relocate(BigEndian2Host(reloc_node->full_name)), name);
		}

		list<unisim::util::xml::Node *>::const_iterator xml_child;
		const list<unisim::util::xml::Node *> *xml_childs = xml_node->Childs();
	
		// Check XML child nodes
		for(xml_child = xml_childs->begin(); xml_child != xml_childs->end(); xml_child++)
		{
			if((*xml_child)->Name() != string("devnode") && (*xml_child)->Name() != string("string_property") && (*xml_child)->Name() != string("hex_property"))
			{
				logger << DebugWarning << (*xml_child)->Filename() << ":" << (*xml_child)->LineNo() << ":expected either tag devnode/string_property/hex_property, ignoring tag " << (*xml_child)->Name() << EndDebugWarning;
			}
		}

		// Check XML node properties
		list<unisim::util::xml::Property *>::const_iterator xml_prop;
		const list<unisim::util::xml::Property *> *xml_props = xml_node->Properties();

		if(!xml_props->empty())
		{
			logger << DebugWarning << (*xml_prop)->Filename() << ":" << (*xml_prop)->LineNo() << ":ignoring properties of tag " << xml_node->Name() << EndDebugWarning;
		}

		// Create Device properties and build a full name for the device node
		for(xml_child = xml_childs->begin(); xml_child != xml_childs->end(); xml_child++)
		{
			if((*xml_child)->Name() == string("string_property") || (*xml_child)->Name() == string("hex_property"))
			{
				DeviceProperty *reloc_prop = CreateDeviceProperty(*xml_child);
	
				if(reloc_prop)
				{
					if(strcmp((char *) Relocate(BigEndian2Host(reloc_prop->name)), "name") == 0)
					{
						// the value of this device property is the name of the device node
						
						reloc_node->name = reloc_prop->value;
						char *reloc_name = (char *) Relocate(BigEndian2Host(reloc_node->name));
	
						if(reloc_parent)
						{
							const char *reloc_parent_full_name = strcmp((char *) Relocate(BigEndian2Host(reloc_parent->full_name)), "/") == 0 ? "" : (char *) Relocate(BigEndian2Host(reloc_parent->full_name));
							uint32_t parent_full_name_length = strlen(reloc_parent_full_name) + 1;
							uint32_t name_length = strlen(reloc_name) + 1;
							reloc_node->full_name = Host2BigEndian(Malloc(parent_full_name_length + name_length));
							sprintf((char *) Relocate(BigEndian2Host(reloc_node->full_name)), "%s/%s", reloc_parent_full_name, reloc_name);
						}
					}
	
					DeviceProperty *reloc_pp;
					DeviceProperty *reloc_prev;
	
					reloc_pp = (DeviceProperty *) Relocate(BigEndian2Host(reloc_node->properties));
					reloc_prev = 0;
			
					if(reloc_pp)
					{
						do
						{
							reloc_prev = reloc_pp;
						} while((reloc_pp = (DeviceProperty *) Relocate(BigEndian2Host(reloc_pp->next))) != 0);
					}
					
					if(reloc_prev)
					{
						reloc_prev->next = Host2BigEndian(UnRelocate(reloc_prop));
					}
					else
					{
						reloc_node->properties = Host2BigEndian(UnRelocate(reloc_prop));
					}
				}
			}
		}

		// Create child device nodes
		for(xml_child = xml_childs->begin(); xml_child != xml_childs->end(); xml_child++)
		{
			if((*xml_child)->Name() == string("devnode"))
			{
				DeviceNode *reloc_child = CreateDeviceNode(reloc_node, *xml_child);

				DeviceNode *reloc_sibling = (DeviceNode *) Relocate(BigEndian2Host(reloc_node->child));
				DeviceNode *reloc_prev = 0;
					
				if(reloc_sibling)
				{
					do
					{
						reloc_prev = reloc_sibling;
					} while((reloc_sibling = (DeviceNode *) Relocate(BigEndian2Host(reloc_sibling->sibling))) != 0);
				}
				
				if(reloc_prev)
				{
					reloc_prev->sibling = Host2BigEndian(UnRelocate(reloc_child));
				}
				else
				{
					reloc_node->child = Host2BigEndian(UnRelocate(reloc_child));
				}
			}
		}
	}

	return reloc_node;
}

void DeviceTree::DumpDeviceNode(DeviceNode *reloc_node)
{
	DeviceNode *reloc_child;
	DeviceProperty *reloc_prop;
	
	fprintf(stderr, "Node %s\n", (char *) Relocate(BigEndian2Host(reloc_node->full_name)));
	
	reloc_prop = (DeviceProperty *) Relocate(BigEndian2Host(reloc_node->properties));
	
	if(reloc_prop)
	{
		fprintf(stderr, "Properties of %s\n", (char *) Relocate(BigEndian2Host(reloc_node->full_name)));
		do
		{
			DumpDeviceProperty(reloc_prop);
		} while((reloc_prop = (DeviceProperty *) Relocate(BigEndian2Host(reloc_prop->next))) != 0);
	}
	
	reloc_child = (DeviceNode *) Relocate(BigEndian2Host(reloc_node->child));
	
	if(reloc_child)
	{
		do
		{
			DumpDeviceNode(reloc_child);
		} while((reloc_child = (DeviceNode *) Relocate(BigEndian2Host(reloc_child->sibling))) != 0);
	}	
}


DeviceTree::DeviceTree(BootInfos *_boot_infos, unisim::kernel::logger::Logger& _logger, bool _verbose)
	: logger(_logger)
	, boot_infos(_boot_infos)
	, root_node(0)
	, last_reloc_node(0)
	, base(0)
	, size(0)
	, verbose(_verbose)
{
}

DeviceTree::~DeviceTree()
{
}

uint32_t DeviceTree::Malloc(uint32_t size)
{
	this->size += size;
	return boot_infos->Malloc(size) - base;
}

void *DeviceTree::Relocate(uint32_t offset)
{
	return offset ? boot_infos->Relocate(base + offset) : 0;
}

uint32_t DeviceTree::UnRelocate(void *p)
{
	return p ? boot_infos->UnRelocate((uint8_t *) p - base) : 0;
}

uint32_t DeviceTree::GetBase()
{
	return base;
}

uint32_t DeviceTree::GetSize()
{
	return size;
}

bool DeviceTree::Load(const string& filename)
{
	base = boot_infos->Malloc(4); // first 4 bytes must be set to zero

	unisim::util::xml::Parser *parser = new unisim::util::xml::Parser(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream());
	unisim::util::xml::Node *xml_node;

	xml_node = parser->Parse(filename);
	delete parser;

	if(xml_node)
	{
		//cerr << *xml_node << endl;
		if(xml_node->Name() == string("devnode"))
		{
			root_node = CreateDeviceNode(0 /* no parent */, xml_node);
			//DumpDeviceNode(root_node);
		}
		else
		{
			logger << DebugWarning << xml_node->Filename() << ":" << xml_node->LineNo() << ":expected tag devnode, got " << xml_node->Name() << EndDebugWarning;
		}
		delete xml_node;
	}

	return root_node != 0;
}

DeviceNode *DeviceTree::FindDevice(const char *prop_name, const char *prop_string_value)
{
	DeviceNode *reloc_node;

	for(reloc_node = root_node; reloc_node; reloc_node = (DeviceNode *) Relocate(BigEndian2Host(reloc_node->allnext)))
	{
		DeviceProperty *reloc_prop;
		for(reloc_prop = (DeviceProperty *) Relocate(BigEndian2Host(reloc_node->properties)); reloc_prop; reloc_prop = (DeviceProperty *) Relocate(BigEndian2Host(reloc_prop->next)))
		{
			if(strcmp((char *) Relocate(BigEndian2Host(reloc_prop->name)), prop_name) == 0)
			{
				uint32_t prop_len = BigEndian2Host(reloc_prop->length);
				if(strncmp(( char *) Relocate(BigEndian2Host(reloc_prop->value)), prop_string_value, prop_len - 1) == 0)
				{
					return reloc_node;
				}
			}
		}
	}
	return 0;
}

uint8_t *DeviceTree::GetProperty(DeviceNode *reloc_node, const char *name, int32_t& len)
{
	DeviceProperty *reloc_prop;
	for(reloc_prop = (DeviceProperty *) Relocate(BigEndian2Host(reloc_node->properties)); reloc_prop; reloc_prop = (DeviceProperty *) Relocate(BigEndian2Host(reloc_prop->next)))
	{
		if(strcmp((char *) Relocate(BigEndian2Host(reloc_prop->name)), name) == 0)
		{
			len = BigEndian2Host(reloc_prop->length);
			return (uint8_t *) Relocate(BigEndian2Host(reloc_prop->value));
		}
	}
	return 0;
}

BootInfos::BootInfos(unisim::kernel::logger::Logger& _logger, bool _verbose)
	: logger(_logger)
	, verbose(_verbose)
	, size(0)
	, max_size(MAX_BOOT_INFOS_IMAGE_SIZE)
	, image(0)
	, blob(0)
{
}

BootInfos::~BootInfos()
{
	if(image)
	{
		free(image);
	}
	
	if(blob)
	{
		blob->Release();
	}
}

uint32_t BootInfos::Malloc(uint32_t size)
{
	if(!image)
	{
		image = (uint8_t *) malloc(max_size);
	}
	uint32_t ret;
	
	if(size > 0)
	{
		size = (size + 3) & 0xfffffffcUL;
		if(this->size + size > max_size)
		{
			logger << DebugError << "PANIC! Boot info image is too big: you should decrease the size of your initial ramdisk" << EndDebugError;
			abort();
		}
	/*	if(this->size + size > max_size)
		{
			max_size = max_size * 2 > this->size + size ? max_size * 2 : this->size + size;
			image = (uint8_t *) realloc(image, max_size);
		}*/
		ret = this->size;
		memset(image + ret, 0, size);
		this->size += size;
		if(ret & 3) abort();
	}
	else
	{
		ret = this->size;
	}
	return ret;
}

void *BootInfos::Relocate(uint32_t offset)
{
	return offset ? image + offset : 0;
}

uint32_t BootInfos::UnRelocate(void *p)
{
	return p ? ((uint8_t *) p) - image : 0;
}

bool BootInfos::Load(uint32_t boot_infos_addr, const string& device_tree_filename, const string& kernel_parms, const string& ramdisk_filename, unsigned int screen_width, unsigned int screen_height)
{
	if(image)
	{
		free(image);
		image = 0;
	}
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	
	unsigned int i;
	DeviceTree device_tree(this, logger, verbose);
	BootInfosImage *boot_infos;
	//char *kernel_parms = "";

	Malloc(sizeof(BootInfosImage));
	boot_infos = (BootInfosImage *) image;

	uint32_t kernel_parms_offset = Malloc(kernel_parms.length() + 1);
	char *kernel_parms_image = (char *) Relocate(kernel_parms_offset);
	strcpy(kernel_parms_image, kernel_parms.c_str());
	
	if(!ramdisk_filename.empty())
	{
		ifstream f(ramdisk_filename.c_str(), ifstream::in | ifstream::binary);
		
		if(f.fail())
		{
			logger << DebugError << "Can't load initial ramdisk from file \"" << ramdisk_filename << "\"" << EndDebugError;
			return false;
		}
		
		if(f.seekg(0, ios::end).fail())
		{
			logger << DebugError << "Can't seek into file \"" << ramdisk_filename << "\"" << EndDebugError;
			return false;
		}

		uint32_t ramdisk_size = f.tellg();
		
		if(f.seekg(0, ios::beg).fail())
		{
			logger << DebugError << "Can't seek into file \"" << ramdisk_filename << "\"" << EndDebugError;
			return false;
		}

		uint32_t ramdisk_offset = Malloc(0);
		uint32_t size_to_align_on_page_boundary = ((ramdisk_offset + 4095) & 0xfffff000UL) - ramdisk_offset;
		Malloc(size_to_align_on_page_boundary);
		ramdisk_offset = Malloc(ramdisk_size);
		if(verbose)
		{
			logger << DebugInfo << "Loading ramdisk at offset " << hex << ramdisk_offset << " of boot infos" << dec << EndDebugInfo;
		}
		
		char *ramdisk = (char *) Relocate(ramdisk_offset);
		
		if(f.read(ramdisk, ramdisk_size).fail())
		{
			logger << DebugError << "Can't load ramdisk file \"" << ramdisk_filename << "\"" << EndDebugError;
			return false;
		}
	
		boot_infos->ramDisk = Host2BigEndian(ramdisk_offset);
		boot_infos->ramDiskSize = Host2BigEndian(ramdisk_size);
	}
	else
	{
		boot_infos->ramDisk = 0;
		boot_infos->ramDiskSize = 0;
	}

	if(!device_tree.Load(device_tree_filename))
	{
		logger << DebugError << "Can't load device tree from file \"" << device_tree_filename << "\"" << EndDebugError;
		return false;
	}

	DeviceNode *display_device = device_tree.FindDevice("device_type", "display");

	if(display_device)
	{
		int32_t len;
		uint8_t *address_prop = device_tree.GetProperty(display_device, "address", len);

		if(address_prop)
		{
			boot_infos->logicalDisplayBase = boot_infos->dispDeviceBase = *(uint32_t *) address_prop;
		}
		else
		{
			logger << DebugWarning << "Can't find display device address in device tree" << EndDebugWarning;
		}

		uint8_t *depth_prop = device_tree.GetProperty(display_device, "depth", len);

		if(depth_prop)
		{
			boot_infos->dispDeviceDepth = *(uint32_t *) depth_prop;
		}
		else
		{
			logger << DebugWarning << "Can't find display device depth in device tree" << EndDebugWarning;
		}

		if(screen_width)
		{
			boot_infos->dispDeviceRect[2] = Host2BigEndian((uint32_t) screen_width); /* right */
		}
		else
		{
			uint8_t *width_prop = device_tree.GetProperty(display_device, "width", len);
	
			if(width_prop)
			{
				boot_infos->dispDeviceRect[2] = *(uint32_t *) width_prop; /* right */
			}
			else
			{
				logger << DebugWarning << "Can't find display device width in device tree" << EndDebugWarning;
			}
		}

		if(screen_height)
		{
			boot_infos->dispDeviceRect[3] = Host2BigEndian((uint32_t) screen_height); /* bottom */
		}
		else
		{
			uint8_t *height_prop = device_tree.GetProperty(display_device, "height", len);
	
			if(height_prop)
			{
				boot_infos->dispDeviceRect[3] = *(uint32_t *) height_prop; /* bottom */
			}
			else
			{
				logger << DebugWarning << "Can't find display device height in device tree" << EndDebugWarning;
			}
		}

		if(screen_width && depth_prop)
		{
			boot_infos->dispDeviceRowBytes = Host2BigEndian(screen_width * ((BigEndian2Host(*(uint32_t *) depth_prop) + 7) / 8));
		}
		else
		{
			uint8_t *linebytes_prop = device_tree.GetProperty(display_device, "linebytes", len);
	
			if(linebytes_prop)
			{
				boot_infos->dispDeviceRowBytes = *(uint32_t *) linebytes_prop;
			}
			else
			{
				logger << DebugWarning << "Can't find display device bytes per scan line in device tree" << EndDebugWarning;
			}
		}
	}
	else
	{
		logger << DebugWarning << "Can't find display device in device tree" << EndDebugWarning;
	}

	boot_infos->version = Host2BigEndian(BOOT_INFO_VERSION);
	boot_infos->compatible_version = Host2BigEndian(BOOT_INFO_COMPATIBLE_VERSION);
	boot_infos->machineID = 0; /* ? */
	boot_infos->architecture = Host2BigEndian(BOOT_ARCH_PCI); /* ? */
	boot_infos->deviceTreeOffset = Host2BigEndian(device_tree.GetBase());// + kernel_parms_size;
	boot_infos->deviceTreeSize = Host2BigEndian(device_tree.GetSize()); /* ? */
// 	boot_infos->ramDisk = 0;
// 	boot_infos->ramDiskSize = 0;

	boot_infos->dispDeviceRect[0] = 0; /* left */
	boot_infos->dispDeviceRect[1] = 0; /* top */
	boot_infos->dispDeviceColorsOffset = 0;
	boot_infos->dispDeviceRegEntryOffset = 0; /* ? may be (char *) display_device - (char *) boot_infos*/
	boot_infos->kernelParamsOffset = Host2BigEndian(kernel_parms_offset); /* ? */
	for(i = 0; i < MAX_MEM_MAP_SIZE; i++)
	{
		boot_infos->physMemoryMap[i].physAddr = 0;
		boot_infos->physMemoryMap[i].size = 0;
	}
	boot_infos->physMemoryMapSize = 0;
	boot_infos->frameBufferSize = 0;
	boot_infos->totalParamsSize = Host2BigEndian(size);
	
	blob = new unisim::util::blob::Blob<uint32_t>();
	blob->Catch();
	
	unisim::util::blob::Section<uint32_t> *section = new unisim::util::blob::Section<uint32_t>(
		unisim::util::blob::Section<uint32_t>::TY_UNKNOWN,
		unisim::util::blob::Section<uint32_t>::SA_A,
		"boot_infos",
		0,
		0,
		boot_infos_addr,
		size,
		image
	);
	
	blob->AddSection(section);
	image = 0; // allocated memory is now managed by the section, and section is managed by the blob

	return true;
}

const unisim::util::blob::Blob<uint32_t> *BootInfos::GetBlob() const
{
	return blob;
}

PMACBootX::PMACBootX(const char *name, Object *parent) :
	Object(name, parent, "This service is a PowerMac BootX loader emulator. It allows bootloading a PowerMac Linux kernel with its initial ramdisk and device tree"),
	Service<Loader>(name, parent),
	Service<Blob<uint32_t> >(name, parent),
	Client<Loader>(name, parent),
	Client<Blob<uint32_t> >(name, parent),
	Client<Memory<uint32_t> >(name, parent),
	Client<Registers>(name, parent),
	loader_export("loader-export", this),
	blob_export("blob-export", this),
	loader_import("loader-import", this),
	blob_import("blob-import", this),
	memory_import("memory-import", this),
	registers_import("cpu-registers-import", this),
	logger(*this),
	device_tree_filename(),
	kernel_parms(),
	ramdisk_filename(),
	screen_width(0),
	screen_height(0),
	verbose(false),
	blob(0),
	param_device_tree_filename("device-tree-filename", this, device_tree_filename, "device tree file name of simulated PowerMac machine"),
	param_kernel_parms("kernel-params", this, kernel_parms, "Linux kernel parameters"),
	param_ramdisk_filename("ramdisk-filename", this, ramdisk_filename, "initial ramdisk filename (either compressed with gzip or uncompressed)"),
	param_screen_width("screen-width", this, screen_width, "screen width in pixels"),
	param_screen_height("screen-height", this, screen_height, "screen height in pixels"),
	param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	loader_export.SetupDependsOn(blob_import);
	loader_export.SetupDependsOn(loader_import);
	loader_export.SetupDependsOn(memory_import);
	loader_export.SetupDependsOn(registers_import);
	
	blob_export.SetupDependsOn(blob_import);
}

void PMACBootX::OnDisconnect()
{
}

bool PMACBootX::BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	
	return true;
}

bool PMACBootX::SetupBlob()
{
	if(blob) return true;
	if(!blob_import)
	{
		logger << DebugError << "no kernel loader connected" << EndDebugError;
		return false;
	}
	
	const unisim::util::blob::Blob<uint32_t> *kernel_blob = blob_import->GetBlob();
	
	if(!kernel_blob)
	{
		logger << DebugError << "Can't get kernel blob" << EndDebugError;
		return false;
	}

	uint32_t kernel_start_addr;
	uint32_t kernet_end_addr;
	
	kernel_blob->GetAddrRange(kernel_start_addr, kernet_end_addr);
	
	uint32_t boot_infos_addr = (kernet_end_addr + 4095) & 0xfffff000; // align boot info to a page boundary
	uint32_t stack_base = 0x400000; // under first 512 KB

	BootInfos boot_infos(logger, verbose);

	if(verbose)
	{
		logger << DebugInfo << "Using device tree from file \"" << device_tree_filename << "\"" << EndDebugInfo;
		logger << DebugInfo << "Linux kernel parameters are \"" << kernel_parms << "\"" << EndDebugInfo;
		logger << DebugInfo << "Using ramdisk from image \"" << ramdisk_filename << "\"" << EndDebugInfo;
	}
	
	const unisim::util::blob::Blob<uint32_t> *boot_infos_blob = 0;

	if(!boot_infos.Load(boot_infos_addr, GetSimulator()->SearchSharedDataFile(device_tree_filename.c_str()).c_str(), kernel_parms, GetSimulator()->SearchSharedDataFile(ramdisk_filename.c_str()).c_str(), screen_width, screen_height) || !(boot_infos_blob = boot_infos.GetBlob()))
	{
		logger << DebugError << "Error while bootloading kernel, initial ramdisk and device tree" << EndDebugError;
		return false;
	}

	blob = new unisim::util::blob::Blob<uint32_t>();
	blob->Catch();
	
	blob->SetArchitecture("powerpc");
	blob->SetStackBase(stack_base);
	
	blob->AddBlob(boot_infos_blob);
	blob->AddBlob(kernel_blob);
	
	return true;
}

bool PMACBootX::SetupLoad()
{
	if(!SetupBlob()) return false;
	if(!blob) return false;
	
	if(!loader_import)
	{
		logger << DebugError << "No loader connected" << EndDebugError;
		return false;
	}
	
	if(!memory_import)
	{
		logger << DebugError << "No memory connected" << EndDebugError;
		return false;
	}

	if(!registers_import)
	{
		logger << DebugError << "No CPU connected" << EndDebugError;
		return false;
	}
	
	return true;
}

bool PMACBootX::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &blob_export) return SetupBlob();
	if(srv_export == &loader_export) return SetupLoad();
	
	logger << DebugError << "Internal error" << EndDebugError;
	return false;
}

bool PMACBootX::EndSetup()
{
	return LoadBootInfosAndRegisters();
}

bool PMACBootX::LoadBootInfosAndRegisters()
{
	if(!blob) return false;
	
	const unisim::util::blob::Section<uint32_t> *boot_infos_section = blob->FindSection("boot_infos");
	if(!boot_infos_section)
	{
		logger << DebugError << "No boot_infos section found" << EndDebugError;
		return false;
	}

	uint32_t entry_point = blob->GetEntryPoint();
	uint32_t r1_value = blob->GetStackBase() - 4;
	uint32_t r3_value = 0x426f6f58UL; /* 'BooX' */
	uint32_t r4_value = boot_infos_section->GetAddr();
	uint32_t r5_value = 0; /* NULL */
	
	if(verbose)
	{
		logger << DebugInfo << "Writing boot infos (" << boot_infos_section->GetSize() << " bytes) at 0x" << hex << boot_infos_section->GetAddr() << dec << EndDebugInfo;
	}
	
	if(boot_infos_section->GetSize())
	{
		if(!memory_import || !memory_import->WriteMemory(boot_infos_section->GetAddr(), boot_infos_section->GetData(), boot_infos_section->GetSize()))
		{
			logger << DebugError << "Can't write into memory (@0x" << hex << boot_infos_section->GetAddr() << " - @0x" << (boot_infos_section->GetAddr() +  boot_infos_section->GetSize() - 1) << dec << ")" << EndDebugError;
			return false;
		}
	}

	unisim::service::interfaces::Register *cia = registers_import->GetRegister("cia");
	if(!cia)
	{
		logger << DebugError << "Register \"cia\" does not exist" << EndDebugError;
		return false;
	}
	cia->SetValue(&entry_point);
	
	unisim::service::interfaces::Register *r1 = registers_import->GetRegister("r1");
	if(!r1)
	{
		logger << DebugError << "Register \"r1\" does not exist" << EndDebugError;
		return false;
	}
	r1->SetValue(&r1_value);
	
	unisim::service::interfaces::Register *r3 = registers_import->GetRegister("r3");
	if(!r3)
	{
		logger << DebugError << "Register \"r3\" does not exist" << EndDebugError;
		return false;
	}
	r3->SetValue(&r3_value);
	
	unisim::service::interfaces::Register *r4 = registers_import->GetRegister("r4");
	if(!r4)
	{
		logger << DebugError << "Register \"r4\" does not exist" << EndDebugError;
		return false;
	}
	r4->SetValue(&r4_value);
	
	unisim::service::interfaces::Register *r5 = registers_import->GetRegister("r5");
	if(!r5)
	{
		logger << DebugError << "Register \"r5\" does not exist" << EndDebugError;
		return false;
	}
	r5->SetValue(&r5_value);
	
	return true;
}

bool PMACBootX::Load()
{
	if(!loader_import) return false;
	return loader_import->Load() && LoadBootInfosAndRegisters();
}

const unisim::util::blob::Blob<uint32_t> *PMACBootX::GetBlob() const
{
	return blob;
}

} // end of namespace pmac_bootx
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim
