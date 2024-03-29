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
    void Test_SetUserinfo(Buffer& packet);
    void Test_GetUserinfo(Buffer& packet);

    
    /**
     * @brief 账号登录，账号名密码登录
     */
    void Handler_PassportInfoLogin(Buffer& packet);
    /**
     * @brief 新用户注册
     */
    void Handler_RegisterNewPassport(Buffer& packet);

    /**
     * @brief 添加一个服务器的信息
     */
    void Handler_AddServerInfo(Buffer& packet);

    /**
     * @brief 查询服务器信息
     */
    void Handler_SearchServerInfo(Buffer& packet);
    /**
     * @brief 修改用户密码
     */
    void Handler_UpdatePassword(Buffer& packet);
    /**
     * @brief 查询用户的所有信息
     */
    void Handler_GetUserInfoList(Buffer& packet);
    /**
     * @brief 获取所有服务器基本信息的列表
     */
    void Handler_GetAllServerInfoList(Buffer& packet);
    /**
     * @brief 获取当前防火墙打开的所有端口
     */
    void Handler_GetAllPortInfoList(Buffer& packet);
    /**
     * @brief 
     */
    void Handler_GetServerIPBySid(Buffer& packet);
    /**
     * @brief nginx测试配置文件
     */
    void Handler_NginxTest(Buffer& packet);

    // 删除服务器
    void Handler_DelServer(Buffer& packet);
    // 删除用户
    void Handler_DelUser(Buffer& packet);
    // 加载nginx配置
    void Handler_ReloadNginx(Buffer& packet);
    // 是否有权限
    void Handler_HasQuanxian(Buffer& packet);
private:
    int32_t m_session_id;
    OnCloseHandler  m_closed_handler;
};

}