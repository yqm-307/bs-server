#include "mainserver/MainSession.hpp"


using namespace MainServer;

#define Y_SESSION_HANDLER(id,handler) {\
    id,\
    [this](ybs::share::util::Buffer&f){this->handler(f);}\
}


Session::Session(boost::asio::io_context&ioc,boost::asio::ip::tcp::socket&& sock)
    :Session_Base(ioc,std::move(sock))
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



void Session::SetId(int32_t id)
{
    m_session_id = id;
}
int32_t Session::GetId()
{
    return m_session_id;
}

void Session::Register_Close(const OnCloseHandler& handler)
{
    m_closed_handler = handler;
}

void Session::CloseSession()
{
    Close();
    if (m_closed_handler)
        m_closed_handler(m_session_id);
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

    int user_id = std::get<0>(p);
    int pp = std::get<1>(p);
    std::string pwd= std::get<2>(p);
    // 验证
    ybs::share::util::Buffer buff;

    if ( (pp == passport) && (pwd == password) )
        buff.WriteInt32(1);
    else
        buff.WriteInt32(0);

    buff.WriteInt32(user_id);
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
    sendpck.WriteInt32(DBHelper::GetInstance()->User_SetUserInfo(passport,password));   // 返回注册结果
    SendPacket(std::move(sendpck));

}


void Session::Handler_AddServerInfo(ybs::share::util::Buffer& packet)
{
    // 解包
    int user_id = packet.ReadInt32();
    int serverid = packet.ReadInt32();
    std::string server_ip = packet.ReadCString();
    int server_port = packet.ReadInt32();
    int server_level = packet.ReadInt32();
    bool flag_firewall = bool(packet.ReadInt32());
    std::string linux_username = packet.ReadCString();
    std::string linux_pwd = packet.ReadCString();

    // 读数据库,检测是否有id冲突问题
    
    


}

