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
    TINT64 strUid = pstSession->m_stReqParam.m_ddwUserId;
    TINT32 strSid = pstSession->m_stReqParam.m_dwSvrId;

    SUserInfo *pstUserInfo = &pstSession->m_stUserInfo;
    TbProduct *ptbProduct = pstUserInfo->m_tbProduct;
    TbUser *ptbUser = pstUserInfo->m_tbUser;

    TSE_LOG_DEBUG(pstSession->m_poServLog, ("[kurotest]Param: Rid = %s Email = %s Password = %s ThId = %s NewGame = %d Device = %s Rpid = %s Uid = %ld Sid = %d",
        strRid.c_str(), strEmail.c_str(), strPasswd.c_str(), strThId.c_str(), dwNewGameFlag, strDevice.c_str(), strRPid.c_str(), strUid, strSid));

    //for server maintain
    TINT32 reqSid = pstSession->m_stReqParam.m_dwSvrId;

    if (pstSession->m_udwCommandStep == EN_COMMAND_STEP__INIT)
    {
        if (-1 != strSid && strSid > CGameSvrInfo::GetInstance()->m_udwSvrNum - 1)
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

    // get account info
    if (EN_COMMAND_STEP__2 == pstSession->m_udwCommandStep)
    {
        //get sid
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__3;
        pstSession->m_udwExpectProcedure = EN_EXPECT_PROCEDURE__AWS;
        bNeedResponse = TRUE;
        pstSession->ResetAwsInfo();

        //login type
        if (0 != strUid && -1 == strSid)
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
                TSE_LOG_DEBUG(pstSession->m_poServLog, ("TYPE: DEBUG    App_uid:[%l]    R_pid[%s]", strUid, strRPid.c_str()));
                tbProduct.Set_App_uid(NumToString(strUid));
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

    // handle account info
    if (EN_COMMAND_STEP__3 == pstSession->m_udwCommandStep)
    {
       if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
      {
          pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
          pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
          return 0;
      }

      pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;
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
    }

    if (EN_COMMAND_STEP__4 == pstSession->m_udwCommandStep)
    {
        if (EN_RET_CODE__SUCCESS != pstSession->m_stCommonResInfo.m_dwRetCode)
        {
            pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }

        pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;

    }

    //server maintain
    if (EN_COMMAND_STEP__7 == pstSession->m_udwCommandStep)
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
        // 分服mintain
        TINT32 dwMaintainStatus = EN_MAINTAIN_TYPE_NORMAL;
        dwMaintainStatus = CStaticDataMaintain::GetMaintainStatus(stRouteInfo, stStaticFileInfo.m_jDataJson[0]["data"]);
        if (EN_MAINTAIN_TYPE_NORMAL == dwMaintainStatus)
        {
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__8;
        }
        else
        {
            pstSession->m_stCommonResInfo.m_vecResultStaticFileList.push_back(EN_STATIC_TYPE_NEW_MAINTAIN);
            pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
            return 0;
        }
    }

    //数据打包
    if (EN_COMMAND_STEP__8 == pstSession->m_udwCommandStep)
    {
        /*
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
        */
        pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
        return 0;
     }

    pstSession->m_stCommonResInfo.m_dwRetCode = 10000;
    pstSession->m_udwCommandStep = EN_COMMAND_STEP__END;
    return 0;
}
