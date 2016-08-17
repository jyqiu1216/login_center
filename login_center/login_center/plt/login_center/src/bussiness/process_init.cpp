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
    //handle request param
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

    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *ptbProduct = pstUserInfo->m_tbProduct;
    TbUser *ptbUser = pstUserInfo->m_tbUser;

    TSE_LOG_DEBUG(pstSession->m_poServLog, ("[kurotest]Param: Rid = %s Email = %s Password = %s ThId = %s NewGame = %d Device = %s Rpid = %s Uid = %ld Sid = %d",
        strRid.c_str(), strEmail.c_str(), strPasswd.c_str(), strThId.c_str(), dwNewGameFlag, strDevice.c_str(), strRPid.c_str(), ddwUid, dwSid));

    //for server maintain
    TINT32 dwreqSid = pstSession->m_stReqParam.m_dwSvrId;

    if (pstSession->m_udwCommandStep == EN_COMMAND_STEP__INIT)
    {
        if (-1 != dwSid && dwSid > CGameSvrInfo::GetInstance()->m_udwSvrNum - 1)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        // next procedure
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__1;
    }

    // global maintain or force update
    if (EN_COMMAND_STEP__1 == pstSession->m_udwCommandStep)
    {
        SStaticFileInfo stStaticFileInfo;
        stStaticFileInfo.Reset();
        SRouteInfo stRouteInfo;
        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = pstSession->m_stReqParam.m_szPlatForm;
        stRouteInfo.m_strSid = NumToString(pstSession->m_stReqParam.m_dwSvrId);
        stRouteInfo.m_strVs = pstSession->m_stReqParam.m_szVs;
        CStaticFileMgr::GetInstance()->GetMaintainJson(stRouteInfo, &stStaticFileInfo);

        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticDataMaintain::GetMaintainStatus(stRouteInfo, stStaticFileInfo.m_jDataJson[0]["data"]);
        if (EN_MAINTAIN_TYPE_NORMAL == dwMaintainStatus)
        {
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__2;
        }
        else
        {
            pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NEW_MAINTAIN);
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
    }

    // get account info from db
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {
        //get sid
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
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: VISITOR   Device:[%s]", strDevice.c_str()));
                tbProduct.Set_Device(strDevice);
                CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_DEVICE, CompareDesc(), false);
                break;
             case EN_LOGIN_TPYE_ACCOUNT:
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: ACCOUNT   Rid:[%s], Email:[%s]", strRid.c_str(), strEmail.c_str()));
                tbProduct.Set_Rid(strRid);
                CAwsRequest::Query(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_GLB_RID, CompareDesc(), false);
                tbUser.Set_Email(strEmail);
                CAwsRequest::Query(pstSession->m_vecAwsReq, &tbUser, ETbUSER_OPEN_TYPE_GLB_EMAIL, CompareDesc(), false);
                break;
            case EN_LOGIN_TPYE_DEBUG:
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: DEBUG    App_uid:[%ld]    R_pid[%s]", ddwUid, strRPid.c_str()));
                tbProduct.Set_App_uid(NumToString(ddwUid));
                tbProduct.Set_R_pid(strRPid);
                CAwsRequest::GetItem(pstSession->m_vecAwsReq, &tbProduct, ETbPRODUCT_OPEN_TYPE_PRIMARY);
                break;
            default:
                assert(0);
                break;
        }
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
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

    if (EN_COMMAND_STEP__3 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__4;
    }

    // handle account info
    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
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

              TSE_LOG_INFO(pstSession->m_poServLog, ("[kurotest]: tbl=%s rspp=%s [seq=%u] app_uid = %s",
              pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo, ptbProduct->m_sApp_uid.c_str()));
              continue;
          }
          if (strTableRawName == EN_AWS_TABLE_USER)
          {
              dwRetcode = CAwsResponse::OnQueryRsp(*pstAwsRspInfo, ptbUser, sizeof(TbUser), MAX_MAX_NUM);
              if (dwRetcode >= 0)
              {
                  pstUserInfo->m_dwUserNum = dwRetcode;

                  TSE_LOG_ERROR(pstSession->m_poServLog, ("[kurotest]: tbl=%s rspp=%s [seq=%u] email = %s",
                  pstAwsRspInfo->sTableName.c_str(), pstAwsRspInfo->sRspContent.c_str(), pstSession->m_udwSeqNo, ptbUser->m_sEmail.c_str()));
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

          for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwProductNum; dwIdx++)
          {
              if (strRPid == pstUserInfo->m_tbProduct[dwIdx].Get_R_pid()
                  && 0 == pstUserInfo->m_tbProduct[dwIdx].Get_Status()
                  && "0" != pstUserInfo->m_tbProduct[dwIdx].Get_Device())
              {
                  TSE_LOG_INFO(pstSession->m_poServLog, ("[kurotest] NEW GAME device:[%s]", pstUserInfo->m_tbProduct[dwIdx].Get_Device().c_str()));
                  vecTbProduct.push_back(&pstUserInfo->m_tbProduct[dwIdx]);
              }
          }
          
          if (0 != vecTbProduct.size())
          {
              pstSession->ResetAwsReq();
              for (TUINT32 dwIdx = 0; dwIdx < vecTbProduct.size(); dwIdx++)
              {
                  vecTbProduct[dwIdx]->Set_Device("0");
                  dwRetcode = CAwsRequest::UpdateItem(pstSession->m_vecAwsReq, vecTbProduct[dwIdx]);
                  if (dwRetcode != 0)
                  {
                      TSE_LOG_DEBUG(pstSession->m_poServLog, ("CProcessInit::requestHandler: not clear device [seq=%u]",
                          pstSession->m_udwSeqNo));
                  }
              }
              pstSession->m_udwCommandStep = EN_COMMAND_STEP__5;
              dwRetcode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
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
              pstSession->m_udwCommandStep = EN_COMMAND_STEP__6;
          }
      }
      else
      {
          pstSession->m_udwCommandStep = EN_COMMAND_STEP__6;
      }

    }

    if (EN_COMMAND_STEP__5 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__6;

    }

    if (EN_COMMAND_STEP__6 == pstSession->m_udwCommandStep)
    {
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
        TSE_LOG_INFO(pstSession->m_poServLog, ("CProcessInit::requestHandler: GetInitPlayerStatus [status=%ld] [seq=%u] [product_num=%d] [user_num=%d]",
            pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus, \
            pstSession->m_udwSeqNo, \
            pstSession->m_stUserInfo.m_dwProductNum, \
            pstSession->m_stUserInfo.m_dwUserNum));
        if (EN_PLAYER_STATUS_ERROR == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        //没有没机数据，导用户
        else if (EN_PLAYER_STATUS_IMPORT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stReqParam.m_dwSvrId = CGameSvrInfo::GetInstance()->GetNewPlayerSvr();
            pstSession->m_stReqParam.m_ddwUserId = 0;
        }
        else if (EN_PLAYER_STATUS_PASSWORD_CHANGE == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 40007;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        else if (EN_PLAYER_STATUS_BLACK_ACCOUNT == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 40008;
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
        if (EN_PLAYER_STATUS_MULTI_EMAIL == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
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
        else if (EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD == pstSession->m_stUserInfo.m_stUserStatus.m_ddwStatus)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        //UNREGISTER, UNACTIVE, ACTIVE
        else
        {
            pstSession->m_stReqParam.m_dwSvrId = pstSession->m_stUserInfo.m_stUserStatus.m_ddwSid;
            pstSession->m_stReqParam.m_ddwUserId = pstSession->m_stUserInfo.m_stUserStatus.m_ddwUid;

            if ("" != stLoginInfo.m_strProductInfo && NULL != ptbProduct)
            {
                pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
                bNeedResponse = TRUE;
                pstSession->ResetAwsReq();

                ptbProduct->Set_App_uid(NumToString(pstSession->m_stReqParam.m_ddwUserId));
                ptbProduct->Set_R_pid(strRPid);
                ptbProduct->Set_Product_info(stLoginInfo.m_strProductInfo);

                dwRetCode = CAwsRequest::UpdateItem(pstSession->m_vecAwsReq, ptbProduct);
                if (dwRetCode == 0)
                {
                    dwRetCode = CBaseProcedure::SendAwsRequest(pstSession, EN_SERVICE_TYPE_QUERY_DYNAMODB_REQ);
                    if (dwRetCode < 0)
                    {
                        pstSession->m_stCommonResInfo.m_dwRetCode = EN_RET_CODE__SEND_FAIL;
                        TSE_LOG_ERROR(pstSession->m_poServLog, ("CProcessInit::requestHandler: send req failed [seq=%u]", pstSession->m_udwSeqNo));
                        return -3;
                    }
                    pstSession->m_udwCommandStep = EN_COMMAND_STEP__7;
                    return 0;
                }
            }
            if (dwRetCode != pstSession->m_stReqParam.m_dwSvrId)
            {
                pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;
            }
            else
            {
                pstSession->m_udwCommandStep = EN_COMMAND_STEP__9;
            }
        }
    }

    if (EN_COMMAND_STEP__7 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
        if (dwreqSid != pstSession->m_stReqParam.m_dwSvrId)
        {
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;
        }
        else
        {
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__9;
        }
    }

    //server maintain
    if (EN_COMMAND_STEP__8 == pstSession->m_udwCommandStep)
    {
        SStaticFileInfo stStaticFileInfo;
        stStaticFileInfo.Reset();
        SRouteInfo stRouteInfo;
        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = pstSession->m_stReqParam.m_szPlatForm;
        stRouteInfo.m_strSid = NumToString(pstSession->m_stReqParam.m_dwSvrId);
        stRouteInfo.m_strVs = pstSession->m_stReqParam.m_szVs;
        CStaticFileMgr::GetInstance()->GetMaintainJson(stRouteInfo, &stStaticFileInfo);
        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticDataMaintain::GetMaintainStatus(stRouteInfo, stStaticFileInfo.m_jDataJson[0]["data"]);
        if (EN_MAINTAIN_TYPE_NORMAL == dwMaintainStatus)
        {
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__9;
        }
        else
        {
            pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NEW_MAINTAIN);
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
    }

    //数据打包
    if (EN_COMMAND_STEP__9 == pstSession->m_udwCommandStep)
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
