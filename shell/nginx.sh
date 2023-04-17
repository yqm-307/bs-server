#!/usr/bin/bash
pwd=$1
cfgstr=$3
test_config()
{
    #更新到nginx
    echo $pwd | sudo -S echo $cfgstr>>nginxconf/my.tmp.conf
    echo $pwd | sudo -S mv /etc/nginx/conf.d/my.conf /etc/nginx/conf.d/my.conf.bf
    echo $pwd | sudo -S cp nginxconf/my.tmp.conf /etc/nginx/conf.d/my.tmp.conf
    res1=$(echo $pwd | sudo -S nginx -t 2>>tmp.file)
    res2=$(echo $pwd | sudo -S cat tmp.file | grep successful)
    $(echo $pwd | sudo -S rm tmp.file)
    res3=$(echo $res2 | wc -L)
    if [ ${#res2} -eq 0 ];then
        echo $pwd | sudo -S rm /etc/nginx/conf.d/my.tmp.conf
        echo $pwd | sudo -S mv /etc/nginx/conf.d/my.conf.bf /etc/nginx/conf.d/my.conf 
        echo 0
    else
        echo $pwd | sudo -S rm /etc/nginx/conf.d/my.conf.bf
        echo $pwd | sudo -S mv nginxconf/my.tmp.conf /etc/nginx/conf.d/my.conf
        echo 1
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
    test_config
elif [[ $2 -eq 2 ]];then
    nginx_reload
fi

# "server{listen 8990;server_name  192.168.114.1;location / { root /data;index index.html;}}"