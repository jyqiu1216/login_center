#ifndef _BASE_CMD_PROCESS_H_
#define _BASE_CMD_PROCESS_H_

#include "session.h"

class CBaseCmdProcess
{
public:
    // function  ===> �����ִ������cmd�ַ�
    // in_value  ===> pstSession: ҵ��session
    // out_value ===> ���������ֵĴ�����(0:�ɹ�;��0:ʧ��)
    static TINT32 ProcedureCmd(SSession *pstSession, TBOOL &bNeedResponse);
};

#endif

