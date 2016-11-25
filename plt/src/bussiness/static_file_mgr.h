#ifndef _NEW_STATIC_FILE_MGR_H_
#define _NEW_STATIC_FILE_MGR_H_


#include "my_include.h"
#include "base/os/wtselock.h"
#include <map>
#include <vector>

using namespace std;
using namespace wtse::os;

#define DATA_LIST_JSON ("../data/data_list.json")
#define DATA_LIST_FLAG ("../data/data_list_flag")


enum EMaintainStatus
{
    EN_MAINTAIN_TYPE_NORMAL = 0,
    EN_MAINTAIN_TYPE_TO_MAINTAIN = 1,   
    EN_MAINTAIN_TYPE_MAINTAINING = 2,
    EN_MAINTAIN_TYPE_VERSION_FORCE_UPDATE = 3,
    EN_MAINTAIN_TYPE_VERSION_ADVICE_UPDATE = 4,
};

enum EPlatForm
{
    EN_PLATFORM__COMMON = 0,
    EN_PLATFORM__IOS,
    EN_PLATFORM__ANDROID,
};

enum EnJsonDiff
{
    EN_JSON_DIFF__EQUAL = 0,
    EN_JSON_DIFF__NEW, 
    EN_JSON_DIFF__ADD,
};

// 静态文件类型
const string EN_STATIC_TYPE_GAME = "game";
const string EN_STATIC_TYPE_DOC = "doc";
const string EN_STATIC_TYPE_GUIDE = "guide";
const string EN_STATIC_TYPE_EQUIP = "equip";
const string EN_STATIC_TYPE_META = "meta";
const string EN_STATIC_TYPE_NEW_MAINTAIN = "new_maintain";
const string EN_STATIC_TYPE_NOTICE = "notice";
const string EN_STATIC_TYPE_ITEMSWITCH = "item_switch";
const string EN_STATIC_TYPE_ALSTORE = "al_store";
const string EN_STATIC_TYPE_ONSALE = "onsale";
const string EN_STATIC_TYPE_SVRCONF = "svr_conf";
const string EN_STATIC_TYPE_ALLLAKE = "all_lake";
const string EN_STATIC_TYPE_CLIENTCONF = "client_conf";
const string EN_STATIC_TYPE_GLOBALCONF = "global_conf";
const string EN_STATIC_TYPE_MD5 = "md5";
const string EN_STATIC_TYPE_ACCOUNT_STATUS = "account_status";
const string EN_STATIC_TYPE_USER_LINK = "user_link";
const string EN_STATIC_TYPE_FUNCTION_SWITCH = "function_switch";
const string EN_STATIC_TYPE_META_RAW = "metaraw";



// 所以静态文件
// 1. 能直接读的静态文件
const string CLIENT_FILE_NAME = "client.json";
const string GLOBAL_CONF_FILE_NAME = "global_conf.json";
const string LAKE_FILE_NAME = "server_0_lake.json";
const string IOS_DOC_GERMAN_FILE_NAME = "document_german.json.de";
const string IOS_DOC_ENGLISH_FILE_NAME = "document_english.json.de";
const string IOS_DOC_FRENCH_FILE_NAME = "document_french.json.de";
const string IOS_DOC_RUSSIAN_FILE_NAME = "document_russian.json.de";
const string IOS_DOC_SPAIN_FILE_NAME = "document_spain.json.de";
const string IOS_DOC_CHINESE_FILE_NAME = "document_chinese.json.de";
const string IOS_GAME_FILE_NAME = "game.json.de";
const string IOS_GUIDE_FILE_NAME = "new_guide.json";
const string IOS_EQUIP_FILE_NAME = "equip.json.de";
const string ANDROID_DOC_GERMAN_FILE_NAME = "document_german_android.json.de";
const string ANDROID_DOC_ENGLISH_FILE_NAME = "document_english_android.json.de";
const string ANDROID_DOC_FRENCH_FILE_NAME = "document_french_android.json.de";
const string ANDROID_DOC_RUSSIAN_FILE_NAME = "document_russian_android.json.de";
const string ANDROID_DOC_SPAIN_FILE_NAME = "document_spain_android.json.de";
const string ANDROID_DOC_CHINESE_FILE_NAME = "document_chinese_android.json.de";
const string ANDROID_GAME_FILE_NAME = "game_android.json.de";
const string ANDROID_GUIDE_FILE_NAME = "new_guide_android.json";
const string ANDROID_EQUIP_FILE_NAME = "equip_android.json.de";
// 2. 需要细化的静态文件
const string IOS_MAINTAIN_FILE_NAME = "maintain.json";
const string ANDROID_MAINTAIN_FILE_NAME = "maintain_android.json";
const string IOS_META_FILE_NAME = "meta2.json";
const string ANDROID_META_FILE_NAME = "meta2_android.json";
const string SVR_FILE_NAME = "svr.json";
const string FUNCTIOTN_FILE_NAME = "function_switch.json";
const string ONSALE_FILE_NAME = "onsale.json";
const string NOTICE_FILE_NAME = "notice.json";
const string USER_LINK_FILE_NAME = "user_link.json";


// 静态数据的路由元素
struct SRouteInfo
{
    string m_strPlatform;
    string m_strVs;
    string m_strUpdateVs;
    string m_strSid;
    string m_strDevice;
    TUINT32 m_udwCurTime;
    TINT64  m_ddwUid;

    SRouteInfo()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_strPlatform = "";
        m_strVs = "";
        m_strUpdateVs = "";
        m_strSid = "";
        m_strDevice = "";
        m_udwCurTime = 0;
        m_ddwUid = 0;
    }
};

struct SStaticFileProperty
{
    TBOOL m_bDiffPlatForm;
    TBOOL m_bDiffVersion;
    TBOOL m_bNeedLoadData;
    TBOOL m_bNeedMd5;
};

class CStaticFileContent
{
public:
    string m_strFileName;
    TUINT8 m_ucReloadFlag;
    string m_strMd5;
    string m_strMd5Raw;

    string m_strStaticDataType;
    string m_strVersion;
    string m_strPlatForm;
    string m_strStaticFilePath;

    TUINT32 m_udwFileTs;
    TUINT32 m_udwFileSize;

    string m_strRealName;

    Json::Value m_jsonContent;
    
public:
    CStaticFileContent()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_strFileName.clear();
        m_ucReloadFlag = 0;
        m_strMd5.clear();

        m_strStaticDataType.clear();
        m_strVersion.clear();
        m_strPlatForm.clear();
        m_strStaticFilePath.clear();

        m_udwFileTs = 0;
        m_udwFileSize = 0;

        m_strRealName.clear();

        m_jsonContent.clear();
    }
};

class CStaticFileMgr
{
private:
    CStaticFileMgr();
    CStaticFileMgr(const CStaticFileMgr &);
    CStaticFileMgr & operator =(const CStaticFileMgr &stStaticFileMgr);
    static CStaticFileMgr *m_poStaticFileMgr;

public:
    ~CStaticFileMgr();

    // function  ===> 实例化 
    static CStaticFileMgr *GetInstance();

    TINT32 CheckUpdate();

public:
    TINT32 Init(CTseLogger *poLog);
    TINT32 UnInit();

    TINT32 LoadStaticFileList();
    TINT32 LoadStaticFile(CStaticFileContent *pobjStaticFile, string strPlatForm, string strVersion);

    CStaticFileContent* GetStaticFile(string strFileType, string strPlatForm, string strVersion);

private:
    TINT32 InitStaticFileProperty();
    TINT32 AddStaticFileProperty(string strFileType, TBOOL bDiffPlatForm, TBOOL bDiffVersion, TBOOL bNeedLoadData = true, TBOOL bNeedMd5 = true);
    TBOOL IsFileTypeExist(string strFileType);
    TBOOL IsFIleTypeNeedLoadData(string strFileType);
    TBOOL IsFileTypeNeedMd5(string strFileType);

    TINT32 IsWhiteAccount(string strDevice);

    string GetFileRealName(string strFileType, string strPlatForm, string strVersion);

    TINT32 LoadNewDataListJson(Json::Value &jNewDataListJson);

public://数据预处理
    TINT32 PreprocessStaticFile(Json::Value &jContent, string strType);

    TINT32 PreprocessStaticFile_Maintain(Json::Value &jContent);
    TINT32 PreprocessStaticFile_Md5(Json::Value &jContent);

public:
    TINT32 UpdtAndGetMaintainStatus(Json::Value &jContent, string strPlatForm, string strVersion, string strSid, TUINT32 udwCurTime, string strDevice);
    TINT32 UpdateMd5Json(Json::Value &jContent, string strKey, string strMd5, TINT32 dwReload);
    TINT32 UpdateMd5Json_Version(Json::Value &jContent, string strKey, string strMd5, TINT32 dwReload, string strPlatForm, string strVersion);

public:
    TINT32 GetStaticJson(Json::Value &jContent, string strType, SRouteInfo *pstInfo);

    TINT32 GetStaticJson_Meta(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_MetaOld(Json::Value &jContent, SRouteInfo *pstInfo);

    TINT32 GetStaticJson_Guide(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_Maintain(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_Notice(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_SvrConf(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_ItemSwitch(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_AlStore(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_ClientConf(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_GlobalConf(Json::Value &jContent, SRouteInfo *pstInfo);
    TINT32 GetStaticJson_UserLinker(Json::Value &jContent, SRouteInfo *pstInfo);

    TINT32 GetStaticJson_Account(Json::Value &jContent, TINT32 dwAccountStatus);
    TINT32 GetStaticJson_Md5(Json::Value &jContent, SRouteInfo *pstInfo);

public:
    static TUINT64 GetStringMD5(const string &strSourceData);
    static TUINT64 GetJsonMd5(Json::Value &jsonTmp);
    static TINT32 JsonDiff(Json::Value &jsonRaw, TUINT64 uddwRawMd5, Json::Value &jsonNew, TUINT64 uddwNewMd5, Json::Value &jsonDiff);
    static TUINT32 GetFileSize(const string &strFileName);
    
private:
    template<typename TNumber>
    static string NumToString(TNumber Input)
    {
        ostringstream oss;
        oss << Input;
        return oss.str();
    }

public:
    map<string, CStaticFileContent*> m_mapStaticFileContent;

private:
    CTseLogger *m_poServLog;
    map<string, SStaticFileProperty> m_mapStaticFileProperty;
    CMutex m_Mutex;

public:
    //md5 Json Value
    Json::Value m_jsonMd5;
    Json::Value m_jsonMd5_version;
};

#endif
