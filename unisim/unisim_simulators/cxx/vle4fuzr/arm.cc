/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *  
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "arm.hh"
#include "xvalue.hh"
#include <unisim/component/cxx/processor/arm/isa_arm32.tcc>
#include <unisim/component/cxx/processor/arm/isa_thumb.tcc>
#include <unisim/component/cxx/processor/arm/cpu.tcc>

ArmProcessor::ArmProcessor( char const* name, bool is_thumb )
  : unisim::kernel::Object( name, 0 )
  , Processor()
  , CPU( name, 0 )
{
  cpsr.Set( unisim::component::cxx::processor::arm::T, uint32_t(is_thumb) );
}

ArmProcessor::~ArmProcessor()
{
}

Processor::RegView const*
ArmProcessor::get_reg(char const* id, uintptr_t size, int regid)
{
  if (regname("apsr",id,size))
    {
      static struct : public RegView
      {
        static uint32_t apsr_mask() { return 0xf80f0000; }
        virtual void write( Processor& proc, int id, uint64_t value ) const
        { Self(proc).SetCPSR(uint32_t(value), apsr_mask()); }
        virtual void read( Processor& proc, int id, uint64_t* value ) const
        { *value = Self(proc).GetCPSR() & apsr_mask(); }
      } _;
      return &_;
    }

  if (regname("gpr",id,size))
    {
      if ((unsigned)regid < 15)
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint64_t value ) const { Self(proc).SetGPR(id, value); }
            void read( Processor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetGPR(id); }
          } _;
          return &_;
        }
      else if (regid == 15)
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint64_t value ) const { Self(proc).DebugBranch(value); }
            void read( Processor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetCIA(); }
          } _;
          return &_;
        }
    }

  std::cerr << "Register '" << std::string(id,size) << "' not found.\n";
  return 0;
}

struct ActionNode : public unisim::util::symbolic::Choice<ActionNode> {};

struct ArmBranch
{
  typedef ArmProcessor::Config Config;
  struct InsnBranch {};
  
  typedef x::XValue<double>   F64;
  typedef x::XValue<float>    F32;
  typedef x::XValue<bool>     BOOL;
  typedef x::XValue<uint8_t>  U8;
  typedef x::XValue<uint16_t> U16;
  typedef x::XValue<uint32_t> U32;
  typedef x::XValue<uint64_t> U64;
  typedef x::XValue<int8_t>   S8;
  typedef x::XValue<int16_t>  S16;
  typedef x::XValue<int32_t>  S32;
  typedef x::XValue<int64_t>  S64;

  ArmBranch( ActionNode& root, uint32_t addr, uint32_t length, bool _thumb )
    : path(&root), r15(addr + (_thumb?4:8)), insn_addr(addr), next_insn_addr(addr+length), thumb(_thumb), has_branch(false)
  {
  }
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  ActionNode* path;
  U32 r15, insn_addr, next_insn_addr;
  bool thumb, has_branch;
  BOOL next_thumb;
  
  U32 GetGPR(int idx) { if (idx != 15) return U32(); return r15; }
  void SetGPR(int idx, U32 val)
  {
    if (idx != 15) return;
    if (thumb) this->Branch( val, B_JMP );
    else this->BranchExchange( val, B_JMP );
  }
  void SetGPR_mem(int idx, U32 val)
  {
    if (idx != 15) return;
    this->BranchExchange( val, B_JMP );
  }
  
  void BranchExchange(U32 target, branch_type_t branch_type)
  {
    next_thumb = BOOL(target & U32(1));
    this->Branch( target, branch_type );
  }
	
  void Branch(U32 target, branch_type_t branch_type)
  {
    this->next_insn_addr = target & U32(thumb ? -2 : -4);
    has_branch = true;
  }
	
  U32 GetCIA() { return this->insn_addr; }
  U32 GetNIA() { return this->next_insn_addr; }

  U32  GetVU32( unsigned idx ) { return U32( 0 ); }
  void SetVU32( unsigned idx, U32 val ) {}
  U64  GetVU64( unsigned idx ) { return U64( 0 ); }
  void SetVU64( unsigned idx, U64 val ) {}
  F32  GetVSR( unsigned idx ) { return F32( 0 ); }
  void SetVSR( unsigned idx, F32 val ) {}
  F64  GetVDR( unsigned idx ) { return F64( 0 ); }
  void SetVDR( unsigned idx, F64 val ) {}

  U32  GetVSU( unsigned idx ) { return U32(); }
  void SetVSU( unsigned idx, U32 val ) {}
  U64  GetVDU( unsigned idx ) { return U64(); }
  void SetVDU( unsigned idx, U64 val ) {}

  struct PSR
  {
    template <typename F, typename V> void Set( F, V ) {}
    template <typename F> U32 Get( F ) { return U32(); }
  } xpsr;
  
  PSR&     CPSR() { return xpsr; };

  void SetCPSR( U32 const&, uint32_t ) {}
  U32 GetCPSR() { return U32(); }
  U32 GetNZCV() { return U32(); }

  struct Mode
  {
    U32 GetSPSR() { return U32(); }
    void SetSPSR(U32) {}
  };
  Mode CurrentMode() { return Mode(); }
  Mode GetMode(int) { return Mode(); }

  struct ITCond {};
  ITCond itcond() const { return ITCond(); }
  bool itblock() { return concretize(); }
  void ITSetState(uint8_t, uint8_t) {}

  template <typename T>
  bool Test( x::XValue<T> const& cond )
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

  U32 MemURead32( U32 const& ) { return U32(); }
  U16 MemURead16( U32 const& ) { return U16(); }
  U32  MemRead32( U32 const& ) { return U32(); }
  U16  MemRead16( U32 const& ) { return U16(); }
  U8    MemRead8( U32 const& ) { return  U8(); }
  
  void MemUWrite32( U32 const&, U32 const& ) {}
  void MemUWrite16( U32 const&, U16 const& ) {}
  void  MemWrite32( U32 const&, U32 const& ) {}
  void  MemWrite16( U32 const&, U16 const& ) {}
  void   MemWrite8( U32 const&,  U8 const& ) {}

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

  template <typename T> void UndefinedInstruction( T ) { /*Illegal branch, ignoring*/ }
  void UnpredictableInsnBehaviour() { /*Illegal branch, ignoring*/ }
  void FPTrap( unsigned exc ) { /*Illegal branch, ignoring*/ }

  void SetBankedRegister( int, int, U32 const& ) {}
  U32  GetBankedRegister( int, int ) { return U32(); }

  void SetExclusiveMonitors( U32 const&, int ) {}
  bool ExclusiveMonitorsPass( U32 const&, int ) { return true; }
  void ClearExclusiveLocal() {}
  bool IntegerZeroDivide( BOOL const& ) { return false; }
  void BKPT( int ) {  }

  void WaitForInterrupt() {}
  
  U32  CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return U32(); }
  void CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 value ) {}
  char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return "no"; }

  void CallSupervisor( uint32_t imm ) {}

  U32 FPSCR, FPEXC;
  U32 RoundTowardsZeroFPSCR() { return U32(); }
  U32 RoundToNearestFPSCR() { return U32(); }
  
  struct FP
  {
    template <typename T> static T Abs(T) { return T(); }
    template <typename T> static T Neg(T) { return T(); }
    template <typename T> static void SetDefaultNan( T& ) {}
    template <typename T> static void SetQuietBit( T& ) {}
    template <typename X, typename Y> static void ItoF( X&, Y const&, int, ArmBranch&, U32 const& ) {}
    template <typename X, typename Y> static void FtoI( Y&, X const&, int, ArmBranch&, U32 const& ) {}
    template <typename X, typename Y> static void FtoF( X&, Y const&, ArmBranch&, U32 const& ) {}


    template <typename T> static void Add( T&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Sub( T&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Div( T&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Mul( T&, T const&, ArmBranch&, U32 const& ) {}
    
    template <typename T> static BOOL IsSNaN(T const&) { return BOOL(); }
    template <typename T> static BOOL IsQNaN(T const&) { return BOOL(); }
    template <typename T> static BOOL IsInvalidMulAdd( T&, T const&, T const&, U32 const& ) { return BOOL(); }
    template <typename T> static BOOL FlushToZero( T&, U32 const& ) { return BOOL(); }
    template <typename T> static void MulAdd( T&, T const&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Sqrt( T&, ArmBranch&, U32 const& ) {}

    template <typename T> static S32 Compare( T const&, T const&, U32 const& ) { return S32(); }
  };
};

inline bool CheckCondition( ArmBranch& ab, ArmBranch::ITCond const& ) { return ab.concretize(); }

template <class T> struct AMO {};

template <class CPU> using Arm32Decoder = unisim::component::cxx::processor::arm::isa::arm32::Decoder<CPU>;
template <class CPU> using ThumbDecoder = unisim::component::cxx::processor::arm::isa::thumb::Decoder<CPU>;

template <> struct AMO< Arm32Decoder<ArmProcessor> >
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor> Operation;
  enum { thumb = 0 };
  typedef ArmProcessor::AOpPage OpPage;
  static OpPage& GetOpPage(ArmProcessor& ap, uint32_t tag) { return ap.arm32_op_cache[tag]; }
  static Arm32Decoder<ArmBranch> bdecoder;
};

Arm32Decoder<ArmProcessor> ArmProcessor::arm32_decoder;
Arm32Decoder<ArmBranch> AMO< Arm32Decoder<ArmProcessor> >::bdecoder;

template <> struct AMO< ThumbDecoder<ArmProcessor> >
{
  typedef unisim::component::cxx::processor::arm::isa::thumb::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor> Operation;
  enum { thumb = 1 };
  typedef ArmProcessor::TOpPage OpPage;
  static OpPage& GetOpPage(ArmProcessor& ap, uint32_t tag) { return ap.thumb_op_cache[tag]; }
  static ThumbDecoder<ArmBranch> bdecoder;
};

ThumbDecoder<ArmProcessor> ArmProcessor::thumb_decoder;
ThumbDecoder<ArmBranch> AMO< ThumbDecoder<ArmProcessor> >::bdecoder;

template <typename Decoder>
void
ArmProcessor::Step( Decoder& decoder )
{
  typedef typename AMO<Decoder>::CodeType CodeType;
  typedef typename AMO<Decoder>::Operation Operation;
  typedef typename AMO<Decoder>::OpPage OpPage;

  // Instruction boundary next_insn_addr becomes current_insn_addr
  uint32_t insn_addr = this->current_insn_addr = this->next_insn_addr, insn_length = 0;
  
  // Fetch 
  CodeType insn = ReadInsn(insn_addr);

  // Decode
  uint32_t insn_idx = insn_addr/(AMO<Decoder>::thumb ? 2 : 4),
    insn_tag = insn_idx / ArmProcessor::OPPAGESIZE, insn_offset = insn_idx % ArmProcessor::OPPAGESIZE;
  OpPage& page = AMO<Decoder>::GetOpPage(*this, insn_tag);
  Operation* op = page.ops[insn_offset];
  if (op and op->GetEncoding() == insn)
    { insn_length = op->GetLength() / 8; }
  else
    {
      delete op;
      op = page.ops[insn_offset] = decoder.NCDecode( insn_addr, insn );
      insn_length = op->GetLength() / 8;
        
      {
        auto bop = AMO<Decoder>::bdecoder.NCDecode( insn_addr, insn );
          
        ActionNode root;
        for (bool end = false; not end;)
          {
            ArmBranch ab( root, insn_addr, insn_length, AMO<Decoder>::thumb );
            bop->execute( ab );
            op->branch.update( ab.has_branch, ab.next_insn_addr );
            end = ab.path->close();
          }
    
        delete bop;
      }
    }

  // Monitor
  if (this->disasm)
    {
      op->disasm(*this, std::cerr << std::hex << insn_addr << ": (" << ("AT"[AMO<Decoder>::thumb]) << ") " );
      std::cerr << std::endl;
    }

  insn_hooks(insn_addr, insn_length);

  // Execute
  this->gpr[15] = insn_addr + (AMO<Decoder>::thumb ? 4 : 8);
  this->next_insn_addr = insn_addr + insn_length;

  op->execute( *this );

  if (AMO<Decoder>::thumb)
    this->ITAdvance();

  if (debug_branch != uint64_t(-1))
    {
      this->bblock = true;
      BranchExchange(debug_branch, B_DBG);
      DebugBranch(-1);
    }
  else
    this->bblock = (op->branch.target != op->branch.BNone);
}

template <typename Decoder>
void
ArmProcessor::Disasm( Decoder& decoder )
{
  typedef typename AMO<Decoder>::Operation Operation;
  typedef typename AMO<Decoder>::OpPage OpPage;

  // Fetch
  uint32_t insn_addr = this->current_insn_addr, insn_idx = insn_addr/(AMO<Decoder>::thumb ? 2 : 4),
    insn_tag = insn_idx / ArmProcessor::OPPAGESIZE,
    insn_offset = insn_idx % ArmProcessor::OPPAGESIZE;
  OpPage& page = AMO<Decoder>::GetOpPage(*this, insn_tag);
  
  if (Operation* op = page.ops[insn_offset])
    {
      std::ostringstream buf;
      op->disasm(*this, buf);
      std::cerr << std::endl;
      asmbuf = buf.str();
    }
  else
    asmbuf = "?";
}

char const*
ArmProcessor::get_asm()
{
  if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
    Disasm(thumb_decoder);
  else
    Disasm(arm32_decoder);
  
  return asmbuf.c_str();
}

void
ArmProcessor::run( uint64_t begin, uint64_t until, uint64_t count )
{
  this->Branch(begin, B_DBG);
  this->bblock = true;

  do
    {
      if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
        Step(thumb_decoder);
      else
        Step(arm32_decoder);
    }
  while (next_insn_addr != until and --count != 0);
}

uint32_t
ArmProcessor::ReadInsn(uint32_t address)
{
  uint64_t value = 0;
  PhysicalFetchMemory( address, 4, 0, &value );
  return value;
}

void
ArmProcessor::UndefinedInstruction( unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor>* insn )
{
  insn->disasm(*this, std::cerr << "Undefined instruction @" << std::hex << current_insn_addr << std::dec << ": " );
  std::cerr << std::endl;
  abort("ProcessorException('undefined instruction')");
}

void
ArmProcessor::UndefinedInstruction( unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor>* insn )
{
  insn->disasm(*this, std::cerr << "Undefined instruction @" << std::hex << current_insn_addr << std::dec << ": " );
  std::cerr << std::endl;
  abort("ProcessorException('undefined instruction')");
}

void
ArmProcessor::PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment and not unisim::component::cxx::processor::arm::sctlr::A.Get( this->SCTLR )))
    {
      uint32_t eaddr = addr;
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
        for (unsigned byte = size; --byte < size; ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      } else {
        for (unsigned byte = 0; byte < size; ++byte, ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      }
      return;
    }
  else
    PerformWriteAccess( addr, size, value );
}

void
ArmProcessor::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment))
    MemoryException(1/*Write*/, addr, "misalignment");

  unsigned endianness = lo_mask*(GetEndianness() == unisim::util::endian::E_BIG_ENDIAN);
  PhysicalWriteMemory( addr, size, endianness, value );
}

uint32_t
ArmProcessor::PerformUReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment and not unisim::component::cxx::processor::arm::sctlr::A.Get( this->SCTLR )))
    {
      uint32_t result = 0;
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
        for (unsigned byte = 0; byte < size; ++byte)
          result = (result << 8) | PerformReadAccess( addr + byte, 1 );
      } else {
        for (unsigned byte = size; --byte < size;)
          result = (result << 8) | PerformReadAccess( addr + byte, 1 );
      }
      return result;
    }
  else
    return PerformReadAccess( addr, size );
}

uint32_t
ArmProcessor::PerformReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment))
    MemoryException(0/*Read*/, addr, "misalignment");

  uint64_t value = 0;
  unsigned endianness = lo_mask*(GetEndianness() == unisim::util::endian::E_BIG_ENDIAN);
  PhysicalReadMemory(addr, size, endianness, &value);
  
  return value;
}

void
ArmProcessor::CallSupervisor( uint32_t imm )
{
  syscall_hooks( this->current_insn_addr, imm );
}

void
ArmProcessor::UnpredictableInsnBehaviour()
{
  abort("Unpredictable()");
}

void
ArmProcessor::FPTrap( unsigned fpx )
{
  std::ostringstream buf;
  buf << "ProcessorException('floating-point'," << std::dec << fpx << ")";
  abort(buf.str());
}

void
ArmProcessor::BKPT( int bkpt )
{
  std::ostringstream buf;
  buf << "ProcessorException('breakpoint'," << std::dec << bkpt << ")";
  abort(buf.str());
}
