#include "MainServer.hpp"
#include "DBHelper.hpp"
using namespace MainServer;



int main()
{
    assert(nullptr != DBHelper::GetInstance());

    Server server(11451);
    server.EventLoop();
}