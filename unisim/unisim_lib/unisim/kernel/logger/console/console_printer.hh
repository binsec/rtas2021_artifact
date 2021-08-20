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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Yves Lhuillier (yves.lhuillier@cea.fr) 
 *          Daniel Gracia Pérez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_KERNEL_LOGGER_CONSOLE_CONSOLE_PRINTER_HH__
#define __UNISIM_KERNEL_LOGGER_CONSOLE_CONSOLE_PRINTER_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger_server.hh>
#include <iostream>

namespace unisim {
namespace kernel {
namespace logger {
namespace console {

struct Printer
	: unisim::kernel::Object
	, unisim::kernel::logger::Printer
{
	Printer();
	virtual ~Printer();
	virtual void Print(mode_t mode, const char *name, const char *buffer);
private:
	bool enable_std_err;
	bool enable_std_out;
	bool enable_std_err_color;
	bool enable_std_out_color;
	
	unisim::kernel::variable::Parameter<bool> param_enable_std_err;
	unisim::kernel::variable::Parameter<bool> param_enable_std_out;
	unisim::kernel::variable::Parameter<bool> param_enable_std_err_color;
	unisim::kernel::variable::Parameter<bool> param_enable_std_out_color;
	
	/** Message debug log command
	 * Loggers should call this method (using the handle obtained with GetInstance)
	 *   to log a debug message.
	 *
	 * @param os output stream
	 * @param opt_color whether to color message
	 * @param mode type of debug message (info, warning or error)
	 * @param obj the unisim::kernel::Object that is sending the debug message
	 * @param buffer the debug message
	 */
	void Print(std::ostream& os, bool opt_color, mode_t mode, const char *name, const char *buffer);
};

} // end of namespace console
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_CONSOLE_CONSOLE_PRINTER_HH__
