#pragma once
#include "share/mysql/DBHelper_Base.hpp"

namespace SubServer
{

class DBHelper:public ybs::share::mysql::DBHelper_Base
{
public:
    DBHelper();
    ~DBHelper(){}
    static DBHelper* GetInstance();


public:
    // 向数据库中写入 server 当前状态
    void SetServerInfo();

private:
    void InitTable();

};

}

