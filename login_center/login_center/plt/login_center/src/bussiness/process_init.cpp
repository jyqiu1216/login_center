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
    TINT64 ddwUid = pstSession->m_stReqParam.m_ddwUserId;
    TINT32 dwSid = pstSession->m_stReqParam.m_dwSvrId;

    string strVs = pstSession->m_stReqParam.m_szVs;

    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *patbDeviceProduct = &pstUserInfo->m_atbDeviceProduct[0];
    TbProduct *ptbRidProduct = &pstUserInfo->m_tbRidProduct;
    TbUser *ptbUser = &pstUserInfo->m_tbUserNew;

    if (pstSession->m_udwCommandStep == EN_COMMAND_STEP__INIT)
    {
        if (-1 != dwSid && dwSid > CGameSvrInfo::GetInstance()->m_udwSvrNum - 1)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE_ERROR_SID;
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
        CStaticFileContent *pobjConent = CStaticFileMgr::GetInstance()->GetStaticFile(EN_STATIC_TYPE_NEW_MAINTAIN, pstSession->m_stReqParam.m_szPlatForm, pstSession->m_stReqParam.m_szVs);
        
        // 判断是否有全服Maintain和版本强制更新
        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticFileMgr::GetInstance()->UpdtAndGetMaintainStatus(pobjConent->m_jsonContent, 
            pstSession->m_stReqParam.m_szPlatForm, pstSession->m_stReqParam.m_szVs, NumToString(pstSession->m_stReqParam.m_dwSvrId), pstSession->m_udwReqTime);
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

    // 请求获取玩家的sid信息---请求
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {
        // 登录请求，所以需要从帐号系统中获取玩家的sid信息,修正请求的sid,保证玩家登录到正确的server
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;

        pstSession->ResetAwsInfo();

        //login type
        if (0 != ddwUid && -1 == dwSid)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_DEBUG;
        }
        else if ("" == strRid && "" == strEmail
            && "" == strThId && "" == strPasswd)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_VISTOR;
        }
        else if ("" != strEmail && "" != strPasswd && "" != strRid)
        {
            pstSession->m_stUserInfo.m_dwLoginTpye = EN_LOGIN_TPYE_ACCOUNT;
        }
        else
        {
            assert(0);
        }
        TSE_LOG_INFO(pstSession->m_poServLog, ("CProcessInit::requestHandler: login_type [%d] [seq=%u]", pstSession->m_stUserInfo.m_dwLoginTpye, pstSession->m_udwSeqNo));
        
        TbProduct tbProduct;
        TbUser tbUser;
       
        switch (pstSession->m_stUserInfo.m_dwLoginTpye) {
            case EN_LOGIN_TPYE_VISTOR:
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: VISITOR   Device:[%s]   [seq=%u]", strDevice.c_str(), pstSession->m_udwSeqNo));
                tbProduct.Set_Device(strDevice);
                tbProduct.Set_R_pid(strRPid);
                CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID, CompareDesc(), false);
                break;
             case EN_LOGIN_TPYE_ACCOUNT:
                 TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: ACCOUNT   Rid:[%s], Email:[%s]   [seq=%u]", strRid.c_str(), strEmail.c_str(), pstSession->m_udwSeqNo));
                tbProduct.Set_Rid(strRid);
                tbProduct.Set_R_pid(strRPid);
                CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_RID_R_PID, CompareDesc(), false);
                tbProduct.Set_Device(strDevice);
                tbProduct.Set_R_pid(strRPid);
                CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE_R_PID, CompareDesc(), false);
                tbUser.Set_Rid(strRid);
                CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbUser, ETbUSER_OPEN_TYPE_PRIMARY);
                break;
            case EN_LOGIN_TPYE_DEBUG:
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: DEBUG    App_uid:[%ld]    R_pid[%s]   [seq=%u]", ddwUid, strRPid.c_str(), pstSession->m_udwSeqNo));
                tbProduct.Set_App_uid(NumToString(ddwUid));
                tbProduct.Set_R_pid(strRPid);
                CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_PRIMARY);
                break;
            default:
                assert(0);
                break;
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

    // 解析获取玩家的sid信息---响应
    if (EN_COMMAND_STEP__3 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            //pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
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

      //new game
      if (1L == dwNewGameFlag)
      {
          pstSession->m_stReqParam.m_dwSvrId = 0;
          pstSession->m_stReqParam.m_ddwUserId = 0;
          vector<TbProduct *> vecTbProduct;
          pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
          bNeedResponse = TRUE;

          pstSession->ResetAwsReq();

          for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwDeviceProductNum; dwIdx++)
          {
              if (strRPid == pstUserInfo->m_atbDeviceProduct[dwIdx].Get_R_pid()
                  && "" == pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Rid()
                  && "0" != pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Device())
              {
                  TSE_LOG_INFO(pstSession->m_poServLog, ("[kurotest] NEW GAME device:[%s]", pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Device().c_str()));
                  pstUserInfo->m_atbDeviceProduct[dwIdx].Set_Device("0");
                  vecTbProduct.push_back(&pstUserInfo->m_atbDeviceProduct[dwIdx]);

                  dwRetCode = CAwsRequest::UpdateItem(pstSession->m_vecAwsReq, &pstUserInfo->m_atbDeviceProduct[dwIdx]);
                  if (dwRetCode != 0)
                  {
                      TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: not clear device [seq=%u]", pstSession->m_udwSeqNo));
                  }
              }
          }
          
          if (0 != vecTbProduct.size())
          {
              pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
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

    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            //pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
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
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__STATUS_ERROR;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        //新用户
        else if (EN_PLAYER_STATUS_IMPORT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stReqParam.m_dwSvrId = CGameSvrInfo::GetInstance()->GetNewPlayerSvr();
            pstSession->m_stReqParam.m_ddwUserId = 0;
        }
        else if (EN_PLAYER_STATUS_PASSWORD_CHANGE == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            if ("1.2" == strVs)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__PASSWORD_CHANGE;
            }
            else
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            }
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_BLACK_ACCOUNT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            if ("1.2" == strVs)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__BLACK_ACCOUNT;
            }
            else
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = 40008;
            }
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            if ("1.2" == strVs)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__ACCOUNT_INFO_INVAILD;
            }
            else
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            }
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_ACCOUNT_NOT_GAME_DATA == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            if ("1.2" == strVs)
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__ACCOUNT_NOT_GAME_DATA;
            }
            else
            {
                pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            }
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_MULTI_EMAIL == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__AMULTI_EMAIL;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_EMAIL_HAS_MULTI_GAME_DATA == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__EMAIL_HAS_MULTI_GAME_DATA;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__PRODUCTION_INFO_INVAILD;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_USER_DATA_NOT_EXIST == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__USER_DATA_NOT_EXIST;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        //UNREGISTER, UNACTIVE, ACTIVE
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
                ptbProduct->Set_R_pid(strRPid);
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
            //pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
    }



    if (EN_COMMAND_STEP__7 == pstSession->m_udwCommandStep)
    {
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;

        TBOOL bHasMaintain = FALSE;
        CStaticFileContent *pobjConent = CStaticFileMgr::GetInstance()->GetStaticFile(EN_STATIC_TYPE_NEW_MAINTAIN, pstSession->m_stReqParam.m_szPlatForm, pstSession->m_stReqParam.m_szVs);

        // 分服mintain
        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticFileMgr::GetInstance()->UpdtAndGetMaintainStatus(pobjConent->m_jsonContent, 
            pstSession->m_stReqParam.m_szPlatForm, pstSession->m_stReqParam.m_szVs, NumToString(pstSession->m_stReqParam.m_dwSvrId), pstSession->m_udwReqTime);
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

    if(EN_COMMAND_STEP__8 == pstSession->m_udwCommandStep) //hu--request
    {
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__9;
        
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__HU;
        bNeedResponse = TRUE;

        dwRetCode = CBaseProcedure::SendLoginGetRequest(pstSession);
        if (dwRetCode < 0)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
            TSE_LOG_ERROR(pstSession->m_poServLog, ("SendLoginGetRequest: send req failed [seq=%u]", pstSession->m_udwSeqNo));
            return -3;
        }
        return 0;
    }

    if (EN_COMMAND_STEP__9 == pstSession->m_udwCommandStep)
    {
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
 
    pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE_FATAL_ERROR;
    pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
    return 0;

}
