#include "aws_table_product.h"

TableDesc TbProduct::oTableDesc;

int TbProduct::Init(const string& sConfFile, const string strProjectName)
{
	CFieldProperty fld_prop;
	if (fld_prop.Init(sConfFile.c_str()) == false)
	{
		assert(0);
		return -1;
	}
	fld_prop.GetTableInfo(&oTableDesc);
	oTableDesc.m_strProjectName = strProjectName;
	AwsTable::AddNewObjectFunc(oTableDesc.sName, NewObject);
	return 0;
}

AwsTable* TbProduct::NewObject()
{
	return new TbProduct;
}

string TbProduct::GetTableName()
{
	ostringstream oss;
	if(!oTableDesc.m_strProjectName.empty())
	{
	//oss << oTableDesc.m_strProjectName << "_";
	}
	oss << "account_";
	oss << oTableDesc.sName;
	return oss.str();
}

TINT32 TbProduct::GetTableIdx()
{
	 return 0;
}

AwsMap* TbProduct::OnScanReq(unsigned int udwIdxNo, bool bHasStartKey, bool bReturnConsumedCapacity,
	 unsigned int dwLimit, unsigned int dwSegment, unsigned int dwTotalSegments)
{
	oJsonWriter.omitEndingLineFeed();
	assert(oTableDesc.mIndexDesc.find(udwIdxNo)!=oTableDesc.mIndexDesc.end());
	IndexDesc& idx_desc = oTableDesc.mIndexDesc[udwIdxNo];
	assert(idx_desc.sName == "PRIMARY"); //只能通过主键查询
	AwsMap* pScan = new AwsMap;
	assert(pScan);
	pScan->AddValue("/TableName", GetTableName());
	if (idx_desc.vecRtnFld.empty())
	{
		pScan->AddValue("/Select", "ALL_ATTRIBUTES");
	}
	else
	{
		ostringstream oss;
		for (unsigned int i = 0; i < idx_desc.vecRtnFld.size(); ++i)
		{
			oss.str("");
			oss << "/AttributesToGet[" << i << "]";
			FieldDesc& fld_desc = oTableDesc.mFieldDesc[idx_desc.vecRtnFld[i]];
			pScan->AddValue(oss.str(),  fld_desc.sName);
		}
	}
	if (dwLimit > 0)
	{
		pScan->AddNumber("/Limit", dwLimit);
	}
	if (bReturnConsumedCapacity)
	{
		pScan->AddValue("/ReturnConsumedCapacity", "TOTAL");
	}
	if (dwTotalSegments > 0)
	{
		pScan->AddNumber("/Segment", dwSegment);
		pScan->AddNumber("/TotalSegments", dwTotalSegments);
	}
	if (bHasStartKey)
	{
		pScan->AddValue("/ExclusiveStartKey/app_uid/S",m_sApp_uid);
		pScan->AddValue("/ExclusiveStartKey/r_pid/S",m_sR_pid);
	}
	return pScan;
}

int TbProduct::OnScanReq(string& sPostData, unsigned int udwIdxNo, bool bHasStartKey, bool bReturnConsumedCapacity,
 unsigned int dwLimit, unsigned int dwSegment, unsigned int dwTotalSegments)
{
	AwsMap* pScan = OnScanReq(udwIdxNo, bHasStartKey, bReturnConsumedCapacity, dwLimit, dwSegment, dwTotalSegments);
	ostringstream oss;
	pScan->Dump(oss);
	sPostData = oss.str();
	delete pScan;
	return 0;
}

int TbProduct::OnScanRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbProduct::OnQueryReq(unsigned int udwIdxNo, const CompareDesc& comp_desc,
	bool bConsistentRead, bool bReturnConsumedCapacity, bool bScanIndexForward, unsigned int dwLimit, bool bCount)
{
	oJsonWriter.omitEndingLineFeed();
	assert(oTableDesc.mIndexDesc.find(udwIdxNo)!=oTableDesc.mIndexDesc.end());
	IndexDesc& idx_desc = oTableDesc.mIndexDesc[udwIdxNo];
	AwsMap* pQuery = new AwsMap;
	assert(pQuery);
	pQuery->AddValue("/TableName", GetTableName());
	pQuery->AddBoolean("/ConsistentRead", bConsistentRead);
	if (!bScanIndexForward)
	{
		pQuery->AddBoolean("/ScanIndexForward", bScanIndexForward);
	}
	if (idx_desc.sName != "PRIMARY")
	{
		pQuery->AddValue("/IndexName", idx_desc.sName);
	}
	if (bCount)
	{
		pQuery->AddValue("/Select", "COUNT");
	}
	else if (idx_desc.vecRtnFld.empty())
	{
		pQuery->AddValue("/Select", "ALL_ATTRIBUTES");
	}
	else
	{
		ostringstream oss;
		for (unsigned int i = 0; i < idx_desc.vecRtnFld.size(); ++i)
		{
			oss.str("");
			oss << "/AttributesToGet[" << i << "]";
			FieldDesc& fld_desc = oTableDesc.mFieldDesc[idx_desc.vecRtnFld[i]];
			pQuery->AddValue(oss.str(),  fld_desc.sName);
		}
	}
	if (dwLimit > 0)
	{
		pQuery->AddNumber("/Limit", dwLimit);
	}
	if (bReturnConsumedCapacity)
	{
		pQuery->AddValue("/ReturnConsumedCapacity", "TOTAL");
	}
	for (unsigned int i = 0; i < idx_desc.vecIdxFld.size(); ++i)
	{
		FieldDesc& fld_desc = oTableDesc.mFieldDesc[idx_desc.vecIdxFld[i]];
		if (i == 0) //0只能是hash key，HASH KEY只能是EQ方式
		{
			if(fld_desc.udwFldNo == TbPRODUCT_FIELD_APP_UID)
			{
				pQuery->AddValue("/KeyConditions/app_uid/AttributeValueList[0]/S", m_sApp_uid);
				pQuery->AddValue("/KeyConditions/app_uid/ComparisonOperator", "EQ");
			}
			if(fld_desc.udwFldNo == TbPRODUCT_FIELD_R_PID)
			{
				pQuery->AddValue("/KeyConditions/r_pid/AttributeValueList[0]/S", m_sR_pid);
				pQuery->AddValue("/KeyConditions/r_pid/ComparisonOperator", "EQ");
			}
			if(fld_desc.udwFldNo == TbPRODUCT_FIELD_DEVICE)
			{
				pQuery->AddValue("/KeyConditions/device/AttributeValueList[0]/S", m_sDevice);
				pQuery->AddValue("/KeyConditions/device/ComparisonOperator", "EQ");
			}
			if(fld_desc.udwFldNo == TbPRODUCT_FIELD_RID)
			{
				pQuery->AddValue("/KeyConditions/rid/AttributeValueList[0]/S", m_sRid);
				pQuery->AddValue("/KeyConditions/rid/ComparisonOperator", "EQ");
			}
		}
		else if (i == 1) //1只能是rangekey or local index，可以有多种方式
		{
			if (COMPARE_TYPE_EQ<=comp_desc.dwCompareType && comp_desc.dwCompareType<=COMPARE_TYPE_GT)
			{
				if(fld_desc.udwFldNo == TbPRODUCT_FIELD_R_PID)
				{
					pQuery->AddValue("/KeyConditions/r_pid/AttributeValueList[0]/S", m_sR_pid);
					pQuery->AddValue("/KeyConditions/r_pid/ComparisonOperator", CompareType2Str(comp_desc.dwCompareType));
				}
			}
			else if (COMPARE_TYPE_BETWEEN == comp_desc.dwCompareType)
			{
				if(fld_desc.udwFldNo == TbPRODUCT_FIELD_R_PID)
				{
					pQuery->AddValue("/KeyConditions/r_pid/AttributeValueList[0]/S" , comp_desc.vecS[0]);
					pQuery->AddValue("/KeyConditions/r_pid/AttributeValueList[1]/S" , comp_desc.vecS[1]);
					pQuery->AddValue("/KeyConditions/r_pid/ComparisonOperator", "BETWEEN");
				}
			}
			else
			{
				assert(0);
			}
		}
		else
		{
			assert(0);
		}
	}
	return pQuery;
}

int TbProduct::OnQueryReq(string& sPostData, unsigned int udwIdxNo, const CompareDesc& comp_desc,
	bool bConsistentRead, bool bReturnConsumedCapacity, bool bScanIndexForward, unsigned int dwLimit)
{
	oJsonWriter.omitEndingLineFeed();
	AwsMap* pQuery = OnQueryReq(udwIdxNo, comp_desc, bConsistentRead, bReturnConsumedCapacity, bScanIndexForward, dwLimit);
	ostringstream oss;
	pQuery->Dump(oss);
	sPostData = oss.str();
	delete pQuery;
	return 0;
}

int TbProduct::OnQueryRsp(const Json::Value& item)
{
	return OnResponse(item);
}

int TbProduct::OnCountReq(string& sPostData, unsigned int udwIdxNo, const CompareDesc& comp_desc, 
		bool bConsistentRead, bool bReturnConsumedCapacity, bool bScanIndexForward, unsigned int dwLimit)
{
	AwsMap* pQuery = OnQueryReq(udwIdxNo, comp_desc, bConsistentRead, bReturnConsumedCapacity, bScanIndexForward, dwLimit, true);
	ostringstream oss;
	pQuery->Dump(oss);
	sPostData = oss.str();
	delete pQuery;
	return 0;
}

AwsMap* TbProduct::OnUpdateItemReq(
	const ExpectedDesc& expected_desc, int dwReturnValuesType, bool bReturnConsumedCapacity)
{
	oJsonWriter.omitEndingLineFeed();
	if (m_mFlag.size() == 0) //没有变化
	{
		return NULL;
	}
	AwsMap* pUpdateItem = new AwsMap;
	assert(pUpdateItem);
	string sBase64Encode;
	string sJsonEncode;
	bool bUpdateFlag = false;
	pUpdateItem->AddValue("/TableName", GetTableName());
	if (bReturnConsumedCapacity)
	{
		pUpdateItem->AddValue("/ReturnConsumedCapacity", "TOTAL");
	}
	if (dwReturnValuesType > RETURN_VALUES_NONE)
	{
		pUpdateItem->AddValue("/ReturnValues", ReturnValuesType2Str(dwReturnValuesType));
	}
	pUpdateItem->AddValue("/Key/app_uid/S", m_sApp_uid);
	pUpdateItem->AddValue("/Key/r_pid/S", m_sR_pid);
	for (map<unsigned int, int>::iterator iter = m_mFlag.begin(); iter != m_mFlag.end(); ++iter)
	{
		bUpdateFlag = true;
		if (TbPRODUCT_FIELD_SID == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/sid/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/sid/Value/N", m_nSid);
				pUpdateItem->AddValue("/AttributeUpdates/sid/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbPRODUCT_FIELD_DEVICE == iter->first)
		{
			if (!m_sDevice.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/device/Value/S", JsonEncode(m_sDevice, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/device/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/device/Action", "DELETE");
			}
			continue;
		}
		if (TbPRODUCT_FIELD_RID == iter->first)
		{
			if (!m_sRid.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/rid/Value/S", JsonEncode(m_sRid, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/rid/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/rid/Action", "DELETE");
			}
			continue;
		}
		if (TbPRODUCT_FIELD_PRODUCT_INFO == iter->first)
		{
			if (!m_sProduct_info.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/product_info/Value/S", JsonEncode(m_sProduct_info, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/product_info/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/product_info/Action", "DELETE");
			}
			continue;
		}
		if (TbPRODUCT_FIELD_BTIME == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/btime/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/btime/Value/N", m_nBtime);
				pUpdateItem->AddValue("/AttributeUpdates/btime/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbPRODUCT_FIELD_LGTIME == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/lgtime/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/lgtime/Value/N", m_nLgtime);
				pUpdateItem->AddValue("/AttributeUpdates/lgtime/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbPRODUCT_FIELD_STATUS == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/status/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/status/Value/N", m_nStatus);
				pUpdateItem->AddValue("/AttributeUpdates/status/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbPRODUCT_FIELD_SEQ == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/seq/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/seq/Value/N", m_nSeq);
				pUpdateItem->AddValue("/AttributeUpdates/seq/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		assert(0);
	}
	if(bUpdateFlag)
	{
		++m_nSeq;
		pUpdateItem->AddValue("/AttributeUpdates/seq/Value/N", m_nSeq);
		pUpdateItem->AddValue("/AttributeUpdates/seq/Action", "PUT");
	}
	ostringstream oss;
	for (unsigned int i = 0; i < expected_desc.vecExpectedItem.size(); ++i)
	{
		const ExpectedItem& item = expected_desc.vecExpectedItem[i];
		FieldDesc& fld_desc = oTableDesc.mFieldDesc[item.udwFldNo];
		oss.str("");
		oss << "/Expected/" << fld_desc.sName << "/Exists";
		pUpdateItem->AddBoolean(oss.str(), item.bExists);
		if (item.bExists) //Exists为true时才需要Value
		{
			oss.str("");
			oss << "/Expected/" << fld_desc.sName << "/Value/" << DataType2Str(fld_desc.dwType);
			if (FIELD_TYPE_N == fld_desc.dwType)
			{
				pUpdateItem->AddValue(oss.str(), item.nValue);
				continue;
			}
			if (FIELD_TYPE_S == fld_desc.dwType)
			{
				pUpdateItem->AddValue(oss.str(), item.sValue);
				continue;
			}
			//FIELD_TYPE_B型不能expected
			assert(0);
		}
	}
	return pUpdateItem;
}

int TbProduct::OnUpdateItemReq(string& sPostData,
	const ExpectedDesc& expected_desc, int dwReturnValuesType, bool bReturnConsumedCapacity)
{
	AwsMap* pUpdateItem = OnUpdateItemReq(expected_desc, dwReturnValuesType, bReturnConsumedCapacity);
	if (!pUpdateItem)
	{
		sPostData.clear();
		return 0;
	}
	ostringstream oss;
	pUpdateItem->Dump(oss);
	sPostData = oss.str();
	delete pUpdateItem;
	return 0;
}

int TbProduct::OnUpdateItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbProduct::OnWriteItemReq(int dwActionType)
{
	++m_nSeq;
	oJsonWriter.omitEndingLineFeed();
	AwsMap* pWriteItem = new AwsMap;
	assert(pWriteItem);
	string sBase64Encode;
	string sJsonEncode;
	AwsMap* pItem = NULL;
	if (WRITE_ACTION_TYPE_PUT == dwActionType)
	{
		pItem = pWriteItem->GetAwsMap("PutRequest")->GetAwsMap("Item");
		if (!m_sApp_uid.empty())
		{
			pItem->AddValue("/app_uid/S", JsonEncode(m_sApp_uid, sJsonEncode));
		}
		if (!m_sR_pid.empty())
		{
			pItem->AddValue("/r_pid/S", JsonEncode(m_sR_pid, sJsonEncode));
		}
		pItem->AddValue("/sid/N", m_nSid);
		if (!m_sDevice.empty())
		{
			pItem->AddValue("/device/S", JsonEncode(m_sDevice, sJsonEncode));
		}
		if (!m_sRid.empty())
		{
			pItem->AddValue("/rid/S", JsonEncode(m_sRid, sJsonEncode));
		}
		if (!m_sProduct_info.empty())
		{
			pItem->AddValue("/product_info/S", JsonEncode(m_sProduct_info, sJsonEncode));
		}
		pItem->AddValue("/btime/N", m_nBtime);
		pItem->AddValue("/lgtime/N", m_nLgtime);
		pItem->AddValue("/status/N", m_nStatus);
		pItem->AddValue("/seq/N", m_nSeq);
	}
	else if (WRITE_ACTION_TYPE_DELETE == dwActionType)
	{
		pItem = pWriteItem->GetAwsMap("DeleteRequest")->GetAwsMap("Key");
		pItem->AddValue("/app_uid/S", m_sApp_uid);
		pItem->AddValue("/r_pid/S", m_sR_pid);
	}
	else
	{
		assert(0);
	}
	return pWriteItem;
}

void TbProduct::OnWriteItemReq(AwsMap* pWriteItem, int dwActionType)
{
	++m_nSeq;
	assert(pWriteItem);
	string sBase64Encode;
	string sJsonEncode;
	AwsMap* pReqItem = new AwsMap;
	assert(pReqItem);
	AwsMap* pItem = NULL;
	if (WRITE_ACTION_TYPE_PUT == dwActionType)
	{
		pItem = pReqItem->GetAwsMap("PutRequest")->GetAwsMap("Item");
		if (!m_sApp_uid.empty())
		{
			pItem->AddValue("/app_uid/S", JsonEncode(m_sApp_uid, sJsonEncode));
		}
		if (!m_sR_pid.empty())
		{
			pItem->AddValue("/r_pid/S", JsonEncode(m_sR_pid, sJsonEncode));
		}
		pItem->AddValue("/sid/N", m_nSid);
		if (!m_sDevice.empty())
		{
			pItem->AddValue("/device/S", JsonEncode(m_sDevice, sJsonEncode));
		}
		if (!m_sRid.empty())
		{
			pItem->AddValue("/rid/S", JsonEncode(m_sRid, sJsonEncode));
		}
		if (!m_sProduct_info.empty())
		{
			pItem->AddValue("/product_info/S", JsonEncode(m_sProduct_info, sJsonEncode));
		}
		pItem->AddValue("/btime/N", m_nBtime);
		pItem->AddValue("/lgtime/N", m_nLgtime);
		pItem->AddValue("/status/N", m_nStatus);
		pItem->AddValue("/seq/N", m_nSeq);
	}
	else if (WRITE_ACTION_TYPE_DELETE == dwActionType)
	{
		pItem = pReqItem->GetAwsMap("DeleteRequest")->GetAwsMap("Key");
		pItem->AddValue("/app_uid/S", m_sApp_uid);
		pItem->AddValue("/r_pid/S", m_sR_pid);
	}
	else
	{
		assert(0);
	}
	pWriteItem->GetAwsMap("RequestItems")->GetAwsList(GetTableName())->SetValue(pReqItem, true);
}

AwsMap* TbProduct::OnReadItemReq(unsigned int udwIdxNo)
{
	oJsonWriter.omitEndingLineFeed();
	IndexDesc& idx_desc = oTableDesc.mIndexDesc[udwIdxNo];
	assert(idx_desc.sName == "PRIMARY"); //只能通过主键查询
	AwsMap* pReadItem = new AwsMap;
	assert(pReadItem);
	pReadItem->AddValue("/Keys[0]/app_uid/S", m_sApp_uid);
	pReadItem->AddValue("/Keys[0]/r_pid/S", m_sR_pid);
	ostringstream oss;
	for (unsigned int i = 0; i < idx_desc.vecRtnFld.size(); ++i)
	{
		oss.str("");
		oss << "/AttributesToGet[" << i << "]";
		FieldDesc& fld_desc = oTableDesc.mFieldDesc[idx_desc.vecRtnFld[i]];
		pReadItem->AddValue(oss.str(),  fld_desc.sName);
	}
	return pReadItem;
}

void TbProduct::OnReadItemReq(AwsMap* pReadItem)
{
	assert(pReadItem);
	AwsList* pKeys = pReadItem->GetAwsMap("RequestItems")->GetAwsMap(GetTableName())->GetAwsList("Keys");
	AwsMap* pKey = new AwsMap;
	assert(pKey);
	pKey->AddValue("/app_uid/S", m_sApp_uid);
	pKey->AddValue("/r_pid/S", m_sR_pid);
	pKeys->SetValue(pKey, true);
}

int TbProduct::OnReadItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbProduct::OnDeleteItemReq(
	const ExpectedDesc& expected_desc, int dwReturnValuesType, bool bReturnConsumedCapacity)
{
	oJsonWriter.omitEndingLineFeed();
	AwsMap* pDeleteItem = new AwsMap;
	assert(pDeleteItem);
	pDeleteItem->AddValue("/TableName", GetTableName());
	if (bReturnConsumedCapacity)
	{
		pDeleteItem->AddValue("/ReturnConsumedCapacity", "TOTAL");
	}
	if (dwReturnValuesType > RETURN_VALUES_NONE)
	{
		pDeleteItem->AddValue("/ReturnValues", ReturnValuesType2Str(dwReturnValuesType));
	}
	pDeleteItem->AddValue("/Key/app_uid/S", m_sApp_uid);
	pDeleteItem->AddValue("/Key/r_pid/S", m_sR_pid);
	ostringstream oss;
	for (unsigned int i = 0; i < expected_desc.vecExpectedItem.size(); ++i)
	{
		const ExpectedItem& item = expected_desc.vecExpectedItem[i];
		FieldDesc& fld_desc = oTableDesc.mFieldDesc[item.udwFldNo];
		oss.str("");
		oss << "/Expected/" << fld_desc.sName << "/Exists";
		pDeleteItem->AddBoolean(oss.str(), item.bExists);
		if (item.bExists) //Exists为true时才需要Value
		{
			oss.str("");
			oss << "/Expected/" << fld_desc.sName << "/Value/" << DataType2Str(fld_desc.dwType);
			if (FIELD_TYPE_N == fld_desc.dwType)
			{
				pDeleteItem->AddValue(oss.str(), item.nValue);
				continue;
			}
			if (FIELD_TYPE_S == fld_desc.dwType)
			{
				pDeleteItem->AddValue(oss.str(), item.sValue);
				continue;
			}
			//FIELD_TYPE_B型不能expected
			assert(0);
		}
	}
	return pDeleteItem;
}

int TbProduct::OnDeleteItemReq(string& sPostData,
	const ExpectedDesc& expected_desc, int dwReturnValuesType, bool bReturnConsumedCapacity)
{
	++m_nSeq;
	AwsMap* pDeleteItem = OnDeleteItemReq(expected_desc, dwReturnValuesType, bReturnConsumedCapacity);
	ostringstream oss;
	pDeleteItem->Dump(oss);
	sPostData = oss.str();
	delete pDeleteItem;
	return 0;
}

int TbProduct::OnDeleteItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbProduct::OnGetItemReq(unsigned int udwIdxNo,
	bool bConsistentRead, bool bReturnConsumedCapacity)
{
	oJsonWriter.omitEndingLineFeed();
	assert(oTableDesc.mIndexDesc.find(udwIdxNo)!=oTableDesc.mIndexDesc.end());
	IndexDesc& idx_desc = oTableDesc.mIndexDesc[udwIdxNo];
	assert(idx_desc.sName == "PRIMARY"); //只能通过主键查询
	AwsMap* pGetItem = new AwsMap;
	assert(pGetItem);
	pGetItem->AddValue("/TableName", GetTableName());
	pGetItem->AddValue("/Key/app_uid/S", m_sApp_uid);
	pGetItem->AddValue("/Key/r_pid/S", m_sR_pid);
	ostringstream oss;
	for (unsigned int i = 0; i < idx_desc.vecRtnFld.size(); ++i)
	{
		oss.str("");
		oss << "/AttributesToGet[" << i << "]";
		FieldDesc& fld_desc = oTableDesc.mFieldDesc[idx_desc.vecRtnFld[i]];
		pGetItem->AddValue(oss.str(),  fld_desc.sName);
	}
	pGetItem->AddBoolean("/ConsistentRead", bConsistentRead);
	if (bReturnConsumedCapacity)
	{
		pGetItem->AddValue("/ReturnConsumedCapacity", "TOTAL");
	}
	return pGetItem;
}

int TbProduct::OnGetItemReq(string& sPostData, unsigned int udwIdxNo,
	bool bConsistentRead, bool bReturnConsumedCapacity)
{
	AwsMap* pGetItem = OnGetItemReq(udwIdxNo, bConsistentRead, bReturnConsumedCapacity);
	ostringstream oss;
	pGetItem->Dump(oss);
	sPostData = oss.str();
	delete pGetItem;
	return 0;
}

int TbProduct::OnGetItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbProduct::OnPutItemReq(
	const ExpectedDesc& expected_desc, int dwReturnValuesType, bool bReturnConsumedCapacity)
{
	oJsonWriter.omitEndingLineFeed();
	AwsMap* pPutItem = new AwsMap;
	assert(pPutItem);
	string sBase64Encode;
	string sJsonEncode;
	pPutItem->AddValue("/TableName", GetTableName());
	if (bReturnConsumedCapacity)
	{
		pPutItem->AddValue("/ReturnConsumedCapacity", "TOTAL");
	}
	if (dwReturnValuesType > RETURN_VALUES_NONE)
	{
		pPutItem->AddValue("/ReturnValues", ReturnValuesType2Str(dwReturnValuesType));
	}
	if (!m_sApp_uid.empty())
	{
		pPutItem->AddValue("/Item/app_uid/S", JsonEncode(m_sApp_uid, sJsonEncode));
	}
	if (!m_sR_pid.empty())
	{
		pPutItem->AddValue("/Item/r_pid/S", JsonEncode(m_sR_pid, sJsonEncode));
	}
	pPutItem->AddValue("/Item/sid/N", m_nSid);
	if (!m_sDevice.empty())
	{
		pPutItem->AddValue("/Item/device/S", JsonEncode(m_sDevice, sJsonEncode));
	}
	if (!m_sRid.empty())
	{
		pPutItem->AddValue("/Item/rid/S", JsonEncode(m_sRid, sJsonEncode));
	}
	if (!m_sProduct_info.empty())
	{
		pPutItem->AddValue("/Item/product_info/S", JsonEncode(m_sProduct_info, sJsonEncode));
	}
	pPutItem->AddValue("/Item/btime/N", m_nBtime);
	pPutItem->AddValue("/Item/lgtime/N", m_nLgtime);
	pPutItem->AddValue("/Item/status/N", m_nStatus);
	pPutItem->AddValue("/Item/seq/N", m_nSeq);
	ostringstream oss;
	for (unsigned int i = 0; i < expected_desc.vecExpectedItem.size(); ++i)
	{
		const ExpectedItem& item = expected_desc.vecExpectedItem[i];
		assert(oTableDesc.mFieldDesc.find(item.udwFldNo)!=oTableDesc.mFieldDesc.end());
		FieldDesc& fld_desc = oTableDesc.mFieldDesc[item.udwFldNo];
		oss.str("");
		oss << "/Expected/" << fld_desc.sName << "/Exists";
		pPutItem->AddBoolean(oss.str(), item.bExists);
		if (item.bExists) //Exists为true时才需要Value
		{
			oss.str("");
			oss << "/Expected/" << fld_desc.sName << "/Value/" << DataType2Str(fld_desc.dwType);
			if (FIELD_TYPE_N == fld_desc.dwType)
			{
				pPutItem->AddValue(oss.str(), item.nValue);
				continue;
			}
			if (FIELD_TYPE_S == fld_desc.dwType)
			{
				pPutItem->AddValue(oss.str(), item.sValue);
				continue;
			}
			//FIELD_TYPE_B型不能expected
			assert(0);
		}
	}
	return pPutItem;
}

int TbProduct::OnPutItemReq(string& sPostData,
	const ExpectedDesc& expected_desc, int dwReturnValuesType, bool bReturnConsumedCapacity)
{
	++m_nSeq;
	AwsMap* pPutItem = OnPutItemReq(expected_desc, dwReturnValuesType, bReturnConsumedCapacity);
	ostringstream oss;
	pPutItem->Dump(oss);
	sPostData = oss.str();
	delete pPutItem;
	return 0;
}

int TbProduct::OnPutItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

int TbProduct::OnResponse(const Json::Value& item)
{
	oJsonWriter.omitEndingLineFeed();
	int dwResLen = 0;
	dwResLen = 0;
	this->Reset();
	Json::Value::Members vecMembers = item.getMemberNames();
	for (unsigned int i = 0; i < vecMembers.size(); ++i)
	{
 		if (vecMembers[i] == "app_uid")
		{
			m_sApp_uid = item["app_uid"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "r_pid")
		{
			m_sR_pid = item["r_pid"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "sid")
		{
			m_nSid = strtoll(item["sid"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "device")
		{
			m_sDevice = item["device"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "rid")
		{
			m_sRid = item["rid"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "product_info")
		{
			m_sProduct_info = item["product_info"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "btime")
		{
			m_nBtime = strtoll(item["btime"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "lgtime")
		{
			m_nLgtime = strtoll(item["lgtime"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "status")
		{
			m_nStatus = strtoll(item["status"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "seq")
		{
			m_nSeq = strtoll(item["seq"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
	}
	return 0;
}

TINT64 TbProduct::GetSeq()
{
	return m_nSeq;
}

