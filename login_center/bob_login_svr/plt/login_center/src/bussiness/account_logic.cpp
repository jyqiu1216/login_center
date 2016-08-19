#include "account_logic.h"
#include "game_svr.h"


TBOOL CAccountLogic::Compare_Uid(TbProduct *ptbProductA, TbProduct *ptbProductB)
{
    return strtol(ptbProductA->Get_App_uid().c_str(), NULL, 10) < strtol(ptbProductB->Get_App_uid().c_str(), NULL, 10);
}




TINT32 CAccountLogic::GetInitPlayerStatus(SUserInfo *pstUserInfo, const SLoginInfo &stLoginInfo, TbProduct *&ptbTmpProduct)
{


    TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: rid=%s email=%s passwd=%s thid=%s device=%s rpid=%s ",
        stLoginInfo.m_strRid.c_str(), stLoginInfo.m_strEmail.c_str(), stLoginInfo.m_strPasswd.c_str(), \
        stLoginInfo.m_strThId.c_str(), stLoginInfo.m_strDevice.c_str(), stLoginInfo.m_strRPid.c_str()));

    TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: type=%d user_num=%d product_num=%d staus=%ld ",
        pstUserInfo->m_dwLoginTpye, pstUserInfo->m_dwUserNum, pstUserInfo->m_dwProductNum, \
        pstUserInfo->m_stUserStatus.m_ddwStatus));

    if (EN_LOGIN_TPYE_DEBUG == pstUserInfo->m_dwLoginTpye)
    {
        Json::Reader reader;
        Json::Value jProductInfo;
        if (false == reader.parse(pstUserInfo->m_tbProduct[0].Get_Product_info().c_str(), jProductInfo))
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD;
            return 0;
        }
        else
        {
            Json::FastWriter rWriter;
            rWriter.omitEndingLineFeed();
            jProductInfo["device"] = stLoginInfo.m_strDevice;
            jProductInfo["idfa"] = stLoginInfo.m_strDevice;
            stLoginInfo.m_strProductInfo = rWriter.write(jProductInfo);
            ptbTmpProduct = &pstUserInfo->m_tbProduct[0];

            // 默认帐号没有注册(debug情况下帐号状态不重要)
            pstUserInfo->m_stUserStatus.m_ddwUid = strtol(ptbTmpProduct->Get_App_uid().c_str(), NULL, 10);
            pstUserInfo->m_stUserStatus.m_ddwSid = ptbTmpProduct->Get_Sid();
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_UNREGISTER;
            return 0;
        }
    }


    // 帐号登录的情况
    if (EN_LOGIN_TPYE_ACCOUNT == pstUserInfo->m_dwLoginTpye)
    {
        vector<TbProduct *> vecTbProduct;
        vecTbProduct.clear();


        

        if (pstUserInfo->m_dwUserNum > 1)
        {
            // 同一邮箱注册了多次
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_MULTI_EMAIL;
            return 0;
        }
        else if (1 == pstUserInfo->m_dwUserNum)
        {
            // 帐号信息不一致
            if (pstUserInfo->m_tbUser[0].Get_Rid() != stLoginInfo.m_strRid)
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD;
                return 0;
            }

            // 帐号密码不一致
            if ("null" != stLoginInfo.m_strPasswd)
            {
                if (pstUserInfo->m_tbUser[0].Get_Passwd() != stLoginInfo.m_strPasswd)
                {
                    pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD;
                    return 0;
                }
            }


            // 帐号密码已重置
            if (1 == pstUserInfo->m_tbUser[0].Get_Pwd_flag())
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PASSWORD_CHANGE;
                return 0;
            }

            // 帐号受限（帐号被封）
            if (1 == pstUserInfo->m_tbUser[0].Get_Status())
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_BLACK_ACCOUNT;
                return 0;
            }

            // 帐号没有本产品的游戏数据
            vecTbProduct.clear();
            for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwProductNum; ++dwIdx)
            {
                if (stLoginInfo.m_strRid == pstUserInfo->m_tbProduct[dwIdx].Get_Rid()
                    && stLoginInfo.m_strRPid == pstUserInfo->m_tbProduct[dwIdx].Get_R_pid()
                    && "0" != pstUserInfo->m_tbProduct[dwIdx].Get_Device())
                {
                    vecTbProduct.push_back(&pstUserInfo->m_tbProduct[dwIdx]);
                }
            }
            if (0 == vecTbProduct.size())
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_NOT_GAME_DATA;
                return 0;
            }
            // 同一个邮箱绑定当前的产品的多份游戏数据
            else if (vecTbProduct.size() > 1)
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_EMAIL_HAS_MULTI_GAME_DATA;
                return 0;
            }
            else
            {
                Json::Reader reader;
                Json::Value jProductInfo;
                if (false == reader.parse(vecTbProduct[0]->Get_Product_info().c_str(), jProductInfo))
                {
                    pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD;
                    return 0;
                }
                else
                {
                    if (stLoginInfo.m_strDevice != jProductInfo["device"].asString())
                    {
                        Json::FastWriter rWriter;
                        rWriter.omitEndingLineFeed();
                        jProductInfo["device"] = stLoginInfo.m_strDevice;
                        jProductInfo["idfa"] = stLoginInfo.m_strDevice;
                        stLoginInfo.m_strProductInfo = rWriter.write(jProductInfo);
                        ptbTmpProduct = vecTbProduct[0];
                        /*
                        if (EN_CLIENT_REQ_COMMAND__INIT != udwCommandID)
                        {
                            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_MULTI_LOGIN;
                            return 0;
                        }
                        */
                    }
                    
                    
                    // 帐号激活
                    pstUserInfo->m_stUserStatus.m_ddwUid = strtol(vecTbProduct[0]->Get_App_uid().c_str(), NULL, 10);
                    pstUserInfo->m_stUserStatus.m_ddwSid = vecTbProduct[0]->Get_Sid();
                    pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_ACTIVE;
                    return 0;
                    
                }
            }
        }
        else
        {
            // 帐号未激活
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_UNACTIVE;
        }
    }


    if (EN_LOGIN_TPYE_VISTOR == pstUserInfo->m_dwLoginTpye
        || pstUserInfo->m_stUserStatus.m_ddwStatus == EN_PLAYER_STATUS_ACCOUNT_UNACTIVE)
    {
        vector<TbProduct *> vecTbProduct;
        vecTbProduct.clear();
        TbProduct *ptbProduct = NULL;
        for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwProductNum; ++dwIdx)
        {
            if (stLoginInfo.m_strRPid == pstUserInfo->m_tbProduct[dwIdx].Get_R_pid()
                && 0 == pstUserInfo->m_tbProduct[dwIdx].Get_Status()
                && "0" != pstUserInfo->m_tbProduct[dwIdx].Get_Device())
            {
                vecTbProduct.push_back(&pstUserInfo->m_tbProduct[dwIdx]);
            }
        }

        // 没有本机数据,导用户
        if (0 == vecTbProduct.size())
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_IMPORT;
            return 0;
        }
        else if (1 == vecTbProduct.size())
        {
            ptbProduct = vecTbProduct[0];
        }
        else
        {
            std::sort(vecTbProduct.begin(), vecTbProduct.end(), Compare_Uid);
            ptbProduct = vecTbProduct[0];
        }


        Json::Reader reader;
        Json::Value jProductInfo;
        if (false == reader.parse(ptbProduct->Get_Product_info().c_str(), jProductInfo))
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD;
            return 0;
        }
        else
        {
            if (stLoginInfo.m_strDevice != jProductInfo["device"].asString())
            {
                Json::FastWriter rWriter;
                rWriter.omitEndingLineFeed();
                jProductInfo["device"] = stLoginInfo.m_strDevice;
                jProductInfo["idfa"] = stLoginInfo.m_strDevice;
                stLoginInfo.m_strProductInfo = rWriter.write(jProductInfo);
                ptbTmpProduct = ptbProduct;
                
                /*
                if (EN_CLIENT_REQ_COMMAND__INIT != udwCommandID)
                {
                    pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_MULTI_LOGIN;
                    return 0;
                }
                */
            }
        }

        if (EN_PLAYER_STATUS_ACCOUNT_UNACTIVE == pstUserInfo->m_stUserStatus.m_ddwStatus)
        {
            pstUserInfo->m_stUserStatus.m_ddwUid = strtol(ptbProduct->Get_App_uid().c_str(), NULL, 10);
            pstUserInfo->m_stUserStatus.m_ddwSid = ptbProduct->Get_Sid();
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_UNACTIVE;
            return 0;
        }
        else
        {
            pstUserInfo->m_stUserStatus.m_ddwUid = strtol(ptbProduct->Get_App_uid().c_str(), NULL, 10);
            pstUserInfo->m_stUserStatus.m_ddwSid = ptbProduct->Get_Sid();
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_UNREGISTER;
            return 0;
        }
    }
    return EN_PLAYER_STATUS_ERROR;
}



TINT32 CAccountLogic::GetUpdatePlayerStatus(SUserInfo *pstUserInfo, const SLoginInfo &stLoginInfo)
{

    if (0 == pstUserInfo->m_dwProductNum)
    {
        pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_USER_DATA_NOT_EXIST;
        return 0;
    }
    else
    {
        Json::Reader reader;
        Json::Value jProductInfo;
        if (false == reader.parse(pstUserInfo->m_tbProduct[0].Get_Product_info().c_str(), jProductInfo))
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD;
            return 0;
        }
        else
        {
            if (stLoginInfo.m_strDevice != jProductInfo["device"].asString())
            {
  
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_MULTI_LOGIN;
                return 0;
            }
        }
    }

    // 必有游戏数据
    if (0 == pstUserInfo->m_dwUserNum)
    {
        if ("" == stLoginInfo.m_strRid)
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_UNREGISTER;
            return 0;
        }
        else
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_UNACTIVE;
            return 0;
        }
    }
    else
    {
        if (1 == pstUserInfo->m_tbUser[0].Get_Pwd_flag())
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PASSWORD_CHANGE;
            return 0;
        }
        else
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_ACTIVE;
            return 0;
        }
    }

    return 0;
}
