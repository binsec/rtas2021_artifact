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
 * Authors: Gilles	Mouchard <gilles.mouchard@cea.fr>
 * 			Reda	Nouacer  <reda.nouacer@cea.fr>
 */



#include "xml_atd_pwm_stub.hh"

XML_ATD_PWM_STUB::XML_ATD_PWM_STUB(const sc_module_name& name, Object *parent) :
ATD_PWM_STUB(name, parent),

atd0_anx_stimulus_file(""),
param_atd0_anx_stimulus_file("atd0-anx-stimulus-file", this, atd0_anx_stimulus_file),
atd0_anx_start_channel(0),
param_atd0_anx_start_channel("atd0-anx-start-channel", this, atd0_anx_start_channel),
atd0_anx_wrap_around_channel(ATD0_SIZE -1),
param_atd0_anx_wrap_around_channel("atd0-anx-wrap-around-channel", this, atd0_anx_wrap_around_channel),

atd1_anx_stimulus_file(""),
param_atd1_anx_stimulus_file("atd1-anx-stimulus-file", this, atd1_anx_stimulus_file),
atd1_anx_start_channel(0),
param_atd1_anx_start_channel("atd1-anx-start-channel", this, atd1_anx_start_channel),
atd1_anx_wrap_around_channel(ATD1_SIZE -1),
param_atd1_anx_wrap_around_channel("atd1-anx-wrap-around-channel", this, atd1_anx_wrap_around_channel)

, cosim_enabled(false)
, param_cosim_enabled("cosim-enabled", this, cosim_enabled)

, atd0_xml_enabled(false)
, param_atd0_xml_enabled("atd0-xml-enabled", this, atd0_xml_enabled)

, atd1_xml_enabled(false)
, param_atd1_xml_enabled("atd1-xml-enabled", this, atd1_xml_enabled)

, atd0_rand_enabled(false)
, param_atd0_rand_enabled("atd0-rand-enabled", this, atd0_rand_enabled)

, atd1_rand_enabled(false)
, param_atd1_rand_enabled("atd1-rand-enabled", this, atd1_rand_enabled)

{

	SC_HAS_PROCESS(XML_ATD_PWM_STUB);

	SC_THREAD(processATD0);
	SC_THREAD(processATD1);
	SC_THREAD(processPWM);

}

XML_ATD_PWM_STUB::~XML_ATD_PWM_STUB() {

	for (unsigned int i=0; i < atd0_vect.size(); i++) {
		delete atd0_vect.at(i);
	}

	for (unsigned int i=0; i < atd1_vect.size(); i++) {
		delete atd1_vect.at(i);
	}

	atd0_vect.clear();
	atd1_vect.clear();

}


template <int SIZE> void XML_ATD_PWM_STUB::parseRow (xmlDocPtr doc, xmlNodePtr cur, data_t<SIZE> &data) {

	xmlChar *key;
	vector<xmlNodePtr> rowCells;

	if ((xmlStrcmp(cur->name, (const xmlChar *)"Row"))) {
		cerr << "Error: Can't parse " << cur->name << endl;
		return;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Cell"))) {
			rowCells.push_back(cur);
		}
		cur = cur->next;
	}

	if ((rowCells.size() < 2) || (rowCells.size() > (SIZE+1))) {
		cerr << "Error: Wrong row size. The minimum is 2 and the maximum is " << std::dec << (SIZE+1) << " cells by row." << endl;
		return;
	}

	for (int i=0; i < SIZE; i++) {
		data.volte[i] = 0;
	}
	data.time = 0;

	/**
	 * First cells of row are ATD voltage
	 * The last cell is time of sampling
	 */
	for (unsigned int i=0; (i < rowCells.size()) && (i < SIZE); i++) {
		cur = rowCells.at(i);

		xmlNodePtr node = cur->children;
		while (xmlStrcmp(node->name, (const xmlChar *)"Data")) {
			node = node->next;
		}

		key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
		if (i < (rowCells.size()-1)) {
			data.volte[i] = std::atof((const char *) key);
		} else {
			data.time = std::atof((const char *) key);
		}

		xmlFree(key);

	}


	return;
}

template <int SIZE> int XML_ATD_PWM_STUB::RandomizeData(std::vector<data_t<SIZE>* > &vect) {


	const int SET_SIZE = 1024;
	const double TIME_STEP = 0.080; // 0.080 Millisecond
	double time = 0;

	srand(12345);

	data_t<SIZE>* data;
	for (int i=0; i < SET_SIZE; i++) {
		data = new data_t<SIZE>();
		for (uint8_t j=0; j < SIZE; j++) {
			data->volte[j] = 5.2 * ((double) rand() / (double) RAND_MAX); // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			data->time = time;
		}

		time = time + TIME_STEP;

		vect.push_back(data);
	}

	return (SET_SIZE);
}

template <int SIZE> int XML_ATD_PWM_STUB::LoadXmlData(const char *filename, std::vector<data_t<SIZE>* > &vect) {

	const char *path = "//Row";

	xmlDocPtr doc = NULL;
	xmlXPathContextPtr context = NULL;
	xmlXPathObjectPtr xmlobject;
	int result = 0;

	cout << sc_object::name() << " Parsing " << filename << endl;

	doc = xmlParseFile (GetSimulator()->SearchSharedDataFile(filename).c_str());
	if (!doc)
	{
		cerr << __FILE__ << ":" << __LINE__ << " Could not parse the document" << endl;
		return (0);
	}

	xmlXPathInit ();
	context = xmlXPathNewContext (doc);

	xmlobject = xmlXPathEval ((xmlChar *) path, context);
	xmlXPathFreeContext (context);
	if ((xmlobject->type == XPATH_NODESET) && (xmlobject->nodesetval))
	{
		if (xmlobject->nodesetval->nodeNr)
		{
			result = xmlobject->nodesetval->nodeNr;

			xmlNodePtr node;
			data_t<SIZE> *data;
			for (int i=0; i<xmlobject->nodesetval->nodeNr; i++) {
				node = xmlobject->nodesetval->nodeTab[i];
				data = new data_t<SIZE>();
				parseRow<SIZE> (doc, node, *data);
				vect.push_back(data);
			}
		}
	}

	xmlXPathFreeObject (xmlobject);
	xmlFreeDoc(doc);

	return (result);
}

bool XML_ATD_PWM_STUB::BeginSetup() {

	if (cosim_enabled) {
		data_t<ATD0_SIZE>* data0  = new data_t<ATD0_SIZE>();
		for (uint8_t j=0; j < ATD0_SIZE; j++) {
			data0->volte[j] = 0; // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			data0->time = 0;
		}
		atd0_vect.push_back(data0);

		data_t<ATD1_SIZE>* data1  = new data_t<ATD1_SIZE>();
		for (uint8_t j=0; j < ATD1_SIZE; j++) {
			data1->volte[j] = 0; // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
			data1->time = 0;
		}
		atd1_vect.push_back(data1);

	}
	else {
		if (atd0_xml_enabled) {
			LoadXmlData<ATD0_SIZE>(atd0_anx_stimulus_file.c_str(), atd0_vect);
		} else if (atd0_rand_enabled) {
			RandomizeData<ATD0_SIZE>(atd0_vect);
		}

		if (atd1_xml_enabled) {
			LoadXmlData<ATD1_SIZE>(atd1_anx_stimulus_file.c_str(), atd1_vect);
		} else if (atd1_rand_enabled){
			RandomizeData<ATD1_SIZE>(atd1_vect);
		}
	}

//	injection_delay = sc_time(1, SC_US);

	return (inherited::BeginSetup());
}

void XML_ATD_PWM_STUB::Inject_ATD0(double anValue[8])
{
//	data_t<ATD0_SIZE>* data = new data_t<ATD0_SIZE>();
//	atd0_vect.push_back(data);

	data_t<ATD0_SIZE>* data  = *(atd0_vect.begin());

	for (uint8_t j=0; j < ATD0_SIZE; j++) {
		data->volte[j] = anValue[j]; // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
		data->time =  0.080; // 0.080 Millisecond
	}

}

void XML_ATD_PWM_STUB::Inject_ATD1(double anValue[16])
{
//	data_t<ATD1_SIZE>* data = new data_t<ATD1_SIZE>();
//	atd1_vect.push_back(data);

	data_t<ATD1_SIZE>* data  = *(atd1_vect.begin());
	for (uint8_t j=0; j < ATD1_SIZE; j++) {
		data->volte[j] = anValue[j]; // Compute a random value: 0 Volts <= anValue[i] < 5 Volts
		data->time =  0.080; // 0.080 Millisecond
	}
}

void XML_ATD_PWM_STUB::Get_PWM(bool (*value)[PWM_SIZE])
{

	for (int i=0; i<PWM_SIZE; i++) {
		(*value)[i] = pwmValue[i];
	}
}

void XML_ATD_PWM_STUB::processATD0()
{

	/**
	 * Note: The Software sample the ATDDRx every 1024us.
	 * The injected trace file start at 20ms. The interval between two sample is 80us
	 */
	atd0_quantumkeeper.set(sc_time(20, SC_MS));
	if (atd0_quantumkeeper.need_sync()) atd0_quantumkeeper.sync();

	double atd0_anValue[ATD0_SIZE];

	uint8_t atd0_wrap_around;
	uint8_t atd0_start;

	if (atd0_anx_wrap_around_channel < ATD0_SIZE) {
		atd0_wrap_around = atd0_anx_wrap_around_channel;
	} else {
		atd0_wrap_around = ATD0_SIZE - 1;
	}

	if (atd0_anx_start_channel < ATD0_SIZE) {
		atd0_start = atd0_anx_start_channel;
	} else {
		atd0_start = 0;
	}

	while (!isTerminated() && (atd0_rand_enabled || atd0_xml_enabled || cosim_enabled)) {

		for (std::vector<data_t<ATD0_SIZE>*>::iterator it = atd0_vect.begin() ; (it != atd0_vect.end()) && !isTerminated(); ++it) {

			uint8_t j = 0;
			for (uint8_t i=0; i < ATD0_SIZE; i++) {
				if ((i < atd0_start) || (i > atd0_wrap_around)) {
					atd0_anValue[i] = 0;
				} else {
					atd0_anValue[i] = (*it)->volte[j++];
				}
			}

			output_ATD0(atd0_anValue);

//			wait(injection_delay);

		}
	}

}


void XML_ATD_PWM_STUB::processATD1()
{
	/**
	 * Note: The Software sample the ATDDRx every 1024us.
	 * The injected trace file start at 20ms. The interval between two sample is 80us
	 */
	atd1_quantumkeeper.set(sc_time(20, SC_MS));
	if (atd1_quantumkeeper.need_sync()) atd1_quantumkeeper.sync();

	double atd1_anValue[ATD1_SIZE];

	uint8_t atd1_wrap_around;
	uint8_t atd1_start;

	if (atd1_anx_wrap_around_channel < ATD1_SIZE) {
		atd1_wrap_around = atd1_anx_wrap_around_channel;
	} else {
		atd1_wrap_around = ATD1_SIZE - 1;
	}

	if (atd1_anx_start_channel < ATD1_SIZE) {
		atd1_start = atd1_anx_start_channel;
	} else {
		atd1_start = 0;
	}

	while (!isTerminated() && (atd1_rand_enabled || atd1_xml_enabled || cosim_enabled)) {

		for (std::vector<data_t<ATD1_SIZE>*>::iterator it = atd1_vect.begin() ; (it != atd1_vect.end()) && !isTerminated(); ++it) {

			uint8_t j = 0;
			for (uint8_t i=0; i < ATD1_SIZE; i++) {
				if ((i < atd1_start) || (i > atd1_wrap_around)) {
					atd1_anValue[i] = 0;
				} else {
					atd1_anValue[i] = (*it)->volte[j++];
				}
			}

			output_ATD1(atd1_anValue);

//			wait(injection_delay);

		}
	}

}

void XML_ATD_PWM_STUB::processPWM()
{

	while (!isTerminated())
	{
		input(&pwmValue);
	}

}
