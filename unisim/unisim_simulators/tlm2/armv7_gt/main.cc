/*
 *  Copyright (c) 2009,
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

#include <simulator.hh>
#include <scml2.h>
#include <scml.h>
#include <scmlinc/scml_simple_property_server.h>
#include <scml2_logging.h>
#include <systemc.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	int ret = 0;

  scml2::logging::registry& registry = scml2::logging::registry::get_instance();
  scml2::logging::logger_base *default_cerr_logger = registry.find_logger_by_name("default_cerr_logger");
  if(default_cerr_logger)
    {
      bool debug = true;
      if(debug)
        default_cerr_logger->enable(scml2::logging::match::severity_threshold_match(scml2::logging::severity::debug().get_level()));
      else
        default_cerr_logger->disable(scml2::logging::match::severity_threshold_match(scml2::logging::severity::debug().get_level()));
    }

  const char *settings_filename = "settings.txt";
  scml_simple_property_server simple_property_server;
  if(!simple_property_server.load(settings_filename))
    {
      std::cerr << "Error while loading \"" << settings_filename << "\"" << std::endl;
      return 0;
    }
  scml_property_registry& property_registry = scml_property_registry::inst();
  property_registry.setCustomPropertyServer(&simple_property_server);


	Simulator *simulator = new Simulator(argc, argv);

	switch ( simulator->Setup() )
	{
	case unisim::kernel::Simulator::ST_ERROR:
		cerr << "ERROR: Can't start simulation because of previous erros" << endl;
		ret = -1;
		break;
	case unisim::kernel::Simulator::ST_OK_DONT_START:
		cerr << "Successfully configured the simulator." << endl;
		ret = 0;
		break;
	case unisim::kernel::Simulator::ST_WARNING:
		cerr << "WARNING: problems detected during setup."
			<< " Starting simulation anyway, but errors could appear during "
			<< "the simulation." << endl;
	case unisim::kernel::Simulator::ST_OK_TO_START:
		cerr << "Starting simulation." << endl;
		ret = simulator->Run();
		break;
	}

	if (simulator) delete simulator;

	return ret;
}
