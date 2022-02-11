#!/bin/sh
cp /etc/dphys-swapfile /tmp
grep 'SIZE=2048' /tmp/dphys-swapfile 2>/dev/null
SIZE_2048=$?
echo $SIZE_2048
grep 'SIZE=100' /tmp/dphys-swapfile 2>/dev/null
echo $?
#exit
if [ "$SIZE_2048" = "0" ] ; then
 NEW_SIZE=100
  sed 's/SIZE=2048/SIZE=100/g' /tmp/dphys-swapfile > /tmp/dphys-swapfile2
else
 NEW_SIZE=2048
 sed 's/SIZE=100/SIZE=2048/g' /tmp/dphys-swapfile > /tmp/dphys-swapfile2
fi
 echo new size=$NEW_SIZE

exit

sudo cp /tmp/dphys-swapfile2 /etc

sudo /etc/init.d/dphys-swapfile stop
sudo /etc/init.d/dphys-swapfile start

