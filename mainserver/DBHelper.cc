#include "DBHelper.hpp"
#include "ConfigManager.hpp"


using namespace MainServer;


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
    runCommand(fmt("\
        CREATE TABLE IF NOT EXISTS user_info_table(\
        user_id INT UNSIGNED  AUTO_INCREMENT,\
        passport INT,\
        password VARCHAR(10),\
        PRIMARY KEY ( user_id )\
        )ENGINE=InnoDB DEFAULT CHARSET=utf8;"));

    INFO("mysql table create success! table name: user_info_table");
}

std::vector<std::tuple<int,std::string,int>> DBHelper::Test_GetUserInfo(int userid)
{
    std::vector<std::tuple<int,std::string,int>> result;
    try
    {   
        runQuery(&result,fmt("select * from test_user_info where user_id = ?",userid));
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
        runQuery(&result,
            fmt("\
                insert into\ 
                test_user_info\
                (user_id,name,age)\
                values\
                ( %d , %s , %d )",
            userid,name.c_str(),age));
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
        runQuery(&result,fmt("select passport,password from bs_db.user_info_table where passport = %d ",passport));
    }
    catch(const std::exception& e)
    {
        ERROR("%s",e.what());
    }   
    return result;
}


int DBHelper::User_SetUserInfo(int passport ,std::string & password)
{
    auto vec = User_GetUserInfo(passport);
    if (vec.size() != 0)
    {// 账号已经存在
        ERROR("passport is exists!");
        return 2;
    }
    if (password.size() == 0)
    {
        // 密码非法
        ERROR("password invalid!");
        return 3;
    }

    try{
        runCommand(fmt("\
        insert into bs_db.user_info_table\
            (passport,password) values\
            ( %d , '%s' )\
        ",passport,password.c_str()));
    }catch(const std::exception& e)
    {
        // sql执行失败
        ERROR("%s",e.what());
        return 4;
    }
    return 1;
}


