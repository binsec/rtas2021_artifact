/*
 * SocketThread.cpp
 *
 *  Created on: 27 avr. 2010
 *      Author: rnouacer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

#include "SocketThread.hpp"
#include "unisim/util/converter/convert.hh"

namespace unisim {
namespace service {
namespace pim {
namespace network {

uint32_t SocketThread::name_resolve(const char *host_name)
{
	struct in_addr addr;
	struct hostent *host_ent;

	if ((addr.s_addr=inet_addr(host_name))==(unsigned)-1) {
		host_ent=gethostbyname(host_name);

		if(host_ent==NULL) return(-1);

		memcpy(host_ent->h_addr, (char *)&addr.s_addr, host_ent->h_length);
	}

	return (addr.s_addr);
}

SocketThread::SocketThread(string host, uint16_t port) :
		GenericThread()
		, hostname(0)
		, hostport(0)
		, sockfd(-1)
		, input_buffer_size(0)
		, input_buffer_index(0)
		, input_buffer(NULL)

{
	hostname = name_resolve(host.c_str());
	hostport = port;

	init();

}

SocketThread::SocketThread() :
				GenericThread(),
				hostname(0),
				hostport(0),
				sockfd(-1),
				input_buffer_size(0),
				input_buffer_index(0),
				input_buffer(NULL)

{

	init();

}

void SocketThread::init() {

//	pthread_mutex_init (&sockfd_mutex, NULL);

	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init (&sockfd_mutex, &Attr);

	pthread_mutex_init (&sockfd_condition_mutex, NULL);
	pthread_cond_init (&sockfd_condition_cond, NULL);

	input_buffer = (char*) malloc(MAXDATASIZE+1);

	output_buffer_strm.str(std::string());

}

SocketThread::~SocketThread() {

    pthread_mutex_lock( &sockfd_mutex );

	if (sockfd) {
#ifdef WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
	}

    pthread_mutex_unlock( &sockfd_mutex );

    pthread_cond_destroy(&sockfd_condition_cond);
	pthread_mutex_destroy(&sockfd_condition_mutex);
    pthread_mutex_destroy( &sockfd_mutex );

    free(input_buffer);
    input_buffer = NULL;

    output_buffer_strm.str(std::string());
}

void SocketThread::startSocketThread(int sockfd) {

	setSockfd(sockfd);

	this->start();
}

void SocketThread::closeSockfd() {

#ifdef WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
		sockfd = -1;

}

void SocketThread::setSockfd(int sockfd) {

    pthread_mutex_lock( &sockfd_mutex );

	this->sockfd = sockfd;

    pthread_cond_broadcast( &sockfd_condition_cond );

    pthread_mutex_unlock( &sockfd_mutex );

}

void SocketThread::waitConnection() {

    pthread_mutex_lock( &sockfd_mutex );

	if (sockfd == -1) {

		pthread_cond_wait( &sockfd_condition_cond, &sockfd_mutex );

	}

	pthread_mutex_unlock( &sockfd_mutex );

}


bool SocketThread::PutChar(char c) {

	output_buffer_strm << c;

	return (true);
}

bool SocketThread::PutPacketWithAck(const string& data, bool Acknowledgment) {
	char c;

	int data_size = data.size();

	uint8_t checksum = 0;

	for (int pos=0; pos < data_size; pos++)
	{
		checksum += (uint8_t) data[pos];
	}

	do
	{
		output_buffer_strm.str(std::string());

		output_buffer_strm << '$' << data << '#';

		output_buffer_strm << nibble2HexChar(checksum >> 4) << nibble2HexChar(checksum & 0xf);

		if (!FlushOutput()) {
			cerr << "SocketThread unable to send !" << endl;

			return (false);
		}

	} while(!isTerminated() && Acknowledgment && (GetChar(c, true) && (c != '+')));

	return (true);

}

bool SocketThread::PutPacket(const string& data) {
	return PutPacketWithAck(data, true);
}

bool SocketThread::OutputTextWithAck(const char *s, int count, bool Acknowledgment) {
	int i;
	uint8_t packet[1 + 2 * count + 1];
	uint8_t *p = packet;

	*p = 'O';
	p++;
	for(i = 0; i < count; i++, p += 2)
	{
		p[0] = nibble2HexChar((uint8_t) s[i] >> 4);
		p[1] = nibble2HexChar((uint8_t) s[i] & 0xf);
	}
	*p = 0;
	return (PutPacketWithAck((const char *) packet, Acknowledgment));
}

bool SocketThread::OutputText(const char *s, int count)
{
	return OutputTextWithAck(s, count, true);
}

bool SocketThread::FlushOutput() {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err = 0;

	waitConnection();

	string output_buffer = output_buffer_strm.str();

	int output_buffer_size = output_buffer.size();
	int index = 0;
	while ((output_buffer_size > 0) && !isTerminated()) {

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

			int n;

#ifdef WIN32
			n = send(sockfd, &(output_buffer.at(index)), output_buffer_size, 0);
#else
			n = write(sockfd, &(output_buffer.at(index)), output_buffer_size);
#endif

			if (n <= 0) {
				int array[] = {sockfd};
				error(array, sizeof(array) / sizeof(array[0]), "ERROR writing to socket");
				break;
			} else {
				index += n;
				output_buffer_size -= n;
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

	output_buffer_strm.str(std::string());

	if (output_buffer_size > 0) {
		return (false);
	}

	return (true);
}

bool SocketThread::GetPacketWithAck(string& str, bool blocking, bool Acknowledgment) {
	str.clear();

	uint8_t checkSum = 0;
	int packet_size = 0;
	uint8_t pchk;
	char c;

	while ((true) && !isTerminated()) {

		if (!GetChar(c, blocking)) { return false; }
		if (c == 0) {
			if (blocking) {
				cerr << "receive EOF - 1" << endl;
			}
			return false;
		}
    	switch(c)
    	{
    		case '+':
    			cerr << "SocketThread:: Warning receive + " << endl;
    			break;
    		case '-':
   				// error response => e.g. retransmission of the last packet
    			cerr << "SocketThread:: Warning receive - " << endl;
    			break;
    		case 3: // '\003'
    			//break;
    			str = "DBG_SUSPEND";
    			return (true);
    		case '$':

    			if (!GetChar(c, blocking)) { return false; }
    			if (c == 0) {
    				if (blocking) {
    					cerr << "receive EOF - 2" << endl;
    				}
    				return false;
    			}
//    			while (true) {
    			while (!isTerminated()) {
    				str = str + c;
        			packet_size++;
    				checkSum = checkSum + c;

    				if (!GetChar(c, blocking)) { return false; }
    				if (c == 0) {
    					if (blocking) {
    						cerr << "receive EOF - 3" << endl;
    					}
    					return false;
    				}

    				if (c == '#') break;
    			}

    			if (!GetChar(c, blocking)) { return false; }
    			if (c == 0) {
    				if (blocking) {
    					cerr << "receive EOF - 4" << endl;
    				}
    				return false;
    			}

    			pchk = hexChar2Nibble(c) << 4;

    			if (!GetChar(c, blocking)) { return false; }
    			if (c == 0) {
    				if (blocking) {
    					cerr << "receive EOF - 5" << endl;
    				}
    				return false;
    			}

    			pchk = pchk + hexChar2Nibble(c);

				if (checkSum != pchk) {
					cerr << "SocketThread:: receive_packet: wrong checksum checkSum= " << checkSum << " pchk= " << pchk << endl;

					if (Acknowledgment) {
	    				if(!PutChar('-')) return (false);
	    				if(!FlushOutput()) return (false);
					}
    			}
    			else
    			{
					if (Acknowledgment) {
						if(!PutChar('+')) return (false);
	    				if(!FlushOutput()) return (false);
					}

    				if(str.length() >= 3 && str[2] == ':')
    				{
    					if(!PutChar(str[0])) return (false);
    					if(!PutChar(str[1])) return (false);
    					if(!FlushOutput()) return (false);
    					str.erase(0, 3);
    				}
    				return (true);
    			}

    			break;
    		default:
    			cerr << "SocketThread:: receive_packet: protocol error (0x" << nibble2HexChar(c) << ":" << c << ")";
    			break;
    	}

	}

	return (false);

}

bool SocketThread::GetPacket(string& str, bool blocking) {
	return GetPacketWithAck(str, blocking, true);
}

bool SocketThread::GetChar(char& c, bool blocking) {

	fd_set read_flags, write_flags;
	struct timeval waitd;

	int err = 0;

	int n = 0;

	waitConnection();

	while ((input_buffer_size == 0) && !isTerminated()) {
		waitd.tv_sec = 0;
		waitd.tv_usec = 1000;

		FD_ZERO(&read_flags); // Zero the flags ready for using
		FD_ZERO(&write_flags);

		// Set the sockets read flag, so when select is called it examines
		// the read status of available data.
		FD_SET(sockfd, &read_flags);

		// Now call select
		err = select(sockfd+1, &read_flags, &write_flags, (fd_set*)0,&waitd);

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

		// Check if data is available to read
		if (FD_ISSET(sockfd, &read_flags)) {

			FD_CLR(sockfd, &read_flags);

			memset(input_buffer, 0, MAXDATASIZE);

#ifdef WIN32
			n = recv(sockfd, input_buffer, MAXDATASIZE, 0);
#else
			n = read(sockfd, input_buffer, MAXDATASIZE);
#endif

			if (n <= 0)	{
		    	int array[] = {sockfd};
		    	error(array, sizeof(array) / sizeof(array[0]), "ERROR reading from socket");
		    } else {
		    	input_buffer_size = n;
		    	input_buffer_index = 0;
		    }

		} else {
			if (blocking) {
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
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

		return (true);
	} else {
		c = 0;

		return (false);
	}

}


} // network 
} // end pim 
} // end service 
} // end unisim 



