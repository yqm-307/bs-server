#include "subserver/Server.hpp"
#include "subserver/DBHelper.hpp"
using namespace SubServer;



int main()
{
    assert(nullptr != DBHelper::GetInstance());

    Server server(11451);
    server.EventLoop();
}