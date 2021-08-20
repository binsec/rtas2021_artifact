/*
 *  Copyright (c) 2007,
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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_UTIL_NETSTUB_NETSTUB_TCC__
#define __UNISIM_UTIL_NETSTUB_NETSTUB_TCC__

#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/types.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) /* Windows 32 or 64 */

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

#include <unistd.h>
#include <errno.h>

namespace unisim {
namespace util {
namespace netstub {

using namespace std;
	
template <class ADDRESS>
NetStub<ADDRESS>::NetStub(bool _is_server, 
	unsigned int _protocol,
	const char *_server_name, unsigned int _tcp_port,
	const char *_pipename)
	: sock(-1)
	, default_tu(TU_MS)
	, is_server(_is_server)
	, server_name(_server_name)
	, tcp_port(_tcp_port)
	, protocol(_protocol)
	, pipename(_pipename)
	, tag(0)
	, device_id(0)
	, started(true)
	, stopped(false)
	, input_buffer_size(0)
	, input_buffer_index(0)
	, output_buffer_size(0)
{
	s_status[STATUS_OK] = "ok";
	s_status[STATUS_FAILED] = "failed";
	s_status[STATUS_INTERNAL_ERROR] = "internal_error";
	s_status[STATUS_BROKEN_PIPE] = "broken_pipe";
	s_status[STATUS_ABORTED] = "aborted";

	s_command[PKC_START] = "start";
	s_command[PKC_STOP] = "stop";
	s_command[PKC_READ] = "read";
	s_command[PKC_WRITE] = "write";
	s_command[PKC_WRITEREG] = "writereg";
	s_command[PKC_READREG] = "readreg";
	s_command[PKC_RUN] = "run";
	s_command[PKC_INTR] = "intr";
	s_command[PKC_SETBRK] = "setbrk";
	s_command[PKC_SETWRITEW] = "setwritew";
	s_command[PKC_SETREADW] = "setreadw";
	s_command[PKC_RMBRK] = "rmbrk";
	s_command[PKC_RMWRITEW] = "rmwritew";
	s_command[PKC_RMREADW] = "rmreadw";
	s_command[PKC_TRAP] = "trap";
	s_command[PKC_STATUS] = "status";

	s_tu[TU_FS] = "fs";
	s_tu[TU_PS] = "ps";
	s_tu[TU_NS] = "ns";
	s_tu[TU_US] = "us";
	s_tu[TU_MS] = "ms";
	s_tu[TU_S] = "s";

	s_bool[0] = "false";
	s_bool[1] = "true";

	s_trap_type[TRAP_BREAKPOINT] = "breakpoint";
	s_trap_type[TRAP_WATCHPOINT] = "watchpoint";

	s_watchpoint_type[WATCHPOINT_READ] = "read";
	s_watchpoint_type[WATCHPOINT_WRITE] = "write";

	s_space[SP_CPU_MEM] = "cpu_mem";
	s_space[SP_DEV_MEM] = "dev_mem";
	s_space[SP_DEV_IO] = "dev_io";
}

template <class ADDRESS>
bool NetStub<ADDRESS>::Initialize()
{
	bool bind_failed = true;

	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer_size = 0;
	tag = 0;
	device_id = 0;
	stopped = false;
	started = false;
	sock = -1;
	
	if(is_server)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Starting as server" << endl;
#endif
		struct sockaddr_in addr;
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		struct sockaddr_un addr_un;
#endif
		int server_sock;
	
		if(protocol == protocol_af_inet) 
		{
			server_sock = socket(AF_INET, SOCK_STREAM, 0);
		}
		else
		{
			server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
		}
		
		if(server_sock < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: socket failed" << endl;
#endif
			return false;
		}
		
		memset(&addr, 0, sizeof(addr));
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		memset(&addr_un, 0, sizeof(addr_un));
		if(protocol == protocol_af_inet)
		{
#endif
			addr.sin_family = AF_INET;
			addr.sin_port = htons(tcp_port);
			addr.sin_addr.s_addr = INADDR_ANY;
			bind_failed =
				(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0);
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			addr_un.sun_family = AF_UNIX;
			memcpy(addr_un.sun_path, pipename.c_str(), strlen(pipename.c_str()) + 1);
			bind_failed =
				(bind(server_sock, (struct sockaddr *) &addr_un, sizeof(addr_un)) < 0);
		}
#endif
		if(bind_failed)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: bind failed" << endl;
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}
	
		if(listen(server_sock, 1))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: listen failed" << endl;
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(sock);
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

#ifdef DEBUG_NETSTUB
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		if(protocol == protocol_af_inet)
		{
#endif
			cerr << "NETSTUB: Waiting for client connection on TCP port " << tcp_port << endl;
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			cerr << "NETSTUB: Waiting for client connection of pipename " << pipename << endl;
		}
#endif
#endif

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		if(protocol == protocol_af_inet) 
		{
#endif
			addr_len = sizeof(addr);
			sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			addr_len = sizeof(addr_un);
			sock = accept(server_sock, (struct sockaddr *) &addr_un, &addr_len);
		}
#endif

		if(sock < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: accept failed" << endl;
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}
		
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
	}
	else
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Starting as client" << endl;
#endif
		bool connected = false;
		struct sockaddr_in sonadr;
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		struct sockaddr_un sonadr_un;
		
		if(protocol == protocol_af_inet)
		{
#endif
			sock = socket(PF_INET, SOCK_STREAM, 0);
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			sock = socket(PF_UNIX, SOCK_STREAM, 0);
		}
#endif

		if(sock < 0)
		{
#ifdef DEBUG_NETSTUB
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
			if(protocol == protocol_af_inet)
			{
#endif
				cerr << "NETSTUB: Can't create socket for connection to " << server_name
					<< ":" << tcp_port << endl;
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
			}
			else
			{
				cerr << "NETSTUB: Can't create socket for connection on pipename "
					<< pipename << endl;
			}
#endif
#endif
			return false;
		}
		memset((char *) &sonadr, 0, sizeof(sonadr));
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		memset((char *) &sonadr_un, 0, sizeof(sonadr_un));

		if(protocol == protocol_af_inet) {
#endif
			sonadr.sin_family = AF_INET;
			sonadr.sin_port = htons(tcp_port);
			sonadr.sin_addr.s_addr = inet_addr(server_name.c_str());
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			sonadr_un.sun_family = AF_UNIX;
			memcpy(sonadr_un.sun_path, pipename.c_str(), strlen(pipename.c_str()) + 1);
		}
#endif

#ifdef DEBUG_NETSTUB
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		if(protocol == protocol_af_inet)
		{
#endif
			cerr << "NETSTUB: Trying to connect to " << server_name << endl;
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			cerr << "NETSTUB: Trying to connect to pipename " << pipename << endl;
		}
#endif
#endif

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		if(protocol == protocol_af_inet)
		{
#endif
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
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: Was not able to determine IP address from host name for" << server_name << ":" << tcp_port << endl;
#endif
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
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
		}
		else
		{
			connected = connect(sock, (struct sockaddr *) &sonadr_un, sizeof(sonadr_un)) != -1;
		}
#endif
		if(!connected)
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(sock);
#else
			close(sock);
#endif
			sock = -1;
#ifdef DEBUG_NETSTUB
			if(protocol == protocol_af_inet)
			{
				cerr << "NETSTUB: Can't connect to " << server_name << ":" << tcp_port << endl;
			}
			else
			{
				cerr << "NETSTUB: Can't connect to pipename " << pipename << endl;
			}
#endif
		}
	}
	
	if(sock >= 0)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Connection established" << endl;
#endif
		/* we can stop here if we are using af_unix */
		if(protocol == protocol_af_unix) return true;

		/* set short latency */
		int opt = 1;
		if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, sizeof(opt)) < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: setsockopt failed requesting short latency" << endl;
#endif
		}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		u_long NonBlock = 1;
		if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: ioctlsocket failed" << endl;
#endif	
			closesocket(sock);
			sock = -1;
			return false;
		}
#else // if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		int socket_flag = fcntl(sock, F_GETFL, 0);
		
		if(socket_flag < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: fcntl failed" << endl;
#endif
			close(sock);
			sock = -1;
			return false;
		}
		
		/* Ask for non-blocking reads on socket */
		if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: fcntl failed" << endl;
#endif
			close(sock);
			sock = -1;
			return false;
		}
#endif // if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	
		return true;
	}

	return false;
}

template <class ADDRESS>
NetStub<ADDRESS>::~NetStub()
{
	if(started && !stopped)
	{
		PutStatusPacket(STATUS_ABORTED, device_id, tag + 1);
	}

	if(sock >= 0)
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(sock);
#else
		close(sock);
#endif
		sock = -1;
	}
}

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

inline uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

inline bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::GetChar(char& c, bool blocking)
{
	if(input_buffer_size == 0)
	{
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			int r = recv(sock, input_buffer, sizeof(input_buffer), 0);
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
					if(blocking)
					{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
						Sleep(1); // sleep for 10ms
#else
						usleep(1000); // sleep for 10ms
#endif
						continue;
					}
					else
					{
						return false;
					}
				}
			
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: Can't read from socket" << endl;
#endif
				return false;
			}
			input_buffer_index = 0;
			input_buffer_size = r;
			break;
		} while(1);
	}

	c = input_buffer[input_buffer_index++];
	input_buffer_size--;
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::FlushOutput()
{
	if(output_buffer_size > 0)
	{
//		cerr << "begin: output_buffer_size = " << output_buffer_size << endl;
		unsigned int index = 0;
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			int r = send(sock, output_buffer + index, output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(sock, output_buffer + index, output_buffer_size);
			if(r <= 0)
#endif
			{
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: can't write into socket" << endl;
#endif
				return false;
			}

			index += r;
			output_buffer_size -= r;
//			cerr << "output_buffer_size = " << output_buffer_size << endl;
		}
		while(output_buffer_size > 0);
	}
//	cerr << "end: output_buffer_size = " << output_buffer_size << endl;
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutChar(char c)
{
	if(output_buffer_size >= sizeof(output_buffer))
	{
		if(!FlushOutput()) return false;
	}

	output_buffer[output_buffer_size++] = c;
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::GetPacket(string& s, bool blocking, uint32_t& device_id, uint32_t& tag)
{
	char c;
	char ch[2];
	ch[1] = 0;
	s.erase();
	device_id = 0;
	tag = 0;
	
	while(1)
	{
		if(!GetChar(c, blocking)) return false;
		if(c == '$') break;
	}
		
	while(1)
	{
		if(!GetChar(c, true)) return false;
		if(c == '#' || c == '$') break;
		ch[0] = c;
		s += ch;
	}
	
	if(c == '#')
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Packet has probably device_id" << endl;
#endif
		uint8_t nibble;
		
		while(1)
		{
			if(!GetChar(c, true)) return false;
			//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
			if(c == '$' || c == ';') break;
			if(!IsHexChar(c))
			{
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: unexpected character '" << c << "'" << endl;
#endif
				return false;
			}
			nibble = HexChar2Nibble(c);
			device_id <<= 4;
			device_id |= nibble;
		}
		
		if(c == ';')
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: Packet has probably tag" << endl;
#endif
			while(1)
			{
				if(!GetChar(c, true)) return false;
				//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
				if(c == '$') break;
				if(!IsHexChar(c))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: unexpected character '" << c << "'" << endl;
#endif
					return false;
				}
				nibble = HexChar2Nibble(c);
				tag <<= 4;
				tag |= nibble;
			}
		}
	}
	
#ifdef DEBUG_NETSTUB
	cerr << "NETSTUB: received '$" << s << "#" << std::hex << device_id << ";" << tag << std::dec << "$'" << endl;
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutHex(uint32_t value)
{
	if(!value)
	{
		return PutChar('0');
	}
	
	int i;
	for(i = 8 * (sizeof(value) - 1) + 4; i >= 0; i -= 4)
	{
		if(value >> i) break;
	}
	for(; i >= 0; i -= 4)
	{
		if(!PutChar(Nibble2HexChar(value >> i))) return false;
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutPacket(const string& s, const uint32_t device_id, const uint32_t tag)
{
	unsigned int pos;
	unsigned int len;
	char c;
	
	if(!PutChar('$')) return false;
	pos = 0;
	len = s.length();
	
	while(pos < len)
	{
		c = s[pos];
		if(!PutChar(c)) return false;
		pos++;
	}
	if(!PutChar('#')) return false;
	if(!PutHex(device_id)) return false;
	if(!PutChar(';')) return false;
	if(!PutHex(tag)) return false;
	if(!PutChar('$')) return false;
	if(!FlushOutput()) return false;
#ifdef DEBUG_NETSTUB
	cerr << "NETSTUB: sent '$" << s << "#" << std::hex << device_id << ";" << tag << std::dec << "$'" << endl;
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseHex(const string& s, unsigned int& pos, uint32_t& value)
{
	unsigned int len = s.length();
	unsigned int n = 0;
	
	value = 0;
	while(pos < len)
	{
		uint8_t nibble;
		char c = s[pos];
		if(!IsHexChar(c)) break;
		//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
		nibble = HexChar2Nibble(c);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	//if(n > 0) cerr << Object::GetName() << ": got 0x" << std::hex << value << std::dec << endl;
#ifdef DEBUG_NETSTUB
	if(n > 2 * sizeof(value))
	{
		cerr << "NETSTUB: WARNING! the upper " << ((4 * n) - (8 * sizeof(value)))
			<< " bits have been lost while converting a "
			<< (4 * n) << "-bit hexadecimal integer to a "
			<< (8 * sizeof(value)) << "-bit integer" << endl;
	}
#endif
	return n > 0;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseBool(const string& s, unsigned int& pos, bool& value)
{
	unsigned int i;
	for(i = 0; i <= 1; i++)
	{
		if(s.compare(pos, s_bool[i].length(), s_bool[i]) == 0)
		{
			value = i ? true : false;
			pos += s_bool[i].length();
			return true;
		}
	}

	uint32_t int_value;
	if(ParseHex(s, pos, int_value))
	{
		value = (int_value != 0);
		return true;
	}

	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseHex(const string& s, unsigned int& pos, uint64_t& value)
{
	unsigned int len = s.length();
	unsigned int n = 0;
	
	value = 0;
	while(pos < len && n < 2 * sizeof(value))
	{
		uint8_t nibble;
		char c = s[pos];
		if(!IsHexChar(c)) break;
		//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
		nibble = HexChar2Nibble(c);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	//if(n > 0) cerr << Object::GetName() << ": got 0x" << std::hex << value << std::dec << endl;
	return n > 0;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseHex(const string& s, unsigned int& pos, uint8_t *buffer, uint32_t size)
{
	unsigned int i;
	uint32_t offset;
	uint32_t l;
	char c;

	l = s.length();
	
	for(offset = 0, i = 0; pos < l && offset < size; i++, pos++)
	{
		c = s[pos];

		//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
		if(!IsHexChar(c))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting an hexadecimal character at col #" << (pos + 1) << " (got '" << c << "')" << endl;
#endif
			return false;
		}
		
		if(i & 1)
		{
			buffer[offset] = buffer[offset] | HexChar2Nibble(c);
			//cerr << "buffer[" << offset << "] = 0x" << std::hex << (unsigned int) buffer[offset] << std::dec << endl;
			offset++;
		}
		else
		{
			buffer[offset] = HexChar2Nibble(c) << 4;
		}
	}
	
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseChar(const string& packet, unsigned int& pos, char c)
{	
	if(pos >= packet.length() || packet[pos] != c) return false;
	//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
	pos++;
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseString(const string& packet, unsigned int& pos, char separator, string& s)
{
	unsigned int start_pos = pos;

	while(pos < packet.length())
	{
		if(packet[pos] == separator) break;
		pos++;
	}

	s = packet.substr(start_pos, pos - start_pos);
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseAddress(const string& packet, unsigned int& pos, ADDRESS& addr)
{
	if(ParseChar(packet, pos, '*'))
	{
		if(!ParseHex(packet, pos, addr))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
			return false;
		}
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSymbol(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name)
{
	if(ParseChar(packet, pos, '*'))
	{
		if(!ParseHex(packet, pos, addr))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
			return false;
		}
	}
	else
	{
		if(!ParseString(packet, pos, 0, symbol_name))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting a symbol name at col #" << (pos + 1) << endl;
#endif
			return false;
		}
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseCommand(const string& packet, unsigned int& pos, PK_COMMAND& command)
{
	for(command = PKC_MIN; command <= PKC_MAX; command = (PK_COMMAND)(command + 1))
	{
		if(packet.compare(pos, s_command[command].length(), s_command[command]) == 0)
		{
			pos += s_command[command].length();
			return true;
		}
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		if(value >= PKC_MIN && value <= PKC_MAX)
		{
			command = (PK_COMMAND) value;
			return true;
		}
	}
		
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseTimeUnit(const string& packet, unsigned int& pos, TIME_UNIT& tu)
{
	for(tu = TU_MIN; tu <= TU_MAX; tu = (TIME_UNIT)(tu + 1))
	{
		if(packet.compare(pos, s_tu[tu].length(), s_tu[tu]) == 0)
		{
			pos += s_tu[tu].length();
			return true;
		}
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		if(value >= TU_MIN && value <= TU_MAX)
		{
			tu = (TIME_UNIT) value;
			return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSpace(const string& packet, unsigned int& pos, SPACE& space)
{
	for(space = SP_MIN; space <= SP_MAX; space = (SPACE)(space + 1))
	{
		if(packet.compare(pos, s_space[space].length(), s_space[space]) == 0)
		{
			pos += s_space[space].length();
			return true;
		}
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		if(value >= SP_MIN && value <= SP_MAX)
		{
			space = (SPACE) value;
			return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseTrapType(const string& packet, unsigned int& pos, TRAP_TYPE& trap_type)
{
	for(trap_type = TRAP_MIN; trap_type <= TRAP_MAX; trap_type = (TRAP_TYPE)(trap_type + 1))
	{
		if(packet.compare(pos, s_trap_type[trap_type].length(), s_trap_type[trap_type]) == 0)
		{
			pos += s_trap_type[trap_type].length();
			return true;
		}
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		if(value >= TRAP_MIN && value <= TRAP_MAX)
		{
			trap_type = (TRAP_TYPE) value;
			return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseWatchpointType(const string& packet, unsigned int& pos, WATCHPOINT_TYPE& watchpoint_type)
{
	for(watchpoint_type = WATCHPOINT_MIN; watchpoint_type <= WATCHPOINT_MAX; watchpoint_type = (WATCHPOINT_TYPE)(watchpoint_type + 1))
	{
		if(packet.compare(pos, s_watchpoint_type[watchpoint_type].length(), s_watchpoint_type[watchpoint_type]) == 0)
		{
			pos += s_watchpoint_type[watchpoint_type].length();
			return true;
		}
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		if(value >= WATCHPOINT_MIN && value <= WATCHPOINT_MAX)
		{
			watchpoint_type = (WATCHPOINT_TYPE) value;
			return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseStatus(const string& packet, unsigned int& pos, STATUS& status)
{
	for(status = STATUS_MIN; status <= STATUS_MAX; status = (STATUS)(status + 1))
	{
		if(packet.compare(pos, s_status[status].length(), s_status[status]) == 0)
		{
			pos += s_status[status].length();
			return true;
		}
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		if(value >= STATUS_MIN && value <= STATUS_MAX)
		{
			status = (STATUS) value;
			return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseStart(const string& packet, unsigned int& pos, TIME_UNIT& tu)
{
	if(pos == packet.length()) return true;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseTimeUnit(packet, pos, tu))
	{
		tu = TU_MS;
	}
	
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseStop(const string& packet, unsigned int& pos)
{
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRead(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(pos < packet.length())
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
#endif
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseWrite(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, uint8_t **data, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(size > 0)
	{
		if(!ParseChar(packet, pos, ';'))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
			return false;
		}
		//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
		*data = new uint8_t[size];
		
		if(!ParseHex(packet, pos, *data, size))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting some data at col #" << (pos + 1) << endl;
#endif
			return false;
		}
		//cerr << Object::GetName() << ": at " << pos << " got some data" << endl;
	}
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseWriteRegister(const string& packet, unsigned int& pos, string& register_name, uint32_t& value)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseString(packet, pos, ';', register_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a register name at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, value))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a value at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseReadRegister(const string& packet, unsigned int& pos, string& register_name)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseString(packet, pos, 0, register_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a register name at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseIntr(const string& packet, unsigned int& pos, uint32_t& intr_id, bool& level)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseHex(packet, pos, intr_id))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an interrupt id at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got interrupt id" << std::hex << intr_id << std::dec << endl;

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseBool(packet, pos, level))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an interrupt level (0/1 or true/false) at col #" << (pos + 1) << endl;
#endif
		return false;
	}
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRun(const string& packet, unsigned int& pos, uint64_t& duration, TIME_UNIT& tu)
{
	if(pos >= packet.length())
	{
		duration = 0;
		return true;
	}

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseHex(packet, pos, duration))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a duration at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got duration " << std::hex << duration << std::dec << endl;
	
	if(pos >= packet.length())
	{
		tu = default_tu;
		return true;
	}
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	if(!ParseTimeUnit(packet, pos, tu))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a time unit at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSetBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSymbol(packet, pos, addr, symbol_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address or a symbol name at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSetReadWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSetWriteWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRemoveBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSymbol(packet, pos, addr, symbol_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address or a symbol name at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRemoveWriteWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRemoveReadWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseTrap(const string& packet, unsigned int& pos, uint64_t& t, TIME_UNIT& tu, list<TRAP>& traps)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseHex(packet, pos, t))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a time at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	if(!ParseTimeUnit(packet, pos, tu))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a time unit at col #" << (pos + 1) << endl;
#endif
		return false;
	}

	traps.clear();

	while(pos < packet.length()) // end of packet ?
	{
		if(!ParseChar(packet, pos, ';'))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
			return false;
		}

		TRAP trap;

		if(!ParseTrapType(packet, pos, trap.type))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting a trap type at col #" << (pos + 1) << endl;
#endif
		}

		switch(trap.type)
		{
			case TRAP_BREAKPOINT:
				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
					return false;
				}
		
				if(!ParseAddress(packet, pos, trap.breakpoint.addr))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
					return false;
				}
				break;

			case TRAP_WATCHPOINT:
				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
					return false;
				}
		
				if(!ParseWatchpointType(packet, pos, trap.watchpoint.wtype))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting a watchpoint type at col #" << (pos + 1) << endl;
#endif
					return false;
				}

				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
					return false;
				}
		
				if(!ParseAddress(packet, pos, trap.watchpoint.addr))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting an address at col #" << (pos + 1) << endl;
#endif
					return false;
				}

				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
					return false;
				}

				if(!ParseHex(packet, pos, trap.watchpoint.size))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting a size at col #" << (pos + 1) << endl;
#endif
					return false;
				}

				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
					return false;
				}

				if(!ParseSpace(packet, pos, trap.watchpoint.space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting an address space ('cpu_mem', 'dev_mem', 'dev_io') at col #" << (pos + 1) << endl;
#endif
					return false;
				}
				break;
			default:
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: unexpected trap type" << endl;
#endif
				break;
		}

		traps.push_back(trap);
	}

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseStatusPacket(const string& packet, unsigned int& pos, STATUS& status)
{
	if(pos == packet.length()) return true;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';' at col #" << (pos + 1) << endl;
#endif
		return false;
	}
	
	if(!ParseStatus(packet, pos, status))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a value at col #" << (pos + 1) << endl;
#endif
	}
	
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at col #" << (pos + 1) << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutStartPacket(const TIME_UNIT tu, const uint32_t device_id, const uint32_t tag)
{
	if(tu < TU_MIN || tu > TU_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_START] << ';';
	sstr << s_tu[tu];
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutStopPacket(const uint32_t device_id, const uint32_t tag)
{
	return PutPacket(s_command[PKC_STOP], device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutWritePacket(ADDRESS addr, uint32_t size, uint8_t *data, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	if(space < SP_MIN || space > SP_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_WRITE] << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << s_space[space];

	if(size > 0)
	{
		sstr << ';';
	
		uint32_t i;
			
		for(i = 0; i < size; i++)
		{
			sstr << Nibble2HexChar(data[i] >> 4);
			sstr << Nibble2HexChar(data[i] & 0xf);
		}
	}
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutWriteRegisterPacket(const char *name, uint32_t value, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_WRITEREG] << ';';
	sstr << name << ';';
	sstr << value;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutReadRegisterPacket(const char *name, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_READREG] << ';';
	sstr << name;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutReadPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	if(space < SP_MIN || space > SP_MAX) return false;

	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_READ] << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << s_space[space];
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutIntrPacket(uint32_t intr_id, bool level, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_INTR] << ';';
	sstr << intr_id << ';';
	sstr << s_bool[level ? 1 : 0];
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRunPacket(uint64_t duration, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_RUN] << ';';
	sstr << duration;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRunPacket(uint64_t duration, TIME_UNIT tu, const uint32_t device_id, const uint32_t tag)
{
	if(tu < TU_MIN || tu > TU_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_RUN] << ';';
	sstr << duration << ';';
	sstr << s_tu[tu];
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutSetBreakpointPacket(ADDRESS addr, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_SETBRK] << ';';
	sstr << '*' << addr;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutSetBreakpointPacket(const char *symbol_name, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_SETBRK] << ';';
	sstr << symbol_name;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutSetReadWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	if(space < SP_MIN || space > SP_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_SETREADW] << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << s_space[space];
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutSetWriteWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	if(space < SP_MIN || space > SP_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_SETWRITEW] << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << s_space[space];
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRemoveBreakpointPacket(ADDRESS addr, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_RMBRK] << ';';
	sstr << '*' << addr;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRemoveBreakpointPacket(const char *symbol_name, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_RMBRK] << ';';
	sstr << symbol_name;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRemoveReadWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	if(space < SP_MIN || space > SP_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_RMREADW] << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << s_space[space];
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRemoveWriteWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	if(space < SP_MIN || space > SP_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_RMWRITEW] << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << s_space[space];
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutTrapPacket(uint64_t t, TIME_UNIT tu, const list<TRAP>& traps, const uint32_t device_id, const uint32_t tag)
{
	if(tu < TU_MIN || tu > TU_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_TRAP] << ';';
	sstr << t << ';';
	sstr << s_tu[tu];
	
	typename list<TRAP>::const_iterator trap_iter;

	for(trap_iter = traps.begin(); trap_iter != traps.end(); trap_iter++)
	{
		sstr << ';';
		if(trap_iter->type < TRAP_MIN || trap_iter->type > TRAP_MAX) return false;
		sstr << s_trap_type[trap_iter->type];
		switch(trap_iter->type)
		{
			case TRAP_BREAKPOINT:
				sstr << ';' << '*' << std::hex << trap_iter->breakpoint.addr;
				break;
			case TRAP_WATCHPOINT:
				if(trap_iter->watchpoint.wtype < WATCHPOINT_MIN || trap_iter->watchpoint.wtype > WATCHPOINT_MAX) return false;
				if(trap_iter->watchpoint.space < SP_MIN || trap_iter->watchpoint.space > SP_MAX) return false;
				sstr << ';' << s_watchpoint_type[trap_iter->watchpoint.wtype];
				sstr << ';' << std::hex << '*' << trap_iter->watchpoint.addr << ';' << trap_iter->watchpoint.size;
				sstr << ';' << s_space[trap_iter->watchpoint.space];
				break;
			default:
				cerr << "WARNING! NetStub::PutTrapPacket executing unknown option" << endl;
				break;
		}
	}
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutStatusPacket(STATUS status, const uint32_t device_id, const uint32_t tag)
{
	if(status < STATUS_MIN || status > STATUS_MAX) return false;
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command[PKC_STATUS] << ';';
	sstr << s_status[status];
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
const char *NetStub<ADDRESS>::GetName(STATUS status) const
{
	return (status >= STATUS_MIN && status <= STATUS_MAX) ? s_status[status].c_str() : 0;
}

template <class ADDRESS>
const char *NetStub<ADDRESS>::GetName(PK_COMMAND command) const
{
	return (command >= PKC_MIN && command <= PKC_MAX) ? s_command[command].c_str() : 0;
}

template <class ADDRESS>
const char *NetStub<ADDRESS>::GetName(TIME_UNIT tu) const
{
	return (tu >= TU_MIN && tu <= TU_MAX) ? s_tu[tu].c_str() : 0;
}

template <class ADDRESS>
const char *NetStub<ADDRESS>::GetName(SPACE space) const
{
	return (space >= SP_MIN && space <= SP_MAX) ? s_space[space].c_str() : 0;
}

template <class ADDRESS>
const char *NetStub<ADDRESS>::GetName(TRAP_TYPE trap_type) const
{
	return (trap_type >= TRAP_MIN && trap_type <= TRAP_MAX) ? s_trap_type[trap_type].c_str() : 0;
}

template <class ADDRESS>
const char *NetStub<ADDRESS>::GetName(WATCHPOINT_TYPE watchpoint_type) const
{
	return (watchpoint_type >= WATCHPOINT_MIN && watchpoint_type <= WATCHPOINT_MAX) ? s_watchpoint_type[watchpoint_type].c_str() : 0;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::Start(TIME_UNIT tu)
{
	if(!PutStartPacket(tu, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_BROKEN_PIPE;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::Stop()
{
	if(!PutStopPacket(device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::Read(ADDRESS addr, void *buffer, uint32_t size, SPACE space)
{
	if(!PutReadPacket(addr, size, space, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;
	ADDRESS pk_addr;
	uint32_t pk_size;
	SPACE pk_space;
	uint8_t *pk_data;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command == PKC_STATUS)
	{
		STATUS status;
		if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Got status '" << s_status[status] << "'" << endl;
#endif
		return status;
	}

	if(pk_command != PKC_WRITE)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(!ParseWrite(packet, pos, pk_addr, pk_size, &pk_data, pk_space)) return STATUS_INTERNAL_ERROR;

	if(pk_size != size)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected data size of " << pk_size << " bytes" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_space != space)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected space '" << GetName(pk_space) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	memcpy(buffer, pk_data, pk_size);
	delete[] pk_data;
	return STATUS_OK;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::Write(ADDRESS addr, const void *buffer, uint32_t size, SPACE space)
{
	if(!PutWritePacket(addr, size, (uint8_t *) buffer, space, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::WriteRegister(const char *name, uint32_t value)
{
	if(!PutWriteRegisterPacket(name, value, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::ReadRegister(const char *name, uint32_t& value)
{
	if(!PutReadRegisterPacket(name, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;

	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command == PKC_STATUS)
	{
		STATUS status;
		if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Got status '" << s_status[status] << "'" << endl;
#endif
		return status;
	}

	if(pk_command != PKC_WRITEREG)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	string pk_register_name;

	if(!ParseWriteRegister(packet, pos, pk_register_name, value)) return STATUS_INTERNAL_ERROR;

	if(pk_register_name != name)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected register '" << pk_register_name << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	return STATUS_OK;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::Intr(uint32_t intr_id, bool level)
{
	if(!PutIntrPacket(intr_id, level, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::Run(int64_t duration, TIME_UNIT duration_tu, uint64_t& t, TIME_UNIT& tu, list<TRAP>& traps)
{
	if(!PutRunPacket(duration, duration_tu, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command == PKC_STATUS)
	{
		STATUS status;
		if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Got status '" << s_status[status] << "'" << endl;
#endif
		return status;
	}

	if(pk_command != PKC_TRAP)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	return ParseTrap(packet, pos, t, tu, traps) ? STATUS_OK : STATUS_INTERNAL_ERROR;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	if(!PutSetBreakpointPacket(addr, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::SetBreakpoint(const char *symbol_name)
{
	if(!PutSetBreakpointPacket(symbol_name, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::SetReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	if(!PutSetReadWatchpointPacket(addr, size, space, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::SetWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	if(!PutSetWriteWatchpointPacket(addr, size, space, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::RemoveBreakpoint(ADDRESS addr)
{
	if(!PutRemoveBreakpointPacket(addr, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::RemoveBreakpoint(const char *symbol_name)
{
	if(!PutRemoveBreakpointPacket(symbol_name, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::RemoveReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	if(!PutRemoveReadWatchpointPacket(addr, size, space, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
typename NetStub<ADDRESS>::STATUS NetStub<ADDRESS>::RemoveWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	if(!PutRemoveWriteWatchpointPacket(addr, size, space, device_id, ++tag)) return STATUS_BROKEN_PIPE;

	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	uint32_t pk_device_id;

	if(!GetPacket(packet, true, pk_device_id, tag)) return STATUS_BROKEN_PIPE;
	pos = 0;
	if(!ParseCommand(packet, pos, pk_command))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	if(pk_command != PKC_STATUS)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: unexpected command '" << GetName(pk_command) << "'" << endl;
#endif
		return STATUS_INTERNAL_ERROR;
	}

	STATUS status;
	if(!ParseStatusPacket(packet, pos, status)) return STATUS_INTERNAL_ERROR;

	return status;
}

template <class ADDRESS>
void NetStub<ADDRESS>::Serve()
{
	string packet;
	unsigned int pos;
	PK_COMMAND pk_command;
	
	while(1)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Reading packet..." << endl;
#endif
		if(!GetPacket(packet, true, device_id, tag))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: GetPacket failed" << endl;
#endif
			stopped = true; ServeStop();
			return;
		}
	
		pos = 0;
		if(!ParseCommand(packet, pos, pk_command))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting a command at col #" << (pos + 1) << endl;
#endif
			PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
			stopped = true; ServeStop();
			return;
		}
			
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: parsing command '" << GetName(pk_command) << "'" << endl;
#endif

		switch(pk_command)
		{
			case PKC_START:
			{
				TIME_UNIT pk_tu;
				
				if(!ParseStart(packet, pos, pk_tu))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
				
				default_tu = pk_tu;
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				started = true;
				ServeStart();
				if(!PutStatusPacket(STATUS_OK, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}
			case PKC_STOP:
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				PutStatusPacket(STATUS_OK, device_id, tag + 1);
				stopped = true; ServeStop();
				return;
				
			case PKC_READ:
			{
				ADDRESS pk_addr;
				uint32_t pk_size;
				uint8_t *pk_data;
				SPACE pk_space;
				if(!ParseRead(packet, pos, pk_addr, pk_size, pk_space))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				
				pk_data = new uint8_t[pk_size];
				memset(pk_data, 0, pk_size);
				if(!ServeRead(pk_addr, pk_data, pk_size, pk_space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: memory read failed" << endl;
#endif
					if(!PutStatusPacket(STATUS_FAILED, device_id, tag + 1))
					{
						stopped = true; ServeStop();
					}
					return;
				}
				if(!PutWritePacket(pk_addr, pk_size, pk_data, pk_space, device_id, tag + 1))
				{
					delete[] pk_data;
					stopped = true; ServeStop();
					return;
				}
				delete[] pk_data;
				break;
			}
			
			case PKC_WRITE:
			{
				ADDRESS pk_addr;
				uint32_t pk_size;
				uint8_t *pk_data;
				SPACE pk_space;
				if(!ParseWrite(packet, pos, pk_addr, pk_size, &pk_data, pk_space))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(!ServeWrite(pk_addr, pk_data, pk_size, pk_space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: memory write failed" << endl;
#endif
					status = STATUS_FAILED;
				}
				else
				{
					delete[] pk_data;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_WRITEREG:
			{
				string pk_register_name;
				uint32_t pk_data;

				if(!ParseWriteRegister(packet, pos, pk_register_name, pk_data))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}

#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				
				STATUS status = STATUS_OK;
				if(!ServeWriteRegister(pk_register_name.c_str(), pk_data))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: register write failed" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_READREG:
			{
				string pk_register_name;
				uint32_t pk_data;

				if(!ParseReadRegister(packet, pos, pk_register_name))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}

#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				
				if(!ServeReadRegister(pk_register_name.c_str(), pk_data))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: register read failed" << endl;
#endif
					if(!PutStatusPacket(STATUS_FAILED, device_id, tag + 1))
					{
						stopped = true; ServeStop();
					}
					return;
				}

				if(!PutWriteRegisterPacket(pk_register_name.c_str(), pk_data, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}
			
			case PKC_INTR:
			{
				uint32_t pk_intr_id;
				bool level;
				if(!ParseIntr(packet, pos, pk_intr_id, level))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				ServeIntr(pk_intr_id, level);
				if(!PutStatusPacket(STATUS_OK, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}
			
			case PKC_RUN:
			{
				TIME_UNIT pk_tu;
				uint64_t pk_duration;
				uint64_t t;
				TIME_UNIT tu;
				list<TRAP> traps;
				
				if(!ParseRun(packet, pos, pk_duration, pk_tu))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				ServeRun(pk_duration, pk_tu, t, tu, traps);

				if(!PutTrapPacket(t, tu, traps, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_SETBRK:
			{
				ADDRESS pk_addr;
				string pk_symbol_name;
				if(!ParseSetBreakpoint(packet, pos, pk_addr, pk_symbol_name))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}

#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(pk_symbol_name.empty() ? !ServeSetBreakpoint(pk_addr) : !ServeSetBreakpoint(pk_symbol_name.c_str()))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: Can't set breakpoint" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_SETWRITEW:
			{
				ADDRESS pk_addr;
				uint32_t pk_size;
				SPACE pk_space;
				if(!ParseSetWriteWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}

#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(!ServeSetWriteWatchpoint(pk_addr, pk_size, pk_space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: Can't set write watchpoint" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_SETREADW:
			{
				ADDRESS pk_addr;
				uint32_t pk_size;
				SPACE pk_space;
				if(!ParseSetReadWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}

#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(!ServeSetReadWatchpoint(pk_addr, pk_size, pk_space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: Can't set read watchpoint" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_RMBRK:
			{
				ADDRESS pk_addr;
				string pk_symbol_name;
				if(!ParseRemoveBreakpoint(packet, pos, pk_addr, pk_symbol_name))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(pk_symbol_name.empty() ? !ServeRemoveBreakpoint(pk_addr) : !ServeRemoveBreakpoint(pk_symbol_name.c_str()))
				{
#ifdef DEBUG_NETSTUB
					cerr << "netstub: can't remove breakpoint" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_RMWRITEW:
			{
				ADDRESS pk_addr;
				uint32_t pk_size;
				SPACE pk_space;
				if(!ParseRemoveWriteWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(!ServeRemoveWriteWatchpoint(pk_addr, pk_size, pk_space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: Can't remove write watchpoint" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			case PKC_RMREADW:
			{
				ADDRESS pk_addr;
				uint32_t pk_size;
				SPACE pk_space;
				if(!ParseRemoveReadWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
				{
					PutStatusPacket(STATUS_INTERNAL_ERROR, device_id, tag + 1);
					stopped = true; ServeStop();
					return;
				}
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: processing command '" << GetName(pk_command) << "'" << endl;
#endif
				STATUS status = STATUS_OK;
				if(!ServeRemoveReadWatchpoint(pk_addr, pk_size, pk_space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: Can't remove read watchpoint" << endl;
#endif
					status = STATUS_FAILED;
				}
				if(!PutStatusPacket(status, device_id, tag + 1))
				{
					stopped = true; ServeStop();
					return;
				}
				break;
			}

			default:
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: ignoring command '" << GetName(pk_command) << "'" << endl;
#endif
				break;
		}
	}
}

template <class ADDRESS>
void NetStub<ADDRESS>::ServeStart()
{
}

template <class ADDRESS>
void NetStub<ADDRESS>::ServeStop()
{
}

template <class ADDRESS>
void NetStub<ADDRESS>::ServeIntr(uint32_t intr_id, bool level)
{
}


template <class ADDRESS>
void NetStub<ADDRESS>::ServeRun(uint64_t duration, TIME_UNIT duration_tu, uint64_t& t, TIME_UNIT& tu, list<TRAP>& traps)
{
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeRead(ADDRESS addr, void *buffer, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeWrite(ADDRESS addr, const void *buffer, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeReadRegister(const char *name, uint32_t& value)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeWriteRegister(const char *name, uint32_t value)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeSetBreakpoint(ADDRESS addr)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeSetBreakpoint(const char *symbol_name)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeSetReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeSetWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeRemoveBreakpoint(ADDRESS addr)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeRemoveBreakpoint(const char *symbol_name)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeRemoveReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ServeRemoveWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

} // end of namespace netstub
} // end of namespace util
} // end of namespace unisim

#endif
