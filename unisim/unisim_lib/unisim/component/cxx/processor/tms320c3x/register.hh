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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_REGISTER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_REGISTER_HH__

#include <inttypes.h>
#include <iosfwd>
#include <string>
#include <unisim/service/interfaces/register.hh>
#include <unisim/util/inlining/inlining.hh>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320c3x {
	
	class Register
	{
	public:
		Register();
		
		inline Register& operator=(const Register& reg) ALWAYS_INLINE;
		inline void SetLo(uint32_t value) ALWAYS_INLINE;
		inline uint32_t GetLo() const ALWAYS_INLINE;
		inline void SetHi(uint8_t value) ALWAYS_INLINE;
		inline uint8_t GetHi() const ALWAYS_INLINE;
		inline uint32_t IsNeg() const ALWAYS_INLINE;
		void SetFromSinglePrecisionFPFormat(uint32_t value);
		void SetFromShortFPFormat(uint16_t value);
		uint32_t GetSinglePrecisionFPFormat();
		void Float(uint32_t value, uint32_t& neg);
		uint32_t Fix(uint32_t& overflow, uint32_t& neg);
		void Absf(const Register& reg, uint32_t& overflow);
		void Absf16(uint16_t imm, uint32_t& overflow);
		void Absf32(uint32_t imm, uint32_t& overflow);
		void Addf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Addf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Addf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Addf(const Register& reg_a, const Register& reg_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Addf(const Register& reg, uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Addf(uint32_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Addf(uint32_t imm_a, uint32_t imm_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf(const Register& reg_a, const Register& reg_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf16(const Register& reg, uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf32(const Register& reg, uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf16(uint16_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf32(uint32_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf(uint32_t imm_a, uint32_t imm_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Negf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Negf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Negf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Normf(const Register& reg, uint32_t& underflow, uint32_t& neg);
		void Normf16(uint16_t imm, uint32_t& underflow, uint32_t& neg);
		void Normf32(uint32_t imm, uint32_t& underflow, uint32_t& neg);
		void Mpyf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf(const Register& reg_a, const Register& reg_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf(const Register& reg, uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf(uint32_t imm, const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf(uint32_t imm_a, uint32_t imm_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Rndf(const Register& reg, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Rndf16(uint16_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Rndf32(uint32_t imm, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void LoadExp(const Register& reg);
		void LoadExp32(uint32_t value);
		void LoadExp16(uint16_t value);
		void LoadMan(const Register& reg);
		void LoadMan32(uint32_t value);
		void LoadMan16(uint16_t value);
		
		friend std::ostream& operator << (std::ostream& os, const Register& reg);
		
		void SetLoWriteMask(uint32_t lo_write_mask);
		void SetHiWriteMask(uint8_t hi_write_mask, uint8_t init_value = 0);

		static uint32_t GetLo16(uint16_t value) ;
		static uint8_t GetHi16(uint16_t value) ;
		static uint32_t GetLo32(uint32_t value) ;
		static uint8_t GetHi32(uint32_t value) ;

	private:
		uint32_t lo_write_mask; // write mask for the 32 LSBs
		uint32_t lo; // 32 LSB
		uint8_t hi_write_mask; // write mask for the 8 MSBs
		uint8_t hi;  // 8 MSB
		
		uint32_t GetLo(uint16_t value) ;
		uint8_t GetHi(uint16_t value) ;
		uint32_t GetLo(uint32_t value) ;
		uint8_t GetHi(uint32_t value) ;
		
		void Absf(uint8_t hi_a, uint32_t lo_a, uint32_t& overflow);
		void Addf(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Subf(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void AddfSubf(bool is_add, uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Mpyf(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
		void Normf(uint8_t hi_a, uint32_t lo_a, uint32_t& underflow, uint32_t& neg);
		void Rndf(uint8_t hi_a, uint32_t lo_a, uint32_t& overflow, uint32_t& underflow, uint32_t& neg);
	};
	
	class RegisterDebugInterface : public unisim::service::interfaces::Register
	{
	public:
		RegisterDebugInterface(const char *name, unisim::component::cxx::processor::tms320c3x::Register *reg, bool extended_precision = false);
		virtual ~RegisterDebugInterface();
		virtual const char *GetName() const;
		virtual void GetValue(void *buffer) const;
		virtual void SetValue(const void *buffer);
		virtual int GetSize() const;
	private:
		std::string name;
		bool extended_precision;
		unisim::component::cxx::processor::tms320c3x::Register *reg;
	};
	
	class RegisterBitFieldDebugInterface : public unisim::service::interfaces::Register
	{
	public:
		RegisterBitFieldDebugInterface(const char *name, unisim::component::cxx::processor::tms320c3x::Register *reg, unsigned int bit_offset, unsigned int bit_size = 1);
		virtual ~RegisterBitFieldDebugInterface();
		virtual const char *GetName() const;
		virtual void GetValue(void *buffer) const;
		virtual void SetValue(const void *buffer);
		virtual int GetSize() const;
	private:
		std::string name;
		unisim::component::cxx::processor::tms320c3x::Register *reg;
		unsigned int bit_offset;
		unsigned int bit_size;
	};
	
	inline Register& Register::operator=(const Register& reg)
	{
		if (this != &reg)
		{
			this->SetHi(reg.GetHi());
			if (reg.GetHi() == (uint8_t)0x80)
				this->SetLo(0);
			else
				this->SetLo(reg.GetLo());
		}
		return *this;
	}
	
	inline void Register::SetLo(uint32_t value)
	{
		lo = value & lo_write_mask;
	}
	
	inline uint32_t Register::GetLo() const
	{
		return lo;
	}
	
	inline void Register::SetHi(uint8_t value)
	{
		hi = (hi & ~hi_write_mask) | (value & hi_write_mask);
	}
	
	inline uint8_t Register::GetHi() const
	{
		return hi;
	}
	
	inline uint32_t Register::IsNeg() const
	{
		if (lo >= (uint32_t)0x80000000) return 1;
		return 0;
	}
	
} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_REGISTER_HH__
