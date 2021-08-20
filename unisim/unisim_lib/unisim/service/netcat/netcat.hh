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
 
#ifndef __UNISIM_SERVICE_NETCAT_NETCAT_HH__
#define __UNISIM_SERVICE_NETCAT_NETCAT_HH__

#include <unisim/service/interfaces/char_io.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <inttypes.h>
#include <string>
#include <vector>

namespace unisim {
namespace service {
namespace netcat {

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::service::interfaces::CharIO;

class Netcat : public Service<CharIO>
{
public:
	ServiceExport<CharIO> char_io_export;
	
	Netcat(const char *name, Object *parent = 0);
	virtual ~Netcat();

	virtual bool EndSetup();
	virtual void Reset();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
private:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool is_server;
	std::string server_name;

	int tcp_port;
	int sock;

	Parameter<bool> param_verbose;
	Parameter<int> param_tcp_port;
	Parameter<bool> param_is_server;
	Parameter<std::string> param_server_name;

	unsigned int input_buffer_size;
	unsigned int input_buffer_index;
	uint8_t input_buffer[256];

	unsigned int output_buffer_size;
	uint8_t output_buffer[256];

	void FlushOutput();
	void Put(uint8_t v);
	bool Get(uint8_t& v);
};

} // end of namespace netcat
} // end of namespace service
} // end of namespace unisim


#endif
