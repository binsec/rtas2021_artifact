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

#include "smpu.h"
#include "MPC5777M.h"

static volatile struct SMPU_tag *smpu[2];

void smpu_drv_init()
{
	smpu[0] = &SMPU_0;
	smpu[1] = &SMPU_1;
}

void smpu_init(unsigned int smpu_id)
{
}

void smpu_enable(unsigned int smpu_id)
{
	smpu[smpu_id]->CESR0.B.GVLD = 1;
}

void smpu_disable(unsigned int smpu_id)
{
	smpu[smpu_id]->CESR0.B.GVLD = 0;
}

unsigned int smpu_get_master_error_flag(unsigned int smpu_id, unsigned int master_id)
{
	return (smpu[smpu_id]->CESR0.B.MERR >> (15 - master_id)) & 1;
}

unsigned int smpu_get_hardware_revision_level(unsigned int smpu_id)
{
	return smpu[smpu_id]->CESR0.B.HRL;
}

unsigned int smpu_get_master_error_overrun_flag(unsigned int smpu_id, unsigned int master_id)
{
	return (smpu[smpu_id]->CESR1.B.MEOVR >> (15 - master_id)) & 1;
}

unsigned int smpu_get_number_of_region_descriptors(unsigned int smpu_id)
{
	return smpu[smpu_id]->CESR1.B.NRGD;
}

uint32_t smpu_get_master_error_address(unsigned int smpu_id, unsigned int master_id)
{
	return smpu[smpu_id]->ERROR[master_id].EAR.B.EADDR;
}

unsigned int smpu_get_master_error_access_control_detail(unsigned int smpu_id, unsigned int master_id)
{
	return smpu[smpu_id]->ERROR[master_id].EDR.B.EACD;
}

enum SMPU_EATTR smpu_get_master_error_attributes(unsigned int smpu_id, unsigned int master_id)
{
	return (enum SMPU_EATTR) smpu[smpu_id]->ERROR[master_id].EDR.B.EATTR;
}

enum SMPU_ERW smpu_get_master_error_read_write(unsigned int smpu_id, unsigned int master_id)
{
	return (enum SMPU_ERW) smpu[smpu_id]->ERROR[master_id].EDR.B.ERW;
}

unsigned smpu_get_error_master_number(unsigned int smpu_id, unsigned int master_id)
{
	return smpu[smpu_id]->ERROR[master_id].EDR.B.EMN;
}

void smpu_region_descriptor_set_start_address(unsigned int smpu_id, unsigned int rgd_id, uint32_t start_address)
{
	smpu[smpu_id]->RGD[rgd_id].WORD0.B.SRTADDR = start_address;
}

uint32_t smpu_region_descriptor_get_start_address(unsigned int smpu_id, unsigned int rgd_id)
{
	return smpu[smpu_id]->RGD[rgd_id].WORD0.B.SRTADDR;
}

void smpu_region_descriptor_set_end_address(unsigned int smpu_id, unsigned int rgd_id, uint32_t start_address)
{
	smpu[smpu_id]->RGD[rgd_id].WORD1.B.ENDADDR = start_address;
}

uint32_t smpu_region_descriptor_get_end_address(unsigned int smpu_id, unsigned int rgd_id)
{
	return smpu[smpu_id]->RGD[rgd_id].WORD1.B.ENDADDR;
}

void smpu_region_descriptor_set_master_permission(unsigned int smpu_id, unsigned int rgd_id, unsigned int master_id, enum SMPU_MP mp)
{
	switch(master_id)
	{
		case 0: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M0P = mp; break;
		case 1: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M1P = mp; break;
		case 2: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M2P = mp; break;
		case 3: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M3P = mp; break;
		case 4: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M4P = mp; break;
		case 5: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M5P = mp; break;
		case 6: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M6P = mp; break;
		case 7: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M7P = mp; break;
		case 8: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M8P = mp; break;
		case 9: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M9P = mp; break;
		case 10: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M10P = mp; break;
		case 11: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M11P = mp; break;
		case 12: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M12P = mp; break;
		case 13: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M13P = mp; break;
		case 14: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M14P = mp; break;
		case 15: smpu[smpu_id]->RGD[rgd_id].WORD2.B.M15P = mp; break;
	}
}

enum SMPU_MP smpu_region_descriptor_get_master_permission(unsigned int smpu_id, unsigned int rgd_id, unsigned int master_id)
{
	switch(master_id)
	{
		case 0: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M0P;
		case 1: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M1P;
		case 2: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M2P;
		case 3: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M3P;
		case 4: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M4P;
		case 5: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M5P;
		case 6: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M6P;
		case 7: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M7P;
		case 8: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M8P;
		case 9: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M9P;
		case 10: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M10P;
		case 11: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M11P;
		case 12: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M12P;
		case 13: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M13P;
		case 14: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M14P;
		case 15: return (enum SMPU_MP) smpu[smpu_id]->RGD[rgd_id].WORD2.B.M15P;
	}
	
	return SMPU_MP_NONE;
}

unsigned int smpu_region_descriptor_get_format(unsigned int smpu_id, unsigned int rgd_id)
{
	return smpu[smpu_id]->RGD[rgd_id].WORD3.B.FMT;
}

void smpu_region_descriptor_set_read_only_flag(unsigned int smpu_id, unsigned int rgd_id, unsigned int ro_flag)
{
	smpu[smpu_id]->RGD[rgd_id].WORD3.B.RO = ro_flag;
}

unsigned int smpu_region_descriptor_get_read_only_flag(unsigned int smpu_id, unsigned int rgd_id)
{
	return smpu[smpu_id]->RGD[rgd_id].WORD3.B.RO;
}

void smpu_region_descriptor_set_cache_inhibit_flag(unsigned int smpu_id, unsigned int rgd_id, unsigned int ci_flag)
{
	smpu[smpu_id]->RGD[rgd_id].WORD3.B.CI = ci_flag;
}

unsigned int smpu_region_descriptor_get_cache_inhibit_flag(unsigned int smpu_id, unsigned int rgd_id)
{
	return smpu[smpu_id]->RGD[rgd_id].WORD3.B.CI;
}

void smpu_region_descriptor_set_valid_flag(unsigned int smpu_id, unsigned int rgd_id, unsigned int vld_flag)
{
	smpu[smpu_id]->RGD[rgd_id].WORD3.B.VLD = vld_flag;
}

unsigned int smpu_region_descriptor_get_valid_flag(unsigned int smpu_id, unsigned int rgd_id)
{
	return smpu[smpu_id]->RGD[rgd_id].WORD3.B.VLD;
}
