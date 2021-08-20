/*
 *  Copyright (c) 2011
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_OS_LINUX_AUX_TABLE_HH__
#define __UNISIM_UTIL_OS_LINUX_AUX_TABLE_HH__

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

static const unsigned int AT_NULL = 0;
static const unsigned int AT_IGNORE = 1;     /* entry should be ignored */
static const unsigned int AT_EXECFD = 2;     /* file descriptor of program */
static const unsigned int AT_PHDR = 3;     /* program headers for program */
static const unsigned int AT_PHENT = 4;     /* size of program header entry */
static const unsigned int AT_PHNUM = 5;     /* number of program headers */
static const unsigned int AT_PAGESZ = 6;     /* system page size */
static const unsigned int AT_BASE = 7;     /* base address of interpreter */
static const unsigned int AT_FLAGS = 8;     /* flags */
static const unsigned int AT_ENTRY = 9;     /* entry point of program */
static const unsigned int AT_NOTELF = 10;    /* program is not ELF */
static const unsigned int AT_UID = 11;    /* real uid */
static const unsigned int AT_EUID = 12;    /* effective uid */
static const unsigned int AT_GID = 13;    /* real gid */
static const unsigned int AT_EGID = 14;    /* effective gid */
static const unsigned int AT_PLATFORM = 15;  /* string identifying CPU for optimizations */
static const unsigned int AT_HWCAP = 16;    /* arch dependent hints at CPU capabilities */
static const unsigned int AT_CLKTCK = 17;    /* frequency at which times() increments */

static const unsigned int AT_IGNOREPPC = 22; /* Entry should be ignored.  */
static const unsigned int AT_SECURE = 23; /* Boolean, was exec setuid-like?  */
static const unsigned int AT_EXECFN = 31; /* Filename of executable.  */

} // end of namespace
} // end of namespace
} // end of namespace
} // end of namespace

#endif // __UNISIM_UTIL_OS_LINUX_AUX_TABLE_HH__

