
#include "threadlock.h"

CNewThreadLock::CNewThreadLock()
{
    pthread_mutex_init(&m_mtxMutex, NULL);
}

CNewThreadLock::~CNewThreadLock()
{
    pthread_mutex_destroy(&m_mtxMutex);
}

TINT32 CNewThreadLock::lock()
{
    if (pthread_mutex_lock(&m_mtxMutex) != 0)
    {
        return -1;
    }
    return 0;
}

TINT32 CNewThreadLock::unlock()
{
    if (pthread_mutex_unlock(&m_mtxMutex) != 0)
    {
        return -1;
    }
    return 0;
}



