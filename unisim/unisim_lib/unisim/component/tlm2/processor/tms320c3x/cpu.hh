/*
 *  Copyright (c) 2007-2010,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_TMS320C3X_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_TMS320C3X_CPU_HH__

#include <unisim/component/cxx/processor/tms320c3x/cpu.hh>
#include <systemc>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <tlm_utils/tlm_quantumkeeper.h>
#include <inttypes.h>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace tms320c3x {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::logger::Logger;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::InterruptPayload;

template <class CONFIG, bool DEBUG = false>
class CPU
	: public sc_core::sc_module
	, public unisim::component::cxx::processor::tms320c3x::CPU<CONFIG, DEBUG>
{
public:
	
	typedef unisim::component::cxx::processor::tms320c3x::CPU<CONFIG, DEBUG> inherited;
	typedef tlm::tlm_initiator_socket<32> bus_master_socket;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;

	bus_master_socket bus_master_sock;     // CPU Bus Master socket
	irq_slave_socket int0_slave_sock;  // INT0(CPU) Interrupt Slave Sock
	irq_slave_socket int1_slave_sock;  // INT1(CPU) Interrupt Slave Sock
	irq_slave_socket int2_slave_sock;  // INT2(CPU) Interrupt Slave Sock
	irq_slave_socket int3_slave_sock;  // INT3(CPU) Interrupt Slave Sock
	irq_slave_socket xint0_slave_sock; // XINT0(CPU) Interrupt Slave Sock
	irq_slave_socket rint0_slave_sock; // RINT0(CPU) Interrupt Slave Sock
	irq_slave_socket xint1_slave_sock; // XINT1(CPU) Interrupt Slave Sock
	irq_slave_socket rint1_slave_sock; // RINT1(CPU) Interrupt Slave Sock
	irq_slave_socket tint0_slave_sock; // TINT0(CPU) Interrupt Slave Sock
	irq_slave_socket tint1_slave_sock; // TINT1(CPU) Interrupt Slave Sock
	irq_slave_socket dint_slave_sock;  // DINT(CPU) Interrupt Slave Sock
	
	CPU(const sc_core::sc_module_name& name, Object *parent = 0);
	virtual ~CPU();
	
	// Back path
	virtual tlm::tlm_sync_enum nb_transport_bw(unsigned int if_id, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(unsigned int if_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Forward path
	void interrupt_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t);
	tlm::tlm_sync_enum interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int interrupt_transport_dbg(unsigned int, InterruptPayload& payload);
	bool interrupt_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data);

	virtual void Synchronize();
	
	void Run();
	
protected:
	virtual bool PrWrite(typename CONFIG::address_t addr, const void *buffer, uint32_t size, bool interlocked = false);
	virtual bool PrRead(typename CONFIG::address_t addr, void *buffer, uint32_t size, bool interlocked = false);
private:
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	sc_core::sc_time cpu_cycle_time;
	sc_core::sc_time cpu_time;
	sc_core::sc_time nice_time;
	sc_core::sc_event ev_irq;
	double ipc;
	bool enable_dmi;
	bool debug_dmi;

	Parameter<sc_core::sc_time> param_cpu_cycle_time;
	Parameter<sc_core::sc_time> param_nice_time;
	Parameter<double> param_ipc;
	Parameter<bool> param_enable_dmi;
	Parameter<bool> param_debug_dmi;
	
	class Event
	{
	public:
		typedef enum
		{
			// In order of priority
			EV_IRQ,               // an IRQ event
		} Type;
		
		typedef enum
		{
			IRQ_INT0 = 0,
			IRQ_INT1 = 1,
			IRQ_INT2 = 2,
			IRQ_INT3 = 3,
			IRQ_XINT0 = 4,
			IRQ_RINT0 = 5,
			IRQ_XINT1 = 6,
			IRQ_RINT1 = 7,
			IRQ_TINT0 = 8,
			IRQ_TINT1 = 9,
			IRQ_DINT = 10
		} IRQ;
		
		Event()
			: type(EV_IRQ)
			, time_stamp(sc_core::SC_ZERO_TIME)
			, irq(0)
			, level(false)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void InitializeIRQEvent(unsigned int _irq, bool _level, const sc_core::sc_time& _time_stamp)
		{
			type = EV_IRQ;
			time_stamp = _time_stamp;
			irq = _irq;
			level = _level;
		}

		void Clear()
		{
			type = EV_IRQ;
			time_stamp = sc_core::SC_ZERO_TIME;
			irq = 0;
			level = false;
		}
		
		Type GetType() const
		{
			return type;
		}
		
		void SetTimeStamp(const sc_core::sc_time& _time_stamp)
		{
			time_stamp = _time_stamp;
		}
		
		const sc_core::sc_time& GetTimeStamp() const
		{
			return time_stamp;
		}
		
		unsigned int GetIRQ() const
		{
			return irq;
		}
		
		bool GetLevel() const
		{
			return level;
		}
		
	private:
		Type type;
		sc_core::sc_time time_stamp;
		unsigned int irq;
		bool level;
	};
	
	class EventAllocator
	{
	public:
		EventAllocator()
			: free_list()
		{
		}
		
		~EventAllocator()
		{
			while(!free_list.empty())
			{
				Event *event = free_list.top();
				delete event;
				free_list.pop();
			}
		}
		
		Event *AllocEvent()
		{
			if(!free_list.empty())
			{
				Event *event = free_list.top();
				free_list.pop();
				return event;
			}
			
			return new Event();
		}
		
		void FreeEvent(Event *event)
		{
			event->Clear();
			free_list.push(event);
		}
	private:
		std::stack<Event *, std::vector<Event *> > free_list;
	};
	
	class ScheduleKey
	{
	public:
		ScheduleKey(const sc_core::sc_time& _time_stamp, typename Event::Type _type, unsigned int _irq)
			: time_stamp(_time_stamp)
			, type(_type)
			, irq(_irq)
		{
		}
		
		int operator < (const ScheduleKey& sk) const
		{
			return (time_stamp < sk.time_stamp) || ((time_stamp == sk.time_stamp) && ((type < sk.type) || ((type == sk.type) && (irq < sk.irq))));
		}
		
		const sc_core::sc_time& GetTimeStamp() const
		{
			return time_stamp;
		}
	private:
		sc_core::sc_time time_stamp;
		typename Event::Type type;
		unsigned int irq;
	};
	
	class Schedule
	{
	public:
		Schedule()
			: schedule()
			, kernel_event()
			, event_allocator()
		{
		}
		
		~Schedule()
		{
			typename std::multimap<ScheduleKey, Event *>::iterator it;
			for(it = schedule.begin(); it != schedule.end(); it++)
			{
				Event *event = (*it).second;
				
				event_allocator.FreeEvent(event);
			}
		}
		
		void NotifyIRQEvent(unsigned int irq, bool level, const sc_core::sc_time& time_stamp)
		{
			ScheduleKey key = ScheduleKey(time_stamp, Event::EV_IRQ, irq);
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.find(key);
			if(it != schedule.end())
			{
				if((*it).second->GetLevel() == level) return; // Already scheduled
			}
			Event *event = event_allocator.AllocEvent();
			event->InitializeIRQEvent(irq, level, time_stamp);
			schedule.insert(std::pair<ScheduleKey, Event *>(key, event));
			sc_core::sc_time t(time_stamp);
			t -= sc_core::sc_time_stamp();
			kernel_event.notify(t);
		}

		void Notify(Event *event)
		{
			const sc_core::sc_time& time_stamp = event->GetTimeStamp();
			schedule.insert(std::pair<ScheduleKey, Event *>(ScheduleKey(time_stamp, event->GetType(), event->GetIRQ()), event));
			sc_core::sc_time t(time_stamp);
			t -= sc_core::sc_time_stamp();
			kernel_event.notify(t);
		}
		
		Event *GetNextEvent(const sc_core::sc_time& time_stamp)
		{
			if(schedule.empty()) return 0;
			
			typename std::multimap<ScheduleKey, Event *>::iterator it = schedule.begin();
			const sc_core::sc_time& event_time_stamp = (*it).first.GetTimeStamp();
			if(event_time_stamp <= time_stamp)
			{
				Event *event = (*it).second;
				schedule.erase(it);
				return event;
			}
			
			sc_core::sc_time t(event_time_stamp);
			t -= time_stamp;
			kernel_event.notify(t);
			
			return 0;
		}
		
		void FreeEvent(Event *event)
		{
			event_allocator.FreeEvent(event);
		}
		
		const sc_core::sc_event& GetKernelEvent() const
		{
			return kernel_event;
		}
		
		void Flush(const sc_core::sc_time& time_stamp, typename Event::Type event_type)
		{
			typename std::multimap<ScheduleKey, Event *>::iterator it;
			for(it = schedule.begin(); it != schedule.end(); it++)
			{
				Event *event = (*it).second;
				
				if(event->GetType() == event_type)
				{
					event_allocator.FreeEvent(event);
					schedule.erase(it);
				}
			}
			kernel_event.cancel();

			if(schedule.empty()) return;
			
			it = schedule.begin();
			const sc_core::sc_time& event_time_stamp = (*it).first.GetTimeStamp();
			if(event_time_stamp <= time_stamp)
			{
				return;
			}
			
			sc_core::sc_time t(event_time_stamp);
			t -= time_stamp;
			kernel_event.notify(t);
		}

		void Clear()
		{
			typename std::multimap<ScheduleKey, Event *>::iterator it;
			for(it = schedule.begin(); it != schedule.end(); it++)
			{
				Event *event = (*it).second;
				
				event_allocator.FreeEvent(event);
			}
			schedule.clear();
			kernel_event.cancel();
		}

	private:
		std::multimap<ScheduleKey, Event *> schedule;
		
		sc_core::sc_event kernel_event;
		EventAllocator event_allocator;
	};

	Schedule external_event_schedule;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *int0_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *int1_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *int2_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *int3_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *xint0_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *rint0_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *xint1_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *rint1_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *tint0_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *tint1_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<CONFIG, DEBUG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *dint_redirector;
	unisim::kernel::tlm2::BwRedirector<CPU<CONFIG, DEBUG> > *bus_master_redirector;
	unisim::kernel::tlm2::DMIRegionCache dmi_region_cache;
	unisim::kernel::tlm2::LatencyLookupTable lat_lut;
	inline void UpdateTime();
	void ProcessExternalEvents();
	void ProcessIRQEvent(Event *event);
};

} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif
