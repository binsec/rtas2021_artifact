/*
 * SyncExample.cpp
 *
 *  Created on: 10 févr. 2010
 *      Author: rnouacer
 */

#include "GenericThread.hpp"

#include <unistd.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#endif

namespace unisim {
namespace service {
namespace pim {
namespace network {

void TObject::error(const char* msg) {
    perror(msg);
    exit(1);
}

void TObject::error(const int* fd, int size, const char* msg) {

	for (int i=0; i<size; i++) {
#ifdef WIN32
		closesocket(fd[i]);
#else
		close(fd[i]);
#endif
	}

    perror(msg);
    exit(1);
}

void* executer(void* param) {
   TObject *obj=(TObject*)param;
   obj->run();

   return NULL;
}

} // network 
} // end pim 
} // end service 
} // end unisim 


