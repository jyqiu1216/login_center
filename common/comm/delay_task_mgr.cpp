#include "delay_task_mgr.h"

CDelayTaskMgr * CDelayTaskMgr::m_delay_task_mgr = NULL;

CDelayTaskMgr::CDelayTaskMgr()
{
}

CDelayTaskMgr::~CDelayTaskMgr()
{
}

CDelayTaskMgr * CDelayTaskMgr::Instance()
{
    if (NULL == m_delay_task_mgr)
    {
        m_delay_task_mgr = new CDelayTaskMgr();
    }

    return m_delay_task_mgr;
}

int CDelayTaskMgr::Init(TUINT32 queue_size, CTseLogger *serv_log)
{
    if (0 == queue_size || NULL == serv_log)
    {
        return -1;
    }

    SDelayTask *delay_task_list = new SDelayTask[queue_size];
    m_serv_log = serv_log;

    m_task_queue = new CDelayTaskQueue();
    if (m_task_queue->Init(queue_size + 1) != 0)
    {
        return -2;
    }
    for (TUINT32 i = 0; i < queue_size; ++i)
    {
        delay_task_list[i].Reset();
        m_task_queue->WaitTillPush(&(delay_task_list[i]));
    }
    TSE_LOG_INFO(m_serv_log, ("CDelayTaskMgr init succ"));

    return 0;
}

int CDelayTaskMgr::WaitTillDelayTask(SDelayTask **delay_task)
{
    if (0 != m_task_queue->WaitTillPop(*delay_task))
    {
        TSE_LOG_ERROR(m_serv_log, ("CDelayTaskMgr wait till delay_task fail"));
        return -1;
    }
    TSE_LOG_DEBUG(m_serv_log, ("CDelayTaskMgr wait till pop delay_task succ, task_size=%u", m_task_queue->Size()));

    return 0;
}

int CDelayTaskMgr::ReleaseDelayTask(SDelayTask *delay_task)
{
    if (NULL == delay_task)
	{
		return 0;
	}

    delay_task->Reset();

    if (NULL == m_task_queue || 0 != m_task_queue->WaitTillPush(delay_task))
    {
        TSE_LOG_ERROR(m_serv_log, ("CDelayTaskMgr release delay_task[%p] fail", delay_task));
        return -1;
    }
    TSE_LOG_DEBUG(m_serv_log, ("CDelayTaskMgr release delay_task[%p] succ", delay_task));

    return 0;
}

int CDelayTaskMgr::Uninit()
{
    // do nothing
    return 0;
}

int CDelayTaskMgr::GetEmptySessionSize()
{
	return m_task_queue->Size();
}


