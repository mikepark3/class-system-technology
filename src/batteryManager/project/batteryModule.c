#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <asm/siginfo.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define PROCFS_MAX_SIZE         1024
#define PROCFS_TESTLEVEL        "battery_test"
#define PROCFS_NOTIFYPID        "battery_notify"
#define PROCFS_THRESHOLD        "battery_threshold"
#define CHR_DEV_NAME 		"chr_dev"
#define CHR_DEV_MAJOR 		240

#define DELAY_TIME_MSEC(msec) ((msec) * HZ / 10)


//// Declaration variables ////
/* Declaration of variables used in this module */
static char procfs_buffer[PROCFS_MAX_SIZE];     //receive side buffer
static int level = 99;
static int test_level = 0;                      //indicates level of battery remain
static int notify_pid = -1;
static int threshold = -1;
/* Declaration of ancillary variables */
static unsigned long procfs_buffer_size = 0;    //size of receive side buffer
static struct proc_dir_entry *proc_entry;       //indicates procfs entry
static struct timer_list timer;			//timer variable


//// Procfs functions ////
/* Implementation of procfs write function */
static int test_level_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
        int status = 0;
        int requested;

        procfs_buffer_size = len;

        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }

        /* write data to the buffer */
        if ( copy_from_user(procfs_buffer, user_space_buffer, procfs_buffer_size) ) {
                return -EFAULT;
        }

        status  = kstrtoint(procfs_buffer, 10, &requested);
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...)\n");
                return -ENOMEM;
        }
        // validate level value.
        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        }
        // accept value.
        test_level = requested;

        // *off += procfs_buffer_size; // not necessary here!

        return procfs_buffer_size;
}
/* Implementation of procfs read function */
static int test_level_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;

        if(*off < 0) *off = 0;

        snprintf(procfs_buffer, 16, "%d\n", test_level);
        procfs_buffer_size = strlen(procfs_buffer);

        if(*off > procfs_buffer_size){
                return -EFAULT;
        }else if(*off == procfs_buffer_size){
                return 0;
        }

        if(procfs_buffer_size - *off > count)
                ret = count;
        else
                ret = procfs_buffer_size - *off;

        flag = copy_to_user(user_space_buffer, procfs_buffer + (*off), ret);

        if(flag < 0)
                return -EFAULT;

        *off += ret;

        return ret;

}


//// Device driver functions ////
/* Implementation of device driver open function */
int chr_open(struct inode *inode, struct file *file)
{
        int number = MINOR(inode->i_rdev);      	// Get minor number 
        printk("Virtual Character Device Open: Minor Number is %d\n", number);
        return 0;
}
/* Implementation of device driver read function */
ssize_t chr_read(struct file *file, const int *num, size_t count, loff_t *f_pos)
{
        copy_to_user(num, &test_level, sizeof(count));
        return count;
}
/* Implementation of device driver write function */
ssize_t chr_write(struct file *file, const int *num, size_t count, loff_t *f_pos)
{
        copy_from_user(&test_level, num, count));
        return count;
}
/* Implementation of device driver release function */
int chr_release(struct inode *inode, struct file *file)
{
        printk("Virtual Character Device Release\n");
        return 0;
}


//// Timer functions ////
static void timer_func(unsigned long data)
{
        static int i = 0;
        printk(KERN_ALERT "timer invoke :  %d\n", ++i);
	if(notify_pid != -1)
	{
		if(test_level < threshold)
			kill(notify_pid, SIGUSR1);
		else
			kill(notify_pid, SIGUSR2);
	}
	else
	{
		printk("Power manager process is not exist.\n");
	}
        timer.expires = get_jiffies_64() + DELAY_TIME_MSEC(10);
        timer.function = timer_func;
        add_timer(&timer);
}


//// Configurration operations  ////
/* Configurations of proc file system */
static const struct file_operations proc_fops = {
        .write = test_level_write,
        .read = test_level_read,
};
/* Configurations of device dreiver */
static struct file_operations chr_fops = {
        owner: THIS_MODULE,
        write: chr_write,
        read: chr_read,
        open: chr_open,
        release: chr_release
};


//// Initialization kernel module ////
/* This function will be called on initialization of kernel module */
int init_module(void)
{
        int reg, ret = 0;

	/* procfs */
        proc_entry = proc_create(PROCFS_TESTLEVEL, 0666, NULL, &proc_fops);
        if(proc_entry == NULL)
        {
                printk("Create procfs failed\n");
                return -ENOMEM;
        }
	/* device driver */
	reg = register_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME, &chr_fops);
        if(reg < 0)
        {
                printk("Register device driver failed\n");
                return reg;
        }
	/* timer */
	init_timer(&timer);
        //timer.expires = jiffies + DELAY_TIME_MSEC(10);
        timer.expires = get_jiffies_64() + DELAY_TIME_MSEC(10);
        timer.function = timer_func;
        add_timer(&timer);

        return ret;
}
/* This function will be called on cleaning up of kernel module */
void cleanup_module(void)
{
	/* tiemr */
	del_timer(&timer);
	/* device driver */
	unregister_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME);
	/* procfs */
        remove_proc_entry(PROCFS_TESTLEVEL, proc_entry);
}
