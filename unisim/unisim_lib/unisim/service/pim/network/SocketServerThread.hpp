/*
 * SocketThread.hpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#ifndef SOCKETSERVERTHREAD_HPP_
#define SOCKETSERVERTHREAD_HPP_


#include "SocketThread.hpp"
#include <vector>

namespace unisim {
namespace service {
namespace pim {
namespace network {

class SocketServerThread: public SocketThread {
public:

	SocketServerThread(string host, uint16_t port, uint8_t connection_req_nb);
	~SocketServerThread();

	virtual void run();

	void setProtocolHandler(SocketThread* protocolHandler) { this->protocolHandler = protocolHandler; }

private:
	int primary_sockfd;

	uint8_t request_nbre;
	SocketThread* protocolHandler;

};

} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* SOCKETSERVERTHREAD_HPP_ */
