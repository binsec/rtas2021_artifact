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
 
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <io.h>     // for function access()
#else
#include <unistd.h>
#endif

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

InlineDebuggerBase::InlineDebuggerBase(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent)
	, search_path()
	, param_search_path("search-path", this, search_path, "Search path for source (separated by ';')")
{
}

InlineDebuggerBase::~InlineDebuggerBase()
{
}

void InlineDebuggerBase::SigInt()
{
	Interrupt();
}

std::string InlineDebuggerBase::SearchFile(const char *filename)
{
	if(access(filename, F_OK) == 0)
	{
		return std::string(filename);
	}

	std::string s;
	const char *p;

	p = search_path.c_str();
	do
	{
		if(*p == 0 || *p == ';')
		{
			std::stringstream sstr;
			sstr << s << "/" << filename;
			std::string path(sstr.str());
			if(access(path.c_str(), F_OK) == 0)
			{
				return path;
			}
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	
	return unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(filename);
}

bool InlineDebuggerBase::LocateFile(const char *filename, std::string& match_file_path)
{
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

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
