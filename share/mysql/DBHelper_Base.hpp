#pragma once
#include "share/mysql/MySql.hpp"
#include "share/mysql/MySqlException.hpp"
#include "share/util/logger.hpp"

namespace ybs::share::mysql
{

class DBHelper_Base
{
public:
    /**
     * @brief Construct a new dbhelper base object
     * 
     * @param ip        ip地址
     * @param username  用户名
     * @param password  密码
     * @param dbname    db名
     */
    DBHelper_Base(
        const std::string& ip,
        const std::string& username,
        const std::string& password,
        const std::string& dbname)
        :m_ip(ip),
        m_username(username),
        m_password(password),
        m_dbname(dbname)
    {
        try
        {
            m_sqlconn = new MySql(
                m_ip.c_str(),
                m_username.c_str(),
                m_password.c_str(),
                m_dbname.c_str());
        }
        catch(MySqlException& e)
        {
            ERROR("Mysql InitError! msg: %s",e.what());
        }
    }
    virtual ~DBHelper_Base()
    {
        delete m_sqlconn;
    }
    
    template<typename... Args>
    bool runQuery(
        std::vector<std::tuple<Args...>>* const result,
        const char* const query
    ) const
    {
        try
        {
            this->m_sqlconn->runQuery(result,query);
        }
        catch(const std::exception& e)
        {
            ERROR("%s",e.what());
            return false;
        }
        return true;
    }

    template<typename... Args>
    bool runCommand(const char* const query) const
    {
        try{
            this->m_sqlconn->runCommand(query);
        }
        catch(const std::exception& e)
        {
            ERROR("%s",e.what());
            return false;
        }
        return true;
    }



protected:


    std::string m_ip;
    std::string m_username;
    std::string m_password;
    std::string m_dbname;
    int         m_port{3306};
private:
    MySql*      m_sqlconn;
};

}