#ifndef _JSON_RESULT_H_
#define _JSON_RESULT_H_

#include <string>
#include "my_include.h"
#include "session.h"
#include "static_file_mgr.h"
#include "jsoncpp_utils.h"
#include "client_netio.pb.h"
using namespace client_netio_protocol;


class CJsonResult
{
public:
    CJsonResult();
    ~CJsonResult();

    TUINT32 GetResultJsonLength() const;
    TCHAR* GenResult(SSession* pstSession);
    TCHAR* GenPushData_Pb(SSession *pstSession);

private:
    TINT32 GenResHeader(SSession* pstSession, TCHAR* pBeginPos, TCHAR* pEndPos);
    TINT32 GenResData(SSession* pstSession, TCHAR* pBeginPos, TCHAR* pEndPos);
    


    string GetDataTableName(const string &strDataKey);

public:
    template<typename TNumber>
    static string NumToString(TNumber Input)
    {
        ostringstream oss;
        oss << Input;
        return oss.str();
    }
   
    TCHAR* m_szResultBuffer;
 
    ClientResponse m_objPbResponse;
    CJsoncppSeri m_jSeri;
    Json::FastWriter m_jsonWriter;

    TINT32 m_dwResultLength;

private:
    // 原始数据和压缩数据
    TCHAR *m_pSourceBuffer;
    TCHAR *m_pCompressBuffer;
    TCHAR *m_pEncodeBuffer;
    CEncryptCR4 *poEncrypt;

    // 地图数据
    TCHAR *m_pszMapData;
};

#endif