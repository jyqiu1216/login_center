#include "json_result.h"
#include "rc4.h"
#include "zlib.h"
#include "hu_work.h"
#include "time_utils.h"
#include "static_file_mgr.h"
#include "game_command.h"
#include "global_serv.h"
#include "my_include.h"

CJsonResult::CJsonResult()
{
    m_szResultBuffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];


    // 原始数据和压缩数据
    m_pSourceBuffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];
    m_pCompressBuffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];
    m_pEncodeBuffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];
    poEncrypt = CEncryptCR4Mgr::get_instance()->GetEncrypt(1.0f);
    m_pszMapData = new TCHAR[500 * 1200];

}

CJsonResult::~CJsonResult()
{
    if(m_szResultBuffer != NULL)
    {
        delete[] m_szResultBuffer;
        m_szResultBuffer = NULL;
    }
}

TUINT32 CJsonResult::GetResultJsonLength() const
{
    return m_dwResultLength;
}

TCHAR* CJsonResult::GenResult(SSession* pstSession)
{
    m_szResultBuffer[0] = '\0';
    TUINT32 udwLengthAppend = 0;
    TCHAR* pCurPos = m_szResultBuffer;
    TCHAR* pEndPos = pCurPos + MAX_NETIO_PACKAGE_BUF_LEN;

    udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos, "{");
    pCurPos += udwLengthAppend;

    udwLengthAppend = GenResHeader(pstSession, pCurPos, pEndPos);
    pCurPos += udwLengthAppend;

    udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos, ",");
    pCurPos += udwLengthAppend;

    udwLengthAppend = GenResData(pstSession, pCurPos, pEndPos);
    pCurPos += udwLengthAppend;

    udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos, "}");
    pCurPos += udwLengthAppend;

    m_dwResultLength = pCurPos - m_szResultBuffer;
    return m_szResultBuffer;
}


TINT32 CJsonResult::GenResHeader(SSession* pstSession, TCHAR* pBeginPos, TCHAR* pEndPos)
{
    TUINT32 udwLengthAppend = 0;
    TCHAR* pCurPos = pBeginPos;
    string strModule = "";
    if (EN_CLIENT_REQ_COMMAND__INIT == pstSession->m_stReqParam.m_udwCommandID)
    {
        strModule = "init";
    }
    else if (EN_CLIENT_REQ_COMMAND__UPDATE == pstSession->m_stReqParam.m_udwCommandID)
    {
        strModule = "update";
    }
    else if (EN_CLIENT_REQ_COMMAND__GETDATA == pstSession->m_stReqParam.m_udwCommandID)
    {
        strModule = pstSession->m_stReqParam.m_szKey[0];
    }
    else
    {
        strModule = "other";
    }

    udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos,
        "\"res_header\":{\"ret_code\":%d,\"cost_time_us\":%lu,\"ret_time_s\":%u,\"sid\":%d,\"uid\":%ld,\"module\":\"%s_data\"}",
        pstSession->m_stCommonResInfo.m_dwRetCode,
        pstSession->m_stCommonResInfo.m_uddwCostTime,
        CTimeUtils::GetUnixTime(),
        pstSession->m_stReqParam.m_dwSvrId,
        pstSession->m_stReqParam.m_ddwUserId,
        strModule.c_str());
    pCurPos += udwLengthAppend;

    return pCurPos - pBeginPos;
}

TINT32 CJsonResult::GenResData(SSession* pstSession, TCHAR* pBeginPos, TCHAR* pEndPos)
{
    //
    return 0;
}

TCHAR* CJsonResult::GenPushData_Pb(SSession *pstSession)
{
    m_objPbResponse.Clear();

    TINT32 dwRet = 0;
    Json::Value jTmpContent;

    // 需要压缩
    TUINT8 ucCompressFlag = 1;

    // 静态数据的单表内容
    const TCHAR *pszData = NULL;
    TUINT32 udwDataLen = 0;

    // 静态数据的原始长度和压缩长度
    TUINT32 udwRawDataLen = 0;
    TUINT32 udwNetCompressRawDataLen = 0;
    unsigned long udwCompressRawDataLen = MAX_NETIO_PACKAGE_BUF_LEN;

    // 静态数据的路由信息
    SRouteInfo stRouteInfo;
    stRouteInfo.Reset();
    stRouteInfo.m_strPlatform = pstSession->m_stReqParam.m_szPlatForm;
    stRouteInfo.m_strVs = pstSession->m_stReqParam.m_szVs;
    stRouteInfo.m_strSid = NumToString(pstSession->m_stReqParam.m_dwSvrId);
    stRouteInfo.m_udwCurTime = pstSession->m_udwReqTime;
    
    // 数据长度(压缩标志/rawdata长度/rawdata)
    TCHAR *pPackBufBeg = (TCHAR*)pstSession->m_pTmpBuf;
    TCHAR *pPackBufCur = pPackBufBeg;
    TINT32 dwTmpPackLen = 0;
    TCHAR *pTmpBuf = NULL;

    // 数据输出
    vector<string> &vecResultStaticFileList = pstSession->m_stCommonResInfo.m_vecResultStaticFileList;
    if (vecResultStaticFileList.size() == 0) //一般为出错时
    {
        pTmpBuf = pPackBufCur;

        // header
        m_objPbResponse.Clear();
        m_objPbResponse.set_service_type(EN_SERVICE_TYPE__CLIENT__LOGIN_RSP);
        m_objPbResponse.set_ret_code(pstSession->m_stCommonResInfo.m_dwRetCode);
        m_objPbResponse.set_fresh_code(0);
        m_objPbResponse.set_seq(pstSession->m_udwClientSeqNo);
        m_objPbResponse.set_svr_seq(pstSession->m_udwSeqNo);
        m_objPbResponse.set_game_time(CTimeUtils::GetUnixTime());
        m_objPbResponse.set_cost_time(1);
        m_objPbResponse.set_uid(pstSession->m_stReqParam.m_ddwUserId);
        m_objPbResponse.set_sid(pstSession->m_stReqParam.m_dwSvrId);

        // serialize
        udwRawDataLen = m_objPbResponse.ByteSize();
        m_objPbResponse.SerializeToArray(m_pSourceBuffer, udwRawDataLen);

        //压缩标记
        ucCompressFlag = 0;
        (*pTmpBuf) = ucCompressFlag;

        //回填数据长度
        udwNetCompressRawDataLen = htonl(udwRawDataLen);
        memcpy(pTmpBuf + 1, &udwNetCompressRawDataLen, 4);

        // data
        memcpy(pTmpBuf + 5, m_pSourceBuffer, udwRawDataLen);

        //数据段长度
        pPackBufCur += 1 + 4 + udwRawDataLen;
    }
    else
    {
        for (TUINT32 udwIdx = 0; udwIdx < vecResultStaticFileList.size(); ++udwIdx)
        {
            TINT32 dwUpdateType = 0;

            pTmpBuf = pPackBufCur;

            m_objPbResponse.Clear();
            jTmpContent.clear();

            if(EN_STATIC_TYPE_ACCOUNT_STATUS == vecResultStaticFileList[udwIdx])
            {
                dwRet = CStaticFileMgr::GetInstance()->GetStaticJson_Account(jTmpContent, pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus);
            }
            else
            {
                dwRet = CStaticFileMgr::GetInstance()->GetStaticJson(jTmpContent, vecResultStaticFileList[udwIdx], &stRouteInfo);
                if(EN_STATIC_TYPE_META == vecResultStaticFileList[udwIdx])
                {
                    if(dwRet == EN_JSON_DIFF__EQUAL)
                    {
                        continue;
                    }
                    else if(dwRet == EN_JSON_DIFF__NEW)
                    {
                        dwUpdateType = 0;
                    }
                    else
                    {
                        dwUpdateType = 3;
                    }
                }
            }            
            if (dwRet < 0)
            {
                TSE_LOG_DEBUG(CGlobalServ::m_poServLog, ("GetStaticJson: [static_file_type=%s] [ret=%d] do not need to output[seq=%u]", vecResultStaticFileList[udwIdx].c_str(), dwRet, pstSession->m_udwSeqNo));
                continue;
            }

            Json::Value::Members jsonDataKeys = jTmpContent.getMemberNames();
            for(Json::Value::Members::iterator it = jsonDataKeys.begin(); it != jsonDataKeys.end(); ++it)
            {
                pszData = NULL;
                pszData = m_jSeri.serializeToBuffer(jTmpContent[*it], udwDataLen);

                TableDomData *pTblData = m_objPbResponse.add_table_dom_data();
                pTblData->set_table_name(*it);
                pTblData->set_updt_type(dwUpdateType);
                pTblData->set_seq_type(0);
                pTblData->set_seq(0);
                pTblData->set_dom_type(0);
                pTblData->set_dom_data(pszData, udwDataLen);


                TSE_LOG_DEBUG(CGlobalServ::m_poServLog, ("table_raw_key: [static_file_type=%s] [key_str=%s] [serial_len=%u] [seq=%u]", 
                    vecResultStaticFileList[udwIdx].c_str(), (*it).c_str(), udwDataLen, pstSession->m_udwSeqNo));

                if (EN_STATIC_TYPE_NEW_MAINTAIN == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_NOTICE == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_ONSALE == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_SVRCONF == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_CLIENTCONF == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_ITEMSWITCH == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_ALSTORE == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_GLOBALCONF == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_USER_LINK == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_MD5 == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_ACCOUNT_STATUS == vecResultStaticFileList[udwIdx]
                || EN_STATIC_TYPE_META == vecResultStaticFileList[udwIdx])
                {
                    m_jsonWriter.omitEndingLineFeed();
                    string strData = m_jsonWriter.write(jTmpContent[*it]);
                    TSE_LOG_DEBUG(CGlobalServ::m_poServLog, ("table_raw_key: [static_file_type=%s] [content=%s] [seq=%u]", 
                        vecResultStaticFileList[udwIdx].c_str(), strData.c_str(), pstSession->m_udwSeqNo));
                }

            }

            // header
            m_objPbResponse.set_service_type(EN_SERVICE_TYPE__CLIENT__LOGIN_RSP);
            m_objPbResponse.set_ret_code(pstSession->m_stCommonResInfo.m_dwRetCode);
            m_objPbResponse.set_fresh_code(0);
            m_objPbResponse.set_seq(pstSession->m_udwClientSeqNo);
            m_objPbResponse.set_svr_seq(pstSession->m_udwSeqNo);
            m_objPbResponse.set_game_time(CTimeUtils::GetUnixTime());
            m_objPbResponse.set_cost_time(1);
            m_objPbResponse.set_uid(pstSession->m_stReqParam.m_ddwUserId);
            m_objPbResponse.set_sid(pstSession->m_stReqParam.m_dwSvrId);

            // serialize
            udwRawDataLen = m_objPbResponse.ByteSize();
            m_objPbResponse.SerializeToArray(m_pSourceBuffer, udwRawDataLen);


            //压缩标记
            (*pTmpBuf) = ucCompressFlag;

            //压缩数据
            udwCompressRawDataLen = MAX_NETIO_PACKAGE_BUF_LEN;
            compress((Bytef*)(pTmpBuf + 5), &udwCompressRawDataLen, (Bytef*)m_pSourceBuffer, udwRawDataLen);//zip
            dwTmpPackLen = udwCompressRawDataLen;

            TSE_LOG_DEBUG(CGlobalServ::m_poServLog, ("table_raw_key: [static_file_type=%s] [len=%ld] [seq=%u]", vecResultStaticFileList[udwIdx].c_str(), dwTmpPackLen, pstSession->m_udwSeqNo));

            //回填数据长度
            udwNetCompressRawDataLen = htonl(dwTmpPackLen);
            memcpy(pTmpBuf+1, &udwNetCompressRawDataLen, 4);

            //数据段长度
            pPackBufCur += 1 + 4 + dwTmpPackLen;
        }
        
        if(pstSession->m_stReqParam.m_udwCommandID == EN_CLIENT_REQ_COMMAND__INIT && pstSession->m_dwRspLen > 0)
        {
            //hu rsp
            pTmpBuf = pPackBufCur;
            //压缩标记
            (*pTmpBuf) = pstSession->m_ucHuCompressFlag;
            //数据
            memcpy(pTmpBuf+5, &pstSession->m_szRspBuf[0], pstSession->m_dwRspLen);
            dwTmpPackLen = pstSession->m_dwRspLen;
            TSE_LOG_DEBUG(CGlobalServ::m_poServLog, ("table_raw_key: [static_file_type=login_get] [len=%ld] [seq=%u]", dwTmpPackLen, pstSession->m_udwSeqNo));
            //回填数据长度
            udwNetCompressRawDataLen = htonl(dwTmpPackLen);
            memcpy(pTmpBuf+1, &udwNetCompressRawDataLen, 4);
            //数据段长度
            pPackBufCur += 1 + 4 + dwTmpPackLen;
        }
    }

    pstSession->m_dwTmpLen = pPackBufCur - pPackBufBeg;

    return pPackBufBeg;

}

string CJsonResult::GetDataTableName(const string &strDataKey)
{
 
    TUINT32 udwEndPos = strDataKey.find(":");
    string strTableName = strDataKey.substr(1, udwEndPos - 1);

    return strTableName;
}
 