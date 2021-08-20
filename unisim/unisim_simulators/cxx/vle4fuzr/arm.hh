/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#ifndef __VLE4FUZR_ARM_HH__
#define __VLE4FUZR_ARM_HH__

#include <emu.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/isa_arm32.hh>
#include <unisim/component/cxx/processor/arm/isa_thumb.hh>
#include <unisim/util/symbolic/symbolic.hh>

struct ARMv7cfg
{
  //=====================================================================
  //=                  ARM architecture model description               =
  //=====================================================================

  // Following a standard armv7 configuration
  static uint32_t const model = unisim::component::cxx::processor::arm::ARMV7;
  static bool const     insns4T = true;
  static bool const     insns5E = true;
  static bool const     insns5J = true;
  static bool const     insns5T = true;
  static bool const     insns6  = true;
  static bool const     insnsRM = true;
  static bool const     insnsT2 = true;
  static bool const     insns7  = true;
  static bool const     hasVFP  = true;
  static bool const     hasAdvSIMD = false;
};

struct ArmProcessor
  : public Processor
  , public unisim::component::cxx::processor::arm::CPU<ARMv7cfg>
{
  typedef unisim::component::cxx::processor::arm::CPU<ARMv7cfg> CP15CPU;
  //  typedef typename CP15CPU::CP15Reg CP15Reg;
  typedef BranchInfo InsnBranch;

  ArmProcessor( char const* name, bool is_thumb );
  ~ArmProcessor();

  virtual void Sync() override { struct No {}; throw No(); }
  
  static unisim::component::cxx::processor::arm::isa::arm32::Decoder<ArmProcessor> arm32_decoder;
  static unisim::component::cxx::processor::arm::isa::thumb::Decoder<ArmProcessor> thumb_decoder;
  
  static ArmProcessor& Self( Processor& proc ) { return dynamic_cast<ArmProcessor&>( proc ); }

  Processor::RegView const* get_reg(char const* id, uintptr_t size, int regid) override;

  enum { OPPAGESIZE = 4096 };
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor> AOperation;
  typedef OpPage<AOperation,OPPAGESIZE> AOpPage;
  typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor> TOperation;
  typedef OpPage<TOperation,OPPAGESIZE> TOpPage;

  std::map< uint32_t, AOpPage > arm32_op_cache;
  std::map< uint32_t, TOpPage > thumb_op_cache;

  enum mem_acc_type_t { mat_write = 0, mat_read, mat_exec };

  uint32_t MemURead32( uint32_t address ) { return PerformUReadAccess( address, 4 ); }
  uint32_t MemRead32( uint32_t address ) { return PerformReadAccess( address, 4 ); }
  uint32_t MemURead16( uint32_t address ) { return PerformUReadAccess( address, 2 ); }
  uint32_t MemRead16( uint32_t address ) { return PerformReadAccess( address, 2 ); }
  uint32_t MemRead8( uint32_t address ) { return PerformReadAccess( address, 1 ); }
  void     MemUWrite32( uint32_t address, uint32_t value ) { PerformUWriteAccess( address, 4, value ); }
  void     MemWrite32( uint32_t address, uint32_t value ) { PerformWriteAccess( address, 4, value ); }
  void     MemUWrite16( uint32_t address, uint16_t value ) { PerformUWriteAccess( address, 2, value ); }
  void     MemWrite16( uint32_t address, uint16_t value ) { PerformWriteAccess( address, 2, value ); }
  void     MemWrite8( uint32_t address, uint8_t value ) { PerformWriteAccess( address, 1, value ); }

  void     PerformPrefetchAccess( uint32_t addr ) {}
  void     PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value );
  uint32_t PerformReadAccess( uint32_t addr, uint32_t size );
  void     PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value );
  uint32_t PerformUReadAccess( uint32_t addr, uint32_t size );

  void     SetExclusiveMonitors( uint32_t addr, unsigned size ) { throw 0; }
  bool     ExclusiveMonitorsPass( uint32_t addr, unsigned size ) { throw 0; return true; }
  void     ClearExclusiveLocal() { throw 0; }

  uint32_t ReadInsn( uint32_t address );
  
  virtual void run( uint64_t begin, uint64_t until, uint64_t count ) override;
  virtual unsigned endian_mask(unsigned size) const override
  { return (size-1)*(GetEndianness() == unisim::util::endian::E_BIG_ENDIAN); }
  virtual char const* get_asm() override;

  template <class Decoder> void Step(Decoder&);
  template <class Decoder> void Disasm(Decoder&);

  void     UndefinedInstruction( unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor>* insn );
  void     UndefinedInstruction( unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor>* insn );
  void     UnpredictableInsnBehaviour();
  void     CallSupervisor( uint32_t imm );
  virtual void FPTrap( unsigned fpx ) override;
  void     BKPT( int );

  // TODO:
  // SetCPSR => CPSRSetByInstriction
  // CP15 functions have access to CPU (already do privilege checks enough)
  // uint32_t    CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  // void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, uint32_t value );
  // Huge issues with Mode system (gives access to system behavior) { checked by nature ? }
  
};


#endif /* __VLE4FUZR_ARM_HH__ */
