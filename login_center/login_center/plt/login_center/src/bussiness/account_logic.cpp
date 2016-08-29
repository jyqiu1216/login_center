#include "account_logic.h"
#include "game_svr.h"


TBOOL CAccountLogic::Compare_Uid(TbProduct *ptbProductA, TbProduct *ptbProductB)
{
    return strtol(ptbProductA->Get_App_uid().c_str(), NULL, 10) < strtol(ptbProductB->Get_App_uid().c_str(), NULL, 10);
}




TINT32 CAccountLogic::GetInitPlayerStatus(SUserInfo *pstUserInfo, const SLoginInfo &stLoginInfo, TbProduct *&ptbProduct)
{
    TSE_LOG_DEBUG(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: rid=%s email=%s passwd=%s thid=%s device=%s rpid=%s logintype=%d",
        stLoginInfo.m_strRid.c_str(), stLoginInfo.m_strEmail.c_str(), stLoginInfo.m_strPasswd.c_str(), \
        stLoginInfo.m_strThId.c_str(), stLoginInfo.m_strDevice.c_str(), stLoginInfo.m_strRPid.c_str(), pstUserInfo->m_dwLoginTpye));

    if (EN_LOGIN_TPYE_DEBUG == pstUserInfo->m_dwLoginTpye)
    {
        if (0 == pstUserInfo->m_dwRidProductNum)
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_USER_DATA_NOT_EXIST;
            TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
            return 0;
        }
        else
        {
            // 设置账号状态
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_UNREGISTER;

            // 设置账号数据
            ptbProduct = &pstUserInfo->m_tbRidProduct;
        }
    }
    else if (EN_LOGIN_TPYE_ACCOUNT == pstUserInfo->m_dwLoginTpye)// 帐号登录的情况
    {
        if (pstUserInfo->m_dwUserNum > 1)
        {
            // 同一邮箱注册了多次
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_MULTI_EMAIL;
            TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
            return 0;
        }
        else if (1 == pstUserInfo->m_dwUserNum)
        {
            // 帐号信息不一致
            if (pstUserInfo->m_tbUserNew.Get_Email() != stLoginInfo.m_strEmail)
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD;
                TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
                return 0;
            }

            // 帐号密码不一致
            if ("null" != stLoginInfo.m_strPasswd)
            {
                if (pstUserInfo->m_tbUserNew.Get_Passwd() != stLoginInfo.m_strPasswd)
                {
                    pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD;
                    TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
                    return 0;
                }
            }


            // 帐号密码已重置
            if (1 == pstUserInfo->m_tbUserNew.Get_Pwd_flag())
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PASSWORD_CHANGE;
                TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
                return 0;
            }

            // 帐号受限（帐号被封）
            if (1 == pstUserInfo->m_tbUserNew.Get_Status())
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_BLACK_ACCOUNT;
                TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
                return 0;
            }

            // product数据没有
            if (pstUserInfo->m_dwRidProductNum == 0)
            {
                pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_USER_DATA_NOT_EXIST;
                TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
                return 0;
            }

            //设置账号状态
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_ACTIVE;

            //选取账号数据
            ptbProduct = &pstUserInfo->m_tbRidProduct;
        }
        else
        {
            // 设置账号状态
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_ACCOUNT_UNACTIVE;

            // 选取product数据
            for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwDeviceProductNum; ++dwIdx)
            {
                if (stLoginInfo.m_strDevice == pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Device()
                    && "" == pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Rid())
                {
                    ptbProduct = &pstUserInfo->m_atbDeviceProduct[dwIdx];
                    break;
                }
            }
        }
    }
    else // EN_LOGIN_TPYE_VISTOR == pstUserInfo->m_dwLoginTpye
    {
        // 设置账号状态
        pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_UNREGISTER;

        // 选取product数据
        for (TINT32 dwIdx = 0; dwIdx < pstUserInfo->m_dwDeviceProductNum; ++dwIdx)
        {
            if (stLoginInfo.m_strRPid == pstUserInfo->m_atbDeviceProduct[dwIdx].Get_R_pid()
                && "" == pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Rid()
                && "0" != pstUserInfo->m_atbDeviceProduct[dwIdx].Get_Device())
            {
                ptbProduct = &pstUserInfo->m_atbDeviceProduct[dwIdx];
                break;
            }
        }
    }


    // 如果没有账号数据，则为新用户
    if (NULL == ptbProduct)
    {
        pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_IMPORT;
        TSE_LOG_INFO(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
        return 0;
    }
    else
    {
        Json::Reader reader;
        Json::Value jProductInfo;
        if (false == reader.parse(ptbProduct->Get_Product_info().c_str(), jProductInfo))
        {
            pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD;
            TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: status=%ld", pstUserInfo->m_stUserStatus.m_ddwStatus));
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
            }

            pstUserInfo->m_stUserStatus.m_ddwUid = strtol(ptbProduct->Get_App_uid().c_str(), NULL, 10);
            pstUserInfo->m_stUserStatus.m_ddwSid = ptbProduct->Get_Sid();
        }
    }

    TSE_LOG_ERROR(CGameSvrInfo::GetInstance()->m_poServLog, ("[wavetest] GetInitPlayerStatus: type=%d user_num=%d product_rid_num=%d product_device_num=%d, res---uid=%ld, sid=%ld, status=%ld", \
        pstUserInfo->m_dwLoginTpye, pstUserInfo->m_dwUserNum, pstUserInfo->m_dwRidProductNum, pstUserInfo->m_dwDeviceProductNum, \
        pstUserInfo->m_stUserStatus.m_ddwUid, pstUserInfo->m_stUserStatus.m_ddwSid, pstUserInfo->m_stUserStatus.m_ddwStatus));

    return 0;
}



TINT32 CAccountLogic::GetUpdatePlayerStatus(SUserInfo *pstUserInfo, const SLoginInfo &stLoginInfo)
{

    if (0 == pstUserInfo->m_dwRidProductNum)
    {
        pstUserInfo->m_stUserStatus.m_ddwStatus = EN_PLAYER_STATUS_IMPORT;
        return 0;
    }
    else
    {
        Json::Reader reader;
        Json::Value jProductInfo;
        if (false == reader.parse(pstUserInfo->m_tbRidProduct.Get_Product_info().c_str(), jProductInfo))
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
        if (1 == pstUserInfo->m_tbUserNew.Get_Pwd_flag())
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
