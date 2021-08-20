/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__

#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace crossbar {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
AddressRange<CONFIG>::AddressRange(typename CONFIG::ADDRESS _start_addr, typename CONFIG::ADDRESS _end_addr)
	: start_addr(_start_addr)
	, end_addr(_end_addr)
{
}

template <class CONFIG>
AddressRange<CONFIG>::~AddressRange()
{
}

template <class CONFIG>
typename CONFIG::ADDRESS AddressRange<CONFIG>::GetStartAddr() const
{
	return start_addr;
}

template <class CONFIG>
typename CONFIG::ADDRESS AddressRange<CONFIG>::GetEndAddr() const
{
	return end_addr;
}

template <class CONFIG>
void AddressRange<CONFIG>::SetStartAddr(typename CONFIG::ADDRESS _start_addr)
{
	start_addr = _start_addr;
}

template <class CONFIG>
void AddressRange<CONFIG>::SetEndAddr(typename CONFIG::ADDRESS _end_addr)
{
	end_addr = _end_addr;
}

template <class CONFIG>
std::ostream& operator << (std::ostream& os, const AddressRange<CONFIG>& addr_range)
{
	return os << "0x" << std::hex << addr_range.start_addr << "-0x" << addr_range.end_addr << std::dec;
}

template <class CONFIG>
Crossbar<CONFIG>::Crossbar(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, Client<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, memory_export("memory-export", this)
	, mci_memory_import("mci-memory-import", this)
	, mplb_memory_import("mplb-memory-import", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	memory_export.SetupDependsOn(mci_memory_import);
	memory_export.SetupDependsOn(mplb_memory_import);
	
	Reset();
	
	std::stringstream sstr_description;
	sstr_description << "This module implements the crossbar of the embedded processor block in Virtex-5 FXT FPGAs from Xilinx. It has the following address mappings at reset:" << std::endl;
	DumpAddressMapping(IF_ICURD_PLB, sstr_description);
	DumpAddressMapping(IF_DCUWR_PLB, sstr_description);
	DumpAddressMapping(IF_DCURD_PLB, sstr_description);
	DumpAddressMapping(IF_SPLB0, sstr_description);
	DumpAddressMapping(IF_SPLB1, sstr_description);
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
Crossbar<CONFIG>::~Crossbar()
{
}

template <class CONFIG>
void Crossbar<CONFIG>::Reset()
{
	ist = CONFIG::IST_RESET_VALUE;
	imask = CONFIG::IMASK_RESET_VALUE;
	arb_xbc = CONFIG::ARB_XBC_RESET_VALUE;
	fifost_xbc = CONFIG::FIFOST_XBC_RESET_VALUE;
	sm_st_xbc = CONFIG::SM_ST_XBC_RESET_VALUE;
	misc_xbc = CONFIG::MISC_XBC_RESET_VALUE;
	arb_xbm = CONFIG::ARB_XBM_RESET_VALUE;
	fifost_xbm = CONFIG::FIFOST_XBM_RESET_VALUE;
	misc_xbm = CONFIG::MISC_XBM_RESET_VALUE;
	tmpl_xbar_map[0] = CONFIG::TMPL0_XBAR_MAP_RESET_VALUE;
	tmpl_xbar_map[1] = CONFIG::TMPL1_XBAR_MAP_RESET_VALUE;
	tmpl_xbar_map[2] = CONFIG::TMPL2_XBAR_MAP_RESET_VALUE;
	tmpl_xbar_map[3] = CONFIG::TMPL3_XBAR_MAP_RESET_VALUE;
	tmpl_sel_reg = CONFIG::TMPL_SEL_REG_RESET_VALUE;
	cfg_plbs0 = CONFIG::CFG_PLBS0_RESET_VALUE;
	sear_u_plbs0 = CONFIG::SEAR_U_PLBS0_RESET_VALUE;
	sear_l_plbs0 = CONFIG::SEAR_L_PLBS0_RESET_VALUE;
	sesr_plbs0 = CONFIG::SESR_PLBS0_RESET_VALUE;
	misc_st_plbs0 = CONFIG::MISC_ST_PLBS0_RESET_VALUE;
	plberr_st_plbs0 = CONFIG::PLBERR_ST_PLBS0_RESET_VALUE;
	sm_st_plbs0 = CONFIG::SM_ST_PLBS0_RESET_VALUE;
	misc_plbs0 = CONFIG::MISC_PLBS0_RESET_VALUE;
	cmd_sniff_plbs0 = CONFIG::CMD_SNIFF_PLBS0_RESET_VALUE;
	cmd_sniffa_plbs0 = CONFIG::CMD_SNIFFA_PLBS0_RESET_VALUE;
	tmpl_plbs0_map[0] = CONFIG::TMPL0_PLBS0_MAP_RESET_VALUE;
	tmpl_plbs0_map[1] = CONFIG::TMPL1_PLBS0_MAP_RESET_VALUE;
	tmpl_plbs0_map[2] = CONFIG::TMPL2_PLBS0_MAP_RESET_VALUE;
	tmpl_plbs0_map[3] = CONFIG::TMPL3_PLBS0_MAP_RESET_VALUE;
	cfg_plbs1 = CONFIG::CFG_PLBS1_RESET_VALUE;
	sear_u_plbs1 = CONFIG::SEAR_U_PLBS1_RESET_VALUE;
	sear_l_plbs1 = CONFIG::SEAR_L_PLBS1_RESET_VALUE;
	sesr_plbs1 = CONFIG::SESR_PLBS1_RESET_VALUE;
	misc_st_plbs1 = CONFIG::MISC_ST_PLBS1_RESET_VALUE;
	plberr_st_plbs1 = CONFIG::PLBERR_ST_PLBS1_RESET_VALUE;
	sm_st_plbs1 = CONFIG::SM_ST_PLBS1_RESET_VALUE;
	misc_plbs1 = CONFIG::MISC_PLBS1_RESET_VALUE;
	cmd_sniff_plbs1 = CONFIG::CMD_SNIFF_PLBS1_RESET_VALUE;
	cmd_sniffa_plbs1 = CONFIG::CMD_SNIFFA_PLBS1_RESET_VALUE;
	tmpl_plbs1_map[0] = CONFIG::TMPL0_PLBS1_MAP_RESET_VALUE;
	tmpl_plbs1_map[1] = CONFIG::TMPL1_PLBS1_MAP_RESET_VALUE;
	tmpl_plbs1_map[2] = CONFIG::TMPL2_PLBS1_MAP_RESET_VALUE;
	tmpl_plbs1_map[3] = CONFIG::TMPL3_PLBS1_MAP_RESET_VALUE;
	cfg_plbm = CONFIG::CFG_PLBM_RESET_VALUE;
	fsear_u_plbm = CONFIG::FSEAR_U_PLBM_RESET_VALUE;
	fsear_l_plbm = CONFIG::FSEAR_U_PLBM_RESET_VALUE;
	fsesr_plbm = CONFIG::FSESR_PLBM_RESET_VALUE;
	misc_st_plbm = CONFIG::MISC_ST_PLBM_RESET_VALUE;
	plberr_st_plbm = CONFIG::PLBERR_ST_PLBM_RESET_VALUE;
	sm_st_plbm = CONFIG::SM_ST_PLBM_RESET_VALUE;
	misc_plbm = CONFIG::MISC_PLBM_RESET_VALUE;
	cmd_sniff_plbm = CONFIG::CMD_SNIFF_PLBM_RESET_VALUE;
	cmd_sniffa_plbm = CONFIG::CMD_SNIFFA_PLBM_RESET_VALUE;
}

template <class CONFIG>
void Crossbar<CONFIG>::ResetMemory()
{
	Reset();
}

template <class CONFIG>
bool Crossbar<CONFIG>::ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size)
{
	typename CONFIG::ADDRESS start_range = 0;
	typename CONFIG::ADDRESS end_range = 0;
	Interface intf = Route(IF_DCURD_PLB, addr, start_range, end_range);
	
	switch(intf)
	{
		case IF_MCI:
			return mci_memory_import->ReadMemory(addr, buffer, size);
			break;
		case IF_MPLB:
			return mplb_memory_import->ReadMemory(addr, buffer, size);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
	return false;
}

template <class CONFIG>
bool Crossbar<CONFIG>::WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size)
{
	typename CONFIG::ADDRESS start_range = 0;
	typename CONFIG::ADDRESS end_range = 0;
	Interface intf = Route(IF_DCUWR_PLB, addr, start_range, end_range);
	
	switch(intf)
	{
		case IF_MCI:
			return mci_memory_import->WriteMemory(addr, buffer, size);
			break;
		case IF_MPLB:
			return mplb_memory_import->WriteMemory(addr, buffer, size);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
	}
	return false;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::ReadDCR(unsigned int dcrn)
{
	uint32_t value;

	switch(dcrn)
	{
		case CONFIG::IST: value = GetIST(); break;
		case CONFIG::IMASK: value = GetIMASK(); break;
		case CONFIG::ARB_XBC: value = GetARB_XBC(); break;
		case CONFIG::FIFOST_XBC: value = GetFIFOST_XBC(); break;
		case CONFIG::SM_ST_XBC: value = GetSM_ST_XBC(); break;
		case CONFIG::MISC_XBC: value = GetMISC_XBC(); break;
		case CONFIG::ARB_XBM: value = GetARB_XBM(); break;
		case CONFIG::FIFOST_XBM: value = GetFIFOST_XBM(); break;
		case CONFIG::MISC_XBM: value = GetMISC_XBM(); break;
		case CONFIG::TMPL0_XBAR_MAP:
		case CONFIG::TMPL1_XBAR_MAP:
		case CONFIG::TMPL2_XBAR_MAP:
		case CONFIG::TMPL3_XBAR_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_XBAR_MAP;
				value = GetTMPL_XBAR_MAP(reg_num);
			}
			break;
		case CONFIG::TMPL_SEL_REG: value = GetTMPL_SEL_REG(); break;
		case CONFIG::CFG_PLBS0: value = GetCFG_PLBS0(); break;
		case CONFIG::SEAR_U_PLBS0: value = GetSEAR_U_PLBS0(); break;
		case CONFIG::SEAR_L_PLBS0: value = GetSEAR_L_PLBS0(); break;
		case CONFIG::SESR_PLBS0: value = GetSESR_PLBS0(); break;
		case CONFIG::MISC_ST_PLBS0: value = GetMISC_ST_PLBS0(); break;
		case CONFIG::PLBERR_ST_PLBS0: value = GetPLBERR_ST_PLBS0(); break;
		case CONFIG::SM_ST_PLBS0: value = GetSM_ST_PLBS0(); break;
		case CONFIG::MISC_PLBS0: value = GetMISC_PLBS0(); break;
		case CONFIG::CMD_SNIFF_PLBS0: value = GetCMD_SNIFF_PLBS0(); break;
		case CONFIG::CMD_SNIFFA_PLBS0: value = GetCMD_SNIFFA_PLBS0(); break;
		case CONFIG::TMPL0_PLBS0_MAP:
		case CONFIG::TMPL1_PLBS0_MAP:
		case CONFIG::TMPL2_PLBS0_MAP:
		case CONFIG::TMPL3_PLBS0_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS0_MAP;
				value = GetTMPL_PLBS0_MAP(reg_num);
			}
			break;
		case CONFIG::CFG_PLBS1: value = GetCFG_PLBS1(); break;
		case CONFIG::SEAR_U_PLBS1: value = GetSEAR_U_PLBS1(); break;
		case CONFIG::SEAR_L_PLBS1: value = GetSEAR_L_PLBS1(); break;
		case CONFIG::SESR_PLBS1: value = GetSESR_PLBS1(); break;
		case CONFIG::MISC_ST_PLBS1: value = GetMISC_ST_PLBS1(); break;
		case CONFIG::PLBERR_ST_PLBS1: value = GetPLBERR_ST_PLBS1(); break;
		case CONFIG::SM_ST_PLBS1: value = GetSM_ST_PLBS1(); break;
		case CONFIG::MISC_PLBS1: value = GetMISC_PLBS1(); break;
		case CONFIG::CMD_SNIFF_PLBS1: value = GetCMD_SNIFF_PLBS1(); break;
		case CONFIG::CMD_SNIFFA_PLBS1: value = GetCMD_SNIFFA_PLBS1(); break;
		case CONFIG::TMPL0_PLBS1_MAP:
		case CONFIG::TMPL1_PLBS1_MAP:
		case CONFIG::TMPL2_PLBS1_MAP:
		case CONFIG::TMPL3_PLBS1_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS1_MAP;
				value = GetTMPL_PLBS1_MAP(reg_num);
			}
			break;
		case CONFIG::CFG_PLBM: value = GetCFG_PLBM(); break;
		case CONFIG::FSEAR_U_PLBM: value = GetFSEAR_U_PLBM(); break;
		case CONFIG::FSEAR_L_PLBM: value = GetFSEAR_L_PLBM(); break;
		case CONFIG::FSESR_PLBM: value = GetFSESR_PLBM(); break;
		case CONFIG::MISC_ST_PLBM: value = GetMISC_ST_PLBM(); break;
		case CONFIG::PLBERR_ST_PLBM: value = GetPLBERR_ST_PLBM(); break;
		case CONFIG::SM_ST_PLBM: value = GetSM_ST_PLBM(); break;
		case CONFIG::MISC_PLBM: value = GetMISC_PLBM(); break;
		case CONFIG::CMD_SNIFF_PLBM: value = GetCMD_SNIFF_PLBM(); break;
		case CONFIG::CMD_SNIFFA_PLBM: value = GetCMD_SNIFFA_PLBM(); break;
		default:
			logger << DebugWarning << "Reading an unmapped DCR #0x" << std::hex << dcrn << std::dec << EndDebugWarning;
			return 0;
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Read 0x" << std::hex << value << std::dec << " from DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
	return value;
}

template <class CONFIG>
void Crossbar<CONFIG>::WriteDCR(unsigned int dcrn, uint32_t value)
{
	if(IsVerbose())
	{
		logger << DebugInfo << "Writing 0x" << std::hex << value << std::dec << " into DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
	switch(dcrn)
	{
		case CONFIG::IST: SetIST(value); break;
		case CONFIG::IMASK: SetIMASK(value); break;
		case CONFIG::ARB_XBC: SetARB_XBC(value); break;
		case CONFIG::FIFOST_XBC: SetFIFOST_XBC(value); break;
		case CONFIG::MISC_XBC: SetMISC_XBC(value); break;
		case CONFIG::ARB_XBM: SetARB_XBM(value); break;
		case CONFIG::FIFOST_XBM: SetFIFOST_XBM(value); break;
		case CONFIG::MISC_XBM: SetMISC_XBM(value); break;
		case CONFIG::TMPL0_XBAR_MAP:
		case CONFIG::TMPL1_XBAR_MAP:
		case CONFIG::TMPL2_XBAR_MAP:
		case CONFIG::TMPL3_XBAR_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_XBAR_MAP;
				SetTMPL_XBAR_MAP(reg_num, value);
			}
			break;
		case CONFIG::TMPL_SEL_REG: SetTMPL_SEL_REG(value); break;
		case CONFIG::CFG_PLBS0: SetCFG_PLBS0(value); break;
		case CONFIG::SESR_PLBS0: SetSESR_PLBS0(value); break;
		case CONFIG::MISC_ST_PLBS0: SetMISC_ST_PLBS0(value); break;
		case CONFIG::PLBERR_ST_PLBS0: SetPLBERR_ST_PLBS0(value); break;
		case CONFIG::MISC_PLBS0: SetMISC_PLBS0(value); break;
		case CONFIG::CMD_SNIFF_PLBS0: SetCMD_SNIFF_PLBS0(value); break;
		case CONFIG::CMD_SNIFFA_PLBS0: SetCMD_SNIFFA_PLBS0(value); break;
		case CONFIG::TMPL0_PLBS0_MAP:
		case CONFIG::TMPL1_PLBS0_MAP:
		case CONFIG::TMPL2_PLBS0_MAP:
		case CONFIG::TMPL3_PLBS0_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS0_MAP;
				SetTMPL_PLBS0_MAP(reg_num, value);
			}
			break;
		case CONFIG::CFG_PLBS1: SetCFG_PLBS1(value); break;
		case CONFIG::SESR_PLBS1: SetSESR_PLBS1(value); break;
		case CONFIG::MISC_ST_PLBS1: SetMISC_ST_PLBS1(value); break;
		case CONFIG::PLBERR_ST_PLBS1: SetPLBERR_ST_PLBS1(value); break;
		case CONFIG::MISC_PLBS1: SetMISC_PLBS1(value); break;
		case CONFIG::CMD_SNIFF_PLBS1: SetCMD_SNIFF_PLBS1(value); break;
		case CONFIG::CMD_SNIFFA_PLBS1: SetCMD_SNIFFA_PLBS1(value); break;
		case CONFIG::TMPL0_PLBS1_MAP:
		case CONFIG::TMPL1_PLBS1_MAP:
		case CONFIG::TMPL2_PLBS1_MAP:
		case CONFIG::TMPL3_PLBS1_MAP:
			{
				unsigned int reg_num = dcrn - CONFIG::TMPL0_PLBS1_MAP;
				SetTMPL_PLBS1_MAP(reg_num, value);
			}
			break;
		case CONFIG::CFG_PLBM: SetCFG_PLBM(value); break;
		case CONFIG::FSEAR_U_PLBM: SetFSEAR_U_PLBM(value); break;
		case CONFIG::FSEAR_L_PLBM: SetFSEAR_L_PLBM(value); break;
		case CONFIG::FSESR_PLBM: SetFSESR_PLBM(value); break;
		case CONFIG::PLBERR_ST_PLBM: SetPLBERR_ST_PLBM(value); break;
		case CONFIG::SM_ST_PLBM: SetSM_ST_PLBM(value); break;
		case CONFIG::MISC_PLBM: SetMISC_PLBM(value); break;
		case CONFIG::CMD_SNIFF_PLBM: SetCMD_SNIFF_PLBM(value); break;
		case CONFIG::CMD_SNIFFA_PLBM: SetCMD_SNIFFA_PLBM(value); break;
		default:
			logger << DebugWarning << "Writing an unmapped or read only DCR #0x" << std::hex << dcrn << std::dec << EndDebugWarning;
	}
}

template <class CONFIG>
bool Crossbar<CONFIG>::IsMappedDCR(uint32_t dcrn) const
{
	return (dcrn >= CONFIG::DCR_LOW) && (dcrn <= CONFIG::DCR_HIGH);
}

template <class CONFIG>
typename Crossbar<CONFIG>::Interface Crossbar<CONFIG>::Route(Interface intf, typename CONFIG::ADDRESS addr, typename CONFIG::ADDRESS& start_range, typename CONFIG::ADDRESS& end_range)
{
	// addr = UAbus[28:31] || ABus[0:4] || ABus[5:31]
	unsigned int uabus_28_31 = (addr >> 32) & 0xf;
	unsigned int tmpl_reg_num = (GetTMPL_SEL_REG() >> (30 - (2 * uabus_28_31))) & 0x3;
	
	unsigned int abus_0_4 = (addr >> 27) & 0x1f;
	
	uint32_t map;
	switch(intf)
	{
		case IF_ICURD_PLB:
		case IF_DCUWR_PLB:
		case IF_DCURD_PLB:
			map = GetTMPL_XBAR_MAP(tmpl_reg_num);
			break;
		case IF_SPLB0:
			map = GetTMPL_PLBS0_MAP(tmpl_reg_num);
			break;
		case IF_SPLB1:
			map = GetTMPL_PLBS1_MAP(tmpl_reg_num);
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return IF_MPLB;
	}
	
	start_range = ((typename CONFIG::ADDRESS) uabus_28_31 << 32) | ((typename CONFIG::ADDRESS) abus_0_4 << 27);
	end_range = start_range | 0x7ffffffULL;
	
	return (map & (1 << (31 - abus_0_4))) ? IF_MCI : IF_MPLB;
}

template <class CONFIG>
void Crossbar<CONFIG>::DumpAddressMapping(typename CONFIG::ADDRESS start_addr, typename CONFIG::ADDRESS end_addr, Interface dst_if, std::ostream& os)
{
	os << "0x" << std::hex << start_addr << "-0x" << end_addr << std::dec << " -> ";
	switch(dst_if)
	{
		case IF_MCI:
			os << "MCI";
			break;
		case IF_MPLB:
			os << "MPLB";
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return;
	}
	os << std::dec << std::endl;
}

template <class CONFIG>
void Crossbar<CONFIG>::DumpAddressMapping(Interface intf, std::ostream& os)
{
	switch(intf)
	{
		case IF_ICURD_PLB:
			os << "ICURD";
			break;
		case IF_DCUWR_PLB:
			os << "DCUWR";
			break;
		case IF_DCURD_PLB:
			os << "DCURD";
			break;
		case IF_SPLB0:
			os << "SPLB0";
			break;
		case IF_SPLB1:
			os << "SPLB1";
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return;
	}
	os << " mapping:" << std::endl;
	
	unsigned int uabus_28_31;
	typename CONFIG::ADDRESS start_addr = 0;
	typename CONFIG::ADDRESS end_addr = 0;
	Interface dst_if = IF_MCI;
	for(uabus_28_31 = 0; uabus_28_31 < 16; uabus_28_31++)
	{
		unsigned int abus_0_4;
		for(abus_0_4 = 0; abus_0_4 < 32; abus_0_4++)
		{
			typename CONFIG::ADDRESS new_start_addr = ((typename CONFIG::ADDRESS) uabus_28_31 << 32) | ((typename CONFIG::ADDRESS) abus_0_4 << 27);
			typename CONFIG::ADDRESS new_end_addr = new_start_addr | 0x7ffffffULL;
			
			unsigned int tmpl_reg_num = (GetTMPL_SEL_REG() >> (30 - (2 * uabus_28_31))) & 0x3;

			uint32_t map;
			
			switch(intf)
			{
				case IF_ICURD_PLB:
				case IF_DCUWR_PLB:
				case IF_DCURD_PLB:
					map = GetTMPL_XBAR_MAP(tmpl_reg_num);
					break;
				case IF_SPLB0:
					map = GetTMPL_PLBS0_MAP(tmpl_reg_num);
					break;
				case IF_SPLB1:
					map = GetTMPL_PLBS1_MAP(tmpl_reg_num);
					break;
				default:
					logger << DebugError << "Internal error" << EndDebugError;
					Object::Stop(-1);
					return;
			}
			
			Interface new_dst_if = (map & (1 << (31 - abus_0_4))) ? IF_MCI : IF_MPLB;
			if((uabus_28_31 == 15) && (abus_0_4 == 31))
			{
				// last mapping
				os << "  - ";
				DumpAddressMapping(start_addr, new_end_addr, dst_if, os);
			}
			else if(((uabus_28_31 != 0) || (abus_0_4 != 0)) && (new_dst_if != dst_if))
			{
				// not the first mapping, and destination interface is different from previous mapping
				os << "  - ";
				DumpAddressMapping(start_addr, end_addr, dst_if, os);
				start_addr = new_start_addr;
			}
			end_addr = new_end_addr;
			dst_if = new_dst_if;
		}
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::GetAddressRanges(Interface src_if, Interface dst_if, std::list<AddressRange<CONFIG> >& lst)
{
	unsigned int uabus_28_31;
	for(uabus_28_31 = 0; uabus_28_31 < 16; uabus_28_31++)
	{
		unsigned int abus_0_4;
		for(abus_0_4 = 0; abus_0_4 < 32; abus_0_4++)
		{
			typename CONFIG::ADDRESS start_addr = ((typename CONFIG::ADDRESS) uabus_28_31 << 32) | ((typename CONFIG::ADDRESS) abus_0_4 << 27);
			typename CONFIG::ADDRESS end_addr = start_addr | 0x7ffffffULL;
			
			unsigned int tmpl_reg_num = (GetTMPL_SEL_REG() >> (30 - (2 * uabus_28_31))) & 0x3;

			uint32_t map;
			
			switch(src_if)
			{
				case IF_ICURD_PLB:
				case IF_DCUWR_PLB:
				case IF_DCURD_PLB:
					map = GetTMPL_XBAR_MAP(tmpl_reg_num);
					break;
				case IF_SPLB0:
					map = GetTMPL_PLBS0_MAP(tmpl_reg_num);
					break;
				case IF_SPLB1:
					map = GetTMPL_PLBS1_MAP(tmpl_reg_num);
					break;
				default:
					logger << DebugError << "Internal error" << EndDebugError;
					Object::Stop(-1);
					return;
			}
			
			if(dst_if == ((map & (1 << (31 - abus_0_4))) ? IF_MCI : IF_MPLB))
			{
				if(!lst.empty() && (lst.back().GetEndAddr() == (start_addr - 1)))
				{
					lst.back().SetEndAddr(end_addr);
				}
				else
				{
					lst.push_back(AddressRange<CONFIG>(start_addr, end_addr));
				}
			}
		}
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::MPLBError(Interface master_if, typename CONFIG::ADDRESS addr, bool rnw, unsigned int length)
{
	uint32_t mid = 0;
	uint32_t ssize = 2; // 128-bit slave size ?
	uint32_t type = 0; // memory access
	uint32_t size = (length <= CONFIG::PLB_WIDTH) ? 0 : 0x6;
	uint32_t be = 0xffff;
	
	switch(master_if)
	{
		case IF_ICURD_PLB:
			mid = 0;
			break;
		case IF_DCUWR_PLB:
			mid = 1;
			break;
		case IF_DCURD_PLB:
			mid = 2;
			break;
		case IF_SPLB0:
			mid = 3;
			break;
		case IF_SPLB1:
			mid = 4;
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return;
	}
	
	fsesr_plbm = fsesr_plbm | CONFIG::FSESR_PLBM_VLD_MASK;
	fsesr_plbm = fsesr_plbm & ~CONFIG::FSESR_PLBM_LOCKERR_MASK;
	fsesr_plbm = fsesr_plbm & ~CONFIG::FSESR_PLBM_PLBS_DMA_MASK;
	fsesr_plbm = (fsesr_plbm & ~CONFIG::FSESR_PLBM_MID_MASK) | ((mid << CONFIG::FSESR_PLBM_MID_OFFSET) & CONFIG::FSESR_PLBM_MID_MASK);
	fsesr_plbm = (fsesr_plbm & ~CONFIG::FSESR_PLBM_SSIZE_MASK) | ((ssize << CONFIG::FSESR_PLBM_SSIZE_OFFSET) & CONFIG::FSESR_PLBM_SSIZE_MASK);
	fsesr_plbm = (fsesr_plbm & ~CONFIG::FSESR_PLBM_TYPE_MASK) | ((type << CONFIG::FSESR_PLBM_TYPE_OFFSET) & CONFIG::FSESR_PLBM_TYPE_MASK);
	fsesr_plbm = (fsesr_plbm & ~CONFIG::FSESR_PLBM_RNW_MASK) | (rnw ? CONFIG::FSESR_PLBM_RNW_MASK : 0);
	fsesr_plbm = (fsesr_plbm & ~CONFIG::FSESR_PLBM_SIZE_MASK) | ((size << CONFIG::FSESR_PLBM_SIZE_OFFSET) & CONFIG::FSESR_PLBM_SIZE_MASK);
	fsesr_plbm = (fsesr_plbm & ~CONFIG::FSESR_PLBM_BE_MASK) | ((be << CONFIG::FSESR_PLBM_BE_OFFSET) & CONFIG::FSESR_PLBM_BE_MASK);
	
	fsear_u_plbm = (addr >> 4) & CONFIG::FSEAR_U_PLBM_MASK;
	fsear_l_plbm = addr & CONFIG::FSEAR_L_PLBM_MASK;
}

template <class CONFIG>
void Crossbar<CONFIG>::SPLBError(Interface master_if, typename CONFIG::ADDRESS addr, bool rnw, unsigned int length)
{
	uint32_t mid = 0; // ?
	uint32_t msize = 2; // 128-bit master size ?
	uint32_t type = 0; // memory access
	uint32_t size = (length <= CONFIG::PLB_WIDTH) ? 0 : 0x6;
	uint32_t be = 0xffff;
	
	switch(master_if)
	{
		case IF_SPLB0:
			sesr_plbs0 = sesr_plbs0 | CONFIG::SESR_PLBS0_VLD_MASK;
			sesr_plbs0 = sesr_plbs0 & ~CONFIG::SESR_PLBS0_LOCKERR_MASK;
			sesr_plbs0 = (sesr_plbs0 & ~CONFIG::SESR_PLBS0_MID_MASK) | ((mid << CONFIG::SESR_PLBS0_MID_OFFSET) & CONFIG::SESR_PLBS0_MID_MASK);
			sesr_plbs0 = (sesr_plbs0 & ~CONFIG::SESR_PLBS0_MSIZE_MASK) | ((msize << CONFIG::SESR_PLBS0_MSIZE_OFFSET) & CONFIG::SESR_PLBS0_MSIZE_MASK);
			sesr_plbs0 = (sesr_plbs0 & ~CONFIG::SESR_PLBS0_TYPE_MASK) | ((type << CONFIG::SESR_PLBS0_TYPE_OFFSET) & CONFIG::SESR_PLBS0_TYPE_MASK);
			sesr_plbs0 = (sesr_plbs0 & ~CONFIG::SESR_PLBS0_RNW_MASK) | (rnw ? CONFIG::SESR_PLBS0_RNW_MASK : 0);
			sesr_plbs0 = (sesr_plbs0 & ~CONFIG::SESR_PLBS0_SIZE_MASK) | ((size << CONFIG::SESR_PLBS0_SIZE_OFFSET) & CONFIG::SESR_PLBS0_SIZE_MASK);
			sesr_plbs0 = (sesr_plbs0 & ~CONFIG::SESR_PLBS0_BE_MASK) | ((be << CONFIG::SESR_PLBS0_BE_OFFSET) & CONFIG::SESR_PLBS0_BE_MASK);
			sear_u_plbs0 = (addr >> 4) & CONFIG::SEAR_U_PLBS0_MASK;
			sear_l_plbs0 = addr & CONFIG::SEAR_L_PLBS0_MASK;
			break;
		case IF_SPLB1:
			sesr_plbs1 = sesr_plbs1 | CONFIG::SESR_PLBS1_VLD_MASK;
			sesr_plbs1 = sesr_plbs1 & ~CONFIG::SESR_PLBS1_LOCKERR_MASK;
			sesr_plbs1 = (sesr_plbs1 & ~CONFIG::SESR_PLBS1_MID_MASK) | ((mid << CONFIG::SESR_PLBS1_MID_OFFSET) & CONFIG::SESR_PLBS1_MID_MASK);
			sesr_plbs1 = (sesr_plbs1 & ~CONFIG::SESR_PLBS1_MSIZE_MASK) | ((msize << CONFIG::SESR_PLBS1_MSIZE_OFFSET) & CONFIG::SESR_PLBS1_MSIZE_MASK);
			sesr_plbs1 = (sesr_plbs1 & ~CONFIG::SESR_PLBS1_TYPE_MASK) | ((type << CONFIG::SESR_PLBS1_TYPE_OFFSET) & CONFIG::SESR_PLBS1_TYPE_MASK);
			sesr_plbs1 = (sesr_plbs1 & ~CONFIG::SESR_PLBS1_RNW_MASK) | (rnw ? CONFIG::SESR_PLBS1_RNW_MASK : 0);
			sesr_plbs1 = (sesr_plbs1 & ~CONFIG::SESR_PLBS1_SIZE_MASK) | ((size << CONFIG::SESR_PLBS1_SIZE_OFFSET) & CONFIG::SESR_PLBS1_SIZE_MASK);
			sesr_plbs1 = (sesr_plbs1 & ~CONFIG::SESR_PLBS1_BE_MASK) | ((be << CONFIG::SESR_PLBS1_BE_OFFSET) & CONFIG::SESR_PLBS1_BE_MASK);
			sear_u_plbs1 = (addr >> 4) & CONFIG::SEAR_U_PLBS1_MASK;
			sear_l_plbs1 = addr & CONFIG::SEAR_L_PLBS1_MASK;
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return;
	}
	
}

template <class CONFIG>
void Crossbar<CONFIG>::Error(Interface master_if, Interface slave_if, typename CONFIG::ADDRESS addr, bool rnw, unsigned int length)
{
	logger << DebugWarning << "Transfer at @0x" << std::hex << addr << std::dec << " (" << (rnw ? "read" : "write")
		<< " of " << length << " bytes) from " << GetInterfaceName(master_if) << " to "
		<< GetInterfaceName(slave_if) << " failed" << EndDebugWarning;

	switch(slave_if)
	{
		case IF_MPLB:
			MPLBError(master_if, addr, rnw, length);
			break;
		case IF_MCI:
			break;
		default:
			logger << DebugError << "Internal error" << EndDebugError;
			Object::Stop(-1);
			return;
	}
	
	switch(master_if)
	{
		case IF_SPLB0:
		case IF_SPLB1:
			SPLBError(master_if, addr, rnw, length);
			break;
		default:
			break;
	}
}

template <class CONFIG>
const char *Crossbar<CONFIG>::GetInterfaceName(typename Crossbar<CONFIG>::Interface intf) const
{
	switch(intf)
	{
		case IF_ICURD_PLB: return "ICURD PLB";
		case IF_DCUWR_PLB: return "DCUWR PLB";
		case IF_DCURD_PLB: return "DCURD PLB";
		case IF_SPLB0: return "SPLB0";
		case IF_SPLB1: return "SPLB1";
		case IF_MPLB: return "MPLB";
		case IF_MCI: return "MCI";
		case IF_CROSSBAR_DCR: return "CROSSBAR DCR";
		case IF_PLBS0_DCR: return "PLBS0 DCR";
		case IF_PLBS1_DCR: return "PLBS1 DCR";
		case IF_PLBM_DCR: return "PLBM DCR";
	}
	return "?";
}

template <class CONFIG>
bool Crossbar<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetIST() const
{
	return ist;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetIMASK() const
{
	return imask;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetARB_XBC() const
{
	return arb_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFIFOST_XBC() const
{
	return fifost_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_XBC() const
{
	return sm_st_xbc;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_XBC() const
{
	return misc_xbc & ~CONFIG::MISC_XBC_WRITE_ONLY_MASK;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetARB_XBM() const
{
	return arb_xbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFIFOST_XBM() const
{
	return fifost_xbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_XBM() const
{
	return misc_xbm & ~CONFIG::MISC_XBM_WRITE_ONLY_MASK;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_XBAR_MAP(unsigned int reg_num) const
{
	return tmpl_xbar_map[reg_num];
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_SEL_REG() const
{
	return tmpl_sel_reg;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCFG_PLBS0() const
{
	return cfg_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_U_PLBS0() const
{
	return sear_u_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_L_PLBS0() const
{
	return sear_l_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSESR_PLBS0() const
{
	return sesr_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_ST_PLBS0()
{
	// Reading this register, clears bits IST[INT_CFG_ERR_S0] and IST[INT_FIFO_ERR_S0]
	ist = ist & ~(CONFIG::IST_INT_CFG_ERR_S0_MASK | CONFIG::IST_INT_FIFO_ERR_S0_MASK);
	return misc_st_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetPLBERR_ST_PLBS0() const
{
	return plberr_st_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_PLBS0() const
{
	return sm_st_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_PLBS0() const
{
	return misc_plbs0 & ~CONFIG::MISC_PLBS0_WRITE_ONLY_MASK;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFF_PLBS0() const
{
	return cmd_sniff_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFFA_PLBS0() const
{
	return cmd_sniffa_plbs0;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_PLBS0_MAP(unsigned int reg_num) const
{
	return tmpl_plbs0_map[reg_num];
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCFG_PLBS1() const
{
	return cfg_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_U_PLBS1() const
{
	return sear_u_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSEAR_L_PLBS1() const
{
	return sear_l_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSESR_PLBS1() const
{
	return sesr_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_ST_PLBS1()
{
	// Reading this register, clears bit IST[INT_CFG_ERR_S1] and IST[INT_FIFO_ERR_S1]
	ist = ist & ~(CONFIG::IST_INT_CFG_ERR_S1_MASK | CONFIG::IST_INT_FIFO_ERR_S1_MASK);
	return misc_st_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetPLBERR_ST_PLBS1() const
{
	return plberr_st_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_PLBS1() const
{
	return sm_st_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_PLBS1() const
{
	return misc_plbs1 & ~CONFIG::MISC_PLBS1_WRITE_ONLY_MASK;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFF_PLBS1() const
{
	return cmd_sniff_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFFA_PLBS1() const
{
	return cmd_sniffa_plbs1;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetTMPL_PLBS1_MAP(unsigned int reg_num) const
{
	return tmpl_plbs1_map[reg_num];
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCFG_PLBM() const
{
	return cfg_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFSEAR_U_PLBM() const
{
	return fsear_u_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFSEAR_L_PLBM() const
{
	return fsear_l_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetFSESR_PLBM() const
{
	return fsesr_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_ST_PLBM()
{
	// Reading this register, clears bits IST[INT_CFG_ERR_M] and IST[INT_FIFO_ERR_M]
	ist = ist & ~(CONFIG::IST_INT_CFG_ERR_M_MASK | CONFIG::IST_INT_FIFO_ERR_M_MASK);
	return misc_st_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetPLBERR_ST_PLBM() const
{
	return plberr_st_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetSM_ST_PLBM() const
{
	return sm_st_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetMISC_PLBM() const
{
	return misc_plbm & ~CONFIG::MISC_PLBM_WRITE_ONLY_MASK;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFF_PLBM() const
{
	return cmd_sniff_plbm;
}

template <class CONFIG>
uint32_t Crossbar<CONFIG>::GetCMD_SNIFFA_PLBM() const
{
	return cmd_sniffa_plbm;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetIST(uint32_t value)
{
	// Clear on write to bit
	ist = ist & ~(value & CONFIG::IST_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetIMASK(uint32_t value)
{
	imask = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetARB_XBC(uint32_t value)
{
	arb_xbc = (arb_xbc & ~CONFIG::ARB_XBC_WRITE_MASK) | (value & CONFIG::ARB_XBC_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFIFOST_XBC(uint32_t value)
{
	// Clear on write to bit
	fifost_xbc = fifost_xbc & ~(value & CONFIG::FIFOST_XBC_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_XBC(uint32_t value)
{
	sm_st_xbc = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_XBC(uint32_t value)
{
	// Clear on write to bit
	misc_xbc = misc_xbc & ~(value & CONFIG::MISC_XBC_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetARB_XBM(uint32_t value)
{
	arb_xbm = (arb_xbm & ~CONFIG::ARB_XBM_WRITE_MASK) | (value & CONFIG::ARB_XBM_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFIFOST_XBM(uint32_t value)
{
	// Clear on write to bit
	fifost_xbm = fifost_xbm & ~(value & CONFIG::FIFOST_XBM_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_XBM(uint32_t value)
{
	// clear bits on write
	misc_xbm = misc_xbm & ~value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_XBAR_MAP(unsigned int reg_num, uint32_t value)
{
	tmpl_xbar_map[reg_num] = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_SEL_REG(uint32_t value)
{
	tmpl_sel_reg = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCFG_PLBS0(uint32_t value)
{
	cfg_plbs0 = (cfg_plbs0 & ~CONFIG::CFG_PLBS0_WRITE_MASK) | (value & CONFIG::CFG_PLBS0_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_U_PLBS0(uint32_t value)
{
	// this register is cleared when writing to register SESR_PLBS0
	if(!(cfg_plbs0 & CONFIG::CFG_PLBS0_LOCK_SESR_MASK))
	{
		sear_u_plbs0 = (sear_u_plbs0 & ~CONFIG::SEAR_U_PLBS0_WRITE_MASK) | (value & CONFIG::SEAR_U_PLBS0_WRITE_MASK);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_L_PLBS0(uint32_t value)
{
	// this register is cleared when writing to register SESR_PLBS0
	if(!(cfg_plbs0 & CONFIG::CFG_PLBS0_LOCK_SESR_MASK))
	{
		sear_l_plbs0 = (sear_l_plbs0 & ~CONFIG::SEAR_L_PLBS0_WRITE_MASK) | (value & CONFIG::SEAR_L_PLBS0_WRITE_MASK);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSESR_PLBS0(uint32_t value)
{
	if(!(cfg_plbs0 & CONFIG::CFG_PLBS0_LOCK_SESR_MASK))
	{
		// Clear bits on write
		sesr_plbs0 = sesr_plbs0 & ~(value & CONFIG::SESR_PLBS0_CLEAR_ON_WRITE_MASK);
		// Writing to this register also clears SEAR_U_PLBS0 and SEAR_L_PLBS0
		sear_u_plbs0 = sear_u_plbs0 & ~CONFIG::SEAR_U_PLBS0_MASK;
		sear_l_plbs0 = sear_l_plbs0 & ~CONFIG::SEAR_L_PLBS0_MASK;
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_ST_PLBS0(uint32_t value)
{
	// clear bits on write
	misc_st_plbs0 = misc_st_plbs0 & ~(value & CONFIG::MISC_ST_PLBS0_CLEAR_ON_WRITE_MASK);
	// Write zero's to this register makes IST[INT_CFG_ERR_S0] become zero
	if(!(misc_st_plbs0 & CONFIG::MISC_ST_PLBS0_MASK)) ist = ist & ~CONFIG::IST_INT_CFG_ERR_S0_MASK;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetPLBERR_ST_PLBS0(uint32_t value)
{
	// clear bits on write
	plberr_st_plbs0 = plberr_st_plbs0 & ~(value & CONFIG::PLBERR_ST_PLBS0_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_PLBS0(uint32_t value)
{
	// register is reserved
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_PLBS0(uint32_t value)
{
	// clear bits on write
	misc_plbs0 = misc_plbs0 & ~(value & CONFIG::MISC_PLBS0_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFF_PLBS0(uint32_t value)
{
	cmd_sniff_plbs0 = (cmd_sniff_plbs0 & ~CONFIG::CMD_SNIFF_PLBS0_WRITE_MASK) | (value & CONFIG::CMD_SNIFF_PLBS0_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFFA_PLBS0(uint32_t value)
{
	cmd_sniffa_plbs0 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_PLBS0_MAP(unsigned int reg_num, uint32_t value)
{
	tmpl_plbs0_map[reg_num] = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCFG_PLBS1(uint32_t value)
{
	cfg_plbs1 = (cfg_plbs1 & ~CONFIG::CFG_PLBS1_WRITE_MASK) | (value & CONFIG::CFG_PLBS1_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_U_PLBS1(uint32_t value)
{
	// this register is cleared when writing to register SESR_PLBS1
	if(!(cfg_plbs1 & CONFIG::CFG_PLBS1_LOCK_SESR_MASK))
	{
		sear_u_plbs1 = (sear_u_plbs1 & ~CONFIG::SEAR_U_PLBS1_WRITE_MASK) | (value & CONFIG::SEAR_U_PLBS1_WRITE_MASK);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSEAR_L_PLBS1(uint32_t value)
{
	// this register is cleared when writing to register SESR_PLBS1
	if(!(cfg_plbs1 & CONFIG::CFG_PLBS1_LOCK_SESR_MASK))
	{
		sear_l_plbs1 = (sear_l_plbs1 & ~CONFIG::SEAR_L_PLBS1_WRITE_MASK) | (value & CONFIG::SEAR_L_PLBS1_WRITE_MASK);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSESR_PLBS1(uint32_t value)
{
	if(!(cfg_plbs1 & CONFIG::CFG_PLBS1_LOCK_SESR_MASK))
	{
		// Clear bits on write
		sesr_plbs1 = sesr_plbs1 & ~(value & CONFIG::SESR_PLBS1_CLEAR_ON_WRITE_MASK);
		// Writing to this register also clears SEAR_U_PLBS1 and SEAR_L_PLBS1
		sear_u_plbs1 = sear_u_plbs1 & ~CONFIG::SEAR_U_PLBS1_MASK;
		sear_l_plbs1 = sear_l_plbs1 & ~CONFIG::SEAR_L_PLBS1_MASK;
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_ST_PLBS1(uint32_t value)
{
	// clear bits on write
	misc_st_plbs1 = misc_st_plbs1 & ~(value & CONFIG::MISC_ST_PLBS1_CLEAR_ON_WRITE_MASK);
	// Write zero's to this register makes IST[INT_CFG_ERR_S1] become zero
	if(!(misc_st_plbs1 & CONFIG::MISC_ST_PLBS1_MASK)) ist = ist & ~CONFIG::IST_INT_CFG_ERR_S1_MASK;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetPLBERR_ST_PLBS1(uint32_t value)
{
	// clear bits on write
	plberr_st_plbs1 = plberr_st_plbs1 & ~(value & CONFIG::PLBERR_ST_PLBS1_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_PLBS1(uint32_t value)
{
	// register is reserved
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_PLBS1(uint32_t value)
{
	// clear bits on write
	misc_plbs1 = misc_plbs1 & ~(value & CONFIG::MISC_PLBS1_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFF_PLBS1(uint32_t value)
{
	cmd_sniff_plbs1 = (cmd_sniff_plbs1 & ~CONFIG::CMD_SNIFF_PLBS1_WRITE_MASK) | (value & CONFIG::CMD_SNIFF_PLBS1_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFFA_PLBS1(uint32_t value)
{
	cmd_sniffa_plbs1 = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetTMPL_PLBS1_MAP(unsigned int reg_num, uint32_t value)
{
	tmpl_plbs1_map[reg_num] = value;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCFG_PLBM(uint32_t value)
{
	cfg_plbm = (cfg_plbm & ~CONFIG::CFG_PLBM_WRITE_MASK) | (value & CONFIG::CFG_PLBM_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFSEAR_U_PLBM(uint32_t value)
{
	// this register is cleared when writing to register SESR_PLBM
	if(!(cfg_plbm & CONFIG::CFG_PLBM_LOCK_SESR_MASK))
	{
		fsear_u_plbm = (fsear_u_plbm & ~CONFIG::FSEAR_U_PLBM_WRITE_MASK) | (value & CONFIG::FSEAR_U_PLBM_WRITE_MASK);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFSEAR_L_PLBM(uint32_t value)
{
	// this register is cleared when writing to register SESR_PLBM
	if(!(cfg_plbm & CONFIG::CFG_PLBM_LOCK_SESR_MASK))
	{
		fsear_l_plbm = (fsear_l_plbm & ~CONFIG::FSEAR_L_PLBM_WRITE_MASK) | (value & CONFIG::FSEAR_L_PLBM_WRITE_MASK);
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetFSESR_PLBM(uint32_t value)
{
	if(!(cfg_plbm & CONFIG::CFG_PLBM_LOCK_SESR_MASK))
	{
		// Clear bits on write
		fsesr_plbm = fsesr_plbm & ~(value & CONFIG::FSESR_PLBM_CLEAR_ON_WRITE_MASK);
		// Writing to this register also clears FSEAR_U_PLBM and FSEAR_L_PLBM
		fsear_u_plbm = fsear_u_plbm & ~CONFIG::FSEAR_U_PLBM_MASK;
		fsear_l_plbm = fsear_l_plbm & ~CONFIG::FSEAR_L_PLBM_MASK;
	}
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_ST_PLBM(uint32_t value)
{
	// clear bits on write
	misc_st_plbm = misc_st_plbm & ~(value & CONFIG::MISC_ST_PLBM_CLEAR_ON_WRITE_MASK);
	// Write zero's to this register makes IST[INT_CFG_ERR_M] become zero
	if(!(misc_st_plbm & CONFIG::MISC_ST_PLBM_MASK)) ist = ist & ~CONFIG::IST_INT_CFG_ERR_M_MASK;
}

template <class CONFIG>
void Crossbar<CONFIG>::SetPLBERR_ST_PLBM(uint32_t value)
{
	// clear bits on write
	plberr_st_plbm = plberr_st_plbm & ~(value & CONFIG::PLBERR_ST_PLBM_CLEAR_ON_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetSM_ST_PLBM(uint32_t value)
{
	// register is reserved
}

template <class CONFIG>
void Crossbar<CONFIG>::SetMISC_PLBM(uint32_t value)
{
	// clear bits on write for some bits, and write to some other bits
	misc_plbm = (misc_plbm & ~CONFIG::MISC_PLBM_WRITE_MASK & ~(value & CONFIG::MISC_PLBM_CLEAR_ON_WRITE_MASK)) | (value & CONFIG::MISC_PLBM_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFF_PLBM(uint32_t value)
{
	cmd_sniff_plbm = (cmd_sniff_plbm & ~CONFIG::CMD_SNIFF_PLBM_WRITE_MASK) | (value & CONFIG::CMD_SNIFF_PLBM_WRITE_MASK);
}

template <class CONFIG>
void Crossbar<CONFIG>::SetCMD_SNIFFA_PLBM(uint32_t value)
{
	cmd_sniffa_plbm = value;
}

} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_TCC__
