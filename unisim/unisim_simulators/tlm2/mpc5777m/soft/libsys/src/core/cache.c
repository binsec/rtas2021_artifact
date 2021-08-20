/*
 *  Copyright (c) 2017,
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

#include "compiler_api.h"
#include "typedefs.h"

#define L1CSR0 1010
#define DCE   0x1
#define DCINV 0x2
#define DCABT 0x4

#define L1CSR1 1011
#define ICE   0x1
#define ICINV 0x2
#define ICABT 0x4

void icache_enable()
{
	uint32_t l1csr1 = MFSPR(L1CSR1);
	l1csr1 |= ICE; // set ICE
	PPC_ISYNC;
	MTSPR(L1CSR1, l1csr1);
}

void dcache_enable()
{
	uint32_t l1csr0 = MFSPR(L1CSR0);
	l1csr0 |= DCE; // set DCE
	PPC_ISYNC;
	MTSPR(L1CSR0, l1csr0);
}

void icache_invalidate()
{
	do
	{
		/* invalidate */
		uint32_t l1csr1 = MFSPR(L1CSR1);
		l1csr1 |= ICINV; // set ICINV
		MTSPR(L1CSR1, l1csr1);

		l1csr1 = MFSPR(L1CSR1);
		if(l1csr1 & ICABT)
		{
			/* icache operation aborted */
			l1csr1 &= ~ICABT; // clear ICABT
			MTSPR(L1CSR1, l1csr1);
		}
		else if(!(l1csr1 & ICINV))
		{
			/* icache invalidation finished */
			break;
		}
	}
	while(1);
}

void dcache_invalidate()
{
	do
	{
		/* invalidate */
		uint32_t l1csr0 = MFSPR(L1CSR0);
		l1csr0 |= DCINV; // set DCINV
		MTSPR(L1CSR0, l1csr0);

		l1csr0 = MFSPR(L1CSR0);
		if(l1csr0 & DCABT)
		{
			/* dcache operation aborted */
			l1csr0 &= ~DCABT; // clear DCABT
			MTSPR(L1CSR0, l1csr0);
		}
		else if(!(l1csr0 & DCINV))
		{
			/* dcache invalidation finished */
			break;
		}
	}
	while(1);
}
