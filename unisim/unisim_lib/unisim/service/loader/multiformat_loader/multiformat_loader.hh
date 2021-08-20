/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_SERVICE_LOADER_MULTIFORMAT_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_MULTIFORMAT_LOADER_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/loader/elf_loader/elf32_loader.hh>
#include <unisim/service/loader/elf_loader/elf64_loader.hh>
#include <unisim/service/loader/raw_loader/raw_loader.hh>
#include <unisim/service/loader/s19_loader/s19_loader.hh>
#include <unisim/service/loader/coff_loader/coff_loader.hh>
#include <unisim/service/tee/loader/tee.hh>
#include <unisim/service/tee/blob/tee.hh>
#include <unisim/service/tee/symbol_table_lookup/tee.hh>
#include <unisim/service/tee/stmt_lookup/tee.hh>
#include <unisim/service/tee/backtrace/tee.hh>


namespace unisim {
namespace service {
namespace loader {
namespace multiformat_loader {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::StatementLookup;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::BackTrace;
using unisim::service::interfaces::Registers;
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::loader::elf_loader::Elf64Loader;
using unisim::service::loader::raw_loader::RawLoader;
using unisim::service::loader::s19_loader::S19_Loader;
using unisim::service::loader::coff_loader::CoffLoader;
	
template <class MEMORY_ADDR, unsigned int MAX_MEMORIES> class MemoryMapper;
	
template <class MEMORY_ADDR, unsigned int MAX_MEMORIES = 16>
class MultiFormatLoader
	: public Object
{
public:
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<MEMORY_ADDR> > blob_export;
	ServiceExport<SymbolTableLookup<MEMORY_ADDR> > symbol_table_lookup_export;
	ServiceExport<StatementLookup<MEMORY_ADDR> > stmt_lookup_export;
	ServiceExport<BackTrace<MEMORY_ADDR> > backtrace_export;
	
	ServiceImport<Memory<MEMORY_ADDR> > *memory_import[MAX_MEMORIES];
	ServiceImport<Registers> registers_import;
	
	MultiFormatLoader(const char *name, Object *parent = 0);
	virtual ~MultiFormatLoader();
protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool verbose_parser;
	bool IsVerbose() const;
	bool IsVerboseParser() const;
private:
	unisim::service::tee::loader::Tee<> *tee_loader;
	unisim::service::tee::blob::Tee<MEMORY_ADDR> *tee_blob;
	unisim::service::tee::symbol_table_lookup::Tee<MEMORY_ADDR> *tee_symbol_table_lookup;
	unisim::service::tee::stmt_lookup::Tee<MEMORY_ADDR> *tee_stmt_lookup;
	unisim::service::tee::backtrace::Tee<MEMORY_ADDR> *tee_backtrace;
	MemoryMapper<MEMORY_ADDR, MAX_MEMORIES> *memory_mapper;
	std::string filename;
	
	typedef enum
	{
		FFMT_UNKNOWN,
		FFMT_ELF32,
		FFMT_ELF64,
		FFMT_S19,
		FFMT_COFF,
		FFMT_RAW
	} FileFormat;
	
	typedef enum
	{
		TOK_COMMA,
#if 0
		TOK_COLON,
#endif
		TOK_EQUAL,
		TOK_STRING,
		TOK_EOF
	} Token;
	
	typedef enum
	{
		OPT_UNKNOWN,
		OPT_FILENAME,
		OPT_FORMAT
	} LoadStatementOptionType;
	
	class LoadStatementOption
	{
	public:
		LoadStatementOptionType type;
		unsigned int pos;
		std::string value;
		
		LoadStatementOption(LoadStatementOptionType _type, unsigned int _pos, const std::string& _value);
	};

	class LoadStatement
	{
	public:
		unsigned int pos;
		std::vector<LoadStatementOption *> opts;
		
		LoadStatement(unsigned int _pos);
		~LoadStatement();
		
		void AddOption(LoadStatementOption *opt);
		LoadStatementOption *FindOption(LoadStatementOptionType opt_type);
	};
	
	std::vector<LoadStatementOptionType> positional_option_types;
	std::vector<RawLoader<MEMORY_ADDR> *> raw_loaders;
	std::vector<Elf32Loader<MEMORY_ADDR> *> elf32_loaders;
	std::vector<Elf64Loader<MEMORY_ADDR> *> elf64_loaders;
	std::vector<S19_Loader<MEMORY_ADDR> *> s19_loaders;
	std::vector<CoffLoader<MEMORY_ADDR> *> coff_loaders;
	
	Parameter<bool> param_verbose;
	Parameter<bool> param_verbose_parser;
	Parameter<std::string> param_filename;
	
	unsigned int ReadToken(const std::string& s, unsigned int pos, Token& tok, std::string& tok_value);
	std::string GetTokenName(Token tok, const std::string& tok_value);
	LoadStatementOptionType GetOptionType(const std::string& opt_name);
	FileFormat GuessFileFormat(const std::string& filename_to_guess_file_format);
	const char *GetFileFormatName(FileFormat file_fmt);
	bool IsFileFormat(const char *name) const;
	FileFormat GetFileFormat(const char *name);
	LoadStatement *ParseLoadStatement(const std::string& s, unsigned int& pos);
	LoadStatementOption *ParseLoadStatementOption(const std::string& s, unsigned int& pos, unsigned int opt_idx);
	void PrettyPrintSyntaxErrorLocation(const char *s, unsigned int pos);
};

template <class MEMORY_ADDR>
class AddressRange
{
public:
	MEMORY_ADDR low;
	MEMORY_ADDR high;
	
	AddressRange();
	AddressRange(const AddressRange<MEMORY_ADDR>& ar);
	bool IsEmpty() const;
	MEMORY_ADDR GetAmplitude() const;
	std::string ToString() const;
	AddressRange<MEMORY_ADDR>& operator = (const AddressRange<MEMORY_ADDR>& ar);
	int operator == (const AddressRange<MEMORY_ADDR>& ar) const;
	int operator != (const AddressRange<MEMORY_ADDR>& ar) const;
};

template <class MEMORY_ADDR>
class Mapping
{
public:
	Mapping(unsigned int _memory_num, const AddressRange<MEMORY_ADDR>& _addr_range, MEMORY_ADDR _translation)
		: memory_num(_memory_num)
		, addr_range(_addr_range)
		, translation(_translation)
	{
	}
	
	unsigned int memory_num;
	AddressRange<MEMORY_ADDR> addr_range;
	MEMORY_ADDR translation;
};

template <class MEMORY_ADDR, unsigned int MAX_MEMORIES>
class MemoryMapper
	: public Service<Memory<MEMORY_ADDR> >
	, public Client<Memory<MEMORY_ADDR> >
{
public:
	ServiceExport<Memory<MEMORY_ADDR> > memory_export;
	ServiceImport<Memory<MEMORY_ADDR> > *memory_import[MAX_MEMORIES];
	
	MemoryMapper(const char *name, Object *parent);
	virtual ~MemoryMapper();

	virtual bool BeginSetup();
	
	virtual void ResetMemory();
	virtual bool ReadMemory(MEMORY_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(MEMORY_ADDR addr, const void *buffer, uint32_t size);

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool verbose_parser;
	bool IsVerbose() const;
	bool IsVerboseParser() const;
private:
	typedef enum
	{
		TOK_STRING,
		TOK_ADDRESS,
		TOK_COMMA,
		TOK_COLON,
		TOK_MINUS,
		TOK_PLUS,
		TOK_EQUAL,
		TOK_EOF
	} Token;

	typedef enum
	{
		OPT_UNKNOWN,
		OPT_MEMORY,
		OPT_RANGE,
		OPT_TRANSLATION
	} MappingStatementOptionType;
	
	class MappingStatementOption
	{
	public:
		MappingStatementOptionType type;
		unsigned int pos;
		std::string value;
		AddressRange<MEMORY_ADDR> range;
		MEMORY_ADDR translation;
		
		MappingStatementOption(MappingStatementOptionType _type, unsigned int _pos, const std::string& _value);
		MappingStatementOption(MappingStatementOptionType _type, unsigned int _pos, const AddressRange<MEMORY_ADDR>& _range);
		MappingStatementOption(MappingStatementOptionType _type, unsigned int _pos, MEMORY_ADDR _translation);
	};

	class MappingStatement
	{
	public:
		unsigned int pos;
		std::vector<MappingStatementOption *> opts;
		
		MappingStatement(unsigned int _pos);
		~MappingStatement();
		void AddOption(MappingStatementOption *opt);
		MappingStatementOption *FindOption(MappingStatementOptionType opt_type);
	};
	
	std::vector<MappingStatementOptionType> positional_option_types;
	std::string mapping;
	std::vector<Mapping<MEMORY_ADDR> *> mapping_table;

	Parameter<bool> param_verbose;
	Parameter<bool> param_verbose_parser;
	Parameter<std::string> param_mapping;
	
	unsigned int ReadToken(const std::string& s, unsigned int pos, Token& tok, std::string& tok_value, MEMORY_ADDR& tok_addr_value);
	std::string GetTokenName(Token tok, const std::string& tok_value);
	MappingStatementOptionType GetOptionType(const std::string& opt_name);
	bool ParseAddressRange(const std::string& s, unsigned int& pos, AddressRange<MEMORY_ADDR>& addr_range);
	bool ParseAddressTranslationOffset(const std::string& s, unsigned int& pos, MEMORY_ADDR& translation);
	MappingStatementOption *ParseMappingStatementOption(const std::string& s, unsigned int& pos, unsigned int opt_idx);
	MappingStatement *ParseMappingStatement(const std::string& s, unsigned int& pos);
	int FindMemory(const char *name);
	void PrettyPrintSyntaxErrorLocation(const char *s, unsigned int pos);
};

} // end of namespace multiformat_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_LOADER_MULTIFORMAT_LOADER_HH__
