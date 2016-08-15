#include "process_init.h"
#include "static_file_mgr.h"
#include "procedure_base.h"
#include "aws_request.h"
#include "game_svr.h"
#include "session.h"
#include "account_logic.h"
#include "http_utils.h"

TINT32 CProcessInit::requestHandler(SSession* pstSession, TBOOL &bNeedResponse)
{
    TINT32 dwRetCode = 0;
    string strRid = pstSession->m_stReqParam.m_szKey[0];
    CHttpUtils::ToLower(pstSession->m_stReqParam.m_szKey[1], strlen(pstSession->m_stReqParam.m_szKey[1]));
    string strEmail = pstSession->m_stReqParam.m_szKey[1];
    string strPasswd = pstSession->m_stReqParam.m_szKey[2];
    string strThId = pstSession->m_stReqParam.m_szKey[3];
    TINT32 dwNewGameFlag = atoi(pstSession->m_stReqParam.m_szKey[4]);

    string strDevice = pstSession->m_stReqParam.m_szDevice;
    string strRPid = kstrRPid;
    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *ptbProduct = pstUserInfo->m_tbProduct;
    TbUser *ptbUser = pstUserInfo->m_tbUser;

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

    // 判断是否有全服Maintain和版本强制更新
    if (EN_COMMAND_STEP__1 == pstSession->m_udwCommandStep)
    {
        TBOOL bHasMaintain = FALSE;
        SStaticFileInfo stStaticFileInfo;
        stStaticFileInfo.Reset();
        SRouteInfo stRouteInfo;
        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = pstSession->m_stReqParam.m_szPlatForm;
        stRouteInfo.m_strSid = NumToString(pstSession->m_stReqParam.m_dwSvrId);
        stRouteInfo.m_strVs = pstSession->m_stReqParam.m_szVs;
        CStaticFileMgr::GetInstance()->GetMaintainJson(stRouteInfo, &stStaticFileInfo);
        
        // 判断是否有全服Maintain和版本强制更新
        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticDataMaintain::GetMaintainStatus(stRouteInfo, stStaticFileInfo.m_jDataJson[0]["data"]);
        if (EN_MAINTAIN_TYPE_NORMAL == dwMaintainStatus)
        {
            bHasMaintain = FALSE;
        }
        else
        {
            bHasMaintain = TRUE;
        }

        if (TRUE == bHasMaintain)
        {
            pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NEW_MAINTAIN);
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else
        {
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__2;
        }
    }


    // 请求获取玩家的sid信息
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {
        // 登录请求，所以需要从帐号系统中获取玩家的sid信息,修正请求的sid,保证玩家登录到正确的server
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;

        pstSession->ResetAwsInfo();
        // 本机数据登录
        if ("" == strRid
            && "" == strEmail
            && "" == strThId
            && "" == strPasswd)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_VISTOR;
        }
        // 带帐号信息登录
        else
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_ACCOUNT;
        }
        if (0 != pstSession->m_stReqParam.m_ddwUserId
            && -1 == pstSession->m_stReqParam.m_dwSvrId)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_DEBUG;
        }
        TSE_LOG_INFO(pstSession->m_poServLog, ("CProcessInit::requestHandler: login_type [%d] [seq=%u]", pstSession->m_stUserInfo.m_dwLoginTpye, pstSession->m_udwSeqNo));

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
        else if (EN_LOGIN_TPYE_DEBUG == pstSession->m_stUserInfo.m_dwLoginTpye)
        {
            TbProduct tbProduct;
            tbProduct.Set_App_uid(NumToString(pstSession->m_stReqParam.m_ddwUserId));
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_PRIMARY);
        }
        else
        {
            assert(0);
        }

        dwRetCode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
        if (dwRetCode < 0)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
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

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__5;
        AwsRspInfo *pstAwsRspInfo = NULL;
        TINT32 dwRetcode = 0;

        for (TUINT32 udIdx = 0; udIdx < pstSession->m_vecAwsRsp.size(); ++udIdx)
        {
            pstAwsRspInfo = pstSession->m_vecAwsRsp[udIdx];
            string strTableRawName = CBaseProcedure::GetTableRawName(pstAwsRspInfo->sTableName);
            if (strTableRawName == EN_AWS_TABLE_PRODUCT)
            {

                switch (pstAwsRspInfo->udwIdxNo)
                {
                case ETbPRODUCT_OPEN_TYPE_PRIMARY:
                    CAwsResponse::OnGetItemRsp(*pstAwsRspInfo, ptbProduct);
                    break;
                case ETbPRODUCT_OPEN_TYPE_GLB_DEVICE:
                    dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbProduct, sizeof(TbProduct), MAX_PRODUCT_NUM);
                    if (dwRetcode >= 0)
                    {
                        pstUserInfo->m_dwProductNum = dwRetcode;
                    }
                    break;
                case ETbPRODUCT_OPEN_TYPE_GLB_RID:
                    dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbProduct, sizeof(TbProduct), MAX_PRODUCT_NUM);
                    if (dwRetcode >= 0)
                    {
                        pstUserInfo->m_dwProductNum = dwRetcode;
                    }
                    break;
                default:
                    break;
                }

                TSE_LOG_ERROR(pstSession->m_poServLog, ("[wavetest]: tbl=%s rspp=%s [seq=%u]", 
                    pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo));

                continue;
            }
            if (strTableRawName == EN_AWS_TABLE_USER)
            {
                dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbUser, sizeof(TbUser), MAX_MAX_NUM);
                if (dwRetcode >= 0)
                {
                    pstUserInfo->m_dwUserNum = dwRetcode;

                    TSE_LOG_ERROR(pstSession->m_poServLog, ("[wavetest]: tbl=%s rspp=%s [seq=%u]",
                        pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo));
                }
                continue;
            }
        }

        if (1L == dwNewGameFlag)
        {
            pstSession->m_stReqParam.m_dwSvrId = 0;
            pstSession->m_stReqParam.m_ddwUserId = 0;

            vector<TbProduct *> vecTbProduct;
            for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwProductNum; ++dwIdx)
            {
                if (strRPid == pstUserInfo->m_tbProduct[dwIdx].Get_R_pid()
                    && 0 == pstUserInfo->m_tbProduct[dwIdx].Get_Status()
                    && "0" != pstUserInfo->m_tbProduct[dwIdx].Get_Device())
                {
                    vecTbProduct.push_back(&pstUserInfo->m_tbProduct[dwIdx]);
                }
            }

            if (0 != vecTbProduct.size())
            {
                pstSession->ResetAwsReq();
                for (TINT32 dwIdy = 0; dwIdy < vecTbProduct.size(); ++dwIdy)
                {
                    vecTbProduct[dwIdy]->Set_Device("0");
                    dwRetCode = CAwsRequest::UpdateItem(pstSession->m_vecAwsReq, vecTbProduct[dwIdy]);
                    if (dwRetCode != 0)
                    {
                        TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: not clear device [seq=%u]",
                            pstSession->m_udwSeqNo));
                    }
                }

                if (0 != pstSession->m_vecAwsReq.size())
                {

                    pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
                    pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
                    bNeedResponse = TRUE;

                    dwRetCode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
                    if (dwRetCode < 0)
                    {
                        pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
                        TSE_LOG_ERROR(pstSession->m_poServLog, ("CProcessInit::requestHandler: send req failed [seq=%u]", pstSession->m_udwSeqNo));
                        return -3;
                    }
                    return 0;
                }
                else
                {
                    pstSession->m_udwCommandStep = EN_COMMAND_STEP__5;
                }
            }
            else
            {
                pstSession->m_udwCommandStep = EN_COMMAND_STEP__5;
            }

        }
    }

    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__5;

    }


    if (EN_COMMAND_STEP__5 == pstSession->m_udwCommandStep)
    {

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
        // 获取玩家帐号信息
        SLoginInfo stLoginInfo;
        stLoginInfo.Reset();
        stLoginInfo.m_strRid = strRid;
        stLoginInfo.m_strEmail = strEmail;
        stLoginInfo.m_strPasswd = strPasswd;
        stLoginInfo.m_strThId = strThId;
        stLoginInfo.m_strDevice = strDevice;
        stLoginInfo.m_strRPid = strRPid;

        TbProduct *ptbProduct = NULL;
        CAccountLogic::GetInitPlayerStatus(&pstSession->m_stUserInfo, stLoginInfo, ptbProduct);
        TSE_LOG_INFO(pstSession->m_poServLog, ("CProcessInit::requestHandler: GetInitPlayerStatus [status=%ld] [seq=%u]", 
            pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus, \
            pstSession->m_udwSeqNo));
        if (EN_PLAYER_STATUS_ERROR == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        if (EN_PLAYER_STATUS_MULTI_EMAIL == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_EMAIL_HAS_MULTI_GAME_DATA == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_BLACK_ACCOUNT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 40008;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_PASSWORD_CHANGE == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }       
        else if (EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_ACCOUNT_NOT_GAME_DATA == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_IMPORT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stReqParam.m_dwSvrId = CGameSvrInfo::GetInstance()->GetNewPlayerSvr();
            pstSession->m_stReqParam.m_ddwUserId = 0;
        }
        else 
        {
            pstSession->m_stReqParam.m_dwSvrId = pstSession->m_stUserInfo.m_stUserStatus.m_ddwSid;
            pstSession->m_stReqParam.m_ddwUserId = pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid;
            // do nothing


            if ("" != stLoginInfo.m_strProductInfo
                && NULL != ptbProduct)
            {
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: update device [seq=%u]",
                    pstSession->m_udwSeqNo));

                pstSession->m_udwCommandStep = EN_COMMAND_STEP__6;
                pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
                bNeedResponse = TRUE;

                pstSession->ResetAwsReq();

                ptbProduct->Set_App_uid(NumToString(pstSession->m_stReqParam.m_ddwUserId));
                ptbProduct->Set_R_pid(NumToString(strRPid));
                ptbProduct->Set_Product_info(stLoginInfo.m_strProductInfo);


                dwRetCode = CAwsRequest::UpdateItem(pstSession->m_vecAwsReq, ptbProduct);
                if (dwRetCode != 0)
                {
                    TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: not update device [seq=%u]",
                        pstSession->m_udwSeqNo));
                    pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
                }

                dwRetCode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
                if (dwRetCode < 0)
                {
                    pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
                    TSE_LOG_ERROR(pstSession->m_poServLog, ("CProcessInit::requestHandler: send req failed [seq=%u]", pstSession->m_udwSeqNo));
                    return -3;
                }

                return 0;

            }
        }
    }


    if (EN_COMMAND_STEP__6 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;

    }



    if (EN_COMMAND_STEP__7 == pstSession->m_udwCommandStep)
    {
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;

        TBOOL bHasMaintain = FALSE;

        SStaticFileInfo stStaticFileInfo;
        stStaticFileInfo.Reset();
        SRouteInfo stRouteInfo;
        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = pstSession->m_stReqParam.m_szPlatForm;
        stRouteInfo.m_strSid = NumToString(pstSession->m_stReqParam.m_dwSvrId);
        stRouteInfo.m_strVs = pstSession->m_stReqParam.m_szVs;
        CStaticFileMgr::GetInstance()->GetMaintainJson(stRouteInfo, &stStaticFileInfo);
        // 分服mintain
        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticDataMaintain::GetMaintainStatus(stRouteInfo, stStaticFileInfo.m_jDataJson[0]["data"]);
        if (EN_MAINTAIN_TYPE_NORMAL == dwMaintainStatus)
        {
            bHasMaintain = FALSE;
        }
        else
        {
            bHasMaintain = TRUE;
        }

        // 分服mintain
        if (TRUE == bHasMaintain)
        {
            pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NEW_MAINTAIN);
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }

    }


    if (EN_COMMAND_STEP__8 == pstSession->m_udwCommandStep)
    {

  
        //pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_GAME);
        //pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_DOC);
        //pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_EQUIP);
        //pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_META);
        //pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_ALLLAKE);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_META);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_GUIDE);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NEW_MAINTAIN);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NOTICE);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_SVRCONF);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_ITEMSWITCH);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_ALSTORE);
        
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_CLIENTCONF);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_GLOBALCONF);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_USER_LINK);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_MD5);
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_ACCOUNT_STATUS);

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return 0;
    }
 
    pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
    pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
    return 0;

}
