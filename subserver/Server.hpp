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

};

}