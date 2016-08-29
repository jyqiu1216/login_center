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
            TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: VISITOR   Device:[%s]", strDevice.c_str()));
            tbProduct.Set_Device(strDevice);
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID, CompareDesc(), false);

        }
        else if (EN_LOGIN_TPYE_ACCOUNT == pstSession->m_stUserInfo.m_dwLoginTpye)
        {
            TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: ACCOUNT   Rid:[%s], Email:[%s]", strRid.c_str(), strEmail.c_str()));
            tbProduct.Set_Rid(strRid);
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_RID_R_PID, CompareDesc(), false);
            tbProduct.Set_Device(strDevice);
            tbProduct.Set_R_pid(strRPid);
            CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID, CompareDesc(), false);
            tbUser.Set_Rid(strRid);
            CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbUser, ETbUSER_OPEN_TYPE_PRIMARY);
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
            //pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
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
            if (strTableRawName == EN_AWS_TABLE_USER)
            {
                dwRetcode = CAwsResponse::OnGetItemRsp(*pstAwsRspInfo, ptbUser);
                if (dwRetcode >= 0)
                {
                    pstUserInfo->m_dwUserNum = dwRetcode;
                    TSE_LOG_ERROR(pstSession->m_poServLog, ("[wavetest]: tbl=%s rspp=%s [seq=%u]",
                        pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo));
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

    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
        pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(strStaticType);
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return 0;
    }


    return 0;
}