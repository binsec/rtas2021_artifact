/***************************************************************************
   CacheContainer.h  -  Defines the CacheContainer class
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Daniel Gracia P�rez (daniel.gracia-perez@cea.fr)
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

#ifndef __UNISIM_COMPONENT_CLM_CACHE_CACHE_CONTAINER_HH__
#define __UNISIM_COMPONENT_CLM_CACHE_CACHE_CONTAINER_HH__

#include <unisim/component/clm/utility/utility.hh>
#include <unisim/component/clm/memory/mem_common.hh>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
//#include <unisim/component/clm/utility/error.h>
#include <unisim/component/clm/cache/cache_coherency_protocol.hh>
#include <unisim/component/clm/cache/mesi.h>


using unisim::component::clm::memory::ReplacementPolicyType;
using unisim::component::clm::memory::randomReplacementPolicy;
using unisim::component::clm::memory::lruReplacementPolicy;
using unisim::component::clm::memory::pseudoLRUReplacementPolicy;
using unisim::component::clm::memory::address_t;

#define error( format, ...) error_func( __LINE__, __FUNCTION__, __FILE__, format, ## __VA_ARGS__)

/* get a random number */
INLINE int myrand()                    /* returns random number */
{
#if !defined(__alpha) && !defined(linux) && !defined(__linux) && !defined(__linux__) && !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64) && !defined(__APPLE_CC__)
  extern long random(void);
#endif

#if defined(__CYGWIN32__) || defined(hpux) || defined(__hpux) || defined(__svr4__) || defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) || defined(__APPLE_CC__)
  return rand();
#else
  return random();
#endif
}


/* A class that implements a generic cache */
/* nCacheLines: total number of cache lines contained (should be bigger than 0, <0 == error) */
/* nLineSize: the size in bytes of each cache line (total cache size=nLineSize*nCacheLines bytes) */
/* nAssociativity: associativity of the cache (<=0 == error) */
/* replacementPolicy: the replacement policy to apply (lru, random, pseudoLRU) */
/* nPseudoLRUHistorySize: the size of the pseudo lru */
template 
< int nCacheLines,
  int nLineSize,
  int nAssociativity,
  ReplacementPolicyType replacementPolicy,
  int nPseudoLRUHistorySize 
>
class CacheContainer
{public:
  /**
   * Class constructor 
   */
void error_func(const int line, const char *funcname, const char *filename, const char *format, ...)
{ static char buffer[1024];
  va_list arg;
  va_start(arg, format);
  vsprintf( buffer, format, arg);
  fprintf(stderr, "%s:%s:%d\tERROR:%s", filename, funcname, line, buffer);
  fflush(stderr);
  va_end(arg);
  exit(-1);
}



  CacheContainer()
  { if(nCacheLines < 1) error("nCacheLines should be bigger than 0");
    if(nAssociativity < 1) error("nAssociativity should be bigger than 0");
    if(nLineSize < 1) error("nLineSize should be bigger than 0");
    for(int i = 0; i < (nCacheLines / nAssociativity); i++)
    { for(int j = 0; j < nAssociativity; j++)
      { cvalidity[i][j] = false;
        cwrite[i][j] = false;
        clock[i][j] = false;
        prefetched[i][j] = false;
        counter[i][j] = 0;
      }
    }
    /* setMask and setShift are calculated */
    setMask = (nCacheLines / nAssociativity) - 1;
    int tmp1;
    tmp1 = nLineSize;
    setShift = 0;
    while(tmp1 >>= 1)
    { setShift++;
    }
    /* blkMask */
    blkMask = nLineSize - 1;
    /* tagMask and tagShift are calculated */
    tagShift = setShift;
    tmp1 = (nCacheLines / nAssociativity);
    while( tmp1 >>= 1 )
      tagShift++;
    tagMask = (((address_t)1) << (64 - tagShift)) - 1;
    tagSetMask = ~blkMask;

    switch(replacementPolicy)
    { case randomReplacementPolicy:
        break;
      case lruReplacementPolicy:
        break;
    }
    last_dump = "";
  }

  /**
   * \brief Invalidate all the cache lines
   */
  void InvalidateAll(){
    for(int i = 0; i < (nCacheLines / nAssociativity); i++)
    { for(int j = 0; j < nAssociativity; j++)
      { cvalidity[i][j] = false;
        cwrite[i][j] = false;
        clock[i][j] = false;
        prefetched[i][j] = false;
        counter[i][j] = 0;
      }
    }
  }

  /**
   * \brief Pretty printer printing one cache line
   */
  void dump(ostream & os, int i, int j) const
  { char buf[nLineSize];   

    os << setw(5) << i << setw(6) << j;

    os << "0x" << hex << right << setfill('0') << setw(8) << getAddress(i,j) << dec << left << setfill(' ') << "  ";

    os << "0x" << hex << right << setw(16) << setfill('0') << caddress[i][j] << dec << left << setfill(' ') << "  ";
    os << setw(7);
    if(getValid(i, j)) os << "T";
    else                  os << "F";
    os << setw(7);
    if(getWrite(i, j)) os << "T";
    else                  os << "F";
    os << getMESI(i, j) << "     ";
    getCacheLine(buf, i, j);
    print_buffer(os,buf,nLineSize);
  }
   
  /**
   * \brief Pretty printer printing the valid cache lines
   */
  friend ostream& operator << (ostream& os, CacheContainer <nCacheLines, nLineSize, nAssociativity, replacementPolicy, nPseudoLRUHistorySize>& cc)
  { stringstream ss;
    char buf[nLineSize];   
    ss << left << "Set  Line  Addr        Tag                 Valid  Dirty  MESI  Data" << endl;
    for(int i = 0; i < (nCacheLines / nAssociativity); i++)
    { for(int j = 0; j < nAssociativity; j++)
      { if( cc.getValid(i, j) || (cc.getMESI(i, j).getState()!=MESI::state_INVALID) )
#ifdef CACHE_DUMP_ONLY_SET
if(i==CACHE_DUMP_ONLY_SET)
#endif
#ifdef CACHE_DUMP_ONLY_SET_MIN
if(i>=CACHE_DUMP_ONLY_SET_MIN)
#endif
#ifdef CACHE_DUMP_ONLY_SET_MAX
if(i<=CACHE_DUMP_ONLY_SET_MAX)
#endif
#ifdef CACHE_DUMP_ONLY_ADDRESS
if(cc.getAddress(i,j)==CACHE_DUMP_ONLY_ADDRESS)
#endif
#ifdef CACHE_DUMP_ONLY_ADDRESS_MIN
if(cc.getAddress(i,j)>=CACHE_DUMP_ONLY_ADDRESS_MIN)
#endif
#ifdef CACHE_DUMP_ONLY_ADDRESS_MAX
if(cc.getAddress(i,j)<=CACHE_DUMP_ONLY_ADDRESS_MAX)
#endif

        { // Displays valid cache lines
          ss << setw(5) << i << setw(6) << j;

          ss << "0x" << hex << right << setfill('0') << setw(8) << cc.getAddress(i,j) << dec << left << setfill(' ') << "  ";

          ss << "0x" << hex << right << setw(16) << setfill('0') << cc.caddress[i][j] << dec << left << setfill(' ') << "  ";
          ss << setw(7);
          if(cc.getValid(i, j)) ss << "T";
          else                  ss << "F";
          ss << setw(7);
          if(cc.getWrite(i, j)) ss << "T";
          else                  ss << "F";
          ss << cc.getMESI(i, j) << "     ";
          cc.getCacheLine(buf, i, j);
          ss << hex << setfill('0') << right;
          for(int k=0;k<nLineSize;k++)
          { unsigned int byte = (unsigned char)buf[k];
            ss << setw(2) << byte;
            if(k%4==3) ss << " ";
          }
          ss << dec << setfill(' ') << left;
          ss << "\n";
        }
      }
    }
    if(cc.last_dump != ss.str())
    { os << "\n";
      os << setfill('-') << setw(50+2*nLineSize+nLineSize/4) << "-" << endl;
      os << "  Cycle " << left << timestamp() << endl;
      os << setfill('-') << setw(50+2*nLineSize+nLineSize/4) << "-" << endl;
      os << ss.str();
      os << setfill('-') << setw(50+2*nLineSize+nLineSize/4) << "-" << endl;
      cc.last_dump = ss.str();
    }
//    os << "SetMask: " << hexa(cc.setMask) << "\tSetShift: " << cc.setShift << endl;
//    os << "TagMask: " << hexa(cc.tagMask) << "\tTagShift: " << cc.tagShift << endl;
    return os;
  }

  /**
   * \brief Returns true if all the cache lines are invalid
   */
  bool all_invalid()
  { for(int i = 0; i < (nCacheLines / nAssociativity); i++)
    { for(int j = 0; j < nAssociativity; j++)
      { if(getValid(i, j)) return false;
      }
    }
    return true;
  }

  /**
   * \brief Returns the set where "address" can be found 
   */
  int getSet(uint64_t address) const
  { return (int)((address >> setShift) & setMask);
  }

  /**
   * \brief Returns the line where "address" can be found 
   */
  int getLine(uint64_t address) const
  { int _set = getSet(address);
    for(int i = 0; i < nAssociativity; i++)
    { if(((address >> tagShift) & tagMask) == caddress[_set][i])
      { if(cvalidity[_set][i]) return i;
      }
    }
    cerr << __PRETTY_FUNCTION__ << "\e[1;31m error: cache line for address 0x" << hex << address << dec << "llx has not been found\e[0m\n" << endl;
    return 0;
  }

  /**
   * \brief Returns true if "address" can be found in the cache 
   */
  bool hit(uint64_t address) const
  { int _set;

    /* get the set where the address can be found */
    _set = getSet(address);
    /* check if the address is in the set */
    for(int i = 0; i < nAssociativity; i++)
    { /* if address found in the set and valid return true */
      if(cvalidity[_set][i])
      { if(((address >> tagShift) & tagMask) == caddress[_set][i]) return true;
      }
    }
    /* the address was not found, return true */
    return false;
  }

  /**
   * \brief Returns the replacement set for the provided address
   */
  int getReplaceSet(uint64_t address)
  { return getSet(address);
  }

  /**
   * \brief Returns the replacement line for the provided address
   */
  int getReplaceLine(uint64_t address)
  { /* using any of the cache policies a line can be selected if it is not filled
     * before applying the replacement policy */
    int _set = getSet(address);
    for(int i = 0; i < nAssociativity; i++)
    { if(!cvalidity[_set][i]) return i;
    }
    switch(replacementPolicy)
    { case randomReplacementPolicy:
        return myrand() % nAssociativity;
        break;
      case lruReplacementPolicy:
        return *lruHistory[_set].SeekAtHead();
        break;
      case pseudoLRUReplacementPolicy:
        int _index = 0;
        int _line = 0;
        int _size = nAssociativity;
        for(int _i = 1; _i <= nPseudoLRUHistorySize; _i = _i + (_i * 2))
        { if(pseudoLRUHistory[_set][_index])
          { _index = (_index * 2) + 1;
            _size = _size / 2;
          }
          else
          { _index = (_index * 2) + 2;
            _size = _size / 2;
            _line = _line + _size;
          }
        }
        if(_size == 1)
        { return _line;
        }
        else
        { _line = _line + (myrand() % _size);
          return _line;
        }

        break;
    }
  }

  /**
   * \brief Returns true it the set/line is locked
   */
  bool IsLocked(int _set, int _line) const
  { return clock[_set][_line];
  }

  /**
   * \brief Lock the set/line
   */
  void Lock(int _set, int _line)
  { clock[_set][_line] = true;
  }

  /**
   * \brief Unlock the set/line
   */
  void Unlock(int _set, int _line)
  { clock[_set][_line] = false;
  }

  /**
   * \brief Get the address corresponding to the set/line
   */
  address_t getAddress(int _set, int line) const
  { return ((caddress[_set][line] << tagShift) + (_set << setShift));
  }

  /**
   * \brief Get the data corresponding to the set/line
   */
  void getCacheLine(char *buf, int _set, int line) const
  { memcpy(buf, cdata[_set][line], nLineSize);
  }

  /**
   * \brief Get a part of the data corresponding to the set/line
   */
  void getCacheLineIndexed(char *_buf, int _set, int _line, int _index, int _size)
  { memcpy(_buf, &(cdata[_set][_line][_index]), _size);
  }

  /**
   * \brief Set a cache line setting the data, validity, modified bits
   */
  void setCacheLine(int _set, int line, address_t address, char *buf, bool valid, bool write)
  { caddress[_set][line] = (address >> tagShift) & tagMask;
    memcpy(cdata[_set][line], buf, nLineSize);
    cvalidity[_set][line] = valid;
    cwrite[_set][line] = write;
  }

  /**
   * \brief Set a cache line setting the data, validity, modified & prefetch bits
   */
  void setCacheLine(int _set, int line, address_t address, char *buf, bool valid, bool write, bool prefetch)
  { setCacheLine(_set, line, address, buf, valid, write);
    prefetched[_set][line] = prefetch;
  }

  /**
   * Set a part of the cache line setting the data, valid and modified bits
   */
  void setCacheLineIndexed(int _set, int line, const char *buf, int index, int size, bool write)
  { memcpy((cdata[_set][line]) + index, buf, size);
    cwrite[_set][line] = write;
  }

  /**
   * \brief Returns the last replaced line data
   */
  void getReplacedLine(char *buf)
  { memcpy(buf, replacedLine, nLineSize);
  }

  /**
   * \brief Returns the last replaced line address
   */
  address_t getReplacedAddress()
  { return replacedAddress;
  }

  /**
   * \brief Returns the last replaced line valid bit
   */
  bool getReplacedValid()
  { return replacedValid;
  }

  /**
   * \brief Returns the last replaced line modified bit
   */
  bool getReplacedWrite()
  { return replacedWrite;
  }

  /**
   * \brief Set the last replaced line valid bit
   */
  void setReplacedValid(bool valid)
  { replacedValid = valid;
  }

  /**
   * \brief Returns true if missLine data is valid
   */
  bool getMissValid() const
  { return missValid;
  }

  /**
   * \brief Set the validity of the missLine
   */
  void setMissValid(bool valid)
  { missValid = valid;
  }

  /**
   * \brief Returns true if missLine has been modified 
   */
  bool getMissWrite()
  { return missWrite;
  }

  /**
   * \brief Set the modified state of the missLine
   */
  void setMissWrite(bool write)
  { missWrite = write;
  }

  /**
   * \brief Returns the address of the missLine
   */
  address_t getMissAddress() const
  { return missAddress;
  }

  /**
   * \brief Set the missLine address
   */
  void setMissAddress(address_t addr)
  { missAddress = addr;
  }

  /**
   * \brief Returns the missLine data
   */
  void getMissLine(char *buf)
  { memcpy(buf, missLine, nLineSize);
  }

  /**
   * \brief Set the missLine data
   */
  void setMissLine(char *buf, int index, int size)
  { memcpy(missLine + index, buf, size);
  }

  /**
   * \brief Set the validity of the set/line
   */
  void setValid(int _set, int line, bool valid)
  { cvalidity[_set][line] = valid;
  }

  /**
   * \brief Returns true if the target set/line is valid
   */
  bool getValid(int _set, int line) const
  { return cvalidity[_set][line];
  }

  /**
   * \brief Get the MESI state of the set/line
   */
  const MESI & getMESI(int _set, int line) const
  { return cmesi[_set][line];
  }
  
  /**
   * \brief Apply a MESI transition
   */
  bool mesi_transition(int _set, int line, CacheCoherencyProtocol::cache_coherency_transition_t trans, const string &_file, int _line, int _cpuid)
  { return cmesi[_set][line].transition(trans,_file,_line,_cpuid);
  }

  /**
   * \brief Set the MESI state of the set/line to invalid
   */
  void mesi_invalidate(int _set, int _line)
  { cmesi[_set][_line].invalidate();
  }

  /**
   * \brief Set the MESI state of the set/line
   */
  void setMESI(int _set, int line, const MESI &mesi)
  { cmesi[_set][line]=mesi;
  }

  /**
   * \brief Set the modified state of the set/line
   */
  void setWrite(int _set, int line, bool write)
  { cwrite[_set][line] = write;
  }

  /**
   * \brief Returns true if the target set/line is modified
   */
  bool getWrite(int _set, int line) const
  { return cwrite[_set][line];
  }

  /**
   * \brief Set the prefetched state of the set/line
   */
  void setPrefetch(int _set, int line, bool prefetch)
  { prefetched[_set][line] = prefetch;
  }

  /**
   * \brief Returns true if the target set/line is prefetched
   */
  bool getPrefetch(int _set, int line)
  { return prefetched[_set][line];
  }

  /**
   * \brief Access to the LRU history
   */
  void historyAccess(int _set, int line)
  { OoOQueuePointer<int, nAssociativity > curline;
    int *new_line;
    switch(replacementPolicy)
    { case lruReplacementPolicy:
        for(curline = lruHistory[_set].SeekAtHead();
            curline && *curline != line;
            curline++)
          ;
        if(curline) lruHistory[_set].Remove(curline);
        new_line = lruHistory[_set].New();
        *new_line = line;
        break;
      case randomReplacementPolicy:
        /* nothing to be done */
        break;
    }
  }

  /**
   * \brief Remove an entry from the LRU history
   */
  void historyRemove(int _set, int line)
  { OoOQueuePointer<int, nAssociativity > curline;
    switch(replacementPolicy)
    {
        case lruReplacementPolicy:
        for(curline = lruHistory[_set].SeekAtHead();
            curline && *curline != line;
            curline++)
          ;
        if(curline) lruHistory[_set].Remove(curline);
        break;
        case randomReplacementPolicy:
        /* nothing to be done */
        break;
    }
  }

  /**
   * \brief Perform a unchecked read to the cache line
   */
  void UncheckRead(void *buffer, address_t address, int size)
  { int set, line;
    /* get the set where the address can be found */
    if(hit(address))
    { set = getSet(address);
      line = getLine(address);
      getCacheLineIndexed((char *)buffer, set, line, address & (nLineSize - 1), size);
    }
  }

  /**
   * \brief Memory interface : Read from memory
   */
  void Read(void *buffer, address_t address, int size)
  { if(size > 0)
    { char *buf = (char *) buffer;
      address_t offset = address & (nLineSize - 1);
      int sz = nLineSize - offset;
      if(size > sz)
      { UncheckRead(buf, address, sz);
        size -= sz;
        address += sz;
        buf += sz;

        if(size >= nLineSize)
        { do
          { UncheckRead(buf, address, nLineSize);
            size -= nLineSize;
            address += nLineSize;
            buf += nLineSize;
          }
          while(size >= nLineSize);
        }

        if(size > 0)
        { UncheckRead(buf, address, size);
        }
      }
      else
      { UncheckRead(buf, address, size);
      }
    }
  }

  /**
   * \brief Perform a unchecked write to the cache line
   */
  void UncheckWrite(address_t address, const void *buffer, int size)
  { int set, line;
    /* get the set where the address can be found */
    if(hit(address))
    { set = getSet(address);
      line = getLine(address);
      setCacheLineIndexed(set, line, (const char *)buffer, address & (nLineSize - 1), size, getWrite(set, line));
    }
  }

  /**
   * \brief Memory interface : Write to the memory
   */
  void Write(address_t address, const void *buffer, int size)
  { if(size > 0)
    { char *buf = (char *) buffer;
      address_t offset = address & (nLineSize - 1);
      int sz = nLineSize - offset;
      if(size > sz)
      { UncheckWrite(address, buf, sz);
        size -= sz;
        address += sz;
        buf += sz;

        if(size >= nLineSize)
        { do
          { UncheckWrite(address, buf, nLineSize);
            size -= nLineSize;
            address += nLineSize;
            buf += nLineSize;
          }
          while(size >= nLineSize);
        }

        if(size > 0)
        { UncheckWrite(address, buf, size);
        }
      }
      else
      { UncheckWrite(address, buf, size);
      }
    }
  }

  /**
   * \brief Perform a unchecked set to the cache line
   */
  void UncheckSet(address_t address, uint8_t data, int size)
  { int set, line, index;
    /* get the set where the address can be found */
    if(hit(address))
    { set = getSet(address);
      line = getLine(address);
      index = address & (nLineSize - 1);
      memset((cdata[set][line]) + index, data, size);
    }
  }

  /**
   * \brief ???
   */
  void Set(address_t address, uint8_t data, int size)
  { if(size > 0)
    { address_t offset = address & (nLineSize - 1);
      int sz = nLineSize - offset;
      if(size > sz)
      { UncheckSet(address, data, sz);
        size -= sz;
        address += sz;

        if(size >= nLineSize)
        { do
          { UncheckSet(address, data, nLineSize);
            size -= nLineSize;
            address += nLineSize;
          }
          while(size >= nLineSize);
        }

        if(size > 0)
        { UncheckSet(address, data, size);
        }
      }
      else
      { UncheckSet(address, data, size);
      }
    }
  }

  /**
   * Get the counted associated to the cache data (used by some prefetch mechanism)
   */
  int GetCounter(int set, int line)
  { return counter[set][line];
  }

  /**
   * Set the counted associated to the cache data (used by some prefetch mechanism)
   */
  void SetCounter(int set, int line, int level)
  { counter[set][line] = level;
  }

  /**
   * \brief Perform a warm restart
   */
  void WarmRestart()
  { for(int i = 0; i < (nCacheLines / nAssociativity); i++)
    { for(int j = 0; j < nAssociativity; j++)
      { clock[i][j] = false;
      }
    }
  }
  
  /**
   * \brief Check the cache coherency, for impossible cases
   */
  void check_coherency(const string &this_name, const string &other_name, const CacheContainer < nCacheLines, nLineSize, nAssociativity, replacementPolicy, nPseudoLRUHistorySize > &other_container)
  { //cerr << "--- check_coherency - " << this_name << " / " << other_name << " -------------" << endl;

    for(int i = 0; i < (nCacheLines / nAssociativity); i++)
    { for(int j = 0; j < nAssociativity; j++)
      { if(cvalidity[i][j])
        { // Local coherency on invalid
          if(cmesi[i][j].getState()==MESI::state_INVALID)
          { cerr << "--- check_coherency - " << this_name << " / " << other_name << " at cycle " << timestamp() << " -------------" << endl;
            cerr << "error: valid line with MESI invalid state" << endl;
            exit(1);
          }
          // Local coherency on modified
          if(cwrite[i][j])
          { if(cmesi[i][j].getState()!=MESI::state_MODIFIED)
            { cerr << "--- check_coherency - " << this_name << " / " << other_name << " at cycle " << timestamp() << " -------------" << endl;
              cerr << "error: modified line with MESI non modified state" << endl;
              exit(1);
            }
          }
          else
          { if(cmesi[i][j].getState()==MESI::state_MODIFIED)
            { cerr << "--- check_coherency - " << this_name << " / " << other_name << " at cycle " << timestamp() << " -------------" << endl;
              cerr << "error: non modified line with MESI modified state" << endl;
              exit(1);
            }
          }

          address_t addr = getAddress(i,j);
          if(other_container.hit(addr)
  
//&& (addr >= 0x10070000)  // for debugging purpose, should be removed

            )
          { // The cache line belongs to two different caches
            int other_i = other_container.getSet(addr);
            int other_j = other_container.getLine(addr);
            if(cmesi[i][j].getState()!=MESI::state_SHARED || other_container.cmesi[other_i][other_j].getState()!=MESI::state_SHARED)
            { cerr << "--- check_coherency - " << this_name << " / " << other_name << " at cycle " << timestamp() << " -------------" << endl;
              cerr << "error: shared cache line not with SHARED state" << endl;
              cerr << "cache line address: " << hex << addr << dec << endl;
              cerr << this_name << ": ";
              dump(cerr,i,j);
              cerr << other_name << ": ";
              other_container.dump(cerr,other_i,other_j);
//              exit(1);
#ifndef CACHE_CHECK_COHERENCY_NOT_FATAL
              terminate_now();
#endif
            }
          }

        }
        else
        { // Local coherency on non-invalid
          if(cmesi[i][j].getState()!=MESI::state_INVALID)
          { cerr << "--- check_coherency - " << this_name << " / " << other_name << " -------------" << endl;
            cerr << "error: invalid line with MESI valid state" << endl;
            exit(1);
          }
        }
        //cwrite[i][j] = false;
      }
    }

  }

protected:
  char cdata[nCacheLines/nAssociativity][nAssociativity][nLineSize];         ///< data contained in the cache
  uint64_t caddress[nCacheLines/nAssociativity][nAssociativity];             ///< addresses of the data contained in the cache
  bool cvalidity[nCacheLines/nAssociativity][nAssociativity];                ///< defines if the data contained in the line is valid
  bool cwrite[nCacheLines/nAssociativity][nAssociativity];                   ///< defines if the data contained in the line has been modified
  bool clock[nCacheLines/nAssociativity][nAssociativity];                    ///< clock associated with the cache data
  bool cfilled[nCacheLines/nAssociativity][nAssociativity];                  ///< defines if the line contained any data at all, if the line does not contain data it can be used anytime
  bool prefetched[nCacheLines/nAssociativity][nAssociativity];               ///< defines if the line is a prefetched line
  int counter[nCacheLines/nAssociativity][nAssociativity];                   ///< counter used by CDP cache
  
  char missLine[nLineSize];                                                  ///< buffer of the last miss being serviced
  address_t missAddress;                                                     ///< the address of the miss serviced
  bool missValid;                                                            ///< true if missLine data is valid
  bool missWrite;                                                            ///< true if missLine has been modified 
  bool replacementValid;                                                     ///< true if the replacement is valid
  char replacedLine[nLineSize];                                              ///< keeps the last line replaced
  address_t replacedAddress;                                                 ///< keeps the address of the last line replaced
  bool replacedValid;                                                        ///< the last replaced line valid bit
  bool replacedWrite;                                                        ///< the last replaced line write bit
  OoOQueue<int,nAssociativity> lruHistory[nCacheLines/nAssociativity];       ///< the lru history of each set used when lru replacement policy requested
  bool pseudoLRUHistory[nCacheLines/nAssociativity][nPseudoLRUHistorySize];  ///< the pseudoLRU history of each set used when pseudoLRU replacement policy requested
  uint64_t setMask;                                                          ///< Mask to apply to an address once shifted (setShift) to get the set index
  uint32_t setShift;                                                         ///< Shift to apply to an address before masking (setMask) to get the set index
  uint64_t blkMask;                                                          ///< Mask to apply
  uint32_t tagShift;                                                         ///< Shift to apply to the tag
  uint64_t tagMask;                                                          ///< Mask to apply to the tag
  uint64_t tagSetMask;                                                       ///< Mask to apply to the tag set
  
  MESI cmesi[nCacheLines/nAssociativity][nAssociativity];                    ///< MESI protocol state associated with the cache line
  
  string last_dump;                                                          ///< Used for smart dumping when debuuging
};

#endif //__CACHECONTAINER_H__
