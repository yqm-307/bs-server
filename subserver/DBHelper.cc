#include "subserver/DBHelper.hpp"
#include "subserver/ConfigManager.hpp"
#include "share/util/cmd.hpp"

using namespace SubServer;


#define fmt(args,...) (ybs::share::util::format(args,##__VA_ARGS__).c_str())

DBHelper* DBHelper::GetInstance()
{
    static DBHelper* instance = nullptr;
    if (instance == nullptr)
    {
        instance = new DBHelper();
    }
    return instance;
}

DBHelper::DBHelper()
    :DBHelper_Base(
        ConfigManager::GetInstance()->GetDBIP(),
        ConfigManager::GetInstance()->GetDBUsername(),
        ConfigManager::GetInstance()->GetDBPassword(),
        ConfigManager::GetInstance()->GetDBname()
    )
{
    try{
        InitTable();    // 创建表结构
    }
    catch(MySqlException& e)
    {
        FATAL("%s",e.what());
    }
    INFO("mysql init success!");
}


void DBHelper::InitTable()
{
}


void DBHelper::SetServerInfo()
{
    auto sys_info = ybs::share::util::cutil::executeCMD("uname -srm");
    auto time_now = (uint64_t)time(NULL);
    
}
