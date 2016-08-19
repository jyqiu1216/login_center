#include "procedure_base.h"
#include "my_include.h"
#include "global_serv.h"
#include "time_utils.h"

string CBaseProcedure::GetTableRawName(const string& strTableName)
{
    string strRawTableName = "";

    std::size_t begin = strTableName.find("account_");
    if (begin != std::string::npos)
    {
        strRawTableName = strTableName.substr(begin + strlen("account_"));
        return strRawTableName;
    }
    return strRawTableName;
}


TINT32 CBaseProcedure::SendAwsRequest(SSession *pstSession, TUINT16 uwReqServiceType)
{
	TUCHAR *pszPack = NULL;
	TUINT32 udwPackLen = 0;
	CDownMgr *pobjDownMgr = CDownMgr::Instance();
	LTasksGroup	stTasksGroup;
	vector<AwsReqInfo*>& vecReq = pstSession->m_vecAwsReq;

	if(vecReq.size() == 0)
	{
		TSE_LOG_ERROR(pstSession->m_poServLog, ("SendAwsRequest: m_vecAwsReq.size() == 0"));
		return -1;
	}

	pstSession->m_uddwDownRqstTimeBeg = CTimeUtils::GetCurTimeUs();

	// 1. 获取下游——对于一个session来说，每次只需获取一次
	if(pstSession->m_bAwsProxyNodeExist == FALSE)
	{
		pstSession->m_pstAwsProxyNode = NULL;

		if(S_OK == pobjDownMgr->zk_GetNode(3U, &pstSession->m_pstAwsProxyNode))
		{
			pstSession->m_bAwsProxyNodeExist = TRUE;
			TSE_LOG_DEBUG(pstSession->m_poServLog, ("Get AwsProx node succ [seq=%u]", pstSession->m_udwSeqNo));
		}
		else
		{
			TSE_LOG_ERROR(pstSession->m_poServLog, ("Get AwsProxy node fail [seq=%u]", pstSession->m_udwSeqNo));
			return -2;
		}
	}

    if(NULL == pstSession->m_pstAwsProxyNode->m_stDownHandle.handle)
    {
        TSE_LOG_ERROR(pstSession->m_poServLog, ("SendAwsRequest:handle [handlevalue=0x%p] [seq=%u]", \
            pstSession->m_pstAwsProxyNode->m_stDownHandle.handle, pstSession->m_udwSeqNo));
        return -3;
    }

	// 2. 封装请求、打包、设置task
	for (unsigned int i = 0; i < vecReq.size(); ++i)
	{
        if (vecReq[i]->sOperatorName == "UpdateItem" 
            || vecReq[i]->sOperatorName == "DeleteItem" 
            || vecReq[i]->sOperatorName == "PutItem")
        {
            pstSession->m_udwDownRqstType = (pstSession->m_udwDownRqstType == 0 || pstSession->m_udwDownRqstType == 2) ? 2 : 3;
        }
        else
        {
            pstSession->m_udwDownRqstType = (pstSession->m_udwDownRqstType == 0 || pstSession->m_udwDownRqstType == 1) ? 1 : 3;
        }
		CBaseProtocolPack* pPack = pstSession->m_ppPackTool[i];
		pPack->ResetContent();
		pPack->SetServiceType(uwReqServiceType);
		pPack->SetSeq(CGlobalServ::GenerateHsReqSeq());
		pPack->SetKey(EN_GLOBAL_KEY__REQ_SEQ, pstSession->m_udwSeqNo);
		pPack->SetKey(EN_GLOBAL_KEY__INDEX_NO, vecReq[i]->udwIdxNo);
		pPack->SetKey(EN_GLOBAL_KEY__TABLE_NAME, (unsigned char*)vecReq[i]->sTableName.c_str(), vecReq[i]->sTableName.size());
		pPack->SetKey(EN_GLOBAL_KEY__OPERATOR_NAME, (unsigned char*)vecReq[i]->sOperatorName.c_str(), vecReq[i]->sOperatorName.size());

		pPack->SetKey(EN_GLOBAL_KEY__AWSTBL_CACHE_MODE, vecReq[i]->m_udwCacheMode);
		pPack->SetKey(EN_GLOBAL_KEY__AWSTBL_ROUTE_FLD, (unsigned char*)vecReq[i]->m_strRouteFld.c_str(), vecReq[i]->m_strRouteFld.size() );
		pPack->SetKey(EN_GLOBAL_KEY__AWSTBL_ROUTE_THRD, vecReq[i]->m_uddwRouteThrd);		
		pPack->SetKey(EN_GLOBAL_KEY__AWSTBL_HASH_KEY, (unsigned char*)vecReq[i]->m_strHashKey.c_str(),vecReq[i]->m_strHashKey.size());	
		if( !vecReq[i]->m_strRangeKey.empty())
        {
            pPack->SetKey(EN_GOLBAL_KEY__AWSTBL_RANGE_KEY, (unsigned char*)vecReq[i]->m_strRangeKey.c_str(),vecReq[i]->m_strRangeKey.size());	
        }      
			

        pPack->SetKey(EN_KEY_HU2HS__REQ_BUF, (unsigned char*)vecReq[i]->sReqContent.c_str(), vecReq[i]->sReqContent.size());
		pPack->GetPackage(&pszPack, &udwPackLen);

		// c. 设置task
		stTasksGroup.m_UserData1.ptr = pstSession;
        
        stTasksGroup.m_Tasks[i].SetConnSession(pstSession->m_pstAwsProxyNode->m_stDownHandle);
		stTasksGroup.m_Tasks[i].SetSendData(pszPack, udwPackLen);
		stTasksGroup.m_Tasks[i].SetNeedResponse(1);
	}
	stTasksGroup.SetValidTasks(vecReq.size());
	stTasksGroup.m_uTaskTimeOutVal = DOWN_SEARCH_NODE_TIMEOUT_MS;

    pstSession->ResetAwsInfo();

	// 3. 发送数据
	if (!pstSession->m_poLongConn->SendData(&stTasksGroup))
	{
		TSE_LOG_ERROR(pstSession->m_poServLog, ("SendGlobalHsRequest send data failed! [seq=%u]", pstSession->m_udwSeqNo));
		return -4;
	}

	return 0;
}

TINT32 CBaseProcedure::SendLoginGetRequest( SSession *pstSession )
{
    CDownMgr *pobjDownMgr = CDownMgr::Instance();
    // 1. 获取下游——对于一个session来说，每次只需获取一次
    if(pstSession->m_bHuNodeExist == FALSE)
    {
        pstSession->m_pstHuNode = NULL;

        if(S_OK == pobjDownMgr->zk_GetNode(25U, &pstSession->m_pstHuNode))
        {
            pstSession->m_bHuNodeExist = TRUE;
            TSE_LOG_DEBUG(pstSession->m_poServLog, ("SendLoginGetRequest: Get Hu node succ [seq=%u]", pstSession->m_udwSeqNo));
        }
        else
        {
            TSE_LOG_ERROR(pstSession->m_poServLog, ("SendLoginGetRequest: Get Hu node fail [seq=%u]", pstSession->m_udwSeqNo));
            return -2;
        }
    }

    if(NULL == pstSession->m_pstHuNode->m_stDownHandle.handle)
    {
        TSE_LOG_ERROR(pstSession->m_poServLog, ("SendLoginGetRequest: handle [handlevalue=0x%p] [seq=%u]", \
            pstSession->m_pstHuNode->m_stDownHandle.handle, pstSession->m_udwSeqNo));
        return -3;
    }

    // 2. 
    string req_url = GetReqUrl_LoginGet(pstSession);
    string req_ser;
    ClientRequest *client_req = new ClientRequest;
    client_req->set_service_type(EN_SERVICE_TYPE__CLIENT__COMMAND_REQ);
    client_req->set_seq(pstSession->m_udwClientSeqNo);
    client_req->set_time(1);
    client_req->set_uid(pstSession->m_stReqParam.m_ddwUserId);
    client_req->set_sid(pstSession->m_stReqParam.m_dwSvrId);
    client_req->set_aid(0);
    client_req->set_req_url(req_url.c_str());
    bool bRet = client_req->SerializeToString(&req_ser);

    // 3.
    TUCHAR *pszPack = NULL;
    TUINT32 udwPackLen = 0;
    CBaseProtocolPack* pPack = pstSession->m_ppPackTool[0];
    pPack->ResetContent();
    pPack->SetServiceType(EN_SERVICE_TYPE__CLIENT__COMMAND_REQ);
    pPack->SetSeq(CGlobalServ::GenerateHsReqSeq());
    pPack->SetKey(EN_GLOBAL_KEY__REQ_BUF, (unsigned char*)req_ser.c_str(), req_ser.size());
    pPack->GetPackage(&pszPack, &udwPackLen);

    // 4.
    LTasksGroup	stTasksGroup;
    stTasksGroup.m_UserData1.ptr = pstSession;
    stTasksGroup.m_Tasks[0].SetConnSession(pstSession->m_pstHuNode->m_stDownHandle);
    stTasksGroup.m_Tasks[0].SetSendData(pszPack, udwPackLen);
    stTasksGroup.m_Tasks[0].SetNeedResponse(1);
    stTasksGroup.SetValidTasks(1);
    stTasksGroup.m_uTaskTimeOutVal = DOWN_SEARCH_NODE_TIMEOUT_MS;

    // 5.
    if (!pstSession->m_poLongConn->SendData(&stTasksGroup))
    {
        TSE_LOG_ERROR(pstSession->m_poServLog, ("SendLoginGetRequest: send data failed! [seq=%u]", pstSession->m_udwSeqNo));
        return -4;
    }

    return 0;
}

string CBaseProcedure::GetReqUrl_LoginGet( SSession *pstSession )
{
    string strHuUrl;
    TCHAR szTmp[2048];
    TCHAR *pBeg = &pstSession->m_stReqParam.m_szReqUrl[0];
    TCHAR *pEnd = strstr(pBeg, "&sid=");
    assert(pEnd);
    strncpy(szTmp, pBeg, pEnd-pBeg);
    szTmp[pEnd-pBeg] = 0;
    strHuUrl.append(szTmp);
    sprintf(szTmp, "&op_en_flag=0&sid=%d&uid=%ld", pstSession->m_stReqParam.m_dwSvrId, pstSession->m_stReqParam.m_ddwUserId);
    strHuUrl.append(szTmp);

    pBeg = strstr(pEnd, "&rid=");
    pEnd = strstr(pBeg, "command=");
    strncpy(szTmp, pBeg, pEnd-pBeg);
    szTmp[pEnd-pBeg] = 0;
    strHuUrl.append(szTmp);
    strHuUrl.append("command=login_get");

    TSE_LOG_INFO(pstSession->m_poServLog, ("GetReqUrl_LoginGet: url=%s [seq=%u]", strHuUrl.c_str(), pstSession->m_udwSeqNo));

    return strHuUrl;
}
