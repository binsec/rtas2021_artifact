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

#include "swt.h"
#include "intc.h"
#include "MPC5777M.h"

static void swt_0_ir_tif();
static void swt_1_ir_tif();
static void swt_2_ir_tif();
static void swt_3_ir_tif();

void swt_int_handler_default(unsigned int swt_id)
{
	//swt_clear_interrupt_flag(swt_id);
}

static volatile struct SWT_tag *swt[4];
static swt_int_handler_t swt_int_handlers[4];

static unsigned int swt_get_irq_vector(unsigned int swt_id)
{
	return 32 + swt_id;
}

void swt_drv_init()
{
	swt[0] = &SWT_0;
	swt[1] = &SWT_1;
	swt[2] = &SWT_2;
	swt[3] = &SWT_3;
	swt_int_handlers[0] = swt_int_handler_default;
	swt_int_handlers[1] = swt_int_handler_default;
	swt_int_handlers[2] = swt_int_handler_default;
	swt_int_handlers[3] = swt_int_handler_default;
}

void swt_init(unsigned int swt_id)
{
	swt_int_handlers[swt_id] = swt_int_handler_default;
	
	if(swt_is_soft_locked(swt_id))
	{
		swt_soft_unlock(swt_id);
	}
	swt_disable(swt_id);
	swt_set_service_mode(swt_id, SMD_FIXED_SERVICE_SEQUENCE);
	swt_disable_window_mode(swt_id);
	swt_disable_reset_on_invalid_access(swt_id);
	swt_disable_interrupt_then_reset(swt_id);
	swt_set_timeout(swt_id, 0);
	swt_set_window(swt_id, 0);
	swt_clear_interrupt_flag(swt_id);
	intc_set_interrupt_handler(swt_get_irq_vector(0), &swt_0_ir_tif);
	intc_set_interrupt_handler(swt_get_irq_vector(1), &swt_1_ir_tif);
	intc_set_interrupt_handler(swt_get_irq_vector(2), &swt_2_ir_tif);
	intc_set_interrupt_handler(swt_get_irq_vector(3), &swt_3_ir_tif);
}

void swt_enable(unsigned int swt_id)
{
	swt[swt_id]->CR.B.WEN = 1;
}

void swt_disable(unsigned int swt_id)
{
	swt[swt_id]->CR.B.WEN = 0;
}

void swt_set_service_mode(unsigned int swt_id, SWT_SMD smd)
{
	swt[swt_id]->CR.B.SMD = smd;
}

SWT_SMD swt_get_service_mode(unsigned int swt_id)
{
	return (SWT_SMD) swt[swt_id]->CR.B.SMD;
}

void swt_enable_window_mode(unsigned int swt_id)
{
	swt[swt_id]->CR.B.WND = 1;
}

void swt_disable_window_mode(unsigned int swt_id)
{
	swt[swt_id]->CR.B.WND = 0;
}

void swt_enable_reset_on_invalid_access(unsigned int swt_id)
{
	swt[swt_id]->CR.B.RIA = 1;
}

void swt_disable_reset_on_invalid_access(unsigned int swt_id)
{
	swt[swt_id]->CR.B.RIA = 0;
}

void swt_enable_interrupt_then_reset(unsigned int swt_id)
{
	swt[swt_id]->CR.B.ITR = 1;
}

void swt_disable_interrupt_then_reset(unsigned int swt_id)
{
	swt[swt_id]->CR.B.ITR = 0;
}

int swt_is_soft_locked(unsigned int swt_id)
{
	return swt[swt_id]->CR.B.SLK;
}

int swt_is_hard_locked(unsigned int swt_id)
{
	return swt[swt_id]->CR.B.HLK;
}

int swt_is_locked(unsigned int swt_id)
{
	return swt_is_soft_locked(swt_id) || swt_is_hard_locked(swt_id);
}

void swt_hard_lock(unsigned int swt_id)
{
	swt[swt_id]->CR.B.HLK = 1;
}

void swt_soft_lock(unsigned int swt_id)
{
	swt[swt_id]->CR.B.SLK = 1;
}

void swt_soft_unlock(unsigned int swt_id)
{
	swt[swt_id]->SR.R = 0xc520;
	swt[swt_id]->SR.R = 0xd928;
}

void swt_set_timeout(unsigned int swt_id, uint32_t value)
{
	swt[swt_id]->TO.B.WTO = value;
}

void swt_set_window(unsigned int swt_id, uint32_t value)
{
	swt[swt_id]->WN.B.WST = value;
}

uint32_t swt_get_counter(unsigned int swt_id)
{
	return swt[swt_id]->CO.B.CNT;
}

void swt_set_service_key(unsigned int swt_id, uint16_t value)
{
	swt[swt_id]->SK.B.SK = value;
}

uint16_t swt_get_service_key(unsigned int swt_id)
{
	return swt[swt_id]->SK.B.SK;
}

void swt_service_sequence(unsigned int swt_id)
{
	switch(swt_get_service_mode(swt_id))
	{
		case SMD_FIXED_SERVICE_SEQUENCE:
			swt[swt_id]->SR.R = 0xa602;
			swt[swt_id]->SR.R = 0xb480;
			break;
		case SMD_KEYED_SERVICE_SEQUENCE:
		{
			uint16_t service_key = swt_get_service_key(swt_id);
			swt[swt_id]->SR.R = service_key = (17 * service_key) + 3;
			swt[swt_id]->SR.R = service_key = (17 * service_key) + 3;
			break;
		}
		
		default:
			break;
	}
}

void swt_clear_interrupt_flag(unsigned int swt_id)
{
	swt[swt_id]->IR.B.TIF = 1;
}

unsigned int swt_get_interrupt_flag(unsigned int swt_id)
{
	return swt[swt_id]->IR.B.TIF;
}

swt_int_handler_t swt_set_interrupt_handler(unsigned int swt_id, swt_int_handler_t swt_int_handler)
{
	swt_int_handler_t old_swt_int_handler = swt_int_handlers[swt_id];
	swt_int_handlers[swt_id] = swt_int_handler;
	return old_swt_int_handler;
}

static void swt_0_ir_tif()
{
	swt_int_handlers[0](0);
}

static void swt_1_ir_tif()
{
	swt_int_handlers[1](1);
}

static void swt_2_ir_tif()
{
	swt_int_handlers[2](2);
}

static void swt_3_ir_tif()
{
	swt_int_handlers[3](3);
}

