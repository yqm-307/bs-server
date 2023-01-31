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
        // InitTable();    // 创建表结构
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


void DBHelper::SetServerInfo(int uid,int sid)
{
    auto result = ybs::share::util::cutil::executeCMD("./shell/getserverinfo_v1.sh");
    // DEBUG("%s",result.c_str());
    auto timenow = uint64_t(time(NULL));
    if (!runCommand(fmt("\
        update bs_db.server_info_table\
        set server_info = \'%s\' , last_update_time = %lu \
        where server_id = %d and user_id = %d\
    ",
        result.c_str(),
        timenow,
        sid,
        uid)))
    {
        ERROR("update failed!");
    }

}


std::tuple<int,int> DBHelper::ServerIsRegister()
{
    QueryResult<int,int> result;
    auto ownip = ybs::share::util::cutil::executeCMD("hostname -I| awk '{print $1}'"); 
    std::string ip;
    for (auto&& c : ownip)
        if (c == '\n')
            break;
        else
            ip+=c;
    
    if (!runQuery(&result,fmt("\
        select user_id,server_id\
        from bs_db.server_info_table\
        where server_ip = \'%s\'\
    ",  
    ip.c_str())))
    {
        ERROR("select failed!");
        return {0,0};
    }
    if (result.size() == 0)
    {
        return {0,0};
    }

    return result[0];

}
