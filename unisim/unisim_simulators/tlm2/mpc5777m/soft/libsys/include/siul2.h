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

#ifndef __SIUL2_H__
#define __SIUL2_H__

#include "typedefs.h"

enum OUTPUT_DRIVE_CONTROL
{
	ODC_OUTPUT_BUFFER_DISABLED   = 0x0,
	ODC_OPEN_DRAIN               = 0x1,
	ODC_PUSH_PULL                = 0x2,
	ODC_OPEN_SOURCE              = 0x3,
	ODC_MICROSECOND_CHANNEL_LVDS = 0x4,
	ODC_LFAST_LVDS               = 0x5,
};

void siul2_drv_init();
void siul2_init();
void siul2_gpio_write(unsigned int port, uint8_t value);
uint8_t siul2_gpio_read(unsigned int port);
void siul2_gpio_parallel_write(unsigned int port, uint16_t value);
uint16_t siul2_gpio_parallel_read(unsigned int port);
void siul2_set_output_drive_control(unsigned int port, enum OUTPUT_DRIVE_CONTROL odc);

#endif
