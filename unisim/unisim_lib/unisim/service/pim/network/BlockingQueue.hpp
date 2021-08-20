/*
 * BlockingQueue.hpp
 *
 *  Created on: 26 avr. 2010
 *      Author: rnouacer
 */

#ifndef BLOCKINGQUEUE_HPP_
#define BLOCKINGQUEUE_HPP_

#include<pthread.h>
#include <queue>

namespace unisim {
namespace service {
namespace pim {
namespace network {

template <class T>
class BlockingQueue {
public:

	BlockingQueue() {
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

	}

	~BlockingQueue() {

		alive = false;
		while (!buffer_queue.empty())
		{
			buffer_queue.pop();
		}

		// wake-up waiting thread
		pthread_cond_signal( &condition_cond );
	};

	void lock() { pthread_mutex_lock( &queue_mutex ); };
	void unlock() { pthread_mutex_unlock( &queue_mutex ); };

	void add(T data) {

		if (!alive) return;

	    pthread_mutex_lock( &condition_mutex );

	    pthread_mutex_lock( &queue_mutex );
	    buffer_queue.push(data);
	    pthread_mutex_unlock( &queue_mutex );

	    pthread_cond_signal( &condition_cond );

	    pthread_mutex_unlock( &condition_mutex );

	}

	void next(T& data) {

		pthread_mutex_lock( &condition_mutex );
		while( isEmpty() && alive)
		{
			pthread_cond_wait( &condition_cond, &condition_mutex );
		}

		if (alive) {
		    pthread_mutex_lock( &queue_mutex );
			data = buffer_queue.front();
			buffer_queue.pop();
		    pthread_mutex_unlock( &queue_mutex );

		    pthread_cond_signal( &condition_cond );

		}

		pthread_mutex_unlock( &condition_mutex );

	}

	bool isEmpty() {

		bool result = false;

	    pthread_mutex_lock( &queue_mutex );
	    result = buffer_queue.empty();
	    pthread_mutex_unlock( &queue_mutex );

	    return result;
	}

	size_t size() {

		size_t size = 0;

	    pthread_mutex_lock( &queue_mutex );
	    size = buffer_queue.size();
	    pthread_mutex_unlock( &queue_mutex );

	    return size;
	}

	std::queue<T> getQueue() { return buffer_queue; }
	
protected:

	pthread_mutex_t queue_mutex;
	pthread_mutex_t condition_mutex;
	pthread_cond_t  condition_cond;

	pthread_mutexattr_t queue_mutex_Attr;
	pthread_mutexattr_t condition_mutex_Attr;

private:
	std::queue<T> buffer_queue;
	bool alive;


};


} // network 
} // end pim 
} // end service 
} // end unisim 

#endif /* BLOCKINGQUEUE_HPP_ */

