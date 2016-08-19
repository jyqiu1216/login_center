#ifndef _DELAY_TASK_H_
#define _DELAY_TASK_H_

#include "queue_t.h"
#include "my_include.h"

#pragma pack(1)

struct SDelayTask
{
    TUINT32 m_udwSeqNo;
    TUINT32 m_udwIdxNo;
    string m_sTableName;
    string m_sOperatorName;
    string m_sReqContent;

    TUINT32 m_udwCacheMode;
    string m_strRouteFld;
    TUINT64 m_uddwRouteThrd;

    string m_strHashKey;
    string m_strRangeKey;

    TUINT32 m_udwBeginTime;

    void Reset()
    {
        m_udwSeqNo = 0;

        m_udwIdxNo = 0;
        m_sTableName.clear();
        m_sOperatorName.clear();
        m_sReqContent.clear();

        m_udwCacheMode = 0;
        m_strRouteFld.clear();
        m_uddwRouteThrd = 0;

        m_strHashKey.clear();
        m_strRangeKey.clear();

        m_udwBeginTime = 0;
    }
};
#pragma pack()

typedef CQueueT<SDelayTask *> CDelayTaskQueue;

#endif