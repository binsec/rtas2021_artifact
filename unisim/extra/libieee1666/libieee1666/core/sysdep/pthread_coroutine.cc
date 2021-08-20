#include "core/sysdep/pthread_coroutine.h"
#include <iostream>

namespace sc_core {

sc_pthread_coroutine::sc_pthread_coroutine()
	: thrd_mutex()
	, thrd_cond()
	, thrd_mutexm()
	, thrd_condm()
	, thrd()
	, cond(false)
	, condm(false)
	, fn(0)
	, arg(0)
{
	pthread_mutex_init(&thrd_mutex, NULL);
	pthread_cond_init(&thrd_cond, NULL);
}

sc_pthread_coroutine::sc_pthread_coroutine(std::size_t _stack_size, void (*_fn)(intptr_t), intptr_t _arg)
	: thrd_mutex()
	, thrd_cond()
	, thrd_mutexm()
	, thrd_condm()
	, thrd()
	, cond(false)
	, condm(false)
	, stack_size(_stack_size)
	, fn(_fn)
	, arg(_arg)
{
	pthread_mutex_init(&thrd_mutex, NULL);
	pthread_cond_init(&thrd_cond, NULL);

	pthread_attr_t thrd_attr;
	pthread_attr_init(&thrd_attr);
	
	if(stack_size)
	{
		pthread_attr_setstacksize(&thrd_attr, stack_size);
	}
	
	pthread_mutex_init(&thrd_mutexm, NULL);
	pthread_cond_init(&thrd_condm, NULL);
	pthread_mutex_lock(&thrd_mutexm);

	condm = false;

	pthread_create(&thrd, &thrd_attr, &sc_pthread_coroutine::entry_point, this);

	do
	{
		pthread_cond_wait(&thrd_condm, &thrd_mutexm);
	}
	while(!condm);
	
	pthread_mutex_unlock(&thrd_mutexm);

	pthread_mutex_destroy(&thrd_mutexm);
	pthread_cond_destroy(&thrd_condm);
	pthread_attr_destroy(&thrd_attr);
}

sc_pthread_coroutine::~sc_pthread_coroutine()
{
	pthread_mutex_destroy(&thrd_mutex);
	pthread_cond_destroy(&thrd_cond);
}

void *sc_pthread_coroutine::entry_point(void *_self)
{
	sc_pthread_coroutine *self = static_cast<sc_pthread_coroutine *>(_self);
	pthread_mutex_lock(&self->thrd_mutexm);
	self->condm = true;
	pthread_cond_signal(&self->thrd_condm);
	pthread_mutex_lock(&self->thrd_mutex);
	self->cond = false;
	pthread_mutex_unlock(&self->thrd_mutexm);
	do
	{
		pthread_cond_wait(&self->thrd_cond, &self->thrd_mutex);
	}
	while(!self->cond);
	pthread_mutex_unlock(&self->thrd_mutex);
	
	(*self->fn)(self->arg);
	
	return 0;
}

void sc_pthread_coroutine::yield(sc_coroutine *next_coroutine)
{
	sc_pthread_coroutine *next_pthread_coroutine = static_cast<sc_pthread_coroutine *>(next_coroutine);
	
	pthread_mutex_lock(&next_pthread_coroutine->thrd_mutex);
	next_pthread_coroutine->cond = true;
	pthread_cond_signal(&next_pthread_coroutine->thrd_cond);
	pthread_mutex_lock(&thrd_mutex);
	cond = false;
	pthread_mutex_unlock(&next_pthread_coroutine->thrd_mutex);
	do
	{
		pthread_cond_wait(&thrd_cond, &thrd_mutex);
	}
	while(!cond);
	pthread_mutex_unlock(&thrd_mutex);
}

void sc_pthread_coroutine::abort(sc_coroutine *next_coroutine)
{
	sc_pthread_coroutine *next_pthread_coroutine = static_cast<sc_pthread_coroutine *>(next_coroutine);
	
	pthread_mutex_lock(&next_pthread_coroutine->thrd_mutex);
	next_pthread_coroutine->cond = true;
	pthread_cond_signal(&next_pthread_coroutine->thrd_cond);
	pthread_mutex_unlock(&next_pthread_coroutine->thrd_mutex);
	
	pthread_exit(NULL);
}

sc_pthread_coroutine_system::sc_pthread_coroutine_system()
	: main_coroutine(0)
{
	main_coroutine = new sc_pthread_coroutine();
}

sc_pthread_coroutine_system::~sc_pthread_coroutine_system()
{
	delete main_coroutine;
}

sc_coroutine *sc_pthread_coroutine_system::get_main_coroutine()
{
	return main_coroutine;
}

sc_coroutine *sc_pthread_coroutine_system::create_coroutine(std::size_t stack_size, void (*fn)(intptr_t), intptr_t arg)
{
	return new sc_pthread_coroutine(stack_size, fn, arg);
}

} // end of namespace sc_core


