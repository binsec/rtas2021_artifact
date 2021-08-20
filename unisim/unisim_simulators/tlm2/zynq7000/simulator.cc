/*
 *  Copyright (c) 2010-2018,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
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
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <simulator.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>
#include <unisim/component/cxx/processor/arm/register_field.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <inttypes.h>

using unisim::component::cxx::processor::arm::RegisterField;

void
ZynqRouter::relative_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end, tlm::tlm_target_socket<32u>& sock )
{
  this->mapping[output_port].used = true;
  this->mapping[output_port].range_start = range_start;
  this->mapping[output_port].range_end =   range_end;
  this->mapping[output_port].output_port = output_port;
  this->mapping[output_port].translation = 0;
  (*init_socket[output_port])( sock );
}

void
ZynqRouter::absolute_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end, tlm::tlm_target_socket<32u>& sock )
{
  this->mapping[output_port].used = true;
  this->mapping[output_port].range_start = range_start;
  this->mapping[output_port].range_end =   range_end;
  this->mapping[output_port].output_port = output_port;
  this->mapping[output_port].translation = range_start;
  (*init_socket[output_port])( sock );
}

ZynqRouter::ZynqRouter(const char* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object( name, parent )
  , unisim::component::tlm2::interconnect::generic_router::Router<ZynqRouterConfig>( name, parent )
{
}

namespace {
  std::string NameSocketFromModule( sc_core::sc_module_name const& name )
  {
    std::string res( name );
    res += "_socket";
    return res;
  }
}

MMDevice::MMDevice( sc_core::sc_module_name const& name, unisim::kernel::Object* parent )
  : unisim::kernel::Object( name, parent )
  , sc_core::sc_module( name )
  , unisim::kernel::Client<unisim::service::interfaces::TrapReporting>( name, parent )
  , socket( NameSocketFromModule( name ).c_str() )
  , trap_reporting_import("trap-reporting-import", this)
  , verbose( false )
  , hardfail( false )
{
  socket( *this );
}

unsigned int MMDevice::transport_dbg(tlm::tlm_generic_payload& payload) { throw std::runtime_error("Not implemented"); return 0; }

tlm::tlm_sync_enum
MMDevice::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
  if (phase != tlm::BEGIN_REQ) { throw std::logic_error("internal error"); }
  
  this->b_transport(payload, t);
  
  return tlm::TLM_COMPLETED;
}

void
MMDevice::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t )
{
  payload.set_dmi_allowed( false );
  
  if (uint8_t* byte_enable_ptr = payload.get_byte_enable_ptr())
    {
      for (int idx = int(payload.get_byte_enable_length()); --idx >= 0; )
        if (not byte_enable_ptr[idx])
          throw std::logic_error("internal error");
    }
  
  // unsigned int streaming_width = payload.get_streaming_width();
  bool status = false;
  
  tlm::tlm_command cmd = payload.get_command();
  switch (cmd)
    {
    case tlm::TLM_READ_COMMAND:
    case tlm::TLM_WRITE_COMMAND: {
      uint32_t addr = payload.get_address(), size = payload.get_data_length();
      
      if ((addr|size) & (size-1))
        std::cerr << "Malformed register address: [" << std::hex << addr << "," << std::dec << size << "].\n";
      else {
        Data d( payload.get_data_ptr(), cmd == tlm::TLM_WRITE_COMMAND, size );
        status = AccessRegister( addr, d, sc_core::sc_time_stamp() + t );
        if (verbose or not status)
          DumpRegisterAccess( std::cerr, addr, d );
        if (not status) {
          std::cerr <<  "Error: register access failed.\n";
          if (hardfail)
            this->Stop(-1);
        }
      }
    } break;
    case tlm::TLM_IGNORE_COMMAND:
      break;
    default:
      throw std::logic_error("internal error");
    }
  
  tlm::tlm_response_status resp_status = status ? tlm::TLM_OK_RESPONSE : tlm::TLM_ADDRESS_ERROR_RESPONSE;
  payload.set_response_status( resp_status );
}

void
MMDevice::DumpRegisterAccess( std::ostream& sink, uint32_t addr, MMDevice::Data const& d )
{
  sink << this->GetName() << " register " << (d.wnr?"write":"read")
       << " @[" << std::hex << addr << ',' << std::dec << d.size << "] ";
  sink << (d.wnr ? "<- 0x" : "-> 0x");
  for (int idx = d.size; --idx >= 0;) {
    uint8_t byte = d.ptr[idx];
    sink << "0123456789abcdef"[(byte>>4)&15]
         << "0123456789abcdef"[(byte>>0)&15];
  }
  sink << ".\n";
}

template <unsigned COUNT> struct ITP
{
  static void Register( MPCore& mpcore )
  {
    unsigned const idx = COUNT - 1;
    sc_core::sc_spawn_options opt;
    opt.set_sensitivity( &mpcore.interrupt_line_events[idx] );
    opt.dont_initialize();
    opt.spawn_method();
    std::ostringstream oss( "ITProcess" );
    oss << idx;
    
    sc_core::sc_spawn(sc_bind(&MPCore::ITProcess<idx>,&mpcore),oss.str().c_str(),&opt);
    
    ITP<idx>::Register( mpcore );
  }
};

template <> struct ITP<0u> { static void Register( MPCore& mpcore ) {} };

/**
 * Constructor.
 * 
 * @param name the name of the module
 * @param parent the parent service
 */
MPCore::MPCore(const sc_core::sc_module_name& name, unisim::kernel::Object* parent)
  : unisim::kernel::Object( name, parent )
  , MMDevice( name, parent )
  , nIRQ("nIRQ")
  , nFIQ("nFIQ")
  , ICCICR(0)
  , ICCPMR(0)
  , ICDDCR(0)
  , generate_exceptions_event("generate_exceptions_event")
{
  
  std::fill_n(IENABLE, state32_count, 0);
  std::fill_n(IPENDING, state32_count, 0);
  std::fill_n(IACTIVE, state32_count, 0);
  memset( &IPRIORITYR[0], 0, sizeof (IPRIORITYR) );
  memset( &ICDIPTR[0], 0, sizeof (ICDIPTR) );
  memset( &ICDICFR[0], 0, sizeof (ICDICFR) );
  
  SC_HAS_PROCESS(MPCore);
  
  SC_METHOD(GenerateExceptionsProcess);
  sc_core::sc_module::sensitive << generate_exceptions_event;
  
  ITP<ITLinesCount>::Register( *this );
}

namespace {
  template <uint32_t BITS, uint32_t MASK>
  struct Match {
    Match( uint32_t value ) : ok( ((value ^ BITS) & MASK) == 0 ), var( value & ~MASK ) {} bool ok; uint32_t var;
    operator bool () const { return ok; }
  };
}

bool
MPCore::AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time )
{
  if (d.size == 4) {
    uint32_t RAZ_WI(0);
    
    if (addr == 0x100) /* ICCICR: CPU Interface Control Register */ {
      d.Access( ICCICR );
    }
    
    else if (addr == 0x104) /* ICCPMR: Interrupt Priority Mask Register */ {
      d.Access( ICCPMR );
    }
    
    else if (addr == 0x10c) /* ICCIAR: Interrupt Acknowledge Register */ {
      if (d.wnr) return false;
      uint32_t iar = ReadGICC_IAR();
      d.Access( iar );
    }
    
    else if (addr == 0x110) /* ICCEOIR: End Of Interrupt Register */ {
      if (not d.wnr) return false;
      uint32_t eoir = 0;
      d.Access( eoir );
      unsigned int_id = eoir % 1024;
      if (int_id < 16)
        return false;
      // Ending interrupt
      IACTIVE[int_id / 32] &= ~(1u << (int_id % 32));
    }
    
    else if (addr == 0x1000) /* ICDDCR:  Distributor Control Register */ {
      d.Access( ICDDCR );
    }
    
    else if (addr == 0x1004) /* ICDICTR:  Interrupt Controller Type Register */ {
      if (d.wnr) return false;
      uint32_t result = ITLinesNumber;
      d.Access( result );
    }
    
    else if (Match<0x1100,-0x80> m = addr) /* ICDISER: Interrupt Set-enable Register  */ {
      unsigned idx = m.var >> 2;
      if (idx > state32_count)
        d.Access( RAZ_WI );
      else {
        uint32_t value = IENABLE[idx];
        d.Access( value );
        if (d.wnr) IENABLE[idx] |= value;
      }
    }
    
    else if (Match<0x1180,-0x80> m = addr) /* ICDICER: Interrupt Clear-Enable Register */ {
      unsigned idx = m.var >> 2;
      if (idx > state32_count)
        d.Access( RAZ_WI );
      else {
        uint32_t value = IENABLE[idx];
        d.Access( value );
        if (d.wnr) IENABLE[idx] &= ~value;
      }
    }
    
    else if (Match<0x1400,-0x400> m = addr) /* ICDIPR: Interrupt Priority Register */ {
      unsigned idx = m.var;
      if (idx < ITLinesCount)
        d.Copy( &IPRIORITYR[idx], 4 );
      else
        d.Access( RAZ_WI );
    }
    
    else if (Match<0x1800,-0x400> m = addr) /* ICDIPTR: Interrupt Processor Targets Register */ {
      unsigned idx = m.var;
      if (idx < 16) {
        uint32_t value = 0x01010101;
        d.Access( value );
      }
      else if ((24 <= idx) and (idx < ITLinesCount)) {
        d.Copy( &ICDIPTR[idx], 4 );
      }
      else
        d.Access( RAZ_WI );
    }
    
    else if (Match<0x1c00,-0x100> m = addr) /* ICDICFR: Interrupt Configuration Register */ {
      unsigned idx = m.var >> 2;
      d.Access( (idx < icfgr_count) ? ICDICFR[idx] : RAZ_WI );
    }
    
    else
      return false;
  }
  
  else return false;

  if (d.wnr)
    generate_exceptions_event.notify(update_time - sc_core::sc_time_stamp());
  
  return true;
}

void
MPCore::ITProcess( unsigned idx )
{
  IPENDING[idx/32] |= (1ul << (idx%32));
  generate_exceptions_event.notify(sc_core::SC_ZERO_TIME);
}

unsigned
MPCore::HighestPriorityPendingInterrupt( uint8_t required, uint8_t enough )
{
  unsigned hppi = 1023;
  
  if ((ICCICR & ICDDCR & 1) == 0)
    return hppi;
  
  // Preemption not supported                                                                                                                               
  for (unsigned idx = 0; idx < state32_count; ++ idx)
    if (IACTIVE[idx])
      return hppi;
  
  for (unsigned int_id = 0; int_id < ITLinesCount; int_id += 1) {
    unsigned word = int_id / 32;
    uint32_t bit = 1u << (int_id % 32);
    if (not (IPENDING[word] & IENABLE[word] & bit))
      continue;
    uint8_t priority = IPRIORITYR[int_id];
    if (priority < enough)
      return int_id;
    if (priority < required) {
      hppi = int_id;
      required = priority;
    }
  }
    
  return hppi;
}

void
MPCore::GenerateExceptionsProcess()
{
  nIRQ = not (HighestPriorityPendingInterrupt( ICCPMR, ICCPMR ) < ITLinesCount);
}

uint32_t
MPCore::ReadGICC_IAR()
{
  unsigned int_id = HighestPriorityPendingInterrupt( ICCPMR, 0 );
  if (int_id < 16)
    throw std::logic_error("not implemented");
  
  // Acknowledging
  unsigned word = int_id / 32;
  uint32_t bit = 1u << (int_id % 32);
  IPENDING[word] &= ~bit;
  IACTIVE[word] |= bit;
  
  return int_id;
}

TTC::TTC( const sc_core::sc_module_name& name, unisim::kernel::Object* parent, MPCore& _mpcore, unsigned _id, unsigned _base_it )
  : unisim::kernel::Object( name, parent )
  , MMDevice( name, parent )
  , mpcore( _mpcore )
  , id( _id )
  , base_it( _base_it )
  , update_state_event( "update_state_event" )
  , clock_period( sc_core::SC_ZERO_TIME )
{
  std::fill_n(Clock_Control, 3, 0);
  std::fill_n(Counter_Control, 3, 0x21);
  std::fill_n(Counter_Value, 3, 0);
  std::fill_n(Interval_Counter, 3, 0);
  std::fill_n(Interrupt_Register, 3, 0);
  std::fill_n(Interrupt_Enable, 3, 0);
  std::fill_n(it_lines, 3, -1);
  std::fill_n(update_counters, 3, 0);
  
  SC_HAS_PROCESS(TTC);
  
  SC_METHOD(UpdateStateProcess);
  sc_core::sc_module::sensitive << update_state_event;
}

void
TTC::UpdateStateProcess()
{
  UpdateState( sc_core::sc_time_stamp() );
}

void
TTC::UpdateCounterState( unsigned idx, sc_core::sc_time const& update_time )
{
  update_counters[idx] += 1;
  if (Clock_Control[idx] & 0x28)
    throw std::logic_error( "internal error" );
  if (Counter_Control[idx] & 0x10) {
    Counter_Value[idx] = 0;
    Counter_Control[idx] &= ~0x10;
  }
  if (Counter_Control[idx] & 1) {
    // Disabled
    last_state_update_time[idx] = update_time;
    return;
  }
  
  // Computing counter ticks since last update
  sc_dt::uint64 ticks;
  sc_core::sc_time  tick_period( clock_period );
  if (Clock_Control[idx] & 1) // Prescaling
    tick_period *= sc_dt::uint64( 1 << (((Clock_Control[idx] >> 1) & 0xf) + 1) );
  
  ticks = (update_time - last_state_update_time[idx]) / tick_period;
  last_state_update_time[idx] += ticks*tick_period;
  
  bool interval_mode = (Counter_Control[idx] & 2);
  uint32_t interrupt_vector = interval_mode ? 0x01 : 0x10;
  bool interrupt_enable = interrupt_vector & Interrupt_Enable[idx];
  sc_dt::uint64 interval = interval_mode ? Interval_Counter[idx] : 0x10000;
  sc_dt::uint64 counter_value = Counter_Value[idx];
  
  // Computing ticks to next zero and updating the counter value to its current state
  sc_dt::uint64 ticks_to_next_zero = 0;
  if (Counter_Control[idx] & 4) {
    // Decrement
    ticks_to_next_zero = counter_value ? counter_value : interval;
    if (ticks > counter_value) {
      counter_value = interval - 1 - ((ticks - counter_value - 1) % interval);
    } else {
      counter_value = counter_value - ticks;
    }
  } else {
    // Increment
    if (interval > counter_value) {
      ticks_to_next_zero = interval - counter_value;
      counter_value = (counter_value + ticks) % interval;
    } else {
      ticks_to_next_zero = 1;
      if (ticks > 0)
        counter_value = (ticks - 1) % interval;
    }
  }
  
  if (interrupt_enable) {
    if (ticks_to_next_zero < ticks)
      throw std::logic_error("internal error");
    
    ticks_to_next_zero -= ticks;
    
    sc_core::sc_time const& sc_now = sc_core::sc_time_stamp();
    sc_core::sc_time sc_int( last_state_update_time[idx] );
    sc_int += ticks_to_next_zero*tick_period;
    
    if (sc_int < sc_now)
      throw std::logic_error("internal error");
    
    sc_int -= sc_now;
    
    if (ticks_to_next_zero <= 0) {
      Interrupt_Register[idx] |= interrupt_vector;
      mpcore.interrupt_line_events[base_it + idx].notify(sc_int);
    }
    update_state_event.notify( sc_int );
  }
  
  Counter_Value[idx] = counter_value;
}

void
TTC::UpdateState( sc_core::sc_time const& update_time )
{
  for (unsigned idx = 0; idx < 3; ++idx)
    UpdateCounterState( idx, update_time );
}

bool
TTC::AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time )
{
  UpdateState( update_time );
  int update_idx = -1;
  
  if (d.size == 4) {
    switch (addr) {
    case 0x00: d.Access( Clock_Control[0] ); update_idx = 0; break;
    case 0x04: d.Access( Clock_Control[1] ); update_idx = 1; break;
    case 0x08: d.Access( Clock_Control[2] ); update_idx = 2; break;
    case 0x0c: d.Access( Counter_Control[0] ); update_idx = 0; break;
    case 0x10: d.Access( Counter_Control[1] ); update_idx = 1; break;
    case 0x14: d.Access( Counter_Control[2] ); update_idx = 2; break;
    case 0x18: d.Access( Counter_Value[0] ); break;
    case 0x1c: d.Access( Counter_Value[1] ); break;
    case 0x20: d.Access( Counter_Value[2] ); break;
    case 0x24: d.Access( Interval_Counter[0] ); update_idx = 0; break;
    case 0x28: d.Access( Interval_Counter[1] ); update_idx = 1; break;
    case 0x2c: d.Access( Interval_Counter[2] ); update_idx = 2; break;
    case 0x54: d.Access( Interrupt_Register[0] ); if (not d.wnr) Interrupt_Register[0] = 0; break;
    case 0x58: d.Access( Interrupt_Register[1] ); if (not d.wnr) Interrupt_Register[1] = 0; break;
    case 0x5c: d.Access( Interrupt_Register[2] ); if (not d.wnr) Interrupt_Register[2] = 0; break;
    case 0x60: d.Access( Interrupt_Enable[0] ); update_idx = 0; break;
    case 0x64: d.Access( Interrupt_Enable[1] ); update_idx = 1; break;
    case 0x68: d.Access( Interrupt_Enable[2] ); update_idx = 2; break;
    default: return false;
    }
  }
  else return false;
  
  if (d.wnr and (update_idx >= 0))
    UpdateCounterState( update_idx, update_time );
  
  return true;
}

PS_UART::PS_UART( sc_core::sc_module_name const& name, unisim::kernel::Object* parent, MPCore& _mpcore, int _it_line )
  : unisim::kernel::Object( name, parent )
  , MMDevice( name, parent )
  , unisim::kernel::Client<unisim::service::interfaces::CharIO>( name, parent )
  , char_io_import("char-io-import", this)
  , exchange_event( "exchange_event" )
  , bit_period( sc_core::SC_ZERO_TIME )
  , last_rx( sc_core::SC_ZERO_TIME )
  , rx_timeout_active(true)
  , mpcore( _mpcore )
  , it_line( _it_line )
  , TxFIFO()
  , RxFIFO()
  , CR(0b100101000)
  , MR(0)
  , IMR(0)
  , ISR(0)
  , BAUDGEN(0x28b)
  , RXTOUT(0)
  , BDIV(0xf)
  , TTRIG(FIFO::CAPACITY/2)
  , RTRIG(FIFO::CAPACITY/2)
  , FDEL(0)
{
  MMDevice::verbose = false;
  MMDevice::hardfail = true;
  
  SC_HAS_PROCESS(PS_UART);
  SC_METHOD(ExchangeProcess);
  sc_core::sc_module::sensitive << exchange_event;
}

void
PS_UART::reload_rxtout( sc_core::sc_time const& update_time )
{
  last_rx = update_time;
  exchange_event.notify( bit_period*rx_timeout_ticks() );
}

bool
PS_UART::AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time )
{
  if (d.size == 4) {
    switch (addr) {
    case 0x00:
      d.Access( CR );
      if (d.wnr) {
        uint32_t const zero(0);
        if (RegisterField<0,1>().Swap( CR, zero ))
          RxFIFO.Clear();
        RegisterField<1,1>().Set( CR, zero );
        if (RegisterField<6,1>().Swap( CR, zero ))
          reload_rxtout( update_time );
      }
      return true;
    case 0x04: d.Access( MR ); return true;
    case 0x08: { // IER
      if (not d.wnr) return false;
      uint32_t enable_mask = 0;
      d.Access( enable_mask );
      IMR |= enable_mask;
    } return true;
    case 0x0c: { // IDR
      if (not d.wnr) return false;
      uint32_t disable_mask = 0;
      d.Access( disable_mask );
      IMR &= ~disable_mask;
    } return true;
    case 0x10:
      if (d.wnr) return false;
      d.Access( IMR ); return true;
    case 0x14: {
      if (d.wnr) {
        uint32_t disable_mask = 0;
        d.Access( disable_mask );
        ISR &= ~disable_mask;
        // if (disable_mask & 0x100)
        //   trap_reporting_import->ReportTrap(*this, "WTF???");
      } else
        d.Access( ISR );
      return true;
    }
    case 0x18: d.Access( BAUDGEN ); return true;
    case 0x1c:
      d.Access( RXTOUT );
      if (d.wnr) { RXTOUT &= 0xff; reload_rxtout( update_time ); }
      return true;
    case 0x20: d.Access( RTRIG ); RTRIG &= 0x3f; return true;
    case 0x2c: {
      // Channel_sts_reg
      if (d.wnr) return false;
      uint32_t SR = 0;
      RegisterField<14,1>().Set( SR, TxFIFO.NearlyFull() );
      RegisterField<13,1>().Set( SR, TxFIFO.Trig( TTRIG ) );
      RegisterField<12,1>().Set( SR, (FDEL >= 4) and RxFIFO.Trig( FDEL ) );
      RegisterField<11,1>().Set( SR, false );
      RegisterField<10,1>().Set( SR, false );
      RegisterField <4,1>().Set( SR, TxFIFO.Full() );
      RegisterField <3,1>().Set( SR, TxFIFO.Empty() );
      RegisterField <2,1>().Set( SR, RxFIFO.Full() );
      RegisterField <1,1>().Set( SR, RxFIFO.Empty() );
      RegisterField <0,1>().Set( SR, RxFIFO.Trig( RTRIG ) );
      d.Access( SR );
    } return true;
    case 0x30: { 
      if (d.wnr) PutChar( d ); // Tx
      else       GetChar( d ); // Rx
    } return true;
    case 0x34: d.Access( BDIV ); BDIV &= 0xff; return true;
    case 0x38: d.Access( FDEL ); FDEL &= 0x3f; return true;
    case 0x44: d.Access( TTRIG ); TTRIG &= 0x3f; return true;
    default: return false;
    }
  }
  else return false;
  
  return false;
}

void
PS_UART::PutChar( Data const& d )
{
  uint32_t value = 0;
  d.Access( value );
  if (value & -256)
    throw std::logic_error("Junk bits in Tx");
  if (not char_io_import)
    throw std::logic_error("no IO client connected");
  char_io_import->PutChar( char(value) );
  
  if (RegisterField<3,1>().Get( IMR )) /* TEMPTY */
    exchange_event.notify( 8*bit_period );
}

void
PS_UART::GetChar( Data const& d )
{
  uint32_t value = 0;
  if (not RxFIFO.Empty())
    value = RxFIFO.Pull();
  d.Access( value );
}

void
PS_UART::ExchangeProcess()
{
  if (not char_io_import)
    return;
  
  sc_core::sc_time update_time( sc_core::sc_time_stamp() );
  exchange_event.notify( bit_period*1024 );
  
  /*** Error handling ***/
  // No error possible
  
  /*** - ISR update ***/
  RegisterField <9,1>().Set( ISR, 0 ); // DMSI, Delta Modem Status Indicator
  RegisterField <7,1>().Set( ISR, 0 ); // PARE, Receiver Parity Error
  RegisterField <6,1>().Set( ISR, 0 ); // FRAME, Receiver Framing Error 
  RegisterField <5,1>().Set( ISR, 0 ); // ROVR, Receiver Overflow Error
  
  /*** Tx Handling ***/
  char_io_import->FlushChars();
  
  /*** - ISR update ***/
  RegisterField<12,1>().Set( ISR, 0 ); // TOVR, Transmitter FIFO Overflow
  RegisterField<11,1>().Set( ISR, 0 ); // TNFULL, Transmitter FIFO Nearly Full
  RegisterField<10,1>().Set( ISR, 0 ); // TTRIG, Transmitter FIFO Trigger
  RegisterField <4,1>().Set( ISR, 0 ); // TFUL, Transmitter FIFO Full
  RegisterField <3,1>().Set( ISR, 1 ); // TEMPTY, Transmitter FIFO Empty
  
  /*** Rx Handling ***/
  for (char ch; (not RxFIFO.Full()) and (char_io_import->GetChar( ch ));) {
    RxFIFO.Push( ch );
    last_rx = update_time;
  }
  
  /*** - ISR update ***/
  if (rx_timeout_active and RXTOUT and ((update_time - last_rx) >= (bit_period*rx_timeout_ticks()))) {
    RegisterField <8,1>().Set( ISR, 1 ); // TIMEOUT, Receiver Timeout Error
    //rx_timeout_active = false; // WTF? xilinx says that timeout is inactive until RST_TO (which linux never gives)
    reload_rxtout( update_time );
  }
  if (RxFIFO.Full())
    RegisterField <2,1>().Set( ISR, 1 ); // RFUL, Receiver FIFO Full
  if (RxFIFO.Empty())
    RegisterField <1,1>().Set( ISR, 1 ); // REMPTY, Receiver FIFO Empty
  if (RxFIFO.Trig( RTRIG ))
    RegisterField <0,1>().Set( ISR, 1 ); // RTRIG, Receiver FIFO Trigger
  
  /*** Interrupt Generation ***/
  if (ISR & IMR)
    mpcore.interrupt_line_events[it_line].notify(sc_core::SC_ZERO_TIME);
}

SLCR::SLCR(const sc_core::sc_module_name& name, Simulator& _simulator, unisim::kernel::Object* parent )
  : unisim::kernel::Object( name, parent )
  , MMDevice( name, parent )
  , simulator(_simulator)
  , ARM_PLL_CTRL(0x1a008)
  , DDR_PLL_CTRL(0x1a008)
  , IO_PLL_CTRL(0x1a008)
  , ARM_CLK_CTRL(0x1f000400)
  , DDR_CLK_CTRL(0x18400003)
  , CLK_621_TRUE(0x1)
  , UART_CLK_CTRL(0x3f03)
{
}

bool
SLCR::AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time )
{
  struct Finally {
    Finally( Simulator& _sim, bool _fixclks ) : sim(_sim), fixclks(_fixclks) {} Simulator& sim; bool fixclks;
    ~Finally() { if (fixclks) sim.UpdateClocks(); }
  } update_clocks( simulator, d.wnr );
      
  if (d.size == 4) {
    switch (addr) {
    case 0x100: d.Access(  ARM_PLL_CTRL ); return true;
    case 0x104: d.Access(  DDR_PLL_CTRL ); return true;
    case 0x108: d.Access(   IO_PLL_CTRL ); return true;
    case 0x120: d.Access(  ARM_CLK_CTRL ); return true;
    case 0x124: d.Access(  DDR_CLK_CTRL ); return true;
    case 0x154: d.Access( UART_CLK_CTRL ); return true;
    case 0x1c4: d.Access(  CLK_621_TRUE ); return true;
    }
  }
  
  return false;
}

L2C::L2C( const sc_core::sc_module_name& name, unisim::kernel::Object* parent )
  : unisim::kernel::Object( name, parent )
  , MMDevice( name, parent )
{
}

bool
L2C::AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time )
{
  if (d.size == 4) {
    //uint32_t RAZ_WI(0);
    
    switch (addr) {
    case 0x0000: d.Access( fastreg( addr, 0x410000c8 ) ); return true;
      // case 0x4: { uint32_t reg0_cache_type = 0; d.Access( reg0_cache_type ); return true; }
    case 0x0100: d.Access( fastreg( addr, 0x00000000 ) ); return true;
    case 0x0108: d.Access( fastreg( addr, 0x00000777 ) ); return true;
    case 0x010c: d.Access( fastreg( addr, 0x00000777 ) ); return true;
    case 0x0c00: d.Access( fastreg( addr, 0x40000001 ) ); return true;
    case 0x0c04: d.Access( fastreg( addr, 0xfff00000 ) ); return true;
    }
  }
  
  return false;
}

Simulator::Simulator(int argc, char **argv, const sc_core::sc_module_name& name)
  : unisim::kernel::tlm2::Simulator(name, argc, argv, Simulator::DefaultConfiguration)
  , ps_clk_period(sc_core::SC_ZERO_TIME)
  , cpu( "cpu", this )
  , router( "router", this )
  , mpcore( "mpcore", this )
  , main_ram( "main_ram", this )
  , boot_rom( "boot_rom", this )
  , slcr( "slcr", *this )
  , ttc0( "ttc0", 0, mpcore, 0, 42 )
  , ttc1( "ttc1", 0, mpcore, 1, 69 )
  , uart0( "uart0", 0, mpcore, 59 )
  , uart1( "uart1", 0, mpcore, 82 )
  , l2c( "l2c", 0 )
  , netstreamer0("netstreamer0", 0)
  , netstreamer1("netstreamer1", 0)
  , char_io_tee0("char-io-tee0", 0)
  , char_io_tee1("char-io-tee1", 0)
  , http_server("http-server", 0)
  , web_terminal0("web-terminal0", 0)
  , web_terminal1("web-terminal1", 0)
  , time("time")
  , host_time("host-time")
  , loader("loader")
  , simulation_spent_time(0.0)
  , debugger(0)
  , gdb_server(0)
  , inline_debugger(0)
  , profiler(0)
  , instrumenter(0)
  , logger_console_printer(0)
  , logger_text_file_writer(0)
  , logger_http_writer(0)
  , logger_xml_file_writer(0)
  , logger_netstream_writer(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server( "enable-gdb-server", 0, enable_gdb_server, "Enable GDB server." )
  , enable_inline_debugger(false)
  , param_enable_inline_debugger( "enable-inline-debugger", 0, enable_inline_debugger, "Enable inline debugger." )
  , enable_profiler(false)
  , param_enable_profiler( "enable-profiler", 0, enable_profiler, "Enable inline debugger." )
{
  param_enable_gdb_server.SetMutable(false);
  param_enable_inline_debugger.SetMutable(false);
  param_enable_profiler.SetMutable(false);
  
  logger_console_printer = new LOGGER_CONSOLE_PRINTER();
  logger_text_file_writer = new LOGGER_TEXT_FILE_WRITER();
  logger_http_writer = new LOGGER_HTTP_WRITER();
  logger_xml_file_writer = new LOGGER_XML_FILE_WRITER();
  logger_netstream_writer = new LOGGER_NETSTREAM_WRITER();
  
  instrumenter = new INSTRUMENTER("instrumenter", this);
  
  // - debugger
  if (enable_gdb_server)
    gdb_server = new GDB_SERVER("gdb-server");
  if (enable_inline_debugger)
    inline_debugger = new INLINE_DEBUGGER( "inline-debugger" );
  if (enable_profiler)
    profiler = new PROFILER( "profiler" );
  if (gdb_server or inline_debugger or profiler)
    debugger = new DEBUGGER( "debugger" );

  instrumenter->CreateSignal("nIRQm", true);
  instrumenter->CreateSignal("nFIQm", true);
  instrumenter->CreateSignal("nRESETm", true);
  instrumenter->CreateClock("CLK");
  
  instrumenter->RegisterPort(cpu.nIRQm);
  instrumenter->RegisterPort(cpu.nFIQm);
  instrumenter->RegisterPort(cpu.nRESETm);

  cpu.master_socket( *router.targ_socket[0] );
  instrumenter->Bind("HARDWARE.cpu.nIRQm", "HARDWARE.nIRQm");
  instrumenter->Bind("HARDWARE.cpu.nFIQm", "HARDWARE.nFIQm");
  instrumenter->Bind("HARDWARE.cpu.nRESETm", "HARDWARE.nRESETm");
  
  instrumenter->RegisterPort(router.input_if_clock);
  instrumenter->RegisterPort(router.output_if_clock);
  
  instrumenter->Bind("HARDWARE.router.input_if_clock", "HARDWARE.CLK");
  instrumenter->Bind("HARDWARE.router.output_if_clock", "HARDWARE.CLK");
  
  router.relative_mapping( 0, 0x00000000, 0x3fffffff, main_ram.slave_sock ); /* Main OCM RAM */
  router.absolute_mapping( 1, 0xffff0000, 0xffffffff, boot_rom.slave_sock ); /* Boot OCM ROM */
  router.relative_mapping( 2, 0xf8f00000, 0xf8f01fff, mpcore.socket ); /* Mpcore - SCU, Interrupt controller, Counters and Timers */
  router.relative_mapping( 3, 0xf8000000, 0xf8000fff, slcr.socket ); /* SLCR */
  router.relative_mapping( 4, 0xf8001000, 0xf8001fff, ttc0.socket ); /* TTC0 */
  router.relative_mapping( 5, 0xf8002000, 0xf8002fff, ttc1.socket ); /* TTC1 */
  router.relative_mapping( 6, 0xf8f02000, 0xf8f02fff, l2c.socket ); /* L2Cpl310 */
  router.relative_mapping( 7, 0xe0000000, 0xe0000fff, uart0.socket ); /* uart0 */
  router.relative_mapping( 8, 0xe0001000, 0xe0001fff, uart1.socket ); /* uart1 */
  
  instrumenter->RegisterPort(mpcore.nIRQ);
  instrumenter->RegisterPort(mpcore.nFIQ);
  
  instrumenter->Bind("HARDWARE.mpcore.nIRQ", "HARDWARE.nIRQm");
  instrumenter->Bind("HARDWARE.mpcore.nFIQ", "HARDWARE.nFIQm");
  
  uart0.char_io_import >> char_io_tee0.char_io_export;
  (*char_io_tee0.char_io_import[0]) >> netstreamer0.char_io_export;
  (*char_io_tee0.char_io_import[1]) >> web_terminal0.char_io_export;
  
  uart1.char_io_import >> char_io_tee1.char_io_export;
  (*char_io_tee1.char_io_import[0]) >> netstreamer1.char_io_export;
  (*char_io_tee1.char_io_import[1]) >> web_terminal1.char_io_export;

  cpu.symbol_table_lookup_import >> loader.symbol_table_lookup_export;
  *loader.memory_import[0] >> main_ram.memory_export;
  *loader.memory_import[1] >> boot_rom.memory_export;
  // debugger->blob_import >> linux_os->blob_export_;
  // debugger->loader_import >> loader.loader_export;

  if (debugger)
    {
      // Debugger <-> CPU+peripherals  connections
      cpu.debug_yielding_import                            >> *debugger->debug_yielding_export[0];
      cpu.memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
      cpu.trap_reporting_import                            >> *debugger->trap_reporting_export[0];
      mpcore.trap_reporting_import                         >> *debugger->trap_reporting_export[0];
      uart0.trap_reporting_import                          >> *debugger->trap_reporting_export[0];
      uart1.trap_reporting_import                          >> *debugger->trap_reporting_export[0];
      *debugger->disasm_import[0]                          >> cpu.disasm_export;
      *debugger->memory_import[0]                          >> cpu.memory_export;
      *debugger->registers_import[0]                       >> cpu.registers_export;
      *debugger->memory_access_reporting_control_import[0] >> cpu.memory_access_reporting_control_export;
      
      // debug
      debugger->blob_import >> loader.blob_export;
    }

  if (inline_debugger)
    {
      // Connect inline-debugger to debugger
      *debugger->debug_event_listener_import[0]      >> inline_debugger->debug_event_listener_export;
      *debugger->debug_yielding_import[0]            >> inline_debugger->debug_yielding_export;
      inline_debugger->debug_yielding_request_import >> *debugger->debug_yielding_request_export[0];
      inline_debugger->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[0];
      inline_debugger->disasm_import                 >> *debugger->disasm_export[0];
      inline_debugger->memory_import                 >> *debugger->memory_export[0];
      inline_debugger->registers_import              >> *debugger->registers_export[0];
      inline_debugger->stmt_lookup_import            >> *debugger->stmt_lookup_export[0];
      inline_debugger->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[0];
      inline_debugger->backtrace_import              >> *debugger->backtrace_export[0];
      inline_debugger->debug_info_loading_import     >> *debugger->debug_info_loading_export[0];
      inline_debugger->data_object_lookup_import     >> *debugger->data_object_lookup_export[0];
      inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[0];
    }
  
  if (gdb_server)
    {
      // Connect gdb-server to debugger
      *debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
      *debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
      gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
      gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
      gdb_server->memory_import                 >> *debugger->memory_export[1];
      gdb_server->registers_import              >> *debugger->registers_export[1];
    }
  
  if (profiler)
    {
      *debugger->debug_yielding_import[2]       >> profiler->debug_yielding_export;
      *debugger->debug_event_listener_import[2] >> profiler->debug_event_listener_export;
      profiler->debug_yielding_request_import      >> *debugger->debug_yielding_request_export[2];
      profiler->debug_event_trigger_import         >> *debugger->debug_event_trigger_export[2];
      profiler->disasm_import                      >> *debugger->disasm_export[2];
      profiler->memory_import                      >> *debugger->memory_export[2];
      profiler->registers_import                   >> *debugger->registers_export[2];
      profiler->stmt_lookup_import                 >> *debugger->stmt_lookup_export[2];
      profiler->symbol_table_lookup_import         >> *debugger->symbol_table_lookup_export[2];
      profiler->backtrace_import                   >> *debugger->backtrace_export[2];
      profiler->debug_info_loading_import          >> *debugger->debug_info_loading_export[2];
      profiler->data_object_lookup_import          >> *debugger->data_object_lookup_export[2];
      profiler->subprogram_lookup_import           >> *debugger->subprogram_lookup_export[2];
    }
    
   *http_server.http_server_import[0] >> logger_http_writer->http_server_export;
   *http_server.http_server_import[1] >> instrumenter->http_server_export;
   *http_server.http_server_import[2] >> web_terminal0.http_server_export;
   *http_server.http_server_import[3] >> web_terminal1.http_server_export;
   if (profiler)
   {
     *http_server.http_server_import[4] >> profiler->http_server_export;
   }
   
   *http_server.registers_import[0] >> cpu.registers_export;
}

Simulator::~Simulator()
{
  delete gdb_server;
  delete inline_debugger;
  delete profiler;
  delete debugger;
  delete instrumenter;
  delete logger_console_printer;
  delete logger_text_file_writer;
  delete logger_http_writer;
  delete logger_xml_file_writer;
  delete logger_netstream_writer;
}

int
Simulator::Run()
{
  if (unlikely(SimulationFinished())) return 0;

  double time_start = host_time.GetTime();
  
  unisim::kernel::tlm2::Simulator::Run();

  double time_stop = host_time.GetTime();
  double spent_time = time_stop - time_start;
  simulation_spent_time += spent_time;

  std::cerr << "Simulation run-time parameters:" << std::endl;
  DumpParameters(std::cerr);
  std::cerr << std::endl;
  
  
  std::cerr << std::endl;
  std::cerr << "Simulation statistics:" << std::endl;
  DumpStatistics(std::cerr);
  std::cerr << std::endl;

  std::cerr << "simulation time: " << simulation_spent_time << " seconds" << std::endl;
  std::cerr << "simulated time: " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
  std::cerr << "guest instructions: " << uint64_t(cpu["instruction-counter"]) << std::endl;
  std::cerr << "host simulation speed: " << ((double) cpu["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << std::endl;
  std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;
  
  std::cerr << "WB counts: " << cpu.get_cwp_stats(false) << std::endl;
  std::cerr << "WT counts: " << cpu.get_cwp_stats(true) << std::endl;
  
  if (profiler) profiler->Output();
  
  return GetExitStatus();
}

void
Simulator::UpdateClocks()
{
  /* PLLs*/
  sc_core::sc_time
    arm_pll_period = ps_clk_period / RegisterField<12,7>().Get( slcr.ARM_PLL_CTRL ), // PLL Feedback divider
    ddr_pll_period = ps_clk_period / RegisterField<12,7>().Get( slcr.DDR_PLL_CTRL ), // PLL Feedback divider
    io_pll_period  = ps_clk_period / RegisterField<12,7>().Get( slcr.IO_PLL_CTRL );  // PLL Feedback divider
  
  if (RegisterField<3,2>().Get( slcr.ARM_PLL_CTRL ) & 2)
    throw std::logic_error("ARM PLL Bypass not supported");
  if (RegisterField<3,2>().Get( slcr.DDR_PLL_CTRL ) & 2)
    throw std::logic_error("DDR PLL Bypass not supported");
  if (RegisterField<3,2>().Get( slcr.IO_PLL_CTRL ) & 2)
    throw std::logic_error("IO PLL Bypass not supported");
  
  
  /* CPU Clock */
  {
    // Source selection
    unsigned srcsel = RegisterField<4,2>().Get( slcr.ARM_CLK_CTRL );
    sc_core::sc_time& pll_period = (srcsel & 2) ? (srcsel & 1) ? io_pll_period : ddr_pll_period : arm_pll_period;
    // 6-bit Programmable Divider
    sc_core::sc_time cpu_6x4x_period = pll_period * RegisterField<8,6>().Get( slcr.ARM_CLK_CTRL ); // divisor
    // Clock Ratio Generator
    bool is621( RegisterField<0,1>().Get( slcr.CLK_621_TRUE ) );
    sc_core::sc_time cpu_3x2x_period = cpu_6x4x_period * 2;
    sc_core::sc_time cpu_2x_period = cpu_6x4x_period * (is621 ? 3 : 2);
    sc_core::sc_time cpu_1x_period = cpu_2x_period * 2;
    std::cerr << "cpu_6x4x_period: " << cpu_6x4x_period.to_string() << " -> cpu" << std::endl
              << "cpu_3x2x_period: " << cpu_3x2x_period.to_string() << " -> " << std::endl
              << "cpu_2x_period: " << cpu_2x_period.to_string() << " -> bus, router, boot_rom" << std::endl
              << "cpu_1x_period: " << cpu_1x_period.to_string() << " -> TTC" << std::endl;
      
    // Clock activation ...
    cpu["cpu-cycle-time"] = cpu_6x4x_period.to_string().c_str();
    cpu["bus-cycle-time"] = cpu_2x_period.to_string().c_str();
    boot_rom["cycle-time"] = cpu_2x_period.to_string().c_str();
    boot_rom["read-latency"] = cpu_2x_period.to_string().c_str();
    boot_rom["write-latency"] = 0;
    router["cycle_time"] = cpu_2x_period.to_string().c_str();
    ttc0.clock_period = ttc1.clock_period = cpu_1x_period;
  }
  /* DDR Clock */
  {
    // 6-bit Programmable Divider
    sc_core::sc_time ddr_3x_period = ddr_pll_period * RegisterField<20,6>().Get( slcr.DDR_CLK_CTRL ); // divisor
    sc_core::sc_time ddr_2x_period = ddr_pll_period * RegisterField<26,6>().Get( slcr.DDR_CLK_CTRL ); // divisor
    std::cerr << "ddr_3x_period: " << ddr_3x_period.to_string() << " -> " << std::endl
              << "ddr_2x_period: " << ddr_2x_period.to_string() << " -> main_ram" << std::endl;
    // Clock activation ...
    main_ram["cycle-time"] = ddr_2x_period.to_string().c_str();
    main_ram["read-latency"] = ddr_2x_period.to_string().c_str();
    main_ram["write-latency"] = 0;
  }
  /* UART clock */
  // Whatever control registers are we provide a 115200 bauds clock
  uart1.bit_period = uart0.bit_period = sc_core::sc_time( 9, sc_core::SC_US ); /* Approx 115200 bauds ;-) */
  /* TTC Clock */
  
  
}

unisim::kernel::Simulator::SetupStatus
Simulator::Setup()
{
  if (inline_debugger or profiler)
    {
      SetVariable("debugger.parse-dwarf", true);
    }
  
  // Build the Loader arguments from the command line arguments
  
  unisim::kernel::VariableBase *cmd_args = FindVariable("cmd-args");
  unsigned int cmd_args_length = cmd_args->GetLength();
  if(cmd_args_length > 0)
    {
      SetVariable( "loader.filename", ((std::string)(*cmd_args)[0]).c_str() );
    }

  unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();
  
  ps_clk_period = sc_core::sc_time( 30, sc_core::SC_NS ); // 33 MHz
  
  UpdateClocks();
  
  return setup_status;
}

bool Simulator::EndSetup()
{
  if (profiler)
  {
    http_server.AddJSAction(
      unisim::service::interfaces::ToolbarOpenTabAction(
        /* name */      profiler->GetName(), 
        /* label */     "<img src=\"/unisim/service/debug/profiler/icon_profile_cpu0.svg\" alt=\"Profile\">",
        /* tips */      std::string("Profile of ") + cpu.GetName(),
        /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
        /* uri */       profiler->URI()
    ));
  }
  
  http_server.AddJSAction(
    unisim::service::interfaces::ToolbarOpenTabAction(
      /* name */      web_terminal0.GetName(),
      /* label */     "<img src=\"/unisim/service/web_terminal/icon_term0.svg\" alt=\"TERM0\">",
      /* tips */      web_terminal0["title"],
      /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
      /* uri */       web_terminal0.URI()
  ));

  http_server.AddJSAction(
    unisim::service::interfaces::ToolbarOpenTabAction(
      /* name */      web_terminal1.GetName(),
      /* label */     "<img src=\"/unisim/service/web_terminal/icon_term1.svg\" alt=\"TERM1\">",
      /* tips */      web_terminal1["title"],
      /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
      /* uri */       web_terminal1.URI()
  ));
  return true;
}

void
Simulator::DefaultConfiguration(unisim::kernel::Simulator *sim)
{
  new unisim::kernel::config::xml::XMLConfigFileHelper(sim);
  new unisim::kernel::config::ini::INIConfigFileHelper(sim);
  new unisim::kernel::config::json::JSONConfigFileHelper(sim);

  // meta information
  sim->SetVariable("program-name", "UNISIM Zynq7000");
  sim->SetVariable("copyright", "Copyright (C) 2017, Commissariat a l'Energie Atomique (CEA)");
  sim->SetVariable("license", "BSD (see file COPYING)");
  sim->SetVariable("authors", "Yves Lhuillier <yves.lhuillier@cea.fr>");
  sim->SetVariable("version", VERSION);
  sim->SetVariable("description", "UNISIM Zynq7000, Zynq7000 full system simulator.");

  //=========================================================================
  //===                     Component run-time configuration              ===
  //=========================================================================
  sim->SetVariable( "kernel_logger.std_err", true );
  sim->SetVariable( "kernel_logger.std_err_color", true );
  
  sim->SetVariable( "HARDWARE.router.cycle_time",        "10 ns" );
  sim->SetVariable( "HARDWARE.CLK.clock-period", "10 ns");
  sim->SetVariable( "HARDWARE.CLK.lazy-clock", true);
  
  sim->SetVariable( "HARDWARE.cpu.SCTLR",                0x00c52078 );
  sim->SetVariable( "HARDWARE.cpu.default-endianness",   "little-endian" );
  sim->SetVariable( "HARDWARE.cpu.cpu-cycle-time",       "1.5 ns" ); // 666Mhz
  sim->SetVariable( "HARDWARE.cpu.bus-cycle-time",       "10 ns" ); // 32Mhz
  sim->SetVariable( "HARDWARE.cpu.icache.size",          0x020000 ); // 128 KB
  sim->SetVariable( "HARDWARE.cpu.dcache.size",          0x020000 ); // 128 KB
  sim->SetVariable( "HARDWARE.cpu.nice-time",            "1 us" ); // 1us
  sim->SetVariable( "HARDWARE.cpu.ipc",                  1.0  );
  sim->SetVariable( "HARDWARE.cpu.voltage",              1.8 * 1e3 ); // 1800 mV
  sim->SetVariable( "HARDWARE.cpu.enable-dmi",           true ); // Enable SystemC TLM 2.0 DMI
  sim->SetVariable( "HARDWARE.cpu.verbose",              true );
  sim->SetVariable( "HARDWARE.cpu.verbose-tlm",          false );
  sim->SetVariable( "HARDWARE.main_ram.bytesize",          0x40000000UL ); 
  sim->SetVariable( "HARDWARE.main_ram.cycle-time",        "10 ns" );
  sim->SetVariable( "HARDWARE.main_ram.read-latency",      "10 ns" );
  sim->SetVariable( "HARDWARE.main_ram.write-latency",     "0 ps" );
  sim->SetVariable( "HARDWARE.boot_rom.org",               0xffff0000UL );
  sim->SetVariable( "HARDWARE.boot_rom.bytesize",          0x00010000UL ); 
  sim->SetVariable( "HARDWARE.boot_rom.cycle-time",        "10 ns" );
  sim->SetVariable( "HARDWARE.boot_rom.read-latency",      "10 ns" );
  sim->SetVariable( "HARDWARE.boot_rom.write-latency",     "0 ps" );
  sim->SetVariable( "loader.memory-mapper.mapping", "HARDWARE.main_ram:0x00000000-0x3fffffff,HARDWARE.boot_rom:0xffff0000-0xffffffff:+0xffff0000" );
  
  
  sim->SetVariable( "gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_arm_with_neon.xml" );
  sim->SetVariable( "debugger.parse-dwarf", false );
  sim->SetVariable( "debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/arm_eabi_dwarf_register_number_mapping.xml" );

  sim->SetVariable( "inline-debugger.num-loaders", 1 );
  sim->SetVariable( "inline-debugger.search-path", "" );
  
  sim->SetVariable("http-server.http-port", 12360);
  
  sim->SetVariable("web-terminal0.title", "Serial Terminal over uart0");
  sim->SetVariable("web-terminal1.title", "Serial Terminal over uart1");
}

void Simulator::SigInt()
{
  if(!enable_inline_debugger)
    {
      unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
    }
}

CPU::CP15Reg&
CPU::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CTR, Cache Type Register"; }
          uint32_t Read( CP15CPU& _cpu )
          {
            /*        FORMAT          CWG         ERG      DminLine        L1Ip       IminLine */
            /*         ARMv7        8 words     8 words     8 words        PIPT        8 words */
            return (0b100 << 29) | (3 << 24) | (3 << 20) | (3 << 16) | (0b11 << 14) | (3 <<  0);
          }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
          uint32_t Read( CP15CPU& _cpu )
          {
            /* 2 L1 caches of 32kB: 32bytes/8words-lines, 128 sets of 4 ways ) */
            CPU& cpu = static_cast<CPU&>( _cpu );
            switch (cpu.csselr) {
              /*              LNSZ      ASSOC       NUMSETS        POLICY      */
            case 0:  return (1 << 0) | (1 << 3) | ( 127 << 13) | (0b0110 << 28); /* L1 dcache */
            case 1:  return (1 << 0) | (3 << 3) | ( 127 << 13) | (0b0100 << 28); /* L1 icache */
            case 2:  return (2 << 0) | (7 << 3) | (1023 << 13) | (0b0110 << 28); /* L2 ucache */
            default: return 0;
            }
          }
        } x;
        return x;
      } break;
      
    // case CP15ENCODE( 0, 1, 0, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "CLIDR, Cache Level ID Register"; }
    //       uint32_t Read( CP15CPU& _cpu )
    //       {
    //         //CPU& cpu = static_cast<CPU&>( _cpu );
    //         uint32_t
    //           LoUU =   0b010, /* Level of Unification Uniprocessor  */
    //           LoC =    0b010, /* Level of Coherency */
    //           LoUIS =  0b010, /* Level of Unification Inner Shareable */
    //           Ctype2 = 0b100, /* Level2 => unified */
    //           Ctype1 = 0b011; /* Level1 => Separate instruction and data caches */
    //         return (LoUU << 27) | (LoC << 24) | (LoUIS << 21) | (Ctype2 << 3) | (Ctype1 << 0);
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 0, 2, 0, 0 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         static_cast<CPU&>( _cpu ).csselr = value;
    //       }
    //       uint32_t Read( CP15CPU& _cpu ) { return static_cast<CPU&>( _cpu ).csselr; }
    //     } x;
    //     return x;
    //   } break;
      
    //   /***************************************************************
    //    * Cache maintenance, address translation, and other functions *
    //    ***************************************************************/
    // case CP15ENCODE( 7, 0, 5, 0 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No cache, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "ICIALLU <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 7, 0, 5, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "ICIMVAU, Clean data* cache line by MVA to PoU"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No cache, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "ICIMVAU <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 7, 0, 5, 6 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "BPIALL, Invalidate all branch predictors"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No branch predictor, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "BPIALL <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 7, 0, 10, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "DCCMVAC, Clean data* cache line by MVA to PoC"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No cache, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "DCCMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 7, 0, 11, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "DCCMVAU, Clean data* cache line by MVA to PoU"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No cache, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "DCCMVAU <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 7, 0, 14, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "DCCIMVAC, Clean and invalidate data cache line by MVA to PoC"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No cache, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "DCCIMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 7, 0, 14, 2 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "DCCISW, Clean and invalidate [d|u]cache line by set/way"; }
    //       void Write( CP15CPU& _cpu, uint32_t value ) {
    //         /* No cache, basically nothing to do */
    //         //_cpu.logger << DebugWarning << "DCCISW <- " << std::hex << value << std::dec << EndDebugWarning;
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    //   /******************************
    //    * TLB maintenance operations *
    //    ******************************/
    // case CP15ENCODE( 8, 0, 7, 0 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
    //       void Write( CP15CPU& _cpu, uint32_t value )
    //       {
    //         CPU& cpu( static_cast<CPU&>( _cpu ) );
    //         if (cpu.verbose)
    //           cpu.logger << DebugInfo << "TLBIALL" << EndDebugInfo;
    //         cpu.tlb.InvalidateAll();
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 8, 0, 7, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "TLBIMVA, invalidate unified TLB entry by MVA and ASID"; }
    //       void Write( CP15CPU& _cpu, uint32_t value )
    //       {
    //         CPU& cpu( static_cast<CPU&>( _cpu ) );
    //         if (cpu.verbose)
    //           cpu.logger << DebugInfo << "TLBIMVA(0x" << std::hex << value << std::dec << ")" << EndDebugInfo;
            
    //         for (TLB::Iterator itr(cpu.tlb); itr.Next();) {
    //           if (itr.MatchMVA( value ) and (itr.IsGlobal() or itr.MatchASID( value )))
    //             itr.Invalidate();
    //         }   
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 8, 0, 7, 2 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "TLBIASID, invalidate unified TLB by ASID match"; }
    //       void Write( CP15CPU& _cpu, uint32_t value )
    //       {
    //         CPU& cpu( static_cast<CPU&>( _cpu ) );
    //         if (cpu.verbose)
    //           cpu.logger << DebugInfo << "TLBIASID(0x" << std::hex << value << std::dec << ")" << EndDebugInfo;
            
    //         for (TLB::Iterator itr(cpu.tlb); itr.Next();) {
    //           if (not itr.IsGlobal() and itr.MatchASID( value ))
    //             itr.Invalidate();
    //         }
    //       }
    //     } x;
    //     return x;
    //   } break;
      
    //   /**********************************************
    //    * Memory remapping and TLB control registers *
    //    **********************************************/
    // case CP15ENCODE( 10, 0, 2, 0 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "PRRR, Primary Region Remap Register"; }
    //       void Write( CP15CPU& _cpu, uint32_t value )
    //       {
    //         CPU& cpu( static_cast<CPU&>( _cpu ) );
    //         cpu.mmu.prrr = value;
    //         cpu.mmu.refresh_attr_cache(sctlr::TRE.Get(cpu.SCTLR));
    //       }
    //       uint32_t Read( CP15CPU& _cpu ) { return static_cast<CPU&>( _cpu ).mmu.prrr; }
    //     } x;
    //     return x;
    //   } break;
      
    // case CP15ENCODE( 10, 0, 2, 1 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "NMRR, Normal Memory Remap Register"; }
    //       void Write( CP15CPU& _cpu, uint32_t value )
    //       {
    //         CPU& cpu( static_cast<CPU&>( _cpu ) );
    //         cpu.mmu.nmrr = value;
    //         cpu.mmu.refresh_attr_cache(sctlr::TRE.Get(cpu.SCTLR));
    //       }
    //       uint32_t Read( CP15CPU& _cpu ) { return static_cast<CPU&>( _cpu ).mmu.nmrr; }
    //     } x;
    //     return x;
    //   } break;
      
    //   /***********************************/
    //   /* Context and thread ID registers */
    //   /***********************************/
      
    // case CP15ENCODE( 13, 0, 0, 3 ):
    //   {
    //     static struct : public CP15Reg
    //     {
    //       char const* Describe() { return "TPIDRURO, User Read-Only Thread ID Register"; }
    //       unsigned RequiredPL() { return 0; /* Reading doesn't requires priviledges */ }
    //       uint32_t Read( CP15CPU& _cpu )
    //       { return static_cast<CPU&>( _cpu ).MemRead32( 0xffff0ff0 ); }
    //     } x;
    //     /* When using linux os emulation, this register overrides the base one */
    //     if (linux_os_import)
    //       return x;
    //   } break;
    }
  
  // Fall back to parent cpu CP15 registers
  return this->PCPU::CP15GetRegister( crn, opcode1, crm, opcode2 );
}
