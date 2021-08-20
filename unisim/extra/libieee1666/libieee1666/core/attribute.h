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

#ifndef __LIBIEEE1666_CORE_ATTRIBUTE_H__
#define __LIBIEEE1666_CORE_ATTRIBUTE_H__

#include <string>

namespace sc_core {

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// class definition /////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

class sc_attr_base
{
public:
	sc_attr_base( const std::string& );
	sc_attr_base( const sc_attr_base& );
	virtual ~sc_attr_base();
	const std::string& name() const;
private:
	// Disabled
	sc_attr_base();
	sc_attr_base& operator= ( const sc_attr_base& );
	// Other members
	// Implementation-defined
	std::string attr_name;
};

template <class T>
class sc_attribute : public sc_attr_base
{
public:
	sc_attribute( const std::string& );
	sc_attribute( const std::string&, const T& );
	sc_attribute( const sc_attribute<T>& );
	virtual ~sc_attribute();
	T value;
private:
	// Disabled
	sc_attribute();
	sc_attribute<T>& operator= ( const sc_attribute<T>& );
};

class sc_attr_cltn
{
public:
	typedef sc_attr_base* elem_type;
	typedef elem_type* iterator;
	typedef const elem_type* const_iterator;
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	// Other members
	// Implementation-defined
	sc_attr_cltn();
	~sc_attr_cltn();
	bool insert(elem_type );
	iterator find( const std::string& );
	const_iterator find( const std::string& ) const;
	void clear();
	void erase( iterator& );
	int size() const;
private:
	// Disabled
	sc_attr_cltn( const sc_attr_cltn& );
	sc_attr_cltn& operator= ( const sc_attr_cltn& );
	// Other members
	// Implementation-defined
	
	friend class sc_object;
	
	elem_type *storage;
	unsigned int max_num_elems;
	unsigned int num_elems;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// template implementation ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// sc_attribute<> /////////////////////////////////////////////

template <class T>
sc_attribute<T>::sc_attribute( const std::string& _name)
	: sc_attr_base(_name)
	, value()
{
}

template <class T>
sc_attribute<T>::sc_attribute( const std::string& _name, const T& _value)
	: sc_attr_base(_name)
	, value(_value)
{
}

template <class T>
sc_attribute<T>::sc_attribute( const sc_attribute<T>& _attribute)
	: sc_attr_base(_attribute.name())
	, value(_attribute.value)
{
}

template <class T>
sc_attribute<T>::~sc_attribute()
{
}

// Disabled
template <class T>
sc_attribute<T>::sc_attribute()
	: sc_attr_base(std::string())
	, value()
{
}

// Disabled
template <class T>
sc_attribute<T>& sc_attribute<T>::operator= ( const sc_attribute<T>& )
{
	return *this;
}

} // end of namespace sc_core

#endif
