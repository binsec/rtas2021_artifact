/*
 *  Copyright (c) 2007-2015,
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
 * Authors: Réda Nouacer (reda.nouacer@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/component/tlm2/processor/risc16/cpu.hh>
#include <unisim/util/endian/endian.hh>

namespace unisim{
namespace component{
namespace tlm2{
namespace processor{
namespace risc16{

CPU::CPU(sc_core::sc_module_name name, Object *_parent)
	: Object(name, _parent, "")
	, sc_core::sc_module(name)
	, unisim::component::cxx::processor::risc16::CPU(name, _parent)
	, master_sock("master-sock")
{
	// bind socket to interface implementation
	master_sock(*this);

	SC_HAS_PROCESS(CPU);
	SC_THREAD(Process);
}

CPU::~CPU()
{
}

void CPU::Process() {
	sc_core::sc_time cycle_time = sc_core::sc_time(1, sc_core::SC_NS);
	sc_core::sc_time quantum = sc_core::sc_time(100, sc_core::SC_NS);
	while(1)
	{
		step_instruction();
		local_time_offset += cycle_time;
		
		if(local_time_offset >= quantum)
		{
			wait(quantum);
			local_time_offset -= quantum;
		}
	}
}

uint16_t CPU::mem_read(uint16_t addr)
{
	uint16_t raw = tlm_read(addr);
	return unisim::util::endian::BigEndian2Host(raw);
}

void CPU::mem_write(uint16_t addr, uint16_t data)
{
	uint16_t raw = unisim::util::endian::Host2BigEndian(data);
	tlm_write(addr, (unsigned char*) &raw, 2);
}

uint16_t CPU::tlm_read(uint16_t addr)
{
	tlm::tlm_generic_payload trans;

	uint16_t data;

	trans.set_command(tlm::TLM_READ_COMMAND);
	trans.set_address(2 * addr);
	trans.set_data_ptr((unsigned char *) &data);
	trans.set_data_length(2);

	master_sock->b_transport(trans, local_time_offset);

	return data;
}

void CPU::tlm_write(uint16_t addr, unsigned char *data, uint16_t size)
{
	tlm::tlm_generic_payload trans;

	trans.set_command(tlm::TLM_WRITE_COMMAND);
	trans.set_address(2 * addr);
	trans.set_data_ptr(data);
	trans.set_data_length(size);

	master_sock->b_transport(trans, local_time_offset);
}

/**************backward transport interface implementation*************/

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	return tlm::TLM_COMPLETED;
}

void CPU::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

} // end of namespace risc16
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
