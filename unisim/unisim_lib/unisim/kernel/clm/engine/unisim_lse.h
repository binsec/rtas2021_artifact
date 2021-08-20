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

#ifndef __INCLUDE__UNISIM_LSE_H__
#define __INCLUDE__UNISIM_LSE_H__

#define FUNC(x,...) x(__VA_ARGS__)
#define GLOBDEF(x,y) x GLOB__##y
#define GLOB(x) GLOB__##x
#define HANDLER(x,y) handler__##x(y)

#define LSE_port_width(x) 6
#define LSE_port_type(x) LSE_port_type__##x


#define LSE_userpoint_invoke(f,...) f(__VA_ARGS__)

#define LSE_signal_data_present(x) (LSE_signal_extract_data(x)==LSE_signal_something)
#define LSE_signal_enable_present(x) (LSE_signal_extract_enable(x)==LSE_signal_enabled)
#define LSE_signal_ack2enable(x) (LSE_signal_extract_ack(x)==LSE_signal_ack ? LSE_signal_enabled : (LSE_signal_extract_ack(x)==LSE_signal_nack ? LSE_signal_disabled : LSE_signal_unknown))
// FIXME
#define LSE_userpoint_defaulted(x) 0

// FIXME
#define LSE_event_record(...) 
#define PARM(x) LSE_PARM__##x
#define LSE_parm_constant(x) 0

#define LSE_time_now timestamp()
#define LSE_sim_keep_alive(x) 

#define LSE_report_err(x) { cerr<<x<<endl;exit(1); }
#define punt(x) LSE_report_err(x)

// Refcounting stuff
#define LSE_dynid_create() NULL
#define LSE_dynid_register(x) 
#define LSE_dynid_cancel(x) 
#define LSE_data_move(type,dest,src) *dest = *src
#define LSE_data_register(x,y)
#define LSE_data_cancel(x,y)

#define LSE_signal_all_no (LSE_signal_nack | LSE_signal_disabled | LSE_signal_nothing)

#define LSE_signal_data_known(x) (LSE_signal_extract_data(x))
#define LSE_signal_ack_known(x) (LSE_signal_extract_ack(x))

#define LSE_signal_unknown 00


#define LSE_LOOP_OVER_PORT(i,p) for(i=0;i<LSE_port_width(p);i++)
#define LSE_LOOP_END

#define LSE_time_numticks_t uint64_t

#define boolean bool
//#define TRUE true
//#define FALSE false
static const bool TRUE = true;
static const bool FALSE = false;

#define ack accept

#define LSE_enum_value(type,val) LSE__PACKAGE__##type val


template<class T, bool B> class inport;
template<class T, bool B> class outport;

class unisim_module;
//typedef void (fsc_module::*fsc_method)();

template <class T, class U> class LSE_port{
  unisim_module *m;

 public:
  bool handler;
  bool independent;
  unsigned int width;
  U **ports;

  fsc_method handler_func;
  const char *handler_func_name;
  
  void set_module(unisim_module *_m) {
    m = _m;
  }
  
  LSE_port() : width(0), ports(NULL), handler_func(NULL), handler(false) {  }
  U & operator [](unsigned int i) { 
    if (i >= width) {
      /* we need to create some new ports */
      ports = (U**)realloc(ports, (i+1)*sizeof(U));
      for (int j = width; j < i+1; j++) {
	ports[j] = new U();
	/* also make sure it gets handled */
	if (handler) {
	  m->declare_process(handler_func,handler_func_name);
	  m->unisim_sensitive << *ports[j];
	}
      }
      width = i+1;
    }
    return *ports[i];
  }

  void add_handler(fsc_method f, const char *n) {
    handler_func = f;
    handler_func_name = n;
  }
};

template <class T> class LSE_inport : public LSE_port<T, inport<T> > {
 public:
  LSE_inport() : LSE_port<T, inport<T> > () { }  

};

template <class T> class LSE_outport : public LSE_port<T, outport<T> > {
 public:
  LSE_outport() : LSE_port<T, outport<T> > () { }

};

#define LSE_port_get(p,i,d) p.port_get(i,d)
#define LSE_port_set(p,...) p.port_set(__VA_ARGS__)

class LSE_module : public module {
 public:
  virtual void phase_start(uint64_t t) {} 
  virtual void phase_end(void) {} 
  virtual void phase(void) {} 

  void phase_start_wrapper(void) { phase_start(0); }

  void declare_process(fsc_method m, const char *n) { 
    module::declare_process(m,n);
  }

  LSE_module(char *name)  : module(name) { } 
};

#define HANDLER_WRAPPER(x) void handler__##x##__wrapper(void) { \
                             for (int i=0;i<x.width;i++) { \
                                handler__##x(i); \
                             }\
                           }



#endif
