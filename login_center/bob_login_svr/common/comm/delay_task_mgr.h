#ifndef _DELAY_TASK_MGR_H_
#define _DELAY_TASK_MGR_H_

#include "delay_task.h"

class CDelayTaskMgr
{
public:
    static CDelayTaskMgr * Instance();

    // ≥ı ºªØ
    int Init(TUINT32 queue_size, CTseLogger *serv_log);

    int WaitTillDelayTask(SDelayTask **delay_task);

    int ReleaseDelayTask(SDelayTask *delay_task);

    int Uninit();
    ~CDelayTaskMgr();

	int GetEmptySessionSize();

private:
    CDelayTaskMgr();
    CDelayTaskMgr(const CDelayTaskMgr &);
    CDelayTaskMgr & operator =(const CDelayTaskMgr &);

    static CDelayTaskMgr *m_delay_task_mgr;

    CDelayTaskQueue *m_task_queue;
    CTseLogger *m_serv_log;
};

#endif

