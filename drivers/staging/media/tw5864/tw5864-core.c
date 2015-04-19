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

static unsigned int latency = UNSET;
module_param(latency, int, 0444);
MODULE_PARM_DESC(latency, "pci latency timer");

static unsigned int video_nr[] = {[0 ... (TW5864_MAXBOARDS - 1)] = UNSET };
module_param_array(video_nr, int, NULL, 0444);
MODULE_PARM_DESC(video_nr, "video device number");

static unsigned int card[] = {[0 ... (TW5864_MAXBOARDS - 1)] = UNSET };
module_param_array(card, int, NULL, 0444);
MODULE_PARM_DESC(card, "card type");

static atomic_t tw5864_instance = ATOMIC_INIT(0);

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

static irqreturn_t tw5864_irq(int irq, void *dev_id)
{
	struct tw5864_dev *dev = dev_id;
	u32 status;

	status = tw_readw(TW5864_INTR_STATUS_L)
		| (tw_readw(TW5864_INTR_STATUS_H) << 16);
	if (!status)
		return IRQ_NONE;
	// TODO Save status for debug printout
	// TODO Handle

	return IRQ_HANDLED;
}

static int tw5864_initdev(struct pci_dev *pci_dev,
				     const struct pci_device_id *pci_id)
{
	struct tw5864_dev *dev;
	int vidnr = -1;
	int err;

	dev = devm_kzalloc(&pci_dev->dev, sizeof(*dev), GFP_KERNEL);
	if (NULL == dev)
		return -ENOMEM;

	dev->instance = v4l2_device_set_name(&dev->v4l2_dev, "tw5864",
						&tw5864_instance);

	err = v4l2_device_register(&pci_dev->dev, &dev->v4l2_dev);
	if (err)
		return err;

	/* pci init */
	dev->pci = pci_dev;
	if (pci_enable_device(pci_dev)) {
		err = -EIO;
		goto fail1;
	}

	dev->name = dev->v4l2_dev.name;

	if (UNSET != latency) {
		pr_info("%s: setting pci latency timer to %d\n",
		       dev->name, latency);
		pci_write_config_byte(pci_dev, PCI_LATENCY_TIMER, latency);
	}

	/* print pci info */
	pci_read_config_byte(pci_dev, PCI_CLASS_REVISION, &dev->pci_rev);
	pci_read_config_byte(pci_dev, PCI_LATENCY_TIMER,  &dev->pci_lat);
	pr_info("%s: found at %s, rev: %d, irq: %d, latency: %d, mmio: 0x%llx\n",
		dev->name, pci_name(pci_dev), dev->pci_rev, pci_dev->irq,
		dev->pci_lat, (u64)pci_resource_start(pci_dev, 0));
	pci_set_master(pci_dev);
	if (!pci_dma_supported(pci_dev, DMA_BIT_MASK(32))) {
		pr_info("%s: Oops: no 32bit PCI DMA ???\n", dev->name);
		err = -EIO;
		goto fail1;
	}

	/* get mmio */
	if (!request_mem_region(pci_resource_start(pci_dev, 0),
				pci_resource_len(pci_dev, 0),
				dev->name)) {
		err = -EBUSY;
		pr_err("%s: can't get MMIO memory @ 0x%llx\n",
			dev->name,
			(unsigned long long)pci_resource_start(pci_dev, 0));
		goto fail1;
	}
	dev->mmio = ioremap(pci_resource_start(pci_dev, 0),
			     pci_resource_len(pci_dev, 0));
	if (NULL == dev->mmio) {
		err = -EIO;
		pr_err("%s: can't ioremap() MMIO memory\n",
		       dev->name);
		goto fail2;
	}

	/* Initialize the device control structures */
	mutex_init(&dev->lock);
	spin_lock_init(&dev->slock);

	dev->alloc_ctx = vb2_dma_sg_init_ctx(&pci_dev->dev);
	if (IS_ERR(dev->alloc_ctx)) {
		err = PTR_ERR(dev->alloc_ctx);
		goto fail3;
	}

	/* get irq */
	err = devm_request_irq(&pci_dev->dev, pci_dev->irq, tw5864_irq,
			  IRQF_SHARED, dev->name, dev);
	if (err < 0) {
		pr_err("%s: can't get IRQ %d\n",
		       dev->name, pci_dev->irq);
		goto fail4;
	}

	/*
	 *  Now do remainder of initialisation, first for
	 *  things unique for this card, then for general board
	 */
	if (dev->instance < TW5864_MAXBOARDS)
		vidnr = video_nr[dev->instance];
	/* initialise video function first */
	err = tw5864_video_init2(dev, vidnr);
	if (err < 0) {
		pr_err("%s: can't register video device\n",
		       dev->name);
		goto fail5;
	}
	tw_setl(TW5864_INTMASK, dev->pci_irqmask);

	pr_info("%s: registered device %s\n",
	       dev->name, video_device_node_name(&dev->vdev));

	return 0;

fail5:
	video_unregister_device(&dev->vdev);
fail4:
	vb2_dma_sg_cleanup_ctx(dev->alloc_ctx);
fail3:
	iounmap(dev->mmio);
fail2:
	release_mem_region(pci_resource_start(pci_dev, 0),
			   pci_resource_len(pci_dev, 0));
fail1:
	v4l2_device_unregister(&dev->v4l2_dev);
	return err;
}

static void tw5864_finidev(struct pci_dev *pci_dev)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct tw5864_dev *dev =
		container_of(v4l2_dev, struct tw5864_dev, v4l2_dev);

	/* shutdown subsystems */
	tw_clearl(TW5864_DMAC, TW5864_DMAP_EN | TW5864_FIFO_EN);
	tw_writel(TW5864_INTMASK, 0);

	/* unregister */
	video_unregister_device(&dev->vdev);
	v4l2_ctrl_handler_free(&dev->hdl);
	vb2_dma_sg_cleanup_ctx(dev->alloc_ctx);

	/* release resources */
	iounmap(dev->mmio);
	release_mem_region(pci_resource_start(pci_dev, 0),
			   pci_resource_len(pci_dev, 0));

	v4l2_device_unregister(&dev->v4l2_dev);
}

#ifdef CONFIG_PM

static int tw5864_suspend(struct pci_dev *pci_dev , pm_message_t state)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct tw5864_dev *dev = container_of(v4l2_dev,
				struct tw5864_dev, v4l2_dev);

	tw_clearl(TW5864_DMAC, TW5864_DMAP_EN | TW5864_FIFO_EN);
	dev->pci_irqmask &= ~TW5864_VID_INTS;
	tw_writel(TW5864_INTMASK, 0);

	synchronize_irq(pci_dev->irq);

	pci_save_state(pci_dev);
	pci_set_power_state(pci_dev, pci_choose_state(pci_dev, state));
	vb2_discard_done(&dev->vidq);

	return 0;
}

static int tw5864_resume(struct pci_dev *pci_dev)
{
	struct v4l2_device *v4l2_dev = pci_get_drvdata(pci_dev);
	struct tw5864_dev *dev = container_of(v4l2_dev,
					    struct tw5864_dev, v4l2_dev);
	struct tw5864_buf *buf;
	unsigned long flags;

	pci_set_power_state(pci_dev, PCI_D0);
	pci_restore_state(pci_dev);

	/* Do things that are done in tw5864_initdev ,
		except of initializing memory structures.*/

	msleep(100);

	tw5864_set_tvnorm_hw(dev);

	/*resume unfinished buffer(s)*/
	spin_lock_irqsave(&dev->slock, flags);
	buf = container_of(dev->active.next, struct tw5864_buf, list);

	tw5864_video_start_dma(dev, buf);

	spin_unlock_irqrestore(&dev->slock, flags);

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
