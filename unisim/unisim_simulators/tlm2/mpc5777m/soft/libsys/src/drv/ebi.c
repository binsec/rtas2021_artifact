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

#include "ebi.h"
#include "MPC5777M.h"

void ebi_drv_init()
{
}

void ebi_init()
{
}

void ebi_enable_module()
{
	EBI.MCR.B.MDIS = 0;
}

void ebi_disable_module()
{
	EBI.MCR.B.MDIS = 1;
}

void ebi_set_bank_base_address(unsigned int bank, uint32_t base_address)
{
	uint32_t ba = base_address >> 15;
	switch(bank)
	{
		case 0: EBI.BR0.B.BA = ba; break;
		case 1: EBI.BR1.B.BA = ba; break;
		case 2: EBI.BR2.B.BA = ba; break;
	}
}

void ebi_set_bank_port_size(unsigned int bank, enum EBI_PORT_SIZE port_size)
{
	switch(bank)
	{
		case 0:
			switch(port_size)
			{
				case EBI_PORT_SIZE_8 : EBI.OR0.B.APS = 0; break;
				case EBI_PORT_SIZE_16: EBI.OR0.B.APS = 1; EBI.BR0.B.PS = 1; break;
				case EBI_PORT_SIZE_32: EBI.OR0.B.APS = 1; EBI.BR0.B.PS = 0; break;
			}
			break;
		case 1:
			switch(port_size)
			{
				case EBI_PORT_SIZE_8 : EBI.OR1.B.APS = 0; break;
				case EBI_PORT_SIZE_16: EBI.OR1.B.APS = 1; EBI.BR1.B.PS = 1; break;
				case EBI_PORT_SIZE_32: EBI.OR1.B.APS = 1; EBI.BR1.B.PS = 0; break;
			}
			break;
		case 2:
			switch(port_size)
			{
				case EBI_PORT_SIZE_8 : EBI.OR2.B.APS = 0; break;
				case EBI_PORT_SIZE_16: EBI.OR2.B.APS = 1; EBI.BR2.B.PS = 1; break;
				case EBI_PORT_SIZE_32: EBI.OR2.B.APS = 1; EBI.BR2.B.PS = 0; break;
			}
			break;
	}
}

void ebi_set_bank_valid_flag(unsigned int bank, int valid_flag)
{
	switch(bank)
	{
		case 0: EBI.BR0.B.V = valid_flag; break;
		case 1: EBI.BR1.B.V = valid_flag; break;
		case 2: EBI.BR2.B.V = valid_flag; break;
	}
}

void ebi_set_bank_address_mask(unsigned int bank, uint32_t address_mask)
{
	uint32_t am = address_mask >> 15;
	switch(bank)
	{
		case 0: EBI.OR0.B.AM = am; break;
		case 1: EBI.OR1.B.AM = am; break;
		case 2: EBI.OR2.B.AM = am; break;
	}
}
