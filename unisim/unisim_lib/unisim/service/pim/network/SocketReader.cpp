/*
 * SocketReader.cpp
 *
 *  Created on: 23 avr. 2010
 *      Author: rnouacer
 */

#include "assert.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sstream>
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

#include "SocketReader.hpp"
#include "unisim/util/converter/convert.hh"

namespace unisim {
namespace service {
namespace pim {
namespace network {

SocketReader::SocketReader(const int sock, bool _blocking) :
		GenericThread(),
		blocking(_blocking),
		input_buffer_size(0),
		input_buffer_index(0),
		input_buffer(NULL)
{
	assert(sock >= 0);
	sockfd = sock;
	buffer_queue = new BlockingQueue<char *>();
	input_buffer = (char*) malloc(MAXDATASIZE+1);

};

SocketReader::~SocketReader() {

	if (buffer_queue) { delete buffer_queue; buffer_queue = NULL; }
	if (input_buffer) { free (input_buffer); input_buffer = NULL; }
};

/*
 * FD_ZERO(sockfd, &write_flags)	sets all associated flags in the socket to 0
 * FD_SET (sockfd, &write_flags)	used to set a socket for checking
 * FD_CLR (sockfd, &write_flags)	used to clear a socket from being checked
 * FD_ISSET(sockfd, &write_flags)	used to query as to if the socket is ready for reading or writing.
 */

void SocketReader::Run() {

	fd_set read_flags, write_flags;
	struct timeval waitd;
	stringstream receive_buffer;

	int err;
	int n;

	super::setTerminated(false);

	while (!super::isTerminated()) {
		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// Set the sockets read flag, so when select is called it examines
		// the read status of available data.
		FD_SET(sockfd, &read_flags);

		// Now call select
		err = select(sockfd+1, &read_flags, &write_flags, (fd_set*)0,&waitd);
		if (err < 0) { // If select breaks then pause for 1 seconds
			sleep(1); // then continue
			continue;
		}

		// Now select will have modified the flag sets to tell us
		// what actions can be performed

		// Check if data is available to read
		if (FD_ISSET(sockfd, &read_flags)) {
			FD_CLR(sockfd, &read_flags);

			memset(input_buffer, 0, MAXDATASIZE+1);

#ifdef WIN32
			n = recv(sockfd, input_buffer, MAXDATASIZE, 0);
			if (n == 0 || n == SOCKET_ERROR)
#else
			n = read(sockfd, input_buffer, MAXDATASIZE);
			if (n <= 0)
#endif
			{
		    	int array[] = {sockfd};
		    	error(array, sizeof(array) / sizeof(array[0]), "ERROR reading from socket");
		    } else if (n > 0) {
		    	receive_buffer << input_buffer;
		    }

		} else {
			receive_buffer.flush();
			string bstr = receive_buffer.str();
			receive_buffer.str("");
			while (bstr.size() > 0) {
				std::size_t pos = 0;
				switch (bstr[pos++]) {
					case '+': break;
					case '-': break;
					case '$': {
						int diese_index = bstr.find_first_of('#');
						char* buffer = (char *) malloc(diese_index);
						memset(buffer, 0, diese_index);
						memcpy(buffer, bstr.c_str()+1, diese_index-1);
						buffer_queue->add(buffer);

						pos = diese_index+3;

					} break;
					default : ;
				}

				if (pos < bstr.size()) {
					bstr = bstr.substr(pos);
				} else {
					bstr = "";
				}
			}

			bstr.clear();
		}

	}

}

void SocketReader::getChar(char& c) {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err;

	int n;

	while (input_buffer_size == 0) {
		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// Set the sockets read flag, so when select is called it examines
		// the read status of available data.
		FD_SET(sockfd, &read_flags);

		// Now call select
		err = select(sockfd+1, &read_flags, &write_flags, (fd_set*)0,&waitd);
		if (err < 0) { // If select breaks then pause for 1 seconds
			sleep(1); // then continue
			continue;
		}

		// Now select will have modified the flag sets to tell us
		// what actions can be performed

		// Check if data is available to read
		if (FD_ISSET(sockfd, &read_flags)) {
			FD_CLR(sockfd, &read_flags);

			memset(input_buffer, 0, MAXDATASIZE+1);

#ifdef WIN32
			n = recv(sockfd, input_buffer, MAXDATASIZE, 0);
			if (n == 0 || n == SOCKET_ERROR)
#else
			n = read(sockfd, input_buffer, MAXDATASIZE);
			if (n <= 0)
#endif
		    {
		    	int array[] = {sockfd};
		    	error(array, sizeof(array) / sizeof(array[0]), "ERROR reading from socket");
		    } else {
		    	input_buffer_size = n;
		    	input_buffer_index = 0;
		    }

		} else {
			if (blocking) {
				sleep(1); // then continue
				continue;
			} else {
				break;
			}
		}

	}


	if (input_buffer_size > 0) {
		c = input_buffer[input_buffer_index];
		input_buffer_size--;
		input_buffer_index++;
	} else {
		c = 0;
	}

}

char* SocketReader::receive() {

	char* str = NULL;
	string s = "";
	uint8_t checkSum = 0;
	int packet_size = 0;
	uint8_t pchk;
	char c;

	if (blocking) {

    	while (true) {
    		getChar(c);
    		if (c == 0) {
    			cerr << "receive EOF " << endl;
    			break;
    		}
        	switch(c)
        	{
        		case '+':
        			break;
        		case '-':
       				// error response => e.g. retransmission of the last packet
        			break;
        		case 3: // '\003'
        			break;
        		case '$':

        			getChar(c);
        			while (true) {
            			s = s + c;
            			packet_size++;
        				checkSum = checkSum + c;
        				getChar(c);

        				if (c == '#') break;
        			}

        			getChar(c);
        			pchk = hexChar2Nibble(c) << 4;
        			getChar(c);
        			pchk = pchk + hexChar2Nibble(c);

        			if (checkSum != pchk) {
        				cerr << "wrong checksum checkSum= " << checkSum << " pchk= " << pchk << endl;
        				return NULL;
        			} else
        			{

						str = (char *) malloc(packet_size+1);
						memset(str, 0, packet_size+1);
						memcpy(str, s.c_str(), packet_size);

						s.clear();

        				return str;
        			}

        			break;
        		default:
        			cerr << "packetParser: protocol error (0x" << nibble2HexChar(c) << ":" << c << ")";
        	}

    	}

    	return str;

	} else {
		buffer_queue->next(str);
	}

	return str;
}


void SocketReader::stop() {

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


