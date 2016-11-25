#include "aws_table_user.h"

TableDesc TbUser::oTableDesc;

int TbUser::Init(const string& sConfFile, const string strProjectName)
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

AwsTable* TbUser::NewObject()
{
	return new TbUser;
}

string TbUser::GetTableName()
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

TINT32 TbUser::GetTableIdx()
{
	 return 0;
}

AwsMap* TbUser::OnScanReq(unsigned int udwIdxNo, bool bHasStartKey, bool bReturnConsumedCapacity,
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
		pScan->AddValue("/ExclusiveStartKey/rid/S",m_sRid);
	}
	return pScan;
}

int TbUser::OnScanReq(string& sPostData, unsigned int udwIdxNo, bool bHasStartKey, bool bReturnConsumedCapacity,
 unsigned int dwLimit, unsigned int dwSegment, unsigned int dwTotalSegments)
{
	AwsMap* pScan = OnScanReq(udwIdxNo, bHasStartKey, bReturnConsumedCapacity, dwLimit, dwSegment, dwTotalSegments);
	ostringstream oss;
	pScan->Dump(oss);
	sPostData = oss.str();
	delete pScan;
	return 0;
}

int TbUser::OnScanRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbUser::OnQueryReq(unsigned int udwIdxNo, const CompareDesc& comp_desc,
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
			if(fld_desc.udwFldNo == TbUSER_FIELD_RID)
			{
				pQuery->AddValue("/KeyConditions/rid/AttributeValueList[0]/S", m_sRid);
				pQuery->AddValue("/KeyConditions/rid/ComparisonOperator", "EQ");
			}
			if(fld_desc.udwFldNo == TbUSER_FIELD_EMAIL)
			{
				pQuery->AddValue("/KeyConditions/email/AttributeValueList[0]/S", m_sEmail);
				pQuery->AddValue("/KeyConditions/email/ComparisonOperator", "EQ");
			}
		}
		else if (i == 1) //1只能是rangekey or local index，可以有多种方式
		{
			if (COMPARE_TYPE_EQ<=comp_desc.dwCompareType && comp_desc.dwCompareType<=COMPARE_TYPE_GT)
			{
			}
			else if (COMPARE_TYPE_BETWEEN == comp_desc.dwCompareType)
			{
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

int TbUser::OnQueryReq(string& sPostData, unsigned int udwIdxNo, const CompareDesc& comp_desc,
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

int TbUser::OnQueryRsp(const Json::Value& item)
{
	return OnResponse(item);
}

int TbUser::OnCountReq(string& sPostData, unsigned int udwIdxNo, const CompareDesc& comp_desc, 
		bool bConsistentRead, bool bReturnConsumedCapacity, bool bScanIndexForward, unsigned int dwLimit)
{
	AwsMap* pQuery = OnQueryReq(udwIdxNo, comp_desc, bConsistentRead, bReturnConsumedCapacity, bScanIndexForward, dwLimit, true);
	ostringstream oss;
	pQuery->Dump(oss);
	sPostData = oss.str();
	delete pQuery;
	return 0;
}

AwsMap* TbUser::OnUpdateItemReq(
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
	pUpdateItem->AddValue("/Key/rid/S", m_sRid);
	for (map<unsigned int, int>::iterator iter = m_mFlag.begin(); iter != m_mFlag.end(); ++iter)
	{
		bUpdateFlag = true;
		if (TbUSER_FIELD_EMAIL == iter->first)
		{
			if (!m_sEmail.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/email/Value/S", JsonEncode(m_sEmail, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/email/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/email/Action", "DELETE");
			}
			continue;
		}
		if (TbUSER_FIELD_PASSWD == iter->first)
		{
			if (!m_sPasswd.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/passwd/Value/S", JsonEncode(m_sPasswd, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/passwd/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/passwd/Action", "DELETE");
			}
			continue;
		}
		if (TbUSER_FIELD_LOGIN_PLATFORM == iter->first)
		{
			if (!m_sLogin_platform.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/login_platform/Value/S", JsonEncode(m_sLogin_platform, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/login_platform/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/login_platform/Action", "DELETE");
			}
			continue;
		}
		if (TbUSER_FIELD_REGISTER_PLATFROM == iter->first)
		{
			if (!m_sRegister_platfrom.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/register_platfrom/Value/S", JsonEncode(m_sRegister_platfrom, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/register_platfrom/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/register_platfrom/Action", "DELETE");
			}
			continue;
		}
		if (TbUSER_FIELD_TYPE == iter->first)
		{
			if (!m_sType.empty())
			{
				pUpdateItem->AddValue("/AttributeUpdates/type/Value/S", JsonEncode(m_sType, sJsonEncode));
				pUpdateItem->AddValue("/AttributeUpdates/type/Action", "PUT");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/type/Action", "DELETE");
			}
			continue;
		}
		if (TbUSER_FIELD_CTIME == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/ctime/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/ctime/Value/N", m_nCtime);
				pUpdateItem->AddValue("/AttributeUpdates/ctime/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbUSER_FIELD_UTIME == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/utime/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/utime/Value/N", m_nUtime);
				pUpdateItem->AddValue("/AttributeUpdates/utime/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbUSER_FIELD_LOGSTATUS == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/logstatus/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/logstatus/Value/N", m_nLogstatus);
				pUpdateItem->AddValue("/AttributeUpdates/logstatus/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbUSER_FIELD_PWD_FLAG == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/pwd_flag/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/pwd_flag/Value/N", m_nPwd_flag);
				pUpdateItem->AddValue("/AttributeUpdates/pwd_flag/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbUSER_FIELD_PWD_SEQ == iter->first)
		{
			if(UPDATE_ACTION_TYPE_DELETE == iter->second)
			{
				pUpdateItem->AddValue("/AttributeUpdates/pwd_seq/Action", "DELETE");
			}
			else
			{
				pUpdateItem->AddValue("/AttributeUpdates/pwd_seq/Value/N", m_nPwd_seq);
				pUpdateItem->AddValue("/AttributeUpdates/pwd_seq/Action", UpdateActionType2Str(iter->second));
			}
			continue;
		}
		if (TbUSER_FIELD_STATUS == iter->first)
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
		if (TbUSER_FIELD_SEQ == iter->first)
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

int TbUser::OnUpdateItemReq(string& sPostData,
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

int TbUser::OnUpdateItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbUser::OnWriteItemReq(int dwActionType)
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
		if (!m_sRid.empty())
		{
			pItem->AddValue("/rid/S", JsonEncode(m_sRid, sJsonEncode));
		}
		if (!m_sEmail.empty())
		{
			pItem->AddValue("/email/S", JsonEncode(m_sEmail, sJsonEncode));
		}
		if (!m_sPasswd.empty())
		{
			pItem->AddValue("/passwd/S", JsonEncode(m_sPasswd, sJsonEncode));
		}
		if (!m_sLogin_platform.empty())
		{
			pItem->AddValue("/login_platform/S", JsonEncode(m_sLogin_platform, sJsonEncode));
		}
		if (!m_sRegister_platfrom.empty())
		{
			pItem->AddValue("/register_platfrom/S", JsonEncode(m_sRegister_platfrom, sJsonEncode));
		}
		if (!m_sType.empty())
		{
			pItem->AddValue("/type/S", JsonEncode(m_sType, sJsonEncode));
		}
		pItem->AddValue("/ctime/N", m_nCtime);
		pItem->AddValue("/utime/N", m_nUtime);
		pItem->AddValue("/logstatus/N", m_nLogstatus);
		pItem->AddValue("/pwd_flag/N", m_nPwd_flag);
		pItem->AddValue("/pwd_seq/N", m_nPwd_seq);
		pItem->AddValue("/status/N", m_nStatus);
		pItem->AddValue("/seq/N", m_nSeq);
	}
	else if (WRITE_ACTION_TYPE_DELETE == dwActionType)
	{
		pItem = pWriteItem->GetAwsMap("DeleteRequest")->GetAwsMap("Key");
		pItem->AddValue("/rid/S", m_sRid);
	}
	else
	{
		assert(0);
	}
	return pWriteItem;
}

void TbUser::OnWriteItemReq(AwsMap* pWriteItem, int dwActionType)
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
		if (!m_sRid.empty())
		{
			pItem->AddValue("/rid/S", JsonEncode(m_sRid, sJsonEncode));
		}
		if (!m_sEmail.empty())
		{
			pItem->AddValue("/email/S", JsonEncode(m_sEmail, sJsonEncode));
		}
		if (!m_sPasswd.empty())
		{
			pItem->AddValue("/passwd/S", JsonEncode(m_sPasswd, sJsonEncode));
		}
		if (!m_sLogin_platform.empty())
		{
			pItem->AddValue("/login_platform/S", JsonEncode(m_sLogin_platform, sJsonEncode));
		}
		if (!m_sRegister_platfrom.empty())
		{
			pItem->AddValue("/register_platfrom/S", JsonEncode(m_sRegister_platfrom, sJsonEncode));
		}
		if (!m_sType.empty())
		{
			pItem->AddValue("/type/S", JsonEncode(m_sType, sJsonEncode));
		}
		pItem->AddValue("/ctime/N", m_nCtime);
		pItem->AddValue("/utime/N", m_nUtime);
		pItem->AddValue("/logstatus/N", m_nLogstatus);
		pItem->AddValue("/pwd_flag/N", m_nPwd_flag);
		pItem->AddValue("/pwd_seq/N", m_nPwd_seq);
		pItem->AddValue("/status/N", m_nStatus);
		pItem->AddValue("/seq/N", m_nSeq);
	}
	else if (WRITE_ACTION_TYPE_DELETE == dwActionType)
	{
		pItem = pReqItem->GetAwsMap("DeleteRequest")->GetAwsMap("Key");
		pItem->AddValue("/rid/S", m_sRid);
	}
	else
	{
		assert(0);
	}
	pWriteItem->GetAwsMap("RequestItems")->GetAwsList(GetTableName())->SetValue(pReqItem, true);
}

AwsMap* TbUser::OnReadItemReq(unsigned int udwIdxNo)
{
	oJsonWriter.omitEndingLineFeed();
	IndexDesc& idx_desc = oTableDesc.mIndexDesc[udwIdxNo];
	assert(idx_desc.sName == "PRIMARY"); //只能通过主键查询
	AwsMap* pReadItem = new AwsMap;
	assert(pReadItem);
	pReadItem->AddValue("/Keys[0]/rid/S", m_sRid);
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

void TbUser::OnReadItemReq(AwsMap* pReadItem)
{
	assert(pReadItem);
	AwsList* pKeys = pReadItem->GetAwsMap("RequestItems")->GetAwsMap(GetTableName())->GetAwsList("Keys");
	AwsMap* pKey = new AwsMap;
	assert(pKey);
	pKey->AddValue("/rid/S", m_sRid);
	pKeys->SetValue(pKey, true);
}

int TbUser::OnReadItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbUser::OnDeleteItemReq(
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
	pDeleteItem->AddValue("/Key/rid/S", m_sRid);
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

int TbUser::OnDeleteItemReq(string& sPostData,
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

int TbUser::OnDeleteItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbUser::OnGetItemReq(unsigned int udwIdxNo,
	bool bConsistentRead, bool bReturnConsumedCapacity)
{
	oJsonWriter.omitEndingLineFeed();
	assert(oTableDesc.mIndexDesc.find(udwIdxNo)!=oTableDesc.mIndexDesc.end());
	IndexDesc& idx_desc = oTableDesc.mIndexDesc[udwIdxNo];
	assert(idx_desc.sName == "PRIMARY"); //只能通过主键查询
	AwsMap* pGetItem = new AwsMap;
	assert(pGetItem);
	pGetItem->AddValue("/TableName", GetTableName());
	pGetItem->AddValue("/Key/rid/S", m_sRid);
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

int TbUser::OnGetItemReq(string& sPostData, unsigned int udwIdxNo,
	bool bConsistentRead, bool bReturnConsumedCapacity)
{
	AwsMap* pGetItem = OnGetItemReq(udwIdxNo, bConsistentRead, bReturnConsumedCapacity);
	ostringstream oss;
	pGetItem->Dump(oss);
	sPostData = oss.str();
	delete pGetItem;
	return 0;
}

int TbUser::OnGetItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

AwsMap* TbUser::OnPutItemReq(
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
	if (!m_sRid.empty())
	{
		pPutItem->AddValue("/Item/rid/S", JsonEncode(m_sRid, sJsonEncode));
	}
	if (!m_sEmail.empty())
	{
		pPutItem->AddValue("/Item/email/S", JsonEncode(m_sEmail, sJsonEncode));
	}
	if (!m_sPasswd.empty())
	{
		pPutItem->AddValue("/Item/passwd/S", JsonEncode(m_sPasswd, sJsonEncode));
	}
	if (!m_sLogin_platform.empty())
	{
		pPutItem->AddValue("/Item/login_platform/S", JsonEncode(m_sLogin_platform, sJsonEncode));
	}
	if (!m_sRegister_platfrom.empty())
	{
		pPutItem->AddValue("/Item/register_platfrom/S", JsonEncode(m_sRegister_platfrom, sJsonEncode));
	}
	if (!m_sType.empty())
	{
		pPutItem->AddValue("/Item/type/S", JsonEncode(m_sType, sJsonEncode));
	}
	pPutItem->AddValue("/Item/ctime/N", m_nCtime);
	pPutItem->AddValue("/Item/utime/N", m_nUtime);
	pPutItem->AddValue("/Item/logstatus/N", m_nLogstatus);
	pPutItem->AddValue("/Item/pwd_flag/N", m_nPwd_flag);
	pPutItem->AddValue("/Item/pwd_seq/N", m_nPwd_seq);
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

int TbUser::OnPutItemReq(string& sPostData,
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

int TbUser::OnPutItemRsp(const Json::Value& item)
{
	return OnResponse(item);
}

int TbUser::OnResponse(const Json::Value& item)
{
	oJsonWriter.omitEndingLineFeed();
	int dwResLen = 0;
	dwResLen = 0;
	this->Reset();
	Json::Value::Members vecMembers = item.getMemberNames();
	for (unsigned int i = 0; i < vecMembers.size(); ++i)
	{
 		if (vecMembers[i] == "rid")
		{
			m_sRid = item["rid"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "email")
		{
			m_sEmail = item["email"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "passwd")
		{
			m_sPasswd = item["passwd"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "login_platform")
		{
			m_sLogin_platform = item["login_platform"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "register_platfrom")
		{
			m_sRegister_platfrom = item["register_platfrom"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "type")
		{
			m_sType = item["type"]["S"].asString();
			continue;
		}
		if (vecMembers[i] == "ctime")
		{
			m_nCtime = strtoll(item["ctime"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "utime")
		{
			m_nUtime = strtoll(item["utime"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "logstatus")
		{
			m_nLogstatus = strtoll(item["logstatus"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "pwd_flag")
		{
			m_nPwd_flag = strtoll(item["pwd_flag"]["N"].asString().c_str(), NULL, 10);
			continue;
		}
		if (vecMembers[i] == "pwd_seq")
		{
			m_nPwd_seq = strtoll(item["pwd_seq"]["N"].asString().c_str(), NULL, 10);
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

TINT64 TbUser::GetSeq()
{
	return m_nSeq;
}

