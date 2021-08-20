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

#include "utilities/version.h"

namespace sc_core {

const unsigned int sc_version_major = SC_VERSION_MAJOR;
const unsigned int sc_version_minor = SC_VERSION_MINOR;
const unsigned int sc_version_patch = SC_VERSION_PATCH;
const std::string sc_version_originator = std::string(SC_VERSION_ORIGINATOR);
const std::string sc_version_release_date = std::string(SC_VERSION_RELEASE_DATE);
const std::string sc_version_prerelease = std::string(SC_VERSION_PRERELEASE);
const bool sc_is_prerelease = SC_IS_PRERELEASE;
const std::string sc_version_string = std::string(SC_VERSION);
const std::string sc_copyright_string = std::string(SC_COPYRIGHT);

const char* sc_copyright()
{
	return sc_copyright_string.c_str();
}

const char* sc_version()
{
	return "2.3.0"; // assume it is to check compatibility against proof of concept releases from Accelera
}

const char* sc_release()
{
	return sc_version_string.c_str();
}

} // end of namespace sc_core
