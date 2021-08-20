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

#ifndef __LIBIEEE1666_TLM1_ANALYSIS_ANALYSIS_PORT_H__
#define __LIBIEEE1666_TLM1_ANALYSIS_ANALYSIS_PORT_H__

#include <tlm1/fwd.h>
#include <systemc>

namespace tlm {

//////////////////////////////// declaration //////////////////////////////////

// Analysis port
template <typename T>
class tlm_analysis_port
	: public sc_core::sc_object
	, public virtual tlm_analysis_if<T>
{
public:
	tlm_analysis_port();
	tlm_analysis_port(const char *);
	
	// bind and () work for both interfaces and analysis ports,
	// since analysis ports implement the analysis interface
	virtual void bind(tlm_analysis_if<T>&);
	void operator () (tlm_analysis_if<T>&);
	virtual bool unbind(tlm_analysis_if<T> &);
	void write(const T&);
};

///////////////////////////////// definition //////////////////////////////////

template <typename T>
tlm_analysis_port<T>::tlm_analysis_port()
{
}

template <typename T>
tlm_analysis_port<T>::tlm_analysis_port(const char *)
{
}

template <typename T>
void tlm_analysis_port<T>::bind(tlm_analysis_if<T>&)
{
}

template <typename T>
void tlm_analysis_port<T>::operator () (tlm_analysis_if<T>&)
{
}

template <typename T>
bool tlm_analysis_port<T>::unbind(tlm_analysis_if<T> &)
{
}

template <typename T>
void tlm_analysis_port<T>::write(const T&)
{
}

} // end of namespace tlm

#endif
