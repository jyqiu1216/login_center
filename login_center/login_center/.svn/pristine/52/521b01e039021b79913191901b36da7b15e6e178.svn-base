#ifndef _AWS_TABLE_PRODUCT_H_
#define _AWS_TABLE_PRODUCT_H_

#include "aws_table_common.h"
#include "field_property.h"

#define EN_AWS_TABLE_PRODUCT "product"

enum ETbProductField
{
	TbPRODUCT_FIELD_APP_UID = 0,
	TbPRODUCT_FIELD_R_PID = 1,
	TbPRODUCT_FIELD_SID = 2,
	TbPRODUCT_FIELD_DEVICE = 3,
	TbPRODUCT_FIELD_RID = 4,
	TbPRODUCT_FIELD_PRODUCT_INFO = 5,
	TbPRODUCT_FIELD_BTIME = 6,
	TbPRODUCT_FIELD_LGTIME = 7,
	TbPRODUCT_FIELD_STATUS = 8,
	TbPRODUCT_FIELD_SEQ = 9,
};

enum ETbProductOpenType
{
	ETbPRODUCT_OPEN_TYPE_PRIMARY = 0,
	ETbPRODUCT_OPEN_TYPE_GLB_R_PID = 1,
	ETbPRODUCT_OPEN_TYPE_GLB_DEVICE = 2,
	ETbPRODUCT_OPEN_TYPE_GLB_RID = 3,
};

class TbProduct : public AwsTable
{
public:
	string m_sApp_uid;
	string m_sR_pid;
	TINT64 m_nSid;
	string m_sDevice;
	string m_sRid;
	string m_sProduct_info;
	TINT64 m_nBtime;
	TINT64 m_nLgtime;
	TINT64 m_nStatus;
	TINT64 m_nSeq;

public:
	static TableDesc oTableDesc;
	Json::Reader oJsonReader;
	Json::FastWriter oJsonWriter;
	static int Init(const string& sConfFile, const string strProjectName);

public:
	TbProduct():AwsTable(&oTableDesc)
	{
		Reset();
	}
	~TbProduct()
	{}
	void Reset()
	{
		m_sApp_uid = "";
		m_sR_pid = "";
		m_nSid = 0;
		m_sDevice = "";
		m_sRid = "";
		m_sProduct_info = "";
		m_nBtime = 0;
		m_nLgtime = 0;
		m_nStatus = 0;
		m_nSeq = 0;
		ClearFlag();
	};
	const string& Get_App_uid()
	{
		return m_sApp_uid;
	}
	void Set_App_uid(const string& sApp_uid)
	{
		m_sApp_uid = sApp_uid;
	}
	const string& Get_R_pid()
	{
		return m_sR_pid;
	}
	void Set_R_pid(const string& sR_pid)
	{
		m_sR_pid = sR_pid;
	}
	const TINT64& Get_Sid()
	{
		return m_nSid;
	}
	void Set_Sid(const TINT64& nSid, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nSid = nSid;
		m_mFlag[TbPRODUCT_FIELD_SID] = dwActionType;
	}
	const string& Get_Device()
	{
		return m_sDevice;
	}
	void Set_Device(const string& sDevice)
	{
		m_sDevice = sDevice;
		m_mFlag[TbPRODUCT_FIELD_DEVICE] = sDevice.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const string& Get_Rid()
	{
		return m_sRid;
	}
	void Set_Rid(const string& sRid)
	{
		m_sRid = sRid;
		m_mFlag[TbPRODUCT_FIELD_RID] = sRid.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const string& Get_Product_info()
	{
		return m_sProduct_info;
	}
	void Set_Product_info(const string& sProduct_info)
	{
		m_sProduct_info = sProduct_info;
		m_mFlag[TbPRODUCT_FIELD_PRODUCT_INFO] = sProduct_info.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const TINT64& Get_Btime()
	{
		return m_nBtime;
	}
	void Set_Btime(const TINT64& nBtime, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nBtime = nBtime;
		m_mFlag[TbPRODUCT_FIELD_BTIME] = dwActionType;
	}
	const TINT64& Get_Lgtime()
	{
		return m_nLgtime;
	}
	void Set_Lgtime(const TINT64& nLgtime, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nLgtime = nLgtime;
		m_mFlag[TbPRODUCT_FIELD_LGTIME] = dwActionType;
	}
	const TINT64& Get_Status()
	{
		return m_nStatus;
	}
	void Set_Status(const TINT64& nStatus, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nStatus = nStatus;
		m_mFlag[TbPRODUCT_FIELD_STATUS] = dwActionType;
	}
	const TINT64& Get_Seq()
	{
		return m_nSeq;
	}
	void Set_Seq(const TINT64& nSeq, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nSeq = nSeq;
		m_mFlag[TbPRODUCT_FIELD_SEQ] = dwActionType;
	}
	//Scan命令使用,返回完整的请求包
	AwsMap* OnScanReq(unsigned int udwIdxNo, bool bHasStartKey = false, bool bReturnConsumedCapacity = true,
		unsigned int dwLimit = 0, unsigned int dwSegment = 0, unsigned int dwTotalSegments = 0);
	int OnScanReq(string& sPostData, unsigned int udwIdxNo, bool bHasStartKey = false, bool bReturnConsumedCapacity = true,
		unsigned int dwLimit = 0, unsigned int dwSegment = 0, unsigned int dwTotalSegments = 0);
	int OnScanRsp(const Json::Value& item);
	//Query命令使用,返回完整的请求包
	AwsMap* OnQueryReq(unsigned int udwIdxNo, const CompareDesc& comp_desc = CompareDesc(), 
		bool bConsistentRead = true, bool bReturnConsumedCapacity = true, bool bScanIndexForward = true, unsigned int dwLimit = 0, bool bCount = false);
	int OnQueryReq(string& sPostData, unsigned int udwIdxNo, const CompareDesc& comp_desc = CompareDesc(), 
		bool bConsistentRead = true, bool bReturnConsumedCapacity = true, bool bScanIndexForward = true, unsigned int dwLimit = 0);
	int OnQueryRsp(const Json::Value& item);
	//查找记录数量
	int OnCountReq(string& sPostData, unsigned int udwIdxNo, const CompareDesc& comp_desc = CompareDesc(), 
		bool bConsistentRead = true, bool bReturnConsumedCapacity = true, bool bScanIndexForward = true, unsigned int dwLimit = 0);	//UpdateItem命令使用,返回完整的请求包
	AwsMap* OnUpdateItemReq(
		const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE, bool bReturnConsumedCapacity = true);
	int OnUpdateItemReq(string& sPostData,
		const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE, bool bReturnConsumedCapacity = true);
	int OnUpdateItemRsp(const Json::Value& item);
	//BatchWriteItem命令使用
	AwsMap* OnWriteItemReq(int dwActionType = WRITE_ACTION_TYPE_PUT);
	void OnWriteItemReq(AwsMap* pWriteItem, int dwActionType = WRITE_ACTION_TYPE_PUT);
	//BatchGetItem命令使用
	AwsMap* OnReadItemReq(unsigned int udwIdxNo);
	void OnReadItemReq(AwsMap*pReadItem);
	int OnReadItemRsp(const Json::Value& item);
	//DeleteItem命令使用,返回完整的请求包
	AwsMap* OnDeleteItemReq(
		const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE, bool bReturnConsumedCapacity = true);
	int OnDeleteItemReq(string& sPostData,
		const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE, bool bReturnConsumedCapacity = true);
	int OnDeleteItemRsp(const Json::Value& item);
	//GetItem命令使用,返回完整的请求包
	AwsMap* OnGetItemReq(unsigned int udwIdxNo,
		bool bConsistentRead = true, bool bReturnConsumedCapacity = true);
	int OnGetItemReq(string& sPostData, unsigned int udwIdxNo,
		bool bConsistentRead = true, bool bReturnConsumedCapacity = true);
	int OnGetItemRsp(const Json::Value& item);
	//PutItem命令使用,返回完整的请求包
	AwsMap* OnPutItemReq(
		const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE, bool bReturnConsumedCapacity = true);
	int OnPutItemReq(string& sPostData,
		const ExpectedDesc& expected_desc = ExpectedDesc(), int dwReturnValuesType = RETURN_VALUES_NONE, bool bReturnConsumedCapacity = true);
	int OnPutItemRsp(const Json::Value& item);
	int OnResponse(const Json::Value& item);
public: 
	static AwsTable* NewObject();
	string GetTableName();
	TINT32 GetTableIdx();
public: 
	TINT64 GetSeq();
};

#endif 

