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
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_REGISTER_HH__

#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace dma {
	
using std::string;

class Registers {
public:
	Registers();
	~Registers();

	/* registers containers */
	uint32_t dmr[2],
		 dsr[2],
		 cdar[2],
		 sar[2],
		 dar[2],
		 bcr[2],
		 ndar[2];

	/* registers addresses */
	static const uint32_t DMR0 = (uint32_t)0x01100,
		DSR0 = (uint32_t)0x01104,
		CDAR0 = (uint32_t)0x01108,
		SAR0 = (uint32_t)0x01110,
		DAR0 = (uint32_t)0x01118,
		BCR0 = (uint32_t)0x01120,
		NDAR0 = (uint32_t)0x01124,
		DMR1 = (uint32_t)0x01200,
		DSR1 = (uint32_t)0x01204,
		CDAR1 = (uint32_t)0x01208,
		SAR1 = (uint32_t)0x01210,
		DAR1 = (uint32_t)0x01218,
		BCR1 = (uint32_t)0x01220,
		NDAR1 = (uint32_t)0x01224;

	/* registers reserved bit masks */
	static const uint32_t 
	DMR_RESERVED_MASK =     (uint32_t)0xff800570,
		DSR_RESERVED_MASK =   (uint32_t)0xffffff68,
		CDAR_RESERVED_MASK =  (uint32_t)0x00000001,
		SAR_RESERVED_MASK =   (uint32_t)0x00000000,
		DAR_RESERVED_MASK =   (uint32_t)0x00000000,
		BCR_RESERVED_MASK =   (uint32_t)0xfc000000,
		NDAR_RESERVED_MASK =  (uint32_t)0x00000000;

	/* DMR field masks */
	static const uint32_t
	DMR_LMDC =     (uint32_t)0x00300000,
		DMR_IRQS =   (uint32_t)0x00080000,
		DMR_PDE =    (uint32_t)0x00040000,
		DMR_DAHTS =  (uint32_t)0x00030000,
		DMR_SAHTS =  (uint32_t)0x0000c000,
		DMR_DAHE =   (uint32_t)0x00002000,
		DMR_SAHE =   (uint32_t)0x00001000,
		DMR_PRC =    (uint32_t)0x00000c00,
		DMR_EIE =    (uint32_t)0x00000100,
		DMR_EOTIE =  (uint32_t)0x00000080,
		DMR_DL =     (uint32_t)0x00000008,
		DMR_CTM =    (uint32_t)0x00000004,
		DMR_CC =     (uint32_t)0x00000002,
		DMR_CS =     (uint32_t)0x00000001;
	/* DSR field masks */
	static const uint32_t
	DSR_LME =        (uint32_t)0x00000080,
		DSR_PE =       (uint32_t)0x00000010,
		DSR_CB =       (uint32_t)0x00000004,
		DSR_EOSI =     (uint32_t)0x00000002,
		DSR_EOCAI =    (uint32_t)0x00000001;
	/* CDAR field masks */
	static const uint32_t
	CDAR_CDA =       (uint32_t)0xffffffe0,
		CDAR_SNEN =    (uint32_t)0x00000010,
		CDAR_EOSIE =   (uint32_t)0x00000008,
		CDAR_CTT =     (uint32_t)0x00000006;
	/* SAR field masks */
	static const uint32_t
	SAR_SAR =        (uint32_t)0xffffffff;
	/* DAR field masks */
	static const uint32_t
	DAR_DAR =        (uint32_t)0xffffffff;
	/* BCR field masks */
	static const uint32_t
	BCR_BCR =        (uint32_t)0x03ffffff;
	/* NDAR field masks */
	static const uint32_t
	NDAR_NDA =       (uint32_t)0xffffffe0,
		NDAR_NDSNEN =  (uint32_t)0x00000010,
		NDAR_NDEOSIE = (uint32_t)0x00000008,
		NDAR_NDCTT =   (uint32_t)0x00000006,
		NDAR_EOTD =    (uint32_t)0x00000001;

	/* registers names */
	static const string
		DMR_NAME_0,
		DSR_NAME_0,
		CDAR_NAME_0,
		SAR_NAME_0,
		DAR_NAME_0,
		BCR_NAME_0,
		NDAR_NAME_0,
		DMR_NAME_1,
		DSR_NAME_1,
		CDAR_NAME_1,
		SAR_NAME_1,
		DAR_NAME_1,
		BCR_NAME_1,
		NDAR_NAME_1;

private:
};

} // end of namespace dma
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_DMA_REGISTER_HH__
