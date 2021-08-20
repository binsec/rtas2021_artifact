/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: 
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */

#include <system.hh>
#include <string>
#include <ostream>
#include <unisim/kernel/kernel.hh>
#include <vector>
#include "fatformat.hh"
#include <iostream>

#include "driver.tcc"
#include "module.tcc"
#include "device.tcc"
#include "event.tcc"


using unisim::kernel::ServiceManager;
using std::string;
using std::endl;
using std::hex;
using std::dec;
using std::cout;
using std::cerr;



System::System() :
	driver(0),
	core_count(CONFIG::CORE_COUNT),
	memory(0),
	scheduler(0)
{
}

System::~System()
{
	// Do NOT delete objects
	//delete scheduler;
	//delete memory;
	//delete driver;
}

Driver<BaseConfig> * System::GetDriver()
{
	return driver;
}

bool ParseBool(char const * str)
{
	if(str == 0 || strcmp(str, "0") == 0
		|| strcmp(str, "false") == 0) {
		return false;
	}
	return true;
}



void System::SetVariableBool(char const * envName, char const * varName)
{
	char const * env;
	env = getenv(envName);
	if(env != 0) {
		bool b = ParseBool(env);
		for(int i = 0; i != core_count; ++i)
		{
			(*cores[i])[varName] = b;
		}
	}
}

void System::SetVariableUInt(char const * envName, char const * varName)
{
	char const * env;
	env = getenv(envName);
	if(env != 0) {
		unsigned int u = atoi(env);
		for(int i = 0; i != core_count; ++i)
		{
			(*cores[i])[varName] = u;
		}
	}
}



void System::Build()
{

	// Parse environment variables and set GPU parameters accordingly
	char const * env;
	env = getenv("CORE_COUNT");
	if(env != 0)
		core_count = atoi(env);
	
	// Initialize memory
	memory = new
		unisim::component::cxx::memory::ram::Memory<CONFIG::address_t>("memory");
	
	// Initialize cores
	cores.resize(core_count);
	for(int i = 0; i != core_count; ++i)
	{
		std::ostringstream name;
		name << "gpu_core_" << i;
		cores[i] = new CPU<CONFIG>(name.str().c_str(), 0, i, core_count);
		cores[i]->memory_import >> memory->memory_export;
	}
	
	scheduler = new CUDAScheduler<CONFIG>(core_count, "scheduler");

		
	for(int i = 0; i != core_count; ++i)
	{
		scheduler->Socket(i).registers_import >> cores[i]->registers_export;
		scheduler->Socket(i).configuration_import >> cores[i]->configuration_export;
		scheduler->Socket(i).shared_memory_import >> cores[i]->shared_memory_export;
		scheduler->Socket(i).reset_import >> cores[i]->reset_export;
		scheduler->Socket(i).run_import >> cores[i]->run_export;
		scheduler->Socket(i).stats_import >> cores[i]->instruction_stats_export;
	}
	

	SetVariableBool("TRACE_INSN", "trace-insn");
	SetVariableBool("TRACE_MASK", "trace-mask");
	SetVariableBool("TRACE_REG", "trace-reg");
	SetVariableBool("TRACE_REG_FLOAT", "trace-reg-float");
	SetVariableBool("TRACE_LOADSTORE", "trace-loadstore");
	SetVariableBool("TRACE_BRANCH", "trace-branch");
	SetVariableBool("TRACE_SYNC", "trace-sync");
	SetVariableBool("TRACE_RESET", "trace-reset");
	SetVariableBool("FILTER_TRACE", "filter-trace");
	SetVariableUInt("FILTER_TRACE_WARP", "filter-warp");
	SetVariableUInt("FILTER_TRACE_BLOCK", "filter-cta");

	env = getenv("TRACE_KERNEL_PARSING");
	if(env != 0) {
		bool b = ParseBool(env);
		Kernel<CONFIG>::trace_parsing = b;
		Module<CONFIG>::verbose = b;
		FatFormat::verbose = b;
	}

	env = getenv("TRACE_KERNEL_LOADING");
	if(env != 0)
		Kernel<CONFIG>::trace_loading = ParseBool(env);
	
	bool export_stats = false;
	char const * stats_prefix = "";
	
	env = getenv("EXPORT_STATS");
	if(env != 0) {
		export_stats = ParseBool(env);
    	SetVariableBool("EXPORT_STATS", "export-stats");
    	(*scheduler)["export-stats"] = export_stats;
	}

	env = getenv("EXPORT_STATS_PREFIX");
	if(env != 0)
		stats_prefix = env;

	// Init driver
	driver = new Driver<CONFIG>(memory, scheduler, core_count, &cores[0],
		export_stats, stats_prefix);


	if(!ServiceManager::Setup()) {
		cerr << "Error, setup failed\n";
		ServiceManager::Dump(cerr);
		assert(false);
	}
	

}

