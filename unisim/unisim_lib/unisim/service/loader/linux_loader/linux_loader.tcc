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
 
#ifndef __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__

#include <stdlib.h>
#include <unistd.h>
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

#include <sstream>
#include <iostream>
#include <vector>

namespace unisim {
namespace service {
namespace loader {
namespace linux_loader {

using namespace std;
using namespace unisim::kernel::logger;

template<class T>
LinuxLoader<T>::LinuxLoader(const char *name, Object *parent)
  : Object(name, parent)
  , Client<Loader>(name, parent)
  , Client<Blob<T> >(name, parent)
  , Client<Memory<T> >(name, parent)
  , Service<Loader>(name, parent)
  , Service<Blob<T> >(name, parent)
  , loader_import("loader_import", this)
  , blob_import("blob_import", this)
  , memory_import("memory_import", this)
  , loader_export("loader_export", this)
  , blob_export("blob_export", this)
  , endianness(E_LITTLE_ENDIAN)
  , stack_base(0x4000000UL)
  , stack_size(0x800000UL)
  , max_environ(0)
  , memory_page_size(4096)
  , argc(0)
  , argv()
  , apply_host_environ(true)
  , envc(0)
  , envp()
  , target_envp()
  , blob(0)
  , stack_blob(0)
  , verbose(false)
  , endianness_string("little-endian")
  , param_endian("endianness", this, endianness_string
  , 		"The endianness of the binary loaded. Available values are:"
		" little-endian and big-endian.")
  , param_stack_base("stack-base", this, stack_base
  , 		"The stack base address used for the load and execution of the "
		"linux application")
  , param_stack_size("stack-size", this, stack_size
  , 		"The stack size used for the load and execution of the linux "
		"application. The top of the stack will be stack-base + stack-size.")
  , param_max_environ("max-environ", this, max_environ
  , 		"The maximum size of the program environment during its execution.")
  , param_argc("argc", this, argc
  , 		"Number of commands in the program execution line (usually at least one"
		" which is the name of the program executed). The different tokens"
		" can be set up with the parameters argv[<n>] where <n> can go up to"
		" argc - 1.")
  , param_argv()
  , param_apply_host_environ("apply-host-environ", this, apply_host_environ
  , 		"Wether to apply the host environment on the target simulator or use"
		" the provided envc and envp.")
  , param_envc("envc", this, envc
  , 		"Number of environment variables defined for the program execution."
		" The different variables can be set up with the parameters envp[<n>]"
		" where <n> can go up to envc - 1.")
  , param_envp()
  , param_memory_page_size("memory-page-size", this, memory_page_size
  , 		"The memory page size to use.")
  , param_verbose("verbose", this, verbose, "Display verbose information")
  , logger(*this)
{
	param_max_environ.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_argc.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	if ( argc )
	{
		for ( unsigned int i = 0; i < argc; i++ )
		{
			std::stringstream argv_name, argv_desc, argv_val;
			argv_name << "argv[" << i << "]";
			argv_desc << "The '" << i << "' token in the command line.";
			argv_val << "Undefined argv[" << i << "] value";
			argv.push_back(new string(argv_val.str().c_str()));
			param_argv.push_back(
					new Parameter<string>(argv_name.str().c_str(), this, 
						*(argv[i]), argv_desc.str().c_str()));
		}
	}
	SetupArgsAndEnvs();
	
	param_envc.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	if ( envc )
	{
		for ( unsigned int i = 0; i < envc; i++ )
		{
			std::stringstream envp_name, envp_desc, envp_val;
			envp_name << "envp[" << i << "]";
			envp_desc << "The '" << i << "' token in the environment.";
			envp_val << "Undefined envp[" << i << "] value";
			envp.push_back(new string(envp_val.str().c_str()));
			param_envp.push_back(
					new Parameter<string>(envp_name.str().c_str(), this,
						*(envp[i]), envp_desc.str().c_str()));
		}
	}
	SetupArgsAndEnvs();

	loader_export.SetupDependsOn(memory_import);
	loader_export.SetupDependsOn(loader_import);
	loader_export.SetupDependsOn(blob_import);
}

template<class T>
LinuxLoader<T>::~LinuxLoader() {
	for ( ;
			!argv.empty();
			argv.erase(argv.begin()) )
	{
		delete *argv.begin();
	}
	for ( ;
			!param_argv.empty();
			param_argv.erase(param_argv.begin()) )
	{
		delete *param_argv.begin();
	}

	for ( ;
			!envp.empty();
			envp.erase(envp.begin()) )
	{
		delete *envp.begin();
	}
	for ( ;
			!param_envp.empty();
			param_envp.erase(param_envp.begin()) )
	{
		delete *param_envp.begin();
	}
	
	if(blob)
	{
		blob->Release();
	}
	if(stack_blob)
	{
		stack_blob->Release();
	}
}

template<class T>
void
LinuxLoader<T>::OnDisconnect() {
}

template<class T>
bool
LinuxLoader<T>::BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}

	if(stack_blob)
	{
		stack_blob->Release();
		stack_blob = 0;
	}

	/* set the endianness depending on the endian parameter */
	if ( (endianness_string.compare("little-endian") != 0) &&
			(endianness_string.compare("big-endian") != 0) )
	{
		logger << DebugError
				<< "Error while setting the endianness."
				<< " '" << endianness_string << "' is not a correct"
				<< " value."
				<< " Available values are: little-endian and big-endian."
				<< EndDebugError;
		return false;
	}
	else
	{
		if ( endianness_string.compare("little-endian") == 0 )
			endianness = E_LITTLE_ENDIAN;
		else
			endianness = E_BIG_ENDIAN;
	}

	// Fetch the environment to apply it to the target environmment
	if ( apply_host_environ )
	{
		for ( char **env = environ; *env != NULL; env++ )
		{
			target_envp.push_back(new std::string(*env));
		}
	}

	return true;
}

template<class T>
bool
LinuxLoader<T>::SetupLoad()
{
	/* check that mem and elf_loader are available */
	if(!memory_import) {
		logger << DebugError << "ERROR(" << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Trying to execute LinuxLoader without a memory service connected."
				<< EndDebugError;
		return false;
	}
	if(!loader_import) {
		logger << DebugError << "ERROR(" << __FUNCTION__ << ":"
				<< __FILE__ << ":"
				<< __LINE__ << "): "
				<< "Trying to execute LinuxLoader without an elf_loader service connected."
				<< EndDebugError;
		return false;
	}

	return true;
}

template <class T>
void
LinuxLoader<T> ::
DumpSegment(unisim::util::blob::Segment<T> const &s, int indent)
{
	std::string s_indent_pre(indent + 1, '*');
	std::stringstream ss_indent;
	ss_indent << s_indent_pre << " ";
	std::string s_indent(ss_indent.str());

	logger << s_indent << "S ";
	logger << "Type: ";
	switch ( s.GetType() )
	{
		case unisim::util::blob::Segment<T>::TY_LOADABLE:
			logger << "Loadable ";
			break;
		case unisim::util::blob::Segment<T>::TY_UNKNOWN:
		default:
			logger << "Unknown  ";
			break;
	}
	logger << " Attribute: ";
	switch ( s.GetAttr() )
	{
		case unisim::util::blob::Segment<T>::SA_NULL:
			logger << "NULL ";
			break;
		case unisim::util::blob::Segment<T>::SA_R:
			logger << "R    ";
			break;
		case unisim::util::blob::Segment<T>::SA_W:
			logger << "W    ";
			break;
		case unisim::util::blob::Segment<T>::SA_X:
			logger << "X    ";
			break;
		case unisim::util::blob::Segment<T>::SA_RW:
			logger << "RW   ";
			break;
		case unisim::util::blob::Segment<T>::SA_RX:
			logger << "RX   ";
			break;
		case unisim::util::blob::Segment<T>::SA_WX:
			logger << "WX   ";
			break;
		case unisim::util::blob::Segment<T>::SA_RWX:
			logger << "RWX  ";
			break;
		default:
			logger << "?    ";
			break;
	}
	logger << " Alignment: " << s.GetAlignment();
	logger << "  Address: 0x" << std::hex << s.GetAddr() << std::dec;
	logger << "  Size: " << s.GetSize();
	logger << std::endl;
}

template <class T>
void
LinuxLoader<T> ::
DumpSection(unisim::util::blob::Section<T> const &s, int indent)
{
	std::string s_indent_pre(indent + 1, '-');
	std::stringstream ss_indent;
	ss_indent << s_indent_pre << " ";
	std::string s_indent(ss_indent.str());

	logger << s_indent << "s ";
	logger << "Name: " << s.GetName() << std::endl;
	logger << s_indent << "  Type: ";
	switch ( s.GetType() )
	{
		case unisim::util::blob::Section<T>::TY_UNKNOWN:
			logger << "Unknown ";
			break;
		case unisim::util::blob::Section<T>::TY_NULL:
			logger << "Null    ";
			break;
		case unisim::util::blob::Section<T>::TY_PROGBITS:
			logger << "Progbits";
			break;
		case unisim::util::blob::Section<T>::TY_NOBITS:
			logger << "Nobits  ";
			break;
		case unisim::util::blob::Section<T>::TY_STAB:
			logger << "Stab    ";
			break;
		case unisim::util::blob::Section<T>::TY_STRTAB:
			logger << "Strtab  ";
			break;
	}
	logger << "  Attribute: ";
	switch ( s.GetAttr() )
	{
		case unisim::util::blob::Section<T>::SA_NULL:
			logger << "(None)";
			break;
		case unisim::util::blob::Section<T>::SA_A:
			logger << "A     ";
			break;
		case unisim::util::blob::Section<T>::SA_W:
			logger << "W     ";
			break;
		case unisim::util::blob::Section<T>::SA_AW:
			logger << "AW    ";
			break;
		case unisim::util::blob::Section<T>::SA_X:
			logger << "X     ";
			break;
		case unisim::util::blob::Section<T>::SA_AX:
			logger << "AX    ";
			break;
		case unisim::util::blob::Section<T>::SA_WX:
			logger << "WX    ";
			break;
		case unisim::util::blob::Section<T>::SA_AWX:
			logger << "AWX   ";
			break;
	}
	logger << "  Link: " << s.GetLink();
	logger << "  Alignment: " << s.GetAlignment();
	logger << "  Address: 0x" << std::hex << s.GetAddr() << std::dec;
	logger << "  Size: " << s.GetSize();
	T min_range, max_range;
	s.GetAddrRange(min_range, max_range);
	logger << "  Range: 0x" << std::hex << min_range
		<< "-0x" << max_range << std::dec << std::endl;
}

template <class T>
void
LinuxLoader<T> ::
DumpBlob(unisim::util::blob::Blob<T> const &b, int indent)
{
	std::string s_indent_pre(indent + 1, '+');
	std::stringstream ss_indent;
	ss_indent << s_indent_pre << " ";
	std::string s_indent(ss_indent.str().c_str());

	typename unisim::util::blob::Blob<T>::Capability cap =
		b.GetCapability();
	const char *filename = b.GetFilename();
	T entry = b.GetEntryPoint();
	T stack = b.GetStackBase();
	unisim::util::endian::endian_type endian =
		b.GetEndian();

	if ( indent == 0 )
		logger << DebugInfo
			<< "Blob structure" << std::endl;
	logger << s_indent << "+" << std::endl;
	logger << s_indent << "Capabilities: ";
	std::stringstream ss_cap;
	bool first = true;
	if ( cap & unisim::util::blob::Blob<T>::CAP_FILENAME ) { first = false; ss_cap << "Filename";}
	if ( cap & unisim::util::blob::Blob<T>::CAP_ENTRY_POINT ) 
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "EntryPoint";
	}
	if ( cap & unisim::util::blob::Blob<T>::CAP_ARCHITECTURE )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "Architecture";
	}
	if ( cap & unisim::util::blob::Blob<T>::CAP_STACK_BASE )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "StackBase";
	}
	if ( cap & unisim::util::blob::Blob<T>::CAP_ENDIAN )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "Endian";
	}
	if ( cap & unisim::util::blob::Blob<T>::CAP_ADDRESS_SIZE )
	{
		if ( !first ) ss_cap << "|";
		first = false; ss_cap << "AddressSize";
	}
	if ( first )
		ss_cap << "(Default)";
	logger << ss_cap.str() << std::endl;
	
	if ( cap & unisim::util::blob::Blob<T>::CAP_FILENAME )
		logger << s_indent << "Filename: "
			<< b.GetFilename() << std::endl;
	if ( cap & unisim::util::blob::Blob<T>::CAP_ENTRY_POINT ) 
		logger << s_indent << "Entry point: 0x"
			<< std::hex << b.GetEntryPoint() << std::dec << std::endl;
	if ( cap & unisim::util::blob::Blob<T>::CAP_ARCHITECTURE )
		logger << s_indent << "Architecture: "
			<< b.GetArchitecture() << std::endl;
	if ( cap & unisim::util::blob::Blob<T>::CAP_STACK_BASE )
		logger << s_indent << "Stack base: "
			<< b.GetStackBase() << std::endl;
	if ( cap & unisim::util::blob::Blob<T>::CAP_ENDIAN )
		logger << s_indent << "Endian: "
			<< (b.GetEndian() == unisim::util::endian::E_LITTLE_ENDIAN ?
					"Little endian" : "Big endian") << std::endl;

	const std::vector<const unisim::util::blob::Segment<T> *> &segments =
		b.GetSegments();
	for ( typename std::vector<const unisim::util::blob::Segment<T> *>::const_iterator it = segments.begin();
			it != segments.end();
			++it )
		DumpSegment(*(*it), indent);

	const std::vector<const unisim::util::blob::Section<T> *> &sections =
		b.GetSections();
	for ( typename std::vector<const unisim::util::blob::Section<T> *>::const_iterator it = sections.begin();
			it != sections.end();
			it++ )
		DumpSection(*(*it), indent);

	const std::vector<const unisim::util::blob::Blob<T> *> &blobs =
		b.GetBlobs();
	for ( typename std::vector<const unisim::util::blob::Blob<T> *>::const_iterator it = blobs.begin();
			it != blobs.end();
			it++ )
		DumpBlob(*(*it), indent + 1);

	logger << s_indent << "+" << std::endl;
	
	if ( indent == 0 )
		logger << EndDebugInfo;
}
template<class T>
bool
LinuxLoader<T>::SetupBlob()
{
	if(blob) return true;
	if(!loader_import) return false;
	const unisim::util::blob::Blob<T> *loader_blob = blob_import->GetBlob();
	if(!loader_blob) return false;

	DumpBlob(*loader_blob, 0);
	T entry_point = loader_blob->GetEntryPoint();
	bool load_addr_set = false;
	T load_addr = ~((T)0);
	T start_code = ~((T)0);
	T end_code = 0;
	T start_data = 0;
	T end_data = 0;
	T elf_stack = ~((T)0);
	T elf_brk = 0;
	T num_segments = 0;

	const std::vector<const unisim::util::blob::Segment<T> *> &segments =
		loader_blob->GetSegments();
	num_segments = segments.size();
	for ( typename std::vector<const unisim::util::blob::Segment<T> *>::const_iterator it = segments.begin();
			it != segments.end();
			it++ )
	{
		typename unisim::util::blob::Segment<T>::Type type = (*it)->GetType();
		if ( type != unisim::util::blob::Segment<T>::TY_LOADABLE )
			continue;

		T segment_addr = (*it)->GetAddr();
		T segment_end_addr = segment_addr + (*it)->GetSize();
		if ( !load_addr_set )
		{
			load_addr_set = true;
			/* TODO
			 * WARNING
			 * We are not considering the offset in the elf file. It would be better to set
			 *   load_addr = segment_addr - segment_offset
			 */
			load_addr = segment_addr;
		}
		if ( segment_addr < start_code )
			start_code = segment_addr;
		if ( start_data < segment_addr )
			start_data = segment_addr;

		if ( (*it)->GetAttr() & unisim::util::blob::Segment<T>::SA_X )
		{
			if ( end_code < segment_end_addr )
				end_code = segment_end_addr;
		}
		if ( end_data < segment_end_addr )
			end_data = segment_end_addr;

		if ( segment_end_addr > elf_brk )
			elf_brk = segment_end_addr;

	}
	logger << DebugInfo
		<< "Loader locations:" << std::endl
		<< " start_code = 0x" << std::hex << start_code << std::endl
		<< " end_code   = 0x" << end_code << std::endl
		<< " start_data = 0x" << start_data << std::endl
		<< " end_data   = 0x" << end_data << std::endl
		<< " elf_stack  = 0x" << elf_stack << std::endl
		<< " elf_brk    = 0x" << elf_brk << std::dec
		<< EndDebugInfo;

	// Create the stack
	uint8_t *stack_data = (uint8_t *)calloc(stack_size, 1);
	// Fill the stack
	T sp = stack_size - sizeof(T);
	// - copy the filename
	T cur_length = argv[0]->length() + 1;
	sp = sp - cur_length;
	T sp_argc = sp;
	memcpy(stack_data + sp, argv[0]->c_str(), cur_length);
	// - copy envp
	std::vector<T> sp_envp;
	if ( apply_host_environ )
	{
		for ( unsigned int i = 0; i < target_envp.size(); i++ )
		{
			cur_length = target_envp[i]->length() + 1;
			sp = sp - cur_length;
			sp_envp.push_back(sp);
			memcpy(stack_data + sp, target_envp[i]->c_str(), cur_length);
		}
	}
	else
	{
		for ( unsigned int i = 0; i < envc; i++ )
		{
			cur_length = envp[i]->length() + 1;
			sp = sp - cur_length;
			sp_envp.push_back(sp);
			memcpy(stack_data + sp, envp[i]->c_str(), cur_length);
		}
	}
	// - copy argv
	std::vector<T> sp_argv;
	for ( unsigned int i = argc; i > 0; i-- )
	{
		cur_length = argv[i - 1]->length() + 1;
		sp = sp - cur_length;
		sp_argv.push_back(sp);
		memcpy(stack_data + sp, argv[i - 1]->c_str(), cur_length);
	}

	// force 16 byte alignment
	sp = sp & ~(T)0x0f;
	std::cerr << "+++ argc = " << argc << std::endl;
	std::cerr << "+++ envc = " << envc << std::endl;
	T sp_content_size = 0;
	sp_content_size += (13 * 2) * sizeof(T); // number of aux table entries and their values
	if ( apply_host_environ )
	{
		sp_content_size += (target_envp.size() + 1) * sizeof(T);
	}
	else
	{
		sp_content_size += (envc + 1) * sizeof(T); // pointers to the env and null pointer
	}
	sp_content_size += (argc + 1) * sizeof(T); // pointers to the arg and null pointer
	sp_content_size += 1 * sizeof(T); // argc itself
	if ( sp_content_size & 0x0f ) // force aligned start
		sp = sp - (16 - (sp_content_size & 0x0f));

	T aux_table_symbol;
	T aux_table_value;

	aux_table_symbol = AT_NULL;
	aux_table_value = 0;
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_PHDR;
	/* TODO
	 * WARNING
	 * load_addr does not consider the segments offsets in the elf file (see previous warning).
	 * The elf library should provide information on the size of the elf header.
	 */
	aux_table_value = load_addr + 52; // 52 = size of the elf32 header
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_PHENT;
	/* TODO
	 * WARNING
	 * The elf library should provide information on the size of the program header.
	 */
	aux_table_value = 32; // 32 = size of the program header
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_PHNUM;
	aux_table_value = num_segments;
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_PAGESZ;
	aux_table_value = memory_page_size;
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	/* TODO
	 * Add support for interpreter.
	 */
	aux_table_symbol = AT_BASE;
	aux_table_value = 0; // 0 to signal no interpreter
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_FLAGS;
	aux_table_value = 0;
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_ENTRY;
	aux_table_value = entry_point;
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_UID;
	aux_table_value = (T)getuid();
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_EUID;
	aux_table_value = (T)geteuid();
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
	
	aux_table_symbol = AT_GID;
	aux_table_value = (T)getgid();
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_EGID;
	aux_table_value = (T)getegid();
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	/* TODO
	 * Adapt for other architectures as PowerPC,...
	 */
	aux_table_symbol = AT_HWCAP;
	aux_table_value = ARM_ELF_HWCAP;
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	aux_table_symbol = AT_CLKTCK;
	aux_table_value = (T)sysconf(_SC_CLK_TCK);
	sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

	/* TODO
	 * Enforce required alignment necessary in some architectures (i.e., PowerPC).
	 */

	T saved_aux = sp;

	// Now we have to put the pointers to the different argv and envp
	//   remark that we are decrementing the stack pointer so we start by the end,
	//   that is envp
	{
		sp -= sizeof(T);
		T value = 0;
		uint8_t *addr = stack_data + sp;
		memcpy(addr, &value, sizeof(value));
		for ( typename std::vector<T>::iterator it_envp = sp_envp.begin();
				it_envp != sp_envp.end();
				++it_envp )
		{
			sp -= sizeof(T);
			value = (*it_envp) + stack_base;
			addr = stack_data + sp;
			memcpy(addr, &value, sizeof(value));
		}
		sp -= sizeof(T);
		value = 0;
		addr = stack_data + sp;
		memcpy(addr, &value, sizeof(value));
		for ( typename std::vector<T>::iterator it_argv = sp_argv.begin();
				it_argv != sp_argv.end();
				++it_argv )
		{
			sp -= sizeof(T);
			value = (*it_argv) + stack_base;
			addr = stack_data + sp;
			memcpy(addr, &value, sizeof(value));
		}
		sp -= sizeof(T);
		value = argc;
		addr = stack_data + sp;
		memcpy(addr, &value, sizeof(argc));
	}
	
	stack_blob = new unisim::util::blob::Blob<T>();
	stack_blob->Catch();
	
	stack_blob->SetEndian(endianness);
	
	stack_blob->SetStackBase(stack_base + sp);

	unisim::util::blob::Segment<T> *stack_segment = new unisim::util::blob::Segment<T>(
			unisim::util::blob::Segment<T>::TY_LOADABLE,
			unisim::util::blob::Segment<T>::SA_RW,
			0,
			stack_base,
			stack_size,
			stack_data);
	
	stack_blob->AddSegment(stack_segment);
	
	blob = new unisim::util::blob::Blob<T>();
	blob->Catch();
	blob->AddBlob(loader_blob);
	blob->AddBlob(stack_blob);

	return true;
}

template<class T>
bool
LinuxLoader<T>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &loader_export) return SetupLoad();
	if(srv_export == &blob_export) return SetupBlob();
	
	logger << DebugError << "Internal error" << EndDebugError;
	return false;
}

template<class T>
bool
LinuxLoader<T>::EndSetup() {
	return LoadStack();
}

template<class T>
bool
LinuxLoader<T>::
Load()
{
	if(!loader_import->Load()) return false;
	return LoadStack();
}

template <class T>
bool
LinuxLoader<T>::
LoadStack()
{
	if(!memory_import) return false;
	if(!stack_blob) return false;
	const std::vector<const unisim::util::blob::Segment<T> *> &segments =
		stack_blob->GetSegments();

	for ( typename std::vector<const unisim::util::blob::Segment<T> *>::const_iterator it = segments.begin();
			it != segments.end();
			it++ )
	{
		T stack_start;
		T stack_end;

		(*it)->GetAddrRange(stack_start, stack_end);
		if(verbose)
		{
			logger << DebugInfo 
				<< "Initializing stack at @0x" << std::hex << stack_start 
				<< " - @0x" << stack_end << std::dec << EndDebugInfo;
		}
	
		if ( !memory_import->WriteMemory((*it)->GetAddr(), 
					(*it)->GetData(), 
					(*it)->GetSize()) ) 
			return false;
	}
	
	return true;
}

template<class T>
const typename unisim::util::blob::Blob<T> *
LinuxLoader<T>::
GetBlob()
{
	return blob;
}

template<class T>
void
LinuxLoader<T>::
Log(T addr, const uint8_t *value, uint32_t size)
{
	logger << DebugInfo
			<< "0x" << std::hex << (unsigned long int)addr << std::dec
			<< " [" << (unsigned int)size << "] =" << std::hex;
	for (uint32_t i = 0; i < size; i++)
		logger << " " << (unsigned short int)value[i];
	logger << std::dec << EndDebugInfo;
}

template<class T>
void
LinuxLoader<T>::SetupArgsAndEnvs()
{
	if ( argc != argv.size() )
	{
		if ( argc < argv.size() )
		{
			// we have to delete entries on argv vector
			for ( ;
					param_argv.size() != argc;
					param_argv.pop_back() )
			{
				delete *param_argv.rbegin();
			}
			for ( ;
					argv.size() != argc;
					argv.pop_back() )
			{
				delete *argv.rbegin();
			}
		}
		else
		{
			// we have to add entries on the argv vector
			while ( argv.size() != argc )
			{
				unsigned int i = argv.size();
				std::stringstream argv_name, argv_desc, argv_val;
				argv_name << "argv[" << i << "]";
				argv_desc << "The '" << i << "' token in the command line.";
				argv_val << "Undefined argv[" << i << "] value";
				argv.push_back(new string(argv_val.str().c_str()));
				param_argv.push_back(
						new Parameter<string>(argv_name.str().c_str(), this, 
							*(argv[i]), argv_desc.str().c_str()));
			}
		}
	}

	if ( envc != envp.size() )
	{
		if ( envc < envp.size() )
		{
			// we have to delete entries on envp vector
			for ( ;
					param_envp.size() != argc;
					param_envp.pop_back() )
			{
				delete *param_envp.rbegin();
			}
			for ( ;
					envp.size() != argc;
					envp.pop_back() )
			{
				delete *envp.rbegin();
			}
		}
		else
		{
			// we have to add entries on the envp vector
			while ( envp.size() != envc )
			{
				unsigned int i = envp.size();
				std::stringstream envp_name, envp_desc, envp_val;
				envp_name << "envp[" << i << "]";
				envp_desc << "The '" << i << "' token in the environment.";
				envp_val << "Undefined envp[" << i << "] value";
				envp.push_back(new string(envp_val.str().c_str()));
				param_envp.push_back(
						new Parameter<string>(envp_name.str().c_str(), this,
							*(envp[i]), envp_desc.str().c_str()));
			}
		}
	}
}

} // end of linux_loader
} // end of loader namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_TCC__
