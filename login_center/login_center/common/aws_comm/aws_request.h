#ifndef _AWS_REQUEST_H_
#define _AWS_REQUEST_H_

#include "aws_req_info.h"
#include "aws_table_common.h"

class CAwsRequest
{
public:
public:
    static int UpdateItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE);

    static int DeleteItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE);

    static int PutItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl);

    static int DeleteItem(AwsReqInfo* pReq, AwsTable* pAwsTbl, const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE);

    static int PutItem(AwsReqInfo* pReq, AwsTable* pAwsTbl);

    static int GetItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo, bool bConsistentRead = true);

    static int Query(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo, const CompareDesc& comp_desc = CompareDesc(), bool bConsistentRead = true,
        bool bReturnConsumedCapacity = true, bool bScanIndexForward = true, unsigned int dwLimit = 0);

    static int QueryCount(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo, const CompareDesc& comp_desc = CompareDesc(), bool bConsistentRead = true,
        bool bReturnConsumedCapacity = true, bool bScanIndexForward = true, unsigned int dwLimit = 0);

    static int Scan(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo,
        bool bHasStartKey = false, bool bReturnConsumedCapacity = true, unsigned int dwLimit = 0, unsigned int dwSegment = 0, unsigned int dwTotalSegments = 0);

    static int BatchGetItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwObjSize, unsigned int udwObjNum, unsigned int udwIdxNo);
};

#endif


