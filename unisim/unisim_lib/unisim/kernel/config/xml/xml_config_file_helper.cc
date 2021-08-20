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

#include <unisim/kernel/config/xml/xml_config_file_helper.hh>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <map>
#include <iostream>

namespace unisim {
namespace kernel {
namespace config {
namespace xml {

using std::map;

const char *XMLConfigFileHelper::XML_ENCODING = "UTF-8"; 

XMLConfigFileHelper::XMLConfigFileHelper(unisim::kernel::Simulator *_simulator)
	: simulator(_simulator)
	, cur_var(0)
	, cur_status(NONE)
	, name_token(0)
	, variables_token(0)
	, object_token(0)
	, variable_token(0)
	, type_token(0)
	, value_token(0)
	, default_value_token(0)
	, data_type_token(0)
	, description_token(0)
	, _text_token(0)
	, empty_text(0)
{
	simulator->Register(this);
	
	xmlInitParser();

	name_token = xmlCharStrdup("name");
	variables_token = xmlCharStrdup("variables");
	object_token = xmlCharStrdup("object");
	variable_token = xmlCharStrdup("variable");
	type_token = xmlCharStrdup("type");
	value_token = xmlCharStrdup("value");
	default_value_token = xmlCharStrdup("default_value");
	data_type_token = xmlCharStrdup("data_type");
	description_token = xmlCharStrdup("description");
	_text_token = xmlCharStrdup("#text");
	empty_text = xmlCharStrdup("");
}

XMLConfigFileHelper::~XMLConfigFileHelper()
{
	xmlFree(name_token); name_token = 0;
	xmlFree(variables_token); variables_token = 0;
	xmlFree(object_token); object_token = 0;
	xmlFree(variable_token); variable_token = 0;
	xmlFree(type_token); type_token = 0;
	xmlFree(value_token); value_token = 0;
	xmlFree(default_value_token); default_value_token = 0;
	xmlFree(data_type_token); data_type_token = 0;
	xmlFree(description_token); description_token = 0;
	xmlFree(_text_token); _text_token = 0;
	xmlFree(empty_text); empty_text = 0;
	
	xmlCleanupParser();
}

const char *XMLConfigFileHelper::GetName() const
{
	return "XML";
}

bool XMLConfigFileHelper::SaveVariables(std::ostream& os, unisim::kernel::VariableBase::Type type)
{
	xmlTextWriterPtr writer;
	
	xmlBuffer *buffer = xmlBufferCreate();
	xmlOutputBuffer *output_buffer = xmlOutputBufferCreateBuffer(buffer, NULL);
	
	writer = xmlNewTextWriter(output_buffer);
	
	bool ret = SaveVariables(writer, type);
	
	if(ret)
	{
		std::string str((char *) buffer->content, buffer->use);
		os << str;
	}
	
	xmlBufferFree(buffer);
	
	return ret;
}


bool XMLConfigFileHelper::SaveVariables(const char *filename, unisim::kernel::VariableBase::Type type)
{
	xmlTextWriterPtr writer;

	writer = xmlNewTextWriterFilename(filename, 0);
	if(writer == NULL) {
		std::cerr << "Error(XMLConfigFileHelper::SaveVariables): "
			<< "could not open output file (" 
			<< filename << ")" << std::endl;
		return false;
	}
	
	bool ret = SaveVariables(writer, type);
	
	xmlFreeTextWriter(writer);
	
	return ret;
}

bool XMLConfigFileHelper::SaveVariables(xmlTextWriterPtr writer, unisim::kernel::VariableBase::Type type)
{
	int rc = xmlTextWriterSetIndent(writer, 1);
	if(rc < 0) {
		std::cerr << "Warning(XMLConfigFileHelper::SaveVariables): "
			<< "could not set indentation" << std::endl;
	} else {
		rc = xmlTextWriterSetIndentString(writer, (xmlChar*)"\t");
		if (rc < 0) {
			std::cerr << "Warning(XMLConfigFileHelper::SaveVariables): "
				<< "could not set indentation string" << std::endl;
		}
	}
	rc = xmlTextWriterStartDocument(writer, NULL,
			XML_ENCODING, NULL);
	if(rc < 0) {
		std::cerr << "Error(XMLConfigFileHelper::SaveVariables): "
			<< "error starting the xml document" << std::endl;
		return false;
	}
	// rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLES");
	rc = xmlTextWriterStartElement(writer, BAD_CAST "variables");
	if(rc < 0) {
		std::cerr << "Error(XMLConfigFileHelper::SaveVariables): "
			<< "error starting the xml document" << std::endl;
		return false;
	}

	/* First the variables of the different objects contained in the simulator
	 * are xmlfied, respecting the tree structure of the objects.
	 * Afterwards the simulator level variables (those that are directly 
	 * attached to the simulator).
	 */
	std::list<unisim::kernel::Object *> obj_list;
	std::list<unisim::kernel::Object *>::iterator obj_iter;
	simulator->GetRootObjects(obj_list);
	for ( obj_iter = obj_list.begin();
			obj_iter != obj_list.end();
			obj_iter++ )
	{
		if ( HasVariable(*obj_iter, type) )
		{
			rc = XmlfyVariables(writer, *obj_iter, type);
			if ( rc < 0 )
			{
				std::cerr << "Error(ServiceManage::SaveVariables): "
					<< "error writing root object"
					<< std::endl;
				return false;
			}
		}
	}

	std::list<unisim::kernel::VariableBase *> variables;
	simulator->GetVariables(variables, type);
	std::list<unisim::kernel::VariableBase *>::iterator var_iter;
	for ( var_iter = variables.begin();
			var_iter != variables.end();
			var_iter++ )
	{
		// check that the variable is a root variable by checking that it
		//   has not object owner
		unisim::kernel::VariableBase *var = *var_iter;
		bool root_var = (var->GetOwner() == 0);
		if ( root_var )
		{
			rc = XmlfyVariable(writer, var);
			if ( rc < 0 )
			{
				std::cerr << "Error(XMLConfigFileHelper::SaveVariables): "
						<< "error writing variable"
						<< std::endl;
				return false;
			}
		}
	}

	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) {
		std::cerr << "Error(XMLConfigFileHelper::SaveVariables): "
			<< "could not close the root element" << std::endl;
		return false;
	}
	rc = xmlTextWriterEndDocument(writer);
	if(rc < 0) {
		std::cerr << "Warning(XMLConfigFileHelper::SaveVariables): "
			<< "could not correctly close the XMLWriter"
			<< std::endl;
	}

	return true;
}

bool XMLConfigFileHelper::HasVariable(const unisim::kernel::Object *obj, unisim::kernel::VariableBase::Type type)
{
	std::list<unisim::kernel::VariableBase *> var_list;
	std::list<unisim::kernel::VariableBase *>::iterator var_iter;
	obj->GetVariables(var_list);
	for(var_iter = var_list.begin();
			var_iter != var_list.end();
			var_iter++) {
		if ((*var_iter)->IsSerializable() && (type == unisim::kernel::VariableBase::VAR_VOID ||
				type == (*var_iter)->GetType()))
			return true;
	}
	
	std::list<unisim::kernel::Object *> obj_list;
	std::list<unisim::kernel::Object *>::iterator obj_iter;
	obj_list = obj->GetLeafs();
	for (obj_iter = obj_list.begin();
			obj_iter != obj_list.end();
			obj_iter++)
	{
		if (HasVariable(*obj_iter, type))
			return true;
	}
	
	return false;
}

int XMLConfigFileHelper::XmlfyVariables(xmlTextWriterPtr writer, const unisim::kernel::Object *obj, unisim::kernel::VariableBase::Type type)
{
	int rc;

	// initiate object
	rc = xmlTextWriterStartElement(writer, BAD_CAST "object");
	if (rc < 0) return rc;

	// write object name
	{
		xmlChar* obj_name = xmlCharStrdup(obj->GetObjectName());

		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", obj_name);
		xmlFree(obj_name); obj_name = 0;
	}
	if (rc < 0) return rc;

	// dump inner objects
	std::list<unisim::kernel::Object *> obj_list;
	std::list<unisim::kernel::Object *>::iterator obj_iter;
	obj_list = obj->GetLeafs();
	for (obj_iter = obj_list.begin();
			obj_iter != obj_list.end();
			obj_iter++)
	{
		if (HasVariable(*obj_iter, type))
		{
			rc = XmlfyVariables(writer, *obj_iter, type);
			if (rc < 0)
			{
				std::cerr << "Error(ServiceManage::XmlfyVariables): "
					<< "error writing object"
					<< std::endl;
				return rc;
			}
		}
	}

	// dump object variables
	std::list<unisim::kernel::VariableBase *> var_list;
	std::list<unisim::kernel::VariableBase *>::iterator var_iter;
	obj->GetVariables(var_list);
	for(var_iter = var_list.begin();
			var_iter != var_list.end();
			var_iter++) {
		if (type == unisim::kernel::VariableBase::VAR_VOID ||
				type == (*var_iter)->GetType())
		{
			rc = XmlfyVariable(writer, *var_iter);
			if(rc < 0) {
				std::cerr << "Error(XMLConfigFileHelper::XmlfyVariables): "
					<< "error writing variable"
					<< std::endl;
				return rc;
			}
		}
	}
	
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) return rc;

	return 0;
}

int XMLConfigFileHelper::XmlfyVariable(xmlTextWriterPtr writer, const unisim::kernel::VariableBase *var) 
{
	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "variable");
	if(rc < 0) return rc;

	// write variable name
	{
		xmlChar* var_name = xmlCharStrdup(var->GetVarName());
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", var_name);
		xmlFree(var_name); var_name = 0;
	}
	if (rc < 0) return rc;

	// writing the variable type
	rc = xmlTextWriterStartElement(writer, BAD_CAST "type");
	if(rc < 0) return rc;
	switch(var->GetType()) {
		case unisim::kernel::VariableBase::VAR_PARAMETER:
			rc = xmlTextWriterWriteFormatString(writer, "parameter");
			break;
		case unisim::kernel::VariableBase::VAR_STATISTIC:
			rc = xmlTextWriterWriteFormatString(writer, "statistic");
			break;
		case unisim::kernel::VariableBase::VAR_REGISTER:
			rc = xmlTextWriterWriteFormatString(writer, "register");
			break;
		case unisim::kernel::VariableBase::VAR_SIGNAL:
			rc = xmlTextWriterWriteFormatString(writer, "signal");
			break;
		case unisim::kernel::VariableBase::VAR_VOID:
		case unisim::kernel::VariableBase::VAR_ARRAY:
			std::cerr << "Unexpected variable type to Xmlfy. Variable name is '"
				<< var->GetVarName() << "'" << std::endl;
			break;
	}
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

//	// writing the variable name
//	rc = xmlTextWriterStartElement(writer, BAD_CAST "name");
//	if(rc < 0) return rc;
//	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetName());
//	if(rc < 0) return rc;
//	rc = xmlTextWriterEndElement(writer);
//	if(rc < 0) return rc;

	// writing the variable data type
	rc = xmlTextWriterStartElement(writer, BAD_CAST "data_type");
	if(rc < 0) return rc;
	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetDataTypeName());
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

//	// writing the variable default value
//	rc = xmlTextWriterStartElement(writer, BAD_CAST "default_value");
//	if(rc < 0) return rc;
//	if(string("double").compare(var->GetDataTypeName()) == 0 ||
//		string("float").compare(var->GetDataTypeName()) == 0) {
//		rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *var);
//	} else {
//		rc = xmlTextWriterWriteFormatString(writer, "%s", ((string) *var).c_str());
//	}
//	if(rc < 0) return rc;
//	rc = xmlTextWriterEndElement(writer);
//	if(rc < 0) return rc;

	// writing the variable value
	rc = xmlTextWriterStartElement(writer, BAD_CAST "value");
	if(rc < 0) return rc;
	if(std::string("double").compare(var->GetDataTypeName()) == 0 ||
		std::string("float").compare(var->GetDataTypeName()) == 0) {
		rc = xmlTextWriterWriteFormatString(writer, "%e", (double) *var);
	} else {
		rc = xmlTextWriterWriteFormatString(writer, "%s", ((std::string) *var).c_str());
	}
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// writing variable description
	rc = xmlTextWriterStartElement(writer, BAD_CAST "description");
	if(rc < 0) return rc;
	rc = xmlTextWriterWriteFormatString(writer, "%s", var->GetDescription());
	if(rc < 0) return rc;
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	// if the variable has enumerated values then create
	// an entry with the possible values
	if(var->HasEnumeratedValues()) {
		rc = xmlTextWriterStartElement(writer, BAD_CAST "enumeration");
		if(rc < 0) return rc;
		std::vector<std::string> values;
		var->GetEnumeratedValues(values);
		if(values.empty()) return -1;
		std::vector<std::string>::iterator it;
		for(it = values.begin(); it != values.end(); it++) {
			rc = xmlTextWriterStartElement(writer, BAD_CAST "item");
			if(rc < 0) return rc;
			rc = xmlTextWriterWriteFormatString(writer, "%s", (*it).c_str());
			if(rc < 0) return rc;
			rc = xmlTextWriterEndElement(writer);
			if(rc < 0) return rc;
		}
		rc = xmlTextWriterEndElement(writer);
		if(rc < 0) return rc;
	}

	// closing variable element
	rc = xmlTextWriterEndElement(writer);
	if(rc < 0) return rc;

	return 1;
}

bool XMLConfigFileHelper::LoadVariables(const char *_filename, unisim::kernel::VariableBase::Type type)
{
	std::string filename = simulator->SearchSharedDataFile(_filename);
	
	xmlTextReaderPtr reader;

	std::cerr << "Loading xml parameters from: " << filename << std::endl;
	
	/*
	 * Pass some special parsing options to activate DTD attribute defaulting,
	 * entities substitution and DTD validation
	 */
	reader = xmlReaderForFile(filename.c_str(), NULL, 0);
	if (reader != NULL) {
		return LoadVariables(reader, type);
	} else {
		std::cerr << "Unable to open " << filename << std::endl;
		return false;
	}
}

bool XMLConfigFileHelper::LoadVariables(xmlTextReaderPtr reader, unisim::kernel::VariableBase::Type type)
{
	int ret;
	cur_status = NONE;
	cur_var = NULL;
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
		if(ProcessXmlVariableNode(reader, type))
			ret = xmlTextReaderRead(reader);
		else
			ret = 0;
	}
	xmlFreeTextReader(reader);
	if (ret != 0) {
		std::cerr << "failed to parse" << std::endl;
		return false;
	}
	return true;
}

bool XMLConfigFileHelper::LoadVariables(std::istream& is, unisim::kernel::VariableBase::Type type)
{
	xmlTextReaderPtr reader;
	
	std::string str;
	
	char buffer[4096];
	do
	{
		std::streamsize n = is.readsome(buffer, sizeof(buffer));
		
		if(is.fail()) return false;
		if(n <= 0) break;
		
		if(n > 0)
		{
			str.append(buffer, n);
		}
	}
	while(1);
	
	reader = xmlReaderForMemory(&str[0], str.length(), "", XML_ENCODING, 0);
	
	return LoadVariables(reader, type);
}

bool 
XMLConfigFileHelper::
ProcessXmlVariableNode(xmlTextReaderPtr reader, unisim::kernel::VariableBase::Type type) 
{ 
	const xmlChar* name = 0;
	const xmlChar* value = 0;
	xmlChar* name_attr = 0;

	name = xmlTextReaderConstName(reader);
	if (name == NULL) 
	{
		std::cerr << "Could not read Xml variable node" << std::endl;
		return false;
	}
	
	if (xmlStrEqual(name, variables_token))
	{
//		if (xmlTextReaderNodeType(reader) == 1)
//			cerr << "variables" << endl;
		return true;
	}

	if (xmlStrEqual(name, object_token))
	{
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
		{
			name_attr = xmlTextReaderGetAttribute(reader, name_token);
			if (name_attr == 0)
			{
				std::cerr << "Error: could not get object name" << std::endl;
				return false;
			}
			std::cerr << " + object " << name_attr << std::endl;
			cur_object.push_back(std::string((char *)name_attr));
			xmlFree(name_attr);
		}
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
		{
			std::cerr << " - object " << std::endl;
			cur_object.pop_back();
		}
		return true;
	}
	
	if (xmlStrEqual(name, variable_token))
	{
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
		{
			name_attr = xmlTextReaderGetAttribute(reader, name_token);
			if (name_attr == 0)
			{
				std::cerr << "Error: could not get variable name" << std::endl;
				return false;
			}
			cur_var = new CurVariable();
			for(CurObject::const_iterator it = cur_object.begin(); it !=  cur_object.end(); ++it)
			{
				cur_var->name << (*it) << ".";
			}
			cur_var->name << name_attr;
			xmlFree(name_attr);
		}
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
		{
//			cerr << "    variable" << endl;
//			cerr << "      type = " << cur_var->type.str() << endl;
//			cerr << "      name = " << cur_var->name.str() << endl;
//			cerr << "      value = " << cur_var->value.str() << endl;
			// cerr << "    description = " << cur_var->description.str() << endl;
			
			bool modify = 
				(type == unisim::kernel::VariableBase::VAR_VOID) ||
				(cur_var->type.str().empty()) ||
				(type == unisim::kernel::VariableBase::VAR_PARAMETER && cur_var->type.str().compare("parameter") == 0) ||
				(type == unisim::kernel::VariableBase::VAR_REGISTER && cur_var->type.str().compare("register") == 0) ||
				(type == unisim::kernel::VariableBase::VAR_STATISTIC && cur_var->type.str().compare("statistic") == 0);
			if (modify)
			{
				simulator->SetVariable(cur_var->name.str().c_str(), cur_var->value.str().c_str());
			}
			delete cur_var;
		}
		return true;
	}

	if(xmlStrEqual(name, type_token)) {
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			cur_status = TYPE;
		}
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {
			cur_status = NONE;
		}
		return true;
	}

	// TODO: this code should be removed because name is now an attribute
	if(xmlStrEqual(name, name_token)) {
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			cur_status = NAME;
		}
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {
			cur_status = NONE;
		}
		return true;
	}
	// end of TODO
	
	// TODO: this code should be removed because default_value has dissappeared
	if(xmlStrEqual(name, default_value_token)) {
		// nothing to do
		return true;
	}
	// end of TODO

	if(xmlStrEqual(name, value_token)) {
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			cur_status = VALUE;
		}
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, description_token)) {
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			cur_status = DESCRIPTION;
		}
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {
			cur_status = NONE;
		}
		return true;
	}
	
	if(xmlStrEqual(name, data_type_token)) {
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
			cur_status = DATA_TYPE;
		}
		if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {
			cur_status = NONE;
		}
	}
	
	if(xmlStrEqual(name, _text_token)) {
		value = (xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) ? xmlTextReaderConstValue(reader) : empty_text;
		switch(cur_status) {
		case NONE:
			break;
		case TYPE:
			cur_var->type << value;
			break;
		// TODO: this code should be removed because name now is an attribute
		case NAME:
			cur_var->name << value;
			break;
		// end of TODO
		case VALUE:
			cur_var->value << value;
			break;
		case DESCRIPTION:
			// cur_var->description << value;
			break;
		case DATA_TYPE:
			cur_var->data_type << value;
			break;
		}
		return true;
	}
	
	return true;
}

} // end of namespace xml
} // end of namespace config
} // end of namespace kernel
} // end of namespace unisim
