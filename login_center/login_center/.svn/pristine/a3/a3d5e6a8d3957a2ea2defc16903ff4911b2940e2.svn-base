#ifndef _NEW_STATIC_FILE_MGR_H_
#define _NEW_STATIC_FILE_MGR_H_


#include "my_include.h"
#include "guard_mutex_rw.h"
#include <map>
#include <vector>

using namespace std;

#define DATA_LIST_JSON ("../data/data_list.json")
#define DATA_LIST_FLAG ("../data/data_list_flag")


enum EMaintainStatus
{
    EN_MAINTAIN_TYPE_NORMAL = 0,
    EN_MAINTAIN_TYPE_VERSION_FORCE_UPDATE = 1,
    EN_MAINTAIN_TYPE_GLOBAL_MAINTAIN = 2,
    EN_MAINTAIN_TYPE_SVR_MAINTAIN = 3,
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
    string m_strSid;
    string m_strLang;
    string m_strStaticFilePath;
    TUINT32 m_udwReload;
    string m_strExInfo;
    TUINT32 m_udwFileTs;

    SRouteInfo()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_strPlatform = "";
        m_strVs = "";
        m_strSid = "";
        m_strLang = "";
        m_strStaticFilePath = "";
        m_udwReload = 0;
        m_strExInfo = "";
        m_udwFileTs = 0;
    }
};

// 保存静态数据的最小信息
struct SStaticFileInfo
{
    TUINT64 m_uddwEnMD5;
    string m_strEnStaticFile;

    TUINT64 m_uddwDeMD5;
    string m_strDeStaticFile;

    string m_strMd5;

    TUINT32 m_udwReload;

    Json::Value m_jDataJson;
    Json::Value m_jDataInfoJson;


    SStaticFileInfo()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_uddwEnMD5 = 0;
        m_strEnStaticFile = "";

        m_uddwDeMD5 = 0;
        m_strDeStaticFile = "";

        m_strMd5 = "";

        m_udwReload = 0;

        m_jDataJson.clear();
        m_jDataInfoJson.clear();
    }
};


enum EStaticDataOperateType
{
    EN_UPDATE_TYPE_INIT,
    EN_UPDATE_TYPE_ADD,        
    EN_UPDATE_TYPE_DEL,
    EN_UPDATE_TYPE_UPDATE,
    EN_UPDATE_TYPE_COMPUTE_UPDATE,
};

// 静态文件的虚类
class CStaticDataBase
{
public:
    CStaticDataBase()
    {
        pthread_rwlock_init(&m_rw_lock_StaticData, NULL);
        m_jStaticDataJson.clear();
        m_jStaticDataUpdateJson.clear();
    }

    virtual ~CStaticDataBase()
    {
        pthread_rwlock_destroy(&m_rw_lock_StaticData);
    }

    virtual TINT32 InitStaticData(CTseLogger *poServLog) = 0;
    virtual TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo) = 0;
    virtual TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate) = 0;
    virtual string GetStaticDataName(const SRouteInfo &stRouteInfo) = 0;


protected:

    // function ==> 加密字符串
    TINT32 EncryptString(const string &strSourceData, const TUINT32 &udwSourceDataLen, string &strEncryptData, TUINT32 &udwEncryptDataLen);

    // function ==> 获取字符串的md5
    TUINT64 GetStringMD5(const string &strSourceData);
    // function ==> 对字符串进行预处理
    TVOID ProcessString(char *pszStr);
    // function ==> 计算字符串的md5
    inline TVOID MD5String(const TCHAR *string, TUCHAR *pstrReturn, TUINT32 uLength);

    // 获取原始数据的md5(二进制文件)
    TUINT64 GetRawDataMd5(const TUCHAR *pszData, const TUINT32 udwDataLen);
    inline TVOID MD5Segment(const TUCHAR *string, TUINT32 uStringLength, TUCHAR *pstrReturn, TUINT32 uReturnLength);

    template<typename TNumber>
    static string NumToString(TNumber Input)
    {
        ostringstream oss;
        oss << Input;
        return oss.str();
    }


protected:
    // 获取静态数据时对cache加读锁,更新静态数据加写锁
    pthread_rwlock_t m_rw_lock_StaticData;
    Json::Value m_jStaticDataJson;
    Json::Value m_jStaticDataUpdateJson;
    CTseLogger *m_poServLog;

};


// 静态文件：game
class CStaticDataGame : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

};

// 静态文件：doc
class CStaticDataDoc : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

    // 获取语种的id
    string GetDocId(const string &strDocName);

    // 获取语种的名字
    static string GetLangName(const string &strDocId);

};

// 静态文件：equip
class CStaticDataEquip : public CStaticDataBase
{
public:    
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);
};

// 静态文件：guide
class CStaticDataGuide : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);
};

// 静态文件：meta
class CStaticDataMeta : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);


};



// 静态文件：lake
class CStaticDataLake : public CStaticDataBase
{
private:
    struct SMapDataInfo
    {
        TCHAR *m_pszMapData;
        TUINT32 m_udwMapDataLen;

        TVOID Reset()
        {
            m_pszMapData = NULL;
            m_udwMapDataLen = 0;
        }

        SMapDataInfo()
        {
            Reset();
        }
    };


public:
    CStaticDataLake();
    ~CStaticDataLake();

    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

public:
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, TCHAR *pMapData, TUINT32 &udwMapDataLen);

    // 获取sid
    string GetLakeSid(const string &strLakeName);
    
    // 获取map数据文件的大小
    TINT32 GetMapFileSize(const string &strLakeName);

private:
    map<string, SMapDataInfo> mapSidMap;
};



// 静态文件：maintain
class CStaticDataMaintain : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

public:
    static TINT32 GetMaintainStatus(const SRouteInfo &stRouteInfo, const Json::Value &jMaintainPlatformJson);
};


// 静态文件：svr_conf
class CStaticDataSvrConf : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

    // 获取静态文件名字
    string GetStaticDataSvrConfName(const SRouteInfo &stRouteInfo);

    // 获取静态文件名字
    string GetStaticDataFunctionSwitchName(const SRouteInfo &stRouteInfo);
};



// 静态文件：client
class CStaticDataClient : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

};




// 静态文件：global_conf
class CStaticDataGlobalConf : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);
};




// 静态文件：onsale
class CStaticDataOnsale : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLogs);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);
};


// 静态文件：notice
class CStaticDataNotice : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);
};



// 静态文件：user_link
class CStaticDataUserLink : public CStaticDataBase
{
public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字
    string GetStaticDataName(const SRouteInfo &stRouteInfo);
};


// account_status管理器
class CStaticDataAccountStatusMgr : public CStaticDataBase
{
// 私有内嵌类，程序结束时的自动释放
private:
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (CStaticDataAccountStatusMgr::m_poStaticDataAccountStatusMgr)
            {
                delete CStaticDataAccountStatusMgr::m_poStaticDataAccountStatusMgr;
                CStaticDataAccountStatusMgr::m_poStaticDataAccountStatusMgr = NULL;
            }
        }
    };
    static CGarbo Garbo;                                //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数 


    // 单例服务的相关定义
private:
    CStaticDataAccountStatusMgr();
    CStaticDataAccountStatusMgr(const CStaticDataAccountStatusMgr &);
    CStaticDataAccountStatusMgr & operator =(const CStaticDataAccountStatusMgr &stStaticDataMd5ListMgr);
    static CStaticDataAccountStatusMgr *m_poStaticDataAccountStatusMgr;

public:
    ~CStaticDataAccountStatusMgr();

    // function  ===> 实例化 
    static CStaticDataAccountStatusMgr *GetInstance();

public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容(不实现)
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容(不实现)
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字(不实现)
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

public:
    TINT32 GetStaticData(const TINT32 &dwAccountStatus, SStaticFileInfo *pstStaticFileInfo);

private:
    set<string> m_setAccountStatus;
};




// 静态数据的md5_list管理器
class CStaticDataMd5ListMgr : public CStaticDataBase
{
// 私有内嵌类，程序结束时的自动释放
private:
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (CStaticDataMd5ListMgr::m_poStaticDataMd5ListMgr)
            {
                delete CStaticDataMd5ListMgr::m_poStaticDataMd5ListMgr;
                CStaticDataMd5ListMgr::m_poStaticDataMd5ListMgr = NULL;
            }
        }
    };
    static CGarbo Garbo;                                //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数 


    // 单例服务的相关定义
private:
    CStaticDataMd5ListMgr();
    CStaticDataMd5ListMgr(const CStaticDataMd5ListMgr &);
    CStaticDataMd5ListMgr & operator =(const CStaticDataMd5ListMgr &stStaticDataMd5ListMgr);
    static CStaticDataMd5ListMgr *m_poStaticDataMd5ListMgr;

public:
    ~CStaticDataMd5ListMgr();

    // function  ===> 实例化 
    static CStaticDataMd5ListMgr *GetInstance();

public:
    // 初始化静态数据
    TINT32 InitStaticData(CTseLogger *poServLog);

    // 获取该节点内容
    TINT32 GetStaticData(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // 更新该节点内容(不实现)
    TINT32 UpdateStaticData(const SRouteInfo &stRouteInfo, const TINT32 &dwOperate);

    // 获取静态文件名字(不实现)
    string GetStaticDataName(const SRouteInfo &stRouteInfo);

public:
    // 更新该节点内容
    TINT32 UpdateMd5Data(const SRouteInfo &stRouteInfo, const string &strStaticType, const TUINT64 &uddwDeMd5, const TINT32 &dwReload);
    
    // 建立md5的cache
    TINT32 GenStaticDataMd5List();


private:
    set<string> m_setPlatform;
    set<string> m_setVs;
    set<string> m_setLang;
    set<string> m_setSid;
    set<string> m_setStaticType;

};





struct SGlobalConfig
{
    string m_strStaticDataType;
    string m_strPlatForm;
    string m_strStaticFileName;
    string m_strStaticFilePath;
    TUINT32 m_udwTimeStamp;
    TUINT32 m_udwReload;
    TUINT32 m_udwUpdateType;

    SGlobalConfig()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_strStaticDataType = "";
        m_strPlatForm = "";
        m_strStaticFileName = "";
        m_strStaticFilePath = "";
        m_udwTimeStamp = 0;
        m_udwReload = 0;
        m_udwUpdateType = EN_UPDATE_TYPE_INIT;
    }
};

struct SVersionConfig
{
    string m_strStaticDataType;
    string m_strVersion;
    string m_strPlatForm;
    string m_strStaticFileName;
    string m_strStaticFilePath;
    TUINT32 m_udwTimeStamp;
    TUINT32 m_udwReload;
    TUINT32 m_udwUpdateType;

    SVersionConfig()
    {
        Reset();
    }

    TVOID Reset()
    {
        m_strStaticDataType = "";
        m_strVersion = "";
        m_strPlatForm = "";
        m_strStaticFileName = "";
        m_strStaticFilePath = "";
        m_udwTimeStamp = 0;
        m_udwReload = 0;
        m_udwUpdateType = EN_UPDATE_TYPE_INIT;
    }
};


struct SStaticFileJsonCache
{
    CStaticDataGame oStaticDataGame;
    CStaticDataDoc oStaticDataDoc;
    CStaticDataEquip oStaticDataEquip;
    CStaticDataGuide oStaticDataGuide;
    CStaticDataMeta oStaticDataMeta;
    CStaticDataLake oStaticDataLake;
    CStaticDataMaintain oStaticDataMaintain;
    CStaticDataSvrConf oStaticDataSvrConf;
    CStaticDataClient oStaticDataClient;
    CStaticDataGlobalConf oStaticDataGlobalConf;
    CStaticDataOnsale oStaticDataOnsale;
    CStaticDataNotice oStaticDataNotice;
    CStaticDataUserLink oStaticDataUserLink;

};


class CStaticFileMgr
{
    // 私有内嵌类，程序结束时的自动释放
private:
    class CGarbo
    {
    public:
        ~CGarbo()
        {
            if (CStaticFileMgr::m_poStaticFileMgr)
            {
                delete CStaticFileMgr::m_poStaticFileMgr;
                CStaticFileMgr::m_poStaticFileMgr = NULL;
            }
        }
    };
    static CGarbo Garbo;                                //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数 


    // 单例服务的相关定义
private:
    CStaticFileMgr();
    CStaticFileMgr(const CStaticFileMgr &);
    CStaticFileMgr & operator =(const CStaticFileMgr &stStaticFileMgr);
    static CStaticFileMgr *m_poStaticFileMgr;

public:
    ~CStaticFileMgr();

    // function  ===> 实例化 
    static CStaticFileMgr *GetInstance();



    // 获取服务的公共接口函数
public:

    // function ==> 初始化file管理器
    TINT32 Init(CTseLogger *poLog);

    // function ==> 检测是否有静态文件更新
    TINT32 CheckStaticFileUpdate();

    // function ==> 检测细化的静态数据是否需要计算更新
    TINT32 CheckStaticDataCompute();

    // function ==> 获取静态文件接口(除lake)
    TINT32 GetStaticFileInfo(const string &strSaticFileType, const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);

    // function ==> 获取Maintain的json(用来做登陆后的maintain校验)
    TINT32 GetMaintainJson(const SRouteInfo &stRouteInfo, SStaticFileInfo *pstStaticFileInfo);


    // function ==> 获取lake
    TINT32 GetMapData(const SRouteInfo &stRouteInfo, TCHAR *pMapData, TUINT32 &udwMapDataLen);
    


    // 获取静态文件名字
    string GetStaticFileMd5(const string strStaticFile, const SRouteInfo &stRouteInfo);

    // 内部成员函数
private:


    // 加载静态文件
    // function ==> 加载新的data_list.json
    TINT32 LoadNewDataListJson(Json::Value &newDataListJson);
    // function ==> 获取静态文件更新列表
    TINT32 GetUpdateStaticFileList(const Json::Value &jNewDataListJson, const Json::Value &jOldDataListJson, vector<SGlobalConfig> &vecGlobalList, vector<SVersionConfig> &vecVersionList);
    // function ==> 更新静态文件
    TINT32 UpdateStaticFile(const vector<SGlobalConfig> &vecGlobalList, const vector<SVersionConfig> &vecVersionList);
    // function ==> 更新data_list.json
    TINT32 UpdateDataList(const Json::Value &jNewDataListJson);
    

    // function ==> 获取data_list.json的map形式
    TINT32 GetDataListMap(const Json::Value &jDataListJson, map<string, SGlobalConfig> &mapGlobal, map<string, SVersionConfig> &mapVersion);
    // function ==> 更新global静态文件
    TINT32 GetUpdateGlobalStaticFileList(vector<SGlobalConfig> &vecGlobalList, map<string, SGlobalConfig> &mapOldGlobal, map<string, SGlobalConfig> &mapNewGlobal);
    // function ==> 更新version静态文件
    TINT32 GetUpdateVersionStaticFileList(vector<SVersionConfig> &vecVersionList, map<string, SVersionConfig> &mapOldVersion, map<string, SVersionConfig> &mapNewVersion);
  
  

    // 静态文件的格式
    string GenStaticFileResult(const string &strStaticString, const TINT32 &dwOpEncryptFlag, const vector<SStaticFileInfo> &vecStaticFileInfo);




    // 内部成员变量
private:
    
    CTseLogger *m_poServLog;

    // 当前的data_list.json
    Json::Value m_jDataListJson;

    SStaticFileJsonCache m_stStaticFileJsonCache;



};

#endif


/************************** m_jDataListJson的结构 ***************************
{
	"globalconfig": 
    {
		"common": 
        {
			"${file_name}": 
            {
                "path": "\/globalconfig\/common\/${file_name}",
                "timestamp": ${timestamp},
                "reload": ${reload}
            }
		},
		"ios": 
        {
            "${file_name}":
            {
                "path": "\/globalconfig\/ios\/${file_name}",
                "timestamp": ${timestamp},
                "reload": ${reload}
            }
		},
		"android": 
        {
            "${file_name}":
            {
                "path": "\/globalconfig\/android\/${file_name}",
                "timestamp": ${timestamp},
                "reload": ${reload}
            }	
		}
	},
	"versionconfig": 
    {
		"new": 
        {
			"ios": 
            {
                "${file_name}":
                {
                    "path": "\/versionconfig\/new\/ios\/${file_name}",
                    "timestamp": ${timestamp},
                    "reload": ${reload}
                }
			},
			"android": 
            {
                "${file_name}":
                {
                    "path": "\/versionconfig\/new\/android\/${file_name}",
                    "timestamp": ${timestamp},
                    "reload": ${reload}
                }
			}
		},
		"1.0": 
        {
			"ios":
            {
                "${file_name}":
                {
                    "path": "\/versionconfig\/1.0\/ios\/${file_name}",
                    "timestamp": ${timestamp},
                    "reload": ${reload}
                }
			},
			"android": 
            {
                "${file_name}":
                {
                    "path": "\/versionconfig\/1.0\/android\/${file_name}",
                    "timestamp": ${timestamp},
                    "reload": ${reload}
                }
			}
		}
	}
}
*/


/************************** m_stStaticFileJsonCache的结构 ***************************
{
	"globalconfig": 
    {
		"common": 
        {
			"${file_name}": 
            {
                "de": 
                {
                    "content": ${de_content},
                    "md5": ${de_md5}
                },
                "en":
                {
                    "content": ${en_content},
                    "md5": ${en_md5}
                }
            }
		},
		"ios": 
        {
			"${file_name}": 
            {
                "de": 
                {
                    "content": ${de_content},
                    "md5": ${de_md5}
                },
                "en":
                {
                    "content": ${en_content},
                    "md5": ${en_md5}
                }
            }
		},
		"android": 
        {
			"${file_name}": 
            {
                "de": 
                {
                    "content": ${de_content},
                    "md5": ${de_md5}
                },
                "en":
                {
                    "content": ${en_content},
                    "md5": ${en_md5}
                }
            }
		}
	},
	"versionconfig": 
    {
		"new": 
        {
			"ios": 
            {
			    "${file_name}": 
                {
                    "de": 
                    {
                        "content": ${de_content},
                        "md5": ${de_md5}
                    },
                    "en":
                    {
                        "content": ${en_content},
                        "md5": ${en_md5}
                    }
                }
			},
			"android": 
            {
			    "${file_name}": 
                {
                    "de": 
                    {
                        "content": ${de_content},
                        "md5": ${de_md5}
                    },
                    "en":
                    {
                        "content": ${en_content},
                        "md5": ${en_md5}
                    }
                }
			}
		},
		"1.0": 
        {
			"ios":
            {
			    "${file_name}": 
                {
                    "de": 
                    {
                        "content": ${de_content},
                        "md5": ${de_md5}
                    },
                    "en":
                    {
                        "content": ${en_content},
                        "md5": ${en_md5}
                    }
                }
			},
			"android": 
            {
   			    "${file_name}": 
                {
                    "de": 
                    {
                        "content": ${de_content},
                        "md5": ${de_md5}
                    },
                    "en":
                    {
                        "content": ${en_content},
                        "md5": ${en_md5}
                    }
                }
			}
		}
	}
}
*/