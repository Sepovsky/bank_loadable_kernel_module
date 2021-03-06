#include <linux/init.h> // For module init and exit
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> // For fops
#include <linux/uaccess.h>
#include <linux/kthread.h> // kernel threads
#include <linux/string.h> // for strings :)))
#include <linux/slab.h>
#include <linux/semaphore.h>

/////////////////////////////////////
// ====> Bank's logics
//

//defines
#define N 100
#define DEFAULT_VALUE 2000000
#define BASE 10

//global vars
static int accounts[100];
char mode;
int from = 0, to = 0, amount = 0;

static struct semaphore lock;

void mine_cmd(char* cmd){

    int ctr = 0;
    int error;
    int i = 0;
    int part = 0;
    char mine[4][10] = {};
    int deli_ctr = 0;
    
    while (1){

        if(cmd[ctr] == '\0')
            break;
        
        else if(cmd[ctr] == ','){
            deli_ctr++;
            i = 0;
            part++;
        }
        
        else{
            mine[part][i] = cmd[ctr];
            i++;
        }

        ctr++;

        if(deli_ctr > 3){
            printk(KERN_ALERT "Your format is wrong\n");
            return;
        }
    }

    mode = mine[0][0];

    // Check reset mode
    if(mode == 'r'){
        int p;
        for( p=0; p < N; p++) accounts[p] = DEFAULT_VALUE;
        return;
    }

    for(i=1; i < 4; i++){

        if(i == 1){
            if(strcmp(mine[i],"-") == 0) from = -1;
    
            else {
                error = kstrtoint(mine[i], BASE, &from);
                if(from > 100 || from < 0){
                    printk(KERN_ALERT "From is not in range 0-99\n");
                    return;
                }
            }
        }
        else if(i == 2){
            if(strcmp(mine[i],"-") == 0) to = -1;
    
            else {
                error = kstrtoint(mine[i], BASE, &to);
                if(to > 100 || to < 0){
                    printk(KERN_ALERT "TO is not in range 0-99\n");
                    return;
                }
            }
        }
        else if(i == 3){
            error = kstrtoint(mine[i], BASE, &amount);
            if(amount < 0){
                printk(KERN_ALERT "Amount is negative\n");
                return;
            }
        }
    }

}

void trx_bank(char mode, int from, int to, int amount){
    if(mode == 'e'){

        if(accounts[from] < amount){
            printk(KERN_ALERT "Your account balance is not enough\n");
            return;
        }

        accounts[from] -= amount;
        accounts[to] += amount;
    }
    else if(mode == 'v'){

        if(from != -1){
            printk(KERN_ALERT "From should be -\n");
            return;
        }

        accounts[to] += amount;
    }
    else if(mode == 'b'){

        if(to != -1){
            printk(KERN_ALERT "To should be -\n");
            return;
        }

        if(accounts[from] < amount){
            printk(KERN_ALERT "Your account balance is not enough\n");
            return;
        }

        accounts[from] -= amount;
    }
}

char* show_accounts(void){
    int j;
    char* ans;
    ans = kmalloc(5000, GFP_KERNEL);
    for(j=0; j<N; j++){
        char t[50];
        sprintf(t, "%d,", accounts[j]);
        strcat(ans, t);
    }

    return ans;
}

/////////////////////////
//===> Module's implemntaion
//////

//defines
#define DEVICE_NAME "mymodule"
MODULE_LICENSE("GPL");

static int bank_open(struct inode*, struct file*);
static int bank_release(struct inode*, struct file*);
static ssize_t bank_read(struct file*, char*, size_t, loff_t*);
static ssize_t bank_write(struct file*, const char __user *, size_t, loff_t*);

// Set file operations
static struct file_operations fops = {
    .open = bank_open,
    .read = bank_read,
    .write = bank_write,
    .release = bank_release,
};

static int major;

// Event --> Load
static int __init bank_init(void){

    int k;
    major = register_chrdev(0, DEVICE_NAME, &fops);
    
    if(major < 0){
        printk(KERN_ALERT "bank_module load faild\n");
        return major;
    }

    sema_init(&lock, 1);

    for(k=0; k < N; k++) accounts[k] = DEFAULT_VALUE;

    printk(KERN_ALERT "bank_module loaded: %d\n", major);
    return 0;
}

// Event --> UNLOAD
static void __exit bank_exit(void){

    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "bank_module unloaded.\n");
}

// Event --> OPEN
static int bank_open(struct inode* inodep, struct file* filep){
    printk(KERN_INFO "bank is opened\n");
    return 0;
}

// Event --> CLOSE
static int bank_release(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "bank is closed.\n");
   return 0;
}

// Event --> READ
static ssize_t bank_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    
    char* balances = show_accounts();

    if(copy_to_user(buffer, balances, strlen(balances)))
        return -EFAULT;

    return strlen(balances);
}

//Event --> WRITE
static ssize_t bank_write(struct file* filep, const char __user *buffer, size_t len, loff_t *offset){
    char* input = NULL;
    input = kmalloc(len + 1, GFP_KERNEL);
    memset(input, 0, len+1);

    if(copy_from_user(input, buffer, len)){
        printk(KERN_INFO "we have errors\n");
        return -EFAULT;
    }

    input[len] = 0;
    //lock
    down(&lock);

    mine_cmd(input);
    trx_bank(mode, from, to, amount);
    
    up(&lock);
    //lock

    kfree(input);
    return len;
}

// Registering load and unload functions.
module_init(bank_init);
module_exit(bank_exit);