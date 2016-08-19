#include "process_update.h"
#include "static_file_mgr.h"
#include "procedure_base.h"
#include "aws_request.h"
#include "game_svr.h"
#include "session.h"
#include "account_logic.h"


TINT32 CProcessUpdate::requestHandler(SSession *pstSession, TBOOL &bNeedResponse)
{
    TINT32 dwRetCode = 0;
    string strRPid = kstrRPid;
    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *ptbProduct = pstUserInfo->m_tbProduct;
    TbUser *ptbUser = pstUserInfo->m_tbUser;
    string strRid = pstSession->m_stReqParam.m_szRid;
    string strDevice = pstSession->m_stReqParam.m_szDevice;

    if (pstSession->m_udwCommandStep == EN_COMMAND_STEP__INIT)
    {
        if (-1 != pstSession->m_stReqParam.m_dwSvrId
            && pstSession->m_stReqParam.m_dwSvrId > CGameSvrInfo::GetInstance()->m_udwSvrNum - 1)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        // next procedure
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__1;
    }


    // 请求获取玩家的sid信息
    if (EN_COMMAND_STEP__1 == pstSession->m_udwCommandStep)
    {
        // 登录请求，所以需要从帐号系统中获取玩家的sid信息,修正请求的sid,保证玩家登录到正确的server
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__2;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;

        pstSession->ResetAwsInfo();

        
        TbProduct tbProduct;
        tbProduct.Set_App_uid(NumToString(pstSession->m_stReqParam.m_ddwUserId));
        tbProduct.Set_R_pid(strRPid);
        CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_PRIMARY);

        if ("" != strRid)
        {
            TbUser tbUser;
            tbUser.Set_Rid(strRid);
            CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbUser, ETbUSER_OPEN_TYPE_PRIMARY);
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
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {

        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }


        pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
        AwsRspInfo *pstAwsRspInfo = NULL;

        for (TUINT32 udIdx = 0; udIdx < pstSession->m_vecAwsRsp.size(); ++udIdx)
        {
            pstAwsRspInfo = pstSession->m_vecAwsRsp[udIdx];
            string strTableRawName = CBaseProcedure::GetTableRawName(pstAwsRspInfo->sTableName);
            if (strTableRawName == EN_AWS_TABLE_PRODUCT)
            {
                dwRetCode = CAwsResponse::OnGetItemRsp(*pstAwsRspInfo, ptbProduct);
                if (dwRetCode >= 0)
                {
                    pstUserInfo->m_dwProductNum = dwRetCode;
                }
                continue;
            }
            if (strTableRawName == EN_AWS_TABLE_USER)
            {
                dwRetCode = CAwsResponse::OnGetItemRsp(*pstAwsRspInfo, ptbUser);
                if (dwRetCode >= 0)
                {
                    pstUserInfo->m_dwUserNum = dwRetCode;
                }
                continue;
            }
        }     
            
        // 获取玩家帐号信息
        SLoginInfo stLoginInfo;
        stLoginInfo.Reset();
        stLoginInfo.m_strRid = strRid;
        stLoginInfo.m_strDevice = strDevice;


        CAccountLogic::GetUpdatePlayerStatus(&pstSession->m_stUserInfo, stLoginInfo);
       
    }

   

    if (EN_COMMAND_STEP__3 == pstSession->m_udwCommandStep)
    {
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_MD5);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_ACCOUNT_STATUS);
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return 0;
    }


    return 0;
}
