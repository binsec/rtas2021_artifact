/*
 *  Copyright (c) 2014,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __LIBIEEE1666_UTILITIES_VERSION_H__
#define __LIBIEEE1666_UTILITIES_VERSION_H__

#include <string>
#include "core/features.h"

namespace sc_core {

#define IEEE_1666_SYSTEMC 201101L
#define SC_VERSION_MAJOR __LIBIEEE1666_PACKAGE_VERSION_MAJOR__  // implementation-defined_number
#define SC_VERSION_MINOR __LIBIEEE1666_PACKAGE_VERSION_MINOR__  // implementation-defined_number
#define SC_VERSION_PATCH __LIBIEEE1666_PACKAGE_VERSION_PATCH__  // implementation-defined_number
#define SC_VERSION_ORIGINATOR "CEA"             // implementation-defined_string
#define SC_VERSION_RELEASE_DATE "20160317"      // implementation-defined_date
#define SC_VERSION_PRERELEASE "alpha"           // implementation-defined_string
#define SC_IS_PRERELEASE 1                      // implementation-defined_bool
#define SC_VERSION __LIBIEEE1666_PACKAGE_VERSION__ SC_VERSION_PRERELEASE "-" SC_VERSION_ORIGINATOR        // implementation-defined_string
#define SC_COPYRIGHT "Copyright (c) 2014-2016,\nCommissariat a l'Energie Atomique (CEA)\nAll rights reserved\n"   // implementation-defined_string
	
extern const unsigned int sc_version_major;
extern const unsigned int sc_version_minor;
extern const unsigned int sc_version_patch;
extern const std::string sc_version_originator;
extern const std::string sc_version_release_date;
extern const std::string sc_version_prerelease;
extern const bool sc_is_prerelease;
extern const std::string sc_version_string;
extern const std::string sc_copyright_string;

const char* sc_copyright();
const char* sc_version();
const char* sc_release();

} // end of namespace sc_core

#endif
