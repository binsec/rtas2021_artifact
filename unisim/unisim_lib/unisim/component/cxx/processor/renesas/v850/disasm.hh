/*
 *  Copyright (c) 2017,
 *  Commissariat a l'Energie Atomique (CEA)
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

/**************************************************************/
/* Disassembling methods                                      */
/**************************************************************/

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_RENESAS_V850_DISASM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_RENESAS_V850_DISASM_HH__

#include <ostream>
#include <iomanip>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace renesas {
namespace v850 {
  
  struct PrintReg
  {
    PrintReg(int _reg) : reg(_reg) {} int reg;
    friend std::ostream& operator << (std::ostream& sink, PrintReg const& pr)
    {
      switch(pr.reg)
        {
        case 3:  sink << "sp"; break;
        case 4:  sink << "gp"; break;
        case 30: sink << "ep"; break;
        case 31: sink << "lp"; break;
        default: sink << 'r' << std::dec << pr.reg; break;
        }
      return sink;
    }
  };

  struct PrintSReg
  {
    PrintSReg(int _sreg) : sreg(_sreg) {} int sreg;
    friend std::ostream& operator << (std::ostream& sink, PrintSReg const& pr)
    {
      switch(pr.sreg)
        {
        case  0: sink << "eipc/vip/mpm"; break;
        case  1: sink << "eipsw/mpc"; break;
        case  2: sink << "fepc/tid"; break;
        case  3: sink << "fepsw/ppa"; break;
        case  4: sink << "ecr/vmecr"; break;
        case  5: sink << "psw/vmtid"; break;
        case  6: sink << "sr6/fpsr/vmadr/dcc"; break;
        case  7: sink << "sr7/fpepc/dc0"; break;
        case  8: sink << "sr8/fpst/vpecr/dcv1"; break;
        case  9: sink << "sr9/fpcc/vptid"; break;
        case 10: sink << "sr10/fpcfg/vpadr/spal"; break;
        case 11: sink << "sr11/spau"; break;
        case 12: sink << "sr12/vdecr/ipa0l"; break;
        case 13: sink << "eiic/vdtid/ipa0u"; break;
        case 14: sink << "feic/ipa1l"; break;
        case 15: sink << "dbic/ipa1u"; break;
        case 16: sink << "ctpc/ipa2l"; break;
        case 17: sink << "ctpsw/ipa2u"; break;
        case 18: sink << "dbpc/ipa3l"; break;
        case 19: sink << "dbpsw/ipa3u"; break;
        case 20: sink << "ctbp/dpa0l"; break;
        case 21: sink << "dir/dpa0u"; break;
        case 22: sink << "bpc/dpa0u"; break;
        case 23: sink << "asid/dpa1l"; break;
        case 24: sink << "bpav/dpa1u"; break;
        case 25: sink << "bpam/dpa2l"; break;
        case 26: sink << "bpdv/dpa2u"; break;
        case 27: sink << "bpdm/dpa3l"; break;
        case 28: sink << "eiwr/dpa3u"; break;
        case 29: sink << "fewr"; break;
        case 30: sink << "dbwr"; break;
        case 31: sink << "bsel"; break;
        default: sink << "sr" << pr.sreg; break;
        }
      return sink;
    }
  };


  struct PrintImm
  {
    PrintImm(int _imm) : imm(_imm) {} int imm;
    friend std::ostream& operator << (std::ostream& sink, PrintImm const& pr)
    {
      switch(pr.imm)
        {
        default: sink << std::dec << pr.imm; break;
        }
      return sink;
    }
  };

  struct PrintHex
  {
    PrintHex(int _hex) : hex(_hex) {} int hex;
    friend std::ostream& operator << (std::ostream& sink, PrintHex const& pr)
    {
      switch(pr.hex)
        {
        default: sink << "0x" << std::hex << pr.hex; break;
        }
      return sink;
    }
  };

  struct PrintBranchCond
  {
    PrintBranchCond(int _cond) : cond(_cond) {} int cond;
    friend std::ostream& operator << (std::ostream& sink, PrintBranchCond const& pr)
    {
      if (pr.cond & -16)
        throw 0;
      static char const* condnames[] = {"v", "l", "e", "nh", "n", "r", "lt", "le", "nv", "nl", "ne", "h", "p", "sa", "ge", "gt"};
      sink << condnames[pr.cond];
      return sink;
    }
  };

  struct PrintCond
  {
    PrintCond(int _cond) : cond(_cond) {} int cond;
    friend std::ostream& operator << (std::ostream& sink, PrintCond const& pr)
    {
      if (pr.cond & -16)
        throw 0;
      static char const* condnames[] = {"v", "c/l", "z", "nh", "s/n", "t", "lt", "le", "nv", "nc/nl", "nz", "h", "ns/p", "sa", "ge", "gt"};
      sink << condnames[pr.cond];
      return sink;
    }
  };

} // end of namespace v850
} // end of namespace renesas
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
  

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_RENESAS_V850_DISASM_HH__
