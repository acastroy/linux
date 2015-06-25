/*
 *  tw5864 functions to handle video data
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

#include <linux/module.h>
#include <media/v4l2-common.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-dma-contig.h>
#include <linux/debugfs.h>

#include "tw5864.h"
#include "tw5864-reg.h"

#if 0

/* ------------------------------------------------------------------ */
/* data structs for video                                             */
/*
 * FIXME -
 * Note that the saa7134 has formats, e.g. YUV420, which are classified
 * as "planar".  These affect overlay mode, and are flagged with a field
 * ".planar" in the format.  Do we need to implement this in this driver?
 */
static const struct tw5864_format formats[] = {
	{
		.name		= "15 bpp RGB, le",
		.fourcc		= V4L2_PIX_FMT_RGB555,
		.depth		= 16,
		.twformat	= ColorFormatRGB15,
	}, {
		.name		= "15 bpp RGB, be",
		.fourcc		= V4L2_PIX_FMT_RGB555X,
		.depth		= 16,
		.twformat	= ColorFormatRGB15 | ColorFormatBSWAP,
	}, {
		.name		= "16 bpp RGB, le",
		.fourcc		= V4L2_PIX_FMT_RGB565,
		.depth		= 16,
		.twformat	= ColorFormatRGB16,
	}, {
		.name		= "16 bpp RGB, be",
		.fourcc		= V4L2_PIX_FMT_RGB565X,
		.depth		= 16,
		.twformat	= ColorFormatRGB16 | ColorFormatBSWAP,
	}, {
		.name		= "24 bpp RGB, le",
		.fourcc		= V4L2_PIX_FMT_BGR24,
		.depth		= 24,
		.twformat	= ColorFormatRGB24,
	}, {
		.name		= "24 bpp RGB, be",
		.fourcc		= V4L2_PIX_FMT_RGB24,
		.depth		= 24,
		.twformat	= ColorFormatRGB24 | ColorFormatBSWAP,
	}, {
		.name		= "32 bpp RGB, le",
		.fourcc		= V4L2_PIX_FMT_BGR32,
		.depth		= 32,
		.twformat	= ColorFormatRGB32,
	}, {
		.name		= "32 bpp RGB, be",
		.fourcc		= V4L2_PIX_FMT_RGB32,
		.depth		= 32,
		.twformat	= ColorFormatRGB32 | ColorFormatBSWAP |
				  ColorFormatWSWAP,
	}, {
		.name		= "4:2:2 packed, YUYV",
		.fourcc		= V4L2_PIX_FMT_YUYV,
		.depth		= 16,
		.twformat	= ColorFormatYUY2,
	}, {
		.name		= "4:2:2 packed, UYVY",
		.fourcc		= V4L2_PIX_FMT_UYVY,
		.depth		= 16,
		.twformat	= ColorFormatYUY2 | ColorFormatBSWAP,
	}
};
#define FORMATS ARRAY_SIZE(formats)

#define NORM_625_50			\
		.h_delay	= 3,	\
		.h_start	= 0,	\
		.h_stop		= 719,	\
		.v_delay	= 24,	\
		.vbi_v_start_0	= 7,	\
		.vbi_v_stop_0	= 22,	\
		.video_v_start	= 24,	\
		.video_v_stop	= 311,	\
		.vbi_v_start_1	= 319

#define NORM_525_60			\
		.h_delay	= 8,	\
		.h_start	= 0,	\
		.h_stop		= 719,	\
		.v_delay	= 22,	\
		.vbi_v_start_0	= 10,	\
		.vbi_v_stop_0	= 21,	\
		.video_v_start	= 22,	\
		.video_v_stop	= 262,	\
		.vbi_v_start_1	= 273

/*
 * The following table is searched by tw5864_s_std, first for a specific
 * match, then for an entry which contains the desired id.  The table
 * entries should therefore be ordered in ascending order of specificity.
 */
static const struct tw5864_tvnorm tvnorms[] = {
	{
		.name		= "PAL", /* autodetect */
		.id		= V4L2_STD_PAL,
		NORM_625_50,

		.sync_control	= 0x18,
		.luma_control	= 0x40,
		.chroma_ctrl1	= 0x81,
		.chroma_gain	= 0x2a,
		.chroma_ctrl2	= 0x06,
		.vgate_misc	= 0x1c,
		.format		= VideoFormatPALBDGHI,
	}, {
		.name		= "NTSC",
		.id		= V4L2_STD_NTSC,
		NORM_525_60,

		.sync_control	= 0x59,
		.luma_control	= 0x40,
		.chroma_ctrl1	= 0x89,
		.chroma_gain	= 0x2a,
		.chroma_ctrl2	= 0x0e,
		.vgate_misc	= 0x18,
		.format		= VideoFormatNTSC,
	}, {
		.name		= "SECAM",
		.id		= V4L2_STD_SECAM,
		NORM_625_50,

		.sync_control	= 0x18,
		.luma_control	= 0x1b,
		.chroma_ctrl1	= 0xd1,
		.chroma_gain	= 0x80,
		.chroma_ctrl2	= 0x00,
		.vgate_misc	= 0x1c,
		.format		= VideoFormatSECAM,
	}, {
		.name		= "PAL-M",
		.id		= V4L2_STD_PAL_M,
		NORM_525_60,

		.sync_control	= 0x59,
		.luma_control	= 0x40,
		.chroma_ctrl1	= 0xb9,
		.chroma_gain	= 0x2a,
		.chroma_ctrl2	= 0x0e,
		.vgate_misc	= 0x18,
		.format		= VideoFormatPALM,
	}, {
		.name		= "PAL-Nc",
		.id		= V4L2_STD_PAL_Nc,
		NORM_625_50,

		.sync_control	= 0x18,
		.luma_control	= 0x40,
		.chroma_ctrl1	= 0xa1,
		.chroma_gain	= 0x2a,
		.chroma_ctrl2	= 0x06,
		.vgate_misc	= 0x1c,
		.format		= VideoFormatPALNC,
	}, {
		.name		= "PAL-60",
		.id		= V4L2_STD_PAL_60,
		.h_delay	= 186,
		.h_start	= 0,
		.h_stop		= 719,
		.v_delay	= 26,
		.video_v_start	= 23,
		.video_v_stop	= 262,
		.vbi_v_start_0	= 10,
		.vbi_v_stop_0	= 21,
		.vbi_v_start_1	= 273,

		.sync_control	= 0x18,
		.luma_control	= 0x40,
		.chroma_ctrl1	= 0x81,
		.chroma_gain	= 0x2a,
		.chroma_ctrl2	= 0x06,
		.vgate_misc	= 0x1c,
		.format		= VideoFormatPAL60,
	}
};
#define TVNORMS ARRAY_SIZE(tvnorms)

static const struct tw5864_format *format_by_fourcc(unsigned int fourcc)
{
	unsigned int i;

	for (i = 0; i < FORMATS; i++)
		if (formats[i].fourcc == fourcc)
			return formats+i;
	return NULL;
}


/* ------------------------------------------------------------------ */
/*
 * Note that the cropping rectangles are described in terms of a single
 * frame, i.e. line positions are only 1/2 the interlaced equivalent
 */
static void set_tvnorm(struct tw5864_dev *dev, const struct tw5864_tvnorm *norm)
{
	if (norm != dev->tvnorm) {
		dev->width = 720;
		dev->height = (norm->id & V4L2_STD_525_60) ? 480 : 576;
		dev->tvnorm = norm;
		tw5864_set_tvnorm_hw(dev);
	}
}

/*
 * tw5864_set_scale
 *
 * Scaling and Cropping for video decoding
 *
 * We are working with 3 values for horizontal and vertical - scale,
 * delay and active.
 *
 * HACTIVE represent the actual number of pixels in the "usable" image,
 * before scaling.  HDELAY represents the number of pixels skipped
 * between the start of the horizontal sync and the start of the image.
 * HSCALE is calculated using the formula
 *	HSCALE = (HACTIVE / (#pixels desired)) * 256
 *
 * The vertical registers are similar, except based upon the total number
 * of lines in the image, and the first line of the image (i.e. ignoring
 * vertical sync and VBI).
 *
 * Note that the number of bytes reaching the FIFO (and hence needing
 * to be processed by the DMAP program) is completely dependent upon
 * these values, especially HSCALE.
 *
 * Parameters:
 *	@dev		pointer to the device structure, needed for
 *			getting current norm (as well as debug print)
 *	@width		actual image width (from user buffer)
 *	@height		actual image height
 *	@field		indicates Top, Bottom or Interlaced
 */
static int tw5864_set_scale(struct tw5864_dev *dev, unsigned int width,
			  unsigned int height, enum v4l2_field field)
{
	const struct tw5864_tvnorm *norm = dev->tvnorm;
	/* set individually for debugging clarity */
	int hactive, hdelay, hscale;
	int vactive, vdelay, vscale;
	int comb;

	if (V4L2_FIELD_HAS_BOTH(field))	/* if field is interlaced */
		height /= 2;		/* we must set for 1-frame */

	pr_debug("%s: width=%d, height=%d, both=%d\n"
		 "  tvnorm h_delay=%d, h_start=%d, h_stop=%d, "
		 "v_delay=%d, v_start=%d, v_stop=%d\n" , __func__,
		width, height, V4L2_FIELD_HAS_BOTH(field),
		norm->h_delay, norm->h_start, norm->h_stop,
		norm->v_delay, norm->video_v_start,
		norm->video_v_stop);

	switch (dev->vdecoder) {
	default:
		hdelay = norm->h_delay;
		break;
	}

	hdelay += norm->h_start;
	hactive = norm->h_stop - norm->h_start + 1;

	hscale = (hactive * 256) / (width);

	vdelay = norm->v_delay;
	vactive = ((norm->id & V4L2_STD_525_60) ? 524 : 624) / 2 - norm->video_v_start;
	vscale = (vactive * 256) / height;

	pr_debug("%s: %dx%d [%s%s,%s]\n", __func__,
		width, height,
		V4L2_FIELD_HAS_TOP(field)    ? "T" : "",
		V4L2_FIELD_HAS_BOTTOM(field) ? "B" : "",
		v4l2_norm_to_name(dev->tvnorm->id));
	pr_debug("%s: hactive=%d, hdelay=%d, hscale=%d; "
		"vactive=%d, vdelay=%d, vscale=%d\n", __func__,
		hactive, hdelay, hscale, vactive, vdelay, vscale);

	comb =	((vdelay & 0x300)  >> 2) |
		((vactive & 0x300) >> 4) |
		((hdelay & 0x300)  >> 6) |
		((hactive & 0x300) >> 8);
	pr_debug("%s: setting CROP_HI=%02x, VDELAY_LO=%02x, "
		"VACTIVE_LO=%02x, HDELAY_LO=%02x, HACTIVE_LO=%02x\n",
		__func__, comb, vdelay, vactive, hdelay, hactive);
	tw_writeb(TW5864_CROP_HI, comb);
	tw_writeb(TW5864_VDELAY_LO, vdelay & 0xff);
	tw_writeb(TW5864_VACTIVE_LO, vactive & 0xff);
	tw_writeb(TW5864_HDELAY_LO, hdelay & 0xff);
	tw_writeb(TW5864_HACTIVE_LO, hactive & 0xff);

	comb = ((vscale & 0xf00) >> 4) | ((hscale & 0xf00) >> 8);
	pr_debug("%s: setting SCALE_HI=%02x, VSCALE_LO=%02x, "
		"HSCALE_LO=%02x\n", __func__, comb, vscale, hscale);
	tw_writeb(TW5864_SCALE_HI, comb);
	tw_writeb(TW5864_VSCALE_LO, vscale);
	tw_writeb(TW5864_HSCALE_LO, hscale);

	return 0;
}

/* ------------------------------------------------------------------ */

int tw5864_video_start_dma(struct tw5864_dev *dev, struct tw5864_buf *buf)
{
	/* Set cropping and scaling */
	tw5864_set_scale(dev, dev->width, dev->height, dev->field);
	/*
	 *  Set start address for RISC program.  Note that if the DMAP
	 *  processor is currently running, it must be stopped before
	 *  a new address can be set.
	 */
	tw_clearl(TW5864_DMAC, TW5864_DMAP_EN);
	tw_writel(TW5864_DMAP_SA, buf->dma);
	/* Clear any pending interrupts */
	tw_writel(TW5864_INTSTAT, dev->board_virqmask);
	/* Enable the risc engine and the fifo */
	tw_andorl(TW5864_DMAC, 0xff, dev->fmt->twformat |
		ColorFormatGamma | TW5864_DMAP_EN | TW5864_FIFO_EN);
	dev->pci_irqmask |= dev->board_virqmask;
	tw_setl(TW5864_INTMASK, dev->pci_irqmask);
	return 0;
}

/* ------------------------------------------------------------------ */

/* calc max # of buffers from size (must not exceed the 4MB virtual
 * address space per DMA channel) */
static int tw5864_buffer_count(unsigned int size, unsigned int count)
{
	unsigned int maxcount;

	maxcount = (4 * 1024 * 1024) / roundup(size, PAGE_SIZE);
	if (count > maxcount)
		count = maxcount;
	return count;
}

#endif
/* ------------------------------------------------------------- */
/* vb2 queue operations                                          */

static int tw5864_queue_setup(struct vb2_queue *q, const struct v4l2_format *fmt,
			   unsigned int *num_buffers, unsigned int *num_planes,
			   unsigned int sizes[], void *alloc_ctxs[])
{
	struct tw5864_input *dev = vb2_get_drv_priv(q);
#if 0
	unsigned tot_bufs = q->num_buffers + *num_buffers;
#endif

	// TODO FIXME Estimate max needed H264 frame size from board buffers size and so on
	sizes[0] = 1024 * 1024;  // 1 megabyte
	alloc_ctxs[0] = dev->alloc_ctx;
	*num_planes = 1;

	if (*num_buffers < 2)
		*num_buffers = 2;
#if 0
	// Seems overly complex
	/*
	 * We allow create_bufs, but only if the sizeimage is the same as the
	 * current sizeimage. The tw5864_buffer_count calculation becomes quite
	 * difficult otherwise.
	 */
	if (fmt && fmt->fmt.pix.sizeimage < sizes[0])
		return -EINVAL;
	if (tot_bufs < 2)
		tot_bufs = 2;
	tot_bufs = tw5864_buffer_count(sizes[0], tot_bufs);
	*num_buffers = tot_bufs - q->num_buffers;
#endif

	return 0;
}

static void tw5864_buf_queue(struct vb2_buffer *vb)
{
	struct vb2_queue *vq = vb->vb2_queue;
	struct tw5864_input *dev = vb2_get_drv_priv(vq);
	struct tw5864_buf *buf = container_of(vb, struct tw5864_buf, vb);
	unsigned long flags;

	spin_lock_irqsave(&dev->slock, flags);  // TODO FIXME which locking is minimal and sufficient?
	list_add_tail(&buf->list, &dev->active);
	spin_unlock_irqrestore(&dev->slock, flags);
}

static void tw5864_buf_finish(struct vb2_buffer *vb)
{
	struct vb2_queue *vq = vb->vb2_queue;
	struct tw5864_input *dev = vb2_get_drv_priv(vq);
	struct tw5864_buf *buf = container_of(vb, struct tw5864_buf, vb);
	// What TODO?
}

static int tw5864_enable_input(struct tw5864_dev *dev, int input_number) {
	dev_dbg(&dev->pci->dev, "enabling channel %d\n", input_number);
	mutex_lock(&dev->lock);
	//tw_setw(TW5864_H264EN_CH_EN, 1 << input_number);
	//tw_setw(TW5864_SEN_EN_CH, 1 << input_number);
	tw_setw(TW5864_H264EN_CH_EN, 0xffff);

	mutex_unlock(&dev->lock);
	dev_dbg(&dev->pci->dev, "status: 0x%04x\n", tw_readw(TW5864_H264EN_CH_STATUS));
	dev_dbg(&dev->pci->dev, "TW5864_PCI_INTR_STATUS: 0x%04x, irqmask: 0x%04x%04x, irq status: 0x%04x%04x, TW5864_VLC_BUF: 0x%04x, TW5864_VLC_DSP_INTR: 0x%04x\n", tw_readw(TW5864_PCI_INTR_STATUS), tw_readw(TW5864_INTR_ENABLE_H), tw_readw(TW5864_INTR_ENABLE_L), tw_readw(TW5864_INTR_STATUS_H), tw_readw(TW5864_INTR_STATUS_L), tw_readw(TW5864_VLC_BUF), tw_readw(TW5864_VLC_DSP_INTR));
	return 0;
}

static int tw5864_disable_input(struct tw5864_dev *dev, int input_number) {
	dev_dbg(&dev->pci->dev, "disabling channel %d\n", input_number);
	mutex_lock(&dev->lock);
	//tw_clearw(TW5864_SEN_EN_CH, 1 << input_number);
	//tw_clearw(TW5864_H264EN_CH_EN, 1 << input_number);
	tw_clearw(TW5864_H264EN_CH_EN, 0xffff);
	tw_clearw(TW5864_SEN_EN_CH, 0xffff);
	mutex_unlock(&dev->lock);
	dev_dbg(&dev->pci->dev, "status: 0x%04x\n", tw_readw(TW5864_H264EN_CH_STATUS));
	dev_dbg(&dev->pci->dev, "TW5864_PCI_INTR_STATUS: 0x%04x, irqmask: 0x%04x%04x, irq status: 0x%04x%04x, TW5864_VLC_BUF: 0x%04x, TW5864_VLC_DSP_INTR: 0x%04x\n", tw_readw(TW5864_PCI_INTR_STATUS), tw_readw(TW5864_INTR_ENABLE_H), tw_readw(TW5864_INTR_ENABLE_L), tw_readw(TW5864_INTR_STATUS_H), tw_readw(TW5864_INTR_STATUS_L), tw_readw(TW5864_VLC_BUF), tw_readw(TW5864_VLC_DSP_INTR));
	return 0;
}

static int tw5864_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct tw5864_input *dev = vb2_get_drv_priv(q);

	dev->seqnr = 0;
	tw5864_enable_input(dev->root, dev->input_number);
	return 0;
}

static void tw5864_stop_streaming(struct vb2_queue *q)
{
	struct tw5864_input *dev = vb2_get_drv_priv(q);

	tw5864_disable_input(dev->root, dev->input_number);

	while (!list_empty(&dev->active)) {
		struct tw5864_buf *buf =
			container_of(dev->active.next, struct tw5864_buf, list);

		list_del(&buf->list);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}
}

static struct vb2_ops tw5864_video_qops = {
	.queue_setup	= tw5864_queue_setup,
	.buf_queue	= tw5864_buf_queue,
	.buf_finish	= tw5864_buf_finish,
	.start_streaming = tw5864_start_streaming,
	.stop_streaming = tw5864_stop_streaming,
	.wait_prepare	= vb2_ops_wait_prepare,
	.wait_finish	= vb2_ops_wait_finish,
};

/* ------------------------------------------------------------------ */
static int tw5864_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct tw5864_input *dev =
		container_of(ctrl->handler, struct tw5864_input, hdl);

	// TODO
#if 0
	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		tw_writeb(TW5864_BRIGHT, ctrl->val);
		break;
	case V4L2_CID_HUE:
		tw_writeb(TW5864_HUE, ctrl->val);
		break;
	case V4L2_CID_CONTRAST:
		tw_writeb(TW5864_CONTRAST, ctrl->val);
		break;
	case V4L2_CID_SATURATION:
		tw_writeb(TW5864_SAT_U, ctrl->val);
		tw_writeb(TW5864_SAT_V, ctrl->val);
		break;
	case V4L2_CID_COLOR_KILLER:
		if (ctrl->val)
			tw_andorb(TW5864_MISC2, 0xe0, 0xe0);
		else
			tw_andorb(TW5864_MISC2, 0xe0, 0x00);
		break;
	case V4L2_CID_CHROMA_AGC:
		if (ctrl->val)
			tw_andorb(TW5864_LOOP, 0x30, 0x20);
		else
			tw_andorb(TW5864_LOOP, 0x30, 0x00);
		break;
	}
#endif
	return 0;
}
/* ------------------------------------------------------------------ */

static int tw5864_g_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct tw5864_input *dev = video_drvdata(file);

	// TODO get info from registers
/*
 * Note that this routine returns what is stored in the fh structure, and
 * does not interrogate any of the device registers.
 */
	f->fmt.pix.width        = 720; //dev->width;
	f->fmt.pix.height       = 480; //dev->height;
	//f->fmt.pix.field        = dev->field;
	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_H264; //dev->fmt->fourcc;
	//f->fmt.pix.bytesperline =
	//	(f->fmt.pix.width * (dev->fmt->depth)) >> 3;
	f->fmt.pix.sizeimage = H264_VLC_BUF_SIZE;
	//	f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace	= V4L2_COLORSPACE_SMPTE170M;
	f->fmt.pix.priv = 0;
	return 0;
}

static int tw5864_try_fmt_vid_cap(struct file *file, void *priv,
						struct v4l2_format *f)
{
	// TODO
	f->fmt.pix.width = 720;
	f->fmt.pix.height = 480;
#if 0
	struct tw5864_input *dev = video_drvdata(file);
	const struct tw5864_format *fmt;
	enum v4l2_field field;
	unsigned int maxh;

	fmt = format_by_fourcc(f->fmt.pix.pixelformat);
	if (NULL == fmt)
		return -EINVAL;

	field = f->fmt.pix.field;
	maxh  = (dev->tvnorm->id & V4L2_STD_525_60) ? 480 : 576;

	switch (field) {
	case V4L2_FIELD_TOP:
	case V4L2_FIELD_BOTTOM:
		break;
	case V4L2_FIELD_INTERLACED:
	case V4L2_FIELD_SEQ_BT:
	case V4L2_FIELD_SEQ_TB:
		maxh = maxh * 2;
		break;
	default:
		field = (f->fmt.pix.height > maxh / 2)
			? V4L2_FIELD_INTERLACED
			: V4L2_FIELD_BOTTOM;
		break;
	}

	f->fmt.pix.field = field;
	if (f->fmt.pix.width  < 48)
		f->fmt.pix.width  = 48;
	if (f->fmt.pix.height < 32)
		f->fmt.pix.height = 32;
	if (f->fmt.pix.width > 720)
		f->fmt.pix.width = 720;
	if (f->fmt.pix.height > maxh)
		f->fmt.pix.height = maxh;
	f->fmt.pix.width &= ~0x03;
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * (fmt->depth)) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
	f->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
#endif
	return 0;
}

/*
 * Note that tw5864_s_fmt_vid_cap sets the information into the fh structure,
 * and it will be used for all future new buffers.  However, there could be
 * some number of buffers on the "active" chain which will be filled before
 * the change takes place.
 */
static int tw5864_s_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct tw5864_input *dev = video_drvdata(file);
	int err;

	err = tw5864_try_fmt_vid_cap(file, priv, f);
	if (0 != err)
		return err;

	//dev->fmt = format_by_fourcc(f->fmt.pix.pixelformat);  // TODO FIXME
	dev->width = f->fmt.pix.width;
	dev->height = f->fmt.pix.height;
	dev->field = f->fmt.pix.field;
	return 0;
}

static int tw5864_enum_input(struct file *file, void *priv,
					struct v4l2_input *i)
{
	struct tw5864_input *dev = video_drvdata(file);

	if (i->index)
		return -EINVAL;

	i->type = V4L2_INPUT_TYPE_CAMERA;
	snprintf(i->name, sizeof(i->name), "Encoder %d", dev->input_number);

	// TODO Fill i->std
	i->std = V4L2_STD_NTSC_M;
	// TODO Fill i->status
#if 0
	/* If the query is for the current input, get live data */
	if (n == dev->input) {
		int v1 = tw_readb(TW5864_STATUS1);
		int v2 = tw_readb(TW5864_MVSN);

		if (0 != (v1 & (1 << 7)))
			i->status |= V4L2_IN_ST_NO_SYNC;
		if (0 != (v1 & (1 << 6)))
			i->status |= V4L2_IN_ST_NO_H_LOCK;
		if (0 != (v1 & (1 << 2)))
			i->status |= V4L2_IN_ST_NO_SIGNAL;
		if (0 != (v1 & 1 << 1))
			i->status |= V4L2_IN_ST_NO_COLOR;
		if (0 != (v2 & (1 << 2)))
			i->status |= V4L2_IN_ST_MACROVISION;
	}
	i->std = video_devdata(file)->tvnorms;
#endif
	return 0;
}

static int tw5864_g_input(struct file *file, void *priv, unsigned int *i)
{
	*i = 0;
	return 0;
}

static int tw5864_s_input(struct file *file, void *priv, unsigned int i)
{
	if (i)
		return -EINVAL;
	return 0;
}

static int tw5864_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{
	struct tw5864_input *dev = video_drvdata(file);

	strcpy(cap->driver, "tw5864");
	snprintf(cap->card, sizeof(cap->card), "TW5864 Encoder %d",
			dev->input_number);
	sprintf(cap->bus_info, "PCI:%s", pci_name(dev->root->pci));
	cap->device_caps =
		V4L2_CAP_VIDEO_CAPTURE |
		V4L2_CAP_READWRITE |
		V4L2_CAP_STREAMING;

	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int tw5864_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct tw5864_input *dev = video_drvdata(file);
	unsigned int i;

	if (vb2_is_busy(&dev->vidq))
		return -EBUSY;

	// TODO FIXME
#if 0
	/* Look for match on complete norm id (may have mult bits) */
	for (i = 0; i < TVNORMS; i++) {
		if (id == tvnorms[i].id)
			break;
	}

	/* If no exact match, look for norm which contains this one */
	if (i == TVNORMS) {
		for (i = 0; i < TVNORMS; i++)
			if (id & tvnorms[i].id)
				break;
	}
	/* If still not matched, give up */
	if (i == TVNORMS)
		return -EINVAL;

	set_tvnorm(dev, &tvnorms[i]);	/* do the actual setting */
#endif
	return 0;
}

static int tw5864_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct tw5864_input *dev = video_drvdata(file);

	// TODO FIXME get from registers
	*id = V4L2_STD_NTSC_M;
	return 0;
}

static int tw5864_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index)
		return -EINVAL;

	f->pixelformat = V4L2_PIX_FMT_H264;
	strcpy(f->description, "H.264");

	return 0;
}

#if 0
/*
 * Used strictly for internal development and debugging, this routine
 * prints out the current register contents for the tw5864xx device.
 */
static void tw5864_dump_regs(struct tw5864_dev *dev)
{
	unsigned char line[80];
	int i, j, k;
	unsigned char *cptr;

	pr_info("Full dump of TW5864 registers:\n");
	/* First we do the PCI regs, 8 4-byte regs per line */
	for (i = 0; i < 0x100; i += 32) {
		cptr = line;
		cptr += sprintf(cptr, "%03x  ", i);
		/* j steps through the next 4 words */
		for (j = i; j < i + 16; j += 4)
			cptr += sprintf(cptr, "%08x ", tw_readl(j));
		*cptr++ = ' ';
		for (; j < i + 32; j += 4)
			cptr += sprintf(cptr, "%08x ", tw_readl(j));
		*cptr++ = '\n';
		*cptr = 0;
		pr_info("%s", line);
	}
	/* Next the control regs, which are single-byte, address mod 4 */
	while (i < 0x400) {
		cptr = line;
		cptr += sprintf(cptr, "%03x ", i);
		/* Print out 4 groups of 4 bytes */
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 4; k++) {
				cptr += sprintf(cptr, "%02x ",
					tw_readb(i));
				i += 4;
			}
			*cptr++ = ' ';
		}
		*cptr++ = '\n';
		*cptr = 0;
		pr_info("%s", line);
	}
}

static int vidioc_log_status(struct file *file, void *priv)
{
	struct tw5864_input *dev = video_drvdata(file);

	tw5864_dump_regs(dev);
	return v4l2_ctrl_log_status(file, priv);
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int vidioc_g_register(struct file *file, void *priv,
			      struct v4l2_dbg_register *reg)
{
	struct tw5864_input *dev = video_drvdata(file);

	if (reg->size == 1)
		reg->val = tw_readb(reg->reg);
	else
		reg->val = tw_readl(reg->reg);
	return 0;
}

static int vidioc_s_register(struct file *file, void *priv,
				const struct v4l2_dbg_register *reg)
{
	struct tw5864_input *dev = video_drvdata(file);

	if (reg->size == 1)
		tw_writeb(reg->reg, reg->val);
	else
		tw_writel(reg->reg & 0xffff, reg->val);
	return 0;
}
#endif

#endif
static const struct v4l2_ctrl_ops tw5864_ctrl_ops = {
	.s_ctrl = tw5864_s_ctrl,
};

static const struct v4l2_file_operations video_fops = {
	.owner			= THIS_MODULE,
	.open			= v4l2_fh_open,
	.release		= vb2_fop_release,
	.read			= vb2_fop_read,
	.poll			= vb2_fop_poll,
	.mmap			= vb2_fop_mmap,
	.unlocked_ioctl		= video_ioctl2,
};

static int vb2_ioctl_dqbuf_proxy(struct file *file, void *priv, struct v4l2_buffer *p) {
	int ret;
	struct tw5864_input *dev = video_drvdata(file);
	dev_dbg(&dev->root->pci->dev, "calling vb2_ioctl_dqbuf\n");
	ret = vb2_ioctl_dqbuf(file, priv, p);
	dev_dbg(&dev->root->pci->dev, "vb2_ioctl_dqbuf ret %d\n", ret);
	return ret;
}

static const struct v4l2_ioctl_ops video_ioctl_ops = {
	.vidioc_querycap		= tw5864_querycap,
	.vidioc_enum_fmt_vid_cap	= tw5864_enum_fmt_vid_cap,
	.vidioc_reqbufs			= vb2_ioctl_reqbufs,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf_proxy,
	.vidioc_s_std			= tw5864_s_std,
	.vidioc_g_std			= tw5864_g_std,
	.vidioc_enum_input		= tw5864_enum_input,
	.vidioc_g_input			= tw5864_g_input,
	.vidioc_s_input			= tw5864_s_input,
	.vidioc_streamon		= vb2_ioctl_streamon,
	.vidioc_streamoff		= vb2_ioctl_streamoff,
	.vidioc_g_fmt_vid_cap		= tw5864_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap		= tw5864_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap		= tw5864_s_fmt_vid_cap,
	.vidioc_log_status		= v4l2_ctrl_log_status,
	.vidioc_subscribe_event		= v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event	= v4l2_event_unsubscribe,
#if 0
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_register              = vidioc_g_register,
	.vidioc_s_register              = vidioc_s_register,
#endif
#endif
};
static struct video_device tw5864_video_template = {
	.name			= "tw5864_video",
	.fops			= &video_fops,
	.ioctl_ops		= &video_ioctl_ops,
	.release		= video_device_release_empty,
	.tvnorms		= TW5864_NORMS,
};
#if 0
/* ------------------------------------------------------------------ */
/* exported stuff                                                     */
void tw5864_set_tvnorm_hw(struct tw5864_dev *dev)
{
	tw_andorb(TW5864_SDT, 0x07, dev->tvnorm->format);
}
#endif
static int tw5864_video_input_init(struct tw5864_input *dev, int video_nr);
static void tw5864_video_input_fini(struct tw5864_input *dev);

int tw5864_video_init(struct tw5864_dev *dev, int *video_nr)
{
	int i;
	int j;
	int ret;
	static struct debugfs_blob_wrapper jpg, vlc;

	for (i = 0; i < TW5864_INPUTS; i++) {
		dev->inputs[i].root = dev;
		dev->inputs[i].input_number = i;
		ret = tw5864_video_input_init(&dev->inputs[i], video_nr[i]);
		if (ret)
			goto input_init_fail;
	}

	for (i = 0; i < H264_BUF_CNT; i++) {
		dev->h264_vlc_buf[i].addr = dma_alloc_coherent(&dev->pci->dev, H264_VLC_BUF_SIZE, &dev->h264_vlc_buf[i].dma_addr, GFP_KERNEL|GFP_DMA32);
		dev->h264_mv_buf[i].addr = dma_alloc_coherent(&dev->pci->dev, H264_MV_BUF_SIZE, &dev->h264_mv_buf[i].dma_addr, GFP_KERNEL|GFP_DMA32);
		if (!dev->h264_vlc_buf[i].addr || !dev->h264_mv_buf[i].addr) {
			dev_err(&dev->pci->dev, "dma alloc & map fail: %p %p\n", dev->h264_vlc_buf[i].addr, dev->h264_mv_buf[i].addr);
			goto dma_alloc_fail;
		}
	}
	for (i = 0; i < 8; i++) {
		dev->jpeg_buf[i].addr = dma_alloc_coherent(&dev->pci->dev, H264_VLC_BUF_SIZE, &dev->jpeg_buf[i].dma_addr, GFP_KERNEL|GFP_DMA32);
	}

	// TODO Setup a mask of interrupts needed for video subsystem

	/* Hardware configuration */

	tw_writel(TW5864_VLC_STREAM_BASE_ADDR, dev->h264_vlc_buf[0].dma_addr);
	tw_writel(TW5864_MV_STREAM_BASE_ADDR, dev->h264_mv_buf[0].dma_addr);

	tw_writel(TW5864_VLC_MAX_LENGTH, H264_VLC_BUF_SIZE);
	tw_writew(TW5864_MPI_DDR_SEL_REG, TW5864_MPI_DDR_SEL2);

#if 1 // VLC disable
	tw_writel(TW5864_VLC, TW5864_VLC_PCI_SEL | (1 << 23) /* ENABLE_VLC_MVD */ | TW5864_VLC_OVFL_CNTL | /* QP */0x0012 );
#endif
	tw_writew(TW5864_DDR, TW5864_DDR_BRST_EN | TW5864_DDR_MODE);
	//tw_setl(TW5864_PCI_INTR_CTL, TW5864_PCI_MAST_ENB | TW5864_MVD_VLC_MAST_ENB | TW5864_PCI_VLC_BURST_ENB);
	tw_writel(TW5864_PCI_INTR_CTL, 0xffffffff /*& (~(TW5864_MVD_VLC_MAST_ENB ))*/   /*& (~(TW5864_TIMER_INTR_ENB | TW5864_JPEG_MAST_ENB))*/);
#if 1 // VLC disable
	//tw_setw(TW5864_MASTER_ENB_REG, TW5864_PCI_VLC_INTR_ENB);
	
#if 0 // VLC dis
	tw_setw(TW5864_MASTER_ENB_REG, 0xffff);
#endif
	//tw_setw(TW5864_MASTER_ENB_REG, 0xffff/*TW5864_PCI_JPEG_INTR_ENB | TW5864_PCI_VLC_INTR_ENB | TW5864_PCI_PREV_INTR_ENB*/);

	tw_writew(0x0008, 0);
	tw_writeb(TW5864_EMU_EN_VARIOUS_ETC, (TW5864_DSP_FRAME_TYPE & (1 << 6)) | 0x1f);
	tw_writew(0x0008, 0x0800);
	tw_writew(TW5864_SLICE, TW5864_MAS_SLICE_END | TW5864_START_NSLICE);
#endif
	tw_setb(TW5864_IIC_ENB, 1);
	tw_writeb(TW5864_I2C_PHASE_CFG, 1);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			// NTSC_MAP[1] as we have 1 FPS
			tw_writew(TW5864_H264EN_RATE_CNTL_LO_WORD(i, i * 4 + j), 0x20000000 & 0xffff);
			tw_writew(TW5864_H264EN_RATE_CNTL_HI_WORD(i, i * 4 + j), 0x20000000 >> 16);
		}
	}

#if 1
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			tw_writew(TW5864_H264EN_RATE_CNTL_LO_WORD(i, i * 4 + j), 1);
			tw_writew(TW5864_H264EN_RATE_CNTL_HI_WORD(i, i * 4 + j), 0);
		}
	}
#endif

#if 1 // VLC
	tw_writew(TW5864_H264EN_BUS0_MAP, 0x3210);
	tw_writew(TW5864_H264EN_BUS1_MAP, 0x7654);
	tw_writew(TW5864_H264EN_BUS2_MAP, 0xBA98);
	tw_writew(TW5864_H264EN_BUS3_MAP, 0xFEDC);

	tw_writew(TW5864_FRAME_BUS1, ((2 << 3) | TW5864_FRAME | (2 << (8 + 3)) | TW5864_FRAME << 8));  // 2D1 for bus 0 & 1
	tw_writew(TW5864_FRAME_BUS2, ((2 << 3) | TW5864_FRAME | (2 << (8 + 3)) | TW5864_FRAME << 8));  // 2D1 for bus 2 & 3

	for (i = 0; i < 4; i++) {
		tw_writew(TW5864_FRAME_WIDTH_BUS_A(i), 0x2CF); // 2D1
		tw_writew(TW5864_FRAME_WIDTH_BUS_B(i), 0x2CF); // 2D1

		tw_writew(TW5864_FRAME_HEIGHT_BUS_A(i), 0x1DF); // 2D1 NTSC
		tw_writew(TW5864_FRAME_HEIGHT_BUS_B(i), 0x1DF); // 2D1 NTSC
	}

	tw_writew(TW5864_FULL_HALF_FLAG, 0xffff);
	tw_writew(TW5864_INTERLACING, TW5864_DSP_INTER_ST | TW5864_DI_EN /* | TW5864_DUAL_STR? */);

#define FPS 1
	tw_writew(TW5864_H264EN_RATE_MAX_LINE_REG1, (FPS << TW5864_H264EN_RATE_MAX_LINE_ODD_SHIFT) | FPS);
	tw_writew(TW5864_H264EN_RATE_MAX_LINE_REG2, (FPS << TW5864_H264EN_RATE_MAX_LINE_ODD_SHIFT) | FPS);

	tw_writew(TW5864_DSP_CODEC, TW5864_CIF_MAP_MD | TW5864_HD1_MAP_MD);
	tw_writew(TW5864_MOTION_SEARCH_ETC, TW5864_ME_EN | TW5864_INTRA_EN);
	tw_writew(TW5864_DSP_INTRA_MODE, 0x06 << TW5864_DSP_INTRA_MODE_SHIFT);
	
	tw_writew(TW5864_H264EN_CH_EN, 0xffff);
	tw_writew(TW5864_SEN_EN_CH, 0xffff);

	tw_writew(TW5864_VLC_RD, TW5864_VLC_RD_BRST);

	tw_writew(TW5864_DSP, 0x0f);

	tw_writeb(TW5864_H264EN_BUS_MAX_CH, 0x0/*f*/);
#endif // VLC

	tw_writew(TW5864_PCI_PV_CH_EN, 0xffff);

	tw_writeb(0xD014, 1);
	tw_writeb(0xD018, 1);
	tw_writeb(0xC800, 2);
	tw_writeb(0xC804, 2);
	tw_writeb(0xD00C, 1); // jpeg enc enable bit
	tw_writeb(0xD0F8, 1); // pci master enable

	tw_writel(0x180c0, dev->jpeg_buf[0].dma_addr);
	tw_writel(0x180c4, dev->jpeg_buf[1].dma_addr);
	tw_writel(0x180c8, dev->jpeg_buf[2].dma_addr);
	tw_writel(0x180cc, dev->jpeg_buf[3].dma_addr);

	tw_writel(0x180d0, dev->jpeg_buf[4].dma_addr);
	tw_writel(0x180d4, dev->jpeg_buf[5].dma_addr);
	tw_writel(0x180d8, dev->jpeg_buf[6].dma_addr);
	tw_writel(0x180dc, dev->jpeg_buf[7].dma_addr);

	tw_writel(0x18050, H264_VLC_BUF_SIZE);
	tw_writel(0xd000, 0x12); // quantization parameter
	tw_writeb(0x18060, 0xff);
	tw_writel(0x18064, 0xffffffff);

	jpg.data = dev->jpeg_buf[0].addr;
	jpg.size = 0x1000;
	
	if (!debugfs_create_blob("jpg", S_IRUGO, dev->debugfs_dir, &jpg)) {
		dev_err(&dev->pci->dev, "jpg debugfs blob creation failed\n");
		return 1;
	}

	vlc.data = dev->h264_vlc_buf[0].addr;
	vlc.size = 0x1000;
	if (!debugfs_create_blob("vlc", S_IRUGO, dev->debugfs_dir, &vlc)) {
		dev_err(&dev->pci->dev, "vlc debugfs blob creation failed\n");
		return 1;
	}

	return 0;

dma_alloc_fail:
	;// TODO Free allocated

	i = TW5864_INPUTS;

input_init_fail:
	for (; i >= 0; i--)
		tw5864_video_input_fini(&dev->inputs[i]);

	return ret;
}

static int tw5864_video_input_init(struct tw5864_input *dev, int video_nr)
{
	int i;
	int ret;
	struct v4l2_ctrl_handler *hdl = &dev->hdl;

	mutex_init(&dev->lock);
	spin_lock_init(&dev->slock);

#if 0  /* try to get these settings from registers directly to check actual defaults */
	set_tvnorm(dev, &tvnorms[0]);

	dev->fmt      = format_by_fourcc(V4L2_PIX_FMT_BGR24);
	dev->width    = 720;
	dev->height   = 576;
	dev->field    = V4L2_FIELD_INTERLACED;
#endif


	/* setup video buffers queue */
	INIT_LIST_HEAD(&dev->active);
	dev->vidq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	dev->vidq.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	dev->vidq.io_modes = VB2_MMAP | VB2_USERPTR | VB2_READ | VB2_DMABUF;
	dev->vidq.ops = &tw5864_video_qops;
	dev->vidq.mem_ops = &vb2_dma_contig_memops;
	dev->vidq.drv_priv = dev;
	dev->vidq.gfp_flags = __GFP_DMA32;
	dev->vidq.buf_struct_size = sizeof(struct tw5864_buf);
	dev->vidq.lock = &dev->lock;
	dev->vidq.min_buffers_needed = 2;
	ret = vb2_queue_init(&dev->vidq);
	if (ret)
		goto vb2_q_init_fail;

	dev->vdev = tw5864_video_template;
	dev->vdev.v4l2_dev = &dev->root->v4l2_dev;
	dev->vdev.lock = &dev->lock;
	dev->vdev.queue = &dev->vidq;
	video_set_drvdata(&dev->vdev, dev);


	/* Initialize the device control structures */
	dev->alloc_ctx = vb2_dma_contig_init_ctx(&dev->root->pci->dev);
	if (IS_ERR(dev->alloc_ctx)) {
		ret = PTR_ERR(dev->alloc_ctx);
		goto vb2_dma_contig_init_ctx_fail;
	}

	v4l2_ctrl_handler_init(hdl, 6);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			V4L2_CID_BRIGHTNESS, -128, 127, 1, 20);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			V4L2_CID_CONTRAST, 0, 255, 1, 100);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			V4L2_CID_SATURATION, 0, 255, 1, 128);
	/* NTSC only */
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			V4L2_CID_HUE, -128, 127, 1, 0);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			V4L2_CID_COLOR_KILLER, 0, 1, 1, 0);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			V4L2_CID_CHROMA_AGC, 0, 1, 1, 1);
	if (hdl->error) {
		ret = hdl->error;
		goto v4l2_ctrl_fail;
	}
	dev->vdev.ctrl_handler = hdl;
	v4l2_ctrl_handler_setup(hdl);


	ret = video_register_device(&dev->vdev, VFL_TYPE_GRABBER, video_nr);
	if (ret)
		goto v4l2_ctrl_fail;

	pr_info("%s (IRQ %d): registered video device %s\n", dev->root->name,
			dev->root->pci->irq, video_device_node_name(&dev->vdev));

	return 0;

v4l2_ctrl_fail:
	v4l2_ctrl_handler_free(hdl);
	vb2_dma_contig_cleanup_ctx(dev->alloc_ctx);
vb2_dma_contig_init_ctx_fail:
	vb2_queue_release(&dev->vidq);
vb2_q_init_fail:
	mutex_destroy(&dev->lock);
	/* Nothing to do to deinit spinlock? */

	return ret;
}

static void tw5864_video_input_fini(struct tw5864_input *dev)
{
	video_unregister_device(&dev->vdev);
	v4l2_ctrl_handler_free(&dev->hdl);
	vb2_dma_contig_cleanup_ctx(dev->alloc_ctx);
	vb2_queue_release(&dev->vidq);
	mutex_destroy(&dev->lock);
}

void tw5864_video_fini(struct tw5864_dev *dev)
{
	int i;

	for (i = 0; i < TW5864_INPUTS; i++)
		tw5864_video_input_fini(&dev->inputs[i]);
}

int tw5864_video_irq(struct tw5864_dev *dev, unsigned long status)
{
	return 0;
#if 0
	__u32 reg;

	/* reset interrupts handled by this routine */
	tw_writel(TW5864_INTSTAT, status);
	/*
	 * Check most likely first
	 *
	 * DMAPI shows we have reached the end of the risc code
	 * for the current buffer.
	 */
	if (status & TW5864_DMAPI) {
		struct tw5864_buf *buf;

		spin_lock(&dev->slock);
		buf = list_entry(dev->active.next, struct tw5864_buf, list);
		list_del(&buf->list);
		spin_unlock(&dev->slock);
		v4l2_get_timestamp(&buf->vb.v4l2_buf.timestamp);
		buf->vb.v4l2_buf.field = dev->field;
		buf->vb.v4l2_buf.sequence = dev->seqnr++;
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_DONE);
		status &= ~(TW5864_DMAPI);
		if (0 == status)
			return;
	}
	if (status & (TW5864_VLOCK | TW5864_HLOCK))
		dev_dbg(&dev->pci->dev, "Lost sync\n");
	if (status & TW5864_PABORT)
		dev_err(&dev->pci->dev, "PABORT interrupt\n");
	if (status & TW5864_DMAPERR)
		dev_err(&dev->pci->dev, "DMAPERR interrupt\n");
	if (status & TW5864_FDMIS)
		dev_dbg(&dev->pci->dev, "FDMIS interrupt\n");
	if (status & TW5864_FFOF) {
		/* probably a logic error */
		reg = tw_readl(TW5864_DMAC) & TW5864_FIFO_EN;
		tw_clearl(TW5864_DMAC, TW5864_FIFO_EN);
		dev_dbg(&dev->pci->dev, "FFOF interrupt\n");
		tw_setl(TW5864_DMAC, reg);
	}
	if (status & TW5864_FFERR)
		dev_dbg(&dev->pci->dev, "FFERR interrupt\n");
#endif
}
