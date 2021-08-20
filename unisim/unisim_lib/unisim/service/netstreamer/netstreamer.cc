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
 
#include <unisim/service/netstreamer/netstreamer.hh>
#include <unisim/kernel/variable/netstreamer_protocol/netstreamer_protocol.hh>
#include <unisim/util/likely/likely.hh>

#include <cstring>
#include <cerrno>

namespace unisim {
namespace service {
namespace netstreamer {

NetStreamer::NetStreamer(const char *name, unisim::kernel::Object *parent)
	: Object(name, parent, "This service provides character I/O over TCP/IP")
	, unisim::kernel::Service<CharIO>(name, parent)
	, char_io_export("char-io-export", this)
	, logger(*this)
	, netstreamer(0, logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream())
	, param_verbose("verbose", this, netstreamer.verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, netstreamer.debug, "Enable/Disable debug (intended for developper)")
	, param_tcp_port("tcp-port", this, netstreamer.tcp_port, "TCP/IP port")
	, param_is_server("is-server", this, netstreamer.is_server, "if 1, act as a server, otherwise act as a client")
	, param_server_name("server-name", this, netstreamer.server_name, "if acting as a client, name of server to connect to")
	, param_protocol("protocol", this, netstreamer.protocol, "network protocol (raw or telnet)")
	, param_filter_null_character("filter-null-character", this, netstreamer.filter_null_character, "Whether to filter null character")
	, param_filter_line_feed("filter-line-feed", this, netstreamer.filter_line_feed, "Whether to filter line feed")
	, param_enable_telnet_binary("enable-telnet-binary", this, netstreamer.enable_telnet_binary, "enable/disable telnet binary option")
	, param_enable_telnet_echo("enable-telnet-echo", this, netstreamer.enable_telnet_echo, "enable/disable telnet echo option")
	, param_enable_telnet_suppress_go_ahead("enable-telnet-suppress-go-ahead", this, netstreamer.enable_telnet_suppress_go_ahead, "enable/disable telnet suppress go ahead option")
	, param_enable_telnet_linemode("enable-telnet-linemode", this, netstreamer.enable_telnet_linemode, "enable/disable telnet linemode option")
{
	param_tcp_port.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
}

NetStreamer::~NetStreamer()
{
}

void NetStreamer::Kill()
{
	netstreamer.Kill();
	unisim::kernel::Object::Kill();
}

bool NetStreamer::EndSetup()
{
	return netstreamer.Initialize();
}

void NetStreamer::ResetCharIO()
{
	netstreamer.Reset();
}

bool NetStreamer::GetChar(char& c)
{
	return netstreamer.GetChar(c);
}

void NetStreamer::PutChar(char c)
{
	netstreamer.PutChar(c);
}

void NetStreamer::FlushChars()
{
	netstreamer.FlushOutput();
}

} // end of namespace netstreamer
} // end of namespace service
} // end of namespace unisim
