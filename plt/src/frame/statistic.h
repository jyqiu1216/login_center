#ifndef TSHP_HU_STATISTIC_H
#define TSHP_HU_STATISTIC_H

#include "my_include.h"
#include "conf.h"
#include "aws_consume_info.h"
#include <map>
#include "guard_mutex.h"
using namespace std;

const TUINT32 kMaxSendMessageSize = 2560;

#pragma pack(1)
struct StatisticInfo
{
	TUINT32     search_total_count;
	TUINT32     search_succ_count;
	TUINT32     search_fail_count;
	TUINT32     search_shield_count;
	TUINT64	   aws_consume_unit;

	TUINT64     search_max_time_us;
	TUINT64     search_total_time_us;
	TUINT32     search_time_0_50_count;
	TUINT32     search_time_50_100_count;
	TUINT32     search_time_100_300_count;
	TUINT32     search_time_300_500_count;
	TUINT32     search_time_500_1000_count;
	TUINT32     search_time_1000_up_count;

	StatisticInfo()
	{
		memset(this, 0, sizeof(StatisticInfo));
	}

	void Reset()
	{
		memset(this, 0, sizeof(StatisticInfo));
	}
};
#pragma pack()

class CStatistic
{
    pthread_mutex_t m_mtxLock;
public :
	static CStatistic *Instance();
	~CStatistic();
	int Init(CTseLogger *stat_log, CConf *conf_obj);
	static void *Start(void *param);

	int Clear();
	int OutputResult();

	/*
	 * ����Ϊ�����ɹ�,��������,����ʧ��,������Ϊ���д�,����������(Ԥ��)
	 */
	int AddSearchSucc(TUINT64 search_time);
	int AddSearchFail(TUINT64 search_time);
	int AddSearchShield(TUINT64 search_time);

	int AddTimeStat(TUINT64 search_time);

	int CheckSendMessage();

	int GetStatInfo(StatisticInfo *stat_info);

	int SendMessage(const TCHAR *pszMsg);

    int AddAwsConsume(const AwsConsumeInfo& consumeInfo)
    {
        GuardMutex Guard(m_mtxLock);
        if (consumeInfo.sTableName.empty() || consumeInfo.sOperName.empty() || consumeInfo.dConsumeUnit <= 0.0)
        {
            return 0;
        }
        m_mmAwsConsume[consumeInfo.sTableName][consumeInfo.sOperName] += consumeInfo.dConsumeUnit;
        return 0;
    }

private :
	CStatistic();
	static CStatistic *m_stat_tool;

	// ��־����
	CTseLogger      *m_stat_log;
	// ͳ�Ƽ��
	TUINT32         m_stat_interval_time;
	// ͳ����Ϣ
	StatisticInfo   m_stat_info;
	// ���������Ϣ
	CConf           *m_conf;
	char            m_message[kMaxSendMessageSize];

private:
	map<string, map<string, double> > m_mmAwsConsume;
};

#endif

