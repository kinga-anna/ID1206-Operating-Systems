/* same idea solved with atomic add instead of mutex */

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/atomic.h>
#include <linux/bitops.h>

struct task_struct *f_thread; //thread 1
struct task_struct *s_thread; //thread 2

static atomic_t counter = ATOMIC_INIT(0); //initializing the shared atomic variable that will be incremented by both threads

static int first_thread(void *arg) //functionality of thread 1
{
    pr_info("Thread 1 is up and running\n");
    for (int i = 0; i<10000; i++){
        atomic_add(1, &counter); //using atomic_add instead of the '++' operator which ensures atomic increment of the counter vaariable
        udelay(1); //added so that the VM schedules the two threads on different cores
    }
    pr_info("Thread 1 is stopping");
    return 0;
}

static int second_thread(void *arg) //functionality of thread 2 (same as thread 1)
{
    pr_info("Thread 2 is up and running\n");
    for (int j = 0; j<10000; j++){
        atomic_add(1, &counter);
        udelay(1);
    }
    pr_info("Thread 2 is stopping");
    return 0;
}

static int __init mytest_init(void)
{
    pr_info("mytest3 init\n");
    
    f_thread = kthread_create(first_thread, NULL, "f_thread"); //creation of thread 1
    if (IS_ERR(f_thread)) //check for unsuccessful creation
        return -1;

    s_thread = kthread_create(second_thread, NULL, "s_thread"); //creation of thread 2
    if (IS_ERR(s_thread)) //check for unsuccessful creation
        return -1;
    
    wake_up_process(f_thread); //starting thread 1
    wake_up_process(s_thread); //starting thread 2
    
    return 0;
}

static void __exit mytest_exit(void)
{
    int c = atomic_read(&counter); //reading the atomic variable safely into an int
    pr_info("Counter= %d", c); //printing the value just read
    pr_info("mytest3 exit\n");
}

/*identifying the init and exit functions*/
module_init(mytest_init);
module_exit(mytest_exit);

MODULE_DESCRIPTION("My test version 3");
MODULE_LICENSE("GPL");
