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

#ifndef __LIBIEEE1666_CORE_FWD_H__
#define __LIBIEEE1666_CORE_FWD_H__

#include <data_types/native.h>
#include "core/features.h"

#if defined(ALWAYS_INLINE)
#undef ALWAYS_INLINE
#endif

#if defined(__GNUC__)
#if (__GNUC__ > 4) || ((__GNUC__ >= 4) && ((__GNUC_MINOR__ > 1) || ((__GNUC_MINOR__ >= 1) && (__GNUC_PATCHLEVEL__ >= 3))))     // GNU C version >= 4.1.3
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#elif defined(__clang__)

#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#endif // __clang__

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#if defined(likely)
#undef likely
#endif

#if defined(unlikely)
#undef unlikely
#endif

#define likely(x)       __builtin_expect(!!(x),1)
#define unlikely(x)     __builtin_expect(!!(x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

namespace sc_core {

enum sc_port_policy
{
	SC_ONE_OR_MORE_BOUND,         // Default
	SC_ZERO_OR_MORE_BOUND,
	SC_ALL_BOUND
};

enum sc_curr_proc_kind
{
	SC_NO_PROC_ ,
	SC_METHOD_PROC_ ,
	SC_THREAD_PROC_ ,
	SC_CTHREAD_PROC_
};

enum sc_descendant_inclusion_info
{
	SC_NO_DESCENDANTS,
	SC_INCLUDE_DESCENDANTS
};

enum sc_time_unit { SC_FS = 0, SC_PS, SC_NS, SC_US, SC_MS, SC_SEC };

enum sc_status
{
	SC_ELABORATION = 0x01,
	SC_BEFORE_END_OF_ELABORATION = 0x02,
	SC_END_OF_ELABORATION = 0x04,
	SC_START_OF_SIMULATION = 0x08,
	SC_RUNNING = 0x10,
	SC_PAUSED = 0x20,
	SC_STOPPED = 0x40,
	SC_END_OF_SIMULATION = 0x80
};

enum sc_stop_mode
{
	SC_STOP_FINISH_DELTA,
	SC_STOP_IMMEDIATE
};

enum sc_starvation_policy
{
	SC_RUN_TO_TIME,
	SC_EXIT_ON_STARVATION
};

enum sc_unwind_exception_type
{
	SC_UNWIND_EXCEPTION_RESET,
	SC_UNWIND_EXCEPTION_KILL
};

class sc_attr_base;
template <class T> class sc_attribute;
class sc_attr_cltn;
class sc_clock;
class sc_event;
class sc_event_and_list;
class sc_event_or_list;
class sc_event_and_expr;
class sc_event_or_expr;
class sc_event_finder;
class sc_interface;
class sc_kernel;
class sc_bind_proxy;
class sc_module;
class sc_module_name;
class sc_object;
class sc_port_base;
class sc_export_base;
template <class IF> class sc_port_b;
template <class IF> class sc_export;
template <class IF, int N, sc_port_policy P> class sc_port;
class sc_prim_channel;
class sc_process_handle;
class sc_sensitive;
class sc_time;
class sc_spawn_options;

/////////////// implementation defined ////////////////
class sc_process_owner;
class sc_process;
class sc_thread_process;
class sc_method_process;
class sc_thread_process_helper;
class sc_kernel_event;
class sc_timed_kernel_event;
class sc_event_list;

typedef void (sc_process_owner::*sc_process_owner_method_ptr)();

#define __LIBIEEE1666_KERNEL_PREFIX__ "ieee1666_kernel"

} // end of namespace sc_core

#endif
