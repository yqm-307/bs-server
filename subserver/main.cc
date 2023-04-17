#include "subserver/Server.hpp"
#include "subserver/DBHelper.hpp"
#include <bbt/config/GlobalConfig.hpp>
using namespace SubServer;
using namespace bbt::config;



int main()
{
    int open = 1;
    GlobalConfig::GetInstance()->GetDynamicCfg()->SetEntry<int>("BBT_LOG_STDOUT_OPEN",&open);
    assert(nullptr != DBHelper::GetInstance());

    Server server(10086);
    server.EventLoop();
}