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
            Y_SESSION_HANDLER(2001,Handler_PassportInfoLogin),      // 登录
            Y_SESSION_HANDLER(2002,Handler_RegisterNewPassport),    // 注册
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


void Session::Handler_PassportInfoLogin(ybs::share::util::Buffer& packet)
{
    
    int passport = packet.ReadInt32();
    auto password = packet.ReadCString();

    // 数据库查询结果
    auto res = DBHelper::GetInstance()->User_GetUserInfo(passport);
    if (res.size() == 0)
    {
        ERROR("sql failed!");
        return;
    }

    auto p = res[0];

    int pp = std::get<0>(p);
    std::string pwd= std::get<1>(p);
    DEBUG("%d %s",pp,pwd);
    // 验证
    ybs::share::util::Buffer buff;

    if ( (pp == passport) && (pwd == password) )
        buff.WriteInt32(1);
    else
        buff.WriteInt32(0);

    this->SendPacket(std::move(buff));
}


void Session::Handler_RegisterNewPassport(ybs::share::util::Buffer& packet)
{
    /**
     * packet:
     * {
     *      int,        账号
     *      cstring,    密码
     * }
     */

    int passport = packet.ReadInt32();
    auto password = packet.ReadCString();
    DEBUG("%d %s",passport,password.c_str());

    Buffer sendpck;

    if (DBHelper::GetInstance()->User_SetUserInfo(passport,password))
    {
        sendpck.WriteInt32(1);
    }   
    else
    {
        sendpck.WriteInt32(0);
    }
    SendPacket(std::move(sendpck));

}
