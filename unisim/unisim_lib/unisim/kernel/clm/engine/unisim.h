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
                         unisim.h -- Main header file
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#ifndef __FRATERNITE_H__
#define __FRATERNITE_H__

#define LIBRARY_CORE_libunisim 0
#define LIBRARY_EXTRA_pthread 0

//#define USE_AUTOMATIC_SEND

/** 
 * @file unisim.h 
 * @brief UNISIM Library interface
 */

#include "unisim_inttypes.h"
#include "fsc.h"
#include "unisim_commandline.h"
#include "unisim_signal.h"
#include "unisim_parameters.h"

#define RUNTIME_TYPE_CHECKS


#ifdef RUNTIME_TYPE_CHECKS
#include <typeinfo>
#include <stdexcept>
#endif
#include <list>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>

#include <boost/array.hpp>

extern bool unisim_terminated;     ///< True if the simulation loop is finished
extern void terminate_now();

/**
 * \brief Userpoint defines parameterizable functions.
 *
 * from c++ point of view userpoints are virtual methods
 */
#define userpoint virtual

// so are collectors
/**
 * \brief Event allow to define Liberty collectors.
 */
#define event virtual void

/**
 * \brief Wrapper used to define sensitivities
 */
#define UNISIM_METHOD(process_name) { typedef typeof(*this) FSC_CURRENT_USER_MODULE; set_process(static_cast<fsc_method>(&FSC_CURRENT_USER_MODULE::process_name), #process_name); }

/**
 * \brief Defines a process sensitive to an incoming signal
 */
#define sensitive_method(x) UNISIM_METHOD(x); unisim_sensitive

/**
 * \brief Defines a process sensitive to the raising edge of the clock
 */
#define sensitive_pos_method(x) UNISIM_METHOD(x); unisim_sensitive_pos

/**
 * \brief Defines a process sensitive to the falling edge of a clock
 */
#define sensitive_neg_method(x) UNISIM_METHOD(x); unisim_sensitive_neg

/**
 * \brief cycle level module class
 */
#define module unisim_module

#ifdef LIBERTY
static inline void terminate_now() {
  LSE_sim_terminate_now = true;
}
#endif

class unisim_module;

//#ifdef USE_UNISIM_SIGNAL_ARRAY
template < class T, uint32_t NCONFIG=1, bool exists=true > class inport;
template < class T, uint32_t NCONFIG=1, bool exists=true > class outport;
//#else
//template < class T, bool exists=true > class inport;
//template < class T, bool exists=true > class outport;
//#endif

/**
 * \brief Signal-less port, both Input and Output port inherit from
 */
class unisim_port
{public:
  unisim_port();
  virtual ~unisim_port();
  static void check_connections();
  static void check_knowness(ostream & os, bool fatal_unknown);
  // DD the following function is used to display signals each cycle...
  static void check_signals(ostream & os, bool fatal_unknown);
  friend ostream & operator<<(ostream &os, const unisim_port &p);
  
  string get_name();
  string get_module_instance_name();
  string get_module_class_name();

  unisim_port * get_connected_port();
  unisim_module * get_connected_module();
 
  // DD Latex rendering ()
  void set_fused();
  bool is_fused();

 protected:
  static List<unisim_port> unisim_inport_list;  ///< static list of instantiated input ports
  static List<unisim_port> unisim_outport_list; ///< static list of instantiated output ports
  virtual bool check_my_knowness();             ///< Check that the 3 signals are known

  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  virtual void display_my_knowness(ostream &os);
  // DD the following function is used to display signals each cycle...
  virtual void display_my_signal(ostream &os);
  int connected;                              ///< How many time this port has been connected
  bool forward;                               ///< Wether or not this port is fowarder (aliased)
  string name;                                ///< Name of the port
  unisim_module *parent_module;               ///< Module owning this port
public:
  unisim_port *connected_port;                ///< Port connected to this port

  bool latex_rendering_fused;
};


/**
 * \brief Port Sensitivity list
 */
class Unisim_Sensitive
{public:
  /**
   * \brief Creates an empty sensitity list
   */
  Unisim_Sensitive(fsc_sensitive &sensitive) : sensitive(sensitive)
  {
  }

  /**
   * \brief Add a new signal to the sensitivity list
   */
  Unisim_Sensitive & operator<< (fsc_prim_port& port)
  { sensitive << port;
    return *this;
  }
  
  /**
   * \brief Add a desactivated signal to the sensitivity list, effectivly doing nothing
   */
  template <class T>
  Unisim_Sensitive & operator<< (const unisim_desactivated_signal<T> & sig)
  { return *this;
  }

 protected:
  fsc_sensitive &sensitive;  ///< The sensitivity list
};


/**
 * \brief Raising edge sensitivity list
 */
class Unisim_Sensitive_Pos
{public:
  /**
   * \brief Creates an empty sensitity list
   */
  Unisim_Sensitive_Pos (fsc_sensitive_pos &sensitive) : sensitive_pos(sensitive)
  {
  }

  /**
   * \brief Add a new signal to the 'start of cycle' sensitivity list
   */
  Unisim_Sensitive_Pos & operator<< (fsc_prim_port& port)
  { sensitive_pos << port;
    return *this;
  }

 protected:
  fsc_sensitive_pos &sensitive_pos;  ///< The sensitivity list
};


/**
 * \brief Falling edge sensitivity list
 */
class Unisim_Sensitive_Neg
{public:
  /**
   * \brief Creates an empty sensitity list
   */
  Unisim_Sensitive_Neg(fsc_sensitive_neg &sensitive) : sensitive_neg(sensitive)
  {
  }

  /**
   * \brief Add a new signal to the 'end of cycle' sensitivity list
   */
  Unisim_Sensitive_Neg & operator<< (fsc_prim_port& port)
  { sensitive_neg << port;
    return *this;
  }

 protected:
  fsc_sensitive_neg &sensitive_neg;  ///< The sensitivity list
};

class module;
class Unisim_Sensitive_Array
{public:
  static module *__current_array_module;
  static int __current_array_size;
  template<class T, bool B> Unisim_Sensitive_Array& operator<< (inport<T,B> p[]);
  template<class T, bool B> Unisim_Sensitive_Array& operator<< (outport<T,B> p[]);
};


/**
 * \brief UNISIM leaf module class
 */
class unisim_module: public fsc_module
{public:
  static unisim_module *unisim_current_module;    ///< currently instantiated module

  unisim_module(const string & name);             // create a new module
  unisim_module(const char *name);                // create a new module
  virtual ~unisim_module()                        // class destructor
  {
  }

  Unisim_Sensitive unisim_sensitive;              ///< sensitivity list of the module
  Unisim_Sensitive_Pos unisim_sensitive_pos;      ///< Raising clock sensitivity list of the module
  Unisim_Sensitive_Neg unisim_sensitive_neg;      ///< Falling clock sensitivity list of the module
  Unisim_Sensitive_Array unisim_sensitive_array;  ///< Wrapper for sensitivity lists of array ports (jonathan)

  //veerle dse functions
  virtual int check_parameters() { return 0; }
  virtual int explore_parameters(FILE * fp) { return 0; }
  virtual void dump_parameters(FILE * fp) {}
  virtual void random_next_parameters(FILE * fp) {}
  virtual void focused_next_parameters() {}
  //eoveerle dse functions
  
  void dump_all_parameters() const
  { parameters.dump(name());
  }

  virtual ModuleParameter get_parameter(const string &pname)
  { return parameters.get_parameter(pname);
  }

//  static void unisim_check_connections();         // Check that all module ports are connected
  friend ostream& operator<< (ostream& os, const unisim_module &mod); // pretty printer

  /**
   * \brief add a new port to the input port list
   */
  //#ifdef USE_UNISIM_SIGNAL_ARRAY
  //  void register_port(Unisim_Inport_Base<NCONFIG> *p)
  //#else
  //  void register_inport(Unisim_Inport_Base *p)
  void register_inport(unisim_port *p)
    //#endif
  { module_inport_list.push_back(p);
  }

  /**
   * \brief add a new port to the output port list
   */
  //  void register_outport(Unisim_Outport_Base *p)
  void register_outport(unisim_port *p)
  { module_outport_list.push_back(p);
  }

  /**
   * Hook called upon a ctrl-z to display debug info
   */
  virtual void ctrlz_hook()
  {
  }

  virtual void show_progress();                // Hook called when displaying progress bar
  virtual void set_progress_max(time_t start, uint64_t max); // Hook for setting the maximum for the progress bar
  void dump_machine_description(ostream &os);  // Dump a machine description from module parameters
  void dump_dot_nodes(ostream &os);
  void dump_dot_conns(ostream &os);
  void compute_latex_ports();
  virtual void dump_latex_module(ostream &os, bool first);
  virtual void dump_latex_ports(ostream &os);
  virtual void dump_latex_conns(ostream &os);
  virtual int get_rank();

  virtual bool has_pending_ops(){ return false;} //Taj - returns false if module has no pending operations
 public:
  static List<unisim_module> unisim_module_list;           ///< static global list of instantiated modules
  enum category_t {category_UNKNOWN, category_INTERCONNECT, category_CACHE, category_MEMORY, category_PROCESSOR};
  category_t category;
 protected: 
  //  std::list < Unisim_Inport_Base  * > module_inport_list;  ///< list of input ports in the module
  //  std::list < Unisim_Outport_Base * > module_outport_list; ///< list of outports defined in the module
  std::list < unisim_port * > module_inport_list;  ///< list of input ports in the module
  std::list < unisim_port * > module_outport_list; ///< list of outports defined in the module
  ModuleParameters parameters;
  list<unisim_port*> latex_left_ports;   ///< Ports to be put on the left of the module for latex rendering
  list<unisim_port*> latex_right_ports;  ///< Ports to be put on the right of the module for latex rendering
  list<unisim_port*> latex_top_ports;    ///< Ports to be put on the top of the module for latex rendering
  list<unisim_port*> latex_bottom_ports; ///< Ports to be put on the bottom of the module for latex rendering
};



/*
class Signal_Status;

template <class T> 
class Unisim_Prim_Out : public fsc_prim_out<T> {
 public:
  Unisim_Prim_Out(const char *n) : fsc_prim_out<T>(n) { }

  Unisim_Prim_Out& operator=(Signal_Status &s)
  { throw std::runtime_error("Cannot assign Signal_Status to non-boolean port");
  }

  Unisim_Prim_Out& operator=(const T& data)
  { (*(fsc_prim_out<T>*)this)=data;
  // DD DEBUG SIGNALS
  //  cerr << "[signal: " << fsc_object::name() << "]" << "[data: "<<data<<"]\t\t\t("<<timestamp()<<")"<<endl; 
    return *this;
  }
};
*/

/**
 * \brief Data-less input port. Input ports inherits from this class 
 */

template <class U, uint32_t NCONFIG>
class unisim_prim_in: public fsc_prim_in< boost::array<U,NCONFIG> >
{
 public:
  unisim_prim_in(const char *name=0) : fsc_prim_in< boost::array<U,NCONFIG> >(name) {}

  const U & operator[](int i) 
    { /*
      return ( 
	      ( (boost::array<U,NCONFIG>) 
		( (fsc_prim_in< boost::array<U,NCONFIG> >) (*this) ) ).operator[](i) ); 
      */
      fsc_prim_in< boost::array<U,NCONFIG> > &tmp = *this;
      const boost::array<U,NCONFIG> &tmp2 = tmp;
      //      cerr << "WARNING :  tmp2.size()= "  << tmp2.size() << endl;
      return tmp2[i];
    }
};

/*
template <class U>
class dummy
{
 public:
  dummy(U &t): tempU(t) {}
  U &tempU;
  void operator=(const U& t)
    {

    }
};
*/   

template <class U, uint32_t NCONFIG>
class unisim_prim_out: public fsc_prim_out< boost::array<U,NCONFIG> >
{
 public:
  unisim_prim_out(const char *name=0) : fsc_prim_out< boost::array<U,NCONFIG> >(name) 
    {
#ifdef USE_AUTOMATIC_SEND
	for (int i=0; i<NCONFIG; i++)
	{
	  written_value[i] = false;
	} 
      written_count = 0;
#endif
    }

    friend ostream & operator<<(ostream &os, unisim_prim_out &upo)
    {
      for (int i=0; i<NCONFIG; i++)
	os << upo.temporary_array[i] << " ";
      return os;
    }

#ifdef USE_AUTOMATIC_SEND
  friend class dummy;
  //  template <class V, uint32_t NCONF>
  class dummy
    {
      friend class unisim_prim_out<U, NCONFIG>; 
    private:
      dummy(unisim_prim_out<U, NCONFIG> upo, int i): uni_out(upo), index(i) 
	{ 
	  cerr << "Debug info : into dummy constructor ..." << endl;
	}
      unisim_prim_out<U, NCONFIG> &uni_out;
      int &index;
    public:
      void operator=(U& t)
	{
	  if (!uni_out.written_value[index])
	    {
	      uni_out.temporary[index] = t;
	      written_value[index] = true;
	      written_count++;
	    }
	  else
	    {
	      cerr << "Error array value already assigned !!!" << endl;
	    }
	  if (written_count>= NCONFIG)
	    {
	      uni_out.send();
	      for (int i=0; i<NCONFIG; i++)
		{
		  uni_out.written_value[i] = false;
		}
	      uni_out.written_count = 0;
	    }
	}
    };
  dummy operator[](int i)
    {   //return ( 
	//      ( (boost::array<U,NCONFIG>) 
	//	( (fsc_prim_out< boost::array<U,NCONFIG> >) (*this) ) ).operator[](i) );
      cerr << "Debug info : into dummy.operator[] ..." << endl;
      return dummy(*this,i);
    }
#else
  U &operator[](int i) { return temporary_array[i]; }
#endif

  void send()
    {
      fsc_prim_out< boost::array<U,NCONFIG> >::operator=(temporary_array);
      /* Work with data but do not work with accept and enable...
      for(int cfg=0; cfg<NCONFIG; cfg++)
	{
	  temporary_array[cfg].nothing();
	}
      */
    }
  //#endif

 protected:
  boost::array<U,NCONFIG> temporary_array;
#ifdef USE_AUTOMATIC_SEND
  bool written_value[NCONFIG];
  int written_count;
#endif
};

template < uint32_t NCONFIG>
class Unisim_Inport_Base : public unisim_port
{public:
  //  fsc_prim_in < boost::array<bool,NCONFIG> > enable;        ///< Enable signal
  unisim_prim_in < bool, NCONFIG > enable;        ///< Enable signal
  //  fsc_prim_out < boost::array<bool,NCONFIG> > accept;    ///< Accept signal
  unisim_prim_out < bool, NCONFIG > accept;    ///< Accept signal
  //  Unisim_Inport_Base();                 // Constructor
  Unisim_Inport_Base(): unisim_port(), enable("enable"), accept("accept")
    { unisim_module::unisim_current_module->register_inport(this);
    }
  
  virtual ~Unisim_Inport_Base() { }     // Destructor
  /**
   * \brief Returns true if the data is known
   */
  virtual bool is_data_known() = 0;
  /**
   * \brief Returns true if the data is nothing
   */
  virtual bool is_data_nothing() = 0;
  /**
   * \brief Returns true if the data is known and somethng
   */
  bool is_data_something() { return is_data_known() && !is_data_nothing(); }
};

template <>
class Unisim_Inport_Base<1> : public unisim_port
{public:
  fsc_prim_in < bool > enable;        ///< Enable signal
  fsc_prim_out < bool > accept;    ///< Accept signal

  //  Unisim_Inport_Base();                 // Constructor
  Unisim_Inport_Base(): unisim_port(), enable("enable"), accept("accept")
    { unisim_module::unisim_current_module->register_inport(this);
    }
  virtual ~Unisim_Inport_Base() { }     // Destructor
  /**
   * \brief Returns true if the data is known
   */
  virtual bool is_data_known() = 0;
  /**
   * \brief Returns true if the data is nothing
   */
  virtual bool is_data_nothing() = 0;
  /**
   * \brief Returns true if the data is known and somethng
   */
  bool is_data_something() { return is_data_known() && !is_data_nothing(); }
};

/**
 * \brief Data-less output port. Output ports inherits from this class 
 */

template < uint32_t NCONFIG>
class Unisim_Outport_Base : public unisim_port
{public:
  //  fsc_prim_out < boost::array<bool,NCONFIG> > enable;     ///< Enable signal
  unisim_prim_out < bool, NCONFIG > enable;        ///< Enable signal
  //  fsc_prim_in < boost::array<bool,NCONFIG> > accept;         ///< Accept signal
  unisim_prim_in < bool, NCONFIG > accept;    ///< Accept signal

  //  Unisim_Outport_Base();                 // Constructor
  Unisim_Outport_Base() : unisim_port(), enable("enable"), accept("accept")
    { unisim_module::unisim_current_module->register_outport(this);
    }
  virtual ~Unisim_Outport_Base() { }     // Destructor
  /**
   * \brief Returns true if the data is known
   */
  virtual bool is_data_known() = 0;
  /**
   * \brief Returns true if the data is nothing
   */
  virtual bool is_data_nothing() = 0;
  /**
   * \brief Returns true if the data is known and somethng
   */
  bool is_data_something() { return is_data_known() && !is_data_nothing(); }
};

template <>
class Unisim_Outport_Base<1> : public unisim_port
{public:
  fsc_prim_out < bool > enable;     ///< Enable signal
  fsc_prim_in < bool > accept;         ///< Accept signal

  //  Unisim_Outport_Base();                 // Constructor
  Unisim_Outport_Base() : unisim_port(), enable("enable"), accept("accept")
    { unisim_module::unisim_current_module->register_outport(this);
    }

  virtual ~Unisim_Outport_Base() { }     // Destructor
  /**
   * \brief Returns true if the data is known
   */
  virtual bool is_data_known() = 0;
  /**
   * \brief Returns true if the data is nothing
   */
  virtual bool is_data_nothing() = 0;
  /**
   * \brief Returns true if the data is known and somethng
   */
  bool is_data_something() { return is_data_known() && !is_data_nothing(); }
};

template <typename T>
class SuperData
{
 public:
  SuperData(): someth(false) {}

  operator const T& () const { return data; }
  //  operator T& () { return data; }

  SuperData<T>& operator=(const T& t) { data=t; someth=true; return *this; }

  const T &Data() const { return data; }
  void nothing() { someth = false; }
  bool something() const { return someth; }
 protected:
  T data;
  bool someth;
};


/**
 * \brief UNISIM cycle-level input port class
 */
//#ifdef USE_UNISIM_SIGNAL_ARRAY
template < class T, uint32_t NCONFIG> 
class inport <T, NCONFIG, true> : public Unisim_Inport_Base<NCONFIG> 
{public:
  //  typedef boost::array< SuperData<T>, NCONFIG> unisim_type_array_t;

  //typedef SuperData<T> U;
  //  fsc_prim_in < boost::array<T,NCONFIG> > data;           ///< Data signal
  unisim_prim_in < SuperData<T> , NCONFIG > data;           ///< Data signal
  //  unisim_prim_in < U , NCONFIG > data;           ///< Data signal
  inport<T, NCONFIG, true> *forwarded_port;   ///< Pointer to the aliased port, if aliased

  //  bool something[NCONFIG];
  /**
   * \brief Creates a new input port
   */ 
  inport() : Unisim_Inport_Base<NCONFIG>(), data("data")
  { forwarded_port=NULL;
    this->unisim_inport_list << this;
  }

  /**
   * \brief Associate its name to a port
   */
  void set_unisim_name(unisim_module *mod, const string &_name, int i=-1)
  { stringstream ss;
    ss << _name;
    if(i!=-1)
    { ss << "[" << i << "]";
    }
    Unisim_Inport_Base<NCONFIG>::name = ss.str();
    Unisim_Inport_Base<NCONFIG>::parent_module = mod;
    //Subsignal naming
    stringstream data_name;
    stringstream enable_name;
    stringstream accept_name;
    data_name << mod->name() << "___"  << Unisim_Inport_Base<NCONFIG>::name << ".data";
    enable_name << mod->name() << "___"  << Unisim_Inport_Base<NCONFIG>::name << ".enable";
    accept_name << mod->name() << "___"  << Unisim_Inport_Base<NCONFIG>::name << ".accept";
    data.SetName(data_name.str());
    Unisim_Inport_Base<NCONFIG>::accept.SetName(accept_name.str());
    Unisim_Inport_Base<NCONFIG>::enable.SetName(enable_name.str());
  }

  /**
   * \brief Connects this input port to another input port. Correspond to port aliasing.
   */
  void operator() (inport < T, NCONFIG, true > & ip)
  { Unisim_Inport_Base<NCONFIG>::forward = true;
    forwarded_port = &ip;
    cerr << "Port forwarding is now disabled !!!" << endl;
    abort();

  }

  /**
   * \brief Connects the port to a 3-signals object
   */
  //  void operator() (fsc_signal < T > &sig)
  void operator() (unisim_3_signals < boost::array< SuperData<T>, NCONFIG>, NCONFIG > &sig)
  { if (Unisim_Inport_Base<NCONFIG>::forward)
    { (*forwarded_port)(sig);
    } 
    else 
    { data(sig.data);
      enable(sig.enable);
      accept(sig.accept);
    }
    this->connected++;
  }

/*

Direct acces tot the data value should be replaced by port.data

  //assign
  operator const T & ()
  { return data;
  }

*/

  virtual bool is_data_known()
  { return data.known();
  }

  virtual bool is_data_nothing()
  { return data.nothing();
  }

  /**
   * \brief Check that the 3 signals are known
   */
  virtual bool check_my_knowness()
  { if(!this->data.was_known()) return false;
    if(!this->accept.was_known()) return false;
    if(!this->enable.was_known()) return false;
    return true;
  }
  
  /**
   * \brief DD Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_knowness(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
    { if(data.something()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
      { // TODO a for loop on accept[i]
	//if(this->accept) os << "\e[36mY\e[0m";
	//else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
      { // TODO a for loop on enable[i]
	//if(this->enable) os << "\e[36mY\e[0m";
	//else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { //if(data.something()) os << data;
      }
#endif   
#undef DD_MY_SIGNAL_DEBUGGER

    os << setw(15) << " " << "|";
    //if(data.was_known() && data.something()) os << data;

    os << endl;
  }
  
  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_signal(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
    { if(data.something()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
      { os << "[";
	for(int cfg=0; cfg<NCONFIG; cfg++)
	  if(this->accept[cfg]) os << "\e[36mY\e[0m";
	  else os << "\e[32mN\e[0m";
	os << "]";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
      { os << "[";
	for(int cfg=0; cfg<NCONFIG; cfg++)
	  if(this->enable[cfg]) os << "\e[36mY\e[0m";
	  else os << "\e[32mN\e[0m";
	os << "]";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
    //#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { 
	os << "[";
	for(int cfg=0; cfg<NCONFIG; cfg++)
	  if(data[cfg].something()) os << " [" << data[cfg] << "]";
	//	os
      }
    //#endif   
    //#undef DD_MY_SIGNAL_DEBUGGER

    os << setw(15) << " " << "|" << endl;
  }

#ifdef LIBERTY_SUPPORT
  /* This function is for LSE support */
  LSE_signal_t port_get(LSE_dynid_t *id, T **dat)
  { if (id)
    { *id = NULL;
    }
    LSE_signal_t s = 0;

    if (accept.known())
    { if ((bool)accept) s |= LSE_signal_ack;
      else              s |= LSE_signal_nack;
    }
    
    if (enable.known())
    { if ((bool)enable) s |= LSE_signal_enabled;
      else              s |= LSE_signal_disabled;
    }
    
    if (data.known())
    { if (!data.nothing())
      { s |= LSE_signal_something;
        if (dat)
        { // FIXME   
          *dat = data.get_pointer();
        }
      } 
      else 
      { s |= LSE_signal_nothing;
        if(dat) *dat = NULL;
      }
    }
    else
    { if (dat) *dat = NULL;
    }
    return s;
  }
  
  /* ...ditto */
  void port_set(LSE_signal_t s, LSE_dynid_t id, T* dat)
  { /* set status */
    if (LSE_signal_extract_ack(s) == LSE_signal_ack)
    { Accept();
    }
    else if (LSE_signal_extract_ack(s) == LSE_signal_nack)
    { Deny();
    } 
    else 
    { // NOTE: THIS CASE IS NOT HANDLED BECAUSE YOU SHOULD ***NEVER*** TRANSITION FROM KNOWN TO UNKNOWN
    }
  }
#endif
 protected:
  unisim_module *_module;   ///< Module this port belongs to
};

//#else
template < class T> 
class inport <T,1,true> : public Unisim_Inport_Base<1> 
{public:
  fsc_prim_in < T > data;           ///< Data signal
  inport<T,true> *forwarded_port;   ///< Pointer to the aliased port, if aliased

  /**
   * \brief Creates a new input port
   */ 
  inport() : Unisim_Inport_Base<1>(), data("data")
  { forwarded_port=NULL;
    this->unisim_inport_list << this;
  }

  /**
   * \brief Associate its name to a port
   */
  void set_unisim_name(unisim_module *mod, const string &_name, int i=-1)
  { stringstream ss;
    ss << _name;
    if(i!=-1)
    { ss << "[" << i << "]";
    }
    name = ss.str();
    parent_module = mod;
    //Subsignal naming
    stringstream data_name;
    stringstream enable_name;
    stringstream accept_name;
    data_name << mod->name() << "___"  << name << ".data";
    enable_name << mod->name() << "___"  << name << ".enable";
    accept_name << mod->name() << "___"  << name << ".accept";
    data.SetName(data_name.str());
    accept.SetName(accept_name.str());
    enable.SetName(enable_name.str());
  }

  /**
   * \brief Connects this input port to another input port. Correspond to port aliasing.
   */
  void operator() (inport < T, true > & ip)
  { forward = true;
    forwarded_port = &ip;
    cerr << "Port forwarding is now disabled !!!" << endl;
    abort();
  }

  /**
   * \brief Connects the port to a 3-signals object
   */
  void operator() (unisim_3_signals < T, 1 > &sig)
  { if (forward)
    { (*forwarded_port)(sig);
    } 
    else 
    { data(sig.data);
      enable(sig.enable);
      accept(sig.accept);
    }
    this->connected++;
  }


/*

Direct acces tot the data value should be replaced by port.data

  //assign
  operator const T & ()
  { return data;
  }

*/

  virtual bool is_data_known()
  { return data.known();
  }

  virtual bool is_data_nothing()
  { return data.nothing();
  }

  /**
   * \brief Check that the 3 signals are known
   */
  virtual bool check_my_knowness()
  { if(!this->data.was_known()) return false;
    if(!this->accept.was_known()) return false;
    if(!this->enable.was_known()) return false;
    return true;
  }
  
  /**
   * \brief DD Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_knowness(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
    { if(data.something()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
    { if(this->accept) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
    { if(this->enable) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { if(data.something()) os << data;
      }
#endif   
#undef DD_MY_SIGNAL_DEBUGGER

    os << setw(15) << " " << "|";
    if(data.was_known() && data.something()) os << " ";//data;

    os << endl;
  }
  
  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_signal(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
    { if(data.something()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
    { if(this->accept) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
    { if(this->enable) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
    //#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { if(data.something()) os << " ";// << data;
      }
    //#endif   
    //#undef DD_MY_SIGNAL_DEBUGGER

    os << setw(15) << " " << "|" << endl;
  }

#ifdef LIBERTY_SUPPORT
  /* This function is for LSE support */
  LSE_signal_t port_get(LSE_dynid_t *id, T **dat)
  { if (id)
    { *id = NULL;
    }
    LSE_signal_t s = 0;

    if (accept.known())
    { if ((bool)accept) s |= LSE_signal_ack;
      else              s |= LSE_signal_nack;
    }
    
    if (enable.known())
    { if ((bool)enable) s |= LSE_signal_enabled;
      else              s |= LSE_signal_disabled;
    }
    
    if (data.known())
    { if (!data.nothing())
      { s |= LSE_signal_something;
        if (dat)
        { // FIXME   
          *dat = data.get_pointer();
        }
      } 
      else 
      { s |= LSE_signal_nothing;
        if(dat) *dat = NULL;
      }
    }
    else
    { if (dat) *dat = NULL;
    }
    return s;
  }
  
  /* ...ditto */
  void port_set(LSE_signal_t s, LSE_dynid_t id, T* dat)
  { /* set status */
    if (LSE_signal_extract_ack(s) == LSE_signal_ack)
    { Accept();
    }
    else if (LSE_signal_extract_ack(s) == LSE_signal_nack)
    { Deny();
    } 
    else 
    { // NOTE: THIS CASE IS NOT HANDLED BECAUSE YOU SHOULD ***NEVER*** TRANSITION FROM KNOWN TO UNKNOWN
    }
  }
#endif
 protected:
  unisim_module *_module;   ///< Module this port belongs to
};

//#endif // end of ifdef USE_UNISIM_SIGNAL_ARRAY

/**
 * \brief UNISIM cycle-level output port class
 */
//#ifdef USE_UNISIM_SIGNAL_ARRAY
template < class T, uint32_t NCONFIG> 
class outport <T, NCONFIG, true> : public Unisim_Outport_Base<NCONFIG>
{ public:
  //  typedef boost::array<SuperData<T>,NCONFIG> unisim_type_array_t;
  //  Unisim_Prim_Out < T > data;        ///< Data signal
  //  fsc_prim_out <  boost::array<T,NCONFIG> > data;        ///< Data signal
  unisim_prim_out < SuperData<T>, NCONFIG > data;           ///< Data signal
  outport< T, NCONFIG , true> *forwarded_port;  ///< Pointer to the forwarded port (output to output connections)
  //  fsc_signal< boost::array<T,NCONFIG> > signal; ///< The 3-signals object connecting this output port to an input port
  unisim_3_signals< boost::array<SuperData<T>,NCONFIG>, NCONFIG > signal; ///< The 3-signals object connecting this output port to an input port

  outport() : Unisim_Outport_Base<NCONFIG>(), data("data")
  { forwarded_port = NULL;
    this->unisim_outport_list << this;
  }

  /**
   * \brief Associate its name to a port
   */
  void set_unisim_name(unisim_module *mod, const string &_name, int i=-1)
  { stringstream ss;
    ss << _name;
    if(i!=-1)
    { ss << "[" << i << "]";
    }
    Unisim_Outport_Base<NCONFIG>::name = ss.str();
    Unisim_Outport_Base<NCONFIG>::parent_module = mod;
    //Subsignal naming
    stringstream data_name;
    stringstream enable_name;
    stringstream accept_name;
    data_name  << mod->name() << "___"   << Unisim_Outport_Base<NCONFIG>::name << ".data";
    enable_name << mod->name() << "___"  << Unisim_Outport_Base<NCONFIG>::name << ".enable";
    accept_name << mod->name() << "___"  << Unisim_Outport_Base<NCONFIG>::name << ".accept";
    data.SetName(data_name.str());
    Unisim_Outport_Base<NCONFIG>::accept.SetName(accept_name.str());
    Unisim_Outport_Base<NCONFIG>::enable.SetName(enable_name.str());
  }

  /**
   * \brief Connects the output port to an input port
   */
  void operator() (inport < T, NCONFIG, true > & ip)
    { //(*this)(signal);
      this->operator()(signal);
    ip(signal);
    Unisim_Outport_Base<NCONFIG>::connected_port = &ip;
    ip.connected_port = this;
  }

  /**
   * \brief Connect the output port to another output port, aliasing both ports.
   */
  void operator() (outport < T, NCONFIG,true > & op)
  { Unisim_Outport_Base<NCONFIG>::forward=true;
    forwarded_port = &op;
    cerr << "Port forwarding is now disabled !!!" << endl;
    abort();
  }

  /**
   * \brief Connects the port to a 3-signals object
   */

  //  void operator() (fsc_signal < T > &sig)
  void operator() (unisim_3_signals < boost::array<SuperData<T>,NCONFIG>, NCONFIG > &sig)
  { if (Unisim_Outport_Base<NCONFIG>::forward) 
    { forwarded_port->operator()(sig);
    } else 
    { data(sig.data);
      enable(sig.enable);
      accept(sig.accept);
    }
    this->connected++;
  }

/*

direct setting of the output data assigniing directly to the port. 
removed, should be replaced by port.data = ... instead of port = ...

  outport < T,true > &operator = (const T & in)
  { data = in;
    return *this;
  }
*/  
  
  virtual bool is_data_known()
  { return data.known();
  }

  virtual bool is_data_nothing()
  { return data.is_nothing();
  }

  /**
   * \brief Check that the 3 signals are known
   */
  virtual bool check_my_knowness()
  { if(!this->data.was_known()) return false;
    if(!this->accept.was_known()) return false;
    if(!this->enable.was_known()) return false;
    return true;
  }

  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_knowness(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
    { if(!data.is_nothing()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
      { //if(this->accept) os << "\e[36mY\e[0m";
	//else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
      { //if(this->enable) os << "\e[36mY\e[0m";
	//else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { //if(!data.is_nothing()) os << data;
      }
#endif   
#undef DD_MY_SIGNAL_DEBUGGER
    os << setw(15) << " " << "|" << endl;
  }

  // DD used to debug signals...
  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_signal(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
      //    { if(!data.is_nothing()) os << "\e[36mS\e[0m";
    { if(!data.is_nothing()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
      { //if(this->accept) os << "\e[36mY\e[0m";
	//else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
      { //if(this->enable) os << "\e[36mY\e[0m";
	//else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
    //#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { //if(!data.is_nothing()) os << " " << data;
      }
    //#endif   
    //#undef DD_MY_SIGNAL_DEBUGGER
    os << setw(15) << " " << "|" << endl;
  }

#ifdef LIBERTY_SUPPORT
  /* This function is for LSE support */
  LSE_signal_t port_get(LSE_dynid_t *id, T **dat) 
  { if(id) *id = NULL;
    
    LSE_signal_t s = 0;

    if (accept.known()) 
    { if ((bool)accept) s |= LSE_signal_ack;
      else              s |= LSE_signal_nack;
    }
    
    if (enable.known()) 
    { if ((bool)enable) s |= LSE_signal_enabled;
      else              s |= LSE_signal_disabled;
    }
    
    if (data.known()) 
    { if (!data.is_nothing()) 
      { s |= LSE_signal_something;
        if(dat) *dat = NULL;
      } 
      else 
      { s |= LSE_signal_nothing;
        if(dat) *dat = NULL;
      }
    } 
    else 
    { if(dat) *dat = NULL;
    }
    return s;
  }


  /* ...ditto */
  void port_set(LSE_signal_t s, LSE_dynid_t id, T* dat) 
  { if (LSE_signal_extract_enable(s) == LSE_signal_enabled) 
    { Enable();
    } 
    else if (LSE_signal_extract_enable(s) == LSE_signal_disabled) 
    { Disable();
    } 
    else 
    { // NOTE: THIS CASE IS NOT HANDLED BECAUSE YOU SHOULD ***NEVER*** TRANSITION FROM KNOWN TO UNKNOWN      
    }

    if (LSE_signal_extract_data(s) == LSE_signal_something) 
    { data = *dat;
    } 
    else if (LSE_signal_extract_data(s) == LSE_signal_nothing) 
    { nothing();
    } 
    else 
    { // NOTE: THIS CASE IS NOT HANDLED BECAUSE YOU SHOULD ***NEVER*** TRANSITION FROM KNOWN TO UNKNOWN
    }
  }
#endif
 protected:
  unisim_module *_module;  ///< The module the port belongs to
};

//#else
template < class T > 
class outport <T, 1, true> : public Unisim_Outport_Base<1>
{ public:
  //  Unisim_Prim_Out < T > data;        ///< Data signal
  fsc_prim_out < T > data;        ///< Data signal
  outport<T, true> *forwarded_port;  ///< Pointer to the forwarded port (output to output connections)
  unisim_3_signals<T, 1> signal;              ///< The 3-signals object connecting this output port to an input port

  outport() : Unisim_Outport_Base<1>(), data("data"), signal()
  { forwarded_port = NULL;
    this->unisim_outport_list << this;
  }

  /**
   * \brief Associate its name to a port
   */
  void set_unisim_name(unisim_module *mod, const string &_name, int i=-1)
  { stringstream ss;
    ss << _name;
    if(i!=-1)
    { ss << "[" << i << "]";
    }
    name = ss.str();
    parent_module = mod;
    //Subsignal naming
    stringstream data_name;
    stringstream enable_name;
    stringstream accept_name;
    data_name  << mod->name() << "___"   << name << ".data";
    enable_name << mod->name() << "___"  << name << ".enable";
    accept_name << mod->name() << "___"  << name << ".accept";
    data.SetName(data_name.str());
    accept.SetName(accept_name.str());
    enable.SetName(enable_name.str());
  }

  /**
   * \brief Connects the output port to an input port
   */
  void operator() (inport < T,true > & ip)
    { 
      //      cerr << "Connecting outport to a input port..." << endl;
      //(*this)(signal);
      //      cerr << "Before call to this->operator()..." << endl;
      this->operator()(signal);
      //      cerr << "After call to this->operator()..." << endl;
      ip(signal);
      //      cerr << "After call to ip(signal)..." << endl;

    connected_port = &ip;
    ip.connected_port = this;
  }

  /**
   * \brief Connect the output port to another output port, aliasing both ports.
   */
  void operator() (outport < T,true > & op)
  { forward=true;
    forwarded_port = &op;
    cerr << "Port forwarding is now disabled !!!" << endl;
    abort();
  }

  /**
   * \brief Connects the port to a 3-signals object
   */
  void operator() (unisim_3_signals < T, 1 > &sig)
  { if (forward)
    { (*forwarded_port)(sig);
    } else 
    { data(sig.data);
      enable(sig.enable);
      accept(sig.accept);
    }
    this->connected++;
  }

  //#endif

/*

direct setting of the output data assigniing directly to the port. 
removed, should be replaced by port.data = ... instead of port = ...

  outport < T,true > &operator = (const T & in)
  { data = in;
    return *this;
  }
*/  
  
  virtual bool is_data_known()
  { return data.known();
  }

  virtual bool is_data_nothing()
  { return data.is_nothing();
  }

  /**
   * \brief Check that the 3 signals are known
   */
  virtual bool check_my_knowness()
  { if(!this->data.was_known()) return false;
    if(!this->accept.was_known()) return false;
    if(!this->enable.was_known()) return false;
    return true;
  }

  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_knowness(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
    { if(!data.is_nothing()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
    { if(this->accept) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
    { if(this->enable) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { if(!data.is_nothing()) os << data;
      }
#endif   
#undef DD_MY_SIGNAL_DEBUGGER
    os << setw(15) << " " << "|" << endl;
  }

  // DD used to debug signals...
  /**
   * \brief Dump the value of each signal (called when some signal are unkonwn)
   */
  void display_my_signal(ostream &os)
  { stringstream ss;
    string parent_module_name;
    if(this->parent_module) parent_module_name = this->parent_module->name();
    else                    parent_module_name = "unknown_module";
  
    ss << parent_module_name << "." << this->name << ":";
    os << "| " << left << setw(30) << ss.str() << "d";
    if(data.was_known())
      //    { if(!data.is_nothing()) os << "\e[36mS\e[0m";
    { if(!data.is_nothing()) os << "\e[36mS\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " a";
    if(this->accept.was_known())
    { if(this->accept) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    os << " e";
    if(this->enable.was_known())
    { if(this->enable) os << "\e[36mY\e[0m";
      else os << "\e[32mN\e[0m";
    }
    else os << "\e[1;31mU\e[0m";
    //#define DD_MY_SIGNAL_DEBUGGER
    //#ifdef DD_MY_SIGNAL_DEBUGGER
    if(data.was_known())
      { if(!data.is_nothing()) os << " ";// << data;
      }
    //#endif   
    //#undef DD_MY_SIGNAL_DEBUGGER
    os << setw(15) << " " << "|" << endl;
  }

#ifdef LIBERTY_SUPPORT
  /* This function is for LSE support */
  LSE_signal_t port_get(LSE_dynid_t *id, T **dat) 
  { if(id) *id = NULL;
    
    LSE_signal_t s = 0;

    if (accept.known()) 
    { if ((bool)accept) s |= LSE_signal_ack;
      else              s |= LSE_signal_nack;
    }
    
    if (enable.known()) 
    { if ((bool)enable) s |= LSE_signal_enabled;
      else              s |= LSE_signal_disabled;
    }
    
    if (data.known()) 
    { if (!data.is_nothing()) 
      { s |= LSE_signal_something;
        if(dat) *dat = NULL;
      } 
      else 
      { s |= LSE_signal_nothing;
        if(dat) *dat = NULL;
      }
    } 
    else 
    { if(dat) *dat = NULL;
    }
    return s;
  }


  /* ...ditto */
  void port_set(LSE_signal_t s, LSE_dynid_t id, T* dat) 
  { if (LSE_signal_extract_enable(s) == LSE_signal_enabled) 
    { Enable();
    } 
    else if (LSE_signal_extract_enable(s) == LSE_signal_disabled) 
    { Disable();
    } 
    else 
    { // NOTE: THIS CASE IS NOT HANDLED BECAUSE YOU SHOULD ***NEVER*** TRANSITION FROM KNOWN TO UNKNOWN      
    }

    if (LSE_signal_extract_data(s) == LSE_signal_something) 
    { data = *dat;
    } 
    else if (LSE_signal_extract_data(s) == LSE_signal_nothing) 
    { nothing();
    } 
    else 
    { // NOTE: THIS CASE IS NOT HANDLED BECAUSE YOU SHOULD ***NEVER*** TRANSITION FROM KNOWN TO UNKNOWN
    }
  }
#endif
 protected:
  unisim_module *_module;  ///< The module the port belongs to
};


/**
 * \brief UNISIM cycle-level desactivated Input port
 */ 
template < class T > 
class inport <T, false> : public unisim_port
{public:
  unisim_desactivated_signal<T> data;            ///< Mimics data signal
  unisim_desactivated_signal<bool> accept;       ///< Mimics accept signal
  unisim_desactivated_signal<bool> enable;       ///< Mimics enable signal
  /**
   * \brief Fakely associate its name to a port
   */
  void set_unisim_name(unisim_module *mod, const string &_name, int i=-1)
  {
  }
};

/**
 * \brief UNISIM cycle-level desactivated Output port
 */ 
template < class T >
class outport <T,false> : public unisim_port
{public:
  unisim_desactivated_signal<T> data;           ///< Mimics data signal
  unisim_desactivated_signal<bool> accept;      ///< Mimics accept signal
  unisim_desactivated_signal<bool> enable;      ///< Mimics accept signal
  /**
   * \brief Fakely associate its name to a port
   */
  void set_unisim_name(unisim_module *mod, const string &_name, int i=-1)
  {
  }
};

/*
STF: Class added by jonathan, I wonder what is it for...

template <class T> 
class param 
{protected:
  T val;
  bool valid;

 public:
  param(): valid(false) { }
  param(T &_val): valid(true), val(_val) { }
  param(T _val): valid(true), val(_val) { }

  T& operator =(const T& a) {
    valid = true;
    return val=a;
  }

  operator const T& () {
    if (!valid) 
      throw std::runtime_error(std::string("Use of uninitialized parameter"));
    return val;
  }
};

*/

/**
 * \brief Connects an output port to an input port, checking that connected data type match
 */
template <class T, class U>
void operator >> (outport<T,true> &a, inport<U,true> &b) {
#ifdef RUNTIME_TYPE_CHECKS
  if (typeid(T) != typeid(U)) {
    throw std::runtime_error(std::string("Port types do not match: ") + typeid(T).name() + " and " +typeid(U).name());
  }
#endif
  //  cerr << "Debug info 001: " << endl;
  //  cerr << "a: " << a << endl; 
  //  cerr << "b: " << b << endl; 
  a(b);
}

/**
 * \brief Connects two input ports together (aliasing) checking that connected data type match
 */
template <class T, class U>
void operator >> (inport<T,true> &a, inport<U,true> &b) {
#ifdef RUNTIME_TYPE_CHECKS
  if (typeid(T) != typeid(U)) {
    throw std::runtime_error(std::string("Port types do not match: ") + typeid(T).name() + " and " +typeid(U).name());
  }
#endif
  a(b);
}

/**
 * \brief Connects two output ports together (aliasing) checking that connected data type match
 */
template <class T, class U>
void operator >> (outport<T,true> &a, outport<U,true> &b) {
#ifdef RUNTIME_TYPE_CHECKS
  if (typeid(T) != typeid(U)) {
    throw std::runtime_error(std::string("Port types do not match: ") + typeid(T).name() + " and " +typeid(U).name());
  }
#endif
  a(b);
}

/**
 * \brief Try to connect two desactivated ports, so perform an error
 */
template <class T, class U>
void operator >> (outport<T,false> &a, inport<U,false> &b)
{ cerr << "error: Port '" << a.get_name() << "' of instance '" << a.get_module_instance_name() << "' of module '" << a.get_module_class_name() << "' is connected whereas it is desactivated !" << endl;
  exit(1);
}

/**
 * \brief Try to connect two desactivated ports, so perform an error
 */
template <class T, class U>
void operator >> (inport<T,false> &a, inport<U,false> &b)
{ cerr << "error: Port '" << a.get_name() << "' of instance '" << a.get_module_instance_name() << "' of module '" << a.get_module_class_name() << "' is connected whereas it is desactivated !" << endl;
  exit(1);
}

/**
 * \brief Try to connect two desactivated ports, so perform an error
 */
template <class T, class U>
void operator >> (outport<T,false> &a, outport<U,false> &b)
{ cerr << "error: Port '" << a.get_name() << "' of instance '" << a.get_module_instance_name() << "' of module '" << a.get_module_class_name() << "' is connected whereas it is desactivated !" << endl;
  exit(1);
}

/**
 * \brief Connects an array output port to an array input port, checking that connected data type match
 */
template <class T, class U, uint32_t NCONFIG>
void operator >> (outport<T, NCONFIG,true> &a, inport<U, NCONFIG, true> &b) {
#ifdef RUNTIME_TYPE_CHECKS
  if (typeid(T) != typeid(U)) {
    throw std::runtime_error(std::string("Port types do not match: ") + typeid(T).name() + " and " +typeid(U).name());
  }
#endif
  a(b);
}



#define modulebody void module_body() 

#define sensitive_method_array(x,w) { Unisim_Sensitive_Array::__current_array_module = this; Unisim_Sensitive_Array::__current_array_size = w; } unisim_sensitive_array

template<class T, bool B>
Unisim_Sensitive_Array& Unisim_Sensitive_Array::operator<< (inport<T,B> p[]) {
  class hook_module : module {
      
  public:
    int index;
    module *real_module;
    void (module::*real_func)(int);
      
    void hook_func(void) {
      /* Dispatch away! */
      (real_module->*real_func)(index);
    }
      
    hook_module(char *n, inport<T,B> &pp, int _index) : module(n), index(_index), real_module(__current_array_module) { 
      sensitive_method(hook_func) << pp.data << pp.enable;
    }
  };
    
  for (int i=0;i<__current_array_size;i++) {
    new hook_module("",p[i],i);
  }    
  return *this;
}


template<class T, bool B>
Unisim_Sensitive_Array& Unisim_Sensitive_Array::operator<< (outport<T,B> p[]) {
  class hook_module : module {
      
  public:
    int index;
    module *real_module;
    void (module::*real_func)(int);
      
    void hook_func(void) {
      /* Dispatch away! */
      (real_module->*real_func)(index);
    }
      
    hook_module(char *n, outport<T,B> &pp, int _index) : module(n), index(_index), real_module(__current_array_module) { 
      sensitive_method(hook_func) << pp.accept;
    }
  };
    
  for (int i=0;i<__current_array_size;i++) {
    new hook_module("",p[i],i);
  }       
  return *this;
}


class Signal_Status 
{ bool value;

 public:
  bool known;

  Signal_Status() : value(false), known(false) { }
  Signal_Status(bool v) : value(v), known(true) { }

  Signal_Status& operator &=(Signal_Status &o)
  { known &= o.known;
    value &= o.value;
    return *this;
  }

  Signal_Status& operator |=(Signal_Status &o)
  { known |= o.known;
    value |= o.value;
    return *this;
  }

  Signal_Status& operator &=(fsc_prim_in<bool> &o)
  { known &= o.known();
    if (o.known()) value &= (bool&)o;
    return *this;
  }

  Signal_Status& operator |=(fsc_prim_in<bool> &o)
  { known |= o.known();
    if (o.known()) value |= (bool&)o;
    return *this;
  }

  Signal_Status& operator &=(fsc_prim_out<bool> &o)
  { known &= o.known();
    if (o.known()) value &= (bool&)o;
    return *this;
  }

  Signal_Status& operator |=(fsc_prim_out<bool> &o)
  { known |= o.known();
    if (o.known()) value |= (bool&)o;
    return *this;
  }

  /*
  Signal_Status& operator &=(Unisim_Inport_Base &o)
  { known &= o.is_data_known();
    if (o.is_data_known()) value &= o.is_data_something();
    return *this;
  }

  Signal_Status& operator |=(Unisim_Inport_Base &o)
  { known |= o.is_data_known();
    if (o.is_data_known()) value |= o.is_data_something();
    return *this;
  }

  Signal_Status& operator &=(Unisim_Outport_Base &o)
  { known &= o.is_data_known();
    if (o.is_data_known()) value &= o.is_data_something();
    return *this;
  }

  Signal_Status& operator |=(Unisim_Outport_Base &o)
  { known |= o.is_data_known();
    if (o.is_data_known()) value |= o.is_data_something();
    return *this;
  }
  */
  bool operator ==(bool v)
  { if (!known) return false;
    return (value == v);
  }

  bool operator !=(bool v)
  { return !(*this == v);
  }

  operator bool()
  { if (!known) throw std::runtime_error("Accessing unknown value");
    return value;
  }

  bool operator =(bool v)
  { known = true;
    return value = v;
  }

  Signal_Status& operator =(fsc_prim_in<bool> &p)
  { if (!p.known()) known = false;
    else
    { known = true;
      value = (bool&)p;
    }
    return *this;
  }

  //  friend class Unisim_Prim_Out<bool>;
  friend class fsc_prim_out<bool>;
};


#include <sstream>
class str : public std::string
{public:
  str(const char *s) : std::string(s)
  { 
  }

  str(const std::string &s) : std::string(s)
  {
  }
  
  str operator +(const char *s)
  { return ((std::string)*this)+s;
  }

  str operator +(const int x)
  { std::stringstream ss;
    ss << this;
    ss << x;
    return ss.str();
  }
};

/**
 * \brief Class the generated simulator is inherited from
 */
class Simulator
{public:
  fsc_clock global_clock;          ///< Global clock signal all clocks are connected to
 
  Simulator();                          // Class constructor
  ~Simulator();                         // Class destructor
  static uint64_t TimeStamp();          // Return the current timestamp (cycle number)
  ostream & stream_knowness();          // Return the ostream to display knowness debug info to

  bool _unknown_display;              ///< Should reminding unkown signals been displayed each cycle
  bool _unknown_fatal;                ///< Should the simulation stop on reminding unknown signals
  bool _signal_display;              ///< Should reminding signals been displayed each cycle
  static void at_ctrlc(int sig);        // Called when ctrl-c is pressed
  static void at_ctrlz(int sig);        // Called when ctrl-z is pressed
  void dump_machine_description(const string &filename);
  void dump_dot(const string &filename);
  void dump_latex(const string &filename);
  static bool has_pending_ops(); //Taj - overall state of simulator
  void set_progress_max(uint64_t max); ///< Set the maximum for the progress bar
  void show_progress();                ///< Display a progress bar with ETA
 protected:
  static fsc_simcontext *context;     ///< Simulation context
 private:
  ofstream _stream_knowness;          ///< The ostream to send the unkonwn signal debug info
  time_t start_time;                 ///< Simulation starting time
};

#endif                /* // __FRATERNITE_H__ */
