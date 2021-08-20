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

#include <csignal>
#include <iostream>
#include <fstream>
#include <limits.h>
#include "unisim.h"
#include "unisim_commandline.h"

#ifdef WIN32
#include <windows.h>
#else
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif
#endif

fsc_simcontext *Simulator::context = 0;

/**
 * \brief Creates a new Simulator
 */  
Simulator::Simulator()
{ if (context) throw runtime_error("simcontext defined twice !");
  context = new fsc_simcontext;
  
  //By default, check knowness at end of cycle, and fatal if unknown
  _unknown_fatal = true;
  _unknown_display = true;  
  _signal_display = false;  
  
  //If check-known is set as a commandline parameter
  if(command_line["check-known"])
  { string v = command_line["check-known"];
    if(v == "LOG")
    { // Output logs to knowns.log, as non fatal
      _stream_knowness.open("knowns.log",ios_base::trunc);
      _unknown_display = true;
      _unknown_fatal = false;
    }
    else if (v == "FATAL")
    { // Outout logs to cerr, as fatal
      _unknown_fatal = true;
      _unknown_display = true;
      cerr << "check-known is set to FATAL" << endl;
    }
    else if (v == "NONE")
    { // Do not check knowness
      _unknown_fatal = false;
      _unknown_display = false;
    }
    else 
    { cerr << "check-known is set to " << v << " !" << endl;
      exit(1);
    }
  }

  // DD used to debug signals...
  //If check-signals is set as a commandline parameter
  if(command_line["check-signals"])
  { string v = command_line["check-signals"];
    if(v == "LOG")
    { // Output logs to knowns.log, as non fatal
      _stream_knowness.open("signals.log",ios_base::trunc);
      _signal_display = true;
      //      _unknown_fatal = false;
    }
    else if (v == "FATAL")
    { // Outout logs to cerr, as fatal
      //      _unknown_fatal = true;
      _signal_display = true;
      cerr << "check-signals is set to FATAL" << endl;
    }
    else if (v == "NONE")
    { // Do not check knowness
      //      _unknown_fatal = false;
      _signal_display = false;
    }
    else 
    { cerr << "check-signals is set to " << v << " !" << endl;
      exit(1);
    }
  }



}

/**
 * \brief Class destructor
 */  
Simulator::~Simulator()
{ cerr << "Simulation ended at cycle " << timestamp() << endl;
  delete context;
  if (_stream_knowness.is_open())
  { _stream_knowness.close();
  }
}

/**
 * \brief Returns the current cycle number
 */
uint64_t Simulator::TimeStamp()
{ return context->TimeStamp();
}

/**
 * \brief Returns the stream where to print the unknown signal messages
 */
ostream & Simulator::stream_knowness()
{ if (_stream_knowness.is_open())
  { return _stream_knowness;
  }
  else
  { return cerr;
  } 
}

/**
 * \brief CTRL-C interrupt handler.
 *
 * Terminate the simulation at the end of the current cycle. Perform a real CTRL-C if pressed twice.
 */
void Simulator::at_ctrlc(int sig)
{ // On a CTRL-C do not end abruptly, but end the simulation at the end of the cycle.
  terminate_now();
  // Just in case CTRL-C is pressed a second time... Put back the default handler.
  signal(SIGINT, SIG_DFL);
}

/**
 * \brief CTRL-Z interrupt handler. Display the current cycle number, and call the ctrlz hook
 * of each instantiated module.
 *
 * If not overloaded, the default hook is doing nothing.
 */
void Simulator::at_ctrlz(int sig)
{ cerr << "[CTRL-Z] Current-cycle: " << timestamp() << endl;
  ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->ctrlz_hook(); 
  }
}

void Simulator::dump_machine_description(const string &filename)
{ ofstream os(filename.c_str(), ios_base::trunc);
  os << "<simulator>" << endl;
  ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->dump_machine_description(os);
  } 
  os << "</simulator>" << endl;
  os.close();
}

void Simulator::dump_dot(const string &filename)
{ ofstream os(filename.c_str(), ios_base::trunc);
  os << "digraph {" << endl;
  os << "rankdir = LR" << endl;
  ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->dump_dot_nodes(os);
  } 
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->dump_dot_conns(os);
  } 
  os << "}" << endl;
  os.close();
}

void Simulator::dump_latex(const string &filename)
{ ofstream os(filename.c_str(), ios_base::trunc);
  os << "\\documentclass{article}" << endl;
  os << "\\usepackage{tikz}" << endl;
  os << "\\usetikzlibrary{shapes,snakes,arrows,automata}" << endl;
  os << "\\usepackage[normalem]{ulem}" << endl;
  os << "\\usepackage[margin=0cm,landscape,nohead,nofoot]{geometry}" << endl;
  os << "\\pagestyle{empty}" << endl;
  os << "\\raggedbottom" << endl;
  os << "\\begin{document}" << endl;
  os << "\\begin{tikzpicture}" << endl;

  os << "\\tikzstyle{bigmodule}=[draw=yellow!50!black,thick,fill=yellow!20,font=\\sffamily,minimum width=1cm,minimum height=1cm,node distance=0.5cm,inner sep=0cm,text width=1.0cm,text centered,anchor=north west]" << endl;
  os << "\\tikzstyle{bigport}=[rectangle,draw=yellow!50!black,thick,fill=black!20,inner sep=0pt,node distance=5mm,font=\\sffamily\\tiny]" << endl;
  os << "\\tikzstyle{biglport}=[bigport,anchor=north west,minimum height=0.5cm,minimum width=2.0mm]" << endl;
  os << "\\tikzstyle{bigrport}=[bigport,anchor=north east,minimum height=0.5cm,minimum width=2.0mm]" << endl;
  os << "\\tikzstyle{bigbport}=[bigport,anchor=south west,minimum height=2.0mm,minimum width=0.5cm]" << endl;
  os << "\\tikzstyle{bigtport}=[bigport,anchor=south east,minimum height=2.0mm,minimum width=0.5cm]" << endl;
  os << "\\tikzstyle{signal}=[->,>=latex,very thick,above,font=\\sffamily]" << endl;

  int rank_min=INT_MAX, rank_max=INT_MIN;
  ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { int rank = m->get_rank();
    if(rank<rank_min) rank_min = rank;
    if(rank>rank_max) rank_max = rank;
  }
  bool first=true;
  for(int rank = rank_min; rank <= rank_max; rank++)
  { for(m = unisim_module::unisim_module_list.Begin(); m; m++)
    { if(rank==m->get_rank())
      { m->compute_latex_ports();
        m->dump_latex_module(os,first);
        m->dump_latex_ports(os);
        first=false;
      }
    }
  }
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->dump_latex_conns(os);
  }

  os << "\\end{tikzpicture}" << endl;
  os << "\\end{document}" << endl;
  os.close();
}

/*
 * See if there's any pending operation anywhere in the simulator
 */
bool Simulator::has_pending_ops()
{ ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { if(m->has_pending_ops()) return true; 
  }
  return false;
}

void Simulator::show_progress()
{ ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->show_progress();
  }
}

#ifdef WIN32
DWORD WINAPI progress_bar_thread_statup(LPVOID sim)
#else
void* progress_bar_thread_statup(void* sim)
#endif
{ Simulator *s = (Simulator *)sim;
  while(1)
  { s->show_progress();
#ifdef WIN32
    Sleep(5000);
#else
    sleep(5);
#endif
  }
#ifdef WIN32
  ExitThread(0);
#else
#if defined(HAVE_PTHREAD)
  pthread_exit(NULL);
#endif
#endif
}

void Simulator::set_progress_max(uint64_t max)
{
#ifdef WIN32
  HANDLE t;
#else
#if defined(HAVE_PTHREAD)
  pthread_t t;
#endif
#endif
  start_time=time(NULL);
  ListPointer<unisim_module> m;
  for(m = unisim_module::unisim_module_list.Begin(); m; m++)
  { m->set_progress_max(start_time,max);
  }
#ifdef WIN32
  LPSECURITY_ATTRIBUTES lpThreadAttributes = 0; // the thread gets a default security descriptor
  SIZE_T dwStackSize = 0; // use the default stack size
  LPTHREAD_START_ROUTINE lpStartAddress = &progress_bar_thread_statup;
  LPVOID lpParameter = this;
  DWORD dwCreationFlags = 0; // The thread runs immediately after creation
  LPDWORD lpThreadId = NULL; // the thread identifier is not returned

  t = CreateThread(
     lpThreadAttributes,
     dwStackSize,
     lpStartAddress,
     lpParameter,
     dwCreationFlags,
     lpThreadId);
#else
#if defined(HAVE_PTHREAD)
  pthread_create(&t, NULL, progress_bar_thread_statup, this);
#endif
#endif
}
