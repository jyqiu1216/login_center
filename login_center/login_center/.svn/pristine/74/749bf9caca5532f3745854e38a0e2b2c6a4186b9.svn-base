#ifndef _SSC_CONF_H_
#define _SSC_CONF_H_

#include "my_include.h"
#include "my_define.h"

class CConf
{
public:
    TCHAR m_szModule[DEFAULT_NAME_STR_LEN];
    TINT32 m_dwModuleGroup;
    TCHAR m_szModuleIp[MAX_IP_LEN];
    TCHAR m_szModuleSvr[DEFAULT_SVR_LIST_STR_LEN];
    TINT32 m_dwModulePriority;
    TCHAR m_szHuOpUrlPre[DEFAULT_NAME_STR_LEN];

public:
	TINT32 Init(const TCHAR *pszConfFile);
private:
    TVOID TrimStr(TCHAR *szSrc, TCHAR cChar);

};

#endif
