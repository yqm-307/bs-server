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
            Y_SESSION_HANDLER(3002,Handler_AddServerInfo),          // 添加服务器
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

    Buffer pck;
    // 读数据库,检测是否有id冲突问题
    switch (DBHelper::GetInstance()->Server_UIDANDSID_Is_Repeat(user_id,serverid))
    {
    case 1:
        pck.WriteInt32(1);
        pck.WriteString("服务器错误:sql查询失败");
        break;
    case 2:
        pck.WriteInt32(2);
        pck.WriteString("user id错误, 该user不存在");
        break;
        
    case 3:
        pck.WriteInt32(1);
        pck.WriteString("server id重复,请更换server id");
        break;
            
    default:
        pck.WriteInt32(0);
        if (!DBHelper::GetInstance()->Server_AddNewServerInfo(
            user_id,
            serverid,
            server_ip,
            server_port,
            server_level,
            flag_firewall,
            linux_username,
            linux_pwd
        ))
        {
            pck.WriteString("插入失败,请查看服务器日志");
        }
        else
            pck.WriteString("插入成功");
        
    }

    SendPacket(std::move(pck));
}

