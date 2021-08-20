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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_HH__

#include <map>
#include <systemc>
#include <tlm>
#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/tlm2/interconnect/generic_bus/bus_types.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace generic_bus {

template<unsigned int BUSWIDTH = 32,
	typename TYPES = tlm::tlm_base_protocol_types,
	bool DEBUG = false>
class Bus : 
	public unisim::kernel::Object,
	public sc_module {
public:
	SC_HAS_PROCESS(Bus);
	Bus(const sc_module_name& name, unisim::kernel::Object *parent = 0);
	~Bus();

	virtual bool Setup();

	tlm_utils::multi_passthrough_target_socket<Bus, BUSWIDTH, TYPES> targ_socket;
	tlm_utils::simple_initiator_socket<Bus, BUSWIDTH, TYPES> init_socket;

private:
	typedef typename TYPES::tlm_payload_type transaction_type;
	typedef typename TYPES::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum               sync_enum_type;

	/*************************************************************************
	 * Multi passtrough target socket callbacks                        START *
	 *************************************************************************/

	sync_enum_type	T_nb_transport_fw_cb(int id, transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void			T_b_transport_cb(int id, transaction_type &trans, sc_core::sc_time &time);
	unsigned int	T_transport_dbg_cb(int id, transaction_type &trans);
	bool			T_get_direct_mem_ptr_cb(int id, transaction_type &trans, tlm::tlm_dmi &dmi);

	/*************************************************************************
	 * Multi passthrough target socket callbacks                         END *
	 *************************************************************************/

	/*************************************************************************
	 * Simple initiator socket callbacks                               START *
	 *************************************************************************/
	
	sync_enum_type	I_nb_transport_bw_cb(transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
	void			I_invalidate_direct_mem_ptr_cb(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

	/*************************************************************************
	 * Simple initiator socket callbacks                                 END *
	 *************************************************************************/

	/*************************************************************************
	 * Dispatcher methods and variables                                  END *
	 *************************************************************************/

	BusPeq<TYPES> peq;
	BusPeq<TYPES> free_peq;
	void Dispatcher();
	void DispatchFW(BusTlmGenericProtocol<TYPES> *item);
	void DispatchBW(BusTlmGenericProtocol<TYPES> *item);
	void BusSynchronize();
	std::map<transaction_type *, BusTlmGenericProtocol<TYPES> *> pending_transactions;
	void PrintPendingTransactions();
	void AddPendingTransaction(transaction_type &trans, BusTlmGenericProtocol<TYPES> *item);
	void RemovePendingTransaction(transaction_type &trans);
	sc_event end_req_event;
	sc_event end_resp_event;

	/*************************************************************************
	 * Dispatcher methods and variables                                  END *
	 *************************************************************************/

	/*************************************************************************
	 * Parameters                                                      START *
	 *************************************************************************/

	sc_time bus_cycle_time;
	double bus_cycle_time_double;
	unisim::kernel::variable::Parameter<double> param_bus_cycle_time_double;

	/*************************************************************************
	 * Parameters                                                        END *
	 *************************************************************************/

	/*************************************************************************
	 * Logger and verbose parameters                                   START *
	 *************************************************************************/

	unisim::kernel::logger::Logger logger;
	bool verbose_all;
	unisim::kernel::variable::Parameter<bool> param_verbose_all;
	bool verbose_setup;
	unisim::kernel::variable::Parameter<bool> param_verbose_setup;
	bool verbose_non_blocking;
	unisim::kernel::variable::Parameter<bool> param_verbose_non_blocking;
	bool verbose_blocking;
	unisim::kernel::variable::Parameter<bool> param_verbose_blocking;

	/*************************************************************************
	 * Logger and verbose parameters                                     END *
	 *************************************************************************/

	/*************************************************************************
	 * Verbose methods                                                 START *
	 *************************************************************************/

	inline bool VerboseSetup();
	inline bool VerboseNonBlocking();
	inline bool VerboseBlocking();

	/*************************************************************************
	 * Verbose methods                                                   END *
	 *************************************************************************/

};

} // end of namespace generic_bus
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_GENERIC_BUS_BUS_HH__
