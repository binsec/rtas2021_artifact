/*
 * gdbthread.hh
 *
 *  Created on: 10 févr. 2014
 *      Author: rnouacer
 */

#ifndef GDBTHREAD_HH_
#define GDBTHREAD_HH_

#include <iostream>
#include <string>

#include <unisim/kernel/kernel.hh>

#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/network/BlockingCircularQueue.hpp>

#include <unisim/util/converter/convert.hh>

using namespace std;

namespace unisim {
namespace service {
namespace pim {

using unisim::kernel::Object;

using unisim::service::pim::network::SocketThread;
using unisim::service::pim::network::BlockingCircularQueue;

class DBGData {
public:

	static const char* DEFAULT_MASTER;
	static const char* DEFAULT_MASTER_SITE;
	static const char* DEFAULT_SLAVE;
	static const char* DEFAULT_SLAVE_SITE;
	static const char* NAME_ATTR;
	static const char* VALUE_ATTR;
	static const char* REG_NUM_ATTR;
	static const char* ADDRESS_ATTR;
	static const char* SIZE_ATTR;
	static const char* TYPE_ATTR;
	static const char* FILE_NAME_ATTR;
	static const char* FILE_COLUMN_ATTR;
	static const char* FILE_LINE_ATTR;

	enum DBGCOMMANDS {DBG_KILL_COMMAND, DBG_PROCESS_EXIT, DBG_RESET_COMMAND, DBG_EXTENDED_MODE_ENABLE, DBG_SET_THREAD_CONTEXT, DBG_REPORT_STOP, DBG_OK_RESPONSE, DBG_NOK_RESPONSE, DBG_ERROR_MALFORMED_REQUEST, DBG_ERROR_READING_DATA_EPERM, DBG_UNKNOWN, /* DBG_CONTINUE */ DBG_CONTINUE_ACTION, /* DBG_SUSPEND */ DBG_SUSPEND_ACTION, DBG_VERBOSE_RESUME_ACTIONS, DBG_VERBOSE_RESUME_CONTINUE, DBG_VERBOSE_RESUME_STEP, DBG_DISCONNECT, DBG_READ_REGISTERS, DBG_WRITE_REGISTERS, DBG_STEP_CYCLE, DBG_READ_MEMORY, DBG_WRITE_MEMORY, DBG_READ_SELECTED_REGISTER, DBG_WRITE_SELECTED_REGISTER, DBG_QUERY_VARIABLE, DBG_STEP_INSTRUCTION, DBG_REMOVE_BREAKPOINT_WATCHPOINT, DBG_SET_BREAKPOINT_WATCHPOINT, DBG_READ_WATCHPOINT, DBG_WRITE_WATCHPOINT, DBG_ACCESS_WATCHPOINT, DBG_REPORT_EXTENDED_STOP, DBG_GET_LAST_SIGNAL, DBG_ENABLE_EXTENDED_MODE, TERMINATE, QUERY_DISASM, QUERY_SRCADDR, QUERY_STACK, QUERY_SYMBOLES, QUERY_PARAMETERS, QUERY_PHYSICAL_ADDRESS, QUERY_STATISTICS, QUERY_STRUCTURED_ADDRESS, QUERY_TIME, QUERY_ENDIAN, QUERY_REGISTERS, QUERY_START_PIM, QUERY_PARAMETER, QUERY_SYMBOL, QUERY_SYMBOL_ACCEPT,QUERY_SYMBOL_READ, QUERY_SYMBOL_READ_ALL, QUERY_SYMBOL_WRITE, QUERY_VAR_READ, QUERY_VAR_WRITE, QUERY_VAR_LISTEN, QUERY_VAR_UNLISTEN, UNKNOWN};


	DBGData(DBGCOMMANDS _command);
	DBGData(DBGCOMMANDS _command, double _simTime);
	DBGData(DBGCOMMANDS _command, double _simTime, string _masterSite, string _master, string _slaveSite, string _slave);
	DBGData(DBGCOMMANDS _command, double _simTime, DBGData* refData);
	~DBGData();

	DBGCOMMANDS getCommand() const;

	bool addAttribute(std::string name, std::string value);
	bool removeAttribute(std::string name);

	bool setAttribute(std::string name, std::string value);
	std::string getAttribute(std::string name);
	std::map<std::string, std::string> getAttributes();

	void setSimTime(double simTime);
	double getSimTime() const;

	void setMasterSite(string _masterSite);
	string getMasterSite() const;

	void setMaster(string _master);
	string getMaster() const;

	void setSlaveSite(string _slaveSite);
	string getSlaveSite() const;

	void setSlave(string _slave);
	string getSlave() const;

	friend std::ostream& operator << (std::ostream& os, DBGData& data);

protected:

private:
	DBGCOMMANDS command;
	double		simTime;
	string masterSite;
	string master;
	string slaveSite;
	string slave;

	std::map<std::string, std::string> attributes;

};

class GDBThread : public SocketThread {
public:

	GDBThread(const char *_name, Object *_parent = 0);
	~GDBThread();

	bool isData();
	DBGData* receiveData();
	bool sendData(DBGData* data);

	virtual void run();
	virtual void stop();
protected:

private:
	static const string SEGMENT_SEPARATOR;
	static const string FIELD_SEPARATOR;
	static const int QUEUE_SIZE = 100;


	class ReceiveThread: public GenericThread {
	public:
		ReceiveThread(GDBThread *_parent, BlockingCircularQueue<DBGData*, QUEUE_SIZE> *_dataQueue);
		~ReceiveThread();

		virtual void run();

	private:
		GDBThread *parent;
		BlockingCircularQueue<DBGData*, QUEUE_SIZE> *dataQueue;

		void HandleQRcmd(string command);

	};

	class SendThread: public GenericThread {
	public:
		SendThread(GDBThread *_parent, BlockingCircularQueue<DBGData*, QUEUE_SIZE> *_dataQueue);
		~SendThread();

		virtual void run();

	private:
		GDBThread *parent;
		BlockingCircularQueue<DBGData*, QUEUE_SIZE> *dataQueue;

	};

	string name;

	BlockingCircularQueue<DBGData*, QUEUE_SIZE> *receiveDataQueue;
	BlockingCircularQueue<DBGData*, QUEUE_SIZE> *sendDataQueue;

	ReceiveThread* receiver;
	SendThread* sender;


};


} // pim namespace
} //service namespace
} // unisim namespace

#endif /* GDBTHREAD_HH_ */
