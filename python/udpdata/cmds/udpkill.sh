#!/bin/sh
# command to kill udpdata
# hostnames are in /etc/udpdata/*.json
files='/etc/udpdata/*.json'
for f in $files
do
    host=$(echo $f | cut -d '/' -f 4)
    host=$(echo $host | cut -d '.' -f 1)
    echo $f
    echo $host
    pid=$(cat /var/run/udpdata/${host}.pid)
    echo $pid
    kill $pid
done
return

cd /home/ubuntu/work/dirtdigger/python/udpdata
git pull
mkdir -p /etc/udpdata/cmds
cp /home/ubuntu/work/dirtdigger/python/udpdata/cmds/*.sh /etc/udpdata/cmds

for f in $files
do
    host=$(echo $f | cut -d '/' -f 4)
    host=$(echo $host | cut -d '.' -f 1)
    echo $f
    echo $host
    /home/ubuntu/work/dirtdigger/python/udpdata/udpdata.py $host&
done

