#pragma once
#include "share/network/base_server.hpp"

namespace SubServer
{

class Server:public ybs::share::network::Server_Base
{
public:
    Server(const std::string& ip,int port);
    Server(int port);   
    virtual void OnConnection(const boost::system::error_code& e,boost::asio::ip::tcp::socket sock) override;
    
private:
    virtual void Connect(std::string ip,int port);
    void Init();
    
    
    /**
     * @brief Client
     */
    void OnTime1s();
    void SendServerInfoToMainServer();
    

    /**
     * @brief server 需要定时向数据库写当前机器信息，并且重置超时定时器
     * 
     */
    boost::asio::steady_timer m_timer;
};

}