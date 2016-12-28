#include "static_file_mgr.h"
#include "time_utils.h"
#include "conf_base.h"

CStaticFileMgr *CStaticFileMgr::m_poStaticFileMgr = NULL;


CStaticFileMgr::CStaticFileMgr()
{
}

CStaticFileMgr::~CStaticFileMgr()
{
   
}

CStaticFileMgr * CStaticFileMgr::GetInstance()
{
    if (NULL == m_poStaticFileMgr)
    {
        try
        {
            m_poStaticFileMgr = new CStaticFileMgr;
        }
        catch (const std::bad_alloc &memExp)
        {
            assert(m_poStaticFileMgr);
        }
    }
    return m_poStaticFileMgr;

}

TINT32 CStaticFileMgr::Init(CTseLogger *poLog)
{
    TINT32 dwRet = 0;

    m_poServLog = poLog;

    InitStaticFileProperty();

    PreprocessStaticFile_Md5(m_jsonMd5);

    dwRet = LoadStaticFileList();
    if(dwRet != 0)
    {
        return -1;
    }

    return 0;
}

TINT32 CStaticFileMgr::UnInit()
{
    map<string, CStaticFileContent*>::iterator it;
    for(it = m_mapStaticFileContent.begin(); it != m_mapStaticFileContent.end(); it++)
    {
        delete it->second;
    }
    return 0;
}

TINT32 CStaticFileMgr::CheckUpdate()
{
    CStaticFileMgr *pobjMgr = new CStaticFileMgr();
    TINT32 dwRet = pobjMgr->Init(m_poServLog);
    if(dwRet != 0)
    {
        pobjMgr->UnInit();
        delete pobjMgr;
        return -1;
    }

    CStaticFileMgr *pobjTmp = m_poStaticFileMgr;
    m_poStaticFileMgr = pobjMgr;

    if(pobjTmp)
    {
        usleep(50000);
        pobjTmp->UnInit();
        delete pobjTmp;
    }

    return 0;
}

TINT32 CStaticFileMgr::LoadStaticFileList()
{
    m_mapStaticFileContent.clear();

    TINT32 dwRet = 0;
    Json::Value jDataListJson;
    dwRet = LoadNewDataListJson(jDataListJson);
    if(dwRet < 0)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, LoadNewDataListJson failed."));
        return -1;
    }

    Json::Value::Members jMemberType = jDataListJson.getMemberNames();
    for (TUINT32 udwIdx = 0; udwIdx < jMemberType.size(); ++udwIdx)
    {
        if ("globalconfig" == jMemberType[udwIdx])
        {
            Json::Value::Members jMemberPlatform = jDataListJson["globalconfig"].getMemberNames();
            for (TUINT32 udwIdy = 0; udwIdy < jMemberPlatform.size(); ++udwIdy)
            {
                Json::Value::Members jMemberFile = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]].getMemberNames();
                for (TUINT32 udwIdz = 0; udwIdz < jMemberFile.size(); ++udwIdz)
                {
                    CStaticFileContent *pobjContent = new CStaticFileContent;

                    // property
                    pobjContent->m_strFileName = jMemberFile[udwIdz];
                    pobjContent->m_strStaticFilePath = "../data" + jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["path"].asString();
                    pobjContent->m_ucReloadFlag = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["reload"].asUInt();
                    pobjContent->m_strStaticDataType = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["type"].asString();
                    pobjContent->m_udwFileTs = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["timestamp"].asUInt();
                    pobjContent->m_strMd5Raw = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["md5"].asString();
                    pobjContent->m_strVersion = "";
                    pobjContent->m_strPlatForm = jMemberPlatform[udwIdy];
                    pobjContent->m_strRealName = GetFileRealName(pobjContent->m_strStaticDataType, pobjContent->m_strPlatForm, pobjContent->m_strVersion);
                    pobjContent->m_udwFileSize = GetFileSize(pobjContent->m_strStaticFilePath);
                    

                    if(IsFileTypeExist(pobjContent->m_strStaticDataType) == FALSE)
                    {
                        TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, do not need load[%s][%s].", pobjContent->m_strFileName.c_str(), pobjContent->m_strRealName.c_str()));
                        delete pobjContent;
                        continue;
                    }

                    // load json
                    dwRet = LoadStaticFile(pobjContent, pobjContent->m_strPlatForm, pobjContent->m_strVersion);
                    if(dwRet != 0)
                    {
                        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, LoadStaticFile[%s][%s] failed[%d].", pobjContent->m_strFileName.c_str(), pobjContent->m_strRealName.c_str(), dwRet));
                        delete pobjContent;
                        break;
                    }
                    else
                    {
                        TSE_LOG_INFO(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, LoadStaticFile[%s][%s] success", pobjContent->m_strFileName.c_str(), pobjContent->m_strRealName.c_str()));
                    }

                    // insert to map
                    assert(pobjContent->m_strRealName.length() > 0);
                    m_mapStaticFileContent.insert(make_pair(pobjContent->m_strRealName, pobjContent));
                }
            }
        }
        else if ("versionconfig" == jMemberType[udwIdx])
        {
            Json::Value::Members jMemberVersion = jDataListJson["versionconfig"].getMemberNames();
            for (TUINT32 udwIdy = 0; udwIdy < jMemberVersion.size(); ++udwIdy)
            {
                if ("new" == jMemberVersion[udwIdy])
                {
                    continue;
                }

                Json::Value::Members jMemberPlatform = jDataListJson["versionconfig"][jMemberVersion[udwIdy]].getMemberNames();
                for (TUINT32 udwIdz = 0; udwIdz < jMemberPlatform.size(); ++udwIdz)
                {
                    Json::Value::Members jMemberFile = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]].getMemberNames();
                    for (TUINT32 udwIdm = 0; udwIdm < jMemberFile.size(); ++udwIdm)
                    {
                        CStaticFileContent *pobjContent = new CStaticFileContent;

                        //property
                        pobjContent->m_strFileName = jMemberFile[udwIdm];
                        pobjContent->m_strStaticFilePath = "../data" + jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["path"].asString();
                        pobjContent->m_ucReloadFlag = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["reload"].asUInt();
                        pobjContent->m_strStaticDataType = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["type"].asString();
                        pobjContent->m_udwFileTs = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["timestamp"].asUInt();
                        pobjContent->m_strMd5Raw = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["md5"].asString();
                        pobjContent->m_strVersion = jMemberVersion[udwIdy];
                        pobjContent->m_strPlatForm = jMemberPlatform[udwIdz];
                        pobjContent->m_strRealName = GetFileRealName(pobjContent->m_strStaticDataType, pobjContent->m_strPlatForm, pobjContent->m_strVersion);
                        pobjContent->m_udwFileSize = GetFileSize(pobjContent->m_strStaticFilePath);

                        if(IsFileTypeExist(pobjContent->m_strStaticDataType) == FALSE)
                        {
                            TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, do not need load[%s][%s].", pobjContent->m_strFileName.c_str(), pobjContent->m_strRealName.c_str()));
                            delete pobjContent;
                            continue;
                        }
                        
                        // load json
                        dwRet = LoadStaticFile(pobjContent, pobjContent->m_strPlatForm, pobjContent->m_strVersion);
                        if(dwRet != 0)
                        {
                            TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, LoadStaticFile[%s][%s] failed[%d].", pobjContent->m_strFileName.c_str(), pobjContent->m_strRealName.c_str(), dwRet));
                            delete pobjContent;
                            break;
                        }
                        else
                        {
                            TSE_LOG_INFO(m_poServLog, ("CStaticFileMgr::LoadStaticFileList, LoadStaticFile[%s][%s] success", pobjContent->m_strFileName.c_str(), pobjContent->m_strRealName.c_str()));
                        }

                        // insert to map
                        assert(pobjContent->m_strRealName.length() > 0);
                        m_mapStaticFileContent.insert(make_pair(pobjContent->m_strRealName, pobjContent));
                    }
                }
            }
        }
        else
        {
            // do nothing
        }
    }
    return dwRet;
}

TINT32 CStaticFileMgr::LoadNewDataListJson(Json::Value &jNewDataListJson)
{
    jNewDataListJson.clear();

    Json::Reader reader;
    std::ifstream is;


    is.open(DATA_LIST_JSON);
    if (reader.parse(is, jNewDataListJson) == false)
    {
        is.close();
        return -1;
    }
    return 0;
}

TINT32 CStaticFileMgr::LoadStaticFile(CStaticFileContent *pobjStaticFile, string strPlatForm, string strVersion)
{
    pobjStaticFile->m_jsonContent.clear();

    TBOOL bNeedLoadData = IsFIleTypeNeedLoadData(pobjStaticFile->m_strStaticDataType);
    if (bNeedLoadData)
    {
        std::ifstream is;
        Json::Reader reader;
        is.open(pobjStaticFile->m_strStaticFilePath.c_str());
        if (false == reader.parse(is, pobjStaticFile->m_jsonContent))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }
    }

    //对数据进行预处理——如果需要
    PreprocessStaticFile(pobjStaticFile->m_jsonContent, pobjStaticFile->m_strStaticDataType);

    //计算md5
    if (bNeedLoadData)
    {
        Json::FastWriter m_jsonWriter;
        m_jsonWriter.omitEndingLineFeed();
        string strData = m_jsonWriter.write(pobjStaticFile->m_jsonContent);
        pobjStaticFile->m_strMd5 = NumToString(GetStringMD5(strData));
    }
    else
    {
        pobjStaticFile->m_strMd5 = pobjStaticFile->m_strMd5Raw;
    }
    

    //更新md5
    if (IsFileTypeNeedMd5(pobjStaticFile->m_strStaticDataType))
    {
        if (strVersion == "")
        {
            UpdateMd5Json(m_jsonMd5, pobjStaticFile->m_strStaticDataType, pobjStaticFile->m_strMd5, pobjStaticFile->m_ucReloadFlag);
        }
        else
        {
            UpdateMd5Json_Version(m_jsonMd5_version, pobjStaticFile->m_strStaticDataType, pobjStaticFile->m_strMd5, pobjStaticFile->m_ucReloadFlag, strPlatForm, strVersion);
        }
    }    

    return 0;
}

CStaticFileContent* CStaticFileMgr::GetStaticFile( string strFileType, string strPlatForm, string strVersion )
{
    CStaticFileContent* pobjContent = NULL;
    string strFileName = GetFileRealName(strFileType, strPlatForm, strVersion);
    map<string, CStaticFileContent*>::iterator it = m_mapStaticFileContent.find(strFileName);
    if(it != m_mapStaticFileContent.end())
    {
        pobjContent = it->second;
    }
    return pobjContent;
}


TINT32 CStaticFileMgr::InitStaticFileProperty()
{
    m_mapStaticFileProperty.clear();

    // global
    AddStaticFileProperty(EN_STATIC_TYPE_SVRCONF, FALSE, FALSE);
    AddStaticFileProperty(EN_STATIC_TYPE_CLIENTCONF,FALSE, FALSE);
    AddStaticFileProperty(EN_STATIC_TYPE_FUNCTION_SWITCH, FALSE, FALSE, TRUE, FALSE);
    AddStaticFileProperty(EN_STATIC_TYPE_USER_LINK, FALSE, FALSE);
    AddStaticFileProperty(EN_STATIC_TYPE_NEW_MAINTAIN, TRUE, FALSE);

    // version
    AddStaticFileProperty(EN_STATIC_TYPE_META, TRUE, TRUE);
    AddStaticFileProperty(EN_STATIC_TYPE_META_RAW, TRUE, TRUE, TRUE, FALSE);

    AddStaticFileProperty(EN_STATIC_TYPE_GUIDE, TRUE, TRUE);

    AddStaticFileProperty(EN_STATIC_TYPE_DOC, TRUE, TRUE, FALSE);
    AddStaticFileProperty(EN_STATIC_TYPE_GAME, TRUE, TRUE, FALSE);
    AddStaticFileProperty(EN_STATIC_TYPE_EQUIP, TRUE, TRUE, FALSE);   

    return 0;
}

TINT32 CStaticFileMgr::AddStaticFileProperty(string strFileType, TBOOL bDiffPlatForm, TBOOL bDiffVersion, TBOOL bNeedLoadData, TBOOL bNeedMd5)
{
    SStaticFileProperty stPro;
    stPro.m_bDiffPlatForm = bDiffPlatForm;
    stPro.m_bDiffVersion = bDiffVersion;
    stPro.m_bNeedLoadData = bNeedLoadData;
    stPro.m_bNeedMd5 = bNeedMd5;
    m_mapStaticFileProperty.insert(make_pair(strFileType, stPro));
    return 0;
}

string CStaticFileMgr::GetFileRealName( string strFileType, string strPlatForm, string strVersion )
{
    string strFileRealName;
    map<string, SStaticFileProperty>::iterator it = m_mapStaticFileProperty.find(strFileType);
    if(it == m_mapStaticFileProperty.end())
    {
        return "";
    }

    if(it->second.m_bDiffVersion == false)
    {
        if(it->second.m_bDiffPlatForm)
        {
            strFileRealName = "global_" + strPlatForm + "_" + strFileType;
        }
        else
        {
            strFileRealName = "global_common_" + strFileType;
        }
    }
    else
    {
        if(it->second.m_bDiffPlatForm)
        {
            strFileRealName = "version_" + strPlatForm + "_" + strFileType + "_" + strVersion;
        }
        else
        {
            strFileRealName = "version_common_" + strFileType + "_" + strVersion;
        }
    }
    return strFileRealName;
}

TINT32 CStaticFileMgr::UpdtAndGetMaintainStatus( Json::Value &jMaintainJson, string strPlatForm, string strVersion, string strSid, TUINT32 udwCurTime, string strDevice)
{
    m_Mutex.acquire();

    TUINT32 udwMaintainBeg = 0;
    TUINT32 udwMaintainEnd = 0;
    TINT32 dwRetStatus = EN_MAINTAIN_TYPE_NORMAL;
    TINT32 dwIsWhiteUid = 0;

    // global——修正url
    if ("android" == strPlatForm)
    {
        jMaintainJson["global"]["url"] = jMaintainJson["global"]["googleplay_url"].asString();
    }
    else
    {
        //do nothing
    }

    //判断是否为白名单
    dwIsWhiteUid = IsWhiteAccount(strDevice);

    // global——强制更新
    TINT32 dwGlobalUpdt = EN_MAINTAIN_TYPE_NORMAL;
    TFLOAT32 fCurVersion = atof(strVersion.c_str()) + 0.001f;
    if (fCurVersion < jMaintainJson["global"]["min_ver"].asFloat())
    {
        dwGlobalUpdt = EN_MAINTAIN_TYPE_VERSION_FORCE_UPDATE;
    }
    else if (fCurVersion < jMaintainJson["global"]["max_ver"].asFloat())
    {
        dwGlobalUpdt = EN_MAINTAIN_TYPE_VERSION_ADVICE_UPDATE;
    }
    else
    {
        dwGlobalUpdt = EN_MAINTAIN_TYPE_NORMAL;
    }

    // global——maintain
    TINT32 dwGlobalMaintain = EN_MAINTAIN_TYPE_NORMAL;
    udwMaintainBeg = jMaintainJson["global"]["begin"].asUInt();
    udwMaintainEnd = jMaintainJson["global"]["end"].asUInt();
    if (udwCurTime > udwMaintainEnd)
    {
        dwGlobalMaintain = EN_MAINTAIN_TYPE_NORMAL;
    }
    else if (udwMaintainBeg <= udwCurTime && udwCurTime <= udwMaintainEnd)
    {
        //jMaintainJson["global"]["status"] = EN_MAINTAIN_TYPE_MAINTAINING;
        dwGlobalMaintain = EN_MAINTAIN_TYPE_MAINTAINING;
    }
    else
    {
        dwGlobalMaintain = EN_MAINTAIN_TYPE_TO_MAINTAIN;
    }

    // global status
    if (dwGlobalUpdt == EN_MAINTAIN_TYPE_VERSION_FORCE_UPDATE)
    {
        jMaintainJson["global"]["status"] = dwGlobalUpdt;
    }
    else if (dwGlobalMaintain == EN_MAINTAIN_TYPE_MAINTAINING)
    {
        jMaintainJson["global"]["status"] = dwGlobalMaintain;
    }
    else if (dwGlobalMaintain == EN_MAINTAIN_TYPE_TO_MAINTAIN)
    {
        jMaintainJson["global"]["status"] = dwGlobalMaintain;
    }
    else if (dwGlobalUpdt == EN_MAINTAIN_TYPE_VERSION_ADVICE_UPDATE)
    {
        jMaintainJson["global"]["status"] = dwGlobalUpdt;
    }
    else
    {
        jMaintainJson["global"]["status"] = EN_MAINTAIN_TYPE_NORMAL;
    }
    if (0 != dwIsWhiteUid)
    {
        jMaintainJson["global"]["status"] = EN_MAINTAIN_TYPE_NORMAL;
    }

    // svr——修改时间和状态,所有节点都需遍历修改
    if (!jMaintainJson["svrList"].empty())
    {
        Json::Value::Members jMemberSid = jMaintainJson["svrList"].getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberSid.size(); ++udwIdx)
        {
            if (jMaintainJson["svrList"][jMemberSid[udwIdx]].empty())
            {
                continue;
            }
            udwMaintainBeg = jMaintainJson["svrList"][jMemberSid[udwIdx]]["sb"].asUInt();
            udwMaintainEnd = jMaintainJson["svrList"][jMemberSid[udwIdx]]["se"].asUInt();
            if (udwCurTime > udwMaintainEnd)
            {
                jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"] = EN_MAINTAIN_TYPE_NORMAL;
            }
            else if (udwMaintainBeg <= udwCurTime && udwCurTime <= udwMaintainEnd)
            {
                jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"] = EN_MAINTAIN_TYPE_MAINTAINING;
            }
            else
            {
                jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"] = EN_MAINTAIN_TYPE_TO_MAINTAIN;
            }

            if (EN_MAINTAIN_TYPE_NORMAL == jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"].asInt())
            {
                jMaintainJson["svrList"].removeMember(jMemberSid[udwIdx]);
            }
        }
    }

    // svr——判断当前svr的状态
    if(jMaintainJson["svrList"].isMember(strSid))
    {
        if (0 != dwIsWhiteUid)
        {
            jMaintainJson["svrList"][strSid]["status"] = EN_MAINTAIN_TYPE_NORMAL;
            dwRetStatus = EN_MAINTAIN_TYPE_NORMAL;
        }
        else if (jMaintainJson["svrList"][strSid]["status"].asUInt() == EN_MAINTAIN_TYPE_MAINTAINING && 0 == dwIsWhiteUid)
        {
            dwRetStatus = EN_MAINTAIN_TYPE_MAINTAINING;
        }
        else
        {
           //do nothing
        }
    }

    string md5str = NumToString(GetJsonMd5(jMaintainJson));
    UpdateMd5Json(m_jsonMd5, "new_maintain", md5str, 0);

    m_Mutex.release();
    return dwRetStatus;
}

TINT32 CStaticFileMgr::IsWhiteAccount(string strDevice)
{
    string strWhite_uid = CConfBase::GetString("white_uid");
    TSE_LOG_DEBUG(m_poServLog, ("[kurotest]CStaticFileMgr::LoadConfJson Json content [json=%s]", strWhite_uid.c_str()));
    Json::Reader reader;
    Json::Value jWhite_uid;
    jWhite_uid.clear();
    if (false == reader.parse(strWhite_uid.c_str(), jWhite_uid))
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::LoadConfJson, LoadConfJson failed. [size=%d]", jWhite_uid.size()));
        return -1;
    }
    TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::LoadConfJson [size=%d]", jWhite_uid["white_list"].size()));

    for (TINT32 dwIdx = 0; dwIdx < jWhite_uid["white_list"].size(); dwIdx++)
    {
        if (strDevice == jWhite_uid["white_list"][dwIdx].asString())
        {
            TSE_LOG_INFO(m_poServLog, ("CStaticFileMgr::IsWhiteAccount, [device=%s]", strDevice.c_str()));
            return 1;
        }
    }
    return 0;
}


TINT32 CStaticFileMgr::GetStaticJson( Json::Value &jContent, string strType, SRouteInfo *pstInfo )
{
    TINT32 dwRet = 0;
    if(strType == EN_STATIC_TYPE_META)
    {
        if(pstInfo->m_strVs == "1.1")
        {
            dwRet = GetStaticJson_MetaOld(jContent, pstInfo);
        }
        else
        {
            dwRet = GetStaticJson_Meta(jContent, pstInfo);
        }
    }
    else if(strType == EN_STATIC_TYPE_GUIDE)
    {
        dwRet = GetStaticJson_Guide(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_NEW_MAINTAIN)
    {
        dwRet = GetStaticJson_Maintain(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_NOTICE)
    {
        dwRet = GetStaticJson_Notice(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_SVRCONF)
    {
        dwRet = GetStaticJson_SvrConf(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_ITEMSWITCH)
    {
        dwRet = GetStaticJson_ItemSwitch(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_ALSTORE)
    {
        dwRet = GetStaticJson_AlStore(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_CLIENTCONF)
    {
        dwRet = GetStaticJson_ClientConf(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_GLOBALCONF)
    {
        dwRet = GetStaticJson_GlobalConf(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_USER_LINK)
    {
        dwRet = GetStaticJson_UserLinker(jContent, pstInfo);
    }
    else if(strType == EN_STATIC_TYPE_MD5)
    {
        dwRet = GetStaticJson_Md5(jContent, pstInfo);
    }
    else
    {
        assert(0);
    }
    return dwRet;
}

TINT32 CStaticFileMgr::GetStaticJson_Meta( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjMeta = GetStaticFile(EN_STATIC_TYPE_META, pstInfo->m_strPlatform, pstInfo->m_strVs);
    CStaticFileContent *pobjMetaRaw = GetStaticFile(EN_STATIC_TYPE_META_RAW, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjMeta == NULL || pobjMetaRaw == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    TUINT64 uddwRawMd5 = GetJsonMd5(pobjMeta->m_jsonContent);
    TUINT64 uddwNewMd5 = GetJsonMd5(pobjMetaRaw->m_jsonContent);

    return JsonDiff(pobjMetaRaw->m_jsonContent, uddwNewMd5, pobjMeta->m_jsonContent, uddwRawMd5, jContent["op_meta"]);
}

TINT32 CStaticFileMgr::GetStaticJson_MetaOld( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjMeta = GetStaticFile(EN_STATIC_TYPE_META, pstInfo->m_strPlatform, pstInfo->m_strVs);

    string strS3CdnUrl = "http://d2ggugd94z1qg1.cloudfront.net/" + CConfBase::GetString("project") + "/versionconfig";
    string strS3BackUpUrl = "http://bob-pro.simpysam.com/pro_sys/data_meta/globalconfig/";

    //string strS3CdnUrl = "http://leyi-game-static-file-test.s3.cn-north-1.amazonaws.com.cn/bob_test_group/versionconfig";
    //string strS3BackUpUrl = "http://leyi-game-static-file-test.s3.cn-north-1.amazonaws.com.cn/bob_test_group/versionconfig";

    //string strS3CdnUrl = "http://54.223.156.94:9090/op.simpysam.com/bob_test_group_op_sys/pro_sys/data/versionconfig";
    //string strS3BackUpUrl = "http://54.223.156.94:9090/op.simpysam.com/bob_test_group_op_sys/pro_sys/data/versionconfig";
    
    Json::Value jDataInfoJson;
    jDataInfoJson.clear();
    jDataInfoJson["name"] = pobjMeta->m_strFileName;
    jDataInfoJson["md5"] = pobjMeta->m_strMd5;
    jDataInfoJson["size"] = pobjMeta->m_udwFileSize;
    jDataInfoJson["ts"] = pobjMeta->m_udwFileTs;
    jDataInfoJson["url"] = strS3CdnUrl + "/" + pstInfo->m_strVs + "/" + pstInfo->m_strPlatform;
    jDataInfoJson["url_bak"] = strS3BackUpUrl + "/" + pstInfo->m_strPlatform + "/" + pstInfo->m_strPlatform + "_meta/" + pstInfo->m_strVs;

    jContent["op_static_file"] = jDataInfoJson;
    return EN_JSON_DIFF__NEW;
}

TINT32 CStaticFileMgr::GetStaticJson_Guide( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjContent = GetStaticFile(EN_STATIC_TYPE_GUIDE, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjContent == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    jContent["op_guide"] = pobjContent->m_jsonContent;
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_Maintain( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjConent = GetStaticFile(EN_STATIC_TYPE_NEW_MAINTAIN, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjConent == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    jContent["op_new_maintain"] = pobjConent->m_jsonContent;

    UpdtAndGetMaintainStatus(jContent["op_new_maintain"], pstInfo->m_strPlatform, pstInfo->m_strUpdateVs, pstInfo->m_strSid, pstInfo->m_udwCurTime, pstInfo->m_strDevice);

    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_Notice( Json::Value &jContent, SRouteInfo *pstInfo )
{
    jContent["op_notice"]["notice_list"] = Json::Value(Json::nullValue);
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_SvrConf( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjContent = GetStaticFile(EN_STATIC_TYPE_SVRCONF, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjContent == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    jContent["op_svr_conf"][pstInfo->m_strSid] = pobjContent->m_jsonContent["svr_json"][pstInfo->m_strSid];
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_ItemSwitch( Json::Value &jContent, SRouteInfo *pstInfo )
{
    jContent["op_item_switch"] = Json::Value(Json::nullValue);
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_AlStore( Json::Value &jContent, SRouteInfo *pstInfo )
{
    jContent["op_al_store"] = Json::Value(Json::nullValue);
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_ClientConf( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjContent = GetStaticFile(EN_STATIC_TYPE_CLIENTCONF, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjContent == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    jContent["op_client_conf"] = pobjContent->m_jsonContent;
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_GlobalConf( Json::Value &jContent, SRouteInfo *pstInfo )
{
    jContent["op_global_conf"] = Json::Value(Json::nullValue);
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_UserLinker( Json::Value &jContent, SRouteInfo *pstInfo )
{
    CStaticFileContent *pobjContent = GetStaticFile(EN_STATIC_TYPE_USER_LINK, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjContent == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    jContent["op_user_link"] = pobjContent->m_jsonContent[pstInfo->m_strSid];
    return 0;
}

TINT32 CStaticFileMgr::UpdateMd5Json( Json::Value &jContent, string strKey, string strMd5, TINT32 dwReload )
{
    jContent[strKey][0] = strMd5;
    jContent[strKey][1] = dwReload;
    return 0;
}

TINT32 CStaticFileMgr::UpdateMd5Json_Version(Json::Value &jContent, string strKey, string strMd5, TINT32 dwReload, string strPlatForm, string strVersion)
{
    jContent[strVersion][strPlatForm][strKey][0] = strMd5;
    jContent[strVersion][strPlatForm][strKey][1] = dwReload;
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_Md5( Json::Value &jContent, SRouteInfo *pstInfo )
{
    //更新maintain信息
    CStaticFileContent *pobjConent = GetStaticFile(EN_STATIC_TYPE_NEW_MAINTAIN, pstInfo->m_strPlatform, pstInfo->m_strVs);
    if (pobjConent == NULL)
    {
        return EN_RET_CODE__STATICFILE_ERROR;
    }
    CStaticFileMgr::GetInstance()->UpdtAndGetMaintainStatus(pobjConent->m_jsonContent, pstInfo->m_strPlatform, pstInfo->m_strUpdateVs, pstInfo->m_strSid, pstInfo->m_udwCurTime, pstInfo->m_strDevice);

    Json::Value::Members jMemberVersion = m_jsonMd5_version.getMemberNames();
    for (TUINT32 udwIdx = 0; udwIdx < jMemberVersion.size(); ++udwIdx)
    {
        if(jMemberVersion[udwIdx] == pstInfo->m_strVs)
        {
            TSE_LOG_INFO(m_poServLog, ("CStaticFileMgr::GetStaticJson_Md5, kurotest: [version=%s]", jMemberVersion[udwIdx].c_str()));
            Json::Value::Members jMemberPlatform = m_jsonMd5_version[jMemberVersion[udwIdx]].getMemberNames();
            for (TUINT32 udwIdy = 0; udwIdy < jMemberPlatform.size(); udwIdy++)
            {
                if (jMemberPlatform[udwIdy] == pstInfo->m_strPlatform)
                {
		            TSE_LOG_INFO(m_poServLog, ("CStaticFileMgr::GetStaticJson_Md5, kurotest: [platform=%s]", jMemberPlatform[udwIdy].c_str()));
                    Json::Value::Members jMemberFile = m_jsonMd5_version[jMemberVersion[udwIdx]][jMemberPlatform[udwIdy]].getMemberNames();
                    for (TUINT32 udwIdz = 0; udwIdz < jMemberFile.size(); udwIdz++)
                    {
			            TSE_LOG_INFO(m_poServLog, ("CStaticFileMgr::GetStaticJson_Md5, kurotest: [file=%s]", jMemberFile[udwIdz].c_str()));
                        m_jsonMd5[jMemberFile[udwIdz]][0] = m_jsonMd5_version[jMemberVersion[udwIdx]][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]][0];
                        m_jsonMd5[jMemberFile[udwIdz]][1] = m_jsonMd5_version[jMemberVersion[udwIdx]][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]][1];
                    }
                    break;
                }
            }
            break;
        }
    }
    jContent["op_md5"] = CStaticFileMgr::GetInstance()->m_jsonMd5;
    return 0;
}

TINT32 CStaticFileMgr::GetStaticJson_Account( Json::Value &jContent, TINT32 dwAccountStatus )
{
    jContent["op_account_status"]["status"] = dwAccountStatus;
    return 0;
}

TINT32 CStaticFileMgr::PreprocessStaticFile( Json::Value &jContent, string strType )
{
    TINT32 dwRet = 0;
    if(strType == EN_STATIC_TYPE_NEW_MAINTAIN)
    {
        dwRet = PreprocessStaticFile_Maintain(jContent);
    }

    return dwRet;
}

TINT32 CStaticFileMgr::PreprocessStaticFile_Maintain( Json::Value &jMaintainJson)
{
    // global——修改版本号
    jMaintainJson["global"]["min_ver"] = atof(jMaintainJson["global"]["min_ver"].asString().c_str());
    jMaintainJson["global"]["max_ver"] = atof(jMaintainJson["global"]["max_ver"].asString().c_str());


    //global——修改时间戳
    TUINT32 udwCurTime = CTimeUtils::GetUnixTime();
    TUINT32 udwGlobalBeginTime = 0;
    TUINT32 udwGlobalEndTime = 0;
    if ("0" != jMaintainJson["global"]["begin"].asString())
    {
        udwGlobalBeginTime = CTimeUtils::GetUinxTimeFromFormatTime(jMaintainJson["global"]["begin"].asString().c_str());
    }
    if ("0" != jMaintainJson["global"]["end"].asString())
    {
        udwGlobalEndTime = CTimeUtils::GetUinxTimeFromFormatTime(jMaintainJson["global"]["end"].asString().c_str());
    }
    jMaintainJson["global"]["begin"] = udwGlobalBeginTime;
    jMaintainJson["global"]["end"] = udwGlobalEndTime;

    //global——修改status
    if (udwCurTime > udwGlobalEndTime)
    {
        jMaintainJson["global"]["status"] = 0U;
    }
    else if (udwGlobalBeginTime <= udwCurTime && udwCurTime <= udwGlobalEndTime)
    {
        jMaintainJson["global"]["status"] = 2U;
    }
    else
    {
        jMaintainJson["global"]["status"] = 1U;
    }

    // svr——修改时间和状态
    if (!jMaintainJson["svrList"].empty())
    {
        Json::Value::Members jMemberSid = jMaintainJson["svrList"].getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberSid.size(); ++udwIdx)
        {

            if (jMaintainJson["svrList"][jMemberSid[udwIdx]].empty())
            {
                continue;
            }

            TUINT32 udwSvrBeginTime = CTimeUtils::GetUinxTimeFromFormatTime(jMaintainJson["svrList"][jMemberSid[udwIdx]]["sb"].asString().c_str());
            TUINT32 udwSvrEndTime = CTimeUtils::GetUinxTimeFromFormatTime(jMaintainJson["svrList"][jMemberSid[udwIdx]]["se"].asString().c_str());
            jMaintainJson["svrList"][jMemberSid[udwIdx]]["sb"] = udwSvrBeginTime;
            jMaintainJson["svrList"][jMemberSid[udwIdx]]["se"] = udwSvrEndTime;
            if (udwCurTime > udwSvrEndTime)
            {
                jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"] = 0U;
            }
            else if (udwSvrBeginTime <= udwCurTime && udwCurTime <= udwSvrEndTime)
            {
                jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"] = 2U;
            }
            else
            {
                jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"] = 1U;
            }

            if (0 == jMaintainJson["svrList"][jMemberSid[udwIdx]]["status"].asInt())
            {
                jMaintainJson["svrList"].removeMember(jMemberSid[udwIdx]);
            }
        }
    }
    return 0;
}

TINT32 CStaticFileMgr::PreprocessStaticFile_Md5( Json::Value &jMd5Json )
{
    jMd5Json[EN_STATIC_TYPE_GAME][0] = "0";
    jMd5Json[EN_STATIC_TYPE_GAME][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_DOC][0] = "0";
    jMd5Json[EN_STATIC_TYPE_DOC][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_EQUIP][0] = "0";
    jMd5Json[EN_STATIC_TYPE_EQUIP][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_META][0] = "0";
    jMd5Json[EN_STATIC_TYPE_META][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_NOTICE][0] = "0";
    jMd5Json[EN_STATIC_TYPE_NOTICE][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_NEW_MAINTAIN][0] = "0";
    jMd5Json[EN_STATIC_TYPE_NEW_MAINTAIN][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_ALLLAKE][0] = "0";
    jMd5Json[EN_STATIC_TYPE_ALLLAKE][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_GUIDE][0] = "0";
    jMd5Json[EN_STATIC_TYPE_GUIDE][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_CLIENTCONF][0] = "0";
    jMd5Json[EN_STATIC_TYPE_CLIENTCONF][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_ITEMSWITCH][0] = "0";
    jMd5Json[EN_STATIC_TYPE_ITEMSWITCH][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_ALSTORE][0] = "0";
    jMd5Json[EN_STATIC_TYPE_ALSTORE][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_SVRCONF][0] = "0";
    jMd5Json[EN_STATIC_TYPE_SVRCONF][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_GLOBALCONF][0] = "0";
    jMd5Json[EN_STATIC_TYPE_GLOBALCONF][1] = 0U;
    jMd5Json[EN_STATIC_TYPE_USER_LINK][0] = "0";
    jMd5Json[EN_STATIC_TYPE_USER_LINK][1] = 0U;
    return 0;
}

TUINT64 CStaticFileMgr::GetStringMD5( const string &strSourceData )
{
    if(strSourceData.length() == 0)
    {
        return 0;
    }

    TUINT64 uddwKey = 0;
    MD5String(strSourceData.c_str(), (TUCHAR*)&uddwKey, sizeof(uddwKey));
    return uddwKey;
}

TBOOL CStaticFileMgr::IsFileTypeExist( string strFileType )
{
    map<string, SStaticFileProperty>::iterator it = m_mapStaticFileProperty.find(strFileType);
    if(it != m_mapStaticFileProperty.end())
    {
        return TRUE;
    }
    return FALSE;
}

TBOOL CStaticFileMgr::IsFIleTypeNeedLoadData( string strFileType )
{
    map<string, SStaticFileProperty>::iterator it = m_mapStaticFileProperty.find(strFileType);
    if(it != m_mapStaticFileProperty.end())
    {
        return it->second.m_bNeedLoadData;
    }
    return FALSE;
}

TBOOL CStaticFileMgr::IsFileTypeNeedMd5(string strFileType)
{
    map<string, SStaticFileProperty>::iterator it = m_mapStaticFileProperty.find(strFileType);
    if (it != m_mapStaticFileProperty.end())
    {
        return it->second.m_bNeedMd5;
    }
    return FALSE;
}

TINT32 CStaticFileMgr::JsonDiff( Json::Value &jsonRaw, TUINT64 uddwRawMd5, Json::Value &jsonNew, TUINT64 uddwNewMd5, Json::Value &jsonDiff )
{
    if(uddwRawMd5 == uddwNewMd5)
    {
        return EN_JSON_DIFF__EQUAL;
    }

    // 检测第一层---类型校验
    bool isRawObject = jsonRaw.isObject();
    bool isNewObject = jsonNew.isObject();
    if(isRawObject == false || isNewObject == false)
    {
        jsonDiff = jsonNew;
        return EN_JSON_DIFF__NEW;
    }

    // 检测第一层---delete
    Json::Value::Members jsonRawKeys = jsonRaw.getMemberNames();
    for(Json::Value::Members::iterator it = jsonRawKeys.begin(); it != jsonRawKeys.end(); ++it)
    {
        if(jsonNew.isMember(*it))
        {
            continue;
        }
        else
        {
            jsonDiff[*it] = Json::Value(Json::nullValue);
        }
    }

    // 检测第一层---update和new
    Json::Value::Members jsonNewKeys = jsonNew.getMemberNames();
    for(Json::Value::Members::iterator it = jsonNewKeys.begin(); it != jsonNewKeys.end(); ++it)
    {
        TUINT64 uddwNewMd5 = GetJsonMd5(jsonNew[*it]);
        TUINT64 uddwRawMd5 = 0;
        if(jsonRaw.isMember(*it))
        {
            uddwRawMd5 = GetJsonMd5(jsonRaw[*it]);
        }
        else
        {
            uddwRawMd5 = 0;
        }

        if(uddwNewMd5 == uddwRawMd5)
        {
            continue;
        }

        JsonDiff(jsonRaw[*it], uddwRawMd5, jsonNew[*it], uddwNewMd5, jsonDiff[*it]);
    }

    return EN_JSON_DIFF__ADD;
}

TUINT64 CStaticFileMgr::GetJsonMd5(Json::Value &jsonTmp)
{
    Json::FastWriter writer;
    writer.omitEndingLineFeed();
    string sRspJsonContent = writer.write(jsonTmp);
    return GetStringMD5(sRspJsonContent.c_str());
}

TUINT32 CStaticFileMgr::GetFileSize( const string &strFileName )
{
    TUINT32 udwFileSize = 0;
    FILE *fp=fopen(strFileName.c_str(),"r");
    if(!fp) 
    {
        return 0;
    }
    fseek(fp,0L,SEEK_END);
    udwFileSize=ftell(fp);
    fclose(fp);

    return udwFileSize;

}
