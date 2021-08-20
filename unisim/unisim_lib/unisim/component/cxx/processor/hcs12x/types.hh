/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_TYPES_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_TYPES_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

typedef uint16_t address_t;
typedef uint32_t physical_address_t;
typedef uint16_t reg_t;
typedef uint64_t service_address_t;
typedef uint8_t	 page_t;
typedef uint64_t clock_t;

class TOWNER {
public:
	enum OWNER { UNKNOWN, CPU12X, XGATE, MASTER3 };

};

class ADDRESS {
	public:
		enum MODE {DIRECT=0, EXTENDED=1};
		enum ENCODING {BANKED=0, LINEAR=1, GNUGCC=2};
};

class MMC_DATA {
public:
	MMC_DATA() : logicalAddress(0x00), type(ADDRESS::EXTENDED), isGlobal(false), buffer(0), data_size(0), isExecute(false) {}

	address_t logicalAddress;
	ADDRESS::MODE type;
	bool isGlobal;
	void *buffer;
	uint32_t data_size;
	bool isExecute;
};


class TSemaphore {
public:

	TSemaphore() : locked(false), who(TOWNER::UNKNOWN) {}
	~TSemaphore() { }

	bool isLocked() { return (locked); }
	TOWNER::OWNER getLocker() { return (who); }

	bool lock(TOWNER::OWNER who) {
		if (!locked) {
			locked = true;
			this->who = who;
			return (true);
		}

		return (false);
	}

	bool unlock(TOWNER::OWNER who)  {
		if (locked && (this->who == who)) {
			locked = false;
			this->who = TOWNER::UNKNOWN;
			return (true);
		}

		return (false);
	}

private:
	bool  locked;
	TOWNER::OWNER who;
};

} // end hcs12x namespace
} // end processor namespace
} // end cxx namespace
} // end component namespace
} // end unisim namespace

#endif
