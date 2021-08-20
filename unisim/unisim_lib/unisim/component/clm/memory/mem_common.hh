/***************************************************************************
   mem_common.h  -  Defines memory primitives
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

#ifndef __UNISIM_COMPONENT_CLM_MEMORY_MEM_COMMON_HH__
#define __UNISIM_COMPONENT_CLM_MEMORY_MEM_COMMON_HH__

#include <iostream>
#include <iomanip>
#include <string.h>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace clm {
namespace memory {


using std::ostream;
using std::right;
using std::left;
using std::dec;
using std::hex;
using std::setw;
using std::setfill;

//using namespace std;

//typedef uint64_t address_t;
typedef uint32_t address_t;

enum endianess_t { big_endian, little_endian };


/**
 * \brief An endian-friendly byte array class
 */
template <int maxsize>
class ByteArray
{public:
  char buffer[maxsize];  ///< The buffer storing the array of byte

  /**
   * \brief Converts the byte array into a char * 
   */
  operator const char * () const
  { return buffer;
  }
  
  /**
   * \brief Creates a new ByteArray, filled with 0.
   */
  ByteArray()
  { memset(buffer, 0, sizeof(buffer));
  }

  /**
   * \brief Compares two byte arrays
   *
   * Always returns false
   */
  int operator == (const ByteArray<maxsize>& ba) const
  { return 0;
  }

  /**
   * \brief ByteArray copy constructor
   */
  ByteArray<maxsize>& operator = (const ByteArray<maxsize>& ba)
  { memcpy(buffer,ba.buffer,maxsize);
    return *this;
  }

  /**
   * \brief ByteArray pretty printer
   */
  friend ostream& operator << (ostream& os, const ByteArray<maxsize>& ba)
  { os << "{" << right << setfill('0');
    for(int i = 0; i < maxsize; i++)
    { unsigned int c = (unsigned char)ba.buffer[i];
      os << hex << setw(2) << c;
      if(i < maxsize - 1) os << ",";
    }
    os << "}" << setfill(' ') << dec << left;
    return os;
  }

  /**
   * Read a 8-bit unsigned value from the ByteArray
   */
  void Read(uint8_t& v, int offset, endianess_t endianess) const
  { v = buffer[offset];
  }

  /**
   * Read a 16-bit unsigned value from the ByteArray
   */
  void Read(uint16_t& v, int offset, endianess_t endianess) const
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if(endianess == big_endian)
    { v = ((uint16_t)(unsigned char) buffer[offset] << 8) |
          (uint16_t)(unsigned char) buffer[offset + 1];
    }
#else
    if(endianess == little_endian)
    { v = (uint16_t)(unsigned char) buffer[offset] |
          ((uint16_t)(unsigned char) buffer[offset + 1] << 8);
    }
#endif
    else
    { v = *(uint16_t *)(buffer + offset);
    }
  }

  /**
   * Read a 32-bit unsigned value from the ByteArray
   */
  void Read(uint32_t& v, int offset, endianess_t endianess) const
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if(endianess == big_endian)
    { v = ((uint32_t)(unsigned char) buffer[offset]     << 24) |
          ((uint32_t)(unsigned char) buffer[offset + 1] << 16) |
          ((uint32_t)(unsigned char) buffer[offset + 2] << 8)  |
          (uint32_t)(unsigned char) buffer[offset + 3];
    }
#else
    if(endianess == little_endian)
    { v =  (uint32_t)(unsigned char) buffer[offset]            |
           ((uint32_t)(unsigned char) buffer[offset + 1] << 8)  |
           ((uint32_t)(unsigned char) buffer[offset + 2] << 16) |
           ((uint32_t)(unsigned char) buffer[offset + 3] << 24);
    }
#endif
    else
    { v = *(uint32_t *)(buffer + offset);
    }
  }

  /**
   * Read a 64-bit unsigned value from the ByteArray
   */
  void Read(uint64_t& v, int offset, endianess_t endianess) const
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if(endianess == big_endian)
    { v = ((uint64_t)(unsigned char) buffer[offset]     << 56) |
          ((uint64_t)(unsigned char) buffer[offset + 1] << 48) |
          ((uint64_t)(unsigned char) buffer[offset + 2] << 40) |
          ((uint64_t)(unsigned char) buffer[offset + 3] << 32) |
          ((uint64_t)(unsigned char) buffer[offset + 4] << 24) |
          ((uint64_t)(unsigned char) buffer[offset + 5] << 16) |
          ((uint64_t)(unsigned char) buffer[offset + 6] << 8)  |
          (uint64_t)(unsigned char) buffer[offset + 7];
    }
#else
    if(endianess == little_endian)
    { v =  (uint64_t)(unsigned char) buffer[offset]            |
           ((uint64_t)(unsigned char) buffer[offset + 1] << 8)  |
           ((uint64_t)(unsigned char) buffer[offset + 2] << 16) |
           ((uint64_t)(unsigned char) buffer[offset + 3] << 24) |
           ((uint64_t)(unsigned char) buffer[offset + 4] << 32) |
           ((uint64_t)(unsigned char) buffer[offset + 5] << 40) |
           ((uint64_t)(unsigned char) buffer[offset + 6] << 48) |
           ((uint64_t)(unsigned char) buffer[offset + 7] << 56);
    }
#endif
    else
    { v = *(uint64_t *)(buffer + offset);
    }
  }

  /**
   * Write the 8-bit unsigned value to the ByteArray
   */
  void Write(uint8_t v, int offset, endianess_t endianess)
  { buffer[offset] = v;
  }

  /**
   * Write the 16-bit unsigned value to the ByteArray
   */
  void Write(uint16_t v, int offset, endianess_t endianess)
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if(endianess == big_endian)
    { buffer[offset]     = v >> 8;
      buffer[offset + 1] = v;
    }
#else
    if(endianess == little_endian)
    { buffer[offset]     = v;
      buffer[offset + 1] = v >> 8;
    }
#endif
    else
    { *(uint16_t *)(buffer + offset) = v;
    }
  }

  /**
   * Write the 32-bit unsigned value to the ByteArray
   */
  void Write(uint32_t v, int offset, endianess_t endianess)
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if(endianess == big_endian)
    { buffer[offset]     = v >> 24;
      buffer[offset + 1] = v >> 16;
      buffer[offset + 2] = v >> 8;
      buffer[offset + 3] = v;
    }
#else
    if(endianess == little_endian)
    { buffer[offset]     = v;
      buffer[offset + 1] = v >> 8;
      buffer[offset + 2] = v >> 16;
      buffer[offset + 3] = v >> 24;
    }
#endif
    else
    { *(uint32_t *)(buffer + offset) = v;
    }
  }

  /**
   * Write the 64-bit unsigned value to the ByteArray
   */
  void Write(uint64_t v, int offset, endianess_t endianess)
  {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    if(endianess == big_endian)
    { buffer[offset]     = v >> 56;
      buffer[offset + 1] = v >> 48;
      buffer[offset + 2] = v >> 40;
      buffer[offset + 3] = v >> 32;
      buffer[offset + 4] = v >> 24;
      buffer[offset + 5] = v >> 16;
      buffer[offset + 6] = v >> 8;
      buffer[offset + 7] = v;
    }
#else
    if(endianess == little_endian)
    { buffer[offset]     = v;
      buffer[offset + 1] = v >> 8;
      buffer[offset + 2] = v >> 16;
      buffer[offset + 3] = v >> 24;
      buffer[offset + 4] = v >> 32;
      buffer[offset + 5] = v >> 40;
      buffer[offset + 6] = v >> 48;
      buffer[offset + 7] = v >> 56;
    }
#endif
    else
    { *(uint64_t *)(buffer + offset) = v;
    }
  }
};


enum WritePolicyType       { writethrough, writeback };
enum ReplacementPolicyType { randomReplacementPolicy, lruReplacementPolicy, pseudoLRUReplacementPolicy };
enum AllocationPolicyType  { allocateOnWritePolicy, nonAllocateOnWritePolicy };

} // end of namespace memory
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
