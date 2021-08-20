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

#ifndef __UNISIM_KERNEL_CONFIG_XML_CONFIG_FILE_HELPER_HH__
#define __UNISIM_KERNEL_CONFIG_XML_CONFIG_FILE_HELPER_HH__

#include <vector>
#include <sstream>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace kernel {
namespace config {
namespace xml {

class XMLConfigFileHelper : public unisim::kernel::ConfigFileHelper
{
public:
	XMLConfigFileHelper(unisim::kernel::Simulator *simulator);
	~XMLConfigFileHelper();
	
	virtual const char *GetName() const;
	virtual bool SaveVariables(const char *filename, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	virtual bool SaveVariables(std::ostream& os, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	virtual bool LoadVariables(const char *filename, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	virtual bool LoadVariables(std::istream& is, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	
private:
	static const char *XML_ENCODING; 

	bool SaveVariables(xmlTextWriterPtr writer, unisim::kernel::VariableBase::Type type);
	bool LoadVariables(xmlTextReaderPtr reader, unisim::kernel::VariableBase::Type type);

	bool HasVariable(const unisim::kernel::Object *obj,
	                 unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);

	int XmlfyVariables(xmlTextWriterPtr writer,
	                   const unisim::kernel::Object *obj,
	                   unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	
	int XmlfyVariable(xmlTextWriterPtr writer, 
	                  const unisim::kernel::VariableBase *var);
	
	unisim::kernel::Simulator *simulator;
	
	bool ProcessXmlVariableNode(xmlTextReaderPtr reader,
	                            unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	
	class CurVariable {
	public:
		std::stringstream type;
		std::stringstream name;
		std::stringstream value;
		std::stringstream description;
		std::stringstream data_type;
	};
	
	enum CurStatus {NONE, TYPE, NAME, VALUE, DESCRIPTION, DATA_TYPE};
	CurVariable *cur_var;
	CurStatus cur_status;
	typedef std::vector<std::string> CurObject;
	CurObject cur_object;

	// tokens required
	xmlChar* name_token;
	xmlChar* variables_token;
	xmlChar* object_token;
	xmlChar* variable_token;
	xmlChar* type_token;
	xmlChar* value_token;
	xmlChar* default_value_token;
	xmlChar* data_type_token;
	xmlChar* description_token;
	xmlChar* _text_token;
	xmlChar* empty_text;
};

} // end of namespace xml
} // end of namespace config
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_CONFIG_XML_CONFIG_FILE_HELPER_HH__
