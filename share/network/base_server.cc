#include "share/network/base_server.hpp"

using namespace ybs::share::network;

Server_Base::Server_Base(std::string ip,int port)
    :m_context_ptr(std::make_shared<decltype(m_context_ptr)::element_type>()),
    m_server_addr(boost::asio::ip::make_address(ip),port)
{
    Register_Listen();
    INFO("Server Base buildOver!");
}

Server_Base::Server_Base(int listenport)
    :m_context_ptr(std::make_shared<decltype(m_context_ptr)::element_type>()),
    m_server_addr(boost::asio::ip::tcp::v4(),listenport)
{
    Register_Listen();
    INFO("Server Base buildOver!");
}

Server_Base::~Server_Base()
{
    INFO("Server Base Exit Success!");
}


void Server_Base::EventLoop()
{
    m_stop_flag.store(true);
    INFO("EventLoop was started!");
    while(m_stop_flag)
        m_context_ptr->run();
    INFO("EventLoop was stopped!");
}
void Server_Base::StopEventLoop()
{
    m_stop_flag.store(false);
}

void Server_Base::OnTime100ms()
{
    // 遍历所有session是否超时
    for (auto it : m_sessionmap)
    {
        if (it.second->IS_TimeOut())
        {
            it.second->Close();
        }
    }
    

}

void Server_Base::SessionTimeoutTimer(const boost::system::error_code& e)
{
    using namespace boost::asio::chrono;
    static time_point<steady_clock,milliseconds> time_once_stamp =
            boost::asio::chrono::time_point_cast<milliseconds,steady_clock,nanoseconds>
            (steady_clock::now()); // 每次trigger更新一次时间戳

    boost::asio::steady_timer t(*m_context_ptr,boost::asio::chrono::milliseconds(100));
    std::function<void(const boost::system::error_code&)> func = 
    [this,&t,&func](const boost::system::error_code& err){
        if (err.failed())
        {
            ERROR("timer failed!");
        }
        OnTime100ms();
        time_once_stamp = time_once_stamp + boost::asio::chrono::milliseconds(100);
        t.expires_at(time_once_stamp);
        t.async_wait(func);
    };
    t.async_wait(func);

}




void Server_Base::OnConnection(const boost::system::error_code& e,boost::asio::ip::tcp::socket sock)
{
    if(e.failed())
    {
        ERROR("on connection failed!");
    }

    auto ptr = std::make_shared<Session_Base>(std::move(sock));
    
    {
        std::lock_guard<std::mutex> lock(m_session_lock);
        auto it = m_sessionmap.insert(std::make_pair(m_sessionid++,ptr));
        if (it.second == false)
        {
            ERROR("session id is repeat!");
        }
    }

}

void Server_Base::Safe_AddSession(Session_Base::SPtr session)
{
    std::lock_guard<std::mutex> lock(m_session_lock);
    auto it = m_sessionmap.insert(std::make_pair(m_sessionid++, session));
    if (it.second == false)
    {
        ERROR("session id is repeat!");
    }
}



void Server_Base::Register_Listen()
{
    m_acceptor = std::make_shared<decltype(m_acceptor)::element_type>(*m_context_ptr,m_server_addr,true);
    m_acceptor->async_accept([this](const boost::system::error_code& e,boost::asio::ip::tcp::socket sock){
        this->OnConnection(e,std::move(sock));
    });
}