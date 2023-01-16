#pragma once
#include "share/network/base_session.hpp"
#include "DBHelper.hpp"

namespace MainServer
{

class Session : public ybs::share::network::Session_Base
{
    typedef ybs::share::util::Buffer Buffer;
public:
    Session(boost::asio::ip::tcp::socket&& sock);
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

};

}