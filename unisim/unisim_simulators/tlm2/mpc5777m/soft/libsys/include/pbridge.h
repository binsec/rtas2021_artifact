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

#ifndef __PBRIDGE_H__
#define __PBRIDGE_H__

#include "typedefs.h"

enum MPROT
{
	MPROT_MPL = 0x1,
	MPROT_MTW = 0x2,
	MPROT_MTR = 0x4
};

enum PACR
{
	PACR_TP = 0x1,
	PACR_WP = 0x2,
	PACR_SP = 0x4
};

enum OPACR
{
	OPACR_TP = 0x1,
	OPACR_WP = 0x2,
	OPACR_SP = 0x4
};

void pbridge_drv_init();
void pbridge_init(unsigned int pbridge_id);
void pbridge_set_master_privilege(unsigned int pbridge_id, unsigned int master_id, unsigned int mprot);
void pbridge_set_peripheral_access_control(unsigned int pbridge_id, unsigned int pacr_id, unsigned int pacr);
void pbridge_set_off_platform_peripheral_access_control(unsigned int pbridge_id, unsigned int opacr_id, unsigned int opacr);

#endif
