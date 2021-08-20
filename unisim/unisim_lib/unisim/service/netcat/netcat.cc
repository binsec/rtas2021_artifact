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
 
#include <unisim/service/netcat/netcat.hh>

#include <cstring>
#include <cerrno>

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
namespace netcat {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

Netcat::Netcat(const char *name, Object *parent)
	: Object(name, parent, "This service provides character I/O over TCP/IP")
	, Service<CharIO>(name, parent)
	, char_io_export("char-io-export", this)
	, logger(*this)
	, verbose(false)
	, is_server(true)
	, server_name("localhost")
	, tcp_port(0)
	, sock(-1)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_tcp_port("tcp-port", this, tcp_port, "TCP/IP port")
	, param_is_server("is-server", this, is_server, "if 1, act as a server, otherwise act as a client")
	, param_server_name("server-name", this, server_name, "if acting as a client, name of server to connect to")
	, input_buffer_size(0)
	, input_buffer_index(0)
	, output_buffer_size(0)

{
	param_tcp_port.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
}

Netcat::~Netcat()
{
	if(sock >= 0)
	{
		FlushOutput();
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(sock);
#else
		close(sock);
#endif
	}
}

bool Netcat::EndSetup()
{
	if(sock >= 0) return true;
	
	if(is_server)
	{
		// Starting a server
		
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
		addr.sin_port = htons(tcp_port);
		addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		{
			logger << DebugError << "Bind failed. TCP Port #" << tcp_port << " may be already in use. Please specify another port in " << param_tcp_port.GetName() << EndDebugError;
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

		logger << DebugInfo << "Listening on TCP port " << tcp_port << EndDebugInfo;
		addr_len = sizeof(addr);
		sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);

		if(sock < 0)
		{
			logger << DebugError << "accept failed" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}

		logger << DebugInfo << "Connection with client established" << EndDebugInfo;

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
	}
	else
	{
		// Acting as a client
		
		bool connected = false;
		struct sockaddr_in sonadr;

		sock = socket(PF_INET, SOCK_STREAM, 0);

		if(sock < 0)
		{
			logger << DebugError << "Can't create socket for connection to " << server_name << ":" << tcp_port << EndDebugError;
			return false;
		}
		memset((char *) &sonadr, 0, sizeof(sonadr));
		sonadr.sin_family = AF_INET;
		sonadr.sin_port = htons(tcp_port);
		sonadr.sin_addr.s_addr = inet_addr(server_name.c_str());

		logger << DebugInfo << "Trying to connect to " << server_name << EndDebugInfo;

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		u_long addr_none = INADDR_NONE;
#else
		in_addr_t addr_none = INADDR_NONE;
#endif
		if(memcmp(&sonadr.sin_addr.s_addr, &addr_none, sizeof(addr_none)) != 0)
		{
			//host format is xxx.yyy.zzz.ttt
			connected = connect(sock, (struct sockaddr *) &sonadr, sizeof(sonadr)) != -1;
		}
		else
		{
			//host format is www.whereitis.gnu need to ask dns
			struct hostent *hp;
			int i = 0;
			hp = gethostbyname(server_name.c_str());
			if(!hp)
			{
				logger << DebugError << "Can't determine IP address from host name for" << server_name << ":" << tcp_port << EndDebugError;
			}
			else
			{
				while(!connected && hp->h_addr_list[i] != NULL)
				{
					memcpy((char *) &sonadr.sin_addr,
					(char *) hp->h_addr_list[i],
					sizeof(sonadr.sin_addr));
					connected = connect(sock, (struct sockaddr *) &sonadr, sizeof(sonadr)) != -1;
					i++;
				}
			}
		}

		if(!connected)
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(sock);
#else
			close(sock);
#endif
			sock = -1;
			logger << DebugError << "Can't connect to " << server_name << ":" << tcp_port << EndDebugError;
			
			return false;
		}
		
		logger << DebugInfo << "Connection with server established" << EndDebugInfo;
	}
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	u_long NonBlock = 1;
	if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket failed" << EndDebugError;
		closesocket(sock);
		sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(sock, F_GETFL, 0);

	if(socket_flag < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(sock);
		sock = -1;
		return false;
	}

	/* Ask for non-blocking reads on socket */
	if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(sock);
		sock = -1;
		return false;
	}
#endif

	return true;
}

void Netcat::FlushOutput()
{
	if(output_buffer_size > 0)
	{
		if(verbose)
		{
			logger << DebugInfo << "sending " << output_buffer_size << " chars" << EndDebugInfo;
		}
		unsigned int index = 0;
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			int r = send(sock, (const char *)(output_buffer + index), output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(sock, output_buffer + index, output_buffer_size);
			if(r <= 0)
#endif
			{
				logger << DebugError << "can't write into socket" << EndDebugError;
				Object::Stop(-1);
				return;
			}

			index += r;
			output_buffer_size -= r;
		}
		while(output_buffer_size > 0);
	}
}

void Netcat::Put(uint8_t v)
{
	if(output_buffer_size >= sizeof(output_buffer))
	{
		FlushOutput();
	}

	output_buffer[output_buffer_size++] = v;
}

bool Netcat::Get(uint8_t& v)
{
	if(input_buffer_size == 0)
	{
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			int r = recv(sock, (char *) input_buffer, sizeof(input_buffer), 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = read(sock, input_buffer, sizeof(input_buffer));
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
			input_buffer_index = 0;
			input_buffer_size = r;
			break;
		} while(1);
	}

	v = input_buffer[input_buffer_index++];
	input_buffer_size--;
	
	return true;
}

void Netcat::Reset()
{
	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer_size = 0;
}

bool Netcat::GetChar(char& c)
{
	uint8_t v;

	if(!Get(v)) return false;

	c = (char) v;
	if(verbose)
	{
		logger << DebugInfo << "Getting character ";
		if((v >= 32) && (v < 128))
			logger << "'" << c << "'";
		else
			logger << "0x" << std::hex << (unsigned int) v << std::dec;
		logger << EndDebugInfo;
	}
	return true;
}

void Netcat::PutChar(char c)
{
	uint8_t v = (uint8_t) c;
	
	if(verbose)
	{
		logger << DebugInfo << "Putting character ";
		if(v >= 32)
			logger << "'" << c << "'";
		else
			logger << "0x" << std::hex << (unsigned int) v << std::dec;
		logger << EndDebugInfo;
	}
	Put(v);
}

void Netcat::FlushChars()
{
	FlushOutput();
}

} // end of namespace netcat
} // end of namespace service
} // end of namespace unisim
