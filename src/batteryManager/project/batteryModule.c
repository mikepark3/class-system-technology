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
//EXPORT_SYMBOL_NOVERS(find_task_by_vpid);	// For problem, unknown symbol in module

#define PROCFS_MAX_SIZE         1024
#define PROCFS_TESTLEVEL        "battery_test"
#define PROCFS_NOTIFYPID        "battery_notify"
#define PROCFS_THRESHOLD        "battery_threshold"
#define CHR_DEV_NAME 		"chr_dev_psu"
#define CHR_DEV_MAJOR 		350

#define DELAY_TIME_MSEC(msec) ((msec) * HZ / 10)


//// Declaration variables ////
/* Declaration of variables used in this module */
static char test_level_procfs_buffer[PROCFS_MAX_SIZE];  //receive side buffer of test_level
static char thresh_procfs_buffer[PROCFS_MAX_SIZE];      //receive side buffer of threshold
static char noti_procfs_buffer[PROCFS_MAX_SIZE];	//receive side buffer of notify_pid
static int level = 99;
static int test_level = 0;     	          	      	//indicates level of battery remain
static int notify_pid = -1;
static int threshold = -1;
/* Declaration of ancillary variables */
static struct timer_list timer;				//timer variable
static struct proc_dir_entry *test_level_proc_entry;       
static struct proc_dir_entry *thresh_proc_entry;      
static struct proc_dir_entry *noti_proc_entry;    


//// Procfs functions ////
/* Implementation of test_level_procfs write, read functions */
static int test_level_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
        int status = 0;
        int requested;
	unsigned long procfs_buffer_size = 0;    //size of receive side buffer

        procfs_buffer_size = len;

        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }

        /* write data to the buffer */
        if ( copy_from_user(test_level_procfs_buffer, user_space_buffer, procfs_buffer_size) ) {
                return -EFAULT;
        }

        status  = kstrtoint(test_level_procfs_buffer, 10, &requested);
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
        return procfs_buffer_size;
}
static int test_level_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;
	unsigned long procfs_buffer_size = 0;    //size of receive side buffer

        if(*off < 0) *off = 0;

        sprintf(test_level_procfs_buffer, "%d\n", test_level);
        procfs_buffer_size = strlen(test_level_procfs_buffer);

        if(*off > procfs_buffer_size){
                return -EFAULT;
        }else if(*off == procfs_buffer_size){
                return 0;
        }

        if(procfs_buffer_size - *off > count)
                ret = count;
        else
                ret = procfs_buffer_size - *off;

        flag = copy_to_user(user_space_buffer, test_level_procfs_buffer + (*off), ret);

        if(flag < 0)
                return -EFAULT;

        *off += ret;
        return ret;
}
/* Implementation of thresh_procfs write, read functions */
static int thresh_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
        int status = 0;
        int requested;
	unsigned long procfs_buffer_size = 0;    //size of receive side buffer

        procfs_buffer_size = len;

        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }

        /* write data to the buffer */
        if ( copy_from_user(thresh_procfs_buffer, user_space_buffer, procfs_buffer_size) ) {
                return -EFAULT;
        }

        status = kstrtoint(thresh_procfs_buffer, 10, &requested);
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...)\n");
                return -ENOMEM;
        }
        // validate level value.
        if(requested< 0 || requested > 100){
                printk(KERN_INFO "Invalid battery level for threshold.\n");
                return -ENOMEM;
        }
        // accept value.
        threshold = requested;
        return procfs_buffer_size;
}
static int thresh_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;
	unsigned long procfs_buffer_size = 0;    //size of receive side buffer

        if(*off < 0) *off = 0;

        sprintf(thresh_procfs_buffer, "%d\n", threshold);
        procfs_buffer_size = strlen(thresh_procfs_buffer);

        if(*off > procfs_buffer_size){
                return -EFAULT;
        }else if(*off == procfs_buffer_size){
                return 0;
        }

        if(procfs_buffer_size - *off > count)
                ret = count;
        else
                ret = procfs_buffer_size - *off;

        flag = copy_to_user(user_space_buffer, thresh_procfs_buffer + (*off), ret);

        if(flag < 0)
                return -EFAULT;

        *off += ret;
        return ret;
}
/* Implementation of noti_procfs write, read functions */
static int noti_write( struct file *filp, const char *user_space_buffer, unsigned long len, loff_t *off )
{
        int status = 0;
        int requested;
	unsigned long procfs_buffer_size = 0;    //size of receive side buffer

        procfs_buffer_size = len;

        if (procfs_buffer_size > PROCFS_MAX_SIZE ) {
                procfs_buffer_size = PROCFS_MAX_SIZE;
        }

        /* write data to the buffer */
        if ( copy_from_user(noti_procfs_buffer, user_space_buffer, procfs_buffer_size) ) {
                return -EFAULT;
        }

        status  = kstrtoint(noti_procfs_buffer, 10, &requested);
        if(status < 0)
        {
                printk(KERN_INFO "Error while called kstrtoint(...)\n");
                return -ENOMEM;
        }
        // accept value.
        notify_pid = requested;
        return procfs_buffer_size;
}
static int noti_read( struct file *filp, char *user_space_buffer, size_t count, loff_t *off )
{
        int ret = 0;
        int flag = 0;
	unsigned long procfs_buffer_size = 0;    //size of receive side buffer

        if(*off < 0) *off = 0;

        sprintf(noti_procfs_buffer, "%d\n", notify_pid);
        procfs_buffer_size = strlen(noti_procfs_buffer);

        if(*off > procfs_buffer_size){
                return -EFAULT;
        }else if(*off == procfs_buffer_size){
                return 0;
        }

        if(procfs_buffer_size - *off > count)
                ret = count;
        else
                ret = procfs_buffer_size - *off;

        flag = copy_to_user(user_space_buffer, noti_procfs_buffer + (*off), ret);

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
        // validate level value.
        if(*num< 0 || *num > 100){
                printk(KERN_INFO "Invalid battery level.\n");
                return -ENOMEM;
        }
        copy_from_user(&test_level, num, count);
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
	int ret = 0;
	struct task_struct *t = NULL;
	// struct siginfo info;

        printk(KERN_ALERT "timer invoke :  %d\n", ++i);
	printk("test_level, threshold, pid : %d, %d, %d\n", test_level, threshold, notify_pid);
	if(notify_pid != -1)
	{
		//t = find_task_by_vpid(notify_pid);
		//t = find_vpid(notify_pid);
		t = pid_task(find_pid_ns(notify_pid, &init_pid_ns), PIDTYPE_PID);
		if(t != NULL)
		{
			if(test_level < threshold)
			{
				//info.si_signo = SIGUSR1;
				//info.si_code = SI_USER;
				//ret = send_sig_info(SIGUSR1, &info, t);
				ret = send_sig_info(SIGUSR1, SEND_SIG_NOINFO, t);
			}
			else
			{
				//info.si_signo = SIGUSR2;
				//info.si_code = SI_USER;
				//ret = send_sig_info(SIGUSR2, &info, t);
				ret = send_sig_info(SIGUSR2, SEND_SIG_NOINFO, t);
			}
			if(ret < 0)
			{
				printk("send signal failed\n");
			}
		}
		else
		{
			printk("find task struct failed, pid : %d\n", notify_pid);
		}
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
static const struct file_operations proc_test_level_fops = {
        .write = test_level_write,
        .read = test_level_read,
};
static const struct file_operations proc_thresh_fops = {
        .write = thresh_write,
        .read = thresh_read,
};
static const struct file_operations proc_noti_fops = {
        .write = noti_write,
        .read = noti_read,
};
/* Configurations of device driver */
static const struct file_operations chr_fops = {
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
        test_level_proc_entry = proc_create(PROCFS_TESTLEVEL, 0666, NULL, &proc_test_level_fops);
        if(test_level_proc_entry == NULL)
        {
                printk("Create test_level procfs failed\n");
                return -ENOMEM;
        }
        thresh_proc_entry = proc_create(PROCFS_THRESHOLD, 0666, NULL, &proc_thresh_fops);
        if(thresh_proc_entry == NULL)
        {
                printk("Create threshold procfs failed\n");
                return -ENOMEM;
        }
        noti_proc_entry = proc_create(PROCFS_NOTIFYPID, 0666, NULL, &proc_noti_fops);
        if(noti_proc_entry == NULL)
        {
                printk("Create notify procfs failed\n");
                return -ENOMEM;
        }
	/* device driver */
	reg = register_chrdev(CHR_DEV_MAJOR, CHR_DEV_NAME, &chr_fops);
        if(reg < 0)
        {
                printk("Register device driver failed\n");
                return reg;
        }
	else
	{
                printk("Register device driver success\n");
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
        remove_proc_entry(PROCFS_TESTLEVEL, test_level_proc_entry);
        remove_proc_entry(PROCFS_THRESHOLD, thresh_proc_entry);
        remove_proc_entry(PROCFS_NOTIFYPID, noti_proc_entry);
}
