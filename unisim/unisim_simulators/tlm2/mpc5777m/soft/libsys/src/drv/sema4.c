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

#include "sema4.h"
#include "MPC5777M.h"
#include "compiler_api.h"

static unsigned int sema4_get_core_id()
{
	return MFSPR(286); // read PIR: our processor number
}

void sema4_drv_init()
{
}

void sema4_init()
{
}

int sema4_trylock_gate(unsigned int gate)
{
	unsigned int gtfsm = sema4_get_core_id() + 1;
	SEMA42.GATE[gate].B.GTFSM = gtfsm;
	return (SEMA42.GATE[gate].B.GTFSM == gtfsm);
}

void sema4_lock_gate(unsigned int gate)
{
	while(!sema4_trylock_gate(gate))
	{
		PPC_WAIT;
	}
}

void sema4_unlock_gate(unsigned int gate)
{
	unsigned int gtfsm = sema4_get_core_id() + 1;
	if(SEMA42.GATE[gate].B.GTFSM == gtfsm)
	{
		SEMA42.GATE[gate].B.GTFSM = 0;
	}
}

void sema4_reset_gate(unsigned int gate)
{
	unsigned int gms = sema4_get_core_id();
	SEMA42_RSTGT_W_tag sema42_rstgt_w;
	sema42_rstgt_w.R = 0;
	sema42_rstgt_w.B.RSTGTN = gate;
	
	do
	{
		switch(SEMA42.RSTGT.R.B.RSTGSM)
		{
			case 0: 
				sema42_rstgt_w.B.RSTGDP = 0xe2;
				break;
			case 1:
				sema42_rstgt_w.B.RSTGDP = 0x1d;
				break;
		}
		
		SEMA42.RSTGT.W.R = sema42_rstgt_w.R;
	}
	while(SEMA42.RSTGT.R.B.RSTGMS != gms);
}

void sema4_reset_all_gates()
{
	sema4_reset_gate(64);
}
