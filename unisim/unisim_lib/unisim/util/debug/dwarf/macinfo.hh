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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_MACINFO_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_MACINFO_HH__

#include <unisim/util/debug/dwarf/fwd.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_MacInfoListEntry<MEMORY_ADDR>& dw_macinfo_list_entry);

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntry
{
public:
	DWARF_MacInfoListEntry(uint8_t dw_mac_info_type);
	virtual ~DWARF_MacInfoListEntry();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset) = 0;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	uint8_t GetType() const;
	uint64_t GetOffset() const;
	std::string GetHREF() const;
	unsigned int GetId() const;
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *GetNext() const;
	virtual std::string to_string() const = 0;
	virtual std::ostream& to_XML(std::ostream& os) const = 0;
	virtual std::ostream& to_HTML(std::ostream& os) const = 0;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_MacInfoListEntry<MEMORY_ADDR>& dw_macinfo_list_entry);
protected:
	friend class DWARF_Handler<MEMORY_ADDR>;
	
	DWARF_MacInfoListEntry<MEMORY_ADDR> *next;
	uint64_t offset; // offset within .debug_macinfo section_name
	unsigned int id;
	uint8_t dw_mac_info_type;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryDefine : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryDefine();
	virtual ~DWARF_MacInfoListEntryDefine();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetLineNo() const;
	const char *GetPreprocessorSymbolName() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
	virtual std::ostream& to_HTML(std::ostream& os) const;
private:
	DWARF_LEB128 lineno;
	const char *preprocessor_symbol_name;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryUndef : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryUndef();
	virtual ~DWARF_MacInfoListEntryUndef();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetLineNo() const;
	const char *GetPreprocessorSymbolName() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
	virtual std::ostream& to_HTML(std::ostream& os) const;
private:
	DWARF_LEB128 lineno;
	const char *preprocessor_symbol_name;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryStartFile : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryStartFile();
	virtual ~DWARF_MacInfoListEntryStartFile();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetLineNo() const;
	const DWARF_LEB128& GetFileIndex() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
	virtual std::ostream& to_HTML(std::ostream& os) const;
private:
	DWARF_LEB128 lineno;
	DWARF_LEB128 file_idx;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryEndFile : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryEndFile();
	virtual ~DWARF_MacInfoListEntryEndFile();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
	virtual std::ostream& to_HTML(std::ostream& os) const;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryVendorExtension : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryVendorExtension();
	virtual ~DWARF_MacInfoListEntryVendorExtension();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetConstant() const;
	const char *GetString() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
	virtual std::ostream& to_HTML(std::ostream& os) const;
private:
	DWARF_LEB128 vendor_ext_constant;
	const char *vendor_ext_string;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryNull : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryNull();
	virtual ~DWARF_MacInfoListEntryNull();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
	virtual std::ostream& to_HTML(std::ostream& os) const;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
