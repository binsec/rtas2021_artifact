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

#include "edma.h"
#include "intc.h"
#include "MPC5777M.h"
#include "compiler_api.h"

static void edma_0_int0();
static void edma_0_int1();
static void edma_0_int2();
static void edma_0_int3();
static void edma_0_int4();
static void edma_0_int5();
static void edma_0_int6();
static void edma_0_int7();
static void edma_0_int8();
static void edma_0_int9();
static void edma_0_int10();
static void edma_0_int11();
static void edma_0_int12();
static void edma_0_int13();
static void edma_0_int14();
static void edma_0_int15();
static void edma_0_int16();
static void edma_0_int17();
static void edma_0_int18();
static void edma_0_int19();
static void edma_0_int20();
static void edma_0_int21();
static void edma_0_int22();
static void edma_0_int23();
static void edma_0_int24();
static void edma_0_int25();
static void edma_0_int26();
static void edma_0_int27();
static void edma_0_int28();
static void edma_0_int29();
static void edma_0_int30();
static void edma_0_int31();
static void edma_0_int32();
static void edma_0_int33();
static void edma_0_int34();
static void edma_0_int35();
static void edma_0_int36();
static void edma_0_int37();
static void edma_0_int38();
static void edma_0_int39();
static void edma_0_int40();
static void edma_0_int41();
static void edma_0_int42();
static void edma_0_int43();
static void edma_0_int44();
static void edma_0_int45();
static void edma_0_int46();
static void edma_0_int47();
static void edma_0_int48();
static void edma_0_int49();
static void edma_0_int50();
static void edma_0_int51();
static void edma_0_int52();
static void edma_0_int53();
static void edma_0_int54();
static void edma_0_int55();
static void edma_0_int56();
static void edma_0_int57();
static void edma_0_int58();
static void edma_0_int59();
static void edma_0_int60();
static void edma_0_int61();
static void edma_0_int62();
static void edma_0_int63();
static void edma_1_int0();
static void edma_1_int1();
static void edma_1_int2();
static void edma_1_int3();
static void edma_1_int4();
static void edma_1_int5();
static void edma_1_int6();
static void edma_1_int7();
static void edma_1_int8();
static void edma_1_int9();
static void edma_1_int10();
static void edma_1_int11();
static void edma_1_int12();
static void edma_1_int13();
static void edma_1_int14();
static void edma_1_int15();
static void edma_1_int16();
static void edma_1_int17();
static void edma_1_int18();
static void edma_1_int19();
static void edma_1_int20();
static void edma_1_int21();
static void edma_1_int22();
static void edma_1_int23();
static void edma_1_int24();
static void edma_1_int25();
static void edma_1_int26();
static void edma_1_int27();
static void edma_1_int28();
static void edma_1_int29();
static void edma_1_int30();
static void edma_1_int31();
static void edma_1_int32();
static void edma_1_int33();
static void edma_1_int34();
static void edma_1_int35();
static void edma_1_int36();
static void edma_1_int37();
static void edma_1_int38();
static void edma_1_int39();
static void edma_1_int40();
static void edma_1_int41();
static void edma_1_int42();
static void edma_1_int43();
static void edma_1_int44();
static void edma_1_int45();
static void edma_1_int46();
static void edma_1_int47();
static void edma_1_int48();
static void edma_1_int49();
static void edma_1_int50();
static void edma_1_int51();
static void edma_1_int52();
static void edma_1_int53();
static void edma_1_int54();
static void edma_1_int55();
static void edma_1_int56();
static void edma_1_int57();
static void edma_1_int58();
static void edma_1_int59();
static void edma_1_int60();
static void edma_1_int61();
static void edma_1_int62();
static void edma_1_int63();

void edma_int_handler_default(unsigned int edma_id, unsigned int chan)
{
	edma_clear_interrupt_request(edma_id, chan);
}

static volatile struct eDMA_tag *edma[2];
static edma_int_handler_t edma_int_handlers[2][64];

static unsigned int edma_get_channel_irq_vector(unsigned int edma_id, unsigned int chan)
{
	return 53 + (edma_id * 64) + chan;
}

void edma_drv_init()
{
	edma[0] = &eDMA_0;
	edma[1] = &eDMA_1;
	unsigned int edma_id;
	for(edma_id = 0; edma_id < 2; edma_id++)
	{
		unsigned int chan;
		for(chan = 0; chan < 64; chan++)
		{
			edma_int_handlers[edma_id][chan] = edma_int_handler_default;
		}
	}
}

void edma_init(unsigned int edma_id)
{
	switch(edma_id)
	{
		case 0:
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 0), &edma_0_int0);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 1), &edma_0_int1);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 2), &edma_0_int2);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 3), &edma_0_int3);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 4), &edma_0_int4);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 5), &edma_0_int5);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 6), &edma_0_int6);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 7), &edma_0_int7);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 8), &edma_0_int8);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 9), &edma_0_int9);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 10), &edma_0_int10);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 11), &edma_0_int11);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 12), &edma_0_int12);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 13), &edma_0_int13);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 14), &edma_0_int14);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 15), &edma_0_int15);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 16), &edma_0_int16);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 17), &edma_0_int17);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 18), &edma_0_int18);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 19), &edma_0_int19);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 20), &edma_0_int20);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 21), &edma_0_int21);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 22), &edma_0_int22);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 23), &edma_0_int23);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 24), &edma_0_int24);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 25), &edma_0_int25);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 26), &edma_0_int26);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 27), &edma_0_int27);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 28), &edma_0_int28);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 29), &edma_0_int29);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 30), &edma_0_int30);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 31), &edma_0_int31);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 32), &edma_0_int32);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 33), &edma_0_int33);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 34), &edma_0_int34);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 35), &edma_0_int35);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 36), &edma_0_int36);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 37), &edma_0_int37);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 38), &edma_0_int38);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 39), &edma_0_int39);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 40), &edma_0_int40);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 41), &edma_0_int41);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 42), &edma_0_int42);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 43), &edma_0_int43);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 44), &edma_0_int44);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 45), &edma_0_int45);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 46), &edma_0_int46);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 47), &edma_0_int47);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 48), &edma_0_int48);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 49), &edma_0_int49);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 50), &edma_0_int50);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 51), &edma_0_int51);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 52), &edma_0_int52);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 53), &edma_0_int53);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 54), &edma_0_int54);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 55), &edma_0_int55);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 56), &edma_0_int56);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 57), &edma_0_int57);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 58), &edma_0_int58);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 59), &edma_0_int59);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 60), &edma_0_int60);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 61), &edma_0_int61);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 62), &edma_0_int62);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(0, 63), &edma_0_int63);
			break;

		case 1:
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 0), &edma_1_int0);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 1), &edma_1_int1);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 2), &edma_1_int2);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 3), &edma_1_int3);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 4), &edma_1_int4);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 5), &edma_1_int5);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 6), &edma_1_int6);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 7), &edma_1_int7);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 8), &edma_1_int8);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 9), &edma_1_int9);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 10), &edma_1_int10);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 11), &edma_1_int11);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 12), &edma_1_int12);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 13), &edma_1_int13);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 14), &edma_1_int14);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 15), &edma_1_int15);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 16), &edma_1_int16);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 17), &edma_1_int17);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 18), &edma_1_int18);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 19), &edma_1_int19);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 20), &edma_1_int20);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 21), &edma_1_int21);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 22), &edma_1_int22);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 23), &edma_1_int23);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 24), &edma_1_int24);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 25), &edma_1_int25);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 26), &edma_1_int26);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 27), &edma_1_int27);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 28), &edma_1_int28);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 29), &edma_1_int29);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 30), &edma_1_int30);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 31), &edma_1_int31);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 32), &edma_1_int32);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 33), &edma_1_int33);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 34), &edma_1_int34);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 35), &edma_1_int35);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 36), &edma_1_int36);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 37), &edma_1_int37);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 38), &edma_1_int38);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 39), &edma_1_int39);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 40), &edma_1_int40);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 41), &edma_1_int41);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 42), &edma_1_int42);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 43), &edma_1_int43);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 44), &edma_1_int44);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 45), &edma_1_int45);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 46), &edma_1_int46);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 47), &edma_1_int47);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 48), &edma_1_int48);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 49), &edma_1_int49);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 50), &edma_1_int50);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 51), &edma_1_int51);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 52), &edma_1_int52);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 53), &edma_1_int53);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 54), &edma_1_int54);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 55), &edma_1_int55);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 56), &edma_1_int56);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 57), &edma_1_int57);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 58), &edma_1_int58);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 59), &edma_1_int59);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 60), &edma_1_int60);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 61), &edma_1_int61);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 62), &edma_1_int62);
			intc_set_interrupt_handler(edma_get_channel_irq_vector(1, 63), &edma_1_int63);
			break;
	}

#if 0
	unsigned int grp;
	for(grp = 0; grp < 4; grp++)
	{
		edma_set_channel_group_priority(edma_id, grp, grp);            // eDMA0                   : set channel group priority (lower to higher)
	}
	
	unsigned int chan;
	for(chan = 0; chan < 64; chan++)
	{
		edma_set_channel_arbitration_priority(edma_id, chan, chan & 15); // eDMA0          : set channel priority in group (lower to higher)
	}
#endif
}

unsigned int edma_channel_group_priority(unsigned int edma_id, unsigned int grp)
{
	switch(grp)
	{
		case 0: return edma[edma_id]->CR.B.GRP0PRI;
		case 1: return edma[edma_id]->CR.B.GRP1PRI;
		case 2: return edma[edma_id]->CR.B.GRP2PRI;
		case 3: return edma[edma_id]->CR.B.GRP3PRI;
	}
	return 0;
}

unsigned int edma_minor_loop_mapping(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.EMLM;
}

unsigned int edma_continuous_link_mode(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.CLM;
}

unsigned int edma_dma_operations_halted(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.HALT;
}

unsigned int edma_halt_on_error(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.HOE;
}

enum eDMA_Arbitration edma_group_arbitration(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.ERGA;
}
 
enum eDMA_Arbitration edma_channel_arbitration(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.ERCA;
}
 
unsigned int edma_debug_mode(unsigned int edma_id)
{
	return edma[edma_id]->CR.B.EDBG;
}

void edma_cancel_transfer(unsigned int edma_id)
{
	edma[edma_id]->CR.B.CX = 1;
}

void edma_error_cancel_transfer(unsigned int edma_id)
{
	edma[edma_id]->CR.B.ECX = 1;
}

void edma_set_channel_group_priority(unsigned int edma_id, unsigned int grp, unsigned int prio)
{
	switch(grp)
	{
		case 0: edma[edma_id]->CR.B.GRP0PRI = prio; break;
		case 1: edma[edma_id]->CR.B.GRP1PRI = prio; break;
		case 2: edma[edma_id]->CR.B.GRP2PRI = prio; break;
		case 3: edma[edma_id]->CR.B.GRP3PRI = prio; break;
	}
}

void edma_enable_minor_loop_mapping(unsigned int edma_id)
{
	edma[edma_id]->CR.B.EMLM = 1;
}

void edma_disable_minor_loop_mapping(unsigned int edma_id)
{
	edma[edma_id]->CR.B.EMLM = 0;
}

void edma_enable_continuous_link_mode(unsigned int edma_id)
{
	edma[edma_id]->CR.B.CLM = 1;
}

void edma_disable_continuous_link_mode(unsigned int edma_id)
{
	edma[edma_id]->CR.B.CLM = 0;
}

void edma_halt_dma_operations(unsigned int edma_id)
{
	edma[edma_id]->CR.B.HALT = 1;
}

void edma_resume_dma_operations(unsigned int edma_id)
{
	edma[edma_id]->CR.B.HALT = 0;
}

void edma_enable_halt_on_error(unsigned int edma_id)
{
	edma[edma_id]->CR.B.HOE = 1;
}

void edma_disable_halt_on_error(unsigned int edma_id)
{
	edma[edma_id]->CR.B.HOE = 0;
}

void edma_select_group_arbitration(unsigned int edma_id, enum eDMA_Arbitration arb)
{
	edma[edma_id]->CR.B.ERGA = arb;
}

void edma_select_channel_arbitration(unsigned int edma_id, enum eDMA_Arbitration arb)
{
	edma[edma_id]->CR.B.ERCA = arb;
}

void edma_enable_debug_mode(unsigned int edma_id)
{
	edma[edma_id]->CR.B.EDBG = 1;
}

void edma_disable_debug_mode(unsigned int edma_id)
{
	edma[edma_id]->CR.B.EDBG = 0;
}

unsigned int edma_valid(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.VLD;
}

unsigned int edma_uncorrectable_ecc_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.UCE;
}

unsigned int edma_transfer_cancelled(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.ECX;
}

unsigned int edma_group_priority_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.GPE;
}

unsigned int edma_channel_priority_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.CPE;
}

unsigned int edma_get_error_cancelled_channel(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.ERRCHN;
}

unsigned int edma_source_address_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.SAE;
}

unsigned int edma_source_offset_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.SOE;
}

unsigned int edma_destination_address_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.DAE;
}

unsigned int edma_destination_offset_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.DOE;
}

unsigned int edma_nbytes_citer_config_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.NCE;
}

unsigned int edma_scatter_gather_config_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.SGE;
}

unsigned int edma_source_bus_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.SBE;
}

unsigned int edma_destination_bus_error(unsigned int edma_id)
{
	return edma[edma_id]->ES.B.DBE;
}

unsigned int edma_request_enabled(unsigned int edma_id, unsigned int chan)
{
	return (chan < 64) ? ((chan < 32) ? ((edma[edma_id]->ERQL.R >> chan) & 1) : ((edma[edma_id]->ERQH.R >> (chan - 32)) & 1)) : 0;
}

unsigned int edma_error_interrupt_enabled(unsigned int edma_id, unsigned int chan)
{
	return (chan < 64) ? ((chan < 32) ? ((edma[edma_id]->EEIL.R >> chan) & 1) : ((edma[edma_id]->EEIH.R >> (chan - 32)) & 1)) : 0;
}

void edma_enable_all_requests(unsigned int edma_id)
{
	union eDMA_SERQ_union_tag serq;
	serq.R = 0;
	serq.B.SAER = 1;
	edma[edma_id]->SERQ = serq;
}

void edma_disable_all_requests(unsigned int edma_id)
{
	union eDMA_CERQ_union_tag cerq;
	cerq.R = 0;
	cerq.B.CAER = 1;
	edma[edma_id]->CERQ = cerq;
}

void edma_enable_request(unsigned int edma_id, unsigned int chan)
{
	union eDMA_SERQ_union_tag serq;
	serq.R = 0;
	serq.B.SERQ = chan;
	edma[edma_id]->SERQ = serq;
}

void edma_disable_request(unsigned int edma_id, unsigned int chan)
{
	union eDMA_CERQ_union_tag cerq;
	cerq.R = 0;
	cerq.B.CERQ = chan;
	edma[edma_id]->CERQ = cerq;
}

void edma_enable_all_error_interrupts(unsigned int edma_id)
{
	union eDMA_SEEI_union_tag seei;
	seei.R = 0;
	seei.B.SAEE = 1;
	edma[edma_id]->SEEI = seei;
}

void edma_disable_all_error_interrupts(unsigned int edma_id)
{
	union eDMA_CEEI_union_tag ceei;
	ceei.R = 0;
	ceei.B.CAEE = 1;
	edma[edma_id]->CEEI = ceei;
}

void edma_enable_error_interrupt(unsigned int edma_id, unsigned int chan)
{
	union eDMA_SEEI_union_tag seei;
	seei.R = 0;
	seei.B.SEEI = chan;
	edma[edma_id]->SEEI = seei;
}

void edma_disable_error_interrupt(unsigned int edma_id, unsigned int chan)
{
	union eDMA_CEEI_union_tag ceei;
	ceei.R = 0;
	ceei.B.CEEI = chan;
	edma[edma_id]->CEEI = ceei;
}

void edma_clear_all_interrupt_requests(unsigned int edma_id)
{
	union eDMA_CINT_union_tag cint;
	cint.R = 0;
	cint.B.CAIR = 1;
	edma[edma_id]->CINT = cint;
}

void edma_clear_interrupt_request(unsigned int edma_id, unsigned int chan)
{
	union eDMA_CINT_union_tag cint;
	cint.R = 0;
	cint.B.CINT = chan;
	edma[edma_id]->CINT = cint;
}

void edma_clear_all_errors(unsigned int edma_id)
{
	union eDMA_CERR_union_tag cerr;
	cerr.R = 0;
	cerr.B.CAEI = 1;
	edma[edma_id]->CERR = cerr;
}

void edma_clear_error(unsigned int edma_id, unsigned int chan)
{
	union eDMA_CERR_union_tag cerr;
	cerr.R = 0;
	cerr.B.CERR = chan;
	edma[edma_id]->CERR = cerr;
}

void edma_set_all_start_bits(unsigned int edma_id)
{
	union eDMA_SSRT_union_tag ssrt;
	ssrt.R = 0;
	ssrt.B.SAST = 1;
	edma[edma_id]->SSRT = ssrt;
}

void edma_set_start_bit(unsigned int edma_id, unsigned int chan)
{
	union eDMA_SSRT_union_tag ssrt;
	ssrt.R = 0;
	ssrt.B.SSRT = chan;
	edma[edma_id]->SSRT = ssrt;
}

void edma_clear_all_done_bits(unsigned int edma_id)
{
	union eDMA_CDNE_union_tag cdne;
	cdne.R = 0;
	cdne.B.CADN = 1;
	edma[edma_id]->CDNE = cdne;
}

void edma_clear_done_bit(unsigned int edma_id, unsigned int chan)
{
	union eDMA_CDNE_union_tag cdne;
	cdne.R = 0;
	cdne.B.CDNE = chan;
	edma[edma_id]->CDNE = cdne;
}

unsigned int edma_interrupt_request(unsigned int edma_id, unsigned int chan)
{
	return (chan < 64) ? ((chan < 32) ? ((edma[edma_id]->INTL.R >> chan) & 1) : ((edma[edma_id]->INTH.R >> (chan - 32)) & 1)) : 0;
}

unsigned int edma_error(unsigned int edma_id, unsigned int chan)
{
	return (chan < 64) ? ((chan < 32) ? ((edma[edma_id]->ERRL.R >> chan) & 1) : ((edma[edma_id]->ERRH.R >> (chan - 32)) & 1)) : 0;
}

unsigned int edma_hardware_request_status(unsigned int edma_id, unsigned int chan)
{
	return (chan < 64) ? ((chan < 32) ? ((edma[edma_id]->HRSL.R >> chan) & 1) : ((edma[edma_id]->HRSH.R >> (chan - 32)) & 1)) : 0;
}

void edma_enable_channel_preemption(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->DCHPRI[chan].B.ECP = 1;
}

void edma_disable_channel_preemption(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->DCHPRI[chan].B.ECP = 0;
}

void edma_enable_preempt_ability(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->DCHPRI[chan].B.DPA = 0;
}

void edma_disable_preempt_ability(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->DCHPRI[chan].B.DPA = 1;
}

unsigned int edma_get_channel_current_group_priority(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->DCHPRI[chan].B.GRPPRI;
}

unsigned int edma_get_channel_arbitration_priority(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->DCHPRI[chan].B.CHPRI;
}

void edma_set_channel_arbitration_priority(unsigned int edma_id, unsigned int chan, unsigned int prio)
{
	edma[edma_id]->DCHPRI[chan].B.CHPRI = prio;
}

void edma_enable_master_id_replication(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->DCHMID[chan].B.EMI = 1;
}

void edma_disable_master_id_replication(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->DCHMID[chan].B.EMI = 0;
}

int edma_is_master_id_replication_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->DCHMID[chan].B.EMI != 0;
}

enum eDMA_PrivilegeAccessLevel edma_get_privilege_access_level(unsigned int edma_id, unsigned int chan)
{
	return (enum eDMA_PrivilegeAccessLevel) edma[edma_id]->DCHMID[chan].B.PAL;
}

unsigned int edma_get_master_id(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->DCHMID[chan].B.MID;
}

uint32_t edma_get_tcd_source_address(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].SADDR.B.SADDR;
}

void edma_set_tcd_source_address(unsigned int edma_id, unsigned int chan, uint32_t saddr)
{
	edma[edma_id]->TCD[chan].SADDR.B.SADDR = saddr;
}

unsigned int edma_get_tcd_source_address_modulo(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].ATTR.B.SMOD;
}

void edma_set_tcd_source_address_modulo(unsigned int edma_id, unsigned int chan, unsigned int smod)
{
	edma[edma_id]->TCD[chan].ATTR.B.SMOD = smod;
}

unsigned int edma_get_tcd_source_data_transfer_size(unsigned int edma_id, unsigned int chan)
{
	return 1 << edma[edma_id]->TCD[chan].ATTR.B.SSIZE;
}

void edma_set_tcd_source_data_transfer_size(unsigned int edma_id, unsigned int chan, unsigned int ssize)
{
	if(ssize >= 16) edma[edma_id]->TCD[chan].ATTR.B.SSIZE = 4;
	else if(ssize >= 4) edma[edma_id]->TCD[chan].ATTR.B.SSIZE = 2;
	else if(ssize >= 2) edma[edma_id]->TCD[chan].ATTR.B.SSIZE = 1;
	else edma[edma_id]->TCD[chan].ATTR.B.SSIZE = 0;
}

unsigned int edma_get_tcd_destination_address_modulo(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].ATTR.B.DMOD;
}

void edma_set_tcd_destination_address_modulo(unsigned int edma_id, unsigned int chan, unsigned int dmod)
{
	edma[edma_id]->TCD[chan].ATTR.B.DMOD = dmod;
}

unsigned int edma_get_tcd_destination_data_transfer_size(unsigned int edma_id, unsigned int chan)
{
	return 1 << edma[edma_id]->TCD[chan].ATTR.B.DSIZE;
}

void edma_set_tcd_destination_data_transfer_size(unsigned int edma_id, unsigned int chan, unsigned int dsize)
{
	if(dsize >= 16) edma[edma_id]->TCD[chan].ATTR.B.DSIZE = 4;
	else if(dsize >= 4) edma[edma_id]->TCD[chan].ATTR.B.DSIZE = 2;
	else if(dsize >= 2) edma[edma_id]->TCD[chan].ATTR.B.DSIZE = 1;
	else edma[edma_id]->TCD[chan].ATTR.B.DSIZE = 0;
}

int32_t edma_get_tcd_signed_source_address_offset(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].SOFF.B.SOFF;
}

void edma_set_tcd_signed_source_address_offset(unsigned int edma_id, unsigned int chan, int32_t soff)
{
	edma[edma_id]->TCD[chan].SOFF.B.SOFF = soff;
}

unsigned int edma_tcd_source_minor_loop_offset_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].NBYTES.MLOFFNO.B.SMLOE;
}

unsigned int edma_tcd_destination_minor_loop_offset_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].NBYTES.MLOFFNO.B.DMLOE;
}

uint32_t edma_get_tcd_minor_byte_count(unsigned int edma_id, unsigned int chan)
{
	if(edma_minor_loop_mapping(edma_id))
	{
		if(edma_tcd_source_minor_loop_offset_enabled(edma_id, chan) || edma_tcd_destination_minor_loop_offset_enabled(edma_id, chan))
		{
			return edma[edma_id]->TCD[chan].NBYTES.MLOFFYES.B.NBYTES;
		}
		else
		{
			return edma[edma_id]->TCD[chan].NBYTES.MLOFFNO.B.NBYTES;
		}
	}
	else
	{
		return edma[edma_id]->TCD[chan].NBYTES.MLNO.B.NBYTES;
	}
}

uint32_t edma_get_tcd_minor_loop_offset(unsigned int edma_id, unsigned int chan)
{
	if(edma_minor_loop_mapping(edma_id))
	{
		if(edma_tcd_source_minor_loop_offset_enabled(edma_id, chan) || edma_tcd_destination_minor_loop_offset_enabled(edma_id, chan))
		{
			return edma[edma_id]->TCD[chan].NBYTES.MLOFFYES.B.MLOFF;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void edma_set_tcd_minor_byte_count(unsigned int edma_id, unsigned int chan, uint32_t nbytes)
{
	if(edma_minor_loop_mapping(edma_id))
	{
		edma[edma_id]->TCD[chan].NBYTES.MLOFFNO.B.NBYTES = nbytes;
	}
	else
	{
		edma[edma_id]->TCD[chan].NBYTES.MLNO.B.NBYTES = nbytes;
	}
}

void edma_set_tcd_source_minor_loop_offset(unsigned int edma_id, unsigned int chan, uint32_t mloff)
{
	if(edma_minor_loop_mapping(edma_id))
	{
		edma[edma_id]->TCD[chan].NBYTES.MLOFFYES.B.SMLOE = 1;
		edma[edma_id]->TCD[chan].NBYTES.MLOFFYES.B.MLOFF = mloff;
	}
}

void edma_set_tcd_destination_minor_loop_offset(unsigned int edma_id, unsigned int chan, uint32_t mloff)
{
	if(edma_minor_loop_mapping(edma_id))
	{
		edma[edma_id]->TCD[chan].NBYTES.MLOFFYES.B.DMLOE = 1;
		edma[edma_id]->TCD[chan].NBYTES.MLOFFYES.B.MLOFF = mloff;
	}
}

uint32_t edma_get_tcd_last_source_address_adjustment(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].SLAST.B.SLAST;
}

void edma_set_tcd_last_source_address_adjustment(unsigned int edma_id, unsigned int chan, uint32_t slast)
{
	edma[edma_id]->TCD[chan].SLAST.B.SLAST = slast;
}

uint32_t edma_get_tcd_destination_address(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].DADDR.B.DADDR;
}

void edma_set_tcd_destination_address(unsigned int edma_id, unsigned int chan, uint32_t daddr)
{
	edma[edma_id]->TCD[chan].DADDR.B.DADDR = daddr;
}

unsigned int edma_tcd_channel_to_channel_linking_currently_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CITER.ELINKNO.B.ELINK;
}

uint32_t edma_get_tcd_current_major_iteration_count(unsigned int edma_id, unsigned int chan)
{
	if(edma_tcd_channel_to_channel_linking_currently_enabled(edma_id, chan))
	{
		return edma[edma_id]->TCD[chan].CITER.ELINKYES.B.CITER;
	}
	else
	{
		return edma[edma_id]->TCD[chan].CITER.ELINKNO.B.CITER;
	}
}

unsigned int edma_get_tcd_minor_loop_link_channel_number(unsigned int edma_id, unsigned int chan)
{
	return edma_tcd_channel_to_channel_linking_currently_enabled(edma_id, chan) ? edma[edma_id]->TCD[chan].CITER.ELINKYES.B.LINKCH : 0;
}

void edma_set_tcd_current_major_iteration_count(unsigned int edma_id, unsigned int chan, uint32_t citer)
{
	edma[edma_id]->TCD[chan].CITER.ELINKNO.B.CITER = citer;
}

void edma_set_tcd_minor_loop_link_channel_number(unsigned int edma_id, unsigned int chan, unsigned int linkch)
{
	edma[edma_id]->TCD[chan].CITER.ELINKYES.B.ELINK = 1;
	edma[edma_id]->TCD[chan].CITER.ELINKYES.B.LINKCH = linkch;
}

int32_t edma_get_tcd_signed_destination_address_offset(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].DOFF.B.DOFF;
}

void edma_set_tcd_signed_destination_address_offset(unsigned int edma_id, unsigned int chan, int32_t doff)
{
	edma[edma_id]->TCD[chan].DOFF.B.DOFF = doff;
}

uint32_t edma_get_tcd_last_destination_address_adjustment(unsigned int edma_id, unsigned int chan)
{
	return edma_tcd_scatter_gather_enabled(edma_id, chan) ? 0 : edma[edma_id]->TCD[chan].DLASTSGA.B.DLASTSGA;
}

uint32_t edma_get_tcd_scatter_gather_address(unsigned int edma_id, unsigned int chan)
{
	return edma_tcd_scatter_gather_enabled(edma_id, chan) ? edma[edma_id]->TCD[chan].DLASTSGA.B.DLASTSGA : 0;
}

void edma_set_tcd_last_destination_address_adjustment(unsigned int edma_id, unsigned int chan, uint32_t ldaa)
{
	if(!edma_tcd_scatter_gather_enabled(edma_id, chan))
	{
		edma[edma_id]->TCD[chan].DLASTSGA.B.DLASTSGA = ldaa;
	}
}

void edma_set_tcd_scatter_gather_address(unsigned int edma_id, unsigned int chan, uint32_t sga)
{
	if(edma_tcd_scatter_gather_enabled(edma_id, chan))
	{
		edma[edma_id]->TCD[chan].DLASTSGA.B.DLASTSGA = sga;
	}
}

unsigned int edma_tcd_channel_to_channel_linking_enabled_at_beginning(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].BITER.ELINKNO.B.ELINK;
}

uint32_t edma_get_tcd_starting_major_iteration_count(unsigned int edma_id, unsigned int chan)
{
	if(edma_tcd_channel_to_channel_linking_enabled_at_beginning(edma_id, chan))
	{
		return edma[edma_id]->TCD[chan].BITER.ELINKYES.B.BITER;
	}
	else
	{
		return edma[edma_id]->TCD[chan].BITER.ELINKNO.B.BITER;
	}
}

unsigned int edma_get_tcd_minor_loop_link_channel_number_at_beginning(unsigned int edma_id, unsigned int chan)
{
	return edma_tcd_channel_to_channel_linking_enabled_at_beginning(edma_id, chan) ? edma[edma_id]->TCD[chan].BITER.ELINKYES.B.LINKCH : 0;
}

void edma_set_tcd_starting_major_iteration_count(unsigned int edma_id, unsigned int chan, uint32_t biter)
{
	edma[edma_id]->TCD[chan].BITER.ELINKNO.B.BITER = biter;
}

void edma_set_tcd_minor_loop_link_channel_number_at_beginning(unsigned int edma_id, unsigned int chan, unsigned int linkch)
{
	edma[edma_id]->TCD[chan].BITER.ELINKYES.B.ELINK = 1;
	edma[edma_id]->TCD[chan].BITER.ELINKYES.B.LINKCH = linkch;
}

enum eDMA_Throttle edma_get_tcd_bandwidth_control(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.BWC;
}

unsigned int edma_get_tcd_major_loop_link_channel_number(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.MAJORLINKCH;
}

unsigned int edma_tcd_channel_done(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.DONE;
}

unsigned int edma_tcd_channel_active(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.ACTIVE;
}

unsigned int edma_tcd_major_loop_linking_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.MAJORELINK;
}

unsigned int edma_tcd_scatter_gather_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.ESG;
}

unsigned int edma_tcd_request_automatically_disabled_after_major_loop_complete(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.DREQ;
}

unsigned int edma_tcd_half_major_complete_interrupt_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.INTHALF;
}

unsigned int edma_tcd_major_complete_interrupt_enabled(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.INTMAJOR;
}

unsigned int edma_tcd_channel_start_requested(unsigned int edma_id, unsigned int chan)
{
	return edma[edma_id]->TCD[chan].CSR.B.START;
}

void edma_select_tcd_bandwidth_control(unsigned int edma_id, unsigned int chan, enum eDMA_Throttle bwc)
{
	edma[edma_id]->TCD[chan].CSR.B.BWC = bwc;
}

void edma_set_tcd_major_loop_link_channel_number(unsigned int edma_id, unsigned int chan, unsigned int majorlinkch)
{
	edma[edma_id]->TCD[chan].CSR.B.MAJORLINKCH = majorlinkch;
}

void edma_set_tcd_channel_done(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.DONE = 1;
}

void edma_clear_tcd_channel_done(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.DONE = 0;
}

void edma_set_tcd_channel_active(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.ACTIVE = 1;
}

void edma_clear_tcd_channel_active(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.ACTIVE = 0;
}

void edma_enable_tcd_major_loop_linking(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.MAJORELINK = 1;
}

void edma_disable_tcd_major_loop_linking(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.MAJORELINK = 0;
}

void edma_enable_tcd_scatter_gather(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.ESG = 1;
}

void edma_disable_tcd_scatter_gather(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.ESG = 0;
}

void edma_tcd_disable_request_automatically_after_major_loop_complete(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.DREQ = 1;
}

void edma_tcd_do_not_disable_request_automatically_after_major_loop_complete(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.DREQ = 0;
}

void edma_enable_tcd_half_major_complete_interrupt(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.INTHALF = 1;
}

void edma_disable_tcd_half_major_complete_interrupt(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.INTHALF = 0;
}

void edma_enable_tcd_major_complete_interrupt(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.INTMAJOR = 1;
}

void edma_disable_tcd_major_complete_interrupt(unsigned int edma_id, unsigned int chan)
{
	edma[edma_id]->TCD[chan].CSR.B.INTMAJOR = 0;
}

edma_int_handler_t edma_set_interrupt_handler(unsigned int edma_id, unsigned int chan, edma_int_handler_t edma_int_handler)
{
	edma_int_handler_t old_edma_int_handler = edma_int_handlers[edma_id][chan];
	edma_int_handlers[edma_id][chan] = edma_int_handler;
	return old_edma_int_handler;
}

void edma_set_irq_priority(unsigned int edma_id, unsigned int chan, unsigned int priority)
{
	intc_set_irq_priority(edma_get_channel_irq_vector(edma_id, chan), priority);
}

void edma_select_irq_for_processor(unsigned int edma_id, unsigned int chan, unsigned int prc_num)
{
	intc_select_irq_for_processor(edma_get_channel_irq_vector(edma_id, chan), prc_num);
}

void edma_deselect_irq_for_processor(unsigned int edma_id, unsigned int chan, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(edma_get_channel_irq_vector(edma_id, chan), prc_num);
}

static void edma_0_int0()
{
	edma_int_handlers[0][0](0, 0);
}

static void edma_0_int1()
{
	edma_int_handlers[0][1](0, 1);
}

static void edma_0_int2()
{
	edma_int_handlers[0][2](0, 2);
}

static void edma_0_int3()
{
	edma_int_handlers[0][3](0, 3);
}

static void edma_0_int4()
{
	edma_int_handlers[0][4](0, 4);
}

static void edma_0_int5()
{
	edma_int_handlers[0][5](0, 5);
}

static void edma_0_int6()
{
	edma_int_handlers[0][6](0, 6);
}

static void edma_0_int7()
{
	edma_int_handlers[0][7](0, 7);
}

static void edma_0_int8()
{
	edma_int_handlers[0][8](0, 8);
}

static void edma_0_int9()
{
	edma_int_handlers[0][9](0, 9);
}

static void edma_0_int10()
{
	edma_int_handlers[0][10](0, 10);
}

static void edma_0_int11()
{
	edma_int_handlers[0][11](0, 11);
}

static void edma_0_int12()
{
	edma_int_handlers[0][12](0, 12);
}

static void edma_0_int13()
{
	edma_int_handlers[0][13](0, 13);
}

static void edma_0_int14()
{
	edma_int_handlers[0][14](0, 14);
}

static void edma_0_int15()
{
	edma_int_handlers[0][15](0, 15);
}

static void edma_0_int16()
{
	edma_int_handlers[0][16](0, 16);
}

static void edma_0_int17()
{
	edma_int_handlers[0][17](0, 17);
}

static void edma_0_int18()
{
	edma_int_handlers[0][18](0, 18);
}

static void edma_0_int19()
{
	edma_int_handlers[0][19](0, 19);
}

static void edma_0_int20()
{
	edma_int_handlers[0][20](0, 20);
}

static void edma_0_int21()
{
	edma_int_handlers[0][21](0, 21);
}

static void edma_0_int22()
{
	edma_int_handlers[0][22](0, 22);
}

static void edma_0_int23()
{
	edma_int_handlers[0][23](0, 23);
}

static void edma_0_int24()
{
	edma_int_handlers[0][24](0, 24);
}

static void edma_0_int25()
{
	edma_int_handlers[0][25](0, 25);
}

static void edma_0_int26()
{
	edma_int_handlers[0][26](0, 26);
}

static void edma_0_int27()
{
	edma_int_handlers[0][27](0, 27);
}

static void edma_0_int28()
{
	edma_int_handlers[0][28](0, 28);
}

static void edma_0_int29()
{
	edma_int_handlers[0][29](0, 29);
}

static void edma_0_int30()
{
	edma_int_handlers[0][30](0, 30);
}

static void edma_0_int31()
{
	edma_int_handlers[0][31](0, 31);
}

static void edma_0_int32()
{
	edma_int_handlers[0][32](0, 32);
}

static void edma_0_int33()
{
	edma_int_handlers[0][33](0, 33);
}

static void edma_0_int34()
{
	edma_int_handlers[0][34](0, 34);
}

static void edma_0_int35()
{
	edma_int_handlers[0][35](0, 35);
}

static void edma_0_int36()
{
	edma_int_handlers[0][36](0, 36);
}

static void edma_0_int37()
{
	edma_int_handlers[0][37](0, 37);
}

static void edma_0_int38()
{
	edma_int_handlers[0][38](0, 38);
}

static void edma_0_int39()
{
	edma_int_handlers[0][39](0, 39);
}

static void edma_0_int40()
{
	edma_int_handlers[0][40](0, 40);
}

static void edma_0_int41()
{
	edma_int_handlers[0][41](0, 41);
}

static void edma_0_int42()
{
	edma_int_handlers[0][42](0, 42);
}

static void edma_0_int43()
{
	edma_int_handlers[0][43](0, 43);
}

static void edma_0_int44()
{
	edma_int_handlers[0][44](0, 44);
}

static void edma_0_int45()
{
	edma_int_handlers[0][45](0, 45);
}

static void edma_0_int46()
{
	edma_int_handlers[0][46](0, 46);
}

static void edma_0_int47()
{
	edma_int_handlers[0][47](0, 47);
}

static void edma_0_int48()
{
	edma_int_handlers[0][48](0, 48);
}

static void edma_0_int49()
{
	edma_int_handlers[0][49](0, 49);
}

static void edma_0_int50()
{
	edma_int_handlers[0][50](0, 50);
}

static void edma_0_int51()
{
	edma_int_handlers[0][51](0, 51);
}

static void edma_0_int52()
{
	edma_int_handlers[0][52](0, 52);
}

static void edma_0_int53()
{
	edma_int_handlers[0][53](0, 53);
}

static void edma_0_int54()
{
	edma_int_handlers[0][54](0, 54);
}

static void edma_0_int55()
{
	edma_int_handlers[0][55](0, 55);
}

static void edma_0_int56()
{
	edma_int_handlers[0][56](0, 56);
}

static void edma_0_int57()
{
	edma_int_handlers[0][57](0, 57);
}

static void edma_0_int58()
{
	edma_int_handlers[0][58](0, 58);
}

static void edma_0_int59()
{
	edma_int_handlers[0][59](0, 59);
}

static void edma_0_int60()
{
	edma_int_handlers[0][60](0, 60);
}

static void edma_0_int61()
{
	edma_int_handlers[0][61](0, 61);
}

static void edma_0_int62()
{
	edma_int_handlers[0][62](0, 62);
}

static void edma_0_int63()
{
	edma_int_handlers[0][63](0, 63);
}

static void edma_1_int0()
{
	edma_int_handlers[1][0](1, 0);
}

static void edma_1_int1()
{
	edma_int_handlers[1][1](1, 1);
}

static void edma_1_int2()
{
	edma_int_handlers[1][2](1, 2);
}

static void edma_1_int3()
{
	edma_int_handlers[1][3](1, 3);
}

static void edma_1_int4()
{
	edma_int_handlers[1][4](1, 4);
}

static void edma_1_int5()
{
	edma_int_handlers[1][5](1, 5);
}

static void edma_1_int6()
{
	edma_int_handlers[1][6](1, 6);
}

static void edma_1_int7()
{
	edma_int_handlers[1][7](1, 7);
}

static void edma_1_int8()
{
	edma_int_handlers[1][8](1, 8);
}

static void edma_1_int9()
{
	edma_int_handlers[1][9](1, 9);
}

static void edma_1_int10()
{
	edma_int_handlers[1][10](1, 10);
}

static void edma_1_int11()
{
	edma_int_handlers[1][11](1, 11);
}

static void edma_1_int12()
{
	edma_int_handlers[1][12](1, 12);
}

static void edma_1_int13()
{
	edma_int_handlers[1][13](1, 13);
}

static void edma_1_int14()
{
	edma_int_handlers[1][14](1, 14);
}

static void edma_1_int15()
{
	edma_int_handlers[1][15](1, 15);
}

static void edma_1_int16()
{
	edma_int_handlers[1][16](1, 16);
}

static void edma_1_int17()
{
	edma_int_handlers[1][17](1, 17);
}

static void edma_1_int18()
{
	edma_int_handlers[1][18](1, 18);
}

static void edma_1_int19()
{
	edma_int_handlers[1][19](1, 19);
}

static void edma_1_int20()
{
	edma_int_handlers[1][20](1, 20);
}

static void edma_1_int21()
{
	edma_int_handlers[1][21](1, 21);
}

static void edma_1_int22()
{
	edma_int_handlers[1][22](1, 22);
}

static void edma_1_int23()
{
	edma_int_handlers[1][23](1, 23);
}

static void edma_1_int24()
{
	edma_int_handlers[1][24](1, 24);
}

static void edma_1_int25()
{
	edma_int_handlers[1][25](1, 25);
}

static void edma_1_int26()
{
	edma_int_handlers[1][26](1, 26);
}

static void edma_1_int27()
{
	edma_int_handlers[1][27](1, 27);
}

static void edma_1_int28()
{
	edma_int_handlers[1][28](1, 28);
}

static void edma_1_int29()
{
	edma_int_handlers[1][29](1, 29);
}

static void edma_1_int30()
{
	edma_int_handlers[1][30](1, 30);
}

static void edma_1_int31()
{
	edma_int_handlers[1][31](1, 31);
}

static void edma_1_int32()
{
	edma_int_handlers[1][32](1, 32);
}

static void edma_1_int33()
{
	edma_int_handlers[1][33](1, 33);
}

static void edma_1_int34()
{
	edma_int_handlers[1][34](1, 34);
}

static void edma_1_int35()
{
	edma_int_handlers[1][35](1, 35);
}

static void edma_1_int36()
{
	edma_int_handlers[1][36](1, 36);
}

static void edma_1_int37()
{
	edma_int_handlers[1][37](1, 37);
}

static void edma_1_int38()
{
	edma_int_handlers[1][38](1, 38);
}

static void edma_1_int39()
{
	edma_int_handlers[1][39](1, 39);
}

static void edma_1_int40()
{
	edma_int_handlers[1][40](1, 40);
}

static void edma_1_int41()
{
	edma_int_handlers[1][41](1, 41);
}

static void edma_1_int42()
{
	edma_int_handlers[1][42](1, 42);
}

static void edma_1_int43()
{
	edma_int_handlers[1][43](1, 43);
}

static void edma_1_int44()
{
	edma_int_handlers[1][44](1, 44);
}

static void edma_1_int45()
{
	edma_int_handlers[1][45](1, 45);
}

static void edma_1_int46()
{
	edma_int_handlers[1][46](1, 46);
}

static void edma_1_int47()
{
	edma_int_handlers[1][47](1, 47);
}

static void edma_1_int48()
{
	edma_int_handlers[1][48](1, 48);
}

static void edma_1_int49()
{
	edma_int_handlers[1][49](1, 49);
}

static void edma_1_int50()
{
	edma_int_handlers[1][50](1, 50);
}

static void edma_1_int51()
{
	edma_int_handlers[1][51](1, 51);
}

static void edma_1_int52()
{
	edma_int_handlers[1][52](1, 52);
}

static void edma_1_int53()
{
	edma_int_handlers[1][53](1, 53);
}

static void edma_1_int54()
{
	edma_int_handlers[1][54](1, 54);
}

static void edma_1_int55()
{
	edma_int_handlers[1][55](1, 55);
}

static void edma_1_int56()
{
	edma_int_handlers[1][56](1, 56);
}

static void edma_1_int57()
{
	edma_int_handlers[1][57](1, 57);
}

static void edma_1_int58()
{
	edma_int_handlers[1][58](1, 58);
}

static void edma_1_int59()
{
	edma_int_handlers[1][59](1, 59);
}

static void edma_1_int60()
{
	edma_int_handlers[1][60](1, 60);
}

static void edma_1_int61()
{
	edma_int_handlers[1][61](1, 61);
}

static void edma_1_int62()
{
	edma_int_handlers[1][62](1, 62);
}

static void edma_1_int63()
{
	edma_int_handlers[1][63](1, 63);
}
