/*
 *  Copyright (c) 2018,
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

#include "pbridge.h"
#include "MPC5777M.h"

static volatile struct PBRIDGE_tag *pbridge[2];

void pbridge_drv_init()
{
	pbridge[0] = &PBRIDGE_A;
	pbridge[1] = &PBRIDGE_B;
}

void pbridge_init(unsigned int pbridge_id)
{
}

void pbridge_set_master_privilege(unsigned int pbridge_id, unsigned int master_id, unsigned int mprot)
{
	if(master_id < 16)
	{
		unsigned int idx = master_id / 8;
		unsigned int mod = master_id % 8;
		unsigned int sh = 4 * (7 - mod);
		uint32_t msk = ((uint32_t) 0xf) << sh;
		volatile uint32_t *mpr = (idx ? &pbridge[pbridge_id]->MPRB.R : &pbridge[pbridge_id]->MPRA.R);
		uint32_t tmp = ((*mpr) & ~msk) | (((uint32_t) mprot << sh) & msk);
		*mpr = tmp;
	}
}

void pbridge_set_peripheral_access_control(unsigned int pbridge_id, unsigned int pacr_id, unsigned int pacr)
{
	unsigned int idx = pacr_id / 8;
	unsigned int mod = pacr_id % 8;
	unsigned int sh = 4 * (7 - mod);
	uint32_t msk = ((uint32_t) 0xf) << sh;
	uint32_t tmp = (pbridge[pbridge_id]->PACR[idx].R & ~msk) | (((uint32_t) pacr << sh) & msk);
	pbridge[pbridge_id]->PACR[idx].R = tmp;
}

void pbridge_set_off_platform_peripheral_access_control(unsigned int pbridge_id, unsigned int opacr_id, unsigned int opacr)
{
	unsigned int idx = opacr_id / 8;
	unsigned int mod = opacr_id % 8;
	unsigned int sh = 4 * (7 - mod);
	uint32_t msk = ((uint32_t) 0xf) << sh;
	uint32_t tmp = (pbridge[pbridge_id]->OPACR[idx].R & ~msk) | (((uint32_t) opacr << sh) & msk);
	pbridge[pbridge_id]->OPACR[idx].R = tmp;
}
