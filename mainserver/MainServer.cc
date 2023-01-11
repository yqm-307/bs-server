#include "MainServer.hpp"

using namespace MainServer;

Server::Server(const std::string& ip,int port)
    :Server_Base(ip,port)
{
}

Server::Server(int port)
    :Server_Base(port)
{

}