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

#include "m_can.h"
#include "intc.h"
#include "MPC5777M.h"
#include "compiler_api.h"
#include <unistd.h>

#define SHARED_CAN_MESSAGE_RAM (*(volatile uint32_t *) 0xffed4000)

static unsigned char m_can_little_endian[5];

#define M_CAN_STORE32(i, p, v) do { if(m_can_little_endian[i]) STORE_LE32(p, v); else *(p) = (v); } while(0)
#define M_CAN_LOAD32(i, p) (m_can_little_endian[i] ? LOAD_LE32(p) : (*(p)))

#define M_CAN_READ_REG(m_can_id,reg,field) \
({ \
    M_CAN_##reg##_tag reg;                                       \
    reg.R = M_CAN_LOAD32(m_can_id, &m_can[m_can_id]->reg.R); \
    reg.B.field; \
})

#define M_CAN_WRITE_REG(m_can_id,reg,field,value)             \
do                                                            \
{                                                             \
    M_CAN_##reg##_tag reg;                                    \
    reg.R = 0;                                                \
    reg.B.field = (value);                                    \
    M_CAN_STORE32(m_can_id, &m_can[m_can_id]->reg.R, reg.R);  \
}                                                             \
while(0)

#define M_CAN_READ_MODIFY_WRITE_REG(m_can_id,reg,field,value) \
do                                                            \
{                                                             \
    M_CAN_##reg##_tag reg;                                    \
    reg.R = M_CAN_LOAD32(m_can_id, &m_can[m_can_id]->reg.R);  \
    reg.B.field = (value);                                    \
    M_CAN_STORE32(m_can_id, &m_can[m_can_id]->reg.R, reg.R);  \
} while(0)

#define M_CAN_READ_MODIFY_WRITE_CHECK_REG(m_can_id,reg,field,value) \
do                                                                  \
{                                                                   \
    M_CAN_##reg##_tag reg;                                          \
    reg.R = M_CAN_LOAD32(m_can_id, &m_can[m_can_id]->reg.R);        \
    reg.B.field = (value);                                          \
    M_CAN_STORE32(m_can_id, &m_can[m_can_id]->reg.R, reg.R);        \
    do                                                              \
    {                                                               \
        reg.R = M_CAN_LOAD32(m_can_id, &m_can[m_can_id]->reg.R);    \
    }                                                               \
    while(reg.B.field != (value));                                  \
} while(0)

static void __m_can_1_int0__();
static void __m_can_1_int1__();

static void __m_can_2_int0__();
static void __m_can_2_int1__();

static void __m_can_3_int0__();
static void __m_can_3_int1__();

static void __m_can_4_int0__();
static void __m_can_4_int1__();

void m_can_int_handler_default(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line)
{
}

static volatile struct M_CAN_tag *m_can[5];
static m_can_int_handler_t m_can_int_handlers[5][2];
static uint16_t m_can_irq_vector_table[5][2];

static unsigned int m_can_get_irq_vector(unsigned int m_can_id, unsigned int int_id)
{
	return m_can_irq_vector_table[m_can_id][int_id];
}

void m_can_drv_init()
{
	m_can_little_endian[0] = 0;
	m_can_little_endian[1] = 0;
	m_can_little_endian[2] = 0;
	m_can_little_endian[3] = 0;
	m_can_little_endian[4] = 0;
	m_can[0] = 0;
	m_can[1] = &M_CAN_1;
	m_can[2] = &M_CAN_2;
	m_can[3] = &M_CAN_3;
	m_can[4] = &M_CAN_4;
	
	unsigned int m_can_id;
	unsigned int int_id;
	for(m_can_id = 0; m_can_id < 5; m_can_id++)
	{
		for(int_id = 0; int_id < 2; int_id++)
		{
			m_can_int_handlers[m_can_id][int_id] = m_can_int_handler_default;
		}
	}
	
	m_can_irq_vector_table[0][0] = 0;
	m_can_irq_vector_table[0][1] = 0;
	m_can_irq_vector_table[1][0] = 688;
	m_can_irq_vector_table[1][1] = 689;
	m_can_irq_vector_table[2][0] = 690;
	m_can_irq_vector_table[2][1] = 691;
	m_can_irq_vector_table[3][0] = 692;
	m_can_irq_vector_table[3][1] = 693;
	m_can_irq_vector_table[4][0] = 694;
	m_can_irq_vector_table[4][1] = 695;
}

void m_can_init(unsigned int m_can_id)
{
	switch(m_can_id)
	{
		case 1:
			intc_set_interrupt_handler(m_can_get_irq_vector(1, 0), &__m_can_1_int0__);
			intc_set_interrupt_handler(m_can_get_irq_vector(1, 1), &__m_can_1_int1__);
			break;
			
		case 2:
			intc_set_interrupt_handler(m_can_get_irq_vector(2, 0), &__m_can_2_int0__);
			intc_set_interrupt_handler(m_can_get_irq_vector(2, 1), &__m_can_2_int1__);
			break;
			
		case 3:
			intc_set_interrupt_handler(m_can_get_irq_vector(3, 0), &__m_can_3_int0__);
			intc_set_interrupt_handler(m_can_get_irq_vector(3, 1), &__m_can_3_int1__);
			break;
			
		case 4:
			intc_set_interrupt_handler(m_can_get_irq_vector(4, 0), &__m_can_4_int0__);
			intc_set_interrupt_handler(m_can_get_irq_vector(4, 1), &__m_can_4_int1__);
			break;
	}
	m_can_little_endian[m_can_id] = (m_can[m_can_id]->ENDN.R != 0x87654321);
}

void m_can_enter_init_mode(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_CHECK_REG(m_can_id, CCCR, INIT, 1);
}

void m_can_exit_init_mode(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_CHECK_REG(m_can_id, CCCR, INIT, 0);
}

void m_can_enable_configuration_change(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, CCCR, CCE, 1);
}

void m_can_disable_configuration_change(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, CCCR, CCE, 0);
}

void m_can_set_transmit_pause(unsigned int m_can_id, unsigned int transmit_pause)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, CCCR, TXP, transmit_pause);
}

unsigned int m_can_get_transmit_pause(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, CCCR, TXP);
}

void m_can_enable_test_mode(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, CCCR, TEST, 1);
}

void m_can_disable_test_mode(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, CCCR, TEST, 0);
}

void m_can_enable_loopback(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TEST, LBCK, 1);
}

void m_can_disable_loopback(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TEST, LBCK, 0);
}

void m_can_enable_timestamp_counter(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TSCC, TSS, 1);
}

void m_can_disable_timestamp_counter(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TSCC, TSS, 0);
}

void m_can_set_baud_rate_prescaler(unsigned m_can_id, unsigned int brp)
{
	if(brp > 0) M_CAN_READ_MODIFY_WRITE_REG(m_can_id, BTP, BRP, brp - 1);
}

void m_can_set_tseg1(unsigned m_can_id, unsigned int tseg1)
{
	if(tseg1 > 0) M_CAN_READ_MODIFY_WRITE_REG(m_can_id, BTP, TSEG1, tseg1 - 1);
}

void m_can_set_tseg2(unsigned m_can_id, unsigned int tseg2)
{
	if(tseg2 > 0) M_CAN_READ_MODIFY_WRITE_REG(m_can_id, BTP, TSEG2, tseg2 - 1);
}

void m_can_set_sjw(unsigned m_can_id, unsigned int sjw)
{
	if(sjw > 0) M_CAN_READ_MODIFY_WRITE_REG(m_can_id, BTP, SJW, sjw - 1);
}

void m_can_set_num_dedicated_tx_buffers(unsigned int m_can_id, unsigned int ndtb)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXBC, NDTB, ndtb);
}

void m_can_set_tx_fifo_queue_mode(unsigned int m_can_id, enum M_CAN_TX_FIFO_QUEUE_MODE tfqm)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXBC, TFQM, tfqm);
};

void m_can_set_tx_fifo_queue_size(unsigned int m_can_id, unsigned int tfqs)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXBC, TFQS, tfqs);
}

void m_can_set_tx_buffers_start_address(unsigned int m_can_id, uint32_t tbsa)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXBC, TBSA, tbsa >> 2);
}

void m_can_set_tx_buffer_data_field_size(unsigned int m_can_id, enum M_CAN_DATA_FIELD_SIZE tbds)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXESC, TBDS, tbds);
}

unsigned int m_can_get_tx_fifo_queue_put_index(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXFQS, TFQPI); 
}

unsigned int m_can_get_tx_fifo_get_index(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXFQS, TFGI); 
}

unsigned int m_can_get_tx_fifo_free_level(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXFQS, TFFL); 
}

void m_can_tx_fifo_push(unsigned int m_can_id, struct m_can_tx_buffer_element *elem)
{
	if(m_can_get_tx_fifo_free_level(m_can_id) != 0)
	{
		unsigned int fqpi = m_can_get_tx_fifo_queue_put_index(m_can_id);
		m_can_write_tx_buffer_element(m_can_id, fqpi, elem);
		m_can_add_tx_buffer_requests(m_can_id, 1 << fqpi);
	}
}

void m_can_add_tx_buffer_request(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	M_CAN_WRITE_REG(m_can_id, TXBAR, AR, (uint32_t) 1 << tx_buffer_element_idx);
}

void m_can_add_tx_buffer_requests(unsigned int m_can_id, uint32_t mask)
{
	M_CAN_WRITE_REG(m_can_id, TXBAR, AR, mask);
}

void m_can_cancel_tx_buffer_request(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	M_CAN_WRITE_REG(m_can_id, TXBCR, CR, (uint32_t) 1 << tx_buffer_element_idx);
}

void m_can_cancel_tx_buffer_requests(unsigned int m_can_id, uint32_t mask)
{
	M_CAN_WRITE_REG(m_can_id, TXBCR, CR, mask);
}

void m_can_enable_tx_buffer_transmission_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	m_can[m_can_id]->TXBTIE.R = (m_can[m_can_id]->TXBTIE.R & ~((uint32_t) 1 << tx_buffer_element_idx)) | ((uint32_t) 1 << tx_buffer_element_idx);
}

void m_can_disable_tx_buffer_transmission_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	m_can[m_can_id]->TXBTIE.R = (m_can[m_can_id]->TXBTIE.R & ~((uint32_t) 1 << tx_buffer_element_idx));
}

void m_can_enable_tx_buffer_cancellation_finished_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	m_can[m_can_id]->TXBCIE.R = (m_can[m_can_id]->TXBCIE.R & ~((uint32_t) 1 << tx_buffer_element_idx)) | ((uint32_t) 1 << tx_buffer_element_idx);
}

void m_can_disable_tx_buffer_cancellation_finished_interrupt(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	m_can[m_can_id]->TXBCIE.R = (m_can[m_can_id]->TXBCIE.R & ~((uint32_t) 1 << tx_buffer_element_idx));
}

int m_can_is_tx_buffer_has_pending_request(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	return (M_CAN_READ_REG(m_can_id, TXBRP, TRP) & ((uint32_t) 1 << tx_buffer_element_idx)) != 0;
}

int m_can_is_tx_buffer_transmission_occurred(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	return (M_CAN_READ_REG(m_can_id, TXBTO, TO) & ((uint32_t) 1 << tx_buffer_element_idx)) != 0;
}

int m_can_is_tx_buffer_cancellation_finished(unsigned int m_can_id, unsigned int tx_buffer_element_idx)
{
	return (M_CAN_READ_REG(m_can_id, TXBCF, CF) & ((uint32_t) 1 << tx_buffer_element_idx)) != 0;
}

void m_can_write_tx_buffer_element(unsigned int m_can_id, unsigned int tx_buffer_element_idx, struct m_can_tx_buffer_element *elem)
{
	uint32_t tbds = M_CAN_READ_REG(m_can_id, TXESC, TBDS);
	uint32_t tbsa = M_CAN_READ_REG(m_can_id, TXBC, TBSA);
	uint32_t tx_buffer_data_field_size = (tbds <= 4) ? (8 + (tbds * 4)) : (16 + ((tbds & 3) * 16));
	uint32_t tx_buffer_element_size = 2 + (tx_buffer_data_field_size / 4);
	volatile uint32_t *source = (volatile uint32_t *) elem;
	volatile uint32_t *dest = &SHARED_CAN_MESSAGE_RAM + tbsa + (tx_buffer_element_idx * tx_buffer_element_size);
	unsigned int i;
	for(i = 0; i < tx_buffer_element_size; dest++, source++, i++)
	{
		M_CAN_STORE32(m_can_id, dest, *source);
	}
}

void m_can_set_tx_event_fifo_watermark(unsigned int m_can_id, unsigned int level)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXEFC, EFWM, level);
}

void m_can_set_tx_event_fifo_size(unsigned int m_can_id, unsigned int efs)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXEFC, EFS, efs);
}

void m_can_set_tx_event_fifo_start_address(unsigned int m_can_id, uint32_t efsa)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, TXEFC, EFSA, efsa);
}

void m_can_acknowledge_tx_event_fifo(unsigned int m_can_id, unsigned int efai)
{
	M_CAN_WRITE_REG(m_can_id, TXEFA, EFAI, efai);
}

int m_can_has_tx_event_fifo_element_lost(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXEFS, TEFL);
}

int m_can_is_tx_event_fifo_full(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXEFS, EFF);
}

unsigned int m_can_get_tx_event_fifo_put_index(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXEFS, EFPI);
}

unsigned int m_can_get_tx_event_fifo_get_index(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXEFS, EFGI);
}

unsigned int m_can_get_tx_event_fill_level(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, TXEFS, EFFL);
}

void m_can_read_tx_event_fifo_element(unsigned int m_can_id, struct m_can_tx_event_fifo_element *elem)
{
	uint32_t efsa = M_CAN_READ_REG(m_can_id, TXEFC, EFSA);
	unsigned int tx_event_fifo_get_index = m_can_get_tx_event_fifo_get_index(m_can_id);
	volatile uint32_t *dest = (volatile uint32_t *) elem;
	volatile uint32_t *source = &SHARED_CAN_MESSAGE_RAM + efsa + (tx_event_fifo_get_index * 2);
	dest[0] = M_CAN_LOAD32(m_can_id, source);
	dest[1] = M_CAN_LOAD32(m_can_id, source + 1);
}

void m_can_set_rx_fifo_operation_mode(unsigned int m_can_id, unsigned int fifo_id, enum M_CAN_RX_FIFO_OPERATION_MODE fom)
{
	switch(fifo_id)
	{
		case 0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF0C, F0OM, fom); break;
		case 1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF1C, F1OM, fom); break;
	}
}

void m_can_set_rx_fifo_watermark(unsigned int m_can_id, unsigned int fifo_id, unsigned int fwm)
{
	switch(fifo_id)
	{
		case 0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF0C, F0WM, fwm); break;
		case 1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF1C, F1WM, fwm); break;
	}
}

void m_can_set_rx_fifo_size(unsigned int m_can_id, unsigned int fifo_id, unsigned int fs)
{
	switch(fifo_id)
	{
		case 0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF0C, F0S, fs); break;
		case 1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF1C, F1S, fs); break;
	}
}

void m_can_set_rx_fifo_start_address(unsigned int m_can_id, unsigned int fifo_id, uint32_t fsa)
{
	switch(fifo_id)
	{
		case 0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF0C, F0SA, fsa); break;
		case 1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXF1C, F1SA, fsa); break;
	}
}

void m_can_acknowledge_rx_fifo(unsigned int m_can_id, unsigned int fifo_id, unsigned int fai)
{
	switch(fifo_id)
	{
		case 0: M_CAN_WRITE_REG(m_can_id, RXF0A, F0AI, fai); break;
		case 1: M_CAN_WRITE_REG(m_can_id, RXF1A, F1AI, fai); break;
	}
}

int m_can_is_rx_fifo_full(unsigned int m_can_id, unsigned int fifo_id)
{
	switch(fifo_id)
	{
		case 0: return M_CAN_READ_REG(m_can_id, RXF0S, F0F);
		case 1: return M_CAN_READ_REG(m_can_id, RXF1S, F1F);
	}
	return 0;
}

unsigned int m_can_get_rx_fifo_fill_level(unsigned int m_can_id, unsigned int fifo_id)
{
	switch(fifo_id)
	{
		case 0: return M_CAN_READ_REG(m_can_id, RXF0S, F0FL);
		case 1: return M_CAN_READ_REG(m_can_id, RXF1S, F1FL);
	}
	return 0;
}

unsigned int m_can_get_rx_fifo_put_index(unsigned int m_can_id, unsigned int fifo_id)
{
	switch(fifo_id)
	{
		case 0: return M_CAN_READ_REG(m_can_id, RXF0S, F0PI);
		case 1: return M_CAN_READ_REG(m_can_id, RXF1S, F1PI);
	}
	return 0;
}

unsigned int m_can_get_rx_fifo_get_index(unsigned int m_can_id, unsigned int fifo_id)
{
	switch(fifo_id)
	{
		case 0: return M_CAN_READ_REG(m_can_id, RXF0S, F0GI);
		case 1: return M_CAN_READ_REG(m_can_id, RXF1S, F1GI);
	}
	return 0;
}

void m_can_set_rx_fifo_data_field_size(unsigned int m_can_id, unsigned int fifo_id, enum M_CAN_DATA_FIELD_SIZE fds)
{
	switch(fifo_id)
	{
		case 0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXESC, F0DS, fds); break;
		case 1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXESC, F1DS, fds); break;
	}
}

void m_can_set_rx_buffer_start_address(unsigned int m_can_id, uint32_t rbsa)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXBC, RBSA, rbsa >> 2);
}

void m_can_set_rx_buffer_data_field_size(unsigned int m_can_id, enum M_CAN_DATA_FIELD_SIZE rbds)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, RXESC, RBDS, rbds);
}

void m_can_read_rx_fifo(unsigned int m_can_id, unsigned int fifo_id, struct m_can_rx_buffer_fifo_element *elem)
{
	uint32_t fds = (fifo_id == 0) ? M_CAN_READ_REG(m_can_id, RXESC, F0DS) : M_CAN_READ_REG(m_can_id, RXESC, F1DS);
	uint32_t fsa = (fifo_id == 0) ? M_CAN_READ_REG(m_can_id, RXF0C, F0SA) : M_CAN_READ_REG(m_can_id, RXF1C, F1SA);
	uint32_t rx_fifo_data_field_size = (fds <= 4) ? (8 + (fds * 4)) : (16 + ((fds & 3) * 16));
	uint32_t rx_fifo_element_size = 2 + (rx_fifo_data_field_size / 4);
	unsigned int rx_fifo_get_index = m_can_get_rx_fifo_get_index(m_can_id, fifo_id);
	volatile uint32_t *dest = (volatile uint32_t *) elem;
	volatile uint32_t *source = &SHARED_CAN_MESSAGE_RAM + fsa + (rx_fifo_get_index * rx_fifo_element_size);
	unsigned int i;
	for(i = 0; i < rx_fifo_element_size; dest++, source++, i++)
	{
		*dest = M_CAN_LOAD32(m_can_id, source);
	}
}

void m_can_read_rx_buffer(unsigned int m_can_id, unsigned int rx_buffer_element_index, struct m_can_rx_buffer_fifo_element *elem)
{
	uint32_t rbds = M_CAN_READ_REG(m_can_id, RXESC, RBDS);
	uint32_t rbsa = M_CAN_READ_REG(m_can_id, RXBC, RBSA);
	uint32_t rx_buffer_data_field_size = (rbds <= 4) ? (8 + (rbds * 4)) : (16 + ((rbds & 3) * 16));
	uint32_t rx_buffer_element_size = 2 + (rx_buffer_data_field_size / 4);
	volatile uint32_t *dest = (volatile uint32_t *) elem;
	volatile uint32_t *source = &SHARED_CAN_MESSAGE_RAM + rbsa + (rx_buffer_element_index * rx_buffer_element_size);
	unsigned int i;
	for(i = 0; i < rx_buffer_element_size; dest++, source++, i++)
	{
		*dest = M_CAN_LOAD32(m_can_id, source);
	}
}

void m_can_pop_rx_fifo(unsigned int m_can_id, unsigned int fifo_id, struct m_can_rx_buffer_fifo_element *elem)
{
	if(m_can_get_rx_fifo_fill_level(m_can_id, fifo_id) != 0)
	{
		unsigned int rx_fifo_get_index = m_can_get_rx_fifo_get_index(m_can_id, fifo_id);
		
		m_can_read_rx_fifo(m_can_id, fifo_id, elem);
		m_can_acknowledge_rx_fifo(m_can_id, fifo_id, rx_fifo_get_index);
	}
}

void m_can_accept_non_matching_frame_standard(unsigned int m_can_id, unsigned int fifo_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, ANFS, fifo_id);
}

void m_can_reject_non_matching_frame_standard(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, ANFS, 2);
}

void m_can_accept_non_matching_frame_extended(unsigned int m_can_id, unsigned int fifo_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, ANFE, fifo_id);
}

void m_can_reject_non_matching_frame_extended(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, ANFE, 2);
}

void m_can_standard_filter_remote_frame(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, RRFS, 0);
}

void m_can_reject_remote_standard_frame(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, RRFS, 1);
}

void m_can_filter_remote_extended_frame(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, RRFE, 0);
}

void m_can_reject_remote_extended_frame(unsigned int m_can_id)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, GFC, RRFE, 1);
}

void m_can_set_standard_filter_list_size(unsigned int m_can_id, unsigned int lss)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, SIDFC, LSS, lss);
}

void m_can_set_extended_filter_list_size(unsigned int m_can_id, unsigned int lse)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, XIDFC, LSE, lse);
}

void m_can_set_standard_filter_list_start_address(unsigned int m_can_id, uint32_t flsa)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, SIDFC, FLSSA, flsa);
}

void m_can_set_extended_filter_list_start_address(unsigned int m_can_id, uint32_t flsa)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, XIDFC, FLESA, flsa);
}

void m_can_set_extended_id_mask(unsigned int m_can_id, uint32_t eidm)
{
	M_CAN_READ_MODIFY_WRITE_REG(m_can_id, XIDAM, EIDM, eidm);
}

void m_can_write_standard_filter(unsigned int m_can_id, unsigned int filter_element_index, struct m_can_standard_message_id_filter_element *elem)
{
	uint32_t flssa = M_CAN_READ_REG(m_can_id, SIDFC, FLSSA);
	volatile uint32_t *source = (volatile uint32_t *) elem;
	volatile uint32_t *dest = &SHARED_CAN_MESSAGE_RAM + flssa + (filter_element_index * 1);
	M_CAN_STORE32(m_can_id, dest, *source);
}

void m_can_write_extended_filter(unsigned int m_can_id, unsigned int filter_element_index, struct m_can_extended_message_id_filter_element *elem)
{
	uint32_t flesa = M_CAN_READ_REG(m_can_id, XIDFC, FLESA);
	volatile uint32_t *source = (volatile uint32_t *) elem;
	volatile uint32_t *dest = &SHARED_CAN_MESSAGE_RAM + flesa + (filter_element_index * 2);
	M_CAN_STORE32(m_can_id, dest, source[0]);
	M_CAN_STORE32(m_can_id, dest + 1, source[1]);
}

uint32_t m_can_new_data1(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, NDAT1, ND1);
}

uint32_t m_can_new_data2(unsigned int m_can_id)
{
	return M_CAN_READ_REG(m_can_id, NDAT2, ND2);
}

int m_can_new_data(unsigned int m_can_id, unsigned int rx_buffer_fifo_element_index)
{
	if(rx_buffer_fifo_element_index >= 64) return 0;
	return ((rx_buffer_fifo_element_index < 32) ? m_can_new_data1(m_can_id) : m_can_new_data2(m_can_id)) & ((uint32_t) 1 << (rx_buffer_fifo_element_index % 32));
}

void m_can_clear_new_data(unsigned int m_can_id, unsigned int rx_buffer_fifo_element_index)
{
	if(rx_buffer_fifo_element_index >= 64) return;
	if(rx_buffer_fifo_element_index < 32)
	{
		M_CAN_WRITE_REG(m_can_id, NDAT1, ND1, (uint32_t) 1 << rx_buffer_fifo_element_index);
	}
	else
	{
		M_CAN_WRITE_REG(m_can_id, NDAT2, ND2, (uint32_t) 1 << (rx_buffer_fifo_element_index % 32));
	}
}

void m_can_enable_interrupt(unsigned int m_can_id, enum M_CAN_INT m_can_int)
{
	m_can[m_can_id]->IE.R = m_can[m_can_id]->IE.R | ((uint32_t) 1 << m_can_int);
}

void m_can_disable_interrupt(unsigned int m_can_id, enum M_CAN_INT m_can_int)
{
	m_can[m_can_id]->IE.R = m_can[m_can_id]->IE.R & ~((uint32_t) 1 << m_can_int);
}

void m_can_select_interrupt_line(unsigned int m_can_id, enum M_CAN_INT m_can_int, enum M_CAN_INT_LINE m_can_int_line)
{
	m_can[m_can_id]->ILS.R = (m_can[m_can_id]->ILS.R & ~((uint32_t) 1 << m_can_int)) | ((m_can_int_line ? 0x1 : 0x0) << m_can_int);
}

void m_can_enable_interrupt_line(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line)
{
	switch(m_can_int_line)
	{
		case M_CAN_INT0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, ILE, EINT0, 1); break;
		case M_CAN_INT1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, ILE, EINT1, 1); break;
	}
}

void m_can_disable_interrupt_line(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line)
{
	switch(m_can_int_line)
	{
		case M_CAN_INT0: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, ILE, EINT0, 0); break;
		case M_CAN_INT1: M_CAN_READ_MODIFY_WRITE_REG(m_can_id, ILE, EINT1, 0); break;
	}
}

int m_can_get_interrupt_flag(unsigned int m_can_id, enum M_CAN_INT m_can_int)
{
	return (m_can[m_can_id]->IR.R & ((uint32_t) 1 << m_can_int)) != 0;
}

void m_can_clear_interrupt_flag(unsigned int m_can_id, enum M_CAN_INT m_can_int)
{
	m_can[m_can_id]->IR.R = ((uint32_t) 1 << m_can_int);
}

m_can_int_handler_t m_can_set_interrupt_handler(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, m_can_int_handler_t m_can_int_handler)
{
	m_can_int_handler_t old_m_can_int_handler = m_can_int_handlers[m_can_id][m_can_int_line];
	m_can_int_handlers[m_can_id][m_can_int_line] = m_can_int_handler;
	return old_m_can_int_handler;
}

void m_can_set_irq_priority(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, unsigned int priority)
{
	intc_set_irq_priority(m_can_get_irq_vector(m_can_id, m_can_int_line), priority);
}

void m_can_select_irq_for_processor(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, unsigned int prc_num)
{
	intc_select_irq_for_processor(m_can_get_irq_vector(m_can_id, m_can_int_line), prc_num);
}

void m_can_deselect_irq_for_processor(unsigned int m_can_id, enum M_CAN_INT_LINE m_can_int_line, unsigned int prc_num)
{
	intc_deselect_irq_for_processor(m_can_get_irq_vector(m_can_id, m_can_int_line), prc_num);
}

static void __m_can_1_int0__()
{
	m_can_int_handlers[1][0](1, M_CAN_INT0);
}

static void __m_can_1_int1__()
{
	m_can_int_handlers[1][1](1, M_CAN_INT1);
}

static void __m_can_2_int0__()
{
	m_can_int_handlers[2][0](2, M_CAN_INT0);
}

static void __m_can_2_int1__()
{
	m_can_int_handlers[2][1](2, M_CAN_INT1);
}

static void __m_can_3_int0__()
{
	m_can_int_handlers[3][0](3, M_CAN_INT0);
}

static void __m_can_3_int1__()
{
	m_can_int_handlers[3][1](3, M_CAN_INT1);
}

static void __m_can_4_int0__()
{
	m_can_int_handlers[4][0](4, M_CAN_INT0);
}

static void __m_can_4_int1__()
{
	m_can_int_handlers[4][1](4, M_CAN_INT1);
}
