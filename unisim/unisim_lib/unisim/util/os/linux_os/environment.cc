/*
 *  Copyright (c) 2011
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include <string>
#include <vector>
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) | defined(_WIN64)
#include <stdlib.h>
#else
#include <unistd.h>
#endif

#include <string>

#include "unisim/util/os/linux_os/environment.hh"

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

// Parses the environment entry (env_entry) looking for the equal sign to
// extract the environment entry variable name. If "=" is not found return the
// complete entry. If the position of "=" is 0 (the first character) return an
// empty string.
std::string GetEnvironmentVariable(std::string env_entry) {
  size_t pos = env_entry.find("=");
  if (pos == std::string::npos) return env_entry;
  if (pos == 0) return std::string("");
  return env_entry.substr(0, pos - 1);
}

// Gets the host environment and apply the environment set by the
// user (input_envp).
// For that purpose two steps are performed:
// - (1) copy the host environment
// - (2) parse the input_envp and add the variables if they are not present in
//       the envp, or modify the envp variable if it is present
void GetHostEnvironment(std::vector<std::string> input_envp,
                        std::vector<std::string> *envp) {
  // Obtain the current host environment
  for (char **env = environ; *env != NULL; env++)
  {
    envp->push_back(std::string(*env));
  }

  // Parse the input_envp and modify the variables from envp that match,
  // variables from input_envp not present in envp are added to envp
  for (std::vector<std::string>::iterator input_it = input_envp.begin();
       input_it != input_envp.end();
       input_it++) {
    std::string input_variable(GetEnvironmentVariable(*input_it));
    bool variable_found = false;
    for (std::vector<std::string>::iterator envp_it = envp->begin();
         (!variable_found) && (envp_it != envp->end());
         envp_it++) {
      std::string envp_variable(GetEnvironmentVariable(*envp_it));
      variable_found = (envp_variable.compare(input_variable) == 0);
      *envp_it = *input_it;
    }
    if (!variable_found)
      envp->push_back(*input_it);
  }
}

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

