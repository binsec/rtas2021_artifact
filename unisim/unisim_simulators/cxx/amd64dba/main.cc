/*
 *  Copyright (c) 2009-2020,
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

#include <arch.hh>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>

#include <cstdint>

struct InstructionAddress : public unisim::util::symbolic::binsec::ASExprNode
{
  InstructionAddress() {}
  virtual void Repr( std::ostream& sink ) const { sink << "insn_addr"; }
  virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<InstructionAddress const&>( rhs ) ); }
  int compare( InstructionAddress const& rhs ) const { return 0; }
};

struct Translator
{
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint64_t _addr, std::vector<uint8_t>&& _code )
    : code(std::move(_code)), addr(_addr), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  void
  extract( std::ostream& sink )
  {
    typedef unisim::component::cxx::processor::intel::Operation<Processor> Operation;
    
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(8, addr) << ")\n";
  
    // Instruction decoding
    struct Instruction
    {
      Instruction(uint64_t address, uint8_t* bytes) : operation(0)
      {
        operation = Processor::Decode(address, bytes);
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }
      Operation& operator * () { return *operation; }
      Operation* operation;
    };
    
    Instruction instruction( addr, &code[0] );

    if (instruction->length > code.size()) { struct LengthError {}; throw LengthError(); }
    code.resize(instruction->length);

    sink << "(opcode . \"";
    char const* sep = "";
    for (auto byte : code) { sink << sep; sep = " "; for (int n = 0; n < 2; n++, byte <<=4) sink << "0123456789abcdef"[byte>>4&15]; }
    sink << "\")\n";
    
    Processor::addr_t insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    uint64_t nia = addr + instruction->length;
    // Processor::U32      insn_addr = Expr(new InstructionAddress()); //< symbolic instruction address
    Processor reference;
    
    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";
    
    // Get actions
    for (bool end = false; not end;)
      {
        Processor state;
        state.path = coderoot;
        state.step(*instruction);
        end = state.close( reference, nia );
      }
    coderoot->simplify();
    coderoot->commit_stats();
  }

  void translate( std::ostream& sink )
  {
    try
      {
        extract( sink );
      }
    catch (Processor::Undefined const&)
      {
        sink << "(undefined)\n";
        return;
      }
    catch (...)
      {
        sink << "(unimplemented)\n";
        return;
      }

    // Translate to DBA
    unisim::util::symbolic::binsec::Program program;
    program.Generate( coderoot );
    typedef unisim::util::symbolic::binsec::Program::const_iterator Iterator;
    for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
      sink << "(" << unisim::util::symbolic::binsec::dbx(8, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }
  std::vector<uint8_t> code;
  uint64_t addr;
  ActionNode* coderoot;
};
  
bool getu64( uint64_t& res, char const* arg )
{
  char *end;
  uint64_t tmp = strtoull( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

bool getbytes( std::vector<uint8_t>& res, char const* arg )
{
  for (char const* end = 0; *arg; arg = end)
    {
      unsigned l = strtoul(arg,(char**)&end,16);
      if (end == arg) break;
      res.push_back(l);
    }
  return res.size();
}

char const* usage()
{
  return
    "usage: <program> arm|intel <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }

  uint64_t addr;
  std::vector<uint8_t> code;

  if (not getu64(addr, argv[1]) or not getbytes(code, argv[2]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Translator actset( addr, std::move(code) );
  actset.translate( std::cout );
  
  return 0;
}

