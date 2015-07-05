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
#if 1
	// TODO express meaningfully
	tw_indir_writel(dev, 0x041/* ISIL_VI_VD_EDGE */, 0x03);/*use falling edge to sample ,54M to 108M*/
	tw_indir_writel(dev, 0x046/* ISIL_VI_AGCGAIN */, 0x0);/*AGC loop function disable*/

	tw_writew(TW5864_SYSPLL1, 0xd000 + ( (185 << 1) - 1 ));
	tw_writew(TW5864_SYSPLL3, 0x284);
	tw_writew(TW5864_SYSPLL5, 0x20);
	tw_writew(TW5864_PLL_CFG, 0x0d);

	//tw_clearb(TW5864_SPLL, 1 /* SPLL_DBG */);
	mdelay(10);

	/*reset*/
	tw_indir_writel(dev, 0xef0/* ISIL_VI_SYSTEM_RESET */, 0x00);/*reset*/
	tw_indir_writel(dev, 0xef0/* ISIL_VI_SYSTEM_RESET */, 0xe0);/*unreset*/
	mdelay(10);
	tw_indir_writel(dev, 0xefc/* ISIL_VI_SYSTEM_CLOCK */, 0xf0);
	tw_indir_writel(dev, 0xefd/* ISIL_VI_SYSTEM_CLOCK_REVERSE */, 0xf0);/*155 set to 0xff, 330 set to 0xf0*/

	tw_writew(TW5864_CS2DAT_CNT, 1);
	tw_writew(TW5864_DATA_VLD_WIDTH, 3);
	mdelay(10);

	tw_writeb(0xA000, 0xc5); // only [2:0] is defined
	tw_writeb(0xA800, 0xc5); // same

	tw_writeb(TW5864_IIC_ENB, 1);
	tw_writeb(TW5864_I2C_PHASE_CFG, 1);

	tw_writeb(0x8028, 0x15);
	tw_writeb(0x801c, 0x18);
	mdelay(5);
	tw_writeb(0x801c, 0x00);
#endif
	
	tw_writeb(TW5864_RST_AND_IF_INFO, TW5864_APP_SOFT_RST);  // reset, board reconfigures its memory settings basing on register values and starts operation
	mdelay(5);
	tw_writeb(TW5864_RST_AND_IF_INFO, 0);  // unreset

	//dev->irqmask |= TW5864_INTR_BURST | TW5864_INTR_MV_DSP | TW5864_INTR_VLC_DONE | TW5864_INTR_VLC_RAM;
	//dev->irqmask |= TW5864_INTR_BURST | TW5864_INTR_MV_DSP | TW5864_INTR_VLC_DONE | TW5864_INTR_VLC_RAM | TW5864_INTR_VIN_LOST;
	////dev->irqmask = 0xfffff00f /* no GPIO */  & (~(TW5864_INTR_TIMER /* | TW5864_INTR_JPEG | TW5864_INTR_MV_DSP | TW5864_INTR_VLC_RAM*/));

	tw_setl(TW5864_DDR_CTL, TW5864_SING_ERR_INTR | TW5864_BRST_ERR_INTR | TW5864_BRST_END_INTR);

	dev->irqmask = TW5864_INTR_VLC_RAM | TW5864_INTR_VLC_DONE | TW5864_INTR_VIN_LOST;
	tw_writew(TW5864_INTR_ENABLE_L, dev->irqmask & 0xffff);
	tw_writew(TW5864_INTR_ENABLE_H, dev->irqmask >> 16);

	tw_writew(TW5864_INTR_ASSERT_L, 0xffff);
	tw_writew(TW5864_INTR_ASSERT_H, 0xffff);

	//tw_writew(TW5864_PCI_INTTM_SCALE, 3);


	/* Use Level-triggered mode, not edge-triggered (nothing to do) */

	mutex_unlock(&dev->lock);
	dev_dbg(&dev->pci->dev, "intr_enable: TW5864_PCI_INTR_STATUS: 0x%04x, irqmask: 0x%04x%04x, irq status: 0x%04x%04x, TW5864_VLC_BUF: 0x%04x, TW5864_VLC_DSP_INTR: 0x%04x\n", tw_readw(TW5864_PCI_INTR_STATUS), tw_readw(TW5864_INTR_ENABLE_H), tw_readw(TW5864_INTR_ENABLE_L), tw_readw(TW5864_INTR_STATUS_H), tw_readw(TW5864_INTR_STATUS_L), tw_readw(TW5864_VLC_BUF), tw_readw(TW5864_VLC_DSP_INTR));
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
	tw_writew(TW5864_INTR_CLR_L, status & 0xffff);
	tw_writew(TW5864_INTR_CLR_H, status >> 16);

	pci_intr_status = tw_readw(TW5864_PCI_INTR_STATUS);
	tw_writew(TW5864_PCI_INTR_STATUS, pci_intr_status);

	pci_intr_ctl = tw_readl(TW5864_PCI_INTR_CTL);

	ddr_ctl_status = tw_readl(TW5864_DDR_CTL);

	dev_dbg(&dev->pci->dev, "tw5864_isr: status: 0x%08x, pci_status: 0x%08x, pci_ctl: 0x%08x, ddr_status: 0x%08x\n", status, pci_intr_status, pci_intr_ctl, ddr_ctl_status);

	// TODO Figure out the channel id of currently encoded frame

	if (status & TW5864_INTR_VLC_DONE) {
		u32 chunk[4];
		// TODO Grab encoded video data
		// TODO Move this block to -video.c

#define WTF "?!"
#ifdef WTF
		tw_writel(0x1807C, 4);
#endif
		vlc_len = tw_readl(TW5864_VLC_LENGTH) << 2;
		vlc_crc = tw_readl(TW5864_VLC_CRC_REG);
#ifdef WTF
		tw_writel(0x1807C, 0);
#endif
		channel = tw_readb(TW5864_DSP) & TW5864_DSP_ENC_CHN;
		vlc_reg = tw_readl(TW5864_VLC);
		vlc_buf_reg = tw_readl(TW5864_VLC_BUF);

		mv_reg = tw_readw(TW5864_MV);

		// switch between buffers 0 and 1 in a loop
		//tw_writew(TW5864_DSP_ENC_ORG_PTR_REG, tw_readw(TW5864_DSP_ENC_ORG_PTR_REG) ^ (1 << TW5864_DSP_ENC_ORG_PTR_SHIFT));

#if 0
		tw_writew(TW5864_DSP, (channel+1) % 4);
#endif

		tw_writel(TW5864_VLC_DSP_INTR, 1);  /* ack to hardware */
		tw_setl(TW5864_PCI_INTR_STATUS, TW5864_VLC_DONE_INTR);  /* another ack to hw */
		tw_setl(TW5864_VLC_BUF, vlc_buf_reg & 0x000f);  /* ack BK{0,1} full, end slice, buf overflow status */
		tw_setl(TW5864_PCI_INTR_CTL, TW5864_PCI_MAST_ENB | TW5864_MVD_VLC_MAST_ENB | TW5864_IIC_INTR_ENB | TW5864_AD_INTR_ENB | TW5864_PCI_TAR_BURST_ENB | TW5864_PCI_VLC_BURST_ENB | TW5864_PCI_DDR_BURST_ENB);
		tw_setl(TW5864_DDR, TW5864_DDR_BRST_EN);

		tw_writeb(TW5864_MV, mv_reg & 0x1f);  /* flags clearing */
		//tw_writew(0x0008, 0x0000);
		//tw_writew(0x0008, 0x0800);

		//tw_writew(TW5864_DSP, 0x0001  | TW5864_DSP_CHROM_SW |  /*mbdelay*/ ((0x430 >> 7) << 8) | TW5864_DSP_MB_WAIT );

		tw_writew(TW5864_SLICE, TW5864_START_NSLICE);
		tw_writew(TW5864_SLICE, 0);

		dev_dbg(&dev->pci->dev, "tw5864_isr: status: 0x%08x, channel 0x%08x, pci_intr_status 0x%08x, vlc_len %d, vlc_crc 0x%08x, vlc_buf_rdy 0x%02x, vlc_buf_reg 0x%08x, mv_reg 0x%04x\n", status, channel, pci_intr_status, vlc_len, vlc_crc, (vlc_reg & TW5864_VLC_BUF_RDY_MASK) >> TW5864_VLC_BUF_RDY_SHIFT, vlc_buf_reg, mv_reg);

		// TODO Swap DMA mapping
		dma_sync_single_for_cpu(&dev->pci->dev, dev->h264_vlc_buf[0].dma_addr, H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
		dma_sync_single_for_cpu(&dev->pci->dev, dev->h264_mv_buf[0].dma_addr, H264_MV_BUF_SIZE, DMA_FROM_DEVICE);

		for (i = 0; i < sizeof(chunk) / sizeof(chunk[0]); i++) {
			chunk[i] = tw_readl(TW5864_VLC_STREAM_MEM_START + i * 4);
		}
		dev_dbg(&dev->pci->dev, "tw5864_isr: TW5864_VLC_STREAM_MEM_START: hex: %08x %08x %08x %08x\n", chunk[0], chunk[1], chunk[2], chunk[3]);

		for (i = 0; i < sizeof(chunk) / sizeof(chunk[0]); i++) {
			chunk[i] = ((u32*)dev->h264_vlc_buf[0].addr)[i];
		}
		dev_dbg(&dev->pci->dev, "tw5864_isr: vlc buf: hex: %08x %08x %08x %08x\n", chunk[0], chunk[1], chunk[2], chunk[3]);

		dev_dbg(&dev->pci->dev, "CPU-computed CRC: %08x\n", 
				crc_check_sum((u32*)dev->h264_vlc_buf[0].addr, vlc_len));

		// TODO Do whatever needed, e.g. dump contents elsewhere
		dma_sync_single_for_device(&dev->pci->dev, dev->h264_vlc_buf[0].dma_addr, H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
		dma_sync_single_for_device(&dev->pci->dev, dev->h264_mv_buf[0].dma_addr, H264_MV_BUF_SIZE, DMA_FROM_DEVICE);

		tw_writew(TW5864_H264EN_CH_EN, 0x0000);
		tw_writew(TW5864_SEN_EN_CH, 0x0000);

		tw5864_video_init_reg_fucking(dev, NULL);


	} else
	if (status & TW5864_INTR_JPEG) {
		u32 jpg_len = tw_readl(0xD040 /* JPG_LENGTH(0) */);
		u32 chunk[4];

		// Clear burst done bit
		//tw_setl(TW5864_DDR_CTL, TW5864_BRST_END);

		for (i = 0; i < sizeof(chunk) / sizeof(chunk[0]); i++) {
			chunk[i] = tw_readl(TW5864_VLC_STREAM_MEM_START /*TW5864_DPR_BUF_START*/ + i * 4);
		}
		dev_dbg(&dev->pci->dev, "tw5864_isr: JPEG_IRQ 0x%02x, %d bytes\n", tw_readb(0xd010), jpg_len);
		dev_dbg(&dev->pci->dev, "tw5864_isr: JPEG: hopeful PIO: hex: %08x %08x %08x %08x\n", chunk[0], chunk[1], chunk[2], chunk[3]);

//#if 0
		// TODO request data transfer
		// select ddr b chip?
		// select sram page 9
		// sram addr 0x2000
		tw_writel(TW5864_DDR, 0x9 | TW5864_DDR_AB_SEL | TW5864_DDR_MODE);
		tw_writel(TW5864_DDR_ADDR, dev->jpeg_buf[0].dma_addr);
		tw_writew(TW5864_DPR_BUF_ADDR, TW5864_VLC_STREAM_MEM_START);
		tw_writel(TW5864_DDR_CTL, 0x100 /* len */ | 0 /* read, */ | TW5864_NEW_BRST_CMD | TW5864_SING_ERR_INTR | TW5864_BRST_ERR_INTR | TW5864_BRST_END_INTR);

		/*
		for (i = 0; i < 8; i++) {
			dma_sync_single_for_cpu(&dev->pci->dev, dev->jpeg_buf[i].dma_addr, H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
			if (((u32*)(dev->jpeg_buf[i].addr))[0] != 0)
				dev_dbg(&dev->pci->dev, "tw5864_isr: jpeg buf %d got modified!\n", i);
			dma_sync_single_for_device(&dev->pci->dev, dev->jpeg_buf[i].dma_addr, H264_MV_BUF_SIZE, DMA_FROM_DEVICE);
		}
		*/
//#endif

		tw_writeb(0xd010 /* JPEG_IRQ*/, 0xff);
		tw_writeb(0xd00c /* ENCODE_EN */, 0x1);
		tw_writeb(0xC800, 2);
		tw_writeb(0xC804, 2);
		tw_writeb(0xd014, 0x1);
		tw_writeb(0xd018, 0x1);
		tw_writeb(0xd0f8, 0x1);
		tw_writeb(0x18060, 0xff);
		tw_writel(0x18064, 0xffffffff);

		//tw_writew(TW5864_MASTER_ENB_REG, 0xffff /*TW5864_PCI_JPEG_INTR_ENB | TW5864_PCI_VLC_INTR_ENB | TW5864_PCI_PREV_INTR_ENB*/);

	} else
	if (status & TW5864_INTR_BURST) {
		u32 chunk[4];
		if (ddr_ctl_status & TW5864_BRST_END)
			tw_writel(TW5864_DDR_CTL, ddr_ctl_status);  // to clear TW5864_BRST_END flag

		for (i = 0; i < sizeof(chunk) / sizeof(chunk[0]); i++) {
			chunk[i] = tw_readl(TW5864_VLC_STREAM_MEM_START + i * 4);
		}
		dev_dbg(&dev->pci->dev, "tw5864_isr: TW5864_VLC_STREAM_MEM_START: hex: %08x %08x %08x %08x\n", chunk[0], chunk[1], chunk[2], chunk[3]);
	}

	//tw_setl(TW5864_PCI_INTR_CTL, 0xffffffff  & (~(TW5864_MVD_VLC_MAST_ENB )) );
	//tw_writew(TW5864_MPI_DDR_SEL_REG, TW5864_MPI_DDR_SEL2);


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

	for (reg_addr = 0x0; (count < size) && (reg_addr <= 0xEFE); reg_addr += 4) {
		value = tw_indir_readl(dev, reg_addr);
		count += scnprintf(buf + count, size - count,
				"indir[0x%03x] = 0x%08x\n", reg_addr, value);
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
	dev->mmio = ioremap_nocache(pci_resource_start(pci_dev, 0),
			     pci_resource_len(pci_dev, 0));
	if (NULL == dev->mmio) {
		err = -EIO;
		pr_err("%s: can't ioremap() MMIO memory\n", dev->name);
		goto ioremap_fail;
	}

	mutex_init(&dev->lock);
	spin_lock_init(&dev->slock);


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
