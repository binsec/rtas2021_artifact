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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CONFIG_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace crossbar {

class DCRMap
{
public:
	static const uint32_t DCR_LOW          = 0x20;
	static const uint32_t DCR_HIGH         = 0x5f;
	
	static const uint32_t IST              = 0x20;
	static const uint32_t IMASK            = 0x21;
	static const uint32_t ARB_XBC          = 0x23;
	static const uint32_t FIFOST_XBC       = 0x24;
	static const uint32_t SM_ST_XBC        = 0x25;
	static const uint32_t MISC_XBC         = 0x26;
	static const uint32_t ARB_XBM          = 0x28;
	static const uint32_t FIFOST_XBM       = 0x29;
	static const uint32_t MISC_XBM         = 0x2b;
	static const uint32_t TMPL0_XBAR_MAP   = 0x2d;
	static const uint32_t TMPL1_XBAR_MAP   = 0x2e;
	static const uint32_t TMPL2_XBAR_MAP   = 0x2f;
	static const uint32_t TMPL3_XBAR_MAP   = 0x30;
	static const uint32_t TMPL_SEL_REG     = 0x31;
	static const uint32_t CFG_PLBS0        = 0x34;
	static const uint32_t SEAR_U_PLBS0     = 0x36;
	static const uint32_t SEAR_L_PLBS0     = 0x37;
	static const uint32_t SESR_PLBS0       = 0x38;
	static const uint32_t MISC_ST_PLBS0    = 0x39;
	static const uint32_t PLBERR_ST_PLBS0  = 0x3a;
	static const uint32_t SM_ST_PLBS0      = 0x3b;
	static const uint32_t MISC_PLBS0       = 0x3c;
	static const uint32_t CMD_SNIFF_PLBS0  = 0x3d;
	static const uint32_t CMD_SNIFFA_PLBS0 = 0x3e;
	static const uint32_t TMPL0_PLBS0_MAP  = 0x40;
	static const uint32_t TMPL1_PLBS0_MAP  = 0x41;
	static const uint32_t TMPL2_PLBS0_MAP  = 0x42;
	static const uint32_t TMPL3_PLBS0_MAP  = 0x43;
	static const uint32_t CFG_PLBS1        = 0x44;
	static const uint32_t SEAR_U_PLBS1     = 0x46;
	static const uint32_t SEAR_L_PLBS1     = 0x47;
	static const uint32_t SESR_PLBS1       = 0x48;
	static const uint32_t MISC_ST_PLBS1    = 0x49;
	static const uint32_t PLBERR_ST_PLBS1  = 0x4a;
	static const uint32_t SM_ST_PLBS1      = 0x4b;
	static const uint32_t MISC_PLBS1       = 0x4c;
	static const uint32_t CMD_SNIFF_PLBS1  = 0x4d;
	static const uint32_t CMD_SNIFFA_PLBS1 = 0x4e;
	static const uint32_t TMPL0_PLBS1_MAP  = 0x50;
	static const uint32_t TMPL1_PLBS1_MAP  = 0x51;
	static const uint32_t TMPL2_PLBS1_MAP  = 0x52;
	static const uint32_t TMPL3_PLBS1_MAP  = 0x53;
	static const uint32_t CFG_PLBM         = 0x54;
	static const uint32_t FSEAR_U_PLBM     = 0x56;
	static const uint32_t FSEAR_L_PLBM     = 0x57;
	static const uint32_t FSESR_PLBM       = 0x58;
	static const uint32_t MISC_ST_PLBM     = 0x59;
	static const uint32_t PLBERR_ST_PLBM   = 0x5a;
	static const uint32_t SM_ST_PLBM       = 0x5b;
	static const uint32_t MISC_PLBM        = 0x5c;
	static const uint32_t CMD_SNIFF_PLBM   = 0x5d;
	static const uint32_t CMD_SNIFFA_PLBM  = 0x5e;
};

class ISTLayout
{
public:
	static const unsigned int IST_INT_CFG_ERR_S0_OFFSET = 28;
	static const unsigned int IST_INT_MIRQ_S0_OFFSET = 27;
	static const unsigned int IST_INT_MRDERR_S0_OFFSET = 26;
	static const unsigned int IST_INT_MWRERR_S0_OFFSET = 25;
	static const unsigned int IST_INT_FIFO_ERR_S0_OFFSET = 24;
	static const unsigned int IST_INT_CFG_ERR_S1_OFFSET = 20;
	static const unsigned int IST_INT_MIRQ_S1_OFFSET = 19;
	static const unsigned int IST_INT_MRDERR_S1_OFFSET = 18;
	static const unsigned int IST_INT_MWRERR_S1_OFFSET = 17;
	static const unsigned int IST_INT_FIFO_ERR_S1_OFFSET = 16;
	static const unsigned int IST_INT_CFG_ERR_M_OFFSET = 14;
	static const unsigned int IST_INT_MIRQ_M_OFFSET = 13;
	static const unsigned int IST_INT_MRDERR_M_OFFSET = 12;
	static const unsigned int IST_INT_MWRERR_M_OFFSET = 11;
	static const unsigned int IST_INT_ARB_TOUT_M_OFFSET = 10;
	static const unsigned int IST_INT_FIFO_ERR_M_OFFSET = 7;
	static const unsigned int IST_INT_FIFO_ERR_XM_OFFSET = 6;
	static const unsigned int IST_INT_FIFO_ERR_MCI_OFFSET = 5;

	static const unsigned int IST_INT_CFG_ERR_S0_BITSIZE = 1;
	static const unsigned int IST_INT_MIRQ_S0_BITSIZE = 1;
	static const unsigned int IST_INT_MRDERR_S0_BITSIZE = 1;
	static const unsigned int IST_INT_MWRERR_S0_BITSIZE = 1;
	static const unsigned int IST_INT_FIFO_ERR_S0_BITSIZE = 1;
	static const unsigned int IST_INT_CFG_ERR_S1_BITSIZE = 1;
	static const unsigned int IST_INT_MIRQ_S1_BITSIZE = 1;
	static const unsigned int IST_INT_MRDERR_S1_BITSIZE = 1;
	static const unsigned int IST_INT_MWRERR_S1_BITSIZE = 1;
	static const unsigned int IST_INT_FIFO_ERR_S1_BITSIZE = 1;
	static const unsigned int IST_INT_CFG_ERR_M_BITSIZE = 1;
	static const unsigned int IST_INT_MIRQ_M_BITSIZE = 1;
	static const unsigned int IST_INT_MRDERR_M_BITSIZE = 1;
	static const unsigned int IST_INT_MWRERR_M_BITSIZE = 1;
	static const unsigned int IST_INT_ARB_TOUT_M_BITSIZE = 1;
	static const unsigned int IST_INT_FIFO_ERR_M_BITSIZE = 1;
	static const unsigned int IST_INT_FIFO_ERR_XM_BITSIZE = 1;
	static const unsigned int IST_INT_FIFO_ERR_MCI_BITSIZE = 1;
	
	static const uint32_t IST_INT_CFG_ERR_S0_MASK = ((1UL << IST_INT_CFG_ERR_S0_BITSIZE) - 1) << IST_INT_CFG_ERR_S0_OFFSET;
	static const uint32_t IST_INT_MIRQ_S0_MASK = ((1UL << IST_INT_MIRQ_S0_BITSIZE) - 1) << IST_INT_MIRQ_S0_OFFSET;
	static const uint32_t IST_INT_MRDERR_S0_MASK = ((1UL << IST_INT_MRDERR_S0_BITSIZE) - 1) << IST_INT_MRDERR_S0_OFFSET;
	static const uint32_t IST_INT_MWRERR_S0_MASK = ((1UL << IST_INT_MWRERR_S0_BITSIZE) - 1) << IST_INT_MWRERR_S0_OFFSET;
	static const uint32_t IST_INT_FIFO_ERR_S0_MASK = ((1UL << IST_INT_FIFO_ERR_S0_BITSIZE) - 1) << IST_INT_FIFO_ERR_S0_OFFSET;
	static const uint32_t IST_INT_CFG_ERR_S1_MASK = ((1UL << IST_INT_CFG_ERR_S1_BITSIZE) - 1) << IST_INT_CFG_ERR_S1_OFFSET;
	static const uint32_t IST_INT_MIRQ_S1_MASK = ((1UL << IST_INT_MIRQ_S1_BITSIZE) - 1) << IST_INT_MIRQ_S1_OFFSET;
	static const uint32_t IST_INT_MRDERR_S1_MASK = ((1UL << IST_INT_MRDERR_S1_BITSIZE) - 1) << IST_INT_MRDERR_S1_OFFSET;
	static const uint32_t IST_INT_MWRERR_S1_MASK = ((1UL << IST_INT_MWRERR_S1_BITSIZE) - 1) << IST_INT_MWRERR_S1_OFFSET;
	static const uint32_t IST_INT_FIFO_ERR_S1_MASK = ((1UL << IST_INT_FIFO_ERR_S1_BITSIZE) - 1) << IST_INT_FIFO_ERR_S1_OFFSET;
	static const uint32_t IST_INT_CFG_ERR_M_MASK = ((1UL << IST_INT_CFG_ERR_M_BITSIZE) - 1) << IST_INT_CFG_ERR_M_OFFSET;
	static const uint32_t IST_INT_MIRQ_M_MASK = ((1UL << IST_INT_MIRQ_M_BITSIZE) - 1) << IST_INT_MIRQ_M_OFFSET;
	static const uint32_t IST_INT_MRDERR_M_MASK = ((1UL << IST_INT_MRDERR_M_BITSIZE) - 1) << IST_INT_MRDERR_M_OFFSET;
	static const uint32_t IST_INT_MWRERR_M_MASK = ((1UL << IST_INT_MWRERR_M_BITSIZE) - 1) << IST_INT_MWRERR_M_OFFSET;
	static const uint32_t IST_INT_ARB_TOUT_M_MASK = ((1UL << IST_INT_ARB_TOUT_M_BITSIZE) - 1) << IST_INT_ARB_TOUT_M_OFFSET;
	static const uint32_t IST_INT_FIFO_ERR_M_MASK = ((1UL << IST_INT_FIFO_ERR_M_BITSIZE) - 1) << IST_INT_FIFO_ERR_M_OFFSET;
	static const uint32_t IST_INT_FIFO_ERR_XM_MASK = ((1UL << IST_INT_FIFO_ERR_XM_BITSIZE) - 1) << IST_INT_FIFO_ERR_XM_OFFSET;
	static const uint32_t IST_INT_FIFO_ERR_MCI_MASK = ((1UL << IST_INT_FIFO_ERR_MCI_BITSIZE) - 1) << IST_INT_FIFO_ERR_MCI_OFFSET;

	static const uint32_t IST_CLEAR_ON_WRITE_MASK = IST_INT_MRDERR_S0_MASK | IST_INT_MWRERR_S0_MASK | IST_INT_MRDERR_S1_MASK | IST_INT_MWRERR_S1_MASK |
	                                                IST_INT_MRDERR_M_MASK | IST_INT_MWRERR_M_MASK | IST_INT_ARB_TOUT_M_MASK;
	
	static const uint32_t IST_RESET_VALUE = 0x0UL;
};

class ARB_XBC_Layout
{
public:
	static const unsigned int ARB_XBC_440ICUR_OFFSET = 20;
	static const unsigned int ARB_XBC_440DCUW_OFFSET = 16;
	static const unsigned int ARB_XBC_440DCUR_OFFSET = 12;
	static const unsigned int ARB_XBC_PLBS1_OFFSET = 8;
	static const unsigned int ARB_XBC_PLBS0_OFFSET = 4;
	static const unsigned int ARB_XBC_SYNCTATTR_OFFSET = 2;
	static const unsigned int ARB_XBC_MODE_OFFSET = 0;

	static const unsigned int ARB_XBC_440ICUR_BITSIZE = 3;
	static const unsigned int ARB_XBC_440DCUW_BITSIZE = 3;
	static const unsigned int ARB_XBC_440DCUR_BITSIZE = 3;
	static const unsigned int ARB_XBC_PLBS1_BITSIZE = 3;
	static const unsigned int ARB_XBC_PLBS0_BITSIZE = 3;
	static const unsigned int ARB_XBC_SYNCTATTR_BITSIZE = 1;
	static const unsigned int ARB_XBC_MODE_BITSIZE = 2;
	
	static const uint32_t ARB_XBC_440ICUR_MASK = ((1UL << ARB_XBC_440ICUR_BITSIZE) - 1) << ARB_XBC_440ICUR_OFFSET;
	static const uint32_t ARB_XBC_440DCUW_MASK = ((1UL << ARB_XBC_440DCUW_BITSIZE) - 1) << ARB_XBC_440DCUW_OFFSET;
	static const uint32_t ARB_XBC_440DCUR_MASK = ((1UL << ARB_XBC_440DCUR_BITSIZE) - 1) << ARB_XBC_440DCUR_OFFSET;
	static const uint32_t ARB_XBC_PLBS1_MASK = ((1UL << ARB_XBC_PLBS1_BITSIZE) - 1) << ARB_XBC_PLBS1_OFFSET;
	static const uint32_t ARB_XBC_PLBS0_MASK = ((1UL << ARB_XBC_PLBS0_BITSIZE) - 1) << ARB_XBC_PLBS0_OFFSET;
	static const uint32_t ARB_XBC_SYNCTATTR_MASK = ((1UL << ARB_XBC_SYNCTATTR_BITSIZE) - 1) << ARB_XBC_SYNCTATTR_OFFSET;
	static const uint32_t ARB_XBC_MODE_MASK = ((1UL << ARB_XBC_MODE_BITSIZE) - 1) << ARB_XBC_MODE_OFFSET;
	
	static const uint32_t ARB_XBC_WRITE_MASK = ARB_XBC_440ICUR_MASK | ARB_XBC_440DCUW_MASK | ARB_XBC_440DCUR_MASK | ARB_XBC_PLBS1_MASK |
	                                           ARB_XBC_PLBS0_MASK | ARB_XBC_SYNCTATTR_MASK | ARB_XBC_MODE_MASK;
	
	static const uint32_t ARB_XBC_RESET_VALUE = 0x00432010UL;
};

class FIFOST_XBC_Layout
{
public:
	static const unsigned int FIFOST_XBC_FIFO_OF_RCMDQ_OFFSET = 3;
	static const unsigned int FIFOST_XBC_FIFO_UF_RCMDQ_OFFSET = 2;
	static const unsigned int FIFOST_XBC_FIFO_OF_WCMDQ_OFFSET = 1;
	static const unsigned int FIFOST_XBC_FIFO_UF_WCMDQ_OFFSET = 0;
	
	static const unsigned int FIFOST_XBC_FIFO_OF_RCMDQ_BITSIZE = 1;
	static const unsigned int FIFOST_XBC_FIFO_UF_RCMDQ_BITSIZE = 1;
	static const unsigned int FIFOST_XBC_FIFO_OF_WCMDQ_BITSIZE = 1;
	static const unsigned int FIFOST_XBC_FIFO_UF_WCMDQ_BITSIZE = 1;

	static const uint32_t FIFOST_XBC_FIFO_OF_RCMDQ_MASK = ((1UL << FIFOST_XBC_FIFO_OF_RCMDQ_BITSIZE) - 1) << FIFOST_XBC_FIFO_OF_RCMDQ_OFFSET;
	static const uint32_t FIFOST_XBC_FIFO_UF_RCMDQ_MASK = ((1UL << FIFOST_XBC_FIFO_UF_RCMDQ_BITSIZE) - 1) << FIFOST_XBC_FIFO_UF_RCMDQ_OFFSET;
	static const uint32_t FIFOST_XBC_FIFO_OF_WCMDQ_MASK = ((1UL << FIFOST_XBC_FIFO_OF_WCMDQ_BITSIZE) - 1) << FIFOST_XBC_FIFO_OF_WCMDQ_OFFSET;
	static const uint32_t FIFOST_XBC_FIFO_UF_WCMDQ_MASK = ((1UL << FIFOST_XBC_FIFO_UF_WCMDQ_BITSIZE) - 1) << FIFOST_XBC_FIFO_UF_WCMDQ_OFFSET;
	
	static const uint32_t FIFOST_XBC_CLEAR_ON_WRITE_MASK = FIFOST_XBC_FIFO_OF_RCMDQ_MASK | FIFOST_XBC_FIFO_UF_RCMDQ_MASK |
	                                                       FIFOST_XBC_FIFO_OF_WCMDQ_MASK | FIFOST_XBC_FIFO_UF_WCMDQ_MASK;
	
	static const uint32_t FIFOST_XBC_RESET_VALUE = 0x0UL;
};

class MISC_XBC_Layout
{
public:
	static const unsigned int MISC_XBC_FIFO_RCMDQ_RST_OFFSET = 1;
	static const unsigned int MISC_XBC_FIFO_WCMDQ_RST_OFFSET = 0;

	static const unsigned int MISC_XBC_FIFO_RCMDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_XBC_FIFO_WCMDQ_RST_BITSIZE = 1;

	static const uint32_t MISC_XBC_FIFO_RCMDQ_RST_MASK = ((1UL << MISC_XBC_FIFO_RCMDQ_RST_BITSIZE) - 1) << MISC_XBC_FIFO_RCMDQ_RST_OFFSET;
	static const uint32_t MISC_XBC_FIFO_WCMDQ_RST_MASK = ((1UL << MISC_XBC_FIFO_WCMDQ_RST_BITSIZE) - 1) << MISC_XBC_FIFO_WCMDQ_RST_OFFSET;
	
	static const uint32_t MISC_XBC_CLEAR_ON_WRITE_MASK = MISC_XBC_FIFO_RCMDQ_RST_MASK | MISC_XBC_FIFO_WCMDQ_RST_MASK;

	static const uint32_t MISC_XBC_WRITE_ONLY_MASK = MISC_XBC_FIFO_RCMDQ_RST_MASK | MISC_XBC_FIFO_WCMDQ_RST_MASK;

	static const uint32_t MISC_XBC_RESET_VALUE = 0x0UL;
};

class ARB_XBM_Layout
{
public:
	static const unsigned int ARB_XBM_440ICUR_OFFSET = 20;
	static const unsigned int ARB_XBM_440DCUW_OFFSET = 16;
	static const unsigned int ARB_XBM_440DCUR_OFFSET = 12;
	static const unsigned int ARB_XBM_PLBS1_OFFSET = 8;
	static const unsigned int ARB_XBM_PLBS0_OFFSET = 4;
	static const unsigned int ARB_XBM_SYNCTATTR_OFFSET = 2;
	static const unsigned int ARB_XBM_MODE_OFFSET = 0;

	static const unsigned int ARB_XBM_440ICUR_BITSIZE = 3;
	static const unsigned int ARB_XBM_440DCUW_BITSIZE = 3;
	static const unsigned int ARB_XBM_440DCUR_BITSIZE = 3;
	static const unsigned int ARB_XBM_PLBS1_BITSIZE = 3;
	static const unsigned int ARB_XBM_PLBS0_BITSIZE = 3;
	static const unsigned int ARB_XBM_SYNCTATTR_BITSIZE = 1;
	static const unsigned int ARB_XBM_MODE_BITSIZE = 2;
	
	static const uint32_t ARB_XBM_440ICUR_MASK = ((1UL << ARB_XBM_440ICUR_BITSIZE) - 1) << ARB_XBM_440ICUR_OFFSET;
	static const uint32_t ARB_XBM_440DCUW_MASK = ((1UL << ARB_XBM_440DCUW_BITSIZE) - 1) << ARB_XBM_440DCUW_OFFSET;
	static const uint32_t ARB_XBM_440DCUR_MASK = ((1UL << ARB_XBM_440DCUR_BITSIZE) - 1) << ARB_XBM_440DCUR_OFFSET;
	static const uint32_t ARB_XBM_PLBS1_MASK = ((1UL << ARB_XBM_PLBS1_BITSIZE) - 1) << ARB_XBM_PLBS1_OFFSET;
	static const uint32_t ARB_XBM_PLBS0_MASK = ((1UL << ARB_XBM_PLBS0_BITSIZE) - 1) << ARB_XBM_PLBS0_OFFSET;
	static const uint32_t ARB_XBM_SYNCTATTR_MASK = ((1UL << ARB_XBM_SYNCTATTR_BITSIZE) - 1) << ARB_XBM_SYNCTATTR_OFFSET;
	static const uint32_t ARB_XBM_MODE_MASK = ((1UL << ARB_XBM_MODE_BITSIZE) - 1) << ARB_XBM_MODE_OFFSET;
	
	static const uint32_t ARB_XBM_WRITE_MASK = ARB_XBM_440ICUR_MASK | ARB_XBM_440DCUW_MASK | ARB_XBM_440DCUR_MASK | ARB_XBM_PLBS1_MASK |
	                                           ARB_XBM_PLBS0_MASK | ARB_XBM_SYNCTATTR_MASK | ARB_XBM_MODE_MASK;
	
	static const uint32_t ARB_XBM_RESET_VALUE = 0x00432010;
};

class FIFOST_XBM_Layout
{
public:
	static const unsigned int FIFOST_XBM_FIFO_OF_RCMDQ_OFFSET = 3;
	static const unsigned int FIFOST_XBM_FIFO_UF_RCMDQ_OFFSET = 2;
	static const unsigned int FIFOST_XBM_FIFO_OF_WCMDQ_OFFSET = 1;
	static const unsigned int FIFOST_XBM_FIFO_UF_WCMDQ_OFFSET = 0;
	
	static const unsigned int FIFOST_XBM_FIFO_OF_RCMDQ_BITSIZE = 1;
	static const unsigned int FIFOST_XBM_FIFO_UF_RCMDQ_BITSIZE = 1;
	static const unsigned int FIFOST_XBM_FIFO_OF_WCMDQ_BITSIZE = 1;
	static const unsigned int FIFOST_XBM_FIFO_UF_WCMDQ_BITSIZE = 1;

	static const uint32_t FIFOST_XBM_FIFO_OF_RCMDQ_MASK = ((1UL << FIFOST_XBM_FIFO_OF_RCMDQ_BITSIZE) - 1) << FIFOST_XBM_FIFO_OF_RCMDQ_OFFSET;
	static const uint32_t FIFOST_XBM_FIFO_UF_RCMDQ_MASK = ((1UL << FIFOST_XBM_FIFO_UF_RCMDQ_BITSIZE) - 1) << FIFOST_XBM_FIFO_UF_RCMDQ_OFFSET;
	static const uint32_t FIFOST_XBM_FIFO_OF_WCMDQ_MASK = ((1UL << FIFOST_XBM_FIFO_OF_WCMDQ_BITSIZE) - 1) << FIFOST_XBM_FIFO_OF_WCMDQ_OFFSET;
	static const uint32_t FIFOST_XBM_FIFO_UF_WCMDQ_MASK = ((1UL << FIFOST_XBM_FIFO_UF_WCMDQ_BITSIZE) - 1) << FIFOST_XBM_FIFO_UF_WCMDQ_OFFSET;
	
	static const uint32_t FIFOST_XBM_CLEAR_ON_WRITE_MASK = FIFOST_XBM_FIFO_OF_RCMDQ_MASK | FIFOST_XBM_FIFO_UF_RCMDQ_MASK |
	                                                       FIFOST_XBM_FIFO_OF_WCMDQ_MASK | FIFOST_XBM_FIFO_UF_WCMDQ_MASK;
	
	static const uint32_t FIFOST_XBM_RESET_VALUE = 0x0UL;
};

class MISC_XBM_Layout
{
public:
	static const unsigned int MISC_XBM_FIFO_RCMDQ_RST_OFFSET = 1;
	static const unsigned int MISC_XBM_FIFO_WCMDQ_RST_OFFSET = 0;

	static const unsigned int MISC_XBM_FIFO_RCMDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_XBM_FIFO_WCMDQ_RST_BITSIZE = 1;

	static const uint32_t MISC_XBM_FIFO_RCMDQ_RST_MASK = ((1UL << MISC_XBM_FIFO_RCMDQ_RST_BITSIZE) - 1) << MISC_XBM_FIFO_RCMDQ_RST_OFFSET;
	static const uint32_t MISC_XBM_FIFO_WCMDQ_RST_MASK = ((1UL << MISC_XBM_FIFO_WCMDQ_RST_BITSIZE) - 1) << MISC_XBM_FIFO_WCMDQ_RST_OFFSET;
	
	static const uint32_t MISC_XBM_CLEAR_ON_WRITE_MASK = MISC_XBM_FIFO_RCMDQ_RST_MASK | MISC_XBM_FIFO_WCMDQ_RST_MASK;

	static const uint32_t MISC_XBM_WRITE_ONLY_MASK = MISC_XBM_FIFO_RCMDQ_RST_MASK | MISC_XBM_FIFO_WCMDQ_RST_MASK;

	static const uint32_t MISC_XBM_RESET_VALUE = 0x0UL;
};

class CFG_PLBS0_Layout
{
public:
	static const unsigned int CFG_PLBS0_LOCK_SESR_OFFSET = 31;
	static const unsigned int CFG_PLBS0_DMA1_EN_OFFSET = 29;
	static const unsigned int CFG_PLBS0_DMA0_EN_OFFSET = 28;
	static const unsigned int CFG_PLBS0_DMA0_PRI_OFFSET = 26;
	static const unsigned int CFG_PLBS0_DMA1_PRI_OFFSET = 24;
	static const unsigned int CFG_PLBS0_THRMCI_OFFSET = 20;
	static const unsigned int CFG_PLBS0_THRPLBM_OFFSET = 16;
	static const unsigned int CFG_PLBS0_THWMCI_OFFSET = 12;
	static const unsigned int CFG_PLBS0_THWPLBM_OFFSET = 8;
	static const unsigned int CFG_PLBS0_LOCKXFER_OFFSET = 6;
	static const unsigned int CFG_PLBS0_RPIPE_OFFSET = 5;
	static const unsigned int CFG_PLBS0_WPIPE_OFFSET = 4;
	static const unsigned int CFG_PLBS0_WPOST_OFFSET = 3;
	static const unsigned int CFG_PLBS0_AERR_LOG_OFFSET = 1;
	static const unsigned int CFG_PLBS0_CMD_CHK_DBL_OFFSET = 0;

	static const unsigned int CFG_PLBS0_LOCK_SESR_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_DMA1_EN_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_DMA0_EN_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_DMA0_PRI_BITSIZE = 2;
	static const unsigned int CFG_PLBS0_DMA1_PRI_BITSIZE = 2;
	static const unsigned int CFG_PLBS0_THRMCI_BITSIZE = 3;
	static const unsigned int CFG_PLBS0_THRPLBM_BITSIZE = 3;
	static const unsigned int CFG_PLBS0_THWMCI_BITSIZE = 3;
	static const unsigned int CFG_PLBS0_THWPLBM_BITSIZE = 3;
	static const unsigned int CFG_PLBS0_LOCKXFER_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_RPIPE_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_WPIPE_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_WPOST_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_AERR_LOG_BITSIZE = 1;
	static const unsigned int CFG_PLBS0_CMD_CHK_DBL_BITSIZE = 1;
	
	static const uint32_t CFG_PLBS0_LOCK_SESR_MASK = ((1UL << CFG_PLBS0_LOCK_SESR_BITSIZE) - 1) << CFG_PLBS0_LOCK_SESR_OFFSET;
	static const uint32_t CFG_PLBS0_DMA1_EN_MASK = ((1UL << CFG_PLBS0_DMA1_EN_BITSIZE) - 1) << CFG_PLBS0_DMA1_EN_OFFSET;
	static const uint32_t CFG_PLBS0_DMA0_EN_MASK = ((1UL << CFG_PLBS0_DMA0_EN_BITSIZE) - 1) << CFG_PLBS0_DMA0_EN_OFFSET;
	static const uint32_t CFG_PLBS0_DMA0_PRI_MASK = ((1UL << CFG_PLBS0_DMA0_PRI_BITSIZE) - 1) << CFG_PLBS0_DMA0_PRI_OFFSET;
	static const uint32_t CFG_PLBS0_DMA1_PRI_MASK = ((1UL << CFG_PLBS0_DMA1_PRI_BITSIZE) - 1) << CFG_PLBS0_DMA1_PRI_OFFSET;
	static const uint32_t CFG_PLBS0_THRMCI_MASK = ((1UL << CFG_PLBS0_THRMCI_BITSIZE) - 1) << CFG_PLBS0_THRMCI_OFFSET;
	static const uint32_t CFG_PLBS0_THRPLBM_MASK = ((1UL << CFG_PLBS0_THRPLBM_BITSIZE) - 1) << CFG_PLBS0_THRPLBM_OFFSET;
	static const uint32_t CFG_PLBS0_THWMCI_MASK = ((1UL << CFG_PLBS0_THWMCI_BITSIZE) - 1) << CFG_PLBS0_THWMCI_OFFSET;
	static const uint32_t CFG_PLBS0_THWPLBM_MASK = ((1UL << CFG_PLBS0_THWPLBM_BITSIZE) - 1) << CFG_PLBS0_THWPLBM_OFFSET;
	static const uint32_t CFG_PLBS0_LOCKXFER_MASK = ((1UL << CFG_PLBS0_LOCKXFER_BITSIZE) - 1) << CFG_PLBS0_LOCKXFER_OFFSET;
	static const uint32_t CFG_PLBS0_RPIPE_MASK = ((1UL << CFG_PLBS0_RPIPE_BITSIZE) - 1) << CFG_PLBS0_RPIPE_OFFSET;
	static const uint32_t CFG_PLBS0_WPIPE_MASK = ((1UL << CFG_PLBS0_WPIPE_BITSIZE) - 1) << CFG_PLBS0_WPIPE_OFFSET;
	static const uint32_t CFG_PLBS0_WPOST_MASK = ((1UL << CFG_PLBS0_WPOST_BITSIZE) - 1) << CFG_PLBS0_WPOST_OFFSET;
	static const uint32_t CFG_PLBS0_AERR_LOG_MASK = ((1UL << CFG_PLBS0_AERR_LOG_BITSIZE) - 1) << CFG_PLBS0_AERR_LOG_OFFSET;
	static const uint32_t CFG_PLBS0_CMD_CHK_DBL_MASK = ((1UL << CFG_PLBS0_CMD_CHK_DBL_BITSIZE) - 1) << CFG_PLBS0_CMD_CHK_DBL_OFFSET;
	
	static const uint32_t CFG_PLBS0_WRITE_MASK = CFG_PLBS0_LOCK_SESR_MASK | CFG_PLBS0_DMA1_EN_MASK | CFG_PLBS0_DMA0_EN_MASK |
	                                             CFG_PLBS0_DMA0_PRI_MASK | CFG_PLBS0_DMA1_PRI_MASK | CFG_PLBS0_THRMCI_MASK |
	                                             CFG_PLBS0_THRPLBM_MASK | CFG_PLBS0_THWMCI_MASK | CFG_PLBS0_THWPLBM_MASK |
	                                             CFG_PLBS0_LOCKXFER_MASK | CFG_PLBS0_RPIPE_MASK | CFG_PLBS0_WPIPE_MASK |
	                                             CFG_PLBS0_WPOST_MASK | CFG_PLBS0_AERR_LOG_MASK | CFG_PLBS0_CMD_CHK_DBL_MASK;
	
	static const uint32_t CFG_PLBS0_RESET_VALUE = 0x8033336c;
};

class SEAR_U_PLBS0_Layout
{
public:
	static const unsigned int SEAR_U_PLBS0_U4BIT_OFFSET = 0;

	static const unsigned int SEAR_U_PLBS0_U4BIT_BITSIZE = 4;

	static const uint32_t SEAR_U_PLBS0_U4BIT_MASK = ((1UL << SEAR_U_PLBS0_U4BIT_BITSIZE) - 1) << SEAR_U_PLBS0_U4BIT_OFFSET;
	
	static const uint32_t SEAR_U_PLBS0_MASK = SEAR_U_PLBS0_U4BIT_MASK;

	static const uint32_t SEAR_U_PLBS0_WRITE_MASK = SEAR_U_PLBS0_U4BIT_MASK;

	static const uint32_t SEAR_U_PLBS0_RESET_VALUE = 0x0UL;
};

class SEAR_L_PLBS0_Layout
{
public:
	static const unsigned int SEAR_L_PLBS0_L32BIT_OFFSET = 0;

	static const unsigned int SEAR_L_PLBS0_L32BIT_BITSIZE = 32;

	static const uint32_t SEAR_L_PLBS0_L32BIT_MASK = 0xffffffffUL; //((1UL << SEAR_L_PLBS0_L32BIT_BITSIZE) - 1) << SEAR_L_PLBS0_L32BIT_OFFSET;
	
	static const uint32_t SEAR_L_PLBS0_MASK = SEAR_L_PLBS0_L32BIT_MASK;

	static const uint32_t SEAR_L_PLBS0_WRITE_MASK = SEAR_L_PLBS0_L32BIT_MASK;

	static const uint32_t SEAR_L_PLBS0_RESET_VALUE = 0x0UL;
};

class SESR_PLBS0_Layout
{
public:
	static const unsigned int SESR_PLBS0_VLD_OFFSET = 31;
	static const unsigned int SESR_PLBS0_LOCKERR_OFFSET = 30;
	static const unsigned int SESR_PLBS0_MID_OFFSET = 26;
	static const unsigned int SESR_PLBS0_MSIZE_OFFSET = 24;
	static const unsigned int SESR_PLBS0_TYPE_OFFSET = 21;
	static const unsigned int SESR_PLBS0_RNW_OFFSET = 20;
	static const unsigned int SESR_PLBS0_SIZE_OFFSET = 16;
	static const unsigned int SESR_PLBS0_BE_OFFSET = 0;

	static const unsigned int SESR_PLBS0_VLD_BITSIZE = 1;
	static const unsigned int SESR_PLBS0_LOCKERR_BITSIZE = 1;
	static const unsigned int SESR_PLBS0_MID_BITSIZE = 2;
	static const unsigned int SESR_PLBS0_MSIZE_BITSIZE = 2;
	static const unsigned int SESR_PLBS0_TYPE_BITSIZE = 3;
	static const unsigned int SESR_PLBS0_RNW_BITSIZE = 1;
	static const unsigned int SESR_PLBS0_SIZE_BITSIZE = 4;
	static const unsigned int SESR_PLBS0_BE_BITSIZE = 16;
	
	static const uint32_t SESR_PLBS0_VLD_MASK = ((1UL << SESR_PLBS0_VLD_BITSIZE) - 1) << SESR_PLBS0_VLD_OFFSET;
	static const uint32_t SESR_PLBS0_LOCKERR_MASK = ((1UL << SESR_PLBS0_LOCKERR_BITSIZE) - 1) << SESR_PLBS0_LOCKERR_OFFSET;
	static const uint32_t SESR_PLBS0_MID_MASK = ((1UL << SESR_PLBS0_MID_BITSIZE) - 1) << SESR_PLBS0_MID_OFFSET;
	static const uint32_t SESR_PLBS0_MSIZE_MASK = ((1UL << SESR_PLBS0_MSIZE_BITSIZE) - 1) << SESR_PLBS0_MSIZE_OFFSET;
	static const uint32_t SESR_PLBS0_TYPE_MASK = ((1UL << SESR_PLBS0_TYPE_BITSIZE) - 1) << SESR_PLBS0_TYPE_OFFSET;
	static const uint32_t SESR_PLBS0_RNW_MASK = ((1UL << SESR_PLBS0_RNW_BITSIZE) - 1) << SESR_PLBS0_RNW_OFFSET;
	static const uint32_t SESR_PLBS0_SIZE_MASK = ((1UL << SESR_PLBS0_SIZE_BITSIZE) - 1) << SESR_PLBS0_SIZE_OFFSET;
	static const uint32_t SESR_PLBS0_BE_MASK = ((1UL << SESR_PLBS0_BE_BITSIZE) - 1) << SESR_PLBS0_BE_OFFSET;
	
	static const uint32_t SESR_PLBS0_CLEAR_ON_WRITE_MASK = SESR_PLBS0_VLD_MASK | SESR_PLBS0_LOCKERR_MASK | SESR_PLBS0_MID_MASK |
	                                                       SESR_PLBS0_MSIZE_MASK | SESR_PLBS0_TYPE_MASK | SESR_PLBS0_RNW_MASK |
	                                                       SESR_PLBS0_SIZE_MASK | SESR_PLBS0_BE_MASK;

	static const uint32_t SESR_PLBS0_RESET_VALUE = 0x0UL;
};

class MISC_ST_PLBS0_Layout
{
public:
	static const unsigned int MISC_ST_PLBS0_WPOST_CFG_ERR_OFFSET = 31;
	static const unsigned int MISC_ST_PLBS0_ILLEGAL_CMD_OFFSET = 30;
	static const unsigned int MISC_ST_PLBS0_ADDR_ERR_OFFSET = 29;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_RDAT_OFFSET = 13;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_RDAT_OFFSET = 12;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_WDAT_OFFSET = 11;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_WDAT_OFFSET = 10;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_SRDQ_OFFSET = 9;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_SRDQ_OFFSET = 8;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_SWRQ_OFFSET = 7;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_SWRQ_OFFSET = 6;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_MRDQ_OFFSET = 5;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_MRDQ_OFFSET = 4;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_MWRQ_OFFSET = 3;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_MWRQ_OFFSET = 2;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_INCMD_OFFSET = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_INCMD_OFFSET = 0;

	static const unsigned int MISC_ST_PLBS0_WPOST_CFG_ERR_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_ILLEGAL_CMD_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_ADDR_ERR_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_RDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_RDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_WDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_WDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_SRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_SRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_SWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_SWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_MRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_MRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_MWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_MWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_OF_INCMD_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS0_FIFO_UF_INCMD_BITSIZE = 1;

	static const uint32_t MISC_ST_PLBS0_WPOST_CFG_ERR_MASK = ((1UL << MISC_ST_PLBS0_WPOST_CFG_ERR_BITSIZE) - 1) << MISC_ST_PLBS0_WPOST_CFG_ERR_OFFSET;
	static const uint32_t MISC_ST_PLBS0_ILLEGAL_CMD_MASK = ((1UL << MISC_ST_PLBS0_ILLEGAL_CMD_BITSIZE) - 1) << MISC_ST_PLBS0_ILLEGAL_CMD_OFFSET;
	static const uint32_t MISC_ST_PLBS0_ADDR_ERR_MASK = ((1UL << MISC_ST_PLBS0_ADDR_ERR_BITSIZE) - 1) << MISC_ST_PLBS0_ADDR_ERR_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_RDAT_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_RDAT_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_RDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_RDAT_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_RDAT_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_RDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_WDAT_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_WDAT_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_WDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_WDAT_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_WDAT_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_WDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_SRDQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_SRDQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_SRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_SRDQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_SRDQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_SRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_SWRQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_SWRQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_SWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_SWRQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_SWRQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_SWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_MRDQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_MRDQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_MRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_MRDQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_MRDQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_MRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_MWRQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_MWRQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_MWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_MWRQ_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_MWRQ_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_MWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_OF_INCMD_MASK = ((1UL << MISC_ST_PLBS0_FIFO_OF_INCMD_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_OF_INCMD_OFFSET;
	static const uint32_t MISC_ST_PLBS0_FIFO_UF_INCMD_MASK = ((1UL << MISC_ST_PLBS0_FIFO_UF_INCMD_BITSIZE) - 1) << MISC_ST_PLBS0_FIFO_UF_INCMD_OFFSET;

	static const uint32_t MISC_ST_PLBS0_MASK = MISC_ST_PLBS0_WPOST_CFG_ERR_MASK | MISC_ST_PLBS0_ILLEGAL_CMD_MASK | MISC_ST_PLBS0_ADDR_ERR_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_RDAT_MASK | MISC_ST_PLBS0_FIFO_UF_RDAT_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_WDAT_MASK | MISC_ST_PLBS0_FIFO_UF_WDAT_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_SRDQ_MASK | MISC_ST_PLBS0_FIFO_UF_SRDQ_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_SWRQ_MASK | MISC_ST_PLBS0_FIFO_UF_SWRQ_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_MRDQ_MASK | MISC_ST_PLBS0_FIFO_UF_MRDQ_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_MWRQ_MASK | MISC_ST_PLBS0_FIFO_UF_MWRQ_MASK |
	                                           MISC_ST_PLBS0_FIFO_OF_INCMD_MASK | MISC_ST_PLBS0_FIFO_UF_INCMD_MASK;
	
	static const uint32_t MISC_ST_PLBS0_CLEAR_ON_WRITE_MASK = MISC_ST_PLBS0_WPOST_CFG_ERR_MASK | MISC_ST_PLBS0_ILLEGAL_CMD_MASK | MISC_ST_PLBS0_ADDR_ERR_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_RDAT_MASK | MISC_ST_PLBS0_FIFO_UF_RDAT_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_WDAT_MASK | MISC_ST_PLBS0_FIFO_UF_WDAT_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_SRDQ_MASK | MISC_ST_PLBS0_FIFO_UF_SRDQ_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_SWRQ_MASK | MISC_ST_PLBS0_FIFO_UF_SWRQ_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_MRDQ_MASK | MISC_ST_PLBS0_FIFO_UF_MRDQ_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_MWRQ_MASK | MISC_ST_PLBS0_FIFO_UF_MWRQ_MASK |
	                                                          MISC_ST_PLBS0_FIFO_OF_INCMD_MASK | MISC_ST_PLBS0_FIFO_UF_INCMD_MASK;

	static const uint32_t MISC_ST_PLBS0_RESET_VALUE = 0x0UL;
};

class PLBERR_ST_PLBS0_Layout
{
public:
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M0_MIRQ_OFFSET = 3;
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M1_MIRQ_OFFSET = 2;
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M2_MIRQ_OFFSET = 1;
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M3_MIRQ_OFFSET = 0;

	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M0_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M1_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M2_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBS0_PLBS0_M3_MIRQ_BITSIZE = 1;

	static const uint32_t PLBERR_ST_PLBS0_PLBS0_M0_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS0_PLBS0_M0_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS0_PLBS0_M0_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBS0_PLBS0_M1_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS0_PLBS0_M1_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS0_PLBS0_M1_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBS0_PLBS0_M2_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS0_PLBS0_M2_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS0_PLBS0_M2_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBS0_PLBS0_M3_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS0_PLBS0_M3_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS0_PLBS0_M3_MIRQ_OFFSET;
	
	static const uint32_t PLBERR_ST_PLBS0_CLEAR_ON_WRITE_MASK = PLBERR_ST_PLBS0_PLBS0_M0_MIRQ_MASK | PLBERR_ST_PLBS0_PLBS0_M1_MIRQ_MASK |
	                                                            PLBERR_ST_PLBS0_PLBS0_M2_MIRQ_MASK | PLBERR_ST_PLBS0_PLBS0_M3_MIRQ_MASK;

	static const uint32_t PLBERR_ST_PLBS0_RESET_VALUE = 0x0UL;
};

class MISC_PLBS0_Layout
{
public:
	static const unsigned int MISC_PLBS0_MODE_128N64_OFFSET = 31;
	static const unsigned int MISC_PLBS0_FIFO_RDAT_RST_OFFSET = 6;
	static const unsigned int MISC_PLBS0_FIFO_WDAT_RST_OFFSET = 5;
	static const unsigned int MISC_PLBS0_FIFO_SRDQ_RST_OFFSET = 4;
	static const unsigned int MISC_PLBS0_FIFO_SWRQ_RST_OFFSET = 3;
	static const unsigned int MISC_PLBS0_FIFO_MRDQ_RST_OFFSET = 2;
	static const unsigned int MISC_PLBS0_FIFO_MWRQ_RST_OFFSET = 1;
	static const unsigned int MISC_PLBS0_FIFO_INCMD_RST_OFFSET = 0;

	static const unsigned int MISC_PLBS0_MODE_128N64_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_RDAT_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_WDAT_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_SRDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_SWRQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_MRDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_MWRQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS0_FIFO_INCMD_RST_BITSIZE = 1;

	static const uint32_t MISC_PLBS0_MODE_128N64_MASK = ((1UL << MISC_PLBS0_MODE_128N64_BITSIZE) - 1) << MISC_PLBS0_MODE_128N64_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_RDAT_RST_MASK = ((1UL << MISC_PLBS0_FIFO_RDAT_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_RDAT_RST_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_WDAT_RST_MASK = ((1UL << MISC_PLBS0_FIFO_WDAT_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_WDAT_RST_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_SRDQ_RST_MASK = ((1UL << MISC_PLBS0_FIFO_SRDQ_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_SRDQ_RST_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_SWRQ_RST_MASK = ((1UL << MISC_PLBS0_FIFO_SWRQ_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_SWRQ_RST_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_MRDQ_RST_MASK = ((1UL << MISC_PLBS0_FIFO_MRDQ_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_MRDQ_RST_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_MWRQ_RST_MASK = ((1UL << MISC_PLBS0_FIFO_MWRQ_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_MWRQ_RST_OFFSET;
	static const uint32_t MISC_PLBS0_FIFO_INCMD_RST_MASK = ((1UL << MISC_PLBS0_FIFO_INCMD_RST_BITSIZE) - 1) << MISC_PLBS0_FIFO_INCMD_RST_OFFSET;
	
	static const uint32_t MISC_PLBS0_CLEAR_ON_WRITE_MASK = MISC_PLBS0_FIFO_RDAT_RST_MASK | MISC_PLBS0_FIFO_WDAT_RST_MASK |
	                                                       MISC_PLBS0_FIFO_SRDQ_RST_MASK | MISC_PLBS0_FIFO_SWRQ_RST_MASK | MISC_PLBS0_FIFO_MRDQ_RST_MASK |
	                                                       MISC_PLBS0_FIFO_MWRQ_RST_MASK | MISC_PLBS0_FIFO_INCMD_RST_MASK;
	
	static const uint32_t MISC_PLBS0_WRITE_ONLY_MASK = MISC_PLBS0_FIFO_RDAT_RST_MASK | MISC_PLBS0_FIFO_WDAT_RST_MASK |
	                                                   MISC_PLBS0_FIFO_SRDQ_RST_MASK | MISC_PLBS0_FIFO_SWRQ_RST_MASK | MISC_PLBS0_FIFO_MRDQ_RST_MASK |
	                                                   MISC_PLBS0_FIFO_MWRQ_RST_MASK | MISC_PLBS0_FIFO_INCMD_RST_MASK;
	
	static const uint32_t MISC_PLBS0_RESET_VALUE = 0x80000000UL;
};

class CMD_SNIFF_PLBS0_Layout
{
public:
	static const unsigned int CMD_SNIFF_PLBS0_ENABLE_OFFSET = 31;
	static const unsigned int CMD_SNIFF_PLBS0_SIZE_OFFSET = 24;
	static const unsigned int CMD_SNIFF_PLBS0_RNW_OFFSET = 23;
	static const unsigned int CMD_SNIFF_PLBS0_MID_OFFSET = 20;
	static const unsigned int CMD_SNIFF_PLBS0_SPLBNDMA_OFFSET = 19;
	static const unsigned int CMD_SNIFF_PLBS0_SPLB_MID_OFFSET = 16;
	static const unsigned int CMD_SNIFF_PLBS0_SSIZE_OFFSET = 14;
	static const unsigned int CMD_SNIFF_PLBS0_SIZE_EN_OFFSET = 6;
	static const unsigned int CMD_SNIFF_PLBS0_RNW_EN_OFFSET = 5;
	static const unsigned int CMD_SNIFF_PLBS0_MID_EN_OFFSET = 4;
	static const unsigned int CMD_SNIFF_PLBS0_SSIZE_EN_OFFSET = 1;
	static const unsigned int CMD_SNIFF_PLBS0_ADDR_EN_OFFSET = 0;

	static const unsigned int CMD_SNIFF_PLBS0_ENABLE_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_SIZE_BITSIZE = 4;
	static const unsigned int CMD_SNIFF_PLBS0_RNW_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_MID_BITSIZE = 3;
	static const unsigned int CMD_SNIFF_PLBS0_SPLBNDMA_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_SPLB_MID_BITSIZE = 2;
	static const unsigned int CMD_SNIFF_PLBS0_SSIZE_BITSIZE = 2;
	static const unsigned int CMD_SNIFF_PLBS0_SIZE_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_RNW_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_MID_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_SSIZE_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS0_ADDR_EN_BITSIZE = 1;

	static const uint32_t CMD_SNIFF_PLBS0_ENABLE_MASK = ((1UL << CMD_SNIFF_PLBS0_ENABLE_BITSIZE) - 1) << CMD_SNIFF_PLBS0_ENABLE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_SIZE_MASK = ((1UL << CMD_SNIFF_PLBS0_SIZE_BITSIZE) - 1) << CMD_SNIFF_PLBS0_SIZE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_RNW_MASK = ((1UL << CMD_SNIFF_PLBS0_RNW_BITSIZE) - 1) << CMD_SNIFF_PLBS0_RNW_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_MID_MASK = ((1UL << CMD_SNIFF_PLBS0_MID_BITSIZE) - 1) << CMD_SNIFF_PLBS0_MID_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_SPLBNDMA_MASK = ((1UL << CMD_SNIFF_PLBS0_SPLBNDMA_BITSIZE) - 1) << CMD_SNIFF_PLBS0_SPLBNDMA_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_SPLB_MID_MASK = ((1UL << CMD_SNIFF_PLBS0_SPLB_MID_BITSIZE) - 1) << CMD_SNIFF_PLBS0_SPLB_MID_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_SSIZE_MASK = ((1UL << CMD_SNIFF_PLBS0_SSIZE_BITSIZE) - 1) << CMD_SNIFF_PLBS0_SSIZE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_SIZE_EN_MASK = ((1UL << CMD_SNIFF_PLBS0_SIZE_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS0_SIZE_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_RNW_EN_MASK = ((1UL << CMD_SNIFF_PLBS0_RNW_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS0_RNW_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_MID_EN_MASK = ((1UL << CMD_SNIFF_PLBS0_MID_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS0_MID_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_SSIZE_EN_MASK = ((1UL << CMD_SNIFF_PLBS0_SSIZE_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS0_SSIZE_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS0_ADDR_EN_MASK = ((1UL << CMD_SNIFF_PLBS0_ADDR_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS0_ADDR_EN_OFFSET;
	
	static const uint32_t CMD_SNIFF_PLBS0_WRITE_MASK = CMD_SNIFF_PLBS0_ENABLE_MASK | CMD_SNIFF_PLBS0_SIZE_MASK | CMD_SNIFF_PLBS0_RNW_MASK |
	                                                   CMD_SNIFF_PLBS0_MID_MASK | CMD_SNIFF_PLBS0_SPLBNDMA_MASK | CMD_SNIFF_PLBS0_SPLB_MID_MASK |
	                                                   CMD_SNIFF_PLBS0_SSIZE_MASK | CMD_SNIFF_PLBS0_SIZE_EN_MASK | CMD_SNIFF_PLBS0_RNW_EN_MASK |
	                                                   CMD_SNIFF_PLBS0_MID_EN_MASK | CMD_SNIFF_PLBS0_SSIZE_EN_MASK | CMD_SNIFF_PLBS0_ADDR_EN_MASK;
	
	static const uint32_t CMD_SNIFF_PLBS0_RESET_VALUE = 0x0;
};

class CFG_PLBS1_Layout
{
public:
	static const unsigned int CFG_PLBS1_LOCK_SESR_OFFSET = 31;
	static const unsigned int CFG_PLBS1_DMA3_EN_OFFSET = 29;
	static const unsigned int CFG_PLBS1_DMA2_EN_OFFSET = 28;
	static const unsigned int CFG_PLBS1_DMA2_PRI_OFFSET = 26;
	static const unsigned int CFG_PLBS1_DMA3_PRI_OFFSET = 24;
	static const unsigned int CFG_PLBS1_THRMCI_OFFSET = 20;
	static const unsigned int CFG_PLBS1_THRPLBM_OFFSET = 16;
	static const unsigned int CFG_PLBS1_THWMCI_OFFSET = 12;
	static const unsigned int CFG_PLBS1_THWPLBM_OFFSET = 8;
	static const unsigned int CFG_PLBS1_LOCKXFER_OFFSET = 6;
	static const unsigned int CFG_PLBS1_RPIPE_OFFSET = 5;
	static const unsigned int CFG_PLBS1_WPIPE_OFFSET = 4;
	static const unsigned int CFG_PLBS1_WPOST_OFFSET = 3;
	static const unsigned int CFG_PLBS1_AERR_LOG_OFFSET = 1;
	static const unsigned int CFG_PLBS1_CMD_CHK_DBL_OFFSET = 0;

	static const unsigned int CFG_PLBS1_LOCK_SESR_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_DMA3_EN_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_DMA2_EN_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_DMA2_PRI_BITSIZE = 2;
	static const unsigned int CFG_PLBS1_DMA3_PRI_BITSIZE = 2;
	static const unsigned int CFG_PLBS1_THRMCI_BITSIZE = 3;
	static const unsigned int CFG_PLBS1_THRPLBM_BITSIZE = 3;
	static const unsigned int CFG_PLBS1_THWMCI_BITSIZE = 3;
	static const unsigned int CFG_PLBS1_THWPLBM_BITSIZE = 3;
	static const unsigned int CFG_PLBS1_LOCKXFER_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_RPIPE_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_WPIPE_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_WPOST_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_AERR_LOG_BITSIZE = 1;
	static const unsigned int CFG_PLBS1_CMD_CHK_DBL_BITSIZE = 1;
	
	static const uint32_t CFG_PLBS1_LOCK_SESR_MASK = ((1UL << CFG_PLBS1_LOCK_SESR_BITSIZE) - 1) << CFG_PLBS1_LOCK_SESR_OFFSET;
	static const uint32_t CFG_PLBS1_DMA3_EN_MASK = ((1UL << CFG_PLBS1_DMA3_EN_BITSIZE) - 1) << CFG_PLBS1_DMA3_EN_OFFSET;
	static const uint32_t CFG_PLBS1_DMA2_EN_MASK = ((1UL << CFG_PLBS1_DMA2_EN_BITSIZE) - 1) << CFG_PLBS1_DMA2_EN_OFFSET;
	static const uint32_t CFG_PLBS1_DMA2_PRI_MASK = ((1UL << CFG_PLBS1_DMA2_PRI_BITSIZE) - 1) << CFG_PLBS1_DMA2_PRI_OFFSET;
	static const uint32_t CFG_PLBS1_DMA3_PRI_MASK = ((1UL << CFG_PLBS1_DMA3_PRI_BITSIZE) - 1) << CFG_PLBS1_DMA3_PRI_OFFSET;
	static const uint32_t CFG_PLBS1_THRMCI_MASK = ((1UL << CFG_PLBS1_THRMCI_BITSIZE) - 1) << CFG_PLBS1_THRMCI_OFFSET;
	static const uint32_t CFG_PLBS1_THRPLBM_MASK = ((1UL << CFG_PLBS1_THRPLBM_BITSIZE) - 1) << CFG_PLBS1_THRPLBM_OFFSET;
	static const uint32_t CFG_PLBS1_THWMCI_MASK = ((1UL << CFG_PLBS1_THWMCI_BITSIZE) - 1) << CFG_PLBS1_THWMCI_OFFSET;
	static const uint32_t CFG_PLBS1_THWPLBM_MASK = ((1UL << CFG_PLBS1_THWPLBM_BITSIZE) - 1) << CFG_PLBS1_THWPLBM_OFFSET;
	static const uint32_t CFG_PLBS1_LOCKXFER_MASK = ((1UL << CFG_PLBS1_LOCKXFER_BITSIZE) - 1) << CFG_PLBS1_LOCKXFER_OFFSET;
	static const uint32_t CFG_PLBS1_RPIPE_MASK = ((1UL << CFG_PLBS1_RPIPE_BITSIZE) - 1) << CFG_PLBS1_RPIPE_OFFSET;
	static const uint32_t CFG_PLBS1_WPIPE_MASK = ((1UL << CFG_PLBS1_WPIPE_BITSIZE) - 1) << CFG_PLBS1_WPIPE_OFFSET;
	static const uint32_t CFG_PLBS1_WPOST_MASK = ((1UL << CFG_PLBS1_WPOST_BITSIZE) - 1) << CFG_PLBS1_WPOST_OFFSET;
	static const uint32_t CFG_PLBS1_AERR_LOG_MASK = ((1UL << CFG_PLBS1_AERR_LOG_BITSIZE) - 1) << CFG_PLBS1_AERR_LOG_OFFSET;
	static const uint32_t CFG_PLBS1_CMD_CHK_DBL_MASK = ((1UL << CFG_PLBS1_CMD_CHK_DBL_BITSIZE) - 1) << CFG_PLBS1_CMD_CHK_DBL_OFFSET;
	
	static const uint32_t CFG_PLBS1_WRITE_MASK = CFG_PLBS1_LOCK_SESR_MASK | CFG_PLBS1_DMA3_EN_MASK | CFG_PLBS1_DMA2_EN_MASK |
	                                             CFG_PLBS1_DMA2_PRI_MASK | CFG_PLBS1_DMA3_PRI_MASK | CFG_PLBS1_THRMCI_MASK |
	                                             CFG_PLBS1_THRPLBM_MASK | CFG_PLBS1_THWMCI_MASK | CFG_PLBS1_THWPLBM_MASK |
	                                             CFG_PLBS1_LOCKXFER_MASK | CFG_PLBS1_RPIPE_MASK | CFG_PLBS1_WPIPE_MASK |
	                                             CFG_PLBS1_WPOST_MASK | CFG_PLBS1_AERR_LOG_MASK | CFG_PLBS1_CMD_CHK_DBL_MASK;
	
	static const uint32_t CFG_PLBS1_RESET_VALUE = 0x8033336c;
};

class SEAR_U_PLBS1_Layout
{
public:
	static const unsigned int SEAR_U_PLBS1_U4BIT_OFFSET = 0;

	static const unsigned int SEAR_U_PLBS1_U4BIT_BITSIZE = 4;

	static const uint32_t SEAR_U_PLBS1_U4BIT_MASK = ((1UL << SEAR_U_PLBS1_U4BIT_BITSIZE) - 1) << SEAR_U_PLBS1_U4BIT_OFFSET;
	
	static const uint32_t SEAR_U_PLBS1_MASK = SEAR_U_PLBS1_U4BIT_MASK;

	static const uint32_t SEAR_U_PLBS1_WRITE_MASK = SEAR_U_PLBS1_U4BIT_MASK;
	
	static const uint32_t SEAR_U_PLBS1_RESET_VALUE = 0x0UL;
};

class SEAR_L_PLBS1_Layout
{
public:
	static const unsigned int SEAR_L_PLBS1_L32BIT_OFFSET = 0;

	static const unsigned int SEAR_L_PLBS1_L32BIT_BITSIZE = 32;

	static const uint32_t SEAR_L_PLBS1_L32BIT_MASK = 0xffffffffUL; //((1UL << SEAR_L_PLBS1_L32BIT_BITSIZE) - 1) << SEAR_L_PLBS1_L32BIT_OFFSET;
	
	static const uint32_t SEAR_L_PLBS1_MASK = SEAR_L_PLBS1_L32BIT_MASK;

	static const uint32_t SEAR_L_PLBS1_WRITE_MASK = SEAR_L_PLBS1_L32BIT_MASK;

	static const uint32_t SEAR_L_PLBS1_RESET_VALUE = 0x0UL;
};

class SESR_PLBS1_Layout
{
public:
	static const unsigned int SESR_PLBS1_VLD_OFFSET = 31;
	static const unsigned int SESR_PLBS1_LOCKERR_OFFSET = 30;
	static const unsigned int SESR_PLBS1_MID_OFFSET = 26;
	static const unsigned int SESR_PLBS1_MSIZE_OFFSET = 24;
	static const unsigned int SESR_PLBS1_TYPE_OFFSET = 21;
	static const unsigned int SESR_PLBS1_RNW_OFFSET = 20;
	static const unsigned int SESR_PLBS1_SIZE_OFFSET = 16;
	static const unsigned int SESR_PLBS1_BE_OFFSET = 0;

	static const unsigned int SESR_PLBS1_VLD_BITSIZE = 1;
	static const unsigned int SESR_PLBS1_LOCKERR_BITSIZE = 1;
	static const unsigned int SESR_PLBS1_MID_BITSIZE = 2;
	static const unsigned int SESR_PLBS1_MSIZE_BITSIZE = 2;
	static const unsigned int SESR_PLBS1_TYPE_BITSIZE = 3;
	static const unsigned int SESR_PLBS1_RNW_BITSIZE = 1;
	static const unsigned int SESR_PLBS1_SIZE_BITSIZE = 4;
	static const unsigned int SESR_PLBS1_BE_BITSIZE = 16;
	
	static const uint32_t SESR_PLBS1_VLD_MASK = ((1UL << SESR_PLBS1_VLD_BITSIZE) - 1) << SESR_PLBS1_VLD_OFFSET;
	static const uint32_t SESR_PLBS1_LOCKERR_MASK = ((1UL << SESR_PLBS1_LOCKERR_BITSIZE) - 1) << SESR_PLBS1_LOCKERR_OFFSET;
	static const uint32_t SESR_PLBS1_MID_MASK = ((1UL << SESR_PLBS1_MID_BITSIZE) - 1) << SESR_PLBS1_MID_OFFSET;
	static const uint32_t SESR_PLBS1_MSIZE_MASK = ((1UL << SESR_PLBS1_MSIZE_BITSIZE) - 1) << SESR_PLBS1_MSIZE_OFFSET;
	static const uint32_t SESR_PLBS1_TYPE_MASK = ((1UL << SESR_PLBS1_TYPE_BITSIZE) - 1) << SESR_PLBS1_TYPE_OFFSET;
	static const uint32_t SESR_PLBS1_RNW_MASK = ((1UL << SESR_PLBS1_RNW_BITSIZE) - 1) << SESR_PLBS1_RNW_OFFSET;
	static const uint32_t SESR_PLBS1_SIZE_MASK = ((1UL << SESR_PLBS1_SIZE_BITSIZE) - 1) << SESR_PLBS1_SIZE_OFFSET;
	static const uint32_t SESR_PLBS1_BE_MASK = ((1UL << SESR_PLBS1_BE_BITSIZE) - 1) << SESR_PLBS1_BE_OFFSET;
	
	static const uint32_t SESR_PLBS1_CLEAR_ON_WRITE_MASK = SESR_PLBS1_VLD_MASK | SESR_PLBS1_LOCKERR_MASK | SESR_PLBS1_MID_MASK |
	                                                       SESR_PLBS1_MSIZE_MASK | SESR_PLBS1_TYPE_MASK | SESR_PLBS1_RNW_MASK |
	                                                       SESR_PLBS1_SIZE_MASK | SESR_PLBS1_BE_MASK;

	static const uint32_t SESR_PLBS1_RESET_VALUE = 0x0UL;
};

class MISC_ST_PLBS1_Layout
{
public:
	static const unsigned int MISC_ST_PLBS1_WPOST_CFG_ERR_OFFSET = 31;
	static const unsigned int MISC_ST_PLBS1_ILLEGAL_CMD_OFFSET = 30;
	static const unsigned int MISC_ST_PLBS1_ADDR_ERR_OFFSET = 29;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_RDAT_OFFSET = 13;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_RDAT_OFFSET = 12;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_WDAT_OFFSET = 11;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_WDAT_OFFSET = 10;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_SRDQ_OFFSET = 9;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_SRDQ_OFFSET = 8;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_SWRQ_OFFSET = 7;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_SWRQ_OFFSET = 6;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_MRDQ_OFFSET = 5;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_MRDQ_OFFSET = 4;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_MWRQ_OFFSET = 3;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_MWRQ_OFFSET = 2;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_INCMD_OFFSET = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_INCMD_OFFSET = 0;

	static const unsigned int MISC_ST_PLBS1_WPOST_CFG_ERR_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_ILLEGAL_CMD_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_ADDR_ERR_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_RDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_RDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_WDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_WDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_SRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_SRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_SWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_SWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_MRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_MRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_MWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_MWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_OF_INCMD_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBS1_FIFO_UF_INCMD_BITSIZE = 1;

	static const uint32_t MISC_ST_PLBS1_WPOST_CFG_ERR_MASK = ((1UL << MISC_ST_PLBS1_WPOST_CFG_ERR_BITSIZE) - 1) << MISC_ST_PLBS1_WPOST_CFG_ERR_OFFSET;
	static const uint32_t MISC_ST_PLBS1_ILLEGAL_CMD_MASK = ((1UL << MISC_ST_PLBS1_ILLEGAL_CMD_BITSIZE) - 1) << MISC_ST_PLBS1_ILLEGAL_CMD_OFFSET;
	static const uint32_t MISC_ST_PLBS1_ADDR_ERR_MASK = ((1UL << MISC_ST_PLBS1_ADDR_ERR_BITSIZE) - 1) << MISC_ST_PLBS1_ADDR_ERR_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_RDAT_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_RDAT_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_RDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_RDAT_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_RDAT_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_RDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_WDAT_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_WDAT_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_WDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_WDAT_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_WDAT_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_WDAT_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_SRDQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_SRDQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_SRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_SRDQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_SRDQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_SRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_SWRQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_SWRQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_SWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_SWRQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_SWRQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_SWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_MRDQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_MRDQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_MRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_MRDQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_MRDQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_MRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_MWRQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_MWRQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_MWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_MWRQ_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_MWRQ_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_MWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_OF_INCMD_MASK = ((1UL << MISC_ST_PLBS1_FIFO_OF_INCMD_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_OF_INCMD_OFFSET;
	static const uint32_t MISC_ST_PLBS1_FIFO_UF_INCMD_MASK = ((1UL << MISC_ST_PLBS1_FIFO_UF_INCMD_BITSIZE) - 1) << MISC_ST_PLBS1_FIFO_UF_INCMD_OFFSET;
	
	static const uint32_t MISC_ST_PLBS1_MASK = MISC_ST_PLBS1_WPOST_CFG_ERR_MASK | MISC_ST_PLBS1_ILLEGAL_CMD_MASK | MISC_ST_PLBS1_ADDR_ERR_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_RDAT_MASK | MISC_ST_PLBS1_FIFO_UF_RDAT_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_WDAT_MASK | MISC_ST_PLBS1_FIFO_UF_WDAT_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_SRDQ_MASK | MISC_ST_PLBS1_FIFO_UF_SRDQ_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_SWRQ_MASK | MISC_ST_PLBS1_FIFO_UF_SWRQ_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_MRDQ_MASK | MISC_ST_PLBS1_FIFO_UF_MRDQ_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_MWRQ_MASK | MISC_ST_PLBS1_FIFO_UF_MWRQ_MASK |
	                                           MISC_ST_PLBS1_FIFO_OF_INCMD_MASK | MISC_ST_PLBS1_FIFO_UF_INCMD_MASK;

	static const uint32_t MISC_ST_PLBS1_CLEAR_ON_WRITE_MASK = MISC_ST_PLBS1_WPOST_CFG_ERR_MASK | MISC_ST_PLBS1_ILLEGAL_CMD_MASK | MISC_ST_PLBS1_ADDR_ERR_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_RDAT_MASK | MISC_ST_PLBS1_FIFO_UF_RDAT_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_WDAT_MASK | MISC_ST_PLBS1_FIFO_UF_WDAT_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_SRDQ_MASK | MISC_ST_PLBS1_FIFO_UF_SRDQ_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_SWRQ_MASK | MISC_ST_PLBS1_FIFO_UF_SWRQ_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_MRDQ_MASK | MISC_ST_PLBS1_FIFO_UF_MRDQ_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_MWRQ_MASK | MISC_ST_PLBS1_FIFO_UF_MWRQ_MASK |
	                                                          MISC_ST_PLBS1_FIFO_OF_INCMD_MASK | MISC_ST_PLBS1_FIFO_UF_INCMD_MASK;

	static const uint32_t MISC_ST_PLBS1_RESET_VALUE = 0x0UL;
};

class PLBERR_ST_PLBS1_Layout
{
public:
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M0_MIRQ_OFFSET = 3;
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M1_MIRQ_OFFSET = 2;
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M2_MIRQ_OFFSET = 1;
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M3_MIRQ_OFFSET = 0;

	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M0_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M1_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M2_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBS1_PLBS1_M3_MIRQ_BITSIZE = 1;

	static const uint32_t PLBERR_ST_PLBS1_PLBS1_M0_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS1_PLBS1_M0_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS1_PLBS1_M0_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBS1_PLBS1_M1_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS1_PLBS1_M1_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS1_PLBS1_M1_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBS1_PLBS1_M2_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS1_PLBS1_M2_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS1_PLBS1_M2_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBS1_PLBS1_M3_MIRQ_MASK = ((1UL << PLBERR_ST_PLBS1_PLBS1_M3_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBS1_PLBS1_M3_MIRQ_OFFSET;
	
	static const uint32_t PLBERR_ST_PLBS1_CLEAR_ON_WRITE_MASK = PLBERR_ST_PLBS1_PLBS1_M0_MIRQ_MASK | PLBERR_ST_PLBS1_PLBS1_M1_MIRQ_MASK |
	                                                            PLBERR_ST_PLBS1_PLBS1_M2_MIRQ_MASK | PLBERR_ST_PLBS1_PLBS1_M3_MIRQ_MASK;
	
	static const uint32_t PLBERR_ST_PLBS1_RESET_VALUE = 0x0UL;
};

class MISC_PLBS1_Layout
{
public:
	static const unsigned int MISC_PLBS1_MODE_128N64_OFFSET = 31;
	static const unsigned int MISC_PLBS1_FIFO_RDAT_RST_OFFSET = 6;
	static const unsigned int MISC_PLBS1_FIFO_WDAT_RST_OFFSET = 5;
	static const unsigned int MISC_PLBS1_FIFO_SRDQ_RST_OFFSET = 4;
	static const unsigned int MISC_PLBS1_FIFO_SWRQ_RST_OFFSET = 3;
	static const unsigned int MISC_PLBS1_FIFO_MRDQ_RST_OFFSET = 2;
	static const unsigned int MISC_PLBS1_FIFO_MWRQ_RST_OFFSET = 1;
	static const unsigned int MISC_PLBS1_FIFO_INCMD_RST_OFFSET = 0;

	static const unsigned int MISC_PLBS1_MODE_128N64_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_RDAT_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_WDAT_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_SRDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_SWRQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_MRDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_MWRQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBS1_FIFO_INCMD_RST_BITSIZE = 1;

	static const uint32_t MISC_PLBS1_MODE_128N64_MASK = ((1UL << MISC_PLBS1_MODE_128N64_BITSIZE) - 1) << MISC_PLBS1_MODE_128N64_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_RDAT_RST_MASK = ((1UL << MISC_PLBS1_FIFO_RDAT_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_RDAT_RST_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_WDAT_RST_MASK = ((1UL << MISC_PLBS1_FIFO_WDAT_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_WDAT_RST_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_SRDQ_RST_MASK = ((1UL << MISC_PLBS1_FIFO_SRDQ_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_SRDQ_RST_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_SWRQ_RST_MASK = ((1UL << MISC_PLBS1_FIFO_SWRQ_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_SWRQ_RST_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_MRDQ_RST_MASK = ((1UL << MISC_PLBS1_FIFO_MRDQ_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_MRDQ_RST_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_MWRQ_RST_MASK = ((1UL << MISC_PLBS1_FIFO_MWRQ_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_MWRQ_RST_OFFSET;
	static const uint32_t MISC_PLBS1_FIFO_INCMD_RST_MASK = ((1UL << MISC_PLBS1_FIFO_INCMD_RST_BITSIZE) - 1) << MISC_PLBS1_FIFO_INCMD_RST_OFFSET;
	
	static const uint32_t MISC_PLBS1_CLEAR_ON_WRITE_MASK = MISC_PLBS1_FIFO_RDAT_RST_MASK | MISC_PLBS1_FIFO_WDAT_RST_MASK |
	                                                       MISC_PLBS1_FIFO_SRDQ_RST_MASK | MISC_PLBS1_FIFO_SWRQ_RST_MASK | MISC_PLBS1_FIFO_MRDQ_RST_MASK |
	                                                       MISC_PLBS1_FIFO_MWRQ_RST_MASK | MISC_PLBS1_FIFO_INCMD_RST_MASK;
	
	static const uint32_t MISC_PLBS1_WRITE_ONLY_MASK = MISC_PLBS1_FIFO_RDAT_RST_MASK | MISC_PLBS1_FIFO_WDAT_RST_MASK |
	                                                   MISC_PLBS1_FIFO_SRDQ_RST_MASK | MISC_PLBS1_FIFO_SWRQ_RST_MASK | MISC_PLBS1_FIFO_MRDQ_RST_MASK |
	                                                   MISC_PLBS1_FIFO_MWRQ_RST_MASK | MISC_PLBS1_FIFO_INCMD_RST_MASK;

	static const uint32_t MISC_PLBS1_RESET_VALUE = 0x80000000UL;
};

class CMD_SNIFF_PLBS1_Layout
{
public:
	static const unsigned int CMD_SNIFF_PLBS1_ENABLE_OFFSET = 31;
	static const unsigned int CMD_SNIFF_PLBS1_SIZE_OFFSET = 24;
	static const unsigned int CMD_SNIFF_PLBS1_RNW_OFFSET = 23;
	static const unsigned int CMD_SNIFF_PLBS1_MID_OFFSET = 20;
	static const unsigned int CMD_SNIFF_PLBS1_SPLBNDMA_OFFSET = 19;
	static const unsigned int CMD_SNIFF_PLBS1_SPLB_MID_OFFSET = 16;
	static const unsigned int CMD_SNIFF_PLBS1_SSIZE_OFFSET = 14;
	static const unsigned int CMD_SNIFF_PLBS1_SIZE_EN_OFFSET = 6;
	static const unsigned int CMD_SNIFF_PLBS1_RNW_EN_OFFSET = 5;
	static const unsigned int CMD_SNIFF_PLBS1_MID_EN_OFFSET = 4;
	static const unsigned int CMD_SNIFF_PLBS1_SSIZE_EN_OFFSET = 1;
	static const unsigned int CMD_SNIFF_PLBS1_ADDR_EN_OFFSET = 0;

	static const unsigned int CMD_SNIFF_PLBS1_ENABLE_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_SIZE_BITSIZE = 4;
	static const unsigned int CMD_SNIFF_PLBS1_RNW_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_MID_BITSIZE = 3;
	static const unsigned int CMD_SNIFF_PLBS1_SPLBNDMA_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_SPLB_MID_BITSIZE = 2;
	static const unsigned int CMD_SNIFF_PLBS1_SSIZE_BITSIZE = 2;
	static const unsigned int CMD_SNIFF_PLBS1_SIZE_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_RNW_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_MID_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_SSIZE_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBS1_ADDR_EN_BITSIZE = 1;

	static const uint32_t CMD_SNIFF_PLBS1_ENABLE_MASK = ((1UL << CMD_SNIFF_PLBS1_ENABLE_BITSIZE) - 1) << CMD_SNIFF_PLBS1_ENABLE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_SIZE_MASK = ((1UL << CMD_SNIFF_PLBS1_SIZE_BITSIZE) - 1) << CMD_SNIFF_PLBS1_SIZE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_RNW_MASK = ((1UL << CMD_SNIFF_PLBS1_RNW_BITSIZE) - 1) << CMD_SNIFF_PLBS1_RNW_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_MID_MASK = ((1UL << CMD_SNIFF_PLBS1_MID_BITSIZE) - 1) << CMD_SNIFF_PLBS1_MID_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_SPLBNDMA_MASK = ((1UL << CMD_SNIFF_PLBS1_SPLBNDMA_BITSIZE) - 1) << CMD_SNIFF_PLBS1_SPLBNDMA_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_SPLB_MID_MASK = ((1UL << CMD_SNIFF_PLBS1_SPLB_MID_BITSIZE) - 1) << CMD_SNIFF_PLBS1_SPLB_MID_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_SSIZE_MASK = ((1UL << CMD_SNIFF_PLBS1_SSIZE_BITSIZE) - 1) << CMD_SNIFF_PLBS1_SSIZE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_SIZE_EN_MASK = ((1UL << CMD_SNIFF_PLBS1_SIZE_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS1_SIZE_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_RNW_EN_MASK = ((1UL << CMD_SNIFF_PLBS1_RNW_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS1_RNW_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_MID_EN_MASK = ((1UL << CMD_SNIFF_PLBS1_MID_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS1_MID_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_SSIZE_EN_MASK = ((1UL << CMD_SNIFF_PLBS1_SSIZE_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS1_SSIZE_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBS1_ADDR_EN_MASK = ((1UL << CMD_SNIFF_PLBS1_ADDR_EN_BITSIZE) - 1) << CMD_SNIFF_PLBS1_ADDR_EN_OFFSET;
	
	static const uint32_t CMD_SNIFF_PLBS1_WRITE_MASK = CMD_SNIFF_PLBS1_ENABLE_MASK | CMD_SNIFF_PLBS1_SIZE_MASK | CMD_SNIFF_PLBS1_RNW_MASK |
	                                                   CMD_SNIFF_PLBS1_MID_MASK | CMD_SNIFF_PLBS1_SPLBNDMA_MASK | CMD_SNIFF_PLBS1_SPLB_MID_MASK |
	                                                   CMD_SNIFF_PLBS1_SSIZE_MASK | CMD_SNIFF_PLBS1_SIZE_EN_MASK | CMD_SNIFF_PLBS1_RNW_EN_MASK |
	                                                   CMD_SNIFF_PLBS1_MID_EN_MASK | CMD_SNIFF_PLBS1_SSIZE_EN_MASK | CMD_SNIFF_PLBS1_ADDR_EN_MASK;
	
	static const uint32_t CMD_SNIFF_PLBS1_RESET_VALUE = 0x0;
};

class CFG_PLBM_Layout
{
public:
	static const unsigned int CFG_PLBM_LOCK_SESR_OFFSET = 31;
	static const unsigned int CFG_PLBM_XBAR_PRIORITY_ENA_OFFSET = 7;
	static const unsigned int CFG_PLBM_SL_ETERM_MODE_OFFSET = 5;
	static const unsigned int CFG_PLBM_LOCKXFER_OFFSET = 4;
	static const unsigned int CFG_PLBM_RPIPE_OFFSET = 3;
	static const unsigned int CFG_PLBM_WPIPE_OFFSET = 2;
	static const unsigned int CFG_PLBM_WPOST_OFFSET = 1;

	static const unsigned int CFG_PLBM_LOCK_SESR_BITSIZE = 1;
	static const unsigned int CFG_PLBM_XBAR_PRIORITY_ENA_BITSIZE = 1;
	static const unsigned int CFG_PLBM_SL_ETERM_MODE_BITSIZE = 1;
	static const unsigned int CFG_PLBM_LOCKXFER_BITSIZE = 1;
	static const unsigned int CFG_PLBM_RPIPE_BITSIZE = 1;
	static const unsigned int CFG_PLBM_WPIPE_BITSIZE = 1;
	static const unsigned int CFG_PLBM_WPOST_BITSIZE = 1;
	
	static const uint32_t CFG_PLBM_LOCK_SESR_MASK = ((1UL << CFG_PLBM_LOCK_SESR_BITSIZE) - 1) << CFG_PLBM_LOCK_SESR_OFFSET;
	static const uint32_t CFG_PLBM_XBAR_PRIORITY_ENA_MASK = ((1UL << CFG_PLBM_XBAR_PRIORITY_ENA_BITSIZE) - 1) << CFG_PLBM_XBAR_PRIORITY_ENA_OFFSET;
	static const uint32_t CFG_PLBM_SL_ETERM_MODE_MASK = ((1UL << CFG_PLBM_SL_ETERM_MODE_BITSIZE) - 1) << CFG_PLBM_SL_ETERM_MODE_OFFSET;
	static const uint32_t CFG_PLBM_LOCKXFER_MASK = ((1UL << CFG_PLBM_LOCKXFER_BITSIZE) - 1) << CFG_PLBM_LOCKXFER_OFFSET;
	static const uint32_t CFG_PLBM_RPIPE_MASK = ((1UL << CFG_PLBM_RPIPE_BITSIZE) - 1) << CFG_PLBM_RPIPE_OFFSET;
	static const uint32_t CFG_PLBM_WPIPE_MASK = ((1UL << CFG_PLBM_WPIPE_BITSIZE) - 1) << CFG_PLBM_WPIPE_OFFSET;
	static const uint32_t CFG_PLBM_WPOST_MASK = ((1UL << CFG_PLBM_WPOST_BITSIZE) - 1) << CFG_PLBM_WPOST_OFFSET;
	
	static const uint32_t CFG_PLBM_WRITE_MASK = CFG_PLBM_LOCK_SESR_MASK | CFG_PLBM_XBAR_PRIORITY_ENA_MASK | CFG_PLBM_SL_ETERM_MODE_MASK |
	                                            CFG_PLBM_LOCKXFER_MASK | CFG_PLBM_RPIPE_MASK | CFG_PLBM_WPIPE_MASK |
	                                            CFG_PLBM_WPOST_MASK;
	
	static const uint32_t CFG_PLBM_RESET_VALUE = 0x80000097UL;
};

class FSEAR_U_PLBM_Layout
{
public:
	static const unsigned int FSEAR_U_PLBM_U4BIT_OFFSET = 0;

	static const unsigned int FSEAR_U_PLBM_U4BIT_BITSIZE = 4;

	static const uint32_t FSEAR_U_PLBM_U4BIT_MASK = ((1UL << FSEAR_U_PLBM_U4BIT_BITSIZE) - 1) << FSEAR_U_PLBM_U4BIT_OFFSET;
	
	static const uint32_t FSEAR_U_PLBM_MASK = FSEAR_U_PLBM_U4BIT_MASK;

	static const uint32_t FSEAR_U_PLBM_WRITE_MASK = FSEAR_U_PLBM_U4BIT_MASK;

	static const uint32_t FSEAR_U_PLBM_RESET_VALUE = 0x0UL;
};

class FSEAR_L_PLBM_Layout
{
public:
	static const unsigned int FSEAR_L_PLBM_L32BIT_OFFSET = 0;

	static const unsigned int FSEAR_L_PLBM_L32BIT_BITSIZE = 32;

	static const uint32_t FSEAR_L_PLBM_L32BIT_MASK = 0xffffffffUL; //((1UL << FSEAR_L_PLBM_L32BIT_BITSIZE) - 1) << FSEAR_L_PLBM_L32BIT_OFFSET;
	
	static const uint32_t FSEAR_L_PLBM_MASK = FSEAR_L_PLBM_L32BIT_MASK;

	static const uint32_t FSEAR_L_PLBM_WRITE_MASK = FSEAR_L_PLBM_L32BIT_MASK;

	static const uint32_t FSEAR_L_PLBM_RESET_VALUE = 0x0UL;
};

class FSESR_PLBM_Layout
{
public:
	static const unsigned int FSESR_PLBM_VLD_OFFSET = 31;
	static const unsigned int FSESR_PLBM_LOCKERR_OFFSET = 30;
	static const unsigned int FSESR_PLBM_PLBS_DMA_OFFSET = 29;
	static const unsigned int FSESR_PLBM_MID_OFFSET = 26;
	static const unsigned int FSESR_PLBM_SSIZE_OFFSET = 24;
	static const unsigned int FSESR_PLBM_TYPE_OFFSET = 21;
	static const unsigned int FSESR_PLBM_RNW_OFFSET = 20;
	static const unsigned int FSESR_PLBM_SIZE_OFFSET = 16;
	static const unsigned int FSESR_PLBM_BE_OFFSET = 0;

	static const unsigned int FSESR_PLBM_VLD_BITSIZE = 1;
	static const unsigned int FSESR_PLBM_LOCKERR_BITSIZE = 1;
	static const unsigned int FSESR_PLBM_PLBS_DMA_BITSIZE = 1;
	static const unsigned int FSESR_PLBM_MID_BITSIZE = 2;
	static const unsigned int FSESR_PLBM_SSIZE_BITSIZE = 2;
	static const unsigned int FSESR_PLBM_TYPE_BITSIZE = 3;
	static const unsigned int FSESR_PLBM_RNW_BITSIZE = 1;
	static const unsigned int FSESR_PLBM_SIZE_BITSIZE = 4;
	static const unsigned int FSESR_PLBM_BE_BITSIZE = 16;
	
	static const uint32_t FSESR_PLBM_VLD_MASK = ((1UL << FSESR_PLBM_VLD_BITSIZE) - 1) << FSESR_PLBM_VLD_OFFSET;
	static const uint32_t FSESR_PLBM_LOCKERR_MASK = ((1UL << FSESR_PLBM_LOCKERR_BITSIZE) - 1) << FSESR_PLBM_LOCKERR_OFFSET;
	static const uint32_t FSESR_PLBM_PLBS_DMA_MASK = ((1UL << FSESR_PLBM_PLBS_DMA_BITSIZE) - 1) << FSESR_PLBM_PLBS_DMA_OFFSET;
	static const uint32_t FSESR_PLBM_MID_MASK = ((1UL << FSESR_PLBM_MID_BITSIZE) - 1) << FSESR_PLBM_MID_OFFSET;
	static const uint32_t FSESR_PLBM_SSIZE_MASK = ((1UL << FSESR_PLBM_SSIZE_BITSIZE) - 1) << FSESR_PLBM_SSIZE_OFFSET;
	static const uint32_t FSESR_PLBM_TYPE_MASK = ((1UL << FSESR_PLBM_TYPE_BITSIZE) - 1) << FSESR_PLBM_TYPE_OFFSET;
	static const uint32_t FSESR_PLBM_RNW_MASK = ((1UL << FSESR_PLBM_RNW_BITSIZE) - 1) << FSESR_PLBM_RNW_OFFSET;
	static const uint32_t FSESR_PLBM_SIZE_MASK = ((1UL << FSESR_PLBM_SIZE_BITSIZE) - 1) << FSESR_PLBM_SIZE_OFFSET;
	static const uint32_t FSESR_PLBM_BE_MASK = ((1UL << FSESR_PLBM_BE_BITSIZE) - 1) << FSESR_PLBM_BE_OFFSET;
	
	static const uint32_t FSESR_PLBM_CLEAR_ON_WRITE_MASK = FSESR_PLBM_VLD_MASK | FSESR_PLBM_LOCKERR_MASK | FSESR_PLBM_PLBS_DMA_MASK | FSESR_PLBM_MID_MASK |
	                                                       FSESR_PLBM_SSIZE_MASK | FSESR_PLBM_TYPE_MASK | FSESR_PLBM_RNW_MASK |
	                                                       FSESR_PLBM_SIZE_MASK | FSESR_PLBM_BE_MASK;
	
	static const uint32_t FSESR_PLBM_RESET_VALUE = 0x0UL;
};

class MISC_ST_PLBM_Layout
{
public:
	static const unsigned int MISC_ST_PLBM_WPOST_CFG_ERR_OFFSET = 31;
	static const unsigned int MISC_ST_PLBM_ETERM_CFG_ERR_OFFSET = 30;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_RDAT_OFFSET = 13;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_RDAT_OFFSET = 12;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_WDAT_OFFSET = 11;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_WDAT_OFFSET = 10;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_SRDQ_OFFSET = 9;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_SRDQ_OFFSET = 8;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_SWRQ_OFFSET = 7;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_SWRQ_OFFSET = 6;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_MRDQ_OFFSET = 5;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_MRDQ_OFFSET = 4;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_MWRQ_OFFSET = 3;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_MWRQ_OFFSET = 2;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_INCMD_OFFSET = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_INCMD_OFFSET = 0;

	static const unsigned int MISC_ST_PLBM_WPOST_CFG_ERR_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_ETERM_CFG_ERR_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_RDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_RDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_WDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_WDAT_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_SRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_SRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_SWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_SWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_MRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_MRDQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_MWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_MWRQ_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_OF_INCMD_BITSIZE = 1;
	static const unsigned int MISC_ST_PLBM_FIFO_UF_INCMD_BITSIZE = 1;

	static const uint32_t MISC_ST_PLBM_WPOST_CFG_ERR_MASK = ((1UL << MISC_ST_PLBM_WPOST_CFG_ERR_BITSIZE) - 1) << MISC_ST_PLBM_WPOST_CFG_ERR_OFFSET;
	static const uint32_t MISC_ST_PLBM_ETERM_CFG_ERR_MASK = ((1UL << MISC_ST_PLBM_ETERM_CFG_ERR_BITSIZE) - 1) << MISC_ST_PLBM_ETERM_CFG_ERR_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_RDAT_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_RDAT_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_RDAT_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_RDAT_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_RDAT_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_RDAT_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_WDAT_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_WDAT_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_WDAT_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_WDAT_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_WDAT_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_WDAT_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_SRDQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_SRDQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_SRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_SRDQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_SRDQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_SRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_SWRQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_SWRQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_SWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_SWRQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_SWRQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_SWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_MRDQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_MRDQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_MRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_MRDQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_MRDQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_MRDQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_MWRQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_MWRQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_MWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_MWRQ_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_MWRQ_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_MWRQ_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_OF_INCMD_MASK = ((1UL << MISC_ST_PLBM_FIFO_OF_INCMD_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_OF_INCMD_OFFSET;
	static const uint32_t MISC_ST_PLBM_FIFO_UF_INCMD_MASK = ((1UL << MISC_ST_PLBM_FIFO_UF_INCMD_BITSIZE) - 1) << MISC_ST_PLBM_FIFO_UF_INCMD_OFFSET;
	
	static const uint32_t MISC_ST_PLBM_MASK = MISC_ST_PLBM_WPOST_CFG_ERR_MASK | MISC_ST_PLBM_ETERM_CFG_ERR_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_RDAT_MASK | MISC_ST_PLBM_FIFO_UF_RDAT_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_WDAT_MASK | MISC_ST_PLBM_FIFO_UF_WDAT_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_SRDQ_MASK | MISC_ST_PLBM_FIFO_UF_SRDQ_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_SWRQ_MASK | MISC_ST_PLBM_FIFO_UF_SWRQ_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_MRDQ_MASK | MISC_ST_PLBM_FIFO_UF_MRDQ_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_MWRQ_MASK | MISC_ST_PLBM_FIFO_UF_MWRQ_MASK |
	                                          MISC_ST_PLBM_FIFO_OF_INCMD_MASK | MISC_ST_PLBM_FIFO_UF_INCMD_MASK;
	
	static const uint32_t MISC_ST_PLBM_CLEAR_ON_WRITE_MASK = MISC_ST_PLBM_WPOST_CFG_ERR_MASK | MISC_ST_PLBM_ETERM_CFG_ERR_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_RDAT_MASK | MISC_ST_PLBM_FIFO_UF_RDAT_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_WDAT_MASK | MISC_ST_PLBM_FIFO_UF_WDAT_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_SRDQ_MASK | MISC_ST_PLBM_FIFO_UF_SRDQ_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_SWRQ_MASK | MISC_ST_PLBM_FIFO_UF_SWRQ_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_MRDQ_MASK | MISC_ST_PLBM_FIFO_UF_MRDQ_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_MWRQ_MASK | MISC_ST_PLBM_FIFO_UF_MWRQ_MASK |
	                                                         MISC_ST_PLBM_FIFO_OF_INCMD_MASK | MISC_ST_PLBM_FIFO_UF_INCMD_MASK;
	
	static const uint32_t MISC_ST_PLBM_RESET_VALUE = 0x0UL;
};

class PLBERR_ST_PLBM_Layout
{
public:
	static const unsigned int PLBERR_ST_PLBM_PLBS0_DMA_MIRQ_OFFSET = 12;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_DMA_MIRQ_OFFSET = 11;
	static const unsigned int PLBERR_ST_PLBM_C440_MIRQ_ICUR_OFFSET = 10;
	static const unsigned int PLBERR_ST_PLBM_C440_MIRQ_DCUW_OFFSET = 9;
	static const unsigned int PLBERR_ST_PLBM_C440_MIRQ_DCUR_OFFSET = 8;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M0_MIRQ_OFFSET = 7;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M1_MIRQ_OFFSET = 6;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M2_MIRQ_OFFSET = 5;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M3_MIRQ_OFFSET = 4;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M0_MIRQ_OFFSET = 3;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M1_MIRQ_OFFSET = 2;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M2_MIRQ_OFFSET = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M3_MIRQ_OFFSET = 0;

	static const unsigned int PLBERR_ST_PLBM_PLBS0_DMA_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_DMA_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_C440_MIRQ_ICUR_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_C440_MIRQ_DCUW_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_C440_MIRQ_DCUR_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M0_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M1_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M2_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS0_M3_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M0_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M1_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M2_MIRQ_BITSIZE = 1;
	static const unsigned int PLBERR_ST_PLBM_PLBS1_M3_MIRQ_BITSIZE = 1;

	static const uint32_t PLBERR_ST_PLBM_PLBS0_DMA_MASK = ((1UL << PLBERR_ST_PLBM_PLBS0_DMA_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS0_DMA_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS1_DMA_MASK = ((1UL << PLBERR_ST_PLBM_PLBS1_DMA_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS1_DMA_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_C440_MIRQ_ICUR_MASK = ((1UL << PLBERR_ST_PLBM_C440_MIRQ_ICUR_BITSIZE) - 1) << PLBERR_ST_PLBM_C440_MIRQ_ICUR_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_C440_MIRQ_DCUW_MASK = ((1UL << PLBERR_ST_PLBM_C440_MIRQ_DCUW_BITSIZE) - 1) << PLBERR_ST_PLBM_C440_MIRQ_DCUW_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_C440_MIRQ_DCUR_MASK = ((1UL << PLBERR_ST_PLBM_C440_MIRQ_DCUR_BITSIZE) - 1) << PLBERR_ST_PLBM_C440_MIRQ_DCUR_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS0_M0_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS0_M0_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS0_M0_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS0_M1_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS0_M1_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS0_M1_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS0_M2_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS0_M2_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS0_M2_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS0_M3_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS0_M3_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS0_M3_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS1_M0_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS1_M0_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS1_M0_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS1_M1_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS1_M1_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS1_M1_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS1_M2_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS1_M2_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS1_M2_MIRQ_OFFSET;
	static const uint32_t PLBERR_ST_PLBM_PLBS1_M3_MIRQ_MASK = ((1UL << PLBERR_ST_PLBM_PLBS1_M3_MIRQ_BITSIZE) - 1) << PLBERR_ST_PLBM_PLBS1_M3_MIRQ_OFFSET;
	
	static const uint32_t PLBERR_ST_PLBM_CLEAR_ON_WRITE_MASK = PLBERR_ST_PLBM_PLBS0_DMA_MASK | PLBERR_ST_PLBM_PLBS1_DMA_MASK |
	                                                           PLBERR_ST_PLBM_C440_MIRQ_ICUR_MASK | PLBERR_ST_PLBM_C440_MIRQ_DCUW_MASK |
	                                                           PLBERR_ST_PLBM_C440_MIRQ_DCUR_MASK |
	                                                           PLBERR_ST_PLBM_PLBS0_M0_MIRQ_MASK | PLBERR_ST_PLBM_PLBS0_M1_MIRQ_MASK |
	                                                           PLBERR_ST_PLBM_PLBS0_M2_MIRQ_MASK | PLBERR_ST_PLBM_PLBS0_M3_MIRQ_MASK |
	                                                           PLBERR_ST_PLBM_PLBS1_M0_MIRQ_MASK | PLBERR_ST_PLBM_PLBS1_M1_MIRQ_MASK |
	                                                           PLBERR_ST_PLBM_PLBS1_M2_MIRQ_MASK | PLBERR_ST_PLBM_PLBS1_M3_MIRQ_MASK;
	
	static const uint32_t PLBERR_ST_PLBM_RESET_VALUE = 0x0UL;
};

class MISC_PLBM_Layout
{
public:
	static const unsigned int MISC_PLBM_FLUSH_MODE_OFFSET = 29;
	static const unsigned int MISC_PLBM_FIFO_RDAT_RST_OFFSET = 6;
	static const unsigned int MISC_PLBM_FIFO_WDAT_RST_OFFSET = 5;
	static const unsigned int MISC_PLBM_FIFO_SRDQ_RST_OFFSET = 4;
	static const unsigned int MISC_PLBM_FIFO_SWRQ_RST_OFFSET = 3;
	static const unsigned int MISC_PLBM_FIFO_MRDQ_RST_OFFSET = 2;
	static const unsigned int MISC_PLBM_FIFO_MWRQ_RST_OFFSET = 1;
	static const unsigned int MISC_PLBM_FIFO_INCMD_RST_OFFSET = 0;

	static const unsigned int MISC_PLBM_FLUSH_MODE_BITSIZE = 2;
	static const unsigned int MISC_PLBM_FIFO_RDAT_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBM_FIFO_WDAT_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBM_FIFO_SRDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBM_FIFO_SWRQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBM_FIFO_MRDQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBM_FIFO_MWRQ_RST_BITSIZE = 1;
	static const unsigned int MISC_PLBM_FIFO_INCMD_RST_BITSIZE = 1;

	static const uint32_t MISC_PLBM_FLUSH_MODE_MASK = ((1UL << MISC_PLBM_FLUSH_MODE_BITSIZE) - 1) << MISC_PLBM_FLUSH_MODE_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_RDAT_RST_MASK = ((1UL << MISC_PLBM_FIFO_RDAT_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_RDAT_RST_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_WDAT_RST_MASK = ((1UL << MISC_PLBM_FIFO_WDAT_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_WDAT_RST_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_SRDQ_RST_MASK = ((1UL << MISC_PLBM_FIFO_SRDQ_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_SRDQ_RST_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_SWRQ_RST_MASK = ((1UL << MISC_PLBM_FIFO_SWRQ_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_SWRQ_RST_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_MRDQ_RST_MASK = ((1UL << MISC_PLBM_FIFO_MRDQ_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_MRDQ_RST_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_MWRQ_RST_MASK = ((1UL << MISC_PLBM_FIFO_MWRQ_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_MWRQ_RST_OFFSET;
	static const uint32_t MISC_PLBM_FIFO_INCMD_RST_MASK = ((1UL << MISC_PLBM_FIFO_INCMD_RST_BITSIZE) - 1) << MISC_PLBM_FIFO_INCMD_RST_OFFSET;
	
	static const uint32_t MISC_PLBM_CLEAR_ON_WRITE_MASK = MISC_PLBM_FIFO_RDAT_RST_MASK | MISC_PLBM_FIFO_WDAT_RST_MASK |
	                                                      MISC_PLBM_FIFO_SRDQ_RST_MASK | MISC_PLBM_FIFO_SWRQ_RST_MASK | MISC_PLBM_FIFO_MRDQ_RST_MASK |
	                                                      MISC_PLBM_FIFO_MWRQ_RST_MASK | MISC_PLBM_FIFO_INCMD_RST_MASK;

	static const uint32_t MISC_PLBM_WRITE_ONLY_MASK = MISC_PLBM_FIFO_RDAT_RST_MASK | MISC_PLBM_FIFO_WDAT_RST_MASK |
	                                                  MISC_PLBM_FIFO_SRDQ_RST_MASK | MISC_PLBM_FIFO_SWRQ_RST_MASK | MISC_PLBM_FIFO_MRDQ_RST_MASK |
	                                                  MISC_PLBM_FIFO_MWRQ_RST_MASK | MISC_PLBM_FIFO_INCMD_RST_MASK;

	static const uint32_t MISC_PLBM_WRITE_MASK = MISC_PLBM_FLUSH_MODE_MASK;
	
	static const uint32_t MISC_PLBM_RESET_VALUE = 0x80000000UL;
};

class CMD_SNIFF_PLBM_Layout
{
public:
	static const unsigned int CMD_SNIFF_PLBM_ENABLE_OFFSET = 31;
	static const unsigned int CMD_SNIFF_PLBM_SIZE_OFFSET = 24;
	static const unsigned int CMD_SNIFF_PLBM_RNW_OFFSET = 23;
	static const unsigned int CMD_SNIFF_PLBM_MID_OFFSET = 20;
	static const unsigned int CMD_SNIFF_PLBM_SPLBNDMA_OFFSET = 19;
	static const unsigned int CMD_SNIFF_PLBM_SPLB_MID_OFFSET = 16;
	static const unsigned int CMD_SNIFF_PLBM_SSIZE_OFFSET = 14;
	static const unsigned int CMD_SNIFF_PLBM_SIZE_EN_OFFSET = 6;
	static const unsigned int CMD_SNIFF_PLBM_RNW_EN_OFFSET = 5;
	static const unsigned int CMD_SNIFF_PLBM_MID_EN_OFFSET = 4;
	static const unsigned int CMD_SNIFF_PLBM_SPLBNDMA_EN_OFFSET = 3;
	static const unsigned int CMD_SNIFF_PLBM_SPLB_MID_EN_OFFSET = 2;
	static const unsigned int CMD_SNIFF_PLBM_SSIZE_EN_OFFSET = 1;
	static const unsigned int CMD_SNIFF_PLBM_ADDR_EN_OFFSET = 0;

	static const unsigned int CMD_SNIFF_PLBM_ENABLE_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_SIZE_BITSIZE = 4;
	static const unsigned int CMD_SNIFF_PLBM_RNW_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_MID_BITSIZE = 3;
	static const unsigned int CMD_SNIFF_PLBM_SPLBNDMA_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_SPLB_MID_BITSIZE = 2;
	static const unsigned int CMD_SNIFF_PLBM_SSIZE_BITSIZE = 2;
	static const unsigned int CMD_SNIFF_PLBM_SIZE_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_RNW_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_MID_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_SPLBNDMA_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_SPLB_MID_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_SSIZE_EN_BITSIZE = 1;
	static const unsigned int CMD_SNIFF_PLBM_ADDR_EN_BITSIZE = 1;

	static const uint32_t CMD_SNIFF_PLBM_ENABLE_MASK = ((1UL << CMD_SNIFF_PLBM_ENABLE_BITSIZE) - 1) << CMD_SNIFF_PLBM_ENABLE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SIZE_MASK = ((1UL << CMD_SNIFF_PLBM_SIZE_BITSIZE) - 1) << CMD_SNIFF_PLBM_SIZE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_RNW_MASK = ((1UL << CMD_SNIFF_PLBM_RNW_BITSIZE) - 1) << CMD_SNIFF_PLBM_RNW_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_MID_MASK = ((1UL << CMD_SNIFF_PLBM_MID_BITSIZE) - 1) << CMD_SNIFF_PLBM_MID_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SPLBNDMA_MASK = ((1UL << CMD_SNIFF_PLBM_SPLBNDMA_BITSIZE) - 1) << CMD_SNIFF_PLBM_SPLBNDMA_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SPLB_MID_MASK = ((1UL << CMD_SNIFF_PLBM_SPLB_MID_BITSIZE) - 1) << CMD_SNIFF_PLBM_SPLB_MID_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SSIZE_MASK = ((1UL << CMD_SNIFF_PLBM_SSIZE_BITSIZE) - 1) << CMD_SNIFF_PLBM_SSIZE_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SIZE_EN_MASK = ((1UL << CMD_SNIFF_PLBM_SIZE_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_SIZE_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_RNW_EN_MASK = ((1UL << CMD_SNIFF_PLBM_RNW_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_RNW_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_MID_EN_MASK = ((1UL << CMD_SNIFF_PLBM_MID_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_MID_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SPLBNDMA_EN_MASK = ((1UL << CMD_SNIFF_PLBM_SPLBNDMA_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_SPLBNDMA_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SPLB_MID_EN_MASK = ((1UL << CMD_SNIFF_PLBM_SPLB_MID_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_SPLB_MID_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_SSIZE_EN_MASK = ((1UL << CMD_SNIFF_PLBM_SSIZE_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_SSIZE_EN_OFFSET;
	static const uint32_t CMD_SNIFF_PLBM_ADDR_EN_MASK = ((1UL << CMD_SNIFF_PLBM_ADDR_EN_BITSIZE) - 1) << CMD_SNIFF_PLBM_ADDR_EN_OFFSET;
	
	static const uint32_t CMD_SNIFF_PLBM_WRITE_MASK = CMD_SNIFF_PLBM_ENABLE_MASK | CMD_SNIFF_PLBM_SIZE_MASK | CMD_SNIFF_PLBM_RNW_MASK |
	                                                  CMD_SNIFF_PLBM_MID_MASK | CMD_SNIFF_PLBM_SPLBNDMA_MASK | CMD_SNIFF_PLBM_SPLB_MID_MASK |
	                                                  CMD_SNIFF_PLBM_SSIZE_MASK | CMD_SNIFF_PLBM_SIZE_EN_MASK | CMD_SNIFF_PLBM_RNW_EN_MASK |
	                                                  CMD_SNIFF_PLBM_MID_EN_MASK | CMD_SNIFF_PLBM_SPLBNDMA_EN_MASK | CMD_SNIFF_PLBM_SPLB_MID_EN_MASK |
	                                                  CMD_SNIFF_PLBM_SSIZE_EN_MASK | CMD_SNIFF_PLBM_ADDR_EN_MASK;
	
	static const uint32_t CMD_SNIFF_PLBM_RESET_VALUE = 0x0;
};

class Config
	: public DCRMap
	, public ISTLayout
	, public ARB_XBC_Layout
	, public FIFOST_XBC_Layout
	, public MISC_XBC_Layout
	, public ARB_XBM_Layout
	, public FIFOST_XBM_Layout
	, public MISC_XBM_Layout
	, public CFG_PLBS0_Layout
	, public SEAR_U_PLBS0_Layout
	, public SEAR_L_PLBS0_Layout
	, public SESR_PLBS0_Layout
	, public MISC_ST_PLBS0_Layout
	, public PLBERR_ST_PLBS0_Layout
	, public MISC_PLBS0_Layout
	, public CMD_SNIFF_PLBS0_Layout
	, public CFG_PLBS1_Layout
	, public SEAR_U_PLBS1_Layout
	, public SEAR_L_PLBS1_Layout
	, public SESR_PLBS1_Layout
	, public MISC_ST_PLBS1_Layout
	, public PLBERR_ST_PLBS1_Layout
	, public MISC_PLBS1_Layout
	, public CMD_SNIFF_PLBS1_Layout
	, public CFG_PLBM_Layout
	, public FSEAR_U_PLBM_Layout
	, public FSEAR_L_PLBM_Layout
	, public FSESR_PLBM_Layout
	, public MISC_ST_PLBM_Layout
	, public PLBERR_ST_PLBM_Layout
	, public MISC_PLBM_Layout
	, public CMD_SNIFF_PLBM_Layout
{
public:
	typedef uint64_t ADDRESS;
	static const unsigned int PLB_WIDTH = 16; // PLB Width in bytes
	static const unsigned int MCI_WIDTH = 16; // MCI Width in bytes
	
	// interrupt masking at reset
	static const uint32_t IMASK_RESET_VALUE = 0xffffffffUL; // all 1's
	
	// Crossbar mapping at reset
	static const uint32_t TMPL0_XBAR_MAP_RESET_VALUE = 0xffff0000UL; // MCI: below 2 GB, MPLB: above 2 GB
	static const uint32_t TMPL1_XBAR_MAP_RESET_VALUE = 0x0UL;
	static const uint32_t TMPL2_XBAR_MAP_RESET_VALUE = 0x0UL;
	static const uint32_t TMPL3_XBAR_MAP_RESET_VALUE = 0x0UL;

	static const uint32_t TMPL0_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
	static const uint32_t TMPL1_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
	static const uint32_t TMPL2_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
	static const uint32_t TMPL3_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's

	static const uint32_t TMPL0_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
	static const uint32_t TMPL1_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
	static const uint32_t TMPL2_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
	static const uint32_t TMPL3_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's

	static const uint32_t TMPL_SEL_REG_RESET_VALUE = 0x3fffffffUL;
	
	// status registers at reset
	static const uint32_t SM_ST_XBC_RESET_VALUE = 0x0UL;
	static const uint32_t SM_ST_PLBS0_RESET_VALUE = 0x0UL;
	static const uint32_t SM_ST_PLBS1_RESET_VALUE = 0x0UL;
	static const uint32_t SM_ST_PLBM_RESET_VALUE = 0x0UL;
	
	// sniff registers at reset
	static const uint32_t CMD_SNIFFA_PLBS0_RESET_VALUE = 0x0UL;
	static const uint32_t CMD_SNIFFA_PLBS1_RESET_VALUE = 0x0UL;
	static const uint32_t CMD_SNIFFA_PLBM_RESET_VALUE = 0x0UL;
	
};

} // end of namespace crossbar
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_CROSSBAR_CONFIG_HH__
