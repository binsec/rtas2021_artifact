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

#include <unisim/kernel/logger/xml_file/xml_file_writer.hh>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <sstream>
#include <iostream>

namespace unisim {
namespace kernel {
namespace logger {
namespace xml_file {

static const char *XML_ENCODING = "UTF-8";

Writer::Writer()
	: unisim::kernel::Object("xml-file", Logger::StaticServerInstance(), "XML Log File Writer")
	, Printer()
	, enable(false)
	, filename("logger_output.xml")
	, gzipped(false)
	, param_enable("enable", this, enable, "Keep logger output in a file xml formatted")
	, param_filename("filename", this, filename, "Filename to keep logger xml output (the option xml_file must be activated)")
	, param_gzipped("gzipped", this, gzipped, "Compress the xml output (a .gz extension is automatically appended to the xml_filename option)")
	, xml_writer(0)
{
	param_gzipped.SetMutable(false);
	param_filename.SetMutable(false);
	
	/* check if a xml output needs to be created */
	if(enable) 
	{
		/* create and initialize the xml output */
		std::stringstream xml_filename;
		xml_filename << filename;
		if (gzipped) xml_filename << ".gz";
		xml_writer = xmlNewTextWriterFilename(xml_filename.str().c_str(), gzipped ? 1 : 0);
		if (xml_writer == NULL) 
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "could not open xml output file for logging" << std::endl;
			this->Stop(-1);
			return;
		}
		int rc = xmlTextWriterSetIndent(xml_writer, 1);
		if (rc < 0) 
		{
			std::cerr << "Warning(LoggerServer::Setup): "
			          << "could not set indentation" << std::endl;
		}
		rc = xmlTextWriterStartDocument(xml_writer, NULL, XML_ENCODING, NULL);
		if (rc < 0) 
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "error starting the xml document" << std::endl;
			this->Stop(-1);
			return;
		}
		rc = xmlTextWriterStartElement(xml_writer, BAD_CAST "LOGGER");
		if (rc < 0) 
		{
			std::cerr << "Error(LoggerServer::Setup): "
			          << "error starting the xml document" << std::endl;
			this->Stop(-1);
			return;
		}
	}
}

Writer::~Writer()
{
	if(xml_writer != NULL)
	{
		int rc = xmlTextWriterEndElement(xml_writer);
		if(rc < 0)
		{
			std::cerr << "Error(LoggerServer): "
			          << "could not close the root element of xml output" << std::endl;
		}
		rc = xmlTextWriterEndDocument(xml_writer);
		if(rc < 0)
		{
			std::cerr << "Warning(LoggerServer::Close): "
			          << "could not correctly close the xml output file" << std::endl;
		}
		xmlFreeTextWriter(xml_writer);
		xml_writer = NULL;
	}
}

void Writer::Print(mode_t mode, const char *name, const char *buffer)
{
	if(enable)
	{
		switch(mode)
		{
			case INFO_MODE: XmlDebug("info", name, buffer); break;
			case WARNING_MODE: XmlDebug("warning", name, buffer); break;
			case ERROR_MODE: XmlDebug("error", name, buffer); break;
			default: break;
		}
	}
}

void Writer::XmlDebug(const char *type, const char *name, const char *buffer)
{
	int rc;
	rc = xmlTextWriterStartElement(xml_writer, BAD_CAST "DEBUG");
	if (rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not add a debug message of type \"" << type << "\"" << std::endl;
	}
	xmlChar *xml_type = xmlCharStrdup(type);
	rc = xmlTextWriterWriteAttribute(xml_writer, BAD_CAST "type", xml_type);
	free(xml_type);
	if(rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not add \"type\" attribute to debug message of type \"" << type << "\"" << std::endl;
	}
	xmlChar *xml_obj_name = xmlCharStrdup(name);
	rc = xmlTextWriterWriteAttribute(xml_writer, BAD_CAST "source", xml_obj_name);
	free(xml_obj_name);
	if (rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not add \"source\" attribute to debug message of type \"" << type << "\"" << std::endl;
	}
	rc = xmlTextWriterWriteFormatString(xml_writer, "%s", buffer);
	if (rc < 0) 
	{
		std::cerr << "Error(LoggerServer): could not attach message buffer to debug message of type \"" << type << "\" buffer = \"" << buffer << "\"" << std::endl;
	}
	rc = xmlTextWriterEndElement(xml_writer);
	if (rc < 0)
	{
		std::cerr << "Error(LoggerServer): could not close debug message of type \"" << type << "\"" << std::endl;
	}
}

} // end of namespace xml_file
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
