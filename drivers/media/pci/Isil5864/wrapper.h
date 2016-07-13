#ifndef WRAPPER_H
#define WRAPPER_H
#include "LINUXV4L2.h"

void wrapper_spin_lock_irqsave(spinlock_t* lock, ULONG flags);

void wrapper_spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);

int wrapper_list_empty(const struct list_head *head);

void wrapper_list_del(struct list_head *entry);

void wrapper_list_add_tail(struct list_head *new, struct list_head *head);

void wrapper_do_gettimeofday(struct timeval *tv);

void wrapper_wake_up( wait_queue_head_t* x);

#define wrapper_list_first_entry(ptr, type, member) \
        list_entry((ptr)->next, type, member)

void wrapper_snd_pcm_period_elapsed(struct snd_pcm_substream *substream);

signed long __sched wrapper_schedule_timeout_interruptible(signed long timeout);

signed long __sched wrapper_schedule_timeout_uninterruptible(signed long timeout);

int wrapper_kthread_stop(struct task_struct *k);

int wrapper_kthread_should_stop(void);

struct task_struct* wrapper_kthread_run(int (*threadfn)(void *data), void *data, const char namefmt[]);

unsigned long wrapper_msecs_to_jiffies(const unsigned int m);

void *wrapper_kmalloc(size_t s, gfp_t gfp);

void *wrapper_vmalloc(unsigned long size);

void *wrapper_videobuf_to_vmalloc(struct videobuf_buffer *buf);

void wrapper_tasklet_hi_schedule(struct tasklet_struct *t);

int wrapper_pci_read_config_dword(struct pci_dev *dev, int addr, u32 *val);

int wrapper_pci_write_config_dword(struct pci_dev *dev, int addr, u32 val);

long wrapper_IS_ERR(const void *ptr);

long wrapper_PTR_ERR(const void *ptr);

void wrapper_ndelay(unsigned long x);

void wrapper_down(struct semaphore *sem);

void wrapper_up(struct semaphore *sem);

unsigned char wrapper_CMOS_READ(unsigned long addr);

void wrapper_init_waitqueue_head(wait_queue_head_t *q);

#endif //WRAPPER_H