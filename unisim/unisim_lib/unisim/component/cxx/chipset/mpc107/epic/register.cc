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
 
#include "unisim/component/cxx/chipset/mpc107/epic/register.hh"

#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace epic {

using std::string;

const string Registers :: FRR_NAME = "FRR";
const string Registers :: GCR_NAME = "GCR";
const string Registers :: EICR_NAME = "EICR";
const string Registers :: EVI_NAME = "EVI";
const string Registers :: PI_NAME = "PI";
const string Registers :: SVR_NAME = "SVR";
const string Registers :: TFRR_NAME = "TFRR";
const string Registers :: GTCCR0_NAME = "GTCCR0";
const string Registers :: GTBCR0_NAME = "GTBCR0";
const string Registers :: GTVPR0_NAME = "GTVPR0";
const string Registers :: GTDR0_NAME = "GTDR0";
const string Registers :: GTCCR1_NAME = "GTCCR1";
const string Registers :: GTBCR1_NAME = "GTBCR1";
const string Registers :: GTVPR1_NAME = "GTVPR1";
const string Registers :: GTDR1_NAME = "GTDR1";
const string Registers :: GTCCR2_NAME = "GTCCR2";
const string Registers :: GTBCR2_NAME = "GTBCR2";
const string Registers :: GTVPR2_NAME = "GTVPR2";
const string Registers :: GTDR2_NAME = "GTDR2";
const string Registers :: GTCCR3_NAME = "GTCCR3";
const string Registers :: GTBCR3_NAME = "GTBCR3";
const string Registers :: GTVPR3_NAME = "GTVPR3";
const string Registers :: GTDR3_NAME = "GTDR3";
const string Registers :: IVPR0_NAME = "IVPR0";
const string Registers :: IDR0_NAME = "IDR0";
const string Registers :: IVPR1_NAME = "IVPR1";
const string Registers :: IDR1_NAME = "IDR1";
const string Registers :: IVPR2_NAME = "IVPR2";
const string Registers :: IDR2_NAME = "IVPR2";
const string Registers :: IVPR3_NAME = "IVPR3";
const string Registers :: IDR3_NAME = "IDR3";
const string Registers :: IVPR4_NAME = "IVPR4";
const string Registers :: IDR4_NAME = "IDR4";
const string Registers :: SVPR0_NAME = "SVPR0";
const string Registers :: SDR0_NAME = "SDR0";
const string Registers :: SVPR1_NAME = "SVPR1";
const string Registers :: SDR1_NAME = "SDR1";
const string Registers :: SVPR2_NAME = "SVPR2";
const string Registers :: SDR2_NAME = "SDR2";
const string Registers :: SVPR3_NAME = "SVPR3";
const string Registers :: SDR3_NAME = "SDR3";
const string Registers :: SVPR4_NAME = "SVPR4";
const string Registers :: SDR4_NAME = "SDR4";
const string Registers :: SVPR5_NAME = "SVPR5";
const string Registers :: SDR5_NAME = "SDR5";
const string Registers :: SVPR6_NAME = "SVPR6";
const string Registers :: SDR6_NAME = "SDR6";
const string Registers :: SVPR7_NAME = "SVPR7";
const string Registers :: SDR7_NAME = "SDR7";
const string Registers :: SVPR8_NAME = "SVPR8";
const string Registers :: SDR8_NAME = "SDR8";
const string Registers :: SVPR9_NAME = "SVPR9";
const string Registers :: SDR9_NAME = "SDR9";
const string Registers :: SVPR10_NAME = "SVPR10";
const string Registers :: SDR10_NAME = "SDR10";
const string Registers :: SVPR11_NAME = "SVPR11";
const string Registers :: SDR11_NAME = "SDR11";
const string Registers :: SVPR12_NAME = "SVPR12";
const string Registers :: SDR12_NAME = "SDR12";
const string Registers :: SVPR13_NAME = "SVPR13";
const string Registers :: SDR13_NAME = "SDR13";
const string Registers :: SVPR14_NAME = "SVPR14";
const string Registers :: SDR14_NAME = "SDR14";
const string Registers :: SVPR15_NAME = "SVPR15";
const string Registers :: SDR15_NAME = "SDR15";
const string Registers :: IIVPR0_NAME = "IIVPR0";
const string Registers :: IIDR0_NAME = "IIDR0";
const string Registers :: IIVPR1_NAME = "IIVPR1";
const string Registers :: IIDR1_NAME = "IIDR1";
const string Registers :: IIVPR2_NAME = "IIVPR2";
const string Registers :: IIDR2_NAME = "IIDR2";
const string Registers :: IIVPR3_NAME = "IIVPR3";
const string Registers :: IIDR3_NAME = "IIDR3";
const string Registers :: PCTPR_NAME = "PCTPR";
const string Registers :: IACK_NAME = "IACK";
const string Registers :: EOI_NAME = "EOI";
 
Registers :: 
Registers() {
		
	/* START registers initialization */
	/* Global and timer registers */
	frr = (uint32_t)0x0 
		| ((uint32_t)0x017 << 16) /* NIRQ */
		| ((uint32_t)0x0 << 8) /* NCPU */ 
		| (uint32_t)0x02; /* VID */
	gcr = (uint32_t)0x0
		| ((uint32_t)0x0 << 31) /* R */
		| ((uint32_t)0x0 << 29); /* M */
	eicr = (uint32_t)0x0
		| ((uint32_t)0x4 << 28) /* R */
		| ((uint32_t)0x0 << 27); /* SIE */
	evi = (uint32_t)0x0
		| ((uint32_t)0x01 << 16) /* STEP */
		| ((uint32_t)0x0 << 8) /* DEVICE_ID */
		| (uint32_t)0x0; /* VENDOR_ID */
	pi = (uint32_t)0x0
		| (uint32_t)0x0; /* P0 */
	svr = (uint32_t)0x0
		| (uint32_t)0xff; /* VECTOR */ 
	tfrr = (uint32_t)0x0; /* TIMER_FREQ */
	for(unsigned int i = 0; i < 4; i++) {
		gtccr[i] = (uint32_t)0x0
			| ((uint32_t)0x0 << 31) /* T */
			| (uint32_t)0x0; /* COUNT */
		gtbcr[i] = ((uint32_t)0x01 << 31) /* CI */
			| (uint32_t)0x0; /* BASE_COUNT */
		gtvpr[i] = (uint32_t)0x0
			| ((uint32_t)0x01 << 31) /* M */
			| ((uint32_t)0x0 << 30) /* A */
			| ((uint32_t)0x0 << 16) /* PRIORITY */
			| (uint32_t)0x0; /* VECTOR */ 
		gtdr[i] = (uint32_t)0x0
			| (uint32_t)0x01; /* P0 */
	}
		
	/* interrupt source configuration registers */
	for(unsigned int i = 0; i < 5; i++) {
		ivpr[i] = (uint32_t)0x0
			| ((uint32_t)0x01 << 31) /* M */
			| ((uint32_t)0x0 << 30) /* A */
			| ((uint32_t)0x0 << 23) /* P */
			| ((uint32_t)0x0 << 22) /* S */
			| ((uint32_t)0x0 << 16) /* PRIORITY */
			| (uint32_t)0x0; /* VECTOR */
		svpr[i] = ivpr[i]; 
	}
	for(unsigned int i = 6; i < 16; i++) {
		svpr[i] = (uint32_t)0x0
			| ((uint32_t)0x01 << 31) /* M */
			| ((uint32_t)0x0 << 30) /* A */
			| ((uint32_t)0x0 << 23) /* P */
			| ((uint32_t)0x0 << 22) /* S */
			| ((uint32_t)0x0 << 16) /* PRIORITY */
			| (uint32_t)0x0; /* VECTOR */
	}
	for(unsigned int i = 0; i < 5; i++) {
		idr[i] = (uint32_t)0x0
			| (uint32_t)0x01; /* P0 */
	}
	for(unsigned int i = 0; i < 16; i++) {
		sdr[i] = (uint32_t)0x0
			| (uint32_t)0x01; /* P0 */
	}
	for(unsigned int i = 0; i < 4; i++) {
		iivpr[i] = (uint32_t)0x0
			| ((uint32_t)0x01 << 31) /* M */
			| ((uint32_t)0x0 << 30) /* A */
			| ((uint32_t)0x0 << 16) /* PRIORITY */
			| (uint32_t)0x0; /* VECTOR */
		iidr[i] = (uint32_t)0x0
			| (uint32_t)0x01; /* P0 */
	}
	
	/* processor related registers */
	pctpr = (uint32_t)0x0
		| (uint32_t)0x0f; /* TASKP */
	iack = (uint32_t)0x0; /* VECTOR */
	eoi = (uint32_t)0x0; /* EOI_CODE */
	
}

Registers ::
~Registers() {
}

} // end of namespace epic
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
