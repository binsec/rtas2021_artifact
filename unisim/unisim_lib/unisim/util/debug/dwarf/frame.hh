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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FRAME_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_FRAME_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegSet<MEMORY_ADDR>& dw_reg_set);

template <class MEMORY_ADDR>
class DWARF_RegSet
{
public:
	DWARF_RegSet();
	void UndefRegister(unsigned int reg_num);
	bool ReadRegister(unsigned int reg_num, MEMORY_ADDR& reg_value) const;
	void WriteRegister(unsigned int reg_num, MEMORY_ADDR value);
private:
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_RegSet<MEMORY_ADDR>& dw_reg_set);
	
	std::map<unsigned int, MEMORY_ADDR> reg_set;
};

template <class MEMORY_ADDR>
class DWARF_Frame
{
public:
	DWARF_Frame(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num);
	DWARF_Frame(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num, MEMORY_ADDR pc);
	~DWARF_Frame();
	
	bool LoadArchRegs();
	bool ComputeCFA(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame);
	bool Unwind(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame, unsigned int dw_ret_addr_reg_num);
	MEMORY_ADDR ReadCFA() const;
// 	void UndefRegister(unsigned int reg_num);
	bool ReadRegister(unsigned int reg_num, MEMORY_ADDR& reg_value) const;
// 	void WriteRegister(unsigned int reg_num, const MEMORY_ADDR value);
	bool ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr, unsigned int read_size) const;
	bool GetPC(MEMORY_ADDR& pc_value) const;
	const DWARF_RegSet<MEMORY_ADDR>& GetRegSet() const;
	unsigned int GetProcessorNumber() const;
private:
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	unsigned int prc_num;
	unsigned int sp_reg_num;
	unsigned int dw_ret_addr_reg_num;
	unisim::util::endian::endian_type endianness;
	unsigned int address_size;
	DW_CFA_Specification dw_cfa_spec;
	DW_CFA_RegRuleOffsetSpecification dw_cfa_reg_rule_offset_spec;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	bool pc_is_defined;
	MEMORY_ADDR pc;
	MEMORY_ADDR cfa;
	std::set<unsigned int> reg_num_set;
	//std::map<unsigned int, MEMORY_ADDR> reg_set;
	DWARF_RegSet<MEMORY_ADDR> dw_reg_set;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
