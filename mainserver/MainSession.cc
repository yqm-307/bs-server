#include "mainserver/MainSession.hpp"
#include "share/util/cmd.hpp"

using namespace MainServer;

#define Y_SESSION_HANDLER(id,handler) {\
    id,\
    [this](ybs::share::util::Buffer&f){this->handler(f);}\
}
#define fmt(args,...) (ybs::share::util::format(args,##__VA_ARGS__).c_str())


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
            Y_SESSION_HANDLER(3003,Handler_SearchServerInfo),       // 查询服务器信息
            Y_SESSION_HANDLER(3004,Handler_UpdatePassword),         // 修改密码
            Y_SESSION_HANDLER(3005,Handler_GetUserInfoList),        // 获取用户信息列表
            Y_SESSION_HANDLER(3006,Handler_GetAllServerInfoList),   // 获取所有服务器信息
            Y_SESSION_HANDLER(3007,Handler_GetServerIPBySid),       // 通过 id 获取 ip
            Y_SESSION_HANDLER(3008,Handler_NginxTest),              // nginx config test
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
     *      uid,        当前用户的id
     *      int,        账号
     *      cstring,    密码
     *      int         权限
     * }
     */
    int uid = packet.ReadInt32();
    int passport = packet.ReadInt32();
    auto password = packet.ReadCString();
    int level = packet.ReadInt32();

    DEBUG("%d %s",passport,password.c_str());

    Buffer sendpck;
    sendpck.WriteInt32(DBHelper::GetInstance()->User_SetUserInfo(uid,passport,password,level));   // 返回注册结果
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



void Session::Handler_SearchServerInfo(Buffer& packet)
{
    int user_id = packet.ReadInt32();
    int server_id = packet.ReadInt32();
    Buffer pck;
    do{
        auto arr_result = DBHelper::GetInstance()->Server_GetServerInfo(user_id,server_id);
        
        if (arr_result.size() == 0)
        {
            pck.WriteInt32(1);  // 查不到结果
            break;
        }
        // 是否超时
        if ( (uint64_t)(time(NULL)) - std::get<1>(arr_result[0]) > 3)
        {
            pck.WriteInt32(2);  // 服务器已经断开
        }
        // 成功获取有效服务器信息
        pck.WriteInt32(0);
        pck.WriteString(std::get<0>(arr_result[0]));

    }while(0);

    SendPacket(std::move(pck));
    
}


void Session::Handler_UpdatePassword(Buffer& packet)
{
    enum Err : int
    {
        OK = 1,
        Pwd_not_need_update = 2,
        Uid_not_found = 3,
        Sql_failed = 4,

    };
    int uid = packet.ReadInt32();
    auto newpassword = packet.ReadCString();

    Buffer pck;
    do{
        // 查询旧密码
        auto result_vec = DBHelper::GetInstance()->User_GetUserInfoByUid_v1(uid);
        if ( newpassword == std::get<2>(result_vec[0]))
        {
            pck.WriteInt32(Pwd_not_need_update);
            break;
        }
        if (result_vec.size() == 0)
        {
            pck.WriteInt32(Uid_not_found);
            break;
        }
        auto res = DBHelper::GetInstance()->User_SetNewPassword(uid,newpassword,std::get<3>(result_vec[0]));
        if (res == 1)
        {
            pck.WriteInt32(Sql_failed);
            break;
        }
        pck.WriteInt32(OK);

    }while(0);

    SendPacket(std::move(pck));
}


void Session::Handler_GetUserInfoList(Buffer& packet)
{
    /**
     * uid
     * pwd
     * level
     */
    int uid = packet.ReadInt32();
    Buffer pck;
    do{
        auto result_vec = DBHelper::GetInstance()->User_GetUserInfoByUid_v1(uid);
        if (result_vec.size() == 0)
        {
            pck.WriteInt32(2);
        }
        pck.WriteInt32(1);
        pck.WriteInt32(std::get<0>(result_vec[0])); // uid
        pck.WriteInt32(std::get<1>(result_vec[0])); // passport
        pck.WriteInt32(std::get<3>(result_vec[0])); // level
    }while(0);

    SendPacket(std::move(pck));
}


void Session::Handler_GetAllServerInfoList(Buffer& packet)
{
    // 获取所有服务器的 id | ip | 状态 |权限
    auto rlt_vec = DBHelper::GetInstance()->Server_GetAllServerInfo();

    std::string str;
    for (auto&& line : rlt_vec)
    {
        str += std::to_string(std::get<0>(line))+'|';
        str += std::get<1>(line)+'|';
        if ((uint64_t)(time(NULL)) - std::get<2>(line) > 3)
            str += "离线|";
        else
            str += "在线|";
        str += std::to_string(std::get<3>(line)) + "|\n";
    }
    Buffer buf(str);

    SendPacket(std::move(buf));
}


void Session::Handler_GetAllPortInfoList(Buffer& packet)
{
    // select port info

}

void Session::Handler_GetServerIPBySid(Buffer& packet)
{
    Buffer pck;
    int sid = packet.ReadInt32();
    do{
        auto res_vec = DBHelper::GetInstance()->Server_GetAllServerInfo();
        for (auto&& line:res_vec)
        {
            if (std::get<0>(line) == sid)
            {
                pck.WriteInt32(1);
                pck.WriteString(std::get<1>(line));
                SendPacket(std::move(pck));
                return;
            }
        }
        pck.WriteInt32(2);  // 找不到serverid没有结果
    }while(0);

    SendPacket(std::move(pck));
}

void Session::Handler_NginxTest(Buffer& packet)
{
    
    Buffer pck;
    int sid = packet.ReadInt32();
    std::string config= packet.ReadCString();
    do{
        auto res_vec = DBHelper::GetInstance()->Server_GetAllServerInfo();
        auto res = ybs::share::util::cutil::executeCMD(fmt("./shell/nginx.sh 200101 1 \"%s\"",config.c_str()));
        pck.WriteInt32(std::stoi(res));  // 找不到serverid没有结果
    }while(0);

    SendPacket(std::move(pck));
}
