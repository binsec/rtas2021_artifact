/*
 *  Copyright (c) 2011,
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
 
#include <unisim/service/telnet/telnet.hh>

#include <cerrno>
#include <cstring>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>

#endif

namespace unisim {
namespace service {
namespace telnet {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

// Telnet commands
const uint8_t IAC = 255;
const uint8_t DONT = 254;
const uint8_t DO = 253;
const uint8_t WONT = 252;
const uint8_t WILL = 251;
const uint8_t SB = 250;
const uint8_t SE = 240;

// Telnet options
const uint8_t BINARY = 0;
const uint8_t ECHO = 1;
const uint8_t SUPPRESS_GO_AHEAD = 3;
const uint8_t LINEMODE = 34;


Telnet::Telnet(const char *name, Object *parent)
	: Object(name, parent, "This service provides character I/O over the TCP/IP telnet protocol")
	, Service<CharIO>(name, parent)
	, char_io_export("char-io-export", this)
	, logger(*this)
	, verbose(false)
	, enable_negotiation(true)
	, filter_null_character(false)
	, filter_line_feed(false)
	, telnet_tcp_port(23)
	, telnet_sock(-1)
	, state(0)
	, sb_opt(0)
	, sb_params_vec()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_telnet_tcp_port("telnet-tcp-port", this, telnet_tcp_port, "TCP/IP port of telnet")
	, param_enable_negotiation("enable-negotiation", this, enable_negotiation, "Enable negotiation with client")
	, param_filter_null_character("filter-null-character", this, filter_null_character, "Whether to filter null character")
	, param_filter_line_feed("filter-line-feed", this, filter_line_feed, "Whether to filter line feed")
	, telnet_input_buffer_size(0)
	, telnet_input_buffer_index(0)
	, telnet_output_buffer_size(0)

{
	param_telnet_tcp_port.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
}

Telnet::~Telnet()
{
	if(telnet_sock >= 0)
	{
		TelnetFlushOutput();
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(telnet_sock);
#else
		close(telnet_sock);
#endif
	}
}

bool Telnet::EndSetup()
{
	if(telnet_sock >= 0) return true;
	
	struct sockaddr_in addr;
	int server_sock;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if(server_sock < 0)
	{
		logger << DebugError << "socket failed" << EndDebugError;
		return false;
	}

    /* ask for reusing TCP port */
    int opt = 1;
    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt failed requesting port reuse" << EndDebugWarning;
		}
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(telnet_tcp_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		logger << DebugError << "Bind failed. TCP Port #" << telnet_tcp_port << " may be already in use. Please specify another port in " << param_telnet_tcp_port.GetName() << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(listen(server_sock, 1))
	{
		logger << DebugError << "Listen failed" << EndDebugError;
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

	logger << DebugInfo << "Listening on TCP port " << telnet_tcp_port << EndDebugInfo;
	addr_len = sizeof(addr);
	telnet_sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);

	if(telnet_sock < 0)
	{
		logger << DebugError << "accept failed" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(IsVerbose())
	{
		logger << DebugInfo << "Connection with telnet client established" << EndDebugInfo;
	}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	u_long NonBlock = 1;
	if(ioctlsocket(telnet_sock, FIONBIO, &NonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket failed" << EndDebugError;
		closesocket(server_sock);
		closesocket(telnet_sock);
		telnet_sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(telnet_sock, F_GETFL, 0);

	if(socket_flag < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(server_sock);
		close(telnet_sock);
		telnet_sock = -1;
		return false;
	}

	/* Ask for non-blocking reads on socket */
	if(fcntl(telnet_sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(server_sock);
		close(telnet_sock);
		telnet_sock = -1;
		return false;
	}
#endif

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	closesocket(server_sock);
#else
	close(server_sock);
#endif
	if(enable_negotiation)
	{
		TelnetPut(IAC);
		TelnetPut(WILL);
		TelnetPut(BINARY);

		TelnetPut(IAC);
		TelnetPut(WILL);
		TelnetPut(ECHO);
		
		TelnetPut(IAC);
		TelnetPut(WILL);
		TelnetPut(SUPPRESS_GO_AHEAD);
		
		TelnetPut(IAC);
		TelnetPut(WONT);
		TelnetPut(LINEMODE);
	}
	
	return true;
}

void Telnet::TelnetFlushOutput()
{
	if(telnet_output_buffer_size > 0)
	{
		if(IsVerbose())
		{
			logger << DebugInfo << "sending " << telnet_output_buffer_size << " chars" << EndDebugInfo;
		}
		unsigned int index = 0;
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			int r = send(telnet_sock, (const char *)(telnet_output_buffer + index), telnet_output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(telnet_sock, telnet_output_buffer + index, telnet_output_buffer_size);
			if(r <= 0)
#endif
			{
				logger << DebugError << "can't write into socket" << EndDebugError;
				Object::Stop(-1);
				return;
			}

			index += r;
			telnet_output_buffer_size -= r;
		}
		while(telnet_output_buffer_size > 0);
	}
}

void Telnet::TelnetPut(uint8_t v)
{
	if(telnet_output_buffer_size >= sizeof(telnet_output_buffer))
	{
		TelnetFlushOutput();
	}

	telnet_output_buffer[telnet_output_buffer_size++] = v;
}

bool Telnet::TelnetGet(uint8_t& v)
{
	if(telnet_input_buffer_size == 0)
	{
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			int r = recv(telnet_sock, (char *) telnet_input_buffer, sizeof(telnet_input_buffer), 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = read(telnet_sock, telnet_input_buffer, sizeof(telnet_input_buffer));
			if(r <= 0)
#endif
			{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				if(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
				if(r < 0 && errno == EAGAIN)
#endif
				{
					return false;
				}

				logger << DebugError << "can't read from socket" << EndDebugError;
				Object::Stop(-1);
				return false;
			}
			telnet_input_buffer_index = 0;
			telnet_input_buffer_size = r;
			break;
		} while(1);
	}

	v = telnet_input_buffer[telnet_input_buffer_index++];
	telnet_input_buffer_size--;
	
	return true;
}

bool Telnet::IsVerbose() const
{
	return verbose;
}

void Telnet::ResetCharIO()
{
	telnet_input_buffer_size = 0;
	telnet_input_buffer_index = 0;
	telnet_output_buffer_size = 0;
}

const char *Telnet::GetOptName(uint8_t opt) const
{
	switch(opt)
	{
		case ECHO: return "ECHO";
		case BINARY: return "BINARY";
		case LINEMODE: return "LINEMODE";
		case SUPPRESS_GO_AHEAD: return "SUPPRESS_GO_AHEAD";
	}
	return 0;
}

void Telnet::Dont(uint8_t opt)
{
	if(IsVerbose())
	{
		const char *opt_name = GetOptName(opt);
		logger << DebugInfo << "DONT ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void Telnet::Do(uint8_t opt)
{
	if(IsVerbose())
	{
		const char *opt_name = GetOptName(opt);
		logger << DebugInfo << "DO ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void Telnet::Wont(uint8_t opt)
{
	if(IsVerbose())
	{
		const char *opt_name = GetOptName(opt);
		logger << DebugInfo << "WONT ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void Telnet::Will(uint8_t opt)
{
	if(IsVerbose())
	{
		const char *opt_name = GetOptName(opt);
		logger << DebugInfo << "WILL ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void Telnet::SubNegociation(uint8_t opt, uint8_t *params, unsigned int num_params)
{
	if(IsVerbose())
	{
		logger << DebugInfo << "SB " << GetOptName(opt);
		unsigned int i;
		for(i = 0; i < num_params; i++)
		{
			logger << (unsigned int) params[i];
			if(i != (num_params - 1)) logger << " ";
		}
		logger << EndDebugInfo;
	}
}

bool Telnet::GetChar(char& c)
{
	uint8_t v;

	do
	{
		if(!TelnetGet(v)) return false;

		switch(state)
		{
			case 0:
				if(v == IAC)
				{
					state = 1;
					break;
				}
				if((v == 0) && filter_null_character) break; // filter null character
				if((v == 10) && filter_line_feed) break; // filter line feed
				c = (char) v;
				if(IsVerbose())
				{
					logger << DebugInfo << "Getting character ";
					if((v >= 32) && (v < 128))
						logger << "'" << c << "'";
					else
						logger << "0x" << std::hex << (unsigned int) v << std::dec;
					logger << EndDebugInfo;
				}
				return true;
				
			case 1: // got an IAC
				switch(v)
				{
					case IAC:
						c = (char) v;
						return true;
					case DONT:
						state = 2;
						break;
					case DO:
						state = 3;
						break;
					case WONT:
						state = 4;
						break;
					case WILL:
						state = 5;
						break;
					case SB:
						state = 6;
						break;
					default:
						state = 0;
						break;
				}
				break;
			case 2: // got a DONT
				Dont(v);
				state = 0;
				break;
			case 3: // got a DO
				Do(v);
				state = 0;
				break;
			case 4: // got a WONT
				Wont(v);
				state = 0;
				break;
			case 5: // got a WILL
				Will(v);
				state = 0;
				break;
			case 6: // got a SB
				sb_opt = v;
				sb_params_vec.clear();
				state = 7;
				break;
			case 7:
				if(v == SE)
				{
					unsigned int num_params = sb_params_vec.size();
					uint8_t sb_params[num_params];
					unsigned int i;
					for(i = 0; i < num_params; i++)
					{
						sb_params[i] = sb_params_vec[i];
					}
					SubNegociation(sb_opt, sb_params, num_params);
					sb_params_vec.clear();
					state = 0;
					break;
				}
				else
				{
					sb_params_vec.push_back(v);
				}
				break;
		}
	}
	while(1);

	return false;
}

void Telnet::PutChar(char c)
{
	uint8_t v = (uint8_t) c;
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Putting character ";
		if(v >= 32)
			logger << "'" << c << "'";
		else
			logger << "0x" << std::hex << (unsigned int) v << std::dec;
		logger << EndDebugInfo;
	}
	TelnetPut(v);
	if(v == IAC) TelnetPut(v);
}

void Telnet::FlushChars()
{
	TelnetFlushOutput();
}

} // end of namespace telnet
} // end of namespace service
} // end of namespace unisim
