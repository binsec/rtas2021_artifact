/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_EBI_EBI_HH__
#define __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_EBI_EBI_HH__

#include <unisim/component/tlm2/interconnect/programmable_router/router.hh>
#include <unisim/util/debug/simple_register_registry.hh>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interconnect {
namespace freescale {
namespace mpc57xx {
namespace ebi {

using unisim::component::tlm2::interconnect::generic_router::MemoryRegion;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::AddressableRegister;
using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::AddressableRegisterFile;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_R_HW_RO;
using unisim::util::reg::core::ReadWriteStatus;
using unisim::util::reg::core::IsReadWriteError;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

#if 0
struct CONFIG
{
	static const unsigned int BUSWIDTH = 32;
};
#endif

template <typename CONFIG>
class MappingCache
{
public:
	typedef typename CONFIG::MAPPING MAPPING;
	
	MappingCache();
	void Insert(uint64_t range_start, uint64_t range_end, uint64_t translation, unsigned int output_port);
	bool ApplyMap(uint64_t addr, uint32_t size, MAPPING const *& applied_mapping);
	void Invalidate();
	
	MAPPING const& operator [] (unsigned int output_port);
private:
	class MappingCacheEntry : public MAPPING
	{
	public:
		MappingCacheEntry()
			: MAPPING()
			, next(0)
		{
		}
		
		MappingCacheEntry *next;
	};
	
	MappingCacheEntry *mru_entry;
	MappingCacheEntry entries[CONFIG::OUTPUT_SOCKETS];
};

template <typename CONFIG>
MappingCache<CONFIG>::MappingCache()
	: mru_entry(0)
	, entries()
{
	MappingCacheEntry *next_entry = 0;
	int output_port;
	for(output_port = CONFIG::OUTPUT_SOCKETS - 1; output_port >= 0; output_port--)
	{
		MappingCacheEntry& entry = entries[output_port];
		entry.output_port = output_port;
		entry.translation = 0;
		entry.id = output_port;
		entry.next = next_entry;
		next_entry = &entry;
	}
	mru_entry = next_entry;
}

template <typename CONFIG>
void MappingCache<CONFIG>::Insert(uint64_t range_start, uint64_t range_end, uint64_t translation, unsigned int output_port)
{
	MappingCacheEntry& entry = entries[output_port];
	entry.used = true;
	entry.range_start = range_start;
	entry.range_end = range_end;
	entry.translation = translation;
}

template <typename CONFIG>
bool MappingCache<CONFIG>::ApplyMap(uint64_t addr, uint32_t size, MAPPING const *& applied_mapping)
{
	for(MappingCacheEntry *prev_entry = 0, *entry = mru_entry; entry != 0; prev_entry = entry, entry = entry->next)
	{
		if(likely(entry->Match(addr, size)))
		{
			applied_mapping = entry;
			
			if(unlikely(entry != mru_entry))
			{
				prev_entry->next = entry->next;
				entry->next = mru_entry;
				mru_entry = entry;
			}
			return true;
		}
	}
	
	return false;
}

template <typename CONFIG>
void MappingCache<CONFIG>::Invalidate()
{
	unsigned int output_port;
	for(output_port = 0; output_port < CONFIG::OUTPUT_SOCKETS; output_port++)
	{
		entries[output_port].used = false;
	}
}

template <typename CONFIG>
typename MappingCache<CONFIG>::MAPPING const& MappingCache<CONFIG>::operator [] (unsigned int output_port)
{
	return entries[output_port];
}

template <typename CONFIG>
class TemporaryMappingKeeper
{
public:
	typedef typename CONFIG::MAPPING MAPPING;

	TemporaryMappingKeeper();
	~TemporaryMappingKeeper();
	MAPPING const *CreateMapping(uint64_t range_start, uint64_t range_end, uint64_t translation, unsigned int output_port);
	void Clear();
private:
	MAPPING *Allocate();
	
	std::vector<MAPPING *> tmp;
	std::vector<MAPPING *> pool;
};

template <typename CONFIG>
TemporaryMappingKeeper<CONFIG>::TemporaryMappingKeeper()
	: tmp()
	, pool()
{
}

template <typename CONFIG>
TemporaryMappingKeeper<CONFIG>::~TemporaryMappingKeeper()
{
	for(typename std::vector<MAPPING *>::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		MAPPING *mapping = *it;
		delete mapping;
	}
	for(typename std::vector<MAPPING *>::iterator it = pool.begin(); it != pool.end(); it++)
	{
		MAPPING *mapping = *it;
		delete mapping;
	}
}

template <typename CONFIG>
typename TemporaryMappingKeeper<CONFIG>::MAPPING const *TemporaryMappingKeeper<CONFIG>::CreateMapping(uint64_t range_start, uint64_t range_end, uint64_t translation, unsigned int output_port)
{
	MAPPING *mapping = Allocate();
	mapping->used = true;
	mapping->range_start = range_start;
	mapping->range_end = range_end;
	mapping->translation = translation;
	mapping->output_port = output_port;
	tmp.push_back(mapping);
	return mapping;
}

template <typename CONFIG>
void TemporaryMappingKeeper<CONFIG>::Clear()
{
	for(typename std::vector<MAPPING *>::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		MAPPING *mapping = *it;
		pool.push_back(mapping);
	}
	tmp.clear();
}

template <typename CONFIG>
typename TemporaryMappingKeeper<CONFIG>::MAPPING *TemporaryMappingKeeper<CONFIG>::Allocate()
{
	if(!pool.empty())
	{
		MAPPING *mapping = pool.back();
		pool.pop_back();
		return mapping;
	}
	
	return new MAPPING();
}


template <typename CONFIG>
class EBI
	: public unisim::component::tlm2::interconnect::programmable_router::Router<CONFIG>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	typedef unisim::component::tlm2::interconnect::programmable_router::Router<CONFIG> Super;
	typedef typename Super::transaction_type transaction_type;
	typedef typename Super::MAPPING MAPPING;
	
	static const unsigned int TLM2_IP_VERSION_MAJOR   = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR   = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH   = 0;
	static const bool threaded_model                  = false;
	static const unsigned int INPUT_ADDRESS_WIDTH     = CONFIG::INPUT_ADDRESS_WIDTH;
	static const unsigned int OUTPUT_ADDRESS_WIDTH    = CONFIG::OUTPUT_ADDRESS_WIDTH;
	static const unsigned int TIED_UPPED_ADDRESS_BITS = CONFIG::TIED_UPPED_ADDRESS_BITS;
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	EBI(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~EBI();

	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);
protected:
	virtual void end_of_elaboration();
	virtual void Reset();
	
	virtual bool ApplyMap(uint64_t addr, uint32_t size, MAPPING const *&applied_mapping, MemoryRegion *mem_rgn);
	virtual void ApplyMap(uint64_t addr, uint32_t size, std::vector<MAPPING const *> &port_mappings);

	// Common EBI register representation
	template <typename REGISTER, Access _ACCESS>
	struct EBI_Register : AddressableRegister<REGISTER, 32, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS> Super;
		
		EBI_Register(EBI<CONFIG> *_ebi) : Super(), ebi(_ebi) {}
		EBI_Register(EBI<CONFIG> *_ebi, uint32_t value) : Super(value), ebi(_ebi) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return ebi->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return ebi->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return ebi->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		EBI<CONFIG> *ebi;
	};
	
	//  Module Configuration Register (EBI_MCR)
	struct EBI_MCR : EBI_Register<EBI_MCR, SW_RW>
	{
		typedef EBI_Register<EBI_MCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;

		struct ACGE     : Field<EBI_MCR, ACGE    , 16    > {}; // Automatic CLKOUT Gating Enable
		struct Reserved : Field<EBI_MCR, Reserved, 19, 20> {}; // This field is reserved
		struct MDIS     : Field<EBI_MCR, MDIS    , 25    > {}; // Module Disable Mode
		struct D16_31   : Field<EBI_MCR, D16_31  , 29    > {}; // Data Bus 16_31 Select
		struct DBM      : Field<EBI_MCR, DBM     , 31    > {}; // Data Bus Mode

		typedef FieldSet<ACGE, Reserved, MDIS, D16_31, DBM> ALL;
		
		EBI_MCR(EBI<CONFIG> *_ebi) : Super(_ebi) { Init(); }
		EBI_MCR(EBI<CONFIG> *_ebi, uint32_t value) : Super(_ebi, value) { Init(); }
		
		void Init()
		{
			this->SetName("EBI_MCR"); this->SetDescription("Module Configuration Register");
			
			ACGE    ::SetName("ACGE");     ACGE    ::SetDescription("Automatic CLKOUT Gating Enable");
			Reserved::SetName("Reserved"); Reserved::SetDescription("This field is reserved");
			MDIS    ::SetName("MDIS");     MDIS    ::SetDescription("Module Disable Mode");
			D16_31  ::SetName("D16_31");   D16_31  ::SetDescription("Data Bus 16_31 Select");
			DBM     ::SetName("DBM");      DBM     ::SetDescription("Data Bus Mode");
		}
		
		void Reset()
		{
			this->Initialize(0x00000800);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying MCR may affect memory mapping, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->ebi->DMI_Invalidate();
			
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->ebi->mapping_cache.Invalidate();
			}
			
			return rws;
		}
		
		virtual void DebugWrite(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying MCR may affect memory mapping, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->ebi->DMI_Invalidate();
			this->ebi->mapping_cache.Invalidate();
			Super::DebugWrite(value, bit_enable);
		}
	};
	
	// Base Register Bank (EBI_BRn)
	struct EBI_BR : EBI_Register<EBI_BR, SW_RW>
	{
		typedef EBI_Register<EBI_BR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;
		static const sc_dt::uint64 ADDRESS_STRIDE = 8;
		
		template <unsigned int N, bool dummy = true>
		struct BA_COMPOUND
		{
			struct BA_UP  : Field<EBI_BR, BA_UP , 0    , N - 1, SW_R_HW_RO> {}; // Base Address (upper bits)
			struct BA_LOW : Field<EBI_BR, BA_LOW, N, 16                   > {}; // Base Address (lower bits)
			struct BA     : FieldSet<BA_UP, BA_LOW>                             // Base Address
			{
				struct _BA : Field<EBI_BR, _BA, 0, 16> {};
				
				static void Init()
				{
					BA_UP ::SetName("BA_UP");  BA_UP::SetDescription("Base Address (upper bits)");
					BA_LOW::SetName("BA_LOW"); BA_LOW::SetDescription("Base Address (lower bits)");
				}
				
				template <typename T> static inline T Get(const T& storage)
				{
					return _BA::template Get<T>(storage);
				}
			};
		};
		
		template <bool dummy>
		struct BA_COMPOUND<0, dummy>
		{
			struct BA  : Field<EBI_BR, BA, 0, 16> // Base Address
			{
				static void Init()
				{
					BA::SetName("BA"); BA::SetDescription("Base Address");
				}
			};
		};
		
		typedef typename BA_COMPOUND<TIED_UPPED_ADDRESS_BITS>::BA BA;
		
		struct LWRN   : Field<EBI_BR, LWRN  , 19                                                        > {}; // Late RD_WR# Negation
		struct PS     : Field<EBI_BR, PS    , 20                                                        > {}; // Port Size
		struct EOE    : Field<EBI_BR, EOE   , 21                     , 22                               > {}; // Early OE#
		struct SBL    : Field<EBI_BR, SBL   , 23                                                        > {}; // Short Burst Length
		struct AD_MUX : Field<EBI_BR, AD_MUX, 24                                                        > {}; // Address on Data Bus Multiplexing
		struct BL     : Field<EBI_BR, BL    , 25                                                        > {}; // Burst Length
		struct WEBS   : Field<EBI_BR, WEBS  , 26                                                        > {}; // Write Enable / Byte Select
		struct TBDIP  : Field<EBI_BR, TBDIP , 27                                                        > {}; // BDIP signal assertion duration for each data beat in a burst cycle
		struct GCSN   : Field<EBI_BR, GCSN  , 28                                                        > {}; // Guarantee CS# Negation
		struct BI     : Field<EBI_BR, BI    , 30                                                        > {}; // Burst Inhibit
		struct V      : Field<EBI_BR, V     , 31                                                        > {}; // Valid bit
		
		typedef FieldSet<BA, LWRN, PS, EOE, SBL, AD_MUX, BL, WEBS, TBDIP, GCSN, BI, V> ALL;     
		
		EBI_BR() : Super(0) {}
		EBI_BR(EBI<CONFIG> *_ebi) : Super(_ebi) {}
		EBI_BR(EBI<CONFIG> *_ebi, uint32_t value) : Super(_ebi, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EBI<CONFIG> *_ebi)
		{
			this->ebi = _ebi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "EBI_BR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Base Register Bank #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			BA    ::Init();
			LWRN  ::SetName("LWRN");   LWRN  ::SetDescription("Late RD_WR# Negation");
			PS    ::SetName("PS");     PS    ::SetDescription("Port Size");
			EOE   ::SetName("EOE");    EOE   ::SetDescription("Early OE#");
			SBL   ::SetName("SBL");    SBL   ::SetDescription("Short Burst Length");
			AD_MUX::SetName("AD_MUX"); AD_MUX::SetDescription("Address on Data Bus Multiplexing");
			BL    ::SetName("BL");     BL    ::SetDescription("Burst Length");
			WEBS  ::SetName("WEBS");   WEBS  ::SetDescription("Write Enable / Byte Select");
			TBDIP ::SetName("TBDIP");  TBDIP ::SetDescription("BDIP signal assertion duration for each data beat in a burst cycle");
			GCSN  ::SetName("GCSN");   GCSN  ::SetDescription("Guarantee CS# Negation");
			BI    ::SetName("BI");     BI    ::SetDescription("Burst Inhibit");
			V     ::SetName("V");      V     ::SetDescription("Valid bit");
		}
		
		void Reset()
		{
			this->Initialize(CONFIG::BR_RESET_VALUE);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying BR may affect memory mapping, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			unsigned int output_port = reg_num;
			this->ebi->DMI_Invalidate(output_port);
			
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->ebi->mapping_cache.Invalidate();
			}
			
			return rws;
		}
		
		virtual void DebugWrite(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying BR may affect memory mapping, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->ebi->DMI_Invalidate();
			this->ebi->mapping_cache.Invalidate();
			Super::DebugWrite(value, bit_enable);
		}
		
	private:
		unsigned int reg_num;
	};
	
	// Option Register Bank (EBI_ORn)
	struct EBI_OR : EBI_Register<EBI_OR, SW_RW>
	{
		typedef EBI_Register<EBI_OR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x14;
		static const sc_dt::uint64 ADDRESS_STRIDE = 8;
		
		template <unsigned int N, bool dummy = true>
		struct AM_COMPOUND
		{
			struct AM_UP  : Field<EBI_OR, AM_UP , 0    , N - 1, SW_R_HW_RO> {}; // Address Mask (upper bits)
			struct AM_LOW : Field<EBI_OR, AM_LOW, N, 16                   > {}; // Address Mask (lower bits)
			struct AM     : FieldSet<AM_UP, AM_LOW>                             // Address Mask
			{
				struct _AM : Field<EBI_OR, _AM, 0, 16> {};
				
				static void Init()
				{
					AM_UP ::SetName("AM_UP");  AM_UP::SetDescription("Address Mask (upper bits)");
					AM_LOW::SetName("AM_LOW"); AM_LOW::SetDescription("Address Mask (lower bits)");
				}
				
				template <typename T> static inline T Get(const T& storage)
				{
					return _AM::template Get<T>(storage);
				}
			}; 
		};
		
		template <bool dummy>
		struct AM_COMPOUND<0, dummy>
		{
			struct AM  : Field<EBI_OR, AM, 0, 16> // Address Mask
			{
				static void Init()
				{
					AM::SetName("AM"); AM::SetDescription("Address Mask");
				}
			};
		};
		
		typedef typename AM_COMPOUND<TIED_UPPED_ADDRESS_BITS>::AM AM;
		
		struct APS  : Field<EBI_OR, APS , 19    > {}; // Address by Port Size
		struct AWE  : Field<EBI_OR, AWE , 21    > {}; // Address / Write Enable Select
		struct SCY  : Field<EBI_OR, SCY , 24, 27> {}; // Cycle length in clocks
		struct BSCY : Field<EBI_OR, BSCY, 29, 30> {}; // Burst beats length in clocks
		
		typedef FieldSet<AM, APS, AWE, SCY, BSCY> ALL;
		
		EBI_OR() : Super(0) {}
		EBI_OR(EBI<CONFIG> *_ebi) : Super(_ebi) {}
		EBI_OR(EBI<CONFIG> *_ebi, uint32_t value) : Super(_ebi, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, EBI<CONFIG> *_ebi)
		{
			this->ebi = _ebi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "EBI_OR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Option Register Bank #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			AM  ::Init();
			APS ::SetName("APS");  APS ::SetDescription("Address by Port Size");
			AWE ::SetName("AWE");  AWE ::SetDescription("Address / Write Enable Select");
			SCY ::SetName("SCY");  SCY ::SetDescription("Cycle length in clocks");
			BSCY::SetName("BSCY"); BSCY::SetDescription("Burst beats length in clocks");
		}
		
		void Reset()
		{
			this->Initialize(CONFIG::OR_RESET_VALUE);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying OR may affect memory mapping, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			unsigned int output_port = reg_num;
			this->ebi->DMI_Invalidate(output_port);
			
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->ebi->mapping_cache.Invalidate();
			}
			
			return rws;
		}
		
		virtual void DebugWrite(const uint32_t& value, const uint32_t& bit_enable)
		{
			// modifying OR may affect memory mapping, so it's safer to invalidate SystemC TLM-2.0 direct memory pointers before modification
			this->ebi->DMI_Invalidate();
			this->ebi->mapping_cache.Invalidate();
			Super::DebugWrite(value, bit_enable);
		}
	private:
		unsigned int reg_num;
	};

	EBI_MCR                                                               ebi_mcr; // EBI_MCR
	AddressableRegisterFile<EBI_BR, CONFIG::OUTPUT_SOCKETS, EBI<CONFIG> > ebi_br;  // EBI_BR
	AddressableRegisterFile<EBI_OR, CONFIG::OUTPUT_SOCKETS, EBI<CONFIG> > ebi_or;  // EBI_OR
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	
	MappingCache<CONFIG> mapping_cache;
	TemporaryMappingKeeper<CONFIG> temporary_mapping_keeper;
	
	bool Match(unsigned int output_port, uint64_t addr, uint64_t& range_start, uint64_t& range_end, uint64_t& translation) const;
	unsigned int OutputAddressWidth(unsigned int output_port) const;
	uint64_t OutputAddressMask(unsigned int output_port) const;
};

} // end of namespace ebi
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interconnect
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERCONNECT_FREESCALE_MPC57XX_EBI_EBI_HH__
