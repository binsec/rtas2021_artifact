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
 
#ifndef SIMULATOR_CXX_TESLA_FATFORMAT_HH
#define SIMULATOR_CXX_TESLA_FATFORMAT_HH

#include <iosfwd>

// From __cudaFatFormat.h in the CUDA toolkit

struct cudaFatCubinEntry
{
	char * gpuProfileName;
	char * cubin;
};

struct cudaFatPtxEntry
{
	char * gpuProfileName;            
	char * ptx;
};

struct cudaFatDebugEntry
{
	char * gpuProfileName;            
	char * debug;
};

struct cudaFatElfEntry {
    char * gpuProfileName;            
    char * elf;
    cudaFatElfEntry * next;
    unsigned int size;
};

enum cudaFatCudaBinaryFlag
{
	cudaFatDontSearchFlag = (1 << 0),
	cudaFatDontCacheFlag  = (1 << 1),
	cudaFatSassDebugFlag  = (1 << 2)
};

struct cudaFatSymbol {
    char* name;
};

struct cudaFatCudaBinary
{
	unsigned long magic;
	unsigned long version;
	unsigned long gpuInfoVersion;
	char * key;
	char * ident;
	char * usageMode;
	cudaFatPtxEntry * ptx;
	cudaFatCubinEntry * cubin;
	cudaFatDebugEntry * debug;
    void * debugInfo;
    unsigned int flags;
    cudaFatSymbol * exported;
    cudaFatSymbol * imported;
    cudaFatCudaBinary * dependends;
    unsigned int characteristic;
    cudaFatElfEntry * elf;
};


struct FatFormat
{
	FatFormat(void const *fatCubin);
	
	char const * GetCubin(char const * gpuname);
	void Dump();

	static bool verbose;

private:
	cudaFatCudaBinary const * fcb;
};

#endif
