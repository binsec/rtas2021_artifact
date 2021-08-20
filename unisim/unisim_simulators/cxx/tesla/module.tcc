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
 
#ifndef SIMULATOR_CXX_TESLA_MODULE_TCC
#define SIMULATOR_CXX_TESLA_MODULE_TCC

#include <module.hh>

#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <cassert>
#include <iostream>

#include "exception.hh"
#include "cuda.h"

#include "kernel.tcc"
#include "fatformat.hh"

using namespace std;

inline string Strip(string s)
{
	typedef string::iterator it_t;
	size_t start = s.find_first_not_of(" \t");
	size_t end = s.find_last_not_of(" \t");
	if(start == string::npos || end == string::npos)
		return string();
	else
		return s.substr(start, end - start + 1);
}

inline void ParseBinCode(vector<uint32_t> & v, istream & is)
{
	while(true)
	{
		if(!is) {
			throw ParsingException("Unexpected EOF");
		}
		string line;
		getline(is, line);
		if(Strip(line) != "}")
		{
			istringstream iss(line);
			iss.setf(ios::hex, ios::basefield);
			iss.setf(ios::showbase);
			uint32_t val;
			iss >> val;
			while(iss)
			{
				//std::cerr << std::hex << val << " ";
				v.push_back(val);
				iss >> val;
			}
			//std::cerr << std::endl;
		}
		else
		{
			break;
		}
	}
}

inline ParsingException::ParsingException() :
	str(0)
{
	assert(false);
}

inline ParsingException::ParsingException(char const * str) :
	str(str)
{
	std::cerr << str << std::endl;
	assert(false);
}

inline ParsingException::ParsingException(char const * str, std::string const & param) :
	str(str), param(param)
{
	std::cerr << str << param<< std::endl;
	assert(false);
}

inline std::ostream & operator<< (std::ostream & os, ParsingException const & pe)
{
	os << pe.str;
	os << pe.param;
	return os;
}

template<class CONFIG>
MemSegment<CONFIG>::MemSegment(std::istream & is, SegmentType st) :
	type(st),
	reloc_address(0)
{
	if(st == SegmentConst) {
		cerr << " Constant segment\n";
	}
	else {
		cerr << " Reloc segment\n";
	}
	
	typedef string::iterator it_t;
	while(true)
	{
		if(!is) {
			throw ParsingException("Unexpected EOF");
		}
		string line;
		getline(is, line);
		if(Strip(line) != "")
		{
			it_t closebrace = find(line.begin(), line.end(), '}');
			it_t openbrace = find(line.begin(), line.end(), '{');
			it_t equalpos = find(line.begin(), line.end(), '=');
			if(openbrace != line.end())
			{
				string cmd = Strip(string(line.begin(), openbrace));
				if(closebrace != line.end())
				{
					throw ParsingException("Unsupported in-line field");
				}
				else
				{
					if(cmd == "mem")
					{
						cerr << "  Mem {...}\n";
						ParseBinCode(mem, is);
					}
					else
					{
						throw ParsingException("Unknown field : ", cmd);
					}
				}
			}
			else if(equalpos != line.end())
			{
				string valname = Strip(string(line.begin(), equalpos));
				string valvalue = Strip(string(++equalpos, line.end()));
				SetAttribute(valname, valvalue);
			}
			else if(closebrace != line.end())
			{
				break;
			}
			// Else, just ignore
		}
	}
}

template<class CONFIG>
MemSegment<CONFIG>::MemSegment()
{
}

template<class CONFIG>
uint32_t MemSegment<CONFIG>::Size() const
{
	assert(mem.size() == 0 || 4 * bytes == mem.size());
	return bytes;
}

template<class CONFIG>
void MemSegment<CONFIG>::SetAttribute(std::string const & attrname, std::string const & value)
{
	cerr << "  " << attrname << " = " << value << endl;
	if(attrname == "name")
	{
		name = value;
	}
	else if(attrname == "segname")
	{
		segname = value;
	}
	else if(attrname == "segnum")
	{
		segnum = atoi(value.c_str());
	}
	else if(attrname == "offset")
	{
		offset = atoi(value.c_str());
	}
	else if(attrname == "bytes")
	{
		bytes = atoi(value.c_str());
	}
	else
	{
		throw ParsingException("Unknown attribute : ", attrname);
	}
}

template<class CONFIG>
void MemSegment<CONFIG>::Load(Service<Memory<typename CONFIG::address_t> > & memory,
	Allocator<CONFIG> & allocator)
{
	
	// Reloc:
	//  - Malloc in device memory (global segment 14)
	//  - Copy initial values from host to device, if applicable
	//  - Write the address to constant segment 14 at given offset
	if(type == SegmentReloc) {
		if(reloc_address == 0) {
			reloc_address = allocator.Alloc(bytes);
		}
	}
	
	cerr << "Loading " << bytes << "B "
		<< (type == SegmentReloc ? "reloc" : "const")
		<< " @" << std::hex << Address() << std::endl;
	
	if(!mem.empty()) {
		if(!memory.WriteMemory(Address(), &mem[0], mem.size() * 4)) {
			throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);	// Generic memory error
		}
	}
	
	if(type == SegmentReloc) {
		typename CONFIG::address_t const_addr = CONFIG::CONST_START
			+ CONFIG::CONST_SEG_SIZE * 14
			+ offset;

		// 32-bit pointer
		// Assumes little-endianness or 32-bit address...			
		if(!memory.WriteMemory(const_addr, &reloc_address, 4)) {
			throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);	// Generic memory error
		}
	}
}

template<class CONFIG>
void MemSegment<CONFIG>::Unload(Service<Memory<typename CONFIG::address_t> > & memory,
	Allocator<CONFIG> & allocator)
{
	if(reloc_address != 0) {
		allocator.Free(reloc_address);
		reloc_address = 0;
	}
}

template<class CONFIG>
std::string const & MemSegment<CONFIG>::Name() const
{
	return name;
}

template<class CONFIG>
typename CONFIG::address_t MemSegment<CONFIG>::Address() const
{
	if(type == SegmentConst) {
		return CONFIG::CONST_START
			+ segnum * CONFIG::CONST_SEG_SIZE
			+ offset;
	}
	else {
		assert(segnum == 14);
		//return CONFIG::GLOBAL_RELOC_START
		//	+ offset * 1024;			// No, I don't think so
										// need to malloc
		assert(reloc_address != 0);
		return reloc_address;
	}
}

template<class CONFIG>
SegmentType MemSegment<CONFIG>::Type() const
{
	return type;
}

//////////////////////////////////////////////////////////////////////

template<class CONFIG>
Sampler<CONFIG>::Sampler(std::istream & is)
{
	typedef string::iterator it_t;
	while(true)
	{
		if(!is) {
			throw ParsingException("Unexpected EOF");
		}
		string line;
		getline(is, line);
		if(Strip(line) != "")
		{
			it_t closebrace = find(line.begin(), line.end(), '}');
			it_t openbrace = find(line.begin(), line.end(), '{');
			it_t equalpos = find(line.begin(), line.end(), '=');
			if(openbrace != line.end())
			{
				string cmd = Strip(string(line.begin(), openbrace));
				if(closebrace != line.end())
				{
					throw ParsingException("Unsupported in-line field");
				}
				else
				{
					throw ParsingException("Unknown field : ", cmd);
				}
			}
			else if(equalpos != line.end())
			{
				string valname = Strip(string(line.begin(), equalpos));
				string valvalue = Strip(string(++equalpos, line.end()));
				SetAttribute(valname, valvalue);
			}
			else if(closebrace != line.end())
			{
				break;
			}
			// Else, just ignore
		}
	}
}

template<class CONFIG>
void Sampler<CONFIG>::SetAttribute(std::string const & attrname, std::string const & value)
{
	cerr << "  " << attrname << " = " << value << endl;
	if(attrname == "name")
	{
		name = value;
	}
	else if(attrname == "texunit")
	{
		this->texunit = atoi(value.c_str());
	}
	else
	{
		throw ParsingException("Unknown attribute : ", attrname);
	}
}

template<class CONFIG>
Sampler<CONFIG>::Sampler()
{
}

template<class CONFIG>
std::string const & Sampler<CONFIG>::Name() const
{
	return name;
}

#if 0
template<class CONFIG>
int Sampler<CONFIG>::TexUnit() const
{
	return texunit;
}

template<class CONFIG>
unsigned int Sampler<CONFIG>::SetAddress(typename CONFIG::address_t addr, unsigned int bytes)
{
	address = addr;
	this->bytes = bytes;
}

template<class CONFIG>
void Sampler<CONFIG>::SetFormat(CUarray_format fmt, int NumPackedComponents)
{
	format = fmt;
	num_packed_components = NumPackedComponents;
}
#endif

template<class CONFIG>
void Sampler<CONFIG>::SetAddressMode(int dim, AddressMode am)
{
	if(dim < 0 || dim > 2) {
		throw CudaException(CUDA_ERROR_INVALID_VALUE);
	}
	Base::SetAddressMode(dim, am);
}


#if 0
template<class CONFIG>
void Sampler<CONFIG>::Load(CPU<CONFIG> & cpu)
{
	unisim::component::cxx::processor::tesla::Sampler<CONFIG> & smp = cpu.GetSampler(texunit);
	smp.baseAddress = address;
	smp.ndims = 1;	// ? TODO
	smp.numPackedComponents = num_packed_components;
	for(int i = 0; i != 3; ++i) {
		smp.size[i] = 0;	// ? TODO
		smp.addressMode[i] = unisim::component::cxx::processor::tesla::AddressMode(address_mode[i]);
	}
	smp.filterMode = unisim::component::cxx::processor::tesla::FilterMode(filter_mode);
	smp.flags = flags;
	smp.format = unisim::component::cxx::processor::tesla::ArrayFormat(format);
}
#endif

//////////////////////////////////////////////////////////////////////


template<class CONFIG>
Module<CONFIG>::Module(char const * fname)
{
	if(verbose)
		cerr << "Cubin " << fname << endl;
	ifstream is(fname, ifstream::in);
	LoadCubin(is);
}

template<class CONFIG>
Module<CONFIG>::Module(void const *fatCubin, int)
{
	FatFormat ff(fatCubin);
	if(verbose)
		ff.Dump();
	char const * cubin = ff.GetCubin(0);
	istringstream is(cubin);
	LoadCubin(is);
}

template<class CONFIG>
Kernel<CONFIG> & Module<CONFIG>::GetKernel(char const * name)
{
	if(verbose)
		cerr << "GetFunction : \"" << name << "\"\n";
	typename KernelMap::iterator it = kernels.find(string(name));
	if(it == kernels.end()) {
		throw CudaException(CUDA_ERROR_NOT_FOUND);
	}
	return it->second;
}

template<class CONFIG>
MemSegment<CONFIG> & Module<CONFIG>::GetGlobal(char const * name)
{
	typename SegMap::iterator it = global_segments.find(string(name));
	if(it == global_segments.end()) {
		throw CudaException(CUDA_ERROR_NOT_FOUND);
	}
	return it->second;
}

template<class CONFIG>
Sampler<CONFIG> & Module<CONFIG>::GetSampler(char const * name)
{
	typename SamplerMap::iterator it = samplers.find(string(name));
	if(it == samplers.end()) {
		throw CudaException(CUDA_ERROR_NOT_FOUND);
	}
	return it->second;
}

template<class CONFIG>
bool Module<CONFIG>::verbose = true;

template<class CONFIG>
void Module<CONFIG>::Load(Service<unisim::service::interfaces::Memory<typename CONFIG::address_t> > & mem,
	Allocator<CONFIG> & allocator)
{
	// Pointers to relocable segments are stored in constant segment 14
	typedef typename SegMap::iterator it_t;
	for(it_t it = global_segments.begin(); it != global_segments.end(); ++it)
	{
		it->second.Load(mem, allocator);
	}
}

template<class CONFIG>
void Module<CONFIG>::LoadCubin(istream & is)
{
	// Mostly adapted from Decuda
	typedef string::iterator it_t;
	while(is)
	{
		string line;
		getline(is, line);
		if(Strip(line) != "")
		{
			it_t closebrace = find(line.begin(), line.end(), '}');
			it_t openbrace = find(line.begin(), line.end(), '{');
			it_t equalpos = find(line.begin(), line.end(), '=');
			if(openbrace != line.end())
			{
				string cmd = Strip(string(line.begin(), openbrace));
				if(closebrace != line.end())
				{
					string value = string(++openbrace, closebrace);
					SetField(cmd, value);
				}
				else
				{
					if(cmd == "code")
					{
						Kernel<CONFIG> kernel(this, is);
						if(kernel.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						kernels[kernel.Name()] = kernel;
					}
					else if(cmd == "consts")
					{
						//global_constants.push_back(MemSegment<CONFIG>(is));
						MemSegment<CONFIG> cs(is, SegmentConst);
						if(cs.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						global_segments[cs.Name()] = cs;
					}
					else if(cmd == "sampler")
					{
						Sampler<CONFIG> s(is);
						if(s.Name().empty()) {
							throw ParsingException("Unnamed sampler field");
						}
						samplers[s.Name()] = s;
					}
					else if(cmd == "reloc")
					{
						MemSegment<CONFIG> cs(is, SegmentReloc);
						if(cs.Name().empty()) {
							throw ParsingException("Unnamed code field");
						}
						global_segments[cs.Name()] = cs;
					}
					else
					{
						throw ParsingException("Unknown multi-line field : ", cmd);
					}
				}
			}
		}
	}
}

template<class CONFIG>
void Module<CONFIG>::SetField(std::string const & attrname, std::string const & value)
{
	cerr << " " << attrname << " = " << value << endl;
	if(attrname == "architecture")
	{
		architecture = value;
	}
	else if(attrname == "abiversion")
	{
		abiversion = value;
	}
	else if(attrname == "modname")
	{
		modname = value;
	}
	else if(attrname == "cubinversion")
	{
		cubinversion = value;
	}
	else if(attrname == "texmode")
	{
		texmode = value;
	}
	else
	{
		throw ParsingException("Unknown in-line field : ", attrname);
	}
}

#endif
