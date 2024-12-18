/* same idea with synchronisation using mutex */

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/delay.h>

struct task_struct *f_thread; //thread 1
struct task_struct *s_thread; //thread 2

static int counter = 0; //shared variable that will be incremented by both threads

static DEFINE_MUTEX(mymutex); //initializing the mutex with name mymutex

static int first_thread(void *arg) //functionality of thread 1
{
    pr_info("Thread 1 is up and running\n");
    for (int i = 0; i<10000; i++){
        mutex_lock(&mymutex); //locking the mutex (if unsuccessful, the thread is put to sleep and will only be woken up once the mutex is unlocked
        counter++;
        mutex_unlock(&mymutex); //releasing the mutex
        udelay(1); //added so that the VM schedules the two threads on different cores
    }
    pr_info("Thread 1 is stopping");
    return 0;
}

static int second_thread(void *arg) //functionality of thread 2 (same as thread 1)
{
    pr_info("Thread 2 is up and running\n");
    for (int j = 0; j<10000; j++){
        mutex_lock(&mymutex);
        counter++;
        mutex_unlock(&mymutex);
        udelay(1);
    }
    pr_info("Thread 2 is stopping");
    return 0;
}

static int __init mytest_init(void)
{
    pr_info("mytest2 init\n");
    
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
    pr_info("Counter= %d", counter); //printing the counter to log
    pr_info("mytest2 exit\n");
}

/*identifying the init and exit functions*/
module_init(mytest_init);
module_exit(mytest_exit);

MODULE_DESCRIPTION("My test version 2");
MODULE_LICENSE("GPL");
