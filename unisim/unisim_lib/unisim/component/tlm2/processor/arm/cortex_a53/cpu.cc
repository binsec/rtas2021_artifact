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
 
#include <unisim/component/tlm2/processor/arm/cortex_a53/cpu.hh>
#include <unisim/component/cxx/processor/arm/vmsav8/cpu.tcc>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/likely/likely.hh>

#include <systemc>
#include <tlm>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace arm {
namespace cortex_a53 {

using namespace unisim::kernel::logger;

CPU::CPU( sc_core::sc_module_name const& name, Object* parent )
  : unisim::kernel::Object(name, parent)
  , sc_core::sc_module(name)
  , unisim::component::cxx::processor::arm::vmsav8::CPU<ConfigCA53>(name, parent)
  , master_socket("master_socket")
  , cpu_time(sc_core::SC_ZERO_TIME)
  , bus_time(sc_core::SC_ZERO_TIME)
  , quantum_time(sc_core::SC_ZERO_TIME)
  , cpu_cycle_time(62500.0, sc_core::SC_PS)
  , bus_cycle_time(62500.0, sc_core::SC_PS)
  , nice_time(1.0, sc_core::SC_MS)
  , time_per_instruction(62500.0, sc_core::SC_PS)
  , ipc(2.0)
  , enable_dmi(true)
  , verbose_tlm(false)
  // , ipc(2.0)
  // , enable_dmi(false)
  // , verbose_tlm(false)
{
  master_socket.bind(*this);
  
  SC_THREAD(Run);
}

CPU::~CPU()
{
}

/** Main thread of the CPU simulator.
 * It is a loop that simply executes one instruction and increases the cpu time.
 * Also the quantum time is updated, and if it is bigger than the nice time, the
 * global SystemC time is updated.
 */
void
CPU::Run()
{
  for (;;)
    {
      StepInstruction();
      
      quantum_time += time_per_instruction;
      if ( quantum_time > nice_time )
        Sync();
    }
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
                 // << std::endl << TIME(time)
                 // << std::endl << PHASE(phase)
                 << std::endl;
    //TRANS(PCPU::logger, trans);
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
                   << ", with unexpected phase"
                   // << std::endl << LOCATION
                   // << std::endl << TIME(time)
                   // << std::endl << PHASE(phase)
                   << std::endl;
      //TRANS(PCPU::logger, trans);
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
        PCPU::logger << DebugError << "Received nb_transport_bw on BEGIN_RESP phase, with unexpected write transaction"
                     // << std::endl << LOCATION
                     // << std::endl << TIME(time)
                     // << std::endl << PHASE(phase)
                     << std::endl;
        //TRANS(PCPU::logger, trans);
        PCPU::logger << EndDebug;
        Stop(-1);
        break;
      }
      sc_core::sc_time tmp_time = sc_core::sc_time_stamp();
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
               << "Reached end of nb_transport_bw, THIS SHOULD NEVER HAPPEN"
               // << std::endl << LOCATION
               // << std::endl << TIME(time)
               // << std::endl << PHASE(phase)
               << std::endl;
  //TRANS(PCPU::logger, trans);
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

void 
CPU::Reset()
{
}
  
void 
CPU::ResetMemory()
{
}

/** Wait for a specific event and update CPU times
 */

void
CPU::Wait( sc_core::sc_event const& evt )
{
  //if (quantum_time != sc_core::SC_ZERO_TIME)
  // Sync();
  wait( evt );
  sc_core::sc_time delta( sc_core::sc_time_stamp() );
  delta -= cpu_time;
  if (delta > quantum_time) {
    quantum_time = sc_core::SC_ZERO_TIME;
  } else {
    quantum_time -= delta;
  }
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
  
#if 0
  sc_core::sc_time deadline(cpu_time);
  deadline += quantum_time;
  while ( bus_time < deadline )
    bus_time += bus_cycle_time;
  quantum_time = bus_time;
  quantum_time -= cpu_time;
#else
  quantum_time += 
    ((((cpu_time + quantum_time) / bus_cycle_time) + 1) * bus_cycle_time) -
    (cpu_time + quantum_time);
#endif
  
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
CPU::PhysicalReadMemory( uint64_t addr, uint8_t* buffer, unsigned size )
{
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
  
  // cpu_time = sc_core::sc_time_stamp() + quantum_time;
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
CPU::PhysicalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size )
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

  // post3 - update DMI region cache
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

/**
 * Virtual method implementation to handle non intrusive reads performed by the inherited
 * cpu to perform external memory accesses.
 * It uses the TLM2 debugging interface to request the data.
 *
 * @param addr    the read base address
 * @param buffer  the buffer to copy the data to the read
 * @param size    the size of the read
 */

bool
CPU::ExternalReadMemory( uint64_t addr, uint8_t*       buffer, unsigned size )
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

  unsigned read_size = master_socket->transport_dbg(*trans);

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
CPU::ExternalWriteMemory( uint64_t addr, uint8_t const* buffer, unsigned size )
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

  unsigned write_size = master_socket->transport_dbg(*trans);

  return (trans->is_response_ok() and (write_size == size));
}

} // end of namespace cortex_a53
} // end of namespace arm
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION
#undef TIME
#undef PHASE
#undef TRANS

