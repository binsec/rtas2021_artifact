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

#ifndef __DSPI_H__
#define __DSPI_H__

#include "typedefs.h"

enum DSPI_REQ
{
	DSPI_REQ_EOQF            = 0,
	DSPI_REQ_TFFF            = 1,
	DSPI_REQ_CMDFFF          = 2,
	DSPI_REQ_TFIWF           = 3,
	DSPI_REQ_TCF             = 4,
	DSPI_REQ_CMDTCF          = 5,
	DSPI_REQ_SPITCF          = 6,
	DSPI_REQ_DSITCF          = 7,
	DSPI_REQ_TFUF            = 8,
	DSPI_REQ_RFDF            = 9,
	DSPI_REQ_RFOF            = 10,
	DSPI_REQ_SPEF            = 11,
	DSPI_REQ_DPEF            = 12,
	DSPI_REQ_DDIF            = 13,
	DSPI_REQ_TFUF_RFOF_TFIWF = 14,
	DSPI_REQ_SPITCF_CMDTCF   = 15,
	DSPI_REQ_DSITCF_CMDFFF   = 16,
	DSPI_REQ_SPEF_DPEF       = 17
};

enum DSPI_REQ_DIR
{
	DSPI_REQ_DIR_INT = 0,
	DSPI_REQ_DIR_DMA = 1
};

enum DSPI_DMA_CHANNEL
{
	DSPI_DMA_RX,
	DSPI_DMA_TX,
	DSPI_DMA_CMD,
	DSPI_DMA_DD
};

enum DSPI_MODE
{
	DSPI_SLAVE_MODE =  0,
	DSPI_MASTER_MODE = 1
};

enum DSPI_CONFIGURATION
{
	DSPI_CONF_SPI = 0,
	DSPI_CONF_DSI = 1,
	DSPI_CONF_CSI = 2
};

enum DSPI_SAMPLE_POINT
{
	DSPI_SAMPLE_POINT_0_PROTOCOL_CLOCK = 0,
	DSPI_SAMPLE_POINT_1_PROTOCOL_CLOCK = 1,
	DSPI_SAMPLE_POINT_2_PROTOCOL_CLOCK = 2
};

enum DSPI_SPI_MODE
{
	DSPI_SPI_NORMAL_MODE   = 0,
	DSPI_EXTENDED_SPI_MODE = 1
};

enum DSPI_CONTINUOUS_PCS_MODE
{
	DSPI_CONTINUOUS_PCS_NORMAL_MODE = 0,
	DSPI_CONTINUOUS_PCS_FAST_MODE   = 1
};

enum DSPI_PARITY_ERROR_MODE
{
	DSPI_PARITY_ERROR_CONTINUE_MODE = 0,
	DSPI_PARITY_ERROR_STOP_MODE     = 1
};

enum DSPI_CLOCK_POLARITY
{
	DSPI_CLOCK_INACTIVE_LOW  = 0,
	DSPI_CLOCK_ACTIVE_HIGH   = DSPI_CLOCK_INACTIVE_LOW,
	DSPI_CLOCK_INACTIVE_HIGH = 1,
	DSPI_CLOCK_ACTIVE_LOW    = DSPI_CLOCK_INACTIVE_HIGH
};

enum DSPI_CLOCK_PHASE
{
	DSPI_CLOCK_CAPTURED_LEADING_EDGE = 0,
	DSPI_CLOCK_CHANGED_LEADING_EDGE  = 1,
};

struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES
{
	unsigned int double_baud_rate;               /* Master only  */
	unsigned int frame_size;                     /* Master/Slave */
	enum DSPI_CLOCK_POLARITY clock_polarity;     /* Master/Slave */
	enum DSPI_CLOCK_PHASE clock_phase;           /* Master/Slave */
	unsigned int lsb_first;                      /* Master only  */
	unsigned int pcs_to_sck_delay_prescaler;     /* Master only  */
	unsigned int after_sck_delay_prescaler;      /* Master only  */
	unsigned int delay_after_transfer_prescaler; /* Master only  */
	unsigned int baud_rate_prescaler;            /* Master only  */
	unsigned int pcs_to_sck_delay_scaler;        /* Master only  */
	unsigned int after_sck_delay_scaler;         /* Master only  */
	unsigned int delay_after_transfer_scaler;    /* Master only  */
	unsigned int baud_rate_scaler;               /* Master only  */
	unsigned int parity_enable;                  /* Slave only   */
	unsigned int parity_polarity;                /* Slave only   */
};

struct DSPI_MASTER_SPI_COMMAND
{
	unsigned int cont;
	unsigned int ctas;
	unsigned int eoq;
	unsigned int ctcnt;
	unsigned int pe_masc;
	unsigned int pp_mcsc;
	uint8_t      pcs;
};

enum DSPI_TSB_MODE
{
	DSPI_NORMAL_TSB_MODE      = 0,
	DSPI_INTERLEAVED_TSB_MODE = 1
};

enum DSPI_DSI_SOURCE
{
	DSPI_SERIALIZED_DATA_SOURCE           = 0,
	DSPI_ALTERNATE_SERIALIZED_DATA_SOURCE = 1
};

enum DSPI_DSI_TRIGGER_POLARITY
{
	DSPI_DSI_FALLING_EDGE_TRIGGER = 0,
	DSPI_DSI_RISING_EDGE_TRIGGER  = 1
};

enum DSPI_PARITY_POLARITY
{
	DSPI_EVEN_PARITY = 0,
	DSPI_ODD_PARITY  = 1
};

typedef void (*dspi_int_handler_t)(unsigned int, enum DSPI_REQ);

void dspi_drv_init();
void dspi_init(unsigned int dspi_id);

enum DSPI_MODE dspi_get_mode(unsigned int dspi_id);
void dspi_set_mode(unsigned int dspi_id, enum DSPI_MODE dspi_mode);
unsigned int dspi_is_continuous_sck_enabled(unsigned int dspi_id);
void dspi_enable_continuous_sck(unsigned int dspi_id);
void dspi_disable_continuous_sck(unsigned int dspi_id);
enum DSPI_CONFIGURATION dspi_get_configuration(unsigned int dspi_id);
void dspi_set_configuration(unsigned int dspi_id, enum DSPI_CONFIGURATION dspi_conf);
unsigned int dspi_is_freezed(unsigned int dspi_id);
void dspi_freeze(unsigned int dspi_id);
void dspi_unfreeze(unsigned int dspi_id);
unsigned int dspi_is_modified_timing_format_enabled(unsigned int dspi_id);
void dspi_enable_modified_timing_format(unsigned int dspi_id);
void dspi_disable_modified_timing_format(unsigned int dspi_id);
unsigned int dspi_is_peripheral_chip_select_strobe_enabled(unsigned int dspi_id);
void dspi_enable_peripheral_chip_select_strobe(unsigned int dspi_id);
void dspi_disable_peripheral_chip_select_strobe(unsigned int dspi_id);
unsigned int dspi_is_receive_fifo_overflow_overwrite_enabled(unsigned int dspi_id);
void dspi_enable_receive_fifo_overflow_overwrite(unsigned int dspi_id);
void dspi_disable_receive_fifo_overflow_overwrite(unsigned int dspi_id);
uint8_t dspi_get_peripheral_chip_select_inactive_states(unsigned int dspi_id);
void dspi_set_peripheral_chip_select_inactive_states(unsigned int dspi_id, uint8_t inactive_states);
unsigned int dspi_is_module_disabled(unsigned int dspi_id);
void dspi_disable_module(unsigned int dspi_id);
void dspi_enable_module(unsigned int dspi_id);
unsigned int dspi_is_transmit_fifo_disabled(unsigned int dspi_id);
void dspi_disable_transmit_fifo(unsigned int dspi_id);
void dspi_enable_transmit_fifo(unsigned int dspi_id);
unsigned int dspi_is_receive_fifo_disabled(unsigned int dspi_id);
void dspi_disable_receive_fifo(unsigned int dspi_id);
void dspi_enable_receive_fifo(unsigned int dspi_id);
void dspi_clear_tx_fifo(unsigned int dspi_id);
void dspi_clear_rx_fifo(unsigned int dspi_id);
enum DSPI_SAMPLE_POINT dspi_get_sample_point(unsigned int dspi_id);
void dspi_set_sample_point(unsigned int dspi_id, enum DSPI_SAMPLE_POINT dspi_sample_point);
enum DSPI_SPI_MODE dspi_get_spi_mode(unsigned int dspi_id);
void dspi_set_spi_mode(unsigned int dspi_id, enum DSPI_SPI_MODE dspi_spi_mode);
enum DSPI_CONTINUOUS_PCS_MODE dspi_get_continuous_pcs_mode(unsigned int dspi_id);
void dspi_set_continuous_pcs_mode(unsigned int dspi_id, enum DSPI_CONTINUOUS_PCS_MODE dspi_continuous_pcs_mode);
enum DSPI_PARITY_ERROR_MODE dspi_get_spi_parity_error_mode(unsigned int dspi_id);
void dspi_set_spi_parity_error_mode(unsigned int dspi_id, enum DSPI_PARITY_ERROR_MODE dspi_spi_parity_error_mode);
unsigned int dspi_are_tranfers_halted(unsigned int dspi_id);
void dspi_start_transfers(unsigned int dspi_id);
void dspi_stop_transfers(unsigned int dspi_id);

unsigned int dspi_get_spi_transfer_count(unsigned int dspi_id);

void dspi_get_spi_clock_and_transfer_attributes(unsigned int dspi_id, unsigned int ctas, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf);
void dspi_set_spi_clock_and_transfer_attributes(unsigned int dspi_id, unsigned int ctas, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf);
void dspi_get_dsi_clock_and_transfer_attributes(unsigned int dspi_id, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf);
void dspi_set_dsi_clock_and_transfer_attributes(unsigned int dspi_id, struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES *buf);

unsigned int dspi_get_interrupt_request_flag(unsigned int dspi_id, enum DSPI_REQ dspi_int);
void dspi_clear_interrupt_request_flag(unsigned int dspi_id, enum DSPI_REQ dspi_int);
unsigned int dspi_get_tx_and_rx_status(unsigned int dspi_id);
void dspi_clear_tx_and_rx_status(unsigned int dspi_id);
unsigned int dspi_get_busy_flag(unsigned int dspi_id);
unsigned int dspi_get_tx_fifo_counter(unsigned int dspi_id);
unsigned int dspi_get_transmit_next_pointer(unsigned int dspi_id);
unsigned int dspi_get_rx_fifo_counter(unsigned int dspi_id);
unsigned int dspi_get_pop_next_pointer(unsigned int dspi_id);

void dspi_enable_request(unsigned int dspi_id, enum DSPI_REQ dspi_int);
void dspi_disable_request(unsigned int dspi_id, enum DSPI_REQ dspi_int);
enum DSPI_REQ_DIR dspi_get_request_direction(unsigned int dspi_id, unsigned int dspi_req);
void dspi_set_request_direction(unsigned int dspi_id, enum DSPI_REQ dspi_req, enum DSPI_REQ_DIR dspi_req_dir);

void dspi_master_spi_tx_fifo_push(unsigned int dspi_id, struct DSPI_MASTER_SPI_COMMAND *dspi_master_spi_cmd, uint16_t data);
void dspi_master_spi_tx_fifo_push_command(unsigned int dspi_id, struct DSPI_MASTER_SPI_COMMAND *dspi_master_spi_cmd);
void dspi_spi_tx_fifo_push_data_byte(unsigned int dspi_id, uint8_t data);
void dspi_spi_tx_fifo_push_data_half_word(unsigned int dspi_id, uint16_t data);

uint8_t dspi_spi_rx_fifo_pop_byte(unsigned int dspi_id);
uint16_t dspi_spi_rx_fifo_pop_half_word(unsigned int dspi_id);
uint32_t dspi_spi_rx_fifo_pop_word(unsigned int dspi_id);

uint32_t dspi_tx_fifo_peek(unsigned int dspi_id, unsigned int txptr);
uint32_t dspi_rx_fifo_peek(unsigned int dspi_id, unsigned int rxptr);

enum DSPI_TSB_MODE dspi_get_dsi_timed_serial_bus_mode(unsigned int dspi_id);
void dspi_enable_dsi_timed_serial_bus_mode(unsigned int dspi_id);
void dspi_disable_dsi_timed_serial_bus_mode(unsigned int dspi_id);
enum DSPI_DSI_SOURCE dspi_get_dsi_source(unsigned int dspi_id);
void dspi_set_dsi_timed_serial_bus_mode(unsigned int dspi_id, enum DSPI_TSB_MODE dspi_tsb_mode);
enum DSPI_DSI_TRIGGER_POLARITY dspi_get_dsi_trigger_polarity(unsigned int dspi_id);
void dspi_set_dsi_trigger_polarity(unsigned int dspi_id, enum DSPI_DSI_TRIGGER_POLARITY dspi_dsi_trigger_polarity);
unsigned int dspi_is_dsi_continuous_peripheral_chip_select_enabled(unsigned int dspi_id);
void dspi_enable_dsi_continuous_peripheral_chip_select(unsigned int dspi_id);
void dspi_disable_dsi_continuous_peripheral_chip_select(unsigned int dspi_id);
unsigned int dspi_get_dsi_clock_and_transfer_attribute_select(unsigned int dspi_id);
void dspi_master_set_dsi_clock_and_transfer_attribute_select(unsigned int dspi_id, unsigned int dsictas);
unsigned int dspi_is_dsi_data_match_stop_enabled(unsigned int dspi_id);
void dspi_enable_dsi_data_match_stop(unsigned int dspi_id);
void dspi_disable_dsi_data_match_stop(unsigned int dspi_id);
unsigned int dspi_is_dsi_stop_on_parity_error_enabled(unsigned int dspi_id);
void dspi_enable_dsi_stop_on_parity_error(unsigned int dspi_id);
void dspi_disable_dsi_stop_on_parity_error(unsigned int dspi_id);
enum DSPI_PARITY_ERROR_MODE dspi_get_dsi_parity_error_mode(unsigned int dspi_id);
void dspi_set_dsi_parity_error_mode(unsigned int dspi_id, enum DSPI_PARITY_ERROR_MODE dspi_dsi_parity_error_mode);
unsigned int dspi_is_dsi_parity_check_enabled(unsigned int dspi_id);
void dspi_enable_dsi_parity_check(unsigned int dspi_id);
void dspi_disable_dsi_parity_check(unsigned int dspi_id);
enum DSPI_PARITY_POLARITY dspi_get_dsi_parity_polarity(unsigned int dspi_id);
void dspi_set_dsi_parity_polarity(unsigned int dspi_id, enum DSPI_PARITY_POLARITY dspi_dsi_parity_polarity);
uint8_t dspi_master_get_dsi_peripheral_chip_select(unsigned int dspi_id);
void dspi_master_set_dsi_peripheral_chip_select(unsigned int dspi_id, uint8_t dpcs);

/**** 
 * 
 * 
 * 
 ******/

unsigned int dspi_get_spi_data_transfer_count_preload(unsigned int dspi_id, unsigned int ctas);
void dspi_set_spi_data_transfer_count_preload(unsigned int dspi_id, unsigned int ctas, unsigned int dtcp);
unsigned int dspi_get_command_fifo_counter(unsigned int dspi_id);
unsigned int dspi_get_command_next_pointer(unsigned int dspi_id);

dspi_int_handler_t dspi_set_interrupt_handler(unsigned int dspi_id, enum DSPI_REQ dspi_int, dspi_int_handler_t dspi_int_handler);
void dspi_set_irq_priority(unsigned int dspi_id, enum DSPI_REQ dspi_int, unsigned int priority);
void dspi_select_irq_for_processor(unsigned int dspi_id, enum DSPI_REQ dspi_int, unsigned int prc_num);
void dspi_deselect_irq_for_processor(unsigned int dspi_id, enum DSPI_REQ dspi_int, unsigned int prc_num);

#endif
