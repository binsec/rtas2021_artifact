/*
 *  Copyright (c) 2015,
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

#ifndef __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_INTERFACE_DIRECT_MEMORY_H__
#define __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_INTERFACE_DIRECT_MEMORY_H__

#include "tlm2/fwd.h"
#include <systemc>

namespace tlm {

class tlm_dmi
{
public:
	tlm_dmi() { init(); }
	void init();
	enum dmi_access_e
	{
		DMI_ACCESS_NONE= 0x00,
		DMI_ACCESS_READ = 0x01,
		DMI_ACCESS_WRITE= 0x02,
		DMI_ACCESS_READ_WRITE = DMI_ACCESS_READ | DMI_ACCESS_WRITE
	};

	unsigned char *get_dmi_ptr() const;
	sc_dt::uint64 get_start_address() const;
	sc_dt::uint64 get_end_address() const;
	sc_core::sc_time get_read_latency() const;
	sc_core::sc_time get_write_latency() const;
	dmi_access_e get_granted_access() const;
	bool is_none_allowed() const;
	bool is_read_allowed() const;
	bool is_write_allowed() const;
	bool is_read_write_allowed() const;
	void set_dmi_ptr(unsigned char* p);
	void set_start_address(sc_dt::uint64 addr);
	void set_end_address(sc_dt::uint64 addr);
	void set_read_latency(sc_core::sc_time t);
	void set_write_latency(sc_core::sc_time t);
	void set_granted_access(dmi_access_e t);
	void allow_none();
	void allow_read();
	void allow_write();
	void allow_read_write();
private:
	unsigned char *dmi_ptr;
	sc_dt::uint64 start_address;
	sc_dt::uint64 end_address;
	dmi_access_e granted_access;
	sc_core::sc_time read_latency;
	sc_core::sc_time write_latency;
};

template <typename TRANS = tlm_generic_payload>
class tlm_fw_direct_mem_if : public virtual sc_core::sc_interface
{
public:
	virtual bool get_direct_mem_ptr(TRANS& trans, tlm_dmi& dmi_data) = 0;
};

class tlm_bw_direct_mem_if : public virtual sc_core::sc_interface
{
public:
	virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) = 0;
};

} // end of namespace tlm

#endif // __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_INTERFACE_DIRECT_MEMORY_H__
