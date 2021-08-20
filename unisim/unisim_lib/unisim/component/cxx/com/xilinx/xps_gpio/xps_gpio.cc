/*
 *  Copyright (c) 2011,
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

#include <unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.hh>
#include <unisim/component/cxx/com/xilinx/xps_gpio/xps_gpio.tcc>
#include <unisim/component/cxx/com/xilinx/xps_gpio/config.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_gpio {

const unsigned int Config::C_SPLB_DWITH;
const unsigned int Config::C_GPIO_WIDTH;
const unsigned int Config::C_GPIO2_WIDTH;
const Config::MEMORY_ADDR Config::C_BASEADDR;
const Config::MEMORY_ADDR Config::C_HIGHADDR;
	
const uint32_t Config::C_DOUT_DEFAULT;
const uint32_t Config::C_TRI_DEFAULT;
const uint32_t Config::C_DOUT_DEFAULT_2;
const uint32_t Config::C_TRI_DEFAULT_2;
const bool Config::C_IS_DUAL;
const bool Config::C_INTERRUPT_IS_PRESENT;

template class XPS_GPIO<Config>;
template void XPS_GPIO<Config>::Read(Config::MEMORY_ADDR, uint8_t& value);
template void XPS_GPIO<Config>::Read(Config::MEMORY_ADDR, uint16_t& value);
template void XPS_GPIO<Config>::Read(Config::MEMORY_ADDR, uint32_t& value);
template void XPS_GPIO<Config>::Write(Config::MEMORY_ADDR, uint8_t value);
template void XPS_GPIO<Config>::Write(Config::MEMORY_ADDR, uint16_t value);
template void XPS_GPIO<Config>::Write(Config::MEMORY_ADDR, uint32_t value);

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
