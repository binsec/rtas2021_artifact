#ifndef __GENERIC_TIMER_H__
#define __GENERIC_TIMER_H__

#include <systemc>
#include <tlm>
#include <scml2.h>
#include <scmlinc/scml_property.h>
#include <string>
#include <stdexcept>

#if (defined(WIN32) || defined(WIN64)) && !defined(__DEFINED_UINT32_T__)
typedef unsigned int uint32_t;
#define __DEFINED_UINT32_T__
#endif

template<unsigned int BUSWIDTH = 32>
class GenericTimer
	: public sc_core::sc_module
{
public:
	tlm::tlm_target_socket<BUSWIDTH> CTRL; // Interface port to access the timer registers.
	sc_core::sc_in<bool> ENABLE;           // This is the hardware enable pin for the timer.
	sc_core::sc_in<bool> CLK;              // Clock input pin for the timer.
	sc_core::sc_out<bool> IRQ;             // A configurable interrupt will be generated at this port whenever the timer reaches the count value in up counter mode, or zero when in down counter mode.
	sc_core::sc_in<bool> RST;              // Reset port. When a reset occurs on this pin, all the timer registers are reset to their default value.

	GenericTimer(const sc_core::sc_module_name& name);
	virtual ~GenericTimer();

protected:
    scml_property<std::string> irq_width_unit;
    scml_property<bool> rst_level;
    scml_property<double> irq_width;
    scml_property<bool> irq_level;

	scml2::stream debug_log;    // Stream used for logging debug messages.
	scml2::stream warning_log;  // Stream used for logging warning messages.
	scml2::stream error_log;    // Stream used for logging fatal error messages.
	scml2::stream sw_error_log; // Stream used for logging software error messages.

	void Reset();               // This method resets all interfaces, registers, and bitfields to their reset value.
	                            // It is typically called from end_of_elaboration(), and connected to the reset port.
	
	void ResetTimeAccounting(); // This method resets time accounting. Should be called when either ENABLE toggle from 0 to 1, or when EnableBit toggle from 0 to 1
	
private:
	// Timer registers/bits mapping and reset values
	static const unsigned int MODE_REG     = 0x00;
	static const unsigned int MODE_REG_RUN_MODE_BIT   = 0;
	static const unsigned int MODE_REG_COUNT_MODE_BIT = 1;
	
	static const unsigned int ENABLE_REG   = 0x04;
	static const unsigned int ENABLE_REG_ENABLE_BIT   = 0;
	
	static const unsigned int CLEAR_REG    = 0x08;
	static const unsigned int LOAD_REG     = 0x0c;
	static const unsigned int VALUE_REG    = 0x10;
	static const unsigned int PRESCALE_REG = 0x14;
	static const unsigned int REGISTER_BANK_SIZE = PRESCALE_REG + 4;

	static const uint32_t MODE_REG_RESET_VALUE     = 0;
	static const uint32_t ENABLE_REG_RESET_VALUE   = 0; 
	static const uint32_t CLEAR_REG_RESET_VALUE    = 0; 
	static const uint32_t LOAD_REG_RESET_VALUE     = 0x1; 
	static const uint32_t VALUE_REG_RESET_VALUE    = 0; 
	static const uint32_t PRESCALE_REG_RESET_VALUE = 0x1; 

	/////////////////////////////////////// Timer registers /////////////////////////////////////
	scml2::memory<uint32_t> RegisterBank;
	
	// - ModeReg
	struct ModeRegType : public scml2::reg<uint32_t>
	{
		scml2::bitfield<uint32_t> CountModeBit;
		scml2::bitfield<uint32_t> RunModeBit;

		ModeRegType(const std::string& name, scml2::memory<uint32_t>& memory, sc_dt::uint64 offset);
		using scml2::reg<uint32_t>::operator =;
	};

	ModeRegType ModeReg; // This register is used for configuring the timer.
	                     // The value written to bit 0 and bit 1 decides the mode of the timer.
	                      

	// - EnableReg
	struct EnableRegType : public scml2::reg<uint32_t>
	{
		scml2::bitfield<uint32_t> EnableBit;
		
		EnableRegType(const std::string& name, scml2::memory<uint32_t>& memory, sc_dt::uint64 offset);
		using scml2::reg<uint32_t>::operator =;
	};

	EnableRegType EnableReg; // The value written to bit 0 of this register starts/stops the timer.

	// - ClearReg
	scml2::reg<uint32_t> ClearReg; // Any value written to this register clears the IRQ pin of the timer.

	// - LoadReg
	scml2::reg<uint32_t> LoadReg; // This register holds the count value.

	// - ValueReg
	scml2::reg<uint32_t> ValueReg; // Reading this register gives you the current value of the counter.

	// - PrescaleReg
	scml2::reg<uint32_t> PrescaleReg; // Value written to this register decides the period multiplier for the internal gated clock.

	/////////////////////////////////////////////////////////////////////////////////////////////

	// TLM2 GP adapter
	scml2::tlm2_gp_target_adapter<BUSWIDTH>* CTRL_adapter;

	// Clock gate
	scml_clock_gate clock_gate;
	
	// Timer internal stuff
	sc_core::sc_time last_state_update_time;
	sc_core::sc_event update_state_event;
	sc_core::sc_event generate_output_event;
	sc_core::sc_time irq_time;
	bool enable;
	bool rst;
	bool clear;
	bool irq;

	// Call backs for TLM2 GP adapter
	bool WriteModeReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time);
	bool WriteEnableReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time);
	bool WriteClearReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time);
	bool WriteLoadReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time);
	bool ReadValueReg(uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time);
	bool WritePrescaleReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time);

	// Timer internal stuff
	sc_core::sc_time_unit StringToTimeUnit(const std::string& s) const;
	void IncrementValueReg(sc_dt::uint64 delta);
	void DecrementValueReg(sc_dt::uint64 delta);
	sc_core::sc_time TimeToNextTick();
	sc_core::sc_time TimeToNextStateUpdate();
	void UpdateState();
	void DoTimerTick();
	
	// Timer SystemC processes (behavior)
	void UpdateStateProcess();
	void GenerateOutputProcess();
};

////////////////////////////////////////////// GenericTimer<> //////////////////////////////////////////////////////////

template<unsigned int BUSWIDTH>
GenericTimer<BUSWIDTH>::GenericTimer(const sc_core::sc_module_name& name)
	: sc_core::sc_module(name)
	, CTRL("CTRL")
	, ENABLE("ENABLE")
	, CLK("CLK")
	, IRQ("IRQ")
	, RST("RST")
	, irq_width_unit("irq_width_unit", "SC_NS")
	, rst_level("rst_level", true)
	, irq_width("irq_width", 0.0)
	, irq_level("irq_level", true)
	, debug_log(scml2::severity::debug())
	, warning_log(scml2::severity::warning())
	, error_log(scml2::severity::error())
	, sw_error_log(scml2::severity("software error", 100))
	, RegisterBank("RegisterBank", REGISTER_BANK_SIZE / scml2::sizeOf<uint32_t>())
	, ModeReg(sc_core::sc_gen_unique_name("ModeReg", true), RegisterBank, MODE_REG / scml2::sizeOf<uint32_t>())
	, EnableReg(sc_core::sc_gen_unique_name("EnableReg", true), RegisterBank, ENABLE_REG / scml2::sizeOf<uint32_t>())
	, ClearReg(sc_core::sc_gen_unique_name("ClearReg", true), RegisterBank, CLEAR_REG / scml2::sizeOf<uint32_t>())
	, LoadReg(sc_core::sc_gen_unique_name("LoadReg", true), RegisterBank, LOAD_REG / scml2::sizeOf<uint32_t>())
	, ValueReg(sc_core::sc_gen_unique_name("ValueReg", true), RegisterBank, VALUE_REG / scml2::sizeOf<uint32_t>())
	, PrescaleReg(sc_core::sc_gen_unique_name("PrescaleReg", true), RegisterBank, PRESCALE_REG / scml2::sizeOf<uint32_t>())
	, CTRL_adapter(0)
	, clock_gate("clock_gate")
	, last_state_update_time(sc_core::SC_ZERO_TIME)
	, update_state_event("update_state_event")
	, generate_output_event("generate_output_event")
	, irq_time(sc_core::SC_ZERO_TIME)
	, enable(false)
	, rst(false)
	, clear(false)
{
	irq = !irq_level;
	SC_HAS_PROCESS(GenericTimer);
	
	scml2::set_write_callback<uint32_t, scml2::reg>(ModeReg, SCML2_CALLBACK(WriteModeReg), scml2::AUTO_SYNCING);
	ModeReg.set_description("This register is used for configuring the timer. The value written to bit 0 and bit 1 decides the mode of the timer.");
	
	scml2::set_write_callback<uint32_t, scml2::reg>(EnableReg, SCML2_CALLBACK(WriteEnableReg), scml2::AUTO_SYNCING);
	EnableReg.set_description("The value written to bit 0 of this register starts/stops the timer.");
	
	scml2::set_write_callback<uint32_t, scml2::reg>(ClearReg, SCML2_CALLBACK(WriteClearReg), scml2::AUTO_SYNCING);
	ClearReg.set_description("Any value written to this register clears the IRQ pin of the timer.");
	
	scml2::set_write_callback<uint32_t, scml2::reg>(LoadReg, SCML2_CALLBACK(WriteLoadReg), scml2::AUTO_SYNCING);
	LoadReg.set_description("This register holds the count value.");
	
	scml2::set_read_callback<uint32_t, scml2::reg>(ValueReg, SCML2_CALLBACK(ReadValueReg), scml2::AUTO_SYNCING);
	scml2::set_ignore_write_access<uint32_t, scml2::reg>(ValueReg);
	
	ValueReg.set_description("Reading this register gives you the current value of the counter.");
	scml2::set_write_callback<uint32_t, scml2::reg>(PrescaleReg, SCML2_CALLBACK(WritePrescaleReg), scml2::AUTO_SYNCING);
	
	PrescaleReg.set_description("Value written to this register decides the period multiplier for the internal gated clock.");
	
	// initialize adapter
	CTRL_adapter = new scml2::tlm2_gp_target_adapter<BUSWIDTH>("CTRL_adapter", CTRL);
	
	// bind TLM2 GP adapter to memory
	(*CTRL_adapter)(RegisterBank);

	scml2::set_posedge_callback(RST, SCML2_CALLBACK(Reset));
	scml2::set_posedge_callback(ENABLE, SCML2_CALLBACK(ResetTimeAccounting));

	clock_gate.clk(CLK);
	clock_gate.en(RST);

	SC_METHOD(UpdateStateProcess);
	sc_core::sc_module::sensitive << update_state_event << ENABLE;
	
	SC_METHOD(GenerateOutputProcess);
	sc_core::sc_module::sensitive << generate_output_event;
	
	Reset();
}

template<unsigned int BUSWIDTH>
GenericTimer<BUSWIDTH>::~GenericTimer()
{
	delete CTRL_adapter;
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::Reset()
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":Reset():" << std::endl;
	
	ModeReg = MODE_REG_RESET_VALUE;
	EnableReg = ENABLE_REG_RESET_VALUE;
	ClearReg = CLEAR_REG_RESET_VALUE;
	LoadReg = LOAD_REG_RESET_VALUE;
	PrescaleReg = PRESCALE_REG_RESET_VALUE;

	EnableReg.EnableBit = 0;
	ModeReg.CountModeBit = 0;
	ModeReg.RunModeBit = 0;
	
	enable = false;
	rst = false;
	clear = false;
	irq = !irq_level;
	generate_output_event.notify(sc_core::SC_ZERO_TIME);
	irq_time = sc_core::SC_ZERO_TIME;
	
	ResetTimeAccounting();
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::ResetTimeAccounting()
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":ResetTimeAccounting():" << std::endl;
	last_state_update_time = sc_time_stamp(); // accounting time may start now
}

template<unsigned int BUSWIDTH>
sc_core::sc_time_unit GenericTimer<BUSWIDTH>::StringToTimeUnit(const std::string& s) const
{
	if(s.compare("SC_NS") == 0) return sc_core::SC_NS;
	if(s.compare("SC_US") == 0) return sc_core::SC_US;
	if(s.compare("SC_MS") == 0) return sc_core::SC_MS;
	if(s.compare("SC_SEC") == 0) return sc_core::SC_SEC;
	if(s.compare("SC_PS") == 0) return sc_core::SC_PS;
	if(s.compare("SC_FS") == 0) return sc_core::SC_FS;
	return sc_core::SC_NS;
}

template<unsigned int BUSWIDTH>
bool GenericTimer<BUSWIDTH>::WriteModeReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time)
{
	assert(time == sc_core::SC_ZERO_TIME); // AUTO_SYNCING
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":WriteModeReg(...): ModeReg <- " << ModeReg;
	ModeReg = ((ModeReg & ~byteEnables) | (value & byteEnables));
	SCML2_LOG(debug_log) << " => " << ModeReg << std::endl;
	UpdateState();
	return true;
}

template<unsigned int BUSWIDTH>
bool GenericTimer<BUSWIDTH>::WriteEnableReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time)
{
	assert(time == sc_core::SC_ZERO_TIME); // AUTO_SYNCING
	uint32_t old_enable_bit = EnableReg.EnableBit;
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":WriteEnableReg(...): EnableReg <- " << EnableReg;
	EnableReg = ((EnableReg & ~byteEnables) | (value & byteEnables));
	if(ENABLE && !old_enable_bit && EnableReg.EnableBit) // EnableBit toggles from 0 to 1
	{
		ResetTimeAccounting();
	}
	SCML2_LOG(debug_log) << " => " << EnableReg << std::endl;
	UpdateState();
	return true;
}

template<unsigned int BUSWIDTH>
bool GenericTimer<BUSWIDTH>::WriteClearReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time)
{
	assert(time == sc_core::SC_ZERO_TIME); // AUTO_SYNCING
	ClearReg = ((ClearReg & ~byteEnables) | (value & byteEnables));
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":WriteClearReg(...): ClearReg <- " << ClearReg << std::endl;
	clear = true;
	UpdateState();
	return true;
}

template<unsigned int BUSWIDTH>
bool GenericTimer<BUSWIDTH>::WriteLoadReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time)
{
	assert(time == sc_core::SC_ZERO_TIME); // AUTO_SYNCING
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":WriteLoadReg(...): LoadReg <- " << LoadReg;
	LoadReg = ((LoadReg & ~byteEnables) | (value & byteEnables));
	SCML2_LOG(debug_log) << " => " << LoadReg;
	if(ModeReg.CountModeBit)
	{
		// count down
		ValueReg = LoadReg;
	}
	else
	{
		// count up
		ValueReg = 0;
	}
	SCML2_LOG(debug_log) << ", ValueReg <- " << ValueReg <<  std::endl;
	UpdateState();
	return true;
}

template<unsigned int BUSWIDTH>
bool GenericTimer<BUSWIDTH>::ReadValueReg(uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time)
{
	assert(time == sc_core::SC_ZERO_TIME); // AUTO_SYNCING
	UpdateState();
	value = ValueReg;
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":ReadValueReg(...): ValueReg -> " << ValueReg << std::endl;
	return true;
}

template<unsigned int BUSWIDTH>
bool GenericTimer<BUSWIDTH>::WritePrescaleReg(const uint32_t& value, const uint32_t& byteEnables, sc_core::sc_time& time)
{
	assert(time == sc_core::SC_ZERO_TIME); // AUTO_SYNCING
	PrescaleReg = ((PrescaleReg & ~byteEnables) | (value & byteEnables));
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":WritePrescaleReg(...): PrescaleReg <- " << PrescaleReg << std::endl;
	UpdateState();
	return true;
}
	
template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::IncrementValueReg(sc_dt::uint64 delta)
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":IncrementValueReg(" << delta << "):";
	if(ValueReg < LoadReg)
	{
		SCML2_LOG(debug_log) << "ValueReg <- " << ValueReg;
		assert((ValueReg + delta) <= LoadReg);
		
		ValueReg = ValueReg + delta;
		
		SCML2_LOG(debug_log) << " => " << ValueReg << std::endl;

		if(ValueReg == LoadReg)
		{
			DoTimerTick();
		}
	}
	else
	{
		SCML2_LOG(debug_log) << std::endl;
	}
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::DecrementValueReg(sc_dt::uint64 delta)
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":DecrementValueReg(" << delta << "):";
	if(ValueReg)
	{
		SCML2_LOG(debug_log) << "ValueReg <- " << ValueReg;
		assert(delta <= ValueReg);
		
		ValueReg = ValueReg - delta;
		
		SCML2_LOG(debug_log) << " => " << ValueReg << std::endl;

		if(ValueReg == 0)
		{
			DoTimerTick();
		}
	}
	else
	{
		SCML2_LOG(debug_log) << std::endl;
	}
}
	
template<unsigned int BUSWIDTH>
sc_core::sc_time GenericTimer<BUSWIDTH>::TimeToNextTick()
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":TimeToNextTick(): clock_gate.get_period()=" << clock_gate.get_period() << ", PrescaleReg=" << PrescaleReg << ", LoadReg=" << LoadReg << ", ValueReg=" << ValueReg;
	sc_core::sc_time time_to_next_tick(clock_gate.get_period());
	time_to_next_tick *= PrescaleReg;
	
	if(ModeReg.CountModeBit)
	{
		// count down
		time_to_next_tick *= ValueReg;
	}
	else
	{
		// count up
		time_to_next_tick *= LoadReg - ValueReg;
	}

	SCML2_LOG(debug_log) << ", returns " << time_to_next_tick << std::endl;
	return time_to_next_tick;
}

template<unsigned int BUSWIDTH>
sc_core::sc_time GenericTimer<BUSWIDTH>::TimeToNextStateUpdate()
{
	sc_core::sc_time time_to_next_tick = TimeToNextTick();
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":irq=" << irq << ", irq_time=" << irq_time;
	sc_core::sc_time time_to_next_state_update = ((irq != irq_level) || (irq_time == sc_core::SC_ZERO_TIME)) ? time_to_next_tick : (time_to_next_tick < irq_time) ? time_to_next_tick : irq_time;
	SCML2_LOG(debug_log) << ", returns " << time_to_next_state_update << std::endl;
	return time_to_next_state_update;
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::UpdateState()
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":UpdateState():" << std::endl;
	if(ENABLE && EnableReg.EnableBit)
	{
		const sc_core::sc_time& update_time = sc_core::sc_time_stamp();
		
		sc_core::sc_time delay_since_last_update(update_time);
		delay_since_last_update -= last_state_update_time;
		
		sc_dt::uint64 prescaled_clock_ticks_since_last_update = (delay_since_last_update / clock_gate.get_period()) / PrescaleReg;
		
		if(irq == irq_level)
		{
			assert(irq_time >= delay_since_last_update);
			irq_time -= delay_since_last_update;
		}
		
		if(ModeReg.CountModeBit)
		{
			// count down
			DecrementValueReg(prescaled_clock_ticks_since_last_update);
		}
		else
		{
			// count up
			IncrementValueReg(prescaled_clock_ticks_since_last_update);
		}
		
		if(clear)
		{
			SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":UpdateState(): Clearing" << std::endl;
			irq = !irq_level;
			irq_time = sc_core::SC_ZERO_TIME;
			clear = false;
			generate_output_event.notify(sc_core::SC_ZERO_TIME);
		}
		else
		{
			if((irq == irq_level) && (irq_time == sc_core::SC_ZERO_TIME))
			{
				SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":UpdateState(): reached IRQ width" << std::endl;
				irq = !irq_level;
				generate_output_event.notify(sc_core::SC_ZERO_TIME);
			}
		}
		
		sc_core::sc_time time_to_next_state_update = TimeToNextStateUpdate();
		SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":UpdateState(): time_to_next_state_update = " << time_to_next_state_update << std::endl;
		
		if(time_to_next_state_update != sc_core::SC_ZERO_TIME)
		{
			update_state_event.notify(time_to_next_state_update);
		}
		
		last_state_update_time = update_time;
	}
	else
	{
		generate_output_event.notify(sc_core::SC_ZERO_TIME);
		irq = !irq_level;
	}
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::DoTimerTick()
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":DoTimerTick(): begin of IRQ width" << std::endl;
	irq = irq_level;
	generate_output_event.notify(sc_core::SC_ZERO_TIME);
	irq_time = sc_core::sc_time(irq_width, StringToTimeUnit(irq_width_unit));
	
	if(ModeReg.RunModeBit)
	{
		// Auto-Reload
		SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":DoTimerTick(): ValueReg <- " << ValueReg;
		ValueReg = LoadReg;
		SCML2_LOG(debug_log) << " => " << ValueReg << std::endl;
	}
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::UpdateStateProcess()
{
	UpdateState();
}

template<unsigned int BUSWIDTH>
void GenericTimer<BUSWIDTH>::GenerateOutputProcess()
{
	SCML2_LOG(debug_log) << sc_core::sc_time_stamp() << ":GenerateOutputProcess(): IRQ <- " << IRQ << " => " << irq << std::endl;
	IRQ = irq;
}

template<unsigned int BUSWIDTH>
GenericTimer<BUSWIDTH>::ModeRegType::ModeRegType(const std::string& name, scml2::memory<uint32_t>& memory, sc_dt::uint64 offset)
	: scml2::reg<uint32_t>(name, memory, offset)
	, CountModeBit(sc_core::sc_gen_unique_name("CountModeBit", true), *this, MODE_REG_COUNT_MODE_BIT, 1)
	, RunModeBit(sc_core::sc_gen_unique_name("RunModeBit", true), *this, MODE_REG_RUN_MODE_BIT, 1)
{
}

template<unsigned int BUSWIDTH>
GenericTimer<BUSWIDTH>::EnableRegType::EnableRegType(const std::string& name, scml2::memory<uint32_t>& memory, sc_dt::uint64 offset)
	: scml2::reg<uint32_t>(name, memory, offset)
	, EnableBit(sc_core::sc_gen_unique_name("EnableBit", true), *this, ENABLE_REG_ENABLE_BIT, 1)
{
}

#endif // __GENERIC_TIMER_H__
