#ifndef _TIME_UTILS_H_
#define _TIME_UTILS_H_

#include "base/common/wtse_std_header.h"

class CTimeUtils
{
public:
	//��ȡ��������ȷ��΢��
	static TUINT64 GetCurTimeUs();

	//��ȡ��������ȷ����
	static TUINT32 GetUnixTime();

    //��ȡ������
    static TUINT64 GetCurTimeMs();

public:
	static TVOID GetFormatTimeFromSec(TUINT32 udwSecs, TCHAR *pszOut);

public:
    static TUINT16 GetYearMonth(TUINT32 udwCurTime = 0);

public:
    static TUINT32 GetUinxTimeFromFormatTime(const TCHAR *szFormatTime);

};

#endif