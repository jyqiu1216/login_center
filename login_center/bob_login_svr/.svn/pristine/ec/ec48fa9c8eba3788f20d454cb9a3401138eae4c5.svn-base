#include "game_svr.h"
#include <time.h>
CGameSvrInfo* CGameSvrInfo::m_poGameSvrInfo = NULL;
CTseLogger*	CGameSvrInfo::m_poServLog = NULL;


CGameSvrInfo* CGameSvrInfo::GetInstance()
{
	if(m_poGameSvrInfo == NULL)
	{
		m_poGameSvrInfo = new CGameSvrInfo;
	}
	return m_poGameSvrInfo;
}


CGameSvrInfo::CGameSvrInfo()
{
	m_udwSvrNum = 0;
}

CGameSvrInfo::~CGameSvrInfo()
{
	m_udwSvrNum = 0;
}

TINT32 CGameSvrInfo::Update(CTseLogger *poLog)
{
	TINT32 dwRetCode = 0;
	CGameSvrInfo *poSvrInfo = new CGameSvrInfo;
	CGameSvrInfo *poTmpSvrInfo = m_poGameSvrInfo;

	dwRetCode = poSvrInfo->Init("../data/game_svr.info", poLog);
	if(dwRetCode != 0)
	{
        TSE_LOG_ERROR(poLog, (" CGameSvrInfo::Update failed[%d]", dwRetCode));
		delete poSvrInfo;
		return -1;
	}

	m_poGameSvrInfo = poSvrInfo;

	if(poTmpSvrInfo != NULL)
	{
	    sleep(5);
		delete poTmpSvrInfo;
	}

    TSE_LOG_INFO(m_poServLog, (" CGameSvrInfo::Update ok", dwRetCode));

	return 0;
}

TINT32 CGameSvrInfo::Init( const TCHAR *pszGameSvrConf, CTseLogger *poLog )
{
	TINT32 dwRetCode = 0;

    m_poServLog = poLog;

    for (TUINT32 udwIdx = 0; udwIdx < MAX_GAME_SVR_NUM; ++udwIdx)
    {
        m_astSvrInfo[udwIdx].Reset();
    }

	// 设置随机数种子
	srand(time(NULL));


	// 加载游戏服务器统计信息，主要是地图相关
	dwRetCode = LoadSvrInfo(pszGameSvrConf);
	if(dwRetCode != 0)
	{
		return -3;
	}


	return 0;
}

TUINT32 CGameSvrInfo::GetNewPlayerSvr()
{
	TUINT32 udwNewPlayerSvr = 0;
    TUINT32 udwRank = random()%SVR_SUM_NEW_IN_RANK;

    // 获取当前所有服务器的总概率值
    TUINT32 udwTotalRank = 0;
	for(TUINT32 idx = 0; idx < m_udwSvrNum; idx++)
	{
	    udwTotalRank += m_astSvrInfo[idx].m_udwNewInChance;
	}
    
    //TUINT32 udwRank = random()%SVR_SUM_NEW_IN_RANK;
    if (0<udwTotalRank)
    {
        udwRank = random()%udwTotalRank;
    }
    
	for(TUINT32 idx = 0; idx < m_udwSvrNum; idx++)
	{

        if (udwRank<m_astSvrInfo[idx].m_udwNewInChance)
        {
			udwNewPlayerSvr = idx;
            break;
        }
        udwRank -= m_astSvrInfo[idx].m_udwNewInChance;
	}

    TSE_LOG_INFO(m_poServLog, ("ProcessCmd_LoginFake:[svr_total_num=%u total_rank=%u rank=%u new_svr_id=%u]", m_udwSvrNum, udwTotalRank, udwRank, udwNewPlayerSvr));

	return udwNewPlayerSvr;
}


TINT32 CGameSvrInfo::LoadSvrInfo(const TCHAR *pszConf)
{
	FILE *fp = fopen(pszConf, "rt");
	if(fp == NULL)
	{
        TSE_LOG_ERROR(m_poServLog, ("LoadSvrInfo: open %s failed", pszConf));
		return -1;
	}

	TINT32 dwRetCode = 0;
	TCHAR szLine[512];
	TCHAR *pCur = NULL;
	SGameSvrInfo *pstSvr = NULL;
	TUINT32 udwSvrId = 0;
	TUINT32 udwLineNum = 0;

	while(fgets(szLine, 512, fp))
	{
		udwLineNum++;
		if(strlen(szLine) < 10)
		{
            TSE_LOG_ERROR(m_poServLog, ("LoadSvrInfo: line[%u] not right", udwLineNum));
			dwRetCode = -2;
			break;
		}

		// id
		pCur = strtok(szLine, "\t\r\n");
		udwSvrId = strtoul(pCur, NULL, 10);
		if(udwSvrId >= MAX_GAME_SVR_NUM)
		{
            TSE_LOG_ERROR(m_poServLog, ("LoadSvrInfo: svrid over thrd. line[%u]", udwLineNum));
			dwRetCode = -3;
			break;
		}
		pstSvr = &m_astSvrInfo[udwSvrId];
		pstSvr->m_udwId = udwSvrId;

		// name
		pCur = strtok(NULL, "\t\r\n");
		strncpy(pstSvr->m_szName, pCur, DEFAULT_NAME_STR_LEN-1);
		pstSvr->m_szName[DEFAULT_NAME_STR_LEN-1] = 0;

		// language
		pCur = strtok(NULL, "\t\r\n");
		strncpy(pstSvr->m_szLanguage, pCur, DEFAULT_NAME_STR_LEN-1);
		pstSvr->m_szLanguage[DEFAULT_NAME_STR_LEN-1] = 0;

		// opentime
		pCur = strtok(NULL, "\t\r\n");
		pstSvr->m_udwOpenTime = strtoul(pCur, NULL, 10);

		// status
		pCur = strtok(NULL, "\t\r\n");
		pstSvr->m_dwStatus = atoi(pCur);

		// plain num
		pCur = strtok(NULL, "\t\r\n");
		pstSvr->m_udwPlainNum = strtoul(pCur, NULL, 10);

		// city num
		pCur = strtok(NULL, "\t\r\n");
		pstSvr->m_udwCityNum = strtoul(pCur, NULL, 10);

		// thrd
		pCur = strtok(NULL, "\t\r\n");
		pstSvr->m_fThrdRate = atof(pCur);

		// valid flag
        pCur = strtok(NULL, "\t\r\n");
		pstSvr->m_bValidFlag = atoi(pCur);

        // new player in ratio
        pCur = strtok(NULL, "\t\r\n");
        pstSvr->m_udwNewInChance = atoi(pCur);

        // server pos
        pCur = strtok(NULL, "\t\r\n");
        pstSvr->m_udwSvrPos = atoi(pCur);

        // server avatar
        pCur = strtok(NULL, "\t\r\n");
        pstSvr->m_udwSvrAvatar = atoi(pCur);


		// svr num
		m_udwSvrNum = m_udwSvrNum < udwSvrId+1 ? udwSvrId+1 : m_udwSvrNum;

        TSE_LOG_INFO(m_poServLog, ("LoadSvrInfo: svrid[%u] load ok", udwSvrId));
	}
	fclose(fp);
	if(dwRetCode < 0)
	{
		return dwRetCode;
	}


	return dwRetCode;
}


TBOOL CGameSvrInfo::CheckSvrStatus( TUINT32 udwSvrId )
{
	if(udwSvrId < m_udwSvrNum)
	{
		TFLOAT32 fCurRate = m_astSvrInfo[udwSvrId].m_udwCityNum*1.0/(m_astSvrInfo[udwSvrId].m_udwCityNum + m_astSvrInfo[udwSvrId].m_udwPlainNum);
		if(fCurRate < SVR_FULL_THRD)
		{
			return TRUE;
		}
	}
	return FALSE;
}



TCHAR* CGameSvrInfo::GetSvrNameBySid(TUINT32 udwSvrId)
{
    return m_astSvrInfo[udwSvrId].m_szName;
}

TUINT32 CGameSvrInfo::GetSvrNewChanceInBySid(TUINT32 udwSvrId)
{
    return m_astSvrInfo[udwSvrId].m_udwNewInChance;
}


