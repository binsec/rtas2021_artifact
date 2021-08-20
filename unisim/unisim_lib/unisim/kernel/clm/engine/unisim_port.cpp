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

#include "unisim.h"

List<unisim_port> unisim_port::unisim_inport_list;
List<unisim_port> unisim_port::unisim_outport_list;

/**
 * \brief Creates a new port
 */
unisim_port::unisim_port()
{ connected = 0;
  forward = false;
  parent_module = NULL;
  connected_port = NULL;
  latex_rendering_fused = false;
  name = "Unset !!!";
}

/**
 * \brief Class destructor
 */
unisim_port::~unisim_port()
{
}

/**
 * \brief Check that every port of every instanciated module is connected
 *
 * Lauched at the beggining of the simulation to check that all ports
 * are connected.
 */
void unisim_port::check_connections()
{ cerr << "Checking port connection..." << endl;
  ListPointer<unisim_port> port;
  bool error_found = false;

  // For each input port created, check it has been connected to exactly 1 port
  for(port = unisim_inport_list.Begin(); port; port++)
  { if(port->connected == 0)
    { error_found = true;
      if(port->parent_module) cerr << "  Input port '" << *port << "' of instance '" << port->parent_module->name() << "' of module '" << port->parent_module->class_name << "' is not connected." << endl;
      else                    cerr << "  Input port '" << *port << "' of unknown instance is not connected." << endl;
    }
    if(port->connected > 1)
    { error_found = true;
      if(port->parent_module) cerr << "  Input port '" << *port << "' of instance '" << port->parent_module->name() << "' of module '" << port->parent_module->class_name << "' is connected " << port->connected << " times." << endl;
      else                    cerr << "  Input port '" << *port << "' of unknown instance is connected " << port->connected << " times." << endl;
    }
  }
  // For each output port created, check it has been connected to exactly 1 port
  for(port = unisim_outport_list.Begin(); port; port++)
  { if(port->connected == 0)
    { error_found = true;
      if(port->parent_module) cerr << "  Output port '" << *port << "' of instance '" << port->parent_module->name() << "' of module '" << port->parent_module->class_name << "' is not connected." << endl;
      else                    cerr << "  Output port '" << *port << "' of unknown instance is not connected." << endl;
    }
    if(port->connected > 1)
    { error_found = true;
      if(port->parent_module) cerr << "  Output port '" << *port << "' of instance '" << port->parent_module->name() << "' of module '" << port->parent_module->class_name << "' is connected " << port->connected << " times." << endl;
      else                    cerr << "  Output port '" << *port << "' of unknown instance is connected " << port->connected << " times." << endl;
    }
  }
  //Do not start the simulation if some ports are not connected
  if(error_found)
  { cerr << "Error: Some ports are not connected." << endl;
    exit(1);
  }
  else cerr << "All ports are connected." << endl;
}

/**
 * \brief Check that every signal input / output port signals are known
 *
 * Lauched at end of cycle to make sure that every signal was set.
 */
void unisim_port::check_knowness(ostream &os, bool fatal_unknown)
{ ListPointer<unisim_port> port;
  bool error_found = false;
  // For each input port created, check that the 3 signals are knwown
  for(port = unisim_inport_list.Begin(); port; port++)
  { if(!port->forward)
    { if (!port->check_my_knowness()) error_found = true;
    }
  }
  // For each output port created, check that the 3 signals are knwown
  for(port = unisim_outport_list.Begin(); port; port++)
  { if(!port->forward)
    { if (!port->check_my_knowness()) error_found = true;
    }
  }

  // Stop the simulation if some signal were unknown
  if(error_found)
  { os << "+---------------+--------------------------------------+" << endl;
    os << "| Cycle " << left << setw(8) << timestamp() << "| Some signals unknown at end of cycle |" << endl;
    os << "+---------------+--------------------------------------+" << endl;
    for(port = unisim_inport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_knowness(os);
    }
    os << "+------------------------------------------------------+" << endl;
    for(port = unisim_outport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_knowness(os);
    }
    os << "+------------------------------------------------------+\n" << endl;
    if(fatal_unknown) exit(1);
  }

  //#define DD_MY_SIGNAL_DEBUGGER
#ifdef DD_MY_SIGNAL_DEBUGGER
  os << "+---------------------------------------------+--------------------------------------+" << endl;
  os << "| Cycle " << left << setw(38) << timestamp() << "| DD MY SIGNAL DEBUGGER |" << endl;
  os << "+---------------------------------------------+--------------------------------------+" << endl;
  for(port = unisim_inport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_knowness(os);
    }
  os << "+------------------------------------------------------------------------------------+" << endl;
  for(port = unisim_outport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_knowness(os);
    }
  os << "+------------------------------------------------------------------------------------+\n" << endl;
#endif
#undef DD_MY_SIGNAL_DEBUGGER

}

// DD used to debug signals...
/**
 * \brief Check that every signal input / output port signals are known
 *
 * Lauched at end of cycle to make sure that every signal was set.
 */
void unisim_port::check_signals(ostream &os, bool fatal_unknown)
{ ListPointer<unisim_port> port;
//  bool error_found = false;
  // For each input port created, check that the 3 signals are knwown
  /*
  for(port = unisim_inport_list.Begin(); port; port++)
  { if(!port->forward)
    { if (!port->check_my_knowness()) error_found = true;
    }
  }
  */
  // For each output port created, check that the 3 signals are knwown
  /*
  for(port = unisim_outport_list.Begin(); port; port++)
  { if(!port->forward)
    { if (!port->check_my_knowness()) error_found = true;
    }
  }
  */
  // Stop the simulation if some signal were unknown
  /*
  if(error_found)
  { os << "+---------------+--------------------------------------+" << endl;
    os << "| Cycle " << left << setw(8) << timestamp() << "| Some signals unknown at end of cycle |" << endl;
    os << "+---------------+--------------------------------------+" << endl;
    for(port = unisim_inport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_knowness(os);
    }
    os << "+------------------------------------------------------+" << endl;
    for(port = unisim_outport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_knowness(os);
    }
    os << "+------------------------------------------------------+\n" << endl;
    if(fatal_unknown) exit(1);
  }
  */
  os << "+---------------------------------------------+--------------------------------------+" << endl;
  os << "| Cycle " << left << setw(38) << timestamp() << "| DD MY SIGNAL DEBUGGER |" << endl;
  os << "+---------------------------------------------+--------------------------------------+" << endl;
  for(port = unisim_inport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_signal(os);
    }
  /*
  os << "+------------------------------------------------------------------------------------+" << endl;
  for(port = unisim_outport_list.Begin(); port; port++)
    { if(!port->forward) port->display_my_signal(os);
    }
  */
  os << "+------------------------------------------------------------------------------------+\n" << endl;
}



/**
 * \brief Check that the 3 signals of the port are knwown
 */
bool unisim_port::check_my_knowness()
{ cerr << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << ": Only the overloaded method should be called." << endl;
  exit(1);
}

void unisim_port::display_my_knowness(ostream &os)
{ cerr << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << ": Only the overloaded method should be called." << endl;
  exit(1);
}
// DD used to debug signals
void unisim_port::display_my_signal(ostream &os)
{ cerr << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << ": Only the overloaded method should be called." << endl;
  exit(1);
}


/**
 * \brief Pretty printer of the unisim_port class
 */
ostream & operator<<(ostream &os, const unisim_port &p)
{ os << p.name;
  return os;
}

/**
 * \brief Returns the name of the port
 */
string unisim_port::get_name()
{ return name;
}

/**
 * \brief Returns the instance name of the object this ports belongs to
 */
string unisim_port::get_module_instance_name()
{ return parent_module->name();
}

/**
 * \brief Returns the class name of the object this ports belongs to
 */
string unisim_port::get_module_class_name()
{ return parent_module->class_name;
}

/**
 * \brief Returns the port connected to this port 
 */
unisim_port * unisim_port::get_connected_port()
{ return connected_port;
}

/**
 * \brief Returns the module connected to this port 
 */
unisim_module * unisim_port::get_connected_module()
{ if(connected_port) return connected_port->parent_module;
//  if(forward) return forwarded_port->get_connected_module();
  cerr << "Error: port '" << name << "' don't seems to be properly connected. Aliased=" << forward << endl;
  exit(1);
}

//#ifdef USE_UNISIM_SIGNAL_ARRAY
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * \brief Creates a new Unisim_Outport_Base
 */
/*
template <uint32_t NCONFIG>
Unisim_Outport_Base<NCONFIG>::Unisim_Outport_Base() : unisim_port(), enable("enable"), accept("accept")
{ unisim_module::unisim_current_module->register_outport(this);
}
*/
/**
 * \brief Creates a new Unisim_Inport_Base
 */
/*
template <uint32_t NCONFIG>
Unisim_Inport_Base<NCONFIG>::Unisim_Inport_Base() : unisim_port(), enable("enable"), accept("accept")
{ unisim_module::unisim_current_module->register_inport(this);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Creates a new Unisim_Outport_Base
 */
/*
template <>
Unisim_Outport_Base<1>::Unisim_Outport_Base() : unisim_port(), enable("enable"), accept("accept")
{ unisim_module::unisim_current_module->register_outport(this);
}
*/
/**
 * \brief Creates a new Unisim_Inport_Base
 */
/*
template <>
Unisim_Inport_Base<1>::Unisim_Inport_Base() : unisim_port(), enable("enable"), accept("accept")
{ unisim_module::unisim_current_module->register_inport(this);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#else
/**
 * \brief Creates a new Unisim_Outport_Base
 */
/*
Unisim_Outport_Base::Unisim_Outport_Base() : unisim_port(), enable("enable"), accept("accept")
{ unisim_module::unisim_current_module->register_port(this);
}
*/
/**
 * \brief Creates a new Unisim_Inport_Base
 */
/*
Unisim_Inport_Base::Unisim_Inport_Base() : unisim_port(), enable("enable"), accept("accept")
{ unisim_module::unisim_current_module->register_port(this);
}
#endif
*/
/**
 * \brief For Latex Rendering set the signal has fused
 */
void unisim_port::set_fused()
{
  latex_rendering_fused = true;
}

bool unisim_port::is_fused()
{
  return latex_rendering_fused;
}

