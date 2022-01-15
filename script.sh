#!/bin/bash

echo "start making module..."
make
insmod mymodule.ko
major=$(cat /proc/devices | grep mymodule | sed 's/[^0-9]*//g')
mknod /dev/iutnode c $major 0
echo "running test....."
python2 test.py
echo "removing module...!!"
rm /dev/iutnode
rmmod mymodule
make clean
