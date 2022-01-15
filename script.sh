make
insmod mymodule.ko
major=$(cat /proc/devices | grep mymodule | sed 's/[^0-9]*//g')
mknod /dev/iutnode c $major 0
python2 test.py
rm /dev/iutnode
rmmod mymodule
make clean