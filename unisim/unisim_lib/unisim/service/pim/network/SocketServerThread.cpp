/*
 * SocketThread.cpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream>

#ifdef WIN32

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

#include "SocketServerThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

SocketServerThread::SocketServerThread(string host, uint16_t port, uint8_t connection_req_nb) :
	SocketThread(host, port), protocolHandler(NULL)
{
	request_nbre = connection_req_nb;

	struct sockaddr_in serv_addr;

	primary_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (primary_sockfd < 0) {
		error("ERROR opening socket");
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(hostport);
	serv_addr.sin_addr.s_addr = hostname;
	if (bind(primary_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		int array[] = {primary_sockfd};
        error(array, sizeof(array) / sizeof(array[0]), "ERROR on binding");
	}

	/* listen()
	 * Prepare to accept connections on socket FD.
	 * N connection requests will be queued before further requests are refused.
	 * Returns 0 on success, -1 for errors.  */

	if (listen(primary_sockfd,request_nbre) < 0) {
		int array[] = {primary_sockfd};
		error(array, sizeof(array) / sizeof(array[0]), "listen failed");
	}

}

SocketServerThread::~SocketServerThread() {

#ifdef WIN32
		closesocket(primary_sockfd);
#else
		close(primary_sockfd);
#endif

}

void SocketServerThread::run() {

#ifdef WIN32
		int cli_addr_len;
#else
		socklen_t cli_addr_len;
#endif

	struct sockaddr_in cli_addr;

    cli_addr_len = sizeof(cli_addr);

	int sockfdTmp = -1;

	// client connected
	sockfdTmp = accept(primary_sockfd, (struct sockaddr *) &cli_addr, &cli_addr_len);

	if (sockfdTmp >= 0) {

		// *** This option is used to disable the Nagle TCP algorithm (disable buffering) ***
		int opt = 1;
		if (setsockopt(sockfdTmp, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt)) < 0) {
			int array[] = {sockfdTmp};
			error(array, sizeof(array) / sizeof(array[0]), "setsockopt <IPPROTO_TCP, TCP_NODELAY> failed");
		}

#ifdef WIN32

		/* Ask for non-blocking reads on socket */
		u_long NonBlock = 1;
		if(ioctlsocket(sockfdTmp, FIONBIO, &NonBlock) != 0) {
			int array[] = {sockfdTmp};
			error(array, sizeof(array) / sizeof(array[0]), "ioctlsocket <FIONBIO, NonBlock> failed");
		}

#else

		int flags = fcntl(sockfdTmp, F_GETFL, 0);
		if (flags < 0)	{
			int array[] = {sockfdTmp};
			error(array, sizeof(array) / sizeof(array[0]), "fcntl <F_GETFL> failed");
		}

		/* Ask for non-blocking reads on socket */
		if (fcntl(sockfdTmp, F_SETFL, flags | O_NONBLOCK) < 0) {
			int array[] = {sockfdTmp};
			error(array, sizeof(array) / sizeof(array[0]), "fcntl <F_SETFL, flags | O_NONBLOCK> failed");
		}

#endif

		protocolHandler->startSocketThread(sockfdTmp);
	}

}


} // network 
} // end pim 
} // end service 
} // end unisim 


