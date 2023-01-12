#include "MainSession.hpp"


using namespace MainServer;

#define Y_SESSION_HANDLER(id,handler) {\
    id,\
    [this](ybs::share::util::Buffer&f){this->handler(f);}\
}


Session::Session(boost::asio::ip::tcp::socket&& sock)
    :Session_Base(std::move(sock))
{
    InitHandler(
    {
        Y_SESSION_HANDLER(1001,Test_SetUserinfo),
        Y_SESSION_HANDLER(1002,Test_GetUserinfo),
    }
    );
}



void Session::Test_SetUserinfo(ybs::share::util::Buffer &packet)
{
    int userid = packet.ReadInt32();
    std::string name = packet.ReadCString();
    int age = packet.ReadInt32();

    // 存入数据库
    DBHelper::GetInstance()->Test_SetUserInfo(userid,name,age);
}

void Session::Test_GetUserinfo(ybs::share::util::Buffer &packet)
{
    
}
