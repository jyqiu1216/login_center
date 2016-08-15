
#ifndef _NEW_THREADLOCK_H_
#define _NEW_THREADLOCK_H_

#include "my_include.h"

class CNewThreadLock
{
private:
    pthread_mutex_t m_mtxMutex;

public:
    CNewThreadLock();
    ~CNewThreadLock();
    TINT32 lock();
    TINT32 unlock();

};

#endif //_THREADLOCK_H_
