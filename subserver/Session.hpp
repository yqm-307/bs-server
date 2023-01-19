#pragma once
#include "share/network/base_session.hpp"
#include "subserver/DBHelper.hpp"

namespace SubServer
{

class Session : public ybs::share::network::Session_Base
{
    typedef ybs::share::util::Buffer Buffer;
    typedef std::function<void(int32_t)>   OnCloseHandler;
public:
    Session(boost::asio::io_context& ioc,boost::asio::ip::tcp::socket&& sock);
    ~Session()=default;
    void SetId(int32_t id);
    int32_t GetId();
    void Register_Close(const OnCloseHandler& handler);
    void CloseSession();
private:
    /**
     * @brief 向 mainserver 定时发送服务器基本信息
     * 
     */
    void Handler_Timer_SendToMain_ServerInfo(); 

private:

private:
    int32_t                     m_session_id;
    OnCloseHandler              m_closed_handler;
    boost::asio::steady_timer   m_timer;
};

}