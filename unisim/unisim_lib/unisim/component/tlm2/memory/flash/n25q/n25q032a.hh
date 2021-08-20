/*
 *  Copyright (c) 2016,
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

#include <unisim/component/tlm2/memory/flash/n25q/n25q.hh>
#include <unisim/component/tlm2/memory/flash/n25q/config.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

typedef N25Q<N25Q032A1_CONFIG32> N25Q032A1_32; typedef N25Q<N25Q032A1_CONFIG64> N25Q032A1_64; // 32 Mb, 65 nm, Byte addressability; HOLD pin; Micron XIP
typedef N25Q<N25Q032A2_CONFIG32> N25Q032A2_32; typedef N25Q<N25Q032A2_CONFIG64> N25Q032A2_64; // 32 Mb, 65 nm, Byte addressability; HOLD pin; Basic XIP
typedef N25Q<N25Q032A3_CONFIG32> N25Q032A3_32; typedef N25Q<N25Q032A3_CONFIG64> N25Q032A3_64; // 32 Mb, 65 nm, Byte addressability; RST# pin; Micron XIP
typedef N25Q<N25Q032A4_CONFIG32> N25Q032A4_32; typedef N25Q<N25Q032A4_CONFIG64> N25Q032A4_64; // 32 Mb, 65 nm, Byte addressability; RST# pin; Basic XIP

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim
