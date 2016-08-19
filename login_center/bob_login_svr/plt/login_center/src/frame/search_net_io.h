#ifndef _SEARCH_NET_IO_H_
#define _SEARCH_NET_IO_H_

#include "my_include.h"
#include "aws_rsp_info.h"
#include "conf.h"
#include "session.h"
#include "time_utils.h"

class CSearchNetIO : public ITasksGroupCallBack
{
public:
	// 初始化
	TINT32 Init( CConf *pobjConf, CTseLogger *pLog, CTseLogger *pRegLog, CTaskQueue *poTaskQueue );

	// 网络IO线程驱动函数
	static TVOID * RoutineNetIO(void *pParam);

	// 网络消息回调
	virtual void OnTasksFinishedCallBack(LTasksGroup *pTasksgrp);

	// 网络消息请求
	virtual void OnUserRequest(LongConnHandle hSession, const TUCHAR *pData, TUINT32 uLen, BOOL &bWillResponse);

private:
	// 处理注册请求
	HRESULT OnRequestRegRequest(LongConnHandle stHandle, TUINT32 udwReqSeqNum, const TUCHAR *pszData, TUINT32 udwDataLen);

	// 处理响应消息
	TINT32  OnAwsResponse(LTasksGroup *pstTasksGrp, SSession *poSession);
	TINT32	ParseAwsResponse(TUCHAR *pszPack, TUINT32 udwPackLen, AwsRspInfo* pAwsRspInfo);

    TINT32 OnHuResponse(LTasksGroup *pstTasksGrp, SSession *poSession);
    TINT32 ParseHuResponse(TUCHAR *pszPack, TUINT32 udwPackLen, SSession *poSession);


private:
	//  创建监听socket
	SOCKET CreateListenSocket(TCHAR *pszListenHost, TUINT16 uwPort);

	// 关闭监听socket
	TINT32 CloseListenSocket();

	// 获取ip和端口
	TINT32 GetIp2PortByHandle(LongConnHandle stHandle, TUINT16 *puwPort, TCHAR **ppszIp);

public:
	// 长连接对象
	ILongConn               *m_pLongConn;
	// 日志对象
	CTseLogger				*m_poServLog;
	CTseLogger				*m_poRegLog;
	// session队列
	CTaskQueue				*m_poTaskQueue;
	// 配置
	CConf					*m_poConf;
	// 打包/解包工具
	CBaseProtocolPack       *m_pPackTool;
	CBaseProtocolUnpack     *m_pUnPackTool;
	// 监听socket
	TINT32                  m_hListenSock;
};

#endif