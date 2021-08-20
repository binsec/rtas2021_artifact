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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_MMU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_MMU_HH__

#include <inttypes.h>
#include <iostream>

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/http_server.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e600 {

template <typename TYPES>
struct PTE
{
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::VIRTUAL_SEGMENT_ID VIRTUAL_SEGMENT_ID;
	typedef typename TYPES::PAGE_INDEX PAGE_INDEX;
	typedef typename TYPES::ABBREVIATED_PAGE_INDEX ABBREVIATED_PAGE_INDEX;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	struct V    : Field64<V   , 0     > {}; // Entry valid (V = 1) or invalid (V = 0)
	struct VSID : Field64<VSID, 1 , 24> {}; // Virtual segment ID
	struct H    : Field64<H   , 25    > {}; // Hash function identifier
	struct API  : Field64<API , 26, 31> {}; // Abbreviated page index
	struct RPN  : Field64<RPN , 32, 51> {}; // Physical page number
	struct XPN  : Field64<XPN , 52, 54> {}; // Extended page number provides physical address bits 0-2
	struct R    : Field64<R   , 55    > {}; // Referenced bit
	struct C    : Field64<C   , 56    > {}; // Changed bit
	struct WIMG : Field64<WIMG, 57, 60> {}; // Memory/cache control bits
	struct W    : Field64<WIMG, 57    > {}; // Write-Through
	struct I    : Field64<WIMG, 58    > {}; // Caching-Inhibited
	struct M    : Field64<WIMG, 59    > {}; // Memory coherence
	struct G    : Field64<WIMG, 60    > {}; // Guarded
	struct X    : Field64<X   , 61    > {}; // Extended page number provides physical address bit 3
	struct PP   : Field64<PP  , 62, 63> {}; // Page protection bits
	
	typedef uint64_t TYPE;
	
	PTE() : value(0) {}
	PTE(const PTE<TYPES>& o) : value(o.value) {}
	PTE<TYPES>& operator = (const PTE<TYPES>& o) { value = o.value; return *this; }
	TYPE GetV() const { return V::template Get<TYPE>(value); }
	TYPE GetVSID() const { return VSID::template Get<TYPE>(value); }
	TYPE GetH() const { return H::template Get<TYPE>(value); }
	TYPE GetAPI() const { return API::template Get<TYPE>(value); }
	TYPE GetXPN() const { return XPN::template Get<TYPE>(value); }
	TYPE GetX() const { return X::template Get<TYPE>(value); }
	TYPE GetRPN() const { return RPN::template Get<TYPE>(value); }
	TYPE GetR() const { return R::template Get<TYPE>(value); }
	TYPE GetC() const { return C::template Get<TYPE>(value); }
	TYPE GetW() const { return W::template Get<TYPE>(value); }
	TYPE GetI() const { return I::template Get<TYPE>(value); }
	TYPE GetM() const { return M::template Get<TYPE>(value); }
	TYPE GetG() const { return G::template Get<TYPE>(value); }
	TYPE GetPP() const { return PP::template Get<TYPE>(value); }
	bool IsValid() const { return GetV(); }
	ADDRESS GetPageVirtualAddress(PAGE_INDEX page_index) const { return (ADDRESS(GetVSID()) << 28) | (ADDRESS(page_index) << 12); }
	PHYSICAL_ADDRESS GetPagePhysicalAddress(bool xaen) const { return xaen ? ((PHYSICAL_ADDRESS(GetXPN()) << 33) | (PHYSICAL_ADDRESS(GetX()) << 32) | (PHYSICAL_ADDRESS(GetRPN()) << 12)) : (PHYSICAL_ADDRESS(GetRPN()) << 12); }
	STORAGE_ATTR GetStorageAttr() const { return STORAGE_ATTR(WIMG::template Get<TYPE>(value)); }
	static uint32_t GetPageSize() { return 4096; }
	void Access() { R::template Set<TYPE>(value, 1); }
	void Change() { C::template Set<TYPE>(value, 1); }
	void Reset() { value = 0; }
	void BigEndian2Host() { value = unisim::util::endian::BigEndian2Host(value); }
	void Host2BigEndian() { value = unisim::util::endian::Host2BigEndian(value); }
	
	static unsigned int SizeOf() { return 8; }
	const void *Get() const { return &value; }
	void *Get() { return &value; }
	
	void SetV(TYPE v) { V::template Set<TYPE>(value, v); }
	void SetVSID(TYPE v) { VSID::template Set<TYPE>(value, v); }
	void SetH(TYPE v) { H::template Set<TYPE>(value, v); }
	void SetAPI(TYPE v) { API::template Set<TYPE>(value, v); }
	void SetXPN(TYPE v) { XPN::template Set<TYPE>(value, v); }
	void SetX(TYPE v) { X::template Set<TYPE>(value, v); }
	void SetRPN(TYPE v) { RPN::template Set<TYPE>(value, v); }
	void SetR(TYPE v) { R::template Set<TYPE>(value, v); }
	void SetC(TYPE v) { C::template Set<TYPE>(value, v); }
	void SetW(TYPE v) { W::template Set<TYPE>(value, v); }
	void SetI(TYPE v) { I::template Set<TYPE>(value, v); }
	void SetM(TYPE v) { M::template Set<TYPE>(value, v); }
	void SetG(TYPE v) { G::template Set<TYPE>(value, v); }
	void SetPP(TYPE v) { PP::template Set<TYPE>(value, v); }

	bool Match(VIRTUAL_SEGMENT_ID _vsid, ABBREVIATED_PAGE_INDEX _api, unsigned int _h) const { return IsValid() && (GetH() == _h) && (GetVSID() == _vsid) && (GetAPI() == _api); }
	
	void ShortPrettyPrint(std::ostream& os) const
	{
		if(!named_fields)
		{
			V   ::SetName("V");    V   ::SetDescription("Entry valid (V = 1) or invalid (V = 0)");
			VSID::SetName("VSID"); VSID::SetDescription("Virtual segment ID");
			H   ::SetName("H");    H   ::SetDescription("Hash function identifier");
			API ::SetName("API");  API ::SetDescription("Abbreviated page index");
			RPN ::SetName("RPN");  RPN ::SetDescription("Physical page number");
			XPN ::SetName("XPN");  XPN ::SetDescription("Extended page number provides physical address bits 0-2");
			R   ::SetName("R");    R   ::SetDescription("Referenced bit");
			C   ::SetName("C");    C   ::SetDescription("Changed bit");
			W   ::SetName("W");    W   ::SetDescription("Write-Through");
			I   ::SetName("I");    I   ::SetDescription("Caching-Inhibited");
			M   ::SetName("M");    M   ::SetDescription("Memory coherence");
			G   ::SetName("G");    G   ::SetDescription("Guarded");
			X   ::SetName("X");    X   ::SetDescription("Extended page number provides physical address bit 3");
			PP  ::SetName("PP");   PP  ::SetDescription("Page protection bits");
			named_fields = true;
		}
		
		std::ios_base::fmtflags ff = os.flags();
		os << "PTE=";
		if(ff & os.hex) os << "0x";
		else if(ff & os.oct) os << "0";
		os << value;
		os << "<";
		V   ::ShortPrettyPrint(os, value);
		os << ":";
		VSID::ShortPrettyPrint(os, value);
		os << ":";
		H   ::ShortPrettyPrint(os, value);
		os << ":";
		API ::ShortPrettyPrint(os, value);
		os << ":";
		RPN ::ShortPrettyPrint(os, value);
		os << ":";
		XPN ::ShortPrettyPrint(os, value);
		os << ":";
		R   ::ShortPrettyPrint(os, value);
		os << ":";
		C   ::ShortPrettyPrint(os, value);
		os << ":";
		W   ::ShortPrettyPrint(os, value);
		os << ":";
		I   ::ShortPrettyPrint(os, value);
		os << ":";
		M   ::ShortPrettyPrint(os, value);
		os << ":";
		G   ::ShortPrettyPrint(os, value);
		os << ":";
		X   ::ShortPrettyPrint(os, value);
		os << ":";
		PP  ::ShortPrettyPrint(os, value);
		os << ">";
	}
	
private:
	TYPE value;
	static bool named_fields;
};

template <typename TYPES>
bool PTE<TYPES>::named_fields = false;

template <typename TYPES, typename CONFIG>
struct TLB;

template <typename TYPES>
struct TLB_ENTRY
{
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::VIRTUAL_SEGMENT_ID VIRTUAL_SEGMENT_ID;
	typedef typename TYPES::PAGE_INDEX PAGE_INDEX;
	typedef typename TYPES::ABBREVIATED_PAGE_INDEX ABBREVIATED_PAGE_INDEX;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	// Constructor and initialization
	TLB_ENTRY() : next(0), prev(0), pte() {}
	void Initialize(const PTE<TYPES>& _pte, PAGE_INDEX _page_index) { pte = _pte; page_index = _page_index; }
	void Reset() { pte.Reset(); page_index = 0; }
	
	// Getters
	const PTE<TYPES>& GetPTE() const { return pte; }
	PTE<TYPES>& GetPTE() { return pte; }
	typename PTE<TYPES>::TYPE GetV() const { return pte.GetV(); }
	typename PTE<TYPES>::TYPE GetVSID() const { return pte.GetVSID(); }
	typename PTE<TYPES>::TYPE GetH() const { return pte.GetH(); }
	ABBREVIATED_PAGE_INDEX GetAPI() const { return pte.GetAPI(); }
	PAGE_INDEX GetPageIndex() const { return page_index; }
	typename PTE<TYPES>::TYPE GetXPN() const { return pte.GetXPN(); }
	typename PTE<TYPES>::TYPE GetX() const { return pte.GetX(); }
	typename PTE<TYPES>::TYPE GetRPN() const { return pte.GetRPN(); }
	typename PTE<TYPES>::TYPE GetR() const { return pte.GetR(); }
	typename PTE<TYPES>::TYPE GetC() const { return pte.GetC(); }
	typename PTE<TYPES>::TYPE GetW() const { return pte.GetW(); }
	typename PTE<TYPES>::TYPE GetI() const { return pte.GetI(); }
	typename PTE<TYPES>::TYPE GetM() const { return pte.GetM(); }
	typename PTE<TYPES>::TYPE GetG() const { return pte.GetG(); }
	typename PTE<TYPES>::TYPE GetPP() const { return pte.GetPP(); }
	bool IsValid() const { return pte.IsValid(); }
	ADDRESS GetPageVirtualAddress() const { return pte.GetPageVirtualAddress(page_index); }
	PHYSICAL_ADDRESS GetPagePhysicalAddress(bool xaen) const { return pte.GetPagePhysicalAddress(xaen); }
	STORAGE_ATTR GetStorageAttr() const { return pte.GetStorageAttr(); }
	static uint32_t GetPageSize() { return PTE<TYPES>::GetPageSize(); }
	void Access() { pte.Access(); }
	void Change() { pte.Change(); }
	void SetPageIndex(PAGE_INDEX _page_index) { page_index = _page_index; }
	
	// Match
	bool Match(VIRTUAL_SEGMENT_ID _vsid, PAGE_INDEX _page_index) const;
	
	// Printer
	void Print(std::ostream& os) const;

private:
	template <typename _TYPES, typename CONFIG>
	friend struct TLB;
	TLB_ENTRY<TYPES> *next, *prev;
	// Hardware TLB entry contains on-chip copy of PTE and EAPI. Page index bits are inferred from API in PTE for upper bits, EAPI in TLB entry for middle bits, and TLB index for lower bits
	PTE<TYPES> pte;
	PAGE_INDEX page_index;
	
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
bool TLB_ENTRY<TYPES>::Match(VIRTUAL_SEGMENT_ID _vsid, PAGE_INDEX _page_index) const
{
	return IsValid() && (GetVSID() == _vsid) && (GetPageIndex() == _page_index);
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
	os.width(13);
	os << +GetPageVirtualAddress();
	os << "-0x";
	os.width(13);
	os << (GetPageVirtualAddress() + GetPageSize() - 1);
	
	// physical address
	os << " Physical Address:0x";
	os.width(8);
	os << +GetPagePhysicalAddress(false);
	os << "-0x";
	os.width(8);
	os << (GetPagePhysicalAddress(false) + GetPageSize() - 1);

	// extended physical address
	os << " Extended Physical Address:0x";
	os.width(9);
	os << +GetPagePhysicalAddress(true);
	os << "-0x";
	os.width(9);
	os << (GetPagePhysicalAddress(true) + GetPageSize() - 1);

	// Reference bit:
	os << " R:" << GetR();
	
	// Changed bit
	os << " C:" << GetC();

	// hash function identifier
	os << " H:" << GetH();
	
	// page protection
	os << " PP:";
	os << ((GetPP() >> 1) & 1) << (GetPP() & 1);
	
	// storage attribute
	os << " W:" << GetW();
	os << " I:" << GetI();
	os << " M:" << GetM();
	os << " G:" << GetG();
	
	os.fill(fill);
	os.flags(flags);
}

template <typename TYPES, typename CONFIG>
struct TLB
	: unisim::kernel::Service<unisim::service::interfaces::HttpServer>
{
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::VIRTUAL_SEGMENT_ID VIRTUAL_SEGMENT_ID;
	typedef typename TYPES::PAGE_INDEX PAGE_INDEX;
	typedef typename TYPES::ABBREVIATED_PAGE_INDEX ABBREVIATED_PAGE_INDEX;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	enum PrintFormat
	{
		PFMT_TXT,
		PFMT_HTML
	};
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
	TLB(const char *name, unisim::kernel::Object *parent = 0, const char *description = 0);
	void Reset();
	void InvalidateSet(PAGE_INDEX page_index);
	TLB_ENTRY<TYPES>& GetEntry(PAGE_INDEX page_index, unsigned int tlb_way);

	static unsigned int Index(PAGE_INDEX page_index);
	void Use(TLB_ENTRY<TYPES>& tlb_entry);
	TLB_ENTRY<TYPES> *Lookup(VIRTUAL_SEGMENT_ID vsid, PAGE_INDEX page_index);
	TLB_ENTRY<TYPES>& Replace(const PTE<TYPES>& pte, PAGE_INDEX page_index);
	const TLB_ENTRY<TYPES>& GetLRU(ADDRESS va, unsigned int& lru_way) const;
	unsigned int GetWay(unsigned int tlb_index, const TLB_ENTRY<TYPES>& tlb_entry) const;
	void Print(std::ostream& os, unsigned int tlb_index, unsigned int tlb_way, PrintFormat pfmt) const;
	void Print(std::ostream& os, PrintFormat pfmt) const;
	
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
protected:
	unisim::kernel::logger::Logger logger;
private:
	TLB_ENTRY<TYPES> entries[CONFIG::SIZE / CONFIG::ASSOCIATIVITY][CONFIG::ASSOCIATIVITY];
	TLB_ENTRY<TYPES> *mru_entries[CONFIG::SIZE / CONFIG::ASSOCIATIVITY];
	TLB_ENTRY<TYPES> *lru_entries[CONFIG::SIZE / CONFIG::ASSOCIATIVITY];
	bool verbose;
	uint64_t num_accesses;
	uint64_t num_misses;
	
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_misses;
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
	, mru_entries()
	, lru_entries()
	, verbose(false)
	, num_accesses(0)
	, num_misses(0)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, stat_num_accesses("num-accesses", this, num_accesses, "Number of accesses")
	, stat_num_misses("num-misses", this, num_misses, "Number of misses")
{
	Reset();
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::Reset()
{
	for(unsigned int i = 0; i < (CONFIG::SIZE / CONFIG::ASSOCIATIVITY); ++i)
	{
		mru_entries[i] = &entries[i][0];
		lru_entries[i] = &entries[i][CONFIG::ASSOCIATIVITY - 1];
		for(unsigned int j = 0; j < CONFIG::ASSOCIATIVITY; ++j)
		{
			entries[i][j].Reset();
			entries[i][j].prev = j ? &entries[i][j - 1] : 0;
			entries[i][j].next = (j != (CONFIG::ASSOCIATIVITY - 1)) ? &entries[i][j + 1] : 0;
		}
	}
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::InvalidateSet(PAGE_INDEX page_index)
{
	unsigned int tlb_index = Index(page_index);
	
	for(unsigned int tlb_way = 0; tlb_way < CONFIG::ASSOCIATIVITY; ++tlb_way)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Invalidating TLB entry at index #" << tlb_index << " and way #" << tlb_way << EndDebugInfo;
		}
		entries[tlb_index][tlb_way].Reset();
	}
}

template <typename TYPES, typename CONFIG>
TLB_ENTRY<TYPES>& TLB<TYPES, CONFIG>::GetEntry(PAGE_INDEX page_index, unsigned int tlb_way)
{
	unsigned int tlb_index = Index(page_index);
	
	return entries[tlb_index][tlb_way];
}

template <typename TYPES, typename CONFIG>
unsigned int TLB<TYPES, CONFIG>::Index(PAGE_INDEX page_index)
{
	return (page_index % CONFIG::SIZE) / CONFIG::ASSOCIATIVITY;
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::Use(TLB_ENTRY<TYPES>& tlb_entry)
{
	if(unlikely(tlb_entry.prev))
	{
		unsigned int tlb_index = (&tlb_entry - &entries[0][0]) / CONFIG::ASSOCIATIVITY;
		if(!(tlb_entry.prev->next = tlb_entry.next))
		{
			lru_entries[tlb_index] = tlb_entry.prev;
		}
		mru_entries[tlb_index]->prev = &tlb_entry;
		tlb_entry.next = mru_entries[tlb_index];
		tlb_entry.prev = 0;
		mru_entries[tlb_index] = &tlb_entry;
	}
}

template <typename TYPES, typename CONFIG>
TLB_ENTRY<TYPES> *TLB<TYPES, CONFIG>::Lookup(VIRTUAL_SEGMENT_ID vsid, PAGE_INDEX page_index)
{
	num_accesses++;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Lookup at @(VSID=0x" << std::hex << vsid << ",PAGE_INDEX=0x" << page_index << ")" << std::dec << EndDebugInfo;
	}
	
	unsigned int tlb_index = Index(page_index);
	
	TLB_ENTRY<TYPES> *tlb_entry = 0;
	unsigned int i;
	for(i = 0, tlb_entry = mru_entries[tlb_index]; i < CONFIG::ASSOCIATIVITY; ++i, tlb_entry = tlb_entry->next)
	{
		if(tlb_entry->Match(vsid, page_index))
		{
			// TLB hit
			if(unlikely(verbose))
			{
				logger << DebugInfo << "TLB Hit at index #" << tlb_index << " and way #" << GetWay(tlb_index, *tlb_entry) << EndDebugInfo;
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
TLB_ENTRY<TYPES>& TLB<TYPES, CONFIG>::Replace(const PTE<TYPES>& pte, PAGE_INDEX page_index)
{
	unsigned int tlb_index = Index(page_index);
	TLB_ENTRY<TYPES> *tlb_entry = lru_entries[tlb_index];
	
	if(unlikely(verbose))
	{
		unsigned int tlb_way = GetWay(tlb_index, *tlb_entry);
		logger << DebugInfo << "Replacing TLB entry at index #" << tlb_index << " and way #" << tlb_way << EndDebugInfo;
	}
	
	tlb_entry->Initialize(pte, page_index);
	return *tlb_entry;
}

template <typename TYPES, typename CONFIG>
const TLB_ENTRY<TYPES>& TLB<TYPES, CONFIG>::GetLRU(ADDRESS va, unsigned int& lru_way) const
{
	unsigned int tlb_index = Index(va);
	const TLB_ENTRY<TYPES>& lru_entry = *lru_entries[tlb_index];
	lru_way = GetWay(tlb_index, lru_entry);
	return lru_entry;
}

template <typename TYPES, typename CONFIG>
unsigned int TLB<TYPES, CONFIG>::GetWay(unsigned int tlb_index, const TLB_ENTRY<TYPES>& tlb_entry) const
{
	return &tlb_entry - &entries[tlb_index][0];
}

template <typename TYPES, typename CONFIG>
void TLB<TYPES, CONFIG>::Print(std::ostream& os, unsigned int tlb_index, unsigned int tlb_way, PrintFormat pfmt) const
{
	const TLB_ENTRY<TYPES>& tlb_entry = entries[tlb_index][tlb_way];
	
	std::ostream::char_type fill(os.fill());
	std::ios_base::fmtflags flags(os.flags());
	os.fill('0');

	switch(pfmt)
	{
		case PFMT_TXT:
		{
			tlb_entry.Print(os);
			os << std::endl;
			break;
		}
		
		case PFMT_HTML:
		{
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << "\"><span>" << +tlb_entry.GetV() << "</span></td>";
			
			os << std::hex;
			
			// virtual address
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " vaddr-range\"><span>0x";
			os.width(13);
			os << +tlb_entry.GetPageVirtualAddress();
			os << "-0x";
			os.width(13);
			os << (tlb_entry.GetPageVirtualAddress() + tlb_entry.GetPageSize() - 1);
			os << "</span></td>";
			
			// physical address
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " paddr-range\"><span>0x";
			os.width(8);
			os << +tlb_entry.GetPagePhysicalAddress(false);
			os << "-0x";
			os.width(8);
			os << (tlb_entry.GetPagePhysicalAddress(false) + tlb_entry.GetPageSize() - 1);
			os << "</span></td>";

			// extended physical address
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " ext-paddr-range\"><span>0x";
			os.width(9);
			os << +tlb_entry.GetPagePhysicalAddress(true);
			os << "-0x";
			os.width(9);
			os << (tlb_entry.GetPagePhysicalAddress(true) + tlb_entry.GetPageSize() - 1);
			os << "</span></td>";

			// access control
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " reference-bit\"><span>" << tlb_entry.GetR() << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " changed-bit\"><span>" << tlb_entry.GetC() << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " hash-func-id\"><span>" << tlb_entry.GetH() << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " page-protect-bits\"><span>" << ((tlb_entry.GetPP() >> 1) & 1) << (tlb_entry.GetPP() & 1) << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " W\"><span>" << tlb_entry.GetW() << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " I\"><span>" << tlb_entry.GetI() << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " M\"><span>" << tlb_entry.GetM() << "</span></td>";
			os << "<td class=\"" << (tlb_entry.GetV() ? "valid" : "invalid") << " G\"><span>" << tlb_entry.GetG() << "</span></td>";
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
		os << "\t<colgroup span=\"1\"></colgroup>" << std::endl;
		for(unsigned int tlb_way = 0; tlb_way < CONFIG::ASSOCIATIVITY; ++tlb_way)
		{
			os << "\t\t<col colspan=\"12\">" << std::endl;
		}
		os << "\t<tr>" << std::endl;
		os << "\t\t<th rowspan=\"2\">Index&bsol;Way</th>" << std::endl;
		for(unsigned int tlb_way = 0; tlb_way < CONFIG::ASSOCIATIVITY; ++tlb_way)
		{
			os << "\t\t<th colspan=\"12\" scope=\"colgroup\">" << tlb_way << "</th>" << std::endl;
		}
		os << "\t</tr>" << std::endl;
		os << "\t<tr>" << std::endl;
		for(unsigned int tlb_way = 0; tlb_way < CONFIG::ASSOCIATIVITY; ++tlb_way)
		{
			os << "\t\t<th scope=\"col\"><span>V</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>Virtual Address</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>Physical Address</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>Ext. Phys. Address</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>R</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>C</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>H</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>PP</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>W</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>I</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>M</span></th>" << std::endl;
			os << "\t\t<th scope=\"col\"><span>G</span></th>" << std::endl;
		}
		os << "\t</tr>" << std::endl;
	}
	for(unsigned int tlb_index = 0; tlb_index < (CONFIG::SIZE / CONFIG::ASSOCIATIVITY); ++tlb_index)
	{
		if(pfmt == PFMT_HTML)
		{
			os << "\t<tr>" << std::endl;
			os << "\t\t<th scope=\"row\">" << tlb_index << "</th>" << std::endl;
		}
		for(unsigned int tlb_way = 0; tlb_way < CONFIG::ASSOCIATIVITY; ++tlb_way)
		{
			if(pfmt == PFMT_TXT)
			{
				os << "Index:" << std::dec << tlb_index << " Way:" << std::dec << tlb_way << " ";
			}
			Print(os, tlb_index, tlb_way, pfmt);
		}
		if(pfmt == PFMT_HTML)
		{
			os << "\t</tr>" << std::endl;
		}
	}
	if(pfmt == PFMT_HTML)
	{
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
				response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/component/cxx/processor/powerpc/e600/mmu_style.css\" type=\"text/css\" />" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				//response << "\t\t<script src=\"/unisim/component/cxx/processor/powerpc/e600/mmu_script.js\"></script>" << std::endl;
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
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::VIRTUAL_SEGMENT_ID VIRTUAL_SEGMENT_ID;
	typedef typename TYPES::PAGE_INDEX PAGE_INDEX;
	typedef typename TYPES::ABBREVIATED_PAGE_INDEX ABBREVIATED_PAGE_INDEX;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	typedef typename CPU::MSR MSR;
	typedef typename CPU::HID0 HID0;
	typedef typename CPU::PTEHI PTEHI;
	typedef typename CPU::PTELO PTELO;
	typedef typename CPU::TLBMISS TLBMISS;
	typedef typename CPU::SDR1 SDR1;
	typedef typename CPU::UpperBlockAddressTranslationRegister UpperBlockAddressTranslationRegister;
	typedef typename CPU::LowerBlockAddressTranslationRegister LowerBlockAddressTranslationRegister;
	
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> itlb_http_server_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> dtlb_http_server_export;

	MMU(CPU *cpu);
	virtual ~MMU();
	void Reset();
	template <bool EXEC> void InvalidateTLBSetByEffectiveAddress(EFFECTIVE_ADDRESS ea);
	template <bool EXEC> void LoadTLBEntryByEffectiveAddress(EFFECTIVE_ADDRESS ea);
	template <bool EXEC, bool WRITE> bool ControlPageAccess(EFFECTIVE_ADDRESS ea, unsigned int key, const PTE<TYPES>& pte);
	template <bool DEBUG, bool EXEC, bool WRITE> inline bool Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr) ALWAYS_INLINE;
	
	SR& GetSR(unsigned int n) { return sr[n]; }
	const SR& GetSR(unsigned int n) const { return sr[n]; }
	void SetSR(unsigned int n, uint32_t value) { sr[n] = value; }
	
protected:
	unisim::kernel::logger::Logger logger;
private:

	SWITCH_ENUM_TRAIT(Model, _NUM_BATS);
	CASE_ENUM_TRAIT(MPC7400 , _NUM_BATS) { static const unsigned int VALUE = 4; };
	CASE_ENUM_TRAIT(MPC7410 , _NUM_BATS) { static const unsigned int VALUE = 4; };
	CASE_ENUM_TRAIT(MPC7441 , _NUM_BATS) { static const unsigned int VALUE = 4; };
	CASE_ENUM_TRAIT(MPC7445 , _NUM_BATS) { static const unsigned int VALUE = 8; };
	CASE_ENUM_TRAIT(MPC7447 , _NUM_BATS) { static const unsigned int VALUE = 8; };
	CASE_ENUM_TRAIT(MPC7447A, _NUM_BATS) { static const unsigned int VALUE = 8; };
	CASE_ENUM_TRAIT(MPC7448 , _NUM_BATS) { static const unsigned int VALUE = 8; };
	CASE_ENUM_TRAIT(MPC7450 , _NUM_BATS) { static const unsigned int VALUE = 4; };
	CASE_ENUM_TRAIT(MPC7451 , _NUM_BATS) { static const unsigned int VALUE = 4; };
	CASE_ENUM_TRAIT(MPC7455 , _NUM_BATS) { static const unsigned int VALUE = 8; };
	CASE_ENUM_TRAIT(MPC7457 , _NUM_BATS) { static const unsigned int VALUE = 8; };
	static const unsigned int NUM_BATS = ENUM_TRAIT(CPU::MODEL, _NUM_BATS)::VALUE;
	
	CPU *cpu;
	UpperBlockAddressTranslationRegister *ibatu[NUM_BATS];
	LowerBlockAddressTranslationRegister *ibatl[NUM_BATS];
	UpperBlockAddressTranslationRegister *dbatu[NUM_BATS];
	LowerBlockAddressTranslationRegister *dbatl[NUM_BATS];
	SR sr[16];
	ITLB itlb;
	DTLB dtlb;
	SDR1 sdr1;
	
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
	, logger(*this)
	, cpu(_cpu)
	, ibatu()
	, ibatl()
	, dbatu()
	, dbatl()
	, sr()
	, itlb("ITLB", this, "Instruction TLB")
	, dtlb("DTLB", this, "Data TLB")
	, sdr1(cpu)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	itlb_http_server_export >> itlb.http_server_export;
	dtlb_http_server_export >> dtlb.http_server_export;
	
	for(unsigned int i = 0; i < NUM_BATS; i++)
	{
		switch(i)
		{
			case 0: ibatu[0] = new typename CPU::IBAT0U(cpu); ibatl[0] = new typename CPU::IBAT0L(cpu); dbatu[0] = new typename CPU::DBAT0U(cpu);  dbatl[0] = new typename CPU::DBAT0L(cpu);break;
			case 1: ibatu[1] = new typename CPU::IBAT1U(cpu); ibatl[1] = new typename CPU::IBAT1L(cpu); dbatu[1] = new typename CPU::DBAT1U(cpu);  dbatl[1] = new typename CPU::DBAT1L(cpu);break;
			case 2: ibatu[2] = new typename CPU::IBAT2U(cpu); ibatl[2] = new typename CPU::IBAT2L(cpu); dbatu[2] = new typename CPU::DBAT2U(cpu);  dbatl[2] = new typename CPU::DBAT2L(cpu);break;
			case 3: ibatu[3] = new typename CPU::IBAT3U(cpu); ibatl[3] = new typename CPU::IBAT3L(cpu); dbatu[3] = new typename CPU::DBAT3U(cpu);  dbatl[3] = new typename CPU::DBAT3L(cpu);break;
			case 4: ibatu[4] = new typename CPU::IBAT4U(cpu); ibatl[4] = new typename CPU::IBAT4L(cpu); dbatu[4] = new typename CPU::DBAT4U(cpu);  dbatl[4] = new typename CPU::DBAT4L(cpu);break;
			case 5: ibatu[5] = new typename CPU::IBAT5U(cpu); ibatl[5] = new typename CPU::IBAT5L(cpu); dbatu[5] = new typename CPU::DBAT5U(cpu);  dbatl[5] = new typename CPU::DBAT5L(cpu);break;
			case 6: ibatu[6] = new typename CPU::IBAT6U(cpu); ibatl[6] = new typename CPU::IBAT6L(cpu); dbatu[6] = new typename CPU::DBAT6U(cpu);  dbatl[6] = new typename CPU::DBAT6L(cpu);break;
			case 7: ibatu[7] = new typename CPU::IBAT7U(cpu); ibatl[7] = new typename CPU::IBAT7L(cpu); dbatu[7] = new typename CPU::DBAT7U(cpu);  dbatl[7] = new typename CPU::DBAT7L(cpu);break;
		}
	}
	
	for(unsigned int i = 0; i < 16; i++)
	{
		sr[i].Init(i);
		cpu->AddRegisterInterface(sr[i].CreateRegisterInterface());
	}
	
	Reset();
}

template <typename TYPES, typename CONFIG>
MMU<TYPES, CONFIG>::~MMU()
{
	for(unsigned int i = 0; i < NUM_BATS; i++)
	{
		delete ibatu[i];
		delete ibatl[i];
		delete dbatu[i];
		delete dbatl[i];
	}
}

template <typename TYPES, typename CONFIG>
void MMU<TYPES, CONFIG>::Reset()
{
	itlb.Reset();
	dtlb.Reset();
}

template <typename TYPES, typename CONFIG>
template <bool EXEC>
void MMU<TYPES, CONFIG>::InvalidateTLBSetByEffectiveAddress(EFFECTIVE_ADDRESS ea)
{
	PAGE_INDEX page_index = TYPES::EA::PAGE_INDEX::template Get<EFFECTIVE_ADDRESS>(ea);
	if(EXEC)
	{
		itlb.InvalidateSet(page_index);
	}
	else
	{
		dtlb.InvalidateSet(page_index);
	}
}

template <typename TYPES, typename CONFIG>
template <bool EXEC>
void MMU<TYPES, CONFIG>::LoadTLBEntryByEffectiveAddress(EFFECTIVE_ADDRESS ea)
{
	const PTEHI& ptehi = cpu->GetPTEHI();
	const PTELO& ptelo = cpu->GetPTELO();
	
	PAGE_INDEX page_index = TYPES::EA::PAGE_INDEX::template Get<EFFECTIVE_ADDRESS>(ea);
	unsigned int tlb_way = ea % EXEC ? CONFIG::ITLB_CONFIG::ASSOCIATIVITY : CONFIG::DTLB_CONFIG::ASSOCIATIVITY;
	
	TLB_ENTRY<TYPES>& tlb_entry = EXEC ? itlb.GetEntry(page_index, tlb_way) : dtlb.GetEntry(page_index, tlb_way);
	
	PTE<TYPES>& pte = tlb_entry.GetPTE();
	pte.SetV(ptehi.template Get<typename PTEHI::V>());
	pte.SetVSID(ptehi.template Get<typename PTEHI::VSID>());
	pte.SetAPI(ptehi.template Get<typename PTEHI::API>());
	pte.SetRPN(ptelo.template Get<typename PTELO::RPN>());
	pte.SetXPN(ptelo.template Get<typename PTELO::XPN>());
	pte.SetC(ptelo.template Get<typename PTELO::C>());
	pte.SetW(ptelo.template Get<typename PTELO::W>());
	pte.SetI(ptelo.template Get<typename PTELO::I>());
	pte.SetM(ptelo.template Get<typename PTELO::M>());
	pte.SetG(ptelo.template Get<typename PTELO::G>());
	pte.SetX(ptelo.template Get<typename PTELO::X>());
	pte.SetPP(ptelo.template Get<typename PTELO::PP>());
	tlb_entry.SetPageIndex(page_index);
}

template <typename TYPES, typename CONFIG>
template <bool EXEC, bool WRITE>
bool MMU<TYPES, CONFIG>::ControlPageAccess(EFFECTIVE_ADDRESS ea, unsigned int key, const PTE<TYPES>& pte)
{
	if(EXEC && pte.GetG()) // Guarded memory?
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Attempt to execute from guarded memory at effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
		}
		
		cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::GuardedMemory>();
		return false;
	}
	
	// Access protection control with key
	unsigned int pp = pte.GetPP();
	
	// Key | PP | Page Type
	//-----+----+-----------
	//  0  | 00 | Read/write
	//  0  | 01 | Read/write
	//  0  | 10 | Read/write
	//  0  | 11 | Read only
	//  1  | 00 | No access
	//  1  | 01 | Read only
	//  1  | 10 | Read/write
	//  1  | 11 | Read only
	if((key && (!pp || (WRITE && (pp & 1)))) || (!key && (WRITE && (pp == 3))))
	{
		if(EXEC)
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Attempt to fetch from effective address 0x" << std::hex << ea << std::dec << " with pp=" << pp << " and key=" << key << EndDebugInfo;
			}
			
			cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::ProtectionViolation>();
			return false;
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Attempt to " << (WRITE ? "store to" : "load from ") << " effective address 0x" << std::hex << ea << std::dec << " with pp=" << pp << " and key=" << key << EndDebugInfo;
			}
			
			cpu->template ThrowException<typename CPU::DataStorageInterrupt::ProtectionViolation>().SetAddress(ea).SetWrite(WRITE);
			return false;
		}
	}
	return true;
}

template <typename TYPES, typename CONFIG>
template <bool DEBUG, bool EXEC, bool WRITE>
inline bool MMU<TYPES, CONFIG>::Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr)
{
	const MSR& msr = cpu->GetMSR();
	
	if(unlikely(verbose))
	{
		std::ostream& info_stream = logger.DebugInfoStream();
		info_stream << "Using " << std::hex;
		msr.ShortPrettyPrint(info_stream);
		info_stream << std::dec << std::endl;
	}
	
	if(likely((EXEC ? (msr.template Get<typename MSR::IR>()) : (msr.template Get<typename MSR::DR>()))))
	{
		// Address Translation is enabled
		const HID0& hid0 = cpu->GetHID0();
		
		bool xaen = hid0.template Get<typename HID0::XAEN>(); // extended addressing enable
		bool xbsen = hid0.template Get<typename HID0::XBSEN>(); // extended block size enable
		
		// Block Address Translation
		for(unsigned int i = 0; i < NUM_BATS; i++)
		{
			UpperBlockAddressTranslationRegister& batu = EXEC ? (*this->ibatu[i]) : (*this->dbatu[i]);
			LowerBlockAddressTranslationRegister& batl = EXEC ? (*this->ibatl[i]) : (*this->dbatl[i]);
			
			// Check if BAT register match
			EFFECTIVE_ADDRESS ea_bepi_0_3 = TYPES::EA::BEPI_0_3::template Get<EFFECTIVE_ADDRESS>(ea);
			EFFECTIVE_ADDRESS batu_bepi_0_3 = batu.template Get<typename UpperBlockAddressTranslationRegister::BEPI_0_3>();
			if(likely(ea_bepi_0_3 == batu_bepi_0_3))
			{
				EFFECTIVE_ADDRESS ea_bepi_4_14 = TYPES::EA::BEPI_4_14::template Get<EFFECTIVE_ADDRESS>(ea);
				EFFECTIVE_ADDRESS batu_bepi_4_14 = batu.template Get<typename UpperBlockAddressTranslationRegister::BEPI_4_14>();
				EFFECTIVE_ADDRESS block_size_mask = xbsen ? batu.template Get<typename UpperBlockAddressTranslationRegister::XBS_BL>() : batu.template Get<typename UpperBlockAddressTranslationRegister::BL>();
				if(likely((ea_bepi_4_14 & ~block_size_mask) == (batu_bepi_4_14 & ~block_size_mask)))
				{
					unsigned int Vs = batu.template Get<typename UpperBlockAddressTranslationRegister::Vs>();
					unsigned int Vp = batu.template Get<typename UpperBlockAddressTranslationRegister::Vp>();
					unsigned int BAT_entry_valid = msr.template Get<typename MSR::PR>() ? Vp : Vs;
					if(likely(BAT_entry_valid))
					{
						// BAT match
						if(unlikely(verbose))
						{
							logger << DebugInfo << batu.GetName() << "/" << batl.GetName() << " match for effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
						}
						
						// Swap matching register pair with first one to speed-up next lookup next time
						if(unlikely(i))
						{
							if(EXEC)
							{
								UpperBlockAddressTranslationRegister *u = this->ibatu[0];
								LowerBlockAddressTranslationRegister *l = this->ibatl[0];
								this->ibatu[0] = &batu;
								this->ibatl[0] = &batl;
								this->ibatu[i] = u;
								this->ibatl[i] = l;
							}
							else
							{
								UpperBlockAddressTranslationRegister *u = this->dbatu[0];
								LowerBlockAddressTranslationRegister *l = this->dbatl[0];
								this->dbatu[0] = &batu;
								this->dbatl[0] = &batl;
								this->dbatu[i] = u;
								this->dbatl[i] = l;
							}
						}
						
						// Check access rights
						if(!DEBUG)
						{
							unsigned int pp = batl.template Get<typename LowerBlockAddressTranslationRegister::PP>();
							
							if(EXEC)
							{
								if(unlikely((batl.template Get<typename LowerBlockAddressTranslationRegister::G>())))
								{
									if(unlikely(verbose))
									{
										logger << DebugInfo << "Attempt to execute from guarded memory at effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
									}
									cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::GuardedMemory>();
									return false;
								}
								if(unlikely(!pp)) // PP=00: No access
								{
									if(unlikely(verbose))
									{
										logger << DebugInfo << "Attempt to fetch from effective address 0x" << std::hex << ea << std::dec << " with pp=" << pp << EndDebugInfo;
									}
									cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::ProtectionViolation>();
									return false;
								}
							}
							else
							{
								if(unlikely(!pp ||                // PP=00: No access
								            ((pp & 1) && WRITE))) // PP=x1: Read-only
								{
									if(unlikely(verbose))
									{
										logger << DebugInfo << "Attempt to " << (WRITE ? "store to" : "load from ") << " effective address 0x" << std::hex << ea << std::dec << " with pp=" << pp << EndDebugInfo;
									}
									cpu->template ThrowException<typename CPU::DataStorageInterrupt::ProtectionViolation>().SetAddress(ea).SetWrite(WRITE);
									return false;
								}
							}
						}

						// Compute the physical address
						EFFECTIVE_ADDRESS masked_ea = ea & (block_size_mask << 17);
						EFFECTIVE_ADDRESS offset = ea & unisim::util::reg::core::template MakeMask<EFFECTIVE_ADDRESS, 17>();
						EFFECTIVE_ADDRESS brpn = EFFECTIVE_ADDRESS(batl) & LowerBlockAddressTranslationRegister::BRPN::template GetMask<EFFECTIVE_ADDRESS>();
						uint32_t bx = batl.template Get<typename LowerBlockAddressTranslationRegister::BX>();
						uint32_t bxpn = batl.template Get<typename LowerBlockAddressTranslationRegister::BXPN>();
						uint32_t wimg = batl.template Get<typename LowerBlockAddressTranslationRegister::WIMG>();
						size_to_protection_boundary = (offset ^ unisim::util::reg::core::MakeMask<EFFECTIVE_ADDRESS, 17>()) + 1;
						virt_addr = ea;
						phys_addr = offset
								| masked_ea
								| brpn
								| (PHYSICAL_ADDRESS(bx) << 32)
								| (PHYSICAL_ADDRESS(bxpn) << 33);
						storage_attr = STORAGE_ATTR(wimg);
						
						if(unlikely(verbose))
						{
							logger << DebugInfo << "Effective address 0x" << std::hex << ea << std::dec << " translates to physical address 0x" << std::hex << phys_addr << std::dec << " with storage attributes 0x" << std::hex << +storage_attr << std::dec << EndDebugInfo;
						}
						
						return true;
					}
				}
			}
		}
		
		// No BAT match
		if(unlikely(verbose))
		{
			logger << DebugInfo << "No " << (EXEC ? "I" : "D") << "BAT match for effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
		}
		EFFECTIVE_ADDRESS offset = ea & unisim::util::reg::core::template MakeMask<EFFECTIVE_ADDRESS, 12>();

		// Memory Segment
		unsigned int sr_num = TYPES::EA::SR::template Get<EFFECTIVE_ADDRESS>(ea);
		SR& sreg = sr[sr_num];
		if(unlikely(verbose))
		{
			std::ostream& info_stream = logger.DebugInfoStream();
			info_stream << "Using " << std::hex;
			sreg.ShortPrettyPrint(info_stream);
			info_stream << std::dec << std::endl;
		}
		
		// Check segment access rights
		if(!DEBUG)
		{
			if(unlikely((EXEC && sreg.template Get<typename SR::N>())))
			{
				cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::NoExecute>();
				return false;
			}
			
			if(unlikely((sreg.template Get<typename SR::T>())))
			{
				if(EXEC)
				{
					cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::DirectStore>();
					return false;
				}
				else
				{
					cpu->template ThrowException<typename CPU::DataStorageInterrupt::DirectStore>().SetAddress(ea).SetWrite(WRITE);
					return false;
				}
			}
		}
		
		VIRTUAL_SEGMENT_ID vsid = sreg.template Get<typename SR::VSID>();
		unsigned int Kp = sreg.template Get<typename SR::Kp>();
		unsigned int Ks = sreg.template Get<typename SR::Ks>();
		unsigned int key = msr.template Get<typename MSR::PR>() ? Kp : Ks;
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Kp=" << Kp << ", Ks=" << Ks << ", PR=" << msr.template Get<typename MSR::PR>() << ", key=" << key << EndDebugInfo;
		}
		
		// Virtual address
		ADDRESS va = (ADDRESS(vsid) << 28) | ADDRESS(ea & unisim::util::reg::core::template MakeMask<EFFECTIVE_ADDRESS, 28>());
		
		// TLB
		PAGE_INDEX page_index = TYPES::EA::PAGE_INDEX::template Get<EFFECTIVE_ADDRESS>(ea);
		ABBREVIATED_PAGE_INDEX api = TYPES::EA::API::template Get<EFFECTIVE_ADDRESS>(ea);
		
		TLB_ENTRY<TYPES> *tlb_entry = EXEC ? itlb.Lookup(vsid, page_index) : dtlb.Lookup(vsid, page_index);
		
		if(likely(tlb_entry))
		{
			// TLB hit
			if(unlikely(verbose))
			{
				logger << DebugInfo << (EXEC ? "I" : "D") << "TLB hit for effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
			}
			const PTE<TYPES>& pte = tlb_entry->GetPTE();
			
			if(unlikely(verbose))
			{
				std::ostream& info_stream = logger.DebugInfoStream();
				info_stream << std::hex;
				pte.ShortPrettyPrint(info_stream);
				info_stream << " in TLB match for effective address 0x" << std::hex << ea << std::dec << std::endl;
			}
			
			if(!DEBUG)
			{
				if(unlikely((!this->template ControlPageAccess<EXEC, WRITE>(ea, key, pte)))) return false;
				
				if(EXEC)
				{
					itlb.Use(*tlb_entry);
				}
				else
				{
					dtlb.Use(*tlb_entry);
				}
			
				tlb_entry->Access();
				bool need_pte_update = WRITE && !tlb_entry->GetC();
				if(WRITE) tlb_entry->Change();
				
				if(unlikely(need_pte_update))
				{
					if(unlikely(verbose))
					{
						logger << DebugInfo << "PTE in page table needs update because PTE[C] is toggling from 0 to 1" << std::hex << ea << std::dec << EndDebugInfo;
					}
					unsigned int lru_way;
					uint32_t tlbmiss_value = 0;
					uint32_t ptehi_value = 0;
					
					TLBMISS::PAGE::template Set<uint32_t>(tlbmiss_value, TYPES::EA::_0_30::template Get<EFFECTIVE_ADDRESS>(ea));
					PTEHI::VSID::template Set<uint32_t>(ptehi_value, vsid);
					PTEHI::API::template Set<uint32_t>(ptehi_value, api);
					const TLB_ENTRY<TYPES>& lru_tlb_entry = dtlb.GetLRU(va, lru_way);
					TLBMISS::LRU::template Set<uint32_t>(tlbmiss_value, lru_way);
					PTEHI::V::template Set<uint32_t>(ptehi_value, lru_tlb_entry.IsValid());
					cpu->template ThrowException<typename CPU::DataTLBMissOnStoreInterrupt::DataTLBMissOnStore>().SetTLBMISS(tlbmiss_value).SetPTEHI(ptehi_value).SetKey(key).SetC(true);
					return false;
				}
			}
			storage_attr = tlb_entry->GetStorageAttr();
			virt_addr = va;
			PHYSICAL_ADDRESS page_phys_addr = tlb_entry->GetPagePhysicalAddress(xaen);
			phys_addr = page_phys_addr | offset;
			size_to_protection_boundary = tlb_entry->GetPageSize() - offset;
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Effective address 0x" << std::hex << ea << std::dec << " (virtual address 0x" << std::hex << virt_addr << std::dec << ") translates to physical address 0x" << std::hex << phys_addr << std::dec << " with storage attributes 0x" << std::hex << +storage_attr << std::dec << EndDebugInfo;
			}
			return true;
		}
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << (EXEC ? "I" : "D") << "TLB miss for effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
		}
		
		if(!DEBUG)
		{
			if(unlikely(hid0.template Get<typename HID0::STEN>()))
			{
				// Software page table search
				unsigned int lru_way;
				uint32_t tlbmiss_value = 0;
				uint32_t ptehi_value = 0;
				
				TLBMISS::PAGE::template Set<uint32_t>(tlbmiss_value, TYPES::EA::_0_30::template Get<EFFECTIVE_ADDRESS>(ea));
				PTEHI::VSID::template Set<uint32_t>(ptehi_value, vsid);
				PTEHI::API::template Set<uint32_t>(ptehi_value, api);
				if(EXEC)
				{
					const TLB_ENTRY<TYPES>& lru_tlb_entry = itlb.GetLRU(va, lru_way);
					TLBMISS::LRU::template Set<uint32_t>(tlbmiss_value, lru_way);
					PTEHI::V::template Set<uint32_t>(ptehi_value, lru_tlb_entry.IsValid());
					cpu->template ThrowException<typename CPU::InstructionTLBMissInterrupt::InstructionTLBMiss>().SetTLBMISS(tlbmiss_value).SetPTEHI(ptehi_value).SetKey(key);
				}
				else
				{
					const TLB_ENTRY<TYPES>& lru_tlb_entry = dtlb.GetLRU(va, lru_way);
					TLBMISS::LRU::template Set<uint32_t>(tlbmiss_value, lru_way);
					PTEHI::V::template Set<uint32_t>(ptehi_value, lru_tlb_entry.IsValid());
					
					if(WRITE)
					{
						cpu->template ThrowException<typename CPU::DataTLBMissOnStoreInterrupt::DataTLBMissOnStore>().SetTLBMISS(tlbmiss_value).SetPTEHI(ptehi_value).SetKey(key);
					}
					else
					{
						cpu->template ThrowException<typename CPU::DataTLBMissOnLoadInterrupt::DataTLBMissOnLoad>().SetTLBMISS(tlbmiss_value).SetPTEHI(ptehi_value).SetKey(key);
					}
				}
				
				return false;
			}
		}
		
		// Hardware Page Table Search
		
		unsigned int h;
		uint32_t hash;
		const uint32_t hash_mask = xaen ? unisim::util::reg::core::MakeMask<uint32_t, 23>()
										: unisim::util::reg::core::MakeMask<uint32_t, 19>();
		PTE<TYPES> pte;
		for(h = 0, hash = (vsid & hash_mask) ^ page_index; h < 2; ++h, hash = (~hash) & hash_mask)
		{
			// Compute the base address of a group of 8 page table entries
			uint32_t pteg_select = xaen ? ((sdr1.template Get<typename SDR1::XA_HTABORG_LSB>() | (sdr1.template Get<typename SDR1::XA_HTABMASK>() & ((hash >> 10) & unisim::util::reg::core::MakeMask<uint32_t, 13>()))) << 10) | (hash & unisim::util::reg::core::MakeMask<uint32_t, 10>())
										: ((sdr1.template Get<typename SDR1::HTABORG_LSB>() | (sdr1.template Get<typename SDR1::HTABMASK>() & ((hash >> 10) & unisim::util::reg::core::MakeMask<uint32_t, 9>()))) << 10) | (hash & unisim::util::reg::core::MakeMask<uint32_t, 10>());
			PHYSICAL_ADDRESS pte_addr = xaen ? (PHYSICAL_ADDRESS(sdr1.template Get<typename SDR1::HTMEXT>()) << 32) | (PHYSICAL_ADDRESS(sdr1.template Get<typename SDR1::XA_HTABORG_MSB>()) << 29) | (PHYSICAL_ADDRESS(pteg_select) << 6)
											: (PHYSICAL_ADDRESS(sdr1.template Get<typename SDR1::HTABORG_MSB>()) << 25) | (PHYSICAL_ADDRESS(pteg_select) << 6);

			// Scan each page table entry in the group
			for(unsigned int i = 0; i < 8; ++i, pte_addr += 8)
			{
				// Load PTE
				if(DEBUG)
				{
					if(unlikely(!cpu->CPU::SuperMSS::DebugDataLoad(pte_addr, pte_addr, pte.Get(), pte.SizeOf(), TYPES::SA_M))) return false;
				}
				else
				{
					if(unlikely(!cpu->CPU::SuperMSS::DataLoad(pte_addr, pte_addr, pte.Get(), pte.SizeOf(), TYPES::SA_M))) return false;
				}
				
				pte.BigEndian2Host();
				
				if(likely(pte.Match(vsid, api, h)))
				{
					// Hit in page table
					if(unlikely(verbose))
					{
						std::ostream& info_stream = logger.DebugInfoStream();
						info_stream << std::hex;
						pte.ShortPrettyPrint(info_stream);
						info_stream << " at physical address 0x" << std::hex << pte_addr << std::dec << " match for effective address 0x" << std::hex << ea << std::dec << std::endl;
					}
					
					if(!DEBUG)
					{
						// Fill TLB
						if(EXEC)
						{
							itlb.Use(itlb.Replace(pte, page_index));
						}
						else
						{
							dtlb.Use(dtlb.Replace(pte, page_index));
						}
					}
					
					if(unlikely((!this->template ControlPageAccess<EXEC, WRITE>(ea, key, pte)))) return false;
					
					bool need_pte_update = !pte.GetR() || (WRITE && !pte.GetC());
					
					pte.Access(); // set accessed bit (R)
					if(WRITE) pte.Change(); // set change bit (C)
					
					storage_attr = pte.GetStorageAttr();
					virt_addr = va;
					PHYSICAL_ADDRESS page_phys_addr = pte.GetPagePhysicalAddress(xaen);
					phys_addr = page_phys_addr | offset;
					size_to_protection_boundary = pte.GetPageSize() - offset;
					
					if(!DEBUG)
					{
						if(unlikely(need_pte_update))
						{
							// Write back PTE
							pte.Host2BigEndian();
							if(unlikely(!cpu->CPU::SuperMSS::DataStore(pte_addr, pte_addr, pte.Get(), pte.SizeOf(), TYPES::SA_M))) return false;
						}
					}
					
					if(unlikely(verbose))
					{
						logger << DebugInfo << "Effective address 0x" << std::hex << ea << std::dec << " (virtual address 0x" << std::hex << virt_addr << std::dec << ") translates to physical address 0x" << std::hex << phys_addr << std::dec << " with storage attributes 0x" << std::hex << +storage_attr << std::dec << EndDebugInfo;
					}
					return true;
				}
			}
		}
		
		// Page Fault
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Page fault for effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
		}
		
		if(!DEBUG)
		{
			if(EXEC)
			{
				cpu->template ThrowException<typename CPU::InstructionStorageInterrupt::PageFault>();
			}
			else
			{
				cpu->template ThrowException<typename CPU::DataStorageInterrupt::PageFault>().SetAddress(ea).SetWrite(WRITE);
			}
		}
	}
	else
	{
		// Address Translation is disabled
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Real addressing mode for effective address 0x" << std::hex << ea << std::dec << EndDebugInfo;
		}
		size_to_protection_boundary = 0;
		virt_addr = ea;
		phys_addr = ea;
		storage_attr = STORAGE_ATTR(TYPES::SA_M | TYPES::SA_G);
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Effective address 0x" << std::hex << ea << std::dec << " translates to physical address 0x" << std::hex << phys_addr << std::dec << " with storage attributes 0x" << std::hex << +storage_attr << std::dec << EndDebugInfo;
		}
		return true;
	}
	
	return false;
}

} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_MMU_HH__
