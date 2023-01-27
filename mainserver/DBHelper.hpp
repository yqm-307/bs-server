#pragma once
#include "share/mysql/DBHelper_Base.hpp"

namespace MainServer
{

class DBHelper:public ybs::share::mysql::DBHelper_Base
{
public:
    template<typename... Args>
    using QueryResult = std::vector<std::tuple<Args...>>;
    
    
    DBHelper();
    ~DBHelper(){}
    static DBHelper* GetInstance();


public:
    /*********************************************************
     * Test Table
     * 
     *********************************************************/
    QueryResult<int,std::string,int> Test_GetUserInfo(int userid);
    bool Test_SetUserInfo(int userid,std::string& name,int age);


    /*********************************************************
     * User Table
     * 
     *********************************************************/
    // 根据账号查询用户信息
    QueryResult<int,int,std::string> User_GetUserInfo(int passport);

    // 插入一个新用户的账号信息
    int User_SetUserInfo(int uid,int portport ,std::string & password,int level);

    /*********************************************************
     * server info Table
     * 
     *********************************************************/
    // 检查该user是否已经有该serverid
    int Server_UIDANDSID_Is_Repeat(int uid,int sid);
    // 插入一个新的服务器
    bool Server_AddNewServerInfo(
        uint32_t user_id,
        uint32_t server_id,
        const std::string& server_ip,
        uint32_t server_port,
        int32_t server_level,
        int32_t flag_ufw,
        const std::string& username,
        const std::string& pwd
        );
    // 获取服务器信息
    QueryResult<std::string,uint64_t> Server_GetServerInfo(int uid,int sid);



private:
    void InitTable();

};

}

