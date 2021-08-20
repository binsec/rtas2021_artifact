/*
 *  Copyright (c) 2010,
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

#include <unisim/util/debug/dwarf/filename.hh>
#include <unisim/util/debug/dwarf/ml.hh>
#include <string.h>
#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

DWARF_Filename::DWARF_Filename(unsigned int _id)
	: id(_id)
	, filename()
	, directory_index()
	, last_modification_time()
	, byte_length()
{
}

DWARF_Filename::DWARF_Filename(const DWARF_Filename& dw_filename)
	: id(dw_filename.id)
	, filename(dw_filename.filename)
	, directory_index(dw_filename.directory_index)
	, last_modification_time(dw_filename.last_modification_time)
	, byte_length(dw_filename.byte_length)
{
}

DWARF_Filename::~DWARF_Filename()
{
}

const char *DWARF_Filename::GetFilename() const
{
	return filename;
}

const DWARF_LEB128& DWARF_Filename::GetDirectoryIndex() const
{
	return directory_index;
}

const DWARF_LEB128& DWARF_Filename::GetLastModificationTime() const
{
	return last_modification_time;
}

const DWARF_LEB128& DWARF_Filename::GetByteLength() const
{
	return byte_length;
}

DWARF_Filename& DWARF_Filename::operator = (const DWARF_Filename& dw_filename)
{
	filename = dw_filename.filename;
	directory_index = dw_filename.directory_index;
	last_modification_time = dw_filename.last_modification_time;
	byte_length = dw_filename.byte_length;
	return *this;
}

int64_t DWARF_Filename::Load(const uint8_t *rawdata, uint64_t max_size)
{
	int64_t size = 0;
	int64_t sz;

	if(!max_size) return -1;
	filename = (const char *) rawdata;
	sz = strlen(filename) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	if((sz = directory_index.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	if((sz = last_modification_time.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if((sz = byte_length.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	return size;
}

std::ostream& DWARF_Filename::to_XML(std::ostream& os) const
{
	os << "<DW_FILENAME filename=\"";
	c_string_to_XML(os, filename);
	os << "\" directory_index=\"" << directory_index.to_string(false) << "\"";
	os << " time=\"" << last_modification_time.to_string(false) << "\"";
	os << " size=\"" << byte_length.to_string(false) << "\"/>";
	return os;
}

std::ostream& DWARF_Filename::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << id << "</td>";
	os << "<td>";
	c_string_to_HTML(os, filename);
	os << "</td>";
	os << "<td>" <<directory_index.to_string(false) << "</td>";
	os << "<td>" << last_modification_time.to_string(false) << "</td>";
	os << "<td>" << byte_length.to_string(false) << "</td>";
	os << "</tr>" << std::endl;
	return os;
}

std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename)
{
	return os << "File: \"" << dw_filename.filename << "\"  Dir: " << dw_filename.directory_index.to_string(false) << "  Time: " << dw_filename.last_modification_time.to_string(false) << "  Size: " << dw_filename.byte_length.to_string(false);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
