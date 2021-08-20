/*
 * SyncExample.h
 *
 *  Created on: 10 févr. 2010
 *      Author: rnouacer
 */

#ifndef GENERIC_THREAD_HPP_
#define GENERIC_THREAD_HPP_

#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<pthread.h>
#include <signal.h>
#include <errno.h>

#define MAXDATASIZE		255

namespace unisim {
namespace service {
namespace pim {
namespace network {


using namespace std;

void* executer(void* param);

class TObject
{
public:

	virtual ~TObject() { }

	/**
	 *  This method has to be overloaded.
	 *  It implement the behavior of the thread
	 */
	virtual void run(){}
	virtual void error(const char* msg);
	virtual void error(const int* fd, int size, const char* msg);

};

class GenericThread : public TObject
{
public:
#ifdef WIN32
	static const int kill_signo = SIGTERM;
#else
	static const int kill_signo = SIGKILL;
#endif

	GenericThread() : thid(), ret(0), terminated(false), started(false) {}

	virtual ~GenericThread() {
		if (!isTerminated()) stop();
		if (started && !isTerminated()) {

			stop();

			pthread_detach(thid);
			/**
			 *  If pthread_kill(thid, 0) return 0 the thread is still running - if not equal to 0 the thread has already terminated.
			 */
			if (pthread_kill(thid, 0) == 0) pthread_kill(thid, kill_signo);
		}
	}

	virtual void start() { ret=pthread_create(&thid,NULL,executer,(void*)this); terminated = false; started = true; }
	virtual void join() { pthread_join(thid,NULL); }
	virtual void stop() { terminated = true; started = false; }
	virtual bool isTerminated() { return (terminated); }
	virtual void setTerminated(bool b) { terminated = b; }
	virtual bool isStarted() { return (started); }
	virtual void setStarted(bool b) { started = b; }

protected:
	typedef GenericThread super;

private:
	pthread_t thid;
	int ret;
	bool terminated;
	bool started;
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* GENERIC_THREAD_H_ */

