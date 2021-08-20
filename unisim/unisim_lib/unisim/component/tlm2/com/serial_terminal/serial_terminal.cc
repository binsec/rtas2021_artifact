/*
 *  Copyright (c) 2017,
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

#include <unisim/kernel/variable/variable.hh>
#include <unisim/component/tlm2/com/serial_terminal/serial_terminal.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace serial_terminal {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_NEED_SYNC;

SerialTerminal::SerialTerminal(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "Serial terminal")
	, sc_core::sc_module(name)
	, unisim::kernel::Client<unisim::service::interfaces::CharIO>(name, parent)
	, TX("TX")
	, RX("RX")
	, CLK("CLK")
	, char_io_import("char-io-import", this)
	, logger(*this)
	, clk_prop_proxy(CLK)
	, clock_period(10.0, sc_core::SC_NS)
	, clock_start_time()
	, clock_posedge_first(true)
	, clock_duty_cycle(0.5)
	, rx_input()
	, rx_time(sc_core::SC_ZERO_TIME)
	, baud_period_lower_bound(sc_core::SC_ZERO_TIME)
	, baud_period(sc_core::SC_ZERO_TIME)
	, baud_period_upper_bound(sc_core::SC_ZERO_TIME)
	, polling_event("polling_event")
	, tx_event("tx_event")
	, tx_fifo()
	, rx_fifo()
	, polling_period(10.0, sc_core::SC_MS)
	, param_polling_period("polling-period", this, polling_period, "polling period")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, parity_type(PARITY_TYPE_NONE)
	, param_parity_type("parity-type", this, parity_type, "parity type (none, even, odd)")
	, num_stop_bits(1)
	, param_num_stop_bits("num-stop-bits", this, num_stop_bits, "number of stop bits (1, 2, ...)")
	, bit_order(MSB)
	, param_bit_order("bit-order", this, bit_order, "first bit in reception/transmission (lsb, msb)")
	, num_data_bits(8)
	, param_num_data_bits("num-data-bits", this, num_data_bits, "number of data bits (1-8; typically 7 or 8)")
	, baud_tolerance(2.0)
	, param_baud_tolerance("baud-tolerance", this, baud_tolerance, "Baud tolerance (up to 5 %) in reception (fraction in percents of baud period); Tolerate +/- X % drift")
	, boot_receive_delay(sc_core::SC_ZERO_TIME)
	, param_boot_receive_delay("boot-receive-delay", this, boot_receive_delay, "Boot receive delay (guest delay to be ready for receiving characters)")
{
	param_baud_tolerance.SetMutable(false);
	param_polling_period.SetMutable(false);
	param_parity_type.SetMutable(false);
	param_num_stop_bits.SetMutable(false);
	param_num_data_bits.SetMutable(false);
	param_bit_order.SetMutable(false);
	param_num_stop_bits.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_num_data_bits.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	TX.register_nb_receive(this, &SerialTerminal::nb_receive, TX_IF);
	RX.register_nb_receive(this, &SerialTerminal::nb_receive, RX_IF);

	SC_HAS_PROCESS(SerialTerminal);
	
	SC_METHOD(PollingProcess);
	sensitive << polling_event;
	
	SC_THREAD(TX_Process);
	sensitive << tx_event;
	
	SC_THREAD(RX_Process);
	sensitive << rx_input.event();
}

SerialTerminal::~SerialTerminal()
{
	ProcessOutput();
}

bool SerialTerminal::EndSetup()
{
	bool setup_status = true;
	
	if(!num_data_bits || (num_data_bits > 8))
	{
		logger << DebugError << param_num_data_bits.GetName() << " shall be >= 1 and <= 8" << EndDebugError;
		setup_status = false;
	}
	
	if(!num_stop_bits)
	{
		logger << DebugError << param_num_stop_bits.GetName() << " shall be >= 1" << EndDebugError;
		setup_status = false;
	}
	
	if(baud_tolerance < 0.0)
	{
		baud_tolerance = 0.0;
		logger << DebugWarning << "Parameter " << param_baud_tolerance.GetName() << " should be > " << baud_tolerance << "; using " << param_baud_tolerance.GetName() << " = " << baud_tolerance << " %" << EndDebugWarning;
	}
	if(baud_tolerance > 5.0)
	{
		baud_tolerance = 5.0;
		logger << DebugWarning << "Parameter " << param_baud_tolerance.GetName() << " should be <= " << baud_tolerance << "; using " << param_baud_tolerance.GetName() << " = " << baud_tolerance << " %" << EndDebugWarning;
	}

	if(!char_io_import)
	{
		setup_status = false;
	}
	
	return setup_status;
}

void SerialTerminal::end_of_elaboration()
{
	// Spawn ClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options clock_properties_changed_process_spawn_options;
	
	clock_properties_changed_process_spawn_options.spawn_method();
	clock_properties_changed_process_spawn_options.set_sensitivity(&clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&SerialTerminal::ClockPropertiesChangedProcess, this), "ClockPropertiesChangedProcess", &clock_properties_changed_process_spawn_options);
}

void SerialTerminal::ClockPropertiesChangedProcess()
{
	clock_period = clk_prop_proxy.GetClockPeriod();
	clock_start_time = clk_prop_proxy.GetClockStartTime();
	clock_posedge_first = clk_prop_proxy.GetClockPosEdgeFirst();
	clock_duty_cycle = clk_prop_proxy.GetClockDutyCycle();
	
	baud_period = clock_period;
	sc_core::sc_time baud_period_tolerance = baud_period * (baud_tolerance / 100.0);
	baud_period_lower_bound = baud_period;
	baud_period_lower_bound -= baud_period_tolerance;
	baud_period_upper_bound = baud_period;
	baud_period_upper_bound += baud_period_tolerance;
}

void SerialTerminal::nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	if(id == RX_IF)
	{
		const unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
		const sc_core::sc_time& period = payload.get_period();
		
		if(verbose)
		{
			logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": receiving [" << data << "] over RX with a period of " << period << EndDebugInfo;
		}

		if((period < baud_period_lower_bound) || (period >= baud_period_upper_bound))
		{
			logger << DebugWarning << "Receiving over " << RX.name() << " with a period " << period << " instead of " << baud_period << " with a tolerance of " << baud_tolerance << " %" << EndDebugWarning;
		}
		
		rx_input.fill(payload, t);
	}
}

void SerialTerminal::TX_Process()
{
	while(1)
	{
		wait();
		
		if(sc_core::sc_time_stamp() >= tx_ready_time)
		{
			tx_ready_time = sc_core::sc_time_stamp();
			
			if(!tx_fifo.empty())
			{
				unisim::kernel::tlm2::tlm_serial_payload payload;
				unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();

				payload.set_period(clock_period);
				
				do
				{
					data.push_back(0); // start bit
					tx_ready_time += clock_period;

					uint8_t value = tx_fifo.front();
					tx_fifo.pop();

					bool parity = (parity_type == PARITY_TYPE_EVEN) ? false : true;
					unsigned int i;
					for(i = 0; i < num_data_bits; i++)
					{
						bool bit_value = (bit_order == MSB) ? ((value >> (num_data_bits - 1 - i)) & 1) : ((value >> i) & 1);
						data.push_back(bit_value);
						tx_ready_time += clock_period;
						parity ^= bit_value;
					}
					
					if(parity_type != PARITY_TYPE_NONE)
					{
						data.push_back(parity); // parity bit
					}
					
					for(i = 0; i < num_stop_bits; i++)
					{
						data.push_back(1); // stop bit
						tx_ready_time += clock_period;
					}
				}
				while(!tx_fifo.empty());
				
				if(verbose)
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": transmitting [" << data << "] over TX with a period of " << clock_period << EndDebugInfo;
				}
				
				TX->nb_send(payload);
				
#if 0
				if(sc_core::sc_time_stamp() > tx_ready_time)
				{
					tx_ready_time = sc_core::sc_time_stamp();
				}
#endif
			}
		}
	}
}

bool SerialTerminal::RX_InputStatus()
{
	unsigned int miss_count = rx_input.get_miss_count();
	
	if(miss_count)
	{
		logger << DebugWarning << (sc_core::sc_time_stamp() + rx_time) << ": " << miss_count << " bit(s) lost from " << RX.name() << EndDebugWarning;
	}
	
	return rx_input.read();
}

void SerialTerminal::IncrementRxTime()
{
	const sc_core::sc_time& input_period = rx_input.get_period();
	if((input_period >= baud_period_lower_bound) && (input_period <= baud_period_upper_bound))
	{
		rx_time += input_period;
	}
	else
	{
		rx_time += baud_period;
	}
}

void SerialTerminal::RX_Process()
{
	wait();
	rx_input.latch();
	
	rx_time = sc_core::SC_ZERO_TIME;
	
	while(1)
	{
		bool bit_value = RX_InputStatus();
		if(verbose)
		{
			logger << DebugInfo << rx_input.get_time_stamp() << ": RX=" << bit_value << EndDebugInfo;
		}
		IncrementRxTime();
		if(!bit_value) // start bit ?
		{
			// received start bit
			if(verbose)
			{
				logger << DebugInfo << rx_input.get_time_stamp() << ": received start bit" << EndDebugInfo;
			}
			
			if(verbose)
			{
				logger << DebugInfo << rx_input.get_time_stamp() << ": expecting " << num_data_bits << " data bits" << EndDebugInfo;
			}
			unsigned int i;
			uint8_t value = 0;
			bool parity = (parity_type == PARITY_TYPE_EVEN) ? false : true;
			for(i = 0; i < num_data_bits; i++)
			{
				if(rx_input.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
				{
					wait(rx_time);
					rx_input.latch();
					rx_time = sc_core::SC_ZERO_TIME;
				}
				
				if(bit_order == MSB)
				{
					value <<= 1;
				}
				else
				{
					value >>= 1;
				}
				bit_value = RX_InputStatus();
				if(verbose)
				{
					logger << DebugInfo << rx_input.get_time_stamp() << ": RX=" << bit_value << EndDebugInfo;
				}
				IncrementRxTime();
				if(bit_order == MSB)
				{
					value |= bit_value;
				}
				else
				{
					value |= (bit_value << (num_data_bits - 1));
				}
				parity ^= bit_value;
			}
			
			bool parity_error = false;
			if(parity_type != PARITY_TYPE_NONE)
			{
				if(verbose)
				{
					logger << DebugInfo << rx_input.get_time_stamp() << ": expecting parity bit" << EndDebugInfo;
				}
				
				if(rx_input.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
				{
					wait(rx_time);
					rx_input.latch();
					rx_time = sc_core::SC_ZERO_TIME;
				}
				bit_value = RX_InputStatus();
				if(verbose)
				{
					logger << DebugInfo << rx_input.get_time_stamp() << ": RX=" << bit_value << EndDebugInfo;
				}
				
				parity_error = (parity != bit_value);
			}
			
			if(verbose)
			{
				logger << DebugInfo << rx_input.get_time_stamp() << ": expecting " << num_stop_bits << " stop bits" << EndDebugInfo;
			}
			unsigned int rec_stop_bits;
			for(rec_stop_bits = 0; rec_stop_bits < num_stop_bits; rec_stop_bits++)
			{
				if(rx_input.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
				{
					wait(rx_time);
					rx_input.latch();
					rx_time = sc_core::SC_ZERO_TIME;
				}
				bit_value = RX_InputStatus();
				if(verbose)
				{
					logger << DebugInfo << rx_input.get_time_stamp() << ": RX=" << bit_value << EndDebugInfo;
				}
				IncrementRxTime();
				if(!bit_value) // not a stop bit
				{
					break;
				}
				
				if(verbose)
				{
					logger << DebugInfo << rx_input.get_time_stamp() << ": received stop bit" << EndDebugInfo;
				}
			}
			
			if(!parity_error && (rec_stop_bits == num_stop_bits))
			{
				if(verbose)
				{
					logger << DebugInfo << rx_input.get_time_stamp() << ": received all stop bits" << EndDebugInfo;
				}
				rx_fifo.push(value);
			}
		}
		
		if(rx_input.next() == TLM_INPUT_BITSTREAM_NEED_SYNC)
		{
			wait();
			rx_input.latch();
			rx_time = sc_core::SC_ZERO_TIME;
		}
	}
}

void SerialTerminal::ProcessInput()
{
	char c;
	uint8_t v;
	
	if(!char_io_import->GetChar(c)) return;
	
	v = (uint8_t) c;
	if(verbose)
	{
		logger << DebugInfo << "Receiving ";
		if(v >= 32)
			logger << "character '" << c << "'";
		else
			logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
		logger << " from telnet client" << EndDebugInfo;
	}

	tx_fifo.push(v);

	if(sc_core::sc_time_stamp() < tx_ready_time)
	{
		sc_core::sc_time notify_delay(tx_ready_time);
		notify_delay -= sc_core::sc_time_stamp();
		tx_event.notify(notify_delay);
	}
	else
	{	
		tx_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void SerialTerminal::ProcessOutput()
{
	char c;
	uint8_t v;
	
	if(!rx_fifo.empty())
	{
		do
		{
			v = rx_fifo.front();
			rx_fifo.pop();
			c = (char) v;
			if(verbose)
			{
				logger << DebugInfo << "Sending ";
				if(v >= 32)
					logger << "character '" << c << "'";
				else
					logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
				logger << " to telnet client" << EndDebugInfo;
			}
			char_io_import->PutChar(c);
		}
		while(!rx_fifo.empty());
	}
	
	char_io_import->FlushChars();
}

void SerialTerminal::PollingProcess()
{
	if(sc_core::sc_time_stamp() >= boot_receive_delay)
	{
		ProcessInput();
	}
	ProcessOutput();
	polling_event.notify(polling_period);
}

} // end of namespace serial_terminal
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace variable {

using unisim::component::tlm2::com::serial_terminal::ParityType;
using unisim::component::tlm2::com::serial_terminal::PARITY_TYPE_NONE;
using unisim::component::tlm2::com::serial_terminal::PARITY_TYPE_EVEN;
using unisim::component::tlm2::com::serial_terminal::PARITY_TYPE_ODD;

  template <> Variable<ParityType>::Variable(const char *_name, Object *_object, ParityType& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("none");
	AddEnumeratedValue("even");
	AddEnumeratedValue("odd");
}

template <>
const char *Variable<ParityType>::GetDataTypeName() const
{
	return "parity-type";
}

template <>
VariableBase::DataType Variable<ParityType>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<ParityType>::GetBitSize() const
{
	return 2;
}

template <> Variable<ParityType>::operator bool () const { return *storage != PARITY_TYPE_NONE; }
template <> Variable<ParityType>::operator long long () const { return *storage; }
template <> Variable<ParityType>::operator unsigned long long () const { return *storage; }
template <> Variable<ParityType>::operator double () const { return (double)(*storage); }
template <> Variable<ParityType>::operator std::string () const
{
	switch(*storage)
	{
		case PARITY_TYPE_NONE: return std::string("none");
		case PARITY_TYPE_EVEN: return std::string("even");
		case PARITY_TYPE_ODD : return std::string("odd");
	}
	return std::string("?");
}

template <> VariableBase& Variable<ParityType>::operator = (bool value)
{
	if(IsMutable())
	{
		ParityType tmp = *storage;
		switch((unsigned int) value)
		{
			case PARITY_TYPE_NONE:
			case PARITY_TYPE_EVEN:
			case PARITY_TYPE_ODD :
				tmp = (ParityType)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<ParityType>::operator = (long long value)
{
	if(IsMutable())
	{
		ParityType tmp = *storage;
		switch(value)
		{
			case PARITY_TYPE_NONE:
			case PARITY_TYPE_EVEN:
			case PARITY_TYPE_ODD :
				tmp = (ParityType) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<ParityType>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		ParityType tmp = *storage;
		switch(value)
		{
			case PARITY_TYPE_NONE:
			case PARITY_TYPE_EVEN:
			case PARITY_TYPE_ODD :
				tmp = (ParityType) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<ParityType>::operator = (double value)
{
	if(IsMutable())
	{
		ParityType tmp = *storage;
		switch((unsigned int) value)
		{
			case PARITY_TYPE_NONE:
			case PARITY_TYPE_EVEN:
			case PARITY_TYPE_ODD :
				tmp = (ParityType)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<ParityType>::operator = (const char *value)
{
	if(IsMutable())
	{
		ParityType tmp = *storage;
		if(std::string(value) == std::string("none")) tmp = PARITY_TYPE_NONE;
		else if(std::string(value) == std::string("even")) tmp = PARITY_TYPE_EVEN;
		else if(std::string(value) == std::string("odd")) tmp = PARITY_TYPE_ODD;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<ParityType>;

using unisim::component::tlm2::com::serial_terminal::BitOrder;
using unisim::component::tlm2::com::serial_terminal::LSB;
using unisim::component::tlm2::com::serial_terminal::MSB;

template <> Variable<BitOrder>::Variable(const char *_name, Object *_object, BitOrder& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("lsb");
	AddEnumeratedValue("msb");
}

template <>
const char *Variable<BitOrder>::GetDataTypeName() const
{
	return "bit-order";
}

template <>
VariableBase::DataType Variable<BitOrder>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<BitOrder>::GetBitSize() const
{
	return 1;
}

template <> Variable<BitOrder>::operator bool () const { return *storage != LSB; }
template <> Variable<BitOrder>::operator long long () const { return *storage; }
template <> Variable<BitOrder>::operator unsigned long long () const { return *storage; }
template <> Variable<BitOrder>::operator double () const { return (double)(*storage); }
template <> Variable<BitOrder>::operator std::string () const
{
	switch(*storage)
	{
		case LSB: return std::string("lsb");
		case MSB: return std::string("msb");
	}
	return std::string("?");
}

template <> VariableBase& Variable<BitOrder>::operator = (bool value)
{
	if(IsMutable())
	{
		BitOrder tmp = *storage;
		switch((unsigned int) value)
		{
			case LSB:
			case MSB:
				tmp = (BitOrder)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<BitOrder>::operator = (long long value)
{
	if(IsMutable())
	{
		BitOrder tmp = *storage;
		switch(value)
		{
			case LSB:
			case MSB:
				tmp = (BitOrder) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<BitOrder>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		BitOrder tmp = *storage;
		switch(value)
		{
			case LSB:
			case MSB:
				tmp = (BitOrder) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<BitOrder>::operator = (double value)
{
	if(IsMutable())
	{
		BitOrder tmp = *storage;
		switch((unsigned int) value)
		{
			case LSB:
			case MSB:
				tmp = (BitOrder)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<BitOrder>::operator = (const char *value)
{
	if(IsMutable())
	{
		BitOrder tmp = *storage;
		if(std::string(value) == std::string("lsb")) tmp = LSB;
		else if(std::string(value) == std::string("msb")) tmp = MSB;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<BitOrder>;

} // end of variable namespace
} // end of kernel namespace
} // end of unisim namespace
