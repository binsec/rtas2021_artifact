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

#ifndef __SWT_H__
#define __SWT_H__

#include "typedefs.h"

typedef void (*swt_int_handler_t)(unsigned int);

enum _SWT_SMD
{
	SMD_FIXED_SERVICE_SEQUENCE        = 0, // 0b00
	SMD_KEYED_SERVICE_SEQUENCE        = 1, // 0b01
	SMD_FIXED_ADDRESS_EXECUTION       = 2, // 0b10
	SMD_INCREMENTAL_ADDRESS_EXECUTION = 3  // 0b11
};

typedef enum _SWT_SMD SWT_SMD;

void swt_drv_init();
void swt_init(unsigned int swt_id);
void swt_enable(unsigned int swt_id);
void swt_disable(unsigned int swt_id);
void swt_set_service_mode(unsigned int swt_id, SWT_SMD smd);
SWT_SMD swt_get_service_mode(unsigned int swt_id);
void swt_enable_window_mode(unsigned int swt_id);
void swt_disable_window_mode(unsigned int swt_id);
void swt_enable_reset_on_invalid_access(unsigned int swt_id);
void swt_disable_reset_on_invalid_access(unsigned int swt_id);
void swt_enable_interrupt_then_reset(unsigned int swt_id);
void swt_disable_interrupt_then_reset(unsigned int swt_id);
void swt_hard_lock(unsigned int swt_id);
int swt_is_soft_locked(unsigned int swt_id);
int swt_is_hard_locked(unsigned int swt_id);
int swt_is_locked(unsigned int swt_id);
void swt_soft_lock(unsigned int swt_id);
void swt_soft_unlock(unsigned int swt_id);
void swt_set_timeout(unsigned int swt_id, uint32_t value);
void swt_set_window(unsigned int swt_id, uint32_t value);
uint32_t swt_get_counter(unsigned int swt_id);
void swt_set_service_key(unsigned int swt_id, uint16_t value);
uint16_t swt_get_service_key(unsigned int swt_id);
void swt_service_sequence(unsigned int swt_id);
void swt_clear_interrupt_flag(unsigned int swt_id);
unsigned int swt_get_interrupt_flag(unsigned int swt_id);
swt_int_handler_t swt_set_interrupt_handler(unsigned int swt_id, swt_int_handler_t swt_int_handler);

#endif
