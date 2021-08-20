/***************************************************************************
   common.h  -  Defines common enums and functions
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Gilles Mouchard (gilles.mouchard@cea.fr) 
         Sylvain Girbal (sylvain.girbal@inria.fr)
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

#ifndef __UNISIM_COMPONENT_CLM_UTILITY_COMMON_HH__
#define __UNISIM_COMPONENT_CLM_UTILITY_COMMON_HH__

#include <unisim/component/clm/utility/utility.hh>
#include <unisim/component/clm/memory/mem_common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace utility {


#if defined(__GNUC__) && defined(EXTENSIVE_INLINING)
#define GCC_INLINE __attribute__((always_inline))
#define INLINE inline
#else
#define GCC_INLINE
#define INLINE inline
#endif

//enum reg_type_t { NONE_REGISTER, INTEGER_REGISTER, FLOATING_POINT_REGISTER};

enum exception_type_t { NONE_EXCEPTION, MISALIGNMENT_EXCEPTION, INVALID_OPCODE_EXCEPTION };

enum branch_direction_t { Taken, NotTaken, DontKnow };

INLINE const char *get_branch_direction_name(branch_direction_t branch_direction)
{ switch(branch_direction)
  { case Taken:
      return "taken";
    case NotTaken:
      return "not taken";
    case DontKnow:
      return "don't know";
  }
  return "error";
}

INLINE const char *get_exception_name(exception_type_t exception)
{ switch(exception)
  { case NONE_EXCEPTION:
      return "None";
    case MISALIGNMENT_EXCEPTION:
      return "Misalignment";
    case INVALID_OPCODE_EXCEPTION:
      return "Invalid Opcode";
  }
  return "?";
}

INLINE string hexa(uint8_t v)
{ static char buffer[5]={0};
  sprintf(buffer, "0x%02x", (v & 0xff));
  return buffer;
}

INLINE string hexa(int8_t v)
{ static char buffer[5]={0};
  sprintf(buffer, "0x%02x", (v & 0xff));
  return buffer;
}

INLINE string hexa(char v)
{ static char buffer[5]={0};
  sprintf(buffer, "0x%02x", (v & 0xff));
  return buffer;
}

INLINE string hexa(uint16_t v)
{ static char buffer[7]={0};
  sprintf(buffer, "0x%04hx", v);
  return buffer;
}

INLINE string hexa(int16_t v)
{ static char buffer[7]={0};
  sprintf(buffer, "0x%04hx", v);
  return buffer;
}

INLINE string hexa(uint32_t v)
{ static char buffer[11]={0};
  sprintf(buffer, "0x%08x", v);
  return buffer;
}

INLINE string hexa(int32_t v)
{ static char buffer[11]={0};
  sprintf(buffer, "0x%08x", v);
  return buffer;
}

INLINE string hexa(uint64_t v)
{ static char buffer[19]={0};
  sprintf(buffer, "0x%016llx", (long long unsigned int)v);
  return buffer;
}

INLINE string hexa(int64_t v)
{ static char buffer[19]={0};
  sprintf(buffer, "0x%016llx", (long long unsigned int)v);
  return buffer;
}

INLINE void print_buffer(ostream &os, const void *buffer, size_t size)
{ char *buf = (char*)buffer;
  os << hex << right << setfill('0');
  for(int i=0; i<size; i++)
  { uint32_t val = (uint8_t)buf[i];
    os << setw(2) << val;
    if(i%4==3) os << " ";
  }
  os << dec << left << setfill(' ') << endl;
}

#undef GCC_INLINE

} // end of namespace utility
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
