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

#ifndef __UNISIM_UTIL_DEBUG_DATA_OBJECT_INITIALIZER_TCC__
#define __UNISIM_UTIL_DEBUG_DATA_OBJECT_INITIALIZER_TCC__

#include <iostream>
#include <unisim/util/ieee754/ieee754.hh>

namespace unisim {
namespace util {
namespace debug {

template <class ADDRESS>
DataObjectInitializer<ADDRESS>::DataObjectInitializer(const DataObject<ADDRESS> *_data_object, ADDRESS _pc, const unisim::service::interfaces::DataObjectLookup<ADDRESS> *_data_object_lookup_if)
	: data_object(_data_object)
	, pc(_pc)
	, data_object_lookup_if(_data_object_lookup_if)
	, os(new std::stringstream())
{
}

template <class ADDRESS>
DataObjectInitializer<ADDRESS>::~DataObjectInitializer()
{
	if(os) delete os;
}

template <class ADDRESS>
void DataObjectInitializer<ADDRESS>::Visit(const char *data_object_name, const Type *type, TypeInitializerToken tok) const
{
	//std::cerr << "Visit: \"" << data_object_name << "\" of class " << type->GetClass() << ", token is " << tok << std::endl;
	switch(type->GetClass())
	{
		case T_UNKNOWN:
			break;
		case T_CHAR:
			{
				DataObject<ADDRESS> *data_object = data_object_lookup_if->FindDataObject(data_object_name, pc);
				
				if(data_object)
				{
					if(!data_object->IsOptimizedOut())
					{
						if(data_object->Fetch())
						{
							uint64_t data_object_bit_size = data_object->GetBitSize();
							uint64_t data_object_value = 0;
							if(data_object->Read(0, data_object_value, data_object_bit_size))
							{
								if((data_object_value >= 32) && (data_object_value < 128))
								{
									(*os) << "'" << (char) data_object_value << "' /* 0x" << std::hex << data_object_value << std::dec << " */";
								}
								else
								{
									(*os) << "'\\0x" << std::hex << data_object_value << std::dec << "'";
								}
							}
							else
							{
								(*os) << "<unreadable>";
							}
						}
						else
						{
							(*os) << "<unfetchable>";
						}
					}
					else
					{
						(*os) << "<optimized out>";
					}
					delete data_object;
				}
				else
				{
					(*os) << "<not found>";
				}
			}
			break;
		case T_INTEGER:
			{
				DataObject<ADDRESS> *data_object = data_object_lookup_if->FindDataObject(data_object_name, pc);
				
				if(data_object)
				{
					if(!data_object->IsOptimizedOut())
					{
						if(data_object->Fetch())
						{
							uint64_t data_object_bit_size = data_object->GetBitSize();
							uint64_t data_object_value = 0;
							if(data_object->Read(0, data_object_value, data_object_bit_size))
							{
								if(((const IntegerType *) type)->IsSigned())
								{
									(*os) << unisim::util::arithmetic::SignExtend(data_object_value, data_object_bit_size);
								}
								else
								{
									(*os) << data_object_value;
								}
							}
							else
							{
								(*os) << "<unreadable>";
							}
						}
						else
						{
							(*os) << "<unfetchable>";
						}
					}
					else
					{
						(*os) << "<optimized out>";
					}
					delete data_object;
				}
				else
				{
					(*os) << "<not found>";
				}
			}
			break;
		case T_FLOAT:
			{
				DataObject<ADDRESS> *data_object = data_object_lookup_if->FindDataObject(data_object_name, pc);
				
				if(data_object)
				{
					if(!data_object->IsOptimizedOut())
					{
						if(data_object->Fetch())
						{
							uint64_t data_object_bit_size = data_object->GetBitSize();
							uint64_t data_object_byte_size = (data_object_bit_size + 7 ) / 8;
							
							uint8_t data_object_raw_value[data_object_byte_size];
							if(data_object->Read(0, data_object_raw_value, 0, data_object_bit_size))
							{
								switch(data_object_bit_size)
								{
									case 32:
										{
											uint32_t value = 0;
											memcpy(&value, data_object_raw_value, 4);
											unisim::util::ieee754::SoftFloat sf_value = unisim::util::ieee754::SoftFloat(unisim::util::endian::Target2Host(data_object->GetEndian(), value));
											sf_value.writeDecimal(*os);
										}
										break;
									case 64:
										{
											uint64_t value = 0;
											memcpy(&value, data_object_raw_value, 8);
											unisim::util::ieee754::SoftDouble sd_value = unisim::util::ieee754::SoftDouble(unisim::util::endian::Target2Host(data_object->GetEndian(), value));
											sd_value.writeDecimal(*os);
										}
										break;
									default:
										(*os) << "<unprintable " << data_object_bit_size << "-bit floating-point value>";
										break;
								}
							}
							else
							{
								(*os) << "<unreadable>";
							}
						}
						else
						{
							(*os) << "<unfetchable>";
						}
					}
					else
					{
						(*os) << "<optimized out>";
					}
					delete data_object;
				}
				else
				{
					(*os) << "<not found>";
				}
			}
			break;
		case T_BOOL:
			{
				DataObject<ADDRESS> *data_object = data_object_lookup_if->FindDataObject(data_object_name, pc);
				if(data_object)
				{
					if(!data_object->IsOptimizedOut())
					{
						if(data_object->Fetch())
						{
							uint64_t data_object_bit_size = data_object->GetBitSize();
							uint64_t data_object_value = 0;
							if(data_object->Read(0, data_object_value, data_object_bit_size))
							{
								(*os) << (data_object_value ? "true" : "false");
							}
							else
							{
								(*os) << "<unreadable>";
							}
						}
						else
						{
							(*os) << "<unfetchable>";
						}
					}
					else
					{
						(*os) << "<optimized out>";
					}
					delete data_object;
				}
				else
				{
					(*os) << "<not found>";
				}
			}
			break;
		case T_STRUCT:
		case T_UNION:
		case T_CLASS:
		case T_INTERFACE:
			switch(tok)
			{
				case TINIT_TOK_BEGIN_OF_STRUCT:
					(*os) << "{ ";
					break;
				case TINIT_TOK_END_OF_STRUCT:
					(*os) << " }";
					break;
				case TINIT_TOK_STRUCT_MEMBER_SEPARATOR:
					(*os) << ", ";
					break;
				default:
					break;
			}
			break;
		case T_ARRAY:
			switch(tok)
			{
				case TINIT_TOK_BEGIN_OF_ARRAY:
					(*os) << "{ ";
					break;
				case TINIT_TOK_END_OF_ARRAY:
					(*os) << " }";
					break;
				case TINIT_TOK_ARRAY_ELEMENT_SEPARATOR:
					(*os) << ", ";
					break;
				default:
					break;
			}
			break;
		case T_POINTER:
			{
				DataObject<ADDRESS> *data_object = data_object_lookup_if->FindDataObject(data_object_name, pc);
				if(data_object)
				{
					if(!data_object->IsOptimizedOut())
					{
						if(data_object->Fetch())
						{
							uint64_t data_object_bit_size = data_object->GetBitSize();
							uint64_t data_object_value = 0;
							if(data_object->Read(0, data_object_value, data_object_bit_size))
							{
								(*os) << "@0x" << std::hex << data_object_value << std::dec;
							}
							else
							{
								(*os) << "<unreadable>";
							}
						}
						else
						{
							(*os) << "<unfetchable>";
						}
					}
					else
					{
						(*os) << "<optimized out>";
					}
					delete data_object;
				}
				else
				{
					(*os) << "<not found>";
				}
			}
			break;
		case T_TYPEDEF:
			break;
		case T_FUNCTION:
			(*os) << "<function>";
			break;
		case T_CONST:
			break;
		case T_ENUM:
			{
				DataObject<ADDRESS> *data_object = data_object_lookup_if->FindDataObject(data_object_name, pc);
				if(data_object)
				{
					if(!data_object->IsOptimizedOut())
					{
						if(data_object->Fetch())
						{
							uint64_t data_object_bit_size = data_object->GetBitSize();
							uint64_t data_object_value = 0;
							if(data_object->Read(0, data_object_value, data_object_bit_size))
							{
								(*os) << data_object_value;
							}
							else
							{
								(*os) << "<unreadable>";
							}
						}
						else
						{
							(*os) << "<unfetchable>";
						}
					}
					else
					{
						(*os) << "<optimized out>";
					}
					delete data_object;
				}
				else
				{
					(*os) << "<not found>";
				}
			}
			break;
		case T_VOID:
			(*os) << "<void>";
			break;
		case T_VOLATILE:
			break;
	}
}

template <class ADDRESS>
void DataObjectInitializer<ADDRESS>::Visit(const Member *member) const
{
	(*os) << '.' << member->GetName() << " = ";
}

template <class ADDRESS>
std::ostream& operator << (std::ostream& os, const DataObjectInitializer<ADDRESS>& data_object_initializer)
{
	const Type *data_object_type = data_object_initializer.data_object->GetType();
	
	std::string data_object_name = std::string(data_object_initializer.data_object->GetName());
	data_object_type->DFS(data_object_name, &data_object_initializer, false);
	os << (*data_object_initializer.os).str();
	data_object_initializer.os->clear();
	return os;
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DATA_OBJECT_INITIALIZER_TCC__
