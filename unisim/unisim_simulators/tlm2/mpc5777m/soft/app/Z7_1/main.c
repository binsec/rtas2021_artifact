/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include <stdio.h>
#include "m_can.h"

volatile int counter = 0;

int main(void)
{
	m_can_init(2);
	m_can_enable_configuration_change(2);
	m_can_set_baud_rate_prescaler(2, 1);
	m_can_set_tseg1(2, 34);
	m_can_set_tseg2(2, 5);
	m_can_set_sjw(2, 1);
	m_can_set_tx_fifo_queue_mode(2, M_CAN_TX_FIFO_MODE);
	m_can_set_num_dedicated_tx_buffers(2, 16);
	m_can_set_tx_fifo_queue_size(2, 16);
	m_can_set_tx_buffers_start_address(2, 0x400);
	m_can_set_tx_event_fifo_start_address(2, 0x600);
	m_can_set_tx_event_fifo_size(2, 32);
	m_can_enable_timestamp_counter(2);
	m_can_disable_configuration_change(2);
	m_can_exit_init_mode(2);
	
	struct m_can_tx_buffer_element e[32] = {
		{ { .XTD = 0, .RTR = 0, .ID = (32 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (33 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (34 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (35 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (36 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (37 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (38 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (39 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (40 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (41 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (42 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (43 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (44 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (45 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (46 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (47 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (48 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (49 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (50 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (51 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (52 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (53 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (54 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (55 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (56 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (57 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (58 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (59 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (60 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } },
		{ { .XTD = 0, .RTR = 0, .ID = (61 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x12345678 } },
		{ { .XTD = 0, .RTR = 0, .ID = (62 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0x98765432 } },
		{ { .XTD = 0, .RTR = 0, .ID = (63 << 18) }, {.MM = 0, .EFC = 1, .DLC = 4}, .DATA = { 0xa55aa55a } }
	};

	unsigned int i;
	for(i = 0; i < 32; i++)
	{
		while(m_can_get_tx_fifo_free_level(2) == 0);
		m_can_tx_fifo_push(2, &e[i]);
	}

	printf("CPU#1 says Hello world!\r\n");
	/* Loop forever */
	for(;;)
	{
		counter++;
		PPC_WAIT;
	}
}
