#include "DBHelper.hpp"
#include "ConfigManager.hpp"


using namespace MainServer;

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

std::vector<std::tuple<int,std::string,int>> DBHelper::Test_GetUserInfo(int userid)
{
    std::vector<std::tuple<int,std::string,int>> result;
    try
    {   
        this->m_sqlconn->runQuery(&result,"select * from test_user_info where user_id = ?",userid);

    }
    catch(const std::exception& e)
    {
        ERROR("%s",e.what());
    }   
    return result;
    
}

bool DBHelper::Test_SetUserInfo(int userid,std::string& name,int age)
{
    std::vector<std::tuple<int,std::string,int>> result;
    bool flag{true};
    std::tuple<int,std::string,int> ret(userid,name,age);
    result.push_back(ret);
    try
    {
        this->m_sqlconn->runQuery(&result,"insert into test_user_info (user_id,name,age) values ( ? , ? , ? )",userid,name,age);
    }
    catch(const std::exception& e)
    {
        ERROR("%s",e.what());
        flag=false;
    }
    return flag;
    
}


std::vector<std::tuple<int,std::string>> DBHelper::User_GetUserInfo(int passport)
{
    std::vector<std::tuple<int,std::string>> result;
    try
    {   
        this->m_sqlconn->runQuery(&result,"select passport,password from bs_db.user_info_table where user_id = ?",passport);
    }
    catch(const std::exception& e)
    {
        ERROR("%s",e.what());
    }   
    return result;
}

