/* -*- c++ -*- */

/***************************************************************************
                    MSHRContainerPrefetcher.h  -  description
                             -------------------
    begin                : Thu Feb 27 2003
    copyright            : (C) 2003 CEA and Paris South University
    author               : Daniel Gracia Perez
    email                : gracia@lri.fr
 ***************************************************************************/

#ifndef __MSHRCONTAINER_PREFETCHER_H__
#define __MSHRCONTAINER_PREFETCHER_H__

#include "mem_common.h"
#include "common.h"
#include "base/utility.h"
#include "system/memory/cache/CacheCommon.h"

#define NEW_MSHR_ENTRY 0
#define EXISTENT_MSHR_ENTRY 1
#define NOT_ACCEPTED_MSHR_ENTRY 2
#define EXISTENT_MSHR_PREFETCH_ENTRY 3

#define MSHR_READ_ENTRY_FREE 0
#define MSHR_READ_ENTRY_VALID 1
#define MSHR_READ_ENTRY_READY 2

#define MSHR_ENTRY_FREE 0
#define MSHR_ENTRY_WAITING_DATA 1
#define MSHR_ENTRY_SERVICING_READS 2
#define MSHR_ENTRY_DONE 3

template <class T,
int nSources,
int nRequestSize>
class MSHRReadEntry
{
public:
  int state;
  address_t address;
  address_t base;
  int index;
  int size;
  uint8_t data[nRequestSize];
  bool valid[nRequestSize];
  int port;
  int tag;
  unsigned long long int id;
  T instruction; // just for debugging information

  MSHRReadEntry()
  {
    int i;

    state = MSHR_READ_ENTRY_FREE;
    address = 0;
    size = 0;
    for (i = 0; i < nRequestSize; i++)
    {
      data[i] = 0;
      valid[i] = 0;
    }
    port = -1;
    tag = -1;
  }

  friend ostream& operator << (ostream& os, const MSHRReadEntry<T, nSources, nRequestSize>& entry)
  {
    int i;

    os << "state=";
    switch (entry.state)
    {
      case MSHR_READ_ENTRY_FREE:
        os << "MSHR_READ_ENTRY_FREE" << endl;
        break;
      case MSHR_READ_ENTRY_VALID:
        os << "MSHR_READ_ENTRY_VALID";
        os << ", address=" << hexa(entry.address);
        os << ", size=" << entry.size;
        os << ", port=" << entry.port;
        os << ", tag=" << entry.tag;
        os << ", instruction=" << entry.instruction;
        os << ", valid data= ";
        for (i = 0; i < entry.size; i++)
          if (entry.valid[i])
          {
            os << "1";
          }
          else os << "0";
        os << ", data=";
        for (i = 0; i < entry.size; i++)
          os << hexa(entry.data[i]);
        os << endl;
        break;
      case MSHR_READ_ENTRY_READY:
        os << "MSHR_READ_ENTRY_READY";
        os << ", address=" << hexa(entry.address);
        os << ", size=" << entry.size;
        os << ", data=";
        for (i = 0; i < entry.size; i++)
          os << hexa(entry.data[i]);
        os << ", port=" << entry.port;
        os << ", tag=" << entry.tag;
        os << ", instruction=" << entry.instruction;
        os << endl;
        break;
    }
    return os;
  }

  void Init(address_t _address, address_t _base, int _index,
            int _size, int _port, int _tag, T _instruction,
            unsigned long long int _id)
  {
    if (state != MSHR_READ_ENTRY_FREE)
    {
      cerr << "Trying to initialize a MSHR read entry that is not free" << endl;
      exit(-1);
    }
    address = _address;
    base = _base;
    index = _index;
    size = _size;
    port = _port;
    tag = _tag;
    instruction = _instruction;
    id = _id;
    for (int i = 0; i < nRequestSize; i++)
      valid[i] = false;
    state = MSHR_READ_ENTRY_VALID;
  }

  void InitWithData(address_t _address, address_t _base, int _index,
                    int _size, int _port, int _tag, T _instruction,
                    uint8_t *_data, bool *_valid, unsigned long long int _id)
  {
    if (state != MSHR_READ_ENTRY_FREE)
    {
      cerr << "Trying to initialize a MSHR read entry that is not free" << endl;
      exit(-1);
    }
    address = _address;
    base = _base;
    index = _index;
    size = _size;
    port = _port;
    tag = _tag;
    instruction = _instruction;
    memcpy(data, _data, size);
    memcpy(valid, _valid, size);
    state = MSHR_READ_ENTRY_READY;
    id = _id;
    for (int i = 0; i < size; i++)
    {
      if (!valid[i]) state = MSHR_READ_ENTRY_VALID;
    }
  }

  void Update(address_t _address, uint8_t *_data)
  {
    if (state == MSHR_READ_ENTRY_FREE)
    {
      cerr << "Trying to update a mshr read entry that is free" << endl;
      exit(-1);
    }
    if (state == MSHR_READ_ENTRY_READY)
    {
      cerr << "Trying to update a mshr read entry that is already ready" << endl;
      exit(-1);
    }
    int index;
    index = base - _address;
    for (int i = 0; i < size; i++)
    {
      if (!valid[i])
      {
        data[i] = _data[index + i];
        valid[i] = true;
      }
    }
    state = MSHR_READ_ENTRY_READY;
  }
};

/* A C++ class (template) modelling a MSHR entry */
template <class T,
int nSources,
int nLineSize,
int nRequestSize,
int nReadRequests>
class MSHREntry
{
public:
  /* MSHR entry information */
  int state;
  address_t line_address;
  uint8_t line_data[nLineSize];
  bool line_valid[nLineSize];
  T instruction; // just for debugging information

  /* MSHR entry cache placement information */
  int set;
  int line;

  /* prefetch information */
  bool prefetch;

  /* MSHR read requests to service */
  MSHRReadEntry<T, nSources, nRequestSize> read_entry[nReadRequests];

  MSHREntry()
  {
    int i;

    state = MSHR_ENTRY_FREE;
    line_address = 0;
    for (i = 0; i < nLineSize; i++)
    {
      line_data[i] = 0;
      line_valid[i] = false;
    }

    set = 0;
    line = 0;
  }

  void Init()
  {
    int i;

    state = MSHR_ENTRY_FREE;
    line_address = 0;
    for (i = 0; i < nLineSize; i++)
    {
      line_data[i] = 0;
      line_valid[i] = false;
    }

    set = 0;
    line = 0;

    prefetch = false;
  }

  friend ostream& operator << (ostream& os, const MSHREntry<T, nSources, nLineSize, nRequestSize, nReadRequests>& entry)
  {
    int i;

    os << "state=";
    switch (entry.state)
    {
      case MSHR_ENTRY_FREE:
        os << "MSHR_ENTRY_FREE";
        break;
      case MSHR_ENTRY_WAITING_DATA:
        os << "MSHR_ENTRY_WAITING_DATA";
        break;
      case MSHR_ENTRY_SERVICING_READS:
        os << "MSHR_ENTRY_SERVICING_READS";
        break;
      case MSHR_ENTRY_DONE:
        os << "MSHR_ENTRY_DONE";
        break;
    }
    os << ", line_address=" << hexa(entry.line_address);
    os << ", line_data=";
    for (i = 0; i < nLineSize; i++)
    {
      os << hexa(entry.line_data[i]);
    }
    os << ", line_valid=";
    for (i = 0; i < nLineSize; i++)
    {
      os << hexa(entry.line_valid[i]);
    }
    os << ", instruction=" << entry.instruction;
    os << ", set=" << entry.set;
    os << ", line=" << entry.line;
    os << ", prefetch=" << entry.prefetch;
    os << endl;
    for (i = 0; i < nReadRequests; i++)
      os << "read request " << i << " : " << entry.read_entry[i];

    return os;
  }

  bool HasAddress(address_t address)
  {
    if ((line_address & ~((address_t)(nLineSize - 1))) == (address & ~((address_t)(nLineSize - 1))))
      return true;
    return false;
  }

  bool AddRead(address_t _address, address_t _base, int _index, int _size, int _port,
               int _tag, T _instruction,
               int _set, int _line, unsigned long long int id, bool *hit_prefetch)
  {
    //	cerr << "AddRead set = " << set << ":" << _set << ", line = " << line << ":" << _line << ",tag = " << _tag << endl;
    //	cerr << "  address = " << hexa(_address) << ", base = " << _base;
    //	cerr << "  ¡instructon = " << _instruction;
    *hit_prefetch = false;
    switch (state)
    {
      case MSHR_ENTRY_FREE:
        state = MSHR_ENTRY_WAITING_DATA;
        line_address = _address & ~(((address_t)nLineSize) - 1);
        set = _set;
        line = _line;
        read_entry[0].Init(_address, _base, _index, _size, _port, _tag, _instruction, id);
        instruction = _instruction;
        return true;
        break;
      case MSHR_ENTRY_WAITING_DATA:
        if (!HasAddress(_address))
          return false;
        int i;
        int read_entry_index;
        for (i = 0, read_entry_index = -1; read_entry_index == -1 && i < nReadRequests; i++)
        {
          if (read_entry[i].state == MSHR_READ_ENTRY_FREE)
            read_entry_index = i;
        }
        if (read_entry_index == -1)
          return false;
        int index;
        index = _base & (((address_t)nLineSize) - 1);
        read_entry[read_entry_index].InitWithData(_address, _base, _index, _size, _port,
            _tag, _instruction,
            &line_data[index], &line_valid[index], id);
        *hit_prefetch = prefetch;
        prefetch = false;
        return true;
        break;
      case MSHR_ENTRY_SERVICING_READS:
        return false;
        break;
    }
    return false;
  }

  bool AddWrite(address_t _address, address_t _base, int _index, int _size, uint8_t *_data,
                T _instruction,
                int _set, int _line, bool *hit_prefetch)
  {
    *hit_prefetch = false;
    switch (state)
    {
      case MSHR_ENTRY_FREE:
        state = MSHR_ENTRY_WAITING_DATA;
        line_address = _address & ~(((address_t)nLineSize) - 1);
        set = _set;
        line = _line;
        instruction = _instruction;
        int i;
        int index;
        index = _address & (((address_t)nLineSize) - 1);
        for (i = 0; i < _size;	i++)
        {
          line_data[index+i] = _data[i];
          line_valid[index+i] = true;
        }
        return true;
        break;
      case MSHR_ENTRY_WAITING_DATA:
        if (!HasAddress(_address))
          return false;
        index = _address & (((address_t)nLineSize) - 1);
        for (i = 0; i < _size;	i++)
        {
          line_data[index+i] = _data[i];
          line_valid[index+i] = true;
        }
        *hit_prefetch = prefetch;
        prefetch = false;
        return true;
        break;
      case MSHR_ENTRY_SERVICING_READS:
        return false;
        break;
    }
    return false;
  }

  bool AddPrefetch(address_t _address, address_t _base, int _index, int _size,
                   T _instruction,
                   int _set, int _line, bool *hit_prefetch)
  {
    *hit_prefetch = false;
    switch (state)
    {
      case MSHR_ENTRY_FREE:
        state = MSHR_ENTRY_WAITING_DATA;
        line_address = _address & ~(((address_t)nLineSize) - 1);
        set = _set;
        line = _line;
        instruction = _instruction;
        prefetch = true; // the line was a prefetch
        return true;
        break;
      case MSHR_ENTRY_WAITING_DATA:
        if (!HasAddress(_address))
          return false;
        *hit_prefetch = prefetch;
        return true;
        break;
      case MSHR_ENTRY_SERVICING_READS:
        return false;
        break;
    }
    return false;
  }

  void Update(address_t _address, uint8_t *_data)
  {
    int i;


    if (!HasAddress(_address))
    {
      cerr << "Trying to update an incorrect MSHR entry" << endl;
      return;
    }
    for (i = 0; i < nLineSize; i++)
    {
      if (!line_valid[i])
        line_data[i] = _data[i];
    }
    state = MSHR_ENTRY_DONE;
    for (i = 0; i < nReadRequests; i++)
    {
      switch (read_entry[i].state)
      {
        case MSHR_READ_ENTRY_FREE:
          /* nothing to do */
          break;
        case MSHR_READ_ENTRY_VALID:
          read_entry[i].Update(_address & ~(((address_t)nLineSize) - 1), _data);
          read_entry[i].state = MSHR_READ_ENTRY_READY;
          state = MSHR_ENTRY_SERVICING_READS;
          break;
        case MSHR_READ_ENTRY_READY:
          /* nothing to do */
          state = MSHR_ENTRY_SERVICING_READS;
          break;
      }
    }
  }

  unsigned long long int GetReadId()
  {
    int i;
    for (i = 0; i < nReadRequests && read_entry[i].state == MSHR_READ_ENTRY_FREE; i++);
    if (i == nReadRequests)
    {
      cerr << "Trying to get a read id entry when there are not" << endl;
      exit(-1);
    }
    if (read_entry[i].state == MSHR_READ_ENTRY_VALID)
    {
      cerr << "Trying to get a read id entry when it is not ready" << endl;
      exit(-1);
    }

    return read_entry[i].id;
  }

  void GetRead(address_t *_address, address_t *_base, int *_index, int *_size, int *_port,
               T *_instruction, int *_tag, uint8_t *_data)
  {
    int i;
    for (i = 0; i < nReadRequests && read_entry[i].state == MSHR_READ_ENTRY_FREE; i++);
    if (i == nReadRequests)
    {
      cerr << "Trying to get a read entry when there are not" << endl;
      exit(-1);
    }
    if (read_entry[i].state == MSHR_READ_ENTRY_VALID)
    {
      cerr << "Trying to get a read entry when it is not ready" << endl;
      exit(-1);
    }
    *_instruction = read_entry[i].instruction;
    *_address = read_entry[i].address;
    *_base = read_entry[i].base;
    *_index = read_entry[i].index;
    *_size = read_entry[i].size;
    *_port = read_entry[i].port;
    *_tag = read_entry[i].tag;
    memcpy(_data, read_entry[i].data, read_entry[i].size);
    read_entry[i].state = MSHR_READ_ENTRY_FREE;

    for (i = 0; i < nReadRequests && read_entry[i].state == MSHR_READ_ENTRY_FREE; i++);
    if (i == nReadRequests)
    {
      state = MSHR_ENTRY_DONE;
    }
  }

  int GetSet()
  {
    return set;
  }

  int GetLine()
  {
    return line;
  }

  uint8_t *GetData()
  {
    return line_data;
  }

  bool GetDirty()
  {
    int i;

    for (i = 0; i < nLineSize; i++)
      if (line_valid[i])
        return true;
    return false;
  }

  bool GetPrefetch()
  {
    return prefetch;
  }
};


template <class T,
int nSources,
int nLineSize,
int nCPULineSize,
int nMSHR,
int nMSHRRead>
class MSHRContainer
{
private:
  OoOQueue<MSHREntry<T, nSources, nLineSize, nCPULineSize, nMSHRRead>, nMSHR> MSHRQueue;
  unsigned long long int id;

public:
  friend ostream& operator << (ostream& os, const MSHRContainer<T, nSources, nLineSize, nCPULineSize, nMSHR, nMSHRRead>& mshr)
  {
    os << mshr.MSHRQueue << endl;
    return os;
  }

  MSHRContainer()
  {
    id = 0;
  }

  void Update(address_t addr, uint8_t *in_data,
              int *locked_set, int *locked_line, bool *dirty, uint8_t *out_data, bool *prefetched)
  {
    OoOQueuePointer<MSHREntry<T, nSources, nLineSize, nCPULineSize, nMSHRRead>, nMSHR> mshrit;

    if (!MSHRQueue.Empty())
    {
      for (mshrit = MSHRQueue.SeekAtHead();
           mshrit && !mshrit->HasAddress(addr);
           mshrit++);

      if (!mshrit)
      {
        cerr << hexa(addr) << endl;
        cerr << "MSHRContainer: data was received that does not have correspondent MSHR entry" << endl;
        exit(-1);
      }

      mshrit->Update(addr, in_data);
      memcpy(out_data, mshrit->GetData(), nLineSize);
      *locked_set = mshrit->GetSet();
      *locked_line = mshrit->GetLine();
      *dirty = mshrit->GetDirty();
      *prefetched = mshrit->GetPrefetch();
      if (mshrit->state == MSHR_ENTRY_DONE)
      {
        MSHRQueue.Remove(mshrit);
      }
    }
    else
    {
      cerr << " MSHRContainer: data was received when there are no entries in the MSHR" << endl;
      exit(-1);
    }
  }

  bool GetRead(address_t *addr, address_t *base, int *index, int *size, int *port,
               T *instruction, int *tag, uint8_t *data)
  {
    OoOQueuePointer<MSHREntry<T, nSources, nLineSize, nCPULineSize, nMSHRRead>, nMSHR> mshrit, candidate;
    unsigned long long int tmp_id;
    bool found;

    tmp_id = 0;
    found = false;
    if (!MSHRQueue.Empty())
    {
      for (mshrit = MSHRQueue.SeekAtHead();
           mshrit; mshrit++)
      {
        if (mshrit->state == MSHR_ENTRY_SERVICING_READS)
        {
          if (!found)
          {
            tmp_id = mshrit->GetReadId();
            candidate = mshrit;
            found = true;
          }
          else
          {
            if (tmp_id > mshrit->GetReadId())
            {
              tmp_id = mshrit->GetReadId();
              candidate = mshrit;
            }
          }
        }
      }
      if (found)
      {
        candidate->GetRead(addr, base, index, size, port, instruction, tag, data);
        if (candidate->state == MSHR_ENTRY_DONE) MSHRQueue.Remove(candidate);
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }

// 		if(!MSHRQueue.Empty()) {
// 			for(mshrit = MSHRQueue.SeekAtHead();
// 					mshrit && mshrit->state != MSHR_ENTRY_SERVICING_READS;
// 					mshrit++);
// 			if(!mshrit) return false;
// 			mshrit->GetRead(addr, base, index, size, port, instruction, tag, data);
// 			if(mshrit->state == MSHR_ENTRY_DONE) MSHRQueue.Remove(mshrit);
// 			return true;
// 		} else {
// 			return false;
// 		}
  }

  int AddReadEntry(address_t addr, address_t base, int index,
                   int size, int port, int tag,
                   T instruction, int lock_set, int lock_line)
  {
    OoOQueuePointer<MSHREntry<T, nSources, nLineSize, nCPULineSize, nMSHRRead>, nMSHR> mshrit;
    bool found;
    bool hit_prefetch = false;

    /* check if the requested address is already in one of the MSHR entries */
    if (!MSHRQueue.Empty())
    {
      found = true;
      mshrit = MSHRQueue.SeekAtHead();
      while (mshrit && !mshrit->HasAddress(addr))
      {
        mshrit++;
      }

      if (!mshrit)
      {
        found = false;
        if (!MSHRQueue.Full())
        {
          MSHRQueue.New();
          mshrit = MSHRQueue.SeekAtTail();
          mshrit->Init();
        }
      }
      if (mshrit)
      {
        if (mshrit->AddRead(addr, base, index, size,
                            port, tag,
                            instruction, lock_set, lock_line, id, &hit_prefetch))
        {
          if (found)
          {
            if (hit_prefetch) return EXISTENT_MSHR_PREFETCH_ENTRY;
            else return EXISTENT_MSHR_ENTRY;
          }
          else
          {
            return NEW_MSHR_ENTRY;
          }
        }
        else
        {
          return NOT_ACCEPTED_MSHR_ENTRY;
        }
      }
      else
      {
        return NOT_ACCEPTED_MSHR_ENTRY;
      }
      id++;
    }
    else
    {
      MSHRQueue.New();
      mshrit = MSHRQueue.SeekAtTail();
      mshrit->Init();
      if (mshrit->AddRead(addr, base, index,
                          size, port, tag,
                          instruction, lock_set, lock_line, id, &hit_prefetch))
      {
        return NEW_MSHR_ENTRY;
      }
      else
      {
        return NOT_ACCEPTED_MSHR_ENTRY;
      }
      id++;
    }
    return NOT_ACCEPTED_MSHR_ENTRY;
  }

  int AddWriteEntry(address_t addr, address_t base, int index,
                    int size, uint8_t *data,
                    T instruction, int lock_set, int lock_line)
  {
    OoOQueuePointer<MSHREntry<T, nSources, nLineSize, nCPULineSize, nMSHRRead>, nMSHR> mshrit;
    bool found;
    bool hit_prefetch = false;

    /* check if the requested address is already in one of the MSHR entries */
    if (!MSHRQueue.Empty())
    {
      mshrit = MSHRQueue.SeekAtHead();
      found = true;
      while (mshrit && !mshrit->HasAddress(addr))
      {
        mshrit++;
      }

      if (!mshrit)
      {
        found = false;
        if (!MSHRQueue.Full())
        {
          MSHRQueue.New();
          mshrit = MSHRQueue.SeekAtTail();
          mshrit->Init();
        }
      }

      if (mshrit)
      {
        if (mshrit->AddWrite(addr, base, index,
                             size, data,
                             instruction, lock_set, lock_line, &hit_prefetch))
        {
          if (found)
          {
            if (hit_prefetch) return EXISTENT_MSHR_PREFETCH_ENTRY;
            else return EXISTENT_MSHR_ENTRY;
          }
          else
          {
            return NEW_MSHR_ENTRY;
          }
        }
        else
        {
          return NOT_ACCEPTED_MSHR_ENTRY;
        }
      }
      else
      {
        return NOT_ACCEPTED_MSHR_ENTRY;
      }
    }
    else
    {
      MSHRQueue.New();
      mshrit = MSHRQueue.SeekAtTail();
      mshrit->Init();
      if (mshrit->AddWrite(addr, base, index,
                           size, data,
                           instruction, lock_set, lock_line, &hit_prefetch))
      {
        return NEW_MSHR_ENTRY;
      }
      else
      {
        return NOT_ACCEPTED_MSHR_ENTRY;
      }
    }
    return NOT_ACCEPTED_MSHR_ENTRY;
  }

  int AddPrefetchEntry(address_t addr, address_t base, int index,
                       int size, int port, int tag,
                       T instruction, int lock_set, int lock_line)
  {
    OoOQueuePointer<MSHREntry<T, nSources, nLineSize, nCPULineSize, nMSHRRead>, nMSHR> mshrit;
    bool found;
    bool hit_prefetch = false;

    /* check if the requested address is already in one of the MSHR entries */
    if (!MSHRQueue.Empty())
    {
      found = true;
      mshrit = MSHRQueue.SeekAtHead();
      while (mshrit && !mshrit->HasAddress(addr))
      {
        mshrit++;
      }

      if (!mshrit)
      {
        found = false;
        if (!MSHRQueue.Full())
        {
          MSHRQueue.New();
          mshrit = MSHRQueue.SeekAtTail();
          mshrit->Init();
        }
      }
      if (mshrit)
      {
        if (mshrit->AddPrefetch(addr, base, index, size,
                                instruction, lock_set, lock_line, &hit_prefetch))
        {
          if (found)
          {
            if (hit_prefetch) return EXISTENT_MSHR_PREFETCH_ENTRY;
            else return EXISTENT_MSHR_ENTRY;
          }
          else
          {
            return NEW_MSHR_ENTRY;
          }
        }
        else
        {
          return NOT_ACCEPTED_MSHR_ENTRY;
        }
      }
      else
      {
        return NOT_ACCEPTED_MSHR_ENTRY;
      }
    }
    else
    {
      MSHRQueue.New();
      mshrit = MSHRQueue.SeekAtTail();
      mshrit->Init();
      if (mshrit->AddPrefetch(addr, base, index, size,
                              instruction, lock_set, lock_line, &hit_prefetch))
      {
        return NEW_MSHR_ENTRY;
      }
      else
      {
        return NOT_ACCEPTED_MSHR_ENTRY;
      }
    }
    return NOT_ACCEPTED_MSHR_ENTRY;
  }

  bool Empty()
  {
    return MSHRQueue.Empty();
  }

};


#endif
