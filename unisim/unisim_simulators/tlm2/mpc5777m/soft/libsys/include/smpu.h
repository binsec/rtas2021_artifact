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

#ifndef __SMPU_H__
#define __SMPU_H__

#include "typedefs.h"

enum SMPU_EATTR
{
	SMPU_EATTR_USER_MODE_INSN_ACCESS       = 0,
	SMPU_EATTR_USER_MODE_DATA_ACCESS       = 1,
	SMPU_EATTR_SUPERVISOR_MODE_INSN_ACCESS = 2,
	SMPU_EATTR_SUPERVISOR_MODE_DATA_ACCESS = 3
};

enum SMPU_ERW
{
	SMPU_ERW_READ  = 0,
	SMPU_ERW_WRITE = 1
};

enum SMPU_MP
{
	SMPU_MP_NONE       = 0,
	SMPU_MP_WRITE      = 1,
	SMPU_MP_READ       = 2,
	SMPU_MP_READ_WRITE = 3
};

void smpu_drv_init();
void smpu_init(unsigned int smpu_id);
void smpu_enable(unsigned int smpu_id);
void smpu_disable(unsigned int smpu_id);
unsigned int smpu_get_master_error_flag(unsigned int smpu_id, unsigned int master_id);
unsigned int smpu_get_hardware_revision_level(unsigned int smpu_id);
unsigned int smpu_get_master_error_overrun_flag(unsigned int smpu_id, unsigned int master_id);
unsigned int smpu_get_number_of_region_descriptors(unsigned int smpu_id);
uint32_t smpu_get_master_error_address(unsigned int smpu_id, unsigned int master_id);
unsigned int smpu_get_master_error_access_control_detail(unsigned int smpu_id, unsigned int master_id);
enum SMPU_EATTR smpu_get_master_error_attributes(unsigned int smpu_id, unsigned int master_id);
enum SMPU_ERW smpu_get_master_error_read_write(unsigned int smpu_id, unsigned int master_id);
unsigned smpu_get_error_master_number(unsigned int smpu_id, unsigned int master_id);
void smpu_region_descriptor_set_start_address(unsigned int smpu_id, unsigned int rgd_id, uint32_t start_address);
uint32_t smpu_region_descriptor_get_start_address(unsigned int smpu_id, unsigned int rgd_id);
void smpu_region_descriptor_set_end_address(unsigned int smpu_id, unsigned int rgd_id, uint32_t start_address);
uint32_t smpu_region_descriptor_get_end_address(unsigned int smpu_id, unsigned int rgd_id);
void smpu_region_descriptor_set_master_permission(unsigned int smpu_id, unsigned int rgd_id, unsigned int master_id, enum SMPU_MP mp);
enum SMPU_MP smpu_region_descriptor_get_master_permission(unsigned int smpu_id, unsigned int rgd_id, unsigned int master_id);
unsigned int smpu_region_descriptor_get_format(unsigned int smpu_id, unsigned int rgd_id);
void smpu_region_descriptor_set_read_only_flag(unsigned int smpu_id, unsigned int rgd_id, unsigned int ro_flag);
unsigned int smpu_region_descriptor_get_read_only_flag(unsigned int smpu_id, unsigned int rgd_id);
void smpu_region_descriptor_set_cache_inhibit_flag(unsigned int smpu_id, unsigned int rgd_id, unsigned int ci_flag);
unsigned int smpu_region_descriptor_get_cache_inhibit_flag(unsigned int smpu_id, unsigned int rgd_id);
void smpu_region_descriptor_set_valid_flag(unsigned int smpu_id, unsigned int rgd_id, unsigned int vld_flag);

#endif
