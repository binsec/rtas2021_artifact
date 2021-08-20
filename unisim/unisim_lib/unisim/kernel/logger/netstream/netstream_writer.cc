/*
 *  Copyright (c) 2008,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr) 
 *          Daniel Gracia Pérez (daniel.gracia-perez@cea.fr)
 */

#include <unisim/kernel/variable/netstreamer_protocol/netstreamer_protocol.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

namespace unisim {
namespace kernel {
namespace logger {
namespace netstream {

Writer::Writer()
	: unisim::kernel::Object("netstream", Logger::StaticServerInstance(), "TCP Network Stream Log Writer")
	, Printer()
	, enable(false)
	, enable_color(false)
	, param_enable("enable", this, enable, "Show logger output character stream")
	, param_enable_color("enable-color", this, enable_color, "Colorize logger output (only works if Character Stream Log Writer is enabled)")
	, mutex()
	, in_print(false)
	, netstreamer(GetName(), std::cerr, std::cerr, std::cerr)
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
	
	pthread_mutex_init(&mutex, NULL);
	
	if(!netstreamer.Initialize())
	{
		this->Stop(-1);
	}
}

Writer::~Writer()
{
	pthread_mutex_destroy(&mutex);
}

void Writer::Print(mode_t mode, const char *name, const char *buffer)
{
	pthread_mutex_lock(&mutex);
	if(in_print) return; // avoid reentrance
	in_print = true;
	
	std::ostringstream sstr;
	
	if(enable_color)
	{
		switch(mode)
		{
			case INFO_MODE: sstr << "\033[36m"; break;
			case WARNING_MODE: sstr << "\033[33m"; break;
			case ERROR_MODE: sstr << "\033[31m"; break;
			default: break;
		}
	}
	sstr << name << ": ";
	switch(mode)
	{
		case WARNING_MODE: sstr << "WARNING! "; break;
		case ERROR_MODE: sstr << "ERROR! "; break;
		default: break;
	}
	int prefix_length = strlen(name) + 2;
	std::string prefix(prefix_length, ' ');
	const char *b = buffer;
	for (const char *p = strchr(b, '\n'); p != NULL; p = strchr(b, '\n'))
	{
		sstr.write(b, p - b);
		sstr << std::endl << prefix;
		b = p + 1;
	}
	sstr << b << std::endl;
		
	if (enable_color) sstr << "\033[0m";
	
	netstreamer.PutString(sstr.str());
	
	in_print = false;
	pthread_mutex_unlock(&mutex);
}

} // end of namespace netstream
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
