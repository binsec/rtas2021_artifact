/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_SERVICE_POWER_CACHE_POWER_ESTIMATOR_HH__
#define __UNISIM_SERVICE_POWER_CACHE_POWER_ESTIMATOR_HH__

#include <map>
#include <inttypes.h>

#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/cache_power_estimator.hh"
#include "unisim/service/interfaces/power_mode.hh"
#include "unisim/service/interfaces/time.hh"
#include "unisim/service/power/cache_profile.hh"
#include "unisim/service/power/cache_dynamic_energy.hh"
#include "unisim/service/power/cache_dynamic_power.hh"
#include "unisim/service/power/cache_leakage_power.hh"

namespace unisim {
namespace service {
namespace power {

class Cacti4_2; // forward declaration

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using std::map;

class CachePowerEstimator :
	public Service<unisim::service::interfaces::CachePowerEstimator>,
	public Service<unisim::service::interfaces::PowerMode>,
	public Client<unisim::service::interfaces::Time>
{
public:
	typedef enum { ACCESS_MODE_NORMAL, ACCESS_MODE_SEQUENTIAL, ACCESS_MODE_FAST } AccessMode;
	
	ServiceExport<unisim::service::interfaces::CachePowerEstimator> power_estimator_export;
	ServiceExport<unisim::service::interfaces::PowerMode> power_mode_export;
	ServiceImport<unisim::service::interfaces::Time> time_import;
	
 	CachePowerEstimator(const char *name, Object *parent = 0);
 	virtual ~CachePowerEstimator();

 	virtual bool BeginSetup();
 	virtual bool Setup(ServiceExportBase *srv_export);

	// Cache power estimator interface
	virtual void ReportReadAccess();
	virtual void ReportWriteAccess();
	virtual double GetDynamicEnergy();
	virtual double GetDynamicPower();
	virtual double GetLeakagePower();
	
	// Power mode interface
	virtual void SetPowerMode(unsigned int cycle_time, unsigned int voltage);
	virtual unsigned int GetMinCycleTime();
	virtual unsigned int GetDefaultVoltage();

private:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
	CacheProfile *current_profile;
	map<CacheProfileKey, CacheProfile *> profiles;
	
	// Cache Power estimator service parameters
	unsigned int p_cache_size; // cache size in bytes
	unsigned int p_line_size;
	unsigned int p_associativity;
	unsigned int p_rw_ports;
	unsigned int p_excl_read_ports;
	unsigned int p_excl_write_ports;
	unsigned int p_single_ended_read_ports;
	unsigned int p_banks;
	double p_tech_node;
	unsigned int p_output_width;
	unsigned int p_tag_width;
	AccessMode p_access_mode;
	
	Parameter<unsigned int> param_cache_size;
	Parameter<unsigned int> param_line_size;
	Parameter<unsigned int> param_associativity;
	Parameter<unsigned int> param_rw_ports;
	Parameter<unsigned int> param_excl_read_ports;
	Parameter<unsigned int> param_excl_write_ports;
	Parameter<unsigned int> param_single_ended_read_ports;
	Parameter<unsigned int> param_banks;
	Parameter<double> param_tech_node;
	Parameter<unsigned int> param_output_width;
	Parameter<unsigned int> param_tag_width;
	Parameter<AccessMode> param_access_mode;
	
	CacheDynamicEnergy dynamic_energy;
	CacheDynamicPower dynamic_power;
	CacheLeakagePower leakage_power;
	Statistic<CacheDynamicEnergy> stat_dynamic_energy;
	Statistic<CacheDynamicPower> stat_dynamic_power;
	Statistic<CacheLeakagePower> stat_leakage_power;
	StatisticFormula<double> stat_total_power;

	int Ndwl;
	int Ndbl;
	int Ntwl;
	int Ntbl;
	int Ntspd;
	double Nspd;
	
	unsigned int min_cycle_time;
	unsigned int default_voltage;
	
	double time_stamp; // in seconds
	
	/* total results */
	Cacti4_2 *cacti;
	
	bool SetupCacti();
};

} // end of namespace power
} // end of namespace service
} // end of namespace unisim

#endif
