#!/bin/sh
# update the udpdata system
#
cd ~/work/dirtdigger
git pull
sudo cp ~/work/dirtdigger/python/udpdata/scripts/udpdata /usr/local/bin
/usr/local/bin/udpdata stop
/usr/local/bin/udpdata start
