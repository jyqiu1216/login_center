#ifndef TSHP_HU_HU_WORK_H
#define TSHP_HU_HU_WORK_H

#include "my_include.h"
#include "key_value.h"
#include "http_utils.h"
#include "session.h"
#include "my_include.h"

class CHuWork
{
public:
    /*
     * 处理前端请求 1.获取请求url 2.获取并解析参数
     */
    static int GetRequestUrl(const char *req_buf, char *url_buf);
    static int GetRequestParam(SSession *session, CTseLogger *serv_log, RequestParam *req_param);

public:
    /*
    * 对特殊字符进行转移以符合json标准
    */
    static int JsonEncode(char *input_buf, char *output_buf, size_t buf_size);
    

private:
    // 获取http的key-value对
    static int GetHttpParam(const TCHAR *url_buf, RequestParam *req_param);
    // 将http的key-value对应到变量当中
    static int ProcessReqParam(RequestParam *pstReqParam, SSession *pstSession);

public:
    static TUINT64 GetUserMd5(TCHAR *pszStr);
    static TUINT64 GetStringMD5(TCHAR* pszStr);
    static TUINT64 GetStringMD5(const TCHAR* pszStr, TCHAR* pMD5Buffer);
    static TVOID ProcessString(TCHAR *pszStr);
};

#endif