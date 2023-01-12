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


};

}