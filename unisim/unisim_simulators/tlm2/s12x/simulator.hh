/*
 * simulator.hh
 *
 *  Created on: 15 mars 2011
 *      Author: rnouacer
 */

#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include <iostream>

#include <signal.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdexcept>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/text_file/text_file_writer.hh>
#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>
#include <unisim/kernel/logger/netstream/netstream_writer.hh>

#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>

#include <unisim/service/interfaces/loader.hh>

#include <unisim/service/loader/elf_loader/elf_loader.hh>
#include <unisim/service/loader/elf_loader/elf_loader.tcc>
#include <unisim/service/loader/s19_loader/s19_loader.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>

#include <unisim/service/tee/registers/registers_tee.hh>
#include <unisim/service/tee/memory_import_export/memory_import_export_tee.hh>

#include <unisim/service/netstreamer/netstreamer.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/http_server/http_server.hh>
#include <unisim/service/instrumenter/instrumenter.hh>
#include <unisim/service/tee/char_io/tee.hh>
#include <unisim/service/web_terminal/web_terminal.hh>

#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>

#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/s12mpu_if.hh>

#include <unisim/component/tlm2/processor/hcs12x/hcs12x.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xgate.hh>
#include <unisim/component/tlm2/processor/hcs12x/xint.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>
#include <unisim/component/tlm2/processor/hcs12x/atd10b.hh>
#include <unisim/component/tlm2/processor/hcs12x/pwm.hh>
#include <unisim/component/tlm2/processor/hcs12x/crg.hh>
#include <unisim/component/tlm2/processor/hcs12x/ect.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xeetx.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12pit24b.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12sci.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12spi.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12mscan.hh>

#include <unisim/component/tlm2/memory/ram/memory.hh>

#include <unisim/service/pim/pim.hh>
#include <unisim/service/pim/pim_server.hh>

#include <unisim/service/monitor/monitor.hh>

#include <unisim/service/debug/inline_debugger/inline_debugger.hh>

#include <xml_atd_pwm_stub.hh>
#include <can_stub.hh>
#include <tle8264_2e.hh>

#ifdef HAVE_RTBCOB
#include "rtb_unisim.hh"
#endif

using unisim::component::cxx::processor::hcs12x::ADDRESS;
using unisim::component::cxx::processor::hcs12x::service_address_t;
using unisim::component::cxx::processor::hcs12x::physical_address_t;
using unisim::component::cxx::processor::hcs12x::address_t;
using unisim::component::cxx::processor::hcs12x::S12MPU_IF;

using unisim::component::tlm2::processor::hcs12x::XINT;
using unisim::component::tlm2::processor::hcs12x::CRG;
using unisim::component::tlm2::processor::hcs12x::ECT;
using unisim::component::tlm2::processor::hcs12x::S12XEETX;
using unisim::component::tlm2::processor::hcs12x::S12PIT24B;
using unisim::component::tlm2::processor::hcs12x::S12SCI;
using unisim::component::tlm2::processor::hcs12x::S12SPIV4;
using unisim::component::tlm2::processor::hcs12x::S12MSCAN;

using unisim::service::debug::gdb_server::GDBServer;
using unisim::service::debug::inline_debugger::InlineDebugger;

using unisim::service::loader::multiformat_loader::MultiFormatLoader;

using unisim::service::pim::PIM;
using unisim::service::pim::PIMServer;

using unisim::service::monitor::Monitor;

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::VariableBase;

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::BigEndian2Host;

using unisim::service::netstreamer::NetStreamer;


class Simulator
  : public unisim::kernel::Simulator
{
private:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================

	typedef unisim::component::cxx::processor::hcs12x::physical_address_t CPU_ADDRESS_TYPE;
	typedef uint64_t MEMORY_ADDRESS_TYPE;
	typedef unisim::component::cxx::processor::hcs12x::service_address_t SERVICE_ADDRESS_TYPE;

		public:
	Simulator(int argc, char **argv);
	virtual ~Simulator();
	virtual void Stop(Object *object, int _exit_status, bool asynchronous);
	virtual Simulator::SetupStatus Setup();

	void Run();
	bool RunSample(double inVal);

	virtual double GetSimTime()	{ if (sim_time) { return (sim_time->GetTime()); } else { return (0); }	}
	virtual double GetHostTime()	{ if (host_time) { return (host_time->GetTime()); } else { return (0); }	}

	virtual unsigned long long GetStructuredAddress(unsigned long long logicalAddress) { return (mmc->getCPU12XPagedAddress(logicalAddress)); }
	virtual unsigned long long GetPhysicalAddress(unsigned long long logicalAddress) { return (mmc->getCPU12XPhysicalAddress(logicalAddress, ADDRESS::EXTENDED, false, false, 0x00)); }

	void GeneratePim() {
		PIM *pim = new PIM("pim");
		pim->generatePimFile();
		if (pim) { delete pim; pim = NULL; }
	};

	/*
	 * To control the progress of the simulation:
	 * - sc_pending_activity_at_current_time()
	 * - sc_get_curr_simcontext()->next_time()
	 */

	double Inject_ATD0(double anValue[8]) {

#ifdef HAVE_RTBCOB
		rtbStub->output_ATD0(anValue);
#else
		xml_atd_pwm_stub->Inject_ATD0(anValue);
#endif

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
	};

	double Inject_ATD1(double anValue[16]) {

#ifdef HAVE_RTBCOB
		rtbStub->output_ATD1(anValue);
#else
		xml_atd_pwm_stub->Inject_ATD1(anValue);
#endif

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
	};

	double Get_PWM(bool (*pwmValue)[8]) {

#ifdef HAVE_RTBCOB
		rtbStub->input(&pwmValue);
#else
		xml_atd_pwm_stub->Get_PWM(pwmValue);
#endif

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
	};

	double Inject_CAN(CAN_DATATYPE msg)
	{
		can_stub->Inject_CAN(msg);

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
	}

	double Get_CAN(CAN_DATATYPE *msg)
	{

		can_stub->Get_CAN(msg);

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
	}

	double Inject_CANArray(CAN_DATATYPE_ARRAY msg)
	{
		can_stub->Inject_CANArray(msg);

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
	}

    double getCANArray(CAN_DATATYPE_ARRAY *msg)
    {
    	can_stub->getCANArray(msg);

		sc_core::sc_time t;
		sc_core::sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
    }

    double symWrite8(const char* strName, uint8_t val) {

		if (debugger) {
			const Symbol<CPU_ADDRESS_TYPE> *symbol = debugger->FindSymbolByName(strName);
			if (symbol) {
				uint8_t value = Host2BigEndian(val);

				if (!cpu->WriteMemory(symbol->GetAddress(), &value, symbol->GetSize())) {
					std::cerr << "INSTRUMENT:: WriteSymbol has reported an error" << std::endl;
				}
			} else {
				std::cerr << "INSTRUMENT:: Symbol " << strName << " not found." << std::endl;
			}
		} else {
			std::cerr << "INSTRUMENT::Debugger not instantiated. Enable at less monitor." << std::endl;
		}

		sc_time t;
		sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
    }

    double symRead8(const char* strName, uint8_t* val) {

		if (debugger) {
			const Symbol<CPU_ADDRESS_TYPE> *symbol = debugger->FindSymbolByName(strName);
			if (symbol) {
				uint8_t value = 0;

				if (!cpu->ReadMemory(symbol->GetAddress(), &value, symbol->GetSize())) {
					std::cerr << "INSTRUMENT:: ReadSymbol has reported an error" << std::endl;
				}
				*val = BigEndian2Host(value);
			} else {
				std::cerr << "INSTRUMENT:: Symbol " << strName << " not found." << std::endl;
			}
		} else {
			std::cerr << "INSTRUMENT::Debugger not instantiated. Enable at less monitor." << std::endl;
		}

		sc_time t;
		sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
    }

    double symWrite16(const char* strName, uint16_t val) {

		if (debugger) {
			const Symbol<CPU_ADDRESS_TYPE> *symbol = debugger->FindSymbolByName(strName);
			if (symbol) {
				uint16_t value = Host2BigEndian(val);

				if (!cpu->WriteMemory(symbol->GetAddress(), &value, symbol->GetSize())) {
					std::cerr << "INSTRUMENT:: WriteSymbol has reported an error" << std::endl;
				}
			} else {
				std::cerr << "INSTRUMENT:: Symbol " << strName << " not found." << std::endl;
			}
		} else {
			std::cerr << "INSTRUMENT::Debugger not instantiated. Enable at less monitor." << std::endl;
		}

		sc_time t;
		sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
    }

    double symRead16(const char* strName, uint16_t* val) {

		if (debugger) {
			const Symbol<CPU_ADDRESS_TYPE> *symbol = debugger->FindSymbolByName(strName);
			if (symbol) {
				uint16_t value = 0;

				if (!cpu->ReadMemory(symbol->GetAddress(), &value, symbol->GetSize())) {
					std::cerr << "INSTRUMENT:: ReadSymbol has reported an error" << std::endl;
				}
				*val = BigEndian2Host(value);
			} else {
				std::cerr << "INSTRUMENT:: Symbol " << strName << " not found." << std::endl;
			}
		} else {
			std::cerr << "INSTRUMENT::Debugger not instantiated. Enable at less monitor." << std::endl;
		}

		sc_time t;
		sc_get_curr_simcontext()->next_time(t);
		return (t.to_seconds());
    }

private:

	//=========================================================================
	//===                     Aliases for components classes                ===
	//=========================================================================

	typedef unisim::component::tlm2::memory::ram::Memory<32, physical_address_t, 8, 1024*1024, false>  RAM;
	typedef unisim::component::tlm2::memory::ram::Memory<32, physical_address_t, 8, 1024*1024, false>  FLASH;
	typedef unisim::component::tlm2::processor::hcs12x::S12XEETX<2, 32, physical_address_t, 8, 1024*1024, false>  EEPROM;

	typedef unisim::component::tlm2::processor::hcs12x::HCS12X CPU;
	typedef unisim::component::tlm2::processor::s12xgate::S12XGATE XGATE;

	typedef unisim::component::tlm2::processor::hcs12x::S12XMMC MMC;

	typedef unisim::component::tlm2::processor::hcs12x::S12MSCAN MSCAN;

	typedef unisim::component::tlm2::processor::hcs12x::PWM<8> PWM;
	typedef unisim::component::tlm2::processor::hcs12x::ATD10B<16> ATD1;
	typedef unisim::component::tlm2::processor::hcs12x::ATD10B<8> ATD0;
	typedef unisim::component::tlm2::processor::hcs12x::S12PIT24B<4> PIT;

	// ******* REGARDE Interface ElfLoader pour le typedef ci-dessous
	struct DEBUGGER_CONFIG
	{
		typedef uint32_t ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger, pim_server, monitor, and profiler */
		static const unsigned int MAX_FRONT_ENDS = 5;
	};
	typedef typename unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> Debugger;
       
	typedef unisim::service::loader::elf_loader::ElfLoaderImpl<CPU_ADDRESS_TYPE, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;

	typedef unisim::service::tee::registers::RegistersTee<32> RegistersTee;
	typedef unisim::service::tee::memory_import_export::MemoryImportExportTee<physical_address_t, 32> MemoryImportExportTee;

	typedef unisim::service::monitor::Monitor<CPU_ADDRESS_TYPE> MONITOR;
	typedef unisim::service::netstreamer::NetStreamer TELNET;
	
	typedef unisim::service::http_server::HttpServer HTTP_SERVER;
	typedef unisim::service::instrumenter::Instrumenter INSTRUMENTER;
	typedef unisim::service::debug::profiler::Profiler<DEBUGGER_CONFIG::ADDRESS> PROFILER;
	typedef unisim::service::tee::char_io::Tee<2> CHAR_IO_TEE;
	typedef unisim::service::web_terminal::WebTerminal WEB_TERMINAL;
	typedef unisim::kernel::logger::console::Printer LOGGER_CONSOLE_PRINTER;
	typedef unisim::kernel::logger::text_file::Writer LOGGER_TEXT_FILE_WRITER;
	typedef unisim::kernel::logger::http::Writer LOGGER_HTTP_WRITER;
	typedef unisim::kernel::logger::xml_file::Writer LOGGER_XML_FILE_WRITER;
	typedef unisim::kernel::logger::netstream::Writer LOGGER_NETSTREAM_WRITER;
	
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - 68HCS12X processor

	CPU *cpu;
	XGATE *xgate;

	MMC *mmc;

	CRG  *crg;
	ECT  *ect;

	ATD1 *atd1;
	ATD0 *atd0;

	PWM *pwm;

	PIT *pit;

	S12SCI *sci0, *sci1, *sci2, *sci3, *sci4, *sci5;

	S12SPIV4 *spi0, *spi1, *spi2;

	MSCAN *can0, *can1, *can2, *can3, *can4;

	//  - Memories
	RAM *global_ram;
	FLASH *global_flash;
	EEPROM *global_eeprom;

	// - Interrupt controller
	XINT *s12xint;

	RegistersTee* registersTee;

	MemoryImportExportTee* memoryImportExportTee;

#ifdef HAVE_RTBCOB
	RTBStub *rtbStub;
#else
	XML_ATD_PWM_STUB *xml_atd_pwm_stub;
#endif

	CAN_STUB *can_stub;
	TLE8264_2E *tranceiver0, *tranceiver1,*tranceiver2,*tranceiver3,*tranceiver4;

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	//  - Multiformat loader
	MultiFormatLoader<CPU_ADDRESS_TYPE>* loader;
	
	Debugger*                         debugger;        //< Debugger
	GDBServer<CPU_ADDRESS_TYPE>*      gdb_server;      //< GDB server
	PIMServer<CPU_ADDRESS_TYPE>*      pim_server;      //< PIM server
	InlineDebugger<CPU_ADDRESS_TYPE>* inline_debugger; //< Inline debugger
	MONITOR*                          monitor;         //< Monitoring tool: ARTiMon or EACSEL

	// - telnet
	TELNET* sci_telnet;
	TELNET* spi_telnet;

	// - Http Server
	HTTP_SERVER *http_server;
	
	// - Instrumenter
	INSTRUMENTER *instrumenter;
	
	// - Profiler
	PROFILER *profiler;

	// - Char I/O Tees
	CHAR_IO_TEE *sci_char_io_tee;
	CHAR_IO_TEE *spi_char_io_tee;
	
	// - Web Terminal
	WEB_TERMINAL *sci_web_terminal;
	WEB_TERMINAL *spi_web_terminal;

	//  - SystemC Time
	unisim::service::time::sc_time::ScTime *sim_time;
	//  - Host Time
	unisim::service::time::host_time::HostTime *host_time;

	//  - Logger Console Printer
	LOGGER_CONSOLE_PRINTER *logger_console_printer;
	//  - Logger Text File Writer
	LOGGER_TEXT_FILE_WRITER *logger_text_file_writer;
	//  - Logger HTTP Writer
	LOGGER_HTTP_WRITER *logger_http_writer;
	//  - Logger XML File Writer
	LOGGER_XML_FILE_WRITER *logger_xml_file_writer;
	//  - Logger TCP Network Stream Writer
	LOGGER_NETSTREAM_WRITER *logger_netstream_writer;
	
	bool enable_pim_server;
	bool enable_gdb_server;
	bool enable_inline_debugger;
	bool enable_monitor;
	bool enable_profiler;

	Parameter<bool> param_enable_pim_server;
	Parameter<bool> param_enable_gdb_server;
	Parameter<bool> param_enable_inline_debugger;
	Parameter<bool> param_enable_monitor;
	Parameter<bool> param_enable_profiler;

	string endian;
	Parameter<string> *param_endian;
	string program_counter_name;
	Parameter<string> *param_pc_reg_name;

	int exit_status;

	bool dump_parameters;
	bool dump_statistics;
	
	Parameter<bool> param_dump_parameters;
	Parameter<bool> param_dump_statistics;

	double null_stat_var;
	struct LoadRatioStatistic : public unisim::kernel::variable::Variable<double>
	{
		LoadRatioStatistic(Simulator& _sim);
		void Get(double& value);
		Simulator& sim;
	} stat_data_load_ratio;

	struct StoreRatioStatistic : public unisim::kernel::variable::Variable<double>
	{
		StoreRatioStatistic(Simulator& _sim);
		void Get(double& value);
		Simulator& sim;
	} stat_data_store_ratio;

	static void LoadBuiltInConfig(unisim::kernel::Simulator *simulator);

	double spent_time;
	bool isStop;

	physical_address_t entry_point;
	Parameter<physical_address_t> param_entry_point;
	
	virtual void SigInt();
};

#endif /* SIMULATOR_HH_ */
