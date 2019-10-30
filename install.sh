#!/bin/sh
echo "Install depends"
sudo urpmi gcc-c++
sudo urpmi libxdotool-devel
sudo urpmi xdotool
sudo sed -i "s/*class/*_class/g" /usr/include/xdo.h 
mkdir distrib
echo "Build driver"
PATH=$PATH:/sbin/
cd driver
make
sudo rmmod microem >/dev/null 2>&1
sudo cp microem.ko /lib/modules/`uname -r`
cp microem.ko ../distrib/
sudo insmod /lib/modules/`uname -r`/microem.ko
cd ..
echo "Build scanner"
cd scanner
make
cp microemscan ../distrib/
touch ../distrib/dont_send_eol.txt
