#pragma once
#include "share/boost_define.hpp"
#include "share/network/base_session.hpp"

namespace ybs::share::network
{

struct simple_timetask
{
    typedef ybs::share::util::clock::Timestamp<ybs::share::util::clock::ms> Timestamp;
    
    
    Timestamp  trigger_stamp;   // 触发时间

    bool autoreset{false}; // 是否重置
    int reset_times{-1};    // 重置次数
    ybs::share::util::clock::ms interval{0};   // 触发间隔
};


class Server_Base
{
public:
    typedef std::function<void()>   TimeOutHandler;
    typedef int32_t SessionID;    
    Server_Base(std::string ip,int port);
    Server_Base(int listenport);
    virtual ~Server_Base();

    /**
     * @brief 开始事件循环
     */
    virtual void EventLoop();

    /**
     * @brief 停止事件循环
     */
    virtual void StopEventLoop();


    void Close_Session(int32_t session_id);

protected:
    void OnTime100ms();
    void SessionTimeoutTimer(const boost::system::error_code& e);
    
protected:

    /**
     * @brief 建立新连接时触发事件
     * 
     * @param e     状态码
     * @param sock  socket
     */
    virtual void OnConnection(const boost::system::error_code& e,boost::asio::ip::tcp::socket sock);
    
    /**
     * @brief 安全的添加一个session
     * 
     * @param session 
     */
    void Safe_AddSession(Session_Base::SPtr session);
    
    /**
     * @brief 注册监听事件
     * 
     */
    void Register_Listen();


    std::shared_ptr<boost::asio::io_context> m_context_ptr; 
private:
    boost::asio::ip::tcp::endpoint  m_server_addr;
    std::map<SessionID,Session_Base::SPtr>  m_sessionmap;   // 所有session
    int32_t     m_sessionid{10001};
    std::mutex          m_session_lock;
    std::atomic_bool    m_stop_flag;

    std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
    std::multimap<int,TimeOutHandler>    m_timeout_func;
    boost::asio::steady_timer           m_timer;

};


}