/*
 * pim_thread.cc
 *
 *  Created on: 20 janv. 2011
 *      Author: rnouacer
 */

#include <unisim/service/pim/pim_thread.hh>

namespace unisim {
namespace service {
namespace pim {

using namespace std;

double PIMThread::GetSimTime() {
	return Object::GetSimulator()->GetSimTime();
}

double PIMThread::GetLastTimeRatio() { return (last_time_ratio); }

bool PIMThread::UpdateTimeRatio() {
/*
	- add a time_ratio = HotsTime/SimulatedTime response
	- the time_ratio is used by timed/periodic operations
*/


	double new_time_ratio = last_time_ratio;
	double sim_time = Object::GetSimulator()->GetSimTime();
	double host_time = Object::GetSimulator()->GetHostTime();

	bool has_changed = false;

	if (sim_time > 0) {
		new_time_ratio = host_time / sim_time;
	}
	if ((sim_time == 0) || (fabs(last_time_ratio - new_time_ratio) > 0.1)) {
		pim_trace_file << (sim_time * 1000) << " \t" << (new_time_ratio) << endl;
		last_time_ratio = new_time_ratio;
		has_changed = true;
	}

	return has_changed;
}

PIMThread::PIMThread(const char *_name, Object *_parent) :
	 Object(_name, _parent)
	, SocketThread()
	, VariableBaseListener()

	, name(string(_name))
	, last_time_ratio(-1)
	, gdbThread(NULL)

{

}

PIMThread::~PIMThread() {

	if (gdbThread) {
		if (!gdbThread->isTerminated()) {
			gdbThread->stop();
		}
		delete gdbThread; gdbThread = NULL;
	}

	pim_trace_file.close();

}

void PIMThread::run(){

	gdbThread = new GDBThread("gdb-Thread");
	gdbThread->startSocketThread(getSockfd());

	pim_trace_file.open ("pim_trace.xls");


	cerr << "PIM::TargetThread start RUN " << std::endl;

	// Explore Simulator Variables to Generate PIM XML File
	vector<VariableBase*> simulator_variables;

	std::list<VariableBase *> lst;

	Simulator::Instance()->GetSignals(lst);

	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_variables.push_back((VariableBase *) *it);

	}

	lst.clear();

	while (!super::isTerminated()) {

		DBGData* request = gdbThread->receiveData();

		if (request->getCommand() == DBGData::TERMINATE) {
			gdbThread->stop();

			super::stop();
		} else {

			// qRcmd,cmd;var_name[:value]{;var_name[:value]}

			if (request->getCommand() == DBGData::QUERY_VAR_LISTEN) {

				string targetVar = request->getSlave();
				for (unsigned int i=0; i < simulator_variables.size(); i++) {
					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {
						simulator_variables[i]->AddListener(this);
						break;
					}
				}

			} else	if (request->getCommand() == DBGData::QUERY_VAR_UNLISTEN) {

				string targetVar = request->getSlave();

				for (unsigned int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						simulator_variables[i]->RemoveListener(this);

						break;
					}

				}

			} else	if (request->getCommand() == DBGData::QUERY_VAR_READ) {

				string targetVar = request->getSlave();

				for (unsigned int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						DBGData *response = new DBGData(DBGData::QUERY_VAR_READ);

						response->setSimTime(GetSimTime());

						response->setMaster(simulator_variables[i]->GetName());
						response->setMasterSite(request->getMasterSite());

						response->setSlave(simulator_variables[i]->GetName());
						response->setSlaveSite(request->getSlaveSite());

						if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {
							double val = *(simulator_variables[i]);

							response->addAttribute("value", stringify(val));

						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {
							float val = *(simulator_variables[i]);
							response->addAttribute("value", stringify(val));
						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {
							bool val = *(simulator_variables[i]);
							response->addAttribute("value", stringify(val));
						}
						else {
							uint64_t val = *(simulator_variables[i]);
							response->addAttribute("value", stringify(val));
						}

						gdbThread->sendData(response);

						break;
					}
				}

			} else if (request->getCommand() == DBGData::QUERY_VAR_WRITE) {

				string targetVar = request->getSlave();

				string value = request->getAttribute("value");

				for (unsigned int i=0; i < simulator_variables.size(); i++) {

					if (targetVar.compare(simulator_variables[i]->GetName()) == 0) {

						if (strcmp(simulator_variables[i]->GetDataTypeName(), "double precision floating-point") == 0) {

							*(simulator_variables[i]) = convertTo<double>(value);

						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "single precision floating-point") == 0) {

							*(simulator_variables[i]) = convertTo<float>(value);

						}
						else if (strcmp(simulator_variables[i]->GetDataTypeName(), "boolean") == 0) {

							*(simulator_variables[i]) = value.compare("false");
						}
						else {

							*(simulator_variables[i]) = convertTo<uint64_t>(value);

						}

						break;
					}
				}


			} else {
				cerr << "PIM-Target UNKNOWN command => " << std::endl;
			}

			if (request) { delete request; request = NULL; }

		}

	}

	cerr << "********** PIM-Target EXIT THREAD ********* " << std::endl;

}

void PIMThread::VariableBaseNotify(const VariableBase *var) {

	DBGData *response = new DBGData(DBGData::QUERY_VAR_LISTEN);

	response->setSimTime(GetSimTime());

	response->setMaster(var->GetName());
	response->setMasterSite("_who_initiate_the_listener_");
	response->setSlave(var->GetName());
	response->setSlaveSite(DBGData::DEFAULT_SLAVE_SITE);

	if (strcmp(var->GetDataTypeName(), "double precision floating-point") == 0) {
		double val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else if (strcmp(var->GetDataTypeName(), "single precision floating-point") == 0) {
		float val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else if (strcmp(var->GetDataTypeName(), "boolean") == 0) {
		bool val = *(var);
		response->addAttribute("value", stringify(val));
	}
	else {
		uint64_t val = *(var);
		response->addAttribute("value", stringify(val));
	}

	gdbThread->sendData(response);

}

} // end pim
} // end service
} // end unisim

