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

#include "unisim_commandline.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>

unisim_commandline command_line;
unisim_commandline_parameter unisim_commandline_parameter_unknown;


/**
 * \brief callback hook for the --help option
 */
void simulator_help()
{ command_line.help();
}

/**
 * \brief Creates an unknown commandline parameter
 */
unisim_commandline_parameter::unisim_commandline_parameter()
{ is_set = false;
  type = UNKNOWN;
  func = NULL;
}

/*
Creates a new commandline parameter
unisim_commandline_parameter::unisim_commandline_parameter(param_type _type, const string &_name, const string &_descr)
{ name = _name;
  descr = _descr;
  is_set = false;
  type = _type;
  func = NULL;
}
 */

/**
 * \brief Creates a new commandline parameter
 */
unisim_commandline_parameter::unisim_commandline_parameter(param_type _type, const string &_name, const string &_arg, const string &_descr, void (*_func)())
{ name = _name;
  arg = _arg;
  descr = _descr;
  is_set = false;
  type = _type;
  func = _func;
}


/**
 * Returns the parameter value as a string
 */
unisim_commandline_parameter::operator string()
{ return value;
}

/**
 * Returns the parameter value as a string
 */
unisim_commandline_parameter::operator const char*()
{ return value.c_str();
}

/**
 * Returns the parameter value as an integer
 */
unisim_commandline_parameter::operator uint64_t()
{ if(!is_set) return 0;
  stringstream ss(value);
  uint64_t i;
  ss >> i;
  return i;
}

/**
 * Returns true if the parameter is set
 */
unisim_commandline_parameter::operator bool()
{ return is_set;
}

/**
 * \brief Pretty printer of the unisim_commandline_parameter::param_type enum
 */
ostream & operator<<(ostream &os, const unisim_commandline_parameter::param_type p)
{ switch(p)
  { case unisim_commandline_parameter::UNKNOWN:    os << "UNKNOWN"; break;
    case unisim_commandline_parameter::FLAG:       os << "FLAG";    break;
    case unisim_commandline_parameter::OPTION:     os << "OPTION";  break;
    case unisim_commandline_parameter::EXTRA:      os << "EXTRA";  break;
    case unisim_commandline_parameter::EXTRA_OPT:  os << "EXTRA_OPT";  break;
  }
  return os;
}

/**
 * \brief Pretty printer of the unisim_commandline_parameter class
 */
ostream & operator<<(ostream &os, const unisim_commandline_parameter &p)
{ os << "unisim_commandline_parameter(";
  os << "type=" << p.type;
  if(p.type!=unisim_commandline_parameter::UNKNOWN)
  { if(p.is_set) os << ", set=true";
    else         os << ", set=false";
    os << ", name=" << p.name;
    if(p.is_set && p.type==unisim_commandline_parameter::OPTION)
    { os << ", value=" << p.value;
    }
  }
  os << ")";
  return os;
}

/**
 * \brief Create a new commandline object
 */
unisim_commandline::unisim_commandline()
{ add_flag("help","Diplays this help message",&simulator_help);
  add_option("check-known","<mode>", "Wether signals should be tested at the end of each cycle.\nThe <mode> can be one of FATAL, LOG or NONE.");
  option_count=0;
  nooption_count=0;
}

/**
 * \brief Add a new parsable flag to the commandline
 */
void unisim_commandline::add_flag(const string &name, const string &descr)
{ parameters.push_back(unisim_commandline_parameter(unisim_commandline_parameter::FLAG,name,"",descr,NULL));
}

/**
 * \brief Add a new parsable flag to the commandline
 */
void unisim_commandline::add_flag(const string &name, const string &descr, void (*_func)())
{ parameters.push_back(unisim_commandline_parameter(unisim_commandline_parameter::FLAG,name,"",descr,_func));
}

/**
 * \brief Add a new parsable option to the commandline
 */
void unisim_commandline::add_option(const string &name, const string &arg, const string &descr)
{ parameters.push_back(unisim_commandline_parameter(unisim_commandline_parameter::OPTION,name,arg,descr,NULL));
}

void unisim_commandline::add_extra(const string &name, const string &descr)
{ extras_descr.push_back(unisim_commandline_parameter(unisim_commandline_parameter::EXTRA,name,"",descr,NULL));
}

void unisim_commandline::add_extra_opt(const string &name, const string &descr)
{ extras_descr.push_back(unisim_commandline_parameter(unisim_commandline_parameter::EXTRA_OPT,name,"",descr,NULL));
}

/**
 * \brief Parse the command line to extract informations
 */
void unisim_commandline::parse(int argc, char* argv[])
{ int i = 1;
  binary_name = argv[0];
  bool within_options = true;
  while(i<argc)
  { char *s = argv[i];
    if(strlen(s)>0)
    { if(within_options && s[0] == '-')
      { while(s[0]=='-') s++;
        if(strlen(s)>0)
        { unisim_commandline_parameter &p = operator[](s);
          if(p.type==unisim_commandline_parameter::UNKNOWN)
          { cerr << "unisim_commzdline error: unexpected '" << s << "' on command line." << endl;
            exit(1);
          }
          p.is_set = true;
          switch(p.type)
          { case unisim_commandline_parameter::FLAG:
            { if(p.func) p.func();
            } break;
            case unisim_commandline_parameter::OPTION:
            { //cerr << "found existing OPTION parameter: " << s << endl;
              if(i==argc-1)
              { cerr << "error: Missing parameter for option " << s << "." << endl;
                exit(1);
              }
              p.value = argv[i+1];
              i++;
            } break;
            default:
              cerr << __FILE__ << ":" << __LINE__ << ": error: unknown parameter type." << endl;
              exit(1);
          }
          option_count++;
        }
      }
      else
      { nooption_count++;
        within_options = false;
        extras.push_back(argv[i]);
      }
    }
    else
    { cerr << "error: empty command line option." << endl;
      exit(1);
    }
    i++;
  }
}

/**
 * \brief Returns the parameter with a matching name
 */
unisim_commandline_parameter & unisim_commandline::operator[](const string &name)
{ list<unisim_commandline_parameter>::iterator p;
  for(p=parameters.begin(); p != parameters.end(); p++)
  { if(name==(*p).name) return *p;
  }
  return unisim_commandline_parameter_unknown;
}

/**
 * \brief Returns the extra parameter at the related index
 */
const char* unisim_commandline::operator[](unsigned int index)
{ list<string>::iterator p = extras.begin();
// DD //  if(index<parameters.size())
  if(index<extras.size())
  { for(unsigned int i=0;i<index;i++)
    { p++;
    }
    return p->c_str();
  }
  return "not found";
}

/**
 * \brief Display all the command line options
 */
void unisim_commandline::help()
{ cerr << "Usage: " << binary_name << " [options] ";
  list<unisim_commandline_parameter>::const_iterator p;
  for(p=extras_descr.begin(); p != extras_descr.end(); p++)
  { if(p->type==unisim_commandline_parameter::EXTRA) cerr << "<";
    if(p->type==unisim_commandline_parameter::EXTRA_OPT) cerr << "[";
    cerr << p->name;
    if(p->type==unisim_commandline_parameter::EXTRA) cerr << ">";
    if(p->type==unisim_commandline_parameter::EXTRA_OPT) cerr << "]";
    cerr << " ";
  }
  cerr << endl;
  for(p=extras_descr.begin(); p != extras_descr.end(); p++)
  { stringstream ss;
    ss << "  " << p->name;
    cerr << left << setw(40) << ss.str();
    cerr << p->descr << endl;
  }
  cerr << "Options:" << endl;
  for(p=parameters.begin(); p != parameters.end(); p++)
  { stringstream ss;
    string formated_descr = p->descr;
    size_t i;
    ss << " --" << (*p).name << " ";
    ss << p->arg;
    cerr << left << setw(40) << ss.str();
    i=formated_descr.find("\n");
    while(i!=string::npos)
    { formated_descr.replace(i,1,"\n                                        ");
      i=formated_descr.find("\n",i+1);
    }
    cerr << formated_descr << endl;
  }
  exit(1);
}

/**
 * \brief Pretty printer of the unisim_commandline class
 */
ostream & operator<<(ostream &os, const unisim_commandline &c)
{ list<unisim_commandline_parameter>::const_iterator p;
  os << "{" << endl;
  for(p=c.parameters.begin(); p != c.parameters.end(); p++)
  { os << "  " << (*p) << endl;
  }
  os << "}" << endl;
  return os;
}

/**
 * \brief Returns the number of parameters on the command line, not including the option
 * starting with "-".
 */
int unisim_commandline::count() const
{ return nooption_count;
}
