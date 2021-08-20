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

#ifndef __M_CAN_H__
#define __M_CAN_H__

#include "typedefs.h"

enum M_CAN_TX_FIFO_QUEUE_MODE
{
	M_CAN_TX_FIFO_MODE  = 0,
	M_CAN_TX_QUEUE_MODE = 1
};

enum M_CAN_RX_FIFO_OPERATION_MODE
{
	M_CAN_RX_FIFO_BLOCKING_MODE    = 0,
	M_CAN_RX_FIFO_OVERWRITING_MODE = 1,
};

enum M_CAN_DATA_FIELD_SIZE
{
	M_CAN_8_BYTE_DATA_FIELD  = 0,
	M_CAN_12_BYTE_DATA_FIELD = 1,
	M_CAN_16_BYTE_DATA_FIELD = 2,
	M_CAN_20_BYTE_DATA_FIELD = 3,
	M_CAN_24_BYTE_DATA_FIELD = 4,
	M_CAN_32_BYTE_DATA_FIELD = 5,
	M_CAN_48_BYTE_DATA_FIELD = 6,
	M_CAN_64_BYTE_DATA_FIELD = 7
};

struct m_can_rx_buffer_fifo_element
{
	struct
	{
		uint32_t ESI : 1;
		uint32_t XTD : 1;
		uint32_t RTR : 1;
		uint32_t ID : 29;
	}
	R0;
	
	struct
	{
		uint32_t ANMF : 1;
		uint32_t FIDX : 7;
		uint32_t      : 2;
		uint32_t EDL  : 1;
		uint32_t BRS  : 1;
		uint32_t DLC  : 4;
		uint32_t RXTS : 16;
	}
	R1;
	
	uint32_t DATA[16];
};

struct m_can_tx_buffer_element
{
	struct
	{
		uint32_t     : 1;
		uint32_t XTD : 1;
		uint32_t RTR : 1;
		uint32_t ID  : 29;
	}
	T0;
	
	struct
	{
		uint32_t MM  : 8;
		uint32_t EFC : 1;
		uint32_t     : 3;
		uint32_t DLC : 4;
		uint32_t     : 16;
	}
	T1;
	
	uint32_t DATA[16];
};

struct m_can_tx_event_fifo_element
{
	struct
	{
		uint32_t ESI : 1;
		uint32_t XTD : 1;
		uint32_t RTR : 1;
		uint32_t ID  : 29;
	}
	E0;
	
	struct
	{
		uint32_t MM   : 8;
		uint32_t ET   : 2;
		uint32_t EDL  : 1;
		uint32_t BRS  : 1;
		uint32_t DLC  : 4;
		uint32_t TXTS : 16;
	}
	E1;
};

struct m_can_standard_message_id_filter_element
{
	struct
	{
		uint32_t SFT   : 2;
		uint32_t SFEC  : 3;
		uint32_t SFID1 : 11;
		uint32_t       : 5;
		uint32_t SFID2 : 11;
	}
	S0;
};

struct m_can_extended_message_id_filter_element
{
	struct
	{
		uint32_t EFEC  : 3;
		uint32_t EFID1 : 29;
	}
	F0;
	
	struct
	{
		uint32_t EFT   : 2;
		uint32_t       : 1;
		uint32_t EFID2 : 29;
	}
	F1;
};

enum M_CAN_INT
{
	M_CAN_INT_STE  = 31,
	M_CAN_INT_FOE  = 30,
	M_CAN_INT_ACKE = 29,
	M_CAN_INT_BE   = 28,
	M_CAN_INT_CRCE = 27,
	M_CAN_INT_WDI  = 26,
	M_CAN_INT_BO   = 25,
	M_CAN_INT_EW   = 24,
	M_CAN_INT_EP   = 23,
	M_CAN_INT_ELO  = 22,
	M_CAN_INT_BLEU = 21,
	M_CAN_INT_BEC  = 20,
	M_CAN_INT_DRX  = 19,
	M_CAN_INT_TOO  = 18,
	M_CAN_INT_MRAF = 17,
	M_CAN_INT_TSW  = 16,
	M_CAN_INT_TEFL = 15,
	M_CAN_INT_TEFF = 14,
	M_CAN_INT_TEFW = 13,
	M_CAN_INT_TEFN = 12,
	M_CAN_INT_TFE  = 11,
	M_CAN_INT_TCF  = 10,
	M_CAN_INT_TC   = 9,
	M_CAN_INT_HPM  = 8,
	M_CAN_INT_RF1L = 7,
	M_CAN_INT_RF1F = 6,
	M_CAN_INT_RF1W = 5,
	M_CAN_INT_RF1N = 4,
	M_CAN_INT_RF0L = 3,
	M_CAN_INT_RF0F = 2,
	M_CAN_INT_RF0W = 1,
	M_CAN_INT_RF0N = 0 
};

enum M_CAN_INT_LINE
{
	M_CAN_INT0 = 0,
	M_CAN_INT1 = 1
};

enum M_CAN_FILTER_TYPE 
{
	RANGE_FILTER    = 0, // Range Filter
	DUAL_ID_FILTER  = 1, // Dual ID Filter
	CLASSIC_FILTER  = 2, // Classic Filter
	RESERVED_FILTER = 3  // Reserved
};

enum FILTER_ELEMENT_CONFIGURATION
{
	FEC_DISABLE_FILTER_ELEMENT                                = 0, // Disable Filter Element
	FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES                  = 1, // Store in Rx FIFO #0 if filter matches
	FEC_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES                  = 2, // Store in Rx FIFO #1 if filter matches
	FEC_REJECT_ID_IF_FILTER_MATCHES                           = 3, // Reject ID if filter matches
	FEC_SET_PRIORITY_IF_FILTER_MATCHES                        = 4, // Set priority if filter matches
	FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES = 5, // Set priority and store in Rx FIFO #0 if filter matches
	FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES = 6, // Set priority and store in Rx FIFO #1 if filter matches
	FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE              = 7  // Store in Rx Buffer or as a Debug Message
};

typedef void (*m_can_int_handler_t)(unsigned int, enum M_CAN_INT_LINE m_can_int_line);

void m_can_drv_init();
void m_can_init(unsigned int m_can_id);

void m_can_enter_init_mode(unsigned int m_can_id);
void m_can_exit_init_mode(unsigned int m_can_id);

void m_can_enable_configuration_change(unsigned int m_can_id);
void m_can_disable_configuration_change(unsigned int m_can_id);

void m_can_set_transmit_pause(unsigned int m_can_id, unsigned int transmit_pause);
unsigned int m_can_get_transmit_pause(unsigned int m_can_id);
void m_can_enable_test_mode(unsigned int m_can_id);
void m_can_disable_test_mode(unsigned int m_can_id);
void m_can_enable_loopback(unsigned int m_can_id);
void m_can_disable_loopback(unsigned int m_can_id);
void m_can_enable_timestamp_counter(unsigned int m_can_id);
void m_can_disable_timestamp_counter(unsigned int m_can_id);

void m_can_set_baud_rate_prescaler(unsigned m_can_id, unsigned int brp);
void m_can_set_tseg1(unsigned m_can_id, unsigned int tseg1);
void m_can_set_tseg2(unsigned m_can_id, unsigned int tseg2);
void m_can_set_sjw(unsigned m_can_id, unsigned int sjw);

void m_can_set_num_dedicated_tx_buffers(unsigned int m_can_id, unsigned int ndtb);
void m_can_set_tx_fifo_queue_mode(unsigned int m_can_id, enum M_CAN_TX_FIFO_QUEUE_MODE tfqm);
void m_can_set_tx_fifo_queue_size(unsigned int m_can_id, unsigned int tfqs);
void m_can_set_tx_buffers_start_address(unsigned int m_can_id, uint32_t tbsa);
void m_can_set_tx_buffer_data_field_size(unsigned int m_can_id, enum M_CAN_DATA_FIELD_SIZE tbds);

unsigned int m_can_get_tx_fifo_queue_put_index(unsigned int m_can_id);
unsigned int m_can_get_tx_fifo_get_index(unsigned int m_can_id);
unsigned int m_can_get_tx_fifo_free_level(unsigned int m_can_id);
void m_can_tx_fifo_push(unsigned int m_can_id, struct m_can_tx_buffer_element *elem);

void m_can_add_tx_buffer_request(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
void m_can_add_tx_buffer_requests(unsigned int m_can_id, uint32_t mask);
void m_can_cancel_tx_buffer_request(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
void m_can_cancel_tx_buffer_requests(unsigned int m_can_id, uint32_t mask);
void m_can_enable_tx_buffer_transmission_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
void m_can_disable_tx_buffer_transmission_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
void m_can_enable_tx_buffer_cancellation_finished_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
void m_can_disable_tx_buffer_cancellation_finished_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
int m_can_is_tx_buffer_has_pending_request(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
int m_can_is_tx_buffer_transmission_occurred(unsigned int m_can_id, unsigned int tx_buffer_element_idx);
int m_can_is_tx_buffer_cancellation_finished(unsigned int m_can_id, unsigned int tx_buffer_element_idx);

void m_can_write_tx_buffer_element(unsigned int m_can_id, unsigned int tx_buffer_element_idx, struct m_can_tx_buffer_element *elem);

void m_can_set_tx_event_fifo_watermark(unsigned int m_can_id, unsigned int level);
void m_can_set_tx_event_fifo_size(unsigned int m_can_id, unsigned int efs);
void m_can_set_tx_event_fifo_start_address(unsigned int m_can_id, uint32_t efsa);
void m_can_acknowledge_tx_event_fifo(unsigned int m_can_id, unsigned int efai);
int m_can_has_tx_event_fifo_element_lost(unsigned int m_can_id);
int m_can_is_tx_event_fifo_full(unsigned int m_can_id);
unsigned int m_can_get_tx_event_fifo_put_index(unsigned int m_can_id);
unsigned int m_can_get_tx_event_fifo_get_index(unsigned int m_can_id);
unsigned int m_can_get_tx_event_fill_level(unsigned int m_can_id);
void m_can_read_tx_event_fifo_element(unsigned int m_can_id, struct m_can_tx_event_fifo_element *elem);

void m_can_set_rx_fifo_operation_mode(unsigned int m_can_id, unsigned int fifo_id, enum M_CAN_RX_FIFO_OPERATION_MODE fom);
void m_can_set_rx_fifo_watermark(unsigned int m_can_id, unsigned int fifo_id, unsigned int fwm);
void m_can_set_rx_fifo_size(unsigned int m_can_id, unsigned int fifo_id, unsigned int fs);
void m_can_set_rx_fifo_start_address(unsigned int m_can_id, unsigned int fifo_id, uint32_t fsa);
void m_can_acknowledge_rx_fifo(unsigned int m_can_id, unsigned int fifo_id, unsigned int fai);
int m_can_is_rx_fifo_full(unsigned int m_can_id, unsigned int fifo_id);
unsigned int m_can_get_rx_fifo_fill_level(unsigned int m_can_id, unsigned int fifo_id);
unsigned int m_can_get_rx_fifo_put_index(unsigned int m_can_id, unsigned int fifo_id);
unsigned int m_can_get_rx_fifo_get_index(unsigned int m_can_id, unsigned int fifo_id);
void m_can_set_rx_fifo_data_field_size(unsigned int m_can_id, unsigned int fifo_id, enum M_CAN_DATA_FIELD_SIZE fds);

void m_can_set_rx_buffer_start_address(unsigned int m_can_id, uint32_t rbsa);
void m_can_set_rx_buffer_data_field_size(unsigned int m_can_id, enum M_CAN_DATA_FIELD_SIZE rbds);

void m_can_read_rx_fifo(unsigned int m_can_id, unsigned int fifo_id, struct m_can_rx_buffer_fifo_element *elem);
void m_can_read_rx_buffer(unsigned int m_can_id, unsigned int rx_buffer_element_index, struct m_can_rx_buffer_fifo_element *elem);

void m_can_pop_rx_fifo(unsigned int m_can_id, unsigned int fifo_id, struct m_can_rx_buffer_fifo_element *elem);

void m_can_accept_non_matching_frame_standard(unsigned int m_can_id, unsigned int fifo_id);
void m_can_reject_non_matching_frame_standard(unsigned int m_can_id);
void m_can_accept_non_matching_frame_extended(unsigned int m_can_id, unsigned int fifo_id);
void m_can_reject_non_matching_frame_extended(unsigned int m_can_id);
void m_can_filter_remote_standard_frame(unsigned int m_can_id);
void m_can_reject_remote_standard_frame(unsigned int m_can_id);
void m_can_filter_remote_extended_frame(unsigned int m_can_id);
void m_can_reject_remote_extended_frame(unsigned int m_can_id);
void m_can_set_standard_filter_list_size(unsigned int m_can_id, unsigned int lss);
void m_can_set_extended_filter_list_size(unsigned int m_can_id, unsigned int lse);
void m_can_set_standard_filter_list_start_address(unsigned int m_can_id, uint32_t flsa);
void m_can_set_extended_filter_list_start_address(unsigned int m_can_id, uint32_t flsa);
void m_can_set_extended_id_mask(unsigned int m_can_id, uint32_t eidm);

void m_can_write_standard_filter(unsigned int m_can_id, unsigned int filter_element_index, struct m_can_standard_message_id_filter_element *elem);
void m_can_write_extended_filter(unsigned int m_can_id, unsigned int filter_element_index, struct m_can_extended_message_id_filter_element *elem);

uint32_t m_can_new_data1(unsigned int m_can_id);
uint32_t m_can_new_data2(unsigned int m_can_id);
int m_can_new_data(unsigned int m_can_id, unsigned int rx_buffer_fifo_element_index);
void m_can_clear_new_data(unsigned int m_can_id, unsigned int rx_buffer_fifo_element_index);

void m_can_enable_interrupt(unsigned int m_can_id, enum M_CAN_INT m_can_int);
void m_can_disable_interrupt(unsigned int m_can_id, enum M_CAN_INT m_can_int);
void m_can_select_interrupt_line(unsigned int m_can_id, enum M_CAN_INT m_can_int, enum M_CAN_INT_LINE m_can_int_line);
void m_can_enable_interrupt_line(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line);
void m_can_disable_interrupt_line(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line);
int m_can_get_interrupt_flag(unsigned int m_can_id, enum M_CAN_INT m_can_int);
void m_can_clear_interrupt_flag(unsigned int m_can_id, enum M_CAN_INT m_can_int);


m_can_int_handler_t m_can_set_interrupt_handler(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, m_can_int_handler_t m_can_int_handler);
void m_can_set_irq_priority(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, unsigned int priority);
void m_can_select_irq_for_processor(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, unsigned int prc_num);
void m_can_deselect_irq_for_processor(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, unsigned int prc_num);

#endif
