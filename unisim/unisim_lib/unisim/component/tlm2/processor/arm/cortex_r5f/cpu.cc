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
 
#include <unisim/component/tlm2/processor/arm/cortex_r5f/cpu.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/likely/likely.hh>

#include <systemc>
#include <tlm>

#define LOCATION \
  " - location = " \
  << __FUNCTION__ \
  << ":unisim_lib/unisim/component/tlm2/processor/arm/cortex_r5f/cpu.cc:" \
  << __LINE__
#define TIME(X) \
  " - time = " \
  << sc_core::sc_time_stamp() + (X)                     \
  << " (current time = " << sc_core::sc_time_stamp() << ")"
#define PHASE(X)   " - phase = " \
  << ( (X) == tlm::BEGIN_REQ  ?   "BEGIN_REQ" : \
     ( (X) == tlm::END_REQ    ?   "END_REQ" : \
     ( (X) == tlm::BEGIN_RESP ?   "BEGIN_RESP" : \
     ( (X) == tlm::END_RESP   ?   "END_RESP" : \
                    "UNINITIALIZED_PHASE"))))
#define TRANS(L,X) \
{ \
  (L) << " - trans = " << &(X) << std::endl \
    << "   - " << ((X).is_read()?"read":"write") << std::endl \
    << "   - address = 0x" << std::hex << (X).get_address() << std::dec << std::endl \
    << "   - data_length = " << (X).get_data_length(); \
  if((X).is_write()) { \
    (L) << std::endl; \
    (L) << "   - data ="; \
    for(unsigned int _trans_i = 0; _trans_i < (X).get_data_length(); _trans_i++) { \
      (L) << " " << std::hex << (unsigned int)((X).get_data_ptr()[_trans_i]) << std::dec; \
    } \
  } \
}

#define ETRANS(L,X) \
{ \
  (L) << " - trans = " << &(X) << std::endl \
    << "   - " << ((X).is_read()?"read":"write") << std::endl \
    << "   - address = 0x" << std::hex << (X).get_address() << std::dec << std::endl \
    << "   - data_length = " << (X).get_data_length() << std::endl \
      << "   - response_status = "; \
  switch((X).get_response_status()) { \
  case tlm::TLM_OK_RESPONSE: \
    (L) << "TLM_OK_RESPONSE"; \
    break; \
  case tlm::TLM_INCOMPLETE_RESPONSE: \
    (L) << "TLM_INCOMPLETE_RESPONSE"; \
    break; \
  case tlm::TLM_GENERIC_ERROR_RESPONSE: \
    (L) << "TLM_GENERIC_ERROR_RESPONSE"; \
    break; \
  case tlm::TLM_ADDRESS_ERROR_RESPONSE: \
    (L) << "TLM_ADDRESS_ERROR_RESPONSE"; \
    break; \
  case tlm::TLM_COMMAND_ERROR_RESPONSE: \
    (L) << "TLM_COMMAND_ERROR_RESPONSE"; \
    break; \
  case tlm::TLM_BURST_ERROR_RESPONSE: \
    (L) << "TLM_BURST_ERROR_RESPONSE"; \
    break; \
  case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE: \
    (L) << "TLM_BYTE_ENABLE_ERROR_RESPONSE"; \
    break; \
  } \
  if((X).get_response_status() == tlm::TLM_OK_RESPONSE) { \
    (L) << std::endl; \
    (L) << "   - data ="; \
    for(unsigned int _trans_i = 0; \
        _trans_i < (X).get_data_length(); \
        _trans_i++) { \
      (L) << " " << std::hex << (unsigned int)((X).get_data_ptr()[_trans_i]) \
        << std::dec; \
    } \
  } \
}

#define ITRANS(X)  " - trans.level = " << (X).level

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace cortex_r5f {

using namespace unisim::kernel::logger;

  CPU::CPU( sc_core::sc_module_name const& name, Object* parent )
  : unisim::kernel::Object(name, parent)
  , sc_core::sc_module(name)
  , unisim::component::cxx::processor::arm::pmsav7::CPU(name, parent)
  , master_socket("master_socket")
  , check_external_event(false)
  , external_event("extevt")
  , nRESETm("nRESETm")
  , nIRQm("nIRQm")
  , nFIQm("nFIQm")
  , IRQADDRVm("IRQADDRVm")
  , IRQADDRm("IRQADDRm")
  , IRQACKm("IRQACKm")
  , end_read_rsp_event()
  , payload_fabric()
  , tmp_time()
  , cpu_time(sc_core::SC_ZERO_TIME)
  , bus_time(sc_core::SC_ZERO_TIME)
  , quantum_time(sc_core::SC_ZERO_TIME)
  , cpu_cycle_time(62500.0, sc_core::SC_PS)
  , bus_cycle_time(62500.0, sc_core::SC_PS)
  , nice_time(1.0, sc_core::SC_MS)
  , ipc(1.0)
  , time_per_instruction(cpu_cycle_time/ipc)
  , enable_dmi(false)
  , stat_cpu_time("cpu-time", this, cpu_time, "The processor time")
  , param_cpu_cycle_time("cpu-cycle-time", this, *this, "The processor cycle time.")
  , param_bus_cycle_time("bus-cycle-time", this, bus_cycle_time, "The processor bus cycle time.")
  , param_nice_time("nice-time", this, nice_time,  "Maximum time between SystemC waits.")
  , param_ipc("ipc", this, ipc, "Instructions per cycle performance.")
  , param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
  , verbose_tlm(false)
  , param_verbose_tlm("verbose-tlm", this, verbose_tlm, "Display TLM information")
  , dmi_region_cache()
  , VINITHI(false)
  , param_VINITHI("VINITHI", this, VINITHI, "Reset V-bit value. When HIGH indicates HIVECS mode at reset" )
  , CFGEE(false)
  , param_CFGEE("CFGEE", this, CFGEE, "Reset EE-bit value. When HIGH indicates BE-8 mode for exceptions at reset")
  , TEINIT(false)
  , param_TEINIT("TEINIT", this, TEINIT, "Reset TE-bit value. Exception instruction set at reset (HIGH=Thumb,LO=ARM)")
  , ACTLR()
  , SACTLR()
  , ATCMRR()
  , BTCMRR()
  , PMCR()
  , PMCNTEN()
  , PMSELR()
  , PMCCNTR()
  , PMUSERENR(0)
  , PMINTEN(0)
  , NVALIRQEN(0)
  , NVALFIQEN(0)
  , NVALRESETEN(0)
  , VALEDBGRQEN(0)
  , SPCTLR(0)
  , XPIR(0)
  , VXPIR(0)
  , HPIR(0)
  , ADFSR(0)
  , AIFSR(0)
  , CFLR(0)
{
  master_socket.bind(*this);
  
  SC_THREAD(Run);
  SC_METHOD(IRQHandler);
  sensitive << nIRQm;
  SC_METHOD(FIQHandler);
  sensitive << nFIQm;
  SC_METHOD(ResetHandler);
  sensitive << nRESETm;
}

CPU::~CPU()
{
}

void
CPU::SetCycleTime(sc_core::sc_time const& cycle_time)
{
  /* Setting main cpu clock */
  cpu_cycle_time = cycle_time;
  /* compute the average time of each instruction */
  time_per_instruction = cycle_time / ipc;
}

/** Initialization of the module
 * Initializes the module depending on the parameters values.
 *
 * @return  true if the initialization suceeds, false otherwise
 */
bool 
CPU::EndSetup()
{
  if (not PCPU::EndSetup())
  {
    PCPU::logger << DebugError
      << "Error while trying to set up the ARM cpu" << std::endl
      << LOCATION
      << EndDebugError;
    return false;
  }

  cpu_time = sc_core::SC_ZERO_TIME;
  bus_time = sc_core::SC_ZERO_TIME;

  if ( verbose_tlm ) 
  {
    PCPU::logger << DebugInfo
      << "Setting CPU cycle time to " 
      << cpu_cycle_time.to_string() << std::endl
      << "Setting Bus cycle time to " 
      << bus_cycle_time.to_string() << std::endl
      << "Setting nice time to " << nice_time.to_string() << std::endl
      << "Setting IPC to " << ipc << std::endl
      << EndDebugInfo;
  }
  
  /* compute the average time of each instruction */
  time_per_instruction = cpu_cycle_time * ipc;

  return true;
}

void 
CPU::Stop(int ret)
{
  // Call BusSynchronize to account for the remaining time spent in the cpu 
  // core
  BusSynchronize();
  unisim::kernel::Object::Stop( ret );
}

/** Wait for a specific event and update CPU times
 */

void
CPU::Wait( sc_core::sc_event const& evt )
{
  if (quantum_time != sc_core::SC_ZERO_TIME)
    Sync();
  wait( evt );
  cpu_time = sc_core::sc_time_stamp();
}

/** Synchronization demanded from the CPU implementation.
 * An example (an for the moment the only synchronization demanded by the CPU
 * implmentation) is the a synchronization demanded by the debugger.
 */
void
CPU::Sync()
{
  if ( unlikely(verbose_tlm) )
  {
    PCPU::logger << DebugInfo
      << "Sync" << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;
  }
  wait(quantum_time);
  cpu_time = sc_core::sc_time_stamp();
  quantum_time = sc_core::SC_ZERO_TIME;
  
  if (unlikely(verbose_tlm))
    PCPU::logger << DebugInfo
                      << "Resuming after wait" << std::endl
                      << " - cpu_time     = " << cpu_time << std::endl
                      << " - nice_time     = " << nice_time << std::endl
                      << EndDebugInfo;
}

/** Updates the cpu time to the next bus cycle.
 * Updates the cpu time to the next bus cycle. Additionally it updates the
 * quantum time and if necessary synchronizes with the global SystemC clock.
 */
void 
CPU::BusSynchronize() 
{
  if ( unlikely(verbose_tlm) )
  {
    PCPU::logger << DebugInfo
      << "Bus Synchronize:" << std::endl
      << " - bus_time     = " << bus_time << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;
  }
  // Note: this needs to be better tested, but in order to avoid 
  //   multiplications and divisions with sc_time we do a loop expecting
  //   it will not loop too much. An idea of the operation to perform to
  //   avoid the loop:
  // quantum_time += 
  //   ((((cpu_time + quantum_time) / bus_cycle_time) + 1) * bus_cycle_time) -
  //   (cpu_time + quantum_time);
  sc_core::sc_time deadline(cpu_time);
  deadline += quantum_time;
  while ( bus_time < deadline )
    bus_time += bus_cycle_time;
  quantum_time = bus_time;
  quantum_time -= cpu_time;
  if (quantum_time > nice_time)
    Sync();
  if (unlikely(verbose_tlm))
  {
    PCPU::logger << DebugInfo
      << "Bus is now Synchronized:" << std::endl
      << " - bus_time     = " << bus_time << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;
  }
  
  return;
}
  
/** Main thread of the CPU simulator.
 * It is a loop that simply executes one instruction and increases the cpu time.
 * Also the quantum time is updated, and if it is bigger than the nice time, the
 * global SystemC time is updated.
 */
void
CPU::Run()
{
  if ( unlikely(verbose) )
    {
      PCPU::logger << DebugInfo
                        << "Starting CPU::Run loop" << std::endl
                        << " - cpu_time     = " << cpu_time << std::endl
                        << " - nice_time = " << nice_time << std::endl
                        << " - time_per_instruction = " << time_per_instruction
                        << EndDebugInfo;
    }
  
  IRQACKm = false;
  
  for (;;)
  {
    if (GetExternalEvent()) {
      if (not nRESETm) {
        Wait( nRESETm.posedge_event() );
        this->TakeReset();
        IRQACKm = false;
      }
      if (not nIRQm or not nFIQm) {
        uint32_t exceptions = 0;
        unisim::component::cxx::processor::arm::I.Set( exceptions, not nIRQm );
        unisim::component::cxx::processor::arm::F.Set( exceptions, not nFIQm );
        
        bool exception_taken = this->HandleAsynchronousException( exceptions ) != 0;
        if (exception_taken) {
          if (trap_reporting_import)
            trap_reporting_import->ReportTrap(*this,"irq or fiq");
        }
      }
    }
    
    if (unlikely(verbose_tlm))
    {
      PCPU::logger << DebugInfo
        << "Starting instruction:" << std::endl
        << " - cpu_time     = " << cpu_time << std::endl
        << " - quantum_time = " << quantum_time
        << EndDebugInfo;
    }
    
    uint32_t cpsr_cleared_bits = GetCPSR();
    StepInstruction();
    // cpu_time += time_per_instruction;
    quantum_time += time_per_instruction;
    cpsr_cleared_bits &= ~GetCPSR();
    
    /* In cortex R5F many FPSCR bits stick to 0 */
    FPSCR &= 0xffc0009f;
    
    if ( unlikely(verbose_tlm) )
    {
      PCPU::logger << DebugInfo
        << "Instruction finished:" << std::endl
        << " - cpu_time     = " << cpu_time << std::endl
        << " - quantum_time = " << quantum_time
        << EndDebugInfo;
    }
    
    // Check if we need to sync
    if (unisim::component::cxx::processor::arm::I.Get( cpsr_cleared_bits ) or
        unisim::component::cxx::processor::arm::F.Get( cpsr_cleared_bits ))
      {
        SetExternalEvent();
        if (unlikely(verbose_tlm))
          PCPU::logger << DebugInfo
                            << "Syncing due to exception being unmasked" << std::endl
                            << " - PC = 0x" << std::hex << current_insn_addr << std::dec << std::endl
                            << " - cpsr_cleared_bits = 0x" << std::hex << cpsr_cleared_bits << std::dec << std::endl
                            << " - cpu_time     = " << cpu_time << std::endl
                            << " - quantum_time = " << quantum_time
                            << EndDebugInfo;
        Sync();
      }
    else if (quantum_time > nice_time)
      Sync();
  }
}

void 
CPU::Reset()
{
}
  
void 
CPU::ResetMemory()
{
}

/**
 * Virtual method implementation to handle backward path of transactions sent 
 * through the master_port
 *
 * @param trans the transcation to handle
 * @param phase the state of the transaction to handle (should only be END_REQ
 *        or BEGIN_RESP)
 * @param time  the relative time at which the call is being done
 *
 * @return      the synchronization status
 */
tlm::tlm_sync_enum 
CPU::nb_transport_bw (transaction_type& trans, phase_type& phase, sc_core::sc_time& time)
{
  sync_enum_type ret = tlm::TLM_ACCEPTED;

  if (trans.get_command() == tlm::TLM_IGNORE_COMMAND) 
  {
    PCPU::logger << DebugWarning << "Received nb_transport_bw on master socket" 
      << ", with an ignore, which the cpu doesn't know how to handle" 
      << std::endl
      << TIME(time) << std::endl
      << PHASE(phase) << std::endl;
    TRANS(PCPU::logger, trans);
    PCPU::logger << EndDebug;
    return ret;
  }

  switch(phase)
  {
    case tlm::BEGIN_REQ:
    case tlm::END_RESP:
      /* The cpu should not receive the BEGIN_REQ (as it is the cpu which 
       * generates cpu requests), neither END_RESP (as it is the cpu which
       * ends responses) */
      PCPU::logger << DebugError << "Received nb_transport_bw on master_socket" 
        << ", with unexpected phase" << std::endl
        << LOCATION << std::endl
        << TIME(time) << std::endl
        << PHASE(phase) << std::endl;
      TRANS(PCPU::logger, trans);
      PCPU::logger << EndDebug;
      Stop(-1);
      break;
    case tlm::END_REQ:
      /* The request phase is finished.
       * If the request was a write, then the request can be released, because we do not 
       *   expect any answer. And we can send a TLM_COMPLETED for the same reason.
       * If the request was a read, the request can not be released and TLM_ACCEPTED has to
       *   be sent, and we should wait for the BEGIN_RESP phase. */
      if (trans.is_write()) 
      {
        trans.release();
        ret = tlm::TLM_COMPLETED;
      }
      else
        ret = tlm::TLM_ACCEPTED;
      return ret;
      break;
    case tlm::BEGIN_RESP:
      /* Starting the response phase.
       * If the request is a write report an error and stop, we should not have received it.
       * The target has initiated the response to a read request, compute when the request can
       *   be completely accepted and send a TLM_COMPLETED back. Send an event to the PhysicalReadMemory
       *   method to unlock the thread that originated the read transaction (using the end_read_event).
       */
      trans.acquire();
      if (trans.is_write())
      {
        PCPU::logger << DebugError << "Received nb_transport_bw on BEGIN_RESP phase, with unexpected write transaction" << std::endl
          << LOCATION << std::endl
          << TIME(time) << std::endl 
          << PHASE(phase) << std::endl;
        TRANS(PCPU::logger, trans);
        PCPU::logger << EndDebug;
        Stop(-1);
        break;
      }
      tmp_time = sc_core::sc_time_stamp();
	  tmp_time += time;
      /* TODO: increase tmp_time depending on the size of the transaction. */
      end_read_rsp_event.notify(time);
      ret = tlm::TLM_COMPLETED;
      trans.release();
      return ret;
      break;
  }

  /* this code should never be executed, if you are here something is wrong 
   *   above :( */
  PCPU::logger << DebugError 
    << "Reached end of nb_transport_bw, THIS SHOULD NEVER HAPPEN" << std::endl
    << LOCATION << std::endl
    << TIME(time) << std::endl
    << PHASE(phase) << std::endl;
  TRANS(PCPU::logger, trans);
  PCPU::logger << EndDebug;
  Stop(-1);
  // useless return to avoid compiler warnings/errors
  return ret;
}

/**
 * Virtual method implementation to handle backward path of dmi requests sent through the
 * master port.
 * We do not use the dmi option in our simulator, so this method is unnecessary. However,
 * we have to declare it in order to be able to compile the simulator.
 *
 * @param start_range the start address of the memory range to remove
 * @param end_range   the end address of the memory range to remove
 */
void 
CPU::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
{
  dmi_region_cache.Invalidate(start_range, end_range);
}

/** nIRQm port handler */
void
CPU::IRQHandler()
{
  SetExternalEvent();
  if (verbose_tlm)
    PCPU::logger << DebugInfo
                      << "IRQ level change:" << std::endl
                      << " - nIRQm = " << nIRQm << std::endl
                      << " - sc_time_stamp() = " << sc_core::sc_time_stamp() << std::endl
                      << EndDebugInfo;
}

/** nFIQm port handler */
void 
CPU::FIQHandler()
{
  SetExternalEvent();
  if (verbose_tlm)
    PCPU::logger << DebugInfo
                      << "FIQ level change:" << std::endl
                      << " - nFIQm = " << nFIQm << std::endl
                      << " - sc_time_stamp() = " << sc_core::sc_time_stamp() << std::endl
                      << EndDebugInfo;
}
  
/** nRESETm port handler */
void 
CPU::ResetHandler()
{
  SetExternalEvent();
  if (verbose_tlm)
    PCPU::logger << DebugInfo
                      << "RESET level change:" << std::endl
                      << " - nRESETm = " << nRESETm << std::endl
                      << " - sc_time_stamp() = " << sc_core::sc_time_stamp() << std::endl
                      << EndDebugInfo;
}

void
CPU::BranchToFIQorIRQvector( bool isIRQ )
{
  if (not isIRQ) {
    // Behavior is default for FIQs
    PCPU::BranchToFIQorIRQvector( isIRQ );
    return;
  }
  if (verbose)
    logger << DebugInfo
           << "Vector IRQ Exception, starting VIC handshake" << std::endl
           << EndDebugInfo;
  
  // Handshake with the VIC to retrieve IRQ vector address
  IRQACKm = true;
  
  if (not IRQADDRVm)
    Wait( IRQADDRVm.posedge_event() );
  
  uint32_t irq_addr = IRQADDRm.read();
  
  if (verbose)
    logger << DebugInfo
           << "Received IRQ vector address: " << std::hex << irq_addr << std::dec << std::endl
           << EndDebugInfo;
  
  IRQACKm = false;
  
  Branch( irq_addr, B_EXC );
}

/**
 * Virtual method implementation to handle non intrusive reads performed by the inherited
 * cpu to perform external memory accesses.
 * It uses the TLM2 debugging interface to request the data.
 *
 * @param addr    the read base address
 * @param buffer   the buffer to copy the data to the read
 * @param size    the size of the read
 */

bool 
CPU::ExternalReadMemory(uint32_t addr, void *buffer, uint32_t size)
{
  if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
  {
    // operator -> of ports is not legal before end of elaboration because
    // an implementation of SystemC can defer complete binding just before end of elaboration
    // Using memory service interface instead
    return PCPU::memory_import->ReadMemory(addr, buffer, size);
  }

  Transaction trans( payload_fabric );

  trans->set_address(addr);
  trans->set_data_length(size);
  trans->set_data_ptr((uint8_t *)buffer);
  trans->set_read();
  trans->set_streaming_width(size);

  unsigned int read_size = master_socket->transport_dbg(*trans);

  return (trans->is_response_ok() and read_size == size);
}

/**
 * Virtual method implementation to handle non intrusive writes performed by the inherited
 * cpu to perform external memory accesses.
 * It uses the TLM2 debugging interface to request the data.
 *
 * @param addr    the write base address
 * @param buffer  the buffer to write into the external memory
 * @param size    the size of the write
 */
bool 
CPU::ExternalWriteMemory(uint32_t addr, const void *buffer, uint32_t size)
{
  if(sc_core::sc_get_status() < sc_core::SC_END_OF_ELABORATION)
  {
    // operator -> of ports is not legal before end of elaboration because
    // an implementation of SystemC can defer complete binding just before end of elaboration
    // Using memory service interface instead
    return PCPU::memory_import->WriteMemory(addr, buffer, size);
  }

  Transaction trans( payload_fabric );

  trans->set_address(addr);
  trans->set_data_length(size);
  trans->set_data_ptr((uint8_t *)buffer);
  trans->set_write();
  trans->set_streaming_width(size);

  unsigned int write_size = master_socket->transport_dbg(*trans);

  return (trans->is_response_ok() and (write_size == size));
}

/**
 * Virtual method implementation to handle memory read operations performed by 
 * the ARM processor implementation.
 * If working with a blocking (BLOCKING = TRUE) version of the ARM processor 
 * this method synchronizes the  processor with the bus (increase local time) 
 * and sends it. If a synchronization is necessary a SystemC synchronization is 
 * performed.
 * TODO: if working with a non-block
 * 
 * @param addr    the read base address
 * @param buffer  the buffer to copy the data to read
 * @param size    the size of the read
 */
bool
CPU::PhysicalReadMemory(uint32_t addr, uint8_t *buffer, uint32_t size, uint32_t attrs)
{
  if (unlikely(verbose_tlm))
    PCPU::logger << DebugInfo
      << "Starting PhysicalReadMemory:" << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;

  /* Use blocking transactions.
   * Steps:
   * 1 - check when the request can be send (synchronize with the bus)
   * 2 - create the (auto-released) transaction
   * 3 - send the transaction
   * 4 - update DMI region cache
   */
  
  // 1 - synchronize with the bus
  BusSynchronize();

  // pre2 - DMI access (if possible)
  unisim::kernel::tlm2::DMIRegion* dmi_region = 0;
  
  if(likely(enable_dmi))
  {
    dmi_region = dmi_region_cache.Lookup(addr, size);
    
    if(likely(dmi_region))
    {
      if(likely(dmi_region->IsAllowed()))
      {
        tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
        if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_READ) == tlm::tlm_dmi::DMI_ACCESS_READ))
        {
          memcpy(buffer, dmi_data->get_dmi_ptr() + (addr - dmi_data->get_start_address()), size);
          quantum_time += dmi_region->GetReadLatency(size);
          if (quantum_time > nice_time)
            Sync();
          return true;
        }
      }
    }
  }

  // 2 - create the (auto-released) transaction
  Transaction trans( payload_fabric );
  
  //uint32_t byte_enable = 0xffffffffUL;
  trans->set_address(addr);
  trans->set_read();
  trans->set_data_ptr(buffer);
  trans->set_data_length(size);
  trans->set_streaming_width(size);
  // trans->set_byte_enable_ptr((unsigned char *) &byte_enable);
  // trans->set_byte_enable_length(size);

  // 3 - send the transaction and check response status
  master_socket->b_transport(*trans, quantum_time);
  if (not trans->is_response_ok())
    return false;
  
  // cpu_time = sc_time_stamp() + quantum_time;
  if (quantum_time > nice_time)
    Sync();

  // 4 - update DMI region cache
  if(likely(enable_dmi))
  {
    if(likely(not dmi_region and trans->is_dmi_allowed()))
    {
      tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
      trans->set_address(addr);
      trans->set_data_length(size);
      unisim::kernel::tlm2::DMIGrant dmi_grant = master_socket->get_direct_mem_ptr(*trans, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
      
      dmi_region_cache.Insert(dmi_grant, dmi_data);
    }
  }

  if ( unlikely(verbose_tlm) )
    PCPU::logger << DebugInfo
      << "Finished PhysicalReadMemory:" << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;

  return true;
}

bool
CPU::PhysicalWriteMemory(uint32_t addr, const uint8_t *buffer, uint32_t size, uint32_t attrs)
{
  if ( unlikely(verbose_tlm) )
    PCPU::logger << DebugInfo
      << "Starting PhysicalWriteMemory:" << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;

  /* Use blocking transactions.
   * Steps:
   * 1 - check when the request can be send (synchronize with the bus)
   * 2 - create the (auto-released) transaction
   * 3 - send the transaction
   * 4 - update DMI region cache
   */
  // 1 - synchronize with the bus
  BusSynchronize();

  // pre2 - DMI access (if possible)
  unisim::kernel::tlm2::DMIRegion* dmi_region = 0;
  
  if(likely(enable_dmi))
  {
    dmi_region = dmi_region_cache.Lookup(addr, size);
    
    if(likely(dmi_region))
    {
      if(likely(dmi_region->IsAllowed()))
      {
        tlm::tlm_dmi *dmi_data = dmi_region->GetDMI();
        if(likely((dmi_data->get_granted_access() & tlm::tlm_dmi::DMI_ACCESS_WRITE) == tlm::tlm_dmi::DMI_ACCESS_WRITE))
        {
          memcpy(dmi_data->get_dmi_ptr() + (addr - dmi_data->get_start_address()), buffer, size);
          quantum_time += dmi_region->GetWriteLatency(size);
          if (quantum_time > nice_time)
            Sync();
          return true;
        }
      }
    }
  }
  
  // 2 - create the (auto-released) transaction
  Transaction trans( payload_fabric );
  //uint32_t byte_enable = 0xffffffffUL;
  trans->set_address(addr);
  trans->set_write();
  trans->set_data_ptr((unsigned char *)buffer);
  trans->set_data_length(size);
  trans->set_streaming_width(size);
  // trans->set_byte_enable_ptr((unsigned char *) &byte_enable);
  // trans->set_byte_enable_length(size);

  // 3 - send the transaction and check response status
  master_socket->b_transport(*trans, quantum_time);
  if (not trans->is_response_ok())
    return false;
  
  if (quantum_time > nice_time)
    Sync();

  // 4 - update DMI region cache
  if (likely(enable_dmi))
  {
    if (likely(not dmi_region and trans->is_dmi_allowed()))
    {
      tlm::tlm_dmi *dmi_data = new tlm::tlm_dmi();
      trans->set_address(addr);
      trans->set_data_length(size);
      unisim::kernel::tlm2::DMIGrant dmi_grant = master_socket->get_direct_mem_ptr(*trans, *dmi_data) ? unisim::kernel::tlm2::DMI_ALLOW : unisim::kernel::tlm2::DMI_DENY;
      
      dmi_region_cache.Insert(dmi_grant, dmi_data);
    }
  }

  if ( unlikely(verbose_tlm) )
    PCPU::logger << DebugInfo
      << "Finished PhysicalWriteMemory:" << std::endl
      << " - cpu_time     = " << cpu_time << std::endl
      << " - quantum_time = " << quantum_time
      << EndDebugInfo;

  return true;
}

/** Resets the internal values of corresponding CP15 Registers
 */
void
CPU::CP15ResetRegisters()
{
  this->PCPU::CP15ResetRegisters();
  
  // Implementation defined values for SCTLR
  cxx::processor::arm::sctlr::V.Set( SCTLR, VINITHI );
  cxx::processor::arm::sctlr::EE.Set( SCTLR, CFGEE );
  cxx::processor::arm::sctlr::TE.Set( SCTLR, TEINIT );
}
    
/** Get the Internal representation of the CP15 Register
 * 
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @return        an internal CP15Reg
 */
CPU::CP15Reg&
CPU::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
    case CP15ENCODE( 0, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "MIDR, Main ID Register"; }
          uint32_t Read( CP15CPU& cpu ) { return 0x411fc151; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 1, 0, 0, 1 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "ACTLR, Auxiliary Control Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).ACTLR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 5, 0, 1, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "ADFSR, Auxiliary Data Fault Status Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).ADFSR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 5, 0, 1, 1 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "AIFSR, Auxiliary Instruction Fault Status Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).AIFSR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 9, 0, 1, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "BTCMRR, BTCM Region Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).BTCMRR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 1, 1 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "ATCMRR, ATCM Region Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).ATCMRR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 12, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMCR, Performance Monitor Control Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).PMCR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 12, 1 ):
    case CP15ENCODE( 9, 0, 12, 2 ):
      {
        struct PMCNTENR : CP15Reg
        {
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).PMCNTEN; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        };
        
        static struct : PMCNTENR
        {
          char const* Describe() { return "PMCTENSET, Count Enable Set Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) |= value; }
        } xset;
        
        static struct : PMCNTENR
        { 
          char const* Describe() { return "PMCTENCLR, Count Enable Clear Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) &= ~value; }
        } xclr;
        
        if (opcode2 == 1) return xset; return xclr;
      } break;
    
    case CP15ENCODE( 9, 0, 12, 3 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMOVSR, Overflow Flag Status Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) {}
          uint32_t Read( CP15CPU& _cpu ) { return 0; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 12, 4 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMSWINC, Software Increment Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) {}
          uint32_t Read( CP15CPU& _cpu ) { return 0; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 12, 5 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMSELR, Performance Counter Selection Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).PMSELR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 13, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMCCNTR, Cycle Count Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).PMCCNTR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 13, 1 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMXEVTYPER, Event Type Selection Register"; }
          uint32_t& reg( CP15CPU& _cpu )
          { CPU& cpu( dynamic_cast<CPU&>( _cpu ) ); return cpu.PMXEVTYPER[cpu.PMSELR]; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 13, 2 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMXEVCNTR, Event Count Register"; }
          uint32_t& reg( CP15CPU& _cpu )
          { CPU& cpu( dynamic_cast<CPU&>( _cpu ) ); return cpu.PMXEVCNTR[cpu.PMSELR]; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 9, 0, 14, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "PMUSERENR, User Enable Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).PMUSERENR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 9, 0, 14, 1 ):
    case CP15ENCODE( 9, 0, 14, 2 ):
      {
        struct PMINTENR : CP15Reg
        {
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).PMINTEN; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        };
        
        static struct : PMINTENR
        {
          char const* Describe() { return "PMINTENSET, Count Enable Set Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) |= value; }
        } xset;
        
        static struct : PMINTENR
        { 
          char const* Describe() { return "PMINTENCLR, Count Enable Clear Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) &= ~value; }
        } xclr;
        
        if (opcode2 == 1) return xset; return xclr;
      } break;
    
    case CP15ENCODE( 11, 0, 0, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "Slave Port Control Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).SPCTLR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 15, 0, 0, 0 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "SACTLR, Secondary Auxiliary Control Register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).SACTLR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 15, 0, 0, 1 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "LLPP Normal AXI region register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).XPIR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 15, 0, 0, 2 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "LLPP Virtual AXI region register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).VXPIR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 15, 0, 0, 3 ):
      {
        static struct : CP15Reg
        {
          char const* Describe() { return "AHB peripheral interface region register"; }
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).HPIR; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 15, 0, 1, 0 ):
    case CP15ENCODE( 15, 0, 1, 4 ):
      {
        struct NVALIRQENR : CP15Reg
        {
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).NVALIRQEN; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        };
        
        static struct : NVALIRQENR
        {
          char const* Describe() { return "nVAL IRQ Enable Set Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) |= value; }
        } xset;
        
        static struct : NVALIRQENR
        { 
          char const* Describe() { return "nVAL IRQ Enable Clear Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) &= ~value; }
        } xclr;
        
        if (opcode2 == 0) return xset; return xclr;
      } break;
    
    case CP15ENCODE( 15, 0, 1, 1 ):
    case CP15ENCODE( 15, 0, 1, 5 ):
      {
        struct NVALFIQENR : CP15Reg
        {
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).NVALFIQEN; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        };
        
        static struct : NVALFIQENR
        {
          char const* Describe() { return "nVAL FIQ Enable Set Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) |= value; }
        } xset;
        
        static struct : NVALFIQENR
        { 
          char const* Describe() { return "nVAL FIQ Enable Clear Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) &= ~value; }
        } xclr;
        
        if (opcode2 == 1) return xset; return xclr;
      } break;
    
    case CP15ENCODE( 15, 0, 1, 2 ):
    case CP15ENCODE( 15, 0, 1, 6 ):
      {
        struct NVALRESETENR : CP15Reg
        {
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).NVALRESETEN; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        };
        
        static struct : NVALRESETENR
        {
          char const* Describe() { return "nVAL Reset Enable Set Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) |= value; }
        } xset;
        
        static struct : NVALRESETENR
        { 
          char const* Describe() { return "nVAL Reset Enable Clear Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) &= ~value; }
        } xclr;
        
        if (opcode2 == 2) return xset; return xclr;
      } break;
    
    case CP15ENCODE( 15, 0, 1, 3 ):
    case CP15ENCODE( 15, 0, 1, 7 ):
      {
        struct VALEDBGRQENR : CP15Reg
        {
          uint32_t& reg( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).VALEDBGRQEN; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( _cpu ); }
        };
        
        static struct : VALEDBGRQENR
        {
          char const* Describe() { return "VAL Debug Request Enable Set Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) |= value; }
        } xset;
        
        static struct : VALEDBGRQENR
        { 
          char const* Describe() { return "VAL Debug Request Enable Clear Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( _cpu ) &= ~value; }
        } xclr;
        
        if (opcode2 == 3) return xset; return xclr;
      } break;
    
    case CP15ENCODE( 15, 0, 3, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "Correctable Fault Location Register"; }
          uint32_t& reg( CP15CPU& cpu ) { return dynamic_cast<CPU&>( cpu ).CFLR; }
          uint32_t Read( CP15CPU& cpu ) { return reg( cpu ); }
          void Write( CP15CPU& cpu, uint32_t value ) { reg( cpu ) = value; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 15, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "Invalidate all Data Cache Register"; }
          void Write( CP15CPU& cpu, uint32_t value ) {}
        } x;
        return x;
      } break;
      
    }
  
  // Fall back to base cpu CP15 registers
  return this->PCPU::CP15GetRegister( crn, opcode1, crm, opcode2 );
}

void
CPU::WaitForInterrupt()
{
  if (not check_external_event)
    Wait( external_event );
}

void
CPU::SetExternalEvent()
{
  check_external_event = true;
  external_event.notify( sc_core::SC_ZERO_TIME );
}

bool
CPU::GetExternalEvent()
{
  if (not check_external_event)
    return false;
  external_event.cancel();
  check_external_event = false;
  return true;
}

} // end of namespace cortex_r5f
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION
#undef TIME
#undef PHASE
#undef TRANS

