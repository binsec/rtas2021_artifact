/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
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
 *   - Neither the name of UPC nor the names of its contributors may be used to
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
 * Authors: Paula Casero (pcasero@upc.edu), Alejandro Schenzle (schenzle@upc.edu)
 */

#ifndef __UNISIM_COMPONENT_TLM_MESSAGE_ISA_HH__
#define __UNISIM_COMPONENT_TLM_MESSAGE_ISA_HH__

#include "unisim/component/cxx/isa/types.hh"

//Datatypes

namespace unisim {
namespace component {
namespace tlm {
namespace message {

template <uint32_t MAX_SIZE>
class ISARequest;

template <uint32_t MAX_SIZE>
class ISAResponse;

template <uint32_t MAX_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, const ISARequest<MAX_SIZE>& req);

template <uint32_t MAX_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, const ISAResponse<MAX_SIZE>& req);

template <uint32_t MAX_SIZE>
class ISARequest {
public:

	unisim::component::cxx::isa::TransactionType type;
	unisim::component::cxx::isa::ISASpace space;
	unisim::component::cxx::isa::isa_address_t addr;
	uint32_t size;
	uint8_t write_data[MAX_SIZE];

	friend unisim::kernel::logger::Logger& operator << <MAX_SIZE>(unisim::kernel::logger::Logger& os, const ISARequest<MAX_SIZE>& req);
};

template <uint32_t MAX_SIZE>
class ISAResponse {
public:
	uint8_t read_data[MAX_SIZE];

	friend unisim::kernel::logger::Logger& operator << <MAX_SIZE>(unisim::kernel::logger::Logger& os, const ISARequest<MAX_SIZE>& req);
};

template <uint32_t MAX_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, const ISARequest<MAX_SIZE>& req)
{
	os << std::hex << "ISA-REQ(space=";
	switch(req.space)
	{
		case unisim::component::cxx::isa::SP_IO: os << "I/O"; break;
		case unisim::component::cxx::isa::SP_MEM: os << "MEM"; break;
	}
	os << ",type=";
	switch(req.type)
	{
		case unisim::component::cxx::isa::TT_READ: os << "READ"; break;
		case unisim::component::cxx::isa::TT_WRITE: os << "WRITE"; break;
	}
	os << ",addr=";
	os << "0x" << req.addr;
	if(req.type == unisim::component::cxx::isa::TT_WRITE)
	{
		os << ",write_data=";
		uint32_t i;
		for(i = 0; i < req.size && i < MAX_SIZE; i++)
		{
			os << (unsigned int) req.write_data[i];
			if(i < req.size - 1) os << ",";
		}
	}
 	os << ")" << std::dec;
	return os;
}

template <uint32_t MAX_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, const ISAResponse<MAX_SIZE>& rsp)
{
	os << std::hex << "ISA-RSP(read_data=";
	uint32_t i;
	for(i = 0; i < MAX_SIZE; i++)
	{
		os << (unsigned int) rsp.read_data[i];
		if(i < MAX_SIZE - 1) os << ",";
	}
 	os << ")" << std::dec;
	return os;
}


} // end of namespace message
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_MESSAGE_ISA_HH__
