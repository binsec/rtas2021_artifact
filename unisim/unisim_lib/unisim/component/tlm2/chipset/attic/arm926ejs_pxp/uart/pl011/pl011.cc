/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/uart/pl011/pl011.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#else

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#endif

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace uart {
namespace pl011 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

PL011 ::
PL011(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, bus_target_socket("bus_target_socket")
	, uartrxintr("uartrxintr")
	, uarttxintr("uarttxintr")
	, uartrtintr("uartrtintr")
	, uartmsintr("uartmsintr")
	, uarteintr("uarteintr")
	, uartintr("uartintr")
	, sock(-1)
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of uart.")
	, pclk(31250000)
	, param_pclk("refclk", this, pclk,
			"Reference clock (PCLK) period in picoseconds.")
	, uartclk(41667)
	, param_uartclk("uartclk", this, uartclk,
			"External UART clock (UARTCLK) period in picoseconds.")
	, enable_logger(false)
	, param_enable_logger("enable-logger", this, enable_logger,
			"Enable logger output for messages.")
	, enable_telnet(false)
	, param_enable_telnet("enable-telnet", this, enable_telnet,
			"Enable the telnet connection.")
	, tcp_port(1234)
	, param_tcp_port("tcp-port", this, tcp_port,
			"TCP port used for the Telnet connection.")
	, logger(*this)
{
	bus_target_socket.register_nb_transport_fw(this,
			&PL011::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&PL011::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&PL011::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&PL011::bus_target_transport_dbg);

	uartrxintr.initialize(false);
	uarttxintr.initialize(false);
	uartrtintr.initialize(false);
	uartmsintr.initialize(false);
	uartintr.initialize(false);

	// init the registers values
	memset(regs, 0, 4096);
	uint16_t val16;
	val16 = Host2LittleEndian((uint16_t)0x0300);
	memcpy(&regs[0x30], &val16, sizeof(val16));
	val16 = Host2LittleEndian((uint16_t)0x090);
	memcpy(&regs[0x18], &val16, sizeof(val16));
}

PL011 ::
~PL011()
{
	if ( sock >= 0 )
	{
		std::string packet("Closing connection. Bye!");
		TelnetPutPacket(packet);
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(sock);
#else
		close(sock);
#endif
		sock = -1;
	}
}

bool
PL011 ::
TelnetPutChar(uint8_t ch)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	int r = send(sock, &ch, 1, 0);
	if ( r == 0 || r == SOCKET_ERROR )
#else
	ssize_t r = write(sock, &ch, 1);
	if ( r <= 0 )
#endif
	{
		logger << DebugError << "can't write into socket" << EndDebugError;
		return false;
	}
	return true;
}

bool
PL011 ::
TransmitChar(uint8_t ch)
{
	if ( enable_telnet )
	{
		return TelnetPutChar(ch);
	}
	if ( enable_logger )
	{
		logger << DebugInfo
			<< "Sending char: '"
			<< (char)ch
			<< "'"
			<< EndDebugInfo;
		return true;
	}
	return true;
}

void
PL011 ::
TelnetPutPacket(std::string packet)
{
	const char *ch = packet.c_str();
	bool success = true;

	for ( unsigned int i = 0; i < packet.length(); i++)
	{
		success &= TelnetPutChar(ch[i]);
	}
	if ( !success )
	{
		logger << DebugError
			<< "Could not successfully send the requested string."
			<< " The requested string is:" << std::endl
			<< packet
			<< EndDebugError;
	}
}

bool 
PL011 ::
EndSetup()
{
	if ( enable_telnet )
	{
		struct sockaddr_in addr;
		int server_sock;
		int on = 1;

		server_sock = socket(AF_INET, SOCK_STREAM, 0);

		if ( server_sock < 0 )
		{
			logger << DebugError << "Socket failed" << EndDebugError;
			return false;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(tcp_port);
		addr.sin_addr.s_addr = INADDR_ANY;
		if ( setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0 )
		{
			logger << DebugWarning << "Could not set socket reuse address option"
				<< EndDebugWarning;
		}
		if ( bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0 )
		{
			logger << DebugError << "Bind failed" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}

		if ( listen(server_sock, 1) )
		{
			logger << DebugError << "listen failed" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		int addr_len;
#else
		socklen_t addr_len;
#endif

		logger << DebugInfo << "Listening on TCP port " << tcp_port << EndDebugInfo;

		addr_len = sizeof(addr);
		sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);

		if ( sock < 0 )
		{
			logger << DebugError << "accept failed" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}

		logger << DebugInfo << "Connection with Telnet client established" 
			<< EndDebugInfo;

    /* set short latency */
    int opt = 1;
    if ( setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, sizeof(opt))
				< 0 )
		{
			logger << DebugWarning 
				<< "setsockopt failed requesting short latency" 
				<< EndDebugWarning;
		}


#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		u_long NonBlock = 1;
		if ( ioctlsocket(sock, FIONBIO, &NonBlock) != 0 )
		{
			logger << DebugError << "ioctlsocket failed" << EndDebugError;
			closesocket(server_sock);
			closesocket(sock);
			sock = -1;
			return false;
		}
#else
		int socket_flag = fcntl(sock, F_GETFL, 0);

		if(socket_flag < 0)
		{
			logger << DebugError << "fcntl failed" << EndDebugError;
			close(server_sock);
			close(sock);
			sock = -1;
			return false;
		}

		/* Ask for non-blocking reads on socket */
		if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
		{
			logger << DebugError << "fcntl failed" << EndDebugError;
			close(server_sock);
			close(sock);
			sock = -1;
			return false;
		}
#endif

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
	}

	if ( enable_logger )
	{
		logger << DebugInfo
			<< "UART message logger output activated."
			<< EndDebugInfo;
	}

	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
PL011 ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

void 
PL011 ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	if ( is_read )
	{
		handled = true;
		memcpy(data, &regs[addr], size);
		trans.set_response_status(tlm::TLM_OK_RESPONSE);
	}
	else // writing
	{
		uint32_t prev_value, new_value;
		memcpy(&prev_value, &regs[addr & ~(uint32_t)0x03], 4);
		memcpy(&regs[addr], data, size);
		memcpy(&new_value, &regs[addr & ~(uint32_t)0x03], 4);
		prev_value = LittleEndian2Host(prev_value);
		new_value = LittleEndian2Host(new_value);
		if ( (addr & ~(uint32_t)0x03) == 0x00 )
		{
			// writing into UARTDR
			handled = true;
			// check that the transmit is enabled
			uint32_t cr;
			memcpy(&cr, &regs[0x30], sizeof(cr));
			cr = LittleEndian2Host(cr);
			if ( cr & (uint32_t)0x0100 )
			{
				uint8_t ch = new_value & 0x0ff;
				TransmitChar(ch);
			}
			else
			{
				logger << DebugWarning
					<< "Writing into UARTDR while transmition is not enabled."
					<< std::endl
					<< "UARTCR = 0x" << std::hex
					<< cr << std::dec
					<< EndDebugWarning;
			}
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else if ( (addr & ~(uint32_t)0x03) == 0x24 )
		{
			// writing into UARTIBRD
			handled = true;
			logger << DebugWarning
				<< "Modified UART Integer baud rate divisor register "
				<< "but the module is not completed. Errors may occur." << endl
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value << std::dec
				<< ")"
				<< EndDebugWarning;
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else if ( (addr & ~(uint32_t)0x03) == 0x28 )
		{
			// writing into UARTFBRD
			handled = true;
			logger << DebugWarning
				<< "Modified UART Fractional baud rate divisor register "
				<< "but the module is not completed. Errors may occur." << endl
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value << std::dec
				<< ")"
				<< EndDebugWarning;
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else if ( (addr & ~(uint32_t)0x03) == 0x2c )
		{
			// writing into UARTLCR_H
			handled = true;
			logger << DebugWarning
				<< "Modified UART Line control (HIGH byte) register "
				<< "but the module is not completed. Errors may occur." << endl
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value << std::dec
				<< ")"
				<< EndDebugWarning;
			// TODO: set read trigger ?
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else if ( (addr & ~(uint32_t)0x03) == 0x30 )
		{
			// writing into UARTCR
			handled = true;
			logger << DebugWarning
				<< "Modified UART Control register but the module "
				<< "is not completed. Errors may occur." << endl
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value << std::dec
				<< ")"
				<< EndDebugWarning;
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else 
		{
			logger << DebugError
				<< "Writing 0x"
				<< std::hex << new_value
				<< " (previous value 0x"
				<< prev_value << std::dec
				<< ")"
				<< EndDebugError;
			trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		}
	}
	if ( !handled )
		assert("TODO" == 0);
}

bool 
PL011 ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	assert("TODO" == 0);
	return false;
}

unsigned int 
PL011 ::
bus_target_transport_dbg(transaction_type &trans)
{
	assert("TODO" == 0);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

} // end of namespace pl011
} // end of namespace uart 
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


