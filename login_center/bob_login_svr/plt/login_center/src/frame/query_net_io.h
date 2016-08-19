#ifndef _QUERY_NET_IO_H_
#define _QUERY_NET_IO_H_

#include "my_include.h"
#include "conf.h"
#include "session.h"
#include "time_utils.h"
#include "client_netio.pb.h"
using namespace client_netio_protocol;


class CQueryNetIO : public ITasksGroupCallBack
{
public:
	// ��ʼ��
	TINT32 Init(CConf *pobjConf, CTseLogger *pLog, CTaskQueue *poTaskQueue);

	// ����IO�߳���������
	static TVOID * RoutineNetIO(void *pParam);

	// ������Ϣ�ص�
	virtual void OnTasksFinishedCallBack(LTasksGroup *pTasksgrp);

	// ������Ϣ����
	virtual void OnUserRequest(LongConnHandle hSession, const TUCHAR *pData, TUINT32 uLen, BOOL &bWillResponse);

public:
    //  ��������socket
    SOCKET CreateListenSocket(TCHAR *pszListenHost, TUINT16 uwPort);

	// �رռ���socket
    TINT32 CloseListenSocket();

	// ��ȡip�Ͷ˿�
    TINT32 GetIp2PortByHandle(LongConnHandle stHandle, TUINT16 *puwPort, TCHAR **ppszIp);
private:
	// �ڽ�������ʱ�ͷ�������ʱ,��front���ش���
	TINT32 SendHttpBackErr(LongConnHandle stHandle, TUINT32 udwSeqno);

public:
	// �����Ӷ���
	ILongConn               *m_pLongConn;
	// ��־����
	CTseLogger				*m_pLog;
	// session����
	CTaskQueue				*m_poTaskQueue;
	// ����
	CConf					*m_poConf;
	// ���/�������
	CBaseProtocolPack       *m_pPackTool;
	CBaseProtocolUnpack     *m_pUnPackTool;
	// ����socket
	TINT32                  m_hListenSock;
	// ���к�
	TUINT32					m_udwSeqno;

    // ģʽ
    TBOOL                   m_bHttpOp;

private:
    HRESULT OnClientCommandRequest_Binary(LongConnHandle stHandle, const TUCHAR *pszData, TUINT32 udwDataLen);
    ClientRequest           *m_pobjClientReq;

private:
    HRESULT OnClientCommandRequest_Http(LongConnHandle stHandle, const TUCHAR *pszData, TUINT32 udwDataLen);
};

#endif