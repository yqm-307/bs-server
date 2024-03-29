#include "share/network/base_session.hpp"

using namespace ybs::share::network;


Session_Base::Session_Base(boost::asio::io_context& ioc,boost::asio::ip::tcp::socket&&sock,const int timeout_ms)
    :m_time_out(3000),
    m_socket(std::move(sock)),
    m_recvbuffer(new char[m_recv_size]),
    m_ioc(ioc)
{
    INFO("Session connected!");
    m_socket.async_receive(boost::asio::buffer(m_recvbuffer,m_recv_size),
    [this](
        const boost::system::error_code& error, // Result of operation.
        std::size_t bytes_transferred           // Number of bytes received.
    ){
        OnRecv(error,bytes_transferred);
    });
}

Session_Base::~Session_Base()
{
    delete[] m_recvbuffer;
    INFO("Session delete!");
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
        ERROR("can`t find this option code! opcode: %d",opcode);
        return;
    }
    DEBUG("call %d handler",opcode);
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

void Session_Base::Update()
{
    m_last_recv_time = ybs::share::util::clock::now<ybs::share::util::clock::ms>();
}


void Session_Base::Close()
{
    m_socket.close();
    INFO("Session is closed!");
}

boost::asio::io_context& Session_Base::GetIOC()
{
    return m_ioc;
}

void Session_Base::SendPacket(ybs::share::util::Buffer&& packet)
{
    if (!m_socket.is_open())
    {
        ERROR("unexpected socket disconnection! can`t send packet!");
    }
    int len = packet.DataSize() + sizeof(int);
    std::string proto_head="";
    proto_head.append((char*)&len,sizeof(int));

    std::string result = proto_head + std::string(packet.View());
    m_socket.async_send(boost::asio::buffer(result),
    [](const boost::system::error_code& e,size_t len){
        if (e.failed())
        {
            ERROR("socket async send error!");
            return;
        }
        else{
            INFO("send %u byte",len);
        }
    });

}

bool Session_Base::IsConnected()
{
    return m_socket.is_open();
}


void Session_Base::OnRecv(const boost::system::error_code& err,size_t nbytes)
{
    ybs::share::util::Buffer tmp;
    if (nbytes > m_recv_size)
        ERROR("recv buffer not engry! 机制上不可能出现");
    if (nbytes < 4 && nbytes != 0)
    {
        ERROR("recv a bad data, because of recv bytes less then 4!");
        return;
    }
    else if(nbytes == 0)
    {
        ERROR("peer socket closed!");
        Close();
        return;
    }
    else
    {
        DEBUG("recv %d byte",nbytes);
    }
    tmp.WriteString(m_recvbuffer,nbytes);    
    int opcode = tmp.ReadInt32();
    Dispatch(opcode,tmp);
    Update();

    // 重新注册

    if (IsConnected())
    {
        m_socket.async_receive(boost::asio::buffer(m_recvbuffer,m_recv_size),
        [this](
            const boost::system::error_code& error, // Result of operation.
            std::size_t bytes_transferred           // Number of bytes received.
        ){
            OnRecv(error,bytes_transferred);
        });
    }
}


void Session_Base::Handler_HeartBeat(ybs::share::util::Buffer& buffer)
{
    uint64_t tsp= buffer.ReadInt64();

}
