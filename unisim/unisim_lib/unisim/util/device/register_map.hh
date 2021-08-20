/*
 *  Copyright (c) 2007,
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
 
#ifndef __FS_UTILS_DEVICE_REGISTER_MAP_HH__
#define __FS_UTILS_DEVICE_REGISTER_MAP_HH__

#include <utils/hash/hash_table.hh>

namespace full_system {
namespace utils {
namespace device {

using full_system::utils::hash::HashTable;

#if 0
template <class ADDRESS>
class Register
{
public:
	ADDRESS GetBaseAddress() const;
	unsigned int GetByteSize() const;
};
#endif

template <class ADDRESS, class REGISTER>
class RegisterMap
{
public:
	RegisterMap()
	{
	}
	
	virtual ~RegisterMap()
	{
		hash_table.Reset();
	}
	
	REGISTER *GetRegister(ADDRESS addr)
	{
		Redirector *redir = hash_table.Find(addr);
		return redir ? redir->reg : 0;
	}
	
	bool Map(REGISTER& reg)
	{
		ADDRESS addr;
		ADDRESS start_addr = reg.GetBaseAddress();
		ADDRESS end_addr = start_addr + reg.GetByteSize() - 1;
		
		for(addr = start_addr; addr <= end_addr; addr++)
		{
			Redirector *redir = hash_table.Find(addr);
			
			if(redir) return false;
			redir = new Redirector(addr, reg);
			hash_table.Insert(redir);
		}
		return true;
	}
	
	void Unmap(ADDRESS start_addr, ADDRESS end_addr)
	{
		ADDRESS addr;
		
		for(addr = start_addr; addr <= end_addr; addr++)
		{
			Redirector *redir = hash_table.Find(addr);
			
			if(redir)
			{
				hash_table.Remove(redir);
				delete redir;
			}
		}
	}
	
	void Unmap(REGISTER& reg)	
	{
		ADDRESS addr;
		ADDRESS start_addr = reg.GetBaseAddress();
		ADDRESS end_addr = start_addr + reg.GetByteSize() - 1;
		
		for(addr = start_addr; addr <= end_addr; addr++)
		{
			Redirector *redir = hash_table.Find(addr);
			
			if(redir)
			{
				hash_table.Remove(redir);
				delete redir;
			}
		}
	}
	
private:
	class Redirector
	{
	public:
		Redirector(ADDRESS _addr, REGISTER& _reg) :
			key(_addr),
			next(0),
			reg(&_reg)
		{
		}
	private:
		friend class HashTable<ADDRESS, Redirector>;
		friend class RegisterMap<ADDRESS, REGISTER>;
		ADDRESS key;
		Redirector *next;
		REGISTER *reg;
	};
	
	HashTable<ADDRESS, Redirector> hash_table;
};

} // end of namespace device
} // end of namespace utils
} // end of namespace full_system

#endif
