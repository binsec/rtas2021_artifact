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

#include "utilities/report_handler.h"

namespace sc_core {

void sc_report_handler::report( sc_severity , const char* msg_type , const char* msg , const char* file , int line )
{
}

void sc_report_handler::report( sc_severity , const char* msg_type , const char* msg , int verbosity,
																		const char* file , int line )
{
}

sc_actions sc_report_handler::set_actions( sc_severity , sc_actions )
{
}

sc_actions sc_report_handler::set_actions( const char * msg_type , sc_actions )
{
}

sc_actions sc_report_handler::set_actions( const char * msg_type , sc_severity , sc_actions )
{
}

int sc_report_handler::stop_after( sc_severity , int limit )
{
}
int sc_report_handler::stop_after( const char* msg_type , int limit )
{
}

int sc_report_handler::stop_after( const char* msg_type , sc_severity , int limit )
{
}

int sc_report_handler::get_count( sc_severity )
{
}

int sc_report_handler::get_count( const char* msg_type )
{
}

int sc_report_handler::get_count( const char* msg_type , sc_severity )
{
}

int sc_report_handler::set_verbosity_level()
{
}

int sc_report_handler::get_verbosity_level()
{
}

sc_actions sc_report_handler::suppress( sc_actions )
{
}

sc_actions sc_report_handler::suppress()
{
}

sc_actions sc_report_handler::force( sc_actions )
{
}

sc_actions sc_report_handler::force()
{
}

void sc_report_handler::set_handler( sc_report_handler_proc )
{
}

void sc_report_handler::default_handler( const sc_report& , const sc_actions& )
{
}

sc_actions sc_report_handler::get_new_action_id()
{
}

sc_report* sc_report_handler::get_cached_report()
{
}

void sc_report_handler::clear_cached_report()
{
}

bool sc_report_handler::set_log_file_name( const char* )
{
}

const char* sc_report_handler::get_log_file_name()
{
}

void sc_interrupt_here( const char* msg_type , sc_severity )
{
}

void sc_stop_here( const char* msg_type , sc_severity )
{
}

} // end of namespace sc_core
