/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_TYPE_HH__
#define __UNISIM_UTIL_DEBUG_TYPE_HH__

#include <inttypes.h>
#include <string>
#include <vector>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {

typedef enum
{
	T_UNKNOWN = 0,
	T_CHAR = 1,
	T_INTEGER = 2,
	T_FLOAT = 3,
	T_BOOL = 4,
	T_STRUCT = 5,
	T_UNION = 6,
	T_CLASS = 7,
	T_INTERFACE = 8,
	T_ARRAY = 9,
	T_POINTER = 10,
	T_TYPEDEF = 11,
	T_FUNCTION = 12,
	T_CONST = 13,
	T_ENUM = 14,
	T_VOID = 15,
	T_VOLATILE = 16
} TYPE_CLASS;

typedef enum
{
	TINIT_TOK_LITERAL = 0,
	TINIT_TOK_BEGIN_OF_STRUCT = 1,
	TINIT_TOK_END_OF_STRUCT = 2,
	TINIT_TOK_STRUCT_MEMBER_SEPARATOR = 3,
	TINIT_TOK_BEGIN_OF_ARRAY = 4,
	TINIT_TOK_END_OF_ARRAY = 5,
	TINIT_TOK_ARRAY_ELEMENT_SEPARATOR = 6
} TypeInitializerToken;

class Type;
class BaseType;
class IntegerType;
class CharType;
class FloatingPointType;
class BooleanType;
class Member;
class StructureType;
class ArraySubRange;
class ArrayType;
class PointerType;
class Typedef;
class FormalParameter;
class FunctionType;
class ConstType;
class EnumType;
class UnspecifiedType;
class VolatileType;
class TypeVisitor;

std::ostream& operator << (std::ostream& os, const Type& type);
std::ostream& operator << (std::ostream& os, const IntegerType& integer_type);
std::ostream& operator << (std::ostream& os, const CharType& char_type);
std::ostream& operator << (std::ostream& os, const FloatingPointType& floating_point_type);
std::ostream& operator << (std::ostream& os, const BooleanType& boolean_type);
std::ostream& operator << (std::ostream& os, const Member& member);
std::ostream& operator << (std::ostream& os, const StructureType& structure_type);
std::ostream& operator << (std::ostream& os, const ArraySubRange& array_sub_range);
std::ostream& operator << (std::ostream& os, const ArrayType& array_type);
std::ostream& operator << (std::ostream& os, const PointerType& pointer_type);
std::ostream& operator << (std::ostream& os, const Typedef& _typedef);
std::ostream& operator << (std::ostream& os, const FormalParameter& formal_param);
std::ostream& operator << (std::ostream& os, const FunctionType& func_type);
std::ostream& operator << (std::ostream& os, const ConstType& const_type);
std::ostream& operator << (std::ostream& os, const EnumType& enum_type);
std::ostream& operator << (std::ostream& os, const UnspecifiedType& unspecified_type);
std::ostream& operator << (std::ostream& os, const VolatileType& volatile_type);

class Type
{
public:
	Type();
	Type(TYPE_CLASS type_class);
	virtual ~Type();
	
	TYPE_CLASS GetClass() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	TYPE_CLASS type_class;
	
	friend std::ostream& operator << (std::ostream& os, const Type& type);
};

class BaseType : public Type
{
public:
	BaseType(TYPE_CLASS type_class, unsigned int bit_size);
	virtual ~BaseType();
	unsigned int GetBitSize() const;
private:
	unsigned int bit_size;
};

class IntegerType : public BaseType
{
public:
	IntegerType(unsigned int bit_size, bool is_signed);
	virtual ~IntegerType();
	bool IsSigned() const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	bool is_signed;
	
	friend std::ostream& operator << (std::ostream& os, const IntegerType& integer_type);
};

class CharType : public BaseType
{
public:
	CharType(unsigned int bit_size, bool is_signed);
	virtual ~CharType();
	bool IsSigned() const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	bool is_signed;
	
	friend std::ostream& operator << (std::ostream& os, const CharType& char_type);
};

class FloatingPointType : public BaseType
{
public:
	FloatingPointType(unsigned int bit_size);
	virtual ~FloatingPointType();
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	friend std::ostream& operator << (std::ostream& os, const FloatingPointType& floating_point_type);
};

class BooleanType : public BaseType
{
public:
	BooleanType(unsigned int bit_size);
	virtual ~BooleanType();
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	friend std::ostream& operator << (std::ostream& os, const BooleanType& boolean_type);
};

class Member
{
public:
	Member(const char *name, const Type *type, uint64_t bit_size);
	virtual ~Member();
	const char *GetName() const;
	const Type *GetType() const;
	uint64_t GetBitSize() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	std::string name;
	const Type *type;
	uint64_t bit_size;
	
	friend std::ostream& operator << (std::ostream& os, const Member& member);
};

class StructureType : public Type
{
public:
	StructureType(TYPE_CLASS type_class, const char *name);
	virtual ~StructureType();
	
	void Add(const Member *member);
	unsigned int GetMemberCount() const;
	const Member *GetMember(unsigned int idx) const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	std::string name;
	std::vector<const Member *> members;
	
	friend std::ostream& operator << (std::ostream& os, const StructureType& structure_type);
};

class ArrayType : public Type
{
public:
	ArrayType(const Type *type_of_element, int64_t lower_bound, int64_t upper_bound);
	virtual ~ArrayType();
	const Type *GetTypeOfElement() const;
	int64_t GetLowerBound() const;
	int64_t GetUpperBound() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	const Type *type_of_element;
	int64_t lower_bound;
	int64_t upper_bound;
	
	friend std::ostream& operator << (std::ostream& os, const ArrayType& array_type);
};

class PointerType : public Type
{
public:
	PointerType(const Type *type_of_dereferenced_object);
	virtual ~PointerType();
	const Type *GetTypeOfDereferencedObject() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	const Type *type_of_dereferenced_object;
	
	friend std::ostream& operator << (std::ostream& os, const PointerType& pointer_type);
};

class Typedef : public Type
{
public:
	Typedef(const Type *type, const char *name);
	virtual ~Typedef();
	const Type *GetType() const;
	const char *GetName() const;
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	const Type *type;
	std::string name;
	
	friend std::ostream& operator << (std::ostream& os, const Typedef& _typedef);
};

class FormalParameter
{
public:
	FormalParameter(const char *name, const Type *type);
	virtual ~FormalParameter();
	const char *GetName() const;
	const Type *GetType() const;
private:
	std::string name;
	const Type *type;
	
	friend std::ostream& operator << (std::ostream& os, const FormalParameter& formal_param);
};

class FunctionType : public Type
{
public:
	FunctionType(const Type *return_type);
	virtual ~FunctionType();
	
	void Add(const FormalParameter *formal_param);
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	const Type *return_type;
	std::vector<const FormalParameter *> formal_params;
	
	friend std::ostream& operator << (std::ostream& os, const FunctionType& func_type);
};

class ConstType : public Type
{
public:
	ConstType(const Type *type);
	virtual ~ConstType();
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
	virtual std::string BuildCDecl(const char *data_object_name = 0) const;
private:
	const Type *type;
	friend std::ostream& operator << (std::ostream& os, const ConstType& const_type);
};

class Enumerator
{
public:
	Enumerator(const char *name);
	virtual ~Enumerator();
	const char *GetName() const;
private:
	std::string name;
	friend std::ostream& operator << (std::ostream& os, const Enumerator& enumerator);
};

class EnumType : public Type
{
public:
	EnumType(const char *name);
	virtual ~EnumType();
	void Add(const Enumerator *enumerator);
private:
	std::string name;
	std::vector<const Enumerator *> enumerators;
	
	friend std::ostream& operator << (std::ostream& os, const EnumType& enum_type);
};

class UnspecifiedType : public Type
{
public:
	UnspecifiedType();
	virtual ~UnspecifiedType();
private:
	friend std::ostream& operator << (std::ostream& os, const UnspecifiedType& unspecified_type);
};

class VolatileType : public Type
{
public:
	VolatileType(const Type *type);
	virtual ~VolatileType();
	virtual void DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const;
private:
	const Type *type;
	friend std::ostream& operator << (std::ostream& os, const VolatileType& volatile_type);
};

class TypeVisitor
{
public:
	virtual ~TypeVisitor() {}
	virtual void Visit(const char *path, const Type *type, TypeInitializerToken tok = TINIT_TOK_LITERAL) const = 0;
	virtual void Visit(const Member *member) const = 0;
private:
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__
