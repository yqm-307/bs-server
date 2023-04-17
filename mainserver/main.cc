#include "mainserver/MainServer.hpp"
#include "mainserver/DBHelper.hpp"
#include <bbt/config/GlobalConfig.hpp>
using namespace MainServer;
using namespace bbt::config;


int main()
{
    int open = 1;
    assert(GlobalConfig::GetInstance()->GetDynamicCfg()->SetEntry<int>("BBT_LOG_STDOUT_OPEN",&open));
    printf("%d\n",*(GlobalConfig::GetInstance()->GetDynamicCfg()->GetEntry<int>("BBT_LOG_STDOUT_OPEN")));
    assert(nullptr != DBHelper::GetInstance());

    Server server(11451);
    server.EventLoop();
}