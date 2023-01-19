#include "subserver/Session.hpp"


using namespace SubServer;

#define Y_SESSION_HANDLER(id,handler) {\
    id,\
    [this](ybs::share::util::Buffer&f){this->handler(f);}\
}


Session::Session(boost::asio::io_context& ioc,boost::asio::ip::tcp::socket&& sock)
    :Session_Base(ioc,std::move(sock)),
    m_timer(GetIOC(),boost::asio::chrono::milliseconds(1000))
{
    InitHandler(
        {   
            
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










/********************************************
 ********************************************
 ********************************************
 ********************************************
 ********************************************
*/

void Session::Handler_Timer_SendToMain_ServerInfo()
{
}
