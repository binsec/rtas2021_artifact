/***************************************************************************
   svg_memreq_service.h  -  Service for generation of memreq wavefom dumps
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
Foundings: Partly founded with SARC foundings
All rights reserved.  
  
Redistribution and use in source and binary forms, with or without modification,   
are permitted provided that the following conditions are met:  
  
 - Redistributions of source code must retain the above copyright notice, this   
   list of conditions and the following disclaimer.   
  
 - Redistributions in binary form must reproduce the above copyright notice,   
   this list of conditions and the following disclaimer in the documentation   
   and/or other materials provided with the distribution.   
   
 - Neither the name of the UNISIM nor the names of its contributors may be   
   used to endorse or promote products derived from this software without   
   specific prior written permission.   
     
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE   
DISCLAIMED.   
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,   
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,   
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY   
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING   
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,   
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
***************************************************************************** */

#ifndef __SVG_MEMREQ_SERVICE_H__
#define __SVG_MEMREQ_SERVICE_H__

//#include "memreq.h"
#include <debug/svg_memreq_interface.hh>
#include <utils/services/service.hh>
#include <sstream>
#include <string>

using full_system::utils::services::Service;
using full_system::plugins::SVGmemreqInterface;

/**
 * \brief Class used by the SVGmemreq_service class to store boxes representing modules
 */
class CacheMessageBox
{public:
  /**
   * \brief Creates a new CacheMessageBox
   */
  CacheMessageBox(int _y, const string &_color, const string &_name)
  { y = _y;
    color = _color;
    name = _name;
  }
  /**
   * \brief Dump the SVG command corresponding to drawing the box and its label
   */
  void dump(ostream &os, int nb_cycle, int step) const
  { int maxx = 70 + nb_cycle * step + 10 + 20;
    os << "<rect x=\"70\" y=\"" << y << "\" width=\"" << maxx-90 << "\" height=\"20\" fill=\"" << color << "\"/>\n";
    os << "<text x=\"10\" y=\"" << y+15 << "\" xml:space=\"preserve\" style=\"font-size:20px;font-style:normal;font-variant:normal;font-weight:bold;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">" << name << "</text>" << endl;
  }
  int y;           ///< y coordinate to put the box at
  string color;    ///< color of the box
  string name;     ///< name associated with the box
};

/**
 * \brief Class used by the SVGmemreq_service  class to memreqs arrows
 */
class CacheMessageInfo
{public:
  /**
   * \brief Creates a new CacheMessageInfo
   */
  CacheMessageInfo(int _cycle, uint32_t _addr, const string &_sender, const string &_target, const string &_label, const string &_hint)
  { cycle = _cycle;
    addr = _addr;
    sender = _sender;
    target = _target;
    label = _label;
    hint = _hint;
  }
  /**
   * \brief Dump the SVG code corresponding to the arrow
   */
  void dump_arrow(ostream &os, const list<CacheMessageBox> &boxes, int mincycle, int step, bool skewing, const list<uint32_t> &addresses) const
  { if(cycle<mincycle) return;
    if(!is_monitored(addr,addresses)) return;
//cerr << cycle << endl;
    int y1 = 0;
    int y2 = 0;
    int a1, a2;
    int x1 = 70 + (cycle-mincycle) * step;
    int x2 = 70 + (cycle-mincycle) * step + (skewing?step/4:0);

    for(list<CacheMessageBox>::const_iterator it = boxes.begin();it!=boxes.end();it++)
    { if(sender == (*it).name) y1 = (*it).y;
      if(target == (*it).name) y2 = (*it).y;
    }

    if(y2>y1) { a1 = y1 + 20;  a2 = y2 - 10; }
    if(y1>y2) { a1 = y1;       a2 = y2 + 30; }

    os << "<path onmouseover=\"on_arrow(evt)\" d=\"M ";
    os << x1 << "," << a1;
    os << " L ";
    os << x2 << "," << a2;
    os << "\">\n";
    os << "<infos>\n";
    os << hint << "\n";
    os << "</infos>\n";
    os << "</path>\n";
  }
  /**
   * \brief Dump the SVG code corresponding to printing the label
   */
  void dump_label(ostream &os, const list<CacheMessageBox> &boxes, int mincycle, int step, bool skewing) const
  { if(cycle<mincycle) return;
    int x = 70 + ((skewing?1:0)+cycle-mincycle) * step;
    int y;
    for(list<CacheMessageBox>::const_iterator it = boxes.begin();it!=boxes.end();it++)
    { if(target == (*it).name) y = (*it).y;
    }
    // Put the ending label in the target box
    os << "<text x=\"" << x << "\" y=\"" << y+10+5 << "\" style=\"font-size:14px;font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:middle;fill:white;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">";
    os << label;
    os << "</text>" << endl;
  }
 private:
  int cycle;       ///< Cycle number the event occured
  uint32_t addr;   ///< Corresponding address
  string sender;   ///< Sender name of the memreq
  string target;   ///< Target name of the memreq
  string label;    ///< Label to be put in the target bar
  string hint;     ///< Information associated with the memreq displayed when hovering
  
  bool is_monitored(uint32_t a, const list<uint32_t> &addresses) const
  { // As long as the list is empty, consider every address as monitorable
    if(addresses.empty()) return true;
    // If there is some adress in the list, only monitor those addresses
    uint32_t aligned = a & 0xFFFFFF80;
    for(list<uint32_t>::const_iterator it = addresses.begin();it!=addresses.end();it++)
    { if(*it==aligned) return true;
    }
    return false;  
  }

};

/**
 * \brief Class used by the SVGmemreq_service class for storing box messages
 */
class CacheMessageBoxInfo
{public:
  /**
   * \brief Create a new CacheMessageBoxInfo
   */
  CacheMessageBoxInfo(int _cycle, const string &_name, const string &_info)
  { cycle = _cycle;
    box_name = _name;
    info = _info;
  }
  /**
   * \brief Dump the svg command generating the label
   */
  void dump(ostream &os, const list<CacheMessageBox> &boxes, int mincycle, int step) const
  { if(cycle<mincycle) return;
    int y = 0;
    int x = 70 + (cycle-mincycle) * step;
    for(list<CacheMessageBox>::const_iterator it = boxes.begin();it!=boxes.end();it++)
    { if(box_name == (*it).name) y = (*it).y;
    }
    os << "<text x=\"" << x << "\" y=\"" << y+10+5 << "\">";
    os << info;
    os << "</text>" << endl;
  }
 private:
  int cycle;         ///< Cycle number the event occured
  string box_name;   ///< Name of the box 
  string info;       ///< Informationto be displayed in the bar
};



/**
 * Interface for pthread compatible CPUs.
 */
class SVGmemreq_service : public Service<SVGmemreqInterface>
{public:

  ServiceExport<SVGmemreqInterface> port;

  /**
   * \brief Creates a new non-sleeping thread, and register it as an available thread
   */
  SVGmemreq_service(const char *name, Object *parent) : 
    Object(name,parent),
    Service<SVGmemreqInterface>(name, parent),
    port("port",this)
  { nb_cpu = 0;
    nb_cac = 0;
    nb_bus = 0;
    nb_mem = 0;
    mincycle=0;
    maxy=0;
    maxcycle=5;
    step=10;
    skewing=false;
    current_y = 20;
  }

  /**
   * \brief Class destructor
   *
   * The SVG file is generated during the destructor
   */
  ~SVGmemreq_service()
  { cerr << "Generating memreq SVG..." << endl;
    dump("messages.svg");
    cerr << "...done." << endl;
  }
  
  // --- Service Interface ----------------------------------------------------
  
  virtual void register_cpu(const string & name)
  { stringstream ss;
//    cerr << "\e[1;35m register_cpu " << name << "\e[0m" << endl;
    nb_cpu++;
    ss << "P" << nb_cpu;
    names[name] = ss.str();
    add_box(current_y, "red", ss.str());
    current_y+=p_to_c;
  }
  
  virtual void register_cache(const string & name)
  { stringstream ss;
//    cerr << "\e[1;35m register_cache " << name << "\e[0m" << endl;
    nb_cac++;
    ss << "$" << nb_cac;
    names[name] = ss.str();
    add_box(current_y, "orange", ss.str());
    current_y+=c_to_np;
  }
  
  virtual void register_bus(const string & name)
  { stringstream ss;
//    cerr << "\e[1;35m register_bus " << name << "\e[0m" << endl;
    nb_bus++;
    ss << "BS";
    names[name] = ss.str();
    add_box(current_y, "blue", ss.str());
    current_y+=p_to_c;
  }
  
  virtual void register_mem(const string & name)
  { stringstream ss;
//    cerr << "\e[1;35m register_mem " << name << "\e[0m" << endl;
    nb_mem++;
    ss << "MM";
    names[name] = ss.str();
    add_box(current_y, "magenta", ss.str());
    current_y+=yspace_mem;
  }
  
  virtual void add_cache_to_cpu(uint64_t cycle, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { map<string,string>::iterator it_t = names.find(target);
    if(it_t==names.end())
    { cerr << "Target " << target << " not found." << endl;
      return;
    }
    string cpu_name = it_t->second;
    string cac_name = cpu_name;
    cac_name[0] = '$';
    add(cycle,cac_name,cpu_name,label,mr);
  }

  virtual void add_mem_to_bus(uint64_t cycle, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { map<string,string>::iterator it_t = names.find(target);
    if(it_t==names.end())
    { cerr << "Target " << target << " not found." << endl;
      return;
    }
    string bus_name = it_t->second;
    string mem_name = "MM";
    
    add(cycle,mem_name,bus_name,label,mr);
  }
  
  virtual void add_cac_to_bus(uint64_t cycle, int cpu_number, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { map<string,string>::iterator it_t = names.find(target);
    if(it_t==names.end())
    { cerr << "Target " << target << " not found." << endl;
      return;
    }
    string bus_name = it_t->second;
    stringstream ss;
    ss << "$" << cpu_number;
    string cac_name = ss.str();
    add(cycle,cac_name,bus_name,label,mr);
  }

  virtual void add_bus_to_mem(uint64_t cycle, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { map<string,string>::iterator it_t = names.find(target);
    if(it_t==names.end())
    { cerr << "Target " << target << " not found." << endl;
      return;
    }
    string mem_name = it_t->second;
    string bus_name = "BS";
//    cerr << "\e[1;35m add_arrow "<< bus_name << " -> " << mem_name << "\e[0m" << endl;  
    add(cycle,bus_name,mem_name,label,mr);
  }

  virtual void add_bus_to_cac(uint64_t cycle, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { map<string,string>::iterator it_t = names.find(target);
    if(it_t==names.end())
    { cerr << "Target " << target << " not found." << endl;
      return;
    }
    string cac_name = it_t->second;
    string bus_name = "BS";
//    cerr << "\e[1;35m add_arrow "<< bus_name << " -> " << mem_name << "\e[0m" << endl;  
    add(cycle,bus_name,cac_name,label,mr);
  }

  virtual void add_cpu_to_cac(uint64_t cycle, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { map<string,string>::iterator it_t = names.find(target);
    if(it_t==names.end())
    { cerr << "Target " << target << " not found." << endl;
      return;
    }
    string cac_name = it_t->second;
    string cpu_name = cac_name;
    cpu_name[0] = 'P';
    add(cycle,cpu_name,cac_name,label,mr);
  }
  
  virtual void add_box_info(uint64_t cycle, const string &name, const string &info)
  { map<string,string>::iterator it_t = names.find(name);
    if(it_t==names.end())
    { cerr << "Target " << name << " not found." << endl;
      return;
    }
    string box_name = it_t->second;
//    cerr << timestamp() << " \e[1;35m add_box_info \e[0m " << box_name << " " << info << endl;
    boxes_info.push_back(CacheMessageBoxInfo(cycle,box_name,info));
    if(cycle>maxcycle) maxcycle = ( ((cycle%5==0)?0:1) + (cycle/5) ) * 5;
  }
  
  virtual void set_mincycle(int value)
  { mincycle = value;
  }
  
  virtual void set_step(int value)
  { step = value;
  }
  
  virtual void set_skewing(bool value)
  { skewing = value;
  }

  virtual void monitor_address(uint32_t address)
  { monitored_addresses.push_back(address & 0xFFFFFF80);
  
cerr << "------------------------------------------> " << hex << address << dec << endl;

  }
  
  // --------------------------------------------------------------------------
  
 private:
 
  /**
   * \brief Add a new memory request (arrow) to the list
   */    
  void add(int cycle, const string &sender, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { stringstream ss, ss_data;
    ss << "[b]Cycle:[/b] " << cycle << "\n";

    ss << "[b]Instr:[/b] 0x" << hex << mr.instr.cia << dec << ": ";
    if(mr.instr.operation) mr.instr.operation->disasm(ss);
    else ss << "NOINSTR";
    ss << "\n";

    ss << "[b]Command:[/b] " << mr.command << ", ";
    ss << "[b]Type:[/b] " << mr.message_type << "\n";
    ss << "[b]Address:[/b] 0x" << hex << mr.address << dec << ", ";
    ss << "[b]Size:[/b] " << mr.size << "\n";
    ss << "[b]Req_sender:[/b] " << mr.req_sender->name() << "\n";
    if(mr.cachable) ss << "cachable\n";
    else ss << "not cachable\n";

    if(mr.command==memreq_types::cmd_WRITE || mr.message_type==memreq_types::type_ANSWER)
    { const memreq_dataless<Instruction> *mr_ptr = (memreq_dataless<Instruction> *)&mr;
      const char *data = mr_ptr->Read();
      int size = mr_ptr->size;
      print_buffer(ss_data,data,size);
      ss << "[b]data:[/b] " << ss_data.str() << "\n";
    }
    else ss << "[b]data:[/b] -\n";
//    cerr << ss_data.str();
//zzz
    
    infos.push_back(CacheMessageInfo(cycle, mr.address, sender, target, label, ss.str()));
    if(cycle>maxcycle) maxcycle = ( ((cycle%5==0)?0:1) + (cycle/5) ) * 5;
  }


  /**
   * \brief Add a new architecture component (box) to the list
   */    
  void add_box(int y, const string &color, const string &name)
  { boxes.push_back(CacheMessageBox(y, color, name));
    if(y>maxy) maxy = y;
  }

  void dump(const char *filename)
  { ofstream os;
    os.open(filename, ios_base::trunc);

    dump_header(os);
    dump_grid(os);
    dump_axis(os);
    for(list<CacheMessageBox>::const_iterator it = boxes.begin();it!=boxes.end();it++)
    { (*it).dump(os,1+maxcycle-mincycle,step);
    }
    os << "<g style=\"fill:none;stroke:#000000;stroke-width:3px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;marker-end:url(#Triangle)\">\n";
    for(list<CacheMessageInfo>::const_iterator it = infos.begin();it!=infos.end();it++)
    { (*it).dump_arrow(os, boxes, mincycle, step, skewing, monitored_addresses);
    }
    os << "</g>" << endl;
    os << "<g style=\"font-size:14px;font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:middle;fill:white;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">";
    for(list<CacheMessageInfo>::const_iterator it = infos.begin();it!=infos.end();it++)
    { (*it).dump_label(os, boxes, mincycle, step, skewing);
    }
    os << "</g>" << endl;
    os << "<g style=\"font-size:14px;font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:middle;fill:white;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">";
    for(list<CacheMessageBoxInfo>::const_iterator it = boxes_info.begin();it!=boxes_info.end();it++)
    { (*it).dump(os, boxes, mincycle, step);
    }
    os << "</g>" << endl;
    dump_footer(os);
    os.close();
    ofstream html;
    html.open("result.htm", ios_base::trunc);
    dump_index(html);
    html.close();
  }

  void dump_header(ostream &os)
  { int maxx = 70 + (1+maxcycle-mincycle) * step + 10 + 20;
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    os << "<svg\n";
    os << "   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n";
    os << "   xmlns:cc=\"http://web.resource.org/cc/\"\n";
    os << "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n";
    os << "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n";
    os << "   xmlns=\"http://www.w3.org/2000/svg\"\n";
    os << "   xmlns:sodipodi=\"http://inkscape.sourceforge.net/DTD/sodipodi-0.dtd\"\n";
    os << "   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n";
    os << "  xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n";
    os << "   version=\"1.1\"\n";
    os << "   baseProfile=\"full\"\n";
    os << "   x=\"0\"\n";
    os << "   y=\"0\"\n";
    os << "   width=\"" << maxx << "\"\n";
    os << "   height=\"" << maxy+80 << "\"\n";
    os << "   id=\"svg2\"\n";
    os << "   sodipodi:version=\"0.32\"\n";
    os << "   inkscape:version=\"0.43\"\n";
    os << "   sodipodi:docname=\"roro.svg\"\n";
    os << "   sodipodi:docbase=\"C:\\Documents and Settings\\Administrator\\Desktop\">\n";
    os << "  <title id=\"title3\">Rectangles</title>\n";
    os << "  <defs id=\"defs4\" />\n";
    os << "  <defs>\n";
    os << "    <marker id=\"Triangle\"\n";
    os << "      viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\"\n";
    os << "      markerUnits=\"strokeWidth\"\n";
    os << "      markerWidth=\"4\" markerHeight=\"3\"\n";
    os << "      orient=\"auto\">\n";
    os << "      <path d=\"M 0 0 L 10 5 L 0 10 z\" />\n";
    os << "    </marker>\n";
    os << "  </defs>" << endl;
    
    os << "<script type=\"text/ecmascript\"> <![CDATA[\n";

    os << "  function on_arrow(evt)\n";
    os << "  { var aro = evt.target;\n";
    os << "    var txt_box = top.document.getElementById('txt');\n";
    os << "    var inf = aro.getElementsByTagName('infos').item(0).childNodes.item(0);\n";
    os << "    if(inf)\n";
    os << "    { s = inf.data;\n";
    os << "      s = s.replace(/^[\\n]*/,'');\n";
    os << "      s = s.replace(/[\\n]*$/,'');\n";
    os << "      s = s.replace(/\\[/g,'<');\n";
    os << "      s = s.replace(/\\]/g,'>');\n";
    os << "      s = s.replace(/\\n/g,'<br>');\n";
    os << "      txt_box.innerHTML = s;\n";
    os << "      txt_box.style.left=top.document.body.scrollLeft+50\n";
    os << "    }\n";
    os << "  }\n";
    os << "]]>\n</script>" << endl;
  }

  void dump_grid(ostream &os)
  { int x=70;
    os << "<g style=\"fill:none;stroke:#888888;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;stroke-dasharray:4,8;stroke-dashoffset:0\">\n";
    for(int i=mincycle;i<=maxcycle;i+=5)
    { os << "<path d=\"M " << x << "," << maxy+40 << "  L " << x << ",10\"/>\n";
      x+=5*step;
    }
    os << "</g>\n";
    x=70;
    os << "<g style=\"font-size:12px;font-style:normal;font-variant:normal;font-weight:bold;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">\n";
    for(int i=mincycle;i<=maxcycle;i+=5)
    { os << "<text x=\"" << x << "\" y=\"" << maxy+70 << "\">" << i << "</text>\n";
      x+=5*step;
    }
    os << "</g>" << endl;
  }

  void dump_axis(ostream &os)
  { int x = 70;
    int maxx = 70 + (1+maxcycle-mincycle) * step + 10;

    os << "<path style=\"fill:none;fill-opacity:0.75;fill-rule:evenodd;stroke:#000000;stroke-width:5px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;marker-end:url(#Triangle)\" ";
    os <<       "d=\"M 50,10 L  50," << maxy+40 << " L " << maxx << "," << maxy+40 << "\" id=\"axis\" />\n";
    os << "<g style=\"fill:none;stroke:#000000;stroke-width:5px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\">\n";
    for(int i=mincycle;i<=maxcycle;i++)
    { os << "<path d=\"M " << x << "," << maxy+40 << "  L " << x << "," << maxy+(i%5==0?50:47) << "\"/>\n";
      x += step;
    }
    os << "</g>" << endl;
  }
  
  void dump_footer(ostream &os)
  { os << "</svg>" << endl;
  }

  void dump_index(ostream &html)
  { int maxx = 70 + (1+maxcycle-mincycle) * step + 10 + 20; 
    html << "<html>\n";
    html << "<head>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "<style>\n";
    html << "#txt {border:1px solid black; position:absolute; left:50px; top:" << maxy+80 << "px; width:700px; height:140px; padding:5px; font-family:sans-serif}\n";
    html << "</style>\n";
    html << "<div style='position:absolute; left:0px; top:0px'>\n";
    html << "<iframe src='messages.svg' width='" << maxx << "' height='" << maxy+80 << "' frameborder='0'>\n";
    html << "<p>NEED IFRAME & SVG SUPPORT</p>\n";
    html << "</iframe>\n";
    html << "</div>\n";
    html << "<div id='txt'>\n";
    html << "Hover on messages for details\n";
    html << "</div>\n";
    html << "</body>\n";
    html << "</html>" << endl;
  }
  
  // --------------------------------------------------------------------------

  int nb_cpu;
  int nb_cac;
  int nb_bus;
  int nb_mem;

  // --------------------------------------------------------------------------
  
  int mincycle;
  int maxcycle;
  int maxy;
  int step;
  bool skewing;
  int current_y;
  
  static const int p_to_c = 45;
  static const int c_to_np = 60;
  static const int yspace_mem = 60;

  // --------------------------------------------------------------------------
  
  list<CacheMessageBox> boxes;
  list<CacheMessageInfo> infos;
  list<CacheMessageBoxInfo> boxes_info;
  
  list<uint32_t> monitored_addresses;
  
  map<string,string> names; // Associates module names to small names

};


#endif
