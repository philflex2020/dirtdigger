#!/bin/bash
HOSTNAME=`cat /etc/hostname`
#sudo cp udprun.py /usr/local/bin
#source /etc/os-release
#if [ $ID == "raspbian" ] ; then
#  HOMEDIR="/home/pi/work/dirtdigger/python/udpdata"
#else
#  HOMEDIR="/home/ubuntu/work/dirtdigger/python/udpdata"
#fi
#bash -c ". /home/pi/osr_ws/devel/setup.sh"
#bash -c ". /home/pi/osr_ws/devel/setup.bash"
#bash -c ". /opt/ros/kinetic/setup.sh"
#bash -c ". /opt/ros/kinetic/setup.bash"
#cd $HOMEDIR
#git pull
#sudo cp udprun.py /usr/local/bin
#sudo cp udpdata.py /usr/local/bin
#sudo cp scripts/Launch_udpdata.sh /usr/local/bin
bash -i -c "/usr/local/bin/udprun.py $HOSTNAME"
