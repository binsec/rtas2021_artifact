/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#ifndef __VLE4FUZR_EMU_HH__
#define __VLE4FUZR_EMU_HH__

#include <iosfwd>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include <cstdint>

struct Processor
{
  Processor();
  
  virtual ~Processor();

  virtual void run( uint64_t begin, uint64_t until, uint64_t count ) = 0;
  virtual unsigned endian_mask(unsigned size) const = 0;

  struct Page
  {
    typedef void* (*hook_t)(void* uc, unsigned access_type, uint64_t address, unsigned size, unsigned endianness, uint64_t* value);
    typedef void (*info_t)(uint64_t first, uint64_t last, unsigned perms, unsigned hooks);
    
    uint64_t hi() const { return last; }

    bool operator < (Page const& p) const { return last < p.base; }
    bool operator > (Page const& p) const { return p.last < base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, unsigned b) const { return a.base > b; }
    };
    Page( Page&& page )
      : base(page.base), last(page.last), data(page.data)
      , rhook(page.rhook), whook(page.whook), xhook(page.xhook), perms(page.perms)
    {
      page.data = 0;
    }
    Page( Page const& ) = delete;
    Page( uint64_t addr, uint64_t _size, unsigned _perms, hook_t _rhook, hook_t _whook, hook_t _xhook )
      : base(addr), last(addr+_size-1), data(_perms ? new uint8_t[_size] : 0)
      , rhook(_rhook), whook(_whook), xhook(_xhook), perms(_perms)
    {}
    ~Page()
    {
      delete [] data;
    }
    uint64_t write(uint64_t addr, uint8_t const* bytes, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &bytes[0], &bytes[cnt], &data[start] );
      return cnt;
    }
    bool write(uint64_t addr, unsigned count, unsigned endianness, uint64_t value) const
    {
      uint8_t bytes[8];
      for (unsigned idx = 0; idx < count; ++idx)
        { bytes[idx^endianness] = value; value >>= 8; }
      return write( addr, &bytes[0], count ) == count;
    }
    bool phys_write(Processor* p, uint64_t addr, unsigned count, unsigned endianness, uint64_t value) const
    {
      if (data and access( Write ))
        {
          if (whook and whook(p, 1, addr, count, endianness, &value)) return false;
          return write(addr, count, endianness, value);
        }
      return whook and not whook(p, 1, addr, count, endianness, &value);
    }
    uint64_t read(uint64_t addr, uint8_t* bytes, uint64_t count) const
    {
      uint64_t cnt = std::min(count,last-addr+1), start = addr-base;
      std::copy( &data[start], &data[start+cnt], &bytes[0] );
      return cnt;
    }
    bool read(uint64_t addr, unsigned count, unsigned endianness, uint64_t* value) const
    {
      uint8_t bytes[8];
      if (read(addr, &bytes[0], count) != count) return false;
      uint64_t res = 0;
      for (unsigned idx = count; idx-- != 0;)
        { res <<= 8; res |= bytes[idx^endianness]; }
      *value = res;
      return true;
    }
    bool phys_read(Processor* p, uint64_t addr, unsigned count, unsigned endianness, uint64_t* value) const
    {
      if (data and access( Read ))
        {
          if (not read(addr, count, endianness, value)) return false;
          return not rhook or not rhook(p, 0, addr, count, endianness, value);
        }
      return rhook and not rhook(p, 0, addr, count, endianness, value);
    }
    bool phys_fetch(Processor* p, uint64_t addr, unsigned count, unsigned endianness, uint64_t* value) const
    {
      if (data and access( Execute ))
        {
          if (not read(addr, count, endianness, value)) return false;
          return not xhook or not xhook(p, 2, addr, count, endianness, value);
        }
      return xhook and not xhook(p, 2, addr, count, endianness, value);
    }
    
    bool chperms(unsigned new_perms) const { if (not perms) return false; const_cast<unsigned&>(perms) = new_perms; return true; }
    void chhook(unsigned access_type, hook_t new_hook) const { const_cast<hook_t&>(*hookptr(access_type)) = new_hook; }
    enum Permision { Read = 1, Write = 2, Execute = 4 };
    bool access(unsigned _perms) const { return (_perms & perms) == _perms; }
    bool has_data() const { return data; }

    void dump(std::ostream&) const;
    friend std::ostream& operator << ( std::ostream& sink, Page const& p ) { p.dump(sink); return sink; }

    void info( info_t sink ) const { (*sink)( base, last, perms, bool(rhook)*1 | bool(whook)*2 | bool(xhook)*4 ); }
    
  public:
    uint64_t  base;
    uint64_t  last;
  private:
    uint8_t*  data;
    hook_t    rhook,whook,xhook;
    unsigned  perms;

    hook_t const* hookptr(unsigned i) const { switch(i) { case 0: return &rhook; case 1: return &whook; case 2: return &xhook; } return 0; }
    void resize(uint64_t last);
  };
  
  typedef std::set<Page, Page::Above> Pages;
  Pages pages;
  Page  failpage;

  void MemoryException(unsigned mtype, uint64_t address, std::string _msg);

  void error_mem_overlap( Page const& a, Page const& b );

  bool mem_map(uint64_t addr, uint64_t size, unsigned perms, Page::hook_t rhook, Page::hook_t whook, Page::hook_t xhook)
  {
    Page page(addr, size, perms, rhook, whook, xhook);
    auto below = pages.lower_bound(page);
    if (below != pages.end() and not (*below < page))
      {
        error_mem_overlap(page, *below);
        return false;
      }
    if (pages.size() and below != pages.begin() and not (page < *std::prev(below)))
      {
        error_mem_overlap(page, *std::prev(below));
        return false;
      }
    
    pages.insert(below,std::move(page));
    return true;
  }

  void error_at( char const* issue, uint64_t addr );
  
  bool mem_unmap(uint64_t addr)
  {
    auto page = pages.lower_bound(addr);
    if (page == pages.end() or page->last < addr)
      return error_at("no", addr), false;
    pages.erase(page);
    return true;
  }

  bool page_info(Page::info_t sink, uint64_t addr)
  {
    auto page = pages.lower_bound(addr);
    if (page == pages.end() or page->last < addr)
      return error_at("no", addr), false;
    page->info(sink);
    return true;
  }
  
  bool pages_info(Page::info_t sink)
  {
    for (Page const& page : pages)
      page.info(sink);
    return true;
  }
  
  bool  mem_chprot(uint64_t addr, unsigned perms);
  bool  mem_chhook(uint64_t addr, unsigned access_type, Page::hook_t hook);
  bool  mem_exc_chhook(unsigned access_type, Page::hook_t hook);
  
  bool
  mem_write(uint64_t addr, uint8_t const* bytes, uint64_t size)
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end()) return error_at("no", addr), false;
    for (;;)
      {
        if (not pi->has_data())
          return error_at("cannot write hooked", addr), false;
        uintptr_t count = pi->write(addr, bytes, size);
        if (count >= size)       return true;
        addr += count;
        bytes += count;
        size -= count;
        if (pi == pages.begin()) break;
        --pi;
      }
    return error_at("no", addr), false;
  }

  bool
  mem_read(uint64_t addr, uint8_t* bytes, uint64_t size)
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end()) return error_at("no", addr), false;
    for (;;)
      {
        if (not pi->has_data())
          return error_at("cannot read hooked", addr), false;
        uintptr_t count = pi->read(addr, bytes, size);
        if (count >= size)       return true;
        addr += count;
        bytes += count;
        size -= count;
        if (pi == pages.begin()) break;
        --pi;
      }
    return error_at("no", addr), false;
  }

  Page const& AccessPage( uint64_t addr )
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end() or pi->last < addr)
      return failpage;
    return *pi;
  }

  void PhysicalWriteMemory( uint64_t addr, unsigned size, unsigned endianness, uint64_t value )
  {
    Page const& page = AccessPage(addr);
    if (not page.phys_write(this, addr, size, endianness, value))
      { error_at("cannot write", addr); MemoryException(1,addr,"protection fault"); }
  }

  void PhysicalReadMemory( uint64_t addr, unsigned size, unsigned endianness, uint64_t* value )
  {
    Page const& page = AccessPage(addr);
    if (not page.phys_read(this, addr, size, endianness, value))
      { error_at("cannot read", addr); MemoryException(0,addr,"protection fault"); }
  }
  
  void PhysicalFetchMemory( uint64_t addr, unsigned size, unsigned endianness, uint64_t* value )
  {
    Page const& page = AccessPage(addr);
    if (not page.phys_fetch(this, addr, size, endianness, value))
      { error_at("cannot fetch", addr); MemoryException(2,addr,"protection fault"); }
  }
  
  struct RegView
  {
    virtual ~RegView() {}
    virtual void write( Processor& proc, int id, uint64_t bytes ) const = 0;
    virtual void read( Processor& proc, int id, uint64_t* bytes ) const = 0;
  };

  virtual RegView const* get_reg(char const* id, uintptr_t size, int regid) = 0;
  
  bool NoSuchRegister() { error = "NoSuchRegister()"; return false; }
  
  bool
  reg_write(char const* id, uintptr_t size, int regid, uint64_t value)
  {
    if (RegView const* rv = get_reg(id, size, regid))
      rv->write(*this, regid, value);
    else
      return NoSuchRegister();
    return true;
  }
  
  bool
  reg_read(char const* id, uintptr_t size, int regid, uint64_t* value)
  {
    if (RegView const* rv = get_reg(id, size, regid))
      rv->read(*this, regid, value);
    else
      return NoSuchRegister();
    return true;
  }
  
  struct Abort {};
  
  void abort(std::string _error) { error=_error; throw Abort(); }
  
  struct Hook
  {
    typedef void (*cb_code)(void* uc, uint64_t address, unsigned size);
    
    Hook(unsigned _types, void* cb, uint64_t _begin, uint64_t _end)
      : types(_types), begin(_begin), end(_end), callback(cb), insn()
    {}

    enum type_t
      {
       INTR = 0, // Hook all interrupt/syscall events
       // INSN = 1 ILLEGAL, // Hook a particular instruction - only a very small subset of instructions supported here
       CODE = 2, // Hook a range of code
       BLOCK = 3, // Hook basic blocks
       MEM = 4, // Hook for memory access on unmapped memory or protected memory
       TYPE_COUNT
      };
    

    template <unsigned BITS> struct Mask
    {
      enum { bits = BITS };
      template <class T> Mask<T::bits | bits> operator | (T const&) { return Mask<T::bits | bits>(); }
      bool check( unsigned types ) { return bool(types&unsigned(bits)) ^ bool(types&~unsigned(bits));}
    };
    template <type_t TYPE> Mask<1<<TYPE> Is() { return Mask<1<<TYPE>(); }
      
    bool check_types();
    
    template <typename T> T cb() const { return (T)callback; }
    bool has_type( type_t tp ) { return (types >> int(tp)) & 1; }
    void release( type_t tp )
    {
      types &= ~(1u << tp);
      if (not types)
        delete this;
    }
    void release( int tp ) { release(type_t(tp)); }

    bool bound_check(uint64_t addr) { return (addr >= begin and addr <= end) or begin > end; }
    
  private:
    uintptr_t refs;
    
  public:
    unsigned types;
    uint64_t begin, end;
    void* callback;
    int insn;
  };

  bool add_hook( int types, void* callback, uint64_t begin, uint64_t end );
  void insn_hooks(uint64_t addr, unsigned len);
  void syscall_hooks(uint64_t addr, unsigned num);
  
  void DebugBranch( uint64_t target ) { debug_branch = target; }
  uint64_t debug_branch;
  
  void set_disasm(bool _disasm) { disasm = _disasm; }
  std::string asmbuf;
  
  virtual char const* get_asm() = 0;
  
  std::vector<Hook*> hooks[Hook::TYPE_COUNT];

  std::string error;
  bool disasm;
  bool bblock;
  //bool terminated;
};

struct BranchInfo
{
  enum { BNone = 0, Direct, Indirect };
  
  BranchInfo() : address(), target(BNone), pass(false) {}
  template <class X> void update( bool branch, X const& x ) { update( branch, x.determined, x.value ); }
  void update( bool branch, bool known, uint64_t target );
  
  uint64_t address;
  unsigned target : 2;
  unsigned pass : 1;
};

template <class OP, uint64_t SZ>
struct OpPage
{
  enum { size = SZ };
  OP* ops[SZ];
  OpPage() : ops() {}
  OP** begin() { return &ops[0]; }
  OP** end() { return &ops[SZ]; }
  ~OpPage() { for (OP* o : *this) delete o;  }
};

template <unsigned N>
bool regname( char const (&ref)[N], char const* id, uintptr_t size )
{
  return size == (N-1) and std::equal( &ref[0], &ref[N-1], id );
}

#endif /* __VLE4FUZR_EMU_HH__ */

