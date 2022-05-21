#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/moduleparam.h>
#include <linux/mm.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("O. Cihangir");
MODULE_DESCRIPTION("Process Memory Usage");
MODULE_VERSION("0.01");

static int pid = 12915;

module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(pid, "Process ID(int)");

static int __init get_memory_usage(void)
{
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long rss;

    for_each_process(task) {
        //get_task_struct(task);
        if(task->pid == pid) {
            mm = get_task_mm(task);
            if (mm){
                rss = get_mm_rss(mm) << PAGE_SHIFT;
                //mmput(mm);
                printk(KERN_INFO "PID %d RSS = %lu bytes\n", task->pid, rss);
            }
        }
    }
    return 0;
}

static void __exit modexit(void){
    // Just for exiting
}

module_init(get_memory_usage);
module_exit(modexit);
