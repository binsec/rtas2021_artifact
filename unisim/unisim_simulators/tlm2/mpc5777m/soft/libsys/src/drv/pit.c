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

#include "pit.h"
#include "intc.h"
#include "MPC5777M.h"

static void pit_0_tflg0_tif();
static void pit_0_tflg1_tif();
static void pit_0_tflg2_tif();
static void pit_0_tflg3_tif();
static void pit_0_tflg4_tif();
static void pit_0_tflg5_tif();
static void pit_0_tflg6_tif();
static void pit_0_tflg7_tif();

static void pit_0_rti_tflg_tif();

static void pit_1_tflg0_tif();
static void pit_1_tflg1_tif();

void pit_timer_int_handler_default(unsigned int pit_id, unsigned int chan)
{
	pit_clear_timer_interrupt_flag(pit_id, chan);
}

void pit_rti_timer_int_handler_default(unsigned int pit_id)
{
	pit_clear_rti_timer_interrupt_flag(pit_id);
}

static volatile struct PIT_tag *pit[2];
static pit_timer_int_handler_t pit_timer_int_handlers[2][8];
static pit_rti_timer_int_handler_t pit_rti_timer_int_handlers[2];
static unsigned int pit_rti_timer_irq_vector_table[2];
static unsigned int pit_timer_irq_vector_table[2][8];

static unsigned int pit_get_timer_irq_vector(unsigned int pit_id, unsigned int chan)
{
	return pit_timer_irq_vector_table[pit_id][chan];
}

static unsigned int pit_get_rti_timer_irq_vector(unsigned int pit_id)
{
	return pit_rti_timer_irq_vector_table[pit_id];
}

void pit_drv_init()
{
	pit[0] = &PIT_0;
	pit[1] = &PIT_1;
	unsigned pit_id;
	for(pit_id = 0; pit_id < 2; pit_id++)
	{
		pit_rti_timer_int_handlers[pit_id] = pit_rti_timer_int_handler_default;
		
		unsigned int chan;
		for(chan = 0; chan < 8; chan++)
		{
			pit_timer_int_handlers[pit_id][chan] = pit_timer_int_handler_default;
		}
	}
	pit_rti_timer_irq_vector_table[0] = 239;
	pit_rti_timer_irq_vector_table[1] = 0;
	pit_timer_irq_vector_table[0][0] = 226;
	pit_timer_irq_vector_table[0][1] = 227;
	pit_timer_irq_vector_table[0][2] = 228;
	pit_timer_irq_vector_table[0][3] = 229;
	pit_timer_irq_vector_table[0][4] = 230;
	pit_timer_irq_vector_table[0][5] = 231;
	pit_timer_irq_vector_table[0][6] = 132;
	pit_timer_irq_vector_table[0][7] = 233;
	pit_timer_irq_vector_table[1][0] = 240;
	pit_timer_irq_vector_table[1][1] = 241;
	pit_timer_irq_vector_table[1][2] = 0;
	pit_timer_irq_vector_table[1][3] = 0;
	pit_timer_irq_vector_table[1][4] = 0;
	pit_timer_irq_vector_table[1][5] = 0;
	pit_timer_irq_vector_table[1][6] = 0;
	pit_timer_irq_vector_table[1][7] = 0;
	
}

void pit_init(unsigned int pit_id)
{
	pit_disable_timers_clock(pit_id);
	if(pit_id == 0)
	{
		pit_disable_rti_timer_clock(pit_id);
		pit_disable_rti_timer(pit_id);
		pit_disable_rti_timer_interrupt(pit_id);
		pit_set_rti_timer_load_value(pit_id, 0);
	}

	unsigned int num_channels = 0;
	
	switch(pit_id)
	{
		case 0:
			num_channels = 8;
			break;
		case 1:
			num_channels = 2;
			break;
	}
	
	unsigned int chan;
	for(chan = 0; chan < num_channels; chan++)
	{
		pit_disable_timer(pit_id, chan);
		pit_disable_timer_interrupt(pit_id, chan);
		pit_set_timer_load_value(pit_id, chan, 0);
		pit_timer_int_handlers[pit_id][chan] = pit_timer_int_handler_default;
	}
	
	switch(pit_id)
	{
		case 0:
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 0), &pit_0_tflg0_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 1), &pit_0_tflg1_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 2), &pit_0_tflg2_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 3), &pit_0_tflg3_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 4), &pit_0_tflg4_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 5), &pit_0_tflg5_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 6), &pit_0_tflg6_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(0, 7), &pit_0_tflg7_tif);
			intc_set_interrupt_handler(pit_get_rti_timer_irq_vector(0), &pit_0_rti_tflg_tif);
			break;
		case 1:
			intc_set_interrupt_handler(pit_get_timer_irq_vector(1, 0), &pit_1_tflg0_tif);
			intc_set_interrupt_handler(pit_get_timer_irq_vector(1, 1), &pit_1_tflg1_tif);
			break;
	}
}

void pit_enable_timers_clock(unsigned int pit_id)
{
	pit[pit_id]->MCR.B.MDIS = 0;
}

void pit_disable_timers_clock(unsigned int pit_id)
{
	pit[pit_id]->MCR.B.MDIS = 1;
}

void pit_enable_rti_timer_clock(unsigned int pit_id)
{
	pit[pit_id]->MCR.B.MDIS_RTI = 0;
}

void pit_disable_rti_timer_clock(unsigned int pit_id)
{
	pit[pit_id]->MCR.B.MDIS_RTI = 1;
}

uint64_t pit_get_lifetime(unsigned int pit_id)
{
	uint32_t h = pit[pit_id]->LTMR64H.R;
	uint32_t l = pit[pit_id]->LTMR64L.R;
	return ((uint64_t) h << 32) | l;
}

void pit_enable_rti_timer(unsigned int pit_id)
{
	pit[pit_id]->RTI_TCTRL.B.TEN = 1;
}

void pit_disable_rti_timer(unsigned int pit_id)
{
	pit[pit_id]->RTI_TCTRL.B.TEN = 0;
}

void pit_enable_rti_timer_interrupt(unsigned int pit_id)
{
	pit[pit_id]->RTI_TCTRL.B.TIE = 1;
}

void pit_disable_rti_timer_interrupt(unsigned int pit_id)
{
	pit[pit_id]->RTI_TCTRL.B.TEN = 0;
}


void pit_set_rti_timer_load_value(unsigned int pit_id, uint32_t value)
{
	pit[pit_id]->RTI_LDVAL.R = value;
}

uint32_t pit_get_rti_timer_load_value(unsigned int pit_id)
{
	return pit[pit_id]->RTI_LDVAL.R;
}

uint32_t pit_get_rti_timer_value(unsigned int pit_id)
{
	return pit[pit_id]->RTI_CVAL.R;
}

int pit_get_rti_timer_interrupt_flag(unsigned int pit_id)
{
	return pit[pit_id]->RTI_TFLG.B.TIF;
}

void pit_clear_rti_timer_interrupt_flag(unsigned int pit_id)
{
	pit[pit_id]->RTI_TFLG.B.TIF = 1;
}

pit_rti_timer_int_handler_t pit_set_rti_timer_interrupt_handler(unsigned int pit_id, pit_rti_timer_int_handler_t pit_rti_timer_int_handler)
{
	pit_rti_timer_int_handler_t old_rti_timer_int_handler = pit_rti_timer_int_handlers[pit_id];
	pit_rti_timer_int_handlers[pit_id] = pit_rti_timer_int_handler;
	return old_rti_timer_int_handler;
}

void pit_set_rti_timer_irq_priority(unsigned int pit_id, unsigned int priority)
{
	intc_set_irq_priority(pit_get_rti_timer_irq_vector(pit_id), priority);
}

void pit_select_rti_timer_irq_for_processor(unsigned int pit_id, unsigned int prc_num)
{
	intc_select_irq_for_processor(pit_get_rti_timer_irq_vector(pit_id), prc_num);
}

void pit_deselect_rti_timer_irq_for_processor(unsigned int pit_id, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(pit_get_rti_timer_irq_vector(pit_id), prc_num);
}

void pit_enable_timer(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TCTRL.B.TEN = 1;
}

void pit_disable_timer(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TCTRL.B.TEN = 0;
}

void pit_chain_timer(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TCTRL.B.CHN = 1;
}

void pit_unchain_timer(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TCTRL.B.CHN = 0;
}

void pit_enable_timer_interrupt(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TCTRL.B.TIE = 1;
}

void pit_disable_timer_interrupt(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TCTRL.B.TIE = 0;
}

void pit_set_timer_load_value(unsigned int pit_id, unsigned int chan, uint32_t value)
{
	pit[pit_id]->TIMER[chan].LDVAL.R = value;
}

uint32_t pit_get_timer_load_value(unsigned int pit_id, unsigned int chan)
{
	return pit[pit_id]->TIMER[chan].LDVAL.R;
}

uint32_t pit_get_timer_value(unsigned int pit_id, unsigned int chan)
{
	return pit[pit_id]->TIMER[chan].CVAL.R;
}

int pit_get_timer_interrupt_flag(unsigned int pit_id, unsigned int chan)
{
	return pit[pit_id]->TIMER[chan].TFLG.B.TIF;
}

void pit_clear_timer_interrupt_flag(unsigned int pit_id, unsigned int chan)
{
	pit[pit_id]->TIMER[chan].TFLG.B.TIF = 1;
}

pit_timer_int_handler_t pit_set_timer_interrupt_handler(unsigned int pit_id, unsigned int chan, pit_timer_int_handler_t pit_timer_int_handler)
{
	pit_timer_int_handler_t old_timer_int_handler = pit_timer_int_handlers[pit_id][chan];
	pit_timer_int_handlers[pit_id][chan] = pit_timer_int_handler;
	return old_timer_int_handler;
}

void pit_set_timer_irq_priority(unsigned int pit_id, unsigned int chan, unsigned int priority)
{
	intc_set_irq_priority(pit_get_timer_irq_vector(pit_id, chan), priority);
}

void pit_select_timer_irq_for_processor(unsigned int pit_id, unsigned int chan, unsigned int prc_num)
{
	intc_select_irq_for_processor(pit_get_timer_irq_vector(pit_id, chan), prc_num);
}

void pit_deselect_timer_irq_for_processor(unsigned int pit_id, unsigned int chan, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(pit_get_timer_irq_vector(pit_id, chan), prc_num);
}

static void pit_0_tflg0_tif()
{
	pit_timer_int_handlers[0][0](0, 0);
}

static void pit_0_tflg1_tif()
{
	pit_timer_int_handlers[0][1](0, 1);
}

static void pit_0_tflg2_tif()
{
	pit_timer_int_handlers[0][2](0, 2);
}

static void pit_0_tflg3_tif()
{
	pit_timer_int_handlers[0][3](0, 3);
}

static void pit_0_tflg4_tif()
{
	pit_timer_int_handlers[0][4](0, 4);
}

static void pit_0_tflg5_tif()
{
	pit_timer_int_handlers[0][5](0, 5);
}

static void pit_0_tflg6_tif()
{
	pit_timer_int_handlers[0][6](0, 6);
}

static void pit_0_tflg7_tif()
{
	pit_timer_int_handlers[0][7](0, 7);
}


static void pit_0_rti_tflg_tif()
{
	pit_rti_timer_int_handlers[0](0);
}


static void pit_1_tflg0_tif()
{
	pit_timer_int_handlers[1][0](1, 0);
}

static void pit_1_tflg1_tif()
{
	pit_timer_int_handlers[1][1](1, 1);
}

