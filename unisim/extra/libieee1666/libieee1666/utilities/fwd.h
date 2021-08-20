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

#ifndef __LIBIEEE1666_UTILITIES_FWD_H__
#define __LIBIEEE1666_UTILITIES_FWD_H__

namespace sc_core {

enum sc_severity {
	SC_INFO = 0 ,
	SC_WARNING ,
	SC_ERROR ,
	SC_FATAL ,
	SC_MAX_SEVERITY
};

enum sc_verbosity {
	SC_NONE = 0,
	SC_LOW = 100,
	SC_MEDIUM = 200,
	SC_HIGH = 300,
	SC_FULL = 400,
	SC_DEBUG = 500
};

enum {
	SC_UNSPECIFIED   = 0x0000 ,
	SC_DO_NOTHING    = 0x0001 ,
	SC_THROW         = 0x0002 ,
	SC_LOG           = 0x0004 ,
	SC_DISPLAY       = 0x0008 ,
	SC_CACHE_REPORT  = 0x0010 ,
	SC_INTERRUPT     = 0x0020 ,
	SC_STOP          = 0x0040 ,
	SC_ABORT         = 0x0080
};

class sc_trace_file;
class sc_report;
class sc_report_handler;
class sc_vector_base;
template <typename T> class sc_vector_iter;
template <typename T> class sc_vector;
template <typename T, typename MT> class sc_vector_assembly;

} // end of namespace sc_core

#endif
