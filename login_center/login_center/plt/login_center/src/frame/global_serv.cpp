#include "rc4.h"
#include "statistic.h"
#include "aws_table_include.h"
#include "game_command.h"
#include "global_serv.h"
#include "my_include.h"
#include "conf_base.h"
#include "static_file_mgr.h"
#include "game_svr.h"


// 静态变量定义
CTseLogger*	CGlobalServ::m_poServLog = NULL;
CTseLogger*	CGlobalServ::m_poDayReqLog = NULL;
CTseLogger* CGlobalServ::m_poRegLog = NULL;
CTseLogger* CGlobalServ::m_poStatLog = NULL;
CTseLogger* CGlobalServ::m_poFormatLog = NULL;
CConf* CGlobalServ::m_poConf = NULL;
CTaskQueue* CGlobalServ::m_poTaskQueue = NULL;
CQueryNetIO* CGlobalServ::m_poQueryNetIO = NULL;
CSearchNetIO* CGlobalServ::m_poSearchNetIO = NULL;
CTaskProcess* CGlobalServ::m_poTaskProcess = NULL;
CTseLogger* CGlobalServ::m_poJasonLog = NULL;
CZkRegConf*	CGlobalServ::m_poZkConf = NULL;
CZkRegClient* CGlobalServ::m_poZkRegClient = NULL;



TUINT32		CGlobalServ::m_udwHsReqSeq = 1;
pthread_mutex_t CGlobalServ::m_mtxHsReqSeq = PTHREAD_MUTEX_INITIALIZER;


TUINT32 CGlobalServ::GenerateHsReqSeq()
{
    TUINT32 udwSeq = 0;
    pthread_mutex_lock(&m_mtxHsReqSeq);
    m_udwHsReqSeq++;
    if(m_udwHsReqSeq < 100000)
    {
        m_udwHsReqSeq = 100000;
    }
    udwSeq = m_udwHsReqSeq;
    pthread_mutex_unlock(&m_mtxHsReqSeq);
    return udwSeq;
}

int CGlobalServ::InitAwsTable(const TCHAR *pszProjectPrefix)
{
    TbProduct::Init("../tblxml/product.xml", pszProjectPrefix);
    TbUser::Init("../tblxml/user.xml", pszProjectPrefix);

    return 0;
}

int CGlobalServ::Init()
{
    ostringstream oss;

    // 1. 初始化日志对象
    INIT_LOG_MODULE("../conf/serv_log.conf");
    config_ret = 0;
    DEFINE_LOG(serv_log, "serv_log");
    DEFINE_LOG(reg_log, "reg_log");
    DEFINE_LOG(req_log, "req_log");
    DEFINE_LOG(stat_log, "stat_log");
    m_poServLog = serv_log;
    m_poDayReqLog = req_log;
    m_poRegLog = reg_log;
    m_poStatLog = stat_log;


    // 2. 初始化配置文件
    m_poConf = new CConf;
    if(0 != m_poConf->Init("../conf/serv_info.conf"))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init conf failed!"));
        return -2;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init conf success!"));

    if(0 != CConfBase::Init("../conf/module.conf", m_poConf->m_szModule))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init project.conf failed!"));
        return -20;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init project.conf success!"));

    
    if (0 != CGameSvrInfo::GetInstance()->Init("../data/game_svr.info", m_poServLog))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init game_svr failed!"));
        return -13;
    }

    // 配置文件需要先读取，才能初始化表
    InitAwsTable(CConfBase::GetString("project").c_str());


    // 初始化静态数据的md5_list管理器
    CStaticDataMd5ListMgr *poStaticDataMd5 = CStaticDataMd5ListMgr::GetInstance();
    if (0 != poStaticDataMd5->InitStaticData(m_poServLog))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init static file md5 list mgr failed!"));
        return -17;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init static file md5 list mgr success!"));


    // 初始化account_status管理器
    CStaticDataAccountStatusMgr *poStaticDataAccountStatusMgr = CStaticDataAccountStatusMgr::GetInstance();
    if (0 != poStaticDataAccountStatusMgr->InitStaticData(m_poServLog))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init account_status mgr failed!"));
        return -17;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init account_status mgr success!"));


    // 初始化静态文件管理器
    CStaticFileMgr *poStaticFileMgr = CStaticFileMgr::GetInstance();
    if (0 != poStaticFileMgr->Init(m_poServLog))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init static file mgr failed!"));
        return -17;
    }
    CStaticFileMgr::GetInstance()->CheckStaticFileUpdate();
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init static file mgr success!"));


    // 建立md5_list(只能在静态文件管理器初始化之后才能调用)
    CStaticDataMd5ListMgr::GetInstance()->GenStaticDataMd5List();
   
    m_poZkConf = new CZkRegConf;
    if (0 != m_poZkConf->Init("../conf/serv_info.conf", "../conf/module.conf"))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init zk_conf failed!"));
        return -3;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init zk_conf success!"));

   
    m_poZkRegClient = new CZkRegClient;
    if (0 != m_poZkRegClient->Init(m_poRegLog, m_poZkConf))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ Init reg_client_zk failed"));
        return -4;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ Init reg_client_zk succ"));

    // 初始化命令字信息
    CClientCmd *poClientCmd = CClientCmd::GetInstance();
    if(0 != poClientCmd->Init())
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init client cmd info failed!"));
        return -17;
    }

    // 3. 初始化队列信息
    m_poTaskQueue = new CTaskQueue();
    if(0 != m_poTaskQueue->Init(CConfBase::GetInt("queue_size")))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init task queue failed! [queue_size=%u]",
            CConfBase::GetInt("queue_size")));
        return -3;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init task_queue success!"));

    // 4. 初始化Session管理器
    if(0 != CSessionMgr::Instance()->Init(CConfBase::GetInt("queue_size"), m_poServLog))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init session_mgr failed!"));
        return -4;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init session_mgr success!"));

    // 5. 初始化网络IO
    m_poQueryNetIO = new CQueryNetIO;
    if(0 != m_poQueryNetIO->Init(m_poConf, m_poServLog, m_poTaskQueue))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init net_io failed!"));
        return -5;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init net_io success!"));

    // 6. 初始化网络IO
    m_poSearchNetIO = new CSearchNetIO;
    if(0 != m_poSearchNetIO->Init(m_poConf, m_poServLog, m_poRegLog, m_poTaskQueue))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init net_io failed!"));
        return -6;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init net_io success!"));


    //初始化zk下游管理器
    if (0 != CDownMgr::Instance()->zk_Init(m_poRegLog, m_poSearchNetIO->m_pLongConn, m_poZkConf))
    {
        TSE_LOG_ERROR(m_poServLog, ("GlobalServ init session_mgr failed!"));
        return -7;
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init zk_session_mgr success!"))

    // 8. 初始化工作线程
    m_poTaskProcess = new CTaskProcess[CConfBase::GetInt("work_thread_num")];
    for (TINT32 dwIdx = 0; dwIdx < CConfBase::GetInt("work_thread_num"); dwIdx++)
    {
        if (0 != m_poTaskProcess[dwIdx].Init(m_poConf, m_poSearchNetIO->m_pLongConn,
            m_poQueryNetIO->m_pLongConn, m_poTaskQueue, m_poServLog, m_poDayReqLog))
        {
            TSE_LOG_ERROR(m_poServLog, ("GlobalServ init %uth task process failed!", dwIdx));
            return -8;
        }
    }
    TSE_LOG_INFO(m_poServLog, ("GlobalServ init task_process success!"));

    // 9. 初始化rc4算法管理器
    CEncryptCR4Mgr::get_instance();

    // 10. statistic
    CStatistic *pstStatistic = CStatistic::Instance();
    pstStatistic->Init(m_poStatLog, m_poConf);


    TSE_LOG_INFO(m_poServLog, ("GlobalServ init all success!"));

    return 0;
}

int CGlobalServ::Uninit()
{
    // do nothing
    return 0;
}

int CGlobalServ::Start()
{
    pthread_t thread_id = 0;
    TINT32 dwIdx = 0;

    // 1. 创建工作线程
    for (dwIdx = 0; dwIdx < CConfBase::GetInt("work_thread_num"); dwIdx++)
    {
        if (pthread_create(&thread_id, NULL, CTaskProcess::Start, &m_poTaskProcess[dwIdx]) != 0)
        {
            TSE_LOG_ERROR(m_poServLog, ("create task process thread failed! [idx=%d] Error[%s]",
                dwIdx, strerror(errno)));
            return -1;
        }
    }
    TSE_LOG_INFO(m_poServLog, ("create task_process thread success!"));
    

    //2.1创建zk下游管理线程
    if (pthread_create(&thread_id, NULL, CDownMgr::zk_StartPull, CDownMgr::Instance()) != 0)
    {
        TSE_LOG_ERROR(m_poServLog, ("create down manager thread failed! Error[%s]", strerror(errno)));
        return -2;
    }
    if (pthread_create(&thread_id, NULL, CDownMgr::zk_StartCheck, CDownMgr::Instance()) != 0)
    {
        TSE_LOG_ERROR(m_poServLog, ("create down manager thread failed! Error[%s]", strerror(errno)));
        return -2;
    }

    // 3. 创建网络线程-search
    if(pthread_create(&thread_id, NULL, CSearchNetIO::RoutineNetIO, m_poSearchNetIO) != 0)
    {
        TSE_LOG_ERROR(m_poServLog, ("create seach net io thread failed! Error[%s]",
            strerror(errno)));
        return -3;
    }
    TSE_LOG_INFO(m_poServLog, ("create net io thread success!"));


    // 5. 开启统计线程
    if(pthread_create(&thread_id, NULL, CStatistic::Start, CStatistic::Instance()) != 0)
    {
        TSE_LOG_ERROR(m_poServLog, ("create statistic thread failed! Error[%s]",
            strerror(errno)));
        return -5;
    }
    TSE_LOG_INFO(m_poServLog, ("create statistic thread success!"));


    m_poZkRegClient->Start();
    TSE_LOG_INFO(m_poServLog, ("create zk reg thread success!"));


    // 4. 创建网络线程-query
    if (pthread_create(&thread_id, NULL, CQueryNetIO::RoutineNetIO, m_poQueryNetIO) != 0)
    {
        TSE_LOG_ERROR(m_poServLog, ("create query net io thread failed! Error[%s]",
            strerror(errno)));
        return -4;
    }
    TSE_LOG_INFO(m_poServLog, ("create net io thread success!"));


    TSE_LOG_INFO(m_poServLog, ("GlobalServ start all thread success!"));
    return 0;
}

int CGlobalServ::StopNet()
{
    // 停止query线程的接收请求端口
    m_poQueryNetIO->CloseListenSocket();
    return 0;
}
