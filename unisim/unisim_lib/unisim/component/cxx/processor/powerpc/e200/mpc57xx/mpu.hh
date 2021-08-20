/*
 *  Copyright (c) 2007-2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_MPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_MPU_HH__

#include <inttypes.h>
#include <iostream>

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

struct MPU_REGION_DESCRIPTOR
{
	MPU_REGION_DESCRIPTOR() : mas0(0), mas1(0), mas2(0), mas3(0) {}
	
	uint32_t mas0;
	uint32_t mas1;
	uint32_t mas2;
	uint32_t mas3;
};

template <typename TYPES, typename CONFIG>
struct MPU
	: unisim::kernel::Service<unisim::service::interfaces::HttpServer>
{
	typedef typename CONFIG::CPU CPU;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	typedef typename CPU::MAS1 MAS1;
	typedef typename CPU::MAS2 MAS2;
	typedef typename CPU::MAS3 MAS3;
	typedef typename CPU::PID0 PID0;
	typedef typename CPU::MSR MSR;
	static const unsigned int NUM_INST_MPU_ENTRIES = CONFIG::NUM_INST_MPU_ENTRIES;
	static const unsigned int NUM_DATA_MPU_ENTRIES = CONFIG::NUM_DATA_MPU_ENTRIES;
	static const unsigned int NUM_SHARED_MPU_ENTRIES = CONFIG::NUM_SHARED_MPU_ENTRIES;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;

	MPU(CPU *cpu, unsigned int sel);
	void WriteEntry();
	void ReadEntry();
	template <bool force> void Invalidate();
	template <bool EXEC, bool WRITE> MPU_REGION_DESCRIPTOR *Lookup(ADDRESS addr);
	template <bool DEBUG, bool EXEC, bool WRITE> inline bool ControlAccess(ADDRESS addr, ADDRESS& size_to_protection_boundary, STORAGE_ATTR& storage_attr);

	enum DumpFormat
	{
		DFMT_TXT,
		DFMT_HTML
	};
	
	enum RegionDescriptorType
	{
		INST,
		DATA,
		SHARED
	};
	
	void DumpRegionDescriptor(std::ostream& os, MPU_REGION_DESCRIPTOR *mpu_region_descriptor, DumpFormat dump_fmt);
	void DumpRegionDescriptors(std::ostream& os, RegionDescriptorType region_descriptor_type, DumpFormat dump_fmt);
	void Dump(std::ostream& os, DumpFormat dump_fmt);
	
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
protected:
	struct MAS0 : CPU::MAS0
	{
		typedef typename CPU::MAS0 Super;
		
		MAS0(CPU *_cpu) : Super(_cpu) {}
		MAS0(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->Initialize(Super::VLE::template GetMask<uint32_t>() | Super::W::template GetMask<uint32_t>()); // VLE and W sticked to 1
		}
		
		using Super::operator =;
	};
	
	struct MMUCFG : CPU::MMUCFG
	{
		typedef typename CPU::MMUCFG Super;
		
		MMUCFG(CPU *_cpu) : Super(_cpu) {}
		MMUCFG(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->template Set<typename CPU::MMUCFG::RASIZE>(32);
			this->template Set<typename CPU::MMUCFG::PIDSIZE>(PID0::Process_ID::GetBitWidth() - 1);
			this->template Set<typename CPU::MMUCFG::NMPUS>(1);
			this->template Set<typename CPU::MMUCFG::NTLBS>(0);
			this->template Set<typename CPU::MMUCFG::MAVN>(3);
		}
	};
	
	struct MPU0CFG : CPU::MPU0CFG
	{
		typedef typename CPU::MPU0CFG Super;
		
		MPU0CFG(CPU *_cpu) : Super(_cpu) {}
		MPU0CFG(CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->template Set<typename CPU::MPU0CFG::FASSOC>(1);
			this->template Set<typename CPU::MPU0CFG::MINSIZE>(0);   // Smallest region size is 1 byte; Note: Due to the use of access address matching, the effective smallest region size is 8 bytes
			this->template Set<typename CPU::MPU0CFG::MAXSIZE>(0xb); // Largest region size is 4 GB
			this->template Set<typename CPU::MPU0CFG::IPROT>(1);
			this->template Set<typename CPU::MPU0CFG::UAMSKA>(1);
			this->template Set<typename CPU::MPU0CFG::SHENTRY>(0x2);
			this->template Set<typename CPU::MPU0CFG::DENTRY>(0x4);
			this->template Set<typename CPU::MPU0CFG::IENTRY>(0x2);
		}
	};
	
	struct MPU0CSR0 : CPU::MPU0CSR0
	{
		typedef typename CPU::MPU0CSR0 Super;
		
		MPU0CSR0(CPU *_cpu, MPU *_mpu) : Super(_cpu), mpu(_mpu) {}
		MPU0CSR0(CPU *_cpu, MPU *_mpu, uint32_t _value) : Super(_cpu, _value), mpu(_mpu) {}
		
		void Effect()
		{
			if(this->template Get<typename CPU::MPU0CSR0::MPUFI>())
			{
				mpu->Invalidate</* force */ false>();
				this->template Set<typename CPU::MPU0CSR0::MPUFI>(0);
			}
		}
		
		MPU0CSR0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		MPU *mpu;
	};
private:
	template <bool EXEC, bool WRITE> bool Match(MPU_REGION_DESCRIPTOR *mpu_region_descriptor, ADDRESS addr);
	
	CPU *cpu;
	unsigned int sel;
	MAS0 mas0;
	MAS1 mas1;
	MAS2 mas2;
	MAS3 mas3;
	MPU0CFG mpu0cfg;
	MPU0CSR0 mpu0csr0;
	MMUCFG mmucfg;
	MPU_REGION_DESCRIPTOR *mru_inst_mpu_region_descriptor;
	MPU_REGION_DESCRIPTOR *mru_data_mpu_region_descriptor;
	MPU_REGION_DESCRIPTOR *mru_shd_mpu_region_descriptor;
	MPU_REGION_DESCRIPTOR hole_mpu_region_descriptor;
	
	MPU_REGION_DESCRIPTOR inst_mpu_region_descriptors[NUM_INST_MPU_ENTRIES];
	MPU_REGION_DESCRIPTOR data_mpu_region_descriptors[NUM_DATA_MPU_ENTRIES];
	MPU_REGION_DESCRIPTOR shd_mpu_region_descriptors[NUM_SHARED_MPU_ENTRIES];
};

template <typename TYPES, typename CONFIG>
MPU<TYPES, CONFIG>::MPU(CPU *_cpu, unsigned int _sel)
	: unisim::kernel::Object("MPU", _cpu, "Memory Protection Unit")
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>("MPU", _cpu)
	, http_server_export("http-server-export", this)
	, cpu(_cpu)
	, sel(_sel)
	, mas0(_cpu)
	, mas1(_cpu)
	, mas2(_cpu)
	, mas3(_cpu)
	, mpu0cfg(_cpu)
	, mpu0csr0(_cpu, this)
	, mmucfg(_cpu)
	, mru_inst_mpu_region_descriptor(0)
	, mru_data_mpu_region_descriptor(0)
	, mru_shd_mpu_region_descriptor(0)
	, hole_mpu_region_descriptor()
{
	Invalidate</* force */ true>();
	MAS0::VALID::Set(hole_mpu_region_descriptor.mas0, 1U); // Valid
	MAS0::I::Set(hole_mpu_region_descriptor.mas0, 0U); // *NOT* cache inhibited
	MAS0::G::Set(hole_mpu_region_descriptor.mas0, 0U); // *NOT* guarded
	MAS2::UPPER_BOUND::Set(hole_mpu_region_descriptor.mas2, ~ADDRESS(0));
	MAS3::LOWER_BOUND::Set(hole_mpu_region_descriptor.mas3, 0U);
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::WriteEntry()
{
	unsigned int sel = mas0.template Get<typename MAS0::SEL>();
	
	if(sel == mas0.template Get<typename MAS0::SEL>())
	{
		unsigned int esel = mas0.template Get<typename MAS0::ESEL>();
		unsigned int inst = mas0.template Get<typename MAS0::INST>();
		unsigned int shd = mas0.template Get<typename MAS0::SHD>();
		
		MPU_REGION_DESCRIPTOR *mpu_region_descriptor = 0;
		
		if(shd)
		{
			// shared entry
			mpu_region_descriptor = (esel < NUM_SHARED_MPU_ENTRIES) ? &shd_mpu_region_descriptors[esel] : 0;
		}
		else if(inst)
		{
			// pure instruction entry
			mpu_region_descriptor = (esel < NUM_INST_MPU_ENTRIES) ? &inst_mpu_region_descriptors[esel] : 0;
		}
		else
		{
			// pure data entry
			mpu_region_descriptor = (esel < NUM_DATA_MPU_ENTRIES) ? &data_mpu_region_descriptors[esel] : 0;
		}
		
		if(mpu_region_descriptor && !MAS0::RO::Get(mpu_region_descriptor->mas0))
		{
			mpu_region_descriptor->mas0 = MAS0::IPROT::Get(mpu_region_descriptor->mas0) ? ((mpu_region_descriptor->mas0 & MAS0::VALID::template GetMask<uint32_t>()) | mas0) : mas0;
			mpu_region_descriptor->mas1 = mas1;
			mpu_region_descriptor->mas2 = mas2;
			mpu_region_descriptor->mas3 = mas3;
		}
	}
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::ReadEntry()
{
	MPU_REGION_DESCRIPTOR *mpu_region_descriptor = 0;
	
	if(sel == mas0.template Get<typename MAS0::SEL>())
	{
		unsigned int esel = mas0.template Get<typename MAS0::ESEL>();
		unsigned int inst = mas0.template Get<typename MAS0::INST>();
		unsigned int shd = mas0.template Get<typename MAS0::SHD>();
		
		if(shd)
		{
			// shared entry
			mpu_region_descriptor = (esel < NUM_SHARED_MPU_ENTRIES) ? &shd_mpu_region_descriptors[esel] : 0;
		}
		else if(inst)
		{
			// pure instruction entry
			mpu_region_descriptor = (esel < NUM_INST_MPU_ENTRIES) ? &inst_mpu_region_descriptors[esel] : 0;
		}
		else
		{
			// pure data entry
			mpu_region_descriptor = (esel < NUM_DATA_MPU_ENTRIES) ? &data_mpu_region_descriptors[esel] : 0;
		}
	}
	
	mas0 = (mpu_region_descriptor ? mpu_region_descriptor->mas0 : 0);
	mas1 = (mpu_region_descriptor ? mpu_region_descriptor->mas1 : 0);
	mas2 = (mpu_region_descriptor ? mpu_region_descriptor->mas2 : 0);
	mas3 = (mpu_region_descriptor ? mpu_region_descriptor->mas3 : 0);
}

template <typename TYPES, typename CONFIG>
template <bool force>
void MPU<TYPES, CONFIG>::Invalidate()
{
	unsigned int esel;
	
	for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
	{
		MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &inst_mpu_region_descriptors[esel];
		
		if(force || !MAS0::IPROT::Get(mpu_region_descriptor->mas0))
		{
			mpu_region_descriptor->mas0 = 0;
			mpu_region_descriptor->mas1 = 0;
			mpu_region_descriptor->mas2 = 0;
			mpu_region_descriptor->mas3 = 0;
			MAS0::SEL::Set(mpu_region_descriptor->mas0, sel);
			MAS0::ESEL::Set(mpu_region_descriptor->mas0, esel);
			MAS0::INST::Set(mpu_region_descriptor->mas0, 1U);
		}
	}
	
	for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
	{
		MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &data_mpu_region_descriptors[esel];
		if(force || !MAS0::IPROT::Get(mpu_region_descriptor->mas0))
		{
			mpu_region_descriptor->mas0 = 0;
			mpu_region_descriptor->mas1 = 0;
			mpu_region_descriptor->mas2 = 0;
			mpu_region_descriptor->mas3 = 0;
			MAS0::SEL::Set(mpu_region_descriptor->mas0, sel);
			MAS0::ESEL::Set(mpu_region_descriptor->mas0, esel);
		}
	}
	
	for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
	{
		MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &shd_mpu_region_descriptors[esel];
		if(force || !MAS0::IPROT::Get(mpu_region_descriptor->mas0))
		{
			mpu_region_descriptor->mas0 = 0;
			mpu_region_descriptor->mas1 = 0;
			mpu_region_descriptor->mas2 = 0;
			mpu_region_descriptor->mas3 = 0;
			MAS0::SEL::Set(mpu_region_descriptor->mas0, sel);
			MAS0::ESEL::Set(mpu_region_descriptor->mas0, esel);
			MAS0::SHD::Set(mpu_region_descriptor->mas0, 1U);
		}
	}

	mru_inst_mpu_region_descriptor = &inst_mpu_region_descriptors[0];
	mru_data_mpu_region_descriptor = &data_mpu_region_descriptors[0];
	mru_shd_mpu_region_descriptor = &shd_mpu_region_descriptors[0];
}

template <typename TYPES, typename CONFIG>
template <bool EXEC, bool WRITE>
bool MPU<TYPES, CONFIG>::Match(MPU_REGION_DESCRIPTOR *mpu_region_descriptor, ADDRESS addr)
{
	MSR& msr = cpu->msr;
	PID0& pid0 = cpu->pid0;
	unsigned int pid = pid0.template Get<typename PID0::Process_ID>();

	if(MAS0::VALID::Get(mpu_region_descriptor->mas0))
	{
		// valid MPU entry
		unsigned int pr = msr.template Get<typename MSR::PR>();
		
		if(( EXEC && ((pr && (mpu0csr0.template Get<typename MPU0CSR0::BYPUX>()            || MAS0::UX_UR::Get(mpu_region_descriptor->mas0))) || (!pr && (mpu0csr0.template Get<typename MPU0CSR0::BYPSX>()    || MAS0::SX_SR::Get(mpu_region_descriptor->mas0))))) ||
		   (!EXEC && ((pr && ((WRITE && (mpu0csr0.template Get<typename MPU0CSR0::BYPUW>() || MAS0::UW::Get(mpu_region_descriptor->mas0)))    || (!WRITE && (mpu0csr0.template Get<typename MPU0CSR0::BYPUR>() || MAS0::UX_UR::Get(mpu_region_descriptor->mas0))))) ||
		             (!pr && ((WRITE && (mpu0csr0.template Get<typename MPU0CSR0::BYPSW>() || MAS0::SW::Get(mpu_region_descriptor->mas0)))    || (!WRITE && (mpu0csr0.template Get<typename MPU0CSR0::BYPSR>() || MAS0::SX_SR::Get(mpu_region_descriptor->mas0) || MAS0::SW::Get(mpu_region_descriptor->mas0))))))))
		{
			// Access right match
			unsigned int tid = MAS1::TID::Get(mpu_region_descriptor->mas1);
			unsigned int tidmsk = MAS1::TIDMSK::Get(mpu_region_descriptor->mas1);
			
			if(!tid || (((tid ^ pid) & ~tidmsk) == 0) || (!pr && mpu0csr0.template Get<typename MPU0CSR0::TIDCTL>()))
			{
				// TID match
				
				// Note: Due to the use of access address matching, the effective smallest region size is 8 bytes
				struct B29_31 : Field<void, 29, 31> {};
				
				ADDRESS upper_bound = MAS2::UPPER_BOUND::Get(mpu_region_descriptor->mas2);
				ADDRESS lower_bound = MAS3::LOWER_BOUND::Get(mpu_region_descriptor->mas3);
				
				unsigned int uamsk = MAS0::UAMSK::Get(mpu_region_descriptor->mas0);
				ADDRESS addr_mask = uamsk ? ~((int32_t) 0x80000000 >> (uamsk - 1)) : ~ADDRESS(0);
				B29_31::Set(addr_mask, 0U);
				
				ADDRESS masked_addr = addr & addr_mask;
				ADDRESS masked_upper_bound = upper_bound & addr_mask;
				ADDRESS masked_lower_bound = lower_bound & addr_mask;
				
				if((masked_addr >= masked_lower_bound) && (masked_addr <= masked_upper_bound))
				{
					// Address match
					return true;
				}
			}
		}
	}
	
	return false;
}

template <typename TYPES, typename CONFIG>
template <bool EXEC, bool WRITE>
MPU_REGION_DESCRIPTOR *MPU<TYPES, CONFIG>::Lookup(ADDRESS addr)
{
	if(mpu0csr0.template Get<typename MPU0CSR0::MPUEN>())
	{
		unsigned int esel;

		if(EXEC)
		{
			// instruction
			if(Match<EXEC, WRITE>(mru_inst_mpu_region_descriptor, addr)) return mru_inst_mpu_region_descriptor;
			
			for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
			{
				MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &inst_mpu_region_descriptors[esel];
				
				if(Match<EXEC, WRITE>(mpu_region_descriptor, addr))
				{
					mru_inst_mpu_region_descriptor = mpu_region_descriptor;
					return mpu_region_descriptor;
				}
			}
		}
		else
		{
			// data
			if(Match<EXEC, WRITE>(mru_data_mpu_region_descriptor, addr)) return mru_data_mpu_region_descriptor;
			
			for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
			{
				MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &data_mpu_region_descriptors[esel];
				
				if(Match<EXEC, WRITE>(mpu_region_descriptor, addr))
				{
					mru_data_mpu_region_descriptor = mpu_region_descriptor;
					return mpu_region_descriptor;
				}
			}
		}
		
		// shared
		if(Match<EXEC, WRITE>(mru_shd_mpu_region_descriptor, addr)) return mru_shd_mpu_region_descriptor;

		for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
		{
			MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &shd_mpu_region_descriptors[esel];
			
			if(Match<EXEC, WRITE>(mpu_region_descriptor, addr))
			{
				mru_shd_mpu_region_descriptor = mpu_region_descriptor;
				return mpu_region_descriptor;
			}
		}
	}
	else
	{
		return &hole_mpu_region_descriptor;
	}

	return 0;
}

template <typename TYPES, typename CONFIG>
template <bool DEBUG, bool EXEC, bool WRITE>
inline bool MPU<TYPES, CONFIG>::ControlAccess(ADDRESS addr, ADDRESS& size_to_protection_boundary, STORAGE_ATTR& storage_attr)
{
	MPU_REGION_DESCRIPTOR *mpu_region_descriptor = Lookup<EXEC, WRITE>(addr);
	
	if(unlikely(!mpu_region_descriptor))
	{
		if(DEBUG)
		{
			mpu_region_descriptor = &hole_mpu_region_descriptor;
		}
		else
		{
			if(EXEC)
			{
				cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::AccessControl>();
			}
			else
			{
				cpu->template ThrowException<typename CPU::DataStorageInterrupt::AccessControl>().SetAddress(addr);
			}
			return false;
		}
	}
	
	size_to_protection_boundary = MAS2::UPPER_BOUND::Get(mpu_region_descriptor->mas2) - addr + 1;
	storage_attr = STORAGE_ATTR((MAS0::I::Get(mpu_region_descriptor->mas0) ? TYPES::SA_I : 0) | (MAS0::G::Get(mpu_region_descriptor->mas0) ? TYPES::SA_G : 0));
	
	return true;
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::DumpRegionDescriptor(std::ostream& os, MPU_REGION_DESCRIPTOR *mpu_region_descriptor, DumpFormat dump_fmt)
{
	std::ostream::char_type fill(os.fill());
	std::ios_base::fmtflags flags(os.flags());
	os.fill('0');
	
	switch(dump_fmt)
	{
		case DFMT_TXT:
			os << "[valid=" << std::dec << MAS0::VALID::Get(mpu_region_descriptor->mas0) << ",";
			os << "iprot=" << std::dec << MAS0::IPROT::Get(mpu_region_descriptor->mas0) << ",";
			os << "sel=" << std::dec << MAS0::SEL::Get(mpu_region_descriptor->mas0) << ",";
			os << "ro=" << std::dec << MAS0::RO::Get(mpu_region_descriptor->mas0) << ",";
			os << "debug=" << std::dec << MAS0::DEBUG::Get(mpu_region_descriptor->mas0) << ",";
			os << "inst=" << std::dec << MAS0::INST::Get(mpu_region_descriptor->mas0) << ",";
			os << "shd=" << std::dec << MAS0::SHD::Get(mpu_region_descriptor->mas0) << ",";
			os << "esel=" << std::dec << MAS0::ESEL::Get(mpu_region_descriptor->mas0) << ",";
			os << "uamsk=" << std::dec << MAS0::UAMSK::Get(mpu_region_descriptor->mas0) << ",";
			os << "uw=" << std::dec << MAS0::UW::Get(mpu_region_descriptor->mas0) << ",";
			os << "sw=" << std::dec << MAS0::SW::Get(mpu_region_descriptor->mas0) << ",";
			os << "ux_ur=" << std::dec << MAS0::UX_UR::Get(mpu_region_descriptor->mas0) << ",";
			os << "sx_sr=" << std::dec << MAS0::SX_SR::Get(mpu_region_descriptor->mas0) << ",";
			os << "iovr=" << std::dec << MAS0::IOVR::Get(mpu_region_descriptor->mas0) << ",";
			os << "govr=" << std::dec << MAS0::GOVR::Get(mpu_region_descriptor->mas0) << ",";
			os << "i=" << std::dec << MAS0::I::Get(mpu_region_descriptor->mas0) << ",";
			os << "g=" << std::dec << MAS0::G::Get(mpu_region_descriptor->mas0) << ",";
			os << "tid=0x" << std::hex << MAS1::TID::Get(mpu_region_descriptor->mas1) << ",";
			os << "tidmsk=0x" << std::hex << MAS1::TIDMSK::Get(mpu_region_descriptor->mas1) << ",";
			os << "upper_bound=0x" << std::hex << MAS2::UPPER_BOUND::Get(mpu_region_descriptor->mas2) << ",";
			os << "lower_bound=0x" << std::hex << MAS3::LOWER_BOUND::Get(mpu_region_descriptor->mas3) << "]";
			break;
			
		case DFMT_HTML:
			os << "\t\t\t<tr class=\"" << (MAS0::VALID::Get(mpu_region_descriptor->mas0) ? "valid" : "invalid") << "\">";
			os << "<td><span>" << std::dec << MAS0::VALID::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::IPROT::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::SEL::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::RO::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::DEBUG::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::INST::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::SHD::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::ESEL::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::UAMSK::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::UW::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::SW::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::UX_UR::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::SX_SR::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::IOVR::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::GOVR::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::I::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>" << std::dec << MAS0::G::Get(mpu_region_descriptor->mas0) << "</span></td>";
			os << "<td><span>0x" << std::hex << MAS1::TID::Get(mpu_region_descriptor->mas1) << "</span></td>";
			os << "<td><span>0x" << std::hex << MAS1::TIDMSK::Get(mpu_region_descriptor->mas1) << "</span></td>";
			os << "<td class=\"address\"><span>0x" << std::hex; os.width(8); os << MAS2::UPPER_BOUND::Get(mpu_region_descriptor->mas2) << "</span></td>";
			os << "<td class=\"address\"><span>0x" << std::hex; os.width(8); os << MAS3::LOWER_BOUND::Get(mpu_region_descriptor->mas3) << "</span></td>";
			os << "</tr>";
			break;
	}
	os.fill(fill);
	os.flags(flags);
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::DumpRegionDescriptors(std::ostream& os, RegionDescriptorType region_descriptor_type, DumpFormat dump_fmt)
{
	switch(region_descriptor_type)
	{
		case INST:
			if(dump_fmt == DFMT_HTML)
			{
				os << "\t\t<h2>Instruction region descriptors</h2>" << std::endl;
				os << "\t\t<table id=\"INST\">" << std::endl;
			}
			break;
		case DATA:
			if(dump_fmt == DFMT_HTML)
			{
				os << "\t\t<h2>Data region descriptors</h2>" << std::endl;
				os << "\t\t<table id=\"DATA\">" << std::endl;
			}
			break;
		case SHARED:
			if(dump_fmt == DFMT_HTML)
			{
				os << "\t\t<h2>Shared region descriptors</h2>" << std::endl;
				os << "\t\t<table id=\"SHARED\">" << std::endl;
			}
			break;
	}
	
	if(dump_fmt == DFMT_HTML)
	{
		os << "\t\t\t<thead>" << std::endl;
		os << "\t\t\t\t<tr>";
		os << "<th><span>VALID</span></th>";
		os << "<th><span>IPROT</span></th>";
		os << "<th><span>SEL</span></th>";
		os << "<th><span>RO</span></th>";
		os << "<th><span>DEBUG</span></th>";
		os << "<th><span>INST</span></th>";
		os << "<th><span>SHD</span></th>";
		os << "<th><span>ESEL</span></th>";
		os << "<th><span>UAMSK</span></th>";
		os << "<th><span>UW</span></th>";
		os << "<th><span>SW</span></th>";
		os << "<th><span>UX_UR</span></th>";
		os << "<th><span>SX_SR</span></th>";
		os << "<th><span>IOVR</span></th>";
		os << "<th><span>GOVR</span></th>";
		os << "<th><span>I</span></th>";
		os << "<th><span>G</span></th>";
		os << "<th><span>TID</span></th>";
		os << "<th><span>TIDMSK</span></th>";
		os << "<th><span>UPPER_BOUND</span></th>";
		os << "<th><span>LOWER_BOUND</span></th>";
		os << "</tr>" << std::endl;
		os << "\t\t\t</thead>" << std::endl;
		os << "\t\t\t<tbody>" << std::endl;
	}

	unsigned int esel;

	switch(region_descriptor_type)
	{
		case INST:
			for(esel = 0; esel < NUM_INST_MPU_ENTRIES; esel++)
			{
				MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &inst_mpu_region_descriptors[esel];
				
				DumpRegionDescriptor(os, mpu_region_descriptor, dump_fmt); os << std::endl;
			}
			break;
		case DATA:
			for(esel = 0; esel < NUM_DATA_MPU_ENTRIES; esel++)
			{
				MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &data_mpu_region_descriptors[esel];
				
				DumpRegionDescriptor(os, mpu_region_descriptor, dump_fmt); os << std::endl;
			}
			break;
		case SHARED:
			for(esel = 0; esel < NUM_SHARED_MPU_ENTRIES; esel++)
			{
				MPU_REGION_DESCRIPTOR *mpu_region_descriptor = &shd_mpu_region_descriptors[esel];

				DumpRegionDescriptor(os, mpu_region_descriptor, dump_fmt); os << std::endl;
			}
			break;
	}

	switch(region_descriptor_type)
	{
		case INST:
		case DATA:
		case SHARED:
			if(dump_fmt == DFMT_HTML)
			{
				os << "\t\t\t</tbody>" << std::endl;
				os << "\t\t</table>" << std::endl;
			}
			break;
	}

}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::Dump(std::ostream& os, DumpFormat dump_fmt)
{
	DumpRegionDescriptors(os, INST, dump_fmt);
	DumpRegionDescriptors(os, DATA, dump_fmt);
	DumpRegionDescriptors(os, SHARED, dump_fmt);
}

template <typename TYPES, typename CONFIG>
bool MPU<TYPES, CONFIG>::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetPath() == "")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, GET, HEAD");
				break;
				
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			{
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html>" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>" << unisim::util::hypapp::HTML_Encoder::Encode(this->GetName()) << "</title>" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"user interface for MPU\">" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/component/cxx/processor/powerpc/e200/mpc57xx/mpu_style.css\" type=\"text/css\" />" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				//response << "\t\t<script src=\"/unisim/component/cxx/processor/powerpc/e200/mpc57xx/mpu_script.js\"></script>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				Dump(response, DFMT_HTML);
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				
				break;
			}
			
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, GET, HEAD");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html>" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error 405 (Method Not Allowed)</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error 405 (Method Not Allowed)\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>HTTP Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				break;
		}
	}
	else
	{
		response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
		
		response << "<!DOCTYPE html>" << std::endl;
		response << "<html>" << std::endl;
		response << "\t<head>" << std::endl;
		response << "\t\t<title>Error 404 (Not Found)</title>" << std::endl;
		response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << "\t\t<meta name=\"description\" content=\"Error 404 (Not Found)\">" << std::endl;
		response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
		response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << "\t\t<style>" << std::endl;
		response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
		response << "\t\t</style>" << std::endl;
		response << "\t</head>" << std::endl;
		response << "\t<body>" << std::endl;
		response << "\t\t<p>Unavailable</p>" << std::endl;
		response << "\t</body>" << std::endl;
		response << "</html>" << std::endl;
	}
	
	return conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));
}

template <typename TYPES, typename CONFIG>
void MPU<TYPES, CONFIG>::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
}

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_MPU_HH__
