import os
fd = os.open("/dev/iutnode" , os.O_RDWR)
os.write(fd,"r")
os.write(fd,"e,0,1,1")
os.write(fd,"v,-,3,1")
os.write(fd, "r")
os.write(fd,"b,2,-,1")
line = os.read(fd,5024)
print(line.decode())
os.close(fd)