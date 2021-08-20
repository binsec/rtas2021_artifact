/*
 * SocketWriter.hpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETWRITER_HPP_
#define SOCKETWRITER_HPP_


#include "unisim/service/pim/network/BlockingQueue.hpp"
#include "unisim/service/pim/network/GenericThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketWriter: public GenericThread {
public:

	SocketWriter(const int sock, bool _blocking);
	~SocketWriter();

	virtual void Run();
	virtual bool send(const char* data, bool blocking);
	virtual void stop();

protected:

private:
	int sockfd;
	BlockingQueue<char *> *buffer_queue;
	bool blocking;
};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETWRITER_HPP_ */

