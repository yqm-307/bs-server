#include "subserver/Session.hpp"
#include "share/util/cmd.hpp"

using namespace SubServer;

#define Y_SESSION_HANDLER(id, handler)                                \
    {                                                                 \
        id,                                                           \
            [this](ybs::share::util::Buffer &f) { this->handler(f); } \
    }

#define fmt(args, ...) (ybs::share::util::format(args, ##__VA_ARGS__).c_str())

Session::Session(boost::asio::io_context &ioc, boost::asio::ip::tcp::socket &&sock)
    : Session_Base(ioc, std::move(sock)),
      m_timer(GetIOC(), boost::asio::chrono::milliseconds(1000))
{
    InitHandler(
        {
            Y_SESSION_HANDLER(4001, Handler_Client_Ping_SubServer),
            Y_SESSION_HANDLER(4002, Handler_ufw_portinfo),
            Y_SESSION_HANDLER(4003, Handler_ufw_close),
            Y_SESSION_HANDLER(4004, Handler_ufw_open),
            Y_SESSION_HANDLER(4005, Handler_addport),
            Y_SESSION_HANDLER(4006, Handler_delport),
        });
}

void Session::SetId(int32_t id)
{
    m_session_id = id;
}
int32_t Session::GetId()
{
    return m_session_id;
}

void Session::Register_Close(const OnCloseHandler &handler)
{
    m_closed_handler = handler;
}

void Session::CloseSession()
{
    Close();
    if (m_closed_handler)
        m_closed_handler(m_session_id);
}

/********************************************
 ********************************************
 ********************************************
 ********************************************
 ********************************************
 */

void Session::Handler_Timer_SendToMain_ServerInfo()
{
    ybs::share::util::Buffer pck;

    auto str = ybs::share::util::cutil::executeCMD("uname -a");
    DEBUG("%s", str.c_str());
}

void Session::Handler_Client_Ping_SubServer(ybs::share::util::Buffer &buf)
{
    ybs::share::util::Buffer pck;
    pck.WriteInt32(1); // 收到这个包，直接回复1 就行
    SendPacket(std::move(pck));
}

void Session::Handler_ufw_portinfo(Buffer &buffer)
{
    Buffer pck;
    int uid = buffer.ReadInt32();

    do
    {
        auto pwd = DBHelper::GetInstance()->Server_GetRootpwd(uid);
        if (pwd.size() == 0)
        {
            pck.WriteInt32(1);
            break;
        }
        auto port = ybs::share::util::cutil::executeCMD(fmt("./shell/ufw.sh 4 %s", std::get<0>(pwd[0]).c_str()));
        pck.WriteInt32(2);
        pck.WriteString(port);
        DEBUG("%s", port.c_str());
    } while (0);

    SendPacket(std::move(pck));
}

void Session::Handler_ufw_close(Buffer &buffer)
{
    Buffer pck;
    int uid = buffer.ReadInt32();
    do
    {
        auto pwd = DBHelper::GetInstance()->Server_GetRootpwd(uid);
        if (pwd.size() == 0)
        {
            pck.WriteInt32(1);
            break;
        }
        auto port = ybs::share::util::cutil::executeCMD(fmt("./shell/ufw.sh 2 %s", std::get<0>(pwd[0]).c_str()));
        pck.WriteInt32(2);
        DEBUG("%s", port.c_str());
    } while (0);

    SendPacket(std::move(pck));
}

void Session::Handler_ufw_open(Buffer &buffer)
{
    Buffer pck;
    int uid = buffer.ReadInt32();
    auto port = ybs::share::util::cutil::executeCMD("./shell/ufw.sh 3");
    DEBUG("%s", port.c_str());
    pck.WriteInt32(1);
    SendPacket(std::move(pck));
}

void Session::Handler_addport(Buffer &buffer)
{
    Buffer pck;
    int uid = buffer.ReadInt32();
    int port = buffer.ReadInt32();

    do
    {
        auto pwd = DBHelper::GetInstance()->Server_GetRootpwd(uid);
        if (pwd.size() == 0)
        {
            pck.WriteInt32(1);
        }
        auto res = ybs::share::util::cutil::executeCMD(fmt("./shell/ufw.sh 5 %s %d", std::get<0>(pwd[0]).c_str(),port));

        DEBUG("%s", res.c_str());
        pck.WriteInt32(2);

    } while (0);
    SendPacket(std::move(pck));
}
void Session::Handler_delport(Buffer &buffer)
{
    Buffer pck;
    int uid = buffer.ReadInt32();
    int port = buffer.ReadInt32();
        do
    {
        auto pwd = DBHelper::GetInstance()->Server_GetRootpwd(uid);
        if (pwd.size() == 0)
        {
            pck.WriteInt32(1);
        }
        auto res = ybs::share::util::cutil::executeCMD(fmt("./shell/ufw.sh 6 %s %d",std::get<0>(pwd[0]).c_str(), port));

        DEBUG("%s", res.c_str());
        pck.WriteInt32(2);
    } while (0);
    SendPacket(std::move(pck));
}