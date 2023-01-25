#!/bin/bash
time_day=`date  +%Y-%m-%d_%H:%M:%S`
time_cront=`date  +%Y%m%d%H%M%S`
time_cront_day=`date  +%Y%m%d`
time_file=`date +%Y-%m-%d`
system_hostname=$(hostname | awk '{print $1}')
 
#获取服务器IP
system_ip=$(hostname -I| awk '{print $1}')

#服务器版本
system_info=$(uname -srm)

#CPU信息
cpu_info=$(cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c)
cpu_core_count=$(echo $cpu_info | awk '{ print $1 }')
cpu_cs=$(echo $cpu_info | awk '{ print $2 }')
cpu_xh=$(echo $cpu_info | awk '{ print $4 }')
cpu_core_pl=$(echo $cpu_info | awk '{ print $7 }')



#CPU使用率
cpu_used=$(top -b -n1 | fgrep "Cpu(s)" | tail -1 | awk -F'id,'  \
    '{\
        split($1, vs, ",");\
        v=vs[length(vs)];\
        sub(/\s+/, "", v);\
        sub(/\s+/, "", v);\ 
        printf "%s%%", 100-v; \
    }')

# echo '#!' `free -m | sed -n '2p' | awk '{ print "Mem total is: "$2" MB"}'`

# echo     `free -m | sed -n '2p' | awk '{ print "Mem used  is: "$3/$2%10*100"%"}'`

#获取总内存
mem_total=$(free -m | sed -n '2p'|  awk '{ print $2 }')
 
#获取剩余内存
mem_free=$(free -m | sed -n '2p' | awk '{ print $4 }')
 
#获取已用内存
mem_use=$(free -m | sed -n '2p' | awk '{ print $3 }')
 
#系统进程数
load_1=`ps -ef |wc -l`
 
#僵尸进程数
load_5=`top -b -n 1 |grep Tasks |awk '{print $10}'`
 
#CPU空闲id
load_15=`top -b -n 1 |grep Cpu |awk -F',' '{print $4}' |awk -F'id' '{print $1}'`
 
#过滤磁盘使用率大于60%目录，并加入描述
#disk_1=$(df -Ph | awk '{if(+$5>15) print "分区:"$6,"总空间:"$2,"使用空间:"$3,"剩余空间:"$4,"磁盘使用率:"$5}')
disk_most=$(df -P | awk '{if(+$5>0) print $2,$6,$1}'|grep -v ":/"|sort -nr|awk '{print $2}'|head -1)
disk_f=$(df -Ph|grep "$disk_most"$ | awk '{if(+$5>0) print $6}')
disk_total=$(df -Ph|grep "$disk_most"$ | awk '{if(+$5>0) print $2}')
disk_free=$(df -Ph|grep "$disk_most"$ | awk '{if(+$5>0) print $4}')
disk_per=$(df -Ph|grep "$disk_most"$ | awk '{if(+$5>0) print $5}')
 
disk_ux=$(df -P | awk '{if(+$5>0) print $5,$1}'|grep -v ":/"|sort -nr|awk -F"%" '{print $1}'|head -1)
if [[ $disk_ux -gt 60 ]]
     then
	#分区
	disk_f_60=$(df -P | awk '{if(+$5>0) print $5,$1,$6}'|grep -v ":/"|sort -nr|awk  '{print $3}'|head -1)
	#磁盘使用率
	disk_per_60=$(df -P | awk '{if(+$5>0) print $5,$1,$6}'|grep -v ":/"|sort -nr|awk  '{print $1}'|head -1)
	disk_status=不正常
     else
	#分区
        disk_f_60=无
        #磁盘使用率
        disk_per_60=无
	disk_status=正常
     fi
#文件路径
CHECK_HOME="$(cd "`dirname "$0"`"; pwd)"

 
#内存阈值
mem_mo='70'
 PERCENT=$(free -m | sed -n '2p' | awk '{ print ""$3/$2%10*100"%"}')
 PERCENT_1=$(echo $PERCENT|sed 's/%//g')
 if [[ $PERCENT_1 -gt $mem_mo ]]
     then
      mem_status_total="$mem_total"MB
      mem_status_use="$mem_use"MB
      mem_status_per=$PERCENT
      mem_status=不正常
    else
      mem_status_total="$mem_total"MB
      mem_status_use=$"$mem_use"MB
      mem_status_per=$PERCENT
      mem_status=正常
 fi

echo -e "统计时间|$time_day"
echo -e "服务器IP|$system_ip"
echo -e "服务器信息|$system_info"
echo -e "cpu核心数|$cpu_core_count"
echo -e "cpu厂商|$cpu_cs"
echo -e "cpu型号|$cpu_xh"
echo -e "cpu核心频率|$cpu_core_pl"
echo -e "cpu占有率|$cpu_used"
echo -e "系统进程数|$load_1"
echo -e "cpu空闲id|$load_15"
echo -e "僵尸进程数|$load_5"
echo -e "总内存大小|$mem_status_total"
echo -e "已用内存|$mem_status_use"
echo -e "内存使用率|$mem_status_per"
echo -e "内存巡检状态|$mem_status"
echo -e "数据盘总空间|$disk_total"
echo -e "数据盘剩余空间|$disk_free"
echo -e "数据盘磁盘使用率|$disk_per"
echo -e "磁盘超60使用分区|$disk_f_60"
echo -e "磁盘超60使用率|$disk_per_60" 
echo -e "磁盘状况|$disk_status"
     
