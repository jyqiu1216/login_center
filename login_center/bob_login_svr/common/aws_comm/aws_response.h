#ifndef _AWS_RESPONSE_H_
#define _AWS_RESPONSE_H_

#include "aws_table_common.h"
#include "aws_consume_info.h"
#include "aws_rsp_info.h"

class CAwsResponse
{
public:
    static int OnScanRsp(const AwsRspInfo& rspInfo, AwsTable* pAwsTable, unsigned int udwObjSize, unsigned int udwObjMaxNum, 
        AwsTable& objLastEvaluatedKey, bool& bLastEvaluatedKey, int& dwScannedCount, AwsConsumeInfo& consumeInfo);
    static int OnScanRsp(const AwsRspInfo& rspInfo, AwsTable* pAwsTable, unsigned int udwObjSize, unsigned int udwObjMaxNum, 
        AwsTable& objLastEvaluatedKey, bool& bLastEvaluatedKey, int& dwScannedCount);

	static int OnQueryRsp(const AwsRspInfo& rspInfo, AwsTable* pAwsTable, unsigned int udwObjSize, unsigned int udwObjMaxNum, AwsConsumeInfo& consumeInfo);
    static int OnQueryRsp(const AwsRspInfo& rspInfo, AwsTable* pAwsTable, unsigned int udwObjSize, unsigned int udwObjMaxNum);

    static int OnQueryCountRsp(const AwsRspInfo& rspInfo, AwsConsumeInfo& consumeInfo);
    static int OnQueryCountRsp(const AwsRspInfo& rspInfo);

    static int OnBatchGetItemRsp(const AwsRspInfo& rspInfo, AwsTable* pAwsTable, unsigned int udwObjSize, unsigned int udwObjMaxNum);
	
    static int OnGetItemRsp(const AwsRspInfo& rspInfo, AwsTable *pAwsTable, AwsConsumeInfo& consumeInfo);
    static int OnGetItemRsp(const AwsRspInfo& rspInfo, AwsTable *pAwsTable);

	static int OnUpdateItemRsp(const AwsRspInfo& rspInfo, AwsTable *pAwsTable, AwsConsumeInfo& consumeInfo);
    static int OnUpdateItemRsp(const AwsRspInfo& rspInfo, AwsTable *pAwsTable);

public:
	static int GetConsumeInfo(const Json::Value& jsonRoot, AwsConsumeInfo& consumeInfo);
};

#endif


