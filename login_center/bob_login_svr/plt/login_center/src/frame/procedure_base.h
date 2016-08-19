#ifndef _PROCEDURE_BASE_H_
#define _PROCEDURE_BASE_H_

#include "session.h"
// #include "model.h"

class CBaseProcedure
{
public:
    static string GetTableRawName(const string& strTableName);
	static TINT32 SendAwsRequest(SSession *pstSession, TUINT16 uwReqServiceType);

public:
    static TINT32 SendLoginGetRequest(SSession *pstSession);

private:
    static string GetReqUrl_LoginGet(SSession *pstSession);
};

#endif
