#ifndef _MY_DEFINE_H_
#define _MY_DEFINE_H_

#define MAX_HTTP_REQ_LEN	(4096)
#define MAX_NETIO_PACKAGE_BUF_LEN (1<<20)
//���ĳ�����session����
#define	MAX_NETIO_LONGCONN_SESSION_NUM	(1024)
//���������õ�ͨ���շ���ʱʱ�䣬��λms
#define MAX_NETIO_LONGCONN_TIMEOUT_MS	(5000)
#define DEFAULT_NAME_STR_LEN (64)
#define MAX_REQ_TASK_NUM (10)
#define MAX_IP_LEN (32)
#define DEFAULT_SVR_LIST_STR_LEN	(512)
#define MAX_DEVICE_ID_LEN (64)
#define	MAX_REQ_PARAM_KEY_NUM (20)
#define DEFAULT_PARAM_STR_LEN (10240)
#define MAX_AWS_REQ_TASK_NUM (5)
#define DOWN_SEARCH_NODE_TIMEOUT_MS	(10000)


#define	MAX_GAME_SVR_NUM (100)
#define MAX_PROVINCE_NUM (16)
#define MAX_PRODUCT_NUM (200)
#define MAX_MAX_NUM (5)


// bob��r_pid
//const string kstrRPid = "2";    //online
const string kstrRPid = "1";    //test

enum EClientReqMode
{
    EN_CLIENT_REQ_MODE__HTTP = 0,
    EN_CLIENT_REQ_MODE__TCP,
};



enum ELoginType
{
    EN_LOGIN_TPYE_VISTOR,
    EN_LOGIN_TPYE_ACCOUNT,
    EN_LOGIN_TPYE_DEBUG,
};


enum EPlayerStatus
{
    EN_PLAYER_STATUS_ERROR = -2,
    EN_PLAYER_STATUS_IMPORT = -1,
    EN_PLAYER_STATUS_UNREGISTER = 0,
    EN_PLAYER_STATUS_ACCOUNT_UNACTIVE = 1,
    EN_PLAYER_STATUS_ACCOUNT_ACTIVE = 2,
    EN_PLAYER_STATUS_PASSWORD_CHANGE = 3,
    EN_PLAYER_STATUS_MULTI_LOGIN = 4,                   
    EN_PLAYER_STATUS_BLACK_ACCOUNT = 5,                 
    EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD = 6,          
    EN_PLAYER_STATUS_ACCOUNT_NOT_GAME_DATA = 7,         
    EN_PLAYER_STATUS_MULTI_EMAIL = 8,                   
    EN_PLAYER_STATUS_EMAIL_HAS_MULTI_GAME_DATA = 9,     
    EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD = 10,      
    EN_PLAYER_STATUS_USER_DATA_NOT_EXIST = 11,      
};



enum EServiceType
{
    EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ = 3003,
    EN_SERVICE_TYPE_QUERY_DYNAMODB_RSP = 3103,

    EN_SERVICE_TYPE__CLIENT__COMMAND_REQ = 2003,    // command
    EN_SERVICE_TYPE__CLIENT__COMMAND_RSP = 2004,

    EN_SERVICE_TYPE__CLIENT__LOGIN_REQ = 2005,      // ��¼����
    EN_SERVICE_TYPE__CLIENT__LOGIN_RSP = 2006,

};

enum EServiceKey
{
    // GLOBAL KEY
    EN_GLOBAL_KEY__BEGIN = 0,
    EN_GLOBAL_KEY__RES_CODE,
    EN_GLOBAL_KEY__RES_COST_TIME,
    EN_GLOBAL_KEY__RES_TOTAL_RES_NUM,
    EN_GLOBAL_KEY__RES_CUR_RES_NUM,
    EN_GLOBAL_KEY__RES_BUF, // any response
    EN_GLOBAL_KEY__REQ_SEQ,

    EN_GLOBAL_KEY__INDEX_NO,
    EN_GLOBAL_KEY__TABLE_NAME,
    EN_GLOBAL_KEY__OPERATOR_NAME,
    EN_GLOBAL_KEY__DB_IDX,
    EN_GLOBAL_KEY__AWSTBL_CACHE_MODE,
    EN_GLOBAL_KEY__AWSTBL_ROUTE_FLD,
    EN_GLOBAL_KEY__AWSTBL_ROUTE_THRD,
    EN_GLOBAL_KEY__AWSTBL_HASH_KEY,
    EN_GOLBAL_KEY__AWSTBL_RANGE_KEY,


	//
    EN_GLOBAL_KEY__REQ_BUF = 16,
    EN_GLOBAL_KEY__REQ_TYPE = 17,// 0: string 1: bjson
    EN_GLOBAL_KEY__RES_TYPE = 18,


    EN_MAP_SVR_KEY__NEED_LOCK = 20,
    EN_MAP_SVR_KEY__NEED_UNLOCK,
    EN_MAP_SVR_KEY__OPERATE,
    EN_MAP_SVR_KEY__SVR_ID,
    EN_MAP_SVR_KEY__REQ_BUF, // ���ݲ�ͬ�������н���

    EN_KEY_HU2HS__REQ_BUF = 5002, // to aws_proxy


	// GlobalKEY ---add
    EN_GLOBAL_KEY__TARGET_UID = 20000,
    EN_GLOBAL_KEY__TARGET_TYPE, //enum
    EN_GLOBAL_KEY__TARGET_LIST_SIZE, //UINT32
    EN_GLOBAL_KEY__TARGET_LIST, //id(uint32) list
    EN_GLOBAL_KEY__EXCEPT_TYPE, //enum
    EN_GLOBAL_KEY__EXCEPT_LIST_SIZE,
    EN_GLOBAL_KEY__EXCEPT_LIST, //id(uint32) list
    EN_GLOBAL_KEY__RES_BUF_COMPRESS_FLAG, //flag(int8)
    EN_GLOBAL_KEY__LINKER_CMD_REF, //INT
    EN_GLOBAL_KEY__CLIENT_IP, //INT
};



enum EProcedure
{
    // ������ʼ״̬
    EN_PROCEDURE__INIT = 0,

    EN_PROCEDURE__CLIENT_REQUEST,

    EN_PROCEDURE__TASK_PROCESS,

    EN_PROCEDURE__SEND_RESULT_BACK,

    EN_PROCEDURE__SEND_ERROR,

    EN_EXPECT_PROCEDURE__AWS,

    EN_EXPECT_PROCEDURE__HU,

    // ���̽������
    EN_PROCEDURE__END,
};



enum ECommandStep
{
    // ������ʼ״̬
    EN_COMMAND_STEP__INIT = 0,

    EN_COMMAND_STEP__1 = 1,
    EN_COMMAND_STEP__2,
    EN_COMMAND_STEP__3,
    EN_COMMAND_STEP__4,
    EN_COMMAND_STEP__5,
    EN_COMMAND_STEP__6,
    EN_COMMAND_STEP__7,
    EN_COMMAND_STEP__8,
    EN_COMMAND_STEP__9,
    EN_COMMAND_STEP__10,


    // ���̽������
    EN_COMMAND_STEP__END,
};

enum ERetCode
{
    EN_RET_CODE__SUCCESS = 0,

    EN_RET_CODE__PROCESS_ERROR = 41001,
    EN_RET_CODE__PARAM_ERROR = 41002,
    EN_RET_CODE__SEND_FAIL = 41003,
    EN_RET_CODE__TIMEOUT = 41004,
    EN_RET_CODE__PACKAGE_LEN_OVERFLOW = 41005,
    EN_RET_CODE__PARSE_AWS_PACKAGE_ERR = 41006,
    EN_RET_CODE__PARSE_HU_PACKAGE_ERR = 41007,

    EN_RET_CODE__POS_NOT_AVAILABLE = 41010,
    EN_RET_CODE__GET_LOCK_FAIL = 41011,

    EN_RET_CODE__STATUS_ERROR = 42001,
    EN_RET_CODE__PASSWORD_CHANGE = 42004,
    EN_RET_CODE__MULTI_LOGIN = 42003,
    EN_RET_CODE__BLACK_ACCOUNT = 42005,
    EN_RET_CODE__ACCOUNT_INFO_INVAILD = 42006,
    EN_RET_CODE__ACCOUNT_NOT_GAME_DATA = 42007,
    EN_RET_CODE__AMULTI_EMAIL = 42008,
    EN_RET_CODE__EMAIL_HAS_MULTI_GAME_DATA = 42009,
    EN_RET_CODE__PRODUCTION_INFO_INVAILD = 42010,
    EN_RET_CODE__USER_DATA_NOT_EXIST = 42011,

    EN_RET_CODE_ERROR_SID = 42020,
    EN_RET_CODE_ERROR_UID = 42021,
    EN_RET_CODE_FATAL_ERROR = 42022,
    EN_RET_CODE__STATICFILE_ERROR = 42023,

    EN_RET_CODE__END,
};

#endif