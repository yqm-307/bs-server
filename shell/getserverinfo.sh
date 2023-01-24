#!/usr/bin/bash

# linux 系统信息
echo '#!' `uname -srm`
# linux cpu 信息
echo '#!' `cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c`
# linux cpu 使用率
echo '#!' `top -b -n1 | fgrep "Cpu(s)" | tail -1 | awk -F'id,'  \
    '{\
        split($1, vs, ",");\
        v=vs[length(vs)];\
        sub(/\s+/, "", v);\
        sub(/\s+/, "", v);\ 
        printf "CPU used   is %s", 100-v; \
    }'` '%'
# linux 内存使用率
echo '#!' `free -m | sed -n '2p' | awk '{ print "Mem total is: "$2" MB"}'`

echo     `free -m | sed -n '2p' | awk '{ print "Mem used  is: "$3/$2%10*100"%"}'`

# linux system time
echo '#!' `date`
# ip 地址
echo '#!' `ifconfig | sed -n '2p' | awk '{ print "IP: "$2"" }'`

