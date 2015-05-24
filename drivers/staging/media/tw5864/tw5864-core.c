/*
 *  tw5864-core.c
 *  Core functions for the Techwell 5864 driver
 *
 *  Much of this code is derived from the cx88 and sa7134 drivers, which
 *  were in turn derived from the bt87x driver.  The original work was by
 *  Gerd Knorr; more recently the code was enhanced by Mauro Carvalho Chehab,
 *  Hans Verkuil, Andy Walls and many others.  Their work is gratefully
 *  acknowledged.  Full credit goes to them - any problems within this code
 *  are mine.
 *
 *  Copyright (C) 2009  William M. Brack
 *
 *  Refactored and updated to the latest v4l core frameworks:
 *
 *  Copyright (C) 2014 Hans Verkuil <hverkuil@xs4all.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#define DEBUG 1

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kmod.h>
#include <linux/sound.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/dma-mapping.h>
#include <linux/pm.h>

#include <media/v4l2-dev.h>
#include "tw5864.h"
#include "tw5864-reg.h"

MODULE_DESCRIPTION("v4l2 driver module for tw5864 based video capture & encoding cards");
MODULE_AUTHOR("Andrey Utkin <andrey.utkin@corp.bluecherry.net>");
MODULE_LICENSE("GPL");

/* take first free /dev/videoX indexes by default */
static unsigned int video_nr[] = {[0 ... (TW5864_INPUTS - 1)] = -1 };
module_param_array(video_nr, int, NULL, 0444);
MODULE_PARM_DESC(video_nr, "video devices numbers array");


/* ------------------------------------------------------------------ */

/*
 * Please add any new PCI IDs to: http://pci-ids.ucw.cz.  This keeps
 * the PCI ID database up to date.  Note that the entries must be
 * added under vendor 0x1797 (Techwell Inc.) as subsystem IDs.
 */
static const struct pci_device_id tw5864_pci_tbl[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_TECHWELL, PCI_DEVICE_ID_5864)},
	{0,}
};

/* ------------------------------------------------------------------ */

static void tw5864_interrupts_enable(struct tw5864_dev *dev)
{
	mutex_lock(&dev->lock);
	//dev->irqmask |= TW5864_INTR_BURST | TW5864_INTR_MV_DSP | TW5864_INTR_VLC_DONE | TW5864_INTR_VLC_RAM;
	dev->irqmask = 0xffffffff & (~TW5864_INTR_TIMER);
	tw_writew(TW5864_INTR_ENABLE_L, dev->irqmask & 0xffff);
	tw_writew(TW5864_INTR_ENABLE_H, dev->irqmask >> 16);

	/* Use Level-triggered mode, not edge-triggered */
	//tw_setw(TW5864_TRIGGER_MODE_L, dev->irqmask & 0xffff);
	//tw_setw(TW5864_TRIGGER_MODE_H, dev->irqmask >> 16);
	mutex_unlock(&dev->lock);
}

static void tw5864_interrupts_disable(struct tw5864_dev *dev)
{
	mutex_lock(&dev->lock);
	//dev->irqmask &= ~(TW5864_INTR_BURST | TW5864_INTR_MV_DSP | TW5864_INTR_VLC_DONE | TW5864_INTR_VLC_RAM);
	dev->irqmask = 0;
	// TODO deduplicate writing to register(s) with _enable
	tw_writew(TW5864_INTR_ENABLE_L, dev->irqmask & 0xffff);
	tw_writew(TW5864_INTR_ENABLE_H, (dev->irqmask >> 16));
	mutex_unlock(&dev->lock);
}

static irqreturn_t tw5864_isr(int irq, void *dev_id)
{
	struct tw5864_dev *dev = dev_id;
	u32 status;
	int channel;
	int pci_intr_status;

	status = tw_readw(TW5864_INTR_STATUS_L)
		/* | (tw_readw(TW5864_INTR_STATUS_H) << 16) */;
	if (!status)
		return IRQ_NONE;
	// TODO Handle
	//if (status & TW5864_INTR_BURST) {
		// Figure out the channel id of currently encoded frame
		channel = tw_readb(TW5864_DSP) & TW5864_DSP_ENC_CHN;

		// TODO Figure out what is the new data, and what to do
		pci_intr_status = tw_readw(TW5864_PCI_INTR_STATUS);
		dev_dbg(&dev->pci->dev, "tw5864_isr: status: 0x%08x, channel 0x%08x, pci_intr_status 0x%08x\n", status, channel, pci_intr_status);
		if (pci_intr_status & TW5864_VLC_DONE_INTR) {
			// TODO Grab encoded video data
			// TODO Move this block to -video.c
		}

	//}
	tw_writew(TW5864_INTR_CLR_L, status & 0xffff);
	tw_writew(TW5864_INTR_CLR_H, status >> 16);

	return IRQ_HANDLED;
}

static int tw5864_initdev(struct pci_dev *pci_dev,
				     const struct pci_device_id *pci_id)
{
	struct tw5864_dev *dev;
	char irq_owner_display_name[64];
	int err;

	dev = devm_kzalloc(&pci_dev->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	snprintf(dev->name, sizeof(dev->name), "tw5864:%s", pci_name(pci_dev));
	//dev_set_name(&pci_dev->dev, "tw5864:%s", pci_name(pci_dev));

	err = v4l2_device_register(&pci_dev->dev, &dev->v4l2_dev);
	if (err)
		goto v4l2_reg_fail;

	/* pci init */
	dev->pci = pci_dev;
	if (pci_enable_device(pci_dev)) {
		err = -EIO;
		goto pci_enable_fail;
	}

	pci_set_master(pci_dev);

	/* FIXME: What exactly for is this needed? Which mask(s) this driver needs? */
	if (!pci_dma_supported(pci_dev, DMA_BIT_MASK(32))) {
		pr_info("%s: Oops: no 32bit PCI DMA ???\n", dev->name);
		err = -EIO;
		goto req_mem_fail;
	}

	/* get mmio */
	if (!request_mem_region(pci_resource_start(pci_dev, 0),
				pci_resource_len(pci_dev, 0),
				dev->name)) {
		err = -EBUSY;
		pr_err("%s: can't get MMIO memory @ 0x%llx\n", dev->name,
			(unsigned long long)pci_resource_start(pci_dev, 0));
		goto req_mem_fail;
	}
	dev->mmio = ioremap(pci_resource_start(pci_dev, 0),
			     pci_resource_len(pci_dev, 0));
	if (NULL == dev->mmio) {
		err = -EIO;
		pr_err("%s: can't ioremap() MMIO memory\n", dev->name);
		goto ioremap_fail;
	}

	mutex_init(&dev->lock);
	spin_lock_init(&dev->slock);

	err = tw5864_video_init(dev, video_nr);
	if (err)
		goto video_init_fail;

	/* get irq */
	//snprintf(irq_owner_display_name, sizeof(irq_owner_display_name),
	//		"tw5864:irq%d", pci_dev->irq);
	err = devm_request_irq(&pci_dev->dev, pci_dev->irq, tw5864_isr,
			IRQF_SHARED, "tw5864", dev);
	if (err < 0) {
		pr_err("%s: can't get IRQ %d\n", dev->name, pci_dev->irq);
		goto irq_req_fail;
	}

	/* Enable interrupts */
	tw5864_interrupts_enable(dev);

	dev_info(&dev->pci->dev, "hi everybody, it's info\n");
	dev_dbg(&dev->pci->dev, "hi everybody, it's debug\n");

	return 0;

irq_req_fail:
	tw5864_video_fini(dev);
video_init_fail:
	iounmap(dev->mmio);
ioremap_fail:
	release_mem_region(pci_resource_start(pci_dev, 0),
			pci_resource_len(pci_dev, 0));
req_mem_fail:
	pci_disable_device(pci_dev);
pci_enable_fail:
	v4l2_device_unregister(&dev->v4l2_dev);
v4l2_reg_fail:
	devm_kfree(&pci_dev->dev, dev);
	return err;
}

static void tw5864_finidev(struct pci_dev *pci_dev)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct tw5864_dev *dev =
		container_of(v4l2_dev, struct tw5864_dev, v4l2_dev);

	/* shutdown subsystems */
	tw5864_interrupts_disable(dev);

	/* unregister */
	tw5864_video_fini(dev);

	/* release resources */
	iounmap(dev->mmio);
	release_mem_region(pci_resource_start(pci_dev, 0),
			   pci_resource_len(pci_dev, 0));

	v4l2_device_unregister(&dev->v4l2_dev);
	devm_kfree(&pci_dev->dev, dev);
}

#ifdef CONFIG_PM

static int tw5864_suspend(struct pci_dev *pci_dev, pm_message_t state)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct tw5864_dev *dev = container_of(v4l2_dev,
				struct tw5864_dev, v4l2_dev);

	tw5864_interrupts_disable(dev);

	synchronize_irq(pci_dev->irq);

	pci_save_state(pci_dev);
	pci_set_power_state(pci_dev, pci_choose_state(pci_dev, state));
	// vb2_discard_done(&dev->vidq);  // TODO replace with a new tw5864_video_suspend(dev);

	return 0;
}

static int tw5864_resume(struct pci_dev *pci_dev)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct tw5864_dev *dev = container_of(v4l2_dev,
					    struct tw5864_dev, v4l2_dev);
#if 0
	struct tw5864_buf *buf;
	unsigned long flags;
#endif

	pci_set_power_state(pci_dev, PCI_D0);
	pci_restore_state(pci_dev);

	/* Do things that are done in tw5864_initdev ,
		except of initializing memory structures.*/

	msleep(100);

#if 0
	/* FIXME TODO I don't know what this means */
	tw5864_set_tvnorm_hw(dev);

	/*resume unfinished buffer(s)*/
	spin_lock_irqsave(&dev->slock, flags);
	buf = container_of(dev->active.next, struct tw5864_buf, list);

	tw5864_video_start_dma(dev, buf);

	spin_unlock_irqrestore(&dev->slock, flags);
#endif

	tw5864_interrupts_enable(dev);

	return 0;
}
#endif

/* ----------------------------------------------------------- */

static struct pci_driver tw5864_pci_driver = {
	.name	  = "tw5864",
	.id_table = tw5864_pci_tbl,
	.probe	  = tw5864_initdev,
	.remove	  = tw5864_finidev,
#ifdef CONFIG_PM
	.suspend  = tw5864_suspend,
	.resume   = tw5864_resume
#endif
};

module_pci_driver(tw5864_pci_driver);
