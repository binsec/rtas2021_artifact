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

#ifndef __LIBIEEE1666_UTILITIES_REPORT_HANDLER_H__
#define __LIBIEEE1666_UTILITIES_REPORT_HANDLER_H__

#include "core/fwd.h"
#include "utilities/fwd.h"
#include <stdexcept>

namespace sc_core {

typedef unsigned sc_actions;

#define SC_DEFAULT_INFO_ACTIONS \
              ( SC_LOG | SC_DISPLAY )

#define SC_DEFAULT_WARNING_ACTIONS \
              ( SC_LOG | SC_DISPLAY )

#define SC_DEFAULT_ERROR_ACTIONS \
              ( SC_LOG | SC_CACHE_REPORT | SC_THROW )

#define SC_DEFAULT_FATAL_ACTIONS \
              ( SC_LOG | SC_DISPLAY | SC_CACHE_REPORT | SC_ABORT )

typedef void ( * sc_report_handler_proc ) ( const sc_report& , const sc_actions& );

class sc_report_handler
{
public:
	static void report( sc_severity , const char* msg_type , const char* msg , const char* file , int line );
	static void report( sc_severity , const char* msg_type , const char* msg , int verbosity,
	                                                                       const char* file , int line );

	static sc_actions set_actions( sc_severity , sc_actions = SC_UNSPECIFIED );
	static sc_actions set_actions( const char * msg_type , sc_actions = SC_UNSPECIFIED );
	static sc_actions set_actions( const char * msg_type , sc_severity , sc_actions = SC_UNSPECIFIED );
	
	static int stop_after( sc_severity , int limit = -1 );
	static int stop_after( const char* msg_type , int limit = -1 );
	static int stop_after( const char* msg_type , sc_severity , int limit = -1 );

	static int get_count( sc_severity );
	static int get_count( const char* msg_type );
	static int get_count( const char* msg_type , sc_severity );

	int set_verbosity_level();
	int get_verbosity_level();

	static sc_actions suppress( sc_actions );
	static sc_actions suppress();
	static sc_actions force( sc_actions );
	static sc_actions force();

	static void set_handler( sc_report_handler_proc );
	static void default_handler( const sc_report& , const sc_actions& );
	static sc_actions get_new_action_id();

	static sc_report* get_cached_report();
	static void clear_cached_report();

	static bool set_log_file_name( const char* );
	static const char* get_log_file_name();
};

#define SC_REPORT_INFO_VERB( msg_type , msg, verbosity ) \
            sc_report_handler::report( SC_INFO , msg_type , msg , verbosity, __FILE__ , __LINE__ )

#define SC_REPORT_INFO( msg_type , msg ) \
            sc_report_handler::report( SC_INFO , msg_type , msg , __FILE__ , __LINE__ )

#define SC_REPORT_WARNING( msg_type , msg ) \
            sc_report_handler::report( SC_WARNING , msg_type , msg , __FILE__ , __LINE__ )

#define SC_REPORT_ERROR( msg_type , msg ) \
            sc_report_handler::report( SC_ERROR , msg_type , msg , __FILE__ , __LINE__ )

#define SC_REPORT_FATAL( msg_type , msg ) \
            sc_report_handler::report( SC_FATAL , msg_type , msg , __FILE__ , __LINE__ )

#define sc_assert( expr ) \
              ( ( void ) ( ( expr ) ? 0 : ( SC_REPORT_FATAL( implementation-defined , #expr ) , 0 ) ) )

void sc_interrupt_here( const char* msg_type , sc_severity );
void sc_stop_here( const char* msg_type , sc_severity );

} // end of namespace sc_core

#endif
