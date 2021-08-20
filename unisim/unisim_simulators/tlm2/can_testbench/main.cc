/*
 *  Copyright (c) 2018,
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/kernel/tlm2/tlm_can.hh>
#include <unisim/kernel/tlm2/simulator.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <unisim/kernel/config/ini/ini_config_file_helper.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/util/random/random.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/instrumenter/instrumenter.hh>
#include <unisim/service/http_server/http_server.hh>

#include <iostream>
#include <vector>
#include <queue>

using unisim::service::instrumenter::OUTPUT_INSTRUMENTATION;

using unisim::kernel::tlm2::tlm_can_core;
using unisim::kernel::tlm2::tlm_can_core_config;
using unisim::kernel::tlm2::tlm_can_error;
using unisim::kernel::tlm2::tlm_can_core_activity;
using unisim::kernel::tlm2::tlm_can_phase;
using unisim::kernel::tlm2::tlm_can_message;
using unisim::kernel::tlm2::tlm_can_message_event;
using unisim::kernel::tlm2::tlm_can_state;
using unisim::kernel::tlm2::TLM_CAN_SOF_PHASE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_ACTIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_CORE_SYNCHRONIZING;
using unisim::kernel::tlm2::TLM_CAN_NO_ERROR;
using unisim::kernel::tlm2::TLM_CAN_STD_FMT;
using unisim::kernel::tlm2::TLM_CAN_XTD_FMT;
using unisim::kernel::tlm2::TLM_CAN_START_OF_FRAME_TRANSMISSION_EVENT;
using unisim::kernel::tlm2::TLM_CAN_START_OF_FRAME_RECEPTION_EVENT;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_ERROR_EVENT;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_CANCELLED_EVENT;
using unisim::kernel::tlm2::TLM_CAN_TRANSMISSION_OCCURRED_EVENT;
using unisim::kernel::tlm2::TLM_CAN_RECEPTION_ERROR_EVENT;
using unisim::kernel::tlm2::TLM_CAN_MESSAGE_RECEIVED_EVENT;
using unisim::kernel::tlm2::tlm_can_bus;

using unisim::util::random::Random;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;

struct Node : tlm_can_core<Node>
{
	Node(const sc_core::sc_module_name& name, unisim::kernel::Object *parent = 0)
		: unisim::kernel::Object(name, parent)
		, tlm_can_core<Node>(name, parent)
		, enabled(false)
		, config()
		, tx_fifo()
		, rx_fifo()
		, phase(TLM_CAN_SOF_PHASE)
		, state(TLM_CAN_ERROR_ACTIVE_STATE)
		, core_activity(TLM_CAN_CORE_SYNCHRONIZING)
		, can_error(TLM_CAN_NO_ERROR)
		, receive_error_count(0)
		, transmit_error_count(0)
		, loopback(false)
		, param_loopback("loopback", this, loopback, "enable/disable loopback mode")
		, bus_monitoring(false)
		, param_bus_monitoring("bus-monitoring", this, bus_monitoring, "enable/disable bus monitoring mode")
		, automatic_retransmission(true)
		, param_automatic_retransmission("automatic-retransmission", this, automatic_retransmission, "enable/disable automatic retransmission")
		, restricted_operation(false)
		, param_restricted_operation("restricted-operation", this, restricted_operation, "enable/disable restricted operation mode")
		, transmit_pause(0)
	{
		config.set_sample_point(sc_core::sc_time(0.75, sc_core::SC_US));
		config.set_phase_seg2(sc_core::sc_time(0.25, sc_core::SC_US));
		config.set_loopback_mode(loopback);
		config.set_bus_monitoring_mode(bus_monitoring);
		config.set_automatic_retransmission(automatic_retransmission);
		config.set_restricted_operation_mode(restricted_operation);

		SC_HAS_PROCESS(Node);
		
		SC_THREAD(cpu);
	}

	virtual ~Node()
	{
	}

	void cpu()
	{
		Random rnd;
		
		if(strcmp(name(), "HARDWARE.node0") == 0)
		{
			rnd.Seed(1234, 5678, 9012, 3456);
			int r = rnd.Generate(16) + 16;
			wait(r * 0.1, sc_core::SC_MS);
			enable();
			while(1)
			{
				int o = rnd.Generate(750) + 750;
				wait(o, sc_core::SC_US);
				bool rtr = rnd.Generate(4) >= 2;
				uint8_t data1[] = {0xaa, 0x55, 0xaa, 0x55};
				tx_fifo.push(tlm_can_message(TLM_CAN_XTD_FMT, 5 << 18, rtr, false, false, tlm_can_message::encode_dlc(TLM_CAN_XTD_FMT, sizeof(data1)), data1));
			}
		}
		else if(strcmp(name(), "HARDWARE.node1") == 0)
		{
			rnd.Seed(3456, 5678, 1234, 9012);
			int r = rnd.Generate(16) + 16;
			wait(r * 0.1, sc_core::SC_MS);
			enable();
			while(1)
			{
				int o = rnd.Generate(250) + 250;
				wait(o, sc_core::SC_US);
				bool e = rnd.Generate(4) >= -3;
				enable(e);
				bool rtr = rnd.Generate(4) >= 3;
				uint8_t data2[6] = {0x01, 0x23, 0x45, 0x67, 0x89, 0x10 };
				tx_fifo.push(tlm_can_message(TLM_CAN_STD_FMT, 3 << 18, rtr, false, false, tlm_can_message::encode_dlc(TLM_CAN_STD_FMT, sizeof(data2)), data2));
			}
		}
		else if(strcmp(name(), "HARDWARE.node2") == 0)
		{
			rnd.Seed(9012, 3456, 1234, 5678);
			int r = rnd.Generate(16) + 16;
			wait(r * 0.1, sc_core::SC_MS);
			enable();
			while(1)
			{
				int o = rnd.Generate(1500) + 1500;
				wait(o, sc_core::SC_US);
				bool rtr = rnd.Generate(4) >= 2;
				uint8_t data3[8] = {0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x34, 0x56 };
				tx_fifo.push(tlm_can_message(TLM_CAN_XTD_FMT, 10 << 18, rtr, false, false, tlm_can_message::encode_dlc(TLM_CAN_XTD_FMT, sizeof(data3)), data3));
			}
		}
	}

protected:
	friend class tlm_can_core<Node>;
	
	bool is_enabled() const
	{
		return enabled;
	}
	
	const tlm_can_core_config& get_config() const
	{
		return config;
	}
	
	tlm_can_error get_can_error() const
	{
		return can_error;
	}
	
	void set_can_error(tlm_can_error _can_error)
	{
		can_error = _can_error;
	}
	
	tlm_can_core_activity get_core_activity() const
	{
		return core_activity;
	}
	
	void set_core_activity(tlm_can_core_activity _core_activity)
	{
		if(core_activity != _core_activity)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "core is now " << _core_activity << EndDebugInfo;
			}
		}
		core_activity = _core_activity;
	}
	
	tlm_can_phase get_phase() const
	{
		return phase;
	}
	
	void set_phase(tlm_can_phase _phase)
	{
		if(phase != _phase)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "entering " << _phase << EndDebugInfo;
			}
		}
		phase = _phase;
	}
	
	uint8_t get_transmit_error_count() const
	{
		return transmit_error_count;
	}

	uint8_t get_receive_error_count() const
	{
		return receive_error_count;
	}

	tlm_can_state get_state() const
	{
		return state;
	}
	
	void set_transmit_error_count(uint8_t v)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "TRANSMIT ERROR COUNT <- " << (unsigned int) v << EndDebugInfo;
		}
		transmit_error_count = v;
	}
	
	void set_receive_error_count(uint8_t v)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "RECEIVE ERROR COUNT <- " << (unsigned int) v << EndDebugInfo;
		}
		receive_error_count = v;
	}
	
	void set_state(tlm_can_state _state)
	{
		if(state != _state)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "entering " << _state << EndDebugInfo;
			}
		}
		state = _state;
	}
	
	bool has_pending_transmission_request() const
	{
		return !tx_fifo.empty();
	}
	
	const tlm_can_message& fetch_pending_transmission_request()
	{
		return tx_fifo.front();
	}
	
	tlm_can_message& get_receive_message()
	{
		return rx_msg;
	}
	
	void process_message_event(const tlm_can_message_event<>& msg_event)
	{
		switch(msg_event.get_type())
		{
			case TLM_CAN_START_OF_FRAME_TRANSMISSION_EVENT:
				if(unlikely(verbose))
				{
					logger << DebugInfo << "start of frame transmission of message " << msg_event.get_message() << EndDebugInfo;
				}
				break;
				
			case TLM_CAN_START_OF_FRAME_RECEPTION_EVENT:
				if(unlikely(verbose))
				{
					logger << DebugInfo << "start of frame reception of message " << msg_event.get_message() << EndDebugInfo;
				}
				break;
				
			case TLM_CAN_TRANSMISSION_ERROR_EVENT:
				if(unlikely(verbose))
				{
					logger << DebugInfo << "transmission error of message " << msg_event.get_message() << EndDebugInfo;
				}
				break;
				
			case TLM_CAN_TRANSMISSION_CANCELLED_EVENT:
				if(unlikely(verbose))
				{
					logger << DebugInfo << "transmission cancelled of message " << msg_event.get_message() << EndDebugInfo;
				}
				break;
				
			case TLM_CAN_TRANSMISSION_OCCURRED_EVENT: 
				if(unlikely(verbose))
				{
					logger << DebugInfo << "transmitted message " << msg_event.get_message() << EndDebugInfo;
				}
				tx_fifo.pop();
				break;
				
			case TLM_CAN_RECEPTION_ERROR_EVENT:
				if(unlikely(verbose))
				{
					logger << DebugInfo << "reception error of message " << msg_event.get_message() << EndDebugInfo;
				}
				break;
				
			case TLM_CAN_MESSAGE_RECEIVED_EVENT:
				if(unlikely(verbose))
				{
					logger << DebugInfo << "received message " << msg_event.get_message() << EndDebugInfo;
				}
				rx_fifo.push(msg_event.get_message());
				break;
		}
	}
	
	void received_bit(bool value)
	{
	}
	
	unsigned int get_transmit_pause() const
	{
		return transmit_pause;
	}
	
	bool is_transmission_cancelled(const tlm_can_message&) const
	{
		return false;
	}

	void enable(bool v = true)
	{
		if(unlikely(verbose))
		{
			if(!enabled && v)
			{
				logger << DebugInfo << "enabling" << EndDebugInfo;
			}
			else if(enabled && !v)
			{
				logger << DebugInfo << "disabling" << EndDebugInfo;
			}
		}
		enabled = v;
	}
	
	bool enabled;
	tlm_can_core_config config;
	tlm_can_message can_msg;
	std::queue<tlm_can_message> tx_fifo;
	tlm_can_message rx_msg;
	std::queue<tlm_can_message> rx_fifo;
	tlm_can_phase phase;
	tlm_can_state state;
	tlm_can_core_activity core_activity;
	tlm_can_error can_error;
	uint8_t receive_error_count;
	uint8_t transmit_error_count;
	bool loopback;
	unisim::kernel::variable::Parameter<bool> param_loopback;
	bool bus_monitoring;
	unisim::kernel::variable::Parameter<bool> param_bus_monitoring;
	bool automatic_retransmission;
	unisim::kernel::variable::Parameter<bool> param_automatic_retransmission;
	bool restricted_operation;
	unisim::kernel::variable::Parameter<bool> param_restricted_operation;
	unsigned int transmit_pause;
	
};

class Simulator : public unisim::kernel::tlm2::Simulator
{
public:
	Simulator(const sc_core::sc_module_name& name, int argc, char **argv);
	virtual ~Simulator();
	void Run();
	using unisim::kernel::Simulator::Setup;
protected:
private:
	//=========================================================================
	//===                           Components                              ===
	//=========================================================================
	Node *node0;
	Node *node1;
	Node *node2;
	tlm_can_bus *can_bus;
	
	//=========================================================================
	//===                            Services                               ===
	//=========================================================================
	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;
	//  - Instrumenter
	unisim::service::instrumenter::Instrumenter *instrumenter;
	//  - HTTP server
	unisim::service::http_server::HttpServer *http_server;
	//  - Logger Console Printer
	unisim::kernel::logger::console::Printer *logger_console_printer;
	//  - Logger Text File Writer
	unisim::kernel::logger::text_file::Writer *logger_text_file_writer;
	//  - Logger HTTP Writer
	unisim::kernel::logger::http::Writer *logger_http_writer;
	//  - Logger XML File Writer
	unisim::kernel::logger::xml_file::Writer *logger_xml_file_writer;
	
	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);
	
	virtual void SigInt();
};

Simulator::Simulator(const sc_core::sc_module_name& name, int argc, char **argv)
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, node0(0)
	, node1(0)
	, node2(0)
	, can_bus(0)
	, sim_time(0)
	, host_time(0)
	, instrumenter(0)
	, http_server(0)
	, logger_console_printer(0)
	, logger_text_file_writer(0)
	, logger_http_writer(0)
	, logger_xml_file_writer(0)
{
	//=========================================================================
	//===                 Logger Printers instantiations                    ===
	//=========================================================================
	//  - Logger Console Printer
	logger_console_printer = new unisim::kernel::logger::console::Printer();
	//  - Logger Text File Writer
	logger_text_file_writer = new unisim::kernel::logger::text_file::Writer();
	//  - Logger HTTP Writer
	logger_http_writer = new unisim::kernel::logger::http::Writer();
	//  - Logger XML File Writer
	logger_xml_file_writer = new unisim::kernel::logger::xml_file::Writer();

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	node0 = new Node("node0", this);
	node1 = new Node("node1", this);
	node2 = new Node("node2", this);
	
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Instrumenter
	instrumenter = new unisim::service::instrumenter::Instrumenter("instrumenter", this);
	//  - HTTP server
	http_server = new unisim::service::http_server::HttpServer("http-server");
	
	//=========================================================================
	//===                          Port registration                        ===
	//=========================================================================
	
	//=========================================================================
	//===                         Channels creation                         ===
	//=========================================================================
	
	std::vector<sc_core::sc_signal<bool> *> CAN_TX;
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_0", true, OUTPUT_INSTRUMENTATION));
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_1", true, OUTPUT_INSTRUMENTATION));
	CAN_TX.push_back(&instrumenter->CreateSignal("CAN_TX_2", true, OUTPUT_INSTRUMENTATION));
	
	can_bus = new tlm_can_bus("can_bus", instrumenter->CreateSignal("CAN_RX", true, OUTPUT_INSTRUMENTATION), CAN_TX, this);
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	// TLM sockets
	node0->CAN_TX(can_bus->CAN_TX);
	node0->CAN_RX(can_bus->CAN_RX);
	
	node1->CAN_TX(can_bus->CAN_TX);
	node1->CAN_RX(can_bus->CAN_RX);
	
	node2->CAN_TX(can_bus->CAN_TX);
	node2->CAN_RX(can_bus->CAN_RX);
	
	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================
	
	{
		unsigned int i = 0;
		*http_server->http_server_import[i++] >> instrumenter->http_server_export;
		*http_server->http_server_import[i++] >> logger_http_writer->http_server_export;
	}

}

Simulator::~Simulator()
{
	if(node0) delete node0;
	if(node1) delete node1;
	if(node2) delete node2;
	if(can_bus) delete can_bus;
	if(http_server) delete http_server;
	if(instrumenter) delete instrumenter;
	if(logger_console_printer) delete logger_console_printer;
	if(logger_text_file_writer) delete logger_text_file_writer;
	if(logger_http_writer) delete logger_http_writer;
	if(logger_xml_file_writer) delete logger_xml_file_writer;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::Simulator *simulator)
{
	new unisim::kernel::config::xml::XMLConfigFileHelper(simulator);
	new unisim::kernel::config::ini::INIConfigFileHelper(simulator);
	new unisim::kernel::config::json::JSONConfigFileHelper(simulator);
	
	// meta information
	simulator->SetVariable("program-name", "UNISIM CAN testbench");
	simulator->SetVariable("copyright", "Copyright (C) 2018, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM CAN testbench");
	simulator->SetVariable("schematic", "can_testbench/fig_schematic.pdf");
	
	// global quantum
	simulator->SetVariable("HARDWARE.global-quantum", "200 ns");

	// CAN global quantum
	simulator->SetVariable("HARDWARE.can-global-quantum", "1 us");
	
	// HTTP server
	simulator->SetVariable("http-server.http-port", 12360);
	
	// CAN observer
	simulator->SetVariable("enable-tlm-can-bus-observer", 1);
	
	// CAN simulation accuracy
	simulator->SetVariable("HARDWARE.node0.model-accuracy", "cycle-accurate");
	simulator->SetVariable("HARDWARE.node1.model-accuracy", "cycle-accurate");
	simulator->SetVariable("HARDWARE.node2.model-accuracy", "cycle-accurate");
}

void Simulator::Run()
{
	std::cerr << "Starting simulation at supervisor privilege level" << std::endl;
	
	double time_start = host_time->GetTime();

	unisim::kernel::tlm2::Simulator::Run();

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	std::cerr << "Simulation run-time parameters:" << std::endl;
	DumpParameters(std::cerr);
	std::cerr << std::endl;
	
	std::cerr << "Simulation statistics:" << std::endl;
	DumpStatistics(std::cerr);
	std::cerr << std::endl;

	std::cerr << "simulation time: " << spent_time << " seconds" << std::endl;
	std::cerr << "simulated time: " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << std::endl;
	std::cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;
}

void Simulator::SigInt()
{
	unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
}

extern "C" 
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
__declspec(dllexport)
#endif
int sc_main(int argc, char *argv[])
{
	Simulator *simulator = new Simulator("HARDWARE", argc, argv);

	switch(simulator->Setup())
	{
		case unisim::kernel::Simulator::ST_OK_DONT_START:
			break;
		case unisim::kernel::Simulator::ST_WARNING:
			std::cerr << "Some warnings occurred during setup" << std::endl;
		case unisim::kernel::Simulator::ST_OK_TO_START:
			simulator->Run();
			break;
		case unisim::kernel::Simulator::ST_ERROR:
			std::cerr << "Can't start simulation because of previous errors" << std::endl;
			break;
	}

	int exit_status = simulator->GetExitStatus();
	if(simulator) delete simulator;

	return exit_status;
}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
int main(int argc, char *argv[])
{
	return sc_core::sc_elab_and_sim(argc, argv);
}
#endif
