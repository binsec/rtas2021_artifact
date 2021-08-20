/*
 * SocketClientThread.cpp
 *
 *  Created on: 27 avr. 2010
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

#include "SocketClientThread.hpp"

namespace unisim {
namespace service {
namespace pim {
namespace network {

void SocketClientThread::run() {

	struct sockaddr_in serv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(hostport);
	serv_addr.sin_addr.s_addr = hostname;
	int err;
	do {
		err = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (err < 0) {
#ifdef WIN32
			Sleep(1);
#else
			usleep(1000);
#endif
		}
	} while (err < 0);


	if (!blocking) {

#ifdef WIN32

		u_long NonBlock = 1;
		if(ioctlsocket(sockfd, FIONBIO, &NonBlock) != 0) {
			int array[] = {sockfd};
			error(array, sizeof(array) / sizeof(array[0]), "ioctlsocket failed");
		}

#else

		int flags = fcntl(sockfd, F_GETFL, 0);
		if (flags < 0)	{
			int array[] = {sockfd};
			error(array, sizeof(array) / sizeof(array[0]), "fcntl failed");
		}

		if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
			int array[] = {sockfd};
			error(array, sizeof(array) / sizeof(array[0]), "fcntl failed");
		}

#endif
	}

}

} // network 
} // end pim 
} // end service 
} // end unisim 



