#include "subserver/Server.hpp"
#include "subserver/Session.hpp"
using namespace SubServer;

Server::Server(const std::string& ip,int port)
    :Server_Base(ip,port)
{
}

Server::Server(int port)
    :Server_Base(port)
{

}


void Server::OnConnection(const boost::system::error_code& e,boost::asio::ip::tcp::socket sock)
{
    if(e.failed())
    {
        ERROR("on connection failed!");
    }

    auto ptr = std::make_shared<Session>(*m_context_ptr,std::move(sock));
    ptr->Register_Close([this](int32_t id){
        this->Close_Session(id);
    });
    Safe_AddSession(ptr);
}
