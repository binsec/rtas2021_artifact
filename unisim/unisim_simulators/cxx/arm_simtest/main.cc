/*
 *  Copyright (c) 2015-2017,
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

#include <top_arm32.tcc>
#include <top_thumb.tcc>
#include <arch.hh>
#include <testutils.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/isa/decode.hh>
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/util/random/random.hh>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <memory>
#include <inttypes.h>

using namespace unisim::component::cxx::processor::arm;

struct Sink
{
  Sink( std::string const& gendir )
    : macros( gendir + ".inc" ), sources( gendir + ".s" )
  {
    sources   << "\t.syntax\tunified\n\n";
  }
  
  ~Sink() {}
  
  std::ofstream macros;
  std::ofstream sources;
  
  char const* test_prefix() const { return "iut_"; }
};

struct TestConfig
{
  ut::Interface const& iif;
  std::string ident, disasm;
  bool bigendian;
  uint8_t offset;
  
  TestConfig( ut::Interface const& _iif, std::string const& _ident, std::string const& _disasm )
    : iif( _iif ), ident( _ident ), disasm( _disasm ), bigendian( false ), offset( 0 )
  {}
  
  std::string prologue() const
  {
    if (iif.usemem()) {
      std::ostringstream sink;
      try {
        auto const& p = iif.GetPrologue();
        DisasmRegister br( p.base );
        uint32_t aoffset = uint32_t(offset);
        sink << "\tldr\t" << br << ", =0x" << std::hex << (p.offset + (p.sign ? -aoffset : aoffset)) << std::dec << "\n";
        if (p.sign)
          sink << "\tsub\t" << br << ", " << br << ", r4\n";
        else
          sink << "\tadd\t" << br << ", " << br << ", r4\n";
        for (auto reg : p.regs) {
          sink << "\tldr\t" << DisasmRegister( reg.first ) << ", =0x" << std::hex << reg.second << std::dec << "\n";
        }
        if (bigendian) sink << "\tsetend\tbe\n";
      } catch (ut::Interface::Prologue::Error const& x) {
        std::cerr << "In: " << disasm << ".\n";
        throw x;
      }
      return sink.str();
    }
    return "";
  }
  
  std::string epilogue() const
  {
    if (iif.usemem()) {
      std::ostringstream sink;  
      if (bigendian) sink << "\tsetend\tle\n";
      if (iif.store_addrs.size()) {
        sink << "\tldm\tr4, {r5, r6, r7, r8}\n";
        sink << "\tadd\tr9, r4, #16\n";
        sink << "\tstm\tr9, {r5, r6, r7, r8}\n";
      }
      return sink.str();
    }
    return "";
  }
  
  std::string finally() const { return iif.usemem() ? "\t.ltorg\n" : ""; }
  TestConfig& altmem( bool be, unsigned _offset )
  {
    bigendian = be;
    offset = _offset;
    if (be) ident += "_eb";
    switch (_offset) {
    case 1: ident += "_o1"; break;
    case 2: ident += "_o2"; break;
    case 3: ident += "_o3"; break;
    }
    return *this;
  }
};

struct ARM32 : isa::arm32::Decoder<ut::Arch>
{
  typedef isa::arm32::DecodeTableEntry<ut::Arch> DecodeTableEntry;
  typedef isa::arm32::CodeType CodeType;
  typedef isa::arm32::Operation<ut::Arch> Operation;
  static CodeType mkcode( uint32_t code ) {
    if ((code >> 28) == 15)
      return CodeType( code );
    return CodeType( (0x0fffffff & code) | 0xe0000000 );
  }
  static char const* Name() { return "Arm32"; }
  void
  write_test( Sink& sink, TestConfig const& cfg, CodeType code )
  {
    std::string hexcode;
    { std::ostringstream oss; oss << std::hex << code; hexcode = oss.str(); }
    std::string opfunc_name = sink.test_prefix() + cfg.ident + '_' + hexcode;
    
    sink.macros << "ENTRY(" << opfunc_name << ",0x" << hexcode << ")\n";
    
    sink.sources << "\t.text\n\t.align\t2\n\t.global\t" << opfunc_name
                 << "\n\t.type\t" << opfunc_name
                 << ", %function\n" << opfunc_name
                 << ":\n"
                 << cfg.prologue()
                 << "\t.long\t0x" << hexcode << '\t' << "/* " << cfg.disasm << " */\n"
                 << cfg.epilogue()
                 << "\tbx\tlr\n"
                 << cfg.finally()
                 << "\t.size\t" << opfunc_name
                 << ", .-" << opfunc_name << "\n\n";
  }
  void
  write_tests( Sink& sink, TestConfig const& cfg, CodeType code )
  {
    if ((code >> 28) == 14)
      write_test( sink, cfg, ((code % 14) << 28) | (code & 0x0fffffff) );
    write_test( sink, cfg, code );
  }
};

struct THUMB : isa::thumb::Decoder<ut::Arch>
{
  typedef isa::thumb::DecodeTableEntry<ut::Arch> DecodeTableEntry;
  typedef isa::thumb::CodeType CodeType;
  typedef isa::thumb::Operation<ut::Arch> Operation;
  static CodeType mkcode( uint32_t code ) {
    if (((code >> 11) & 0b11111) < 0b11101) 
      return CodeType( code & 0xffff );
    return CodeType( code );
  }
  static char const* Name() { return "Thumb2"; }
  
  void
  write_test( Sink& sink, std::string const& midfix, TestConfig const& cfg, unsigned cond, bool wide, std::string const& hexcode )
  {
    char const* condname = (cond < 15) ? &"eq\0ne\0cs\0cc\0mi\0pl\0vs\0vc\0hi\0ls\0ge\0lt\0gt\0le\0al"[cond*3] : 0;
    
    std::string opfunc_name( sink.test_prefix() + midfix );
    if (condname) opfunc_name = opfunc_name + '_' + condname;
    
    sink.macros << "ENTRY(" << opfunc_name << ",0x" << hexcode << ")\n";
    
    sink.sources << "\t.text\n\t.align\t2\n\t.global\t" << opfunc_name
                 << "\n\t.type\t" << opfunc_name
                 << ", %function\n\t.thumb\n\t.thumb_func\n" << opfunc_name
                 << ":\n"
                 << cfg.prologue();
    if (condname) sink.sources << "\t.short\t0x" << std::hex << (0b1011111100001000 | (cond << 4)) << std::dec  << "/* it " << condname << " */\n";
    sink.sources << "\t." << (wide ? "long" : "short") << "\t0x" << hexcode << '\t' << "/* " << cfg.disasm << " */\n"
                 << cfg.epilogue()
                 << "\tbx\tlr\n"
                 << cfg.finally()
                 << "\t.size\t" << opfunc_name
                 << ", .-" << opfunc_name << "\n\n";
  }
  
  void
  write_tests( Sink& sink, TestConfig const& cfg, CodeType code )
  {
    int cond = code % 14;
    std::string hexcode;
    { std::ostringstream oss; oss << std::hex << code; hexcode = oss.str(); }
    std::string midfix = cfg.ident + '_' + hexcode;
    bool wide = (cfg.iif.length == 32);
    write_test( sink, midfix, cfg, 15, wide, hexcode );
    if (cfg.iif.itsensitive)
      write_test( sink, midfix, cfg, 14, wide, hexcode );
    write_test( sink, midfix, cfg, cond, wide, hexcode );
  }
};

// template <typename ISA>
// struct CodeUnit
// {
//   typedef typename ISA::CodeType CodeType;
//   typedef typename ISA::Operation Operation;
  
//   CodeType   code;
//   Operation* operation;
  
//   CodeUnit() : operation(0) {}
//   ~CodeUnit() { delete operation; }
// };

template <typename ISA>
struct Checker
{
  typedef typename ISA::CodeType CodeType;
  typedef typename ISA::Operation Operation;
  typedef std::multimap<ut::Interface, CodeType> CodeClass;
  typedef std::map<std::string, CodeClass> TestClasses;
  
  unisim::util::random::Random random;
  ISA isa;
  
  TestClasses testclasses;
  
  Checker() : random( 1, 2, 3, 4 ) {}
  
  void discover( uintptr_t ttl )
  {
    uint64_t step = 0;
    for (auto const& opc : isa.GetDecodeTable())
      {
        uint32_t mask = opc.opcode_mask, bits = opc.opcode & mask;
        auto testclass = testclasses.end();
        
        try {
          for (uintptr_t trial = 0; trial < ttl; ++trial)
            {
              step += 1;
              try {
                CodeType code = ISA::mkcode( (random.Generate() & ~mask) | bits );
                
                std::unique_ptr<Operation> codeop( opc.decode( code, 0 ) );
                
                {
                  std::string name( codeop->GetName() );
                  if (testclass == testclasses.end()) {
                    testclasses[name];
                    testclass = testclasses.find(name);
                    std::cerr << "Tests[" << ISA::Name() << "::" << name << "]: ";
                  } else if (testclass->first != name) {
                    std::cerr << "Incoherent Operation names: " << testclass->first << " and " << name << std::endl;
                    throw 0;
                  }
                }
                
                if (codeop->donttest())
                  throw ut::DontTest( "not under test (explicit donttest)." );
                
                {
                  std::unique_ptr<Operation> realop( isa.NCDecode( 0, code ) );
                  if (strcmp( realop->GetName(), codeop->GetName() ) != 0)
                    continue; /* Not the op we were looking for. */
                }
                
                if (testclass->second.size() >= 256) {
                  std::cerr << "Possible issue: too many tests for" << testclass->first << "...\n";
                  throw 0;
                }
                // We need to perform an abstract execution of the
                // instruction to 1/ further check testability and 2/
                // compute operation interface.
                ut::Arch arch;
                codeop->execute( arch );
                try { arch.interface.finalize( codeop->GetLength() ); }
                catch (ut::Interface::Untestable const& ut) {
                  //std::cerr << "Can't test (#" << step << ")" << this->disasm( code ) << ", " << ut.argument << std::endl;
                  throw isa::Reject();
                }
                // At this point, code corresponds to valid operation
                // to be tested. Nevertheless, if the interface of
                // this operation matches an existing test, we don't
                // add the operation since the new test is unlikely to
                // reveal new bugs.
                if (testclass->second.find( arch.interface ) != testclass->second.end()) continue;
                testclass->second.insert( std::make_pair( arch.interface, code ) );
                trial = 0;
              }
              catch (isa::Reject const& reject) { continue; }
            }
          if (testclass == testclasses.end())
            std::cerr << "Tests[" << ISA::Name() << "::?]: max ttl reached (nothing found).\n";
          else if (testclass->second.size() == 0)
            std::cerr << "max ttl reached (nothing found).\n";
          else
            std::cerr << testclass->second.size() << " patterns found." << std::endl;
        }
        catch (ut::DontTest const& dt) { std::cerr << dt.msg << std::endl; }
      }
  }
  
  void
  write_repos( std::string const& reposname )
  {
    std::ofstream sink( reposname );
    
    typedef typename TestClasses::value_type TCItem;
    typedef typename CodeClass::value_type CCItem;
    
    for (TCItem const& tcitem : testclasses)
      {
        std::string name = tcitem.first;
        
        for (CCItem const& ccitem : tcitem.second)
          {
            sink << name << '\t' << std::hex << uint32_t( ccitem.second ) << std::dec
                 << "\t# " << this->disasm( ccitem.second ) << '\n';
          }
      }
  }

  std::string disasm( CodeType code )
  {
    std::unique_ptr<Operation> operation( isa.NCDecode( 0, code ) );
    std::ostringstream oss;
    static ut::Arch for_disasm_purpose;
    operation->disasm( for_disasm_purpose, oss );
    return oss.str();
  }
  
  struct FileLoc
  {
    std::string name;
    unsigned    line;
    
    FileLoc( std::string const& _name ) : name( _name ) {}
    void newline() { line += 1; }
    std::ostream& dump( std::ostream& sink ) const { sink << name << ':' << line << ": "; return sink; }
  };
  
  void
  read_repos( std::string const& reposname )
  {
    FileLoc fl( reposname );
    std::ifstream source( fl.name );
    
    while (source)
      {
        fl.newline();
        // Parsing incoming repository
        std::string name;
        if (not (source >> name)) break;
        uint32_t rawcode;
        if (not (source >> std::hex >> rawcode)) break;
        std::string extra;
        std::getline( source, extra, '\n' );
        // Decoding operation and checking that given operation name is coherent
        CodeType code = ISA::mkcode( rawcode );
        try { 
          std::unique_ptr<Operation> codeop( isa.NCDecode( 0, code ) );
          if (name != codeop->GetName()) {
            std::cerr << "Operation '" << std::hex << rawcode << std::dec << "' "
                      << "is said to be: '" << name << "' "
                      << "whereas it is: '" << codeop->GetName() << "'\n";
            throw 0;
          }
          if (codeop->donttest()) {
            extra += " ... explicit donttest";
            throw isa::Reject();
          }
          
          // Performing an abstract execution to check the validity of
          // the opcode, and to compute the interface of the operation
          ut::Arch arch;
          codeop->execute( arch );
          try { arch.interface.finalize( codeop->GetLength() ); }
          catch (ut::Interface::Untestable const& ut) {
            std::stringstream err;
            err << " ... can't test " << this->disasm( code ) << ", " << ut.argument;
            extra += err.str();
            throw isa::Reject();
          }
          arch.interface.length = codeop->GetLength();
          
          // Finally recording the operation test
          testclasses[name].insert( std::make_pair( arch.interface, code ) );
        } 
        catch (isa::Reject const& reject) {
          fl.dump( std::cerr ) << "(" << extra << ") rejected.\n";
        }
      }
    
  }
  
  void
  write_tests( Sink& sink )
  {
    typedef typename TestClasses::value_type TCItem;
    typedef typename CodeClass::value_type CCItem;
    
    for (TCItem const& tcitem : testclasses)
      {
        std::string name = tcitem.first;
        
        for (CCItem const& ccitem : tcitem.second)
          {
            TestConfig cfg( ccitem.first, std::string( ISA::Name() ) + '_' + name, disasm( ccitem.second ) );
            if (cfg.iif.usemem()) {
              unsigned offset_end = cfg.iif.aligned ? 1 : 4;
              for (unsigned offset = 0; offset < offset_end; ++offset) {
                isa.write_tests( sink, TestConfig( cfg ).altmem( false, offset ), ccitem.second );
                isa.write_tests( sink, TestConfig( cfg ).altmem( true, offset ), ccitem.second );
              }
            } else {
              isa.write_tests( sink, cfg, ccitem.second );
            }
          }
      }
  }
};

template <typename T>
void Update( std::string const& reposname )
{
  uintptr_t ttl = 10000;
  if (char const* ttl_cfg = getenv("TTL_CFG")) { ttl = strtoull(ttl_cfg,0,0); }
  
  std::string suffix(".tests");
  
  if ((suffix.size() >= reposname.size()) or not std::equal(suffix.rbegin(), suffix.rend(), reposname.rbegin()))
    {
      std::cerr << "Bad test repository name (should ends with " << suffix << ").\n";
      throw 0;
    }
  
  std::string basename( reposname.substr( 0, reposname.size() - suffix.size() ) );
  
  Sink sink( basename );
      
  Checker<T> checker;
  checker.read_repos( reposname );
  checker.discover( ttl );
  checker.write_repos( reposname );
  checker.write_tests( sink );
}

int
main( int argc, char** argv )
{
  try
    {
      if (argc != 3) {
        std::cerr << "Wrong number of argument.\n";
        throw 0;
      }
      
      if      (strcmp("arm32",argv[1]) == 0)
        {
          Update<ARM32>( argv[2] );
        }
      else if (strcmp("thumb",argv[1]) == 0)
        {
          Update<THUMB>( argv[2] );
        }
      else
        throw 0;
    }
  catch (...)
    {
      std::cerr << argv[0] << " [arm32|thumb] <path_to_test_file>\n";
      return 1;
    }
  
  return 0;
}
