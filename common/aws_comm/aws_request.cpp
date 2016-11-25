#include "aws_request.h"
#include "aws_table_include.h"

int CAwsRequest::UpdateItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, const ExpectedDesc& expected_desc, int dwReturnValuesType)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "UpdateItem");
    pAwsTbl->OnUpdateItemReq(pAwsReq->sReqContent, expected_desc, dwReturnValuesType);
    if(pAwsReq->sReqContent.empty()) //不需要更新
    {
        delete pAwsReq;
        return 1;
    }
    vecReq.push_back(pAwsReq);
    return 0;
}

int CAwsRequest::DeleteItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, const ExpectedDesc& expected_desc, int dwReturnValuesType)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "DeleteItem");
    pAwsTbl->OnDeleteItemReq(pAwsReq->sReqContent, expected_desc, dwReturnValuesType);
    vecReq.push_back(pAwsReq);
    return 0;
}

int CAwsRequest::PutItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "PutItem");
    pAwsTbl->OnPutItemReq(pAwsReq->sReqContent);
    vecReq.push_back(pAwsReq);
    return 0;
}

int CAwsRequest::DeleteItem(AwsReqInfo* pReq, AwsTable* pAwsTbl, const ExpectedDesc& expected_desc, int dwReturnValuesType)
{
    pReq->SetVal(pAwsTbl, pAwsTbl->GetTableName(), "DeleteItem");
    pAwsTbl->OnDeleteItemReq(pReq->sReqContent, expected_desc, dwReturnValuesType);
    return 0;
}

int CAwsRequest::PutItem(AwsReqInfo* pReq, AwsTable* pAwsTbl)
{
    pReq->SetVal(pAwsTbl, pAwsTbl->GetTableName(), "PutItem");
    pAwsTbl->OnPutItemReq(pReq->sReqContent);
    return 0;
}

int CAwsRequest::GetItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo, bool bConsistentRead)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "GetItem", udwIdxNo);
    pAwsTbl->OnGetItemReq(pAwsReq->sReqContent, udwIdxNo, bConsistentRead);
    vecReq.push_back(pAwsReq);
    return 0;
}


int CAwsRequest::Query(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo, const CompareDesc& comp_desc, bool bConsistentRead,
    bool bReturnConsumedCapacity, bool bScanIndexForward, unsigned int dwLimit)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "Query", udwIdxNo);
    pAwsTbl->OnQueryReq(pAwsReq->sReqContent, udwIdxNo, comp_desc,
        bConsistentRead, bReturnConsumedCapacity, bScanIndexForward, dwLimit);
    vecReq.push_back(pAwsReq);
    return 0;
}


int CAwsRequest::QueryCount(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo,
    const CompareDesc& comp_desc, bool bConsistentRead, bool bReturnConsumedCapacity, bool bScanIndexForward, unsigned int dwLimit)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "Query", udwIdxNo);
    pAwsTbl->OnCountReq(pAwsReq->sReqContent, udwIdxNo, comp_desc, bConsistentRead, bReturnConsumedCapacity, bScanIndexForward, dwLimit);
    vecReq.push_back(pAwsReq);
    return 0;
}

int CAwsRequest::Scan(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwIdxNo,
    bool bHasStartKey, bool bReturnConsumedCapacity, unsigned int dwLimit, unsigned int dwSegment, unsigned int dwTotalSegments)
{
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "Scan", udwIdxNo);
    pAwsTbl->OnScanReq(pAwsReq->sReqContent, udwIdxNo,
        bHasStartKey, bReturnConsumedCapacity, dwLimit, dwSegment, dwTotalSegments);
    vecReq.push_back(pAwsReq);
    return 0;
}



int CAwsRequest::BatchGetItem(vector<AwsReqInfo*>& vecReq, AwsTable* pAwsTbl, unsigned int udwObjSize, unsigned int udwObjNum, unsigned int udwIdxNo)
{
    if(udwObjNum == 0)
    {
        return 0;
    }
    IndexDesc& idx_desc = pAwsTbl->pTableDesc->mIndexDesc[udwIdxNo];
    assert(idx_desc.sName == "PRIMARY"); //只能通过主键查询

    AwsMap BatchReadItem;
    for(unsigned int i = 0; i < udwObjNum; ++i)
    {
        pAwsTbl[i].OnReadItemReq(&BatchReadItem);
    }
    AwsMap *pReadItem = BatchReadItem.GetAwsMap("RequestItems")->GetAwsMap(pAwsTbl->GetTableName());
    ostringstream oss;
    for(unsigned int i = 0; i < idx_desc.vecRtnFld.size(); ++i)
    {
        oss.str("");
        oss << "/AttributesToGet[" << i << "]";
        FieldDesc& fld_desc = pAwsTbl->pTableDesc->mFieldDesc[idx_desc.vecRtnFld[i]];
        pReadItem->AddValue(oss.str(), fld_desc.sName);
    }
    pReadItem->AddBoolean("/ConsistentRead", true);
    BatchReadItem.AddValue("/ReturnConsumedCapacity", "TOTAL");
    oss.str("");
    BatchReadItem.Dump(oss);
    AwsReqInfo* pAwsReq = new AwsReqInfo(pAwsTbl, pAwsTbl->GetTableName(), "BatchGetItem", udwIdxNo, oss.str());
    vecReq.push_back(pAwsReq);
    return 0;
}