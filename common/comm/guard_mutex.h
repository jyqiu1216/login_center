#ifndef _GUARD_MUTEX_H_
#define _GUARD_MUTEX_H_
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

class GuardMutex
{
	pthread_mutex_t& m_mtxLock;
public:
	GuardMutex(pthread_mutex_t& mtxLock):m_mtxLock(mtxLock)
	{
		pthread_mutex_lock(&m_mtxLock);
	}
	~GuardMutex()
	{
		pthread_mutex_unlock(&m_mtxLock);
	}
};


#endif 

