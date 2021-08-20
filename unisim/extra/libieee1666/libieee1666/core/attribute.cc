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

#include "core/attribute.h"

namespace sc_core {

//////////////////////////////////// sc_attr_base /////////////////////////////////////////////

sc_attr_base::sc_attr_base( const std::string& _attr_name)
	: attr_name(_attr_name)
{
}

sc_attr_base::sc_attr_base( const sc_attr_base& _attr_base)
	: attr_name(_attr_base.attr_name)
{
}

sc_attr_base::~sc_attr_base()
{
}

const std::string& sc_attr_base::name() const
{
	return attr_name;
}

// Disabled
sc_attr_base::sc_attr_base()
	: attr_name()
{
	throw "disabled";
}

// Disabled
sc_attr_base& sc_attr_base::operator= ( const sc_attr_base& )
{
	throw "disabled";
	return *this;
}

//////////////////////////////////// sc_attr_cltn /////////////////////////////////////////////

sc_attr_cltn::iterator sc_attr_cltn::begin()
{
	return storage;
}

sc_attr_cltn::const_iterator sc_attr_cltn::begin() const
{
	return storage;
}

sc_attr_cltn::iterator sc_attr_cltn::end()
{
	return storage + num_elems;
}

sc_attr_cltn::const_iterator sc_attr_cltn::end() const
{
	return storage + num_elems;
}

// Implementation-defined
sc_attr_cltn::sc_attr_cltn()
	: storage(0)
	, max_num_elems(0)
	, num_elems(0)
{
}

// Implementation-defined
sc_attr_cltn::~sc_attr_cltn()
{
	if(storage) delete[] storage;
}

// Implementation-defined
bool sc_attr_cltn::insert(elem_type _elem)
{
	if(find(_elem->name())) return false;
	
	if(num_elems >= max_num_elems)
	{
		if(storage)
		{
			unsigned int new_max_num_elems = 2 * max_num_elems;
			elem_type *new_storage = new elem_type[new_max_num_elems];
			unsigned int i;
			for(i = 0; i < max_num_elems; i++)
			{
				new_storage[i] = storage[i];
			}
			for(i = max_num_elems; i < new_max_num_elems; i++)
			{
				new_storage[i] = 0;
			}
			delete[] storage;
			storage = new_storage;
			max_num_elems = new_max_num_elems;
		}
		else
		{
			max_num_elems = 8;
			storage = new elem_type[max_num_elems];
		}
	}
	storage[num_elems] = _elem;
	num_elems++;
	
	return true;
}

// Implementation-defined
sc_attr_cltn::iterator sc_attr_cltn::find( const std::string& _attr_name)
{
	if(storage && num_elems)
	{
		unsigned int n = num_elems;
		elem_type *ep = storage;
		do
		{
			if((*ep)->name() == _attr_name) return ep;
		}
		while(ep++, --n);
	}
	return 0;
}

// Implementation-defined
sc_attr_cltn::const_iterator sc_attr_cltn::find( const std::string& _attr_name) const
{
	if(storage && num_elems)
	{
		unsigned int n = num_elems;
		elem_type *ep = storage;
		do
		{
			if((*ep)->name() == _attr_name) return ep;
		}
		while(ep++, --n);
	}
	return 0;
}

// Implementation-defined
void sc_attr_cltn::erase( iterator& it )
{
	if(storage && num_elems)
	{
		elem_type *ep = it;
		if((ep >= storage) && (ep < (storage + num_elems)))
		{
			unsigned int n = storage + num_elems - ep;
			n = n - 1;
					
			if(n)
			{
				do
				{
					*ep = *(ep + 1);
				}
				while(ep++, --n);
			}
			
			num_elems--;
			return;
		}
	}
}

// Implementation-defined
void sc_attr_cltn::clear()
{
	num_elems = 0;
}

// Implementation-defined
int sc_attr_cltn::size() const
{
	return num_elems;
}

// Disabled
sc_attr_cltn::sc_attr_cltn( const sc_attr_cltn& )
{
	throw "disabled";
}

// Disabled
sc_attr_cltn& sc_attr_cltn::operator= ( const sc_attr_cltn& )
{
	throw "disabled";
	return *this;
}

} // end of namespace sc_core
