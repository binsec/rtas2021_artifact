/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include "intc.h"
#include "stm.h"
#include "swt.h"
#include "pit.h"
#include "linflexd.h"
#include "dmamux.h"
#include "edma.h"
#include "console.h"
#include "dspi.h"
#include "siul2.h"
#include "pbridge.h"
#include "xbar.h"
#include "smpu.h"
#include "m_can.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>

#define DSPI_INT_MODE 1

void periodic_task(unsigned int stm_id, unsigned int chan)
{
	swt_service_sequence(2);                   // rearm SWT_2
	stm_set_channel_compare(stm_id, chan, stm_get_channel_compare(stm_id, chan) + 10000); // schedule next STM_2 tic 200 us later (i.e. 10000 cycles at 50 MHz)
	stm_clear_interrupt_flag(stm_id, chan);    // clear STM_2 interrupt flag
}

void periodic_task2(unsigned int pit_id, unsigned int chan)
{
	pit_clear_timer_interrupt_flag(pit_id, chan); // clear PIT_0 interrupt flag
}

#if DSPI_INT_MODE
void dspi_2_tfff_rfdf(unsigned int dspi_id, enum DSPI_REQ dspi_req)
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
		
		struct DSPI_MASTER_SPI_COMMAND spi_cmd;
		spi_cmd.cont = 1;
		spi_cmd.ctas = 0;
		spi_cmd.eoq = 0;
		spi_cmd.ctcnt = 0;
		spi_cmd.pe_masc = 0;
		spi_cmd.pp_mcsc = 0;
		spi_cmd.pcs = 1 << 0;

		uint16_t v = x ? 0xcafe : 0xbabe;
		dspi_master_spi_tx_fifo_push(dspi_id, &spi_cmd, v);
		x ^= 1;
	}
}
#endif

int main(void)
{
	swt_set_service_mode(2, SMD_KEYED_SERVICE_SEQUENCE); // SWT_2: select keyed service sequence for SWT_2
	swt_set_service_key(2, 1234);                        // SWT_2: set SWT_2 service key
	swt_set_timeout(2, 50000);                           // SWT_2: set a timeout of 3.125 ms for SWT_2
	swt_enable(2);                                       // SWT_2: enable SWT_2
	
	// trigger periodic_task every 200 us (with a 50 Mhz clock)
	stm_set_interrupt_handler(2, 0, periodic_task); // STM_2: install a hook for STM_2 channel #0 interrupts
	stm_enable_counter(2);                          // STM_2: enable STM_2 counter
	stm_set_channel_compare(2, 0, 10000);           // STM_2: set STM_2 channel #0 compare value to 200 us (i.e. 10000 cycles at 50 MHz)
	stm_enable_channel(2, 0);                       // STM_2: enable STM_2 channel #0
	
	// trigger periodic_task2 every 300 us (with a 80 MHz clock)
	pit_set_timer_interrupt_handler(0, 0, periodic_task2); // PIT_0: install a hook for PIT_0 timer #0 interrupts
	pit_set_timer_load_value(0, 0, 24000 - 1);             // PIT_0: set PIT_0 timer #0 down counter to zero every 300 us (24000 cycles at 80 MHz)
	pit_enable_timer(0, 0);                                // PIT_0: enable timer #0
	pit_enable_timer_interrupt(0, 0);                      // PIT_0: enable interrupts for timer #0
	pit_enable_timers_clock(0);                            // PIT_0: enable PIT_0 timers clock
	
	m_can_init(3);
	m_can_enable_configuration_change(3);
	m_can_set_baud_rate_prescaler(3, 1);
	m_can_set_tseg1(3, 34);
	m_can_set_tseg2(3, 5);
	m_can_set_sjw(3, 1);
	m_can_set_tx_fifo_queue_mode(3, M_CAN_TX_FIFO_MODE);
	m_can_set_num_dedicated_tx_buffers(3, 16);
	m_can_set_tx_fifo_queue_size(3, 16);
	m_can_set_tx_buffers_start_address(3, 0x0);
	m_can_set_tx_event_fifo_start_address(3, 0x200);
	m_can_set_tx_event_fifo_size(3, 32);
	m_can_enable_timestamp_counter(3);
	m_can_disable_configuration_change(3);
	m_can_exit_init_mode(3);
	
	struct m_can_tx_buffer_element e[32] = {
		{ { .XTD = 0, .RTR = 0, .ID = (0 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (1 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (2 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (3 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (4 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (5 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (6 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (7 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (8 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (9 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (10 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (11 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (12 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (13 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (14 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (15 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (16 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (17 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (18 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (19 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (20 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (21 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (22 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (23 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (24 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (25 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (26 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (27 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (28 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (29 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (30 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (31 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } }
	};

	unsigned int i;
	for(i = 0; i < 32; i++)
	{
		while(m_can_get_tx_fifo_free_level(3) == 0);
		m_can_tx_fifo_push(3, &e[i]);
	}
	
	dspi_init(2);
	dspi_set_mode(2, DSPI_MASTER_MODE);
	dspi_set_peripheral_chip_select_inactive_states(2, 0xff); // inactive high, active low
	
	struct DSPI_CLOCK_AND_TRANSFER_ATTRIBUTES cta0; // 1 Mbit/s with a protocol clock at 80 Mhz
	cta0.double_baud_rate               = 0;
	cta0.frame_size                     = 16;
	cta0.clock_polarity                 = DSPI_CLOCK_INACTIVE_LOW;
	cta0.clock_phase                    = DSPI_CLOCK_CAPTURED_LEADING_EDGE;
	cta0.lsb_first                      = 0;
	cta0.pcs_to_sck_delay_prescaler     = 0;
	cta0.after_sck_delay_prescaler      = 0;
	cta0.delay_after_transfer_prescaler = 0;
	cta0.baud_rate_prescaler            = 2;
	cta0.pcs_to_sck_delay_scaler        = 0;
	cta0.after_sck_delay_scaler         = 0;
	cta0.delay_after_transfer_scaler    = 0;
	cta0.baud_rate_scaler               = 3;
	cta0.parity_enable                  = 0;
	cta0.parity_polarity                = 0;

	dspi_set_spi_clock_and_transfer_attributes(2, 0, &cta0);
	
#if DSPI_INT_MODE
	dspi_set_interrupt_handler(2, DSPI_REQ_TFFF, dspi_2_tfff_rfdf);
	dspi_set_interrupt_handler(2, DSPI_REQ_RFDF, dspi_2_tfff_rfdf);
	dspi_enable_request(2, DSPI_REQ_TFFF);
	dspi_enable_request(2, DSPI_REQ_RFDF);
#endif
	
	dspi_enable_module(2);
	dspi_start_transfers(2);
	
#if !DSPI_INT_MODE
	struct DSPI_MASTER_SPI_COMMAND spi_cmd;
	spi_cmd.cont = 0;
	spi_cmd.ctas = 0;
	spi_cmd.eoq = 0;
	spi_cmd.ctcnt = 0;
	spi_cmd.pe_masc = 0;
	spi_cmd.pp_mcsc = 0;
	spi_cmd.pcs = 1 << 0;
	
	int x = 0;
	
	do
	{
		if(dspi_get_interrupt_request_flag(2, DSPI_REQ_TFFF))
		{
			dspi_clear_interrupt_request_flag(2, DSPI_REQ_TFFF);
			uint16_t v = x ? 0xcafe : 0xbabe;
			dspi_master_spi_tx_fifo_push(2, &spi_cmd, v);
			//printf("push 0x%x\n", v);
			x ^= 1;
		}
		if(dspi_get_interrupt_request_flag(2, DSPI_REQ_RFDF))
		{
			dspi_clear_interrupt_request_flag(2, DSPI_REQ_RFDF);
			uint16_t v = dspi_spi_rx_fifo_pop_half_word(2);
			//printf("pop 0x%x\n", v);
		}
	} while(1);
#endif
	
	siul2_set_output_drive_control(12, ODC_PUSH_PULL);
	siul2_set_output_drive_control(14, ODC_OUTPUT_BUFFER_DISABLED);
	
	const char boot_msg[] = "HAL 9000...booting..............................\r\n"
						"I'm ready to control Discovery One spacecraft.\r\n\r\n"
						"Dave, Discovery One is approaching Jupiter.\r\n"
						"What should I do ?\r\n";

	const char prompt[] = "HAL> ";

	const char response[] = "I'm sorry Dave, I'm afraid I can't do that.\r\n"
					"I know you and Frank were planning to disconnect me.\r\n"
					"And that's something I cannot allow to happen.\r\n"
					"Look Dave, I can see you're really upset about this.\r\n"
					"I honestly think you ought to sit down calmly, take a stress pill, and think things over.\r\n";
	
	fputs(boot_msg, stdout);

	do
	{
		fputs(prompt, stdout);
		
		char line[1536];

		siul2_gpio_write(12, 1);
		
		if(!fgets(line, sizeof(line), stdin))
		{
			fprintf(stderr, "can' get line\r\n");
			break;
		}
		
		siul2_gpio_write(12, 0);
		
		uint8_t gpio_button_status = siul2_gpio_read(14);
		
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		char time_str[32];
		strftime(time_str, sizeof(time_str), "%c", &tm);
		
		fprintf(stdout, "at %s, got \"%s\" on prompt and %u on GPIO button\r\n", time_str/*asctime(&tm)*/, line, gpio_button_status);
		
		//struct tms tms;
		
		//fprintf(stdout, "at %f sec, got \"%s\"\n", (double) times(&tms) / sysconf(_SC_CLK_TCK), line);

		fputs(response, stdout);
	}
	while(1);
	
	return 0;
}
