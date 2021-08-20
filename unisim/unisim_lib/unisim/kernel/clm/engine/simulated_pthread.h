#ifndef __MY_THREAH_H__
#define __MY_THREAH_H__
#define _PTHREAD_H  1
 
#include "plugins/pthread/pthread_types.hh"

#include <inttypes.h> 
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

// ----------------------------------------------------------------------------

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{ return syscall(SYSCALL_PTHREAD_CREATE,thread,attr,start_routine,arg);
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{ int ret = EBUSY;
  while(ret == EBUSY)
  { ret = syscall(SYSCALL_PTHREAD_MUTEX_LOCK,mutex);
  }
  return ret;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{ return syscall(SYSCALL_PTHREAD_MUTEX_UNLOCK,mutex);
}

void pthread_exit(void *value_ptr)
{ syscall(SYSCALL_PTHREAD_EXIT,value_ptr);
}

int pthread_join(pthread_t thread, void **value_ptr)
{ return syscall(SYSCALL_PTHREAD_JOIN,thread,value_ptr);
}

pthread_t pthread_self(void)
{ return syscall(SYSCALL_PTHREAD_SELF);
}

int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{ return syscall(SYSCALL_PTHREAD_MUTEX_INIT,mutex,attr);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{ return syscall(SYSCALL_PTHREAD_MUTEX_DESTROY,mutex);
}

#endif
