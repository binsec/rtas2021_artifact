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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_UTIL_REG_CORE_REGISTER_TCC__
#define __UNISIM_UTIL_REG_CORE_REGISTER_TCC__

#include <unisim/util/likely/likely.hh>
#include <limits.h>
#include <assert.h>
#include <stdexcept>

namespace unisim {
namespace util {
namespace reg {
namespace core {

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DEFINITIONS /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////////////////////////////// TypeForBitSize<> ///////////////////////////////

template <unsigned int SIZE> const unsigned int TypeForBitSize<SIZE>::BYTE_SIZE;
template <unsigned int SIZE> const typename TypeForBitSize<SIZE>::TYPE TypeForBitSize<SIZE>::MASK;

///////////////////////////// LongPrettyPrinter ///////////////////////////////

template <typename T>
void LongPrettyPrinter::PrintValue(std::ostream& os, const T& value)
{
	std::ios_base::fmtflags ff = os.flags();
	std::stringstream sstr_value;

	if(ff & os.hex) sstr_value << "0x";
	else if(ff & os.oct) sstr_value << "0";
	sstr_value.flags(ff);
	sstr_value << value;

	std::string str_value(sstr_value.str());

	Print(os, str_value, VALUE_PRINT_WIDTH);
}

/////////////////////////////////// Field /////////////////////////////////////

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::BITOFFSET;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::BITWIDTH;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const Access Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ACCESS;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::BITWIDTH_MINUS_1;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
const unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ID = FieldID();

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetMask()
{
	const bool legal = _BITWIDTH && (_BITOFFSET < (sizeof(T) * CHAR_BIT));
	return legal ? (((T(2) << (legal ? BITWIDTH_MINUS_1 : 0)) - T(1)) << (legal ? _BITOFFSET : 0)) : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetAssignMask()
{
	return (_ACCESS & AF_HW_W) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetWriteMask()
{
	return (_ACCESS & AF_SW_W) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetWriteOneClearMask()
{
	return (_ACCESS & AF_SW_W1C) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetReadMask()
{
	return (_ACCESS & AF_SW_R) ? GetMask<T>() : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::HasOverlappingBitFields()
{
	return false;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T> inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::Mask(const T& storage)
{
	return storage & GetMask<T>();
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::Get(const T& storage)
{
	const bool legal = _BITWIDTH && (_BITOFFSET < (sizeof(T) * CHAR_BIT));
	return legal ? ((storage & GetMask<T>()) >> (legal ? _BITOFFSET : 0)) : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::Set(T& storage, T field_value)
{
	const bool legal = _BITWIDTH && (_BITOFFSET < (sizeof(T) * CHAR_BIT));
	if(legal)
	{
		storage = (storage & ~GetAssignMask<T>()) | ((field_value << (legal ? _BITOFFSET : 0)) & GetAssignMask<T>());
	}
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline T Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::MakeValue(T field_value)
{
	const bool legal = _BITWIDTH && (_BITOFFSET < (sizeof(T) * CHAR_BIT));
	return legal ? ((field_value << (legal ? _BITOFFSET : 0)) & GetAssignMask<T>()) : 0;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::SetName(const std::string& name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_NAME, ID, name);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::SetDisplayName(const std::string& disp_name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DISP_NAME, ID, disp_name);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::SetDescription(const std::string& desc)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DESC, ID, desc);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetBitOffset()
{
	return _BITOFFSET;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline unsigned int Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetBitWidth()
{
	return _BITWIDTH;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline Access Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetAccess()
{
	return _ACCESS;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsReadable()
{
	return _ACCESS & AF_SW_R;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWritable()
{
	return _ACCESS & AF_SW_W;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWriteOnce()
{
	return _ACCESS & AF_SW_W1;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsReadOnly()
{
	return (_ACCESS & (AF_SW_R | AF_SW_W)) == AF_SW_R;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWriteOnly()
{
	return (_ACCESS & (AF_SW_R | AF_SW_W)) == AF_SW_W;
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline bool Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::IsWriteOnlyAndOnce()
{
	return (_ACCESS & (AF_SW_R | AF_SW_W | AF_SW_W1)) == (AF_SW_W | AF_SW_W1);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline const std::string& Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetName()
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_NAME, ID);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline const std::string& Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetDisplayName()
{
	const std::string& display_name = PropertyRegistry::GetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DISP_NAME, ID);
	if(!display_name.empty()) return display_name;
	return GetName();
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
inline const std::string& Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::GetDescription()
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_FIELD, PropertyRegistry::STR_PROP_DESC, ID);
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ShortPrettyPrint(std::ostream& os, const T& storage)
{
	const bool legal = _BITWIDTH && (_BITOFFSET < (sizeof(T) * CHAR_BIT));

	std::ios_base::fmtflags ff = os.flags();
	os << GetDisplayName();
	if(legal)
	{
		os << '[';
		os << std::dec;
		if(_BITWIDTH != 1)
		{
			os << (_BITOFFSET + BITWIDTH_MINUS_1);
			os << "..";
		}
		os << _BITOFFSET;
		os << ']';
		os << '=';
		if(_BITWIDTH > 1)
		{
			if(ff & os.hex) os << "0x";
			else if(ff & os.oct) os << "0";
		}
		os.flags(ff);
		os << Get<T>(storage);
	}
	else
	{
		os << "[]";
	}
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename T>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::LongPrettyPrint(std::ostream& os, const T& storage)
{
	const bool legal = _BITWIDTH && (_BITOFFSET < (sizeof(T) * CHAR_BIT));
	LongPrettyPrinter::PrintKind(os, "field");
	LongPrettyPrinter::PrintFieldRange(os, _BITOFFSET, legal ? _BITWIDTH : 0);
	LongPrettyPrinter::PrintDisplayName(os, GetDisplayName());
	LongPrettyPrinter::PrintValue(os, Get<T>(storage));
	LongPrettyPrinter::PrintAccess(os, _ACCESS);
	LongPrettyPrinter::PrintDescription(os, GetDescription());
}

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS>
template <typename VISITOR>
inline void Field<FIELD, _BITOFFSET, _BITWIDTH, _ACCESS>::ForEach(VISITOR& visitor)
{
	visitor.template operator()<FIELD>();
}

/////////////////////////////// MakeMask ///////////////////////////////////

template <typename T, unsigned int BITWIDTH> T MakeMask()
{
	return BITWIDTH ? (T(2) << (BITWIDTH - 1)) - 1 : 0;
}

/////////////////////////////// NullField //////////////////////////////////

template <typename T>
inline T NullField::GetMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetAssignMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetWriteMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetWriteOneClearMask()
{
	return 0;
}

template <typename T>
inline T NullField::GetReadMask()
{
	return 0;
}

template <typename T>
inline T NullField::Get(const T& storage)
{
	return T();
}

template <typename T>
inline void NullField::Set(T& storage, T field_value)
{
}

template <typename T>
inline void NullField::ShortPrettyPrint(std::ostream& os, const T& storage)
{
}

template <typename T>
inline void NullField::LongPrettyPrint(std::ostream& os, const T& storage)
{
}

template <typename VISITOR>
inline void NullField::ForEach(VISITOR& visitor)
{
}

//////////////////////////////// FieldSet //////////////////////////////////

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetMask()
{
	return  BF0::template GetMask<T>() |  BF1::template GetMask<T>() |  BF2::template GetMask<T>() |  BF3::template GetMask<T>() |  BF4::template GetMask<T>() |  BF5::template GetMask<T>() |  BF6::template GetMask<T>() |  BF7::template GetMask<T>()
	     |  BF8::template GetMask<T>() |  BF9::template GetMask<T>() | BF10::template GetMask<T>() | BF11::template GetMask<T>() | BF12::template GetMask<T>() | BF13::template GetMask<T>() | BF14::template GetMask<T>() | BF15::template GetMask<T>()
	     | BF16::template GetMask<T>() | BF17::template GetMask<T>() | BF18::template GetMask<T>() | BF19::template GetMask<T>() | BF20::template GetMask<T>() | BF21::template GetMask<T>() | BF22::template GetMask<T>() | BF23::template GetMask<T>()
	     | BF24::template GetMask<T>() | BF25::template GetMask<T>() | BF26::template GetMask<T>() | BF27::template GetMask<T>() | BF28::template GetMask<T>() | BF29::template GetMask<T>() | BF30::template GetMask<T>() | BF31::template GetMask<T>()
	     | BF32::template GetMask<T>() | BF33::template GetMask<T>() | BF34::template GetMask<T>() | BF35::template GetMask<T>() | BF36::template GetMask<T>() | BF37::template GetMask<T>() | BF38::template GetMask<T>() | BF39::template GetMask<T>()
	     | BF40::template GetMask<T>() | BF41::template GetMask<T>() | BF42::template GetMask<T>() | BF43::template GetMask<T>() | BF44::template GetMask<T>() | BF45::template GetMask<T>() | BF46::template GetMask<T>() | BF47::template GetMask<T>()
	     | BF48::template GetMask<T>() | BF49::template GetMask<T>() | BF50::template GetMask<T>() | BF51::template GetMask<T>() | BF52::template GetMask<T>() | BF53::template GetMask<T>() | BF54::template GetMask<T>() | BF55::template GetMask<T>()
	     | BF56::template GetMask<T>() | BF57::template GetMask<T>() | BF58::template GetMask<T>() | BF59::template GetMask<T>() | BF60::template GetMask<T>() | BF61::template GetMask<T>() | BF62::template GetMask<T>() | BF63::template GetMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetAssignMask()
{
	return  BF0::template GetAssignMask<T>() |  BF1::template GetAssignMask<T>() |  BF2::template GetAssignMask<T>() |  BF3::template GetAssignMask<T>() |  BF4::template GetAssignMask<T>() |  BF5::template GetAssignMask<T>() |  BF6::template GetAssignMask<T>() |  BF7::template GetAssignMask<T>()
	     |  BF8::template GetAssignMask<T>() |  BF9::template GetAssignMask<T>() | BF10::template GetAssignMask<T>() | BF11::template GetAssignMask<T>() | BF12::template GetAssignMask<T>() | BF13::template GetAssignMask<T>() | BF14::template GetAssignMask<T>() | BF15::template GetAssignMask<T>()
	     | BF16::template GetAssignMask<T>() | BF17::template GetAssignMask<T>() | BF18::template GetAssignMask<T>() | BF19::template GetAssignMask<T>() | BF20::template GetAssignMask<T>() | BF21::template GetAssignMask<T>() | BF22::template GetAssignMask<T>() | BF23::template GetAssignMask<T>()
	     | BF24::template GetAssignMask<T>() | BF25::template GetAssignMask<T>() | BF26::template GetAssignMask<T>() | BF27::template GetAssignMask<T>() | BF28::template GetAssignMask<T>() | BF29::template GetAssignMask<T>() | BF30::template GetAssignMask<T>() | BF31::template GetAssignMask<T>()
	     | BF32::template GetAssignMask<T>() | BF33::template GetAssignMask<T>() | BF34::template GetAssignMask<T>() | BF35::template GetAssignMask<T>() | BF36::template GetAssignMask<T>() | BF37::template GetAssignMask<T>() | BF38::template GetAssignMask<T>() | BF39::template GetAssignMask<T>()
	     | BF40::template GetAssignMask<T>() | BF41::template GetAssignMask<T>() | BF42::template GetAssignMask<T>() | BF43::template GetAssignMask<T>() | BF44::template GetAssignMask<T>() | BF45::template GetAssignMask<T>() | BF46::template GetAssignMask<T>() | BF47::template GetAssignMask<T>()
	     | BF48::template GetAssignMask<T>() | BF49::template GetAssignMask<T>() | BF50::template GetAssignMask<T>() | BF51::template GetAssignMask<T>() | BF52::template GetAssignMask<T>() | BF53::template GetAssignMask<T>() | BF54::template GetAssignMask<T>() | BF55::template GetAssignMask<T>()
	     | BF56::template GetAssignMask<T>() | BF57::template GetAssignMask<T>() | BF58::template GetAssignMask<T>() | BF59::template GetAssignMask<T>() | BF60::template GetAssignMask<T>() | BF61::template GetAssignMask<T>() | BF62::template GetAssignMask<T>() | BF63::template GetAssignMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetWriteMask()
{
	return  BF0::template GetWriteMask<T>() |  BF1::template GetWriteMask<T>() |  BF2::template GetWriteMask<T>() |  BF3::template GetWriteMask<T>() |  BF4::template GetWriteMask<T>() |  BF5::template GetWriteMask<T>() |  BF6::template GetWriteMask<T>() |  BF7::template GetWriteMask<T>()
	     |  BF8::template GetWriteMask<T>() |  BF9::template GetWriteMask<T>() | BF10::template GetWriteMask<T>() | BF11::template GetWriteMask<T>() | BF12::template GetWriteMask<T>() | BF13::template GetWriteMask<T>() | BF14::template GetWriteMask<T>() | BF15::template GetWriteMask<T>()
	     | BF16::template GetWriteMask<T>() | BF17::template GetWriteMask<T>() | BF18::template GetWriteMask<T>() | BF19::template GetWriteMask<T>() | BF20::template GetWriteMask<T>() | BF21::template GetWriteMask<T>() | BF22::template GetWriteMask<T>() | BF23::template GetWriteMask<T>()
	     | BF24::template GetWriteMask<T>() | BF25::template GetWriteMask<T>() | BF26::template GetWriteMask<T>() | BF27::template GetWriteMask<T>() | BF28::template GetWriteMask<T>() | BF29::template GetWriteMask<T>() | BF30::template GetWriteMask<T>() | BF31::template GetWriteMask<T>()
	     | BF32::template GetWriteMask<T>() | BF33::template GetWriteMask<T>() | BF34::template GetWriteMask<T>() | BF35::template GetWriteMask<T>() | BF36::template GetWriteMask<T>() | BF37::template GetWriteMask<T>() | BF38::template GetWriteMask<T>() | BF39::template GetWriteMask<T>()
	     | BF40::template GetWriteMask<T>() | BF41::template GetWriteMask<T>() | BF42::template GetWriteMask<T>() | BF43::template GetWriteMask<T>() | BF44::template GetWriteMask<T>() | BF45::template GetWriteMask<T>() | BF46::template GetWriteMask<T>() | BF47::template GetWriteMask<T>()
	     | BF48::template GetWriteMask<T>() | BF49::template GetWriteMask<T>() | BF50::template GetWriteMask<T>() | BF51::template GetWriteMask<T>() | BF52::template GetWriteMask<T>() | BF53::template GetWriteMask<T>() | BF54::template GetWriteMask<T>() | BF55::template GetWriteMask<T>()
	     | BF56::template GetWriteMask<T>() | BF57::template GetWriteMask<T>() | BF58::template GetWriteMask<T>() | BF59::template GetWriteMask<T>() | BF60::template GetWriteMask<T>() | BF61::template GetWriteMask<T>() | BF62::template GetWriteMask<T>() | BF63::template GetWriteMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetWriteOneClearMask()
{
	return  BF0::template GetWriteOneClearMask<T>() |  BF1::template GetWriteOneClearMask<T>() |  BF2::template GetWriteOneClearMask<T>() |  BF3::template GetWriteOneClearMask<T>() |  BF4::template GetWriteOneClearMask<T>() |  BF5::template GetWriteOneClearMask<T>() |  BF6::template GetWriteOneClearMask<T>() |  BF7::template GetWriteOneClearMask<T>()
	     |  BF8::template GetWriteOneClearMask<T>() |  BF9::template GetWriteOneClearMask<T>() | BF10::template GetWriteOneClearMask<T>() | BF11::template GetWriteOneClearMask<T>() | BF12::template GetWriteOneClearMask<T>() | BF13::template GetWriteOneClearMask<T>() | BF14::template GetWriteOneClearMask<T>() | BF15::template GetWriteOneClearMask<T>()
	     | BF16::template GetWriteOneClearMask<T>() | BF17::template GetWriteOneClearMask<T>() | BF18::template GetWriteOneClearMask<T>() | BF19::template GetWriteOneClearMask<T>() | BF20::template GetWriteOneClearMask<T>() | BF21::template GetWriteOneClearMask<T>() | BF22::template GetWriteOneClearMask<T>() | BF23::template GetWriteOneClearMask<T>()
	     | BF24::template GetWriteOneClearMask<T>() | BF25::template GetWriteOneClearMask<T>() | BF26::template GetWriteOneClearMask<T>() | BF27::template GetWriteOneClearMask<T>() | BF28::template GetWriteOneClearMask<T>() | BF29::template GetWriteOneClearMask<T>() | BF30::template GetWriteOneClearMask<T>() | BF31::template GetWriteOneClearMask<T>()
	     | BF32::template GetWriteOneClearMask<T>() | BF33::template GetWriteOneClearMask<T>() | BF34::template GetWriteOneClearMask<T>() | BF35::template GetWriteOneClearMask<T>() | BF36::template GetWriteOneClearMask<T>() | BF37::template GetWriteOneClearMask<T>() | BF38::template GetWriteOneClearMask<T>() | BF39::template GetWriteOneClearMask<T>()
	     | BF40::template GetWriteOneClearMask<T>() | BF41::template GetWriteOneClearMask<T>() | BF42::template GetWriteOneClearMask<T>() | BF43::template GetWriteOneClearMask<T>() | BF44::template GetWriteOneClearMask<T>() | BF45::template GetWriteOneClearMask<T>() | BF46::template GetWriteOneClearMask<T>() | BF47::template GetWriteOneClearMask<T>()
	     | BF48::template GetWriteOneClearMask<T>() | BF49::template GetWriteOneClearMask<T>() | BF50::template GetWriteOneClearMask<T>() | BF51::template GetWriteOneClearMask<T>() | BF52::template GetWriteOneClearMask<T>() | BF53::template GetWriteOneClearMask<T>() | BF54::template GetWriteOneClearMask<T>() | BF55::template GetWriteOneClearMask<T>()
	     | BF56::template GetWriteOneClearMask<T>() | BF57::template GetWriteOneClearMask<T>() | BF58::template GetWriteOneClearMask<T>() | BF59::template GetWriteOneClearMask<T>() | BF60::template GetWriteOneClearMask<T>() | BF61::template GetWriteOneClearMask<T>() | BF62::template GetWriteOneClearMask<T>() | BF63::template GetWriteOneClearMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetReadMask()
{
	return  BF0::template GetReadMask<T>() |  BF1::template GetReadMask<T>() |  BF2::template GetReadMask<T>() |  BF3::template GetReadMask<T>() |  BF4::template GetReadMask<T>() |  BF5::template GetReadMask<T>() |  BF6::template GetReadMask<T>() |  BF7::template GetReadMask<T>()
	     |  BF8::template GetReadMask<T>() |  BF9::template GetReadMask<T>() | BF10::template GetReadMask<T>() | BF11::template GetReadMask<T>() | BF12::template GetReadMask<T>() | BF13::template GetReadMask<T>() | BF14::template GetReadMask<T>() | BF15::template GetReadMask<T>()
	     | BF16::template GetReadMask<T>() | BF17::template GetReadMask<T>() | BF18::template GetReadMask<T>() | BF19::template GetReadMask<T>() | BF20::template GetReadMask<T>() | BF21::template GetReadMask<T>() | BF22::template GetReadMask<T>() | BF23::template GetReadMask<T>()
	     | BF24::template GetReadMask<T>() | BF25::template GetReadMask<T>() | BF26::template GetReadMask<T>() | BF27::template GetReadMask<T>() | BF28::template GetReadMask<T>() | BF29::template GetReadMask<T>() | BF30::template GetReadMask<T>() | BF31::template GetReadMask<T>()
	     | BF32::template GetReadMask<T>() | BF33::template GetReadMask<T>() | BF34::template GetReadMask<T>() | BF35::template GetReadMask<T>() | BF36::template GetReadMask<T>() | BF37::template GetReadMask<T>() | BF38::template GetReadMask<T>() | BF39::template GetReadMask<T>()
	     | BF40::template GetReadMask<T>() | BF41::template GetReadMask<T>() | BF42::template GetReadMask<T>() | BF43::template GetReadMask<T>() | BF44::template GetReadMask<T>() | BF45::template GetReadMask<T>() | BF46::template GetReadMask<T>() | BF47::template GetReadMask<T>()
	     | BF48::template GetReadMask<T>() | BF49::template GetReadMask<T>() | BF50::template GetReadMask<T>() | BF51::template GetReadMask<T>() | BF52::template GetReadMask<T>() | BF53::template GetReadMask<T>() | BF54::template GetReadMask<T>() | BF55::template GetReadMask<T>()
	     | BF56::template GetReadMask<T>() | BF57::template GetReadMask<T>() | BF58::template GetReadMask<T>() | BF59::template GetReadMask<T>() | BF60::template GetReadMask<T>() | BF61::template GetReadMask<T>() | BF62::template GetReadMask<T>() | BF63::template GetReadMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline bool FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                          ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                          , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                          , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                          , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                          , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                          , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                          , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::HasOverlappingBitFields()
{
	return (BF0::template GetMask<T>() & BF1::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF2::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF3::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF4::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF5::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF6::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF0::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF2::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF3::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF4::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF5::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF6::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF1::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF3::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF4::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF5::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF6::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF2::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF4::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF5::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF6::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF3::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF5::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF6::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF4::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF6::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF5::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF7::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF6::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF8::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF7::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF9::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF8::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF10::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF9::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF11::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF10::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF12::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF11::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF13::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF12::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF14::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF13::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF15::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF14::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF16::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF15::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF17::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF16::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF18::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF17::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF19::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF18::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF20::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF19::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF21::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF20::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF22::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF21::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF23::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF22::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF24::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF23::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF25::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF24::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF26::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF25::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF27::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF26::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF28::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF27::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF29::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF28::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF30::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF29::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF31::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF30::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF32::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF31::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF33::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF32::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF34::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF33::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF35::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF34::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF36::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF35::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF37::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF36::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF38::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF37::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF39::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF38::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF40::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF39::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF41::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF40::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF42::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF41::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF43::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF42::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF44::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF43::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF45::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF44::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF46::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF45::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF47::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF46::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF48::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF47::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF49::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF48::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF50::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF49::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF51::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF50::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF52::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF51::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF53::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF52::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF54::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF53::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF55::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF54::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF56::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF55::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF57::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF56::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF57::template GetMask<T>() & BF58::template GetMask<T>()) ||
	       (BF57::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF57::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF57::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF57::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF57::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF58::template GetMask<T>() & BF59::template GetMask<T>()) ||
	       (BF58::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF58::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF58::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF58::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF59::template GetMask<T>() & BF60::template GetMask<T>()) ||
	       (BF59::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF59::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF59::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF60::template GetMask<T>() & BF61::template GetMask<T>()) ||
	       (BF60::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF60::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF61::template GetMask<T>() & BF62::template GetMask<T>()) ||
	       (BF61::template GetMask<T>() & BF63::template GetMask<T>()) ||
	       (BF62::template GetMask<T>() & BF63::template GetMask<T>());
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline T FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                       ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                       , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                       , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                       , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                       , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                       , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                       , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::Get(const T& storage)
{
	return storage & GetMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> inline void FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                          ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                          , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                          , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                          , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                          , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                          , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                          , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::Set(T& storage, T value)
{
	storage = (storage & ~GetMask<T>()) | (value & GetMask<T>());
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename VISITOR>
inline void FieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                    ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                    , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                    , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                    , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                    , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                    , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                    , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::ForEach(VISITOR& visitor)
{
	BF0 ::ForEach(visitor);
	BF1 ::ForEach(visitor);
	BF2 ::ForEach(visitor);
	BF3 ::ForEach(visitor);
	BF4 ::ForEach(visitor);
	BF5 ::ForEach(visitor);
	BF6 ::ForEach(visitor);
	BF7 ::ForEach(visitor);
	BF8 ::ForEach(visitor);
	BF9 ::ForEach(visitor);
	BF10::ForEach(visitor);
	BF11::ForEach(visitor);
	BF12::ForEach(visitor);
	BF13::ForEach(visitor);
	BF14::ForEach(visitor);
	BF15::ForEach(visitor);
	BF16::ForEach(visitor);
	BF17::ForEach(visitor);
	BF18::ForEach(visitor);
	BF19::ForEach(visitor);
	BF20::ForEach(visitor);
	BF21::ForEach(visitor);
	BF22::ForEach(visitor);
	BF23::ForEach(visitor);
	BF24::ForEach(visitor);
	BF25::ForEach(visitor);
	BF26::ForEach(visitor);
	BF27::ForEach(visitor);
	BF28::ForEach(visitor);
	BF29::ForEach(visitor);
	BF30::ForEach(visitor);
	BF31::ForEach(visitor);
	BF32::ForEach(visitor);
	BF33::ForEach(visitor);
	BF34::ForEach(visitor);
	BF35::ForEach(visitor);
	BF36::ForEach(visitor);
	BF37::ForEach(visitor);
	BF38::ForEach(visitor);
	BF39::ForEach(visitor);
	BF40::ForEach(visitor);
	BF41::ForEach(visitor);
	BF42::ForEach(visitor);
	BF43::ForEach(visitor);
	BF44::ForEach(visitor);
	BF45::ForEach(visitor);
	BF46::ForEach(visitor);
	BF47::ForEach(visitor);
	BF48::ForEach(visitor);
	BF49::ForEach(visitor);
	BF50::ForEach(visitor);
	BF51::ForEach(visitor);
	BF52::ForEach(visitor);
	BF53::ForEach(visitor);
	BF54::ForEach(visitor);
	BF55::ForEach(visitor);
	BF56::ForEach(visitor);
	BF57::ForEach(visitor);
	BF58::ForEach(visitor);
	BF59::ForEach(visitor);
	BF60::ForEach(visitor);
	BF61::ForEach(visitor);
	BF62::ForEach(visitor);
	BF63::ForEach(visitor);
}

////////////////////////////////// Register<> /////////////////////////////////

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
const typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE_MASK;

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
const unsigned int Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::SIZE;

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
const Access Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::ACCESS;

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetMask() const
{
	return (_REGISTER::ALL::template GetMask<TYPE>() & TYPE_MASK);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetAssignMask() const
{
	return (_ACCESS & AF_HW_W) ? (_REGISTER::ALL::template GetAssignMask<TYPE>() & TYPE_MASK) : 0;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetWriteMask() const
{
	return (_ACCESS & AF_SW_W) ? (_REGISTER::ALL::template GetWriteMask<TYPE>() & TYPE_MASK): 0;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetWriteOneClearMask() const
{
	return (_ACCESS & AF_SW_W) ? ((_ACCESS & AF_SW_W1C) ? _REGISTER::ALL::template GetWriteMask<TYPE>() : (_REGISTER::ALL::template GetWriteOneClearMask<TYPE>() & TYPE_MASK)) : 0;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetReadMask() const
{
	return (_ACCESS & AF_SW_R) ? (_REGISTER::ALL::template GetReadMask<TYPE>() & TYPE_MASK) : 0;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Get() const
{
	return value;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Set(TYPE _value)
{
	value = (value & ~GetAssignMask()) | (_value & GetAssignMask());
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::HasBit(unsigned int bit_offset) const
{
	return _REGISTER::ALL::template GetMask<TYPE>() & (TYPE(1) << bit_offset);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Get(unsigned int bit_offset) const
{
	return ((value & TYPE_MASK) >> bit_offset) & 1;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Set(unsigned int bit_offset, TYPE bit_value)
{
	TYPE mask = (TYPE(1) << bit_offset) & GetMask();
	value = (value & ~mask) | ((TYPE(bit_value & 1) << bit_offset) & mask);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
template <typename FIELD> inline bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::HasField() const
{
	return _REGISTER::ALL::template GetMask<TYPE>() & FIELD::template GetMask<TYPE>();
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
template <typename FIELD> inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Get() const
{
	return FIELD::Get(value);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
template <typename FIELD> inline void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Set(TYPE field_value)
{
	FIELD::Set(value, field_value);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Register()
	: value(0)
{
	if(static_cast<_REGISTER *>(this)->NeedCheckForOverlappingBitFields())
	{
		assert(_REGISTER::ALL::template HasOverlappingBitFields<TYPE>() == false);
	}
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Register(TYPE _value)
	: value(_value & TYPE_MASK)
{
	if(static_cast<_REGISTER *>(this)->NeedCheckForOverlappingBitFields())
	{
		assert(_REGISTER::ALL::template HasOverlappingBitFields<TYPE>() == false);
	}
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Register(const Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& r)
	: value(r.value)
{
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::WithinRegisterFileCtor(unsigned int index, void *custom_ctor_arg)
{
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Initialize(TYPE _value)
{
	value = _value & TYPE_MASK;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::operator = (const TYPE& _value)
{
	Set(_value);
	return *this;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::operator const TYPE () const
{
	return value & TYPE_MASK;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline typename Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::operator [] (unsigned int bit_offset) const
{
	return Get(bit_offset);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
ReadWriteStatus Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Write(const TYPE& _value, const TYPE& bit_enable)
{
	if(unlikely(__IsVerboseWrite__()))
	{
		std::ostream& os = __GetInfoStream__();
		std::ios_base::fmtflags ff = os.flags();
		os << "writing " << GetName() << " <- 0x" << std::hex << (uint64_t)(_value & bit_enable);
		if(bit_enable != (~TYPE(0) & TYPE_MASK))
		{
			os << " (bit_enable=0x" << bit_enable << ")";
		}
		os.flags(ff);
	}

	ReadWriteStatus rws(RWS_OK);
	const TYPE write_mask = bit_enable & GetWriteMask();
	const TYPE write_one_clear_mask = bit_enable & GetWriteOneClearMask();
	const TYPE copy_mask = bit_enable & write_mask & ~write_one_clear_mask;
	const TYPE unwritable_mask = bit_enable & ~write_mask & ~write_one_clear_mask;
	
	if((_value & bit_enable & TYPE_MASK) != (_value & bit_enable))
	{
		// writing out-of-range register value
		rws = ReadWriteStatus(rws | RWSF_WOORV);
	}
	
	if(!(_ACCESS & AF_SW_W))
	{
		// writing read-only register
		rws = ReadWriteStatus(rws | RWSF_WROR);
	}
	else if((_value & unwritable_mask) != (value & unwritable_mask))
	{
		// writing read-only bits
		rws = ReadWriteStatus(rws | RWSF_WROB);
	}
	
	value = (value & ~copy_mask) | (_value & copy_mask);
	value = value & ~(_value & write_one_clear_mask);
	
	if(unlikely(__IsVerboseWrite__()))
	{
		std::ostream& os = __GetInfoStream__();
		std::ios_base::fmtflags ff = os.flags();
		os << " => " << std::hex;
		ShortPrettyPrint(os);
		os << ", " << rws << std::endl;
		os.flags(ff);
	}

	return rws;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
template <typename PRESERVED_FIELD>
ReadWriteStatus Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::WritePreserve(const TYPE& _value, const TYPE& bit_enable)
{
	if(unlikely(__IsVerboseWrite__()))
	{
		std::ostream& os = __GetInfoStream__();
		std::ios_base::fmtflags ff = os.flags();
		os << "writing " << GetName() << " <- 0x" << std::hex << (uint64_t)(_value & bit_enable);
		if(bit_enable != (~TYPE(0) & TYPE_MASK))
		{
			os << " (bit_enable=0x" << bit_enable << ")";
		}
		os.flags(ff);
	}

	ReadWriteStatus rws(RWS_OK);
	const TYPE write_mask = bit_enable & GetWriteMask() & ~PRESERVED_FIELD::template GetMask<TYPE>();
	const TYPE write_one_clear_mask = bit_enable & GetWriteOneClearMask() & ~PRESERVED_FIELD::template GetMask<TYPE>();
	const TYPE copy_mask = bit_enable & write_mask & ~write_one_clear_mask;
	const TYPE unwritable_mask = bit_enable & ~write_mask & ~write_one_clear_mask;
	
	if((_value & bit_enable & TYPE_MASK) != (_value & bit_enable))
	{
		// writing out-of-range register value
		rws = ReadWriteStatus(rws | RWSF_WOORV);
	}
	
	if(!(_ACCESS & AF_SW_W))
	{
		// writing read-only register
		rws = ReadWriteStatus(rws | RWSF_WROR);
	}
	else if((_value & unwritable_mask) != (value & unwritable_mask))
	{
		// writing read-only bits
		rws = ReadWriteStatus(rws | RWSF_WROB);
	}
	
	value = (value & ~copy_mask) | (_value & copy_mask);
	value = value & ~(_value & write_one_clear_mask);
	
	if(unlikely(__IsVerboseWrite__()))
	{
		std::ostream& os = __GetInfoStream__();
		std::ios_base::fmtflags ff = os.flags();
		os << " => " << std::hex;
		ShortPrettyPrint(os);
		os << ", " << rws << std::endl;
		os.flags(ff);
	}

	return rws;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
ReadWriteStatus Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Read(TYPE& _value, const TYPE& bit_enable)
{
	ReadWriteStatus rws(RWS_OK);
	const TYPE read_mask = bit_enable & GetReadMask();
	
	if(!(_ACCESS & AF_SW_R))
	{
		// reading write-only register
		rws = ReadWriteStatus(rws | RWSF_RWOR);
	}
	
	_value = (_value & ~bit_enable) | (value & read_mask);
	
	if(unlikely(__IsVerboseRead__()))
	{
		std::ostream& os = __GetInfoStream__();
		std::ios_base::fmtflags ff = os.flags();
		os << "reading " << std::hex;
		ShortPrettyPrint(os);
		os << ", " << rws << std::endl;
		os.flags(ff);
	}

	return rws;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::DebugWrite(const TYPE& _value, const TYPE& bit_enable)
{
	TYPE old_value = value & TYPE_MASK;
	TYPE debug_write_mask = bit_enable & GetAssignMask();
	value = (old_value & ~debug_write_mask) | (_value & debug_write_mask);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::DebugRead(TYPE& _value, const TYPE& bit_enable)
{
	const TYPE mask = bit_enable & GetMask();
	_value = (_value & ~bit_enable) | (value & mask);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
ReadWriteStatus Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Write(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	const TYPE *data_to_write = (const TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(const TYPE *) bit_enable_ptr : ~TYPE(0);
	return Write(*data_to_write, bit_enable);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
ReadWriteStatus Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::Read(unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	TYPE *read_data = (TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(const TYPE *) bit_enable_ptr : ~TYPE(0);
	return Read(*read_data, bit_enable);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::DebugWrite(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	const TYPE *data_to_write = (const TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(const TYPE *) bit_enable_ptr : ~TYPE(0);
	DebugWrite(*data_to_write, bit_enable);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::DebugRead(unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	TYPE *read_data = (TYPE *) data_ptr;
	const TYPE bit_enable = (bit_enable_ptr) ? *(const TYPE *) bit_enable_ptr : ~TYPE(0);
	DebugRead(*read_data, bit_enable);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
std::ostream& operator << (std::ostream& os, const Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& reg)
{
	return os << (reg.value & Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::TYPE_MASK);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::ShortPrettyPrint(std::ostream& os) const
{
	std::ios_base::fmtflags ff = os.flags();
	os << GetName() << "=";
	if(ff & os.hex) os << "0x";
	else if(ff & os.oct) os << "0";
	os << (value & TYPE_MASK);
	os << "<";
	FieldsShortPrettyPrinter fields_short_pretty_printer(os, value);
	_REGISTER::ALL::ForEach(fields_short_pretty_printer);
	os << ">";
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::LongPrettyPrint(std::ostream& os) const
{
	LongPrettyPrinter::PrintHeader(os);
	LongPrettyPrinter::PrintKind(os, "register");
	LongPrettyPrinter::PrintRegisterSize(os, _SIZE);
	LongPrettyPrinter::PrintDisplayName(os, GetDisplayName());
	LongPrettyPrinter::PrintValue(os, value & TYPE_MASK);
	LongPrettyPrinter::PrintAccess(os, _ACCESS);
	LongPrettyPrinter::PrintDescription(os, GetDescription());
	os << std::endl;
	
	FieldsLongPrettyPrinter fields_long_pretty_printer(os, value);
	_REGISTER::ALL::ForEach(fields_long_pretty_printer);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::SetName(const std::string& name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_NAME, (intptr_t) this, name);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::SetDisplayName(const std::string& disp_name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DISP_NAME, (intptr_t) this, disp_name);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
void Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::SetDescription(const std::string& desc)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DESC, (intptr_t) this, desc);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
unsigned int Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetSize() const
{
	return _SIZE;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
Access Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetAccess() const
{
	return _ACCESS;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsReadable() const
{
	return _ACCESS & AF_SW_R;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsWritable() const
{
	return _ACCESS & AF_SW_W;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsWriteOnce() const
{
	return _ACCESS & AF_SW_W1;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsReadOnly() const
{
	return (_ACCESS & (AF_SW_R | AF_SW_W)) == AF_SW_R;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsWriteOnly() const
{
	return (_ACCESS & (AF_SW_R | AF_SW_W)) == AF_SW_W;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsWriteOnlyAndOnce() const
{
	return (_ACCESS & (AF_SW_R | AF_SW_W | AF_SW_W1)) == (AF_SW_W | AF_SW_W1);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
const std::string& Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetName() const
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_NAME, (intptr_t) this);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
const std::string& Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetDisplayName() const
{
	const std::string& display_name = PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DISP_NAME, (intptr_t) this);
	if(!display_name.empty()) return display_name;
	return GetName();
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
const std::string& Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetDescription() const
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER, PropertyRegistry::STR_PROP_DESC, (intptr_t) this);
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
unisim::service::interfaces::Register *Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::CreateRegisterInterface()
{
	return new RegisterInterface<_REGISTER>(*static_cast<_REGISTER *>(this));
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsVerboseRead() const
{
	return false;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::IsVerboseWrite() const
{
	return false;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline std::ostream& Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::GetInfoStream()
{
	return std::cout;
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::__IsVerboseRead__() const
{
	return static_cast<const _REGISTER *>(this)->IsVerboseRead();
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline bool Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::__IsVerboseWrite__() const
{
	return static_cast<const _REGISTER *>(this)->IsVerboseWrite();
}

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
inline std::ostream&  Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>::__GetInfoStream__()
{
	return static_cast<_REGISTER *>(this)->GetInfoStream();
}

/////////////////////////////// RegisterFile<> ////////////////////////////////

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::RegisterFile(CUSTOM_CTOR_ARG *custom_ctor_arg)
	: regs()
{
	unsigned int index;
	
	for(index = 0; index < _DIM; index++)
	{
		regs[index].WithinRegisterFileCtor(index, custom_ctor_arg);
	}
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::RegisterFile(typename REGISTER::TYPE value, CUSTOM_CTOR_ARG *custom_ctor_arg)
	: regs()
{
	unsigned int index;
	
	for(index = 0; index < _DIM; index++)
	{
		regs[index].WithinRegisterFileCtor(index, custom_ctor_arg);
		regs[index].Initialize(value);
	}
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
inline REGISTER& RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::operator [] (int index)
{
	return regs[index];
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
inline const REGISTER& RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::operator [] (int index) const
{
	return regs[index];
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
inline unsigned int RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::GetDim() const
{
	return _DIM;
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
inline unsigned int RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::GetSize() const
{
	return _DIM * REGISTER::SIZE;
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
void RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::SetName(const std::string& name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER_FILE, PropertyRegistry::STR_PROP_NAME, (intptr_t) this, name);
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
void RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::SetDisplayName(const std::string& disp_name)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER_FILE, PropertyRegistry::STR_PROP_DISP_NAME, (intptr_t) this, disp_name);
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
void RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::SetDescription(const std::string& desc)
{
	PropertyRegistry::SetStringProperty(PropertyRegistry::EL_REGISTER_FILE, PropertyRegistry::STR_PROP_DESC, (intptr_t) this, desc);
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
const std::string& RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::GetName() const
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER_FILE, PropertyRegistry::STR_PROP_NAME, (intptr_t) this);
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
const std::string& RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::GetDisplayName() const
{
	const std::string& display_name = PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER_FILE, PropertyRegistry::STR_PROP_DISP_NAME, (intptr_t) this);
	if(!display_name.empty()) return display_name;
	return GetName();
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename REGISTER_FILE_BASE>
const std::string& RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, REGISTER_FILE_BASE>::GetDescription() const
{
	return PropertyRegistry::GetStringProperty(PropertyRegistry::EL_REGISTER_FILE, PropertyRegistry::STR_PROP_DESC, (intptr_t) this);
}

/////////////////////////// AddressableRegister<> /////////////////////////////

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::AddressableRegister()
	: Super()
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::AddressableRegister(typename Super::TYPE value)
	: Super(value)
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::AddressableRegister(const AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>& ar)
	: Super(ar)
{
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
ReadWriteStatus AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::Write(const typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	return Super::Write(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
ReadWriteStatus AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::Read(typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	return Super::Read(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::DebugWrite(const typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	Super::DebugWrite(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::DebugRead(typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	Super::DebugRead(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
ReadWriteStatus AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::Write(CUSTOM_RW_ARG& custom_rw_arg, const typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	return Super::Write(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
ReadWriteStatus AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::Read(CUSTOM_RW_ARG& custom_rw_arg, typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	return Super::Read(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::DebugWrite(CUSTOM_RW_ARG& custom_rw_arg, const typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	Super::DebugWrite(value, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::DebugRead(CUSTOM_RW_ARG& custom_rw_arg, typename Super::TYPE& value, const typename Super::TYPE& bit_enable)
{
	Super::DebugRead(value, bit_enable);
}

// template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
// void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::DebugWrite(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
// {
// 	Super::DebugWrite(data_ptr, bit_enable_ptr);
// }
// 
// template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
// void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::DebugRead(unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
// {
// 	Super::DebugRead(data_ptr, bit_enable_ptr);
// }

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::ShortPrettyPrint(std::ostream& os) const
{
	Super::ShortPrettyPrint(os);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::LongPrettyPrint(std::ostream& os) const
{
	Super::LongPrettyPrint(os);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
unsigned int AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::__ARB_GetSize__() const
{
	return _SIZE;
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
const std::string& AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::__ARB_GetName__() const
{
	return Super::GetName();
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
ReadWriteStatus AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::__ARB_Write__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	const typename Super::TYPE *data_to_write = (const typename Super::TYPE *) data_ptr;
	const typename Super::TYPE bit_enable = (bit_enable_ptr) ? *(const typename Super::TYPE *) bit_enable_ptr : ~typename Super::TYPE(0);
	return custom_rw_arg ? Write(*custom_rw_arg, *data_to_write, bit_enable) : Write(*data_to_write, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
ReadWriteStatus AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::__ARB_Read__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	typename Super::TYPE *read_data = (typename Super::TYPE *) data_ptr;
	const typename Super::TYPE bit_enable = (bit_enable_ptr) ? *(const typename Super::TYPE *) bit_enable_ptr : ~typename Super::TYPE(0);
	return custom_rw_arg ? Read(*custom_rw_arg, *read_data, bit_enable) : Read(*read_data, bit_enable);
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::__ARB_DebugWrite__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	const typename Super::TYPE *data_to_write = (const typename Super::TYPE *) data_ptr;
	const typename Super::TYPE bit_enable = (bit_enable_ptr) ? *(const typename Super::TYPE *) bit_enable_ptr : ~typename Super::TYPE(0);
	if(custom_rw_arg)
	{
		DebugWrite(*custom_rw_arg, *data_to_write, bit_enable);
	}
	else
	{
		DebugWrite(*data_to_write, bit_enable);
	}
}

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG>
void AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>::__ARB_DebugRead__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr)
{
	typename Super::TYPE *read_data = (typename Super::TYPE *) data_ptr;
	const typename Super::TYPE bit_enable = (bit_enable_ptr) ? *(const typename Super::TYPE *) bit_enable_ptr : ~typename Super::TYPE(0);
	if(custom_rw_arg)
	{
		DebugRead(*custom_rw_arg, *read_data, bit_enable);
	}
	else
	{
		DebugRead(*read_data, bit_enable);
	}
}

/////////////////////////// AddressableRegisterFile<> /////////////////////////

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename CUSTOM_RW_ARG>
AddressableRegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, CUSTOM_RW_ARG>::AddressableRegisterFile(CUSTOM_CTOR_ARG *custom_ctor_arg)
	: Super(custom_ctor_arg)
{
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename CUSTOM_RW_ARG>
AddressableRegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, CUSTOM_RW_ARG>::AddressableRegisterFile(typename REGISTER::TYPE value, CUSTOM_CTOR_ARG *custom_ctor_arg)
	: Super(value, custom_ctor_arg)
{
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename CUSTOM_RW_ARG>
unsigned int AddressableRegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, CUSTOM_RW_ARG>::__ARFB_GetDim__() const
{
	return Super::GetDim();
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename CUSTOM_RW_ARG>
const std::string& AddressableRegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, CUSTOM_RW_ARG>::__ARFB_GetName__() const
{
	return Super::GetName();
}

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG, typename CUSTOM_RW_ARG>
AddressableRegisterBase<CUSTOM_RW_ARG> *AddressableRegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, CUSTOM_RW_ARG>::__ARFB_GetRegister__(unsigned int index)
{
	return &Super::operator [] (index);
}

////////////////////////// AddressableRegisterHandle<> ////////////////////////

template <typename ADDRESS, typename CUSTOM_RW_ARG>
AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG>::AddressableRegisterHandle(ADDRESS _addr, unsigned int _size, AddressableRegisterBase<CUSTOM_RW_ARG> *_arb)
	: addr(_addr)
	, size(_size)
	, arb(_arb)
	, ref_count(0)
{
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG>::Acquire()
{
	ref_count++;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG>::Release()
{
	if(ref_count && (--ref_count == 0))
	{
		delete this;
	}
}

///////////////////////////// RegisterAddressMap<> ////////////////////////////

template <typename ADDRESS, typename CUSTOM_RW_ARG>
RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::RegisterAddressMap()
	: optimized(true)
	, optimizable(true)
	, addr_range(ADDRESS(-1), ADDRESS(0))
	, opt_reg_addr_map()
	, reg_addr_map()
	, warning_stream(0)
	, data_endian(unisim::util::endian::GetHostEndian())
{
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::~RegisterAddressMap()
{
	ClearRegisterMap();
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::SetWarningStream(std::ostream& _warning_stream)
{
	warning_stream = &_warning_stream;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::SetEndian(unisim::util::endian::endian_type _endian)
{
	data_endian = _endian;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::MapRegister(ADDRESS addr, AddressableRegisterBase<CUSTOM_RW_ARG> *arb, unsigned int reg_byte_size)
{
	unsigned int _reg_byte_size = reg_byte_size ? reg_byte_size : (arb->__ARB_GetSize__() + 7) / 8;
	//std::cerr << "Mapping " << arb->__ARB_GetName__() << " to 0x" << std::hex << addr << std::dec << " (" << reg_byte_size << " bytes)" << std::endl;

	unsigned int byte_idx;

	if(_reg_byte_size)
	{
		optimized = false;
		optimizable = true;
		
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = new AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG>(addr, _reg_byte_size, arb);
		
		for(byte_idx = 0; byte_idx < _reg_byte_size; byte_idx++)
		{
			ADDRESS byte_addr = addr + byte_idx;
			
			// check that byte is not already mapped
			typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *>::const_iterator it = reg_addr_map.find(byte_addr);
			
			if(it != reg_addr_map.end())
			{
				std::stringstream error_sstr;
				AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = (*it).second;
				error_sstr << "Internal Error! while mapping \"" << arb->__ARB_GetName__() << "\",  register \"" << arh->GetName() << "\" is already mapped at the same address";
				throw std::runtime_error(error_sstr.str());
			}

			arh->Acquire();
			reg_addr_map.insert(std::pair<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *>(byte_addr, arh));
		}
	}
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::MapRegisterFile(ADDRESS addr, AddressableRegisterFileBase<CUSTOM_RW_ARG> *arfb, unsigned int reg_byte_size, unsigned int stride)
{
	//std::cerr << "Mapping " << arfb->__ARFB_GetName__() << " to 0x" << std::hex << addr << std::dec << std::endl;
	ADDRESS reg_addr;
	unsigned int idx;
	for(idx = 0, reg_addr = addr; idx < arfb->__ARFB_GetDim__(); idx++)
	{
		AddressableRegisterBase<CUSTOM_RW_ARG> *arb = arfb->__ARFB_GetRegister__(idx);
		unsigned int _reg_byte_size = reg_byte_size ? reg_byte_size : (arb->__ARB_GetSize__() + 7) / 8;
		MapRegister(reg_addr, arb, _reg_byte_size);
		reg_addr += stride ? stride : _reg_byte_size;
	}
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::UnmapRegistersAt(ADDRESS addr, unsigned int byte_size)
{
	if(byte_size)
	{
		unsigned int byte_idx;

		optimized = false;
		optimizable = true;
		
		for(byte_idx = 0; byte_idx < byte_size; byte_idx++)
		{
			ADDRESS byte_addr = addr + byte_idx;
			
			typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *>::const_iterator it = reg_addr_map.find(byte_addr);
			
			if(it != reg_addr_map.end())
			{
				AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = (*it).second;
				arh->Release();
				reg_addr_map.erase(it);
			}
		}
	}
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::ClearRegisterMap()
{
	typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *>::const_iterator it;

	for(it = reg_addr_map.begin(); it != reg_addr_map.end(); it++)
	{
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = (*it).second;
		
		if(arh)
		{
			arh->Release();
		}
	}
	reg_addr_map.clear();
	optimized = false;
	optimizable = true;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
void RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Optimize() const
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

	typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *>::const_iterator it;
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

	// "reasonably optimizable" means that at least half of the register map address range shall be filled
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
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = (*it).second;
		opt_reg_addr_map[addr - addr_range.first] = arh;
	}
	
	optimizable = true;
	optimized = true;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::FindAddressableRegister(ADDRESS addr) const
{
	if(optimized)
	{
		return (opt_reg_addr_map.size() && ((addr >= addr_range.first) && (addr <= addr_range.second))) ? opt_reg_addr_map[addr - addr_range.first] : 0;
	}
	
	if(optimizable)
	{
		Optimize();
	}
	
	typename std::map<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *>::const_iterator it = reg_addr_map.find(addr);

	return it != reg_addr_map.end() ? (*it).second : 0;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Write(ADDRESS addr, const unsigned char *data_ptr, unsigned int data_length)
{
	return Write((CUSTOM_RW_ARG *) 0, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Read(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	return Read((CUSTOM_RW_ARG *) 0, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
unsigned int RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::DebugWrite(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	return DebugWrite((CUSTOM_RW_ARG *) 0, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
unsigned int RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::DebugRead(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	return DebugRead((CUSTOM_RW_ARG *) 0, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Write(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, const unsigned char *data_ptr, unsigned int data_length)
{
	return Write(&custom_rw_arg, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Read(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	return Read(&custom_rw_arg, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
unsigned int RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::DebugWrite(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	return DebugWrite(&custom_rw_arg, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
unsigned int RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::DebugRead(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	return DebugRead(&custom_rw_arg, addr, data_ptr, data_length); 
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Write(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, const unsigned char *data_ptr, unsigned int data_length)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return RWS_UA;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		unsigned char reg_be[reg_size];
		
		memset(reg_value, 0, sizeof(reg_value));
		memset(reg_be, 0, sizeof(reg_be));
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = data_ptr[byte_offset];
			if(host_endian != data_endian)
			{
				reg_value[reg_size_minus_1 - byte_pos] = byte_value;
				reg_be[reg_size_minus_1 - byte_pos] = 0xff;
			}
			else
			{
				reg_value[byte_pos] = byte_value;
				reg_be[byte_pos] = 0xff;
			}
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
		
		ReadWriteStatus rws = arh->__ARH_Write__(custom_rw_arg, reg_value, reg_be);
		
		if(rws)
		{
			if(warning_stream)
			{
				(*warning_stream) << "while writing to " << arh->GetName() << ", " << rws << "; See below " << arh->GetName() << " content after mapped write access: " << std::endl;
				arh->LongPrettyPrint(*warning_stream);
				(*warning_stream) << std::endl;
			}
			
			if(IsReadWriteError(rws)) return rws;
		}
	}
	
	return RWS_OK;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Read(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return RWS_UA;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		unsigned char reg_be[reg_size];
		
		memset(reg_value, 0, sizeof(reg_value));
		memset(reg_be, 0, sizeof(reg_be));

		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			if(host_endian != data_endian)
			{
				reg_be[reg_size_minus_1 - byte_pos] = 0xff;
			}
			else
			{
				reg_be[byte_pos] = 0xff;
			}
			
			byte_pos++;
		}
		while(--sz);

		//std::cerr << "Reading \"" << arh->GetName() << "\"" << std::endl;
		ReadWriteStatus rws = arh->__ARH_Read__(custom_rw_arg, reg_value, reg_be);
		
		if(rws)
		{
			if(warning_stream)
			{
				(*warning_stream) << "while reading to " << arh->GetName() << ", " << rws << "; See below " << arh->GetName() << " content after mapped read access: " << std::endl;
				arh->LongPrettyPrint(*warning_stream);
				(*warning_stream) << std::endl;
			}
			
			if(IsReadWriteError(rws)) return rws;
		}

		byte_pos = byte_addr - reg_addr;
		
		sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = (host_endian != data_endian) ? reg_value[reg_size_minus_1 - byte_pos] : reg_value[byte_pos];
			data_ptr[byte_offset] = byte_value;
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
	}
	
	return RWS_OK;
}


#if 0
template <typename ADDRESS, typename CUSTOM_RW_ARG>
ReadWriteStatus RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::Read(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return RWS_UA;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		
		//std::cerr << "Reading \"" << arh->GetName() << "\"" << std::endl;
		ReadWriteStatus rws = arh->__ARH_Read__(custom_rw_arg, reg_value, /* bit enable */ 0);
		
		if(rws)
		{
			if(warning_stream)
			{
				(*warning_stream) << "while reading to " << arh->GetName() << ", " << rws << "; See below " << arh->GetName() << " content after mapped read access: " << std::endl;
				arh->LongPrettyPrint(*warning_stream);
				(*warning_stream) << std::endl;
			}
			
			if(IsReadWriteError(rws)) return rws;
		}

		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = (host_endian != data_endian) ? reg_value[reg_size_minus_1 - byte_pos] : reg_value[byte_pos];
			data_ptr[byte_offset] = byte_value;
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
	}
	
	return RWS_OK;
}
#endif

template <typename ADDRESS, typename CUSTOM_RW_ARG>
unsigned int RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::DebugWrite(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return byte_offset;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		unsigned char reg_be[reg_size];
		
		memset(reg_value, 0, sizeof(reg_value));
		memset(reg_be, 0, sizeof(reg_be));
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = data_ptr[byte_offset];
			if(host_endian != data_endian)
			{
				reg_value[reg_size_minus_1 - byte_pos] = byte_value;
				reg_be[reg_size_minus_1 - byte_pos] = 0xff;
			}
			else
			{
				reg_value[byte_pos] = byte_value;
				reg_be[byte_pos] = 0xff;
			}
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
		
		arh->__ARH_DebugWrite__(custom_rw_arg, reg_value, reg_be);
	}
	
	return true;
}

template <typename ADDRESS, typename CUSTOM_RW_ARG>
unsigned int RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>::DebugRead(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length)
{
	unsigned int byte_offset;
	
	unisim::util::endian::endian_type host_endian = unisim::util::endian::GetHostEndian();
	
	for(byte_offset = 0; byte_offset < data_length;)
	{
		ADDRESS byte_addr = addr + byte_offset;
		
		AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *arh = FindAddressableRegister(byte_addr);
		
		if(!arh) return byte_offset;
		
		ADDRESS reg_addr = arh->GetAddress();
		
		unsigned int reg_size = arh->GetSize();
		unsigned int reg_size_minus_1 = reg_size - 1;
		unsigned char reg_value[reg_size];
		
		arh->__ARH_DebugRead__(custom_rw_arg, reg_value, 0 /* bit_enable */);
		
		unsigned int byte_pos = byte_addr - reg_addr;
		
		unsigned int sz = std::min(data_length - byte_offset, reg_size - byte_pos);
		
		do
		{
			uint8_t byte_value = (host_endian != data_endian) ? reg_value[reg_size_minus_1 - byte_pos] : reg_value[byte_pos];
			data_ptr[byte_offset] = byte_value;
			
			byte_offset++;
			byte_pos++;
		}
		while(--sz);
	}
	
	return true;
}

} // end of namespace core
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_REG_CORE_REGISTER_TCC__
