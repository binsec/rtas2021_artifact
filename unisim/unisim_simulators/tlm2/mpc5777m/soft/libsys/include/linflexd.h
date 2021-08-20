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

#ifndef __LINFLEXD_H__
#define __LINFLEXD_H__

#include "typedefs.h"

enum LINFlexD_INT
{
	LINFlexD_INT_RX  = 0,
	LINFlexD_INT_TX  = 1,
	LINFlexD_INT_ERR = 2
};

enum LINFlexD_WORD_LENGTH
{
	LINFlexD_WORD_LENGTH_7  = 0,
	LINFlexD_WORD_LENGTH_8  = 1,
	LINFlexD_WORD_LENGTH_15 = 2,
	LINFlexD_WORD_LENGTH_16 = 3
};

enum LINFlexD_PARITY_CONTROL
{
	LINFlexD_PARITY_CONTROL_EVEN = 0,
	LINFlexD_PARITY_CONTROL_ODD  = 1,
	LINFlexD_PARITY_CONTROL_LOG0 = 2,
	LINFlexD_PARITY_CONTROL_LOG1 = 3
};

typedef void (*linflexd_int_handler_t)(unsigned int, enum LINFlexD_INT);

void linflexd_drv_init();

void linflexd_init(unsigned int linflexd_id);

void linflexd_request_to_enter_init_mode(unsigned int linflexd_id);
void linflexd_request_to_exit_init_mode(unsigned int linflexd_id);
void linflexd_request_to_enter_sleep_mode(unsigned int linflexd_id);
void linflexd_request_to_exit_sleep_mode(unsigned int linflexd_id);

void linflexd_enable_auto_wakeup(unsigned int linflexd_id);
void linflexd_disable_auto_wakeup(unsigned int linflexd_id);

void linflexd_enable_loop_back_mode(unsigned int linflexd_id);
void linflexd_disable_loop_back_mode(unsigned int linflexd_id);

void linflexd_enable_receiver_buffer_locked_mode(unsigned int linflexd_id);
void linflexd_disable_receiver_buffer_locked_mode(unsigned int linflexd_id);

void linflexd_set_lin_clock_divisor(unsigned int linflexd_id, unsigned int int_part, unsigned int frac_part); // int_part in [0,1048575] (zero to disable LIN clock), frac_part in [0,15], ldiv = int_part + (frac_part / 16)

void linflexd_transmit_msb_first(unsigned int linflexd_id);
void linflexd_transmit_lsb_first(unsigned int linflexd_id);

void linflexd_receive_msb_first(unsigned int linflexd_id);
void linflexd_receive_lsb_first(unsigned int linflexd_id);

void linflexd_enable_inverted_data_transmission(unsigned int linflexd_id);
void linflexd_disable_inverted_data_transmission(unsigned int linflexd_id);

void linflexd_enable_inverted_data_reception(unsigned int linflexd_id);
void linflexd_disable_inverted_data_reception(unsigned int linflexd_id);

void linflexd_select_tx_stop_bits(unsigned int linflexd_id, unsigned int stop_bits); // stop_bits in {1,2}

void linflexd_soft_reset(unsigned int linflexd_id);

void linflexd_enable_uart_mode(unsigned int linflexd_id);
void linflexd_disable_uart_mode(unsigned int linflexd_id);

void linflexd_enable_uart_rx(unsigned int linflexd_id);
void linflexd_disable_uart_rx(unsigned int linflexd_id);

void linflexd_enable_uart_tx(unsigned int linflexd_id);
void linflexd_disable_uart_tx(unsigned int linflexd_id);

void linflexd_set_uart_over_sampling_rate(unsigned int linflexd_id, unsigned int v); // v in {4, 5, 6, 8}
void linflexd_enable_uart_reduced_over_sampling(unsigned int linflexd_id);
void linflexd_disable_uart_reduced_over_sampling(unsigned int linflexd_id);

void linflexd_set_uart_number_of_expected_frame(unsigned int linflexd_id, unsigned int v); // 0 <= v <= 7

void linflexd_disable_uart_timeout(unsigned int linflexd_id); // available only when PSI5S is in UART_STDALONE mode
void linflexd_enable_uart_timeout(unsigned int linflexd_id); // available only when PSI5S is in UART_STDALONE mode

unsigned int linflexd_uart_rx_buffer_mode(unsigned int linflexd_id);
unsigned int linflexd_uart_rx_fifo_mode(unsigned int linflexd_id);

void linflexd_select_uart_rx_stop_bits(unsigned int linflexd_id, unsigned int stop_bits); // stop_bits in {1, 2, 3}
void linflexd_select_uart_rx_buffer_mode(unsigned int linflexd_id);
void linflexd_select_uart_rx_fifo_mode(unsigned int linflexd_id);

unsigned int linflexd_uart_tx_buffer_mode(unsigned int linflexd_id);
unsigned int linflexd_uart_tx_fifo_mode(unsigned int linflexd_id);

void linflexd_select_uart_tx_buffer_mode(unsigned int linflexd_id);
void linflexd_select_uart_tx_fifo_mode(unsigned int linflexd_id);

void linflexd_set_uart_rx_buffer_length(unsigned int linflexd_id, unsigned int length); // 1, 2, 3, or 4
unsigned int linflexd_get_uart_rx_buffer_length(unsigned int linflexd_id);
unsigned int linflexd_get_uart_rx_fifo_counter(unsigned int linflexd_id);
unsigned int linflexd_is_uart_rx_fifo_empty(unsigned int linflexd_id);

void linflexd_set_uart_tx_buffer_length(unsigned int linflexd_id, unsigned int length); // 1, 2, 3, or 4
unsigned int linflexd_get_uart_tx_buffer_length(unsigned int linflexd_id);
unsigned int linflexd_get_uart_tx_fifo_counter(unsigned int linflexd_id);
unsigned int linflexd_is_uart_tx_fifo_full(unsigned int linflexd_id);

void linflexd_enable_uart_special_word_length(unsigned int linflexd_id);
void linflexd_disable_uart_special_word_length(unsigned int linflexd_id);
void linflexd_enable_uart_parity_control(unsigned int linflexd_id);
void linflexd_disable_uart_parity_control(unsigned int linflexd_id);
void linflexd_select_uart_parity_control(unsigned int linflexd_id, enum LINFlexD_PARITY_CONTROL linflexd_parity_control);
void linflexd_select_uart_word_length(unsigned int linflexd_id, enum LINFlexD_WORD_LENGTH wl);

void linflexd_uart_rx_buffer_read_byte(unsigned int linflexd_id, uint8_t *data_ptr, unsigned int byte_index); // byte_index=0, 1, 2, or 3
void linflexd_uart_rx_buffer_read_half_word(unsigned int linflexd_id, uint16_t *data_ptr, unsigned int half_word_index); // half_word_index=0, 1
void linflexd_uart_rx_buffer_read_word(unsigned int linflexd_id, uint32_t *data_ptr);
void linflexd_uart_rx_fifo_read_byte(unsigned int linflexd_id, uint8_t *data_ptr);
void linflexd_uart_rx_fifo_read_half_word(unsigned int linflexd_id, uint16_t *data_ptr);

void linflexd_uart_tx_buffer_write_byte(unsigned int linflexd_id, const uint8_t *data_ptr, unsigned int byte_index); // byte_index=0, 1, 2, or 3
void linflexd_uart_tx_buffer_write_half_word(unsigned int linflexd_id, const uint16_t *data_ptr, unsigned int half_word_index); // half_word_index=0, 1
void linflexd_uart_tx_buffer_write_word(unsigned int linflexd_id, const uint32_t *data_ptr);
void linflexd_uart_tx_fifo_write_byte(unsigned int linflexd_id, const uint8_t *data_ptr);
void linflexd_uart_tx_fifo_write_half_word(unsigned int linflexd_id, const uint16_t *data_ptr);

void *linflexd_uart_rx_fifo_byte_direct_ptr(unsigned int linflexd_id);
void *linflexd_uart_tx_fifo_byte_direct_ptr(unsigned int linflexd_id);

unsigned int linflexd_get_uart_byte_parity_error(unsigned int linflexd_id, unsigned int byte_index); // byte_index in {0,1,2,3}
unsigned int linflexd_get_uart_half_word_parity_error(unsigned int linflexd_id, unsigned int half_word_index); // half_word_index in {0,1}

unsigned int linflexd_is_uart_rx_buffer_ready(unsigned int linflexd_id);
void linflexd_release_uart_rx_buffer(unsigned int linflexd_id);
unsigned int linflexd_get_uart_receiver_data_input_signal(unsigned int linflexd_id);
unsigned int linflexd_is_uart_rx_fifo_no_empty(unsigned int linflexd_id);

void linflexd_enable_dma_tx(unsigned int linflexd_id, unsigned int chan);
void linflexd_disable_dma_tx(unsigned int linflexd_id, unsigned int chan);
void linflexd_enable_dma_rx(unsigned int linflexd_id, unsigned int chan);
void linflexd_disable_dma_rx(unsigned int linflexd_id, unsigned int chan);

linflexd_int_handler_t linflexd_set_interrupt_handler(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, linflexd_int_handler_t linflexd_int_handler);
void linflexd_set_irq_priority(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, unsigned int priority);
void linflexd_select_irq_for_processor(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, unsigned int prc_num);
void linflexd_deselect_irq_for_processor(unsigned int linflexd_id, enum LINFlexD_INT linflexd_int, unsigned int prc_num);

void linflexd_enable_stuck_at_zero_interrupt(unsigned int linflexd_id);
void linflexd_disable_stuck_at_zero_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_stuck_at_zero_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_stuck_at_zero_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_output_compare_interrupt(unsigned int linflexd_id);
void linflexd_disable_output_compare_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_output_compare_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_output_compare_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_frame_error_interrupt(unsigned int linflexd_id);
void linflexd_disable_frame_error_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_frame_error_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_frame_error_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_buffer_overrun_error_interrupt(unsigned int linflexd_id);
void linflexd_disable_buffer_overrun_error_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_buffer_overrun_error_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_buffer_overrun_error_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_wakeup_interrupt(unsigned int linflexd_id);
void linflexd_disable_wakeup_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_wakeup_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_wakeup_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_uart_timeout_interrupt(unsigned int linflexd_id);
void linflexd_disable_uart_timeout_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_timeout_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_timeout_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_data_received_interrupt(unsigned int linflexd_id);
void linflexd_disable_data_received_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_data_received_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_data_received_interrupt_flag(unsigned int linflexd_id);

void linflexd_enable_data_transmitted_interrupt(unsigned int linflexd_id);
void linflexd_disable_data_transmitted_interrupt(unsigned int linflexd_id);
unsigned int linflexd_get_uart_data_transmitted_interrupt_flag(unsigned int linflexd_id);
void linflexd_clear_uart_data_transmitted_interrupt_flag(unsigned int linflexd_id);

#endif
