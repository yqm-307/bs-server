#pragma once
#include "share/network/base_session.hpp"
#include "DBHelper.hpp"

namespace MainServer
{

class Session : public ybs::share::network::Session_Base
{
public:
    Session(boost::asio::ip::tcp::socket&& sock);
private:
    void Test_SetUserinfo(ybs::share::util::Buffer& packet);
    void Test_GetUserinfo(ybs::share::util::Buffer& packet);
};

}