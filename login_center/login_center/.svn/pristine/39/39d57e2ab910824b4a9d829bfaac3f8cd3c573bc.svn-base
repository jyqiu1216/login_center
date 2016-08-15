#ifndef _AWS_TABLE_USER_H_
#define _AWS_TABLE_USER_H_

#include "aws_table_common.h"
#include "field_property.h"

#define EN_AWS_TABLE_USER "user"

enum ETbUserField
{
	TbUSER_FIELD_RID = 0,
	TbUSER_FIELD_EMAIL = 1,
	TbUSER_FIELD_PASSWD = 2,
	TbUSER_FIELD_LOGIN_PLATFORM = 3,
	TbUSER_FIELD_REGISTER_PLATFROM = 4,
	TbUSER_FIELD_TYPE = 5,
	TbUSER_FIELD_CTIME = 6,
	TbUSER_FIELD_UTIME = 7,
	TbUSER_FIELD_LOGSTATUS = 8,
	TbUSER_FIELD_PWD_FLAG = 9,
	TbUSER_FIELD_PWD_SEQ = 10,
	TbUSER_FIELD_STATUS = 11,
	TbUSER_FIELD_SEQ = 12,
};

enum ETbUserOpenType
{
	ETbUSER_OPEN_TYPE_PRIMARY = 0,
	ETbUSER_OPEN_TYPE_GLB_EMAIL = 1,
};

class TbUser : public AwsTable
{
public:
	string m_sRid;
	string m_sEmail;
	string m_sPasswd;
	string m_sLogin_platform;
	string m_sRegister_platfrom;
	string m_sType;
	TINT64 m_nCtime;
	TINT64 m_nUtime;
	TINT64 m_nLogstatus;
	TINT64 m_nPwd_flag;
	TINT64 m_nPwd_seq;
	TINT64 m_nStatus;
	TINT64 m_nSeq;

public:
	static TableDesc oTableDesc;
	Json::Reader oJsonReader;
	Json::FastWriter oJsonWriter;
	static int Init(const string& sConfFile, const string strProjectName);

public:
	TbUser():AwsTable(&oTableDesc)
	{
		Reset();
	}
	~TbUser()
	{}
	void Reset()
	{
		m_sRid = "";
		m_sEmail = "";
		m_sPasswd = "";
		m_sLogin_platform = "";
		m_sRegister_platfrom = "";
		m_sType = "";
		m_nCtime = 0;
		m_nUtime = 0;
		m_nLogstatus = 0;
		m_nPwd_flag = 0;
		m_nPwd_seq = 0;
		m_nStatus = 0;
		m_nSeq = 0;
		ClearFlag();
	};
	const string& Get_Rid()
	{
		return m_sRid;
	}
	void Set_Rid(const string& sRid)
	{
		m_sRid = sRid;
	}
	const string& Get_Email()
	{
		return m_sEmail;
	}
	void Set_Email(const string& sEmail)
	{
		m_sEmail = sEmail;
		m_mFlag[TbUSER_FIELD_EMAIL] = sEmail.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const string& Get_Passwd()
	{
		return m_sPasswd;
	}
	void Set_Passwd(const string& sPasswd)
	{
		m_sPasswd = sPasswd;
		m_mFlag[TbUSER_FIELD_PASSWD] = sPasswd.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const string& Get_Login_platform()
	{
		return m_sLogin_platform;
	}
	void Set_Login_platform(const string& sLogin_platform)
	{
		m_sLogin_platform = sLogin_platform;
		m_mFlag[TbUSER_FIELD_LOGIN_PLATFORM] = sLogin_platform.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const string& Get_Register_platfrom()
	{
		return m_sRegister_platfrom;
	}
	void Set_Register_platfrom(const string& sRegister_platfrom)
	{
		m_sRegister_platfrom = sRegister_platfrom;
		m_mFlag[TbUSER_FIELD_REGISTER_PLATFROM] = sRegister_platfrom.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const string& Get_Type()
	{
		return m_sType;
	}
	void Set_Type(const string& sType)
	{
		m_sType = sType;
		m_mFlag[TbUSER_FIELD_TYPE] = sType.empty() ? UPDATE_ACTION_TYPE_DELETE : UPDATE_ACTION_TYPE_PUT;
	}
	const TINT64& Get_Ctime()
	{
		return m_nCtime;
	}
	void Set_Ctime(const TINT64& nCtime, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nCtime = nCtime;
		m_mFlag[TbUSER_FIELD_CTIME] = dwActionType;
	}
	const TINT64& Get_Utime()
	{
		return m_nUtime;
	}
	void Set_Utime(const TINT64& nUtime, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nUtime = nUtime;
		m_mFlag[TbUSER_FIELD_UTIME] = dwActionType;
	}
	const TINT64& Get_Logstatus()
	{
		return m_nLogstatus;
	}
	void Set_Logstatus(const TINT64& nLogstatus, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nLogstatus = nLogstatus;
		m_mFlag[TbUSER_FIELD_LOGSTATUS] = dwActionType;
	}
	const TINT64& Get_Pwd_flag()
	{
		return m_nPwd_flag;
	}
	void Set_Pwd_flag(const TINT64& nPwd_flag, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nPwd_flag = nPwd_flag;
		m_mFlag[TbUSER_FIELD_PWD_FLAG] = dwActionType;
	}
	const TINT64& Get_Pwd_seq()
	{
		return m_nPwd_seq;
	}
	void Set_Pwd_seq(const TINT64& nPwd_seq, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nPwd_seq = nPwd_seq;
		m_mFlag[TbUSER_FIELD_PWD_SEQ] = dwActionType;
	}
	const TINT64& Get_Status()
	{
		return m_nStatus;
	}
	void Set_Status(const TINT64& nStatus, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nStatus = nStatus;
		m_mFlag[TbUSER_FIELD_STATUS] = dwActionType;
	}
	const TINT64& Get_Seq()
	{
		return m_nSeq;
	}
	void Set_Seq(const TINT64& nSeq, int dwActionType=UPDATE_ACTION_TYPE_PUT)
	{
		m_nSeq = nSeq;
		m_mFlag[TbUSER_FIELD_SEQ] = dwActionType;
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

