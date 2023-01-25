#include "subserver/Server.hpp"
#include "subserver/Session.hpp"
#include "share/util/cmd.hpp"
using namespace SubServer;

Server::Server(const std::string& ip,int port)
    :Server_Base(ip,port),
    m_timer(*m_context_ptr)
{
    Init();
}

Server::Server(int port)
    :Server_Base(port),
    m_timer(*m_context_ptr)
{
    Init();
}


void Server::Init()
{
    // 超时事件
    static std::function<void(const boost::system::error_code&)> func = 
    [this](const boost::system::error_code& e){
        OnTime1s();
        m_timer.cancel();
        m_timer.expires_after(boost::asio::chrono::seconds(1));
        m_timer.async_wait(func);
    };
    m_timer.expires_after(boost::asio::chrono::seconds(1));
    m_timer.async_wait(func);
}

void Server::OnTime1s()
{
    DEBUG("Trigger Once!");
    // 定时向数据库写入基础信息
    this->SendServerInfoToMainServer();

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


void Server::Connect(std::string ip,int port)
{
    boost::asio::ip::tcp::socket socket(*m_context_ptr);
    try
    {
        /* code */
        socket.connect(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(ip),
            port));
    }
    catch(const std::exception& e)
    {
        ERROR("%s",e.what());
        return;
    }

    auto ptr = std::make_shared<Session>(*m_context_ptr,std::move(socket));
    ptr->Register_Close([this](int32_t id){
        this->Close_Session(id);
    });
    Safe_AddSession(ptr);
    
}


void Server::SendServerInfoToMainServer()
{
    do
    {
        auto&& [user_id,server_id] = DBHelper::GetInstance()->ServerIsRegister();  
        // 当前服务器是否注册在数据库中
        if ( user_id == 0 && server_id == 0 )
        {
            DEBUG("当前服务器没有注册");
            break;
        }

        // 去数据库写
        DBHelper::GetInstance()->SetServerInfo(user_id,server_id);

    } while (0);
}

void Server::LoginAndReconnect()
{

}

