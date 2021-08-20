/*
 *  Copyright (c) 2015,
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

#ifndef ARCH_HH
#define ARCH_HH

#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <map>
#include <vector>
#include <list>

namespace ut
{

  template <typename operT, typename fpscrT> void FloatFlushToZero( operT& op, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatAdd( operT& res, operT op1, operT op2, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatSub( operT& res, operT op1, operT op2, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatDiv( operT& res, operT op1, operT op2, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatMul( operT& res, operT op1, operT op2, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatMulAdd( operT& acc, operT op1, operT op2, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatNeg( operT& res, operT op, fpscrT& fpscr ) {}

  template <typename fpT, typename intT, typename fpscrT> void FloatItoF( fpT& res, intT op, int fracbits, fpscrT& fpscr ) {}

  template <typename intT, typename fpT, typename fpscrT> void FloatFtoI( intT& res, fpT op, int fracbits, fpscrT& fpscr ) {}

  template <typename ofpT, typename ifpT, typename fpscrT> void FloatFtoF( ofpT& res, ifpT op, fpscrT& fpscr ) {}

  template <typename fpscrT> void FloatAbs( double& res, double op, fpscrT& fpscr ) {}

  template <typename fpscrT> void FloatAbs( float& res, float op, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> bool FloatIsSNaN( operT op, fpscrT const& fpscr ) { return false; }

  template <typename operT, typename fpscrT> bool FloatIsQNaN( operT op, fpscrT const& fpscr ) { return false; }

  template <typename operT, typename fpscrT> void FloatSetQuietBit( operT& op, fpscrT const& fpscr ) {}

  template <typename fpscrT> void FloatSetDefaultNan( double& result, fpscrT const& fpscr ) {}

  template <typename fpscrT> void FloatSetDefaultNan( float& result, fpscrT const& fpscr ) {}

  template <typename operT, typename fpscrT> void FloatSqrt( operT& res, operT const& op, fpscrT& fpscr ) {}

  template <typename operT, typename fpscrT> int FloatCompare( operT op1, operT op2, fpscrT& fpscr ) { return 0; }

  typedef double   F64;
  typedef float    F32;
  typedef unisim::util::symbolic::SmartValue<bool>     BOOL;
  typedef unisim::util::symbolic::SmartValue<uint8_t>  U8;
  typedef unisim::util::symbolic::SmartValue<uint16_t> U16;
  typedef unisim::util::symbolic::SmartValue<uint32_t> U32;
  typedef unisim::util::symbolic::SmartValue<uint64_t> U64;
  typedef unisim::util::symbolic::SmartValue<int8_t>   S8;
  typedef unisim::util::symbolic::SmartValue<int16_t>  S16;
  typedef unisim::util::symbolic::SmartValue<int32_t>  S32;
  typedef unisim::util::symbolic::SmartValue<int64_t>  S64;

  struct UniqueVId {
    UniqueVId() : index(0) {} unsigned index;
    unsigned next() { return index++; }
    void reset() { index = 0; }
  };
  
  struct VirtualRegister
  {
    VirtualRegister() : vindex(0), source(false), destination(false), bad(true) {}
    VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
    uint8_t vindex      : 5;
    uint8_t source      : 1;
    uint8_t destination : 1;
    uint8_t bad         : 1;
    
    void allocate( UniqueVId& uvi ) { if (not bad) throw 0; vindex = uvi.next(); bad = false; }
    void addaccess( bool w ) { source |= not w; destination |= w; }
    
    int  cmp( VirtualRegister const& ) const;
  };
  
  struct Interface
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    Interface()
      : irmap(), iruse(), load_addrs(), store_addrs()
      , psr(0), length(0), itsensitive(false), base_register(-1), aligned(true)
    {}
    
    std::map<unsigned,VirtualRegister> irmap;
    std::vector<unsigned>              iruse;
    std::list<Expr>                    load_addrs;
    std::list<Expr>                    store_addrs;
    VirtualRegister                    psr;
    uint8_t                            length;
    bool                               itsensitive;
    uint8_t                            base_register;
    bool                               aligned;
    
    void irappend( uint8_t _index, bool w, UniqueVId& uvi );
    void readflags() { psr.addaccess( false ); }
    void writeflags() { psr.addaccess( true ); }
    
    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    bool usemem() const { return store_addrs.size() or load_addrs.size(); }
    
    void finalize( uint8_t _length );
    
    struct Prologue
    {
      struct Error {};
      
      typedef std::map<unsigned,uint32_t> Regs;
      Prologue( Regs const& _regs, uint32_t _offset, bool _sign, uint8_t _base )
        : regs( _regs ), offset( _offset ), sign( _sign ), base( _base )
      {}
      Regs regs; uint32_t offset; bool sign; uint8_t base;
    };
    
    Prologue GetPrologue() const;
    
    void PCRelPrologue( Prologue const& pc ) const;
    struct Untestable
    {
      std::string argument;
      Untestable( std::string part ) : argument( part ) {}
      Untestable( std::string part, Untestable const& ut ) : argument( part + ' ' + ut.argument ) {}
    };
  };
  
  struct SourceReg : public unisim::util::symbolic::ExprNode
  {
    SourceReg( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    virtual SourceReg* Mutate() const { return new SourceReg(*this); }
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 0; }
    virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<SourceReg const&>( rhs ) ); }
    int compare( SourceReg const& rhs ) const { return int(reg) - int(rhs.reg); }
    typedef unisim::util::symbolic::ScalarType ScalarType;
    virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
  };
  
  struct Arch
  {
    typedef ut::F64  F64;
    typedef ut::F32  F32;
    typedef ut::BOOL BOOL;
    typedef ut::U8   U8;
    typedef ut::U16  U16;
    typedef ut::U32  U32;
    typedef ut::U64  U64;
    typedef ut::S8   S8;
    typedef ut::S16  S16;
    typedef ut::S32  S32;
    typedef ut::S64  S64;
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    bool Test( BOOL const& _cond_expr ) const { return false; /* TODO WTF ?*/ }
    
    struct Config
    {
      // typedef ut::Arch::FPSCRTracer FPSCR;
      // typedef ut::Arch Arch;
  
      // Following a standard armv7 configuration
      static uint32_t const model = unisim::component::cxx::processor::arm::ARMEMU;
      static bool const     insns4T = true;
      static bool const     insns5E = true;
      static bool const     insns5J = true;
      static bool const     insns5T = true;
      static bool const     insns6  = true;
      static bool const     insnsRM = false;
      static bool const     insnsT2 = true;
      static bool const     insns7  = true;
  
      struct DisasmState {};
    };

    Arch() : psr_value( 0 ), psr_ok_mask( 0xf80f0000 ), cpsr(*this), fpscr(*this)
    {
      for (unsigned reg = 0; reg < 16; ++reg)
        reg_values[reg] = U32( new SourceReg( reg ) );
    }
    
    Interface interface;
    UniqueVId gpr_uvi;
    U32   reg_values[16];
    
    void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w, gpr_uvi ); }

    void reject();
    
    void donttest_system();
    U32  GetGPR_usr( uint32_t id ) { /* Only work in system mode instruction */ donttest_system(); return U32(); }
    U32  GetGPR( uint32_t id ) { gpr_append( id, false ); return reg_values[id]; }
    void SetGPR_usr( uint32_t id, U32 const& value ) { /* Only work in system mode instruction */ donttest_system(); }
    void SetGPR_mem( uint32_t id, U32 const& value ) { gpr_append( id, true ); reg_values[id] = value; }
    void SetGPR( uint32_t id, U32 const& value ) { gpr_append( id, true ); reg_values[id] = value; }
    
    struct NPC : public ExprNode
    {
      virtual NPC* Mutate() const { return new NPC(*this); }
      virtual void Repr( std::ostream& sink ) const { sink << "@NextInsn"; }
      virtual unsigned SubCount() const { return 0; }
      virtual int cmp( unisim::util::symbolic::ExprNode const& brhs ) const override { return 0; }
      typedef unisim::util::symbolic::ScalarType ScalarType;
      virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    };
    
    U32 GetNIA() { gpr_append( 15, false ); return U32( new NPC ); }

    // template <typename RF>  uint32_t GetPSR( RF const& rf ) const { return 0; }
    // template <typename RF>  void     SetPSR( RF const& rf, uint32_t value ) {}
    
    struct PSRFlags : public ExprNode
    {
      PSRFlags( uint32_t _mask ) : mask( _mask ) {} uint32_t mask;
      virtual PSRFlags* Mutate() const { return new PSRFlags(*this); }
      virtual void Repr( std::ostream& sink ) const { sink << "PSR_flags"; }
      virtual unsigned SubCount() const { return 0; }
      virtual int cmp( unisim::util::symbolic::ExprNode const& brhs ) const override { return 0; }
      typedef unisim::util::symbolic::ScalarType ScalarType;
      virtual ScalarType::id_t GetType() const { return ScalarType::U32 ; }
    };
    
    uint32_t   psr_value, psr_ok_mask;
    template <typename RF> U32 GetPSR( RF const& rf )
    {
      interface.readflags();
      return U32( new PSRFlags( rf.Mask( uint32_t( -1 ) ) ) );
    }
    
    U32 GetNZCV()
    {
      interface.readflags();
      return U32( new PSRFlags( 0xf0000000 ) );
    };
    
    //void SetPSR( typeof( unisim::component::cxx::processor::arm::ALL32 ) const&, uint32_t value ) { reject(); }
    template <typename RF>
    void SetPSR( RF const& rf )
    {
      interface.writeflags();
      if (rf.Get( ~psr_ok_mask ) != 0) reject();
    }
    
    struct PSRTracer
    {
      PSRTracer( Arch& _arch ) : arch( _arch ) {} Arch& arch;
      template <typename RF>  U32      Get( RF const& rf ) const { return arch.GetPSR( rf ); }
      template <typename RF>  void     Set( RF const& rf, U32 const& value ) { arch.SetPSR( rf ); }
      template <typename RF>  void     Set( RF const& rf, BOOL const& value ) { arch.SetPSR( rf ); }
      U32 bits() const { return arch.GetPSR( unisim::component::cxx::processor::arm::ALL32 ); }
    };
    
    typedef PSRTracer psr_type;
    PSRTracer  cpsr;
    PSRTracer& CPSR() { return cpsr; };
    PSRTracer& SPSR() { /* Only work in system mode instruction */ donttest_system(); return cpsr; };

    U32 GetCPSR() { return cpsr.bits(); }
    void SetCPSR(U32 mask, uint32_t bits) { donttest_system(); }
    
    void SetGPRMapping( uint32_t src_mode, uint32_t tar_mode ) { /* system related */ donttest_system(); }

    template <unisim::util::symbolic::ScalarType::id_t ID>
    struct Load : public ExprNode
    {
      Load( Expr const& _address ) : address( _address ) {} Expr address;
      virtual Load* Mutate() const { return new Load(*this); }
      virtual void Repr( std::ostream& sink ) const { sink << "Load( "; address->Repr( sink ); sink << " )"; }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx!=0) return ExprNode::GetSub(0); return address; }
      virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
      int compare( Load const& brhs ) const { return 0; }
      virtual unisim::util::symbolic::ScalarType::id_t GetType() const { return ID; }
    };
    
    U32 MemRead( Expr const& addr, bool aligned=true )
    {
      interface.load_addrs.push_back( addr );
      interface.aligned &= aligned;
      return U32( new Load<unisim::util::symbolic::ScalarType::U32>( addr ) );
    }
    
    void MemWrite( Expr const& addr, bool aligned=true )
    {
      interface.store_addrs.push_back( addr );
      interface.aligned &= aligned;
    }
    
    U32  MemURead32( U32 const& address ) { return MemRead( address.expr, false ); }
    U32  MemURead16( U32 const& address ) { return MemRead( address.expr, false ); }
    U32  MemRead32( U32 const& address ) { return MemRead( address.expr ); }
    U32  MemRead16( U32 const& address ) { return MemRead( address.expr ); }
    U32  MemRead8( U32 const& address ) { return MemRead( address.expr ); }
    
    void MemUWrite32( U32 const& address, U32 const& value ) { MemWrite( address.expr, false ); }
    void MemUWrite16( U32 const& address, U16 const& value ) { MemWrite( address.expr, false ); }
    void MemWrite32( U32 const& address, U32 const& value ) { MemWrite( address.expr ); }
    void MemWrite16( U32 const& address, U16 const& value ) { MemWrite( address.expr ); }
    void MemWrite8( U32 const& address, U8 const& value ) { MemWrite( address.expr ); }
    
    void SetExclusiveMonitors( U32 const& address, unsigned size ) {}
    bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { return true; }
    void ClearExclusiveLocal() {}
    
    enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
    void donttest_branch();
    bool Check( BOOL condition ) { return true; }
    void BranchExchange( U32 const& target, branch_type_t ) { donttest_branch(); }
    void Branch( U32 const& target, branch_type_t  ) { donttest_branch(); }
    
    void donttest_copro();
    // bool CoprocessorLoad( uint32_t cp_num, uint32_t address) { donttest_copro(); return false; }
    // bool CoprocessorLoad( uint32_t cp_num, uint32_t address, uint32_t option) { donttest_copro(); return false; }
    // bool CoprocessorStore( uint32_t cp_num, uint32_t address) { donttest_copro(); return false; }
    // bool CoprocessorStore( uint32_t cp_num, uint32_t address, uint32_t option) { donttest_copro(); return false; }
    // void CoprocessorDataProcess( uint32_t cp_num, uint32_t op1, uint32_t op2, uint32_t crd, uint32_t crn, uint32_t crm ) { donttest_copro(); }
    // void MoveToCoprocessor( uint32_t cp_num, uint32_t op1, uint32_t op2, uint32_t rd, uint32_t crn, uint32_t crm ) { donttest_copro(); }
    // void MoveFromCoprocessor( uint32_t cp_num, uint32_t op1, uint32_t op2, uint32_t rd, uint32_t crn, uint32_t crm ) { donttest_copro(); }
    // uint32_t Coproc_GetOneWord( unsigned code, unsigned cp_num, unsigned op1, unsigned op2, unsigned crn, unsigned crm ) { donttest_copro(); return 0; }
    
    void WaitForInterrupt() { donttest_system(); }
    void SWI( uint32_t imm ) { donttest_system(); }
    void BKPT( uint32_t imm ) { donttest_system(); }
    void UnpredictableInsnBehaviour() { reject(); }
    template <typename OP>
    void UndefinedInstruction( OP* op ) { reject(); }
    void CallSupervisor( uint32_t imm ) { donttest_system(); }
    void UnpredictableIf( BOOL const& condition ) { donttest_system(); }
    bool IntegerZeroDivide( BOOL const& condition ) { return false; }

    U32  GetVU32( unsigned idx ) { return U32( 0 ); }
    void SetVU32( unsigned idx, U32 val ) {}
    U64  GetVU64( unsigned idx ) { return U64( 0 ); }
    void SetVU64( unsigned idx, U64 val ) {}
    F32  GetVSR( unsigned idx ) { return F32( 0 ); }
    void SetVSR( unsigned idx, F32 val ) {}
    F64  GetVDR( unsigned idx ) { return F64( 0 ); }
    void SetVDR( unsigned idx, F64 val ) {}
  
    struct FPSCRTracer
    {
      FPSCRTracer( Arch& _arch ) : arch( _arch ) {} Arch& arch;
      template <typename RF>  uint32_t Get( RF const& rf ) const { return 0; }
      template <typename RF>  void     Set( RF const& rf, uint32_t value ) {}
    };
  
    FPSCRTracer fpscr;
    FPSCRTracer& FPSCR() { return fpscr; }
  
    uint32_t itcond() const { return this->COND_AL; }
    bool itblock() { interface.itsensitive = true; return false; }
    void ITSetState( uint32_t cond, uint32_t mask );

    // /* masks for the different running modes */
    // static uint32_t const RUNNING_MODE_MASK = 0x1F;
    // static uint32_t const USER_MODE = 0x10;
    // static uint32_t const FIQ_MODE = 0x11;
    // static uint32_t const IRQ_MODE = 0x12;
    // static uint32_t const SUPERVISOR_MODE = 0x13;
    // static uint32_t const ABORT_MODE = 0x17;
    // static uint32_t const UNDEFINED_MODE = 0x1B;
    // static uint32_t const SYSTEM_MODE = 0x1F;
    
    struct Mode
    {
      Mode() {}
      bool     HasBR( unsigned index ) { return false; }
      bool     HasSPSR() { return false; }
      void     SetSPSR(U32 value) {};
      U32      GetSPSR() { return U32(0); };
      void     Swap( Arch& ) {};
      
    } mode;
    
    Mode&  CurrentMode() { donttest_system(); return mode; }
    Mode&  GetMode(uint8_t) { donttest_system(); return mode; }
    U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx ) { donttest_system(); return U32(0); }
    void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value ) { donttest_system(); }
    
    U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { donttest_system(); return U32(0); }
    void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value ) { donttest_system(); }
    char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { donttest_system(); return ""; }
    
    
    static const unsigned PC_reg = 15;
    static const unsigned LR_reg = 14;
    static const unsigned SP_reg = 13;
    
    /* masks for the different running modes */
    static uint32_t const USER_MODE = 0b10000;
    static uint32_t const FIQ_MODE = 0b10001;
    static uint32_t const IRQ_MODE = 0b10010;
    static uint32_t const SUPERVISOR_MODE = 0b10011;
    static uint32_t const MONITOR_MODE = 0b10110;
    static uint32_t const ABORT_MODE = 0b10111;
    static uint32_t const HYPERVISOR_MODE = 0b11010;
    static uint32_t const UNDEFINED_MODE = 0b11011;
    static uint32_t const SYSTEM_MODE = 0b11111;
    
    /* values of the different condition codes */
    static uint32_t const COND_EQ = 0x00;
    static uint32_t const COND_NE = 0x01;
    static uint32_t const COND_CS_HS = 0x02;
    static uint32_t const COND_CC_LO = 0x03;
    static uint32_t const COND_MI = 0x04;
    static uint32_t const COND_PL = 0x05;
    static uint32_t const COND_VS = 0x06;
    static uint32_t const COND_VC = 0x07;
    static uint32_t const COND_HI = 0x08;
    static uint32_t const COND_LS = 0x09;
    static uint32_t const COND_GE = 0x0a;
    static uint32_t const COND_LT = 0x0b;
    static uint32_t const COND_GT = 0x0c;
    static uint32_t const COND_LE = 0x0d;
    static uint32_t const COND_AL = 0x0e;

    /* mask for valid bits in processor control and status registers */
    static uint32_t const PSR_UNALLOC_MASK = 0x00f00000;
  };

}

#endif // ARCH_HH
