/*
 *  Copyright (c) 2007, 2010
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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Reda   Nouacer      (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_SERVICE_PIM_PIM_SERVER_TCC__
#define __UNISIM_SERVICE_PIM_PIM_SERVER_TCC__

#include <unisim/service/pim/pim_server.hh>

#include <unisim/util/xml/xml.hh>

#include <iostream>
#include <sstream>
#include <list>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

#include "unisim/util/xml/xml.hh"
#include "unisim/util/debug/memory_access_type.hh"

namespace unisim {
namespace service {
namespace pim {

using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::list;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::kernel::variable::Statistic;

using unisim::util::debug::Statement;

using unisim::service::pim::PIMThread;

template <class ADDRESS>
PIMServer<ADDRESS>::PIMServer(const char *_name, Object *_parent)
	: Object(_name, _parent, "PIM Server")
	, Service<DebugYielding>(_name, _parent)
	, Service<TrapReporting>(_name, _parent)
	, unisim::kernel::Client<Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<Registers>(_name, _parent)
	, Service<DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<DebugEventTrigger<ADDRESS> >(_name, _parent)

	, unisim::kernel::Client<Monitor_if<ADDRESS> > (_name, _parent)

	, VariableBaseListener()

	, debug_yielding_export("debug-control-export", this)
	, memory_access_reporting_export("memory-access-reporting-export", this)
	, trap_reporting_export("trap-reporting-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, memory_access_reporting_control_import("memory_access_reporting_control_import", this)
	, memory_import("memory-import", this)
	, registers_import("cpu-registers-import", this)
	, disasm_import("disasm-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)

	, monitor_import("monitor-import", this)

	, socketServer(NULL)
	, gdbThread(NULL)
	, monitorThread(NULL)

	, logger(*this)
	, tcp_port(12345)
	, architecture_description_filename()
	, pc_reg(0)
	, pc_reg_index(0)
	, endian (GDB_BIG_ENDIAN)
	, killed(false)
	, trap(false)

	, watchpoint_hit(NULL)

	, breakpoint_registry()
	, watchpoint_registry()
	, running_mode(GDBSERVER_MODE_WAITING_GDB_CLIENT)
	, extended_mode(false)
	, counter(0)
	, period(50)
	, disasm_addr(0)
	, memory_atom_size(1)
	, verbose(false)
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, param_tcp_port("tcp-port", this, tcp_port, "TCP/IP port to listen waiting for a GDB client connection")
	, param_architecture_description_filename("architecture-description-filename", this, architecture_description_filename, "filename of a XML description of the connected processor")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_host("host", this, fHost)

//	, last_time_ratio(1e+9)

	, local_time(0)

{

	memory_access_reporting_export.SetupDependsOn(memory_access_reporting_control_import);

	counter = period;

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		throw std::runtime_error("WSAStartup failed: Windows sockets not available");
	}
#endif
}

template <class ADDRESS>
PIMServer<ADDRESS>::~PIMServer()
{

	if (gdbThread) { delete gdbThread; gdbThread = NULL; }

	if (monitorThread) { delete monitorThread; monitorThread = NULL; }

	if (socketServer) { delete socketServer; socketServer = NULL;}

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	//releases the winsock2 resources
	WSACleanup();
#endif
}

template <class ADDRESS>
double PIMServer<ADDRESS>::GetSimTime() {
	return (Object::GetSimulator()->GetSimTime());
}

template <class ADDRESS>
double PIMServer<ADDRESS>::GetHostTime() {
	return (Object::GetSimulator()->GetHostTime());
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::BeginSetup() {

	return (true);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::Setup(ServiceExportBase *srv_export) {

	if(memory_access_reporting_control_import)
	{
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_MEM_ACCESS, false);
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_FETCH_INSN, false);
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_COMMIT_INSN, false);
	}

	return (true);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::EndSetup() {

	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		cerr << Object::GetName() << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << endl;
		return (false);
	}

	// Open Socket Stream
	// connection_req_nbre parameter has to be set to two "2" connections. once is for GDB requests and the second is for PIM requests
	socketServer = new SocketServerThread(GetHost(), GetTCPPort(), 2);

	gdbThread = new GDBThread("gdb-Thread");

	socketServer->setProtocolHandler(gdbThread);

	socketServer->start();

	// Load Architecture informations from simulator
	bool has_architecture_name = false;
	bool has_architecture_endian = false;

	bool has_program_counter = false;
	string program_counter_name;

	VariableBase* architecture_name = Simulator::Instance()->FindParameter("program-name");
	has_architecture_name = (architecture_name != NULL);
	if(!has_architecture_name)
	{
		logger << DebugError << "architecture has no property 'name'" << std::endl << EndDebugError;
		return (false);
	}

	VariableBase* architecture_endian = Simulator::Instance()->FindParameter("endian");
	if (architecture_endian != NULL) {
		string endianstr = *architecture_endian;
		if(endianstr == "little")
		{
			endian = GDB_LITTLE_ENDIAN;
			has_architecture_endian = true;
		}
		else if(endianstr == "big")
		{
			endian = GDB_BIG_ENDIAN;
			has_architecture_endian = true;
		}
	}

	if(!has_architecture_endian)
	{
		logger << DebugWarning << "assuming target architecture endian is 'big endian'" << std::endl << EndDebugWarning;
	}

	VariableBase* param_program_counter_name = Simulator::Instance()->FindParameter("program-counter-name");

	if (param_program_counter_name != NULL) {
		pc_reg = (VariableBase *) Simulator::Instance()->FindRegister(((string) *param_program_counter_name).c_str());
		if (pc_reg != NULL) {
			has_program_counter = true;
		} else {
			logger << DebugWarning << "Simulator has no <program-counter> register named '" << (string) *param_program_counter_name << "'" << std::endl << EndDebugWarning;
		}
	} else {
		logger << DebugWarning << "Simulator has no <program-counter-name> parameter" << std::endl << EndDebugWarning;
	}

	std::list<VariableBase *> lst;

	Simulator::Instance()->GetRegisters(lst);

	uint32_t index = 0;
	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_registers.push_back((VariableBase *) *it);

		if (has_program_counter) {
			if (((VariableBase *) *it) == pc_reg) {
				pc_reg_index = index;
			}
		}

		index++;
	}

	lst.clear();

	return (true);
}

template <class ADDRESS>
void PIMServer<ADDRESS>::Stop(int exit_status) {

	ReportProgramExit();

	if (gdbThread) { gdbThread->stop();}
	if (monitorThread) { monitorThread->stop();}
	if (socketServer) { socketServer->stop();}

}

template <class ADDRESS>
void PIMServer<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void PIMServer<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS>* event)
{
	switch(event->GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			watchpoint_hit = dynamic_cast<const Watchpoint<ADDRESS> *> (event);
			break;
		default:
			// ignore event
			return;
	}

	trap = true;
}

template <class ADDRESS>
void PIMServer<ADDRESS>::ReportTrap()
{
	trap = true;
}

template <class ADDRESS>
void
PIMServer<ADDRESS>::
ReportTrap(const unisim::kernel::Object &obj)
{
	ReportTrap();
}

template <class ADDRESS>
void
PIMServer<ADDRESS>::
ReportTrap(const unisim::kernel::Object &obj,
		   const std::string &str)
{
	ReportTrap();
}

template <class ADDRESS>
void
PIMServer<ADDRESS>::
ReportTrap(const unisim::kernel::Object &obj,
		   const char *c_str)
{
	ReportTrap();
}

template <class ADDRESS>
void PIMServer<ADDRESS>::DebugYield()
{
	ADDRESS addr;
	ADDRESS size;
	unsigned int reg_num;
	ADDRESS type;

	if(running_mode == GDBSERVER_MODE_CONTINUE && !trap)
	{
		if(--counter > 0)
		{
			return;
		}

		counter = period;

		if(gdbThread->isData())
		{
			DBGData* request = gdbThread->receiveData();
			if(request->getCommand() == DBGData::DBG_SUSPEND_ACTION)
			{
				running_mode = GDBSERVER_MODE_WAITING_GDB_CLIENT;
				ReportTracePointTrap();
			}
		}
		else
		{
			return;
		}
	}

	if(trap || running_mode == GDBSERVER_MODE_STEP)
	{
		ReportTracePointTrap();
		running_mode = GDBSERVER_MODE_WAITING_GDB_CLIENT;
		trap = false;
	}

	while(!killed)
	{
		DBGData* request = gdbThread->receiveData();
		if (request == NULL)
		{
			Object::Stop(0);
			return;
		}

		switch (request->getCommand())
		{
			case DBGData::DBG_READ_REGISTERS:
			{

				DBGData *response = new DBGData(DBGData::DBG_READ_REGISTERS);

				string packet;

				for (vector<VariableBase *>::iterator it = simulator_registers.begin(); it != simulator_registers.end(); it++) {
					ADDRESS val = **it;
					string hex;

					number2HexString((uint8_t*) &val, ((VariableBase *) *it)->GetBitSize()/8, hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

					packet += hex;
				}

				response->addAttribute(DBGData::VALUE_ATTR, packet);
				gdbThread->sendData(response);

			}

				break;

			case DBGData::DBG_READ_SELECTED_REGISTER:
			{
				std::string reg_str = request->getAttribute(DBGData::REG_NUM_ATTR);
				reg_num = convertTo<unsigned int>(reg_str);

				string packet;

				ADDRESS val = *(simulator_registers[reg_num]);
				number2HexString((uint8_t*) &val, (simulator_registers[reg_num])->GetBitSize()/8, packet, (endian == GDB_BIG_ENDIAN)? "big":"little");

				DBGData *response = new DBGData(DBGData::DBG_READ_SELECTED_REGISTER);
				response->addAttribute(DBGData::VALUE_ATTR, packet);
				gdbThread->sendData(response);

			}
				break;

			case DBGData::DBG_WRITE_REGISTERS: {
				std::string registers_values = request->getAttribute(DBGData::VALUE_ATTR);

				DBGData *response;
				if(WriteRegisters(registers_values)) {
					response = new DBGData(DBGData::DBG_OK_RESPONSE);
				}
				else {
					response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
				}

				gdbThread->sendData(response);

			}
				break;

			case DBGData::DBG_WRITE_SELECTED_REGISTER: {

				std::string reg_str = request->getAttribute(DBGData::REG_NUM_ATTR);
				reg_num = convertTo<unsigned int>(reg_str);

				std::string reg_val_str = request->getAttribute(DBGData::VALUE_ATTR);

				DBGData *response;
				if(WriteRegister(reg_num, reg_val_str))
					response = new DBGData(DBGData::DBG_OK_RESPONSE);
				else
					response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);

				gdbThread->sendData(response);
			}
				break;

			case DBGData::DBG_READ_MEMORY: {

				std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
				addr = convertTo<ADDRESS>(addr_str);
				std::string size_str = request->getAttribute(DBGData::SIZE_ATTR);
				size = convertTo<ADDRESS>(size_str);

				string packet;

				if(!InternalReadMemory(addr, size, packet))
				{
					if(verbose)
					{
						logger << DebugWarning << memory_import.GetName() << "->ReadMemory has reported an error" << EndDebugWarning;
					}
				}

				DBGData *response = new DBGData(DBGData::DBG_READ_MEMORY);
				response->addAttribute(DBGData::VALUE_ATTR, packet);
				gdbThread->sendData(response);

			}
				break;

			case DBGData::DBG_WRITE_MEMORY: {

				std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
				addr = convertTo<ADDRESS>(addr_str);
				std::string size_str = request->getAttribute(DBGData::SIZE_ATTR);
				size = convertTo<ADDRESS>(size_str);

				std::string value_str = request->getAttribute(DBGData::VALUE_ATTR);

				DBGData *response;
				if(WriteMemory(addr, value_str, size))
					response = new DBGData(DBGData::DBG_OK_RESPONSE);
				else
					response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);

				gdbThread->sendData(response);
			}
				break;

			case DBGData::DBG_STEP_INSTRUCTION: {

				std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
				if (addr_str.empty()) {
					running_mode = GDBSERVER_MODE_STEP;

					if (request) { delete request; request = NULL; }

					return;
				}

				addr = convertTo<ADDRESS>(addr_str);
				if (pc_reg) {
					*pc_reg = addr;
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "CPU has no program counter" << EndDebugWarning;
					}
				}
				running_mode = GDBSERVER_MODE_STEP;

				if (request) { delete request; request = NULL; }

				return;
			}
				break;

			case DBGData::DBG_CONTINUE_ACTION: {
				std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
				if (addr_str.empty()) {
					running_mode = GDBSERVER_MODE_CONTINUE;

					if (request) { delete request; request = NULL; }

					return;
				}

				addr = convertTo<ADDRESS>(addr_str);
				if (pc_reg) {
					*pc_reg = addr;
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "CPU has no program counter" << EndDebugWarning;
					}
				}
				running_mode = GDBSERVER_MODE_CONTINUE;

				if (request) { delete request; request = NULL; }

				return;
			}
				break;

			case DBGData::DBG_KILL_COMMAND: {
				if(!extended_mode)
				{
					killed = true;

					DBGData* response = new DBGData(DBGData::DBG_KILL_COMMAND);

					gdbThread->sendData(response);

				}
			}

				break;

			case DBGData::DBG_RESET_COMMAND: {
				if (request) { delete request; request = NULL; }

				return;
			}
				break;

			case DBGData::DBG_GET_LAST_SIGNAL: {
				ReportTracePointTrap();
			}
				break;

			case DBGData::DBG_ENABLE_EXTENDED_MODE: {
				extended_mode = true;

				DBGData *response = new DBGData(DBGData::DBG_OK_RESPONSE);
				gdbThread->sendData(response);

			}
				break;

			case DBGData::DBG_SET_BREAKPOINT_WATCHPOINT: {
				std::string type_str = request->getAttribute(DBGData::TYPE_ATTR);
				type = convertTo<uint32_t>(type_str);

				std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
				addr = convertTo<ADDRESS>(addr_str);

				std::string size_str = request->getAttribute(DBGData::SIZE_ATTR);
				size = convertTo<uint32_t>(size_str);

				DBGData *response;
				if(SetBreakpointWatchpoint(type, addr, size))
					response = new DBGData(DBGData::DBG_OK_RESPONSE);
				else
					response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);

				gdbThread->sendData(response);
			}
				break;

			case DBGData::DBG_REMOVE_BREAKPOINT_WATCHPOINT: {
				std::string type_str = request->getAttribute(DBGData::TYPE_ATTR);
				type = convertTo<uint32_t>(type_str);

				std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
				addr = convertTo<ADDRESS>(addr_str);

				std::string size_str = request->getAttribute(DBGData::SIZE_ATTR);
				size = convertTo<uint32_t>(size_str);

				DBGData *response;
				if(RemoveBreakpointWatchpoint(type, addr, size))
					response = new DBGData(DBGData::DBG_OK_RESPONSE);
				else
					response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);

				gdbThread->sendData(response);
			}
				break;

//			case DBGData::UNKNOWN: {
//
//				DBGData *response = new DBGData(DBGData::DBG_ERROR_READING_DATA_EPERM);
//
//				gdbThread->sendData(response);
//			}
//				break;
			default:

				if(request->getCommand() == DBGData::QUERY_SYMBOL_READ) {

					const string name = request->getAttribute(DBGData::NAME_ATTR);

					list<const Symbol<ADDRESS> *> symbol_registries;

					if (symbol_table_lookup_import) {
						symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);
					}
					typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;

					string value;

					for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
					{

						if ((name.compare((*symbol_iter)->GetName()) == 0)) {

							value = "";

							if(!InternalReadMemory((*symbol_iter)->GetAddress(), (*symbol_iter)->GetSize(), value))
							{
								if(verbose)
								{
									logger << DebugWarning << memory_import.GetName() << "->ReadSymbol has reported an error" << EndDebugWarning;
								}
							}

							string hexName;

							textToHex((*symbol_iter)->GetName(), strlen((*symbol_iter)->GetName()), hexName);

							DBGData* response = new DBGData(DBGData::QUERY_SYMBOL_READ);
							response->addAttribute(DBGData::NAME_ATTR, hexName);
							response->addAttribute(DBGData::VALUE_ATTR, value);
							gdbThread->sendData(response);

							break;
						}

					}

				}
				else if(request->getCommand() == DBGData::QUERY_SYMBOL_READ_ALL) {

					list<const Symbol<ADDRESS> *> symbol_registries;

					if (symbol_table_lookup_import) {
						symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);
					}
					typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;

					for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
					{

						string value = "";

						if(!InternalReadMemory((*symbol_iter)->GetAddress(), (*symbol_iter)->GetSize(), value))
						{
							if(verbose)
							{
								logger << DebugWarning << memory_import.GetName() << "->ReadSymbol has reported an error" << EndDebugWarning;
							}
						}

						string hexName;

						textToHex((*symbol_iter)->GetName(), strlen((*symbol_iter)->GetName()), hexName);

						DBGData* response = new DBGData(DBGData::QUERY_SYMBOL_READ);
						response->addAttribute(DBGData::NAME_ATTR, hexName);
						response->addAttribute(DBGData::VALUE_ATTR, value);
						gdbThread->sendData(response);

					}

					DBGData* response = new DBGData(DBGData::QUERY_SYMBOL);
					gdbThread->sendData(response);

				}
				else if(request->getCommand() == DBGData::QUERY_SYMBOL_WRITE) {

					const string name = request->getAttribute(DBGData::NAME_ATTR);
					const string hexValue = request->getAttribute(DBGData::VALUE_ATTR);

					list<const Symbol<ADDRESS> *> symbol_registries;

					if (symbol_table_lookup_import) {
						symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);
					}
					typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;

					string packet = "";

					for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
					{

						if (name.compare((*symbol_iter)->GetName()) == 0) {

							uint32_t size = (*symbol_iter)->GetSize();
							uint8_t *value = (uint8_t*) malloc(size);
							memset(value, 0, size);

							uint32_t value_index;
							uint8_t val;
							uint32_t hex_index;
							if ((hexValue.length() % 2) == 0) {
								hex_index = 0;
								value_index = size - (hexValue.length() / 2);
							} else {
								hex_index = 1;
								value_index = size - (hexValue.length() / 2) - 1;
								value[value_index++] = hexChar2Nibble(hexValue[0]);
							}
							for (; (hex_index < hexValue.length()); hex_index = hex_index+2) {
								val = (hexChar2Nibble(hexValue[hex_index]) << 4) | hexChar2Nibble(hexValue[hex_index+1]);
								value[value_index++] = val;
							}

							if (!memory_import->WriteMemory((*symbol_iter)->GetAddress(), value, size)) {
								if(verbose)
								{
									logger << DebugWarning << memory_import.GetName() << "->WriteSymbol has reported an error" << EndDebugWarning;
								}

								DBGData* response = new DBGData(DBGData::DBG_NOK_RESPONSE);
								gdbThread->sendData(response);

							} else {

								DBGData* response = new DBGData(DBGData::DBG_OK_RESPONSE);
								gdbThread->sendData(response);
							}

							break;
						}

					}

				}
				else if(request->getCommand() == DBGData::DBG_VERBOSE_RESUME_ACTIONS)
				{
					DBGData *response =  new DBGData(DBGData::DBG_VERBOSE_RESUME_ACTIONS);
					response->addAttribute(DBGData::VALUE_ATTR, "c;C;s;S");
					gdbThread->sendData(response);

				}
				else if(request->getCommand() == DBGData::DBG_VERBOSE_RESUME_CONTINUE)
				{
					running_mode = GDBSERVER_MODE_CONTINUE;

					if (request) { delete request; request = NULL; }

					return;
				}
				else if(request->getCommand() == DBGData::DBG_VERBOSE_RESUME_STEP)
				{
					running_mode = GDBSERVER_MODE_STEP;

					if (request) { delete request; request = NULL; }

					return;
				}
				else if (!HandleQRcmd(request)) {
					if(verbose)
					{
						logger << DebugWarning << "Received an unknown command" << EndDebugWarning;
					}

					DBGData *response = new DBGData(DBGData::DBG_ERROR_READING_DATA_EPERM);
					gdbThread->sendData(response);
				}

				break;

		} // end of switch

		if (request) { delete request; request = NULL; }

	} // end of while(!killed)

	Object::Stop(0);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteRegisters(const string& hex)
{

	unsigned int pos = 0;

	for(vector<VariableBase *>::iterator it = simulator_registers.begin(); it != simulator_registers.end(); pos += 2 * (*it)->GetBitSize()/8, it++)
	{
		ADDRESS val = 0;
		string subhex = hex.substr(pos, 2 * (*it)->GetBitSize()/8);
		if (hexString2Number(subhex, &val, (*it)->GetBitSize()/8, (endian == GDB_BIG_ENDIAN)? "big":"little")) {
			*(*it) = val;
		} else {
			return (false);
		}

	}

	return (true);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteRegister(unsigned int regnum, const string& hex)
{

	string packet;
	ADDRESS val = 0;

	if (hexString2Number(hex, &val, (simulator_registers[regnum])->GetBitSize()/8, (endian == GDB_BIG_ENDIAN)? "big":"little")) {
		*(simulator_registers[regnum]) = val;
	} else {
		return (false);
	}

	return (true);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::InternalReadMemory(ADDRESS addr, uint32_t size, string& packet)
{
	bool read_error = false;
	bool overwrapping = false;

	char ch[2];
	ch[1] = 0;

	if(size > 0)
	{
		do
		{
			uint8_t value = 0;
			if(!overwrapping)
			{
				if(!memory_import->ReadMemory(addr, &value, 1))
				{
					read_error = true;
					value = 0;
				}
			}
			ch[0] = nibble2HexChar(value >> 4);
			packet += ch;
			ch[0] = nibble2HexChar(value & 0xf);
			packet += ch;
			if((addr + 1) == 0) overwrapping = true;
		} while(++addr, --size);
	}

	if(read_error)
	{
		return (false);
	}

	return (true);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteMemory(ADDRESS addr, const string& hex, uint32_t size)
{
	bool write_error = false;
	bool overwrapping = false;
	unsigned int pos = 0;
	int len = hex.length();

	if(size > 0 && len > 0)
	{
		do
		{
			uint8_t value;
			value = (hexChar2Nibble(hex[pos]) << 4) | hexChar2Nibble(hex[pos + 1]);
			if(!overwrapping)
			{
				if(!memory_import->WriteMemory(addr, &value, 1))
				{
					write_error = true;
				}
			}
			if((addr + 1) == 0) overwrapping = true;
		} while(pos += 2, ++addr, (--size) && ((len -= 2) >= 0));
	}

	if(write_error)
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->WriteMemory has reported an error" << EndDebugWarning;
		}
	}

	return (true);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReportProgramExit()
{
	DBGData* response = new DBGData(DBGData::DBG_PROCESS_EXIT);
	gdbThread->sendData(response);

//#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
//			Sleep(1);
//#else
//			usleep(1000);
//#endif

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReportSignal(unsigned int signum)
{
	DBGData* response = new DBGData(DBGData::DBG_REPORT_STOP);

	char signum_str[3];
	snprintf(signum_str, 3, "%02x", signum);
	response->addAttribute(DBGData::VALUE_ATTR, signum_str);

	gdbThread->sendData(response);

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReportTracePointTrap()
{
	DBGData* response;

	if (watchpoint_hit != NULL) {

// *******************************

//	if (monitor_import) {
//		string name;
//
//		list<const Symbol<ADDRESS> *> symbol_registries;
//
//		if (symbol_table_lookup_import) {
//			symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);
//
//		}
//
//		typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;
//
//		string value;
//
//		for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
//		{
//
//			if ((*symbol_iter)->GetAddress() == watchpoint_hit->GetAddress()) {
//
//				name = (*symbol_iter)->GetName();
//				value = "";
//
//				if(!InternalReadMemory((*symbol_iter)->GetAddress(), (*symbol_iter)->GetSize(), value))
//				{
//					if(verbose)
//					{
//						logger << DebugWarning << memory_import.GetName() << "->ReadSymbol has reported an error" << EndDebugWarning;
//					}
//				}
//
////				double d = convertTo<double>(value);
//
//				unsigned long d = 0;
//				hexString2Number(value, &d, (*symbol_iter)->GetSize(), (endian == GDB_BIG_ENDIAN)? "big":"little");
//
////				std::cout << "res = " << d << "   at " << local_time << std::endl;
//
//				monitor_import->refresh_value(name.c_str(), (double) d, local_time++);
//
//				break;
//			}
//
//		}
//	} // end if(monitor)

// *******************************

		if (watchpoint_hit->GetMemoryAccessType() == unisim::util::debug::MAT_READ) {
			response = new DBGData(DBGData::DBG_READ_WATCHPOINT);
		} else {
			response = new DBGData(DBGData::DBG_WRITE_WATCHPOINT);
		}

		std::stringstream sstr;
		sstr << std::hex << watchpoint_hit->GetAddress();
		response->addAttribute(DBGData::ADDRESS_ATTR, sstr.str());

		sstr.str(std::string());

		watchpoint_hit = NULL;

	} else {
		response = new DBGData(DBGData::DBG_REPORT_EXTENDED_STOP);
	}

	gdbThread->sendData(response);

	return true;

}

template <class ADDRESS>
bool PIMServer<ADDRESS>::SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size)
{
	uint32_t i;

// 	cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
// 	     << ": type = " << type
// 	     << " address = 0x" << hex << addr << dec
// 	     << " size = " << size << endl;

	switch(type)
	{
		case 0:
		case 1:
			for(i = 0; i < size; i++)
			{
				if(!debug_event_trigger_import->SetBreakpoint(addr + i)) return (false);
			}
			return (true);

		case 2:
			if(debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
			{
				return (true);
			}
			else
			{
				return (false);
			}

		case 3:
			if(debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, false))
			{
				return (true);
			}
			else
			{
				return (false);
			}


		case 4:
			if(!debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size, false))
			{
				return (false);
			}

			if(debug_event_trigger_import->SetWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size, false))
			{
				return (true);
			}
			else
			{
				debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
				return (false);
			}

	}
	return (false);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size)
{
	uint32_t i;

// 	cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
// 	     << ": type = " << type
// 	     << " address = 0x" << hex << addr << dec
// 	     << " size = " << size << endl;

	switch(type)
	{
		case 0:
		case 1:
			for(i = 0; i < size; i++)
			{
				if(!debug_event_trigger_import->RemoveBreakpoint(addr + i)) return (false);
			}
			return (true);

		case 2:
			return debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);

		case 3:
			return debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);

		case 4:
			{
				return
				  debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size) and
				  debug_event_trigger_import->RemoveWatchpoint(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);
			}

	}
	return (false);
}

// *** Start ************ REDA ADDED CODE ****************

template <class ADDRESS>
bool PIMServer<ADDRESS>::HandleQRcmd(DBGData *request) {

	// Explore Simulator Variables to Generate PIM XML File
	vector<VariableBase*> simulator_variables;

	std::list<VariableBase *> lst;

	Simulator::Instance()->GetSignals(lst);

	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_variables.push_back((VariableBase *) *it);

	}

	lst.clear();

	// qRcmd,cmd;var_name[:value]{;var_name[:value]}

	if (request->getCommand() == DBGData::QUERY_VAR_LISTEN) {

		string targetVar = request->getSlave();
		for (unsigned int i=0; i < simulator_variables.size(); i++) {
			if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {
				simulator_variables[i]->AddListener(this);
				break;
			}
		}

	} else	if (request->getCommand() == DBGData::QUERY_VAR_UNLISTEN) {

		string targetVar = request->getSlave();

		for (unsigned int i=0; i < simulator_variables.size(); i++) {

			if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

				simulator_variables[i]->RemoveListener(this);

				break;
			}

		}

	} else	if (request->getCommand() == DBGData::QUERY_VAR_READ) {

		string targetVar = request->getSlave();

		for (unsigned int i=0; i < simulator_variables.size(); i++) {

			if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

				DBGData *response = new DBGData(DBGData::QUERY_VAR_READ);

				response->setSimTime(GetSimTime());

				response->setMaster(simulator_variables[i]->GetName());
				response->setMasterSite(request->getMasterSite());

				response->setSlave(simulator_variables[i]->GetName());
				response->setSlaveSite(request->getSlaveSite());

				if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {
					double val = *(simulator_variables[i]);

					response->addAttribute("value", stringify(val));

				}
				else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {
					float val = *(simulator_variables[i]);
					response->addAttribute("value", stringify(val));
				}
				else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {
					bool val = *(simulator_variables[i]);
					response->addAttribute("value", stringify(val));
				}
				else {
					uint64_t val = *(simulator_variables[i]);
					response->addAttribute("value", stringify(val));
				}

				gdbThread->sendData(response);

				break;
			}
		}

	} else if (request->getCommand() == DBGData::QUERY_VAR_WRITE) {

		string targetVar = request->getSlave();

		string value = request->getAttribute("value");

		for (unsigned int i=0; i < simulator_variables.size(); i++) {

			if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

				if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {

					*(simulator_variables[i]) = convertTo<double>(value);

				}
				else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {

					*(simulator_variables[i]) = convertTo<float>(value);

				}
				else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {

					*(simulator_variables[i]) = value.compare("false");
				}
				else {

					*(simulator_variables[i]) = convertTo<uint64_t>(value);

				}

				break;
			}
		}


	} else {

		if (request->getCommand() == DBGData::QUERY_PARAMETERS) {

			DBGData *response = new DBGData(DBGData::QUERY_PARAMETERS);

			Simulator *simulator = Object::GetSimulator();

			std::list<VariableBase *> lst;
			simulator->GetParameters(lst);

			std::stringstream strstm;

			for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

				if (!((VariableBase *) *it)->IsVisible()) continue;

				std::string name = ((VariableBase *) *it)->GetName();
				std::string dataType = ((VariableBase *) *it)->GetDataTypeName();
				std::string value = *((VariableBase *) *it);
				std::string type = ((VariableBase *) *it)->GetTypeName();
				std::string format = "";
				switch (((VariableBase *) *it)->GetFormat()) {
					case VariableBase::FMT_HEX: format = "HEX"; break;
					case VariableBase::FMT_DEC: format = "DEC"; break;
					default: format = "Default"; break;
				}

				const char *description = ((VariableBase *) *it)->GetDescription();

				strstm << type << ":" << name << ":" << dataType << ":" << value << ":" << format << ":" << ((strlen(description) != 0)? std::string(description): "No description available!");

				response->addAttribute(name, strstm.str());

				strstm.str(std::string());

			}

			lst.clear();

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_PARAMETER) {


			string name = request->getAttribute(DBGData::NAME_ATTR);
			string value = request->getAttribute(DBGData::VALUE_ATTR);

			VariableBase* parameter = Simulator::Instance()->FindParameter(name.c_str());
			if (parameter) {
				if (value.empty()) // read parameter request
				{
					std::ostringstream os;

					if (strcmp(parameter->GetDataTypeName(), "double precision floating-point") == 0) {
						double val = *parameter;
						os << stringify(val);
					}
					else if (strcmp(parameter->GetDataTypeName(), "single precision floating-point") == 0) {
						float val = *parameter;
						os << stringify(val);
					}
					else if (strcmp(parameter->GetDataTypeName(), "boolean") == 0) {
						bool val = *parameter;
						os << stringify(val);
					}
					else {
						uint64_t val = *parameter;
						os << stringify(val);
					}

					value = os.str();

					DBGData *response = new DBGData(DBGData::QUERY_PARAMETER);
					response->addAttribute(DBGData::NAME_ATTR, name);
					response->addAttribute(DBGData::VALUE_ATTR, value);

					os.str(std::string());

					gdbThread->sendData(response);
				}
				else // write parameter request
				{
					*parameter = value.c_str();
				}

			} else {

			}

		}
		else if (request->getCommand() == DBGData::QUERY_SYMBOLES) {

			DBGData *response = new DBGData(DBGData::QUERY_SYMBOLES);

			/**
			 * command: qRcmd, symboles
			 * return "symbName:symbAddress:symbSize:symbType"
			 *
			 * with: symbType in {"FUNCTION", "VARIABLE"}
			 */

			list<const Symbol<ADDRESS> *> symbol_registries;

			if (symbol_table_lookup_import) {
				symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_FUNC);
			}

			typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;
			std::stringstream strstm;

			for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
			{
				strstm << (*symbol_iter)->GetName();

				strstm << ":" << std::hex;
				strstm.width(8);
				strstm << (*symbol_iter)->GetAddress();

				strstm << ":" << std::dec << (*symbol_iter)->GetSize();

				strstm << ":" << "FUNCTION";

				string str = strstm.str();

				response->addAttribute((*symbol_iter)->GetName(), str);

				strstm.str(std::string());
			}

			symbol_registries.clear();
			symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);

			for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
			{

				strstm << (*symbol_iter)->GetName();

				strstm << ":" << std::hex;
				strstm.width(8);
				strstm << (*symbol_iter)->GetAddress();

				strstm << ":" << std::dec << (*symbol_iter)->GetSize();

				strstm << ":" << "VARIABLE";

				string str = strstm.str();

				response->addAttribute((*symbol_iter)->GetName(), str);

				strstm.str(std::string());

			}

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_DISASM) {
			/**
			 * command: qRcmd, disasm:address:size
			 *
			 * return "O<next_address>
	         *        "O<disassembled code>"
	         *        "T05"
			 *
			 */

			std::string addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);
			ADDRESS current_address = convertTo<ADDRESS>(addr_str);

			std::string size_str = request->getAttribute(DBGData::SIZE_ATTR);
			unsigned int symbol_size = convertTo<unsigned int>(size_str);


			DBGData *response;;
			if (addr_str.empty() || size_str.empty()) {
				response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
			} else {
				response = new DBGData(DBGData::QUERY_DISASM);
				
				if(disasm_import) {
					ADDRESS next_address = current_address;
					ADDRESS disassembled_size = 0;
					std::stringstream strstm;

					while (disassembled_size < symbol_size) {

						std::string dis = disasm_import->Disasm(current_address, next_address);

						if (current_address == next_address) {
							break;
						}

						strstm << hex << current_address << ":";

						strstm << hex;
						strstm.width(8);
						strstm << (current_address / memory_atom_size) << ":" << dec << dis << endl;
						strstm.fill(' ');

						response->addAttribute(stringify(current_address), strstm.str());

						disassembled_size += next_address - current_address;
						current_address = next_address;

						strstm.str(std::string());
					}
				}
			}

			gdbThread->sendData(response);
		}
		else if (request->getCommand() == DBGData::QUERY_START_PIM) {
			// Start PIMThread
			monitorThread = new PIMThread("pim-thread");

			// Open Socket Stream
			// restart the SocketServer to get connection with Pim-Client

			socketServer->setProtocolHandler(monitorThread);

			socketServer->start();

			DBGData *response = new DBGData(DBGData::DBG_OK_RESPONSE);

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_TIME) {

			DBGData *response = new DBGData(DBGData::QUERY_TIME, GetSimTime());

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_ENDIAN) {
			DBGData *response = new DBGData(DBGData::QUERY_ENDIAN);

			std::stringstream sstr;
			sstr << GetEndian();

			response->addAttribute(DBGData::VALUE_ATTR, sstr.str());

			sstr.str(std::string());

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_REGISTERS) {

			DBGData *response = new DBGData(DBGData::QUERY_REGISTERS);

			std::stringstream sstr;

			std::list<VariableBase *> lst;

			Simulator::Instance()->GetRegisters(lst);

			for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

				if (!((VariableBase *) *it)->IsVisible()) continue;

				sstr << ((VariableBase *) *it)->GetName() << ":" << ((VariableBase *) *it)->GetBitSize()/8 << ";";

			}

			lst.clear();

			response->addAttribute(DBGData::VALUE_ATTR, sstr.str());

			sstr.str(std::string());

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_STACK) {
			DBGData *response = new DBGData(DBGData::QUERY_REGISTERS);

			std::stringstream sstr;

			if(pc_reg)
			{
				string hex;

				const Statement<ADDRESS> *stmt = 0;
				ADDRESS addr = *pc_reg;
				uint64_t mcuAddress = Object::GetSimulator()->GetStructuredAddress(addr);

				number2HexString((uint8_t*) &mcuAddress, 8, hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

				sstr << hex << ":";

				if(stmt_lookup_import)
				{
					stmt = stmt_lookup_import->FindStatement(mcuAddress);
				}

				if(stmt)
				{
					const char *source_filename = stmt->GetSourceFilename();
					if(source_filename)
					{
						const char *source_dirname = stmt->GetSourceDirname();
						if(source_dirname)
						{
							sstr << string(source_dirname);

						}
						sstr << ";" << string(source_filename);

						unsigned int lineno = stmt->GetLineNo();
						unsigned int colno = stmt->GetColNo();

						sstr << ";" << lineno << ";" << colno;

					}
				}
			}

			sstr << ";";

			response->addAttribute(DBGData::VALUE_ATTR, sstr.str());

			sstr.str(std::string());

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_PHYSICAL_ADDRESS) {

			string hex_addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);

			DBGData *response;

			if (hex_addr_str.empty()) {
				response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
			} else {
				response = new DBGData(DBGData::QUERY_PHYSICAL_ADDRESS);

				ADDRESS logical_address;
				hexString2Number(hex_addr_str, &logical_address, hex_addr_str.size()/2, (endian == GDB_BIG_ENDIAN)? "big":"little");

				uint64_t physicalAddress = Object::GetSimulator()->GetPhysicalAddress(logical_address);

				number2HexString((uint8_t*) &physicalAddress, 8, hex_addr_str, (endian == GDB_BIG_ENDIAN)? "big":"little");

				response->addAttribute(DBGData::ADDRESS_ATTR, hex_addr_str);
			}

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_STRUCTURED_ADDRESS) {

			string hex_addr_str = request->getAttribute(DBGData::ADDRESS_ATTR);

			DBGData *response;

			if (hex_addr_str.empty()) {
				response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
			} else {
				response = new DBGData(DBGData::QUERY_STRUCTURED_ADDRESS);

				ADDRESS logical_address;
				hexString2Number(hex_addr_str, &logical_address, hex_addr_str.size()/2, (endian == GDB_BIG_ENDIAN)? "big":"little");

				uint64_t physicalAddress = Object::GetSimulator()->GetStructuredAddress(logical_address);

				number2HexString((uint8_t*) &physicalAddress, 8, hex_addr_str, (endian == GDB_BIG_ENDIAN)? "big":"little");

				response->addAttribute(DBGData::ADDRESS_ATTR, hex_addr_str);
			}

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_SRCADDR) {

			string source_filename = request->getAttribute(DBGData::FILE_NAME_ATTR);
			string lineno = request->getAttribute(DBGData::FILE_LINE_ATTR);
			string colno = request->getAttribute(DBGData::FILE_COLUMN_ATTR);

			DBGData *response;

			if (source_filename.empty() || lineno.empty() || colno.empty()) {
				response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
			} else {
				const Statement<ADDRESS> *stmt = 0;

				if(stmt_lookup_import)
				{
					stmt = stmt_lookup_import->FindStatement(source_filename.c_str(), convertTo<unsigned int>(lineno), convertTo<unsigned int>(colno));
				}

				if(stmt)
				{
					response = new DBGData(DBGData::QUERY_SRCADDR);

					ADDRESS addr = stmt->GetAddress();
					std::stringstream sstr;
					sstr << std::hex << addr;

					response->addAttribute(DBGData::ADDRESS_ATTR, sstr.str());

					sstr.str(std::string());

				} else {
					response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
				}
			}

			gdbThread->sendData(response);

		}
		else if (request->getCommand() == DBGData::QUERY_STATISTICS) {

			DBGData* response = new DBGData(DBGData::QUERY_STATISTICS);

			list<VariableBase *> lst;
			list<VariableBase *>::iterator iter;

			Object::GetSimulator()->GetStatistics(lst);
			std::stringstream sstr;
			sstr << GetSimTime()*1000 << " ms";
			response->addAttribute("simulated time", sstr.str());
			for (iter = lst.begin(); iter != lst.end(); iter++) {

				response->addAttribute((*iter)->GetName(), (string) *(*iter));
			}

			sstr.str(std::string());

			gdbThread->sendData(response);

		} else {
			DBGData* response = new DBGData(DBGData::DBG_ERROR_MALFORMED_REQUEST);
			gdbThread->sendData(response);

			return false;
		}

	}

	return true;

} // end HandleQRcmd methode


template <class ADDRESS>
void PIMServer<ADDRESS>::VariableBaseNotify(const VariableBase *var) {

	DBGData *response = new DBGData(DBGData::QUERY_VAR_LISTEN);

	response->setSimTime(GetSimTime());

	response->setMaster(var->GetName());
	response->setMasterSite("_who_initiate_the_listener_");
	response->setSlave(var->GetName());
	response->setSlaveSite(DBGData::DEFAULT_SLAVE_SITE);

	if (strcmp(var->GetDataTypeName(), "double precision floating-point") == 0) {
		double val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else if (strcmp(var->GetDataTypeName(), "single precision floating-point") == 0) {
		float val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else if (strcmp(var->GetDataTypeName(), "boolean") == 0) {
		bool val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else {
		uint64_t val = *(var);
		response->addAttribute("value", stringify(val));
	}

	gdbThread->sendData(response);

}


// *** End ************ REDA ADDED CODE ****************


} // end of namespace pim
} // end of namespace service
} // end of namespace unisim

#endif
