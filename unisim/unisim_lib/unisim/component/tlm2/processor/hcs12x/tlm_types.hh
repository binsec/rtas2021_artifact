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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */


#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_TLM_TYPES_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_TLM_TYPES_HH__

#include <inttypes.h>
#include <string>

#include <tlm>

#include "unisim/kernel/tlm2/tlm.hh"

#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/tlm2/processor/hcs12x/can_struct.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

using namespace std;
using namespace tlm;

using unisim::kernel::tlm2::ManagedPayload;

using unisim::component::cxx::processor::hcs12x::address_t;

class INT_TRANS_T {
public:
	INT_TRANS_T() :
		id(0), vectorAddress(0), priority(0) { }

	INT_TRANS_T(unsigned int _id, address_t _vectorAddress, uint8_t _priority) :
		id(_id), vectorAddress(_vectorAddress), priority(_priority) { }

	~INT_TRANS_T() { }

	void Reset() { id = 0; vectorAddress = 0; priority = 0; }
	unsigned int getID() { return (id); }
	void setID(unsigned int _id) { id = _id; }
	address_t getVectorAddress() { return (vectorAddress); }
	void setVectorAddress(address_t _vectorAddress) { vectorAddress =_vectorAddress; }
	uint8_t getPriority() { return (priority); }
	void setPriority(uint8_t _priority) { priority = _priority; }

private:
	unsigned int id;
	address_t vectorAddress;
	uint8_t priority;
};

class XINT_Payload : public ManagedPayload
{
public:
	XINT_Payload(): interrupt_offset(0), xgate_shared_channel(false) { }

	void setInterruptOffset(unsigned int	offset) { interrupt_offset = offset; }
	void setXGATE_shared_channel(bool isXGate = false) { xgate_shared_channel = isXGate; }
	unsigned int getInterruptOffset() { return (interrupt_offset); }
	bool    isXGATE_shared_channel() { return (xgate_shared_channel); }
	void reset() { xgate_shared_channel = false; interrupt_offset = 0; }

private:
	unsigned int	interrupt_offset;
	bool    xgate_shared_channel;
};

class XINT_REQ_ProtocolTypes
{
public:
  typedef XINT_Payload tlm_payload_type;
  typedef tlm_phase tlm_phase_type;

};

template <uint8_t PWM_SIZE>
class PWM_Payload : public ManagedPayload
{
public:
	bool pwmChannel[PWM_SIZE];

	friend std::ostream& operator << (std::ostream& os, const PWM_Payload& payload) {

		for (int i=0; i<PWM_SIZE; i++) {
			os << payload.pwmChannel[i] << " ";
		}

		return (os);
	}

};

template <uint8_t PWM_SIZE>
class UNISIM_PWM_ProtocolTypes
{
public:
  typedef PWM_Payload<PWM_SIZE> tlm_payload_type;
  typedef tlm_phase tlm_phase_type;
};

template <uint8_t ATD_SIZE>
class ATD_Payload : public ManagedPayload
{
public:
	double anPort[ATD_SIZE];

	friend std::ostream& operator << (std::ostream& os, const ATD_Payload& payload) {

		os.precision(3);

		for (int i=0; i<ATD_SIZE; i++) {
			os << fixed << payload.anPort[i] << " ";
		}

		os << std::dec;

		return (os);
	}

};

template <uint8_t ATD_SIZE>
class UNISIM_ATD_ProtocolTypes
{
public:
  typedef ATD_Payload<ATD_SIZE> tlm_payload_type;
  typedef tlm_phase tlm_phase_type;
};

//// ******* CAN Data structures **************
//
//#define CAN_MSG_SIZE			16
//#define CAN_EXG_SIZE			13
//#define CAN_ID_INDEX			0
//#define CAN_ID_SIZE				4
//#define CAN_DATA_INDEX			4
//#define CAN_DATA_SIZE			8
//#define CAN_LENGTH_INDEX		12
//#define CAN_DATA_LENGTH_SIZE	1
//#define CAN_PRIORITY_INDEX		13
//#define CAN_PRIORITY_SIZE		1
//#define CAN_TIMESTAMP_INDEX		14
//#define CAN_TIMESTAMP_SIZE		2
//
//typedef enum {CAN_MSG_STD, CAN_MSG_EXT} CanMsgType;
//#define INVALID_CAN_ID 0xFFFFFFFF // identify an invalid message
//
//#define CAN_BUS_WIDTH 32
//
///*
//The CAN_DATATYPE structure
//*/
//struct CAN_MESSAGE {
//
//    /* Is Extended frame: extended ID = 29-bits and standard ID = 11-bits */
//    uint8_t Extended;
//
//    /* RTR: Remote Transmission Request (Request for transmission)
//     *   0: Data frame
//     *   1: Remote frame (forces 0 bytes of data)
//     *
//     *   This flag reflects the status of the Remote Transmission Request bit in the CAN frame.
//     *   In the case of a receive buffer, it indicates the status of the received frame and supports the transmission of an answering frame in software.
//     *   In the case of a transmit buffer, this flag defines the setting of the RTR bit to be sent.
//     */
//    uint8_t Remote;
//
//    /* Error ? */
//    uint8_t Error;
//
//    /* CAN ID */
//    uint8_t ID[CAN_ID_SIZE];
//
//    /* Data field */
//    uint8_t Data[CAN_DATA_SIZE];
//
//    /* Length */
//    uint8_t Length; // 4 bits of DLC (Data Length Code), which may only take on the values 0 - 8
//
//    /* Priority */
//    uint8_t Priority;
//
//    /* MSG Time Stamp */
//    uint8_t Timestamp[CAN_TIMESTAMP_SIZE];
//
//} ;
//
//typedef struct CAN_MESSAGE CAN_DATATYPE;

///**
// * Hold 1xN array of CAN_DATATYPE
// *
// * @param nmsgs: Indicates how many messages are in the array.
// * @param canMsg: The array of messages.
// */
//struct CAN_MESSAGE_ARRAY {
//    int nmsgs;
//    CAN_DATATYPE* canMsg;
//} ;
//
//typedef struct CAN_MESSAGE_ARRAY CAN_DATATYPE_ARRAY;
//
///**
// * Hold CAN ID information.
// *
// * @param nIDs: Indicates how many messages IDs are in the array.
// * @param IDs: The array of messages IDs.
// */
//struct CAN_ID_MSG_ARRAY {
//    size_t nIDs;
//    uint32_t* IDs;
//} ;
//
//typedef struct CAN_ID_MSG_ARRAY CAN_ID_ARRAY;

class CAN_Payload : public ManagedPayload
{
public:
	uint8_t msgVect[CAN_MSG_SIZE];
//	CAN_DATATYPE msg;
	static const uint8_t IDE_MASK = 0x08;
	static const uint8_t EXT_RTR_MASK = 0x01;
	static const uint8_t STD_RTR_MASK = 0x10;

	tlm_extension_base* set_auto_extension(tlm_extension_base* ext) { return 0; }

	void setMsgVect(uint8_t vect[CAN_MSG_SIZE]) {
		for (int i=0; i<CAN_MSG_SIZE; i++) {
			msgVect[i] = vect[i];
		}
	}

	void pack(CAN_DATATYPE msg) {

		for (int i=0; i<CAN_ID_SIZE; i++) {
			msgVect[CAN_ID_INDEX+i] = msg.ID[i];
		}

		for (int i=0; i < CAN_DATA_SIZE; i++) {
			msgVect[CAN_DATA_INDEX+i] = msg.Data[i];
		}

		msgVect[CAN_LENGTH_INDEX] = msg.Length;
		msgVect[CAN_PRIORITY_INDEX] = msg.Priority;

		for (int i=0; i < CAN_TIMESTAMP_SIZE; i++) {
			msgVect[CAN_TIMESTAMP_INDEX+i] = msg.Timestamp[i];
		}

	}

	void unpack(CAN_DATATYPE &msg) {

		for (int i=0; i<CAN_ID_SIZE; i++) {
			msg.ID[i] = msgVect[CAN_ID_INDEX+i];
		}

		for (int i=0; i < CAN_DATA_SIZE; i++) {
			msg.Data[i] = msgVect[CAN_DATA_INDEX+i];
		}

		msg.Length = msgVect[CAN_LENGTH_INDEX];
		msg.Priority = msgVect[CAN_PRIORITY_INDEX];

		for (int i=0; i < CAN_TIMESTAMP_SIZE; i++) {
			msg.Timestamp[i] = msgVect[CAN_TIMESTAMP_INDEX+i];
		}

		msg.Extended = (msg.ID[1] & IDE_MASK) >> 3;
		if (msg.Extended == 1) // extended ID = 29-bits and RTR is at bit-32
		{
			msg.Remote = msg.ID[3] & EXT_RTR_MASK;
		}
		else // standard ID = 11-bits and RTR is at bit-12
		{
			msg.Remote = msg.ID[1] & STD_RTR_MASK;
		}

	}

	friend std::ostream& operator << (std::ostream& os, const CAN_Payload& payload) {
		CAN_DATATYPE msg;
		((CAN_Payload) payload).unpack(msg);

		os << std::hex << (unsigned int) msg.Extended << "-" << std::hex << (unsigned int) msg.Length;

		os << "-" ;
		for (int i=0; i<CAN_ID_SIZE; i++) {
			os << std::hex << (unsigned int) msg.ID[i] << ".";
		}

		os << "-" ;
		for (int i=0; i < CAN_DATA_SIZE; i++) {
			os << std::hex << (unsigned int) msg.Data[i] << ".";
		}

		os << "-" << std::hex << (unsigned int) msg.Remote << "-" << (unsigned int) msg.Error;

		os << "-" << std::hex << (unsigned int) msg.Timestamp[0] << (unsigned int) msg.Timestamp[1];

		os << std::dec;

		return (os);
	}

};

class UNISIM_CAN_ProtocolTypes
{
public:
  typedef CAN_Payload tlm_payload_type;
  typedef tlm_phase tlm_phase_type;
};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif /*__UNISIM_COMPONENT_TLM2_PROCESSOR_TLM_TYPES_HH__*/
