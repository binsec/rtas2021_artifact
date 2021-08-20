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

/* Example of use:
 * 
 * struct MR : Register<MR, 32, SW_RW>   // 32-bit register, hardware read-write/software read-write access
 * {
 *     typedef Register<MR, 32> Super;
 * 
 *     struct A : Field<31, 2, SW_W> {}; // field [31..2], hardware write-only/software write-only access
 *     struct B : Field<26> {};          // field [26], implicitely one bit length field, implicitely hardware read-write/software read-write access
 *     struct C : Field<11, 3> {};       // field [11..3], implicitely hardware read-write/software read-write
 *     struct E : Field<7, 2, HW_R> {};  // field [7..2], hardware read-only/no software access 
 *     struct D : Field<0, 5, SW_R> {};  // field [5..0], hardware read-only/software read-only access
 * 
 *     typedef FieldSet<A, B, C, D> ALL; // field order is not important but determine pretty print order
 * 
 *     MR(uint32_t _value) : Super(_value) {}
 *     using Super::operator =;
 * };
 * 
 * // construction
 * MR mr(0x12345678);
 * 
 * // initialization
 * mr.Initialize(0x12345678);
 * 
 * // hardware write of a register field
 * mr.Set<MR::B>(1);
 * 
 * // hardware read of a register field
 * unsigned int a_value = mr.Get<MR::A>();
 * 
 * // hardware read of register value
 * MR::TYPE mr_val = mr;
 * 
 * // hardware write register value
 * mr = 0x0;
 * 
 * // software write of register value
 * ReadWriteStatus rws_write = mr.Write(0xffffffff);
 * if(rws_write)
 * {
 *     std::cerr << rws_write << std::endl;
 * }
 * 
 * // software read of register value
 * uint32_t v;
 * ReadWriteStatus rws_read = mr.Read(v);
 * if(rws_read)
 * {
 *     std::cerr << rws_read << std::endl;
 * }
 * 
 * // print register value
 * std::cout << mr << std::endl;
 * 
 * // short pretty print of register content
 * mr.ShortPrettyPrint(std::cout); std::cout << std::endl;
 * 
 * // long pretty print of register content
 * mr.LongPrettyPrint(std::cout); std::cout << std::endl;
 */

#ifndef __UNISIM_UTIL_REG_CORE_REGISTER_HH__
#define __UNISIM_UTIL_REG_CORE_REGISTER_HH__

#include <unisim/util/inlining/inlining.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/register.hh>
#include <unisim/util/debug/simple_field_registry.hh>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <typeinfo>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace reg {
namespace core {

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// DECLARATIONS //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////// AccessFlag //////////////////////////////////

enum AccessFlags
{
	AF_HW_W   = 1,
	AF_SW_R   = 2,
	AF_SW_W   = 4,
	AF_SW_W1  = 8,
	AF_SW_W1C = 16
};

/////////////////////////////////// Access ////////////////////////////////////

enum Access
{
	HW_RO = 0,                                                       // hardware read-only/no software access
	HW_RW = AF_HW_W,                                                 // hardware read-write/no software access
	SW_R_HW_RO = AF_SW_R,                                            // hardware read-only/software read access
	SW_R = AF_SW_R | AF_HW_W,                                        // hardware read-write/software read access
	SW_W = AF_SW_W | AF_HW_W,                                        // hardware read-write/software write access
	SW_RW = AF_SW_R | AF_SW_W | AF_HW_W,                             // hardware read-write/software read-write access (default)
	SW_W1 = AF_SW_W | AF_SW_W1 | AF_HW_W,                            // hardware write/software write once access
	SW_R_W1 = AF_SW_R | AF_SW_W | AF_SW_W1 | AF_HW_W,                // hardware read-write/software read-write once access
	SW_W1C = AF_SW_W | AF_SW_W1C | AF_HW_W,                          // hardware read-write/software write 1 clear access
	SW_W1_W1C = AF_SW_W | AF_SW_W1 | AF_SW_W1C | AF_HW_W,            // hardware read-write/software write one and 1 clear access
	SW_R_W1C = AF_SW_R | AF_SW_W | AF_SW_W1C | AF_HW_W,              // hardware read-write/software read-write 1 clear access
	SW_R_W1_W1C = AF_SW_R | AF_SW_W | AF_SW_W1 | AF_SW_W1C | AF_HW_W // hardware read-write/software read-write once and write 1 clear access
};

std::ostream& operator << (std::ostream& os, const Access& access);

////////////////////////////// ReadWriteStatusFlag///////////////////////////////

enum ReadWriteStatusFlag
{
	RWSF_WOORV         = 1,                                   // writing out-of-range value
	RWSF_WROR          = 2,                                   // writing read-only register
	RWSF_WROB          = 4,                                   // writing read-only bits
	RWSF_RWOR          = 8,                                   // reading write-only register
	RWSF_ANA           = 16,                                  // access not allowed
	RWSF_UA            = 32                                   // unmapped access
};

//////////////////////////////// ReadWriteStatus ////////////////////////////////

enum ReadWriteStatus
{
	RWS_OK            = 0,                                   // OK
	RWS_WOORV         = RWSF_WOORV,                          // writing out-of-range value
	RWS_WROR          = RWSF_WROR,                           // writing read-only register
	RWS_WROB          = RWSF_WROB,                           // writing read-only bits
	RWS_RWOR          = RWSF_RWOR,                           // reading write-only register
	RWS_WOORV_WROR    = RWS_WOORV | RWS_WROR,                // writing out-of-range value and writing read-only register
	RWS_WOORV_WROB    = RWS_WOORV | RWS_WROB,                // writing out-of-range value and writing read-only bits
	RWS_ANA           = RWSF_ANA,                            // access not allowed
	RWS_WOORV_NA      = RWS_WOORV | RWS_ANA,                 // writing out-of-range value and access not allowed
	RWS_WROR_NA       = RWS_WROR | RWS_ANA,                  // writing read-only register and access not allowed
	RWS_WROB_NA       = RWS_WROB | RWS_ANA,                  // writing read-only bits and access not allowed
	RWS_RWOR_NA       = RWS_RWOR | RWS_ANA,                  // reading write-only register and access not allowed
	RWS_WOORV_WROR_NA = RWS_WOORV | RWS_WROR | RWS_ANA,      // writing out-of-range value and writing read-only register and access not allowed
	RWS_WOORV_WROB_NA = RWS_WOORV | RWS_WROB | RWS_ANA,      // writing out-of-range value and writing read-only bits and access not allowed
	RWS_UA            = RWSF_UA                              // unmapped access
};

std::ostream& operator << (std::ostream& os, const ReadWriteStatus& ws);

inline bool IsReadWriteError(ReadWriteStatus rws) ALWAYS_INLINE;

inline bool IsReadWriteError(ReadWriteStatus rws) { return (rws & (RWSF_ANA | RWSF_UA)) != 0; }

inline bool IsUnmappedAccessError(ReadWriteStatus rws) { return (rws & RWSF_UA) != 0; }

inline bool IsAccessNotAllowedError(ReadWriteStatus rws) { return (rws & RWSF_ANA) != 0; }

inline void SetReadWriteStatusFlag(ReadWriteStatus& rws, ReadWriteStatusFlag rwsf) { rws = ReadWriteStatus(rws | rwsf); }

inline void ClearReadWriteStatusFlag(ReadWriteStatus& rws, ReadWriteStatusFlag rwsf) { rws = ReadWriteStatus(rws & ~rwsf); }

////////////////////////// Forward declarations ///////////////////////////////

class PropertyRegistry;

class LongPrettyPrinter;

template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH, Access _ACCESS> class Field;

struct NullField;

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
         , typename BF60, typename BF61, typename BF62, typename BF63> struct FieldSet;

template <typename ADDRESS, typename CUSTOM_RW_ARG> class RegisterAddressMap;

template <unsigned int BYTE_SIZE> struct TypeForByteSize;

template <unsigned int SIZE> struct TypeForBitSize;

struct NullRegisterBase;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename REGISTER_BASE> class Register;

template <typename CUSTOM_RW_ARG> class AddressableRegisterBase;

template <typename CUSTOM_RW_ARG> class AddressableRegisterFileBase;

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS, typename CUSTOM_RW_ARG> class AddressableRegister;

template <typename ADDRESS, typename CUSTOM_RW_ARG> class AddressableRegisterHandle;

template <typename ADDRESS, typename CUSTOM_RW_ARG> class RegisterAddressMap;

////////////////////////////////// Same<> /////////////////////////////////////

template <typename T1, typename T2>
struct Same
{
	enum { VALUE = false };
};

template <typename T>
struct Same<T, T>
{
	enum { VALUE = true };
};

/////////////////////////////// NullCustomArg /////////////////////////////////

struct NullCustomArg {};

///////////////////////////// PropertyRegistry ////////////////////////////////

class PropertyRegistry
{
public:
	enum ElementType
	{
		EL_REGISTER      = 0,
		EL_FIELD         = 1,
		EL_REGISTER_FILE = 2
	};
	
	enum StringPropertyType
	{
		STR_PROP_NAME      = 0,
		STR_PROP_DISP_NAME = 1,
		STR_PROP_DESC      = 2
	};

	static void SetStringProperty(ElementType el_type, StringPropertyType prop_type, intptr_t key, const std::string& value);
	static const std::string& GetStringProperty(ElementType el_type, StringPropertyType prop_type, intptr_t key);
	
private:
	static std::map<intptr_t, std::string> string_prop_registry[3][3];
	
};

///////////////////////////// LongPrettyPrinter ///////////////////////////////

class LongPrettyPrinter
{
public:
	static const unsigned int COLUMN_SPACING_PRINT_WIDTH = 2;
	static const unsigned int KIND_PRINT_WIDTH           = 8;
	static const unsigned int RANGE_PRINT_WIDTH          = 8;
	static const unsigned int NAME_PRINT_WIDTH           = 16;
	static const unsigned int VALUE_PRINT_WIDTH          = 23; // for at most 64-bit numbers in octal format
	static const unsigned int ACCESS_PRINT_WIDTH         = 46;
	
	static void Print(std::ostream& os, const std::string& s, unsigned int max_length);
	static void PrintHeader(std::ostream& os);
	static void PrintKind(std::ostream& os, const std::string& kind);
	static void PrintRegisterSize(std::ostream& os, unsigned int register_size);
	static void PrintFieldRange(std::ostream& os, unsigned int bitoffset, unsigned int bitwidth);
	static void PrintDisplayName(std::ostream& os, const std::string& display_name);
	template <typename T> static void PrintValue(std::ostream& os, const T& value);
	static void PrintAccess(std::ostream& os, const Access& access);
	static void PrintDescription(std::ostream& os, const std::string& s);
};

//////////////////////////////// FieldID ///////////////////////////////////

unsigned int FieldID();

////////////////////////////////// Field<> ////////////////////////////////////


template <typename FIELD, unsigned int _BITOFFSET, unsigned int _BITWIDTH = 1, Access _ACCESS = SW_RW>
class Field
{
public:
	static const unsigned int BITOFFSET = _BITOFFSET;
	static const unsigned int BITWIDTH = _BITWIDTH;
	static const Access ACCESS = _ACCESS;
	static const unsigned int BITWIDTH_MINUS_1 = _BITWIDTH - 1;
	
	static const unsigned int ID;

	template <typename T> static inline T GetMask() ALWAYS_INLINE;
	template <typename T> static inline T GetAssignMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteOneClearMask() ALWAYS_INLINE;
	template <typename T> static inline T GetReadMask() ALWAYS_INLINE;
	template <typename T> static inline bool HasOverlappingBitFields();
	template <typename T> static inline T Mask(const T& storage) ALWAYS_INLINE;
	template <typename T> static inline T Get(const T& storage) ALWAYS_INLINE;
	template <typename T> static inline void Set(T& storage, T field_value) ALWAYS_INLINE;
	template <typename T> static inline T MakeValue(T field_value) ALWAYS_INLINE;
	static inline void SetName(const std::string& name);
	static inline void SetDisplayName(const std::string& disp_name);
	static inline void SetDescription(const std::string& desc);
	static inline unsigned int GetBitOffset();
	static inline unsigned int GetBitWidth();
	static inline Access GetAccess();
	static inline bool IsReadable();
	static inline bool IsWritable();
	static inline bool IsWriteOnce();
	static inline bool IsReadOnly();
	static inline bool IsWriteOnly();
	static inline bool IsWriteOnlyAndOnce();
	static inline const std::string& GetName();
	static inline const std::string& GetDisplayName();
	static inline const std::string& GetDescription();
	template <typename T> static inline void ShortPrettyPrint(std::ostream& os, const T& storage);
	template <typename T> static inline void LongPrettyPrint(std::ostream& os, const T& storage);
	template <typename VISITOR> static inline void ForEach(VISITOR& visitor);
private:
	static inline const std::string& GetNameKey();
	static inline const std::string& GetDisplayNameKey();
	static inline const std::string& GetDescriptionKey();
};

/////////////////////////////// MakeMask ///////////////////////////////////

template <typename T, unsigned int BITWIDTH> T MakeMask();

/////////////////////////////// NullField //////////////////////////////////

struct NullField
{
	static const unsigned int BITWIDTH = 0;
	static const unsigned int BITOFFSET = 0;
	
	template <typename T> static inline T GetMask() ALWAYS_INLINE;
	template <typename T> static inline T GetAssignMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteOneClearMask() ALWAYS_INLINE;
	template <typename T> static inline T GetReadMask() ALWAYS_INLINE;
	template <typename T> static inline T Get(const T& storage) ALWAYS_INLINE;
	template <typename T> static inline void Set(T& storage, T field_value) ALWAYS_INLINE;
	static void SetName(const std::string& name);
	static void SetDisplayName(const std::string& disp_name);
	static void SetDescription(const std::string& desc);
	static const std::string& GetName();
	static const std::string& GetDescription();
	template <typename T> static void ShortPrettyPrint(std::ostream& os, const T& storage);
	template <typename T> static void LongPrettyPrint(std::ostream& os, const T& storage);
	template <typename VISITOR> static void ForEach(VISITOR& visitor);
};

/////////////////////////////// FieldSet<> /////////////////////////////////

template < typename  BF0 = NullField, typename  BF1 = NullField, typename  BF2 = NullField, typename  BF3 = NullField
         , typename  BF4 = NullField, typename  BF5 = NullField, typename  BF6 = NullField, typename  BF7 = NullField
         , typename  BF8 = NullField, typename  BF9 = NullField, typename BF10 = NullField, typename BF11 = NullField
         , typename BF12 = NullField, typename BF13 = NullField, typename BF14 = NullField, typename BF15 = NullField
         , typename BF16 = NullField, typename BF17 = NullField, typename BF18 = NullField, typename BF19 = NullField
         , typename BF20 = NullField, typename BF21 = NullField, typename BF22 = NullField, typename BF23 = NullField
         , typename BF24 = NullField, typename BF25 = NullField, typename BF26 = NullField, typename BF27 = NullField
         , typename BF28 = NullField, typename BF29 = NullField, typename BF30 = NullField, typename BF31 = NullField
         , typename BF32 = NullField, typename BF33 = NullField, typename BF34 = NullField, typename BF35 = NullField
         , typename BF36 = NullField, typename BF37 = NullField, typename BF38 = NullField, typename BF39 = NullField
         , typename BF40 = NullField, typename BF41 = NullField, typename BF42 = NullField, typename BF43 = NullField
         , typename BF44 = NullField, typename BF45 = NullField, typename BF46 = NullField, typename BF47 = NullField
         , typename BF48 = NullField, typename BF49 = NullField, typename BF50 = NullField, typename BF51 = NullField
         , typename BF52 = NullField, typename BF53 = NullField, typename BF54 = NullField, typename BF55 = NullField
         , typename BF56 = NullField, typename BF57 = NullField, typename BF58 = NullField, typename BF59 = NullField
         , typename BF60 = NullField, typename BF61 = NullField, typename BF62 = NullField, typename BF63 = NullField>
struct FieldSet
{
	static const unsigned int BITWIDTH = BF0::BITWIDTH + BF1::BITWIDTH + BF2::BITWIDTH + BF3::BITWIDTH + BF4::BITWIDTH + BF5::BITWIDTH + BF6::BITWIDTH + BF7::BITWIDTH +
	                                     BF8::BITWIDTH + BF9::BITWIDTH + BF10::BITWIDTH + BF11::BITWIDTH + BF12::BITWIDTH + BF13::BITWIDTH + BF14::BITWIDTH + BF15::BITWIDTH +
	                                     BF16::BITWIDTH + BF17::BITWIDTH + BF18::BITWIDTH + BF19::BITWIDTH + BF20::BITWIDTH + BF21::BITWIDTH + BF22::BITWIDTH + BF23::BITWIDTH +
	                                     BF24::BITWIDTH + BF25::BITWIDTH + BF26::BITWIDTH + BF27::BITWIDTH + BF28::BITWIDTH + BF29::BITWIDTH + BF30::BITWIDTH + BF31::BITWIDTH +
	                                     BF32::BITWIDTH + BF33::BITWIDTH + BF34::BITWIDTH + BF35::BITWIDTH + BF36::BITWIDTH + BF37::BITWIDTH + BF38::BITWIDTH + BF39::BITWIDTH +
	                                     BF40::BITWIDTH + BF41::BITWIDTH + BF42::BITWIDTH + BF43::BITWIDTH + BF44::BITWIDTH + BF45::BITWIDTH + BF46::BITWIDTH + BF47::BITWIDTH +
	                                     BF48::BITWIDTH + BF49::BITWIDTH + BF50::BITWIDTH + BF51::BITWIDTH + BF52::BITWIDTH + BF53::BITWIDTH + BF54::BITWIDTH + BF55::BITWIDTH +
	                                     BF56::BITWIDTH + BF57::BITWIDTH + BF58::BITWIDTH + BF59::BITWIDTH + BF60::BITWIDTH + BF61::BITWIDTH + BF62::BITWIDTH + BF63::BITWIDTH;
	template <typename T> static inline T GetMask() ALWAYS_INLINE;
	template <typename T> static inline T GetAssignMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteMask() ALWAYS_INLINE;
	template <typename T> static inline T GetWriteOneClearMask() ALWAYS_INLINE;
	template <typename T> static inline T GetReadMask() ALWAYS_INLINE;
	template <typename T> static inline bool HasOverlappingBitFields();
	template <typename T> static inline T Get(const T& storage) ALWAYS_INLINE;
	template <typename T> static inline void Set(T& storage, T value) ALWAYS_INLINE;
	template <typename VISITOR> static inline void ForEach(VISITOR& visitor);
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

////////////////////////////// NullRegisterBase ///////////////////////////////

struct NullRegisterBase {};

////////////////////////////////// Register<> /////////////////////////////////

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS, typename _REGISTER_BASE>
std::ostream& operator << (std::ostream& os, const Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& reg);

template <typename _REGISTER, unsigned int _SIZE, Access _ACCESS = SW_RW, typename _REGISTER_BASE = NullRegisterBase>
class Register : public _REGISTER_BASE
{
public:
	typedef typename TypeForBitSize<_SIZE>::TYPE TYPE;
	static const TYPE TYPE_MASK = TypeForBitSize<_SIZE>::MASK;
	typedef _REGISTER REGISTER;
	static const unsigned int SIZE = _SIZE;
	static const Access ACCESS = _ACCESS;
	typedef _REGISTER_BASE REGISTER_BASE;
	
	inline TYPE Get() const ALWAYS_INLINE;
	inline void Set(TYPE value) ALWAYS_INLINE;
	inline bool HasBit(unsigned int bit_offset) const ALWAYS_INLINE;
	inline TYPE Get(unsigned int bit_offset) const ALWAYS_INLINE;
	inline void Set(unsigned int bit_offset, TYPE bit_value) ALWAYS_INLINE;
	template <typename FIELD> inline bool HasField() const ALWAYS_INLINE;
	template <typename FIELD> inline TYPE Get() const ALWAYS_INLINE;
	template <typename FIELD> inline void Set(TYPE field_value) ALWAYS_INLINE;
	
	inline TYPE GetMask() const ALWAYS_INLINE;
	inline TYPE GetAssignMask() const ALWAYS_INLINE;
	inline TYPE GetWriteMask() const ALWAYS_INLINE;
	inline TYPE GetWriteOneClearMask() const ALWAYS_INLINE;
	inline TYPE GetReadMask() const ALWAYS_INLINE;
	
	inline bool NeedCheckForOverlappingBitFields() const { return true; }
	
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
	struct _32 : Field<_32, 32> {};
	struct _33 : Field<_33, 33> {};
	struct _34 : Field<_34, 34> {};
	struct _35 : Field<_35, 35> {};
	struct _36 : Field<_36, 36> {};
	struct _37 : Field<_37, 37> {};
	struct _38 : Field<_38, 38> {};
	struct _39 : Field<_39, 39> {};
	struct _40 : Field<_40, 40> {};
	struct _41 : Field<_41, 41> {};
	struct _42 : Field<_42, 42> {};
	struct _43 : Field<_43, 43> {};
	struct _44 : Field<_44, 44> {};
	struct _45 : Field<_45, 45> {};
	struct _46 : Field<_46, 46> {};
	struct _47 : Field<_47, 47> {};
	struct _48 : Field<_48, 48> {};
	struct _49 : Field<_49, 49> {};
	struct _50 : Field<_50, 50> {};
	struct _51 : Field<_51, 51> {};
	struct _52 : Field<_52, 52> {};
	struct _53 : Field<_53, 53> {};
	struct _54 : Field<_54, 54> {};
	struct _55 : Field<_55, 55> {};
	struct _56 : Field<_56, 56> {};
	struct _57 : Field<_57, 57> {};
	struct _58 : Field<_58, 58> {};
	struct _59 : Field<_59, 59> {};
	struct _60 : Field<_60, 60> {};
	struct _61 : Field<_61, 61> {};
	struct _62 : Field<_62, 62> {};
	struct _63 : Field<_63, 63> {};
	
	Register();
	Register(TYPE value);
	Register(const Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& r);
	void WithinRegisterFileCtor(unsigned int index, void *custom_ctor_arg);
	void Initialize(TYPE value);
	inline Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& operator = (const TYPE& value) ALWAYS_INLINE;
	inline operator const TYPE () const ALWAYS_INLINE;
	inline TYPE operator [] (unsigned int bit_offset) const ALWAYS_INLINE;
	ReadWriteStatus Write(const TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	template <typename PRESERVED_FIELD> ReadWriteStatus WritePreserve(const TYPE& value, const TYPE& bit_enable);
	ReadWriteStatus Read(TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	void DebugWrite(const TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	void DebugRead(TYPE& value, const TYPE& bit_enable = (~TYPE(0) & TYPE_MASK));
	ReadWriteStatus Write(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0);
	ReadWriteStatus Read(unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0);
	void DebugWrite(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0);
	void DebugRead(unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0);
	void ShortPrettyPrint(std::ostream& os) const;
	void LongPrettyPrint(std::ostream& os) const;
	
	friend std::ostream& operator << <_REGISTER, _SIZE>(std::ostream& os, const Register<_REGISTER, _SIZE, _ACCESS, _REGISTER_BASE>& reg);
	
	void SetName(const std::string& name);
	void SetDisplayName(const std::string& disp_name);
	void SetDescription(const std::string& desc);
	unsigned int GetSize() const;
	Access GetAccess() const;
	bool IsReadable() const;
	bool IsWritable() const;
	bool IsWriteOnce() const;
	bool IsReadOnly() const;
	bool IsWriteOnly() const;
	bool IsWriteOnlyAndOnce() const;
	const std::string& GetName() const;
	const std::string& GetDisplayName() const;
	const std::string& GetDescription() const;
	
	unisim::service::interfaces::Register *CreateRegisterInterface();
	
	inline bool IsVerboseRead() const ALWAYS_INLINE;
	inline bool IsVerboseWrite() const ALWAYS_INLINE;
	inline std::ostream& GetInfoStream() ALWAYS_INLINE;
private:
	TYPE value;
	const std::string GetNameKey() const;
	const std::string GetDisplayNameKey() const;
	const std::string GetDescriptionKey() const;
	
	inline bool __IsVerboseRead__() const ALWAYS_INLINE;
	inline bool __IsVerboseWrite__() const ALWAYS_INLINE;
	inline std::ostream& __GetInfoStream__() ALWAYS_INLINE;
	
	struct FieldsShortPrettyPrinter
	{
		FieldsShortPrettyPrinter(std::ostream& _os, const TYPE& _value) : os(_os), value(_value), first(true) {}
		
		template <typename FIELD> void operator () ()
		{
			if(!FIELD::GetName().empty())
			{
				if(!first) os << ':';
				first = false;
				FIELD::ShortPrettyPrint(os, value & TYPE_MASK);
			}
		}
		
		std::ostream& os;
		const TYPE& value;
		bool first;
	};

	struct FieldsLongPrettyPrinter
	{
		FieldsLongPrettyPrinter(std::ostream& _os, const TYPE& _value) : os(_os), value(_value), first(true) {}
		
		template <typename FIELD> void operator () ()
		{
			if(!FIELD::GetName().empty())
			{
				if(!first) os << std::endl;
				first = false;
				FIELD::LongPrettyPrint(os, value & TYPE_MASK);
			}
		}
		
		std::ostream& os;
		const TYPE& value;
		bool first;
	};
};

//////////////////////////// FieldInterface<> /////////////////////////////////

template <typename REGISTER, typename FIELD>
struct FieldInterface : unisim::service::interfaces::Field
{
	FieldInterface(REGISTER& _reg) : reg(_reg) {}

	virtual const char *GetName() const { return FIELD::GetName().c_str(); }
	virtual const char *GetDescription() const { return FIELD::GetDescription().c_str(); }
	virtual unsigned int GetBitOffset() const { return FIELD::BITOFFSET; }
	virtual unsigned int GetBitWidth() const { return FIELD::BITWIDTH; }
	virtual uint64_t GetValue() const { return reg.template Get<FIELD>(); }
	virtual void SetValue(uint64_t val) { reg.template Set<FIELD>(val); }
	
private:
	REGISTER& reg;
};

/////////////////////////// RegisterInterface<> ///////////////////////////////

template <typename REGISTER>
struct RegisterInterface : public unisim::service::interfaces::Register
{
	typedef unisim::util::reg::core::Register<REGISTER, REGISTER::SIZE, REGISTER::ACCESS, typename REGISTER::REGISTER_BASE> RegType;
	RegisterInterface(REGISTER& _reg) : reg(_reg) { Visitor visitor(reg, *this); REGISTER::ALL::ForEach(visitor); }
	virtual const char *GetName() const { return reg.GetName().c_str(); }
	virtual const char *GetDescription() const { return reg.GetDescription().c_str(); }
	virtual void GetValue(void *buffer) const { reg.RegType::DebugRead((unsigned char *) buffer, (const unsigned char *) 0); }
	virtual void SetValue(const void *buffer) { reg.RegType::DebugWrite((const unsigned char *) buffer, (const unsigned char *) 0); }
	virtual int GetSize() const { return (reg.GetSize() + 7) / 8; }
	virtual void ScanFields(unisim::service::interfaces::FieldScanner& scanner) { fields.ScanFields(scanner); }
	void AddFieldInterface(unisim::service::interfaces::Field *field_if) { fields.AddFieldInterface(field_if); }
	unisim::service::interfaces::Field *GetField(const char *name) { return fields.GetField(name); }
private:
	struct Visitor
	{
		Visitor(REGISTER& _reg, RegisterInterface& _reg_if) : reg(_reg), reg_if(_reg_if) {}
		
		template <typename FIELD> void operator () () { if(!FIELD::GetName().empty()) reg_if.AddFieldInterface(new FieldInterface<REGISTER, FIELD>(reg)); }
		
		REGISTER& reg;
		RegisterInterface& reg_if;
	};
	
	REGISTER& reg;
	unisim::util::debug::SimpleFieldRegistry fields;
};

/////////////////////////// NullRegisterFileBase //////////////////////////////

struct NullRegisterFileBase {};

/////////////////////////////// RegisterFile<> ////////////////////////////////

template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG = NullCustomArg, typename REGISTER_FILE_BASE = NullRegisterFileBase>
class RegisterFile : public REGISTER_FILE_BASE
{
public:
	RegisterFile(CUSTOM_CTOR_ARG *custom_ctor_arg = 0);
	RegisterFile(typename REGISTER::TYPE value, CUSTOM_CTOR_ARG *custom_ctor_arg = 0);
	inline REGISTER& operator [] (int index) ALWAYS_INLINE;
	inline const REGISTER& operator [] (int index) const ALWAYS_INLINE;
	
	inline unsigned int GetDim() const ALWAYS_INLINE;
	inline unsigned int GetSize() const ALWAYS_INLINE;
	
	void SetName(const std::string& name);
	void SetDisplayName(const std::string& disp_name);
	void SetDescription(const std::string& desc);
	
	const std::string& GetName() const;
	const std::string& GetDisplayName() const;
	const std::string& GetDescription() const;
private:
	REGISTER regs[_DIM];
	
	const std::string& GetNameKey() const;
	const std::string& GetDisplayNameKey() const;
	const std::string& GetDescriptionKey() const;
};

///////////////////////// AddressableRegisterBase<> ///////////////////////////

// Note: this class is internal, it is not intended to be used directly by the user
//       use AddressableRegister instead
template <typename CUSTOM_RW_ARG = NullCustomArg>
class AddressableRegisterBase
{
public:
	virtual void ShortPrettyPrint(std::ostream& os) const = 0;
	virtual void LongPrettyPrint(std::ostream& os) const = 0;
protected:
	virtual unsigned int __ARB_GetSize__() const = 0;
	virtual const std::string& __ARB_GetName__() const = 0;
	virtual ReadWriteStatus __ARB_Write__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr) = 0;
	virtual ReadWriteStatus __ARB_Read__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr) = 0;
	virtual void __ARB_DebugWrite__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr) = 0;
	virtual void __ARB_DebugRead__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr) = 0;
	
	template <typename ADDRESS, typename _CUSTOM_RW_ARG> friend class AddressableRegisterHandle;
	template <typename ADDRESS, typename _CUSTOM_RW_ARG> friend class RegisterAddressMap;
};

/////////////////////// AddressableRegisterFileBase<> /////////////////////////
// Note: this class is internal, it is not intended to be used directly by the user
//       use AddressableRegisterFile instead
template <typename CUSTOM_RW_ARG = NullCustomArg>
class AddressableRegisterFileBase
{
protected:
	virtual unsigned int __ARFB_GetDim__() const = 0;
	virtual const std::string& __ARFB_GetName__() const = 0;
	virtual AddressableRegisterBase<CUSTOM_RW_ARG> *__ARFB_GetRegister__(unsigned int index) = 0;
	
	template <typename ADDRESS, typename _CUSTOM_RW_ARG> friend class RegisterAddressMap;
};

/////////////////////////// AddressableRegister<> /////////////////////////////

template <typename REGISTER, unsigned int _SIZE, Access _ACCESS = SW_RW, typename CUSTOM_RW_ARG = NullCustomArg>
class AddressableRegister : public Register<REGISTER, _SIZE, _ACCESS, AddressableRegisterBase<CUSTOM_RW_ARG> >
{
public:
	typedef Register<REGISTER, _SIZE, _ACCESS, AddressableRegisterBase<CUSTOM_RW_ARG> > Super;
	
	AddressableRegister();
	AddressableRegister(typename Super::TYPE value);
	AddressableRegister(const AddressableRegister<REGISTER, _SIZE, _ACCESS, CUSTOM_RW_ARG>& ar);

	using Super::operator =;

public:
	virtual void ShortPrettyPrint(std::ostream& os) const;
	virtual void LongPrettyPrint(std::ostream& os) const;
	virtual ReadWriteStatus Write(const typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual ReadWriteStatus Read(typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual void DebugWrite(const typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual void DebugRead(typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual ReadWriteStatus Write(CUSTOM_RW_ARG& custom_rw_arg, const typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual ReadWriteStatus Read(CUSTOM_RW_ARG& custom_rw_arg, typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual void DebugWrite(CUSTOM_RW_ARG& custom_rw_arg, const typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
	virtual void DebugRead(CUSTOM_RW_ARG& custom_rw_arg, typename Super::TYPE& value, const typename Super::TYPE& bit_enable);
private:
	virtual unsigned int __ARB_GetSize__() const;
	virtual const std::string& __ARB_GetName__() const;
	virtual ReadWriteStatus __ARB_Write__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr);
	virtual ReadWriteStatus __ARB_Read__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr);
	virtual void __ARB_DebugWrite__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr);
	virtual void __ARB_DebugRead__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr);
};

////////////////////////// AddressableRegisterFile<> //////////////////////////
template <typename REGISTER, unsigned int _DIM, typename CUSTOM_CTOR_ARG = NullCustomArg, typename CUSTOM_RW_ARG = NullCustomArg>
class AddressableRegisterFile : public RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, AddressableRegisterFileBase<CUSTOM_RW_ARG> >
{
public:
	typedef RegisterFile<REGISTER, _DIM, CUSTOM_CTOR_ARG, AddressableRegisterFileBase<CUSTOM_RW_ARG> > Super;
	
	AddressableRegisterFile(CUSTOM_CTOR_ARG *custom_ctor_arg = 0);
	AddressableRegisterFile(typename REGISTER::TYPE value, CUSTOM_CTOR_ARG *custom_ctor_arg = 0);
	
	using Super::operator [];
private:
	virtual unsigned int __ARFB_GetDim__() const;
	virtual const std::string& __ARFB_GetName__() const;
	virtual AddressableRegisterBase<CUSTOM_RW_ARG> *__ARFB_GetRegister__(unsigned int index);
};

////////////////////////// AddressableRegisterHandle<> ////////////////////////

template <typename ADDRESS, typename CUSTOM_RW_ARG = NullCustomArg>
class AddressableRegisterHandle
{
public:
	AddressableRegisterHandle(ADDRESS addr, unsigned int size, AddressableRegisterBase<CUSTOM_RW_ARG> *arb);
	
	ADDRESS GetAddress() const { return addr; }
	unsigned int GetSize() const { return size; }
	const std::string& GetName() const { return arb->__ARB_GetName__(); }
	ReadWriteStatus Write(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0) { return arb->__ARB_Write__((CUSTOM_RW_ARG *) 0, data_ptr, bit_enable_ptr); }
	ReadWriteStatus Read(unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0)  { return arb->__ARB_Read__((CUSTOM_RW_ARG *) 0, data_ptr, bit_enable_ptr); }
	void DebugWrite(const unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0) { return arb->__ARB_DebugWrite__((CUSTOM_RW_ARG *) 0, data_ptr, bit_enable_ptr); }
	void DebugRead(unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0)  { return arb->__ARB_DebugRead__((CUSTOM_RW_ARG *) 0, data_ptr, bit_enable_ptr); }
	ReadWriteStatus Write(CUSTOM_RW_ARG& custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0) { return arb->__ARB_Write__(&custom_rw_arg, data_ptr, bit_enable_ptr); }
	ReadWriteStatus Read(CUSTOM_RW_ARG& custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0)  { return arb->__ARB_Read__(&custom_rw_arg, data_ptr, bit_enable_ptr); }
	void DebugWrite(CUSTOM_RW_ARG& custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0) { return arb->__ARB_DebugWrite__(&custom_rw_arg, data_ptr, bit_enable_ptr); }
	void DebugRead(CUSTOM_RW_ARG& custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr = 0)  { return arb->__ARB_DebugRead__(&custom_rw_arg, data_ptr, bit_enable_ptr); }
	void ShortPrettyPrint(std::ostream& os) const { arb->ShortPrettyPrint(os); }
	void LongPrettyPrint(std::ostream& os) const { arb->LongPrettyPrint(os); }
private:
	friend class RegisterAddressMap<ADDRESS, CUSTOM_RW_ARG>;
	
	ADDRESS addr;
	unsigned int size; // in bytes (with padding)
	AddressableRegisterBase<CUSTOM_RW_ARG> *arb;
	unsigned int ref_count;
	
	void Acquire();
	void Release();

	ReadWriteStatus __ARH_Write__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr) { return arb->__ARB_Write__(custom_rw_arg, data_ptr, bit_enable_ptr); }
	ReadWriteStatus __ARH_Read__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr)  { return arb->__ARB_Read__(custom_rw_arg, data_ptr, bit_enable_ptr); }
	void __ARH_DebugWrite__(CUSTOM_RW_ARG *custom_rw_arg, const unsigned char *data_ptr, const unsigned char *bit_enable_ptr) { return arb->__ARB_DebugWrite__(custom_rw_arg, data_ptr, bit_enable_ptr); }
	void __ARH_DebugRead__(CUSTOM_RW_ARG *custom_rw_arg, unsigned char *data_ptr, const unsigned char *bit_enable_ptr)  { return arb->__ARB_DebugRead__(custom_rw_arg, data_ptr, bit_enable_ptr); }
};

///////////////////////////// RegisterAddressMap<> ////////////////////////////

template <typename ADDRESS, typename CUSTOM_RW_ARG = NullCustomArg>
class RegisterAddressMap
{
public:
	RegisterAddressMap();
	virtual ~RegisterAddressMap();
	void SetWarningStream(std::ostream& warning_stream);
	void SetEndian(unisim::util::endian::endian_type endian);
	void MapRegister(ADDRESS addr, AddressableRegisterBase<CUSTOM_RW_ARG> *reg, unsigned int reg_byte_size = 0 /* in bytes (with padding) */);
	void MapRegisterFile(ADDRESS addr, AddressableRegisterFileBase<CUSTOM_RW_ARG> *regfile, unsigned int reg_byte_size = 0 /* in bytes (with padding) */, unsigned int stride = 0 /* in bytes */);
	void UnmapRegistersAt(ADDRESS addr, unsigned int byte_size);
	void ClearRegisterMap();
	AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *FindAddressableRegister(ADDRESS addr) const;
	
	ReadWriteStatus Write(ADDRESS addr, const unsigned char *data_ptr, unsigned int data_length);
	ReadWriteStatus Read(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	unsigned int DebugWrite(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	unsigned int DebugRead(ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	
	ReadWriteStatus Write(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, const unsigned char *data_ptr, unsigned int data_length);
	ReadWriteStatus Read(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	unsigned int DebugWrite(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	unsigned int DebugRead(CUSTOM_RW_ARG& custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
private:
	mutable bool optimized;
	mutable bool optimizable;
	mutable std::pair<ADDRESS, ADDRESS> addr_range;
	mutable std::vector<AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *> opt_reg_addr_map;
	std::map<ADDRESS, AddressableRegisterHandle<ADDRESS, CUSTOM_RW_ARG> *> reg_addr_map;
	std::ostream *warning_stream;
	unisim::util::endian::endian_type data_endian;

	void Optimize() const;
	
	ReadWriteStatus Write(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, const unsigned char *data_ptr, unsigned int data_length);
	ReadWriteStatus Read(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	unsigned int DebugWrite(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
	unsigned int DebugRead(CUSTOM_RW_ARG *custom_rw_arg, ADDRESS addr, unsigned char *data_ptr, unsigned int data_length);
};

} // end of namespace core
} // end of namespace reg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_REG_CORE_REGISTER_HH__
