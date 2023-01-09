#pragma once
#include <share/boost_define.hpp>
#include "share/util/logger.hpp"
#include "share/util/buffer.hpp"
#include "share/util/clock.hpp"
#include "share/util/classinfo.hpp"
namespace ybs::share::network
{


class Session_Base : public ybs::share::util::ClassInfo<Session_Base>
{
    typedef std::function<void(ybs::share::util::Buffer&)> Protocol_Handler;
public:
    Session_Base(boost::asio::ip::tcp::socket&& socket,const int timeout_ms=3000);
    ~Session_Base()=default;
    
    /**
     * @brief 将已经去除协议头的内容部分的比特流传递给对应
     *  的协议处理程序
     * 
     * @param opcode 选项id
     * @param buf 协议比特流
     */
    void Dispatch(int opcode,ybs::share::util::Buffer& buf);
    /**
     * @brief 判断Session是否超时
     * 
     * @return true 已经超时
     * @return false 没有超时
     */
    bool IS_TimeOut();

    /**
     * @brief 主动断开与对端的连接
     */
    void Close();


    /**
     * @brief 发送一条消息，比特流
     * 
     * @param packet 协议的比特流
     */
    void SendPacket(ybs::share::util::Buffer&& packet);
    
protected:
    /**
     * @brief 通过初始化列表初始化func handler
     *  例如:
     *  InitHandler({ 
     *      {1,[](ybs::share::util::Buffer&){ ... }}, 
     *      {2,[](ybs::share::util::Buffer&){ ... }}, 
     *  });
     * @param list 初始化列表
     */
    void InitHandler(std::initializer_list<std::pair<int32_t,Protocol_Handler>> list);

    
private:
    std::unordered_map<int32_t,Protocol_Handler>    
        m_protocol_handlers;
    ybs::share::util::clock::Timestamp<ybs::share::util::clock::ms>
        m_last_recv_time;   // 最后接受数据的时间
    ybs::share::util::clock::ms 
        m_time_out;         // 超时时间 
    boost::asio::ip::tcp::socket    
        m_socket;
};

// void func()

// {
//     Session_Base base;
//     base.InitHandler({
//         {1,[](uint32_t,ybs::share::util::Buffer&){}},
//         {2,[](uint32_t,ybs::share::util::Buffer&){}},
//         {3,[](uint32_t,ybs::share::util::Buffer&){}},
//     });
// }



}