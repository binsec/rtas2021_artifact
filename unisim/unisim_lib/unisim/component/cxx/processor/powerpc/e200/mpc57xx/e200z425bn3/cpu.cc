/*
 *  Copyright (c) 2017,
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

#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/cpu.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {
namespace e200z425bn3 {

CPU::CPU(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "e200z425bn3 PowerPC core")
	, SuperCPU(name, parent)
	, mpu_http_server_export("mpu-http-server-export", this)
	, mpu(this, 0x2)
	, l1i(this)
	, imem(this)
	, dmem(this)
	, l1cfg0(this)
	, l1cfg1(this)
	, l1csr0(this)
{
	mpu_http_server_export >> mpu.http_server_export;
}

CPU::~CPU()
{
}

bool CPU::Dcba(EFFECTIVE_ADDRESS addr)
{
	// dcba is treated as no-op on e200z
	return true;
}

bool CPU::Dcbi(EFFECTIVE_ADDRESS addr)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}

	return true; // dcbi is treated as a no-op in supervisor mode if the data cache is disabled
}

bool CPU::Dcbf(EFFECTIVE_ADDRESS addr)
{
	return true; // dcbf is treated as a no-op if the data cache is disabled
}

bool CPU::Dcbst(EFFECTIVE_ADDRESS addr)
{
	return true; // dcbst is treated as a no-op if the data cache is disabled
}

bool CPU::Dcbt(EFFECTIVE_ADDRESS addr)
{
	if(hid0.Get<HID0::NOPTI>()) return true; // icbt is treated as a no-op if HID0[NOPTI]=1
	
	// TODO: data cache software prefetching
	return true;
}

bool CPU::Dcbtst(EFFECTIVE_ADDRESS addr)
{
	if(hid0.Get<HID0::NOPTI>()) return true; // icbt is treated as a no-op if HID0[NOPTI]=1

	// TODO: data cache software prefetching
	return true;
}

bool CPU::Dcbz(EFFECTIVE_ADDRESS addr)
{
	// dcbz with writethrough cache results in an alignment interrupt
	ThrowException<AlignmentInterrupt::WriteThroughDCBZ>().SetAddress(addr);
	return false;
}

bool CPU::Icbi(EFFECTIVE_ADDRESS addr)
{
	if(msr.Get<MSR::PR>())
	{
		ThrowException<ProgramInterrupt::PrivilegeViolation>();
		return false;
	}
	
	MPU_REGION_DESCRIPTOR *mpu_region_descriptor = mpu.Lookup</* exec */ false, /* write */ false>(addr); // icbi is treated as a load for the purpose of access protection
	
	if(!mpu_region_descriptor)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}

	InvalidateLineByAddress<L1I>(addr, addr);
	return true;
}

bool CPU::Icbt(EFFECTIVE_ADDRESS addr)
{
	if(hid0.Get<HID0::NOPTI>()) return true; // icbt is treated as a no-op if HID0[NOPTI]=1
	
	MPU_REGION_DESCRIPTOR *mpu_region_descriptor = mpu.Lookup</* exec */ false, /* write */ false>(addr); // icbt is treated as a load for the purpose of access protection
	
	if(!mpu_region_descriptor)
	{
		ThrowException<DataStorageInterrupt::AccessControl>().SetAddress(addr);
		return false;
	}

	// TODO: instruction cache software prefetching
	return true;
}

bool CPU::Mpure()
{
	mpu.ReadEntry();
	return true;
}

bool CPU::Mpuwe()
{
	mpu.WriteEntry();
	return true;
}

bool CPU::Mpusync()
{
	// mpusync is treated as no-op in this architecture
	return true;
}

} // end of namespace e200z425bn3
} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
