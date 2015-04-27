#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>

#define DELAY_TIME_MSEC(msec) ((msec) * HZ / 10)

MODULE_LICENSE("Dual BSD/GPL");

static struct timer_list timer;

static void timer_func(unsigned long data)
{
	static int i = 0;
	printk(KERN_ALERT "%d\n", ++i);
	timer.expires = get_jiffies_64() + DELAY_TIME_MSEC(10);
	timer.function = timer_func;
	add_timer(&timer);
}

static int battery_init(void)
{
	printk(KERN_ALERT "Hello World!\n");

	init_timer(&timer);
	//timer.expires = jiffies + DELAY_TIME_MSEC(10);
	timer.expires = get_jiffies_64() + DELAY_TIME_MSEC(10);
	timer.function = timer_func;
	add_timer(&timer);

	return 0; 					/* return zero on successful loading */
}

static void battery_exit(void)
{
	del_timer(&timer);
	printk(KERN_ALERT "Good bye!\n");
}

module_init(battery_init);
module_exit(battery_exit);
