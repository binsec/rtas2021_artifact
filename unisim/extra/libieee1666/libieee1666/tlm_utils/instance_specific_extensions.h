/*
 *  Copyright (c) 2016,
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

#ifndef __TLM_UTILS_INSTANCE_SPECIFIC_EXTENSIONS_H__
#define __TLM_UTILS_INSTANCE_SPECIFIC_EXTENSIONS_H__

#include <tlm>

namespace tlm_utils {

class embedded_instance_specific_extension : public tlm::tlm_extension<embedded_instance_specific_extension>
{
public:
	virtual tlm_extension_base* clone() const;
	virtual void copy_from(tlm_extension_base const &);
private:
	friend class instance_specific_extension_accessor;
	
	embedded_instance_specific_extension(tlm::tlm_generic_payload *trans);
	
	proxy<instance_specific_extension_set>& get_proxy(unsigned int accessor_id);
	
	instance_specific_extension_set *instance_specific_extensions;
};

class instance_specific_extension_base
{
public:
	virtual ~instance_specific_extension_base() {}
private:
	static unsigned int allocate_instance_specific_extension_id();
public:
};

template <typename T>
class instance_specific_extension : public instance_specific_extension_base // derived class is implementation-defined
{
public:
	virtual ~instance_specific_extension() {}
	
/////////////////////
	const static unsigned int ID;
};

template <typename T>
const static unsigned int instance_specific_extension<T>::ID = instance_specific_extension_base::allocate_instance_specific_extension_id();

template <typename U>
class proxy
{
public:
	proxy(U *u);
	
	template <typename T> T *set_extension(T *extension);
	template <typename T> void get_extension(T *& extension) const;
	template <typename T> void clear_extension(const T *extension);
	void resize_extensions();
private:
	U *u;
	std::vector<instance_specific_extension_base *> instance_specific_extensions;
};

template <typename U>
proxy<U>::proxy(U *_u)
	: u(_u)
	, instance_specific_extensions()
{
}

template <typename U>
template <typename T> T *proxy<U>::set_extension(T *extension)
{
	instance_specific_extension_base *old_extension = (id < instance_specific_extensions.size()) ? instance_specific_extensions[T::ID] : 0;
	instance_specific_extensions[id] = extension;
	return static_cast<T *>(old_extension);
}

template <typename U>
template <typename T> void proxy<U>::get_extension(T *& extension) const
{
	extension = static_cast<T *>((id < instance_specific_extensions.size()) ? instance_specific_extensions[T::ID] : 0);
}

template <typename U>
template <typename T> void proxy<U>::clear_extension(const T *extension)
{
	instance_specific_extensions[T::ID] = static_cast<T *>(0);
}

template <typename U>
void proxy<U>::resize_extensions()
{
}

class proxy_set
{
public:
	instance_specific_extension_set();
private:
	std::vector<proxy *> proxies;
	tlm::tlm_generic_payload *trans;
	
	static instance_specific_extension_set *allocate();
	static void reuse(instance_specific_extension_set *i);
	static std::stack<instance_specific_extension_set *> free_pool;
	static std::vector<instance_specific_extension_set *> pool;
};

class instance_specific_extension_accessor
{
public:
	instance_specific_extension_accessor();
	
	template <typename T> proxy& operator () (T&);
	////////////////////////////////////////
private:
	unsigned int id;
	
	static unsigned int num_accessors;
	
	static unsigned int allocate_instance_specific_extension_accessor_id();
};

template <typename T> proxy<instance_specific_extension_set>& instance_specific_extension_accessor::operator () (T& trans)
{
	embedded_instance_specific_extension *embedded_extension;
	trans.get_extension(embedded_extension);
	if(!embedded_extension)
	{
		embedded_extension = new embedded_instance_specific_extension(&trans);
	}
	return embedded_extension->get_proxy(id);
}

} // end of namespace tlm_utils

#endif // __TLM_UTILS_INSTANCE_SPECIFIC_EXTENSIONS_H__
