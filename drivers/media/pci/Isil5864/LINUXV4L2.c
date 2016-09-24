#include "LINUXV4L2.h" 
#include "property.h"
#include "tw6805.h"
#include "cx2581.h"
#include "cx2588.h"
#include "sa7160.h"
#include "sl6010.h"
#include "ah8400.h"
#include "tw5864.h"
#include "fh8735.h"
#include "mz0380.h"

// ##########################################################################################################################################################################################################
// #
// # PRODUCT NAME: LINUXV4L2
// #        CHIPS: NONE
// #  MANUFACTORY: NONE
// #         DATE: 2008.06.24 ~ 201X.XX.XX
// #
// ##########################################################################################################################################################################################################
//

// ##########################################################################################################################################################################################################
//
static int  __init       common_module_on_init( void );
static void __exit       common_module_on_exit( void );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
#define __devinit        __section(.devinit.text) __cold notrace
#define __devexit        __section(.devexit.text) __exitused __cold notrace
#endif

static int  __devinit    common_pci_driver_on_probe( struct pci_dev * p_pci_dev, const struct pci_device_id * p_pci_device_id );
static void __devexit    common_pci_driver_on_remove( struct pci_dev * p_pci_dev );

static int               common_pci_driver_on_suspend( struct pci_dev * p_pci_dev, pm_message_t s_pm_message_state );
static int               common_pci_driver_on_resume( struct pci_dev * p_pci_dev );
static irqreturn_t       common_pci_driver_on_interrupt_2_6_19( int n_irq, void * p_context );
static irqreturn_t       common_pci_driver_on_interrupt_2_6_18( int n_irq, void * p_context, struct pt_regs * p_regs );
static void              common_pci_driver_on_tasklet( unsigned long p_context );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)

static int               common_file_operations_open( struct file * p_file );
static int               common_file_operations_release( struct file * p_file );
#else
static int               common_file_operations_open( struct inode * p_inode, struct file * p_file );
static int               common_file_operations_release( struct inode * p_inode, struct file * p_file );
#endif

static ssize_t           common_file_operations_read( struct file * p_file, char __user * p_data, size_t n_count, loff_t * p_offset );
static unsigned int      common_file_operations_poll( struct file * p_file, struct poll_table_struct * p_poll_table_struct_wait );
static int               common_file_operations_mmap( struct file * p_file, struct vm_area_struct * p_vma );
static int               common_file_operations_ioctl( struct inode * p_inode, struct file * p_file, unsigned int cmd, unsigned long arg );
static int               common_file_operations_do_ioctl( struct inode * p_inode, struct file * p_file, unsigned int cmd, void * p_arg );
static int               common_video_device_vidioc_querycap( struct file * p_file, void * p_priv, struct v4l2_capability * p_cap );
static int               common_video_device_vidioc_querystd( struct file * p_file, void * p_priv, v4l2_std_id * s );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
static int               common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id  s );
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static int               common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id * s );
#else
static int               common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id  s );
#endif

static int               common_video_device_vidioc_g_std( struct file * p_file, void * p_priv, v4l2_std_id * s );
static int               common_video_device_vidioc_enum_input( struct file * p_file, void * p_priv, struct v4l2_input * p_input );
static int				 common_video_device_vidioc_enumaudio(struct file *p_file, void *priv, struct v4l2_audio *a);

static int               common_video_device_vidioc_s_input( struct file * p_file, void * p_priv, unsigned int i );
static int               common_video_device_vidioc_g_input( struct file * p_file, void * p_priv, unsigned int * i );

static int				 common_video_device_vidioc_s_audio( struct file *file, void *fh, const struct v4l2_audio *a );
static int				 common_video_device_vidioc_g_audio( struct file *p_file, void *fh, struct v4l2_audio *a);

//static int				common_video_device_vidioc_enum_dv_presets(struct file *file, void *fh, struct v4l2_dv_enum_preset *preset);

static int               common_video_device_vidioc_s_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp );
static int               common_video_device_vidioc_g_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp );

static int				 common_video_device_vidioc_enum_frameintervals(struct file *p_file, void *fh, struct v4l2_frmivalenum *fival);
static int				 common_video_device_vidioc_enum_framesizes(struct file *p_file, void *fh, struct v4l2_frmsizeenum *fsize);

static int				 common_video_device_vidioc_cropcap(struct file *p_file, void *fh, struct v4l2_cropcap *cropcap);

static int               common_video_device_vidioc_enum_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_fmtdesc * p_fmtdesc );
static int               common_video_device_vidioc_try_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt );
static int               common_video_device_vidioc_s_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt );
static int               common_video_device_vidioc_g_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt );
static int               common_video_device_vidioc_queryctrl( struct file * p_file, void * p_priv, struct v4l2_queryctrl * p_queryctrl );
static int				 common_video_device_vidioc_querymenu( struct file * p_file, void * p_priv, struct v4l2_querymenu *a);

static int               common_video_device_vidioc_s_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control );
static int               common_video_device_vidioc_g_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

static int               common_video_device_vidioc_s_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls );
static int               common_video_device_vidioc_g_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls );
static int				 common_video_device_vidioc_try_ext_ctrls( struct file *p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls);
#endif

static int               common_video_device_vidioc_g_mpegcomp( struct file * p_file, void * p_priv, struct v4l2_mpeg_compression * p_mpeg_compression );
static int               common_video_device_vidioc_s_mpegcomp( struct file * p_file, void * p_priv, struct v4l2_mpeg_compression * p_mpeg_compression );
static int               common_video_device_vidioc_encoder_cmd( struct file * p_file, void * p_priv, struct v4l2_encoder_cmd * p_encoder_cmd );
static int               common_video_device_vidioc_reqbufs( struct file * p_file, void * p_priv, struct v4l2_requestbuffers * p_requestbuffers );
static int               common_video_device_vidioc_querybuf ( struct file * p_file, void * p_priv, struct v4l2_buffer * p_buffer );
static int               common_video_device_vidioc_qbuf( struct file * p_file, void * p_priv, struct v4l2_buffer * p_buffer );
static int               common_video_device_vidioc_dqbuf( struct file * p_file, void * p_priv, struct v4l2_buffer * p_buffer );
static int               common_video_device_vidioc_streamon( struct file * p_file, void * p_priv, enum v4l2_buf_type e_buf_type );
static int               common_video_device_vidioc_streamoff( struct file * p_file, void * p_priv, enum v4l2_buf_type e_buf_type );
static int               common_videobuf_queue_ops_buf_setup( struct videobuf_queue * p_queue, unsigned int * p_count, unsigned int * p_size );
static int               common_videobuf_queue_ops_buf_prepare( struct videobuf_queue * p_queue, struct videobuf_buffer * p_buffer, enum v4l2_field e_field );
static void              common_videobuf_queue_ops_buf_queue( struct videobuf_queue * p_queue, struct videobuf_buffer * p_buffer );
static void              common_videobuf_queue_ops_buf_release( struct videobuf_queue * p_queue, struct videobuf_buffer * p_buffer );
static int               common_snd_pcm_ops_open( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_close( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_hw_params( struct snd_pcm_substream * p_substream, struct snd_pcm_hw_params * p_params );
static int               common_snd_pcm_ops_hw_free( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_prepare( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_trigger( struct snd_pcm_substream * p_substream, int cmd );
static snd_pcm_uframes_t common_snd_pcm_ops_pointer( struct snd_pcm_substream * p_substream );
static int               common_snd_kcontrol_new_volume_info( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_info * p_info );
static int               common_snd_kcontrol_new_volume_get( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value );
static int               common_snd_kcontrol_new_volume_put( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value );
static int               common_custom_thread_start( struct file * p_file );
static int               common_custom_thread_stop( struct file * p_file );
static int               common_custom_thread( void * p_context );



static struct pci_device_id LINUXV4L2_PCI_DEVICE_ID_TABLE[ ] = {

	{ 0x1797, 0x6801, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, //
	{ 0x1797, 0x6802, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

	{ 0x1797, 0x6804, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, //
//peter[2011.07.06]
	{ 0x1797, 0x6805, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, 

	{ 0x1797, 0x6810, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, //
	{ 0x1797, 0x6811, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6812, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, 
	{ 0x1797, 0x6813, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, 

	{ 0x1797, 0x6814, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6815, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6816, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, 
	{ 0x1797, 0x6817, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, 

	{ 0x14F1, 0x8210, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, //

//	{ 0x14F1, 0x5851, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, // CONEXANT CX25858 PRODUCTS

#ifdef AME_SA7160
	{ 0x1131, 0x7160, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, // NXP SAA7160 PRODUCTS
#endif

	{ 0x1999, 0xA04C, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, //

#ifdef AME_FH8735
	{ 0x3086, 0x0101, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, // FULLHAN FH8735 PRODUCTS
#endif
	
#ifdef AME_MZ0380
	{ 0x0907, 0x0100, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, // VATICS MOZART 380 PRODUCTS
#endif

	{ 0x1797, 0x5864, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 }, // TECHWELL TW5864 PRODUCTS

	{ 0x0000, }
};

#ifdef AME_FH8735
static struct pci_driver linuxv4l2_pci_driver8735 = { //

	.name     = "LINUXV4L28735",
	.id_table =  LINUXV4L2_PCI_DEVICE_ID_TABLE,
	.probe    =  common_pci_driver_on_probe,
	.remove   =  common_pci_driver_on_remove,
	.suspend  =  common_pci_driver_on_suspend,
	.resume   =  common_pci_driver_on_resume,
};
#endif

#ifdef AME_SA7160
static struct pci_driver linuxv4l2_pci_driver7160 = { //

	.name     = "LINUXV4L27160",
	.id_table =  LINUXV4L2_PCI_DEVICE_ID_TABLE,
	.probe    =  common_pci_driver_on_probe,
	.remove   =  common_pci_driver_on_remove,
	.suspend  =  common_pci_driver_on_suspend,
	.resume   =  common_pci_driver_on_resume,
};
#endif


#ifdef AME_MZ0380
static struct pci_driver linuxv4l2_pci_driver0380 = { //

	.name     = "LINUXV4L20380",
	.id_table =  LINUXV4L2_PCI_DEVICE_ID_TABLE,
	.probe    =  common_pci_driver_on_probe,
	.remove   =  common_pci_driver_on_remove,
	.suspend  =  common_pci_driver_on_suspend,
	.resume   =  common_pci_driver_on_resume,
};
#endif


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)

static const struct v4l2_file_operations LINUXV4L2_FILE_OPERATIONS =
{
	.owner	  = THIS_MODULE,
	.open	  = common_file_operations_open,
	.release  = common_file_operations_release,
	.read	  = common_file_operations_read,
	.poll     = common_file_operations_poll,
	.mmap	  = common_file_operations_mmap,
	.unlocked_ioctl  = video_ioctl2,
};

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

static const struct file_operations LINUXV4L2_FILE_OPERATIONS =
{
	.owner	  = THIS_MODULE,
	.open	  = common_file_operations_open,
	.release  = common_file_operations_release,
	.read	  = common_file_operations_read,
	.poll     = common_file_operations_poll,
	.mmap	  = common_file_operations_mmap,
	.ioctl	  = video_ioctl2,
	.llseek   = no_llseek, 
};

#else

static const struct file_operations LINUXV4L2_FILE_OPERATIONS =
{
	.owner	  = THIS_MODULE,
	.open	  = common_file_operations_open,
	.release  = common_file_operations_release,
	.read	  = common_file_operations_read,
	.poll     = common_file_operations_poll,
	.mmap	  = common_file_operations_mmap,
	.ioctl	  = common_file_operations_ioctl,
	.llseek   = no_llseek,
};

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)

#ifdef CONFIG_VIDEO_ADV_DEBUG

#define INDIR_SPACE_MAP_SHIFT 0x100000

static int tw5864_g_reg(struct file *file, void *fh,
			struct v4l2_dbg_register *reg)
{
	CVideo * pVideo = (CVideo *)(file->private_data);

	if (reg->reg < INDIR_SPACE_MAP_SHIFT) {
		if (reg->reg > 0x87fff)
			return -EINVAL;
		reg->size = 4;
		reg->val = TW5864_GetRegister(pVideo->m_pDevice, reg->reg);
	} else {
		__u64 indir_addr = reg->reg - INDIR_SPACE_MAP_SHIFT;

		if (indir_addr > 0xefe)
			return -EINVAL;
		reg->size = 1;
		reg->val = TW5864_GetAnalogVideoDecoderRegister(pVideo->m_pDevice, reg->reg);
	}
	return 0;
}

static int tw5864_s_reg(struct file *file, void *fh,
			const struct v4l2_dbg_register *reg)
{
	CVideo * pVideo = (CVideo *)(file->private_data);

	if (reg->reg < INDIR_SPACE_MAP_SHIFT) {
		if (reg->reg > 0x87fff)
			return -EINVAL;
		TW5864_SetRegister(pVideo->m_pDevice, reg->reg, reg->val);
	} else {
		__u64 indir_addr = reg->reg - INDIR_SPACE_MAP_SHIFT;

		if (indir_addr > 0xefe)
			return -EINVAL;
		TW5864_SetAnalogVideoDecoderRegister(pVideo->m_pDevice, reg->reg, reg->val);
	}
	return 0;
}
#endif

static const struct v4l2_ioctl_ops LINUXV4L2_V4L2_IOCTL_OPS =
{
	.vidioc_querycap         = common_video_device_vidioc_querycap,
	.vidioc_querystd         = common_video_device_vidioc_querystd,
	.vidioc_s_std            = common_video_device_vidioc_s_std,
	.vidioc_g_std            = common_video_device_vidioc_g_std,
	.vidioc_enum_input       = common_video_device_vidioc_enum_input,
	.vidioc_s_input          = common_video_device_vidioc_s_input,
	.vidioc_g_input          = common_video_device_vidioc_g_input,
	.vidioc_enum_fmt_vid_cap = common_video_device_vidioc_enum_fmt_cap,
	.vidioc_try_fmt_vid_cap  = common_video_device_vidioc_try_fmt_cap,
	.vidioc_s_fmt_vid_cap    = common_video_device_vidioc_s_fmt_cap,
	.vidioc_g_fmt_vid_cap    = common_video_device_vidioc_g_fmt_cap,
	.vidioc_queryctrl        = common_video_device_vidioc_queryctrl,
	.vidioc_querymenu		 = common_video_device_vidioc_querymenu,
	.vidioc_s_ctrl           = common_video_device_vidioc_s_ctrl,
	.vidioc_g_ctrl           = common_video_device_vidioc_g_ctrl,
	.vidioc_s_ext_ctrls		 = common_video_device_vidioc_s_ext_ctrls,
	.vidioc_g_ext_ctrls		 = common_video_device_vidioc_g_ext_ctrls,
	.vidioc_try_ext_ctrls	 = common_video_device_vidioc_try_ext_ctrls,
	.vidioc_encoder_cmd      = common_video_device_vidioc_encoder_cmd,
	.vidioc_reqbufs          = common_video_device_vidioc_reqbufs,
	.vidioc_querybuf         = common_video_device_vidioc_querybuf,
	.vidioc_qbuf             = common_video_device_vidioc_qbuf,
	.vidioc_dqbuf            = common_video_device_vidioc_dqbuf,
	.vidioc_streamon         = common_video_device_vidioc_streamon,
	.vidioc_streamoff        = common_video_device_vidioc_streamoff,
	.vidioc_s_audio          = common_video_device_vidioc_s_audio,
	.vidioc_g_audio          = common_video_device_vidioc_g_audio,
	.vidioc_enumaudio		 = common_video_device_vidioc_enumaudio,
//	.vidioc_enum_dv_presets  = common_video_device_vidioc_enum_dv_presets,
	.vidioc_s_parm          = common_video_device_vidioc_s_parm,
	.vidioc_g_parm          = common_video_device_vidioc_g_parm,
	.vidioc_enum_frameintervals = common_video_device_vidioc_enum_frameintervals,
	.vidioc_enum_framesizes	= common_video_device_vidioc_enum_framesizes,
	.vidioc_cropcap			= common_video_device_vidioc_cropcap,
#ifdef CONFIG_VIDEO_ADV_DEBUG
	.vidioc_g_register = tw5864_g_reg,
	.vidioc_s_register = tw5864_s_reg,
#endif


};
#endif

#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)) || (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,19))

static struct v4l2_tvnorm LINUXV4L2_V4L2_TVNORM[ 2 ] = 
{
	{	.name = "V4L2_STD_525_60",
		.id   = V4L2_STD_525_60,
	},
	{	.name = "V4L2_STD_625_50",
		.id   = V4L2_STD_625_50,
	},
};

#endif

static struct videobuf_queue_ops LINUXV4L2_VIDEOBUF_QUEUE_OPS =
{
	.buf_setup   = common_videobuf_queue_ops_buf_setup,
	.buf_prepare = common_videobuf_queue_ops_buf_prepare,
	.buf_queue   = common_videobuf_queue_ops_buf_queue,
	.buf_release = common_videobuf_queue_ops_buf_release,
};

static struct snd_kcontrol_new LINUXV4L2_SND_KCONTROL_NEW_VOLUME = {

	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name  = "VOLUME",
	.info  = common_snd_kcontrol_new_volume_info,
	.get   = common_snd_kcontrol_new_volume_get,
	.put   = common_snd_kcontrol_new_volume_put,
};

static struct snd_pcm_ops LINUXV4L2_SND_PCM_OPS =
{
	.open      = common_snd_pcm_ops_open,
	.close     = common_snd_pcm_ops_close,
	.ioctl     = snd_pcm_lib_ioctl,
	.hw_params = common_snd_pcm_ops_hw_params,
	.hw_free   = common_snd_pcm_ops_hw_free,
	.prepare   = common_snd_pcm_ops_prepare,
	.trigger   = common_snd_pcm_ops_trigger,
	.pointer   = common_snd_pcm_ops_pointer,
	.page      = snd_pcm_sgbuf_ops_page,
};

static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_FH8735 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_32000,
	.rate_min         = 32000,
	.rate_max         = 48000,
	.channels_min     = 2,
	.channels_max     = 2,
	.buffer_bytes_max = 8192 * 64,
	.period_bytes_min = 8192,
	.period_bytes_max = 8192,
	.periods_max      = 64,
	.periods_min      = 64,
};

static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_FH8735_SC3A0 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000,
	.rate_min         = 48000,
	.rate_max         = 48000,
#ifdef SC3A0_STEREO 
	.channels_min     = 2,
	.channels_max     = 2,
	.buffer_bytes_max = 4096 * 64,
	.period_bytes_min = 4096,
	.period_bytes_max = 4096,
#else
	.channels_min     = 1,
	.channels_max     = 1,
	.buffer_bytes_max = 2048 * 64,
	.period_bytes_min = 2048,
	.period_bytes_max = 2048,
#endif
	.periods_max      = 64,
	.periods_min      = 64,
};

static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_SA7160 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_32000,
	.rate_min         = 32000,
	.rate_max         = 48000,
	.channels_min     = 2,
	.channels_max     = 2,
	.buffer_bytes_max = 4096 * 64,
	.period_bytes_min = 4096,
	.period_bytes_max = 4096,
	.periods_max      = 64,
	.periods_min      = 64,
};


static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC5C0 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_32000,
	.rate_min         = 32000,
	.rate_max         = 48000,
	.channels_min     = 2,
	.channels_max     = 2,
	.buffer_bytes_max = 4096 * 64,
	.period_bytes_min = 4096,
	.period_bytes_max = 4096,
	.periods_max      = 64,
	.periods_min      = 64,
};

static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC3C0 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000,
	.rate_min         = 48000,
	.rate_max         = 48000,
	.channels_min     = 1,
	.channels_max     = 1,
	.buffer_bytes_max = 4096 * 64,
	.period_bytes_min = 4096,
	.period_bytes_max = 4096,
	.periods_max      = 64,
	.periods_min      = 64,
};

static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_AH8400 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_8000,
	.rate_min         = 8000,
	.rate_max         = 8000,
	.channels_min     = 1,
	.channels_max     = 1,
//below is for G721
//	.buffer_bytes_max = 80 * 64,
//	.period_bytes_min = 80,
//	.period_bytes_max = 80,

	.buffer_bytes_max = 320 * 64,//one byte in, 4 bytes out
	.period_bytes_min = 320,
	.period_bytes_max = 320,

	.periods_max      = 64,
	.periods_min      = 64,
};


static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_TW5864 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_8000,
	.rate_min         = 8000,
	.rate_max         = 8000,
	.channels_min     = 1,
	.channels_max     = 1,
	.buffer_bytes_max = 498 * 64,
	.period_bytes_min = 498,
	.period_bytes_max = 498,
	.periods_max      = 64,
	.periods_min      = 64,
};

static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_CX2581 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000 ,
	.rate_min         = 48000,
	.rate_max         = 48000,
	.channels_min     = 1,
	.channels_max     = 1,
	.buffer_bytes_max = 960 * 64,
	.period_bytes_min = 960,
	.period_bytes_max = 960,
	.periods_max      = 64,
	.periods_min      = 64,
};

//peter [2011.07.15]
static struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000,
	.rate_min         = 48000,
	.rate_max         = 48000,
	.channels_min     = 1,
	.channels_max     = 1,
	.buffer_bytes_max = 960 * 64,
	.period_bytes_min = 960,
	.period_bytes_max = 960,
	.periods_max      = 64,
	.periods_min      = 64,
};

static ULONG LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ 19 ][ 22 ] = 
{
	{ 0x1797, 0x6801, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6802, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6804, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6805, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6810, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6811, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6812, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6813, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6814, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6815, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6816, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x6817, 128, 100, 128, 128,  16, 1, 120, 2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x14F1, 0x8210, 128, 128, 128, 128, 128, 1, 256, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x14F1, 0x5851, 128, 128, 128, 128, 128, 1, 256, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1131, 0x7160, 128, 128, 128, 128, 128, 1, 256, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1999, 0xA04C, 128, 128, 128, 128,  16, 1, 256, 0, 0x00000000, 0x00000000, 0x00000003, 0x00000001, 0x3FFFFFFF, 0x00FFFFFF, 0x00000008, 0x00000008, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x3086, 0x0101, 128, 128, 128, 128, 128, 1, 256, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x1797, 0x5864, 128, 128, 128, 128, 128, 1, 256, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
	{ 0x0907, 0x0100, 128, 128, 128, 128, 128, 1, 256, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 /*GPIO DIRECTION*/, 0x00000000 /*GPIO DATA*/ },
};

static ULONG LINUXV4L2_V4L2_QUERYCTRL_ENABLE[ 19 ][ 22 ] = 
{
	{ 0x1797, 0x6801, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6802, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6804, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6805, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6810, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6811, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6812, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6813, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6814, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6815, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6816, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x6817, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x14F1, 0x8210, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x14F1, 0x5851, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1131, 0x7160, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1999, 0xA04C, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x3086, 0x0101, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x1797, 0x5864, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
	{ 0x0907, 0x0100, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 /*GPIO DIRECTION*/, 1 /*GPIO DATA*/ },
};

static struct v4l2_queryctrl LINUXV4L2_V4L2_QUERYCTRL[ ] = 
{
	{	.id            =  V4L2_CID_BRIGHTNESS,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "BRIGHTNESS",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	}, 
	{	.id            =  V4L2_CID_CONTRAST,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "CONTRAST",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	}, 
	{	.id            =  V4L2_CID_SATURATION,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SATURATION",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	}, 
	{	.id            =  V4L2_CID_HUE,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "HUE",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SHARPNESS,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SHARPNESS",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_AUTOGAIN,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "AUTO GAIN",
		.minimum       =  0,
		.maximum       =  1,
		.step          =  1,
		.default_value =  1,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_GAIN,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "GAIN",
		.minimum       =  0,
		.maximum       =  511,
		.step          =  1,
		.default_value =  256,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SWITCH_SPEED,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SWITCH SPEED",
		.minimum       =  0,
		.maximum       =  2,
		.step          =  1,
		.default_value =  1,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SWITCH_TABLE,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SWITCH TABLE",
		.minimum       =  0x00000000,
		.maximum       =  0x00FFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_STATUS,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION STATUS",
		.minimum       =  0,
		.maximum       =  1,
		.step          =  1,
		.default_value =  0,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION SENSITIVITY (TH.L)",
		.minimum       =  0,
		.maximum       =  127,
		.step          =  1,
		.default_value =  64,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION SENSITIVITY (TH.H)",
		.minimum       =  0,
		.maximum       =  127,
		.step          =  1,
		.default_value =  64,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_MASK_CX,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION MASK (CX)",
		.minimum       =  0x00000000,
		.maximum       =  0x3FFFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_MASK_CY,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION MASK (CY)",
		.minimum       =  0x00000000,
		.maximum       =  0x00FFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_POS_X,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD POS (X)",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_POS_Y,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD POS (Y)",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_COLOR,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD COLOR",
		.minimum       =  0,
		.maximum       =  15,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_TEXT,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD TEXT",
		.minimum       =  0x00000000,
		.maximum       =  0x000000FF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_GPIO_DIRECTION,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "GPIO DIRECTION",
		.minimum       =  0x00000000,
		.maximum       =  0xFFFFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_GPIO_DATA,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "GPIO DATA",
		.minimum       =  0x00000000,
		.maximum       =  0xFFFFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)

extern struct video_device * video_devdata( struct file * p_file );

static inline void * video_get_drvdata( struct video_device * p_dev ) { return p_dev->priv; }

static inline void video_set_drvdata( struct video_device * p_dev, void * p_data ) { p_dev->priv = p_data; }

#endif



#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

#define init_MUTEX(sem)         sema_init(sem, 1)
#define init_MUTEX_LOCKED(sem)  sema_init(sem, 0)

#endif

ULONG g_n_analog_decoder_brightness                    [ 64 ][ 4 ] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

ULONG g_n_analog_decoder_contrast                      [ 64 ][ 4 ] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

ULONG g_n_analog_decoder_saturation                    [ 64 ][ 4 ] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

ULONG g_n_analog_decoder_hue                           [ 64 ][ 4 ] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

ULONG g_n_analog_decoder_sharpness                     [ 64 ][ 4 ] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

ULONG g_n_analog_decoder_gain_auto                     [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_analog_decoder_gain                          [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//ULONG g_n_analog_decoder_lock_status                   [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//peter [2011.04.12]
ULONG g_n_analog_decoder_lock_status                   [ 64 ] = { 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102 };


ULONG g_n_multiplexer_motion_status                    [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_multiplexer_motion_sensitivity_low_threshold [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_multiplexer_motion_sensitivity_high_threshold[ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_multiplexer_motion_mask_cx                   [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_multiplexer_motion_mask_cy                   [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


ULONG g_n_encoder_osd_pos_x                            [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_encoder_osd_pos_y                            [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_encoder_osd_color                            [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

CHAR  g_p_encoder_osd_text                             [ 64 ][ 64 + 1 ] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };


ULONG g_n_gpio_direction                               [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

ULONG g_n_gpio_data                                    [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

CDevice * g_pDevice[ 128 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };


//below is from kernel 3.2
//////////////////////////////////////////////////////////////////////////////////////////////////////

// ##############################################################################################################################################################################
//

static int Init_pci_driver( void * p_context )
{
	int ret = 0;

	ret = pci_register_driver( p_context );

	if( ret != 0 ) { 
		
		LINUXV4L2_DEBUG( KERN_ERR, "Init_pci_driver() - pci_register_driver() - [FAIL]\n" ); 
		
		return ret; 
	}

}

static int __init common_module_on_init( void )
{	
	int ret = 0;

	LINUXV4L2_PRINT( KERN_INFO, "common_module_on_init(), 1.1.0.101.285, 2015.04.09 PAL\n" );

#ifdef MEDICAL
	LINUXV4L2_PRINT( KERN_INFO, "MEDICAL\n" );
#endif

#ifdef FFMPEG
	LINUXV4L2_PRINT( KERN_INFO, "FFMPEG\n" );
#endif

	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(ULONG)(0x%x)\n", sizeof(ULONG) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(SHORT)(0x%x)\n", sizeof(SHORT) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(LONG)(0x%x)\n", sizeof(LONG) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(LONGLONG)(0x%x)\n", sizeof(LONGLONG) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(UINT)(0x%x)\n", sizeof(UINT) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(WORD)(0x%x)\n", sizeof(WORD) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(DWORD)(0x%x)\n", sizeof(DWORD) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(DWORD*)(0x%x)\n", sizeof(DWORD*) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(UINT*)(0x%x)\n", sizeof(UINT*) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(LONG*)(0x%x)\n", sizeof(LONG*) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(BYTE*)(0x%x)\n", sizeof(BYTE*) );

//	ippStaticInit();

#ifdef AME_FH8735
	//ret = pci_register_driver( &linuxv4l2_pci_driver8735 );

	struct task_struct * init_thread = kthread_run( Init_pci_driver, &linuxv4l2_pci_driver8735, "Init_pci_driver" );

#endif

#ifdef AME_SA7160
	//ret = pci_register_driver( &linuxv4l2_pci_driver7160 );
	
	struct task_struct * init_thread = kthread_run( Init_pci_driver, &linuxv4l2_pci_driver7160, "Init_pci_driver" );

#endif

#ifdef AME_MZ0380

	struct task_struct * init_thread = kthread_run( Init_pci_driver, &linuxv4l2_pci_driver0380, "Init_pci_driver" );

#endif

	//if( ret != 0 ) { 
	if( IS_ERR( init_thread ) ) {
		
		LINUXV4L2_DEBUG( KERN_ERR, "common_module_on_init() - kthread_run() - [FAIL]\n" ); 
		
		return ret; 
	}
	return 0;
}

static void __exit common_module_on_exit( void )
{
	LINUXV4L2_DEBUG( KERN_INFO, "common_module_on_exit()\n" );

#ifdef AME_FH8735
	pci_unregister_driver( &linuxv4l2_pci_driver8735 );
#endif

#ifdef AME_SA7160
	pci_unregister_driver( &linuxv4l2_pci_driver7160 );
#endif

#ifdef AME_MZ0380
	pci_unregister_driver( &linuxv4l2_pci_driver0380 );
#endif

}

// ##############################################################################################################################################################################
// 
static ULONG g_n_counter_of_board_number         = 0x00000000;
static ULONG g_n_counter_of_video_number         = 0x00000000;
static ULONG g_n_counter_of_audio_number         = 0x00000000;
static ULONG g_n_counter_of_video_encoder_number = 0x00000000;
static ULONG g_n_counter_of_audio_encoder_number = 0x00000000;

static int __devinit common_pci_driver_on_probe( struct pci_dev * p_pci_dev, const struct pci_device_id * p_pci_device_id )
{
	CDevice * pDevice = NULL;

	USHORT w_vender_id = 0x0000;

	USHORT w_device_id = 0x0000;

	USHORT w_subsystem_vender_id = 0x0000;

	USHORT w_subsystem_id = 0x0000;

	CHAR psz[ 256 ];

	int ret = 0;

	int i = 0;

	pci_read_config_word( p_pci_dev, 0x00, &w_vender_id );

	pci_read_config_word( p_pci_dev, 0x02, &w_device_id );

	pci_read_config_word( p_pci_dev, 0x2C, &w_subsystem_vender_id );

	pci_read_config_word( p_pci_dev, 0x2E, &w_subsystem_id );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe( %02X:%06X:%04X:%04X:%04X:%04X (%04X:%04X:%04X:%04X) )\n", (int)(g_n_counter_of_board_number), p_pci_dev->bus->number, p_pci_dev->class, p_pci_dev->vendor, p_pci_dev->device, p_pci_dev->subsystem_vendor, p_pci_dev->subsystem_device, w_vender_id, w_device_id, w_subsystem_vender_id, w_subsystem_id );

//	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 && p_pci_dev->subsystem_vendor == 0xF519 && p_pci_dev->subsystem_device == 0x12AB )
//	{ 
//		return 0;
//	}

	//
	// 
	pDevice = (CDevice *)(kmalloc( sizeof(struct CDevice), GFP_KERNEL ));

	if( pDevice == NULL ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - kmalloc() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		ret = -ENOMEM;

		goto common_pci_driver_on_probe_fail;
	}
	else { 

		pci_set_drvdata( p_pci_dev, (PVOID)(pDevice) );

		memset( pDevice, 0, sizeof(struct CDevice) );

		tasklet_init( &(pDevice->m_sTasklet), common_pci_driver_on_tasklet_EX, (unsigned long)(pDevice) );

		spin_lock_init( &(pDevice->m_sTaskletAccessSpinLock) );
// peter [2011.05.09]
		init_MUTEX( &(pDevice->m_sComponentAccessSemaphore) );

		init_MUTEX( &(pDevice->m_sSGSemaphore) ); 

		init_MUTEX( &(pDevice->m_FH8735AccessSemaphore) ); 

		init_MUTEX( &(pDevice->m_MZ0380AccessSemaphore) ); 

		init_MUTEX( &(pDevice->m_sI2CAccessSemaphore) );

		pDevice->m_pKsDevice = p_pci_dev;

		pDevice->m_nKsDeviceNumber = g_n_counter_of_board_number;

		pDevice->m_nKsDeviceBusNumber = p_pci_dev->bus->number;

		pDevice->m_nKsDeviceVideoNumber = 0x00000000;

		pDevice->m_nKsDeviceAudioNumber = 0x00000000;

		pDevice->m_nKsDeviceVideoEncoderNumber = 0x00000000;

		pDevice->m_nKsDeviceAudioEncoderNumber = 0x00000000;

		pDevice->m_nTaskletExtraParameterA = 0x00000000;

		pDevice->m_nTaskletExtraParameterB = 0x00000000;

		pDevice->m_hInterruptAccessLock = FALSE;

		pDevice->m_nDigitalCaptureStreamPowerReference = 0x00000000;

		pDevice->m_nAnalogCaptureStreamPowerReference = 0x00000000;

		pDevice->m_nCustomGpioDirectionProperty = 0;

		pDevice->m_nCustomGpioDataProperty = 0;
		
		pDevice->m_nCustomGpioSupportProperty = 0;

		pDevice->m_nCustomSoftwareWatchdogDurationProperty = 180; // shawn 2010.08.11

		pDevice->m_nCustomAnalogVideoScaleOutputProperty = 1; // shawn 2010.10.04

		pDevice->m_nAnalogCopyProtMacrovisionProperty = 0; // shawn 2010.10.19

		pDevice->m_nAnalogVideoDecoderStatusProperty = 0; // shawn 2010.10.19

		pDevice->m_nAnalogCrossbarVideoInputProperty = 4; // shawn 2010.10.25

		if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 )
		{
			if( (p_pci_dev->subsystem_vendor & 0xFF) == 0x1A ||
				(p_pci_dev->subsystem_vendor & 0xFF) == 0x18 ||
				(p_pci_dev->subsystem_vendor & 0xFF) == 0x16 ||
				(p_pci_dev->subsystem_vendor & 0xFF) == 0x0D ||
				(p_pci_dev->subsystem_vendor & 0xFF) == 0x0A ||
				(p_pci_dev->subsystem_vendor & 0xFF) == 0x13 )
			{
				pDevice->m_nAnalogCrossbarVideoInputProperty = 0; // shawn 2010.10.25
			}
			if( (p_pci_dev->subsystem_vendor & 0xFF) == 0x04 )//Euresys SC512L-DVI
			{
				pDevice->m_nAnalogCrossbarVideoInputProperty = 1;
			}
		}
		if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 )
		{
			pDevice->m_nAnalogCrossbarVideoInputProperty = 1; // shawn 2010.10.25
		}


		LINUXV4L2_PRINT( KERN_INFO, "m_nAnalogCrossbarVideoInputProperty(0d%d)\n",pDevice->m_nAnalogCrossbarVideoInputProperty );

		pDevice->m_nAnalogCrossbarAudioInputProperty = 0;//compatible to previous, do not change

//		pDevice->m_nAnalogCrossbarVideoInputProperty = 6; // shawn 2010.10.25

//		pDevice->m_nAnalogCrossbarVideoInputProperty = 0; // shawn 2010.10.25

//HDMI 0
//DVI.DIGITAL (TMDS.A) 1
//COMPONENT 2
//DVI.ANALOG 3
//SDI 4
//COMPOSITE 5
//S 6
		pDevice->m_pCustomSystemConfigProperty = NULL; // shawn 2010.10.25


		pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

		pDevice->m_nCustomAnalogVideoInterleavedProperty = 1;//for analog signal

		pDevice->m_nCustomSoftwareWatchdogResetProperty = 0;

		pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 1;

		pDevice->m_nHVScaleFactor = 0x00010001;

		pDevice->m_nH264HVScaleFactor = 0x00000202;

		pDevice->m_nCustomAnalogVideoVgaHActiveProperty = 640;

		pDevice->m_nCustomAnalogVideoVgaAspectRatioProperty =  0;

		pDevice->m_nCustomCompanyVision4CEProperty = 0;

		pDevice->m_nCustomAnalogVideoColorRangeProperty = 0x00010000;

		pDevice->m_MST3367_bank = 0xFF;//invalid value

		pDevice->n_video_buffer_count = 0;

		pDevice->m_nCustomCompanyIvsProperty = 0;

		pDevice->m_nCustomSpecialCameraInputProperty = 0;

		pDevice->m_nCustomAnalogVideoVbiLinesEnableProperty = 0;
		
		pDevice->m_nCustomAnalogVideoVbiLinesProperty = 0;

		pDevice->m_No_Signal_black_pattern_property = 0;


		for( i = 0 ; i < 8 ; i++ ) {

			pDevice->m_CX2581_video_buffer_count[ i ] = 0;
		}

		pDevice->m_ReadyToStopControlThread = 0;

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			pDevice->m_pKsVideo[ i ] = NULL;

			pDevice->m_pKsAudioCard[ i ] = NULL;

			pDevice->m_pKsAudioPCM[ i ] = NULL;

			pDevice->m_pVideo[ i ] = NULL;

			pDevice->m_pAudio[ i ] = NULL;

			pDevice->m_eVideoStreamState[ i ] = 0x00000000;

			pDevice->m_eAudioStreamState[ i ] = 0x00000000;
		}
		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			pDevice->m_pKsVideoEncoder[ i ] = NULL;

			pDevice->m_pKsAudioEncoderCard[ i ] = NULL;

			pDevice->m_pKsAudioEncoderPCM[ i ] = NULL;

			pDevice->m_pVideoEncoder[ i ] = NULL;

			pDevice->m_pAudioEncoder[ i ] = NULL;

			pDevice->m_eVideoEncoderStreamState[ i ] = 0x00000000;

			pDevice->m_eAudioEncoderStreamState[ i ] = 0x00000000;
		}
		for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

			pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ i ] = 0;

			pDevice->m_nDmaBaseCommonBufferNumber[ i ] = 0;

			pDevice->m_nDmaBaseCommonBufferSize[ i ] = 0;

			pDevice->m_pDmaBaseCommonBuffer[ i ] = NULL;

			pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] = 0x00000000; //
		}
		for( i = 0 ; i < MAX_REG_BUFFER_NUM ; i++ ) {

			pDevice->m_nRegBaseCommonBufferSize[ i ] = 0;

			pDevice->m_pRegBaseCommonBuffer[ i ] = NULL;
		}

		//
		// 
		pDevice->m_nCustomHardwareTopologyProperty = 0x00000000; 

		pDevice->m_nCustomVideoStandardProperty = LINUXV4L2_DEFAULT_VIDEO_STANDARD;

		pDevice->m_nCustomAudioStandardProperty = 0x00000000;

		pDevice->m_IRQ_ready = 1; //0 is got IRQ

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			pDevice->m_Preview_Opened_counter[ i ] = 0;
		}

		pDevice->m_Preview_frame_rate_setting = 0x800186A0; //100000;//max
	}

	ret = pci_enable_device( p_pci_dev );

	if( ret != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_enable_device() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}

	pDevice->CFG0 = 0x00000000; pci_read_config_dword( p_pci_dev, 0x00, &(pDevice->CFG0) );

	pDevice->CFGB = 0x00000000; pci_read_config_dword( p_pci_dev, 0x2C, &(pDevice->CFGB) );

	pDevice->CFG2 = 0x00000000; pci_read_config_dword( p_pci_dev, 0x08, &(pDevice->CFG2) );

	pDevice->idVendor      = (USHORT)((pDevice->CFG0 & 0x0000FFFF) >>  0);
	pDevice->idProduct     = (USHORT)((pDevice->CFG0 & 0xFFFF0000) >> 16);
	pDevice->bcdDevice     = (USHORT)((pDevice->CFGB & 0xFFFF0000) >> 16);
	pDevice->iManufacturer = (  BYTE)((pDevice->CFGB & 0x000000FF) >>  0);
	pDevice->iProduct      = (  BYTE)((pDevice->CFGB & 0x0000FF00) >>  8);
	pDevice->iSerialNumber = (  BYTE)((pDevice->CFG2 & 0x000000FF) >>  0);

	LINUXV4L2_DEBUG( KERN_INFO, "    idVendor           = %04X\n", pDevice->idVendor );
	LINUXV4L2_DEBUG( KERN_INFO, "    idProduct          = %04X\n", pDevice->idProduct );
	LINUXV4L2_DEBUG( KERN_INFO, "    bcdDevice          = %04X\n", pDevice->bcdDevice );
	LINUXV4L2_DEBUG( KERN_INFO, "    iManufacturer      = %02X\n", pDevice->iManufacturer );
	LINUXV4L2_DEBUG( KERN_INFO, "    iProduct           = %02X\n", pDevice->iProduct );
	LINUXV4L2_DEBUG( KERN_INFO, "    iSerialNumber      = %02X\n", pDevice->iSerialNumber );


	pci_set_master( p_pci_dev );

	//pci_dma_supported( p_pci_dev, DMA_32BIT_MASK );
	ret = pci_set_dma_mask( p_pci_dev, DMA_32BIT_MASK );

	if( ret != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_dma_supported() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}

	ret = pci_request_regions( p_pci_dev, "LINUXV4L2" );

	if( ret != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_request_regions() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}

	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6802 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                     1920 * 4 * 1; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6805 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                     1920 * 4 * 1; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6814 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                     1920 * 4 * 1; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6815 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                     1920 * 4 * 1; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6816 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                     1920 * 4 * 1; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6817 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW6805_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] =                     1920 * 4 * 1; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = CX2581_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = CX2581_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                     1920 * 4 * 4; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x5851 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = CX2588_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = CX2588_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =                     1920 * 4 * 4; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = SA7160_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = SA7160_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] =	SA7160_MAX_XXVBI_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =								0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x9413 && p_pci_dev->device == 0x6010 ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = SL6010_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = SL6010_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = SL6010_MAX_XXVBI_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = SL6010_MAX_MPEG4_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = AH8400_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = AH8400_MAX_AUDIO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = AH8400_MAX_HX264_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] =                                0; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0; pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0; }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864 ) 
	{	
		pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; 
		pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = TW5864_MAX_HX264_DMA_BUFFER_SIZE; 
		pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 7 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = TW5864_MAX_VIDEO_DMA_BUFFER_SIZE;
	}

//peter[2011.06.29]
	if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 )
	{ 
		pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = FH8735_MAX_HD_VIDEO_DMA_BUFFER_SIZE; 
		pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = FH8735_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = FH8735_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = FH8735_MAX_VIDEO_DMA_BUFFER_SIZE; pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = FH8735_MAX_HX264_DMA_BUFFER_SIZE; 
		pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = FH8735_MAX_HD_VIDEO_DMA_BUFFER_SIZE;
#ifdef FFMPEG
		pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = FH8735_MAX_HX264_DMA_BUFFER_SIZE; //H264 main temp storage
		pDevice->m_nDmaBaseCommonBufferSize[ 7 ] = FH8735_MAX_HX264_DMA_BUFFER_SIZE; //H264 sub temp storage
#endif //FFMPEG
	}
	if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) 
	{
		if( ((pDevice->iProduct & 0x0F) == 0x02 && (pDevice->iManufacturer & 0x0F) == 0x0A) || // 960H & D1
			
			((pDevice->iProduct & 0x0F) == 0x03 && (pDevice->iManufacturer & 0x0F) == 0x0A) ){ // 960H & D1

			//buffer 0~7 preview, buffer 8~15 audio, buffer 16~23 main H264
			pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 7 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			/*
			pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 9 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 10 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 11 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 12 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 13 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 14 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 15 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			*/
			pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = MZ0380_MAX_AUDIO_SD_DMA_BUFFER_SIZE;

			pDevice->m_nDmaBaseCommonBufferSize[ 16 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 17 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 18 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 19 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 20 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 21 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 22 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 23 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;

			pDevice->m_nDmaBaseCommonBufferSize[ 24 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 25 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 26 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 27 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 28 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 29 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 30 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 31 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;

		}
		else if( ((pDevice->iProduct & 0x0F) == 0x02 && (pDevice->iManufacturer & 0x0F) == 0x0C) || // 960H & D1
			
				 ((pDevice->iProduct & 0x0F) == 0x03 && (pDevice->iManufacturer & 0x0F) == 0x0B) ){ // 960H & D1


			//buffer 0~7 preview, buffer 8~15 audio, buffer 16~23 main H264
			pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			//pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			//pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE; 
			//pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 7 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			/*
			pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 9 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 10 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 11 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 12 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 13 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 14 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 15 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE;
			*/
			pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = MZ0380_MAX_AUDIO_SD_DMA_BUFFER_SIZE;

			pDevice->m_nDmaBaseCommonBufferSize[ 16 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 17 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 18 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			pDevice->m_nDmaBaseCommonBufferSize[ 19 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 20 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 21 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 22 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 23 ] = MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE;

			//pDevice->m_nDmaBaseCommonBufferSize[ 24 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 25 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 26 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 27 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 28 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 29 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 30 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;
			//pDevice->m_nDmaBaseCommonBufferSize[ 31 ] = MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE;

		}
		else {
			//buffer 0, 2 preview, buffer 1 audio, buffer 3,4 main H264, buffer 5,6 sub H264
			/*
			pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE;
			*/
			//buffer 0, 1, 2, 3 preview, buffer 4 audio, buffer 5,6, 7, 8 main H264, buffer 9, 10, 11, 12 sub H264
			pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE; 

			pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE; 

			pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 7 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 

			pDevice->m_nDmaBaseCommonBufferSize[ 9 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 10 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 11 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 
			pDevice->m_nDmaBaseCommonBufferSize[ 12 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; 

#ifdef FFMPEG
			pDevice->m_nDmaBaseCommonBufferSize[ 13 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; //H264 main temp storage
			pDevice->m_nDmaBaseCommonBufferSize[ 14 ] = MZ0380_MAX_MPEG_DMA_BUFFER_SIZE; //H264 sub temp storage
#endif //FFMPEG

		}
	}
	for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

		if( pDevice->m_nDmaBaseCommonBufferSize[ i ] != 0 ) {

			pDevice->m_pDmaBaseCommonBuffer[ i ] = pci_alloc_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ i ], &pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );
			
			if( pDevice->m_pDmaBaseCommonBuffer[ i ] == NULL ) {
		
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_alloc_consistent() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
		
				ret = -ENOMEM;
		
				goto common_pci_driver_on_probe_fail;
			}

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_nDmaBaseCommonBufferSize[%d] (0d%d)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_nDmaBaseCommonBufferSize[ i ] );
			#ifdef CONFIG_64BIT
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonBuffer[%d] (llx%llx)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonBuffer[ i ] );
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonPhysicalBuffer[%d] (llx%llx)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );
			#else
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonBuffer[%d] (0x%x)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonBuffer[ i ] );
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonPhysicalBuffer[%d] (0x%x)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );
			#endif
			memset( pDevice->m_pDmaBaseCommonBuffer[ i ], 0x00, pDevice->m_nDmaBaseCommonBufferSize[ i ] );
		}
	}

	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 )
	{
	//allocate memory for SA7160 video DMA	
		int index = 0;

		memset( pDevice->m_SA7160_video_buffer, 0x00, sizeof(ULONG*) *  SA7160_MAX_BUFFER );

		memset( pDevice->m_SA7160_video_buffer_physical_address, 0x00, sizeof(ULONG) *  SA7160_MAX_BUFFER);

		for( index = 0; index < SA7160_MAX_BUFFER; index++ )
		{

			pDevice->m_SA7160_video_buffer[ index ] = pci_alloc_consistent( p_pci_dev, 1920 * 1080 * 2, &pDevice->m_SA7160_video_buffer_physical_address[ index ] );

			if(pDevice->m_SA7160_video_buffer[ index ]  == NULL)
			{
				LINUXV4L2_DEBUG( KERN_INFO, " can not be allocated m_SA7160_video_buffer[ %d ]\n", index );

				ret = -ENOMEM;

				goto common_pci_driver_on_probe_fail;

			}
			else
			{
				#ifdef CONFIG_64BIT
				LINUXV4L2_DEBUG( KERN_INFO, " M virtual(llx%llx)\n",  pDevice->m_SA7160_video_buffer[ index ] );

				LINUXV4L2_DEBUG( KERN_INFO, " M physical(llx%llx)\n",  pDevice->m_SA7160_video_buffer_physical_address[ index ]  );
				#else
				LINUXV4L2_DEBUG( KERN_INFO, " M virtual(0x%x)\n",  pDevice->m_SA7160_video_buffer[ index ] );
				LINUXV4L2_DEBUG( KERN_INFO, " M physical(0x%x)\n",  pDevice->m_SA7160_video_buffer_physical_address[ index ]  );
				#endif
			}

		}

	}

#ifdef TW6805_64bit

	if( (p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813)  )
	{
	//allocate memory for TW6816 video DMA	
		int index = 0;

		memset( pDevice->m_TW6816_video_buffer, 0x00, sizeof(ULONG*) *  TW6816_MAX_BUFFER );

		memset( pDevice->m_TW6816_video_buffer_physical_address, 0x00, sizeof(ULONG) *  TW6816_MAX_BUFFER);

		for( index = 0; index < TW6816_MAX_BUFFER; index++ )
		{

			pDevice->m_TW6816_video_buffer[ index ] = pci_alloc_consistent( p_pci_dev, 720 * 576 * 2, &pDevice->m_TW6816_video_buffer_physical_address[ index ] );

			if(pDevice->m_TW6816_video_buffer[ index ]  == NULL)
			{
				LINUXV4L2_DEBUG( KERN_INFO, " can not be allocated m_TW6816_video_buffer[ %d ]\n", index );

				ret = -ENOMEM;

				goto common_pci_driver_on_probe_fail;

			}
			else
			{
				#ifdef CONFIG_64BIT
				LINUXV4L2_DEBUG( KERN_INFO, " M virtual(llx%llx)\n",  pDevice->m_TW6816_video_buffer[ index ] );
				LINUXV4L2_DEBUG( KERN_INFO, " M physical(llx%llx)\n",  pDevice->m_TW6816_video_buffer_physical_address[ index ]  );
				#else
				LINUXV4L2_DEBUG( KERN_INFO, " M virtual(0x%x)\n",  pDevice->m_TW6816_video_buffer[ index ] );
				LINUXV4L2_DEBUG( KERN_INFO, " M physical(0x%x)\n",  pDevice->m_TW6816_video_buffer_physical_address[ index ]  );
				#endif
			}

		}

	}
#endif //TW6805_64bit

#ifdef CX2581_64bit

	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210)  
	{
	//allocate memory for CX2581 video DMA	
		int index = 0;

		int cx2581_ch = 0;

		memset( pDevice->m_CX2581_video_buffer, 0x00, sizeof(ULONG*) *  CX2581_MAX_BUFFER * 8 );

		memset( pDevice->m_CX2581_video_buffer_physical_address, 0x00, sizeof(ULONG) *  CX2581_MAX_BUFFER * 8 );

		for( cx2581_ch = 0; cx2581_ch < 8; cx2581_ch++ )
		{
			for( index = 0; index < CX2581_MAX_BUFFER; index++ )
			{

				pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] = pci_alloc_consistent( p_pci_dev, 720 * 576 * 2, &pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ] );

				if(pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ]  == NULL)
				{
					LINUXV4L2_DEBUG( KERN_INFO, " can not be allocated m_CX2581_video_buffer[ %d ][ %d ]\n", cx2581_ch, index );

					ret = -ENOMEM;

					goto common_pci_driver_on_probe_fail;

				}
				else
				{
					#ifdef CONFIG_64BIT
					LINUXV4L2_DEBUG( KERN_INFO, " M virtual(llx%llx)\n",  pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] );
					LINUXV4L2_DEBUG( KERN_INFO, " M physical(llx%llx)\n",  pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ]  );
					#else
					LINUXV4L2_DEBUG( KERN_INFO, " M virtual(0x%x)\n",  pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] );
					LINUXV4L2_DEBUG( KERN_INFO, " M physical(0x%x)\n",  pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ]  );
					#endif
				}

			}
		}
	}
#endif //CX2581_64bit

	if( (p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801) || 
	    (p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6802) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6805) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810) || 
	    (p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6814) || 
	    (p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6815) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6816) || 
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6817) || 
		(p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210) ||
		(p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x5851) ||
		(p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160) ||
		(p_pci_dev->vendor == 0x9413 && p_pci_dev->device == 0x6010) ||
		(p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864) ) { 

//if below function return 0, turn off blue tooth, Y Fi !!!!!!!!!
		pDevice->m_nRegBaseCommonBufferSize[ 0 ] = pci_resource_len( p_pci_dev, 0 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 0 ] != 0 ) {

			pDevice->m_pRegBaseCommonPhysicalBuffer[ 0 ] = pci_resource_start( p_pci_dev, 0 );

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonPhysicalBuffer[ 0 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonPhysicalBuffer[ 0 ] );

			pDevice->m_pRegBaseCommonBuffer[ 0 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 0 ), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );
		
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonBuffer[ 0 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonBuffer[ 0 ] );

			if( pDevice->m_pRegBaseCommonBuffer[ 0 ] == NULL ) {
		
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
		
				ret = -EIO;
		
				goto common_pci_driver_on_probe_fail;
			}
		}
		else
		{
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_resource_len() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
	
			ret = -EIO;
	
			goto common_pci_driver_on_probe_fail;
		}
	}
	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { 

		pDevice->m_nRegBaseCommonBufferSize[ 0 ] = pci_resource_len( p_pci_dev, 2 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 0 ] != 0 ) {

			pDevice->m_pRegBaseCommonBuffer[ 0 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 2 ), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );
		
			if( pDevice->m_pRegBaseCommonBuffer[ 0 ] == NULL ) {
		
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
		
				ret = -EIO;
		
				goto common_pci_driver_on_probe_fail;
			}
		}
		pDevice->m_nRegBaseCommonBufferSize[ 1 ] = pci_resource_len( p_pci_dev, 3 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 1 ] != 0 ) {

			pDevice->m_pRegBaseCommonBuffer[ 1 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 3 ), pDevice->m_nRegBaseCommonBufferSize[ 1 ] );
		
			if( pDevice->m_pRegBaseCommonBuffer[ 1 ] == NULL ) {
		
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
		
				ret = -EIO;
		
				goto common_pci_driver_on_probe_fail;
			}
		}
	}

	if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 ) { 

		pDevice->m_nRegBaseCommonBufferSize[ 0 ] = pci_resource_len( p_pci_dev, 0 );

		if(pDevice->m_nRegBaseCommonBufferSize[ 0 ] > FH35_MAX_BASE_SIZE)
		{
			pDevice->m_nRegBaseCommonBufferSize[ 0 ] = FH35_MAX_BASE_SIZE;
		}


		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() pDevice->m_nRegBaseCommonBufferSize[ 0 ](0d%d)\n", pDevice->m_nRegBaseCommonBufferSize[ 0 ] );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() addr0 (0x%x)\n", pci_resource_start( p_pci_dev, 0 ) );

		ULONG CFG10 = 0;

		pci_read_config_dword( p_pci_dev, 0x10, &CFG10 );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG10 (0x%x)\n", CFG10 );


		if( pDevice->m_nRegBaseCommonBufferSize[ 0 ] != 0 ) {

			pDevice->m_pRegBaseCommonBuffer[ 0 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 0 ), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );
			
			if( pDevice->m_pRegBaseCommonBuffer[ 0 ] == NULL ) {
			
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL] 0\n", (int)(g_n_counter_of_board_number) );
			
				ret = -EIO;
			
				goto common_pci_driver_on_probe_fail;
			}
		}

		pDevice->m_nRegBaseCommonBufferSize[ 1 ] = pci_resource_len( p_pci_dev, 1 );

		if(pDevice->m_nRegBaseCommonBufferSize[ 1 ] > FH35_MAX_BASE_SIZE)
		{
			pDevice->m_nRegBaseCommonBufferSize[ 1 ] = FH35_MAX_BASE_SIZE;
		}

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() pDevice->m_nRegBaseCommonBufferSize[ 1 ](0d%d)\n", pDevice->m_nRegBaseCommonBufferSize[ 1 ] );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() addr1 (0x%x)\n", pci_resource_start( p_pci_dev, 1 ) );

		ULONG CFG14 = 0;

		pci_read_config_dword( p_pci_dev, 0x14, &CFG14 );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG14 (0x%x)\n", CFG14 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 1 ] != 0 ) {

			pDevice->m_pRegBaseCommonBuffer[ 1 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 1 ), pDevice->m_nRegBaseCommonBufferSize[ 1 ] );
			
			if( pDevice->m_pRegBaseCommonBuffer[ 1 ] == NULL ) {
			
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL] 1\n", (int)(g_n_counter_of_board_number) );
			
				ret = -EIO;
			
				goto common_pci_driver_on_probe_fail;
			}
		}

		pDevice->m_nRegBaseCommonBufferSize[ 2 ] = pci_resource_len( p_pci_dev, 2 );

		if(pDevice->m_nRegBaseCommonBufferSize[ 2 ] > FH35_MAX_BASE_SIZE)
		{
			pDevice->m_nRegBaseCommonBufferSize[ 2 ] = FH35_MAX_BASE_SIZE;
		}

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() pDevice->m_nRegBaseCommonBufferSize[ 2 ](0d%d)\n", pDevice->m_nRegBaseCommonBufferSize[ 2 ] );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() addr2 (0x%x)\n", pci_resource_start( p_pci_dev, 2 ) );

		ULONG CFG18 = 0;

		pci_read_config_dword( p_pci_dev, 0x18, &CFG18 );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG18 (0x%x)\n", CFG18 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 2 ] != 0 ) {

			pDevice->m_pRegBaseCommonBuffer[ 2 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 2 ), pDevice->m_nRegBaseCommonBufferSize[ 2 ] );
			
			if( pDevice->m_pRegBaseCommonBuffer[ 2 ] == NULL ) {
			
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL] 2\n", (int)(g_n_counter_of_board_number) );
			
				ret = -EIO;
			
				goto common_pci_driver_on_probe_fail;
			}
		}

		pDevice->m_nRegBaseCommonBufferSize[ 3 ] = pci_resource_len( p_pci_dev, 3 );

		if(pDevice->m_nRegBaseCommonBufferSize[ 3 ] > FH35_MAX_BASE_SIZE)
		{
			pDevice->m_nRegBaseCommonBufferSize[ 3 ] = FH35_MAX_BASE_SIZE;
		}

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() pDevice->m_nRegBaseCommonBufferSize[ 3 ](0d%d)\n", pDevice->m_nRegBaseCommonBufferSize[ 3 ] );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() addr3 (0x%x)\n", pci_resource_start( p_pci_dev, 3 ) );

		ULONG CFG1C = 0;

		pci_read_config_dword( p_pci_dev, 0x1C, &CFG1C );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG1C (0x%x)\n", CFG1C );

		if( pDevice->m_nRegBaseCommonBufferSize[ 3 ] != 0 ) {

			pDevice->m_pRegBaseCommonBuffer[ 3 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 3 ), pDevice->m_nRegBaseCommonBufferSize[ 3 ] );
			
			if( pDevice->m_pRegBaseCommonBuffer[ 3 ] == NULL ) {
			
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL] 3\n", (int)(g_n_counter_of_board_number) );
			
				ret = -EIO;
			
				goto common_pci_driver_on_probe_fail;
			}
		}

	}
	if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 ) { 

		ULONG R400008 = readl( (pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x400000 + 0x08) ) & 0xFFFFFFFF;
		
		LINUXV4L2_DEBUG( KERN_INFO, "R400008(0x%x)------\n", R400008 );

		pDevice->m_hInterruptAccessLock = TRUE;
	}

	if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) { 

		UINT CFG10 = 0;
		UINT CFG14 = 0;
		UINT CFG18 = 0;
		UINT CFG1C = 0;
		UINT CFG20 = 0;
		UINT CFG24 = 0;

		pci_read_config_dword( p_pci_dev, 0x10, &CFG10 );
		pci_read_config_dword( p_pci_dev, 0x14, &CFG14 );
		pci_read_config_dword( p_pci_dev, 0x18, &CFG18 );
		pci_read_config_dword( p_pci_dev, 0x1C, &CFG1C );
		pci_read_config_dword( p_pci_dev, 0x20, &CFG20 );
		pci_read_config_dword( p_pci_dev, 0x24, &CFG24 );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG10 (0x%x)\n", CFG10 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG14 (0x%x)\n", CFG14 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG18 (0x%x)\n", CFG18 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG1C (0x%x)\n", CFG1C );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG20 (0x%x)\n", CFG20 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG24 (0x%x)\n", CFG24 );



		pDevice->m_nRegBaseCommonBufferSize[ 0 ] = pci_resource_len( p_pci_dev, 0 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 0 ] != 0 ) { //should be 33554432

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_nRegBaseCommonBufferSize[ 0 ] (0d%d)\n", (int)(g_n_counter_of_board_number), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );

			pDevice->m_pRegBaseCommonPhysicalBuffer[ 0 ] = pci_resource_start( p_pci_dev, 0 );

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonPhysicalBuffer[ 0 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonPhysicalBuffer[ 0 ] );

			pDevice->m_pRegBaseCommonBuffer[ 0 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 0 ), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );
		
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonBuffer[ 0 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonBuffer[ 0 ] );

			if( pDevice->m_pRegBaseCommonBuffer[ 0 ] == NULL ) {
		
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
		
				ret = -EIO;
		
				goto common_pci_driver_on_probe_fail;
			}
		}
		
		pDevice->m_nRegBaseCommonBufferSize[ 5 ] = pci_resource_len( p_pci_dev, 5 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 5 ] != 0 ) {//should be 4096

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_nRegBaseCommonBufferSize[ 5 ] (0d%d)\n", (int)(g_n_counter_of_board_number), pDevice->m_nRegBaseCommonBufferSize[ 5 ] );

			pDevice->m_pRegBaseCommonPhysicalBuffer[ 5 ] = pci_resource_start( p_pci_dev, 5 );

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonPhysicalBuffer[ 5 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonPhysicalBuffer[ 5 ] );

			pDevice->m_pRegBaseCommonBuffer[ 5 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 5 ), pDevice->m_nRegBaseCommonBufferSize[ 5 ] );

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonBuffer[ 5 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonBuffer[ 5 ] );

			if( pDevice->m_pRegBaseCommonBuffer[ 5 ] == NULL ) {
		
				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap_nocache() - [FAIL]\n", (int)(g_n_counter_of_board_number) );
		
				ret = -EIO;
		
				goto common_pci_driver_on_probe_fail;
			}
		}
		

	}

	//
	// 
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6802 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6805 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6814 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6815 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6816 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6817 ) { if( FALSE == TW6805_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210 ) { if( FALSE == CX2581_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x5851 ) { if( FALSE == CX2588_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 ) { if( FALSE == SA7160_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { if( FALSE == AH8400_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864 ) { if( FALSE == TW5864_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
	if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 ) { if( FALSE == FH8735_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }
// do not do here	
//	if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) { if( FALSE == MZ0380_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }



	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6802 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6805 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6814 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6815 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6816 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6817 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW6805" ); }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210 ) { pDevice->m_nKsDeviceVideoNumber = 8; pDevice->m_nKsDeviceAudioNumber = 8; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "CX2581" ); }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x5851 ) { pDevice->m_nKsDeviceVideoNumber = 8; pDevice->m_nKsDeviceAudioNumber = 8; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "CX2588" ); }
	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 ) { pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "SA7160" ); }
	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { pDevice->m_nKsDeviceVideoNumber = 4; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 4; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "AH8400" ); }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864 ) { pDevice->m_nKsDeviceVideoNumber = 4; pDevice->m_nKsDeviceAudioNumber = 4; pDevice->m_nKsDeviceVideoEncoderNumber = 8; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW5864" ); }

	if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) 
	{
		if( ((pDevice->iProduct & 0x0F) == 0x02) || ((pDevice->iProduct & 0x0F) == 0x03) )// 960H & D1
		{	
			if( (pDevice->iManufacturer & 0x0F) == 0x0A ) { 

				pDevice->m_nKsDeviceVideoNumber = 8; pDevice->m_nKsDeviceAudioNumber = 8; pDevice->m_nKsDeviceVideoEncoderNumber = 8; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "MZ0380" );
			}
			else
			{
				pDevice->m_nKsDeviceVideoNumber = 4; pDevice->m_nKsDeviceAudioNumber = 4; pDevice->m_nKsDeviceVideoEncoderNumber = 4; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "MZ0380" );
			}
		}
		else {

			pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 1; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "MZ0380" );
		}
	}

	if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 )
	{
		LINUXV4L2_DEBUG( KERN_INFO,"probe() iProduct(0x%x) iManufacturer(0x%x)\n", pDevice->iProduct, pDevice->iManufacturer );

		if( (pDevice->iProduct & 0x0F) == (0x03) ||	(pDevice->iManufacturer == 0x85) )
		{
			pDevice->m_nKsDeviceVideoNumber = 4; pDevice->m_nKsDeviceAudioNumber = 4; pDevice->m_nKsDeviceVideoEncoderNumber = 8; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "SC3A0" );
			
			 //clear unnecessary DMA
			if( pDevice->m_pDmaBaseCommonBuffer[ 5 ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ 5 ], pDevice->m_pDmaBaseCommonBuffer[ 5 ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ 5 ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 5 ] = 0;
					
				pDevice->m_nDmaBaseCommonBufferNumber[ 5 ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ 5 ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ 5 ] = 0x00000000;
			}


		}
		else if( (pDevice->iProduct & 0x0F) == (0x05) || (pDevice->iProduct & 0x0F) == (0x07) )
		{
			pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 2; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "SC580" );

			 //clear unnecessary DMA
			if( pDevice->m_pDmaBaseCommonBuffer[ 1 ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ 1 ], pDevice->m_pDmaBaseCommonBuffer[ 1 ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = 0;
					
				pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ 1 ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ] = 0x00000000;
			}
			if( pDevice->m_pDmaBaseCommonBuffer[ 2 ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ 2 ], pDevice->m_pDmaBaseCommonBuffer[ 2 ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ 2 ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 0;
					
				pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ 2 ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ 2 ] = 0x00000000;
			}

			if( pDevice->m_pDmaBaseCommonBuffer[ 3 ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ 3 ], pDevice->m_pDmaBaseCommonBuffer[ 3 ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ 3 ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0;
					
				pDevice->m_nDmaBaseCommonBufferNumber[ 3 ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ 3 ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ 3 ] = 0x00000000;
			}

		}
		else
		{
			 pDevice->m_nKsDeviceVideoNumber = 4; pDevice->m_nKsDeviceAudioNumber = 4; pDevice->m_nKsDeviceVideoEncoderNumber = 8; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "SC3A0" );

			 //clear unnecessary DMA
			if( pDevice->m_pDmaBaseCommonBuffer[ 5 ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ 5 ], pDevice->m_pDmaBaseCommonBuffer[ 5 ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ 5 ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 5 ] = 0;
					
				pDevice->m_nDmaBaseCommonBufferNumber[ 5 ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ 5 ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ 5 ] = 0x00000000;
			}

		}
	}

	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { 

		if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000002 ) { pDevice->m_nKsDeviceAudioNumber = 16; }

		if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) { pDevice->m_nKsDeviceAudioNumber =  4; }
	}

//peter [2011.03.31]
//	pDevice->m_nKsDeviceAudioNumber = 0;
//peter [2011.07.15]
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864 ) { pDevice->m_nKsDeviceAudioNumber = 4;}

#ifdef VTC
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864 ) { pDevice->m_nKsDeviceVideoNumber = 4; pDevice->m_nKsDeviceAudioNumber = 0; pDevice->m_nKsDeviceVideoEncoderNumber = 12; pDevice->m_nKsDeviceAudioEncoderNumber = 0; sprintf( psz, "TW5864" ); }
#endif //VTC

	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6805 ) { pDevice->m_nKsDeviceAudioNumber = 1;}
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6814 ) { pDevice->m_nKsDeviceAudioNumber = 1;}
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6815 ) { pDevice->m_nKsDeviceAudioNumber = 1;}
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6816 ) { pDevice->m_nKsDeviceAudioNumber = 1;}
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6817 ) { pDevice->m_nKsDeviceAudioNumber = 1;}

	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 && (w_subsystem_vender_id & 0xFF) == 0x19 ) { pDevice->m_nKsDeviceVideoNumber = 0; pDevice->m_nKsDeviceAudioNumber = 0;}

	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210 ) { pDevice->m_nKsDeviceAudioNumber = 8;}

	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { pDevice->m_nKsDeviceAudioNumber = 4;}

	//
	//

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	ret = v4l2_device_register(&p_pci_dev->dev, &pDevice->v4l2_dev);
    if (ret < 0)
	{
		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - v4l2_device_register() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}
	#endif
	
	for( i = 0 ; i < pDevice->m_nKsDeviceVideoNumber ; i++ ) { 

		pDevice->m_pKsVideo[ i ] = video_device_alloc();

		if( pDevice->m_pKsVideo[ i ] == NULL ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - video_device_alloc() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			ret = -ENOMEM;

			goto common_pci_driver_on_probe_fail;
		}
		else {

			ULONG current_norm = pDevice->m_nCustomVideoStandardProperty;

			sprintf( pDevice->m_pKsVideo[ i ]->name, "%s:RAW %02d.%02d %08x", psz, (int)(g_n_counter_of_board_number), (int)(i), ((pDevice->bcdDevice << 16 ) & 0xFFFF0000 ) | ((pDevice->iProduct  <<  8) & 0xFF00) | ((pDevice->iManufacturer << 0) & 0x00FF) );

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)

			pDevice->m_pKsVideo[ i ]->minor = -1;
			pDevice->m_pKsVideo[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
			//pDevice->m_pKsVideo[ i ]->current_norm = current_norm;
			pDevice->m_pKsVideo[ i ]->release = video_device_release;
			pDevice->m_pKsVideo[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;
			pDevice->m_pKsVideo[ i ]->ioctl_ops = &LINUXV4L2_V4L2_IOCTL_OPS;

			//do not init ->dev, maybe already done
			//pDevice->m_pKsVideo[ i ]->dev = &(p_pci_dev->dev);

			pDevice->m_pKsVideo[ i ]->v4l2_dev = &pDevice->v4l2_dev;

			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)

			pDevice->m_pKsVideo[ i ]->minor = -1;
			pDevice->m_pKsVideo[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
			pDevice->m_pKsVideo[ i ]->current_norm = current_norm;
			pDevice->m_pKsVideo[ i ]->release = video_device_release;
			pDevice->m_pKsVideo[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;
			pDevice->m_pKsVideo[ i ]->ioctl_ops = &LINUXV4L2_V4L2_IOCTL_OPS;

			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

			pDevice->m_pKsVideo[ i ]->type = VID_TYPE_CAPTURE; //
			pDevice->m_pKsVideo[ i ]->dev = &(p_pci_dev->dev);
		//	pDevice->m_pKsVideo[ i ]->hardware = 0;
			pDevice->m_pKsVideo[ i ]->minor = -1;
			pDevice->m_pKsVideo[ i ]->current_norm = current_norm;
			pDevice->m_pKsVideo[ i ]->release = video_device_release;
			pDevice->m_pKsVideo[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS; 
			pDevice->m_pKsVideo[ i ]->vidioc_querycap = common_video_device_vidioc_querycap;
			pDevice->m_pKsVideo[ i ]->vidioc_querystd = common_video_device_vidioc_querystd;
			pDevice->m_pKsVideo[ i ]->vidioc_s_std = common_video_device_vidioc_s_std;
			pDevice->m_pKsVideo[ i ]->vidioc_enum_input = common_video_device_vidioc_enum_input;
			pDevice->m_pKsVideo[ i ]->vidioc_s_input = common_video_device_vidioc_s_input;
			pDevice->m_pKsVideo[ i ]->vidioc_g_input = common_video_device_vidioc_g_input;
			pDevice->m_pKsVideo[ i ]->vidioc_enum_fmt_cap = common_video_device_vidioc_enum_fmt_cap;
			pDevice->m_pKsVideo[ i ]->vidioc_try_fmt_cap = common_video_device_vidioc_try_fmt_cap;
			pDevice->m_pKsVideo[ i ]->vidioc_s_fmt_cap = common_video_device_vidioc_s_fmt_cap;
			pDevice->m_pKsVideo[ i ]->vidioc_g_fmt_cap = common_video_device_vidioc_g_fmt_cap;
			pDevice->m_pKsVideo[ i ]->vidioc_queryctrl = common_video_device_vidioc_queryctrl;
			pDevice->m_pKsVideo[ i ]->vidioc_s_ctrl = common_video_device_vidioc_s_ctrl;
			pDevice->m_pKsVideo[ i ]->vidioc_g_ctrl = common_video_device_vidioc_g_ctrl;
			pDevice->m_pKsVideo[ i ]->vidioc_s_ext_ctrls = common_video_device_vidioc_s_ext_ctrls;
			pDevice->m_pKsVideo[ i ]->vidioc_g_ext_ctrls = common_video_device_vidioc_g_ext_ctrls;
			pDevice->m_pKsVideo[ i ]->vidioc_reqbufs = common_video_device_vidioc_reqbufs;
			pDevice->m_pKsVideo[ i ]->vidioc_querybuf = common_video_device_vidioc_querybuf;
			pDevice->m_pKsVideo[ i ]->vidioc_qbuf = common_video_device_vidioc_qbuf;
			pDevice->m_pKsVideo[ i ]->vidioc_dqbuf = common_video_device_vidioc_dqbuf;
			pDevice->m_pKsVideo[ i ]->vidioc_streamon = common_video_device_vidioc_streamon;
			pDevice->m_pKsVideo[ i ]->vidioc_streamoff = common_video_device_vidioc_streamoff;

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)

				pDevice->m_pKsVideo[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
			
				#else

				pDevice->m_pKsVideo[ i ]->tvnormsize = ARRAY_SIZE(LINUXV4L2_V4L2_TVNORM);

				pDevice->m_pKsVideo[ i ]->tvnorms = LINUXV4L2_V4L2_TVNORM;

				#endif
			
			#else 

			pDevice->m_pKsVideo[ i ]->type = VID_TYPE_CAPTURE; //
			pDevice->m_pKsVideo[ i ]->dev = &(p_pci_dev->dev);
		//	pDevice->m_pKsVideo[ i ]->hardware = 0;
			pDevice->m_pKsVideo[ i ]->minor = -1;
			pDevice->m_pKsVideo[ i ]->release = video_device_release;
			pDevice->m_pKsVideo[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;

			#endif

			#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,24)

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

			pDevice->m_pKsVideo[ i ]->vidioc_s_mpegcomp = common_video_device_vidioc_s_mpegcomp;
			pDevice->m_pKsVideo[ i ]->vidioc_g_mpegcomp = common_video_device_vidioc_g_mpegcomp;

			#endif

			#endif

			#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,27)

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)

			pDevice->m_pKsVideo[ i ]->vidioc_encoder_cmd = common_video_device_vidioc_encoder_cmd;
			
			#endif

			#endif
		}
		ret = video_register_device( pDevice->m_pKsVideo[ i ], VFL_TYPE_GRABBER, (int)(g_n_counter_of_video_number) + (int)(g_n_counter_of_video_encoder_number) + (int)(i) );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - video_register_device() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}

		video_set_drvdata( pDevice->m_pKsVideo[ i ], pDevice );
	}

	g_n_counter_of_video_number += pDevice->m_nKsDeviceVideoNumber;

	//
	//
	for( i = 0 ; i < pDevice->m_nKsDeviceVideoEncoderNumber ; i++ ) {  

		pDevice->m_pKsVideoEncoder[ i ] = video_device_alloc();

		if( pDevice->m_pKsVideoEncoder[ i ] == NULL ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - video_device_alloc() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			ret = -ENOMEM;

			goto common_pci_driver_on_probe_fail;
		}
		else {

			ULONG current_norm = pDevice->m_nCustomVideoStandardProperty;


			if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 )
			{

				if( (pDevice->iProduct & 0x0F) == (0x03) ||	(pDevice->iManufacturer == 0x85) )
				{
					if(i <= 3)// main
					{
						sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s:H.264 M #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
					}
					else
					{
						sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s:H.264 S #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
					}
				}
				else if( (pDevice->iProduct & 0x0F) == (0x05) )
				{
					if(i == 0)// main
					{
						sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s:H.264 M #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
					}
					else
					{
						sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s:H.264 S #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
					}
				}
				else
				{
					if(i <= 3)// main
					{
						sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s:H.264 M #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
					}
					else
					{
						sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s:H.264 S #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
					}
				}
			}
			else
			{
				sprintf( pDevice->m_pKsVideoEncoder[ i ]->name, "%s, Video Encoder #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)

			pDevice->m_pKsVideoEncoder[ i ]->minor = -1;
			pDevice->m_pKsVideoEncoder[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
			//pDevice->m_pKsVideoEncoder[ i ]->current_norm = current_norm;
			pDevice->m_pKsVideoEncoder[ i ]->release = video_device_release;
			pDevice->m_pKsVideoEncoder[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;
			pDevice->m_pKsVideoEncoder[ i ]->ioctl_ops = &LINUXV4L2_V4L2_IOCTL_OPS;

			//pDevice->m_pKsVideoEncoder[ i ]->dev = &(p_pci_dev->dev);
			
			pDevice->m_pKsVideoEncoder[ i ]->v4l2_dev = &pDevice->v4l2_dev;


			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)

			pDevice->m_pKsVideoEncoder[ i ]->minor = -1;
			pDevice->m_pKsVideoEncoder[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
			pDevice->m_pKsVideoEncoder[ i ]->current_norm = current_norm;
			pDevice->m_pKsVideoEncoder[ i ]->release = video_device_release;
			pDevice->m_pKsVideoEncoder[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;
			pDevice->m_pKsVideoEncoder[ i ]->ioctl_ops = &LINUXV4L2_V4L2_IOCTL_OPS;

			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

			pDevice->m_pKsVideoEncoder[ i ]->type = VID_TYPE_CAPTURE; //
			pDevice->m_pKsVideoEncoder[ i ]->dev = &(p_pci_dev->dev);
		//	pDevice->m_pKsVideoEncoder[ i ]->hardware = 0;
			pDevice->m_pKsVideoEncoder[ i ]->minor = -1;
			pDevice->m_pKsVideoEncoder[ i ]->current_norm = current_norm;
			pDevice->m_pKsVideoEncoder[ i ]->release = video_device_release;
			pDevice->m_pKsVideoEncoder[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_querycap = common_video_device_vidioc_querycap;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_querystd = common_video_device_vidioc_querystd;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_s_std = common_video_device_vidioc_s_std;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_enum_input = common_video_device_vidioc_enum_input;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_s_input = common_video_device_vidioc_s_input;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_g_input = common_video_device_vidioc_g_input;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_enum_fmt_cap = common_video_device_vidioc_enum_fmt_cap;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_try_fmt_cap = common_video_device_vidioc_try_fmt_cap;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_s_fmt_cap = common_video_device_vidioc_s_fmt_cap;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_g_fmt_cap = common_video_device_vidioc_g_fmt_cap;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_queryctrl = common_video_device_vidioc_queryctrl;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_s_ctrl = common_video_device_vidioc_s_ctrl;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_g_ctrl = common_video_device_vidioc_g_ctrl;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_s_ext_ctrls = common_video_device_vidioc_s_ext_ctrls;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_g_ext_ctrls = common_video_device_vidioc_g_ext_ctrls;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_reqbufs = common_video_device_vidioc_reqbufs;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_querybuf = common_video_device_vidioc_querybuf;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_qbuf = common_video_device_vidioc_qbuf;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_dqbuf = common_video_device_vidioc_dqbuf;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_streamon = common_video_device_vidioc_streamon;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_streamoff = common_video_device_vidioc_streamoff;
			
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)

				pDevice->m_pKsVideoEncoder[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
			
				#else

				pDevice->m_pKsVideoEncoder[ i ]->tvnormsize = ARRAY_SIZE(LINUXV4L2_V4L2_TVNORM);

				pDevice->m_pKsVideoEncoder[ i ]->tvnorms = LINUXV4L2_V4L2_TVNORM;

				#endif
			
			#else 

			pDevice->m_pKsVideoEncoder[ i ]->type = VID_TYPE_CAPTURE; //
			pDevice->m_pKsVideoEncoder[ i ]->dev = &(p_pci_dev->dev);
		//	pDevice->m_pKsVideoEncoder[ i ]->hardware = 0;
			pDevice->m_pKsVideoEncoder[ i ]->minor = -1;
			pDevice->m_pKsVideoEncoder[ i ]->release = video_device_release;
			pDevice->m_pKsVideoEncoder[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;

			#endif

			#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,24)

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

			pDevice->m_pKsVideoEncoder[ i ]->vidioc_s_mpegcomp = common_video_device_vidioc_s_mpegcomp;
			pDevice->m_pKsVideoEncoder[ i ]->vidioc_g_mpegcomp = common_video_device_vidioc_g_mpegcomp;

			#endif

			#endif

			#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,27)

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)

			pDevice->m_pKsVideoEncoder[ i ]->vidioc_encoder_cmd = common_video_device_vidioc_encoder_cmd;
			
			#endif

			#endif
		}

		ret = video_register_device( pDevice->m_pKsVideoEncoder[ i ], VFL_TYPE_GRABBER, (int)(g_n_counter_of_video_number) + (int)(g_n_counter_of_video_encoder_number) + (int)(i) );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - video_register_device() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}

		video_set_drvdata( pDevice->m_pKsVideoEncoder[ i ], pDevice );
	}

	g_n_counter_of_video_encoder_number += pDevice->m_nKsDeviceVideoEncoderNumber;

	//
	//
	for( i = 0 ; i < pDevice->m_nKsDeviceAudioNumber ; i++ ) { // YOU CAN USE COMMAND "arecord -D hw:1,0 -r 48000 -c 1 -f S16_LE | aplay" TO TEST ALSA

		CHAR psz_audio_name[ 16 ] = "PCM #00";

		#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)

		ret = snd_card_new( &p_pci_dev->dev, -1, NULL, THIS_MODULE, sizeof(CAudio), &pDevice->m_pKsAudioCard[ i ] );

		if( ret != 0 ) {
			
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}

		#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)

		ret = snd_card_create( -1, NULL, THIS_MODULE, sizeof(CAudio), &pDevice->m_pKsAudioCard[ i ] );

		if( ret != 0 ) {
			
			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_create() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}
		#else

		pDevice->m_pKsAudioCard[ i ] = snd_card_new( -1, NULL, THIS_MODULE, sizeof(CAudio) );
		
		#endif

		if( pDevice->m_pKsAudioCard[ i ] == NULL ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_create() / snd_card_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			ret = -ENOMEM;

		//	goto common_pci_driver_on_probe_fail; // SUPPORT "PCM #08" CREATE ERROR!!

			continue ;
		}
		else {

			sprintf( pDevice->m_pKsAudioCard[ i ]->driver, "LINUXV4L2" );

			sprintf( pDevice->m_pKsAudioCard[ i ]->shortname, "%s, Audio Capture #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );

			sprintf( pDevice->m_pKsAudioCard[ i ]->longname, "%s, Audio Capture #%02d.%02d (Long Name)", psz, (int)(g_n_counter_of_board_number), (int)(i) );

			sprintf( pDevice->m_pKsAudioCard[ i ]->mixername, "%s, Audio Capture #%02d.%02d (Mixer Name)", psz, (int)(g_n_counter_of_board_number), (int)(i) );
		}

		pDevice->m_pAudio[ i ] = (CAudio *)(pDevice->m_pKsAudioCard[ i ]->private_data);

		pDevice->m_pAudio[ i ]->m_pDevice = pDevice;
		pDevice->m_pAudio[ i ]->m_nSubDeviceNumber = i;
		pDevice->m_pAudio[ i ]->m_nSubDeviceType = 0x00000001;
		pDevice->m_pAudio[ i ]->m_nInput = 0;
		pDevice->m_pAudio[ i ]->m_nVolume = 128;
		pDevice->m_pAudio[ i ]->m_nRate = 48000;
		pDevice->m_pAudio[ i ]->m_nChannels = 2;
		pDevice->m_pAudio[ i ]->m_nSampleBits = 16;
		pDevice->m_pAudio[ i ]->m_nFrameSize = 1920;
		pDevice->m_pAudio[ i ]->m_nFrameNumber = 0;
		pDevice->m_pAudio[ i ]->m_nFrames = 64;
		pDevice->m_pAudio[ i ]->m_pKsSubStream = NULL;

		snd_card_set_dev( pDevice->m_pKsAudioCard[ i ], &p_pci_dev->dev );

		//////////////////////////////////////////////////////////////////

		ret = snd_ctl_add( pDevice->m_pKsAudioCard[ i ], snd_ctl_new1( &LINUXV4L2_SND_KCONTROL_NEW_VOLUME, pDevice->m_pAudio[ i ] ) );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_ctl_add() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}

		sprintf( psz_audio_name, "PCM #%02d", (int)(g_n_counter_of_audio_number + i) );

		ret = snd_pcm_new( pDevice->m_pKsAudioCard[ i ], psz_audio_name, 0x00000000, 0, 1, &(pDevice->m_pKsAudioPCM[ i ]) );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_pcm_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}
		else {

		//	pDevice->m_pKsAudioPCM[ i ]->private_free = NULL;

		//	pDevice->m_pKsAudioPCM[ i ]->private_data = pDevice;

			sprintf( pDevice->m_pKsAudioPCM[ i ]->name, "%s", psz_audio_name );

			pDevice->m_pKsAudioPCM[ i ]->info_flags = 0x00000000;

			if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
			{
				ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_TW5864.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_TW5864.buffer_bytes_max );
			}
			else if( pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C )
			{
				ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_AH8400.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_AH8400.buffer_bytes_max );
			}
			else if( pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160 )
			{
				ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_SA7160.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_SA7160.buffer_bytes_max );
			}
			else if( pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101 )
			{
				if( (pDevice->iProduct & 0x0F) == (0x05) || (pDevice->iProduct & 0x0F) == (0x07) )
				{
					ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_FH8735.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_FH8735.buffer_bytes_max );
				}
				else
				{
					ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_FH8735_SC3A0.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_FH8735_SC3A0.buffer_bytes_max );
				}
			}
			else if( pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210 )
			{
				ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_CX2581.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_CX2581.buffer_bytes_max );
			}
			else if( pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100 )
			{
				if( ((pDevice->iProduct & 0x0F) == 0x02) || ((pDevice->iProduct & 0x0F) == 0x03)  ) // 960H & D1
				{
					ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC3C0.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC3C0.buffer_bytes_max );
				}
				else
				{
					ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC5C0.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC5C0.buffer_bytes_max );
				}
			}
			else
			{
				ret = snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_DEV_SG, snd_dma_pci_data( p_pci_dev ), LINUXV4L2_SND_PCM_HARDWARE.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE.buffer_bytes_max );
			}
			
			LINUXV4L2_DEBUG( KERN_INFO, "snd_pcm_lib_preallocate_pages_for_all()  ret(0x%x)\n", ret );

			if(ret != 0)
			{
				goto common_pci_driver_on_probe_fail;
			}
			
			snd_pcm_set_ops( pDevice->m_pKsAudioPCM[ i ], SNDRV_PCM_STREAM_CAPTURE, &LINUXV4L2_SND_PCM_OPS );
		}

		////////////////////////////////////////////////////////////////////////////////////////////////

		ret = snd_card_register( pDevice->m_pKsAudioCard[ i ] );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_register() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}
	}

	g_n_counter_of_audio_number += pDevice->m_nKsDeviceAudioNumber;

	//
	//
	for( i = 0 ; i < pDevice->m_nKsDeviceAudioEncoderNumber ; i++ ) { // YOU CAN USE COMMAND "arecord -D hw:1,0 -r 48000 -c 1 -f S16_LE | aplay" TO TEST ALSA

		;
	}

	g_n_counter_of_audio_encoder_number += pDevice->m_nKsDeviceAudioEncoderNumber;

	// // // // // // // // // // ... // // // // //


	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)

//	ret = request_irq( p_pci_dev->irq, common_pci_driver_on_interrupt_2_6_19, IRQF_SHARED | IRQF_DISABLED, "LINUXV4L2", (PVOID)(pDevice) );

	pDevice->m_IRQ_ready = request_irq( p_pci_dev->irq, common_pci_driver_on_interrupt_2_6_19, IRQF_SHARED, "LINUXV4L2", (PVOID)(pDevice) );

	#else

	pDevice->m_IRQ_ready = request_irq( p_pci_dev->irq, common_pci_driver_on_interrupt_2_6_18, SA_SHIRQ | SA_INTERRUPT, "LINUXV4L2", (PVOID)(pDevice) );

	#endif

	if( pDevice->m_IRQ_ready != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - request_irq() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}

	//hwinit use ISR and pDevice->m_pKsVideoEncoder[ i ]->dev
	if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) { if( FALSE == MZ0380_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; } }

	g_n_counter_of_board_number++;

	for( i = 0; i < 32 ; i++ )
	{
		
		ULONG j = 0;

		for( j = 0; j < 8; j++ )
		{
			pDevice->m_nCustomMotionStatusProperty[ i ][ j ] = 0;
		}

		pDevice->m_nCustomMotionTemporalSensitivityProperty[ i ] = 0;

		pDevice->m_nCustomMotionSpatialSensitivityProperty[ i ] = 0;

		pDevice->m_nCustomMotionLevelSensitivityProperty[ i ] = 0;

		pDevice->m_nCustomMotionSpeedProperty[ i ] = 0;
	}
	
	LINUXV4L2_DEBUG( KERN_INFO, " common_pci_driver_on_probe() end\n" );

	return 0;

common_pci_driver_on_probe_fail:

	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		if( pDevice->m_pKsAudioCard[ i ] ) { 

			snd_card_free( pDevice->m_pKsAudioCard[ i ] );

			pDevice->m_pKsAudioCard[ i ] = NULL;
		}
		if( pDevice->m_pKsAudioPCM[ i ] ) { 

			pDevice->m_pKsAudioPCM[ i ] = NULL;
		}
		if( pDevice->m_pAudio[ i ] ) { 

			pDevice->m_pAudio[ i ] = NULL;
		}

		if( pDevice->m_pKsVideo[ i ] ) {

			video_set_drvdata( pDevice->m_pKsVideo[ i ], NULL );

			if( -1 != pDevice->m_pKsVideo[ i ]->minor ) { video_unregister_device( pDevice->m_pKsVideo[ i ] ); }

			if( -1 == pDevice->m_pKsVideo[ i ]->minor ) { video_device_release( pDevice->m_pKsVideo[ i ] ); }

			pDevice->m_pKsVideo[ i ] = NULL;
		}
		if( pDevice->m_pVideo[ i ] ) { 

			pDevice->m_pVideo[ i ] = NULL;
		}
	}
	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		if( pDevice->m_pKsAudioEncoderCard[ i ] ) { 

			snd_card_free( pDevice->m_pKsAudioEncoderCard[ i ] );

			pDevice->m_pKsAudioEncoderCard[ i ] = NULL;
		}
		if( pDevice->m_pKsAudioEncoderPCM[ i ] ) { 

			pDevice->m_pKsAudioEncoderPCM[ i ] = NULL;
		}
		if( pDevice->m_pAudioEncoder[ i ] ) { 

			pDevice->m_pAudioEncoder[ i ] = NULL;
		}

		if( pDevice->m_pKsVideoEncoder[ i ] ) {

			video_set_drvdata( pDevice->m_pKsVideoEncoder[ i ], NULL );

			if( -1 != pDevice->m_pKsVideoEncoder[ i ]->minor ) { video_unregister_device( pDevice->m_pKsVideoEncoder[ i ] ); }

			if( -1 == pDevice->m_pKsVideoEncoder[ i ]->minor ) { video_device_release( pDevice->m_pKsVideoEncoder[ i ] ); }

			pDevice->m_pKsVideoEncoder[ i ] = NULL;
		}
		if( pDevice->m_pVideoEncoder[ i ] ) { 

			pDevice->m_pVideoEncoder[ i ] = NULL;
		}
	}
	
	if(pDevice->m_IRQ_ready == 0)
	{
		free_irq( p_pci_dev->irq, (PVOID)(pDevice) );
	}

	if( pDevice ) {

		tasklet_kill( &(pDevice->m_sTasklet) );

		for( i = 0 ; i < MAX_REG_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pRegBaseCommonBuffer[ i ] ) {

				iounmap( pDevice->m_pRegBaseCommonBuffer[ i ] );

				pDevice->m_nRegBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pRegBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pRegBaseCommonPhysicalBuffer[ i ] = 0;
			}
		}
		for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pDmaBaseCommonBuffer[ i ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ i ], pDevice->m_pDmaBaseCommonBuffer[ i ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferNumber[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] = 0x00000000;
			}
		}

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			pDevice->m_Preview_Opened_counter[ i ] = 0;
		}

	}

	{
		int index = 0;

		int cx2581_ch = 0;

		for( index = 0; index < SA7160_MAX_BUFFER; index++ )
		{
			if( pDevice->m_SA7160_video_buffer[ index ] ) 
			{
				pci_free_consistent( p_pci_dev, 1920 * 1080 * 2, pDevice->m_SA7160_video_buffer[ index ], pDevice->m_SA7160_video_buffer_physical_address[ index ] );

				pDevice->m_SA7160_video_buffer[ index ] = 0;

				pDevice->m_SA7160_video_buffer_physical_address[ index ] = 0;

			}
		}

#ifdef TW6805_64bit

		for( index = 0; index < TW6816_MAX_BUFFER; index++ )
		{
			if( pDevice->m_TW6816_video_buffer[ index ] ) 
			{
				pci_free_consistent( p_pci_dev, 720 * 576 * 2, pDevice->m_TW6816_video_buffer[ index ], pDevice->m_TW6816_video_buffer_physical_address[ index ] );

				pDevice->m_TW6816_video_buffer[ index ] = 0;

				pDevice->m_TW6816_video_buffer_physical_address[ index ] = 0;

			}
		}
#endif //TW6805_64bit

#ifdef CX2581_64bit

		for( cx2581_ch = 0; cx2581_ch < 8; cx2581_ch++ )
		{
			for( index = 0; index < CX2581_MAX_BUFFER; index++ )
			{
				if( pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] ) 
				{
					pci_free_consistent( p_pci_dev, 720 * 576 * 2, pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ], pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ] );

					pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] = 0;

					pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ] = 0;

				}
			}
		}
	}
#endif //CX2581_64bit


	pci_release_regions( p_pci_dev );
	
	pci_disable_device( p_pci_dev );

	pci_set_drvdata( p_pci_dev, NULL );

	FREE( pDevice );

	return ret;
}

static void __devexit common_pci_driver_on_remove( struct pci_dev * p_pci_dev )
{

//	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 && ( (p_pci_dev->subsystem_vendor & 0xFF) == 0x19) && p_pci_dev->subsystem_device == 0x12AB )
//	{ 
//		return;
//	}

	CDevice * pDevice = pci_get_drvdata( p_pci_dev );

	unsigned int i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_remove()\n", (int)(pDevice->m_nKsDeviceNumber) );

	//
	// 
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6802 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6805 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6814 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6815 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6816 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6817 ) { if( FALSE == TW6805_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210 ) { if( FALSE == CX2581_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x5851 ) { if( FALSE == CX2588_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 ) { if( FALSE == SA7160_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x9413 && p_pci_dev->device == 0x6010 ) { if( FALSE == SL6010_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1999 && p_pci_dev->device == 0xA04C ) { if( FALSE == AH8400_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x5864 ) { if( FALSE == TW5864_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x3086 && p_pci_dev->device == 0x0101 ) { if( FALSE == FH8735_HwUnInitialize( pDevice ) ) { ; } }
	if( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) { if( FALSE == MZ0380_HwUnInitialize( pDevice ) ) { ; } }

	// // // // // // // // // // ... // // // // //

	//
	// 
	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		//
		// 
		if( pDevice->m_pKsAudioCard[ i ] ) {

		//	snd_card_free_when_closed( pDevice->m_pKsAudioCard[ i ] );

			snd_card_free( pDevice->m_pKsAudioCard[ i ] );

			pDevice->m_pKsAudioCard[ i ] = NULL;
		}
		if( pDevice->m_pKsAudioPCM[ i ] ) { 

			pDevice->m_pKsAudioPCM[ i ] = NULL; //
		}
		if( pDevice->m_pAudio[ i ] ) { 

			pDevice->m_pAudio[ i ] = NULL;
		}
		
		//
		// 
		if( pDevice->m_pKsVideo[ i ] ) { 

			video_set_drvdata( pDevice->m_pKsVideo[ i ], NULL );

			if( -1 != pDevice->m_pKsVideo[ i ]->minor ) { video_unregister_device( pDevice->m_pKsVideo[ i ] ); }

			if( -1 == pDevice->m_pKsVideo[ i ]->minor ) { video_device_release( pDevice->m_pKsVideo[ i ] ); }

			pDevice->m_pKsVideo[ i ] = NULL;
		}
		if( pDevice->m_pVideo[ i ] ) { 

			pDevice->m_pVideo[ i ] = NULL;
		}
	}
	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		//
		// 
		if( pDevice->m_pKsAudioEncoderCard[ i ] ) {

		//	snd_card_free_when_closed( pDevice->m_pKsAudioEncoderCard[ i ] );

			snd_card_free( pDevice->m_pKsAudioEncoderCard[ i ] );

			pDevice->m_pKsAudioEncoderCard[ i ] = NULL;
		}
		if( pDevice->m_pKsAudioEncoderPCM[ i ] ) { 

			pDevice->m_pKsAudioEncoderPCM[ i ] = NULL; //
		}
		if( pDevice->m_pAudioEncoder[ i ] ) { 

			pDevice->m_pAudioEncoder[ i ] = NULL;
		}
		
		//
		// 
		if( pDevice->m_pKsVideoEncoder[ i ] ) { 

			video_set_drvdata( pDevice->m_pKsVideoEncoder[ i ], NULL );

			if( -1 != pDevice->m_pKsVideoEncoder[ i ]->minor ) { video_unregister_device( pDevice->m_pKsVideoEncoder[ i ] ); }

			if( -1 == pDevice->m_pKsVideoEncoder[ i ]->minor ) { video_device_release( pDevice->m_pKsVideoEncoder[ i ] ); }

			pDevice->m_pKsVideoEncoder[ i ] = NULL;
		}
		if( pDevice->m_pVideoEncoder[ i ] ) { 

			pDevice->m_pVideoEncoder[ i ] = NULL;
		}
	}

	//
	// 
	if(pDevice->m_IRQ_ready == 0)
	{
		free_irq( p_pci_dev->irq, (PVOID)(pDevice) );
	}

	if( pDevice ) {

		tasklet_kill( &(pDevice->m_sTasklet) );

		for( i = 0 ; i < MAX_REG_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pRegBaseCommonBuffer[ i ] ) {

				iounmap( pDevice->m_pRegBaseCommonBuffer[ i ] );

				pDevice->m_nRegBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pRegBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pRegBaseCommonPhysicalBuffer[ i ] = 0;
			}
		}
		for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pDmaBaseCommonBuffer[ i ] ) {

				pci_free_consistent( p_pci_dev, pDevice->m_nDmaBaseCommonBufferSize[ i ], pDevice->m_pDmaBaseCommonBuffer[ i ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ i ] = 0;
					
				pDevice->m_nDmaBaseCommonBufferNumber[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] = 0x00000000;
			}
		}

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			pDevice->m_Preview_Opened_counter[ i ] = 0;
		}

	}

	{
		int index = 0;

		int cx2581_ch = 0;

		for( index = 0; index < SA7160_MAX_BUFFER; index++ )
		{
			if( pDevice->m_SA7160_video_buffer[ index ] ) 
			{
				pci_free_consistent( p_pci_dev, 1920 * 1080 * 2, pDevice->m_SA7160_video_buffer[ index ], pDevice->m_SA7160_video_buffer_physical_address[ index ] );

				pDevice->m_SA7160_video_buffer[ index ] = 0;

				pDevice->m_SA7160_video_buffer_physical_address[ index ] = 0;

			}
		}

#ifdef TW6805_64bit

		for( index = 0; index < TW6816_MAX_BUFFER; index++ )
		{
			if( pDevice->m_TW6816_video_buffer[ index ] ) 
			{
				pci_free_consistent( p_pci_dev, 720 * 576 * 2, pDevice->m_TW6816_video_buffer[ index ], pDevice->m_TW6816_video_buffer_physical_address[ index ] );

				pDevice->m_TW6816_video_buffer[ index ] = 0;

				pDevice->m_TW6816_video_buffer_physical_address[ index ] = 0;

			}
		}
#endif //TW6805_64bit

#ifdef CX2581_64bit

		for( cx2581_ch = 0; cx2581_ch < 8; cx2581_ch++ )
		{
			for( index = 0; index < CX2581_MAX_BUFFER; index++ )
			{
				if( pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] ) 
				{
					pci_free_consistent( p_pci_dev, 720 * 576 * 2, pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ], pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ] );

					pDevice->m_CX2581_video_buffer[ cx2581_ch ][ index ] = 0;

					pDevice->m_CX2581_video_buffer_physical_address[ cx2581_ch ][ index ] = 0;

				}
			}
		}
	}
#endif //CX2581_64bit


	pci_release_regions( p_pci_dev );

	pci_disable_device( p_pci_dev );

	pci_set_drvdata( p_pci_dev, NULL );

	FREE( pDevice );
}

static int common_pci_driver_on_suspend( struct pci_dev * p_pci_dev, pm_message_t s_pm_message_state )
{
	CDevice * pDevice = pci_get_drvdata( p_pci_dev );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_suspend()\n", (int)(pDevice->m_nKsDeviceNumber) ); 

	return 0;
}

static int common_pci_driver_on_resume( struct pci_dev * p_pci_dev )
{
	CDevice * pDevice = pci_get_drvdata( p_pci_dev );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_resume()\n", (int)(pDevice->m_nKsDeviceNumber) );

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)

static irqreturn_t common_pci_driver_on_interrupt_2_6_18( int n_irq, void * p_context, struct pt_regs * p_regs ) { return IRQ_HANDLED; }

static irqreturn_t common_pci_driver_on_interrupt_2_6_19( int n_irq, void * p_context )

#else

static irqreturn_t common_pci_driver_on_interrupt_2_6_19( int n_irq, void * p_context ) { return IRQ_HANDLED; }

static irqreturn_t common_pci_driver_on_interrupt_2_6_18( int n_irq, void * p_context, struct pt_regs * p_regs )

#endif

{	

	CDevice * pDevice = (CDevice *)(p_context);

	ULONG interrupt_status = 0x00000000;

	ULONG interrupt_params = 0x00000000;

	ULONG i = 0;

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) || 
	    (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) || 
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) || 
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) || 
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) || 
	    (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) || 
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) || 
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) {

		//
		//
		interrupt_status = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000001C ));

		//if( ((interrupt_status & 0xFFFFFFFF) == 0xFFFFFFFF) ) { return IRQ_NONE; }

		//if( ((interrupt_status & 0x1C00006A) == 0x00000000) ) { return IRQ_NONE; }
		
		//for boser
		if( ((interrupt_status & 0xFFFFFFFF) == 0xFFFFFFFF) ) { return IRQ_HANDLED; }

		if( ((interrupt_status & 0x1C00006A) == 0x00000000) ) { return IRQ_HANDLED; }

		if( ((interrupt_status & 0x10000000) == 0x10000000) ||  //
			((interrupt_status & 0x08000000) == 0x08000000) ||  //
			((interrupt_status & 0x04000000) == 0x04000000) ||  //
			((interrupt_status & 0x00000040) == 0x00000040) ||  //
			((interrupt_status & 0x00000020) == 0x00000020) ||  //
			((interrupt_status & 0x00000008) == 0x00000008) ) { //

			// RESTART ENGINE
			// 
			ULONG R00000000 = readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000000 ) & 0xFFFFFFFC; 
		  
			writel( R00000000 | 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000000 );

			writel( R00000000 | 0x00000003, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000000 ); 

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] HARDWARE RESET (S:%08X)\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(interrupt_status) );
		}
		//
		//
		writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000020 );

		//
		//
		writel( 0xFFFFFFFF, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000001C );

		//
		// 
		pDevice->m_nTaskletExtraParameterA = interrupt_status;

		pDevice->m_nTaskletExtraParameterB = interrupt_params;

		tasklet_hi_schedule( &(pDevice->m_sTasklet) ); //
	}
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) {

		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_NONE; }

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00040028 = 0x00000000;

			ULONG R00040020 = 0x00000000;

			//
			//
			R00040028 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040028 + (0x00000010 * i) ));

			if( ((R00040028 & 0xFFFFFFFF) == 0xFFFFFFFF) ) { continue ; }

			if( ((R00040028 & 0x00033303) == 0x00000000) ) { continue ; }

		//	if( ((R00040028 & 0x00010000) == 0x00000000) &&  //
		//		((R00040028 & 0x00001000) == 0x00000000) &&  //
		//		((R00040028 & 0x00000100) == 0x00000000) &&  //
		//		((R00040028 & 0x00020000) == 0x00000000) &&  //
		//		((R00040028 & 0x00002000) == 0x00000000) &&  //
		//		((R00040028 & 0x00000200) == 0x00000000) ) { //
		//	}
			if( ((R00040028 & 0x00000001) == 0x00000001) ) { 
				
				interrupt_status |= (0x00000001 << (i + 0)); 
			}
			if( ((R00040028 & 0x00033303) != 0x00000000) ) { 
				
				interrupt_status |= (0x00010000 << (i + 0)); 
			}
			//
			//
			R00040020 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) ));

			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );

			//
			// 
			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040024 + (0x00000010 * i) );
		}
		for( i = 0 ; i < 4 ; i++ ) {

			ULONG R000400C8 = 0x00000000;

			//
			//
			R000400C8 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C8 + (0x00000010 * i) ));

			if( ((R000400C8 & 0xFFFFFFFF) == 0xFFFFFFFF) ) { continue ; }

			if( ((R000400C8 & 0x00033303) == 0x00000000) ) { continue ; }

		//	if( ((R000400C8 & 0x00010000) == 0x00000000) &&  //
		//		((R000400C8 & 0x00001000) == 0x00000000) &&  //
		//		((R000400C8 & 0x00000100) == 0x00000000) &&  //
		//		((R000400C8 & 0x00020000) == 0x00000000) &&  //
		//		((R000400C8 & 0x00002000) == 0x00000000) &&  //
		//		((R000400C8 & 0x00000200) == 0x00000000) ) { //
		//	}
			if( ((R000400C8 & 0x00000001) == 0x00000001) ) { 
				
				interrupt_status |= (0x00000001 << (i + 8)); 
			}
			if( ((R000400C8 & 0x00033303) != 0x00000000) ) { 
				
				interrupt_status |= (0x00010000 << (i + 8)); 
			}
			//
			//
			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );

			//
			// 
			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C4 + (0x00000010 * i) );
		}
		if( interrupt_status == 0x00000000 ) {
			
			return IRQ_NONE;
		} 
		for( i = 0 ; i < 8 ; i++ ) {

			//
			// 
			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );
		}
		for( i = 0 ; i < 4 ; i++ ) {

			//
			//
			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );
		}
		//
		//
		pDevice->m_nTaskletExtraParameterA = interrupt_status;

		pDevice->m_nTaskletExtraParameterB = interrupt_params;

		pDevice->m_hInterruptAccessLock = FALSE;

		tasklet_hi_schedule( &(pDevice->m_sTasklet) ); //
	}
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) {
	
		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_NONE; }

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00040028 = 0x00000000;

			ULONG R00040020 = 0x00000000;

			//
			//
			R00040028 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040028 + (0x00000010 * i) ));

			if( ((R00040028 & 0xFFFFFFFF) == 0xFFFFFFFF) ) { continue ; }

			if( ((R00040028 & 0x00011111) == 0x00000000) ) { continue ; }

		//	if( ((R00040028 & 0x00010000) == 0x00000000) &&  //
		//		((R00040028 & 0x00001000) == 0x00000000) &&  //
		//		((R00040028 & 0x00000100) == 0x00000000) ) { //
		//	}
			if( ((R00040028 & 0x00000001) == 0x00000001) ) { 
				
				interrupt_status |= (0x00000001 << (i + 0)); 
			}
			if( ((R00040028 & 0x00011111) != 0x00000000) ) { 
				
				interrupt_status |= (0x00010000 << (i + 0)); 
			}
			//
			//
			R00040020 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) ));

			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );

			//
			// 
			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040024 + (0x00000010 * i) );
		}
		for( i = 0 ; i < 4 ; i++ ) {

			ULONG R000400C8 = 0x00000000;

			//
			//
			R000400C8 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C8 + (0x00000010 * i) ));

			if( ((R000400C8 & 0xFFFFFFFF) == 0xFFFFFFFF) ) { continue ; }

			if( ((R000400C8 & 0x00011111) == 0x00000000) ) { continue ; }

		//	if( ((R000400C8 & 0x00010000) == 0x00000000) &&  //
		//		((R000400C8 & 0x00001000) == 0x00000000) &&  //
		//		((R000400C8 & 0x00000100) == 0x00000000) ) { //
		//	}
			if( ((R000400C8 & 0x00000001) == 0x00000001) ) { 
				
				interrupt_status |= (0x00000001 << (i + 8)); 
			}
			if( ((R000400C8 & 0x00011111) != 0x00000000) ) { 
				
				interrupt_status |= (0x00010000 << (i + 8)); 
			}
			//
			//
			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );

			//
			// 
			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C4 + (0x00000010 * i) );
		}
		if( interrupt_status == 0x00000000 ) {
			
			return IRQ_NONE;
		} 
		for( i = 0 ; i < 8 ; i++ ) {

			//
			// 
			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );
		}
		for( i = 0 ; i < 4 ; i++ ) {

			//
			//
			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );
		}
		//
		//
		pDevice->m_nTaskletExtraParameterA = interrupt_status;

		pDevice->m_nTaskletExtraParameterB = interrupt_params;

		pDevice->m_hInterruptAccessLock = FALSE;

		tasklet_hi_schedule( &(pDevice->m_sTasklet) ); //
	}
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) {

//		LINUXV4L2_PRINT( KERN_INFO, "m_hInterruptAccessLock(0x%x)\n", pDevice->m_hInterruptAccessLock );

		//if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_NONE; }
		
		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_HANDLED; }//for IRQ nobody cared

		//
		//
		ULONG R0000AFC0 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000A000 + 0x0FC0)); // MSI.INT.STATUS.L

		ULONG R0000AFC4 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000A000 + 0x0FC4)); // MSI.INT.STATUS.H

//		LINUXV4L2_PRINT( KERN_INFO, "R0000AFC0(0x%x) R0000AFC4(0x%x)\n", R0000AFC0, R0000AFC4 );

		if( R0000AFC0 == 0x00000000 && R0000AFC4 == 0x00000000 ) { return IRQ_NONE ; }

		if( R0000AFC0 == 0xFFFFFFFF && R0000AFC4 == 0xFFFFFFFF ) { return IRQ_NONE ; }

		if( R0000AFC0 & 0x00000001 )
		{
			//register 0xFE0 will change very fast, get it ASAP
			pDevice->m_nTaskletExtraParameterB = interrupt_params = SA7160_GetRegister( pDevice, 0x00000000 + 0x00000FE0 );

			//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] interrupt_params(0x%x)\n", pDevice->m_nKsDeviceNumber, interrupt_params );
		}

		//
		//
		if( R0000AFC0 ) { writel( R0000AFC0, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000A000 + 0x0FC8 ); } // MSI.INT.STATUS.CLR.L

		if( R0000AFC4 ) { writel( R0000AFC4, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000A000 + 0x0FCC ); } // MSI.INT.STATUS.CLR.H

		if( R0000AFC0 & 0x00000C01 )
		{
			//
			// 
			pDevice->m_hInterruptAccessLock = FALSE;

			pDevice->m_nTaskletExtraParameterA = interrupt_status = R0000AFC0;

			//pDevice->m_nTaskletExtraParameterB = interrupt_params = R0000AFC4;
	
			
			tasklet_hi_schedule( &(pDevice->m_sTasklet) ); //
		}
	}
	if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) {
	
		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_NONE; }

		//
		//
		interrupt_status = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000100 ));

		if( ((interrupt_status & 0xFFFFFFFF) == 0xFFFFFFFF) ) { return IRQ_NONE; }

		if( ((interrupt_status & 0x000000FF) == 0x00000000) ) { return IRQ_NONE; }

		//
		//
		writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000104 );

		//

		//
		// 
		interrupt_params = readl( pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x007D8000 + 0x00000000 );

		pDevice->m_nTaskletExtraParameterA = interrupt_status;

		pDevice->m_nTaskletExtraParameterB = interrupt_params;

		pDevice->m_hInterruptAccessLock = FALSE;

		tasklet_hi_schedule( &(pDevice->m_sTasklet) ); //
	}
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) {

//		LINUXV4L2_DEBUG( KERN_INFO, "TW5864() interrupt\n" );


#ifdef VTC
		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_HANDLED; }//VTC
#else
		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_HANDLED; }
#endif //VTC


		//
		//
		interrupt_status = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 ));

		if( ((interrupt_status & 0xFFFFFFFF) == 0xFFFFFFFF) ) { return IRQ_NONE; }

		if( ((interrupt_status & 0xFFFFFFFF) == 0x00000000) ) { return IRQ_NONE; }

		//
		//
		writel( 0x0000FF37, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018004 );

		if( interrupt_status & 0x00000020 ) { // ISIL_INTR_OVERFLOW

			writel( 0x00000020, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 );

			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000C02C );// 解決 SC3B0N16 平台當機問題

			//
			//
			writel( 0x0000FF77, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018004 );

			LINUXV4L2_DEBUG( KERN_INFO, "ISIL.INTR.OVERFLOW#%02d\n", (int)(pDevice->m_nKsDeviceNumber) );

			return IRQ_HANDLED;
		}

		if( interrupt_status & 0x00000002 ) {

			ULONG R00001004 = 0x00000000;
			
			//
			// 
			writel( 0x00000002, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 );

			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00008814 );

			R00001004 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00001004 ));

			if( R00001004 > (384 * 1024 * 8) ) { R00001004 = (384 * 1024 * 8); }
	
			if( R00001004 > (0x00097E00 * 8) ) { R00001004 = (0x00097E00 * 8); }

			pDevice->m_nTaskletExtraParameterA = 0x00000002;

			pDevice->m_nTaskletExtraParameterB = R00001004;

			goto TW5864_DPC_JUMP;
		}
		if( interrupt_status & 0x00000040 ) {

			//
			// 
			writel( 0x00000040, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 );

			pDevice->m_nTaskletExtraParameterA = 0x00000040;

			pDevice->m_nTaskletExtraParameterB = 0x00000000;

			goto TW5864_DPC_JUMP;
		}
		if( interrupt_status & 0x00000010 ) {

			ULONG R00018008 = 0x00000000;

			ULONG R00018010 = 0x00000000;

			ULONG i = 0;

			//
			// 
			writel( 0x00000010, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 );

			R00018008 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018008 )) & 0x0000FFFF;

			R00018010 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018010 )) & 0x0000FFFF;

			pDevice->m_nTaskletExtraParameterA = 0x00000010;

			pDevice->m_nTaskletExtraParameterB = (R00018010 << 16) | (R00018008 << 0);

			for( i = 0 ; i < 16 ; i++ ) {

				if( (R00018008 >> i) & 0x01 ) {

					writel( (1) << (i + 0), pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018008 );

					writel( (1) << (i + 5), pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00008814 );
				}
			}
			goto TW5864_DPC_JUMP;
		}	
		if( interrupt_status & 0x00000100 ) {

			ULONG R00018008 = 0x00000000;

			ULONG R00018010 = 0x00000000;

			ULONG i = 0;

			//
			// 

			writel( 0x000000100, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 );

			R00018008 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018008 )) >> 16;

			R00018010 = (ULONG)(readl( pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018010 )) >> 16;

			pDevice->m_nTaskletExtraParameterA = 0x00000100;

			pDevice->m_nTaskletExtraParameterB = (R00018010 << 16) | (R00018008 << 0);

			for( i = 0 ; i < 16 ; i++ ) {

				if( (R00018008 >> i) & 0x01 ) {

					writel( (1) << (i + 16), pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018008 );

					writel( (1) << (i + 9), pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00008814 );
				}
			}
			goto TW5864_DPC_JUMP;
		}	

		//
		// 
		writel( interrupt_status, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018000 );

		if( interrupt_status & 0x00000020 ) { // ISIL_INTR_OVERFLOW

			writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0000C02C );// 解決 SC3B0N16 平台當機問題
		}

		//
		//
		writel( 0x0000FF77, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018004 );

		return IRQ_HANDLED;


TW5864_DPC_JUMP:

		pDevice->m_hInterruptAccessLock = FALSE;

		tasklet_hi_schedule( &(pDevice->m_sTasklet) ); //
	}

//peter [2011.06.29]
	if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) {

//		LINUXV4L2_DEBUG( KERN_INFO, "FH8735() interrupt------------ m_hInterruptAccessLock(0x%x)\n", pDevice->m_hInterruptAccessLock );
//FH8735 does not need to disable/enable interrupt
		if( pDevice->m_hInterruptAccessLock == FALSE ) { return IRQ_HANDLED; }

		//
		// 
		ULONG checks = (ULONG) (readl(pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x0040001C));
		
//		LINUXV4L2_DEBUG( KERN_INFO, "[%d]FH8735() interrupt -------- checks = %08X\n", (int)(pDevice->m_nKsDeviceNumber), checks );

		if( ((checks & 0xFFFFFFFF) == 0x00000000) ) { return IRQ_HANDLED; }

		ULONG status = (ULONG) readl( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x00400008) ) & 0x0000FFFF;

//		LINUXV4L2_DEBUG( KERN_INFO, "[%d]FH8735() interrupt -------- status = %08X\n", (int)(pDevice->m_nKsDeviceNumber), status );

		if( ((status & 0xFFFFFFFF) == 0xFFFFFFFF) ) { return IRQ_HANDLED; }

		if( ((status & 0xFFFFFFFF) == 0x00000000) ) { return IRQ_HANDLED; }

		if( status == 0x00000013 ) { // HCMD_REQ_DATA

			pDevice->m_nTaskletExtraParameterB = (ULONG) readl( (pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x00400078) );

			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x00400078 );
		}

		pDevice->m_nTaskletExtraParameterA = status; 
		
		pDevice->m_hInterruptAccessLock = FALSE;

		tasklet_hi_schedule( &(pDevice->m_sTasklet) );
	}

	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) {

		if( pDevice->m_hInterruptAccessLock == 0x00000000 ) { return IRQ_HANDLED; }

		//
		// 
		ULONG status = readl( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000030) );

		if( ((status & 0xFFFFFFFF) == 0xFFFFFFFF) ) { return FALSE; }

		if( ((status & 0xFFFFFFFF) == 0x00000000) ) { return FALSE; }

		ULONG arguments = 0x00000000;

		//
		//
		writel( 0x00000002, (pDevice->m_pRegBaseCommonBuffer[ 5 ] + 0x000000DC) ); // 手動清掉 INTX 中斷, 否則會一直發中斷不會停 (為什麼非 MSI 模式還不知道)

		writel( 0x00000000, (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000030) ); // 這段程式必須清除中斷

		writel( 0x00000400, (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000000) ); // 通知硬體可以發下個 INT 了

		if( status & 0x00000800 ) {

			LINUXV4L2_PRINT( KERN_INFO, "OnInterruptHandle( %04x %04x %04x * 0x00000030 = 0x%08X )\n", pDevice->idVendor, pDevice->idProduct, pDevice->bcdDevice, status);
		}
		pDevice->m_hInterruptAccessLock = 0x00000000;

		pDevice->m_nTaskletExtraParameterA = status; 

		tasklet_hi_schedule( &(pDevice->m_sTasklet) );

		pDevice->m_hInterruptAccessLock = 0x00000001;
	}

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_interrupt( S:%08X, M:%08X )\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(interrupt_status), (unsigned int)(interrupt_params) );

	return IRQ_HANDLED; 
}

// ##############################################################################################################################################################################
//
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)

static int common_file_operations_open( struct file * p_file )

#else

static int common_file_operations_open( struct inode * p_inode, struct file * p_file )

#endif
{
	struct video_device * pKsVideo = (struct video_device *)(video_devdata( p_file ));

	CDevice * pDevice = (CDevice *)(video_get_drvdata( pKsVideo ));

	CVideo * pVideo = NULL;

	int ret = 0;

	int is_success = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_open()\n", (int)(pDevice->m_nKsDeviceNumber) );

	pVideo = (CVideo *)(kmalloc( sizeof(struct CVideo), GFP_KERNEL ));

	if( pVideo == NULL ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_file_operations_open() - kmalloc() - [FAIL]\n", (int)(pDevice->m_nKsDeviceNumber) );

		ret = -ENOMEM;

		goto common_file_operations_open_fail;
	}
	else {

		memset( pVideo, 0, sizeof(struct CVideo) );

		BYTE SWITCH_CHANNEL_TABLE[ 13 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

	//	BYTE SWITCH_CHANNEL_TABLE[ 13 ] = { 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x0F };
	
	//	BYTE SWITCH_CHANNEL_TABLE[ 13 ] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x03 };

		unsigned int n_sub_device_number = 0;

		unsigned int n_sub_device_type = 0x00000000;

		unsigned int i = 0;

		unsigned int j = 0;

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) { //
			
			if( pKsVideo == pDevice->m_pKsVideo[ i ] ) { break; }
		} 
		for( j = 0 ; j < MAX_SUB_DEVICE_NUM ; j++ ) { //
			
			if( pKsVideo == pDevice->m_pKsVideoEncoder[ j ] ) { break; }
		}
		if( i < 16 ) { n_sub_device_number = i; n_sub_device_type = 0x00000001; } //

		if( j < 16 ) { n_sub_device_number = j; n_sub_device_type = 0x00000002; } //

		pVideo->m_pDevice                         = video_get_drvdata( video_devdata( p_file ) );
		pVideo->m_nSubDeviceNumber                = n_sub_device_number;
		pVideo->m_nSubDeviceType                  = n_sub_device_type;

		if( pVideo->m_nSubDeviceType == 0x00000001 )
		{
			if( pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] == NULL )
			{ 
				NULL;
			}
			else
			{
				//wait first open command complete,therwise some parameters will effect first open command
				schedule_timeout_interruptible( msecs_to_jiffies( 4000 ) ); // 4.0 FPS

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_open() for control\n", (int)(pDevice->m_nKsDeviceNumber) );

				pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ]++;
	
				//copy first pVideo data
				if( pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] )
				{
					memcpy( pVideo, pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ], sizeof(CVideo) );
				}
	
				p_file->private_data = (PVOID)(pVideo);

				pVideo->m_video_preview_streaming = 0;

			
				return 0;
			}
		}

		pVideo->m_nStandard                       = pDevice->m_nCustomVideoStandardProperty;
		pVideo->m_nInput                          = pDevice->m_nAnalogCrossbarVideoInputProperty;
		pVideo->m_nLockStatus                     = 0;
		pVideo->m_nLockAccumulatorCounter_Spliter = 0;
		pVideo->m_nDropDecumulatorCounter_Spliter = 0;
		pVideo->m_nSwitchNumber_Spliter           = 0;
		pVideo->m_nSrcFrameWidth                  = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (            352) : (            352);
		pVideo->m_nSrcFrameHeight                 = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (            240) : (            288);
		pVideo->m_nSrcFrameBitCount               = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (             16) : (             16);
		pVideo->m_nSrcFrameSize                   = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (  352 * 240 * 2) : (  352 * 288 * 2);
		pVideo->m_nDstFrameWidth                  = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (            352) : (            352);
		pVideo->m_nDstFrameHeight                 = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (            240) : (            288);
		pVideo->m_nDstFramePitch                  = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (           704) : (           704);
		pVideo->m_nDstFrameBitCount               = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (             16) : (             16);
		pVideo->m_nDstFrameSize                   = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (  352 * 240 * 2) : (  352 * 288 * 2);
		pVideo->m_nFramePixelFormat               = V4L2_PIX_FMT_YUYV;
		pVideo->m_nFramePixelField                = V4L2_FIELD_INTERLACED;
		pVideo->m_nFrameRate                      = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (          30000) : (          25000);
		pVideo->m_nFrameNumber                    = 0;
		pVideo->m_nFrameCopyPolarity              = 0;
		pVideo->m_nEncoderMode                    = V4L2_BITRATE_VBR;
		pVideo->m_nEncoderBitRate                 = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (4 * 1024 * 1024) : (4 * 1024 * 1024);
		pVideo->m_nEncoderQuality                 = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (           6000) : (           6000);
		pVideo->m_nEncoderGop                     = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (             30) : (             25);
		pVideo->m_nEncoderQPStep                  = (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ? (              1) : (              1);
		pVideo->m_pTaskSoftIntEmulator            = NULL;
		pVideo->m_video_preview_frame_number	  = 0;

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			if(pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty != 0)
			{
				pVideo->m_nDstFrameWidth                  = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
				pVideo->m_nDstFrameHeight                 = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

				if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
				{
					pVideo->m_nDstFrameHeight *= 2;
				}

				pVideo->m_nDstFramePitch                  = pVideo->m_nDstFrameWidth * 2;
				pVideo->m_nDstFrameBitCount               = 16;
				pVideo->m_nDstFrameSize                   = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 2;
			}
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ||
			
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) )
		{
			if(pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty != 0)
			{
				pVideo->m_nDstFrameWidth                  = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
				pVideo->m_nDstFrameHeight                 = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

				if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
				{
					pVideo->m_nDstFrameHeight *= 2;
				}
				pVideo->m_nDstFramePitch                  = pVideo->m_nDstFrameWidth;
				pVideo->m_nDstFrameBitCount               = 12;
				pVideo->m_nDstFrameSize                   = (pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 12) >> 3;
			}
		}
/*
		if( pVideo->m_nSubDeviceType == 0x00000001 )
		{
			if( pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] == NULL )
			{ 
				NULL;
			}
			else
			{
				pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ]++;
	
				p_file->private_data = (PVOID)(pVideo);

				pVideo->m_video_preview_streaming = 0;

				return 0;
			}
		}
*/

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ) { j =  0; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ) { j =  1; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ) { j =  2; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ) { j =  3; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ) { j =  4; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ) { j =  5; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ) { j =  6; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) { j =  7; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ) { j =  8; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ) { j =  9; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ) { j = 10; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) { j = 11; }
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) { j = 12; }
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) { j = 13; } // shawn 2010.07.26
		if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) { j = 14; } // shawn 2010.07.26
		if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) { j = 15; }
		if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) { j = 16; }
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) { j = 17; }
		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) { j = 18; }

		spin_lock_init( &(pVideo->m_sVideoBufQueueSpinLock) );

		spin_lock_init( &(pVideo->m_sKsBufferListSpinLock) );

		memcpy( pVideo->m_pSwitchTable_Spliter, SWITCH_CHANNEL_TABLE, 12 );

		pVideo->m_nLockStatusMask_Spliter = SWITCH_CHANNEL_TABLE[ 12 ];

		pVideo->m_nSwitchSpeed_Spliter = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 7 + 2 ];

		//
		//
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) ||
			//(pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {

			g_n_analog_decoder_brightness [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
			g_n_analog_decoder_brightness [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 1 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
			g_n_analog_decoder_brightness [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 2 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
			g_n_analog_decoder_brightness [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 3 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
			g_n_analog_decoder_contrast   [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
			g_n_analog_decoder_contrast   [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 1 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
			g_n_analog_decoder_contrast   [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 2 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
			g_n_analog_decoder_contrast   [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 3 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
			g_n_analog_decoder_saturation [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
			g_n_analog_decoder_saturation [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 1 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
			g_n_analog_decoder_saturation [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 2 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
			g_n_analog_decoder_saturation [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 3 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
			g_n_analog_decoder_hue        [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
			g_n_analog_decoder_hue        [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 1 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
			g_n_analog_decoder_hue        [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 2 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
			g_n_analog_decoder_hue        [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 3 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
			g_n_analog_decoder_sharpness  [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
			g_n_analog_decoder_sharpness  [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 1 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
			g_n_analog_decoder_sharpness  [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 2 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
			g_n_analog_decoder_sharpness  [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ][ 3 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
			g_n_analog_decoder_gain_auto  [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  5 + 2 ];
			g_n_analog_decoder_gain       [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  6 + 2 ];
			g_n_analog_decoder_lock_status[ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ]      = 0x103;
			g_n_gpio_direction            [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 18 + 2 ];
			g_n_gpio_data                 [ pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 19 + 2 ];
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) )
		{
			if(pVideo->m_nSubDeviceNumber < 4)//only for preview and main H264, not for sub H264
			{
				g_n_analog_decoder_brightness [ pDevice->m_nKsDeviceNumber ][pVideo->m_nSubDeviceNumber ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
				g_n_analog_decoder_contrast   [ pDevice->m_nKsDeviceNumber ][pVideo->m_nSubDeviceNumber ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
				g_n_analog_decoder_saturation [ pDevice->m_nKsDeviceNumber ][pVideo->m_nSubDeviceNumber ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
				g_n_analog_decoder_hue        [ pDevice->m_nKsDeviceNumber ][pVideo->m_nSubDeviceNumber ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
				g_n_analog_decoder_sharpness  [ pDevice->m_nKsDeviceNumber ][pVideo->m_nSubDeviceNumber ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
				g_n_analog_decoder_gain_auto  [ pDevice->m_nKsDeviceNumber ] 							  = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  5 + 2 ];
				g_n_analog_decoder_gain       [ pDevice->m_nKsDeviceNumber ] 							  = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  6 + 2 ];
				g_n_analog_decoder_lock_status[ pDevice->m_nKsDeviceNumber ] 							  = 0x103;
				g_n_gpio_direction            [ pDevice->m_nKsDeviceNumber ] 							  = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 18 + 2 ];
				g_n_gpio_data                 [ pDevice->m_nKsDeviceNumber ] 							  = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 19 + 2 ];
			}
		}


		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) {

			g_n_analog_decoder_brightness [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
			g_n_analog_decoder_contrast   [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
			g_n_analog_decoder_saturation [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
			g_n_analog_decoder_hue        [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
			g_n_analog_decoder_sharpness  [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
			g_n_analog_decoder_gain_auto  [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  5 + 2 ];
			g_n_analog_decoder_gain       [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  6 + 2 ];
			g_n_analog_decoder_lock_status[ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ]      = 0x103;
			g_n_gpio_direction            [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 18 + 2 ];
			g_n_gpio_data                 [ pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 19 + 2 ];
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x9413 && pVideo->m_pDevice->m_pKsDevice->device == 0x6010) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864)  ) {

			g_n_analog_decoder_brightness                    [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  0 + 2 ];
			g_n_analog_decoder_contrast                      [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  1 + 2 ];
			g_n_analog_decoder_saturation                    [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  2 + 2 ];
			g_n_analog_decoder_hue                           [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  3 + 2 ];
			g_n_analog_decoder_sharpness                     [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ][ 0 ] = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  4 + 2 ];
			g_n_analog_decoder_gain_auto                     [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  5 + 2 ];
			g_n_analog_decoder_gain                          [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  6 + 2 ];
			g_n_analog_decoder_lock_status                   [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = 0x103;
			g_n_multiplexer_motion_status                    [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][  9 + 2 ];
			g_n_multiplexer_motion_sensitivity_low_threshold [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 10 + 2 ];
			g_n_multiplexer_motion_sensitivity_high_threshold[ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 11 + 2 ];
			g_n_multiplexer_motion_mask_cx                   [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 12 + 2 ];
			g_n_multiplexer_motion_mask_cy                   [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 13 + 2 ];
			g_n_encoder_osd_pos_x                            [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 14 + 2 ];
			g_n_encoder_osd_pos_y                            [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 15 + 2 ];			
			g_n_encoder_osd_color                            [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 16 + 2 ];			
			g_n_gpio_direction								 [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 18 + 2 ];
			g_n_gpio_data									 [ pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1 ]      = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ 19 + 2 ];
		}

		//
		//

//for FC16 X64 , kernel 3.4.9  4G RAM NULL->pVideo->m_pDevice
//for FC16 X64 , kernel 3.4.6  NULL->pVideo->m_pDevice
//for Ubuntu 12.04, kernel 3.2.0 , 4G RAM NULL->pVideo->m_pDevice
//for FC16 X64 , kernel 3.1.0  , 4G RAM, NULL->pVideo->m_pDevice, 3G NULL, 2G NULL

//for Ubuntu 12.04, kernel 3.2.0-24 , 4G RAM NULL
//for FC16 X86 , kernel 3.4.6  NULL->pVideo->m_pDevice
//for FC16 X86 , kernel 3.1.0  NULL
//for Ubuntu 12.04 kernel3.2.0-31-generic 4G NULL
//for Ubuntu 12.04 kernel3.2.0-23-generic 4G NULL

		if(	(pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ){

			#ifdef CX2581_64bit
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

				#ifdef LINUXV4L2_VMALLOC_MODULE

				videobuf_queue_vmalloc_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER] 

				#endif

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				#ifdef LINUXV4L2_VMALLOC_MODULE

				videobuf_queue_vmalloc_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif

				#else

				videobuf_queue_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif
			#else
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)

				#ifdef CONFIG_64BIT
					videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]
				#else
					videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]
				#endif

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,7,0)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				videobuf_queue_pci_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#else

				videobuf_queue_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif
			#endif
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) )
		{
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,7,0)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, &(pDevice->m_pKsDevice->dev), &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, &(pDevice->m_pKsDevice->dev), &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, &(pDevice->m_pKsDevice->dev), &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				videobuf_queue_pci_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, &(pDevice->m_pKsDevice->dev), &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#else

				videobuf_queue_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif

		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {
			
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

			#ifdef LINUXV4L2_VMALLOC_MODULE

			videobuf_queue_vmalloc_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER] 

			#endif

			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			#ifdef LINUXV4L2_VMALLOC_MODULE

			videobuf_queue_vmalloc_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

			#endif

			#else

			videobuf_queue_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

			#endif
		}
		if(	(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ){
			
			#ifdef TW6805_64bit

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

				#ifdef LINUXV4L2_VMALLOC_MODULE

				videobuf_queue_vmalloc_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER] 

				#endif

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				#ifdef LINUXV4L2_VMALLOC_MODULE

				videobuf_queue_vmalloc_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif

				#else

				videobuf_queue_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif

			#else

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,7,0)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo, &(pVideo->m_pDevice->m_sSGSemaphore) ); // [PETER]

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)

				videobuf_queue_sg_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				videobuf_queue_pci_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, NULL, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#else

				videobuf_queue_init( &(pVideo->m_sVideoBufQueue), &LINUXV4L2_VIDEOBUF_QUEUE_OPS, pVideo->m_pDevice, &(pVideo->m_sVideoBufQueueSpinLock), V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED, sizeof(struct CVideoBuffer), pVideo ); //

				#endif

			#endif //TW6805_64bit

		}

		INIT_LIST_HEAD( &(pVideo->m_sKsBufferList) );

		INIT_LIST_HEAD( &(pVideo->m_sKsBufferListList) );
	}

	p_file->private_data = (PVOID)(pVideo);

	if( pVideo->m_nSubDeviceType == 0x00000001 ) {

		if( pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] == NULL ) { 
//skype
//		if( 1 ) { 
			
			pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] = pVideo; 

			pDevice->m_eVideoStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000;

			is_success = 1;

			pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ]++;

			pVideo->m_video_preview_streaming = 1;

		}
	}
	if( pVideo->m_nSubDeviceType == 0x00000002 ) {

		if( pDevice->m_pVideoEncoder[ pVideo->m_nSubDeviceNumber ] == NULL ) { 
			
			pDevice->m_pVideoEncoder[ pVideo->m_nSubDeviceNumber ] = pVideo; 

			pDevice->m_eVideoEncoderStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000;

			is_success = 1;

			//encoder will be streaming
			pVideo->m_video_preview_streaming = 1;

		}
	}
	if( is_success == 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_file_operations_open() - reopen() - [FAIL]\n", (int)(pDevice->m_nKsDeviceNumber) );

		FREE( pVideo );

		ret = -ENOMEM;

		goto common_file_operations_open_fail;
	}

//	common_custom_thread_start( p_file );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_open() end, m_video_preview_streaming(0x%x), m_nSubDeviceType(0x%x)\n", (int)(pDevice->m_nKsDeviceNumber), pVideo->m_video_preview_streaming, pVideo->m_nSubDeviceType );

	return 0;

common_file_operations_open_fail:

	return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)

static int common_file_operations_release( struct file * p_file )

#else

static int common_file_operations_release( struct inode * p_inode, struct file * p_file )

#endif
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_release() m_nSubDeviceType(0x%x) m_Preview_Opened_counter(0x%x)\n", (int)(pDevice->m_nKsDeviceNumber) , pVideo->m_nSubDeviceType, pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ]);

	//use counter instead of m_video_preview_streaming
	if( pVideo->m_nSubDeviceType == 0x00000001 )
	{
		if( pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ] > 0)
		{
			pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ]--;

			if(pDevice->m_Preview_Opened_counter[ pVideo->m_nSubDeviceNumber ] != 0)
			{
				return 0;
			}

		}
		else
		{
			return 0;
		}
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_release() enter\n", (int)(pDevice->m_nKsDeviceNumber) );



//	common_custom_thread_stop( p_file );

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

	//???? sometimes killall -9 vlc will error
	videobuf_stop( &(pVideo->m_sVideoBufQueue) ); //

#if 0
void videobuf_queue_cancel(struct videobuf_queue *q)
{
        unsigned long flags = 0;
        int i;

        q->streaming = 0;
        q->reading  = 0;
        wake_up_interruptible_sync(&q->wait);

        /* remove queued buffers from list */
        spin_lock_irqsave(q->irqlock, flags);
        for (i = 0; i < VIDEO_MAX_FRAME; i++) {
                if (NULL == q->bufs[i])
                        continue;
                if (q->bufs[i]->state == VIDEOBUF_QUEUED) {
                        list_del(&q->bufs[i]->queue);
                        q->bufs[i]->state = VIDEOBUF_ERROR;
                        wake_up_all(&q->bufs[i]->done);
                }
        }
        spin_unlock_irqrestore(q->irqlock, flags);

        /* free all buffers + clear queue */
        for (i = 0; i < VIDEO_MAX_FRAME; i++) {
                if (NULL == q->bufs[i])
                        continue;
                q->ops->buf_release(q, q->bufs[i]);
        }
        INIT_LIST_HEAD(&q->stream);
}
#endif //0

	#else

	videobuf_streamoff( &(pVideo->m_sVideoBufQueue) );

	videobuf_read_stop( &(pVideo->m_sVideoBufQueue) );

	#endif

	videobuf_mmap_free( &(pVideo->m_sVideoBufQueue) );

	if( pVideo->m_nSubDeviceType == 0x00000001 ) {

		if( pVideo == pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] ) { pDevice->m_eVideoStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000; }

		if( pVideo == pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] ) { pDevice->m_pVideo[ pVideo->m_nSubDeviceNumber ] = NULL; }
	}
	if( pVideo->m_nSubDeviceType == 0x00000002 ) {

		if( pVideo == pDevice->m_pVideoEncoder[ pVideo->m_nSubDeviceNumber ] ) { pDevice->m_eVideoEncoderStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000; }

		if( pVideo == pDevice->m_pVideoEncoder[ pVideo->m_nSubDeviceNumber ] ) { pDevice->m_pVideoEncoder[ pVideo->m_nSubDeviceNumber ] = NULL; }
	}
	p_file->private_data = NULL;

	FREE( pVideo );

	return 0;
}

static ssize_t common_file_operations_read( struct file * p_file, char __user * p_data, size_t n_count, loff_t * p_offset )
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

//	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_read( B:%08X, C:%08d, O:%08d )\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(p_data), (int)(n_count), (int)(*p_offset) );

	return -ENODEV;
}

static unsigned int common_file_operations_poll( struct file * p_file, struct poll_table_struct * p_poll_table_struct_wait )
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_poll() return (0x%x)\n", (int)(pDevice->m_nKsDeviceNumber), POLLIN | POLLRDNORM );
/*
	if( list_empty( &pVideo->m_sKsBufferList ) == 0 ) { //not empty
	}
	else
	{
		return POLLERR;
	}

	return -ENODEV;
*/
	return POLLIN | POLLRDNORM;
}

static int common_file_operations_mmap( struct file * p_file, struct vm_area_struct * p_vma )
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	int ret = 0;

	if( pVideo->m_video_preview_streaming == 1 )
	{
		ret = videobuf_mmap_mapper( &(pVideo->m_sVideoBufQueue), p_vma );
	}
	else
	{
		ret = -EINVAL;;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_mmap( S:%08X, E:%08X, R:%08X )\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(p_vma->vm_start), (unsigned int)(p_vma->vm_end), ret );

	return ret;
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,17)

static int common_file_operations_ioctl( struct inode * p_inode, struct file * p_file, unsigned int cmd, unsigned long arg )
{
//	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

//	CVideo * pVideo = (CVideo *)(p_file->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_ioctl( %08X )\n", (int)(pDevice->m_nKsDeviceNumber), (cmd) );

	return video_usercopy( p_inode, p_file, cmd, arg, common_file_operations_do_ioctl );
}

static int common_file_operations_do_ioctl( struct inode * p_inode, struct file * p_file, unsigned int cmd, void * p_arg )
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_file_operations_do_ioctl( %08X )\n", (int)(pDevice->m_nKsDeviceNumber), (cmd) );

	switch( cmd ) {

	case VIDIOC_QUERYCAP:   return common_video_device_vidioc_querycap( p_file, p_file->private_data, (struct v4l2_capability *)(p_arg) ); 

	case VIDIOC_ENUMINPUT:  return common_video_device_vidioc_enum_input( p_file, p_file->private_data, (struct v4l2_input *)(p_arg) );

	case VIDIOC_S_INPUT:    return common_video_device_vidioc_s_input( p_file, p_file->private_data, *((unsigned int *)(p_arg)) );

	case VIDIOC_G_INPUT:    return common_video_device_vidioc_g_input( p_file, p_file->private_data, (unsigned int *)(p_arg) );

	case VIDIOC_ENUM_FMT:   return common_video_device_vidioc_enum_fmt_cap( p_file, p_file->private_data, (struct v4l2_fmtdesc *)(p_arg) );
	
	case VIDIOC_TRY_FMT:    return common_video_device_vidioc_try_fmt_cap( p_file, p_file->private_data, (struct v4l2_format *)(p_arg) );
	
	case VIDIOC_G_FMT:      return common_video_device_vidioc_g_fmt_cap( p_file, p_file->private_data, (struct v4l2_format *)(p_arg) );
	
	case VIDIOC_S_FMT:      return common_video_device_vidioc_s_fmt_cap( p_file, p_file->private_data, (struct v4l2_format *)(p_arg) );
	
	case VIDIOC_QUERYCTRL:  return common_video_device_vidioc_queryctrl( p_file, p_file->private_data, (struct v4l2_queryctrl *)(p_arg) );

	case VIDIOC_G_CTRL:     return common_video_device_vidioc_g_ctrl( p_file, p_file->private_data, (struct v4l2_control *)(p_arg) );

	case VIDIOC_S_CTRL:     return common_video_device_vidioc_s_ctrl( p_file, p_file->private_data, (struct v4l2_control *)(p_arg) );

	case VIDIOC_G_MPEGCOMP: return common_video_device_vidioc_g_mpegcomp( p_file, p_file->private_data, (struct v4l2_mpeg_compression *)(p_arg) );

	case VIDIOC_S_MPEGCOMP: return common_video_device_vidioc_s_mpegcomp( p_file, p_file->private_data, (struct v4l2_mpeg_compression *)(p_arg) );

	case VIDIOC_REQBUFS:    return common_video_device_vidioc_reqbufs( p_file, p_file->private_data, (struct v4l2_requestbuffers *)(p_arg) );
	
	case VIDIOC_QUERYBUF:   return common_video_device_vidioc_querybuf( p_file, p_file->private_data, (struct v4l2_buffer *)(p_arg) );

	case VIDIOC_QBUF:       return common_video_device_vidioc_qbuf( p_file, p_file->private_data, (struct v4l2_buffer *)(p_arg) );

	case VIDIOC_DQBUF:      return common_video_device_vidioc_dqbuf( p_file, p_file->private_data, (struct v4l2_buffer *)(p_arg) );

	case VIDIOC_STREAMON:   return common_video_device_vidioc_streamon( p_file, p_file->private_data, *((unsigned int *)(p_arg)) );
	
	case VIDIOC_STREAMOFF:  return common_video_device_vidioc_streamoff( p_file, p_file->private_data, *((unsigned int *)(p_arg)) );

	case VIDIOC_S_STD: 	return common_video_device_vidioc_s_std( p_file, p_file->private_data, (v4l2_std_id *)(p_arg) );

	case VIDIOC_G_STD: {

		v4l2_std_id * p_std_id = (v4l2_std_id *)(p_arg);

		*p_std_id = pVideo->m_nStandard;

		*p_std_id = pVideo->m_pDevice->m_nCustomVideoStandardProperty;

//		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_std( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(*p_std_id) );

		return 0;
	}
	case VIDIOC_ENUMSTD: {

		struct v4l2_standard * p_standard = (struct v4l2_standard *)(p_arg);

		int ret = -EINVAL;

		if( p_standard->index == 0 ) {

			ret = v4l2_video_std_construct( p_standard, V4L2_STD_525_60, "V4L2_STD_525_60" );
		}
		if( p_standard->index == 1 ) {

			ret = v4l2_video_std_construct( p_standard, V4L2_STD_625_50, "V4L2_STD_625_50" );
		}
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_std( %02d )\n", (int)(pDevice->m_nKsDeviceNumber), p_standard->index );

		return ret;
	}
	}
	return v4l_compat_translate_ioctl( p_inode, p_file, cmd, p_arg, common_file_operations_do_ioctl );
}

#endif

// ##############################################################################################################################################################################
// 
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0)
#define V4L2_CAP_EXT_PIX_FORMAT         0x00200000  /* Supports the extended pixel format */
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)
#define V4L2_CAP_DEVICE_CAPS            0x80000000  /* sets device capabilities field */
#endif

static int common_video_device_vidioc_querycap( struct file * p_file, void * p_priv, struct v4l2_capability * p_cap )
{
	struct video_device * pKsVideo = (struct video_device *)(video_devdata( p_file ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querycap()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	memset( p_cap, 0, sizeof(struct v4l2_capability) );

	strcpy( p_cap->driver, "LINUXV4L2" );
	
	strcpy( p_cap->card, pKsVideo->name ); // i.e., "TW6805, Video Capture #00.00"

	strcpy( p_cap->bus_info, "PCIe: " );

	strcat( p_cap->bus_info, pVideo->m_pDevice->m_pKsDevice->bus->name );
	
	p_cap->version = KERNEL_VERSION(3,12, 0);

	p_cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_TIMEPERFRAME | V4L2_CAP_AUDIO | V4L2_CAP_DEVICE_CAPS | V4L2_CAP_EXT_PIX_FORMAT;

	if(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160)
	{
		p_cap->capabilities |= V4L2_CAP_VBI_CAPTURE;
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
	p_cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_AUDIO;
	#endif

	//p_cap->reserved[ 0 ] = ((pVideo->m_pDevice->bcdDevice << 16 ) & 0xFFFF0000 ) | ((pVideo->m_pDevice->iProduct  <<  8) & 0xFF00) | ((pVideo->m_pDevice->iManufacturer << 0) & 0x00FF);

	if( pVideo->m_pDevice->bcdDevice == 0x6810 || 
		pVideo->m_pDevice->bcdDevice == 0x6811 || 
		pVideo->m_pDevice->bcdDevice == 0x6812 || 
		pVideo->m_pDevice->bcdDevice == 0x6813 ||
		pVideo->m_pDevice->bcdDevice == 0x6814 || 
		pVideo->m_pDevice->bcdDevice == 0x6815 || 
		pVideo->m_pDevice->bcdDevice == 0x6816 || 
		pVideo->m_pDevice->bcdDevice == 0x6817 ) {

		if( pVideo->m_pDevice->iProduct == 0xC3 ) {

			p_cap->reserved[ 0 ] = (0x6816 << 16) & 0xFFFF0000 | ((pVideo->m_pDevice->iProduct  <<  8) & 0xFF00) | ((pVideo->m_pDevice->iManufacturer << 0) & 0x00FF);

		}
		else {

			p_cap->reserved[ 0 ] = (0x12AB << 16) & 0xFFFF0000 | ((pVideo->m_pDevice->iProduct  <<  8) & 0xFF00) | ((pVideo->m_pDevice->iManufacturer << 0) & 0x00FF);
		}
	}


	return 0;
}

static int common_video_device_vidioc_querystd( struct file * p_file, void * p_priv, v4l2_std_id * s )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querystd( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(pVideo->m_nStandard) );

 	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querystd( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(pVideo->m_pDevice->m_nCustomVideoStandardProperty) );

   *s = (v4l2_std_id)(pVideo->m_nStandard);

   *s = (v4l2_std_id)(pVideo->m_pDevice->m_nCustomVideoStandardProperty);

	return 0;
}

static int common_video_device_vidioc_g_std( struct file * p_file, void * p_priv, v4l2_std_id * s )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	*s = pVideo->m_pDevice->m_nCustomVideoStandardProperty;

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) || 
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) || 
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) || 
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) || 
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) || 
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813)  )
	{

		BYTE R00000204 = (BYTE)(TW6805_GetRegister( pVideo->m_pDevice, 0x00000204, 1 ));

		if( (R00000204 & 0x000000C0) == 0x00000040 )
		{
			if( R00000204 & 0x1)
			{
				*s = V4L2_STD_PAL_B;
		
				//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_std( PAL )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

			}
			else
			{
				*s = V4L2_STD_NTSC_M;

				//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_std( NTSC )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );
			}
		}
	}

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_std( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(*s) );
	
	return 0;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
//vidioc_s_std is wrong, parameter should be pointer
static int common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id s )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 1\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), s );

	struct pci_dev *p_pci_dev = pVideo->m_pDevice->m_pKsDevice;
	
	pVideo->m_pDevice->m_nCustomVideoStandardProperty = (ULONG)(s);

	pVideo->m_nStandard = (ULONG)(s);
//peter [2011.04.11]

	int std_err = 1;
	
	if((unsigned int)(s) & V4L2_STD_NTSC)
	{
		if((unsigned int)(s) & V4L2_STD_PAL)//std err
		{
			NULL;		
		}
		else
		{
			std_err = 0;		
		}
			
	}	
	if((unsigned int)(s) & V4L2_STD_PAL)
	{
		if((unsigned int)(s) & V4L2_STD_NTSC)//std err
		{
			NULL;		
		}
		else
		{
			std_err = 0;		
		}
			
	}	
//force to default
	if(std_err)

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static int common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id * s )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	struct pci_dev *p_pci_dev = pVideo->m_pDevice->m_pKsDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 2\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(*s) );

	pVideo->m_pDevice->m_nCustomVideoStandardProperty = (ULONG)(*s);

	pVideo->m_nStandard = (ULONG)(*s);
//peter [2011.04.11]

	int std_err = 1;
	
	if((unsigned int)(*s) & V4L2_STD_NTSC)
	{
		if((unsigned int)(*s) & V4L2_STD_PAL)//std err
		{
			NULL;		
		}
		else
		{
			std_err = 0;		
		}
			
	}	
	if((unsigned int)(*s) & V4L2_STD_PAL)
	{
		if((unsigned int)(*s) & V4L2_STD_NTSC)//std err
		{
			NULL;		
		}
		else
		{
			std_err = 0;		
		}
			
	}	
	if(std_err)

#else
//vidioc_s_std is wrong, parameter should be pointer
static int common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id s )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 3\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), s );

	struct pci_dev *p_pci_dev = pVideo->m_pDevice->m_pKsDevice;
	
	pVideo->m_pDevice->m_nCustomVideoStandardProperty = (ULONG)(s);

	pVideo->m_nStandard = (ULONG)(s);
//peter [2011.04.11]

	int std_err = 1;
	
	if((unsigned int)(s) & V4L2_STD_NTSC)
	{
		if((unsigned int)(s) & V4L2_STD_PAL)//std err
		{
			NULL;		
		}
		else
		{
			std_err = 0;		
		}
			
	}	
	if((unsigned int)(s) & V4L2_STD_PAL)
	{
		if((unsigned int)(s) & V4L2_STD_NTSC)//std err
		{
			NULL;		
		}
		else
		{
			std_err = 0;		
		}
			
	}	
//force to default
//	if(std_err)
#endif
	{	
		pVideo->m_pDevice->m_nCustomVideoStandardProperty = LINUXV4L2_DEFAULT_VIDEO_STANDARD;

		pVideo->m_nStandard = LINUXV4L2_DEFAULT_VIDEO_STANDARD;

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) force to default\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nStandard );

	}
	
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 4\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nStandard );


	if( ( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6801 ) ) { TW6805_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice ); }

	if( ( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6804 ) ) { TW6805_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice ); }

	if( ( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6810 ) ) { TW6805_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice ); }

	if( ( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6811 ) ) { TW6805_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice ); }

	if( ( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6812 ) ) { TW6805_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice ); }

	if( ( p_pci_dev->vendor == 0x1797 && p_pci_dev->device == 0x6813 ) ) { TW6805_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice ); }

	if( ( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x8210 ) ) { CX2581_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice, pVideo ); }

	if( ( p_pci_dev->vendor == 0x14F1 && p_pci_dev->device == 0x5851 ) ) { CX2588_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice, pVideo ); }

	if( ( p_pci_dev->vendor == 0x1131 && p_pci_dev->device == 0x7160 ) ) { SA7160_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice, pVideo->m_nStandard ); }

	if( ( p_pci_dev->vendor == 0x0907 && p_pci_dev->device == 0x0100 ) ) { MZ0380_SetAnalogVideoDecoderStandardProperty( pVideo->m_pDevice, pVideo->m_nStandard, false); }

	return 0;
}

static int common_video_device_vidioc_enumaudio(struct file *p_file, void *priv, struct v4l2_audio *a)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	CDevice * pDevice = pVideo->m_pDevice; // shawn 2010.11.26

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] vidioc_enumaudio( %02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), a->index );

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ||

		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160)	) { 

        if (a->index == 0)
		{
			strcpy(a->name, "embedded");

			a->capability = V4L2_AUDCAP_STEREO;
			
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] vidioc_enumaudio() index(0d%d) capability(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), a->index, a->capability );

			return 0;
		}

        if (a->index == 1)
		{

			strcpy(a->name, "Line in");

			a->capability = V4L2_AUDCAP_STEREO;
			
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] vidioc_enumaudio() index(0d%d) capability(0x%x) ---\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), a->index, a->capability );
			return 0;
		}

	}
	return -EINVAL;
}

static int common_video_device_vidioc_enum_input( struct file * p_file, void * p_priv, struct v4l2_input * p_input )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	CDevice * pDevice = pVideo->m_pDevice; // shawn 2010.11.26

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( %02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_input->index );

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) { 
		
		if( p_input->index == 0 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
//			p_input->status = (pVideo->m_nLockStatus >> 0) & (0x00000001);
//peter[2011.04.29]
			p_input->status = ((pVideo->m_nLockStatus >> 0) & (0x00000001))? 0x0: 0x102;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 
			
			return 0;
		}
		if( p_input->index == 1 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
//			p_input->status = (pVideo->m_nLockStatus >> 1) & (0x00000001);
//peter[2011.04.29]
			p_input->status = ((pVideo->m_nLockStatus >> 1) & (0x00000001))? 0x0: 0x102;

			strcpy( p_input->name, "#01 COMPOSITE INPUT" );
			
			return 0;
		}
		if( p_input->index == 2 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
//			p_input->status = (pVideo->m_nLockStatus >> 2) & (0x00000001);
//peter[2011.04.29]
			p_input->status = ((pVideo->m_nLockStatus >> 2) & (0x00000001))? 0x0: 0x102;

			strcpy( p_input->name, "#02 COMPOSITE INPUT" );
			
			return 0;
		}
		if( p_input->index == 3 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
//			p_input->status = (pVideo->m_nLockStatus >> 3) & (0x00000001);
//peter[2011.04.29]
			p_input->status = ((pVideo->m_nLockStatus >> 3) & (0x00000001))? 0x0: 0x102;

			strcpy( p_input->name, "#03 COMPOSITE INPUT" );
			
			return 0;
		}
		//this is for ffmpeg 2.X, 
		if( p_input->index == 4 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
			p_input->status = 0x0;

			strcpy( p_input->name, "dummy INPUT for ffmpeg V2.X" );
			
			return 0;
		}
	}

	//shawn 2010.11.26
	//
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) || 
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) { // shawn 2010.07.12
		
		if( p_input->index == 0 ) {

			ULONG R0000100C = 0x00000000;

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
			R0000100C = CX2581_GetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x100C + (0x0200 * pVideo->m_nSubDeviceNumber)), 1 ); 

			pVideo->m_nLockStatus = (R0000100C & 0x00000010) ? (0) : (2);

			p_input->status = (R0000100C & 0x00000010) ? (0) : (2);

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 

			return 0;
		}
	}
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ){

		ULONG i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;

		if( p_input->index == 0 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
//			p_input->status = g_n_analog_decoder_lock_status[ i ] & 0x00000001;
//peter [2011.04.12]
			p_input->status = g_n_analog_decoder_lock_status[ i ] & 0x00000FFF;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 

			return 0;
		}
	}


	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { 

		if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
		{
			if( p_input->index == 0 || p_input->index == 4) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				p_input->status = ((((pVideo->m_pDevice->m_nAnalogVideoDecoderStatusProperty) >> (pVideo->m_nSubDeviceNumber % 4) ) & 0x1) ? 0: 0x102);

				strcpy( p_input->name, "COMPOSITE INPUT" ); 

				//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				return 0;
			}	
		}


		if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) || (pVideo->m_pDevice->iProduct & 0x0F) == (0x07) )
		{
			if( (pVideo->m_pDevice->iManufacturer) == 0x8A )
			{
				if( p_input->index == 0 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif
					
					strcpy( p_input->name, "HDMI INPUT(0)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
					return 0;
				}	
			}
			else if((pVideo->m_pDevice->iManufacturer) == 0x8B)
			{
				/*
				if( p_input->index == 0 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);

					strcpy( p_input->name, "SDI INPUT" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
					return 0;
				}
				*/
				// 0 must exist
				if( (p_input->index == 0) || (p_input->index == 1) || (p_input->index == 2) || (p_input->index == 3) ) {

					p_input->status = 0x102;

					strcpy( p_input->name, "NULL" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}
				
				if( p_input->index == 4 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
					{
						p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
					}
					else
					{
						p_input->status = 0x102;
					}
					
					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif

					strcpy( p_input->name, "SDI INPUT(4)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}

			}
			else
			{
				if( p_input->index == 0 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
					{
						p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
					}
					else
					{
						p_input->status = 0x102;
					}
					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif

					strcpy( p_input->name, "HDMI INPUT(0)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}

				if( p_input->index == 1 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
					{
						p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
					}
					else
					{
						p_input->status = 0x102;
					}
					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif

					strcpy( p_input->name, "DVI-D INPUT(1)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}
				if( p_input->index == 2 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
					{
						p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
					}
					else
					{
						p_input->status = 0x102;
					}
					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif

					strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}
				if( p_input->index == 3 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
					{
						p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
					}
					else
					{
						p_input->status = 0x102;
					}
					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif

					strcpy( p_input->name, "DVI-A INPUT(3)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}
				if( p_input->index == 4 ) {

					p_input->type = V4L2_INPUT_TYPE_CAMERA; //
					
					p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
					
					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
					{
						p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
					}
					else
					{
						p_input->status = 0x102;
					}
					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
					p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
					#endif

					strcpy( p_input->name, "SDI INPUT(4)" ); 

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_input( 0x%x )--\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				
					return 0;
				}

			}
		}
	}

	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { 

		if( p_input->index == 0 ) {
		
			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
			p_input->status = ((pVideo->m_pDevice->m_nAnalogVideoDecoderStatusProperty) >> 0 ) & 0x1 ? 0: 0x102;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 

			return 0;
		}
		if( p_input->index == 1 ) {
		
			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
			p_input->status = ((pVideo->m_pDevice->m_nAnalogVideoDecoderStatusProperty) >> 1 ) & 0x1 ? 0: 0x102;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 

			return 0;
		}
		if( p_input->index == 2 ) {
		
			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
			p_input->status = ((pVideo->m_pDevice->m_nAnalogVideoDecoderStatusProperty) >> 2 ) & 0x1 ? 0: 0x102;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 

			return 0;
		}
		if( p_input->index == 3 ) {
		
			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
			
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
			
			p_input->status = ((pVideo->m_pDevice->m_nAnalogVideoDecoderStatusProperty) >> 3 ) & 0x1 ? 0: 0x102;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 

			return 0;
		}
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { 

		if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x17) ||

			(pVideo->m_pDevice->iManufacturer == 0x04 && pVideo->m_pDevice->iProduct == 0x07) ||
				
			(pVideo->m_pDevice->iManufacturer == 0x0A && pVideo->m_pDevice->iProduct == 0x07) )
		{
			// 0 must exist
			if( (p_input->index == 0) || (p_input->index == 4) ) {

				p_input->status = 0x102;

				strcpy( p_input->name, "NULL" ); 

				return 0;
			}
			
			if( p_input->index == 1 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "DVI-D INPUT(1)" ); 

				return 0;
			}

			if( p_input->index == 2 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" ); 

				return 0;
			}

			if( p_input->index == 3 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-A INPUT(3)" ); 

				return 0;
			}

			if( p_input->index == 5 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 5)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "COMPOSITE INPUT(5)" ); 

				return 0;
			}
			if( p_input->index == 6 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 6)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "S-VIDEO INPUT(6)" ); 

				return 0;
			}
		}

		if( ((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x1B)) || //SC510 SDI N4
			
			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x12)) ||  //SC510 SDI N2

			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x08)) )  //SC502 SDI MC
		{
			// 0 must exist
			if( (p_input->index == 0) || (p_input->index == 1) || (p_input->index == 2) || (p_input->index == 3) ) {

				p_input->status = 0x102;

				strcpy( p_input->name, "NULL" ); 

				return 0;
			}
			
			if( p_input->index == 4 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "SDI INPUT(4)" ); 

				return 0;
			}
		}

		if( ((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x1A)) || //SC510 HDMI N4
			
			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x18)) || //SC510 HDMI N2

			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x16)) || //SC510 HDMI N2
			
			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x0D)) ||  //SC502 HDMI MC

			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x0A)) ||  //SC502 HDMI MC

			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x13)) )  //SC510N2-L4K

		{
			// 0 must exist
			
			if( p_input->index == 0 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "HDMI INPUT(0)" ); 

				return 0;
			}
		}

		if( ((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x00)) || //SC500 N1
			((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x10)) )  //SC510 N1
		{
			// 0 must exist
			if( p_input->index == 0 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "HDMI INPUT(0)" ); 

				return 0;
			}
			if( p_input->index == 1 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-D INPUT(1)" ); 

				return 0;
			}

			if( p_input->index == 2 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" ); 

				return 0;
			}

			if( p_input->index == 3 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-A INPUT(3)" ); 

				return 0;
			}

			if( p_input->index == 4 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "SDI INPUT(4)" ); 

				return 0;
			}
			if( p_input->index == 5 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 5)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "COMPOSITE INPUT(5)" ); 

				return 0;
			}

			if( p_input->index == 6 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 6)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "S-VIDEO INPUT(6)" ); 

				return 0;
			}

		}

		if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x0C) )     //SC502 MC HDMI, DVI, VGA, component
		{
			// 0 must exist
			if( p_input->index == 0 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "HDMI INPUT(0)" ); 

				return 0;
			}
			if( p_input->index == 1 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-D INPUT(1)" ); 

				return 0;
			}

			if( p_input->index == 2 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" ); 

				return 0;
			}

			if( p_input->index == 3 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "DVI-A INPUT(3)" ); 

				return 0;
			}
		}

		if( (pVideo->m_pDevice->iManufacturer == 0x00 && pVideo->m_pDevice->iProduct == 0x07) ||

			(pVideo->m_pDevice->iManufacturer == 0x01 && pVideo->m_pDevice->iProduct == 0x07) ||

			(pVideo->m_pDevice->iManufacturer == 0x03 && pVideo->m_pDevice->iProduct == 0x07) )
		{
			// 0 must exist
			if( p_input->index == 0 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "HDMI INPUT(0)" ); 

				return 0;
			}
			if( p_input->index == 1 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-D INPUT(1)" ); 

				return 0;
			}

			if( p_input->index == 2 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" ); 

				return 0;
			}

			if( p_input->index == 3 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "DVI-A INPUT(3)" ); 

				return 0;
			}
			if( p_input->index == 4 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "SDI INPUT(4)" ); 

				return 0;
			}
		}
////////////////////////////////////////
		if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x03) ) //SC500, DVI-D, DVI-A, component
		{
			// 0 must exist
			if( p_input->index == 0 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "HDMI INPUT(0)" ); 

				return 0;
			}
			if( p_input->index == 1 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-D INPUT(1)" ); 

				return 0;
			}

			if( p_input->index == 2 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" ); 

				return 0;
			}

			if( p_input->index == 3 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "DVI-A INPUT(3)" ); 

				return 0;
			}
		}

//////////////////////////////////////////
	}

	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { 
	
		if( ((pDevice->iProduct & 0x0F) == 0x02) || ((pDevice->iProduct & 0x0F) == 0x03)  )  // 960H & D1
		{
			p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //

			if( p_input->index == 0 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 0) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#00 COMPOSITE INPUT" ); 
				
				return 0;
			}

			if( p_input->index == 1 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 1) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#01 COMPOSITE INPUT" ); 
				
				return 0;
			}

			if( p_input->index == 2 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 2) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#02 COMPOSITE INPUT" ); 
				
				return 0;
			}
			if( p_input->index == 3 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 3) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#03 COMPOSITE INPUT" ); 
				
				return 0;
			}
			if( p_input->index == 4 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 4) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#04 COMPOSITE INPUT" ); 
				
				return 0;
			}
			if( p_input->index == 5 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 5) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#05 COMPOSITE INPUT" ); 
				
				return 0;
			}
			if( p_input->index == 6 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 6) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#06 COMPOSITE INPUT" ); 
				
				return 0;
			}
			if( p_input->index == 7 ) {
				
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty >> 7) & (0x00000001))? 0x0: 0x102;

				strcpy( p_input->name, "#07 COMPOSITE INPUT" ); 
				
				return 0;
			}
		}
		else
		{
			// 0 must exist
			if( p_input->index == 0 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "HDMI INPUT(0)" ); 

				return 0;
			}
			if( p_input->index == 1 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif
				
				strcpy( p_input->name, "DVI-D INPUT(1)" ); 

				return 0;
			}

			if( p_input->index == 3 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "DVI-A INPUT(3)" ); 

				return 0;
			}
			if( p_input->index == 4 ) {

				p_input->type = V4L2_INPUT_TYPE_CAMERA; //
				
				p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50; //
				
				if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
				{
					p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusProperty & 0x01) ? 0x0: 0x102);
				}
				else
				{
					p_input->status = 0x102;
				}
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
				p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
				#endif

				strcpy( p_input->name, "SDI INPUT(4)" ); 

				return 0;
			}
		}
	}

	return -EINVAL;
}

static int common_video_device_vidioc_s_input( struct file * p_file, void * p_priv, unsigned int i )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_input( %02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), i );


	if( ( i ) != pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty )
	{
		pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty = ( i);

		pVideo->m_nInput = ( i);

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			SA7160_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice);

			if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x17) ||

				(pVideo->m_pDevice->iManufacturer == 0x04 && pVideo->m_pDevice->iProduct == 0x07) ||
					
				(pVideo->m_pDevice->iManufacturer == 0x0A && pVideo->m_pDevice->iProduct == 0x07) )
			{
				if(pVideo->m_nInput > 6)
				{
					return -EINVAL;
				}
			}

			if( ((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x1B)) || //SC510 SDI N4
				
				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x12)) ||  //SC510 SDI N2

				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x08)) )  //SC502 SDI MC
			{
				if(pVideo->m_nInput > 4)
				{
					return -EINVAL;
				}
			}

			if( ((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x1A)) || //SC510 HDMI N4
				
				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x18)) || //SC510 HDMI N2

				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x16)) || //SC510 HDMI N2
				
				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x0D)) ||  //SC502 HDMI MC

				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x0A)) ||  //SC502 HDMI MC

				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x13)) )  //SC510N2-L4K

			{
				if(pVideo->m_nInput > 0)
				{
					return -EINVAL;
				}
			}

			if( ((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x00)) || //SC500 N1
				((pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x10)) )  //SC510 N1
			{

				LINUXV4L2_PRINT( KERN_INFO, "[%02d] try input(%d) ??????????????????", pVideo->m_pDevice->m_nKsDeviceNumber, pVideo->m_nInput );

				if(pVideo->m_nInput > 6)
				{
					LINUXV4L2_PRINT( KERN_INFO, "[%02d] wrong input(%d) ??????????????????", pVideo->m_pDevice->m_nKsDeviceNumber, pVideo->m_nInput );
					return -EINVAL;
				}
			}

			if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x0C) )     //SC502 MC HDMI, DVI, VGA, component
			{
				if(pVideo->m_nInput > 3)
				{
					return -EINVAL;
				}
			}

			if( (pVideo->m_pDevice->iManufacturer == 0x00 && pVideo->m_pDevice->iProduct == 0x07) ||

				(pVideo->m_pDevice->iManufacturer == 0x01 && pVideo->m_pDevice->iProduct == 0x07) ||

				(pVideo->m_pDevice->iManufacturer == 0x03 && pVideo->m_pDevice->iProduct == 0x07) )
			{
				if(pVideo->m_nInput > 4)
				{
					return -EINVAL;
				}
			}
			if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) && (pVideo->m_pDevice->iManufacturer == 0x03) ) //SC500, DVI-D, DVI-A, component
			{
				if(pVideo->m_nInput > 3)
				{
					return -EINVAL;
				}
			}
		}
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			FH8735_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice);

		}
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 )
		{
			MZ0380_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice, pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty, 0, pVideo->m_pDevice->m_nAnalogCrossbarAudioInputProperty, 0, FALSE);
		}
				
	}
	else
	{
		LINUXV4L2_PRINT( KERN_INFO, "[%02d] same input(%d) ", pVideo->m_pDevice->m_nKsDeviceNumber, pVideo->m_nInput );
	}

	return 0;
}

static int common_video_device_vidioc_g_input( struct file * p_file, void * p_priv, unsigned int * i )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_input( %02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (int)(pVideo->m_nInput) );

   *i = pVideo->m_nInput;

	return 0;
}

static int common_video_device_vidioc_s_audio( struct file * p_file, void *fh, const struct v4l2_audio *a )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_audio() index(0d%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), a->index );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_audio() mode(0d%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), a->mode );

	if(a->index > 1)
	{
		return -EINVAL;
	}

	if( (a->index & 0xF) != pVideo->m_pDevice->m_nAnalogCrossbarAudioInputProperty)
	{
		pVideo->m_pDevice->m_nAnalogCrossbarAudioInputProperty = (a->index & 0xF);


		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			SA7160_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice);


		}
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			FH8735_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice);
		}
	}
	return 0;
}
static int common_video_device_vidioc_g_audio(struct file *p_file, void *fh, struct v4l2_audio *a)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	a->index = pVideo->m_pDevice->m_nAnalogCrossbarAudioInputProperty;

	return 0;
}
/*
static int	common_video_device_vidioc_enum_dv_presets(struct file *p_file, void *fh, struct v4l2_dv_enum_preset *preset)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);
	return 0;
}
*/
static int common_video_device_vidioc_s_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_parm() \n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	if (sp->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if( (sp->parm.capture.timeperframe.numerator > 0) && (sp->parm.capture.timeperframe.denominator > 0) && (sp->parm.capture.timeperframe.denominator > sp->parm.capture.timeperframe.numerator) )
	{
		pVideo->m_pDevice->m_Preview_frame_rate_setting = (sp->parm.capture.timeperframe.denominator * 1000)/ sp->parm.capture.timeperframe.numerator;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_parm() numerator(0d%d) denominator(0d%d) m_Preview_frame_rate_setting(0d%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), sp->parm.capture.timeperframe.numerator, sp->parm.capture.timeperframe.denominator, pVideo->m_pDevice->m_Preview_frame_rate_setting );

	return 0;
}
static int common_video_device_vidioc_g_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	sp->parm.capture.capability |= V4L2_CAP_TIMEPERFRAME;
	
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_parm() capability(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), sp->parm.capture.capability );

	if (sp->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if(pVideo->m_pDevice->m_Preview_frame_rate_setting & 0x80000000)
	{
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

			sp->parm.capture.timeperframe.denominator = p_sys_cfg->n_input_video_resolution_fps * 1000;

			if(p_sys_cfg->n_input_video_resolution_fps_m == 1)
			{
				sp->parm.capture.timeperframe.numerator = 1001;
			}
			else
			{
				sp->parm.capture.timeperframe.numerator = 1000;
			}
		}
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

			sp->parm.capture.timeperframe.denominator = p_sys_cfg->n_input_video_resolution_fps * 1000;

			if(p_sys_cfg->n_input_video_resolution_fps_m == 1)
			{
				sp->parm.capture.timeperframe.numerator = 1001;
			}
			else
			{
				sp->parm.capture.timeperframe.numerator = 1000;
			}
		}

	}
	else
	{
		sp->parm.capture.timeperframe.denominator = (pVideo->m_pDevice->m_Preview_frame_rate_setting) / 1000;

		sp->parm.capture.timeperframe.numerator = 1;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_parm() numerator(0d%d) denominator(0d%d)!!!!!!!!!\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), sp->parm.capture.timeperframe.numerator, sp->parm.capture.timeperframe.denominator );

	return 0;
}

static int common_video_device_vidioc_enum_frameintervals(struct file *p_file, void *fh, struct v4l2_frmivalenum *fival)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_frameintervals() \n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

}

static int common_video_device_vidioc_enum_framesizes(struct file *p_file, void *fh, struct v4l2_frmsizeenum *fsize)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_framesizes() \n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	if (fsize->index > 0)
	    return -EINVAL;
 
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;

	fsize->discrete.width = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;

	fsize->discrete.height = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;
	
	if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
	{
		fsize->discrete.height *= 2;
	}
	return 0;
}
static int common_video_device_vidioc_cropcap(struct file *p_file, void *fh, struct v4l2_cropcap *cropcap)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_cropcap() $$$$$$$$$$$$$$$$\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	int type = cropcap->type;
 
	memset(cropcap, 0, sizeof(*cropcap));
	
	cropcap->type = type;

	if (cropcap->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

//no crop
	cropcap->bounds.top = cropcap->bounds.left = 0;
	
	cropcap->bounds.width = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
	cropcap->bounds.height = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

	if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
	{
		cropcap->bounds.height *= 2;
	}
	
	
	cropcap->defrect.top = cropcap->defrect.left = 0;
	cropcap->defrect.width = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
	cropcap->defrect.height = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;
	if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
	{
		cropcap->defrect.height *= 2;
	}

	cropcap->pixelaspect.numerator = 1;
	cropcap->pixelaspect.denominator = 1;
	
	return 0;
}




#define V4L2_PIX_FMT_MPG4 v4l2_fourcc( 'M', 'P', 'G', '4' )

#define V4L2_PIX_FMT_H264 v4l2_fourcc( 'H', '2', '6', '4' )

static int common_video_device_vidioc_enum_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_fmtdesc * p_fmtdesc )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_enum_fmt_cap( I:%02d, T:%02d, m_nSubDeviceType(%02d) )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmtdesc->index, p_fmtdesc->type, pVideo->m_nSubDeviceType );

	if( p_fmtdesc->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {

		if( pVideo->m_nSubDeviceType == 0x00000001 ) {

			if( p_fmtdesc->index == 0 ) {

				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x9413 && pVideo->m_pDevice->m_pKsDevice->device == 0x6010 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { sprintf( p_fmtdesc->description, "%s", "UYVY 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_UYVY; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { sprintf( p_fmtdesc->description, "%s", "YV12 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420; }
//below is for FC16X64				
//				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { sprintf( p_fmtdesc->description, "%s", "UYVY 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_UYVY; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { sprintf( p_fmtdesc->description, "%s", "YV12 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { sprintf( p_fmtdesc->description, "%s", "YV12 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420; }

				return 0;
			}
		}
		if( pVideo->m_nSubDeviceType == 0x00000002 ) {

			if( p_fmtdesc->index == 0 ) {

				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
			//	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x9413 && pVideo->m_pDevice->m_pKsDevice->device == 0x6010 ) { sprintf( p_fmtdesc->description, "%s", "MPEG4"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_MPG4; }
			//	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { sprintf( p_fmtdesc->description, "%s", "H.264"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_H264; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { sprintf( p_fmtdesc->description, "%s", "H.264"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_H264; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x9413 && pVideo->m_pDevice->m_pKsDevice->device == 0x6010 ) { sprintf( p_fmtdesc->description, "%s", "MPEG4"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { sprintf( p_fmtdesc->description, "%s", "H.264"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV; }
		//		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { sprintf( p_fmtdesc->description, "%s", "H.264"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420; }
//peter [2011.06.29]
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { sprintf( p_fmtdesc->description, "%s", "H.264"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420; }
				if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { sprintf( p_fmtdesc->description, "%s", "H.264"      ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420; }

				return 0;
			}
		}
	}
	return -EINVAL;
}

static int common_video_device_vidioc_try_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_try_fmt_cap( T:%02d, F:%02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmt->type, p_fmt->fmt.pix.field );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_try_fmt_cap() width(%d) height(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmt->fmt.pix.width, p_fmt->fmt.pix.height );

	if( p_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {

		LINUXV4L2_DEBUG( KERN_INFO, "- %08X:%03dX%03d:%d:%d\n", p_fmt->fmt.pix.pixelformat, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.bytesperline, p_fmt->fmt.pix.sizeimage );

		if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420  ||
//not work for ffmpeg			
//			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420  ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_MPG4 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_H264 ) { 

			if( p_fmt->fmt.pix.field == V4L2_FIELD_ANY ) { p_fmt->fmt.pix.field = V4L2_FIELD_INTERLACED; }
			
			//if( p_fmt->fmt.pix.field != V4L2_FIELD_INTERLACED ) { return -EINVAL; }
//below is for gstreamer
/*
			if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1)
			{
				if(p_fmt->fmt.pix.field != V4L2_FIELD_INTERLACED)
				{ 
					return -EINVAL;
				}
			}
			else
			{
				if(p_fmt->fmt.pix.field == V4L2_FIELD_INTERLACED)
				{ 
					return -EINVAL;
				}
			}
*/
			
			if( pVideo->m_pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { 

				if( p_fmt->fmt.pix.width  <  320 ) { p_fmt->fmt.pix.width  =  320; }

				if( p_fmt->fmt.pix.height <  240 ) { p_fmt->fmt.pix.height =  240; }

//				if( p_fmt->fmt.pix.width  > 1024 ) { p_fmt->fmt.pix.width  = 1024; }

//				if( p_fmt->fmt.pix.height >  768 ) { p_fmt->fmt.pix.height =  768; }
			}
			if( pVideo->m_pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { 

				if( p_fmt->fmt.pix.width  <  320 ) { p_fmt->fmt.pix.width  =  320; }

				if( p_fmt->fmt.pix.height <  288 ) { p_fmt->fmt.pix.height =  288; }

//				if( p_fmt->fmt.pix.width  > 1024 ) { p_fmt->fmt.pix.width  = 1024; }

//				if( p_fmt->fmt.pix.height >  768 ) { p_fmt->fmt.pix.height =  768; }
			}
//gstreamer 1.2
			if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420  ) {

				p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width );
				p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
			}
			else {
				
				p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 16) >> (3);
				p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height);
			}

			if( pVideo->m_pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { 

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 480; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 720 * 480 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height <  480 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 240; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 720 * 240 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 480; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 704 * 480 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height <  480 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 240; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 704 * 240 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 480; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 640 * 480 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height <  480 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 240; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 640 * 240 * pVideo->m_nSrcFrameBitCount / 8; } else
				//
				if( p_fmt->fmt.pix.width >= 360 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 360; pVideo->m_nSrcFrameHeight = 240; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 360 * 240 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 352 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 352; pVideo->m_nSrcFrameHeight = 240; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 352 * 240 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 320 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 320; pVideo->m_nSrcFrameHeight = 240; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 320 * 240 * pVideo->m_nSrcFrameBitCount / 8; }
			}
			if( pVideo->m_pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { 

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 576; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 720 * 576 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height <  576 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 288; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 720 * 288 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 576; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 704 * 576 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height <  576 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 288; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 704 * 288 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 576; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 640 * 576 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height <  576 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 288; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 640 * 288 * pVideo->m_nSrcFrameBitCount / 8; } else
				//
				if( p_fmt->fmt.pix.width >= 360 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 360; pVideo->m_nSrcFrameHeight = 288; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 360 * 288 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 352 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 352; pVideo->m_nSrcFrameHeight = 288; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 352 * 288 * pVideo->m_nSrcFrameBitCount / 8; } else

				if( p_fmt->fmt.pix.width >= 320 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 320; pVideo->m_nSrcFrameHeight = 288; pVideo->m_nSrcFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16; pVideo->m_nSrcFrameSize = 320 * 288 * pVideo->m_nSrcFrameBitCount / 8; }
			}
			LINUXV4L2_DEBUG( KERN_INFO, "- RESET!!\n" );
			return 0;

		}//end of format 

	}//end of capture
    return -EINVAL;
}

static int common_video_device_vidioc_s_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_fmt_cap( T:%02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmt->type );

	if( p_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {

		int ret = common_video_device_vidioc_try_fmt_cap( p_file, p_priv, p_fmt );

		if( ret != 0 ) { return ret; }

		LINUXV4L2_DEBUG( KERN_INFO, "- %08X:%03dX%03d:%d:%d\n", p_fmt->fmt.pix.pixelformat, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.bytesperline, p_fmt->fmt.pix.sizeimage );

		if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420  ||
//not work for ffmpeg			
//			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420  ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_MPG4 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_H264 ) { 

			pVideo->m_nDstFrameWidth    = p_fmt->fmt.pix.width;
			pVideo->m_nDstFrameHeight   = p_fmt->fmt.pix.height;
			pVideo->m_nDstFramePitch    = p_fmt->fmt.pix.bytesperline;
			pVideo->m_nDstFrameSize     = p_fmt->fmt.pix.sizeimage;
			pVideo->m_nFramePixelFormat = p_fmt->fmt.pix.pixelformat;
//			pVideo->m_nFramePixelField  = p_fmt->fmt.pix.field;
			
			if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1)
			{
				pVideo->m_nFramePixelField  = V4L2_FIELD_INTERLACED;
			}
			else
			{
				pVideo->m_nFramePixelField  = V4L2_FIELD_NONE;
			}
			

			pVideo->m_nDstFrameBitCount = (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420 ) ? 12 : 16;

			LINUXV4L2_DEBUG( KERN_INFO, "- MATCH!! (SRC:%d:%d) (DST:%d:%d)\n", (int)(pVideo->m_nSrcFrameWidth), (int)(pVideo->m_nSrcFrameHeight), (int)(pVideo->m_nDstFrameWidth), (int)(pVideo->m_nDstFrameHeight) );

			if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) {

				TW5864_update_resolution_dynamic( pVideo->m_pDevice, pVideo );						
			}

			if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {
		
				int index = 0;

				if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
				{
					index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11
				}
				else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) )
				{
					index = ((pVideo->m_nSubDeviceNumber < 1) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 7);// subdevice 0 use array 0, subdevice 1 use array 8
				}
				else
				{
					index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11
				}

				if( pVideo->m_nSubDeviceType == 0x00000001)
				{
					pVideo->m_pDevice->m_nCustomPreviewVideoResolutionProperty[ index ]  =   0x88000000 | (pVideo->m_nDstFrameWidth) << 12 | (pVideo->m_nDstFrameHeight);
				}

				if( pVideo->m_nSubDeviceType == 0x00000002)
				{
					
					pVideo->m_pDevice->m_nCustomEncoderVideoResolutionProperty[ index ]  =   0x88000000 | (pVideo->m_nDstFrameWidth) << 12 | (pVideo->m_nDstFrameHeight);
				
				}
			}

			if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {
		
				int index = 0;
				/*
				if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
				{
					index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11
				}
				else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) )
				{
					index = ((pVideo->m_nSubDeviceNumber < 1) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 7);// subdevice 0 use array 0, subdevice 1 use array 8
				}
				else
				{
					index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11
				}
				*/
				if( ((pVideo->m_pDevice->iProduct & 0x0F) == 0x02) || ((pVideo->m_pDevice->iProduct & 0x0F) == 0x03)  ) { // 960H & D1
				
					if( (pVideo->m_pDevice->iManufacturer & 0x0F) == 0x0A ) {

						//counts = 8;
						index = ((pVideo->m_nSubDeviceNumber < 8) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 8);// subdevice 0~7 use array 0~7, subdevice 8~15 use array 8~15
					}
					else {

						//counts = 4;
						index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11

					}
				}

				if( pVideo->m_nSubDeviceType == 0x00000002)
				{
					pVideo->m_pDevice->m_nCustomEncoderVideoResolutionProperty[ index ]  =   0x88000000 | (pVideo->m_nDstFrameWidth) << 12 | (pVideo->m_nDstFrameHeight);
				}
			}
			return 0;
		}
	}
	return -EINVAL;
}

static int common_video_device_vidioc_g_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt )
{
	struct video_device * pKsVideo = (struct video_device *)(video_devdata( p_file ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap( T:%02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmt->type );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap() m_nDstFrameWidth(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nDstFrameWidth );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap() m_nDstFrameHeight(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nDstFrameHeight );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap() m_nDstFramePitch(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nDstFramePitch );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap() m_nDstFrameSize(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nDstFrameSize );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap() m_nFramePixelFormat(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nFramePixelFormat );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap() m_nFramePixelField(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_nFramePixelField );

	if( p_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {


		p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M; //
		p_fmt->fmt.pix.width        = pVideo->m_nDstFrameWidth;
		p_fmt->fmt.pix.height       = pVideo->m_nDstFrameHeight;
		p_fmt->fmt.pix.bytesperline = pVideo->m_nDstFramePitch;
		p_fmt->fmt.pix.sizeimage    = pVideo->m_nDstFrameSize;
		p_fmt->fmt.pix.pixelformat  = pVideo->m_nFramePixelFormat;
		p_fmt->fmt.pix.field        = pVideo->m_nFramePixelField;
		p_fmt->fmt.pix.priv         = 0;


		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) 
		{
			p_fmt->fmt.pix.width        = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;

			p_fmt->fmt.pix.height       = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

			if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
			{
				p_fmt->fmt.pix.height *= 2;
			}

			if( pVideo->m_nSubDeviceType == 0x00000001)//preview
			{
				if( (p_fmt->fmt.pix.width == 1280) && (p_fmt->fmt.pix.height == 720) )
				{
					if( pVideo->m_pDevice->m_nHVScaleFactor == 0x00020002 )
					{
						p_fmt->fmt.pix.width /= 2;
						p_fmt->fmt.pix.height /= 2;
					}
				}
			
				if( (p_fmt->fmt.pix.width == 1920) && (p_fmt->fmt.pix.height == 1080) )
				{
					if( pVideo->m_pDevice->m_nHVScaleFactor == 0x00020002 )
					{
						p_fmt->fmt.pix.width /= 2;
						p_fmt->fmt.pix.height /= 2;
					}

					if( pVideo->m_pDevice->m_nHVScaleFactor == 0x00040004 )
					{
						p_fmt->fmt.pix.width /= 4;
						p_fmt->fmt.pix.height /= 4;
					}

				}
				p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width );
				p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;

			}

			if( pVideo->m_nSubDeviceType == 0x00000002)//encode
			{
				
				BYTE* pch = NULL;
				
				pch = strchr(pKsVideo->name,'M');

				if( pch )
				{
					//main
					if( ( (pVideo->m_pDevice->m_nH264HVScaleFactor >> 16) & 0x303) == 0x0202)
					{
						p_fmt->fmt.pix.width /= 2;
						p_fmt->fmt.pix.height /= 2;
					}

					if( ( (pVideo->m_pDevice->m_nH264HVScaleFactor >> 16) & 0x303) == 0x0303)
					{
						p_fmt->fmt.pix.width /= 4;
						p_fmt->fmt.pix.height /= 4;
					}
				
					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap( main ) width(%d) height(%d) m_nH264HVScaleFactor(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, pVideo->m_pDevice->m_nH264HVScaleFactor  );

				}
				else
				{
					//sub
					if( (pVideo->m_pDevice->m_nH264HVScaleFactor & 0xFFFF) == 0x0202)
					{
						p_fmt->fmt.pix.width /= 2;
						p_fmt->fmt.pix.height /= 2;
					}

					if( (pVideo->m_pDevice->m_nH264HVScaleFactor & 0xFFFF) == 0x0303)
					{
						p_fmt->fmt.pix.width /= 4;
						p_fmt->fmt.pix.height /= 4;
					}
					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_fmt_cap( sub ) width(%d) height(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_fmt->fmt.pix.width, p_fmt->fmt.pix.height  );

				}
			}


		}
		
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			p_fmt->fmt.pix.width        = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;

			p_fmt->fmt.pix.height       = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

			if( (pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty == 5) || (pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty == 6) )//AV, S
			{
				p_fmt->fmt.pix.width = 720;

				if( pVideo->m_pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 )
				{
					p_fmt->fmt.pix.height = 240;
				}
				else
				{
					p_fmt->fmt.pix.height = 288;
				}
			}

//if gstreamer, disable below
			if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
			{
				p_fmt->fmt.pix.height *= 2;
		
				p_fmt->fmt.pix.field        = V4L2_FIELD_INTERLACED;
			}
			else
			{
				p_fmt->fmt.pix.field        = V4L2_FIELD_NONE;
			}

			if(p_fmt->fmt.pix.width >= 1280)
			{
				p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_REC709;
			}
			else
			{
				p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
			}

			if( pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty == 3)//VGA
			{
				p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_SRGB;
			}

			p_fmt->fmt.pix.bytesperline = p_fmt->fmt.pix.width * 2;

			p_fmt->fmt.pix.sizeimage    = p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height;


		}

		//one strange thing, if below are not 0, s_fmt_cap will not be issued
		#ifdef FFMPEG_V1_1_5
		p_fmt->fmt.pix.width = 0;
		p_fmt->fmt.pix.height = 0;
		#endif

		LINUXV4L2_DEBUG( KERN_INFO, "- %08X:%03dX%03d:%d:%d\n", p_fmt->fmt.pix.pixelformat, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.bytesperline, p_fmt->fmt.pix.sizeimage );


		return 0;
	}
    return -EINVAL;
}
static int common_video_device_vidioc_querymenu( struct file * p_file, void * p_priv, struct v4l2_querymenu *a)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	ULONG i = 0;

	ULONG j = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querymenu( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), a->id );

}


static int common_video_device_vidioc_queryctrl( struct file * p_file, void * p_priv, struct v4l2_queryctrl * p_queryctrl )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	ULONG i = 0;

	ULONG j = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_queryctrl( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_queryctrl->id );

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ) { j =  0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ) { j =  1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) { j =  2; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ) { j =  3; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ) { j =  4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ) { j =  5; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ) { j =  6; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) { j =  7; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ) { j =  8; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ) { j =  9; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ) { j = 10; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ) { j = 11; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { j = 12; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) { j = 13; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) ) { j = 14; }

//	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x9413 && pVideo->m_pDevice->m_pKsDevice->device == 0x6010) ) { j = 15; }
//	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) { j = 16; }

//peter [2011.06.29]
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) { j = 15; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) { j = 16; }

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { j = 17; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) { j = 18; }

	for( i = 0 ; i < ARRAY_SIZE(LINUXV4L2_V4L2_QUERYCTRL); i++ ) {

		if( p_queryctrl->id == LINUXV4L2_V4L2_QUERYCTRL[ i ].id ) {

			if( LINUXV4L2_V4L2_QUERYCTRL_ENABLE[ j ][ i + 2 ] == 1 ) {

				memcpy( p_queryctrl, &(LINUXV4L2_V4L2_QUERYCTRL[ i ]), sizeof(struct v4l2_queryctrl) );

				p_queryctrl->default_value = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ i + 2 ];

				return 0;
			}
		}
	}
    return -EINVAL;
}

static int common_video_device_vidioc_g_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	ULONG i = 0;

	ULONG j = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_ctrl( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_control->id - V4L2_CID_BASE );

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) { j = pVideo->m_nSubDeviceNumber % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) { j = 0; }

	if( p_control->id == V4L2_CID_BRIGHTNESS ) { //0

		p_control->value = g_n_analog_decoder_brightness[ i ][ j ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_CONTRAST ) { //1

		p_control->value = g_n_analog_decoder_contrast[ i ][ j ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SATURATION ) { //2

		p_control->value = g_n_analog_decoder_saturation[ i ][ j ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_HUE ) { //3

		p_control->value = g_n_analog_decoder_hue[ i ][ j ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SHARPNESS ) { //27

		p_control->value = g_n_analog_decoder_sharpness[ i ][ j ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_AUTOGAIN ) { //18

		p_control->value = g_n_analog_decoder_gain_auto[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_GAIN ) { //19

		p_control->value = g_n_analog_decoder_gain[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_SPEED ) { //200

		p_control->value = pVideo->m_nSwitchSpeed_Spliter & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_TABLE ) { //201

		p_control->value = 0x00000000;

		for( i = 0 ; i < 12 ; i++ ) {

			p_control->value |= (ULONG)(pVideo->m_pSwitchTable_Spliter[ i ] & 0x03) << (i * 2);
		}
		return 0;
	}
	if( p_control->id == V4L2_CID_INPUT_SELECT ) { //210

		p_control->value = pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_MOTION_STATUS ) { //250

		p_control->value = g_n_multiplexer_motion_status[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD ) { //251
		
		p_control->value = g_n_multiplexer_motion_sensitivity_low_threshold[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD ) { //252
		
		p_control->value = g_n_multiplexer_motion_sensitivity_high_threshold[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CX ) { //253

		p_control->value = g_n_multiplexer_motion_mask_cx[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CY ) { //254

		p_control->value = g_n_multiplexer_motion_mask_cy[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_X ) { //260

		p_control->value = g_n_encoder_osd_pos_x[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_Y ) { //261

		p_control->value = g_n_encoder_osd_pos_y[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_COLOR ) { //262

		p_control->value = g_n_encoder_osd_color[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DIRECTION ) { //270

		if(	(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) {

			g_n_gpio_direction[ i ] = (BYTE)(pVideo->m_pDevice->m_nCustomGpioDirectionProperty & 0xFF); 
		}


		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) {

			g_n_gpio_direction[ i ] = TW6805_GetRegister( pVideo->m_pDevice, 0x00000028, 4 );
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) {

			ULONG R00110018 = 0x00000000;

			ULONG R0011001C = 0x00000000;

			R00110018 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110018, 4 ); R00110018 = ~R00110018; R00110018 &= 0xFFFFFF00; 

//			R0011001C = CX2581_GetRegister( pVideo->m_pDevice, 0x0011001C, 4 ); R0011001C = ~R0011001C; R0011001C &= 0x000000FF; 

//			g_n_gpio_direction[ i ] = (R00110018 >> 8) | (R0011001C << 24);
			g_n_gpio_direction[ i ] = (R00110018 >> 8);
		}
		// shawn 2010.07.12
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) {

//			ULONG R00110018 = 0x00000000;
//
//			ULONG R0011001C = 0x00000000;
//
//			R00110018 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110018, 4 ); R00110018 = ~R00110018; R00110018 &= 0xFFFFFF00; 
//
//			R0011001C = CX2581_GetRegister( pVideo->m_pDevice, 0x0011001C, 4 ); R0011001C = ~R0011001C; R0011001C &= 0x000000FF; 
//
//			g_n_gpio_direction[ i ] = (R00110018 >> 8) | (R0011001C << 24);
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) {

			ULONG R007D3400 = 0x00000000;
			
			AH8400_GetRegister( pVideo->m_pDevice, 3, 0x007D3000 + 0x00000400, 1 ); R007D3400 &= 0x000000FF;

			g_n_gpio_direction[ i ] = R007D3400;

		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) {
//note: dir can not be read, chip feature

//			p_control->value = g_n_gpio_direction[ pVideo->m_pDevice->m_nKsDeviceNumber ];

			TW5864_SYS_CFG * p_sys_cfg = (TW5864_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

			p_control->value = (p_sys_cfg->R00009801 >> 12) & 0xF;
	
			return 0;
		}

		//
		p_control->value = g_n_gpio_direction[ i ];

		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DATA ) { //271
	
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) {
/*
			TW6802_CLOSE_THREAD();

			BYTE R = (BYTE)(pDevice->m_nCustomGpioDirectionProperty & 0xFF); 

			R = ~R; // INPUT PIN IS ALWAYS HIGH

			R |= (BYTE)(pDevice->m_nCustomGpioDataProperty & 0xFF);

			I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0x40, 125 ); I2C_WRITE_BYTE( pDevice, R, 125 ); I2C_STOP( pDevice, 125 );

			I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0x41, 125 ); R = I2C_READ_BYTE( pDevice, 1, 125 ); I2C_STOP( pDevice, 125 );

			R &= ~((BYTE)(pDevice->m_nCustomGpioDirectionProperty & 0xFF));

			R |= (BYTE)((pDevice->m_nCustomGpioDataProperty & pDevice->m_nCustomGpioDirectionProperty) & 0xFF);

			pDevice->m_nCustomGpioDataProperty = R;

			DELAY_100NS( 1250000 );

			TW6802_START_THREAD();
*/

			BYTE R = (BYTE)(pVideo->m_pDevice->m_nCustomGpioDirectionProperty & 0xFF); 

			R = ~R; // INPUT PIN IS ALWAYS HIGH

			R |= (BYTE)g_n_gpio_data[ i ];
			
			R = TW6816_GET_GPIO_DATA( pVideo->m_pDevice, R );

			R &= ~((BYTE)(pVideo->m_pDevice->m_nCustomGpioDirectionProperty & 0xFF));

			R |= (BYTE) ( (g_n_gpio_data[ i ] & pVideo->m_pDevice->m_nCustomGpioDirectionProperty) & 0xFF);

			g_n_gpio_data[ i ] = R;

		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) {

			g_n_gpio_data[ i ] = TW6805_GetRegister( pVideo->m_pDevice, 0x00000100, 4 );
		
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) {

			ULONG R00110010 = 0x00000000;

			ULONG R00110014 = 0x00000000;

//			R00110010 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110010, 4 ); R00110010 &= 0xFFFFFF00; 
			R00110010 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110010, 4 ); R00110010 &= 0x0000FF00; 
			
//			R00110014 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110014, 4 ); R00110014 &= 0x000000FF; 
			
//			g_n_gpio_data[ i ] = (R00110010 >> 8) | (R00110014 << 24);
			g_n_gpio_data[ i ] = (R00110010 >> 8);
		}
		// shawn 2010.07.12
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) {

//			ULONG R00110010 = 0x00000000;
//
//			ULONG R00110014 = 0x00000000;
//
//			R00110010 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110010, 4 ); R00110010 &= 0xFFFFFF00; 
//			
//			R00110014 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110014, 4 ); R00110014 &= 0x000000FF; 
//			
//			g_n_gpio_data[ i ] = (R00110010 >> 8) | (R00110014 << 24);
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) {

			ULONG R007D33FC = 0x00000000;

			R007D33FC = AH8400_GetRegister( pVideo->m_pDevice, 3, 0x007D3000 + 0x000003FC, 1 ); R007D33FC &= 0x000000FF;

			g_n_gpio_data[ i ] = R007D33FC;
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) {

/*
			ULONG R00009800 = 0x00000000;

			TW5864_StopControlPanelAnalysisThread_EX( pVideo->m_pDevice );

			R00009800 = TW5864_GetRegister( pVideo->m_pDevice, 0x00009800 );	R00009800 &= 0x000000F0;

			TW5864_StartControlPanelAnalysisThread_EX( pVideo->m_pDevice );

			g_n_gpio_data[ i ] = R00009800 >> 4;
*/
			ULONG R00009800 = TW5864_GetRegister( pVideo->m_pDevice, 0x9800 ) & 0x00F0;

			g_n_gpio_data[ i ] = ( R00009800 >> 4 ) & 0xF;

		}


		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) 
		{
			FH8735_StopControlPanelAnalysisThreadEX( pVideo->m_pDevice );

			{	ULONG R = g_n_gpio_direction[ i ] & 0xFFFF;
			
				R = ~R; // INPUT PIN IS ALWAYS HIGH

				R |= g_n_gpio_data[ i ] & 0xFFFF;

				FH8735_SetGpioRegister( pVideo->m_pDevice, 0x00, 0x40, (BYTE)(R >> 0) );

				FH8735_SetGpioRegister( pVideo->m_pDevice, 0x00, 0x42, (BYTE)(R >> 8) );
			}
			USHORT R40 = (USHORT)(FH8735_GetGpioRegister( pVideo->m_pDevice, 0x00, 0x40 ));

			USHORT R42 = (USHORT)(FH8735_GetGpioRegister( pVideo->m_pDevice, 0x00, 0x42 ));

			USHORT R = (R42 << 8) | (R40 << 0); 

			R &= ~(USHORT)(g_n_gpio_direction[ i ] & 0xFFFF);

			R |=  (USHORT)((g_n_gpio_data[ i ] & g_n_gpio_direction[ i ]) & 0xFFFF);

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_ctrl() data R( %08X ) \n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), R );

			g_n_gpio_data[ i ] = R;

			//DELAY_100NS( 1250000 );

			FH8735_StartControlPanelAnalysisThreadEX( pVideo->m_pDevice );
		}
		//
		p_control->value = g_n_gpio_data[ i ];

		return 0;
	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_00 ) { //280

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{
			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 0 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_01 ) { //281

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 1 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_02 ) { //282

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 2 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_03 ) { //283

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 3 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_04 ) { //284

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 4 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_05 ) { //285

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 5 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_06 ) { //286

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 6 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_07 ) { //287

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pVideo->m_pDevice->m_nCustomMotionStatusProperty[ pVideo->m_nSubDeviceNumber ][ 7 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_TEMPORAL_SENSITIVITY ) { //290

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pVideo->m_pDevice->m_nCustomMotionTemporalSensitivityProperty[ pVideo->m_nSubDeviceNumber ] & 0x77FFFFFF;;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPATIAL_SENSITIVITY ) { //291

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pVideo->m_pDevice->m_nCustomMotionSpatialSensitivityProperty[ pVideo->m_nSubDeviceNumber ] & 0x77FFFFFF;;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_LEVEL_SENSITIVITY ) { //292

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pVideo->m_pDevice->m_nCustomMotionLevelSensitivityProperty[ pVideo->m_nSubDeviceNumber ] & 0x77FFFFFF;;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPEED ) { //293

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pVideo->m_pDevice->m_nCustomMotionSpeedProperty[ pVideo->m_nSubDeviceNumber ] & 0x77FFFFFF;;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_FRAME_RATE ) { //208

		p_control->value = pVideo->m_pDevice->m_nCustomAnalogVideoFrameRateProperty & 0x77FFFFFF;;

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			p_control->value = pVideo->m_pDevice->m_Preview_frame_rate_setting;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG ) { //207

		p_control->value = pVideo->m_pDevice->m_nCustomSoftwareWatchdogResetProperty & 0x77FFFFFF;

		return 0;
	}

	if( p_control->id == V4L2_CID_X_RES_DETECTED ) { //294

		p_control->value = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
		
		return 0;
	}

	if( p_control->id == V4L2_CID_Y_RES_DETECTED ) { //295

		p_control->value = ( pVideo->m_pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

		if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
		{
			p_control->value *= 2;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_FPS_DETECTED ) { //296

		p_control->value = ( pVideo->m_pDevice->m_nCustomAnalogVideoFrameRateProperty ) & 0xFFFF;
		
		if( pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
		{
			p_control->value /= 2;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_HDCP_ENABLE ) { //202

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			p_control->value = pVideo->m_pDevice->m_nAnalogCopyProtMacrovisionEnableProperty & 0x1;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_HDCP_STATUS ) { //203

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			p_control->value = pVideo->m_pDevice->m_nAnalogCopyProtMacrovisionProperty & 0x1;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_AUDIO_SAMPLING_FREQ ) { //211

/*
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

			p_control->value = p_sys_cfg->n_input_audio_sampling_frequency;
		}
*/
		p_control->value = ( pVideo->m_pDevice->m_nCustomAnalogAudioSampleFrequencyProperty ) & 0xFFFF;

		return 0;
	}

	if( p_control->id == V4L2_CID_SPECIAL_CAMERA ) { //214
		
		p_control->value = pVideo->m_pDevice->m_nCustomSpecialCameraInputProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_HV_SCALE_FACTOR ) { //204

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			p_control->value = pVideo->m_pDevice->m_nHVScaleFactor;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_H264_HV_SCALE_FACTOR ) { //205

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			p_control->value = (pVideo->m_pDevice->m_nH264HVScaleFactor) & 0x77FFFFFF;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_INTERLEAVE_DETECTED ) { //297

		//if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			p_control->value = pVideo->m_pDevice->m_nCustomAnalogVideoInterleavedProperty;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_ENC_DEINTERLACE ) { //298

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			p_control->value = pVideo->m_pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ]; 
		}

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 )
		{
			p_control->value = pVideo->m_pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ]; 
		}

		return 0;
	}
	if( p_control->id == V4L2_CID_VBI_ENABLE ) { //300

		p_control->value = pVideo->m_pDevice->m_nCustomAnalogVideoVbiLinesEnableProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_LINE ) { //301

		p_control->value = pVideo->m_pDevice->m_nCustomAnalogVideoVbiLinesProperty;

		return 0;
	}


	return -EINVAL;
}

static int common_video_device_vidioc_s_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	ULONG i = 0;

	ULONG j = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_ctrl( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_control->id - V4L2_CID_BASE );

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) { i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) ) { j = pVideo->m_nInput % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) { j = (pVideo->m_nSubDeviceNumber) % 4; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { j = 0; }
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) { j = 0; }


	if( p_control->id == V4L2_CID_BRIGHTNESS ) { //0

		g_n_analog_decoder_brightness[ i ][ j ] = p_control->value | 0x80000000;

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { CX2581_SetAnalogVideoDecoderProcAmpProperties( pVideo->m_pDevice, pVideo ); } // [2010.06.02] [HUENGPEI]
//peter [2011.07.11]
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { TW5864_SetAnalogVideoDecoderRegister( pVideo->m_pDevice, (0x0000000A + ((pVideo->m_nSubDeviceNumber) % 4) * 0x10) << (2), (g_n_analog_decoder_brightness[ i ][ j ] & 0xFF) -128 ); } // BRIGHTNESS 

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { SA7160_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { FH8735_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }
		
		return 0;
	}
	if( p_control->id == V4L2_CID_CONTRAST ) { //1

		g_n_analog_decoder_contrast[ i ][ j ] = p_control->value | 0x80000000;

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { CX2581_SetAnalogVideoDecoderProcAmpProperties( pVideo->m_pDevice, pVideo ); } // [2010.06.02] [HUENGPEI]
//peter [2011.07.11]
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { TW5864_SetAnalogVideoDecoderRegister( pVideo->m_pDevice, (0x00000009 + ((pVideo->m_nSubDeviceNumber) % 4) * 0x10) << (2), g_n_analog_decoder_contrast[ i ][ j ] & 0xFF ); }  

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { SA7160_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { FH8735_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		return 0;
	}
	if( p_control->id == V4L2_CID_SATURATION ) { //2

		g_n_analog_decoder_saturation[ i ][ j ] = p_control->value | 0x80000000;

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { CX2581_SetAnalogVideoDecoderProcAmpProperties( pVideo->m_pDevice, pVideo ); } // [2010.06.02] [HUENGPEI]
//peter [2011.07.11]
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { TW5864_SetAnalogVideoDecoderRegister( pVideo->m_pDevice, (0x0000000B + ((pVideo->m_nSubDeviceNumber) % 4) * 0x10) << (2), g_n_analog_decoder_saturation[ i ][ j ] & 0xFF ); }  
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { TW5864_SetAnalogVideoDecoderRegister( pVideo->m_pDevice, (0x0000000C + ((pVideo->m_nSubDeviceNumber) % 4) * 0x10) << (2), g_n_analog_decoder_saturation[ i ][ j ] & 0xFF ); }  

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { SA7160_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { FH8735_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		return 0;
	}
	if( p_control->id == V4L2_CID_HUE ) { //3

		g_n_analog_decoder_hue[ i ][ j ] = p_control->value | 0x80000000;

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { CX2581_SetAnalogVideoDecoderProcAmpProperties( pVideo->m_pDevice, pVideo ); } // [2010.06.02] [HUENGPEI]
//peter [2011.07.11]
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { TW5864_SetAnalogVideoDecoderRegister( pVideo->m_pDevice, (0x00000007 + ((pVideo->m_nSubDeviceNumber) % 4) * 0x10)  << (2), (g_n_analog_decoder_hue[ i ][ j ] & 0xFF) -128 ); }  

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { SA7160_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { FH8735_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		return 0;
	}
	if( p_control->id == V4L2_CID_SHARPNESS ) { //27

		g_n_analog_decoder_sharpness[ i ][ j ] = p_control->value | 0x80000000;

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) { CX2581_SetAnalogVideoDecoderProcAmpProperties( pVideo->m_pDevice, pVideo ); } // [2010.06.02] [HUENGPEI]
//peter [2011.07.11]
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) { TW5864_SetAnalogVideoDecoderRegister( pVideo->m_pDevice, (0x00000008 + ((pVideo->m_nSubDeviceNumber) % 4) * 0x10) << (2), ((g_n_analog_decoder_sharpness[ i ][ j ] & 0xFF) >> 4) | 0x10 ); }  

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { SA7160_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { FH8735_SetAnalogVideoProcAmpProperties( pVideo->m_pDevice ); }

		return 0;
	}
	if( p_control->id == V4L2_CID_AUTOGAIN ) { //18

		g_n_analog_decoder_gain_auto[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_GAIN ) { //19

		g_n_analog_decoder_gain[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_SPEED ) { //200

		pVideo->m_nSwitchSpeed_Spliter = p_control->value;

		pVideo->m_nDropDecumulatorCounter_Spliter = 3;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_TABLE ) { //201

		BYTE SWITCH_CHANNEL_TABLE[ 13 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

		for( i = 0 ; i < 12 ; i++ ) {

			SWITCH_CHANNEL_TABLE[ i ] = (BYTE)(((p_control->value) >> (i * 2)) & 0x00000003);

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x00 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x01; }

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x01 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x02; }

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x02 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x04; }

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x03 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x08; }
		}
		memcpy( pVideo->m_pSwitchTable_Spliter, SWITCH_CHANNEL_TABLE, 12 );

		pVideo->m_nLockStatusMask_Spliter = SWITCH_CHANNEL_TABLE[ 12 ];

		pVideo->m_nDropDecumulatorCounter_Spliter = 3;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD ) { //251

		g_n_multiplexer_motion_sensitivity_low_threshold[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD ) { //252

		g_n_multiplexer_motion_sensitivity_high_threshold[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CX ) { //253

		g_n_multiplexer_motion_mask_cx[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CY ) { //254

		g_n_multiplexer_motion_mask_cy[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_X ) { //260

		g_n_encoder_osd_pos_x[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_Y ) { //261

		g_n_encoder_osd_pos_y[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_COLOR ) { //262

		g_n_encoder_osd_color[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DIRECTION ) { //270

		g_n_gpio_direction[ i ] = p_control->value;

		if(	(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) {

			pVideo->m_pDevice->m_nCustomGpioDirectionProperty = g_n_gpio_direction[ i ] & 0xFF;
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ) {

			TW6805_SetRegister( pVideo->m_pDevice, 0x00000028, g_n_gpio_direction[ i ], 4 );

		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) {

			ULONG R00110018 = 0x00000000;

			ULONG R0011001C = 0x00000000;

//			R00110018 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110018, 4 ); R00110018 &= 0x000000FF; R00110018 |= (((~g_n_gpio_direction[ i ]) & 0x00FFFFFF) << ( 8));
			R00110018 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110018, 4 ); R00110018 &= 0x000000FF; R00110018 |= (((~g_n_gpio_direction[ i ]) & 0x000000FF) << ( 8));

			CX2581_SetRegister( pVideo->m_pDevice, 0x00110018, R00110018, 4 );

//			R0011001C = CX2581_GetRegister( pVideo->m_pDevice, 0x0011001C, 4 ); R0011001C &= 0xFFFFFF00; R0011001C |= (((~g_n_gpio_direction[ i ]) & 0xFF000000) >> (24));

//			CX2581_SetRegister( pVideo->m_pDevice, 0x0011001C, R0011001C, 4 );
		}
		// shawn 2010.07.12
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) {

//			ULONG R00110018 = 0x00000000;
//
//			ULONG R0011001C = 0x00000000;
//
//			R00110018 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110018, 4 ); R00110018 &= 0x000000FF; R00110018 |= (((~g_n_gpio_direction[ i ]) & 0x00FFFFFF) << ( 8));
//
//			CX2581_SetRegister( pVideo->m_pDevice, 0x00110018, R00110018, 4 );
//
//			R0011001C = CX2581_GetRegister( pVideo->m_pDevice, 0x0011001C, 4 ); R0011001C &= 0xFFFFFF00; R0011001C |= (((~g_n_gpio_direction[ i ]) & 0xFF000000) >> (24));
//
//			CX2581_SetRegister( pVideo->m_pDevice, 0x0011001C, R0011001C, 4 );
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) {

			ULONG R007D3400 = 0x00000000;
			
			AH8400_GetRegister( pVideo->m_pDevice, 3, 0x007D3000 + 0x00000400, 1 ); R007D3400 &= 0xFFFFFF00;

			R007D3400 |=  g_n_gpio_direction[ i ] & 0x000000FF;

			AH8400_SetRegister( pVideo->m_pDevice, 3, 0x007D3000 + 0x00000400, R007D3400, 1 );
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) {
		
			//direction will not be remember by register
			
			g_n_gpio_direction[ pVideo->m_pDevice->m_nKsDeviceNumber ] = (p_control->value) & 0xF;

			TW5864_SYS_CFG * p_sys_cfg = (TW5864_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

			p_sys_cfg->R00009801 &= 0x0F00;
				
			p_sys_cfg->R00009801 |=	((p_control->value & 0x0F) << 12) | 0x0F00;

			ULONG R00009800 = TW5864_GetRegister( pVideo->m_pDevice, 0x9800 ) & 0x00FF;

			R00009800 |= p_sys_cfg->R00009801;

			TW5864_SetRegister( pVideo->m_pDevice, 0x9800, R00009800 );

		}

		//
		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DATA ) { //271

		g_n_gpio_data[ i ] = p_control->value;

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) {
/*
			TW6802_CLOSE_THREAD();

			BYTE R = (BYTE)(pDevice->m_nCustomGpioDirectionProperty & 0xFF); 

			R = ~R; // INPUT PIN IS ALWAYS HIGH

			R |= (BYTE)(*pPropertyValue & 0xFF);

			I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0x40, 125 ); I2C_WRITE_BYTE( pDevice, R, 125 ); I2C_STOP( pDevice, 125 );

			DELAY_100NS( 1250000 );

			TW6802_START_THREAD();
*/

			BYTE R = (BYTE)(pVideo->m_pDevice->m_nCustomGpioDirectionProperty & 0xFF); 

			R = ~R; // INPUT PIN IS ALWAYS HIGH

			R |= (BYTE)(g_n_gpio_data[ i ] & 0xFF);

			TW6816_SET_GPIO_DATA( pVideo->m_pDevice, R );
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) )
		{
			TW6805_SetRegister( pVideo->m_pDevice, 0x00000100, g_n_gpio_data[ i ], 4 );
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ) {

			ULONG R00110010 = 0x00000000;

			ULONG R00110014 = 0x00000000;

//			R00110010 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110010, 4 ); R00110010 &= 0x000000FF; R00110010 |= ((g_n_gpio_data[ i ] & 0x00FFFFFF) << ( 8));
			R00110010 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110010, 4 ); R00110010 &= 0x000000FF; R00110010 |= ((g_n_gpio_data[ i ] & 0x000000FF) << ( 8));

			CX2581_SetRegister( pVideo->m_pDevice, 0x00110010, R00110010, 4 );

//			R00110014 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110014, 4 ); R00110014 &= 0xFFFFFF00; R00110014 |= ((g_n_gpio_data[ i ] & 0xFF000000) >> (24));

//			CX2581_SetRegister( pVideo->m_pDevice, 0x00110014, R00110014, 4 );
		}
		// shawn 2010.07.12
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) {

//			ULONG R00110010 = 0x00000000;
//
//			ULONG R00110014 = 0x00000000;
//
//			R00110010 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110010, 4 ); R00110010 &= 0x000000FF; R00110010 |= ((g_n_gpio_data[ i ] & 0x00FFFFFF) << ( 8));
//
//			CX2581_SetRegister( pVideo->m_pDevice, 0x00110010, R00110010, 4 );
//
//			R00110014 = CX2581_GetRegister( pVideo->m_pDevice, 0x00110014, 4 ); R00110014 &= 0xFFFFFF00; R00110014 |= ((g_n_gpio_data[ i ] & 0xFF000000) >> (24));
//
//			CX2581_SetRegister( pVideo->m_pDevice, 0x00110014, R00110014, 4 );
		}
		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ) {

			ULONG R007D33FC = 0x00000000;

			R007D33FC = AH8400_GetRegister( pVideo->m_pDevice, 3, 0x007D3000 + 0x000003FC, 1 ); R007D33FC &= 0xFFFFFF00;

			R007D33FC |= g_n_gpio_data[ i ] & 0x000000FF;

			AH8400_SetRegister( pVideo->m_pDevice, 3, 0x007D3000 + 0x000003FC, R007D33FC, 1 );
		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ) {
/*
			TW5864_StopControlPanelAnalysisThread_EX( pVideo->m_pDevice );
			
			ULONG R00009800 = 0x00000000;

			R00009800 = TW5864_GetRegister( pVideo->m_pDevice, 0x00009800 );	

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_ctrl() read R00009800( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), R00009800 );
			
			R00009800 &= 0xFFFFFF0F;

			R00009800 |= (g_n_gpio_data[ i ] & 0x0000000F) << 4;

			R00009800 |= g_n_gpio_direction[ pVideo->m_pDevice->m_nKsDeviceNumber ] << 12;

			TW5864_SetRegister( pVideo->m_pDevice, 0x9800, R00009800);

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_ctrl() data R00009800( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), R00009800 );
			
			TW5864_StartControlPanelAnalysisThread_EX( pVideo->m_pDevice );
*/
			g_n_gpio_data[ pVideo->m_pDevice->m_nKsDeviceNumber ] = (p_control->value) & 0xF;
			
			TW5864_SYS_CFG * p_sys_cfg = (TW5864_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

			ULONG R00009800 = TW5864_GetRegister( pVideo->m_pDevice, 0x9800 ) & 0x000F;

			R00009800 |= p_sys_cfg->R00009801;

			R00009800 |= (p_control->value & 0x0F) << 4;

			TW5864_SetRegister( pVideo->m_pDevice, 0x9800, R00009800 );

		}

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) 
		{
			FH8735_StopControlPanelAnalysisThreadEX( pVideo->m_pDevice );

			ULONG R = g_n_gpio_direction[ i ] & 0xFFFF;
			
			R = ~R; // INPUT PIN IS ALWAYS HIGH

			R |= g_n_gpio_data[ i ] & 0xFFFF;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_ctrl() data R( %08X ) direct\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), R );

			FH8735_SetGpioRegister( pVideo->m_pDevice, 0x00, 0x40, (BYTE)(R >> 0) );

			FH8735_SetGpioRegister( pVideo->m_pDevice, 0x00, 0x42, (BYTE)(R >> 8) );

			//DELAY_100NS( 1250000 );

			FH8735_StartControlPanelAnalysisThreadEX( pVideo->m_pDevice );
		}
		//
		return 0;
	}
	if( p_control->id == V4L2_CID_PCM_UPDATE ) { //272

		ULONG i = 0;
	
//		for( i = 0; i < 16; i++ )
//		{
//			pVideo->m_pDevice->m_pcm_update[ i ] = p_control->value & 0x1;
//		
//			LINUXV4L2_PRINT( KERN_INFO, " m_pcm_update[%d] (%d)\n", i, pVideo->m_pDevice->m_pcm_update[ i ] );
//		}

		return 0;
	}

	//shawn 2010.10.25
	if( p_control->id == V4L2_CID_INPUT_SELECT ) { //210

//common_video_device_vidioc_s_input


		if( ( p_control->value % 7) != pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty )
		{
			pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty = ( p_control->value % 7);

			pVideo->m_nInput = ( p_control->value % 7);

			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
			{
				SA7160_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice);
			}
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
			{
				FH8735_SetAnalogCrossbarRouteProperty(pVideo->m_pDevice);
			}

		}

		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_TEMPORAL_SENSITIVITY ) { //290

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

				pVideo->m_pDevice->m_nCustomMotionTemporalSensitivityProperty[ pVideo->m_nSubDeviceNumber ] = (p_control->value) | 0x08000000;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPATIAL_SENSITIVITY ) { //291

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

				pVideo->m_pDevice->m_nCustomMotionSpatialSensitivityProperty[ pVideo->m_nSubDeviceNumber ] = (p_control->value) | 0x08000000;

	}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_LEVEL_SENSITIVITY ) { //292

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

				pVideo->m_pDevice->m_nCustomMotionLevelSensitivityProperty[ pVideo->m_nSubDeviceNumber ] = (p_control->value) | 0x08000000;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPEED ) { //293

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
		{

				pVideo->m_pDevice->m_nCustomMotionSpeedProperty[ pVideo->m_nSubDeviceNumber ] = (p_control->value) | 0x08000000;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_FRAME_RATE ) { //208

		if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
			(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ) {

				pVideo->m_pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_control->value) & 0xFF;

				OnSetCustomAnalogVideoFrameRateProperty( pVideo->m_pDevice );
		}
		
		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			pVideo->m_pDevice->m_Preview_frame_rate_setting = (p_control->value) & 0xFFFF;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG_PERIOD ) { //206

		pVideo->m_pDevice->m_nCustomSoftwareWatchdogResetProperty = 0x80000001;

		if( (pVideo->m_pDevice->m_nCustomSoftwareWatchdogDurationProperty) != ((p_control->value) & 0xFF) )
		{
			pVideo->m_pDevice->m_nCustomSoftwareWatchdogDurationProperty = ((p_control->value) & 0xFF) | 0x80000000;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG ) { //207
		
		pVideo->m_pDevice->m_nCustomSoftwareWatchdogResetProperty = 0x80000001;

		return 0;
	}




	if( p_control->id == V4L2_CID_WATCH_DOG_ENABLE ) { //212
		
		pVideo->m_pDevice->m_nCustomSoftwareWatchdogEnableProperty = 0x80000001;

		return 0;
	}

	if( p_control->id == V4L2_CID_NO_SIGNAL_BLACK_PATTERN ) { //213
		
		pVideo->m_pDevice->m_No_Signal_black_pattern_property = 0x00000001;

		return 0;
	}

	if( p_control->id == V4L2_CID_SPECIAL_CAMERA ) { //214
		
		pVideo->m_pDevice->m_nCustomSpecialCameraInputProperty = 0x00000001;

		return 0;
	}

	if( p_control->id == V4L2_CID_HDCP_ENABLE ) { //202

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			pVideo->m_pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = (p_control->value) & 0xFF;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_HV_SCALE_FACTOR ) { //204

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			pVideo->m_pDevice->m_nHVScaleFactor = p_control->value & 0x000F000F; 
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_H264_HV_SCALE_FACTOR ) { //205

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			pVideo->m_pDevice->m_nH264HVScaleFactor = (p_control->value & 0x03030303) | 0x88000000; 
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_ENC_DEINTERLACE ) { //298

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			pVideo->m_pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ] = (p_control->value) | 0x88000000; 
		}

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 )
		{
			pVideo->m_pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ] = (p_control->value) | 0x88000000; 
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_RESET_I2S ) { //299

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			FH8735_SendVendorCommand( pVideo->m_pDevice, 0x00000023, 0x00000000 );
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_ENABLE ) { //300

		pVideo->m_pDevice->m_nCustomAnalogVideoVbiLinesEnableProperty = (p_control->value ? TRUE : 0);

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_VBI_ENABLE(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_pDevice->m_nCustomAnalogVideoVbiLinesEnableProperty );

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			if( pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty == 4 )
			{
				SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

				p_sys_cfg->n_input_video_resolution_fps = 0;//reset fps to make reset m_nCustomAnalogVideoVbiLinesEnableProperty
			}
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_LINE ) { //301

		pVideo->m_pDevice->m_nCustomAnalogVideoVbiLinesProperty = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_VBI_LINE(%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), pVideo->m_pDevice->m_nCustomAnalogVideoVbiLinesProperty );

		if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 )
		{
			if( pVideo->m_pDevice->m_nAnalogCrossbarVideoInputProperty == 4 )
			{
				SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

				p_sys_cfg->n_input_video_resolution_fps = 0;//reset fps to make reset m_nCustomAnalogVideoVbiLinesProperty
			}
		}
		return 0;
	}


	return -EINVAL;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

static int common_video_device_vidioc_g_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	struct v4l2_ext_control * p_ext_control = p_ext_controls->controls;

	int count = p_ext_controls->count;

	int size = sizeof(struct v4l2_ext_control);

	int i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;
	
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_ext_ctrls()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	if( p_ext_controls->which == V4L2_CTRL_CLASS_MPEG ) {

		if( count > 6 ) return 0;

		while( count ) {

			switch( p_ext_control->id ) {

			case V4L2_CID_MPEG_VIDEO_BITRATE_MODE: p_ext_control->value = (pVideo->m_nEncoderMode == V4L2_BITRATE_VBR) ? 0 : 1; break;
			
			case V4L2_CID_MPEG_VIDEO_BITRATE:      p_ext_control->value = pVideo->m_nEncoderBitRate; break;
			
			case V4L2_CID_MPEG_VIDEO_QUALITY:      p_ext_control->value = pVideo->m_nEncoderQuality; break;
			
			case V4L2_CID_MPEG_VIDEO_GOP_SIZE:     p_ext_control->value = pVideo->m_nEncoderGop; break;
			
			case V4L2_CID_MPEG_VIDEO_FRAMERATE:    p_ext_control->value = pVideo->m_nFrameRate; break;
			
			case V4L2_CID_MPEG_VIDEO_QPSTEP:       p_ext_control->value = pVideo->m_nEncoderQPStep; break;
			
			case V4L2_CID_MPEG_VIDEO_OSD_ENABLE:	p_ext_control->value = pVideo->m_pDevice->m_bCustomOsdTextEnableProperty;	break;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
			
			case V4L2_CID_MPEG_VIDEO_OSD:		   //memcpy( p_ext_control->string, g_p_encoder_osd_text[ i ], 32 ); break;

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) )
													{
														if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
														{
															memcpy( p_ext_control->string, pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], 64 );

														}
														else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) || (pVideo->m_pDevice->iProduct & 0x0F) == (0x07) )
														{
															memcpy( p_ext_control->string, pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ 0 ][ 0 ], 64 );
														}
														else
														{
															memcpy( p_ext_control->string, pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], 64 );
														}

													}
													else
													{
														memcpy( p_ext_control->string, g_p_encoder_osd_text[ i ], 32 );
													}
													break;

			#else
			
			case V4L2_CID_MPEG_VIDEO_OSD:		   //memcpy( p_ext_control->reserved, g_p_encoder_osd_text[ i ], 32 ); break;

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) )
													{
														if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
														{
															memcpy( p_ext_control->reserved, pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], 64 );

														}
														else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) || (pVideo->m_pDevice->iProduct & 0x0F) == (0x07) )
														{
															memcpy( p_ext_control->reserved, pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ 0 ][ 0 ], 64 );
														}
														else
														{
															memcpy( p_ext_control->reserved, pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], 64 );
														}

													}
													else
													{
														memcpy( p_ext_control->reserved, g_p_encoder_osd_text[ i ], 32 );
													}
													break;


			#endif

			case V4L2_CID_MPEG_VIDEO_H264_PROFILE:	p_ext_control->value = (pVideo->m_pDevice->m_nCustomEncoderVideoProfileProperty[ pVideo->m_nSubDeviceNumber ]) & 0xFF;	break;

			case V4L2_CID_MPEG_VIDEO_H264_LEVEL:	p_ext_control->value = (pVideo->m_pDevice->m_nCustom_H264_Level_Property) & 0xFF;	break;

			case V4L2_CID_MPEG_VIDEO_ASPECT:		p_ext_control->value = (pVideo->m_pDevice->m_nCustomEncoderVideoAspectRatioProperty[ pVideo->m_nSubDeviceNumber ]) & 0x77FFFFFF; break;

			default:  return -EINVAL;
			}
			p_ext_control = p_ext_control + size;

			count--;
		}
	}
	return 0;
}

static int common_video_device_vidioc_s_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	struct v4l2_ext_control * p_ext_control = p_ext_controls->controls;

	int count = p_ext_controls->count;

	int size = sizeof(struct v4l2_ext_control);

	int i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_ext_ctrls()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	int index = 0;

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {

		if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
		{
			index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11
		}
		else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) )
		{
			index = ((pVideo->m_nSubDeviceNumber < 1) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 7);// subdevice 0 use array 0, subdevice 1 use array 8
		}
		else
		{
			index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);// subdevice 0~3 use array 0~3, subdevice 4~7 use array 8~11
		}
	}

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {

		ULONG counts = 1;
		
		if( ((pVideo->m_pDevice->iProduct & 0x0F) == 0x02) || ((pVideo->m_pDevice->iProduct & 0x0F) == 0x03)  ) {

			if( (pVideo->m_pDevice->iManufacturer & 0x0F) == 0x0A ) 
			{
				counts = 8;
			}
			else
			{
				counts = 4;
			}
		}
		
		index = pVideo->m_nSubDeviceNumber;
	}


	if( p_ext_controls->which == V4L2_CTRL_CLASS_MPEG ) {

		if( count > 7 ) return 0;

		while( count ) {

			switch(p_ext_control->id) {

			case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:	pVideo->m_nEncoderMode    = (p_ext_control->value ? V4L2_BITRATE_CBR : V4L2_BITRATE_VBR); 
				
													if(pVideo->m_nEncoderMode == V4L2_BITRATE_VBR )
													{
														if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
														{
															if( pVideo->m_nSubDeviceType == 2 )
															{
																CBR_to_VBR( pVideo, pVideo->m_nSubDeviceNumber );//0~7
															}
														}
													}

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoBitRateModeProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderMode;
														}
													}
													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoBitRateModeProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderMode;
														}
													}


													break;

			case V4L2_CID_MPEG_VIDEO_BITRATE:		pVideo->m_nEncoderBitRate = p_ext_control->value; 

												
													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoBitRateProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderBitRate;
														}
													}

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoBitRateProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderBitRate;
														}
													}
													break;


			case V4L2_CID_MPEG_VIDEO_QUALITY:		pVideo->m_nEncoderQuality = p_ext_control->value; 

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nAnalogVideoCompressionQualityProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderQuality;
														}
													}

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nAnalogVideoCompressionQualityProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderQuality;
														}
													}
				
													break;

			case V4L2_CID_MPEG_VIDEO_GOP_SIZE:		pVideo->m_nEncoderGop     = p_ext_control->value; 

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderGop;
														}
													}
				
													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {
														//only for encoder
														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderGop;
														}
													}
				
													break;

			case V4L2_CID_MPEG_VIDEO_FRAMERATE:		pVideo->m_nFrameRate      = p_ext_control->value; 


													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {

														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															
															int skip = 0;

															switch(pVideo->m_nFrameRate)
															{
																case 50000:
																case 60000:
																	skip = 0;
																	break;

																case 25000:
																case 30000:
																	skip = 0;
																	break;

																case 12500:
																case 15000:
																	skip = 1;
																	break;

																case 6250:
																case 7500:
																	skip = 3;
																	break;

																case 5000:
																case 6000:
																	skip = 4;
																	break;

																case 2500:
																case 3000:
																	skip = 9;
																	break;
															
																default:
																	skip = 0;
																	break;
															};
															
															pVideo->m_pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ index ]  =   0x88000000 | skip;
														}

													}

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ) {

														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ index ]  =   0x88000000 | (pVideo->m_nFrameRate/1000);
														}
													}
													
													break;

			case V4L2_CID_MPEG_VIDEO_QPSTEP:		pVideo->m_nEncoderQPStep  = p_ext_control->value; 

													
													break;

			case V4L2_CID_MPEG_SET_KEY_FRAME:		
													if(pVideo->m_nSubDeviceType == 0x00000002)
													{													
														if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 )
														{
															pVideo->m_pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ index ]  =   0x1;
														}
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_LINE:		
													if(pVideo->m_nSubDeviceType == 0x00000002)
													{													
														pVideo->m_pDevice->m_nCustomOsdTextLineProperty[ pVideo->m_nSubDeviceNumber ] = (p_ext_control->value) % 8; 
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_ENABLE:		
													{													
														pVideo->m_pDevice->m_bCustomOsdTextEnableProperty = (p_ext_control->value) & 0x1;
													}

													break;


			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)

			case V4L2_CID_MPEG_VIDEO_OSD:			
													
													if(pVideo->m_nSubDeviceType == 0x00000002)
													{													
														if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ pVideo->m_nSubDeviceNumber ][ pVideo->m_pDevice->m_nCustomOsdTextLineProperty[ pVideo->m_nSubDeviceNumber ] ], p_ext_control->string, 32 );

															pVideo->m_pDevice->m_bCustomOsdTextStringRefreshProperty[ pVideo->m_nSubDeviceNumber ][ pVideo->m_pDevice->m_nCustomOsdTextLineProperty[ pVideo->m_nSubDeviceNumber ] ] = TRUE; 
														}
														else
														{
															memcpy( g_p_encoder_osd_text[ i ], p_ext_control->string, 64 );
														}
													}

													if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) )
													{
														if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], p_ext_control->string, 64 );
														}
														else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) || (pVideo->m_pDevice->iProduct & 0x0F) == (0x07) )
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ 0 ][ 0 ], p_ext_control->string, 64 );
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ 4 ][ 0 ], p_ext_control->string, 64 );
														}
														else
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], p_ext_control->string, 64 );
														}

													}
													break;

			#else
			
			case V4L2_CID_MPEG_VIDEO_OSD:			
													if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 )
													{
														memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ pVideo->m_nSubDeviceNumber ][ pVideo->m_pDevice->m_nCustomOsdTextLineProperty[ pVideo->m_nSubDeviceNumber ] ], p_ext_control->reserved, 32 );

														pVideo->m_pDevice->m_bCustomOsdTextStringRefreshProperty[ pVideo->m_nSubDeviceNumber ][ pVideo->m_pDevice->m_nCustomOsdTextLineProperty[ pVideo->m_nSubDeviceNumber ] ] = TRUE; 
													}
													else if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) )
													{
														if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x03) ||	(pVideo->m_pDevice->iManufacturer == 0x85) )
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], p_ext_control->reserved, 64 );
														}
														else if( (pVideo->m_pDevice->iProduct & 0x0F) == (0x05) || (pVideo->m_pDevice->iProduct & 0x0F) == (0x07) )
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ 0 ][ 0 ], p_ext_control->reserved, 64 );
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ 4 ][ 0 ], p_ext_control->reserved, 64 );
														}
														else
														{
															memcpy( pVideo->m_pDevice->m_pCustomOsdTextStringProperty[ (pVideo->m_nSubDeviceNumber % 4)  ][ 0 ], p_ext_control->reserved, 64 );
														}
													}
													else
													{
														memcpy( g_p_encoder_osd_text[ i ], p_ext_control->reserved, 64 );
													}

													break;


			#endif
			
			case V4L2_CID_MPEG_VIDEO_H264_PROFILE:		
													{													
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoProfileProperty[ pVideo->m_nSubDeviceNumber ] = 0x88000000 | ((p_ext_control->value) & 0xFF); 
														}
													}

													break;

			case V4L2_CID_MPEG_VIDEO_H264_LEVEL:		
													{													
														pVideo->m_pDevice->m_nCustom_H264_Level_Property = 0x88000000 | ((p_ext_control->value) & 0xFF); 
													}

													break;

			case V4L2_CID_MPEG_VIDEO_ASPECT:		
													{													
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_pDevice->m_nCustomEncoderVideoAspectRatioProperty[ pVideo->m_nSubDeviceNumber ] = 0x88000000 | (p_ext_control->value); 
														}
													}

													break;


			default: break;
			}
			p_ext_control = p_ext_control + size;

			count--;
		}
	}
	
	return 0;
}
static int common_video_device_vidioc_try_ext_ctrls( struct file *p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls)
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	struct v4l2_ext_control * p_ext_control = p_ext_controls->controls;

	int count = p_ext_controls->count;

	int size = sizeof(struct v4l2_ext_control);

	int i = pVideo->m_pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_try_ext_ctrls()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

}

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)

static int common_video_device_vidioc_g_mpegcomp( struct file * p_file, void * p_priv, struct v4l2_mpeg_compression * p_mpeg_compression )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_g_mpegcomp()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	if( pVideo->m_nEncoderMode == V4L2_BITRATE_CBR ) {

		//
		//
		//
		//
		p_mpeg_compression->vi_bitrate.mode = V4L2_BITRATE_CBR;            //

		p_mpeg_compression->vi_bitrate.target = pVideo->m_nEncoderBitRate; //

		p_mpeg_compression->vi_frames_per_gop = pVideo->m_nEncoderGop;     //

		p_mpeg_compression->vi_frame_rate = pVideo->m_nFrameRate;          //
		
																           //

		p_mpeg_compression->reserved_5[ 0 ] = pVideo->m_nEncoderQPStep;    //

		return 0;
	}
	if( pVideo->m_nEncoderMode == V4L2_BITRATE_VBR ) {

		//
		//
		//B DEFAULT: MODE = VBR / QUALITY = 6000 / GOP = 25 / QPSTEP = 1 / FPS = 25000 (25.000 FPS)
		//
		p_mpeg_compression->vi_bitrate.mode = V4L2_BITRATE_VBR;            //

		p_mpeg_compression->vi_bitrate.target = pVideo->m_nEncoderQuality; // 

		p_mpeg_compression->vi_frames_per_gop = pVideo->m_nEncoderGop;     //

		p_mpeg_compression->vi_frame_rate = pVideo->m_nFrameRate;          //

																           //

		p_mpeg_compression->reserved_5[ 0 ] = pVideo->m_nEncoderQPStep;    //

		return 0;
	}
	return -EINVAL;
}

static int common_video_device_vidioc_s_mpegcomp( struct file * p_file, void * p_priv, struct v4l2_mpeg_compression * p_mpeg_compression )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_mpegcomp()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	if( p_mpeg_compression->vi_bitrate.mode == V4L2_BITRATE_CBR ) {

		pVideo->m_nEncoderMode = p_mpeg_compression->vi_bitrate.mode;      //

		pVideo->m_nEncoderBitRate = p_mpeg_compression->vi_bitrate.target; //

		pVideo->m_nEncoderGop = p_mpeg_compression->vi_frames_per_gop;     //

		pVideo->m_nEncoderQPStep = 	p_mpeg_compression->reserved_5[ 0 ];   //

		pVideo->m_nFrameRate = p_mpeg_compression->vi_frame_rate;          //
		
																           //
		return 0;
	}
	if( p_mpeg_compression->vi_bitrate.mode == V4L2_BITRATE_VBR ) {

		pVideo->m_nEncoderMode = p_mpeg_compression->vi_bitrate.mode;      //

		pVideo->m_nEncoderQuality = p_mpeg_compression->vi_bitrate.target; // 

		pVideo->m_nEncoderGop = p_mpeg_compression->vi_frames_per_gop;     //

		pVideo->m_nEncoderQPStep = 	p_mpeg_compression->reserved_5[ 0 ];   //

		pVideo->m_nFrameRate = p_mpeg_compression->vi_frame_rate;          //

																           //
		return 0;
	}
	return -EINVAL;
}

#endif

static int common_video_device_vidioc_encoder_cmd( struct file * p_file, void * p_priv, struct v4l2_encoder_cmd * p_encoder_cmd )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_encoder_cmd( %08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_encoder_cmd->cmd );

//	enum v4l2_bitrate_mode { V4L2_BITRATE_NONE = 0, V4L2_BITRATE_CBR = 1, V4L2_BITRATE_VBR = 2 };

	if( p_encoder_cmd->cmd == 0x00000004 ) { //

		p_encoder_cmd->raw.data[ 0 ] = pVideo->m_nEncoderMode;    //

		p_encoder_cmd->raw.data[ 1 ] = pVideo->m_nEncoderBitRate; //

		p_encoder_cmd->raw.data[ 2 ] = pVideo->m_nEncoderQuality; // 

		p_encoder_cmd->raw.data[ 3 ] = pVideo->m_nEncoderGop;     //

		p_encoder_cmd->raw.data[ 4 ] = pVideo->m_nEncoderQPStep;  //

		p_encoder_cmd->raw.data[ 5 ] = pVideo->m_nFrameRate;      //

															      //
		return 0;
	}
	if( p_encoder_cmd->cmd == 0x00000005 ) { //

		pVideo->m_nEncoderMode    = p_encoder_cmd->raw.data[ 0 ]; //

		pVideo->m_nEncoderBitRate = p_encoder_cmd->raw.data[ 1 ]; //

		pVideo->m_nEncoderQuality = p_encoder_cmd->raw.data[ 2 ]; // 

		pVideo->m_nEncoderGop     = p_encoder_cmd->raw.data[ 3 ]; //

		pVideo->m_nEncoderQPStep  = p_encoder_cmd->raw.data[ 4 ]; //

		pVideo->m_nFrameRate      = p_encoder_cmd->raw.data[ 5 ]; //

																  //
		return 0;
	}
	return -EINVAL;
}

static int common_video_device_vidioc_reqbufs( struct file * p_file, void * p_priv, struct v4l2_requestbuffers * p_requestbuffers )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_reqbufs()\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber) );

	if( pVideo->m_video_preview_streaming == 1 )
	{
		return videobuf_reqbufs( &(pVideo->m_sVideoBufQueue), p_requestbuffers );
	}
	else
	{
		return -EINVAL;
	}
}

static int common_video_device_vidioc_querybuf( struct file * p_file, void * p_priv, struct v4l2_buffer * p_buffer )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querybuf( I:%d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_buffer->index );

	if( pVideo->m_video_preview_streaming == 1 )
	{
		return videobuf_querybuf( &(pVideo->m_sVideoBufQueue), p_buffer );
	}
	else
	{
		return -EINVAL;
	}
}
#ifdef AME_MZ0380
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0)
		#define V4L2_BUF_FLAG_INPUT     0x0200
	#endif
#endif


static int common_video_device_vidioc_qbuf( struct file * p_file, void * p_priv, struct v4l2_buffer * p_buffer )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_qbuf( I:%d ) (0d%d) (0d%d) flags(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_buffer->index , pVideo->m_sVideoBufQueue.inputs, p_buffer->input, p_buffer->flags);

#ifdef AME_MZ0380
	//must set this otherwise H264 will have error
	p_buffer->flags &= ~V4L2_BUF_FLAG_INPUT;
#endif
/*
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_qbuf() type(0d%d) memory(0d%d) index(0x%x) m.userptr(llx%llx) length(0d%d)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), 
		
	p_buffer->type,
	p_buffer->memory, 
	p_buffer->index,
	p_buffer->m.userptr,
	p_buffer->length 
	);
*/
	if( pVideo->m_video_preview_streaming == 1 )
	{
		return videobuf_qbuf( &(pVideo->m_sVideoBufQueue), p_buffer );
	}
	else
	{
		return -EINVAL;
	}
}

static int common_video_device_vidioc_dqbuf( struct file * p_file, void * p_priv, struct v4l2_buffer * p_buffer )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_dqbuf( I:%d, I:%08X ) state(0x%x)\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), p_buffer->index, p_buffer->state );
	if( pVideo->m_video_preview_streaming == 1 )
	{
		return videobuf_dqbuf( &(pVideo->m_sVideoBufQueue), p_buffer, p_file->f_flags & O_NONBLOCK );
	}
	else
	{
		return 0;
	}
}

static int common_video_device_vidioc_streamon( struct file * p_file, void * p_priv, enum v4l2_buf_type e_buf_type )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	unsigned long flags = 0x00000000;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_streamon( T:%02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), e_buf_type );

	if( pVideo->m_video_preview_streaming == 0 )
	{
		return 0;
	}

	INIT_LIST_HEAD( &(pVideo->m_sKsBufferList) );

	INIT_LIST_HEAD( &(pVideo->m_sKsBufferListList) );

//	spin_lock_irqsave( &pVideo->m_pDevice->m_sTaskletAccessSpinLock, flags );

	down( &(pVideo->m_pDevice->m_sComponentAccessSemaphore) ); // [2011.07.01] [HUENGPEI]

	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210 ) { if( FALSE == CX2581_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851 ) { if( FALSE == CX2588_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { if( FALSE == SA7160_HwAnalogComponentsInitialize( pVideo->m_pDevice, NULL, FALSE ) ) { ; } }

	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { if( FALSE == AH8400_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { if( FALSE == TW5864_HwAnalogComponentsInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { if( FALSE == FH8735_HwAnalogComponentsInitialize( pVideo->m_pDevice ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { if( FALSE == MZ0380_HwAnalogComponentsInitialize( pVideo->m_pDevice ) ) { ; } }

	up( &(pVideo->m_pDevice->m_sComponentAccessSemaphore) ); // [2011.07.01] [HUENGPEI]


	if( pVideo->m_nSubDeviceType == 0x00000001 ) { pVideo->m_pDevice->m_eVideoStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000001; }

	if( pVideo->m_nSubDeviceType == 0x00000002 ) { pVideo->m_pDevice->m_eVideoEncoderStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000001; }

//	spin_unlock_irqrestore( &pVideo->m_pDevice->m_sTaskletAccessSpinLock, flags );

	return videobuf_streamon( &(pVideo->m_sVideoBufQueue) );
}

static int common_video_device_vidioc_streamoff( struct file * p_file, void * p_priv, enum v4l2_buf_type e_buf_type )
{
	CVideo * pVideo = (CVideo *)(p_file->private_data);

	int returns = 0;

	unsigned long flags = 0x00000000;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_streamoff( T:%02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), e_buf_type );

	if( pVideo->m_video_preview_streaming == 0 )
	{
		return 0;
	}

//	spin_lock_irqsave( &pVideo->m_pDevice->m_sTaskletAccessSpinLock, flags ); // [2011.07.01] [HUENGPEI] 會造成當機

	down( &(pVideo->m_pDevice->m_sComponentAccessSemaphore) ); // [2011.07.01] [HUENGPEI]

	if( pVideo->m_nSubDeviceType == 0x00000001 ) { pVideo->m_pDevice->m_eVideoStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000; }

	if( pVideo->m_nSubDeviceType == 0x00000002 ) { pVideo->m_pDevice->m_eVideoEncoderStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000; }

	schedule_timeout_interruptible( msecs_to_jiffies( 10 ) ); // [2011.07.01] [HUENGPEI]

	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210 ) { if( FALSE == CX2581_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851 ) { if( FALSE == CX2588_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { if( FALSE == SA7160_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { if( FALSE == AH8400_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { if( FALSE == TW5864_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { if( FALSE == FH8735_HwAnalogComponentsUnInitialize( pVideo->m_pDevice ) ) { ; } }
	if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { if( FALSE == MZ0380_HwAnalogComponentsUnInitialize( pVideo->m_pDevice ) ) { ; } }

	up( &(pVideo->m_pDevice->m_sComponentAccessSemaphore) ); // // [2011.07.01] [HUENGPEI]

//	spin_unlock_irqrestore( &pVideo->m_pDevice->m_sTaskletAccessSpinLock, flags ); // [2011.07.01] [HUENGPEI]


	return videobuf_streamoff( &(pVideo->m_sVideoBufQueue) );
}

// ##########################################################################################################################################################################################################
// 
static int common_videobuf_queue_ops_buf_setup( struct videobuf_queue * p_queue, unsigned int * p_count, unsigned int * p_size )
{
	CVideo * pVideo = (CVideo *)(p_queue->priv_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_videobuf_queue_ops_buf_setup( C:%02d, S:%02d )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), *p_count, *p_size );

	if( pVideo->m_video_preview_streaming == 0 )
	{
		return 0;
	}

	*p_size = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 2;

   *p_size = pVideo->m_nDstFramePitch * pVideo->m_nDstFrameHeight;

   *p_size = pVideo->m_nDstFrameSize;

	if( *p_count == 0 ) { 
		
		*p_count = 32; //
	}
    while( (*p_size) * (*p_count) > (1024 * 1024 * 48) ) { // 48MB

		*p_count = *p_count - 1;
	}
	return 0;
}

static int common_videobuf_queue_ops_buf_prepare( struct videobuf_queue * p_queue, struct videobuf_buffer * p_buffer, enum v4l2_field e_field ) 
{
	CVideo * pVideo = (CVideo *)(p_queue->priv_data);

	CVideoBuffer * pVideoBuffer = (CVideoBuffer *)(container_of(p_buffer, CVideoBuffer, m_sKsBuffer));

	int ret = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_videobuf_queue_ops_buf_prepare( B:%08X/%08X, F:%08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(p_buffer), (unsigned int)(&(pVideoBuffer->m_sKsBuffer)), e_field );

	if( pVideo->m_video_preview_streaming == 0 )
	{
		return 0;
	}

	if( p_buffer->baddr != 0x00000000 && p_buffer->bsize < pVideo->m_nDstFrameSize ) { return -EINVAL; }

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

	if( p_buffer->state == VIDEOBUF_NEEDS_INIT ) {

	#else

	if( p_buffer->state == STATE_NEEDS_INIT ) {

	#endif

		p_buffer->width = pVideo->m_nDstFrameWidth;

		p_buffer->height = pVideo->m_nDstFrameHeight;

		p_buffer->bytesperline = pVideo->m_nDstFramePitch;

		p_buffer->size = pVideo->m_nDstFrameSize;

		p_buffer->field = e_field;

		ret = videobuf_iolock( p_queue, p_buffer, NULL );

		if( ret != 0 ) { 
			
			common_videobuf_queue_ops_buf_release( p_queue, p_buffer );

			return ret;
		}
	}
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

	p_buffer->state = VIDEOBUF_PREPARED;

	#else

	p_buffer->state = STATE_PREPARED;

	#endif

	return 0;
}

static void common_videobuf_queue_ops_buf_queue( struct videobuf_queue * p_queue, struct videobuf_buffer * p_buffer )
{
	CVideo * pVideo = (CVideo *)(p_queue->priv_data);

	CVideoBuffer * pVideoBuffer = (CVideoBuffer *)(container_of(p_buffer, CVideoBuffer, m_sKsBuffer));

	unsigned long flags = 0x00000000;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_videobuf_queue_ops_buf_queue( B:%08X/%08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(p_buffer), (unsigned int)(&(pVideoBuffer->m_sKsBuffer)) );

	if( pVideo->m_video_preview_streaming == 0 )
	{
		return 0;
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

	p_buffer->state = VIDEOBUF_QUEUED;

	#else

	p_buffer->state = STATE_QUEUED;

	#endif

	spin_lock_irqsave( &pVideo->m_sKsBufferListSpinLock, flags );

	list_add_tail( &pVideoBuffer->m_sKsBuffer.queue, &pVideo->m_sKsBufferList );

	spin_unlock_irqrestore( &pVideo->m_sKsBufferListSpinLock, flags );
}

static void common_videobuf_queue_ops_buf_release( struct videobuf_queue * p_queue, struct videobuf_buffer * p_buffer )
{
	CVideo * pVideo = (CVideo *)(p_queue->priv_data);

	//CVideoBuffer * pVideoBuffer = (CVideoBuffer *)(container_of(p_buffer, CVideoBuffer, m_sKsBuffer));

	unsigned long flags = 0x00000000;

	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_videobuf_queue_ops_buf_release( B:%08X/%08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(p_buffer), (unsigned int)(&(pVideoBuffer->m_sKsBuffer)) );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_videobuf_queue_ops_buf_release( B:%08X )\n", (int)(pVideo->m_pDevice->m_nKsDeviceNumber), (unsigned int)(p_buffer) );

//	spin_lock_irqsave( &pVideo->m_pDevice->m_sTaskletAccessSpinLock, flags ); // [2011.07.01] [HUENGPEI]

	if( pVideo->m_video_preview_streaming == 0 )
	{
		return 0;
	}

	down( &(pVideo->m_pDevice->m_sComponentAccessSemaphore) ); // [2011.07.01] [HUENGPEI]

	if( pVideo->m_nSubDeviceType == 0x00000001 ) { 
		
		if( pVideo->m_pDevice->m_eVideoStreamState[ pVideo->m_nSubDeviceNumber ] > 0x00000000 ) {

			pVideo->m_pDevice->m_eVideoStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000; 

			schedule_timeout_interruptible( msecs_to_jiffies( 10 ) ); // [2011.07.01] [HUENGPEI] 解決同步

			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } } //
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210 ) { if( FALSE == CX2581_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851 ) { if( FALSE == CX2588_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { if( FALSE == SA7160_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x9413 && pVideo->m_pDevice->m_pKsDevice->device == 0x6010 ) { if( FALSE == SL6010_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { if( FALSE == AH8400_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { if( FALSE == TW5864_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { if( FALSE == FH8735_HwAnalogComponentsUnInitialize( pVideo->m_pDevice ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { if( FALSE == MZ0380_HwAnalogComponentsUnInitialize( pVideo->m_pDevice ) ) { ; } }

		}
	}
	if( pVideo->m_nSubDeviceType == 0x00000002 ) { 
		
		if( pVideo->m_pDevice->m_eVideoEncoderStreamState[ pVideo->m_nSubDeviceNumber ] > 0x00000000 ) {
		
			pVideo->m_pDevice->m_eVideoEncoderStreamState[ pVideo->m_nSubDeviceNumber ] = 0x00000000; 

			schedule_timeout_interruptible( msecs_to_jiffies( 10 ) ); // [2011.07.01] [HUENGPEI] 解決同步

			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } } //
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210 ) { if( FALSE == CX2581_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851 ) { if( FALSE == CX2588_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160 ) { if( FALSE == SA7160_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C ) { if( FALSE == AH8400_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864 ) { if( FALSE == TW5864_HwAnalogComponentsUnInitialize( pVideo->m_pDevice, pVideo, NULL ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101 ) { if( FALSE == FH8735_HwAnalogComponentsUnInitialize( pVideo->m_pDevice ) ) { ; } }
			if( pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100 ) { if( FALSE == MZ0380_HwAnalogComponentsUnInitialize( pVideo->m_pDevice ) ) { ; } }
		}
	}
	up( &(pVideo->m_pDevice->m_sComponentAccessSemaphore) ); // [2011.07.01] [HUENGPEI]

//	spin_unlock_irqrestore( &pVideo->m_pDevice->m_sTaskletAccessSpinLock, flags ); // [2011.07.01] [HUENGPEI]

	//////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////

//	spin_lock_irqsave( &pVideo->m_sKsBufferListSpinLock, flags );

	if(	(pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x8210) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pVideo->m_pDevice->m_pKsDevice->device == 0x5851) ) 
	{

#ifdef CX2581_64bit

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			#ifdef LINUXV4L2_VMALLOC_MODULE

			videobuf_vmalloc_free( p_buffer ); //

			#endif

			#else

			struct videobuf_dmabuf * p_dma_buffer = &(p_buffer->dma); //

			videobuf_dma_unmap( p_queue, p_dma_buffer );

			videobuf_dma_free( p_dma_buffer );

			#endif

#else

		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)

		struct videobuf_dmabuf * p_dma_buffer = videobuf_to_dma( p_buffer ); //

		videobuf_dma_unmap( p_queue->dev, p_dma_buffer ); 

		videobuf_dma_free( p_dma_buffer );


		#else 
			
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			struct videobuf_dmabuf * p_dma_buffer = videobuf_to_dma( p_buffer ); //

			videobuf_dma_unmap( p_queue, p_dma_buffer );

	//		videobuf_dma_unmap( p_queue->dev, p_dma_buffer ); // [2011.07.01] [MARCELO]


			videobuf_dma_free( p_dma_buffer );

			#else

			struct videobuf_dmabuf * p_dma_buffer = &(p_buffer->dma); //

			videobuf_dma_unmap( p_queue, p_dma_buffer );

			videobuf_dma_free( p_dma_buffer );

			#endif

		#endif

#endif

	}
	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1131 && pVideo->m_pDevice->m_pKsDevice->device == 0x7160) )
	{

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)

				struct videobuf_dmabuf * p_dma_buffer = videobuf_to_dma( p_buffer ); //

				videobuf_dma_unmap( p_queue->dev, p_dma_buffer ); 

				videobuf_dma_free( p_dma_buffer );

			#else 
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				struct videobuf_dmabuf * p_dma_buffer = videobuf_to_dma( p_buffer ); //

				videobuf_dma_unmap( p_queue, p_dma_buffer );

		//		videobuf_dma_unmap( p_queue->dev, p_dma_buffer ); // [2011.07.01] [MARCELO]


				videobuf_dma_free( p_dma_buffer );

				#else

				struct videobuf_dmabuf * p_dma_buffer = &(p_buffer->dma); //

				videobuf_dma_unmap( p_queue, p_dma_buffer );

				videobuf_dma_free( p_dma_buffer );

				#endif

			#endif

	}

	if( (pVideo->m_pDevice->m_pKsDevice->vendor == 0x1999 && pVideo->m_pDevice->m_pKsDevice->device == 0xA04C) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x5864) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x0907 && pVideo->m_pDevice->m_pKsDevice->device == 0x0100) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x3086 && pVideo->m_pDevice->m_pKsDevice->device == 0x0101) ) {
		
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

		#ifdef LINUXV4L2_VMALLOC_MODULE
		
		videobuf_vmalloc_free( p_buffer ); //

		#endif

		#else

		struct videobuf_dmabuf * p_dma_buffer = &(p_buffer->dma); //

		videobuf_dma_unmap( p_queue, p_dma_buffer );

		videobuf_dma_free( p_dma_buffer );

		#endif
	}
	if(	(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6801) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6802) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6804) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6805) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6810) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6811) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6812) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6813) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6814) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6815) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6816) ||
		(pVideo->m_pDevice->m_pKsDevice->vendor == 0x1797 && pVideo->m_pDevice->m_pKsDevice->device == 0x6817) )
	{
	
		#if TW6805_64bit

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			#ifdef LINUXV4L2_VMALLOC_MODULE

			videobuf_vmalloc_free( p_buffer ); //

			#endif

			#else

			struct videobuf_dmabuf * p_dma_buffer = &(p_buffer->dma); //

			videobuf_dma_unmap( p_queue, p_dma_buffer );

			videobuf_dma_free( p_dma_buffer );

			#endif

		#else
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)

				struct videobuf_dmabuf * p_dma_buffer = videobuf_to_dma( p_buffer ); //

				videobuf_dma_unmap( p_queue->dev, p_dma_buffer ); 

				videobuf_dma_free( p_dma_buffer );


			#else 
				
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				struct videobuf_dmabuf * p_dma_buffer = videobuf_to_dma( p_buffer ); //

				videobuf_dma_unmap( p_queue, p_dma_buffer );

		//		videobuf_dma_unmap( p_queue->dev, p_dma_buffer ); // [2011.07.01] [MARCELO]


				videobuf_dma_free( p_dma_buffer );

				#else

				struct videobuf_dmabuf * p_dma_buffer = &(p_buffer->dma); //

				videobuf_dma_unmap( p_queue, p_dma_buffer );

				videobuf_dma_free( p_dma_buffer );

				#endif

			#endif

		#endif //TW6805_64bit
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

	p_buffer->state = VIDEOBUF_NEEDS_INIT;	

	#else

	p_buffer->state = STATE_NEEDS_INIT;

	#endif

//	spin_unlock_irqrestore( &pVideo->m_sKsBufferListSpinLock, flags );
}

// ##########################################################################################################################################################################################################
//
static int common_snd_pcm_ops_open( struct snd_pcm_substream * p_substream )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);
	
	int ret = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_open()\n", (int)(pDevice->m_nKsDeviceNumber) );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_open()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );

	p_substream->runtime->private_data = NULL;

	p_substream->runtime->private_free = NULL;

	if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x5864 )
	{
		p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_TW5864;
	}
	else if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1999 && pAudio->m_pDevice->m_pKsDevice->device == 0xA04C )
	{
		p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_AH8400;
	}
	else if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1131 && pAudio->m_pDevice->m_pKsDevice->device == 0x7160 )
	{
		p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_SA7160;
	}
	else if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 )
	{
		if( (pAudio->m_pDevice->iProduct & 0x0F) == (0x05) || (pAudio->m_pDevice->iProduct & 0x0F) == (0x07) )
		{
			p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_FH8735;
		}
		else
		{
			p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_FH8735_SC3A0;
		}
	}
	else if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x14F1 && pAudio->m_pDevice->m_pKsDevice->device == 0x8210 )
	{
		p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_CX2581;
	}
	else if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x0907 && pAudio->m_pDevice->m_pKsDevice->device == 0x0100 )
	{
		if( ((pAudio->m_pDevice->iProduct & 0x0F) == 0x02) || ((pAudio->m_pDevice->iProduct & 0x0F) == 0x03) ) // 960H & D1
		{
			p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC3C0;
		}
		else
		{
			p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_MZ0380_SC5C0;
		}
	}
	else
	{
		p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE;
	}



	ret = snd_pcm_hw_constraint_integer( p_substream->runtime, SNDRV_PCM_HW_PARAM_PERIODS );

	if( ret < 0 ) { return ret; }

	pAudio->m_pKsSubStream = p_substream;

	return 0;
}

static int common_snd_pcm_ops_close( struct snd_pcm_substream * p_substream )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_close()\n", (int)(pDevice->m_nKsDeviceNumber) );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_close()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );

	pAudio->m_pKsSubStream = NULL;

	return 0;
}

static int common_snd_pcm_ops_hw_params( struct snd_pcm_substream * p_substream, struct snd_pcm_hw_params * p_hw_params )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_hw_params( PERIODS:%d BYTES/PERIOD:%d )\n", (int)(pDevice->m_nKsDeviceNumber), params_periods(p_hw_params), params_period_bytes(p_hw_params) );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_hw_params( PERIODS:%d BYTES/PERIOD:%d )\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), params_periods(p_hw_params), params_period_bytes(p_hw_params) );

	return snd_pcm_lib_malloc_pages( p_substream, params_buffer_bytes( p_hw_params ) );
}

static int common_snd_pcm_ops_hw_free( struct snd_pcm_substream * p_substream )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_hw_free()\n", (int)(pDevice->m_nKsDeviceNumber) );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_hw_free()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );

	return snd_pcm_lib_free_pages( p_substream );
}

static int common_snd_pcm_ops_prepare( struct snd_pcm_substream * p_substream )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_prepare( R:%d C:%d F:%d PERIODS:%d BYTES/PERIOD:%d TOTAL:%d )\n", (int)(pDevice->m_nKsDeviceNumber), p_substream->runtime->rate, p_substream->runtime->channels, p_substream->runtime->sample_bits, p_substream->runtime->periods, (int)(p_substream->runtime->period_size) * (int)(p_substream->runtime->frame_bits) / (int)(8), (int)(p_substream->runtime->buffer_size) * (int)(p_substream->runtime->frame_bits) / (int)(8) );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_prepare( R:%d C:%d F:%d PERIODS:%d BYTES/PERIOD:%d TOTAL:%d )\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), p_substream->runtime->rate, p_substream->runtime->channels, p_substream->runtime->sample_bits, p_substream->runtime->periods, (int)(p_substream->runtime->period_size) * (int)(p_substream->runtime->frame_bits) / (int)(8), (int)(p_substream->runtime->buffer_size) * (int)(p_substream->runtime->frame_bits) / (int)(8) );

	pAudio->m_nRate = p_substream->runtime->rate;

	pAudio->m_nChannels = p_substream->runtime->channels;

	pAudio->m_nSampleBits = p_substream->runtime->sample_bits;

	pAudio->m_nFrameSize = (int)(p_substream->runtime->period_size) * (int)(p_substream->runtime->frame_bits) / (int)(8);

	pAudio->m_nFrameNumber = 0;

	pAudio->m_nFrames = p_substream->runtime->periods;

//	LINUXV4L2_DEBUG( KERN_INFO, "pAudio->m_nFrames = %d \n", pAudio->m_nFrames );

	return 0;
}

static int common_snd_pcm_ops_trigger( struct snd_pcm_substream * p_substream, int cmd )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_trigger( %08X )\n", (int)(pDevice->m_nKsDeviceNumber), cmd );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_trigger( %08X )\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), cmd );

	if( cmd == SNDRV_PCM_TRIGGER_START ) { //1

		unsigned long flags = 0x00000000;

		if( ( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6801 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6802 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6804 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6805 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6810 ) || 
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6811 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6812 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6813 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6814 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6815 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6816 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6817 )  )
		{
			down( &(pAudio->m_pDevice->m_sComponentAccessSemaphore) ); 
		}

		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			//down( &(pAudio->m_pDevice->m_FH8735AccessSemaphore) ); 
		}
		pAudio->m_nFrameNumber = 0;
		
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6801 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6802 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6804 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6805 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6810 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6811 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6812 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6813 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6814 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6815 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6816 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6817 ) { if( FALSE == TW6805_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		
		//error for more than 2 chip ???
		if( pAudio->m_pDevice->m_nKsDeviceNumber < 1 )//only for 1 chip max
		{
			//if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1131 && pAudio->m_pDevice->m_pKsDevice->device == 0x7160 ) { if( FALSE == SA7160_HwAnalogComponentsInitialize( pAudio->m_pDevice, NULL, FALSE ) ) { ; } }
			//if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 ) { if( FALSE == FH8735_HwAnalogComponentsInitialize( pAudio->m_pDevice ) ) { ; } }
			//if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x0907 && pAudio->m_pDevice->m_pKsDevice->device == 0x0100 ) { if( FALSE == MZ0380_HwAnalogComponentsInitialize( pAudio->m_pDevice ) ) { ; } }
		}
		
		if( pAudio->m_nSubDeviceType == 0x00000001 ) { pAudio->m_pDevice->m_eAudioStreamState[ pAudio->m_nSubDeviceNumber ] = 0x00000001; }

		if( pAudio->m_nSubDeviceType == 0x00000002 ) { pAudio->m_pDevice->m_eAudioEncoderStreamState[ pAudio->m_nSubDeviceNumber ] = 0x00000001; }

		if( ( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6801 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6802 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6804 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6805 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6810 ) || 
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6811 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6812 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6813 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6814 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6815 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6816 ) ||
			( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6817 )  )
		{
			up( &(pAudio->m_pDevice->m_sComponentAccessSemaphore) ); // [2011.07.01] [HUENGPEI]
		}

		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 )
		{
			//up( &(pAudio->m_pDevice->m_FH8735AccessSemaphore) ); // [2011.07.01] [HUENGPEI]
		}

	}
	if( cmd == SNDRV_PCM_TRIGGER_STOP ) { //0

		unsigned long flags = 0x00000000;

		if( pAudio->m_nSubDeviceType == 0x00000001 ) { pAudio->m_pDevice->m_eAudioStreamState[ pAudio->m_nSubDeviceNumber ] = 0x00000000; }

		if( pAudio->m_nSubDeviceType == 0x00000002 ) { pAudio->m_pDevice->m_eAudioEncoderStreamState[ pAudio->m_nSubDeviceNumber ] = 0x00000000; }

//can not put any delay here , otherwise ebell SC300 will hang

//there is no down, up , otherwise ebell SC300 will hang

		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6801 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6802 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6804 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6805 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6810 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6811 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6812 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6813 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6814 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6815 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6816 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6817 ) { if( FALSE == TW6805_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL, pAudio ) ) { ; } }
		
		if( pAudio->m_pDevice->m_nKsDeviceNumber < 1 )//only for 2 chip max
		{
			//if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1131 && pAudio->m_pDevice->m_pKsDevice->device == 0x7160 ) { if( FALSE == SA7160_HwAnalogComponentsUnInitialize( pAudio->m_pDevice, NULL ) ) { ; } }
			//if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 ) { if( FALSE == FH8735_HwAnalogComponentsUnInitialize( pAudio->m_pDevice ) ) { ; } }
			//if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x0907 && pAudio->m_pDevice->m_pKsDevice->device == 0x0100 ) { if( FALSE == MZ0380_HwAnalogComponentsUnInitialize( pAudio->m_pDevice ) ) { ; } }
		}
		pAudio->m_nFrameNumber = 0;

	}
	return 0;
}

static snd_pcm_uframes_t common_snd_pcm_ops_pointer( struct snd_pcm_substream * p_substream )
{
//	CDevice * pDevice = (CDevice *)(snd_pcm_substream_chip( p_substream ));

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_pointer( %d )\n", (int)(pDevice->m_nKsDeviceNumber), (int)(pAudio->m_nFrameNumber) );

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_pointer( %d )\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), (int)(pAudio->m_nFrameNumber) );

	return bytes_to_frames( p_substream->runtime, (pAudio->m_nFrameNumber) * (pAudio->m_nFrameSize) );
}

static int common_snd_kcontrol_new_volume_info( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_info * p_info )
{
	CAudio * pAudio = (CAudio *)(snd_kcontrol_chip( p_kcontrol ));

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_info()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );

	p_info->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	p_info->count = 1;

	p_info->value.integer.min = 0;

	p_info->value.integer.max = 255;

	return 0;
}

static int common_snd_kcontrol_new_volume_get( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value )
{
	CAudio * pAudio = (CAudio *)(snd_kcontrol_chip( p_kcontrol ));

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_get()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );

	if( (pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6802) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6805) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6814) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6815) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6816) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6817) ) {
	
		ULONG R00000048 = 0x00000000;

		R00000048 = TW6805_GetRegister( pAudio->m_pDevice, 0x00000048, 4 ) & 0x0000003F;
		
		p_value->value.integer.value[ 0 ] = pAudio->m_nVolume = R00000048 << 2;
	}
	if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 )
	{
		p_value->value.integer.value[ 0 ] = pAudio->m_nVolume;
	}

	return 0;
}

static int common_snd_kcontrol_new_volume_put( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value )
{
	CAudio * pAudio = (CAudio *)(snd_kcontrol_chip( p_kcontrol ));

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_put( %d )\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), (int)(p_value->value.integer.value[ 0 ]) );

	pAudio->m_nVolume = p_value->value.integer.value[ 0 ]; 

	if( pAudio->m_nVolume > 255 ) { pAudio->m_nVolume = 255; }

	if( pAudio->m_nVolume <   0 ) { pAudio->m_nVolume =   0; }

	if( (pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6802) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6805) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6814) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6815) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6816) ||
		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x1797 && pAudio->m_pDevice->m_pKsDevice->device == 0x6817) ){
	
		ULONG R00000048 = 0x00000000;

		R00000048 = TW6805_GetRegister( pAudio->m_pDevice, 0x00000048, 4 ) & 0xFFFFFFC0;
		
		TW6805_SetRegister( pAudio->m_pDevice, 0x00000048, R00000048 | (pAudio->m_nVolume >> 2), 4 );
	}

	if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x3086 && pAudio->m_pDevice->m_pKsDevice->device == 0x0101 )
	{
		FH8735_SetAnalogAudioVoulmeProperty( pAudio->m_pDevice, pAudio->m_nVolume, pAudio->m_nSubDeviceNumber);
	}
	return 0;
}

// ##########################################################################################################################################################################################################
//
static int common_custom_thread_start( struct file * p_file )
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_custom_thread_start()\n", (int)(pDevice->m_nKsDeviceNumber) );

	pVideo->m_pTaskSoftIntEmulator = kthread_run( common_custom_thread, p_file, "common_custom_thread" );

	if( IS_ERR( pVideo->m_pTaskSoftIntEmulator ) ) {

		int ret = PTR_ERR( pVideo->m_pTaskSoftIntEmulator );

		pVideo->m_pTaskSoftIntEmulator = NULL;

		return ret;
	}
	return 0;
}

static int common_custom_thread_stop( struct file * p_file )
{
	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_custom_thread_stop()\n", (int)(pDevice->m_nKsDeviceNumber) );

	if( pVideo->m_pTaskSoftIntEmulator ) {

		kthread_stop( pVideo->m_pTaskSoftIntEmulator ); // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "STOP" SYNC.

		pVideo->m_pTaskSoftIntEmulator = NULL;
	}
	return 0;
}

static int common_custom_thread( void * p_context )
{
	struct file * p_file = (struct file *)(p_context);

	CDevice * pDevice = (CDevice *)(video_get_drvdata( video_devdata( p_file ) ));

	CVideo * pVideo = (CVideo *)(p_file->private_data);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_custom_thread( enter ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pVideo->m_pTaskSoftIntEmulator) );

	while( kthread_should_stop() == FALSE ) {

#if 0 // FOR TEST ONLY

		unsigned long flags = 0x00000000;

		spin_lock_irqsave( &pVideo->m_sKsBufferListSpinLock, flags );

		if( list_empty( &pVideo->m_sKsBufferList ) == 0 ) {

			CVideoBuffer * pVideoBuffer = list_first_entry( &pVideo->m_sKsBufferList, CVideoBuffer, m_sKsBuffer.queue ); //

			if( waitqueue_active( &pVideoBuffer->m_sKsBuffer.done ) ) { // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "RELEASE" SYNC.

				struct videobuf_dmabuf * p_dma_buffer = NULL;

			//	BYTE * pe = NULL;

			//	BYTE * po = NULL;

				struct timeval ts;

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_custom_thread( begin::remove )\n", (int)(pDevice->m_nKsDeviceNumber) );

				list_del( &pVideoBuffer->m_sKsBuffer.queue );

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				p_dma_buffer = videobuf_to_dma( &pVideoBuffer->m_sKsBuffer ); //

			//	pe = videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer ); //

				#else 

				p_dma_buffer = &(pVideoBuffer->m_sKsBuffer.dma); //

			//	pe = pVideoBuffer->m_sKsBuffer.dma.varea; //

				#endif

				do_gettimeofday( &ts ); 

				pVideoBuffer->m_sKsBuffer.ts = ts;

				pVideoBuffer->m_sKsBuffer.field_count = pVideo->m_nFrameNumber++;

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

				pVideoBuffer->m_sKsBuffer.state = VIDEOBUF_DONE;

				#else

				pVideoBuffer->m_sKsBuffer.state = STATE_DONE;

				#endif

				wake_up( &pVideoBuffer->m_sKsBuffer.done );
	
				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_custom_thread( end::remove )\n", (int)(pDevice->m_nKsDeviceNumber) );
			}
		}
		spin_unlock_irqrestore( &pVideo->m_sKsBufferListSpinLock, flags );

#endif

		schedule_timeout_interruptible( msecs_to_jiffies( 1000 ) ); //
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_custom_thread( exit ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pVideo->m_pTaskSoftIntEmulator) );

	return 0;
}

// ##############################################################################################################################################################################
// 
MODULE_DEVICE_TABLE( pci, LINUXV4L2_PCI_DEVICE_ID_TABLE );

MODULE_AUTHOR( "Linux V4L2" );

MODULE_DESCRIPTION( "Linux V4L2 Device Driver" );

MODULE_LICENSE( "GPL" );

unsigned int g_dw_common_command_param = 0x00000000;

module_param_named( command, g_dw_common_command_param, uint, 0 );

module_init( common_module_on_init );

module_exit( common_module_on_exit );

