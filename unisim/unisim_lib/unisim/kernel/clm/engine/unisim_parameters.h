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

#ifndef __UNISIM_PARAMETERS_H__
#define __UNISIM_PARAMETERS_H__

#include <string>
#include <list>
#include <stdexcept>
#include "unisim_inttypes.h"

using namespace std;

//#include <iostream>
//#include <sstream>
//#include <iomanip>

class unisim_module;

/**
 * \brief Exception raised when searching for a non existing parmameter
 */
struct UnknownParameterException : runtime_error
{UnknownParameterException(const string &s) : runtime_error(s) {}
};

/**
 * \brief Exception raised when a parameter is converted to a mismatching type
 */
struct ParameterTypeMismatchException : runtime_error
{ParameterTypeMismatchException(const string &s) : runtime_error(s) {}
};


class ModuleParameterType
{private:
  enum parameter_type_t {tINVALID,tINT,tUINT64,tBOOL};
 public:
  ModuleParameterType();
  ModuleParameterType(uint64_t _value);
  ModuleParameterType(int _value);
  ModuleParameterType(bool _value);
  //Casters
  operator uint64_t();
  operator bool();
  operator int();
  //Methods
  friend ostream & operator<<(ostream &os, const ModuleParameterType::parameter_type_t &t);
  friend ostream & operator<<(ostream &os, const ModuleParameterType &t);
// private:
  //Attributes
  int      value_int;
  uint64_t value_uint64;
  bool     value_bool;
  parameter_type_t value_type;
};

/**
 * \brief Module parameter class
 */
class ModuleParameter
{public:
  //Constructors
  ModuleParameter(const string &_name, const ModuleParameterType &_value, bool _has_minmax, const ModuleParameterType & _min, const ModuleParameterType & _max);
  //Casters
  operator ModuleParameterType&();
  operator uint64_t();
  operator bool();
  operator int();
  //Methods
  ModuleParameterType & min();
  ModuleParameterType & max();
  void dump(const string &module_name) const;
  bool operator==(const string &_name) const;
  void dump_machine_description(ostream &os) const;
  friend ostream & operator<<(ostream &os, const ModuleParameter &p);
 private:
  //Attributes
  string name;
  bool     has_minmax;
  ModuleParameterType value, value_min, value_max;
};

class ModuleParameters
{public:
   ModuleParameters();
   template <typename T> void add(const string &name, T value)
   { params.push_back(ModuleParameter(name,value,false,value,value));
   }
   template <typename T> void add(const string &name, T value, T value_min, T value_max)
   { params.push_back(ModuleParameter(name,value,true,value_min,value_max));
   }
   void dump(const string &module_name) const;
   void dump_machine_description(ostream &os);
   ModuleParameter get_parameter(const string &pname);
 private:
   list < ModuleParameter > params;
};

#endif
