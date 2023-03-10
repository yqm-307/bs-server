#pragma once
#include "share/util/config.hpp"

namespace MainServer
{

class ConfigManager: ybs::share::util::JsonReader
{
public:
    ConfigManager()
        :JsonReader("mainconfig.json")
    {
    }
    ~ConfigManager()=default;

    static ConfigManager* GetInstance()
    {
        static ConfigManager* ptr = nullptr;
        if (ptr == nullptr)
        {
            ptr = new ConfigManager();
        }
        return ptr;
    }
    std::string GetDBIP()
    { return Json()["db"]["ip"].asString(); }

    std::string GetDBUsername()
    { return Json()["db"]["username"].asString(); }

    std::string GetDBPassword()
    { return Json()["db"]["password"].asString(); }
    
    std::string GetDBname()
    { return Json()["db"]["dbname"].asString(); }



};
}