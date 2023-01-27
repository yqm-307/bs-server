#include "mainserver/DBHelper.hpp"
#include "mainserver/ConfigManager.hpp"


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
    // user_info_table
    runCommand(fmt("\
        CREATE TABLE IF NOT EXISTS bs_db.user_info_table(\
        user_id INT UNSIGNED  AUTO_INCREMENT,\
        passport INT,\
        password VARCHAR(10),\
        qx_level INT,\
        group_id INT,\
        group_type INT,\
        PRIMARY KEY ( user_id )\
        )ENGINE=InnoDB DEFAULT CHARSET=utf8;\
        "));

    // server_info_table
    runCommand(fmt("\
        CREATE TABLE IF NOT EXISTS bs_db.server_info_table(\
        user_id INT UNSIGNED,\
        server_id INT UNSIGNED,\
        server_ip VARCHAR(30),\
        server_port INT UNSIGNED,\
        server_level INT,\
        flag_ufw INT,\
        linux_username VARCHAR(255),\
        linux_pwd VARCHAR(255),\
        open_port MEDIUMTEXT,\
        server_info MEDIUMTEXT,\
        last_update_time BIGINT UNSIGNED,\
        ssh_flag INT UNSIGNED,\
        ssh_publickey TEXT,\
        PRIMARY KEY ( user_id )\
        )ENGINE=InnoDB DEFAULT CHARSET=utf8;\
    "));

    
    // runCommand(fmt("\
    //     CREATE TABLE IF NOT EXISTS bs_db.server_updata_table(\
    //     server_id INT UNSIGNED,\
    //     linux_sys_info TEXT,\
    //     cpu_info TEXT,\
    //     )ENGINE=InnoDB DEFAULT CHARSET=utf8;\
    // "));


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


std::vector<std::tuple<int,int,std::string>> DBHelper::User_GetUserInfo(int passport)
{
    std::vector<std::tuple<int,int,std::string>> result;
    try
    {   
        runQuery(&result,fmt("select user_id,passport,password from bs_db.user_info_table where passport = %d ",passport));
    }
    catch(const std::exception& e)
    {
        ERROR("%s",e.what());
    }   
    return result;
}


int DBHelper::User_SetUserInfo(int uid,int passport ,std::string & password,int level)
{
    enum Errtype{
        ok = 1,
        passport_exist = 2,
        pwd_bad =3,
        sql_error = 4,
        level_bad = 5,
    };
    // 注册的用户权限是否为root权限
    QueryResult<int> user;
    if (!runQuery(&user,fmt("\
        select qx_level\
        from user_info_table\
        where user_id=%d\
    ",
    uid)))
    {
        ERROR("select failed!");
        return sql_error;
    }

    if (user.size() == 0)
    {
        FATAL("使用不存在用户登录成功");
        return sql_error;
    }
    if (std::get<0>(user[0]) != 1)
    {
        return level_bad;
    }

    auto vec = User_GetUserInfo(passport);
    if (vec.size() != 0)
    {// 账号已经存在
        ERROR("passport is exists!");
        return  passport_exist;
    }
    if (password.size() == 0)
    {
        // 密码非法
        ERROR("password invalid!");
        return pwd_bad;
    }

    try{
        runCommand(fmt("\
        insert into bs_db.user_info_table\
            (passport,password,qx_level) values\
            ( %d , '%s' , %d )\
        ",
        passport,
        password.c_str(),
        level));
    }catch(const std::exception& e)
    {
        // sql执行失败
        ERROR("%s",e.what());
        return sql_error;
    }
    return ok;
}


int DBHelper::Server_UIDANDSID_Is_Repeat(int uid,int sid)
{
    // ResultType(int,int) resutlt1;
    int ret=0;
    do
    {
        QueryResult<int,int>   r1;
        QueryResult<int> r2;
        // user 是否存在
        if (!runQuery(&r2,fmt("\
            select user_id from \
            bs_db.user_info_table\
            where user_id = %d\
            ",
            uid)))
        {
            ERROR("select failed!");
            ret = 1;
            break;
        }
        if (r2.size() == 0)
        {
            ret = 2;
            break;
        }
        if (!runQuery(&r1,fmt("\
            select user_id,server_id\
            from bs_db.server_info_table\
            where user_id = %d",uid,sid)))
        {
            ERROR("select failed!");
            ret = 1;    // sql失败
            break;
        }

        for (auto && i : r1)
        {   // sid 是否重复
            if (std::get<1>(i) == sid)
            {
                ret = 3;
                break;
            }
        }
    } while (0);
    return ret;
}

bool DBHelper::Server_AddNewServerInfo(
    uint32_t user_id,
    uint32_t server_id,
    const std::string &server_ip,
    uint32_t server_port,
    int32_t server_level,
    int32_t flag_ufw,
    const std::string &username,
    const std::string &pwd)
{
    if (!runCommand(fmt("\
        insert into bs_db.server_info_table \
        (user_id,server_id,server_ip,server_port,server_level,flag_ufw,linux_username,linux_pwd)\
        values\
        (%u,%u,\"%s\",%u,%d,%d,\"%s\",\"%s\")\
    ",
    user_id,
    server_id,
    server_ip.c_str(),
    server_port,
    server_level,
    flag_ufw,
    username.c_str(),
    pwd.c_str())))
    {
        ERROR("insert error!");
        return false;
    }
    return true;
}




DBHelper::QueryResult<std::string,uint64_t> DBHelper::Server_GetServerInfo(int uid,int sid)
{
    QueryResult<std::string,uint64_t> result;

    if (!runQuery(&result,fmt("\
        select server_info,last_update_time from\
            bs_db.server_info_table\
        where\
        user_id = %d and server_id = %d\
    ",
        uid,
        sid)))
    {
        ERROR("select failed!");
    }

    return result;
}