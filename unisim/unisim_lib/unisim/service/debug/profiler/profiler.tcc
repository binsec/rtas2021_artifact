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
 
#ifndef __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_TCC_
#define __UNISIM_SERVICE_DEBUG_PROFILER_PROFILER_TCC_

#include <unisim/service/debug/profiler/profiler.hh>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <set>
#include <stdexcept>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/data_object_initializer.hh>
#include <unisim/util/debug/data_object_initializer.tcc>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/debug/profile.tcc>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/host_time/time.hh>
#include <fstream>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access() and mkdir()
#ifdef GetPrinter
#undef GetPrinter
#endif
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename T>
std::string to_string(const T& v)
{
	std::ostringstream sstr;
	sstr << v;
	return sstr.str();
}

//////////////////////////////// Sample ///////////////////////////////////////

template <typename T>
void Sample<T>::Capture()
{
	prev_value = curr_value;
	curr_value = var->Get();
	diff_value = curr_value - prev_value;
}

////////////////////// FunctionNameLocationConversion<> ///////////////////////

template <typename ADDRESS>
FunctionNameLocationConversion<ADDRESS>::FunctionNameLocationConversion(unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, std::ostream& warn_log)
	: func_to_addr()
	, addr_to_func()
	, func_to_sloc()
	, sloc_to_func()
{
	std::list<const unisim::util::debug::Symbol<ADDRESS> *> func_symbols;
	
	symbol_table_lookup_if->GetSymbols(func_symbols, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
	
	typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>::const_iterator func_symbol_it;
	
	for(func_symbol_it = func_symbols.begin(); func_symbol_it != func_symbols.end(); func_symbol_it++)
	{
		const unisim::util::debug::Symbol<ADDRESS> *func_symbol = *func_symbol_it;
		
		const char *func_name = func_symbol->GetName();
		
		ADDRESS func_addr = func_symbol->GetAddress();
		ADDRESS func_size = func_symbol->GetSize();
		ADDRESS func_start_addr = func_addr;
		ADDRESS func_end_addr = func_addr + func_size - 1;

		typename std::map<std::string, ADDRESS>::iterator func_to_addr_it = func_to_addr.find(func_name);
		
		if(func_to_addr_it == func_to_addr.end())
		{
			func_to_addr[func_name] = func_start_addr;
		}
		else
		{
			warn_log << "Multiple definition of function \"" << func_name << "\"" << std::endl;
		}
		
		ADDRESS addr;
		for(addr = func_start_addr; addr <= func_end_addr; addr++)
		{
			addr_to_func[addr] = func_name;
			
			//const unisim::util::debug::Statement<ADDRESS> *stmt = stmt_lookup_if->FindStatement(addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT);
			
			std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
			
			if(stmt_lookup_if->FindStatements(stmts, addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT))
			{
				typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::const_iterator stmt_it;
				
				for(stmt_it = stmts.begin(); stmt_it != stmts.end(); stmt_it++)
				{
					const unisim::util::debug::Statement<ADDRESS> *stmt = *stmt_it;
				
					const char *source_filename = stmt->GetSourceFilename();
					
					if(source_filename)
					{
						unsigned int lineno = stmt->GetLineNo();
					
						std::string source_path;
						const char *source_dirname = stmt->GetSourceDirname();
						if(source_dirname)
						{
							source_path += source_dirname;
							source_path += '/';
						}
						source_path += source_filename;
						
						SLoc sloc = SLoc(source_path.c_str(), lineno);
				
						sloc_to_func[sloc] = func_name;
						
						if(func_to_sloc.find(func_name) == func_to_sloc.end())
						{
							// temporary heuristic: associate first sloc (in address order) to function
							func_to_sloc[func_name] = sloc;
						}
					}
				}
			}
		}
	}
}

template <typename ADDRESS>
bool FunctionNameLocationConversion<ADDRESS>::FunctionNameToAddress(const char *func_name, ADDRESS& addr) const
{
	typename std::map<std::string, ADDRESS>::const_iterator func_to_addr_it = func_to_addr.find(func_name);
	
	if(func_to_addr_it != func_to_addr.end())
	{
		addr = (*func_to_addr_it).second;
		return true;
	}
	
	return false;
}

template <typename ADDRESS>
const char *FunctionNameLocationConversion<ADDRESS>::AddressToFunctionName(ADDRESS addr, bool& is_entry_point) const
{
	typename std::map<ADDRESS, std::string>::const_iterator addr_to_func_it = addr_to_func.find(addr);
	
	if(addr_to_func_it != addr_to_func.end())
	{
		const std::string& func_name = (*addr_to_func_it).second;
		
		typename std::map<std::string, ADDRESS>::const_iterator func_to_addr_it = func_to_addr.find(func_name);
		
		ADDRESS func_addr = (*func_to_addr_it).second;
		
		is_entry_point = (addr == func_addr);
		
		return func_name.c_str();
	}
	
	return 0;
}

template <typename ADDRESS>
const SLoc *FunctionNameLocationConversion<ADDRESS>::FunctionNameToSLoc(const char *func_name) const
{
	typename std::map<std::string, SLoc>::const_iterator func_to_sloc_it = func_to_sloc.find(func_name);
	
	if(func_to_sloc_it != func_to_sloc.end())
	{
		const SLoc *sloc = &(*func_to_sloc_it).second; // returning a pointer is dangerous (any insertion/deletion may invalidate pointer)
	
		return sloc;
	}
	
	return 0;
}

template <typename ADDRESS>
const char *FunctionNameLocationConversion<ADDRESS>::SLocToFunctionName(const SLoc& sloc, bool& is_entry_point) const
{
	typename std::map<SLoc, std::string>::const_iterator sloc_to_func_it = sloc_to_func.find(sloc);
	
	if(sloc_to_func_it != sloc_to_func.end())
	{
		const char *func_name = (*sloc_to_func_it).second.c_str(); // returning a pointer is dangerous (any insertion/deletion may invalidate pointer)
		
		typename std::map<std::string, SLoc>::const_iterator func_to_sloc_it = func_to_sloc.find(func_name);
		
		if(func_to_sloc_it != func_to_sloc.end())
		{
			const SLoc& entry_point_sloc = (*func_to_sloc_it).second;
		
			is_entry_point = (sloc == entry_point_sloc);
			return func_name; // returning a pointer is dangerous (any insertion/deletion may invalidate pointer)
		}
	}
	
	return 0;
}

///////////////////////////// AddressProfile<> ////////////////////////////////

template <typename ADDRESS, typename T>
AddressProfile<ADDRESS, T>::AddressProfile(unisim::kernel::variable::Variable<T> *stat, unisim::service::interfaces::Disassembly<ADDRESS> *_disasm_if, unisim::service::interfaces::StatementLookup<ADDRESS> *_stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *_symbol_table_lookup_if, std::ostream& _warn_log)
	: Super()
	, warn_log(_warn_log)
	, disasm_if(_disasm_if)
	, stmt_lookup_if(_stmt_lookup_if)
	, symbol_table_lookup_if(_symbol_table_lookup_if)
	, sample(stat)
{
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		addr_profile_printers[f_fmt] = new AddressProfilePrinter(*this, f_fmt);
	}
}

template <typename ADDRESS, typename T>
AddressProfile<ADDRESS, T>::~AddressProfile()
{
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		delete addr_profile_printers[f_fmt];
	}
}

template <typename ADDRESS, typename T>
void AddressProfile<ADDRESS, T>::Capture(ADDRESS addr, unsigned int length)
{
	static T zero = T();
	
	sample.Capture();
	
	const T& value = sample.Get();
	
	if(value != zero)
	{
		Super::Cover(addr, length);
		Super::Accumulate(addr, sample.Get());
	}
}

template <typename ADDRESS, typename T>
void AddressProfile<ADDRESS, T>::Print(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const
{
	OStreamContext osc(os);
	
	ReportFormat r_fmt = visitor.GetReportFormat();

	switch(f_fmt)
	{
		case F_FMT_TEXT:
			os << "Instruction profile report - By address [" << GetSampledVariableName() << "]" << std::endl;
			os << (*(Super *) this) << std::endl;
			break;
			
		case F_FMT_CSV:
		{
			const std::string& csv_delimiter = visitor.GetCSVDelimiter();
			
			os << "Address" << csv_delimiter << "Value" << std::endl;
			
			std::map<ADDRESS, T> value_per_addr_map = *(Super *) this;
			typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
			for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
			{
				ADDRESS addr = (*value_per_addr_map_it).first;
				const T& value = (*value_per_addr_map_it).second;
				
				os << "0x" << std::hex << addr << csv_delimiter << std::dec << value << std::endl;
			}
			break;
		}
			
		case F_FMT_HTML:
		{
			std::map<ADDRESS, T> value_per_addr_map = *(Super *) this;
			typename std::map<ADDRESS, T>::size_type num_pc_locs = value_per_addr_map.size();
			
			bool is_first_value = true;
			
			std::pair<T, T> value_range;
			
			typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
			for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
			{
				const T& value = (*value_per_addr_map_it).second;
				
				if(is_first_value)
				{
					value_range.first = value;
					value_range.second = value;
					is_first_value = false;
				}
				else
				{
					if(value < value_range.first) value_range.first = value;
					if(value > value_range.second) value_range.second = value;
				}
			}
			
			Quantizer<T> quantizer = Quantizer<T>(16, value_range);
			Indent indent;
			
			os << indent << "<!DOCTYPE HTML>" << std::endl;
			os << indent << "<html lang=\"en\">" << std::endl;
			os << ++indent << "<head>" << std::endl;
			os << ++indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << indent << "<meta name=\"description\" content=\"Instruction profile by address for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "\">" << std::endl;
			os << indent << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << indent << "<title>Instruction profile by address for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</title>" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_style.css")) << "\" type=\"text/css\" />" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/by_address.css")) << "\" type=\"text/css\" />" << std::endl;
			const std::string& domain = visitor.GetDomain();
			if(!domain.empty())
			{
				os << indent<< "<script>document.domain='" << visitor.GetDomain() << "';</script>" << std::endl;
			}
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_script.js")) << "\"></script>" << std::endl;
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/script.js")) << "\"></script>" << std::endl;
			os << --indent << "</head>" << std::endl;
			os << indent << "<body";
			if(r_fmt == R_FMT_HTTP)
			{
				os << " onload=\"gui.auto_reload(" << (unsigned int)(visitor.GetRefreshPeriod() * 1000) << ", 'self-refresh-when-active')\"";
			}
			os << ">" << std::endl;
			os << ++indent << "<div id=\"content\">" << std::endl;
			std::string href_csv(std::string("by_address") + FileFormatSuffix(F_FMT_CSV));
			os << ++indent << "<div id=\"index\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Go back to index\" draggable=\"false\" ondragstart=\"return false\" href=\"../index.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_index.svg")) << "\" alt=\"Index\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<div id=\"save\" class=\"button\" draggable=\"false\">" << std::endl;
			os << ++indent << "<a title=\"Save as .CSV spreadsheet file\" ondragstart=\"return false\" href=\"" << href_csv << "\" download=\"" << href_csv << "\" target=\"_blank\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_csv_table.svg")) << "\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<h1 id=\"title\">Instruction profile by address<br>for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</h1>" << std::endl;
			os << indent << "<table id=\"addresses\">" << std::endl;
			os << ++indent << "<tr>" << std::endl;
			os << ++indent << "<th>Program Address</th>" << std::endl;
			os << indent << "<th>" << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</th>" << std::endl;
			os << --indent << "</tr>" << std::endl;
			
			for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
			{
				ADDRESS addr = (*value_per_addr_map_it).first;
				const T& value = (*value_per_addr_map_it).second;
				
				if(value >= value_range.first)
				{
					unsigned int scale = quantizer.Quantize(value);
					//std::cerr << value << " -> " << scale << std::endl;
					
					os << indent << "<tr class=\"scale" << scale << "\">";
				}
				else
				{
					os << indent << "<tr>";
				}
				
				os << "<td>";
				os << "0x";

				os.fill('0');
				os.width(2 * sizeof(ADDRESS));
				os << std::hex << addr;
				os << "</td>";
				
				std::stringstream sstr;
				sstr << value;
				
				os << "<td>" << unisim::util::hypapp::HTML_Encoder::Encode(sstr.str().c_str()) << "</td></tr>" << std::endl;
				
			}
			os << indent << "<tr><td>" << std::dec << num_pc_locs << " PC locations</td><td>" << GetCumulativeValueAsString() << "</td></tr>" << std::endl;
			
			os << --indent << "</table>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << --indent << "</body>" << std::endl;
			os << --indent << "</html>" << std::endl;

			break;
		}
		
		default:
			break;
	}
}

template <typename ADDRESS, typename T>
InstructionProfileBase *AddressProfile<ADDRESS, T>::CreateInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index) const
{
	return new InstructionProfile<ADDRESS, T>(this, func_name_loc_conv, filename_index, disasm_if);
}

template <typename ADDRESS, typename T>
FunctionInstructionProfileBase *AddressProfile<ADDRESS, T>::CreateFunctionInstructionProfile(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index) const
{
	return new FunctionInstructionProfile<ADDRESS, T>(this, func_name_loc_conv, filename_index, symbol_table_lookup_if);
}

template <typename ADDRESS, typename T>
SourceCodeProfileBase<ADDRESS> *AddressProfile<ADDRESS, T>::CreateSourceCodeProfile() const
{
	return new SourceCodeProfile<ADDRESS, T>(this, stmt_lookup_if, symbol_table_lookup_if, warn_log);
}

template <typename ADDRESS, typename T>
bool AddressProfile<ADDRESS, T>::GetValue(ADDRESS addr, T& value) const
{
	return this->GetWeight(addr, value);
}

template <typename ADDRESS, typename T>
std::pair<T, T> AddressProfile<ADDRESS, T>::GetValueRange() const
{
	std::pair<T, T> value_range;
	std::map<ADDRESS, T> value_per_addr_map = *(Super *) this;

	typename std::map<ADDRESS, T>::const_iterator value_per_addr_map_it = value_per_addr_map.begin();
	bool is_first_value = true;

	for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
	{
		const T& value = (*value_per_addr_map_it).second;
		
		if(is_first_value)
		{
			value_range.first = value;
			value_range.second = value;
			is_first_value = false;
		}
		else
		{
			if(value < value_range.first) value_range.first = value;
			if(value > value_range.second) value_range.second = value;
		}
	}
	
	return value_range;
}

template <typename ADDRESS, typename T>
const Printer& AddressProfile<ADDRESS, T>::GetPrinter(FileFormat f_fmt) const
{
	return *addr_profile_printers[f_fmt];
}

template <typename ADDRESS, typename T>
AddressProfile<ADDRESS, T>::AddressProfilePrinter::AddressProfilePrinter(AddressProfile<ADDRESS, T>& _addr_profile, FileFormat _f_fmt)
	: addr_profile(_addr_profile)
	, f_fmt(_f_fmt)
{
}
	
template <typename ADDRESS, typename T>
FileFormat AddressProfile<ADDRESS, T>::AddressProfilePrinter::GetFileFormat() const
{
	return f_fmt;
}
	
template <typename ADDRESS, typename T>
void AddressProfile<ADDRESS, T>::AddressProfilePrinter::Print(std::ostream& os, Visitor& visitor) const
{
	addr_profile.Print(os, visitor, f_fmt);
}

//////////////////////////// InstructionProfile<> /////////////////////////////

template <typename ADDRESS, typename T>
InstructionProfile<ADDRESS, T>::InstructionProfile(const AddressProfile<ADDRESS, T> *_addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, FilenameIndex *_filename_index, unisim::service::interfaces::Disassembly<ADDRESS> *_disasm_if)
	: addr_profile(_addr_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, filename_index(_filename_index)
	, disasm_if(_disasm_if)
{
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		instruction_profile_printers[f_fmt] = new InstructionProfilePrinter(*this, f_fmt);
	}
}

template <typename ADDRESS, typename T>
InstructionProfile<ADDRESS, T>::~InstructionProfile()
{
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		delete instruction_profile_printers[f_fmt];
	}
}

template <typename ADDRESS, typename T>
void InstructionProfile<ADDRESS, T>::Print(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const
{
	OStreamContext osc(os);

	ReportFormat r_fmt = visitor.GetReportFormat();

	std::vector<std::pair<ADDRESS, ADDRESS> > addr_ranges;
	std::pair<T, T> value_range = addr_profile->GetValueRange();
	addr_profile->GetAddressRanges(addr_ranges);
	
	ADDRESS addr;
	ADDRESS next_addr;

	switch(f_fmt)
	{
		case F_FMT_TEXT:
		{
			os << "Instruction profile report - Annotated disassembly [" << GetSampledVariableName() << "]" << std::endl;
			typename std::vector<std::pair<ADDRESS, ADDRESS> >::const_iterator addr_range_it;
			
			for(addr_range_it = addr_ranges.begin(); addr_range_it < addr_ranges.end(); addr_range_it++)
			{
				os << "..." << std::endl;
				
				const std::pair<ADDRESS, ADDRESS>& addr_range = *addr_range_it;
				
				for(addr = addr_range.first; addr <= addr_range.second; addr = next_addr)
				{
					T value = T();
					
					if(!addr_profile->GetValue(addr, value)) throw std::runtime_error("Internal Error! can't get value");
					
					{
						OStreamContext osc(os);
						os.fill(' ');
						os.flags(std::ios_base::right);
						
						os << "0x";
						os.width(2 * sizeof(ADDRESS));
						os << std::hex << addr << "  ";
						
						os.width(21);
					}
					
					if(value != T())
					{
						os << std::dec << value;
					}
					else
					{
						os << ' ';
					}
					
					os << "  " << disasm_if->Disasm(addr, next_addr) << std::endl;
				}
				os << "..." << std::endl;
			}
			break;
		}
		
		case F_FMT_CSV:
		{
			const std::string& csv_delimiter = visitor.GetCSVDelimiter();
			
			os << "Address" << csv_delimiter << "Value" << csv_delimiter << "Assembly" << std::endl;
			typename std::vector<std::pair<ADDRESS, ADDRESS> >::const_iterator addr_range_it;
			
			for(addr_range_it = addr_ranges.begin(); addr_range_it < addr_ranges.end(); addr_range_it++)
			{
				const std::pair<ADDRESS, ADDRESS>& addr_range = *addr_range_it;
				
				for(addr = addr_range.first; addr <= addr_range.second; addr = next_addr)
				{
					T value = T();
					
					if(!addr_profile->GetValue(addr, value)) throw std::runtime_error("Internal Error! can't get value");
					
					{
						OStreamContext osc(os);
						os << "0x";
						os.fill('0');
						os.width(2 * sizeof(ADDRESS));
						os << std::hex << addr << std::dec << "  " << csv_delimiter;
					}

					if(value != T())
					{
						os << value;
					}
					
					os << csv_delimiter << c_string_to_CSV(disasm_if->Disasm(addr, next_addr).c_str()) << std::endl;
				}
			}
			break;
		}
		
		case F_FMT_HTML:
		{
			Quantizer<T> quantizer = Quantizer<T>(16, value_range);
			//std::cerr << "\"" << filename << "\": value_range=[" << value_range.first << "," << value_range.second << "]" << std::endl;
			Indent indent;
			
			os << indent << "<!DOCTYPE HTML>" << std::endl;
			os << indent << "<html lang=\"en\">" << std::endl;
			os << ++indent << "<head>" << std::endl;
			os << ++indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << indent << "<meta name=\"description\" content=\"Instruction profile with annotated disassembly for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "\">" << std::endl;
			os << indent << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << indent << "<title>Instruction profile for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</title>" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_style.css")) << "\" type=\"text/css\" />" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/disassembly.css")) << "\" type=\"text/css\" />" << std::endl;
			const std::string& domain = visitor.GetDomain();
			if(!domain.empty())
			{
				os << indent << "<script>document.domain='" << visitor.GetDomain() << "';</script>" << std::endl;
			}
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_script.js")) << "\"></script>" << std::endl;
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/script.js")) << "\"></script>" << std::endl;
			os << --indent << "</head>" << std::endl;
			os << indent << "<body";
			if(r_fmt == R_FMT_HTTP)
			{
				os << " onload=\"gui.auto_reload(" << (unsigned int)(visitor.GetRefreshPeriod() * 1000) << ", 'self-refresh-when-active')\"";
			}
			os << ">" << std::endl;
			os << ++indent << "<div id=\"content\">" << std::endl;
			os << ++indent << "<div id=\"show-table\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Go back to function profile table\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_table.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_to_table.svg")) << "\" alt=\"→Table\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<div id=\"show-histogram\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Go back to function profile chart\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_histogram.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_to_histogram.svg")) << "\" alt=\"→Histogram\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			std::string href_csv(std::string("disassembly") + FileFormatSuffix(F_FMT_CSV));
			os << indent << "<div id=\"save\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Save as .CSV spreadsheet file\" draggable=\"false\" ondragstart=\"return false\" href=\"" << href_csv << "\" download=\"" << href_csv << "\" target=\"_blank\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_csv_table.svg")) << "\" alt=\".CSV↓\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<h1 id=\"title\">Instruction profile with annotated disassembly<br>for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</h1>" << std::endl;
			
			os << indent << "<table id=\"disassembly\">" << std::endl;
			++indent;
			
			uint64_t num_asm_locs = 0;
			typename std::vector<std::pair<ADDRESS, ADDRESS> >::const_iterator addr_range_it;
			
			for(addr_range_it = addr_ranges.begin(); addr_range_it < addr_ranges.end(); addr_range_it++)
			{
				os << indent << "<tr><td colspan=\"3\">&hellip;</td></tr>" << std::endl;
				
				const std::pair<ADDRESS, ADDRESS>& addr_range = *addr_range_it;
				
				bool first_addr_in_addr_range = true;
				
				for(addr = addr_range.first; addr <= addr_range.second; addr = next_addr, first_addr_in_addr_range = false)
				{
					T value = T();
					
					if(!addr_profile->GetValue(addr, value)) throw std::runtime_error("Internal Error! can't get value");
					
					std::stringstream sstr;
					sstr << value;
					
					bool is_entry_point = false;
					const char *func_name = func_name_loc_conv->AddressToFunctionName(addr, is_entry_point);
					
					if(func_name && is_entry_point)
					{
						if(!first_addr_in_addr_range)
						{
							os << indent << "<tr><td colspan=\"3\">&nbsp;</td></tr>" << std::endl;
						}
						os << indent << "<tr><td>0x" << std::hex << addr << std::dec << "</td><td colspan=\"2\">&lt;";
						const SLoc *sloc = func_name_loc_conv->FunctionNameToSLoc(func_name);
						if(sloc)
						{
							os << "<a class=\"function\" title=\"Go to source code\" href=\"source" << filename_index->IndexFilename(sloc->GetFilename()) << ".html#" << unisim::util::hypapp::URI_Encoder::Encode(func_name) << "\">";
						}
						
						os << unisim::util::hypapp::HTML_Encoder::Encode(func_name);
						
						if(sloc)
						{
							os << "</a>";
						}
						
						os << "&gt;:</td></tr>" << std::endl;
						
						
					}

					if(value >= value_range.first)
					{
						unsigned int scale = quantizer.Quantize(value);
						
						os << indent << "<tr class=\"scale" << scale << "\"";
					}
					else
					{
						os << indent << "<tr";
					}
					
					if(func_name && is_entry_point)
					{
						os << " id=\"" << unisim::util::hypapp::URI_Encoder::Encode(func_name) <<"\"";
					}
					
					os << ">";

					if(func_name)
					{
						os << "<!--in " << unisim::util::hypapp::HTML_Encoder::Encode(func_name) << " -->";
					}
					
					os << "<td class=\"address\">0x" << std::hex << addr << std::dec << "</td><td class=\"value\">" << ((value != T()) ? unisim::util::hypapp::HTML_Encoder::Encode(sstr.str().c_str()) : "") << "</td><td class=\"source-code\">" << unisim::util::hypapp::HTML_Encoder::Encode(disasm_if->Disasm(addr, next_addr).c_str()) << "</td></tr>" << std::endl;
					
					num_asm_locs++;
				}
				
				os << indent << "<tr><td colspan=\"3\">&hellip;</td></tr>" << std::endl;
			}
			os << indent << "<tr><td>Total<td>" << addr_profile->GetCumulativeValueAsString() << "</td><td>" << num_asm_locs << " assembly SLOCs</td></tr>" << std::endl;
			os << --indent << "</table>" << std::endl;
			
			os << --indent << "</div>" << std::endl;
			os << --indent << "</body>" << std::endl;
			os << --indent << "</html>" << std::endl;
			break;
		}
		
		default:
			break;
	}
}

template <typename ADDRESS, typename T>
const Printer& InstructionProfile<ADDRESS, T>::GetPrinter(FileFormat f_fmt) const
{
	return *instruction_profile_printers[f_fmt];
}

template <typename ADDRESS, typename T>
InstructionProfile<ADDRESS, T>::InstructionProfilePrinter::InstructionProfilePrinter(InstructionProfile<ADDRESS, T>& _instruction_profile, FileFormat _f_fmt)
	: instruction_profile(_instruction_profile)
	, f_fmt(_f_fmt)
{
}

template <typename ADDRESS, typename T>
FileFormat InstructionProfile<ADDRESS, T>::InstructionProfilePrinter::GetFileFormat() const
{
	return f_fmt;
}

template <typename ADDRESS, typename T>
void InstructionProfile<ADDRESS, T>::InstructionProfilePrinter::Print(std::ostream& os, Visitor& visitor) const
{
	instruction_profile.Print(os, visitor, f_fmt);
}


/////////////////////// FunctionInstructionProfile<> //////////////////////////

template <typename ADDRESS, typename T>
FunctionInstructionProfile<ADDRESS, T>::FunctionInstructionProfile(const AddressProfile<ADDRESS, T> *_addr_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, FilenameIndex *_filename_index, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *_symbol_table_lookup_if)
	: addr_profile(_addr_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, filename_index(_filename_index)
	, symbol_table_lookup_if(_symbol_table_lookup_if)
	, func_insn_profile()
	, value_range()
	, cumulative_value()
	, table_printers()
	, histogram_printers()
{
	Init();
}

template <typename ADDRESS, typename T>
FunctionInstructionProfile<ADDRESS, T>::~FunctionInstructionProfile()
{
	Clear();
}

template <typename ADDRESS, typename T>
const Printer& FunctionInstructionProfile<ADDRESS, T>::GetTablePrinter(FileFormat f_fmt) const
{
	return *table_printers[f_fmt];
}

template <typename ADDRESS, typename T>
const Printer& FunctionInstructionProfile<ADDRESS, T>::GetHistogramPrinter(FileFormat f_fmt) const
{
	return *histogram_printers[f_fmt];
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::PrintTable(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const
{
	const std::string& csv_delimiter = visitor.GetCSVDelimiter();
	const std::string& csv_hyperlink = visitor.GetCSVHyperlink();
	const std::string& csv_arg_separator = visitor.GetCSVArgSeparator();
	ReportFormat r_fmt = visitor.GetReportFormat();
	Indent indent;
	std::multimap<T, std::string> sorted_func_insn_profile;
	RatioCalculator<T, 100> percent_calc(cumulative_value);
	RatioCalculator<T> ratio_calc(cumulative_value);
	
	typename std::map<std::string, T>::size_type num_functions = func_insn_profile.size();
	std::size_t func_name_max_length = 0;
	std::size_t value_max_length = 0;
	for(typename std::map<std::string, T>::const_iterator func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
	{
		const std::string& func_name = (*func_prof_it).first;
		const T& value = (*func_prof_it).second;
		if(func_name.length() > func_name_max_length)
		{
			func_name_max_length = func_name.length();
		}
		std::string value_str(to_string(value));
		if(value_str.length() > value_max_length)
		{
			value_max_length = value_str.length();
		}
		
		sorted_func_insn_profile.insert(std::pair<T, std::string>(value, func_name));
	}
	
	switch(f_fmt)
	{
		case F_FMT_TEXT:
			os << "Instruction profile report - By function [" << GetSampledVariableName() << "]" << std::endl;
			break;
			
		case F_FMT_CSV:
			os << "Function" << csv_delimiter << "Ratio" << csv_delimiter << "Value" << csv_delimiter << "Disassembly" << csv_delimiter << "Source code" << std::endl;
			break;
			
		case F_FMT_HTML:
		{
			os << indent << "<!DOCTYPE HTML>" << std::endl;
			os << indent << "<html lang=\"en\">" << std::endl;
			os << ++indent << "<head>" << std::endl;
			os << ++indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << indent << "<meta name=\"description\" content=\"Instruction profile by function for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "\">" << std::endl;
			os << indent << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << indent << "<title>Instruction profile by function for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</title>" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_style.css")) << "\" type=\"text/css\" />" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/by_function_table.css")) << "\" type=\"text/css\" />" << std::endl;
			const std::string& domain = visitor.GetDomain();
			if(!domain.empty())
			{
				os << indent << "<script>document.domain='" << visitor.GetDomain() << "';</script>" << std::endl;
			}
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_script.js")) << "\"></script>" << std::endl;
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/by_function_table.js")) << "\"></script>" << std::endl;
			os << --indent << "</head>" << std::endl;
			os << indent << "<body";
			if(r_fmt == R_FMT_HTTP)
			{
				os << " onload=\"gui.auto_reload(" << (unsigned int)(visitor.GetRefreshPeriod() * 1000) << ", 'self-refresh-when-active')\"";
			}
			os << ">" << std::endl;
			os << ++indent << "<div id=\"content\">" << std::endl;
			os << ++indent << "<div id=\"index\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Go back to index\" draggable=\"false\" ondragstart=\"return false\" href=\"../index.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_index.svg")) << "\" alt=\"→Index\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<div id=\"show-histogram\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Show function profile chart\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_histogram.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_histogram.svg")) << "\" alt=\"→Histogram\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<div id=\"save-histogram\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Save as .SVG image file\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_histogram.svg\" download=\"by_function_histogram.svg\" target=\"_blank\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_svg_histogram.svg")) << "\" alt=\".SVG↓\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<div id=\"save-table\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Save as .CSV spreadsheet file\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_table.csv\" download=\"by_function_table.csv\" target=\"_blank\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_csv_table.svg")) << "\" alt=\".CSV↓\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<h1 id=\"title\">Instruction profile by function<br>for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</h1>" << std::endl;
			os << indent << "<table id=\"summary\">" << std::endl;
			os << ++indent << "<tr>" << std::endl;
			os << ++indent << "<th>Function</th>" << std::endl;
			//os << "<th>" << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</th>" << std::endl;
			os << indent << "<th>Ratio</th>" << std::endl;
			os << indent << "<th>Value</th>" << std::endl;
			os << indent << "<th>Annotated<br>disassembly</th>" << std::endl;
			os << indent << "<th>Annotated<br>source code</th>" << std::endl;
			os << --indent << "</tr>" << std::endl;
			break;
		}
		default:
			break;
	}
	
	for(typename std::multimap<T, std::string>::const_reverse_iterator func_prof_it = sorted_func_insn_profile.rbegin(); func_prof_it != sorted_func_insn_profile.rend(); func_prof_it++)
	{
		const T& value = (*func_prof_it).first;
		const std::string& func_name = (*func_prof_it).second;
		const SLoc *sloc = func_name_loc_conv->FunctionNameToSLoc(func_name.c_str());
		
		std::stringstream value_sstr;
		value_sstr << value;
		std::string value_str(value_sstr.str());
		
		std::stringstream percent_sstr;
		percent_sstr.setf(std::ios::fixed);
		percent_sstr.precision(3);
		double percent = percent_calc.Compute(value);
		percent_sstr << percent;
		std::string percent_str(percent_sstr.str());
		
		std::stringstream ratio_sstr;
		ratio_sstr.setf(std::ios::fixed);
		ratio_sstr.precision(16);
		double ratio = ratio_calc.Compute(value);
		ratio_sstr << ratio;
		std::string ratio_str(ratio_sstr.str());
		
		std::stringstream func_addr_sstr;
		ADDRESS func_addr = 0;
		if(func_name_loc_conv->FunctionNameToAddress(func_name.c_str(), func_addr))
		{
			func_addr_sstr << "@0x" << std::hex << func_addr;
		}
		else
		{
			func_addr_sstr << "?";
		}
		std::string func_addr_str(func_addr_sstr.str());
		
		switch(f_fmt)
		{
			case F_FMT_TEXT:
				os << StringPadder(func_name, func_name_max_length);
				os << ':' << percent_str << "%";
				os << ':' << func_addr_str;
				os << ':';
				if(sloc)
				{
					os << sloc->GetFilename();
				}
				os << ':' << value_str << std::endl;
				break;
				
			case F_FMT_CSV:
			{
				os << c_string_to_CSV(func_name.c_str()) << csv_delimiter << ratio_str << csv_delimiter << value_str << csv_delimiter << "\"=" << csv_hyperlink << "(" << "\"\"" << ((r_fmt = R_FMT_HTTP) ? "http://" : "file://");
				const std::string& dir_path = visitor.GetDirPath();
				std::string href(dir_path);
				href += '/';
				href += "disassembly.csv";
				
				os << unisim::util::hypapp::URI_Encoder::Encode(href) << "" << "\"\"" << csv_arg_separator << "\"\"" << func_addr_str << "\"\")\"" << csv_delimiter;
				if(sloc)
				{
					os << "\"=" << csv_hyperlink << "(\"\"" << ((r_fmt = R_FMT_HTTP) ? "http://" : "file://");
					std::string href(dir_path);
					href += '/';
					href += "source";
					href += to_string(filename_index->IndexFilename(sloc->GetFilename()));
					href += ".csv";
					
					os << unisim::util::hypapp::URI_Encoder::Encode(href) << "\"\"" << csv_arg_separator << "\"\"" << sloc->GetFilename() << "\"\")\"";
				}
				os << std::endl;
				break;
			}
				
			case F_FMT_HTML:
			{

				os << indent << "<tr>";
				
				// Function
				os << "<td class=\"function\">" << unisim::util::hypapp::HTML_Encoder::Encode(func_name.c_str()) << "</td>";
				
				// Ratio
				os << "<td class=\"ratio\">" << unisim::util::hypapp::HTML_Encoder::Encode(percent_str.c_str()) << "&#37;</td>";
			
				// Value
				os << "<td class=\"value\">" << unisim::util::hypapp::HTML_Encoder::Encode(value_str.c_str()) << "</td>";

				// Annotated disassembly
				os << "<td class=\"disasm\"><a title=\"Show annotated disassembly\" href=\"disassembly.html#" << unisim::util::hypapp::URI_Encoder::Encode(func_name.c_str()) << "\">" << unisim::util::hypapp::HTML_Encoder::Encode(func_addr_str.c_str()) << "</td>";
				
				// Annotated souce code
				os << "<td class=\"source\">";
				
				if(sloc)
				{
					os << "<a title=\"Show annotated source code\" href=\"source" << filename_index->IndexFilename(sloc->GetFilename()) << ".html#" << unisim::util::hypapp::URI_Encoder::Encode(func_name.c_str()) << "\">" << unisim::util::hypapp::HTML_Encoder::Encode(sloc->GetFilename()) << "</a>";
				}
				os << "</td>";

				os << "</tr>" << std::endl;
				break;
			}
			
			default:
				break;
		}
	}
	
	switch(f_fmt)
	{
		case F_FMT_HTML:
			os << indent << "<tr><td>" << num_functions << " functions</td><td>100.00 &#37;</td><td>" << cumulative_value << "</td><td></td><td></td></tr>" << std::endl;
			os << --indent << "</table>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << --indent << "</body>" << std::endl;
			os << --indent << "</html>" << std::endl;
			break;
			
		default:
			break;
	}
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::PrintHistogram(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const
{
	if((f_fmt != F_FMT_HTML) && (f_fmt != F_FMT_SVG)) return;
	
	ReportFormat r_fmt = visitor.GetReportFormat();

	Indent indent;
	
	if(f_fmt == F_FMT_HTML)
	{
		os << indent << "<!DOCTYPE HTML>" << std::endl;
		os << indent << "<html lang=\"en\">" << std::endl;
		os << ++indent << "<head>" << std::endl;
		os << ++indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		os << indent << "<meta name=\"description\" content=\"Instruction profile by function for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "\">" << std::endl;
		os << indent << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
		os << indent << "<title>Instruction profile by function for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</title>" << std::endl;
		os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_style.css")) << "\" type=\"text/css\" />" << std::endl;
		os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/by_function_histogram.css")) << "\" type=\"text/css\" />" << std::endl;
		const std::string& domain = visitor.GetDomain();
		if(!domain.empty())
		{
			os << indent << "<script>document.domain='" << visitor.GetDomain() << "';</script>" << std::endl;
		}
		os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_script.js")) << "\"></script>" << std::endl;
		os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/by_function_histogram.js")) << "\"></script>" << std::endl;
		os << --indent << "</head>" << std::endl;
		os << indent << "<body";
		if(r_fmt == R_FMT_HTTP)
		{
			os << " onload=\"gui.auto_reload(" << (unsigned int)(visitor.GetRefreshPeriod() * 1000) << ", 'self-refresh-when-active')\"";
		}
		os << ">" << std::endl;
		os << ++indent << "<div id=\"content\">" << std::endl;
		os << ++indent << "<div id=\"index\" class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Go back to index\" draggable=\"false\" ondragstart=\"return false\" href=\"../index.html\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_index.svg")) << "\" alt=\"→Index\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << indent << "<div id=\"show-table\" class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Show function profile table\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_table.html\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_table.svg")) << "\" alt=\"→Table\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << indent << "<div id=\"save-histogram\" class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Save as .SVG image file\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_histogram.svg\" download=\"by_function_histogram.svg\" target=\"_blank\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_svg_histogram.svg")) << "\" alt=\".SVG↓\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << indent << "<div id=\"save-table\" class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Save as .CSV spreadsheet file\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_table.csv\" download=\"by_function_table.csv\" target=\"_blank\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_csv_table.svg")) << "\" alt=\".CSV↓\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << indent << "<h1 id=\"title\">Instruction profile by function<br>for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</h1>" << std::endl;
		os << indent << "<div id=\"histogram\">" << std::endl;
		++indent;
	}
	
	if(f_fmt == F_FMT_SVG)
	{
		os << indent << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	}
	
	//std::cerr << "FunctionInstructionProfile: value_range=[" << value_range.first << "," << value_range.second << "]" << std::endl;
	std::multimap<T, std::string> sorted_func_insn_profile;
	
	std::size_t func_name_max_length = 0;
	std::size_t value_max_length = 0;
	for(typename std::map<std::string, T>::const_iterator func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
	{
		const std::string& func_name = (*func_prof_it).first;
		const T& value = (*func_prof_it).second;
		if(func_name.length() > func_name_max_length)
		{
			func_name_max_length = func_name.length();
		}
		std::string value_str(to_string(value));
		if(value_str.length() > value_max_length)
		{
			value_max_length = value_str.length();
		}
		
		sorted_func_insn_profile.insert(std::pair<T, std::string>(value, func_name));
	}
	
	double min_histogram_height = 256.0;
	double font_size_px = 12.0;
	double font_scale = font_size_px / 14.0;
	
	double svg_margin = 6;
	double svg_text_height = font_scale * 8.5 * func_name_max_length;
	double svg_value_height = font_scale * 8.5 * value_max_length;
	double svg_histogram_height = 0.5 * svg_text_height;
	if(svg_histogram_height < min_histogram_height) svg_histogram_height = min_histogram_height;
	double svg_height = svg_histogram_height + svg_value_height + svg_text_height + (2 * svg_margin);
	
	double svg_bar_width = 16 * font_scale;
	double svg_histogram_width = ((svg_bar_width * 1.25) * func_insn_profile.size());
	double svg_width = (2 * svg_margin) + (font_scale * 8.5 * 7) + svg_histogram_width;
	double axis_y = svg_margin + svg_histogram_height + svg_value_height;
	
	double svg_histogram_x = svg_margin + (font_scale * 8.5 * 7);
	double level_axis_x = svg_margin + (font_scale * 8.5 * 6);
	
	std::pair<T, T> histogram_value_range(T(), value_range.second); // from 0 to max value
	Scaler<T> histogram_scaler = Scaler<T>(svg_histogram_height, histogram_value_range);
	
	os << indent << "<svg version=\"1.2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"" << svg_width << "\" height=\"" << svg_height << "\" viewBox=\"0 0 " << svg_width << " " << svg_height << "\">" << std::endl;
	os << ++indent << "<defs>" << std::endl;
	os << ++indent << "<style type=\"text/css\">" << std::endl;
	os << ++indent << "<![CDATA[" << std::endl;
	os << ++indent << ".background { fill:#ffffff; }" << std::endl;
	os << indent << ".axis { stroke:#000000; stroke-width:1; }" << std::endl;
	os << indent << ".level-axis { stroke:#c0c0c0; stroke-width:1; stroke-dasharray:2,2; }" << std::endl;
	os << indent << ".level-axis-text { font-size:" << font_size_px << "px; font-family:'Courier New',Courier,monospace; font-weight:bold; fill:#000000; }" << std::endl;
	os << indent << ".func-name { font-size:" << font_size_px << "px; font-family:'Courier New',Courier,monospace; font-weight:bold; fill:#000000; }" << std::endl;
	os << indent << ".bar { fill:#ff0000; }" << std::endl;
	os << indent << ".value { font-size:" << font_size_px << "px; font-family:'Courier New',Courier,monospace; font-weight:bold; fill:#000000; }" << std::endl;
	os << --indent << "]]>" << std::endl;
	os << --indent << "</style>" << std::endl;
	os << --indent << "</defs>" << std::endl;
	os << indent << "<title>Instruction profile by function for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</title>" << std::endl;
	os << indent << "<rect class=\"background\" x=\"0\" y=\"0\" width=\"" << svg_width << "\" height=\"" << svg_height << "\"/>" << std::endl;
	
	if(value_range.second != T())
	{
		double ratio = Divider<T>::Divide(cumulative_value, value_range.second);
		unsigned int ideal_num_levels = (ratio * svg_histogram_height) / (8.5 * font_scale * 3);
		unsigned int num_levels = (ideal_num_levels >= 100) ? 100 : ((ideal_num_levels >= 50) ? 50 : ((ideal_num_levels >= 40) ? 40 : ((ideal_num_levels >= 20) ? 20 : ((ideal_num_levels >= 10) ? 10 : 5))));
		unsigned int level;
		for(level = 1; level <= num_levels; level++)
		{
			T level_value((cumulative_value * level) / num_levels);
			if(level_value > value_range.second) break;
			
			double level_height = Divider<T>::Divide(level * cumulative_value, num_levels * value_range.second) * svg_histogram_height;
			double level_axis_y = axis_y - level_height;
			os << indent << "<line class=\"level-axis\" x1=\"" << level_axis_x << "\" y1=\"" << level_axis_y << "\" x2=\"" << svg_width << "\" y2=\"" << level_axis_y << "\"/>" << std::endl;
			
			double level_text_x = svg_margin + (font_scale * 8.5 * 5);
			double level_text_y = level_axis_y + (8.5 * font_scale / 2.0);
			unsigned int int_part = (level * 100) / num_levels;
			unsigned int frac_part = (level * 100) % num_levels;
			os << indent << "<text class=\"level-axis-text\" text-anchor=\"end\" x=\"" << level_text_x << "\" y=\"" << level_text_y << "\">" << int_part << '.' << ((10 * frac_part) / num_levels) << "&#37;</text>" << std::endl;
		}
	}
	
	unsigned int histogram_column = 0;
	
	for(typename std::multimap<T, std::string>::const_reverse_iterator func_prof_it = sorted_func_insn_profile.rbegin(); func_prof_it != sorted_func_insn_profile.rend(); func_prof_it++)
	{
		const T& value = (*func_prof_it).first;
		const std::string& func_name = (*func_prof_it).second;
		
		const SLoc *sloc = func_name_loc_conv->FunctionNameToSLoc(func_name.c_str());

		if(value >= value_range.first)
		{
			//std::cerr << value << " -> " << scale << std::endl;
			
			if(sloc && ((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP)))
			{
				os << indent << "<a title=\"Go to source code\" draggable=\"false\" ondragstart=\"return false\" href=\"source" << filename_index->IndexFilename(sloc->GetFilename()) << ".html#" << unisim::util::hypapp::URI_Encoder::Encode(func_name.c_str()) << "\">" << std::endl;
				++indent;
			}
			
			// draw bar
			double svg_bar_height = histogram_scaler.Scale(value);
			double svg_bar_x = svg_histogram_x + ((svg_bar_width * 1.25) * histogram_column);
			double svg_bar_y = (axis_y - svg_bar_height);
			os << indent << "<rect class=\"bar\" x=\"" << svg_bar_x << "\" y=\"" << svg_bar_y << "\" width=\"" << svg_bar_width << "\" height=\"" << svg_bar_height << "\"/>" << std::endl;
			
			// draw value
			double svg_value_x = svg_bar_x - (font_scale * 4.2);
			double svg_value_y = svg_bar_y - (font_scale * 20.0);
			os << indent << "<text class=\"value\" text-anchor=\"start\" transform=\"rotate(-90," << (svg_value_x + svg_bar_width) << "," << svg_value_y << ")\" x=\"" << svg_value_x << "\" y=\"" << svg_value_y << "\">" << unisim::util::hypapp::HTML_Encoder::Encode(to_string(value)) << "</text>" << std::endl;
			
			// draw function name
			double svg_text_x = svg_bar_x - (font_scale * 4.2);
			double svg_text_y = axis_y - (font_scale * 12.0);
			
			os << indent << "<text class=\"func-name\" text-anchor=\"end\" transform=\"rotate(-90," << (svg_text_x + svg_bar_width) << "," << svg_text_y << ")\" x=\"" << svg_text_x << "\" y=\"" << svg_text_y << "\">" << unisim::util::hypapp::HTML_Encoder::Encode(func_name.c_str()) << "</text>" << std::endl;
			
			if(sloc && ((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP)))
			{
				os << --indent << "</a>" << std::endl;
			}
			
			histogram_column++;
		}
	}
	
	// draw horizontal axis
	os << indent << "<line class=\"axis\" x1=\"0\" y1=\"" << axis_y << "\" x2=\"" << svg_width << "\" y2=\"" << axis_y << "\"/>" << std::endl;
	os << --indent << "</svg>" << std::endl;
	
	if(f_fmt == F_FMT_HTML)
	{
		os << --indent << "</div>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << --indent << "</body>" << std::endl;
		os << --indent << "</html>" << std::endl;
	}
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::Init()
{
	std::map<ADDRESS, T> value_per_addr_map = *addr_profile;
	
	typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
	
	for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
	{
		ADDRESS addr = (*value_per_addr_map_it).first;
		const T& value = (*value_per_addr_map_it).second;
		
		const unisim::util::debug::Symbol<ADDRESS> *func_symbol = symbol_table_lookup_if->FindSymbolByAddr(addr, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
		
		if(func_symbol)
		{
			func_insn_profile[func_symbol->GetName()] += value;
			cumulative_value += value;
		}
	}
	
	bool is_first_value = true;
	
	typename std::map<std::string, T>::const_iterator func_prof_it;
	
	for(func_prof_it = func_insn_profile.begin(); func_prof_it != func_insn_profile.end(); func_prof_it++)
	{
		const T& value = (*func_prof_it).second;
		
		if(is_first_value)
		{
			value_range.first = value;
			value_range.second = value;
			is_first_value = false;
		}
		else
		{
			if(value < value_range.first) value_range.first = value;
			if(value > value_range.second) value_range.second = value;
		}
	}
	
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		table_printers[f_fmt] = new TablePrinter(*this, f_fmt);
		histogram_printers[f_fmt] = new HistogramPrinter(*this, f_fmt);
	}
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::Clear()
{
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		delete table_printers[f_fmt];
		delete histogram_printers[f_fmt];
	}
}

template <typename ADDRESS, typename T>
FunctionInstructionProfile<ADDRESS, T>::TablePrinter::TablePrinter(FunctionInstructionProfile<ADDRESS, T>& _func_insn_profile, FileFormat _f_fmt)
	: func_insn_profile(_func_insn_profile)
	, f_fmt(_f_fmt)
{
}

template <typename ADDRESS, typename T>
FileFormat FunctionInstructionProfile<ADDRESS, T>::TablePrinter::GetFileFormat() const
{
	return f_fmt;
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::TablePrinter::Print(std::ostream& os, Visitor& visitor) const
{
	func_insn_profile.PrintTable(os, visitor, f_fmt);
}

template <typename ADDRESS, typename T>
FunctionInstructionProfile<ADDRESS, T>::HistogramPrinter::HistogramPrinter(FunctionInstructionProfile<ADDRESS, T>& _func_insn_profile, FileFormat _f_fmt)
	: func_insn_profile(_func_insn_profile)
	, f_fmt(_f_fmt)
{
}

template <typename ADDRESS, typename T>
FileFormat FunctionInstructionProfile<ADDRESS, T>::HistogramPrinter::GetFileFormat() const
{
	return f_fmt;
}

template <typename ADDRESS, typename T>
void FunctionInstructionProfile<ADDRESS, T>::HistogramPrinter::Print(std::ostream& os, Visitor& visitor) const
{
	func_insn_profile.PrintHistogram(os, visitor, f_fmt);
}

//////////////////////////// SourceCodeProfile<> //////////////////////////////

template <typename ADDRESS, typename T>
SourceCodeProfile<ADDRESS, T>::SourceCodeProfile(const AddressProfile<ADDRESS, T> *_addr_profile, unisim::service::interfaces::StatementLookup<ADDRESS> *_stmt_lookup_if, unisim::service::interfaces::SymbolTableLookup<ADDRESS> *_symbol_table_lookup_if, std::ostream& _warn_log)
	: warn_log(_warn_log)
	, addr_profile(_addr_profile)
	, stmt_lookup_if(_stmt_lookup_if)
	, symbol_table_lookup_if(_symbol_table_lookup_if)
	, other_profile()
	, sloc_profile()
	, source_filename_set()
	, value_range()
{
}

template <typename ADDRESS, typename T>
void SourceCodeProfile<ADDRESS, T>::Print(std::ostream& os, Visitor& visitor) const
{
	OStreamContext osc(os);

	os << "-------- slocs ---------" << std::endl;
	
	typename std::map<SLoc, T>::const_iterator sloc_prof_it;
	
	for(sloc_prof_it = sloc_profile.begin(); sloc_prof_it != sloc_profile.end(); sloc_prof_it++)
	{
		const SLoc& sloc = (*sloc_prof_it).first;
		const T& value = (*sloc_prof_it).second;
		
		sloc.Print(os);
		os << ":" << value << std::endl;
	}
	os << "-------- other ---------" << std::endl;
	
	typename std::map<ADDRESS, T>::const_iterator other_prof_it;
	
	for(other_prof_it = other_profile.begin(); other_prof_it != other_profile.end(); other_prof_it++)
	{
		const ADDRESS& addr = (*other_prof_it).first;
		const T& value = (*other_prof_it).second;
		
		os << "0x" << std::hex << addr << std::dec << ":" << value << std::endl;
	}
	os << "-----------------------------" << std::endl;
}

template <typename ADDRESS, typename T>
bool SourceCodeProfile<ADDRESS, T>::FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *>& stmts, ADDRESS addr)
{
	const unisim::util::debug::Statement<ADDRESS> *stmt = stmt_lookup_if->FindStatements(stmts, addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT);
	
	if(stmt) return stmt;
	
	const unisim::util::debug::Symbol<ADDRESS> *func_symbol = symbol_table_lookup_if->FindSymbolByAddr(addr, unisim::util::debug::Symbol<ADDRESS>::SYM_FUNC);
	
	ADDRESS func_start_addr = addr;
	ADDRESS func_end_addr = addr;
	if(func_symbol)
	{
		ADDRESS func_addr = func_symbol->GetAddress();
		ADDRESS func_size = func_symbol->GetSize();
		if(func_size != 0)
		{
			func_start_addr = func_addr;
			func_end_addr = func_addr + func_size - 1;
		}
	}

	// could not find exact statemement, try to find nearest statements
	stmts.clear();
	stmt = stmt_lookup_if->FindStatements(stmts, addr, unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT);
	
	// filter statements outside from the function scope
	if(stmt && (stmt->GetAddress() >= func_start_addr) && (stmt->GetAddress() <= func_end_addr))
	{
		typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::iterator stmt_it = stmts.begin();
		
		do
		{
			stmt = *stmt_it;
			if((stmt->GetAddress() < func_start_addr) || (stmt->GetAddress() > func_end_addr))
			{
				stmt_it = stmts.erase(stmt_it);
			}
			else
			{
				stmt_it++;
			}
		}
		while(stmt_it != stmts.end());
		
		return !stmts.empty();
	}
	
	return false;
}

template <typename ADDRESS, typename T>
void SourceCodeProfile<ADDRESS, T>::Update()
{
	sloc_profile.clear();
	other_profile.clear();
	
	std::map<ADDRESS, T> value_per_addr_map = *addr_profile;
	
	typename std::map<ADDRESS, T>::iterator value_per_addr_map_it;
	
	for(value_per_addr_map_it = value_per_addr_map.begin(); value_per_addr_map_it != value_per_addr_map.end(); value_per_addr_map_it++)
	{
		ADDRESS addr = (*value_per_addr_map_it).first;
		const T& value = (*value_per_addr_map_it).second;
		
		std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
		if(FindStatements(stmts, addr))
		{
			typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::iterator stmt_it;
			for(stmt_it = stmts.begin(); stmt_it != stmts.end(); stmt_it++)
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = *stmt_it;
		
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					unsigned int lineno = stmt->GetLineNo();
					//unsigned int colno = stmt->GetColNo();
					std::string source_path;
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;
					
					SLoc sloc = SLoc(source_path.c_str(), lineno);
					
					sloc_profile[sloc] += value;
					
					source_filename_set.insert(source_path);
				}
			}
		}
		else
		{
			other_profile[addr] += value;
		}
	}
	
	bool is_first_value = true;

	typename std::map<SLoc, T>::const_iterator sloc_prof_it;
	
	for(sloc_prof_it = sloc_profile.begin(); sloc_prof_it != sloc_profile.end(); sloc_prof_it++)
	{
		const T& value = (*sloc_prof_it).second;
		
		if(is_first_value)
		{
			value_range.first = value;
			value_range.second = value;
			is_first_value = false;
		}
		else
		{
			if(value < value_range.first) value_range.first = value;
			if(value > value_range.second) value_range.second = value;
		}
	}
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFileSetBase *SourceCodeProfile<ADDRESS, T>::CreateAnnotatedSourceCodeFileSet(const FunctionNameLocationConversionBase<ADDRESS> *func_name_loc_conv, FilenameIndex *filename_index, const char *search_path) const
{
	return new AnnotatedSourceCodeFileSet<ADDRESS, T>(this, func_name_loc_conv, filename_index, search_path, warn_log);
}

template <typename ADDRESS, typename T>
const T& SourceCodeProfile<ADDRESS, T>::GetValue(const char *filename, unsigned int lineno) const
{
	SLoc sloc = SLoc(filename, lineno);
	
	typename std::map<SLoc, T>::const_iterator sloc_prof_it = sloc_profile.find(sloc);
	
	if(sloc_prof_it != sloc_profile.end())
	{
		const T& value = (*sloc_prof_it).second;
		return value;
	}
	
	static T zero = T();
	
	return zero;
}


///////////////////////// AnnotatedSourceCodeFile<> ///////////////////////////

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFile<ADDRESS, T>::AnnotatedSourceCodeFile(const char *_filename, const SourceCodeProfile<ADDRESS, T> *_source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, FilenameIndex *_filename_index, const char *_search_path, std::ostream& _warn_log)
	: warn_log(_warn_log)
	, filename(_filename)
	, real_filename()
	, search_path(_search_path)
	, source_code_profile(_source_code_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, filename_index(_filename_index)
	, content()
	, value_range()
{
	Init();
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFile<ADDRESS, T>::~AnnotatedSourceCodeFile()
{
	Clear();
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFile<ADDRESS, T>::Print(std::ostream& os, Visitor& visitor, FileFormat f_fmt) const
{
	OStreamContext osc(os);

	ReportFormat r_fmt = visitor.GetReportFormat();

	switch(f_fmt)
	{
		case F_FMT_TEXT:
		{
			os << "Instruction profile report - Annotated Source Code - [" << GetSampledVariableName() << "]" << std::endl;
			unsigned int num_lines = content.size();
			unsigned int i;
			for(i = 0; i < num_lines; i++)
			{
				unsigned int lineno = i + 1;
				
				const T& value = source_code_profile->GetValue(filename.c_str(), lineno);
				
				os.fill(' ');
				os.flags(std::ios_base::right);
				
				os.width(6);
				os << std::dec << lineno << "  ";
				
				os.width(21);
				
				if(value != T())
				{
					os << std::dec << value;
				}
				else
				{
					os << ' ';
				}
				
				os << "  " << content[i] << std::endl;
			}
			break;
		}
		
		case F_FMT_CSV:
		{
			const std::string& csv_delimiter = visitor.GetCSVDelimiter();
			
			os << "Line number" << csv_delimiter << "Value" << csv_delimiter << "Statement" << std::endl;
			unsigned int num_lines = content.size();
			unsigned int i;
			for(i = 0; i < num_lines; i++)
			{
				unsigned int lineno = i + 1;
				
				const T& value = source_code_profile->GetValue(filename.c_str(), lineno);
				
				os << std::dec << lineno << csv_delimiter;
				
				if(value != T())
				{
					os << std::dec << value;
				}
				
				os << csv_delimiter << c_string_to_CSV(content[i].c_str()) << std::endl;
			}
			break;
		}
		
		case F_FMT_HTML:
		{
			Quantizer<T> quantizer = Quantizer<T>(16, value_range);
			//std::cerr << "\"" << filename << "\": value_range=[" << value_range.first << "," << value_range.second << "]" << std::endl;
			
			Indent indent;
			
			os << indent << "<!DOCTYPE HTML>" << std::endl;
			os << indent << "<html lang=\"en\">" << std::endl;
			os << ++indent << "<head>" << std::endl;
			os << ++indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			os << indent << "<meta name=\"description\" content=\"Instruction profile with annotated source code of " << unisim::util::hypapp::HTML_Encoder::Encode(filename.c_str()) << " for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "\">" << std::endl;
			os << indent << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
			os << indent << "<title>Instruction profile of " << unisim::util::hypapp::HTML_Encoder::Encode(filename.c_str()) << " for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</title>" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_style.css")) << "\" type=\"text/css\" />" << std::endl;
			os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/source_code.css")) << "\" type=\"text/css\" />" << std::endl;
			const std::string& domain = visitor.GetDomain();
			if(!domain.empty())
			{
				os << indent << "<script>document.domain='" << visitor.GetDomain() << "';</script>" << std::endl;
			}
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_script.js")) << "\"></script>" << std::endl;
			os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/script.js")) << "\"></script>" << std::endl;
			os << --indent << "</head>" << std::endl;
			os << indent << "<body";
			if(r_fmt == R_FMT_HTTP)
			{
				os << " onload=\"gui.auto_reload(" << (unsigned int)(visitor.GetRefreshPeriod() * 1000) << ", 'self-refresh-when-active')\"";
			}
			os << ">" << std::endl;
			os << ++indent << "<div id=\"content\">" << std::endl;
			os << ++indent << "<div id=\"show-table\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Go back to function profile table\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_table.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_to_table.svg")) << "\" alt=\"→Table\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<div id=\"show-histogram\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Go back to function profile chart\" draggable=\"false\" ondragstart=\"return false\" href=\"by_function_histogram.html\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_to_histogram.svg")) << "\" alt=\".SVG↓\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			std::string href_csv(std::string("source") + to_string(filename_index->IndexFilename(filename)) + FileFormatSuffix(F_FMT_CSV));
			os << indent << "<div id=\"save\" class=\"button\">" << std::endl;
			os << ++indent << "<a title=\"Save as .CSV spreadsheet file\" draggable=\"false\" ondragstart=\"return false\" href=\"" << href_csv << "\" download=\"" << href_csv << "\" target=\"_blank\">" << std::endl;
			os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_csv_table.svg")) << "\" alt=\".CSV↓\">" << std::endl;
			os << --indent << "</a>" << std::endl;
			os << --indent << "</div>" << std::endl;
			os << indent << "<h1 id=\"title\">Instruction profile with annotated source code of " << unisim::util::hypapp::HTML_Encoder::Encode(filename.c_str()) << "<br>for " << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</h1>" << std::endl;
			
			os << indent << "<table id=\"source\">" << std::endl;
			os << ++indent << "<tr>" << std::endl;
			os << ++indent << "<th>Line</th>" << std::endl;
			//os << "<th>" << unisim::util::hypapp::HTML_Encoder::Encode(GetSampledVariableName()) << "</th>" << std::endl;
			os << indent << "<th>Value</th>" << std::endl;
			os << indent << "<th></th>" << std::endl;
			os << --indent << "</tr>" << std::endl;
			unsigned int num_lines = content.size();
			unsigned int i;
			for(i = 0; i < num_lines; i++)
			{
				unsigned int lineno = i + 1;
				
				const T& value = source_code_profile->GetValue(filename.c_str(), lineno); // may return a zero value
				
				std::stringstream sstr;
				sstr << value;
				
				SLoc sloc = SLoc(filename.c_str(), lineno);
				
				bool is_entry_point = false;
				const char *func_name = func_name_loc_conv->SLocToFunctionName(sloc, is_entry_point);
				
				if(value >= value_range.first)
				{
					unsigned int scale = quantizer.Quantize(value);
					//std::cerr << "#" << lineno << ": " << value << " -> " << scale << std::endl;
					
					os << indent << "<tr class=\"scale" << scale << "\"";
				}
				else
				{
					os << indent << "<tr";
				}
				
				if(func_name)
				{
					if(is_entry_point)
					{
						os << " id=\"" << unisim::util::hypapp::URI_Encoder::Encode(func_name) <<"\"";
					}
				}
				
				os << ">";

				if(func_name)
				{
					os << "<!--in " << unisim::util::hypapp::HTML_Encoder::Encode(func_name) << " -->";
				}
				
				os << "<td class=\"lineno\">" << lineno << "</td><td class=\"value\">" << ((value != T()) ? unisim::util::hypapp::HTML_Encoder::Encode(sstr.str().c_str()) : "") << "<td class=\"source-code\">" << unisim::util::hypapp::HTML_Encoder::Encode(content[i].c_str()) << "</td></tr>" << std::endl;
			}
			os << --indent << "</table>" << std::endl;
			
			os << --indent << "</div>" << std::endl;
			os << --indent << "</body>" << std::endl;
			os << --indent << "</html>" << std::endl;
			break;
		}
		
		default:
			break;
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFile<ADDRESS, T>::Init()
{
	if(LocateFile(filename.c_str(), real_filename))
	{
		std::ifstream f(real_filename.c_str(), std::ifstream::in);
		
		if(f.fail())
		{
			warn_log << "Can't open \"" << real_filename << "\"" << std::endl;
		}
		else
		{
			std::string line;
			
			while(std::getline(f, line))
			{
				content.push_back(line);
			}
			
			if(!f.eof())
			{
				warn_log << "I/O error while reading \"" << real_filename << "\"" << std::endl;
			}
		}
	}
	else
	{
		warn_log << "File \"" << filename << "\" not found" << std::endl;
	}
	
	value_range = source_code_profile->GetValueRange();
	
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		annotated_source_code_file_printers[f_fmt] = new AnnotatedSourceCodeFilePrinter(*this, f_fmt);
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFile<ADDRESS, T>::Clear()
{
	for(FileFormat f_fmt = static_cast<FileFormat>(0); f_fmt != NUM_FILE_FORMATS; f_fmt = static_cast<FileFormat>(f_fmt + 1))
	{
		delete annotated_source_code_file_printers[f_fmt];
	}
}

template <typename ADDRESS, typename T>
bool AnnotatedSourceCodeFile<ADDRESS, T>::LocateFile(const char *filename, std::string& match_file_path)
{
	const std::string& shared_data_dir = unisim::kernel::Simulator::Instance()->GetSharedDataDirectory();
	
	if(access(filename, R_OK) == 0)
	{
		match_file_path = filename;
		return true;
	}
	
	std::string s;
	const char *p;

	std::vector<std::string> search_paths;

	s.clear();
	p = search_path.c_str();
	do
	{
		if(*p == 0 || *p == ';')
		{
			search_paths.push_back(s);
			search_paths.push_back(shared_data_dir + '/' + s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	std::vector<std::string> hierarchical_path;
	
	s.clear();
	p = filename;
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_path.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	bool match = false;
	
	int hierarchical_path_depth = hierarchical_path.size();
	
	if(hierarchical_path_depth > 0)
	{
		int num_search_paths = search_paths.size();
		int k;
		
		for(k = 0; k < num_search_paths; k++)
		{
			const std::string& search_path = search_paths[k];
			int i;
			for(i = 0; (!match) && (i < hierarchical_path_depth); i++)
			{
				std::string try_file_path = search_path;
				int j;
				for(j = i; j < hierarchical_path_depth; j++)
				{
					if(!try_file_path.empty()) try_file_path += '/';
					try_file_path += hierarchical_path[j];
				}
				//std::cerr << "try_file_path=\"" << try_file_path << "\":";
				if(access(try_file_path.c_str(), R_OK) == 0)
				{
					//std::cerr << "found" << std::endl;
					match = true;
					match_file_path = try_file_path;
				}
				else
				{
					//std::cerr << "not found" << std::endl;
				}
			}
		}
	}
	
	return match;
}

template <typename ADDRESS, typename T>
const Printer& AnnotatedSourceCodeFile<ADDRESS, T>::GetPrinter(FileFormat f_fmt) const
{
	return *annotated_source_code_file_printers[f_fmt];
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFile<ADDRESS, T>::AnnotatedSourceCodeFilePrinter::AnnotatedSourceCodeFilePrinter(AnnotatedSourceCodeFile<ADDRESS, T>& _annotated_source_code_file, FileFormat _f_fmt)
	: annotated_source_code_file(_annotated_source_code_file)
	, f_fmt(_f_fmt)
{
}

template <typename ADDRESS, typename T>
FileFormat AnnotatedSourceCodeFile<ADDRESS, T>::AnnotatedSourceCodeFilePrinter::GetFileFormat() const
{
	return f_fmt;
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFile<ADDRESS, T>::AnnotatedSourceCodeFilePrinter::Print(std::ostream& os, Visitor& visitor) const
{
	annotated_source_code_file.Print(os, visitor, f_fmt);
}


template <typename ADDRESS, typename T>
AnnotatedSourceCodeFileSet<ADDRESS, T>::AnnotatedSourceCodeFileSet(const SourceCodeProfile<ADDRESS, T> *_source_code_profile, const FunctionNameLocationConversionBase<ADDRESS> *_func_name_loc_conv, FilenameIndex *_filename_index, const char *_search_path, std::ostream& _warn_log)
	: warn_log(_warn_log)
	, source_code_profile(_source_code_profile)
	, func_name_loc_conv(_func_name_loc_conv)
	, filename_index(_filename_index)
	, search_path(_search_path)
	, annotated_source_code_files()
{
	Init();
}

template <typename ADDRESS, typename T>
AnnotatedSourceCodeFileSet<ADDRESS, T>::~AnnotatedSourceCodeFileSet()
{
	Clear();
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Output(Visitor& visitor)
{
	ReportFormat r_fmt = visitor.GetReportFormat();
	FileFormat f_fmt = F_FMT_NONE;
	switch(r_fmt)
	{
		case R_FMT_TEXT: f_fmt = F_FMT_TEXT; break;
		case R_FMT_CSV: f_fmt = F_FMT_CSV; break;
		case R_FMT_HTML:
		case R_FMT_HTTP: f_fmt = F_FMT_HTML; break;
	}
	assert(f_fmt != F_FMT_NONE);
	typename std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *>::const_iterator annotated_source_code_file_it;
	
	for(annotated_source_code_file_it = annotated_source_code_files.begin(); annotated_source_code_file_it != annotated_source_code_files.end(); annotated_source_code_file_it++)
	{
		const AnnotatedSourceCodeFile<ADDRESS, T> *annotated_source_code_file = (*annotated_source_code_file_it).second;
		
		if(!visitor.Visit(GetSampledVariableName(), std::string("source") + to_string(filename_index->IndexFilename(annotated_source_code_file->GetFilename())) + FileFormatSuffix(f_fmt), annotated_source_code_file->GetPrinter(f_fmt))) return;
		
		if((f_fmt != F_FMT_CSV) && ((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP)))
		{
			if(!visitor.Visit(GetSampledVariableName(), std::string("source") + to_string(filename_index->IndexFilename(annotated_source_code_file->GetFilename())) + FileFormatSuffix(F_FMT_CSV), annotated_source_code_file->GetPrinter(F_FMT_CSV))) return;
		}
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Init()
{
	Clear();
	
	const std::set<std::string>& source_filename_set = source_code_profile->GetSourceFilenameSet();
	
	typename std::set<std::string>::const_iterator source_filename_set_it;
	for(source_filename_set_it = source_filename_set.begin(); source_filename_set_it != source_filename_set.end(); source_filename_set_it++)
	{
		const std::string& source_filename = *source_filename_set_it;
		
		filename_index->IndexFilename(source_filename);

		annotated_source_code_files[source_filename] = new AnnotatedSourceCodeFile<ADDRESS, T>(source_filename.c_str(), source_code_profile, func_name_loc_conv, filename_index, search_path.c_str(), warn_log);
	}
}

template <typename ADDRESS, typename T>
void AnnotatedSourceCodeFileSet<ADDRESS, T>::Clear()
{
	typename std::map<std::string, const AnnotatedSourceCodeFile<ADDRESS, T> *>::iterator annotated_source_code_file_it;
	
	for(annotated_source_code_file_it = annotated_source_code_files.begin(); annotated_source_code_file_it != annotated_source_code_files.end(); annotated_source_code_file_it++)
	{
		const AnnotatedSourceCodeFile<ADDRESS, T> *annotated_source_code_file = (*annotated_source_code_file_it).second;
		delete annotated_source_code_file;
	}
	annotated_source_code_files.clear();
}

//////////////////////////////// Profiler<> ///////////////////////////////////

template <typename ADDRESS>
Profiler<ADDRESS>::Profiler(const char *_name, Object *_parent)
	: Object(_name, _parent, "this service implements an instruction profiler")
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, http_server_export("http-server-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, backtrace_import("backtrace-import", this)
	, profiling_import("profiling-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, logger(*this)
	, search_path()
	, filename()
	, sampled_variables()
	, output_directory()
	, csv_delimiter(",")
	, csv_hyperlink("HYPERLINK")
	, csv_arg_separator(";")
	, enable_text_report(false)
	, enable_html_report(false)
	, enable_csv_report(false)
	, verbose(false)
	, http_refresh_period(1.0)
	, param_search_path("search-path", this, search_path, "Search path for source (separated by ';')")
	, param_filename("filename", this, filename, "List of (binary) files (preferably ELF) to profile (separated by ',')")
	, param_sampled_variables("sampled-variables", this, sampled_variables, "Variables to sample (separated by spaces)")
	, param_output_directory("output-directory", this, output_directory, "Output directory where to generate profiling report")
	, param_csv_delimiter("csv-delimiter", this, csv_delimiter, "CSV delimiter")
	, param_csv_hyperlink("csv-hyperlink", this, csv_hyperlink, "CSV hyperlink macro (e.g. HYPERLINK or LIEN_HYPERTEXTE)")
	, param_csv_arg_separator("csv-arg-separator", this, csv_arg_separator, "CSV argument separator in formulas (e.g. ';' or ',')")
	, param_enable_text_report("enable-text-report", this, enable_text_report, "Enable/Disable text report")
	, param_enable_html_report("enable-html-report", this, enable_html_report, "Enable/Disable HTML report")
	, param_enable_csv_report("enable-csv-report", this, enable_csv_report, "Enable/Disable CSV report")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_http_refresh_period("http-refresh-period", this, http_refresh_period, "Refresh period for reporting over HTTP")
	, listening_commit(false)
	, trap(false)
	, commit_insn_event(0)
	, num_sampled_variables(0)
	, func_name_loc_conv(0)
	, need_update(true)
	, addr_profiles()
	, func_insn_profiles()
	, pc(0)
	, instruction_profiles()
	, source_code_profiles()
	, annotated_source_code_file_sets()
	, filename_indexes()
	, http_print_times_per_path()
	, mutex()
{
	pthread_mutex_init(&mutex, NULL);
	
	commit_insn_event = new unisim::util::debug::CommitInsnEvent<ADDRESS>();
	commit_insn_event->Catch();
}

template <typename ADDRESS>
Profiler<ADDRESS>::~Profiler()
{
	unsigned int i;
	
	if(num_sampled_variables > 0)
	{
		UnlistenCommit();
	}

	if(func_name_loc_conv) delete func_name_loc_conv;
	
	unsigned int num_func_insn_profiles = func_insn_profiles.size();
	for(i = 0; i < num_func_insn_profiles; i++)
	{
		FunctionInstructionProfileBase *func_insn_profile = func_insn_profiles[i];
		delete func_insn_profile;
	}

	unsigned int num_annotated_source_code_file_sets = annotated_source_code_file_sets.size();
	for(i = 0; i < num_annotated_source_code_file_sets; i++)
	{
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		delete annotated_source_code_file_set;
	}

	unsigned int num_instruction_profiles = instruction_profiles.size();
	for(i = 0; i < num_instruction_profiles; i++)
	{
		InstructionProfileBase *instruction_profile = instruction_profiles[i];
		delete instruction_profile;
	}

	unsigned int num_source_code_profiles = source_code_profiles.size();
	for(i = 0; i < num_source_code_profiles; i++)
	{
		SourceCodeProfileBase<ADDRESS> *source_code_profile = source_code_profiles[i];
		delete source_code_profile;
	}

	unsigned int num_addr_profiles = addr_profiles.size();
	for(i = 0; i < num_addr_profiles; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		delete addr_profile;
	}
	
	unsigned int num_filename_indexes = filename_indexes.size();
	for(i = 0; i < num_filename_indexes; i++)
	{
		FilenameIndex *filename_index = filename_indexes[i];
		delete filename_index;
	}

	if(commit_insn_event) commit_insn_event->Release();
	
	pthread_mutex_destroy(&mutex);
}

template<class ADDRESS>
bool Profiler<ADDRESS>::EndSetup()
{
	num_sampled_variables = 0;
	
	if(!registers_import)
	{
		logger << DebugError << "registers_import is not connected" << EndDebugError;
		return false;
	}
	
	if(!stmt_lookup_import)
	{
		logger << DebugError << "stmt_lookup_import is not connected" << EndDebugError;
		return false;
	}
	
	if(!symbol_table_lookup_import)
	{
		logger << DebugError << "symbol_table_lookup_import is not connected" << EndDebugError;
		return false;
	}

	LoadDebugInfo();
	
	func_name_loc_conv = new FunctionNameLocationConversion<ADDRESS>(stmt_lookup_import, symbol_table_lookup_import, logger.DebugWarningStream());
	
	std::stringstream sstr(sampled_variables);
	std::string var_name;

	while(sstr >> var_name)
	{
		unisim::kernel::VariableBase *var = unisim::kernel::Object::GetSimulator()->FindVariable(var_name.c_str());
		
		if(var->IsVoid())
		{
			logger << DebugWarning << "Variable \"" << var_name << "\" does not exist" << EndDebugWarning;
		}
		else
		{
			if(TryProfile<signed char>(var) ||
			   TryProfile<short>(var) ||
			   TryProfile<int>(var) ||
			   TryProfile<long>(var) ||
			   TryProfile<long long>(var) ||
			   TryProfile<unsigned char>(var) ||
			   TryProfile<unsigned short>(var) ||
			   TryProfile<unsigned int>(var) ||
			   TryProfile<unsigned long>(var) ||
			   TryProfile<unsigned long long>(var) ||
			   TryProfile<double>(var)
			)
			{
				logger << DebugInfo << "Profiling for Variable \"" << var_name << "\"" << EndDebugInfo;
			}
			else if((strcmp(var->GetDataTypeName(), "sc_time") == 0) && TryProfile<sc_core::sc_time>(var))
			{
				logger << DebugInfo << "Profiling for Variable \"" << var_name << "\"" << EndDebugInfo;
			}
			else
			{
				logger << DebugWarning << "Can't profile for Statistic \"" << var_name << "\"" << EndDebugWarning;
			}
		}
	}
	
	listening_commit = false;
	pc = 0;
	trap = false;

	if(num_sampled_variables > 0)
	{
		if(!ListenCommit()) return false;
	}
	
	return true;
}

template <typename ADDRESS>
void Profiler<ADDRESS>::OnDisconnect()
{
}

// unisim::service::interfaces::DebugYielding
template <typename ADDRESS>
void Profiler<ADDRESS>::DebugYield()
{
	unsigned int num_addr_profiles = addr_profiles.size();
	
	if(num_addr_profiles)
	{
		Lock();
		
		unsigned int i;
		
		for(i = 0; i < num_addr_profiles; i++)
		{
			AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
			
			addr_profile->Capture(pc, length);
		}
		
		need_update = true;
		Unlock();
	}
}

// unisim::service::interfaces::DebugEventListener<ADDRESS>
template <typename ADDRESS>
void Profiler<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	
	if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN))
	{
		const unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<const unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);
		
		pc = commit_insn_event->GetAddress();
		length = commit_insn_event->GetLength();
	}
}

// unisim::service::interfaces::HttpServer
template <typename ADDRESS>
bool Profiler<ADDRESS>::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	class HttpVisitor : public Visitor
	{
	public:
		HttpVisitor(Profiler<ADDRESS>& _profiler, std::ostream& _stream, const char *_host, const std::string& _root, const std::string& _req_path, const std::string& _domain, double _refresh_period)
			: profiler(_profiler)
			, stream(_stream)
			, host(_host ? _host : "")
			, root(_root)
			, req_path(_req_path)
			, domain(_domain)
			, dir_path()
			, refresh_period(_refresh_period)
		{
		}
		
		virtual const std::string& GetCSVDelimiter() const
		{
			return profiler.csv_delimiter;
		}
		
		virtual const std::string& GetCSVHyperlink() const
		{
			return profiler.csv_hyperlink;
		}
		
		virtual const std::string& GetCSVArgSeparator() const
		{
			return profiler.csv_arg_separator;
		}
		
		virtual const std::string& GetRoot() const
		{
			return root;
		}
		
		virtual const std::string& GetDomain() const
		{
			return domain;
		}
		
		virtual ReportFormat GetReportFormat() const
		{
			return R_FMT_HTTP;
		}
		
		virtual std::string GetFilePath(const std::string& css_file) const
		{
			return std::string("/") + css_file;
		}
		
		virtual const std::string& GetDirPath() const
		{
			return dir_path;
		}
		
		virtual double GetRefreshPeriod() const
		{
			return refresh_period;
		}
		
		virtual bool Visit(const std::string& dirname, const std::string& filename, const Printer& printer)
		{
			dir_path = host;
			dir_path += root;
			if(!dirname.empty())
			{
				dir_path += '/';
				dir_path += dirname;
			}
			std::string visited_path;
			if(!dirname.empty())
			{
				visited_path += dirname;
				visited_path += '/';
			}
			visited_path += filename;
			
			if((visited_path == req_path) || ((visited_path == "index.html") && req_path.empty()))
			{
				switch(printer.GetFileFormat())
				{
					case F_FMT_TEXT: content_type = "text/plain; charset=utf-8"; break;
					case F_FMT_HTML: content_type = "text/html; charset=utf-8";  break;
					case F_FMT_CSV : content_type = "text/csv; charset=utf-8";   break;
					case F_FMT_SVG : content_type = "image/svg+xml";             break;
					default        :                                             break;
				}
				printer.Print(stream, *this);
				return false;
			}
			
			return true;
		}
		
		const std::string& GetContentType() const
		{
			return content_type;
		}
	private:
		Profiler<ADDRESS>& profiler;
		std::ostream& stream;
		const char *host;
		const std::string& root;
		const std::string& req_path;
		const std::string& domain;
		std::string content_type;
		std::string dir_path;
		double refresh_period;
	};
	
	double print_time = http_print_times_per_path[req.GetPath()];
	double refresh_period = std::max(4 * print_time, http_refresh_period); // configured refresh period for reporting over HTTP or at least four times the last print time
	HttpVisitor http_visitor(*this, response, req.GetHost(), URI(), req.GetPath(), req.GetDomain(), refresh_period);
	double start_time = unisim::util::host_time::GetHostTime();
	Output(http_visitor);
	double end_time = unisim::util::host_time::GetHostTime();
	print_time = end_time - start_time;
	http_print_times_per_path[req.GetPath()] = print_time;
	
	bool available = (response.str().length() != 0);
	
	if(available)
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::OPTIONS:
				response.ClearContent();
				response.Allow("OPTIONS, GET, HEAD");
				break;
				
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
				response.SetContentType(http_visitor.GetContentType());
				break;
				
			case unisim::util::hypapp::Request::POST:
				if((req.GetPath() == "") || (req.GetPath() == "index.html"))
				{
					Lock();
					PropertySetter property_setter(*this);
					if(property_setter.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
					{
						property_setter.Apply();
					}
					else
					{
						logger << DebugWarning << "parse error in POST data" << EndDebugWarning;
					}
					Unlock();
					
					// Post/Redirect/Get pattern: got Post, so do Redirect
					response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
					response.SetHeaderField("Location", req.GetRequestURI());
					
					break;
				}
				
			default:
			{
				response.ClearContent();
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);

				Indent indent;
				
				response << indent << "<!DOCTYPE html>" << std::endl;
				response << indent << "<html lang=\"en\">" << std::endl;
				response << ++indent << "<head>" << std::endl;
				response << ++indent << "<title>Error 405 (Method Not Allowed)</title>" << std::endl;
				response << indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << indent << "<meta name=\"description\" content=\"Error 405 (Method Not Allowed)\">" << std::endl;
				response << indent << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << indent << "<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << indent << "<script src=\"unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				response << indent << "<script src=\"unisim/service/debug/profiler/script.js\"></script>" << std::endl;
				response << indent << "<style>" << std::endl;
				response << ++indent << "body { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << --indent << "</style>" << std::endl;
				response << --indent << "</head>" << std::endl;
				response << indent << "<body onload=\"gui.auto_reload(0)\">" << std::endl;
				response << ++indent << "<p>Method Not Allowed</p>" << std::endl;
				response << --indent << "</body>" << std::endl;
				response << --indent << "</html>" << std::endl;
				break;
			}
		}
	}
	else
	{
		response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);

		Indent indent;
		
		response << indent << "<!DOCTYPE html>" << std::endl;
		response << indent << "<html lang=\"en\">" << std::endl;
		response << ++indent << "<head>" << std::endl;
		response << ++indent << "<title>Error 404 (Not Found)</title>" << std::endl;
		response << indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << indent << "<meta name=\"description\" content=\"Error 404 (Not Found)\">" << std::endl;
		response << indent << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
		response << indent << "<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << indent << "<style>" << std::endl;
		response << ++indent << "body { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
		response << --indent << "</style>" << std::endl;
		response << --indent << "</head>" << std::endl;
		response << indent << "<body>" << std::endl;
		response << ++indent << "<p>Unavailable</p>" << std::endl;
		response << --indent << "</body>" << std::endl;
		response << --indent << "</html>" << std::endl;
	}
	
	return conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));
}

template <typename ADDRESS>
void Profiler<ADDRESS>::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
	std::stringstream sstr(sampled_variables);
	std::string var_name;

	while(sstr >> var_name)
	{
		scanner.Append(unisim::service::interfaces::BrowserOpenTabAction(
			/* name        */ std::string(this->GetName()) + "-" + var_name,
			/* object name */ this->GetName(),
			/* label       */ std::string("Show profile for ") + var_name,
			/* tile        */ unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
			/* uri         */ URI() + "/" + unisim::util::hypapp::URI_Encoder::EncodeComponent(var_name) + "/by_function_histogram.html"
		));
	}
}

template <typename ADDRESS>
bool Profiler<ADDRESS>::ListenCommit()
{
	if(listening_commit) return true;
	
	if(!debug_event_trigger_import->Listen(commit_insn_event))
	{
		logger << DebugError << "Can't track committed instructions" << EndDebugError;
		return false;
	}

	listening_commit = true;
	
	return true;
}

template <typename ADDRESS>
bool Profiler<ADDRESS>::UnlistenCommit()
{
	if(!listening_commit) return true;
	
	if(!debug_event_trigger_import->Unlisten(commit_insn_event))
	{
		logger << DebugError << "Can't untrack committed instructions" << EndDebugError;
		return false;
	}
	
	listening_commit = false;
	
	return true;
}

template <typename ADDRESS>
template <typename T> bool Profiler<ADDRESS>::TryProfile(unisim::kernel::VariableBase *var)
{
	unisim::kernel::variable::Variable<T> *typed_var = dynamic_cast<unisim::kernel::variable::Variable<T> *>(var);
	
	if(typed_var)
	{
		FilenameIndex *filename_index = new FilenameIndex();
		filename_indexes.push_back(filename_index);
		
		AddressProfile<ADDRESS, T> *addr_profile = new AddressProfile<ADDRESS, T>(typed_var, disasm_import, stmt_lookup_import, symbol_table_lookup_import, logger.DebugWarningStream());
		addr_profiles.push_back(addr_profile);
		
		InstructionProfileBase *instruction_profile = addr_profile->CreateInstructionProfile(func_name_loc_conv, filename_index);
		instruction_profiles.push_back(instruction_profile);
		
		SourceCodeProfileBase<ADDRESS> *source_code_profile = addr_profile->CreateSourceCodeProfile();
		source_code_profiles.push_back(source_code_profile);
		
		num_sampled_variables++;
		return true;
	}
	
	return false;
}

template <typename ADDRESS>
void Profiler<ADDRESS>::LoadDebugInfo()
{
	// Disable all binaries for debugging
	
	std::list<std::string> binaries;
	
	debug_info_loading_import->EnumerateBinaries(binaries);
	
	std::list<std::string>::iterator binary_it;
	for(binary_it = binaries.begin(); binary_it != binaries.end(); binary_it++)
	{
		const std::string& binary = *binary_it;
		
		debug_info_loading_import->EnableBinary(binary.c_str(), false);
	}
	
	// Selectively load/reenable debug information from binaries
	if(filename.empty())
	{
		logger << DebugWarning << "No debugging information loaded" << EndDebugWarning;
	}
	else
	{
		std::size_t pos = 0;
		std::size_t delim_pos = std::string::npos;
		
		do
		{
			delim_pos = filename.find_first_of(",", pos);
			
			std::string _filename = (delim_pos != std::string::npos) ? filename.substr(pos, delim_pos - pos) : filename.substr(pos);
			
			std::string real_filename = unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(_filename.c_str());
			
			if(debug_info_loading_import->EnableBinary(real_filename.c_str(), true))
			{
				logger << DebugInfo << "Debugging information from \"" << real_filename << "\" previously loaded" << EndDebugInfo;
			}
			else
			{
				if(debug_info_loading_import->LoadDebugInfo(real_filename.c_str()))
				{
					logger << DebugInfo << "Debugging information from \"" << real_filename << "\" loaded" << EndDebugInfo;
				}
				else
				{
					logger << DebugInfo << "No debugging information loaded from \"" << real_filename << "\"" << EndDebugInfo;
				}
			}
			pos = (delim_pos != std::string::npos) ? delim_pos + 1 : std::string::npos;
		}
		while(pos != std::string::npos);
	}
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Clear()
{
	unsigned int i;
	for(i = 0; i < num_sampled_variables; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		addr_profile->Clear();
	}
	need_update = true;
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Clear(const std::string& variable_name)
{
	unsigned int i;
	for(i = 0; i < num_sampled_variables; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		if(variable_name == addr_profile->GetSampledVariableName())
		{
			addr_profile->Clear();
		}
	}
	need_update = true;
}


template <typename ADDRESS>
void Profiler<ADDRESS>::Update()
{
	unsigned int i;
	
	unsigned int num_func_insn_profiles = func_insn_profiles.size();
	for(i = 0; i < num_func_insn_profiles; i++)
	{
		FunctionInstructionProfileBase *func_insn_profile = func_insn_profiles[i];
		delete func_insn_profile;
	}
	func_insn_profiles.clear();

	unsigned int num_annotated_source_code_file_sets = annotated_source_code_file_sets.size();
	for(i = 0; i < num_annotated_source_code_file_sets; i++)
	{
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		delete annotated_source_code_file_set;
	}
	annotated_source_code_file_sets.clear();
	
	for(i = 0; i < num_sampled_variables; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		
		FilenameIndex *filename_index = filename_indexes[i];
		
		FunctionInstructionProfileBase *func_insn_profile = addr_profile->CreateFunctionInstructionProfile(func_name_loc_conv, filename_index);
		func_insn_profiles.push_back(func_insn_profile);
	}
	
	for(i = 0; i < num_sampled_variables; i++)
	{
		SourceCodeProfileBase<ADDRESS> *source_code_profile = source_code_profiles[i];
		
		source_code_profile->Update();
		
		FilenameIndex *filename_index = filename_indexes[i];
		
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = source_code_profile->CreateAnnotatedSourceCodeFileSet(func_name_loc_conv, filename_index, search_path.c_str());
		annotated_source_code_file_sets.push_back(annotated_source_code_file_set);
	}
}

#if 0
template <typename ADDRESS>
void Profiler<ADDRESS>::Print(std::ostream& os, DebugVisitor& dbg_visitor)
{
	OStreamContext osc(os);

	unsigned int i;

	for(i = 0; i < num_sampled_variables; i++)
	{
		os << "################ Address profiles ###################" << std::endl;
	
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
	
		os << "[" << addr_profile->GetSampledVariableName() << "]" << std::endl;
		addr_profile->Print(os, dbg_visitor);

		os << "################ Function/Source code profiles ###################" << std::endl;
	
		SourceCodeProfileBase<ADDRESS> *source_code_profile = source_code_profiles[i];
		
		os << "[" << source_code_profile->GetSampledVariableName() << "]" << std::endl;
		source_code_profile->Print(os, dbg_visitor);
		
		continue;
		
		os << "################ Annotated Source codes ###################" << std::endl;

		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		
		os << "[" << annotated_source_code_file_set->GetSampledVariableName() << "]" << std::endl;
		annotated_source_code_file_set->Print(os, dbg_visitor);
	}
}
#endif

template <typename ADDRESS>
void Profiler<ADDRESS>::PrintIndex(std::ostream& os, Visitor& visitor) const
{
	const std::string& root = visitor.GetRoot();
	
	unsigned int i;
	
	ReportFormat r_fmt = visitor.GetReportFormat();

	Indent indent;
	
	os << indent << "<!DOCTYPE HTML>" << std::endl;
	os << indent << "<html lang=\"en\">" << std::endl;
	os << ++indent << "<head>" << std::endl;
	os << ++indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	os << indent << "<meta name=\"description\" content=\"Instruction profile index by variable\">" << std::endl;
	os << indent << "<meta name=\"keywords\" content=\"profiling\">" << std::endl;
	os << indent << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	os << indent << "<title>Instruction profile index by variable</title>" << std::endl;
	os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_style.css")) << "\" type=\"text/css\" />" << std::endl;
	os << indent << "<link rel=\"stylesheet\" href=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/index.css")) << "\" type=\"text/css\" />" << std::endl;
	const std::string& domain = visitor.GetDomain();
	if(!domain.empty())
	{
		os << indent << "<script>document.domain='" << visitor.GetDomain() << "';</script>" << std::endl;
	}
	os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/http_server/embedded_script.js")) << "\"></script>" << std::endl;
	os << indent << "<script src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/index.js")) << "\"></script>" << std::endl;
	os << --indent << "</head>" << std::endl;
	os << indent << "<body";
	if(r_fmt == R_FMT_HTTP)
	{
		os << " onload=\"gui.auto_reload(" << (unsigned int)(visitor.GetRefreshPeriod() * 1000) << ", 'self-refresh-when-active')\"";
	}
	os << ">" << std::endl;
	os << ++indent << "<div id=\"content\">" << std::endl;
	os << ++indent << "<h1 id=\"title\">Instruction profile index by variable</h1>" << std::endl;
	os << indent << "<table id=\"var-index\">" << std::endl;
	os << ++indent << "<tr>" << std::endl;
	os << ++indent << "<th id=\"var-clear-all\"><form id=\"var-clear-all-form\" action=\"" << URI() << "\" method=\"post\">" << std::endl;
	os << ++indent << "<div class=\"button\"><input type=\"hidden\" name=\"clear-all\" value=\"on\">" << std::endl;
	os << ++indent << "<a title=\"Clear all\" draggable=\"false\" ondragstart=\"return false\" onclick=\"document.getElementById('var-clear-all-form').submit()\">" << std::endl;
	os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_sweeper.svg")) << "\" alt=\"Clear all\">" << std::endl;
	os << --indent << "</a>" << std::endl;
	os << --indent << "</div>" << std::endl;
	os << --indent << "</form>" << std::endl;
	os << --indent << "</th>" << std::endl;
	os << indent << "<th>Variable</th>" << std::endl;
	os << indent << "<th>Description</th>" << std::endl;
	os << indent << "<th>Value</th>" << std::endl;
	os << indent << "<th colspan=\"3\">Profile</th>" << std::endl;
	os << --indent << "</tr>" << std::endl;
	for(i = 0; i < num_sampled_variables; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		
		os << indent << "<tr>" << std::endl;
		os << ++indent << "<td class=\"var-clear\">" << std::endl;
		os << ++indent << "<form id=\"var-clear-form-" << i << "\" action=\"" << URI() << "\" method=\"post\"><div class=\"button\">" << std::endl;
		os << ++indent << "<input type=\"hidden\" name=\"clear*" << unisim::util::hypapp::URI_Encoder::Encode(addr_profile->GetSampledVariableName()) << "\" value=\"on\">" << std::endl;
		os << ++indent << "<a title=\"Clear\" draggable=\"false\" ondragstart=\"return false\" onclick=\"document.getElementById('var-clear-form-" << i << "').submit()\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_sweeper.svg")) << "\" alt=\"Clear\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << --indent << "</form>" << std::endl;
		os << --indent << "</td>" << std::endl;
		os << indent << "<td class=\"var-name\">" << unisim::util::hypapp::HTML_Encoder::Encode(addr_profile->GetSampledVariableName()) << "</td>" << std::endl;
		os << indent << "<td class=\"var-description\">" << unisim::util::hypapp::HTML_Encoder::Encode(addr_profile->GetSampledVariable()->GetDescription()) << "</td>" << std::endl;
		os << indent << "<td class=\"var-value\">" << unisim::util::hypapp::HTML_Encoder::Encode(addr_profile->GetCumulativeValueAsString()) << "</td>" << std::endl;
		os << indent << "<td class=\"by-address\">" << std::endl;
		os << ++indent << "<div class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Show instruction profile by address\" draggable=\"false\" ondragstart=\"return false\" href=\"" << root << (root.empty() ? "" : "/") << unisim::util::hypapp::URI_Encoder::Encode(addr_profile->GetSampledVariableName()) << "/by_address.html\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_pc_table.svg")) << "\" alt=\"PC Table\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << --indent << "</td>" << std::endl;
		os << indent << "<td class=\"by-address-histogram\">" << std::endl;
		os << ++indent << "<div class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Show function profile chart\" draggable=\"false\" ondragstart=\"return false\" href=\"" << root << (root.empty() ? "" : "/") << unisim::util::hypapp::URI_Encoder::Encode(addr_profile->GetSampledVariableName()) << "/by_function_histogram.html\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_histogram.svg")) << "\" alt=\"Histogram\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << --indent << "</td>" << std::endl;
		os << indent << "<td class=\"by-address-table\">" << std::endl;
		os << ++indent << "<div class=\"button\">" << std::endl;
		os << ++indent << "<a title=\"Show function profile table\" draggable=\"false\" ondragstart=\"return false\" href=\"" << root << (root.empty() ? "" : "/") << unisim::util::hypapp::URI_Encoder::Encode(addr_profile->GetSampledVariableName()) << "/by_function_table.html\">" << std::endl;
		os << ++indent << "<img draggable=\"false\" src=\"" << unisim::util::hypapp::URI_Encoder::Encode(visitor.GetFilePath("unisim/service/debug/profiler/icon_table.svg")) << "\" alt=\"Table\">" << std::endl;
		os << --indent << "</a>" << std::endl;
		os << --indent << "</div>" << std::endl;
		os << --indent << "</td>" << std::endl;
		os << --indent << "</tr>" << std::endl;
	}
	os << --indent << "</table>" << std::endl;
	os << --indent << "</div>" << std::endl;
	os << --indent << "</body>" << std::endl;
	os << --indent << "</html>" << std::endl;
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Output(Visitor& visitor)
{
	if(num_sampled_variables == 0) return;
	
	Lock();
	
	if(need_update)
	{
		Update();
		need_update = false;
	}
	
	unsigned int i;
	
	ReportFormat r_fmt = visitor.GetReportFormat();

	if((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP))
	{
		struct IndexPrinter : Printer
		{
			IndexPrinter(Profiler<ADDRESS>& _profiler)
				: profiler(_profiler)
			{
			}
			
			virtual FileFormat GetFileFormat() const
			{
				return F_FMT_HTML;
			}
			
			virtual void Print(std::ostream& os, Visitor& visitor) const
			{
				profiler.PrintIndex(os, visitor);
			}
			
		private:
			Profiler<ADDRESS>& profiler;
		};
		
		IndexPrinter index_printer(*this);
		
		if(!visitor.Visit("", "index.html", index_printer))
		{
			Unlock();
			return;
		}
	}

	FileFormat f_fmt = F_FMT_NONE;
	switch(r_fmt)
	{
		case R_FMT_TEXT: f_fmt = F_FMT_TEXT; break;
		case R_FMT_CSV: f_fmt = F_FMT_CSV; break;
		case R_FMT_HTML:
		case R_FMT_HTTP: f_fmt = F_FMT_HTML; break;
	}
	
	assert(f_fmt != F_FMT_NONE);
	for(i = 0; i < num_sampled_variables; i++)
	{
		AddressProfileBase<ADDRESS> *addr_profile = addr_profiles[i];
		
		if(!visitor.Visit(addr_profile->GetSampledVariableName(), std::string("by_address") + FileFormatSuffix(f_fmt), addr_profile->GetPrinter(f_fmt)))
		{
			Unlock();
			return;
		}
		
		if((f_fmt != F_FMT_CSV) && ((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP)))
		{
			if(!visitor.Visit(addr_profile->GetSampledVariableName(), std::string("by_address") + FileFormatSuffix(F_FMT_CSV), addr_profile->GetPrinter(F_FMT_CSV)))
			{
				Unlock();
				return;
			}
		}
	}

	for(i = 0; i < num_sampled_variables; i++)
	{
		FunctionInstructionProfileBase *func_insn_profile = func_insn_profiles[i];
		
		if(!visitor.Visit(func_insn_profile->GetSampledVariableName(), std::string("by_function_table") + FileFormatSuffix(f_fmt), func_insn_profile->GetTablePrinter(f_fmt))) break;
		
		if((f_fmt != F_FMT_CSV) && ((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP)))
		{
			if(!visitor.Visit(func_insn_profile->GetSampledVariableName(), std::string("by_function_table.csv"), func_insn_profile->GetTablePrinter(F_FMT_CSV))) break;
		}
		
		if(!visitor.Visit(func_insn_profile->GetSampledVariableName(), std::string("by_function_histogram.svg"), func_insn_profile->GetHistogramPrinter(F_FMT_SVG))) break;

		if((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP))
		{
			if(!visitor.Visit(func_insn_profile->GetSampledVariableName(), std::string("by_function_histogram.html"), func_insn_profile->GetHistogramPrinter(F_FMT_HTML))) break;
		}
		
		InstructionProfileBase *instruction_profile = instruction_profiles[i];
		
		if(!visitor.Visit(func_insn_profile->GetSampledVariableName(), std::string("disassembly") + FileFormatSuffix(f_fmt), instruction_profile->GetPrinter(f_fmt))) break;
		
		if((f_fmt != F_FMT_CSV) && ((r_fmt == R_FMT_HTML) || (r_fmt == R_FMT_HTTP)))
		{
			if(!visitor.Visit(func_insn_profile->GetSampledVariableName(), std::string("disassembly.csv"), instruction_profile->GetPrinter(F_FMT_CSV))) break;
		}
		
		AnnotatedSourceCodeFileSetBase *annotated_source_code_file_set = annotated_source_code_file_sets[i];
		
		annotated_source_code_file_set->Output(visitor);
	}
	
	Unlock();
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Output()
{
	if((enable_text_report || enable_csv_report || enable_html_report) && output_directory.empty())
	{
		logger << DebugWarning << "No output directory was specified in \"" << param_output_directory.GetName() << "\"" << EndDebugWarning;
		return;
	}
	
	if(enable_text_report)
	{
		FileVisitor file_visitor(output_directory, R_FMT_TEXT, csv_delimiter, csv_hyperlink, csv_arg_separator, logger.DebugErrorStream());
		Output(file_visitor);
	}
	
	if(enable_csv_report)
	{
		FileVisitor file_visitor(output_directory, R_FMT_CSV, csv_delimiter, csv_hyperlink, csv_arg_separator, logger.DebugErrorStream());
		Output(file_visitor);
	}

	if(enable_html_report)
	{
		FileVisitor file_visitor(output_directory, R_FMT_HTML, csv_delimiter, csv_hyperlink, csv_arg_separator, logger.DebugErrorStream());
		Output(file_visitor);
	}
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Lock()
{
	pthread_mutex_lock(&mutex);
}

template <typename ADDRESS>
void Profiler<ADDRESS>::Unlock()
{
	pthread_mutex_unlock(&mutex);
}

template <typename ADDRESS>
Profiler<ADDRESS>::PropertySetter::PropertySetter(Profiler<ADDRESS>& _profiler)
	: profiler(_profiler)
	, clear_all(false)
	, clear()
{
}

template <typename ADDRESS>
bool Profiler<ADDRESS>::PropertySetter::FormAssign(const std::string& name, const std::string& value)
{
	if((name == "clear-all") && (value == "on"))
	{
		clear_all = true;
	}
	else
	{
		std::size_t delim_pos = name.find_first_of('*');
		if(delim_pos != std::string::npos)
		{
			std::string action = name.substr(0, delim_pos);
			std::string variable_name = name.substr(delim_pos + 1);
			
			if((action == "clear") && (value == "on"))
			{
				clear.push_back(variable_name);
			}
		}
		else
		{
		}
	}
	
	return true;
}
	
template <typename ADDRESS>
void Profiler<ADDRESS>::PropertySetter::Apply()
{
	if(clear_all)
	{
		profiler.Clear();
	}
	else
	{
		for(std::vector<std::string>::iterator it = clear.begin(); it != clear.end(); ++it)
		{
			const std::string& variable_name = *it;
			profiler.Clear(variable_name);
		}
	}
}

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
