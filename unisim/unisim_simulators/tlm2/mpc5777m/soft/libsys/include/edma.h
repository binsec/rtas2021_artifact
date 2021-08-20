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

#ifndef __EDMA_H__
#define __EDMA_H__

#include "typedefs.h"

typedef void (*edma_int_handler_t)(unsigned int, unsigned int);

enum eDMA_Arbitration
{
	eDMA_FIXED_PRIORITY_ARBITRATION = 0,
	eDMA_ROUND_ROBIN_ARBITRATION    = 1
};

enum eDMA_PrivilegeAccessLevel
{
	eDMA_USER_PROTECTION_LEVEL      = 0,
	eDMA_PRIVILEGE_PROTECTION_LEVEL = 1,
};

enum eDMA_Throttle
{
	eDMA_NO_THROTTLE             = 0,
	eDMA_THROTTLE_4_CYCLES_STALL = 2,
	eDMA_THROTTLE_8_CYCLES_STALL = 3,
};

enum eDMA_INT
{
	eDMA_INT_CHAN     = 0,
	eDMA_INT_CHAN_ERR = 1
};

void edma_drv_init();
void edma_init(unsigned int edma_id);

unsigned int edma_channel_group_priority(unsigned int edma_id, unsigned int grp);
unsigned int edma_minor_loop_mapping(unsigned int edma_id);
unsigned int edma_continuous_link_mode(unsigned int edma_id);
unsigned int edma_dma_operations_halted(unsigned int edma_id);
unsigned int edma_halt_on_error(unsigned int edma_id);
enum eDMA_Arbitration edma_group_arbitration(unsigned int edma_id); 
enum eDMA_Arbitration edma_channel_arbitration(unsigned int edma_id); 
unsigned int edma_debug_mode(unsigned int edma_id);

void edma_cancel_transfer(unsigned int edma_id);
void edma_error_cancel_transfer(unsigned int edma_id);
void edma_set_channel_group_priority(unsigned int edma_id, unsigned int grp, unsigned int prio);
void edma_enable_minor_loop_mapping(unsigned int edma_id);
void edma_disable_minor_loop_mapping(unsigned int edma_id);
void edma_enable_continuous_link_mode(unsigned int edma_id);
void edma_disable_continuous_link_mode(unsigned int edma_id);
void edma_halt_dma_operations(unsigned int edma_id);
void edma_resume_dma_operations(unsigned int edma_id);
void edma_enable_halt_on_error(unsigned int edma_id);
void edma_disable_halt_on_error(unsigned int edma_id);
void edma_select_group_arbitration(unsigned int edma_id, enum eDMA_Arbitration arb);
void edma_select_channel_arbitration(unsigned int edma_id, enum eDMA_Arbitration arb);
void edma_enable_debug_mode(unsigned int edma_id);
void edma_disable_debug_mode(unsigned int edma_id);

unsigned int edma_valid(unsigned int edma_id);
unsigned int edma_uncorrectable_ecc_error(unsigned int edma_id);
unsigned int edma_transfer_cancelled(unsigned int edma_id);
unsigned int edma_group_priority_error(unsigned int edma_id);
unsigned int edma_channel_priority_error(unsigned int edma_id);
unsigned int edma_get_error_cancelled_channel(unsigned int edma_id);
unsigned int edma_source_address_error(unsigned int edma_id);
unsigned int edma_source_offset_error(unsigned int edma_id);
unsigned int edma_destination_address_error(unsigned int edma_id);
unsigned int edma_destination_offset_error(unsigned int edma_id);
unsigned int edma_nbytes_citer_config_error(unsigned int edma_id);
unsigned int edma_scatter_gather_config_error(unsigned int edma_id);
unsigned int edma_source_bus_error(unsigned int edma_id);
unsigned int edma_destination_bus_error(unsigned int edma_id);

unsigned int edma_request_enabled(unsigned int edma_id, unsigned int chan);
unsigned int edma_error_interrupt_enabled(unsigned int edma_id, unsigned int chan);

void edma_enable_all_requests(unsigned int edma_id);
void edma_disable_all_requests(unsigned int edma_id);
void edma_enable_request(unsigned int edma_id, unsigned int chan);
void edma_disable_request(unsigned int edma_id, unsigned int chan);
void edma_enable_all_error_interrupts(unsigned int edma_id);
void edma_disable_all_error_interrupts(unsigned int edma_id);
void edma_enable_error_interrupt(unsigned int edma_id, unsigned int chan);
void edma_disable_error_interrupt(unsigned int edma_id, unsigned int chan);
void edma_clear_all_interrupt_requests(unsigned int edma_id);
void edma_clear_interrupt_request(unsigned int edma_id, unsigned int chan);
void edma_clear_all_errors(unsigned int edma_id);
void edma_clear_error(unsigned int edma_id, unsigned int chan);
void edma_set_all_start_bits(unsigned int edma_id);
void edma_set_start_bit(unsigned int edma_id, unsigned int chan);
void edma_clear_all_done_bits(unsigned int edma_id);
void edma_clear_done_bit(unsigned int edma_id, unsigned int chan);

unsigned int edma_interrupt_request(unsigned int edma_id, unsigned int chan);
unsigned int edma_error(unsigned int edma_id, unsigned int chan);
unsigned int edma_hardware_request_status(unsigned int edma_id, unsigned int chan);

void edma_enable_channel_preemption(unsigned int edma_id, unsigned int chan);
void edma_disable_channel_preemption(unsigned int edma_id, unsigned int chan);
void edma_enable_preempt_ability(unsigned int edma_id, unsigned int chan);
void edma_disable_preempt_ability(unsigned int edma_id, unsigned int chan);
unsigned int edma_get_channel_current_group_priority(unsigned int edma_id, unsigned int chan);
unsigned int edma_get_channel_arbitration_priority(unsigned int edma_id, unsigned int chan);
void edma_set_channel_arbitration_priority(unsigned int edma_id, unsigned int chan, unsigned int prio);

void edma_enable_master_id_replication(unsigned int edma_id, unsigned int chan);
void edma_disable_master_id_replication(unsigned int edma_id, unsigned int chan);
int edma_is_master_id_replication_enabled(unsigned int edma_id, unsigned int chan);
enum eDMA_PrivilegeAccessLevel edma_get_privilege_access_level(unsigned int edma_id, unsigned int chan);
unsigned int edma_get_master_id(unsigned int edma_id, unsigned int chan);

uint32_t edma_get_tcd_source_address(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_source_address(unsigned int edma_id, unsigned int chan, uint32_t saddr);
unsigned int edma_get_tcd_source_address_modulo(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_source_address_modulo(unsigned int edma_id, unsigned int chan, unsigned int smod);
unsigned int edma_get_tcd_source_data_transfer_size(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_source_data_transfer_size(unsigned int edma_id, unsigned int chan, unsigned int ssize);
unsigned int edma_get_tcd_destination_address_modulo(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_destination_address_modulo(unsigned int edma_id, unsigned int chan, unsigned int dmod);
unsigned int edma_get_tcd_destination_data_transfer_size(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_destination_data_transfer_size(unsigned int edma_id, unsigned int chan, unsigned int dsize);
int32_t edma_get_tcd_signed_source_address_offset(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_signed_source_address_offset(unsigned int edma_id, unsigned int chan, int32_t soff);

unsigned int edma_tcd_source_minor_loop_offset_enabled(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_destination_minor_loop_offset_enabled(unsigned int edma_id, unsigned int chan);
uint32_t edma_get_tcd_minor_byte_count(unsigned int edma_id, unsigned int chan);
uint32_t edma_get_tcd_minor_loop_offset(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_minor_byte_count(unsigned int edma_id, unsigned int chan, uint32_t nbytes);
void edma_set_tcd_source_minor_loop_offset(unsigned int edma_id, unsigned int chan, uint32_t mloff);
void edma_set_tcd_destination_minor_loop_offset(unsigned int edma_id, unsigned int chan, uint32_t mloff);

uint32_t edma_get_tcd_last_source_address_adjustment(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_last_source_address_adjustment(unsigned int edma_id, unsigned int chan, uint32_t slast);

uint32_t edma_get_tcd_destination_address(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_destination_address(unsigned int edma_id, unsigned int chan, uint32_t daddr);

unsigned int edma_tcd_channel_to_channel_linking_currently_enabled(unsigned int edma_id, unsigned int chan);
uint32_t edma_get_tcd_current_major_iteration_count(unsigned int edma_id, unsigned int chan);
unsigned int edma_get_tcd_minor_loop_link_channel_number(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_current_major_iteration_count(unsigned int edma_id, unsigned int chan, uint32_t citer);
void edma_set_tcd_minor_loop_link_channel_number(unsigned int edma_id, unsigned int chan, unsigned int linkch);

int32_t edma_get_tcd_signed_destination_address_offset(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_signed_destination_address_offset(unsigned int edma_id, unsigned int chan, int32_t doff);

uint32_t edma_get_tcd_last_destination_address_adjustment(unsigned int edma_id, unsigned int chan);
uint32_t edma_get_tcd_scatter_gather_address(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_last_destination_address_adjustment(unsigned int edma_id, unsigned int chan, uint32_t ldaa);
void edma_set_tcd_scatter_gather_address(unsigned int edma_id, unsigned int chan, uint32_t sga);

unsigned int edma_tcd_channel_to_channel_linking_enabled_at_beginning(unsigned int edma_id, unsigned int chan);
uint32_t edma_get_tcd_starting_major_iteration_count(unsigned int edma_id, unsigned int chan);
unsigned int edma_get_tcd_minor_loop_link_channel_number_at_beginning(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_starting_major_iteration_count(unsigned int edma_id, unsigned int chan, uint32_t biter);
void edma_set_tcd_minor_loop_link_channel_number_at_beginning(unsigned int edma_id, unsigned int chan, unsigned int linkch);

enum eDMA_Throttle edma_get_tcd_bandwidth_control(unsigned int edma_id, unsigned int chan);
unsigned int edma_get_tcd_major_loop_link_channel_number(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_channel_done(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_channel_active(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_major_loop_linking_enabled(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_scatter_gather_enabled(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_request_automatically_disabled_after_major_loop_complete(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_half_major_complete_interrupt_enabled(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_major_complete_interrupt_enabled(unsigned int edma_id, unsigned int chan);
unsigned int edma_tcd_channel_start_requested(unsigned int edma_id, unsigned int chan);

void edma_select_tcd_bandwidth_control(unsigned int edma_id, unsigned int chan, enum eDMA_Throttle bwc);
void edma_set_tcd_major_loop_link_channel_number(unsigned int edma_id, unsigned int chan, unsigned int majorlinkch);
void edma_set_tcd_channel_done(unsigned int edma_id, unsigned int chan);
void edma_clear_tcd_channel_done(unsigned int edma_id, unsigned int chan);
void edma_set_tcd_channel_active(unsigned int edma_id, unsigned int chan);
void edma_clear_tcd_channel_active(unsigned int edma_id, unsigned int chan);
void edma_enable_tcd_major_loop_linking(unsigned int edma_id, unsigned int chan);
void edma_disable_tcd_major_loop_linking(unsigned int edma_id, unsigned int chan);
void edma_enable_tcd_scatter_gather(unsigned int edma_id, unsigned int chan);
void edma_disable_tcd_scatter_gather(unsigned int edma_id, unsigned int chan);
void edma_tcd_disable_request_automatically_after_major_loop_complete(unsigned int edma_id, unsigned int chan);
void edma_tcd_do_not_disable_request_automatically_after_major_loop_complete(unsigned int edma_id, unsigned int chan);
void edma_enable_tcd_half_major_complete_interrupt(unsigned int edma_id, unsigned int chan);
void edma_disable_tcd_half_major_complete_interrupt(unsigned int edma_id, unsigned int chan);
void edma_enable_tcd_major_complete_interrupt(unsigned int edma_id, unsigned int chan);
void edma_disable_tcd_major_complete_interrupt(unsigned int edma_id, unsigned int chan);

edma_int_handler_t edma_set_interrupt_handler(unsigned int edma_id, unsigned int chan, edma_int_handler_t edma_int_handler);
void edma_set_irq_priority(unsigned int edma_id, unsigned int chan, unsigned int priority);
void edma_select_irq_for_processor(unsigned int edma_id, unsigned int chan, unsigned int prc_num);
void edma_deselect_irq_for_processor(unsigned int edma_id, unsigned int chan, unsigned int prc_num);

#endif
