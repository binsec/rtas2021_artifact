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
 
#include <string>
#include <sstream>
#include <stdexcept>
#include "unisim/service/os/linux_os/linux_os_exception.hh"

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

using std::string;
using std::stringstream;
using std::endl;
using std::runtime_error;

/* Exceptions implementation */
UnimplementedSystemCall::UnimplementedSystemCall(const char *function,
		const char *file,
        int line,
        unsigned int number) : runtime_error("") {
	this->function = new string(function);
	this->file = new string(file);
	this->line = line;
	this->number = number;
	stringstream sstr;
	sstr << "(" << *function
       	<< ":" << *file
       	<< ":" << line
       	<< "): syscall number " << number << " not implemented (or not associated)" 
       	<< endl;
	str = sstr.str();
}
  
UnimplementedSystemCall::~UnimplementedSystemCall() throw() {
	delete function;
	delete file;
}
  
const char *
UnimplementedSystemCall::what() const throw() {
	return str.c_str();
}

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim
