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

#include <unisim/service/debug/profiler/profiler.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access() and mkdir()
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

template class Sample<signed char>;
template class Sample<short>;
template class Sample<int>;
template class Sample<long>;
template class Sample<long long>;
template class Sample<unsigned char>;
template class Sample<unsigned short>;
template class Sample<unsigned int>;
template class Sample<unsigned long>;
template class Sample<unsigned long long>;
template class Sample<double>;
template class Sample<sc_core::sc_time>;

////////////////////////////// FileFormat /////////////////////////////////////

std::ostream& operator << (std::ostream& os, FileFormat f_fmt)
{
	switch(f_fmt)
	{
		case F_FMT_TEXT: os << "text"; break;
		case F_FMT_HTML: os << "html"; break;
		case F_FMT_CSV: os << "csv"; break;
		case F_FMT_SVG: os << "svg"; break;
		default: os << "?"; break;
	}
	
	return os;
}

std::string FileFormatSuffix(FileFormat f_fmt)
{
	switch(f_fmt)
	{
		case F_FMT_TEXT: return std::string(".txt");
		case F_FMT_HTML: return std::string(".html");
		case F_FMT_CSV: return std::string(".csv");
		case F_FMT_SVG: return std::string(".svg");
		default: return std::string();
	}
	
	return std::string();
}

///////////////////////////// c_string_to_CSV /////////////////////////////////

std::string c_string_to_CSV(const char *s)
{
	std::stringstream sstr;
	
	sstr << "\"";
	
	char c = *s;

	if(c)
	{
		do
		{
			switch(c)
			{
				case '"':
					sstr << "\"\"";
					break;
				default:
					sstr << c;
			}
		}
		while((c = *(++s)));
	}
	
	sstr << "\"";
	
	return sstr.str();
}

//////////////////////////////// MakeDir //////////////////////////////////////

void MakeDir(const char *dirname)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	mkdir(dirname);
#else
	mkdir(dirname, S_IRWXU);
#endif
}

/////////////////////////////// RealPath //////////////////////////////////////

std::string RealPath(const char *dirname)
{
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	char dir_path[FILENAME_MAX + 1];
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	char dir_path[PATH_MAX + 1];
#endif

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	char *dir_path_pointer = realpath(dirname, dir_path);
	if(dir_path_pointer == dir_path)
	{
		return std::string(dir_path);
	}
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(GetFullPathName(dirname, sizeof(dir_path), dir_path, NULL))
	{
		return std::string(dir_path);
	}
#endif
	return std::string(dirname);
}

//////////////////////////// FilenameIndex ////////////////////////////////////

unsigned int FilenameIndex::IndexFilename(const std::string& filename)
{
	typename std::map<std::string, unsigned int>::const_iterator it = index.find(filename);
	
	if(it != index.end())
	{
		return (*it).second;
	}
	
	unsigned int filename_id = index.size();
	index.insert(std::pair<std::string, unsigned int>(filename, filename_id));
	return filename_id;
}

/////////////////////////////// FileVisitor ////////////////////////////////////

FileVisitor::FileVisitor(const std::string& _output_directory, ReportFormat _r_fmt, const std::string& _csv_delimiter, const std::string& _csv_hyperlink, const std::string& _csv_arg_separator, std::ostream& _warn_log)
	: file()
	, output_directory(_output_directory)
	, r_fmt(_r_fmt)
	, csv_delimiter(_csv_delimiter)
	, csv_hyperlink(_csv_hyperlink)
	, csv_arg_separator(_csv_arg_separator)
	, warn_log(_warn_log)
	, dir_path(output_directory)
{
	MakeDir(output_directory.c_str());
	output_directory = RealPath(output_directory.c_str());
}

const std::string& FileVisitor::GetCSVDelimiter() const
{
	return csv_delimiter;
}

const std::string& FileVisitor::GetCSVHyperlink() const
{
	return csv_hyperlink;
}

const std::string& FileVisitor::GetCSVArgSeparator() const
{
	return csv_arg_separator;
}

const std::string& FileVisitor::GetRoot() const
{
	static std::string null_str;
	return null_str;
}

const std::string& FileVisitor::GetDomain() const
{
	static std::string null_str;
	return null_str;
}

ReportFormat FileVisitor::GetReportFormat() const
{
	return r_fmt;
}

std::string FileVisitor::GetFilePath(const std::string& filename) const
{
	return unisim::kernel::Simulator::Instance()->SearchSharedDataFile(filename.c_str());
}

const std::string& FileVisitor::GetDirPath() const
{
	return dir_path;
}

double FileVisitor::GetRefreshPeriod() const
{
	return 0.0;
}

bool FileVisitor::Visit(const std::string& dirname, const std::string& filename, const Printer& printer)
{
	dir_path = output_directory;
	if(!dirname.empty())
	{
		dir_path += '/';
		dir_path += dirname;
	}
	MakeDir(dir_path.c_str());
	std::string file_path(output_directory);
	if(!dirname.empty())
	{
		file_path += '/';
		file_path += dirname;
	}
	file_path += '/';
	file_path += filename;
	
	file.open(file_path.c_str(), std::ios::out);
	
	if(file.fail())
	{
		warn_log << "Can't create \"" << file_path << "\"" << std::endl;
		return false;
	}
	
	printer.Print(file, *this);
	
	file.close();
	
	return true;
}

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
