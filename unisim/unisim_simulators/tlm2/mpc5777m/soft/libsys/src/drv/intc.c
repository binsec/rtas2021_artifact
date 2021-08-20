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

#include "intc.h"
#include "MPC5777M.h"
#include "compiler_api.h"

#define INTC_NUM_IRQS 1024

extern void VTABLE (void);

uint32_t __attribute__ ((section (".intc_vector_table"))) intc_isr_vector_table[INTC_NUM_IRQS];

static unsigned int intc_get_core_id()
{
	return MFSPR(286);
}

void intc_set_ivpr(uint32_t value)
{
    MTSPR(63, value);
}

static void intc_set_interrupt_vector_table_address(unsigned int prc_num, uint32_t intc_isr_vector_table_addr);

static void intc_dummy_int_handler()
{
	while(1);
}

void intc_drv_init()
{
	unsigned int irq;
	for(irq = 0; irq < INTC_NUM_IRQS; irq++)
	{
		intc_isr_vector_table[irq] = (uint32_t) intc_dummy_int_handler;
	}
}

void intc_init()
{
	intc_set_ivpr((uint32_t) &VTABLE);
	
    unsigned int core_id = intc_get_core_id();

	intc_set_processor_interrupt_vector_mode(core_id, INTC_SW_VEC_MODE);
	intc_acknowledge_interrupt(core_id);
	intc_set_interrupt_vector_table_address(core_id, (uint32_t) &intc_isr_vector_table[0]);
	intc_set_current_irq_priority(core_id, 0);
}

void intc_set_processor_interrupt_vector_mode(unsigned int prc_num, enum INTC_VEC_MODE intc_vec_mode)
{
	switch(prc_num)
	{
		case 0: INTC_0.BCR.B.HVEN0 = intc_vec_mode; break;
		case 1: INTC_0.BCR.B.HVEN1 = intc_vec_mode; break;
		case 2: INTC_0.BCR.B.HVEN2 = intc_vec_mode; break;
		case 3: INTC_0.BCR.B.HVEN3 = intc_vec_mode; break;
	}
}

void intc_acknowledge_interrupt(unsigned int prc_num)
{
	uint32_t __attribute__((unused)) temp = INTC_0.IACKR[prc_num].R;
}

static void intc_set_interrupt_vector_table_address(unsigned int prc_num, uint32_t intc_isr_vector_table_addr)
{
	INTC_0.IACKR[prc_num].R = intc_isr_vector_table_addr;
}

void intc_enable_external_interrupt()
{
    PPCASM (" wrteei 1 ");
}


irq_handler_t intc_set_interrupt_handler(unsigned int irq, irq_handler_t irq_handler)
{
	irq_handler_t old_irq_handler = (irq_handler_t) intc_isr_vector_table[irq];
	intc_isr_vector_table[irq] = (uint32_t) irq_handler;
	return old_irq_handler;
}

void intc_set_current_irq_priority(unsigned int prc_num, unsigned int priority)
{
	INTC_0.CPR[prc_num].R = priority;
}

void intc_select_irq_for_processor(unsigned int irq, unsigned int prc_num)
{
	switch(prc_num)
	{
		case 0: INTC_0.PSR[irq].B.PRC_SELN0=1; break;
		case 1: INTC_0.PSR[irq].B.PRC_SELN1=1; break;
		case 2: INTC_0.PSR[irq].B.PRC_SELN2=1; break;
		case 3: INTC_0.PSR[irq].B.PRC_SELN3=1; break;
	}
}

void intc_deselect_irq_for_processor(unsigned int irq, unsigned int prc_num)
{
	switch(prc_num)
	{
		case 0: INTC_0.PSR[irq].B.PRC_SELN0=0; break;
		case 1: INTC_0.PSR[irq].B.PRC_SELN1=0; break;
		case 2: INTC_0.PSR[irq].B.PRC_SELN2=0; break;
		case 3: INTC_0.PSR[irq].B.PRC_SELN3=0; break;
	}
}

void intc_set_irq_priority(unsigned int irq, unsigned int priority)
{
	INTC_0.PSR[irq].B.PRIN=priority;
}

