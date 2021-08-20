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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/kernel/logger/logger.hh>
#include <inttypes.h>
#include <iosfwd>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace crossbar {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::service::interfaces::Memory;

template <class CONFIG> class AddressRange;

template <class CONFIG>
std::ostream& operator << (std::ostream& os, const AddressRange<CONFIG>& addr_range);

template <class CONFIG>
class AddressRange
{
public:
	AddressRange(typename CONFIG::ADDRESS start_addr, typename CONFIG::ADDRESS end_addr);
	~AddressRange();
	typename CONFIG::ADDRESS GetStartAddr() const;
	typename CONFIG::ADDRESS GetEndAddr() const;
	void SetStartAddr(typename CONFIG::ADDRESS start_addr);
	void SetEndAddr(typename CONFIG::ADDRESS end_addr);
private:
	friend std::ostream& operator << <CONFIG>(std::ostream& os, const AddressRange<CONFIG>& addr_range);
	typename CONFIG::ADDRESS start_addr;
	typename CONFIG::ADDRESS end_addr;
};

template <class CONFIG>
class Crossbar
	: public Service<Memory<typename CONFIG::ADDRESS> >
	, public Client<Memory<typename CONFIG::ADDRESS> >
{
public:
	ServiceExport<Memory<typename CONFIG::ADDRESS> > memory_export;
	ServiceImport<Memory<typename CONFIG::ADDRESS> > mci_memory_import;
	ServiceImport<Memory<typename CONFIG::ADDRESS> > mplb_memory_import;
	
	Crossbar(const char *name, Object *parent = 0);
	virtual ~Crossbar();
	
	void Reset();
	
	virtual void ResetMemory();
	virtual bool ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size);

	bool IsMappedDCR(uint32_t dcrn) const;
	uint32_t ReadDCR(unsigned int dcrn);
	void WriteDCR(unsigned int dcrn, uint32_t value);
	
protected:
	typedef enum
	{
		IF_ICURD_PLB,
		IF_DCUWR_PLB,
		IF_DCURD_PLB,
		IF_SPLB0,
		IF_SPLB1,
		IF_MPLB,
		IF_MCI,
		IF_CROSSBAR_DCR,
		IF_PLBS0_DCR,
		IF_PLBS1_DCR,
		IF_PLBM_DCR
	} Interface;
	
	unisim::kernel::logger::Logger logger;
	bool IsVerbose() const;
	Interface Route(Interface intf, typename CONFIG::ADDRESS addr, typename CONFIG::ADDRESS& start_range, typename CONFIG::ADDRESS& end_range);
	void DumpAddressMapping(typename CONFIG::ADDRESS start_addr, typename CONFIG::ADDRESS end_addr, Interface intf, std::ostream& os);
	void DumpAddressMapping(Interface intf, std::ostream& os);
	void GetAddressRanges(Interface src_if, Interface dst_if, std::list<AddressRange<CONFIG> >& lst);
	const char *GetInterfaceName(Interface intf) const;
	
	void MPLBError(Interface master_if, typename CONFIG::ADDRESS addr, bool rnw, unsigned int length);
	void SPLBError(Interface master_if, typename CONFIG::ADDRESS addr, bool rnw, unsigned int length);
	void Error(Interface master_if, Interface slave_if, typename CONFIG::ADDRESS addr, bool rnw, unsigned int length);
private:
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Crossbar ////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	// Global configuration and status
	uint32_t ist;   // Interrupt Status Register
	uint32_t imask; // interrupt Mask Register
	
	// Crossbar for PLB Master Configuration
	uint32_t arb_xbc; // Arbitration Configuration Register
	
	// Crossbar for PLB Master Status
	uint32_t fifost_xbc; // FIFO overflow and Underflow Status
	
	// Crossbar for PLB Master Hardware Debug
	uint32_t sm_st_xbc;  // State Machine States Register
	uint32_t misc_xbc;   // Miscellaneous Control and Status
	
	// crossbar for MCI Configuration
	uint32_t arb_xbm;    // Arbitration Configuration Register
	
	// Crossbar for MCI Status
	uint32_t fifost_xbm;  // FIFO Overflow and Underflow Status
	
	// Crossbar for MCI Hardware Debug
	uint32_t misc_xbm;   // Miscellaneous Control and Status
	
	// Address Map Configuration
	uint32_t tmpl_xbar_map[4]; // Template Registers for Crossbar
	uint32_t tmpl_sel_reg;     // Template Selection Register
	
	//////////////////////////////////////////////////////////////////////////
	///////////////////////// PLB Slave 0 (SPLB0)/////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	// Configuration
	uint32_t cfg_plbs0;  // Configuration Register
	
	// Status
	uint32_t sear_u_plbs0;     // Slave Error Address Register, upper 4 bits
	uint32_t sear_l_plbs0;     // Slave Error Address Register, lower 32 bits
	uint32_t sesr_plbs0;       // Slave Error Status Register
	uint32_t misc_st_plbs0;    // Miscellaneous Status Register
	uint32_t plberr_st_plbs0;  // PLB Error Status
	
	// Hardware Debug
	uint32_t sm_st_plbs0;       // State Machine States Register
	uint32_t misc_plbs0;        // Miscellaneous Control and Status
	uint32_t cmd_sniff_plbs0;   // Command Sniffer
	uint32_t cmd_sniffa_plbs0;  // Command Sniffer Address
	
	// Address Map
	uint32_t tmpl_plbs0_map[4];  // Template Register
	
	//////////////////////////////////////////////////////////////////////////
	////////////////////////// PLB Slave 1 (SPLB1) ///////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	// Configuration
	uint32_t cfg_plbs1;  // Configuration Register
	
	// Status
	uint32_t sear_u_plbs1;     // Slave Error Address Register, upper 4 bits
	uint32_t sear_l_plbs1;     // Slave Error Address Register, lower 32 bits
	uint32_t sesr_plbs1;       // Slave Error Status Register
	uint32_t misc_st_plbs1;    // Miscellaneous Status Register
	uint32_t plberr_st_plbs1;  // PLB Error Status
	
	// Hardware Debug
	uint32_t sm_st_plbs1;       // State Machine States Register
	uint32_t misc_plbs1;        // Miscellaneous Control and Status
	uint32_t cmd_sniff_plbs1;   // Command Sniffer
	uint32_t cmd_sniffa_plbs1;  // Command Sniffer Address
	
	// Address Map
	uint32_t tmpl_plbs1_map[4];  // Template Register
	
	//////////////////////////////////////////////////////////////////////////
	/////////////////////////// PLB Master (MPLB) ////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	// Configuration
	uint32_t cfg_plbm;  // Configuration Register
	
	// Status
	uint32_t fsear_u_plbm;     // FPGA Logic Slave Error Address Register, upper 4 bits
	uint32_t fsear_l_plbm;     // FPGA Logic Slave Error Address Register, lower 32 bits
	uint32_t fsesr_plbm;       // FPGA Logic Slave Error Status Register
	uint32_t misc_st_plbm;     // Miscellaneous Status Register
	uint32_t plberr_st_plbm;   // PLB Error Status
	
	// Hardware Debug
	uint32_t sm_st_plbm;       // State Machine States Register
	uint32_t misc_plbm;        // Miscellaneous Control and Status
	uint32_t cmd_sniff_plbm;   // Command Sniffer
	uint32_t cmd_sniffa_plbm;  // Command Sniffer Address
	
	//--------------------------------------------------------------------------
	
	bool verbose;
	Parameter<bool> param_verbose;
	
	uint32_t GetIST() const;
	uint32_t GetIMASK() const;
	uint32_t GetARB_XBC() const;
	uint32_t GetFIFOST_XBC() const;
	uint32_t GetSM_ST_XBC() const;
	uint32_t GetMISC_XBC() const;
	uint32_t GetARB_XBM() const;
	uint32_t GetFIFOST_XBM() const;
	uint32_t GetMISC_XBM() const;
	uint32_t GetTMPL_XBAR_MAP(unsigned int reg_num) const;
	uint32_t GetTMPL_SEL_REG() const;
	uint32_t GetCFG_PLBS0() const;
	uint32_t GetSEAR_U_PLBS0() const;
	uint32_t GetSEAR_L_PLBS0() const;
	uint32_t GetSESR_PLBS0() const;
	uint32_t GetMISC_ST_PLBS0();
	uint32_t GetPLBERR_ST_PLBS0() const;
	uint32_t GetSM_ST_PLBS0() const;
	uint32_t GetMISC_PLBS0() const;
	uint32_t GetCMD_SNIFF_PLBS0() const;
	uint32_t GetCMD_SNIFFA_PLBS0() const;
	uint32_t GetTMPL_PLBS0_MAP(unsigned int reg_num) const;
	uint32_t GetCFG_PLBS1() const;
	uint32_t GetSEAR_U_PLBS1() const;
	uint32_t GetSEAR_L_PLBS1() const;
	uint32_t GetSESR_PLBS1() const;
	uint32_t GetMISC_ST_PLBS1();
	uint32_t GetPLBERR_ST_PLBS1() const;
	uint32_t GetSM_ST_PLBS1() const;
	uint32_t GetMISC_PLBS1() const;
	uint32_t GetCMD_SNIFF_PLBS1() const;
	uint32_t GetCMD_SNIFFA_PLBS1() const;
	uint32_t GetTMPL_PLBS1_MAP(unsigned int reg_num) const;
	uint32_t GetCFG_PLBM() const;
	uint32_t GetFSEAR_U_PLBM() const;
	uint32_t GetFSEAR_L_PLBM() const;
	uint32_t GetFSESR_PLBM() const;
	uint32_t GetMISC_ST_PLBM();
	uint32_t GetPLBERR_ST_PLBM() const;
	uint32_t GetSM_ST_PLBM() const;
	uint32_t GetMISC_PLBM() const;
	uint32_t GetCMD_SNIFF_PLBM() const;
	uint32_t GetCMD_SNIFFA_PLBM() const;

	void SetIST(uint32_t value);
	void SetIMASK(uint32_t value);
	void SetARB_XBC(uint32_t value);
	void SetFIFOST_XBC(uint32_t value);
	void SetSM_ST_XBC(uint32_t value);
	void SetMISC_XBC(uint32_t value);
	void SetARB_XBM(uint32_t value);
	void SetFIFOST_XBM(uint32_t value);
	void SetMISC_XBM(uint32_t value);
	void SetTMPL_XBAR_MAP(unsigned int reg_num, uint32_t value);
	void SetTMPL_SEL_REG(uint32_t value);
	void SetCFG_PLBS0(uint32_t value);
	void SetSEAR_U_PLBS0(uint32_t value);
	void SetSEAR_L_PLBS0(uint32_t value);
	void SetSESR_PLBS0(uint32_t value);
	void SetMISC_ST_PLBS0(uint32_t value);
	void SetPLBERR_ST_PLBS0(uint32_t value);
	void SetSM_ST_PLBS0(uint32_t value);
	void SetMISC_PLBS0(uint32_t value);
	void SetCMD_SNIFF_PLBS0(uint32_t value);
	void SetCMD_SNIFFA_PLBS0(uint32_t value);
	void SetTMPL_PLBS0_MAP(unsigned int reg_num, uint32_t value);
	void SetCFG_PLBS1(uint32_t value);
	void SetSEAR_U_PLBS1(uint32_t value);
	void SetSEAR_L_PLBS1(uint32_t value);
	void SetSESR_PLBS1(uint32_t value);
	void SetMISC_ST_PLBS1(uint32_t value);
	void SetPLBERR_ST_PLBS1(uint32_t value);
	void SetSM_ST_PLBS1(uint32_t value);
	void SetMISC_PLBS1(uint32_t value);
	void SetCMD_SNIFF_PLBS1(uint32_t value);
	void SetCMD_SNIFFA_PLBS1(uint32_t value);
	void SetTMPL_PLBS1_MAP(unsigned int reg_num, uint32_t value);
	void SetCFG_PLBM(uint32_t value);
	void SetFSEAR_U_PLBM(uint32_t value);
	void SetFSEAR_L_PLBM(uint32_t value);
	void SetFSESR_PLBM(uint32_t value);
	void SetMISC_ST_PLBM(uint32_t value);
	void SetPLBERR_ST_PLBM(uint32_t value);
	void SetSM_ST_PLBM(uint32_t value);
	void SetMISC_PLBM(uint32_t value);
	void SetCMD_SNIFF_PLBM(uint32_t value);
	void SetCMD_SNIFFA_PLBM(uint32_t value);
};

} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CROSSBAR_HH__
