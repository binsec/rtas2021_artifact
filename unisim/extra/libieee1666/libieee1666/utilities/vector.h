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

#ifndef __LIBIEEE1666_UTILITIES_VECTOR_H__
#define __LIBIEEE1666_UTILITIES_VECTOR_H__

#include "core/fwd.h"
#include "core/object.h"
#include "utilities/fwd.h"
#include <stdexcept>
#include <vector>

namespace sc_core {

//////////////////////////////// declaration //////////////////////////////////

////////////////////////////// sc_vector_base /////////////////////////////////

class sc_vector_base : public sc_object
{
public:
	typedef std::vector<sc_object *>::size_type size_type; // implementation-defined

	virtual const char *kind() const;
	size_type size() const;
	const std::vector<sc_object *>& get_elements() const;
protected:
	sc_vector_base();
	explicit sc_vector_base(const char *name);
	
	void push_back(sc_object *);
	sc_object *at(size_type);
	const sc_object *at(size_type) const;
	void clear();
private:
	std::vector<sc_object *> elements;
};

///////////////////////////// sc_vector_iter<> ////////////////////////////////

template <typename T>
class sc_vector_iter : public std::iterator<std::random_access_iterator_tag, T>
{
// Conforms to Random Access Iterator category.
// See ISO/IEC 14882:2003(E), 24.1 [lib.iterator.requirements]
// implementation-defined
	typedef std::iterator<std::random_access_iterator_tag, T> base_type;
	
	typedef base_type::reference reference;
	
	sc_vector_iter& operator ++ () { ++obj_iter; return *this; }
	sc_vector_iter& operator ++ (int)  { sc_vector_iter prev(*this); ++obj_iter; return prev; };
	reference operator * () const { *static_cast<T *>(*obj_iter); }
	
private:
	std::vector<sc_object *>::iterator obj_iter;
	
	sc_vector_iter(const sc_vector_iter& other) : obj_iter(other.obj_iter) {}
};

//////////////////////////////// sc_vector<> //////////////////////////////////

template <typename T>
class sc_vector : public sc_vector_base
{
public:
	using sc_vector_base::size_type;
	typedef sc_vector_iter<T> iterator;
	typedef sc_vector_iter<const T> const_iterator;
	
	sc_vector();
	explicit sc_vector(const char *);
	sc_vector(const char *, size_type);
	template <typename Creator> sc_vector(const char *, size_type, Creator);
	virtual ~sc_vector();
	
	void init(size_type);
	static T *create_element(const char *, size_type);
	template <typename Creator> void init(size_type, Creator);
	T& operator [] (size_type);
	const T& operator [] (size_type) const;
	T& at(size_type);
	const T& at(size_type) const;
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	template <typename ContainerType, typename ArgumentType> iterator bind(sc_vector_assembly<ContainerType,ArgumentType>);

	template <typename BindableContainer> iterator bind(BindableContainer&);

	template <typename BindableIterator> iterator bind(BindableIterator, BindableIterator);

	template <typename BindableIterator> iterator bind(BindableIterator, BindableIterator, iterator);

	template <typename ContainerType, typename ArgumentType> iterator operator() (sc_vector_assembly<ContainerType,ArgumentType> c);

	template <typename ArgumentContainer> iterator operator () (ArgumentContainer&);

	template <typename ArgumentIterator> iterator operator () (ArgumentIterator, ArgumentIterator);

	template <typename ArgumentIterator> iterator operator () (ArgumentIterator, ArgumentIterator, iterator);
	
private:
	// Disabled
	sc_vector(const sc_vector&);
	sc_vector<T>& operator = (const sc_vector&);
	
	void clear();
};

template <typename T, typename MT>
class sc_vector_assembly
{
public:
	typedef int size_type; // implementation-defined HACK
	typedef sc_vector_iter<T> iterator;  // implementation-defined HACK
	typedef sc_vector_iter<const T> const_iterator; // implementation-defined; HACK
	typedef MT(T::*member_type);

	sc_vector_assembly(const sc_vector_assembly&);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	size_type size() const;
	std::vector<sc_object *> get_elements() const;
	typename iterator::reference operator [] (size_type);
	typename const_iterator::reference operator [] (size_type) const;
	typename iterator::reference at(size_type);
	typename const_iterator::reference at(size_type) const;

	template <typename ContainerType, typename ArgumentType> iterator bind(sc_vector_assembly<ContainerType, ArgumentType>);

	template <typename BindableContainer> iterator bind(BindableContainer&);

	template <typename BindableIterator> iterator bind(BindableIterator, BindableIterator);

	template <typename BindableIterator> iterator bind(BindableIterator, BindableIterator, iterator);

	template <typename BindableIterator> iterator bind(BindableIterator, BindableIterator, typename sc_vector<T>::iterator);

	template <typename ContainerType, typename ArgumentType> iterator operator () (sc_vector_assembly<ContainerType, ArgumentType>);

	template <typename ArgumentContainer> iterator operator () (ArgumentContainer&);

	template <typename ArgumentIterator> iterator operator () (ArgumentIterator, ArgumentIterator);

	template <typename ArgumentIterator> iterator operator () (ArgumentIterator, ArgumentIterator, iterator);

	template <typename ArgumentIterator> iterator operator () (ArgumentIterator, ArgumentIterator, typename sc_vector<T>::iterator);
	
private:
	// Disabled
	sc_vector_assembly& operator =(const sc_vector_assembly&);
};

template <typename T, typename MT>
sc_vector_assembly<T,MT> sc_assemble_vector(sc_vector<T>& , MT(T::*member_ptr));

///////////////////////////////// definition //////////////////////////////////

//////////////////////////////// sc_vector<> //////////////////////////////////

template <typename T>
sc_vector<T>::sc_vector()
	: sc_vector_base()
{
}

template <typename T>
sc_vector<T>::sc_vector(const char *name)
	: sc_vector_base(name)
{
}

template <typename T>
sc_vector<T>::sc_vector(const char *nm, size_type sz)
	: sc_vector_base(nm)
{
	init(nm, sz, sc_vector::create_element);
}

template <typename T>
template <typename Creator> sc_vector<T>::sc_vector(const char *nm, size_type sz, Creator c)
	: sc_vector_base(nm)
{
	init(nm, sz, c);
}

template <typename T>
sc_vector<T>::~sc_vector()
{
	clear();
}

template <typename T>
void sc_vector<T>::init(size_type sz)
{
	init(sz, &sc_vector<T>::create_element);
}

template <typename T>
T *sc_vector<T>::create_element(const char *nm, size_type idx)
{
	return new T(nm);
}

template <typename T>
template <typename Creator> void sc_vector<T>::init(size_type sz, Creator c)
{
	if(sz && get_elements().size())
	{
		throw std::runtime_error("function sc_vector<T>::init called more than once with an argument value greater than 0");
	}
	
	size_type idx;
	
	for(idx = 0; idx < sz; idx++)
	{
		T *element = c(sc_gen_unique_name(this->base_name()), idx);
		
		sc_vector_base::push_back(element);
	}
}

template <typename T>
T& sc_vector<T>::operator [] (size_type idx)
{
	return at(idx);
}

template <typename T>
const T& sc_vector<T>::operator [] (size_type idx) const
{
	return at(idx);
}

template <typename T>
T& sc_vector<T>::at(size_type idx)
{
	return *static_cast<T *>((sc_object *) sc_vector_base::at(idx));
}

template <typename T>
const T& sc_vector<T>::at(size_type idx) const
{
	return *static_cast<const T *>((const sc_object *) sc_vector_base::at(idx));
}

template <typename T>
typename sc_vector<T>::iterator sc_vector<T>::begin()
{
}

template <typename T>
typename sc_vector<T>::iterator sc_vector<T>::end()
{
}

template <typename T>
typename sc_vector<T>::const_iterator sc_vector<T>::begin() const
{
}

template <typename T>
typename sc_vector<T>::const_iterator sc_vector<T>::end() const
{
}

template <typename T>
typename sc_vector<T>::const_iterator sc_vector<T>::cbegin() const
{
}

template <typename T>
typename sc_vector<T>::const_iterator sc_vector<T>::cend() const
{
}

template <typename T>
template <typename ContainerType, typename ArgumentType> typename sc_vector<T>::iterator sc_vector<T>::bind(sc_vector_assembly<ContainerType,ArgumentType>)
{
}

template <typename T>
template <typename BindableContainer> typename sc_vector<T>::iterator sc_vector<T>::bind(BindableContainer&)
{
}

template <typename T>
template <typename BindableIterator> typename sc_vector<T>::iterator sc_vector<T>::bind(BindableIterator, BindableIterator)
{
}

template <typename T>
template <typename BindableIterator> typename sc_vector<T>::iterator sc_vector<T>::bind(BindableIterator, BindableIterator, iterator)
{
}

template <typename T>
template <typename ContainerType, typename ArgumentType> typename sc_vector<T>::iterator sc_vector<T>::operator() (sc_vector_assembly<ContainerType,ArgumentType> c)
{
}

template <typename T>
template <typename ArgumentContainer> typename sc_vector<T>::iterator sc_vector<T>::operator () (ArgumentContainer&)
{
}

template <typename T>
template <typename ArgumentIterator> typename sc_vector<T>::iterator sc_vector<T>::operator () (ArgumentIterator, ArgumentIterator)
{
}

template <typename T>
template <typename ArgumentIterator> typename sc_vector<T>::iterator sc_vector<T>::operator () (ArgumentIterator, ArgumentIterator, iterator)
{
}

// Disabled
template <typename T>
sc_vector<T>::sc_vector(const sc_vector&)
{
}

template <typename T>
sc_vector<T>& sc_vector<T>::operator = (const sc_vector&)
{
}

template <typename T>
void sc_vector<T>::clear()
{
	size_type sz = size();
	size_type idx;
	
	for(idx = 0; idx < sz; idx++)
	{
		sc_object *obj = at(idx);
		delete obj;
	}
	
	sc_vector_base::clear();
}

/////////////////////////// sc_vector_assembly<> //////////////////////////////

template <typename T, typename MT>
sc_vector_assembly<T, MT>::sc_vector_assembly(const sc_vector_assembly&)
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::begin()
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::end()
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::const_iterator sc_vector_assembly<T, MT>::begin() const
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::const_iterator sc_vector_assembly<T, MT>::end() const
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::const_iterator sc_vector_assembly<T, MT>::cbegin() const
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::const_iterator sc_vector_assembly<T, MT>::cend() const
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::size_type sc_vector_assembly<T, MT>::size() const
{
}

template <typename T, typename MT>
std::vector<sc_object *> sc_vector_assembly<T, MT>::get_elements() const
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::iterator::reference sc_vector_assembly<T, MT>::operator [] (size_type)
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::const_iterator::reference sc_vector_assembly<T, MT>::operator [] (size_type) const
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::iterator::reference sc_vector_assembly<T, MT>::at(size_type)
{
}

template <typename T, typename MT>
typename sc_vector_assembly<T, MT>::const_iterator::reference sc_vector_assembly<T, MT>::at(size_type) const
{
}

template <typename T, typename MT>
template <typename ContainerType, typename ArgumentType> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::bind(sc_vector_assembly<ContainerType, ArgumentType>)
{
}

template <typename T, typename MT>
template <typename BindableContainer> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::bind(BindableContainer&)
{
}

template <typename T, typename MT>
template <typename BindableIterator> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::bind(BindableIterator, BindableIterator)
{
}

template <typename T, typename MT>
template <typename BindableIterator> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::bind(BindableIterator, BindableIterator, iterator)
{
}

template <typename T, typename MT>
template <typename BindableIterator> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::bind(BindableIterator, BindableIterator, typename sc_vector<T>::iterator)
{
}

template <typename T, typename MT>
template <typename ContainerType, typename ArgumentType> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::operator () (sc_vector_assembly<ContainerType, ArgumentType>)
{
}

template <typename T, typename MT>
template <typename ArgumentContainer> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::operator () (ArgumentContainer&)
{
}

template <typename T, typename MT>
template <typename ArgumentIterator> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::operator () (ArgumentIterator, ArgumentIterator)
{
}

template <typename T, typename MT>
template <typename ArgumentIterator> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::operator () (ArgumentIterator, ArgumentIterator, iterator)
{
}

template <typename T, typename MT>
template <typename ArgumentIterator> typename sc_vector_assembly<T, MT>::iterator sc_vector_assembly<T, MT>::operator () (ArgumentIterator, ArgumentIterator, typename sc_vector<T>::iterator)
{
}

// Disabled
template <typename T, typename MT>
sc_vector_assembly<T, MT>& sc_vector_assembly<T, MT>::operator = (const sc_vector_assembly&)
{
}

template <typename T, typename MT>
sc_vector_assembly<T,MT> sc_assemble_vector(sc_vector<T>&, MT(T::*member_ptr))
{
}

} // end of namespace sc_core

#endif
