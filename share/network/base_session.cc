#include "share/network/base_session.hpp"

using namespace ybs::share::network;


Session_Base::Session_Base(boost::asio::ip::tcp::socket&&sock,const int timeout_ms)
    :m_time_out(3000),
    m_socket(std::move(sock))
{
    INFO("Session build");
}


void Session_Base::InitHandler(std::initializer_list<std::pair<int32_t,Protocol_Handler>> list)
{
    for (auto pi : list)
    {
        assert(m_protocol_handlers.find(pi.first) == m_protocol_handlers.end());
        m_protocol_handlers.insert(pi);
    }
}

void Session_Base::Dispatch(int opcode,ybs::share::util::Buffer& buf)
{
    auto it = m_protocol_handlers.find(opcode);
    if (it == m_protocol_handlers.end())
    {
        ERROR("can`t find this option code!");
        return;
    }
    it->second(buf);
}


bool Session_Base::IS_TimeOut()
{
    if ( ybs::share::util::clock::expired<ybs::share::util::clock::ms>
        (m_last_recv_time+m_time_out) )
    {
        return true;
    }
    return false;
}


void Session_Base::Close()
{
    m_socket.close();
    INFO("Session is closed!");
}


void Session_Base::SendPacket(ybs::share::util::Buffer&& packet)
{
    if (!m_socket.is_open())
    {
        ERROR("unexpected socket disconnection! can`t send packet!");
    }
    // m_socket.async_send();
}
