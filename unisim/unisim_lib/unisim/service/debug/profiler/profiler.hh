/*
 *  Copyright (c) 2017,
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
 
#ifndef __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_HH__
#define __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/http_server.hh>

#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/ieee754/ieee754.hh>

#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>

#include <systemc>

#include <pthread.h>

#include <inttypes.h>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <stack>
#include <set>
#include <iostream>
#include <fstream>

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

////////////////////////// Forward Declarations ///////////////////////////////

class Printer;
class Visitor;
template <typename ADDRESS> class AddressProfileBase;
template <typename T> class Sample;
template <typename ADDRESS, typename T> class AddressProfile;
class FunctionInstructionProfileBase;
template <typename ADDRESS, typename T> class FunctionInstructionProfile;
template <typename ADDRESS> class FunctionNameLocationConversionBase;
template <typename ADDRESS> class FunctionNameLocationConversion;
template <typename ADDRESS> class SourceCodeProfileBase;
template <typename ADDRESS, typename T> class SourceCodeProfile;
class AnnotatedSourceCodeFileBase;
template <typename ADDRESS, typename T> class AnnotatedSourceCodeFile;
class AnnotatedSourceCodeFileSetBase;
template <typename ADDRESS, typename T> class AnnotatedSourceCodeFileSet;
template <typename ADDRESS> class Profiler;
class FilenameIndex;

////////////////////////////// FileFormat /////////////////////////////////////

enum FileFormat
{
	F_FMT_TEXT = 0,
	F_FMT_HTML,
	F_FMT_CSV,
	F_FMT_SVG,
	F_FMT_NONE,
	NUM_FILE_FORMATS = F_FMT_NONE
};

std::ostream& operator << (std::ostream& os, FileFormat f_fmt);

std::string FileFormatSuffix(FileFormat f_fmt);

////////////////////////////// ReportFormat ///////////////////////////////////

enum ReportFormat
{
	R_FMT_TEXT,
	R_FMT_HTML,
	R_FMT_CSV,
	R_FMT_HTTP
};

////////////////////////////// to_string<> ////////////////////////////////////

template <typename T>
std::string to_string(const T& v);

///////////////////////////// c_string_to_CSV /////////////////////////////////

std::string c_string_to_CSV(const char *s);

//////////////////////////////// MakeDir //////////////////////////////////////

void MakeDir(const char *dirname);

//////////////////////////////// RealPath ////////////////////////////////////

std::string RealPath(const char *dirname);

//////////////////////////////// Indent /////////////////////////////////////

class Indent
{
public:
	Indent() : count(0) {}
	Indent& operator ++ () { ++count; return *this; }
	Indent& operator -- () { if(count) --count; return *this; }
private:
	friend std::ostream& operator << (std::ostream& os, const Indent& indent);
	unsigned int count;
};

inline std::ostream& operator << (std::ostream& os, const Indent& indent)
{
	for(unsigned int i = 0; i < indent.count; i++) os << '\t';
	return os;
}

////////////////////////////// StringPadder ///////////////////////////////////

class StringPadder
{
public:
	StringPadder(const std::string& _str, std::size_t _width) : str(_str), width(_width) {}
private:
	friend std::ostream& operator << (std::ostream& os, const StringPadder& string_padded);
	const std::string& str;
	std::size_t width;
};

inline std::ostream& operator << (std::ostream& os, const StringPadder& string_padded)
{
	std::size_t len = string_padded.str.length();
	std::size_t n = (len < string_padded.width) ? (string_padded.width - len) : 0;
	os << string_padded.str;
	for(unsigned int i = 0; i < n; i++) os << ' ';
	return os;
}

///////////////////////////////// Printer /////////////////////////////////////

class Printer
{
public:
	virtual ~Printer() {}
	virtual FileFormat GetFileFormat() const = 0;
	virtual void Print(std::ostream& os, Visitor& visitor) const = 0;
};

///////////////////////////////// Visitor /////////////////////////////////////

class Visitor
{
public:
	virtual ~Visitor() {}
	virtual const std::string& GetCSVDelimiter() const = 0;
	virtual const std::string& GetCSVHyperlink() const = 0;
	virtual const std::string& GetCSVArgSeparator() const = 0;
	virtual const std::string& GetRoot() const = 0;
	virtual const std::string& GetDomain() const = 0;
	virtual ReportFormat GetReportFormat() const = 0;
	virtual std::string GetFilePath(const std::string& filename) const = 0;
	virtual const std::string& GetDirPath() const = 0;
	virtual double GetRefreshPeriod() const = 0;
	virtual bool Visit(const std::string& dirname, const std::string& filename, const Printer& printer) = 0;
};

/////////////////////////////// FileVisitor ////////////////////////////////////

class FileVisitor : public Visitor
{
public:
	FileVisitor(const std::string& output_directory, ReportFormat r_fmt, const std::string& csv_delimiter, const std::string& csv_hyperlink, const std::string& csv_arg_separator, std::ostream& warn_log);
	virtual const std::string& GetCSVDelimiter() const;
	virtual const std::string& GetCSVHyperlink() const;
	virtual const std::string& GetCSVArgSeparator() const;
	virtual const std::string& GetRoot() const;
	virtual const std::string& GetDomain() const;
	virtual ReportFormat GetReportFormat() const;
	virtual std::string GetFilePath(const std::string& filename) const;
	virtual const std::string& GetDirPath() const;
	virtual double GetRefreshPeriod() const;
	virtual bool Visit(const std::string& dirname, const std::string& filename, const Printer& printer);
private:
	std::ofstream file;
	std::string output_directory;
	ReportFormat r_fmt;
	std::string csv_delimiter;
	std::string csv_hyperlink;
	std::string csv_arg_separator;
	std::ostream& warn_log;
	std::string dir_path;
};

///////////////////////////// OStreamContext //////////////////////////////////

class OStreamContext
{
public:
	OStreamContext(std::ostream& _os) : os(_os), fill(os.fill()), flags(os.flags()) {}
	~OStreamContext() { os.fill(fill); os.flags(flags); }
private:
	std::ostream& os;
	std::ostream::char_type fill;
	std::ios_base::fmtflags flags;
};

//////////////////////////// FilenameIndex ////////////////////////////////////

class FilenameIndex
{
public:
	unsigned int IndexFilename(const std::string& filename);
private:
	std::map<std::string, unsigned int> index;
};

//////////////////////////////// Ratio ////////////////////////////////////////

template <typename T, unsigned int SCALE = 1>
class RatioCalculator
{
public:
	RatioCalculator(const T& _divisor) : divisor(_divisor) {}
	double Compute(const T& value) { return (value * (double) SCALE) / divisor; }
private:
	const T& divisor;
};

//////////////////////////////// Sample ///////////////////////////////////////

template <typename T>
class Sample
{
public:
	Sample(unisim::kernel::variable::Variable<T> *_var)
		: var(_var)
		, prev_value()
		, curr_value()
		, diff_value()
	{
	}

	const T& Get() const { return diff_value; }
	void Capture();
	void Print(std::ostream& os) const { os << diff_value; }
	const char *GetSampledVariableName() const { return var->GetName(); }
	unisim::kernel::variable::Variable<T> *GetSampledVariable() const { return var; }
	
private:
	unisim::kernel::variable::Variable<T> *var;
	T prev_value;
	T curr_value;
	T diff_value;
};

////////////////////////// InstructionProfileBase /////////////////////////////

class InstructionProfileBase
{
public:
	virtual ~InstructionProfileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual const Printer& GetPrinter(FileFormat f_fmt) const = 0;
};

//////////////////////////// InstructionProfile<> /////////////////////////////

template <typename ADDRESS, typename T>
class InstructionProfile : public InstructionProfileBase
{
public:
	InstructionProfile(const AddressProfile<ADDRESS, T> *addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if);
	virtual ~InstructionProfile();
	
	virtual const char *GetSampledVariableName() const { return addr_profile->GetSampledVariableName(); }
	virtual const Printer& GetPrinter(FileFormat f_fmt) const;
private:
	struct InstructionProfilePrinter : Printer
	{
		InstructionProfilePrinter(InstructionProfile<ADDRESS, T>& instruction_profile, FileFormat f_fmt);
		virtual FileFormat GetFileFormat() const;
		virtual void Print(std::ostream& os, Visitor& visitor) const;
		
	private:
		InstructionProfile<ADDRESS, T>& instruction_profile;
		FileFormat f_fmt;
	};

	const AddressProfile<ADDRESS, T> *addr_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	FilenameIndex *filename_index;

	unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if;
	InstructionProfilePrinter *instruction_profile_printers[NUM_FILE_FORMATS];
	
	void Print(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const;
};

//////////////////////////////// SLoc //////////////////////////////////////

class SLoc
{
public:
	SLoc() : filename(), lineno(1) {}
	SLoc(const char *_filename, unsigned int _lineno) : filename(_filename), lineno(_lineno) {}

	const char *GetFilename() const { return filename.c_str(); }
	unsigned int GetLineNo() const { return lineno; }
	
	int operator < (const SLoc& sloc) const { return (filename < sloc.filename) || ((filename == sloc.filename) && (lineno < sloc.lineno)); }
	int operator == (const SLoc& sloc) const { return (filename == sloc.filename) && (lineno == sloc.lineno); }
	
	void Print(std::ostream& os) const { os << "<" << filename << ":" << lineno << ">"; }
	
private:
	std::string filename;
	unsigned int lineno;
};

inline std::ostream& operator << (std::ostream& os, const SLoc& sloc)
{
	sloc.Print(os);
	return os;
}

////////////////////// FunctionNameLocationConversionBase /////////////////////////

template <typename ADDRESS>
class FunctionNameLocationConversionBase
{
public:
	virtual ~FunctionNameLocationConversionBase() {}
	virtual bool FunctionNameToAddress(const char *func_name, ADDRESS& addr) const = 0;
	virtual const char *AddressToFunctionName(ADDRESS addr, bool& is_entry_point) const = 0;
	virtual const SLoc *FunctionNameToSLoc(const char *func_name) const = 0;
	virtual const char *SLocToFunctionName(const SLoc& sloc, bool& is_entry_point) const = 0;
};

//////////////////////// FunctionNameLocationConversion<> /////////////////////////

template <typename ADDRESS>
class FunctionNameLocationConversion : public FunctionNameLocationConversionBase<ADDRESS>
{
public:
	FunctionNameLocationConversion(unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, std::ostream& warn_log);
	
	virtual bool FunctionNameToAddress(const char *func_name, ADDRESS& addr) const;
	virtual const char *AddressToFunctionName(ADDRESS addr, bool& is_entry_point) const;
	virtual const SLoc *FunctionNameToSLoc(const char *func_name) const;
	virtual const char *SLocToFunctionName(const SLoc& sloc, bool& is_entry_point) const;
	
private:
	std::map<std::string, ADDRESS> func_to_addr;
	std::map<ADDRESS, std::string> addr_to_func;
	std::map<std::string, SLoc> func_to_sloc;
	std::map<SLoc, std::string> sloc_to_func;
};

////////////////////////// AddressProfileBase<> ///////////////////////////////

template <typename ADDRESS>
class AddressProfileBase
{
public:
	virtual ~AddressProfileBase() {}
	virtual void Capture(ADDRESS addr, unsigned int length) = 0;
	virtual const char *GetSampledVariableName() const = 0;
	virtual unisim::kernel::VariableBase *GetSampledVariable() const = 0;
	virtual std::string GetCumulativeValueAsString() const = 0;
	virtual InstructionProfileBase *CreateInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index) const = 0;
	virtual FunctionInstructionProfileBase *CreateFunctionInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index) const = 0;
	virtual SourceCodeProfileBase<ADDRESS> *CreateSourceCodeProfile() const = 0;
	virtual const Printer& GetPrinter(FileFormat) const = 0;
	virtual void Clear() = 0;
};

///////////////////////////// AddressProfile<> ////////////////////////////////

template <typename ADDRESS, typename T>
class AddressProfile : public AddressProfileBase<ADDRESS>, public unisim::util::debug::Profile<ADDRESS, T>
{
public:
	typedef unisim::util::debug::Profile<ADDRESS, T> Super;
	
	AddressProfile(unisim::kernel::variable::Variable<T> *stat, unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if, unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, std::ostream& warn_log);
	virtual ~AddressProfile();
	
	virtual void Capture(ADDRESS addr, unsigned int length);
	virtual const char *GetSampledVariableName() const { return sample.GetSampledVariableName(); }
	virtual unisim::kernel::VariableBase *GetSampledVariable() const { return sample.GetSampledVariable(); }
	virtual std::string GetCumulativeValueAsString() const { return to_string(this->GetWeight()); }
	const T& GetCumulativeValue() const { return this->GetWeight(); }
	virtual InstructionProfileBase *CreateInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index) const;
	virtual FunctionInstructionProfileBase *CreateFunctionInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index) const;
	virtual SourceCodeProfileBase<ADDRESS> *CreateSourceCodeProfile() const;
	virtual void Clear() { Super::Clear(); }
	
	bool GetValue(ADDRESS addr, T& value) const;
	std::pair<T, T> GetValueRange() const;
	
	virtual const Printer& GetPrinter(FileFormat) const;
private:
	struct AddressProfilePrinter : Printer
	{
		AddressProfilePrinter(AddressProfile<ADDRESS, T>& addr_profile, FileFormat f_fmt);
		virtual FileFormat GetFileFormat() const;
		virtual void Print(std::ostream& os, Visitor& visitor) const;
		
	private:
		AddressProfile<ADDRESS, T>& addr_profile;
		FileFormat f_fmt;
	};

	std::ostream& warn_log;
	unisim::service::interfaces::Disassembly<ADDRESS> *disasm_if;
	unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	Sample<T> sample;
	AddressProfilePrinter *addr_profile_printers[NUM_FILE_FORMATS];
	
	void Print(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const;
};

////////////////////////// SourceCodeProfileBase<> ////////////////////////////

template <typename ADDRESS>
class SourceCodeProfileBase
{
public:
	virtual ~SourceCodeProfileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual void Update() = 0;
	virtual AnnotatedSourceCodeFileSetBase *CreateAnnotatedSourceCodeFileSet(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, const char *search_path) const = 0;
};

///////////////////////// FunctionInstructionProfileBase /////////////////////////////////

class FunctionInstructionProfileBase
{
public:
	virtual ~FunctionInstructionProfileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual const Printer& GetTablePrinter(FileFormat f_fmt) const = 0;
	virtual const Printer& GetHistogramPrinter(FileFormat f_fmt) const = 0;
};

template <typename T>
struct Divider
{
	static double Divide(const T& dividend, const T& divisor)
	{
		return (double) dividend / (double) divisor;
	}
};

template <>
struct Divider<sc_core::sc_time>
{
	static double Divide(const sc_core::sc_time& dividend, const sc_core::sc_time& divisor)
	{
		return dividend / divisor;
	}
};

template <typename T>
class Scaler
{
public:
	Scaler(double _factor, const std::pair<T, T>& _value_range) : factor(_factor), value_range(_value_range) {}
	double Scale(const T& value)
	{
		return ((value_range.first != value_range.second) && (value >= value_range.first) && (value <= value_range.second))
			? ((double)(value - value_range.first) / (double)(value_range.second - value_range.first)) * factor
			: 0.0;
	}
private:
	double factor;
	std::pair<T, T> value_range;
};

template <>
class Scaler<sc_core::sc_time>
{
public:
	Scaler(unsigned int _factor, const std::pair<sc_core::sc_time, sc_core::sc_time>& _value_range) : factor(_factor), value_range(_value_range) {}
	double Scale(const sc_core::sc_time& value)
	{
		return ((value_range.first != value_range.second) && (value >= value_range.first) && (value <= value_range.second))
			? factor * ((value - value_range.first) / (value_range.second - value_range.first))
			: 0.0;
	}
private:
	unsigned int factor;
	std::pair<sc_core::sc_time, sc_core::sc_time> value_range;
};

template <typename T>
class Quantizer
{
public:
	Quantizer(unsigned int _factor, const std::pair<T, T>& _value_range) : factor(_factor), value_range(_value_range) {}
	unsigned int Quantize(const T& value)
	{
		return ((value_range.first != value_range.second) && (value >= value_range.first) && (value <= value_range.second))
			? floor/*ceil*/(((double)(value - value_range.first) / (double)(value_range.second - value_range.first)) * factor)
			: 0;
	}
private:
	unsigned int factor;
	std::pair<T, T> value_range;
};

template <>
class Quantizer<sc_core::sc_time>
{
public:
	Quantizer(unsigned int _factor, const std::pair<sc_core::sc_time, sc_core::sc_time>& _value_range) : factor(_factor), value_range(_value_range) {}
	unsigned int Quantize(const sc_core::sc_time& value)
	{
		return ((value_range.first != value_range.second) && (value >= value_range.first) && (value <= value_range.second))
			? floor/*ceil*/(factor * ((value - value_range.first) / (value_range.second - value_range.first)))
			: 0;
	}
private:
	unsigned int factor;
	std::pair<sc_core::sc_time, sc_core::sc_time> value_range;
};

////////////////////// FunctionInstructionProfile<> ///////////////////////////

template <typename ADDRESS, typename T>
class FunctionInstructionProfile : public FunctionInstructionProfileBase
{
public:
	FunctionInstructionProfile(const AddressProfile<ADDRESS, T> *addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if);
	virtual ~FunctionInstructionProfile();
	
	virtual const char *GetSampledVariableName() const { return addr_profile->GetSampledVariableName(); }
	virtual const Printer& GetTablePrinter(FileFormat f_fmt) const;
	virtual const Printer& GetHistogramPrinter(FileFormat f_fmt) const;
private:
	struct TablePrinter : Printer
	{
		TablePrinter(FunctionInstructionProfile<ADDRESS, T>& func_insn_profile, FileFormat f_fmt);
		virtual FileFormat GetFileFormat() const;
		virtual void Print(std::ostream& os, Visitor& visitor) const;
	private:
		FunctionInstructionProfile<ADDRESS, T>& func_insn_profile;
		FileFormat f_fmt;
	};
	
	struct HistogramPrinter : Printer
	{
		HistogramPrinter(FunctionInstructionProfile<ADDRESS, T>& func_insn_profile, FileFormat f_fmt);
		virtual FileFormat GetFileFormat() const;
		virtual void Print(std::ostream& os, Visitor& visitor) const;
	private:
		FunctionInstructionProfile<ADDRESS, T>& func_insn_profile;
		FileFormat f_fmt;
	};

	const AddressProfile<ADDRESS, T> *addr_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	FilenameIndex *filename_index;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	std::map<std::string, T> func_insn_profile;
	std::pair<T, T> value_range;
	T cumulative_value;
	
	TablePrinter *table_printers[NUM_FILE_FORMATS];
	HistogramPrinter *histogram_printers[NUM_FILE_FORMATS];
	
	void Init();
	void Clear();
	void PrintTable(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const;
	void PrintHistogram(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const;
};

//////////////////////////// SourceCodeProfile<> //////////////////////////////

template <typename ADDRESS, typename T>
class SourceCodeProfile : public SourceCodeProfileBase<ADDRESS>
{
public:
	SourceCodeProfile(const AddressProfile<ADDRESS, T> *addr_profile, unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, std::ostream& warn_log);
	
	virtual const char *GetSampledVariableName() const { return addr_profile->GetSampledVariableName(); }
	virtual void Print(std::ostream& os, Visitor& visitor) const;
	virtual void Update();
	virtual AnnotatedSourceCodeFileSetBase *CreateAnnotatedSourceCodeFileSet(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, const char *search_path) const;
	
	const std::set<std::string>& GetSourceFilenameSet() const { return source_filename_set; }
	const T& GetValue(const char *filename, unsigned int lineno) const;
	const std::pair<T, T>& GetValueRange() const { return value_range; }
private:

	bool FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *>& stmts, ADDRESS addr);
	
	std::ostream& warn_log;
	const AddressProfile<ADDRESS, T> *addr_profile;
	unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if;
	unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if;
	std::map<ADDRESS, T> other_profile;
	std::map<SLoc, T> sloc_profile;
	std::set<std::string> source_filename_set;
	std::pair<T, T> value_range;
};

//////////////////////// AnnotatedSourceCodeFileBase //////////////////////////

class AnnotatedSourceCodeFileBase
{
public:
	virtual ~AnnotatedSourceCodeFileBase() {}
	virtual const char *GetSampledVariableName() const = 0;
	virtual const char *GetFilename() const = 0;
	virtual const char *GetRealFilename() const = 0;
	virtual const Printer& GetPrinter(FileFormat) const = 0;
};

///////////////////////// AnnotatedSourceCodeFile<> ///////////////////////////

template <typename ADDRESS, typename T>
class AnnotatedSourceCodeFile : public AnnotatedSourceCodeFileBase
{
public:
	AnnotatedSourceCodeFile(const char *_filename, const SourceCodeProfile<ADDRESS, T> *_source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, const char *search_path, std::ostream& warn_log);
	virtual ~AnnotatedSourceCodeFile();
	
	virtual const char *GetSampledVariableName() const { return source_code_profile->GetSampledVariableName(); }
	virtual const char *GetFilename() const { return filename.c_str(); }
	virtual const char *GetRealFilename() const { return real_filename.c_str(); }
	virtual const Printer& GetPrinter(FileFormat f_fmt) const;
private:
	struct AnnotatedSourceCodeFilePrinter : Printer
	{
		AnnotatedSourceCodeFilePrinter(AnnotatedSourceCodeFile<ADDRESS, T>& annotated_source_code_file, FileFormat f_fmt);
		virtual FileFormat GetFileFormat() const;
		virtual void Print(std::ostream& os, Visitor& visitor) const;
		
	private:
		AnnotatedSourceCodeFile<ADDRESS, T>& annotated_source_code_file;
		FileFormat f_fmt;
	};

	std::ostream& warn_log;
	std::string filename;
	std::string real_filename;
	std::string search_path;
	const SourceCodeProfile<ADDRESS, T> *source_code_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	FilenameIndex *filename_index;
	std::vector<std::string> content;
	std::pair<T, T> value_range;
	AnnotatedSourceCodeFilePrinter *annotated_source_code_file_printers[NUM_FILE_FORMATS];

	bool LocateFile(const char *filename, std::string& match_file_path);
	void Init();
	void Clear();
	void Print(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const;
};

/////////////////////// AnnotatedSourceCodeFileSetBase ////////////////////////

class AnnotatedSourceCodeFileSetBase
{
public:
	virtual ~AnnotatedSourceCodeFileSetBase() {}
	virtual const char *GetSampledVariableName() = 0;
	virtual void Output(Visitor& visitor) = 0;
};

/////////////////////// AnnotatedSourceCodeFileSet<> //////////////////////////

template <typename ADDRESS, typename T>
class AnnotatedSourceCodeFileSet : public AnnotatedSourceCodeFileSetBase
{
public:
	AnnotatedSourceCodeFileSet(const SourceCodeProfile<ADDRESS, T> *source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, const char *search_path, std::ostream& warn_log);
	virtual ~AnnotatedSourceCodeFileSet();
	
	virtual const char *GetSampledVariableName() { return source_code_profile->GetSampledVariableName(); }
	virtual void Output(Visitor& visitor);
private:
	std::ostream& warn_log;
	const SourceCodeProfile<ADDRESS, T> *source_code_profile;
	const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv;
	FilenameIndex *filename_index;
	std::string search_path;
	std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *> annotated_source_code_files;
	
	void Clear();
	void Init();
};

//////////////////////////////// Profiler<> ///////////////////////////////////

template <typename ADDRESS>
class Profiler
	: public unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
	, public unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	, public unisim::kernel::Service<unisim::service::interfaces::HttpServer>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>
	, public unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Registers>
	, public unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>
	, public unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
	, public unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
{
public:
	// Exports to debugger
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                debug_yielding_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_export;
	
	// Exports to HTTP server
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer>                   http_server_export;
	
	// Imports from debugger
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYieldingRequest>         debug_yielding_request_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >  debug_event_trigger_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >        disasm_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >             memory_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>                    registers_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >  symbol_table_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::StatementLookup<ADDRESS> >    stmt_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::BackTrace<ADDRESS> >          backtrace_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::Profiling<ADDRESS> >          profiling_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugInfoLoading>             debug_info_loading_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >   data_object_lookup_import;
	unisim::kernel::ServiceImport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >   subprogram_lookup_import;
	
	Profiler(const char *name, Object *parent = 0);
	virtual ~Profiler();

	// unisim::service::interfaces::DebugYielding
	virtual void DebugYield();
	
	// unisim::service::interfaces::DebugEventListener<ADDRESS>
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event);
	
	// unisim::service::interfaces::HttpServer
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);

	virtual bool EndSetup();
	virtual void OnDisconnect();
	
	void Output();

private:
	struct PropertySetter : public unisim::util::hypapp::Form_URL_Encoded_Decoder
	{
		PropertySetter(Profiler<ADDRESS>& _profiler);
		virtual bool FormAssign(const std::string& name, const std::string& value);
		void Apply();
	private:
		Profiler<ADDRESS>& profiler;
		bool clear_all;
		std::vector<std::string> clear;
	};

	template <typename T> bool TryProfile(unisim::kernel::VariableBase *var);
	
	unisim::kernel::logger::Logger logger;
	std::string search_path;
	std::string filename;
	std::string sampled_variables;
	std::string output_directory;
	std::string csv_delimiter;
	std::string csv_hyperlink;
	std::string csv_arg_separator;
	bool enable_text_report;
	bool enable_html_report;
	bool enable_csv_report;
	bool verbose;
	double http_refresh_period;
	unisim::kernel::variable::Parameter<std::string> param_search_path;
	unisim::kernel::variable::Parameter<std::string> param_filename;
	unisim::kernel::variable::Parameter<std::string> param_sampled_variables;
	unisim::kernel::variable::Parameter<std::string> param_output_directory;
	unisim::kernel::variable::Parameter<std::string> param_csv_delimiter;
	unisim::kernel::variable::Parameter<std::string> param_csv_hyperlink;
	unisim::kernel::variable::Parameter<std::string> param_csv_arg_separator;
	unisim::kernel::variable::Parameter<bool> param_enable_text_report;
	unisim::kernel::variable::Parameter<bool> param_enable_html_report;
	unisim::kernel::variable::Parameter<bool> param_enable_csv_report;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<double> param_http_refresh_period;

	bool listening_commit;
	bool trap;
	unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event;
	unsigned int num_sampled_variables;
	FunctionNameLocationConversion<ADDRESS> *func_name_loc_conv;
	bool need_update;
	std::vector<AddressProfileBase<ADDRESS> *> addr_profiles; // address profile for each variable
	std::vector<FunctionInstructionProfileBase *> func_insn_profiles; // function profile for each variable
	ADDRESS pc;
	unsigned int length;
	std::vector<InstructionProfileBase *> instruction_profiles; // instruction profile for each variable
	std::vector<SourceCodeProfileBase<ADDRESS> *> source_code_profiles; // source code profile for each variable
	std::vector<AnnotatedSourceCodeFileSetBase *> annotated_source_code_file_sets; // annotated source code file set for each variable
	std::vector<FilenameIndex *> filename_indexes; // filename index for each variable
	std::map<std::string, double> http_print_times_per_path; // last print time (for reporting over HTTP) in seconds for each HTTP request path
	
	pthread_mutex_t mutex;
	
	bool ListenCommit();
	bool UnlistenCommit();
	void LoadDebugInfo();
	void Clear();
	void Clear(const std::string& variable_name);
	void Update();
	void PrintIndex(std::ostream& os, Visitor& visitor) const;
	//void Print(std::ostream& os, DebugVisitor& dbg_visitor);
	void Output(Visitor& visitor);
	void Lock();
	void Unlock();
};

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
