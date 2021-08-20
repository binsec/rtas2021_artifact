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

#include "dspi.h"
#include "intc.h"
#include "MPC5777M.h"
#include "compiler_api.h"

static void __dspi_0_int_eoqf__();
static void __dspi_0_int_tfff__();
static void __dspi_0_int_cmdfff__();
static void __dspi_0_int_tcf__();
static void __dspi_0_int_cmdtcf__();
static void __dspi_0_int_rfdf__();
static void __dspi_0_int_spef__();
static void __dspi_0_int_tfuf_rfof_tfiwf__();

static void __dspi_1_int_eoqf__();
static void __dspi_1_int_tfff__();
static void __dspi_1_int_cmdfff__();
static void __dspi_1_int_tcf__();
static void __dspi_1_int_cmdtcf__();
static void __dspi_1_int_rfdf__();
static void __dspi_1_int_spef__();
static void __dspi_1_int_tfuf_rfof_tfiwf__();

static void __dspi_2_int_eoqf__();
static void __dspi_2_int_tfff__();
static void __dspi_2_int_cmdfff__();
static void __dspi_2_int_tcf__();
static void __dspi_2_int_cmdtcf__();
static void __dspi_2_int_rfdf__();
static void __dspi_2_int_spef__();
static void __dspi_2_int_tfuf_rfof_tfiwf__();

static void __dspi_3_int_eoqf__();
static void __dspi_3_int_tfff__();
static void __dspi_3_int_cmdfff__();
static void __dspi_3_int_tcf__();
static void __dspi_3_int_cmdtcf__();
static void __dspi_3_int_rfdf__();
static void __dspi_3_int_spef__();
static void __dspi_3_int_tfuf_rfof_tfiwf__();

static void __dspi_4_int_eoqf__();
static void __dspi_4_int_tfff__();
static void __dspi_4_int_tcf__();
static void __dspi_4_int_rfdf__();
static void __dspi_4_int_ddif__();
static void __dspi_4_int_tfuf_rfof_tfiwf__();
static void __dspi_4_int_spitcf_cmdtcf__();
static void __dspi_4_int_dsitcf_cmdfff__();
static void __dspi_4_int_spef_dpef__();

static void __dspi_5_int_eoqf__();
static void __dspi_5_int_tfff__();
static void __dspi_5_int_tcf__();
static void __dspi_5_int_rfdf__();
static void __dspi_5_int_ddif__();
static void __dspi_5_int_tfuf_rfof_tfiwf__();
static void __dspi_5_int_spitcf_cmdtcf__();
static void __dspi_5_int_dsitcf_cmdfff__();
static void __dspi_5_int_spef_dpef__();

static void __dspi_6_int_eoqf__();
static void __dspi_6_int_tfff__();
static void __dspi_6_int_tcf__();
static void __dspi_6_int_rfdf__();
static void __dspi_6_int_ddif__();
static void __dspi_6_int_tfuf_rfof_tfiwf__();
static void __dspi_6_int_spitcf_cmdtcf__();
static void __dspi_6_int_dsitcf_cmdfff__();
static void __dspi_6_int_spef_dpef__();

static void __dspi_12_int_eoqf__();
static void __dspi_12_int_tfff__();
static void __dspi_12_int_cmdfff__();
static void __dspi_12_int_tcf__();
static void __dspi_12_int_cmdtcf__();
static void __dspi_12_int_rfdf__();
static void __dspi_12_int_spef__();
static void __dspi_12_int_tfuf_rfof_tfiwf__();

void dspi_int_handler_default(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
}

static volatile struct DSPI_tag *dspi[13];
static dspi_int_handler_t dspi_int_handlers[13][18];
static uint16_t dspi_irq_vector_table[13][18];

static unsigned int dspi_get_irq_vector(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	return dspi_irq_vector_table[dspi_id][dspi_req];
}

void dspi_drv_init()
{
	dspi[0] = &DSPI_0;
	dspi[1] = &DSPI_1;
	dspi[2] = &DSPI_2;
	dspi[3] = &DSPI_3;
	dspi[4] = &DSPI_4;
	dspi[5] = &DSPI_5;
	dspi[6] = &DSPI_6;
	dspi[7] = 0;
	dspi[8] = 0;
	dspi[9] = 0;
	dspi[10] = 0;
	dspi[11] = 0;
	dspi[12] = &DSPI_12;
	
	unsigned int dspi_id;
	unsigned int int_id;
	for(dspi_id = 0; dspi_id < 13; dspi_id++)
	{
		for(int_id = 0; int_id < 14; int_id++)
		{
			dspi_int_handlers[dspi_id][int_id] = dspi_int_handler_default;
		}
	}
	
	dspi_irq_vector_table[0][DSPI_REQ_EOQF  ] = 260;
	dspi_irq_vector_table[0][DSPI_REQ_TFFF  ] = 261;
	dspi_irq_vector_table[0][DSPI_REQ_CMDFFF] = 265;
	dspi_irq_vector_table[0][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_TCF   ] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_CMDTCF] = 264;
	dspi_irq_vector_table[0][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_RFDF  ] = 263;
	dspi_irq_vector_table[0][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_SPEF  ] = 266;
	dspi_irq_vector_table[0][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_DDIF  ] = 0;
	dspi_irq_vector_table[0][DSPI_REQ_TFUF_RFOF_TFIWF] = 259;
	dspi_irq_vector_table[0][DSPI_REQ_SPITCF_CMDTCF  ] = 264;
	dspi_irq_vector_table[0][DSPI_REQ_DSITCF_CMDFFF  ] = 265;
	dspi_irq_vector_table[0][DSPI_REQ_SPEF_DPEF      ] = 266;
	
	dspi_irq_vector_table[1][DSPI_REQ_EOQF  ] = 269;
	dspi_irq_vector_table[1][DSPI_REQ_TFFF  ] = 270;
	dspi_irq_vector_table[1][DSPI_REQ_CMDFFF] = 274;
	dspi_irq_vector_table[1][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_TCF   ] = 271;
	dspi_irq_vector_table[1][DSPI_REQ_CMDTCF] = 273;
	dspi_irq_vector_table[1][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_RFDF  ] = 272;
	dspi_irq_vector_table[1][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_SPEF  ] = 275;
	dspi_irq_vector_table[1][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_DDIF  ] = 0;
	dspi_irq_vector_table[1][DSPI_REQ_TFUF_RFOF_TFIWF] = 268;
	dspi_irq_vector_table[1][DSPI_REQ_SPITCF_CMDTCF  ] = 273;
	dspi_irq_vector_table[1][DSPI_REQ_DSITCF_CMDFFF  ] = 274;
	dspi_irq_vector_table[1][DSPI_REQ_SPEF_DPEF      ] = 275;
	
	dspi_irq_vector_table[2][DSPI_REQ_EOQF  ] = 278;
	dspi_irq_vector_table[2][DSPI_REQ_TFFF  ] = 279;
	dspi_irq_vector_table[2][DSPI_REQ_CMDFFF] = 283;
	dspi_irq_vector_table[2][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_TCF   ] = 280;
	dspi_irq_vector_table[2][DSPI_REQ_CMDTCF] = 282;
	dspi_irq_vector_table[2][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_RFDF  ] = 281;
	dspi_irq_vector_table[2][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_SPEF  ] = 284;
	dspi_irq_vector_table[2][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_DDIF  ] = 0;
	dspi_irq_vector_table[2][DSPI_REQ_TFUF_RFOF_TFIWF] = 277;
	dspi_irq_vector_table[2][DSPI_REQ_SPITCF_CMDTCF  ] = 282;
	dspi_irq_vector_table[2][DSPI_REQ_DSITCF_CMDFFF  ] = 283;
	dspi_irq_vector_table[2][DSPI_REQ_SPEF_DPEF      ] = 284;
	
	
	dspi_irq_vector_table[3][DSPI_REQ_EOQF  ] = 287;
	dspi_irq_vector_table[3][DSPI_REQ_TFFF  ] = 288;
	dspi_irq_vector_table[3][DSPI_REQ_CMDFFF] = 292;
	dspi_irq_vector_table[3][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_TCF   ] = 289;
	dspi_irq_vector_table[3][DSPI_REQ_CMDTCF] = 291;
	dspi_irq_vector_table[3][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_RFDF  ] = 290;
	dspi_irq_vector_table[3][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_SPEF  ] = 293;
	dspi_irq_vector_table[3][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_DDIF  ] = 0;
	dspi_irq_vector_table[3][DSPI_REQ_TFUF_RFOF_TFIWF] = 286;
	dspi_irq_vector_table[3][DSPI_REQ_SPITCF_CMDTCF  ] = 291;
	dspi_irq_vector_table[3][DSPI_REQ_DSITCF_CMDFFF  ] = 292;
	dspi_irq_vector_table[3][DSPI_REQ_SPEF_DPEF      ] = 293;
	
	dspi_irq_vector_table[4][DSPI_REQ_EOQF  ] = 296;
	dspi_irq_vector_table[4][DSPI_REQ_TFFF  ] = 297;
	dspi_irq_vector_table[4][DSPI_REQ_CMDFFF] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_TCF   ] = 298;
	dspi_irq_vector_table[4][DSPI_REQ_CMDTCF] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_RFDF  ] = 299;
	dspi_irq_vector_table[4][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_SPEF  ] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[4][DSPI_REQ_DDIF  ] = 303;
	dspi_irq_vector_table[4][DSPI_REQ_TFUF_RFOF_TFIWF] = 295;
	dspi_irq_vector_table[4][DSPI_REQ_SPITCF_CMDTCF  ] = 300;
	dspi_irq_vector_table[4][DSPI_REQ_DSITCF_CMDFFF  ] = 301;
	dspi_irq_vector_table[4][DSPI_REQ_SPEF_DPEF      ] = 302;
	
	dspi_irq_vector_table[5][DSPI_REQ_EOQF  ] = 305;
	dspi_irq_vector_table[5][DSPI_REQ_TFFF  ] = 306;
	dspi_irq_vector_table[5][DSPI_REQ_CMDFFF] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_TCF   ] = 307;
	dspi_irq_vector_table[5][DSPI_REQ_CMDTCF] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_RFDF  ] = 308;
	dspi_irq_vector_table[5][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_SPEF  ] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[5][DSPI_REQ_DDIF  ] = 312;
	dspi_irq_vector_table[5][DSPI_REQ_TFUF_RFOF_TFIWF] = 304;
	dspi_irq_vector_table[5][DSPI_REQ_SPITCF_CMDTCF  ] = 309;
	dspi_irq_vector_table[5][DSPI_REQ_DSITCF_CMDFFF  ] = 310;
	dspi_irq_vector_table[5][DSPI_REQ_SPEF_DPEF      ] = 311;
	
	dspi_irq_vector_table[6][DSPI_REQ_EOQF  ] = 314;
	dspi_irq_vector_table[6][DSPI_REQ_TFFF  ] = 315;
	dspi_irq_vector_table[6][DSPI_REQ_CMDFFF] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_TCF   ] = 316;
	dspi_irq_vector_table[6][DSPI_REQ_CMDTCF] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_RFDF  ] = 317;
	dspi_irq_vector_table[6][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_SPEF  ] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[6][DSPI_REQ_DDIF  ] = 321;
	dspi_irq_vector_table[6][DSPI_REQ_TFUF_RFOF_TFIWF] = 313;
	dspi_irq_vector_table[6][DSPI_REQ_SPITCF_CMDTCF  ] = 318;
	dspi_irq_vector_table[6][DSPI_REQ_DSITCF_CMDFFF  ] = 319;
	dspi_irq_vector_table[6][DSPI_REQ_SPEF_DPEF      ] = 320;
	
	dspi_irq_vector_table[12][DSPI_REQ_EOQF  ] = 368;
	dspi_irq_vector_table[12][DSPI_REQ_TFFF  ] = 369;
	dspi_irq_vector_table[12][DSPI_REQ_CMDFFF] = 373;
	dspi_irq_vector_table[12][DSPI_REQ_TFIWF ] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_TCF   ] = 370;
	dspi_irq_vector_table[12][DSPI_REQ_CMDTCF] = 372;
	dspi_irq_vector_table[12][DSPI_REQ_SPITCF] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_DSITCF] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_TFUF  ] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_RFDF  ] = 371;
	dspi_irq_vector_table[12][DSPI_REQ_RFOF  ] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_SPEF  ] = 374;
	dspi_irq_vector_table[12][DSPI_REQ_DPEF  ] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_DDIF  ] = 0;
	dspi_irq_vector_table[12][DSPI_REQ_TFUF_RFOF_TFIWF] = 367;
	dspi_irq_vector_table[12][DSPI_REQ_SPITCF_CMDTCF  ] = 372;
	dspi_irq_vector_table[12][DSPI_REQ_DSITCF_CMDFFF  ] = 373;
	dspi_irq_vector_table[12][DSPI_REQ_SPEF_DPEF      ] = 374;
}

void dspi_init(unsigned int dspi_id)
{
	switch(dspi_id)
	{
		case 0:
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_EOQF), &__dspi_0_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_TFFF), &__dspi_0_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_CMDFFF), &__dspi_0_int_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_TCF), &__dspi_0_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_CMDTCF), &__dspi_0_int_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_RFDF), &__dspi_0_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_SPEF), &__dspi_0_int_spef__);
			intc_set_interrupt_handler(dspi_get_irq_vector(0, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_0_int_tfuf_rfof_tfiwf__);
			break;
			
		case 1:
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_EOQF), &__dspi_1_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_TFFF), &__dspi_1_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_CMDFFF), &__dspi_1_int_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_TCF), &__dspi_1_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_CMDTCF), &__dspi_1_int_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_RFDF), &__dspi_1_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_SPEF), &__dspi_1_int_spef__);
			intc_set_interrupt_handler(dspi_get_irq_vector(1, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_1_int_tfuf_rfof_tfiwf__);
			break;
			
		case 2:
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_EOQF), &__dspi_2_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_TFFF), &__dspi_2_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_CMDFFF), &__dspi_2_int_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_TCF), &__dspi_2_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_CMDTCF), &__dspi_2_int_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_RFDF), &__dspi_2_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_SPEF), &__dspi_2_int_spef__);
			intc_set_interrupt_handler(dspi_get_irq_vector(2, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_2_int_tfuf_rfof_tfiwf__);
			break;
			
		case 3:
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_EOQF), &__dspi_3_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_TFFF), &__dspi_3_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_CMDFFF), &__dspi_3_int_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_TCF), &__dspi_3_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_CMDTCF), &__dspi_3_int_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_RFDF), &__dspi_3_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_SPEF), &__dspi_3_int_spef__);
			intc_set_interrupt_handler(dspi_get_irq_vector(3, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_3_int_tfuf_rfof_tfiwf__);
			break;
		
		case 4:
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_EOQF), &__dspi_4_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_TFFF), &__dspi_4_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_TCF), &__dspi_4_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_RFDF), &__dspi_4_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_DDIF), &__dspi_4_int_ddif__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_4_int_tfuf_rfof_tfiwf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_SPITCF_CMDTCF), &__dspi_4_int_spitcf_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_DSITCF_CMDFFF), &__dspi_4_int_dsitcf_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(4, DSPI_REQ_SPEF_DPEF), &__dspi_4_int_spef_dpef__);
			break;
			
		case 5:
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_EOQF), &__dspi_5_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_TFFF), &__dspi_5_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_TCF), &__dspi_5_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_RFDF), &__dspi_5_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_DDIF), &__dspi_5_int_ddif__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_5_int_tfuf_rfof_tfiwf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_SPITCF_CMDTCF), &__dspi_5_int_spitcf_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_DSITCF_CMDFFF), &__dspi_5_int_dsitcf_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(5, DSPI_REQ_SPEF_DPEF), &__dspi_5_int_spef_dpef__);
			break;
			
		case 6:
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_EOQF), &__dspi_6_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_TFFF), &__dspi_6_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_TCF), &__dspi_6_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_RFDF), &__dspi_6_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_DDIF), &__dspi_6_int_ddif__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_6_int_tfuf_rfof_tfiwf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_SPITCF_CMDTCF), &__dspi_6_int_spitcf_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_DSITCF_CMDFFF), &__dspi_6_int_dsitcf_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(6, DSPI_REQ_SPEF_DPEF), &__dspi_6_int_spef_dpef__);
			break;
			
		case 12:
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_EOQF), &__dspi_12_int_eoqf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_TFFF), &__dspi_12_int_tfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_CMDFFF), &__dspi_12_int_cmdfff__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_TCF), &__dspi_12_int_tcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_CMDTCF), &__dspi_12_int_cmdtcf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_RFDF), &__dspi_12_int_rfdf__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_SPEF), &__dspi_12_int_spef__);
			intc_set_interrupt_handler(dspi_get_irq_vector(12, DSPI_REQ_TFUF_RFOF_TFIWF), &__dspi_12_int_tfuf_rfof_tfiwf__);
			break;
	}
}

enum DSPI_MODE dspi_get_mode(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.MSTR;
}

void dspi_set_mode(unsigned int dspi_id, enum DSPI_MODE dspi_mode)
{
	dspi[dspi_id]->MCR.B.MSTR = dspi_mode;
}

unsigned int dspi_is_continuous_sck_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.CONT_SCKE;
}

void dspi_enable_continuous_sck(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.CONT_SCKE = 1;
}

void dspi_disable_continuous_sck(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.CONT_SCKE = 0;
}

enum DSPI_CONFIGURATION dspi_get_configuration(unsigned int dspi_id)
{
	return (enum DSPI_CONFIGURATION) dspi[dspi_id]->MCR.B.DCONF;
}

void dspi_set_configuration(unsigned int dspi_id, enum DSPI_CONFIGURATION dspi_conf)
{
	dspi[dspi_id]->MCR.B.DCONF = dspi_conf;
}

unsigned int dspi_is_freezed(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.FRZ;
}

void dspi_freeze(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.FRZ = 1;
}

void dspi_unfreeze(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.FRZ = 0;
}

void dspi_enable_modified_timing_format(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.MTFE = 1;
}

unsigned int dspi_is_modified_timing_format_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.MTFE;
}

void dspi_disable_modified_timing_format(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.MTFE = 0;
}

unsigned int dspi_is_peripheral_chip_select_strobe_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.PCSSE;
}

void dspi_enable_peripheral_chip_select_strobe(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.PCSSE = 1;
}

void dspi_disable_peripheral_chip_select_strobe(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.PCSSE = 0;
}

unsigned int dspi_is_receive_fifo_overflow_overwrite_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.ROOE;
}

void dspi_enable_receive_fifo_overflow_overwrite(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.ROOE = 1;
}

void dspi_disable_receive_fifo_overflow_overwrite(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.ROOE = 0;
}

uint8_t dspi_get_peripheral_chip_select_inactive_states(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.PCSIS;
}

void dspi_set_peripheral_chip_select_inactive_states(unsigned int dspi_id, uint8_t inactive_states)
{
	dspi[dspi_id]->MCR.B.PCSIS = inactive_states;
}

unsigned int dspi_is_module_disabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.MDIS;
}

void dspi_disable_module(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.MDIS = 1;
}

void dspi_enable_module(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.MDIS = 0;
}

unsigned int dspi_is_transmit_fifo_disabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.DIS_TXF;
}

void dspi_disable_transmit_fifo(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.DIS_TXF = 1;
}

void dspi_enable_transmit_fifo(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.DIS_TXF = 0;
}

unsigned int dspi_is_receive_fifo_disabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.DIS_RXF;
}

void dspi_disable_receive_fifo(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.DIS_RXF = 1;
}

void dspi_enable_receive_fifo(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.DIS_RXF = 0;
}

void dspi_clear_tx_fifo(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.CLR_TXF = 1;
}

void dspi_clear_rx_fifo(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.CLR_RXF = 1;
}

enum DSPI_SAMPLE_POINT dspi_get_sample_point(unsigned int dspi_id)
{
	return (enum DSPI_SAMPLE_POINT) dspi[dspi_id]->MCR.B.SMPL_PT;
}

void dspi_set_sample_point(unsigned int dspi_id, enum DSPI_SAMPLE_POINT dspi_sample_point)
{
	dspi[dspi_id]->MCR.B.SMPL_PT = dspi_sample_point;
}

enum DSPI_SPI_MODE dspi_get_spi_mode(unsigned int dspi_id)
{
	return (enum DSPI_SPI_MODE) dspi[dspi_id]->MCR.B.XSPI;
}

void dspi_set_spi_mode(unsigned int dspi_id, enum DSPI_SPI_MODE dspi_spi_mode)
{
	dspi[dspi_id]->MCR.B.XSPI = dspi_spi_mode;
}

enum DSPI_CONTINUOUS_PCS_MODE dspi_get_continuous_pcs_mode(unsigned int dspi_id)
{
	return (enum DSPI_CONTINUOUS_PCS_MODE) dspi[dspi_id]->MCR.B.FCPCS;
}

void dspi_set_continuous_pcs_mode(unsigned int dspi_id, enum DSPI_CONTINUOUS_PCS_MODE dspi_continuous_pcs_mode)
{
	dspi[dspi_id]->MCR.B.FCPCS = dspi_continuous_pcs_mode;
}

enum DSPI_PARITY_ERROR_MODE dspi_get_spi_parity_error_mode(unsigned int dspi_id)
{
	return (enum DSPI_PARITY_ERROR_MODE) dspi[dspi_id]->MCR.B.PES;
}

void dspi_set_spi_parity_error_mode(unsigned int dspi_id, enum DSPI_PARITY_ERROR_MODE dspi_spi_parity_error_mode)
{
	dspi[dspi_id]->MCR.B.PES = dspi_spi_parity_error_mode;
}

unsigned int dspi_are_tranfers_halted(unsigned int dspi_id)
{
	return dspi[dspi_id]->MCR.B.HALT;
}

void dspi_start_transfers(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.HALT = 0;
}

void dspi_stop_transfers(unsigned int dspi_id)
{
	dspi[dspi_id]->MCR.B.HALT = 1;
}

unsigned int dspi_get_spi_transfer_count(unsigned int dspi_id)
{
	return dspi[dspi_id]->TCR.B.SPI_TCNT;
}

void dspi_get_spi_clock_and_transfer_attributes(unsigned int dspi_id, unsigned int ctas, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
		{
			DSPI_MODE_CTAR_SLAVE_tag ctar_slave = dspi[dspi_id]->MODE.CTAR_SLAVE[0];
			
			buf->double_baud_rate = 0;
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					buf->frame_size = ctar_slave.B.FMSZ + 1;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					buf->frame_size = ctar_slave.B.FMSZ + (dspi[dspi_id]->CTARE[0].B.FMSZE << 4) + 1;
					break;
			}
			
			buf->clock_polarity = (enum DSPI_CLOCK_POLARITY) ctar_slave.B.CPOL;
			buf->clock_phase = (enum DSPI_CLOCK_PHASE) ctar_slave.B.CPOL;
			buf->lsb_first = 0;
			buf->pcs_to_sck_delay_prescaler = 0;
			buf->after_sck_delay_prescaler = 0;
			buf->delay_after_transfer_prescaler = 0;
			buf->baud_rate_prescaler = 0;
			buf->pcs_to_sck_delay_scaler = 0;
			buf->after_sck_delay_scaler = 0;
			buf->delay_after_transfer_scaler = 0;
			buf->baud_rate_scaler = 0;
			buf->parity_enable = ctar_slave.B.PE;
			buf->parity_polarity = ctar_slave.B.PP;
			break;
		}
			
		case DSPI_MASTER_MODE:
		{
			DSPI_MODE_CTAR_tag ctar = dspi[dspi_id]->MODE.CTAR[ctas];
			
			buf->double_baud_rate = ctar.B.DBR;
			
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					buf->frame_size = ctar.B.FMSZ + 1;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					buf->frame_size = ctar.B.FMSZ + (dspi[dspi_id]->CTARE[ctas].B.FMSZE << 4) + 1;
					break;
			}
			
			buf->clock_polarity = (enum DSPI_CLOCK_POLARITY) ctar.B.CPOL;
			buf->clock_phase = (enum DSPI_CLOCK_PHASE) ctar.B.CPHA;
			buf->lsb_first = ctar.B.LSBFE;
			buf->pcs_to_sck_delay_prescaler = ctar.B.PCSSCK;
			buf->after_sck_delay_prescaler = ctar.B.PASC;
			buf->delay_after_transfer_prescaler = ctar.B.PDT;
			buf->baud_rate_prescaler = ctar.B.PBR;
			buf->pcs_to_sck_delay_scaler = ctar.B.CSSCK;
			buf->after_sck_delay_scaler = ctar.B.ASC;
			buf->delay_after_transfer_scaler = ctar.B.DT;
			buf->baud_rate_scaler = ctar.B.BR;
			buf->parity_enable = 0;
			buf->parity_polarity = 0;
			break;
		}
	}
}

void dspi_set_spi_clock_and_transfer_attributes(unsigned int dspi_id, unsigned int ctas, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
		{
			DSPI_MODE_CTAR_SLAVE_tag ctar_slave = dspi[dspi_id]->MODE.CTAR_SLAVE[0];
			
			unsigned int frame_size_minus_1 = buf->frame_size ? (buf->frame_size - 1) : 0;
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					ctar_slave.B.FMSZ = frame_size_minus_1 & 0xf;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					ctar_slave.B.FMSZ = frame_size_minus_1 & 0xf;
					dspi[dspi_id]->CTARE[0].B.FMSZE = (frame_size_minus_1 >> 4) & 1;
					break;
			}
			
			ctar_slave.B.CPOL = buf->clock_polarity;
			ctar_slave.B.CPOL = buf->clock_phase;
			ctar_slave.B.PE = buf->parity_enable;
			ctar_slave.B.PP = buf->parity_polarity;
			
			dspi[dspi_id]->MODE.CTAR_SLAVE[0] = ctar_slave;
			break;
		}
			
		case DSPI_MASTER_MODE:
		{
			DSPI_MODE_CTAR_tag ctar = dspi[dspi_id]->MODE.CTAR[ctas];
			
			ctar.B.DBR = buf->double_baud_rate;
			
			unsigned int frame_size_minus_1 = buf->frame_size ? (buf->frame_size - 1) : 0;
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					ctar.B.FMSZ = frame_size_minus_1 & 0xf;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					ctar.B.FMSZ = frame_size_minus_1 & 0xf;
					dspi[dspi_id]->CTARE[ctas].B.FMSZE = (frame_size_minus_1 >> 4) & 1;
					break;
			}
			
			ctar.B.CPOL = buf->clock_polarity;
			ctar.B.CPHA = buf->clock_phase;
			ctar.B.LSBFE = buf->lsb_first;
			ctar.B.PCSSCK = buf->pcs_to_sck_delay_prescaler;
			ctar.B.PASC = buf->after_sck_delay_prescaler;
			ctar.B.PDT = buf->delay_after_transfer_prescaler;
			ctar.B.PBR = buf->baud_rate_prescaler;
			ctar.B.CSSCK = buf->pcs_to_sck_delay_scaler;
			ctar.B.ASC = buf->after_sck_delay_scaler;
			ctar.B.DT = buf->delay_after_transfer_scaler;
			ctar.B.BR = buf->baud_rate_scaler;
			
			dspi[dspi_id]->MODE.CTAR[ctas] = ctar;
			break;
		}
	}
}

void dspi_get_dsi_clock_and_transfer_attributes(unsigned int dspi_id, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
		{
			DSPI_MODE_CTAR_SLAVE_tag ctar_slave = dspi[dspi_id]->MODE.CTAR_SLAVE[1];
			
			buf->double_baud_rate = 0;
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					buf->frame_size = ctar_slave.B.FMSZ + 1;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					buf->frame_size = ctar_slave.B.FMSZ + (dspi[dspi_id]->DSICR0.B.FMSZ4 << 4) + 1;
					break;
			}
			
			buf->clock_polarity = (enum DSPI_CLOCK_POLARITY) ctar_slave.B.CPOL;
			buf->clock_phase = (enum DSPI_CLOCK_PHASE) ctar_slave.B.CPOL;
			buf->lsb_first = 0;
			buf->pcs_to_sck_delay_prescaler = 0;
			buf->after_sck_delay_prescaler = 0;
			buf->delay_after_transfer_prescaler = 0;
			buf->baud_rate_prescaler = 0;
			buf->pcs_to_sck_delay_scaler = 0;
			buf->after_sck_delay_scaler = 0;
			buf->delay_after_transfer_scaler = 0;
			buf->baud_rate_scaler = 0;
			buf->parity_enable = ctar_slave.B.PE;
			buf->parity_polarity = ctar_slave.B.PP;
			
			dspi[dspi_id]->MODE.CTAR_SLAVE[1] = ctar_slave;
			break;
		}
			
		case DSPI_MASTER_MODE:
		{
			unsigned int ctas = dspi_get_dsi_clock_and_transfer_attribute_select(dspi_id);
			DSPI_MODE_CTAR_tag ctar = dspi[dspi_id]->MODE.CTAR[ctas];
	
			buf->double_baud_rate = ctar.B.DBR;
			
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					buf->frame_size = ctar.B.FMSZ + 1;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					buf->frame_size = ctar.B.FMSZ + (dspi[dspi_id]->CTARE[ctas].B.FMSZE << 4) + 1;
					break;
			}
			
			buf->clock_polarity = (enum DSPI_CLOCK_POLARITY) ctar.B.CPOL;
			buf->clock_phase = (enum DSPI_CLOCK_PHASE) ctar.B.CPHA;
			buf->lsb_first = ctar.B.LSBFE;
			buf->pcs_to_sck_delay_prescaler = ctar.B.PCSSCK;
			buf->after_sck_delay_prescaler = ctar.B.PASC;
			buf->delay_after_transfer_prescaler = ctar.B.PDT;
			buf->baud_rate_prescaler = ctar.B.PBR;
			buf->pcs_to_sck_delay_scaler = ctar.B.CSSCK;
			buf->after_sck_delay_scaler = ctar.B.ASC;
			buf->delay_after_transfer_scaler = ctar.B.DT;
			buf->baud_rate_scaler = ctar.B.BR;
			buf->parity_enable = 0;
			buf->parity_polarity = 0;
			
			dspi[dspi_id]->MODE.CTAR[ctas] = ctar;
			break;
		}
	}
}

void dspi_set_dsi_clock_and_transfer_attributes(unsigned int dspi_id, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
		{
			DSPI_MODE_CTAR_SLAVE_tag ctar_slave = dspi[dspi_id]->MODE.CTAR_SLAVE[1];
			
			unsigned int frame_size_minus_1 = buf->frame_size ? (buf->frame_size - 1) : 0;
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					ctar_slave.B.FMSZ = frame_size_minus_1 & 0xf;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					ctar_slave.B.FMSZ = frame_size_minus_1 & 0xf;
					dspi[dspi_id]->CTARE[1].B.FMSZE = (frame_size_minus_1 >> 4) & 1;
					break;
			}
			
			ctar_slave.B.CPOL = buf->clock_polarity;
			ctar_slave.B.CPOL = buf->clock_phase;
			ctar_slave.B.PE = buf->parity_enable;
			ctar_slave.B.PP = buf->parity_polarity;
			
			dspi[dspi_id]->MODE.CTAR_SLAVE[1] = ctar_slave;
			break;
		}
			
		case DSPI_MASTER_MODE:
		{
			unsigned int ctas = dspi_get_dsi_clock_and_transfer_attribute_select(dspi_id);
			DSPI_MODE_CTAR_tag ctar = dspi[dspi_id]->MODE.CTAR[ctas];

			ctar.B.DBR = buf->double_baud_rate;
			
			unsigned int frame_size_minus_1 = buf->frame_size ? (buf->frame_size - 1) : 0;
			switch(dspi_get_spi_mode(dspi_id))
			{
				case DSPI_SPI_NORMAL_MODE:
					ctar.B.FMSZ = frame_size_minus_1 & 0xf;
					break;
				case DSPI_EXTENDED_SPI_MODE:
					ctar.B.FMSZ = frame_size_minus_1 & 0xf;
					dspi[dspi_id]->CTARE[ctas].B.FMSZE = (frame_size_minus_1 >> 4) & 1;
					break;
			}
			
			ctar.B.CPOL = buf->clock_polarity;
			ctar.B.CPHA = buf->clock_phase;
			ctar.B.LSBFE = buf->lsb_first;
			ctar.B.PCSSCK = buf->pcs_to_sck_delay_prescaler;
			ctar.B.PASC = buf->after_sck_delay_prescaler;
			ctar.B.PDT = buf->delay_after_transfer_prescaler;
			ctar.B.PBR = buf->baud_rate_prescaler;
			ctar.B.CSSCK = buf->pcs_to_sck_delay_scaler;
			ctar.B.ASC = buf->after_sck_delay_scaler;
			ctar.B.DT = buf->delay_after_transfer_scaler;
			ctar.B.BR = buf->baud_rate_scaler;
			
			dspi[dspi_id]->MODE.CTAR[ctas] = ctar;
			break;
		}
	}
}

unsigned int dspi_get_interrupt_request_flag(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	DSPI_SR_tag sr = dspi[dspi_id]->SR;
	switch(dspi_req)
	{
		case DSPI_REQ_EOQF           : return sr.B.EOQF  ;
		case DSPI_REQ_TFFF           : return sr.B.TFFF  ;
		case DSPI_REQ_CMDFFF         : return sr.B.CMDFFF;
		case DSPI_REQ_TFIWF          : return sr.B.TFIWF ;
		case DSPI_REQ_TCF            : return sr.B.TCF   ;
		case DSPI_REQ_CMDTCF         : return sr.B.CMDTCF;
		case DSPI_REQ_SPITCF         : return sr.B.SPITCF;
		case DSPI_REQ_DSITCF         : return sr.B.DSITCF;
		case DSPI_REQ_TFUF           : return sr.B.TFUF  ;
		case DSPI_REQ_RFDF           : return sr.B.RFDF  ;
		case DSPI_REQ_RFOF           : return sr.B.RFOF  ;
		case DSPI_REQ_SPEF           : return sr.B.SPEF  ;
		case DSPI_REQ_DPEF           : return sr.B.DPEF  ;
		case DSPI_REQ_DDIF           : return sr.B.DDIF  ;
		case DSPI_REQ_TFUF_RFOF_TFIWF: return sr.B.TFUF | sr.B.RFOF | sr.B.TFIWF;
		case DSPI_REQ_SPITCF_CMDTCF  : return sr.B.SPITCF | sr.B.CMDTCF;
		case DSPI_REQ_DSITCF_CMDFFF  : return sr.B.DSITCF | sr.B.CMDFFF;
		case DSPI_REQ_SPEF_DPEF      : return sr.B.SPEF | sr.B.DPEF;
	}
	
	return 0;
}

void dspi_clear_interrupt_request_flag(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	DSPI_SR_tag sr = dspi[dspi_id]->SR;
	DSPI_SR_tag tmp;
	tmp.R = 0;
	tmp.B.BSYF = sr.B.BSYF;
	switch(dspi_req)
	{
		case DSPI_REQ_EOQF           : if(sr.B.EOQF  ) tmp.B.EOQF   = 1; break;
		case DSPI_REQ_TFFF           : if(sr.B.TFFF  ) tmp.B.TFFF   = 1; break;
		case DSPI_REQ_CMDFFF         : if(sr.B.CMDFFF) tmp.B.CMDFFF = 1; break;
		case DSPI_REQ_TFIWF          : if(sr.B.TFIWF ) tmp.B.TFIWF  = 1; break;
		case DSPI_REQ_TCF            : if(sr.B.TCF   ) tmp.B.TCF    = 1; break;
		case DSPI_REQ_CMDTCF         : if(sr.B.CMDTCF) tmp.B.CMDTCF = 1; break;
		case DSPI_REQ_SPITCF         : if(sr.B.SPITCF) tmp.B.SPITCF = 1; break;
		case DSPI_REQ_DSITCF         : if(sr.B.DSITCF) tmp.B.DSITCF = 1; break;
		case DSPI_REQ_TFUF           : if(sr.B.TFUF  ) tmp.B.TFUF   = 1; break;
		case DSPI_REQ_RFDF           : if(sr.B.RFDF  ) tmp.B.RFDF   = 1; break;
		case DSPI_REQ_RFOF           : if(sr.B.RFOF  ) tmp.B.RFOF   = 1; break;
		case DSPI_REQ_SPEF           : if(sr.B.SPEF  ) tmp.B.SPEF   = 1; break;
		case DSPI_REQ_DPEF           : if(sr.B.DPEF  ) tmp.B.DPEF   = 1; break;
		case DSPI_REQ_DDIF           : if(sr.B.DDIF  ) tmp.B.DDIF   = 1; break;
		case DSPI_REQ_TFUF_RFOF_TFIWF: if(sr.B.TFUF  ) { tmp.B.TFUF   = 1; } if(sr.B.RFOF  ) { tmp.B.RFOF   = 1; } if(sr.B.TFIWF) { tmp.B.TFIWF = 1; } break;
		case DSPI_REQ_SPITCF_CMDTCF  : if(sr.B.SPITCF) { tmp.B.SPITCF = 1; } if(sr.B.CMDTCF) { tmp.B.CMDTCF = 1; } break;
		case DSPI_REQ_DSITCF_CMDFFF  : if(sr.B.DSITCF) { tmp.B.DSITCF = 1; } if(sr.B.CMDFFF) { tmp.B.CMDFFF = 1; } break;
		case DSPI_REQ_SPEF_DPEF      : if(sr.B.SPEF  ) { tmp.B.SPEF   = 1; } if(sr.B.DPEF  ) { tmp.B.DPEF   = 1; } break;
	}
	*(volatile uint16_t *) &dspi[dspi_id]->SR = *(volatile uint16_t *) &tmp;
}

unsigned int dspi_get_tx_and_rx_status(unsigned int dspi_id)
{
	return dspi[dspi_id]->SR.B.TXRXS;
}

void dspi_clear_tx_and_rx_status(unsigned int dspi_id)
{
	DSPI_SR_tag sr = dspi[dspi_id]->SR;
	DSPI_SR_tag tmp;
	tmp.R = 0;
	tmp.B.BSYF = sr.B.BSYF;
	tmp.B.TXCTR = sr.B.TXCTR;
	tmp.B.TXNXTPTR = sr.B.TXNXTPTR;
	tmp.B.RXCTR = sr.B.RXCTR;
	tmp.B.POPNXTPTR = sr.B.POPNXTPTR;
	tmp.B.TXRXS = 1;
	dspi[dspi_id]->SR = tmp;
}

unsigned int dspi_get_busy_flag(unsigned int dspi_id)
{
	return dspi[dspi_id]->SR.B.BSYF;
}

unsigned int dspi_get_tx_fifo_counter(unsigned int dspi_id)
{
	return dspi[dspi_id]->SR.B.TXCTR;
}

unsigned int dspi_get_transmit_next_pointer(unsigned int dspi_id)
{
	return dspi[dspi_id]->SR.B.TXNXTPTR;
}

unsigned int dspi_get_rx_fifo_counter(unsigned int dspi_id)
{
	return dspi[dspi_id]->SR.B.RXCTR;
}

unsigned int dspi_get_pop_next_pointer(unsigned int dspi_id)
{
	return dspi[dspi_id]->SR.B.POPNXTPTR;
}

void dspi_enable_request(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	DSPI_RSER_tag rser = dspi[dspi_id]->RSER;
	switch(dspi_req)
	{
		case DSPI_REQ_EOQF           : rser.B.EOQF_RE   = 1; break;
		case DSPI_REQ_TFFF           : rser.B.TFFF_RE   = 1; break;
		case DSPI_REQ_CMDFFF         : rser.B.CMDFFF_RE = 1; break;
		case DSPI_REQ_TFIWF          : rser.B.TFIWF_RE  = 1; break;
		case DSPI_REQ_TCF            : rser.B.TCF_RE    = 1; break;
		case DSPI_REQ_CMDTCF         : rser.B.CMDTCF_RE = 1; break;
		case DSPI_REQ_SPITCF         : rser.B.SPITCF_RE = 1; break;
		case DSPI_REQ_DSITCF         : rser.B.DSITCF_RE = 1; break;
		case DSPI_REQ_TFUF           : rser.B.TFUF_RE   = 1; break;
		case DSPI_REQ_RFDF           : rser.B.RFDF_RE   = 1; break;
		case DSPI_REQ_RFOF           : rser.B.RFOF_RE   = 1; break;
		case DSPI_REQ_SPEF           : rser.B.SPEF_RE   = 1; break;
		case DSPI_REQ_DPEF           : rser.B.DPEF_RE   = 1; break;
		case DSPI_REQ_DDIF           : rser.B.DDIF_RE   = 1; break;
		case DSPI_REQ_TFUF_RFOF_TFIWF: rser.B.TFUF_RE = 1; rser.B.RFOF_RE = 1; rser.B.TFIWF_RE = 1; break;
		case DSPI_REQ_SPITCF_CMDTCF  : rser.B.SPITCF_RE = 1; rser.B.CMDTCF_RE = 1; break;
		case DSPI_REQ_DSITCF_CMDFFF  : rser.B.DSITCF_RE = 1; rser.B.CMDFFF_RE = 1; break;
		case DSPI_REQ_SPEF_DPEF      : rser.B.SPEF_RE = 1; rser.B.DPEF_RE = 1; break;
	}
	dspi[dspi_id]->RSER = rser;
}

void dspi_disable_request(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	DSPI_RSER_tag rser = dspi[dspi_id]->RSER;
	switch(dspi_req)
	{
		case DSPI_REQ_EOQF           : rser.B.EOQF_RE   = 0; break;
		case DSPI_REQ_TFFF           : rser.B.TFFF_RE   = 0; break;
		case DSPI_REQ_CMDFFF         : rser.B.CMDFFF_RE = 0; break;
		case DSPI_REQ_TFIWF          : rser.B.TFIWF_RE  = 0; break;
		case DSPI_REQ_TCF            : rser.B.TCF_RE    = 0; break;
		case DSPI_REQ_CMDTCF         : rser.B.CMDTCF_RE = 0; break;
		case DSPI_REQ_SPITCF         : rser.B.SPITCF_RE = 0; break;
		case DSPI_REQ_DSITCF         : rser.B.DSITCF_RE = 0; break;
		case DSPI_REQ_TFUF           : rser.B.TFUF_RE   = 0; break;
		case DSPI_REQ_RFDF           : rser.B.RFDF_RE   = 0; break;
		case DSPI_REQ_RFOF           : rser.B.RFOF_RE   = 0; break;
		case DSPI_REQ_SPEF           : rser.B.SPEF_RE   = 0; break;
		case DSPI_REQ_DPEF           : rser.B.DPEF_RE   = 0; break;
		case DSPI_REQ_DDIF           : rser.B.DDIF_RE   = 0; break;
		case DSPI_REQ_TFUF_RFOF_TFIWF: rser.B.TFUF_RE = 0; rser.B.RFOF_RE = 0; rser.B.TFIWF_RE = 0; break;
		case DSPI_REQ_SPITCF_CMDTCF  : rser.B.SPITCF_RE = 0; rser.B.CMDTCF_RE = 0; break;
		case DSPI_REQ_DSITCF_CMDFFF  : rser.B.DSITCF_RE = 0; rser.B.CMDFFF_RE = 0; break;
		case DSPI_REQ_SPEF_DPEF      : rser.B.SPEF_RE = 0; rser.B.DPEF_RE = 0; break;
	}
	dspi[dspi_id]->RSER = rser;
}

enum DSPI_REQ_DIR dspi_get_request_direction(unsigned int dspi_id, unsigned int dspi_req)
{
	DSPI_RSER_tag rser = dspi[dspi_id]->RSER;
	switch(dspi_req)
	{
		case DSPI_REQ_TFFF         : return rser.B.TFFF_DIRS;
		case DSPI_REQ_RFDF         : return rser.B.RFDF_DIRS;
		case DSPI_REQ_CMDFFF       :
		case DSPI_REQ_DSITCF_CMDFFF: return rser.B.CMDFFF_DIRS; 
		case DSPI_REQ_DDIF         : return rser.B.DDIF_DIRS;
		default: return DSPI_REQ_DIR_INT;
	}
}

void dspi_set_request_direction(unsigned int dspi_id, enum DSPI_REQ dspi_req, enum DSPI_REQ_DIR dspi_req_dir)
{
	DSPI_RSER_tag rser = dspi[dspi_id]->RSER;
	switch(dspi_req)
	{
		case DSPI_REQ_TFFF         : rser.B.TFFF_DIRS   = dspi_req_dir; break;
		case DSPI_REQ_RFDF         : rser.B.RFDF_DIRS   = dspi_req_dir; break;
		case DSPI_REQ_CMDFFF       :
		case DSPI_REQ_DSITCF_CMDFFF: rser.B.CMDFFF_DIRS = dspi_req_dir; break; 
		case DSPI_REQ_DDIF         : rser.B.DDIF_DIRS   = dspi_req_dir; break;
		default: break;
	}
	dspi[dspi_id]->RSER = rser;
}

void dspi_master_spi_tx_fifo_push(unsigned int dspi_id, struct DSPI_MASTER_SPI_COMMAND *dspi_master_spi_cmd, uint16_t data)
{
	if(dspi_get_mode(dspi_id) != DSPI_MASTER_MODE) return;
	
	DSPI_PUSHR_tag pushr;
	pushr.PUSHR.B.CONT = dspi_master_spi_cmd->cont;
	pushr.PUSHR.B.CTAS = dspi_master_spi_cmd->ctas;
	pushr.PUSHR.B.EOQ = dspi_master_spi_cmd->eoq;
	pushr.PUSHR.B.CTCNT = dspi_master_spi_cmd->ctcnt;
	pushr.PUSHR.B.PE_MASC = dspi_master_spi_cmd->pe_masc;
	pushr.PUSHR.B.PP_MCSC = dspi_master_spi_cmd->pp_mcsc;
	pushr.PUSHR.B.PCS = dspi_master_spi_cmd->pcs;
	pushr.PUSHR.B.TXDATA = data;
	dspi[dspi_id]->PUSHR = pushr;
}

void dspi_master_spi_tx_fifo_push_command(unsigned int dspi_id, struct DSPI_MASTER_SPI_COMMAND *dspi_master_spi_cmd)
{
	if((dspi_get_mode(dspi_id) != DSPI_MASTER_MODE) && (dspi_get_spi_mode(dspi_id) != DSPI_EXTENDED_SPI_MODE)) return;
	
	DSPI_PUSHR_tag pushr;
	pushr.PUSHR.B.CONT = dspi_master_spi_cmd->cont;
	pushr.PUSHR.B.CTAS = dspi_master_spi_cmd->ctas;
	pushr.PUSHR.B.EOQ = dspi_master_spi_cmd->eoq;
	pushr.PUSHR.B.CTCNT = dspi_master_spi_cmd->ctcnt;
	pushr.PUSHR.B.PE_MASC = dspi_master_spi_cmd->pe_masc;
	pushr.PUSHR.B.PP_MCSC = dspi_master_spi_cmd->pp_mcsc;
	pushr.PUSHR.B.PCS = dspi_master_spi_cmd->pcs;
	*(volatile uint16_t *) &dspi[dspi_id]->PUSHR = *(volatile uint16_t *) &pushr;
}

void dspi_spi_tx_fifo_push_data_byte(unsigned int dspi_id, uint8_t data)
{
	if((dspi_get_mode(dspi_id) == DSPI_MASTER_MODE) && dspi_get_spi_mode(dspi_id) != DSPI_EXTENDED_SPI_MODE) return;
	
	*((volatile uint8_t *) &dspi[dspi_id]->PUSHR + 3) = data;
}

void dspi_spi_tx_fifo_push_data_half_word(unsigned int dspi_id, uint16_t data)
{
	if((dspi_get_mode(dspi_id) == DSPI_MASTER_MODE) && dspi_get_spi_mode(dspi_id) != DSPI_EXTENDED_SPI_MODE) return;
	
	*((volatile uint16_t *) &dspi[dspi_id]->PUSHR + 1) = data;
}

void dspi_slave_spi_tx_fifo_push_byte(unsigned int dspi_id, uint8_t data)
{
	if(dspi_get_mode(dspi_id) != DSPI_MASTER_MODE) return;
	
	*((volatile uint8_t *) &dspi[dspi_id]->PUSHR + 3) = data;
}

void dspi_slave_spi_tx_fifo_push_half_word(unsigned int dspi_id, uint16_t data)
{
	if(dspi_get_mode(dspi_id) != DSPI_MASTER_MODE) return;
	
	*((volatile uint16_t *) &dspi[dspi_id]->PUSHR + 1) = data;
}

uint8_t dspi_spi_rx_fifo_pop_byte(unsigned int dspi_id)
{
	return *((volatile uint16_t *) &dspi[dspi_id]->POPR + 3);
}

uint16_t dspi_spi_rx_fifo_pop_half_word(unsigned int dspi_id)
{
	return *((volatile uint16_t *) &dspi[dspi_id]->POPR + 1);
}

uint32_t dspi_spi_rx_fifo_pop_word(unsigned int dspi_id)
{
	return dspi[dspi_id]->POPR.R;
}

uint32_t dspi_tx_fifo_peek(unsigned int dspi_id, unsigned int txptr)
{
	return dspi[dspi_id]->TXFR[txptr].R;
}

uint32_t dspi_rx_fifo_peek(unsigned int dspi_id, unsigned int rxptr)
{
	return dspi[dspi_id]->RXFR[rxptr].R;
}

enum DSPI_TSB_MODE dspi_get_dsi_timed_serial_bus_mode(unsigned int dspi_id)
{
	return (enum DSPI_TSB_MODE) dspi[dspi_id]->DSICR0.B.ITSB;
}

void dspi_set_dsi_timed_serial_bus_mode(unsigned int dspi_id, enum DSPI_TSB_MODE dspi_tsb_mode)
{
	dspi[dspi_id]->DSICR0.B.ITSB = dspi_tsb_mode;
}

void dspi_enable_dsi_timed_serial_bus_mode(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.TSBC = 1;
}

void dspi_disable_dsi_timed_serial_bus_mode(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.TSBC = 0;
}

enum DSPI_DSI_SOURCE dspi_get_dsi_source(unsigned int dspi_id)
{
	return (enum DSPI_DSI_SOURCE) dspi[dspi_id]->DSICR0.B.TXSS;
}

void dspi_set_dsi_source(unsigned int dspi_id, enum DSPI_DSI_SOURCE dspi_dsi_source)
{
	dspi[dspi_id]->DSICR0.B.TXSS = dspi_dsi_source;
}

enum DSPI_DSI_TRIGGER_POLARITY dspi_get_dsi_trigger_polarity(unsigned int dspi_id)
{
	return (enum DSPI_DSI_TRIGGER_POLARITY) dspi[dspi_id]->DSICR0.B.TPOL;
}

void dspi_set_dsi_trigger_polarity(unsigned int dspi_id, enum DSPI_DSI_TRIGGER_POLARITY dspi_dsi_trigger_polarity)
{
	dspi[dspi_id]->DSICR0.B.TPOL = dspi_dsi_trigger_polarity;
}

unsigned int dspi_is_dsi_continuous_peripheral_chip_select_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->DSICR0.B.DCONT;
}

void dspi_enable_dsi_continuous_peripheral_chip_select(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.DCONT = 1;
}

void dspi_disable_dsi_continuous_peripheral_chip_select(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.DCONT = 0;
}

unsigned int dspi_get_dsi_clock_and_transfer_attribute_select(unsigned int dspi_id)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
			return 1;
		
		case DSPI_MASTER_MODE:
			return dspi[dspi_id]->DSICR0.B.DSICTAS;
	}
	
	return 0;
}

void dspi_master_set_dsi_clock_and_transfer_attribute_select(unsigned int dspi_id, unsigned int dsictas)
{
	if(dspi_get_mode(dspi_id) != DSPI_MASTER_MODE) return;
	
	dspi[dspi_id]->DSICR0.B.DSICTAS = dsictas;
}

unsigned int dspi_is_dsi_data_match_stop_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->DSICR0.B.DMS;
}

void dspi_enable_dsi_data_match_stop(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.DMS = 1;
}

void dspi_disable_dsi_data_match_stop(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.DMS = 0;
}

enum DSPI_PARITY_ERROR_MODE dspi_get_dsi_parity_error_mode(unsigned int dspi_id)
{
	return (enum DSPI_PARITY_ERROR_MODE) dspi[dspi_id]->DSICR0.B.PES;
}

void dspi_set_dsi_parity_error_mode(unsigned int dspi_id, enum DSPI_PARITY_ERROR_MODE dspi_dsi_parity_error_mode)
{
	dspi[dspi_id]->DSICR0.B.PES = dspi_dsi_parity_error_mode;
}

unsigned int dspi_is_dsi_parity_check_enabled(unsigned int dspi_id)
{
	return dspi[dspi_id]->DSICR0.B.PE;
}

void dspi_enable_dsi_parity_check(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.PE = 1;
}

void dspi_disable_dsi_parity_check(unsigned int dspi_id)
{
	dspi[dspi_id]->DSICR0.B.PE = 0;
}

enum DSPI_PARITY_POLARITY dspi_get_dsi_parity_polarity(unsigned int dspi_id)
{
	return (enum DSPI_PARITY_POLARITY) dspi[dspi_id]->DSICR0.B.PP;
}

void dspi_set_dsi_parity_polarity(unsigned int dspi_id, enum DSPI_PARITY_POLARITY dspi_dsi_parity_polarity)
{
	dspi[dspi_id]->DSICR0.B.PP = dspi_dsi_parity_polarity;
}

uint8_t dspi_master_get_dsi_peripheral_chip_select(unsigned int dspi_id)
{
	return dspi[dspi_id]->DSICR0.B.DPCSx;
}

void dspi_master_set_dsi_peripheral_chip_select(unsigned int dspi_id, uint8_t dpcs)
{
	dspi[dspi_id]->DSICR0.B.DPCSx = dpcs;
}

/**** 
 * 
 * 
 * 
 ******/

unsigned int dspi_get_spi_data_transfer_count_preload(unsigned int dspi_id, unsigned int ctas)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
			return dspi[dspi_id]->CTARE[0].B.DTCP;
			
		case DSPI_MASTER_MODE:
			return dspi[dspi_id]->CTARE[ctas].B.DTCP;
	}
	
	return 0;
}

void dspi_set_spi_data_transfer_count_preload(unsigned int dspi_id, unsigned int ctas, unsigned int dtcp)
{
	switch(dspi_get_mode(dspi_id))
	{
		case DSPI_SLAVE_MODE:
			break;
			
		case DSPI_MASTER_MODE:
			dspi[dspi_id]->CTARE[ctas].B.DTCP = dtcp;
	}
}

unsigned int dspi_get_command_fifo_counter(unsigned int dspi_id)
{
	return dspi[dspi_id]->SREX.B.CMDCTR;
}

unsigned int dspi_get_command_next_pointer(unsigned int dspi_id)
{
	return dspi[dspi_id]->SREX.B.CMDNXTPTR;
}

dspi_int_handler_t dspi_set_interrupt_handler(unsigned int dspi_id, enum DSPI_REQ dspi_req, dspi_int_handler_t dspi_int_handler)
{
	dspi_int_handler_t old_dspi_int_handler = dspi_int_handlers[dspi_id][dspi_req];
	dspi_int_handlers[dspi_id][dspi_req] = dspi_int_handler;
	return old_dspi_int_handler;
}

void dspi_set_irq_priority(unsigned int dspi_id, enum DSPI_REQ dspi_req, unsigned int priority)
{
	intc_set_irq_priority(dspi_get_irq_vector(dspi_id, dspi_req), priority);
}

void dspi_select_irq_for_processor(unsigned int dspi_id, enum DSPI_REQ dspi_req, unsigned int prc_num)
{
	intc_select_irq_for_processor(dspi_get_irq_vector(dspi_id, dspi_req), prc_num);
}

void dspi_deselect_irq_for_processor(unsigned int dspi_id, enum DSPI_REQ dspi_req, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(dspi_get_irq_vector(dspi_id, dspi_req), prc_num);
}

static void __dspi_0_int_eoqf__()
{
	dspi_int_handlers[0][DSPI_REQ_EOQF](0, DSPI_REQ_EOQF);
}

static void __dspi_0_int_tfff__()
{
	dspi_int_handlers[0][DSPI_REQ_TFFF](0, DSPI_REQ_TFFF);
}

static void __dspi_0_int_cmdfff__()
{
	dspi_int_handlers[0][DSPI_REQ_CMDFFF](0, DSPI_REQ_CMDFFF);
}

static void __dspi_0_int_tcf__()
{
	dspi_int_handlers[0][DSPI_REQ_TCF](0, DSPI_REQ_TCF);
}

static void __dspi_0_int_cmdtcf__()
{
	dspi_int_handlers[0][DSPI_REQ_CMDTCF](0, DSPI_REQ_CMDTCF);
}

static void __dspi_0_int_rfdf__()
{
	dspi_int_handlers[0][DSPI_REQ_RFDF](0, DSPI_REQ_RFDF);
}

static void __dspi_0_int_spef__()
{
	dspi_int_handlers[0][DSPI_REQ_SPEF](0, DSPI_REQ_SPEF);
}

static void __dspi_0_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[0][DSPI_REQ_TFUF_RFOF_TFIWF](0, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_1_int_eoqf__()
{
	dspi_int_handlers[1][DSPI_REQ_EOQF](1, DSPI_REQ_EOQF);
}

static void __dspi_1_int_tfff__()
{
	dspi_int_handlers[1][DSPI_REQ_TFFF](1, DSPI_REQ_TFFF);
}

static void __dspi_1_int_cmdfff__()
{
	dspi_int_handlers[1][DSPI_REQ_CMDFFF](1, DSPI_REQ_CMDFFF);
}

static void __dspi_1_int_tcf__()
{
	dspi_int_handlers[1][DSPI_REQ_TCF](1, DSPI_REQ_TCF);
}

static void __dspi_1_int_cmdtcf__()
{
	dspi_int_handlers[1][DSPI_REQ_CMDTCF](1, DSPI_REQ_CMDTCF);
}

static void __dspi_1_int_rfdf__()
{
	dspi_int_handlers[1][DSPI_REQ_RFDF](1, DSPI_REQ_RFDF);
}

static void __dspi_1_int_spef__()
{
	dspi_int_handlers[1][DSPI_REQ_SPEF](1, DSPI_REQ_SPEF);
}

static void __dspi_1_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[1][DSPI_REQ_TFUF_RFOF_TFIWF](1, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_2_int_eoqf__()
{
	dspi_int_handlers[2][DSPI_REQ_EOQF](2, DSPI_REQ_EOQF);
}

static void __dspi_2_int_tfff__()
{
	dspi_int_handlers[2][DSPI_REQ_TFFF](2, DSPI_REQ_TFFF);
}

static void __dspi_2_int_cmdfff__()
{
	dspi_int_handlers[2][DSPI_REQ_CMDFFF](2, DSPI_REQ_CMDFFF);
}

static void __dspi_2_int_tcf__()
{
	dspi_int_handlers[2][DSPI_REQ_TCF](2, DSPI_REQ_TCF);
}

static void __dspi_2_int_cmdtcf__()
{
	dspi_int_handlers[2][DSPI_REQ_CMDTCF](2, DSPI_REQ_CMDTCF);
}

static void __dspi_2_int_rfdf__()
{
	dspi_int_handlers[2][DSPI_REQ_RFDF](2, DSPI_REQ_RFDF);
}

static void __dspi_2_int_spef__()
{
	dspi_int_handlers[2][DSPI_REQ_SPEF](2, DSPI_REQ_SPEF);
}

static void __dspi_2_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[2][DSPI_REQ_TFUF_RFOF_TFIWF](2, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_3_int_eoqf__()
{
	dspi_int_handlers[3][DSPI_REQ_EOQF](3, DSPI_REQ_EOQF);
}

static void __dspi_3_int_tfff__()
{
	dspi_int_handlers[3][DSPI_REQ_TFFF](3, DSPI_REQ_TFFF);
}

static void __dspi_3_int_cmdfff__()
{
	dspi_int_handlers[3][DSPI_REQ_CMDFFF](3, DSPI_REQ_CMDFFF);
}

static void __dspi_3_int_tcf__()
{
	dspi_int_handlers[3][DSPI_REQ_TCF](3, DSPI_REQ_TCF);
}

static void __dspi_3_int_cmdtcf__()
{
	dspi_int_handlers[3][DSPI_REQ_CMDTCF](3, DSPI_REQ_CMDTCF);
}

static void __dspi_3_int_rfdf__()
{
	dspi_int_handlers[3][DSPI_REQ_RFDF](3, DSPI_REQ_RFDF);
}

static void __dspi_3_int_spef__()
{
	dspi_int_handlers[3][DSPI_REQ_SPEF](3, DSPI_REQ_SPEF);
}

static void __dspi_3_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[3][DSPI_REQ_TFUF_RFOF_TFIWF](3, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_4_int_eoqf__()
{
	dspi_int_handlers[4][DSPI_REQ_EOQF](4, DSPI_REQ_EOQF);
}

static void __dspi_4_int_tfff__()
{
	dspi_int_handlers[4][DSPI_REQ_TFFF](4, DSPI_REQ_TFFF);
}

static void __dspi_4_int_tcf__()
{
	dspi_int_handlers[4][DSPI_REQ_TCF](4, DSPI_REQ_TCF);
}

static void __dspi_4_int_rfdf__()
{
	dspi_int_handlers[4][DSPI_REQ_RFDF](4, DSPI_REQ_RFDF);
}

static void __dspi_4_int_ddif__()
{
	dspi_int_handlers[4][DSPI_REQ_DDIF](4, DSPI_REQ_DDIF);
}

static void __dspi_4_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[4][DSPI_REQ_TFUF_RFOF_TFIWF](4, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_4_int_spitcf_cmdtcf__()
{
	dspi_int_handlers[4][DSPI_REQ_SPITCF_CMDTCF](4, DSPI_REQ_SPITCF_CMDTCF);
}

static void __dspi_4_int_dsitcf_cmdfff__()
{
	dspi_int_handlers[4][DSPI_REQ_DSITCF_CMDFFF](4, DSPI_REQ_DSITCF_CMDFFF);
}

static void __dspi_4_int_spef_dpef__()
{
	dspi_int_handlers[4][DSPI_REQ_SPEF_DPEF](4, DSPI_REQ_SPEF_DPEF);
}

static void __dspi_5_int_eoqf__()
{
	dspi_int_handlers[5][DSPI_REQ_EOQF](5, DSPI_REQ_EOQF);
}

static void __dspi_5_int_tfff__()
{
	dspi_int_handlers[5][DSPI_REQ_TFFF](5, DSPI_REQ_TFFF);
}

static void __dspi_5_int_tcf__()
{
	dspi_int_handlers[5][DSPI_REQ_TCF](5, DSPI_REQ_TCF);
}

static void __dspi_5_int_rfdf__()
{
	dspi_int_handlers[5][DSPI_REQ_RFDF](5, DSPI_REQ_RFDF);
}

static void __dspi_5_int_ddif__()
{
	dspi_int_handlers[5][DSPI_REQ_DDIF](5, DSPI_REQ_DDIF);
}

static void __dspi_5_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[5][DSPI_REQ_TFUF_RFOF_TFIWF](5, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_5_int_spitcf_cmdtcf__()
{
	dspi_int_handlers[5][DSPI_REQ_SPITCF_CMDTCF](5, DSPI_REQ_SPITCF_CMDTCF);
}

static void __dspi_5_int_dsitcf_cmdfff__()
{
	dspi_int_handlers[5][DSPI_REQ_DSITCF_CMDFFF](5, DSPI_REQ_DSITCF_CMDFFF);
}

static void __dspi_5_int_spef_dpef__()
{
	dspi_int_handlers[5][DSPI_REQ_SPEF_DPEF](5, DSPI_REQ_SPEF_DPEF);
}

static void __dspi_6_int_eoqf__()
{
	dspi_int_handlers[6][DSPI_REQ_EOQF](6, DSPI_REQ_EOQF);
}

static void __dspi_6_int_tfff__()
{
	dspi_int_handlers[6][DSPI_REQ_TFFF](6, DSPI_REQ_TFFF);
}

static void __dspi_6_int_tcf__()
{
	dspi_int_handlers[6][DSPI_REQ_TCF](6, DSPI_REQ_TCF);
}

static void __dspi_6_int_rfdf__()
{
	dspi_int_handlers[6][DSPI_REQ_RFDF](6, DSPI_REQ_RFDF);
}

static void __dspi_6_int_ddif__()
{
	dspi_int_handlers[6][DSPI_REQ_DDIF](6, DSPI_REQ_DDIF);
}

static void __dspi_6_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[6][DSPI_REQ_TFUF_RFOF_TFIWF](6, DSPI_REQ_TFUF_RFOF_TFIWF);
}

static void __dspi_6_int_spitcf_cmdtcf__()
{
	dspi_int_handlers[6][DSPI_REQ_SPITCF_CMDTCF](6, DSPI_REQ_SPITCF_CMDTCF);
}

static void __dspi_6_int_dsitcf_cmdfff__()
{
	dspi_int_handlers[6][DSPI_REQ_DSITCF_CMDFFF](6, DSPI_REQ_DSITCF_CMDFFF);
}

static void __dspi_6_int_spef_dpef__()
{
	dspi_int_handlers[6][DSPI_REQ_SPEF_DPEF](6, DSPI_REQ_SPEF_DPEF);
}

static void __dspi_12_int_eoqf__()
{
	dspi_int_handlers[12][DSPI_REQ_EOQF](12, DSPI_REQ_EOQF);
}

static void __dspi_12_int_tfff__()
{
	dspi_int_handlers[12][DSPI_REQ_TFFF](12, DSPI_REQ_TFFF);
}

static void __dspi_12_int_cmdfff__()
{
	dspi_int_handlers[12][DSPI_REQ_CMDFFF](12, DSPI_REQ_CMDFFF);
}

static void __dspi_12_int_tcf__()
{
	dspi_int_handlers[12][DSPI_REQ_TCF](12, DSPI_REQ_TCF);
}

static void __dspi_12_int_cmdtcf__()
{
	dspi_int_handlers[12][DSPI_REQ_CMDTCF](12, DSPI_REQ_CMDTCF);
}

static void __dspi_12_int_rfdf__()
{
	dspi_int_handlers[12][DSPI_REQ_RFDF](12, DSPI_REQ_RFDF);
}

static void __dspi_12_int_spef__()
{
	dspi_int_handlers[12][DSPI_REQ_SPEF](12, DSPI_REQ_SPEF);
}

static void __dspi_12_int_tfuf_rfof_tfiwf__()
{
	dspi_int_handlers[12][DSPI_REQ_TFUF_RFOF_TFIWF](12, DSPI_REQ_TFUF_RFOF_TFIWF);
}
