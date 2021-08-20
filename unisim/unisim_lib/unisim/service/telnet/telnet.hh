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
 
#ifndef __UNISIM_SERVICE_TELNET_TELNET_HH__
#define __UNISIM_SERVICE_TELNET_TELNET_HH__

#include <unisim/service/interfaces/char_io.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <inttypes.h>
#include <string>
#include <vector>

namespace unisim {
namespace service {
namespace telnet {

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::service::interfaces::CharIO;
	
class Telnet : public Service<CharIO>
{
public:
	ServiceExport<CharIO> char_io_export;
	
	Telnet(const char *name, Object *parent = 0);
	virtual ~Telnet();

	virtual bool EndSetup();
	virtual void ResetCharIO();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
private:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool enable_negotiation;
	bool filter_null_character;
	bool filter_line_feed;

	int telnet_tcp_port;
	int telnet_sock;
	int state;
	uint8_t sb_opt;
	std::vector<uint8_t> sb_params_vec;

	Parameter<bool> param_verbose;
	Parameter<int> param_telnet_tcp_port;
	Parameter<bool> param_enable_negotiation;
	Parameter<bool> param_filter_null_character;
	Parameter<bool> param_filter_line_feed;

	unsigned int telnet_input_buffer_size;
	unsigned int telnet_input_buffer_index;
	uint8_t telnet_input_buffer[256];

	unsigned int telnet_output_buffer_size;
	uint8_t telnet_output_buffer[256];

	const char *GetOptName(uint8_t opt) const;
	void Dont(uint8_t opt);
	void Do(uint8_t opt);
	void Wont(uint8_t opt);
	void Will(uint8_t opt);
	void SubNegociation(uint8_t opt, uint8_t *params, unsigned int num_params);

	void TelnetFlushOutput();
	void TelnetPut(uint8_t v);
	bool TelnetGet(uint8_t& v);
	bool IsVerbose() const;
};

} // end of namespace telnet
} // end of namespace service
} // end of namespace unisim


#endif
