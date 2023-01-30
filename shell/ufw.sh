#!/usr/bin/bash


get_ufw_status()
{
    local res=$(echo 200101 | sudo -S ufw status | grep inactive)
    if [ -z "$res" ];
    then 
        echo '1'
    else 
        echo '0'
    fi
}

close_ufw()
{
    local res=$(echo 200101 | sudo ufw disable)
}

open_ufw()
{
    local res=$(echo 200101 | sudo ufw enable)
}




if (( $1 == '1' ));
then
    get_ufw_status
elif (( $1 == '2' ));
then
    close_ufw
elif (( $1 == '3' ));
then
    open_ufw
fi


