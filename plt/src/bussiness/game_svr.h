#ifndef _GAME_SVR_H_
#define _GAME_SVR_H_

#include "my_include.h"

using namespace wtse::log;
using namespace std;

#define UPDATE_DATA_FLAG_FILE			("../data/update_data_flag")


#define SVR_FULL_THRD					(0.95)
#define SVR_SUM_NEW_IN_RANK             (100)

#pragma pack(1)

enum SvrMergeStatus
{
    EN_SVR_MERGE_STATUS__NORMAL = 0,
    EN_SVR_MERGE_STATUS__PROTECTED,
    EN_SVR_MERGE_STATUS__DOING,
    EN_SVR_MERGE_STATUS__DONE,
};

struct SGameSvrInfo
{
	TUINT32 m_udwId;
	TCHAR	m_szName[DEFAULT_NAME_STR_LEN];
	TCHAR	m_szLanguage[DEFAULT_NAME_STR_LEN];
	TUINT32	m_udwOpenTime;
	TUINT32	m_dwStatus;

	TUINT32 m_udwPlainNum;		// 平原数目：表示空闲的plain总量，即无人占领的
	TUINT32 m_udwCityNum;		// 用户数量：表示城市总量
	TFLOAT32 m_fThrdRate;
    TUINT32 m_udwNewInChance;     // 新用户进入该服务器的概率
    TUINT32 m_udwSvrPos;          // server position
    TUINT32 m_udwSvrAvatar;       // server avatar
	TBOOL	m_bValidFlag;


	TVOID Reset()
	{
		memset((char*)this, 0, sizeof(*this));
	}
};

class CGameSvrInfo
{
public:
	CGameSvrInfo();
	~CGameSvrInfo();
	static CGameSvrInfo* m_poGameSvrInfo;
	static CGameSvrInfo* GetInstance();
	static TINT32 Update(CTseLogger *poLog);

	TINT32 Init(const TCHAR *pszGameSvrConf, CTseLogger *poLog);


public:
    static CTseLogger *m_poServLog;
	// 每个svr中用户的最大数量、现有数量、阈值
	// 格式：id\tplain_num\tplayer_num\tthrd\n
	TUINT32 m_udwSvrNum;
	SGameSvrInfo m_astSvrInfo[MAX_GAME_SVR_NUM];
	TINT32 LoadSvrInfo(const TCHAR *pszConf);

    // function ===> 获取全新用户创建用户时该导向的服号
	TUINT32 GetNewPlayerSvr();
    
	TBOOL CheckSvrStatus(TUINT32 udwSvrId);
    TUINT32 GetSvrNewChanceInBySid(TUINT32 udwSvrId);
    TCHAR* GetSvrNameBySid(TUINT32 udwSvrId);


};

#pragma pack()
#endif


