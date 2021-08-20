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

#ifndef __PIT_H__
#define __PIT_H__

#include "typedefs.h"

typedef void (*pit_timer_int_handler_t)(unsigned int, unsigned int);
typedef void (*pit_rti_timer_int_handler_t)(unsigned int);

void pit_drv_init();

void pit_init(unsigned int pit_id);
void pit_enable_timers_clock(unsigned int pit_id);
void pit_disable_timers_clock(unsigned int pit_id);
void pit_enable_rti_timer_clock(unsigned int pit_id);
void pit_disable_rti_timer_clock(unsigned int pit_id);
uint64_t pit_get_lifetime(unsigned int pit_id);

void pit_enable_rti_timer(unsigned int pit_id);
void pit_disable_rti_timer(unsigned int pit_id);
void pit_enable_rti_timer_interrupt(unsigned int pit_id);
void pit_disable_rti_timer_interrupt(unsigned int pit_id);
void pit_set_rti_timer_load_value(unsigned int pit_id, uint32_t value);
uint32_t pit_get_rti_timer_load_value(unsigned int pit_id);
uint32_t pit_get_rti_timer_value(unsigned int pit_id);
int pit_get_rti_timer_interrupt_flag(unsigned int pit_id);
void pit_clear_rti_timer_interrupt_flag(unsigned int pit_id);
pit_rti_timer_int_handler_t pit_set_rti_timer_interrupt_handler(unsigned int pit_id, pit_rti_timer_int_handler_t pit_rti_timer_int_handler);
void pit_set_rti_timer_irq_priority(unsigned int pit_id, unsigned int priority);
void pit_select_rti_timer_irq_for_processor(unsigned int pit_id, unsigned int prc_num);
void pit_deselect_rti_timer_irq_for_processor(unsigned int pit_id, unsigned int prc_num);

void pit_enable_timer(unsigned int pit_id, unsigned int chan);
void pit_disable_timer(unsigned int pit_id, unsigned int chan);
void pit_chain_timer(unsigned int pit_id, unsigned int chan);
void pit_unchain_timer(unsigned int pit_id, unsigned int chan);
void pit_enable_timer_interrupt(unsigned int pit_id, unsigned int chan);
void pit_disable_timer_interrupt(unsigned int pit_id, unsigned int chan);
void pit_set_timer_load_value(unsigned int pit_id, unsigned int chan, uint32_t value);
uint32_t pit_get_timer_load_value(unsigned int pit_id, unsigned int chan);
uint32_t pit_get_timer_value(unsigned int pit_id, unsigned int chan);
int pit_get_timer_interrupt_flag(unsigned int pit_id, unsigned int chan);
void pit_clear_timer_interrupt_flag(unsigned int pit_id, unsigned int chan);
pit_timer_int_handler_t pit_set_timer_interrupt_handler(unsigned int pit_id, unsigned int chan, pit_timer_int_handler_t pit_timer_int_handler);
void pit_set_timer_irq_priority(unsigned int pit_id, unsigned int chan, unsigned int priority);
void pit_select_timer_irq_for_processor(unsigned int pit_id, unsigned int chan, unsigned int prc_num);
void pit_deselect_timer_irq_for_processor(unsigned int pit_id, unsigned int chan, unsigned int prc_num);

#endif
