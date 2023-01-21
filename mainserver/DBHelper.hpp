#pragma once
#include "share/mysql/DBHelper_Base.hpp"

namespace MainServer
{

class DBHelper:public ybs::share::mysql::DBHelper_Base
{
public:
    DBHelper();
    ~DBHelper(){}
    static DBHelper* GetInstance();


public:
    /*********************************************************
     * Test Table
     * 
     *********************************************************/
    std::vector<std::tuple<int,std::string,int>> Test_GetUserInfo(int userid);
    bool Test_SetUserInfo(int userid,std::string& name,int age);


    /*********************************************************
     * User Table
     * 
     *********************************************************/
    // 根据账号查询用户信息
    std::vector<std::tuple<int,int,std::string>> User_GetUserInfo(int passport);

    // 插入一个新用户的账号信息
    int User_SetUserInfo(int portport ,std::string & password);

    /*********************************************************
     * server info Table
     * 
     *********************************************************/
    



private:
    void InitTable();

};

}

