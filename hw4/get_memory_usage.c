#include <linux/syscalls.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched/task.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/mm.h>
#include <linux/stat.h>

SYSCALL_DEFINE1(get_memory_usage,int,arg1)
{
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long rss;

    for_each_process(task) {
        //get_task_struct(task);
        if(task->pid == arg1) {
            mm = get_task_mm(task);
            if (mm){
                rss = get_mm_rss(mm) << PAGE_SHIFT;
                //mmput(mm);
                //printk(KERN_INFO "PID %d RSS = %lu bytes\n", task->pid, rss);
                return rss;
            }
        }
    }
    return 0;
}

