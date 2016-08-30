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
    TINT32 dwRetCode = 0;
    string strStaticType = pstSession->m_stReqParam.m_szKey[0];
    string strEmail = "";
    string strSid = "0";
    if (EN_STATIC_TYPE_NEW_MAINTAIN == strStaticType)
    {
        CHttpUtils::ToLower(pstSession->m_stReqParam.m_szKey[1], strlen(pstSession->m_stReqParam.m_szKey[1]));
        strEmail = pstSession->m_stReqParam.m_szKey[1];
    }
    if (EN_STATIC_TYPE_ALLLAKE == strStaticType
        || EN_STATIC_TYPE_DOC == strStaticType
        || EN_STATIC_TYPE_GAME == strStaticType
        || EN_STATIC_TYPE_EQUIP == strStaticType
        || EN_STATIC_TYPE_FUNCTION_SWITCH == strStaticType
		|| EN_STATIC_TYPE_META_RAW == strStaticType)
    {
        pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__PARAM_ERROR;
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return -1;
    }
    
    string strDevice = pstSession->m_stReqParam.m_szDevice;
    string strRPid = kstrRPid;
    string strRid = pstSession->m_stReqParam.m_szRid;
    TINT64 ddwUid = pstSession->m_stReqParam.m_ddwUserId;
    TINT32 dwSid = pstSession->m_stReqParam.m_dwSvrId;

    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *patbDeviceProduct = &pstUserInfo->m_atbDeviceProduct[0];
    TbProduct *ptbRidProduct = &pstUserInfo->m_tbRidProduct;
    TbUser *ptbUser = &pstUserInfo->m_tbUserNew;

    if (pstSession->m_udwCommandStep == EN_COMMAND_STEP__INIT)
    {

        if (-1 != pstSession->m_stReqParam.m_dwSvrId
            && pstSession->m_stReqParam.m_dwSvrId > CGameSvrInfo::GetInstance()->m_udwSvrNum - 1)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE_ERROR_SID;
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
            if (0 == ddwUid)
            {
                pstSession->m_udwCommandStep = EN_COMMAND_STEP__2;
            }
            else
            {
                pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
            }
        }
    }

    // 请求获取玩家的sid信息
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {
        // 登录请求，所以需要从帐号系统中获取玩家的sid信息,修正请求的sid,保证玩家登录到正确的server
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;

        pstSession->ResetAwsInfo();
        if ("" == strRid && "" == strEmail)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_VISTOR;
        }
        else if ("" != strRid && "" != strEmail)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_ACCOUNT;
        }
        else
        {
            assert(0);
        }

        TbProduct tbProduct;
        TbUser tbUser;
        if (EN_LOGIN_TPYE_VISTOR == pstSession->m_stUserInfo.m_dwLoginTpye)
        {
            TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: VISITOR   [uid=%ld]   Device:[%s]   [seq=%u]", ddwUid, strDevice.c_str(), pstSession->m_udwSeqNo));
            tbProduct.Set_Device(strDevice);
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID, CompareDesc(), false);
        }
        else if (EN_LOGIN_TPYE_ACCOUNT == pstSession->m_stUserInfo.m_dwLoginTpye)
        {
            TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: ACCOUNT   [uid=%ld]   Rid:[%s], Email:[%s]   [seq=%u]", ddwUid, strRid.c_str(), strEmail.c_str(), pstSession->m_udwSeqNo));
            tbProduct.Set_Rid(strRid);
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_RID_R_PID, CompareDesc(), false);
            tbProduct.Set_Device(strDevice);
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID, CompareDesc(), false);
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

    if (EN_COMMAND_STEP__3 == pstSession->m_udwCommandStep)
    {
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;
        pstSession->ResetAwsInfo();

        TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: UID != 0    App_uid:[%ld]    R_pid[%s]   [seq=%u]", ddwUid, strRPid.c_str(), pstSession->m_udwSeqNo));
        TbProduct tbProduct;
        tbProduct.Set_App_uid(NumToString(ddwUid));
        tbProduct.Set_R_pid(strRPid);
        CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_PRIMARY);

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
    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            //pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
        AwsRspInfo *pstAwsRspInfo = NULL;
        TINT32 dwRetcode = 0;
        for (TUINT32 udIdx = 0; udIdx < pstSession->m_vecAwsRsp.size(); ++udIdx)
        {
            pstAwsRspInfo = pstSession->m_vecAwsRsp[udIdx];
            string strTableRawName = CBaseProcedure::GetTableRawName(pstAwsRspInfo->sTableName);
            if (EN_AWS_TABLE_PRODUCT == strTableRawName)
            {
                if (ETbPRODUCT_OPEN_TYPE_GLB_RID_R_PID == pstAwsRspInfo->udwIdxNo)
                {
                    dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbRidProduct, sizeof(TbProduct), 1);
                    if (dwRetcode >= 0)
                    {
                        pstUserInfo->m_dwRidProductNum = dwRetcode;
                    }
                }
                else if (ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID == pstAwsRspInfo->udwIdxNo)
                {
                    dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, patbDeviceProduct, sizeof(TbProduct), MAX_PRODUCT_NUM);
                    if (dwRetcode >= 0)
                    {
                        pstUserInfo->m_dwDeviceProductNum = dwRetcode;
                    }
                }
                else if (ETbPRODUCT_OPEN_TYPE_PRIMARY == pstAwsRspInfo->udwIdxNo)
                {
                    pstSession->m_udwCommandStep = EN_COMMAND_STEP__5;
                    dwRetCode = CAwsResponse::OnGetItemRsp(*pstAwsRspInfo, ptbRidProduct);
                    if (dwRetCode >= 0)
                    {
                        pstUserInfo->m_dwRidProductNum = dwRetCode;
                    }
                }
                else
                {
                    assert(0);
                }
                TSE_LOG_ERROR(pstSession->m_poServLog, ("[wavetest]: tbl=%s rspp=%s [seq=%u]",
                    pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo));
                continue;
            }
        }
    }

    if (EN_COMMAND_STEP__5 == pstSession->m_udwCommandStep)
    {
        //login_type == account
        if ("" != ptbRidProduct->Get_Rid())
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_ACCOUNT;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__6;
            pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
            bNeedResponse = TRUE;
            TSE_LOG_ERROR(pstSession->m_poServLog, ("[kurotest]: logintype=%d rid=%s [seq=%u]", pstSession->m_stUserInfo.m_dwLoginTpye, ptbRidProduct->Get_Rid().c_str(), pstSession->m_udwSeqNo));

            pstSession->ResetAwsInfo();
            TbUser tbUser;
            tbUser.Set_Rid(ptbRidProduct->Get_Rid());
            CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbUser, ETbUSER_OPEN_TYPE_PRIMARY);

            dwRetCode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
            if (dwRetCode < 0)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
                TSE_LOG_ERROR(pstSession->m_poServLog, ("CProcessInit::requestHandler: send req failed [seq=%u]", pstSession->m_udwSeqNo));
                return -1;
            }
            return 0;
        }
        //login_type == visitor
        else
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_VISTOR;
            pstUserInfo->m_atbDeviceProduct[0] = *ptbRidProduct;
            pstUserInfo->m_dwDeviceProductNum = pstUserInfo->m_dwRidProductNum;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
            TSE_LOG_ERROR(pstSession->m_poServLog, ("[kurotest]: logintype=%d device=%s  devicenum=%d [seq=%u]", pstSession->m_stUserInfo.m_dwLoginTpye, pstUserInfo->m_atbDeviceProduct[0].Get_Device().c_str(), pstUserInfo->m_dwDeviceProductNum, pstSession->m_udwSeqNo));
        }
    }

    if (EN_COMMAND_STEP__6 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            //pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
        AwsRspInfo *pstAwsRspInfo = NULL;
        for (TUINT32 udIdx = 0; udIdx < pstSession->m_vecAwsRsp.size(); ++udIdx)
        {
            pstAwsRspInfo = pstSession->m_vecAwsRsp[udIdx];
            string strTableRawName = CBaseProcedure::GetTableRawName(pstAwsRspInfo->sTableName);
            dwRetCode = CAwsResponse::OnGetItemRsp(*pstAwsRspInfo, ptbUser);
            if (dwRetCode >= 0)
            {
                pstUserInfo->m_dwUserNum = dwRetCode;
            }

            TSE_LOG_ERROR(pstSession->m_poServLog, ("[wavetest]: tbl=%s rspp=%s [seq=%u]",
                pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo));
        }
    }

    if (EN_COMMAND_STEP__7 == pstSession->m_udwCommandStep)
    {
        // 获取玩家帐号信息
        SLoginInfo stLoginInfo;
        stLoginInfo.Reset();
        stLoginInfo.m_strRid = strRid;
        stLoginInfo.m_strEmail = strEmail;
        stLoginInfo.m_strPasswd = "null";
        stLoginInfo.m_strDevice = strDevice;
        stLoginInfo.m_strRPid = strRPid;
        TbProduct *ptbProduct = NULL;
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;

        CAccountLogic::GetInitPlayerStatus(&pstSession->m_stUserInfo, stLoginInfo, ptbProduct);
        TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: CAccountLogic::GetInitPlayerStatus [uid=%ld] [status=%ld] [seq=%u]", \
            pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid, \
            pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus, \
            pstSession->m_udwSeqNo));
        
        if (EN_PLAYER_STATUS_IMPORT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            //do nothing
        }
        else
        {
            // 帐号存在问题的
            if (0 == pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE_ERROR_UID;
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

    if (EN_COMMAND_STEP__8 == pstSession->m_udwCommandStep)
    {
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(strStaticType);
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return 0;
    }


    return 0;
}