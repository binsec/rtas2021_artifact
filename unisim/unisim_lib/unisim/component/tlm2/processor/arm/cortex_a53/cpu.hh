/*
 *  Copyright (c) 2016,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A53_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A53_CPU_HH__

#include <systemc>
#include <tlm>
#include <unisim/component/cxx/processor/arm/vmsav8/cpu.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace cortex_a53 {

struct ConfigCA53
{
  //=====================================================================
  //=                  ARM architecture model description               =
  //=====================================================================
  
  // Following a standard armv7 configuration
  // static uint32_t const model = unisim::component::cxx::processor::arm::ARMV7;
  static bool const     insns4T = true;
  static bool const     insns5E = true;
  static bool const     insns5J = true;
  static bool const     insns5T = true;
  static bool const     insns6  = true;
  static bool const     insnsRM = true;
  static bool const     insnsT2 = true;
  static bool const     insns7  = true;
  static bool const     hasVFP  = true;
  static bool const     hasAdvSIMD = false;
};


struct CPU
  : public sc_core::sc_module
  , public tlm::tlm_bw_transport_if<>
  , public unisim::component::cxx::processor::arm::vmsav8::CPU<ConfigCA53>
{
  typedef tlm::tlm_base_protocol_types::tlm_payload_type  transaction_type;
  typedef tlm::tlm_base_protocol_types::tlm_phase_type    phase_type;
  typedef tlm::tlm_sync_enum     sync_enum_type;
	
  typedef unisim::component::cxx::processor::arm::vmsav8::CPU<ConfigCA53> PCPU;
  // typedef PCPU::CP15CPU CP15CPU;
  // typedef PCPU::CP15Reg CP15Reg;

  /**************************************************************************
   * Port to the bus and its virtual methods to handle                START *
   *   incomming calls.                                                     *
   **************************************************************************/
  
  SC_HAS_PROCESS(CPU);
  CPU( sc_core::sc_module_name const& name, Object* parent=0 );
  virtual ~CPU();
  
  void Reset();
  
  virtual void ResetMemory();
  
  // Master port to the bus port
  tlm::tlm_initiator_socket<64> master_socket;
	
  void Run();

private:
  // virtual method implementation to handle backward path of
  //   transactions sent through the master_port
  virtual sync_enum_type nb_transport_bw(transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
  // virtual method implementation to handle backward path of the dmi
  //   mechanism
  virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

  /**************************
   * SystemC / TLM mechanic *
   **************************/

  /** Event used to signalize the end of a read transaction.
   * Method PhysicalReadMemory waits for this event once the read transaction has been 
   *   sent, and the nb_transport_bw notifies on it when the read transaction 
   *   is finished. 
   */
  sc_core::sc_event end_read_rsp_event;
  unisim::kernel::tlm2::DMIRegionCache dmi_region_cache;

  typedef unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> PayloadFabric;
  PayloadFabric payload_fabric;
  
  /** Transaction
   * A transaction_type smart pointer with release() on destruction
   */
  struct Transaction
  {
    Transaction( PayloadFabric& pf ) : t( pf.allocate() ) {}
    ~Transaction() { t->release(); }
    transaction_type* operator -> () { return t; }
    transaction_type& operator * () { return *t; }
    transaction_type* t;
  };
  
  sc_core::sc_time cpu_time;
  sc_core::sc_time bus_time;
  sc_core::sc_time quantum_time;
  sc_core::sc_time cpu_cycle_time;
  sc_core::sc_time bus_cycle_time;
  sc_core::sc_time nice_time;
  sc_core::sc_time time_per_instruction;
  double ipc;
  bool enable_dmi;
  
  bool verbose_tlm;
  //unisim::kernel::variable::Parameter<bool> param_verbose_tlm;
  
  virtual void Sync();
  void         Wait( sc_core::sc_event const& evt );
  void         BusSynchronize();

  // Intrusive memory accesses
  virtual bool  PhysicalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size );
  virtual bool PhysicalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size );
  // Non-itrusive memory accesses
  virtual bool  ExternalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size );
  virtual bool ExternalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size );
};

} // end of namespace cortex_a53
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A53_CPU_HH__
