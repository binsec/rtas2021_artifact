/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
#include <limits>
#include <iostream>

namespace unisim {
namespace util {
namespace dbgate {
  
  DBGated::DBGated(int _port, char const* _root)
    : port(_port)
    , root()
  {
    if (_root)
      root.assign(_root);
    else
      { char tmpdirbuf[] = "/tmp/dbgateXXXXXX"; root = mkdtemp( tmpdirbuf ); }
    if (not _port)
      port = 12345;
  }
  
  void
  DBGated::write(int cd, char const* buffer, uintptr_t size)
  {
    auto itr = ostreams.find(cd);
    if (itr != ostreams.end())
      itr->second.stream.write(buffer,size);
  }
  
  int
  DBGated::open(char const* path)
  {
    std::string filepath = root + "/dbg";
    {
      /* generating unique name with file count */
      struct { void nibble(std::string& s, uintptr_t i) { if (i>=16) nibble(s,i>>4); s += "0123456789abcdef"[i%16]; } } _;
      _.nibble( filepath, files.size() + ostreams.size() );
      /* extracting any existing extension */
      std::string buffer = path;
      uintptr_t spos = buffer.rfind('/'), dpos = buffer.rfind('.');
      if (dpos != std::string::npos and (spos == std::string::npos or spos < dpos))
        filepath.insert(filepath.end(),buffer.begin()+dpos,buffer.end());
    }
    
    int rcd = 0;
    auto itr = ostreams.end();
    
    if (ostreams.size())
      {
        rcd = std::numeric_limits<int>::max();
        for (auto end = itr; --itr != end;)
          {
            if (itr->first < rcd) { rcd = itr->first+1; break; }
            else                  { rcd = itr->first-1; continue; }
          }
      }
    
    if (rcd < 0)
      {
        std::cerr << "Descriptor allocation error\n";
        return rcd;
      }
    
    itr = ostreams.emplace_hint( itr, std::piecewise_construct, std::forward_as_tuple( rcd ), std::forward_as_tuple( path, std::move(filepath) ) );
    
    if (not itr->second.stream.good())
      {
        std::cerr << "Can't create Sink(" << itr->second.chanpath << ", " << itr->second.filepath << ")\n";
        ostreams.erase(itr);
        return -1;
      }

    return rcd;
  }

  void
  DBGated::close(int cd)
  {
    auto itr = ostreams.find(cd);
    if (itr == ostreams.end())
      return;
    Sink& sink(itr->second);
    sink.stream.close();
    files.emplace( std::piecewise_construct, std::forward_as_tuple( std::move(sink.chanpath) ), std::forward_as_tuple( std::move(sink.filepath)) );
    ostreams.erase(itr);
  }

  DBGated::Sink::Sink(std::string&& _chanpath, std::string&& _filepath)
    : chanpath(std::move(_chanpath)), filepath(std::move(_filepath)), stream(filepath.c_str())
  {}
  
} /* end of namespace dbgate */
} /* end of namespace util */
} /* end of namespace unisim */

