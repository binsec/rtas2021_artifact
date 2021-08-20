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

#include <unisim/kernel/logger/text_file/text_file_writer.hh>

#include <iostream>

namespace unisim {
namespace kernel {
namespace logger {
namespace text_file {

Writer::Writer()
	: unisim::kernel::Object("text-file", Logger::StaticServerInstance(), "Text File Log Writer")
	, Printer()
	, enable(false)
	, filename("logger_output.txt")
	, param_enable("enable", this, enable, "Keep logger output in a file")
	, param_filename("filename", this, filename, "Filename to keep logger output (the option file must be activated)")
{
	param_filename.SetMutable(false);
	
	/* check if a file text output needs to be created */
	if(enable)
	{
		/* create and initialize the text output file */
		text_file.open(filename.c_str(), std::ios::trunc);
		if (!text_file.is_open())
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "could not open text output file for logging" << std::endl;
			this->Stop(-1);
			return;
		}
	}
}

Writer::~Writer()
{
	if (text_file.is_open())
	{
		text_file.close();
	}
}

void Writer::Print(mode_t mode, const char *name, const char *buffer)
{
	if(enable)
	{
		text_file << name << ": ";
		switch(mode)
		{
			case WARNING_MODE: text_file << "WARNING! "; break;
			case ERROR_MODE: text_file << "ERROR! "; break;
			default: break;
		}
		text_file << buffer << std::endl;
	}
}

} // end of namespace text_file
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
