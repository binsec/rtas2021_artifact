/*
 * BlockingCircularQueue.hpp
 *
 *  Created on: 06 fev. 2014
 *      Author: rnouacer
 */

#ifndef _BLOCKINGCIRCULARQUEUE_HPP__
#define _BLOCKINGCIRCULARQUEUE_HPP__

#include<pthread.h>
#include<iostream>
#include<cstdlib>
#include <cmath>

namespace unisim {
namespace service {
namespace pim {
namespace network {

template <class T, unsigned int MAX_SIZE=100>
class BlockingCircularQueue {
public:

	BlockingCircularQueue() {

//		pthread_mutex_init (&queue_mutex, NULL);
//		pthread_mutex_init (&condition_mutex, NULL);
//		pthread_cond_init (&condition_cond, NULL);

		pthread_mutexattr_init(&queue_mutex_Attr);
		pthread_mutexattr_settype(&queue_mutex_Attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init (&queue_mutex, &queue_mutex_Attr);

		pthread_mutexattr_init(&condition_mutex_Attr);
		pthread_mutexattr_settype(&condition_mutex_Attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init (&condition_mutex, &condition_mutex_Attr);

		pthread_cond_init (&condition_cond, NULL);

		alive = true;
	    head = 0;
		tail = 0;
		length = 0;
	}

	~BlockingCircularQueue() {

		alive = false;

		// wake-up waiting thread
		pthread_cond_signal( &condition_cond );
	};

	void lock() { pthread_mutex_lock( &queue_mutex ); };
	void unlock() { pthread_mutex_unlock( &queue_mutex ); };

	bool add(T data) {

		if (!alive) { return (false); }

	    pthread_mutex_lock( &condition_mutex );

		while( isFull() && alive)
		{
			pthread_cond_wait( &condition_cond, &condition_mutex );
		}

		if (!alive) { return (false); }

	    pthread_mutex_lock( &queue_mutex );

	    item[tail] = data;
	    length++;
	    tail = (tail+1) % MAX_SIZE;

	    pthread_mutex_unlock( &queue_mutex );

	    pthread_cond_signal( &condition_cond );

	    pthread_mutex_unlock( &condition_mutex );

	    return (true);
	}

	bool next(T& temp) {

		if (!alive) { return (false); }

		pthread_mutex_lock( &condition_mutex );
		while( isEmpty() && alive)
		{
			pthread_cond_wait( &condition_cond, &condition_mutex );
		}

		if (!alive) { return (false); }

	    pthread_mutex_lock( &queue_mutex );

	    temp = item[head];
	    length--;
	    head = (head+1) % MAX_SIZE;

	    pthread_mutex_unlock( &queue_mutex );

	    pthread_cond_signal( &condition_cond );

		pthread_mutex_unlock( &condition_mutex );

	    return (true);
	}

	bool isEmpty() {

		bool result = false;

	    pthread_mutex_lock( &queue_mutex );

	    result = (length == 0);

	    pthread_mutex_unlock( &queue_mutex );

	    return (result);
	}

	bool isFull() {

		bool result = false;

	    pthread_mutex_lock( &queue_mutex );

	    result = (length == MAX_SIZE);

	    pthread_mutex_unlock( &queue_mutex );

	    return (result);
	}

	int size() {

		size_t size = 0;

	    pthread_mutex_lock( &queue_mutex );

	    size = length;

	    pthread_mutex_unlock( &queue_mutex );

	    return (size);
	}

	T* getElements() {

		T* tmp_queue = (T) malloc(MAX_SIZE * sizeof(T));

	    int i;
	    int j=0;
        for(i=head; i!=tail; i=(i+1)%MAX_SIZE){
        	tmp_queue[j++] = item[i];;
        }

        return (tmp_queue);
	}

protected:

	pthread_mutex_t queue_mutex;
	pthread_mutex_t condition_mutex;
	pthread_cond_t  condition_cond;

	pthread_mutexattr_t queue_mutex_Attr;
	pthread_mutexattr_t condition_mutex_Attr;

private:
	T item[MAX_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int length;

	bool alive;

	T* getQueue() { return (item); }

};


} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* _BLOCKINGCIRCULARQUEUE_HPP__ */

