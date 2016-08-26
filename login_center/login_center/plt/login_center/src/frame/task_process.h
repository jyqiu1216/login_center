#ifndef _SSC_TASK_PROCESS_H_
#define _SSC_TASK_PROCESS_H_

#include "my_include.h"
#include "des.h"
#include "util_base64.h"
#include "process_base.h"
#include "json_result.h"

class CTaskProcess
{
public:
    // ��ʼ��
    TINT32 Init(CConf *poConf, ILongConn *poSearchLongConn, ILongConn *poQueryLongConn,
        CTaskQueue *pTaskQueue, CTseLogger *poServLog, CTseLogger *poDbLog);
    // �߳���ں���
    static void *Start(void *pParam);
    // ���ԴӶ������ȡ����
    TINT32 WorkRoutine();

public:
    // function  ===>����client������(����ʱ��Ҫ��֤session���ͷźͿͻ��˵ķ���)
    TINT32 ProcessClientRequest(SSession *pstSession);


    // function  ===> normal/special���̵�ѡȡ
    TINT32 ProcessCommand(SSession *pstSession);


    // function  ===> normal�����ִ����������(��Ҫ��֤session���ͷźͿͻ��˵ķ���)
    // in_value  ===> pstSession: ҵ��session
    //           ===> pBaseCmd: �����ֵĴ�����Ļ���,ָ�������������
    TVOID NormalMainProdure(SSession *pstSession, const TINT32 dwCmdType);


    // function  ===> special�����ִ����������(��Ҫ��֤session���ͷźͿͻ��˵ķ���)
    // in_value  ===> pstSession: ҵ��session
    //           ===> pBaseCmd: �����ֵĴ�����Ļ���,ָ�������������
    TVOID SpecialMainProdure(SSession *pstSession, const TINT32 dwCmdType);

private:
    TINT32 SendBackResult(SSession *pstSession);
    TINT32 SendBackResult_Http(SSession *pstSession);
    TINT32 SendBackResult_Binary(SSession *pstSession);


    TVOID ResetSessionTmpParam(SSession *pstSession);
    TVOID PrintLog(SSession *pstSession);
    TVOID GetStatistics(SSession *pstSession);


private:
    /*****************************************
    ���ܽ������
    *****************************************/
    bool EncryptUrl(char *pszIn, char *pszOut, int &dwOutLen, float fVersion);
    bool DecryptUrl(char *pszIn, char *pszOut, int &dwOutLen, float fVersion);
    CDes				*m_pobjEncrypt;
    TCHAR				m_szEncryptBuf[MAX_HTTP_REQ_LEN];
    TCHAR				m_szEncryptUrl[MAX_HTTP_REQ_LEN];



public:
    // �����ļ�����
    CConf               *m_poConf;
    ILongConn           *m_poSearchLongConn;
    ILongConn			*m_poQueryLongConn;

    // ��־����
    CTseLogger          *m_poServLog;
    CTseLogger			*m_poDayLog;

    // �������
    CTaskQueue          *m_pTaskQueue;

    // ���/�����
    CBaseProtocolPack   *m_pPackTool[MAX_AWS_REQ_TASK_NUM];
    CBaseProtocolUnpack *m_pUnPackTool;

    // hs������Ͱ���
    TUCHAR m_szReqBuf[MAX_HTTP_REQ_LEN];

    TUCHAR              m_szTmpBuf[MAX_NETIO_PACKAGE_BUF_LEN];

private:
    CJsonResult m_oJsonResult;

private:
    //�м�ʹ�õı���////////////////////////////////////
    // http��k-v��
    RequestParam		m_stHttpParam;
    TCHAR				*m_pHttpResBuf;



};

#endif
