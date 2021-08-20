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

#ifndef __STM_H__
#define __STM_H__

#include "typedefs.h"

typedef void (*stm_int_handler_t)(unsigned int, unsigned int);

void stm_drv_init();
void stm_init(unsigned int stm_id);
void stm_set_channel_irq_priority(unsigned int stm_id, unsigned int chan, unsigned int priority);
void stm_select_channel_irq_for_processor(unsigned int stm_id, unsigned int chan, unsigned int prc_num);
void stm_deselect_channel_irq_for_processor(unsigned int stm_id, unsigned int chan, unsigned int prc_num);
void stm_set_counter_prescale(unsigned int stm_id, unsigned int prescale);
unsigned int stm_get_counter_prescale(unsigned int stm_id);
void stm_enable_counter(unsigned int stm_id);
void stm_disable_counter(unsigned int stm_id);
void stm_set_counter(unsigned int stm_id, uint32_t counter);
uint32_t stm_get_counter(unsigned int stm_id);
void stm_enable_channel(unsigned int stm_id, unsigned int chan);
void stm_disable_channel(unsigned int stm_id, unsigned int chan);
void stm_set_channel_compare(unsigned int stm_id, unsigned int chan, uint32_t cmp);
uint32_t stm_get_channel_compare(unsigned int stm_id, unsigned int chan);
void stm_clear_interrupt_flag(unsigned int stm_id, unsigned int chan);
unsigned int stm_get_interrupt_flag(unsigned int stm_id, unsigned int chan);
stm_int_handler_t stm_set_interrupt_handler(unsigned int stm_id, unsigned int chan, stm_int_handler_t stm_int_handler);

#endif
