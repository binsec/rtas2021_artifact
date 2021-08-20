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

#include <unisim/kernel/logger/console/console_printer.hh>

#include <string>
#include <cstring>

namespace unisim {
namespace kernel {
namespace logger {
namespace console {

Printer::Printer()
	: unisim::kernel::Object("console", Logger::StaticServerInstance(), "Console Log printer")
	, unisim::kernel::logger::Printer()
	, enable_std_err(true)
	, enable_std_out(false)
	, enable_std_err_color(false)
	, enable_std_out_color(false)
	, param_enable_std_err("enable-std-err", this, enable_std_err, "Show logger output through the standard error output")
	, param_enable_std_out("enable-std-out", this, enable_std_out, "Show logger output through the standard output")
	, param_enable_std_err_color("enable-std-err-color", this, enable_std_err_color, "Colorize logger output through the standard error output (only works if std_err is active)")
	, param_enable_std_out_color("enable-std-out-color", this, enable_std_out_color, "Colorize logger output through the standard output (only works if std_out is active)")
{
}

Printer::~Printer()
{
}

void Printer::Print(mode_t mode, const char *name, const char *buffer)
{
	if(enable_std_out)
	{
		Print(std::cout, enable_std_out_color, mode, name, buffer);
	}
	if(enable_std_err)
	{
		Print(std::cerr, enable_std_err_color, mode, name, buffer);
	}
}

void Printer::Print(std::ostream& os, bool opt_color, mode_t mode, const char *name, const char *buffer)
{
	if(opt_color)
	{
		switch(mode)
		{
			case INFO_MODE: os << "\033[36m"; break;
			case WARNING_MODE: os << "\033[33m"; break;
			case ERROR_MODE: os << "\033[31m"; break;
			default: break;
		}
	}

	os << name << ": ";
	switch(mode)
	{
		case WARNING_MODE: os << "WARNING! "; break;
		case ERROR_MODE: os << "ERROR! "; break;
		default: break;
	}
	int prefix_length = strlen(name) + 2;
	std::string prefix(prefix_length, ' ');
	const char *b = buffer;
	for (const char *p = strchr(b, '\n'); p != NULL; p = strchr(b, '\n'))
	{
		os.write(b, p - b);
		os << std::endl << prefix;
		b = p + 1;
	}
	os << b << std::endl;
		
	if (opt_color) os << "\033[0m";
}

} // end of namespace console
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
