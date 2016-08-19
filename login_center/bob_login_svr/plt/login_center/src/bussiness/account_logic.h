#ifndef _ACCOUNT_LOGIC_H_
#define _ACCOUNT_LOGIC_H_

#include "session.h"
#include "my_include.h"
#include "aws_table_include.h"



class CAccountLogic
{
private:
    static TBOOL Compare_Uid(TbProduct *ptbProductA, TbProduct *ptbProductB);

public:
    // init/getdata
    static TINT32 GetInitPlayerStatus(SUserInfo *pstUserInfo, const SLoginInfo &stLoginInfo, TbProduct *&ptbTmpProduct);

    // update
    static TINT32 GetUpdatePlayerStatus(SUserInfo *pstUserInfo, const SLoginInfo &stLoginInfo);


};



#endif
