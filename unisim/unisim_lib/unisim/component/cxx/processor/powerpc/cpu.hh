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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__

#include <unisim/util/reg/core/register.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/nat_sort/nat_sort.hh>
#include <unisim/util/cache/cache.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/synchronizable.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/ppc.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <iosfwd>

#include <unisim/util/backtrace/backtrace.hh>

#ifdef powerpc
#undef powerpc
#endif

#define IF_COND_TRAIT(CLASS_NAME) template <bool, bool __IF_COND_TRAIT_DUMMY__ = true> struct CLASS_NAME {}; template <bool __IF_COND_TRAIT_DUMMY__> struct CLASS_NAME<true, __IF_COND_TRAIT_DUMMY__>
#define ELSE_COND_TRAIT(CLASS_NAME) template <bool __IF_COND_TRAIT_DUMMY__> struct CLASS_NAME<false, __IF_COND_TRAIT_DUMMY__>
#define COND_TRAIT(COND, CLASS_NAME) CLASS_NAME<COND>
	
#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;

using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

enum Model
{
	// Freescale models
	// G1 processors
	MPC601 = 0,
	// G2 processors
	MPC603E,
	MPC604E,
	// G3 processors
	MPC740,
	MPC745,
	MPC750,
	MPC755,
	// G4 processors
	MPC7400,
	MPC7410,
	MPC7441,
	MPC7445,
	MPC7447,
	MPC7447A,
	MPC7448,
	MPC7450,
	MPC7451,
	MPC7455,
	MPC7457,
	// IBM models
	PPC405,
	PPC440,
	// Freescale models
	E200Z425BN3,
	E200Z710N3
};

inline std::ostream& operator << (std::ostream& os, const Model& model)
{
	switch(model)
	{
		case MPC601:      os << "MPC601"; break;
		case MPC603E:     os << "MPC603E"; break;
		case MPC604E:     os << "MPC604E"; break;
		case MPC740:      os << "MPC740"; break;
		case MPC745:      os << "MPC745"; break;
		case MPC750:      os << "MPC750"; break;
		case MPC755:      os << "MPC755"; break;
		case MPC7400:     os << "MPC7400"; break;
		case MPC7410:     os << "MPC7410"; break;
		case MPC7441:     os << "MPC7441"; break;
		case MPC7445:     os << "MPC7445"; break;
		case MPC7447:     os << "MPC7447"; break;
		case MPC7447A:    os << "MPC7447A"; break;
		case MPC7448:     os << "MPC7448"; break;
		case MPC7450:     os << "MPC7450"; break;
		case MPC7451:     os << "MPC7451"; break;
		case MPC7455:     os << "MPC7455"; break;
		case MPC7457:     os << "MPC7457"; break;
		case PPC405:      os << "PPC405"; break;
		case PPC440:      os << "PPC440"; break;
		case E200Z425BN3: os << "e200z425Bn3"; break;
		case E200Z710N3:  os << "e200z710n3"; break;
		default:          os << "unknown"; break;
	}
	return os;
}

using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::RegisterFile;

////////////////////////////////// LOG2 ///////////////////////////////////////

template <unsigned int N>
struct LOG2
{
    static const unsigned int VALUE = LOG2<N / 2>::VALUE + 1;
};

template <>
struct LOG2<1>
{
    static const unsigned int VALUE = 0;
};

 /////////////////////////////// CEIL_LOG2 ////////////////////////////////////

template <unsigned int N>
struct CEIL_LOG2
{
    static const unsigned int VALUE = (N > (1 << LOG2<N>::VALUE)) ? LOG2<N>::VALUE + 1 : LOG2<N>::VALUE;
};

//////////////////////////////////// Field<> //////////////////////////////////

template <typename FIELD, int OFFSET1, int OFFSET2 = -1, unisim::util::reg::core::Access ACCESS = unisim::util::reg::core::SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (31 - OFFSET2) : (31 - OFFSET1)) : (31 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS> Super;
};

//////////////////////////////////// Field64<> //////////////////////////////////

template <typename FIELD, int OFFSET1, int OFFSET2 = -1, unisim::util::reg::core::Access ACCESS = unisim::util::reg::core::SW_RW>
struct Field64 : unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (63 - OFFSET2) : (63 - OFFSET1)) : (63 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS>
{
	typedef unisim::util::reg::core::Field<FIELD, (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (63 - OFFSET2) : (63 - OFFSET1)) : (63 - OFFSET1), (OFFSET2 >= 0) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 1, ACCESS> Super;
};

//////////////////////////////// Register /////////////////////////////////

template <typename REGISTER, unsigned int _SIZE, typename REGISTER_BASE = unisim::util::reg::core::NullRegisterBase>
class Register : public unisim::util::reg::core::Register<REGISTER, _SIZE, unisim::util::reg::core::HW_RW, REGISTER_BASE>
{
public:
	typedef unisim::util::reg::core::Register<REGISTER, _SIZE, unisim::util::reg::core::HW_RW, REGISTER_BASE> Super;
	
	struct  _0 : Field< _0,  0> {};
	struct  _1 : Field< _1,  1> {};
	struct  _2 : Field< _2,  2> {};
	struct  _3 : Field< _3,  3> {};
	struct  _4 : Field< _4,  4> {};
	struct  _5 : Field< _5,  5> {};
	struct  _6 : Field< _6,  6> {};
	struct  _7 : Field< _7,  7> {};
	struct  _8 : Field< _8,  8> {};
	struct  _9 : Field< _9,  9> {};
	struct _10 : Field<_10, 10> {};
	struct _11 : Field<_11, 11> {};
	struct _12 : Field<_12, 12> {};
	struct _13 : Field<_13, 13> {};
	struct _14 : Field<_14, 14> {};
	struct _15 : Field<_15, 15> {};
	struct _16 : Field<_16, 16> {};
	struct _17 : Field<_17, 17> {};
	struct _18 : Field<_18, 18> {};
	struct _19 : Field<_19, 19> {};
	struct _20 : Field<_20, 20> {};
	struct _21 : Field<_21, 21> {};
	struct _22 : Field<_22, 22> {};
	struct _23 : Field<_23, 23> {};
	struct _24 : Field<_24, 24> {};
	struct _25 : Field<_25, 25> {};
	struct _26 : Field<_26, 26> {};
	struct _27 : Field<_27, 27> {};
	struct _28 : Field<_28, 28> {};
	struct _29 : Field<_29, 29> {};
	struct _30 : Field<_30, 30> {};
	struct _31 : Field<_31, 31> {};
	
	Register() : Super() {}
	Register(typename Super::TYPE value) : Super(value) {}
	
	using Super::operator =;
};

///////// Common User land Special Purpose Register Representation ////////

// Integer Exception Register
struct XER : Register<XER, 32>
{
	typedef Register<XER, 32> Super;
	
	struct SO         : Field<SO        ,0> {};      // Summary Overflow
	struct OV         : Field<OV        ,1> {};      // Overflow
	struct CA         : Field<CA        ,2> {};      // Carry
	struct BYTE_COUNT : Field<BYTE_COUNT,25, 31> {}; // Byte count
	
	struct _0_3       : Field<_0_3, 0, 3> {}; // XER[0-3]
	
	typedef FieldSet<SO, OV, CA, BYTE_COUNT> ALL;
	
	XER() : Super() { Init(); }
	XER(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	virtual void Reset() { this->Initialize(0x00000000); }
private:
	void Init()
	{
		this->SetName("XER");
		this->SetDescription("Fixed-Point Exception Register");
		SO::SetName("SO");
		OV::SetName("OV");
		CA::SetName("CA");
		BYTE_COUNT::SetName("BYTE_COUNT");
		SO::SetDescription("Summary Overflow");
		OV::SetDescription("Overflow");
		CA::SetDescription("Carry");
		BYTE_COUNT::SetDescription("Byte count");
	}
};

// Link Register
struct LR : Register<LR, 32>
{
	typedef Register<LR, 32> Super;
	
	struct ALL : Field<ALL, 0, 31> {};
	
	LR() : Super() { Init(); }
	LR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	virtual void Reset() { /* unaffected */ }
private:
	void Init()
	{
		this->SetName("LR");
		this->SetDescription("Link Register");
	}
};

// Count Register
struct CTR : Register<CTR, 32>
{
	typedef Register<CTR, 32> Super;
	
	struct ALL : Field<ALL, 0, 31> {};
	
	CTR() : Super() { Init(); }
	CTR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	virtual void Reset() { /* unaffected */ }
private:
	void Init()
	{
		this->SetName("CTR");
		this->SetDescription("Count Register");
	}
};

// Condition Register
struct CR : Register<CR, 32>
{
public:
	typedef Register<CR, 32> Super;
	
	struct CR0 : Field<CR0,0,3>
	{
		struct LT : Field<LT,0> {};
		struct GT : Field<GT,1> {};
		struct EQ : Field<EQ,2> {};
		struct SO : Field<SO,3> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR1 : Field<CR1,4,7>
	{
		struct LT : Field<LT,4> {};
		struct GT : Field<GT,5> {};
		struct EQ : Field<EQ,6> {};
		struct SO : Field<SO,7> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR2 : Field<CR2,8,11>
	{
		struct LT : Field<LT,8> {};
		struct GT : Field<GT,9> {};
		struct EQ : Field<EQ,10> {};
		struct SO : Field<SO,11> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR3 : Field<CR3,12,15>
	{
		struct LT : Field<LT,12> {};
		struct GT : Field<GT,13> {};
		struct EQ : Field<EQ,14> {};
		struct SO : Field<SO,15> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR4 : Field<CR4,16,19>
	{
		struct LT : Field<LT,16> {};
		struct GT : Field<GT,17> {};
		struct EQ : Field<EQ,18> {};
		struct SO : Field<SO,19> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR5 : Field<CR5,20,23>
	{
		struct LT : Field<LT,20> {};
		struct GT : Field<GT,21> {};
		struct EQ : Field<EQ,22> {};
		struct SO : Field<SO,23> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR6 : Field<CR6,24,27>
	{
		struct LT : Field<LT,24> {};
		struct GT : Field<GT,25> {};
		struct EQ : Field<EQ,26> {};
		struct SO : Field<SO,27> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	struct CR7 : Field<CR7,28,31>
	{
		struct LT : Field<LT,28> {};
		struct GT : Field<GT,29> {};
		struct EQ : Field<EQ,30> {};
		struct SO : Field<SO,31> {};
		typedef FieldSet<LT, GT, EQ, SO> ALL;
	};
	
	typedef FieldSet< CR0::LT, CR0::GT, CR0::EQ, CR0::SO
					, CR1::LT, CR1::GT, CR1::EQ, CR1::SO
					, CR2::LT, CR2::GT, CR2::EQ, CR2::SO
					, CR3::LT, CR3::GT, CR3::EQ, CR3::SO
					, CR4::LT, CR4::GT, CR4::EQ, CR4::SO
					, CR5::LT, CR5::GT, CR5::EQ, CR5::SO
					, CR6::LT, CR6::GT, CR6::EQ, CR6::SO
					, CR7::LT, CR7::GT, CR7::EQ, CR7::SO> ALL;
	
	CR() : Super() { Init(); }
	CR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
private:
	void Init()
	{
		this->SetName("CR");
		this->SetDescription("Condition Register");
		
		CR0::LT::SetName("CR0[LT]"); CR0::GT::SetName("CR0[GT]"); CR0::EQ::SetName("CR0[EQ]"); CR0::SO::SetName("CR0[SO]");
		CR1::LT::SetName("CR1[LT]"); CR1::GT::SetName("CR1[GT]"); CR1::EQ::SetName("CR1[EQ]"); CR1::SO::SetName("CR1[SO]");
		CR2::LT::SetName("CR2[LT]"); CR2::GT::SetName("CR2[GT]"); CR2::EQ::SetName("CR2[EQ]"); CR2::SO::SetName("CR2[SO]");
		CR3::LT::SetName("CR3[LT]"); CR3::GT::SetName("CR3[GT]"); CR3::EQ::SetName("CR3[EQ]"); CR3::SO::SetName("CR3[SO]");
		CR4::LT::SetName("CR4[LT]"); CR4::GT::SetName("CR4[GT]"); CR4::EQ::SetName("CR4[EQ]"); CR4::SO::SetName("CR4[SO]");
		CR5::LT::SetName("CR5[LT]"); CR5::GT::SetName("CR5[GT]"); CR5::EQ::SetName("CR5[EQ]"); CR5::SO::SetName("CR5[SO]");
		CR6::LT::SetName("CR6[LT]"); CR6::GT::SetName("CR6[GT]"); CR6::EQ::SetName("CR6[EQ]"); CR6::SO::SetName("CR6[SO]");
		CR7::LT::SetName("CR7[LT]"); CR7::GT::SetName("CR7[GT]"); CR7::EQ::SetName("CR7[EQ]"); CR7::SO::SetName("CR7[SO]");
		
		CR0::LT::SetDescription("Less Than"); CR0::GT::SetDescription("Greater Than"); CR0::EQ::SetDescription("Equal"); CR0::SO::SetDescription("Summary Overflow");
		CR1::LT::SetDescription("Less Than"); CR1::GT::SetDescription("Greater Than"); CR1::EQ::SetDescription("Equal"); CR1::SO::SetDescription("Summary Overflow");
		CR2::LT::SetDescription("Less Than"); CR2::GT::SetDescription("Greater Than"); CR2::EQ::SetDescription("Equal"); CR2::SO::SetDescription("Summary Overflow");
		CR3::LT::SetDescription("Less Than"); CR3::GT::SetDescription("Greater Than"); CR3::EQ::SetDescription("Equal"); CR3::SO::SetDescription("Summary Overflow");
		CR4::LT::SetDescription("Less Than"); CR4::GT::SetDescription("Greater Than"); CR4::EQ::SetDescription("Equal"); CR4::SO::SetDescription("Summary Overflow");
		CR5::LT::SetDescription("Less Than"); CR5::GT::SetDescription("Greater Than"); CR5::EQ::SetDescription("Equal"); CR5::SO::SetDescription("Summary Overflow");
		CR6::LT::SetDescription("Less Than"); CR6::GT::SetDescription("Greater Than"); CR6::EQ::SetDescription("Equal"); CR6::SO::SetDescription("Summary Overflow");
		CR7::LT::SetDescription("Less Than"); CR7::GT::SetDescription("Greater Than"); CR7::EQ::SetDescription("Equal"); CR7::SO::SetDescription("Summary Overflow");
	}
};

// Floating-Point Status and Control Register
struct FPSCR : Register<FPSCR, 32>
{
public:
	typedef Register<FPSCR, 32> Super;
	
	struct FX     : Field<FX    , 0     > {}; // Floating-point exception summary
	struct FEX    : Field<FEX   , 1     > {}; // Floating-point enabled exception summary
	struct VX     : Field<VX    , 2     > {}; // Floating-point invalid operation exception summary
	struct OX     : Field<OX    , 3     > {}; // Floating-point overflow exception
	struct _0_3   : Field<_0_3  , 0, 3  > {}; // FX, FEX, VX, OX bunch
	struct UX     : Field<UX    , 4     > {}; // Floating-point underflow exception
	struct ZX     : Field<ZX    , 5     > {}; // Floating-point zero divide exception
	struct XX     : Field<XX    , 6     > {}; // Floating-point inexact exception
	struct VXSNAN : Field<VXSNAN, 7     > {}; // Floating-point invalid operation exception for SNaN
	struct VXISI  : Field<VXISI , 8     > {}; // Floating-point invalid operation exception for infinity - infinity
	struct VXIDI  : Field<VXIDI , 9     > {}; // Floating-point invalid operation exception for infinity / infinity
	struct VXZDZ  : Field<VXZDZ , 10    > {}; // Floating-point invalid operation exception for zero / zero
	struct VXIMZ  : Field<VXIMZ , 11    > {}; // Floating-point invalid operation exception for infinity * zero
	struct VXVC   : Field<VXVC  , 12    > {}; // Floating-point invalid operation exception for invalid compare
	struct FR     : Field<FR    , 13    > {}; // Floating-point fraction rounded
	struct FI     : Field<FI    , 14    > {}; // Floating-point fraction inexact
	struct FPRF   : Field<FPRF  , 15, 19>     // Floating-point result flags
	{
		static TYPE          QNAN    () { return 0x11; }
		static TYPE NEGATIVE_INFINITY() { return 0x9; }
		static TYPE NEGATIVE_NORMAL  () { return 0x8; }
		static TYPE NEGATIVE_DENORMAL() { return 0x18; }
		static TYPE NEGATIVE_ZERO    () { return 0x12; }
		static TYPE POSITIVE_ZERO    () { return 0x2; }
		static TYPE POSITIVE_DENORMAL() { return 0x14; }
		static TYPE POSITIVE_NORMAL  () { return 0x4; }
		static TYPE POSITIVE_INFINITY() { return 0x5; }
	};
	struct FPCC   : Field<FPCC  , 16, 19> {}; // Floating-Point Condition Code
	struct VXSOFT : Field<VXSOFT, 21    > {}; // Floating-point invalid operation exception for software request
	struct VXSQRT : Field<VXSQRT, 22    > {}; // Floating-point invalid operation exception for invalid square root
	struct VXCVI  : Field<VXCVI , 23    > {}; // Floating-point invalid operation exception for invalid integer convert
	struct VE     : Field<VE    , 24    > {}; // Floating-point invalid operation exception enable
	struct OE     : Field<OE    , 25    > {}; // IEEE floating-point overflow exception enable
	struct UE     : Field<UE    , 26    > {}; // IEEE floating-point underflow exception enable
	struct ZE     : Field<ZE    , 27    > {}; // IEEE floating-point zero divide exception enable
	struct XE     : Field<XE    , 28    > {}; // Floating-point inexact exception enable
	struct NI     : Field<NI    , 29    > {}; // Floating-point non-IEEE mode
	struct RN     : Field<RN    , 30, 31> {}; // Floating-point rounding control

	typedef FieldSet< FX, FEX, VX, OX, UX, ZX, XX, VXSNAN, VXISI, VXIDI, VXZDZ, VXIMZ, VXVC  
	                , FR, FI, FPRF, VXSOFT, VXSQRT, VXCVI, VE, OE, UE, ZE, XE, NI, RN > ALL;
	
	FPSCR() : Super() { Init(); }
	FPSCR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
	
	void SetInexact(bool i)
	{
		this->template Set<FI>(i);
		if(i)
		{
			SetException(XX());
		}
	}

	template <typename FIELD>
	void SetInvalid(FIELD const& vxfield)
	{
		SetException(vxfield);
		this->template Set<VX>(1);
	}
	
	template <typename FIELD, bool dummy = true> struct EnableFlag { struct FLAG : unisim::util::reg::core::Field<FLAG, 0, 0> {}; };
	template <bool dummy> struct EnableFlag<VX, dummy> { typedef VE FLAG; };
	template <bool dummy> struct EnableFlag<OX, dummy> { typedef OE FLAG; };
	template <bool dummy> struct EnableFlag<UX, dummy> { typedef UE FLAG; };
	template <bool dummy> struct EnableFlag<ZX, dummy> { typedef ZE FLAG; };
	template <bool dummy> struct EnableFlag<XX, dummy> { typedef XE FLAG; };

	template <typename FIELD>
	void SetException(FIELD const&)
	{
		if(not this->Get<FIELD>())
		{
			this->template Set<FX>(1); // Exception bit change from 0 to 1
		}
		this->template Set<FIELD>(1);
		// Check if exception (VE, OE, UE, ZE, XE) is enable (enable bits locations are 22 bit
		// upper than their exception bits counterparts that is VX, OX, UX, ZX, XX)
		if(this->template Get<typename EnableFlag<FIELD>::FLAG>())
		{
			this->Set<FEX>(1);
		}
	}
private:
	void Init()
	{
		this->SetName("FPSCR");
		this->SetDescription("Floating-point Status and Control Register");
		
		FX    ::SetName("FX");     FX    ::SetDescription("Floating-point exception summary");
		FEX   ::SetName("FEX");    FEX   ::SetDescription("Floating-point enabled exception summary");
		VX    ::SetName("VX");     VX    ::SetDescription("Floating-point invalid operation exception summary");
		OX    ::SetName("OX");     OX    ::SetDescription("Floating-point overflow exception");
		UX    ::SetName("UX");     UX    ::SetDescription("Floating-point underflow exception");
		ZX    ::SetName("ZX");     ZX    ::SetDescription("Floating-point zero divide exception");
		XX    ::SetName("XX");     XX    ::SetDescription("Floating-point inexact exception");
		VXSNAN::SetName("VXSNAN"); VXSNAN::SetDescription("Floating-point invalid operation exception for SNaN");
		VXISI ::SetName("VXISI");  VXISI ::SetDescription("Floating-point invalid operation exception for infinity - infinity");
		VXIDI ::SetName("VXIDI");  VXIDI ::SetDescription("Floating-point invalid operation exception for infinity / infinity");
		VXZDZ ::SetName("VXZDZ");  VXZDZ ::SetDescription("Floating-point invalid operation exception for zero / zero");
		VXIMZ ::SetName("VXIMZ");  VXIMZ ::SetDescription("Floating-point invalid operation exception for infinity * zero");
		VXVC  ::SetName("VXVC");   VXVC  ::SetDescription("Floating-point invalid operation exception for invalid compare");
		FR    ::SetName("FR");     FR    ::SetDescription("Floating-point fraction rounded");
		FI    ::SetName("FI");     FI    ::SetDescription("Floating-point fraction inexact");
		FPRF  ::SetName("FPRF");   FPRF  ::SetDescription("Floating-point result flags");
		VXSOFT::SetName("VXSOFT"); VXSOFT::SetDescription("Floating-point invalid operation exception for software request");
		VXSQRT::SetName("VXSQRT"); VXSQRT::SetDescription("Floating-point invalid operation exception for invalid square root");
		VXCVI ::SetName("VXCVI");  VXCVI ::SetDescription("Floating-point invalid operation exception for invalid integer convert");
		VE    ::SetName("VE");     VE    ::SetDescription("Floating-point invalid operation exception enable");
		OE    ::SetName("OE");     OE    ::SetDescription("IEEE floating-point overflow exception enable");
		UE    ::SetName("UE");     UE    ::SetDescription("IEEE floating-point underflow exception enable");
		ZE    ::SetName("ZE");     ZE    ::SetDescription("IEEE floating-point zero divide exception enable");
		XE    ::SetName("XE");     XE    ::SetDescription("Floating-point inexact exception enable");
		NI    ::SetName("NI");     NI    ::SetDescription("Floating-point non-IEEE mode");
		RN    ::SetName("RN");     RN    ::SetDescription("Floating-point rounding control");
	}
};

// Vector Status and Control Register
struct VSCR : Register<VSCR, 32>
{
	typedef Register<VSCR, 32> Super;
	
	struct NJ  : Field<NJ , 15> {}; // Non-Java
	struct SAT : Field<SAT, 31> {}; // Saturation
	
	typedef FieldSet<NJ, SAT> ALL;
	
	VSCR() : Super() { Init(); }
	VSCR(uint32_t _value) : Super(_value) { Init(); }
	using Super::operator =;
private:
	void Init()
	{
		this->SetName("VSCR"); this->SetDescription("Vector Status and Control Register");
		
		NJ ::SetName("NJ");  NJ ::SetDescription("Non-Java");
		SAT::SetName("SAT"); SAT::SetDescription("Saturation");
	}
};

//////////////////////// Floating-Point Register //////////////////////////

struct FPR : SoftDouble
{
	typedef SoftDouble Super;
	
	FPR() : Super() {}
	
	void Init(unsigned int reg_num)
	{
		std::stringstream sstr;
		sstr << "f" << reg_num;
		unisim::util::reg::core::PropertyRegistry::SetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_NAME, (intptr_t) this, sstr.str());
		
		unisim::util::reg::core::PropertyRegistry::SetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_DESC, (intptr_t) this, "Floating-Point Register");
	}
	
	unisim::service::interfaces::Register *CreateRegisterInterface()
	{
		return new FloatingPointRegisterInterface(GetName().c_str(), GetDescription().c_str(), this);
	}
	
	const std::string& GetName() const { return unisim::util::reg::core::PropertyRegistry::GetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_NAME, (intptr_t) this); }
	
	const std::string& GetDescription() const { return unisim::util::reg::core::PropertyRegistry::GetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_DESC, (intptr_t) this); }
	
	using Super::operator =;
};

////////////////////////// Vector Register ////////////////////////////////////

struct VR
{
	VR() : value() {}
	
	void Init(unsigned int reg_num)
	{
		std::stringstream sstr;
		sstr << "v" << reg_num;
		unisim::util::reg::core::PropertyRegistry::SetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_NAME, (intptr_t) this, sstr.str());
		
		unisim::util::reg::core::PropertyRegistry::SetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_DESC, (intptr_t) this, "Vector Register");
	}

	unisim::service::interfaces::Register *CreateRegisterInterface()
	{
		return new unisim::util::debug::SimpleRegister<value_t>(GetName().c_str(), GetDescription().c_str(), &value);
	}
	
	const std::string& GetName() const { return unisim::util::reg::core::PropertyRegistry::GetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_NAME, (intptr_t) this); }
	
	const std::string& GetDescription() const { return unisim::util::reg::core::PropertyRegistry::GetStringProperty(unisim::util::reg::core::PropertyRegistry::EL_REGISTER, unisim::util::reg::core::PropertyRegistry::STR_PROP_DESC, (intptr_t) this); }
private:
	struct value_t
	{
		uint32_t w[4];
	};
	value_t value;
};

///////////////////////////// Segment Register ////////////////////////////////

struct SR : Register<SR, 32>
{
	typedef Register<SR, 32> Super;
	
	struct T          : Field<T         , 0     > {}; // Format (T=0:memory segment, T=1:direct-store segment)
	struct Ks         : Field<Ks        , 1     > {}; // Supervisor-state protection key
	struct Kp         : Field<Kp        , 2     > {}; // User-state protection key
	struct N          : Field<N         , 3     > {}; // No-execute protection bit (when T=0)
	struct VSID       : Field<VSID      , 8 , 31> {}; // Virtual segment ID (when T=0)
	struct BUID       : Field<BUID      , 3 , 11> {}; // Bus unit ID (when T=1)
	struct CNTLR_SPEC : Field<CNTLR_SPEC, 12, 31> {}; // Device-specific data for I/O controller (when T=1)
	
	typedef FieldSet< T, Ks, Kp, N, VSID, BUID, CNTLR_SPEC > ALL;
	
	SR() : Super() {}
	SR(unsigned int reg_num) : Super() { Init(reg_num); }
	SR(unsigned int reg_num, uint32_t _value) : Super(_value) { Init(reg_num); }
	
	void Init(unsigned int reg_num)
	{
		std::stringstream name_sstr;
		name_sstr << "SR" << reg_num;
		
		std::stringstream desc_sstr;
		desc_sstr << "Segment Register " << reg_num;
		
		this->SetName(name_sstr.str());
		this->SetDescription(desc_sstr.str());
		
		T         ::SetName("T");          T         ::SetDescription("Format (T=0:memory segment, T=1:direct-store segment)");
		Ks        ::SetName("Ks");         Ks        ::SetDescription("Supervisor-state protection key");
		Kp        ::SetName("Kp");         Kp        ::SetDescription("User-state protection key");
		N         ::SetName("N");          N         ::SetDescription("No-execute protection bit (when T=0)");
		VSID      ::SetName("VSID");       VSID      ::SetDescription("Virtual segment ID (when T=0)");
		BUID      ::SetName("BUID");       BUID      ::SetDescription("Bus unit ID (when T=1)");
		CNTLR_SPEC::SetName("CNTLR_SPEC"); CNTLR_SPEC::SetDescription("Device-specific data for I/O controller (when T=1)");
	}
	
	bool NeedCheckForOverlappingBitFields() const { return false; }
	
	using Super::operator =;
};

///////////////////////////// TypeForByteSize<> ///////////////////////////////

template <unsigned int BYTE_SIZE> struct TypeForByteSize {};
template <> struct TypeForByteSize<1> { typedef uint8_t TYPE; };
template <> struct TypeForByteSize<2> { typedef uint16_t TYPE; };
template <> struct TypeForByteSize<3> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<4> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<5> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<6> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<7> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<8> { typedef uint64_t TYPE; };

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> struct TypeForBitSize
{
	static const unsigned int BYTE_SIZE = (SIZE + 7) / 8;
	typedef typename TypeForByteSize<BYTE_SIZE>::TYPE TYPE;
	static const TYPE MASK = (SIZE == (8 * BYTE_SIZE)) ? (~TYPE(0)) : ((TYPE(1) << SIZE) - 1);
};

/////////////////////////////////// CPU ///////////////////////////////////////

template <typename TYPES, typename CONFIG>
class CPU
	: public unisim::util::cache::MemorySubSystem<TYPES, typename CONFIG::CPU>
	, public unisim::kernel::Client<typename unisim::service::interfaces::Memory<typename TYPES::PHYSICAL_ADDRESS> >
	, public unisim::kernel::Client<typename unisim::service::interfaces::SymbolTableLookup<typename TYPES::EFFECTIVE_ADDRESS> >
	, public unisim::kernel::Client<typename unisim::service::interfaces::DebugYielding>
	, public unisim::kernel::Client<typename unisim::service::interfaces::MemoryAccessReporting<typename TYPES::EFFECTIVE_ADDRESS> >
	, public unisim::kernel::Client<typename unisim::service::interfaces::TrapReporting>
	, public unisim::kernel::Client<typename unisim::service::interfaces::LinuxOS>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Memory<typename TYPES::EFFECTIVE_ADDRESS> >
	, public unisim::kernel::Service<typename unisim::service::interfaces::MemoryAccessReportingControl>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Synchronizable>
	, public unisim::kernel::Service<typename unisim::service::interfaces::MemoryInjection<typename TYPES::EFFECTIVE_ADDRESS> >
{
public:
	typedef typename unisim::util::cache::MemorySubSystem<TYPES, typename CONFIG::CPU> SuperMSS;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	static const Model MODEL = CONFIG::MODEL;
	static const unsigned int DATA_FSB_WIDTH = CONFIG::DATA_FSB_WIDTH;
	
	/////////////////////////// service imports ///////////////////////////////

	unisim::kernel::ServiceImport<typename unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> > memory_import;
	unisim::kernel::ServiceImport<typename unisim::service::interfaces::SymbolTableLookup<EFFECTIVE_ADDRESS> > symbol_table_lookup_import;
	unisim::kernel::ServiceImport<typename unisim::service::interfaces::DebugYielding> debug_yielding_import;
	unisim::kernel::ServiceImport<typename unisim::service::interfaces::MemoryAccessReporting<EFFECTIVE_ADDRESS> > memory_access_reporting_import;
	unisim::kernel::ServiceImport<typename unisim::service::interfaces::TrapReporting> trap_reporting_import;
	unisim::kernel::ServiceImport<typename unisim::service::interfaces::LinuxOS> linux_os_import;
	
	/////////////////////////// service exports ///////////////////////////////

	unisim::kernel::ServiceExport<typename unisim::service::interfaces::Memory<EFFECTIVE_ADDRESS> > memory_export;
	unisim::kernel::ServiceExport<typename unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_export;
	unisim::kernel::ServiceExport<typename unisim::service::interfaces::Registers> registers_export;
	unisim::kernel::ServiceExport<typename unisim::service::interfaces::Synchronizable> synchronizable_export;
	unisim::kernel::ServiceExport<typename unisim::service::interfaces::MemoryInjection<EFFECTIVE_ADDRESS> > memory_injection_export;

	////////////////////////////// constructor ////////////////////////////////

	CPU(const char *name, unisim::kernel::Object *parent = 0);

	/////////////////////////////// destructor ////////////////////////////////

	virtual ~CPU();
	
	////////////////////////////// setup hooks ////////////////////////////////

	virtual bool BeginSetup();
	virtual bool EndSetup();
	
	//////////////////////////////// Reset ////////////////////////////////////

	void Reset();
	
	//////////////////////////// Logger streams ///////////////////////////////
	
	std::ostream& GetDebugInfoStream() ALWAYS_INLINE { return logger.DebugInfoStream(); }
	std::ostream& GetDebugWarningStream() ALWAYS_INLINE { return logger.DebugWarningStream(); }
	std::ostream& GetDebugErrorStream() ALWAYS_INLINE { return logger.DebugErrorStream(); }
	
	/////////////// unisim::service::interfaces::Memory<> /////////////////////
	
	virtual void ResetMemory();
	virtual bool ReadMemory(EFFECTIVE_ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(EFFECTIVE_ADDRESS addr, const void *buffer, uint32_t size);

	//////////////// unisim::service::interfaces::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

	//////////////// unisim::service::interfaces::DebugControl /////////////////
	
	virtual void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report);
	
	/////////////// unisim::service::interfaces::Synchronizable ////////////////
	
	virtual void Synchronize();

	//////////// unisim::service::interfaces::MemoryInjection<> ////////////////
	
	virtual bool InjectReadMemory(EFFECTIVE_ADDRESS ea, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(EFFECTIVE_ADDRESS ea, const void *buffer, uint32_t size);

	///////////// External Device Control Register access methods /////////////
		
	virtual void DCRRead(unsigned int dcrn, uint32_t& value) { value = 0; }
	virtual void DCRWrite(unsigned int dcrn, uint32_t value) {}
	
	/////////////////////////// Internal timers ///////////////////////////////
	
	virtual void RunInternalTimers() {}

	///////////// Interface with .isa behavioral description files ////////////
	
	// see also ThrowException<> for interrupt
	
	inline uint32_t GetGPR(unsigned int n) const ALWAYS_INLINE { return gpr[n]; }
	inline void SetGPR(unsigned int n, uint32_t value) ALWAYS_INLINE { gpr[n] = value; }

	inline LR& GetLR() ALWAYS_INLINE { return lr; }
	inline CTR& GetCTR() ALWAYS_INLINE { return ctr; }
	inline XER& GetXER() ALWAYS_INLINE { return xer; }
	inline CR& GetCR() ALWAYS_INLINE { return cr; }
	inline FPSCR& GetFPSCR() { return fpu.GetFPSCR(); }
	inline FPR& GetFPR(unsigned int n) { return fpu.GetFPR(n); }
	inline void SetFPR(unsigned int n, const SoftDouble& v) { fpu.SetFPR(n, v); }
	inline bool CheckFloatingPointException() { return fpu.CheckFloatingPointException(static_cast<typename CONFIG::CPU *>(this)); }
	inline VSCR& GetVSCR() { return vector_unit.GetVSCR(); }
	inline VR& GetVR(unsigned int n) { return vector_unit.GetVR(n); }
	
	inline EFFECTIVE_ADDRESS GetCIA() const ALWAYS_INLINE { return cia; }
	inline void SetCIA(uint32_t value) ALWAYS_INLINE { cia = value; }
	inline EFFECTIVE_ADDRESS GetNIA() const ALWAYS_INLINE { return nia; }
	inline void SetNIA(EFFECTIVE_ADDRESS value) ALWAYS_INLINE { nia = value; }
	inline void Branch(EFFECTIVE_ADDRESS value) ALWAYS_INLINE { nia = value; }

	bool MoveFromSPR(unsigned int n, uint32_t& value);
	bool MoveToSPR(unsigned int n, uint32_t value);
	bool MoveFromTBR(unsigned int n, uint32_t& value);
	bool MoveToTBR(unsigned int n, uint32_t value);
	bool MoveFromDCR(unsigned int n, uint32_t& value);
	bool MoveToDCR(unsigned int n, uint32_t value);
	bool MoveFromPMR(unsigned int n, uint32_t& value);
	bool MoveToPMR(unsigned int n, uint32_t value);
	bool MoveFromTMR(unsigned int n, uint32_t& value);
	bool MoveToTMR(unsigned int n, uint32_t value);

	template <bool DEBUG, bool EXEC, bool WRITE>
	inline bool Translate(EFFECTIVE_ADDRESS ea, EFFECTIVE_ADDRESS& size_to_protection_boundary, ADDRESS& virt_addr, PHYSICAL_ADDRESS& phys_addr, STORAGE_ATTR& storage_attr);

	virtual bool DataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr, bool rwitm);
	virtual bool DataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool InstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	virtual bool DebugDataBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugDataBusWrite(PHYSICAL_ADDRESS addr, const void *buffer, unsigned int size, STORAGE_ATTR storage_attr);
	virtual bool DebugInstructionBusRead(PHYSICAL_ADDRESS addr, void *buffer, unsigned int size, STORAGE_ATTR storage_attr);

	template <typename T, bool REVERSE, unisim::util::endian::endian_type ENDIAN> void ConvertDataLoadStoreEndian(T& value, STORAGE_ATTR storage_attr);
	template <typename T> EFFECTIVE_ADDRESS MungEffectiveAddress(EFFECTIVE_ADDRESS ea) { return ea; }

	template <typename T, bool REVERSE, bool CONVERT_ENDIAN, unisim::util::endian::endian_type ENDIAN> bool DataLoad(T& value, EFFECTIVE_ADDRESS ea);
	template <typename T, bool REVERSE, bool CONVERT_ENDIAN, unisim::util::endian::endian_type ENDIAN> bool DataStore(T& value, EFFECTIVE_ADDRESS ea);
	
	bool DebugDataLoad(EFFECTIVE_ADDRESS ea, void *buffer, unsigned int size);
	bool DebugDataStore(EFFECTIVE_ADDRESS ea, const void *buffer, unsigned int size);

	bool Int8Load(unsigned int rd, EFFECTIVE_ADDRESS ea);
	bool Int16Load(unsigned int rd, EFFECTIVE_ADDRESS ea);
	bool SInt16Load(unsigned int rd, EFFECTIVE_ADDRESS ea);
	bool Int32Load(unsigned int rd, EFFECTIVE_ADDRESS ea);
	bool Int16LoadByteReverse(unsigned int rd, EFFECTIVE_ADDRESS ea);
	bool Int32LoadByteReverse(unsigned int rd, EFFECTIVE_ADDRESS ea);
	bool IntLoadMSBFirst(unsigned int rd, EFFECTIVE_ADDRESS ea, unsigned int size);
	bool Int8Store(unsigned int rs, EFFECTIVE_ADDRESS ea);
	bool Int16Store(unsigned int rs, EFFECTIVE_ADDRESS ea);
	bool Int32Store(unsigned int rs, EFFECTIVE_ADDRESS ea);
	bool Int16StoreByteReverse(unsigned int rs, EFFECTIVE_ADDRESS ea);
	bool Int32StoreByteReverse(unsigned int rs, EFFECTIVE_ADDRESS ea);
	bool IntStoreMSBFirst(unsigned int rs, EFFECTIVE_ADDRESS ea, unsigned int size);
	template <typename REGISTER> bool SpecialLoad(REGISTER& reg, EFFECTIVE_ADDRESS ea);
	template <typename REGISTER> bool SpecialStore(const REGISTER& reg, EFFECTIVE_ADDRESS ea);
	bool Fp32Load(unsigned int fd, EFFECTIVE_ADDRESS ea) { return fpu.Fp32Load(static_cast<typename CONFIG::CPU *>(this), fd, ea); }
	bool Fp64Load(unsigned int fd, EFFECTIVE_ADDRESS ea) { return fpu.Fp64Load(static_cast<typename CONFIG::CPU *>(this), fd, ea); }
	bool Fp32Store(unsigned int fs, EFFECTIVE_ADDRESS ea) { return fpu.Fp32Store(static_cast<typename CONFIG::CPU *>(this), fs, ea); }
	bool Fp64Store(unsigned int fs, EFFECTIVE_ADDRESS ea) { return fpu.Fp64Store(static_cast<typename CONFIG::CPU *>(this), fs, ea); }
	bool FpStoreLSW(unsigned int fs, EFFECTIVE_ADDRESS ea) { return fpu.FpStoreLSW(static_cast<typename CONFIG::CPU *>(this), fs, ea); }
	
	std::string GetObjectFriendlyName(EFFECTIVE_ADDRESS addr);
	void LogLinuxSystemCall();
	
	void Trap();

	///////////////////////////////////////////////////////////////////////////
protected:
	bool CheckInt16LoadAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckSInt16LoadAlignment(EFFECTIVE_ADDRESS ea) { return static_cast<typename CONFIG::CPU *>(this)->CheckInt16LoadAlignment(ea); }
	bool CheckInt32LoadAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckInt16LoadByteReverseAlignment(EFFECTIVE_ADDRESS ea) { return static_cast<typename CONFIG::CPU *>(this)->CheckInt16LoadAlignment(ea); }
	bool CheckInt32LoadByteReverseAlignment(EFFECTIVE_ADDRESS ea) { return static_cast<typename CONFIG::CPU *>(this)->CheckInt32LoadAlignment(ea); }
	bool CheckInt16StoreAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckInt32StoreAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckInt16StoreByteReverseAlignment(EFFECTIVE_ADDRESS ea) { return static_cast<typename CONFIG::CPU *>(this)->CheckInt16StoreAlignment(ea); }
	bool CheckInt32StoreByteReverseAlignment(EFFECTIVE_ADDRESS ea) { return static_cast<typename CONFIG::CPU *>(this)->CheckInt32StoreAlignment(ea); }
	template <typename REGISTER> bool CheckSpecialLoadAlignment(EFFECTIVE_ADDRESS ea)
	{
		switch(REGISTER::SIZE)
		{
			case 32: return static_cast<typename CONFIG::CPU *>(this)->CheckInt32LoadAlignment(ea);
		}
		return true;
		
	}
	template <typename REGISTER> bool CheckSpecialStoreAlignment(EFFECTIVE_ADDRESS ea)
	{
		switch(REGISTER::SIZE)
		{
			case 32: return static_cast<typename CONFIG::CPU *>(this)->CheckInt32StoreAlignment(ea);
		}
		return true;
		
	}
	bool CheckFp32LoadAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckFp64LoadAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckFp32StoreAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckFp64StoreAlignment(EFFECTIVE_ADDRESS ea) { return true; }
	bool CheckFpStoreLSWAlignment(EFFECTIVE_ADDRESS ea) { return true; }

protected:
	
	///////////////////////////////// InterruptBase ///////////////////////////////////

	class InterruptBase
	{
	public:
		InterruptBase()
			: trap(false)
			, trapped(false)
		{
		}
		
		virtual ~InterruptBase() {}
		virtual void ProcessInterrupt(typename CONFIG::CPU *cpu) = 0;
		virtual const char *GetInterruptName() const { return "Unknown Interrupt"; }
		virtual unsigned int GetOffset() const { return 0; }
		
		bool Trap() const { return trap; }
		void Trapped(bool v) { trapped = v; }
		bool Trapped() const { return trapped; }
	protected:
		bool trap;
		bool trapped;
	};

	//////////////////////////////// Exception<> ///////////////////////////////////////

	template <typename _INTERRUPT>
	class Exception
	{
	public:
		typedef _INTERRUPT INTERRUPT;
	};
	
	//////////////////////// Interrupt<> ///////////////////////////

	template <typename INTERRUPT, unsigned int _OFFSET>
	class Interrupt : public InterruptBase
	{
	public:
		static const unsigned int OFFSET = _OFFSET;
		
		Interrupt(typename CONFIG::CPU *cpu)
			: param_trap((std::string("trap-") + INTERRUPT::GetId()).c_str(), cpu, this->trap, (std::string("enable/disable trap (in debugger) of ") + INTERRUPT::GetName()).c_str())
		{
			INTERRUPT::ALL::InstallInterrupt(cpu, this);
		}
		
		template <typename T> static T GetMask() { return INTERRUPT::ALL::template GetMask<T>(); }
		
		virtual void ProcessInterrupt(typename CONFIG::CPU *cpu) { cpu->ProcessInterrupt(static_cast<INTERRUPT *>(this)); }

		virtual unsigned int GetOffset() const { return _OFFSET; }
		
		virtual const char *GetInterruptName() const { return INTERRUPT::GetName(); }
	private:
		unisim::kernel::variable::Parameter<bool> param_trap;
	};

	/////////////////////// InterruptWithAddress<> ////////////////////////////

	template <typename INTERRUPT, unsigned int _OFFSET>
	struct InterruptWithAddress : public Interrupt<INTERRUPT, _OFFSET>
	{
		typedef Interrupt<INTERRUPT, _OFFSET> Super;
		InterruptWithAddress(typename CONFIG::CPU *cpu) : Super(cpu) {}
		INTERRUPT& SetAddress(EFFECTIVE_ADDRESS _addr) { addr = _addr; has_addr = true; return *static_cast<INTERRUPT *>(this); }
		void ClearAddress() { has_addr = false; }
		bool HasAddress() const { return has_addr; }
		EFFECTIVE_ADDRESS GetAddress() const { return addr; }
		
	private:
		bool has_addr;
		EFFECTIVE_ADDRESS addr;
	};
	
	

	////////////////////////// NullException ///////////////////////////

	struct NullException
	{
	};
	
	///////////////////////// ExceptionSet<> ///////////////////////////

	template < typename E0                 , typename E1  = NullException, typename E2  = NullException, typename E3  = NullException
	         , typename E4  = NullException, typename E5  = NullException, typename E6  = NullException, typename E7  = NullException
	         , typename E8  = NullException, typename E9  = NullException, typename E10 = NullException, typename E11 = NullException
	         , typename E12 = NullException, typename E13 = NullException, typename E14 = NullException, typename E15 = NullException
	         , typename E16 = NullException, typename E17 = NullException, typename E18 = NullException, typename E19 = NullException
	         , typename E20 = NullException, typename E21 = NullException, typename E22 = NullException, typename E23 = NullException
	         , typename E24 = NullException, typename E25 = NullException, typename E26 = NullException, typename E27 = NullException
	         , typename E28 = NullException, typename E29 = NullException, typename E30 = NullException, typename E31 = NullException
	         , typename E32 = NullException, typename E33 = NullException, typename E34 = NullException, typename E35 = NullException
	         , typename E36 = NullException, typename E37 = NullException, typename E38 = NullException, typename E39 = NullException
	         , typename E40 = NullException, typename E41 = NullException, typename E42 = NullException, typename E43 = NullException
	         , typename E44 = NullException, typename E45 = NullException, typename E46 = NullException, typename E47 = NullException
	         , typename E48 = NullException, typename E49 = NullException, typename E50 = NullException, typename E51 = NullException
	         , typename E52 = NullException, typename E53 = NullException, typename E54 = NullException, typename E55 = NullException
	         , typename E56 = NullException, typename E57 = NullException, typename E58 = NullException, typename E59 = NullException
	         , typename E60 = NullException, typename E61 = NullException, typename E62 = NullException, typename E63 = NullException >
	class ExceptionSet
	{
	public:
		template <typename T> static T GetMask()
		{
			return CPU::template GetExceptionMask<E0 , T>() | CPU::template GetExceptionMask<E1, T>()  | CPU::template GetExceptionMask<E2 , T>() | CPU::template GetExceptionMask<E3 , T>()
			     | CPU::template GetExceptionMask<E4 , T>() | CPU::template GetExceptionMask<E5 , T>() | CPU::template GetExceptionMask<E6 , T>() | CPU::template GetExceptionMask<E7 , T>()
			     | CPU::template GetExceptionMask<E8 , T>() | CPU::template GetExceptionMask<E9 , T>() | CPU::template GetExceptionMask<E10, T>() | CPU::template GetExceptionMask<E11, T>()
			     | CPU::template GetExceptionMask<E12, T>() | CPU::template GetExceptionMask<E13, T>() | CPU::template GetExceptionMask<E14, T>() | CPU::template GetExceptionMask<E15, T>()
			     | CPU::template GetExceptionMask<E16, T>() | CPU::template GetExceptionMask<E17, T>() | CPU::template GetExceptionMask<E18, T>() | CPU::template GetExceptionMask<E19, T>()
			     | CPU::template GetExceptionMask<E20, T>() | CPU::template GetExceptionMask<E21, T>() | CPU::template GetExceptionMask<E22, T>() | CPU::template GetExceptionMask<E23, T>()
			     | CPU::template GetExceptionMask<E24, T>() | CPU::template GetExceptionMask<E25, T>() | CPU::template GetExceptionMask<E26, T>() | CPU::template GetExceptionMask<E27, T>()
			     | CPU::template GetExceptionMask<E28, T>() | CPU::template GetExceptionMask<E29, T>() | CPU::template GetExceptionMask<E30, T>() | CPU::template GetExceptionMask<E31, T>()
			     | CPU::template GetExceptionMask<E32, T>() | CPU::template GetExceptionMask<E33, T>() | CPU::template GetExceptionMask<E34, T>() | CPU::template GetExceptionMask<E35, T>()
			     | CPU::template GetExceptionMask<E36, T>() | CPU::template GetExceptionMask<E37, T>() | CPU::template GetExceptionMask<E38, T>() | CPU::template GetExceptionMask<E39, T>()
			     | CPU::template GetExceptionMask<E40, T>() | CPU::template GetExceptionMask<E41, T>() | CPU::template GetExceptionMask<E42, T>() | CPU::template GetExceptionMask<E43, T>()
			     | CPU::template GetExceptionMask<E44, T>() | CPU::template GetExceptionMask<E45, T>() | CPU::template GetExceptionMask<E46, T>() | CPU::template GetExceptionMask<E47, T>()
			     | CPU::template GetExceptionMask<E48, T>() | CPU::template GetExceptionMask<E49, T>() | CPU::template GetExceptionMask<E50, T>() | CPU::template GetExceptionMask<E51, T>()
			     | CPU::template GetExceptionMask<E52, T>() | CPU::template GetExceptionMask<E53, T>() | CPU::template GetExceptionMask<E54, T>() | CPU::template GetExceptionMask<E55, T>()
			     | CPU::template GetExceptionMask<E56, T>() | CPU::template GetExceptionMask<E57, T>() | CPU::template GetExceptionMask<E58, T>() | CPU::template GetExceptionMask<E59, T>()
			     | CPU::template GetExceptionMask<E60, T>() | CPU::template GetExceptionMask<E61, T>() | CPU::template GetExceptionMask<E62, T>() | CPU::template GetExceptionMask<E63, T>();
		}
		
		static void InstallInterrupt(typename CONFIG::CPU *cpu, InterruptBase *interrupt)
		{
			if(typeid(E0 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E0 >());
			if(typeid(E1 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E1 >());
			if(typeid(E2 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E2 >());
			if(typeid(E3 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E3 >());
			if(typeid(E4 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E4 >());
			if(typeid(E5 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E5 >());
			if(typeid(E6 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E6 >());
			if(typeid(E7 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E7 >());
			if(typeid(E8 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E8 >());
			if(typeid(E9 ) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E9 >());
			if(typeid(E10) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E10>());
			if(typeid(E11) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E11>());
			if(typeid(E12) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E12>());
			if(typeid(E13) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E13>());
			if(typeid(E14) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E14>());
			if(typeid(E15) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E15>());
			if(typeid(E16) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E16>());
			if(typeid(E17) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E17>());
			if(typeid(E18) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E18>());
			if(typeid(E19) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E19>());
			if(typeid(E20) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E20>());
			if(typeid(E21) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E21>());
			if(typeid(E22) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E22>());
			if(typeid(E23) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E23>());
			if(typeid(E24) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E24>());
			if(typeid(E25) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E25>());
			if(typeid(E26) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E26>());
			if(typeid(E27) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E27>());
			if(typeid(E28) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E28>());
			if(typeid(E29) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E29>());
			if(typeid(E30) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E30>());
			if(typeid(E31) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E31>());
			if(typeid(E32) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E32>());
			if(typeid(E33) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E33>());
			if(typeid(E34) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E34>());
			if(typeid(E35) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E35>());
			if(typeid(E36) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E36>());
			if(typeid(E37) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E37>());
			if(typeid(E38) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E38>());
			if(typeid(E39) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E39>());
			if(typeid(E40) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E40>());
			if(typeid(E41) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E41>());
			if(typeid(E42) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E42>());
			if(typeid(E43) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E43>());
			if(typeid(E44) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E44>());
			if(typeid(E45) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E45>());
			if(typeid(E46) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E46>());
			if(typeid(E47) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E47>());
			if(typeid(E48) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E48>());
			if(typeid(E49) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E49>());
			if(typeid(E50) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E50>());
			if(typeid(E51) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E51>());
			if(typeid(E52) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E52>());
			if(typeid(E53) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E53>());
			if(typeid(E54) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E54>());
			if(typeid(E55) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E55>());
			if(typeid(E56) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E56>());
			if(typeid(E57) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E57>());
			if(typeid(E58) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E58>());
			if(typeid(E59) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E59>());
			if(typeid(E60) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E60>());
			if(typeid(E61) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E61>());
			if(typeid(E62) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E62>());
			if(typeid(E63) == typeid(NullException)) return; cpu->InstallInterrupt(interrupt, CPU::template GetExceptionPriority<E63>());
		}
	};

	////////////////////////// ExceptionPrioritySet<> //////////////////////////////

	struct NullException0  {};
	struct NullException1  {};
	struct NullException2  {};
	struct NullException3  {};
	struct NullException4  {};
	struct NullException5  {};
	struct NullException6  {};
	struct NullException7  {};
	struct NullException8  {};
	struct NullException9  {};
	struct NullException10 {};
	struct NullException11 {};
	struct NullException12 {};
	struct NullException13 {};
	struct NullException14 {};
	struct NullException15 {};
	struct NullException16 {};
	struct NullException17 {};
	struct NullException18 {};
	struct NullException19 {};
	struct NullException20 {};
	struct NullException21 {};
	struct NullException22 {};
	struct NullException23 {};
	struct NullException24 {};
	struct NullException25 {};
	struct NullException26 {};
	struct NullException27 {};
	struct NullException28 {};
	struct NullException29 {};
	struct NullException30 {};
	struct NullException31 {};
	struct NullException32 {};
	struct NullException33 {};
	struct NullException34 {};
	struct NullException35 {};
	struct NullException36 {};
	struct NullException37 {};
	struct NullException38 {};
	struct NullException39 {};
	struct NullException40 {};
	struct NullException41 {};
	struct NullException42 {};
	struct NullException43 {};
	struct NullException44 {};
	struct NullException45 {};
	struct NullException46 {};
	struct NullException47 {};
	struct NullException48 {};
	struct NullException49 {};
	struct NullException50 {};
	struct NullException51 {};
	struct NullException52 {};
	struct NullException53 {};
	struct NullException54 {};
	struct NullException55 {};
	struct NullException56 {};
	struct NullException57 {};
	struct NullException58 {};
	struct NullException59 {};
	struct NullException60 {};
	struct NullException61 {};
	struct NullException62 {};
	struct NullException63 {};
	
	template < typename E0  = NullException0 , typename E1  = NullException1 , typename E2  = NullException2 , typename E3  = NullException3 
	         , typename E4  = NullException4 , typename E5  = NullException5 , typename E6  = NullException6 , typename E7  = NullException7 
	         , typename E8  = NullException8 , typename E9  = NullException9 , typename E10 = NullException10, typename E11 = NullException11
	         , typename E12 = NullException12, typename E13 = NullException13, typename E14 = NullException14, typename E15 = NullException15
	         , typename E16 = NullException16, typename E17 = NullException17, typename E18 = NullException18, typename E19 = NullException19
	         , typename E20 = NullException20, typename E21 = NullException21, typename E22 = NullException22, typename E23 = NullException23
	         , typename E24 = NullException24, typename E25 = NullException25, typename E26 = NullException26, typename E27 = NullException27
	         , typename E28 = NullException28, typename E29 = NullException29, typename E30 = NullException30, typename E31 = NullException31
	         , typename E32 = NullException32, typename E33 = NullException33, typename E34 = NullException34, typename E35 = NullException35
	         , typename E36 = NullException36, typename E37 = NullException37, typename E38 = NullException38, typename E39 = NullException39
	         , typename E40 = NullException40, typename E41 = NullException41, typename E42 = NullException42, typename E43 = NullException43
	         , typename E44 = NullException44, typename E45 = NullException45, typename E46 = NullException46, typename E47 = NullException47
	         , typename E48 = NullException48, typename E49 = NullException49, typename E50 = NullException50, typename E51 = NullException51
	         , typename E52 = NullException52, typename E53 = NullException53, typename E54 = NullException54, typename E55 = NullException55
	         , typename E56 = NullException56, typename E57 = NullException57, typename E58 = NullException58, typename E59 = NullException59
	         , typename E60 = NullException60, typename E61 = NullException61, typename E62 = NullException62, typename E63 = NullException63 >
	struct ExceptionPrioritySet
	{
		template <typename E, bool dummy = true> struct ExceptionPriority { static const unsigned int PRIORITY = 0 ; static const uint64_t MASK = 0; };
		
		template <bool dummy> struct ExceptionPriority<E0 , dummy> { static const unsigned int PRIORITY = 0 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E1 , dummy> { static const unsigned int PRIORITY = 1 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E2 , dummy> { static const unsigned int PRIORITY = 2 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E3 , dummy> { static const unsigned int PRIORITY = 3 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E4 , dummy> { static const unsigned int PRIORITY = 4 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E5 , dummy> { static const unsigned int PRIORITY = 5 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E6 , dummy> { static const unsigned int PRIORITY = 6 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E7 , dummy> { static const unsigned int PRIORITY = 7 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E8 , dummy> { static const unsigned int PRIORITY = 8 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E9 , dummy> { static const unsigned int PRIORITY = 9 ; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E10, dummy> { static const unsigned int PRIORITY = 10; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E11, dummy> { static const unsigned int PRIORITY = 11; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E12, dummy> { static const unsigned int PRIORITY = 12; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E13, dummy> { static const unsigned int PRIORITY = 13; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E14, dummy> { static const unsigned int PRIORITY = 14; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E15, dummy> { static const unsigned int PRIORITY = 15; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E16, dummy> { static const unsigned int PRIORITY = 16; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E17, dummy> { static const unsigned int PRIORITY = 17; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E18, dummy> { static const unsigned int PRIORITY = 18; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E19, dummy> { static const unsigned int PRIORITY = 19; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E20, dummy> { static const unsigned int PRIORITY = 20; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E21, dummy> { static const unsigned int PRIORITY = 21; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E22, dummy> { static const unsigned int PRIORITY = 22; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E23, dummy> { static const unsigned int PRIORITY = 23; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E24, dummy> { static const unsigned int PRIORITY = 24; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E25, dummy> { static const unsigned int PRIORITY = 25; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E26, dummy> { static const unsigned int PRIORITY = 26; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E27, dummy> { static const unsigned int PRIORITY = 27; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E28, dummy> { static const unsigned int PRIORITY = 28; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E29, dummy> { static const unsigned int PRIORITY = 29; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E30, dummy> { static const unsigned int PRIORITY = 30; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E31, dummy> { static const unsigned int PRIORITY = 31; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E32, dummy> { static const unsigned int PRIORITY = 32; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E33, dummy> { static const unsigned int PRIORITY = 33; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E34, dummy> { static const unsigned int PRIORITY = 34; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E35, dummy> { static const unsigned int PRIORITY = 35; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E36, dummy> { static const unsigned int PRIORITY = 36; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E37, dummy> { static const unsigned int PRIORITY = 37; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E38, dummy> { static const unsigned int PRIORITY = 38; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E39, dummy> { static const unsigned int PRIORITY = 39; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E40, dummy> { static const unsigned int PRIORITY = 40; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E41, dummy> { static const unsigned int PRIORITY = 41; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E42, dummy> { static const unsigned int PRIORITY = 42; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E43, dummy> { static const unsigned int PRIORITY = 43; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E44, dummy> { static const unsigned int PRIORITY = 44; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E45, dummy> { static const unsigned int PRIORITY = 45; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E46, dummy> { static const unsigned int PRIORITY = 46; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E47, dummy> { static const unsigned int PRIORITY = 47; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E48, dummy> { static const unsigned int PRIORITY = 48; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E49, dummy> { static const unsigned int PRIORITY = 49; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E50, dummy> { static const unsigned int PRIORITY = 50; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E51, dummy> { static const unsigned int PRIORITY = 51; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E52, dummy> { static const unsigned int PRIORITY = 52; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E53, dummy> { static const unsigned int PRIORITY = 53; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E54, dummy> { static const unsigned int PRIORITY = 54; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E55, dummy> { static const unsigned int PRIORITY = 55; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E56, dummy> { static const unsigned int PRIORITY = 56; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E57, dummy> { static const unsigned int PRIORITY = 57; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E58, dummy> { static const unsigned int PRIORITY = 58; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E59, dummy> { static const unsigned int PRIORITY = 59; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E60, dummy> { static const unsigned int PRIORITY = 60; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E61, dummy> { static const unsigned int PRIORITY = 61; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E62, dummy> { static const unsigned int PRIORITY = 62; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
		template <bool dummy> struct ExceptionPriority<E63, dummy> { static const unsigned int PRIORITY = 63; static const uint64_t MASK = uint64_t(1) << PRIORITY; };
	};

	///////////////////////////// ExceptionDispatcher<> ///////////////////////////

	template <unsigned int NUM_EXCEPTIONS>
	class ExceptionDispatcher
	{
	public:
		ExceptionDispatcher(typename CONFIG::CPU *cpu);
		void InstallInterrupt(InterruptBase *interrupt, unsigned int priority);
		template <typename EXCEPTION> typename EXCEPTION::INTERRUPT& ThrowException();
		template <typename EXCEPTION> void AckException();
		template <typename INTERRUPT> void AckInterrupt();
		template <typename EXCEPTION> void EnableException(bool v = true);
		template <typename INTERRUPT> void EnableInterrupt(bool v = true);
		template <typename EXCEPTION> void DisableException() { EnableException<EXCEPTION>(false); }
		template <typename INTERRUPT> void DisableInterrupt() { EnableInterrupt<INTERRUPT>(false); }
		template <typename EXCEPTION> bool RecognizedException() const;
		inline void ProcessExceptions() ALWAYS_INLINE;
		inline bool HasPendingInterrupts() const ALWAYS_INLINE;
	private:
		typedef typename TypeForBitSize<NUM_EXCEPTIONS>::TYPE MASK_TYPE;
		
		typename CONFIG::CPU *cpu;
		MASK_TYPE exc_flags;                       // Exception flags (bits are ordered according to exception priority): 1=thrown 0=not thrown
		MASK_TYPE exc_mask;                        // Exception mask (bits are ordered according to exception priority): 1=enabled, 0=disabled
		InterruptBase *interrupts[NUM_EXCEPTIONS]; // Installed interrupts (sorted by interrupt priority)
	};
	
public:
	void InstallInterrupt(InterruptBase *interrupt, unsigned int priority) { exception_dispatcher.InstallInterrupt(interrupt, priority); }
	template <typename EXCEPTION> typename EXCEPTION::INTERRUPT& ThrowException() { return exception_dispatcher.template ThrowException<EXCEPTION>(); }
	template <typename EXCEPTION> void AckException() { exception_dispatcher.template AckException<EXCEPTION>(); }
	template <typename INTERRUPT> void AckInterrupt() { exception_dispatcher.template AckInterrupt<INTERRUPT>(); }
	template <typename EXCEPTION> void EnableException(bool v = true) { exception_dispatcher.template EnableException<EXCEPTION>(v); }
	template <typename INTERRUPT> void EnableInterrupt(bool v = true) { exception_dispatcher.template EnableInterrupt<INTERRUPT>(v); }
	template <typename EXCEPTION> void DisableException() { exception_dispatcher.template DisableException<EXCEPTION>(); }
	template <typename INTERRUPT> void DisableInterrupt() { exception_dispatcher.template DisableInterrupt<INTERRUPT>(); }
	template <typename EXCEPTION> bool RecognizedException() const { return exception_dispatcher.template RecognizedException<EXCEPTION>(); }
	inline void ProcessExceptions() ALWAYS_INLINE { exception_dispatcher.ProcessExceptions(); }
	inline bool HasPendingInterrupts() const ALWAYS_INLINE { return exception_dispatcher.HasPendingInterrupts(); }
	template <typename EXCEPTION> static unsigned int GetExceptionPriority() { return CONFIG::CPU::EXCEPTION_PRIORITIES::template ExceptionPriority<EXCEPTION>::PRIORITY; }
	template <typename EXCEPTION, typename MASK_TYPE> static MASK_TYPE GetExceptionMask() { return CONFIG::CPU::EXCEPTION_PRIORITIES::template ExceptionPriority<EXCEPTION>::MASK; }
protected:
	
	////////////////////// System Level Register Modeling /////////////////////
	
	enum SLR_Space_Type
	{
		SLR_SPR_SPACE = 0,
		SLR_TBR_SPACE = 1,
		SLR_DCR_SPACE = 2,
		SLR_PMR_SPACE = 3,
		SLR_TMR_SPACE = 4
	};
	
	enum SLR_Access_Type
	{
		SLR_NO_ACCESS = unisim::util::reg::core::HW_RW,
		SLR_RO        = unisim::util::reg::core::SW_R,
		SLR_WO        = unisim::util::reg::core::SW_W,
		SLR_RW        = unisim::util::reg::core::SW_RW
	};
	
	enum SLR_Privilege_Type
	{
		SLR_NON_PRIVILEGED = 0,
		SLR_PRIVILEGED     = 1
	};

	struct SLRBase
	{
		virtual ~SLRBase() {}
		virtual bool IsValid() const { return false; }
		virtual unsigned int GetRegNum() const { return 0; }
		virtual SLR_Space_Type GetSpace() const { return SLR_SPR_SPACE; }
		virtual bool IsPrivileged() const { return false; }
		virtual bool IsReadOnly() const { return false; }
		virtual bool IsWriteOnly() const { return false; }
		virtual bool CheckMoveToLegality() { return false; }
		virtual bool CheckMoveFromLegality() { return false; }
		virtual bool MoveTo(uint32_t value) { return false; }
		virtual bool MoveFrom(uint32_t& value) { return false; }
		virtual void Reset() {}
		virtual void AddRegisterInterface() {}
	};
	
	typedef SLRBase SPRBase;
	typedef SLRBase TBRBase;
	typedef SLRBase DCRBase;
	typedef SLRBase PMRBase;
	typedef SLRBase TMRBase;

	template <SLR_Space_Type _SLR_SPACE>
	struct InvalidSLR : SLRBase
	{
		InvalidSLR(typename CONFIG::CPU *_cpu) : cpu(_cpu) {}
		
		virtual bool CheckMoveToLegality()
		{
			// Illegal Instruction
			if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugWarningStream() << "Move to invalid system level register is an illegal operation" << std::endl;
			}
			cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
			return false;
		}
		
		virtual bool CheckMoveFromLegality()
		{
			// Illegal Instruction
			if(cpu->verbose_move_from_slr)
			{
				cpu->GetDebugWarningStream() << "Move from invalid system level register is an illegal operation" << std::endl;
			}
			cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
			return false;
		}
	private:
		typename CONFIG::CPU *cpu;
	};
	
	struct ExternalDCR : DCRBase
	{
		typename CONFIG::CPU *cpu;
		unsigned int n;
		
		ExternalDCR(typename CONFIG::CPU *_cpu) : cpu(_cpu), n(0) {}
		
		void Attach(unsigned int _n) { n = _n; }
		virtual bool IsValid() const { return true; }
		virtual bool CheckMoveFromLegality() { return true; }
		virtual bool CheckMoveToLegality() { return true; }
		virtual bool MoveTo(uint32_t value) { cpu->DCRWrite(n, value); return true; }
		virtual bool MoveFrom(uint32_t& value) { cpu->DCRRead(n, value); return true; }
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, SLR_Access_Type _SLR_ACCESS = SLR_RW, SLR_Privilege_Type _SLR_PRIVILEGE = SLR_NON_PRIVILEGED>
	struct UnnumberedSLR : unisim::util::reg::core::Register<SLR_REGISTER, 32, unisim::util::reg::core::Access(_SLR_ACCESS), SLRBase>
	{
		typedef unisim::util::reg::core::Register<SLR_REGISTER, 32, unisim::util::reg::core::Access(_SLR_ACCESS), SLRBase> Super;
		static const SLR_Space_Type SLR_SPACE = _SLR_SPACE;
		static const SLR_Access_Type SLR_ACCESS = _SLR_ACCESS;
		static const SLR_Privilege_Type SLR_PRIVILEGE = _SLR_PRIVILEGE;
		
		struct  _0 : Field< _0,  0> {};
		struct  _1 : Field< _1,  1> {};
		struct  _2 : Field< _2,  2> {};
		struct  _3 : Field< _3,  3> {};
		struct  _4 : Field< _4,  4> {};
		struct  _5 : Field< _5,  5> {};
		struct  _6 : Field< _6,  6> {};
		struct  _7 : Field< _7,  7> {};
		struct  _8 : Field< _8,  8> {};
		struct  _9 : Field< _9,  9> {};
		struct _10 : Field<_10, 10> {};
		struct _11 : Field<_11, 11> {};
		struct _12 : Field<_12, 12> {};
		struct _13 : Field<_13, 13> {};
		struct _14 : Field<_14, 14> {};
		struct _15 : Field<_15, 15> {};
		struct _16 : Field<_16, 16> {};
		struct _17 : Field<_17, 17> {};
		struct _18 : Field<_18, 18> {};
		struct _19 : Field<_19, 19> {};
		struct _20 : Field<_20, 20> {};
		struct _21 : Field<_21, 21> {};
		struct _22 : Field<_22, 22> {};
		struct _23 : Field<_23, 23> {};
		struct _24 : Field<_24, 24> {};
		struct _25 : Field<_25, 25> {};
		struct _26 : Field<_26, 26> {};
		struct _27 : Field<_27, 27> {};
		struct _28 : Field<_28, 28> {};
		struct _29 : Field<_29, 29> {};
		struct _30 : Field<_30, 30> {};
		struct _31 : Field<_31, 31> {};
		
		UnnumberedSLR(typename CONFIG::CPU *_cpu) : Super(), cpu(_cpu) {}
		UnnumberedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_value), cpu(_cpu) {}
		virtual bool IsValid() const { return true; }
		virtual SLR_Space_Type GetSpace() const { return _SLR_SPACE; }
		virtual bool IsPrivileged() const { return SLR_PRIVILEGE == SLR_PRIVILEGED; }
		virtual bool IsReadOnly() const { return SLR_ACCESS == SLR_RO; }
		virtual bool IsWriteOnly() const { return SLR_ACCESS == SLR_WO; }

		virtual bool CheckMoveToLegality()
		{
			if(cpu->GetMSR().template Get<typename CONFIG::CPU::MSR::PR>() && (SLR_PRIVILEGE == SLR_PRIVILEGED))
			{
				// Privilege Violation
				if(cpu->verbose_move_to_slr)
				{
					cpu->GetDebugWarningStream() << "Move to " << this->GetName() << " is a privileged operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::PrivilegeViolation>();
				return false;
			}

			if(SLR_ACCESS == SLR_RO)
			{
				// Illegal Instruction
				if(cpu->verbose_move_to_slr)
				{
					cpu->GetDebugWarningStream() << "Move to " << this->GetName() << " is an illegal operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}
			
			return true;
		}
		
		virtual bool CheckMoveFromLegality()
		{
			if(cpu->GetMSR().template Get<typename CONFIG::CPU::MSR::PR>() && (SLR_PRIVILEGE == SLR_PRIVILEGED))
			{
				// Privilege Violation
				if(cpu->verbose_move_from_slr)
				{
					cpu->GetDebugWarningStream() << "Move from " << this->GetName() << " is a privileged operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::PrivilegeViolation>();
				return false;
			}
			
			if(SLR_ACCESS == SLR_WO)
			{
				// Illegal Instruction
				if(cpu->verbose_move_from_slr)
				{
					cpu->GetDebugWarningStream() << "Move from " << this->GetName() << " is an illegal operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}

			return true;
		}
		
		virtual bool MoveTo(uint32_t value)
		{
			unisim::util::reg::core::ReadWriteStatus rws = this->Write(value);
			if(rws)
			{
				cpu->GetDebugWarningStream() << "While moving 0x" << std::hex << value << std::dec << " to " << this->GetName() << ", " << rws << "; See below " << this->GetName() << " content after move operation: " << std::endl;
				this->LongPrettyPrint(cpu->GetDebugWarningStream());
				cpu->GetDebugWarningStream() << std::endl;
			}
			else if(cpu->verbose_move_to_slr)
			{
				cpu->GetDebugInfoStream() << "Moving 0x" << std::hex << value << " to " << this->GetName() << ": "; this->ShortPrettyPrint(cpu->GetDebugInfoStream()); cpu->GetDebugInfoStream() << std::dec << std::endl;
			}
			
			return !unisim::util::reg::core::IsReadWriteError(rws);
		}
		
		virtual bool MoveFrom(uint32_t& value)
		{
			value = 0;
			unisim::util::reg::core::ReadWriteStatus rws = this->Read(value);
			if(rws)
			{
				cpu->GetDebugWarningStream() << rws << std::endl;
				cpu->GetDebugWarningStream() << "While moving 0x" << std::hex << value << std::dec << " from " << this->GetName() << ", " << rws << "; See below " << this->GetName() << " content after move operation: " << std::endl;
				this->LongPrettyPrint(cpu->GetDebugWarningStream());
				cpu->GetDebugWarningStream() << std::endl;
			}
			else if(cpu->verbose_move_from_slr)
			{
				cpu->GetDebugInfoStream() << "Moving 0x" << std::hex << value << " from " << this->GetName() << ": "; this->ShortPrettyPrint(cpu->GetDebugInfoStream()); cpu->GetDebugInfoStream() << std::dec << std::endl;
			}
			
			return !unisim::util::reg::core::IsReadWriteError(rws);
		}

		typename CONFIG::CPU *GetCPU() const { return cpu; }
		
		virtual void AddRegisterInterface()
		{
			cpu->AddRegisterInterface(this->CreateRegisterInterface());
		}

		using Super::operator =;
		
	protected:
		typename CONFIG::CPU *cpu;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, SLR_Access_Type _SLR_ACCESS = SLR_RW, SLR_Privilege_Type _SLR_PRIVILEGE = SLR_NON_PRIVILEGED>
	struct SLR : UnnumberedSLR<SLR_REGISTER, _SLR_SPACE, _SLR_ACCESS, _SLR_PRIVILEGE>
	{
		typedef UnnumberedSLR<SLR_REGISTER, _SLR_SPACE, _SLR_ACCESS, _SLR_PRIVILEGE> Super;
		static const unsigned int SLR_NUM = _SLR_NUM;
		static const unsigned int REG_NUM = _SLR_NUM;
		
		SLR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		virtual unsigned int GetRegNum() const { return _SLR_NUM; }
		
		using Super::operator =;
	private:
		
		void Init()
		{
			this->cpu->RegisterSLR(_SLR_NUM, this);
		}
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct PrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_PRIVILEGED> Super;
		
		PrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct ReadOnlyPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_PRIVILEGED> Super;

		ReadOnlyPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct WriteOnlyPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_PRIVILEGED> Super;

		WriteOnlyPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		WriteOnlyPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct NonPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_NON_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_NON_PRIVILEGED> Super;

		NonPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct ReadOnlyNonPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_NON_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_NON_PRIVILEGED> Super;

		ReadOnlyNonPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct WriteOnlyNonPrivilegedSLR : SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_NON_PRIVILEGED>
	{
		typedef SLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_NON_PRIVILEGED> Super;

		WriteOnlyNonPrivilegedSLR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		WriteOnlyNonPrivilegedSLR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM, SLR_Access_Type _SLR_ACCESS = SLR_RW, SLR_Privilege_Type _SLR_PRIVILEGE = SLR_NON_PRIVILEGED>
	struct AltSLR : SLRBase
	{
		static const SLR_Space_Type SLR_SPACE = _SLR_SPACE;
		static const unsigned int SLR_NUM = _SLR_NUM;
		static const unsigned int REG_NUM = _SLR_NUM;
		static const SLR_Access_Type SLR_ACCESS = _SLR_ACCESS;
		static const SLR_Privilege_Type SLR_PRIVILEGE = _SLR_PRIVILEGE;

		AltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : cpu(_cpu), slr(_slr) { Init(); }
		virtual bool IsValid() const { return true; }
		virtual unsigned int GetRegNum() const { return _SLR_NUM; }
		virtual SLR_Space_Type GetSpace() const { return _SLR_SPACE; }
		virtual bool IsPrivileged() const { return SLR_PRIVILEGE == SLR_PRIVILEGED; }
		virtual bool IsReadOnly() const { return SLR_ACCESS == SLR_RO; }
		virtual bool IsWriteOnly() const { return SLR_ACCESS == SLR_WO; }

		virtual bool CheckMoveToLegality()
		{
			if(cpu->GetMSR().template Get<typename CONFIG::CPU::MSR::PR>() && (SLR_PRIVILEGE == SLR_PRIVILEGED))
			{
				// Privilege Violation
				if(cpu->verbose_move_to_slr)
				{
					cpu->GetDebugWarningStream() << "Move to " << slr->GetName() << " is a privileged operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::PrivilegeViolation>();
				return false;
			}

			if(SLR_ACCESS == SLR_RO)
			{
				// Illegal Instruction
				if(cpu->verbose_move_to_slr)
				{
					cpu->GetDebugWarningStream() << "Move to " << slr->GetName() << " is an illegal operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}
			
			return true;
		}
		
		virtual bool CheckMoveFromLegality()
		{
			if(cpu->GetMSR().template Get<typename CONFIG::CPU::MSR::PR>() && (SLR_PRIVILEGE == SLR_PRIVILEGED))
			{
				// Privilege Violation
				if(cpu->verbose_move_from_slr)
				{
					cpu->GetDebugWarningStream() << "Move from " << slr->GetName() << " is a privileged operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::PrivilegeViolation>();
				return false;
			}
			
			if(SLR_ACCESS == SLR_WO)
			{
				// Illegal Instruction
				if(cpu->verbose_move_from_slr)
				{
					cpu->GetDebugWarningStream() << "Move from " << slr->GetName() << " is an illegal operation" << std::endl;
				}
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}

			return true;
		}
		
		virtual bool MoveTo(uint32_t value) { return slr->MoveTo(value); }
		virtual bool MoveFrom(uint32_t& value) { return slr->MoveFrom(value); }
		
		typename CONFIG::CPU *GetCPU() const { return cpu; }
		
	protected:
		typename CONFIG::CPU *cpu;
	private:
		SLR_REGISTER *slr;
		
		void Init()
		{
			cpu->RegisterSLR(_SLR_NUM, this);
		}
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct PrivilegedAltSLR : AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_PRIVILEGED>
	{
		typedef AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_PRIVILEGED> Super;
		
		PrivilegedAltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct ReadOnlyPrivilegedAltSLR : AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_PRIVILEGED>
	{
		typedef AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_PRIVILEGED> Super;

		ReadOnlyPrivilegedAltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct WriteOnlyPrivilegedAltSLR : AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_PRIVILEGED>
	{
		typedef AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_PRIVILEGED> Super;

		WriteOnlyPrivilegedAltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct NonPrivilegedAltSLR : AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_NON_PRIVILEGED>
	{
		typedef AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RW, SLR_NON_PRIVILEGED> Super;

		NonPrivilegedAltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
		
		using Super::operator =;
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct ReadOnlyNonPrivilegedAltSLR : AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_NON_PRIVILEGED>
	{
		typedef AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_RO, SLR_NON_PRIVILEGED> Super;

		ReadOnlyNonPrivilegedAltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
		
		using Super::operator =;
	};
	
	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _SLR_NUM>
	struct WriteOnlyNonPrivilegedAltSLR : AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_NON_PRIVILEGED>
	{
		typedef AltSLR<SLR_REGISTER, _SLR_SPACE, _SLR_NUM, SLR_WO, SLR_NON_PRIVILEGED> Super;

		WriteOnlyNonPrivilegedAltSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
		
		using Super::operator =;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _USLR_NUM, SLR_Access_Type _USLR_ACCESS = SLR_RW>
	struct USLR : SLRBase
	{
		USLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : cpu(_cpu), slr(_slr) {}
		virtual bool IsValid() const { return true; }
		virtual unsigned int GetRegNum() const { return _USLR_NUM; }
		virtual SLR_Space_Type GetSpace() const { return _SLR_SPACE; }
		virtual bool IsPrivileged() const { return false; }
		virtual bool IsReadOnly() const { return _USLR_ACCESS == SLR_RO; }
		virtual bool IsWriteOnly() const { return _USLR_ACCESS == SLR_WO; }
		virtual bool CheckMoveToLegality()
		{
			if(_USLR_ACCESS == SLR_RO)
			{
				// Illegal Instruction
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}

			return true;
		}
		virtual bool CheckMoveFromLegality()
		{
			if(_USLR_ACCESS == SLR_WO)
			{
				// Illegal Instruction
				cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
				return false;
			}
			
			return true;
		}
		virtual bool MoveTo(uint32_t value) { return slr->MoveTo(value); }
		virtual bool MoveFrom(uint32_t& value) { return slr->MoveFrom(value); }
	private:
		typename CONFIG::CPU *cpu;
		SLR_REGISTER *slr;
	};

	template <typename SLR_REGISTER, SLR_Space_Type _SLR_SPACE, unsigned int _USLR_NUM>
	struct ReadOnlyUSLR : USLR<SLR_REGISTER, _SLR_SPACE, _USLR_NUM, SLR_RO>
	{
		typedef USLR<SLR_REGISTER, _SLR_SPACE, _USLR_NUM, SLR_RO> Super;
		
		ReadOnlyUSLR(typename CONFIG::CPU *_cpu, SLR_REGISTER *_slr) : Super(_cpu, _slr) {}
	};

	//////////////////// Special Purpose Register Modeling ////////////////////

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct PrivilegedSPR : PrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef PrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;
		
		PrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct ReadOnlyPrivilegedSPR : ReadOnlyPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		ReadOnlyPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct WriteOnlyPrivilegedSPR : WriteOnlyPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef WriteOnlyPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		WriteOnlyPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		WriteOnlyPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct NonPrivilegedSPR : NonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef NonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		NonPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct ReadOnlyNonPrivilegedSPR : ReadOnlyNonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		ReadOnlyNonPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _SPR_NUM>
	struct WriteOnlyNonPrivilegedSPR : WriteOnlyNonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM>
	{
		typedef WriteOnlyNonPrivilegedSLR<SPR_REGISTER, SLR_SPR_SPACE, _SPR_NUM> Super;

		WriteOnlyNonPrivilegedSPR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		WriteOnlyNonPrivilegedSPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename SPR_REGISTER, unsigned int _USPR_NUM>
	struct USPR : USLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM>
	{
		typedef USLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM> Super;
		
		USPR(SPR_REGISTER *_spr) : Super(_spr) {}
	};

	template <typename SPR_REGISTER, unsigned int _USPR_NUM>
	struct ReadOnlyUSPR : ReadOnlyUSLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM>
	{
		typedef ReadOnlyUSLR<SPR_REGISTER, SLR_SPR_SPACE, _USPR_NUM> Super;
		
		ReadOnlyUSPR(SPR_REGISTER *_spr) : Super(_spr) {}
	};

	/////////////////////// Time Base Register Modeling ///////////////////////

	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct PrivilegedTBR : PrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef PrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;
		
		PrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct ReadOnlyPrivilegedTBR : ReadOnlyPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;

		ReadOnlyPrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct NonPrivilegedTBR : NonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef NonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;

		NonPrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename TBR_REGISTER, unsigned int _TBR_NUM>
	struct ReadOnlyNonPrivilegedTBR : ReadOnlyNonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<TBR_REGISTER, SLR_TBR_SPACE, _TBR_NUM> Super;

		ReadOnlyNonPrivilegedTBR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedTBR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TBR_REGISTER, unsigned int _UTBR_NUM>
	struct UTBR : USLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM>
	{
		typedef USLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM> Super;
		
		UTBR(TBR_REGISTER *_spr) : Super(_spr) {}
	};

	template <typename TBR_REGISTER, unsigned int _UTBR_NUM>
	struct ReadOnlyUTBR : ReadOnlyUSLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM>
	{
		typedef ReadOnlyUSLR<TBR_REGISTER, SLR_TBR_SPACE, _UTBR_NUM> Super;
		
		ReadOnlyUTBR(TBR_REGISTER *_spr) : Super(_spr) {}
	};

	////////////////////// Device Control Register Modeling ///////////////////

	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct PrivilegedDCR : PrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef PrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;
		
		PrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct ReadOnlyPrivilegedDCR : ReadOnlyPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;

		ReadOnlyPrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct NonPrivilegedDCR : NonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef NonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;

		NonPrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename DCR_REGISTER, unsigned int _DCR_NUM>
	struct ReadOnlyNonPrivilegedDCR : ReadOnlyNonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<DCR_REGISTER, SLR_DCR_SPACE, _DCR_NUM> Super;

		ReadOnlyNonPrivilegedDCR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedDCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename DCR_REGISTER, unsigned int _UDCR_NUM>
	struct UDCR : USLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM>
	{
		typedef USLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM> Super;
		
		UDCR(DCR_REGISTER *_spr) : Super(_spr) {}
	};

	template <typename DCR_REGISTER, unsigned int _UDCR_NUM>
	struct ReadOnlyUDCR : ReadOnlyUSLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM>
	{
		typedef ReadOnlyUSLR<DCR_REGISTER, SLR_DCR_SPACE, _UDCR_NUM> Super;
		
		ReadOnlyUDCR(DCR_REGISTER *_spr) : Super(_spr) {}
	};

	////////////////// Performance Monitor Register Modeling //////////////////

	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct PrivilegedPMR : PrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef PrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;
		
		PrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct ReadOnlyPrivilegedPMR : ReadOnlyPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;

		ReadOnlyPrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct NonPrivilegedPMR : NonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef NonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;

		NonPrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename PMR_REGISTER, unsigned int _PMR_NUM>
	struct ReadOnlyNonPrivilegedPMR : ReadOnlyNonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<PMR_REGISTER, SLR_PMR_SPACE, _PMR_NUM> Super;

		ReadOnlyNonPrivilegedPMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedPMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename PMR_REGISTER, unsigned int _UPMR_NUM>
	struct UPMR : USLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM>
	{
		typedef USLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM> Super;
		
		UPMR(typename CONFIG::CPU *_cpu, PMR_REGISTER *_pmr) : Super(_cpu, _pmr) {}
	};

	template <typename PMR_REGISTER, unsigned int _UPMR_NUM>
	struct ReadOnlyUPMR : ReadOnlyUSLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM>
	{
		typedef ReadOnlyUSLR<PMR_REGISTER, SLR_PMR_SPACE, _UPMR_NUM> Super;
		
		ReadOnlyUPMR(typename CONFIG::CPU *_cpu, PMR_REGISTER *_pmr) : Super(_cpu, _pmr) {}
	};

	//////////////////// Thread Management Register Modeling //////////////////

	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct PrivilegedTMR : PrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef PrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;
		
		PrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct ReadOnlyPrivilegedTMR : ReadOnlyPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef ReadOnlyPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;

		ReadOnlyPrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyPrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};

	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct NonPrivilegedTMR : NonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef NonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;

		NonPrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		NonPrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename TMR_REGISTER, unsigned int _TMR_NUM>
	struct ReadOnlyNonPrivilegedTMR : ReadOnlyNonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM>
	{
		typedef ReadOnlyNonPrivilegedSLR<TMR_REGISTER, SLR_TMR_SPACE, _TMR_NUM> Super;

		ReadOnlyNonPrivilegedTMR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ReadOnlyNonPrivilegedTMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		using Super::operator =;
	};
	
	template <typename TMR_REGISTER, unsigned int _UTMR_NUM>
	struct UTMR : USLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM>
	{
		typedef USLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM> Super;
		
		UTMR(typename CONFIG::CPU *_cpu, TMR_REGISTER *_tmr) : Super(_cpu, _tmr) {}
	};

	template <typename TMR_REGISTER, unsigned int _UTMR_NUM>
	struct ReadOnlyUTMR : ReadOnlyUSLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM>
	{
		typedef ReadOnlyUSLR<TMR_REGISTER, SLR_TMR_SPACE, _UTMR_NUM> Super;
		
		ReadOnlyUTMR(typename CONFIG::CPU *_cpu, TMR_REGISTER *_tmr) : Super(_cpu, _tmr) {}
	};

	void RegisterSLR(unsigned int n, SLRBase *slr);
	
	virtual SPRBase& GetSPR(unsigned int n);
	virtual TBRBase& GetTBR(unsigned int n);
	virtual DCRBase& GetDCR(unsigned int n);
	virtual PMRBase& GetPMR(unsigned int n);
	virtual TMRBase& GetTMR(unsigned int n);
	
public:
	/////////////////////////// Machine State Register ////////////////////////

	// Machine State Register
	struct MSR : unisim::util::reg::core::Register<MSR, 32>
	{
		typedef unisim::util::reg::core::Register<MSR, 32> Super;
		
		struct VEC : Field<VEC,6>  {}; // Altivec vector unit available
		struct SPV : Field<SPV,6>  {}; // SP/Embedded FP/Vector available
		struct POW : Field<POW,13> {}; // Power management enable
		struct WE  : Field<WE ,13> {}; // Wait state (Power management) enable
		struct CE  : Field<CE ,14> {}; // Critical Interrupt Enable
		struct ILE : Field<ILE,15> {}; // Exception little-endian mode
		struct EE  : Field<EE ,16> {}; // External Interrupt Enable
		struct PR  : Field<PR ,17> {}; // Problem State
		struct FP  : Field<FP ,18> {}; // Floating-Point Available
		struct ME  : Field<ME ,19> {}; // Machine Check Enable
		struct FE0 : Field<FE0,20> {}; // Floating-point exception mode 0
		struct SE  : Field<SE ,21> {}; // Single-step trace enable
		struct DWE : Field<DWE,21> {}; // Debug Wait Enable
		struct BE  : Field<BE ,22> {}; // Branch trace enable
		struct DE  : Field<DE ,22> {}; // Debug Interrupt Enable
		struct FE1 : Field<FE1,23> {}; // Floating-point exception mode 1
		struct IP  : Field<IP ,25> {}; // Exception prefix
		struct IR  : Field<IR, 26> {}; // Instruction address translation
		struct IS  : Field<IS ,26> {}; // Instruction Address Space
		struct DR  : Field<DR, 27> {}; // Data address translation
		struct DS  : Field<DS ,27> {}; // Data Address Space
		struct PMM : Field<PMM,29> {}; // PMM Performance monitor mark bit
		struct RI  : Field<RI ,30> {}; // Recoverable Interrupt
		struct LE  : Field<LE ,31> {}; // Little-endian mode enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7400    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7410    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7441    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7445    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7447    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7448    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7450    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7451    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7455    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(MPC7457    , _) { typedef FieldSet<VEC, POW, ILE, EE, PR, FP, ME, FE0, SE, BE, FE1, IP, IR, DR, PMM, RI, LE> ALL; };
		CASE_ENUM_TRAIT(PPC440     , _) { typedef FieldSet<WE, CE, EE, PR, FP, ME, FE0, DWE, DE, FE1, IS, DS> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { typedef FieldSet<SPV, WE, CE, EE, PR, FP, ME, FE0, DE, FE1, IS, DS, PMM, RI> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CE, EE, PR, ME, DE, PMM, RI> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MSR(typename CONFIG::CPU *_cpu) : Super(), cpu(_cpu) { Init(); }
		MSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_value), cpu(_cpu) { Init(); }
		using Super::operator =;
	protected:
		typename CONFIG::CPU *cpu;
	private:
		
		void Init()
		{
			this->SetName("MSR"); this->SetDescription("Machine State Register");
			VEC::SetName("VEC"); VEC::SetDescription("Altivec vector unit available");
			SPV::SetName("SPV"); SPV::SetDescription("SP/Embedded FP/Vector available");
			POW::SetName("POW"); POW::SetDescription("Power management enable");
			WE ::SetName("WE");  WE ::SetDescription("Wait state (Power management) enable");
			CE ::SetName("CE");  CE ::SetDescription("Critical Interrupt Enable");
			ILE::SetName("ILE"); ILE::SetDescription("Exception little-endian mode");
			EE ::SetName("EE");  EE ::SetDescription("External Interrupt Enable");
			PR ::SetName("PR");  PR ::SetDescription("Problem State");
			FP ::SetName("FP");  FP ::SetDescription("Floating-Point Available");
			ME ::SetName("ME");  ME ::SetDescription("Machine Check Enable");
			FE0::SetName("FE0"); FE0::SetDescription("Floating-point exception mode 0");
			SE ::SetName("SE");  SE ::SetDescription("Single-step trace enable");
			DWE::SetName("DWE"); DWE::SetDescription("Debug Wait Enable");
			BE ::SetName("BE");  BE ::SetDescription("Branch trace enable");
			DE ::SetName("DE");  DE ::SetDescription("Debug Interrupt Enable");
			FE1::SetName("FE1"); FE1::SetDescription("Floating-point exception mode 1");
			IP ::SetName("IP");  IP ::SetDescription("Exception prefix");
			IR ::SetName("IR");  IR ::SetDescription("Instruction address translation");
			IS ::SetName("IS");  IS ::SetDescription("Instruction Address Space");
			DR ::SetName("DR");  DR ::SetDescription("Data address translation");
			DS ::SetName("DS");  DS ::SetDescription("Data Address Space");
			PMM::SetName("PMM"); PMM::SetDescription("PMM Performance monitor mark bit");
			RI ::SetName("RI");  RI ::SetDescription("Recoverable Interrupt");
			LE ::SetName("LE");  LE ::SetDescription("Little-endian mode enable");
			
			cpu->AddRegisterInterface(this->CreateRegisterInterface());
		}
	};

	////////////////////////// Special Purpose Registers //////////////////////
	
	// Data Storage Interrupt Status Register
	struct DSISR : PrivilegedSPR<DSISR, 18>
	{
		typedef PrivilegedSPR<DSISR, 18> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DSISR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DSISR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }
		
	private:
		void Init()
		{
			this->SetName("DSISR"); this->SetDescription("Data Storage Interrupt Status Register");
		}
	};
	
	// Data Address Register
	struct DAR : PrivilegedSPR<DAR, 19>
	{
		typedef PrivilegedSPR<DAR, 19> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }
	
	private:
		void Init()
		{
			this->SetName("DAR"); this->SetDescription("Data Address Register");
		}
	};

	// Decrementer
	struct DEC : PrivilegedSPR<DEC, 22>
	{
		typedef PrivilegedSPR<DEC, 22> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DEC(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DEC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
		
		virtual bool MoveFrom(uint32_t& value)
		{
			this->cpu->RunInternalTimers();
			return Super::MoveFrom(value);
		}
		
		virtual bool MoveTo(uint32_t value)
		{
			this->cpu->RunInternalTimers();
			bool status = Super::MoveTo(value);
			this->cpu->RunInternalTimers();
			return status;
		}
	private:
		void Init() { this->SetName("DEC"); this->SetDescription("Decrementer"); }
	};
	
	// Sample Data Register
	struct SDR1 : PrivilegedSPR<SDR1, 25>
	{
		typedef PrivilegedSPR<SDR1, 25> Super;
		
		struct HTABORG  : Field<HTABORG , 0 , 15> {}; // Physical base address of page table
		struct HTABEXT  : Field<HTABEXT , 16, 18> {}; // Extension bits for physical base address of page table
		struct HTMEXT   : Field<HTMEXT  , 19, 22> {}; // Hash table mask extension bits
		struct HTABMASK : Field<HTABMASK, 23, 31> {}; // Mask for page table address
		
		struct HTABORG_MSB : Field<HTABORG_MSB, 0, 6>  {}; // 7 most significative bits of HTABORG
		struct HTABORG_LSB : Field<HTABORG_LSB, 7, 15> {}; // 9 least significative bits of HTABORG
		
		struct XA_HTABORG_MSB : Field<HTABORG_MSB, 0, 2>  {}; // 3 most significative bits of HTABORG (for extended addressing)
		struct XA_HTABORG_LSB : Field<HTABORG_LSB, 3, 15> {}; // 13 least significative bits of HTABORG (for extended addressing)
		
		struct XA_HTABMASK : Field<XA_HTABMASK, 19, 31> {}; // Mask for page table address (for extended addressing)

		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC601  , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC603E , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC604E , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC740  , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC745  , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC750  , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC755  , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7400 , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7410 , _) { typedef FieldSet<HTABORG, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7441 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7445 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7451 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7455 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _) { typedef FieldSet<HTABORG, HTABEXT, HTMEXT, HTABMASK> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		SDR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SDR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }
		
	private:
		void Init()
		{
			this->SetName("SDR1"); this->SetDescription("Register that specifies the page table base address for virtual-to-physical address translation");
			
			HTABORG ::SetName("HTABORG");  HTABORG ::SetDescription("Physical base address of page table");
			HTABEXT ::SetName("HTABEXT");  HTABEXT ::SetDescription("Extension bits for physical base address of page table");
			HTMEXT  ::SetName("HTMEXT");   HTMEXT  ::SetDescription("Hash table mask extension bits");
			HTABMASK::SetName("HTABMASK"); HTABMASK::SetDescription("Mask for page table address");
		}
	};
	
	// Save/Restore Register 0
	struct SRR0 : PrivilegedSPR<SRR0, 26>
	{
		typedef PrivilegedSPR<SRR0, 26> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SRR0"); this->SetDescription("Save/Restore Register 0"); }
	};
	
	// Save/Restore Register 1
	struct SRR1 : PrivilegedSPR<SRR1, 27>
	{
		typedef PrivilegedSPR<SRR1, 27> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SRR1"); this->SetDescription("Save/Restore Register 1"); }
	};

	// Process ID Register
	struct PID0 : PrivilegedSPR<PID0, 48>
	{
	public:
		typedef PrivilegedSPR<PID0, 48> Super;
		
		struct Process_ID : Field<Process_ID, 24, 31> {}; // Process ID
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<Process_ID> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<Process_ID> ALL; };
		CASE_ENUM_TRAIT(PPC440, _) { typedef FieldSet<Process_ID> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PID0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PID0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("PID0"); this->SetDescription("Process ID Register"); Process_ID::SetName("process_id"); }
	};
	
	// Decrementer Auto-Reload
	struct DECAR : PrivilegedSPR<DECAR, 54>
	{
		typedef PrivilegedSPR<DECAR, 54> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DECAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DECAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("DECAR"); this->SetDescription("Decrementer Auto-Reload"); }
	};
	
	// Critical Save/Restore Register 0
	struct CSRR0 : PrivilegedSPR<CSRR0, 58>
	{
		typedef PrivilegedSPR<CSRR0, 58> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		CSRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CSRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("CSRR0"); this->SetDescription("Critical Save/Restore Register 0"); }
	};

	// Critical Save/Restore Register 1
	struct CSRR1 : PrivilegedSPR<CSRR1, 59>
	{
		typedef PrivilegedSPR<CSRR1, 59> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		CSRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CSRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;

		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("CSRR1"); this->SetDescription("Critical Save/Restore Register 1"); }
	};

	// Data Exception Address Register
	struct DEAR : PrivilegedSPR<DEAR, 61>
	{
		typedef PrivilegedSPR<DEAR, 61> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DEAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DEAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("DEAR"); this->SetDescription("Data Exception Address Register");
		}
	};

public:
	// Exception Syndrome Register
	struct ESR : PrivilegedSPR<ESR, 62>
	{
		typedef PrivilegedSPR<ESR, 62> Super;
		
		struct MCI   : Field<MCI  , 0     > {}; // Machine Check Instruction Fetch Exception
		struct PIL   : Field<PIL  , 4     > {}; // Illegal Instruction exception
		struct PPR   : Field<PPR  , 5     > {}; // Privileged Instruction exception
		struct PTR   : Field<PTR  , 6     > {}; // Trap exception
		struct FP    : Field<FP   , 7     > {}; // Floating-point operation
		struct ST    : Field<ST   , 8     > {}; // Store operation
		struct DLK   : Field<DLK  , 10, 11> {}; // Data Storage Interrupt Locking Exception
		struct AP    : Field<AP   , 12    > {}; // Auxiliary Processor Operation
		struct PUO   : Field<PUO  , 13    > {}; // Program Interrupt Unimplemented Operation Exception
		struct BO    : Field<BO   , 14    > {}; // Byte Ordering exception/Mismatched Instruction Storage exception
		struct PIE   : Field<PIE  , 15    > {}; // Program Interrupt Imprecise Exception
		struct SPV   : Field<SPV  , 24    > {}; // EFPU APU Operation
		struct VLEMI : Field<VLEMI, 26    > {}; // VLE Mode Instruction
		struct PCRE  : Field<PCRE , 27    > {}; // Program Interrupt Condition Register Enable
		struct PCMP  : Field<PCMP , 28    > {}; // Program Interrupt Compare
		struct PCRF  : Field<PCRF , 29, 31> {}; // Program Interrupt Condition Register Field
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet<MCI, PIL, PPR, PTR, FP, ST, DLK, AP, PUO, BO, PIE, PCRE, PCMP, PCRF> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<PIL, PPR, PTR, FP, ST, BO, SPV, VLEMI> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<PIL, PPR, PTR, FP, ST, BO, SPV, VLEMI> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;

		ESR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ESR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;

		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			 this->SetName("ESR");    this->SetDescription("Exception Syndrome Register");
			MCI  ::SetName("MCI");   MCI  ::SetDescription("Machine Check Instruction Fetch Exception");
			PIL  ::SetName("PIL");   PIL  ::SetDescription("Illegal Instruction exception");
			PPR  ::SetName("PPR");   PPR  ::SetDescription("Privileged Instruction exception");
			PTR  ::SetName("PTR");   PTR  ::SetDescription("Trap exception");
			FP   ::SetName("FP");    FP   ::SetDescription("Floating-point operation");
			ST   ::SetName("ST");    ST   ::SetDescription("Store operation");
			DLK  ::SetName("DLK");   DLK  ::SetDescription("Data Storage Interrupt Locking Exception");
			AP   ::SetName("AP");    AP   ::SetDescription("Auxiliary Processor Operation");
			PUO  ::SetName("PUO");   PUO  ::SetDescription("Program Interrupt Unimplemented Operation Exception");
			BO   ::SetName("BO");    BO   ::SetDescription("Byte Ordering exception/Mismatched Instruction Storage exception");
			PIE  ::SetName("PIE");   PIE  ::SetDescription("Program Interrupt Imprecise Exception");
			SPV  ::SetName("SPV");   SPV  ::SetDescription("EFPU APU Operation");
			VLEMI::SetName("VLEMI"); VLEMI::SetDescription("VLE Mode Instruction");
			PCRE ::SetName("PCRE");  PCRE ::SetDescription("Program Interrupt Condition Register Enable");
			PCMP ::SetName("PCMP");  PCMP ::SetDescription("Program Interrupt Compare");
			PCRF ::SetName("PCRF");  PCRF ::SetDescription("Program Interrupt Condition Register Field");
		}
	};
protected:
	
	// Interrupt Vector Prefix Register
	struct IVPR : PrivilegedSPR<IVPR, 63>
	{
		typedef PrivilegedSPR<IVPR, 63> Super;
		
		struct Vector_Base : Field<Vector_Base, 0, 23> {}; // Vector Base
		struct IVP         : Field<IVP        , 0, 15> {}; // Interrupt Vector Prefix
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<Vector_Base> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<Vector_Base> ALL; };
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet<IVP> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IVPR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IVPR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			             this->SetName("IVPR");                     this->SetDescription("Interrupt Vector Prefix Register");
			Vector_Base::SetName("Vector_Base"); Vector_Base::SetDescription("Vector Base");
			IVP        ::SetName("IVP");         IVP        ::SetDescription("Interrupt Vector Prefix");
		}
	};
	
	// Vector Save/Restore Register
	struct VRSAVE : NonPrivilegedSPR<VRSAVE, 256>
	{
		typedef NonPrivilegedSPR<VRSAVE, 256> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		VRSAVE(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		VRSAVE(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("VRSAVE"); this->SetDescription("Vector Save/Restore Register"); }
	};
	
	// User SPR General 0
	struct USPRG0 : NonPrivilegedSPR<USPRG0, 256>
	{
		typedef NonPrivilegedSPR<USPRG0, 256> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		USPRG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		USPRG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("USPRG0"); this->SetDescription("User SPR General 0"); }
	};
	
	// SPR General 0
	struct SPRG0 : PrivilegedSPR<SPRG0, 272>
	{
		typedef PrivilegedSPR<SPRG0, 272> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG0"); this->SetDescription("SPR General 0"); }
	};

	// SPR General 1
	struct SPRG1 : PrivilegedSPR<SPRG1, 273>
	{
		typedef PrivilegedSPR<SPRG1, 273> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG1"); this->SetDescription("SPR General 1"); }
	};

	// SPR General 2
	struct SPRG2 : PrivilegedSPR<SPRG2, 274>
	{
		typedef PrivilegedSPR<SPRG2, 274> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG2"); this->SetDescription("SPR General 2"); }
	};

	// SPR General 3
	struct SPRG3 : PrivilegedSPR<SPRG3, 275>
	{
		typedef PrivilegedSPR<SPRG3, 275> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SPRG3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPRG3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG3"); this->SetDescription("SPR General 3"); }
	};
	
	// SPR General 4
	struct SPRG4 : PrivilegedSPR<SPRG4, 276>
	{
		typedef PrivilegedSPR<SPRG4, 276> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		struct SPR260 : AltSLR<SPRG4, SLR_SPR_SPACE, 260, SLR_RO, SLR_NON_PRIVILEGED>
		{
			typedef AltSLR<SPRG4, SLR_SPR_SPACE, 260, SLR_RO, SLR_NON_PRIVILEGED> Super;
			
			SPR260(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) : Super(_cpu, _sprg4) {}
		};

		SPRG4(typename CONFIG::CPU *_cpu) : Super(_cpu), user_read_only_spr(_cpu, this) { Init(); }
		SPRG4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value), user_read_only_spr(_cpu, this) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG4"); this->SetDescription("SPR General 4"); }
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC740     , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC745     , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC750     , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC755     , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7400    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7410    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7441    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7445    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7447    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7448    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7450    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7451    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7455    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(MPC7457    , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) {}; };
		CASE_ENUM_TRAIT(PPC405     , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) : spr260(_cpu, _sprg4) {}; SPR260 spr260; };
		CASE_ENUM_TRAIT(PPC440     , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) : spr260(_cpu, _sprg4) {}; SPR260 spr260; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) : spr260(_cpu, _sprg4) {}; SPR260 spr260; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { _(typename CONFIG::CPU *_cpu, SPRG4 *_sprg4) : spr260(_cpu, _sprg4) {}; SPR260 spr260; };
		ENUM_TRAIT(CONFIG::MODEL, _) user_read_only_spr;
	};

	// SPR General 5
	struct SPRG5 : PrivilegedSPR<SPRG5, 277>
	{
		typedef PrivilegedSPR<SPRG5, 277> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		struct SPR261 : AltSLR<SPRG5, SLR_SPR_SPACE, 261, SLR_RO, SLR_NON_PRIVILEGED>
		{
			typedef AltSLR<SPRG5, SLR_SPR_SPACE, 261, SLR_RO, SLR_NON_PRIVILEGED> Super;
			
			SPR261(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) : Super(_cpu, _sprg5) {}
		};

		SPRG5(typename CONFIG::CPU *_cpu) : Super(_cpu), user_read_only_spr(_cpu, this) { Init(); }
		SPRG5(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value), user_read_only_spr(_cpu, this) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG5"); this->SetDescription("SPR General 5"); }
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC740     , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC745     , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC750     , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC755     , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7400    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7410    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7441    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7445    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7447    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7448    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7450    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7451    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7455    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(MPC7457    , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) {}; };
		CASE_ENUM_TRAIT(PPC405     , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) : spr261(_cpu, _sprg5) {}; SPR261 spr261; };
		CASE_ENUM_TRAIT(PPC440     , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) : spr261(_cpu, _sprg5) {}; SPR261 spr261; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) : spr261(_cpu, _sprg5) {}; SPR261 spr261; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { _(typename CONFIG::CPU *_cpu, SPRG5 *_sprg5) : spr261(_cpu, _sprg5) {}; SPR261 spr261; };
		ENUM_TRAIT(CONFIG::MODEL, _) user_read_only_spr;
	};

	// SPR General 6
	struct SPRG6 : PrivilegedSPR<SPRG6, 278>
	{
		typedef PrivilegedSPR<SPRG6, 278> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		struct SPR262 : AltSLR<SPRG6, SLR_SPR_SPACE, 262, SLR_RO, SLR_NON_PRIVILEGED>
		{
			typedef AltSLR<SPRG6, SLR_SPR_SPACE, 262, SLR_RO, SLR_NON_PRIVILEGED> Super;
			
			SPR262(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) : Super(_cpu, _sprg6) {}
		};

		SPRG6(typename CONFIG::CPU *_cpu) : Super(_cpu), user_read_only_spr(_cpu, this) { Init(); }
		SPRG6(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value), user_read_only_spr(_cpu, this) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG6"); this->SetDescription("SPR General 6"); }
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC740     , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC745     , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC750     , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC755     , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7400    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7410    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7441    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7445    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7447    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7448    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7450    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7451    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7455    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(MPC7457    , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) {}; };
		CASE_ENUM_TRAIT(PPC405     , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) : spr262(_cpu, _sprg6) {}; SPR262 spr262; };
		CASE_ENUM_TRAIT(PPC440     , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) : spr262(_cpu, _sprg6) {}; SPR262 spr262; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) : spr262(_cpu, _sprg6) {}; SPR262 spr262; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { _(typename CONFIG::CPU *_cpu, SPRG6 *_sprg6) : spr262(_cpu, _sprg6) {}; SPR262 spr262; };
		ENUM_TRAIT(CONFIG::MODEL, _) user_read_only_spr;
	};

	// SPR General 7
	struct SPRG7 : PrivilegedSPR<SPRG7, 279>
	{
		typedef PrivilegedSPR<SPRG7, 279> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		struct SPR263 : AltSLR<SPRG7, SLR_SPR_SPACE, 263, SLR_RO, SLR_NON_PRIVILEGED>
		{
			typedef AltSLR<SPRG7, SLR_SPR_SPACE, 263, SLR_RO, SLR_NON_PRIVILEGED> Super;
			
			SPR263(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) : Super(_cpu, _sprg7) {}
		};

		SPRG7(typename CONFIG::CPU *_cpu) : Super(_cpu), user_read_only_spr(_cpu, this) { Init(); }
		SPRG7(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value), user_read_only_spr(_cpu, this) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("SPRG7"); this->SetDescription("SPR General 7"); }
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC740     , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC745     , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC750     , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC755     , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7400    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7410    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7441    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7445    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7447    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7448    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7450    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7451    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7455    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(MPC7457    , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) {}; };
		CASE_ENUM_TRAIT(PPC405     , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) : spr263(_cpu, _sprg7) {}; SPR263 spr263; };
		CASE_ENUM_TRAIT(PPC440     , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) : spr263(_cpu, _sprg7) {}; SPR263 spr263; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) : spr263(_cpu, _sprg7) {}; SPR263 spr263; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { _(typename CONFIG::CPU *_cpu, SPRG7 *_sprg7) : spr263(_cpu, _sprg7) {}; SPR263 spr263; };
		ENUM_TRAIT(CONFIG::MODEL, _) user_read_only_spr;
	};
	
	// External Access Register
	struct EAR : PrivilegedSPR<EAR, 282>
	{
		typedef PrivilegedSPR<EAR, 282> Super;
		
		struct E   : Field<E  , 0     > {}; // Enable bit
		struct RID : Field<RID, 26, 31> {}; // Resource ID

		typedef FieldSet<E, RID> ALL;
		
		EAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		EAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("EAR"); this->SetDescription("External Access Register");
			
			E  ::SetName("E");   E  ::SetDescription("Enable bit");
			RID::SetName("RID"); RID::SetDescription("Resource ID");
		}
	};

	SWITCH_ENUM_TRAIT(Model, PIR_Mapping);
	CASE_ENUM_TRAIT(MPC7400    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7410    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7441    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7445    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7447    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7447A   , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7448    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7450    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7451    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7455    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(MPC7457    , PIR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(PPC440     , PIR_Mapping) { static const unsigned int SPR_NUM = 286; };
	CASE_ENUM_TRAIT(E200Z425BN3, PIR_Mapping) { static const unsigned int SPR_NUM = 286; };
	CASE_ENUM_TRAIT(E200Z710N3 , PIR_Mapping) { static const unsigned int SPR_NUM = 286; };
	
	// Processor ID Register
	struct PIR : PrivilegedSPR<PIR, PIR_Mapping<CONFIG::MODEL>::SPR_NUM>
	{
		typedef PrivilegedSPR<PIR, PIR_Mapping<CONFIG::MODEL>::SPR_NUM> Super;
		
		struct PROCID   : Field<PROCID  , 0 , 31> {}; // Unique, core-specific value
		struct ID_0_23  : Field<ID_0_23 , 0 , 23> {}; // bits 0:23
		struct ID_24_31 : Field<ID_24_31, 24, 31> {}; // bits 24:31
		struct PIN      : Field<PIN     , 28, 31> {}; // Processor Identification Number
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7400    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7410    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7441    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7445    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7447    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7448    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7450    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7451    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7455    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(MPC7457    , _) { typedef FieldSet<PROCID> ALL; };
		CASE_ENUM_TRAIT(PPC440     , _) { typedef FieldSet<PIN> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { typedef FieldSet<ID_0_23, ID_24_31> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ID_0_23, ID_24_31> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PIR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("PIR");
			this->SetDescription("Processor ID Register");
			
			PROCID  ::SetName("PROCID");   PROCID  ::SetDescription("Unique, core-specific value");
			ID_0_23 ::SetName("ID_0_23");  ID_0_23 ::SetDescription("bits 0:23");
			ID_24_31::SetName("ID_24_31"); ID_24_31::SetDescription("bits 24:31");
			PIN     ::SetName("PIN");      PIN     ::SetDescription("Processor Identification Number");
		}
	};
	
	// Processor Version Register
	struct PVR : NonPrivilegedSPR<PVR, 287>
	{
		typedef NonPrivilegedSPR<PVR, 287> Super;
		
		struct Version   : Field<Version  , 0 , 15> {}; // Version
		struct Revision  : Field<Revision , 16, 31> {}; // Revision
		
		struct OWN       : Field<OWN, 0 , 11> {}; // Owner Identifier
		struct PCF       : Field<PCF, 12, 15> {}; // Processor Core Family
		struct CAS       : Field<CAS, 16, 21> {}; // Cache Array Size
		struct PCL       : Field<PCL, 22, 25> {}; // Processor Core Version
		struct AID       : Field<AID, 26, 31> {}; // ASIC Identifier
		struct PVN       : Field<PVN, 12, 31> {}; // Processor Version Number
		
		struct MANID         : Field<MANID    , 0, 3>   {}; // Manufacturer ID
		struct Type          : Field<Type     , 6, 11>  {}; // Processor Type
		struct Version_12_15 : Field<Version  , 12, 15> {}; // Version
		struct MBG_Use       : Field<MBG_Use  , 16, 19> {}; // System Variant
		struct Minor_Rev     : Field<Minor_Rev, 20, 23> {}; // Minor Revision
		struct Major_Rev     : Field<Major_Rev, 24, 27> {}; // Major Revision
		struct MBG_ID        : Field<MBG_ID   , 28, 31> {}; // Freescale Business Group responsible for a particular mask set

		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC601  , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC603E , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC604E , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC740  , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC745  , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC750  , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC755  , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7400 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7410 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7441 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7445 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7451 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7455 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _)    { typedef FieldSet<Version, Revision> ALL; };
		CASE_ENUM_TRAIT(PPC405, _)      { typedef FieldSet<OWN, PCF, CAS, PCL, AID> ALL; };
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet<OWN, PVN> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<MANID, Type, Version_12_15, MBG_Use, Minor_Rev, MBG_ID> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<MANID, Type, Version_12_15, MBG_Use, Minor_Rev, MBG_ID> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		PVR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PVR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("PVR"); this->SetDescription("Processor Version Register");
			
			Version ::SetName("Version");  Version ::SetDescription("Version");
			Revision::SetName("Revision"); Revision::SetDescription("Revision");
			
			OWN::SetName("OWN"); OWN::SetDescription("Owner Identifier");
			PCF::SetName("PCF"); PCF::SetDescription("Processor Core Family");
			CAS::SetName("CAS"); CAS::SetDescription("Cache Array Size");
			PCL::SetName("PCL"); PCL::SetDescription("Processor Core Version");
			AID::SetName("AID"); AID::SetDescription("ASIC Identifier");
			PVN::SetName("PWN"); PVN::SetDescription("Processor Version Number");
			
			MANID          ::SetName("MANID");     MANID        ::SetDescription("Manufacturer ID");
			Type           ::SetName("Type");      Type         ::SetDescription("Processor Type");
			Version_12_15  ::SetName("Version");   Version_12_15::SetDescription("Version");
			MBG_Use        ::SetName("MBG_Use");   MBG_Use      ::SetDescription("System Variant");
			Minor_Rev      ::SetName("Minor_Rev"); Minor_Rev    ::SetDescription("Minor Revision");
			Major_Rev      ::SetName("Major_Rev"); Major_Rev    ::SetDescription("Major Revision");
			MBG_ID         ::SetName("MBG_ID");    MBG_ID       ::SetDescription("Freescale Business Group responsible for a particular mask set");
		}
	};

	// Debug Status Register
	struct DBSR : PrivilegedSPR<DBSR, 304>
	{
		typedef PrivilegedSPR<DBSR, 304> Super;
		
		struct IDE      : Field<IDE     , 0>      {}; // Imprecise Debug Event
		struct UDE      : Field<UDE     , 1>      {}; // Unconditional Debug Event
		struct MRR      : Field<MRR     , 2, 3>   {}; // Most Recent Reset
		struct ICMP     : Field<ICMP    , 4>      {}; // Instruction Complete Debug Event
		struct BRT      : Field<BRT     , 5>      {}; // Branch Taken Debug Event
		struct IRPT     : Field<IRPT    , 6>      {}; // Interrupt Taken Debug Event
		struct TRAP     : Field<TRAP    , 7>      {}; // Trap Taken Debug Event
		struct IAC1     : Field<IAC1    , 8>      {}; // Instruction Address Compare 1 Debug Event
		struct IAC2     : Field<IAC2    , 9>      {}; // Instruction Address Compare 2 Debug Event
		struct IAC3     : Field<IAC3    , 10>     {}; // Instruction Address Compare 3 Debug Event
		struct IAC4     : Field<IAC4    , 11>     {}; // Instruction Address Compare 4 Debug Event
		struct IAC4_8   : Field<IAC4_8  , 11>     {}; // Instruction Address Compare 4-8 Debug Event
		struct DAC1R    : Field<DAC1R   , 12>     {}; // Data Address Compare 1 Read Debug Event
		struct DAC1W    : Field<DAC1W   , 13>     {}; // Data Address Compare 1 Write Debug Event
		struct DAC2R    : Field<DAC2R   , 14>     {}; // Data Address Compare 2 Read Debug Event
		struct DAC2W    : Field<DAC2W   , 15>     {}; // Data Address Compare 2 Write Debug Event
		struct RET      : Field<RET     , 16>     {}; // Return Debug Event
		struct DEVT1    : Field<DEVT1   , 21>     {}; // External Debug Event 1 Debug Event
		struct DEVT2    : Field<DEVT2   , 22>     {}; // External Debug Event 2 Debug Event
		struct PMI      : Field<PMI     , 23>     {}; // Performance Monitor Interrupt Debug Event
		struct MPU      : Field<MPU     , 24>     {}; // Memory Protection Unit Debug Event
		struct CIRPT    : Field<CIRPT   , 25>     {}; // Critical Interrupt Taken Debug Event
		struct CRET     : Field<CRET    , 26>     {}; // Critical Return Debug Event
		struct DNI      : Field<DNI     , 27>     {}; // Debug Notify Interrupt (se_dni)
		struct DAC_OFST : Field<DAC_OFST, 28, 30> {}; // Data Address Compare Offset
		struct IAC12ATS : Field<IAC12ATS, 30>     {}; // Instruction Address Compare 1/2 Auto-Toggle Status
		struct IAC34ATS : Field<IAC34ATS, 31>     {}; // Instruction Address Compare 3/4 Auto-Toggle Status
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet< IDE, UDE, MRR, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4, DAC1R, DAC1W
		                                                  , DAC2R, DAC2W, RET, IAC12ATS, IAC34ATS> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< IDE, UDE, MRR, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4_8, DAC1R, DAC1W
		                                                  , DAC2R, DAC2W, RET, DEVT1, DEVT2, PMI, MPU, CIRPT, CRET, DNI, DAC_OFST> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< IDE, UDE, MRR, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4_8, DAC1R, DAC1W
		                                                  , DAC2R, DAC2W, RET, DEVT1, DEVT2, PMI, MPU, CIRPT, CRET, DNI, DAC_OFST> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual bool MoveTo(uint32_t value) { this->Set(this->Get() & ~value); return true; } // W1C
	private:
		void Init()
		{
			          this->SetName("DBSR");               this->SetDescription("Debug Status Register");
			IDE     ::SetName("IDE");      IDE     ::SetDescription("Imprecise Debug Event");
			UDE     ::SetName("UDE");      UDE     ::SetDescription("Unconditional Debug Event");
			MRR     ::SetName("MRR");      MRR     ::SetDescription("Most Recent Reset");
			ICMP    ::SetName("ICMP");     ICMP    ::SetDescription("Instruction Complete Debug Event");
			BRT     ::SetName("BRT");      BRT     ::SetDescription("Branch Taken Debug Event");
			IRPT    ::SetName("IRPT");     IRPT    ::SetDescription("Interrupt Taken Debug Event");
			TRAP    ::SetName("TRAP");     TRAP    ::SetDescription("Trap Taken Debug Event");
			IAC1    ::SetName("IAC1");     IAC1    ::SetDescription("Instruction Address Compare 1 Debug Event");
			IAC2    ::SetName("IAC2");     IAC2    ::SetDescription("Instruction Address Compare 2 Debug Event");
			IAC3    ::SetName("IAC3");     IAC3    ::SetDescription("Instruction Address Compare 3 Debug Event");
			IAC4    ::SetName("IAC4");     IAC4    ::SetDescription("Instruction Address Compare 4 Debug Event");
			IAC4_8  ::SetName("IAC4_8");   IAC4_8  ::SetDescription("Instruction Address Compare 4-8 Debug Event");
			DAC1R   ::SetName("DAC1R");    DAC1R   ::SetDescription("Data Address Compare 1 Read Debug Event");
			DAC1W   ::SetName("DAC1W");    DAC1W   ::SetDescription("Data Address Compare 1 Write Debug Event");
			DAC2R   ::SetName("DAC2R");    DAC2R   ::SetDescription("Data Address Compare 2 Read Debug Event");
			DAC2W   ::SetName("DAC2W");    DAC2W   ::SetDescription("Data Address Compare 2 Write Debug Event");
			RET     ::SetName("RET");      RET     ::SetDescription("Return Debug Event");
			DEVT1   ::SetName("DEVT1");    DEVT1   ::SetDescription("External Debug Event 1 Debug Event");
			DEVT2   ::SetName("DEVT2");    DEVT2   ::SetDescription("External Debug Event 2 Debug Event");
			PMI     ::SetName("PMI");      PMI     ::SetDescription("Performance Monitor Interrupt Debug Event");
			MPU     ::SetName("MPU");      MPU     ::SetDescription("Memory Protection Unit Debug Event");
			CIRPT   ::SetName("CIRPT");    CIRPT   ::SetDescription("Critical Interrupt Taken Debug Event");
			CRET    ::SetName("CRET");     CRET    ::SetDescription("Critical Return Debug Event");
			DNI     ::SetName("DNI");      DNI     ::SetDescription("Debug Notify Interrupt (se_dni)");
			DAC_OFST::SetName("DAC_OFST"); DAC_OFST::SetDescription("Data Address Compare Offset");
			IAC12ATS::SetName("IAC12ATS"); IAC12ATS::SetDescription("Instruction Address Compare 1/2 Auto-Toggle Status");
			IAC34ATS::SetName("IAC34ATS"); IAC34ATS::SetDescription("Instruction Address Compare 3/4 Auto-Toggle Status");
		}
	};
	
	// Debug Control Register 0
	struct DBCR0 : PrivilegedSPR<DBCR0, 308>
	{
		typedef PrivilegedSPR<DBCR0, 308> Super;
		
		struct EDM   : Field<EDM  , 0>      {}; // External Debug Mode
		struct IDM   : Field<IDM  , 1>      {}; // Internal Debug Mode
		struct RST   : Field<RST  , 2, 3>   {}; // Reset Control
		struct ICMP  : Field<ICMP , 4>      {}; // Instruction Complete Debug Event Enable
		struct BRT   : Field<BRT  , 5>      {}; // Branch Taken Debug Event Enable
		struct IRPT  : Field<IRPT , 6>      {}; // Interrupt Taken Debug Event Enable
		struct TRAP  : Field<TRAP , 7>      {}; // Trap Taken Debug Event Enable
		struct IAC1  : Field<IAC1 , 8>      {}; // Instruction Address Compare 1 Debug Event Enable
		struct IAC2  : Field<IAC2 , 9>      {}; // Instruction Address Compare 2 Debug Event Enable
		struct IAC3  : Field<IAC3 , 10>     {}; // Instruction Address Compare 3 Debug Event Enable
		struct IAC4  : Field<IAC4 , 11>     {}; // Instruction Address Compare 4 Debug Event Enable
		struct DAC1  : Field<DAC1 , 12, 13> {}; // Data Address Compare 1 Debug Event Enable
		struct DAC1R : Field<DAC1R, 12    > {}; // Data Address Compare 1 Read Debug Event Enable
		struct DAC1W : Field<DAC1W, 13    > {}; // Data Address Compare 1 Write Debug Event Enable
		struct DAC2  : Field<DAC2 , 14, 15> {}; // Data Address Compare 2 Debug Event Enable
		struct DAC2R : Field<DAC2R, 14    > {}; // Data Address Compare 2 Read Debug Event Enable
		struct DAC2W : Field<DAC2W, 15    > {}; // Data Address Compare 2 Write Debug Event Enable
		struct RET   : Field<RET  , 16>     {}; // Return Debug Event Enable
		struct IAC5  : Field<IAC5 , 17>     {}; // Instruction Address Compare 5 Debug Event Enable
		struct IAC6  : Field<IAC6 , 18>     {}; // Instruction Address Compare 6 Debug Event Enable
		struct IAC7  : Field<IAC7 , 19>     {}; // Instruction Address Compare 7 Debug Event Enable
		struct IAC8  : Field<IAC8 , 20>     {}; // Instruction Address Compare 8 Debug Event Enable
		struct DEVT1 : Field<DEVT1, 21>     {}; // External Debug Event 1 Enable
		struct DEVT2 : Field<DEVT2, 22>     {}; // External Debug Event 2 Enable
		struct DCNT1 : Field<DCNT1, 23>     {}; // Debug Counter 1 Debug Event Enable
		struct DCNT2 : Field<DCNT2, 24>     {}; // Debug Counter 2 Debug Event Enable
		struct CIRPT : Field<CIRPT, 25>     {}; // Critical Interrupt Taken Debug Event Enable
		struct CRET  : Field<CRET , 26>     {}; // Critical Return Debug Event Enable
		struct FT    : Field<FT   , 31>     {}; // Freeze Timers on Debug Event
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet< EDM, IDM, RST, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4, DAC1R, DAC1W, DAC2R, DAC2W> ALL; }; 
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< EDM, IDM, RST, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4, DAC1, DAC2
		                                                  , RET, IAC5, IAC6, IAC7, IAC8, DEVT1, DEVT2, DCNT1, DCNT2, CIRPT, CRET, FT> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< EDM, IDM, RST, ICMP, BRT, IRPT, TRAP, IAC1, IAC2, IAC3, IAC4, DAC1, DAC2, RET
		                                                  , IAC5, IAC6, IAC7, IAC8, DEVT1, DEVT2, DCNT1, DCNT2, CIRPT, CRET, FT> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			       this->SetName("DBCR0");        this->SetDescription("Debug Control Register 0");
			EDM  ::SetName("EDM");   EDM  ::SetDescription("External Debug Mode");
			IDM  ::SetName("IDM");   IDM  ::SetDescription("Internal Debug Mode");
			RST  ::SetName("RST");   RST  ::SetDescription("Reset Control");
			ICMP ::SetName("ICMP");  ICMP ::SetDescription("Instruction Complete Debug Event Enable"); 
			BRT  ::SetName("BRT");   BRT  ::SetDescription("Branch Taken Debug Event Enable");
			IRPT ::SetName("IRPT");  IRPT ::SetDescription("Interrupt Taken Debug Event Enable"); 
			TRAP ::SetName("TRAP");  TRAP ::SetDescription("Trap Taken Debug Event Enable"); 
			IAC1 ::SetName("IAC1");  IAC1 ::SetDescription("Instruction Address Compare 1 Debug Event Enable"); 
			IAC2 ::SetName("IAC2");  IAC2 ::SetDescription("Instruction Address Compare 2 Debug Event Enable"); 
			IAC3 ::SetName("IAC3");  IAC3 ::SetDescription("Instruction Address Compare 3 Debug Event Enable"); 
			IAC4 ::SetName("IAC4");  IAC4 ::SetDescription("Instruction Address Compare 4 Debug Event Enable"); 
			DAC1 ::SetName("DAC1");  DAC1 ::SetDescription("Data Address Compare 1 Debug Event Enable"); 
			DAC1R::SetName("DAC1R"); DAC1R::SetDescription("Data Address Compare 1 Read Debug Event Enable");  
			DAC1W::SetName("DAC1W"); DAC1W::SetDescription("Data Address Compare 1 Write Debug Event Enable");  
			DAC2 ::SetName("DAC2");  DAC2 ::SetDescription("Data Address Compare 2 Debug Event Enable"); 
			DAC2R::SetName("DAC2R"); DAC2R::SetDescription("Data Address Compare 2 Read Debug Event Enable");  
			DAC2W::SetName("DAC2W"); DAC2W::SetDescription("Data Address Compare 2 Write Debug Event Enable");  
			RET  ::SetName("RET");   RET  ::SetDescription("Return Debug Event Enable");
			IAC5 ::SetName("IAC5");  IAC5 ::SetDescription("Instruction Address Compare 5 Debug Event Enable"); 
			IAC6 ::SetName("IAC6");  IAC6 ::SetDescription("Instruction Address Compare 6 Debug Event Enable"); 
			IAC7 ::SetName("IAC7");  IAC7 ::SetDescription("Instruction Address Compare 7 Debug Event Enable"); 
			IAC8 ::SetName("IAC8");  IAC8 ::SetDescription("Instruction Address Compare 8 Debug Event Enable"); 
			DEVT1::SetName("DEVT1"); DEVT1::SetDescription("External Debug Event 1 Enable");  
			DEVT2::SetName("DEVT2"); DEVT2::SetDescription("External Debug Event 2 Enable");  
			DCNT1::SetName("DCNT1"); DCNT1::SetDescription("Debug Counter 1 Debug Event Enable");  
			DCNT2::SetName("DCNT2"); DCNT2::SetDescription("Debug Counter 2 Debug Event Enable");  
			CIRPT::SetName("CIRPT"); CIRPT::SetDescription("Critical Interrupt Taken Debug Event Enable");  
			CRET ::SetName("CRET");  CRET ::SetDescription("Critical Return Debug Event Enable"); 
			FT   ::SetName("FT");    FT   ::SetDescription("Freeze Timers on Debug Event");
		}
	};
	
	// Debug Control Register 1
	struct DBCR1 : PrivilegedSPR<DBCR1, 309>
	{
		typedef PrivilegedSPR<DBCR1, 309> Super;
		
		struct IAC1US  : Field<IAC1US , 0 , 1 > {}; // Instruction Address Compare 1 User/Supervisor Mode
		struct IAC1ER  : Field<IAC1ER , 2 , 3 > {}; // Instruction Address Compare 1 Effective/Real Mode
		struct IAC2US  : Field<IAC2US , 4 , 5 > {}; // Instruction Address Compare 2 User/Supervisor Mode
		struct IAC2ER  : Field<IAC2ER , 6 , 7 > {}; // Instruction Address Compare 2 Effective/Real Mode
		struct IAC12M  : Field<IAC12M , 8 , 9 > {}; // Instruction Address Compare 1/2 Mode
		struct IAC12AT : Field<IAC12AT, 15    > {}; // Instruction Address Compare 1/2 Auto-Toggle Enable 
		struct IAC3US  : Field<IAC3US , 16, 17> {}; // Instruction Address Compare 3 User/Supervisor Mode
		struct IAC3ER  : Field<IAC3ER , 18, 19> {}; // Instruction Address Compare 3 Effective/Real Mode
		struct IAC4US  : Field<IAC4US , 20, 21> {}; // Instruction Address Compare 4 User/Supervisor Mode
		struct IAC4ER  : Field<IAC4ER , 22, 23> {}; // Instruction Address Compare 4 Effective/Real Mode
		struct IAC34M  : Field<IAC34M , 24, 25> {}; // Instruction Address Compare 3/4 Mode
		struct IAC34AT : Field<IAC34AT, 31    > {}; // Instruction Address Compare 3/4 Auto-Toggle Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet<IAC1US, IAC1ER, IAC2US, IAC2ER, IAC12M, IAC12AT, IAC3US, IAC3ER, IAC4US, IAC4ER, IAC34M, IAC34AT> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IAC1US, IAC1ER, IAC2US, IAC2ER, IAC12M, IAC3US, IAC3ER, IAC4US, IAC4ER, IAC34M> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IAC1US, IAC1ER, IAC2US, IAC2ER, IAC12M, IAC3US, IAC3ER, IAC4US, IAC4ER, IAC34M> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			   this->SetName("DBCR1");      this->SetDescription("Debug Control Register 1");
			IAC1US ::SetName("IAC1US");  IAC1US ::SetDescription("Instruction Address Compare 1 User/Supervisor Mode");
			IAC1ER ::SetName("IAC1ER");  IAC1ER ::SetDescription("Instruction Address Compare 1 Effective/Real Mode");
			IAC2US ::SetName("IAC2US");  IAC2US ::SetDescription("Instruction Address Compare 2 User/Supervisor Mode");
			IAC2ER ::SetName("IAC2ER");  IAC2ER ::SetDescription("Instruction Address Compare 2 Effective/Real Mode");
			IAC12M ::SetName("IAC12M");  IAC12M ::SetDescription("Instruction Address Compare 1/2 Mode");
			IAC12AT::SetName("IAC12AT"); IAC12AT::SetDescription("Instruction Address Compare 1/2 Auto-Toggle Enable");
			IAC3US ::SetName("IAC3US");  IAC3US ::SetDescription("Instruction Address Compare 3 User/Supervisor Mode");
			IAC3ER ::SetName("IAC3ER");  IAC3ER ::SetDescription("Instruction Address Compare 3 Effective/Real Mode");
			IAC4US ::SetName("IAC4US");  IAC4US ::SetDescription("Instruction Address Compare 4 User/Supervisor Mode");
			IAC4ER ::SetName("IAC4ER");  IAC4ER ::SetDescription("Instruction Address Compare 4 Effective/Real Mode");
			IAC34M ::SetName("IAC34M");  IAC34M ::SetDescription("Instruction Address Compare 3/4 Mode");
			IAC34AT::SetName("IAC34AT"); IAC34AT::SetDescription("Instruction Address Compare 3/4 Auto-Toggle Enable");
		}
	};
	
	// Debug Control Register 2
	struct DBCR2 : PrivilegedSPR<DBCR2, 310>
	{
		typedef PrivilegedSPR<DBCR2, 310> Super;
		
		struct DAC1US  : Field<DAC1US , 0 , 1 > {}; // Data Address Compare 1 User/Supervisor Mode
		struct DAC1ER  : Field<DAC1ER , 2 , 3 > {}; // Data Address Compare 1 Effective/Read Mode
		struct DAC2US  : Field<DAC2US , 4 , 5 > {}; // Data Address Compare 2 User/Supervisor Mode
		struct DAC2ER  : Field<DAC2ER , 6 , 7 > {}; // Data Address Compare 2 Effective/Read Mode
		struct DAC12M  : Field<DAC12M , 8 , 9 > {}; // Data Address Compare 1/2 Mode
		struct DAC12A  : Field<DAC12A , 10    > {}; // Data Address Compare 1/2 Asynchronous
		struct DAC1LNK : Field<DAC1LNK, 10>     {}; // Data Address Compare 1 Linked
		struct DAC2LNK : Field<DAC2LNK, 11>     {}; // Data Address Compare 2 Linked
		struct DVC1M   : Field<DVC1M  , 12, 13> {}; // Data Value Compare 1 Mode
		struct DVC2M   : Field<DVC2M  , 14, 15> {}; // Data Value Compare 2 Mode
		
		SWITCH_ENUM_TRAIT(Model, _1);
		CASE_ENUM_TRAIT(PPC440, _1)      { struct DVC1BE : Field<DVC1BE , 20, 23> {}; }; 
		CASE_ENUM_TRAIT(E200Z710N3, _1)  { struct DVC1BE : Field<DVC1BE , 16, 23> {}; };
		CASE_ENUM_TRAIT(E200Z425BN3, _1) { struct DVC1BE : Field<DVC1BE , 16, 23> {}; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _1)::DVC1BE DVC1BE; // Data Value Compare 1 Byte Enables
		
		SWITCH_ENUM_TRAIT(Model, _2);
		CASE_ENUM_TRAIT(PPC440, _2)      { struct DVC2BE : Field<DVC2BE , 28, 31> {}; };
		CASE_ENUM_TRAIT(E200Z710N3, _2)  { struct DVC2BE : Field<DVC2BE , 24, 31> {}; };
		CASE_ENUM_TRAIT(E200Z425BN3, _2) { struct DVC2BE : Field<DVC2BE , 24, 31> {}; };  
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _2)::DVC2BE DVC2BE; // Data Value Compare 2 Byte Enables
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet<DAC1US, DAC1ER, DAC2US, DAC2ER, DAC12M, DAC12A, DVC1M, DVC2M, DVC1BE, DVC2BE> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DAC1US, DAC1ER, DAC2US, DAC2ER, DAC12M, DAC1LNK, DAC2LNK, DVC1M, DVC2M, DVC1BE, DVC2BE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DAC1US, DAC1ER, DAC2US, DAC2ER, DAC12M, DAC1LNK, DAC2LNK, DVC1M, DVC2M, DVC1BE, DVC2BE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			   this->SetName("DBCR2");      this->SetDescription("Debug Control Register 2");
			DAC1US ::SetName("DAC1US");  DAC1US ::SetDescription("Data Address Compare 1 User/Supervisor Mode");
			DAC1ER ::SetName("DAC1ER");  DAC1ER ::SetDescription("Data Address Compare 1 Effective/Read Mode");
			DAC2US ::SetName("DAC2US");  DAC2US ::SetDescription("Data Address Compare 2 User/Supervisor Mode");
			DAC2ER ::SetName("DAC2ER");  DAC2ER ::SetDescription("Data Address Compare 2 Effective/Read Mode");
			DAC12M ::SetName("DAC12M");  DAC12M ::SetDescription("Data Address Compare 1/2 Mode");
			DAC12A ::SetName("DAC12A");  DAC12A ::SetDescription("Data Address Compare 1/2 Asynchronous");
			DAC1LNK::SetName("DAC1LNK"); DAC1LNK::SetDescription("Data Address Compare 1 Linked");
			DAC2LNK::SetName("DAC2LNK"); DAC2LNK::SetDescription("Data Address Compare 2 Linked");
			DVC1M  ::SetName("DVC1M");   DVC1M  ::SetDescription("Data Value Compare 1 Mode");
			DVC2M  ::SetName("DVC2M");   DVC2M  ::SetDescription("Data Value Compare 2 Mode");
			DVC1BE ::SetName("DVC1BE");  DVC1BE ::SetDescription("Data Value Compare 1 Byte Enables");
			DVC2BE ::SetName("DVC2BE");  DVC2BE ::SetDescription("Data Value Compare 2 Byte Enables");
		}
	};
	
	// Instruction Address Compare 1
	struct IAC1 : PrivilegedSPR<IAC1, 312>
	{
		typedef PrivilegedSPR<IAC1, 312> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC1"); this->SetDescription("Instruction Address Compare 1"); }
	};

	// Instruction Address Compare 2
	struct IAC2 : PrivilegedSPR<IAC2, 313>
	{
		typedef PrivilegedSPR<IAC2, 313> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC2"); this->SetDescription("Instruction Address Compare 2"); }
	};

	// Instruction Address Compare 3
	struct IAC3 : PrivilegedSPR<IAC3, 314>
	{
		typedef PrivilegedSPR<IAC3, 314> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC3"); this->SetDescription("Instruction Address Compare 3"); }
	};

	// Instruction Address Compare 4
	struct IAC4 : PrivilegedSPR<IAC4, 315>
	{
		typedef PrivilegedSPR<IAC4, 315> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC4(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC4"); this->SetDescription("Instruction Address Compare 4"); }
	};

	// Data Address Compare 1
	struct DAC1 : PrivilegedSPR<DAC1, 316>
	{
		typedef PrivilegedSPR<DAC1, 316> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAC1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("DAC1"); this->SetDescription("Data Address Compare 1"); }
	};

	// Data Address Compare 2
	struct DAC2 : PrivilegedSPR<DAC2, 317>
	{
		typedef PrivilegedSPR<DAC2, 317> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAC2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DAC2"); this->SetDescription("Data Address Compare 2"); }
	};

	// Data Value Compare 1
	struct DVC1 : PrivilegedSPR<DVC1, 318>
	{
		typedef PrivilegedSPR<DVC1, 318> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("DVC1"); this->SetDescription("Data Value Compare 1"); }
	};

	// Data Value Compare 2
	struct DVC2 : PrivilegedSPR<DVC2, 319>
	{
		typedef PrivilegedSPR<DVC2, 319> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("DVC2"); this->SetDescription("Data Value Compare 2"); }
	};
	
	// Timer Status Register
	struct TSR : PrivilegedSPR<TSR, 336>
	{
		typedef PrivilegedSPR<TSR, 336> Super;
		
		struct ENW : Field<ENW, 0   > {}; // Enable Next Watchdog Timer Exception
		struct WIS : Field<WIS, 1   > {}; // Watchdog Timer Interrupt Status
		struct WRS : Field<WRS, 2, 3> {}; // Watchdog Timer Reset Status
		struct DIS : Field<DIS, 4   > {}; // Decrementer Interrupt Status
		struct FIS : Field<FIS, 5   > {}; // Fixed Interval Timer Interrupt Status
		
		typedef FieldSet<ENW, WIS, WRS, DIS, FIS> ALL;
		
		TSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		TSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
		
		virtual bool MoveTo(uint32_t value) { this->Set(this->Get() & ~value); return true; } // W1C
	private:
		void Init()
		{
			this->SetName("TSR"); this->SetDescription("Timer Status Register");
			
			ENW::SetName("ENW"); ENW::SetDescription("Enable Next Watchdog Timer Exception");
			WIS::SetName("WIS"); WIS::SetDescription("Watchdog Timer Interrupt Status");
			WRS::SetName("WRS"); WRS::SetDescription("Watchdog Timer Reset Status");
			DIS::SetName("DIS"); DIS::SetDescription("Decrementer Interrupt Status");
			FIS::SetName("FIS"); FIS::SetDescription("Fixed Interval Timer Interrupt Status");
		}
	};
	
	// Timer Control Register
	struct TCR : PrivilegedSPR<TCR, 340>
	{
		typedef PrivilegedSPR<TCR, 340> Super;
		
		struct WP  : Field<WP , 0, 1> {}; // Watchdog Timer Period
		struct WRC : Field<WRC, 2, 3> {}; // Watchdog Timer Reset Control
		struct WIE : Field<WIE, 4   > {}; // Watchdog Timer Interrupt Enable
		struct DIE : Field<DIE, 5   > {}; // Decrementer Interrupt Enable
		struct FP  : Field<FP , 6, 7> {}; // Fixed Interval Timer Period
		struct FIE : Field<FIE, 8   > {}; // Fixed Interval Timer Interrupt Enable
		struct ARE : Field<ARE, 9   > {}; // Auto-Reload Enable
		
		typedef FieldSet<WP, WRC, WIE, DIE, FP, FIE, ARE> ALL;
		
		TCR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		TCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("TCR"); this->SetDescription("Timer Control Register");
			
			WP ::SetName("WP");  WP ::SetDescription("Watchdog Timer Period");
			WRC::SetName("WRC"); WRC::SetDescription("Watchdog Timer Reset Control");
			WIE::SetName("WIE"); WIE::SetDescription("Watchdog Timer Interrupt Enable");
			DIE::SetName("DIE"); DIE::SetDescription("Decrementer Interrupt Enable");
			FP ::SetName("FP");  FP ::SetDescription("Fixed Interval Timer Period");
			FIE::SetName("FIE"); FIE::SetDescription("Fixed Interval Timer Interrupt Enable");
			ARE::SetName("ARE"); ARE::SetDescription("Auto-Reload Enable");
		}
	};

	// IVOR
	template <unsigned int IVOR_NUM>
	struct IVOR : PrivilegedSPR<IVOR<IVOR_NUM>, 400 + IVOR_NUM>
	{
		typedef PrivilegedSPR<IVOR<IVOR_NUM>, 400 + IVOR_NUM> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		IVOR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IVOR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			std::stringstream name_sstr;
			name_sstr << "IVOR" << IVOR_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Interrupt Vector Offset Register " << IVOR_NUM;
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};
	
	typedef IVOR<0> IVOR0;
	typedef IVOR<1> IVOR1;
	typedef IVOR<2> IVOR2;
	typedef IVOR<3> IVOR3;
	typedef IVOR<4> IVOR4;
	typedef IVOR<5> IVOR5;
	typedef IVOR<6> IVOR6;
	typedef IVOR<7> IVOR7;
	typedef IVOR<8> IVOR8;
	typedef IVOR<9> IVOR9;
	typedef IVOR<10> IVOR10;
	typedef IVOR<11> IVOR11;
	typedef IVOR<12> IVOR12;
	typedef IVOR<13> IVOR13;
	typedef IVOR<14> IVOR14;
	typedef IVOR<15> IVOR15;

	// Thread ID
	struct TIR : ReadOnlyPrivilegedSPR<TIR, 446>
	{
		typedef ReadOnlyPrivilegedSPR<TIR, 446> Super;
		
		struct ALL : Field<ALL, 0, 31> {};

		TIR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		TIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("TIR"); this->SetDescription("Thread ID"); }
	};

	// Signal Processing Extension/Embedded Floating-point Status and Control Register
	struct SPEFSCR : NonPrivilegedSPR<SPEFSCR, 512>
	{
		typedef NonPrivilegedSPR<SPEFSCR, 512> Super;
		
		struct SOVH    : Field<SOVH,      0> {}; // Summary Integer overflow high
		struct OVH     : Field<OVH,       1> {}; // Integer overflow high
		struct FGH     : Field<FGH,       2> {}; // Embedded floating-point guard bit high
		struct FXH     : Field<FXH,       3> {}; // Embedded floating-point inexact bit high
		struct FINVH   : Field<FINVH,     4> {}; // Embedded Floating-point Invalid Operation / Input error
		struct FDBZH   : Field<FDBZH,     5> {}; // Embedded Floating-point Divide by Zero
		struct FUNFH   : Field<FUNFH,     6> {}; // Embedded Floating-point Underflow
		struct FOVFH   : Field<FOVFH,     7> {}; // Embedded Floating-point Overflow
		struct FINXS   : Field<FINXS,    10> {}; // Embedded Floating-point Inexact Sticky Flag
		struct FINVS   : Field<FINVS,    11> {}; // Enmedded Floating-point Invalid Operation Sticky Flag
		struct FDBZS   : Field<FDBZS,    12> {}; // Embedded Floating-point Divide by Zero Sticky Flag
		struct FUNFS   : Field<FUNFS,    13> {}; // Embedded Floating-point Underflow Sticky Flag
		struct FOVFS   : Field<FOVFS,    14> {}; // Embedded Floating-point Overflow Sticky Flag
		struct MODE    : Field<MODE ,    15> {}; // Embedded Floating-point Operating Mode
		struct MODE_RO : Field<MODE_RO,  15, 15, unisim::util::reg::core::SW_R_HW_RO> {}; // Embedded Floating-point Operating Mode
		struct SOV     : Field<SOV  ,    16> {}; // Summary Integer overflow
		struct OV      : Field<OV   ,    17> {}; // Integer overflow
		struct FG      : Field<FG,       18> {}; // Embedded Floating-point Guard bit
		struct FX      : Field<FX,       19> {}; // Embedded Floating-point Sticky bit
		struct FINV    : Field<FINV ,    20> {}; // Embedded Floating-point Invalid Operation / Input error
		struct FDBZ    : Field<FDBZ ,    21> {}; // Embedded Floating-point Divide by Zero
		struct FUNF    : Field<FUNF ,    22> {}; // Embedded Floating-point Underflow
		struct FOVF    : Field<FOVF ,    23> {}; // Embedded Floating-point Overflow
		struct FINXE   : Field<FINXE,    25> {}; // Embedded Floating-point Inexact Exception Enable
		struct FINVE   : Field<FINVE,    26> {}; // Embedded Floating-point Invalid Operation / Input Error Exception Enable
		struct FDBZE   : Field<FDBZE,    27> {}; // Embedded Floating-point Divide by Zero Exception Enable
		struct FUNFE   : Field<FUNFE,    28> {}; // Embedded Floating-point Underflow Exception Enable
		struct FOVFE   : Field<FOVFE,    29> {}; // Embedded Floating-point Overflow Exception Enable
		struct FRMC    : Field<FRMC , 30,31>     // Embedded Floating-point Rounding Mode Control
		{
			static uint32_t NEAREST() { return 0; }
			static uint32_t ZERO()    { return 1; }
			static uint32_t UP()      { return 2; }
			static uint32_t DOWN()    { return 3; }
		};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<FINXS, FINVS, FDBZS, FUNFS, FOVFS, MODE_RO, SOV, OV, FG, FX, FINV, FDBZ, FUNF, FOVF, FINXE, FINVE, FDBZE, FUNFE, FOVFE, FRMC> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<FINXS, FINVS, FDBZS, FUNFS, FOVFS, MODE_RO, SOV, OV, FG, FX, FINV, FDBZ, FUNF, FOVF, FINXE, FINVE, FDBZE, FUNFE, FOVFE, FRMC> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		SPEFSCR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SPEFSCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		bool SetInvalid( bool inv, bool invh=false )
		{
			this->template Set<FINV>( inv );
			this->template Set<FINVH>( invh );
			if (inv or invh)
			{
				this->template Set<FINVS>( true );
				if (this->template Get<FINVE>())
				{
					this->cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
					return false;
				}
			}
			return true;
		}
		
		bool SetDivideByZero( bool dbz, bool dbzh=false )
		{
			this->template Set<FDBZ>( dbz );
			this->template Set<FDBZH>( dbzh );
			if (dbz or dbzh)
			{
				this->template Set<FDBZS>( true );
				if (this->template Get<FDBZE>())
				{
					this->cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
					return false;
				}
			}
			return true;
		}

		bool SetOverflow( bool ovf, bool ovfh=false )
		{
			this->template Set<FOVF>( ovf );
			this->template Set<FOVFH>( ovfh );
			if (ovf or ovfh)
			{
				this->template Set<FOVFS>( true );
				if (this->template Get<FOVFE>())
				{
					this->cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
					return false;
				}
			}
			return true;
		}
		
		bool SetUnderflow( bool unf, bool unfh=false )
		{
			this->template Set<FUNF>( unf );
			this->template Set<FUNFH>( unfh );
			if (unf or unfh)
			{
				this->template Set<FUNFS>( true );
				if (this->template Get<FUNFE>())
				{
					this->cpu->template ThrowException<typename CONFIG::CPU::ProgramInterrupt::IllegalInstruction>();
					return false;
				}
			}
			return true;
		}
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			this->SetName("SPEFSCR"); this->SetDescription("Signal Processing Extension/Embedded Floating-point Status and Control Register");
			
			SOVH   ::SetName("SOVH");  SOVH   ::SetDescription("Summary Integer overflow high");
			OVH    ::SetName("OVH");   OVH    ::SetDescription("Integer overflow high");
			FGH    ::SetName("FGH");   FGH    ::SetDescription("Embedded floating-point guard bit high");
			FXH    ::SetName("FXH");   FXH    ::SetDescription("Embedded floating-point inexact bit high");
			FINVH  ::SetName("FINVH"); FINVH  ::SetDescription("Embedded Floating-point Invalid Operation / Input error");
			FDBZH  ::SetName("FDBZH"); FDBZH  ::SetDescription("Embedded Floating-point Divide by Zero");
			FUNFH  ::SetName("FUNFH"); FUNFH  ::SetDescription("Embedded Floating-point Underflow");
			FOVFH  ::SetName("FOVFH"); FOVFH  ::SetDescription("Embedded Floating-point Overflow");
			FINXS  ::SetName("FINXS"); FINXS  ::SetDescription("Embedded Floating-point Inexact Sticky Flag");
			FINVS  ::SetName("FINVS"); FINVS  ::SetDescription("Enmedded Floating-point Invalid Operation Sticky Flag");
			FDBZS  ::SetName("FDBZS"); FDBZS  ::SetDescription("Embedded Floating-point Divide by Zero Sticky Flag");
			FUNFS  ::SetName("FUNFS"); FUNFS  ::SetDescription("Embedded Floating-point Underflow Sticky Flag");
			FOVFS  ::SetName("FOVFS"); FOVFS  ::SetDescription("Embedded Floating-point Overflow Sticky Flag");
			MODE   ::SetName("MODE");  MODE   ::SetDescription("Embedded Floating-point Operating Mode");
			MODE_RO::SetName("MODE");  MODE_RO::SetDescription("Embedded Floating-point Operating Mode");
			SOV    ::SetName("SOV");   SOV    ::SetDescription("Summary Integer overflow");
			OV     ::SetName("OV");    OV     ::SetDescription("Integer overflow");
			FG     ::SetName("FG");    FG     ::SetDescription("Embedded Floating-point Guard bit");
			FX     ::SetName("FX");    FX     ::SetDescription("Embedded Floating-point Sticky bit");
			FINV   ::SetName("FINV");  FINV   ::SetDescription("Embedded Floating-point Invalid Operation / Input error");
			FDBZ   ::SetName("FDBZ");  FDBZ   ::SetDescription("Embedded Floating-point Divide by Zero");
			FUNF   ::SetName("FUNF");  FUNF   ::SetDescription("Embedded Floating-point Underflow");
			FOVF   ::SetName("FOVF");  FOVF   ::SetDescription("Embedded Floating-point Overflow");
			FINXE  ::SetName("FINXE"); FINXE  ::SetDescription("Embedded Floating-point Inexact Exception Enable");
			FINVE  ::SetName("FINVE"); FINVE  ::SetDescription("Embedded Floating-point Invalid Operation / Input Error Exception Enable");
			FDBZE  ::SetName("FDBZE"); FDBZE  ::SetDescription("Embedded Floating-point Divide by Zero Exception Enable");
			FUNFE  ::SetName("FUNFE"); FUNFE  ::SetDescription("Embedded Floating-point Underflow Exception Enable");
			FOVFE  ::SetName("FOVFE"); FOVFE  ::SetDescription("Embedded Floating-point Overflow Exception Enable");
			FRMC   ::SetName("FRMC");  FRMC   ::SetDescription("Embedded Floating-point Rounding Mode Control");
		}
	};
	
	enum
	{
		CARCH_HARVARD          = 0,
		CARCH_UNIFIED          = 1,
		CARCH_INSTRUCTION_ONLY = 2
	};
	
	enum
	{
		CREPL_LRU                = 0,
		CREPL_PLRU               = 1,
		CREPL_PSEUDO_ROUND_ROBIN = 2,
		CREPL_FIFO               = 3
	};
	
	// L1 Cache Configuration Register 0
	struct L1CFG0 : NonPrivilegedSPR<L1CFG0, 515>
	{
		typedef NonPrivilegedSPR<L1CFG0, 515> Super;
	
		struct CARCH   : Field<CARCH  , 0, 1>   {}; // Cache Architecture: 00=Harvard, 01=Unified, 10=Instruction only (L1CFG1 contains configuration information), 11=Reserved
		struct CWPA    : Field<CWPA   , 2>      {}; // Cache Way Partitioning Available
		struct DCFAHA  : Field<DCFAHA , 3>      {}; // Data Cache Flush All by Hardware Available
		struct DCFISWA : Field<DCFISWA, 4>      {}; // Data Cache Flush/Invalidate by Set and Way Available
		struct DCBSIZE : Field<DCBSIZE, 7, 8>   {}; // Data Cache Block Size: 00=32 bytes, 01=64 bytes, 10=128 bytes, 11=Reserved
		struct DCREPL  : Field<DCREPL , 9, 10>  {}; // Data Cache Replacement Policy: 00=True LRU, 01=Pseudo LRU, 10=Pseudo round robin, 11=FIFO
		struct DCLA    : Field<DCLA   , 11>     {}; // Data Cache Lockup APU Available
		struct DCECA   : Field<DCECA  , 12>     {}; // Data Cache Error Checking Available
		struct DCNWAY  : Field<DCNWAY , 13, 20> {}; // Data Cache Number of Ways (minus 1)
		struct DCSIZE  : Field<DCSIZE , 21, 31> {}; // Data Cache Size (in KB)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CARCH, CWPA, DCFAHA, DCFISWA, DCBSIZE, DCREPL, DCLA, DCECA, DCNWAY, DCSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CARCH, CWPA, DCFAHA, DCFISWA, DCBSIZE, DCREPL, DCLA, DCECA, DCNWAY, DCSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("L1CFG0");           this->SetDescription("L1 Cache Configuration Register 0");
			CARCH  ::SetName("CARCH");   CARCH  ::SetDescription("Cache Architecture");
			CWPA   ::SetName("CWPA");    CWPA   ::SetDescription("Cache Way Partitioning Available");
			DCFAHA ::SetName("DCFAHA");  DCFAHA ::SetDescription("Data Cache Flush All by Hardware Available");
			DCFISWA::SetName("DCFISWA"); DCFISWA::SetDescription("Data Cache Flush/Invalidate by Set and Way Available");
			DCBSIZE::SetName("DCBSIZE"); DCBSIZE::SetDescription("Data Cache Block Size");
			DCREPL ::SetName("DCREPL");  DCREPL ::SetDescription("Data Cache Replacement Policy");
			DCLA   ::SetName("DCLA");    DCLA   ::SetDescription("Data Cache Lockup APU Available");
			DCECA  ::SetName("DCECA");   DCECA  ::SetDescription("Data Cache Error Checking Available");
			DCNWAY ::SetName("DCNWAY");  DCNWAY ::SetDescription("Data Cache Number of Ways");
			DCSIZE ::SetName("DCSIZE");  DCSIZE ::SetDescription("Data Cache Size");
		}
	};

	// L1 Cache Configuration Register 1
	struct L1CFG1 : NonPrivilegedSPR<L1CFG1, 516>
	{
		typedef NonPrivilegedSPR<L1CFG1, 516> Super;
		
		struct ICFISWA : Field<ICFISWA, 4>      {}; // Instruction Cache Flush/Invalidate by Set and Way Available
		struct ICBSIZE : Field<ICBSIZE, 7, 8>   {}; // Instruction Cache Block Size
		struct ICREPL  : Field<ICREPL , 9, 10>  {}; // Instruction Cache Replacement Policy
		struct ICLA    : Field<ICLA   , 11>     {}; // Instruction Cache Locking APU Available
		struct ICECA   : Field<ICECA  , 12>     {}; // Instruction Cache Error Checking Available
		struct ICNWAY  : Field<ICNWAY , 13, 20> {}; // Instruction Cache Number of Ways (minus 1)
		struct ICSIZE  : Field<ICSIZE , 21, 31> {}; // Instruction Cache Size (in KB)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ICFISWA, ICBSIZE, ICREPL, ICLA, ICECA, ICNWAY, ICSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ICFISWA, ICBSIZE, ICREPL, ICLA, ICECA, ICNWAY, ICSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CFG1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CFG1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("L1CFG1");           this->SetDescription("L1 Cache Configuration Register 1");
			ICFISWA::SetName("ICFISWA"); ICFISWA::SetDescription("Instruction Cache Flush/Invalidate by Set and Way Available");
			ICBSIZE::SetName("ICBSIZE"); ICBSIZE::SetDescription("Instruction Cache Block Size");
			ICREPL ::SetName("ICREPL");  ICREPL ::SetDescription("Instruction Cache Replacement Policy");
			ICLA   ::SetName("ICLA");    ICLA   ::SetDescription("Instruction Cache Locking APU Available");
			ICECA  ::SetName("ICECA");   ICECA  ::SetDescription("Instruction Cache Error Checking Available");
			ICNWAY ::SetName("ICNWAY");  ICNWAY ::SetDescription("Instruction Cache Number of Ways");
			ICSIZE ::SetName("ICSIZE");  ICSIZE ::SetDescription("Instruction Cache Size");
		}
	};
	
	// Nexus 3 Process ID register
	struct NPIDR : NonPrivilegedSPR<NPIDR, 517>
	{
		typedef NonPrivilegedSPR<NPIDR, 517> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		NPIDR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		NPIDR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("NPIDR"); this->SetDescription("Nexus 3 Process ID register"); }
	};
	
	// UpperBlockAddressTranslationRegister
	struct UpperBlockAddressTranslationRegister : UnnumberedSLR<UpperBlockAddressTranslationRegister, SLR_SPR_SPACE, SLR_RW, SLR_PRIVILEGED>
	{
		typedef UnnumberedSLR<UpperBlockAddressTranslationRegister, SLR_SPR_SPACE, SLR_RW, SLR_PRIVILEGED> Super;
		
		struct BEPI : Field<BEPI, 0 , 14> {}; // Block Effective Page Index
		struct XBL  : Field<XBL , 15, 18> {}; // Extended Block Length
		struct BL   : Field<BL  , 19, 29> {}; // Block Length
		struct Vs   : Field<Vs  , 30    > {}; // Supervisor mode valid bit
		struct Vp   : Field<Vp  , 31    > {}; // User mode valid bit
		
		struct BEPI_0_3  : Field<BEPI_0_3 , 0 , 3 > {};
		struct BEPI_4_14 : Field<BEPI_4_14, 4 , 14> {};
		
		struct XBS_BL : Field<XBS_BL, 15, 29> {}; // Block Length when Extended Block Size is enabled
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC601  , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC603E , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC604E , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC740  , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC745  , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC750  , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC755  , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC7400 , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC7410 , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC7441 , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC7445 , _) { typedef FieldSet<BEPI, XBL, BL, Vs, Vp> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _) { typedef FieldSet<BEPI, XBL, BL, Vs, Vp> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _) { typedef FieldSet<BEPI, XBL, BL, Vs, Vp> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _) { typedef FieldSet<BEPI, XBL, BL, Vs, Vp> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC7451 , _) { typedef FieldSet<BEPI, BL, Vs, Vp> ALL;      };
		CASE_ENUM_TRAIT(MPC7455 , _) { typedef FieldSet<BEPI, XBL, BL, Vs, Vp> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _) { typedef FieldSet<BEPI, XBL, BL, Vs, Vp> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		UpperBlockAddressTranslationRegister(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		UpperBlockAddressTranslationRegister(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			BEPI::SetName("BEPI"); BEPI::SetDescription("Block Effective Page Index");
			XBL ::SetName("XBL");  XBL ::SetDescription("Extended Block Length");
			BL  ::SetName("BL");   BL  ::SetDescription("Block Length");
			Vs  ::SetName("Vs");   Vs  ::SetDescription("Supervisor mode valid bit");
			Vp  ::SetName("Vp");   Vp  ::SetDescription("User mode valid bit");
		}
	};
	
	// LowerBlockAddressTranslationRegister
	struct LowerBlockAddressTranslationRegister : UnnumberedSLR<LowerBlockAddressTranslationRegister, SLR_SPR_SPACE, SLR_RW, SLR_PRIVILEGED>
	{
		typedef UnnumberedSLR<LowerBlockAddressTranslationRegister, SLR_SPR_SPACE, SLR_RW, SLR_PRIVILEGED> Super;
		
		struct BRPN : Field<BRPN, 0 , 14> {}; // Block physical page number
		struct BXPN : Field<BXPN, 20, 22> {}; // Block extended physical page number
		struct WIMG : Field<WIMG, 25, 28> {}; // Memory/cache access mode bits
		struct W    : Field<W   , 25    > {}; // Write-Through
		struct I    : Field<I   , 26    > {}; // Caching-Inhibited
		struct M    : Field<M   , 27    > {}; // Memory coherence
		struct G    : Field<G   , 28    > {}; // Guarded
		struct BX   : Field<BX  , 29    > {}; // Block extended physical page number
		struct PP   : Field<PP  , 30, 31> {}; // Protection bits for block
		
		struct BRPN_0_3  : Field<BRPN_0_3 , 0 , 3 > {};
		struct BRPN_4_14 : Field<BRPN_4_14, 4 , 14> {};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC601  , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC603E , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC604E , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC740  , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC745  , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC750  , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC755  , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7400 , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7410 , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7441 , _) { typedef FieldSet<BRPN, BXPN, WIMG, BX, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7445 , _) { typedef FieldSet<BRPN, BXPN, WIMG, BX, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _) { typedef FieldSet<BRPN, BXPN, WIMG, BX, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7451 , _) { typedef FieldSet<BRPN, BXPN, WIMG, BX, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7455 , _) { typedef FieldSet<BRPN, BXPN, WIMG, BX, PP> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _) { typedef FieldSet<BRPN, WIMG, PP> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		LowerBlockAddressTranslationRegister(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		LowerBlockAddressTranslationRegister(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			BRPN::SetName("BRPN"); BRPN::SetDescription("Block physical page number");
			BXPN::SetName("BXPN"); BXPN::SetDescription("Block extended physical page number");
			WIMG::SetName("WIMG"); WIMG::SetDescription("Memory/cache access mode bits");
			W   ::SetName("W");    W   ::SetDescription("Write-Through");
			I   ::SetName("I");    I   ::SetDescription("Caching-Inhibited");
			M   ::SetName("M");    M   ::SetDescription("Memory coherence");
			G   ::SetName("G");    G   ::SetDescription("Guarded");
			BX  ::SetName("BX");   BX  ::SetDescription("Block extended physical page number");
			PP  ::SetName("PP");   PP  ::SetDescription("Protection bits for block");
		}
	};
	
	// IBATU
	template <unsigned int IBAT_NUM>
	struct IBATU : UpperBlockAddressTranslationRegister
	{
		typedef UpperBlockAddressTranslationRegister Super;
		
		static const unsigned int SLR_NUM = (IBAT_NUM < 4) ? (528 + (2 * IBAT_NUM)) : (560 + (2 * (IBAT_NUM - 4)));
		static const unsigned int REG_NUM = SLR_NUM;

		IBATU(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IBATU(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "IBAT" << IBAT_NUM << "U";
			
			std::stringstream desc_sstr;
			desc_sstr << "Instruction Block Address Translation " << IBAT_NUM << " (Upper)";
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};

	// IBATL
	template <unsigned int IBAT_NUM>
	struct IBATL : LowerBlockAddressTranslationRegister
	{
		typedef LowerBlockAddressTranslationRegister Super;
		
		static const unsigned int SLR_NUM = (IBAT_NUM < 4) ? (529 + (2 * IBAT_NUM)) : (561 + (2 * (IBAT_NUM - 4)));
		static const unsigned int REG_NUM = SLR_NUM;

		IBATL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IBATL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "IBAT" << IBAT_NUM << "L";
			
			std::stringstream desc_sstr;
			desc_sstr << "Instruction Block Address Translation " << IBAT_NUM << " (Lower)";
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};

	// DBATU
	template <unsigned int DBAT_NUM>
	struct DBATU : UpperBlockAddressTranslationRegister
	{
		typedef UpperBlockAddressTranslationRegister Super;
		
		static const unsigned int SLR_NUM = (DBAT_NUM < 4) ? (536 + (2 * DBAT_NUM)) : (568 + (2 * (DBAT_NUM - 4)));
		static const unsigned int REG_NUM = SLR_NUM;

		DBATU(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBATU(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "DBAT" << DBAT_NUM << "U";
			
			std::stringstream desc_sstr;
			desc_sstr << "Data Block Address Translation " << DBAT_NUM << " (Upper)";
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};

	// DBATL
	template <unsigned int DBAT_NUM>
	struct DBATL : LowerBlockAddressTranslationRegister
	{
		typedef LowerBlockAddressTranslationRegister Super;
		
		static const unsigned int SLR_NUM = (DBAT_NUM < 4) ? (537 + (2 * DBAT_NUM)) : (569 + (2 * (DBAT_NUM - 4)));
		static const unsigned int REG_NUM = SLR_NUM;

		DBATL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBATL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "DBAT" << DBAT_NUM << "L";
			
			std::stringstream desc_sstr;
			desc_sstr << "Data Block Address Translation " << DBAT_NUM << " (Lower)";
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};
	
	typedef IBATU<0> IBAT0U;
	typedef IBATL<0> IBAT0L;
	typedef IBATU<1> IBAT1U;
	typedef IBATL<1> IBAT1L;
	typedef IBATU<2> IBAT2U;
	typedef IBATL<2> IBAT2L;
	typedef IBATU<3> IBAT3U;
	typedef IBATL<3> IBAT3L;
	typedef IBATU<4> IBAT4U;
	typedef IBATL<4> IBAT4L;
	typedef IBATU<5> IBAT5U;
	typedef IBATL<5> IBAT5L;
	typedef IBATU<6> IBAT6U;
	typedef IBATL<6> IBAT6L;
	typedef IBATU<7> IBAT7U;
	typedef IBATL<7> IBAT7L;

	typedef DBATU<0> DBAT0U;
	typedef DBATL<0> DBAT0L;
	typedef DBATU<1> DBAT1U;
	typedef DBATL<1> DBAT1L;
	typedef DBATU<2> DBAT2U;
	typedef DBATL<2> DBAT2L;
	typedef DBATU<3> DBAT3U;
	typedef DBATL<3> DBAT3L;
	typedef DBATU<4> DBAT4U;
	typedef DBATL<4> DBAT4L;
	typedef DBATU<5> DBAT5U;
	typedef DBATL<5> DBAT5L;
	typedef DBATU<6> DBAT6U;
	typedef DBATL<6> DBAT6L;
	typedef DBATU<7> DBAT7U;
	typedef DBATL<7> DBAT7L;

	// Debug Control Register 3
	struct DBCR3 : PrivilegedSPR<DBCR3, 561>
	{
		typedef PrivilegedSPR<DBCR3, 561> Super;
		
		struct DEVT1C1 : Field<DEVT1C1, 0>  {}; // External Debug Event 1 Count 1 Enable
		struct DEVT2C1 : Field<DEVT2C1, 1>  {}; // External Debug Event 2 Count 1 Enable
		struct ICMPC1  : Field<ICMPC1 , 2>  {}; // Instruction Complete Debug Event Count 1 Enable
		struct IAC1C1  : Field<IAC1C1 , 3>  {}; // Instruction Address Compare 1 Count 1 Enable
		struct IAC2C1  : Field<IAC2C1 , 4>  {}; // Instruction Address Compare 2 Count 1 Enable
		struct IAC3C1  : Field<IAC3C1 , 5>  {}; // Instruction Address Compare 3 Count 1 Enable
		struct IAC4C1  : Field<IAC4C1 , 6>  {}; // Instruction Address Compare 4 Count 1 Enable
		struct DAC1RC1 : Field<DAC1RC1, 7>  {}; // Data Address Compare 1 Read Debug Event Count 1 Enable
		struct DAC1WC1 : Field<DAC1WC1, 8>  {}; // Data Address Compare 1 Write Debug Event Count 1 Enable
		struct DAC2RC1 : Field<DAC2RC1, 9>  {}; // Data Address Compare 2 Read Debug Event Count 1 Enable
		struct DAC2WC1 : Field<DAC2WC1, 10> {}; // Data Address Compare 2 Write Debug Event Count 1 Enable
		struct IRPTC1  : Field<IRPTC1 , 11> {}; // Interrupt Taken Debug Event Count 1 Enable
		struct RETC1   : Field<RETC1  , 12> {}; // Return Debug Event Count 1 Enable
		struct DEVT1C2 : Field<DEVT1C2, 13> {}; // External Debug Event 1 Count 2 Enable
		struct DEVT2C2 : Field<DEVT2C2, 14> {}; // External Debug Event 2 Count 2 Enable
		struct ICMPC2  : Field<ICMPC2 , 15> {}; // Instruction Complete Debug Event Count 2 Enable
		struct IAC1C2  : Field<IAC1C2 , 16> {}; // Instruction Address Compare 1 Debug Event Count 2 Enable
		struct IAC2C2  : Field<IAC2C2 , 17> {}; // Instruction Address Compare 2 Debug Event Count 2 Enable
		struct IAC3C2  : Field<IAC3C2 , 18> {}; // Instruction Address Compare 3 Debug Event Count 2 Enable
		struct IAC4C2  : Field<IAC4C2 , 19> {}; // Instruction Address Compare 4 Debug Event Count 2 Enable
		struct DAC1RC2 : Field<DAC1RC2, 20> {}; // Data Address Compare 1 Read Debug Event Count 2 Enable
		struct DAC1WC2 : Field<DAC1WC2, 21> {}; // Data Address Compare 1 Write Debug Event Count 2 Enable
		struct DAC2RC2 : Field<DAC2RC2, 22> {}; // Data Address Compare 2 Read Debug Event Count 2 Enable
		struct DAC2WC2 : Field<DAC2WC2, 23> {}; // Data Address Compare 2 Write Debug Event Count 2 Enable
		struct DEVT1T1 : Field<DEVT1T1, 24> {}; // External Debug Event 1 Trigger Counter 1 Enable
		struct DEVT2T1 : Field<DEVT2T1, 25> {}; // External Debug Event 2 Trigger Counter 1 Enable
		struct IAC1T1  : Field<IAC1T1 , 26> {}; // Instruction Address Compare 1 Trigger Counter 1 Enable
		struct IAC3T1  : Field<IAC3T1 , 27> {}; // Instruction Address Compare 3 Trigger Counter 1 Enable
		struct DAC1RT1 : Field<DAC1RT1, 28> {}; // Data Address Compare 1 Read Trigger Counter 1 Enable
		struct DAC1WT1 : Field<DAC1WT1, 29> {}; // Data Address Compare 1 Write Trigger Counter 1 Enable
		struct CNT2T1  : Field<CNT2T1 , 30> {}; // Debug Counter 2 Trigger Counter 1 Enable
		struct _CONFIG : Field<_CONFIG , 31> {}; // Debug Counter Configuration
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< DEVT1C1, DEVT2C1, ICMPC1, IAC1C1, IAC2C1, IAC3C1
		                                                  , IAC4C1, DAC1RC1, DAC1WC1, DAC2RC1, DAC2WC1, IRPTC1
		                                                  , RETC1, DEVT1C2, DEVT2C2, ICMPC2, IAC1C2, IAC2C2
		                                                  , IAC3C2, IAC4C2, DAC1RC2, DAC1WC2, DAC2RC2, DAC2WC2
		                                                  , DEVT1T1, DEVT2T1, IAC1T1, IAC3T1, DAC1RT1, DAC1WT1
		                                                  , CNT2T1, _CONFIG > ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< DEVT1C1, DEVT2C1, ICMPC1, IAC1C1, IAC2C1, IAC3C1
		                                                  , IAC4C1, DAC1RC1, DAC1WC1, DAC2RC1, DAC2WC1, IRPTC1
		                                                  , RETC1, DEVT1C2, DEVT2C2, ICMPC2, IAC1C2, IAC2C2
		                                                  , IAC3C2, IAC4C2, DAC1RC2, DAC1WC2, DAC2RC2, DAC2WC2
		                                                  , DEVT1T1, DEVT2T1, IAC1T1, IAC3T1, DAC1RT1, DAC1WT1
		                                                  , CNT2T1, _CONFIG > ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("DBCR3");            this->SetDescription("Debug Control Register 3");
			DEVT1C1::SetName("DEVT1C1"); DEVT1C1::SetDescription("External Debug Event 1 Count 1 Enable");
			DEVT2C1::SetName("DEVT2C1"); DEVT2C1::SetDescription("External Debug Event 2 Count 1 Enable");
			ICMPC1 ::SetName("ICMPC1");  ICMPC1 ::SetDescription("Instruction Complete Debug Event Count 1 Enable");
			IAC1C1 ::SetName("IAC1C1");  IAC1C1 ::SetDescription("Instruction Address Compare 1 Count 1 Enable");
			IAC2C1 ::SetName("IAC2C1");  IAC2C1 ::SetDescription("Instruction Address Compare 2 Count 1 Enable");
			IAC3C1 ::SetName("IAC3C1");  IAC3C1 ::SetDescription("Instruction Address Compare 3 Count 1 Enable");
			IAC4C1 ::SetName("IAC4C1");  IAC4C1 ::SetDescription("Instruction Address Compare 4 Count 1 Enable");
			DAC1RC1::SetName("DAC1RC1"); DAC1RC1::SetDescription("Data Address Compare 1 Read Debug Event Count 1 Enable");
			DAC1WC1::SetName("DAC1WC1"); DAC1WC1::SetDescription("Data Address Compare 1 Write Debug Event Count 1 Enable");
			DAC2RC1::SetName("DAC2RC1"); DAC2RC1::SetDescription("Data Address Compare 2 Read Debug Event Count 1 Enable");
			DAC2WC1::SetName("DAC2WC1"); DAC2WC1::SetDescription("Data Address Compare 2 Write Debug Event Count 1 Enable");
			IRPTC1 ::SetName("IRPTC1");  IRPTC1 ::SetDescription("Interrupt Taken Debug Event Count 1 Enable");
			RETC1  ::SetName("RETC1");   RETC1  ::SetDescription("Return Debug Event Count 1 Enable");
			DEVT1C2::SetName("DEVT1C2"); DEVT1C2::SetDescription("External Debug Event 1 Count 2 Enable");
			DEVT2C2::SetName("DEVT2C2"); DEVT2C2::SetDescription("External Debug Event 2 Count 2 Enable");
			ICMPC2 ::SetName("ICMPC2");  ICMPC2 ::SetDescription("Instruction Complete Debug Event Count 2 Enable");
			IAC1C2 ::SetName("IAC1C2");  IAC1C2 ::SetDescription("Instruction Address Compare 1 Debug Event Count 2 Enable");
			IAC2C2 ::SetName("IAC2C2");  IAC2C2 ::SetDescription("Instruction Address Compare 2 Debug Event Count 2 Enable");
			IAC3C2 ::SetName("IAC3C2");  IAC3C2 ::SetDescription("Instruction Address Compare 3 Debug Event Count 2 Enable");
			IAC4C2 ::SetName("IAC4C2");  IAC4C2 ::SetDescription("Instruction Address Compare 4 Debug Event Count 2 Enable");
			DAC1RC2::SetName("DAC1RC2"); DAC1RC2::SetDescription("Data Address Compare 1 Read Debug Event Count 2 Enable");
			DAC1WC2::SetName("DAC1WC2"); DAC1WC2::SetDescription("Data Address Compare 1 Write Debug Event Count 2 Enable");
			DAC2RC2::SetName("DAC2RC2"); DAC2RC2::SetDescription("Data Address Compare 2 Read Debug Event Count 2 Enable");
			DAC2WC2::SetName("DAC2WC2"); DAC2WC2::SetDescription("Data Address Compare 2 Write Debug Event Count 2 Enable");
			DEVT1T1::SetName("DEVT1T1"); DEVT1T1::SetDescription("External Debug Event 1 Trigger Counter 1 Enable");
			DEVT2T1::SetName("DEVT2T1"); DEVT2T1::SetDescription("External Debug Event 2 Trigger Counter 1 Enable");
			IAC1T1 ::SetName("IAC1T1");  IAC1T1 ::SetDescription("Instruction Address Compare 1 Trigger Counter 1 Enable");
			IAC3T1 ::SetName("IAC3T1");  IAC3T1 ::SetDescription("Instruction Address Compare 3 Trigger Counter 1 Enable");
			DAC1RT1::SetName("DAC1RT1"); DAC1RT1::SetDescription("Data Address Compare 1 Read Trigger Counter 1 Enable");
			DAC1WT1::SetName("DAC1WT1"); DAC1WT1::SetDescription("Data Address Compare 1 Write Trigger Counter 1 Enable");
			CNT2T1 ::SetName("CNT2T1");  CNT2T1 ::SetDescription("Debug Counter 2 Trigger Counter 1 Enable");
			_CONFIG ::SetName("CONFIG"); _CONFIG ::SetDescription("Debug Counter Configuration");
		}
	};
	
	// Debug Counter Register
	struct DBCNT : PrivilegedSPR<DBCNT, 562>
	{
		typedef PrivilegedSPR<DBCNT, 562> Super;
		
		struct CNT1 : Field<CNT1, 0, 15>  {};
		struct CNT2 : Field<CNT2, 16, 31> {};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CNT1, CNT2> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CNT1, CNT2> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCNT(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCNT(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("DBCNT"); this->SetDescription("Debug Counter Register");
			CNT1::SetName("CNT1"); CNT2::SetName("CNT2");
		}
	};

	// Debug Control Register 4
	struct DBCR4 : PrivilegedSPR<DBCR4, 563>
	{
		typedef PrivilegedSPR<DBCR4, 563> Super;
		
		struct DVC1C   : Field<DVC1C  , 1>      {}; // Data Value Compare 1 Control
		struct DVC2C   : Field<DVC2C  , 3>      {}; // Data Value Compare 2 Control
		struct DAC1XMH : Field<DAC1XMH, 13>     {}; // Data Address Compare 1 Extended Mask Control High
		struct DAC2XMH : Field<DAC2XMH, 15>     {}; // Data Address Compare 2 Extended Mask Control High 
		struct DAC1XM  : Field<DAC1XM , 16, 19> {}; // Data Address Compare 1 Extended Mask Control
		struct DAC2XM  : Field<DAC2XM , 20, 23> {}; // Data Address Compare 2 Extended Mask Control
		struct DAC1CFG : Field<DAC1CFG, 24, 27> {}; // Data Address Compare 1 Configuration 
		struct DAC2CFG : Field<DAC2CFG, 28, 31> {}; // Data Address Compare 2 Configuration 
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DVC1C, DVC2C, DAC1XMH, DAC2XMH, DAC1XM, DAC2XM, DAC1CFG, DAC2CFG> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DVC1C, DVC2C, DAC1XMH, DAC2XMH, DAC1XM, DAC2XM, DAC1CFG, DAC2CFG> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;

		DBCR4(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("DBCR4");            this->SetDescription("Debug Control Register 4");
			DVC1C  ::SetName("DVC1C");   DVC1C  ::SetDescription("Data Value Compare 1 Control");
			DVC2C  ::SetName("DVC2C");   DVC2C  ::SetDescription("Data Value Compare 2 Control");
			DAC1XMH::SetName("DAC1XMH"); DAC1XMH::SetDescription("Data Address Compare 1 Extended Mask Control High");
			DAC2XMH::SetName("DAC2XMH"); DAC2XMH::SetDescription("Data Address Compare 2 Extended Mask Control High");
			DAC1XM ::SetName("DAC1XM");  DAC1XM ::SetDescription("Data Address Compare 1 Extended Mask Control");
			DAC2XM ::SetName("DAC2XM");  DAC2XM ::SetDescription("Data Address Compare 2 Extended Mask Control");
			DAC1CFG::SetName("DAC1CFG"); DAC1CFG::SetDescription("Data Address Compare 1 Configuration ");
			DAC2CFG::SetName("DAC2CFG"); DAC2CFG::SetDescription("Data Address Compare 2 Configuration ");
		}
	};

	// Debug Control Register 5
	struct DBCR5 : PrivilegedSPR<DBCR5, 564>
	{
		typedef PrivilegedSPR<DBCR5, 564> Super;
		
		struct IAC5US : Field<IAC5US, 0, 1>   {}; // Instruction Address Compare 5 User/Supervisor Mode
		struct IAC5ER : Field<IAC5ER, 2, 3>   {}; // Instruction Address Compare 5 Effective/Real Mode
		struct IAC6US : Field<IAC6US, 4, 5>   {}; // Instruction Address Compare 6 User/Supervisor Mode
		struct IAC6ER : Field<IAC6ER, 6, 7>   {}; // Instruction Address Compare 6 Effective/Real Mode
		struct IAC56M : Field<IAC56M, 8, 9>   {}; // Instruction Address Compare 5/6 Mode
		struct IAC7US : Field<IAC7US, 16, 17> {}; // Instruction Address Compare 7 User/Supervisor Mode
		struct IAC7ER : Field<IAC7ER, 18, 19> {}; // Instruction Address Compare 7 Effective/Real Mode
		struct IAC8US : Field<IAC8US, 20, 21> {}; // Instruction Address Compare 8 User/Supervisor Mode
		struct IAC8ER : Field<IAC8ER, 22, 23> {}; // Instruction Address Compare 8 Effective/Real Mode
		struct IAC78M : Field<IAC78M, 24, 25> {}; // Instruction Address Compare 7/8 Mode
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IAC5US, IAC5ER, IAC6US, IAC6ER, IAC56M, IAC7US, IAC7ER, IAC8US, IAC8ER, IAC78M> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IAC5US, IAC5ER, IAC6US, IAC6ER, IAC56M, IAC7US, IAC7ER, IAC8US, IAC8ER, IAC78M> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR5(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR5(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("DBCR5");          this->SetDescription("Debug Control Register 5");
			IAC5US::SetName("IAC5US"); IAC5US::SetDescription("Instruction Address Compare 5 User/Supervisor Mode");
			IAC5ER::SetName("IAC5ER"); IAC5ER::SetDescription("Instruction Address Compare 5 Effective/Real Mode");
			IAC6US::SetName("IAC6US"); IAC6US::SetDescription("Instruction Address Compare 6 User/Supervisor Mode");
			IAC6ER::SetName("IAC6ER"); IAC6ER::SetDescription("Instruction Address Compare 6 Effective/Real Mode");
			IAC56M::SetName("IAC56M"); IAC56M::SetDescription("Instruction Address Compare 5/6 Mode");
			IAC7US::SetName("IAC7US"); IAC7US::SetDescription("Instruction Address Compare 7 User/Supervisor Mode");
			IAC7ER::SetName("IAC7ER"); IAC7ER::SetDescription("Instruction Address Compare 7 Effective/Real Mode");
			IAC8US::SetName("IAC8US"); IAC8US::SetDescription("Instruction Address Compare 8 User/Supervisor Mode");
			IAC8ER::SetName("IAC8ER"); IAC8ER::SetDescription("Instruction Address Compare 8 Effective/Real Mode");
			IAC78M::SetName("IAC78M"); IAC78M::SetDescription("Instruction Address Compare 7/8 Mode");
		}
	};

	// Instruction Address Compare 5
	struct IAC5 : PrivilegedSPR<IAC5, 565>
	{
		typedef PrivilegedSPR<IAC5, 565> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC5(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC5(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC5"); this->SetDescription("Instruction Address Compare 5"); }
	};

	// Instruction Address Compare 6
	struct IAC6 : PrivilegedSPR<IAC6, 566>
	{
		typedef PrivilegedSPR<IAC6, 566> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC6(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC6(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC6"); this->SetDescription("Instruction Address Compare 6"); }
	};

	// Instruction Address Compare 7
	struct IAC7 : PrivilegedSPR<IAC7, 567>
	{
		typedef PrivilegedSPR<IAC7, 567> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC7(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC7(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC7"); this->SetDescription("Instruction Address Compare 7"); }
	};

	// Instruction Address Compare 8
	struct IAC8 : PrivilegedSPR<IAC8, 568>
	{
		typedef PrivilegedSPR<IAC8, 568> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		IAC8(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IAC8(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init() { this->SetName("IAC8"); this->SetDescription("Instruction Address Compare 8"); }
	};

	// Machine Check Save/Restore Register 0
	struct MCSRR0 : PrivilegedSPR<MCSRR0, 570>
	{
		typedef PrivilegedSPR<MCSRR0, 570> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		MCSRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCSRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("MCSRR0"); this->SetDescription("Machine Check Save/Restore Register 0"); }
	};
	
	// Machine Check Save/Restore Register 1
	struct MCSRR1 : PrivilegedSPR<MCSRR1, 571>
	{
		typedef PrivilegedSPR<MCSRR1, 571> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		MCSRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCSRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("MCSRR1"); this->SetDescription("Machine Check Save/Restore Register 1"); }
	};
	
	// Machine Check Syndrome Register
	struct MCSR : PrivilegedSPR<MCSR, 572>
	{
		typedef PrivilegedSPR<MCSR, 572> Super;
		
		struct MCP         : Field<MCP        , 0>  {}; // Machine check input pin
		struct MCS         : Field<MCS        , 0>  {}; // Machine Check Summary
		struct IC_DPERR    : Field<IC_DPERR   , 1>  {}; // Instruction Cache data array parity error
		struct IB          : Field<IB         , 1>  {}; // Instruction PLB Error
		struct DRB         : Field<DRB        , 2>  {}; // Data Read PLB Error
		struct DC_DPERR    : Field<DC_DPERR   , 3>  {}; // Data Cache data array parity error
		struct DWB         : Field<DWB        , 3>  {}; // Data Write PLB Error
		struct EXCP_ERR    : Field<EXCP_ERR   , 4>  {}; // ISI or Bus Error on first instruction fetch for an exception handler
		struct TLBP        : Field<TLBP       , 4>  {}; // Translation Lookaside Buffer Parity Error
		struct IC_TPERR    : Field<IC_TPERR   , 5>  {}; // Instruction Cache Tag parity error
		struct ICP         : Field<ICP        , 5>  {}; // Instruction Cache Parity Error
		struct DC_TPERR    : Field<DC_TPERR   , 6>  {}; // Data Cache Tag parity error
		struct DCSP        : Field<DCSP       , 6>  {}; // Data Cache Search Parity Error
		struct IC_LKERR    : Field<IC_LKERR   , 7>  {}; // Instruction Cache Lock error
		struct DCFP        : Field<DCFP       , 7>  {}; // Data Cache Flush Parity Error
		struct DC_LKERR    : Field<DC_LKERR   , 8>  {}; // Data Cache Lock error
		struct IMPE        : Field<IMPE       , 8>  {}; // Imprecise Machine Check Exception
		struct NMI         : Field<NMI        , 11> {}; // NMI input pin
		struct MAV         : Field<MAV        , 12> {}; // MCAR Address Valid
		struct MEA         : Field<MEA        , 13> {}; // MCAR holds Effective Address
		struct U           : Field<U          , 14> {}; // User
		struct IF          : Field<IF         , 15> {}; // Instruction fecth Error Report
		struct LD          : Field<LD         , 16> {}; // Load type instruction Error Report
		struct ST          : Field<ST         , 17> {}; // Store type instruction Error Report
		struct G           : Field<G          , 18> {}; // Guarded instruction Error Report
		struct STACK_ERR   : Field<STACK_ERR  , 21> {}; // Stack Access Limit Check Error
		struct IMEM_PERR   : Field<IMEM_PERR  , 22> {}; // Instruction Mem (IMEM) Parity Error
		struct DMEM_RDPERR : Field<DMEM_RDPERR, 23> {}; // Data Mem (DMEM) Parity Error
		struct DMEM_WRPERR : Field<DMEM_WRPERR, 24> {}; // Data Mem (DMEM) Write Parity Error
		struct BUS_IRERR   : Field<BUS_IRERR  , 27> {}; // Read bus error on Instruction recovery linefill
		struct BUS_DRERR   : Field<BUS_DRERR  , 28> {}; // Read bus error on data load or linefill
		struct BUS_WRERR   : Field<BUS_WRERR  , 29> {}; // Write bus error on store to bus or DMEM/imprecise write error
		struct BUS_WRDSI   : Field<BUS_WRDSI  , 30> {}; // Write bus error on buffered store to bus with DSI signaled. Set concurrently with BUS_WRERR for this case
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(PPC440, _)      { typedef FieldSet< MCS, IB, DRB, DWB, TLBP, ICP, DCSP, DCFP, IMPE> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet< MCP, IC_DPERR, DC_DPERR, EXCP_ERR, IC_TPERR
		                                                  , DC_TPERR, IC_LKERR, DC_LKERR, NMI, MAV, MEA
		                                                  , U, IF, LD, ST, G, STACK_ERR, IMEM_PERR
		                                                  , DMEM_RDPERR, DMEM_WRPERR, BUS_IRERR, BUS_DRERR
		                                                  , BUS_WRERR, BUS_WRDSI > ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet< MCP, IC_DPERR, DC_DPERR, EXCP_ERR, IC_TPERR
		                                                  , DC_TPERR, IC_LKERR, DC_LKERR, NMI, MAV, MEA
		                                                  , U, IF, LD, ST, G, STACK_ERR, IMEM_PERR
		                                                  , DMEM_RDPERR, DMEM_WRPERR, BUS_IRERR, BUS_DRERR
		                                                  , BUS_WRERR, BUS_WRDSI > ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MCSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
		
		virtual bool MoveTo(uint32_t value) { this->Set(this->Get() & ~value); return true; } // W1C
	
	private:
		void Init()
		{
			       this->SetName("MCSR");               this->SetDescription("Machine Check Syndrome Register");
			MCP        ::SetName("MCP");         MCP        ::SetDescription("Machine check input pin");
			MCS        ::SetName("MCS");         MCS        ::SetDescription("Machine Check Summary");     
			IC_DPERR   ::SetName("IC_DPERR");    IC_DPERR   ::SetDescription("Instruction Cache data array parity error");
			IB         ::SetName("IB");          IB         ::SetDescription("Instruction PLB Error");    
			DRB        ::SetName("DRB");         DRB        ::SetDescription("Data Read PLB Error");     
			DC_DPERR   ::SetName("DC_DPERR");    DC_DPERR   ::SetDescription("Data Cache data array parity error");
			DWB        ::SetName("DWB");         DWB        ::SetDescription("Data Write PLB Error");     
			EXCP_ERR   ::SetName("EXCP_ERR");    EXCP_ERR   ::SetDescription("ISI or Bus Error on first instruction fetch for an exception handler");
			TLBP       ::SetName("TLBP");        TLBP       ::SetDescription("Translation Lookaside Buffer Parity Error");      
			IC_TPERR   ::SetName("IC_TPERR");    IC_TPERR   ::SetDescription("Instruction Cache Tag parity error");
			ICP        ::SetName("ICP");         ICP        ::SetDescription("Instruction Cache Parity Error");     
			DC_TPERR   ::SetName("DC_TPERR");    DC_TPERR   ::SetDescription("Data Cache Tag parity error");
			DCSP       ::SetName("DCSP");        DCSP       ::SetDescription("Data Cache Search Parity Error");      
			IC_LKERR   ::SetName("IC_LKERR");    IC_LKERR   ::SetDescription("Instruction Cache Lock error");
			DCFP       ::SetName("DCFP");        DCFP       ::SetDescription("Data Cache Flush Parity Error");      
			DC_LKERR   ::SetName("DC_LKERR");    DC_LKERR   ::SetDescription("Data Cache Lock error");
			IMPE       ::SetName("IMPE");        IMPE       ::SetDescription("Imprecise Machine Check Exception");      
			NMI        ::SetName("NMI");         NMI        ::SetDescription("NMI input pin");
			MAV        ::SetName("MAV");         MAV        ::SetDescription("MCAR Address Valid");
			MEA        ::SetName("MEA");         MEA        ::SetDescription("MCAR holds Effective Address");
			U          ::SetName("U");           U          ::SetDescription("User");
			IF         ::SetName("IF");          IF         ::SetDescription("Instruction fecth Error Report");
			LD         ::SetName("LD");          LD         ::SetDescription("Load type instruction Error Report");
			ST         ::SetName("ST");          ST         ::SetDescription("Store type instruction Error Report");
			G          ::SetName("G");           G          ::SetDescription("Guarded instruction Error Report");
			STACK_ERR  ::SetName("STACK_ERR");   STACK_ERR  ::SetDescription("Stack Access Limit Check Error");
			IMEM_PERR  ::SetName("IMEM_PERR");   IMEM_PERR  ::SetDescription("Instruction Mem (IMEM) Parity Error");
			DMEM_RDPERR::SetName("DMEM_RDPERR"); DMEM_RDPERR::SetDescription("Data Mem (DMEM) Parity Error");
			DMEM_WRPERR::SetName("DMEM_WRPERR"); DMEM_WRPERR::SetDescription("Data Mem (DMEM) Write Parity Error");
			BUS_IRERR  ::SetName("BUS_IRERR");   BUS_IRERR  ::SetDescription("Read bus error on Instruction recovery linefill");
			BUS_DRERR  ::SetName("BUS_DRERR");   BUS_DRERR  ::SetDescription("Read bus error on data load or linefill");
			BUS_WRERR  ::SetName("BUS_WRERR");   BUS_WRERR  ::SetDescription("Write bus error on store to bus or DMEM/imprecise write error");
			BUS_WRDSI  ::SetName("BUS_WRDSI");   BUS_WRDSI  ::SetDescription("Write bus error on buffered store to bus with DSI signaled. Set concurrently with BUS_WRERR for this case");
		}
	};

	// Machine Check Address Register
	struct MCAR : PrivilegedSPR<MCAR, 573>
	{
		typedef PrivilegedSPR<MCAR, 573> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		MCAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MCAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("MCAR"); this->SetDescription("Machine Check Address Register"); }
	};
	
	// Debug Save/Restore Register 0
	struct DSRR0 : PrivilegedSPR<DSRR0, 574>
	{
		typedef PrivilegedSPR<DSRR0, 574> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DSRR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DSRR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("DSRR0"); this->SetDescription("Debug Save/Restore Register 0"); }
	};
	
	// Debug Save/Restore Register 1
	struct DSRR1 : PrivilegedSPR<DSRR1, 575>
	{
		typedef PrivilegedSPR<DSRR1, 575> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DSRR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DSRR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("DSRR1"); this->SetDescription("Debug Save/Restore Register 1"); }
	};
	
	// Debug Data Acquisition Messaging Register
	struct DDAM : NonPrivilegedSPR<DDAM, 576>
	{
		typedef NonPrivilegedSPR<DDAM, 576> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DDAM(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DDAM(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DDAM"); this->SetDescription("Debug Data Acquisition Messaging Register"); }
	};

	// Data Address Compare 3
	struct DAC3 : PrivilegedSPR<DAC3, 592>
	{
		typedef PrivilegedSPR<DAC3, 592> Super;
		
		struct ALL : Field<ALL, 0, 31> {};

		DAC3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DAC3"); this->SetDescription("Data Address Compare 3"); }
	};

	// Data Address Compare 4
	struct DAC4 : PrivilegedSPR<DAC4, 593>
	{
		typedef PrivilegedSPR<DAC4, 593> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DAC4(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DAC4(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DAC4"); this->SetDescription("Data Address Compare 4"); }
	};

	// Debug Control Register 7
	struct DBCR7 : PrivilegedSPR<DBCR7, 596>
	{
		typedef PrivilegedSPR<DBCR7, 596> Super;
		
		struct DAC3XMH : Field<DAC3XMH, 13>     {}; // Data Address Compare 3 Extended Mask Control High
		struct DAC4XMH : Field<DAC4XMH, 15>     {}; // Data Address Compare 4 Extended Mask Control High
		struct DAC3XM  : Field<DAC3XM , 16, 19> {}; // Data Address Compare 3 Extended Mask Control
		struct DAC4XM  : Field<DAC4XM , 20, 23> {}; // Data Address Compare 4 Extended Mask Control
		struct DAC3CFG : Field<DAC3CFG, 24, 27> {}; // Data Address Compare 3 Configuration
		struct DAC4CFG : Field<DAC4CFG, 28, 31> {}; // Data Address Compare 4 Configuration
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DAC3XMH, DAC4XMH, DAC3XM, DAC4XM, DAC3CFG, DAC4CFG> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DAC3XMH, DAC4XMH, DAC3XM, DAC4XM, DAC3CFG, DAC4CFG> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR7(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR7(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("DBCR7");            this->SetDescription("Debug Control Register 7");
			DAC3XMH::SetName("DAC3XMH"); DAC3XMH::SetDescription("Data Address Compare 3 Extended Mask Control High");
			DAC4XMH::SetName("DAC4XMH"); DAC4XMH::SetDescription("Data Address Compare 4 Extended Mask Control High");
			DAC3XM ::SetName("DAC3XM");  DAC3XM ::SetDescription("Data Address Compare 3 Extended Mask Control");
			DAC4XM ::SetName("DAC4XM");  DAC4XM ::SetDescription("Data Address Compare 4 Extended Mask Control");
			DAC3CFG::SetName("DAC3CFG"); DAC3CFG::SetDescription("Data Address Compare 3 Configuration");
			DAC4CFG::SetName("DAC4CFG"); DAC4CFG::SetDescription("Data Address Compare 4 Configuration");
		}
	};

	// Debug Control Register 8
	struct DBCR8 : PrivilegedSPR<DBCR8, 597>
	{
		typedef PrivilegedSPR<DBCR8, 597> Super;
		
		struct DAC3US  : Field<DAC3US , 0, 1> {}; // Data Address Compare 3 User/Supervisor Mode
		struct DAC3ER  : Field<DAC3ER , 2, 3> {}; // Data Address Compare 3 Effective/Real Mode
		struct DAC4US  : Field<DAC4US , 4, 5> {}; // Data Address Compare 4 User/Supervisor Mode
		struct DAC4ER  : Field<DAC4ER , 6, 7> {}; // Data Address Compare 4 Effective/Real Mode
		struct DAC34M  : Field<DAC34M , 8, 9> {}; // Data Address Compare 3/4 Mode
		struct DAC3LNK : Field<DAC3LNK, 10>   {}; // Data Address Compare 3 Linked
		struct DAC4LNK : Field<DAC4LNK, 11>   {}; // Data Address Compare 4 Linked
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DAC3US, DAC3ER, DAC4US, DAC4ER, DAC34M, DAC3LNK, DAC4LNK> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DAC3US, DAC3ER, DAC4US, DAC4ER, DAC34M, DAC3LNK, DAC4LNK> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR8(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR8(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("DBCR8");            this->SetDescription("Debug Control Register 8");
			DAC3US ::SetName("DAC3US");  DAC3US ::SetDescription("Data Address Compare 3 User/Supervisor Mode");
			DAC3ER ::SetName("DAC3ER");  DAC3ER ::SetDescription("Data Address Compare 3 Effective/Real Mode");
			DAC4US ::SetName("DAC4US");  DAC4US ::SetDescription("Data Address Compare 4 User/Supervisor Mode");
			DAC4ER ::SetName("DAC4ER");  DAC4ER ::SetDescription("Data Address Compare 4 Effective/Real Mode");
			DAC34M ::SetName("DAC34M");  DAC34M ::SetDescription("Data Address Compare 3/4 Mode");
			DAC3LNK::SetName("DAC3LNK"); DAC3LNK::SetDescription("Data Address Compare 3 Linked");
			DAC4LNK::SetName("DAC4LNK"); DAC4LNK::SetDescription("Data Address Compare 4 Linked");
		}
	};

	// Debug Data Effective Address Register
	struct DDEAR : PrivilegedSPR<DDEAR, 600>
	{
		typedef PrivilegedSPR<DDEAR, 600> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DDEAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DDEAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init() { this->SetName("DDEAR"); this->SetDescription("Debug Data Effective Address Register"); }
	};
	
	// Data Value Compare 1 Upper
	struct DVC1U : PrivilegedSPR<DVC1U, 601>
	{
		typedef PrivilegedSPR<DVC1U, 601> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC1U(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC1U(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DVC1U"); this->SetDescription("Data Value Compare 1 Upper"); }
	};

	// Data Value Compare 2 Upper
	struct DVC2U : PrivilegedSPR<DVC2U, 602>
	{
		typedef PrivilegedSPR<DVC2U, 602> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DVC2U(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVC2U(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DVC2U"); this->SetDescription("Data Value Compare 2 Upper"); }
	};

	// Debug Control Register 6
	struct DBCR6 : PrivilegedSPR<DBCR6, 603>
	{
		typedef PrivilegedSPR<DBCR6, 603> Super;
		
		struct IAC1XM : Field<IAC1XM, 0, 3>   {}; // Instruction Address Compare 1 Extended Mask Control
		struct IAC2XM : Field<IAC2XM, 4, 7>   {}; // Instruction Address Compare 2 Extended Mask Control
		struct IAC3XM : Field<IAC3XM, 8, 11>  {}; // Instruction Address Compare 3 Extended Mask Control
		struct IAC4XM : Field<IAC4XM, 12, 15> {}; // Instruction Address Compare 4 Extended Mask Control
		struct IAC5XM : Field<IAC5XM, 16, 19> {}; // Instruction Address Compare 5 Extended Mask Control
		struct IAC6XM : Field<IAC6XM, 20, 23> {}; // Instruction Address Compare 6 Extended Mask Control
		struct IAC7XM : Field<IAC7XM, 24, 27> {}; // Instruction Address Compare 7 Extended Mask Control
		struct IAC8XM : Field<IAC8XM, 28, 31> {}; // Instruction Address Compare 8 Extended Mask Control
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IAC1XM, IAC2XM, IAC3XM, IAC4XM, IAC5XM, IAC6XM, IAC7XM, IAC8XM> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IAC1XM, IAC2XM, IAC3XM, IAC4XM, IAC5XM, IAC6XM, IAC7XM, IAC8XM> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DBCR6(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBCR6(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("DBCR6");          this->SetDescription("Debug Control Register 6");
			IAC1XM::SetName("IAC1XM"); IAC1XM::SetDescription("Instruction Address Compare 1 Extended Mask Control");
			IAC2XM::SetName("IAC2XM"); IAC2XM::SetDescription("Instruction Address Compare 2 Extended Mask Control");
			IAC3XM::SetName("IAC3XM"); IAC3XM::SetDescription("Instruction Address Compare 3 Extended Mask Control");
			IAC4XM::SetName("IAC4XM"); IAC4XM::SetDescription("Instruction Address Compare 4 Extended Mask Control");
			IAC5XM::SetName("IAC5XM"); IAC5XM::SetDescription("Instruction Address Compare 5 Extended Mask Control");
			IAC6XM::SetName("IAC6XM"); IAC6XM::SetDescription("Instruction Address Compare 6 Extended Mask Control");
			IAC7XM::SetName("IAC7XM"); IAC7XM::SetDescription("Instruction Address Compare 7 Extended Mask Control");
			IAC8XM::SetName("IAC8XM"); IAC8XM::SetDescription("Instruction Address Compare 8 Extended Mask Control");
		}
	};

	// L1 Cache Control and Status Register 2
	struct L1CSR2 : PrivilegedSPR<L1CSR2, 606>
	{
		typedef PrivilegedSPR<L1CSR2, 606> Super;
		
		struct STGC : Field<STGC, 2, 3> {}; // Store Gather Control
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<STGC> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<STGC> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CSR2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CSR2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			      this->SetName("L1CSR2");       this->SetDescription("L1 Cache Control and Status Register 2");
			STGC::SetName("STGC");   STGC::SetDescription("Store Gather Control");
		}
	};
	
	// MPU Assist Register 0
	struct MAS0 : PrivilegedSPR<MAS0, 624>
	{
		typedef PrivilegedSPR<MAS0, 624> Super;
		
		struct VALID  : Field<VALID, 0>                                           {}; // MPU Entry Valid
		struct IPROT  : Field<IPROT, 1>                                           {}; // Invalidation Protect
		struct SEL    : Field<SEL  , 2, 3>                                        {}; // Selects MPU for access
		struct RO     : Field<RO   , 5>                                           {}; // Read-Only
		struct DEBUG  : Field<DEBUG, 6>                                           {}; // Debug Control for Entry
		struct INST   : Field<INST , 7>                                           {}; // Instruction Entry
		struct SHD    : Field<SHD  , 8>                                           {}; // Shared Entry Select
		struct ESEL   : Field<ESEL , 12, 15>                                      {}; // Entry select for MPU
		struct UAMSK  : Field<UAMSK, 17, 19>                                      {}; // Upper Address Mask Control
		struct UW     : Field<UW   , 20>                                          {}; // User Mode Write Permission
		struct SW     : Field<SW   , 21>                                          {}; // Supervisor Mode Write / Read Permission
		struct UX_UR  : Field<UX_UR, 22>                                          {}; // User Mode Execute / Read Permission
		struct SX_SR  : Field<SX_SR, 23>                                          {}; // Supervisor Mode Execute / Read Permission
		struct IOVR   : Field<IOVR , 24>                                          {}; // Cache-Inihibit attribute Override
		struct GOVR   : Field<GOVR , 25>                                          {}; // G attribute Override
		struct VLE    : Field<VLE  , 26>                                          {}; // VLE (0=standard book E, 1=Power ISA VLE)
		struct VLE_RO : Field<VLE  , 26, 26, unisim::util::reg::core::SW_R_HW_RO> {};
		struct W      : Field<W    , 27>                                          {}; // Write-through
		struct W_RO   : Field<W    , 27, 27, unisim::util::reg::core::SW_R_HW_RO> {};
		struct I      : Field<I    , 28>                                          {}; // Cache Inhibited
		struct M      : Field<M    , 29>                                          {}; // Memory coherency enforced
		struct M_RO   : Field<M    , 29, 29, unisim::util::reg::core::SW_R_HW_RO> {};
		struct G      : Field<G    , 30>                                          {}; // Guarded
		struct E      : Field<E    , 31>                                          {}; // Endian (0=big-endian, 1=little-endian)
		struct E_RO   : Field<E    , 31, 31, unisim::util::reg::core::SW_R_HW_RO> {};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<VALID, IPROT, SEL, RO, DEBUG, INST, SHD, ESEL, UAMSK, UW, SW, UX_UR, SX_SR, IOVR, GOVR, VLE_RO, W_RO, I, M_RO, G, E_RO> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<VALID, IPROT, SEL, RO, DEBUG, INST, SHD, ESEL, UAMSK, UW, SW, UX_UR, SX_SR, IOVR, GOVR, VLE_RO, W_RO, I, M_RO, G, E_RO> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			       this->SetName("MAS0");         this->SetDescription("MPU Assist Register 0");
			VALID ::SetName("VALID"); VALID ::SetDescription("MPU Entry Valid");
			IPROT ::SetName("IPROT"); IPROT ::SetDescription("Invalidation Protect");
			SEL   ::SetName("SEL");   SEL   ::SetDescription("Selects MPU for access");
			RO    ::SetName("RO");    RO    ::SetDescription("Read-Only");
			DEBUG ::SetName("DEBUG"); DEBUG ::SetDescription("Debug Control for Entry");
			INST  ::SetName("INST");  INST  ::SetDescription("Instruction Entry");
			SHD   ::SetName("SHD");   SHD   ::SetDescription("Shared Entry Select");
			ESEL  ::SetName("ESEL");  ESEL  ::SetDescription("Entry select for MPU");
			UAMSK ::SetName("UAMSK"); UAMSK ::SetDescription("Upper Address Mask Control");
			UW    ::SetName("UW");    UW    ::SetDescription("User Mode Write Permission");
			SW    ::SetName("SW");    SW    ::SetDescription("Supervisor Mode Write / Read Permission");
			UX_UR ::SetName("UX_UR"); UX_UR ::SetDescription("User Mode Execute / Read Permission");
			SX_SR ::SetName("SX_SR"); SX_SR ::SetDescription("Supervisor Mode Execute / Read Permission");
			IOVR  ::SetName("IOVR");  IOVR  ::SetDescription("Cache-Inihibit attribute Override");
			GOVR  ::SetName("GOVR");  GOVR  ::SetDescription("G attribute Override");
			VLE   ::SetName("VLE");   VLE   ::SetDescription("PowerISA VLE");
			VLE_RO::SetName("VLE");   VLE_RO::SetDescription("PowerISA VLE");
			W     ::SetName("W");     W     ::SetDescription("Write-through Required");
			W_RO  ::SetName("W");     W_RO  ::SetDescription("Write-through Required");
			I     ::SetName("I");     I     ::SetDescription("Cache Inhibited");
			M     ::SetName("M");     M     ::SetDescription("Memory Coherence Required");
			M_RO  ::SetName("M");     M_RO  ::SetDescription("Memory Coherence Required");
			G     ::SetName("G");     G     ::SetDescription("Guarded");
			E     ::SetName("E");     E     ::SetDescription("Endianness");
			E_RO  ::SetName("E");     E_RO  ::SetDescription("Endianness");
		}
	};
	
	// MPU Assist Register 1
	struct MAS1 : PrivilegedSPR<MAS1, 625>
	{
		typedef PrivilegedSPR<MAS1, 625> Super;
		
		struct TID    : Field<TID   , 8, 15>  {}; // Region ID bits
		struct TIDMSK : Field<TIDMSK, 24, 31> {}; // Region ID mask
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<TID, TIDMSK> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<TID, TIDMSK> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			        this->SetName("MAS1");           this->SetDescription("MPU Assist Register 1");
			TID   ::SetName("TID");    TID   ::SetDescription("Region ID bits");
			TIDMSK::SetName("TIDMSK"); TIDMSK::SetDescription("Region ID mask");
		}
	};
	
	// MPU Assist Register 2
	struct MAS2 : PrivilegedSPR<MAS2, 626>
	{
		typedef PrivilegedSPR<MAS2, 626> Super;
		
		struct UPPER_BOUND : Field<UPPER_BOUND, 0, 31> {}; // Upper bound of address range covered by entry
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<UPPER_BOUND> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<UPPER_BOUND> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			             this->SetName("MAS2");                     this->SetDescription("MPU Assist Register 2");
			UPPER_BOUND::SetName("UPPER_BOUND"); UPPER_BOUND::SetDescription("Upper bound of address range covered by entry");
		}
	};
	
	// MPU Assist Register 3
	struct MAS3 : PrivilegedSPR<MAS3, 627>
	{
		typedef PrivilegedSPR<MAS3, 627> Super;
		
		struct LOWER_BOUND : Field<LOWER_BOUND, 0, 31> {}; // Lower bound of address range covered by entry
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<LOWER_BOUND> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<LOWER_BOUND> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MAS3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MAS3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			             this->SetName("MAS3");                     this->SetDescription("MPU Assist Register 3");
			LOWER_BOUND::SetName("LOWER_BOUND"); LOWER_BOUND::SetDescription("Lower bound of address range covered by entry");
		}
	};
	
	// MAS4 ????

	// External Debug Resource Allocation Control Register 0
	struct EDBRAC0 : ReadOnlyPrivilegedSPR<EDBRAC0, 638>
	{
		typedef ReadOnlyPrivilegedSPR<EDBRAC0, 638> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		EDBRAC0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		EDBRAC0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("EDBRAC0"); this->SetDescription("External Debug Resource Allocation Control Register 0"); }
	};
	
	// MPU0 Configuration Register
	struct MPU0CFG : ReadOnlyPrivilegedSPR<MPU0CFG, 692>
	{
		typedef ReadOnlyPrivilegedSPR<MPU0CFG, 692> Super;
		
		struct FASSOC  : Field<FASSOC , 0>      {}; // Fully Associative
		struct MINSIZE : Field<MINSIZE, 8, 11>  {}; // Minimum Region Size
		struct MAXSIZE : Field<MAXSIZE, 12, 15> {}; // Maximum Region Size
		struct IPROT   : Field<IPROT  , 16>     {}; // Invalidate Protect Capability
		struct UAMSKA  : Field<UAMSKA , 18>     {}; // Upper Address Masking Availability
		struct SHENTRY : Field<SHENTRY, 23, 25> {}; // Number of Shared (configurable for I or D) Entries
		struct DENTRY  : Field<DENTRY , 26, 28> {}; // Number of Data Entries
		struct IENTRY  : Field<IENTRY , 29, 31> {}; // Number of Instruction Entries
		
		template <Model _MODEL, bool dummy = true> struct IMPLEMENTATION {};
		template <bool dummy> struct IMPLEMENTATION<E200Z710N3, dummy>
		{
			typedef FieldSet<FASSOC, MINSIZE, MAXSIZE, IPROT, UAMSKA, SHENTRY, DENTRY, IENTRY> ALL;
		};
		template <bool dummy> struct IMPLEMENTATION<E200Z425BN3, dummy>
		{
			typedef FieldSet<FASSOC, MINSIZE, MAXSIZE, IPROT, UAMSKA, SHENTRY, DENTRY, IENTRY> ALL;
		};

		typedef typename IMPLEMENTATION<CONFIG::MODEL>::ALL ALL;
		
		MPU0CFG(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MPU0CFG(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("MPU0CFG");          this->SetDescription("MPU0 Configuration Register");
			FASSOC ::SetName("FASSOC");  FASSOC ::SetDescription("Fully Associative");
			MINSIZE::SetName("MINSIZE"); MINSIZE::SetDescription("Minimum Region Size");
			MAXSIZE::SetName("MAXSIZE"); MAXSIZE::SetDescription("Maximum Region Size");
			IPROT  ::SetName("IPROT");   IPROT  ::SetDescription("Invalidate Protect Capability");
			UAMSKA ::SetName("UAMSKA");  UAMSKA ::SetDescription("Upper Address Masking Availability");
			SHENTRY::SetName("SHENTRY"); SHENTRY::SetDescription("Number of Shared (configurable for I or D) Entries");
			DENTRY ::SetName("DENTRY");  DENTRY ::SetDescription("Number of Data Entries");
			IENTRY ::SetName("IENTRY");  IENTRY ::SetDescription("Number of Instruction Entries");
		}
	};

	// DMEM Configuration Register 0
	struct DMEMCFG0 : ReadOnlyPrivilegedSPR<DMEMCFG0, 694>
	{
		typedef ReadOnlyPrivilegedSPR<DMEMCFG0, 694> Super;
		
		struct DMEM_BASE_ADDR : Field<DMEM_BASE_ADDR, 0, 19>  {}; // DMEM BASE ADDRESS (CPU Port)
		struct DECUA          : Field<DECUA         , 21>     {}; // DMEM Error Correction Update Available
		struct DECA           : Field<DECA          , 22>     {}; // DMEM Error Correction Available
		struct DMSIZE         : Field<DMSIZE        , 24, 29> {}; // DMEM Size
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DMEM_BASE_ADDR, DECUA, DECA, DMSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DMEM_BASE_ADDR, DECUA, DECA, DMSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DMEMCFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DMEMCFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("DMEMCFG0");                       this->SetDescription("DMEM Configuration Register 0");
			DMEM_BASE_ADDR::SetName("DMEM_BASE_ADDR"); DMEM_BASE_ADDR::SetDescription("DMEM BASE ADDRESS (CPU Port)");
			DECUA         ::SetName("DECUA");          DECUA         ::SetDescription("DMEM Error Correction Update Available");
			DECA          ::SetName("DECA");           DECA          ::SetDescription("DMEM Error Correction Available");
			DMSIZE        ::SetName("DMSIZE");         DMSIZE        ::SetDescription("DMEM Size");
		}
	};

	// IMEM Configuration Register 0
	struct IMEMCFG0 : PrivilegedSPR<IMEMCFG0, 695>
	{
		typedef PrivilegedSPR<IMEMCFG0, 695> Super;
		
		struct IMEM_BASE_ADDR : Field<IMEM_BASE_ADDR, 0, 19>  {}; // IMEM BASE ADDRESS (CPU Port)
		struct IECUA          : Field<IECUA         , 21>     {}; // IMEM Error Correction Update Available
		struct IECA           : Field<IECA          , 22>     {}; // IMEM Error Correction Available
		struct IMSIZE         : Field<IMSIZE        , 24, 29> {}; // IMEM Size
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IMEM_BASE_ADDR, IECUA, IECA, IMSIZE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IMEM_BASE_ADDR, IECUA, IECA, IMSIZE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IMEMCFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IMEMCFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("IMEMCFG0");                       this->SetDescription("IMEM Configuration Register 0");
			IMEM_BASE_ADDR::SetName("IMEM_BASE_ADDR"); IMEM_BASE_ADDR::SetDescription("IMEM BASE ADDRESS (CPU Port)");
			IECUA         ::SetName("IECUA");          IECUA         ::SetDescription("IMEM Error Correction Update Available");
			IECA          ::SetName("IECA");           IECA          ::SetDescription("IMEM Error Correction Available");
			IMSIZE        ::SetName("IMSIZE");         IMSIZE        ::SetDescription("IMEM Size");
		}
	};

	struct CacheVictimRegister : UnnumberedSLR<CacheVictimRegister, SLR_SPR_SPACE, SLR_RW, SLR_PRIVILEGED>
	{
		typedef UnnumberedSLR<CacheVictimRegister, SLR_SPR_SPACE, SLR_RW, SLR_PRIVILEGED> Super;
		struct VNDXA : Field<VNDXA, 0 , 7 > {}; // Victim Index A
		struct VNDXB : Field<VNDXB, 8 , 15> {}; // Victim Index B
		struct VNDXC : Field<VNDXC, 16, 23> {}; // Victim Index C
		struct VNDXD : Field<VNDXD, 24, 31> {}; // Victim Index D
		
		typedef FieldSet<VNDXA, VNDXB, VNDXC, VNDXD> ALL;
		
		CacheVictimRegister(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CacheVictimRegister(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual void Reset() { /* unaffected */ }
		
		unsigned int GetVictimIndex(unsigned int i) const { return (this->Get() >> (8 * (3 - i))) & 0xff; }
		void SetVictimIndex(unsigned int i, unsigned int vndx) { unsigned int bofs = 8 * (3 - i); this->Set((this->Get() & ~(0xff << bofs)) | ((vndx & 0xff) << bofs)); }
		void ClearVictimIndex(unsigned i) { unsigned int bofs = 8 * (3 - i); this->Set(this->Get() & ~(0xff << bofs)); }
		
		using Super::operator =;
	private:
		void Init()
		{
			VNDXA::SetName("VNDXA"); VNDXA::SetDescription("Victim Index A");
			VNDXB::SetName("VNDXB"); VNDXB::SetDescription("Victim Index B");
			VNDXC::SetName("VNDXC"); VNDXC::SetDescription("Victim Index C");
			VNDXD::SetName("VNDXD"); VNDXD::SetDescription("Victim Index D");
		}
	};

	// Instruction Cache Normal Victim Register
	template <unsigned int INV_NUM>
	struct INV : CacheVictimRegister
	{
		typedef CacheVictimRegister Super;
		
		static const unsigned int SLR_NUM = 880 + INV_NUM;
		static const unsigned int REG_NUM = SLR_NUM;

		INV(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		INV(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);

			std::stringstream name_sstr;
			name_sstr << "INV" << INV_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Instruction Cache Normal Victim Register " << INV_NUM;
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};
	
	typedef INV<0> INV0;
	typedef INV<1> INV1;
	typedef INV<2> INV2;
	typedef INV<3> INV3;

	// Instruction Cache Transient Victim Register
	template <unsigned int ITV_NUM>
	struct ITV : CacheVictimRegister
	{
		typedef CacheVictimRegister Super;
		
		static const unsigned int SLR_NUM = 884 + ITV_NUM;
		static const unsigned int REG_NUM = SLR_NUM;

		ITV(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ITV(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "ITV" << ITV_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Instruction Cache Transient Victim Register " << ITV_NUM;
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};
	
	typedef ITV<0> ITV0;
	typedef ITV<1> ITV1;
	typedef ITV<2> ITV2;
	typedef ITV<3> ITV3;
	
	// Core Configuration Register 1
	struct CCR1 : PrivilegedSPR<CCR1, 888>
	{
		typedef PrivilegedSPR<CCR1, 888> Super;
		
		struct ICDPEI : Field<ICDPEI, 0 , 7 > {}; // Instruction Cache Data Parity Error Insert
		struct ICTPEI : Field<ICTPEI, 8 , 9 > {}; // Instruction Cache Tag Parity Error Insert
		struct DCTPEI : Field<DCTPEI, 10, 11> {}; // Data Cache Tag Parity Error Insert
		struct DCDPEI : Field<DCDPEI, 12    > {}; // Data Cache Data Parity Error Insert
		struct DCUPEI : Field<DCUPEI, 13    > {}; // Data Cache U-bit Parity Error Insert
		struct DCMPEI : Field<DCMPEI, 14    > {}; // Data Cache Modified-bit Parity Error Insert
		struct FCOM   : Field<FCOM  , 15    > {}; // Force Cache Operation Miss
		struct MMUPEI : Field<MMUPEI, 16, 19> {}; // Memory Management Unit Parity Error Insert
		struct FFF    : Field<FFF   , 20    > {}; // Force Full-line Flush
		struct TCS    : Field<TCS   , 24    > {}; // Timer Clock Select
		
		typedef FieldSet<ICDPEI, ICTPEI, DCTPEI, DCDPEI, DCUPEI, DCMPEI, FCOM, MMUPEI, FFF, TCS> ALL;
		
		CCR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CCR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("CCR1"); this->SetDescription("Core Configuration Register 1");
			
			ICDPEI::SetName("ICDPEI"); ICDPEI::SetDescription("Instruction Cache Data Parity Error Insert");
			ICTPEI::SetName("ICTPEI"); ICTPEI::SetDescription("Instruction Cache Tag Parity Error Insert");
			DCTPEI::SetName("DCTPEI"); DCTPEI::SetDescription("Data Cache Tag Parity Error Insert");
			DCDPEI::SetName("DCDPEI"); DCDPEI::SetDescription("Data Cache Data Parity Error Insert");
			DCUPEI::SetName("DCUPEI"); DCUPEI::SetDescription("Data Cache U-bit Parity Error Insert");
			DCMPEI::SetName("DCMPEI"); DCMPEI::SetDescription("Data Cache Modified-bit Parity Error Insert");
			FCOM  ::SetName("FCOM");   FCOM  ::SetDescription("Force Cache Operation Miss");
			MMUPEI::SetName("MMUPEI"); MMUPEI::SetDescription("Memory Management Unit Parity Error Insert");
			FFF   ::SetName("FFF");    FFF   ::SetDescription("Force Full-line Flush");
			TCS   ::SetName("TCS");    TCS   ::SetDescription("Timer Clock Select");
		}
	};

	// Data Cache Normal Victim Register
	template <unsigned int DNV_NUM>
	struct DNV : CacheVictimRegister
	{
		typedef CacheVictimRegister Super;
		
		static const unsigned int SLR_NUM = 912 + DNV_NUM;
		static const unsigned int REG_NUM = SLR_NUM;

		DNV(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DNV(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "DNV" << DNV_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Data Cache Normal Victim Register " << DNV_NUM;
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};
	
	typedef DNV<0> DNV0;
	typedef DNV<1> DNV1;
	typedef DNV<2> DNV2;
	typedef DNV<3> DNV3;

	// Data Cache Transient Victim Register
	template <unsigned int DTV_NUM>
	struct DTV : CacheVictimRegister
	{
		typedef CacheVictimRegister Super;
		
		static const unsigned int SLR_NUM = 916 + DTV_NUM;
		static const unsigned int REG_NUM = SLR_NUM;
		
		DTV(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DTV(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		
		virtual unsigned int GetRegNum() const { return SLR_NUM; }
		virtual void Reset() { /* unaffected */ }
		
		using Super::operator =;
	private:
		void Init()
		{
			this->cpu->RegisterSLR(SLR_NUM, this);
			
			std::stringstream name_sstr;
			name_sstr << "DTV" << DTV_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Data Cache Transient Victim Register " << DTV_NUM;
			
			this->SetName(name_sstr.str());
			this->SetDescription(desc_sstr.str());
		}
	};
	
	typedef DTV<0> DTV0;
	typedef DTV<1> DTV1;
	typedef DTV<2> DTV2;
	typedef DTV<3> DTV3;
	
	// Data Cache Victim Limit
	struct DVLIM : PrivilegedSPR<DVLIM, 920>
	{
		typedef PrivilegedSPR<DVLIM, 920> Super;
		
		struct TFLOOR   : Field<TFLOOR  , 2 , 9 > {}; // Transient Floor
		struct TCEILING : Field<TCEILING, 13, 20> {}; // Transient Ceiling
		struct NFLOOR   : Field<NFLOOR  , 24, 31> {}; // Normal Floor
		
		typedef FieldSet<TFLOOR, TCEILING, NFLOOR> ALL;

		DVLIM(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DVLIM(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("DVLIM");  this->SetDescription("Data Cache Victim Limit");
			
			TFLOOR  ::SetName("TFLOOR");   TFLOOR  ::SetDescription("Transient Floor");
			TCEILING::SetName("TCEILING"); TCEILING::SetDescription("Transient Ceiling");
			NFLOOR  ::SetName("NFLOOR");   NFLOOR  ::SetDescription("Normal Floor");
		}
	};

	// Instruction Cache Victim Limit
	struct IVLIM : PrivilegedSPR<IVLIM, 921>
	{
		typedef PrivilegedSPR<IVLIM, 921> Super;
		
		struct TFLOOR   : Field<TFLOOR  , 2 , 9 > {}; // Transient Floor
		struct TCEILING : Field<TCEILING, 13, 20> {}; // Transient Ceiling
		struct NFLOOR   : Field<NFLOOR  , 24, 31> {}; // Normal Floor
		
		typedef FieldSet<TFLOOR, TCEILING, NFLOOR> ALL;

		IVLIM(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IVLIM(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("IVLIM");  this->SetDescription("Instruction Cache Victim Limit");
			
			TFLOOR  ::SetName("TFLOOR");   TFLOOR  ::SetDescription("Transient Floor");
			TCEILING::SetName("TCEILING"); TCEILING::SetDescription("Transient Ceiling");
			NFLOOR  ::SetName("NFLOOR");   NFLOOR  ::SetDescription("Normal Floor");
		}
	};
	
	// Reset Configuration
	struct RSTCFG : ReadOnlyPrivilegedSPR<RSTCFG, 923>
	{
		typedef ReadOnlyPrivilegedSPR<RSTCFG, 923> Super;
		
		struct U0   : Field<U0  , 16    > {}; // U0 Storage Attribute
		struct U1   : Field<U1  , 17    > {}; // U1 Storage Attribute
		struct U2   : Field<U2  , 18    > {}; // U2 Storage Attribute
		struct U3   : Field<U3  , 19    > {}; // U3 Storage Attribute
		struct E    : Field<E   , 24    > {}; // E Storage Attribute
		struct ERPN : Field<ERPN, 28, 31> {}; // Extended Real Page Number
		
		typedef FieldSet<U0, U1, U2, U3, E, ERPN> ALL;
		
		RSTCFG(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		RSTCFG(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("RSTCFG");  this->SetDescription("Reset Configuration");
			
			U0  ::SetName("U0");   U0  ::SetDescription("U0 Storage Attribute");
			U1  ::SetName("U1");   U1  ::SetDescription("U1 Storage Attribute");
			U2  ::SetName("U2");   U2  ::SetDescription("U2 Storage Attribute");
			U3  ::SetName("U3");   U3  ::SetDescription("U3 Storage Attribute");
			E   ::SetName("E");    E   ::SetDescription("E Storage Attribute");
			ERPN::SetName("ERPN"); ERPN::SetDescription("Extended Real Page Number");
		}
	};
	
	// Data Cache Debug Tag Register Low
	struct DCDBTRL : PrivilegedSPR<DCDBTRL, 924>
	{
		typedef PrivilegedSPR<DCDBTRL, 924> Super;
		
		struct UPAR : Field<UPAR, 13    > {}; // U bit parity
		struct TPAR : Field<TPAR, 14, 15> {}; // Tag parity
		struct DPAR : Field<DPAR, 16, 19> {}; // Data parity
		struct MPAR : Field<MPAR, 20, 23> {}; // Modified (dirty) parity
		struct D    : Field<D   , 24, 27> {}; // Dirty indicators
		struct U0   : Field<U0  , 28    > {}; // U0 Storage Attribute
		struct U1   : Field<U1  , 29    > {}; // U1 Storage Attribute
		struct U2   : Field<U2  , 30    > {}; // U2 Storage Attribute
		struct U3   : Field<U3  , 31    > {}; // U3 Storage Attribute
		
		typedef FieldSet<UPAR, TPAR, DPAR, MPAR, D, U0, U1, U2, U3> ALL;
		
		DCDBTRL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DCDBTRL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("DCDBTRL");  this->SetDescription("Data Cache Debug Tag Register Low");
			
			UPAR::SetName("UPAR"); UPAR::SetDescription("U bit parity");
			TPAR::SetName("TPAR"); TPAR::SetDescription("Tag parity");
			DPAR::SetName("DPAR"); DPAR::SetDescription("Data parity");
			MPAR::SetName("MPAR"); MPAR::SetDescription("Modified (dirty) parity");
			D   ::SetName("D");    D   ::SetDescription("Dirty indicators");
			U0  ::SetName("U0");   U0  ::SetDescription("U0 Storage Attribute");
			U1  ::SetName("U1");   U1  ::SetDescription("U1 Storage Attribute");
			U2  ::SetName("U2");   U2  ::SetDescription("U2 Storage Attribute");
			U3  ::SetName("U3");   U3  ::SetDescription("U3 Storage Attribute");
		}
	};

	// Data Cache Debug Tag Register High
	struct DCDBTRH : PrivilegedSPR<DCDBTRH, 925>
	{
		typedef PrivilegedSPR<DCDBTRH, 925> Super;
		
		struct TRA  : Field<TRA , 0 , 23> {}; // Tag Real Address
		struct V    : Field<V   , 24    > {}; // Cache Line Valid
		struct TERA : Field<TERA, 28, 31> {}; // Tag Extended Real Address
		
		typedef FieldSet<TRA, V, TERA> ALL;
		
		DCDBTRH(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DCDBTRH(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("DCDBTRH");  this->SetDescription("Data Cache Debug Tag Register High");
			
			TRA ::SetName("TRA");  TRA ::SetDescription("Tag Real Address");
			V   ::SetName("V");    V   ::SetDescription("Cache Line Valid");
			TERA::SetName("TERA"); TERA::SetDescription("Tag Extended Real Address");
		}
	};
	
	// Instruction Cache Debug Tag Register Low
	struct ICDBTRL : ReadOnlyNonPrivilegedSPR<ICDBTRL, 926>
	{
		typedef ReadOnlyNonPrivilegedSPR<ICDBTRL, 926> Super;
		
		struct TS  : Field<TS , 22    > {}; // Translation Space
		struct TD  : Field<TD , 23    > {}; // Translation ID (TID) Disable
		struct TID : Field<TID, 24, 31> {}; // Translation ID
		
		typedef FieldSet<TS, TD, TID> ALL;
		
		ICDBTRL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ICDBTRL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("ICDBTRL");  this->SetDescription("Instruction Cache Debug Tag Register Low");
			
			TS ::SetName("TS");  TS ::SetDescription("Translation Space");
			TD ::SetName("TD");  TD ::SetDescription("Translation ID (TID) Disable");
			TID::SetName("TID"); TID::SetDescription("Translation ID");
		}
	};
	
	// Instruction Cache Debug Tag Register High
	struct ICDBTRH : ReadOnlyNonPrivilegedSPR<ICDBTRH, 927>
	{
		typedef ReadOnlyNonPrivilegedSPR<ICDBTRH, 927> Super;
		
		struct TEA   : Field<TEA  , 0 , 23> {}; // Tag Effective Address
		struct V     : Field<V    , 24    > {}; // Cache Line Valid
		struct TPAR  : Field<TPAR , 25, 26> {}; // Tag Parity
		struct DAPAR : Field<DAPAR, 27    > {}; // Instruction Data parity
		
		typedef FieldSet<TEA, V, TPAR, DAPAR> ALL;
		
		ICDBTRH(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ICDBTRH(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("ICDBTRH");  this->SetDescription("Instruction Cache Debug Tag Register High");
			
			TEA  ::SetName("TEA");   TEA  ::SetDescription("Tag Effective Address");
			V    ::SetName("V");     V    ::SetDescription("Cache Line Valid");
			TPAR ::SetName("TPAR");  TPAR ::SetDescription("Tag Parity");
			DAPAR::SetName("DAPAR"); DAPAR::SetDescription("Instruction Data parity");
		}
	};
	
	// Memory Management Unit Control Register
	struct MMUCR : PrivilegedSPR<MMUCR, 946>
	{
		typedef PrivilegedSPR<MMUCR, 946> Super;
		
		struct SWOA    : Field<SWOA   , 7     > {}; // Store Without Allocate
		struct U1TE    : Field<U1TE   , 9     > {}; // U1 Transient Enable
		struct U2SWOAE : Field<U2SWOAE, 10    > {}; // U2 Store without Allocate Enable
		struct DULXE   : Field<DULXE  , 12    > {}; // Data Cache Unlock Exception Enable
		struct IULXE   : Field<IULXE  , 13    > {}; // Instruction Cache Unlock Exception Enable
		struct STS     : Field<STS    , 15    > {}; // Search Translation Space
		struct STID    : Field<STID   , 24, 31> {}; // Search Translation ID
		
		typedef FieldSet<SWOA, U1TE, U2SWOAE, DULXE, IULXE, STS, STID> ALL;
		
		MMUCR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MMUCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("MMUCR");  this->SetDescription("Memory Management Unit Control Register");
			
			SWOA   ::SetName("SWOA");    SWOA   ::SetDescription("Store Without Allocate");
			U1TE   ::SetName("U1TE");    U1TE   ::SetDescription("U1 Transient Enable");
			U2SWOAE::SetName("U2SWOAE"); U2SWOAE::SetDescription("U2 Store without Allocate Enable");
			DULXE  ::SetName("DULXE");   DULXE  ::SetDescription("Data Cache Unlock Exception Enable");
			IULXE  ::SetName("IULXE");   IULXE  ::SetDescription("Instruction Cache Unlock Exception Enable");
			STS    ::SetName("STS");     STS    ::SetDescription("Search Translation Space");
			STID   ::SetName("STID");    STID   ::SetDescription("Search Translation ID");
		}
	};
	
	// Core Configuration Register 0
	struct CCR0 : PrivilegedSPR<CCR0, 947>
	{
		typedef PrivilegedSPR<CCR0, 947> Super;
		
		struct PRE    : Field<PRE   ,  1    > {}; // Parity Recovery Enable
		struct CRPE   : Field<CRPE  ,  4    > {}; // Cache Read Parity Enable
		struct DSTG   : Field<DSTG  , 10    > {}; // Disable Store Gathering
		struct DAPUIB : Field<DAPUIB, 11    > {}; // Disable APU Instruction Broadcast
		struct DTB    : Field<DTB   , 16    > {}; // Disable Trace Broadcast
		struct GICBT  : Field<GICBT , 17    > {}; // Guaranteed Instruction Cache Block Touch
		struct GDCBT  : Field<GDCBT , 18    > {}; // Guaranteed Data Cache Block Touch
		struct FLSTA  : Field<FLSTA , 23    > {}; // Force Load/Store Alignment
		struct ICSLC  : Field<ICSLC , 28, 29> {}; // Instruction Cache Speculative Line Count
		struct ICSLT  : Field<ICSLT , 30, 31> {}; // Instruction Cache Speculative Live Threshold
		
		typedef FieldSet<PRE, CRPE, DSTG, DAPUIB, DTB, GICBT, GDCBT, FLSTA, ICSLC, ICSLT> ALL;
		
		CCR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		CCR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { /* unaffected */ }
	private:
		void Init()
		{
			this->SetName("CCR0");  this->SetDescription("Core Configuration Register 0");
			
			PRE   ::SetName("PRE");    PRE   ::SetDescription("Parity Recovery Enable");
			CRPE  ::SetName("CRPE");   CRPE  ::SetDescription("Cache Read Parity Enable");
			DSTG  ::SetName("DSTG");   DSTG  ::SetDescription("Disable Store Gathering");
			DAPUIB::SetName("DAPUIB"); DAPUIB::SetDescription("Disable APU Instruction Broadcast");
			DTB   ::SetName("DTB");    DTB   ::SetDescription("Disable Trace Broadcast");
			GICBT ::SetName("GICBT");  GICBT ::SetDescription("Guaranteed Instruction Cache Block Touch");
			GDCBT ::SetName("GDCBT");  GDCBT ::SetDescription("Guaranteed Data Cache Block Touch");
			FLSTA ::SetName("FLSTA");  FLSTA ::SetDescription("Force Load/Store Alignment");
			ICSLC ::SetName("ICSLC");  ICSLC ::SetDescription("Instruction Cache Speculative Line Count");
			ICSLT ::SetName("ICSLT");  ICSLT ::SetDescription("Instruction Cache Speculative Live Threshold");
		}
	};

	// L1 Flush and Invalidate Control Register 1
	struct L1FINV1 : PrivilegedSPR<L1FINV1, 959>
	{
		typedef PrivilegedSPR<L1FINV1, 959> Super;
		
		struct CWAY : Field<CWAY, 7>      {}; // Cache Way
		struct CSET : Field<CSET, 19, 26> {}; // Cache Set
		struct CCMD : Field<CCMD, 29, 31> {}; // Cache Command
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;

		L1FINV1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1FINV1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			      this->SetName("L1FINV1");    this->SetDescription("L1 Flush and Invalidate Control Register 1");
			CWAY::SetName("CWAY"); CWAY::SetDescription("Cache Way");
			CSET::SetName("CSET"); CSET::SetDescription("Cache Set");
			CCMD::SetName("CCMD"); CCMD::SetDescription("Cache Command");
		}
	};
	
	// Debug Event Register
	struct DEVENT : NonPrivilegedSPR<DEVENT, 975>
	{
		typedef NonPrivilegedSPR<DEVENT, 975> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DEVENT(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DEVENT(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DEVENT"); this->SetDescription("Debug Event Register"); }
	};
	
	// Instruction Cache Debug Data Register
	struct ICDBDR : ReadOnlyPrivilegedSPR<ICDBDR, 979>
	{
		typedef ReadOnlyPrivilegedSPR<ICDBDR, 979> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		ICDBDR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ICDBDR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("ICDBDR"); this->SetDescription("Instruction Cache Debug Data Register"); }
	};
	
	// System Information
	struct SIR : ReadOnlyPrivilegedSPR<SIR, 992>
	{
		typedef ReadOnlyPrivilegedSPR<SIR, 992> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		SIR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("SIR"); this->SetDescription("System Information"); }
	};

	// Hardware Implementation Dependent Register 0
	struct HID0 : PrivilegedSPR<HID0, 1008>
	{
		typedef PrivilegedSPR<HID0, 1008> Super;
		
		struct EMCP        : Field<EMCP       , 0>  {}; // Enable machine check pin (p_mcp_b)
		struct TBEN        : Field<TBEN       , 5 > {}; // Time base enable
		struct STEN        : Field<STEN       , 7 > {}; // Software table search enable
		struct HIGH_BAT_EN : Field<HIGH_BAT_EN, 8 > {}; // Additional BATs enabled
		struct NAP         : Field<NAP        , 9 > {}; // Nap mode enable
		struct SLEEP       : Field<SLEEP      , 10> {}; // Sleep mode enable
		struct DPM         : Field<DPM        , 11> {}; // Dynamic power management enable
		struct BHTCLR      : Field<BHTCLR     , 13> {}; // Clear branch history table
		struct XAEN        : Field<XAEN       , 14> {}; // Extended addressing enabled
		struct ICR         : Field<ICR        , 14> {}; // Input Inputs Clear Reservation
		struct NHR         : Field<NHR        , 15> {}; // Not hard reset
		struct ICE         : Field<ICE        , 16> {}; // Instruction cache enable
		struct DCE         : Field<DCE        , 17> {}; // Data cache enable
		struct ILOCK       : Field<ILOCK      , 18> {}; // Instruction cache lock
		struct DLOCK       : Field<DLOCK      , 19> {}; // Data cache lock
		struct DCLREE      : Field<DCLREE     , 19> {}; // Debug Interrupt Clears MSR[EE]
		struct ICFI        : Field<ICFI       , 20> {}; // Instruction cache flash invalidate
		struct DCLRCE      : Field<DCLRCE     , 20> {}; // Debug Interrupt Clears MSR[CE]
		struct DCFI        : Field<DCFI       , 21> {}; // Data cache flash invalidate
		struct CICLRDE     : Field<CICLRDE    , 21> {}; // Critical Interrupt Clears MSR[DE]
		struct SPD         : Field<SPD        , 22> {}; // Speculative data cache and instruction cache access disable
		struct MCCLRDE     : Field<MCCLRDE    , 22> {}; // Machine Check Interrupt Clears MSR[DE]
		struct XBSEN       : Field<XBSEN      , 23> {}; // Extended BAT block size enable
		struct SGE         : Field<SGE        , 24> {}; // Store gathering enable
		struct BTIC        : Field<BTIC       , 26> {}; // Branch target instruction cache enable
		struct LRSTK       : Field<LRSTK      , 27> {}; // Link register stack enable
		struct FOLD        : Field<FOLD       , 28> {}; // Branch folding enable
		struct BHT         : Field<BHT        , 29> {}; // Branch history table enable
		struct NOPDST      : Field<NOPDST     , 30> {}; // No-op dst, dstt, dstst, and dststt instructions
		struct NOPTI       : Field<NOPTI      , 31> {}; // No-op the data cache touch instructions
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7441    , _) { typedef FieldSet<TBEN, STEN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7445    , _) { typedef FieldSet<TBEN, STEN, HIGH_BAT_EN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7447    , _) { typedef FieldSet<TBEN, STEN, HIGH_BAT_EN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { typedef FieldSet<TBEN, STEN, HIGH_BAT_EN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7448    , _) { typedef FieldSet<TBEN, STEN, HIGH_BAT_EN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7450    , _) { typedef FieldSet<TBEN, STEN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7451    , _) { typedef FieldSet<TBEN, STEN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7455    , _) { typedef FieldSet<TBEN, STEN, HIGH_BAT_EN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(MPC7457    , _) { typedef FieldSet<TBEN, STEN, HIGH_BAT_EN, NAP, SLEEP, DPM, BHTCLR, XAEN, NHR, ICE, DCE, ILOCK, DLOCK, ICFI, DCFI, SPD, XBSEN, SGE, BTIC, LRSTK, FOLD, BHT, NOPDST, NOPTI> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { typedef FieldSet<EMCP, ICR, NHR, DCLREE, DCLRCE, CICLRDE, MCCLRDE, NOPTI> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<EMCP, ICR, NHR, DCLREE, DCLRCE, CICLRDE, MCCLRDE, NOPTI> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		HID0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		HID0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{      
			   this->SetName("HID0");       this->SetDescription("Hardware Implementation Dependent Register 0");
			   
			   EMCP       ::SetName("EMCP");        EMCP       ::SetDescription("Enable machine check pin (p_mcp_b)");
			   TBEN       ::SetName("TBEN");        TBEN       ::SetDescription("Time base enable");
			   STEN       ::SetName("STEN");        STEN       ::SetDescription("Software table search enable");
			   HIGH_BAT_EN::SetName("HIGH_BAT_EN"); HIGH_BAT_EN::SetDescription("Additional BATs enabled");
			   NAP        ::SetName("NAP");         NAP        ::SetDescription("Nap mode enable");
			   SLEEP      ::SetName("SLEEP");       SLEEP      ::SetDescription("Sleep mode enable");
			   DPM        ::SetName("DPM");         DPM        ::SetDescription("Dynamic power management enable");
			   BHTCLR     ::SetName("BHTCLR");      BHTCLR     ::SetDescription("Clear branch history table");
			   XAEN       ::SetName("XAEN");        XAEN       ::SetDescription("Extended addressing enabled");
			   ICR        ::SetName("ICR");         ICR        ::SetDescription("Input Inputs Clear Reservation");
			   NHR        ::SetName("NHR");         NHR        ::SetDescription("Not hard reset");
			   ICE        ::SetName("ICE");         ICE        ::SetDescription("Instruction cache enable");
			   DCE        ::SetName("DCE");         DCE        ::SetDescription("Data cache enable");
			   ILOCK      ::SetName("ILOCK");       ILOCK      ::SetDescription("Instruction cache lock");
			   DLOCK      ::SetName("DLOCK");       DLOCK      ::SetDescription("Data cache lock");
			   DCLREE     ::SetName("DCLREE");      DCLREE     ::SetDescription("Debug Interrupt Clears MSR[EE]");
			   ICFI       ::SetName("ICFI");        ICFI       ::SetDescription("Instruction cache flash invalidate");
			   DCLRCE     ::SetName("DCLRCE");      DCLRCE     ::SetDescription("Debug Interrupt Clears MSR[CE]");
			   DCFI       ::SetName("DCFI");        DCFI       ::SetDescription("Data cache flash invalidate");
			   CICLRDE    ::SetName("CICLRDE");     CICLRDE    ::SetDescription("Critical Interrupt Clears MSR[DE]");
			   SPD        ::SetName("SPD");         SPD        ::SetDescription("Speculative data cache and instruction cache access disable");
			   MCCLRDE    ::SetName("MCCLRDE");     MCCLRDE    ::SetDescription("Machine Check Interrupt Clears MSR[DE]");
			   XBSEN      ::SetName("XBSEN");       XBSEN      ::SetDescription("Extended BAT block size enable");
			   SGE        ::SetName("SGE");         SGE        ::SetDescription("Store gathering enable");
			   BTIC       ::SetName("BTIC");        BTIC       ::SetDescription("Branch target instruction cache enable");
			   LRSTK      ::SetName("LRSTK");       LRSTK      ::SetDescription("Link register stack enable");
			   FOLD       ::SetName("FOLD");        FOLD       ::SetDescription("Branch folding enable");
			   BHT        ::SetName("BHT");         BHT        ::SetDescription("Branch history table enable");
			   NOPDST     ::SetName("NOPDST");      NOPDST     ::SetDescription("No-op dst, dstt, dstst, and dststt instructions");
			   NOPTI      ::SetName("NOPTI");       NOPTI      ::SetDescription("No-op the data cache touch instructions");
		}
	};

	// Hardware Implementation Dependent Register 1
	struct HID1 : PrivilegedSPR<HID1, 1009>
	{
		typedef PrivilegedSPR<HID1, 1009> Super;
		
		struct EMCP   : Field<EMCP  , 0 > {}; // Machine check signal enable
		struct EBA    : Field<EBA   , 2 > {}; // Enable/disable 60x/MPX bus address bus parity checking
		struct EBD    : Field<EBD   , 3 > {}; // Enable/disbale MPX/60x bus data parity checking
		struct BCLK   : Field<BCLK  , 4 > {}; // CLK_OUT output enable and clock type selection
		struct ECLK   : Field<ECLK  , 6 > {}; // CLK_OUT output enable and clock type selection
		struct PAR    : Field<PAR   , 7 > {}; // Disable precharge of ARTRY#, SHD0#, SHD1# pins
		struct DFS4   : Field<DFS4  , 8 > {}; // Dynamic frequency switching (DFS) divide-by-four mode
		struct DFS2   : Field<DFS2  , 9 > {}; // Dynamic frequency switching (DFS) divide-by-two mode
		struct PC5    : Field<PC5   , 14> {}; // PLL configuration bit 5
		struct PC0    : Field<PC0   , 15> {}; // PLL configuration bit 0
		struct PC1    : Field<PC1   , 16> {}; // PLL configuration bit 1
		struct PC2    : Field<PC2   , 17> {}; // PLL configuration bit 2
		struct PC3    : Field<PC3   , 18> {}; // PLL configuration bit 3
		struct PC4    : Field<PC4   , 19> {}; // PLL configuration bit 4
		struct SYNCBE : Field<SYNCBE, 20> {}; // Address broadcast enable for sync, eieio
		struct ABE    : Field<ABE   , 21> {}; // Address broadcast enable for dcbf, dcbst, dcbi, icbi, tlbie, and tlbsync
		
		struct HP_NOR : Field<HP_NOR, 22> {}; // High priority elevation for normal and external interrupts
		struct HP_NMI : Field<HP_NMI, 23> {}; // High priority elevation for NMI and critical interrupts
		struct ATS    : Field<ATS   , 24> {}; // Atomic status
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7441    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7445    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7447    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7447A   , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, DFS2, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7448    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, DFS4, DFS2, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7450    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7451    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7455    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(MPC7457    , _) { typedef FieldSet<EMCP, EBA, EBD, BCLK, ECLK, PAR, PC5, PC0, PC1, PC2, PC3, PC4, SYNCBE, ABE> ALL; };
		CASE_ENUM_TRAIT(E200Z710N3 , _) { typedef FieldSet<HP_NOR, HP_NMI, ATS> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<HP_NOR, HP_NMI, ATS> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		HID1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		HID1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("HID1"); this->SetDescription("Hardware Implementation Dependent Register 1");
			
			EMCP  ::SetName("EMCP");   EMCP  ::SetDescription("Machine check signal enable");
			EBA   ::SetName("EBA");    EBA   ::SetDescription("Enable/disable 60x/MPX bus address bus parity checking");
			EBD   ::SetName("EBD");    EBD   ::SetDescription("Enable/disbale MPX/60x bus data parity checking");
			BCLK  ::SetName("BCLK");   BCLK  ::SetDescription("CLK_OUT output enable and clock type selection");
			ECLK  ::SetName("ECLK");   ECLK  ::SetDescription("CLK_OUT output enable and clock type selection");
			PAR   ::SetName("PAR");    PAR   ::SetDescription("Disable precharge of ARTRY#, SHD0#, SHD1# pins");
			DFS4  ::SetName("DFS4");   DFS4  ::SetDescription("Dynamic frequency switching (DFS) divide-by-four mode");
			DFS2  ::SetName("DFS2");   DFS2  ::SetDescription("Dynamic frequency switching (DFS) divide-by-two mode");
			PC5   ::SetName("PC5");    PC5   ::SetDescription("PLL configuration bit 5");
			PC0   ::SetName("PC0");    PC0   ::SetDescription("PLL configuration bit 0");
			PC1   ::SetName("PC1");    PC1   ::SetDescription("PLL configuration bit 1");
			PC2   ::SetName("PC2");    PC2   ::SetDescription("PLL configuration bit 2");
			PC3   ::SetName("PC3");    PC3   ::SetDescription("PLL configuration bit 3");
			PC4   ::SetName("PC4");    PC4   ::SetDescription("PLL configuration bit 4");
			SYNCBE::SetName("SYNCBE"); SYNCBE::SetDescription("Address broadcast enable for sync, eieio");
			ABE   ::SetName("ABE");    ABE   ::SetDescription("Address broadcast enable for dcbf, dcbst, dcbi, icbi, tlbie, and tlbsync");
			
			HP_NOR::SetName("HP_NOR"); HP_NOR::SetDescription("High priority elevation for normal and external interrupts");
			HP_NMI::SetName("HP_NMI"); HP_NMI::SetDescription("High priority elevation for NMI and critical interrupts");
			ATS   ::SetName("ATS");    ATS   ::SetDescription("Atomic status");
		}
	};
	
	// Instruction Address Breakpoint Register 
	struct IABR : PrivilegedSPR<IABR, 1010>
	{
		typedef PrivilegedSPR<IABR, 1010> Super;
		
		struct Address : Field<Address, 0 , 29> {}; // Word instruction breakpoint address to be compared with EA[0-29] of the next instruction
		struct BE      : Field<BE     , 30    > {}; // Breakpoint enabled
		struct TE      : Field<TE     , 31    > {}; // Translation enable
		
		typedef FieldSet<Address, BE, TE> ALL;
		
		IABR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IABR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("IABR"); this->SetDescription("Instruction Address Breakpoint Register");
			
			Address::SetName("Address"); Address::SetDescription("Word instruction breakpoint address to be compared with EA[0-29] of the next instruction");
			BE     ::SetName("BE");      BE     ::SetDescription("Breakpoint enabled");
			TE     ::SetName("TE");      TE     ::SetDescription("Translation enable");
		}
	};

	// L1 Cache Control and Status Register 0
	struct L1CSR0 : PrivilegedSPR<L1CSR0, 1010>
	{
		typedef PrivilegedSPR<L1CSR0, 1010> Super;
		
		struct WID     : Field<WID    , 0, 1>   {}; // Way Instruction Disable
		struct WDD     : Field<WDD    , 4, 5>   {}; // Way Data Disable
		struct DCWA    : Field<DCWA   , 12>     {}; // Data Cache Write Allocation Policy
		struct DCECE   : Field<DCECE  , 15>     {}; // Data Cache Error Checking Enable
		struct DCEI    : Field<DCEI   , 16>     {}; // Data Cache Error Injection
		struct DCLOC   : Field<DCLOC  , 17, 18> {}; // Data Cache Lockout Control
		struct DCEA    : Field<DCEA   , 25, 26> {}; // Data Cache Error Action
		struct DCLOINV : Field<DCLOINV, 27>     {}; // Data Cache Lockout Indicator Invalidate
		struct DCABT   : Field<DCABT  , 29>     {}; // Data Cache Operation Aborted
		struct DCINV   : Field<DCINV  , 30>     {}; // Data Cache Invalidate
		struct DCE     : Field<DCE    , 31>     {}; // Data Cache Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<WID, WDD, DCWA, DCECE, DCEI, DCLOC, DCEA, DCLOINV, DCABT, DCINV, DCE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<WID> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			         this->SetName("L1CSR0");           this->SetDescription("L1 Cache Control and Status Register 0");
			WID    ::SetName("WID");     WID    ::SetDescription("Way Instruction Disable");
			WDD    ::SetName("WDD");     WDD    ::SetDescription("Way Data Disable");
			DCWA   ::SetName("DCWA");    DCWA   ::SetDescription("Data Cache Write Allocation Policy");
			DCECE  ::SetName("DCECE");   DCECE  ::SetDescription("Data Cache Error Checking Enable");
			DCEI   ::SetName("DCEI");    DCEI   ::SetDescription("Data Cache Error Injection");
			DCLOC  ::SetName("DCLOC");   DCLOC  ::SetDescription("Data Cache Lockout Control");
			DCEA   ::SetName("DCEA");    DCEA   ::SetDescription("Data Cache Error Action");
			DCLOINV::SetName("DCLOINV"); DCLOINV::SetDescription("Data Cache Lockout Indicator Invalidate");
			DCABT  ::SetName("DCABT");   DCABT  ::SetDescription("Data Cache Operation Aborted");
			DCINV  ::SetName("DCINV");   DCINV  ::SetDescription("Data Cache Invalidate");
			DCE    ::SetName("DCE");     DCE    ::SetDescription("Data Cache Enable");
		}
	};

	// L1 Cache Control and Status Register 1
	struct L1CSR1 : PrivilegedSPR<L1CSR1, 1011>
	{
		typedef PrivilegedSPR<L1CSR1, 1011> Super;
		
		struct ICECE   : Field<ICECE  , 15>     {}; // Instruction Cache Error Checking Enable
		struct ICEI    : Field<ICEI   , 16>     {}; // Instruction Cache Error Injection Enable
		struct ICLOC   : Field<ICLOC  , 17, 18> {}; // Instruction Cache Lockout Control
		struct ICEA    : Field<ICEA   , 25, 26> {}; // Instruction Cache Error Action
		struct ICLOINV : Field<ICLOINV, 27>     {}; // Instruction Cache Lockout Indicator Invalidate
		struct ICABT   : Field<ICABT  , 29>     {}; // Instruction Cache Operation Aborted
		struct ICINV   : Field<ICINV  , 30>     {}; // Instruction Cache Invalidate
		struct ICE     : Field<ICE    , 31>     {}; // Instruction Cache Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ICECE, ICEI, ICLOC, ICEA, ICLOINV, ICABT, ICINV, ICE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ICECE, ICEI, ICLOC, ICEA, ICLOINV, ICABT, ICINV, ICE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1CSR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1CSR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			         this->SetName("L1CSR1");           this->SetDescription("L1 Cache Control and Status Register 1");
			ICECE  ::SetName("ICECE");   ICECE  ::SetDescription("Instruction Cache Error Checking Enable");
			ICEI   ::SetName("ICEI");    ICEI   ::SetDescription("Instruction Cache Error Injection Enable");
			ICLOC  ::SetName("ICLOC");   ICLOC  ::SetDescription("Instruction Cache Lockout Control");
			ICEA   ::SetName("ICEA");    ICEA   ::SetDescription("Instruction Cache Error Action");
			ICLOINV::SetName("ICLOINV"); ICLOINV::SetDescription("Instruction Cache Lockout Indicator Invalidate");
			ICABT  ::SetName("ICABT");   ICABT  ::SetDescription("Instruction Cache Operation Aborted");
			ICINV  ::SetName("ICINV");   ICINV  ::SetDescription("Instruction Cache Invalidate");
			ICE    ::SetName("ICE");     ICE    ::SetDescription("Instruction Cache Enable");
		}
	};
	
	// Debug Data Register
	struct DBDR : PrivilegedSPR<DBDR, 1011>
	{
		typedef PrivilegedSPR<DBDR, 1011> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		DBDR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DBDR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init() { this->SetName("DBDR"); this->SetDescription("Debug Data Register"); }
	};

	// Instruction Cache and Interrupt Control Register 
	struct ICTRL : PrivilegedSPR<ICTRL, 1011>
	{
		typedef PrivilegedSPR<ICTRL, 1011> Super;
		
		struct CIRQ : Field<CIRQ, 0     > {}; // CPU interrupt request
		struct EIEC : Field<EIEC, 4     > {}; // Instruction cache parity error enable
		struct EDCE : Field<EDCE, 5     > {}; // Data cache parity error enable
		struct EICP : Field<EICP, 23    > {}; // Enable instruction cache parity checking
		struct ICWL : Field<ICWL, 24, 31> {}; // Instruction cache way lock
		
		typedef FieldSet<CIRQ, EIEC, EDCE, EICP, ICWL> ALL;
		
		ICTRL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ICTRL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			this->SetName("ICTRL"); this->SetDescription("Instruction Cache and Interrupt Control Register");
			
			CIRQ::SetName("CIRQ"); CIRQ::SetDescription("CPU interrupt request");
			EIEC::SetName("EIEC"); EIEC::SetDescription("Instruction cache parity error enable");
			EDCE::SetName("EDCE"); EDCE::SetDescription("Data cache parity error enable");
			EICP::SetName("EICP"); EICP::SetDescription("Enable instruction cache parity checking");
			ICWL::SetName("ICWL"); ICWL::SetDescription("Instruction cache way lock");
		}
	};
	
	// Load/Store Debug Register (undocumented)
	struct LDSTDB : PrivilegedSPR<LDSTDB, 1012>
	{
		typedef PrivilegedSPR<LDSTDB, 1012> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		LDSTDB(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		LDSTDB(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("LDSTDB"); this->SetDescription("Load/Store Debug Register (undocumented)");
		}
	};

	// Branch Unit Control and Status Register
	struct BUCSR : PrivilegedSPR<BUCSR, 1013>
	{
		typedef PrivilegedSPR<BUCSR, 1013> Super;
		
		struct BBFI   : Field<BBFI  , 22>     {}; // Branch target buffer flash Invalidate
		struct BALLOC : Field<BALLOC, 26, 27> {}; // Branch Target Buffer Allocation Control
		struct BPRED  : Field<BPRED , 29, 30> {}; // Branch Prediction Control (Static)
		struct BPEN   : Field<BPEN  , 31>     {}; // Branch target buffer prediction enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<BBFI, BALLOC, BPRED, BPEN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<BBFI, BALLOC, BPRED, BPEN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		BUCSR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		BUCSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			        this->SetName("BUCSR");          this->SetDescription("Branch Unit Control and Status Register");
			BBFI  ::SetName("BBFI");   BBFI  ::SetDescription("Branch target buffer flash Invalidate");
			BALLOC::SetName("BALLOC"); BALLOC::SetDescription("Branch Target Buffer Allocation Control");
			BPRED ::SetName("BPRED");  BPRED ::SetDescription("Branch Prediction Control (Static)");
			BPEN  ::SetName("BPEN");   BPEN  ::SetDescription("Branch target buffer prediction enable");
		}
	};

	// Data Address Breakpoint Register 
	struct DABR : PrivilegedSPR<DABR, 1013>
	{
		typedef PrivilegedSPR<DABR, 1013> Super;
		
		struct DAB : Field<DAB, 0 , 28> {}; // Data address breakpoint
		struct BT  : Field<BT , 29    > {}; // Breakpoint translation enable
		struct DW  : Field<DW , 30    > {}; // Data write enable
		struct DR  : Field<DR , 31    > {}; // Data read enable
		        
		typedef FieldSet<DAB, BT, DW, DR> ALL;
		
		DABR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DABR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("DABR"); this->SetDescription("Data Address Breakpoint Register");
			
			DAB::SetName("DAB"); DAB::SetDescription("Data address breakpoint");
			BT ::SetName("BT");  BT ::SetDescription("Breakpoint translation enable");
			DW ::SetName("DW");  DW ::SetDescription("Data write enable");
			DR ::SetName("DR");  DR ::SetDescription("Data read enable");
		}
	};
	
	// MPU0 Control and Status Register 0
	struct MPU0CSR0 : PrivilegedSPR<MPU0CSR0, 1014>
	{
		typedef PrivilegedSPR<MPU0CSR0, 1014> Super;
		
		struct BYPSR  : Field<BYPSR , 16> {}; // Bypass MPU protection for Supervisor Read accesses
		struct BYPSW  : Field<BYPSW , 17> {}; // Bypass MPU protection for Supervisor Write accesses
		struct BYPSX  : Field<BYPSX , 18> {}; // Bypass MPU protection for Supervisor Instruction accesses
		struct BYPUR  : Field<BYPUR , 19> {}; // Bypass MPU protection for User Read accesses
		struct BYPUW  : Field<BYPUW , 20> {}; // Bypass MPU protection for User Write accesses
		struct BYPUX  : Field<BYPUX , 21> {}; // Bypass MPU protection for User Instruction accesses
		struct DRDEND : Field<DRDEND, 24> {}; // Data Read Debug Enable
		struct DWDEN  : Field<DWDEN , 25> {}; // Data Write Debug Enable
		struct IDEN   : Field<IDEN  , 26> {}; // Instruction Debug Enable
		struct TIDCTL : Field<TIDCTL, 28> {}; // TID usage control
		struct MPUFI  : Field<MPUFI , 30> {}; // MPU flash invalidate
		struct MPUEN  : Field<MPUEN , 31> {}; // MPU Enable
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<BYPSR, BYPSW, BYPSX, BYPUR, BYPUW, BYPUX, DRDEND, DWDEN, IDEN, TIDCTL, MPUFI, MPUEN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<BYPSR, BYPSW, BYPSX, BYPUR, BYPUW, BYPUX, DRDEND, DWDEN, IDEN, TIDCTL, MPUFI, MPUEN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MPU0CSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MPU0CSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			        this->SetName("MPU0CSR0");       this->SetDescription("MPU0 Control and Status Register 0");
			BYPSR ::SetName("BYPSR");  BYPSR ::SetDescription("Bypass MPU protection for Supervisor Read accesses");
			BYPSW ::SetName("BYPSW");  BYPSW ::SetDescription("Bypass MPU protection for Supervisor Write accesses");
			BYPSX ::SetName("BYPSX");  BYPSX ::SetDescription("Bypass MPU protection for Supervisor Instruction accesses");
			BYPUR ::SetName("BYPUR");  BYPUR ::SetDescription("Bypass MPU protection for User Read accesses");
			BYPUW ::SetName("BYPUW");  BYPUW ::SetDescription("Bypass MPU protection for User Write accesses");
			BYPUX ::SetName("BYPUX");  BYPUX ::SetDescription("Bypass MPU protection for User Instruction accesses");
			DRDEND::SetName("DRDEND"); DRDEND::SetDescription("Data Read Debug Enable");
			DWDEN ::SetName("DWDEN");  DWDEN ::SetDescription("Data Write Debug Enable");
			IDEN  ::SetName("IDEN");   IDEN  ::SetDescription("Instruction Debug Enable");
			TIDCTL::SetName("TIDCTL"); TIDCTL::SetDescription("TID usage control");
			MPUFI ::SetName("MPUFI");  MPUFI ::SetDescription("MPU flash invalidate");
			MPUEN ::SetName("MPUEN");  MPUEN ::SetDescription("MPU Enable");
		}
	};

	// MMU/MPU Configuration Register
	struct MMUCFG : ReadOnlyPrivilegedSPR<MMUCFG, 1015>
	{
		typedef ReadOnlyPrivilegedSPR<MMUCFG, 1015> Super;
		
		struct RASIZE  : Field<RASIZE , 8, 14>  {}; // Number of Bits of Real Address supported
		struct PIDSIZE : Field<PIDSIZE, 21, 25> {}; // PID Register Size
		struct NMPUS   : Field<NMPUS  , 26, 27> {}; // Number of MPUs
		struct NTLBS   : Field<NTLBS  , 28, 29> {}; // Number of TLBs
		struct MAVN    : Field<MAVN   , 30, 31> {}; // MMU Architecture Version Number
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<RASIZE, PIDSIZE, NMPUS, NTLBS, MAVN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<RASIZE, PIDSIZE, NMPUS, NTLBS, MAVN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MMUCFG(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MMUCFG(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			         this->SetName("MMUCFG");           this->SetDescription("MMU/MPU Configuration Register");
			RASIZE ::SetName("RASIZE");  RASIZE ::SetDescription("Number of Bits of Real Address supported");
			PIDSIZE::SetName("PIDSIZE"); PIDSIZE::SetDescription("PID Register Size");
			NMPUS  ::SetName("NMPUS");   NMPUS  ::SetDescription("Number of MPUs");
			NTLBS  ::SetName("NTLBS");   NTLBS  ::SetDescription("Number of TLBs");
			MAVN   ::SetName("MAVN");    MAVN   ::SetDescription("MMU Architecture Version Number");
		}
	};
	
	// Memory Subsystem Control Register
	struct MSSCR0 : PrivilegedSPR<MSSCR0, 1014>
	{
		typedef PrivilegedSPR<MSSCR0, 1014> Super;
		
		struct DTQ     : Field<DTQ    , 3 , 5                                > {}; // DTQ size
		struct EIDIS   : Field<EIDIS  , 7                                    > {}; // Disable external intervention in MPX bus mode
		struct L3TCEXT : Field<L3TCEXT, 10                                   > {}; // L3 turn around clockcount extension
		struct ABD     : Field<ABD    , 11                                   > {}; // Address bus driven mode
		struct L3TCEN  : Field<L3TCEN , 12                                   > {}; // L3 turnaround clock enable
		struct L3TC    : Field<L3TC   , 13, 14                               > {}; // L3 turnaround clock count
		struct BMODE   : Field<BMODE  , 16, 17, unisim::util::reg::core::SW_R> {}; // Bus mode
		struct ID      : Field<ID     , 26                                   > {}; // Processor identification
		struct L2PFE   : Field<L2PFE  , 30, 31                               > {}; // L2 prefetching enabled
		
		typedef FieldSet<DTQ, EIDIS, L3TCEXT, ABD, L3TCEN, L3TC, BMODE, ID, L2PFE> ALL;
		
		MSSCR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MSSCR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("MSSCR0"); this->SetDescription("Memory Subsystem Control Register");
			
			DTQ    ::SetName("DTQ");     DTQ    ::SetDescription("DTQ size");
			EIDIS  ::SetName("EIDIS");   EIDIS  ::SetDescription("Disable external intervention in MPX bus mode");
			L3TCEXT::SetName("L3TCEXT"); L3TCEXT::SetDescription("L3 turn around clockcount extension");
			ABD    ::SetName("ABD");     ABD    ::SetDescription("Address bus driven mode");
			L3TCEN ::SetName("L3TCEN");  L3TCEN ::SetDescription("L3 turnaround clock enable");
			L3TC   ::SetName("L3TC");    L3TC   ::SetDescription("L3 turnaround clock count");
			BMODE  ::SetName("BMODE");   BMODE  ::SetDescription("Bus mode");
			ID     ::SetName("ID");      ID     ::SetDescription("Processor identification");
			L2PFE  ::SetName("L2PFE");   L2PFE  ::SetDescription("L2 prefetching enabled");
		}
	};
	
	// Memory Subsystem Status Register
	struct MSSSR0 : PrivilegedSPR<MSSCR0, 1015>
	{
		typedef PrivilegedSPR<MSSCR0, 1015> Super;
		
		struct L2TAG : Field<L2TAG, 13> {}; // L2 tag parity error
		struct L2DAT : Field<L2DAT, 14> {}; // L2 data parity error
		struct L3TAG : Field<L3TAG, 15> {}; // L3 tag parity error
		struct L3DAT : Field<L3DAT, 16> {}; // L3 data parity error
		struct APE   : Field<APE  , 17> {}; // Address bus parity error
		struct DPE   : Field<DPE  , 18> {}; // Data bus parity error
		struct TEA   : Field<TEA  , 19> {}; // Bus Transfer error aknownledge
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7441 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7445 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _) { typedef FieldSet<L2TAG, L2DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7451 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7455 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _) { typedef FieldSet<L2TAG, L2DAT, L3TAG, L3DAT, APE, DPE, TEA> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		MSSSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MSSSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("MSSSR0"); this->SetDescription("Memory Subsystem Status Register");
			
			L2TAG::SetName("L2TAG"); L2TAG::SetDescription("L2 tag parity error");
			L2DAT::SetName("L2DAT"); L2DAT::SetDescription("L2 data parity error");
			L3TAG::SetName("L3TAG"); L3TAG::SetDescription("L3 tag parity error");
			L3DAT::SetName("L3DAT"); L3DAT::SetDescription("L3 data parity error");
			APE  ::SetName("APE");   APE  ::SetDescription("Address bus parity error");
			DPE  ::SetName("DPE");   DPE  ::SetDescription("Data bus parity error");
			TEA  ::SetName("TEA");   TEA  ::SetDescription("Bus Transfer error aknownledge");
		}
	};

	// L1 Flush and Invalidate Control Register 0
	struct L1FINV0 : PrivilegedSPR<L1FINV0, 1016>
	{
		typedef PrivilegedSPR<L1FINV0, 1016> Super;
		
		struct CWAY : Field<CWAY, 7>      {}; // Cache Way
		struct CSET : Field<CSET, 21, 26> {}; // Cache Set
		struct CCMD : Field<CCMD, 29, 31> {}; // Cache Command
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<CWAY, CSET, CCMD> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L1FINV0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L1FINV0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x00000000); }
	private:
		void Init()
		{
			      this->SetName("L1FINV0");    this->SetDescription("L1 Flush and Invalidate Control Register 0");
			CWAY::SetName("CWAY"); CWAY::SetDescription("CWAY");
			CSET::SetName("CSET"); CSET::SetDescription("CSET");
			CCMD::SetName("CCMD"); CCMD::SetDescription("CCMD");
		}
	};
	
	// Load/Store Control Register
	struct LDSTCR : PrivilegedSPR<LDSTCR, 1016>
	{
		typedef PrivilegedSPR<LDSTCR, 1016> Super;
		
		struct DCWL : Field<DCWL, 24, 31> {}; // Data cache way lock
		
		typedef FieldSet<DCWL> ALL;
		
		LDSTCR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		LDSTCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("LDSTCR"); this->SetDescription("Load/Store Control Register");
			
			DCWL::SetName("DCWL"); DCWL::SetDescription("Data cache way lock");
		}
	};
	
	// L2 Cache Control Register
	struct L2CR : PrivilegedSPR<L2CR, 1017>
	{
		typedef PrivilegedSPR<L2CR, 1017> Super;
		
		struct L2E    : Field<L2E   , 0                                    > {}; // L2 enable
		struct L2PE   : Field<L2PE  , 1                                    > {}; // L2 (tag and ) data parity checking enable
		struct L2I    : Field<L2I   , 10                                   > {}; // L2 global invalidate
		struct L2IO   : Field<L2IO  , 11                                   > {}; // L2 instruction-only
		struct L3OH0  : Field<L3OH0 , 12                                   > {}; // L3 output hold 0
		struct L2DO   : Field<L2DO  , 15                                   > {}; // L2 data only
		struct L2REP  : Field<L2REP , 19                                   > {}; // L2 replacement algorithm
		struct L2HWF  : Field<L2HWF , 20                                   > {}; // L2 hardware flush
		struct LVRAME : Field<LVRAME, 24                                   > {}; // LVRAM enable
		struct LVRAMM : Field<LVRAMM, 25, 27, unisim::util::reg::core::SW_R> {}; // LVRAM mode
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7441 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7445 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF, LVRAME, LVRAMM> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7451 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7455 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L3OH0, L2DO, L2REP, L2HWF> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _) { typedef FieldSet<L2E, L2PE, L2I, L2IO, L2DO, L2REP, L2HWF> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L2CR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2CR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
	private:
		void Init()
		{
			this->SetName("L2CR"); this->SetDescription("L2 Cache Control Register");
			
			L2E   ::SetName("L2E");    L2E   ::SetDescription("L2 enable");
			
			L2PE  ::SetName("L2PE");
			if(CONFIG::MODEL == MPC7448)
			{
				L2PE  ::SetDescription("L2 data parity checking enable");
			}
			else
			{
				L2PE  ::SetDescription("L2 tag and data parity checking enable");
			}
			
			L2I   ::SetName("L2I");    L2I   ::SetDescription("L2 global invalidate");
			L2IO  ::SetName("L2IO");   L2IO  ::SetDescription("L2 instruction-only");
			L3OH0 ::SetName("L3OH0");  L3OH0 ::SetDescription("L3 output hold 0");
			L2DO  ::SetName("L2DO");   L2DO  ::SetDescription("L2 data only");
			L2REP ::SetName("L2REP");  L2REP ::SetDescription("L2 replacement algorithm");
			L2HWF ::SetName("L2HWF");  L2HWF ::SetDescription("L2 hardware flush");
			LVRAME::SetName("LVRAME"); LVRAME::SetDescription("LVRAM enable");
			LVRAMM::SetName("LVRAMM"); LVRAMM::SetDescription("LVRAM mode");
			
			
		}
	};
	
	// L3 Cache Control Register
	struct L3CR : PrivilegedSPR<L3CR, 1018>
	{
		typedef PrivilegedSPR<L3CR, 1018> Super;
		
		struct L3E       : Field<L3E      , 0     > {}; // L3 enable
		struct L3PE      : Field<L3PE     , 1     > {}; // L3 data parity checking enable
		struct L3APE     : Field<L3APE    , 2     > {}; // L3 address parity checking enable
		struct L3SIZ     : Field<L3SIZ    , 3     > {}; // L3 size
		struct L3CLKEN   : Field<L3CLKEN  , 4     > {}; // Enables the L3_CLK[0:1] signals
		struct L3CLK     : Field<L3CLK    , 6, 8  > {}; // L3 clock ratio
		struct L3IO      : Field<L3IO     , 9     > {}; // L3 instruction-only mode
		struct L3CLKEXT  : Field<L3CLKEXT , 10    > {}; // L3 clock ratio extension
		struct L3CKSPEXT : Field<L3CKSPEXT, 11    > {}; // L3 clock sample point extension
		struct L3OH1     : Field<L3OH1    , 12    > {}; // L3 output hold 1
		struct L3SPO     : Field<L3SPO    , 13    > {}; // L3 sample point override
		struct L3CKSP    : Field<L3CKSP   , 14, 15> {}; // L3 clock sample point
		struct L3PSP     : Field<L3PSP    , 16, 18> {}; // L3 P-clock sample point
		struct L3REP     : Field<L3REP    , 19    > {}; // L3 replacement algorithm
		struct L3HWF     : Field<L3HWF    , 20    > {}; // L3 hardware flush
		struct L3I       : Field<L3I      , 21    > {}; // L3 global invalidate
		struct L3RT      : Field<L3RT     , 22, 23> {}; // L3 SRAM type
		struct L3NIRCA   : Field<L3NIRCA  , 24    > {}; // L3 non-integer ratio clock adjustment for the SRAM
		struct L3DO      : Field<L3DO     , 25    > {}; // L3 data-only mode
		struct PMEN      : Field<PMEN     , 29    > {}; // Private memory enable
		struct PMSIZ     : Field<PMSIZ    , 30, 31> {}; // Preivate memory size
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7441 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7445 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7447 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7447A, _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7448 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7450 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7451 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7455 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3OH1, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		CASE_ENUM_TRAIT(MPC7457 , _) { typedef FieldSet<L3E, L3PE, L3APE, L3SIZ, L3CLKEN, L3CLK, L3IO, L3CLKEXT, L3CKSPEXT, L3SPO, L3CKSP, L3PSP, L3REP, L3HWF, L3I, L3RT, L3NIRCA, L3DO, PMEN, PMSIZ> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		L3CR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3CR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
	private:
		void Init()
		{
			L3CR::SetName("L3CR"); L3CR::SetDescription("L3 Cache Control Register");
			
			L3E      ::SetName("L3E");       L3E      ::SetDescription("L3 enable");
			L3PE     ::SetName("L3PE");      L3PE     ::SetDescription("L3 data parity checking enable");
			L3APE    ::SetName("L3APE");     L3APE    ::SetDescription("L3 address parity checking enable");
			L3SIZ    ::SetName("L3SIZ");     L3SIZ    ::SetDescription("L3 size");
			L3CLKEN  ::SetName("L3CLKEN");   L3CLKEN  ::SetDescription("Enables the L3_CLK[0:1] signals");
			L3CLK    ::SetName("L3CLK");     L3CLK    ::SetDescription("L3 clock ratio");
			L3IO     ::SetName("L3IO");      L3IO     ::SetDescription("L3 instruction-only mode");
			L3CLKEXT ::SetName("L3CLKEXT");  L3CLKEXT ::SetDescription("L3 clock ratio extension");
			L3CKSPEXT::SetName("L3CKSPEXT"); L3CKSPEXT::SetDescription("L3 clock sample point extension");
			L3OH1    ::SetName("L3OH1");     L3OH1    ::SetDescription("L3 output hold 1");
			L3SPO    ::SetName("L3SPO");     L3SPO    ::SetDescription("L3 sample point override");
			L3CKSP   ::SetName("L3CKSP");    L3CKSP   ::SetDescription("L3 clock sample point");
			L3PSP    ::SetName("L3PSP");     L3PSP    ::SetDescription("L3 P-clock sample point");
			L3REP    ::SetName("L3REP");     L3REP    ::SetDescription("L3 replacement algorithm");
			L3HWF    ::SetName("L3HWF");     L3HWF    ::SetDescription("L3 hardware flush");
			L3I      ::SetName("L3I");       L3I      ::SetDescription("L3 global invalidate");
			L3RT     ::SetName("L3RT");      L3RT     ::SetDescription("L3 SRAM type");
			L3NIRCA  ::SetName("L3NIRCA");   L3NIRCA  ::SetDescription("L3 non-integer ratio clock adjustment for the SRAM");
			L3DO     ::SetName("L3DO");      L3DO     ::SetDescription("L3 data-only mode");
			PMEN     ::SetName("PMEN");      PMEN     ::SetDescription("Private memory enable");
			PMSIZ    ::SetName("PMSIZ");     PMSIZ    ::SetDescription("Preivate memory size");
		}
	};
	
	// Instruction Cache Throttling Control Register
	struct ICTC : PrivilegedSPR<ICTC, 1019>
	{
		typedef PrivilegedSPR<ICTC, 1019> Super;
		
		struct INTERVAL : Field<INTERVAL, 23, 30> {}; // Instruction forwarding interval expressed in processor clocks
		struct E        : Field<E       , 31    > {}; // Enable instruction throttling
		
		typedef FieldSet<INTERVAL, E> ALL;
		
		ICTC(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		ICTC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
	private:
		void Init()
		{
			ICTC::SetName("ICTC"); ICTC::SetDescription("Instruction Cache Throttling Control Register");
			
			INTERVAL::SetName("INTERVAL"); INTERVAL::SetDescription("Instruction forwarding interval expressed in processor clocks");
			E       ::SetName("E");        E       ::SetDescription("Enable instruction throttling");
		}
	};

	SWITCH_ENUM_TRAIT(Model, SVR_Mapping);
	CASE_ENUM_TRAIT(MPC7448, SVR_Mapping) { static const unsigned int SPR_NUM = 286; };
	CASE_ENUM_TRAIT(E200Z425BN3, SVR_Mapping) { static const unsigned int SPR_NUM = 1023; };
	CASE_ENUM_TRAIT(E200Z710N3, SVR_Mapping) { static const unsigned int SPR_NUM = 1023; };

	// System Version Register
	struct SVR : ReadOnlyPrivilegedSPR<SVR, SVR_Mapping<CONFIG::MODEL>::SPR_NUM>
	{
		typedef ReadOnlyPrivilegedSPR<SVR, SVR_Mapping<CONFIG::MODEL>::SPR_NUM> Super;
		
		struct System_Version : Field<System_Version, 0, 31> {};
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<System_Version> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<System_Version> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		SVR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SVR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("SVR");
			this->SetDescription("System Version Register");
			System_Version::SetName("system_version");
		}
	};
	
	////////////////////////// Device Control Registers ///////////////////////

	// DMEM Control Register 0
	struct DMEMCTL0 : PrivilegedDCR<DMEMCTL0, 496>
	{
		typedef PrivilegedDCR<DMEMCTL0, 496> Super;
		
		struct DMEM_BASE_ADDR : Field<DMEM_BASE_ADDR, 0, 15>  {}; // DMEM BASE ADDRESS Field (CPU Port)
		struct DBYPCB         : Field<DBYPCB        , 19>     {}; // DMEM Bypass Cache Bypass CPU accesses
		struct DBYPAT         : Field<DBYPAT        , 20>     {}; // DMEM Bypass Atomic CPU accesses
		struct DBYPDEC        : Field<DBYPDEC       , 21>     {}; // DMEM Bypass Decorated CPU accesses
		struct DECUE          : Field<DECUE         , 22>     {}; // DMEM Error Correction Update Enable
		struct DBAPD          : Field<DBAPD         , 24>     {}; // DMEM Base Address Port Disable
		struct DPEIE          : Field<DPEIE         , 25>     {}; // DMEM Processor Error Injection Enable
		struct DICWE          : Field<DICWE         , 26>     {}; // DMEM Imprecise CPU Write Enable
		struct DSWCE          : Field<DSWCE         , 27, 28> {}; // DMEM Slave port Write Check/Correct Enable
		struct DDAUEC         : Field<DDAUEC        , 29>     {}; // DMEM Disable Address Use in Error Check
		struct DCPECE         : Field<DCPECE        , 30>     {}; // DMEM CPU Port ECC Enabled (CPU Port)
		struct DSECE          : Field<DSECE         , 31>     {}; // DMEM Slave port Error Checking Enable (Slave port)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DMEM_BASE_ADDR, DBYPCB, DBYPAT, DBYPDEC, DECUE, DBAPD, DPEIE, DICWE, DSWCE, DDAUEC, DCPECE, DSECE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DMEM_BASE_ADDR, DBYPCB, DBYPAT, DBYPDEC, DECUE, DBAPD, DPEIE, DICWE, DSWCE, DDAUEC, DCPECE, DSECE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DMEMCTL0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DMEMCTL0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("DMEMCTL0");                       this->SetDescription("DMEM Control Register 0");
			DMEM_BASE_ADDR::SetName("DMEM_BASE_ADDR"); DMEM_BASE_ADDR::SetDescription("DMEM BASE ADDRESS Field (CPU Port)");
			DBYPCB        ::SetName("DBYPCB");         DBYPCB        ::SetDescription("DMEM Bypass Cache Bypass CPU accesses");
			DBYPAT        ::SetName("DBYPAT");         DBYPAT        ::SetDescription("DMEM Bypass Atomic CPU accesses");
			DBYPDEC       ::SetName("DBYPDEC");        DBYPDEC       ::SetDescription("DMEM Bypass Decorated CPU accesses");
			DECUE         ::SetName("DECUE");          DECUE         ::SetDescription("DMEM Error Correction Update Enable");
			DBAPD         ::SetName("DBAPD");          DBAPD         ::SetDescription("DMEM Base Address Port Disable");
			DPEIE         ::SetName("DPEIE");          DPEIE         ::SetDescription("DMEM Processor Error Injection Enable");
			DICWE         ::SetName("DICWE");          DICWE         ::SetDescription("DMEM Imprecise CPU Write Enable");
			DSWCE         ::SetName("DSWCE");          DSWCE         ::SetDescription("DMEM Slave port Write Check/Correct Enable");
			DDAUEC        ::SetName("DDAUEC");         DDAUEC        ::SetDescription("DMEM Disable Address Use in Error Check");
			DCPECE        ::SetName("DCPECE");         DCPECE        ::SetDescription("DMEM CPU Port ECC Enabled (CPU Port)");
			DSECE         ::SetName("DSECE");          DSECE         ::SetDescription("DMEM Slave port Error Checking Enable (Slave port)");
		}
	};
	
	// DMEM Control Register 1
	struct DMEMCTL1 : PrivilegedDCR<DMEMCTL1, 498>
	{
		typedef PrivilegedDCR<DMEMCTL1, 498> Super;
		
		struct DSWAC3 : Field<DSWAC3, 0, 1>   {}; // DMEM Supervisor Write Access Control for Quadrant 3
		struct DSWAC2 : Field<DSWAC2, 2, 3>   {}; // DMEM Supervisor Write Access Control for Quadrant 2
		struct DSWAC1 : Field<DSWAC1, 4, 5>   {}; // DMEM Supervisor Write Access Control for Quadrant 1
		struct DSWAC0 : Field<DSWAC0, 6, 7>   {}; // DMEM Supervisor Write Access Control for Quadrant 0
		struct DSRAC3 : Field<DSRAC3, 8, 9>   {}; // DMEM Supervisor Read Access Control for Quadrant 3
		struct DSRAC2 : Field<DSRAC2, 10, 11> {}; // DMEM Supervisor Read Access Control for Quadrant 2
		struct DSRAC1 : Field<DSRAC1, 12, 13> {}; // DMEM Supervisor Read Access Control for Quadrant 1
		struct DSRAC0 : Field<DSRAC0, 14, 15> {}; // DMEM Supervisor Read Access Control for Quadrant 0
		struct DUWAC3 : Field<DUWAC3, 16, 17> {}; // DMEM User Write Access Control for Quadrant 3
		struct DUWAC2 : Field<DUWAC2, 18, 19> {}; // DMEM User Write Access Control for Quadrant 2
		struct DUWAC1 : Field<DUWAC1, 20, 21> {}; // DMEM User Write Access Control for Quadrant 1
		struct DUWAC0 : Field<DUWAC0, 22, 23> {}; // DMEM User Write Access Control for Quadrant 0
		struct DURAC3 : Field<DURAC3, 24, 25> {}; // DMEM User Read Access Control for Quadrant 3
		struct DURAC2 : Field<DURAC2, 26, 27> {}; // DMEM User Read Access Control for Quadrant 2
		struct DURAC1 : Field<DURAC1, 28, 29> {}; // DMEM User Read Access Control for Quadrant 1
		struct DURAC0 : Field<DURAC0, 30, 31> {}; // DMEM User Read Access Control for Quadrant 0
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<DSWAC3, DSWAC2, DSWAC1, DSWAC0, DSRAC3, DSRAC2, DSRAC1, DSRAC0, DUWAC3, DUWAC2, DUWAC1, DUWAC0, DURAC3, DURAC2, DURAC1, DURAC0> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<DSWAC3, DSWAC2, DSWAC1, DSWAC0, DSRAC3, DSRAC2, DSRAC1, DSRAC0, DUWAC3, DUWAC2, DUWAC1, DUWAC0, DURAC3, DURAC2, DURAC1, DURAC0> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		DMEMCTL1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		DMEMCTL1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }
		
	private:
		void Init()
		{
			        this->SetName("DMEMCTL1");       this->SetDescription("DMEM Control Register 1");
			DSWAC3::SetName("DSWAC3"); DSWAC3::SetDescription("DMEM Supervisor Write Access Control for Quadrant 3");
			DSWAC2::SetName("DSWAC2"); DSWAC2::SetDescription("DMEM Supervisor Write Access Control for Quadrant 2");
			DSWAC1::SetName("DSWAC1"); DSWAC1::SetDescription("DMEM Supervisor Write Access Control for Quadrant 1");
			DSWAC0::SetName("DSWAC0"); DSWAC0::SetDescription("DMEM Supervisor Write Access Control for Quadrant 0");
			DSRAC3::SetName("DSRAC3"); DSRAC3::SetDescription("DMEM Supervisor Read Access Control for Quadrant 3");
			DSRAC2::SetName("DSRAC2"); DSRAC2::SetDescription("DMEM Supervisor Read Access Control for Quadrant 2");
			DSRAC1::SetName("DSRAC1"); DSRAC1::SetDescription("DMEM Supervisor Read Access Control for Quadrant 1");
			DSRAC0::SetName("DSRAC0"); DSRAC0::SetDescription("DMEM Supervisor Read Access Control for Quadrant 0");
			DUWAC3::SetName("DUWAC3"); DUWAC3::SetDescription("DMEM User Write Access Control for Quadrant 3");
			DUWAC2::SetName("DUWAC2"); DUWAC2::SetDescription("DMEM User Write Access Control for Quadrant 2");
			DUWAC1::SetName("DUWAC1"); DUWAC1::SetDescription("DMEM User Write Access Control for Quadrant 1");
			DUWAC0::SetName("DUWAC0"); DUWAC0::SetDescription("DMEM User Write Access Control for Quadrant 0");
			DURAC3::SetName("DURAC3"); DURAC3::SetDescription("DMEM User Read Access Control for Quadrant 3");
			DURAC2::SetName("DURAC2"); DURAC2::SetDescription("DMEM User Read Access Control for Quadrant 2");
			DURAC1::SetName("DURAC1"); DURAC1::SetDescription("DMEM User Read Access Control for Quadrant 1");
			DURAC0::SetName("DURAC0"); DURAC0::SetDescription("DMEM User Read Access Control for Quadrant 0");
		}
	};
	
	// IMEM Control Register 0
	struct IMEMCTL0 : PrivilegedDCR<IMEMCTL0, 497>
	{
		typedef PrivilegedDCR<IMEMCTL0, 497> Super;
		
		struct IMEM_BASE_ADDR : Field<IMEM_BASE_ADDR, 0, 17>  {}; // IMEM BASE ADDRESS Field (CPU Port)
		struct IECUE          : Field<IECUE         , 22>     {}; // IMEM Error Correction Update Enable
		struct IBAPD          : Field<IBAPD         , 24>     {}; // IMEM Base Address Port Disable
		struct ISWCE          : Field<ISWCE         , 27, 28> {}; // IMEM Slave port Write Check/Correct Enable
		struct IDAUEC         : Field<IDAUEC        , 29>     {}; // IMEM Disable Address Use in Error Check
		struct ICPECE         : Field<ICPECE        , 30>     {}; // IMEM CPU Port ECC Enable (CPU Port)
		struct ISECE          : Field<ISECE         , 31>     {}; // IMEM Slave port Error Checking Enable (Slave port)
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IMEM_BASE_ADDR, IECUE, IBAPD, ISWCE, IDAUEC, ICPECE, ISECE> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IMEM_BASE_ADDR, IECUE, IBAPD, ISWCE, IDAUEC, ICPECE, ISECE> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IMEMCTL0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IMEMCTL0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			                this->SetName("IMEMCTL0");                       this->SetDescription("IMEM Control Register 0");
			IMEM_BASE_ADDR::SetName("IMEM_BASE_ADDR"); IMEM_BASE_ADDR::SetDescription("IMEM BASE ADDRESS Field (CPU Port)");
			IECUE         ::SetName("IECUE");          IECUE         ::SetDescription("IMEM Error Correction Update Enable");
			IBAPD         ::SetName("IBAPD");          IBAPD         ::SetDescription("IMEM Base Address Port Disable");
			ISWCE         ::SetName("ISWCE");          ISWCE         ::SetDescription("IMEM Slave port Write Check/Correct Enable");
			IDAUEC        ::SetName("IDAUEC");         IDAUEC        ::SetDescription("IMEM Disable Address Use in Error Check");
			ICPECE        ::SetName("ICPECE");         ICPECE        ::SetDescription("IMEM CPU Port ECC Enable (CPU Port)");
			ISECE         ::SetName("ISECE");          ISECE         ::SetDescription("IMEM Slave port Error Checking Enable (Slave port)");
		}
	};
	
	// IMEM Control Register 1
	struct IMEMCTL1 : PrivilegedDCR<IMEMCTL1, 499>
	{
		typedef PrivilegedDCR<IMEMCTL1, 499> Super;
		
		struct ISXAC3 : Field<ISXAC3, 8, 9>   {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 3
		struct ISXAC2 : Field<ISXAC2, 10, 11> {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 2
		struct ISXAC1 : Field<ISXAC1, 12, 13> {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 1
		struct ISXAC0 : Field<ISXAC0, 14, 15> {}; // IMEM Supervisor Instruction Fetch Access Control for Quadrant 0
		struct IUXAC3 : Field<IUXAC3, 24, 25> {}; // IMEM User Instruction Fetch Access Control for Quadrant 3
		struct IUXAC2 : Field<IUXAC2, 26, 27> {}; // IMEM User Instruction Fetch Access Control for Quadrant 2
		struct IUXAC1 : Field<IUXAC1, 28, 29> {}; // IMEM User Instruction Fetch Access Control for Quadrant 1
		struct IUXAC0 : Field<IUXAC0, 30, 31> {}; // IMEM User Instruction Fetch Access Control for Quadrant 0
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<ISXAC3, ISXAC2, ISXAC1, ISXAC0, IUXAC3, IUXAC2, IUXAC1, IUXAC0> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<ISXAC3, ISXAC2, ISXAC1, ISXAC0, IUXAC3, IUXAC2, IUXAC1, IUXAC0> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		IMEMCTL1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		IMEMCTL1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
		
		virtual void Reset() { this->Initialize(0x0); }

	private:
		void Init()
		{
			        this->SetName("IMEMCTL1");       this->SetDescription("IMEM Control Register 1");
			ISXAC3::SetName("ISXAC3"); ISXAC3::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 3");
			ISXAC2::SetName("ISXAC2"); ISXAC2::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 2");
			ISXAC1::SetName("ISXAC1"); ISXAC1::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 1");
			ISXAC0::SetName("ISXAC0"); ISXAC0::SetDescription("IMEM Supervisor Instruction Fetch Access Control for Quadrant 0");
			IUXAC3::SetName("IUXAC3"); IUXAC3::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 3");
			IUXAC2::SetName("IUXAC2"); IUXAC2::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 2");
			IUXAC1::SetName("IUXAC1"); IUXAC1::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 1");
			IUXAC0::SetName("IUXAC0"); IUXAC0::SetDescription("IMEM User Instruction Fetch Access Control for Quadrant 0");
		}
	};
	
	// End-to-End ECC Control Register 0
	struct E2ECTL0 : PrivilegedDCR<E2ECTL0, 510>
	{
		typedef PrivilegedDCR<E2ECTL0, 510> Super;
		
		struct IDAC   : Field<IDAC  , 28> {}; // Instruction Disable Address Checking
		struct IECCEN : Field<IECCEN, 29> {}; // Instruction ECC Enable Field
		struct DDAC   : Field<DDAC  , 30> {}; // Data Disable Address Checking
		struct DECCEN : Field<DECCEN, 31> {}; // Data ECC Enable Field
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<IDAC, IECCEN, DDAC, DECCEN> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<IDAC, IECCEN, DDAC, DECCEN> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		E2ECTL0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		E2ECTL0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			        this->SetName("E2ECTL0");        this->SetDescription("End-to-End ECC Control Register 0");
			IDAC  ::SetName("IDAC");   IDAC  ::SetDescription("Instruction Disable Address Checking");
			IECCEN::SetName("IECCEN"); IECCEN::SetDescription("Instruction ECC Enable Field");
			DDAC  ::SetName("DDAC");   DDAC  ::SetDescription("Data Disable Address Checking");
			DECCEN::SetName("DECCEN"); DECCEN::SetDescription("Data ECC Enable Field");
		}
	};
	
	// End-to-End ECC Error Control/Status Register 0
	struct E2EECSR0 : PrivilegedDCR<E2EECSR0, 511>
	{
		typedef PrivilegedDCR<E2EECSR0, 511> Super;
		
		struct RCHKBIT         : Field<RCHKBIT        , 4, 11>  {}; // Read Checkbits
		struct WRC             : Field<WRC            , 16, 17> {}; // Write Control
		struct INVC            : Field<INVC           , 18, 19> {}; // Invert Control
		struct WCHKBIT_CHKINVT : Field<WCHKBIT_CHKINVT, 24, 31> {}; // Write Checkbits / Checkbit Invert Mask
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<RCHKBIT, WRC, INVC, WCHKBIT_CHKINVT> ALL; };
		CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<RCHKBIT, WRC, INVC, WCHKBIT_CHKINVT> ALL; };
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
		
		E2EECSR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		E2EECSR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			           this->SetName("E2EECSR0");                          this->SetDescription("End-to-End ECC Error Control/Status Register 0");
			RCHKBIT        ::SetName("RCHKBIT");                RCHKBIT        ::SetDescription("Read Checkbits");
			WRC            ::SetName("WRC");                    WRC            ::SetDescription("Write Control");
			INVC           ::SetName("INVC");                   INVC           ::SetDescription("Invert Control");
			WCHKBIT_CHKINVT::SetName("WCHKBIT_CHKINVT");        WCHKBIT_CHKINVT::SetDescription("Write Checkbits / Checkbit Invert Mask");
			WCHKBIT_CHKINVT::SetDisplayName("WCHKBIT/CHKINVT"); 
		}
	};

	// Nexus 3 DCRs are voluntary missing
	
	//  Performance Monitor Counter registers 1-6
	SWITCH_ENUM_TRAIT(unsigned int, PMC_Mapping);
	CASE_ENUM_TRAIT(1, PMC_Mapping) { static const unsigned int SPR_NUM = 953; };
	CASE_ENUM_TRAIT(2, PMC_Mapping) { static const unsigned int SPR_NUM = 954; };
	CASE_ENUM_TRAIT(3, PMC_Mapping) { static const unsigned int SPR_NUM = 957; };
	CASE_ENUM_TRAIT(4, PMC_Mapping) { static const unsigned int SPR_NUM = 958; };
	CASE_ENUM_TRAIT(5, PMC_Mapping) { static const unsigned int SPR_NUM = 945; };
	CASE_ENUM_TRAIT(6, PMC_Mapping) { static const unsigned int SPR_NUM = 946; };
	
	template <unsigned int PMC_NUM>
	struct PMC : PrivilegedSPR<PMC<PMC_NUM>, PMC_Mapping<PMC_NUM>::SPR_NUM>
	{
		typedef PrivilegedSPR<PMC<PMC_NUM>, PMC_Mapping<PMC_NUM>::SPR_NUM> Super;
		
		struct OV            : Field<OV           , 0> {};      // Overflow
		struct Counter_Value : Field<Counter_Value, 1, 31> {};  // Indicates the number of occurrences of the specified event
		
		typedef FieldSet<OV, Counter_Value> ALL;
		
		PMC(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PMC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			std::stringstream name_sstr;
			name_sstr << "PMC" << PMC_NUM;
			
			std::stringstream desc_sstr;
			desc_sstr << "Performance Monitor Counter register" << PMC_NUM;
			
						this->SetName(name_sstr.str());                         this->SetDescription(desc_sstr.str());
			OV           ::SetName("OV");            OV           ::SetDescription("Overflow");
			Counter_Value::SetName("Counter_Value"); Counter_Value::SetDescription("Indicates the number of occurrences of the specified event");
		}
	};
	
	typedef PMC<1> PMC1;
	typedef PMC<2> PMC2;
	typedef PMC<3> PMC3;
	typedef PMC<4> PMC4;
	typedef PMC<5> PMC5;
	typedef PMC<6> PMC6;

	// User Performance Monitor Counter registers 1-6
	SWITCH_ENUM_TRAIT(unsigned int, UPMC_Mapping);
	CASE_ENUM_TRAIT(1, UPMC_Mapping) { static const unsigned int SPR_NUM = 937; };
	CASE_ENUM_TRAIT(2, UPMC_Mapping) { static const unsigned int SPR_NUM = 938; };
	CASE_ENUM_TRAIT(3, UPMC_Mapping) { static const unsigned int SPR_NUM = 941; };
	CASE_ENUM_TRAIT(4, UPMC_Mapping) { static const unsigned int SPR_NUM = 942; };
	CASE_ENUM_TRAIT(5, UPMC_Mapping) { static const unsigned int SPR_NUM = 929; };
	CASE_ENUM_TRAIT(6, UPMC_Mapping) { static const unsigned int SPR_NUM = 930; };
	
	template <unsigned int PMC_NUM>
	struct UPMC : ReadOnlyUSPR<PMC<PMC_NUM>, UPMC_Mapping<PMC_NUM>::SPR_NUM>
	{
		typedef ReadOnlyUSPR<PMC<PMC_NUM>, UPMC_Mapping<PMC_NUM>::SPR_NUM> Super;
		
		UPMC(typename CONFIG::CPU *_cpu, PMC<PMC_NUM> *pmc) : Super(_cpu, pmc) {}
	};
	
	typedef UPMC<1> UPMC1;
	typedef UPMC<2> UPMC2;
	typedef UPMC<3> UPMC3;
	typedef UPMC<4> UPMC4;
	typedef UPMC<5> UPMC5;
	typedef UPMC<6> UPMC6;
	
	// Monitor Mode Control Register 0
	struct MMCR0 : PrivilegedSPR<MMCR0, 952>
	{
		typedef PrivilegedSPR<MMCR0, 952> Super;
		
		struct FC        : Field<FC       , 0     > {}; // Freeze counters
		struct FCS       : Field<FCS      , 1     > {}; // Freeze counters in supervisor mode
		struct FCP       : Field<FCP      , 2     > {}; // Freeze counters in user mode
		struct FCM1      : Field<FCM1     , 3     > {}; // Freeze counters while mark=1
		struct FCM0      : Field<FCM0     , 4     > {}; // Freeze counters while mark=0
		struct PMXE      : Field<PMXE     , 5     > {}; // Performance monitor exception enable
		struct FCECE     : Field<FCECE    , 6     > {}; // Freeze counters on enabled condition or event
		struct TBSEL     : Field<TBSEL    , 7 , 8 > {}; // Time base selector
		struct TBEE      : Field<TBEE     , 9     > {}; // Time base event enable
		struct THRESHOLD : Field<THRESHOLD, 10, 15> {}; // Threshold
		struct PMC1CE    : Field<PMC1CE   , 16    > {}; // PMC1 condition enable
		struct PMCnCE    : Field<PMCnCE   , 17    > {}; // PMCn condition enable
		struct TRIGGER   : Field<TRIGGER  , 18    > {}; // Trigger
		struct PMC1SEL   : Field<PMC1SEL  , 19, 25> {}; // PMC1 selector
		struct PMC2SEL   : Field<PMC2SEL  , 26, 31> {}; // PMC2 selector
		
		typedef FieldSet<FC, FCS, FCP, FCM1, FCM0, PMXE, FCECE, TBSEL, TBEE, THRESHOLD, PMC1CE, PMCnCE, TRIGGER, PMC1SEL, PMC2SEL> ALL;
	
		MMCR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MMCR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("MMCR0"); this->SetDescription("Monitor Mode Control Register 0");
			
			FC       ::SetName("FC");        FC       ::SetDescription("Freeze counters");
			FCS      ::SetName("FCS");       FCS      ::SetDescription("Freeze counters in supervisor mode");
			FCP      ::SetName("FCP");       FCP      ::SetDescription("Freeze counters in user mode");
			FCM1     ::SetName("FCM1");      FCM1     ::SetDescription("Freeze counters while mark=1");
			FCM0     ::SetName("FCM0");      FCM0     ::SetDescription("Freeze counters while mark=0");
			PMXE     ::SetName("PMXE");      PMXE     ::SetDescription("Performance monitor exception enable");
			FCECE    ::SetName("FCECE");     FCECE    ::SetDescription("Freeze counters on enabled condition or event");
			TBSEL    ::SetName("TBSEL");     TBSEL    ::SetDescription("Time base selector");
			TBEE     ::SetName("TBEE");      TBEE     ::SetDescription("Time base event enable");
			THRESHOLD::SetName("THRESHOLD"); THRESHOLD::SetDescription("Threshold");
			PMC1CE   ::SetName("PMC1CE");    PMC1CE   ::SetDescription("PMC1 condition enable");
			PMCnCE   ::SetName("PMCnCE");    PMCnCE   ::SetDescription("PMCn condition enable");
			TRIGGER  ::SetName("TRIGGER");   TRIGGER  ::SetDescription("Trigger");
			PMC1SEL  ::SetName("PMC1SEL");   PMC1SEL  ::SetDescription("PMC1 selector");
			PMC2SEL  ::SetName("PMC2SEL");   PMC2SEL  ::SetDescription("PMC2 selector");
		}
	};
	
	// Monitor Mode Control Register 1
	struct MMCR1 : PrivilegedSPR<MMCR1, 956>
	{
		typedef PrivilegedSPR<MMCR1, 956> Super;
		
		struct PMC3SELECT : Field<PMC3SELECT, 0 , 4 > {}; // PMC3 selector
		struct PMC4SELECT : Field<PMC4SELECT, 5 , 9 > {}; // PMC4 selector
		struct PMC5SELECT : Field<PMC5SELECT, 10, 14> {}; // PMC5 selector
		struct PMC6SELECT : Field<PMC6SELECT, 15, 20> {}; // PMC6 selector
		
		typedef FieldSet<PMC3SELECT, PMC4SELECT, PMC5SELECT, PMC6SELECT> ALL;
		
		MMCR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MMCR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("MMCR1"); this->SetDescription("Monitor Mode Control Register 1");
			
			PMC3SELECT::SetName("PMC3SELECT"); PMC3SELECT::SetDescription("PMC3 selector");
			PMC4SELECT::SetName("PMC4SELECT"); PMC4SELECT::SetDescription("PMC4 selector");
			PMC5SELECT::SetName("PMC5SELECT"); PMC5SELECT::SetDescription("PMC5 selector");
			PMC6SELECT::SetName("PMC6SELECT"); PMC6SELECT::SetDescription("PMC6 selector");
		}
	};
	
	// Monitor Mode Control Register 2
	struct MMCR2 : PrivilegedSPR<MMCR2, 944>
	{
		typedef PrivilegedSPR<MMCR2, 944> Super;
		
		struct THRESHMULT : Field<THRESHMULT, 0> {}; // Threshold multiplier
		
		typedef FieldSet<THRESHMULT> ALL;
		
		MMCR2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		MMCR2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("MMCR2"); this->SetDescription("Monitor Mode Control Register 2");
			
			THRESHMULT::SetName("THRESHMULT"); THRESHMULT::SetDescription("Threshold multiplier");
		}
	};
	
	// User Monitor Control Register 0
	struct UMMCR0 : ReadOnlyUSPR<MMCR0, 936>
	{
		typedef ReadOnlyUSPR<MMCR0, 936> Super;
		
		UMMCR0(typename CONFIG::CPU *_cpu, MMCR0 *mmcr0) : Super(_cpu, mmcr0) {}
	};
	
	// User Monitor Control Register 1
	struct UMMCR1 : ReadOnlyUSPR<MMCR1, 940>
	{
		typedef ReadOnlyUSPR<MMCR1, 940> Super;
		
		UMMCR1(typename CONFIG::CPU *_cpu, MMCR1 *mmcr1) : Super(_cpu, mmcr1) {}
	};
	
	// User Monitor Control Register 2
	struct UMMCR2 : ReadOnlyUSPR<MMCR2, 928>
	{
		typedef ReadOnlyUSPR<MMCR2, 928> Super;
		
		UMMCR2(typename CONFIG::CPU *_cpu, MMCR2 *mmcr2) : Super(_cpu, mmcr2) {}
	};
	
	// Sampled Instruction Address Register
	struct SIAR : PrivilegedSPR<SIAR, 955>
	{
		typedef PrivilegedSPR<SIAR, 955> Super;
		
		struct Instruction_Address : Field<Instruction_Address, 0, 31> {}; // Instruction Address
		
		typedef FieldSet<Instruction_Address> ALL;
		
		SIAR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		SIAR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("SIAR"); this->SetDescription("Sampled Instruction Address Register");
			
			Instruction_Address::SetName("Instruction_Address"); Instruction_Address::SetDescription("Instruction Address");
		}
	};
	
	// User Sampled Instruction Address Register
	struct USIAR : ReadOnlyUSPR<SIAR, 939>
	{
		typedef ReadOnlyUSPR<SIAR, 939> Super;
		
		USIAR(typename CONFIG::CPU *_cpu, SIAR *siar) : Super(_cpu, siar) {}
	};
	
	// Breakpoint Address Mask Register
	struct BAMR : PrivilegedSPR<BAMR, 951>
	{
		typedef PrivilegedSPR<BAMR, 951> Super;
		
		struct MASK : Field<MASK, 0, 29> {}; // Breakpoint Address Mask
		
		typedef FieldSet<MASK> ALL;
		
		BAMR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		BAMR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("BAMR"); this->SetDescription("Breakpoint Address Mask Register");
			
			MASK::SetName("MASK"); MASK::SetDescription("Breakpoint Address Mask");
		}
	};
	
	// TLB Miss Register
	struct TLBMISS : PrivilegedSPR<TLBMISS, 980>
	{
		typedef PrivilegedSPR<TLBMISS, 980> Super;
		
		struct PAGE : Field<PAGE, 0 , 30, unisim::util::reg::core::SW_R> {}; // Effective page address
		struct LRU  : Field<LRU , 31, 31, unisim::util::reg::core::SW_R> {}; // Least recently used way of the addressed TLB set
		
		typedef FieldSet<PAGE, LRU> ALL;
		
		TLBMISS(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		TLBMISS(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("TLBMISS"); this->SetDescription("TLB Miss Register");
			
			PAGE::SetName("PAGE"); PAGE::SetDescription("Effective page address");
			LRU ::SetName("LRU "); LRU ::SetDescription("Least recently used way of the addressed TLB set");
		}
	};
	
	// Page Table Entry High Register 
	struct PTEHI : PrivilegedSPR<PTEHI, 981>
	{
		typedef PrivilegedSPR<PTEHI, 981> Super;
		
		struct V    : Field<V   , 0     > {}; // Entry valid
		struct VSID : Field<VSID, 1 , 24> {}; // Virtual segment ID
		struct API  : Field<API , 26, 31> {}; // Abbreviated page index
		
		typedef FieldSet<V, VSID, API> ALL;
		
		PTEHI(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PTEHI(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("PTEHI"); this->SetDescription("Page Table Entry High Register");
			
			V   ::SetName("V");    V   ::SetDescription("Entry valid");
			VSID::SetName("VSID"); VSID::SetDescription("Virtual segment ID");
			API ::SetName("API");  API ::SetDescription("Abbreviated page index");
		}
	};
	
	// Page Table Entry Low Register 
	struct PTELO : PrivilegedSPR<PTELO, 982>
	{
		typedef PrivilegedSPR<PTELO, 982> Super;
		
		struct RPN  : Field<RPN , 0 , 19> {}; // Physical page number
		struct XPN  : Field<XPN , 20, 22> {}; // Extended page number
		struct C    : Field<C   , 24    > {}; // Changed bit
		struct WIMG : Field<WIMG, 25, 28> {}; // Memory/cache control bits
		struct W    : Field<W   , 25    > {}; // Write-Through
		struct I    : Field<I   , 26    > {}; // Caching-Inhibited
		struct M    : Field<M   , 27    > {}; // Memory coherence
		struct G    : Field<G   , 28    > {}; // Guarded
		struct X    : Field<X   , 29    > {}; // Extended page number
		struct PP   : Field<PP  , 30, 31> {}; // Page protection bits
		
		typedef FieldSet<RPN, XPN, C, WIMG, X, PP> ALL;
		
		PTELO(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		PTELO(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("PTELO"); this->SetDescription("Page Table Entry Low Register");
			
			RPN ::SetName("RPN");  RPN ::SetDescription("Physical page number");
			XPN ::SetName("XPN");  XPN ::SetDescription("Extended page number");
			C   ::SetName("C");    C   ::SetDescription("Changed bit");
			WIMG::SetName("WIMG"); WIMG::SetDescription("Memory/cache control bits");
			X   ::SetName("X");    X   ::SetDescription("Extended page number");
			PP  ::SetName("PP");   PP  ::SetDescription("Page protection bits");
		}
	};
	
	// L3 Private Memory Address Register
	struct L3PM : PrivilegedSPR<L3PM, 983>
	{
		typedef PrivilegedSPR<L3PM, 983> Super;
		
		struct L3PMADDR : Field<L3PMADDR, 0, 15> {}; // L3 base address of L3 private memory
		
		typedef FieldSet<L3PMADDR> ALL;

		L3PM(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3PM(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L3PM"); this->SetDescription("L3 Private Memory Address Register");
			
			L3PMADDR::SetName("L3PMADDR"); L3PMADDR::SetDescription("L3 base address of L3 private memory");
		}
	};
	
	// L3 Cache Input Timing Control 0
	struct L3ITCR0 : PrivilegedSPR<L3ITCR0, 984>
	{
		typedef PrivilegedSPR<L3ITCR0, 984> Super;
		
		SWITCH_ENUM_TRAIT(Model, _);
		CASE_ENUM_TRAIT(MPC7451, _)
		{
			struct L3DC0    : Field<L3DC0   , 0, 22> {};
			struct L3DCDIS0 : Field<L3DCDIS0, 23   > {};
			struct L3DCO0   : Field<L3DCO0  , 24   > {};
		};
		CASE_ENUM_TRAIT(MPC7455, _)
		{
			struct L3DC0    : Field<L3DC0   , 0, 22> {};
			struct L3DCDIS0 : Field<L3DCDIS0, 23   > {};
			struct L3DCO0   : Field<L3DCO0  , 24   > {};
		};
		CASE_ENUM_TRAIT(MPC7457, _)
		{
			struct L3DC0    : Field<L3DC0   , 0, 29> {};
			struct L3DCDIS0 : Field<L3DCDIS0, 30   > {};
			struct L3DCO0   : Field<L3DCO0  , 31   > {};
		};
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::L3DC0    L3DC0;    // L3 delay count
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::L3DCDIS0 L3DCDIS0; // L3 delay counter disable
		typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::L3DCO0   L3DCO0;   // L3 delay counter override
		
		typedef FieldSet<L3DC0, L3DCDIS0, L3DCO0> ALL;
		
		L3ITCR0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3ITCR0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L3ITCR0"); this->SetDescription("L3 Cache Input Timing Control 0");
			
			L3DC0   ::SetName("L3DC0");    L3DC0   ::SetDescription("L3 delay count");
			L3DCDIS0::SetName("L3DCDIS0"); L3DCDIS0::SetDescription("L3 delay counter disable");
			L3DCO0  ::SetName("L3DCO0");   L3DCO0  ::SetDescription("L3 delay counter override");
		}
	};
	
	// L2 Error Injection Mask High Register
	struct L2ERRINJHI : PrivilegedSPR<L2ERRINJHI, 985>
	{
		typedef PrivilegedSPR<L2ERRINJHI, 985> Super;
		
		struct EIMASKHI : Field<EIMASKHI, 0, 31> {}; // Error injection mask for the high word of the data path
		
		typedef FieldSet<EIMASKHI> ALL;
		
		L2ERRINJHI(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRINJHI(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRINJHI"); this->SetDescription("L2 Error Injection Mask High Register");
			
			EIMASKHI::SetName("EIMASKHI"); EIMASKHI::SetDescription("Error injection mask for the high word of the data path");
		}
	};
	
	// L2 Error Injection Mask High Register
	struct L2ERRINJLO : PrivilegedSPR<L2ERRINJLO, 986>
	{
		typedef PrivilegedSPR<L2ERRINJLO, 986> Super;
		
		struct EIMASKLO : Field<EIMASKLO, 0, 31> {}; // Error injection mask for the low word of the data path
		
		typedef FieldSet<EIMASKLO> ALL;
		
		L2ERRINJLO(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRINJLO(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRINJLO"); this->SetDescription("L2 Error Injection Mask High Register");
			
			EIMASKLO::SetName("EIMASKLO"); EIMASKLO::SetDescription("Error injection mask for the low word of the data path");
		}
	};
	
	// L2 Error Injection Mask Control Register
	struct L2ERRINJCTL : PrivilegedSPR<L2ERRINJCTL, 987>
	{
		typedef PrivilegedSPR<L2ERRINJCTL, 987> Super;
		
		struct TERRIEN  : Field<TERRIEN , 15    > {}; // L2 tag error injection enable
		struct ECCMB    : Field<ECCMB   , 22    > {}; // ECC mirror byte enable
		struct DERRIEN  : Field<DERRIEN , 23    > {}; // L2 data array error injection enable
		struct ECCERRIM : Field<ECCERRIM, 24, 31> {}; // Error injection mask for the ECC bits
		
		typedef FieldSet<TERRIEN, ECCMB, DERRIEN, ECCERRIM> ALL;
	
		L2ERRINJCTL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRINJCTL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRINJCTL"); this->SetDescription("L2 Error Injection Mask Control Register");
			
			TERRIEN ::SetName("TERRIEN");  TERRIEN ::SetDescription("L2 tag error injection enable");
			ECCMB   ::SetName("ECCMB");    ECCMB   ::SetDescription("ECC mirror byte enable");
			DERRIEN ::SetName("DERRIEN");  DERRIEN ::SetDescription("L2 data array error injection enable");
			ECCERRIM::SetName("ECCERRIM"); ECCERRIM::SetDescription("Error injection mask for the ECC bits");
		}
	};
	
	// L2 Error Capture Data High Register
	struct L2CAPTDATAHI : ReadOnlyPrivilegedSPR<L2CAPTDATAHI, 988>
	{
		typedef ReadOnlyPrivilegedSPR<L2CAPTDATAHI, 988> Super;
		
		struct L2DATA : Field<L2DATA, 0, 31> {}; // L2 data high word
		
		typedef FieldSet<L2DATA> ALL;
		
		L2CAPTDATAHI(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2CAPTDATAHI(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2CAPTDATAHI"); this->SetDescription("L2 Error Capture Data High Register");
			
			L2DATA::SetName("L2DATA"); L2DATA::SetDescription("L2 data high word");
		}
	};
	
	// L2 Error Capture Data Low Register
	struct L2CAPTDATALO : ReadOnlyPrivilegedSPR<L2CAPTDATALO, 989>
	{
		typedef ReadOnlyPrivilegedSPR<L2CAPTDATALO, 989> Super;
		
		struct L2DATA : Field<L2DATA, 0, 31> {}; // L2 data low word
		
		typedef FieldSet<L2DATA> ALL;

		L2CAPTDATALO(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2CAPTDATALO(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2CAPTDATALO"); this->SetDescription("L2 Error Capture Data Low Register");
			
			L2DATA::SetName("L2DATA"); L2DATA::SetDescription("L2 data low word");
		}
	};
	
	// L2 Error Syndrome Register
	struct L2CAPTECC : ReadOnlyPrivilegedSPR<L2CAPTECC, 990>
	{
		typedef ReadOnlyPrivilegedSPR<L2CAPTECC, 990> Super;
		
		struct ECCSYND   : Field<ECCSYND  , 0 , 7 > {}; // The calculted ECC syndrome of the failing double word
		struct ECCCHKSUM : Field<ECCCHKSUM, 24, 31> {}; // The datapath ECC of the failing double word
		
		typedef FieldSet<ECCSYND, ECCCHKSUM> ALL;
		
		L2CAPTECC(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2CAPTECC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2CAPTECC"); this->SetDescription("L2 Error Syndrome Register");
			
			ECCSYND  ::SetName("ECCSYND  "); ECCSYND  ::SetDescription("The calculted ECC syndrome of the failing double word");
			ECCCHKSUM::SetName("ECCCHKSUM"); ECCCHKSUM::SetDescription("The datapath ECC of the failing double word");
		}
	};
	
	// L2 Error Detect Register
	struct L2ERRDET : PrivilegedSPR<L2ERRDET, 991>
	{
		typedef PrivilegedSPR<L2ERRDET, 991> Super;
		
		struct MULL2ERR : Field<MULL2ERR, 0 , 0 , unisim::util::reg::core::SW_R_W1C> {}; // Multiple L2 errors
		struct TPARERR  : Field<TPARERR , 27, 27, unisim::util::reg::core::SW_R_W1C> {}; // Tag parity error
		struct MBECCERR : Field<MBECCERR, 28, 28, unisim::util::reg::core::SW_R_W1C> {}; // Multiple-bit ECC error
		struct SBECCERR : Field<SBECCERR, 29, 29, unisim::util::reg::core::SW_R_W1C> {}; // Single-bit ECC error
		
		typedef FieldSet<MULL2ERR, TPARERR, MBECCERR, SBECCERR> ALL;
		
		L2ERRDET(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRDET(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRDET"); this->SetDescription("L2 Error Detect Register");
			
			MULL2ERR::SetName("MULL2ERR"); MULL2ERR::SetDescription("Multiple L2 errors");
			TPARERR ::SetName("TPARERR");  TPARERR ::SetDescription("Tag parity error");
			MBECCERR::SetName("MBECCERR"); MBECCERR::SetDescription("Multiple-bit ECC error");
			SBECCERR::SetName("SBECCERR"); SBECCERR::SetDescription("Single-bit ECC error");
		}
	};
	
	// L2 Error Disable Register
	struct L2ERRDIS : PrivilegedSPR<L2ERRDIS, 992>
	{
		typedef PrivilegedSPR<L2ERRDIS, 992> Super;
		
		struct TPARDIS  : Field<TPARDIS , 27> {}; // Tag parity error disable
		struct MBECCDIS : Field<MBECCDIS, 28> {}; // Multiple-bit ECC error disable
		struct SBECCDIS : Field<SBECCDIS, 29> {}; // Single-bit ECC error disable
		
		typedef FieldSet<TPARDIS, MBECCDIS, SBECCDIS> ALL;
		
		L2ERRDIS(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRDIS(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRDIS"); this->SetDescription("L2 Error Disable Register");
			
			TPARDIS ::SetName("TPARDIS");  TPARDIS ::SetDescription("Tag parity error disable");
			MBECCDIS::SetName("MBECCDIS"); MBECCDIS::SetDescription("Multiple-bit ECC error disable");
			SBECCDIS::SetName("SBECCDIS"); SBECCDIS::SetDescription("Single-bit ECC error disable");
		}
	};
	
	// L2 Error Interrupt Enable Register
	struct L2ERRINTEN : PrivilegedSPR<L2ERRINTEN, 993>
	{
		typedef PrivilegedSPR<L2ERRINTEN, 993> Super;
		
		struct TPARINTEN  : Field<TPARINTEN , 27> {}; // Tag parity error reporting enable
		struct MBECCINTEN : Field<MBECCINTEN, 28> {}; // Multiple-bit ECC error reporting enable
		struct SBECCINTEN : Field<SBECCINTEN, 29> {}; // Single-bit ECC error enable
		
		typedef FieldSet<TPARINTEN, MBECCINTEN, SBECCINTEN> ALL;
		
		L2ERRINTEN(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRINTEN(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRINTEN"); this->SetDescription("L2 Error Interrupt Enable Register");
			
			TPARINTEN ::SetName("TPARINTEN");  TPARINTEN ::SetDescription("Tag parity error reporting enable");
			MBECCINTEN::SetName("MBECCINTEN"); MBECCINTEN::SetDescription("Multiple-bit ECC error reporting enable");
			SBECCINTEN::SetName("SBECCINTEN"); SBECCINTEN::SetDescription("Single-bit ECC error enable");
		}
	};
	
	// L2 Error Attributes Capture Register
	struct L2ERRATTR : PrivilegedSPR<L2ERRATTR, 994>
	{
		typedef PrivilegedSPR<L2ERRATTR, 994> Super;
		
		struct DWNUM     : Field<DWNUM    , 2 , 3 , unisim::util::reg::core::SW_R> {}; // Double-word number of the detected error
		struct TRANSSIZ  : Field<TRANSSIZ , 5 , 7 , unisim::util::reg::core::SW_R> {}; // Transaction size for detected error
		struct BURST     : Field<BURST    , 8 , 8 , unisim::util::reg::core::SW_R> {}; // Burst transaction for detected error
		struct TRANSSRC  : Field<TRANSSRC , 11, 15, unisim::util::reg::core::SW_R> {}; // Transaction source for detected error
		struct TRANSTYPE : Field<TRANSTYPE, 18, 19, unisim::util::reg::core::SW_R> {}; // Transaction type for detected error
		struct VALINFO   : Field<VALINFO  , 31, 31, unisim::util::reg::core::SW_R> {}; // L2 capture registers valid
		
		typedef FieldSet<DWNUM, TRANSSIZ, BURST, TRANSSRC, TRANSTYPE, VALINFO> ALL;
		
		L2ERRATTR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRATTR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRATTR"); this->SetDescription("L2 Error Attributes Capture Register");
			
			DWNUM    ::SetName("DWNUM");     DWNUM    ::SetDescription("Double-word number of the detected error");
			TRANSSIZ ::SetName("TRANSSIZ");  TRANSSIZ ::SetDescription("Transaction size for detected error");
			BURST    ::SetName("BURST");     BURST    ::SetDescription("Burst transaction for detected error");
			TRANSSRC ::SetName("TRANSSRC");  TRANSSRC ::SetDescription("Transaction source for detected error");
			TRANSTYPE::SetName("TRANSTYPE"); TRANSTYPE::SetDescription("Transaction type for detected error");
			VALINFO  ::SetName("VALINFO");   VALINFO  ::SetDescription("L2 capture registers valid");
		}
	};
	
	// L2 Error Address Error Capture Register
	struct L2ERRADDR : ReadOnlyPrivilegedSPR<L2ERRADDR, 995>
	{
		typedef ReadOnlyPrivilegedSPR<L2ERRADDR, 995> Super;
		
		struct L2ADDR : Field<L2ADDR, 0, 31, unisim::util::reg::core::SW_R> {}; // L2 address[4:35] corresponding to detected error
		
		typedef FieldSet<L2ADDR> ALL;
		
		L2ERRADDR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRADDR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRADDR"); this->SetDescription("L2 Error Address Error Capture Register");
			
			L2ADDR::SetName("L2ADDR"); L2ADDR::SetDescription("L2 address[4:35] corresponding to detected error");
		}
	};
	
	// L2 Error Address Error Capture Register
	struct L2ERREADDR : ReadOnlyPrivilegedSPR<L2ERREADDR, 996>
	{
		typedef ReadOnlyPrivilegedSPR<L2ERREADDR, 996> Super;
		
		struct L2ADDR : Field<L2ADDR, 28, 31, unisim::util::reg::core::SW_R> {}; // L2 address[0:3] corresponding to detected error
		
		typedef FieldSet<L2ADDR> ALL;
		
		L2ERREADDR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERREADDR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERREADDR"); this->SetDescription("L2 Error Address Error Capture Register");
			
			L2ADDR::SetName("L2ADDR"); L2ADDR::SetDescription("L2 address[0:3] corresponding to detected error");
		}
	};
	
	// L2 Error Control Register
	struct L2ERRCTL : PrivilegedSPR<L2ERRCTL, 997>
	{
		typedef PrivilegedSPR<L2ERRCTL, 997> Super;
		
		struct L2CTHRESH : Field<L2CTHRESH, 8 , 15, unisim::util::reg::core::SW_R> {}; // L2 cache Threshold
		struct L2CCOUNT  : Field<L2CCOUNT , 24, 31                               > {}; // L2 count
		
		typedef FieldSet<L2CTHRESH, L2CCOUNT> ALL;
		
		L2ERRCTL(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L2ERRCTL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L2ERRCTL"); this->SetDescription("L2 Error Control Register");
			
			L2CTHRESH::SetName("L2CTHRESH"); L2CTHRESH::SetDescription("L2 cache Threshold");
			L2CCOUNT ::SetName("L2CCOUNT");  L2CCOUNT ::SetDescription("L2 count");
		}
	};
	
	// L3 Cache Input Timing Control 1
	struct L3ITCR1 : PrivilegedSPR<L3ITCR1, 1001>
	{
		typedef PrivilegedSPR<L3ITCR1, 1001> Super;
		
		struct L3DC1    : Field<L3DC1   , 0 , 29> {}; // L3 delay count
		struct L3DCDIS1 : Field<L3DCDIS1, 30    > {}; // L3 delay counter disable
		struct L3DCO1   : Field<L3DCO1  , 31    > {}; // L3 delay counter override
		
		typedef FieldSet<L3DC1, L3DCDIS1, L3DCO1> ALL;
		
		L3ITCR1(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3ITCR1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L3ITCR1"); this->SetDescription("L3 Cache Input Timing Control 1");
			
			L3DC1   ::SetName("L3DC1");    L3DC1   ::SetDescription("L3 delay count");
			L3DCDIS1::SetName("L3DCDIS1"); L3DCDIS1::SetDescription("L3 delay counter disable");
			L3DCO1  ::SetName("L3DCO1");   L3DCO1  ::SetDescription("L3 delay counter override");
		}
	};

	// L3 Cache Input Timing Control 2
	struct L3ITCR2 : PrivilegedSPR<L3ITCR2, 1002>
	{
		typedef PrivilegedSPR<L3ITCR2, 1001> Super;
		
		struct L3DC2    : Field<L3DC2   , 0 , 29> {}; // L3 delay count
		struct L3DCDIS2 : Field<L3DCDIS2, 30    > {}; // L3 delay counter disable
		struct L3DCO2   : Field<L3DCO2  , 31    > {}; // L3 delay counter override
		
		typedef FieldSet<L3DC2, L3DCDIS2, L3DCO2> ALL;
		
		L3ITCR2(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3ITCR2(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L3ITCR2"); this->SetDescription("L3 Cache Input Timing Control 2");
			
			L3DC2   ::SetName("L3DC2");    L3DC2   ::SetDescription("L3 delay count");
			L3DCDIS2::SetName("L3DCDIS2"); L3DCDIS2::SetDescription("L3 delay counter disable");
			L3DCO2  ::SetName("L3DCO2");   L3DCO2  ::SetDescription("L3 delay counter override");
		}
	};

	// L3 Cache Input Timing Control 3
	struct L3ITCR3 : PrivilegedSPR<L3ITCR3, 1003>
	{
		typedef PrivilegedSPR<L3ITCR3, 1001> Super;
		
		struct L3DC3    : Field<L3DC3   , 0 , 29> {}; // L3 delay count
		struct L3DCDIS3 : Field<L3DCDIS3, 30    > {}; // L3 delay counter disable
		struct L3DCO3   : Field<L3DCO3  , 31    > {}; // L3 delay counter override
		
		typedef FieldSet<L3DC3, L3DCDIS3, L3DCO3> ALL;
		
		L3ITCR3(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3ITCR3(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L3ITCR3"); this->SetDescription("L3 Cache Input Timing Control 3");
			
			L3DC3   ::SetName("L3DC3");    L3DC3   ::SetDescription("L3 delay count");
			L3DCDIS3::SetName("L3DCDIS3"); L3DCDIS3::SetDescription("L3 delay counter disable");
			L3DCO3  ::SetName("L3DCO3");   L3DCO3  ::SetDescription("L3 delay counter override");
		}
	};
	
	// L3 Cache Output Hold Control Register
	struct L3OHCR : PrivilegedSPR<L3OHCR, 1000>
	{
		typedef PrivilegedSPR<L3OHCR, 1000> Super;
		
		struct L3AOH     : Field<L3AOH    , 0 , 1 > {}; // L3 address output hold
		struct L3CLK0_OH : Field<L3CLK0_OH, 2 , 4 > {}; // L3_CLK0 output hold
		struct L3CLK1_OH : Field<L3CLK1_OH, 5 , 7 > {}; // L3_CLK1 output hold
		struct L3DOH0    : Field<L3DOH0   , 8 , 10> {}; // L3_DATA[00:07]/L3_DP[0] output hold
		struct L3DOH8    : Field<L3DOH8   , 11, 13> {}; // L3_DATA[08:15]/L3_DP[1] output hold
		struct L3DOH16   : Field<L3DOH16  , 14, 16> {}; // L3_DATA[16:23]/L3_DP[2] output hold
		struct L3DOH24   : Field<L3DOH24  , 17, 19> {}; // L3_DATA[24:31]/L3_DP[3] output hold
		struct L3DOH32   : Field<L3DOH32  , 20, 22> {}; // L3_DATA[32:39]/L3_DP[4] output hold
		struct L3DOH40   : Field<L3DOH40  , 23, 25> {}; // L3_DATA[40:47]/L3_DP[5] output hold
		struct L3DOH48   : Field<L3DOH48  , 26, 28> {}; // L3_DATA[48:55]/L3_DP[6] output hold
		struct L3DOH56   : Field<L3DOH56  , 29, 31> {}; // L3_DATA[56:63]/L3_DP[7] output hold
		
		typedef FieldSet<L3AOH, L3CLK0_OH, L3CLK1_OH, L3DOH0, L3DOH8, L3DOH16, L3DOH24, L3DOH32, L3DOH40, L3DOH48, L3DOH56> ALL;
		
		L3OHCR(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
		L3OHCR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("L3OHCR"); this->SetDescription("L3 Cache Output Hold Control Register");
			
			L3AOH    ::SetName("L3AOH");     L3AOH    ::SetDescription("L3 address output hold");
			L3CLK0_OH::SetName("L3CLK0_OH"); L3CLK0_OH::SetDescription("L3_CLK0 output hold");
			L3CLK1_OH::SetName("L3CLK1_OH"); L3CLK1_OH::SetDescription("L3_CLK1 output hold");
			L3DOH0   ::SetName("L3DOH0");    L3DOH0   ::SetDescription("L3_DATA[00:07]/L3_DP[0] output hold");
			L3DOH8   ::SetName("L3DOH8");    L3DOH8   ::SetDescription("L3_DATA[08:15]/L3_DP[1] output hold");
			L3DOH16  ::SetName("L3DOH16");   L3DOH16  ::SetDescription("L3_DATA[16:23]/L3_DP[2] output hold");
			L3DOH24  ::SetName("L3DOH24");   L3DOH24  ::SetDescription("L3_DATA[24:31]/L3_DP[3] output hold");
			L3DOH32  ::SetName("L3DOH32");   L3DOH32  ::SetDescription("L3_DATA[32:39]/L3_DP[4] output hold");
			L3DOH40  ::SetName("L3DOH40");   L3DOH40  ::SetDescription("L3_DATA[40:47]/L3_DP[5] output hold");
			L3DOH48  ::SetName("L3DOH48");   L3DOH48  ::SetDescription("L3_DATA[48:55]/L3_DP[6] output hold");
			L3DOH56  ::SetName("L3DOH56");   L3DOH56  ::SetDescription("L3_DATA[56:63]/L3_DP[7] output hold");
		}
	};

	/////////////////////////// Time Base Registers ///////////////////////////
	
	// Time Base Lower
	struct TBL : ReadOnlyNonPrivilegedTBR<TBL, 268>
	{
		typedef ReadOnlyNonPrivilegedTBR<TBL, 268> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		struct SPR268 : AltSLR<TBL, SLR_SPR_SPACE, 268, SLR_RO, SLR_NON_PRIVILEGED>
		{
			typedef AltSLR<TBL, SLR_SPR_SPACE, 268, SLR_RO, SLR_NON_PRIVILEGED> Super;
			
			SPR268(typename CONFIG::CPU *_cpu, TBL *_tbl) : Super(_cpu, _tbl) {}
		};
		
		struct SPR284 : AltSLR<TBL, SLR_SPR_SPACE, 284, SLR_WO, SLR_PRIVILEGED>
		{
			typedef AltSLR<TBL, SLR_SPR_SPACE, 284, SLR_WO, SLR_PRIVILEGED> Super;
			
			SPR284(typename CONFIG::CPU *_cpu, TBL *_tbl) : Super(_cpu, _tbl) {}
		};
		
		TBL(typename CONFIG::CPU *_cpu) : Super(_cpu), spr268(_cpu, this), spr284(_cpu, this) { Init(); }
		TBL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value), spr268(_cpu, this), spr284(_cpu, this) { Init(); }
		
		virtual void Reset() { /* unaffected */ }

		virtual bool MoveFrom(uint32_t& value)
		{
			this->cpu->RunInternalTimers();
			return Super::MoveFrom(value);
		}
		
		virtual bool MoveTo(uint32_t value)
		{
			uint32_t old_value = this->Get();
			bool status = Super::MoveTo(value);
			uint32_t new_value = this->Get();
			if(old_value != new_value)
			{
				ValueChanged(old_value);
			}
			return status;
		}
		
		virtual void ValueChanged(uint32_t old_value)
		{
		}
		
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("TBL"); this->SetDescription("Time Base Lower");
		}
		
		SPR268 spr268;
		SPR284 spr284;
	};
	
	// Time Base Upper
	struct TBU : ReadOnlyNonPrivilegedTBR<TBU, 269>
	{
		typedef ReadOnlyNonPrivilegedTBR<TBU, 269> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		struct SPR269 : AltSLR<TBU, SLR_SPR_SPACE, 269, SLR_RO, SLR_NON_PRIVILEGED>
		{
			typedef AltSLR<TBU, SLR_SPR_SPACE, 269, SLR_RO, SLR_NON_PRIVILEGED> Super;
			
			SPR269(typename CONFIG::CPU *_cpu, TBU *_tbu) : Super(_cpu, _tbu) {}
		};
		
		struct SPR285 : AltSLR<TBU, SLR_SPR_SPACE, 285, SLR_WO, SLR_PRIVILEGED>
		{
			typedef AltSLR<TBU, SLR_SPR_SPACE, 285, SLR_WO, SLR_PRIVILEGED> Super;
			
			SPR285(typename CONFIG::CPU *_cpu, TBU *_tbu) : Super(_cpu, _tbu) {}
		};
		
		TBU(typename CONFIG::CPU *_cpu) : Super(_cpu), spr269(_cpu, this), spr285(_cpu, this) { Init(); }
		TBU(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value), spr269(_cpu, this), spr285(_cpu, this) { Init(); }

		virtual void Reset() { /* unaffected */ }

		virtual bool MoveFrom(uint32_t& value)
		{
			this->cpu->RunInternalTimers();
			return Super::MoveFrom(value);
		}
		
		virtual bool MoveTo(uint32_t value)
		{
			this->cpu->RunInternalTimers();
			uint32_t old_value = this->Get();
			bool status = Super::MoveTo(value);
			uint32_t new_value = this->Get();
			this->cpu->RunInternalTimers();
			if(old_value != new_value)
			{
				ValueChanged(old_value);
			}
			return status;
		}
		
		virtual void ValueChanged(uint32_t old_value)
		{
		}
		
		using Super::operator =;
	private:
		void Init()
		{
			this->SetName("TBU"); this->SetDescription("Time Base Upper");
		}
		
		SPR269 spr269;
		SPR285 spr285;
	};
	
	struct TB
	{
		struct _TBU : TBU
		{
			_TBU(TB *_tb, typename CONFIG::CPU *_cpu) : TBU(_cpu), tb(_tb) {}
			_TBU(TB *_tb, typename CONFIG::CPU *_cpu, uint32_t _value) : TBU(_cpu, _value), tb(_tb) {}
			
			virtual void ValueChanged(uint32_t old_value)
			{
				tb->UpperValueChanged(old_value);
			}
			
			using TBU::operator =;
		private:
			TB *tb;
		};
		
		struct _TBL : TBL
		{
			_TBL(TB *_tb, typename CONFIG::CPU *_cpu) : TBL(_cpu), tb(_tb) {}
			_TBL(TB *_tb, typename CONFIG::CPU *_cpu, uint32_t _value) : TBL(_cpu, _value), tb(_tb) {}
			
			virtual void ValueChanged(uint32_t old_value)
			{
				tb->LowerValueChanged(old_value);
			}
			
			using TBL::operator =;
		private:
			TB *tb;
		};

		TB(typename CONFIG::CPU *cpu) : tbu(this, cpu), tbl(this, cpu) {}
		TB(typename CONFIG::CPU *cpu, uint64_t value) : tbu(this, cpu, uint32_t(value >> 32)), tbl(this, cpu, uint32_t(value)) {}
		
		TB& operator = (uint64_t value)
		{
			uint64_t old_value = uint64_t(*this);
			tbu = uint32_t(value >> 32);
			tbl = uint32_t(value);
			uint64_t new_value = uint64_t(*this);
			
			if(old_value != new_value)
			{
				ValueChanged(old_value);
			}
			
			return *this;
		}
		
		operator uint64_t() const
		{
			return (uint64_t(tbu) << 32) | uint64_t(tbl);
		}
		
		void UpperValueChanged(uint32_t tbu_old_value)
		{
			ValueChanged((uint64_t(tbu_old_value) << 32) | uint64_t(tbl));
		}
		
		void LowerValueChanged(uint32_t tbl_old_value)
		{
			ValueChanged((uint64_t(tbu) << 32) | uint64_t(tbl_old_value));
		}

		virtual void ValueChanged(uint64_t old_value)
		{
		}
	private:
		_TBU tbu;
		_TBL tbl;
	};

	/////////////////////// Performance Monitor Registers /////////////////////
	
	struct performance_monitoring
	{
		//  Performance Monitor Counter registers 0-3
		template <unsigned int PMC_NUM>
		struct PMC : PrivilegedPMR<PMC<PMC_NUM>, 16 + PMC_NUM>
		{
			typedef PrivilegedPMR<PMC<PMC_NUM>, 16 + PMC_NUM> Super;
			
			struct OV            : Field<OV           , 0> {};      // Overflow
			struct Counter_Value : Field<Counter_Value, 1, 31> {};  // Indicates the number of occurrences of the specified event
			
			typedef FieldSet<OV, Counter_Value> ALL;
			
			PMC(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
			PMC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
			using Super::operator =;
		private:
			void Init()
			{
				std::stringstream name_sstr;
				name_sstr << "PMC" << PMC_NUM;
				
				std::stringstream desc_sstr;
				desc_sstr << "Performance Monitor Counter register" << PMC_NUM;
				
							this->SetName(name_sstr.str());                         this->SetDescription(desc_sstr.str());
				OV           ::SetName("OV");            OV           ::SetDescription("Overflow");
				Counter_Value::SetName("Counter_Value"); Counter_Value::SetDescription("Indicates the number of occurrences of the specified event");
			}
		};
		
		typedef PMC<0> PMC0;
		typedef PMC<1> PMC1;
		typedef PMC<2> PMC2;
		typedef PMC<3> PMC3;

		// Performance Monitor Global Control Register 0
		struct PMGC0 : PrivilegedPMR<PMGC0, 400>
		{
			typedef PrivilegedPMR<PMGC0, 400> Super;
			
			struct FAC   : Field<FAC  , 0>  {}; // Freeze All Counters
			struct PMIE  : Field<PMIE , 1>  {}; // Performance monitor interrupt enable
			struct FCECE : Field<FCECE, 2>  {}; // Freeze Counters on Enabled Condition or Event
			struct TBSEL : Field<TBSEL, 19> {}; // Time Base Selector
			struct TBEE  : Field<TBEE , 23> {}; // Time base transition Event Enable
			
			SWITCH_ENUM_TRAIT(Model, _);
			CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<FAC, PMIE, FCECE, TBSEL, TBEE> ALL; };
			CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<FAC, PMIE, FCECE, TBSEL, TBEE> ALL; };
			typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
			
			PMGC0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
			PMGC0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
			using Super::operator =;
		private:
			void Init()
			{
					this->SetName("PMGC0");        this->SetDescription("Performance Monitor Global Control Register 0");
				FAC  ::SetName("FAC");   FAC  ::SetDescription("Freeze All Counters");
				PMIE ::SetName("PMIE");  PMIE ::SetDescription("Performance monitor interrupt enable");
				FCECE::SetName("FCECE"); FCECE::SetDescription("Freeze Counters on Enabled Condition or Event");
				TBSEL::SetName("TBSEL"); TBSEL::SetDescription("Time Base Selector");
				TBEE ::SetName("TBEE");  TBEE ::SetDescription("Time base transition Event Enable");
			}
		};
		
		// Performance monitor local control A(0-3)
		template <unsigned int PMLCA_NUM>
		struct PMLCa : PrivilegedPMR<PMLCa<PMLCA_NUM>, 144 + PMLCA_NUM>
		{
			typedef PrivilegedPMR<PMLCa<PMLCA_NUM>, 144 + PMLCA_NUM> Super;
			
			struct FC    : Field<FC   , 0>     {}; // Freeze Counter
			struct FCS   : Field<FCS  , 1>     {}; // Freeze Counter in Supervisor state
			struct FCU   : Field<FCU  , 2>     {}; // Freeze Counter in User state
			struct FCM1  : Field<FCM1 , 3>     {}; // Freeze Counter while Mark is set
			struct FCM0  : Field<FCM0 , 4>     {}; // Freeze Counter while Mark is cleared
			struct CE    : Field<CE   , 5>     {}; // Condition Enable
			struct EVENT : Field<EVENT, 8,15>  {}; // Event selector
			struct PMP   : Field<PMP  , 17,19> {}; // Performance Monitor Watchpoint Periodicity Select
			
			SWITCH_ENUM_TRAIT(Model, _);
			CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<FC, FCS, FCU, FCM1, FCM0, CE, EVENT, PMP> ALL; };
			CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<FC, FCS, FCU, FCM1, FCM0, CE, EVENT, PMP> ALL; };
			typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
			
			PMLCa(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
			PMLCa(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
			using Super::operator =;
		private:
			void Init()
			{
				std::stringstream name_sstr;
				name_sstr << "PMLCA" << PMLCA_NUM;
				
				std::stringstream desc_sstr;
				desc_sstr << "Performance monitor local control A" << PMLCA_NUM;
				
				this->SetName(name_sstr.str()); this->SetDescription(desc_sstr.str());
				
				FC   ::SetName("FC");     FC   ::SetDescription("Freeze Counter");
				FCS  ::SetName("FCS");    FCS  ::SetDescription("Freeze Counter in Supervisor state");
				FCU  ::SetName("FCU");    FCU  ::SetDescription("Freeze Counter in User state");
				FCM1 ::SetName("FCM1");   FCM1 ::SetDescription("Freeze Counter while Mark is set"); 
				FCM0 ::SetName("FCM0");   FCM0 ::SetDescription("Freeze Counter while Mark is cleared"); 
				CE   ::SetName("CE");     CE   ::SetDescription("Condition Enable");
				EVENT::SetName("EVENT");  EVENT::SetDescription("Event selector");  
				PMP  ::SetName("PMP");    PMP  ::SetDescription("Performance Monitor Watchpoint Periodicity Select");
			}
		};
		
		typedef PMLCa<0> PMLCa0;
		typedef PMLCa<1> PMLCa1;
		typedef PMLCa<2> PMLCa2;
		typedef PMLCa<3> PMLCa3;
		
		// Performance monitor local control B(0-3)
		template <unsigned int PMLCB_NUM>
		struct PMLCb : PrivilegedPMR<PMLCb<PMLCB_NUM>, 272 + PMLCB_NUM>
		{
			typedef PrivilegedPMR<PMLCb<PMLCB_NUM>, 272 + PMLCB_NUM> Super;
			
			struct TRIGONCTL  : Field<TRIGONCTL , 1,3>   {}; // Trigger-on Control Class - Class of Trigger-on source
			struct TRIGOFFCTL : Field<TRIGOFFCTL, 5,7>   {}; // Trigger-off Control Class - Class of Trigger-off source
			struct TRIGONSEL  : Field<TRIGONSEL , 9,12>  {}; // Trigger-on Source Select - Source Select based on setting of TRIGONCTL
			struct TRIGOFFSEL : Field<TRIGOFFSEL, 14,17> {}; // Trigger-off Source Select - Source Select based on setting of TRIGOFFCTL
			struct TRIGGERED  : Field<TRIGGERED , 18>    {}; // Triggered
			struct THRESHMUL  : Field<THRESHMUL , 21,23> {}; // Threshold multiple
			struct THRESHOLD  : Field<THRESHOLD , 26,31> {}; // Threshold
			
			SWITCH_ENUM_TRAIT(Model, _);
			CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<TRIGONCTL, TRIGOFFCTL, TRIGONSEL, TRIGOFFSEL, TRIGGERED, THRESHMUL, THRESHOLD> ALL; };
			CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<TRIGONCTL, TRIGOFFCTL, TRIGONSEL, TRIGOFFSEL, TRIGGERED, THRESHMUL, THRESHOLD> ALL; };
			typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
			
			PMLCb(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
			PMLCb(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
			using Super::operator =;
		private:
			void Init()
			{
				std::stringstream name_sstr;
				name_sstr << "PMLCB" << PMLCB_NUM;
				
				std::stringstream desc_sstr;
				desc_sstr << "Performance monitor local control B" << PMLCB_NUM;
				
				this->SetName(name_sstr.str()); this->SetDescription(desc_sstr.str());
				
				TRIGONCTL ::SetName("TRIGONCTL");  TRIGONCTL ::SetDescription("Trigger-on Control Class - Class of Trigger-on source");
				TRIGOFFCTL::SetName("TRIGOFFCTL"); TRIGOFFCTL::SetDescription("Trigger-off Control Class - Class of Trigger-off source");  
				TRIGONSEL ::SetName("TRIGONSEL");  TRIGONSEL ::SetDescription("Trigger-on Source Select - Source Select based on setting of TRIGONCTL"); 
				TRIGOFFSEL::SetName("TRIGOFFSEL"); TRIGOFFSEL::SetDescription("Trigger-off Source Select - Source Select based on setting of TRIGOFFCTL");  
				TRIGGERED ::SetName("TRIGGERED");  TRIGGERED ::SetDescription("Triggered"); 
				THRESHMUL ::SetName("THRESHMUL");  THRESHMUL ::SetDescription("Threshold multiple"); 
				THRESHOLD ::SetName("THRESHOLD");  THRESHOLD ::SetDescription("Threshold"); 
			}
		};
		
		typedef PMLCb<0> PMLCb0;
		typedef PMLCb<1> PMLCb1;
		typedef PMLCb<2> PMLCb2;
		typedef PMLCb<3> PMLCb3;

		// User Performance Monitor Counter registers 0-3
		template <unsigned int PMC_NUM>
		struct UPMC : ReadOnlyUPMR<PMC<PMC_NUM>, 0 + PMC_NUM>
		{
			typedef ReadOnlyUPMR<PMC<PMC_NUM>, 0 + PMC_NUM> Super;
			
			UPMC(typename CONFIG::CPU *_cpu, PMC<PMC_NUM> *pmc) : Super(_cpu, pmc) {}
		};
		
		typedef UPMC<0> UPMC0;
		typedef UPMC<1> UPMC1;
		typedef UPMC<2> UPMC2;
		typedef UPMC<3> UPMC3;
		
		// User Performance Monitor Global Control Register 0
		struct UPMGC0 : ReadOnlyUPMR<PMGC0, 384>
		{
			typedef ReadOnlyUPMR<PMGC0, 384> Super;
			
			UPMGC0(typename CONFIG::CPU *_cpu, PMGC0 *pmgc0) : Super(_cpu, pmgc0) {}
		};

		//  User performance monitor local control A(0-3)
		template <unsigned int PMLCA_NUM>
		struct UPMLCa : ReadOnlyUPMR<PMLCa<PMLCA_NUM>, 128 + PMLCA_NUM>
		{
			typedef ReadOnlyUPMR<PMLCa<PMLCA_NUM>, 128 + PMLCA_NUM> Super;
			
			UPMLCa(typename CONFIG::CPU *_cpu, PMLCa<PMLCA_NUM> *pmlca) : Super(_cpu, pmlca) {}
		};
		
		typedef UPMLCa<0> UPMLCa0;
		typedef UPMLCa<1> UPMLCa1;
		typedef UPMLCa<2> UPMLCa2;
		typedef UPMLCa<3> UPMLCa3;

		//  User performance monitor local control B(0-3)
		template <unsigned int PMLCB_NUM>
		struct UPMLCb : ReadOnlyUPMR<PMLCb<PMLCB_NUM>, 256 + PMLCB_NUM>
		{
			typedef ReadOnlyUPMR<PMLCb<PMLCB_NUM>, 256 + PMLCB_NUM> Super;
			
			UPMLCb(typename CONFIG::CPU *_cpu, PMLCb<PMLCB_NUM> *pmlca) : Super(_cpu, pmlca) {}
		};
		
		typedef UPMLCb<0> UPMLCb0;
		typedef UPMLCb<1> UPMLCb1;
		typedef UPMLCb<2> UPMLCb2;
		typedef UPMLCb<3> UPMLCb3;
	};

	//////////////////////// Thread Management Registers //////////////////////
	
	struct thread_management
	{
		// Thread Management Configuration Register 0
		struct TMCFG0 : ReadOnlyPrivilegedTMR<TMCFG0, 16>
		{
			typedef ReadOnlyPrivilegedTMR<TMCFG0, 16> Super;
			
			struct NPRIBITS : Field<NPRIBITS, 10,15> {}; // Number of bits of thread priority implemented
			struct NATHRD   : Field<NATHRD  , 18,23> {}; // Number of active threads implemented
			struct NTHRD    : Field<NTHRD   , 26,31> {}; // Number of threads implemented
			
			SWITCH_ENUM_TRAIT(Model, _);
			CASE_ENUM_TRAIT(E200Z710N3, _)  { typedef FieldSet<NPRIBITS, NATHRD, NTHRD> ALL; };
			CASE_ENUM_TRAIT(E200Z425BN3, _) { typedef FieldSet<NPRIBITS, NATHRD, NTHRD> ALL; };
			typedef typename ENUM_TRAIT(CONFIG::MODEL, _)::ALL ALL;
			
			TMCFG0(typename CONFIG::CPU *_cpu) : Super(_cpu) { Init(); }
			TMCFG0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) { Init(); }
			using Super::operator =;
		private:
			void Init()
			{
						this->SetName("TMCFG0");             this->SetDescription("Thread Management Configuration Register 0");
				NPRIBITS::SetName("NPRIBITS"); NPRIBITS::SetDescription("Number of bits of thread priority implemented");
				NATHRD  ::SetName("NATHRD");   NATHRD  ::SetDescription("Number of active threads implemented");
				NTHRD   ::SetName("NTHRD");    NTHRD   ::SetDescription("Number of threads implemented");
			}
		};
		
	};
	
	/////////////////////// General Purpose Register //////////////////////////
	
	struct GPR : unisim::util::reg::core::Register<GPR, 32>
	{
		typedef unisim::util::reg::core::Register<GPR, 32> Super;
		
		struct ALL : Field<ALL, 0, 31> {};
		
		GPR() : Super() {}
		GPR(uint32_t _value) : Super(_value) {}
		void Init(unsigned int reg_num)
		{
			std::stringstream sstr;
			sstr << "r" << reg_num;
			this->SetName(sstr.str());
			this->SetDescription("General Purpose Register");
		}
		
		using Super::operator =;
		
	};
	
	////////////////////////// Floating-Point unit ////////////////////////////
	
	struct NoFPU
	{
		NoFPU(typename CONFIG::CPU *_cpu) {}
		
		bool Fp32Load(typename CONFIG::CPU *cpu, unsigned int fd, EFFECTIVE_ADDRESS ea)   { throw std::runtime_error("INTERNAL ERROR! Fp32Load is unavailable"); return false; }
		bool Fp64Load(typename CONFIG::CPU *cpu, unsigned int fd, EFFECTIVE_ADDRESS ea)   { throw std::runtime_error("INTERNAL ERROR! Fp64Load is unavailable"); return false; }
		bool Fp32Store(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea)  { throw std::runtime_error("INTERNAL ERROR! Fp32Store is unavailable"); return false; }
		bool Fp64Store(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea)  { throw std::runtime_error("INTERNAL ERROR! Fp64Store is unavailable"); return false; }
		bool FpStoreLSW(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea) { throw std::runtime_error("INTERNAL ERROR! FpStoreLSW is unavailable"); return false; }
		
		FPR& GetFPR(unsigned int n) { static FPR dummy; throw std::runtime_error("INTERNAL ERROR! GetFPR is unavailable"); return dummy; }
		void SetFPR(unsigned int n, const SoftDouble& v) { throw std::runtime_error("INTERNAL ERROR! SetFPR is unavailable"); }
		FPSCR& GetFPSCR() { static FPSCR dummy; throw std::runtime_error("INTERNAL ERROR! GetFPSCR is unavailable"); return dummy; }
		
		bool CheckFloatingPointException(typename CONFIG::CPU *cpu) { throw std::runtime_error("INTERNAL ERROR! CheckFloatingPointException is unavailable"); return false; }
	};
	
	struct LegacyFPU
	{
		LegacyFPU(typename CONFIG::CPU *cpu);
		
		bool Fp32Load(typename CONFIG::CPU *cpu, unsigned int fd, EFFECTIVE_ADDRESS ea);
		bool Fp64Load(typename CONFIG::CPU *cpu, unsigned int fd, EFFECTIVE_ADDRESS ea);
		bool Fp32Store(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea);
		bool Fp64Store(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea);
		bool FpStoreLSW(typename CONFIG::CPU *cpu, unsigned int fs, EFFECTIVE_ADDRESS ea);
		
		FPR& GetFPR(unsigned int n) { return fpr[n]; }
		void SetFPR(unsigned int n, const SoftDouble& v) { fpr[n] = v; }
		FPSCR& GetFPSCR() { return fpscr; }
		
		bool CheckFloatingPointException(typename CONFIG::CPU *cpu);
		
	protected:
		FPR fpr[32];   // floating point registers (C++ objects implementing IEEE 754 floating point numbers)
		FPSCR fpscr;   // floating point status and control register
	};
	
	SWITCH_ENUM_TRAIT(bool, _FPU_impl);
	CASE_ENUM_TRAIT(false, _FPU_impl) { typedef NoFPU FPU; };
	CASE_ENUM_TRAIT(true, _FPU_impl)  { typedef LegacyFPU FPU; };
	typedef typename ENUM_TRAIT(CONFIG::HAS_FPU, _FPU_impl)::FPU FPU;
	
	//////////////////////////// Vector unit //////////////////////////////////
	
	struct NoVectorUnit
	{
		NoVectorUnit(typename CONFIG::CPU *cpu) {}
		
		VSCR& GetVSCR() { static VSCR dummy; throw std::runtime_error("INTERNAL ERROR! GetVSCR is unavailable"); return dummy; }
		VR& GetVR(unsigned int n) { static VR dummy; throw std::runtime_error("INTERNAL ERROR! GetVR is unavailable"); return dummy; }
	};
	
	struct AltivecVectorUnit
	{
		AltivecVectorUnit(typename CONFIG::CPU *cpu);
		
		VSCR& GetVSCR() { return vscr; }
		VR& GetVR(unsigned int n) { return vr[n]; }
		
	protected:
		VRSAVE vrsave;
		VSCR vscr;
		VR vr[32];
	};
	
	SWITCH_ENUM_TRAIT(bool, _VU_impl);
	CASE_ENUM_TRAIT(false, _VU_impl) { typedef NoVectorUnit VectorUnit; };
	CASE_ENUM_TRAIT(true, _VU_impl)  { typedef AltivecVectorUnit VectorUnit; };
	typedef typename ENUM_TRAIT(CONFIG::HAS_ALTIVEC, _VU_impl)::VectorUnit VectorUnit;
	
protected:
	///////////////////////////// Logger //////////////////////////////////////
	
	unisim::kernel::logger::Logger logger;
	
	/////////// unisim::service::interfaces::MemoryAccessReporting<> //////////
	
	bool requires_memory_access_reporting;      // indicates if the memory accesses require to be reported
	bool requires_fetch_instruction_reporting;  // indicates if the fetched instructions require to be reported
	bool requires_commit_instruction_reporting; // indicates if the committed instructions require to be reported
	
	inline bool MonitorLoad(EFFECTIVE_ADDRESS ea, unsigned int size);
	inline bool MonitorStore(EFFECTIVE_ADDRESS ea, unsigned int size);
	
	//////////////////////////// Statistics ///////////////////////////////////

	uint64_t instruction_counter;
	unisim::kernel::variable::Statistic<uint64_t> stat_instruction_counter;

	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_load_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_store_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_instruction_fetch_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_incoming_load_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_incoming_store_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_bus_read_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_bus_write_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_instruction_bus_read_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_load_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_store_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_instruction_fetch_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_incoming_load_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_incoming_store_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_bus_read_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_data_bus_write_xfered_bytes;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_instruction_bus_read_xfered_bytes;
	
	////////////////////////// Run-time parameters ////////////////////////////

	uint64_t trap_on_instruction_counter;
	unisim::kernel::variable::Parameter<uint64_t> param_trap_on_instruction_counter;
	
	uint64_t max_inst;
	unisim::kernel::variable::Parameter<uint64_t> param_max_inst;
	
	EFFECTIVE_ADDRESS halt_on_addr;
	std::string halt_on;
	unisim::kernel::variable::Parameter<std::string> param_halt_on;
	
	bool verbose_setup;
	unisim::kernel::variable::Parameter<bool> param_verbose_setup;

	bool verbose_exception;
	unisim::kernel::variable::Parameter<bool> param_verbose_exception;

	bool verbose_interrupt;
	unisim::kernel::variable::Parameter<bool> param_verbose_interrupt;
	
	bool verbose_move_to_slr;
	unisim::kernel::variable::Parameter<bool> param_verbose_move_to_slr;

	bool verbose_move_from_slr;
	unisim::kernel::variable::Parameter<bool> param_verbose_move_from_slr;
	
	bool enable_insn_trace;
	unisim::kernel::variable::Parameter<bool> param_enable_insn_trace;

	bool verbose_data_load;
	unisim::kernel::variable::Parameter<bool> param_verbose_data_load;
	
	bool verbose_data_store;
	unisim::kernel::variable::Parameter<bool> param_verbose_data_store;
	
	bool verbose_instruction_fetch;
	unisim::kernel::variable::Parameter<bool> param_verbose_instruction_fetch;
	
	bool verbose_data_bus_read;
	unisim::kernel::variable::Parameter<bool> param_verbose_data_bus_read;
	
	bool verbose_data_bus_write;
	unisim::kernel::variable::Parameter<bool> param_verbose_data_bus_write;
	
	bool verbose_instruction_bus_read;
	unisim::kernel::variable::Parameter<bool> param_verbose_instruction_bus_read;
	
	bool enable_linux_syscall_snooping;
	unisim::kernel::variable::Parameter<bool> param_enable_linux_syscall_snooping;
	
	unisim::kernel::variable::Parameter<bool> param_enable_shadow_memory;

public:
	inline bool IsVerboseDataLoad() const ALWAYS_INLINE { return verbose_data_load; }
	inline bool IsVerboseDataStore() const ALWAYS_INLINE { return verbose_data_store; }
	inline bool IsVerboseInstructionFetch() const ALWAYS_INLINE { return verbose_instruction_fetch; }
	inline bool IsVerboseDataBusRead() const ALWAYS_INLINE { return verbose_data_bus_read; }
	inline bool IsVerboseDataBusWrite() const ALWAYS_INLINE { return verbose_data_bus_write; }
	inline bool IsVerboseInstructionBusRead() const ALWAYS_INLINE { return verbose_instruction_bus_read; }

private:
	/////////////////////////// Registers Registry ////////////////////////////

	unisim::util::debug::SimpleRegisterRegistry registers_registry;       // Every CPU register interfaces

protected:
	void AddRegisterInterface(unisim::service::interfaces::Register *reg_if);
private:

	///////////////////////// Exception dispatcher ////////////////////////////
	
	ExceptionDispatcher<CONFIG::NUM_EXCEPTIONS> exception_dispatcher;
	
	//////////////////////// System Level Registers ///////////////////////////
	
	InvalidSLR<SLR_SPR_SPACE> invalid_spr;
	InvalidSLR<SLR_TBR_SPACE> invalid_tbr;
	InvalidSLR<SLR_PMR_SPACE> invalid_pmr;
	InvalidSLR<SLR_TMR_SPACE> invalid_tmr;
	ExternalDCR external_dcr;
	SLRBase *slr[SLR_TMR_SPACE - SLR_SPR_SPACE + 1][1024];
	
protected:
	//////////////////////////// Reset Address ////////////////////////////////
	
	EFFECTIVE_ADDRESS reset_addr;
	unisim::kernel::variable::Parameter<EFFECTIVE_ADDRESS> param_reset_addr;
	
	/////////////////////////// Program Counter ///////////////////////////////
	
	EFFECTIVE_ADDRESS cia;
	EFFECTIVE_ADDRESS nia;
	
	////////////////////// General Purpose Registers //////////////////////////
	
	GPR gpr[32];

	////////////////////// Special Purpose Registers //////////////////////////
	
	XER xer;
	LR lr;
	CTR ctr;
	
	////////////////////////// Condition Register  ////////////////////////////
	
	CR cr;
	
	////////////////////////// Floating-point unit ////////////////////////////
	
	FPU fpu;
	
	///////////////////////////// Vector unit /////////////////////////////////
	
	VectorUnit vector_unit;
	
private:
	////////////////////////// PowerPCLinux32 /////////////////////////////////
	struct PowerPCLinux32 : public unisim::util::os::linux_os::Linux<uint32_t, uint32_t>
	{
		typedef unisim::util::os::linux_os::PPCTS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> > PPCTarget;

		PowerPCLinux32(CPU *_cpu)
			: unisim::util::os::linux_os::Linux<uint32_t, uint32_t>(_cpu->GetDebugInfoStream(), _cpu->GetDebugWarningStream(), _cpu->GetDebugErrorStream(), _cpu, _cpu, _cpu)
			, ppc_target(0)
		{
			ppc_target = new PPCTarget(*this);
			SetTargetSystem(ppc_target);
		}
		
		~PowerPCLinux32()
		{
			delete ppc_target;
		}
		
	private:
		PPCTarget *ppc_target;
	};
	
	PowerPCLinux32 powerpc_linux32;
};

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_CPU_HH__
