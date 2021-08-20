/*
 * SocketWriter.cpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */


#include "assert.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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

#include "SocketWriter.hpp"
#include "unisim/util/converter/convert.hh"

namespace unisim {
namespace service {
namespace pim {
namespace network {

SocketWriter::SocketWriter(const int sock, bool _blocking) :
		GenericThread(),
		blocking(_blocking)
{
	assert(sock >= 0);
	sockfd = sock;
	buffer_queue = new BlockingQueue<char *>();
};

SocketWriter::~SocketWriter() {

	if (buffer_queue) { delete buffer_queue; buffer_queue = NULL; }
};

/*
 * FD_ZERO(sockfd, &write_flags)	sets all associated flags in the socket to 0
 * FD_SET (sockfd, &write_flags)	used to set a socket for checking
 * FD_CLR (sockfd, &write_flags)	used to clear a socket from being checked
 * FD_ISSET(sockfd, &write_flags)	used to query as to if the socket is ready for reading or writing.
 */

void SocketWriter::Run() {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err;
	int n;

	super::setTerminated(false);

	while (!super::isTerminated()) {
		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// If there is data in the output buffer to be sent then we
		// need to set the write flag to check the write status
		// of the socket
		if (!buffer_queue->isEmpty()) {
			FD_SET(sockfd, &write_flags);
		}

		// Now call select
		err = select(sockfd+1, &read_flags,&write_flags, (fd_set*)0,&waitd);
		if (err < 0) { // If select breaks then pause for 1 seconds
			sleep(1); // then continue
			continue;
		}

		// Now select will have modified the flag sets to tell us
		// what actions can be performed

		//Check if the socket is prepared to accept data
		if (FD_ISSET(sockfd, &write_flags)) {
			FD_CLR(sockfd, &write_flags);

			while (!buffer_queue->isEmpty() && !super::isTerminated()) {

				char *output_buffer = NULL;
				buffer_queue->next(output_buffer);

				n = write(sockfd, output_buffer, strlen(output_buffer));

				free(output_buffer);
				if (n < 0) {
					int array[] = {sockfd};
					error(array, sizeof(array) / sizeof(array[0]), "ERROR writing to socket");
				}
			}

		}

	}

}

bool SocketWriter::send(const char* data, bool blocking) {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err;

	size_t data_size = strlen(data);
	char* dd = (char *) malloc(data_size+5);
	memset(dd, 0, data_size+5);

	dd[0] = '$';
	uint8_t checksum = 0;
	size_t pos = 0;
	char c;

	while(pos < data_size)
	{
		c = data[pos];
		dd[pos+1] = c;
		checksum += (uint8_t) c;
		pos++;
	}
	dd[pos+1] = '#';

	dd[pos+2] = nibble2HexChar(checksum >> 4);
	dd[pos+3] = nibble2HexChar(checksum & 0xf);

	if (blocking) {
		int dd_size = strlen(dd);
		int index = 0;
		while (dd_size > 0) {

			waitd.tv_sec = 0;
			waitd.tv_usec = 1000;

			FD_ZERO(&read_flags); // Zero the flags ready for using
			FD_ZERO(&write_flags);

			// Set the write flag to check the write status of the socket
			FD_SET(sockfd, &write_flags);

			// Now call select
			err = select(sockfd+1, &read_flags,&write_flags, (fd_set*)0,&waitd);
			if (err < 0) {
				// If select breaks then pause for 1 milliseconds and then continue
	#ifdef WIN32
				Sleep(1);
	#else
				usleep(1000);
	#endif

				continue;
			}

			// Now select will have modified the flag sets to tell us
			// what actions can be performed

			//Check if the socket is prepared to accept data
			if (FD_ISSET(sockfd, &write_flags)) {
				FD_CLR(sockfd, &write_flags);

				int n = write(sockfd, dd+index, dd_size);

				if (n < 0) {
					int array[] = {sockfd};
					error(array, sizeof(array) / sizeof(array[0]), "ERROR writing to socket");
				} else {
					index += n;
					dd_size -= n;
				}

			} else {
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;

			}

		}

		free(dd);

	} else {
		buffer_queue->add(dd);
	}

	return true;
}

void SocketWriter::stop() {

	if (buffer_queue) {
		buffer_queue->lock();
		std::queue<char*> myqueue = buffer_queue->getQueue();
		while (!myqueue.empty())
		{
			char* data = myqueue.front();
			free(data);
			myqueue.pop();
		}

		buffer_queue->unlock();

		delete buffer_queue;
		buffer_queue = NULL;
	}

	super::stop();

}

} // network 
} // end pim 
} // end service 
} // end unisim 


