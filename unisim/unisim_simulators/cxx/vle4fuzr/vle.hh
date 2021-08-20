/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */


#ifndef __VLE4FUZR_VLE_HH__
#define __VLE4FUZR_VLE_HH__

#include "emu.hh"
#include "xvalue.hh"
#include <unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point/integer.hh>
#include <unisim/component/cxx/processor/powerpc/isa/book_vle/vle.hh>
#include <unisim/component/cxx/processor/powerpc/disasm.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <map>

typedef Processor EmuProcessor;

namespace vle {
  
  struct PPCBase
  {
    struct ProgramInterrupt
    {
      struct UnimplementedInstruction {};
      struct IllegalInstruction {};
      struct Trap {};
      struct PrivilegeViolation {};
    };

    struct SystemCallInterrupt
    {
      struct SystemCall { void SetELEV(unsigned x) {} };
    };
    
    struct AlignmentInterrupt
    {
      struct UnalignedLoadStoreMultiple {};
    };
  };
  
namespace concrete {

  struct Operation;

  typedef bool  BOOL;
  typedef uint8_t   U8;
  typedef uint16_t  U16;
  typedef uint32_t  U32;
  typedef uint64_t  U64;
  typedef int8_t    S8;
  typedef int16_t   S16;
  typedef int32_t   S32;
  typedef int64_t   S64;

  typedef uint32_t  UINT;
  typedef int32_t   SINT;
  typedef uint32_t  ADDRESS;

  struct XER
  {
    typedef unisim::component::cxx::processor::powerpc::XER::OV   OV;
    typedef unisim::component::cxx::processor::powerpc::XER::SO   SO;
    typedef unisim::component::cxx::processor::powerpc::XER::CA   CA;
    typedef unisim::component::cxx::processor::powerpc::XER::_0_3 _0_3;

    template <typename PART> void Set( U32 value ) { PART::Set(xer_value, value); }
    template <typename PART> U32 Get() { return PART::Get(xer_value); }
    operator U32 () { return xer_value; }
    XER& operator= ( U32 value ) { xer_value = value; return *this; }
    XER& GetXER() { return *this; }
    
    XER() : xer_value() {}
    U32 xer_value;
  };

  struct CR
  {
    typedef unisim::component::cxx::processor::powerpc::CR::CR0 CR0;
    typedef unisim::component::cxx::processor::powerpc::CR::CR1 CR1;
    typedef unisim::component::cxx::processor::powerpc::CR::CR2 CR2;
    typedef unisim::component::cxx::processor::powerpc::CR::CR3 CR3;
    typedef unisim::component::cxx::processor::powerpc::CR::CR4 CR4;
    typedef unisim::component::cxx::processor::powerpc::CR::CR5 CR5;
    typedef unisim::component::cxx::processor::powerpc::CR::CR6 CR6;
    typedef unisim::component::cxx::processor::powerpc::CR::CR7 CR7;

    template <typename PART> void Set( U32 value ) { PART::Set(cr_value,value); }
    template <typename PART> U32 Get() { return PART::Get(cr_value); }
    operator U32 () { return cr_value; }
    CR& operator= ( U32 const& value ) { cr_value = value; return *this; }
    CR& GetCR() { return *this; }
    
    CR() : cr_value() {}
    U32 cr_value;
  };
  
  struct LR
  {
    operator U32 () { return lr_value; }
    LR& operator= (U32 const& value) { lr_value = value; return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U32 const& value) { lr_value = value; }
    LR() : lr_value() {}
    U32 lr_value;
  };
  
  struct CTR
  {
    operator U32 () { return ctr_value; }
    CTR& operator= (U32 const& value) { ctr_value = value; return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U32 const& value) { ctr_value = value; }
    CTR() : ctr_value() {}
    U32 ctr_value;
  };
  
  struct MSR
  {
    struct PR {};
    struct EE {};
    struct TODO {};
    
    template <typename P> void Set( U32 value ) { throw TODO(); }
    template <typename PART> U32 Get() { throw TODO(); return U32(); }
    operator U32 () { throw TODO(); return U32(); }
    MSR& operator= (U32 const& v) { throw TODO(); return *this; }
    MSR& GetMSR() { return *this; }
  };

  struct Processor
    : public EmuProcessor, PPCBase
    , public XER, public CR, public MSR, public LR, public CTR
  {
    struct TODO {};
    
    template <class T> T ThrowException() { return DispatchException( T() ); }

    template <class T> T DispatchException( T const& exc ) { throw TODO(); return T(); }

    struct SystemCallInterrupt
    {
      struct SystemCall
      {
        SystemCall( Processor* _proc ) : proc(_proc) {}
        SystemCall() : proc() {}
        void SetELEV(unsigned x) { proc->syscall_hooks(proc->GetCIA(), x); }
        Processor* proc;
      };
    };
    
    SystemCallInterrupt::SystemCall DispatchException( SystemCallInterrupt::SystemCall const& exc )
    {
      return SystemCallInterrupt::SystemCall(this);
    }
    
    Processor();

    static Processor& Self( EmuProcessor& proc ) { return dynamic_cast<Processor&>( proc ); }
  
    EmuProcessor::RegView const* get_reg(char const* id, uintptr_t size, int regid) override;
    
    enum { OPPAGESIZE = 4096 };
    typedef vle::concrete::Operation Operation;
    typedef OpPage<Operation,OPPAGESIZE> InsnPage;

    std::map< uint32_t, InsnPage > insn_cache;

    bool Cond(bool c) { return c; }
    
    U32          reg_values[32];
    U32          cia, nia;
    
    U32 GetGPR(unsigned n) { return reg_values[n]; };
    void SetGPR(unsigned n, U32 value) { reg_values[n] = value; }
    U32 GetCIA() { return cia; };
    U32 GetNIA() { return nia; }
    void Branch(U32 addr) { nia = addr; }

    U32 Fetch(U32 address)
    {
      uint64_t value = 0;
      PhysicalFetchMemory( address, 4, 3, &value );
      return value;
    }
    
    U32 MemRead(U32 address, unsigned size, bool sext, bool bigendian)
    {
      unsigned mask = size-1;
      if (size > 4 or mask & size) { throw "illegal size"; }
      uint64_t result = 0;
      PhysicalReadMemory( address, size, bigendian*mask, &result );
      if (sext)
        {
          int sh = (32 - size*8);
          return U32(int32_t(result << sh) >> sh);
        }
      return U32(result);
    }
    void MemWrite( U32 address, unsigned size, U32 value, bool bigendian )
    {
      unsigned mask = size-1;
      if (size > 4 or mask & size) { throw "illegal size"; }
      PhysicalWriteMemory( address, size, bigendian*mask, value );
    }
    
    bool Int8Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 1, false, true)); return true; }
    bool Int16Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 2, false, true)); return true; }
    bool Int32Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 4, false, true)); return true; }
    
    bool SInt8Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 1, true, true)); return true; }
    bool SInt16Load(unsigned n, U32 address) { SetGPR(n, MemRead(address, 2, true, true)); return true; }

    bool Int16LoadByteReverse(unsigned n, U32 address) { SetGPR(n, MemRead(address, 2, false, false)); return true; }
    bool Int32LoadByteReverse(unsigned n, U32 address) { SetGPR(n, MemRead(address, 4, false, false)); return true; }
    
    bool Int8Store(unsigned n, U32 address ) { MemWrite(address, 1, GetGPR(n),true); return true; }
    bool Int16Store(unsigned n, U32 address ) { MemWrite(address, 2, GetGPR(n), true); return true; }
    bool Int32Store(unsigned n, U32 address ) { MemWrite(address, 4, GetGPR(n), true); return true; }

    bool Int16StoreByteReverse(unsigned n, U32 address ) { MemWrite( address, 2, GetGPR(n), false); return true; }
    bool Int32StoreByteReverse(unsigned n, U32 address ) { MemWrite( address, 4, GetGPR(n), false); return true; }

    bool Rfmci() { throw TODO(); return false; }
    bool Rfci() { throw TODO(); return false; }
    bool Rfdi() { throw TODO(); return false; }
    bool Rfi() { throw TODO(); return false; }

    bool Dcba(U32 const& addr) { throw TODO(); return false; }
    bool Dcbf(U32 const& addr) { throw TODO(); return false; }
    bool Dcbst(U32 const& addr) { throw TODO(); return false; }
    bool Dcbz(U32 const& addr) { throw TODO(); return false; }
    bool Dcbi(U32 const& addr) { throw TODO(); return false; }
    bool Icbi(U32 const& addr) { throw TODO(); return false; }
    bool Icbt(U32 const& addr) { throw TODO(); return false; }
    
    bool Msync() { throw TODO(); return false; }
    bool Isync() { throw TODO(); return false; }
    bool Mpure() { throw TODO(); return false; }
    bool Mpuwe() { throw TODO(); return false; }
    bool Mpusync() { throw TODO(); return false; }
    
    bool Lbarx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Lharx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Lwarx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Stbcx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Sthcx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool Stwcx(unsigned n, U32 const& addr) { throw TODO(); return false; }
    bool MoveFromDCR(unsigned dcrn, U32& result) { throw TODO(); return false; }
    bool MoveFromSPR(unsigned dcrn, U32& result) { throw TODO(); return false; }
    bool MoveToSPR(unsigned dcrn, U32 const& result) { throw TODO(); return false; }
    
    bool CheckSPV() { return true; }
    bool Wait() { throw TODO(); return false; }
    
    virtual void run( uint64_t begin, uint64_t until, uint64_t count ) override;
    virtual unsigned endian_mask(unsigned size) const override { return size-1; }
    virtual char const* get_asm() override;
    
  };
  
  using unisim::util::arithmetic::BitScanReverse;
  using unisim::util::arithmetic::RotateLeft;

} // end of namespace concrete

  using unisim::component::cxx::processor::powerpc::GPRPrint;
  using unisim::component::cxx::processor::powerpc::HexPrint;
  using unisim::component::cxx::processor::powerpc::CRPrint;
  using unisim::component::cxx::processor::powerpc::CondPrint;
  using unisim::component::cxx::processor::powerpc::EAPrint;
  using unisim::component::cxx::processor::powerpc::Mask64;
  using unisim::component::cxx::processor::powerpc::scaled_immediate;

namespace branch
{
  struct Operation;

  typedef x::XValue<bool>     BOOL;
  typedef x::XValue<uint8_t>  U8;
  typedef x::XValue<uint16_t> U16;
  typedef x::XValue<uint32_t> U32;
  typedef x::XValue<uint64_t> U64;
  typedef x::XValue<int8_t>   S8;
  typedef x::XValue<int16_t>  S16;
  typedef x::XValue<int32_t>  S32;
  typedef x::XValue<int64_t>  S64;

  typedef x::XValue<uint32_t> UINT;
  typedef x::XValue<int32_t>  SINT;
  typedef x::XValue<uint32_t> ADDRESS;

  struct XER
  {
    typedef unisim::component::cxx::processor::powerpc::XER::OV   OV;
    typedef unisim::component::cxx::processor::powerpc::XER::SO   SO;
    typedef unisim::component::cxx::processor::powerpc::XER::CA   CA;
    typedef unisim::component::cxx::processor::powerpc::XER::_0_3 _0_3;

    template <typename P, typename T> void Set( T value ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    XER& operator= ( U32 value ) { return *this; }
    XER& GetXER() { return *this; }
  };

  struct CR
  {
    typedef unisim::component::cxx::processor::powerpc::CR::CR0 CR0;
    typedef unisim::component::cxx::processor::powerpc::CR::CR1 CR1;
    typedef unisim::component::cxx::processor::powerpc::CR::CR2 CR2;
    typedef unisim::component::cxx::processor::powerpc::CR::CR3 CR3;
    typedef unisim::component::cxx::processor::powerpc::CR::CR4 CR4;
    typedef unisim::component::cxx::processor::powerpc::CR::CR5 CR5;
    typedef unisim::component::cxx::processor::powerpc::CR::CR6 CR6;
    typedef unisim::component::cxx::processor::powerpc::CR::CR7 CR7;

    template <typename P, typename T> void Set( T value ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    CR& operator= ( U32 const& value ) { return *this; }
    CR& GetCR() { return *this; }
  };
  
  struct LR
  {
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    LR& operator= (U32 const& v) { return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U32 const& v) {}
  };
  
  struct CTR
  {
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    CTR& operator= (U32 const& v) { return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U32 const& v) {}
  };
  
  struct MSR
  {
    struct PR {};
    struct EE {};
    
    template <typename P, typename T> void Set( T value ) {}
    template <typename PART> U32 Get() { return U32(); }
    operator U32 () { return U32(); }
    MSR& operator= (U32 const& v) { return *this; }
    MSR& GetMSR() { return *this; }
  };

  struct ActionNode : public unisim::util::symbolic::Choice<ActionNode> {};
  
  struct Processor
    : public PPCBase
    , public XER, public CR, public MSR, public LR, public CTR
  {
    template <class T> T ThrowException() { return T(); }

    Processor( ActionNode& root, uint32_t addr, uint32_t length );

    static Processor& Self( EmuProcessor& proc ) { return dynamic_cast<Processor&>( proc ); }
  
    bool Cond(bool c) { return c; }
    template <typename T>
    bool Cond( x::XValue<T> const& cond )
    {
      BOOL c = BOOL(cond);
      if (c.determined) return c.value;
      return this->concretize();
    }
    bool concretize()
    {
      bool predicate = path->proceed();
      path = path->next( predicate );
      return predicate;
    }
    
    ActionNode*   path;
    U32           reg_values[32];
    U32           cia, nia;
    bool          has_branch;
    
    U32 GetGPR(unsigned n) { return reg_values[n]; };
    void SetGPR(unsigned n, U32 value) { reg_values[n] = value; }
    U32 GetCIA() { return cia; };
    void Branch(U32 addr) { nia = addr; has_branch = true; }
    
    bool Int8Load(unsigned n, U32 address) { SetGPR(n, U32()); return true; }
    bool Int16Load(unsigned n, U32 address) { SetGPR(n, U32()); return true; }
    bool Int32Load(unsigned n, U32 address) { SetGPR(n, U32()); return true; }
    
    bool SInt8Load(unsigned n, U32 address) { SetGPR(n, U32()); return true; }
    bool SInt16Load(unsigned n, U32 address) { SetGPR(n, U32()); return true; }

    bool Int16LoadByteReverse(unsigned n, U32 address) { SetGPR(n, U32()); return true; }
    bool Int32LoadByteReverse(unsigned n, U32 address) { SetGPR(n, U32()); return true; }
    
    bool Int8Store(unsigned n, U32 address ) { return true; }
    bool Int16Store(unsigned n, U32 address ) { return true; }
    bool Int32Store(unsigned n, U32 address ) { return true; }

    bool Int16StoreByteReverse(unsigned n, U32 address ) { return true; }
    bool Int32StoreByteReverse(unsigned n, U32 address ) { return true; }

    bool Rfmci() { return true; }
    bool Rfci() { return true; }
    bool Rfdi() { return true; }
    bool Rfi() { return true; }

    bool Dcba(U32 const& addr) { return true; }
    bool Dcbf(U32 const& addr) { return true; }
    bool Dcbst(U32 const& addr) { return true; }
    bool Dcbz(U32 const& addr) { return true; }
    bool Dcbi(U32 const& addr) { return true; }
    bool Icbi(U32 const& addr) { return true; }
    bool Icbt(U32 const& addr) { return true; }
    
    bool Msync() { return true; }
    bool Isync() { return true; }
    bool Mpure() { return true; }
    bool Mpuwe() { return true; }
    bool Mpusync() { return true; }
    
    bool Lbarx(unsigned n, U32 const& addr) { return true; }
    bool Lharx(unsigned n, U32 const& addr) { return true; }
    bool Lwarx(unsigned n, U32 const& addr) { return true; }
    bool Stbcx(unsigned n, U32 const& addr) { return true; }
    bool Sthcx(unsigned n, U32 const& addr) { return true; }
    bool Stwcx(unsigned n, U32 const& addr) { return true; }
    bool MoveFromDCR(unsigned dcrn, U32& result) { return true; }
    bool MoveFromSPR(unsigned dcrn, U32& result) { return true; }
    bool MoveToSPR(unsigned dcrn, U32 const& result) { return true; }
    
    bool CheckSPV() { return true; }
    bool Wait() { return true; }
  };
  
} // end of namespace branch
  
} // end of namespace vle

#endif /* __VLE4FUZR_VLE_HH__ */

