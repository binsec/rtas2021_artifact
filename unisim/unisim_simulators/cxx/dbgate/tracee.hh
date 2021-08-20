/*
 *  Copyright (c) 2019,
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

/* UNISIM */
#include <unisim/util/identifier/identifier.hh>
/* C++ */
#include <iosfwd>
#include <string>
/* C */
#include <cstdint>
/* POSIX */
#include <sys/types.h>
#include <sys/ptrace.h>

struct DBGateMethodID  : public unisim::util::identifier::Identifier<DBGateMethodID>
{
  enum Code { open = 0, close, write, end } code;

  char const* c_str() const
  {
    switch (code)
      {
      case open:  return "dbgate_open";
      case close: return "dbgate_close";
      case write: return "dbgate_write";
      case end: break;
      }
    return "NA";
  }

  bool good() const { return code != end; }

  DBGateMethodID() : code(end) {}
  DBGateMethodID( Code _code ) : code(_code) {}
  DBGateMethodID( char const* _code ) : code(end) { init(_code); }
};

struct Tracee
{
  struct Driver
  {
    virtual ~Driver() {};
    virtual void Decode(Tracee& tracee, char const* execpath) = 0;
    virtual void PrintInsnAddr( Tracee const& tracee, std::ostream& sink ) const = 0;
    virtual void InsertBreakPoint( Tracee const& tracee, DBGateMethodID method ) = 0;
    virtual std::string GetString( Tracee const& tracee, int adpos ) const = 0;
    virtual std::string GetBuffer( Tracee const& tracee, int adpos, int szpos ) const = 0;
    virtual int GetInt( Tracee const& tracee, int pos ) const = 0;
    virtual bool AtBreakPoint( Tracee const& tracee, DBGateMethodID method ) const = 0;
    virtual void ReturnInt( Tracee const& tracee, int res ) const = 0;
    virtual void Return( Tracee const& tracee ) const = 0;
  };

  Tracee( char** argv );
  
  ~Tracee() { delete driver; }

  bool good() const { return pid >= 0; }

  DBGateMethodID nextcall() const;

  std::string getstring( int adpos ) const { return driver->GetString(*this, adpos); }
  std::string getbuffer( int adpos, int szpos ) const { return driver->GetBuffer(*this, adpos, szpos); };
  int getint( int pos ) const { return driver->GetInt(*this, pos); };
  void returnint( int res ) const { driver->ReturnInt(*this, res); }
  void doreturn() const { driver->Return(*this); }

  uintptr_t ptrace(enum __ptrace_request request, uintptr_t addr, uintptr_t data) const;

  struct PPC
  {
    PPC(Tracee const& _t) : t(_t) {} Tracee const& t;
    friend std::ostream& operator << (std::ostream& sink, PPC const& ppc )
    { return ppc.t.printinsnaddr( sink ); return sink; }
  };
  PPC printinsnaddr() { return PPC(*this); }
  
  std::ostream& printinsnaddr( std::ostream& sink ) const;
  
private:
  pid_t pid;
  Driver* driver;
};
