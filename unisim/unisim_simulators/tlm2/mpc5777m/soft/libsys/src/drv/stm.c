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

#include "stm.h"
#include "intc.h"
#include "MPC5777M.h"

static void stm_0_cir0_cif();
static void stm_0_cir1_cif();
static void stm_0_cir2_cif();
static void stm_0_cir3_cif();
static void stm_1_cir0_cif();
static void stm_1_cir1_cif();
static void stm_1_cir2_cif();
static void stm_1_cir3_cif();
static void stm_2_cir0_cif();
static void stm_2_cir1_cif();
static void stm_2_cir2_cif();
static void stm_2_cir3_cif();

void stm_int_handler_default(unsigned int stm_id, unsigned int chan)
{
	stm_clear_interrupt_flag(stm_id, chan);
}

static volatile struct STM_tag *stm[3];
static stm_int_handler_t stm_int_handlers[3][4];

static unsigned int stm_get_channel_irq_vector(unsigned int stm_id, unsigned int chan)
{
	return 36 + (stm_id * 4) + chan;
}

void stm_drv_init()
{
	stm[0] = &STM_0;
	stm[1] = &STM_1;
	stm[2] = &STM_2;
	unsigned stm_id;
	for(stm_id = 0; stm_id < 3; stm_id++)
	{
		unsigned int chan;
		for(chan = 0; chan < 4; chan++)
		{
			stm_int_handlers[stm_id][chan] = stm_int_handler_default;
		}
	}
}

void stm_init(unsigned int stm_id)
{
	stm_disable_counter(stm_id);
	stm_set_counter(stm_id, 0);
	unsigned int chan;
	for(chan = 0; chan < 4; chan++)
	{
		stm_disable_channel(stm_id, chan);
		stm_set_channel_compare(stm_id, chan, 0);
		stm_int_handlers[stm_id][chan] = stm_int_handler_default;
	}
	
	intc_set_interrupt_handler(stm_get_channel_irq_vector(0, 0), &stm_0_cir0_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(0, 1), &stm_0_cir1_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(0, 2), &stm_0_cir2_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(0, 3), &stm_0_cir3_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(1, 0), &stm_1_cir0_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(1, 1), &stm_1_cir1_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(1, 2), &stm_1_cir2_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(1, 3), &stm_1_cir3_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(2, 0), &stm_2_cir0_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(2, 1), &stm_2_cir1_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(2, 2), &stm_2_cir2_cif);
	intc_set_interrupt_handler(stm_get_channel_irq_vector(2, 3), &stm_2_cir3_cif);
}

void stm_set_channel_irq_priority(unsigned int stm_id, unsigned int chan, unsigned int priority)
{
	intc_set_irq_priority(stm_get_channel_irq_vector(stm_id, chan), priority);
}

void stm_select_channel_irq_for_processor(unsigned int stm_id, unsigned int chan, unsigned int prc_num)
{
	intc_select_irq_for_processor(stm_get_channel_irq_vector(stm_id, chan), prc_num);
}

void stm_deselect_channel_irq_for_processor(unsigned int stm_id, unsigned int chan, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(stm_get_channel_irq_vector(stm_id, chan), prc_num);
}

void stm_set_counter_prescale(unsigned int stm_id, unsigned int prescale)
{
	stm[stm_id]->CR.B.CPS = prescale;
}

unsigned int stm_get_counter_prescale(unsigned int stm_id)
{
	return stm[stm_id]->CR.B.CPS;
}

void stm_enable_counter(unsigned int stm_id)
{
	stm[stm_id]->CR.B.TEN = 1;
}

void stm_disable_counter(unsigned int stm_id)
{
	stm[stm_id]->CR.B.TEN = 0;
}

void stm_set_counter(unsigned int stm_id, uint32_t counter)
{
	stm[stm_id]->CNT.R = counter;
}

uint32_t stm_get_counter(unsigned int stm_id)
{
	return stm[stm_id]->CNT.R;
}

void stm_enable_channel(unsigned int stm_id, unsigned int chan)
{
	stm[stm_id]->CHANNEL[chan].CCR.B.CEN = 1;
}

void stm_disable_channel(unsigned int stm_id, unsigned int chan)
{
	stm[stm_id]->CHANNEL[chan].CCR.B.CEN = 0;
}

void stm_set_channel_compare(unsigned int stm_id, unsigned int chan, uint32_t cmp)
{
	stm[stm_id]->CHANNEL[chan].CMP.R = cmp;
}

uint32_t stm_get_channel_compare(unsigned int stm_id, unsigned int chan)
{
	return stm[stm_id]->CHANNEL[chan].CMP.R;
}

void stm_clear_interrupt_flag(unsigned int stm_id, unsigned int chan)
{
	stm[stm_id]->CHANNEL[chan].CIR.B.CIF = 1;
}

unsigned int stm_get_interrupt_flag(unsigned int stm_id, unsigned int chan)
{
	return stm[stm_id]->CHANNEL[chan].CIR.B.CIF;
}

stm_int_handler_t stm_set_interrupt_handler(unsigned int stm_id, unsigned int chan, stm_int_handler_t stm_int_handler)
{
	stm_int_handler_t old_stm_int_handler = stm_int_handlers[stm_id][chan];
	stm_int_handlers[stm_id][chan] = stm_int_handler;
	return old_stm_int_handler;
}

static void stm_0_cir0_cif()
{
	stm_int_handlers[0][0](0, 0);
}

static void stm_0_cir1_cif()
{
	stm_int_handlers[0][1](0, 1);
}

static void stm_0_cir2_cif()
{
	stm_int_handlers[0][2](0, 2);
}

static void stm_0_cir3_cif()
{
	stm_int_handlers[0][3](0, 3);
}

static void stm_1_cir0_cif()
{
	stm_int_handlers[1][0](1, 0);
}

static void stm_1_cir1_cif()
{
	stm_int_handlers[1][1](1, 1);
}

static void stm_1_cir2_cif()
{
	stm_int_handlers[1][2](1, 2);
}

static void stm_1_cir3_cif()
{
	stm_int_handlers[1][3](1, 3);
}

static void stm_2_cir0_cif()
{
	stm_int_handlers[2][0](2, 0);
}

static void stm_2_cir1_cif()
{
	stm_int_handlers[2][1](2, 1);
}

static void stm_2_cir2_cif()
{
	stm_int_handlers[2][2](2, 2);
}

static void stm_2_cir3_cif()
{
	stm_int_handlers[2][3](2, 3);
}
