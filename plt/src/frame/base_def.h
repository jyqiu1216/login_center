#ifndef _BASE_DEF_H_
#define _BASE_DEF_H_

#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "my_include.h"

#define MAX_INS_REQ_NUM			(50)

#pragma pack(1)

typedef __uint64_t		KEY_TYPE;
typedef unsigned int	VALUE_TYPE;

enum CommandType
{
	UNKNOWN_CMD = 0,
	CMD_SET = 1,        //����һ��key��value
	CMD_ADD = 2,        //׷��
	CMD_DEL_USER = 3, //ɾ��һ��key����value
	CMD_DEL = 4,     //ɾ��valve�е�ĳЩֵ
};

enum DataType
{
	UNKOWN_DATA =0,
	UIN = 1,
	GROUP = 2
};

enum RelationDataServiceType
{
	EN_SERVICE_TYPE__USER_INS_UPDATE_REQ = 10001,
	EN_SERVICE_TYPE__USER_INS_UPDATE_RES,
	EN_SERVICE_TYPE__USER_INS_SEARCH_REQ,
	EN_SERVICE_TYPE__USER_INS_SEARCH_RES,

	EN_SERVICE_TYPE__USER_TEACH_DATA_UPDATE_REQ,
	EN_SERVICE_TYPE__USER_TEACH_DATA_UPDATE_RES,
	EN_SERVICE_TYPE__USER_TEACH_DATA_SEARCH_REQ,
	EN_SERVICE_TYPE__USER_TEACH_DATA_SEARCH_RES,
};

struct SInsUpdateData
{
	KEY_TYPE	m_uddwDocid;
	VALUE_TYPE	m_udwAction;
	TUINT32		m_udwCmd;
};

struct SInsQueryReq
{
	KEY_TYPE	m_uddwDocid;
	TUINT8		m_ucQueryType;
	TUINT8		m_ucInsType;
};

struct SInsQueryRes
{
	VALUE_TYPE	m_udwAction;
	TUINT8		m_ucQueryType;
	TUINT8		m_ucInsType;
};

enum DataUpdataKeySet
{
	EN_KEY_INFO_BEGIN = 1200,
	EN_KEY_DATA_TYPE,
	EN_KEY_CMD_TYPE,                        //��������
	EN_KEY_OWNERID,                         //�û�id
	EN_KEY_VALUE_COUNT,                     //value����
	EN_KEY_VALUE_LIST,                        //value buf
};

enum QueryKeySet
{
	E_KEY_CBU2CS_REQ_BEGIN=2600,
	E_KEY_CBU2CS_REQ_UINS=2601,         //cbu��������key buf
	E_KEY_CBU2CS_REQ_USER_ACTION_LIST=2602,	//�û����action_id;

	E_KEY_CS2CBU_RES_BEGIN=2200,
	E_KEY_CS2CBU_RES_COUNT=2201,        //����cbu�Ľ��key�ĸ���
	E_KEY_CS2CBU_RES_BUF,               //���buf
};

enum ErrorCode
{
	EN_KEY_NORMAL_ERR=-1,
};

enum BufSessionTag
{
	E_KEY_BUFSESSION_BEGIN=0,
	ONE_QUERY,
	SECOND_QUERY,
	DATA_UPDATE,
};


//��ȡ��ǰʱ��,��us����
inline TUINT64 rdtsc()
{
	struct timeval  tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime     = 0;
	gettimeofday(&tv, &tz);
	TUINT64 time = tv.tv_sec * (TUINT64)1000000 + tv.tv_usec;
	return time;
}

#pragma pack()

#endif
