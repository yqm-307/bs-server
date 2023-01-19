#pragma once
#include "share/network/base_session.hpp"
#include "mainserver/DBHelper.hpp"

namespace MainServer
{

class Session : public ybs::share::network::Session_Base
{
    typedef ybs::share::util::Buffer Buffer;
    typedef std::function<void(int32_t)>   OnCloseHandler;
public:
    Session(boost::asio::io_context& ioc,boost::asio::ip::tcp::socket&& sock);
    
    void SetId(int32_t id);
    int32_t GetId();
    void Register_Close(const OnCloseHandler& handler);
    void CloseSession() ;
private:
    void Test_SetUserinfo(ybs::share::util::Buffer& packet);
    void Test_GetUserinfo(ybs::share::util::Buffer& packet);

    
    /**
     * @brief 账号登录，账号名密码登录
     */
    void Handler_PassportInfoLogin(ybs::share::util::Buffer& packet);
    /**
     * @brief 新用户注册
     */
    void Handler_RegisterNewPassport(ybs::share::util::Buffer& packet);

private:
    int32_t m_session_id;
    OnCloseHandler  m_closed_handler;
};

}