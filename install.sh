#!/bin/sh
set -xefu
dir0="$PWD"
sudo mkdir -p /usr/share/microem
echo "Install depends"
# apt install xdo-dev xdotool
sudo urpmi gcc-c++ "$(rpm --eval '%_lib')xdotool-devel" xdotool || :
# Что это и зачем?!
#sudo sed -i "s/*class/*_class/g" /usr/include/xdo.h 
echo "Build driver"
cd "$dir0/driver"
make
sudo rmmod microem >/dev/null 2>&1 || :
sudo mv -v microem.ko /lib/modules/`uname -r`
sudo insmod /lib/modules/`uname -r`/microem.ko
echo "microem" | sudo tee /etc/modprobe.d/microem.conf
make clean
echo "Build scanner"
cd "$dir0/scanner"
make
sudo install -m0755 microemscan /usr/local/bin/microemscan
touch /usr/share/microem/dont_send_eol.txt
sudo install -m0644 "$dir0/scanner/microemscan.service" /etc/systemd/system/microemscan.service
sudo systemctl daemon-reload
sudo systemctl enable --now microem.service
