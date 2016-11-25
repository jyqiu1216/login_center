#ifndef _SSC_SESSION_MGR_H_
#define _SSC_SESSION_MGR_H_

#include "my_include.h"
#include "session.h"

/*
 * Session������
 */
class CSessionMgr
{
public:
    // ��ȡsession����������
    static CSessionMgr * Instance();

    // ��ʼ��
    int Init(TUINT32 queue_size, CTseLogger *serv_log);

    // ��ȡsession
    int WaitTillSession(SSession **session);

    // �ͷ�session
    int ReleaseSession(SSession *session);

    int Uninit();
    ~CSessionMgr();

	int GetEmptySessionSize();

private:
    CSessionMgr();
    CSessionMgr(const CSessionMgr &);
    CSessionMgr & operator =(const CSessionMgr &);

    static CSessionMgr *m_session_mgr;

    CTaskQueue  *m_task_queue;
    CTseLogger  *m_serv_log;
};

#endif

