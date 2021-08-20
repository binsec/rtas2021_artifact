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
 *     David Parello (david.parello@univ-perp.fr)
 *     Sylvain Collange (sylvain.collange@univ-perp.fr)
 *
 */

#include <cuda.h>
#include <driver.hh>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <cerrno>

#include <unisim/component/cxx/processor/tesla/config.hh>
#include "system.hh"
#include "driver.hh"

#include "module.hh"
#include "device.hh"
#include "event.hh"
#include "exception.hh"

#include "driver.tcc"
#include "module.tcc"
#include "device.tcc"
#include "event.tcc"
#include <unisim/component/cxx/processor/tesla/interfaces.hh>

using std::cerr;
using std::endl;
using boost::shared_ptr;
using unisim::component::cxx::processor::tesla::SamplerBase;
using unisim::component::cxx::processor::tesla::ArrayFormat;
using unisim::component::cxx::processor::tesla::AddressMode;
using unisim::component::cxx::processor::tesla::FilterMode;
using unisim::component::cxx::processor::tesla::TextureFlags;


typedef BaseConfig MyConfig;

// Do NOT create static Unisim objects!
// Undefined initialization order would cause undefined behavior.
//Driver<MyConfig> driver;
// What about destruction order??
//shared_ptr<Driver<MyConfig> > driver;
Driver<MyConfig> * driver = 0;	// Dumb pointer
// Memory leak here

System mysystem;

#define CHECK_PTR(p) \
	{ if(!p) { \
		cerr << "Error: null pointer " #p " at " << __FILE__ << ":" << __LINE__ << endl; \
		return CUDA_ERROR_INVALID_VALUE; } }

#define CHECK_DRIVER \
	{ if(!driver) { \
		cerr << "Error: driver not initialized. At " << __FILE__ << ":" << __LINE__ << endl; \
		return CUDA_ERROR_NOT_INITIALIZED; } }


bool const verbose = true;

/*********************************
 ** Initialization
 *********************************/
CUresult  CUDAAPI cuInit(unsigned int Flags)
{
	if(verbose) cerr << "cuInit(" << Flags << ")" << endl;
	//driver = shared_ptr<Driver<MyConfig> >(new Driver<MyConfig>());
	//driver = new Driver<MyConfig>();
	mysystem.Build();
	driver = mysystem.GetDriver();
	return driver->cuInit(Flags);
}


CUresult  CUDAAPI cuDriverGetVersion(int *driverVersion)
{
	if(verbose) cerr << "cuDriverGetVersion()" << endl;
	*driverVersion = CUDA_VERSION;
	return CUDA_SUCCESS;
}


/************************************
 **
 **    Device management
 **
 ***********************************/

CUresult CUDAAPI cuDeviceGet(CUdevice *device, int ordinal)
{
	if(verbose) cerr << "cuDeviceGet(" << device << ", " << ordinal << ")" << endl;
	CHECK_DRIVER;
	return driver->cuDeviceGet(device, ordinal);
}

CUresult CUDAAPI cuDeviceGetCount(int *count)
{
	if(verbose) cerr << "cuDeviceGetCount(" << count << ")" << endl;
	CHECK_DRIVER;
	return driver->cuDeviceGetCount(count);
}

CUresult CUDAAPI cuDeviceGetName(char *name, int len, CUdevice dev)
{
	if(verbose) cerr << "cuDeviceGetName(" << name << ", " << len << ")" << endl;
	CHECK_DRIVER;
	try {
		strncpy(name, driver->Dev(dev).Name().c_str(), len);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuDeviceComputeCapability(int *major, int *minor, CUdevice dev)
{
	if(verbose) cerr << "cuDeviceComputeCapability(..., " << dev << ")" << endl;
	CHECK_DRIVER;
	try {
		*major = driver->Dev(dev).ComputeCapabilityMajor();
		*minor = driver->Dev(dev).ComputeCapabilityMinor();
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuDeviceTotalMem(unsigned int *bytes, CUdevice dev)
{
	if(verbose) cerr << "cuDeviceTotalMem(..., " << dev << ")" << endl;
 	CHECK_DRIVER;
	try {
		*bytes = driver->Dev(dev).TotalMem();
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuDeviceGetProperties(CUdevprop *prop, CUdevice dev)
{
	if(verbose) cerr << "cuDeviceGetProperties(..., " << dev << ")" << endl;
	CHECK_DRIVER;
  	try {
		*prop = driver->Dev(dev).Properties();
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuDeviceGetAttribute(int *pi, CUdevice_attribute attrib, CUdevice dev)
{
	if(verbose) cerr << "cuDeviceGetAttribute(..., " << attrib << ", " << dev << ")" << endl;
	CHECK_DRIVER;
 	try {
		*pi = driver->Dev(dev).Attribute(attrib);
		errno = 0;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

        
    /************************************
     **
     **    Context management
     **
     ***********************************/

CUresult  CUDAAPI cuCtxCreate(CUcontext *pctx, unsigned int flags, CUdevice dev )
{
	if(verbose) cerr << "cuCtxCreate(..., " << flags << ", " << dev << ")" << endl;
	CHECK_DRIVER;
	return driver->cuCtxCreate(pctx, flags, dev);
}

CUresult  CUDAAPI cuCtxDestroy( CUcontext ctx )
{
	if(verbose) cerr << "cuCtxDestroy(" << ctx << ")" << endl;
	CHECK_DRIVER;
	return driver->cuCtxDestroy(ctx);
}

CUresult  CUDAAPI cuCtxAttach(CUcontext *pctx, unsigned int flags)
{
	if(verbose) cerr << "cuCtxAttach(..., " << flags << ")" << endl;
	CHECK_DRIVER;
	return driver->cuCtxCreate(pctx, flags, 0);	// ?
}

CUresult  CUDAAPI cuCtxDetach(CUcontext ctx)
{
	if(verbose) cerr << "cuCtxDetach(" << ctx << ")" << endl;
	CHECK_DRIVER;
	return driver->cuCtxDestroy(ctx);	// ?
}

CUresult  CUDAAPI cuCtxPushCurrent( CUcontext ctx )
{
	if(verbose) cerr << "cuPushCurrent(" << ctx << ")" << endl;
	CHECK_DRIVER;
	cerr << "function not implemented !!!" << endl;
	assert(false);
}

CUresult  CUDAAPI cuCtxPopCurrent( CUcontext *pctx )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

CUresult  CUDAAPI cuCtxGetDevice(CUdevice *device)
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

CUresult CUDAAPI cuCtxSynchronize()
{
	if(verbose) cerr << "cuCtxSynchronize()" << endl;
	CHECK_DRIVER;
	return CUDA_SUCCESS;
}



/************************************
 **
 **    Module management
 **
 ***********************************/

CUresult CUDAAPI cuModuleLoad(CUmodule *module, const char *fname)
{
	if(verbose) cerr << "cuModuleLoad(..., " << fname << ")" << endl;
	CHECK_DRIVER;
	Module<MyConfig>* mod = 0;
	CUresult res = driver->ModuleLoad(mod, fname);
	assert(mod != 0);
	*module = mod;
	return res;
}

CUresult  CUDAAPI cuModuleLoadData(CUmodule *module, const void *image)
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

CUresult  CUDAAPI cuModuleLoadFatBinary(CUmodule *module, const void *fatCubin)
{
	if(verbose) cerr << "cuModuleLoadFatBinary(..., " << fatCubin << ")" << endl;
	CHECK_DRIVER;
	try
	{
		Module<MyConfig>* mod = new Module<MyConfig>(fatCubin, 17);
		*module = mod;
		return CUDA_SUCCESS;
	}
	// TODO: catch specific exceptions, display and return appropriate errors
	catch(...)
	{
		std::cerr << "Cannot load module\n";
		return CUDA_ERROR_UNKNOWN;
	}
}

CUresult  CUDAAPI cuModuleUnload(CUmodule hmod)
{
	if(verbose) cerr << "cuModuleUnload(" << hmod << ")" << endl;
	CHECK_DRIVER;
	return driver->ModuleUnload(static_cast<Module<MyConfig>*>(hmod));
}

CUresult cuModuleGetFunction(CUfunction *hfunc, CUmodule hmod, const char *name)
{
	if(verbose) cerr << "cuModuleGetFunction(..." << hmod << ", " << name << ")" << endl;
	CHECK_DRIVER;
	CHECK_PTR(hmod);
	Module<MyConfig>* mod = static_cast<Module<MyConfig>*>(hmod);
	Kernel<MyConfig>* kernel;
	try {
		kernel = &mod->GetKernel(name);
		*hfunc = kernel;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuModuleGetGlobal(CUdeviceptr *dptr, unsigned int *bytes, CUmodule hmod, const char *name)
{
	if(verbose) cerr << "cuModuleGetGlobal(..." << hmod << ", " << name << ")" << endl;
	CHECK_DRIVER;
	CHECK_PTR(hmod);
	Module<MyConfig>* mod = static_cast<Module<MyConfig>*>(hmod);
	try {
		MemSegment<MyConfig> & seg = mod->GetGlobal(name);
		if(dptr != 0) {
			// If reloc, need to alloc in device mem first
			driver->CurrentDevice().LoadSegment(seg);
			*dptr = seg.Address();
		}
		if(bytes != 0)
			*bytes = seg.Size();
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuModuleGetTexRef(CUtexref *pTexRef, CUmodule hmod, const char *name)
{
	if(verbose) cerr << "cuModuleGetTexRef(..." << hmod << ", " << name << ")" << endl;
	CHECK_DRIVER;
	CHECK_PTR(hmod);
	Module<MyConfig>* mod = static_cast<Module<MyConfig>*>(hmod);
	try {
		Sampler<MyConfig> & smp = mod->GetSampler(name);
		if(pTexRef != 0) {
			*pTexRef = &smp;
		}
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


/************************************
 **
 **    Memory management
 **
 ***********************************/

CUresult CUDAAPI cuMemGetInfo(unsigned int *free, unsigned int *total)
{
	if(verbose) cerr << "cuMemGetInfo(...)" << endl;
	CHECK_DRIVER;
	try
	{
		driver->CurrentDevice().MemGetInfo(free, total);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuMemAlloc( CUdeviceptr *dptr, unsigned int bytesize)
{
	if(verbose) cerr << "cuMemAlloc(..." << bytesize << ")" << endl;
	CHECK_DRIVER;
	try
	{
		MyConfig::address_t addr = driver->MAlloc(bytesize);
		*dptr = addr;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuMemAllocPitch( CUdeviceptr *dptr, 
				  unsigned int *pPitch,
				  unsigned int WidthInBytes, 
				  unsigned int Height, 
				  // size of biggest r/w to be performed by kernels on this memory
				  // 4, 8 or 16 bytes
				  unsigned int ElementSizeBytes
				  )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

CUresult CUDAAPI cuMemFree(CUdeviceptr dptr)
{
	if(verbose) cerr << "cuMemFree(" << dptr << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->Free(dptr);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuMemGetAddressRange( CUdeviceptr *pbase, unsigned int *psize, CUdeviceptr dptr )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

CUresult CUDAAPI cuMemAllocHost(void **pp, unsigned int bytesize)
{
	if(verbose) cerr << "cuMemAllocHost(..." << bytesize << ")" << endl;
	CHECK_DRIVER;
	try
	{
		*pp = new uint8_t[bytesize];
		return CUDA_SUCCESS;
	}
	catch(std::bad_alloc & ba)
	{
		return CUDA_ERROR_OUT_OF_MEMORY;
	}
}

CUresult CUDAAPI cuMemFreeHost(void *p)
{
	if(verbose) cerr << "cuMemFreeHost(" << p << ")" << endl;
	CHECK_DRIVER;
	uint8_t * ptr = static_cast<uint8_t*>(p);
	delete [] ptr;
	return CUDA_SUCCESS;
}


/************************************
 **
 **    Synchronous Memcpy
 **
 ** Intra-device memcpy's done with these functions may execute in parallel with the CPU,
 ** but if host memory is involved, they wait until the copy is done before returning.
 **
 ***********************************/

// 1D functions
// system <-> device memory
CUresult  CUDAAPI cuMemcpyHtoD (CUdeviceptr dstDevice, const void *srcHost, unsigned int ByteCount )
{
	if(verbose) cerr << "cuMemcpyHtoD(" << dstDevice << ", " << srcHost << ", " << ByteCount << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->CopyHtoD(dstDevice, srcHost, ByteCount);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult  CUDAAPI cuMemcpyDtoH (void *dstHost, CUdeviceptr srcDevice, unsigned int ByteCount )
{
	if(verbose) cerr << "cuMemcpyDtoH(" << dstHost << ", " << srcDevice << ", " << ByteCount << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->CopyDtoH(dstHost, srcDevice, ByteCount);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


// device <-> device memory
CUresult  CUDAAPI cuMemcpyDtoD (CUdeviceptr dstDevice, CUdeviceptr srcDevice, unsigned int ByteCount )
{
	if(verbose) cerr << "cuMemcpyDtoD(" << dstDevice << ", " << srcDevice << ", " << ByteCount << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->CopyDtoD(dstDevice, srcDevice, ByteCount);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


// device <-> array memory
CUresult  CUDAAPI cuMemcpyDtoA ( CUarray dstArray, unsigned int dstIndex, CUdeviceptr srcDevice, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

CUresult  CUDAAPI cuMemcpyAtoD ( CUdeviceptr dstDevice, CUarray hSrc, unsigned int SrcIndex, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


// system <-> array memory
CUresult  CUDAAPI cuMemcpyHtoA( CUarray dstArray, unsigned int dstIndex, const void *pSrc, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuMemcpyAtoH( void *dstHost, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



// array <-> array memory
CUresult  CUDAAPI cuMemcpyAtoA( CUarray dstArray, unsigned int dstIndex, CUarray srcArray, unsigned int srcIndex, unsigned int ByteCount )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



// 2D memcpy
CUresult  CUDAAPI cuMemcpy2D( const CUDA_MEMCPY2D *pCopy )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuMemcpy2DUnaligned( const CUDA_MEMCPY2D *pCopy )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



// 3D memcpy
CUresult  CUDAAPI cuMemcpy3D( const CUDA_MEMCPY3D *pCopy )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



/************************************
 **
 **    Asynchronous Memcpy
 **
 ** Any host memory involved must be DMA'able (e.g., allocated with cuMemAllocHost).
 ** memcpy's done with these functions execute in parallel with the CPU and, if
 ** the hardware is available, may execute in parallel with the GPU.
 ** Asynchronous memcpy must be accompanied by appropriate stream synchronization.
 **
 ***********************************/

// 1D functions
// system <-> device memory
CUresult  CUDAAPI cuMemcpyHtoDAsync (CUdeviceptr dstDevice, 
				     const void *srcHost, unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuMemcpyDtoHAsync (void *dstHost, 
				     CUdeviceptr srcDevice, unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



// system <-> array memory
CUresult  CUDAAPI cuMemcpyHtoAAsync( CUarray dstArray, unsigned int dstIndex, 
				     const void *pSrc, unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuMemcpyAtoHAsync( void *dstHost, CUarray srcArray, unsigned int srcIndex, 
				     unsigned int ByteCount, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



// 2D memcpy
CUresult  CUDAAPI cuMemcpy2DAsync( const CUDA_MEMCPY2D *pCopy, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



// 3D memcpy
CUresult  CUDAAPI cuMemcpy3DAsync( const CUDA_MEMCPY3D *pCopy, CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



/************************************
 **
 **    Memset
 **
 ***********************************/
CUresult  CUDAAPI cuMemsetD8( CUdeviceptr dstDevice, unsigned char uc, unsigned int N )
{
	if(verbose) cerr << "cuMemsetD8(" << dstDevice << ", " << (unsigned int)uc << ", " << N << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->Memset(dstDevice, uc, N);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult  CUDAAPI cuMemsetD16( CUdeviceptr dstDevice, unsigned short us, unsigned int N )
{
	if(verbose) cerr << "cuMemsetD16(" << dstDevice << ", " << us << ", " << N << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->Memset(dstDevice, us, N);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult  CUDAAPI cuMemsetD32( CUdeviceptr dstDevice, unsigned int ui, unsigned int N )
{
	if(verbose) cerr << "cuMemsetD16(" << dstDevice << ", " << ui << ", " << N << ")" << endl;
	CHECK_DRIVER;
	try
	{
		driver->Memset(dstDevice, ui, N);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}



CUresult  CUDAAPI cuMemsetD2D8( CUdeviceptr dstDevice, unsigned int dstPitch, unsigned char uc, unsigned int Width, unsigned int Height )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuMemsetD2D16( CUdeviceptr dstDevice, unsigned int dstPitch, unsigned short us, unsigned int Width, unsigned int Height )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuMemsetD2D32( CUdeviceptr dstDevice, unsigned int dstPitch, unsigned int ui, unsigned int Width, unsigned int Height )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



/************************************
 **
 **    Function management
 **
 ***********************************/


CUresult CUDAAPI cuFuncSetBlockShape(CUfunction hfunc, int x, int y, int z)
{
	if(verbose) cerr << "cuFuncSetBlockShape(" << hfunc << ", "
		<< x << ", " << y << ", " << z << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->SetBlockShape(x, y, z);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
	return CUDA_SUCCESS;
}


CUresult CUDAAPI cuFuncSetSharedSize(CUfunction hfunc, unsigned int bytes)
{
	if(verbose) cerr << "cuFuncSetSharedSize(" << hfunc << ", " << bytes << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->SetSharedSize(bytes);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}



/************************************
 **
 **    Array management 
 **
 ***********************************/
CUresult  CUDAAPI cuArrayCreate( CUarray *pHandle, const CUDA_ARRAY_DESCRIPTOR *pAllocateArray )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuArrayGetDescriptor( CUDA_ARRAY_DESCRIPTOR *pArrayDescriptor, CUarray hArray )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuArrayDestroy( CUarray hArray )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



CUresult  CUDAAPI cuArray3DCreate( CUarray *pHandle, const CUDA_ARRAY3D_DESCRIPTOR *pAllocateArray )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuArray3DGetDescriptor( CUDA_ARRAY3D_DESCRIPTOR *pArrayDescriptor, CUarray hArray )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



/************************************
 **
 **    Texture reference management
 **
 ***********************************/
CUresult  CUDAAPI cuTexRefCreate( CUtexref *pTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuTexRefDestroy( CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



CUresult  CUDAAPI cuTexRefSetArray( CUtexref hTexRef, CUarray hArray, unsigned int Flags )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


// override the texref format with a format inferred from the array
//        #define CU_TRSA_OVERRIDE_FORMAT 0x01
CUresult  CUDAAPI cuTexRefSetAddress( unsigned int *ByteOffset, CUtexref hTexRef, CUdeviceptr dptr, unsigned int bytes )
{
	if(verbose) cerr << "cuTexRefSetAddress(...," << hTexRef << ", " << dptr << ", " << bytes << ")" << endl;
	CHECK_DRIVER;
	Sampler<MyConfig> * sampler = static_cast<Sampler<MyConfig> *>(hTexRef);
	try {
		sampler->SetAddress(dptr, bytes);
		if(ByteOffset != 0)
			*ByteOffset = 0;	// Does anyone use this feature?
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult  CUDAAPI cuTexRefSetFormat( CUtexref hTexRef, CUarray_format fmt, int NumPackedComponents )
{
	if(verbose) cerr << "cuTexRefSetFormat(...," << hTexRef << ", " << fmt << ", " << NumPackedComponents << ")" << endl;
	CHECK_DRIVER;
	Sampler<MyConfig> * sampler = static_cast<Sampler<MyConfig> *>(hTexRef);
	try {
		sampler->SetFormat(ArrayFormat(fmt), NumPackedComponents);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}



CUresult  CUDAAPI cuTexRefSetAddressMode( CUtexref hTexRef, int dim, CUaddress_mode am )
{
	if(verbose) cerr << "cuTexRefSetAddressMode(...," << hTexRef << ", " << dim << ", " << am << ")" << endl;
	CHECK_DRIVER;
	Sampler<MyConfig> * sampler = static_cast<Sampler<MyConfig> *>(hTexRef);
	try {
		sampler->SetAddressMode(dim, AddressMode(am));
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult  CUDAAPI cuTexRefSetFilterMode( CUtexref hTexRef, CUfilter_mode fm )
{
	if(verbose) cerr << "cuTexRefSetFilterMode(...," << hTexRef << ", " << fm << endl;
	CHECK_DRIVER;
	Sampler<MyConfig> * sampler = static_cast<Sampler<MyConfig> *>(hTexRef);
	try {
		sampler->SetFilterMode(FilterMode(fm));
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult  CUDAAPI cuTexRefSetFlags( CUtexref hTexRef, unsigned int Flags )
{
	if(verbose) cerr << "cuTexRefSetFlags(...," << hTexRef << ", " << Flags << endl;
	CHECK_DRIVER;
	Sampler<MyConfig> * sampler = static_cast<Sampler<MyConfig> *>(hTexRef);
	try {
		sampler->SetFlags(Flags);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


// read the texture as integers rather than promoting the values
// to floats in the range [0,1]
//        #define CU_TRSF_READ_AS_INTEGER         0x01

// use normalized texture coordinates in the range [0,1) instead of [0,dim)
//        #define CU_TRSF_NORMALIZED_COORDINATES  0x02

CUresult  CUDAAPI cuTexRefGetAddress( CUdeviceptr *pdptr, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuTexRefGetArray( CUarray *phArray, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuTexRefGetAddressMode( CUaddress_mode *pam, CUtexref hTexRef, int dim )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuTexRefGetFilterMode( CUfilter_mode *pfm, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuTexRefGetFormat( CUarray_format *pFormat, int *pNumChannels, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult  CUDAAPI cuTexRefGetFlags( unsigned int *pFlags, CUtexref hTexRef )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}



/************************************
 **
 **    Parameter management
 **
 ***********************************/

CUresult CUDAAPI cuParamSetSize(CUfunction hfunc, unsigned int numbytes)
{
	if(verbose) cerr << "cuParamSetSize(" << hfunc << ", " << numbytes << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSetSize(numbytes);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuParamSeti(CUfunction hfunc, int offset, unsigned int value)
{
	if(verbose) cerr << "cuParamSeti(" << hfunc << ", " << offset << ", " << value << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSeti(offset, value);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuParamSetf(CUfunction hfunc, int offset, float value)
{
	if(verbose) cerr << "cuParamSetf(" << hfunc << ", " << offset << ", " << value << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSetf(offset, value);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}

CUresult CUDAAPI cuParamSetv(CUfunction hfunc, int offset, void * ptr, unsigned int numbytes)
{
	if(verbose) cerr << "cuParamSetv(" << hfunc << ", " << offset << ", ..." << numbytes << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->ParamSetv(offset, ptr, numbytes);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuParamSetTexRef(CUfunction hfunc, int texunit, CUtexref hTexRef)
{
	if(verbose) cerr << "cuParamSetTexRef(" << hfunc << ", " << texunit << ", " << hTexRef << ")" << endl;
	CHECK_DRIVER;
	if(texunit != CU_PARAM_TR_DEFAULT) return CUDA_ERROR_INVALID_VALUE;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(hfunc);
	try {
		kernel->SetTexRef(static_cast<Sampler<MyConfig> *>(hTexRef));
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


// for texture references loaded into the module,
// use default texunit from texture reference
//        #define CU_PARAM_TR_DEFAULT -1

/************************************
 **
 **    Launch functions
 **
 ***********************************/

CUresult CUDAAPI cuLaunch (CUfunction f)
{
	if(verbose) cerr << "cuLaunch(" << f << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(f);
	try {
		driver->Launch(*kernel);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuLaunchGrid (CUfunction f, int grid_width, int grid_height)
{
	if(verbose) cerr << "cuLaunchGrid(" << f << ", " << grid_width << ", " << grid_height << ")" << endl;
	CHECK_DRIVER;
	Kernel<MyConfig> * kernel = static_cast<Kernel<MyConfig> *>(f);
	try {
		driver->LaunchGrid(*kernel, grid_width, grid_height);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuLaunchGridAsync( CUfunction f, int grid_width, int grid_height, CUstream hStream )
{
	CHECK_DRIVER;
	assert(hStream == 0);
	return cuLaunchGrid(f, grid_width, grid_height);
}



/************************************
 **
 **    Events
 **
 ***********************************/
CUresult CUDAAPI cuEventCreate( CUevent *phEvent, unsigned int Flags )
{
	CHECK_DRIVER;
	try {
		Event<MyConfig>* evt = new Event<MyConfig>(Flags);
		*phEvent = evt;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuEventRecord( CUevent hEvent, CUstream hStream )
{
	CHECK_DRIVER;
	try {
		Event<MyConfig> * evt = static_cast<Event<MyConfig> *>(hEvent);
		evt->Record(/*...hStream*/);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuEventQuery( CUevent hEvent )
{
	CHECK_DRIVER;
	try {
		Event<MyConfig> * evt = static_cast<Event<MyConfig> *>(hEvent);
		return evt->Query() ? CUDA_SUCCESS : CUDA_ERROR_NOT_READY;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuEventSynchronize( CUevent hEvent )
{
	CHECK_DRIVER;
	try {
		Event<MyConfig> * evt = static_cast<Event<MyConfig> *>(hEvent);
		evt->Synchronize();
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuEventDestroy( CUevent hEvent )
{
	CHECK_DRIVER;
	try {
		Event<MyConfig> * evt = static_cast<Event<MyConfig> *>(hEvent);
		delete evt;
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


CUresult CUDAAPI cuEventElapsedTime( float *pMilliseconds, CUevent hStart, CUevent hEnd )
{
	CHECK_DRIVER;
	try {
		Event<MyConfig> * evtStart = static_cast<Event<MyConfig> *>(hStart);
		Event<MyConfig> * evtEnd = static_cast<Event<MyConfig> *>(hEnd);
		*pMilliseconds = evtStart->ElapsedTime(*evtEnd);
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}



/************************************
 **
 **    Streams
 **
 ***********************************/
CUresult CUDAAPI  cuStreamCreate( CUstream *phStream, unsigned int Flags )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult CUDAAPI  cuStreamQuery( CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult CUDAAPI  cuStreamSynchronize( CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}


CUresult CUDAAPI  cuStreamDestroy( CUstream hStream )
{
  cerr << "function not implemented !!!" << endl;
  assert(false);
}

/************************************
 **
 **    Barra extensions (debug, instrumentation...)
 **
 ***********************************/


CUresult CUDAAPI barFunctionDump(CUfunction f)
{
	CHECK_DRIVER;
	try {
		driver->FunctionDump(*static_cast<Kernel<MyConfig>*>(f));
		return CUDA_SUCCESS;
	}
	catch(CudaException e) {
		return e.code;
	}
}


