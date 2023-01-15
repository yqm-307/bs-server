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
    std::vector<std::tuple<int,std::string>> User_GetUserInfo(int passport);

private:
    void InitTable()
    {
        this->m_sqlconn->runCommand("\
        CREATE TABLE IF NOT EXISTS user_info_table(\
        user_id INT UNSIGNED  AUTO_INCREMENT,\
        passport INT,\
        password VARCHAR(10),\
        PRIMARY KEY ( user_id )\
        )ENGINE=InnoDB DEFAULT CHARSET=utf8;");

        INFO("mysql table create success! table name: user_info_table");


    
    }

};

}

