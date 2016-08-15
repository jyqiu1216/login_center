#ifndef _AWS_RSP_INFO_H_
#define _AWS_RSP_INFO_H_
#include <string>
using namespace std;

struct AwsRspInfo
{
    int dwRetCode;
    unsigned int udwIdxNo; //打开方式,用于区分主动和被动action
    unsigned int udwServiceType;
    unsigned int udwCostTime;
    int udwTableType;
    string sAmzReqId;
    string sRspContent;
    string sTableName;
    string sOperatorName;
    AwsRspInfo()
    {
        dwRetCode = 0;
        sAmzReqId = "";
        sRspContent = "";
        udwTableType = 0;
    }

    void Reset()
    {
        dwRetCode = 0;
        sAmzReqId = "";
        sRspContent = "";
        udwTableType = 0;
    }
};

#endif      