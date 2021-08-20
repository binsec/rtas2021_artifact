/*
 *  Copyright (c) 2014,
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

#include "core/object.h"
#include "core/kernel.h"
#include <string.h>

namespace sc_core {

const char* sc_object::name() const
{
	return object_name.c_str();
}

const char* sc_object::basename() const
{
	size_t pos = object_name.find_last_of(".", 0);
	return (pos != std::string::npos) ? object_name.c_str() + pos + 1 : object_name.c_str();
}

const char* sc_object::kind() const
{
	return "sc_object";
}

void sc_object::print(std::ostream& os) const
{
	os << object_name;
}

void sc_object::dump( std::ostream& os) const
{
	os << kind() << "::\"" << object_name << "\"";
}

const std::vector<sc_object*>& sc_object::get_child_objects() const
{
	return child_objects;
}

const std::vector<sc_event*>& sc_object::get_child_events() const
{
	return child_events;
}

sc_object* sc_object::get_parent_object() const
{
	return parent_object;
}

bool sc_object::add_attribute( sc_attr_base& _attr)
{
	return attributes.insert(&_attr);
}

sc_attr_base* sc_object::get_attribute( const std::string& _attr_name)
{
	sc_attr_cltn::iterator it = attributes.find(_attr_name);
	return it ? *it : 0;
}

const sc_attr_base* sc_object::get_attribute( const std::string& _attr_name) const
{
	sc_attr_cltn::const_iterator it = attributes.find(_attr_name);
	return it ? *it : 0;
}

sc_attr_base* sc_object::remove_attribute( const std::string& _attr_name)
{
	sc_attr_cltn::iterator it = attributes.find(_attr_name);
	
	sc_attr_base *attr = 0;
	if(it)
	{
		attr = *it;
		attributes.erase(it);
	}
	
	return attr;
}

void sc_object::remove_all_attributes()
{
	attributes.clear();
}

int sc_object::num_attributes() const
{
	return attributes.size();
}

sc_attr_cltn& sc_object::attr_cltn()
{
	return attributes;
}

const sc_attr_cltn& sc_object::attr_cltn() const
{
	return attributes;
}

std::string sc_object::create_hierarchical_name(const char *_name) const
{
	std::string hierarchical_name;
	
	sc_kernel *kernel = sc_kernel::get_kernel();
	sc_object *_parent_object = kernel->get_current_object();
	if(_parent_object)
	{
		hierarchical_name += _parent_object->name();
		hierarchical_name += '.';
	}

	hierarchical_name += _name;

	if(kernel->find_object(hierarchical_name.c_str()))
	{
		// hierarchical name already exists
		std::string new_hierarchical_name = sc_kernel::get_kernel()->gen_unique_name(hierarchical_name.c_str());
		if(strncmp(_name, __LIBIEEE1666_KERNEL_PREFIX__, strlen(__LIBIEEE1666_KERNEL_PREFIX__)) != 0)
		{
			std::cerr << "WARNING! object \"" << hierarchical_name << "\" has been renamed \"" << new_hierarchical_name << "\"" << std::endl;
		}
		hierarchical_name = new_hierarchical_name;
	}
	
	return hierarchical_name;
}

sc_object::sc_object()
	: kernel(sc_kernel::get_kernel())
	, object_name(create_hierarchical_name("object"))
	, child_objects()
	, child_events()
	, attributes()
	, parent_object(0)
{
	init();
}

sc_object::sc_object(const char *_name)
	: kernel(sc_kernel::get_kernel())
	, object_name((_name && *_name) ? create_hierarchical_name(_name) : create_hierarchical_name("object"))
	, child_objects()
	, child_events()
	, attributes()
	, parent_object(0)
{
	init();
}

sc_object::sc_object(const sc_object& object)
	: kernel(sc_kernel::get_kernel())
	, object_name(create_hierarchical_name(object.basename()))
	, child_objects()
	, child_events()
	, attributes()
	, parent_object(0)
{
	init();
}

void sc_object::init()
{
	parent_object = kernel->get_current_object();

	if(parent_object)
		parent_object->add_child_object(this);

	kernel->register_object(this);
}

sc_object& sc_object::operator = (const sc_object& object)
{
	return *this;
}

sc_object::~sc_object()
{
	unsigned int i;

	unsigned int num_child_events = child_events.size();
	for(i = 0; i < num_child_events; i++)
	{
		sc_event *child_event = child_events[i];
		child_event->parent_object = 0;
	}

	unsigned int num_child_objects = child_objects.size();
	for(i = 0; i < num_child_objects; i++)
	{
		sc_object *child_object = child_objects[i];
		child_object->parent_object = 0;
	}
	
	if(parent_object) parent_object->remove_child_object(this);
	if(kernel) kernel->unregister_object(this);
}

void sc_object::add_child_object(sc_object *object)
{
	child_objects.push_back(object);
}

void sc_object::add_child_event(sc_event *event)
{
	child_events.push_back(event);
}

void sc_object::remove_child_object(sc_object *object)
{
	unsigned int num_child_objects = child_objects.size();
	unsigned int i;
	for(i = 0; i < num_child_objects; i++)
	{
		sc_object *child_object = child_objects[i];
		
		if(child_object == object)
		{
			child_objects[i] = child_objects[num_child_objects - 1];
			child_objects.pop_back();
			break;
		}
	}
}

void sc_object::remove_child_event(sc_event *event)
{
	unsigned int num_child_events = child_events.size();
	unsigned int i;
	for(i = 0; i < num_child_events; i++)
	{
		if(child_events[i] == event)
		{
			child_events[i] = child_events[num_child_events - 1];
			child_events.pop_back();
			break;
		}
	}
}

sc_object *sc_object::find_child_object(const char *name) const
{
	unsigned int n_child_objects = child_objects.size();
	unsigned int i;
	for(i = 0; i < n_child_objects; i++)
	{
		sc_object *child_object = child_objects[i];
		if(strcmp(child_object->name(), name) == 0) return child_object;
	}
	return 0;
}

void sc_object::dump_hierarchy(std::ostream& os, unsigned int indent) const
{
	unsigned int i;
	for(i = 0; i < indent; i++) os << "\t";
	indent++;
	os << "- object " << name() << std::endl;
	unsigned int num_child_events = child_events.size();
	for(i = 0; i < num_child_events; i++)
	{
		sc_event *child_event = child_events[i];
		for(unsigned int j = 0; j < indent; j++) os << "\t";
		os << "- event " << child_event->name() << std::endl;
	}
	unsigned int n_child_objects = child_objects.size();
	for(i = 0; i < n_child_objects; i++)
	{
		sc_object *child_object = child_objects[i];
		child_object->dump_hierarchy(os, indent);
	}
}

} // end of namespace sc_core
