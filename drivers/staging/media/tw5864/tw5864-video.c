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

#include "tw5864.h"
#include "tw5864-reg.h"

/* ------------------------------------------------------------- */
/* vb2 queue operations                                          */

static int tw5864_queue_setup(struct vb2_queue *q, const struct v4l2_format *fmt,
		unsigned int *num_buffers, unsigned int *num_planes,
		unsigned int sizes[], void *alloc_ctxs[])
{
	struct tw5864_input *dev = vb2_get_drv_priv(q);

	sizes[0] = H264_VLC_BUF_SIZE;
	alloc_ctxs[0] = dev->alloc_ctx;
	*num_planes = 1;

	if (*num_buffers < 12)
		*num_buffers = 12;

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
	// TODO Copy all data to output buffer
}

int tw5864_enable_input(struct tw5864_dev *dev, int input_number) {
	struct tw5864_input *input = &dev->inputs[input_number];
	unsigned long flags;
	int i;
	dev_dbg(&dev->pci->dev, "enabling channel %d\n", input_number);

	u8 indir_0x0Ne = tw_indir_readb(dev, 0x00e + input_number * 0x010);
	u8 std = (indir_0x0Ne & 0x70) >> 4;

	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (std == STD_INVALID) {
		dev_err(&dev->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	input->std = std;
	input->v4l2_std = tw5864_get_v4l2_std(std);

	if (tw_readl(TW5864_VLC_BUF))
		tw_writel(TW5864_VLC_BUF, tw_readl(TW5864_VLC_BUF) & 0x0f);

	//tw_writel(TW5864_DSP_ENC_ORG_PTR_REG,0x00000000);
	tw_writel(TW5864_DSP_CODEC,0x00000000);
	//tw_writel(TW5864_DSP_ENC_REC,0x00000000);
	tw_writel(0x0000021C,0x00000000);
	tw_writel(0x00000210,0x00000003);



	tw_writel(TW5864_DSP_QP, QP_VALUE);
	tw_writel(TW5864_DSP_REF_MVP_LAMBDA,Lambda_lookup_table[QP_VALUE]);
	tw_writel(TW5864_INTERLACING, TW5864_DI_EN);
	tw_writel(TW5864_EMU_EN_VARIOUS_ETC,TW5864_EMU_EN_LPF | TW5864_EMU_EN_BHOST | TW5864_EMU_EN_SEN | TW5864_EMU_EN_ME | TW5864_EMU_EN_DDR);
	tw_writel(TW5864_DSP_I4x4_WEIGHT,Intra4X4_Lambda3[QP_VALUE]);
	tw_writel(TW5864_DSP_INTRA_MODE,0x00000070);
	tw_writel(TW5864_DSP, 0 /* channel id */ | TW5864_DSP_CHROM_SW | ((0xa << 8) & TW5864_DSP_MB_DELAY) /* 0x00000A20 */);
	tw_writel(TW5864_MOTION_SEARCH_ETC, TW5864_INTRA_EN);
	tw_writel(TW5864_PCI_INTR_CTL, TW5864_PREV_MAST_ENB | TW5864_PREV_OVERFLOW_ENB | TW5864_TIMER_INTR_ENB | TW5864_PCI_MAST_ENB | (1<<1)  /* TODO try TW5864_MVD_VLC_MAST_ENB*/ /*0x00000073*/);
	tw_writel(TW5864_MASTER_ENB_REG,TW5864_PCI_VLC_INTR_ENB | TW5864_PCI_PREV_INTR_ENB | TW5864_PCI_PREV_OF_INTR_ENB/*0x00000032*/);

	tw_indir_writeb(dev, 0x200, 720 / 4); // indir in width/4
#if 0 // D1
	tw_indir_writeb(dev, 0x202, 720 / 4); // indir out width/4
	input->width = 720;
	for (i = 0; i < 4; i++) {
		tw_writel(TW5864_FRAME_WIDTH_BUS_A(i), 0x2cf);
		tw_writel(TW5864_FRAME_WIDTH_BUS_B(i), 0x2cf);
		tw_writel(TW5864_H264EN_RATE_CNTL_LO_WORD(i, input_number), 0xffff);
		tw_writel(TW5864_H264EN_RATE_CNTL_HI_WORD(i, input_number), 0x3fff);
	}
#else // CIF
	tw_indir_writeb(dev, 0x202, 720 / 8); // indir out width/8
	input->width = 720;
	for (i = 0; i < 4; i++) {
		tw_writel(TW5864_FRAME_WIDTH_BUS_A(i), 0x15f);
		tw_writel(TW5864_FRAME_WIDTH_BUS_B(i), 0x15f);
		tw_writel(TW5864_H264EN_RATE_CNTL_LO_WORD(i, input_number), 0xffff);
		tw_writel(TW5864_H264EN_RATE_CNTL_HI_WORD(i, input_number), 0x3fff);
	}
#endif


	if (std == STD_NTSC) {
#if 0 // D1
		input->height = 480;
#else
		input->height = 240;
#endif
		tw_indir_writeb(dev, 0x201, input->height / 4);
		tw_indir_writeb(dev, 0x203, input->height / 8);
		input->height = 480;
		tw_writel(TW5864_DSP_PIC_MAX_MB, ((input->width / 16) << 8) | (input->width / 16));

		for (i = 0; i < 4; i++) {
#if 0 // D1
			tw_writel(TW5864_FRAME_HEIGHT_BUS_A(i), 0x1df);
			tw_writel(TW5864_FRAME_HEIGHT_BUS_B(i), 0x1df);
#else
			tw_writel(TW5864_FRAME_HEIGHT_BUS_A(i), 0x0ef);
			tw_writel(TW5864_FRAME_HEIGHT_BUS_B(i), 0x0ef);
#endif
		}
		tw_writel(TW5864_H264EN_RATE_MAX_LINE_REG1, 0x3bd);
		tw_writel(TW5864_H264EN_RATE_MAX_LINE_REG2, 0x3bd);
	} else {
		input->height = 576;
		tw_indir_writeb(dev, 0x201, 0x48);
		tw_indir_writeb(dev, 0x203, 0x48);
		tw_writel(TW5864_DSP_PIC_MAX_MB, ((720 / 16) << 8) | (576 / 16));

		for (i = 0; i < 4; i++) {
			tw_writel(TW5864_FRAME_HEIGHT_BUS_A(i), 0x23f);
			tw_writel(TW5864_FRAME_HEIGHT_BUS_B(i), 0x23f);
		}
		tw_writel(TW5864_H264EN_RATE_MAX_LINE_REG1, 0x318);
		tw_writel(TW5864_H264EN_RATE_MAX_LINE_REG2, 0x318);
	}

	tw5864_prepare_frame_headers(input);
	tw_writel(TW5864_VLC, TW5864_VLC_PCI_SEL | ((input->tail_nb_bits + 24) << TW5864_VLC_BIT_ALIGN_SHIFT) | QP_VALUE);

	spin_lock_irqsave(&dev->slock, flags);
	dev->inputs[input_number].enabled = 1;
	dev->irqmask |= TW5864_INTR_VLC_DONE | TW5864_INTR_PV_OVERFLOW | TW5864_INTR_TIMER | TW5864_INTR_AUD_EOF;
	tw5864_irqmask_apply(dev);
	spin_unlock_irqrestore(&dev->slock, flags);
	tw_writel(TW5864_SLICE,0x00008000);
	tw_writel(TW5864_SLICE,0x00000000);
	return 0;
}

static int tw5864_disable_input(struct tw5864_dev *dev, int input_number) {
	unsigned long flags;
	dev_dbg(&dev->pci->dev, "disabling channel %d\n", input_number);
	mutex_lock(&dev->lock);

	tw_clearl(TW5864_H264EN_CH_EN, 1 << input_number);
	tw_clearl(TW5864_SEN_EN_CH, 1 << input_number);
	tw_clearl(TW5864_MASTER_ENB_REG, TW5864_PCI_VLC_INTR_ENB);
	tw_clearl(TW5864_PCI_INTR_CTL, TW5864_PCI_MAST_ENB | TW5864_MVD_VLC_MAST_ENB);
	spin_lock_irqsave(&dev->slock, flags);
	dev->inputs[input_number].enabled = 0;
	dev->irqmask &= ~TW5864_INTR_VLC_DONE;  // timer doesn't like to get turned off?
	tw5864_irqmask_apply(dev);
	spin_unlock_irqrestore(&dev->slock, flags);
	tw_clearl(TW5864_VLC, 0x8000);

	mutex_unlock(&dev->lock);
	return 0;
}

static int tw5864_start_streaming(struct vb2_queue *q, unsigned int count)
{
	struct tw5864_input *input = vb2_get_drv_priv(q);
	struct tw5864_dev *dev = input->root;

	input->discard_frames = GOP_SIZE;
	input->frame_seqno = 0;
	input->h264_idr_pic_id = 1;
	tw_writel(TW5864_DSP_REF, (tw_readl(TW5864_DSP_REF) & ~TW5864_DSP_REF_FRM) | input->h264_idr_pic_id);
	input->h264_frame_seqno_in_gop = 0;
	input->h264 = tw5864_h264_init();
	tw5864_enable_input(input->root, input->input_number);
	return 0;
}

static void tw5864_stop_streaming(struct vb2_queue *q)
{
	unsigned long flags;
	struct tw5864_input *input = vb2_get_drv_priv(q);

	tw5864_disable_input(input->root, input->input_number);

	spin_lock_irqsave(&input->slock, flags);
	if (input->vb) {
		vb2_buffer_done(&input->vb->vb, VB2_BUF_STATE_ERROR);
		input->vb = NULL;
	}
	while (!list_empty(&input->active)) {
		struct tw5864_buf *buf =
			container_of(input->active.next, struct tw5864_buf, list);

		list_del(&buf->list);
		vb2_buffer_done(&buf->vb, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&input->slock, flags);
	tw5864_h264_destroy(input->h264);
	input->h264 = NULL;
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
	struct tw5864_input *input = video_drvdata(file);

	u8 indir_0x0Ne = tw_indir_readb(input->root, 0x00e + input->input_number * 0x010);
	u8 std = (indir_0x0Ne & 0x70) >> 4;

	if (indir_0x0Ne & 0x80) {
		dev_err(&input->root->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (std == STD_INVALID) {
		dev_err(&input->root->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	f->fmt.pix.width = 720;
	switch (std) {
		default:
			WARN_ON_ONCE(1);
		case STD_NTSC:
			f->fmt.pix.height = 480;
			break;
		case STD_PAL:
		case STD_SECAM:
			f->fmt.pix.height = 576;
			break;
	}
	f->fmt.pix.field        = V4L2_FIELD_NONE;
	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_H264;
	f->fmt.pix.sizeimage = H264_VLC_BUF_SIZE;
	f->fmt.pix.colorspace	= V4L2_COLORSPACE_SMPTE170M;
	f->fmt.pix.priv = 0;
	return 0;
}

static int tw5864_enum_input(struct file *file, void *priv,
		struct v4l2_input *i)
{
	struct tw5864_input *dev = video_drvdata(file);

	u8 indir_0x0N0 = tw_indir_readb(dev->root, 0x000 + dev->input_number * 0x010);
	u8 indir_0x0Nd = tw_indir_readb(dev->root, 0x00d + dev->input_number * 0x010);
	u8 indir_0x0Ne = tw_indir_readb(dev->root, 0x00e + dev->input_number * 0x010);
	u8 std = (indir_0x0Ne & 0x70) >> 4;
	u8 v1 = indir_0x0N0;
	u8 v2 = indir_0x0Nd;

	if (i->index)
		return -EINVAL;

	// TODO Deduplicate
	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->root->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (std == STD_INVALID) {
		dev_err(&dev->root->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	i->type = V4L2_INPUT_TYPE_CAMERA;
	snprintf(i->name, sizeof(i->name), "Encoder %d", dev->input_number);
	//i->std = tw5864_get_v4l2_std(std);
	i->std = TW5864_NORMS;
	if (v1 & (1 << 7))
		i->status |= V4L2_IN_ST_NO_SYNC;
	if (!(v1 & (1 << 6)))
		i->status |= V4L2_IN_ST_NO_H_LOCK;
	if (v1 & (1 << 2))
		i->status |= V4L2_IN_ST_NO_SIGNAL;
	if (v1 & (1 << 1))
		i->status |= V4L2_IN_ST_NO_COLOR;
	if (v2 & (1 << 2))
		i->status |= V4L2_IN_ST_MACROVISION;

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
	cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_READWRITE |
		V4L2_CAP_STREAMING;

	cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	return 0;
}

static int tw5864_g_std(struct file *file, void *priv, v4l2_std_id *id)
{
	struct tw5864_input *dev = video_drvdata(file);

	u8 indir_0x0Ne = tw_indir_readb(dev->root, 0x00e + dev->input_number * 0x010);
	u8 std = (indir_0x0Ne & 0x70) >> 4;

	// TODO Deduplicate
	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->root->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (std == STD_INVALID) {
		dev_err(&dev->root->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	*id = tw5864_get_v4l2_std(std);
	return 0;
}

static int tw5864_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct tw5864_input *dev = video_drvdata(file);

	if (vb2_is_busy(&dev->vidq))
		return -EBUSY;
	// TODO FIXME compare with currently detected, refuse otherwise
	if (!(id & TW5864_NORMS))
		return -EINVAL;

	return 0;
}

static int tw5864_try_fmt_vid_cap(struct file *file, void *priv,
		struct v4l2_format *f)
{
	return tw5864_g_fmt_vid_cap(file, priv, f);
}

static int tw5864_s_fmt_vid_cap(struct file *file, void *priv,
		struct v4l2_format *f)
{
	struct tw5864_input *dev = video_drvdata(file);
	int err;

	err = tw5864_try_fmt_vid_cap(file, priv, f);
	if (0 != err)
		return err;

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
	if (ret)
		mdelay(100);
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
	.vidioc_try_fmt_vid_cap		= tw5864_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap		= tw5864_s_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap		= tw5864_g_fmt_vid_cap,
	.vidioc_log_status		= v4l2_ctrl_log_status,
	.vidioc_subscribe_event		= v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event	= v4l2_event_unsubscribe,
};
static struct video_device tw5864_video_template = {
	.name			= "tw5864_video",
	.fops			= &video_fops,
	.ioctl_ops		= &video_ioctl_ops,
	.release		= video_device_release_empty,
	.tvnorms		= TW5864_NORMS,
};

static int tw5864_video_input_init(struct tw5864_input *dev, int video_nr);
static void tw5864_video_input_fini(struct tw5864_input *dev);

int tw5864_video_init_reg_fucking(struct tw5864_dev *dev, int *video_nr)
{
	return 0;
}

int tw5864_video_init(struct tw5864_dev *dev, int *video_nr)
{
	int i;
	int j;
	int ret;

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

	ret = tw5864_video_init_reg_fucking(dev, video_nr);

	dev->jpg.data = dev->jpeg_buf[0].addr;
	dev->jpg.size = 0x1000;

	if (!debugfs_create_blob("jpg", S_IRUGO, dev->debugfs_dir, &dev->jpg)) {
		dev_err(&dev->pci->dev, "jpg debugfs blob creation failed\n");
		return 1;
	}

	for (i = 0; i < VLC_DUMP_CNT; i++) {
		char filename[10];
		snprintf(filename, sizeof(filename), "vlc_%02d", i);
		dev->vlc[i].data = kmalloc(H264_VLC_BUF_SIZE, GFP_KERNEL);
		dev->vlc[i].size = 0;
		if (!debugfs_create_blob(filename, S_IRUGO, dev->debugfs_dir, &dev->vlc[i])) {
			dev_err(&dev->pci->dev, "vlc debugfs blob creation failed\n");
			return 1;
		}
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
	dev->vidq.min_buffers_needed = 12;
	ret = vb2_queue_init(&dev->vidq);
	if (ret)
		goto vb2_q_init_fail;

	dev->vdev = tw5864_video_template;
	// TODO Set tvnorms to actual recognized format instead of wildcard?
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

	for (i = 0; i < H264_BUF_CNT; i++) {
		dma_free_coherent(&dev->pci->dev, H264_VLC_BUF_SIZE, dev->h264_vlc_buf[i].addr, dev->h264_vlc_buf[i].dma_addr);
		dma_free_coherent(&dev->pci->dev, H264_MV_BUF_SIZE, dev->h264_mv_buf[i].addr, dev->h264_mv_buf[i].dma_addr);
	}
	for (i = 0; i < 8; i++) {
		dma_free_coherent(&dev->pci->dev, H264_VLC_BUF_SIZE, dev->jpeg_buf[i].addr, dev->jpeg_buf[i].dma_addr);
	}
}

void tw5864_prepare_frame_headers(struct tw5864_input *input)
{
	struct tw5864_dev *dev = input->root;
	struct tw5864_buf *vb = input->vb;
	u8 *dst;
	unsigned long dst_size;
	unsigned long dst_space;
	int skip_bytes = 3;

	if (!vb) {
		spin_lock(&input->slock);
		if (list_empty(&input->active)) {
			spin_unlock(&input->slock);
			input->vb = NULL;
			return;
		}
		vb = list_first_entry(&input->active, struct tw5864_buf, list);
		list_del(&vb->list);
		spin_unlock(&input->slock);
	}

	dst = vb2_plane_vaddr(&vb->vb, 0);
	dst_size = vb2_plane_size(&vb->vb, 0);
	dst_space = dst_size;

	// Generate H264 headers:
	// If this is first frame, put SPS and PPS
	if (input->frame_seqno == 0)
		tw5864_h264_put_stream_header(input->h264, &dst, &dst_space, QP_VALUE, input->width, input->height);

	// Put slice header
	tw5864_h264_put_slice_header(input->h264, &dst, &dst_space, input->h264_idr_pic_id, input->h264_frame_seqno_in_gop, &input->tail_nb_bits, &input->tail);
	input->vb = vb;
	input->buf_cur_ptr = dst;
	input->buf_cur_space_left = dst_space;
	dev_dbg(&dev->pci->dev, "Prepared slice header for frame #%d in GOP #%d. Tail: %d bits, 0x%02x\n", input->h264_frame_seqno_in_gop, input->h264_idr_pic_id, input->tail_nb_bits, input->tail);
}

void tw5864_handle_frame(struct tw5864_input *input, unsigned long frame_len)
{
	struct tw5864_dev *dev = input->root;
	struct tw5864_buf *vb;
	unsigned long dst_size;
	unsigned long dst_space;
	int skip_bytes = 3;

	dev_dbg(&dev->pci->dev, "Total VLC bits: %u, residue: %u, bitalign: %u, our tail_nb_bits: %u\n",
			tw_readl(TW5864_SLICE_TOTAL_BIT),
			tw_readl(TW5864_RES_TOTAL_BIT),
			(tw_readl(TW5864_VLC) & TW5864_VLC_BIT_ALIGN_MASK) >> TW5864_VLC_BIT_ALIGN_SHIFT,
			input->tail_nb_bits
	       );

	spin_lock(&input->slock);
	vb = input->vb;
	input->vb = NULL;
	spin_unlock(&input->slock);

	if (!vb)  /* Gone because of disabling */
		return;

	u8 *dst = input->buf_cur_ptr;
	dst_size = vb2_plane_size(&vb->vb, 0);

	dst_space = input->buf_cur_space_left;
	frame_len -= skip_bytes;  // skip first bytes of frame produced by hardware
	if (WARN_ON_ONCE(dst_space < frame_len)) {
		dev_err_once(&dev->pci->dev, "Left space in vb2 buffer %lu is insufficient for frame length %lu, writing truncated frame\n", dst_space, frame_len);
		frame_len = dst_space;
	}
	u8 tail_mask = 0xff, vlc_mask = 0;
	int i;
	for (i = 0; i < 8 - input->tail_nb_bits; i++)
		vlc_mask |= 1 << i;
	tail_mask = (~vlc_mask) & 0xff;

	tail_mask = vlc_mask = 0xff;
	u8 vlc_first_byte = ((u8 *)(dev->h264_vlc_buf[0].addr + skip_bytes))[0];
	dst[0] = (input->tail & tail_mask) | (vlc_first_byte  & vlc_mask );
	skip_bytes++;
	frame_len--;
	dst++;
	dst_space--;
	memcpy(dst, dev->h264_vlc_buf[0].addr + skip_bytes, frame_len);
	dst_space -= frame_len;
	vb2_set_plane_payload(&vb->vb, 0, dst_size - dst_space);

	vb2_buffer_done(&vb->vb, VB2_BUF_STATE_DONE);
}

v4l2_std_id tw5864_get_v4l2_std(enum tw5864_vid_std std)
{
	switch (std) {
		case STD_NTSC: return V4L2_STD_NTSC_M;
		case STD_PAL: return V4L2_STD_PAL_B;
		case STD_SECAM: return V4L2_STD_SECAM_B;
		case STD_INVALID: WARN_ON_ONCE(1); return 0;
	}
}

enum tw5864_vid_std tw5864_from_v4l2_std(v4l2_std_id v4l2_std)
{
	if (v4l2_std & V4L2_STD_NTSC) {
		return STD_NTSC;
	} else if (v4l2_std & V4L2_STD_PAL) {
		return STD_PAL;
	} else if (v4l2_std & V4L2_STD_SECAM) {
		return STD_SECAM;
	} else {
		WARN_ON_ONCE(1);
		return STD_AUTO;
	}
}
