/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/cxx/processor/hcs12x/ccr.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

const char *CCR_t::GetName() const
{
	return ("CCR");
}

void CCR_t::GetValue(void *buffer) const
{
	*(uint16_t *) buffer = *ccrReg;
}

void CCR_t::SetValue(const void *buffer)
{
	*ccrReg = *(uint16_t *) buffer;
}

int CCR_t::GetSize() const
{
	return (2);
}

unisim::service::interfaces::Register *CCR_t::GetLowRegister()
{
	return (new unisim::util::debug::SimpleRegister<uint8_t>("CCRL",
#if BYTE_ORDER == BIG_ENDIAN
            ((uint8_t *) ccrReg) + 1
#else
            ((uint8_t *) ccrReg)
#endif
        ));
}

unisim::service::interfaces::Register *CCR_t::GetHighRegister()
{
	return (new unisim::util::debug::SimpleRegister<uint8_t>("CCRH",
#if BYTE_ORDER == BIG_ENDIAN
            ((uint8_t *) ccrReg)
#else
            ((uint8_t *) ccrReg) + 1
#endif
        ));
}

// ****************************************

TimeBaseRegisterView::TimeBaseRegisterView(const char *name, unisim::kernel::Object *owner, uint16_t& _storage, TimeBaseRegisterView::Type _type, const char *description)
	: unisim::kernel::VariableBase(name, owner, unisim::kernel::VariableBase::VAR_REGISTER, description)
	, storage(_storage)
	, type(_type)
{
}

TimeBaseRegisterView::~TimeBaseRegisterView()
{
}

const char *TimeBaseRegisterView::GetDataTypeName() const
{
	return ("unsigned 16-bit integer");
}

unsigned int TimeBaseRegisterView::GetBitSize() const { return (8); }

TimeBaseRegisterView::operator bool () const
{
	return ((bool) (type == TB_LOW) ? (uint8_t) storage : (uint8_t)(storage >> 8));
}

TimeBaseRegisterView::operator long long () const
{
	return ((long long) (type == TB_LOW) ? (uint8_t) storage : (uint32_t)(storage >> 8));
}

TimeBaseRegisterView::operator unsigned long long () const
{
	return ((unsigned long long) (type == TB_LOW) ? (uint8_t) storage : (uint8_t)(storage >> 8));
}

TimeBaseRegisterView::operator double () const
{
	return ((double) (type == TB_LOW) ? (uint8_t) storage : (uint8_t)(storage >> 8));
}

TimeBaseRegisterView::operator std::string () const
{
	uint8_t value = (type == TB_LOW) ? (uint8_t) storage : (uint8_t)(storage >> 8);
	std::stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x";
			sstr.fill('0');
			sstr.width(2);
			sstr << std::hex << (unsigned int) value;

			break;
		case FMT_DEC:
			sstr << std::dec << (unsigned int) value;
			break;
	}
	return (sstr.str());
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (bool value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xff00ULL) | (uint16_t) value
								: (storage & 0x00ffULL) | ((uint16_t) value << 8);
		NotifyListeners();
	}
	return (*this);
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (long long value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xff00ULL) | (uint16_t) value
								: (storage & 0x00ffULL) | ((uint16_t) value << 8);
		NotifyListeners();
	}
	return (*this);
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xff00ULL) | (uint16_t) value
								: (storage & 0x00ffULL) | ((uint16_t) value << 8);
		NotifyListeners();
	}
	return (*this);
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (double value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xff00ULL) | (uint16_t) value
								: (storage & 0x00ffULL) | ((uint16_t) value << 8);
		NotifyListeners();
	}
	return (*this);
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (const char * value)
{
	if(IsMutable())
	{
		uint8_t v = (uint8_t) (strcmp(value, "true") == 0) ? 1 : ((strcmp(value, "false") == 0) ? 0 : strtoull(value, 0, 0));
		storage = (type == TB_LOW) ? (storage & 0xff00ULL) | (uint16_t) v
								: (storage & 0x00ffULL) | ((uint16_t) v << 8);
		NotifyListeners();
	}
	return (*this);
}


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

