/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: 
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */
 
#ifndef SIMULATOR_CXX_TESLA_MODULE_HH
#define SIMULATOR_CXX_TESLA_MODULE_HH

#include <string>
#include <iosfwd>
#include <vector>
#include <list>
#include <map>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/component/cxx/processor/tesla/interfaces.hh>

#include "kernel.hh"

using unisim::kernel::Service;
using unisim::service::interfaces::Memory;
using unisim::component::cxx::processor::tesla::SamplerBase;
using unisim::component::cxx::processor::tesla::ArrayFormat;
using unisim::component::cxx::processor::tesla::AddressMode;
using unisim::component::cxx::processor::tesla::FilterMode;
using unisim::component::cxx::processor::tesla::TextureFlags;

struct ParsingException
{
	ParsingException();
	ParsingException(char const * str);
	ParsingException(char const * str, std::string const & param);

	friend std::ostream & operator<< (std::ostream & os, ParsingException const & pe);
private:
	char const * str;
	std::string param;
};

std::ostream & operator<< (std::ostream & os, ParsingException const & pe);

enum SegmentType
{
	SegmentConst,
	SegmentReloc
};

template<class CONFIG>
struct MemSegment
{
	MemSegment(std::istream & is, SegmentType st);
	MemSegment();
	uint32_t Size() const;
	void Load(Service<Memory<typename CONFIG::address_t> > & mem, Allocator<CONFIG> & allocator);
	void Unload(Service<Memory<typename CONFIG::address_t> > & mem, Allocator<CONFIG> & allocator);
	std::string const & Name() const;
	typename CONFIG::address_t Address() const;
	SegmentType Type() const;
	
private:
	void SetAttribute(std::string const & name, std::string const & value);

	SegmentType type;
	std::string name;
	std::string segname;
	int segnum;
	uint32_t offset;
	uint32_t bytes;
	std::vector<uint32_t> mem;
	typename CONFIG::address_t reloc_address;
};

template<class CONFIG>
struct Sampler : CUtexref_st, SamplerBase<typename CONFIG::address_t>
{
	Sampler(std::istream & is);
	Sampler();
	
	std::string const & Name() const;
//	int TexUnit() const;
//	unsigned int SetAddress(typename CONFIG::address_t addr, unsigned int bytes);

//	void SetFormat(CUarray_format fmt, int NumPackedComponents);
	void SetAddressMode(int dim, AddressMode am);
//	void SetFilterMode(CUfilter_mode fm);
//	void SetFlags(unsigned int Flags);
	//void Load(CPU<CONFIG> & cpu);
	
	typedef SamplerBase<typename CONFIG::address_t> Base;

private:
	void SetAttribute(std::string const & name, std::string const & value);
	std::string name;
#if 0
	int texunit;
	typename CONFIG::address_t address;
	uint32_t bytes;
	CUarray_format format;
	int num_packed_components;
	CUaddress_mode address_mode[3];
	CUfilter_mode filter_mode;
	unsigned int flags;

public:
	virtual int GetTexUnit() const {
		return texunit; }
	virtual address_t GetBaseAddress() const {
		return address; }
	virtual int GetNumPackedComponents() const {
		return num_packed_components; }
	virtual ArrayFormat GetFormat() const {
		return ArrayFormat(format); }
	virtual AddressMode GetAddressMode(unsigned int i) const {
		assert(i < 3); return AddressMode(address_mode[i]); }
	virtual FilterMode GetFilterMode() const {
		return FilterMode(filter_mode); }
	virtual uint32_t GetFlags() const {
		return flags; }
#endif	
};

template<class CONFIG>
struct Module : CUmod_st
{
	Module(char const * fname);
	Module(void const *fatCubin, int);
	
	Kernel<CONFIG> & GetKernel(char const * name);
	MemSegment<CONFIG> & GetGlobal(char const * name);
	::Sampler<CONFIG> & GetSampler(char const * name);
	void Load(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem,
		Allocator<CONFIG> & allocator);

	static bool verbose;

private:
	void LoadCubin(std::istream & is);
	void SetField(std::string const & name, std::string const & value);

	std::string architecture;
	std::string abiversion;
	std::string modname;
	std::string cubinversion;
	std::string texmode;
	
	typedef std::map<std::string, Kernel<CONFIG> > KernelMap;
	KernelMap kernels;

	typedef std::map<std::string, MemSegment<CONFIG> > SegMap;
	SegMap global_segments;

	typedef std::map<std::string, ::Sampler<CONFIG> > SamplerMap;
	SamplerMap samplers;

//	std::list<CUfunction> functions;
	
};

inline std::string Strip(std::string s);
inline void ParseBinCode(std::vector<uint32_t> & v, std::istream & is);


#endif

