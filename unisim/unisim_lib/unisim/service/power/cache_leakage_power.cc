/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "unisim/service/power/cache_leakage_power.hh"
#include <sstream>

namespace unisim {
namespace service {
namespace power {

CacheLeakagePower ::
CacheLeakagePower (const double *_time_stamp,
		CacheProfile **_current_profile,
		const map<CacheProfileKey, CacheProfile *> *_profiles,
		unisim::kernel::ServiceImport<unisim::service::interfaces::Time> *_time_import) :
time_stamp(_time_stamp),
current_profile(_current_profile),
profiles(_profiles),
time_import(_time_import)
{
}

CacheLeakagePower ::
~CacheLeakagePower ()
{
	time_stamp = 0;
	current_profile = 0;
	profiles = 0;
	time_import = 0;
}

double
CacheLeakagePower ::
GetLeakagePower() const
{
	map<CacheProfileKey, CacheProfile *>::const_iterator prof_iter;
	double leakage_power = 0.0;
	double total_time = (*time_import)->GetTime(); // in seconds

	for(prof_iter = profiles->begin(); prof_iter != profiles->end(); prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;
		double duration = prof->duration;
		if ( prof == *current_profile )
		{
			if ( total_time > *time_stamp )
			{
				duration = prof->duration + (total_time - *time_stamp);
			}
		}

		leakage_power = leakage_power + prof->leak_power * (duration / total_time);
	}
	return leakage_power;
}

} // end of namespace power
} // end of namespace service
} // end of namespace unisim

#include <unisim/kernel/variable/variable.hh>
#include "unisim/kernel/kernel.hh"

namespace unisim {
namespace kernel {
namespace variable {

template <> Variable<unisim::service::power::CacheLeakagePower>::Variable(const char *_name, Object *_object, unisim::service::power::CacheLeakagePower& _storage, Type type, const char *_description) :
VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
}

template <>
const char *Variable<unisim::service::power::CacheLeakagePower>::GetDataTypeName() const
{
	return "cache leakage power estimator";
}

template <>
unsigned int Variable<unisim::service::power::CacheLeakagePower>::GetBitSize() const
{
	return 0;
}

template <>
Variable<unisim::service::power::CacheLeakagePower>::operator bool () const
{
	if ( storage->GetLeakagePower() == 0.0) return false;
	return true;
}
template <>
Variable<unisim::service::power::CacheLeakagePower>::operator long long () const
{
	return (long long)storage->GetLeakagePower();
}

template <>
Variable<unisim::service::power::CacheLeakagePower>::operator unsigned long long () const
{
	return (unsigned long long)storage->GetLeakagePower();
}

template <>
Variable<unisim::service::power::CacheLeakagePower>::operator double () const
{
	return storage->GetLeakagePower();
}

template <>
Variable<unisim::service::power::CacheLeakagePower>::operator std::string () const
{
	std::stringstream data;
	data << storage->GetLeakagePower();
	return data.str();
}

template <> VariableBase& Variable<unisim::service::power::CacheLeakagePower>::operator = (unsigned long long value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheLeakagePower>::operator = (bool value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheLeakagePower>::operator = (long long value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheLeakagePower>::operator = (double value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheLeakagePower>::operator = (const char *value)
{
	return *this;
}

template class Variable<unisim::service::power::CacheLeakagePower>;

} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim
