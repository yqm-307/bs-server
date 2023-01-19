#pragma once
#include "share/util/logger.hpp"

namespace ybs::share::util::cutil
{


std::string executeCMD(const char *cmd)
{
    char* result = new char[65535];
    char ps[1024]={0};
    FILE *ptr;
    strcpy(ps, cmd);

    if((ptr=popen(ps, "r"))!=NULL)
    {
        while(fgets(buf_ps, 1024, ptr)!=NULL)
        {
//	       可以通过这行来获取shell命令行中的每一行的输出
//	   	   printf("%s", buf_ps);
           strcat(result, buf_ps);
           if(strlen(result)>1024)
               break;
        }
        pclose(ptr);
        ptr = NULL;
    }
    else
    {
        printf("popen %s error\n", ps);
    }
}


}