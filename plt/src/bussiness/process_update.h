#ifndef _PROCESS_UPDATE_H_
#define _PROCESS_UPDATE_H_

#include "session.h"

class CProcessUpdate
{

public:
    static TINT32 requestHandler(SSession* pstSession, TBOOL &bNeedResponse);

private:
    template<typename TNumber>
    static string NumToString(TNumber Input)
    {
        ostringstream oss;
        oss << Input;
        return oss.str();
    }
   
};



#endif
