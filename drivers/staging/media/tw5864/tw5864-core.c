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
#include <linux/debugfs.h>

#include <asm/dma.h>

#include <media/v4l2-dev.h>
#include "tw5864.h"
#include "tw5864-reg.h"

#include "tables_upload.c"

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

#define SWAP32(x) \
	((u32)( \
		(((u32)(x) & (u32)0x000000ffUL) << 24) | \
		(((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
		(((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
		(((u32)(x) & (u32)0xff000000UL) >> 24) ))

#define PLATFORM_ENDIAN_SAME 1
static u32 crc_check_sum(u32 *data, int len){
	u32 val, count_len=len;

	val = *data++;
	while(((count_len>>2) - 1) > 0) {
		val ^= *data++;
		count_len -= 4;
	}
#if defined(PLATFORM_ENDIAN_SAME)
	val ^= SWAP32((len >> 2));
#else
	val ^= (len >> 2);
#endif
	return val;
}

static irqreturn_t tw5864_isr(int irq, void *dev_id)
{
	struct tw5864_dev *dev = dev_id;
	u32 status;
	u32 vlc_len;
	u32 vlc_crc;
	u32 vlc_reg;  // TW5864_VLC
	u32 mv_reg;
	u32 vlc_buf_reg;  // TW5864_VLC_BUF
	int channel;
	int pci_intr_status;
	int i;
	u32 ddr_ctl_status;
	u32 pci_intr_ctl;

	status = tw_readw(TW5864_INTR_STATUS_L)
		 | (tw_readw(TW5864_INTR_STATUS_H) << 16);
	if (!status)
		return IRQ_NONE;

	tw_writel(0x00008810, 0xffff);
	tw_writel(0x00008814, 0xffff);

	pci_intr_status = tw_readw(TW5864_PCI_INTR_STATUS);

	pci_intr_ctl = tw_readl(TW5864_PCI_INTR_CTL);

	ddr_ctl_status = tw_readl(TW5864_DDR_CTL);
	if (status & TW5864_INTR_VLC_DONE) {
		struct tw5864_input *input = &dev->inputs[0];
				u32 chunk[4];

				vlc_len = tw_readl(TW5864_VLC_LENGTH) << 2;
				vlc_crc = tw_readl(TW5864_VLC_CRC_REG);
				channel = tw_readl(TW5864_DSP) & TW5864_DSP_ENC_CHN;
				vlc_reg = tw_readl(TW5864_VLC);
				vlc_buf_reg = tw_readl(TW5864_VLC_BUF);

		dev_dbg(&dev->pci->dev, "tw5864_isr: vlc done. channel 0x%08x, vlc_len %d, vlc_crc 0x%08x, vlc_buf_rdy 0x%02x, vlc_buf_reg 0x%08x\n", channel, vlc_len, vlc_crc, (vlc_reg & TW5864_VLC_BUF_RDY_MASK) >> TW5864_VLC_BUF_RDY_SHIFT, vlc_buf_reg);
		dma_sync_single_for_cpu(&dev->pci->dev, dev->h264_vlc_buf[0].dma_addr, H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
		//dma_sync_single_for_cpu(&dev->pci->dev, dev->h264_mv_buf[0].dma_addr, H264_MV_BUF_SIZE, DMA_FROM_DEVICE);


		for (i = 0; i < sizeof(chunk) / sizeof(chunk[0]); i++) {
			chunk[i] = ((u32*)dev->h264_vlc_buf[0].addr)[i];
		}
		dev_dbg(&dev->pci->dev, "tw5864_isr: vlc buf: hex: %08x %08x %08x %08x\n", chunk[0], chunk[1], chunk[2], chunk[3]);

		dev_dbg(&dev->pci->dev, "CPU-computed CRC: %08x\n", 
				crc_check_sum((u32*)dev->h264_vlc_buf[0].addr, vlc_len));

		tw5864_handle_frame(input, vlc_len);
		// TODO Do whatever needed, e.g. dump contents elsewhere
		dma_sync_single_for_device(&dev->pci->dev, dev->h264_vlc_buf[0].dma_addr, H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
		//dma_sync_single_for_device(&dev->pci->dev, dev->h264_mv_buf[0].dma_addr, H264_MV_BUF_SIZE, DMA_FROM_DEVICE);
		

		u32 prev_buf_id = dev->buf_id;
		dev->buf_id = (dev->buf_id + 1) % 4;
		w(0x0000021C, dev->buf_id << 12); /* chip f5880300 */ /* in ISR */
		w(0x00000210,(dev->buf_id << 12) | prev_buf_id); /* chip f5880300 */ /* in ISR */

		input->frame_seqno++;

		// TODO Move this section to be done just before encoding job is fired
		if (input->frame_seqno % 4 == 0) {
			w(TW5864_MOTION_SEARCH_ETC,0x00000008); // produce intra frame for #4, #8, #12...
			input->h264_idr_pic_id++;
			input->h264_idr_pic_id &= TW5864_DSP_REF_FRM;
			tw_writel(TW5864_DSP_REF, (tw_readl(TW5864_DSP_REF) & ~TW5864_DSP_REF_FRM) | input->h264_idr_pic_id);
		} else {
			w(TW5864_MOTION_SEARCH_ETC,0x000000BF);
		}
		// End TODO

#include "vlc_intr_0.c"
	}
	
	if (status & TW5864_INTR_TIMER) {
		if (!((tw_readl(TW5864_INTR_ENABLE_H) << 16) & TW5864_INTR_VLC_DONE)) {
			dev->timers_with_vlc_disabled++;
			if (dev->timers_with_vlc_disabled > 10) {
				dev_dbg(&dev->pci->dev, "enabling VLC irq again\n");
				dev->timers_with_vlc_disabled = 0;
				if (!dev->long_timer_scenario_done) {
					dev->long_timer_scenario_done = 1;
#include "timer_intr_6.c"
					tw_writel(TW5864_DSP_QP, QP_VALUE);
				} else {
#include "timer_intr_7.c"
					tw_writel(TW5864_DSP_QP, QP_VALUE);
				}
			}
		}

		w(0x00018000,0x00000040);


	}
	
	if (!(status & (TW5864_INTR_TIMER | TW5864_INTR_VLC_DONE))){
		dev_dbg(&dev->pci->dev, "tw5864_isr: not timer and not vlc, status 0x%08X\n", status);
	}

	return IRQ_HANDLED;
}


static size_t regs_dump(struct tw5864_dev *dev, char *buf, size_t size)
{
	size_t count = 0;

	u32 reg_addr;
	u32 value;

	for (reg_addr = 0x0000; (count < size) && (reg_addr <= 0x2FFC); reg_addr += 4) {
		value = tw_readl(reg_addr);
		count += scnprintf(buf + count, size - count,
				"[0x%05x] = 0x%08x\n", reg_addr, value);
	}

	for (reg_addr = 0x4000; (count < size) && (reg_addr <= 0x4FFC); reg_addr += 4) {
		value = tw_readl(reg_addr);
		count += scnprintf(buf + count, size - count,
				"[0x%05x] = 0x%08x\n", reg_addr, value);
	}

	for (reg_addr = 0x8000; (count < size) && (reg_addr <= 0x180DC); reg_addr += 4) {
		value = tw_readl(reg_addr);
		count += scnprintf(buf + count, size - count,
				"[0x%05x] = 0x%08x\n", reg_addr, value);
	}

	for (reg_addr = 0x18100; (count < size) && (reg_addr <= 0x1817C); reg_addr += 4) {
		value = tw_readl(reg_addr);
		count += scnprintf(buf + count, size - count,
				"[0x%05x] = 0x%08x\n", reg_addr, value);
	}

	for (reg_addr = 0x80000; (count < size) && (reg_addr <= 0x87FFF); reg_addr += 4) {
		value = tw_readl(reg_addr);
		count += scnprintf(buf + count, size - count,
				"[0x%05x] = 0x%08x\n", reg_addr, value);
	}

	for (reg_addr = 0x0; (count < size) && (reg_addr <= 0xEFE); reg_addr += 1) {
		value = tw_indir_readb(dev, reg_addr);
		count += scnprintf(buf + count, size - count,
				"indir[0x%03x] = 0x%02x\n", reg_addr, value);
	}

	return count;
}


#define DEBUGFS_BUF_SIZE	1024 * 1024

struct debugfs_buffer {
	size_t count;
	char data[DEBUGFS_BUF_SIZE];
};

static int debugfs_regs_dump_open(struct inode *inode, struct file *file)
{
	struct tw5864_dev *dev = inode->i_private;
	struct debugfs_buffer *buf;

	buf = kmalloc(sizeof(*buf), GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	buf->count = regs_dump(dev, buf->data, sizeof(buf->data));

	file->private_data = buf;
	return 0;
}

static ssize_t debugfs_regs_dump_read(struct file *file, char __user *user_buf,
				      size_t nbytes, loff_t *ppos)
{
	struct debugfs_buffer *buf = file->private_data;

	return simple_read_from_buffer(user_buf, nbytes, ppos, buf->data,
				       buf->count);
}

static int debugfs_regs_dump_release(struct inode *inode, struct file *file)
{
	kfree(file->private_data);
	file->private_data = NULL;

	return 0;
}

static const struct file_operations debugfs_regs_dump_fops = {
	.owner = THIS_MODULE,
	.open = debugfs_regs_dump_open,
	.llseek = no_llseek,
	.read = debugfs_regs_dump_read,
	.release = debugfs_regs_dump_release,
};


static int x86_dma_init(struct tw5864_dev *dev)
{
	unsigned int dmanr;

	for(dmanr = 0; dmanr < MAX_DMA_CHANNELS; dmanr++){
		if(dmanr == 4){/*for cascade*/
			continue;
		}
		if(request_dma(dmanr, "tw5864") == 0){
			dev_dbg(&dev->pci->dev, "got dmanr %d\n", dmanr);
			//s_dmanr = dmanr;
			break;
		}
	}
	if(dmanr == 8){
		dev_dbg(&dev->pci->dev, "dma request failed\n");
		return 1;
	}

	return 0;
}


static int tw5864_initdev(struct pci_dev *pci_dev,
				     const struct pci_device_id *pci_id)
{
	struct tw5864_dev *dev;
	char irq_owner_display_name[64];
	int err;
	u16 cmd;

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

	if ((pci_name(pci_dev))[9] != '4')
		return -1;

	pci_read_config_word(pci_dev, PCI_COMMAND, &cmd);
	cmd |= PCI_COMMAND_IO;
	pci_write_config_word(pci_dev, PCI_COMMAND, cmd);
	pci_read_config_word(pci_dev, PCI_COMMAND, &cmd);
	pr_err("%s: cmd after setting PCI_COMMAND_IO: 0x%04x\n", pci_name(pci_dev), cmd);

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
	dev->mmio = ioremap_nocache(pci_resource_start(pci_dev, 0),
			     pci_resource_len(pci_dev, 0));
	if (NULL == dev->mmio) {
		err = -EIO;
		pr_err("%s: can't ioremap() MMIO memory\n", dev->name);
		goto ioremap_fail;
	}

	mutex_init(&dev->lock);
	spin_lock_init(&dev->slock);

	x86_dma_init(dev);

	/* Enable interrupts */
	tw5864_interrupts_enable(dev);








	dev->debugfs_dir = debugfs_create_dir(dev->name, NULL);
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

	debugfs_create_file("regs_dump", S_IRUGO, dev->debugfs_dir, dev, &debugfs_regs_dump_fops);

	dev_info(&dev->pci->dev, "hi everybody, it's info\n");
	dev_dbg(&dev->pci->dev, "hi everybody, it's debug\n");



	WriteForwardQuantizationTable(dev);
	WriteInverseQuantizationTable(dev);
	WriteEncodeVLCLookupTable(dev);

	pci_init_ad(dev);
//#include "init_no_i2c.c"
//#if 0
//#include "init2.c"
//#include "init3.c"
//#if 0
//#include "init4.c"
//#include "init5.c"
//#include "init6.c"
#if 0
//#include "init7.c"
#endif
	tw_writel(TW5864_VLC_STREAM_BASE_ADDR, dev->h264_vlc_buf[0].dma_addr);
	tw_writel(TW5864_MV_STREAM_BASE_ADDR, dev->h264_mv_buf[0].dma_addr);
	w(TW5864_MOTION_SEARCH_ETC,0x00000008);
	tw_indir_writeb(dev, 0xefc, 0x00);
	// set real bitalign
	int bitalign_32;
	bitalign_32 = 0;
	w(TW5864_VLC, (tw_readl(TW5864_VLC) & ~TW5864_VLC_BIT_ALIGN_MASK)
			| ((bitalign_32 << TW5864_VLC_BIT_ALIGN_SHIFT) & TW5864_VLC_BIT_ALIGN_MASK)
			);


	int i;
	for (i = 0xC100; i <= 0xC17C; i += 8) {
		tw_writel(i, 1);
		tw_writel(i + 4, 1);
	}
#if 0
	tw_indir_writeb(dev, 0x00E, 0x0f /* ATREG=1, //PAL */);
	// maybe just tw_indir_writeb(dev, 0x00f, 0x80); // to initiate auto format recognition
	//tw_indir_writeb(dev, 0x002, 0x0a);
	//tw_indir_writeb(dev, 0x0e8, 0x90);
	tw_indir_writeb(dev, 0x200, 0xb4);
	tw_indir_writeb(dev, 0x201, 0x3c);
	tw_indir_writeb(dev, 0x202, 0x5a);
	tw_indir_writeb(dev, 0x203, 0x1e);

	tw_writel(0x0d10, 0x2cf);
	tw_writel(0x0d14, 0x2cf);
	tw_writel(0x0d18, 0x23f);
	tw_writel(0x0d1c, 0x23f);

	tw_writel(0xc014, (24 | (24 << 5)));
	tw_writel(0xc018, (24 | (24 << 5)));

	tw_writel(0xc020, 0);
	tw_writel(0xc024, 0);

	tw_writel(0x18044, 0x6fff);

	tw_indir_writeb(dev, 0x260, 0x01 /* PAL */);
#endif

	tw_writel(0xc020, 0);
	tw_writel(0xc024, 0);
	
	tw_indir_writeb(dev, 0x00e, 0x07);
	tw_indir_writeb(dev, 0x00f, 0xff); // to initiate auto format recognition

	tw_indir_writeb(dev, 0x200, 0xb4);
	tw_indir_writeb(dev, 0x201, 0x48);
	tw_indir_writeb(dev, 0x202, 0xb4);
	tw_indir_writeb(dev, 0x203, 0x48);

	tw_writel(0x0d10, 0x2cf);
	tw_writel(0x0d14, 0x2cf);
	tw_writel(0x0d18, 0x23f);
	tw_writel(0x0d1c, 0x23f);


	//tw_writel(TW5864_PCI_PV_CH_EN, 0x0001);
	tw_writel(TW5864_SEN_EN_CH, 0x0001);

	tw_writel(0x00000D00,0x00000017);

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

	debugfs_remove_recursive(dev->debugfs_dir);

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

	pci_set_power_state(pci_dev, PCI_D0);
	pci_restore_state(pci_dev);

	/* Do things that are done in tw5864_initdev ,
		except of initializing memory structures.*/

	msleep(100);


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
