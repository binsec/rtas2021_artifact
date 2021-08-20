/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_REGISTER_NUMBER_MAPPING_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_REGISTER_NUMBER_MAPPING_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/xml/xml.hh>
#include <iosfwd>
#include <set>
#include <map>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

std::ostream& operator << (std::ostream& os, const DWARF_RegisterNumberMapping& dw_reg_num_mapping);

class DWARF_RegisterNumberMapping
{
public:
	DWARF_RegisterNumberMapping(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream, unisim::service::interfaces::Registers *regs_if);
	~DWARF_RegisterNumberMapping();
	
	bool Load(const char *filename, const char *architecture);
	void Map(unsigned int dw_reg_num, unisim::service::interfaces::Register *arch_reg);
	unisim::service::interfaces::Register *GetArchReg(unsigned int dw_reg_num);
	const unisim::service::interfaces::Register *GetArchReg(unsigned int dw_reg_num) const;
	void EnumRegisterNumbers(std::set<unsigned int>& reg_num_set) const;
	unsigned int GetSPRegNum() const;
	friend std::ostream& operator << (std::ostream& os, const DWARF_RegisterNumberMapping& dw_reg_num_mapping);
private:
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	unisim::service::interfaces::Registers *regs_if;
	std::map<unsigned int, unisim::service::interfaces::Register *> reg_num_mapping;
	unsigned int sp_reg_num;
	
	bool Load(unisim::util::xml::Node *xml_node);
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
