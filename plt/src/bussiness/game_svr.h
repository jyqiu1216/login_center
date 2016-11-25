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

	TUINT32 m_udwPlainNum;		// ƽԭ��Ŀ����ʾ���е�plain������������ռ���
	TUINT32 m_udwCityNum;		// �û���������ʾ��������
	TFLOAT32 m_fThrdRate;
    TUINT32 m_udwNewInChance;     // ���û�����÷������ĸ���
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
	// ÿ��svr���û������������������������ֵ
	// ��ʽ��id\tplain_num\tplayer_num\tthrd\n
	TUINT32 m_udwSvrNum;
	SGameSvrInfo m_astSvrInfo[MAX_GAME_SVR_NUM];
	TINT32 LoadSvrInfo(const TCHAR *pszConf);

    // function ===> ��ȡȫ���û������û�ʱ�õ���ķ���
	TUINT32 GetNewPlayerSvr();
    
	TBOOL CheckSvrStatus(TUINT32 udwSvrId);
    TUINT32 GetSvrNewChanceInBySid(TUINT32 udwSvrId);
    TCHAR* GetSvrNameBySid(TUINT32 udwSvrId);


};

#pragma pack()
#endif


