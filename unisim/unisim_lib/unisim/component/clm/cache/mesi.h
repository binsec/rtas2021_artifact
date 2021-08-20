/***************************************************************************
   mesi.h  -  Defines the MESI class implementing the MESI protocol
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
         David Parello (david.parello@univ-perp.fr)
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

#ifndef __MESI_H__
#define __MESI_H__

#include <unisim/component/clm/cache/cache_coherency_protocol.hh>

class MESI: public CacheCoherencyProtocol
{public:
  MESI() : CacheCoherencyProtocol()
  {
  }
  
  MESI(cache_coherency_state_t mesi_state) : CacheCoherencyProtocol(mesi_state)
  {
  }
  
  virtual bool transition(cache_coherency_transition_t &trans, const string &_file, int _line, int _cpuid)
  { cache_coherency_state_t last_state = state;
    switch(state)
    { case state_MODIFIED:
        switch(trans)
        { case transition_PrRd_XX:           /* no change */            break;
          case transition_PrWr_XX:           /* no change */            break;
          case transition_BusRd_Flush:       state = state_SHARED;      break;
          case transition_BusRdX_Flush:      state = state_INVALID;     break;
          default:
            cerr << __FILE__ << ":" << __LINE__ << ": error: Forbidden transition in the mesi protocol: From state '" << state << "' with transition " << trans << " in " << _file << " at line " << _line << " at cycle " << timestamp()  << endl;
            return true;
        }
        break;
      case state_EXCLUSIVE:
        switch(trans)
        { case transition_PrRd_XX:         /* no change */            break;
          case transition_PrWr_XX:         state = state_MODIFIED;    break;
          case transition_BusRd_Flush:     state = state_SHARED;      break;
          case transition_BusRdX_Flush:    state = state_INVALID;     break;
          default:
            cerr << __FILE__ << ":" << __LINE__ << ": error: Forbidden transition in the mesi protocol: From state '" << state << "' with transition " << trans << " in " << _file << " at line " << _line << " at cycle " << timestamp()  << endl;
            return true;
        }
        break;
      case state_SHARED:
        switch(trans)
        { case transition_PrRd_XX:         /* no change */            break;
          case transition_PrWr_BusRdX:     state = state_MODIFIED;    break;
          case transition_BusRd_Flush:     /* no change */            break;
          case transition_BusRdX_Flush:    state = state_INVALID;     break;
          default:
            cerr << __FILE__ << ":" << __LINE__ << ": error: Forbidden transition in the mesi protocol: From state '" << state << "' with transition " << trans << " in " << _file << " at line " << _line << " at cycle " << timestamp()  << endl;
            return true;
        }
        break;
      case state_INVALID:
        switch(trans)
        { case transition_PrRd_BusRd_NS:   state = state_EXCLUSIVE;   break;
          case transition_PrRd_BusRd_S:    state = state_SHARED;      break;
          case transition_PrWr_BusRdX:     state = state_MODIFIED;    break;
          default:
            cerr << __FILE__ << ":" << __LINE__ << ": error: Forbidden transition in the mesi protocol: From state '" << state << "' with transition " << trans << " in " << _file << " at line " << _line << " at cycle " << timestamp()  << endl;
            return true;
        }
        break;
      default:
        cerr << __FILE__ << ":" << __LINE__ << ": error: Unknown state in the mesi protocol: From state '" << state << "' with transition " << trans << " in " << _file << " at line " << _line << " at cycle " << timestamp() << endl;
        exit(1);
    }
//    cerr << "\e[1;33mP" << _cpuid << ": " << last_state << " --- " << trans << " --> " << state << "\e[0m" << endl;
    return false;
  }

  
};

#endif //__MESI_H__
