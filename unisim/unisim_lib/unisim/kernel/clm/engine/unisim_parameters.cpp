/******************************************************************************* 
 *                                   BSD LICENSE 
 *******************************************************************************
 *  Copyright (c) 2006, CEA, INRIA and Universite Paris Sud
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
 *   - Neither the name of CEA, INRIA and Universite Paris Sud nor the names of its 
 *     contributors may be used to endorse or promote products derived from this 
 *     software without specific prior written permission.  
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
 ****************************************************************************** */ 

/***************************************************************************
         unisim_parameters.cpp - Information about module parameters
                             -------------------
    author               : Sylvain Girbal
    email                : sylvain.girbal@inria.fr
 ***************************************************************************/

#include <unisim_parameters.h>
#include <iostream>
#include <sstream>



//
// --- ModuleParameterType ----------------------------------------------------
//

ModuleParameterType::ModuleParameterType()
{ value_type = tINVALID;
}

ModuleParameterType::ModuleParameterType(uint64_t _value)
{ value_uint64 = _value;
  value_type = tUINT64;
}

ModuleParameterType::ModuleParameterType(int _value)
{ value_int = _value;
  value_type = tINT;
}

ModuleParameterType::ModuleParameterType(bool _value)
{ value_bool = _value;
  value_type = tBOOL;
}

ModuleParameterType::operator uint64_t()
{ if(value_type==tUINT64) return value_uint64;
  stringstream ss;
  ss << "Casting a " << value_type << " parameter to an uint64_t results in an invalid cast !";
  throw ParameterTypeMismatchException(ss.str());
}

ModuleParameterType::operator bool()
{ if(value_type==tBOOL) return value_bool;
  stringstream ss;
  ss << "Casting a " << value_type << " parameter to an bool results in an invalid cast !";
  throw ParameterTypeMismatchException(ss.str());
}

ModuleParameterType::operator int()
{ if(value_type==tINT) return value_int;
  stringstream ss;
  ss << "Casting a " << value_type << " parameter to an int results in an invalid cast !";
  throw ParameterTypeMismatchException(ss.str());
}

ostream & operator<<(ostream &os, const ModuleParameterType::parameter_type_t &t)
{ switch(t)
  { case ModuleParameterType::tINVALID:
      os << "INVALID";
      break;
    case ModuleParameterType::tINT:
      os << "int";
      break;
    case ModuleParameterType::tBOOL:
      os << "bool";
      break;
    case ModuleParameterType::tUINT64:
      os << "uint64_t";
      break;
  }
  return os;
}

/**
 * \brief Pretty printer of the ModuleParameterType class
 */
ostream & operator<<(ostream &os, const ModuleParameterType &t)
{ switch(t.value_type)
  { case ModuleParameterType::tINVALID:
      os << "INVALID";
      break;
    case ModuleParameterType::tINT:
      os << t.value_int;
      break;
    case ModuleParameterType::tUINT64:
      os << t.value_uint64;
      break;
    case ModuleParameterType::tBOOL:
      os << boolalpha << t.value_bool << noboolalpha;
      break;
  }
  return os;
}

//
// --- ModuleParameter --------------------------------------------------------
//

/**
 * \brief Creates a new ModuleParameter associated to a uint64_t value
 */
ModuleParameter::ModuleParameter(const string &_name, const ModuleParameterType &_value, bool _has_minmax, const ModuleParameterType &_min, const ModuleParameterType &_max)
{ name = _name;
  value = _value;
  value_min = _min;
  value_max = _max;
  has_minmax = _has_minmax;
}

ModuleParameter::operator ModuleParameterType&()
{ return value;
}

ModuleParameter::operator int()
{ return value;
}

ModuleParameter::operator bool()
{ return value;
}

ModuleParameter::operator uint64_t()
{ return value;
}

ModuleParameterType & ModuleParameter::min()
{ return value_min;
}

ModuleParameterType & ModuleParameter::max()
{ return value_max;
}

void ModuleParameter::dump(const string &module_name) const
{ cerr << module_name << "." << *this << endl;
}

ostream & operator<<(ostream &os, const ModuleParameter &p)
{ os << p.name << "=" << p.value;
  return os;
}

/**
 * \brief Returns true if the parameter name matches the provided name
 */
bool ModuleParameter::operator==(const string &_name) const
{ return (name==_name);
}

void ModuleParameter::dump_machine_description(ostream &os) const
{ os << "   <parameter name=\"" << name << "\">" << endl;
//  os << "    <name>" << name << "</name>" << endl;
  os << "    <type>" << value.value_type << "</type>" << endl;
  os << "    <value>" << value << "</value>" << endl;
  os << "   </parameter>" << endl;
}

//
// --- ModuleParameters -------------------------------------------------------
//

/**
 * \brief Creates a new empty ModuleParameters
 */
ModuleParameters::ModuleParameters()
{
}

/**
 * \brief Dump all the parameters of the current module
 */
void ModuleParameters::dump(const string &module_name) const
{ list<ModuleParameter>::const_iterator p;
  for(p=params.begin();p!=params.end();p++)
  { p->dump(module_name);
  }
}

/**
 * \brief Retreive a parameter from the parameter repository
 */
ModuleParameter ModuleParameters::get_parameter(const string &pname)
{ list<ModuleParameter>::const_iterator p;
  for(p=params.begin();p!=params.end();p++)
  { if(*p==pname)
    { return *p;
    }
  }
  throw UnknownParameterException(pname);
}

void ModuleParameters::dump_machine_description(ostream &os)
{ os << "  <parameters>" << endl;
  list<ModuleParameter>::const_iterator p;
  for(p=params.begin();p!=params.end();p++)
  { p->dump_machine_description(os);
  }
  os << "  </parameters>" << endl;
}
