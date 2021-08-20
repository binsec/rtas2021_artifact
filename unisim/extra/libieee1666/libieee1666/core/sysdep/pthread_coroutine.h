#ifndef __LIBIEEE1666_CORE_SYSDEP_PTHREAD_COROUTINE_H__
#define __LIBIEEE1666_CORE_SYSDEP_PTHREAD_COROUTINE_H__

#include "core/coroutine.h"
#include <pthread.h>

namespace sc_core {
	
class sc_pthread_coroutine : public sc_coroutine
{
public:
	sc_pthread_coroutine();
	sc_pthread_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg);
	virtual ~sc_pthread_coroutine();
	
	virtual void yield(sc_coroutine *next_coroutine);
	virtual void abort(sc_coroutine *next_coroutine);
private:
	pthread_mutex_t thrd_mutex;
	pthread_cond_t thrd_cond;
	pthread_mutex_t thrd_mutexm;
	pthread_cond_t thrd_condm;
	pthread_t thrd;
	bool cond;
	bool condm;
	std::size_t stack_size;
	void (*fn)(intptr_t);
	intptr_t arg;

	static void *entry_point(void *self);
};

class sc_pthread_coroutine_system : public sc_coroutine_system
{
public:
	sc_pthread_coroutine_system();
	virtual ~sc_pthread_coroutine_system();
	
	virtual sc_coroutine *get_main_coroutine();
	virtual sc_coroutine *create_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg);
private:
	sc_pthread_coroutine *main_coroutine;
};

} // end of namespace

#endif
