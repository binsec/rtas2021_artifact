/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include <stdio.h>
#include "dspi.h"
#include "m_can.h"

volatile int counter = 0;

#define DSPI_INT_MODE 1

#if DSPI_INT_MODE
void dspi_0_tfff_rfdf(unsigned int dspi_id, enum DSPI_REQ dspi_req)
{
	static int x = 0;
	
	while(dspi_get_interrupt_request_flag(dspi_id, DSPI_REQ_RFDF))
	{
		dspi_clear_interrupt_request_flag(dspi_id, DSPI_REQ_RFDF);
		volatile uint16_t v = dspi_spi_rx_fifo_pop_half_word(dspi_id);
	}
	while(dspi_get_interrupt_request_flag(dspi_id, DSPI_REQ_TFFF))
	{
		dspi_clear_interrupt_request_flag(dspi_id, DSPI_REQ_TFFF);
		uint16_t v = x ? 0xc0ca : 0xbaba;
		dspi_spi_tx_fifo_push_data_half_word(dspi_id, v);
		x ^= 1;
	}
}
#endif

void m_can_1_int0(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line)
{
	if(m_can_get_interrupt_flag(m_can_id, M_CAN_INT_RF0N))
	{
		m_can_clear_interrupt_flag(1, M_CAN_INT_RF0N);
		struct m_can_rx_buffer_fifo_element e;
		m_can_pop_rx_fifo(m_can_id, 0, &e);
	}
}

int main(void)
{
	struct m_can_standard_message_id_filter_element filt0 = { .S0 = { .SFT = DUAL_ID_FILTER, .SFEC = FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES, .SFID1 = 0x12, .SFID2 = 0x14 } };
	struct m_can_standard_message_id_filter_element filt1 = { .S0 = { .SFT = CLASSIC_FILTER, .SFEC = FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES, .SFID1 = 0x10, .SFID2 = 0xff } };
	
	m_can_init(1);
	m_can_enable_configuration_change(1);
	m_can_set_baud_rate_prescaler(1, 1);
	m_can_set_tseg1(1, 34);
	m_can_set_tseg2(1, 5);
	m_can_set_sjw(1, 1);
	m_can_set_rx_fifo_operation_mode(1, 0, M_CAN_RX_FIFO_OVERWRITING_MODE);
	m_can_set_rx_fifo_size(1, 0, 2);
	m_can_set_rx_fifo_start_address(1, 0, 0x800);
	m_can_enable_timestamp_counter(1);
	m_can_enable_interrupt(1, M_CAN_INT_RF0N);
	m_can_select_interrupt_line(1, M_CAN_INT_RF0N, M_CAN_INT0);
	m_can_enable_interrupt_line(1, M_CAN_INT0);
	m_can_set_interrupt_handler(1, M_CAN_INT0, m_can_1_int0);
	m_can_reject_non_matching_frame_standard(1);
	m_can_set_standard_filter_list_size(1, 64);
	m_can_set_standard_filter_list_start_address(1, 0x1000);
	m_can_write_standard_filter(1, 0, &filt0);
	m_can_write_standard_filter(1, 1, &filt1);
	m_can_disable_configuration_change(1);
	m_can_exit_init_mode(1);
	
// 	struct m_can_tx_buffer_element e = { { .XTD = 0, .RTR = 0, .ID = (0x10 << 18) }, {.MM = 0, .EFC = 0, .DLC = 4}, .DATA = { 0x12, 0x34, 0x56, 0x78 } };
// 	m_can_write_tx_buffer_element(3, 0, &e);
// 	
// 	m_can_add_tx_buffer_request(3, 0);

	dspi_init(0);
	dspi_enable_module(0);
	dspi_set_mode(0, DSPI_SLAVE_MODE);
	
	struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES cta0;
	cta0.double_baud_rate               = 0;
	cta0.frame_size                     = 16;
	cta0.clock_polarity                 = DSPI_CLOCK_INACTIVE_LOW;
	cta0.clock_phase                    = DSPI_CLOCK_CAPTURED_LEADING_EDGE;
	cta0.lsb_first                      = 0;
	cta0.pcs_to_sck_delay_prescaler     = 0;
	cta0.after_sck_delay_prescaler      = 0;
	cta0.delay_after_transfer_prescaler = 0;
	cta0.baud_rate_prescaler            = 0;
	cta0.pcs_to_sck_delay_scaler        = 0;
	cta0.after_sck_delay_scaler         = 0;
	cta0.delay_after_transfer_scaler    = 0;
	cta0.baud_rate_scaler               = 0;
	cta0.parity_enable                  = 0;
	cta0.parity_polarity                = 0;

	dspi_set_spi_clock_and_transfer_attributes(0, 0, &cta0);

#if DSPI_INT_MODE
	dspi_set_interrupt_handler(0, DSPI_REQ_TFFF, dspi_0_tfff_rfdf);
	dspi_set_interrupt_handler(0, DSPI_REQ_RFDF, dspi_0_tfff_rfdf);
	dspi_enable_request(0, DSPI_REQ_TFFF);
	dspi_enable_request(0, DSPI_REQ_RFDF);
#endif

	dspi_start_transfers(0);
	
#if !DSPI_INT_MODE
	int x = 0;
	
	do
	{
		if(dspi_get_interrupt_request_flag(0, DSPI_REQ_TFFF))
		{
			dspi_clear_interrupt_request_flag(0, DSPI_REQ_TFFF);
			uint16_t v = x ? 0xc0ca : 0xbaba;
			dspi_spi_tx_fifo_push_data_half_word(0, v);
			//printf("push 0x%x\n", v);
			x ^= 1;
		}
		if(dspi_get_interrupt_request_flag(0, DSPI_REQ_RFDF))
		{
			dspi_clear_interrupt_request_flag(0, DSPI_REQ_RFDF);
			uint16_t v = dspi_spi_rx_fifo_pop_half_word(0);
			//printf("pop 0x%x\n", v);
		}
	} while(1);
#endif

	printf("CPU#0 says Hello world!\r\n");
	/* Loop forever */
	for(;;)
	{
		counter++;
		PPC_WAIT;
	}
}
