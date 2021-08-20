/***************************************************************************
   cache_coherency_protocol.h  -  Virtual class all coherency protocol inherits
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
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

#ifndef __CACHE_COHERENCY_PROTOCOL_H__
#define __CACHE_COHERENCY_PROTOCOL_H__

class CacheCoherencyProtocol
{public:
  enum cache_coherency_state_t
  { // --- moesi states --------------------
    state_UNKNOWN,
    state_MODIFIED,
    state_OWNER,
    state_EXCLUSIVE,
    state_SHARED,
    state_INVALID,
    // --- extra cisc states ---------------
    state_SHARED_SPECULATIVE,
    state_STALE,
    state_STALE_SPECULATIVE,
    state_MODIFIED_SPECULATIVE,
    state_MODIFIED_SPECULATIVE_FORWARDED,
    state_MODIFIED_SHARED_SPECULATIVE,
    state_MODIFIED_SHARED_SPECULATIVE_FORWARDED,
    state_MODIFIED_Commited
  };
  enum cache_coherency_transition_t
  { transition_PrRd_XX,         ///< Local Read, nothing to bus = Read Hit
    transition_PrWr_XX,         ///< Local Write, nothing to bus = Write hit
    transition_PrRd_BusRd_S,    ///< Local read, Shared answer from bus
    transition_PrRd_BusRd_NS,   ///< Local read, Non shared answer from bus
    transition_PrWr_BusRdX,     ///< Local Write, Invalidate on bus  
    transition_BusRd_Flush,
    transition_BusRdX_Flush
  };

  CacheCoherencyProtocol()
  { state = state_INVALID;
  }
    
  CacheCoherencyProtocol(cache_coherency_state_t _state)
  { state = _state;
  }
    
  virtual bool transition(cache_coherency_transition_t &trans, const string &_file, int _line, int _cpuid)=0;

  cache_coherency_state_t getState() const
  { return state;
  }

  void invalidate()
  { state = state_INVALID;
  }
  
  friend ostream & operator<<(ostream &os, const CacheCoherencyProtocol &p)
  { os << p.state;
    return os;
  }
  
  friend ostream & operator<<(ostream &os, cache_coherency_state_t state)
  { switch(state)
    { // --- moesi states --------------------
      case state_MODIFIED:                              os << "M";     break;
      case state_OWNER:                                 os << "O";     break;
      case state_EXCLUSIVE:                             os << "E";     break;
      case state_SHARED:                                os << "S";     break;
      case state_INVALID:                               os << "I";     break;
      // --- extra cisc states ---------------
      case state_SHARED_SPECULATIVE:                    os << "SSp";   break;
      case state_STALE:                                 os << "St";    break;
      case state_STALE_SPECULATIVE:                     os << "StSp";  break;
      case state_MODIFIED_SPECULATIVE:                  os << "MSp";   break;
      case state_MODIFIED_SPECULATIVE_FORWARDED:        os << "MSpF";  break;
      case state_MODIFIED_SHARED_SPECULATIVE:           os << "MSSp";  break;
      case state_MODIFIED_SHARED_SPECULATIVE_FORWARDED: os << "MSSpF"; break;
      case state_MODIFIED_Commited:                     os << "MC";    break;
      // -------------------------------------
      default:                                          os << "U";     break;
    }
    return os;
  }

  friend ostream & operator<<(ostream &os, cache_coherency_transition_t trans)
  { switch(trans)
    { case transition_PrRd_XX:        os << "PrRd_XX";        break;
      case transition_PrWr_XX:        os << "PrWr_XX";        break;
      case transition_PrRd_BusRd_S:   os << "PrRd_BusRd_S";   break;
      case transition_PrRd_BusRd_NS:  os << "PrRd_BusRd_NS";  break;
      case transition_PrWr_BusRdX:    os << "PrWr_BusRdX";    break;
      case transition_BusRd_Flush:    os << "BusRd_Flush";    break;
      case transition_BusRdX_Flush:   os << "BusRdX_Flush";   break;
      default:                        os << "UNKNOWN";        break;
    }
    return os;
  }
  
 protected:
  cache_coherency_state_t state;
};

#endif //__CACHE_COHERENCY_PROTOCOL_H__
