#pragma once
#include "share/mysql/DBHelper_Base.hpp"

namespace SubServer
{

class DBHelper:public ybs::share::mysql::DBHelper_Base
{
    template<typename... Args>
    using QueryResult = std::vector<std::tuple<Args...>>;
public:

    DBHelper();
    ~DBHelper(){}
    static DBHelper* GetInstance();


public:
    // 向数据库中写入 server 当前状态
    void SetServerInfo(int,int);
    // 服务器是否注册了,如果存在返回user_id&server_id,否则返回0
    std::tuple<int,int> ServerIsRegister();

private:
    void InitTable();

};

}

