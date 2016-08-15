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
    m_poServLog = poLog;

    m_stStaticFileJsonCache.oStaticDataGame.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataDoc.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataEquip.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataGuide.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataMeta.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataLake.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataMaintain.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataSvrConf.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataClient.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataGlobalConf.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataOnsale.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataNotice.InitStaticData(poLog);
    m_stStaticFileJsonCache.oStaticDataUserLink.InitStaticData(poLog);
    


    
    m_jDataListJson.clear();
    
   
    
    return 0;
}

TINT32 CStaticFileMgr::CheckStaticFileUpdate()
{
    TINT32 dwRet = 0;


    // 更新data_list.json
    Json::Value jNewDataListJson;
    jNewDataListJson.clear();
    dwRet = LoadNewDataListJson(jNewDataListJson);
    if (0 != dwRet)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::UpdateStaticFile, load data_list.json failed."));
        return -1;
    }


    // 旧的data_list.json
    Json::Value jOldDataListJson;
    jOldDataListJson.clear();
    jOldDataListJson = m_jDataListJson;


    // 获取变化的静态文件列表(globalconfig/versionconfig)
    vector<SGlobalConfig> vecGlobalList;
    vector<SVersionConfig> vecVersionList;
    vecGlobalList.clear();
    vecVersionList.clear();
    dwRet = GetUpdateStaticFileList(jNewDataListJson, jOldDataListJson, vecGlobalList, vecVersionList);
    if (0 != dwRet)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::UpdateStaticFile, get static_file list failed."));
        return -2;
    }

    dwRet = UpdateStaticFile(vecGlobalList, vecVersionList);
    if (0 != dwRet)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::UpdateStaticFile, update static_file failed."));
        return -3;
    }

    // 确保更新完所有的静态文件再更新data_list.json
    dwRet = UpdateDataList(jNewDataListJson);
    if (0 != dwRet)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::UpdateStaticFile, update data_list.json failed."));
        return -3;
    }


    return 0;
}


TINT32 CStaticFileMgr::GetStaticFileInfo(const string &strSaticFileType, const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{

    if (EN_STATIC_TYPE_GAME == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataGame.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_DOC == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataDoc.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_GUIDE == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataGuide.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_EQUIP == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataEquip.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_META == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataMeta.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_NEW_MAINTAIN == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataMaintain.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_NOTICE == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataNotice.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_ITEMSWITCH == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataOnsale.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_ALSTORE == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataOnsale.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_SVRCONF == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataSvrConf.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_ALLLAKE == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataLake.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_CLIENTCONF == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataClient.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_GLOBALCONF == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataGlobalConf.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else if (EN_STATIC_TYPE_USER_LINK == strSaticFileType)
    {
        m_stStaticFileJsonCache.oStaticDataUserLink.GetStaticData(stRouteInfo, pstStaticFileInfo);
    }
    else
    {
        assert(0);
    }
    
    return 0;
}


TINT32 CStaticFileMgr::GetMaintainJson(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    m_stStaticFileJsonCache.oStaticDataMaintain.GetStaticData(stRouteInfo, pstStaticFileInfo);
    return 0;
}

TINT32 CStaticFileMgr::GetMapData(const SRouteInfo &stRouteInfo, TCHAR *pMapData, TUINT32 &udwMapDataLen)
{
    return m_stStaticFileJsonCache.oStaticDataLake.GetStaticData(stRouteInfo, pMapData, udwMapDataLen);
}


TINT32 CStaticFileMgr::LoadNewDataListJson(Json::Value &jNewDataListJson)
{
    jNewDataListJson.clear();

    Json::Reader reader;
    std::ifstream is;


    is.open(DATA_LIST_JSON);
    if (reader.parse(is, jNewDataListJson) == false)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::UpdateStaticFile, parse file[%s] failed.", DATA_LIST_JSON));
        is.close();
        return -1;
    }
    return 0;
}

TINT32 CStaticFileMgr::GetUpdateStaticFileList(const Json::Value &jNewDataListJson, const Json::Value &jOldDataListJson, vector<SGlobalConfig> &vecGlobalList, vector<SVersionConfig> &vecVersionList)
{
    TINT32 dwRetCode = 0;
    

    // 遍历旧的data_list.json
    map<string, SGlobalConfig> mapOldGlobal;
    map<string, SVersionConfig> mapOldVersion;
    mapOldGlobal.clear();
    mapOldGlobal.clear();
    dwRetCode = GetDataListMap(jOldDataListJson, mapOldGlobal, mapOldVersion);
    if (0 != dwRetCode)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::GetUpdateStaticFileList, get old data_list.json map failed"));
        return -1;
    }


    // 遍历新的data_list.json
    map<string, SGlobalConfig> mapNewGlobal;
    map<string, SVersionConfig> mapNewVersion;
    mapNewGlobal.clear();
    mapNewVersion.clear();
    dwRetCode = GetDataListMap(jNewDataListJson, mapNewGlobal, mapNewVersion);
    if (0 != dwRetCode)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::GetUpdateStaticFileList, get new data_list.json map failed"));
        return -2;
    }

    // 比较新旧data_list.json的差异
    // 1. globalconfig
    vecGlobalList.clear();
    dwRetCode = GetUpdateGlobalStaticFileList(vecGlobalList, mapOldGlobal, mapNewGlobal);
    if (0 != dwRetCode)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::GetUpdateStaticFileList, get global static_file list failed"));
        return -3;
    }
    // 2. versionconfig
    vecVersionList.clear();
    dwRetCode = GetUpdateVersionStaticFileList(vecVersionList, mapOldVersion, mapNewVersion);
    if (0 != dwRetCode)
    {
        TSE_LOG_ERROR(m_poServLog, ("CStaticFileMgr::GetUpdateStaticFileList, get version static_file list failed"));
        return -4;
    }

    return 0;
}

TINT32 CStaticFileMgr::UpdateDataList(const Json::Value &jNewDataListJson)
{
    m_jDataListJson.clear();
    m_jDataListJson = jNewDataListJson;
    return 0;
}


TINT32 CStaticFileMgr::UpdateStaticFile(const vector<SGlobalConfig> &vecGlobalList, const vector<SVersionConfig> &vecVersionList)
{
    SRouteInfo stRouteInfo;
    stRouteInfo.Reset();

    set<string> setVersion;
    setVersion.clear();
    set<string>::iterator itVersion;

    set<string> setPlatform;
    setVersion.clear();
    set<string>::iterator itPlatform;


    // versionconfig
    for (TUINT32 udwIdx = 0; udwIdx < vecVersionList.size(); ++udwIdx)
    {
        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = vecVersionList[udwIdx].m_strPlatForm;
        stRouteInfo.m_strVs = vecVersionList[udwIdx].m_strVersion;
        stRouteInfo.m_udwReload = vecVersionList[udwIdx].m_udwReload;
        stRouteInfo.m_strStaticFilePath = "../data" + vecVersionList[udwIdx].m_strStaticFilePath;
        stRouteInfo.m_udwFileTs = vecVersionList[udwIdx].m_udwTimeStamp;
        setVersion.insert(stRouteInfo.m_strVs);
        setPlatform.insert(stRouteInfo.m_strPlatform);


        if (EN_UPDATE_TYPE_DEL == vecVersionList[udwIdx].m_udwUpdateType)
        {
            // todo
        }

        if (EN_STATIC_TYPE_META == vecVersionList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataMeta.UpdateStaticData(stRouteInfo, vecVersionList[udwIdx].m_udwUpdateType);
        }
    }




    // globalconfig
    for (TUINT32 udwIdx = 0; udwIdx < vecGlobalList.size(); ++udwIdx)
    {

        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = vecGlobalList[udwIdx].m_strPlatForm;
        stRouteInfo.m_udwReload = vecGlobalList[udwIdx].m_udwReload;
        stRouteInfo.m_strStaticFilePath = "../data" + vecGlobalList[udwIdx].m_strStaticFilePath;
        stRouteInfo.m_udwFileTs = vecGlobalList[udwIdx].m_udwTimeStamp;

        if (EN_UPDATE_TYPE_DEL == vecGlobalList[udwIdx].m_udwUpdateType)
        {
           // todo
        }

        if (EN_STATIC_TYPE_ALLLAKE == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            for (itVersion = setVersion.begin(); itVersion != setVersion.end(); ++itVersion)
            {
                for (itPlatform = setPlatform.begin(); itPlatform != setPlatform.end(); ++itPlatform)
                {
                    if ("new" == *itVersion)
                    {
                        continue;
                    }
                    stRouteInfo.m_strPlatform = *itPlatform;
                    stRouteInfo.m_strVs = *itVersion;
                    stRouteInfo.m_strSid = m_stStaticFileJsonCache.oStaticDataLake.GetLakeSid(vecGlobalList[udwIdx].m_strStaticFileName);
                    m_stStaticFileJsonCache.oStaticDataLake.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
                }
  
            }
        }
        if (EN_STATIC_TYPE_NEW_MAINTAIN == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataMaintain.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_NOTICE == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataNotice.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_ONSALE == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataOnsale.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_SVRCONF == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataSvrConf.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }
       
        if (EN_STATIC_TYPE_CLIENTCONF == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataClient.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_GLOBALCONF == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataGlobalConf.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }   
        if (EN_STATIC_TYPE_USER_LINK == vecGlobalList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataUserLink.UpdateStaticData(stRouteInfo, vecGlobalList[udwIdx].m_udwUpdateType);
        }
    }

 

    // versionconfig
    for (TUINT32 udwIdx = 0; udwIdx < vecVersionList.size(); ++udwIdx)
    {
        stRouteInfo.Reset();
        stRouteInfo.m_strPlatform = vecVersionList[udwIdx].m_strPlatForm;
        stRouteInfo.m_strVs = vecVersionList[udwIdx].m_strVersion;
        stRouteInfo.m_udwReload = vecVersionList[udwIdx].m_udwReload;
        stRouteInfo.m_strStaticFilePath = "../data" + vecVersionList[udwIdx].m_strStaticFilePath;
        stRouteInfo.m_udwFileTs = vecVersionList[udwIdx].m_udwTimeStamp;

        if (EN_UPDATE_TYPE_DEL == vecVersionList[udwIdx].m_udwUpdateType)
        {
            // todo
        }


        if (EN_STATIC_TYPE_GAME == vecVersionList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataGame.UpdateStaticData(stRouteInfo, vecVersionList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_DOC == vecVersionList[udwIdx].m_strStaticDataType)
        {
            stRouteInfo.m_strLang = m_stStaticFileJsonCache.oStaticDataDoc.GetDocId(vecVersionList[udwIdx].m_strStaticFileName);
            m_stStaticFileJsonCache.oStaticDataDoc.UpdateStaticData(stRouteInfo, vecVersionList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_GUIDE == vecVersionList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataGuide.UpdateStaticData(stRouteInfo, vecVersionList[udwIdx].m_udwUpdateType);
        }
        if (EN_STATIC_TYPE_EQUIP == vecVersionList[udwIdx].m_strStaticDataType)
        {
            m_stStaticFileJsonCache.oStaticDataEquip.UpdateStaticData(stRouteInfo, vecVersionList[udwIdx].m_udwUpdateType);
        }
      
    }


    return 0;
}

TINT32 CStaticFileMgr::GetDataListMap(const Json::Value &jDataListJson, map<string, SGlobalConfig> &mapGlobal, map<string, SVersionConfig> &mapVersion)
{
    mapGlobal.clear();
    mapVersion.clear();

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
                    SGlobalConfig stGlobalConfig;
                    stGlobalConfig.Reset();
                    stGlobalConfig.m_strStaticDataType = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["type"].asString();
                    stGlobalConfig.m_strPlatForm = jMemberPlatform[udwIdy];
                    stGlobalConfig.m_strStaticFileName = jMemberFile[udwIdz];
                    stGlobalConfig.m_strStaticFilePath = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["path"].asString();
                    stGlobalConfig.m_udwTimeStamp = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["timestamp"].asUInt();
                    stGlobalConfig.m_udwReload = jDataListJson["globalconfig"][jMemberPlatform[udwIdy]][jMemberFile[udwIdz]]["reload"].asUInt();
                    mapGlobal.insert(make_pair(stGlobalConfig.m_strStaticFileName, stGlobalConfig));
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
                        SVersionConfig stVersionConfig;
                        stVersionConfig.Reset();
                        stVersionConfig.m_strStaticDataType = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["type"].asString();
                        stVersionConfig.m_strVersion = jMemberVersion[udwIdy];
                        stVersionConfig.m_strPlatForm = jMemberPlatform[udwIdz];
                        stVersionConfig.m_strStaticFileName = jMemberFile[udwIdm];
                        stVersionConfig.m_strStaticFilePath = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["path"].asString();
                        stVersionConfig.m_udwTimeStamp = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["timestamp"].asUInt();
                        stVersionConfig.m_udwReload = jDataListJson["versionconfig"][jMemberVersion[udwIdy]][jMemberPlatform[udwIdz]][jMemberFile[udwIdm]]["reload"].asUInt();
                        mapVersion.insert(make_pair(stVersionConfig.m_strStaticFileName, stVersionConfig));
                    }
                }
            }
        }
        else
        {
            return -1;
        }
    }
    return 0;
}

TINT32 CStaticFileMgr::GetUpdateGlobalStaticFileList(vector<SGlobalConfig> &vecGlobalList, map<string, SGlobalConfig> &mapOldGlobal, map<string, SGlobalConfig> &mapNewGlobal)
{
    vecGlobalList.clear();
    map<string, SGlobalConfig>::iterator it_global_old;
    map<string, SGlobalConfig>::iterator it_global_new;
    // 1. 以old为标准
    for (it_global_old = mapOldGlobal.begin(); it_global_old != mapOldGlobal.end(); ++it_global_old)
    {
        it_global_new = mapNewGlobal.find(it_global_old->first);
        if (it_global_new == mapNewGlobal.end())
        {
            it_global_old->second.m_udwUpdateType = EN_UPDATE_TYPE_DEL;
            vecGlobalList.push_back(it_global_old->second);
        }
        else
        {
            if (it_global_old->second.m_udwTimeStamp < it_global_new->second.m_udwTimeStamp)
            {
                it_global_old->second.m_udwUpdateType = EN_UPDATE_TYPE_UPDATE;
                it_global_old->second.m_udwTimeStamp = it_global_new->second.m_udwTimeStamp;
                vecGlobalList.push_back(it_global_old->second);
            }
        }
    }

    // 2. 以new为标准
    for (it_global_new = mapNewGlobal.begin(); it_global_new != mapNewGlobal.end(); ++it_global_new)
    {
        it_global_old = mapOldGlobal.find(it_global_new->first);
        if (it_global_old == mapOldGlobal.end())
        {
            it_global_new->second.m_udwUpdateType = EN_UPDATE_TYPE_ADD;
            vecGlobalList.push_back(it_global_new->second);
        }
    }
    return 0;
}

TINT32 CStaticFileMgr::GetUpdateVersionStaticFileList(vector<SVersionConfig> &vecVersionList, map<string, SVersionConfig> &mapOldVersion, map<string, SVersionConfig> &mapNewVersion)
{
    vecVersionList.clear();
    map<string, SVersionConfig>::iterator it_version_old;
    map<string, SVersionConfig>::iterator it_version_new;
    // 1. 以old为标准
    for (it_version_old = mapOldVersion.begin(); it_version_old != mapOldVersion.end(); ++it_version_old)
    {
        it_version_new = mapNewVersion.find(it_version_old->first);
        if (it_version_new == mapNewVersion.end())
        {
            it_version_old->second.m_udwUpdateType = EN_UPDATE_TYPE_DEL;
            vecVersionList.push_back(it_version_old->second);
        }
        else
        {
            if (it_version_old->second.m_udwTimeStamp < it_version_new->second.m_udwTimeStamp)
            {
                it_version_old->second.m_udwUpdateType = EN_UPDATE_TYPE_UPDATE;
                it_version_old->second.m_udwTimeStamp = it_version_new->second.m_udwTimeStamp;
                vecVersionList.push_back(it_version_old->second);
            }
        }
    }

    // 2. 以new为标准
    for (it_version_new = mapNewVersion.begin(); it_version_new != mapNewVersion.end(); ++it_version_new)
    {
        it_version_old = mapOldVersion.find(it_version_new->first);
        if (it_version_old == mapOldVersion.end())
        {
            it_version_new->second.m_udwUpdateType = EN_UPDATE_TYPE_ADD;
            vecVersionList.push_back(it_version_new->second);
        }
    }
    return 0;
}


TINT32 CStaticFileMgr::CheckStaticDataCompute()
{
    SRouteInfo stRouteInfo;
    stRouteInfo.Reset();
    
    m_stStaticFileJsonCache.oStaticDataMaintain.UpdateStaticData(stRouteInfo, EN_UPDATE_TYPE_COMPUTE_UPDATE);
    m_stStaticFileJsonCache.oStaticDataNotice.UpdateStaticData(stRouteInfo, EN_UPDATE_TYPE_COMPUTE_UPDATE);
    m_stStaticFileJsonCache.oStaticDataOnsale.UpdateStaticData(stRouteInfo, EN_UPDATE_TYPE_COMPUTE_UPDATE);
    
    //TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::CheckStaticDataCompute, check static data compute end"));
    return 0;
}




string CStaticFileMgr::GenStaticFileResult(const string &strStaticString, const TINT32 &dwOpEncryptFlag, const vector<SStaticFileInfo> &vecStaticFileInfo)
{

    TCHAR szDataBuffer[MAX_NETIO_PACKAGE_BUF_LEN];
    memset(szDataBuffer, 0, MAX_NETIO_PACKAGE_BUF_LEN);
    TUINT32 udwLengthAppend = 0;
    TCHAR* pCurPos = szDataBuffer;
    TCHAR* pEndPos = pCurPos + MAX_NETIO_PACKAGE_BUF_LEN;

    string strModuleName = "";
    udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos, "[");
    pCurPos += udwLengthAppend;


    for (TUINT32 udwIdx = 0; udwIdx < vecStaticFileInfo.size(); ++udwIdx)
    {
        if (udwIdx != 0)
        {
            udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos, ",");
            pCurPos += udwLengthAppend;
        }
        strModuleName = "op_" + strStaticString;

        if (1 == dwOpEncryptFlag)
        {
            udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos,
                "[{\"key\":\"[%s:%lu:0:1:%lu]\",\"data\":\"%s\"}",
                strModuleName.c_str(), vecStaticFileInfo[udwIdx].m_uddwEnMD5, vecStaticFileInfo[udwIdx].m_strEnStaticFile.length(), vecStaticFileInfo[udwIdx].m_strEnStaticFile.c_str());
            pCurPos += udwLengthAppend;

        }
        else
        {
            udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos,
                "[{\"key\":\"[%s:%lu:0:0:%lu]\",\"data\":%s}",
                strModuleName.c_str(), vecStaticFileInfo[udwIdx].m_uddwDeMD5, vecStaticFileInfo[udwIdx].m_strDeStaticFile.length(), vecStaticFileInfo[udwIdx].m_strDeStaticFile.c_str());
            pCurPos += udwLengthAppend;
        }
    }

    udwLengthAppend = snprintf(pCurPos, pEndPos - pCurPos, "]");
    pCurPos += udwLengthAppend;

    (*pCurPos) = '\0';

    return pCurPos;


    return 0;

}

std::string CStaticFileMgr::GetStaticFileMd5(const string strStaticFile, const SRouteInfo &stRouteInfo)
{
    string strKey = "";
    SStaticFileInfo stStaticFileInfo;
    stStaticFileInfo.Reset();
    m_stStaticFileJsonCache.oStaticDataMeta.GetStaticData(stRouteInfo, &stStaticFileInfo);
    if (EN_STATIC_TYPE_GAME == strStaticFile)
    {
        strKey = "game.bytes.txt---.force.zip.de";
    }
    else if (EN_STATIC_TYPE_DOC == strStaticFile)
    {
        strKey = "document_" + CStaticDataDoc::GetLangName(stRouteInfo.m_strLang) + ".bytes.txt---.option.zip.de";
    }
    else if (EN_STATIC_TYPE_EQUIP == strStaticFile)
    {
        strKey = "equip.bytes.txt---.force.zip.de";

    }
    else if (EN_STATIC_TYPE_ALLLAKE == strStaticFile)
    {
        strKey = "op_" + stRouteInfo.m_strSid + "_all_lake.bytes.txt---.option.zip.de";
    }
    else
    {
        strKey = "";
    }
    
    if ("" == strKey)
    {
        return "0";
    }
    else
    {
        /*
        if ("android" == stRouteInfo.m_strPlatform)
        {
            return "0";
        }
        else
        {
            return stStaticFileInfo.m_jDataJson[0]["data"]["pkg"]["asset_bundle"][strKey]["md5"].asString();
        }
        */
        return stStaticFileInfo.m_jDataJson[0]["data"]["pkg"]["static_files"][strKey]["md5"].asString();
    }


}


// ====================================================================================== //

TINT32 CStaticDataBase::EncryptString(const string &strSourceData, const TUINT32 &udwSourceDataLen, string &strEncryptData, TUINT32 &udwEncryptDataLen)
{
    CEncryptCR4 *poEncrypt = CEncryptCR4Mgr::get_instance()->GetEncrypt(1.0f);

    TINT32 dwBodyBufLen = MAX_NETIO_PACKAGE_BUF_LEN;
    unsigned long uddwCompressBufLen = MAX_NETIO_PACKAGE_BUF_LEN;


    TCHAR *szCompressBuffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];
    TCHAR *szEncodeBuffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];
    memset(szCompressBuffer, 0, MAX_NETIO_PACKAGE_BUF_LEN);
    memset(szEncodeBuffer, 0, MAX_NETIO_PACKAGE_BUF_LEN);

    compress((Bytef*)szCompressBuffer, &uddwCompressBufLen, (Bytef*)strSourceData.c_str(), udwSourceDataLen);//zip
    poEncrypt->rc4((unsigned char*)szCompressBuffer, uddwCompressBufLen);//encrypt
    CUtilBase64::encode(szCompressBuffer, szCompressBuffer + uddwCompressBufLen, szEncodeBuffer, dwBodyBufLen);//encode

    szEncodeBuffer[dwBodyBufLen] = '\0';
    strEncryptData = szEncodeBuffer;
    udwEncryptDataLen = dwBodyBufLen;

    delete[] szCompressBuffer;
    delete[] szEncodeBuffer;

    return 0;
}




TUINT64 CStaticDataBase::GetStringMD5(const string &strSourceData)
{
    TCHAR *pMD5Buffer = new TCHAR[MAX_NETIO_PACKAGE_BUF_LEN];
    memset(pMD5Buffer, 0, MAX_NETIO_PACKAGE_BUF_LEN);
    TUINT64 uddwKey = 0;
    strcpy(pMD5Buffer, strSourceData.c_str());
    ProcessString(pMD5Buffer);
    MD5String(pMD5Buffer, (TUCHAR*)&uddwKey, sizeof(uddwKey));
    delete[] pMD5Buffer;
    return uddwKey;
}

TVOID CStaticDataBase::ProcessString(char *pszStr)
{
    //1、全半角
    CUtf8Util::QuanJiaoToBanJiao(pszStr, pszStr);

    //2、删除标点
    CUtf8Util::strtrim(pszStr);

    //3、转大写
    CUtf8Util::strtoupper(pszStr);
}

TVOID CStaticDataBase::MD5String(const TCHAR *string, TUCHAR *pstrReturn, TUINT32 uLength)
{
    assert(string && string[0] && pstrReturn);
    assert(uLength > 0 && uLength <= 16);

    MD5_CTX context;
    TUCHAR digest[16];
    TUINT32 len = strlen(string);

    MD5Init(&context);
    MD5Update(&context, (const TUCHAR*)string, len);
    MD5Final(digest, &context);

    memcpy(pstrReturn, digest, uLength);
}

TUINT64 CStaticDataBase::GetRawDataMd5(const TUCHAR *pszData, const TUINT32 udwDataLen)
{
    TUINT64 uddwKey = 0;
    if (NULL == pszData || 0 == udwDataLen)
    {
        uddwKey = 0;
    }
    else
    {
        MD5Segment(pszData, udwDataLen, (TUCHAR*)&uddwKey, sizeof(uddwKey));
    }
    return uddwKey;
}

inline TVOID CStaticDataBase::MD5Segment(const TUCHAR *string, TUINT32 uStringLength, TUCHAR *pstrReturn, TUINT32 uReturnLength)
{
    //assert(string && string[0] && uStringLength > 0 && pstrReturn);
    assert(uReturnLength > 0 && uReturnLength <= 16);

    MD5_CTX context;
    TUCHAR digest[16];
    TUINT32 len = uStringLength;

    MD5Init(&context);
    MD5Update(&context, string, len);
    MD5Final(digest, &context);

    memcpy(pstrReturn, digest, uReturnLength);
}


// ====================================================================================== //

TINT32 CStaticDataGame::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;

    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);



    m_jStaticDataJson["default"]["json"] = Json::Value(Json::arrayValue);
    m_jStaticDataJson["default"]["de"]["content"] = strDeString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnMd5;
    m_jStaticDataJson["default"]["reload"] = 0;
    m_jStaticDataJson["default"]["md5"] = "0";
    

    return 0;
}

TINT32 CStaticDataGame::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);

    if (m_jStaticDataJson[stRouteInfo.m_strPlatform].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["md5"].asString();

    }
    return 0;
}

TINT32 CStaticDataGame::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {


        string strDeString = "[]";
        TUINT64 uddwDeMd5 = 0;

        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;

        TUINT32 udwEncryptDataLen = 0;

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo) + stRouteInfo.m_strVs;
        if ("" == strFileName)
        {
            return -1;
        }

        
        /*
        std::ifstream is(stRouteInfo.m_strStaticFilePath.c_str());
        if (is.is_open())
        {
            std::stringstream buffer;
            buffer << is.rdbuf();
            strDeString = buffer.str();
            if ("" == strDeString)
            {
                strDeString = "[]";
            }
        }
        */


        std::ifstream is;
        Json::Reader reader;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        Json::Value jGameJson;
        jGameJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jGameJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }

        strDeString = jsonWriter.write(jGameJson);

        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);


        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"] = jGameJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"] = strDeString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"] = uddwDeMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"] = strEnString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"] = uddwEnMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"] = stRouteInfo.m_udwReload;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["md5"] = CStaticFileMgr::GetInstance()->GetStaticFileMd5(EN_STATIC_TYPE_GAME, stRouteInfo);

        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_GAME, uddwDeMd5, stRouteInfo.m_udwReload);

        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }
    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;

}

string CStaticDataGame::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";

    if ("ios" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = IOS_GAME_FILE_NAME;
    }
    else if ("android" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = ANDROID_GAME_FILE_NAME;
    }
    else
    {
        // do nothing
    }
    return strStaticDataName;
}

// ====================================================================================== //

TINT32 CStaticDataDoc::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;

    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    m_jStaticDataJson["default"]["json"] = Json::Value(Json::arrayValue);
    m_jStaticDataJson["default"]["de"]["content"] = strDeString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnMd5;
    m_jStaticDataJson["default"]["reload"] = 0;
    m_jStaticDataJson["default"]["md5"] = "0";

    return 0;
}

TINT32 CStaticDataDoc::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);

    if (m_jStaticDataJson[stRouteInfo.m_strPlatform].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["md5"].asString();
        
    }
    return 0;
}

TINT32 CStaticDataDoc::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        
        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;

        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;

        TUINT32 udwEncryptDataLen = 0;

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo) + "." + stRouteInfo.m_strVs;
        if ("" == strFileName)
        {
            return -1;
        }

        /*
        std::ifstream is(stRouteInfo.m_strStaticFilePath.c_str());
        if (is.is_open())
        {
            std::stringstream buffer;
            buffer << is.rdbuf();
            strDeString = buffer.str();
            is.close();
            if ("" == strDeString)
            {
                strDeString = "\"null\"";
            }
        }
        */


        std::ifstream is;
        Json::Reader reader;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        Json::Value jDocJson;
        jDocJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jDocJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }

        strDeString = jsonWriter.write(jDocJson);



        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);


        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["json"] = jDocJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["de"]["content"] = strDeString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["de"]["md5"] = uddwDeMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["en"]["content"] = strEnString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["en"]["md5"] = uddwEnMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["reload"] = stRouteInfo.m_udwReload;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang]["md5"] = CStaticFileMgr::GetInstance()->GetStaticFileMd5(EN_STATIC_TYPE_DOC, stRouteInfo);
        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_DOC, uddwDeMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;

}

string CStaticDataDoc::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";

    if ("ios" == stRouteInfo.m_strPlatform)
    {
        switch (atoi(stRouteInfo.m_strLang.c_str()))
        {
        case 0:
            strStaticDataName = IOS_DOC_ENGLISH_FILE_NAME;
            break;
        case 1:
            strStaticDataName = IOS_DOC_GERMAN_FILE_NAME;
            break;
        case 2:
            strStaticDataName = IOS_DOC_FRENCH_FILE_NAME;
            break;
        case 4:
            strStaticDataName = IOS_DOC_SPAIN_FILE_NAME;
            break;
        case 5:
            strStaticDataName = IOS_DOC_RUSSIAN_FILE_NAME;
            break;
        case 6:
            strStaticDataName = IOS_DOC_CHINESE_FILE_NAME;
            break;
        default:
            strStaticDataName = IOS_DOC_ENGLISH_FILE_NAME;
          
        }
    }
    else if ("android" == stRouteInfo.m_strPlatform)
    {
        switch (atoi(stRouteInfo.m_strLang.c_str()))
        {
        case 0:
            strStaticDataName = ANDROID_DOC_ENGLISH_FILE_NAME;
            break;
        case 1:
            strStaticDataName = ANDROID_DOC_GERMAN_FILE_NAME;
            break;
        case 2:
            strStaticDataName = ANDROID_DOC_FRENCH_FILE_NAME;
            break;
        case 4:
            strStaticDataName = ANDROID_DOC_SPAIN_FILE_NAME;
            break;
        case 5:
            strStaticDataName = ANDROID_DOC_RUSSIAN_FILE_NAME;
            break;
        case 6:
            strStaticDataName = ANDROID_DOC_CHINESE_FILE_NAME;
            break;
        default:
            strStaticDataName = ANDROID_DOC_ENGLISH_FILE_NAME;
           
        }
    }
    else
    {
        // do nothing
    }
    return strStaticDataName;
   
}

string CStaticDataDoc::GetDocId(const string &strDocName)
{
    string strFind = "";
    string::size_type pos = 0;
    string strDocId = "";
       
   
    pos = 0;
    strFind = IOS_DOC_GERMAN_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "1";
    }
    pos = 0;
    strFind = ANDROID_DOC_GERMAN_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "1";
    }

    pos = 0;
    strFind = IOS_DOC_ENGLISH_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "0";
    }
    pos = 0;
    strFind = ANDROID_DOC_ENGLISH_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "0";
    }


    pos = 0;
    strFind = IOS_DOC_FRENCH_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "2";
    }
    pos = 0;
    strFind = ANDROID_DOC_FRENCH_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "2";
    }


    pos = 0;
    strFind = IOS_DOC_RUSSIAN_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "5";
    }
    pos = 0;
    strFind = ANDROID_DOC_RUSSIAN_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "5";
    }


    pos = 0;
    strFind = IOS_DOC_SPAIN_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "4";
    }
    pos = 0;
    strFind = ANDROID_DOC_SPAIN_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "4";
    }


    pos = 0;
    strFind = IOS_DOC_CHINESE_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "6";
    }
    pos = 0;
    strFind = ANDROID_DOC_CHINESE_FILE_NAME;
    if (strDocName.find(strFind, pos) != string::npos)
    {
        strDocId = "6";
    }


    if ("" == strDocId)
    {
        strDocId = "0";
    }

    return strDocId;
}

std::string CStaticDataDoc::GetLangName(const string &strDocId)
{
    string strLangName = "";
    if ("0" == strDocId)
    {
        strLangName = "english";
    }
    else if ("1" == strDocId)
    {
        strLangName = "german";
    }
    else if ("2" == strDocId)
    {
        strLangName = "french";
    }
    else if ("4" == strDocId)
    {
        strLangName = "spain";
    }
    else if ("5" == strDocId)
    {
        strLangName = "russian";
    }
    else if ("6" == strDocId)
    {
        strLangName = "chinese";
    }
    else
    {
        strLangName = "english";
    }
    return strLangName;
}



// ====================================================================================== //

TINT32 CStaticDataEquip::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    m_jStaticDataJson["default"]["json"] = Json::Value(Json::arrayValue);
    m_jStaticDataJson["default"]["de"]["content"] = strDeString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnMd5;
    m_jStaticDataJson["default"]["reload"] = 0;
    m_jStaticDataJson["default"]["md5"] = "0";

    return 0;
}

TINT32 CStaticDataEquip::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strPlatform].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["md5"].asString();

    }
    return 0;
}

TINT32 CStaticDataEquip::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        
        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;

        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;

        TUINT32 udwEncryptDataLen = 0;

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo) + "." + stRouteInfo.m_strVs;
        if ("" == strFileName)
        {
            return -1;
        }

        /*
        std::ifstream is(stRouteInfo.m_strStaticFilePath.c_str());
        if (is.is_open())
        {
            std::stringstream buffer;
            buffer << is.rdbuf();
            strDeString = buffer.str();
            is.close();
            if ("" == strDeString)
            {
                strDeString = "\"null\"";
            }
        }
        */

        std::ifstream is;
        Json::Reader reader;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        Json::Value jEquipJson;
        jEquipJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jEquipJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }
        strDeString = jsonWriter.write(jEquipJson);


        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);

        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"] = jEquipJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"] = strDeString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"] = uddwDeMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"] = strEnString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"] = uddwEnMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"] = stRouteInfo.m_udwReload;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["md5"] = CStaticFileMgr::GetInstance()->GetStaticFileMd5(EN_STATIC_TYPE_EQUIP, stRouteInfo);
        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_EQUIP, uddwDeMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;

}

string CStaticDataEquip::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";

    if ("ios" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = IOS_EQUIP_FILE_NAME;
    }
    else if ("android" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = ANDROID_EQUIP_FILE_NAME;
    }
    else
    {
        // do nothing
    }
    return strStaticDataName;

}




// ====================================================================================== //

TINT32 CStaticDataGuide::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;


    string strEnString = "";
    TUINT32 udwEncryptDataLen = 0;

    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;
    TUINT64 uddwEnMd5 = 0;
    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;
   

    strDeDataString = "[{\"key\":\"[op_guide:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_guide:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }

    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;

    return 0;
}

TINT32 CStaticDataGuide::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strPlatform].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();
    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["md5"].asString();
       
    }
    return 0;
}

TINT32 CStaticDataGuide::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        
        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;

        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;

        TUINT32 udwEncryptDataLen = 0;





        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo) + "." + stRouteInfo.m_strVs;
        if ("" == strFileName)
        {
            return -1;
        }


        /*
        std::ifstream is(stRouteInfo.m_strStaticFilePath.c_str());
        if (is.is_open())
        {
            std::stringstream buffer;
            buffer << is.rdbuf();
            strDeString = buffer.str();
            is.close();
            if ("" == strDeString)
            {
                strDeString = "\"null\"";
            }
        }
        */

        std::ifstream is;
        Json::Reader reader;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        Json::Value jGuideJson;
        jGuideJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jGuideJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }

        strDeString = jsonWriter.write(jGuideJson);

        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);


        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        strDeDataString = "[{\"key\":\"[op_guide:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_guide:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);

        Json::Value jTmpJson;
        Json::Reader oJsonReader;
        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }

        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"] = jTmpJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"] = strDeDataString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"] = strEnString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"] = uddwEnDataMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"] = stRouteInfo.m_udwReload;
        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_GUIDE, uddwDeDataMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;

}

string CStaticDataGuide::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";

    if ("ios" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = IOS_GUIDE_FILE_NAME;
    }
    else if ("android" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = ANDROID_GUIDE_FILE_NAME;
    }
    else
    {
        // do nothing
    }
    return strStaticDataName;

}



// ====================================================================================== //

TINT32 CStaticDataMeta::InitStaticData(CTseLogger *poServLog)
{
    /*
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_meta:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_meta:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }

    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;


    return 0;
    */


    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;
    Json::Value jTmpJson;
    Json::Reader oJsonReader;


    strDeDataString = "[{\"key\":\"[op_meta:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    jTmpJson.clear();
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }
    m_jStaticDataJson["default"]["json"] = jTmpJson;




    strDeDataString = "[{\"key\":\"[op_static_file:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_static_file:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);
    jTmpJson.clear();
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }
    m_jStaticDataJson["default"]["info_json"] = jTmpJson;

    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;


    return 0;



    
}

TINT32 CStaticDataMeta::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strPlatform].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_jDataInfoJson = m_jStaticDataJson["default"]["info_json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"];
        pstStaticFileInfo->m_jDataInfoJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["info_json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"].asUInt();

    }
    return 0;
}

TINT32 CStaticDataMeta::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{

    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo) + "." + stRouteInfo.m_strVs;
        if ("" == strFileName)
        {
            return -1;
        }

        std::ifstream is;
        Json::Reader reader;
        Json::Value jMetaJson;
        jMetaJson.clear();
        // TBOOL bVaildJson = TRUE;
        is.open(stRouteInfo.m_strStaticFilePath.c_str());

        is.seekg(0, ios::end);
        TUINT32 udwMetaSize = is.tellg();
        is.seekg(0, ios::beg);

        TSE_LOG_DEBUG(m_poServLog, ("CStaticDataMeta::UpdateStaticData, [platform=%s] [version=%s] [udwMetaSize=%u]", \
                                    stRouteInfo.m_strPlatform.c_str(), stRouteInfo.m_strVs.c_str(), udwMetaSize));
        if (false == reader.parse(is, jMetaJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }        

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;

        //string strS3CdnUrl = "http://d2ggugd94z1qg1.cloudfront.net/" + CConfBase::GetString("project") + "/versionconfig";
        //string strS3BackUpUrl = "http://bob-pro.simpysam.com/pro_sys/data_meta/globalconfig/";

        //string strS3CdnUrl = "http://leyi-game-static-file-test.s3.cn-north-1.amazonaws.com.cn/bob_test_group/versionconfig";
        //string strS3BackUpUrl = "http://leyi-game-static-file-test.s3.cn-north-1.amazonaws.com.cn/bob_test_group/versionconfig";

        string strS3CdnUrl = "http://54.223.156.94:9090/op.simpysam.com/bob_test_group_op_sys/pro_sys/data/versionconfig";
        string strS3BackUpUrl = "http://54.223.156.94:9090/op.simpysam.com/bob_test_group_op_sys/pro_sys/data/versionconfig";

        strDeString = jsonWriter.write(jMetaJson);




        uddwDeMd5 = GetStringMD5(strDeString);
        strDeDataString = "[{\"key\":\"[op_meta:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);

        Json::Value jDataJson;
        Json::Reader oJsonReader;
        if (FALSE == oJsonReader.parse(strDeDataString, jDataJson))
        {
            jDataJson = Json::Value(Json::arrayValue);
        }


        Json::Value jDataInfoJson;
        jDataInfoJson.clear();
        jDataInfoJson["name"] = strFileName;
        jDataInfoJson["md5"] = NumToString(uddwDeDataMd5);
        //jDataInfoJson["size"] = strDeString.length();
        jDataInfoJson["size"] = udwMetaSize;
        jDataInfoJson["ts"] = stRouteInfo.m_udwFileTs;
        jDataInfoJson["url"] = strS3CdnUrl + "/" + stRouteInfo.m_strVs + "/" + stRouteInfo.m_strPlatform;
        jDataInfoJson["url_bak"] = strS3BackUpUrl + "/" + stRouteInfo.m_strPlatform + "/" + stRouteInfo.m_strPlatform + "_meta/" + stRouteInfo.m_strVs;
        strDeString = jsonWriter.write(jDataInfoJson);



        strDeDataString = "[{\"key\":\"[op_static_file:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_static_file:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);


        jDataInfoJson.clear();
        if (FALSE == oJsonReader.parse(strDeDataString, jDataInfoJson))
        {
            jDataInfoJson = Json::Value(Json::arrayValue);
        }

        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["json"] = jDataJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["info_json"] = jDataInfoJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["content"] = strDeDataString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["content"] = strEnString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["en"]["md5"] = uddwEnDataMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs]["reload"] = stRouteInfo.m_udwReload;

        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_META, uddwDeDataMd5, stRouteInfo.m_udwReload);

        
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }
    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;



    /*
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        std::ifstream is;
        Json::Reader reader;
        Json::Value jMetaJson;
        jMetaJson.clear();
        // TBOOL bVaildJson = TRUE;
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jMetaJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        Json::Value::Members jMemberVs = jMetaJson.getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberVs.size(); ++udwIdx)
        {
            strDeString = jsonWriter.write(jMetaJson[jMemberVs[udwIdx]]);

            uddwDeMd5 = GetStringMD5(strDeString);
            EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
            uddwEnMd5 = GetStringMD5(strEnString);

     

            strDeDataString = "[{\"key\":\"[op_meta:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
            uddwDeDataMd5 = GetStringMD5(strDeDataString);
            strEnDataString = "[{\"key\":\"[op_meta:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
            EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
            uddwEnDataMd5 = GetStringMD5(strEnString);


            Json::Value jTmpJson;
            Json::Reader oJsonReader;
            if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
            {
                jTmpJson = Json::Value(Json::arrayValue);
            }

            {
                GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

                m_jStaticDataJson[stRouteInfo.m_strPlatform][jMemberVs[udwIdx]]["json"] = jTmpJson;
                m_jStaticDataJson[stRouteInfo.m_strPlatform][jMemberVs[udwIdx]]["de"]["content"] = strDeDataString;
                m_jStaticDataJson[stRouteInfo.m_strPlatform][jMemberVs[udwIdx]]["de"]["md5"] = uddwDeDataMd5;
                m_jStaticDataJson[stRouteInfo.m_strPlatform][jMemberVs[udwIdx]]["en"]["content"] = strEnString;
                m_jStaticDataJson[stRouteInfo.m_strPlatform][jMemberVs[udwIdx]]["en"]["md5"] = uddwEnDataMd5;
                m_jStaticDataJson[stRouteInfo.m_strPlatform][jMemberVs[udwIdx]]["reload"] = stRouteInfo.m_udwReload;

            }

            stRouteInfo.m_strVs = jMemberVs[udwIdx];
            CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_META, uddwDeDataMd5, stRouteInfo.m_udwReload);
            
        }    
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }
    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
    */

}

string CStaticDataMeta::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";

    if ("ios" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = IOS_META_FILE_NAME;
    }
    else if ("android" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = ANDROID_META_FILE_NAME;
    }
    else
    {
        // do nothing
    }
    return strStaticDataName;

}

// ====================================================================================== //

CStaticDataLake::CStaticDataLake()
{

}


CStaticDataLake::~CStaticDataLake()
{
   
    map<string, SMapDataInfo>::iterator it;
    for (it = mapSidMap.begin(); it != mapSidMap.end(); ++it)
    {
        delete it->second.m_pszMapData;
    }
    mapSidMap.clear();
}


TINT32 CStaticDataLake::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;

 
    m_jStaticDataJson["default"]["json"] = Json::Value(Json::arrayValue);
    m_jStaticDataJson["default"]["de"]["content"] = "";
    m_jStaticDataJson["default"]["de"]["md5"] = 0;
    m_jStaticDataJson["default"]["en"]["content"] = "";
    m_jStaticDataJson["default"]["en"]["md5"] = 0;
    m_jStaticDataJson["default"]["reload"] = 0;
    m_jStaticDataJson["default"]["md5"] = "0";

    return 0;
}

TINT32 CStaticDataLake::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    if (m_jStaticDataJson[stRouteInfo.m_strSid].empty())
    {
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();
    }
    else
    {
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson[stRouteInfo.m_strSid]["md5"].asString();
    }

    return 0;
}

TINT32 CStaticDataLake::GetStaticData(const SRouteInfo &stRouteInfo, TCHAR *pMapData, TUINT32 &udwMapDataLen)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);

    map<string, SMapDataInfo>::iterator it;
    it = mapSidMap.find(stRouteInfo.m_strSid);
    if (it != mapSidMap.end())
    {
        memcpy(pMapData, it->second.m_pszMapData, it->second.m_udwMapDataLen);
        udwMapDataLen = it->second.m_udwMapDataLen;
        return 0;
    }
    else
    {
        return -1;
    }
}


TINT32 CStaticDataLake::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{

    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {

        

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }


        TUINT64 uddwDeMd5 = 0;
        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);


            TBOOL bHasMap = FALSE;
            TCHAR *pszMapDataBuff = NULL;
            map<string, SMapDataInfo>::iterator it;
            it = mapSidMap.find(stRouteInfo.m_strSid);
            if (it == mapSidMap.end())
            {
                bHasMap = FALSE;
                pszMapDataBuff = new TCHAR[600 * 1200];
            }
            else
            {
                bHasMap = TRUE;
                pszMapDataBuff = it->second.m_pszMapData;
            }



            
            TINT32 dwReadBytes = 0;
            TINT32 dwMapDataFileSize = GetMapFileSize(stRouteInfo.m_strStaticFilePath.c_str());
            if (dwMapDataFileSize > 0)
            {
                std::ifstream is(stRouteInfo.m_strStaticFilePath.c_str(), std::ios::binary);
                if (!is.is_open())
                {
                    is.clear();
                    is.close();
                    return -2;
                }
                
                while (!is.eof() && dwReadBytes < dwMapDataFileSize)
                {

                    is.read(pszMapDataBuff + dwReadBytes, dwMapDataFileSize - dwReadBytes);
                    if (is.fail())
                    {
                        delete pszMapDataBuff;
                        assert(0);
                    }
                    dwReadBytes += is.gcount();
                }
                is.clear();
                is.close();
            }

            uddwDeMd5 = GetRawDataMd5(pszMapDataBuff, dwReadBytes);

   
            m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["md5"] = uddwDeMd5;
            m_jStaticDataJson[stRouteInfo.m_strSid]["reload"] = stRouteInfo.m_udwReload;
            m_jStaticDataJson[stRouteInfo.m_strSid]["md5"] = CStaticFileMgr::GetInstance()->GetStaticFileMd5(EN_STATIC_TYPE_ALLLAKE, stRouteInfo);



            if (FALSE == bHasMap)
            {
                SMapDataInfo stMapDataInfo;
                stMapDataInfo.Reset();

                stMapDataInfo.m_pszMapData = pszMapDataBuff;
                stMapDataInfo.m_udwMapDataLen = dwReadBytes;


                mapSidMap.insert(make_pair(stRouteInfo.m_strSid, stMapDataInfo));
            }

        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_ALLLAKE, uddwDeMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataLake::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = "server_" + stRouteInfo.m_strSid + "_map";
    return strStaticDataName;
}

string CStaticDataLake::GetLakeSid(const string &strLakeName)
{
    TBOOL bGetSid = FALSE;
    string strSid = strLakeName;
    // 初始化document_map
    string strFind = "_map";
    string strReplace = "";
    string::size_type pos = 0;
    string::size_type FindSize = strFind.size();
    string::size_type ReplaceSize = strReplace.size();
    while ((pos = strSid.find(strFind, pos)) != string::npos)
    {
        bGetSid = TRUE;
        strSid.replace(pos, FindSize, strReplace);
        pos += ReplaceSize;
    }
    strFind = "server_";
    strReplace = "";
    pos = 0;
    FindSize = strFind.size();
    ReplaceSize = strReplace.size();
    while ((pos = strSid.find(strFind, pos)) != string::npos)
    {
        bGetSid = TRUE;
        strSid.replace(pos, FindSize, strReplace);
        pos += ReplaceSize;
    }
    if (FALSE == bGetSid)
    {
        strSid = "";
    }

    return strSid;
}

TINT32 CStaticDataLake::GetMapFileSize(const string &strLakeName)
{
    struct stat f_stat;
    if (stat(strLakeName.c_str(), &f_stat) == -1)
    {
        return -1;
    }
    return f_stat.st_size;   
}

// ====================================================================================== //

TINT32 CStaticDataMaintain::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_new_maintain:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_new_maintain:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);


    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }


    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;

    return 0;
}

TINT32 CStaticDataMaintain::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strPlatform].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strPlatform]["reload"].asUInt();

    }
    return 0;
}

TINT32 CStaticDataMaintain::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {


        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        std::ifstream is;
        Json::Reader reader;
        Json::Value jMaintainJson;
        jMaintainJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jMaintainJson))
        {
            is.close();
            return -2;
        }
        else
        {
            is.close();
        }

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        // 计算Maintain数据
        if ("android" == stRouteInfo.m_strPlatform)
        {
            if ("googleplay" == stRouteInfo.m_strExInfo)
            {
                jMaintainJson["global"]["url"] = jMaintainJson["global"]["googleplay_url"].asString();
            }
            else if ("amazon" == stRouteInfo.m_strExInfo)
            {
                jMaintainJson["global"]["url"] = jMaintainJson["global"]["amazon_url"].asString();
            }
            else if ("playphone" == stRouteInfo.m_strExInfo)
            {
                jMaintainJson["global"]["url"] = jMaintainJson["global"]["playphone_url"].asString();
            }
            else
            {
                jMaintainJson["global"]["url"] = jMaintainJson["global"]["googleplay_url"].asString();
            }
        }
        else if ("ios" == stRouteInfo.m_strPlatform)
        {
            // todo::ios的url转换
            jMaintainJson["global"]["url"] = jMaintainJson["global"]["url"].asString();
        }
        else
        {
            return -3;
        }
        jMaintainJson["global"]["min_ver"] = atof(jMaintainJson["global"]["min_ver"].asString().c_str());
        jMaintainJson["global"]["max_ver"] = atof(jMaintainJson["global"]["max_ver"].asString().c_str());


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
        if (udwCurTime > udwGlobalBeginTime)
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


        strDeString = jsonWriter.write(jMaintainJson);

        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);


        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        strDeDataString = "[{\"key\":\"[op_new_maintain:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_new_maintain:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);

        Json::Value jTmpJson;
        Json::Reader oJsonReader;
        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }

        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

            m_jStaticDataJson[stRouteInfo.m_strPlatform]["json"] = jTmpJson;
            m_jStaticDataJson[stRouteInfo.m_strPlatform]["de"]["content"] = strDeDataString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform]["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform]["en"]["content"] = strEnString;
            m_jStaticDataJson[stRouteInfo.m_strPlatform]["en"]["md5"] = uddwEnDataMd5;
            m_jStaticDataJson[stRouteInfo.m_strPlatform]["reload"] = stRouteInfo.m_udwReload;
        }
        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_NEW_MAINTAIN, uddwDeDataMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        Json::Value jTmpStaticDataJson;
        jTmpStaticDataJson.clear();
        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
            jTmpStaticDataJson = m_jStaticDataJson;
        }

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();



        Json::Value::Members jMemberPlatform = jTmpStaticDataJson.getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberPlatform.size(); ++udwIdx)
        {

            if ("default" == jMemberPlatform[udwIdx])
            {
                continue;
            }
            Json::Value &jPlatformMaintainJson = jTmpStaticDataJson[jMemberPlatform[udwIdx]]["json"][0]["data"];

            TBOOL bComputeUpdate = FALSE;
            TUINT32 udwMaintainStatus = 0;
            TUINT32 udwCurTime = CTimeUtils::GetUnixTime();
            TUINT32 udwGlobalBeginTime =jPlatformMaintainJson["global"]["begin"].asUInt();
            TUINT32 udwGlobalEndTime = jPlatformMaintainJson["global"]["end"].asUInt();
            udwMaintainStatus = jPlatformMaintainJson["global"]["status"].asUInt();
            if (udwCurTime > udwGlobalEndTime)
            {
                jPlatformMaintainJson["global"]["status"] = 0U;
            }
            else if (udwGlobalBeginTime <= udwCurTime && udwCurTime <= udwGlobalEndTime)
            {
                jPlatformMaintainJson["global"]["status"] = 2U;
            }
            else
            {
                jPlatformMaintainJson["global"]["status"] = 1U;
            }
            if (udwMaintainStatus != jPlatformMaintainJson["global"]["status"].asUInt())
            {
                bComputeUpdate = TRUE;
            }

            
            TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::UpdateStaticData, [platform=%s] global [udwCurTime=%u] [udwGlobalBeginTime=%u] [udwGlobalEndTime=%u] [oldstatus=%u] [newstatus=%u]", 
                jMemberPlatform[udwIdx].c_str(), udwCurTime, udwGlobalBeginTime, udwGlobalEndTime, udwMaintainStatus, jPlatformMaintainJson["global"]["status"].asUInt()));
            

            if (!jPlatformMaintainJson["svrList"].empty())
            {
                Json::Value::Members jMemberSid = jPlatformMaintainJson["svrList"].getMemberNames();
                for (TUINT32 udwIdy = 0; udwIdy < jMemberSid.size(); ++udwIdy)
                {
                    if (jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]].empty())
                    {
                        continue;
                    }

                    TUINT32 udwSvrBeginTime = jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["sb"].asUInt();
                    TUINT32 udwSvrEndTime = jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["se"].asUInt();
                    udwMaintainStatus = jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"].asUInt();
                    if (udwCurTime > udwSvrEndTime)
                    {
                        jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"] = 0U;
                    }
                    else if (udwSvrBeginTime <= udwCurTime && udwCurTime <= udwSvrEndTime)
                    {
                        jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"] = 2U;
                    }
                    else
                    {
                        jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"] = 1U;
                    }
                    if (udwMaintainStatus != jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"].asUInt())
                    {
                        bComputeUpdate = TRUE;
                    }
                    TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::UpdateStaticData, [platform=%s] svr [udwCurTime=%u] [udwSvrBeginTime=%d] [udwSvrEndTime=%d] [oldstatus=%u] [newstatus=%u]",
                        jMemberPlatform[udwIdx].c_str(), udwCurTime, udwSvrBeginTime, udwSvrEndTime, udwMaintainStatus, jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"].asUInt()));

                    if (0 == jPlatformMaintainJson["svrList"][jMemberSid[udwIdy]]["status"].asInt())
                    {
                        jPlatformMaintainJson["svrList"].removeMember(jMemberSid[udwIdy]);
                    }
                    

                }
               
            }

            if (TRUE == bComputeUpdate)
            {
                strDeString = jsonWriter.write(jTmpStaticDataJson[jMemberPlatform[udwIdx]]["json"]);
                TSE_LOG_DEBUG(m_poServLog, ("CStaticFileMgr::UpdateStaticData, [platform=%s] maintain_update [content=%s]", jMemberPlatform[udwIdx].c_str(), strDeString.c_str()));


                strDeString = jsonWriter.write(jTmpStaticDataJson[jMemberPlatform[udwIdx]]["json"]);

                uddwDeMd5 = GetStringMD5(strDeString);
                EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
                uddwEnMd5 = GetStringMD5(strEnString);


                {
                    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

                    m_jStaticDataJson[jMemberPlatform[udwIdx]]["json"] = jTmpStaticDataJson[jMemberPlatform[udwIdx]]["json"];
                    m_jStaticDataJson[jMemberPlatform[udwIdx]]["de"]["content"] = strDeString;
                    m_jStaticDataJson[jMemberPlatform[udwIdx]]["de"]["md5"] = uddwDeMd5;
                    m_jStaticDataJson[jMemberPlatform[udwIdx]]["en"]["content"] = strEnString;
                    m_jStaticDataJson[jMemberPlatform[udwIdx]]["en"]["md5"] = uddwEnMd5;

                }

                SRouteInfo stRouteInfo;
                stRouteInfo.Reset();
                stRouteInfo.m_strPlatform = jMemberPlatform[udwIdx];
                CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_NEW_MAINTAIN, uddwDeMd5, -1);
                CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
                
            }
        }
       
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataMaintain::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";

    if ("ios" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = IOS_MAINTAIN_FILE_NAME;
    }
    else if ("android" == stRouteInfo.m_strPlatform)
    {
        strStaticDataName = ANDROID_MAINTAIN_FILE_NAME;
    }
    else
    {
        // do nothing
    }
    return strStaticDataName;

}

TINT32 CStaticDataMaintain::GetMaintainStatus(const SRouteInfo &stRouteInfo, const Json::Value &jMaintainPlatformJson)
{
    // 强制更新

    if (atof(stRouteInfo.m_strVs.c_str()) + 0.001f < jMaintainPlatformJson["global"]["min_ver"].asFloat())
    {
        return EN_MAINTAIN_TYPE_VERSION_FORCE_UPDATE;
    }

    if (2 == jMaintainPlatformJson["global"]["status"].asInt())
    {
        return EN_MAINTAIN_TYPE_GLOBAL_MAINTAIN;
    }

    if ("-1" == stRouteInfo.m_strSid)
    {
        return EN_MAINTAIN_TYPE_NORMAL;
    }

    if (false == jMaintainPlatformJson["svrList"].empty()
        && 2 == jMaintainPlatformJson["svrList"][stRouteInfo.m_strSid]["status"].asUInt())
    {
        return EN_MAINTAIN_TYPE_SVR_MAINTAIN;
    }
    return EN_MAINTAIN_TYPE_NORMAL;
}


// ====================================================================================== //

TINT32 CStaticDataSvrConf::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_svr_conf:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_svr_conf:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }


    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;

    return 0;
}

TINT32 CStaticDataSvrConf::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strSid].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();
 
    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strSid]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strSid]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strSid]["reload"].asUInt();

    }
    return 0;
}

TINT32 CStaticDataSvrConf::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        

        string strSvrConfFileName = "";
        strSvrConfFileName = GetStaticDataSvrConfName(stRouteInfo);
        if ("" == strSvrConfFileName)
        {
            return -1;
        }

        string strFunctionSwitchFileName = "";
        strFunctionSwitchFileName = GetStaticDataFunctionSwitchName(stRouteInfo);
        if ("" == strFunctionSwitchFileName)
        {
            return -2;
        }

        
        std::ifstream is;
        Json::Reader reader;
        Json::Value jSvrConfJson;
        jSvrConfJson.clear();
        Json::Value jFunctionSwitchJson;
        jFunctionSwitchJson.clear();
        Json::Value jsvrJson;
        jsvrJson.clear();

        strSvrConfFileName = "../data/globalconfig/common/svr.json";
        strFunctionSwitchFileName = "../data/globalconfig/common/function_switch.json";

        is.open(strSvrConfFileName.c_str());
        if (false == reader.parse(is, jSvrConfJson))
        {
            is.close();
            return -2;
        }
        else
        {
            is.close();
        }

        
        is.open(strFunctionSwitchFileName.c_str());
        if (false == reader.parse(is, jFunctionSwitchJson))
        {
            is.close();
            return -2;
        }
        else
        {
            is.close();
        }
        
      

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();


        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        Json::Value::Members jMemberSid = jSvrConfJson["svr_json"].getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberSid.size(); ++udwIdx)
        {
            jsvrJson[jMemberSid[udwIdx]] = jSvrConfJson["svr_json"][jMemberSid[udwIdx]];

            for (TUINT32 udwIdy = 0; udwIdy < jFunctionSwitchJson.size(); ++udwIdy)
            {
                // 默认开放
                if ("0" == jFunctionSwitchJson[udwIdy]["default_switch_type"].asString())
                {
                    for (TUINT32 udwIdz = 0; udwIdz < jFunctionSwitchJson[udwIdy]["svr_list"].size(); ++udwIdz)
                    {
                        jsvrJson[jMemberSid[udwIdx]]["svr_switch"][jFunctionSwitchJson[udwIdy]["function_id"].asString()] = 0;
                        if (jMemberSid[udwIdx] == jFunctionSwitchJson[udwIdy]["svr_list"][udwIdz].asString())
                        {
                            jsvrJson[jMemberSid[udwIdx]]["svr_switch"][jFunctionSwitchJson[udwIdy]["function_id"].asString()] = 1;
                        }
                    }
                }
                if ("1" == jFunctionSwitchJson[udwIdy]["default_switch_type"].asString())
                {
                    for (TUINT32 udwIdz = 0; udwIdz < jFunctionSwitchJson[udwIdy]["svr_list"].size(); ++udwIdz)
                    {
                        jsvrJson[jMemberSid[udwIdx]]["svr_switch"][jFunctionSwitchJson[udwIdy]["function_id"].asString()] = 1;
                        if (jMemberSid[udwIdx] == jFunctionSwitchJson[udwIdy]["svr_list"][udwIdz].asString())
                        {
                            jsvrJson[jMemberSid[udwIdx]]["svr_switch"][jFunctionSwitchJson[udwIdy]["function_id"].asString()] = 0;
                        }
                    }
                }
            }

            strDeString = jsonWriter.write(jsvrJson[jMemberSid[udwIdx]]);
            uddwDeMd5 = GetStringMD5(strDeString);
            EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
            uddwEnMd5 = GetStringMD5(strEnString);


            strDeDataString = "[{\"key\":\"[op_svr_conf:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
            uddwDeDataMd5 = GetStringMD5(strDeDataString);
            strEnDataString = "[{\"key\":\"[op_svr_conf:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
            EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
            uddwEnDataMd5 = GetStringMD5(strEnString);

            Json::Value jTmpJson;
            Json::Reader oJsonReader;
            if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
            {
                jTmpJson = Json::Value(Json::arrayValue);
            }

            {
                GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);
                m_jStaticDataJson[jMemberSid[udwIdx]]["json"] = jTmpJson;
                m_jStaticDataJson[jMemberSid[udwIdx]]["de"]["content"] = strDeDataString;
                m_jStaticDataJson[jMemberSid[udwIdx]]["de"]["md5"] = uddwDeDataMd5;
                m_jStaticDataJson[jMemberSid[udwIdx]]["en"]["content"] = strEnString;
                m_jStaticDataJson[jMemberSid[udwIdx]]["en"]["md5"] = uddwEnDataMd5;
                m_jStaticDataJson[jMemberSid[udwIdx]]["reload"] = stRouteInfo.m_udwReload;
            }

            stRouteInfo.m_strSid = jMemberSid[udwIdx];
            CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_SVRCONF, uddwDeDataMd5, stRouteInfo.m_udwReload);
            
           
        }
       
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }
    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        

    }
    else
    {
        // do nothing
    }

   

    return 0;
}

string CStaticDataSvrConf::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    return "";
}

string CStaticDataSvrConf::GetStaticDataSvrConfName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = SVR_FILE_NAME;
    return strStaticDataName;
}

string CStaticDataSvrConf::GetStaticDataFunctionSwitchName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = FUNCTIOTN_FILE_NAME;
    return strStaticDataName;
}


// ====================================================================================== //

TINT32 CStaticDataClient::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_client_conf:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_client_conf:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }

    m_jStaticDataJson["json"] = jTmpJson;
    m_jStaticDataJson["de"]["content"] = strDeDataString;
    m_jStaticDataJson["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["en"]["content"] = strEnString;
    m_jStaticDataJson["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["reload"] = 0;

    return 0;
}

TINT32 CStaticDataClient::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);

    pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["json"];
    pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["de"]["content"].asString();
    pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["de"]["md5"].asUInt64();
    pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["en"]["content"].asString();
    pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["en"]["md5"].asUInt64();
    pstStaticFileInfo->m_udwReload = m_jStaticDataJson["reload"].asUInt();
    pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["md5"].asString();


    return 0;

}

TINT32 CStaticDataClient::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        
        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;

        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;

        TUINT32 udwEncryptDataLen = 0;

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        /*
        std::ifstream is(stRouteInfo.m_strStaticFilePath.c_str());
        if (is.is_open())
        {
            std::stringstream buffer;
            buffer << is.rdbuf();
            strDeString = buffer.str();
            is.close();
            if ("" == strDeString)
            {
                strDeString = "\"null\"";
            }
        }
        */

        std::ifstream is;
        Json::Reader reader;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        Json::Value jClientJson;
        jClientJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jClientJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }

        strDeString = jsonWriter.write(jClientJson);

        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);

        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        strDeDataString = "[{\"key\":\"[op_client_conf:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_client_conf:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);


        Json::Value jTmpJson;
        Json::Reader oJsonReader;
        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }


        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);
            m_jStaticDataJson["json"] = jTmpJson;
            m_jStaticDataJson["de"]["content"] = strDeDataString;
            m_jStaticDataJson["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson["en"]["content"] = strEnString;
            m_jStaticDataJson["en"]["md5"] = uddwEnDataMd5;
            m_jStaticDataJson["reload"] = stRouteInfo.m_udwReload;
        }

        SRouteInfo stRouteInfo;
        stRouteInfo.Reset();
        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_CLIENTCONF, uddwDeDataMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataClient::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = CLIENT_FILE_NAME;
    return strStaticDataName;
}

// ====================================================================================== //

TINT32 CStaticDataGlobalConf::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_global_conf:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_global_conf:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);


    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }


    m_jStaticDataJson["json"] = jTmpJson;
    m_jStaticDataJson["de"]["content"] = strDeDataString;
    m_jStaticDataJson["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["en"]["content"] = strEnString;
    m_jStaticDataJson["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["reload"] = 0;


    return 0;
}

TINT32 CStaticDataGlobalConf::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);

    pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["json"];
    pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["de"]["content"].asString();
    pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["de"]["md5"].asUInt64();
    pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["en"]["content"].asString();
    pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["en"]["md5"].asUInt64();
    pstStaticFileInfo->m_udwReload = m_jStaticDataJson["reload"].asUInt();
    pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["md5"].asString();


    return 0;

}

TINT32 CStaticDataGlobalConf::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;

        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;

        TUINT32 udwEncryptDataLen = 0;

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        /*
        std::ifstream is(strFileName.c_str());
        if (is.is_open())
        {
        std::stringstream buffer;
        buffer << is.rdbuf();
        strDeString = buffer.str();
        is.close();
        if ("" == strDeString)
        {
        strDeString = "\"null\"";
        }
        }
        */


        std::ifstream is;
        Json::Reader reader;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();

        Json::Value jGlobalConfJson;
        jGlobalConfJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jGlobalConfJson))
        {
            is.close();
            return -1;
        }
        else
        {
            is.close();
        }

        strDeString = jsonWriter.write(jGlobalConfJson);


        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);

        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        strDeDataString = "[{\"key\":\"[op_global_conf:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_global_conf:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);

        Json::Value jTmpJson;
        Json::Reader oJsonReader;
        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }


        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);
            m_jStaticDataJson["json"] = jTmpJson;
            m_jStaticDataJson["de"]["content"] = strDeDataString;
            m_jStaticDataJson["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson["en"]["content"] = strEnString;
            m_jStaticDataJson["en"]["md5"] = uddwEnDataMd5;
            m_jStaticDataJson["reload"] = stRouteInfo.m_udwReload;
        }

        SRouteInfo stRouteInfo;
        stRouteInfo.Reset();
        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_GLOBALCONF, uddwDeDataMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataGlobalConf::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = GLOBAL_CONF_FILE_NAME;
    return strStaticDataName;
}

// ====================================================================================== //

TINT32 CStaticDataOnsale::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;

    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnNull = "";
    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    strEnNull = strEnString;
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;

    Json::Value jTmpJson;
    Json::Reader oJsonReader;


    strDeDataString = "[{\"key\":\"[op_item_switch:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_item_switch:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);



    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }

    m_jStaticDataJson["item_switch"]["default"]["json"] = jTmpJson;
    m_jStaticDataJson["item_switch"]["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["item_switch"]["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["item_switch"]["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["item_switch"]["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["item_switch"]["default"]["reload"] = 0;



    strDeDataString = "[{\"key\":\"[op_al_store:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_al_store:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnNull.length()) + "]\",\"data\":\"" + strEnNull + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }


    m_jStaticDataJson["al_store"]["default"]["json"] = jTmpJson;
    m_jStaticDataJson["al_store"]["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["al_store"]["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["al_store"]["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["al_store"]["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["al_store"]["default"]["reload"] = 0;


    return 0;
}

TINT32 CStaticDataOnsale::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson[stRouteInfo.m_strExInfo]["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strExInfo][stRouteInfo.m_strSid]["reload"].asUInt();

    }
    return 0;
}

TINT32 CStaticDataOnsale::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        std::ifstream is;
        Json::Reader reader;
        Json::Value jOnSaleJson;
        jOnSaleJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jOnSaleJson))
        {
            is.close();
            return -2;
        }
        else
        {
            is.close();
        }

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();


        Json::Value jTmpJson;
        Json::Reader oJsonReader;

        strDeString = jsonWriter.write(jOnSaleJson["onsale"]);

        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);


        string strDeDataString = "";
        string strEnDataString = "";
        TUINT64 uddwDeDataMd5 = 0;
        TUINT64 uddwEnDataMd5 = 0;


        strDeDataString = "[{\"key\":\"[op_item_switch:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_item_switch:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);



        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }

        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);
            m_jStaticDataJson["item_switch"][stRouteInfo.m_strSid]["json"] = jTmpJson;
            m_jStaticDataJson["item_switch"][stRouteInfo.m_strSid]["de"]["content"] = strDeDataString;
            m_jStaticDataJson["item_switch"][stRouteInfo.m_strSid]["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson["item_switch"][stRouteInfo.m_strSid]["en"]["content"] = strEnString;
            m_jStaticDataJson["item_switch"][stRouteInfo.m_strSid]["en"]["md5"] = uddwEnDataMd5;
        }

        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_ITEMSWITCH, uddwDeDataMd5, stRouteInfo.m_udwReload);


        strDeString = jsonWriter.write(jOnSaleJson["al_store"]);

        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);



        strDeDataString = "[{\"key\":\"[op_al_store:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_al_store:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);


        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }

        {
            GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);
            m_jStaticDataJson["al_store"][stRouteInfo.m_strSid]["json"] = jTmpJson;
            m_jStaticDataJson["al_store"][stRouteInfo.m_strSid]["de"]["content"] = strDeDataString;
            m_jStaticDataJson["al_store"][stRouteInfo.m_strSid]["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson["al_store"][stRouteInfo.m_strSid]["en"]["content"] = strEnString;
            m_jStaticDataJson["al_store"][stRouteInfo.m_strSid]["en"]["md5"] = uddwEnDataMd5;
        }



        CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_ALSTORE, uddwDeDataMd5, stRouteInfo.m_udwReload);
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataOnsale::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = ONSALE_FILE_NAME;
    return strStaticDataName;
}



// ====================================================================================== //

TINT32 CStaticDataNotice::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_notice:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_notice:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }


    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;


    return 0;
}

TINT32 CStaticDataNotice::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strSid].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();


    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strSid]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strSid]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strSid]["reload"].asUInt64();

    }
    return 0;
}

TINT32 CStaticDataNotice::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        return 0;

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        std::ifstream is;
        Json::Reader reader;
        Json::Value jNoticeJson;
        jNoticeJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jNoticeJson))
        {
            is.close();
            return -2;
        }
        else
        {
            is.close();
        }
        

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();


        Json::Value::Members jMemberSid = jNoticeJson.getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberSid.size(); ++udwIdx)
        {
            strDeString = jsonWriter.write(jNoticeJson[jMemberSid[udwIdx]]);

            uddwDeMd5 = GetStringMD5(strDeString);
            EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
            uddwEnMd5 = GetStringMD5(strEnString);

            string strDeDataString = "";
            string strEnDataString = "";
            TUINT64 uddwDeDataMd5 = 0;
            TUINT64 uddwEnDataMd5 = 0;


            strDeDataString = "[{\"key\":\"[op_notice:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
            uddwDeDataMd5 = GetStringMD5(strDeDataString);
            strEnDataString = "[{\"key\":\"[op_notice:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
            EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
            uddwEnDataMd5 = GetStringMD5(strEnString);

            Json::Value jTmpJson;
            Json::Reader oJsonReader;
            if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
            {
                jTmpJson = Json::Value(Json::arrayValue);
            }

            {
                GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);
                m_jStaticDataJson[jMemberSid[udwIdx]]["json"] = jTmpJson;
                m_jStaticDataJson[jMemberSid[udwIdx]]["de"]["content"] = strDeDataString;
                m_jStaticDataJson[jMemberSid[udwIdx]]["de"]["md5"] = uddwDeDataMd5;
                m_jStaticDataJson[jMemberSid[udwIdx]]["en"]["content"] = strEnString;
                m_jStaticDataJson[jMemberSid[udwIdx]]["en"]["md5"] = uddwEnDataMd5;
                m_jStaticDataJson[jMemberSid[udwIdx]]["reload"] = stRouteInfo.m_udwReload;
            }

            stRouteInfo.m_strSid = jMemberSid[udwIdx];
            CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_NOTICE, uddwDeDataMd5, stRouteInfo.m_udwReload);

        }
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataNotice::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = NOTICE_FILE_NAME;
    return strStaticDataName;
}



// ====================================================================================== //

CStaticDataAccountStatusMgr *CStaticDataAccountStatusMgr::m_poStaticDataAccountStatusMgr = NULL;

CStaticDataAccountStatusMgr::CStaticDataAccountStatusMgr()
{

}

CStaticDataAccountStatusMgr::~CStaticDataAccountStatusMgr()
{

}


CStaticDataAccountStatusMgr * CStaticDataAccountStatusMgr::GetInstance()
{
    if (NULL == m_poStaticDataAccountStatusMgr)
    {
        try
        {
            m_poStaticDataAccountStatusMgr = new CStaticDataAccountStatusMgr;
        }
        catch (const std::bad_alloc &memExp)
        {
            assert(m_poStaticDataAccountStatusMgr);
        }
    }
    return m_poStaticDataAccountStatusMgr;
}

TINT32 CStaticDataAccountStatusMgr::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;


    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_IMPORT));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_UNREGISTER));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_ACCOUNT_UNACTIVE));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_ACCOUNT_ACTIVE));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_PASSWORD_CHANGE));
    
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_MULTI_LOGIN));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_BLACK_ACCOUNT));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_ACCOUNT_INFO_INVAILD));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_ACCOUNT_NOT_GAME_DATA));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_MULTI_EMAIL));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_EMAIL_HAS_MULTI_GAME_DATA));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_PRODUCTION_INFO_INVAILD));
    m_setAccountStatus.insert(NumToString(EN_PLAYER_STATUS_USER_DATA_NOT_EXIST));
    


    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);

    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;



    strDeDataString = "[{\"key\":\"[op_account_status:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_account_status:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }

    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;

    set<string>::iterator it;
    for (it = m_setAccountStatus.begin(); it != m_setAccountStatus.end(); ++it)
    {
        strDeString = "{\"status\":" + *it + "}";
        uddwDeMd5 = GetStringMD5(strDeString);
        EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
        uddwEnMd5 = GetStringMD5(strEnString);



        strDeDataString = "[{\"key\":\"[op_account_status:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
        uddwDeDataMd5 = GetStringMD5(strDeDataString);
        strEnDataString = "[{\"key\":\"[op_account_status:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
        EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
        uddwEnDataMd5 = GetStringMD5(strEnString);


        Json::Value jTmpJson;
        Json::Reader oJsonReader;
        if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
        {
            jTmpJson = Json::Value(Json::arrayValue);
        }

        m_jStaticDataJson[*it]["json"] = jTmpJson;
        m_jStaticDataJson[*it]["de"]["content"] = strDeDataString;
        m_jStaticDataJson[*it]["de"]["md5"] = uddwDeDataMd5;
        m_jStaticDataJson[*it]["en"]["content"] = strEnString;
        m_jStaticDataJson[*it]["en"]["md5"] = uddwEnDataMd5;
    }


    return 0;
}

TINT32 CStaticDataAccountStatusMgr::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    return 0;
}


TINT32 CStaticDataAccountStatusMgr::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    return 0;
}

string CStaticDataAccountStatusMgr::GetStaticDataName(const SRouteInfo &stRouteInfo)
{



    return 0;
}


TINT32 CStaticDataAccountStatusMgr::GetStaticData(const TINT32 &dwAccountStatus, SStaticFileInfo *pstStaticFileInfo)
{

    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[NumToString(dwAccountStatus)].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[NumToString(dwAccountStatus)]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[NumToString(dwAccountStatus)]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[NumToString(dwAccountStatus)]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[NumToString(dwAccountStatus)]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[NumToString(dwAccountStatus)]["en"]["md5"].asUInt64();

    }
    return 0;


}



// ====================================================================================== //

CStaticDataMd5ListMgr *CStaticDataMd5ListMgr::m_poStaticDataMd5ListMgr = NULL;

CStaticDataMd5ListMgr::CStaticDataMd5ListMgr()
{
  
}


CStaticDataMd5ListMgr::~CStaticDataMd5ListMgr()
{
}

CStaticDataMd5ListMgr *CStaticDataMd5ListMgr::GetInstance()
{
    if (NULL == m_poStaticDataMd5ListMgr)
    {
        try
        {
            m_poStaticDataMd5ListMgr = new CStaticDataMd5ListMgr;
        }
        catch (const std::bad_alloc &memExp)
        {
            assert(m_poStaticDataMd5ListMgr);
        }
    }
    return m_poStaticDataMd5ListMgr;

}


TINT32 CStaticDataMd5ListMgr::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    return 0;
}

string CStaticDataMd5ListMgr::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    return "";
}



TINT32 CStaticDataMd5ListMgr::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;

    m_setPlatform.clear();
    m_setVs.clear();
    m_setLang.clear();
    m_setSid.clear();


    m_setStaticType.insert(EN_STATIC_TYPE_GAME);
    m_setStaticType.insert(EN_STATIC_TYPE_DOC);
    m_setStaticType.insert(EN_STATIC_TYPE_GUIDE);
    m_setStaticType.insert(EN_STATIC_TYPE_EQUIP);
    m_setStaticType.insert(EN_STATIC_TYPE_META);
    m_setStaticType.insert(EN_STATIC_TYPE_NEW_MAINTAIN);
    m_setStaticType.insert(EN_STATIC_TYPE_NOTICE);
    m_setStaticType.insert(EN_STATIC_TYPE_ITEMSWITCH);
    m_setStaticType.insert(EN_STATIC_TYPE_ALSTORE);
    m_setStaticType.insert(EN_STATIC_TYPE_SVRCONF);
    m_setStaticType.insert(EN_STATIC_TYPE_ALLLAKE);
    m_setStaticType.insert(EN_STATIC_TYPE_CLIENTCONF);
    m_setStaticType.insert(EN_STATIC_TYPE_GLOBALCONF);
    m_setStaticType.insert(EN_STATIC_TYPE_USER_LINK);


    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;


    Json::Value jMd5Json;
    jMd5Json.clear();
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

    Json::FastWriter jsonWriter;
    jsonWriter.omitEndingLineFeed();
    strDeString = jsonWriter.write(jMd5Json);

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_md5:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_md5:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);


    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }

    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;


    return 0;
}

// 一定能够要所有静态文件初始化完成才能调用,因为会在第一次调用的时候重建
TINT32 CStaticDataMd5ListMgr::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if(m_jStaticDataJson[stRouteInfo.m_strPlatform].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strSid].empty()
        || m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][stRouteInfo.m_strSid].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();
 
    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][stRouteInfo.m_strSid]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][stRouteInfo.m_strSid]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][stRouteInfo.m_strSid]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][stRouteInfo.m_strSid]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][stRouteInfo.m_strSid]["en"]["md5"].asUInt64();
       
    }
    return 0;

}


TINT32 CStaticDataMd5ListMgr::UpdateMd5Data(const SRouteInfo &stRouteInfo, const string &strStaticType, const TUINT64 &uddwDeMd5, const TINT32 &dwReload)
{

    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);


    if (EN_STATIC_TYPE_GAME == strStaticType
        || EN_STATIC_TYPE_GUIDE == strStaticType
        || EN_STATIC_TYPE_EQUIP == strStaticType
        || EN_STATIC_TYPE_META == strStaticType)
    {
        /*
        m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][0] = NumToString(uddwDeMd5);
        if (-1 != dwReload)
        {
            m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][1] = dwReload;
        }
        */
        m_setPlatform.insert(stRouteInfo.m_strPlatform);
        m_setVs.insert(stRouteInfo.m_strVs);
        //m_setStaticType.insert(strStaticType);
    }
    else if (EN_STATIC_TYPE_DOC == strStaticType)
    {
        /*
        m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][0] = NumToString(uddwDeMd5);
        if (-1 != dwReload)
        {
            m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strPlatform][stRouteInfo.m_strVs][stRouteInfo.m_strLang][1] = dwReload;
        }
        */
        m_setPlatform.insert(stRouteInfo.m_strPlatform);
        m_setVs.insert(stRouteInfo.m_strVs);
        m_setLang.insert(stRouteInfo.m_strLang);
        //m_setStaticType.insert(strStaticType);
    }
    else if (EN_STATIC_TYPE_NEW_MAINTAIN == strStaticType)
    {
        /*
        m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strPlatform][0] = NumToString(uddwDeMd5);
        if (-1 != dwReload)
        {
            m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strPlatform][1] = dwReload;
        }
        */
        
        m_setPlatform.insert(stRouteInfo.m_strPlatform);
        //m_setStaticType.insert(strStaticType);
    }
    else if (EN_STATIC_TYPE_NOTICE == strStaticType
        || EN_STATIC_TYPE_ITEMSWITCH == strStaticType
        || EN_STATIC_TYPE_ALSTORE == strStaticType
        || EN_STATIC_TYPE_SVRCONF == strStaticType
        || EN_STATIC_TYPE_ALLLAKE == strStaticType
        || EN_STATIC_TYPE_USER_LINK == strStaticType)
    {
        /*
        m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strSid][0] = NumToString(uddwDeMd5);
        m_jStaticDataJson["json"][strStaticType][stRouteInfo.m_strSid][1] = dwReload;
        */
        m_setSid.insert(stRouteInfo.m_strSid);
        //m_setStaticType.insert(strStaticType);
    }
    else
    {   /*
        m_jStaticDataJson["json"][strStaticType][0] = NumToString(uddwDeMd5);
        m_jStaticDataJson["json"][strStaticType][1] = dwReload;
        */
        //m_setStaticType.insert(strStaticType);
    }

    return 0;
}

TINT32 CStaticDataMd5ListMgr::GenStaticDataMd5List()
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

    if (0 == m_setPlatform.size()
        || 0 == m_setVs.size()
        || 0 == m_setSid.size()
        || 0 == m_setLang.size()
        || 0 == m_setStaticType.size())
    {
        return -1;
    }           
    
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    Json::Value jMd5Json;
    jMd5Json.clear();

    Json::FastWriter jsonWriter;
    jsonWriter.omitEndingLineFeed();

    set<string>::iterator itPlatform;
    set<string>::iterator itVs;
    set<string>::iterator itLang;
    set<string>::iterator itSid;
    set<string>::iterator itStaticType;


    SRouteInfo stRouteInfo;
    stRouteInfo.Reset();
    SStaticFileInfo stStaticFileInfo;
    stStaticFileInfo.Reset();


    for (itPlatform = m_setPlatform.begin(); itPlatform != m_setPlatform.end(); ++itPlatform)
    {
        for (itVs = m_setVs.begin(); itVs != m_setVs.end();++itVs)
        {
            for (itLang = m_setLang.begin(); itLang != m_setLang.end(); ++itLang)
            {
                for (itSid = m_setSid.begin(); itSid != m_setSid.end(); ++itSid)
                {
                    for (itStaticType = m_setStaticType.begin(); itStaticType != m_setStaticType.end(); ++itStaticType)
                    {

                        stRouteInfo.m_strPlatform = *itPlatform;
                        stRouteInfo.m_strVs = *itVs;
                        stRouteInfo.m_strLang = *itLang;
                        stRouteInfo.m_strSid = *itSid;
                        if (EN_STATIC_TYPE_ITEMSWITCH == *itStaticType)
                        {
                            stRouteInfo.m_strExInfo = EN_STATIC_TYPE_ITEMSWITCH;
                        }
                        if (EN_STATIC_TYPE_ALSTORE == *itStaticType)
                        {
                            stRouteInfo.m_strExInfo = EN_STATIC_TYPE_ALSTORE;
                        }
                        CStaticFileMgr::GetInstance()->GetStaticFileInfo(*itStaticType, stRouteInfo, &stStaticFileInfo);
                       

                        if (EN_STATIC_TYPE_GAME == *itStaticType
                            || EN_STATIC_TYPE_DOC == *itStaticType
                            || EN_STATIC_TYPE_EQUIP == *itStaticType
                            || EN_STATIC_TYPE_ALLLAKE == *itStaticType)
                        {
                            jMd5Json[*itPlatform][*itVs][*itLang][*itSid][*itStaticType][0] = stStaticFileInfo.m_strMd5;
                        }
                        else
                        {
                            jMd5Json[*itPlatform][*itVs][*itLang][*itSid][*itStaticType][0] = NumToString(stStaticFileInfo.m_uddwDeMD5);
                        }
                        
                        jMd5Json[*itPlatform][*itVs][*itLang][*itSid][*itStaticType][1] = stStaticFileInfo.m_udwReload;

                    }

                    strDeString = jsonWriter.write(jMd5Json[*itPlatform][*itVs][*itLang][*itSid]);

                    uddwDeMd5 = GetStringMD5(strDeString);
                    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
                    uddwEnMd5 = GetStringMD5(strEnString);


                    string strDeDataString = "";
                    string strEnDataString = "";
                    TUINT64 uddwDeDataMd5 = 0;
                    TUINT64 uddwEnDataMd5 = 0;


                    strDeDataString = "[{\"key\":\"[op_md5:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
                    uddwDeDataMd5 = GetStringMD5(strDeDataString);
                    strEnDataString = "[{\"key\":\"[op_md5:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
                    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
                    uddwEnDataMd5 = GetStringMD5(strEnString);

                    Json::Value jTmpJson;
                    Json::Reader oJsonReader;
                    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
                    {
                        jTmpJson = Json::Value(Json::arrayValue);
                    }
                    m_jStaticDataJson[*itPlatform][*itVs][*itLang][*itSid]["json"] = jTmpJson;
                    m_jStaticDataJson[*itPlatform][*itVs][*itLang][*itSid]["de"]["content"] = strDeDataString;
                    m_jStaticDataJson[*itPlatform][*itVs][*itLang][*itSid]["de"]["md5"] = uddwDeDataMd5;
                    m_jStaticDataJson[*itPlatform][*itVs][*itLang][*itSid]["en"]["content"] = strEnString;
                    m_jStaticDataJson[*itPlatform][*itVs][*itLang][*itSid]["en"]["md5"] = uddwEnDataMd5;

                }
            }
        }
    }


    return 0;
}




TINT32 CStaticDataUserLink::InitStaticData(CTseLogger *poServLog)
{
    m_poServLog = poServLog;
    string strDeString = "\"null\"";
    TUINT64 uddwDeMd5 = 0;

    string strEnString = "";
    TUINT64 uddwEnMd5 = 0;

    TUINT32 udwEncryptDataLen = 0;

    uddwDeMd5 = GetStringMD5(strDeString);
    EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
    uddwEnMd5 = GetStringMD5(strEnString);


    string strDeDataString = "";
    string strEnDataString = "";
    TUINT64 uddwDeDataMd5 = 0;
    TUINT64 uddwEnDataMd5 = 0;


    strDeDataString = "[{\"key\":\"[op_user_link:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
    uddwDeDataMd5 = GetStringMD5(strDeDataString);
    strEnDataString = "[{\"key\":\"[op_user_link:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
    EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
    uddwEnDataMd5 = GetStringMD5(strEnString);

    Json::Value jTmpJson;
    Json::Reader oJsonReader;
    if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
    {
        jTmpJson = Json::Value(Json::arrayValue);
    }


    m_jStaticDataJson["default"]["json"] = jTmpJson;
    m_jStaticDataJson["default"]["de"]["content"] = strDeDataString;
    m_jStaticDataJson["default"]["de"]["md5"] = uddwDeDataMd5;
    m_jStaticDataJson["default"]["en"]["content"] = strEnString;
    m_jStaticDataJson["default"]["en"]["md5"] = uddwEnDataMd5;
    m_jStaticDataJson["default"]["reload"] = 0;


    return 0;
}


TINT32 CStaticDataUserLink::GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo)
{
    GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_READ);
    if (m_jStaticDataJson[stRouteInfo.m_strSid].empty())
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson["default"]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson["default"]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson["default"]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson["default"]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson["default"]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson["default"]["reload"].asUInt();
        pstStaticFileInfo->m_strMd5 = m_jStaticDataJson["default"]["md5"].asString();

    }
    else
    {
        pstStaticFileInfo->m_jDataJson = m_jStaticDataJson[stRouteInfo.m_strSid]["json"];
        pstStaticFileInfo->m_strDeStaticFile = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["content"].asString();
        pstStaticFileInfo->m_uddwDeMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["de"]["md5"].asUInt64();
        pstStaticFileInfo->m_strEnStaticFile = m_jStaticDataJson[stRouteInfo.m_strSid]["en"]["content"].asString();
        pstStaticFileInfo->m_uddwEnMD5 = m_jStaticDataJson[stRouteInfo.m_strSid]["en"]["md5"].asUInt64();
        pstStaticFileInfo->m_udwReload = m_jStaticDataJson[stRouteInfo.m_strSid]["reload"].asUInt64();

    }
    return 0;
}

TINT32 CStaticDataUserLink::UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate)
{
    if (EN_UPDATE_TYPE_ADD == dwOperate
        || EN_UPDATE_TYPE_UPDATE == dwOperate)
    {
        GuardMutexRW Guard(m_rw_lock_StaticData, GUARD_WRITE);

        string strFileName = "";
        strFileName = GetStaticDataName(stRouteInfo);
        if ("" == strFileName)
        {
            return -1;
        }

        std::ifstream is;
        Json::Reader reader;
        Json::Value jUserLinkJson;
        jUserLinkJson.clear();
        is.open(stRouteInfo.m_strStaticFilePath.c_str());
        if (false == reader.parse(is, jUserLinkJson))
        {
            is.close();
            return -2;
        }
        else
        {
            is.close();
        }

        string strDeString = "\"null\"";
        TUINT64 uddwDeMd5 = 0;
        string strEnString = "";
        TUINT64 uddwEnMd5 = 0;
        TUINT32 udwEncryptDataLen = 0;
        Json::FastWriter jsonWriter;
        jsonWriter.omitEndingLineFeed();


        Json::Value::Members jMemberSid = jUserLinkJson.getMemberNames();
        for (TUINT32 udwIdx = 0; udwIdx < jMemberSid.size(); ++udwIdx)
        {

            strDeString = jsonWriter.write(jUserLinkJson[jMemberSid[udwIdx]]);

            uddwDeMd5 = GetStringMD5(strDeString);
            EncryptString(strDeString, strDeString.length(), strEnString, udwEncryptDataLen);
            uddwEnMd5 = GetStringMD5(strEnString);

            string strDeDataString = "";
            string strEnDataString = "";
            TUINT64 uddwDeDataMd5 = 0;
            TUINT64 uddwEnDataMd5 = 0;


            strDeDataString = "[{\"key\":\"[op_user_link:" + NumToString(uddwDeMd5) + ":0:0:" + NumToString(strDeString.length()) + "]\",\"data\":" + strDeString + "}]";
            uddwDeDataMd5 = GetStringMD5(strDeDataString);
            strEnDataString = "[{\"key\":\"[op_user_link:" + NumToString(uddwEnMd5) + ":0:0:" + NumToString(strEnString.length()) + "]\",\"data\":\"" + strEnString + "\"}]";
            EncryptString(strEnDataString, strEnDataString.length(), strEnString, udwEncryptDataLen);
            uddwEnDataMd5 = GetStringMD5(strEnString);

            Json::Value jTmpJson;
            Json::Reader oJsonReader;
            if (FALSE == oJsonReader.parse(strDeDataString, jTmpJson))
            {
                jTmpJson = Json::Value(Json::arrayValue);
            }

            m_jStaticDataJson[jMemberSid[udwIdx]]["json"] = jTmpJson;
            m_jStaticDataJson[jMemberSid[udwIdx]]["de"]["content"] = strDeDataString;
            m_jStaticDataJson[jMemberSid[udwIdx]]["de"]["md5"] = uddwDeDataMd5;
            m_jStaticDataJson[jMemberSid[udwIdx]]["en"]["content"] = strEnString;
            m_jStaticDataJson[jMemberSid[udwIdx]]["en"]["md5"] = uddwEnDataMd5;
            m_jStaticDataJson[jMemberSid[udwIdx]]["reload"] = stRouteInfo.m_udwReload;


            stRouteInfo.m_strSid = jMemberSid[udwIdx];
            CStaticDataMd5ListMgr::GetInstance()->UpdateMd5Data(stRouteInfo, EN_STATIC_TYPE_USER_LINK, uddwDeDataMd5, stRouteInfo.m_udwReload);

        }
        if (EN_UPDATE_TYPE_UPDATE == dwOperate)
        {
            CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
        }

    }
    else if (EN_UPDATE_TYPE_DEL == dwOperate)
    {
        // todo
    }
    else if (EN_UPDATE_TYPE_COMPUTE_UPDATE == dwOperate)
    {
        // todo
    }
    else
    {
        // do nothing
    }

    return 0;
}

string CStaticDataUserLink::GetStaticDataName(const SRouteInfo &stRouteInfo)
{
    string strStaticDataName = "";
    strStaticDataName = USER_LINK_FILE_NAME;
    return strStaticDataName;
}
