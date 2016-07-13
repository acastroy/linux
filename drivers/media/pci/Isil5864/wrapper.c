#include "wrapper.h"

void wrapper_spin_lock_irqsave(spinlock_t* lock, ULONG flags)
{
#ifndef CONFIG_ARM
	spin_lock_irqsave(lock, flags);	
#endif
}

void wrapper_spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)
{
#ifndef CONFIG_ARM
	spin_unlock_irqrestore( lock, flags);
#endif
}

int wrapper_list_empty(const struct list_head *head)
{
	return list_empty( head);
}

void wrapper_list_del(struct list_head *entry)
 {
	list_del(entry);
 }

void wrapper_list_add_tail(struct list_head *new, struct list_head *head)
{
	list_add_tail(new, head);
}

void wrapper_do_gettimeofday(struct timeval *tv)
{
	do_gettimeofday(tv);
}

void wrapper_wake_up( wait_queue_head_t* x)
{
	wake_up(x);
}

void wrapper_snd_pcm_period_elapsed(struct snd_pcm_substream *substream)
{
	snd_pcm_period_elapsed(substream);
}

signed long __sched wrapper_schedule_timeout_interruptible(signed long timeout)
{
	return schedule_timeout_interruptible(timeout);
}

signed long __sched wrapper_schedule_timeout_uninterruptible(signed long timeout)
{
	return schedule_timeout_uninterruptible(timeout);
}

int wrapper_kthread_stop(struct task_struct *k)
{
	return kthread_stop(k);
}

int wrapper_kthread_should_stop(void)
{
	return kthread_should_stop();
}

struct task_struct* wrapper_kthread_run(int (*threadfn)(void *data), void *data, const char namefmt[])
{
	return kthread_run(threadfn, data, namefmt);
}

unsigned long wrapper_msecs_to_jiffies(const unsigned int m)
{
	return msecs_to_jiffies(m);
}

void *wrapper_kmalloc(size_t s, gfp_t gfp)
{
	return kmalloc(s, gfp);
}
void *wrapper_vmalloc(unsigned long size)
{
	vmalloc(size);
}

void *wrapper_videobuf_to_vmalloc(struct videobuf_buffer *buf)
{
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	return videobuf_to_vmalloc(buf);
	#endif
}

void wrapper_tasklet_hi_schedule(struct tasklet_struct *t)
{
	tasklet_hi_schedule(t);
}

int wrapper_pci_read_config_dword(struct pci_dev *dev, int addr, u32 *val)
{
	return pci_read_config_dword(dev, addr, val);
}

int wrapper_pci_write_config_dword(struct pci_dev *dev, int addr, u32 val)
{
	return pci_write_config_dword(dev, addr, val);
}

long wrapper_IS_ERR(const void *ptr)
{
	return IS_ERR(ptr);
}

long wrapper_PTR_ERR(const void *ptr)
{
	return PTR_ERR(ptr);
}
void wrapper_ndelay(unsigned long x)
{
	ndelay(x);
}

void wrapper_down(struct semaphore *sem)
{
	down(sem);
}
void wrapper_up(struct semaphore *sem)
{
	up(sem);	
}

unsigned char wrapper_CMOS_READ(unsigned long addr)
{
	CMOS_READ(addr);
}

void wrapper_init_waitqueue_head(wait_queue_head_t *q)
{
	init_waitqueue_head(q);
}