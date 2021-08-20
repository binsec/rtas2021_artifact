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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_MESSAGE_SNOOPINGFSB_HH__
#define __UNISIM_COMPONENT_TLM_MESSAGE_SNOOPINGFSB_HH__

#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/component/tlm/debug/transaction_spy.hh"
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm {
namespace message {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS, unsigned int DATA_SIZE>
class SnoopingFSBRequest;
template <unsigned int DATA_SIZE>
class SnoopingFSBResponse;
template <class ADDRESS, unsigned int DATA_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, const SnoopingFSBRequest<ADDRESS, DATA_SIZE>& req);
template <unsigned int DATA_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, const SnoopingFSBResponse<DATA_SIZE>& rsp);

template <class ADDRESS, unsigned int DATA_SIZE>
class SnoopingFSBRequest {
public:
	enum Type {
		READ,         // Read request
		READX,        // Read with intent to modify
		WRITE,        // Write request
		INV_BLOCK,    // Invalidate block
		FLUSH_BLOCK,  // Flush block
		ZERO_BLOCK,   // Fill in block with zero
		INV_TLB       // Invalidate TLB set
	};
	
	Type type;       // Request type
 	bool global;      // true if the request must be snooped by other processors
	ADDRESS addr;    // Address
	unsigned int size;              // Size of bus transfer (<= DATA_SIZE)
	uint8_t write_data[DATA_SIZE];  // Data to write into memory

	friend unisim::kernel::logger::Logger& operator << <ADDRESS, DATA_SIZE>(unisim::kernel::logger::Logger& os,
			const SnoopingFSBRequest<ADDRESS, DATA_SIZE>& req);
};

template <unsigned int DATA_SIZE>
class SnoopingFSBResponse {
public:
	typedef enum {
		RS_MISS     = 0,   // target processor doesn't have the block
		RS_SHARED   = 1,   // target processor has an unmodified copy of the block
		RS_MODIFIED = 2,   // target processor has a modified copy of the block which need to be written back to memory
		RS_BUSY     = 4    // target processor is busy and can't response to the snoop request. The initiator should retry later.
	} ReadStatus;


	ReadStatus read_status;       // Status of the target processor in case of READ or READX
	uint8_t read_data[DATA_SIZE]; // Data read from memory/target processor caches

	friend unisim::kernel::logger::Logger& operator << <DATA_SIZE>(unisim::kernel::logger::Logger& os, 
			const SnoopingFSBResponse<DATA_SIZE>& rsp);
};

template <class ADDRESS, unsigned int DATA_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, 
		const SnoopingFSBRequest<ADDRESS, DATA_SIZE>& req) {
	
	typedef SnoopingFSBRequest<ADDRESS, DATA_SIZE> ReqType;
	
	os << "- type = ";
	switch(req.type) {
	case ReqType::READ:
		os << "READ";
		break;
	case ReqType::READX:
		os << "READX";
		break;
	case ReqType::WRITE:
		os << "WRITE";
		break;
	case ReqType::INV_BLOCK:
		os << "INV_BLOCK";
		break;
	case ReqType::FLUSH_BLOCK:
		os << "FLUSH_BLOCK";
		break;
	case ReqType::ZERO_BLOCK:
		os << "ZERO_BLOCK";
		break;
	case ReqType::INV_TLB:
		os << "INV_TLB";
		break;
	}
	os << std::endl;
	os << "- global = " << (req.global?"TRUE":"FALSE") << std::endl;
	os << "- address = 0x" << std::hex << req.addr << std::dec << std::endl;
	os << "- size = " << req.size;
	if(req.type == ReqType::WRITE) {
		os << std::endl;
		os << "- write_data(hex) =" << std::hex;
		for(unsigned int i = 0; i < req.size; i++) {
			os << " " << (unsigned int)req.write_data[i];
		}
		os << std::dec;
	}
	return os;
}

template <unsigned int DATA_SIZE>
unisim::kernel::logger::Logger& operator << (unisim::kernel::logger::Logger& os, 
		const SnoopingFSBResponse<DATA_SIZE>& rsp) {
	typedef SnoopingFSBResponse<DATA_SIZE> RspType;
	
	os << "- read_status = ";
	switch(rsp.read_status) {
	case RspType::RS_MISS: os << "RS_MISS"; break;
	case RspType::RS_SHARED: os << "RS_SHARED"; break;
	case RspType::RS_MODIFIED: os << "RS_MODIFIED"; break;
	case RspType::RS_BUSY: os << "RS_BUSY"; break;
	}
	os << std::endl;
	os << "- read_data(hex) =" << std::hex;
	for(unsigned int i = 0; i < DATA_SIZE; i++) {
		os << " " << (unsigned int)rsp.read_data[i];
	}
	os << std::dec;
	return os;
}

} // end of namespace message
} // end of namespace tlm
} // end of namespace component
} // end of namespace unisim

namespace unisim {
namespace component {
namespace tlm {
namespace debug {

template<class ADDRESS, unsigned int DATA_SIZE>
class RequestSpy<unisim::component::tlm::message::SnoopingFSBRequest<ADDRESS, DATA_SIZE> > {
private:
	typedef unisim::component::tlm::message::SnoopingFSBRequest<ADDRESS, DATA_SIZE> ReqType;
	typedef unisim::util::garbage_collector::Pointer<ReqType> PReqType;

public:
	void Dump(unisim::kernel::logger::Logger &os, PReqType &req) {
		os << "- type = ";
		switch(req->type) {
		case ReqType::READ:
			os << "READ";
			break;
		case ReqType::READX:
			os << "READX";
			break;
		case ReqType::WRITE:
			os << "WRITE";
			break;
		case ReqType::INV_BLOCK:
			os << "INV_BLOCK";
			break;
		case ReqType::FLUSH_BLOCK:
			os << "FLUSH_BLOCK";
			break;
		case ReqType::ZERO_BLOCK:
			os << "ZERO_BLOCK";
			break;
		case ReqType::INV_TLB:
			os << "INV_TLB";
			break;
		}
		os << std::endl;
		os << "- global = " << (req->global?"TRUE":"FALSE") << std::endl;
		os << "- address = 0x" << std::hex << req->addr << std::dec << std::endl;
		os << "- size = " << req->size;
		if(req->type == ReqType::WRITE) {
			os << std::endl;
			os << "- write_data(hex) =" << std::hex;
			for(unsigned int i = 0; i < req->size; i++) {
				os << " " << (unsigned int)req->write_data[i];
			}
			os << std::dec;
		}
	}

};

template <class ADDRESS, unsigned int DATA_SIZE>
class ResponseSpy<unisim::component::tlm::message::SnoopingFSBResponse<DATA_SIZE>, 
	unisim::component::tlm::message::SnoopingFSBRequest<ADDRESS, DATA_SIZE> > {
private:
	typedef unisim::component::tlm::message::SnoopingFSBRequest<ADDRESS, DATA_SIZE> ReqType;
	typedef unisim::component::tlm::message::SnoopingFSBResponse<DATA_SIZE> RspType;
	typedef unisim::util::garbage_collector::Pointer<ReqType> PReqType;
	typedef unisim::util::garbage_collector::Pointer<RspType> PRspType;

public:
	void Dump(unisim::kernel::logger::Logger &os, PRspType &rsp, 
		PReqType &req) {
		os << "- read_status = ";
		switch(rsp->read_status) {
		case RspType::RS_MISS: os << "RS_MISS"; break;
		case RspType::RS_SHARED: os << "RS_SHARED"; break;
		case RspType::RS_MODIFIED: os << "RS_MODIFIED"; break;
		case RspType::RS_BUSY: os << "RS_BUSY"; break;
		}
		os << std::endl;
		os << "- read_data(hex) =" << std::hex;
		for(unsigned int i = 0; i < req->size; i++) {
			os << " " << (unsigned int)rsp->read_data[i];
		}
		os << std::dec;
	}
};

} // end of namespace debug
} // end of namespace tlm 
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM_MESSAGE_SNOOPINGFSB_HH__
