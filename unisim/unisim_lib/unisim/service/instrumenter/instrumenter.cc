/*
 *  Copyright (c) 2015-2019,
 *  Commissariat a l'Energie Atomique et aux Energies Alternatives (CEA)
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

#include <unisim/service/instrumenter/instrumenter.hh>
#include <unisim/kernel/tlm2/tlm.hh>

namespace unisim {
namespace service {
namespace instrumenter {

Instrumenter::Instrumenter(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "Hardware instrumenter")
	, http_server_export("http-server-export", this)
	, logger(*this)
	, verbose(false)
	, debug(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, debug, "Enable/Disable debugging (intended for developper)")
	, vcd_trace_filename()
	, param_vcd_trace_filename("vcd-trace-file", this, vcd_trace_filename, "VCD output filename for signals trace")
	, trace_signals()
	, param_trace_signals("trace-signals", this, trace_signals, "Signals to trace (wildcards '*' and '?' are allowed in signal names)")
	, enable_trace_signals(false)
	, param_enable_trace_signals("enable-trace-signals", this, enable_trace_signals, "Enable/Disable signals trace")
	, trace_file(0)
	, gtkwave_init_script()
	, param_gtkwave_init_script("gtkwave-init-script", this, gtkwave_init_script, "GTKWave initialization script that simulators must automatically generate at startup")
	, enable_user_interface(false)
	, param_enable_user_interface("enable-user-interface", this, enable_user_interface, "Enable/Disable User interface over HTTP")
	, user_interface(0)
	, enable_csv_reader(false)
	, param_enable_csv_reader("enable-csv-reader", this, enable_csv_reader, "Enable/Disable CSV reader")
	, csv_reader(0)
	, enable_csv_writer(false)
	, param_enable_csv_writer("enable-csv-writer", this, enable_csv_writer, "Enable/Disable CSV writer")
	, csv_writer(0)
	, front_ends_priority_order()
	, param_front_ends_priority_order("front-ends-priority-order", this, front_ends_priority_order, "Front-ends priority order (from left/higher to right/lower priority)")
	, signal_pool()
	, auto_signal_pool()
	, port_pool()
	, netlist()
	, typers()
	, signal_tees()
	, instruments()
	, instrumenter_front_ends()
	, pull_down_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_down", false, OUTPUT_INSTRUMENTATION))
	, pull_down_0_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_down_0", false, OUTPUT_INSTRUMENTATION))
	, pull_up_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_up", true, OUTPUT_INSTRUMENTATION))
	, pull_up_0_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("pull_up_0", true, OUTPUT_INSTRUMENTATION))
	, unused_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("unused", false, OUTPUT_INSTRUMENTATION))
	, unused_0_signal(CreateSignal<bool, sc_core::SC_MANY_WRITERS>("unused_0", false, OUTPUT_INSTRUMENTATION))
{
	if(front_ends_priority_order.empty())
	{
		std::stringstream front_ends_priority_order_sstr;
		front_ends_priority_order_sstr << this->GetName() << ".user-interface " << this->GetName() << ".csv-reader " << this->GetName() << ".csv-writer";
		front_ends_priority_order = front_ends_priority_order_sstr.str();
	}
	
	if(enable_user_interface)
	{
		user_interface = new UserInterface("user-interface", this);
		http_server_export >> user_interface->http_server_export;
	}
	
	if(enable_csv_reader)
	{
		csv_reader = new CSV_Reader("csv-reader", this);
	}
	
	if(enable_csv_writer)
	{
		csv_writer = new CSV_Writer("csv-writer", this);
	}
}

Instrumenter::~Instrumenter()
{
	if(trace_file)
	{
		sc_close_vcd_trace_file(trace_file);
	}

	std::vector<InstrumenterFrontEnd *>::iterator instrument_front_end_it;
	for(instrument_front_end_it = instrumenter_front_ends.begin(); instrument_front_end_it != instrumenter_front_ends.end(); instrument_front_end_it++)
	{
		InstrumenterFrontEnd *instrumenter_front_end = *instrument_front_end_it;
		delete instrumenter_front_end;
	}
	
	std::map<InstrumentKey, InstrumentBase *>::iterator instrument_it;
	for(instrument_it = instruments.begin(); instrument_it != instruments.end(); instrument_it++)
	{
		InstrumentBase *instrument = (*instrument_it).second;
		delete instrument;
	}
	
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		delete signal_if; 
	}

	std::map<std::string, sc_core::sc_interface *>::iterator auto_signal_pool_it;
	
	for(auto_signal_pool_it = auto_signal_pool.begin(); auto_signal_pool_it != auto_signal_pool.end(); auto_signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*auto_signal_pool_it).second;
		delete signal_if; 
	}

	std::map<std::string, TyperBase *>::iterator typer_it;
	
	for(typer_it = typers.begin(); typer_it != typers.end(); typer_it++)
	{
		TyperBase *typer = (*typer_it).second;
		delete typer;
	}
	
	std::map<std::string, SignalTeeBase *>::iterator signal_tee_it;
	for(signal_tee_it = signal_tees.begin(); signal_tee_it != signal_tees.end(); signal_tee_it++)
	{
		SignalTeeBase *signal_tee = (*signal_tee_it).second;
		delete signal_tee;
	}
}

bool Instrumenter::BeginSetup()
{
	if(!SetupInstrumentation()) return false;
	
	StartBinding();
	
	return true;
}

bool Instrumenter::EndSetup()
{
	return true;
}

unisim::kernel::tlm2::Clock& Instrumenter::CreateClock(const std::string& clock_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(clock_name);
	if(signal_pool_it != signal_pool.end())
	{
		throw std::runtime_error("Internal error! clock already exists");
	}

	unisim::kernel::tlm2::Clock *clock = new unisim::kernel::tlm2::Clock(clock_name.c_str(), GetParent());
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Creating " << (clock->IsClockLazy() ? "lazy (fast) " : "toggling (painfully slow) ") << "Clock \"" << clock->sc_core::sc_object::name() << "\" with a period of " << clock->GetClockPeriod() << ", a duty cycle of " << clock->GetClockDutyCycle() << ", starting with " << (clock->GetClockPosEdgeFirst() ? "rising" : "falling") << " edge at " << clock->GetClockStartTime() << EndDebugInfo;
	}
	
	signal_pool[clock->sc_core::sc_object::name()] = clock;
	
	typers[clock->sc_core::sc_object::name()] = new Typer<bool>(this);

	return *clock;
}

void Instrumenter::GenerateGTKWaveInitScript(const std::string& filename, int zoom_factor) const
{
	std::ofstream of(filename.c_str(), std::ofstream::out);
	
	if(of.fail())
	{
		logger << DebugWarning << "Can't open output \"" << filename << "\"" << EndDebugWarning;
		return;
	}
	
	of << "# Added some signals to view" << std::endl;
	of << "set my_sig_list [list]" << std::endl;
	std::map<std::string, sc_core::sc_interface *>::const_iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		sc_core::sc_prim_channel *prim_channel = dynamic_cast<sc_core::sc_prim_channel *>(signal_if);
		
		if(prim_channel)
		{
			of << "lappend my_sig_list \"" << prim_channel->name() << "\"" << std::endl;
		}
	}
	
	of << "set num_added [ gtkwave::addSignalsFromList $my_sig_list ]" << std::endl;
	of << "# Adjust zoom" << std::endl;
	of << "gtkwave::setZoomFactor " << zoom_factor << std::endl;
}

void Instrumenter::RegisterPort(sc_core::sc_port_base& port)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Registering Port \"" << port.name() << "\"" << EndDebugInfo;
	}
	std::map<std::string, sc_core::sc_port_base *>::iterator port_pool_it;
	for(port_pool_it = port_pool.begin(); port_pool_it != port_pool.end(); port_pool_it++)
	{
		sc_core::sc_port_base *registered_port_base = (*port_pool_it).second;
		
		if(registered_port_base && (&port == registered_port_base))
		{
			throw std::runtime_error("Internal error! port already registered");
		}
	}
	
	std::string port_name = port.name();
	
	port_pool[port_name] = &port;
}

void Instrumenter::PrepareInstrumentation(const std::string& signal_name_pattern, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id)
{
	instrumentations.insert(InstrumentKey(signal_name_pattern, instrumentation_type, front_end_id));
}

bool Instrumenter::Match(const char *p, const char *s)
{
	while(*p)
	{
		if(*p == '?')
		{
			if(!*s) return false;

			++s;
			++p;
		}
		else if(*p == '*')
		{
			return Match(p + 1, s) || (*s && Match(p, s + 1));
		}
		else
		{
			if(*s++ != *p++) return false;
		}
	}
	
	return !*s && !*p;
}

bool Instrumenter::Match(const std::string& p, const std::string& s)
{
	return Match(p.c_str(), s.c_str());
}

void Instrumenter::MatchSignalPattern(const std::string& signal_name_pattern, std::vector<std::string>& vec_signal_names) const
{
	std::map<std::string, sc_core::sc_interface *>::const_iterator signal_pool_it;
	
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		std::string s((*signal_pool_it).first);
		const std::string& p = signal_name_pattern;
			
		if(Match(p, s))
		{
			vec_signal_names.push_back(s);
		}
	}
}

void Instrumenter::TraceSignalPattern(const std::string& signal_name_pattern)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it;
	
	bool traced = false;
	for(signal_pool_it = signal_pool.begin(); signal_pool_it != signal_pool.end(); signal_pool_it++)
	{
		std::string s((*signal_pool_it).first);
		const std::string& p = signal_name_pattern;
			
		if(Match(p, s))
		{
			std::map<std::string, TyperBase *>::iterator typer_it = typers.find(s);
			
			if(typer_it != typers.end())
			{
				TyperBase *typer = (*typer_it).second;
				
				if(typer->TryTraceSignal(s))
				{
					traced = true;
				}
			}
		}
	}
	
	if(!traced)
	{
		logger << DebugWarning << "Can't trace Signal \"" << signal_name_pattern << "\"" << EndDebugWarning;
	}
}

void Instrumenter::Bind(const std::string& port_name, const std::string& signal_name)
{
	if(netlist.find(port_name) != netlist.end())
	{
		logger << DebugWarning << "port \"" << port_name << "\" is already bound" << EndDebugWarning;
		return;
	}
	
	netlist[port_name] = signal_name;
}

void Instrumenter::BindArray(unsigned int dim, const std::string& port_array_name, unsigned int port_array_begin_idx, unsigned int port_array_stride, const std::string& signal_array_name, unsigned int signal_array_begin_idx, unsigned int signal_array_stride)
{
	unsigned int idx;
	
	for(idx = 0; idx < dim; idx++)
	{
		unsigned int port_array_idx = port_array_begin_idx + (idx * port_array_stride);
		unsigned int signal_array_idx = signal_array_begin_idx + (idx * signal_array_stride);
		std::stringstream port_name_sstr;
		port_name_sstr << port_array_name << "_" << port_array_idx;
		std::string port_name(port_name_sstr.str());

		std::stringstream signal_name_sstr;
		signal_name_sstr << signal_array_name << "_" << signal_array_idx;
		std::string signal_name(signal_name_sstr.str());
		
		Bind(port_name, signal_name);
	}
}

void Instrumenter::BindArray(unsigned int dim, const std::string& port_array_name, unsigned int port_array_begin_idx, const std::string& signal_array_name, unsigned int signal_array_begin_idx)
{
	BindArray(dim, port_array_name, port_array_begin_idx, 1, signal_array_name, signal_array_begin_idx, 1);
}

void Instrumenter::BindArray(unsigned int dim, const std::string& port_array_name, const std::string& signal_array_name)
{
	BindArray(dim, port_array_name, 0, signal_array_name, 0);
}

void Instrumenter::StartBinding()
{
	std::map<std::string, std::string>::iterator netlist_it;
	
	for(netlist_it = netlist.begin(); netlist_it != netlist.end(); netlist_it++)
	{
		const std::string& port_name = (*netlist_it).first;
		const std::string& signal_name = (*netlist_it).second;

		std::map<std::string, TyperBase *>::iterator typer_it = typers.find(signal_name);
		
		if(typer_it != typers.end())
		{
			TyperBase *typer = (*typer_it).second;
			
			if(typer->TryBind(port_name, signal_name))
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Connecting Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << EndDebugInfo;
				}
				continue;
			}
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Signal \"" << signal_name << "\" does not exist" << EndDebugInfo;
			}
		}
		
		logger << DebugWarning << "Can't connect Port \"" << port_name << "\" to Signal \"" << signal_name << "\"" << EndDebugWarning;
	}
}

bool Instrumenter::SetupInstrumentation()
{
	// enable generation of GTKWave initialization script if requested
	if(!gtkwave_init_script.empty())
	{
		GenerateGTKWaveInitScript(gtkwave_init_script, -34);
	}

	// enable signal tracing if requested
	if(enable_trace_signals)
	{
		if(!((std::string) vcd_trace_filename).empty())
		{
			std::string vcd_trace_filename_w_ext((std::string) vcd_trace_filename);
			
			std::size_t dot_pos = vcd_trace_filename_w_ext.find_last_of('.');
			std::string vcd_trace_filename_wo_ext = (dot_pos != std::string::npos) ? vcd_trace_filename_w_ext.substr(0, dot_pos) : vcd_trace_filename_w_ext;
			
			trace_file = sc_core::sc_create_vcd_trace_file(vcd_trace_filename_wo_ext.c_str());

			std::string signal_name;
			std::stringstream sstr(trace_signals);

			while(sstr >> signal_name)
			{
				TraceSignalPattern(signal_name);
			}
		}
	}

	std::vector<InstrumenterFrontEnd *>::iterator instrument_front_end_it;
	for(instrument_front_end_it = instrumenter_front_ends.begin(); instrument_front_end_it != instrumenter_front_ends.end(); instrument_front_end_it++)
	{
		InstrumenterFrontEnd *instrumenter_front_end = *instrument_front_end_it;
		if(instrumenter_front_end)
		{
			if(!instrumenter_front_end->SetupInstrumentation()) return false;
			instrumenter_front_end->Initialize();
		}
	}
	
	std::map<std::string, SignalTeeBase *>::iterator signal_tee_it;
	for(signal_tee_it = signal_tees.begin(); signal_tee_it != signal_tees.end(); signal_tee_it++)
	{
		SignalTeeBase *signal_tee = (*signal_tee_it).second;
		signal_tee->Initialize();
	}

	
	return true;
}

InstrumentBase *Instrumenter::FindInstrument(const std::string& name, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id)
{
	std::map<InstrumentKey, InstrumentBase *>::iterator instrument_it = instruments.find(InstrumentKey(name, instrumentation_type, front_end_id));
	
	if(instrument_it != instruments.end())
	{
		InstrumentBase *instrument = (*instrument_it).second;
		return instrument;
	}
	
	return 0;
}

InputInstrumentBase *Instrumenter::FindInputInstrument(const std::string& name, unsigned int front_end_id)
{
	return dynamic_cast<InputInstrumentBase *>(FindInstrument(name, INPUT_INSTRUMENTATION, front_end_id));
}

OutputInstrumentBase *Instrumenter::FindOutputInstrument(const std::string& name, unsigned int front_end_id)
{
	return dynamic_cast<OutputInstrumentBase *>(FindInstrument(name, OUTPUT_INSTRUMENTATION, front_end_id));
}

unsigned int Instrumenter::RegisterFrontEnd(InstrumenterFrontEnd *instrumenter_front_end)
{
	unsigned int front_end_id = 0;
	std::istringstream sstr(front_ends_priority_order);
	std::string front_end_name;
	
	while(sstr >> front_end_name)
	{
		if(front_end_name == instrumenter_front_end->GetName())
		{
			unsigned int num_front_ends = instrumenter_front_ends.size();
			if(front_end_id >= num_front_ends) instrumenter_front_ends.resize(front_end_id + 1);
			instrumenter_front_ends[front_end_id] = instrumenter_front_end;
			return front_end_id;
		}
		
		front_end_id++;
	}
	
// 	unsigned int front_end_id = instrumenter_front_ends.size();
// 	instrumenter_front_ends.push_back(instrumenter_front_end);
// 	return front_end_id;
	logger << DebugError << "No priority specified for \"" << instrumenter_front_end->GetName() << "\" in Parameter \"" << param_front_ends_priority_order.GetName() << "\"" << EndDebugError;
	this->Stop(-1);
	return 0;
}

SignalTeeBase::SignalTeeBase(const std::string& _name)
	: enable_injection_event(sc_core::sc_gen_unique_name("enable_injection_event"))
	, process_spawn_options()
	, enable_mask(0)
	, name(_name)
{
	process_spawn_options.spawn_method();
	process_spawn_options.set_sensitivity(&enable_injection_event);
}

SignalTeeBase::~SignalTeeBase()
{
}

void SignalTeeBase::EnableInjection(unsigned int front_end_id)
{
	uint64_t new_enable_mask = enable_mask | (uint64_t(1) << front_end_id);
	if(new_enable_mask != enable_mask)
	{
		enable_mask = new_enable_mask;
		enable_injection_event.notify(sc_core::SC_ZERO_TIME);
	}
}

void SignalTeeBase::DisableInjection(unsigned int front_end_id)
{
	uint64_t new_enable_mask = enable_mask & ~(uint64_t(1) << front_end_id);
	if(new_enable_mask != enable_mask)
	{
		enable_mask = new_enable_mask;
		enable_injection_event.notify(sc_core::SC_ZERO_TIME);
	}
}

InputInstrumentBase::InputInstrumentBase(const std::string& name, unsigned int front_end_id, SignalTeeBase *_signal_tee)
	: InstrumentBase(name, INPUT_INSTRUMENTATION, front_end_id)
	, signal_tee(_signal_tee)
{
}

void InputInstrumentBase::EnableInjection()
{
	signal_tee->EnableInjection(GetFrontEndId());
}

void InputInstrumentBase::DisableInjection()
{
	signal_tee->DisableInjection(GetFrontEndId());
}

OutputInstrumentBase::OutputInstrumentBase(const std::string& name, unsigned int front_end_id)
	: InstrumentBase(name, OUTPUT_INSTRUMENTATION, front_end_id)
{
}

InstrumenterFrontEnd::InstrumenterFrontEnd(const char *name, Instrumenter *_instrumenter)
	: unisim::kernel::Object(name, _instrumenter)
	, instrumenter(_instrumenter)
	, front_end_id(instrumenter->RegisterFrontEnd(this))
	, input_instruments()
	, output_instruments()
	, input_instruments_map()
	, output_instruments_map()
	, output_instrumentation_process_spawn_options()
	, next_input_instrumentation_event(sc_core::sc_gen_unique_name("next_input_instrumentation_event"))
{
}

InstrumenterFrontEnd::~InstrumenterFrontEnd()
{
}

void InstrumenterFrontEnd::MatchSignalPattern(const std::string& signal_name_pattern, std::vector<std::string>& vec_signal_names) const
{
	instrumenter->MatchSignalPattern(signal_name_pattern, vec_signal_names);
}

void InstrumenterFrontEnd::PrepareInstrumentation(const std::string& signal_name_pattern, INSTRUMENTATION_TYPE instrumentation_type)
{
	instrumenter->PrepareInstrumentation(signal_name_pattern, instrumentation_type, front_end_id);
}

InputInstrumentBase *InstrumenterFrontEnd::InstrumentInputSignal(const std::string& signal_name)
{
	InputInstrumentBase *input_instrument = instrumenter->FindInputInstrument(signal_name, front_end_id);
	if(input_instrument)
	{
		input_instruments.push_back(input_instrument);
		input_instruments_map[signal_name] = input_instrument;
	}
	return input_instrument;
}

OutputInstrumentBase *InstrumenterFrontEnd::InstrumentOutputSignal(const std::string& signal_name)
{
	OutputInstrumentBase *output_instrument = instrumenter->FindOutputInstrument(signal_name, front_end_id);
	if(output_instrument)
	{
		output_instrumentation_process_spawn_options.set_sensitivity(output_instrument->GetInterface());
		output_instruments.push_back(output_instrument);
		output_instruments_map[signal_name] = output_instrument;
	}
	return output_instrument;
}

InputInstrumentBase *InstrumenterFrontEnd::FindInputInstrument(const std::string& signal_name) const
{
	std::map<std::string, InputInstrumentBase *>::const_iterator it = input_instruments_map.find(signal_name);
	return (it != input_instruments_map.end()) ? (*it).second : 0;
}

const std::vector<InputInstrumentBase *>& InstrumenterFrontEnd::GetInputInstruments() const
{
	return input_instruments;
}

OutputInstrumentBase *InstrumenterFrontEnd::FindOutputInstrument(const std::string& signal_name) const
{
	std::map<std::string, OutputInstrumentBase *>::const_iterator it = output_instruments_map.find(signal_name);
	return (it != output_instruments_map.end()) ? (*it).second : 0;
}

const std::vector<OutputInstrumentBase *>& InstrumenterFrontEnd::GetOutputInstruments() const
{
	return output_instruments;
}

void InstrumenterFrontEnd::Initialize()
{
	sc_core::sc_spawn_options input_instrumentation_process_spawn_options;
	
	input_instrumentation_process_spawn_options.spawn_method();
	input_instrumentation_process_spawn_options.set_sensitivity(&next_input_instrumentation_event);
	sc_core::sc_spawn(sc_bind(&InstrumenterFrontEnd::InputInstrumentationProcess, this), sc_core::sc_gen_unique_name("InputInstrumentationProcess"), &input_instrumentation_process_spawn_options);
	
	output_instrumentation_process_spawn_options.spawn_method();
	sc_core::sc_spawn(sc_bind(&InstrumenterFrontEnd::OutputInstrumentationProcess, this), sc_core::sc_gen_unique_name("OutputInstrumentationProcess"), &output_instrumentation_process_spawn_options);
}

void InstrumenterFrontEnd::OutputInstrumentationProcess()
{
	ProcessOutputInstruments();
}

void InstrumenterFrontEnd::InputInstrumentationProcess()
{
	ProcessInputInstruments();
}

UserInstrument::UserInstrument(const std::string& _name, InputInstrumentBase *_input_instrument, OutputInstrumentBase *_output_instrument)
	: name(_name)
  , sstr()
	, set_value()
	, get_value()
	, input_instrument(_input_instrument)
	, output_instrument(_output_instrument)
	, enable(false)
	, new_enable(enable)
	, value_changed_breakpoint(false)
	, has_breakpoint_cond(false)
	, get_value_valid(false)
{
	if(input_instrument)
	{
		std::ostringstream set_value_sstr;
		input_instrument->Print(set_value_sstr);
		set_value = set_value_sstr.str();
	}
	
	if(output_instrument)
	{
		std::ostringstream get_value_sstr;
		output_instrument->Output(get_value_sstr);
		get_value = get_value_sstr.str();
		get_value_valid = true;
	}
	
	if(input_instrument)
	{
		if(enable)
		{
			input_instrument->EnableInjection();
		}
		else
		{
			input_instrument->DisableInjection();
		}
	}
}

const std::string& UserInstrument::GetName() const
{
	return name;
}

void UserInstrument::Set(const std::string& value)
{
	set_value = value;
}

void UserInstrument::Get(std::string& value) const
{
	Update();
	value = get_value;
}

void UserInstrument::Set(bool value)
{
	set_value = value ? "1" : "0";
}

void UserInstrument::Get(bool& value) const
{
	Update();
	value = (get_value != "0");
}

void UserInstrument::Toggle()
{
	Update();
	if(get_value == "1") set_value = "0";
	else if(get_value == "0") set_value = "1";
}

void UserInstrument::InitialFetch()
{
	if(likely(output_instrument))
	{
		sstr.clear();
		sstr.seekp(0);
		output_instrument->Output(sstr);
		get_value = sstr.str();
	}
}

void UserInstrument::Update() const
{
	if(!get_value_valid)
	{
		sstr.clear();
		sstr.seekp(0);
		output_instrument->Output(sstr);
		get_value = sstr.str();
		get_value_valid = true;
	}
}

void UserInstrument::Sample()
{
	if(likely(output_instrument))
	{
		output_instrument->Sample();
// 		sstr.clear();
// 		sstr.seekp(0);
// 		output_instrument->Output(sstr);
		if(value_changed_breakpoint && output_instrument->ValueChanged()) has_breakpoint_cond = true;
// 		get_value = sstr.str();
		get_value_valid = false;
	}
}

void UserInstrument::Latch()
{
	if(likely(output_instrument))
	{
		output_instrument->Latch();
	}
}

void UserInstrument::Fetch()
{
	Sample();
	if(ValueChanged())
	{
		Latch();
	}
}

void UserInstrument::EnableValueChangedBreakpoint()
{
	value_changed_breakpoint = true;
}

void UserInstrument::DisableValueChangedBreakpoint()
{
	value_changed_breakpoint = false;
}

bool UserInstrument::IsValueChangedBreakpointEnabled() const
{
	return value_changed_breakpoint;
}

void UserInstrument::ClearBreakpointCondition()
{
	has_breakpoint_cond = false;
}

bool UserInstrument::HasBreakpointCondition() const
{
	return has_breakpoint_cond;
}

void UserInstrument::Commit()
{
	if(likely(input_instrument))
	{
		if(new_enable != enable)
		{
			enable = new_enable;
			if(enable)
			{
				input_instrument->EnableInjection();
			}
			else
			{
				input_instrument->DisableInjection();
			}
		}
		//std::cerr << "UserInstrument:Commit(): \"" << name << "\" <- " << set_value << std::endl;
		std::istringstream sstr(set_value);
		input_instrument->Input(sstr);
		input_instrument->Inject();
	}
}

void UserInstrument::EnableInjection()
{
	if(input_instrument) new_enable = true;
}

void UserInstrument::DisableInjection()
{
	new_enable = false;
}

bool UserInstrument::IsInjectionEnabled() const
{
	return new_enable;
}

bool UserInstrument::IsReadOnly() const
{
	return !input_instrument;
}

bool UserInstrument::IsBoolean() const
{
	return (input_instrument && (input_instrument->GetTypeInfo() == typeid(bool))) || (output_instrument && (output_instrument->GetTypeInfo() == typeid(bool)));
}

bool UserInstrument::ValueChanged() const
{
	return output_instrument->ValueChanged();

}

UserInterface::UserInterface(const char *name, Instrumenter *instrumenter)
	: unisim::kernel::Object(name, instrumenter, "Hardware Instrumenter user interface")
	, InstrumenterFrontEnd(name, instrumenter)
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>(name, instrumenter)
	, http_server_export("http-server-export", this)
	, logger(*this)
	, program_name(unisim::kernel::Simulator::Instance()->FindVariable("program-name")->operator std::string())
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, instrumentation()
	, param_instrumentation("instrumentation", this, instrumentation, "Instrumented signals (wildcards '*' and '?' are allowed in signal names) that are controlled by user over HTTP")
	, intr_poll_period(1.0, sc_core::SC_US)
	, param_intr_poll_period("intr-poll-period", this, intr_poll_period, "Polling period (target time) for user interrupt request while continue")
	, min_cont_refresh_period(0.125)
	, param_min_cont_refresh_period("min-cont-refresh-period", this, min_cont_refresh_period, "Minimum refresh period (host time) in seconds while continue")
	, max_cont_refresh_period(1.0)
	, param_max_cont_refresh_period("cont-refresh-period", this, max_cont_refresh_period, "Maximum refresh period (host time) in seconds while continue")
	, enable_cache(true)
	, param_enable_cache("enable-cache", this, enable_cache, "Enable/Disable web browser caching")
	, instrumented_signal_names()
	, user_instruments()
	, schedule()
	, user_step_time(sc_core::SC_ZERO_TIME)
	, curr_time_stamp(sc_core::SC_ZERO_TIME)
	, delta_steps(0)
	, cont(false)
	, intr(true)
	, halt(false)
	, refresh_period(0.125)
	, auto_reload(false)
	, mutex_instruments()
	, mutex_schedule()
	, mutex_delta_steps()
	, mutex_post()
	, cond_cont()
	, mutex_cont()
{
	pthread_mutex_init(&mutex_instruments, NULL);
	pthread_mutex_init(&mutex_schedule, NULL);
	pthread_mutex_init(&mutex_delta_steps, NULL);
	pthread_mutex_init(&mutex_post, NULL);
	pthread_cond_init(&cond_cont, NULL);
	pthread_mutex_init(&mutex_cont, NULL);
	
	std::stringstream sstr(instrumentation);
	
	std::string signal_name_pattern;
	while(sstr >> signal_name_pattern)
	{
		PrepareInstrumentation(signal_name_pattern, INPUT_OUTPUT_INSTRUMENTATION);
	}
}

UserInterface::~UserInterface()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		delete user_instrument;
	}

	pthread_mutex_destroy(&mutex_instruments);
	pthread_mutex_destroy(&mutex_schedule);
	pthread_mutex_destroy(&mutex_delta_steps);
	pthread_mutex_destroy(&mutex_post);
	pthread_cond_destroy(&cond_cont);
	pthread_mutex_destroy(&mutex_cont);
}

bool UserInterface::EndSetup()
{
	return true;
}

void UserInterface::Kill()
{
	halt = true;
	Continue();
	unisim::kernel::Object::Kill();
}

bool UserInterface::SetupInstrumentation()
{
	std::stringstream sstr(instrumentation);
	std::string signal_name_pattern;

	instrumented_signal_names.clear();
	
	while(sstr >> signal_name_pattern)
	{
		MatchSignalPattern(signal_name_pattern, instrumented_signal_names);
	}
	
	std::vector<std::string>::iterator instrumented_signal_name_it;
	for(instrumented_signal_name_it = instrumented_signal_names.begin(); instrumented_signal_name_it != instrumented_signal_names.end(); instrumented_signal_name_it++)
	{
		const std::string& instrumented_signal_name = *instrumented_signal_name_it;
	
		InputInstrumentBase *input_instrument = InstrumentInputSignal(instrumented_signal_name);
		OutputInstrumentBase *output_instrument = InstrumentOutputSignal(instrumented_signal_name);
		
		if(input_instrument || output_instrument)
		{
			UserInstrument *user_instrument = new UserInstrument(instrumented_signal_name, input_instrument, output_instrument);
			user_instruments[instrumented_signal_name] = user_instrument;
			if(!input_instrument)
			{
				logger << DebugWarning << "User instrument \"" << user_instrument->GetName() << "\" is read-only" << EndDebugWarning;
			}
			if(!output_instrument)
			{
				logger << DebugWarning << "User instrument \"" << user_instrument->GetName() << "\" is write-only" << EndDebugWarning;
			}
		}
		else
		{
			logger << DebugWarning << "Can't get input/output instruments for signal \"" << instrumented_signal_name << "\"" << EndDebugWarning;
		}
	}
	
	InitialFetch();
	DisableInjection();
	DisableValueChangedBreakpoint();
	
	return true;
}

void UserInterface::WaitForUser()
{
// 	std::cerr << sc_core::sc_time_stamp() << ":WaitForUser: start" << std::endl;
	pthread_mutex_lock(&mutex_cont);
	cont = false;
	intr = false;
	halt = false;
	delta_steps = 0;
	do
	{
		pthread_cond_wait(&cond_cont, &mutex_cont);
	}
	while(!cont);
	pthread_mutex_unlock(&mutex_cont);
// 	std::cerr << sc_core::sc_time_stamp() << ":WaitForUser: end" << std::endl;
}

void UserInterface::Continue()
{
// 	std::cerr << "Run" << std::endl;
	pthread_mutex_lock(&mutex_cont);
	cont = true;
	pthread_cond_signal(&cond_cont);
	pthread_mutex_unlock(&mutex_cont);
}

void UserInterface::LockInstruments()
{
	pthread_mutex_lock(&mutex_instruments);
}

void UserInterface::UnlockInstruments()
{
	pthread_mutex_unlock(&mutex_instruments);
}

void UserInterface::LockPost()
{
	pthread_mutex_lock(&mutex_post);
}

void UserInterface::UnlockPost()
{
	pthread_mutex_unlock(&mutex_post);
}

void UserInterface::Commit()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->Commit();
	}
}

void UserInterface::ClearBreakpointConditions()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->ClearBreakpointCondition();
	}
}

void UserInterface::ProcessInputInstruments()
{
	if(unlikely(halt))
	{
		return;
	}
	LockInstruments();
	curr_time_stamp = sc_core::sc_time_stamp();
	sc_core::sc_time deadline(sc_core::sc_max_time());
	Schedule::iterator it = schedule.begin();
	if(unlikely(it != schedule.end()))
	{
		deadline = *it;
		if(curr_time_stamp >= deadline)
		{
			intr = true;
			schedule.erase(it);
			Schedule::iterator next_it = schedule.begin();
			deadline = (next_it != schedule.end()) ? (*next_it) : sc_core::sc_max_time();
		}
	}
	if(unlikely(delta_steps > 0))
	{
		if((delta_steps > 0) && (--delta_steps == 0))
		{
			intr = true;
		}
	}
	if(unlikely(intr))
	{
		do
		{
			UnlockInstruments();
			WaitForUser();
			LockInstruments();
			ClearBreakpointConditions();
			Commit();
			if(unlikely(halt))
			{
				UnlockInstruments();
				return;
			}
		}
		while(unlikely(intr));
		
		Schedule::iterator it = schedule.begin();
		if(unlikely(it != schedule.end()))
		{
			deadline = *it;
		}
	}

	sc_core::sc_time time_to_deadline(deadline);
	time_to_deadline -= curr_time_stamp;
	sc_core::sc_time t(std::min(time_to_deadline, (delta_steps || intr || halt) ? sc_core::SC_ZERO_TIME : intr_poll_period));
	//std::cerr << sc_core::sc_time_stamp() << ": next input instrumentation after " << t << std::endl;
	UnlockInstruments();
	NextInputTrigger(t);
}

void UserInterface::InitialFetch()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->InitialFetch();
	}
}

void UserInterface::Sample()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->Sample();
	}
}

void UserInterface::Fetch()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->Fetch();
		
		if(user_instrument->HasBreakpointCondition())
		{
			//std::cerr << "\"" << user_instrument->GetName() << "\" has breakpoint condition" << std::endl;
			intr = true;
			NextInputTrigger(sc_core::SC_ZERO_TIME);
		}
	}
}

void UserInterface::ProcessOutputInstruments()
{
	LockInstruments();
	Fetch();
	UnlockInstruments();
}

bool UserInterface::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetPath() == "toolbar_actions.js")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
				if(enable_cache)
				{
					response.EnableCache();
				}
				response.SetContentType("application/javascript");
				
				response << "InstrumenterToobarActions.prototype.bound_background_iframe_onload = null;" << std::endl;
				response << "InstrumenterToobarActions.prototype.pending_cmd = null;" << std::endl;
				response << "InstrumenterToobarActions.prototype.background_iframe = null;" << std::endl;
				response << "InstrumenterToobarActions.prototype.retry_count = 0;" << std::endl;
				response << "" << std::endl;
				response << "function InstrumenterToobarActions()" << std::endl;
				response << "{" << std::endl;
				response << "\tthis.bound_background_iframe_onload = new Map();" << std::endl;
				response << "\tthis.pending_cmd = null;" << std::endl;
				response << "\tthis.background_iframe = null;" << std::endl;
				response << "\tthis.retry_count = 0;" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "InstrumenterToobarActions.prototype.get_instrumenter_tab = function()" << std::endl;
				response << "{" << std::endl;
				response << "\treturn gui.find_tab_by_uri('" << URI() << "/control') || gui.find_tab_by_uri('" << URI() << "') || gui.open_tab('top-middle-tile','control-" << GetName() << "','" << URI() << "/control',true);" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "InstrumenterToobarActions.prototype.execute = function(cmd)" << std::endl;
				response << "{" << std::endl;
				response << "\tif(this.pending_cmd)" << std::endl;
				response << "\t{" << std::endl;
				response << "\t\t// if new command is same as the pending command, let pending command execute" << std::endl;
				response << "\t\tif(this.pending_cmd == cmd) return;" << std::endl;
				response << "\t\t// cancel pending command" << std::endl;
				response << "\t\tif(this.bound_background_iframe_onload[this.pending_cmd])" << std::endl;
				response << "\t\t{" << std::endl;
				response << "\t\t\tthis.background_iframe.iframe_element.removeEventListener('load', this.bound_background_iframe_onload[this.pending_cmd]);" << std::endl;
				response << "\t\t}" << std::endl;
				response << "\t\tthis.pending_cmd = null;" << std::endl;
				response << "\t}" << std::endl;
				response << "\t" << std::endl;
				response << "\tvar tab = this.get_instrumenter_tab();" << std::endl;
				response << "\tif(tab)" << std::endl;
				response << "\t{" << std::endl;
				response << "\t\tvar foreground_iframe = tab.get_foreground_iframe();" << std::endl;
				response << "\t\tif(foreground_iframe)" << std::endl;
				response << "\t\t{" << std::endl;
				response << "\t\t\tif(foreground_iframe.loaded)" << std::endl;
				response << "\t\t\t{" << std::endl;
				response << "\t\t\t\tvar cmd_button = foreground_iframe.iframe_element.contentWindow.document.getElementById(cmd);" << std::endl;
				response << "\t\t\t\tif(cmd_button)" << std::endl;
				response << "\t\t\t\t{" << std::endl;
				response << "\t\t\t\t\t// simulate a submit" << std::endl;
				response << "\t\t\t\t\tcmd_button.click();" << std::endl;
				response << "\t\t\t\t\tthis.retry_count = 0;" << std::endl;
				response << "\t\t\t\t\treturn true;" << std::endl;
				response << "\t\t\t\t}" << std::endl;
				response << "\t\t\t}" << std::endl;
				response << "\t\t\tif(this.retry_count < 2)" << std::endl;
				response << "\t\t\t{" << std::endl;
				response << "\t\t\t\t// instrumenter page is either not yet loaded or command button is not available yet" << std::endl;
				response << "\t\t\t\t++this.retry_count;" << std::endl;
				response << "\t\t\t\tthis.background_iframe = tab.get_background_iframe();" << std::endl;
				response << "\t\t\t\tif(this.background_iframe)" << std::endl;
				response << "\t\t\t\t{" << std::endl;
				response << "\t\t\t\t\t// try to execute command once instrumenter page is loaded" << std::endl;
				response << "\t\t\t\t\tthis.pending_cmd = cmd;" << std::endl;
				response << "\t\t\t\t\tthis.bound_background_iframe_onload[cmd] = this.onload.bind(this)" << std::endl;
				response << "\t\t\t\t\tthis.background_iframe.iframe_element.addEventListener('load', this.bound_background_iframe_onload[cmd]);" << std::endl;
				response << "\t\t\t\t}" << std::endl;
				response << "\t\t\t}" << std::endl;
				response << "\t\t}" << std::endl;
				response << "\t}" << std::endl;
				response << "\treturn false;" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "InstrumenterToobarActions.prototype.onload = function()" << std::endl;
				response << "{" << std::endl;
				response << "\tvar cmd = this.pending_cmd;" << std::endl;
				response << "\tif(this.bound_background_iframe_onload[cmd])" << std::endl;
				response << "\t{" << std::endl;
				response << "\t\tthis.background_iframe.iframe_element.removeEventListener('load', this.bound_background_iframe_onload[cmd]);" << std::endl;
				response << "\t}" << std::endl;
				response << "\tthis.pending_cmd = null;" << std::endl;
				response << "\t// instrumenter page is loaded: retry execution of command" << std::endl;
				response << "\tthis.execute(cmd);" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "if((typeof GUI !== 'undefined') && GUI.prototype.constructor)" << std::endl;
				response << "{" << std::endl;
				response << "\tGUI.prototype.instrumenter_toolbar_actions = new InstrumenterToobarActions();" << std::endl;
				response << "}" << std::endl;
				
				break;
			
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, GET, HEAD");
				break;
				
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, GET, HEAD");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				break;
		}
	}
	else if((req.GetPath() == "") || (req.GetPath() == "control") || (req.GetPath() == "instrument"))
	{
		std::string form_action;
		bool is_control_interface = false;
		bool is_instrument_interface = false;
		if(req.GetPath() == "")
		{
			is_control_interface = true;
			is_instrument_interface = true;
			form_action = URI();
		}
		else if(req.GetPath() == "control")
		{
			is_control_interface = true;
			form_action = URI() + "/control";
		}
		else if(req.GetPath() == "instrument")
		{
			is_instrument_interface = true;
			form_action = URI() + "/instrument";
		}
		
		std::string title;
		if(is_control_interface && is_instrument_interface)
		{
			title = "Hardware instrumenter";
		}
		else if(is_control_interface)
		{
			title = "Simulation controls";
		}
		else if(is_instrument_interface)
		{
			title = "Signals instrumenter";
		}
		
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::POST:
			{
				LockPost();
				LockInstruments();
				
				PropertySetter property_setter(*this);
				if(property_setter.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
				{
					property_setter.Apply();
				}
				else
				{
					logger << DebugWarning << "parse error in POST data" << EndDebugWarning;
				}
				
				UnlockInstruments();
				
				if(property_setter.cont || halt)
				{
					refresh_period = min_cont_refresh_period;
					Continue();
				}
				
				auto_reload = true;

				UnlockPost();

				if(halt)
				{
					response << "\t<head>" << std::endl;
					response << "\t\t<title>" << title << "</title>" << std::endl;
					response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
					response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
					response << "\t\t<script type=\"text/javascript\">document.domain=\"" << req.GetDomain() << "\";</script>" << std::endl;
					response << "\t</head>" << std::endl;
					response << "\t<body>" << std::endl;
					response << "\t<p>Disconnected</p>" << std::endl;
					response << "\t<script>Reload = function() { window.location.href=window.location.href; }</script>" << std::endl;
					response << "\t<button onclick=\"Reload()\">Reconnect</button>" << std::endl;
					response << "\t</body>" << std::endl;
				}
				else
				{
					// Post/Redirect/Get pattern: got Post, so do Redirect
					response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
					response.SetHeaderField("Location", form_action);
				}
				
				break;
			}
			
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			{
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				
				if(halt)
				{
					response << "\t<head>" << std::endl;
					response << "\t\t<title>" << title << "</title>" << std::endl;
					response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
					response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
					response << "\t\t<script type=\"text/javascript\">document.domain=\"" << req.GetDomain() << "\";</script>" << std::endl;
					if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
					{
						response << "\t\t<script>window.history.back();</script>" << std::endl;
					}
					response << "\t</head>" << std::endl;
					response << "\t<body>" << std::endl;
					response << "\t<p>Disconnected</p>" << std::endl;
					response << "\t<script>Reload = function() { window.location.href=window.location.href; }</script>" << std::endl;
					response << "\t<button onclick=\"Reload()\">Reconnect</button>" << std::endl;
					response << "\t</body>" << std::endl;
				}
				else
				{
					LockInstruments();
					
					response << "\t<head>" << std::endl;
					response << "\t\t<title>" << title << "</title>" << std::endl;
					response << "\t\t<meta name=\"description\" content=\"remote control interface over HTTP of virtual platform simulation instrumenter\">" << std::endl;
					response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
					response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
					response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
					response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/instrumenter/style.css\" type=\"text/css\" />" << std::endl;
					response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
					response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
					response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
					response << "\t\t<script>" << std::endl;
					response << "\t\t\tGUI.prototype.set_status = function()" << std::endl;
					response << "\t\t\t{" << std::endl;
					if(is_control_interface)
					{
						response << "\t\t\t\tvar status_item = gui.find_statusbar_item_by_name('status-" << GetName() << "');" << std::endl;
						response << "\t\t\t\tif(status_item)" << std::endl;
						response << "\t\t\t\t{" << std::endl;
						response << "\t\t\t\t\tvar child_nodes = status_item.div_element.childNodes;" << std::endl;
						response << "\t\t\t\t\tfor(var i = 0; i < child_nodes.length; i++)" << std::endl;
						response << "\t\t\t\t\t{" << std::endl;
						response << "\t\t\t\t\t\tvar child_node = child_nodes[i];" << std::endl;
						response << "\t\t\t\t\t\tif(child_node.className == 'state')" << std::endl;
						response << "\t\t\t\t\t\t{" << std::endl;
						response << "\t\t\t\t\t\t\tchild_node.innerHTML = '<span>" << (cont ? "running" : "paused") << "</span>';" << std::endl;
						response << "\t\t\t\t\t\t}" << std::endl;
						response << "\t\t\t\t\t\telse if(child_node.className == 'time')" << std::endl;
						response << "\t\t\t\t\t\t{" << std::endl;
						{
							std::ios_base::fmtflags ff = response.flags();
							response.setf(std::ios::fixed);
							response.precision(3);
							response << "\t\t\t\t\t\t\t\tchild_node.innerHTML = '<span>" << curr_time_stamp.to_seconds() << " seconds</span>';" << std::endl;
							response.flags(ff);
						}
						response << "\t\t\t\t\t\t}" << std::endl;
						response << "\t\t\t\t\t\telse if(child_node.className == 'time-exact')" << std::endl;
						response << "\t\t\t\t\t\t{" << std::endl;
						response << "\t\t\t\t\t\t\tchild_node.innerHTML = '<span>(" << curr_time_stamp << ")</span>';" << std::endl;
						response << "\t\t\t\t\t\t}" << std::endl;
						response << "\t\t\t\t\t}" << std::endl;
						response << "\t\t\t\t}" << std::endl;
					}
					response << "\t\t\t}" << std::endl;
					response << "\t\t</script>" << std::endl;
					response << "\t\t<script src=\"/unisim/service/instrumenter/script.js\"></script>" << std::endl;
					response << "\t</head>" << std::endl;
					
					response << "\t<body onload=\"";
					if(is_control_interface)
					{
						if(auto_reload)
						{
							response << "gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh');";
							if(is_instrument_interface)
							{
								// unified (control + instrument) view
								response << " gui.auto_reload_tab_by_uri('" << (URI() + "/control") << "', 0.1, 'self-refresh');";
							}
							else
							{
								// control view
								response << " gui.auto_reload_tab_by_uri('" << URI() << "', 0.1, 'self-refresh');";
							}
							
							response << " gui.auto_reload_tab_by_uri('" << (URI() + "/instrument") << "', 0.1, 'self-refresh-when-active');";
							
							if(refresh_period < max_cont_refresh_period)
							{
								refresh_period = std::min(2.0 * refresh_period, max_cont_refresh_period);
							}
							if(!cont)
							{
								auto_reload = false;
							}
						}
						else
						{
							response << "gui.auto_reload(0)";
						}
					}
					else if(is_instrument_interface)
					{
						response << "gui.auto_reload(0);";
						if(auto_reload)
						{
							response << " gui.auto_reload_tab_by_uri('" << URI() << "', 0.1, 'self-refresh-when-active');";
						}
					}
					else
					{
						response << "gui.auto_reload(0)";
					}
					response << "\">" << std::endl;
					
					if(is_control_interface)
					{
						response << "\t\t<table class=\"status-table\">" << std::endl;
						response << "\t\t\t<thead>" << std::endl;
						response << "\t\t\t\t<tr>" << std::endl;
						response << "\t\t\t\t\t<th class=\"status\">Status</th>" << std::endl;
						response << "\t\t\t\t\t<th class=\"time\">Time</th>" << std::endl;
						response << "\t\t\t\t\t<th class=\"time\">(exactly)</th>" << std::endl;
						response << "\t\t\t\t</tr>" << std::endl;
						response << "\t\t\t</thead>" << std::endl;
						response << "\t\t\t<tbody>" << std::endl;
						response << "\t\t\t\t<tr>" << std::endl;
						response << "\t\t\t\t\t<td class=\"status\">" << (cont ? "running" : "paused") << "</td>" << std::endl;
						
						{
							std::ios_base::fmtflags ff = response.flags();
							response.setf(std::ios::fixed);
							response.precision(3);
							response << "\t\t\t\t\t<td class=\"time\" title=\"Target time\">" << curr_time_stamp.to_seconds() << " seconds</td>" << std::endl;
							response.flags(ff);
						}
						
						response << "\t\t\t\t\t<td class=\"time\" title=\"Target time\">(" << unisim::util::hypapp::HTML_Encoder::Encode(curr_time_stamp.to_string()) << ")</td>" << std::endl;
						response << "\t\t\t\t</tr>" << std::endl;
						response << "\t\t\t</tbody>" << std::endl;
						response << "\t\t</table>" << std::endl;
					
						response << "\t\t<h2>Commands</h2>" << std::endl;
						response << "\t\t<table class=\"command-table\">" << std::endl;
						response << "\t\t\t<tbody>" << std::endl;
						response << "\t\t\t\t<tr>" << std::endl;
						response << "\t\t\t\t\t<td><form id=\"delta-step-form\" action=\"" << form_action << "\" method=\"post\"><button title=\"Step some delta cycles\" id=\"delta-step\" type=\"submit\" name=\"delta-step\" value=\"on\"" << ((cont || halt) ? " disabled" : "") << ">&delta;</button></form></td>" << std::endl;
						response << "\t\t\t\t\t<td><form id=\"timed-step-form\" action=\"" << form_action << "\" method=\"post\"><button title=\"Step by time\" id=\"timed-step\" type=\"submit\" name=\"timed-step\" value=\"on\"" << ((cont || halt) ? " disabled" : "") << ">Step</button>&nbsp;by&nbsp;<input class=\"step-time\" type=\"text\" spellcheck=\"false\" name=\"step-time\" value=\"" << user_step_time << "\"" << ((cont || halt) ? " disabled" : "") << "><input style=\"display:none;\" type=\"text\" name=\"curr-time-stamp\" value=\"" << curr_time_stamp << "\" readonly></form></td>" << std::endl;
						response << "\t\t\t\t\t<td><form id=\"" << (cont ? "intr" : "cont") << "-form\" action=\"" << form_action << "\" method=\"post\"><button title=\"" << (cont ? "Interrupt" : "Continue") << "\" id=\"" << (cont ? "intr" : "cont") << "\" type=\"submit\" name=\"" << (cont ? "intr" : "cont") << "\" value=\"on\"" << (halt ? " disabled" : "") << ">" << (cont ? "Interrupt" : "Continue") << "</button></form></td>" << std::endl;
						response << "\t\t\t\t\t<td><form id=\"halt-form\" action=\"" << form_action << "\" method=\"post\"><button title=\"Halt\" id=\"halt\" type=\"submit\" name=\"halt\" value=\"on\"" << (halt ? " disabled" : "")  << ">Halt</button></form></td>" << std::endl;
						response << "\t\t\t\t</tr>" << std::endl;
						response << "\t\t\t</tbody>" << std::endl;
						response << "\t\t</table>" << std::endl;
					}
					
					if(is_instrument_interface)
					{
						response << "\t\t<h2>Instruments</h2>" << std::endl;
						response << "\t\t<div class=\"instruments-table\">" << std::endl;
						response << "\t\t\t<div class=\"instruments-table-head\">" << std::endl;
						response << "\t\t\t\t<div class=\"signal-enable\">Enable<br><form action=\"" << form_action << "\" method=\"post\"><button title=\"Disable injection for all instruments\" class=\"signal-disable-all\" type=\"submit\" name=\"disable*all\" value=\"on\">C</button><button title=\"Enable injection for all instruments\" class=\"signal-enable-all\" type=\"submit\" name=\"enable*all\" value=\"on\">A</button></form></div>" << std::endl;
						response << "\t\t\t\t<div class=\"signal-brkpt-enable\">Brkpt<br><form action=\"" << form_action << "\" method=\"post\"><button title=\"Disable breakpoints for all instruments\" class=\"signal-brkpt-disable-all\" type=\"submit\" name=\"disable-brkpt*all\" value=\"on\">C</button><button title=\"Enable breakpoints for all instruments\" class=\"signal-brkpt-enable-all\" type=\"submit\" name=\"enable-brkpt*all\" value=\"on\">A</button></form></div>" << std::endl;
						response << "\t\t\t\t<div class=\"signal-name\">Hardware signal</div>" << std::endl;
						response << "\t\t\t\t<div class=\"signal-toggle\">Toggle</div>" << std::endl;
						response << "\t\t\t\t<div class=\"signal-value\">Value</div>" << std::endl;
						response << "\t\t\t\t<div class=\"scrollbar\"></div>" << std::endl;
						response << "\t\t\t</div>" << std::endl;
						response << "\t\t\t<div class=\"instruments-table-body scroller\">" << std::endl;

						std::stringstream sstr(instrumentation);
						
						std::vector<std::string>::iterator instrumented_signal_name_it;
						for(instrumented_signal_name_it = instrumented_signal_names.begin(); instrumented_signal_name_it != instrumented_signal_names.end(); instrumented_signal_name_it++)
						{
							const std::string& instrumented_signal_name = *instrumented_signal_name_it;

							UserInstrument *user_instrument = FindUserInstrument(instrumented_signal_name);
							
							if(user_instrument)
							{
								std::string value;
								user_instrument->Get(value);
								bool is_boolean = user_instrument->IsBoolean();
								bool bool_value = false;
								if(is_boolean) user_instrument->Get(bool_value);
								response << "\t\t\t\t<div class=\"signal" << (user_instrument->HasBreakpointCondition() ? " brkpt-cond" : "") << "\">" << std::endl;
								response << "\t\t\t\t\t<div class=\"signal-enable\"><form action=\"" << form_action << "\" method=\"post\"><button title=\"Enable/Disable injection\" class=\"signal-enable" << (user_instrument->IsInjectionEnabled() ? " checked" : " unchecked") << "\" type=\"submit\" name=\"" << (user_instrument->IsInjectionEnabled() ? "disable" : "enable") << "*" << unisim::util::hypapp::HTML_Encoder::Encode(user_instrument->GetName()) << "\"" << ((cont || halt || user_instrument->IsReadOnly()) ? " disabled" : "") << "></button></form></div>" << std::endl;
								response << "\t\t\t\t\t<div class=\"signal-brkpt-enable\"><form action=\"" << form_action << "\" method=\"post\"><button title=\"Enable/Disable breakpoint\" class=\"signal-brkpt-enable" << (user_instrument->IsValueChangedBreakpointEnabled() ? " checked" : " unchecked") << ((cont || halt) ? " disabled" : "") << "\" type=\"submit\" name=\"" << (user_instrument->IsValueChangedBreakpointEnabled() ? "disable" : "enable") << "-brkpt*" << unisim::util::hypapp::HTML_Encoder::Encode(user_instrument->GetName()) << "\"" << ((cont || halt) ? " disabled" : "") << "></button></form></div>" << std::endl;
								response << "\t\t\t\t\t<div class=\"signal-name\">" << unisim::util::hypapp::HTML_Encoder::Encode(user_instrument->GetName()) << "</div>" << std::endl;
								response << "\t\t\t\t\t<div class=\"signal-toggle\">";
								if(is_boolean)
								{
									response << "<form action=\"" << form_action << "\" method=\"post\"><button title=\"Click to toggle\" class=\"signal-toggle-button signal-" << (bool_value ? "on" : "off") << "\" type=\"submit\" name=\"toggle*" << unisim::util::hypapp::HTML_Encoder::Encode(user_instrument->GetName()) << "\"" << ((cont || halt || user_instrument->IsReadOnly()) ? " disabled" : "") << ">" << (bool_value ? "on" : "off")  << "</button></form>";
								}
								response << "</div>" << std::endl;
								response << "\t\t\t\t\t\t<div class=\"signal-value\"><form action=\"" << form_action << "\" method=\"post\"><input title=\"Type a value then press enter\" class=\"signal-value-text" << (user_instrument->IsReadOnly() ? " disabled" : "") << "\" type=\"text\" spellcheck=\"false\" name=\"set*" << unisim::util::hypapp::HTML_Encoder::Encode(user_instrument->GetName()) << "\" value=\"" << unisim::util::hypapp::HTML_Encoder::Encode(value) << "\"" << ((cont || halt || user_instrument->IsReadOnly()) ? " disabled" : "") << "></form></div>" << std::endl;
								response << "\t\t\t\t</div>" << std::endl;
							}
						}

						
						response << "\t\t\t</div>" << std::endl;
						response << "\t\t</div>" << std::endl;
					}
					
					response << "\t</body>" << std::endl;
					
					UnlockInstruments();
				}
					
				response << "</html>" << std::endl;
				response << std::endl;
				
				break;
			}
			
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, POST, GET, HEAD");
				break;
				
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, POST, GET, HEAD");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				
				break;
		}
	}
	else
	{
		response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
		
		response << "<!DOCTYPE html>" << std::endl;
		response << "<html lang=\"en\">" << std::endl;
		response << "\t<head>" << std::endl;
		response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::NOT_FOUND << " (" << unisim::util::hypapp::HttpResponse::NOT_FOUND << ")</title>" << std::endl;
		response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::NOT_FOUND << " (" << unisim::util::hypapp::HttpResponse::NOT_FOUND << ")\">" << std::endl;
		response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
		response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << "\t\t<style>" << std::endl;
		response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
		response << "\t\t</style>" << std::endl;
		response << "\t</head>" << std::endl;
		response << "\t<body>" << std::endl;
		response << "\t\t<p>Unavailable</p>" << std::endl;
		response << "\t</body>" << std::endl;
		response << "</html>" << std::endl;
	}
	
	bool send_status = conn.Send(response.ToString(req.GetRequestType() == unisim::util::hypapp::Request::HEAD));

	if(send_status)
	{
		if(verbose)
		{
			logger << DebugInfo << "sending HTTP response: done" << EndDebugInfo;
		}
	}
	else
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response" << EndDebugWarning;
	}
	
	if(halt)
	{
		Stop(-1, /* asynchronous */ true);
	}
	
	return send_status;
}

void UserInterface::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
	scanner.Append(unisim::service::interfaces::JSFile(URI() + "/toolbar_actions.js"));
	
	scanner.Append(unisim::service::interfaces::CSSFile("/unisim/service/instrumenter/status_style.css"));
	
	scanner.Append(unisim::service::interfaces::ToolbarOpenTabAction(
		/* name */      GetName(), 
		/* label */     "<img src=\"/unisim/service/instrumenter/icon.svg\">",
		/* tips */      "Signal instrumenter",
		/* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
		/* uri */       URI() + "/instrument"
	));
	
	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("cont-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/instrumenter/icon_cont.svg\">",
		/* tips */      "Continue",
		/* js action */ "return gui && gui.instrumenter_toolbar_actions && gui.instrumenter_toolbar_actions.execute('cont')"
	));
	
	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("intr-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/instrumenter/icon_intr.svg\">",
		/* tips */      "Interrupt",
		/* js action */ "return gui && gui.instrumenter_toolbar_actions && gui.instrumenter_toolbar_actions.execute('intr')"
	));
	
	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("halt-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/instrumenter/icon_halt.svg\">",
		/* tips */      "Halt",
		/* js action */ "return gui && gui.instrumenter_toolbar_actions && gui.instrumenter_toolbar_actions.execute('halt')"
	));

	std::ostringstream status_sstr;
	status_sstr << "<div class=\"name\"><span>Simulation:</span></div>";
	status_sstr << "<div class=\"state\" title=\"click play HW/interrupt HW buttons in toolbar to change simulation state\"></div>";
	status_sstr << "<div class=\"time\"></div>";
	status_sstr << "<div class=\"time-exact\"></div>";
	std::string status(status_sstr.str());
	
	scanner.Append(unisim::service::interfaces::StatusBarItem(
		/* name */       std::string("status-") + GetName(),
		/* class name */ "instrumenter-status",
		/* HTML */       status
	));
}

UserInstrument *UserInterface::FindUserInstrument(const std::string& name)
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it = user_instruments.find(name);
	
	if(user_instrument_it != user_instruments.end())
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		return user_instrument;
	}
	
	return 0;
}

void UserInterface::EnableInjection()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->EnableInjection();
	}
}

void UserInterface::DisableInjection()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->DisableInjection();
	}
}

void UserInterface::EnableValueChangedBreakpoint()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->EnableValueChangedBreakpoint();
	}
}

void UserInterface::DisableValueChangedBreakpoint()
{
	std::map<std::string, UserInstrument *>::iterator user_instrument_it;
	
	for(user_instrument_it = user_instruments.begin(); user_instrument_it != user_instruments.end(); user_instrument_it++)
	{
		UserInstrument *user_instrument = (*user_instrument_it).second;
		
		user_instrument->DisableValueChangedBreakpoint();
	}
}

void UserInterface::PropertySetter::UserInstrumentCommand::Apply(PropertySetter& property_setter) const
{
	UserInterface& user_interface = property_setter.user_interface;
	
	switch(type)
	{
		case NOP:
			break;
		case ENABLE_ALL:
			user_interface.EnableInjection();
			property_setter.delta_steps = std::max(3u, property_setter.delta_steps);
			break;
		case DISABLE_ALL:
			user_interface.DisableInjection();
			property_setter.delta_steps = std::max(3u, property_setter.delta_steps);
			break;
		case ENABLE_ALL_VALUE_CHANGED_BREAKPOINTS:
			user_interface.EnableValueChangedBreakpoint();
			break;
		case DISABLE_ALL_VALUE_CHANGED_BREAKPOINTS:
			user_interface.DisableValueChangedBreakpoint();
			break;
		case ENABLE:
			user_instrument->EnableInjection();
			property_setter.delta_steps = std::max(3u, property_setter.delta_steps);
			break;
		case DISABLE:
			user_instrument->DisableInjection();
			property_setter.delta_steps = std::max(3u, property_setter.delta_steps);
			break;
		case ENABLE_VALUE_CHANGED_BRKPT:
			user_instrument->EnableValueChangedBreakpoint();
			break;
		case DISABLE_VALUE_CHANGED_BRKPT:
			user_instrument->DisableValueChangedBreakpoint();
			break;
		case SET:
			user_instrument->Set(value);
			property_setter.delta_steps = std::max(3u, property_setter.delta_steps);
			break;
		case TOGGLE:
			user_instrument->Toggle();
			property_setter.delta_steps = std::max(3u, property_setter.delta_steps);
			break;
	}
}

UserInterface::PropertySetter::PropertySetter(UserInterface& _user_interface)
	: user_interface(_user_interface)
	, curr_time_stamp()
	, valid_curr_time_stamp(false)
	, user_step_time()
	, valid_user_step_time(false)
	, timed_step(false)
	, delta_steps(0)
	, cont(false)
	, intr(false)
	, halt(false)
	, user_instrument_commands()
{
}
	
bool UserInterface::PropertySetter::FormAssign(const std::string& name, const std::string& value)
{
	if(name.compare("curr-time-stamp") == 0)
	{
		std::istringstream curr_time_stamp_sstr(value);
		valid_curr_time_stamp = !(curr_time_stamp_sstr >> curr_time_stamp).bad()/* && curr_time_stamp_sstr.eof()*/;
		
		if(!valid_curr_time_stamp)
		{
			user_interface.logger << DebugWarning << "expecting a time in curr-time-stamp, got '" << value << "'" << EndDebugWarning;
			return false;
		}
	}
	else if(name.compare("step-time") == 0)
	{
		std::istringstream value_sstr(value);
		valid_user_step_time = !(value_sstr >> user_step_time).bad()/* && !value_sstr.eof()*/;
		
		if(!valid_user_step_time)
		{
			user_interface.logger << DebugWarning << "expecting a time in time-step, got '" << value << "'" << EndDebugWarning;
			return false;
		}
	}
	else if((name.compare("timed-step") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Timed Step" << EndDebugInfo;
		}
		timed_step = true;
	}
	else if((name.compare("delta-step") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Delta Step" << EndDebugInfo;
		}
		delta_steps = std::max(3u, delta_steps);
	}
	else if((name.compare("cont") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Continue" << EndDebugInfo;
		}
		cont = true;
	}
	else if((name.compare("intr") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Interrupt" << EndDebugInfo;
		}
		intr = true;
	}
	else if((name.compare("halt") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Halt" << EndDebugInfo;
		}
		halt = true;
	}
	else
	{
		std::size_t delim_pos = name.find_first_of('*');
		if(delim_pos != std::string::npos)
		{
			std::string user_instrument_action = name.substr(0, delim_pos);
			std::string user_instrument_name = name.substr(delim_pos + 1);
			
			if(user_instrument_name == "all")
			{
				if(user_instrument_action == "enable")
				{
					user_instrument_commands.push_back(UserInstrumentCommand(ENABLE_ALL));
				}
				else if(user_instrument_action == "disable")
				{
					user_instrument_commands.push_back(UserInstrumentCommand(DISABLE_ALL));
				}
				else if(user_instrument_action == "enable-brkpt")
				{
					user_instrument_commands.push_back(UserInstrumentCommand(ENABLE_ALL_VALUE_CHANGED_BREAKPOINTS));
				}
				else if(user_instrument_action == "disable-brkpt")
				{
					user_instrument_commands.push_back(UserInstrumentCommand(DISABLE_ALL_VALUE_CHANGED_BREAKPOINTS));
				}
			}
			else
			{
				UserInstrument *user_instrument = user_interface.FindUserInstrument(user_instrument_name);
				
				if(user_instrument)
				{
					if(user_instrument_action == "enable")
					{
						user_instrument_commands.push_back(UserInstrumentCommand(ENABLE, user_instrument));
					}
					else if(user_instrument_action == "disable")
					{
						user_instrument_commands.push_back(UserInstrumentCommand(DISABLE, user_instrument));
					}
					else if(user_instrument_action == "enable-brkpt")
					{
						user_instrument_commands.push_back(UserInstrumentCommand(ENABLE_VALUE_CHANGED_BRKPT, user_instrument));
					}
					else if(user_instrument_action == "disable-brkpt")
					{
						user_instrument_commands.push_back(UserInstrumentCommand(DISABLE_VALUE_CHANGED_BRKPT, user_instrument));
					}
					else if(user_instrument_action == "set")
					{
						user_instrument_commands.push_back(UserInstrumentCommand(SET, user_instrument, value));
					}
					else if(user_instrument_action == "toggle")
					{
						user_instrument_commands.push_back(UserInstrumentCommand(TOGGLE, user_instrument));
					}
					else
					{
						user_interface.logger << DebugWarning << "unknown Action \"" << user_instrument_action << "\" on User instrument \"" << user_instrument_name << "\"" << EndDebugWarning;
						return false;
					}
				}
				else
				{
					user_interface.logger << DebugWarning << "Can't find User instrument \"" << user_instrument_name << "\"" << EndDebugWarning;
					return false;
				}
			}
		}
	}
	
	return true;
}

void UserInterface::PropertySetter::Apply()
{
	for(UserInstrumentCommands::const_iterator it = user_instrument_commands.begin(); it != user_instrument_commands.end(); it++)
	{
		const UserInstrumentCommand& user_instrument_command = *it;
		user_instrument_command.Apply(*this);
	}
	if(valid_user_step_time)
	{
		user_interface.user_step_time = user_step_time;
	}
	
	if(timed_step)
	{
		if(valid_user_step_time && valid_curr_time_stamp)
		{
			sc_core::sc_time deadline(curr_time_stamp);
			deadline += user_step_time;
			if(user_interface.curr_time_stamp < deadline)
			{
				cont = true;
				user_interface.schedule.insert(deadline);
			}
			else
			{
				user_interface.intr = true;
			}
		}
		else
		{
			cont = false;
		}
	}
	else if(delta_steps)
	{
		user_interface.delta_steps = delta_steps;
		cont = true;
	}
	if(intr)
	{
		user_interface.intr = true;
	}
	if(halt)
	{
		user_interface.halt = true;
	}
}

CSV_Reader::CSV_Reader(const char *name, Instrumenter *instrumenter)
	: unisim::kernel::Object(name, instrumenter, "Hardware instrument .CSV file reader")
	, InstrumenterFrontEnd(name, instrumenter)
	, logger(*this)
	, filename()
	, param_filename("filename", this, filename, "name of CSV input file")
	, instrumentation()
	, param_instrumentation("instrumentation", this, instrumentation, "Instrumented signals (wildcards '*' and '?' are allowed in signal names) that take there input from the CSV file")
	, instrumentation_start_time(sc_core::SC_ZERO_TIME)
	, param_instrumentation_start_time("instrumentation-start-time", this, instrumentation_start_time, "Start time of instrumentation")
	, instrumentation_end_time(sc_core::sc_max_time())
	, param_instrumentation_end_time("instrumentation-end-time", this, instrumentation_end_time, "End time of instrumentation")
	, csv_delimiter(",")
	, param_csv_delimiter("csv-delimiter", this, csv_delimiter, "delimiter in CSV file")
	, lineno(1)
	, file(0)
	, time_resolution(sc_core::SC_ZERO_TIME)
	, csv_instrument_names()
	, csv_instrument_map()
	, csv_time_stamp(sc_core::SC_ZERO_TIME)
{
	file = new std::ifstream(filename.c_str());
		
	if(file->fail())
	{
		delete file;
		file = 0;
		logger << DebugWarning << "Can't open File \"" << filename << "\"" << EndDebugWarning;
		return;
	}
	
	if(!ParseCSVHeaderAndInstrumentInput())
	{
		delete file;
		file = 0;
	}
}

CSV_Reader::~CSV_Reader()
{
	if(file) delete file;
}

bool CSV_Reader::SetupInstrumentation()
{
	if(!file) return false;
	
	csv_instrument_map.resize(csv_instrument_names.size());
	
	for(unsigned int i = 0; i < csv_instrument_names.size(); i++)
	{
		const std::string& csv_instrument_name = csv_instrument_names[i];
		InputInstrumentBase *input_instrument = InstrumentInputSignal(csv_instrument_name);
		//std::cerr << "CSV column " << csv_column << " is for instrument " << (input_instrument ? input_instrument->GetName() : "unknown") << std::endl;
		
		if(!input_instrument)
		{
			logger << DebugWarning << "Input Signal \"" << csv_instrument_name << "\" can't be instrumented or does not exists" << EndDebugWarning;
		}
		
		csv_instrument_map[i] = input_instrument;
	}
	
	do
	{
		if(!ParseCSV(csv_time_stamp)) return false;
	}
	while(csv_time_stamp < instrumentation_start_time);
	
	instrumentation_start_time = csv_time_stamp; // crop interval
	
	return true;
}

void CSV_Reader::ProcessInputInstruments()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if((time_stamp >= instrumentation_start_time) && (time_stamp <= instrumentation_end_time))
	{
		if(csv_time_stamp == time_stamp)
		{
			unsigned int n = csv_instrument_map.size();
			unsigned int i;
			
			for(i = 0; i < n; i++)
			{
				InputInstrumentBase *input_instrument = csv_instrument_map[i];
				
				//std::cerr << "#" << i << ": Instrument " << (input_instrument ? input_instrument->GetName() : "unknown") << std::endl;
				if(input_instrument)
				{
					input_instrument->Inject();
				}
			}
		}
	
		if(ParseCSV(csv_time_stamp) && (csv_time_stamp >= time_stamp))
		{
			sc_core::sc_time time_to_next_input_instrumentation(csv_time_stamp);
			time_to_next_input_instrumentation -= time_stamp;
			NextInputTrigger(time_to_next_input_instrumentation);
		}
		else
		{
			instrumentation_end_time = time_stamp; // stop instrumentation as there're no more data
		}
	}
}

void CSV_Reader::ProcessOutputInstruments()
{
}

bool CSV_Reader::ParseCSVHeaderAndInstrumentInput()
{
	if(file)
	{
		std::string line;
		
		if(std::getline(*file, line))
		{
			unsigned int csv_column = 0;
			std::size_t pos = 0;
			std::size_t delim_pos = 0;
			
			do
			{
				delim_pos = line.find(csv_delimiter, pos);
				
				std::string value;
				if(delim_pos != std::string::npos)
				{
					value = line.substr(pos, delim_pos - pos);
				}
				else
				{
					value = line.substr(pos);
				}
				
				//std::cerr << "value is \"" << value << "\"" << std::endl;
				if(csv_column)
				{
					PrepareInstrumentation(value, INPUT_INSTRUMENTATION);
					csv_instrument_names.push_back(value);
				}
				else
				{
					// first column is time unit
					std::stringstream value_sstr(value);
					double time_value;
					std::string time_unit;
					
					if((value_sstr >> time_value) && (value_sstr >> time_unit))
					{
						if(time_unit.compare("s") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_SEC);
						else if(time_unit.compare("ms") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_MS);
						else if(time_unit.compare("us") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_US);
						else if(time_unit.compare("ns") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_NS);
						else if(time_unit.compare("ps") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_PS);
						else if(time_unit.compare("fs") == 0) time_resolution = sc_core::sc_time(time_value, sc_core::SC_FS);
						else
						{
							logger << DebugWarning << "In input CSV file, first line, first column, malformed time_unit ('" << time_unit << "'): expecting 's', 'ms', 'us', 'ns', 'ps', or 'fs'." << EndDebugWarning;
							return false;
						}
					}
					else
					{
						logger << DebugWarning << "expecting a time resolution in first line, first column (e.g. 1 ps) of input CSV file" << EndDebugWarning;
						return false;
					}
					
					if(!value_sstr.eof())
					{
						logger << DebugWarning << "ignoring extra characters after time resolution in first line, first column of input CSV file" << EndDebugWarning;
					}
				}
				
				pos = delim_pos + csv_delimiter.length(); // skip value and delimiter
				csv_column++;
			}
			while(delim_pos != std::string::npos);
			lineno++;
			
			return true;
		}
	}
	
	return false;
}

bool CSV_Reader::ParseCSV(sc_core::sc_time& deadline)
{
	if(file)
	{
		std::string line;
		
		if(std::getline(*file, line))
		{
			unsigned int csv_column = 0;
			std::size_t pos = 0;
			std::size_t delim_pos = 0;
			
			do
			{
				delim_pos = line.find(csv_delimiter, pos);
				
				std::string value;
				if(delim_pos != std::string::npos)
				{
					value = line.substr(pos, delim_pos - pos);
				}
				else
				{
					value = line.substr(pos);
				}
				
				//std::cerr << "value is \"" << value << "\"" << std::endl;
				if(csv_column)
				{
					if((csv_column - 1) < csv_instrument_map.size())
					{
						InputInstrumentBase *input_instrument = csv_instrument_map[csv_column - 1];
						
						if(input_instrument)
						{
							//std::cerr << "value is for " << input_instrument->GetName() << std::endl;
							std::stringstream value_sstr(value);
							input_instrument->Input(value_sstr);
						}
					}
					else
					{
						logger << DebugWarning << "At Line #" << lineno << ", ignoring Column #" << csv_column << EndDebugWarning;
					}
				}
				else
				{
					std::stringstream value_sstr(value);
					sc_dt::uint64 time_value;
					
					if(value_sstr >> time_value)
					{
						deadline = time_resolution;
						deadline *= time_value;
					}
					else
					{
						deadline = sc_core::sc_max_time();
						return false;
					}
				}
				
				pos = delim_pos + csv_delimiter.length(); // skip value and delimiter
				csv_column++;
			}
			while(delim_pos != std::string::npos);
			
			lineno++;
			return true;
		}
	}
	return false;
}

CSV_Writer::CSV_Writer(const char *name, Instrumenter *instrumenter)
	: unisim::kernel::Object(name, instrumenter, "Hardware instrumenter .CSV file writer")
	, InstrumenterFrontEnd(name, instrumenter)
	, logger(*this)
	, filename()
	, param_filename("filename", this, filename, "name of CSV output file")
	, instrumentation()
	, param_instrumentation("instrumentation", this, instrumentation, "Instrumented signals (wildcards '*' and '?' are allowed in signal names) to record in the CSV file")
	, instrumentation_start_time(sc_core::SC_ZERO_TIME)
	, param_instrumentation_start_time("instrumentation-start-time", this, instrumentation_start_time, "Start time of instrumentation")
	, instrumentation_end_time(sc_core::sc_max_time())
	, param_instrumentation_end_time("instrumentation-end-time", this, instrumentation_end_time, "End time of instrumentation")
	, csv_delimiter(",")
	, param_csv_delimiter("csv-delimiter", this, csv_delimiter, "delimiter in CSV file")
	, file(0)
	, last_instrument_sampling_time_stamp(sc_core::SC_ZERO_TIME)
	, force_output(true)
{
	std::stringstream sstr(instrumentation);
	
	std::string signal_name_pattern;
	while(sstr >> signal_name_pattern)
	{
		PrepareInstrumentation(signal_name_pattern, OUTPUT_INSTRUMENTATION);
	}
}

CSV_Writer::~CSV_Writer()
{
	if(file)
	{
		const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
		if((time_stamp >= instrumentation_start_time) && (time_stamp <= instrumentation_end_time))
		{
			if(time_stamp > last_instrument_sampling_time_stamp)
			{
				OutputInstrumentsAsCSV(*file, last_instrument_sampling_time_stamp);
			}
		}
		
		delete file;
	}
}

bool CSV_Writer::SetupInstrumentation()
{
	file = new std::ofstream(filename.c_str());
	
	if(file->fail())
	{
		file = 0;
		logger << DebugWarning << "Can't open File \"" << filename << "\"" << EndDebugWarning;
		return false;
	}
	
	std::stringstream sstr(instrumentation);
	std::string signal_name_pattern;

	while(sstr >> signal_name_pattern)
	{
		std::vector<std::string> instrumented_signal_names;
		MatchSignalPattern(signal_name_pattern, instrumented_signal_names);
		
		std::vector<std::string>::iterator instrumented_signal_name_it;
		for(instrumented_signal_name_it = instrumented_signal_names.begin(); instrumented_signal_name_it != instrumented_signal_names.end(); instrumented_signal_name_it++)
		{
			const std::string& instrumented_signal_name = *instrumented_signal_name_it;
		
			InstrumentOutputSignal(instrumented_signal_name);
		}
	}
	
	last_instrument_sampling_time_stamp = sc_core::SC_ZERO_TIME;
	
	OutputInstrumentsNamesAsCSV(*file);
	
	return true;
}

void CSV_Writer::ProcessInputInstruments()
{
}

void CSV_Writer::ProcessOutputInstruments()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if((time_stamp >= instrumentation_start_time) && (time_stamp <= instrumentation_end_time))
	{
		if(time_stamp > last_instrument_sampling_time_stamp)
		{
			OutputInstrumentsAsCSV(*file, last_instrument_sampling_time_stamp);
			LatchInstruments();
		}
		
		SampleInstruments();
		last_instrument_sampling_time_stamp = time_stamp;
	}
}

void CSV_Writer::SampleInstruments()
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		output_instrument->Sample();
	}
}

void CSV_Writer::LatchInstruments()
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		output_instrument->Latch();
	}
}

void CSV_Writer::OutputInstrumentsNamesAsCSV(std::ostream& os)
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;

	os << sc_core::sc_get_time_resolution();
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		os << csv_delimiter << output_instrument->GetName();
	}
	os << std::endl;
}

void CSV_Writer::OutputInstrumentsAsCSV(std::ostream& os, const sc_core::sc_time& time_stamp)
{
	const std::vector<OutputInstrumentBase *>& output_instruments = GetOutputInstruments();
	unsigned int n = output_instruments.size();
	unsigned int i;
	
	os << last_instrument_sampling_time_stamp.value();
	for(i = 0; i < n; i++)
	{
		OutputInstrumentBase *output_instrument = output_instruments[i];
		
		os << csv_delimiter;
		if(force_output || output_instrument->ValueChanged())
		{
			output_instrument->Output(os);
		}
	}
	force_output = false;
	os << std::endl;
}

} // end of namespace instrumenter
} // end of namespace service
} // end of namespace unisim
