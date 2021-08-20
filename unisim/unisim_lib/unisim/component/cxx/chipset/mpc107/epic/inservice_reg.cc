/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "unisim/component/cxx/chipset/mpc107/epic/inservice_reg.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace epic {

InserviceReg::InserviceReg() : irq(0) {
}

InserviceReg::~InserviceReg() {
	while (HasIRQ()) {
		RemoveIRQ();
	}
}

bool InserviceReg::HasIRQ() {
	return irq != 0;
}

uint32_t InserviceReg::Vector() {
	if(!HasIRQ()) return 0;
	
	return irq->vector;
}

uint32_t InserviceReg::Priority() {
	if(!HasIRQ()) return 0;
	
	return irq->priority;
}

uint32_t InserviceReg::Id() {
	if(!HasIRQ()) return 0;
	
	return irq->id;
}

/* Adds a new IRQ to the head of the register,
 *   note that this IRQ must have higher priority than IRQs already
 *   present in the register */
void InserviceReg::PushIRQ(uint32_t vector, uint32_t priority, uint32_t id) {
	IRQ *item;
	
	item = new IRQ();
	item->vector = vector;
	item->priority = priority;
	item->id = id;
	
	item->next = irq;
	item->prev = 0;
	if(HasIRQ())
		irq->prev = item;
	irq = item;
}

/* Removes the head IRQ of the register (that is, the IRQ with
 *   the highest priority */
void InserviceReg::RemoveIRQ() {
	IRQ *item;
	
	if(!HasIRQ()) return;
	item = irq;
	irq = item->next;
	if(HasIRQ())
		irq->prev = 0;
	delete item;
}

} // end of namespace epic
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
