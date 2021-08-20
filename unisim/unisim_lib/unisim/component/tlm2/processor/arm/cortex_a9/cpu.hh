/*
 *  Copyright (c) 2010,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A9_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A9_CPU_HH__

#include <systemc>
#include <tlm>
#include <unisim/component/cxx/processor/arm/vmsav7/cpu.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace cortex_a9 {

class CPU
	: public sc_core::sc_module
	, public tlm::tlm_bw_transport_if<>
	, public unisim::component::cxx::processor::arm::vmsav7::CPU
{
public:
  typedef tlm::tlm_base_protocol_types::tlm_payload_type  transaction_type;
  typedef tlm::tlm_base_protocol_types::tlm_phase_type    phase_type;
  typedef tlm::tlm_sync_enum     sync_enum_type;
	
  typedef unisim::component::cxx::processor::arm::vmsav7::CPU PCPU;
  typedef PCPU::CP15CPU CP15CPU;
  typedef PCPU::CP15Reg CP15Reg;

  /**************************************************************************
   * Port to the bus and its virtual methods to handle                START *
   *   incomming calls.                                                     *
   **************************************************************************/

  // Master port to the bus port
  tlm::tlm_initiator_socket<32> master_socket;
	
  void WaitForInterrupt();
    
private:
  // virtual method implementation to handle backward path of
  //   transactions sent through the master_port
  virtual sync_enum_type nb_transport_bw(transaction_type &trans, phase_type &phase, sc_core::sc_time &time);
  // virtual method implementation to handle backward path of the dmi
  //   mechanism
  virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range);

  /**************************************************************************
   * Port to the bus and its virtual methods to handle                  END *
   *   incomming calls.                                                     *
   **************************************************************************/

 private:
  void SetExternalEvent();
  bool GetExternalEvent();
  
  bool        check_external_event;
  sc_core::sc_event external_event;
  
  /**************************************************************************
   * Interrupt ports and their handles                                START *
   **************************************************************************/

public:
  
  // Slave port for the nIRQm signal
  sc_core::sc_in<bool> nIRQm;
  // Slave port for the nFIQm signal
  sc_core::sc_in<bool> nFIQm;
  // Slave port for the nRESETm signal
  sc_core::sc_in<bool> nRESETm;
  
private:
  /** nIRQm port handler */
  void IRQHandler();
  /** nFIQm port handler */
  void FIQHandler();
  /** nRESETm port hander */
  void ResetHandler();
	
  /**************************************************************************
   * Interrupt ports and their handles                                  END *
   **************************************************************************/

public:
  SC_HAS_PROCESS(CPU);
  CPU(const sc_core::sc_module_name& name, Object *parent = 0);
  virtual ~CPU();

public:
  virtual void Stop(int ret);
  virtual void Sync();
  void Wait( sc_core::sc_event const& evt );
	
  virtual bool EndSetup();

  void BusSynchronize();
	
  void Run();

  void Reset();
  
  virtual void ResetMemory();
  
  virtual bool PhysicalWriteMemory(uint32_t addr, uint32_t paddr, const uint8_t *buffer, uint32_t size, uint32_t attrs);
  virtual bool PhysicalReadMemory(uint32_t addr, uint32_t paddr, uint8_t *buffer, uint32_t size, uint32_t attrs);
  virtual bool PhysicalFetchMemory(uint32_t addr, uint32_t paddr, uint8_t *buffer, uint32_t size, uint32_t attrs) { return PhysicalReadMemory(addr, paddr, buffer, size, attrs); }
	
  void SetCycleTime( sc_core::sc_time const& cycle_time );
  void SetBusCycleTime( sc_core::sc_time const& cycle_time );

private:
  virtual bool ExternalReadMemory(uint32_t addr, void* buffer, uint32_t size);
  virtual bool ExternalWriteMemory(uint32_t addr, void const* buffer, uint32_t size);
	
  /** Event used to signalize the end of a read transaction.
   * Method PhysicalReadMemory waits for this event once the read transaction has been 
   *   sent, and the nb_transport_bw notifies on it when the read transaction 
   *   is finished. 
   */
  sc_core::sc_event end_read_rsp_event;
  
  typedef unisim::kernel::tlm2::PayloadFabric<tlm::tlm_generic_payload> PayloadFabric;
  PayloadFabric payload_fabric;
  
  /** Transaction
   * A transaction_type smart pointer with release() on destruction
   */
  struct Transaction {
    Transaction( PayloadFabric& pf ) : t( pf.allocate() ) {}
    ~Transaction() { t->release(); }
    transaction_type* operator -> () { return t; }
    transaction_type& operator * () { return *t; }
    transaction_type* t;
  };
  
  /** A temporary variable used anywhere in the code to compute a time.
   * A temporary variable that can be used anywhere in the code to compute a 
   *   time.
   *   Should be initialized everytime it is used.
   */
  sc_core::sc_time tmp_time;
	
  sc_core::sc_time cpu_time;
  sc_core::sc_time bus_time;
  sc_core::sc_time quantum_time;
  sc_core::sc_time cpu_cycle_time;
  sc_core::sc_time bus_cycle_time;
  sc_core::sc_time nice_time;
  double ipc;
  bool enable_dmi;
  sc_core::sc_time time_per_instruction;
  
  
  unisim::kernel::variable::Statistic<sc_core::sc_time> stat_cpu_time;
  
  struct CpuCycleTimeParam : public unisim::kernel::variable::Parameter<sc_core::sc_time>
  {
    CpuCycleTimeParam(char const* name, CPU* _cpu)
      : unisim::kernel::variable::Parameter<sc_core::sc_time>(name, _cpu, _cpu->cpu_cycle_time, "Processor cycle time parameter"), cpu(*_cpu)
    {}
    void Set( sc_core::sc_time const& value ) { cpu.SetCycleTime( value ); }
    CPU& cpu;
  } param_cpu_cycle_time;
  unisim::kernel::variable::Parameter<sc_core::sc_time> param_bus_cycle_time;
  unisim::kernel::variable::Parameter<sc_core::sc_time> param_nice_time;
  unisim::kernel::variable::Parameter<double> param_ipc;
  unisim::kernel::variable::Parameter<bool> param_enable_dmi;
	
  /*************************************************************************
   * Logger, verbose and trap parameters/methods/ports               START *
   *************************************************************************/

  bool verbose_tlm;
  unisim::kernel::variable::Parameter<bool> param_verbose_tlm;
  inline bool VerboseTLM();

  /*************************************************************************
   * Logger, verbose and trap parameters/methods/ports                 END *
   *************************************************************************/
	
  unisim::kernel::tlm2::DMIRegionCache dmi_region_cache;

  /****************************/
  /* Configuration pins START */
  /****************************/
protected:
  bool VINITHI; unisim::kernel::variable::Parameter<bool> param_VINITHI;
    
  /****************************/
  /* Configuration pins  END  */
  /****************************/
    
  /**************************/
  /* CP15 Interface   START */
  /**************************/
  
protected:
  virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  virtual void     CP15ResetRegisters();
  
  /**************************/
  /* CP15 Interface    END  */
  /**************************/

};

} // end of namespace cortex_a9
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_ARM_CORTEX_A9_CPU_HH__
