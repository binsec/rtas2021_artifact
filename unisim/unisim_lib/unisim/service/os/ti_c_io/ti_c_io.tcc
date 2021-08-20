/*
 *  Copyright (c) 2009,
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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_TCC__
#define __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_TCC__

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/likely/likely.hh"

namespace unisim {
namespace service {
namespace os {
namespace ti_c_io {

using std::string;
using std::stringstream;
using std::hex;
using std::dec;
using std::endl;
using std::flush;
using std::map;
using unisim::kernel::Service;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::SymbolTableLookup;

template <class MEMORY_ADDR>
TI_C_IO<MEMORY_ADDR>::TI_C_IO(const char *name, Object *parent) :
	Object(name, parent),
	Service<unisim::service::interfaces::TI_C_IO>(name, parent),
	Client<Memory<MEMORY_ADDR> >(name, parent),
	Client<MemoryInjection<MEMORY_ADDR> >(name, parent),
	Client<SymbolTableLookup<MEMORY_ADDR> >(name, parent),
	Client<Registers>(name, parent),
	Client<Blob<MEMORY_ADDR> >(name, parent),
	ti_c_io_export("ti-c-io-export", this),
	memory_import("memory-import", this),
	memory_injection_import("memory-injection-import", this),
	registers_import("registers-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	blob_import("blob-import", this),
	logger(*this),
	c_io_buffer_addr(0),
	c_exit_breakpoint_addr(0),
	max_buffer_byte_length(0),
	buffer(0),
	enable(false),
	param_enable("enable", this, enable, "enable/disable TI C I/O support"),
	reg_pc(0),
	reg_sp(0),
	warning_as_error(false),
	pc_register_name("PC"),
	sp_register_name("SP"),
	c_io_buffer_symbol_name("__CIOBUF_"),
	c_io_breakpoint_symbol_name("C$$IO$$"),
	c_exit_breakpoint_symbol_name("C$$EXIT"),
	param_warning_as_error("warning-as-error", this, warning_as_error, "Whether Warnings are considered as error or not"),
	param_pc_register_name("pc-register-name", this, pc_register_name, "Name of the CPU program counter register"),
	param_c_io_buffer_symbol_name("c-io-buffer-symbol-name", this, c_io_buffer_symbol_name, "C I/O buffer symbol name"),
	param_c_io_breakpoint_symbol_name("c-io-breakpoint-symbol-name", this, c_io_breakpoint_symbol_name, "C I/O breakpoint symbol name"),
	param_c_exit_breakpoint_symbol_name("c-exit-breakpoint-symbol-name", this, c_exit_breakpoint_symbol_name, "C EXIT breakpoint symbol name"),
	verbose_all(false),
	verbose_io(false),
	verbose_setup(false),
	enable_lseek_bug(false),
	param_verbose_all("verbose-all", this, verbose_all, "globally enable/disable verbosity"),
	param_verbose_io("verbose-io", this, verbose_io, "enable/disable verbosity while I/Os"),
	param_verbose_setup("verbose-setup", this, verbose_setup, "enable/disable verbosity while setup"),
	param_enable_lseek_bug("enable-lseek-bug", this, enable_lseek_bug, "enable/disable lseek bug (as code composer)")
{
}

template <class MEMORY_ADDR>
TI_C_IO<MEMORY_ADDR>::~TI_C_IO()
{
	if(buffer) free(buffer);
}

template <class MEMORY_ADDR>
void TI_C_IO<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
bool TI_C_IO<MEMORY_ADDR>::EndSetup()
{
	return true;
}

template <class MEMORY_ADDR>
bool TI_C_IO<MEMORY_ADDR>::LoadMemoryAndRegisters()
{
	target_to_host_fildes.clear();
	
	if(enable)
	{
		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "TI C I/O support is enabled" << EndDebugInfo;
		}

		if(!memory_injection_import)
		{
			logger << DebugError << memory_injection_import.GetName() << " is not connected" << EndDebugError;
			enable = false;
			return false;
		}

		if(!registers_import)
		{
			logger << DebugError << registers_import.GetName() << " is not connected" << EndDebugError;
			enable = false;
			return false;
		}

		if(!symbol_table_lookup_import)
		{
			logger << DebugError << symbol_table_lookup_import.GetName() << " is not connected" << EndDebugError;
			enable = false;
			return false;
		}

		if(!blob_import)
		{
			logger << DebugError << blob_import.GetName() << " is not connected" << EndDebugError;
			enable = false;
			return false;
		}

		reg_pc = registers_import->GetRegister(pc_register_name.c_str());

		if(!reg_pc)
		{
			logger << DebugWarning << "Undefined register " << pc_register_name << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		if(reg_pc->GetSize() != 4)
		{
			logger << DebugWarning << "Register " << pc_register_name << " is not a 32-bit register. Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}
		
		const typename unisim::util::blob::Blob<MEMORY_ADDR> *blob = blob_import->GetBlob();
		if(!blob)
		{
			logger << DebugWarning << "Can't get program informations" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}
		
		MEMORY_ADDR entry_point = blob->GetEntryPoint() / 4;
		
		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "Initializing register " << reg_pc->GetName() << " to 0x" << hex << entry_point << dec << EndDebugInfo;
		}
		reg_pc->SetValue(&entry_point);

		reg_sp = registers_import->GetRegister(sp_register_name.c_str());

		if(!reg_sp)
		{
			logger << DebugWarning << "Undefined register " << sp_register_name << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		if(reg_sp->GetSize() != 4)
		{
			logger << DebugWarning << "Register " << sp_register_name << " is not a 32-bit register. Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		MEMORY_ADDR stack_base = blob->GetStackBase() / 4;
		
		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "Initializing register " << reg_sp->GetName() << " to 0x" << hex << stack_base << dec << EndDebugInfo;
		}
		reg_sp->SetValue(&stack_base);

		const unisim::util::debug::Symbol<MEMORY_ADDR> *c_io_buffer_symbol = symbol_table_lookup_import->FindSymbolByName(c_io_buffer_symbol_name.c_str());

		if(!c_io_buffer_symbol)
		{
			logger << DebugWarning << "Undefined symbol " << c_io_buffer_symbol_name << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		const unisim::util::debug::Symbol<MEMORY_ADDR> *c_io_breakpoint_symbol = symbol_table_lookup_import->FindSymbolByName(c_io_breakpoint_symbol_name.c_str());

		if(!c_io_breakpoint_symbol)
		{
			logger << DebugWarning << "Undefined symbol " << c_io_breakpoint_symbol_name << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		const unisim::util::debug::Symbol<MEMORY_ADDR> *c_exit_breakpoint_symbol = symbol_table_lookup_import->FindSymbolByName(c_exit_breakpoint_symbol_name.c_str());

		if(!c_exit_breakpoint_symbol)
		{
			logger << DebugWarning << "Undefined symbol " << c_exit_breakpoint_symbol_name << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		c_io_buffer_addr = c_io_buffer_symbol->GetAddress();

		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "Using " << c_io_buffer_symbol->GetName() << " at 0x" << hex << c_io_buffer_addr << dec << " as I/O buffer" << EndDebugInfo;
		}

		MEMORY_ADDR c_io_breakpoint_addr = c_io_breakpoint_symbol->GetAddress();
		c_exit_breakpoint_addr = c_exit_breakpoint_symbol->GetAddress();

		uint8_t swi[4] = { 0x00, 0x00, 0x00, 0x66 };

		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "Installing emulator breakpoint (SWI) for I/O at 0x" << hex << c_io_breakpoint_addr << dec << " (symbol " << c_io_breakpoint_symbol->GetName() << ")" << EndDebugInfo;
		}

		if(!memory_import->WriteMemory(c_io_breakpoint_addr, swi, sizeof(swi)))
		{
			logger << DebugWarning << "Cannot install breakpoint at 0x" << hex << c_io_breakpoint_addr << dec << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}

		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "Installing emulator breakpoint (SWI) for EXIT at 0x" << hex << c_exit_breakpoint_addr << dec << " (symbol " << c_exit_breakpoint_symbol->GetName() << ")" << EndDebugInfo;
		}

		if(!memory_import->WriteMemory(c_exit_breakpoint_addr, swi, sizeof(swi)))
		{
			logger << DebugWarning << "Cannot install breakpoint at 0x" << hex << c_exit_breakpoint_addr << dec << ". Disabling TI C I/O support" << EndDebugWarning;
			enable = false;
			return !warning_as_error;
		}
	}
	else
	{
		if(unlikely(verbose_setup || verbose_all))
		{
			logger << DebugInfo << "TI C I/O support is disabled" << EndDebugInfo;
		}
	}

	target_to_host_fildes.insert(std::pair<int16_t, int>(0, 0));
	target_to_host_fildes.insert(std::pair<int16_t, int>(1, 1));
	target_to_host_fildes.insert(std::pair<int16_t, int>(2, 2));

	return true;
}

template <class MEMORY_ADDR>
void TI_C_IO<MEMORY_ADDR>::GrowBuffer(uint32_t byte_length)
{
	if(buffer)
	{
		if(byte_length > max_buffer_byte_length)
		{
			buffer = (uint8_t *) realloc(buffer, byte_length);
		}
	}
	else
	{
		buffer = (uint8_t *) calloc(byte_length, 1);
	}

	max_buffer_byte_length = byte_length;
}

template <class MEMORY_ADDR>
const char *TI_C_IO<MEMORY_ADDR>::GetCommandFriendlyName(uint32_t command)
{
	switch(command)
	{
		case C_IO_CMD_OPEN: return "open";
		case C_IO_CMD_CLOSE: return "close";
		case C_IO_CMD_READ: return "read";
		case C_IO_CMD_WRITE: return "write";
		case C_IO_CMD_LSEEK: return "lseek";
		case C_IO_CMD_UNLINK: return "unlink";
		case C_IO_CMD_GETENV: return "getenv";
		case C_IO_CMD_RENAME: return "rename";
		case C_IO_CMD_GETTIME: return "gettime";
		case C_IO_CMD_GETCLOCK: return "getclock";
	}
	return "?";
}

template <class MEMORY_ADDR>
void TI_C_IO<MEMORY_ADDR>::Reset()
{
	LoadMemoryAndRegisters();
}

template <class MEMORY_ADDR>
unisim::service::interfaces::TI_C_IO::Status TI_C_IO<MEMORY_ADDR>::HandleEmulatorInterrupt()
{
	if(!enable) return unisim::service::interfaces::TI_C_IO::ERROR;

	uint32_t i;
	InputMsg input_msg;
	MEMORY_ADDR addr = c_io_buffer_addr;

	uint32_t pc;
	reg_pc->GetValue(&pc);

	if((4 * pc) == c_exit_breakpoint_addr) return unisim::service::interfaces::TI_C_IO::EXIT;

	if(!memory_injection_import->InjectReadMemory(addr, &input_msg, sizeof(input_msg))) return unisim::service::interfaces::TI_C_IO::ERROR;

	// Adapt input message to host endianness
	input_msg.length = unisim::util::endian::LittleEndian2Host(input_msg.length);
	input_msg.command = unisim::util::endian::LittleEndian2Host(input_msg.command);
	for(i = 0; i < NUM_PARMS; i++)
	{
		input_msg.parm[i] = unisim::util::endian::LittleEndian2Host(input_msg.parm[i]);
	}

	GrowBuffer(input_msg.length);

	if(input_msg.length)
	{
		addr += sizeof(input_msg);

		if(!memory_injection_import->InjectReadMemory(addr, buffer, input_msg.length)) return unisim::service::interfaces::TI_C_IO::ERROR;
	}

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "input msg:" << endl << "  - length=" << input_msg.length << endl << "  - command=0x" << hex << input_msg.command << dec << " (" << GetCommandFriendlyName(input_msg.command) << ")" << endl;

		for(i = 0; i < NUM_PARMS; i++)
		{
			logger << "  - parm[" << i << "]=0x" << hex << input_msg.parm[i] << dec << endl;
		}
		
		logger << "  - buffer=";

		if(input_msg.length)
		{
			logger << hex;
			for(i = 0; i < input_msg.length; i++)
			{
				logger << "0x" << (unsigned int) buffer[i];
				if(i != input_msg.length - 1) logger << " ";
			}
			logger << dec;
		}
		else
		{
			logger << "<empty>";
		}
		logger << endl << EndDebugInfo;
	}

	OutputMsg output_msg;
	memset(&output_msg, 0, sizeof(output_msg));

	switch(input_msg.command)
	{
		case C_IO_CMD_OPEN:
			output_msg.parm[0] = c_io_open((const char *) buffer, input_msg.parm[1], input_msg.parm[0]);
			break;
		case C_IO_CMD_CLOSE:
			output_msg.parm[0] = c_io_close(input_msg.parm[0]);
			break;
		case C_IO_CMD_READ:
			GrowBuffer(input_msg.parm[1]);
			output_msg.parm[0] = c_io_read(input_msg.parm[0], (char *) buffer, input_msg.parm[1]);
			output_msg.length = (int16_t) output_msg.parm[0] < 0 ? 0 : output_msg.parm[0];
			break;
		case C_IO_CMD_WRITE:
			output_msg.parm[0] = c_io_write(input_msg.parm[0], (const char *) buffer, input_msg.parm[1]);
			break;
		case C_IO_CMD_LSEEK:
			{
				uint32_t fpos = c_io_lseek(input_msg.parm[0], (int32_t) input_msg.parm[1] | ((int32_t) input_msg.parm[2] << 16), input_msg.parm[3]);
				output_msg.parm[0] = fpos & 0xffff;
				if(!enable_lseek_bug)
				{
					output_msg.parm[1] = (fpos >> 16) & 0xffff; // Code Composer 4.10.37 for Windows has a bug, it leaves this value unchanged.
				}
			}
			break;
		case C_IO_CMD_UNLINK:
			output_msg.parm[0] = c_io_unlink((const char *) buffer);
			break;
		case C_IO_CMD_GETENV:
			{
				const char *value = c_io_getenv((const char *) buffer);
				if(value)
				{
					size_t len = strlen(value);
					output_msg.parm[0] = 1;
					GrowBuffer(len + 1);
					strcpy((char *) buffer, value);
					output_msg.length = len + 1;
				}
				else
				{
					output_msg.parm[0] = 0;
				}
			}
			break;
		case C_IO_CMD_RENAME:
			{
				const char *oldpath = (const char *) buffer;
				const char *newpath = (const char *) buffer + strlen((const char *) buffer) + 1;
				output_msg.parm[0] = c_io_rename(oldpath, newpath);
			}
			break;
		case C_IO_CMD_GETTIME:
			{
				uint32_t t = c_io_gettime();
				output_msg.parm[0] = t;
				output_msg.parm[1] = t >> 16;
			}
			break;
		case C_IO_CMD_GETCLOCK:
			{
				uint32_t clk = c_io_getclock();
				output_msg.parm[0] = clk;
				output_msg.parm[1] = clk >> 16;
			}
			break;
	}

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "output msg:" << endl << "  - length=" << output_msg.length << endl;
		for(i = 0; i < NUM_PARMS; i++)
		{
			logger << "  - parm[" << i << "]=0x" << hex << output_msg.parm[i] << dec << endl;
		}
		logger << "  - buffer=";

		if(output_msg.length)
		{
			logger << hex;
			for(i = 0; i < output_msg.length; i++)
			{
				logger << "0x" << (unsigned int) buffer[i];
				if(i != output_msg.length - 1) logger << " ";
			}
			logger << dec;
		}
		else
		{
			logger << "<empty>";
		}
		logger << endl << EndDebugInfo;
	}

	// Adapt output message to target endianness
	uint32_t output_msg_length = output_msg.length; // Keep the length in the host endian format as we need it for writing the message in the target memory
	output_msg.length = unisim::util::endian::Host2LittleEndian(output_msg.length);
	for(i = 0; i < NUM_PARMS; i++)
	{
		output_msg.parm[i] = unisim::util::endian::Host2LittleEndian(output_msg.parm[i]);
	}

	addr = c_io_buffer_addr;

	if(!memory_injection_import->InjectWriteMemory(addr, &output_msg, sizeof(output_msg))) return unisim::service::interfaces::TI_C_IO::ERROR;

	if(output_msg_length)
	{
		addr += sizeof(output_msg);

		if(!memory_injection_import->InjectWriteMemory(addr, buffer, output_msg_length)) return unisim::service::interfaces::TI_C_IO::ERROR;
	}

	return unisim::service::interfaces::TI_C_IO::OK;
}

template <class MEMORY_ADDR>
int TI_C_IO<MEMORY_ADDR>::TranslateFileDescriptor(int16_t fno)
{
	// Return an error if file descriptor does not exist
	map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
	if(iter == target_to_host_fildes.end()) return -1;

	// Translate the C I/O file descriptor to an host file descriptor
	return (*iter).second;
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_open(const char *path, uint16_t c_io_flags, int16_t fno)
{
	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "open:" << endl << "  - path=\"" << path << "\"" << endl << "  - flags=0x" << hex
		       << (c_io_flags & ~(C_IO_O_RDONLY | C_IO_O_WRONLY | C_IO_O_RDWR | C_IO_O_APPEND | C_IO_O_CREAT | C_IO_O_TRUNC | C_IO_O_BINARY)) << dec;
		if(c_io_flags & C_IO_O_RDONLY) logger << " | O_RDONLY";
		if(c_io_flags & C_IO_O_WRONLY) logger << " | O_WRONLY";
		if(c_io_flags & C_IO_O_RDWR) logger << " | O_RDWR";
		if(c_io_flags & C_IO_O_APPEND) logger << " | O_APPEND";
		if(c_io_flags & C_IO_O_CREAT) logger << " | O_CREAT";
		if(c_io_flags & C_IO_O_TRUNC) logger << " | O_TRUNC";
		if(c_io_flags & C_IO_O_BINARY) logger << " | O_BINARY";

		logger << endl << "  - fno=" << fno << EndDebugInfo;
	}

	// Return an error if file description is already opened
	map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
	if(iter != target_to_host_fildes.end())
	{
		if(unlikely(verbose_io || verbose_all))
		{
			logger << DebugInfo << "File descriptor " << fno << " is already opened" << EndDebugInfo;
		}
		return -1;
	}

	int fd;      // the host file descriptor
	int flags = 0;   // the host flags

	// Translate C I/O flags to host flags
	if(c_io_flags & C_IO_O_RDONLY) flags |= O_RDONLY;
	if(c_io_flags & C_IO_O_WRONLY) flags |= O_WRONLY;
	if(c_io_flags & C_IO_O_RDWR) flags |= O_RDWR;
	if(c_io_flags & C_IO_O_APPEND) flags |= O_APPEND;
	if(c_io_flags & C_IO_O_CREAT) flags |= O_CREAT;
	if(c_io_flags & C_IO_O_TRUNC) flags |= O_TRUNC;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(c_io_flags & C_IO_O_BINARY) flags |= O_BINARY;
#endif

	// Open the file on the host machine
	if(flags & O_CREAT)
	{
		mode_t mode = S_IRWXU;
		fd = open(path, flags, mode);
	}
	else
	{
		fd = open(path, flags);
	}

	// if an error occurs, report an error
	if(fd == -1)
	{
		if(unlikely(verbose_io || verbose_all))
		{
			logger << DebugInfo << "'open' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
		}

		return -1;
	}

	// else keep relation between the C I/O file descriptor or the host file descriptor
	target_to_host_fildes.insert(std::pair<int16_t, int>(fno, fd));

	// return the C I/O file descriptor
	return fno;
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_close(int16_t fno)
{
	int fd;      // the host file descriptor

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "close:" << endl << "  - fno=" << fno << EndDebugInfo;
	}

	// Check whether target file descriptor is stdin, stdout or stderr
	if(fno == 0 || fno == 1 || fno == 2)
	{
		return 0;
	}
	else
	{
		// Return an error if file descriptor does not exist
		map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
		if(iter == target_to_host_fildes.end()) return -1;

		// Translate the C I/O file descriptor to an host file descriptor
		fd = (*iter).second;

		target_to_host_fildes.erase(iter);
	}

	// Do not close host stdin, stdout or stderr
	if(fd == 0 || fd == 1 || fd == 2) return 0;

	int ret = close(fd);

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'close' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_read(int16_t fno, char *buf, uint16_t count)
{
	int fd;      // the host file descriptor

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "read:" << endl << "  - fno=" << fno << endl << "  - count=" << count << EndDebugInfo;
	}

	// Translate TI C I/O file descriptor to an host file descriptor
	fd = TranslateFileDescriptor(fno);

	// Return an error if file descriptor does not exist
	if(fd == -1) return -1;

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "file descriptor translation: fno=" << fno << " => fd=" << fd << EndDebugInfo;
	}

	ssize_t ret = read(fd, buf, count);

	if(unlikely((verbose_io || verbose_all) && ret == 0))
	{
		off_t ofs = lseek(fd, 0, SEEK_CUR);
		logger << DebugInfo << "End-of-File (EOF) at file offset " << ofs << EndDebugInfo;
	}

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'read' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_write(int16_t fno, const char *buf, uint16_t count)
{
	int fd;      // the host file descriptor

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "write:" << endl << "  - fno=" << fno << endl << "  - count=" << count << EndDebugInfo;
	}

	// Translate TI C I/O file descriptor to an host file descriptor
	fd = TranslateFileDescriptor(fno);

	// Return an error if file descriptor does not exist
	if(fd == -1) return -1;

	ssize_t ret = write(fd, buf, count);

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'write' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
int32_t TI_C_IO<MEMORY_ADDR>::c_io_lseek(int16_t fno, int32_t offset, int16_t origin)
{
	int fd;      // the host file descriptor

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "lseek:" << endl << "  - fno=" << fno << endl << "  - offset=" << offset << endl << "  - origin=" << origin << EndDebugInfo;
	}

	// Translate TI C I/O file descriptor to an host file descriptor
	fd = TranslateFileDescriptor(fno);

	// Return an error if file descriptor does not exist
	if(fd == -1) return -1;

	int whence;

	switch(origin)
	{
		case C_IO_SEEK_SET:
			whence = SEEK_SET;
			break;
		case C_IO_SEEK_CUR:
			whence = SEEK_CUR;
			break;
		case C_IO_SEEK_END:
			whence = SEEK_END;
			break;
		default:
			if(unlikely(verbose_io || verbose_all))
			{
				logger << DebugInfo << "bad origin (" << origin << ")" << EndDebugInfo;
			}
			return -1;
	}

	off_t ret = lseek(fd, offset, whence);

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'lseek' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "'lseek' returns " << ret << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_unlink(const char *path)
{
	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "unlink:" << endl << "  - path=\"" << path << "\"" << EndDebugInfo;
	}

	int ret = unlink(path);

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'unlink' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
const char *TI_C_IO<MEMORY_ADDR>::c_io_getenv(const char *name)
{
	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "getenv:" << endl << "  - name=\"" << name << "\"" << EndDebugInfo;
	}

	const char *ret = getenv(name);

	if(unlikely((verbose_io || verbose_all) && !ret))
	{
		logger << DebugInfo << "'gentenv' reports an error (no match)" << EndDebugInfo;
	}

	return ret;

}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_rename(const char *oldpath, const char *newpath)
{
	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "rename:" << endl << "  - oldpath=\"" << oldpath << "\"" << endl << "  - newpath=\"" << newpath << "\"" << EndDebugInfo;
	}

	int ret = rename(oldpath, newpath);

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'rename' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
uint32_t TI_C_IO<MEMORY_ADDR>::c_io_gettime()
{
	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "getime" << EndDebugInfo;
	}

	time_t ret = time(0);

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'time' reports an error (" << strerror(errno) << ")" << EndDebugInfo;
	}

	return ret;
}

template <class MEMORY_ADDR>
uint32_t TI_C_IO<MEMORY_ADDR>::c_io_getclock()
{
	if(unlikely(verbose_io || verbose_all))
	{
		logger << DebugInfo << "getclock" << EndDebugInfo;
	}

	clock_t ret = clock();

	if(unlikely((verbose_io || verbose_all) && ret == -1))
	{
		logger << DebugInfo << "'clock' reports an error (processor time used is not available or its value cannot be represented)" << EndDebugInfo;
	}

	return ret;
}

} // end of namespace ti_c_io
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_HH__
