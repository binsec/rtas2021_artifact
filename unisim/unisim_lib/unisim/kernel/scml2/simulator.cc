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

#include <unisim/kernel/scml2/simulator.hh>

namespace unisim {
namespace kernel {
namespace scml2 {

ScmlPropertyServer::ScmlPropertyServer(unisim::kernel::Simulator *_sim)
	: sim(_sim)
	, int_values()
	, uint_values()
	, bool_values()
	, string_values()
	, double_values()
	, int_variables()
	, uint_variables()
	, bool_variables()
	, string_variables()
	, double_variables()
	, auto_objects()
{
}

ScmlPropertyServer::~ScmlPropertyServer()
{
	std::vector<long long *>::iterator int_values_it;
	for(int_values_it = int_values.begin(); int_values_it != int_values.end(); int_values_it++)
	{
		long long *int_value = *int_values_it;
		delete int_value;
	}

	std::vector<unsigned long long *>::iterator uint_values_it;
	for(uint_values_it = uint_values.begin(); uint_values_it != uint_values.end(); uint_values_it++)
	{
		unsigned long long *uint_value = *uint_values_it;
		delete uint_value;
	}

	std::vector<bool *>::iterator bool_values_it;
	for(bool_values_it = bool_values.begin(); bool_values_it != bool_values.end(); bool_values_it++)
	{
		bool *bool_value = *bool_values_it;
		delete bool_value;
	}

	std::vector<std::string *>::iterator string_values_it;
	for(string_values_it = string_values.begin(); string_values_it != string_values.end(); string_values_it++)
	{
		std::string *string_value = *string_values_it;
		delete string_value;
	}

	std::vector<double *>::iterator double_values_it;
	for(double_values_it = double_values.begin(); double_values_it != double_values.end(); double_values_it++)
	{
		double *double_value = *double_values_it;
		delete double_value;
	}

	std::map<std::string, unisim::kernel::variable::Variable<long long> *>::iterator int_variable_it;
	for(int_variable_it = int_variables.begin(); int_variable_it != int_variables.end(); int_variable_it++)
	{
		unisim::kernel::variable::Variable<long long> *int_variable = (*int_variable_it).second;
		delete int_variable;
	}

	std::map<std::string, unisim::kernel::variable::Variable<unsigned long long> *>::iterator uint_variable_it;
	for(uint_variable_it = uint_variables.begin(); uint_variable_it != uint_variables.end(); uint_variable_it++)
	{
		unisim::kernel::variable::Variable<unsigned long long> *uint_variable = (*uint_variable_it).second;
		delete uint_variable;
	}

	std::map<std::string, unisim::kernel::variable::Variable<bool> *>::iterator bool_variable_it;
	for(bool_variable_it = bool_variables.begin(); bool_variable_it != bool_variables.end(); bool_variable_it++)
	{
		unisim::kernel::variable::Variable<bool> *bool_variable = (*bool_variable_it).second;
		delete bool_variable;
	}

	std::map<std::string, unisim::kernel::variable::Variable<std::string> *>::iterator string_variable_it;
	for(string_variable_it = string_variables.begin(); string_variable_it != string_variables.end(); string_variable_it++)
	{
		unisim::kernel::variable::Variable<std::string> *string_variable = (*string_variable_it).second;
		delete string_variable;
	}

	std::map<std::string, unisim::kernel::variable::Variable<double> *>::iterator double_variable_it;
	for(double_variable_it = double_variables.begin(); double_variable_it != double_variables.end(); double_variable_it++)
	{
		unisim::kernel::variable::Variable<double> *double_variable = (*double_variable_it).second;
		delete double_variable;
	}
	
	// deleting automatically created objects (leaf objects first)
	while(auto_objects.size())
	{
		std::vector<unisim::kernel::Object *>::iterator auto_object_it = auto_objects.begin();
		do
		{
			unisim::kernel::Object *auto_object = *auto_object_it;
				
			const std::list<unisim::kernel::Object *>& leaf_objects = auto_object->GetLeafs();
				
			if(leaf_objects.empty())
			{
				delete auto_object;
				auto_objects.erase(auto_object_it);
			}
			else
			{
				auto_object_it++;
			}
		}
		while(auto_object_it != auto_objects.end());
	}
}

unisim::kernel::Object *ScmlPropertyServer::GetObject(unisim::kernel::Object *object, const std::string& leaf_hierarchical_name)
{
	std::size_t hierarchical_delimiter_pos = leaf_hierarchical_name.find_first_of('.');
	
	std::string child_name = leaf_hierarchical_name.substr(0, hierarchical_delimiter_pos);
	
	const std::list<unisim::kernel::Object *>& leaf_objects = object->GetLeafs();
	
	std::list<unisim::kernel::Object *>::const_iterator it;
	
	unisim::kernel::Object *found_child = 0;
	
	for(it = leaf_objects.begin(); it != leaf_objects.end(); it++)
	{
		unisim::kernel::Object *child = *it;
		
		if(child_name.compare(child->GetObjectName()) == 0)
		{
			// found child
			found_child = child;
			break;
		}
	}
	
	if(!found_child)
	{
		// not found
		found_child = new unisim::kernel::Object(child_name.c_str(), object);
		
		auto_objects.push_back(found_child);
	}
	
	return (hierarchical_delimiter_pos == std::string::npos) ? found_child : GetObject(found_child, leaf_hierarchical_name.substr(hierarchical_delimiter_pos + 1));
}

unisim::kernel::Object *ScmlPropertyServer::GetObject(const std::string& leaf_hierarchical_name)
{
	std::size_t hierarchical_delimiter_pos = leaf_hierarchical_name.find_first_of('.');
	std::string root_object_name = leaf_hierarchical_name.substr(0, hierarchical_delimiter_pos);
	
	std::list<unisim::kernel::Object *> root_objects;
	
	sim->GetRootObjects(root_objects);
	
	std::list<unisim::kernel::Object *>::const_iterator it;
	
	unisim::kernel::Object *found_root_object = 0;
	
	for(it = root_objects.begin(); it != root_objects.end(); it++)
	{
		unisim::kernel::Object *root_object = *it;
		
		if(root_object_name.compare(root_object->GetObjectName()) == 0)
		{
			// found root object
			found_root_object = root_object;
			break;
		}
	}
	
	if(!found_root_object)
	{
		// not found
		found_root_object = new unisim::kernel::Object(root_object_name.c_str());
		
		auto_objects.push_back(found_root_object);
	}
	
	return (hierarchical_delimiter_pos == std::string::npos) ? found_root_object : GetObject(found_root_object, leaf_hierarchical_name.substr(hierarchical_delimiter_pos + 1));
}

unisim::kernel::Object *ScmlPropertyServer::GetOwner(const std::string & name)
{
	std::size_t hierarchical_delimiter_pos = name.find_last_of('.');
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		return 0;
	}
	
	return GetObject(name.substr(0, hierarchical_delimiter_pos));
}

bool ScmlPropertyServer::GetLeafName(const std::string& hierarchical_name, std::string& leaf_name)
{
	std::size_t hierarchical_delimiter_pos = hierarchical_name.find_last_of('.');
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		return false;
	}
	
	leaf_name = hierarchical_name.substr(hierarchical_delimiter_pos + 1);
	
	return true;
}

long long ScmlPropertyServer::getIntProperty(const std::string & name)
{
	std::string variable_name;
	
	if(!GetLeafName(name, variable_name)) return 0;
	
	unisim::kernel::variable::Variable<long long> *int_variable = 0;
	
	std::map<std::string, unisim::kernel::variable::Variable<long long> *>::const_iterator it = int_variables.find(name);
	
	if(it != int_variables.end())
	{
		int_variable = (*it).second;
	}
	else
	{
		unisim::kernel::Object *owner = GetOwner(name);
		std::size_t value_index = int_values.size();
		int_values.resize(value_index + 1);
		
		int_values[value_index] = new long long();

		int_variable = new unisim::kernel::variable::Variable<long long>(variable_name.c_str(), owner, *int_values[value_index], unisim::kernel::VariableBase::VAR_PARAMETER, "scml property (int)");
		int_variable->SetFormat(unisim::kernel::VariableBase::FMT_DEC);
		
		int_variables[name] = int_variable;
	}
	
	long long property_value = (long long)(*int_variable);
	
	return property_value;
}

unsigned long long ScmlPropertyServer::getUIntProperty(const std::string & name)
{
	std::string variable_name;
	
	if(!GetLeafName(name, variable_name)) return 0;

	unisim::kernel::variable::Variable<unsigned long long> *uint_variable = 0;
	
	std::map<std::string, unisim::kernel::variable::Variable<unsigned long long> *>::const_iterator it = uint_variables.find(name);
	
	if(it != uint_variables.end())
	{
		uint_variable = (*it).second;
	}
	else
	{
		unisim::kernel::Object *owner = GetOwner(name);
	
		std::size_t value_index = uint_values.size();
		uint_values.resize(value_index + 1);
		
		uint_values[value_index] = new unsigned long long();
		
		uint_variable = new unisim::kernel::variable::Variable<unsigned long long>(variable_name.c_str(), owner, *uint_values[value_index], unisim::kernel::VariableBase::VAR_PARAMETER, "scml property (uint)");
		uint_variable->SetFormat(unisim::kernel::VariableBase::FMT_DEC);
		
		uint_variables[name] = uint_variable;
	}
	
	unsigned long long property_value = (unsigned long long)(*uint_variable);
	
	return property_value;
}

bool ScmlPropertyServer::getBoolProperty(const std::string & name)
{
	std::string variable_name;
	
	if(!GetLeafName(name, variable_name)) return false;

	unisim::kernel::variable::Variable<bool> *bool_variable = 0;
	
	std::map<std::string, unisim::kernel::variable::Variable<bool> *>::const_iterator it = bool_variables.find(name);
	
	if(it != bool_variables.end())
	{
		bool_variable = (*it).second;
	}
	else
	{
		unisim::kernel::Object *owner = GetOwner(name);
		
		std::size_t value_index = bool_values.size();
		bool_values.resize(value_index + 1);
		
		bool_values[value_index] = new bool();

		bool_variable = new unisim::kernel::variable::Variable<bool>(variable_name.c_str(), owner, *bool_values[value_index], unisim::kernel::VariableBase::VAR_PARAMETER, "scml property (bool)");
	
		bool_variables[name] = bool_variable;
	}
	
	bool property_value = (bool)(*bool_variable);
	
	return property_value;
}

std::string ScmlPropertyServer::getStringProperty(const std::string & name)
{
	std::string variable_name;
	
	if(!GetLeafName(name, variable_name)) return std::string();

	unisim::kernel::variable::Variable<std::string> *string_variable = 0;
	
	std::map<std::string, unisim::kernel::variable::Variable<std::string> *>::const_iterator it = string_variables.find(name);
	
	if(it != string_variables.end())
	{
		string_variable = (*it).second;
	}
	else
	{
		unisim::kernel::Object *owner = GetOwner(name);
		
		std::size_t value_index = string_values.size();
		string_values.resize(value_index + 1);
		
		string_values[value_index] = new std::string();
		
		string_variable = new unisim::kernel::variable::Variable<std::string>(variable_name.c_str(), owner, *string_values[value_index], unisim::kernel::VariableBase::VAR_PARAMETER, "scml property (string)");
		string_variables[name] = string_variable;
	}
	
	std::string property_value = (std::string)(*string_variable);
	
	return property_value;
}

double ScmlPropertyServer::getDoubleProperty(const std::string & name)
{
	std::string variable_name;
	
	if(!GetLeafName(name, variable_name)) return 0.0;

	unisim::kernel::variable::Variable<double> *double_variable = 0;
	
	std::map<std::string, unisim::kernel::variable::Variable<double> *>::const_iterator it = double_variables.find(name);
	
	if(it != double_variables.end())
	{
		double_variable = (*it).second;
	}
	else
	{
		unisim::kernel::Object *owner = GetOwner(name);
		
		std::size_t value_index = double_values.size();
		double_values.resize(value_index + 1);
		
		double_values[value_index] = new double();

		double_variable = new unisim::kernel::variable::Variable<double>(variable_name.c_str(), owner, *double_values[value_index], unisim::kernel::VariableBase::VAR_PARAMETER, "scml property (double)");
		double_variables[name] = double_variable;
	}
	
	return (double)(*double_variable);
}

Simulator::Simulator(sc_core::sc_module_name const& name, int argc, char **argv, void (*LoadBuiltInConfig)(unisim::kernel::Simulator *simulator))
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, scml_debug(false)
	, param_scml_debug("scml-debug", 0, scml_debug, "Enable/Disable debug of SCML modules")
	, scml_note(false)
	, param_scml_note("scml-note", 0, scml_note, "Enable/Disable note of SCML modules")
	, custom_property_server(0)
	, unisim_debug_info_logger(0)
	, unisim_debug_warning_logger(0)
	, unisim_debug_error_logger(0)
{
	custom_property_server = new ScmlPropertyServer(this);

	scml_property_registry& property_registry = scml_property_registry::inst();
	property_registry.setCustomPropertyServer(custom_property_server);

	::scml2::logging::registry& registry = ::scml2::logging::registry::get_instance();
	::scml2::logging::logger_base *default_cerr_logger = registry.find_logger_by_name("default_cerr_logger");
	::scml2::logging::logger_base *unisim_debug_info_logger = new ::scml2::logging::stream_logger("unisim_debug_info", logger.DebugInfoStream());
	::scml2::logging::logger_base *unisim_debug_warning_logger = new ::scml2::logging::stream_logger("unisim_debug_warning", logger.DebugWarningStream());
	::scml2::logging::logger_base *unisim_debug_error_logger = new ::scml2::logging::stream_logger("unisim_debug_error", logger.DebugErrorStream());

	registry.register_logger(unisim_debug_info_logger);
	registry.register_logger(unisim_debug_warning_logger);
	registry.register_logger(unisim_debug_error_logger);
	
	if(default_cerr_logger)
	{
		default_cerr_logger->disable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::note().get_level()));
		default_cerr_logger->disable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::debug().get_level()));
		default_cerr_logger->disable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::warning().get_level()));
		default_cerr_logger->disable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::error().get_level()));
	}
	
	unisim_debug_error_logger->enable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::error().get_level()));
	unisim_debug_warning_logger->enable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::warning().get_level()));

	if(scml_debug)
	{
		unisim_debug_info_logger->enable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::debug().get_level()));
	}
	else
	{
		unisim_debug_info_logger->disable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::debug().get_level()));
	}
		
	if(scml_note)
	{
		unisim_debug_info_logger->enable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::note().get_level()));
	}
	else
	{
		unisim_debug_info_logger->disable(::scml2::logging::match::severity_threshold_match(::scml2::logging::severity::note().get_level()));
	}
}

Simulator::~Simulator()
{
	delete custom_property_server;
	
	::scml2::logging::registry& registry = ::scml2::logging::registry::get_instance();
	registry.unregister_logger(unisim_debug_info_logger);
	registry.unregister_logger(unisim_debug_warning_logger);
	registry.unregister_logger(unisim_debug_error_logger);
	delete unisim_debug_info_logger;
	delete unisim_debug_warning_logger;
	delete unisim_debug_error_logger;
}

} // end of namespace scml2
} // end of namespace kernel
} // end of namespace unisim
