/*
 *  Copyright (c) 2016,
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

#ifndef __UNISIM_UTIL_REG_DEV_REGISTER_HH__
#define __UNISIM_UTIL_REG_DEV_REGISTER_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/register.hh>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace reg {
namespace dev {

using unisim::util::endian::endian_type;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;

enum Access
{
	ACC_NONE = 0,
	ACC_RO   = 1,
	ACC_WO   = 2,
	ACC_RW   = ACC_RO | ACC_WO
};

enum Control
{
	CTRL_DEFAULT = 0,
	CTRL_MASK_HW_R = 1,
	CTRL_MASK_HW_W = 2,
	CTRL_MASK_HW_RW = CTRL_MASK_HW_R | CTRL_MASK_HW_W,
	CTRL_MASK_SW_R = 4,
	CTRL_MASK_SW_W = 8,
	CTRL_MASK_SW_RW = CTRL_MASK_SW_R | CTRL_MASK_SW_W,
	CTRL_MASK_HW_SW_RW = CTRL_MASK_HW_RW | CTRL_MASK_SW_RW,
	CTRL_CHECK_HW_W = 16 | CTRL_MASK_HW_W,
	CTRL_CHECK_SW_R = 32 | CTRL_MASK_SW_R,
	CTRL_CHECK_SW_W = 64 | CTRL_MASK_SW_W,
	CTRL_CHECK_HW_SW_W = CTRL_CHECK_HW_W | CTRL_CHECK_SW_W,
	CTRL_CHECK_SW_RW = CTRL_CHECK_SW_R | CTRL_CHECK_SW_W,
	CTRL_CHECK_HW_W_SW_RW = CTRL_CHECK_HW_W | CTRL_CHECK_SW_RW
};

template <int BYTE_SIZE> struct TypeFor {};
template <> struct TypeFor<1> { typedef uint8_t TYPE; };
template <> struct TypeFor<2> { typedef uint16_t TYPE; };
template <> struct TypeFor<3> { typedef uint32_t TYPE; };
template <> struct TypeFor<4> { typedef uint32_t TYPE; };
template <> struct TypeFor<5> { typedef uint64_t TYPE; };
template <> struct TypeFor<6> { typedef uint64_t TYPE; };
template <> struct TypeFor<7> { typedef uint64_t TYPE; };
template <> struct TypeFor<8> { typedef uint64_t TYPE; };

template <int BIT_SIZE>
struct Storage
{
	static const int BYTE_SIZE = (BIT_SIZE + 7) / 8;
	typedef typename TypeFor<BYTE_SIZE>::TYPE TYPE;
	static const TYPE MASK = (BIT_SIZE == (8 * BYTE_SIZE)) ? TYPE(-1) : (TYPE(1) << BIT_SIZE) - 1;
	TYPE value;

	Storage() : value() {}
	Storage(TYPE initial_value) : value(initial_value & MASK) {}
};

class RegisterBase
{
public:
	inline RegisterBase(const std::string& name, const std::string& friendly_name = std::string());
	
	inline void SetDescription(const std::string& description);
	inline const std::string& GetName() const;
	inline const std::string& GetFriendlyName() const;
	inline const std::string& GetDescription() const;
	virtual unsigned int GetBitSize() const = 0;
	virtual bool Write(unsigned char *data_ptr, unsigned char *byte_enable) = 0;
	virtual bool Read(unsigned char *data_ptr, unsigned char *byte_enable) = 0;
	virtual void DebugWrite(unsigned char *data_ptr) = 0;
	virtual void DebugRead(unsigned char *data_ptr) = 0;

	inline unisim::service::interfaces::Register *CreateRegisterInterface();
private:
	std::string name;
	std::string friendly_name;
	std::string description;
};

RegisterBase::RegisterBase(const std::string& _name, const std::string& _friendly_name)
	: name(_name)
	, friendly_name(_friendly_name.empty() ? _name : _friendly_name)
{
}

void RegisterBase::SetDescription(const std::string& _description)
{
	description = _description;
}

const std::string& RegisterBase::GetName() const
{
	return name;
}

const std::string& RegisterBase::GetFriendlyName() const
{
	return friendly_name;
}

const std::string& RegisterBase::GetDescription() const
{
	return description;
}

unisim::service::interfaces::Register *RegisterBase::CreateRegisterInterface()
{
	struct RegisterInterface : public unisim::service::interfaces::Register
	{
		RegisterInterface(RegisterBase *_reg) : reg(_reg) {}
		virtual const char *GetName() const { return reg->GetName().c_str(); }
		virtual void GetValue(void *buffer) const { reg->DebugRead((unsigned char *) buffer); }
		virtual void SetValue(const void *buffer) { reg->DebugWrite((unsigned char *) buffer); }
		virtual int GetSize() const { return (reg->GetBitSize() + 7) / 8; }
		
		RegisterBase *reg;
	};
	
	return new RegisterInterface(this);
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
class Register;

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
std::ostream& operator << (std::ostream& os, const Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>& reg);

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL = CTRL_CHECK_HW_W_SW_RW>
class Register : public RegisterBase
{
public:
	typedef typename Storage<B_SIZE>::TYPE STORAGE_TYPE;
	static const int BIT_SIZE = B_SIZE;
	static const endian_type BIT_ORDERING = B_ORDER;
	Register(const std::string& name, const std::string& friendly_name = std::string());
	Register(const std::string& name, const STORAGE_TYPE& reset_value);
	Register(const std::string& name, const std::string& friendly_name, const STORAGE_TYPE& reset_value);
	virtual ~Register();
	
	virtual unsigned int GetBitSize() const;
	void SetHW_WriteMask(STORAGE_TYPE mask);
	void SetHW_ReadMask(STORAGE_TYPE mask);
	void SetSW_WriteMask(STORAGE_TYPE mask);
	void SetSW_ReadMask(STORAGE_TYPE mask);
	STORAGE_TYPE GetHW_WriteMask() const;
	STORAGE_TYPE GetHW_ReadMask() const;
	STORAGE_TYPE GetSW_WriteMask() const;
	STORAGE_TYPE GetSW_ReadMask() const;
	void SetResetValue(STORAGE_TYPE value, STORAGE_TYPE mask);
	void Reset();
	operator const STORAGE_TYPE() const;
	Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>& operator = (const STORAGE_TYPE& value);
	virtual bool CheckWrite(const STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable);
	virtual bool CheckRead(STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable);
	virtual bool Write(const STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable = STORAGE_TYPE(-1));
	virtual bool Read(STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable = STORAGE_TYPE(-1));
	virtual void DebugWrite(const STORAGE_TYPE& value);
	virtual void DebugRead(STORAGE_TYPE& value);
	virtual bool Write(unsigned char *data_ptr, unsigned char *byte_enable);
	virtual bool Read(unsigned char *data_ptr, unsigned char *byte_enable);
	virtual void DebugWrite(unsigned char *data_ptr);
	virtual void DebugRead(unsigned char *data_ptr);
	
	friend std::ostream& operator << <B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>(std::ostream& os, const Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>& reg);
private:
	Storage<B_SIZE> storage;
	Storage<B_SIZE> reset_storage;
	Storage<B_SIZE> hw_write_mask_storage;
	Storage<B_SIZE> hw_read_mask_storage;
	Storage<B_SIZE> sw_write_mask_storage;
	Storage<B_SIZE> sw_read_mask_storage;
};

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
std::ostream& operator << (std::ostream& os, const Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>& reg)
{
	return os << (uint64_t) reg.storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Register(const std::string& name, const std::string& friendly_name)
	: RegisterBase(name, friendly_name)
	, storage()
	, reset_storage()
	, hw_write_mask_storage(0)
	, hw_read_mask_storage(0)
	, sw_write_mask_storage(0)
	, sw_read_mask_storage(0)
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Register(const std::string& name, const std::string& friendly_name, const STORAGE_TYPE& reset_value)
	: RegisterBase(name, friendly_name)
	, storage(reset_value)
	, reset_storage(reset_value)
	, hw_write_mask_storage(0)
	, hw_read_mask_storage(0)
	, sw_write_mask_storage(0)
	, sw_read_mask_storage(0)
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Register(const std::string& name, const STORAGE_TYPE& reset_value)
	: RegisterBase(name)
	, storage(reset_value)
	, reset_storage(reset_value)
	, hw_write_mask_storage(0)
	, hw_read_mask_storage(0)
	, sw_write_mask_storage(0)
	, sw_read_mask_storage(0)
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::~Register()
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
unsigned int Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::GetBitSize() const
{
	return B_SIZE;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::SetHW_WriteMask(STORAGE_TYPE mask)
{
	hw_write_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::SetHW_ReadMask(STORAGE_TYPE mask)
{
	hw_read_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::SetSW_WriteMask(STORAGE_TYPE mask)
{
	sw_write_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::SetSW_ReadMask(STORAGE_TYPE mask)
{
	sw_read_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::GetHW_WriteMask() const
{
	return hw_write_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::GetHW_ReadMask() const
{
	return hw_read_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::GetSW_WriteMask() const
{
	return sw_write_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::GetSW_ReadMask() const
{
	return sw_read_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::SetResetValue(STORAGE_TYPE value, STORAGE_TYPE mask)
{
	reset_storage.value = ((reset_storage.value & ~mask) | (value & mask)) & reset_storage.MASK;
	Reset();
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Reset()
{
	storage.value = reset_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::operator const STORAGE_TYPE() const
{
	return (CTRL & CTRL_MASK_HW_R) ? (((reset_storage.value & ~hw_read_mask_storage.value) | (storage.value & hw_read_mask_storage.value)) & storage.MASK) : storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>& Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::operator = (const STORAGE_TYPE& value)
{
	if(CTRL & CTRL_MASK_HW_W)
	{
		if(CTRL & CTRL_CHECK_HW_W)
		{
			if(((reset_storage.value & hw_write_mask_storage.value) | (value & ~hw_write_mask_storage.value & storage.MASK)) != reset_storage.value)
			{
				// Attempt to modify reserved or read-only bits
				std::cerr << "Hardware attempt to modify reserved bits in Register \"" << GetName() << "\"" << std::endl;
			}
		}

		storage.value = ((storage.value & ~hw_write_mask_storage.value) | (value & hw_write_mask_storage.value)) & storage.MASK;
	}
	else
	{
		storage.value = value;
	}
	return *this;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::CheckWrite(const STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable)
{
	if(SW_ACCESS & ACC_WO)
	{
		STORAGE_TYPE old_value = this->operator const STORAGE_TYPE();
		
		if((value & byte_enable & ~sw_write_mask_storage.value) != (old_value & byte_enable & ~sw_write_mask_storage.value)) // is write would modify read-only bits ?
		{
			// yes: attempt to modify reserved bits
			std::cerr << "Software attempt to modify read-only bits in Register \"" << GetName() << "\" (from value 0x" << std::hex << (unsigned long long) old_value << " to value 0x" << (unsigned long long) value << std::dec << ")" << std::endl;
		}
		
		return true;
	}
	
	std::cerr << "Software attempt to modify read-only Register \"" << GetName() << "\"" << std::endl;
	return false;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::CheckRead(STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable)
{
	if(SW_ACCESS & ACC_RO)
	{
		return true;
	}
	
	std::cerr << "Software attempt to read write-only Register \"" << GetName() << "\"" << std::endl;
	
	return false;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Write(const STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable)
{
	if(CTRL & CTRL_MASK_SW_W)
	{
		if(!(CTRL & CTRL_CHECK_SW_W) || CheckWrite(value, byte_enable))
		{
			STORAGE_TYPE old_value = this->operator const STORAGE_TYPE();
			STORAGE_TYPE write_mask = byte_enable & sw_write_mask_storage.value;
			STORAGE_TYPE new_value = (old_value & ~write_mask) | (value & write_mask);
			this->operator = (new_value);
			return true;
		}
	
		return false;
	}
	
	STORAGE_TYPE old_value = this->operator const STORAGE_TYPE();
	STORAGE_TYPE new_value = (old_value & ~byte_enable) | (value & byte_enable);
	this->operator = (new_value);
	return true;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Read(STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable)
{
	if(CTRL & CTRL_MASK_SW_R)
	{
		if(!(CTRL & CTRL_CHECK_SW_R) || CheckRead(value, byte_enable))
		{
			STORAGE_TYPE reg_value = this->operator const STORAGE_TYPE();
			value = (value & ~byte_enable) | (reg_value & sw_read_mask_storage.value & byte_enable);
			return true;
		}
	
		return false;
	}
	
	STORAGE_TYPE reg_value = this->operator const STORAGE_TYPE();
	value = (value & ~byte_enable) | (reg_value & byte_enable);
	return true;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::DebugWrite(const STORAGE_TYPE& value)
{
	this->operator = (value);
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::DebugRead(STORAGE_TYPE& value)
{
	value = this->operator const STORAGE_TYPE();
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Write(unsigned char *data_ptr, unsigned char *byte_enable_ptr)
{
	STORAGE_TYPE *data_to_write = (STORAGE_TYPE *) data_ptr;
	STORAGE_TYPE *byte_enable = (STORAGE_TYPE *) byte_enable_ptr;
	return Write(*data_to_write, *byte_enable);
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::Read(unsigned char *data_ptr, unsigned char *byte_enable_ptr)
{
	STORAGE_TYPE *read_data = (STORAGE_TYPE *) data_ptr;
	STORAGE_TYPE *byte_enable = (STORAGE_TYPE *) byte_enable_ptr;
	return Read(*read_data, *byte_enable);
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::DebugWrite(unsigned char *data_ptr)
{
	STORAGE_TYPE *data_to_write = (STORAGE_TYPE *) data_ptr;
	DebugWrite(*data_to_write);
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, endian_type B_ORDER, Control CTRL>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER, CTRL>::DebugRead(unsigned char *data_ptr)
{
	STORAGE_TYPE *read_data = (STORAGE_TYPE *) data_ptr;
	DebugRead(*read_data);
}

class BitFieldBase
{
public:
	inline BitFieldBase(RegisterBase *reg, const std::string& name, const std::string& friendly_name = std::string());
	inline virtual ~BitFieldBase();
	
	inline void SetDescription(const std::string& description);
	inline const std::string& GetName() const;
	inline const std::string& GetFriendlyName() const;
	inline const std::string& GetDescription() const;
private:
	std::string name;
	std::string friendly_name;
	std::string description;
};

BitFieldBase::BitFieldBase(RegisterBase *reg, const std::string& _name, const std::string& _friendly_name)
	: name(reg->GetName() + '.' + _name)
	, friendly_name(_friendly_name.empty() ? name : _friendly_name)
{
}

BitFieldBase::~BitFieldBase()
{
}

void BitFieldBase::SetDescription(const std::string& _description)
{
	description = _description;
}

const std::string& BitFieldBase::GetName() const
{
	return name;
}

const std::string& BitFieldBase::GetFriendlyName() const
{
	return friendly_name;
}

const std::string& BitFieldBase::GetDescription() const
{
	return description;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
class BitField;

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
std::ostream& operator << (std::ostream& os, const BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& bf);

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2 = -1>
class BitField : public BitFieldBase
{
public:
	typedef typename REGISTER::STORAGE_TYPE STORAGE_TYPE;
	static const endian_type BIT_ORDERING = REGISTER::BIT_ORDERING;

	BitField(REGISTER *reg, const std::string& name);
	BitField(REGISTER *reg, const std::string& name, const std::string& friendly_name);
	BitField(REGISTER *reg, const std::string& name, const STORAGE_TYPE& reset_value);
	BitField(REGISTER *reg, const std::string& name, const std::string& friendly_name, const STORAGE_TYPE& reset_value);
	virtual ~BitField();
	
	inline operator const STORAGE_TYPE () const;
	inline BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& operator = (const STORAGE_TYPE& value);
	inline unsigned int Shift() const;
	inline unsigned int BitSize() const;
	inline STORAGE_TYPE Mask() const;
	inline STORAGE_TYPE HW_WriteMask() const;
	inline STORAGE_TYPE HW_ReadMask() const;
	inline STORAGE_TYPE SW_WriteMask() const;
	inline STORAGE_TYPE SW_ReadMask() const;
private:
	REGISTER *reg;
	
	friend std::ostream& operator << <REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>(std::ostream& os, const BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& bf);
};

template <bool COND>
struct CompileTimeAssert
{
};

template <>
struct CompileTimeAssert<true>
{
	static void _() {}
};

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
std::ostream& operator << (std::ostream& os, const BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& bf)
{
	return os << (uint64_t) bf.operator const typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE();
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitField(REGISTER *_reg, const std::string& _name)
	: BitFieldBase(_reg, _name)
	, reg(_reg)
{
	CompileTimeAssert<(OFFSET1 >= 0) && (OFFSET1 < (int) REGISTER::BIT_SIZE)>::_();
	CompileTimeAssert<OFFSET2 < REGISTER::BIT_SIZE>::_();
	
	reg->SetHW_WriteMask(HW_WriteMask());
	reg->SetHW_ReadMask(HW_ReadMask());
	reg->SetSW_WriteMask(SW_WriteMask());
	reg->SetSW_ReadMask(SW_ReadMask());
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitField(REGISTER *_reg, const std::string& _name, const std::string& _friendly_name)
	: BitFieldBase(_reg, _name, _friendly_name)
	, reg(_reg)
{
	CompileTimeAssert<(OFFSET1 >= 0) && (OFFSET1 < (int) REGISTER::BIT_SIZE)>::_();
	CompileTimeAssert<OFFSET2 < REGISTER::BIT_SIZE>::_();
	
	reg->SetHW_WriteMask(HW_WriteMask());
	reg->SetHW_ReadMask(HW_ReadMask());
	reg->SetSW_WriteMask(SW_WriteMask());
	reg->SetSW_ReadMask(SW_ReadMask());
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitField(REGISTER *_reg, const std::string& _name, const STORAGE_TYPE& reset_value)
	: BitFieldBase(_reg, _name)
	, reg(_reg)
{
	CompileTimeAssert<(OFFSET1 >= 0) && (OFFSET1 < (int) REGISTER::BIT_SIZE)>::_();
	CompileTimeAssert<OFFSET2 < REGISTER::BIT_SIZE>::_();
	
	reg->SetHW_WriteMask(HW_WriteMask());
	reg->SetHW_ReadMask(HW_ReadMask());
	reg->SetSW_WriteMask(SW_WriteMask());
	reg->SetSW_ReadMask(SW_ReadMask());
	reg->SetResetValue(reset_value << Shift(), Mask());
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitField(REGISTER *_reg, const std::string& _name, const std::string& _friendly_name, const STORAGE_TYPE& reset_value)
	: BitFieldBase(_reg, _name, _friendly_name)
	, reg(_reg)
{
	CompileTimeAssert<(OFFSET1 >= 0) && (OFFSET1 < (int) REGISTER::BIT_SIZE)>::_();
	CompileTimeAssert<OFFSET2 < REGISTER::BIT_SIZE>::_();
	
	reg->SetHW_WriteMask(HW_WriteMask());
	reg->SetHW_ReadMask(HW_ReadMask());
	reg->SetSW_WriteMask(SW_WriteMask());
	reg->SetSW_ReadMask(SW_ReadMask());
	reg->SetResetValue(reset_value << Shift(), Mask());
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::~BitField()
{
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::operator const STORAGE_TYPE () const
{
	STORAGE_TYPE reg_value = reg->operator const STORAGE_TYPE();
	
	return (reg_value & HW_ReadMask()) >> Shift();
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::operator = (const STORAGE_TYPE& value)
{
	STORAGE_TYPE hw_write_mask = HW_WriteMask();
	STORAGE_TYPE reg_value = reg->operator const STORAGE_TYPE();
	STORAGE_TYPE ins_value = value << Shift();
	reg->operator = ((reg_value & ~hw_write_mask) | (ins_value & hw_write_mask));
	return *this;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline unsigned int BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::Shift() const
{
	if(OFFSET2 >= 0)
	{
		if(OFFSET1 >= OFFSET2)
		{
			switch(REGISTER::BIT_ORDERING)
			{
				case E_LITTLE_ENDIAN:
					return OFFSET2;
				case E_BIG_ENDIAN:
					return REGISTER::BIT_SIZE - 1 - OFFSET2;
				default:
					return 0;
			}
		}
		else
		{
			switch(REGISTER::BIT_ORDERING)
			{
				case E_LITTLE_ENDIAN:
					return OFFSET1;
				case E_BIG_ENDIAN:
					return REGISTER::BIT_SIZE - 1 - OFFSET1;
				default:
					return 0;
			}
		}
	}
	
	return OFFSET1;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline unsigned int BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitSize() const
{
	if(OFFSET2 >= 0)
	{
		if(OFFSET1 >= OFFSET2)
		{
			return OFFSET1 - OFFSET2 + 1;
		}
		else
		{
			return OFFSET2 - OFFSET1 + 1;
		}
	}
	
	return 1;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::Mask() const
{
	return (REGISTER::BIT_SIZE == BitSize()) ? STORAGE_TYPE(-1) : ((STORAGE_TYPE(1) << BitSize()) - 1) << Shift();
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::HW_WriteMask() const
{
	return (HW_ACCESS & ACC_WO) ? Mask() : 0;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::HW_ReadMask() const
{
	return (HW_ACCESS & ACC_RO) ? Mask() : 0;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::SW_WriteMask() const
{
	return (SW_ACCESS & ACC_WO) ? Mask() : 0;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
inline typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::SW_ReadMask() const
{
	return (SW_ACCESS & ACC_RO) ? Mask() : 0;
}

template <typename ADDRESS>
class RegisterAddressMap;

template <typename ADDRESS>
class AddressableRegister
{
public:
	AddressableRegister(ADDRESS addr, unsigned int size, RegisterBase *reg);
	
	ADDRESS GetAddress() const { return addr; }
	unsigned int GetSize() const { return size; }
	RegisterBase *GetRegister() const { return reg; }
	bool Write(unsigned char *data_ptr, unsigned char *byte_enable) { return reg->Write(data_ptr, byte_enable); }
	bool Read(unsigned char *data_ptr, unsigned char *byte_enable)  { return reg->Read(data_ptr, byte_enable); }
	void DebugWrite(unsigned char *data_ptr) { return reg->DebugWrite(data_ptr); }
	void DebugRead(unsigned char *data_ptr)  { return reg->DebugRead(data_ptr); }
private:
	friend class RegisterAddressMap<ADDRESS>;
	
	ADDRESS addr;
	unsigned int size;
	RegisterBase *reg;
	unsigned int ref_count;
	
	void Acquire();
	void Release();
};

template <typename ADDRESS>
AddressableRegister<ADDRESS>::AddressableRegister(ADDRESS _addr, unsigned int _size, RegisterBase *_reg)
	: addr(_addr)
	, size(_size)
	, reg(_reg)
	, ref_count(0)
{
}

template <typename ADDRESS>
void AddressableRegister<ADDRESS>::Acquire()
{
	ref_count++;
}

template <typename ADDRESS>
void AddressableRegister<ADDRESS>::Release()
{
	if(ref_count && (--ref_count == 0))
	{
		delete this;
	}
}

template <typename ADDRESS>
class RegisterAddressMap
{
public:
	RegisterAddressMap();
	virtual ~RegisterAddressMap();
	void MapRegister(ADDRESS addr, RegisterBase *r);
	AddressableRegister<ADDRESS> *FindRegister(ADDRESS addr) const;
private:
	mutable bool optimized;
	mutable bool optimizable;
	mutable std::pair<ADDRESS, ADDRESS> addr_range;
	mutable std::vector<AddressableRegister<ADDRESS> *> opt_reg_addr_map;
	std::map<ADDRESS, AddressableRegister<ADDRESS> *> reg_addr_map;

	void Optimize() const;
};

template <typename ADDRESS>
RegisterAddressMap<ADDRESS>::RegisterAddressMap()
	: optimized(true)
	, optimizable(true)
	, addr_range(ADDRESS(-1), ADDRESS(0))
	, opt_reg_addr_map()
	, reg_addr_map()
{
}

template <typename ADDRESS>
RegisterAddressMap<ADDRESS>::~RegisterAddressMap()
{
	typename std::map<ADDRESS, AddressableRegister<ADDRESS> *>::const_iterator it;

	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		AddressableRegister<ADDRESS> *ar = (*it).second;
		
		if(ar)
		{
			ar->Release();
		}
	}
}

template <typename ADDRESS>
void RegisterAddressMap<ADDRESS>::MapRegister(ADDRESS addr, RegisterBase *r)
{
	unsigned int reg_bit_size = r->GetBitSize();
	unsigned int reg_byte_size = (reg_bit_size + 7) / 8;

	unsigned int byte_idx;

	if(reg_byte_size)
	{
		optimized = false;
		optimizable = true;
		
		AddressableRegister<ADDRESS> *ar = new AddressableRegister<ADDRESS>(addr, reg_byte_size, r);
		
		for(byte_idx = 0; byte_idx < reg_byte_size; byte_idx++)
		{
			ADDRESS byte_addr = addr + byte_idx;
			
			// check that byte is not already mapped
			typename std::map<ADDRESS, AddressableRegister<ADDRESS> *>::const_iterator it = reg_addr_map.find(byte_addr);
			
			if(it == reg_addr_map.end())
			{
				ar->Acquire();
				reg_addr_map.insert(std::pair<ADDRESS, AddressableRegister<ADDRESS> *>(byte_addr, ar));
			}
		}
	}
}

template <typename ADDRESS>
void RegisterAddressMap<ADDRESS>::Optimize() const
{
	opt_reg_addr_map.clear();
	
	if(reg_addr_map.empty())
	{
		optimizable = true;
		optimized = true;
		return;
	}
	
	ADDRESS num_addr_holes = 0;
	addr_range.first = ADDRESS(-1);
	addr_range.second = 0;

	typename std::map<ADDRESS, AddressableRegister<ADDRESS> *>::const_iterator it;
	ADDRESS next_addr = 1;

	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		ADDRESS addr = (*it).first;
		if(addr < addr_range.first) addr_range.first = addr;
		if(addr > addr_range.second) addr_range.second = addr;
		if(addr != addr_range.first)
		{
			if(addr > next_addr)
			{
				num_addr_holes += addr - next_addr;
			}
		}
		next_addr = addr + 1;
	}

	if((addr_range.first == 0) && (addr_range.second == ADDRESS(-1)))
	{
		optimizable = false;
		optimized = false;
		return; // whole address space: unoptimizable
	}

	ADDRESS threshold_range = (addr_range.second - addr_range.first + 1) / 2;
	
	if(num_addr_holes > threshold_range)
	{
		optimizable = false;
		optimized = false;
		return; // not reasonably optimizable
	}
	   
	opt_reg_addr_map.resize(addr_range.second - addr_range.first + 1);
	
	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		ADDRESS addr = (*it).first;
		AddressableRegister<ADDRESS> *ar = (*it).second;
		opt_reg_addr_map[addr - addr_range.first] = ar;
	}
	
	optimizable = true;
	optimized = true;
}

template <typename ADDRESS>
AddressableRegister<ADDRESS> *RegisterAddressMap<ADDRESS>::FindRegister(ADDRESS addr) const
{
	if(optimized)
	{
		return opt_reg_addr_map.size() ? opt_reg_addr_map[addr - addr_range.first] : 0;
	}
	
	if(optimizable)
	{
		Optimize();
	}
	
	typename std::map<ADDRESS, AddressableRegister<ADDRESS> *>::const_iterator it = reg_addr_map.find(addr);

	return it != reg_addr_map.end() ? (*it).second : 0;
}

} // end of namespace dev
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_REG_DEV_REGISTER_HH__
