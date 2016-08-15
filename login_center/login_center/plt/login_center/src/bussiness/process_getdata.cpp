
#include "process_getdata.h"
#include "static_file_mgr.h"
#include "procedure_base.h"
#include "aws_request.h"
#include "game_svr.h"
#include "session.h"
#include "account_logic.h"
#include "http_utils.h"

TINT32 CProcessGetdata::requestHandler(SSession* pstSession, TBOOL &bNeedResponse)
{
    //handle request param
    TINT32 dwRetCode = 0;
    string strStaticType = pstSession->m_stReqParam.m_szKey[0];
    string strEmail = "";
    string strSid = "0";
    if (EN_STATIC_TYPE_NEW_MAINTAIN == strStaticType)
    {
        CHttpUtils::ToLower(pstSession->m_stReqParam.m_szKey[1], strlen(pstSession->m_stReqParam.m_szKey[1]));
        strEmail = pstSession->m_stReqParam.m_szKey[1];
    }
    if (EN_STATIC_TYPE_ALLLAKE == strStaticType)
    {
        if ('\0' == pstSession->m_stReqParam.m_szKey[1][0])
        {
            strcpy(pstSession->m_stReqParam.m_szKey[1], "0");
        }
        else
        {
            strSid = pstSession->m_stReqParam.m_szKey[1];
        }
   
    }

    string strDevice = pstSession->m_stReqParam.m_szDevice;
    string strRPid = kstrRPid;
    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *ptbProduct = pstUserInfo->m_tbProduct;
    TbUser *ptbUser = pstUserInfo->m_tbUser;
    string strRid = pstSession->m_stReqParam.m_szRid;

    TSE_LOG_DEBUG(pstSession->m_poServLog, ("[kurotest]Param: Rid = %s Email = %s Device = %s Rpid = %s Uid = %ld Sid = %d",
        strRid.c_str(), strEmail.c_str(), strDevice.c_str(), strRPid.c_str(), pstSession->m_stReqParam.m_ddwUserId, strSid));

    if (pstSession->m_udwCommandStep == EN_COMMAND_STEP__INIT)
    {

        if (-1 != pstSession->m_stReqParam.m_dwSvrId
            && pstSession->m_stReqParam.m_dwSvrId > CGameSvrInfo::GetInstance()->m_udwSvrNum - 1)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }

        if (EN_STATIC_TYPE_NEW_MAINTAIN != strStaticType)
        {
            // next procedure
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
        }       
        else
        {
            // next procedure
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__2;
        }
    }

    //get account info
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;



        pstSession->ResetAwsInfo();
        // 本机数据登录
        if ("" == strRid
            && "" == strEmail)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_VISTOR;
        }
        // 带帐号信息登录
        else
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_ACCOUNT;
        }



        if (EN_LOGIN_TPYE_VISTOR == pstSession->m_stUserInfo.m_dwLoginTpye)
        {
            TbProduct tbProduct;

            tbProduct.Set_Device(pstSession->m_stReqParam.m_szDevice);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE, CompareDesc(), false);

        }
        else if (EN_LOGIN_TPYE_ACCOUNT == pstSession->m_stUserInfo.m_dwLoginTpye)
        {
            TbProduct tbProduct;
            tbProduct.Set_Rid(strRid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_RID, CompareDesc(), false);
            TbUser tbUser;
            tbUser.Set_Email(strEmail);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbUser, ETbUSER_OPEN_TYPE_GLB_EMAIL, CompareDesc(), false);
        }
        else
        {
            assert(0);
        }

        dwRetCode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
        if (dwRetCode < 0)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
            TSE_LOG_ERROR(pstSession->m_poServLog, ("CProcessInit::requestHandler: send req failed [seq=%u]", pstSession->m_udwSeqNo));
            return -1;
        }
        return 0;
    }

    // 解析获取玩家的sid信息
    if (EN_COMMAND_STEP__3 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }


        pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
        AwsRspInfo *pstAwsRspInfo = NULL;
        TINT32 dwRetcode = 0;

        for (TUINT32 udIdx = 0; udIdx < pstSession->m_vecAwsRsp.size(); ++udIdx)
        {
            pstAwsRspInfo = pstSession->m_vecAwsRsp[udIdx];
            string strTableRawName = CBaseProcedure::GetTableRawName(pstAwsRspInfo->sTableName);
            if (strTableRawName == EN_AWS_TABLE_PRODUCT)
            {
                dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbProduct, sizeof(TbProduct), MAX_PRODUCT_NUM);
                if (dwRetcode >= 0)
                {
                    pstUserInfo->m_dwProductNum = dwRetcode;
                }
                continue;
            }
            if (strTableRawName == EN_AWS_TABLE_USER)
            {
                dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbUser, sizeof(TbUser), MAX_MAX_NUM);
                if (dwRetcode >= 0)
                {
                    pstUserInfo->m_dwUserNum = dwRetcode;
                }
                continue;
            }
        }

        // 获取玩家帐号信息
        SLoginInfo stLoginInfo;
        stLoginInfo.Reset();
        stLoginInfo.m_strRid = strRid;
        stLoginInfo.m_strEmail = strEmail;
        stLoginInfo.m_strPasswd = "null";
        stLoginInfo.m_strDevice = strDevice;
        stLoginInfo.m_strRPid = strRPid;
        TbProduct *ptbProduct = NULL;

        CAccountLogic::GetInitPlayerStatus(&pstSession->m_stUserInfo, stLoginInfo, ptbProduct);
        TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: CAccountLogic::GetInitPlayerStatus [uid=%ld] [status=%ld] [seq=%u]", \
            pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid, \
            pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus, \
            pstSession->m_udwSeqNo));
        if (EN_PLAYER_STATUS_IMPORT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stReqParam.m_dwSvrId = CGameSvrInfo::GetInstance()->GetNewPlayerSvr();
            pstSession->m_stReqParam.m_ddwUserId = 0;
        }
        else
        {
            // 帐号存在问题的
            if (0 == pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
                pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
                return 0;
            }
            else
            {
                pstSession->m_stReqParam.m_dwSvrId = pstSession->m_stUserInfo.m_stUserStatus.m_ddwSid;
                pstSession->m_stReqParam.m_ddwUserId = pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid;
            }
        }
    }

    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(strStaticType);
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return 0;
    }

    return 0;
}


