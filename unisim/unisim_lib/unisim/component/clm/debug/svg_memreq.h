/***************************************************************************
  svg_memreq.h  -  cycle-level module that when instatiatied prior to the
  bus, cache and memories modules, will dump all the communications corres-
  ponding to memory requests into an SVG file.
***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
Foundings: Partly founded with HiPEAC foundings
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

#ifndef _CACHE_MESSAGES_SVG_H_
#define _CACHE_MESSAGES_SVG_H_

#include "common.h"
#include "system/cpu/powerpc405/ppc_instruction.h"
#include "memreq.h"
#include <ostream>
#include <sstream>
#include <list>

using namespace std;

/**
 * \brief Class used by the CacheMessageSVG class to store boxes representing modules
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
 * \brief Class used by the CacheMessageSVG class to memreqs arrows
 */
class CacheMessageInfo
{public:
  /**
   * \brief Creates a new CacheMessageInfo
   */
  CacheMessageInfo(int _cycle, const string &_sender, const string &_target, const string &_label, const string &_hint)
  { cycle = _cycle;
    sender = _sender;
    target = _target;
    label = _label;
    hint = _hint;
  }
  /**
   * \brief Dump the SVG code corresponding to the arrow
   */
  void dump_arrow(ostream &os, const list<CacheMessageBox> &boxes, int mincycle, int step, bool skewing) const
  { if(cycle<mincycle) return;
    int y1 = 0;
    int y2 = 0;
    int a1, a2;
    int x1 = 70 + (cycle-mincycle) * step;
    int x2 = 70 + ((skewing?1:0)+cycle-mincycle) * step;

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
  string sender;   ///< Sender name of the memreq
  string target;   ///< Target name of the memreq
  string label;    ///< Label to be put in the target bar
  string hint;     ///< Information associated with the memreq displayed when hovering
};

/**
 * \brief Class used by the CacheMessageSVG class for storing box messages
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
 *  \brief 
 */
class CacheMessageSVG
{public:
  /**
   * \brief Creates a new CacheMessageSVG
   */
  CacheMessageSVG()
  { mincycle=0;
    maxy=0;
    maxcycle=5;
    step=10;
    skewing=false;
    
    int y = 20;
    const int p_to_c = 45;
    const int c_to_np = 60;

    add_box(y, "red", "P1");       y+=p_to_c;
    add_box(y, "orange", "$1");    y+=c_to_np;

    add_box(y, "red", "P2");       y+=p_to_c;
    add_box(y, "orange", "$2");    y+=c_to_np;

    add_box(y, "red", "P3");       y+=p_to_c;
    add_box(y, "orange", "$3");    y+=c_to_np;

    add_box(y, "blue",    "BS");   y+=p_to_c;
    add_box(y, "magenta", "MM");
  }
  
  /**
   * \brief Class destructor
   *
   * The SVG file is generated during the destructor
   */
  ~CacheMessageSVG()
  { if(!infos.empty())
    { cerr << "Generating memreq SVG..." << endl;
      dump("messages.svg");
      cerr << "...done." << endl;
    }
  }

  /**
   * \brief Set the minimum cycle number to start to display memory request
   */
  void set_mincycle(int c)
  { mincycle = c;
  }

  /**
   * \brief Set the step size between two cycles in pixels
   */
  void set_step(int s)
  { step = s;
  }

  /**
   * \brief Set if arrow should be skewed
   */    
  void set_skewing(bool s)
  { skewing = s;
  }

  /**
   * \brief Add a new memory request (arrow) to the list
   */    
  void add(int cycle, const string &sender, const string &target, const string &label, const memreq_dataless<Instruction> &mr)
  { stringstream ss;
    ss << "[b]Cycle:[/b] " << cycle << "\n";
//    ss << "[b]Instr:[/b] 0x" << hex << mr.instr.cia << dec << ": " << mr.instr.disasm() << "\n";
    ss << "[b]Command:[/b] " << mr.command << ", ";
    ss << "[b]Type:[/b] " << mr.message_type << "\n";
    ss << "[b]Address:[/b] 0x" << hex << mr.address << dec << ", ";
    ss << "[b]Size:[/b] " << mr.size << "\n";
    ss << "[b]Req_sender:[/b] " << mr.req_sender->name() << "\n";
    if(mr.cachable) ss << "cachable\n";
    else ss << "not cachable\n";

    infos.push_back(CacheMessageInfo(cycle, sender, target, label, ss.str()));
//    if(cycle>maxcycle) maxcycle = cycle;
    if(cycle>maxcycle) maxcycle = ( ((cycle%5==0)?0:1) + (cycle/5) ) * 5;
//    if(cycle<mincycle) mincycle = cycle;
  }

  /**
   * \brief Add a new architecture component (box) to the list
   */    
  void add_box(int y, const string &color, const string &name)
  { boxes.push_back(CacheMessageBox(y, color, name));
    if(y>maxy) maxy = y;
  }
  
  /**
   * \brief Add a new comment on a architecture box to the list
   */
  void add_box_info(int cycle, const string &name, const string &info)
  { boxes_info.push_back(CacheMessageBoxInfo(cycle,name,info));
//    if(cycle>maxcycle) maxcycle = cycle;
    if(cycle>maxcycle) maxcycle = ( ((cycle%5==0)?0:1) + (cycle/5) ) * 5;
//    if(cycle<mincycle) mincycle = cycle;
  }
  
  void dump(char *filename)
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
    { (*it).dump_arrow(os, boxes, mincycle, step, skewing);
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
  
  /**
   * \brief Retuns the maximum cycle number
   */
  int get_max_cycle()
  { int result = maxcycle;
    return result;
  }
  
 private:
  list<CacheMessageInfo> infos;
  list<CacheMessageBox> boxes;
  list<CacheMessageBoxInfo> boxes_info;
  int mincycle;
  int maxcycle;
  int maxy;
  int step;
  bool skewing;

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

//    os << "  function on_arrow(evt)\n";
//    os << "  { var aro = evt.target;\n";
//    os << "    var txt_box = document.getElementById('txt').childNodes.item(0);\n";
//    os << "    var inf = aro.getElementsByTagName('infos').item(0).childNodes.item(0);\n";
//    os << "    if(inf)\n";
//    os << "    { txt_box.data = inf.data;\n";
//    os << "    }\n";
//    os << "  }\n";

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
    os << "<g style=\"font-size:20px;font-style:normal;font-variant:normal;font-weight:bold;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">\n";
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
  { //os << "<text id=\"txt\" x=\"50\" y=\"" << maxy+100 << "\" style=\"font-size:14px;font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;text-align:start;line-height:125%;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;font-family:Arial\">\n";
    //os << "hover on connections for details";
    //os << "</text>\n";
    os << "</svg>" << endl;
  }

  void dump_index(ostream &html)
  { int maxx = 70 + (1+maxcycle-mincycle) * step + 10 + 20; 
    html << "<html>\n";
    html << "<head>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "<style>\n";
    html << "#txt {border:1px solid black; position:absolute; left:50px; top:" << maxy+80 << "px; width:300px; height:100px; padding:5px; font-family:sans-serif}\n";
//    html << "div {border:1px solid red}\n";
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
};

CacheMessageSVG SVG;

#endif
