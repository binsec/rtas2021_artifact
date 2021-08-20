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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include "unisim/service/power/cache_dynamic_energy.hh"
#include <unisim/kernel/variable/variable.hh>
#include <sstream>

namespace unisim {
namespace service {
namespace power {

CacheDynamicEnergy ::
CacheDynamicEnergy (const map<CacheProfileKey, CacheProfile *> *_profiles) :
profiles(_profiles)
{
}

CacheDynamicEnergy ::
~CacheDynamicEnergy ()
{
	profiles = 0;
}

double
CacheDynamicEnergy ::
GetDynamicEnergy() const
{
	map<CacheProfileKey, CacheProfile *>::const_iterator prof_iter;
	double dyn_energy = 0.0;

	for ( prof_iter = profiles->begin();
			prof_iter != profiles->end();
			prof_iter++)
	{
		CacheProfile *prof = (*prof_iter).second;

/*		cerr << "read_accesses = " << prof->read_accesses << endl;
		cerr << "write_accesses = " << prof->write_accesses << endl;*/
		dyn_energy = dyn_energy +
				prof->dyn_energy_per_read * prof->read_accesses +
				prof->dyn_energy_per_write * prof->write_accesses;
	}
	return dyn_energy;
}

} // end of namespace power
} // end of namespace service
} // end of namespace unisim

#include "unisim/kernel/kernel.hh"

namespace unisim {
namespace kernel {
namespace variable {

template <>
Variable<unisim::service::power::CacheDynamicEnergy>::Variable(const char *_name, Object *_object, unisim::service::power::CacheDynamicEnergy& _storage, Type type, const char *_description) :
VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
}

template <>
const char *Variable<unisim::service::power::CacheDynamicEnergy>::GetDataTypeName() const
{
	return "cache dynamic energy estimator";
}

template <>
unsigned int Variable<unisim::service::power::CacheDynamicEnergy>::GetBitSize() const
{
	return 0;
}

template <>
Variable<unisim::service::power::CacheDynamicEnergy>::operator bool () const
{
	if ( storage->GetDynamicEnergy() == 0.0) return false;
	return true;
}
template <>
Variable<unisim::service::power::CacheDynamicEnergy>::operator long long () const
{
	return (long long)storage->GetDynamicEnergy();
}

template <>
Variable<unisim::service::power::CacheDynamicEnergy>::operator unsigned long long () const
{
	return (unsigned long long)storage->GetDynamicEnergy();
}

template <>
Variable<unisim::service::power::CacheDynamicEnergy>::operator double () const
{
	return storage->GetDynamicEnergy();
}

template <>
Variable<unisim::service::power::CacheDynamicEnergy>::operator std::string () const
{
	std::stringstream data;
	data << storage->GetDynamicEnergy();
	return data.str();
}

template <> VariableBase& Variable<unisim::service::power::CacheDynamicEnergy>::operator = (unsigned long long value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheDynamicEnergy>::operator = (bool value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheDynamicEnergy>::operator = (long long value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheDynamicEnergy>::operator = (double value)
{
	return *this;
}

template <> VariableBase& Variable<unisim::service::power::CacheDynamicEnergy>::operator = (const char *value)
{
	return *this;
}

template class Variable<unisim::service::power::CacheDynamicEnergy>;

} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim
