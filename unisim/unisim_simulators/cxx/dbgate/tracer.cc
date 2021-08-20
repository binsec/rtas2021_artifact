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

#include <unisim/util/dbgate/dbgate.hh>
#include <tracee.hh>
#include <iostream>
#include <cassert>

int
main(int argc, char** argv)
{
  int optidx = 1;

  assert(not argv[argc]);

  if (optidx >= argc)
    {
      std::cerr << "Usage: " << argv[0] << " <program> <program args...>\n";
      return 1;
    }

  Tracee tracee(argv+optidx);

  if (not tracee.good())
    {
      std::cerr << "No tracee, giving up.\n";
      return 1;
    }

  unisim::util::dbgate::DBGated server(12345, 0);
  std::cerr << "DBGate server started in " << server.root << " at localhost:" << server.port << "\n";

  for (DBGateMethodID method; (method = tracee.nextcall()).good();)
    {
      //      std::cerr << "In '" << method.c_str() << "'@" << tracee.printinsnaddr() << std::endl;
  
      switch (method.code)
        {
        case DBGateMethodID::open:
          {
            // int dbgate_open(char const* chan);
            std::string chan = tracee.getstring(0);
            int res = server.open(chan.c_str());
            tracee.returnint(res);
          } break;
          
        case DBGateMethodID::close:
          {
            // void dbgate_close(int cd);
            int cd = tracee.getint(0);
            server.close(cd);
            tracee.doreturn();
          } break;
          
        case DBGateMethodID::write:
          {
            // void dbgate_write(int cd, char const* buffer, uintptr_t size);
            int cd = tracee.getint(0);
            std::string buffer = tracee.getbuffer(1,2);
            server.write(cd,buffer.c_str(), buffer.size());
            tracee.doreturn();
          } break;
          
        default:
          throw 0;
        }
    }

  if (server.ostreams.size())
    {
      std::cerr << "Warning: pending debug streams:\n";
      for (auto const& os : server.ostreams)
        {
          std::cerr << os.second.chanpath << "; " << os.second.filepath << "\n";
        }
    }
  // std::cerr << "Received:\n";
  // for (auto const& p : server.files)
  //   {
  //     std::cerr << "  \"" << p.first << "\": \"" << p.second << "\"\n";
  //   }
  std::cerr << "DBGate server stopped" << std::endl;
  
  return 0;
}

