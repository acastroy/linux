/*
 *  tw5864 driver common header file
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

#include <linux/pci.h>
#include <linux/videodev2.h>
#include <linux/notifier.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/debugfs.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/videobuf2-dma-sg.h>

#include "tw5864-reg.h"

#define	UNSET	(-1U)

/* system vendor and device ID's */
#define	PCI_VENDOR_ID_TECHWELL 0x1797

/* tw5864 based cards */
#define	PCI_DEVICE_ID_5864     0x5864

#define TW5864_NORMS ( \
	V4L2_STD_NTSC    | V4L2_STD_PAL       | V4L2_STD_SECAM    | \
	V4L2_STD_PAL_M   | V4L2_STD_PAL_Nc    | V4L2_STD_PAL_60)

#if 0
#define	TW5864_VID_INTS	(TW5864_FFERR | TW5864_PABORT | TW5864_DMAPERR | \
			 TW5864_FFOF   | TW5864_DMAPI)
#define	TW5864_VID_INTSX	(TW5864_FDMIS | TW5864_HLOCK | TW5864_VLOCK)

#define	TW5864_I2C_INTS	(TW5864_SBERR | TW5864_SBDONE | TW5864_SBERR2  | \
			 TW5864_SBDONE2)
#endif

/* ----------------------------------------------------------- */
/* static data                                                 */

struct tw5864_tvnorm {
	char		*name;
	v4l2_std_id	id;

	/* video decoder */
	u32	sync_control;
	u32	luma_control;
	u32	chroma_ctrl1;
	u32	chroma_gain;
	u32	chroma_ctrl2;
	u32	vgate_misc;

	/* video scaler */
	u32	h_delay;
	u32	h_start;
	u32	h_stop;
	u32	v_delay;
	u32	video_v_start;
	u32	video_v_stop;
	u32	vbi_v_start_0;
	u32	vbi_v_stop_0;
	u32	vbi_v_start_1;

	/* Techwell specific */
	u32	format;
};

struct tw5864_format {
	char	*name;
	u32	fourcc;
	u32	depth;
	u32	twformat;
};

/* ----------------------------------------------------------- */
/* card configuration					  */

#define	TW5864_INPUTS 4

#define H264_VLC_BUF_SIZE 0x80000
#define H264_MV_BUF_SIZE 0x40000


/* ----------------------------------------------------------- */
/* device / file handle status                                 */

struct tw5864_dev;	/* forward delclaration */

/* buffer for one video/vbi/ts frame */
struct tw5864_buf {
	struct vb2_buffer vb;
	struct list_head list;

	unsigned int   size;
#if 0
	__le32         *cpu;
	__le32         *jmp;
	dma_addr_t     dma;
#endif
};

struct tw5864_fmt {
	char			*name;
	u32			fourcc;	/* v4l2 format id */
	int			depth;
	int			flags;
	u32			twformat;
};

/* bad name, TODO improve */
struct tw5864_recv_buf {
	void *addr;
	dma_addr_t dma_addr;
};


struct tw5864_input {
	int                     input_number;
	struct tw5864_dev       *root;
	struct mutex		lock;
	spinlock_t		slock;
	struct video_device	vdev;
	struct v4l2_ctrl_handler hdl;
	const struct tw5864_tvnorm *tvnorm;
	void			*alloc_ctx;
	struct vb2_queue	vidq;
	struct list_head	active;
	const struct tw5864_format *fmt;
	unsigned		width, height;
	unsigned		seqnr;
	unsigned		field;
};

/* global device status */
struct tw5864_dev {
	struct mutex		lock;
	spinlock_t		slock;
	struct v4l2_device	v4l2_dev;
	struct tw5864_input     inputs[TW5864_INPUTS];
#define H264_BUF_CNT 2
	struct tw5864_recv_buf       h264_vlc_buf[H264_BUF_CNT];
	struct tw5864_recv_buf       h264_mv_buf[H264_BUF_CNT];
	struct tw5864_recv_buf       jpeg_buf[8];

	/* TODO audio stuff */

	/* pci i/o */
	char			name[64];
	struct pci_dev		*pci;
	void                    __iomem *mmio;
	u32			irqmask;
	u32                     buf_id;
	u32                     long_timer_scenario_done;
	u32                     timers_with_vlc_disabled;
	u32                     frame_seqno;

	u32                     stored_len;

#define VLC_DUMP_CNT 64
	struct debugfs_blob_wrapper jpg, vlc[VLC_DUMP_CNT];

	struct dentry           *debugfs_dir;
};

/* ----------------------------------------------------------- */

#define tw_readl(reg) readl(dev->mmio + reg)
#define tw_readw(reg) readw(dev->mmio + reg)
#define	tw_readb(reg) readb(dev->mmio + reg)

#define tw_writel(reg, value) writel((value), dev->mmio + reg)
#define tw_writew(reg, value) writel((value), dev->mmio + reg)
#define	tw_writeb(reg, value) writel((value), dev->mmio + reg)
#if 0
#define tw_andorl(reg, mask, value) \
		tw_writel((reg), (tw_readl(reg) & ~(mask)) |\
		((value) & (mask)))
#define tw_andorw(reg, mask, value) \
		tw_writew((reg), (tw_readw(reg) & ~(mask)) |\
		((value) & (mask)))
#define tw_andorb(reg, mask, value) \
		tw_writeb((reg), (tw_readb(reg) & ~(mask)) |\
		((value) & (mask)))
#endif
#define	tw_setl(reg, bit)	tw_writel((reg), tw_readl(reg) | (bit))
#define	tw_setw(reg, bit)	tw_writel((reg), tw_readl(reg) | (bit))
#define	tw_setb(reg, bit)	tw_writel((reg), tw_readl(reg) | (bit))
#define	tw_clearl(reg, bit)	tw_writel((reg), tw_readl(reg) & ~(bit))
#define	tw_clearw(reg, bit)	tw_writel((reg), tw_readl(reg) & ~(bit))
#define	tw_clearb(reg, bit)	tw_writel((reg), tw_readl(reg) & ~(bit))
#define tw_wait(us) { udelay(us); }

static void tw_indir_writeb(struct tw5864_dev *dev, u16 addr, u8 data) {
	int timeout = 30000;
	addr <<= 2;

	while ((tw_readl(TW5864_IND_CTL) >> 31) && (timeout--))
		;
	if (!timeout)
		dev_err(&dev->pci->dev, "tw_indir_writel() timeout before writing\n");

	tw_writel(TW5864_IND_DATA, data);
	tw_writel(TW5864_IND_CTL, addr | TW5864_RW | TW5864_ENABLE);
}

static u8 tw_indir_readb(struct tw5864_dev *dev, u16 addr) {
	int timeout = 30000;
	u32 data = 0;
	addr <<= 2;

	while ((tw_readl(TW5864_IND_CTL) >> 31) && (timeout--))
		;
	if (!timeout)
		dev_err(&dev->pci->dev, "tw_indir_writel() timeout before reading\n");

	tw_writel(TW5864_IND_CTL, addr | TW5864_ENABLE);

	timeout = 30000;
	while ((tw_readl(TW5864_IND_CTL) >> 31) && (timeout--))
		;
	if (!timeout)
		dev_err(&dev->pci->dev, "tw_indir_writel() timeout at reading\n");

	data = tw_readl(TW5864_IND_DATA);
	return data & 0xff;
}

#include "w.c"

// Don't do any writes, so that we take intact regs dump
#if 0
#define tw_writel(reg, value)
#define tw_writew(reg, value)
#define	tw_writeb(reg, value)
#define	tw_setl(reg, bit)
#define	tw_setw(reg, bit)
#define	tw_setb(reg, bit)
#define	tw_clearl(reg, bit)
#define	tw_clearw(reg, bit)
#define	tw_clearb(reg, bit)
#endif
/* ----------------------------------------------------------- */
/* tw5864-video.c                                                */

void tw5864_set_tvnorm_hw(struct tw5864_dev *dev);

void pci_init_ad(struct tw5864_dev *dev);
int tw5864_video_init(struct tw5864_dev *dev, int *video_nr);
int tw5864_video_init_reg_fucking(struct tw5864_dev *dev, int *video_nr);
void tw5864_video_fini(struct tw5864_dev *dev);
int tw5864_video_irq(struct tw5864_dev *dev, unsigned long status);
#if 0
int tw5864_video_start_dma(struct tw5864_dev *dev, struct tw5864_buf *buf);

/* ----------------------------------------------------------- */
/* tw5864-risc.c                                                 */

int tw5864_risc_buffer(struct pci_dev *pci, struct tw5864_buf *buf,
	struct scatterlist *sglist, unsigned int top_offset,
	unsigned int bottom_offset, unsigned int bpl,
	unsigned int padding, unsigned int lines);
#endif
