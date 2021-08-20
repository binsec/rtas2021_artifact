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

#include "unisim/component/cxx/chipset/mpc107/dma/register.hh"

#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace dma {

using std::string;

const string Registers :: DMR_NAME_0 = "DMR0";
const string Registers :: DMR_NAME_1 = "DMR1";
const string Registers :: DSR_NAME_0 = "DSR0";
const string Registers :: DSR_NAME_1 = "DSR1";
const string Registers :: CDAR_NAME_0 = "CDAR0";
const string Registers :: CDAR_NAME_1 = "CDAR1";
const string Registers :: SAR_NAME_0 = "SAR0";
const string Registers :: SAR_NAME_1 = "SAR1";
const string Registers :: DAR_NAME_0 = "DAR0";
const string Registers :: DAR_NAME_1 = "DAR1";
const string Registers :: BCR_NAME_0 = "BCR0";
const string Registers :: BCR_NAME_1 = "BCR1";
const string Registers :: NDAR_NAME_0 = "NDAR0";
const string Registers :: NDAR_NAME_1 = "NDAR1";

Registers ::
Registers() {
	dmr[0] = 0;
	dmr[1] = 0;
	dsr[0] = 0;
	dsr[1] = 0;
	cdar[0] = 0;
	cdar[1] = 0;
	sar[0] = 0;
	sar[1] = 0;
	dar[0] = 0;
	dar[1] = 0;
	bcr[0] = 0;
	bcr[1] = 0;
	ndar[0] = 0;
	ndar[1] = 0;
}

Registers ::
~Registers() {
}

} // end of namespace dma
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace dma
