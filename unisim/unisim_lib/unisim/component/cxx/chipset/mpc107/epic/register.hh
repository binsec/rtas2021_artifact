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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_REGISTER_HH__

#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace epic {

using std::string;

class Registers {
public:
	Registers();
	~Registers();
	
	/* Global and timer registers */
	uint32_t frr,
		gcr,
		eicr,
		evi,
		pi,
		svr,
		tfrr,
		gtccr[4],
		gtbcr[4],
		gtvpr[4],
		gtdr[4];
		
	/* interrupt source configuration registers */
	uint32_t ivpr[5],
		idr[5],
		svpr[16],
		sdr[16],
		iivpr[4],
		iidr[4];
	
	/* processor related registers */
	uint32_t pctpr,
		iack,
		eoi;
		
	/* registers addresses */
	static const uint32_t FRR = (uint32_t)0x41000,
		GCR = (uint32_t)0x41020,
		EICR = (uint32_t)0x41030,
		EVI = (uint32_t)0x41080,
		PI = (uint32_t)0x41090,
		SVR = (uint32_t)0x410e0,
		TFRR = (uint32_t)0x410f0,
		GTCCR0 = (uint32_t)0x41100,
		GTBCR0 = (uint32_t)0x41110,
		GTVPR0 = (uint32_t)0x41120,
		GTDR0 = (uint32_t)0x41130,
		GTCCR1 = (uint32_t)0x41140,
		GTBCR1 = (uint32_t)0x41150,
		GTVPR1 = (uint32_t)0x41160,
		GTDR1 = (uint32_t)0x41170,
		GTCCR2 = (uint32_t)0x41180,
		GTBCR2 = (uint32_t)0x41190,
		GTVPR2 = (uint32_t)0x411a0,
		GTDR2 = (uint32_t)0x411b0,
		GTCCR3 = (uint32_t)0x411c0,
		GTBCR3 = (uint32_t)0x411d0,
		GTVPR3 = (uint32_t)0x411e0,
		GTDR3 = (uint32_t)0x411f0,
		IVPR0 = (uint32_t)0x50200,
		IDR0 = (uint32_t)0x50210,
		IVPR1 = (uint32_t)0x50220,
		IDR1 = (uint32_t)0x50230,
		IVPR2 = (uint32_t)0x50240,
		IDR2 = (uint32_t)0x50250,
		IVPR3 = (uint32_t)0x50260,
		IDR3 = (uint32_t)0x50270,
		IVPR4 = (uint32_t)0x50280,
		IDR4 = (uint32_t)0x50290,
		SVPR0 = (uint32_t)0x50200,
		SDR0 = (uint32_t)0x50210,
		SVPR1 = (uint32_t)0x50220,
		SDR1 = (uint32_t)0x50230,
		SVPR2 = (uint32_t)0x50240,
		SDR2 = (uint32_t)0x50250,
		SVPR3 = (uint32_t)0x50260,
		SDR3 = (uint32_t)0x50270,
		SVPR4 = (uint32_t)0x50280,
		SDR4 = (uint32_t)0x50290,
		SVPR5 = (uint32_t)0x502a0,
		SDR5 = (uint32_t)0x502b0,
		SVPR6 = (uint32_t)0x502c0,
		SDR6 = (uint32_t)0x502d0,
		SVPR7 = (uint32_t)0x502e0,
		SDR7 = (uint32_t)0x502f0,
		SVPR8 = (uint32_t)0x50300,
		SDR8 = (uint32_t)0x50310,
		SVPR9 = (uint32_t)0x50320,
		SDR9 = (uint32_t)0x50330,
		SVPR10 = (uint32_t)0x50340,
		SDR10 = (uint32_t)0x50350,
		SVPR11 = (uint32_t)0x50360,
		SDR11 = (uint32_t)0x50370,
		SVPR12 = (uint32_t)0x50380,
		SDR12 = (uint32_t)0x50390,
		SVPR13 = (uint32_t)0x503a0,
		SDR13 = (uint32_t)0x503b0,
		SVPR14 = (uint32_t)0x503c0,
		SDR14 = (uint32_t)0x503d0,
		SVPR15 = (uint32_t)0x503e0,
		SDR15 = (uint32_t)0x503f0,
		IIVPR0 = (uint32_t)0x51020,
		IIDR0 = (uint32_t)0x51030,
		IIVPR1 = (uint32_t)0x51040,
		IIDR1 = (uint32_t)0x51050,
		IIVPR2 = (uint32_t)0x51060,
		IIDR2 = (uint32_t)0x51070,
		IIVPR3 = (uint32_t)0x510c0,
		IIDR3 = (uint32_t)0x510d0,
		PCTPR = (uint32_t)0x60080,
		IACK = (uint32_t)0x600a0,
		EOI = (uint32_t)0x600b0;

	/* registers names */
	static const string
		FRR_NAME,
		GCR_NAME,
		EICR_NAME,
		EVI_NAME,
		PI_NAME,
		SVR_NAME,
		TFRR_NAME,
		GTCCR0_NAME,
		GTBCR0_NAME,
		GTVPR0_NAME,
		GTDR0_NAME,
		GTCCR1_NAME,
		GTBCR1_NAME,
		GTVPR1_NAME,
		GTDR1_NAME,
		GTCCR2_NAME,
		GTBCR2_NAME,
		GTVPR2_NAME,
		GTDR2_NAME,
		GTCCR3_NAME,
		GTBCR3_NAME,
		GTVPR3_NAME,
		GTDR3_NAME,
		IVPR0_NAME,
		IDR0_NAME,
		IVPR1_NAME,
		IDR1_NAME,
		IVPR2_NAME,
		IDR2_NAME,
		IVPR3_NAME,
		IDR3_NAME,
		IVPR4_NAME,
		IDR4_NAME,
		SVPR0_NAME,
		SDR0_NAME,
		SVPR1_NAME,
		SDR1_NAME,
		SVPR2_NAME,
		SDR2_NAME,
		SVPR3_NAME,
		SDR3_NAME,
		SVPR4_NAME,
		SDR4_NAME,
		SVPR5_NAME,
		SDR5_NAME,
		SVPR6_NAME,
		SDR6_NAME,
		SVPR7_NAME,
		SDR7_NAME,
		SVPR8_NAME,
		SDR8_NAME,
		SVPR9_NAME,
		SDR9_NAME,
		SVPR10_NAME,
		SDR10_NAME,
		SVPR11_NAME,
		SDR11_NAME,
		SVPR12_NAME,
		SDR12_NAME,
		SVPR13_NAME,
		SDR13_NAME,
		SVPR14_NAME,
		SDR14_NAME,
		SVPR15_NAME,
		SDR15_NAME,
		IIVPR0_NAME,
		IIDR0_NAME,
		IIVPR1_NAME,
		IIDR1_NAME,
		IIVPR2_NAME,
		IIDR2_NAME,
		IIVPR3_NAME,
		IIDR3_NAME,
		PCTPR_NAME,
		IACK_NAME,
		EOI_NAME;
		
private:
};

} // end of namespace epic
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_REGISTER_HH__
