/*
 *  Copyright (c) 20016e,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <iostream>
#include <systemc.h>
#include <simulator.hh>

int sc_main(int argc, char *argv[])
{
  int ret = 0;

  Simulator simulator( argc, argv );

  switch (simulator.Setup())
    {
    case unisim::kernel::Simulator::ST_ERROR:
      std::cerr << "ERROR: Can't start simulation because of previous erros" << std::endl;
      ret = -1;
      break;
    case unisim::kernel::Simulator::ST_OK_DONT_START:
      std::cerr << "Successfully configured the simulator." << std::endl;
      ret = 0;
      break;
    case unisim::kernel::Simulator::ST_WARNING:
      std::cerr << "WARNING: problems detected during setup."
           << " Starting simulation anyway, but errors could appear during "
           << "the simulation." << std::endl;
    case unisim::kernel::Simulator::ST_OK_TO_START:
      std::cerr << "Starting simulation." << std::endl;
      ret = simulator.Run();
      break;
    }

  return ret;
}
