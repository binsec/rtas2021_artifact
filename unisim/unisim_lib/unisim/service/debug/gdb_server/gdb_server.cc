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
 */

#include <unisim/kernel/variable/variable.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/util/likely/likely.hh>
#include <cctype>

#include <errno.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>
#include <unistd.h>

#endif

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

/////////////////////////////// GDBServerBase /////////////////////////////////

const uint64_t GDBServerBase::SERVER_ACCEPT_POLL_PERIOD_MS;
const uint64_t GDBServerBase::NON_BLOCKING_READ_POLL_PERIOD_MS;
const uint64_t GDBServerBase::NON_BLOCKING_WRITE_POLL_PERIOD_MS;

GDBServerBase::GDBServerBase(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent)
	, logger(*this)
	, tcp_port(12345)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	, sock(INVALID_SOCKET)
#else
	, sock(-1)
#endif
	, sock_error(false)
	, input_buffer_size(0)
	, input_buffer_index(0)
	, output_buffer_size(0)
	, verbose(false)
	, debug(false)
	, remote_serial_protocol_input_traffic_recording_filename()
	, remote_serial_protocol_input_traffic_recording_file()
	, remote_serial_protocol_output_traffic_recording_filename()
	, remote_serial_protocol_output_traffic_recording_file()
	, param_tcp_port("tcp-port", this, tcp_port, "TCP/IP port to listen waiting for a GDB client connection")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, debug, "Enable/Disable debug (intended for developper)")
	, param_remote_serial_protocol_input_traffic_recording_filename("remote-serial-protocol-input-traffic-recording-filename", this, remote_serial_protocol_input_traffic_recording_filename, "Filename where to record GDB remote serial protocol input traffic")
	, param_remote_serial_protocol_output_traffic_recording_filename("remote-serial-protocol-output-traffic-recording-filename", this, remote_serial_protocol_output_traffic_recording_filename, "Filename where to record GDB remote serial protocol input traffic")
{
	param_tcp_port.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	param_tcp_port.SetMutable(false);
	
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		throw std::runtime_error("WSAStartup failed: Windows sockets not available");
	}
#endif
}

GDBServerBase::~GDBServerBase()
{
	if(remote_serial_protocol_input_traffic_recording_file.is_open())
	{
		remote_serial_protocol_input_traffic_recording_file.close();
	}
	if(remote_serial_protocol_output_traffic_recording_file.is_open())
	{
		remote_serial_protocol_output_traffic_recording_file.close();
	}

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	//releases the winsock2 resources
	WSACleanup();
#endif
}

bool GDBServerBase::EndSetup()
{
	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer_size = 0;
	
	return true;
}

bool GDBServerBase::StartServer()
{
	struct sockaddr_in addr;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	SOCKET server_sock;
#else
	int server_sock;
#endif

	server_sock = socket(AF_INET, SOCK_STREAM, 0);

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(server_sock == INVALID_SOCKET)
#else
	if(server_sock < 0)
#endif
	{
		logger << DebugError << "failed obtaining a server socket (" << GetLastErrorString() << ")" << EndDebugError;
		return false;
	}

	/* Ask for non-blocking accept on server socket */
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	u_long ServerSocketNonBlock = 1;
	if(ioctlsocket(server_sock, FIONBIO, &ServerSocketNonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket(FIONBIO) on server socked failed (" << GetLastErrorString() << ")" << EndDebugError;
		closesocket(server_sock);
		return false;
	}
#else
	int server_socket_flag = fcntl(server_sock, F_GETFL, 0);

	if(server_socket_flag < 0)
	{
		logger << DebugError << "fcntl(F_GETFL) on server socket failed (" << GetLastErrorString() << ")" << EndDebugError;
		close(server_sock);
		return false;
	}

	if(fcntl(server_sock, F_SETFL, server_socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl(F_SETFL, O_NONBLOCK) on server socket failed (" << GetLastErrorString() << ")" << EndDebugError;
		close(server_sock);
		return false;
	}
#endif

#if 1
	/* ask for reusing TCP port */
    int opt_so_reuseaddr = 1;
    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_so_reuseaddr, sizeof(opt_so_reuseaddr)) != 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt(SOL_SOCKET, SO_REUSEADDR) on server socket failed while requesting port reuse (" << GetLastErrorString() << ")" << EndDebugWarning;
		}
	}
#endif

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(tcp_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) != 0)
	{
		logger << DebugError << "Bind failed (" << GetLastErrorString() << "). TCP Port #" << tcp_port << " may be already in use. Please specify another port in " << param_tcp_port.GetName() << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(listen(server_sock, 1) != 0)
	{
		logger << DebugError << "Listen failed (" << GetLastErrorString() << ")" << EndDebugError;
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

	logger << DebugInfo << "Listening on TCP port #" << tcp_port << EndDebugInfo;
	addr_len = sizeof(addr);
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	sock = INVALID_SOCKET;
#else
	sock = -1;
#endif

	while(!Killed())
	{
		sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		if(sock != INVALID_SOCKET)
#else
		if(sock >= 0)
#endif
		{
			// a client has connected
			logger << DebugInfo << "Connection with GDB client established on TCP Port #" << tcp_port << EndDebugInfo;
			break;
		}
			
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		if(WSAGetLastError() != WSAEWOULDBLOCK)
#else
		if((errno != EAGAIN) && (errno != EWOULDBLOCK))
#endif
		{
			logger << DebugError << "accept failed (" << GetLastErrorString() << ")" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}
			
		WaitTime(SERVER_ACCEPT_POLL_PERIOD_MS); // retry later
	}
	
	if(Killed())
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		logger << DebugInfo << "Server shuts down" << EndDebugInfo;
		return false;
	}

    /* set short latency */
    int opt_tcp_nodelay = 1;
    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &opt_tcp_nodelay, sizeof(opt_tcp_nodelay)) != 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt(IPPROTO_TCP, TCP_NODELAY) on socket failed (" << GetLastErrorString() << ") while requesting short latency" << EndDebugWarning;
		}
	}

#if 0
    /* unset TCP keep alive */
    int opt_tcp_keepalive = 0;
    if(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt_tcp_keepalive, sizeof(opt_tcp_keepalive)) != 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt(SOL_SOCKET, SO_KEEPALIVE) on socket failed (" << GetLastErrorString() << ") while requesting keep alive off" << EndDebugWarning;
		}
	}

	/* set linger */
	struct linger opt_tcp_linger;
	opt_tcp_linger.l_onoff = 1;    /* linger active */
	opt_tcp_linger.l_linger = 5;   /* how many seconds to linger for */
    if(setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *) &opt_tcp_linger, sizeof(opt_tcp_linger)) != 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt(SOL_SOCKET, SO_LINGER) on socket failed (" << GetLastErrorString() << ") while requesting linger (send output before close or after timeout)" << EndDebugWarning;
		}
	}
#endif

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	u_long NonBlock = 1;
	if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket(FBIONIO) on socket failed (" << GetLastErrorString() << ")" << EndDebugError;
		closesocket(server_sock);
		closesocket(sock);
		sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(sock, F_GETFL, 0);

	if(socket_flag < 0)
	{
		logger << DebugError << "fcntl(F_GETFL) on socket failed (" << GetLastErrorString() << ")" << EndDebugError;
		close(server_sock);
		close(sock);
		sock = -1;
		return false;
	}

	/* Ask for non-blocking reads on socket */
	if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl(F_SETFL, O_NONBLOCK) on socket failed (" << GetLastErrorString() << ")" << EndDebugError;
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

	if(!remote_serial_protocol_input_traffic_recording_filename.empty())
	{
		if(verbose)
		{
			logger << DebugInfo << "Opening File \"" << remote_serial_protocol_input_traffic_recording_filename << "\" for recording remote serial protocol (RSP) input traffic" << EndDebugInfo;
		}
		remote_serial_protocol_input_traffic_recording_file.open(remote_serial_protocol_input_traffic_recording_filename.c_str());
		
		if(remote_serial_protocol_input_traffic_recording_file.fail())
		{
			logger << DebugWarning << "Can't open File \"" << remote_serial_protocol_input_traffic_recording_filename << "\"" << EndDebugWarning;
		}
	}
	
	if(!remote_serial_protocol_output_traffic_recording_filename.empty())
	{
		if(verbose)
		{
			logger << DebugInfo << "Opening File \"" << remote_serial_protocol_output_traffic_recording_filename << "\" for recording remote serial protocol (RSP) output traffic" << EndDebugInfo;
		}
		remote_serial_protocol_output_traffic_recording_file.open(remote_serial_protocol_output_traffic_recording_filename.c_str());
		
		if(remote_serial_protocol_output_traffic_recording_file.fail())
		{
			logger << DebugWarning << "Can't open File \"" << remote_serial_protocol_output_traffic_recording_filename << "\"" << EndDebugWarning;
		}
	}

	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer_size = 0;

	sock_error = false;
	return true;
}

bool GDBServerBase::StopServer()
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(sock != INVALID_SOCKET)
#else
	if(sock >= 0)
#endif
	{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(sock);
		sock = INVALID_SOCKET;
#else
		close(sock);
		sock = -1;
#endif
		
		logger << DebugInfo << "Connection with GDB client closed" << EndDebugInfo;
		if(Killed())
		{
			logger << DebugInfo << "Server shuts down" << EndDebugInfo;
		}
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "GDB command processing thread: Unlocking" << EndDebugInfo;
	}
	
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	//releases the winsock2 resources
	WSACleanup();
#endif
	
	if(remote_serial_protocol_input_traffic_recording_file.is_open())
	{
		remote_serial_protocol_input_traffic_recording_file.close();
	}
	if(remote_serial_protocol_output_traffic_recording_file.is_open())
	{
		remote_serial_protocol_output_traffic_recording_file.close();
	}

	return true;
}

void GDBServerBase::WaitTime(unsigned int msec)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(!Killed())
	{
		Sleep(msec);
	}
#else
	struct timespec tim_req, tim_rem;
	tim_req.tv_sec = msec / 1000;
	tim_req.tv_nsec = 1000000 * (msec % 1000);
	
	while(!Killed())
	{
		int status = nanosleep(&tim_req, &tim_rem);
		
		if(status == 0) break;
		
		if(status != -1) break;
		
		if(errno != EINTR) break;

		tim_req.tv_nsec = tim_rem.tv_nsec;
	}
#endif
}

std::string GDBServerBase::GetLastErrorString()
{
	std::stringstream err_sstr;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	int err_code = WSAGetLastError();
	err_sstr << "Error #" << err_code << ": ";
	char *err_c_str = 0;
	LPTSTR *lptstr_err_str = NULL;
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
	              NULL,
	              err_code,
	              0,
	              (LPTSTR) &lptstr_err_str,
	              0,
	              NULL) == 0)
	{
		return err_sstr.str();
	}
#if UNICODE
	size_t error_c_str_length = wcstombs(NULL, lptstr_err_str, 0);
    err_c_str = new char[error_c_str_length + 1];
	memset(err_c_str, 0, error_c_str_length);
    if(wcstombs(err_c_str, lptstr_err_str, error_c_str_length + 1) < 0)
	{
		LocalFree(lptstr_err_str);
		delete[] err_c_str;
		return err_sstr.str();
	}
#else
	err_c_str = (char *) lptstr_err_str;
#endif
	
	err_sstr << err_c_str;
	
	LocalFree(lptstr_err_str);
#if UNICODE
	delete[] err_c_str;
#endif
	
#else
	err_sstr << "Error #" << errno << ": " << strerror(errno);
#endif
	return err_sstr.str();
}

bool GDBServerBase::GetChar(char& c, bool blocking)
{
	if(sock_error) return false;
	
	if(input_buffer_size == 0)
	{
		do
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			if(sock == INVALID_SOCKET) return false;
			int r = recv(sock, input_buffer, sizeof(input_buffer), 0);
			if((r == 0) || (r == SOCKET_ERROR))
#else
			if(sock < 0) return false;
			ssize_t r = read(sock, input_buffer, sizeof(input_buffer));
			if(r <= 0)
#endif
			{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				if(r == SOCKET_ERROR)
#else
				if(r < 0)
#endif
				{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
					if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
					if((errno == EAGAIN) || (errno == EWOULDBLOCK))
#endif
					{
						if(blocking && !Killed())
						{
							WaitTime(NON_BLOCKING_READ_POLL_PERIOD_MS);
							continue;
						}
						else
						{
							return false;
						}
					}
				}
				
				logger << DebugError << "can't read from socket (" << GetLastErrorString() << ")" << EndDebugError;
				sock_error = true;
				return false;
			}
			input_buffer_index = 0;
			input_buffer_size = r;
			break;
		} while(1);
	}

	if(input_buffer_size)
	{
		c = input_buffer[input_buffer_index++];
		input_buffer_size--;
		
		if(remote_serial_protocol_input_traffic_recording_file.is_open())
		{
			remote_serial_protocol_input_traffic_recording_file.put(c);
		}
		
		return true;
	}
	
	return false;
}

bool GDBServerBase::FlushOutput()
{
	if(sock_error) return false;
	
	if(output_buffer_size > 0)
	{
//		std::cerr << "begin: output_buffer_size = " << output_buffer_size << std::endl;
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				if(r == SOCKET_ERROR)
#else
				if(r < 0)
#endif
				{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
					if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
					if((errno == EAGAIN) || (errno == EWOULDBLOCK))
#endif
					{
						if(!Killed())
						{
							WaitTime(NON_BLOCKING_WRITE_POLL_PERIOD_MS);
							continue;
						}
						else
						{
							return false;
						}
					}
					
					logger << DebugError << "can't write to socket (" << GetLastErrorString() << ")" << EndDebugError;
					sock_error = true;
					return false;
				}
				
				WaitTime(NON_BLOCKING_READ_POLL_PERIOD_MS);
				continue;
			}

			if(remote_serial_protocol_output_traffic_recording_file.is_open())
			{
				remote_serial_protocol_output_traffic_recording_file.write(output_buffer + index, r);
			}
			
			index += r;
			output_buffer_size -= r;
//			std::cerr << "output_buffer_size = " << output_buffer_size << std::endl;
		}
		while(output_buffer_size > 0);
	}
//	std::cerr << "end: output_buffer_size = " << output_buffer_size << std::endl;
	return true;
}

bool GDBServerBase::PutChar(char c)
{
	if(output_buffer_size >= sizeof(output_buffer))
	{
		if(!FlushOutput()) return false;
	}

	output_buffer[output_buffer_size++] = c;
	return true;
}

bool GDBServerBase::IsConnected() const
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	return (sock != INVALID_SOCKET);
#else
	return (sock >= 0);
#endif
}

//////////////////////////////// GDBRegister //////////////////////////////////

GDBRegister::GDBRegister()
	: name()
	, bitsize(0)
	, arch_regs()
	, endian(GDB_LITTLE_ENDIAN)
	, reg_num(0)
	, type()
	, group()
{
}

GDBRegister::GDBRegister(const std::string& reg_name, int reg_bitsize, GDBEndian reg_endian, unsigned int _reg_num, const std::string& _type, const std::string& _group)
	: name(reg_name)
	, bitsize(reg_bitsize)
	, arch_regs()
	, endian(reg_endian)
	, reg_num(_reg_num)
	, type(_type)
	, group(_group)
{
}

void GDBRegister::SetRegisterInterface(unsigned int prc_num, unisim::service::interfaces::Register *reg)
{
	if(prc_num >= arch_regs.size())
	{
		arch_regs.resize(prc_num + 1);
	}
	
	arch_regs[prc_num] = reg;
}

bool GDBRegister::SetValue(unsigned int prc_num, const std::string& hex)
{
	unisim::service::interfaces::Register* reg = GetRegisterInterface(prc_num);

	if (not reg) return false;

	unsigned int len = hex.length(), pos = 0;
	int size = bitsize / 8;
	uint8_t value[size];
	std::fill(&value[0], &value[size], 0);

	if(endian == GDB_BIG_ENDIAN)
	{
#if BYTE_ORDER == BIG_ENDIAN
		for (int i = 0; i < size && pos < len; i++, pos += 2)
#else
		for (int i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for (int i = 0; i < size && pos < len; i++, pos += 2)
#else
		for (int i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}

	reg->SetValue(&value);

	return true;
}

bool GDBRegister::SetValue(unsigned int prc_num, const void *buffer)
{
	unisim::service::interfaces::Register* reg = GetRegisterInterface(prc_num);

	if (not reg) return false;
	
	reg->SetValue(buffer);
	
	return true;
}

bool GDBRegister::GetValue(unsigned int prc_num, std::string& hex) const
{
	if (unisim::service::interfaces::Register const* reg = GetRegisterInterface(prc_num))
	{
		int size = bitsize / 8;
		uint8_t value[size];
		std::fill(&value[0], &value[size], 0);
		
		reg->GetValue(&value);
		hex.clear();
		
		if(endian == GDB_BIG_ENDIAN)
		{
#if BYTE_ORDER == BIG_ENDIAN
			for (int i = 0; i < size; i++)
#else
			for (int i = size - 1; i >= 0; i--)
#endif
			{
				hex += Nibble2HexChar(value[i] >> 4);
				hex += Nibble2HexChar(value[i] & 0xf);
			}
		}
		else
		{
#if BYTE_ORDER == LITTLE_ENDIAN
			for (int i = 0; i < size; i++)
#else
			for (int i = size - 1; i >= 0; i--)
#endif
			{
				hex += Nibble2HexChar(value[i] >> 4);
				hex += Nibble2HexChar(value[i] & 0xf);
			}
		}
		return true;
	}

	// else no register access
	
	hex = std::string(bitsize/4,'x');
	
	return false;
}

bool GDBRegister::GetValue(unsigned int prc_num, void *buffer) const
{
	int size = bitsize / 8;
	memset(buffer, 0, size);
	
	if (unisim::service::interfaces::Register* reg = GetRegisterInterface(prc_num))
	{
		reg->GetValue(buffer);
		return true;
	}
	
	return false;
}

std::ostream& GDBRegister::ToXML(std::ostream& os, unsigned int _reg_num) const
{
	os << "<reg name=\"" << name << "\" bitsize=\"" << bitsize << "\"";
	if(reg_num != _reg_num)
	{
		os << " regnum=\"" << reg_num << "\"";
	}
	if(!type.empty())
	{
		os << " type=\"" << type << "\"";
	}
	if(!group.empty())
	{
		os << " group=\"" << group << "\"";
	}
	os << "/>";
	
	return os;
}

///////////////////////////////// GDBFeature //////////////////////////////////

GDBFeature::GDBFeature(const std::string& _name, unsigned int _id)
	: name(_name)
	, id(_id)
	, gdb_registers()
{
}

void GDBFeature::AddRegister(const GDBRegister *gdb_register)
{
	gdb_registers.push_back(gdb_register);
}

unsigned int GDBFeature::GetId() const
{
	return id;
}

std::ostream& GDBFeature::ToXML(std::ostream& os, std::string req_filename) const
{
	std::stringstream feature_filename_sstr;
	feature_filename_sstr << "feature" << id << ".xml";
	std::string feature_filename(feature_filename_sstr.str());

	if(req_filename == feature_filename)
	{
		os << "<feature name=\"" << name << "\"";

		unsigned int n = gdb_registers.size();
		if(n > 0)
		{
			os << ">";
			
			unsigned int reg_num = (unsigned int) -1;
			for (unsigned i = 0; i < n; i++)
			{
				const GDBRegister *gdb_reg = gdb_registers[i];
				if(gdb_reg)
				{
					gdb_reg->ToXML(os, reg_num);
					
					reg_num = gdb_reg->GetRegNum() + 1;
				}
			}
			
			os << "</feature>";
		}
		else
		{
			os << "/>";
		}
	}
	
	return os;
}

std::ostream& operator << (std::ostream& os, const GDBServerError& gdb_server_error)
{
	switch(gdb_server_error)
	{
		case GDB_SERVER_ERROR_EXPECTING_HEX              : os << "expecting hexadecimal value"; break;
		case GDB_SERVER_ERROR_EXPECTING_COMMA            : os << "expecting ','"; break;
		case GDB_SERVER_ERROR_EXPECTING_COLON            : os << "expecting ':'"; break;
		case GDB_SERVER_ERROR_EXPECTING_SEMICOLON        : os << "expecting ';'"; break;
		case GDB_SERVER_ERROR_EXPECTING_ASSIGNMENT       : os << "expecting '='"; break;
		case GDB_SERVER_ERROR_GARBAGE                    : os << "got garbage (extra characters)"; break;
		case GDB_SERVER_ERROR_EXPECTING_THREAD_ID        : os << "expecting thread id"; break;
		case GDB_SERVER_ERROR_INVALID_THREAD_ID          : os << "invalid thread id"; break;
		case GDB_SERVER_ERROR_EXPECTING_OPERATION        : os << "expecting operation"; break;
		case GDB_SERVER_ERROR_INVALID_OPERATION          : os << "invalid operation"; break;
		case GDB_SERVER_ERROR_CANT_DEBUG_PROCESSOR       : os << "can't debug processor"; break;
		case GDB_SERVER_ERROR_UNKNOWN_REGISTER           : os << "unknown register"; break;
		case GDB_SERVER_ERROR_CANT_READ_REGISTER         : os << "can't read register"; break;
		case GDB_SERVER_ERROR_CANT_WRITE_REGISTER        : os << "can't write register"; break;
		case GDB_SERVER_ERROR_CANT_READ_MEMORY           : os << "can't read memory"; break;
		case GDB_SERVER_ERROR_CANT_WRITE_MEMORY          : os << "can't write memory"; break;
		case GDB_SERVER_ERROR_MALFORMED_BINARY_DATA      : os << "malformed binary data"; break;
		case GDB_SERVER_CANT_SET_BREAKPOINT_WATCHPOINT   : os << "can't set breakpoint/watchpoint"; break;
		case GDB_SERVER_CANT_REMOVE_BREAKPOINT_WATCHPOINT: os << "can't remove breakpoint/watchpoint"; break;
		case GDB_SERVER_ERROR_EXPECTING_ACTION           : os << "expecting action"; break;
		default                                          : os << "?"; break;
	}
	
	return os;
}

std::ostream& operator << (std::ostream& os, const GDBServerAction& gdb_server_action)
{
	switch(gdb_server_action)
	{
		case GDB_SERVER_NO_ACTION      : os << "no action"; break;
		case GDB_SERVER_ACTION_STEP    : os << "step"; break;
		case GDB_SERVER_ACTION_CONTINUE: os << "continue"; break;
		default                        : os << "?"; break;
	}
	
	return os;
}

std::ostream& operator << (std::ostream& os, const GDBMode& gdb_mode)
{
	switch(gdb_mode)
	{
		case GDB_MODE_SINGLE_THREAD: os << "SINGLE_THREAD"; break;
		case GDB_MODE_MULTI_THREAD: os << "MULTI_THREAD"; break;
		default          : os << "?"; break;
	}
	
	return os;
}

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace variable {

//////////////////// Variable<GDBWaitConnectionMode> //////////////////////////

using unisim::service::debug::gdb_server::GDBWaitConnectionMode;
using unisim::service::debug::gdb_server::GDB_WAIT_CONNECTION_NEVER;
using unisim::service::debug::gdb_server::GDB_WAIT_CONNECTION_STARTUP_ONLY;
using unisim::service::debug::gdb_server::GDB_WAIT_CONNECTION_ALWAYS;

template <> Variable<GDBWaitConnectionMode>::Variable(const char *_name, Object *_object, GDBWaitConnectionMode& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("never");
	AddEnumeratedValue("startup-only");
	AddEnumeratedValue("always");
}

template <>
const char *Variable<GDBWaitConnectionMode>::GetDataTypeName() const
{
	return "gdb-wait-connection-mode";
}

template <>
VariableBase::DataType Variable<GDBWaitConnectionMode>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<GDBWaitConnectionMode>::GetBitSize() const
{
	return 2;
}

template <> Variable<GDBWaitConnectionMode>::operator bool () const { return *storage != GDB_WAIT_CONNECTION_NEVER; }
template <> Variable<GDBWaitConnectionMode>::operator long long () const { return *storage; }
template <> Variable<GDBWaitConnectionMode>::operator unsigned long long () const { return *storage; }
template <> Variable<GDBWaitConnectionMode>::operator double () const { return (double)(*storage); }
template <> Variable<GDBWaitConnectionMode>::operator std::string () const
{
	switch(*storage)
	{
		case GDB_WAIT_CONNECTION_NEVER: return std::string("never");
		case GDB_WAIT_CONNECTION_STARTUP_ONLY: return std::string("startup-only");
		case GDB_WAIT_CONNECTION_ALWAYS: return std::string("always");
	}
	return std::string("?");
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (bool value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (long long value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch(value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch(value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (double value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_WAIT_CONNECTION_NEVER:
			case GDB_WAIT_CONNECTION_STARTUP_ONLY:
			case GDB_WAIT_CONNECTION_ALWAYS:
				tmp = (GDBWaitConnectionMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBWaitConnectionMode>::operator = (const char *value)
{
	if(IsMutable())
	{
		GDBWaitConnectionMode tmp = *storage;
		if(std::string(value) == std::string("never")) tmp = GDB_WAIT_CONNECTION_NEVER;
		else if(std::string(value) == std::string("startup-only")) tmp = GDB_WAIT_CONNECTION_STARTUP_ONLY;
		else if(std::string(value) == std::string("always")) tmp = GDB_WAIT_CONNECTION_ALWAYS;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<GDBWaitConnectionMode>;

//////////////////////////// Variable<GDBMode> ////////////////////////////////

using unisim::service::debug::gdb_server::GDBMode;
using unisim::service::debug::gdb_server::GDB_MODE_SINGLE_THREAD;
using unisim::service::debug::gdb_server::GDB_MODE_MULTI_THREAD;

template <> Variable<GDBMode>::Variable(const char *_name, Object *_object, GDBMode& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("single-thread");
	AddEnumeratedValue("multi-thread");
}

template <>
const char *Variable<GDBMode>::GetDataTypeName() const
{
	return "gdb-mode";
}

template <>
VariableBase::DataType Variable<GDBMode>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<GDBMode>::GetBitSize() const
{
	return 2;
}

template <> Variable<GDBMode>::operator bool () const { return *storage != GDB_MODE_SINGLE_THREAD; }
template <> Variable<GDBMode>::operator long long () const { return *storage; }
template <> Variable<GDBMode>::operator unsigned long long () const { return *storage; }
template <> Variable<GDBMode>::operator double () const { return (double)(*storage); }
template <> Variable<GDBMode>::operator std::string () const
{
	switch(*storage)
	{
		case GDB_MODE_SINGLE_THREAD: return std::string("single-thread");
		case GDB_MODE_MULTI_THREAD: return std::string("multi-thread");
	}
	return std::string("?");
}

template <> VariableBase& Variable<GDBMode>::operator = (bool value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (long long value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch(value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch(value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (double value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		switch((unsigned int) value)
		{
			case GDB_MODE_SINGLE_THREAD:
			case GDB_MODE_MULTI_THREAD:
				tmp = (GDBMode)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<GDBMode>::operator = (const char *value)
{
	if(IsMutable())
	{
		GDBMode tmp = *storage;
		if(std::string(value) == std::string("single-thread")) tmp = GDB_MODE_SINGLE_THREAD;
		else if(std::string(value) == std::string("multi-thread")) tmp = GDB_MODE_MULTI_THREAD;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<GDBMode>;

} // end of variable namespace
} // end of kernel namespace
} // end of unisim namespace
