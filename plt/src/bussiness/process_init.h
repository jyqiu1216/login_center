#ifndef _PROCESS_INIT_H_
#define _PROCESS_INIT_H_

#include "session.h"
#include "my_include.h"
#include "aws_table_include.h"
#include "game_command.h"


class CProcessInit
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
