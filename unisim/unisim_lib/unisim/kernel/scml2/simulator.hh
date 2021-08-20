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

#ifndef __UNISIM_KERNEL_SCML2_SIMULATOR_HH__
#define __UNISIM_KERNEL_SCML2_SIMULATOR_HH__

#include <scml2.h>
#include <scml2_logging.h>
#include <scmlinc/scml_property_server_if.h>
#include <unisim/kernel/tlm2/simulator.hh>

namespace unisim {
namespace kernel {
namespace scml2 {

class ScmlPropertyServer : public scml_property_server_if
{
public:
	ScmlPropertyServer(unisim::kernel::Simulator *sim);
	virtual ~ScmlPropertyServer();
	
	virtual long long getIntProperty(const std::string & name);
	virtual unsigned long long getUIntProperty(const std::string & name);
	virtual bool getBoolProperty(const std::string & name);
	virtual std::string getStringProperty(const std::string & name);
	virtual double getDoubleProperty(const std::string & name);
	
private:
	unisim::kernel::Simulator *sim;
	
	std::vector<long long *> int_values;
	std::vector<unsigned long long *> uint_values;
	std::vector<bool *> bool_values;
	std::vector<std::string *> string_values;
	std::vector<double *> double_values;
	std::map<std::string, unisim::kernel::variable::Variable<long long> *> int_variables;
	std::map<std::string, unisim::kernel::variable::Variable<unsigned long long> *> uint_variables;
	std::map<std::string, unisim::kernel::variable::Variable<bool> *> bool_variables;
	std::map<std::string, unisim::kernel::variable::Variable<std::string> *> string_variables;
	std::map<std::string, unisim::kernel::variable::Variable<double> *> double_variables;
	std::vector<unisim::kernel::Object *> auto_objects;
	
	unisim::kernel::Object *GetObject(unisim::kernel::Object *object, const std::string& leaf_hierarchical_name);
	unisim::kernel::Object *GetObject(const std::string& leaf_hierarchical_name);
	unisim::kernel::Object *GetOwner(const std::string & name);
	static bool GetLeafName(const std::string& hierarchical_name, std::string& leaf_name);
	//static std::string GetRootName(const std::string& hierarchical_name);
};

class Simulator : public unisim::kernel::tlm2::Simulator
{
public:
	Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::Simulator *simulator) = 0);
	virtual ~Simulator();
protected:
	bool scml_debug;
	unisim::kernel::variable::Parameter<bool> param_scml_debug;
	bool scml_note;
	unisim::kernel::variable::Parameter<bool> param_scml_note;
private:
	ScmlPropertyServer *custom_property_server;
	::scml2::logging::logger_base *unisim_debug_info_logger;
	::scml2::logging::logger_base *unisim_debug_warning_logger;
	::scml2::logging::logger_base *unisim_debug_error_logger;
};

} // end of namespace scml2
} // end of namespace kernel
} // end of namespace unisim

#endif
