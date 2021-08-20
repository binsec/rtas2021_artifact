/*
 *  Copyright (c) 2018,
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
 * Author: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_KERNEL_TLM2_TRANS_ATTR_HH__
#define __UNISIM_KERNEL_TLM2_TRANS_ATTR_HH__

#include <systemc>
#include <tlm>

namespace unisim {
namespace kernel {
namespace tlm2 {

class tlm_trans_attr : public tlm::tlm_extension<tlm_trans_attr>
{
public:
	tlm_trans_attr()
		: m_master_id(0)
		, m_privileged(false)
		, m_instruction(false)
	{
	}

	void set_master_id(unsigned int v) { m_master_id = v; }
	void set_privileged(bool v = true) { m_privileged = v; }
	void set_instruction(bool v = true) { m_instruction = v; }
	
	int master_id() const { return m_master_id; }
	bool is_privileged() const { return m_privileged; }
	bool is_instruction() const { return m_instruction; }
	
	virtual tlm::tlm_extension_base *clone() const
	{
		tlm_trans_attr *clone = new tlm_trans_attr();
		clone->copy(*this);
		return clone;
	}
	
	virtual void copy_from(const tlm::tlm_extension_base& ext)
	{
		copy(static_cast<const tlm_trans_attr &>(ext));
	}
private:
	int m_master_id;
	bool m_privileged;
	bool m_instruction;
	
	void copy(const tlm_trans_attr& trans_attr)
	{
		m_master_id = trans_attr.m_master_id;
		m_privileged = trans_attr.m_privileged;
		m_instruction = trans_attr.m_instruction;
	}
};

} // end of namespace tlm2
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_TLM2_TRANS_ATTR_HH__
