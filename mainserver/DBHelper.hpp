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

};

}