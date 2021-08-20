/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_BOOK_E_CPU_HH__
#define __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_BOOK_E_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/book_e/cpu.hh>
#include <systemc>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <inttypes.h>
#include <unisim/component/tlm2/interrupt/types.hh>
#include <stack>
#include <vector>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace powerpc {
namespace book_e {

using unisim::kernel::tlm2::PayloadFabric;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::logger::Logger;
using unisim::component::tlm2::interrupt::InterruptProtocolTypes;
using unisim::component::tlm2::interrupt::InterruptPayload;

template <typename TYPES, typename CONFIG>
class CPU
	: public sc_core::sc_module
	, public CONFIG::CPU
{
public:
	
	typedef typename CONFIG::CPU Super;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	typedef typename Super::MachineCheckInterrupt MachineCheckInterrupt;
	typedef tlm::tlm_initiator_socket<CONFIG::FSB_WIDTH * 8> plb_master_socket;
	typedef tlm::tlm_target_socket<0, InterruptProtocolTypes> irq_slave_socket;
	typedef tlm::tlm_initiator_socket<4> dcr_master_socket;

	plb_master_socket icurd_plb_master_sock; // Instruction Cache Unit Read PLB interface
	plb_master_socket dcuwr_plb_master_sock; // Data Cache Unit Write PLB interface
	plb_master_socket dcurd_plb_master_sock; // Data Cache Unit Read PLB interface
	irq_slave_socket external_input_interrupt_slave_sock; // External Input
	irq_slave_socket critical_input_interrupt_slave_sock; // Critical Input
	dcr_master_socket dcr_master_sock; // DCR master interface
	
	CPU(const sc_core::sc_module_name& name, unisim::kernel::Object *parent = 0);
	virtual ~CPU();
	
	virtual bool EndSetup();

	// Back path
	virtual tlm::tlm_sync_enum nb_transport_bw(unsigned int if_id, tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	virtual void invalidate_direct_mem_ptr(unsigned int if_id, sc_dt::uint64 start_range, sc_dt::uint64 end_range);
	
	// Forward path
	void interrupt_b_transport(unsigned int irq, InterruptPayload& payload, sc_core::sc_time& t);
	tlm::tlm_sync_enum interrupt_nb_transport_fw(unsigned int irq, InterruptPayload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
	unsigned int interrupt_transport_dbg(unsigned int, InterruptPayload& payload);
	bool interrupt_get_direct_mem_ptr(unsigned int, InterruptPayload& payload, tlm::tlm_dmi& dmi_data);

	virtual void Synchronize();
	virtual void Idle();
	virtual void Halt();
	
	void Run();
	
protected:
	sc_core::sc_time GetBurstLatency(uint32_t size, const sc_core::sc_time& latency) const;
	virtual bool DataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual bool DataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool InstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugDataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugDataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugInstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual void DCRRead(unsigned int dcrn, uint32_t& value);
	virtual void DCRWrite(unsigned int dcrn, uint32_t value);
private:
	typedef enum
	{
		IF_ICURD_PLB,
		IF_DCUWR_PLB,
		IF_DCURD_PLB,
		IF_DCR
	} Interface;
	
	PayloadFabric<tlm::tlm_generic_payload> payload_fabric;
	sc_core::sc_time cpu_cycle_time;         //<! CPU core cycle time
	sc_core::sc_time bus_cycle_time;         //<! Bus (PLB) cycle time
	sc_core::sc_time ext_timer_cycle_time;   //<! external timer cycle time
	sc_core::sc_time cpu_time;               //<! local time (relative to sc_time_stamp)
	sc_core::sc_time timer_time;             //<! absolute time from the internal timers point of view
	sc_core::sc_time nice_time;              //<! period of synchronization with other threads
	sc_core::sc_time max_idle_time;          //<! Maximum idle time (temporary variable)
	sc_core::sc_time run_time;               //<! absolute timer (local time + sc_time_stamp)
	sc_core::sc_time idle_time;              //<! total idle time
	sc_core::sc_time timers_update_deadline; //<! deadline for updating internal timers in order to keep internal timer accuracy
	bool enable_host_idle;
	sc_core::sc_event ev_max_idle;
	sc_core::sc_event ev_irq;
	double ipc;
	double one;
	bool enable_dmi;
	bool debug_dmi;

	unisim::kernel::variable::Parameter<sc_core::sc_time> param_cpu_cycle_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_bus_cycle_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_ext_timer_cycle_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_nice_time;
	unisim::kernel::variable::Parameter<double> param_ipc;
	unisim::kernel::variable::Parameter<bool> param_enable_host_idle;
	unisim::kernel::variable::Parameter<bool> param_enable_dmi;
	unisim::kernel::variable::Parameter<bool> param_debug_dmi;
	unisim::kernel::variable::Statistic<double> stat_one;
	unisim::kernel::variable::Statistic<sc_core::sc_time> stat_run_time;
	unisim::kernel::variable::Statistic<sc_core::sc_time> stat_idle_time;
	unisim::kernel::variable::StatisticFormula<double> stat_idle_rate;
	unisim::kernel::variable::StatisticFormula<double> stat_load_rate;
	
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
			IRQ_CRITICAL_INPUT = 0,
			IRQ_EXTERNAL_INPUT = 1,
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
		
		bool Empty() const
		{
			return schedule.empty();
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
	unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *critical_input_interrupt_redirector;
	unisim::kernel::tlm2::FwRedirector<CPU<TYPES, CONFIG>, unisim::kernel::tlm2::SimpleProtocolTypes<bool> > *external_input_interrupt_redirector;
	unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> > *icurd_plb_redirector;
	unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> > *dcuwr_plb_redirector;
	unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> > *dcurd_plb_redirector;
	unisim::kernel::tlm2::BwRedirector<CPU<TYPES, CONFIG> > *dcr_redirector;
	unisim::kernel::tlm2::DMIRegionCache icurd_dmi_region_cache;
	unisim::kernel::tlm2::DMIRegionCache dcuwr_dmi_region_cache;
	unisim::kernel::tlm2::DMIRegionCache dcurd_dmi_region_cache;

	inline void AlignToBusClock() ALWAYS_INLINE;
	void AlignToBusClock(sc_core::sc_time& t);
	inline void ProcessExternalEvents() ALWAYS_INLINE;
	void ProcessIRQEvent(Event *event);
protected:
	virtual void RunInternalTimers();
	inline void LazyRunInternalTimers() ALWAYS_INLINE;
	virtual void end_of_simulation();
};

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_PROCESSOR_POWERPC_BOOK_E_PPC440_CPU_HH__
