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
                 unisim_port.cpp  - UNISIM module class
                             -------------------
    begin                : Mon 17 2006
    author               : Sylvain Girbal
    email                : sylvain.girbal@inria.fr
 ***************************************************************************/

#ifndef UNISIM_COMMANDLINE_H
#define UNISIM_COMMANDLINE_H

#include <list>
#include <string>
#include <iostream>
#include <inttypes.h>

using namespace std;

/**
 * \brief Commandline parameter base class
 */
class unisim_commandline_parameter
{public:
  enum param_type  /// Type of the parameter (Flag, Option, ...)
  { UNKNOWN,      ///< Unkown parameter
    FLAG,         ///< Flag parameter
    OPTION,       ///< Option parameter (with an additionnal parameter)
    EXTRA,        ///< Extra parameter not starting by "-".
    EXTRA_OPT     ///< Extra optional parameters
  }; 
  unisim_commandline_parameter();
  unisim_commandline_parameter(param_type _type, const string &_name, const string &_arg, const string &_descr, void (*_func)());
  string name;                            ///< Name of the parameter
  string arg;                             ///< String representation of the argument (aka "<port>")
  string descr;                           ///< Description of the option
  string value;                           ///< Value associated to the option
  bool is_set;                            ///< Set to true if on the command line
  param_type type;                        ///< Parameter type (flag, option, ...)
  void (*func)(void);                     ///< Function to call if the parameter is set
  operator string();                      ///< Returns the parameter value as a string
  operator const char*();                 ///< Returns the parameter value as a string
  operator bool();                        ///< Returns true if the parameter is set
  operator uint64_t();                    ///< Returns the parameter value as an integer
  friend ostream & operator<<(ostream &os, const unisim_commandline_parameter &p);
  friend ostream & operator<<(ostream &os, const param_type p);         ///< Pretty printer of the param_type enum
};

/**
 * \brief Commandline class
 */
class unisim_commandline
{public:
  unisim_commandline();
  void add_flag(const string &name, const string &descr);
  void add_flag(const string &name, const string &descr, void (*func)());
  void add_option(const string &name, const string &arg, const string &descr);
  void add_extra(const string &name, const string &descr);
  void add_extra_opt(const string &name, const string &descr);
  void parse(int argc, char* argv[]);
  unisim_commandline_parameter & operator[](const string &name);
  const char* operator[](unsigned int index);
  void help();
  int count() const;
  friend ostream & operator<<(ostream &os, const unisim_commandline &c);
 private:
  list < unisim_commandline_parameter > parameters;
  list < unisim_commandline_parameter > extras_descr;
  list < string > extras;
  string binary_name;
  int option_count;
  int nooption_count;
};

extern unisim_commandline command_line;
#endif
