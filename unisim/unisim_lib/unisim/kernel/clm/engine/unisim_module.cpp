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
              unisim_module.cpp  - UNISIM module class
                             -------------------
    begin                : Mon 17 2006
    author               : Sylvain Girbal
    email                : sylvain.girbal@inria.fr
 ***************************************************************************/

#include "unisim.h"

List<unisim_module> unisim_module::unisim_module_list;
unisim_module *unisim_module::unisim_current_module;

/**
 * \brief Creates a new unisim_module
 * \param name Name of the module
 */
unisim_module::unisim_module(const string & name): fsc_module(name), unisim_sensitive(this->sensitive), unisim_sensitive_pos(this->sensitive_pos), unisim_sensitive_neg(this->sensitive_neg)
{ unisim_module_list << this;
  unisim_current_module = this;
  category = category_UNKNOWN;
}

/**
 * \brief Creates a new unisim_module
 * \param name Name of the module
 */
unisim_module::unisim_module(const char *name): fsc_module(name), unisim_sensitive(this->sensitive), unisim_sensitive_pos(this->sensitive_pos), unisim_sensitive_neg(this->sensitive_neg)
{ unisim_module_list << this;
  unisim_current_module = this;
  category = category_UNKNOWN;
}

/**
 * \brief unisim_module pretty printer
 */
ostream& operator << (ostream& os, const unisim_module &mod)
{ os << mod.Name();
  return os;
}

void unisim_module::dump_machine_description(ostream &os)
{ os << " <module name=\"" << Name() <<  "\">" << endl;
//  os << "  <name>" << Name() << "</name>" << endl;
  parameters.dump_machine_description(os);
  os << " </module>" << endl;
}

void unisim_module::dump_dot_nodes(ostream &os)
{ list<string> left;
  list<string> right;
  string name = Name();


  for(list<unisim_port*>::iterator p=module_inport_list.begin();p!=module_inport_list.end();p++)
  { string pname = (*p)->get_name();
    if(pname.find("MEM")!=string::npos)
    { // MEM ports => to the right
      right.push_back(pname);
    }
    else 
    { left.push_back(pname);
    }
  }
  for(list<unisim_port*>::iterator p=module_outport_list.begin();p!=module_outport_list.end();p++)
  { string pname = (*p)->get_name();
    if(pname.find("MEM")!=string::npos)
    { // MEM ports => to the right
      right.push_back(pname);
    }
    else 
    { left.push_back(pname);
    }
  }

//  left.sort();
//  right.sort();
//  right.reverse();
 
  os << name << " [shape=record,label=\"{{";
  for(list<string>::iterator l=left.begin();l!=left.end();l++)
  { os << "<" << *l << ">" << *l << "|";
  }
  os << "}|" << name << "|{";
  for(list<string>::iterator r=right.begin();r!=right.end();r++)
  { os << "<" << *r << ">" << *r << "|";
  }
  os << "}}\"]" << endl;
}

void unisim_module::dump_dot_conns(ostream &os)
{ 
  list<unisim_port*>::iterator o;
  for(o=module_outport_list.begin();o!=module_outport_list.end();o++)
  { string pname = (*o)->get_name();
    os << '"' << Name() << "\":\"" << pname << "\" -> \"";
    os << (*o)->get_connected_module()->Name() << "\":\"" << (*o)->get_connected_port()->get_name();
    os << '"';
    if(pname.find("MEM")==string::npos)
    { os << " [constraint=false]";
    }
    os << endl;
  }
}

void unisim_module::dump_latex_module(ostream &os, bool first)
{ string name = Name();
  string latex_name = "";

  unsigned int height = 3;
  if(latex_left_ports.size()*3/2>height) height = latex_left_ports.size()*3/2;
  if(latex_right_ports.size()*3/2>height) height = latex_right_ports.size()*3/2;

  for(unsigned int i=0;i<name.size();i++)
  { if(name[i]=='_') latex_name += "\\_";
    else latex_name += name[i];
  }
  os << "% --- " << name << " module ---------------------" << endl;
  os << "\\draw ";
  if(!first) os << "(module.north east) +(2,0) ";
  os << "node [bigmodule,minimum height=" << height << "cm] (module) {" << latex_name << "};" << endl;
}

void unisim_module::compute_latex_ports()
{ if( latex_left_ports.empty() && latex_right_ports.empty() && latex_top_ports.empty() && latex_bottom_ports.empty() ) 
  { // No latex hints specified, let's guess a layout from port names
    for(list<unisim_port*>::iterator p=module_inport_list.begin();p!=module_inport_list.end();p++)
    { string pname = (*p)->get_name();
      if(pname.find("MEM")!=string::npos) latex_right_ports.push_back(*p);
      else                                latex_left_ports.push_back(*p);
    }
    for(list<unisim_port*>::iterator p=module_outport_list.begin();p!=module_outport_list.end();p++)
    { string pname = (*p)->get_name();
      if(pname.find("MEM")!=string::npos) latex_right_ports.push_back(*p);
      else                                latex_left_ports.push_back(*p);
    }

    latex_left_ports.sort();
    latex_right_ports.sort();
    latex_right_ports.reverse();
  }
}

void unisim_module::dump_latex_ports(ostream &os)
{ string x,y;
  int c;
  int fused_size;
  const char*port_distance="0.30";
  c = 1;
  y = "-0.33";
  fused_size = 1;
  for(list<unisim_port*>::iterator p=latex_left_ports.begin();p!=latex_left_ports.end();p++)
    { if ( ((*p)->latex_rendering_fused) )
      {
	fused_size++;
      }
    else
      {
	os << "\\draw (module.north west) +(0," << y << ") node [biglport] (" << Name() << "_" << (*p)->get_name() << ") {\\pgftext[rotate=90]{" << (*p)->get_name() << "(" << fused_size << ")" << "}};" << endl;
	y += "-"; y+= port_distance; y += "-0.33";
	c = 1-c;
	if(c) y += "-0.33";
      }
    }
  c = 1;
  y = "-0.33";
  fused_size = 1;
  for(list<unisim_port*>::iterator p=latex_right_ports.begin();p!=latex_right_ports.end();p++)
    { if ( ((*p)->latex_rendering_fused) )
      {
	fused_size++;
      }
    else
      {
	os << "\\draw (module.north east) +(0," << y << ") node [bigrport] (" << Name() << "_" << (*p)->get_name() << ") {\\pgftext[rotate=90]{" << (*p)->get_name() << "(" << fused_size << ")" << "}};" << endl;
	//    y += "-1-0.33";
	y += "-"; y+= port_distance; y += "-0.33";
	c = 1-c;
      }
    }
  c = 1;
  x = "0.33";
  fused_size = 1;
  for(list<unisim_port*>::iterator p=latex_bottom_ports.begin();p!=latex_bottom_ports.end();p++)
  { if ( ((*p)->latex_rendering_fused) )
      {
	fused_size++;
      }
    else
    {
      os << "\\draw (module.south west) +(" << x << ",0) node [bigbport] (" << Name() << "_" << (*p)->get_name() << ") {\\pgftext{" << (*p)->get_name() << "}};" << endl;
      //    x += "+1+0.33";
      x += "+"; x+= port_distance; x += "+0.33";
      c = 1-c;
    }
  }
  c = 1;
  x = "0.33";
  for(list<unisim_port*>::iterator p=latex_top_ports.begin();p!=latex_top_ports.end();p++)
    { if ( ((*p)->latex_rendering_fused) )
      {
	fused_size++;
      }
    else
      {
	os << "\\draw (module.south east) +(" << x << ",0) node [bigbport] (" << Name() << "_" << (*p)->get_name() << ") {\\pgftext{" << (*p)->get_name() << "(" << fused_size << ")" << "}};" << endl;
	//    x += "+1+0.33";
	x += "+"; x+= port_distance; x += "+0.33";
	c = 1-c;
      }
    }
}

void unisim_module::dump_latex_conns(ostream &os)
{ list<unisim_port*>::iterator o;
  char link_from, link_to;
  os << "% --- connections ---------------------" << endl;
  for(o=module_outport_list.begin();o!=module_outport_list.end();o++)
  { 
    if (! ((*o)->is_fused()) )
    {
    string pname = (*o)->get_name();
    link_from = '-';
    link_to = '-';
    os << "\\draw [signal] (" << Name() << "_" << pname << ") ";

    for(list<unisim_port*>::iterator p=latex_bottom_ports.begin();p!=latex_bottom_ports.end();p++)
    { if(pname==(*p)->get_name()) link_from = '|';
    }
    for(list<unisim_port*>::iterator p=latex_top_ports.begin();p!=latex_top_ports.end();p++)
    { if(pname==(*p)->get_name()) link_from = '|';
    }

    unisim_module *to_module = (*o)->get_connected_module();
    unisim_port *to_port = (*o)->get_connected_port();

    if (to_module==NULL) { cerr << "DD Kernel Error: to_module = NULL" << endl; exit(-1); }
    if (to_port==NULL) { cerr << "DD Kernel Error: to_port = NULL" << endl; exit(-1); }

    for(list<unisim_port*>::iterator p=to_module->latex_bottom_ports.begin();p!=to_module->latex_bottom_ports.end();p++)
    {
      /*
      cerr << "DD Kernel Latex rendering Debug :" << endl;
      cerr << "to_port: " << (*to_port) << endl;
      cerr << "p: " << *(*p) << endl;
      */
      if(to_port->get_name()==(*p)->get_name()) link_to = '|';
    }
    for(list<unisim_port*>::iterator p=to_module->latex_top_ports.begin();p!=to_module->latex_top_ports.end();p++)
    {
      /*
      cerr << "DD Kernel Latex rendering Debug :" << endl;
      cerr << "to_module: " << (*to_module) << endl;
      cerr << "to_port: " << (*to_port) << endl;
      cerr << "p: " << *(*p) << endl;
      */
      if(to_port->get_name()==(*p)->get_name()) link_to = '|';
    }

    os << link_from << link_to;
    os << " (" << (*o)->get_connected_module()->Name() << "_" << (*o)->get_connected_port()->get_name() << ");" << endl;
    }
  }
}

/**
 * \brief Hook called when displaying progress bar
 */
void unisim_module::show_progress()
{
}

/**
 * \brief  Hook for setting the maximum for the progress bar
 */ 
void unisim_module::set_progress_max(time_t start, uint64_t max)
{
}

/**
 * \brief Returns the rank of the module for drawing purpose.
 *
 * The rank specifies which module should be placed to the left / right
 * of which module. M1.rank < M2.rank => M1 is to the left of M2.
 *
 * Default rank of any module is -100. get_rank() should be overloaded to
 * effectively set ranks. The basic algorithm to set rank should be
 * something like:
 *  - cpu => rank = 0
 *  - other module => rank = (rank of module connected to a left port) + 1
 */
int unisim_module::get_rank()
{ return -100;
}
