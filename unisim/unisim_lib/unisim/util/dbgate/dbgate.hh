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
 
#ifndef __UNISIM_UTIL_DBGATE_DBGATE_HH__
#define __UNISIM_UTIL_DBGATE_DBGATE_HH__

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace dbgate {

  struct DBGated /* : public hypapp::HttpServer */
  {
    DBGated(int port, char const* string);

    /* Communication with guest program. */
    void write(int cd, char const* buffer, uintptr_t size);
    int open(char const* path);
    void close(int cd);

    /* Communication with debugging webclients */

    // /*** hypapp::HttpServer deployment ***/
    // virtual void Serve(hypapp::ClientConnection const& conn) override;

    /* Compound holding current debugging stream */
    struct Sink
    {
      std::string chanpath, filepath;
      std::ofstream stream;
      Sink(std::string&& _chanpath, std::string&& _filepath);
    };
    
    int port;                                      /*< port of the webserver */
    std::string root;                              /*< location of debugging files */ 
    std::map<int,Sink> ostreams;                   /*< active debugging streams */
    std::multimap<std::string,std::string> files;  /*< debugging file store */
  };

} /* end of namespace dbgate */
} /* end of namespace util */
} /* end of namespace unisim */

#endif /* __UNISIM_UTIL_DBGATE_DBGATE_HH__ */
