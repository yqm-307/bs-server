#include "share/network/base_server.hpp"

namespace MainServer
{

class Server:public ybs::share::network::Server_Base
{
public:
    Server(const std::string& ip,int port);
    Server(int port);   
private:

};

}