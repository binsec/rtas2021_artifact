/*
 *  Copyright (c) 2013,
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

#include <unisim/util/debug/type.hh>
#include <iostream>
#include <sstream>
#include <queue>

namespace unisim {
namespace util {
namespace debug {

Type::Type()
	: type_class(T_UNKNOWN)
{
}

Type::Type(TYPE_CLASS _type_class)
	: type_class(_type_class)
{
}

Type::~Type()
{
}

TYPE_CLASS Type::GetClass() const
{
	return type_class;
}

void Type::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	visitor->Visit(path.c_str(), this, TINIT_TOK_LITERAL);
}

std::string Type::BuildCDecl(const char *data_object_name) const
{
	return std::string("unknown_type ") + (data_object_name ? data_object_name : "");
}

std::ostream& operator << (std::ostream& os, const Type& type)
{
	switch(type.type_class)
	{
		case T_UNKNOWN:
			return os << "unknown_type";
		case T_CHAR:
			return os << *((const CharType *) &type);
		case T_INTEGER:
			return os << *((const IntegerType *) &type);
		case T_FLOAT:
			return os << *((const FloatingPointType *) &type);
		case T_BOOL:
			return os << *((const BooleanType *) &type);
		case T_STRUCT:
			return os << *((const StructureType *) &type);
		case T_UNION:
			return os << *((const StructureType *) &type);
		case T_CLASS:
			return os << *((const StructureType *) &type);
		case T_INTERFACE:
			return os << *((const StructureType *) &type);
		case T_ARRAY:
			return os << *((const ArrayType *) &type);
		case T_POINTER:
			return os << *((const PointerType *) &type);
		case T_TYPEDEF:
			return os << *((const Typedef *) &type);
		case T_FUNCTION:
			return os << *((const FunctionType *) &type);
		case T_CONST:
			return os << *((const ConstType *) &type);
		case T_ENUM:
			return os << *((const EnumType *) &type);
		case T_VOID:
			return os << *((const UnspecifiedType *) &type);
		case T_VOLATILE:
			return os << *((const VolatileType *) &type);
	}
	return os;
}

BaseType::BaseType(TYPE_CLASS _type_class, unsigned int _bit_size)
	: Type(_type_class)
	, bit_size(_bit_size)
{
}

BaseType::~BaseType()
{
}

unsigned int BaseType::GetBitSize() const
{
	return bit_size;
}

IntegerType::IntegerType(unsigned int _bit_size, bool _is_signed)
	: BaseType(T_INTEGER, _bit_size)
	, is_signed(_is_signed)
{
}

IntegerType::~IntegerType()
{
}

bool IntegerType::IsSigned() const
{
	return is_signed;
}

std::string IntegerType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	sstr << (is_signed ? "s" : "u") << "int";
	uint64_t bit_size = GetBitSize();
	if(bit_size) sstr << bit_size;
	if(data_object_name) sstr << " " << data_object_name;
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const IntegerType& integer_type)
{
	os << (integer_type.is_signed ? "s" : "u") << "int";
	uint64_t bit_size = integer_type.GetBitSize();
	if(bit_size) os << bit_size;
	return os;
}

CharType::CharType(unsigned int _bit_size, bool _is_signed)
	: BaseType(T_CHAR, _bit_size)
	, is_signed(_is_signed)
{
}

CharType::~CharType()
{
}

bool CharType::IsSigned() const
{
	return is_signed;
}

std::string CharType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	sstr << (is_signed ? "s" : "u") << "char";
	uint64_t bit_size = GetBitSize();
	
	if(bit_size) sstr << bit_size;
	if(data_object_name) sstr << " " << data_object_name;
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const CharType& char_type)
{
	os << (char_type.is_signed ? "s" : "u") << "char";
	uint64_t bit_size = char_type.GetBitSize();
	if(bit_size) os << bit_size;
	return os;
}

FloatingPointType::FloatingPointType(unsigned int bit_size)
	: BaseType(T_FLOAT, bit_size)
{
}

FloatingPointType::~FloatingPointType()
{
}

std::string FloatingPointType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	sstr << "float";
	uint64_t bit_size = GetBitSize();
	if(bit_size) sstr << bit_size;
	if(data_object_name) sstr << " " << data_object_name;
	
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const FloatingPointType& floating_point_type)
{
	os << "float";
	uint64_t bit_size = floating_point_type.GetBitSize();
	if(bit_size) os << bit_size;
	return os;
}

BooleanType::BooleanType(unsigned int _bit_size)
	: BaseType(T_BOOL, _bit_size)
{
}

BooleanType::~BooleanType()
{
}

std::string BooleanType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	sstr << "bool";
	uint64_t bit_size = GetBitSize();
	if(bit_size) sstr << bit_size;
	if(data_object_name) sstr << " " << data_object_name;
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const BooleanType& boolean_type)
{
	os << "bool";
	uint64_t bit_size = boolean_type.GetBitSize();
	if(bit_size) os << bit_size;
	return os;
}

Member::Member(const char *_name, const Type *_type, uint64_t _bit_size)
	: name(_name)
	, type(_type)
	, bit_size(_bit_size)
{
}

Member::~Member()
{
	delete type;
}

const char *Member::GetName() const
{
	return name.c_str();
}

const Type *Member::GetType() const
{
	return type;
}

uint64_t Member::GetBitSize() const
{
	return bit_size;
}

void Member::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	visitor->Visit(this);
	std::string member_path = path + '.' + name;
	type->DFS(member_path, visitor, follow_pointer);
}

std::ostream& operator << (std::ostream& os, const Member& member)
{
	os << *member.type;
	if(member.type->GetClass() != T_POINTER) os << " ";
	os << member.name;
	if(member.bit_size)
	{
		os << ":" << member.bit_size;
	}
	return os;
}

StructureType::StructureType(TYPE_CLASS _type_class, const char *_name)
	: Type(_type_class)
	, name(_name ? _name : "__anonymous__")
{
}

StructureType::~StructureType()
{
	unsigned int member_count = members.size();
	unsigned int i;
	for(i = 0; i < member_count; i++)
	{
		delete members[i];
	}
}

void StructureType::Add(const Member *_member)
{
	members.push_back(_member);
}

unsigned int StructureType::GetMemberCount() const
{
	return members.size();
}

const Member *StructureType::GetMember(unsigned int _idx) const
{
	unsigned int member_count = members.size();
	
	return (_idx < member_count) ? members[_idx] : 0;
}

void StructureType::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	visitor->Visit(path.c_str(), this, TINIT_TOK_BEGIN_OF_STRUCT);
	unsigned int member_count = members.size();
	unsigned int i;
	for(i = 0; i < member_count; i++)
	{
		const Member *member = members[i];
		member->DFS(path, visitor, follow_pointer);
		if(i != (member_count - 1))
		{
			visitor->Visit(path.c_str(), this, TINIT_TOK_STRUCT_MEMBER_SEPARATOR);
		}
	}
	visitor->Visit(path.c_str(), this, TINIT_TOK_END_OF_STRUCT);
}

std::string StructureType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	
	switch(GetClass())
	{
		case T_STRUCT:
			sstr << "struct";
			break;
		case T_UNION:
			sstr << "union";
			break;
		case T_CLASS:
			sstr << "class";
			break;
		case T_INTERFACE:
			sstr << "interface";
			break;
		default:
			return std::string();
	}
	sstr << " " << name;
	if(data_object_name) sstr << " " << data_object_name;
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const StructureType& structure_type)
{
	switch(structure_type.GetClass())
	{
		case T_STRUCT:
			os << "struct";
			break;
		case T_UNION:
			os << "union";
			break;
		case T_CLASS:
			os << "class";
			break;
		case T_INTERFACE:
			os << "interface";
			break;
		default:
			return os;
	}
	os << " " << structure_type.name;
	unsigned int member_count = structure_type.members.size();
	if(member_count)
	{
		os << " { ";
		unsigned int i;
		for(i = 0; i < member_count; i++)
		{
			os << *structure_type.members[i] << "; ";
		}
		os << "}";
	}
	return os;
}

ArrayType::ArrayType(const Type *_type_of_element, int64_t _lower_bound, int64_t _upper_bound)
	: Type(T_ARRAY)
	, type_of_element(_type_of_element)
	, lower_bound(_lower_bound)
	, upper_bound(_upper_bound)
{
}

ArrayType::~ArrayType()
{
	delete type_of_element;
}

const Type *ArrayType::GetTypeOfElement() const
{
	return type_of_element;
}

int64_t ArrayType::GetLowerBound() const
{
	return lower_bound;
}

int64_t ArrayType::GetUpperBound() const
{
	return upper_bound;
}

void ArrayType::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	visitor->Visit(path.c_str(), this, TINIT_TOK_BEGIN_OF_ARRAY);
	int64_t subscript;
	for(subscript = lower_bound; subscript <= upper_bound; subscript++)
	{
		std::stringstream element_path_sstr;
		element_path_sstr << path << '[' << subscript << ']';
		type_of_element->DFS(element_path_sstr.str(), visitor, follow_pointer);
		if(subscript != upper_bound)
		{
			visitor->Visit(path.c_str(), this, TINIT_TOK_ARRAY_ELEMENT_SEPARATOR);
		}
	}
	visitor->Visit(path.c_str(), this, TINIT_TOK_END_OF_ARRAY);
}

std::string ArrayType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	
	std::queue<const ArrayType *> fifo;
	
	const ArrayType *a = 0;
	const Type *t = this; 
	do
	{
		a = (const ArrayType *) t;
		fifo.push(a);
	}
	while((t = a->type_of_element)->GetClass() == T_ARRAY);
	
	sstr << t->BuildCDecl();
	if(data_object_name) sstr << " " << data_object_name;

	while(!fifo.empty())
	{
		a = fifo.front();
		fifo.pop();
		
		if(a->lower_bound <= a->upper_bound)
		{
			if(a->lower_bound)
				sstr << '[' << a->lower_bound << ".." << a->upper_bound << ']';
			else
				sstr << '[' << a->upper_bound + 1 << ']';
		}
		else
		{
			sstr << "[]";
		}
	}
	
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const ArrayType& array_type)
{
	std::queue<const ArrayType *> fifo;
	
	const ArrayType *a = 0;
	const Type *t = &array_type; 
	do
	{
		a = (const ArrayType *) t;
		fifo.push(a);
	}
	while((t = a->type_of_element)->GetClass() == T_ARRAY);
	
	os << *t;

	while(!fifo.empty())
	{
		a = fifo.front();
		fifo.pop();
		
		if(a->lower_bound <= a->upper_bound)
		{
			if(a->lower_bound)
				os << '[' << a->lower_bound << ".." << a->upper_bound << ']';
			else
				os << '[' << (a->upper_bound + 1) << ']';
		}
		else
		{
			os << "[]";
		}
	}
	
	return os;
}

PointerType::PointerType(const Type *_type_of_dereferenced_object)
	: Type(T_POINTER)
	, type_of_dereferenced_object(_type_of_dereferenced_object)
{
	
}

PointerType::~PointerType()
{
	delete type_of_dereferenced_object;
}

const Type *PointerType::GetTypeOfDereferencedObject() const
{
	return type_of_dereferenced_object;
}

void PointerType::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	if(follow_pointer)
	{
		std::string dereferenced_object_path = std::string("*(") + std::string(path) + ')';
		type_of_dereferenced_object->DFS(dereferenced_object_path, visitor, follow_pointer);
	}
	else
	{
		visitor->Visit(path.c_str(), this, TINIT_TOK_LITERAL);
	}
}

std::string PointerType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	if((type_of_dereferenced_object->GetClass() == T_FUNCTION) || (type_of_dereferenced_object->GetClass() == T_ARRAY))
	{
		std::string s("(*");
		if(data_object_name) s += data_object_name;
		s += ')';
		sstr << type_of_dereferenced_object->BuildCDecl(s.c_str());
	}
	else
	{
		sstr << type_of_dereferenced_object->BuildCDecl() << " *";
		if(data_object_name) sstr << data_object_name;
	}
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const PointerType& pointer_type)
{
	return os << *pointer_type.type_of_dereferenced_object << " *";
}

Typedef::Typedef(const Type *_type, const char *_name)
	: Type(T_TYPEDEF)
	, type(_type)
	, name(_name ? _name : "__anonymous__")
{
}

Typedef::~Typedef()
{
	delete type;
}

const Type *Typedef::GetType() const
{
	return type;
}

const char *Typedef::GetName() const
{
	return name.c_str();
}

std::string Typedef::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	sstr << name;
	if(data_object_name) sstr << " " << data_object_name;
	return sstr.str();
}

void Typedef::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	type->DFS(path, visitor, follow_pointer);
}

std::ostream& operator << (std::ostream& os, const Typedef& _typedef)
{
	return os << _typedef.name;
}

FormalParameter::FormalParameter(const char *_name, const Type *_type)
	: name(_name)
	, type(_type)
{
}

FormalParameter::~FormalParameter()
{
	delete type;
}

const char *FormalParameter::GetName() const
{
	return name.c_str();
}

const Type *FormalParameter::GetType() const
{
	return type;
}

std::ostream& operator << (std::ostream& os, const FormalParameter& formal_param)
{
	os << *formal_param.type;
	if(formal_param.type->GetClass() != T_POINTER) os << " ";
	os << formal_param.name;
	return os;
}

FunctionType::FunctionType(const Type *_return_type)
	: Type(T_FUNCTION)
	, return_type(_return_type)
{
}

FunctionType::~FunctionType()
{
	unsigned int formal_param_count = formal_params.size();
	unsigned int i;
	for(i = 0; i < formal_param_count; i++)
	{
		delete formal_params[i];
	}
	if(return_type) delete return_type;
}

void FunctionType::Add(const FormalParameter *formal_param)
{
	formal_params.push_back(formal_param);
}

std::string FunctionType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	
	if(return_type)
	{
		sstr << return_type->BuildCDecl();
	}
	else
	{
		sstr << "void";
	}
	sstr << " ";
	if(data_object_name) sstr << data_object_name;
	sstr << "(";
	unsigned int formal_param_count = formal_params.size();
	if(formal_param_count)
	{
		unsigned int i;
		for(i = 0; i < formal_param_count; i++)
		{
			sstr << formal_params[i]->GetType()->BuildCDecl();
			if(i != (formal_param_count - 1)) sstr << ", ";
		}
	}
	sstr << ")";
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const FunctionType& func_type)
{
	if(func_type.return_type)
	{
		os << *func_type.return_type;
	}
	else
	{
		os << "void";
	}
	os << " (";
	unsigned int formal_param_count = func_type.formal_params.size();
	if(formal_param_count)
	{
		unsigned int i;
		for(i = 0; i < formal_param_count; i++)
		{
			os << *func_type.formal_params[i];
			if(i != (formal_param_count - 1)) os << ", ";
		}
	}
	os << ")";
	return os;
}

ConstType::ConstType(const Type *_type)
	: Type(T_CONST)
	, type(_type)
{
}

ConstType::~ConstType()
{
	delete type;
}

void ConstType::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	type->DFS(path, visitor, follow_pointer);
}

std::string ConstType::BuildCDecl(const char *data_object_name) const
{
	std::stringstream sstr;
	
	sstr << type->BuildCDecl() << " const";
	if(data_object_name) sstr << " " << data_object_name;
	return sstr.str();
}

std::ostream& operator << (std::ostream& os, const ConstType& const_type)
{
	return os << *const_type.type << " const";
}

Enumerator::Enumerator(const char *_name)
	: name(_name)
{
}

Enumerator::~Enumerator()
{
}

std::ostream& operator << (std::ostream& os, const Enumerator& enumerator)
{
	return os << enumerator.name;
}

EnumType::EnumType(const char *_name)
	: Type(T_ENUM)
	, name(_name ? _name : "__anonymous__")
{
}

EnumType::~EnumType()
{
	unsigned int enumerator_count = enumerators.size();
	unsigned int i;
	for(i = 0; i < enumerator_count; i++)
	{
		delete enumerators[i];
	}
}

void EnumType::Add(const Enumerator *enumerator)
{
	enumerators.push_back(enumerator);
}

std::ostream& operator << (std::ostream& os, const EnumType& enum_type)
{
	os << "enum " << enum_type.name;
	return os;
}

UnspecifiedType::UnspecifiedType()
	: Type(T_VOID)
{
}

UnspecifiedType::~UnspecifiedType()
{
}

std::ostream& operator << (std::ostream& os, const UnspecifiedType& unspecified_type)
{
	return os << "void";
}

VolatileType::VolatileType(const Type *_type)
	: Type(T_CONST)
	, type(_type)
{
}

VolatileType::~VolatileType()
{
	delete type;
}

void VolatileType::DFS(const std::string& path, const TypeVisitor *visitor, bool follow_pointer) const
{
	type->DFS(path, visitor, follow_pointer);
}

std::ostream& operator << (std::ostream& os, const VolatileType& volatile_type)
{
	return os << "volatile " << *volatile_type.type;
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
