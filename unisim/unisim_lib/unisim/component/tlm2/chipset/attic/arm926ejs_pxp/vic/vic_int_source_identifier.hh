/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_INT_SOURCE_IDENTIFIER_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_INT_SOURCE_IDENTIFIER_HH__

#include <systemc>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace vic {

class VICIntSourceIdentifierInterface
{
public:
	virtual ~VICIntSourceIdentifierInterface() {};
	virtual void VICIntSourceReceived(int id, bool value) = 0;
};

class VICIntSourceIdentifier
	: public sc_module
{
public:
	sc_core::sc_in<bool> vicinttarget;

	SC_HAS_PROCESS(VICIntSourceIdentifier);
	VICIntSourceIdentifier(const sc_module_name &name, 
			int _id,
			VICIntSourceIdentifierInterface *_master)
		: sc_module(name)
		, vicinttarget("vicinttarget")
		, id(_id)
		, master(_master)
	{
		SC_METHOD(Identify);
		sensitive << vicinttarget;
	}

	~VICIntSourceIdentifier()
	{
		master = 0;
	}

private:
	int id;
	VICIntSourceIdentifierInterface *master;
	void Identify() 
	{
		master->VICIntSourceReceived(id, vicinttarget);
	}
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm2
} // end of namespace chipset
} // end of namespace arm926ejs_pxp
} // end of namespace vic

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_VIC_VIC_INT_SOURCE_IDENTIFIER_HH__

