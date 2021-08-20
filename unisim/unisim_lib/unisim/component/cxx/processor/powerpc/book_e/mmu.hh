/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_MMU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_MMU_HH__

#include <inttypes.h>
#include <iostream>

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace book_e {

template <typename TYPES, typename CONFIG>
struct TLB;

template <typename TYPES>
struct TLB_ENTRY
{
	// Constructor and initialization
	TLB_ENTRY() : next(0), w(), tid(0) {}
	void Reset() { w[0] = 0; w[1] = 0; w[2] = 0; tid = 0; }
	void Initialize(uint32_t epn, uint32_t v, uint32_t ts, uint32_t size, uint8_t tid, uint32_t rpn, uint32_t erpn, typename TYPES::STORAGE_ATTR storage_attr, typename TYPES::ACCESS_CTRL);
	void Initialize(const TLB_ENTRY& o) { w[0] = o.w[0]; w[1] = o.w[1]; w[2] = o.w[2]; tid = o.tid; }
	
	// Getters
	uint32_t GetWord0(bool parity_enable) const { return parity_enable ? W0::template Get<uint32_t>(w[0]) : W0_NO_PARITY::template Get<uint32_t>(w[0]); }
	uint32_t GetWord1(bool parity_enable) const { return parity_enable ? W1::template Get<uint32_t>(w[1]) : W1_NO_PARITY::template Get<uint32_t>(w[1]); }
	uint32_t GetWord2(bool parity_enable) const { return parity_enable ? W2::template Get<uint32_t>(w[2]) : W2_NO_PARITY::template Get<uint32_t>(w[2]); }
	uint32_t GetV() const { return V::template Get<uint32_t>(w[0]); }
	uint32_t GetSIZE() const { return SIZE::template Get<uint32_t>(w[0]); }
	typename TYPES::ADDRESS_SPACE GetTS() const { return typename TYPES::ADDRESS_SPACE(TS::template Get<uint32_t>(w[0])); }
	uint32_t GetEPN() const { return EPN::template Get<uint32_t>(w[0]); }
	uint32_t GetERPN() const { return ERPN::template Get<uint32_t>(w[1]); }
	uint32_t GetRPN() const { return RPN::template Get<uint32_t>(w[1]); }
	typename TYPES::PROCESS_ID GetTID() const { return typename TYPES::PROCESS_ID(tid); }
	typename TYPES::STORAGE_ATTR GetStorageAttr() const { return typename TYPES::STORAGE_ATTR(STORAGE_ATTR::template Get<uint32_t>(w[2])); }
	typename TYPES::ACCESS_CTRL GetAccessCtrl() const { return typename TYPES::ACCESS_CTRL(ACCESS_CTRL::template Get<uint32_t>(w[2])); }
	bool IsValid() const { return GetV(); }
	uint32_t GetPageSize() const { return 1024 << (2 * GetSIZE()); }
	typename TYPES::EFFECTIVE_ADDRESS GetPageEffectiveAddress() const { return (GetEPN() << (32 - EPN::BITWIDTH)) & ~(GetPageSize() - 1); }
	typename TYPES::ADDRESS GetPageVirtualAddress() const { return ((typename TYPES::ADDRESS) GetTS() << 40) | ((typename TYPES::ADDRESS) GetTID() << 32) | GetPageEffectiveAddress(); }
	typename TYPES::PHYSICAL_ADDRESS GetPagePhysicalAddress() const { return (((typename TYPES::PHYSICAL_ADDRESS) GetERPN()) << 32) | (((typename TYPES::PHYSICAL_ADDRESS) GetRPN()) << (32 - RPN::BITWIDTH)); }
	
	// Setters
	void SetWord0(uint32_t v) { W0_NO_PARITY::template Set<uint32_t>(w[0], v); }
	void SetWord1(uint32_t v) { W1_NO_PARITY::template Set<uint32_t>(w[1], v); }
	void SetWord2(uint32_t v) { W2_NO_PARITY::template Set<uint32_t>(w[2], v); }
	void SetTID(uint8_t v) { tid = v; }
	
	// Match
	bool Match(typename TYPES::ADDRESS_SPACE as, typename TYPES::PROCESS_ID pid, typename TYPES::EFFECTIVE_ADDRESS ea, typename TYPES::EFFECTIVE_ADDRESS& size_to_protection_boundary, typename TYPES::ADDRESS& virt_addr, typename TYPES::PHYSICAL_ADDRESS& phys_addr) const;
	
	// Control access
	bool ControlAccess(typename TYPES::ACCESS_CTRL access_ctrl) const { return access_ctrl & GetAccessCtrl(); }
	
	// Printer
	void Print(std::ostream& os) const;

private:
	template <typename _TYPES, typename CONFIG>
	friend struct TLB;
	
	// word 0
	struct EPN  : Field<EPN , 0 , 21> {}; // Effective Page Number
	struct V    : Field<V   , 22    > {}; // Valid
	struct TS   : Field<TS  , 23    > {}; // Translation Address Space
	struct SIZE : Field<SIZE, 24, 27> {}; // Page Size
	struct TPAR : Field<TPAR, 28, 31> {}; // Tag Parity
	
	typedef FieldSet<EPN, V, TS, SIZE> W0_NO_PARITY;
	typedef FieldSet<EPN, V, TS, SIZE, TPAR> W0;
	
	// word 1
	struct RPN  : Field<RPN , 0 , 21> {}; // Real Page Number
	struct PAR1 : Field<PAR1, 22, 23> {}; // Parity for TLB word 1
	struct ERPN : Field<ERPN, 28, 31> {}; // Extended Real Page Number
	
	typedef FieldSet<RPN, ERPN> W1_NO_PARITY;
	typedef FieldSet<RPN, PAR1, ERPN> W1;

	// word 2
	struct PAR2 : Field<PAR2, 0 , 1 > {}; // Parity for TLB word 2
	struct U0   : Field<U0  , 16    > {}; // User-Definable Storage Attribute 0
	struct U1   : Field<U1  , 17    > {}; // User-Definable Storage Attribute 1
	struct U2   : Field<U2  , 18    > {}; // User-Definable Storage Attribute 2
	struct U3   : Field<U3  , 19    > {}; // User-Definable Storage Attribute 3
	struct W    : Field<W   , 20    > {}; // Write-Through
	struct I    : Field<I   , 21    > {}; // Cache Inhibited
	struct M    : Field<M   , 22    > {}; // Memory Coherency Enforced
	struct G    : Field<G   , 23    > {}; // Guarded
	struct E    : Field<E   , 24    > {}; // Endian
	struct UX   : Field<UX  , 26    > {}; // User State Execute Enable
	struct UW   : Field<UW  , 27    > {}; // User State Write Enable
	struct UR   : Field<UR  , 28    > {}; // User State Read Enable
	struct SX   : Field<SX  , 29    > {}; // Supervisor State Execute Enable
	struct SW   : Field<SW  , 30    > {}; // Supervisor State Write Enable
	struct SR   : Field<SR  , 31    > {}; // Supervisor State Read Enable

	typedef FieldSet<U0, U1, U2, U3, W, I, M, G, E, UX, UW, UR, SX, SW, SR> W2_NO_PARITY;
	typedef FieldSet<PAR2, U0, U1, U2, U3, W, I, M, G, E, UX, UW, UR, SX, SW, SR> W2;
	
	
	struct STORAGE_ATTR : Field<STORAGE_ATTR, 16, 24> {};
	struct ACCESS_CTRL  : Field<ACCESS_CTRL , 26, 31> {};
	
	TLB_ENTRY<TYPES> *next;
	uint32_t w[3];
	uint8_t tid;
	
	TLB_ENTRY(const TLB_ENTRY&) {}
	TLB_ENTRY& operator = (const TLB_ENTRY&) { return *this; }

};

template <typename TYPES>
std::ostream& operator << (std::ostream& os, const TLB_ENTRY<TYPES>& tlb_entry)
{
	tlb_entry.Print(os);
	return os;
}

template <typename TYPES>
void TLB_ENTRY<TYPES>::Initialize(uint32_t epn, uint32_t v, uint32_t ts, uint32_t size, uint8_t _tid, uint32_t rpn, uint32_t erpn, typename TYPES::STORAGE_ATTR storage_attr, typename TYPES::ACCESS_CTRL access_ctrl)
{
	EPN::template Set<uint32_t>(w[0], epn);
	V::template Set<uint32_t>(w[0], v);
	TS::template Set<uint32_t>(w[0], ts);
	SIZE::template Set<uint32_t>(w[0], size);
	tid = _tid;
	RPN::template Set<uint32_t>(w[1], rpn);
	ERPN::template Set<uint32_t>(w[1], erpn);
	STORAGE_ATTR::template Set<uint32_t>(w[2], storage_attr);
	ACCESS_CTRL::template Set<uint32_t>(w[2], access_ctrl);
}

template <typename TYPES>
bool TLB_ENTRY<TYPES>::Match(typename TYPES::ADDRESS_SPACE as, typename TYPES::PROCESS_ID pid, typename TYPES::EFFECTIVE_ADDRESS ea, typename TYPES::EFFECTIVE_ADDRESS& size_to_protection_boundary, typename TYPES::ADDRESS& virt_addr, typename TYPES::PHYSICAL_ADDRESS& phys_addr) const
{
	bool valid = V::template Get<uint32_t>(w[0]);
	if(valid)
	{
		typename TYPES::ADDRESS_SPACE ts = typename TYPES::ADDRESS_SPACE(TS::template Get<uint32_t>(w[0]));
		if(ts == as)
		{
			if(!tid || (tid == pid))
			{
				uint32_t page_size = GetPageSize();
				typename TYPES::EFFECTIVE_ADDRESS lsb_mask = page_size - 1;
				typename TYPES::EFFECTIVE_ADDRESS msb_mask = ~lsb_mask;
				typename TYPES::EFFECTIVE_ADDRESS epn = EPN::template Get<uint32_t>(w[0]) << (32 - EPN::BITWIDTH);
				bool match = (epn & msb_mask) == (ea & msb_mask);
				
				if(match)
				{
					typename TYPES::PHYSICAL_ADDRESS erpn = (typename TYPES::PHYSICAL_ADDRESS) ERPN::template Get<uint32_t>(w[1]) << 32;
					typename TYPES::PHYSICAL_ADDRESS rpn = (typename TYPES::PHYSICAL_ADDRESS) RPN::template Get<uint32_t>(w[1]) << (32 - RPN::BITWIDTH);
					size_to_protection_boundary = page_size - (ea & lsb_mask);
					virt_addr = ((typename TYPES::ADDRESS) as << 40) | ((typename TYPES::ADDRESS) pid << 32) | (epn & msb_mask) | (ea & lsb_mask);
					phys_addr = erpn | (rpn & msb_mask) | (ea & lsb_mask);
					return true;
				}
			}
		}
	}
	
	return false;
}

template <typename TYPES>
void TLB_ENTRY<TYPES>::Print(std::ostream& os) const
{
	std::ostream::char_type fill(os.fill());
	std::ios_base::fmtflags flags(os.flags());
	os.fill('0');
	
	os << "V:" << +GetV();
	os << std::hex;
	
	// virtual address
	os << " Virtual Address:0x";
	os.width(1);
	os << +GetTS();
	if(tid)
	{
		os.width(2);
		os << +GetTID();
	}
	else
	{
		os << "??";
	}
	
	os.width(8);
	os << GetPageEffectiveAddress();
	
	os << "-0x";
	os.width(1);
	os << +GetTS();
	if(tid)
	{
		os.width(2);
		os << +GetTID();
	}
	else
	{
		os << "??";
	}
	os.width(8);
	os << (GetPageEffectiveAddress() + GetPageSize() - 1);
	
	// physical address
	os << " Physical Address:0x";
	os.width(9);
	os << GetPagePhysicalAddress();
	os << "-0x";
	os.width(9);
	os << (GetPagePhysicalAddress() + GetPageSize() - 1);

	// access control
	typename TYPES::ACCESS_CTRL access_ctrl = GetAccessCtrl();
	os << " Super:";
	os << ((access_ctrl & TYPES::AC_SR) ? 'r' : '-');
	os << ((access_ctrl & TYPES::AC_SW) ? 'w' : '-');
	os << ((access_ctrl & TYPES::AC_SX) ? 'x' : '-');
	os << " User:";
	os << ((access_ctrl & TYPES::AC_UR) ? 'r' : '-');
	os << ((access_ctrl & TYPES::AC_UW) ? 'w' : '-');
	os << ((access_ctrl & TYPES::AC_UX) ? 'x' : '-');
	
	// storage attribute
	typename TYPES::STORAGE_ATTR storage_attr = GetStorageAttr();
	os << " U0:" << ((storage_attr & TYPES::SA_U0) ? '1' : '0') << ' ';
	os << " U1:" << ((storage_attr & TYPES::SA_U1) ? '1' : '0') << ' ';
	os << " U2:" << ((storage_attr & TYPES::SA_U2) ? '1' : '0') << ' ';
	os << " U3:" << ((storage_attr & TYPES::SA_U3) ? '1' : '0') << ' ';
	os << " W:" << ((storage_attr & TYPES::SA_W) ? '1' : '0') << ' ';
	os << " I:" << ((storage_attr & TYPES::SA_I) ? '1' : '0') << ' ';
	os << " M:" << ((storage_attr & TYPES::SA_M) ? '1' : '0') << ' ';
	os << " G:" << ((storage_attr & TYPES::SA_G) ? '1' : '0') << ' ';
	os << " E:" << ((storage_attr & TYPES::SA_E) ? '1' : '0') << ' ';

	os.fill(fill);
	os.flags(flags);
}

template <typename TYPES, typename CONFIG>
struct TLB
	: unisim::kernel::Service<unisim::service::interfaces::HttpServer>
{
	enum PrintFormat
	{
		PFMT_TXT,
		PFMT_HTML
	};
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
	TLB(const char *name, unisim::kernel::Object *parent = 0, const char *description = 0);
	void Reset();
	TLB_ENTRY<TYPES> *Lookup(typename TYPES::ADDRESS_SPACE as, typename TYPES::PROCESS_ID pid, typename TYPES::EFFECTIVE_ADDRESS ea, typename TYPES::EFFECTIVE_ADDRESS& size_to_protection_boundary, typename TYPES::ADDRESS& virt_addr, typename TYPES::PHYSICAL_ADDRESS& phys_addr);
	void Replace(TLB_ENTRY<TYPES> *tlb_entry) { entries[victim_way].Initialize(*tlb_entry); victim_way = (victim_way + 1) % CONFIG::ASSOCIATIVITY; }
	unsigned int GetWay(const TLB_ENTRY<TYPES> *tlb_entry) const { return tlb_entry - &entries[0]; /*(tlb_entry - &entries[0]) / sizeof(TLB_ENTRY<TYPES>);*/ }
	TLB_ENTRY<TYPES>& LookupByWay(unsigned int way);
	void Print(std::ostream& os, unsigned int way, PrintFormat pfmt) const;
	void Print(std::ostream& os, PrintFormat pfmt) const;
	
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
private:
	unisim::kernel::logger::Logger logger;
	TLB_ENTRY<TYPES> entries[CONFIG::ASSOCIATIVITY];
	TLB_ENTRY<TYPES> *mrs_entry;
	unsigned int victim_way;
	bool verbose;
	uint64_t num_accesses;
	uint64_t num_misses;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_misses;
	unisim::kernel::variable::StatisticFormula<double> stat_miss_rate;
};

template <typename TYPES, typename CONFIG>
std::ostream& operator << (std::ostream& os, const TLB<TYPES, CONFIG>& tlb)
{
	tlb.Print(os, TLB<TYPES, CONFIG>::PFMT_TXT);
	return os;
}

template <typename TYPES, typename CONFIG>
TLB<TYPES, CONFIG>::TLB(const char *name, unisim::kernel::Object *parent, const char *description)
	: unisim::kernel::Object(name, parent, description)
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>(name, parent, description)
	, http_server_export("http-server-export", this)
	, logger(*this)
	, entries()
	, mrs_entry(0)
	, victim_way(0)
	, verbose(false)
	, num_accesses(0)
	, num_misses(0)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, stat_num_accesses("num-accesses", this, num_accesses, "Number of accesses")
	, stat_num_misses("num-misses", this, num_misses, "Number of misses")
	, stat_miss_rate("miss-rate", this, "/", &stat_num_misses, &stat_num_accesses, "TLB miss rate")
{
	Reset();
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::Reset()
{
	victim_way = 0;
	for(unsigned int way = 0; way < CONFIG::ASSOCIATIVITY; way++)
	{
		TLB_ENTRY<TYPES>& tlb_entry = entries[way];
		
		tlb_entry.Reset();
		tlb_entry.next = (way < (CONFIG::ASSOCIATIVITY - 1)) ? &entries[way + 1] : 0;
	}
	mrs_entry = &entries[0];
}

template <typename TYPES, typename CONFIG>
TLB_ENTRY<TYPES> *TLB<TYPES, CONFIG>::Lookup(typename TYPES::ADDRESS_SPACE as, typename TYPES::PROCESS_ID pid, typename TYPES::EFFECTIVE_ADDRESS ea, typename TYPES::EFFECTIVE_ADDRESS& size_to_protection_boundary, typename TYPES::ADDRESS& virt_addr, typename TYPES::PHYSICAL_ADDRESS& phys_addr)
{
	num_accesses++;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Lookup at @(as=0x" << std::hex << +as << ",pid=0x" << +pid << "ea=0x" << ea << std::dec << ")" << EndDebugInfo;
	}
	for(TLB_ENTRY<TYPES> *tlb_entry = mrs_entry, *prev_tlb_entry = 0; tlb_entry; prev_tlb_entry = tlb_entry, tlb_entry = tlb_entry->next)
	{
		if(tlb_entry->Match(as, pid, ea, size_to_protection_boundary, virt_addr, phys_addr))
		{
			// TLB hit
			if(unlikely(verbose))
			{
				logger << DebugInfo << "TLB Hit" << EndDebugInfo;
			}
			if(unlikely(prev_tlb_entry))
			{
				prev_tlb_entry->next = tlb_entry->next;
				tlb_entry->next = mrs_entry;
				mrs_entry = tlb_entry;
			}
			
			return tlb_entry;
		}
	}

	if(unlikely(verbose))
	{
		logger << DebugInfo << "TLB Miss" << EndDebugInfo;
	}
	num_misses++;
	return 0;
}

template <typename TYPES, typename CONFIG>
TLB_ENTRY<TYPES>& TLB<TYPES, CONFIG>::LookupByWay(unsigned int way)
{
	return entries[way % CONFIG::ASSOCIATIVITY];
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::Print(std::ostream& os, unsigned int way, PrintFormat pfmt) const
{
	const TLB_ENTRY<TYPES>& tlb_entry = entries[way];
	
	std::ostream::char_type fill(os.fill());
	std::ios_base::fmtflags flags(os.flags());
	os.fill('0');

	switch(pfmt)
	{
		case PFMT_TXT:
		{
			os << "Way:" << std::dec << way << " ";
			tlb_entry.Print(os);
			os << std::endl;
			break;
		}
		
		case PFMT_HTML:
		{
			os << "<tr class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << "\">";
			
			os << "<td><span>" << std::dec << way << "</span></td>";
			os << "<td><span>" << +tlb_entry.GetV() << "</span></td>";
			
			os << std::hex;
			
			// virtual address
			os << "<td class=\"vaddr-range\"><span>0x";
			os.width(1);
			os << +tlb_entry.GetTS();
			if(tlb_entry.GetTID())
			{
				os.width(2);
				os << +tlb_entry.GetTID();
			}
			else
			{
				os << "??";
			}
			
			os.width(8);
			os << tlb_entry.GetPageEffectiveAddress();
			
			os << "-0x";
			os.width(1);
			os << +tlb_entry.GetTS();
			if(tlb_entry.GetTID())
			{
				os.width(2);
				os << +tlb_entry.GetTID();
			}
			else
			{
				os << "??";
			}
			os.width(8);
			os << (tlb_entry.GetPageEffectiveAddress() + tlb_entry.GetPageSize() - 1);
			os << "</span></td>";
			
			// physical address
			os << "<td class=\"paddr-range\"><span>0x";
			os.width(9);
			os << tlb_entry.GetPagePhysicalAddress();
			os << "-0x";
			os.width(9);
			os << (tlb_entry.GetPagePhysicalAddress() + tlb_entry.GetPageSize() - 1);
			os << "</span></td>";

			// access control
			typename TYPES::ACCESS_CTRL access_ctrl = tlb_entry.GetAccessCtrl();
			os << "<td class=\"supervisor-access-ctrl\"><span>";
			os << ((access_ctrl & TYPES::AC_SR) ? 'r' : '-');
			os << ((access_ctrl & TYPES::AC_SW) ? 'w' : '-');
			os << ((access_ctrl & TYPES::AC_SX) ? 'x' : '-');
			os << "</span></td>";
			os << "<td class=\"user-access-ctrl\"><span>";
			os << ((access_ctrl & TYPES::AC_UR) ? 'r' : '-');
			os << ((access_ctrl & TYPES::AC_UW) ? 'w' : '-');
			os << ((access_ctrl & TYPES::AC_UX) ? 'x' : '-');
			os << "</span></td>";
			
			// storage attribute
			typename TYPES::STORAGE_ATTR storage_attr = tlb_entry.GetStorageAttr();
			os << "<td class=\"U0\"><span>";
			os << ((storage_attr & TYPES::SA_U0) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"U1\"><span>";
			os << ((storage_attr & TYPES::SA_U1) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"U2\"><span>";
			os << ((storage_attr & TYPES::SA_U2) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"U3\"><span>";
			os << ((storage_attr & TYPES::SA_U3) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"W\"><span>";
			os << ((storage_attr & TYPES::SA_W) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"I\"><span>";
			os << ((storage_attr & TYPES::SA_I) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"M\"><span>";
			os << ((storage_attr & TYPES::SA_M) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"G\"><span>";
			os << ((storage_attr & TYPES::SA_G) ? '1' : '0');
			os << "</span></td>";
			os << "<td class=\"E\"><span>";
			os << ((storage_attr & TYPES::SA_E) ? '1' : '0');
			os << "</span></td>";
			
			os << "</tr>" << std::endl;
			break;
		}
	}
	
	os.fill(fill);
	os.flags(flags);
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::Print(std::ostream& os, PrintFormat pfmt) const
{
	if(pfmt == PFMT_HTML)
	{
		os << "<table>" << std::endl;
		os << "\t<thead>" << std::endl;
		os << "\t\t<tr>";
		os << "<th><span>Way</span></th>";
		os << "<th><span>V</span></th>";
		os << "<th><span>Virtual Address</span></th>";
		os << "<th><span>Physical Address</span></th>";
		os << "<th><span>Super</span></th>";
		os << "<th><span>User</span></th>";
		os << "<th><span>U0</span></th>";
		os << "<th><span>U1</span></th>";
		os << "<th><span>U2</span></th>";
		os << "<th><span>U3</span></th>";
		os << "<th><span>W</span></th>";
		os << "<th><span>I</span></th>";
		os << "<th><span>M</span></th>";
		os << "<th><span>G</span></th>";
		os << "<th><span>E</span></th>";
		os << "</tr>" << std::endl;
		os << "\t</thead>" << std::endl;
		os << "\t<tbody>" << std::endl;
	}
	for(unsigned int tlb_way = 0; tlb_way < CONFIG::ASSOCIATIVITY; tlb_way++)
	{
		Print(os, tlb_way, pfmt);
	}
	if(pfmt == PFMT_HTML)
	{
		os << "\t</tbody>" << std::endl;
		os << "</table>" << std::endl;
	}
}

template <typename TYPES, typename CONFIG>
bool TLB<TYPES, CONFIG>::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
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
				response << "\t\t<meta name=\"description\" content=\"user interface for MMU\">" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/component/cxx/processor/powerpc/book_e/mmu_style.css\" type=\"text/css\" />" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				//response << "\t\t<script src=\"/unisim/component/cxx/processor/powerpc/book_e/mmu_script.js\"></script>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				Print(response, PFMT_HTML);
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
void TLB<TYPES, CONFIG>::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
}

template <typename TYPES, typename CONFIG>
struct MMU
	: unisim::kernel::Object
{
	typedef typename CONFIG::CPU CPU;
	typedef TLB<TYPES, typename CONFIG::ITLB_CONFIG> ITLB;
	typedef TLB<TYPES, typename CONFIG::DTLB_CONFIG> DTLB;
	typedef TLB<TYPES, typename CONFIG::UTLB_CONFIG> UTLB;
	typedef typename TYPES::ADDRESS_SPACE ADDRESS_SPACE;
	typedef typename TYPES::PROCESS_ID PROCESS_ID;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	typedef typename TYPES::ACCESS_CTRL ACCESS_CTRL;
	typedef typename CPU::MSR MSR;
	typedef typename CPU::PID0 PID0;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> itlb_http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> dtlb_http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> utlb_http_server_export;

	MMU(CPU *cpu);
	void Reset();
	void InvalidateShadowTLBs();
	template <bool DEBUG> TLB_ENTRY<TYPES> *Lookup(ADDRESS_SPACE as, PROCESS_ID pid, EFFECTIVE_ADDRESS ea);
	template <bool DEBUG, bool EXEC, bool WRITE> inline bool Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr) ALWAYS_INLINE;
	unsigned int GetWay(const TLB_ENTRY<TYPES> *tlb_entry) const { return utlb.GetWay(tlb_entry); }
	TLB_ENTRY<TYPES>& LookupByWay(unsigned int way);
	void Print(std::ostream& os) const;
protected:
private:
	CPU *cpu;
	ITLB itlb;
	DTLB dtlb;
	UTLB utlb;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
};

template <typename TYPES, typename CONFIG>
std::ostream& operator << (std::ostream& os, const MMU<TYPES, CONFIG>& mmu)
{
	mmu.Print(os);
	return os;
}

template <typename TYPES, typename CONFIG>
MMU<TYPES, CONFIG>::MMU(CPU *_cpu)
	: unisim::kernel::Object("MMU", _cpu, "Memory Management Unit")
	, itlb_http_server_export("itlb-http-server-export", this)
	, dtlb_http_server_export("dtlb-http-server-export", this)
	, utlb_http_server_export("utlb-http-server-export", this)
	, cpu(_cpu)
	, itlb("ITLB", this, "Shadow Instruction TLB")
	, dtlb("DTLB", this, "Shadow Data TLB")
	, utlb("UTLB", this, "Unified TLB")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	itlb_http_server_export >> itlb.http_server_export;
	dtlb_http_server_export >> dtlb.http_server_export;
	utlb_http_server_export >> utlb.http_server_export;
	
	Reset();
}

template <typename TYPES, typename CONFIG>
void MMU<TYPES, CONFIG>::Reset()
{
	InvalidateShadowTLBs();
	
	typename CPU::RSTCFG& rstcfg = cpu->GetRSTCFG();
	uint32_t erpn = rstcfg.template Get<typename CPU::RSTCFG::ERPN>();
	unsigned int ts = TYPES::AS_SYSTEM;
	unsigned int rstcfg_u0 = rstcfg.template Get<typename CPU::RSTCFG::U0>();
	unsigned int rstcfg_u1 = rstcfg.template Get<typename CPU::RSTCFG::U1>();
	unsigned int rstcfg_u2 = rstcfg.template Get<typename CPU::RSTCFG::U2>();
	unsigned int rstcfg_u3 = rstcfg.template Get<typename CPU::RSTCFG::U3>();
	unsigned int rstcfg_e = rstcfg.template Get<typename CPU::RSTCFG::E>();
	STORAGE_ATTR storage_attr = STORAGE_ATTR(TYPES::SA_I | TYPES::SA_G | (rstcfg_u0 ? TYPES::SA_U0 : 0) | (rstcfg_u1 ? TYPES::SA_U1 : 0) | (rstcfg_u2 ? TYPES::SA_U2 : 0) | (rstcfg_u3 ? TYPES::SA_U3 : 0) | (rstcfg_e ? TYPES::SA_E : 0));
	ACCESS_CTRL access_ctrl = ACCESS_CTRL(TYPES::AC_SX | TYPES::AC_SR);
	
	TLB_ENTRY<TYPES>& auto_config_itlb_entry = itlb.LookupByWay(0);
	auto_config_itlb_entry.Initialize(
		/* EPN */ 0x3ffffc,
		/*  V  */ 1,
		/* TS  */ ts,
		/* SIZE */ 1, // 4 KB page
		/* TID */ 0,
		/* RPN */ 0x3ffffc,
		/* ERPN */ erpn,
		/* STORAGE ATTR */ storage_attr,
		/* ACCESS CTRL */ access_ctrl
	);
	
	TLB_ENTRY<TYPES>& auto_config_dtlb_entry = dtlb.LookupByWay(0);
	auto_config_dtlb_entry.Initialize(
		/* EPN */ 0x3ffffc,
		/*  V  */ 1,
		/* TS  */ ts,
		/* SIZE */ 1, // 4 KB page
		/* TID */ 0,
		/* RPN */ 0x3ffffc,
		/* ERPN */ erpn,
		/* STORAGE ATTR */ storage_attr,
		/* ACCESS CTRL */ access_ctrl
	);
}

template <typename TYPES, typename CONFIG>
void MMU<TYPES, CONFIG>::InvalidateShadowTLBs()
{
	itlb.Reset();
	dtlb.Reset();
}

template <typename TYPES, typename CONFIG>
template <bool DEBUG>
TLB_ENTRY<TYPES> *MMU<TYPES, CONFIG>::Lookup(ADDRESS_SPACE as, PROCESS_ID pid, EFFECTIVE_ADDRESS ea)
{
	EFFECTIVE_ADDRESS size_to_protection_boundary;
	ADDRESS virt_addr;
	PHYSICAL_ADDRESS phys_addr;
	return utlb.Lookup(as, pid, ea, size_to_protection_boundary, virt_addr, phys_addr);
}

template <typename TYPES, typename CONFIG>
template <bool DEBUG, bool EXEC, bool WRITE>
inline bool MMU<TYPES, CONFIG>::Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr)
{
	const MSR& msr = cpu->GetMSR();
	ADDRESS_SPACE as = ADDRESS_SPACE(EXEC ? msr.template Get<typename MSR::IS>()
	                                      : msr.template Get<typename MSR::DS>());
	
	PROCESS_ID pid = PROCESS_ID(cpu->GetPID().template Get<typename PID0::Process_ID>());
	
	// Page Table Lookup
	TLB_ENTRY<TYPES> *tlb_entry = EXEC ? itlb.Lookup(as, pid, ea, size_to_protection_boundary, virt_addr, phys_addr) : dtlb.Lookup(as, pid, ea, size_to_protection_boundary, virt_addr, phys_addr);
	
	if(unlikely(!tlb_entry))
	{
		tlb_entry = utlb.Lookup(as, pid, ea, size_to_protection_boundary, virt_addr, phys_addr);
		
		if(likely(tlb_entry))
		{
			if(EXEC)
			{
				itlb.Replace(tlb_entry);
			}
			else
			{
				dtlb.Replace(tlb_entry);
			}
		}
	}
	
	if(unlikely(!tlb_entry))
	{
		if(!DEBUG)
		{
			if(EXEC)
			{
				cpu->template ThrowException<typename CPU::InstructionTLBErrorInterrupt::InstructionTLBError>().SetAddress(ea);
			}
			else
			{
				cpu->template ThrowException<typename CPU::DataTLBErrorInterrupt::DataTLBError>().SetAddress(ea).SetWrite(WRITE);
			}
		}
		return false;
	}
	
	if(!DEBUG)
	{
		// Access Control
		bool user = msr.template Get<typename MSR::PR>();
		ACCESS_CTRL access_ctrl = user ? (EXEC ? TYPES::AC_UX
		                                       : (WRITE ? TYPES::AC_UW
		                                                : TYPES::AC_UR))
		                               : (EXEC ? TYPES::AC_SX
		                                       : (WRITE ? TYPES::AC_SW
		                                                : TYPES::AC_SR));
		
		if(unlikely(!tlb_entry->ControlAccess(access_ctrl)))
		{
			if(EXEC)
			{
// 				Print(std::cerr);
// 				std::cerr << "Hit:way #" << tlb_entry->way << ":" << *tlb_entry << std::endl;
				cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::AccessControl>();
			}
			else
			{
				cpu->template ThrowException<typename CPU::DataStorageInterrupt::AccessControl>().SetAddress(ea).SetWrite(WRITE);
			}
			return false;
		}
	}

	storage_attr = tlb_entry->GetStorageAttr();
	
	return true;
}

template <typename TYPES, typename CONFIG>
TLB_ENTRY<TYPES>& MMU<TYPES, CONFIG>::LookupByWay(unsigned int way)
{
	return utlb.LookupByWay(way);
}

template <typename TYPES, typename CONFIG>
void MMU<TYPES, CONFIG>::Print(std::ostream& os) const
{
	os << "ITLB:" << std::endl << itlb << std::endl;
	os << "DTLB:" << std::endl << dtlb << std::endl;
	os << "UTLB:" << std::endl << utlb << std::endl;
}

} // end of namespace book_e
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_BOOK_E_MMU_HH__
