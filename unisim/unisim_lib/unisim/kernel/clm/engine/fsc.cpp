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
                    fsc.cpp  - FraternitéSysC source code
                             -------------------
    begin                : Thu Apr 3 2003
    author               : Gilles Mouchard, Daniel Gracia Pérez
    email                : mouchard@lri.fr, gilles.mouchard@cea.fr
                           gracia@lri.fr
 ***************************************************************************/

#include "fsc.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

bool unisim_terminated = false;

/**
 * \brief End the simulation at the end of current cycle
 */
void terminate_now()
{ unisim_terminated = true;
}

fsc_simcontext *simcontext = 0;

const int SIGNALS_TO_UPDATE_STEP = 16;

fsc_module *current_user_module = 0;

/*************************** fsc_object ********************************/

/**
 * \brief Creates a new fsc_object
 */
fsc_object::fsc_object(const char *name)
{ id = -1;
  if(name)
  { object_name = new char[strlen(name) + 1];
    strcpy(object_name, name);
  }
  else
  { object_name = 0;
  }
}

/**
 * \brief Destructor of the fsc_object class
 */
fsc_object::~fsc_object()
{ delete[] object_name;
}

/**
 * \brief Set the ID of the object
 */
void fsc_object::SetId(int id)
{ this->id = id;
  if(!object_name)
  { SetName("#%d", id);
  }
}

/**
 * \brief Returns the ID of the object
 */
int fsc_object::GetId()
{ return id;
}

/**
 * \brief Set the name of the object
 */
void fsc_object::SetName(const char *format, ...)
{
  va_list args;
  char buffer[256];

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if(object_name)
    delete[] object_name;

  object_name = new char [strlen(buffer) + 1];
  strcpy(object_name, buffer);
}

/**
 * \brief Set the name of the object
 */
void fsc_object::SetName(const string& name)
{
  if(object_name)
    delete[] object_name;
  object_name = new char [name.length() + 1];
  strcpy(object_name, name.c_str());
}

/**
 * \brief Returns the object name
 */
const char *fsc_object::name() const
{
  return object_name;
}

/**
 * \brief Returns the object name
 */
const char *fsc_object::Name() const
{
  return object_name;
}

/************************* fsc_signal_base *****************************/

int fsc_signal_base::next_id = 0;
List<fsc_signal_base> fsc_signal_base::signal_list;

/**
 * \brief Creates a new fsc_signal_base
 */
fsc_signal_base::fsc_signal_base(fsc_signal_type type, const char *name) : fsc_object(name)
{
  SetId(next_id++);
#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cout << "FastSysC: Creating signal " << Name() << endl;
  cout.flush();
#endif

  simc = simcontext;
  signal_type = type;
  read_buf = 0;
  to_update = false;
  sensitive_process_table = 0;
  last_of_sensitive_processes = 0;
  if(simcontext)
    simcontext->Add(this);
  signal_list << this;
}

/**
 * \brief Destructor of the fsc_signal_base class
 */
fsc_signal_base::~fsc_signal_base()
{
  if(sensitive_process_table)
    delete[] sensitive_process_table;
}

/**
 * \brief Add a new process to the process list
 */
void fsc_signal_base::Bind(fsc_process *process)
{
  process_list << process;
}

/**
 * \brief Add a new process to the process list sensitive to raising edge
 */
void fsc_signal_base::BindPos(fsc_process *process)
{
  clk_pos_process_list << process;
}

/**
 * \brief Add a new process to the process list sensitive to falling edge
 */
void fsc_signal_base::BindNeg(fsc_process *process)
{
  clk_neg_process_list << process;
}

/**
 * \brief Returns the signal type of the signal
 */
fsc_signal_type fsc_signal_base::GetSignalType()
{
  return signal_type;
}

/**
 * \brief Buils the process table
 */
void fsc_signal_base::BuildProcessTable()
{
  ListPointer<fsc_process> process;
  if(process_list.Size() > 0)
  {
    sensitive_process_table = new fsc_process_ptr[process_list.Size()];
    fsc_process **sensitive_process = sensitive_process_table;
    for(process = process_list.Begin(); process; process++)
    {
      *sensitive_process++ = process;
    }
    last_of_sensitive_processes = sensitive_process - 1;

  }
}

/************************* fsc_prim_port ***********************************/

int fsc_prim_port::next_id = 0;
List<fsc_prim_port> fsc_prim_port::port_list;

/**
 * \brief Creates a new fsc_prim_port
 */
fsc_prim_port::fsc_prim_port(fsc_prim_port_type type, const char *name) : fsc_object(name)
{
  SetId(next_id++);
  port_type = type;
  alias = 0;
  signal = 0;
  mod = current_user_module;
  port_list << this;
  mod->Add(this);
}

/**
 * \brief Returns the type of the port
 */
fsc_prim_port_type fsc_prim_port::GetPortType()
{
  return port_type;
}

/**
 * Connect a signal to the port
 */
void fsc_prim_port::Bind(fsc_signal_base *signal)
{
  if(this->signal && this->signal != signal)
  {
    cerr << "ERROR: Port " << Name() << " of module " << mod->name() << " is already bound to signal " << this->signal->name() << "!" << endl;
    exit(1);
  }
  // cerr << mod->name() << "." << Name()  << " bound to " << signal->name() << endl;

  this->signal = signal;
#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FastSysC: Port " << Name() << " of module " << mod->name() << " is bound to signal " << this->signal->name() << endl;
#endif
}

/**
 * \brief Connect a new process to the port
 */
void fsc_prim_port::Bind(fsc_process *process)
{
  process_list << process;
}

/**
 * \brief Connect a new process to the raising edge of the port
 */
void fsc_prim_port::BindPos(fsc_process *process)
{
  clk_pos_process_list << process;
}

/**
 * \brief Connect a new process to the falling edge of the port
 */
void fsc_prim_port::BindNeg(fsc_process *process)
{
  clk_neg_process_list << process;
}

/**
 * \brief Connect a port to a port (aliasing)
 */
void fsc_prim_port::Bind(fsc_prim_port *port)
{
  if(alias)
  {
    cerr << "ERROR: Port " << Name() << " of module \"" << mod->name() << "\" is already bound to a port !" << endl;
    exit(1);
  }
  alias = port;
#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FastSysC: Port " << Name() << " of module " << mod->name() << " is bound to port " << alias->Name() << " of module " << alias->mod->name() << endl;
#endif
}

/**
 * \brief Add a new port to the dependency list
 */
void fsc_prim_port::Dependency(fsc_prim_port *port)
{
  if(port->port_type != FSC_OUT)
  {
    cerr << "ERROR: Can't set a dependency between \"" << Name() << "\" and \"" << port->Name() << "\" !" << endl;
    abort();
  }
  dependencies << port;
}



/*************************** inclock *******************************/

/**
 * \brief Creates a new inclock 
 */
inclock::inclock() : fsc_prim_port(FSC_IN_CLK)
{}

/**
 * \brief Bind the clock port to a clock signal
 */
void inclock::operator () (fsc_clock& clock)
{ Bind(&clock);
}

/****************************** fsc_clock *****************************/

/**
 * \brief Creates a new clock signal
 */
fsc_clock::fsc_clock() : fsc_signal_base(FSC_CLOCK)
{ value = true;
}

/**
 * \brief Set the clock signal value
 */
fsc_clock& fsc_clock::operator = (bool value)
{ this->value = value;
  return *this;
}

/************************* fsc_module *********************************/

int fsc_module::next_id = 0;
List<fsc_module> fsc_module::module_list;

/**
 * \brief Creates a new fsc_module
 */
fsc_module::fsc_module(const string& name) : fsc_object(name.c_str()), sensitive(this), sensitive_pos(this), sensitive_neg(this)
{
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug)
    cout << "FastSysC: Creating module \"" << name << "\"" << endl;
#endif

  fsc_prim_port::next_id = 0;
  SetId(next_id++);
  module_list << this;
  current_user_module = this;
}

/**
 * \brief Creates a new fsc_module
 */
fsc_module::fsc_module(const char *name) : fsc_object(name), sensitive(this), sensitive_pos(this), sensitive_neg(this)
{
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug)
    cout << "FastSysC: Creating module \"" << name << "\"" << endl;
#endif

  fsc_prim_port::next_id = 0;
  SetId(next_id++);
  module_list << this;
  current_user_module = this;
}

/**
 * \brief Destructor of the fsc_module class
 */
fsc_module::~fsc_module()
{}

/**
 * \brief Declares a new process in the module
 */
void fsc_module::declare_process(fsc_method method, const char *name)
{
  current_process = new fsc_process(this, method, name);
  process_list << current_process;
}

/**
 * \brief Set the current process of the module.
 *
 * Reuse an existing process with the same name, or creates a new one if none exists
 */
void fsc_module::set_process(fsc_method method, const char *name)
{
  ListPointer<fsc_process> process = NULL;
  //Look into the list of processes for a proces with the same name
  for(process = process_list.Begin(); process; process++)
  {
    if(strcmp(name,(*process).name())==0)
    {
      break;
    }
  }
  if(process)
  { // Such a process already exists, just point the current process to the existing one
    current_process = process;
  }
  else
  { // Such a process does not exists, create a new one
    declare_process(method,name);
  }
}

/**
 * \brief Adds a new port to the module port list
 */
void fsc_module::Add(fsc_prim_port *port)
{
#ifdef DEBUG_FSC
  if(fsc_simcontext::debug)
    cerr << "FastSysC: registering Port " << port->Name() << " into module " << name() << endl;
#endif

  port_list << port;
}

/* values to signify color used for transparent GIF color */

#define		TRANSP_BACKGROUND	-3	/* use background of figure as transp color */
#define		TRANSP_NONE		-2	/* no transp color */

/* DEFAULT is used for many things - font, color etc */

#define		DEFAULT		      (-1)

#define		SOLID_LINE		0
#define		DASH_LINE		1
#define		DOTTED_LINE		2
#define		DASH_DOT_LINE		3
#define		DASH_2_DOTS_LINE	4
#define		DASH_3_DOTS_LINE	5

#define		RUBBER_LINE		11
#define		PANEL_LINE		12

#define		BLACK			0
#define		BLUE			1
#define		GREEN			2
#define		CYAN			3
#define		RED			4
#define		MAGENTA			5
#define		YELLOW			6
#define		WHITE			7

#define DEF_ARROW_WID 4
#define DEF_ARROW_HT 8

#define					T_ELLIPSE_BY_RAD	1
#define					T_ELLIPSE_BY_DIA	2
#define					T_CIRCLE_BY_RAD		3
#define					T_CIRCLE_BY_DIA		4

#define					UNFILLED	-1

#define					T_OPEN_ARC		0
#define					T_PIE_WEDGE_ARC		1

#define					CAP_BUTT	0
#define					CAP_ROUND	1
#define					CAP_PROJECT	2


#define		CLOSED_PATH		0
#define		OPEN_PATH		1
#define		DEF_BOXRADIUS		7
#define		DEF_DASHLENGTH		4
#define		DEF_DOTGAP		3

#define					T_POLYLINE	1
#define					T_BOX		2
#define					T_POLYGON	3
#define					T_ARC_BOX	4
#define					T_PICTURE	5

#define					JOIN_MITER	0
#define					JOIN_ROUND	1
#define					JOIN_BEVEL	2

#define					T_LEFT_JUSTIFIED	0
#define					T_CENTER_JUSTIFIED	1
#define					T_RIGHT_JUSTIFIED	2

#define					RIGID_TEXT		1
#define					SPECIAL_TEXT		2
#define					PSFONT_TEXT		4
#define					HIDDEN_TEXT		8



#define S_SPLINE_ANGULAR 0.0
#define S_SPLINE_APPROX 1.0
#define S_SPLINE_INTERP (-1.0)

#define					T_OPEN_APPROX	0
#define					T_CLOSED_APPROX 1
#define					T_OPEN_INTERP	2
#define					T_CLOSED_INTERP 3
#define                                 T_OPEN_XSPLINE  4
#define                                 T_CLOSED_XSPLINE  5

/* separate the "type" and the "style" from the cur_arrowtype */
#define		ARROW_TYPE(x)	((x)==0? 0 : ((x)+1)/2)
#define		ARROW_STYLE(x)	((x)==0? 0 : ((x)+1)%2)

#define		ARROW_SIZE	sizeof(struct f_arrow)
#define		POINT_SIZE	sizeof(struct f_point)
#define		CONTROL_SIZE	sizeof(struct f_shape)
#define		ELLOBJ_SIZE	sizeof(struct f_ellipse)
#define		ARCOBJ_SIZE	sizeof(struct f_arc)
#define		LINOBJ_SIZE	sizeof(struct f_line)
#define		TEXOBJ_SIZE	sizeof(struct f_text)
#define		SPLOBJ_SIZE	sizeof(struct f_spline)
#define		COMOBJ_SIZE	sizeof(struct f_compound)
#define		PIC_SIZE	sizeof(struct f_pic)
#define		LINKINFO_SIZE	sizeof(struct f_linkinfo)

/**********************  object codes  **********************/

#define		O_COLOR_DEF	0
#define		O_ELLIPSE	1
#define		O_POLYLINE	2
#define		O_SPLINE	3
#define		O_TEXT		4
#define		O_ARC		5
#define		O_COMPOUND	6
#define		O_END_COMPOUND	-O_COMPOUND
#define		O_ALL_OBJECT	99

/********************* object masks for update  ************************/

#define M_NONE			0x0000
#define M_POLYLINE_POLYGON	0x0001
#define M_POLYLINE_LINE		0x0002
#define M_POLYLINE_BOX		0x0004	/* includes ARCBOX */
#define M_SPLINE_O_APPROX	0x0008
#define M_SPLINE_C_APPROX	0x0010
#define M_SPLINE_O_INTERP	0x0020
#define M_SPLINE_C_INTERP	0x0040
#define M_SPLINE_O_XSPLINE      0x0080
#define M_SPLINE_C_XSPLINE      0x0100
#define M_TEXT_NORMAL		0x0200
#define M_TEXT_HIDDEN		0x0400
#define M_ARC			0x0800
#define M_ELLIPSE		0x1000
#define M_COMPOUND		0x2000

#define M_TEXT		(M_TEXT_HIDDEN | M_TEXT_NORMAL)
#define M_SPLINE_O	(M_SPLINE_O_APPROX | M_SPLINE_O_INTERP | M_SPLINE_O_XSPLINE)
#define M_SPLINE_C	(M_SPLINE_C_APPROX | M_SPLINE_C_INTERP | M_SPLINE_C_XSPLINE)
#define M_SPLINE_APPROX (M_SPLINE_O_APPROX | M_SPLINE_C_APPROX)
#define M_SPLINE_INTERP (M_SPLINE_O_INTERP | M_SPLINE_C_INTERP)
#define M_SPLINE_XSPLINE (M_SPLINE_O_XSPLINE | M_SPLINE_C_XSPLINE)
#define M_SPLINE	(M_SPLINE_APPROX | M_SPLINE_INTERP | M_SPLINE_XSPLINE)
#define M_POLYLINE	(M_POLYLINE_LINE | M_POLYLINE_POLYGON | M_POLYLINE_BOX)
#define M_VARPTS_OBJECT (M_POLYLINE_LINE | M_POLYLINE_POLYGON | M_SPLINE)
#define M_OPEN_OBJECT	(M_POLYLINE_LINE | M_SPLINE_O | M_ARC)
#define M_ROTATE_ANGLE	(M_VARPTS_OBJECT | M_ARC | M_TEXT | M_COMPOUND | M_ELLIPSE)
#define M_OBJECT	(M_ELLIPSE | M_POLYLINE | M_SPLINE | M_TEXT | M_ARC)
#define M_NO_TEXT	(M_ELLIPSE | M_POLYLINE | M_SPLINE | M_COMPOUND | M_ARC)
#define M_ALL		(M_OBJECT | M_COMPOUND)










void DrawBox(ostream& os, int x1, int y1, int x2, int y2)
{
  os << O_POLYLINE << " "; // polyline object code
  os << T_BOX << " "; // subtype : box
  os << SOLID_LINE << " "; // line style
  os << 1 << " "; // thickness (1/80 inch)
  os << 0 << " "; // pen color
  os << 7 << " "; // fill color
  os << 50 << " "; // depth
  os << DEFAULT << " "; // pen style
  os << DEFAULT << " "; // area style
  os << (double) 0.0 << " "; // style val (1/80 inch)
  os << 0 << " "; // join style
  os << 0 << " "; // cap style
  os << DEFAULT << " "; // radius
  os << 0 << " "; // forward arrow
  os << 0 << " "; // backward arrow
  os << 5 << endl; // number of points in line
  os << "\t " << x1 << " " << y1 << " ";
  os << x2 << " " << y1 << " ";
  os << x2 << " " << y2 << " ";
  os << x1 << " " << y2 << " ";
  os << x1 << " " << y1 << endl;
}

void DrawPolyline(ostream& os, int npoints, int x[], int y[])
{
  os << O_POLYLINE << " "; // polyline object code
  os << T_POLYLINE << " "; // subtype : polyline
  os << SOLID_LINE << " "; // line style
  os << 1 << " "; // thickness (1/80 inch)
  os << 0 << " "; // pen color
  os << 7 << " "; // fill color
  os << 50 << " "; // depth
  os << DEFAULT << " "; // pen style
  os << DEFAULT << " "; // area style
  os << (double) 0.0 << " "; // style val (1/80 inch)
  os << 0 << " "; // join style
  os << 0 << " "; // cap style
  os << DEFAULT << " "; // radius
  os << 0 << " "; // forward arrow
  os << 0 << " "; // backward arrow
  os << npoints << endl; // number of points in line
  os << "\t";
  int i;
  for(i = 0; i < npoints; i++)
  {
    os << " " << x[i] << " " << y[i];
  }
  os << endl;
}

void DrawText(ostream& os, int x, int y, int justification, int fontsize, const char *s)
{
  os << O_TEXT << " "; // text object
  os << justification << " "; // sub type
  os << 0 << " "; // color
  os << 50 << " "; // depth
  os << DEFAULT << " "; // pen style
  os << 0 << " "; // font: Times Roman
  os << fontsize << " "; // font size in points
  os << 0 << " "; // angle
  os << RIGID_TEXT << " "; // font flag
  os << 200 << " "; // height
  os << 100 << " "; // length
  os << x << " "; // x
  os << y << " "; // y
  os << s << "\\001" << endl;
}

void DrawInPort(ostream& os, int x, int y, const char *name)
{
  DrawText(os, x + 250, y + 150, T_LEFT_JUSTIFIED, 12, name);
  DrawBox(os, x, y, x + 200, y + 200);
  int px[3], py[3];
  px[0] = x;
  py[0] = y;
  px[1] = x + 200;
  py[1] = y + 100;
  px[2] = x;
  py[2] = y + 200;
  DrawPolyline(os, 3, px, py);
}

void DrawClockPort(ostream& os, int x, int y, const char *name)
{
  DrawText(os, x + 250, y + 150, T_LEFT_JUSTIFIED, 12, name);
  DrawBox(os, x, y, x + 200, y + 200);
  int px[6], py[6];
  px[0] = x;
  py[0] = y;
  px[1] = x + 200;
  py[1] = y + 100;
  px[2] = x;
  py[2] = y + 200;
  DrawPolyline(os, 3, px, py);
  px[0] = x - 200;
  py[0] = y + 150;
  px[1] = x - 150;
  py[1] = y + 150;
  px[2] = x - 150;
  py[2] = y + 50;
  px[3] = x - 100;
  py[3] = y + 50;
  px[4] = x - 100;
  py[4] = y + 150;
  px[5] = x - 50;
  py[5] = y + 150;
  DrawPolyline(os, 6, px, py);
}

void DrawOutPort(ostream& os, int x, int y, const char *name)
{
  DrawText(os, x - 250, y + 150, T_RIGHT_JUSTIFIED, 12, name);
  DrawBox(os, x - 200, y, x, y + 200);
  int px[3], py[3];
  px[0] = x - 200;
  py[0] = y;
  px[1] = x;
  py[1] = y + 100;
  px[2] = x - 200;
  py[2] = y + 200;
  DrawPolyline(os, 3, px, py);
}


void fsc_module::DrawFig(ostream& os)
{
  os << "#FIG 3.2" << endl;
  os << "Landscape" << endl;
  os << "Center" << endl;
  os << "Metric" << endl;
  os << "A4" << endl;
  os << "100.0" << endl;
  os << "Single" << endl;
  os << "-2" << endl;
  os << "1200 2" << endl;

  ListPointer<fsc_prim_port> port;
  int n_inports = 0;
  int n_outports = 0;

  for(port = port_list.Begin(); port; port++)
  {
    switch(port->port_type)
    {
        case FSC_IN:
        n_inports++;
        break;

        case FSC_OUT:
        n_outports++;
        break;

        case FSC_IN_CLK:
        n_inports++;
        break;
    }
  }

  int n_max = n_inports;
  if(n_max < n_outports)
    n_max = n_outports;

  int height = n_max * 300 + 100;
  int width = height;
  if(width > 6000)
    width = 6000;
  if(width < 2000)
    width = 2000;
  int y_in = (height - (n_inports * 300)) / 2 + 50;
  int y_out = (height - (n_outports * 300)) / 2 + 50;

  for(port = port_list.Begin(); port; port++)
  {
    switch(port->port_type)
    {
        case FSC_IN:
        DrawInPort(os, 0, y_in, port->Name());
        y_in += 300;
        break;

        case FSC_OUT:
        DrawOutPort(os, width, y_out, port->Name());
        y_out += 300;
        break;

        case FSC_IN_CLK:
        DrawClockPort(os, 0, y_in, port->Name());
        y_in += 300;
        break;
    }
  }

  DrawBox(os, 0, 0, width, height);
  DrawText(os, width / 2, height / 2, T_CENTER_JUSTIFIED, 16, name());
}

void fsc_module::GenerateDocSkeleton(ostream& os)
{
  os << "\\begin{figure}[h!tb]" << endl;
  os << "\\begin{center}" << endl;
  os << "\\includegraphics[width=8.33cm]{" << name() << ".eps}" << endl;
  os << "\\end{center}" << endl;
  os << "\\caption{" << name() << " input/output ports.}" << endl;
  os << "\\label{" << name() << "}" << endl;
  os << "\\end{figure}" << endl;
  ListPointer<fsc_prim_port> port;
  bool has_input_ports = false;
  bool has_output_ports = false;
  for(port = port_list.Begin(); port; port++)
  {
    if(port->port_type == FSC_IN || port->port_type == FSC_IN_CLK)
    {
      has_input_ports = true;
      break;
    }
  }
  for(port = port_list.Begin(); port; port++)
  {
    if(port->port_type == FSC_OUT)
    {
      has_output_ports = true;
      break;
    }
  }
  if(has_input_ports)
  {
    os << "Input Ports:" << endl;
    os << "\\begin{itemize}" << endl;
    for(port = port_list.Begin(); port; port++)
    {
      if(port->port_type == FSC_IN || port->port_type == FSC_IN_CLK)
      {
        os << "\\item " << port->name() << ": insert here a description" << endl;
      }
    }
    os << "\\end{itemize}" << endl;
  }
  if(has_output_ports)
  {
    os << "Output Ports:" << endl;
    os << "\\begin{itemize}" << endl;
    for(port = port_list.Begin(); port; port++)
    {
      if(port->port_type == FSC_OUT)
      {
        os << "\\item " << port->name() << ": insert here a description" << endl;
      }
    }
    os << "\\end{itemize}" << endl;
  }
}

/************************* fsc_sensitive ******************************/

/**
 * \brief Creates a new sensitivity list
 */
fsc_sensitive::fsc_sensitive(fsc_module *mod)
{
  this->mod = mod;
}

/**
 * \brief Add a new port to the sensitivity list
 */
fsc_sensitive& fsc_sensitive::operator << (fsc_prim_port& port)
{
  mod->current_process->Sensitive(&port);
  return *this;
}

/************************* fsc_sensitive_pos ******************************/

/**
 * \brief Creates a new raising edge sensitivity list
 */
fsc_sensitive_pos::fsc_sensitive_pos(fsc_module *mod)
{
  this->mod = mod;
}

/**
 * \brief Add a new port to the sensitivity list
 */
fsc_sensitive_pos& fsc_sensitive_pos::operator << (fsc_prim_port& port)
{
  mod->current_process->SensitivePos(&port);
  return *this;
}

/************************* fsc_sensitive_neg ******************************/

/**
 * \brief Creates a new falling edge sensitivity list
 */
fsc_sensitive_neg::fsc_sensitive_neg(fsc_module *mod)
{
  this->mod = mod;
}

/**
 * \brief Add a new port to the sensitivity list
 */
fsc_sensitive_neg& fsc_sensitive_neg::operator << (fsc_prim_port& port)
{
  mod->current_process->SensitiveNeg(&port);
  return *this;
}

/************************** fsc_process *******************************/

int fsc_process::next_id = 0;
List<fsc_process> fsc_process::process_list;

/**
 * \brief Creates a new fsc_process
 */
fsc_process::fsc_process(fsc_module *mod, fsc_method method, const char *name) : fsc_object(name)
{
  SetId(next_id++);
  this->mod = mod;
  this->method = method;
  to_execute = false;
  time = 0;
  process_list << this;
#ifdef FSC

  if(fsc_simcontext::debug)
    cout << "FastSysC: Creating Process " << mod->Name() << "::" << Name() << endl;
  cout.flush();
#endif
}

/**
 * \brief Add a port to the connected port list
 */
void fsc_process::Sensitive(fsc_prim_port *port)
{
  port_list << port;
  port->Bind(this);
}

/**
 * \brief Add a port to the raising edge connected port list
 */
void fsc_process::SensitivePos(fsc_prim_port *port)
{
  clk_pos_port_list << port;
  port->BindPos(this);
}

/**
 * \brief Add a port to the falling edge connected port list
 */
void fsc_process::SensitiveNeg(fsc_prim_port *port)
{
  clk_neg_port_list << port;
  port->BindNeg(this);
}

/**
 * \brief Add process to the linked process list
 */
void fsc_process::Bind(fsc_signal_base *signal)
{
  signal_list << signal;
}

/**
 * \brief Add process to the raising edge linked process list
 */
void fsc_process::BindPos(fsc_signal_base *clk_signal)
{
  clk_pos_signal_list << clk_signal;
}

/**
 * \brief Add process to the falling edge linked process list
 */
void fsc_process::BindNeg(fsc_signal_base *clk_signal)
{
  clk_neg_signal_list << clk_signal;
}

/**
 * \brief Pretty printer of the fsc_process class
 */
ostream& operator << (ostream& os, const fsc_process& process)
{
  os << process.mod->Name() << "::" << process.Name();
  return os;
}

/**
 * \brief Returns true if the process is sensitive to clock edge
 */
bool fsc_process::IsSensitiveToClockEdge(fsc_clock_edge edge)
{
  bool sensitive_to_clock_edge = false;

  switch(edge)
  {
      case POS_EDGE:
      sensitive_to_clock_edge = !clk_pos_signal_list.Empty();
      break;

      case NEG_EDGE:
      sensitive_to_clock_edge = !clk_neg_signal_list.Empty();
      break;

      default:
      abort();
  }
  ListPointer<fsc_signal_base> signal;
  for(signal = signal_list.Begin(); signal; signal++)
  {
    if(signal->GetSignalType() == FSC_CLOCK)
    {
      sensitive_to_clock_edge = true;
      break;
    }
  }

  return sensitive_to_clock_edge;
}


/************************** fsc_simcontext ****************************/

bool fsc_simcontext::debug = false;

/**
 * \brief Creates a new fsc_simcontext 
 */
fsc_simcontext::fsc_simcontext()
{
  simcontext = this;
  module_table = 0;
  signal_table = 0;
  process_table = 0;
  initialized = false;
  time_stamp = 0;//0.0;
  current_module = 0;
  stop = true;
  sensitive_clock_process_table[POS_EDGE] = 0;
  sensitive_clock_process_table[NEG_EDGE] = 0;
  clock_signal_table = 0;
  last_of_clock_signals = 0;
  processes_to_execute = 0;
  clock_edge = NEG_EDGE; //POS_EDGE;
  debug_start = 0xffffffffffffffffULL; //HUGE_VAL;
  debug_stop = 0xffffffffffffffffULL; //HUGE_VAL;

  sizeof_signals_to_update_table = fsc_signal_base::signal_list.Size() > 64 ? fsc_signal_base::signal_list.Size() : 64;
  signals_to_update = new fsc_signal_ptr[sizeof_signals_to_update_table];
  signal_to_update = signals_to_update - 1;

  ListPointer<fsc_signal_base> signal;

  /* Add all signals that were previously defined as global (before fsc_main) */
  for(signal = fsc_signal_base::signal_list.Begin(); signal; signal++)
  {
    if(!signal->simc)
      Add(signal);
  }
}

/**
 * \brief Destructor of the fsc_simcontext class
 */
fsc_simcontext::~fsc_simcontext()
{
  ListPointer<fsc_module> mod;
  ListPointer<fsc_process> process;

  for(mod = fsc_module::module_list.Begin(); mod;)
  {
    //delete mod;
    fsc_module::module_list.Remove(mod);
  }

  for(process = fsc_process::process_list.Begin(); process;)
  {
    delete process;
    fsc_process::process_list.Remove(process);
  }

  delete[] signals_to_update;

  if(module_table)
    delete[] module_table;
  if(process_table)
    delete[] process_table;
  if(signal_table)
    delete[] signal_table;

  if(sensitive_clock_process_table[POS_EDGE])
    delete[] sensitive_clock_process_table[POS_EDGE];
  if(sensitive_clock_process_table[NEG_EDGE])
    delete[] sensitive_clock_process_table[NEG_EDGE];

  if(clock_signal_table)
    delete[] clock_signal_table;

  if(processes_to_execute)
    delete[] processes_to_execute;

#ifdef FFSC_STAT

  if(ncycles)
    delete[] ncycles;
#endif

}

/**
 * \brief Add a new signal
 */
void fsc_simcontext::Add(fsc_signal_base *signal)
{
  signal->simc = this;
  int nsignals = fsc_signal_base::signal_list.Size();

  if(nsignals > sizeof_signals_to_update_table)
  {
    if(signal_to_update >= signals_to_update)
    {
      fsc_signal_base **old_signals_to_update = signals_to_update;
      signals_to_update = new fsc_signal_ptr[sizeof_signals_to_update_table + SIGNALS_TO_UPDATE_STEP];
      memcpy(signals_to_update, old_signals_to_update, sizeof_signals_to_update_table * sizeof(fsc_signal_base *));
      signal_to_update = signal_to_update - old_signals_to_update + signals_to_update;
      sizeof_signals_to_update_table += SIGNALS_TO_UPDATE_STEP;
      delete[] old_signals_to_update;
    }
    else
    {
      delete[] signals_to_update;
      sizeof_signals_to_update_table += SIGNALS_TO_UPDATE_STEP;
      signals_to_update = new fsc_signal_ptr[sizeof_signals_to_update_table];
      signal_to_update = signals_to_update - 1;
    }
  }
}

int indent_size = 0;

/*
static void indent(ostream& os)
{
	int i;
	for(i = 0; i < indent_size; i++)
	{
		os << "\t";
	}
}

static void open_brace(ostream& os)
{
	indent(os);
	os << "{";
	indent_size += 1;
}

static void close_brace(ostream& os)
{
	indent_size -= 1;
	indent(os);
	os << "}";
}
*/


/**
 * \brief Initialize a new fsc_simcontext
 */
void fsc_simcontext::Initialize()
{
  if(initialized)
    return;

#ifdef DEBUG_FSC

  fsc_simcontext::debug = time_stamp >= debug_start && time_stamp <= debug_stop;
#endif

  nwakeups = 0;

  initialized = true;
  ListPointer<fsc_module> mod;
  ListPointer<fsc_signal_base> signal;
  ListPointer<fsc_prim_port> port;
  ListPointer<fsc_process> process;

  module_table = new fsc_module_ptr[fsc_module::module_list.Size()];
  signal_table = new fsc_signal_ptr[fsc_signal_base::signal_list.Size()];
  process_table = new fsc_process_ptr[fsc_process::process_list.Size()];

  for(signal = fsc_signal_base::signal_list.Begin(); signal; signal++)
  {
    signal_table[signal->GetId()] = signal;
    signal->to_update = false;
  }

  /* Bind port to port */
  for(mod = fsc_module::module_list.Begin(); mod; mod++)
  {
    module_table[mod->GetId()] = mod;

    for(port = mod->port_list.Begin(); port; port++)
    { fsc_prim_port *alias, *lastalias;

      if(!port->signal)
      { alias = port->alias;
        lastalias = alias;

        while(alias)
        { lastalias = alias;
          alias = alias->alias;
        }
        if(lastalias)
        { port->signal = lastalias->signal;
#ifdef DEBUG_FSC
          if(fsc_simcontext::debug) cerr << "FastSysC: Port " << port->Name() << " of module " << port->mod->name() << " is bound to signal " << port->signal->name() << endl;
#endif
        }
      }
    }
  }

  /* Bind signals to processes */
  for(process = fsc_process::process_list.Begin(); process; process++)
  { process_table[process->GetId()] = process;
    for(port = process->clk_pos_port_list.Begin(); port; port++)
    { if(!port->signal)
      { cerr << "ERROR: Clock Port " << port->Name() << " of module \"" << process->mod->Name() << "\" is unbound !" << endl;
        exit(1);
      }
      port->signal->BindPos(process);
      process->BindPos(port->signal);
    }
  }

  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    for(port = process->clk_neg_port_list.Begin(); port; port++)
    {
      if(!port->signal)
      {
        cerr << "ERROR: Clock Port " << port->Name() << " of module \"" << process->mod->Name() << "\" is unbound !" << endl;
        exit(1);
      }
      port->signal->BindNeg(process);
      process->BindNeg(port->signal);
    }
  }

  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    for(port = process->port_list.Begin(); port; port++)
    {
      if(port->port_type == FSC_OUT)
      {
        cerr << "ERROR: process " << process->Name() << " of module " << process->mod->name() << " can't be sensitive to output port " << port->name() << "!" << endl;
        exit(1);
      }
      if(!port->signal)
      {
        cerr << "ERROR: Port " << port->Name() << " of module \"" << process->mod->Name() << "\" is unbound !" << endl;
        exit(1);
      }
      port->signal->Bind(process);
      process->Bind(port->signal);
      process->to_execute = false;
    }
  }

/*

  THIS IS ALREADY CHECKED ON UNISIM_PORT WITH SUPPORT FOR ALIASING

  //Check that every sub-port is connected to a sub-signal
  for(port = fsc_prim_port::port_list.Begin(); port; port++)
  { if(!port->signal)
    { cerr << "WARNING: Port " << port->Name() << " of module \"" << port->mod->Name() << "\" is unbound !" << endl;
    }
  }
*/

  for(signal = fsc_signal_base::signal_list.Begin(); signal; signal++)
  { signal->BuildProcessTable();
  }

  int nsensitive_pos_processes = 0;
  int nsensitive_neg_processes = 0;
  for(process = fsc_process::process_list.Begin(); process; process++)
  { if(!process->clk_pos_signal_list.Empty()) nsensitive_pos_processes++;
    if(!process->clk_neg_signal_list.Empty()) nsensitive_neg_processes++;
  }

  fsc_process **sensitive_clock_process;

  sensitive_clock_process_table[POS_EDGE] = new fsc_process_ptr[nsensitive_pos_processes];
  sensitive_clock_process = sensitive_clock_process_table[POS_EDGE];
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    if(!process->clk_pos_signal_list.Empty())
    {
      *sensitive_clock_process = process;
      sensitive_clock_process++;
    }
  }
  last_of_sensitive_clock_processes[POS_EDGE] = sensitive_clock_process - 1;

  sensitive_clock_process_table[NEG_EDGE] = new fsc_process_ptr[nsensitive_neg_processes];
  sensitive_clock_process = sensitive_clock_process_table[NEG_EDGE];
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    if(!process->clk_neg_signal_list.Empty())
    {
      *sensitive_clock_process = process;
      sensitive_clock_process++;
    }
  }
  last_of_sensitive_clock_processes[NEG_EDGE] = sensitive_clock_process - 1;

  int nclocks = 0;
  for(signal = fsc_signal_base::signal_list.Begin(); signal; signal++)
  {
    if(signal->GetSignalType() == FSC_CLOCK)
      nclocks++;
  }

  if(nclocks > 0)
  {
    clock_signal_table = new fsc_clock_ptr[nclocks];
    fsc_clock **clock = clock_signal_table;

    for(signal = fsc_signal_base::signal_list.Begin(); signal; signal++)
    {
      if(signal->GetSignalType() == FSC_CLOCK)
      {
        *clock = static_cast<fsc_clock *>((fsc_signal_base *) signal);
        clock++;
      }
    }
    last_of_clock_signals = clock - 1;
  }

  nprocesses = fsc_process::process_list.Size();

  processes_to_execute = new fsc_process_ptr[nprocesses];

#ifdef FFSC_STAT

  int i;
  ncycles = new int[nprocesses * 32];
  for(i = 0; i < nprocesses * 32; i++)
  {
    ncycles[i] = 0;
  }
#endif


  /* First propagate signals if user has updated signal before full initialization */
  PropagateSignals();
}



/**
 * \brief Propagate the signals
 */
void fsc_simcontext::PropagateSignals()
{
  if(signal_to_update >= signals_to_update)
  {
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug)
      cerr << "FastSysC: Propagating signals" << endl;
#endif

    do
    {
      /* update the signals */
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
        cerr << "FastSysC: updating signal " << (*signal_to_update)->Name() << endl;
#endif

      (*signal_to_update)->read_buf ^= 1;
      (*signal_to_update)->to_update = 0;
    }
    while(--signal_to_update >= signals_to_update);
  }
}

#if !defined(STATICSCHED)
void fsc_start(uint64_t t)
{
  fsc_simcontext *simc = simcontext;
  if(t < 0)
    t = 0xffffffffffffffffULL; //HUGE_VAL;
  t = t + simc->time_stamp;
#ifdef DEBUG_FSC

  fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
  if(fsc_simcontext::debug)
    cerr << "FastSysC: Entering into fsc_start" << endl;
#endif

  simc->stop = false;
  if(!simc->initialized)
  {
    simc->Initialize();
  }

  fsc_process **processes_to_execute = simc->processes_to_execute;
  fsc_process **process_to_execute = simc->processes_to_execute - 1;
#ifdef FFSC_STAT

  int count_wakeup[simc->nprocesses];
  int nw = 0;
  int i;
  int c = 0;
  unsigned long long start;
  RDTSC(start);
#endif

  /* main simulation loop */
  do
  {
    fsc_clock **clock;
#ifdef FFSC_STAT

    memset(count_wakeup, 0, sizeof(count_wakeup));
#endif
#ifdef DEBUG_FSC

    fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
#endif

    /* update the clocks */
    clock = simc->last_of_clock_signals;
    if(clock)
    {
      do
      {
        (*clock)->value ^= true;
      }
      while(--clock >= simc->clock_signal_table);
    }

    simc->clock_edge = (fsc_clock_edge)((int) simc->clock_edge ^ 1);

#ifdef DEBUG_FSC

    if(fsc_simcontext::debug)
    {
      cerr << "FastSysC: simc->clock_edge = " << simc->clock_edge << endl;
      cerr << "FastSysC: =================== time stamp = " << simc->time_stamp << " ====================" << endl;
    }
#endif

    /* Call all FSC_METHOD process sensitive to the clock edge */
    if(simc->last_of_sensitive_clock_processes[simc->clock_edge] >= simc->sensitive_clock_process_table[simc->clock_edge])
    {
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
      {
        switch(simc->clock_edge)
        {
            case POS_EDGE:
            cerr << "FastSysC: Clock front edge" << endl;
            break;

            case NEG_EDGE:
            cerr << "FastSysC: Clock falling edge" << endl;
            break;

            default:
            abort();
        }
      }
#endif
      fsc_process **sensitive_clock_process = simc->last_of_sensitive_clock_processes[simc->clock_edge];
      do
      {
        fsc_process *process = *sensitive_clock_process;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
        cerr.flush();
#endif

        CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
        cerr.flush();
#endif

      }
      while(--sensitive_clock_process >= simc->sensitive_clock_process_table[simc->clock_edge]);
    }

    if(simc->signal_to_update >= simc->signals_to_update)
    {
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
        cerr << "FastSysC: Propagating signals" << endl;
#endif

      do
      {
        /* update the signals */
        do
        {
          fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

          signal_to_update->read_buf ^= 1;
          signal_to_update->to_update = 0;

          fsc_process **process = signal_to_update->last_of_sensitive_processes;
          if(process)
          {
            do
            {
#ifdef DEBUG_FSC
              if(fsc_simcontext::debug)
                cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

              if(!(*process)->to_execute)
              {
                *(++process_to_execute) = *process;
                (*process)->to_execute = true;
              }
            }
            while(--process >= signal_to_update->sensitive_process_table);
          }
        }
        while(--simc->signal_to_update >= simc->signals_to_update);

        /* execute all sensitive processes */
        if(process_to_execute >= processes_to_execute)
        {
          do
          {
            fsc_process *process = *process_to_execute;
#ifdef DEBUG_FSC

            if(fsc_simcontext::debug)
              cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
            cerr.flush();
#endif

            CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

            if(fsc_simcontext::debug)
              cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
            cerr.flush();
#endif

            process->to_execute = false;
#ifdef FFSC_STAT

            nw++;
            count_wakeup[process->GetId()]++;
#endif

          }
          while(--process_to_execute >= processes_to_execute);
        }
      }
      while(simc->signal_to_update >= simc->signals_to_update);
    }

#ifdef FFSC_STAT
    if(simc->clock_edge == POS_EDGE)
    {
      for(i = 0; i < simc->nprocesses; i++)
      {
        if(count_wakeup[i] >= 32)
          abort();
        simc->ncycles[i * 32 + count_wakeup[i]]++;
      }
    }
#endif
    /* increment the simulation time */
    if(simc->clock_edge)
      simc->time_stamp = simc->time_stamp + 1; //+ 1.0;
  }
  while(!simc->stop && simc->time_stamp < t);

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FastSysC: Returning from fsc_start" << endl;
#endif
#ifdef FFSC_STAT

  unsigned long long stop;
  RDTSC(stop);
  cout << "c = " << c << endl;
  cout << "nw = " << nw << endl;

  ListPointer<fsc_process> process;
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    if(!process->port_list.Empty())
    {
      cout << process->mod->Name() << "::" << process->Name();
      for(i = 0; i < 32; i++)
      {
        cout << " " << simc->ncycles[32 * process->GetId() + i];
      }
      cout << endl;
    }
  }
  unsigned long long total_time = stop - start;
  unsigned long long total_process_time = 0;
  for(process = fsc_process::process_list.Begin(); process; process++)
    total_process_time += process->time;
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    cout << process->mod->Name() << "::" << process->Name() << " : " << (((double) process->time * 100.0) / (double) total_process_time) << " %" << endl;
  }
  cout << "FastSysC scheduler : " << (((double)(total_time - total_process_time) * 100.0)/ (double) total_time) << " %" << endl;
#endif
}

void fsc_phase()
{
  fsc_simcontext *simc = simcontext;
#ifdef DEBUG_FSC

  fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
  if(fsc_simcontext::debug)
    cerr << "FastSysC: Entering into fsc_start" << endl;
#endif

  simc->stop = false;
  if(!simc->initialized)
  {
    simc->Initialize();
  }

  fsc_process **processes_to_execute = simc->processes_to_execute;
  fsc_process **process_to_execute = simc->processes_to_execute - 1;
#ifdef FFSC_STAT

  int count_wakeup[simc->nprocesses];
  int nw = 0;
  int i;
  int c = 0;
  unsigned long long start;
  RDTSC(start);
#endif

  /* main simulation loop */
  fsc_clock **clock;
#ifdef FFSC_STAT

  memset(count_wakeup, 0, sizeof(count_wakeup));
#endif
#ifdef DEBUG_FSC

  fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
#endif

  /* update the clocks */
  clock = simc->last_of_clock_signals;
  if(clock)
  {
    do
    {
      (*clock)->value ^= true;
    }
    while(--clock >= simc->clock_signal_table);
  }

  simc->clock_edge = (fsc_clock_edge)((int) simc->clock_edge ^ 1);

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
  {
    cerr << "FastSysC: simc->clock_edge = " << simc->clock_edge << endl;
    cerr << "FastSysC: =================== time stamp = " << simc->time_stamp << " ====================" << endl;
  }
#endif

  /* Call all FSC_METHOD process sensitive to the clock edge */
  if(simc->last_of_sensitive_clock_processes[simc->clock_edge] >= simc->sensitive_clock_process_table[simc->clock_edge])
  {
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug)
    {
      switch(simc->clock_edge)
      {
          case POS_EDGE:
          cerr << "FastSysC: Clock front edge" << endl;
          break;

          case NEG_EDGE:
          cerr << "FastSysC: Clock falling edge" << endl;
          break;

          default:
          abort();
      }
    }
#endif
    fsc_process **sensitive_clock_process = simc->last_of_sensitive_clock_processes[simc->clock_edge];
    do
    {
      fsc_process *process = *sensitive_clock_process;
#ifdef DEBUG_FSC

      if(fsc_simcontext::debug)
        cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
      cerr.flush();
#endif

      CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

      if(fsc_simcontext::debug)
        cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
      cerr.flush();
#endif

    }
    while(--sensitive_clock_process >= simc->sensitive_clock_process_table[simc->clock_edge]);
  }

  if(simc->signal_to_update >= simc->signals_to_update)
  {
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug)
      cerr << "FastSysC: Propagating signals" << endl;
#endif

    do
    {
      /* update the signals */
      do
      {
        fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

        signal_to_update->read_buf ^= 1;
        signal_to_update->to_update = 0;

        fsc_process **process = signal_to_update->last_of_sensitive_processes;
        if(process)
        {
          do
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

            if(!(*process)->to_execute)
            {
              *(++process_to_execute) = *process;
              (*process)->to_execute = true;
            }
          }
          while(--process >= signal_to_update->sensitive_process_table);
        }
      }
      while(--simc->signal_to_update >= simc->signals_to_update);

      /* execute all sensitive processes */
      if(process_to_execute >= processes_to_execute)
      {
        do
        {
          fsc_process *process = *process_to_execute;
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
          cerr.flush();
#endif

          CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
          cerr.flush();
#endif

          process->to_execute = false;
#ifdef FFSC_STAT

          nw++;
          count_wakeup[process->GetId()]++;
#endif

        }
        while(--process_to_execute >= processes_to_execute);
      }
    }
    while(simc->signal_to_update >= simc->signals_to_update);
  }

#ifdef FFSC_STAT
  if(simc->clock_edge == POS_EDGE)
  {
    for(i = 0; i < simc->nprocesses; i++)
    {
      if(count_wakeup[i] >= 32)
        abort();
      simc->ncycles[i * 32 + count_wakeup[i]]++;
    }
  }
#endif
  /* increment the simulation time */
  if(simc->clock_edge)
    simc->time_stamp = simc->time_stamp + 1; //+ 1.0;

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FastSysC: Returning from fsc_start" << endl;
#endif

#ifdef FFSC_STAT
  // DD
  static unsigned long long total_time = 0;//stop - start;
  //  static unsigned long long total_process_time = 0;//0;

  //  if(unisim_terminated)
  //  {
  unsigned long long stop;
  RDTSC(stop);
  //  cout << "c = " << c << endl;
  //  cout << "nw = " << nw << endl;

  total_time += stop - start;

  if(unisim_terminated)
  {
    ListPointer<fsc_process> process;

    for(process = fsc_process::process_list.Begin(); process; process++)
      {
	if(!process->port_list.Empty())
	  {
	    cout << process->mod->Name() << "::" << process->Name();
	    for(i = 0; i < 32; i++)
	      {
		cout << " " << simc->ncycles[32 * process->GetId() + i];
	      }
	    cout << endl;
	  }
      }
    //  }

  //  unsigned long long total_time = stop - start;
    unsigned long long total_process_time = 0;

    //  total_process_time += 0;
    for(process = fsc_process::process_list.Begin(); process; process++)
      total_process_time += process->time;
    for(process = fsc_process::process_list.Begin(); process; process++)
      {
	cout << process->mod->Name() << "::" << process->Name() << "\t\t\t t: " << process->time << "\t\t\t" << (((double) process->time * 100.0) / (double) total_process_time) << " %" << endl;
      }
    cout << "FastSysC scheduler : " << (((double)(total_time - total_process_time) * 100.0)/ (double) total_time) << " %" << endl;
    cout << " Total time         : " << total_time << endl;
    cout << " Total process time : " << total_process_time << endl;
    cout << " Scheduler time     : " << total_time - total_process_time << endl;
  }

#endif
}


void fsc_start_alt(uint64_t t)
{
  fsc_simcontext *simc = simcontext;
  if(t < 0)
    t = 0xffffffffffffffffULL; //HUGE_VAL;
  t = t + simc->time_stamp;
#ifdef DEBUG_FSC

  fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
  if(fsc_simcontext::debug)
    cerr << "FastSysC: Entering into fsc_start_alt" << endl;
#endif

  simc->stop = false;
  if(!simc->initialized)
  {
    simc->Initialize();
  }

  fsc_process **processes_to_execute = simc->processes_to_execute;
  fsc_process **process_to_execute = simc->processes_to_execute - 1;

#ifdef FFSC_STAT

  int count_wakeup[simc->nprocesses];
  int nw = 0;
  int i;
  int c = 0;
  unsigned long long start;
  RDTSC(start);
#endif
  /* main simulation loop */
  do
  {
    fsc_clock **clock;
#ifdef FFSC_STAT

    memset(count_wakeup, 0, sizeof(count_wakeup));
#endif
#ifdef DEBUG_FSC

    fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
#endif

    /* update the clocks */
    clock = simc->last_of_clock_signals;
    if(clock)
    {
      do
      {
        (*clock)->value ^= true;
      }
      while(--clock >= simc->clock_signal_table);
    }

    simc->clock_edge = (fsc_clock_edge)((int) simc->clock_edge ^ 1);

#ifdef DEBUG_FSC

    if(fsc_simcontext::debug)
    {
      cerr << "FastSysC: simc->clock_edge = " << simc->clock_edge << endl;
      cerr << "FastSysC: =================== time stamp = " << simc->time_stamp << " ====================" << endl;
    }
#endif

    /* Call all FSC_METHOD process sensitive to the clock edge */
    if(simc->last_of_sensitive_clock_processes[simc->clock_edge] >= simc->sensitive_clock_process_table[simc->clock_edge])
    {
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
      {
        switch(simc->clock_edge)
        {
            case POS_EDGE:
            cerr << "FastSysC: Clock front edge" << endl;
            break;

            case NEG_EDGE:
            cerr << "FastSysC: Clock falling edge" << endl;
            break;

            default:
            abort();
        }
      }
#endif
      fsc_process **sensitive_clock_process = simc->last_of_sensitive_clock_processes[simc->clock_edge];
      do
      {
        fsc_process *process = *sensitive_clock_process;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
        cerr.flush();
#endif

        CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
        cerr.flush();
#endif

      }
      while(--sensitive_clock_process >= simc->sensitive_clock_process_table[simc->clock_edge]);
    }



    if(simc->signal_to_update >= simc->signals_to_update)
    {
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
        cerr << "FastSysC: Propagating signals" << endl;
#endif
      /* update the signals */
      do
      {
        fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

        signal_to_update->read_buf ^= 1;
        signal_to_update->to_update = 0;

        fsc_process **process = signal_to_update->last_of_sensitive_processes;
        if(process)
        {
          do
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

            if(!(*process)->to_execute)
            {
              *(++process_to_execute) = *process;
              (*process)->to_execute = true;
            }
          }
          while(--process >= signal_to_update->sensitive_process_table);
        }
      }
      while(--simc->signal_to_update >= simc->signals_to_update);

      /* execute all sensitive processes */
      if(process_to_execute >= processes_to_execute)
      {
        do
        {
          fsc_process *source_process = *process_to_execute;
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: Calling process \"" << source_process->Name() << "\" of module \"" << source_process->mod->Name() << "\"" << endl;
          cerr.flush();
#endif

          CALL_INDIRECT_PROCESS(source_process);
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: Returning from process \"" << source_process->Name() << "\" of module \"" << source_process->mod->Name() << "\"" << endl;
          cerr.flush();
#endif

          source_process->to_execute = false;
#ifdef FFSC_STAT

          nw++;
          count_wakeup[source_process->GetId()]++;
#endif

          --process_to_execute;

          if(simc->signal_to_update >= simc->signals_to_update)
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: Propagating signals" << endl;
#endif
            /* update the signals */
            do
            {
              fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

              if(fsc_simcontext::debug)
                cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

              signal_to_update->read_buf ^= 1;
              signal_to_update->to_update = 0;

              fsc_process **target_process = signal_to_update->last_of_sensitive_processes;
              if(target_process)
              {
                do
                {
#ifdef DEBUG_FSC
                  if(fsc_simcontext::debug)
                    cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*target_process)->Name() << "\" of module \"" << (*target_process)->mod->Name() << "\"" << endl;
#endif

                  if(!(*target_process)->to_execute)
                  {
                    *(++process_to_execute) = *target_process;
                    (*target_process)->to_execute = true;
                  }
                }
                while(--target_process >= signal_to_update->sensitive_process_table);
              }
            }
            while(--simc->signal_to_update >= simc->signals_to_update);
          }
        }
        while(process_to_execute >= processes_to_execute);
      }
    }

#ifdef FFSC_STAT
    if(simc->clock_edge == POS_EDGE)
    {
      for(i = 0; i < simc->nprocesses; i++)
      {
        if(count_wakeup[i] >= 32)
          abort();
        simc->ncycles[i * 32 + count_wakeup[i]]++;
      }
    }
#endif

    /* increment the simulation time */
    if(simc->clock_edge)
      simc->time_stamp = simc->time_stamp + 1;//+ 1.0;
  }
  while(!simc->stop && simc->time_stamp < t);

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FastSysC: Returning from fsc_start_alt" << endl;
#endif
#ifdef FFSC_STAT

  unsigned long long stop;
  RDTSC(stop);
  cout << "c = " << c << endl;
  cout << "nw = " << nw << endl;

  ListPointer<fsc_process> process;
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    if(!process->port_list.Empty())
    {
      cout << process->mod->Name() << "_" << process->Name();
      for(i = 0; i < 32; i++)
      {
        cout << " " << simc->ncycles[32 * process->GetId() + i];
      }
      cout << endl;
    }
  }
  unsigned long long total_time = stop - start;
  unsigned long long total_process_time = 0;
  for(process = fsc_process::process_list.Begin(); process; process++)
    total_process_time += process->time;
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    cout << process->mod->Name() << "::" << process->Name() << " : " << (((double) process->time * 100.0) / (double) total_process_time) << " %" << endl;
  }
  cout << "FastSysC scheduler : " << (((double)(total_time - total_process_time) * 100.0)/ (double) total_time) << " %" << endl;
#endif
}


void fsc_start_acyclic(uint64_t t)
{
  fsc_simcontext *simc = simcontext;
  if(t < 0)
    t = 0xffffffffffffffffULL; //HUGE_VAL;
  t = t + simc->time_stamp;
#ifdef DEBUG_FSC

  fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
  if(fsc_simcontext::debug)
    cerr << "FastSysC: Entering into fsc_start_acyclic" << endl;
#endif

  simc->stop = false;
  if(!simc->initialized)
  {
    simc->Initialize();
  }
  if(!simc->static_sch)
  {
    simc->BuildSchedule();
  }

#ifdef FFSC_STAT
  int count_wakeup[simc->nprocesses];
  int nw = 0;
  int i;
  int c = 0;
  //	int d = 0;
  //	int e = 0;
  int n[fsc_process::process_list.Size()];
  memset(n, 0, sizeof(n));
  int m[fsc_process::process_list.Size()];
  memset(m, 0, sizeof(m));
  unsigned long long start;
  RDTSC(start);
#endif

  fsc_process **processes_to_execute = simc->processes_to_execute;
  fsc_process **process_to_execute = simc->processes_to_execute - 1;
  fsc_process **process_table = simc->process_table;
  int to_execute = 0;

  /* main simulation loop */
  do
  {
    fsc_clock **clock;

#ifdef FFSC_STAT

    memset(count_wakeup, 0, sizeof(count_wakeup));
#endif
#ifdef DEBUG_FSC

    fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
#endif

    /* update the clocks */
    clock = simc->last_of_clock_signals;
    if(clock)
    {
      do
      {
        (*clock)->value ^= true;
      }
      while(--clock >= simc->clock_signal_table);
    }

    simc->clock_edge = (fsc_clock_edge)((int) simc->clock_edge ^ 1);

#ifdef DEBUG_FSC

    if(fsc_simcontext::debug)
    {
      cerr << "FastSysC: simc->clock_edge = " << simc->clock_edge << endl;
      cerr << "FastSysC: =================== time stamp = " << simc->time_stamp << " ====================" << endl;
    }
#endif

    /* Call all FSC_METHOD process sensitive to the clock edge */
    if(simc->last_of_sensitive_clock_processes[simc->clock_edge] >= simc->sensitive_clock_process_table[simc->clock_edge])
    {
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
      {
        switch(simc->clock_edge)
        {
            case POS_EDGE:
            cerr << "FastSysC: Clock front edge" << endl;
            break;

            case NEG_EDGE:
            cerr << "FastSysC: Clock falling edge" << endl;
            break;

            default:
            abort();
        }
      }
#endif
      fsc_process **sensitive_clock_process = simc->last_of_sensitive_clock_processes[simc->clock_edge];
      do
      {
        fsc_process *process = *sensitive_clock_process;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
        cerr.flush();
#endif

        CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
        cerr.flush();
#endif

      }
      while(--sensitive_clock_process >= simc->sensitive_clock_process_table[simc->clock_edge]);
    }

    if(simc->signal_to_update >= simc->signals_to_update)
    {
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
        cerr << "FastSysC: Propagating signals" << endl;
#endif
      /* update the signals */
      do
      {
        fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

        signal_to_update->read_buf ^= 1;
        signal_to_update->to_update = 0;

        fsc_process **process = signal_to_update->last_of_sensitive_processes;
        if(process)
        {
          do
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

            if(!(*process)->to_execute)
            {
              to_execute++;
              (*process)->to_execute = true;
            }
          }
          while(--process >= signal_to_update->sensitive_process_table);
        }
      }
      while(--simc->signal_to_update >= simc->signals_to_update);

      fsc_process ***schedule = simc->static_sch;
      fsc_process **rank = *schedule;
      if(rank)
      {
        do
        {
          fsc_process *process = *rank;
          if(process)
          {
            do
            {
              if(process->to_execute)
              {
#ifdef DEBUG_FSC
                if(fsc_simcontext::debug)
                  cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
                cerr.flush();
#endif

                CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

                if(fsc_simcontext::debug)
                  cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
                cerr.flush();
#endif

                process->to_execute = false;
#ifdef FFSC_STAT

                nw++;
                count_wakeup[process->GetId()]++;
#endif

                to_execute--;
              }
            }
            while(to_execute && (process = *(++rank)));
          }

          if(simc->signal_to_update >= simc->signals_to_update)
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: Propagating signals" << endl;
#endif
            /* update the signals */
            do
            {
              fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

              if(fsc_simcontext::debug)
                cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

              signal_to_update->read_buf ^= 1;
              signal_to_update->to_update = 0;


              fsc_process **process = signal_to_update->last_of_sensitive_processes;
              if(process)
              {
                do
                {
#ifdef DEBUG_FSC
                  if(fsc_simcontext::debug)
                    cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

                  if(!(*process)->to_execute)
                  {
                    to_execute++;
                    (*process)->to_execute = true;
                  }
                }
                while(--process >= signal_to_update->sensitive_process_table);
              }
            }
            while(--simc->signal_to_update >= simc->signals_to_update);
          }
        }
        while(to_execute && (rank = *(++schedule)));
      }
      /* End of static scheduling */
#ifdef DEBUG_FSC
      if(fsc_simcontext::debug)
        cerr << "FastSysC: End of static scheduling" << endl;
#endif

      if(to_execute > 0)
      {
#ifdef FFSC_STAT
        c++;
#endif

        cout << timestamp() << endl;
        fsc_process **process = process_table;

        do
        {
          if((*process)->to_execute)
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: Calling process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
            cerr.flush();
#endif

            CALL_INDIRECT_PROCESS(*process);
#ifdef DEBUG_FSC

            if(fsc_simcontext::debug)
              cerr << "FastSysC: Returning from process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
            cerr.flush();
#endif

            (*process)->to_execute = false;
            to_execute--;
#ifdef FFSC_STAT

            nw++;
            count_wakeup[(*process)->GetId()]++;
            n[(*process)->GetId()]++;
#endif

          }
        }
        while(++process, to_execute > 0);

        if(simc->signal_to_update >= simc->signals_to_update)
        {
#ifdef DEBUG_FSC
          if(fsc_simcontext::debug)
            cerr << "FastSysC: Propagating signals" << endl;
#endif
          /* update the signals */
          do
          {
            fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

            if(fsc_simcontext::debug)
              cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

            signal_to_update->read_buf ^= 1;
            signal_to_update->to_update = 0;

            fsc_process **process = signal_to_update->last_of_sensitive_processes;
            if(process)
            {
              do
              {
#ifdef DEBUG_FSC
                if(fsc_simcontext::debug)
                  cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

                if(!(*process)->to_execute)
                {
                  *(++process_to_execute) = *process;
                  (*process)->to_execute = true;
                }
              }
              while(--process >= signal_to_update->sensitive_process_table);
            }
          }
          while(--simc->signal_to_update >= simc->signals_to_update);

          /* execute all sensitive processes */
          if(process_to_execute >= processes_to_execute)
          {
            do
            {
              fsc_process *source_process = *process_to_execute;
#ifdef DEBUG_FSC

              if(fsc_simcontext::debug)
                cerr << "FastSysC: Calling process \"" << source_process->Name() << "\" of module \"" << source_process->mod->Name() << "\"" << endl;
              cerr.flush();
#endif

              CALL_INDIRECT_PROCESS(source_process);
#ifdef DEBUG_FSC

              if(fsc_simcontext::debug)
                cerr << "FastSysC: Returning from process \"" << source_process->Name() << "\" of module \"" << source_process->mod->Name() << "\"" << endl;
              cerr.flush();
#endif

              source_process->to_execute = false;
#ifdef FFSC_STAT

              count_wakeup[source_process->GetId()]++;
              nw++;
#endif

              --process_to_execute;

              if(simc->signal_to_update >= simc->signals_to_update)
              {
#ifdef DEBUG_FSC
                if(fsc_simcontext::debug)
                  cerr << "FastSysC: Propagating signals" << endl;
#endif
                /* update the signals */
                do
                {
                  fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

                  if(fsc_simcontext::debug)
                    cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

                  signal_to_update->read_buf ^= 1;
                  signal_to_update->to_update = 0;

                  fsc_process **target_process = signal_to_update->last_of_sensitive_processes;
                  if(target_process)
                  {
                    do
                    {
#ifdef DEBUG_FSC
                      if(fsc_simcontext::debug)
                        cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*target_process)->Name() << "\" of module \"" << (*target_process)->mod->Name() << "\"" << endl;
#endif

                      if(!(*target_process)->to_execute)
                      {
                        *(++process_to_execute) = *target_process;
                        (*target_process)->to_execute = true;
                      }
                    }
                    while(--target_process >= signal_to_update->sensitive_process_table);
                  }
                }
                while(--simc->signal_to_update >= simc->signals_to_update);
              }
            }
            while(process_to_execute >= processes_to_execute);
          }
        }
      }
    }

#ifdef FFSC_STAT
    if(simc->clock_edge == POS_EDGE)
    {
      for(i = 0; i < simc->nprocesses; i++)
      {
        if(count_wakeup[i] >= 32)
          abort();
        simc->ncycles[i * 32 + count_wakeup[i]]++;
      }
    }
#endif

    /* increment the simulation time */
    if(simc->clock_edge)
      simc->time_stamp = simc->time_stamp + 1;//+ 1.0;
  }
  while(!simc->stop && simc->time_stamp < t);

#ifdef DEBUG_FSC

  if(fsc_simcontext::debug)
    cerr << "FastSysC: Returning from fsc_start_acyclic" << endl;
#endif

#ifdef FFSC_STAT

  unsigned long long stop;
  RDTSC(stop);
  cout << "c = " << c << endl;
  cout << "nw = " << nw << endl;

  ListPointer<fsc_process> process;
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    if(!process->port_list.Empty())
    {
      cout << process->mod->Name() << "::" << process->Name();
      for(i = 0; i < 32; i++)
      {
        cout << " " << simc->ncycles[32 * process->GetId() + i];
      }
      cout << endl;
    }
  }
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    if(!process->port_list.Empty())
    {
      cout << process->mod->Name() << "::" << process->Name() << " = " << n[process->GetId()] << endl;
    }
  }
  unsigned long long total_time = stop - start;
  unsigned long long total_process_time = 0;
  for(process = fsc_process::process_list.Begin(); process; process++)
    total_process_time += process->time;
  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    cout << process->mod->Name() << "::" << process->Name() << " : " << (((double) process->time * 100.0) / (double) total_process_time) << " %" << endl;
  }
  cout << "FastSysC scheduler : " << (((double)(total_time - total_process_time) * 100.0)/ (double) total_time) << " %" << endl;
#endif
}

#endif

void fsc_stabilize()
{
  fsc_simcontext *simc = simcontext;
#ifdef DEBUG_FSC

  fsc_simcontext::debug = simc->time_stamp >= simc->debug_start && simc->time_stamp <= simc->debug_stop;
#endif

  fsc_process **processes_to_execute = simc->processes_to_execute;
  fsc_process **process_to_execute = simc->processes_to_execute - 1;

  if(simc->signal_to_update >= simc->signals_to_update)
  {
#ifdef DEBUG_FSC
    if(fsc_simcontext::debug)
      cerr << "FastSysC: Propagating signals" << endl;
#endif

    do
    {
      /* update the signals */
      do
      {
        fsc_signal_base *signal_to_update = *simc->signal_to_update;
#ifdef DEBUG_FSC

        if(fsc_simcontext::debug)
          cerr << "FastSysC: updating signal " << signal_to_update->Name() << endl;
#endif

        signal_to_update->read_buf ^= 1;
        signal_to_update->to_update = 0;

        fsc_process **process = signal_to_update->last_of_sensitive_processes;
        if(process)
        {
          do
          {
#ifdef DEBUG_FSC
            if(fsc_simcontext::debug)
              cerr << "FastSysC: signal " << signal_to_update->Name() << " will wake up process \"" << (*process)->Name() << "\" of module \"" << (*process)->mod->Name() << "\"" << endl;
#endif

            if(!(*process)->to_execute)
            {
              *(++process_to_execute) = *process;
              (*process)->to_execute = true;
            }
          }
          while(--process >= signal_to_update->sensitive_process_table);
        }
      }
      while(--simc->signal_to_update >= simc->signals_to_update);

      /* execute all sensitive processes */
      if(process_to_execute >= processes_to_execute)
      {
        do
        {
          fsc_process *process = *process_to_execute;
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: Calling process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
#endif

          CALL_INDIRECT_PROCESS(process);
#ifdef DEBUG_FSC

          if(fsc_simcontext::debug)
            cerr << "FastSysC: Returning from process \"" << process->Name() << "\" of module \"" << process->mod->Name() << "\"" << endl;
          cerr.flush();
#endif

          process->to_execute = false;
        }
        while(--process_to_execute >= processes_to_execute);
      }
    }
    while(simc->signal_to_update >= simc->signals_to_update);
  }

}

/*
int fsc_private_main(int argc, char *argv[])
{
	cout << "        -= FraternitéSysC v1.0 =-" << endl << endl;
	cout << "Authors   : Gilles Mouchard and Daniel Gracia Pérez" << endl;
	cout << "Copyright: (C) 2003-2005 CEA and Université Paris Sud" << endl;
	cout << endl;
	simcontext = new fsc_simcontext();
	int r = fsc_main(argc, argv);
	delete simcontext;
	return r;
}
*/

fsc_simcontext *GetSimContext()
{
  return simcontext;
}

/**
 * \brief Returns the current time stamp (cycle number)
 */
uint64_t timestamp()
{ return simcontext->time_stamp;
}

void fsc_stop()
{
  simcontext->stop = true;
}

void fsc_debug(uint64_t start, uint64_t stop)
{
  simcontext->debug_start = start;
  simcontext->debug_stop = stop;
}

/**
 * \brief Build the signal dependency graph
 */
Graph<fsc_signal_base> *fsc_simcontext::BuildDependencyGraph()
{
  ListPointer<fsc_signal_base> signal;
  int nsignals = fsc_signal_base::signal_list.Size();
  typedef Vertex<fsc_signal_base> *PVertexSignal;
  Vertex<fsc_signal_base> **lookup = new PVertexSignal[nsignals];
  ListPointer<fsc_prim_port> inport, outport, target_port;
  ListPointer<fsc_process> process, target_process;
  Graph<fsc_signal_base> *graph = new Graph<fsc_signal_base>();

  for(signal = fsc_signal_base::signal_list.Begin(); signal; signal++)
  {
    Vertex<fsc_signal_base> *v = new Vertex<fsc_signal_base>(signal);
    lookup[signal->GetId()] = v;
  }

  for(process = fsc_process::process_list.Begin(); process; process++)
  {
    bool use_internal_dependencies = false;
    for(inport = process->port_list.Begin(); inport; inport++)
    {
      if(!inport->dependencies.Empty())
      {
        use_internal_dependencies = true;
        break;
      }
    }

#ifdef DEBUG_FSC
    cerr << process->mod->Name() << "::" << process->Name() << ": use internal dependencies = " << (use_internal_dependencies ? "YES" : "NO") << endl;
#endif

    for(inport = process->port_list.Begin(); inport; inport++)
    {
      for(outport = process->mod->port_list.Begin(); outport; outport++)
      {
        if(outport->port_type == FSC_OUT)
        {
          if(!use_internal_dependencies || inport->dependencies.Search(outport))
          {
            if(inport->signal != outport->signal)
            {
              Vertex<fsc_signal_base> *v = lookup[inport->signal->GetId()];
              Vertex<fsc_signal_base> *w = lookup[outport->signal->GetId()];
              //cerr << v->Data()->Name() << " -> " << w->Data()->Name() << endl;
              (*v)(w);
            }
          }
        }
      }
    }
  }

  int i, j;
  /* This code eliminates solitary vertex, and adds all other vertices into the dependency graph */
  for(i = 0; i < nsignals; i++)
  {
    Vertex<fsc_signal_base> *v = lookup[i];
    if(v->Successors().Empty())
    {
      bool found = false;
      for(j = 0; j < nsignals && !found; j++)
      {
        Vertex<fsc_signal_base> *w = lookup[j];
        if(w)
        {
          ListPointer<Vertex<fsc_signal_base> > u;
          for(u = w->Successors().Begin(); u && !found; u++)
          {
            if(u == v)
            {
              found = true;
            }
          }
        }
      }
      if(!found)
      {
        delete v;
        lookup[i] = 0;
        continue;
      }
    }
    (*graph) << v;
  }

  delete[] lookup;

  return graph;
}

/**
 * \brief Associte a rank to each process for ordering
 */
List<List<fsc_process> > *fsc_simcontext::RankProcesses()
{
  cerr << "Building dependency graph" << endl;
  Graph<fsc_signal_base> *graph = BuildDependencyGraph();

  graph->Dotify("dependency_graph.dot");
  //	graph->Dotify("graph.dot");

  List<Graph<fsc_signal_base> > *sccs = graph->SCCs();
  //	cerr << graph->Card() << " vertices, " << sccs->Size() << " SCCs" << endl;

  if(sccs->Size() != graph->Card())
  {
    cerr << "WARNING!: Cyclic dependency graph" << endl;
    ListPointer<Graph<fsc_signal_base> > g;

    cerr << "FastSysC will dump the strongly connected graphs with more than one vertex into the following .dot files:" << endl;
    int i;
    for(i = 0, g = sccs->Begin(); g; g++, i++)
    {
      if(g->Card() != 1)
      {
        char filename[256];
        sprintf(filename, "scc%d.dot", i);
        cerr << "\"" << filename << "\"" << endl;
        g->Dotify(filename);
      }
    }
    cerr << ".dot files can be visualized using graphviz from AT&T" << endl;



    cerr << "Making graph acyclic" << endl;
    graph->MakeAcyclic();
    graph->Dotify("acyclic_dependency_graph.dot");
  }

  ListPointer<Graph<fsc_signal_base> > g;
  for(g = sccs->Begin(); g;)
  {
    delete g;
    sccs->Remove(g);
  }
  delete sccs;


  //	graph->Dotify("acgraph.dot");

  cerr << "Finding a topological order" << endl;
  List<List<Vertex<fsc_signal_base> > > *topological = graph->Topological();

  ListPointer<List<Vertex<fsc_signal_base> > > rank;
  ListPointer<Vertex<fsc_signal_base> > u, v;
  ListPointer<fsc_process> process;
  ListPointer<fsc_prim_port> inport, outport;

  List<List<fsc_process> > *schedule = new List<List<fsc_process> >();

  cerr << "Ranking processes" << endl;

  for(rank = topological->Begin(); rank;)
  {
    List<fsc_process> *rank_schedule = new List<fsc_process>();
    for(u = rank->Begin(); u; u++)
    {
      fsc_signal_base *signal = u->Data();

      for(process = fsc_process::process_list.Begin(); process; process++)
      {
        if(!process->port_list.Empty())
        {
          if(!rank_schedule->Search(process))
          {
            bool use_internal_dependencies = false;
            for(inport = process->port_list.Begin(); inport; inport++)
            {
              if(!inport->dependencies.Empty())
              {
                use_internal_dependencies = true;
                break;
              }
            }
            for(outport = process->mod->port_list.Begin(); outport; outport++)
            {
              if(outport->port_type == FSC_OUT)
              {
                bool found = false;
                if(use_internal_dependencies)
                {
                  for(inport = process->port_list.Begin(); inport; inport++)
                  {
                    if(inport->dependencies.Search(outport))
                    {
                      found = true;
                      break;
                    }
                  }
                }
                else
                {
                  found = true;
                }

                if(found)
                {
                  if(outport->signal == signal)
                  {
                    (*rank_schedule) << process;
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }
    if(!rank_schedule->Empty())
      (*schedule) << rank_schedule;
    else
      delete rank_schedule;

    delete rank;
    topological->Remove(rank);
  }
  delete topological;


  cerr << "A schedule was found:" << endl;
  cout << *schedule << endl;

  cout << "Dumping schedule into " << SCHEDULE_FILENAME << endl;
  ofstream of(SCHEDULE_FILENAME);

  of << *schedule << endl;

  delete graph;
  return schedule;
}

/**
 * \brief Build the static schedule
 */
void fsc_simcontext::BuildSchedule()
{ List<List<fsc_process> > *schedule = RankProcesses();
  ListPointer<List<fsc_process> > rank;
  ListPointer<fsc_process> process;
  int i, j;
  int nranks = schedule->Size();
  typedef fsc_process *PProcess;
  typedef fsc_process **PPProcess;
  static_sch = new PPProcess[nranks + 1];

  for(i = 0, rank = schedule->Begin(); rank; i++)
  { int nprocesses = rank->Size();
    static_sch[i] = new PProcess[nprocesses + 1];
    for(j = 0, process = rank->Begin(); process; process++, j++)
    { *(*(static_sch + i) + j) = process;
    }
    *(*(static_sch + i) + j) = 0;
    delete rank;
    schedule->Remove(rank);
  }
  delete schedule;
  static_sch[i] = 0;
}
