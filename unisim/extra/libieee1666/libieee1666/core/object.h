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

#ifndef __LIBIEEE1666_CORE_OBJECT_H__
#define __LIBIEEE1666_CORE_OBJECT_H__

#include "core/fwd.h"
#include "core/event.h"
#include "core/attribute.h"
#include <iostream>
#include <string>
#include <vector>

namespace sc_core {

class sc_object
{
public:
	const char *name() const;
	const char *basename() const;
	virtual const char *kind() const;
	
	virtual void print(std::ostream& = std::cout) const;
	virtual void dump(std::ostream& = std::cout) const;
	virtual const std::vector<sc_object *>& get_child_objects() const;
	virtual const std::vector<sc_event *>& get_child_events() const;
	sc_object* get_parent_object() const;
	bool add_attribute(sc_attr_base&);
	sc_attr_base* get_attribute(const std::string&);
	const sc_attr_base* get_attribute(const std::string&) const;
	sc_attr_base* remove_attribute(const std::string&);
	void remove_all_attributes();
	int num_attributes() const;
	sc_attr_cltn& attr_cltn();
	const sc_attr_cltn& attr_cltn() const;
protected:
	sc_object();
	sc_object(const char*);
	sc_object( const sc_object& );
	sc_object& operator= ( const sc_object& );
	virtual ~sc_object();
	
	///////////////////// EVERYTHING BELOW IS NOT PART OF IEEE1666 STANDARD /////////////////////////
	sc_kernel *kernel;
private:
	friend class sc_kernel;
	friend class sc_event;
	
	std::string object_name;
	std::vector<sc_object *> child_objects;
	std::vector<sc_event *> child_events;
	sc_attr_cltn attributes;
	sc_object *parent_object;
	
	void add_child_object(sc_object *object);
	void add_child_event(sc_event *event);
	void remove_child_object(sc_object *object);
	void remove_child_event(sc_event *event);
	sc_object *find_child_object(const char *name) const;
	std::string create_hierarchical_name(const char *_name) const;
	void init();
public:
	// debug stuff
	void dump_hierarchy(std::ostream& os, unsigned int indent = 0) const;
};

} // end of namespace sc_core

#endif
