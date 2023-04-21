#!/usr/bin/bash
pwd=$1
cfgstr=$3
test_config()
{
    #更新到nginx
    echo $pwd | sudo -S echo $cfgstr>>nginxconf/my.tmp.conf                         # 将客户端内容输出到my.tmp.conf中
    echo $pwd | sudo -S mv /etc/nginx/conf.d/my.conf /etc/nginx/conf.d/my.conf.bf   # 备份/etc/nginx/conf.d/my.conf
    echo $pwd | sudo -S cp nginxconf/my.tmp.conf /etc/nginx/conf.d/my.tmp.conf      # 将客户端的配置文件拷贝到/etc/nginx/cond.f下
    res1=$(echo $pwd | sudo -S nginx -t 2>>tmp.file)                                # 获取客户端配置的测试结果
    res2=$(echo $pwd | sudo -S cat tmp.file | grep successful)                      # 获取成功标识
    res3=$(echo $res2 | wc -L)
    if [ $res3 -eq 0 ];then
        # 如果不成功
        echo $pwd | sudo -S rm /etc/nginx/conf.d/my.tmp.conf
        echo $pwd | sudo -S mv /etc/nginx/conf.d/my.conf.bf /etc/nginx/conf.d/my.conf # 恢复nginx配置
        echo 0
    else
        echo $pwd | sudo -S rm /etc/nginx/conf.d/my.conf.bf
        echo $pwd | sudo -S rm /etc/nginx/conf.d/my.tmp.conf
        echo $pwd | sudo -S cp nginxconf/my.tmp.conf /etc/nginx/conf.d/my.conf      # 使用新配置
        echo 1
    fi
    echo $pwd | sudo -S rm nginxconf/my.tmp.conf # 删除客户端配置
    echo $pwd | sudo -S rm tmp.file # 删除中间文件
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

if [[ $2 -eq 1 ]];then  # 测试并应用配置文件
    test_config
elif [[ $2 -eq 2 ]];then
    nginx_reload
fi

# "server{listen 8990;server_name  192.168.114.1;location / { root /data;index index.html;}}"