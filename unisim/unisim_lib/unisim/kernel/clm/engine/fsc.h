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
                       fsc.h  -  FraternitéSysC header
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#ifndef __FSC_H__
#define __FSC_H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <inttypes.h>

// DD
//#define FSC_STAT
//#define FFSC_STAT
//#define DEBUG_FSC

/***************************************
              LIBERTY CODE
****************************************/
#if LIBERTY_SUPPORT
typedef int LSE_signal_t;
typedef void * LSE_dynid_t;

#define LSE_signal_ack     03
#define LSE_signal_nack    01

#define LSE_signal_enabled     030
#define LSE_signal_disabled    010

#define LSE_signal_something     0300
#define LSE_signal_nothing       0100


#define LSE_signal_extract_data(x) (x&LSE_signal_something)
#define LSE_signal_extract_ack(x) (x&LSE_signal_ack)
#define LSE_signal_extract_enable(x) (x&LSE_signal_enabled)
#endif
/***************************************
              END LIBERTY
****************************************/


using namespace std;

#define SCHEDULER_FILENAME "scheduler.h"
#define SCHEDULE_FILENAME "schedule.txt"

#define FSC

#if defined(__GNUC__) && defined(EXTENSIVE_INLINING)
#ifndef GCC_INLINE
#define GCC_INLINE __attribute__((always_inline))
#endif
#define INLINE inline
#else
#ifndef GCC_INLINE
#define GCC_INLINE
#endif
#define INLINE inline
#endif

#if defined(__i386__)
#define RDTSC(p) \
__asm__ __volatile__( \
        ".byte 0x0f; .byte 0x31" \
        : "=A" (p))
#elif defined(PPC)
#define RDTSC(p) __asm__ __volatile__ (".byte 124, 76, 66, 230, 124, 109, 66, 230; ori %0, %%r2, 0; ori %1, %%r3, 0" : "=r" (*((uint32_t *) &p + 1)), "=r" (*(uint32_t *) &p) : : "r2", "r3")
#else
#define RDTSC(p) (p = 0)
#endif

#if defined(FSC_STAT)

#define CALL_INDIRECT_PROCESS(process) {\
  unsigned long long start, stop;\
  fsc_module *mod = (process)->mod;\
  fsc_method method = (process)->method;\
  RDTSC(start);\
  (mod->*method)();\
  RDTSC(stop);\
  (process)->time += stop - start;\
}

#define CALL_DIRECT_PROCESS(process, direct_call) {\
  unsigned long long start, stop;\
  RDTSC(start);\
  direct_call;\
  RDTSC(stop);\
  process->time += stop - start;\
}

#else
#define CALL_INDIRECT_PROCESS(process) {\
  fsc_module *mod = (process)->mod;\
  fsc_method method = (process)->method;\
  (mod->*method)();\
}

#define CALL_DIRECT_PROCESS(process, direct_call) {\
  direct_call;\
}
#endif

#include "unisim_list.h"
#include "unisim_graph.h"
#include "unisim_string.h"

class fsc_object;
class fsc_prim_port;
class fsc_signal_base;
class fsc_module;
class fsc_process;
class fsc_sensitive;
class fsc_sensitive_pos;
class fsc_sensitive_neg;
class fsc_simcontext;
class fsc_clock;
class fsc_trace_file;

template < class T > class fsc_prim_in;
template < class T > class fsc_prim_out;
template < class T > class fsc_prim_signal;

typedef fsc_module *fsc_module_ptr;
typedef fsc_prim_port *fsc_prim_port_ptr;
typedef fsc_signal_base *fsc_signal_ptr;
typedef fsc_process *fsc_process_ptr;
typedef fsc_clock *fsc_clock_ptr;

typedef void (fsc_module::*fsc_method)();

extern fsc_simcontext *GetSimContext();
extern int fsc_main(int argc, char *argv[]);
extern fsc_simcontext *simcontext;


/**
 * \brief Generic named object every object inherits from
 */
class fsc_object
{public:
  fsc_object(const char *name = 0);
  virtual ~fsc_object();
  void SetId(int id);
  int GetId();
  const char *name() const;
  const char *Name() const;
  void SetName(const char *format, ...);
  void SetName(const string& name);
 private:
  int id;
  char *object_name;
  friend class fsc_simcontext;
};

class fsc_trace_file
{public:
 private:
};

enum fsc_signal_type { FSC_SIGNAL, FSC_CLOCK };

/**
 * \brief Signal class primitive
 */
class fsc_signal_base : public fsc_object
{public:
  fsc_signal_base(fsc_signal_type type, const char *name = 0);
  virtual ~fsc_signal_base();

  void Bind(fsc_process *process);
  fsc_signal_type GetSignalType();
  bool HasChanged();
  void BindPos(fsc_process *process);
  void BindNeg(fsc_process *process);
  void BuildProcessTable();
 private:
  fsc_process *process;
  List<fsc_process> process_list;
  List<fsc_process> clk_pos_process_list;
  List<fsc_process> clk_neg_process_list;
  friend class fsc_simcontext;
  fsc_signal_type signal_type;
  fsc_process **sensitive_process_table;
  fsc_process **last_of_sensitive_processes;
  friend class fsc_prim_port;
#if !defined(GENSCHED)

  friend void fsc_phase();
  friend void fsc_start(uint64_t t);
  friend void fsc_start_alt(uint64_t t);
  friend void fsc_start_acyclic(uint64_t t);
  friend void fsc_stabilize();
#endif
protected:
  uint64_t time_stamp[2];
  bool something[2];
  int read_buf;
  int to_update;
  fsc_simcontext *simc;
private:
  static int next_id;
  static List<fsc_signal_base> signal_list;
};

enum fsc_prim_port_type { FSC_IN, FSC_OUT, FSC_IN_CLK };

/**
 * \brief Port class primitive
 */
class fsc_prim_port : public fsc_object
{public:
  fsc_prim_port(fsc_prim_port_type type, const char *name = 0);

  void Bind(fsc_signal_base *signal);
  void Bind(fsc_prim_port *port);
  void Bind(fsc_process *process);
  void BindPos(fsc_process *process);
  void BindNeg(fsc_process *process);
  void Dependency(fsc_prim_port *port);
  fsc_prim_port_type GetPortType();
 protected:
  fsc_signal_base *signal;
 private:
  fsc_prim_port *alias;
  List<fsc_process> process_list;
  List<fsc_process> clk_pos_process_list;
  List<fsc_process> clk_neg_process_list;
  fsc_module *mod;
  friend class fsc_simcontext;
  friend class fsc_module;
  fsc_prim_port_type port_type;
  List<fsc_prim_port> dependencies;
  static int next_id;
  static List<fsc_prim_port> port_list;
};

/**
 * \brief Clock input port
 */ 
class inclock : public fsc_prim_port
{public:
  inclock();
  void operator () (fsc_clock& clock);
  operator const bool& ();
  void operator () (inclock& port);
};

/**
 * \brief Clock signal
 */
class fsc_clock : public fsc_signal_base
{public:
  fsc_clock();
  void NextEdge();
  fsc_clock& operator = (bool value);
 private:
  bool value;
  friend class inclock;
  friend class fsc_simcontext;
  friend void fsc_phase();
  friend void fsc_start(uint64_t t);
  friend void fsc_start_alt(uint64_t t);
  friend void fsc_start_acyclic(uint64_t t);
  friend void fsc_stabilize();
};

/**
 * \brief Process sensitivity list
 */
class fsc_sensitive
{public:
  fsc_sensitive(fsc_module *mod);
  fsc_sensitive& operator << (fsc_prim_port& port);
 private:
  fsc_module *mod;
  friend class fsc_module;
  friend class fsc_simcontext;
};

/**
 * \brief Raising edge sensitivity list
 */
class fsc_sensitive_pos
{public:
  fsc_sensitive_pos(fsc_module *mod);
  fsc_sensitive_pos& operator << (fsc_prim_port& port);
 private:
  fsc_module *mod;
  friend class fsc_module;
  friend class fsc_simcontext;
};

enum fsc_clock_edge { POS_EDGE = 0, NEG_EDGE = 1};

/**
 * \brief Falling edge sensitivity list
 */
class fsc_sensitive_neg
{public:
  fsc_sensitive_neg(fsc_module *mod);
  fsc_sensitive_neg& operator << (fsc_prim_port& port);
 private:
  fsc_module *mod;
  friend class fsc_module;
  friend class fsc_simcontext;
};

/**
 * \brief Module class primitive
 */
class fsc_module : public fsc_object
{public:
  fsc_module(const string& name);
  fsc_module(const char *name);
  virtual ~fsc_module();

  void Add(fsc_prim_port *port);
  void DrawFig(ostream& os);
  void GenerateDocSkeleton(ostream& os);
  void declare_process(fsc_method method, const char *name);
  void set_process(fsc_method method, const char *name);

  string class_name;
 protected:
  fsc_sensitive sensitive;
  fsc_sensitive_pos sensitive_pos;
  fsc_sensitive_neg sensitive_neg;

  //	void declare_module_class(const char *name);
 private:
  fsc_process *current_process;
  List<fsc_prim_port> port_list;
  List<fsc_process> process_list;
  friend class fsc_sensitive;
  friend class fsc_sensitive_pos;
  friend class fsc_sensitive_neg;
  friend class fsc_simcontext;
  friend class fsc_process;

#if !defined(GENSCHED)
  friend void fsc_phase();
  friend void fsc_start(uint64_t t);
  friend void fsc_start_alt(uint64_t t);
  friend void fsc_start_acyclic(uint64_t t);
  friend void fsc_stabilize();
#endif

  static int next_id;
  static List<fsc_module> module_list;
};

/**
 * \brief Process of a module
 */
class fsc_process : public fsc_object
{public:
  fsc_process(fsc_module *mod, fsc_method method, const char *name);

  void Sensitive(fsc_prim_port *port);
  void SensitivePos(fsc_prim_port *port);
  void SensitiveNeg(fsc_prim_port *port);
  //	const char *Name();
  void Bind(fsc_signal_base *signal);
  void BindPos(fsc_signal_base *clk_signal);
  void BindNeg(fsc_signal_base *clk_signal);
  friend ostream& operator << (ostream& os, const fsc_process& process);
  bool IsSensitiveToClockEdge(fsc_clock_edge edge);

 private:
  List<fsc_prim_port> port_list;
  List<fsc_prim_port> output_port_list;
  List<fsc_prim_port> clk_pos_port_list;
  List<fsc_prim_port> clk_neg_port_list;
  fsc_module *mod;
  List<fsc_signal_base> signal_list;
  List<fsc_signal_base> clk_pos_signal_list;
  List<fsc_signal_base> clk_neg_signal_list;
  //	const char *name;
  fsc_method method;
  bool to_execute;
  unsigned long long time;

  friend class fsc_simcontext;
  friend void fsc_phase();
  friend void fsc_start(uint64_t t);
  friend void fsc_start_alt(uint64_t t);
  friend void fsc_start_acyclic(uint64_t t);
  friend void fsc_stabilize();

  static int next_id;
  static List<fsc_process> process_list;
};

/**
 * \brief Simulation context associated with the simulator
 */
class fsc_simcontext
{public:
  fsc_simcontext();
  ~fsc_simcontext();

  void Add(fsc_prim_port *port);
  void Add(fsc_module *mod);
  void Add(fsc_signal_base *signal);
  void Add(fsc_process *process);

  void Initialize();
  //	void Profile();
  void SubmitUpdate(fsc_signal_base *signal) GCC_INLINE;
  void Start();
  void PropagateSignals();
  void Dotify(ostream& os);
  void DotifyDependencyMatrix(ostream& os);
  //	void ComputeStaticSchedule();
  //	void ComputeStaticSchedule(List<fsc_process> *path, fsc_process *process);

  void DrawFig();
  void GenerateDocSkeleton();

  static bool debug;                                  ///< Wether debugging info should be displayed
  Graph<fsc_signal_base> *BuildDependencyGraph();
  List<List<fsc_process> > *RankProcesses();
  void BuildSchedule();
  /**
   * \brief Return the current cycle number
   */
  INLINE uint64_t TimeStamp() const
  { return time_stamp;
  }
 private:
  //	ListPointer<fsc_prim_port> last_identified_port;
 public:
  uint64_t time_stamp;                                ///< current cycle number
 private:
  bool initialized;                                   ///< True after initializing the simulator
  /*
  List<fsc_prim_port> port_list;
  List<fsc_module> module_list;
  List<fsc_signal_base> signal_list;
  List<fsc_process> process_list;
  */
  fsc_module **module_table;                          ///< All the defined modules
  fsc_signal_base **signal_table;                     ///< All the defined signals
  fsc_process **process_table;                        ///< All the defined processes
  int sizeof_signals_to_update_table;
  fsc_signal_base **signals_to_update;
  fsc_signal_base **signal_to_update;
  fsc_process **sensitive_clock_process_table[2];
  fsc_process **last_of_sensitive_clock_processes[2];
  fsc_clock **clock_signal_table;                     ///< All the defined clock signals
  fsc_clock **last_of_clock_signals;                  ///< Last defined clock signals
  fsc_module *current_module;                         ///< Last defined module
  fsc_process **processes_to_execute;

  fsc_process ***static_sch;

  int nprocesses;
  /*
  long long int *dependency_matrix;
  double *weight_matrix;
  long long int *count_vector;
  */
  fsc_clock_edge clock_edge;

  //	double weight_threshold;

  int *ncycles;

  bool stop;
  uint64_t debug_start;
  uint64_t debug_stop;

  long long int nwakeups;

  friend uint64_t timestamp();
  friend void fsc_stop();
  friend bool fsc_stopped();
  friend bool fsc_debugging();
#if !defined(GENSCHED)

  friend void fsc_phase();
  friend void fsc_start(uint64_t t);
  friend void fsc_start_alt(uint64_t t);
  friend void fsc_start_acyclic(uint64_t t);
  friend void fsc_stabilize();
  friend void fsc_debug(uint64_t, uint64_t);
#endif

  friend class fsc_signal_base;
};

/**
 * \brief Input port primitive
 */
template <class T>
class fsc_prim_in : public fsc_prim_port
{public:
  fsc_prim_in(const char *name = 0);

  void operator () (fsc_prim_signal<T>& signal);
  void operator () (fsc_prim_in<T>& port);

  operator const T& () GCC_INLINE;
  bool known() const GCC_INLINE;
  bool unknown() const GCC_INLINE;
  bool something() const GCC_INLINE;
  bool nothing() const GCC_INLINE;

  bool was_known() const GCC_INLINE;

  /* another liberty-ism */
  T* get_pointer();
};

template <class T>
class fsc_out : public fsc_prim_port
{public:
  fsc_out(const char *name = 0);

  void operator () (fsc_prim_signal<T>& signal);
  void operator () (fsc_prim_out<T>& port);
  void operator () (fsc_prim_port& port);
  operator const T& ();
  fsc_out<T>& operator = (const T& data) GCC_INLINE;
};

/**
 * \brief Output port primitive
 */
template <class T>
class fsc_prim_out : public fsc_prim_port
{public:
  fsc_prim_out(const char *name = 0);

  void operator () (fsc_prim_signal<T>& signal);
  void operator () (fsc_prim_out<T>& port);
  void operator () (fsc_prim_port& port);
  operator const T& ();
  fsc_prim_out<T>& operator = (const T& data) GCC_INLINE;
  void nothing() GCC_INLINE;
  bool is_nothing() const GCC_INLINE;
  bool known() const GCC_INLINE;
  bool unknown() const GCC_INLINE;

  bool was_known() const GCC_INLINE;


#ifdef LIBERTY_SUPPORT
  /* This is for liberty support */
  void port_set(LSE_signal_t s)
  {
    if (!strcmp(name(),"accept"))
    {
      if (LSE_signal_extract_ack(s) == LSE_signal_ack)
        *this = (bool)true;
      else if (LSE_signal_extract_ack(s) == LSE_signal_nack)
        *this = (bool)false;
    }
    else if (!strcmp(name(),"enable"))
    {
      if (LSE_signal_extract_enable(s) == LSE_signal_enabled)
        *this = (bool)true;
      else if (LSE_signal_extract_enable(s) == LSE_signal_disabled)
        *this = (bool)false;
    }
    else
    {
      cerr << "Cannot perform LSE_port_set on a port: " << name() << endl;
      exit(1);
    }
  }

  /* ditto */
  void port_set(LSE_signal_t s, LSE_dynid_t dynid, T *data)
  {
    /* dynid completely ignored! */
    if (!strcmp(name(),"data"))
    {
      if (LSE_signal_extract_data(s) == LSE_signal_something)
        *this = *data;
      else if (LSE_signal_extract_data(s) == LSE_signal_nothing)
        this->nothing();
    }
    else
    {
      cerr << "Cannot perform LSE_port_set on a port: " << name() << endl;
      exit(1);
    }
  }
#endif
};

template <class T>
class fsc_prim_signal : public fsc_signal_base
{public:
  fsc_prim_signal(const char *name = 0);

  operator const T& ();
  fsc_prim_signal<T>& operator = (const T& data);
  void write(const T& data);
  void nothing();

  bool is_known() const;
  bool is_nothing() const;
  bool is_something() const;
  const char *name();
 private:
  T buffer[2];
  friend class fsc_prim_out<T>;
  friend class fsc_prim_in<T>;
};

//#define FSC_CTOR(class_name) typedef class_name FSC_CURRENT_USER_MODULE; class_name(const fsc_module_name& name) : fsc_module(name, #class_name)
//#define FSC_METHOD(process_name) { typedef typeof(*this) FSC_CURRENT_USER_MODULE; declare_process(static_cast<fsc_method>(&FSC_CURRENT_USER_MODULE::process_name), #process_name); }
//#define FSC_MODULE(module_name) struct module_name : public fsc_module

/*************************** fsc_prim_in ***********************************/

/**
 * \brief Creates a new fsc_prim_in
 */
template <class T>
fsc_prim_in<T>::fsc_prim_in(const char *name) : fsc_prim_port(FSC_IN, name)
{
}

/**
 * \brief Connects a signal
 */
template <class T>
void fsc_prim_in<T>::operator () (fsc_prim_signal<T>& signal)
{ Bind(&signal);

}

/**
 * \brief Connects a port
 */
template <class T>
void fsc_prim_in<T>::operator () (fsc_prim_in<T>& port)
{ Bind(&port);
}

/**
 * \brief Returns true if the signal was set this cycle
 */
template <class T>
INLINE bool fsc_prim_in<T>::known() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  bool is_known = typed_signal->time_stamp[typed_signal->read_buf] == simcontext->TimeStamp();
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug) cerr << "FraternitéSysC: testing whether signal " << signal->Name() << " is known (" << (is_known ? "yes": "no") << ")" << endl;
#endif
  return is_known;
}

/**
 * \brief Returns true if the signal was set last cycle
 */
template <class T>
INLINE bool fsc_prim_in<T>::was_known() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  bool was_known = typed_signal->time_stamp[typed_signal->read_buf] == simcontext->TimeStamp()-1;
  return was_known;
}

/**
 * \brief Returns true if the signal is not set this cycle
 */
template <class T>
INLINE bool fsc_prim_in<T>::unknown() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  bool is_unknown = typed_signal->time_stamp[typed_signal->read_buf] != simcontext->TimeStamp();
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug) cerr << "FraternitéSysC: testing whether signal " << signal->Name() << " is known (" << (is_unknown ? "no": "yes") << ")" << endl;
#endif
  return is_unknown;
}

/**
 * \brief Returns true if the signal data is something
 */
template <class T>
INLINE bool fsc_prim_in<T>::something() const
{
  fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
#ifdef DEBUG_FSC

  bool is_unknown = typed_signal->time_stamp[typed_signal->read_buf] != simcontext->TimeStamp();
  if(is_unknown)
  {
    cerr << "FraternitéSysC: checking whether signal " << signal->Name() << " has a value while signal is unknown" << endl;
    exit(-1);
  }
#endif
  bool has_something = typed_signal->something[typed_signal->read_buf];

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FraternitéSysC: check whether port " << Name() << " has a value (" << (has_something ? "yes" : "no") << ")" << endl;
#endif

  return has_something;
}

/**
 * \brief Returns true if the signal data is nothing
 */
template <class T>
INLINE bool fsc_prim_in<T>::nothing() const
{
  fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  return typed_signal->is_nothing();
}

/**
 * \brief Returns the data of the signal
 */
template <class T>
INLINE fsc_prim_in<T>::operator const T& ()
{
  fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
#ifdef DEBUG_FSC

  bool is_unknown = typed_signal->time_stamp[typed_signal->read_buf] != simcontext->TimeStamp();
  if(is_unknown)
  {
    cerr << "FraternitéSysC: reading signal " << signal->Name() << " while signal is unknown" << endl;
    exit(-1);
  }
#endif
  const T& value = typed_signal->buffer[typed_signal->read_buf];

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    //DD//    cerr << "FraternitéSysC: reading (value = " << value << ") on port " << Name() << endl;
#endif

  return value;
}

/**
 * \brief Converts into a pointer
 */
template <class T>
INLINE T* fsc_prim_in<T>::get_pointer ()
{
  fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  return &typed_signal->buffer[typed_signal->read_buf];
}

/**************************** fsc_prim_out *********************************/

/**
 * \brief Creates a new fsc_prim_out
 */
template <class T>
fsc_prim_out<T>::fsc_prim_out(const char *name) : fsc_prim_port(FSC_OUT, name)
{}

/**
 * \brief Connects a new signal
 */
template <class T>
void fsc_prim_out<T>::operator () (fsc_prim_signal<T>& signal)
{
  Bind(&signal);
}

/**
 * \brief Connects a new port
 */
template <class T>
void fsc_prim_out<T>::operator () (fsc_prim_out<T>& port)
{
  Bind(&port);
}

/**
 * \brief Set the port data
 */
template <class T>
INLINE fsc_prim_out<T>& fsc_prim_out<T>::operator = (const T& data)
{
  fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  uint64_t simc_time_stamp = simcontext->TimeStamp();

  if(typed_signal->time_stamp[typed_signal->read_buf ^ 1] != simc_time_stamp)
  {
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug)
      //DD//      cerr << "FraternitéSysC: writing on port " << Name() << "(value = " << data << ")" << endl;
#endif

    typed_signal->time_stamp[typed_signal->read_buf ^ 1] = simc_time_stamp;
    typed_signal->buffer[typed_signal->read_buf ^ 1] = data;
    typed_signal->something[typed_signal->read_buf ^ 1] = true;
    /* typed_signal->to_update should be zero at this point */
    typed_signal->to_update = 1;
#ifdef DEBUG_FSC

    if(fsc_simcontext::debug)
      //DD//      cerr << "FraternitéSysC: submiting an update (new value = " << data << ", old value = " << typed_signal->buffer[typed_signal->read_buf] << ") for signal " << typed_signal->Name() << endl;
#endif

    simcontext->SubmitUpdate(signal);
  }
#ifdef DEBUG_FSC
  else
  { cerr << "FraternitéSysC: writing twice in the same port " << Name() << " (cycle = " << typed_signal->time_stamp[typed_signal->read_buf] << ")" << endl;
    exit(-1);
  }
#endif
  return *this;
}

/**
 * \brief Returns true if the signal was set this cycle
 */
template <class T>
INLINE bool fsc_prim_out<T>::known() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  // See discussion in fsc_prim_out<T>::operator const T&
  bool is_known = typed_signal->time_stamp[typed_signal->read_buf] == simcontext->TimeStamp();
  //	bool is_known = typed_signal->time_stamp[typed_signal->read_buf^1] == simcontext->TimeStamp();
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug) cerr << "FraternitéSysC: testing whether signal " << signal->Name() << " is known (" << (is_known ? "yes": "no") << ")" << endl;
#endif
  return is_known;
}

/**
 * \brief Returns true if the signal was set last cycle
 */
template <class T>
INLINE bool fsc_prim_out<T>::was_known() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  bool was_known = typed_signal->time_stamp[typed_signal->read_buf] == simcontext->TimeStamp()-1;
  return was_known;
}

/**
 * \brief Returns true if the signal is unknown
 */
template <class T>
INLINE bool fsc_prim_out<T>::unknown() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  bool is_unknown = typed_signal->time_stamp[typed_signal->read_buf^1] != simcontext->TimeStamp();
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug) cerr << "FraternitéSysC: testing whether signal " << signal->Name() << " is known (" << (is_unknown ? "no": "yes") << ")" << endl;
#endif
  return is_unknown;
}

/**
 * \brief Set the signal data to nothing
 */
template <class T>
INLINE void fsc_prim_out<T>::nothing()
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  uint64_t simc_time_stamp = simcontext->TimeStamp();

  if(typed_signal->time_stamp[typed_signal->read_buf ^ 1] != simc_time_stamp)
  {
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug) cerr << "FraternitéSysC: port " << Name() << " has no value" << endl;
#endif

    typed_signal->time_stamp[typed_signal->read_buf ^ 1] = simc_time_stamp;
    typed_signal->something[typed_signal->read_buf ^ 1] = false;
    /* typed_signal->to_update should be zero at this point */
    typed_signal->to_update = 1;
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug) cerr << "FraternitéSysC: submiting an update (no value) for signal " << typed_signal->Name() << endl;
#endif

    simcontext->SubmitUpdate(signal);
  }
#ifdef DEBUG_FSC
  else
  { cerr << "FraternitéSysC: writing twice in the same port " << Name() << " (cycle = " << typed_signal->time_stamp[typed_signal->read_buf] << ")" << endl;
    exit(-1);
  }
#endif
}

/**
 * \brief Returns true if the signal data is nothing
 */
template <class T>
INLINE bool fsc_prim_out<T>::is_nothing() const
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  return typed_signal->is_nothing();
}


/**
 * \brief Returns the data of the signal
 */
template <class T>
fsc_prim_out<T>::operator const T& ()
{ fsc_prim_signal<T> *typed_signal = static_cast<fsc_prim_signal<T> *>(signal);
  /*
     weird behavior here....
     
     if using read_buf ^ 1, then a handler reading the value in the same cycle but at a later 
     delta time step will have the old value.   But doing it this way will ensure that the same
     value is gotten in the SAME handler.	   
  */

  // cerr << "Reading from a prim_out " << typed_signal->buffer[typed_signal->read_buf] << endl;
  return typed_signal->buffer[typed_signal->read_buf];

  //return typed_signal->buffer[typed_signal->read_buf ^ 1];
}

/**
 * \brief Add a port to the dependency list
 */
template <class T>
void fsc_prim_out<T>::operator () (fsc_prim_port& port)
{ port.Dependency(this);
}

/**************************** fsc_prim_signal ******************************/

/**
 * \brief Creates a new fsc_prim_signal
 */
template <class T>
fsc_prim_signal<T>::fsc_prim_signal(const char *name) : fsc_signal_base(FSC_SIGNAL, name)
{ time_stamp[0] = time_stamp[1] = (uint64_t)-1;
}

/**
 * \brief Set the signal data
 */
template <class T>
fsc_prim_signal<T>& fsc_prim_signal<T>::operator = (const T& data)
{
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug)
    cerr << "FraternitéSysC: Writing " << data << " on signal " << Name() << endl;
#endif

  uint64_t simc_time_stamp = simcontext->TimeStamp();
  if(time_stamp[read_buf] != simc_time_stamp)
  { time_stamp[read_buf ^ 1] = simc_time_stamp;
    buffer[read_buf ^ 1] = data;
    something[read_buf ^ 1] = true;
    /* to_update should be zero at this point */
    to_update = 1;
#ifdef DEBUG_FSC

    if(fsc_simcontext::debug)
      cerr << "FraternitéSysC: submiting an update (new value = " << data << ", old value = " << buffer[read_buf] << ") for signal " << Name() << endl;
#endif

    simcontext->SubmitUpdate(this);
  }
#ifdef DEBUG_FSC
  else
  { cerr << "FraternitéSysC: writing twice in the same port " << Name() << " (cycle = " << time_stamp[read_buf] << ")" << endl;
    exit(-1);
  }
#endif
  return *this;
}

/**
 * \brief Set the signal data to nothing
 */
template <class T>
void fsc_prim_signal<T>::nothing()
{
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug)
    cerr << "FraternitéSysC: Writing nothing on signal " << Name() << endl;
#endif

  uint64_t simc_time_stamp = simcontext->TimeStamp();
  if(time_stamp[read_buf] != simc_time_stamp)
  { time_stamp[read_buf ^ 1] = simc_time_stamp;
    something[read_buf ^ 1] = false;
    /* to_update should be zero at this point */
    to_update = 1;
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug) cerr << "FraternitéSysC: submiting an update (no value) for signal " << Name() << endl;
#endif

    simcontext->SubmitUpdate(this);
  }
#ifdef DEBUG_FSC
  else
  { cerr << "FraternitéSysC: writing twice in the same port " << Name() << " (cycle = " << time_stamp[read_buf] << ")" << endl;
    exit(-1);
  }
#endif
}

/**
 * \brief Returns true if the signal is known
 */
template <class T>
INLINE bool fsc_prim_signal<T>::is_known() const
{ bool is_unknown = time_stamp[read_buf] != simcontext->TimeStamp();
  return !is_unknown;
}

/**
 * \brief Returns true if the signal is not nothing
 */
template <class T>
INLINE bool fsc_prim_signal<T>::is_something() const
{
#ifdef DEBUG_FSC
  bool is_unknown = time_stamp[read_buf] != simcontext->TimeStamp();
  if(is_unknown)
  { cerr << "FraternitéSysC: checking whether signal " << Name() << " has a value while signal is unknown" << endl;
    exit(-1);
  }
#endif
  bool has_something = something[read_buf];

#ifdef DEBUG_FSC
  if(fsc_simcontext::debug) cerr << "FraternitéSysC: check whether signal " << Name() << " has a value (" << (has_something ? "yes" : "no") << ")" << endl;
#endif

  return is_known() && has_something;
}

/**
 * \brief Returns true if the signal is nothing
 */
template <class T>
INLINE bool fsc_prim_signal<T>::is_nothing() const
{ bool has_something = something[read_buf];
  return is_known() && !has_something;
}

/**
 * \brief Set the data
 */
template <class T>
void fsc_prim_signal<T>::write(const T& data)
{ (*this) = data;
}

/**
 * \brief Get the signal data
 */
template <class T>
fsc_prim_signal<T>::operator const T& ()
{ return buffer[read_buf];
}

/**************************** inclock ******************************/

/**
 * \brief Return the boolean clock value
 */
INLINE inclock::operator const bool& ()
{ fsc_clock *clock = static_cast<fsc_clock *>(signal);
  return clock->value;
}

/**
 * \brief Connect a clock port to the clock signal
 */
INLINE void inclock::operator () (inclock& port)
{ Bind(&port);
}

/**************************** fsc_clock *******************************/

/**
 * \brief Switch clock edge
 */
INLINE void fsc_clock::NextEdge()
{ value = !value;
}

/**************************** fsc_simcontext **************************/

/**
 * \brief Add a signal to the list of signal thats need to be updated
 */
INLINE void fsc_simcontext::SubmitUpdate(fsc_signal_base *signal)
{
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug)
    cerr << "SubmitUpdate on signal " << signal->Name() << endl;
#endif

  *(++signal_to_update) = signal;
}

/**
 * \brief Returns true if simulation is stopped
 */
INLINE bool fsc_stopped()
{ return simcontext->stop;
}

/**
 * \brief Returns true if currently debugging
 */
INLINE bool fsc_debugging()
{ return fsc_simcontext::debug;
}

#if !defined(IN_FSC)
#if !defined(GENSCHED)
extern void fsc_phase();
extern void fsc_start(uint64_t t);
extern void fsc_start_alt(uint64_t t);
extern void fsc_start_acyclic(uint64_t t);
extern void fsc_stabilize();
#else
#if defined(GENSCHED)
#define fsc_start(t) simcontext->GenScheduler(SCHEDULER_FILENAME)
#endif
#endif
#endif

extern uint64_t timestamp();
extern void fsc_stop();
extern void fsc_debug(uint64_t start, uint64_t stop);

#undef GCC_INLINE

#endif
