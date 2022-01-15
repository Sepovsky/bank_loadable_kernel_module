import os
import threading
import random

MAX_VALUE = 2000000
MAX_RANGNE = 1000000
MAX_PORTAL = 2

# thread's function
def donation():
    fd_t = os.open("/dev/iutnode" , os.O_RDWR)
    
    for i in range(MAX_RANGNE):
        acc_num = random.randint(1, 99)
        amount = 1
        trx = "e,{},0,{}".format(acc_num, amount)
        os.write(fd_t, trx)
    
    os.close(fd_t)

# start portals
for i in range(MAX_PORTAL):
    t = threading.Thread(target=donation)
    t.start()
    t.join()

# read answers
fd = os.open("/dev/iutnode" , os.O_RDWR)
line = os.read(fd,5024)
print(line.decode())
os.close(fd)