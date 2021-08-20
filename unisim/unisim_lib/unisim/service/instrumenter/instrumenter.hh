/*
 *  Copyright (c) 2015-2016,
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

#ifndef __UNISIM_SERVICE_INSTRUMENTER_INSTRUMENTER_HH__
#define __UNISIM_SERVICE_INSTRUMENTER_INSTRUMENTER_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/kernel/variable/sc_time/sc_time.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/nat_sort/nat_sort.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <systemc>
#include <tlm>
#include <stdexcept>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>

#include <unisim/util/hypapp/hypapp.hh>
#include <unisim/util/arithmetic/arithmetic.hh>

namespace sc_core {
inline std::ostream& operator << (std::ostream& os, const sc_writer_policy& pol)
{
	switch(pol)
	{
		case SC_ONE_WRITER: os << "SC_ONE_WRITER"; break;
		case SC_MANY_WRITERS: os << "SC_MANY_WRITERS"; break;
		case SC_UNCHECKED_WRITERS: os << "SC_UNCHECKED_WRITERS"; break;
	}
	return os;
}

}

namespace unisim {
namespace service {
namespace instrumenter {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

class SignalTeeBase
{
public:
	SignalTeeBase(const std::string& name);
	virtual ~SignalTeeBase();
	virtual void Initialize() = 0;
	void EnableInjection(unsigned int front_end_id);
	void DisableInjection(unsigned int front_end_id);
	inline bool IsInjectionEnabled(unsigned int front_end_id) const;
	inline bool Select(unsigned int& n) const;
protected:
	sc_core::sc_event enable_injection_event;
	sc_core::sc_spawn_options process_spawn_options;
private:
	uint64_t enable_mask;
	std::string name;
};

inline bool SignalTeeBase::IsInjectionEnabled(unsigned int front_end_id) const
{
	return (enable_mask & (uint64_t(1) << front_end_id)) != 0;
}

inline bool SignalTeeBase::Select(unsigned int& n) const
{
	return unisim::util::arithmetic::BitScanForward(n, enable_mask);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
class SignalTee : public SignalTeeBase
{
public:
	SignalTee(const std::string& name, sc_core::sc_signal<T, WRITER_POLICY> *original_signal, sc_core::sc_signal<T, WRITER_POLICY> *signal);
	
	virtual void Initialize();
	
	sc_core::sc_signal<T, WRITER_POLICY> *GetOriginalSignal();
	void SetInjectedSignal(unsigned int front_end_id, sc_core::sc_signal<T, WRITER_POLICY> *injected_signal);
	
	void Process();
private:
	sc_core::sc_signal<T, WRITER_POLICY> *original_signal;
	std::vector<sc_core::sc_signal<T, WRITER_POLICY> *> injected_signals;
	sc_core::sc_signal<T, WRITER_POLICY> *signal;
};

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
SignalTee<T, WRITER_POLICY>::SignalTee(const std::string& _name, sc_core::sc_signal<T, WRITER_POLICY> *_original_signal, sc_core::sc_signal<T, WRITER_POLICY> *_signal)
	: SignalTeeBase(_name)
	, original_signal(_original_signal)
	, signal(_signal)
{
	process_spawn_options.set_sensitivity(original_signal);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void SignalTee<T, WRITER_POLICY>::Initialize()
{
	sc_core::sc_spawn(sc_bind(&SignalTee<T, WRITER_POLICY>::Process, this), sc_core::sc_gen_unique_name("signal_tee_process"), &process_spawn_options);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY> *SignalTee<T, WRITER_POLICY>::GetOriginalSignal()
{
	return original_signal;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void SignalTee<T, WRITER_POLICY>::SetInjectedSignal(unsigned int front_end_id, sc_core::sc_signal<T, WRITER_POLICY> *injected_signal)
{
	if(injected_signals.size() <= front_end_id)
	{
		injected_signals.resize(front_end_id + 1);
	}
	
	if(!injected_signals[front_end_id])
	{
		injected_signals[front_end_id] = injected_signal;
		process_spawn_options.set_sensitivity(injected_signal);
	}
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void SignalTee<T, WRITER_POLICY>::Process()
{
	T value;
	
	unsigned int n = 0;
	if(Select(n))
	{
#if 0
		std::cerr << "SignalTee<T, WRITER_POLICY>::Process(): At " << sc_core::sc_time_stamp() << ", Inject(" << n << "): " << signal->name() << " <- '" << injected_signals[n]->read() << "'" << std::endl;
#endif
		value = injected_signals[n]->read();
	}
	else
	{
#if 0
		std::cerr << "SignalTee<T, WRITER_POLICY>::Process(): At " << sc_core::sc_time_stamp() << ", Passthrough: " << signal->name() << " <- '" << original_signal->read() << "'" << std::endl;
#endif
		value = original_signal->read();
	}
	
	signal->write(value);
}

enum INSTRUMENTATION_TYPE
{
	NO_INSTRUMENTATION           = 0,
	INPUT_INSTRUMENTATION        = 1,
	OUTPUT_INSTRUMENTATION       = 2,
	INPUT_OUTPUT_INSTRUMENTATION = INPUT_INSTRUMENTATION | OUTPUT_INSTRUMENTATION
};

inline std::ostream& operator << (std::ostream& os, const INSTRUMENTATION_TYPE& instrumentation_type)
{
	switch(instrumentation_type)
	{
		case NO_INSTRUMENTATION          : os << "no instrumentation"; break;
		case INPUT_INSTRUMENTATION       : os << "input instrumentation"; break;
		case OUTPUT_INSTRUMENTATION      : os << "output instrumentation"; break;
		case INPUT_OUTPUT_INSTRUMENTATION: os << "input/output instrumentation"; break;
		default                          : os << "?"; break;
	}
	
	return os;
}

struct InstrumentKey
{
	std::string name;
	INSTRUMENTATION_TYPE instrumentation_type;
	unsigned int front_end_id;

	InstrumentKey(const std::string& _name, INSTRUMENTATION_TYPE _instrumentation_type, unsigned int _front_end_id) : name(_name), instrumentation_type(_instrumentation_type), front_end_id(_front_end_id) {}
	int operator < (const InstrumentKey& key) const { return (name.compare(key.name) < 0) || ((name.compare(key.name) == 0) && ((instrumentation_type < key.instrumentation_type) || ((instrumentation_type == key.instrumentation_type) && (front_end_id < key.front_end_id)))); }
};

class InstrumentBase
{
public:
	inline InstrumentBase(const std::string& name, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id);
	inline virtual ~InstrumentBase();
	
	inline const std::string& GetName() const;
	inline INSTRUMENTATION_TYPE GetInstrumentationType() const;
	inline unsigned int GetFrontEndId() const;
	inline const InstrumentKey& GetKey() const;
	
	virtual const std::type_info& GetTypeInfo() const = 0;
private:
	InstrumentKey key;
};

class InputInstrumentBase : public InstrumentBase
{
public:
	InputInstrumentBase(const std::string& name, unsigned int front_end_id, SignalTeeBase *_signal_tee);
	virtual void Inject() = 0;
	virtual void Input(std::istream& is) = 0;
	void EnableInjection();
	void DisableInjection();
	inline bool IsInjectionEnabled() const;
	virtual void Print(std::ostream& os) const = 0;
private:
	SignalTeeBase *signal_tee;
};

class OutputInstrumentBase : public InstrumentBase
{
public:
	OutputInstrumentBase(const std::string& name, unsigned int front_end_id);
	virtual sc_core::sc_interface *GetInterface() = 0;
	virtual void Sample() = 0;
	virtual void Latch() = 0;
	virtual bool ValueChanged() const = 0;
	virtual void Output(std::ostream& os) = 0;
};

template <typename T, sc_core::sc_writer_policy WRITER_POLICY = sc_core::SC_ONE_WRITER>
class InputInstrument : public InputInstrumentBase
{
public:
	InputInstrument(const std::string& name, unsigned int front_end_id, sc_core::sc_signal<T, WRITER_POLICY> *signal, SignalTee<T, WRITER_POLICY> *signal_tee, const T& init_value);
	
	virtual void Inject();
	virtual void Input(std::istream& is);
	virtual const std::type_info& GetTypeInfo() const;
	virtual void Print(std::ostream& os) const;
private:
	sc_core::sc_signal<T, WRITER_POLICY> *signal;
	T value;
	bool value_changed;
};

template <typename T, sc_core::sc_writer_policy WRITER_POLICY = sc_core::SC_ONE_WRITER>
class OutputInstrument : public OutputInstrumentBase
{
public:
	OutputInstrument(const std::string& name, unsigned int front_end_id, const sc_core::sc_signal<T, WRITER_POLICY> *signal, const T& init_value);
	
	virtual sc_core::sc_interface *GetInterface();
	virtual void Sample();
	virtual void Latch();
	virtual bool ValueChanged() const;
	virtual void Output(std::ostream& os);
	virtual const std::type_info& GetTypeInfo() const;
private:
	const sc_core::sc_signal<T, WRITER_POLICY> *signal;
	T latched_value;
	T sample_value;
};

class TyperBase
{
public:
	virtual ~TyperBase() {}
	virtual bool TryTraceSignal(const std::string& signal_name) = 0;
	virtual bool TryBind(const std::string& port_name, const std::string& signal_name) = 0;
};

class Instrumenter;

template <typename T, sc_core::sc_writer_policy WRITER_POLICY = sc_core::SC_ONE_WRITER>
class Typer : public TyperBase
{
public:
	Typer(Instrumenter *instrumenter);
	virtual bool TryTraceSignal(const std::string& signal_name);
	virtual bool TryBind(const std::string& port_name, const std::string& signal_name);
private:
	Instrumenter *instrumenter;
};

class InstrumenterFrontEnd : public virtual unisim::kernel::Object
{
public:
	InstrumenterFrontEnd(const char *name, Instrumenter *instrumenter);
	virtual ~InstrumenterFrontEnd();
	
	virtual bool SetupInstrumentation() = 0;
	virtual void ProcessInputInstruments() = 0;
	virtual void ProcessOutputInstruments() = 0;
	void MatchSignalPattern(const std::string& signal_name_pattern, std::vector<std::string>& vec_signal_names) const;
	void PrepareInstrumentation(const std::string& signal_name_pattern, INSTRUMENTATION_TYPE instrumentation_type);
	InputInstrumentBase *InstrumentInputSignal(const std::string& signal_name);
	OutputInstrumentBase *InstrumentOutputSignal(const std::string& signal_name);
	InputInstrumentBase *FindInputInstrument(const std::string& signal_name) const;
	const std::vector<InputInstrumentBase *>& GetInputInstruments() const;
	OutputInstrumentBase *FindOutputInstrument(const std::string& signal_name) const;
	const std::vector<OutputInstrumentBase *>& GetOutputInstruments() const;
	inline void NextInputTrigger(const sc_core::sc_time& t);
private:
	friend class Instrumenter;
	Instrumenter *instrumenter;
	unsigned int front_end_id;
	std::vector<InputInstrumentBase *> input_instruments;
	std::vector<OutputInstrumentBase *> output_instruments;
	std::map<std::string, InputInstrumentBase *> input_instruments_map;
	std::map<std::string, OutputInstrumentBase *> output_instruments_map;
	sc_core::sc_spawn_options output_instrumentation_process_spawn_options;
	sc_core::sc_event next_input_instrumentation_event;
	
	void Initialize();
	void OutputInstrumentationProcess();
	void InputInstrumentationProcess();
};

inline void InstrumenterFrontEnd::NextInputTrigger(const sc_core::sc_time& t)
{
	next_input_instrumentation_event.notify(t);
}

class UserInstrument
{
public:
	UserInstrument(const std::string& name, InputInstrumentBase *input_instrument, OutputInstrumentBase *output_instrument);
	
	const std::string& GetName() const;
	void Set(const std::string& value);
	void Get(std::string& value) const;
	void Set(bool value);
	void Get(bool& value) const;
	void Toggle();
	void InitialFetch();
	void Sample();
	void Latch();
	void Fetch();
	void Commit();
	void EnableInjection();
	void DisableInjection();
	bool IsInjectionEnabled() const;
	bool IsReadOnly() const;
	void EnableValueChangedBreakpoint();
	void DisableValueChangedBreakpoint();
	bool IsValueChangedBreakpointEnabled() const;
	void ClearBreakpointCondition();
	bool HasBreakpointCondition() const;
	bool IsBoolean() const;
	bool ValueChanged() const;
private:
	std::string name;
	mutable std::ostringstream sstr;
	std::string set_value;
	mutable std::string get_value;
	InputInstrumentBase *input_instrument;
	OutputInstrumentBase *output_instrument;
	bool enable;
	bool new_enable;
	bool value_changed_breakpoint;
	bool has_breakpoint_cond;
	mutable bool get_value_valid;
	
	void Update() const;
};

class UserInterface
	: public InstrumenterFrontEnd
	, public unisim::kernel::Service<unisim::service::interfaces::HttpServer>
{
public:
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
	UserInterface(const char *name, Instrumenter *instrumenter = 0);
	virtual ~UserInterface();
	
	virtual bool EndSetup();
	virtual void Kill();
	
	virtual bool SetupInstrumentation();
	virtual void ProcessInputInstruments();
	virtual void ProcessOutputInstruments();
	
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
	
	UserInstrument *FindUserInstrument(const std::string& name);
	
	void EnableInjection();
	void DisableInjection();
	void EnableValueChangedBreakpoint();
	void DisableValueChangedBreakpoint();
private:
	struct PropertySetter : public unisim::util::hypapp::Form_URL_Encoded_Decoder
	{
		enum UserInstrumentCommandType
		{
			NOP = 0,
			ENABLE_ALL,
			DISABLE_ALL,
			ENABLE_ALL_VALUE_CHANGED_BREAKPOINTS,
			DISABLE_ALL_VALUE_CHANGED_BREAKPOINTS,
			ENABLE,
			DISABLE,
			ENABLE_VALUE_CHANGED_BRKPT,
			DISABLE_VALUE_CHANGED_BRKPT,
			SET,
			TOGGLE
		};
		
		struct UserInstrumentCommand
		{
			UserInstrumentCommandType type;
			UserInstrument *user_instrument;
			std::string value;
			
			UserInstrumentCommand() : type(NOP), user_instrument(0), value() {}
			UserInstrumentCommand(UserInstrumentCommandType _type) : type(_type), user_instrument(0), value() {}
			UserInstrumentCommand(UserInstrumentCommandType _type, UserInstrument *_user_instrument) : type(_type), user_instrument(_user_instrument), value() {}
			UserInstrumentCommand(UserInstrumentCommandType _type, UserInstrument *_user_instrument, const std::string& _value) : type(_type), user_instrument(_user_instrument), value(_value) {}
			
			void Apply(PropertySetter& property_setter) const;
		};
		
		PropertySetter(UserInterface& _user_interface);
		virtual bool FormAssign(const std::string& name, const std::string& value);
		void Apply();
		
		UserInterface& user_interface;
		sc_core::sc_time curr_time_stamp;
		bool valid_curr_time_stamp;
		sc_core::sc_time user_step_time;
		bool valid_user_step_time;
		bool timed_step;
		unsigned int delta_steps;
		bool cont;
		bool intr;
		bool halt;
		typedef std::vector<UserInstrumentCommand, std::allocator<UserInstrumentCommand> > UserInstrumentCommands;
		UserInstrumentCommands user_instrument_commands;
	};
	
	unisim::kernel::logger::Logger logger;
	
	std::string program_name;

	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	std::string instrumentation;
	unisim::kernel::variable::Parameter<std::string> param_instrumentation;
	sc_core::sc_time intr_poll_period;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_intr_poll_period;
	double min_cont_refresh_period;
	unisim::kernel::variable::Parameter<double> param_min_cont_refresh_period;
	double max_cont_refresh_period;
	unisim::kernel::variable::Parameter<double> param_max_cont_refresh_period;
	bool enable_cache;
	unisim::kernel::variable::Parameter<bool> param_enable_cache;
	std::vector<std::string> instrumented_signal_names;
	std::map<std::string, UserInstrument *> user_instruments;
	typedef std::set<sc_core::sc_time> Schedule;
	Schedule schedule;
	sc_core::sc_time user_step_time;
	sc_core::sc_time curr_time_stamp;
	unsigned int delta_steps;
	bool cont;
	bool intr;
	bool halt;
	double refresh_period;
	bool auto_reload;
	
	pthread_mutex_t mutex_instruments;
	pthread_mutex_t mutex_schedule;
	pthread_mutex_t mutex_delta_steps;
	pthread_mutex_t mutex_post;
	
	pthread_cond_t cond_cont;
	pthread_mutex_t mutex_cont;
	
	void InitialFetch();
	void Sample();
	void Fetch();
	void Commit();
	void ClearBreakpointConditions();
	void WaitForUser();
	void Continue();
	void LockInstruments();
	void UnlockInstruments();
	void LockPost();
	void UnlockPost();
};

class CSV_Reader
	: public InstrumenterFrontEnd
{
public:
	CSV_Reader(const char *name, Instrumenter *instrumenter);
	virtual ~CSV_Reader();

	virtual bool SetupInstrumentation();
	virtual void ProcessInputInstruments();
	virtual void ProcessOutputInstruments();
private:
	unisim::kernel::logger::Logger logger;
	
	std::string filename;
	unisim::kernel::variable::Parameter<std::string> param_filename;
	std::string instrumentation;
	unisim::kernel::variable::Parameter<std::string> param_instrumentation;
	sc_core::sc_time instrumentation_start_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_instrumentation_start_time;
	sc_core::sc_time instrumentation_end_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_instrumentation_end_time;
	std::string csv_delimiter;
	unisim::kernel::variable::Parameter<std::string> param_csv_delimiter;
	
	unsigned int lineno;
	std::ifstream *file;
	sc_core::sc_time time_resolution;
	std::vector<std::string> csv_instrument_names;
	std::vector<InputInstrumentBase *> csv_instrument_map;
	sc_core::sc_time csv_time_stamp;
	
	bool ParseCSVHeaderAndInstrumentInput();
	bool ParseCSV(sc_core::sc_time& deadline);
};

class CSV_Writer
	: public InstrumenterFrontEnd
{
public:
	CSV_Writer(const char *name, Instrumenter *instrumenter);
	virtual ~CSV_Writer();
	
	virtual bool SetupInstrumentation();
	virtual void ProcessInputInstruments();
	virtual void ProcessOutputInstruments();
private:
	unisim::kernel::logger::Logger logger;
	
	std::string filename;
	unisim::kernel::variable::Parameter<std::string> param_filename;
	std::string instrumentation;
	unisim::kernel::variable::Parameter<std::string> param_instrumentation;
	sc_core::sc_time instrumentation_start_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_instrumentation_start_time;
	sc_core::sc_time instrumentation_end_time;
	unisim::kernel::variable::Parameter<sc_core::sc_time> param_instrumentation_end_time;
	std::string csv_delimiter;
	unisim::kernel::variable::Parameter<std::string> param_csv_delimiter;
	
	std::ofstream *file;
	sc_core::sc_time last_instrument_sampling_time_stamp;
	bool force_output;
	
	void SampleInstruments();
	void LatchInstruments();
	void OutputInstrumentsNamesAsCSV(std::ostream& os);
	void OutputInstrumentsAsCSV(std::ostream& os, const sc_core::sc_time& time_stamp);
};

class Instrumenter
	: public unisim::kernel::Object
{
public:
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;

	Instrumenter(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Instrumenter();
	virtual bool BeginSetup();
	virtual bool EndSetup();
	
	unisim::kernel::tlm2::Clock& CreateClock(const std::string& clock_name);

	template <typename T> sc_core::sc_signal<T>& CreateSignal(const T& init_value, INSTRUMENTATION_TYPE instr_type = INPUT_OUTPUT_INSTRUMENTATION);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> sc_core::sc_signal<T, WRITER_POLICY>& CreateSignal(const T& init_value, INSTRUMENTATION_TYPE instr_type = INPUT_OUTPUT_INSTRUMENTATION);
	template <typename T> sc_core::sc_signal<T>& CreateSignal(const std::string& signal_name, const T& init_value, INSTRUMENTATION_TYPE instr_type = INPUT_OUTPUT_INSTRUMENTATION);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> sc_core::sc_signal<T, WRITER_POLICY>& CreateSignal(const std::string& signal_name, const T& init_value, INSTRUMENTATION_TYPE instr_type = INPUT_OUTPUT_INSTRUMENTATION);
	template <typename T> void CreateSignalArray(unsigned int signal_array_dim, const std::string& signal_array_name, const T& init_value, INSTRUMENTATION_TYPE limit = NO_INSTRUMENTATION);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> void CreateSignalArray(unsigned int signal_array_dim, const std::string& signal_array_name, const T& init_value, INSTRUMENTATION_TYPE instr_type = INPUT_OUTPUT_INSTRUMENTATION);

	void PrepareInstrumentation(const std::string& signal_name_pattern, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id);

	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> void RegisterSignal(sc_core::sc_signal<T, WRITER_POLICY> *signal);
	void RegisterPort(sc_core::sc_port_base& port);
	template <typename T> void RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& in_port);
	template <typename T> void RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& out_port);

	template <typename T> sc_core::sc_signal<T>& GetSignal(const std::string& signal_name);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> sc_core::sc_signal<T, WRITER_POLICY>& GetSignal(const std::string& signal_name);
	template <typename T> sc_core::sc_signal<T>& GetSignal(const std::string& signal_array_name, unsigned int idx);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> sc_core::sc_signal<T, WRITER_POLICY>& GetSignal(const std::string& signal_array_name, unsigned int idx);

	void TraceSignalPattern(const std::string& signal_name_pattern);
	void Bind(const std::string& port_name, const std::string& signal_name);
	void BindArray(unsigned int dim, const std::string& port_array_name, const std::string& signal_array_name);
	void BindArray(unsigned int dim, const std::string& port_array_name, unsigned int port_array_begin_idx, const std::string& signal_array_name, unsigned int signal_array_begin_idx);
	void BindArray(unsigned int dim, const std::string& port_array_name, unsigned int port_array_begin_idx, unsigned int port_array_stride, const std::string& signal_array_name, unsigned int signal_array_begin_idx, unsigned int signal_array_stride);
	
	void StartBinding();
	bool SetupInstrumentation();

protected:
	mutable unisim::kernel::logger::Logger logger;
	
	bool verbose;
	bool debug;
private:
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> friend class Typer;
	friend class InstrumenterFrontEnd;
	
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<bool> param_debug;
	
	std::string vcd_trace_filename;
	unisim::kernel::variable::Parameter<std::string> param_vcd_trace_filename;
	std::string trace_signals;
	unisim::kernel::variable::Parameter<std::string> param_trace_signals;
	bool enable_trace_signals;
	unisim::kernel::variable::Parameter<bool> param_enable_trace_signals;
	sc_core::sc_trace_file *trace_file;

	std::string gtkwave_init_script;
	unisim::kernel::variable::Parameter<std::string> param_gtkwave_init_script;
	
	bool enable_user_interface;
	unisim::kernel::variable::Parameter<bool> param_enable_user_interface;
	UserInterface *user_interface;
	
	bool enable_csv_reader;
	unisim::kernel::variable::Parameter<bool> param_enable_csv_reader;
	CSV_Reader *csv_reader;
	
	bool enable_csv_writer;
	unisim::kernel::variable::Parameter<bool> param_enable_csv_writer;
	CSV_Writer *csv_writer;
	
	std::string front_ends_priority_order;
	unisim::kernel::variable::Parameter<std::string> param_front_ends_priority_order;

	std::map<std::string, sc_core::sc_interface *, unisim::util::nat_sort::nat_ltstr> signal_pool;
	std::map<std::string, sc_core::sc_interface *> auto_signal_pool;
	std::map<std::string, sc_core::sc_port_base *> port_pool;
	std::map<std::string, std::string> netlist;
	std::map<std::string, TyperBase *> typers;
	std::map<std::string, SignalTeeBase *> signal_tees;

	std::set<InstrumentKey> instrumentations;
	std::map<std::string, INSTRUMENTATION_TYPE> instrumentation_limit;
	std::map<InstrumentKey, InstrumentBase *> instruments;
	std::vector<InstrumenterFrontEnd *> instrumenter_front_ends;
	
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& pull_down_signal;
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& pull_down_0_signal;
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& pull_up_signal;
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& pull_up_0_signal;
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& unused_signal;
	sc_core::sc_signal<bool, sc_core::SC_MANY_WRITERS>& unused_0_signal;
	
	static bool Match(const char *p, const char *s);
	static bool Match(const std::string& p, const std::string& s);
	void MatchSignalPattern(const std::string& signal_name_pattern, std::vector<std::string>& vec_signal_names) const;
	
	template <typename T> sc_core::sc_signal<T> *TryGetSignal(const std::string& signal_name);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> sc_core::sc_signal<T, WRITER_POLICY> *TryGetSignal(const std::string& signal_name);
	template <typename T> sc_core::sc_signal<T> *TryGetSignal(const std::string& signal_array_name, unsigned int idx);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> sc_core::sc_signal<T, WRITER_POLICY> *TryGetSignal(const std::string& signal_array_name, unsigned int idx);
	template <typename T> bool SignalIsA(const std::string& signal_name, const T& sample);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> bool SignalIsA(const std::string& signal_name, const T& sample);
	template <typename T> bool TryTraceSignal(const std::string& signal_name);
	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> bool TryTraceSignal(const std::string& signal_name);
	void GenerateGTKWaveInitScript(const std::string& filename, int zoom_factor) const;
 	template <typename T, sc_core::sc_writer_policy WRITER_POLICY> bool TryBind(const std::string& port_name, const std::string& signal_name);
	
	InstrumentBase *FindInstrument(const std::string& name, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id);
	InputInstrumentBase *FindInputInstrument(const std::string& name, unsigned int front_end_id);
	OutputInstrumentBase *FindOutputInstrument(const std::string& name, unsigned int front_end_id);
	
	unsigned int RegisterFrontEnd(InstrumenterFrontEnd *instrumenter_front_end);
};

inline InstrumentBase::InstrumentBase(const std::string& name, INSTRUMENTATION_TYPE instrumentation_type, unsigned int front_end_id)
	: key(name, instrumentation_type, front_end_id)
{
}

inline InstrumentBase::~InstrumentBase()
{
}

inline unsigned int InstrumentBase::GetFrontEndId() const
{
	return key.front_end_id;
}

inline const std::string& InstrumentBase::GetName() const
{
	return key.name;
}

inline INSTRUMENTATION_TYPE InstrumentBase::GetInstrumentationType() const
{
	return key.instrumentation_type;
}

inline const InstrumentKey& InstrumentBase::GetKey() const
{
	return key;
}

inline bool InputInstrumentBase::IsInjectionEnabled() const
{
	return signal_tee->IsInjectionEnabled(GetFrontEndId());
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
InputInstrument<T, WRITER_POLICY>::InputInstrument(const std::string& name, unsigned int front_end_id, sc_core::sc_signal<T, WRITER_POLICY> *_signal, SignalTee<T, WRITER_POLICY> *signal_tee, const T& init_value)
	: InputInstrumentBase(name, front_end_id, signal_tee)
	, signal(_signal)
	, value(init_value)
	, value_changed(false)
{
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void InputInstrument<T, WRITER_POLICY>::Inject()
{
	if(value_changed)
	{
#if 0
		std::cout << "InputInstrument<T, WRITER_POLICY>::Inject(): At " << sc_core::sc_time_stamp() << ", " << signal->name() << " <- '" << value << "'" << std::endl;
#endif
		signal->write(value);
		value_changed = false;
	}
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void InputInstrument<T, WRITER_POLICY>::Input(std::istream& is)
{
	T tmp_value;
	
	if(is >> tmp_value)
	{
		if(!is.eof())
		{
			std::cerr << "WARNING! ignoring extra characters after \"" << tmp_value << "\"" << std::endl;
		}
		if(!(value == tmp_value)) value_changed = true;
#if 0
		std::cout << "InputInstrument<T, WRITER_POLICY>::Input(): At " << sc_core::sc_time_stamp() << ", " << signal->name() << ": '" << value << "' -> '" << tmp_value << "'" << std::endl;
#endif
		value = tmp_value;
	}
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
const std::type_info& InputInstrument<T, WRITER_POLICY>::GetTypeInfo() const
{
	return typeid(T);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void InputInstrument<T, WRITER_POLICY>::Print(std::ostream& os) const
{
	os << value;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
OutputInstrument<T, WRITER_POLICY>::OutputInstrument(const std::string& name, unsigned int front_end_id, const sc_core::sc_signal<T, WRITER_POLICY> *_signal, const T& init_value)
	: OutputInstrumentBase(name, front_end_id)
	, signal(_signal)
	, latched_value(init_value)
	, sample_value(init_value)
{
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_interface *OutputInstrument<T, WRITER_POLICY>::GetInterface()
{
	return (sc_core::sc_interface *) signal;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void OutputInstrument<T, WRITER_POLICY>::Sample()
{
	sample_value = signal->read();
// 	std::cerr << sc_core::sc_time_stamp() << ":" << GetName() << ":Sample:" << sample_value << std::endl;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void OutputInstrument<T, WRITER_POLICY>::Latch()
{
	latched_value = sample_value;
// 	std::cerr << sc_core::sc_time_stamp() << ":" << GetName() << ":Latch:" << latched_value << std::endl;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
bool OutputInstrument<T, WRITER_POLICY>::ValueChanged() const
{
	return sample_value != latched_value;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void OutputInstrument<T, WRITER_POLICY>::Output(std::ostream& os)
{
	os << sample_value;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
const std::type_info& OutputInstrument<T, WRITER_POLICY>::GetTypeInfo() const
{
	return typeid(T);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
Typer<T, WRITER_POLICY>::Typer(Instrumenter *_instrumenter)
	: instrumenter(_instrumenter)
{
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
bool Typer<T, WRITER_POLICY>::TryTraceSignal(const std::string& signal_name)
{
	return instrumenter->TryTraceSignal<T, WRITER_POLICY>(signal_name);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
bool Typer<T, WRITER_POLICY>::TryBind(const std::string& port_name, const std::string& signal_name)
{
	return instrumenter->TryBind<T, WRITER_POLICY>(port_name, signal_name); // Note: Binding does not depend on writer policy
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::CreateSignal(const T& init_value, INSTRUMENTATION_TYPE instr_type)
{
	return CreateSignal<T, sc_core::SC_ONE_WRITER>(init_value, instr_type);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY>& Instrumenter::CreateSignal(const T& init_value, INSTRUMENTATION_TYPE instr_type)
{
	return CreateSignal<T, WRITER_POLICY>(sc_core::sc_gen_unique_name("signal"), init_value, instr_type);
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::CreateSignal(const std::string& signal_name, const T& init_value, INSTRUMENTATION_TYPE instr_type)
{
	return CreateSignal<T, sc_core::SC_ONE_WRITER>(signal_name, init_value, instr_type);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY>& Instrumenter::CreateSignal(const std::string& signal_basename, const T& init_value, INSTRUMENTATION_TYPE instr_type)
{
	unisim::kernel::Object *parent = this->GetParent();
	std::string signal_name;
	if(parent)
	{
		signal_name += parent->GetName();
		signal_name += '.';
	}
	signal_name += signal_basename;
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		logger << DebugError << "Signal \"" << signal_name << "\" already exists" << EndDebugError;
		throw std::runtime_error("Internal error!");
	}
	
	sc_core::sc_signal<T, WRITER_POLICY> *signal = new sc_core::sc_signal<T, WRITER_POLICY>(signal_basename.c_str());
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Creating Signal \"" << signal->name() << "\" <- '" << init_value << "' (" << WRITER_POLICY << ")" << EndDebugInfo;
	}
	signal_pool[signal->name()] = signal;
	*signal = init_value;
	
	typers[signal->name()] = new Typer<T, WRITER_POLICY>(this);
	
	SignalTee<T, WRITER_POLICY> *signal_tee = 0;
	sc_core::sc_signal<T, WRITER_POLICY> *original_signal = 0;
	
	std::set<InstrumentKey>::iterator instrumentation_it;
	
	for(instrumentation_it = instrumentations.begin(); instrumentation_it != instrumentations.end(); instrumentation_it++)
	{
		InstrumentKey instrumentation = *instrumentation_it;
		const std::string& signal_name_pattern = instrumentation.name;
		unsigned int front_end_id = instrumentation.front_end_id;
		INSTRUMENTATION_TYPE instrumentation_type = instrumentation.instrumentation_type;
		
		if(Match(signal_name_pattern, signal_name))
		{
			if(((instr_type & INPUT_INSTRUMENTATION) != 0) && ((instrumentation_type & INPUT_INSTRUMENTATION) != 0))
			{
				if(!original_signal)
				{
					std::string original_signal_basename(std::string(signal->basename()) + "_original");
					original_signal = new sc_core::sc_signal<T, WRITER_POLICY>(original_signal_basename.c_str());;
					auto_signal_pool[original_signal->name()] = original_signal;
					*original_signal = init_value;
				}
				
				std::stringstream injected_signal_basename_sstr;
				injected_signal_basename_sstr << signal->basename() << "_injected_" << front_end_id;
				std::string injected_signal_basename(injected_signal_basename_sstr.str());
				sc_core::sc_signal<T, WRITER_POLICY> *injected_signal = new sc_core::sc_signal<T, WRITER_POLICY>(injected_signal_basename.c_str());;
				auto_signal_pool[injected_signal->name()] = injected_signal;
				*injected_signal = init_value;
				
				if(!signal_tee)
				{
					std::string signal_tee_name(std::string(signal->name()) + "_tee");
					if(unlikely(verbose))
					{
						logger << DebugInfo << "Creating Signal tee \"" << signal_tee_name << "\" (" << WRITER_POLICY << ")" << EndDebugInfo;
					}
					signal_tee = new SignalTee<T, WRITER_POLICY>(signal_tee_name, original_signal, signal);;
					signal_tees[signal_tee_name] = signal_tee;
				}
				
				signal_tee->SetInjectedSignal(front_end_id, injected_signal);
				signal_tee->EnableInjection(front_end_id); // FIXME
				
				InputInstrument<T, WRITER_POLICY> *input_instrument = new InputInstrument<T, WRITER_POLICY>(signal_name, front_end_id, injected_signal, signal_tee, init_value);
				instruments.insert(std::pair<InstrumentKey, InstrumentBase *>(input_instrument->GetKey(), input_instrument));
			}
			
			if(((instr_type & OUTPUT_INSTRUMENTATION) != 0) && ((instrumentation_type & OUTPUT_INSTRUMENTATION) != 0))
			{
				OutputInstrument<T, WRITER_POLICY> *output_instrument = new OutputInstrument<T, WRITER_POLICY>(signal_name, front_end_id, signal, init_value);
				instruments.insert(std::pair<InstrumentKey, InstrumentBase *>(output_instrument->GetKey(), output_instrument));
			}
		}
	}
	
	return original_signal ? (*original_signal) : (*signal);
}

template <typename T>
void Instrumenter::CreateSignalArray(unsigned int signal_array_dim, const std::string& signal_array_name, const T& init_value, INSTRUMENTATION_TYPE instr_type)
{
	CreateSignalArray<T, sc_core::SC_ONE_WRITER>(signal_array_dim, signal_array_name, init_value, instr_type);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void Instrumenter::CreateSignalArray(unsigned int signal_array_dim, const std::string& signal_array_name, const T& init_value, INSTRUMENTATION_TYPE instr_type)
{
	unsigned int signal_array_index;
	
	for(signal_array_index = 0; signal_array_index < signal_array_dim; signal_array_index++)
	{
		std::stringstream sstr;
		sstr << signal_array_name << "_" << signal_array_index;
		std::string signal_name(sstr.str());
		CreateSignal<T, WRITER_POLICY>(signal_name, init_value, instr_type);
	}
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::GetSignal(const std::string& signal_name)
{
	return GetSignal<T, sc_core::SC_ONE_WRITER>(signal_name);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY>& Instrumenter::GetSignal(const std::string& signal_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T, WRITER_POLICY> *signal = dynamic_cast<sc_core::sc_signal<T, WRITER_POLICY> *>(signal_if);
			
			if(signal)
			{
				std::string signal_tee_name(std::string(signal->name()) + "_tee");
				std::map<std::string, SignalTeeBase *>::iterator signal_tee_it = signal_tees.find(signal_tee_name);
				
				if(signal_tee_it != signal_tees.end())
				{
					SignalTee<T, WRITER_POLICY> *signal_tee = dynamic_cast<SignalTee<T, WRITER_POLICY> *>((*signal_tee_it).second);
					
					if(signal_tee)
					{
						sc_core::sc_signal<T, WRITER_POLICY> *original_signal = signal_tee->GetOriginalSignal();
						return *original_signal;
					}

					logger << DebugError << "Can't retrieve signal tee \"" << signal_tee_name << "\" (" << WRITER_POLICY << ")" << EndDebugError;
					throw std::runtime_error("Internal error!");
				}
				return *signal;
			}
			
			logger << DebugError << "ERROR! Signal \"" << signal_name << "\" has an unexpected type" << EndDebugError;
			throw std::runtime_error("Internal error!");
		}
	}
	
	logger << DebugError << "Signal \"" << signal_name << "\" not found" << EndDebugError;
	throw std::runtime_error("Internal error!");
	static sc_core::sc_signal<T, WRITER_POLICY> dummy_signal;
	return dummy_signal;
}

template <typename T>
sc_core::sc_signal<T>& Instrumenter::GetSignal(const std::string& signal_array_name, unsigned int signal_array_index)
{
	return GetSignal<T, sc_core::SC_ONE_WRITER>(signal_array_name, signal_array_index);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY>& Instrumenter::GetSignal(const std::string& signal_array_name, unsigned int signal_array_index)
{
	std::stringstream sstr;
	sstr << signal_array_name << "_" << signal_array_index;
	std::string signal_name(sstr.str());

	return GetSignal<T, WRITER_POLICY>(signal_name);
}

template <typename T>
sc_core::sc_signal<T> *Instrumenter::TryGetSignal(const std::string& signal_name)
{
	return TryGetSignal<T, sc_core::SC_ONE_WRITER>(signal_name);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY> *Instrumenter::TryGetSignal(const std::string& signal_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T, WRITER_POLICY> *signal = dynamic_cast<sc_core::sc_signal<T, WRITER_POLICY> *>(signal_if);
			
			return signal;
		}
	}
	
	return 0;
}

template <typename T>
sc_core::sc_signal<T> *Instrumenter::TryGetSignal(const std::string& signal_array_name, unsigned int signal_array_index)
{
	return TryGetSignal<T, sc_core::SC_ONE_WRITER>(signal_array_name, signal_array_index);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
sc_core::sc_signal<T, WRITER_POLICY> *Instrumenter::TryGetSignal(const std::string& signal_array_name, unsigned int signal_array_index)
{
	std::stringstream sstr;
	sstr << signal_array_name << "_" << signal_array_index;
	std::string signal_name(sstr.str());

	return TryGetSignal<T, WRITER_POLICY>(signal_name);
}

template <typename T>
bool Instrumenter::SignalIsA(const std::string& signal_name, const T& sample)
{
	return SignalIsA<T, sc_core::SC_ONE_WRITER>(signal_name, sample);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
bool Instrumenter::SignalIsA(const std::string& signal_name, const T& sample)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T, WRITER_POLICY> *signal = dynamic_cast<sc_core::sc_signal<T, WRITER_POLICY> *>(signal_if);
			
			if(signal) return true;
		}
	}
	
	return false;
}

template <typename T>
bool Instrumenter::TryTraceSignal(const std::string& signal_name)
{
	return TryTraceSignal<T, sc_core::SC_ONE_WRITER>(signal_name);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
bool Instrumenter::TryTraceSignal(const std::string& signal_name)
{
	std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
	if(signal_pool_it != signal_pool.end())
	{
		sc_core::sc_interface *signal_if = (*signal_pool_it).second;
		
		if(signal_if)
		{
			sc_core::sc_signal<T, WRITER_POLICY> *signal = dynamic_cast<sc_core::sc_signal<T, WRITER_POLICY> *>(signal_if);
			
			if(signal)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Tracing Signal \"" << signal->name() << "\"" << EndDebugInfo;
				}
				sc_trace(trace_file, *signal, signal->name());
				return true;
			}
		}
	}
	
	return false;
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
void Instrumenter::RegisterSignal(sc_core::sc_signal<T, WRITER_POLICY> *signal)
{
	if(!signal) return;
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Registering Signal \"" << signal->name() << "\"" << EndDebugInfo;
	}
	signal_pool[signal->name()] = signal;
	typers[signal->name()] = new Typer<T, WRITER_POLICY>(this);
}

template <typename T>
void Instrumenter::RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_in_if<T> >& in_port)
{
	RegisterPort(*(sc_core::sc_port_base *) &in_port);
	//typers[in_port.name()] = new Typer<T>(this);
}

template <typename T>
void Instrumenter::RegisterPort(sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> >& out_port)
{
	RegisterPort(*(sc_core::sc_port_base *) &out_port);
	//typers[out_port.name()] = new Typer<T>(this);
}

template <typename T, sc_core::sc_writer_policy WRITER_POLICY>
bool Instrumenter::TryBind(const std::string& port_name, const std::string& signal_name)
{
	sc_core::sc_port_b<sc_core::sc_signal_in_if<T> > *in_port = 0;
	sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> > *out_port = 0;
	
	std::map<std::string, sc_core::sc_port_base *>::iterator port_pool_it = port_pool.find(port_name);
	
	if(port_pool_it != port_pool.end())
	{
		sc_core::sc_port_base *port_base = (*port_pool_it).second;
		
		in_port = dynamic_cast<sc_core::sc_port_b<sc_core::sc_signal_in_if<T> > *>(port_base);
		
		if(unlikely(debug))
		{
			logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): in_port=" << in_port << EndDebugInfo;
		}

		if(!in_port)
		{
			out_port = dynamic_cast<sc_core::sc_port_b<sc_core::sc_signal_inout_if<T> > *>(port_base);
			if(unlikely(debug))
			{
				logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): out_port=" << out_port << EndDebugInfo;
			}
		}
	}
	else
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "While binding, port \"" << port_name << "\" does not exist" << EndDebugInfo;
		}
	}
	
	if(in_port || out_port)
	{
		std::map<std::string, sc_core::sc_interface *>::iterator signal_pool_it = signal_pool.find(signal_name);
			
		if(signal_pool_it != signal_pool.end())
		{
			sc_core::sc_interface *signal_if = (*signal_pool_it).second;
			
			if(in_port)
			{
				sc_core::sc_signal_in_if<T> *signal_in_if = dynamic_cast<sc_core::sc_signal_in_if<T> *>(signal_if);
				
				if(unlikely(debug))
				{
					logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): signal_in_if=" << signal_in_if << EndDebugInfo;
				}
				
				if(signal_in_if)
				{
					(*in_port)(*signal_in_if);
					if(unlikely(debug))
					{
						logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): success (in port <- signal_in_if)" << EndDebugInfo;
					}
					return true;
				}
			}
			else if(out_port)
			{
				sc_core::sc_signal_inout_if<T> *signal_inout_if = dynamic_cast<sc_core::sc_signal_inout_if<T> *>(signal_if);

				if(unlikely(debug))
				{
					logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): signal_inout_if=" << signal_inout_if << EndDebugInfo;
				}

				if(signal_inout_if)
				{
					std::string signal_tee_name(signal_name + "_tee");
					
					std::map<std::string, SignalTeeBase *>::iterator signal_tee_it = signal_tees.find(signal_tee_name);
					
					if(signal_tee_it != signal_tees.end())
					{
						SignalTee<T, WRITER_POLICY> *signal_tee = dynamic_cast<SignalTee<T, WRITER_POLICY> *>((*signal_tee_it).second);
						
						if(signal_tee)
						{
							(*out_port)(*signal_tee->GetOriginalSignal());
							if(unlikely(debug))
							{
								logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): success (out_port -> original_signal_inout_if)" << EndDebugInfo;
							}
						}
						else
						{
							logger << DebugError << "Can't retrieve signal tee \"" << signal_tee_name << "\" (" << WRITER_POLICY << ") because instrumenter front-end probably did not prepared signal for input instrumentation" << EndDebugError;
							throw std::runtime_error("Internal error!");
						}
					}
					else
					{
						(*out_port)(*signal_inout_if);
						if(unlikely(debug))
						{
							logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): success (output -> signal_inout_if)" << EndDebugInfo;
						}
					}
					
					return true;
				}
				else
				{
					logger << DebugWarning << "While binding, signal \"" << signal_name << "\" either does not exist or data type is incompatible" << EndDebugWarning;
				}
			}
		}
		else
		{
			logger << DebugWarning << "While binding, signal \"" << signal_name << "\" does not exist" << EndDebugWarning;
		}
	}
	else
	{
		logger << DebugWarning << "While binding, port \"" << port_name << "\" either does not exist or data type is incompatible" << EndDebugWarning;
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "TryBind(\"" << port_name << ",\"" << signal_name << "): failed" << EndDebugInfo;
	}
	return false;
}

} // end of namespace instrumenter
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INSTRUMENTER_INSTRUMENTER_HH__
