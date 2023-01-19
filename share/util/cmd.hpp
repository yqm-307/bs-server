#pragma once
#include "share/util/logger.hpp"

namespace ybs::share::util::cutil
{


std::string executeCMD(const char *cmd)
{

    char buffer[4096];
    FILE *ptr;
    std::string result;
    if((ptr=popen(cmd, "r"))!=NULL)
    {
        while(fgets(buffer,4096, ptr)!=NULL)
        {
            result.append(buffer,strlen(buffer));
        }
        pclose(ptr);
    }
    else
    {
        ERROR("system call error! popen call failed!");
    }
    return result;
}


}