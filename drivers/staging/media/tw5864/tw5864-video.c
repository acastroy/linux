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

	if (*num_buffers < 2)
		*num_buffers = 2;

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
	int i;
	dev_dbg(&dev->pci->dev, "enabling channel %d\n", input_number);
	//mutex_lock(&dev->lock);

#include "init6.c"

#if 0
	u8 indir_0x0Ne = tw_indir_readb(dev, 0x00e + input_number * 0x010);
	u8 fmt = indir_0x0Ne & 0x70;

	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (fmt == 0x70) {
		dev_err(&dev->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}



	tw_setl(TW5864_SEN_EN_CH, 1 << input_number);
	tw_setl(TW5864_H264EN_CH_EN, 1 << input_number);
	w(TW5864_DSP,0x00000A20);
	tw_setl(TW5864_MASTER_ENB_REG, TW5864_PCI_VLC_INTR_ENB);
	tw_setl(TW5864_PCI_INTR_CTL, TW5864_PCI_MAST_ENB | TW5864_MVD_VLC_MAST_ENB);
	dev->irqmask |= TW5864_INTR_VLC_DONE;
	tw_writel(TW5864_INTR_ENABLE_L, dev->irqmask & 0xffff);
	tw_writel(TW5864_INTR_ENABLE_H, dev->irqmask >> 16);
	tw_writel(TW5864_SLICE, 0x00000070);
	tw_writel(TW5864_SLICE, 0x00008000);
	tw_writel(TW5864_SLICE, 0x00000000);
	w(TW5864_DSP_PIC_MAX_MB,0x00002D00 | (fmt==0x00 ? 0x1e : 0x24));
	tw_writel(0x18044, 0xefff | (fmt==0 ? 0x1000 : 0));
	tw_writel(TW5864_VLC, 0x8000 | 0x1a /* QP */);


	// BUS 0 only setting with 0x0d1x
	// TODO tune other buses
	for (i = 0; i < 4; i++) {
		tw_writel(TW5864_FRAME_WIDTH_BUS_A(i), 0x2cf);
		tw_writel(TW5864_FRAME_WIDTH_BUS_B(i), 0x2cf);
	}

	tw_indir_writeb(dev, 0x200, 0xb4);
	tw_indir_writeb(dev, 0x202, 0xb4);

	if (fmt == 0x00 /* NTSC */) {
		tw_indir_writeb(dev, 0x201, 0x3c);
		tw_indir_writeb(dev, 0x203, 0x3c);

		for (i = 0; i < 4; i++) {
			tw_writel(TW5864_FRAME_HEIGHT_BUS_A(i), 0x1df);
			tw_writel(TW5864_FRAME_HEIGHT_BUS_B(i), 0x1df);
		}
	} else {
		tw_indir_writeb(dev, 0x201, 0x48);
		tw_indir_writeb(dev, 0x203, 0x48);

		for (i = 0; i < 4; i++) {
			tw_writel(TW5864_FRAME_HEIGHT_BUS_A(i), 0x23f);
			tw_writel(TW5864_FRAME_HEIGHT_BUS_B(i), 0x23f);
		}
	}
#endif

	//mutex_unlock(&dev->lock);
	return 0;
}

static int tw5864_disable_input(struct tw5864_dev *dev, int input_number) {
	dev_dbg(&dev->pci->dev, "disabling channel %d\n", input_number);
	mutex_lock(&dev->lock);

	tw_clearl(TW5864_H264EN_CH_EN, 1 << input_number);
	tw_clearl(TW5864_SEN_EN_CH, 1 << input_number);
	tw_clearl(TW5864_MASTER_ENB_REG, TW5864_PCI_VLC_INTR_ENB);
	tw_clearl(TW5864_PCI_INTR_CTL, TW5864_PCI_MAST_ENB | TW5864_MVD_VLC_MAST_ENB);
	dev->irqmask &= ~TW5864_INTR_VLC_DONE;
	tw_writel(TW5864_INTR_ENABLE_L, dev->irqmask & 0xffff);
	tw_writel(TW5864_INTR_ENABLE_H, dev->irqmask >> 16);
	tw_clearl(TW5864_VLC, 0x8000);

	mutex_unlock(&dev->lock);
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

	u8 indir_0x0Ne = tw_indir_readb(dev->root, 0x00e + dev->input_number * 0x010);
	u8 fmt = indir_0x0Ne & 0x70;

	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->root->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (fmt == 0x70) {
		dev_err(&dev->root->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	f->fmt.pix.width = 720;
	switch (fmt) {
		case 0:  /* NTSC */
			f->fmt.pix.height = 480;
			break;
		case 1:  /* PAL */
		case 2:  /* SECAM */
			f->fmt.pix.height = 576;
			break;
		default:
			BUG();
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
	u8 fmt = indir_0x0Ne & 0x70;
	u8 v1 = indir_0x0N0;
	u8 v2 = indir_0x0Nd;

	if (i->index)
		return -EINVAL;

	// TODO Deduplicate
	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->root->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (fmt == 0x70) {
		dev_err(&dev->root->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	i->type = V4L2_INPUT_TYPE_CAMERA;
	snprintf(i->name, sizeof(i->name), "Encoder %d", dev->input_number);
/*
	switch (fmt) {
		case 0: i->std = V4L2_STD_NTSC_M; break;
		case 1: i->std = V4L2_STD_PAL_B; break;
		case 2: i->std = V4L2_STD_SECAM; break;
		default: BUG();
	}
*/
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
	u8 fmt = indir_0x0Ne & 0x70;

	// TODO Deduplicate
	if (indir_0x0Ne & 0x80) {
		dev_err(&dev->root->pci->dev, "Video format detection is in progress, please wait\n");
		return -EAGAIN;
	}

	if (fmt == 0x70) {
		dev_err(&dev->root->pci->dev, "Video format detection done, no valid video format\n");
		return -1;
	}

	*id = !fmt ? V4L2_STD_NTSC_M : V4L2_STD_PAL_B;
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

	for (i = 0; i < H264_BUF_CNT; i++) {
		dma_free_coherent(&dev->pci->dev, H264_VLC_BUF_SIZE, dev->h264_vlc_buf[i].addr, dev->h264_vlc_buf[i].dma_addr);
		dma_free_coherent(&dev->pci->dev, H264_MV_BUF_SIZE, dev->h264_mv_buf[i].addr, dev->h264_mv_buf[i].dma_addr);
	}
	for (i = 0; i < 8; i++) {
		dma_free_coherent(&dev->pci->dev, H264_VLC_BUF_SIZE, dev->jpeg_buf[i].addr, dev->jpeg_buf[i].dma_addr);
	}
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
