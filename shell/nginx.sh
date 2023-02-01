#!/usr/bin/bash
pwd=$1
test_config()
{
    #更新到nginx
    echo $pwd | sudo -S touch /home/yqm/Code/Git/bs-server/nginxconfmy.tmp.conf
    $(echo $pwd | sudo -S echo \"$1\">>/home/yqm/Code/Git/bs-server/nginxconf/my.tmp.conf)
    res1=$(echo $pwd | sudo -S nginx -t 2>>tmp.file)
    res2=$(cat tmp.file | grep successful)
    res3=$(rm tmp.file)
    if [ ${#res2} -eq 0 ];then
        $(echo $pwd | sudo -S mv /home/yqm/Code/Git/bs-server/nginxconf/my.tmp.conf /home/yqm/Code/Git/bs-server/nginxconf/my.conf)
        echo 1
    else
        $(echo $pwd | sudo -S rm /home/yqm/Code/Git/bs-server/nginxconf/my.tmp.conf)
        echo 0
    fi
}


nginx_reload()
{
    s=$(echo $pwd | sudo -S nginx -s reload)
    if [ ${#s} -eq 0 ];then
        echo 1
    else
        echo 0
    fi


}

if [[ $2 -eq 1 ]];then  # 测试配置文件
    test_config $3
elif [[ $2 -eq 2 ]];then
    nginx_reload
fi