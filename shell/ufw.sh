#!/usr/bin/bash
pwd=$2

get_ufw_open_status()
{
    local res=$(echo $pwd | sudo -S ufw status | grep inactive)
    if [ -z "$res" ];
    then 
        echo '1'
    else 
        echo '0'
    fi
}

close_ufw()
{
    local res=$(echo $pwd | sudo -S ufw disable)
    echo $res
}

open_ufw()
{
    local res=$(echo $pwd | sudo -S ufw -f enable)
    echo $res
}

get_ufw_open_port()
{
    local res=$(echo $pwd | sudo -S ufw status | grep 'ALLOW' | grep -v 'v6\|tcp\|udp' | awk {'print $1"|"$2'})
    echo $res
}

ufw_open_port()
{
    local res=$(echo $pwd | sudo -S ufw allow $1)
}
ufw_close_port()
{
    local res=$(echo $pwd | sudo -S ufw deny $1)
}


if [ $1 -eq 1 ];     # 是否打开
then
    get_ufw_open_status 
elif [ $1 -eq 2 ];   # 关闭防火墙
then
    close_ufw
elif [ $1 -eq 3 ];   # 打开防火墙
then
    open_ufw    
elif [ $1 -eq 4 ];   # 获取打开的端口
then
    get_ufw_open_port
elif [ $1 -eq 5 ];   # 打开端口
then
    ufw_open_port $3
elif [ $1 -eq 6 ];   # 关闭端口
then
    ufw_close_port $3
fi


