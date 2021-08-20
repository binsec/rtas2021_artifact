/***************************************************************************
   MemoryContainer.h  -  Defines the MemoryContainer class
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, Commissariat a l'Energie Atomique (CEA) & INRIA
Authors: Daniel Gracia Pérez (daniel.gracia-perez@cea.fr)
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

#ifndef __UNISIM_COMPONENT_CLM_MEMORY_MEMORYCONTAINER_HH__
#define __UNISIM_COMPONENT_CLM_MEMORY_MEMORYCONTAINER_HH__

//#include "mem_common.h"
#include <unisim/component/clm/memory/mem_common.hh>
#include <list>
#include <set>

#define MEMORY_CONTAINER_PAGE_SIZE 4096
//#define MEMORY_CONTAINER_HASH_TABLE_SIZE (4096*4)
#define MEMORY_CONTAINER_HASH_TABLE_SIZE (4096*64)
#define MEMORY_CONTAINER_DUMP_LINE_SIZE 32
#define MEMORY_CONTAINER_DUMP_PACK_SIZE 4

class MemoryPageTableEntry
{public:
  /**
   * \brief Empty constructor, required to build lists
   */
  MemoryPageTableEntry()
  {
  }
  
  /**
   * \brief Class constructor
   */
  MemoryPageTableEntry(address_t _address, MemoryPageTableEntry *_next)
  { memset(storage,0,sizeof(storage));
    address=_address;
    next=_next;
  }

  /**
   * \brief Class destructor
   */
  ~MemoryPageTableEntry()
  {
  }

  /**
   * \brief Dumps some memory content
   */
  void dump(ostream &os, address_t addr, const list<address_t> &last_modified)
  { uint32_t val,cpt;
    bool changed;
    os << hex << right << setfill('0');
    os << addr << " : " ;
    for(int i=0; i<MEMORY_CONTAINER_DUMP_LINE_SIZE; i++)
    { val = storage[addr-address+i];
      changed=false;
      for(list<address_t>::const_iterator it=last_modified.begin(); it!=last_modified.end(); it++)
      { if(*it==addr+i) changed=true;
      }
      if(changed) os << "\e[1;31m";
      os << setw(2) << val;
      if(changed) os << "\e[0m";
      if(i%MEMORY_CONTAINER_DUMP_PACK_SIZE==MEMORY_CONTAINER_DUMP_PACK_SIZE-1) os << " ";
    }
    os << dec << left << setfill(' ') << "\n";
  }

  address_t address;                           ///< Corresponding address
  class MemoryPageTableEntry *next;            ///< Pointer to next memory page for this hash key
  uint8_t storage[MEMORY_CONTAINER_PAGE_SIZE]; ///< Memory data
};


/**
 * \brief MemoryContainer: memory content
 */
template
< bool VERBOSE
>
class MemoryContainer
{public:
  /**
   * \brief Creates a new MemoryContainer
   */
  MemoryContainer()
  { memset(hash_table,0,sizeof(hash_table));
  }
  
  /**
   * \brief Class destructor
   */
  ~MemoryContainer()
  { int h;
    MemoryPageTableEntry *entry, *next_entry;
    for(h = 0; h < MEMORY_CONTAINER_HASH_TABLE_SIZE; h++)
    { entry = hash_table[h];
      if(entry)
      { do
        { next_entry = entry->next;
          //delete entry;
        } while((entry = next_entry));
      }
    }
  }

  /**
   * \brief Loads the Memory content from a checkpoint file
   */
  void load_checkpoint(istream &is)
  { string section;
    address_t dummy_org;
    uint32_t dummy_bytesize;
    address_t dummy_lo_addr;
    address_t dummy_hi_addr;
    char c;
    address_t addr;
    uint32_t page_size;
    is >> section;
    if(section!="===MEM=====================================")
    { cerr << "error: Invalid checkpoint file" << std::endl;
      exit(1);
    }
    is >> dummy_org;
    is >> dummy_bytesize;
    is >> dummy_lo_addr;
    is >> dummy_hi_addr;
    is >> page_size;

    is >> c;
    
    while(c!='.')
    { uint8_t buffer[page_size];
      is >> addr >> hex;
      addr = addr * page_size;
      for(uint32_t i=0; i<page_size; i++)
      { uint32_t val;
        is >> val;
        buffer[i] = val;
      }
      is >> dec;
      Write(addr, &buffer, page_size);
      is >> c;
    }
  }

  /**
   * \brief Memory interface : read from memory
   */
  void Read(void *buffer, address_t address, int size)
  { if(size > 0)
    { char *buf = (char *) buffer;
      address_t offset = address % MEMORY_CONTAINER_PAGE_SIZE;
      MemoryPageTableEntry *entry = GetPage(address);
      int sz = MEMORY_CONTAINER_PAGE_SIZE - offset;
      if(size > sz)
      { memcpy(buf, entry->storage + offset, sz);
        size -= sz;
        address += sz;
        buf += sz;
        if(size >= MEMORY_CONTAINER_PAGE_SIZE)
        { do
          { entry = GetPage(address);
            memcpy(buf, entry->storage, MEMORY_CONTAINER_PAGE_SIZE);
            size -= MEMORY_CONTAINER_PAGE_SIZE;
            address += MEMORY_CONTAINER_PAGE_SIZE;
            buf += MEMORY_CONTAINER_PAGE_SIZE;
          } while(size >= MEMORY_CONTAINER_PAGE_SIZE);
        }

        if(size > 0)
        { entry = GetPage(address);
          memcpy(buf, entry->storage, size);
        }
      }
      else
      { memcpy(buf, entry->storage + offset, size);
      }
    }
  }

  /**
   * \brief Memory interface : write to memory
   */
  void Write(address_t address, const void *buffer, int size)
  { if(size > 0)
    { 
      if(VERBOSE)
      {
#ifdef DRAM_DUMP_ONLY_ADDRESS_MIN
        if(address>=DRAM_DUMP_ONLY_ADDRESS_MIN)
#endif
#ifdef DRAM_DUMP_ONLY_ADDRESS_MAX
        if(address<=DRAM_DUMP_ONLY_ADDRESS_MAX)
#endif
        { last_modified.push_back(address);
        }
      } // if(VERBOSE)
    
      char *buf = (char *) buffer;
      address_t offset = address % MEMORY_CONTAINER_PAGE_SIZE;
      MemoryPageTableEntry *entry = GetPage(address);
      int sz = MEMORY_CONTAINER_PAGE_SIZE - offset;
      if(size > sz)
      { memcpy(entry->storage + offset, buf, sz);
        size -= sz;
        address += sz;
       buf += sz;
  
       if(size >= MEMORY_CONTAINER_PAGE_SIZE)
        { do
          { entry = GetPage(address);
           memcpy(entry->storage, buf, MEMORY_CONTAINER_PAGE_SIZE);
           size -= MEMORY_CONTAINER_PAGE_SIZE;
           address += MEMORY_CONTAINER_PAGE_SIZE;
            buf += MEMORY_CONTAINER_PAGE_SIZE;
          } while(size >= MEMORY_CONTAINER_PAGE_SIZE);
        }

        if(size > 0)
        { entry = GetPage(address);
          memcpy(entry->storage, buf, size);
        }
      }
      else
      { memcpy(entry->storage + offset, buf, size);
      }
    }
  }

  /**
   * \brief Set a data in the corresponding memory page
   */
  void Set(address_t address, uint8_t data, int size)
  { if(size > 0)
    { address_t offset = address % MEMORY_CONTAINER_PAGE_SIZE;
      MemoryPageTableEntry *entry = GetPage(address);
      int sz = MEMORY_CONTAINER_PAGE_SIZE - offset;
      if(size > sz)
      { memset(entry->storage + offset, data, sz);
        size -= sz;
        address += sz;

        if(size >= MEMORY_CONTAINER_PAGE_SIZE)
        { do
          { entry = GetPage(address);
            memset(entry->storage, data, MEMORY_CONTAINER_PAGE_SIZE);
            size -= MEMORY_CONTAINER_PAGE_SIZE;
            address += MEMORY_CONTAINER_PAGE_SIZE;
          } while(size >= MEMORY_CONTAINER_PAGE_SIZE);
        }

        if(size > 0)
        { entry = GetPage(address);
          memset(entry->storage, data, size);
        }
      }
      else
      { memset(entry->storage + offset, data, size);
      }
    }
  }

  /**
   * \brief Scan or consistency the whole memmory hash
   */
  void Scan()
  { int h;
    MemoryPageTableEntry *entry, *next_entry;
    for(h = 0; h < MEMORY_CONTAINER_HASH_TABLE_SIZE; h++)
    { entry = hash_table[h];
      if(entry)
      { do
        { if(entry < (MemoryPageTableEntry *) 0x1000000)
          { cerr << "WARNING! Memory container will crash" << endl; cerr.flush();
          }
          next_entry = entry->next;
        } while((entry = next_entry));
      }
    }
  }

  /**
   * \brief pretty printer
   */
  friend ostream & operator<<(ostream &os, MemoryContainer &mc)
  { stringstream dump;

    set<address_t> to_print;
    for(list<address_t>::iterator i=mc.last_modified.begin(); i!=mc.last_modified.end(); i++)
    { address_t addr = *i;
      addr = addr - (addr%MEMORY_CONTAINER_DUMP_LINE_SIZE);
      to_print.insert(addr);
    }
    
    for(set<address_t>::iterator i=to_print.begin(); i!=to_print.end(); i++)
    { MemoryPageTableEntry *entry = mc.GetPage(*i);
      entry->dump(dump, *i, mc.last_modified);
    }
    
    mc.last_modified.clear();
    
    if(dump.str().size())
    { os << "\n";
      os << setfill('-') << setw(82) << "-" << endl;
      os << "  Cycle " << left << timestamp() << endl;
      os << setfill('-') << setw(82) << "-" << endl;
      os << dump.str();
      os << setfill('-') << setw(82) << "-" << endl;
    }

    return os;
  }
  
 private:
  MemoryPageTableEntry *hash_table[MEMORY_CONTAINER_HASH_TABLE_SIZE];
  uint32_t Hash(address_t address)
  { return (address/MEMORY_CONTAINER_PAGE_SIZE)%MEMORY_CONTAINER_HASH_TABLE_SIZE;
  }

  MemoryPageTableEntry *SearchPage(address_t address)
  { uint32_t h;
    MemoryPageTableEntry *entry,*prev;

    address=address-(address%MEMORY_CONTAINER_PAGE_SIZE);
    h=Hash(address);

    entry=hash_table[h];
    if(entry)
    { if(entry->address==address)
      { return entry;
      }
      prev=entry;
      entry=entry->next;
      while(entry)
      { if(entry->address==address)
        { prev->next = entry->next;
          entry->next = hash_table[h];
          hash_table[h] = entry;
          return entry;
        }
        prev=entry;
        entry=entry->next;
      }
    }
    return 0;
  }

  /**
   * \brief Returns the memory page corresponding to the address
   */
  MemoryPageTableEntry *GetPage(address_t address)
  { uint32_t h;
    MemoryPageTableEntry *entry;

    address=address-(address%MEMORY_CONTAINER_PAGE_SIZE);
    entry=SearchPage(address);
    if(!entry)
    { h=Hash(address);
      entry=new MemoryPageTableEntry(address,hash_table[h]);
      hash_table[h]=entry;
    }
    return entry;
  }

  list<address_t> last_modified;  ///< List of modified values since last dump
};

#endif // __MEMORYCONTAINER_H__
