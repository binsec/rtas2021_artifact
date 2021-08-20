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
 
#ifndef __UNISIM_UTIL_DBGATE_CLIENT_CLIENT_HH__
#define __UNISIM_UTIL_DBGATE_CLIENT_CLIENT_HH__

#include <ostream>
#include <streambuf>
#include <cstdio>
#include <cstring>
#include <cstdint>

/***********************/
/*** DBGate bindings ***/
/***********************/

extern "C"
{
  void dbgate_write(int cd, char const* buffer, uintptr_t size);
  int dbgate_open(char const* path);
  void dbgate_close(int cd);
}

#define DBGATE_DEFS                                                     \
  void dbgate_write(int cd, char const* buffer, uintptr_t size) { /* dont't write */ } \
  int  dbgate_open(char const* path)                            { return -1; /* can't open */ } \
  void dbgate_close(int cd)                                     { /* dont't close */ } \
  void semicolon_expected()

namespace unisim {
namespace util {
namespace dbgate {
namespace client {

  struct odbgbuf : public std::streambuf
  {
    odbgbuf (int _cd) : cd(_cd) {}
    virtual int_type overflow (int_type c) override { if (c != EOF) { char z = c; dbgate_write(cd, &z, 1); } return c; }
    virtual std::streamsize xsputn (const char* s, std::streamsize num) override { return dbgate_write(cd, s, num), num; }
    int cd;
  };

  struct odbgstream : public std::ostream
  {
    odbgstream (int cd) : std::ostream(0), buf(cd) { rdbuf(&buf); if (cd < 0) this->setstate(ios_base::failbit);  }
    odbgbuf  buf;
    int cd() const { return buf.cd; }
  };

} // end of namespace client
} // end of namespace dbgate
} // end of namespace util
} // end of namespace unisim
  
#endif //__UNISIM_UTIL_DBGATE_CLIENT_CLIENT_HH__

