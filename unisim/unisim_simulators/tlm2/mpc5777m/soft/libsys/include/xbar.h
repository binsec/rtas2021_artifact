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

#ifndef __XBAR_H__
#define __XBAR_H__

#include "typedefs.h"

enum XBAR_ARBITRATION_MODE
{
	XBAR_ARB_FIXED_PRIORITY       = 0,
	XBAR_ARB_ROUND_ROBIN_PRIORITY = 1,
};

enum XBAR_PARKING_CONTROL
{
	XBAR_PCTL_PARK    = 0,
	XBAR_PCTL_LAST    = 1,
	XBAR_PCTL_NO_PARK = 2
};

void xbar_drv_init();
void xbar_init(unsigned int xbar_id);
void xbar_set_master_priority_per_slave_port(unsigned int xbar_id, unsigned int slave_port, unsigned int master_port, unsigned int prio);
void xbar_set_slave_port_read_only_flag(unsigned int xbar_id, unsigned int slave_port, int ro_flag);
void xbar_enable_master_high_priority_per_slave_port(unsigned int xbar_id, unsigned int slave_port);
void xbar_disable_master_high_priority_per_slave_port(unsigned int xbar_id, unsigned int slave_port);
void xbar_set_master_high_request_priority_per_slave_port(unsigned int xbar_id, unsigned int slave_port, unsigned int master_port, unsigned int prio_elevation);
void xbar_set_arbitration_mode_per_slave_port(unsigned int xbar_id, unsigned int slave_port, enum XBAR_ARBITRATION_MODE arb);
void xbar_set_parking_control_per_slave_port(unsigned int xbar_id, unsigned int slave_port, enum XBAR_PARKING_CONTROL pctl);
void xbar_set_park_per_slave_port(unsigned int xbar_id, unsigned int slave_port, unsigned master_port);

#endif
