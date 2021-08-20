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
 
#ifndef __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_HH__
#define __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_HH__

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>

#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/ti_c_io.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/blob.hh"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif

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
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::logger::Logger;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Blob;

template <class MEMORY_ADDR>
class TI_C_IO :
	public Service<unisim::service::interfaces::TI_C_IO>,
	public Client<Memory<MEMORY_ADDR> >,
	public Client<MemoryInjection<MEMORY_ADDR> >,
	public Client<SymbolTableLookup<MEMORY_ADDR> >,
	public Client<Registers>,
	public Client<Blob<MEMORY_ADDR> >
{
public:
	ServiceExport<unisim::service::interfaces::TI_C_IO> ti_c_io_export;

	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceImport<MemoryInjection<MEMORY_ADDR> > memory_injection_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<SymbolTableLookup<MEMORY_ADDR> > symbol_table_lookup_import; 
	ServiceImport<Blob<MEMORY_ADDR> > blob_import;

    TI_C_IO(const char *name, Object *parent = 0);
    virtual ~TI_C_IO();

    virtual void OnDisconnect();
	virtual bool EndSetup();

	virtual void Reset();
    virtual unisim::service::interfaces::TI_C_IO::Status HandleEmulatorInterrupt();
private:
	bool LoadMemoryAndRegisters();

	// TI's C I/O commands
	static const uint8_t C_IO_CMD_OPEN = 0xf0;
	static const uint8_t C_IO_CMD_CLOSE = 0xf1;
	static const uint8_t C_IO_CMD_READ = 0xf2;
	static const uint8_t C_IO_CMD_WRITE = 0xf3;
	static const uint8_t C_IO_CMD_LSEEK = 0xf4;
	static const uint8_t C_IO_CMD_UNLINK = 0xf5;
	static const uint8_t C_IO_CMD_GETENV = 0xf6;
	static const uint8_t C_IO_CMD_RENAME = 0xf7;
	static const uint8_t C_IO_CMD_GETTIME = 0xf8;
	static const uint8_t C_IO_CMD_GETCLOCK = 0xf9;

	static const uint16_t C_IO_O_RDONLY = 0x0000;
	static const uint16_t C_IO_O_WRONLY = 0x0001;
	static const uint16_t C_IO_O_RDWR = 0x0002;
	static const uint16_t C_IO_O_APPEND = 0x0008;
	static const uint16_t C_IO_O_CREAT = 0x0200;
	static const uint16_t C_IO_O_TRUNC = 0x0400;
	static const uint16_t C_IO_O_BINARY = 0x8000;

	static const int16_t C_IO_SEEK_SET = 0x0000;
	static const int16_t C_IO_SEEK_CUR = 0x0001;
	static const int16_t C_IO_SEEK_END = 0x0002;

	static const unsigned int NUM_PARMS = 4;

	map<int16_t, int> target_to_host_fildes;

	typedef struct PACKED
	{
		uint32_t length;   // length of data in bytes after the parameters
		uint32_t command;
		uint16_t parm[NUM_PARMS];
	} InputMsg;

	typedef struct PACKED
	{
		uint32_t length;   // length of data in bytes after the parameters
		uint16_t parm[4];
	} OutputMsg;

	// the kernel logger
	unisim::kernel::logger::Logger logger;

	MEMORY_ADDR c_io_buffer_addr;   // _CIOBUF_ I/O buffer address
	MEMORY_ADDR c_exit_breakpoint_addr;   // C$$EXIT buffer address
	uint32_t max_buffer_byte_length;
	uint8_t *buffer;

	bool enable;
	Parameter<bool> param_enable; 

	unisim::service::interfaces::Register *reg_pc;
	unisim::service::interfaces::Register *reg_sp;

	bool warning_as_error;
	string pc_register_name;
	string sp_register_name;
	string c_io_buffer_symbol_name;
	string c_io_breakpoint_symbol_name;
	string c_exit_breakpoint_symbol_name;

	Parameter<bool> param_warning_as_error;
	Parameter<string> param_pc_register_name;
	Parameter<string> param_c_io_buffer_symbol_name;
	Parameter<string> param_c_io_breakpoint_symbol_name;
	Parameter<string> param_c_exit_breakpoint_symbol_name;

	bool verbose_all;
	bool verbose_io;
	bool verbose_setup;
	bool enable_lseek_bug;
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_io;
	Parameter<bool> param_verbose_setup;
	Parameter<bool> param_enable_lseek_bug;

	void GrowBuffer(uint32_t byte_length);
	const char *GetCommandFriendlyName(uint32_t command);
	int TranslateFileDescriptor(int16_t fno);

	int16_t c_io_open(const char *path, uint16_t c_io_flags, int16_t fno);
	int16_t c_io_close(int16_t fno);
	int16_t c_io_read(int16_t fno, char *buf, uint16_t count);
	int16_t c_io_write(int16_t fno, const char *buf, uint16_t count);
	int32_t c_io_lseek(int16_t fno, int32_t offset, int16_t origin);
	int16_t c_io_unlink(const char *path);
	const char *c_io_getenv(const char *name);
	int16_t c_io_rename(const char *oldpath, const char *newpath);
	uint32_t c_io_gettime();
	uint32_t c_io_getclock();
};

} // end of namespace ti_c_io
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_HH__
