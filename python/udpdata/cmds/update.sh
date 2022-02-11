#!/bin/sh
# command to update udpdata
# hostnames are in /etc/udpdata/*.json
files='/var/run/udpdata/*.pid'
for f in $files
do
    host=$(echo $f | cut -d '/' -f 5)
    host=$(echo $host | cut -d '.' -f 1)
    echo $f
    echo $host
    pid=$(cat /var/run/udpdata/${host}.pid)
    echo $pid
    ok=$(ps ax | grep $pid | grep udpdata)
    if [ -n "$ok" ] 
    then
	echo "found OK"
	kill $pid
    else
	echo "Not found OK"
    fi

done
#return

cd /home/ubuntu/work/dirtdigger/python/udpdata
git pull
mkdir -p /etc/udpdata/cmds
cp cmds/*.sh /etc/udpdata/cmds

#files='/etc/udpdata/*.json'
for f in $files
do
    host=$(echo $f | cut -d '/' -f 5)
    host=$(echo $host | cut -d '.' -f 1)
    echo $f
    echo $host
    nohup /home/ubuntu/work/dirtdigger/python/udpdata/udpdata.py $host > /dev/null&
done

