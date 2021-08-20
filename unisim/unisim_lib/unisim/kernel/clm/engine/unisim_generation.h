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

#ifndef __INCLUDE__UNISIM_GENERATION_H__
#define __INCLUDE__UNISIM_GENERATION_H__

using namespace std;

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <string>
#include <iostream>
#include <map>
#include <stdexcept> 
#include <sstream>

enum phase_t { phase_Include, phase_InstanceClasses, phase_ConstructorHead, phase_ConstructorBody} phase;
char char_class_separator;

/**
 * \brief Pretty printer for the phase_t enum
 */
std::ostream & operator<<(std::ostream &os, const phase_t p)
{ switch(p)
  { case phase_Include:          os << "phase_Include";         break;
    case phase_InstanceClasses:  os << "phase_InstanceClasses"; break;
    case phase_ConstructorHead:  os << "phase_ConstructorHead"; break;
    case phase_ConstructorBody:  os << "phase_ConstructorBody"; break;
  }
  return os;
}

class punt : public std::runtime_error
{public:
  punt(std::string &s) : std::runtime_error(s) { } 
  punt(std::string s) : std::runtime_error(s) { } 
  punt(const char *s) : std::runtime_error(std::string(s)) { } 
};

std::list<std::string> hierarchy;

#define GENERATION_FUNCTION_HEAD(x,...) class generator__class__##x { public: static void generate(__VA_ARGS__)
#define GENERATION_FUNCTION_TAIL(x) } ;

#define GENERATION_NAMESPACE_HEAD(x) struct x { x() { 
#define GENERATION_NAMESPACE_TAIL(x) } } __ns_instance__##x;

#define HIER_MODULE_INSTANTIATION(x,y,z,...) hierarchy.push_back(x); generator__class__##z::generate(__VA_ARGS__); hierarchy.pop_back();


/**
 * \brief Generates a #include entry
 */
void generate_include(const char *file)
{ if (phase == phase_Include) 
  { cout << "#include \"" << file << "\"" << endl;
  }
}

/**
 * \brief Generates a 'using namespace' entry
 */
void generate_using(const char *ns)
{ if (phase == phase_Include)
  { cout << "using namespace " << ns << ";" << endl;
  }
}

void generate_array_instance(const char *name, int width, const char *type)
{
  /*
  if (phase == phase_InstanceClasses) cout << "  " << type << "  *" << name << "[];" << endl;
  */
}

std::string get_hierarchy_string()
{ std::string res = "";  
  for(std::list<std::string>::iterator i = hierarchy.begin(); i != hierarchy.end(); i++)
  { res += *i;
    res += "__";
  }
  return res;
}

void generate_instance(const char *name, int width, const char *type,const char *clockname,int numtemplates,...)
{ char templatestring[255];
  char *tmpptr = templatestring;
  va_list argp;
  templatestring[0] = 0;
  char displayname[256];
  
  stringstream define, undefine;
  stringstream classname_stream, displayname_stream, instancename_stream;

  string hname = get_hierarchy_string();
  classname_stream << hname << name;
  displayname_stream << hname << name;
  instancename_stream << hname << name;

  if(width != -1)
  { classname_stream    << "__867__" << width << "__5309";
    instancename_stream << "__867__" << width << "__5309";
    displayname_stream  << "[" << width << "]";
  }
  string classname = classname_stream.str();

  va_start(argp,  numtemplates);
  for (int i=0;i<numtemplates;i++)
  { *tmpptr++=(i==0 ? '<' : ',');
    char *paramname = va_arg(argp,char *);
    char fmt[3];
    fmt[0] = '%'; fmt[2] = 0;
    fmt[1] = va_arg(argp,int);

    if (fmt[1] == 'i')      sprintf(tmpptr,fmt,va_arg(argp,int));
    else if (fmt[1] == 's') sprintf(tmpptr,fmt,va_arg(argp,char *));
    else
    { cerr << "Did not understand template parameter '" << fmt[1] << "'!\n" << endl;
      exit(1);
    }

    define << "#define " << paramname << " " << tmpptr << "\n";
    undefine << "#undef " << paramname << "\n";

    tmpptr+=strlen(tmpptr);
  }
  if (numtemplates > 0)
  { *tmpptr++='>';
    *tmpptr++=0;      
  }
  
  if (numtemplates == 0) strcpy(templatestring,"<>");

  if(phase == phase_InstanceClasses)
  { cout << "  class " << classname << "__class: public " << type << templatestring << endl;
    cout << "  {public:" << endl;
    cout << define.str();
    cout << "    " << classname << "__class(char *modulename) : " << type << templatestring << "(modulename)" << endl;
    cout << "    { class_name = \"" << type << "\";" << endl;
    cout << "      port_naming();\n";
    cout << "    }\n\n";
    int numuserpoints = va_arg(argp,int);
    for (int i=0;i<numuserpoints;i++)
    { char *rettype = va_arg(argp,char*);
      char *upname = va_arg(argp,char*);
      char *params = va_arg(argp,char*);
      cout << "#ifdef UP__" << classname << "__" << upname << endl;
      cout << "    virtual " << rettype << " " << upname << "(" << params << ") {" << endl;
      cout << "      UP__" << classname << "__" << upname << endl;
      cout << "    }" << endl;
      cout << "#endif\n" << endl;
    }
    cout << "    void port_naming()" << endl;
    cout << "    { // Associate each module port with its name" << endl;
    int numports = va_arg(argp,int);
    { for (int i=0;i<numports;i++)
      { char *portname = va_arg(argp,char*);
        char *portwidth = va_arg(argp,char*);
        if(strlen(portwidth)==0)
        { // This is not an array port
          cout << "      " << portname << ".set_unisim_name(this,\"" << portname << "\");" << endl;
        }
        else
        { // This is an array port
          cout << "      for(int __i=0;__i<" << portwidth << ";__i++)" << endl;
          cout << "      { " << portname << "[__i].set_unisim_name(this,\"" << portname << "\",__i);" << endl;
          cout << "      }" << endl;
        }
      }
    }
    cout << "    }\n" << endl;
    cout << undefine.str();
    cout << "  }; // " << classname << "__class" << endl;
    cout << "  " << classname << "__class  " << instancename_stream.str() << ";\n" << endl;
  } 
  else if (phase == phase_ConstructorHead)
  { // The constructor initialize each module through inheritance to allow naming.
    cout << "  " << char_class_separator << " " << instancename_stream.str() <<"(\"" << displayname_stream.str() << "\")\n";
    char_class_separator = ',';
  }
  else if (phase == phase_ConstructorBody)
  { // Connect each clock signal to a global clock
    if(strcmp(clockname,"UNKNOWN_CLOCK")) cout << "    " << instancename_stream.str() << "." << clockname << "(global_clock);" << endl;
  }
  va_end(argp);
}


void generate_connection(const char *modulea,int mindexa,const char *porta, int indexa,
			 const char *moduleb,int mindexb,const char *portb, int indexb,
			 int width = -1)
{ if (phase == phase_ConstructorBody)
  { if (width != -1)
    { if ((indexa != -1) || (indexb != -1))
	throw punt("Cannot explicitly specify port numbers for bus connections");
    }

    std::string h = get_hierarchy_string();

    if (width != -1) cout << "    for (int __i=0;__i<" << width << ";__i++)";
    cout << "    " << h.c_str() << modulea;
    if (mindexa != -1) cout << "__867__" << mindexa << "__5309";
    cout << "." << porta;
    if (width != -1) cout << "[__i]";
    if (indexa != -1) cout << "[" << indexa << "]";
    cout << " >> " << h.c_str() << moduleb;
    if (mindexb != -1) cout << "__867__" << mindexb<< "__5309";
    cout << "." << portb;
    if (width != -1)  cout << "[__i]";
    if (indexb != -1) cout << "[" << indexb << "]";
    cout << ";" << endl;
  }
}


#define punt(message) throw punt(message)
//#define width(x) __width_helper(#x)    conflicts with stl many classes have a width method !

std::map<std::string,int> __port_width_map;

template<class T>
class UnisimModuleParameter {
  T val;
  bool defaulted;

 public:
  UnisimModuleParameter() : defaulted(true) {    
  }

  virtual ~UnisimModuleParameter() {    
  }  

  T& operator =(T v) {
    val = v;
    defaulted = false;
    return val;
  }

  /*
  operator T&() {
    return val;
  }
  */
};

#define module_parameter(m,p) m.p
#define declare_module_parameter(t,m,p) UnisimModuleParameter< t > p
#define module_params_begin(M,T) struct { 
#define module_params_end(M,T) } M;

unsigned int __width_helper(const char *name) {
  std::string n = name;
  std::map<std::string,int>::iterator i = __port_width_map.find(n);

  /* Fixme...incorporate hierarchy*/

  if (i == __port_width_map.end()) {
    punt(std::string("width(")+n+"): " + n + " is not a valid port.");        
  }

  if (i->second == -1) 
    return 1;
  else
    return i->second;
}

void record_port_width(const char *name, int wid)
{ /* Fixme...incorporate hierarchy*/
  std::string n = name;
  std::map<std::string,int>::iterator i = __port_width_map.find(n);

  if (i != __port_width_map.end()) {
    punt(std::string("Multiple definition of port ")+n);
  }
  __port_width_map[n] = wid;
}

void generate_array_port(const char *name, const char *type, int wid)
{ record_port_width(name,wid);
}

void generate_port(const char *name, const char *type)
{ record_port_width(name,-1);
}

void generate_userpoint(const char *module, const char *name, const char *code) {
  if (phase == phase_Include)
  { std::string hname = get_hierarchy_string();
    printf("#define UP__%s__%s ",(hname+module).c_str(),name);
    for (; *code; code++)
    { if (*code == '\n') printf("\\");
      printf("%c",*code);
    }
    cout << "\n" << endl;
  }
}

void generate_head()
{ cout << "#include <csignal>" << endl;
  cout << "#include \"unisim.h\"" << endl;
  cout << "#include \"unisim_lse.h\"\n" << endl;
}

void unisim_include_if_exists(const char *filename)
{ FILE *f;
  f=fopen(filename,"r");
  if(f)
  { cout << "#include \"" << filename << "\"\n";
    fclose(f);
  }
  else 
  { cout << "//#include \"" << filename << "\" the comment is removed if the file exists\n";
  }
}

void generate_tail(const char *input_file)
{ cout << "  } //GeneratedSimulator()\n";
  cout << "}; //class GeneratedSimulator\n\n";
  unisim_include_if_exists("global_definitions.cxx");
  cout << "\nint main(int argc, char* argv[])\n";
  cout << "{\n";
  unisim_include_if_exists("prolog.cxx");
  cout << "  command_line.parse(argc,argv);\n";
  cout << "  GeneratedSimulator s;\n";
  cout << "  unisim_port::check_connections();\n";
  cout << "#include \""<< input_file << ".inc.init.cxx\"\n";
  cout << "  signal(SIGINT,GeneratedSimulator::at_ctrlc);\n";
  cout << "  signal(SIGTSTP,GeneratedSimulator::at_ctrlz);\n";
  unisim_include_if_exists("before_simloop.cxx");
  cout << "  while(!unisim_terminated)\n";
  cout << "  { fsc_phase();\n";
  cout << "    fsc_phase();\n";
  cout << "#include \"" << input_file << ".inc.eoc.cxx\"\n";
  cout << "    if(s._unknown_display) unisim_port::check_knowness(s.stream_knowness(),s._unknown_fatal);\n";
  cout << "  }\n";
  unisim_include_if_exists("epilog.cxx");
  cout << "  return 0;\n";
  cout << "} //main()\n" << endl;
}

void generate_runtime_function(const char *rettype, const char *name, const char *params, const char *body)
{ if (phase==phase_InstanceClasses)
  { printf("  %s %s%s%s\n",rettype,name,params,body);
  }
}


void generate_runtime_var(const char *type, const char *name, int width)
{ if (phase==phase_InstanceClasses)
  { if (width != -1) printf("  %s %s[%i];\n",type,name,width);
    else             printf("  %s %s;\n",type,name);
  }
}

#endif
