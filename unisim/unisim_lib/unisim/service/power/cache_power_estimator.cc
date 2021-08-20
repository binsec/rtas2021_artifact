/*
 *  Copyright (c) 2007, 2010,
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
 */

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#if defined(HAVE_CONFIG_HH)
#include "unisim/service/power/config.hh"
#endif

#if defined(HAVE_CACTI4_2)
#include <cacti4_2.hh>
#endif

#include "unisim/service/power/cache_power_estimator.hh"
// #if defined(HAVE_CACTI4_2)
// #define cacti (*(Cacti4_2 *) opaque)
// #endif


namespace unisim {
namespace kernel {
namespace variable {

template <> Variable<unisim::service::power::CachePowerEstimator::AccessMode>::Variable(const char *_name, Object *_object, unisim::service::power::CachePowerEstimator::AccessMode& _storage, Type type, const char *_description) :
VariableBase(_name, _object, type, _description), storage(&_storage)
{
	AddEnumeratedValue("normal");
	AddEnumeratedValue("sequential");
	AddEnumeratedValue("fast");
	Initialize();
}

template <>
const char *Variable<unisim::service::power::CachePowerEstimator::AccessMode>::GetDataTypeName() const
{
	return "cache power estimator access mode";
}

template <>
unsigned int Variable<unisim::service::power::CachePowerEstimator::AccessMode>::GetBitSize() const
{
	return 0;
}

template <> Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator bool () const { return false; }
template <> Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator long long () const { return (long long)(*storage); }
template <> Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator unsigned long long () const { return (unsigned long long)(*storage); }
template <> Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator double () const { return (double)(unsigned int)(*storage); }
template <> Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator std::string () const
{
	switch(*storage)
	{
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL: return std::string("normal");
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL: return std::string("sequential");
		case unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST: return std::string("fast");
	}
	return std::string("?");
}

template <> VariableBase& Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		switch(value)
		{
			case unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL:
				SetModified((unsigned long long) *storage != value);
				*storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL;
				break;
			case unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST:
				SetModified((unsigned long long) *storage != value);
				*storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST;
				break;
			case unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL:
			default:
				SetModified((unsigned long long) *storage != value);
				*storage = unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL;
				break;
		}
	}
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (bool value)
{
	return (*this) = (unsigned long long)(value ? 1 : 0);
}

template <> VariableBase& Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (long long value)
{
	return (*this) = (unsigned long long) value;
}

template <> VariableBase& Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (double value)
{
	return (*this) = (unsigned long long) value;
}

template <> VariableBase& Variable<unisim::service::power::CachePowerEstimator::AccessMode>::operator = (const char *value)
{
	if(IsMutable())
	{
		if(strcmp(value, "normal") == 0)
		{
			unisim::service::power::CachePowerEstimator::AccessMode tmp = unisim::service::power::CachePowerEstimator::ACCESS_MODE_NORMAL;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
		else if(strcmp(value, "sequential") == 0)
		{
			unisim::service::power::CachePowerEstimator::AccessMode tmp = unisim::service::power::CachePowerEstimator::ACCESS_MODE_SEQUENTIAL;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
		else if(strcmp(value, "fast") == 0)
		{
			unisim::service::power::CachePowerEstimator::AccessMode tmp = unisim::service::power::CachePowerEstimator::ACCESS_MODE_FAST;
			SetModified(*storage != tmp);
			*storage = tmp;
		}
	}
	return *this;
}

template class Variable<unisim::service::power::CachePowerEstimator::AccessMode>;

} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim

namespace unisim {
namespace service {
namespace power {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

CachePowerEstimator::CachePowerEstimator(const char *name, Object *parent) :
	Object(name, parent, "this service implements an SRAM/Cache power estimator (dynamic energy and leakage power)"),
	Service<unisim::service::interfaces::CachePowerEstimator>(name, parent),
	Service<unisim::service::interfaces::PowerMode>(name, parent),
	Client<unisim::service::interfaces::Time>(name, parent),
	power_estimator_export("power-estimator-export", this),
	power_mode_export("power-mode-export", this),
	time_import("time-import", this),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose),
	current_profile(0),
	p_cache_size(0),
	p_line_size(0),
	p_associativity(0),
	p_rw_ports(0),
	p_excl_read_ports(0),
	p_excl_write_ports(0),
	p_single_ended_read_ports(0),
	p_banks(0),
	p_tech_node(0.0),
	p_output_width(0),
	p_tag_width(0),
	p_access_mode(ACCESS_MODE_NORMAL),
	param_cache_size("cache-size", this, p_cache_size, "cache size (in bytes)"),
	param_line_size("line-size", this, p_line_size,
			"cache line size (in bytes)"),
	param_associativity("associativity", this, p_associativity,
			"cache associativity level (0 for fully associative)"),
	param_rw_ports("rw-ports", this, p_rw_ports, "number of read-write ports"),
	param_excl_read_ports("excl-read-ports", this, p_excl_read_ports,
			"number of read only ports"),
	param_excl_write_ports("excl-write-ports", this, p_excl_write_ports,
			"number of write only ports"),
	param_single_ended_read_ports("single-ended-read-ports", this,
			p_single_ended_read_ports, "number of single ended read ports"),
	param_banks("banks", this, p_banks, "number of cache banks"),
	param_tech_node("tech-node", this, p_tech_node,
			"size of the technology node (in nm)"),
	param_output_width("output-width", this, p_output_width,
			"cache output width (in number of bits)"),
	param_tag_width("tag-width", this, p_tag_width,
			"tag width in bits"),
	param_access_mode("access-mode", this, p_access_mode,
			"access mode type (normal, sequential, fast)"),
	dynamic_energy(&profiles),
	dynamic_power(&profiles, &time_import),
	leakage_power(&time_stamp, &current_profile, &profiles, &time_import),
	stat_dynamic_energy("dynamic-energy", this, dynamic_energy, "cache dynamic energy (in J)"),
	stat_dynamic_power("dynamic-power", this, dynamic_power, "cache dynamic power (in W)"),
	stat_leakage_power("leakage-power", this, leakage_power, "cache leakage power (in W)"),
	stat_total_power("total-power", this, "+", &stat_dynamic_power, &stat_leakage_power, 0,
			"cache total (dynamic+leakage) power (in W)"),
	Ndwl(0),
	Ndbl(0),
	Ntwl(0),
	Ntbl(0),
	Ntspd(0),
	Nspd(0.0),
	min_cycle_time(0),
	time_stamp(0.0)
{
	param_cache_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_line_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_associativity.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_rw_ports.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_excl_read_ports.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_excl_write_ports.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_single_ended_read_ports.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_banks.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_output_width.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_tag_width.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	cacti = 0;
}

CachePowerEstimator::~CachePowerEstimator()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		delete prof;
	}
	
#if defined(HAVE_CACTI4_2)
	if(cacti)
	{
		delete cacti;
	}
#endif
}

void CachePowerEstimator::SetPowerMode(unsigned int cycle_time, unsigned int voltage)
{
#if defined(HAVE_CACTI4_2)
	double new_time_stamp = time_import->GetTime(); // in seconds
	
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	
	prof_iter = profiles.find(CacheProfileKey(cycle_time, voltage));
	
	if(prof_iter != profiles.end())
	{
		if(new_time_stamp > time_stamp)
		{
			// accumulate the time spent in the current profile
			current_profile->duration = current_profile->duration + (new_time_stamp - time_stamp);
			time_stamp = new_time_stamp;
		}
		// switch to the new profile
		current_profile = (*prof_iter).second;
		return;
	}
	
	Cacti4_2::total_result_type total_result;
	
	total_result = cacti->cacti_interface(
			p_cache_size,
			p_line_size,
			p_associativity,
			p_rw_ports,
			p_excl_read_ports,
			p_excl_write_ports,
			p_single_ended_read_ports,
			p_banks,
			p_tech_node / 1000.0,
			p_output_width,
			p_tag_width ? 1:0,
			p_tag_width,
			p_access_mode,
			0,
			Ndwl,
			Ndbl,
			Ntwl,
			Ntbl,
			Ntspd,
			Nspd,
			(double) voltage / 1e3 /* V */);
	
	//cacti->output_data(&total_result.result,&total_result.area,&total_result.params);
	
	if(cycle_time < min_cycle_time)
	{
		logger << DebugWarning << "A cycle of " << cycle_time << " ps is too low for the simulated hardware !" << EndDebugWarning;
	}
	
	CacheProfile *prof = new CacheProfile(
		cycle_time,
		voltage,
		total_result.result.total_power_allbanks.readOp.dynamic,
		total_result.result.total_power_allbanks.writeOp.dynamic,
		total_result.result.total_power_allbanks.readOp.leakage);
	
	profiles[*prof] = prof;
	current_profile = prof;
	time_stamp = new_time_stamp;

// 	cerr << "(cycle-time=" << prof->cycle_time
// 			<<" ns, min-cycle-time=" << (total_result.result.cycle_time * 1e9)
// 			<<  " ns, frequency=" << prof->frequency
// 			<< " Mhz, voltage=" << ((double) prof->voltage / 1e3)
// 			<< " V, dyn-energy-per-read=" << (prof->dyn_energy_per_read * 1e9)
// 			<< " nJ, dyn-energy-per-write=" << (prof->dyn_energy_per_write * 1e9)
// 			<< " nJ, max-dyn-power=" << (prof->dyn_energy_per_read * prof->frequency * 1e6)
// 			<< " W, leak-power=" << prof->leak_power << " W)" << endl;
#endif
}

unsigned int CachePowerEstimator::GetMinCycleTime()
{
	return min_cycle_time;
}

unsigned int CachePowerEstimator::GetDefaultVoltage()
{
	return default_voltage;
}

void CachePowerEstimator::ReportReadAccess()
{
	current_profile->read_accesses++;
}

void CachePowerEstimator::ReportWriteAccess()
{
	current_profile->write_accesses++;
}

double CachePowerEstimator::GetDynamicEnergy()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	double dyn_energy = 0.0;

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		
/*		cerr << "read_accesses = " << prof->read_accesses << endl;
		cerr << "write_accesses = " << prof->write_accesses << endl;*/
		dyn_energy = dyn_energy + prof->dyn_energy_per_read * prof->read_accesses + prof->dyn_energy_per_write * prof->write_accesses;
	}
	return dyn_energy;
}

double CachePowerEstimator::GetDynamicPower()
{
	double dyn_energy = GetDynamicEnergy(); // in J
	double total_time = time_import->GetTime(); // in seconds
	return dyn_energy / total_time;
}

double CachePowerEstimator::GetLeakagePower()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;
	double leakage_power = 0.0;
	double total_time = time_import->GetTime(); // in seconds

	if(total_time > time_stamp)
	{
		// accumulate the time spent in the current profile
		current_profile->duration = current_profile->duration + (total_time - time_stamp);
		time_stamp = total_time;
	}

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		
		leakage_power = leakage_power + prof->leak_power * (prof->duration / total_time);
	}
	return leakage_power;
}

bool CachePowerEstimator::BeginSetup()
{
	map<CacheProfileKey, CacheProfile *>::iterator prof_iter;

	for(prof_iter = profiles.begin(); prof_iter != profiles.end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		delete prof;
	}
	
#if defined(HAVE_CACTI4_2)
	if(cacti)
	{
		delete cacti;
		cacti = 0;
	}
#endif
	return true;
}

bool CachePowerEstimator::SetupCacti()
{
	if(!profiles.empty()) return true;
#if defined(HAVE_CACTI4_2)
	if(!time_import)
	{
		logger << DebugError << "no time service is connected." << EndDebugError;
		return false;
	}

	cacti = new Cacti4_2();

	if(verbose)
	{
		logger << DebugInfo << ((double) p_cache_size / 1024.0) << " KB cache" << EndDebugInfo;
		logger << DebugInfo << p_line_size << " bytes per line" << EndDebugInfo;
		logger << DebugInfo;
		if(p_associativity)
			logger << p_associativity << " way/set associative";
		else
			logger << "fully associative";
		logger << EndDebugInfo;
		logger << DebugInfo << p_rw_ports << " read/write ports" << EndDebugInfo;
		logger << DebugInfo << p_excl_read_ports << " read-only ports" << EndDebugInfo;
		logger << DebugInfo << p_excl_write_ports << " write-only ports" << EndDebugInfo;
		logger << DebugInfo << p_single_ended_read_ports << " single-ended-read-ports" << EndDebugInfo;
		logger << DebugInfo << p_banks << " banks" << EndDebugInfo;
		logger << DebugInfo << p_tech_node << " nm tech-node" << EndDebugInfo;
		logger << DebugInfo << p_output_width << "-bit output" << EndDebugInfo;
		if(p_tag_width) logger  << p_tag_width << "-bit tag" << EndDebugInfo;
		logger << DebugInfo;
		switch(p_access_mode)
		{
			case ACCESS_MODE_NORMAL: logger << "normal"; break;
			case ACCESS_MODE_SEQUENTIAL: logger << "sequential"; break;
			case ACCESS_MODE_FAST: logger << "fast"; break;
			default: logger << "?";
		}
		logger << " access mode" << EndDebugInfo;
	}

	Cacti4_2::total_result_type total_result;
	double VddPow;

	total_result = cacti->cacti_interface(
			p_cache_size,
	p_line_size,
	p_associativity,
	p_rw_ports,
	p_excl_read_ports,
	p_excl_write_ports,
	p_single_ended_read_ports,
	p_banks,
	p_tech_node / 1000.0,
	p_output_width,
	p_tag_width ? 1:0,
	p_tag_width,
	p_access_mode,
	0,
	&VddPow);

	//cacti->output_data(&total_result.result,&total_result.area,&total_result.params);

	Ndwl = total_result.result.best_Ndwl;
	Ndbl = total_result.result.best_Ndbl;
	Ntwl = total_result.result.best_Ntwl;
	Ntbl = total_result.result.best_Ntbl;
	Ntspd = total_result.result.best_Ntspd;
	Nspd = total_result.result.best_Nspd;
	
	min_cycle_time = (unsigned int)(total_result.result.cycle_time * 1e12);
	default_voltage = (unsigned int) floor(VddPow * 1000.0);
	
	if(verbose)
	{
		logger << DebugInfo << "Ndwl=" << Ndwl << ", Ndbl=" << Ndbl
				<< ", Ntwl=" << Ntbl << ", Ntspd=" << Ntspd
				<< ", Nspd=" << Nspd << ", min cycle time=" << min_cycle_time << " ps, default VddPow=" << VddPow << " V" << EndDebugInfo;
	}
	
	SetPowerMode(min_cycle_time, default_voltage); // this create and select the default power profile
	return true;
#else
	logger << DebugError << "Cacti 4.2 is not available." << EndDebugError;
	return false;
#endif
}

bool CachePowerEstimator::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &power_estimator_export) return SetupCacti();
	if(srv_export == &power_mode_export) return SetupCacti();
	
	logger << DebugError << "Internal error" << EndDebugError;
	
	return false;
}

} // end of namespace power
} // end of namespace service
} // end of namespace unisim
