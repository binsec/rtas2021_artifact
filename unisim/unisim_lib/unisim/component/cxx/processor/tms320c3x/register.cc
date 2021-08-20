/*
 *  Copyright (c) 2009,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include <unisim/component/cxx/processor/tms320c3x/register.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <sstream>
#include <iostream>
#include <string.h>

// #define __DEBUG_TMS320C3X_REGISTER__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320c3x {

	Register::Register()
	: lo_write_mask(0xffffffff)
	, lo(0)
	, hi_write_mask(0xff)
	, hi(0)
	{
	}
	
	void Register::SetLoWriteMask(uint32_t _lo_write_mask)
	{
		lo_write_mask = _lo_write_mask;
	}

	void Register::SetHiWriteMask(uint8_t _hi_write_mask, uint8_t _init_value)
	{
		hi = _init_value;
		hi_write_mask = _hi_write_mask;
	}
	
	std::ostream& operator << (std::ostream& os, const Register& reg)
	{
		std::stringstream sstr;
		
		sstr << "0x" << std::hex;
		sstr.fill('0');
		sstr.width(2);
		sstr << (unsigned int) reg.hi;
		sstr << " ";
		sstr.width(8);
		sstr << reg.lo;
		return os << sstr.str();
	}
	
	uint32_t Register::GetLo(uint16_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetLo(16)" << std::endl;
		std::cerr << " -> in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
#endif
		if ((value & (uint16_t)0xf000) == (uint16_t)0x8000)
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr << " <- out = 0x0" << std::endl;
#endif
			return 0;
		}
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << " <- out = 0x" << std::hex << (unsigned long)(((uint32_t)value & (uint32_t)0x00000fff) << 20) << std::dec << std::endl;
#endif
		return (((uint32_t)value) & (uint32_t)0x00000fff) << 20;
	}
	
	uint8_t Register::GetHi(uint16_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetHi(16)" << std::endl;
		std::cerr << " -> in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
#endif
		if ((value & (uint16_t)0xf000) == (uint16_t)0x8000)
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr << " <- out = 0x80" << std::endl;
#endif
			return (uint8_t)0x80;
		}
		//		if (value == (uint16_t)0x8000)
		//			return (uint8_t)0x80;
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << " <- out = 0x" << std::hex << (unsigned long)(uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff) << std::dec << std::endl;
#endif
		return (uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff);
	}
	
	uint32_t Register::GetLo16(uint16_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetLo(16)" << std::endl;
		std::cerr << " -> in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
#endif
		if ((value & (uint16_t)0xf000) == (uint16_t)0x8000)
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr << " <- out = 0x0" << std::endl;
#endif
			return 0;
		}
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << " <- out = 0x" << std::hex << (unsigned long)(((uint32_t)value & (uint32_t)0x00000fff) << 20) << std::dec << std::endl;
#endif
		return (((uint32_t)value) & (uint32_t)0x00000fff) << 20;
	}
	
	uint8_t Register::GetHi16(uint16_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetHi(16)" << std::endl;
		std::cerr << " -> in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
#endif
		if ((value & (uint16_t)0xf000) == (uint16_t)0x8000)
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr << " <- out = 0x80" << std::endl;
#endif
			return (uint8_t)0x80;
		}
		//		if (value == (uint16_t)0x8000)
		//			return (uint8_t)0x80;
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << " <- out = 0x" << std::hex << (unsigned long)(uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff) << std::dec << std::endl;
#endif
		return (uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff);
	}
	
	void Register::SetFromShortFPFormat(uint16_t value)
	{
		this->SetHi(GetHi16(value));
		this->SetLo(GetLo16(value));
		// lo = value & (uint32_t)0x00000fff;
		// lo = lo << 20;
		// hi = (uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff);
	}
	
	uint32_t Register::GetLo(uint32_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetLo(32)" << std::endl;
		std::cerr << " -> in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
#endif
		if ((value & (uint32_t)0xff000000) == (uint32_t)0x80000000)
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr << " <- out = 0x0" << std::endl;
#endif
			return 0;
		}
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << " <- out = 0x" << std::hex << (unsigned long)(((uint32_t)value & (uint32_t)0x00ffffff) << 8) << std::dec << std::endl;
#endif
		return ((uint32_t)value & (uint32_t)0x00ffffff) << 8;
	}
	
	uint8_t Register::GetHi(uint32_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetHi(32)" << std::endl;
		std::cerr << " <- in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
		std::cerr << " -> out = 0x" << std::hex << (unsigned int)(uint8_t)((value >> 24) & (uint32_t)0x00ff) << std::dec << std::endl;
#endif
		return (uint8_t)((value >> 24) & (uint32_t)0x00ff);
	}
	
	uint32_t Register::GetLo32(uint32_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetLo(32)" << std::endl;
		std::cerr << " -> in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
#endif
		if ((value & (uint32_t)0xff000000) == (uint32_t)0x80000000)
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr << " <- out = 0x0" << std::endl;
#endif
			return 0;
		}
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << " <- out = 0x" << std::hex << (unsigned long)(((uint32_t)value & (uint32_t)0x00ffffff) << 8) << std::dec << std::endl;
#endif
		return ((uint32_t)value & (uint32_t)0x00ffffff) << 8;
	}
	
	uint8_t Register::GetHi32(uint32_t value)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "GetHi(32)" << std::endl;
		std::cerr << " <- in = 0x" << std::hex << (unsigned long)value << std::dec << std::endl;
		std::cerr << " -> out = 0x" << std::hex << (unsigned int)(uint8_t)((value >> 24) & (uint32_t)0x00ff) << std::dec << std::endl;
#endif
		return (uint8_t)((value >> 24) & (uint32_t)0x00ff);
	}
	
	void Register::SetFromSinglePrecisionFPFormat(uint32_t value)
	{
		// the 0 value is converted seamlessly from single Precision to extended
		//   so no need to check the input value
		this->SetLo(GetLo32(value));
		this->SetHi(GetHi32(value));
	}
	
	uint32_t Register::GetSinglePrecisionFPFormat() 
	{
		uint32_t t_hi;
		uint32_t t_lo;
		
		t_hi = GetHi();
		t_hi = t_hi << 24;
		t_lo = GetLo();
		t_lo = t_lo & ~((uint32_t)0x00ff);
		t_lo = t_lo >> 8;
		return t_hi + t_lo;
	}
		
	void Register::Float(uint32_t value, uint32_t& neg)
	{
		uint32_t _lo = value;
		uint32_t _hi = 30;
		unsigned int count_nsb = 0; // counter of non significative bits
		neg = 0;
		if (_lo)
		{
			if (_lo >= (uint32_t)0x80000000)
			{
				neg = 1;
				count_nsb = unisim::util::arithmetic::CountLeadingZeros(~_lo) - 1;
			}
			else
			{
				count_nsb = unisim::util::arithmetic::CountLeadingZeros(_lo) - 1;
			}
			if (count_nsb == 31)
				_lo = 0;
			else
				_lo = _lo << (count_nsb + 1);
			_hi = 30 - count_nsb;
			if (neg)
			{
				_lo = _lo | (uint32_t)0x80000000;
			}
			else
			{
				_lo = _lo & ~(uint32_t)0x80000000;
			}
		}
		else
		{
			_hi = (uint8_t)0x080;
		}
		SetLo(_lo);
		SetHi(_hi);
	}
	
	uint32_t Register::Fix(uint32_t& overflow, uint32_t& neg)
	{
		overflow = 0;
		neg = 0;
		
		if (GetHi() == 0x80)
		{
			return 0;
		}
		
		// check for special cases of exponent to signal an overflow
		if ((int8_t)GetHi() > 30)
		{
			overflow = 1;
			// check if the sign is positive or negative
			// if positive return the biggest positive number
			// if negative return the biggest negative number
			if (GetLo() & (uint32_t)0x80000000) // the sign is negative
			{
				neg = 1;
				return (uint32_t)0x80000000;
			}
			else // the sign is positve
			{
				return ~(uint32_t)0x80000000;
			}
		}
		
		// comput the shift value
		uint32_t shift = 31 - (int8_t)GetHi();
		// to avoid circular shift trunk it to 32
		if (shift > 32) shift = 32;
		
		int64_t ext_lo;
		// convert mantissas to their full representation (33-bit)
		if (lo & (uint32_t)0x80000000)
		{
			ext_lo = (int64_t)((int32_t)GetLo()) & ~(uint64_t)0x80000000;
			neg = 1;
		}
		else
			ext_lo = (uint64_t)(GetLo() | (uint32_t)0x80000000);
		
		// perform the shift on the mantissa and return the lowest 32 bits
		ext_lo = (int64_t)ext_lo >> shift;
		return (uint32_t)ext_lo;
	}
	
	void Register::Absf(const Register& reg, uint32_t& overflow)
	{
		this->Absf(reg.GetHi(), reg.GetLo(), overflow);
	}
	
	void Register::Absf16(uint16_t imm, uint32_t& overflow)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "Abs(imm16)" << std::endl;
#endif
		this->Absf(GetHi16(imm), GetLo16(imm), overflow);
	}
	
	void Register::Absf32(uint32_t imm, uint32_t& overflow)
	{
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "Abs(imm32)" << std::endl;
#endif
		this->Absf(GetHi32(imm), GetLo32(imm), overflow);
	}
	
	void Register::Addf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg) 
	{
		this->Addf(this->GetHi(), this->GetLo(), reg.GetHi(), reg.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Addf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Addf(this->GetHi(), this->GetLo(), GetHi16(imm), GetLo16(imm), overflow, underflow, neg);
	}
	
	void Register::Addf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Addf(this->GetHi(), this->GetLo(), GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Addf(const Register& reg_a, const Register& reg_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Addf(reg_a.GetHi(), reg_a.GetLo(), reg_b.GetHi(), reg_b.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Addf(const Register& reg, uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Addf(reg.GetHi(), reg.GetLo(), GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Addf(uint32_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Addf(reg, imm, overflow, underflow, neg);
	}
	
	void Register::Addf(uint32_t imm_a, uint32_t imm_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Addf(GetHi32(imm_a), GetLo32(imm_a), GetHi32(imm_b), GetLo32(imm_b), overflow, underflow, neg);
	}
	
	void Register::Subf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg) 
	{
		this->Subf(this->GetHi(), this->GetLo(), reg.hi, reg.lo, overflow, underflow, neg);
	}
	
	void Register::Subf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(this->GetHi(), this->GetLo(), GetHi16(imm), GetLo16(imm), overflow, underflow, neg);
	}
	
	void Register::Subf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(this->GetHi(), this->GetLo(), GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Subf(const Register& reg_a, const Register& reg_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(reg_a.GetHi(), reg_a.GetLo(), reg_b.GetHi(), reg_b.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Subf16(const Register& reg, uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(reg.GetHi(), reg.GetLo(), GetHi16(imm), GetLo16(imm), overflow, underflow, neg);
	}
	
	void Register::Subf32(const Register& reg, uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(reg.GetHi(), reg.GetLo(), GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Subf16(uint16_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(GetHi16(imm), GetLo16(imm), reg.GetHi(), reg.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Subf32(uint32_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(GetHi32(imm), GetLo32(imm), reg.GetHi(), reg.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Subf(uint32_t imm_a, uint32_t imm_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf(GetHi32(imm_a), GetLo32(imm_a), GetHi32(imm_b), GetLo32(imm_b), overflow, underflow, neg);
	}
	
	void Register::Negf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf((uint8_t)0x80, 0, reg.GetHi(), reg.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Negf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf((uint8_t)0x80, 0, GetHi16(imm), GetLo16(imm), overflow, underflow, neg);
	}
	
	void Register::Negf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Subf((uint8_t)0x80, 0, GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Normf(const Register& reg, uint32_t& underflow, uint32_t& neg)
	{
		this->Normf(reg.GetHi(), reg.GetLo(), underflow, neg);
	}
	
	void Register::Normf16(uint16_t imm, uint32_t& underflow, uint32_t& neg)
	{
		this->Normf(GetHi16(imm), GetLo16(imm), underflow, neg);
	}
	
	void Register::Normf32(uint32_t imm, uint32_t& underflow, uint32_t& neg)
	{
		this->Normf((uint8_t)((imm >> 24) & (uint32_t)0x00ff), ((uint32_t)imm & (uint32_t)0x00ffffff) << 8, underflow, neg);
	}
	
	void Register::Mpyf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(this->GetHi(), this->GetLo(), reg.GetHi(), reg.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Mpyf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(this->GetHi(), this->GetLo(), GetHi16(imm), GetLo16(imm), overflow, underflow, neg);
	}
	
	void Register::Mpyf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(this->GetHi(), this->GetLo(), GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Mpyf(const Register& reg_a, const Register& reg_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(reg_a.GetHi(), reg_a.GetLo(), reg_b.GetHi(), reg_b.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Mpyf(const Register& reg, uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(reg.GetHi(), reg.GetLo(), GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}
	
	void Register::Mpyf(uint32_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(reg, imm, overflow, underflow, neg);
	}
	
	void Register::Mpyf(uint32_t imm_a, uint32_t imm_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Mpyf(GetHi32(imm_a), GetLo32(imm_a), GetHi32(imm_b), GetLo32(imm_b), overflow, underflow, neg);
	}
	
	void Register::Rndf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Rndf(reg.GetHi(), reg.GetLo(), overflow, underflow, neg);
	}
	
	void Register::Rndf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Rndf(GetHi16(imm), GetLo16(imm), overflow, underflow, neg);
	}
	
	void Register::Rndf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		this->Rndf(GetHi32(imm), GetLo32(imm), overflow, underflow, neg);
	}

	void Register::Absf(uint8_t hi_a, uint32_t lo_a, uint32_t& overflow)
	{
		overflow = 0;
		// if the exponent value is -128, then the mantissa should be 0
		//   to represent the 0.0 value
		if (hi_a == (uint8_t)0x80)
		{
			this->SetHi((uint8_t)0x80);
			this->SetLo(0);
			return;
		}
		overflow = false;
		if (hi_a == (uint8_t)0x7f && lo_a == (uint32_t)0x80000000)
		{
			this->SetHi((uint8_t)0x7f);
			this->SetLo((uint32_t)0x7fffffff);
			overflow = 1;
			return;
		}
		// if the value is negative, then the abs operation can be performed
		if ((int32_t)lo_a < 0)
		{
			if (lo_a == (uint32_t)0x80000000)
			{
				this->SetLo(0);
				this->SetHi(hi_a + 1);
			}
			else
			{
				this->SetLo((~lo_a) + 1); // absolute value of the mantissa
				this->SetHi(hi_a);
			}
		}
		else
		{
			// the value is positive, simply copy exponent and mantissa
			this->SetHi(hi_a);
			this->SetLo(lo_a);
		}
	}
	
	void Register::Addf(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		AddfSubf(true, hi_a, lo_a, hi_b, lo_b, overflow, underflow, neg);
	}
	
	void Register::Subf(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		AddfSubf(false, hi_a, lo_a, hi_b, lo_b, overflow, underflow, neg);
	}
	
	void Register::AddfSubf(bool is_add, uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		uint8_t r_hi = 0;
		//uint32_t r_lo = 0;
		int64_t ext_lo_a = 0;
		int64_t ext_lo_b = 0;
		overflow = 0;
		underflow = 0;
		neg = 0;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr 
		<< "1- add = " << is_add << std::endl
		<< "   hi_a = 0x" << std::hex << (unsigned int)hi_a << "\t lo_a = 0x" << (unsigned int)lo_a << std::endl
		<< "   hi_b = 0x"             << (unsigned int)hi_b << "\t lo_b = 0x" << (unsigned int)lo_b << std::dec << std::endl;
#endif

		// convert mantissas to their full representation (33-bit)
		// when the exponent is 0x80 (-128) the mantissa should be considered to contain 0, whatever its real value is
		if (hi_a == (uint8_t)0x80)
			ext_lo_a = 0;
		else
			if (lo_a & (uint32_t)0x80000000)
				ext_lo_a = (int64_t)((int32_t)lo_a) & ~(uint64_t)0x80000000;
			else
				ext_lo_a = (uint64_t)(lo_a | (uint32_t)0x80000000);
		
		if (hi_b == (uint8_t)0x80)
			ext_lo_b = 0;
		else
			if (lo_b & (uint32_t)0x80000000)
				ext_lo_b = (int64_t)((int32_t)lo_b) & ~(uint64_t)0x80000000;
			else
				ext_lo_b = (uint64_t)(lo_b | (uint32_t)0x80000000);
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
	std::cerr << std::hex
		<< "2- hi_a = 0x" << (unsigned int)hi_a << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::dec << std::endl;
#endif										
		// result takes the biggest exponent
		// and compute the difference of the exponent to align mantissas
		uint32_t diff_exp;
		if ((int8_t)hi_a <= (int8_t)hi_b)
		{
			r_hi = hi_b;
			diff_exp = (int32_t)(int8_t)hi_b - (int32_t)(int8_t)hi_a;
			if (diff_exp >= 64)
			{
				if (ext_lo_a < 0) ext_lo_a = -1;
				else ext_lo_a = 0;
			}
			else
				ext_lo_a = ext_lo_a >> diff_exp;
		}
		else
		{
			r_hi = hi_a;
			diff_exp = (int32_t)(int8_t)hi_a - (int32_t)(int8_t)hi_b;
			if (diff_exp >= 64)
			{
				if (ext_lo_b < 0) ext_lo_b = -1;
				else ext_lo_b = 0;
			}
			else
				ext_lo_b = ext_lo_b >> diff_exp;
		}
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
	std::cerr
		<< "3- diff_exp = " << (int)diff_exp << std::endl << std::hex
		<< "   hi_a = 0x" << (unsigned int)hi_a << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::endl
		<< "   hi   = 0x" << (unsigned int)r_hi << std::dec << std::endl;
#endif								
		// add the mantissas
		int64_t ext_lo_c = ext_lo_a;
		if (is_add) ext_lo_c += ext_lo_b;
		else ext_lo_c -= ext_lo_b;
		
		// set the negative flag if necessary
		if (ext_lo_c < 0) neg = 1;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr
		<< "4- diff_exp = " << (int)diff_exp << std::endl << std::hex
		<< "   hi_a = 0x" << (unsigned int)hi_a          << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b          << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::endl
		<< "   hi   = 0x" << (unsigned int)r_hi << "\t ext_lo_c = 0x" << (unsigned long long int)ext_lo_c << std::dec << std::endl;
#endif										
		// check for special cases of the result mantissa
		// 1 - check mantissa is 0
		if (ext_lo_c == 0)
		{
			r_hi = (uint8_t)0x80;
		}
		else
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr
			<< "5- ext_lo_c = 0x" << std::hex << (unsigned long long int)ext_lo_c << std::dec << std::endl
			<< "   ext_lo_c > 0 --> " << (ext_lo_c > 0) << std::endl
			<< "   ext_lo_c >> 32 --> 0x" << std::hex << (unsigned long long int)(ext_lo_c >> 32) << std::dec << std::endl;
#endif
			// 2 - check for overflow
			if ((ext_lo_c > 0) && ((ext_lo_c >> 32) != 0))
			{
#ifdef __DEBUG_TMS320C3X_REGISTER__
				std::cerr
				<< "6- positive overflow checking" << std::endl;
#endif
				// the mantissa is positive, check how many bits we have to shift it
				//   to be an signed 33bits number
				ext_lo_c = ext_lo_c >> 1;
				if (((int32_t)(int8_t)r_hi) + 1 > 127) 
				{
					overflow = 1;
					r_hi = (uint8_t)0x7f;
				}
				else
					r_hi = r_hi + 1;
			}
			else
			{
				if ((ext_lo_c < 0) && (~(ext_lo_c >> 32) != 0))
				{
#ifdef __DEBUG_TMS320C3X_REGISTER__
					std::cerr
					<< "7- negative overflow checking" << std::endl;
#endif
					// the mantissa is negative, check how many bits we have to shift it
					//   to be a signed 33bits number
					ext_lo_c = ext_lo_c >> 1;
					if (((int32_t)(int8_t)r_hi) + 1 > 127) 
					{
						overflow = 1;
						r_hi = (uint8_t)0x7f;
					}
					else
						r_hi = r_hi + 1;
				}
				else
				{
#ifdef __DEBUG_TMS320C3X_REGISTER__
				std::cerr << "8- normalizing" << std::endl;
#endif					
					// 3 - check if the result needs to be normalized
					if (ext_lo_c > 0 && (ext_lo_c & (uint64_t)0x80000000) == 0)
					{
						uint32_t count = unisim::util::arithmetic::CountLeadingZeros((uint64_t)ext_lo_c);
						count = count - 32;
						ext_lo_c = ext_lo_c << count;
#ifdef __DEBUG_TMS320C3X_REGISTER__
						std::cerr << "8.1- normalizing pos" << std::endl
						<< "  hi_c = " << (int)(int8_t)r_hi << " (" << (int)(((int32_t)(int8_t)r_hi) - count) << ")" << std::endl
						<< "  count = " << count << std::endl;
#endif
						if ((int32_t)(((int32_t)(int8_t)r_hi) - count) <= -128)
						{
							underflow = 1;
							r_hi = (uint8_t)0x80;
						}
						else
							r_hi = r_hi - count;
					}
					else
					{
#ifdef __DEBUG_TMS320C3X_REGISTER__
						std::cerr 
						<< "8.1- normalizing neg" << std::endl
						<< "   hi   = 0x" << std::hex << (unsigned int)r_hi << "\t ext_lo_c = 0x" << (unsigned long long)ext_lo_c << std::dec << std::endl
						<< std::endl;
#endif						
						if (ext_lo_c < 0 && ((ext_lo_c & (uint64_t)0x80000000) != 0))
						{
							uint32_t count = unisim::util::arithmetic::CountLeadingZeros(~(uint64_t)ext_lo_c);

							count = count - 32;
							ext_lo_c = ext_lo_c << count;
							if ((int32_t)((int32_t)(int8_t)r_hi - count) <= -128)
							{
								underflow = 1;
								r_hi = (uint8_t)0x80;
							}
							else
								r_hi = r_hi - count;
						}
					}
				}
			}
		}
		
		// check for special cases of the result exponent
		// 1 - exponent overflow
		if (overflow)
		{
			if (ext_lo_c > 0)
			{
				ext_lo_c = (uint64_t)0xffffffff;
			}
			else // ext_lo_c < 0
			{
				ext_lo_c = ~(uint64_t)0xffffffff;
			}
		}
		else
		{
			// 2 - exponent underflow
			if (underflow)
			{
				ext_lo_c = 0;
			}
			else
			{
				// 3 - exponent in range
				// nothing to do
			}
		}
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << "9- Setting lo" << std::endl;
		std::cerr << "  ext_lo_c = 0x" << std::hex << (unsigned long long)ext_lo_c << std::dec << std::endl;
		
#endif
		// set the mantissa and exponent into the register
		this->SetLo((uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) 
							   | ((ext_lo_c >> 1) & 
								  (uint64_t)0x80000000)));
		this->SetHi(r_hi);
	}
	
	void Register::Mpyf(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		int64_t ext_lo_a = 0;
		int64_t ext_lo_b = 0;
		overflow = 0;
		underflow = 0;
		neg = 0;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr 
		<< "1- mpyf " << std::endl
		<< "   hi_a = 0x" << std::hex << (unsigned int)hi_a << "\t lo_a = 0x" << (unsigned int)lo_a << std::endl
		<< "   hi_b = 0x"             << (unsigned int)hi_b << "\t lo_b = 0x" << (unsigned int)lo_b << std::dec << std::endl;
#endif
		
		// convert mantissas to their full representation (33-bit)
		// when the exponent is 0x80 (-128) the mantissa should be considered to contain 0, whatever its real value is
		if (hi_a == (uint8_t)0x80)
			ext_lo_a = 0;
		else
			if (lo_a & (uint32_t)0x80000000)
				ext_lo_a = (int64_t)((int32_t)lo_a) & ~(uint64_t)0x80000000;
			else
				ext_lo_a = (uint64_t)(lo_a | (uint32_t)0x80000000);
		
		if (hi_b == (uint8_t)0x80)
			ext_lo_b = 0;
		else
			if (lo_b & (uint32_t)0x80000000)
				ext_lo_b = (int64_t)((int32_t)lo_b) & ~(uint64_t)0x80000000;
			else
				ext_lo_b = (uint64_t)(lo_b | (uint32_t)0x80000000);
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << std::hex
		<< "2- extended mantissas" << std::endl
		<< "   hi_a = 0x" << (unsigned int)hi_a << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::dec << std::endl;
#endif										

		// convert the mantissas to their 25-bit representation
		ext_lo_a = ext_lo_a >> 8;
		ext_lo_b = ext_lo_b >> 8;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << std::hex
		<< "3- 25-bit representation" << std::endl
		<< "   hi_a = 0x" << (unsigned int)hi_a << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::dec << std::endl;
#endif										

		// compute the result exponent
		int32_t exp_c = (int32_t)((int8_t)hi_a) + (int32_t)((int8_t)hi_b);
		
		// multiply the mantissas
		int64_t ext_lo_c = ext_lo_a * ext_lo_b;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << std::hex
		<< "3.1- intermediate result" << std::endl
		<< "   hi_c = 0x" << (unsigned int)exp_c << "\t ext_lo_c = 0x" << (unsigned long long int)ext_lo_c << std::dec << std::endl;
#endif										
		
		// test for special cases of the mantissa
		// 1 - shift by one or two the result
		int64_t tmp = ext_lo_c;
		unsigned int shift = 0;
		tmp = tmp >> 46;
		if (ext_lo_c < 0) tmp = ~tmp;
		if (tmp & 4) 
		{
			shift += 2;
		}
		else
		{
			if (tmp & 2) 
			{
				shift += 1;
			}
		}
		ext_lo_c = ext_lo_c >> shift;
		exp_c += shift;
		// 2 - dispose of extra bits
		ext_lo_c = ext_lo_c >> 15;
		// 3 - check that the result is not 0
		if (ext_lo_c == 0)
			exp_c = -128;
		// 4 - check if the result is negative
		if (ext_lo_c < 0) neg = 1;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr << std::hex
		<< "4- result" << std::endl
		<< "   hi_c = 0x" << (unsigned int)exp_c << "\t ext_lo_c = 0x" << (unsigned long long int)ext_lo_c << std::dec << std::endl;
#endif										

		// test for special cases of the result exponent
		// 1 - exponent overflow
		if (exp_c > 127)
		{
			overflow = 1;
			exp_c = 127;
			if (ext_lo_c > 0)
			{
				ext_lo_c = (uint64_t)0xffffffff;
			}
			else // ext_lo_c < 0
			{
				ext_lo_c = ~(uint64_t)0xffffffff;
			}
		}
		else
		{
			// 2 - exponent underflow
			if ( exp_c <= -128 && ext_lo_c != 0 )
			{
				underflow = 1;
				exp_c = -128;
				ext_lo_c = 0;
			}
			else
			{
				// 3 - exponent in range
				// nothing to do
			}
		}
		
		// set the mantissa and exponent into the register
		this->SetLo((uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) | ((ext_lo_c >> 1) & (uint64_t)0x80000000)));
		this->SetHi((uint8_t)((int8_t)exp_c));
	}
	
	void Register::Normf(uint8_t hi_a, uint32_t lo_a, uint32_t& underflow, uint32_t& neg)
	{
		underflow = 0;
		neg = 0;
		
		if (lo_a == 0)
		{
			this->SetLo(0);
			this->SetHi((uint8_t)0x80);
			return;
		}
		
		uint32_t man = lo_a; // sign extend the mantissa
		int32_t exp = (int8_t)hi_a; // sign extend the exponent
		uint32_t count = 0;
		
		if ((int32_t)lo_a < 0)
		{
			neg = 1;
			count = unisim::util::arithmetic::CountLeadingZeros(~lo_a);
		}
		else
			count = unisim::util::arithmetic::CountLeadingZeros(lo_a);
		
		if (count == 32)
			man = 0;
		else
			man = man << count;
		exp = exp - count;
		if (exp <= -128)
		{
			this->SetHi((uint8_t)0x80);
			this->SetLo(0);
			underflow = 1;
		}
		else
		{
			this->SetHi((uint32_t)exp);
			this->SetLo(man ^ (uint32_t)0x80000000);
		}
	}
	
	void Register::Rndf(uint8_t hi_a, uint32_t lo_a, uint32_t& overflow, uint32_t& underflow, uint32_t& neg)
	{
		uint8_t r_hi = 0;
		overflow = 0;
		underflow = 0;
		neg = 0;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr 
		<< "1- rnd = " << std::endl
		<< "   hi_a = 0x" << std::hex << (unsigned int)hi_a << "\t lo_a = 0x" << (unsigned int)lo_a << std::dec << std::endl;
#endif

		// check when src = 0
		if (hi_a == (uint8_t)0x80)
		{
			underflow = 1;
			this->SetHi((uint8_t)0x80);
			this->SetLo(0);
			return;
		}
		
		// create a fake register (hi_b and ext_lo_b) to perform an addition of the input
		//   with:
		//           signed(hi_a) - 24
		//     1 x 2
		int64_t ext_lo_a = 0;
		int64_t ext_lo_b = (uint32_t)0x00000080;
		// convert mantissas to their full representation (33-bit)
		if (lo_a & (uint32_t)0x80000000)
			ext_lo_a = (int64_t)((int32_t)lo_a) & ~(uint64_t)0x80000000;
		else
			ext_lo_a = (uint64_t)(lo_a | (uint32_t)0x80000000);
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr 
		<< "2- rnd (extending sign) " << std::endl
		<< "   ext_lo_a = 0x" << std::hex << (unsigned long long int)ext_lo_a << std::dec << std::endl;
#endif
		
		// add the mantissas
		int64_t ext_lo_c = ext_lo_a + ext_lo_b;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr 
		<< "3- rnd (adding 1x2^(exp)-24) " << std::endl
		<< "   ext_lo_c = 0x" << std::hex << (unsigned long long int)ext_lo_c << std::dec << std::endl;
#endif
		
		// get the sign
		if (ext_lo_c < 0) neg = 1;
		
		// set the exponent result
		r_hi = hi_a;

		// check for special cases of the result mantissa
		// 1 - mantissa is 0
		if (ext_lo_c == 0)
		{
			ext_lo_c = 0;
			r_hi = (uint8_t)0x80;
		}
		else
		{
			// 2 - mantissa overflow
			if ((ext_lo_c > 0) && ((ext_lo_c >> 32) != 0))
			{
				// the mantissa is possitive, check how many bits we have to shift it
				//   to be a signed 33bits number
				ext_lo_c = ext_lo_c >> 1;
				if (((int32_t)(int8_t)r_hi + 1) > 127) 
				{
					overflow = 1;
					r_hi = (uint8_t)0x7f;
				}
				else
					r_hi = hi_a + 1;
			}
			else
			{
				if ((ext_lo_c < 0) && (~(ext_lo_c >> 32) != 0))
				{
					// the mantissa is negative, check how many bits we have to shift it
					//   to be a signed 33bits number
					ext_lo_c = (uint64_t)ext_lo_c >> 1;
					if (((int32_t)(int8_t)r_hi + 1) > 127) 
					{
						overflow = 1;
						r_hi = (uint8_t)0x7f;
					}
					else
						r_hi = hi_a + 1;
				}
				else 
				{
					// 3 - check if the result needs to be normalized
					if (ext_lo_c > 0 && (ext_lo_c & (uint64_t)0x80000000) == 0)
					{
						uint32_t count = unisim::util::arithmetic::CountLeadingZeros((uint64_t)ext_lo_c);
						count = count - 32;
						ext_lo_c = ext_lo_c << count;
						if ((int32_t)((int32_t)(int8_t)r_hi - count) <= -128)
						{
							underflow = 1;
							r_hi = (uint8_t)0x80;
						}
						else
							r_hi = r_hi - count;
					}
					else
					{
						if ((ext_lo_c < 0) && ((ext_lo_c & (uint64_t)0x80000000) != 0))
						{
							uint32_t count = unisim::util::arithmetic::CountLeadingZeros(~(uint64_t)ext_lo_c);
							
							count = count - 32;
							ext_lo_c = ext_lo_c << count;
							if ((int32_t)(((int32_t)(int8_t)r_hi) - count) <= -128)
							{
								underflow = true;
								r_hi = (uint8_t)0x80;
							}
							else
								r_hi = r_hi - count;
						}
					}
				}
			}
		}

		// check for special cases of the result exponent
		// 1 - exponent overflow
		if (overflow)
		{
			if (ext_lo_c > 0)
			{
				ext_lo_c = (uint64_t)0xffffffff;
			}
			else // ext_lo_c < 0
			{
				ext_lo_c = ~(uint64_t)0xffffffff;
			}
		}
		else
		{
			// 2 - exponent underflow
			if (underflow)
			{
				ext_lo_c = 0;
			}
			else
			{
				// 3 - exponent in range
				// nothing to do
			}
		}
		
		// set the mantissa into the register
		this->SetLo((uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) 
							   | ((ext_lo_c >> 1) & 
								  (uint64_t)0x80000000)));
		this->SetLo(this->GetLo() & (uint32_t)0xffffff00);
		this->SetHi(r_hi);
	}
	
	void Register::LoadExp(const Register& reg)
	{
		if (reg.GetHi() == 0x80)
			this->SetLo(0);
		this->SetHi(reg.GetHi());
	}
	
	void Register::LoadExp32(uint32_t value)
	{
		if (GetHi32(value) == 0x80)
			this->SetLo(0);
		this->SetHi(GetHi32(value));
	}
	
	void Register::LoadExp16(uint16_t value)
	{
		if (GetHi16(value) == 0x80)
			this->SetLo(0);
		this->SetHi(GetHi16(value));
	}
	
	void Register::LoadMan(const Register& reg)
	{
		this->SetLo(reg.GetLo());
	}
	
	void Register::LoadMan32(uint32_t value)
	{
		// TOCHECK: the documentation states that the memory value
		//   doesn't need to be interpreted as a float, but that all
		//   the value is loaded into the register mantissa (see LDM instruction)
		this->SetLo(value);
	}
	
	void Register::LoadMan16(uint16_t value)
	{
		this->SetLo(GetLo16(value));
	}
	
	RegisterDebugInterface::RegisterDebugInterface(const char *_name, unisim::component::cxx::processor::tms320c3x::Register *_reg, bool _extended_precision)
	: name(_name)
	, extended_precision(_extended_precision)
	, reg(_reg)
	{
	}
	
	RegisterDebugInterface::~RegisterDebugInterface()
	{
	}
	
	const char *RegisterDebugInterface::GetName() const
	{
		return name.c_str();
	}
	
	void RegisterDebugInterface::GetValue(void *buffer) const
	{
		uint32_t lo = reg->GetLo();
		
		if(extended_precision)
		{
			// Packing lo and hi into the buffer
			uint8_t hi = reg->GetHi();
#if BYTE_ORDER == LITTLE_ENDIAN
			memcpy(buffer, &lo, sizeof(lo));
			memcpy((char *) buffer + sizeof(lo), &hi, sizeof(hi));
#else
			memcpy(buffer, &hi, sizeof(hi));
			memcpy((char *) buffer + sizeof(hi), &lo, sizeof(lo));
#endif
		}
		else
		{
			memcpy(buffer, &lo, sizeof(lo));
		}
	}
	
	void RegisterDebugInterface::SetValue(const void *buffer)
	{
		uint32_t lo;
		
		if(extended_precision)
		{
			uint8_t hi;
			// Unpacking the buffer into lo and hi
#if BYTE_ORDER == LITTLE_ENDIAN
			memcpy(&lo, buffer, sizeof(lo));
			memcpy(&hi, (char *) buffer + sizeof(lo), sizeof(hi));
#else
			memcpy(&hi, buffer, sizeof(hi));
			memcpy(&lo, (char *) buffer + sizeof(hi), sizeof(lo));
#endif
			reg->SetHi(hi);
		}
		else
		{
			memcpy(&lo, buffer, sizeof(lo));
		}
		
		reg->SetLo(lo);
	}
	
	int RegisterDebugInterface::GetSize() const
	{
		return extended_precision ? sizeof(uint32_t) + sizeof(uint8_t) : sizeof(uint32_t);
	}
	
	RegisterBitFieldDebugInterface::RegisterBitFieldDebugInterface(const char *_name, unisim::component::cxx::processor::tms320c3x::Register *_reg, unsigned int _bit_offset, unsigned int _bit_size)
	: name(_name)
	, reg(_reg)
	, bit_offset(_bit_offset)
	, bit_size(_bit_size > 32 ? 32 : _bit_size)
	{
	}
	
	RegisterBitFieldDebugInterface::~RegisterBitFieldDebugInterface()
	{
	}
	
	const char *RegisterBitFieldDebugInterface::GetName() const
	{
		return name.c_str();
	}
	
	void RegisterBitFieldDebugInterface::GetValue(void *buffer) const
	{
		uint64_t value40 = ((uint64_t) reg->GetHi() << 32) | reg->GetLo();
		uint32_t value = (value40 >> bit_offset) & ((1 << bit_size) - 1);
		
		if(bit_size <= 8)
			*(uint8_t *) buffer = value; 
		else if(bit_size <= 16)
			*(uint16_t *) buffer = value; 
		else
			*(uint32_t *) buffer = value;
	}
	
	void RegisterBitFieldDebugInterface::SetValue(const void *buffer)
	{
		uint32_t value;
		
		if(bit_size <= 8)
			value = *(uint8_t *) buffer; 
		else if(bit_size <= 16)
			value = *(uint16_t *) buffer; 
		else
			value = *(uint32_t *) buffer;
			
		uint64_t value40 = ((uint64_t) reg->GetHi() << 32) | reg->GetLo();
		uint64_t mask = (((1 << bit_size) - 1) << bit_offset);
		value40 = (value40 & ~mask) | ((value << bit_offset) & mask);
		reg->SetLo(value40);
		reg->SetHi(value40 >> 32);
		
		//reg->SetLo((reg->GetLo() & ~(((1 << bit_size) - 1) << bit_offset)) | (value << bit_offset));
	}
	
	int RegisterBitFieldDebugInterface::GetSize() const
	{
		return (bit_size + 7) / 8;
	}
	
} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

