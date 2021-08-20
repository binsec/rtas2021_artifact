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
 
#ifndef __UNISIM_SERVICE_NETSTREAMER_NETSTREAMER_HH__
#define __UNISIM_SERVICE_NETSTREAMER_NETSTREAMER_HH__

#include <unisim/service/interfaces/char_io.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/netstreamer/netstreamer.hh>
#include <string>
#include <vector>
#include <stdint.h>
#include <pthread.h>

namespace unisim {
namespace service {
namespace netstreamer {

class NetStreamer : public unisim::kernel::Service<unisim::service::interfaces::CharIO>
{
public:
	unisim::kernel::ServiceExport<unisim::service::interfaces::CharIO> char_io_export;
	
	NetStreamer(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~NetStreamer();

	virtual bool EndSetup();
	virtual void Kill();
	
	// unisim::service::interfaces::CharIO
	virtual void ResetCharIO();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
private:
	unisim::kernel::logger::Logger logger;
	unisim::util::netstreamer::NetStreamer netstreamer;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<bool> param_debug;
	unisim::kernel::variable::Parameter<int> param_tcp_port;
	unisim::kernel::variable::Parameter<bool> param_is_server;
	unisim::kernel::variable::Parameter<std::string> param_server_name;
	unisim::kernel::variable::Parameter<unisim::util::netstreamer::NetStreamerProtocol> param_protocol;
	unisim::kernel::variable::Parameter<bool> param_filter_null_character;
	unisim::kernel::variable::Parameter<bool> param_filter_line_feed;
	unisim::kernel::variable::Parameter<bool> param_enable_telnet_binary;
	unisim::kernel::variable::Parameter<bool> param_enable_telnet_echo;
	unisim::kernel::variable::Parameter<bool> param_enable_telnet_suppress_go_ahead;
	unisim::kernel::variable::Parameter<bool> param_enable_telnet_linemode;
};

} // end of namespace netstreamer
} // end of namespace service
} // end of namespace unisim


#endif
