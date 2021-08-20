/*
 *  Copyright (c) 2008, 2015
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



#include "can_stub.hh"

CAN_STUB::CAN_STUB(const sc_module_name& name, Object *parent) :
	Object(name)
	, sc_module(name)
	, can_tx_sock("can_rx_sock")
	, can_rx_sock("can_tx_sock")

	, bw_inject_count(0)
	, dont_care_bw_event(false)

	, trace_enable(false)
	, param_trace_enable("trace-enabled", this, trace_enable)

	, cosim_enabled(false)
	, param_cosim_enabled("cosim-enabled", this, cosim_enabled)

	, xml_enabled(false)
	, param_xml_enabled("xml-enabled", this, xml_enabled)

	, rand_enabled(false)
	, param_rand_enabled("rand-enabled", this, rand_enabled)

	, input_payload_queue("input_payload_queue")

	, can_inject_stimulus_period(20000)
	, param_can_inject_stimulus_period("can-rx-stimulus-period", this, can_inject_stimulus_period)
	, can_inject_stimulus_period_sc(NULL)

	, can_inject_stimulus_file("")
	, param_can_inject_stimulus_file("can-rx-stimulus-file", this, can_inject_stimulus_file)

	, broadcast_enabled(false)
	, param_broadcast_enabled("broadcast-enabled", this, broadcast_enabled)

	, terminated(false)


{
	can_tx_sock(*this);
	can_rx_sock(*this);

	SC_HAS_PROCESS(CAN_STUB);

	SC_THREAD(processCAN_Inject);
	SC_THREAD(processCAN_Observe);
	SC_THREAD(watchdog);

}

CAN_STUB::~CAN_STUB() {

	if (trace_enable) {
		can_inject_output_file.close();
		can_observe_output_file.close();
	}

	if (can_inject_stimulus_period_sc) { delete can_inject_stimulus_period_sc; can_inject_stimulus_period_sc = NULL; }
}

bool CAN_STUB::BeginSetup() {

	if (cosim_enabled) {
//		CAN_DATATYPE* data0  = new CAN_DATATYPE();
//		for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
//			data0->ID[j] = 0;
//		}
//		for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
//			data0->Data[j] = 0;
//		}
//		data0->Length = 0;
//		data0->Priority = 0;
//		data0->Extended = 0;
//		data0->Error = 0;
//		data0->Remote = 0;
//		data0->Timestamp[0] = 0;
//		data0->Timestamp[1] = 0;
//		can_rx_vect.push_back(data0);
	}
	else if (xml_enabled) {
		LoadXmlData(can_inject_stimulus_file.c_str(), can_inject_vect);
	} else if (rand_enabled) {
		RandomizeData(can_inject_vect);
	}

	if (trace_enable) {
		stringstream strm;
		strm << sc_object::name() << "_inject_output.txt";
		can_inject_output_file.open (strm.str().c_str());
		strm.str(string());
		strm << sc_object::name() << "_observe_output.txt";
		can_observe_output_file.open (strm.str().c_str());
		strm.str(string());
	}

//	can_rx_stimulus_period_sc = new sc_time(can_rx_stimulus_period, SC_US);
	can_inject_stimulus_period_sc = new sc_time(100, SC_MS);

	watchdog_delay = sc_time(1, SC_US);

	return (true);
}

bool CAN_STUB::Setup(ServiceExportBase *srv_export) {
	return (true);
}

bool CAN_STUB::EndSetup() {
	return (true);
}

void CAN_STUB::Stop(int exit_status) {
	terminated = true;
}

// Slave methods
bool CAN_STUB::get_direct_mem_ptr( CAN_Payload& payload, tlm_dmi&  dmi_data)
{
	// Leave this empty as it is designed for memory mapped buses
	return (false);
}

unsigned int CAN_STUB::transport_dbg( CAN_Payload& payload)
{
	// Leave this empty as it is designed for memory mapped buses
	return (0);
}

tlm_sync_enum CAN_STUB::nb_transport_fw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_REQ)
	{
		phase = END_REQ; // update the phase
		payload.acquire();

		input_payload_queue.notify(payload, t); // queue the payload and the associative time
		return (TLM_UPDATED);
	}

	// we received an unexpected phase, so we return TLM_ACCEPTED
	return (TLM_ACCEPTED);
}

void CAN_STUB::b_transport( CAN_Payload& payload, sc_core::sc_time& t)
{
	payload.acquire();
	input_payload_queue.notify(payload, t);
}

// Master methods
tlm_sync_enum CAN_STUB::nb_transport_bw( CAN_Payload& payload, tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == BEGIN_RESP)
	{
		//payload.release();
		bw_inject_count++;
		if (bw_inject_count == can_tx_sock.size()) {
			can_bw_event.notify();
		}

		return (TLM_COMPLETED);
	}
	return (TLM_ACCEPTED);
}

void CAN_STUB::invalidate_direct_mem_ptr( sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

// Implementation
void CAN_STUB::observe(CAN_DATATYPE &msg)
{

	CAN_Payload *payload = NULL;

	wait(input_payload_queue.get_event());

	payload = input_payload_queue.get_next_transaction();

	payload->unpack(msg);

	if (trace_enable) {
		can_observe_output_file << (sc_time_stamp().to_seconds() * 1000) << " ms \t\t" << *payload <<  std::endl;
	}

	payload->release();

	tlm_phase phase = BEGIN_RESP;
	sc_time local_time = SC_ZERO_TIME;
	can_rx_sock->nb_transport_bw( *payload, phase, local_time);

}

void CAN_STUB::inject(CAN_DATATYPE msg)
{

	CAN_Payload *can_rx_payload = can_inject_payload_fabric.allocate();

	can_rx_payload->pack(msg);

	if (trace_enable) {
		can_inject_output_file << (sc_time_stamp().to_seconds() * 1000) << " ms \t\t" << *can_rx_payload << std::endl;
		can_inject_output_file.flush();
	}

	for (int i=0; i<can_tx_sock.size(); i++) {
		sc_time local_time = SC_ZERO_TIME;

		tlm_phase phase = BEGIN_REQ;

		tlm_sync_enum ret = can_tx_sock[i]->nb_transport_fw(*can_rx_payload, phase, local_time);

		switch(ret)
		{
			case TLM_ACCEPTED:
				// neither payload, nor phase and local_time have been modified by the callee
				break;
			case TLM_UPDATED:
				// the callee may have modified 'payload', 'phase' and 'local_time'
				break;
			case TLM_COMPLETED:
				// the callee may have modified 'payload', and 'local_time' ('phase' can be ignored)
				break;
		}
	}

	dont_care_bw_event = false;
	watchdog_enable_event.notify();
	wait(can_bw_event | time_out_event);
	dont_care_bw_event = true;

	can_rx_payload->release();

}

void CAN_STUB::Inject_CAN(CAN_DATATYPE msg)
{
	if (cosim_enabled) {
		CAN_DATATYPE* data = new CAN_DATATYPE();
		can_inject_vect.push_back(data);

		for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
			data->ID[j] = msg.ID[j];
		}
		for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
			data->Data[j] = msg.Data[j];
		}
		data->Length = msg.Length;
		data->Priority = msg.Priority;
		data->Extended = msg.Extended;
		data->Error = msg.Error;
		data->Remote = msg.Remote;
		data->Timestamp[0] = msg.Timestamp[0];
		data->Timestamp[1] = msg.Timestamp[1];
	}
}

void CAN_STUB::Get_CAN(CAN_DATATYPE *msg)
{
	if (cosim_enabled && (can_observe_vect.size() > 0)) {
		std::vector<CAN_DATATYPE*>::iterator it = can_observe_vect.begin();
		for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
			msg->ID[j] = (*it)->ID[j];
		}
		for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
			msg->Data[j] = (*it)->Data[j];
		}
		msg->Length = (*it)->Length;
		msg->Priority = (*it)->Priority;
		msg->Extended = (*it)->Extended;
		msg->Error = (*it)->Error;
		msg->Remote = (*it)->Remote;
		msg->Timestamp[0] = (*it)->Timestamp[0];
		msg->Timestamp[1] = (*it)->Timestamp[1];

		delete (*it); (*it) = NULL;
		can_observe_vect.erase(it);
	}

}

void CAN_STUB::Inject_CANArray(CAN_DATATYPE_ARRAY msgArray)
{
	unsigned int nbmsg = msgArray.nmsgs;
	CAN_DATATYPE *canMsg = msgArray.canMsg;
	for (unsigned int i=0; i<nbmsg; i++)
	{
		Inject_CAN(canMsg[i]);
	}
}

void CAN_STUB::getCANArray(CAN_DATATYPE_ARRAY *msg){
	msg->nmsgs = can_observe_vect.size();
	if (can_observe_vect.size() > 0) {
		msg->canMsg = new CAN_DATATYPE[msg->nmsgs];
		unsigned int i = 0;
		for (std::vector<CAN_DATATYPE*>::iterator it = can_observe_vect.begin() ; (it != can_observe_vect.end()) ; ++it)
		{
			for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
				(msg->canMsg[i]).ID[j] = (*it)->ID[j];
			}
			for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
				(msg->canMsg[i]).Data[j] = (*it)->Data[j];
			}
			(msg->canMsg[i]).Length = (*it)->Length;
			(msg->canMsg[i]).Priority = (*it)->Priority;
			(msg->canMsg[i]).Extended = (*it)->Extended;
			(msg->canMsg[i]).Error = (*it)->Error;
			(msg->canMsg[i]).Remote = (*it)->Remote;
			(msg->canMsg[i]).Timestamp[0] = (*it)->Timestamp[0];
			(msg->canMsg[i]).Timestamp[1] = (*it)->Timestamp[1];

			i++;
		}
		for (unsigned int i=0; i<can_observe_vect.size(); i++) {
			if (can_observe_vect[i]) { delete can_observe_vect[i]; can_observe_vect[i] = NULL; }
		}
		can_observe_vect.clear();
	}
}

void CAN_STUB::watchdog() {

	while (true) {
		wait(watchdog_enable_event);

		wait(watchdog_delay);
		if (!dont_care_bw_event) {
			time_out_event.notify();
		}
	}
}

void CAN_STUB::processCAN_Inject()
{
	while (!isTerminated() && (rand_enabled || xml_enabled || cosim_enabled)) {

		wait(*can_inject_stimulus_period_sc);

		for (unsigned int i=0; i<can_inject_vect.size()  && !isTerminated(); i++) {
			if (can_inject_vect[i] != NULL) {
				inject(*(can_inject_vect[i]));
			}

			wait(*can_inject_stimulus_period_sc);
		}

		if (cosim_enabled) {
			for (unsigned int i=0; i<can_inject_vect.size(); i++) {
				if (can_inject_vect[i]) { delete can_inject_vect[i]; can_inject_vect[i] = NULL; }
			}
			can_inject_vect.clear();
		}
	}

}


void CAN_STUB::processCAN_Observe()
{

	while (!isTerminated())
	{
		CAN_DATATYPE *can_observe_buffer = new CAN_DATATYPE();
		observe(*can_observe_buffer);
/*
 * Don't automatically broadcast the CAN messages because it is point to point.
 *
 */
		if (broadcast_enabled) {
			inject(*can_observe_buffer);
		}

		if (cosim_enabled) {
			can_observe_vect.push_back(can_observe_buffer);
		} else {
			delete can_observe_buffer;
		}
	}

}

int CAN_STUB::RandomizeData(std::vector<CAN_DATATYPE* > &vect) {


	const int SET_SIZE = 1024;

	srand(12345);

	CAN_DATATYPE* data;
	for (int i=0; i < SET_SIZE; i++) {
		data = new CAN_DATATYPE();

		for (uint8_t j=0; j < CAN_ID_SIZE; j++) {
			data->ID[j] = rand();
		}

		for (uint8_t j=0; j < CAN_DATA_SIZE; j++) {
			data->Data[j] = rand();
		}
		data->Length = rand() % 9;
		data->Priority = 0;
		data->Extended = rand() % 2;
		data->Remote = rand() % 2;
		if (data->Extended == 1) {
			data->ID[1] = (data->ID[1] | CAN_Payload::IDE_MASK);
			if (data->Remote) {
				data->ID[3] = data->ID[3] | CAN_Payload::EXT_RTR_MASK;
			} else {
				data->ID[3] = data->ID[3] & ~CAN_Payload::EXT_RTR_MASK;
			}

		} else {
			data->ID[1] = (data->ID[1] & ~CAN_Payload::IDE_MASK);
			if (data->Remote) {
				data->ID[1] = data->ID[1] | CAN_Payload::STD_RTR_MASK;
			} else {
				data->ID[1] = data->ID[1] & ~CAN_Payload::STD_RTR_MASK;
			}
		}
		data->Error = 0;
		data->Timestamp[0] = 0;
		data->Timestamp[1] = 0;

		vect.push_back(data);
	}

	return (SET_SIZE);
}

void CAN_STUB::parseRow (xmlDocPtr doc, xmlNodePtr cur, CAN_DATATYPE &data) {

//	xmlChar *key;
//	vector<xmlNodePtr> rowCells;
//
//	if ((xmlStrcmp(cur->name, (const xmlChar *)"Row"))) {
//		cerr << "Error: Can't parse " << cur->name << endl;
//		return;
//	}
//
//	cur = cur->xmlChildrenNode;
//	while (cur != NULL) {
//		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Cell"))) {
//			rowCells.push_back(cur);
//		}
//		cur = cur->next;
//	}
//
//	if ((rowCells.size() < 2) || (rowCells.size() > (SIZE+1))) {
//		cerr << "Error: Wrong row size. The minimum is 2 and the maximum is " << std::dec << (SIZE+1) << " cells by row." << endl;
//		return;
//	}
//
//	for (int i=0; i < SIZE; i++) {
//		data.volte[i] = 0;
//	}
//	data.time = 0;
//
//	/**
//	 * First cells of row are ATD voltage
//	 * The last cell is time of sampling
//	 */
//	for (unsigned int i=0; (i < rowCells.size()) && (i < SIZE); i++) {
//		cur = rowCells.at(i);
//
//		xmlNodePtr node = cur->children;
//		while (xmlStrcmp(node->name, (const xmlChar *)"Data")) {
//			node = node->next;
//		}
//
//		key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
//		if (i < (rowCells.size()-1)) {
//			data.volte[i] = std::atof((const char *) key);
//		} else {
//			data.time = std::atof((const char *) key);
//		}
//
//		xmlFree(key);
//
//	}
//

	return;
}

int CAN_STUB::LoadXmlData(const char *filename, std::vector<CAN_DATATYPE* > &vect) {

//	const char *path = "//Row";
//
//	xmlDocPtr doc = NULL;
//	xmlXPathContextPtr context = NULL;
//	xmlXPathObjectPtr xmlobject;
//	int result = 0;
//
//	cout << sc_object::name() << " Parsing " << filename << endl;
//
//	doc = xmlParseFile (GetSimulator()->SearchSharedDataFile(filename).c_str());
//	if (!doc)
//	{
//		cerr << __FILE__ << ":" << __LINE__ << " Could not parse the document" << endl;
//		return (0);
//	}
//
//	xmlXPathInit ();
//	context = xmlXPathNewContext (doc);
//
//	xmlobject = xmlXPathEval ((xmlChar *) path, context);
//	xmlXPathFreeContext (context);
//	if ((xmlobject->type == XPATH_NODESET) && (xmlobject->nodesetval))
//	{
//		if (xmlobject->nodesetval->nodeNr)
//		{
//			result = xmlobject->nodesetval->nodeNr;
//
//			xmlNodePtr node;
//			data_t<SIZE> *data;
//			for (int i=0; i<xmlobject->nodesetval->nodeNr; i++) {
//				node = xmlobject->nodesetval->nodeTab[i];
//				data = new data_t<SIZE>();
//				parseRow<SIZE> (doc, node, *data);
//				vect.push_back(data);
//			}
//		}
//	}
//
//	xmlXPathFreeObject (xmlobject);
//	xmlFreeDoc(doc);
//
//	return (result);

	return 0;
}

