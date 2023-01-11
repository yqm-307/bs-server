#include "MainServer.hpp"

using namespace MainServer;



int main()
{
    Server server(11451);
    server.EventLoop();
}