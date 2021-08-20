/*
 *  Copyright (c) 2016,
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
 * Author: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_KERNEL_TLM2_TLM_SPI_HH__
#define __UNISIM_KERNEL_TLM2_TLM_SPI_HH__

#include <tlm>
#include <iostream>
#include <string.h>

namespace unisim {
namespace kernel {
namespace tlm2 {

enum tlm_spi_status
{
	TLM_SPI_OK,
	TLM_SPI_ERROR
};

enum tlm_spi_sync_enum
{
	TLM_SPI_ACCEPTED,
	TLM_SPI_COMPLETED
};

enum tlm_spi_com_mode
{
	TLM_SPI_FULL_DUPLEX,
	TLM_SPI_HALF_DUPLEX
};

class tlm_spi_payload
{
public:
	tlm_spi_payload()
		: com_mode(TLM_SPI_FULL_DUPLEX)
		, tx_width(0)
		, rx_width(0)
		, data_ptr(0)
		, tx_data_bit_length(0)
		, rx_data_bit_length(0)
		, clock(sc_core::SC_ZERO_TIME)
		, status(TLM_SPI_OK)
		, mm(0)
		, ref_count(0)
	{
	}
	
	virtual ~tlm_spi_payload() {}

	void set_com_mode(tlm_spi_com_mode v) { com_mode = v; }
	void set_data_ptr(unsigned char *v) { data_ptr = v; }
	void set_tx_data_bit_length(unsigned int v) { tx_data_bit_length = v; }
	void set_rx_data_bit_length(unsigned int v) { rx_data_bit_length = v; }
	void set_tx_width(unsigned int v) { tx_width = v; }
	void set_rx_width(unsigned int v) { rx_width = v; }
	void set_clock(const sc_core::sc_time& v) { clock = v; }
	void set_status(tlm_spi_status v) { status = v; }
	
	tlm_spi_com_mode get_com_mode() const { return com_mode; }
	unsigned char *get_data_ptr() const { return data_ptr; }
	unsigned char *get_tx_data_ptr() const { return data_ptr; }
	unsigned char *get_rx_data_ptr() const { return data_ptr + ((tx_data_bit_length + 7) / 8); }
	unsigned int get_tx_data_bit_length() const { return tx_data_bit_length; }
	unsigned int get_rx_data_bit_length() const { return rx_data_bit_length; }
	unsigned int get_tx_data_length() const { return (get_tx_data_bit_length() + 7) / 8; }
	unsigned int get_rx_data_length() const { return (get_rx_data_bit_length() + 7) / 8; }
	unsigned int get_data_bit_length() const { return get_tx_data_bit_length() + get_rx_data_bit_length(); }
	unsigned int get_data_length() const { return get_tx_data_length() + get_rx_data_length(); }
	unsigned int get_tx_width() const { return tx_width; }
	unsigned int get_rx_width() const { return rx_width; }
	unsigned int get_bus_width() const { return tx_width + rx_width; } // should be equal to BUSWIDTH
	const sc_core::sc_time& get_clock() const { return clock; }
	tlm_spi_status get_status() const { return status; }
	
	bool full_duplex() const { return com_mode == TLM_SPI_FULL_DUPLEX; }
	bool half_duplex() const { return com_mode == TLM_SPI_HALF_DUPLEX; }
	
	unsigned int get_tx_cycles() const { return (get_tx_data_bit_length() + tx_width - 1) / tx_width; }
	unsigned int get_rx_cycles() const { return (get_rx_data_bit_length() + rx_width - 1) / rx_width; }
	unsigned int get_cycles() const { return (com_mode == TLM_SPI_FULL_DUPLEX) ? std::max<unsigned int>(get_tx_cycles(), get_rx_cycles()) : get_tx_cycles() + get_rx_cycles(); }

	sc_core::sc_time get_tx_time() const { return get_tx_cycles() * clock; }
	sc_core::sc_time get_rx_time() const { return get_rx_cycles() * clock; }
	sc_core::sc_time get_time() const { return get_cycles() * clock; }
	
	void copy_from(const tlm_spi_payload& other)
	{
		com_mode = other.com_mode;
		tx_data_bit_length = other.tx_data_bit_length;
		rx_data_bit_length = other.rx_data_bit_length;
		tx_width = other.tx_width;
		rx_width = other.rx_width;
		clock = other.clock;
		status = other.status;
		if(data_ptr && other.data_ptr)
		{
			memcpy(data_ptr, other.data_ptr, other.get_data_length());
		}
	}
	
	void set_mm(tlm::tlm_mm_interface *_mm) { mm = _mm; }
	bool has_mm() const { return mm != 0; }
	unsigned int get_ref_count() const { return ref_count; }
	void acquire() { assert(mm != 0); ref_count++; }
	void release() { assert((mm != 0) && (ref_count > 0)); if(--ref_count == 0) mm->free(reinterpret_cast<tlm::tlm_generic_payload *>(this)); }
	void free_all_extensions() {}
private:
	/*         _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
	     CLOCK  \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_
	  
	  
	                      === HALF DUPLEX COMMUNICATION ===
	              ______________________________________________________
	     SS/CS __/                                                      \_____
	              ________________________                   
	   MOSI/TX   /    MASTER EMISSION     \     |    TX
	             \________________________/     |   Width
	                                        ____________________________
	   MISO/RX                 RX   |      /     MASTER RECEPTION       \
	                          Width |      \____________________________/
	  
	             <------- tx_cycles ------><-------- rx_cycles --------->

	   
	  
	                      === FULL DUPLEX COMMUNICATION ===
	              ______________________________________________________
	     SS/CS __/                                                      \_____
	              ______________________________________________________ 
	   MOSI/TX   /                  MASTER EMISSION                     \   |   TX
	             \______________________________________________________/   |  Width
	              ______________________________________________________         
	   MISO/RX   /                  MASTER RECEPTION                    \   |   RX
	             \______________________________________________________/   |  Width
	  
	             <--------------- tx_cycles = rx_cycles ---------------->
	  
	  
	                            === DATA BITS ===
	                            
	                         array index   DATA ARRAY
	                                0 -> +------------+  
	                                     |            |
	                                     |   MASTER   |
	                                     |  EMISSION  |
	                                     |            |
	     (tx_data_bit_length + 7) / 8 -> +------------+
	                                     |            |
	                                     |   MASTER   |
	                                     |  RECEPTION |
	     (tx_data_bit_length + 7) / 8    |            |
	   + (rx_data_bit_length + 7) / 8 -> +------------+
	   
	*/
	
	tlm_spi_com_mode com_mode;       // communication modes:
	                                 //   - half duplex: master emission followed by master reception
	                                 //   - full duplex: simultaneous master emission and reception
	unsigned int tx_width;
	unsigned int rx_width;
	unsigned char *data_ptr;         // pointer to data array, MSB first (byte aligned master request data bits followed by by aligned slave response data bits)
	unsigned int tx_data_bit_length; // number of bits at the beginning of data array for the emission; also for timing purpose in combination width TX width
	unsigned int rx_data_bit_length; // number of bits at the end of data array for the reception; also for timing purpose in combination width RX width
	sc_core::sc_time clock;          // master clock period
	tlm_spi_status status;           // status for programmatic error handling
	tlm::tlm_mm_interface *mm;
	unsigned int ref_count;
};

struct tlm_spi_protocol_types
{
	typedef tlm_spi_payload tlm_payload_type;
};

template <typename TYPES = tlm_spi_protocol_types>
class tlm_spi_nb_send_if : public virtual sc_core::sc_interface
{
public:
	virtual tlm_spi_sync_enum spi_nb_send(tlm_spi_protocol_types::tlm_payload_type&) = 0; // non-blocking master emission/slave reception
};

template <typename TYPES = tlm_spi_protocol_types>
class tlm_spi_b_send_if : public virtual sc_core::sc_interface
{
public:
	virtual void spi_b_send(tlm_spi_protocol_types::tlm_payload_type&) = 0; // blocking master emission/slave reception
};

template <typename TYPES = tlm_spi_protocol_types>
class tlm_spi_fw_if
	: public tlm_spi_b_send_if<TYPES>
	, public tlm_spi_nb_send_if<TYPES>
{
};

template <typename TYPES = tlm_spi_protocol_types>
class tlm_spi_bw_if : public virtual sc_core::sc_interface
{
public:
	virtual void spi_nb_receive(tlm_spi_protocol_types::tlm_payload_type&) = 0; // master reception/slave emission
};

template <unsigned int BUSWIDTH = 1, typename TYPES = tlm_spi_protocol_types, int N = 1, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class tlm_spi_master_socket : public tlm::tlm_base_initiator_socket<BUSWIDTH, tlm_spi_fw_if<TYPES>, tlm_spi_bw_if<TYPES>, N, POL>
{
public:
	tlm_spi_master_socket() : tlm::tlm_base_initiator_socket<BUSWIDTH, tlm_spi_fw_if<TYPES>, tlm_spi_bw_if<TYPES>, N, POL>() {}
	explicit tlm_spi_master_socket(const char *name) : tlm::tlm_base_initiator_socket<BUSWIDTH, tlm_spi_fw_if<TYPES>, tlm_spi_bw_if<TYPES>, N, POL>(name) {}
	virtual const char *kind() const { return "tlm_spi_master_socket"; }
};

template <unsigned int BUSWIDTH = 1, typename TYPES = tlm_spi_protocol_types, int N = 1, sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class tlm_spi_slave_socket : public tlm::tlm_base_target_socket<BUSWIDTH, tlm_spi_fw_if<TYPES>, tlm_spi_bw_if<TYPES>, N, POL>
{
public:
	tlm_spi_slave_socket() : tlm::tlm_base_target_socket<BUSWIDTH, tlm_spi_fw_if<TYPES>, tlm_spi_bw_if<TYPES>, N, POL>() {}
	explicit tlm_spi_slave_socket(const char *name) : tlm::tlm_base_target_socket<BUSWIDTH, tlm_spi_fw_if<TYPES>, tlm_spi_bw_if<TYPES>, N, POL>(name) {}
	virtual const char *kind() const { return "tlm_spi_slave_socket"; }
};

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif
