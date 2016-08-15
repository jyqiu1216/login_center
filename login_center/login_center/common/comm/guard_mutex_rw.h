#ifndef _GUARD_MUTEX_RW_H_
#define _GUARD_MUTEX_RW_H_
#include <pthread.h>

#define GUARD_READ 0
#define GUARD_WRITE 1
class GuardMutexRW
{
    pthread_rwlock_t& m_rw_lock;
    int m_flag;
public:
    GuardMutexRW(pthread_rwlock_t& mtxLock, int flag) :m_rw_lock(mtxLock), m_flag(flag)
	{
        if (m_flag == GUARD_WRITE)
        {
            pthread_rwlock_wrlock(&m_rw_lock);
        }
        else if (m_flag == GUARD_READ)
        {
            pthread_rwlock_rdlock(&m_rw_lock);
        }
        else
        {
            assert(0);
        }
	}
    ~GuardMutexRW()
	{
        pthread_rwlock_unlock(&m_rw_lock);
	}
};


#endif 

