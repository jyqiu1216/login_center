#ifndef _SSC_SESSION_H_
#define _SSC_SESSION_H_

#include <vector>
#include <map>
#include "my_include.h"
#include "queue_t.h"
#include "my_define.h"
#include "key_value.h"
#include "http_utils.h"
#include "aws_request.h"
#include "aws_response.h"
#include "conf.h"
#include "aws_table_include.h"

using namespace std;

#pragma pack(1)

struct SLoginInfo
{
    string m_strRid;
    string m_strEmail;
    string m_strPasswd;
    string m_strThId;
    string m_strDevice;
    string m_strRPid;
    string m_strProductInfo;
    
    

    TVOID Reset()
    {
        m_strRid = "";
        m_strEmail = "";
        m_strPasswd = "";
        m_strThId = "";
        m_strDevice = "";
        m_strRPid = "";
        m_strProductInfo = "";
    }

    SLoginInfo()
    {
        Reset();
    }
};

struct SUserStatus
{
    TINT64 m_ddwUid;
    TINT64 m_ddwSid;
    TINT64 m_ddwStatus;

    TVOID Reset()
    {
        m_ddwUid = 0;
        m_ddwSid = 0;
        m_ddwStatus = 0;
    }

    SUserStatus()
    {
        Reset();
    }

};



struct SProductIndex
{
    TINT64 m_dwUid;
    TUINT32 m_udwIndex;

    TVOID Reset()
    {
        m_dwUid = 0;
        m_udwIndex = 0;
    }

    SProductIndex()
    {
        Reset();
    }
};





/*
 *  结果状态
 */
enum EResult
{
    // 结果状态初始值
    EN_RESULT__INIT                 = 0,
    // 结果可信
    EN_RESULT__TRUST,
    // 结果不可信
    EN_RESULT__UNTRUST,
    // 结果失败
    EN_RESULT__FAIL,
};

struct SReqParam
{
    TINT32              m_dwOpEncryptFlag;

    // req url
    TCHAR               m_szReqUrl[MAX_HTTP_REQ_LEN];

    // common param
    TCHAR               m_szIp[MAX_IP_LEN];
    TUINT64             m_uddwDeviceId;                     // device id
    TCHAR				m_szDevice[MAX_DEVICE_ID_LEN];
    TINT32              m_dwSvrId;                         // svr id
    TINT64              m_ddwUserId;                        // user global id
    TUINT32             m_udwSeqNo;
    TCHAR               m_szVs[DEFAULT_NAME_STR_LEN];
    TCHAR               m_szUpdateVs[DEFAULT_NAME_STR_LEN];
    TCHAR               m_szPid[DEFAULT_NAME_STR_LEN];
    TCHAR               m_szRid[DEFAULT_NAME_STR_LEN];
    TCHAR               m_szPlatForm[DEFAULT_NAME_STR_LEN];
    TUINT32				m_udwLang;		                    //用户的语言


    TCHAR               m_szCommand[DEFAULT_NAME_STR_LEN];
    TUINT32             m_udwCommandID;
    TCHAR               m_szKey[MAX_REQ_PARAM_KEY_NUM][DEFAULT_PARAM_STR_LEN];
    TCHAR               m_szExKey[MAX_REQ_PARAM_KEY_NUM][DEFAULT_PARAM_STR_LEN];




    void Reset()
    {
        m_dwOpEncryptFlag = 0;
        m_szReqUrl[0] = 0;

        m_szIp[0] = 0;
        m_uddwDeviceId = 0;
        m_dwSvrId = (TUINT32)-1;
        m_ddwUserId = 0;

        m_udwSeqNo = 0;
        m_szVs[0] = '\0';
        m_szUpdateVs[0] = 0;
        m_szPid[0] = '\0';
        m_szRid[0] = '\0';
        m_szPlatForm[0] = '\0';
        m_udwLang = 0;

        m_szCommand[0] = 0;
        m_udwCommandID = 0;
        for(TUINT32 udwIdx = 0; udwIdx < MAX_REQ_PARAM_KEY_NUM; udwIdx++)
        {
            m_szKey[udwIdx][0] = '\0';
            m_szExKey[udwIdx][0] = '\0';
        }
    }
};

struct SCommonResInfo
{
    TINT32              m_dwRetCode;
    TUINT64             m_uddwCostTime;
    vector<string>      m_vecResultStaticFileList;

    
    SCommonResInfo()
    {
        Reset();
    }

    void Reset()
    {
        m_dwRetCode = EN_RET_CODE__SUCCESS;
        m_uddwCostTime = 0;
        m_vecResultStaticFileList.clear();
    }

};

struct SUserInfo
{
    // seq号,与外层session的seq号一致,外层seq用于框架,userinfo层用于业务逻辑
    TUINT32 m_udwBSeqNo;

    TINT32 m_dwProductNum;
    TbProduct m_tbProduct[MAX_PRODUCT_NUM];
    
    TbUser m_tbUser[MAX_MAX_NUM];


    TINT32 m_dwRidProductNum;
    TbProduct m_tbRidProduct;
    TINT32 m_dwDeviceProductNum;
    TbProduct m_atbDeviceProduct[MAX_PRODUCT_NUM];
    TINT32 m_dwUserNum;
    TbUser m_tbUserNew;

    TINT32 m_dwLoginTpye;  // 用户登录形式
    SUserStatus m_stUserStatus;

    SUserInfo()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_udwBSeqNo = 0;
        
        m_dwRidProductNum = 0;
        m_tbRidProduct.Reset();

        m_dwDeviceProductNum = 0;
        for (TUINT32 udwIdx = 0; udwIdx < MAX_PRODUCT_NUM; ++udwIdx)
        {
            m_atbDeviceProduct[udwIdx].Reset();
        }

        m_dwUserNum = 0;
        m_tbUserNew.Reset();

        m_dwLoginTpye = EN_LOGIN_TPYE_VISTOR;
        m_stUserStatus.Reset();
    }
};

struct SSession
{
    /********************************************************************************************
                        外部链接信息和序列号信息
    *********************************************************************************************/
    TUINT32             m_udwSeqNo;
    TUINT32             m_udwClientSeqNo;
    LongConnHandle      m_stClientHandle;
    TUINT32             m_udwExpectProcedure;
    TUINT32             m_udwNextProcedure;
    TUINT32             m_udwWaitFinishProcedure;
    TUINT32             m_udwCommand;
    TUINT32             m_udwCommandStep;
    TUINT32             m_udwProcessSeq; // 处理流程序号

    /********************************************************************************************
                        session本身状态
    *********************************************************************************************/
    TUINT8              m_ucIsUsing;

    /********************************************************************************************
                        当前使用的下游节点
    *********************************************************************************************/
    SDownNode            *m_pstAwsProxyNode;
    TBOOL               m_bAwsProxyNodeExist;

    SDownNode           *m_pstHuNode;
    TBOOL               m_bHuNodeExist;



    /********************************************************************************************
                        网络消息内存
    *********************************************************************************************/
    TUCHAR              m_szClientReqBuf[MAX_HTTP_REQ_LEN];                 // client请求Buf
    TUINT32             m_udwClientReqBufLen;                               // client请求Buf长度
    TINT32              m_dwClientReqMode;
    TUINT32             m_udwReqTime;

    TCHAR               m_szRspBuf[MAX_NETIO_PACKAGE_BUF_LEN];
    TINT32              m_dwRspLen;
    TINT32              m_dwHuRetCode;
    TINT32              m_dwHuCostTime;
    TUINT8              m_ucHuCompressFlag;

    /********************************************************************************************
                        内部保留信息
    *********************************************************************************************/
    SReqParam           m_stReqParam;   // 请求参数
    SCommonResInfo      m_stCommonResInfo;  // 结果相关
    SUserInfo           m_stUserInfo;	// 用户信息结构体
    


    /********************************************************************************************
                        借用的task_process变量
    *********************************************************************************************/
    CConf               *m_poConf;
    CTseLogger          *m_poServLog;
    CTseLogger          *m_poDbLog;
    ILongConn           *m_poLongConn;
    CBaseProtocolPack   **m_ppPackTool;
    RequestParam        *m_pstHttpParam;
    TUCHAR              *m_pszReqBuf;
    TUINT32             m_udwReqBufLen;
    TUCHAR*             m_pTmpBuf;
    TINT32              m_dwTmpLen;

    /********************************************************************************************
                        时间参数
    *********************************************************************************************/
    TUINT64             m_uddwTimeBeg;     // 收到请求的时间
    TUINT64             m_uddwTimeEnd;     // 发送前端回应的时间
    TUINT64             m_uddwDownRqstTimeBeg;
    TUINT64             m_uddwDownRqstTimeEnd;

    //aws info
    vector<AwsReqInfo*> m_vecAwsReq;
    vector<AwsRspInfo*> m_vecAwsRsp;


    TUINT64 m_uddwAwsReadSumTime;
    TUINT64 m_uddwAwsWriteSumTime;
    TUINT64 m_uddwAwsReadWriteSumTime;
    TUINT64 m_uddwAwsNoOpSumTime;
    TUINT32 m_udwDownRqstType; //1:read 2:write 3:read&write

    TBOOL m_bGotoOtherCmd;

    void ResetAwsReq()
    {
        for (unsigned int i = 0; i < m_vecAwsReq.size(); ++i)
        {
            delete m_vecAwsReq[i];
        }
        m_vecAwsReq.clear();
    }
    void ResetAwsRsp()
    {
        for (unsigned int i = 0; i < m_vecAwsRsp.size(); ++i)
        {
            delete m_vecAwsRsp[i];
        }
        m_vecAwsRsp.clear();
    }
    
    void ResetAwsInfo()
    {
        ResetAwsReq();
        ResetAwsRsp();
    }

    void Reset()
    {
        m_udwSeqNo = 0;
        m_udwClientSeqNo = 0;
        memset((char*)&m_stClientHandle, 0, sizeof(LongConnHandle));
        m_udwExpectProcedure = EN_PROCEDURE__INIT;
        m_udwNextProcedure = EN_PROCEDURE__INIT;
        m_udwWaitFinishProcedure = 0; //nemo
        m_udwCommandStep = EN_COMMAND_STEP__INIT;
        m_udwProcessSeq = 0;

        m_ucIsUsing = 0;

        m_bAwsProxyNodeExist = FALSE;
        m_pstAwsProxyNode = NULL;
        m_pstHuNode = NULL;
        m_bHuNodeExist = FALSE;

        m_szClientReqBuf[0] = 0;
        m_udwClientReqBufLen = 0;
        m_udwReqTime = 0;

        m_dwRspLen = 0;
        m_ucHuCompressFlag = 0;
        m_dwHuRetCode = 0;
        m_dwHuCostTime = 0;

        m_stReqParam.Reset();
        m_stCommonResInfo.Reset();
        m_stUserInfo.Reset();

 

        m_uddwTimeBeg = 0;
        m_uddwTimeEnd = 0;
        
        // task process 变量
        m_poConf = NULL;
        m_poServLog = NULL;
        m_poDbLog = NULL;
        m_poLongConn = NULL;
        m_ppPackTool = NULL;
        //m_pPackTool = NULL;
        m_pstHttpParam = NULL;
        m_pszReqBuf = NULL;
        m_udwReqBufLen = 0;
        m_pTmpBuf = NULL;
        m_dwTmpLen = 0;


        //aws info
        ResetAwsInfo();


        m_uddwAwsReadSumTime = 0;
        m_uddwAwsWriteSumTime = 0;
        m_uddwAwsReadWriteSumTime = 0;
        m_uddwAwsNoOpSumTime = 0;


        m_udwDownRqstType = 0;
        m_bGotoOtherCmd = FALSE;
    }
};

#pragma pack()

typedef CQueueT<SSession *> CTaskQueue;

#endif

