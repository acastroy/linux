#ifndef LINUXV4L2_H
#define LINUXV4L2_H

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/firmware.h>
#include <linux/mc146818rtc.h>


//#include "ippcore.h"
//#include "ippi.h"
//#include "ipp.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
#include <media/v4l2-device.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf-vmalloc.h>
#include <media/videobuf-dma-sg.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#include <media/v4l2-common.h>
#include <media/videobuf-vmalloc.h>
#include <media/videobuf-dma-sg.h>
#else
#include <media/v4l2-common.h>
#include <media/video-buf.h>
#include <linux/highmem.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>
#else
#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>
#endif

#if LINUX_VERSION_CODE >=  KERNEL_VERSION(3,1,0)

typedef u64 DMA_nnBIT_MASK __deprecated;

#define DMA_32BIT_MASK  (DMA_nnBIT_MASK)DMA_BIT_MASK(32)

#endif

#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,21)

struct v4l2_encoder_cmd { __u32 cmd; __u32 flags; union { struct { __u32 data[ 8 ]; } raw; }; };

#endif

#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,22)

#define list_first_entry( ptr, type, member ) list_entry( (ptr)->next, type, member )

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

enum v4l2_bitrate_mode { V4L2_BITRATE_NONE = 0, V4L2_BITRATE_CBR, V4L2_BITRATE_VBR };

struct v4l2_mpeg_compression { __u32 _unsupport_struct; };

#endif

#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,26)

#define V4L2_CID_SHARPNESS (V4L2_CID_BASE + 27)

#endif

#if LINUX_VERSION_CODE >=  KERNEL_VERSION(3,6,0)

#pragma message("3,6,0")
//no this member 
#define input privsize

#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,1,0)
#define V4L2_CID_MPEG_VIDEO_H264_PROFILE                (V4L2_CID_MPEG_BASE+363)
enum v4l2_mpeg_video_h264_profile {
        V4L2_MPEG_VIDEO_H264_PROFILE_BASELINE                   = 0,
        V4L2_MPEG_VIDEO_H264_PROFILE_CONSTRAINED_BASELINE       = 1,
        V4L2_MPEG_VIDEO_H264_PROFILE_MAIN                       = 2,
        V4L2_MPEG_VIDEO_H264_PROFILE_EXTENDED                   = 3,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH                       = 4,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10                    = 5,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422                   = 6,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_PREDICTIVE        = 7,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10_INTRA              = 8,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422_INTRA             = 9,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_INTRA             = 10,
        V4L2_MPEG_VIDEO_H264_PROFILE_CAVLC_444_INTRA            = 11,
        V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_BASELINE          = 12,
        V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH              = 13,
        V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH_INTRA        = 14,
        V4L2_MPEG_VIDEO_H264_PROFILE_STEREO_HIGH                = 15,
        V4L2_MPEG_VIDEO_H264_PROFILE_MULTIVIEW_HIGH             = 16,
};

#define V4L2_CID_MPEG_VIDEO_H264_LEVEL                  (V4L2_CID_MPEG_BASE+359)
enum v4l2_mpeg_video_h264_level {
        V4L2_MPEG_VIDEO_H264_LEVEL_1_0  = 0,
        V4L2_MPEG_VIDEO_H264_LEVEL_1B   = 1,
        V4L2_MPEG_VIDEO_H264_LEVEL_1_1  = 2,
        V4L2_MPEG_VIDEO_H264_LEVEL_1_2  = 3,
        V4L2_MPEG_VIDEO_H264_LEVEL_1_3  = 4,
        V4L2_MPEG_VIDEO_H264_LEVEL_2_0  = 5,
        V4L2_MPEG_VIDEO_H264_LEVEL_2_1  = 6,
        V4L2_MPEG_VIDEO_H264_LEVEL_2_2  = 7,
        V4L2_MPEG_VIDEO_H264_LEVEL_3_0  = 8,
        V4L2_MPEG_VIDEO_H264_LEVEL_3_1  = 9,
        V4L2_MPEG_VIDEO_H264_LEVEL_3_2  = 10,
        V4L2_MPEG_VIDEO_H264_LEVEL_4_0  = 11,
        V4L2_MPEG_VIDEO_H264_LEVEL_4_1  = 12,
        V4L2_MPEG_VIDEO_H264_LEVEL_4_2  = 13,
        V4L2_MPEG_VIDEO_H264_LEVEL_5_0  = 14,
        V4L2_MPEG_VIDEO_H264_LEVEL_5_1  = 15,
};
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,24)
enum videobuf_state2 {
         VIDEOBUF_NEEDS_INIT = 0,
         VIDEOBUF_PREPARED   = 1,
         VIDEOBUF_QUEUED     = 2,
         VIDEOBUF_ACTIVE     = 3,
         VIDEOBUF_DONE       = 4,
         VIDEOBUF_ERROR      = 5,
         VIDEOBUF_IDLE       = 6,
};
#endif


typedef          char           CHAR;//    1     1
typedef          short         SHORT;//    2     2
typedef			 int			 INT;//	   4     4
typedef          long           LONG;//    4     8
typedef     long long       LONGLONG;//    8     8
typedef unsigned char          UCHAR;//    1     1
typedef unsigned short        USHORT;//    2     2
typedef unsigned int		    UINT;//    4     4
typedef unsigned long          ULONG;//    4     8
typedef unsigned long long ULONGLONG;//    8     8
typedef unsigned char           BYTE;//    1     1
typedef unsigned short          WORD;//    2     2
typedef unsigned int           DWORD;//    4     4
typedef unsigned char        BOOLEAN;//    1     1
typedef void                    VOID;
typedef void *                 PVOID;//    4     8

//#define AME_FH8735 1
//#define AME_SA7160 1
#define AME_MZ0380 1

//#define VTC 1
//#define APLUS 1

//#define Excelement 1

#define ENABLE_1920X1080PX60FPS 1

#define MEDICAL 1

//#define FFMPEG 1

//#define FFMPEG_V1_1_5 1

#define DMA_ERROR_1080i 1

#define CX2581_64bit 1

#define TW6805_64bit 1

//#define SC3A0_STEREO 1


#define V4L2_IN_CAP_PRESETS             0x00000001 /* Supports S_DV_PRESET */
#define V4L2_IN_CAP_CUSTOM_TIMINGS      0x00000002 /* Supports S_DV_TIMINGS */
#define V4L2_IN_CAP_STD                 0x00000004 

//#define LINUXV4L2_DEFAULT_VIDEO_STANDARD	V4L2_STD_NTSC_M //
#define LINUXV4L2_DEFAULT_VIDEO_STANDARD	V4L2_STD_PAL_B

#define LINUXV4L2_PRINT( level, msg... )	printk( level "[LINUXV4L2] : " msg )

#define LINUXV4L2_DEBUG( level, msg... )	printk( level "[LINUXV4L2] : " msg )

//#define LINUXV4L2_DEBUG( level, msg... )

#define LINUXV4L2_VMALLOC_MODULE

#define FREE( p ) { if( p ) { kfree( p ); p = NULL; } }

#define FALSE 0

#define TRUE 1

#define MAX_SUB_DEVICE_NUM_X_2 32

#define MAX_SUB_DEVICE_NUM     16

#define MAX_DMA_BUFFER_NUM      32

#define MAX_REG_BUFFER_NUM      6

#define V4L2_CID_SWITCH_SPEED                       (V4L2_CID_BASE + 200) //

#define V4L2_CID_SWITCH_TABLE                       (V4L2_CID_BASE + 201)

#define V4L2_CID_HDCP_ENABLE                        (V4L2_CID_BASE + 202)

#define V4L2_CID_HDCP_STATUS                        (V4L2_CID_BASE + 203)

#define V4L2_CID_HV_SCALE_FACTOR                    (V4L2_CID_BASE + 204)

#define V4L2_CID_H264_HV_SCALE_FACTOR               (V4L2_CID_BASE + 205)

#define V4L2_CID_WATCH_DOG_PERIOD                   (V4L2_CID_BASE + 206)

#define V4L2_CID_WATCH_DOG		                    (V4L2_CID_BASE + 207)

#define V4L2_CID_WATCH_DOG_ENABLE                   (V4L2_CID_BASE + 212)

#define V4L2_CID_NO_SIGNAL_BLACK_PATTERN            (V4L2_CID_BASE + 213)

#define V4L2_CID_SPECIAL_CAMERA			            (V4L2_CID_BASE + 214)

#define V4L2_CID_FRAME_RATE		                    (V4L2_CID_BASE + 208)


#define V4L2_CID_INPUT_SELECT                       (V4L2_CID_BASE + 210) // shawn 2010.10.25

#define V4L2_CID_AUDIO_SAMPLING_FREQ                (V4L2_CID_BASE + 211) 

#define V4L2_CID_MOTION_STATUS						(V4L2_CID_BASE + 250) //

#define V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD	(V4L2_CID_BASE + 251)

#define V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD	(V4L2_CID_BASE + 252)

#define V4L2_CID_MOTION_MASK_CX						(V4L2_CID_BASE + 253)

#define V4L2_CID_MOTION_MASK_CY						(V4L2_CID_BASE + 254)

#define V4L2_CID_OSD_POS_X							(V4L2_CID_BASE + 260) //

#define V4L2_CID_OSD_POS_Y							(V4L2_CID_BASE + 261)

#define V4L2_CID_OSD_COLOR							(V4L2_CID_BASE + 262)

#define V4L2_CID_OSD_TEXT							(V4L2_CID_BASE + 263)

#define V4L2_CID_GPIO_DIRECTION						(V4L2_CID_BASE + 270) // CUSTOM GPIO IOCTL

#define V4L2_CID_GPIO_DATA							(V4L2_CID_BASE + 271)

#define V4L2_CID_PCM_UPDATE							(V4L2_CID_BASE + 272)

#define V4L2_CID_TW5864_MOTION_STATUS_00			(V4L2_CID_BASE + 280) 
#define V4L2_CID_TW5864_MOTION_STATUS_01			(V4L2_CID_BASE + 281) 
#define V4L2_CID_TW5864_MOTION_STATUS_02			(V4L2_CID_BASE + 282) 
#define V4L2_CID_TW5864_MOTION_STATUS_03			(V4L2_CID_BASE + 283) 
#define V4L2_CID_TW5864_MOTION_STATUS_04			(V4L2_CID_BASE + 284) 
#define V4L2_CID_TW5864_MOTION_STATUS_05			(V4L2_CID_BASE + 285) 
#define V4L2_CID_TW5864_MOTION_STATUS_06			(V4L2_CID_BASE + 286) 
#define V4L2_CID_TW5864_MOTION_STATUS_07			(V4L2_CID_BASE + 287) 

#define V4L2_CID_TW5864_MOTION_TEMPORAL_SENSITIVITY	(V4L2_CID_BASE + 290) 
#define V4L2_CID_TW5864_MOTION_SPATIAL_SENSITIVITY	(V4L2_CID_BASE + 291) 
#define V4L2_CID_TW5864_MOTION_LEVEL_SENSITIVITY	(V4L2_CID_BASE + 292) 
#define V4L2_CID_TW5864_MOTION_SPEED				(V4L2_CID_BASE + 293) 

#define V4L2_CID_X_RES_DETECTED						(V4L2_CID_BASE + 294) 
#define V4L2_CID_Y_RES_DETECTED						(V4L2_CID_BASE + 295) 
#define V4L2_CID_FPS_DETECTED						(V4L2_CID_BASE + 296) 

#define V4L2_CID_INTERLEAVE_DETECTED				(V4L2_CID_BASE + 297) 
#define V4L2_CID_ENC_DEINTERLACE					(V4L2_CID_BASE + 298) 

#define V4L2_CID_RESET_I2S							(V4L2_CID_BASE + 299) 

#define V4L2_CID_VBI_ENABLE							(V4L2_CID_BASE + 300) 

#define V4L2_CID_VBI_LINE							(V4L2_CID_BASE + 301) 

#define V4L2_CID_MPEG_VIDEO_QUALITY					(V4L2_CID_MPEG_BASE + 250) //

#define V4L2_CID_MPEG_VIDEO_FRAMERATE				(V4L2_CID_MPEG_BASE + 251)

#define V4L2_CID_MPEG_VIDEO_QPSTEP					(V4L2_CID_MPEG_BASE + 252)
//shawn 2010.09.08
#define V4L2_CID_MPEG_VIDEO_OSD						(V4L2_CID_MPEG_BASE + 253)

#define V4L2_CID_MPEG_VIDEO_OSD_LINE				(V4L2_CID_MPEG_BASE + 254)

#define V4L2_CID_MPEG_VIDEO_OSD_ENABLE				(V4L2_CID_MPEG_BASE + 256)

#define V4L2_CID_MPEG_SET_KEY_FRAME					(V4L2_CID_MPEG_BASE + 255)


typedef union _CSCMatrix
{
    struct
	{
	    BYTE data[ 24 ];

	} b;

	struct
	{
		USHORT CSC_M11, CSC_M12, CSC_M13, CSC_M21, CSC_M22, CSC_M23, CSC_M31, CSC_M32, CSC_M33;

		USHORT CSC_A1, CSC_A2, CSC_A3;

	} w;

} CSCMatrix;

typedef struct _H264_STREAM_HEADER
{
	DWORD nFrameBufferSize : 24; // FRAME BUFFER SIZE (H.264 BITSTREAM LENGTH) = 0x00000000 ~ 0x00FFFFFFFF

	DWORD nFrameType       :  1; // 0 = I FRAME / 1 = P FRAME

	DWORD nMotionStatus    :  1; // 0 = STATIC / 1 = MOTION

	DWORD nLockStatus      :  1; // 0 = UNLOCK / 1 = LOCK

	DWORD nReserved        :  5;
		
} H264_STREAM_HEADER;


typedef struct CVideoBuffer {

	struct videobuf_buffer  m_sKsBuffer;

} CVideoBuffer;

typedef struct CAudioBuffer {

	struct snd_dma_buffer * m_pKsBuffer;

} CAudioBuffer;

typedef struct CVideo {

	struct CDevice *        m_pDevice;
	ULONG                   m_nSubDeviceNumber;
	ULONG                   m_nSubDeviceType;

	ULONG                   m_nStandard;
	ULONG                   m_nInput;
	ULONG                   m_nLockStatus;
	ULONG                   m_nLockStatusMask_Spliter;
	ULONGLONG               m_nLockAccumulatorCounter_Spliter;
	ULONG					m_nDropDecumulatorCounter_Spliter;
	ULONG					m_nSwitchNumber_Spliter;
	ULONG					m_nSwitchSpeed_Spliter;
	BYTE					m_pSwitchTable_Spliter[ 12 ];

	ULONG                   m_nSrcFrameWidth; //
	ULONG                   m_nSrcFrameHeight;
	ULONG                   m_nSrcFrameBitCount;
	ULONG                   m_nSrcFrameSize;
	ULONG                   m_nDstFrameWidth; //
	ULONG                   m_nDstFrameHeight;
	ULONG                   m_nDstFramePitch;
	ULONG                   m_nDstFrameBitCount;
	ULONG                   m_nDstFrameSize;
	ULONG                   m_nFramePixelFormat; //
	ULONG                   m_nFramePixelField;
	ULONG                   m_nFrameRate;
	ULONG                   m_nFrameNumber;
	UINT                    m_nFrameCopyPolarity;

	ULONG					m_nEncoderMode; //
	ULONG					m_nEncoderBitRate;
	ULONG					m_nEncoderQuality;
	ULONG					m_nEncoderGop;
	ULONG					m_nEncoderQPStep;
	
    spinlock_t              m_sVideoBufQueueSpinLock; //
	struct videobuf_queue   m_sVideoBufQueue;
	spinlock_t              m_sKsBufferListSpinLock; //
	struct list_head        m_sKsBufferList;
	struct list_head        m_sKsBufferListList;
//peter [2011.05.09]
	struct completion       m_pKsBufferListFrameProcessCompletion;

	struct task_struct *    m_pTaskSoftIntEmulator; //

	ULONG					m_video_preview_frame_number;//for custom fps
	
	ULONG					m_video_preview_streaming;//1 for streaming, 0 for control

	ULONG	   				n_main_H264_count;//number of bytes in AP buffer
	ULONG	   				n_main_H264_left;//number of bytes in temp buffer

	ULONG	   				n_sub_H264_count;//number of bytes in AP buffer
	ULONG	   				n_sub_H264_left;//number of bytes in temp buffer

} CVideo;

typedef struct CAudio {

	struct CDevice *           m_pDevice;
	ULONG                      m_nSubDeviceNumber;
	ULONG                      m_nSubDeviceType;
	ULONG                      m_nInput;
	ULONG                      m_nVolume;
	ULONG                      m_nRate;
	ULONG                      m_nChannels;
	ULONG                      m_nSampleBits;
	ULONG                      m_nFrameSize;
	ULONG                      m_nFrameNumber;
	ULONG                      m_nFrames;
	struct snd_pcm_substream * m_pKsSubStream;

} CAudio;

//do not shorten than 3, since DMA is i = i + 1, (prepare for next)
#define SA7160_MAX_BUFFER 3

#define TW6816_MAX_BUFFER 2

#define CX2581_MAX_BUFFER 2

typedef struct CDevice {

	struct pci_dev *       m_pKsDevice;
	ULONG                  m_nKsDeviceNumber;
	ULONG                  m_nKsDeviceBusNumber;
	ULONG                  m_nKsDeviceVideoNumber;
	ULONG                  m_nKsDeviceAudioNumber;
	ULONG                  m_nKsDeviceVideoEncoderNumber;
	ULONG                  m_nKsDeviceAudioEncoderNumber;
	struct video_device *  m_pKsVideo                              [ MAX_SUB_DEVICE_NUM ];
	struct snd_card *      m_pKsAudioCard                          [ MAX_SUB_DEVICE_NUM ];
	struct snd_pcm  *      m_pKsAudioPCM                           [ MAX_SUB_DEVICE_NUM ];
	CVideo *               m_pVideo                                [ MAX_SUB_DEVICE_NUM ];
	CAudio *               m_pAudio                                [ MAX_SUB_DEVICE_NUM ];
	ULONG			       m_eVideoStreamState                     [ MAX_SUB_DEVICE_NUM ];
	ULONG				   m_eAudioStreamState                     [ MAX_SUB_DEVICE_NUM ];
	struct video_device *  m_pKsVideoEncoder                       [ MAX_SUB_DEVICE_NUM ];
	struct snd_card *      m_pKsAudioEncoderCard                   [ MAX_SUB_DEVICE_NUM ];
	struct snd_pcm  *      m_pKsAudioEncoderPCM                    [ MAX_SUB_DEVICE_NUM ];
	CVideo *               m_pVideoEncoder                         [ MAX_SUB_DEVICE_NUM ];
	CAudio *               m_pAudioEncoder                         [ MAX_SUB_DEVICE_NUM ];
	ULONG				   m_eVideoEncoderStreamState              [ MAX_SUB_DEVICE_NUM ];
	ULONG				   m_eAudioEncoderStreamState              [ MAX_SUB_DEVICE_NUM ];
	dma_addr_t             m_pDmaBaseCommonPhysicalBuffer          [ MAX_DMA_BUFFER_NUM ]; // // shawn 2010.11.09
	BYTE *                 m_pDmaBaseCommonBuffer                  [ MAX_DMA_BUFFER_NUM ]; //
	ULONG                  m_nDmaBaseCommonBufferSize              [ MAX_DMA_BUFFER_NUM ]; //
	ULONG                  m_nDmaBaseCommonBufferNumber            [ MAX_DMA_BUFFER_NUM ]; //
	ULONG                  m_nDmaBaseCommonBufferCustomMiscProperty[ MAX_DMA_BUFFER_NUM ]; //
	BYTE __iomem *         m_pRegBaseCommonBuffer                  [ MAX_REG_BUFFER_NUM ]; //
	ULONG                  m_nRegBaseCommonBufferSize              [ MAX_REG_BUFFER_NUM ]; //
	ULONG			       m_pRegBaseCommonPhysicalBuffer          [ MAX_REG_BUFFER_NUM ]; // REG BASE BUFFER (PHYSICAL ADDRESS)

	struct tasklet_struct  m_sTasklet;
	ULONG                  m_nTaskletExtraParameterA;
	ULONG                  m_nTaskletExtraParameterB;
	spinlock_t             m_sTaskletAccessSpinLock;
	volatile BOOLEAN	   m_hInterruptAccessLock;
	struct semaphore	   m_sI2CAccessSemaphore;
	ULONG                  m_nDigitalCaptureStreamPowerReference;
	volatile ULONG         m_nAnalogCaptureStreamPowerReference;
	ULONG                  m_nCustomHardwareTopologyProperty; 
	ULONG                  m_nCustomVideoStandardProperty;
	ULONG                  m_nCustomAudioStandardProperty;

	ULONG		           m_nCustomAnalogAudioSampleFrequencyProperty;

	ULONG					m_nCustomGpioDirectionProperty;
	ULONG					m_nCustomGpioDataProperty;
	ULONG					m_nCustomGpioSupportProperty;

	ULONG				   m_nCustomSoftwareWatchdogDurationProperty;	// shawn 2010.08.11
	struct task_struct *   m_pControlThread;							// shawn 2010.08.11

	ULONG				   m_ReadyToStopControlThread;	// shawn 2010.08.11

	ULONG				   m_nCustomAnalogVideoScaleOutputProperty;		// shawn 2010.10.04
	PVOID				   m_pCustomSystemConfigProperty;				// shawn 2010.11.04
	ULONG				   m_nAnalogCopyProtMacrovisionProperty;		// shawn 2010.10.19
	ULONG				   m_nAnalogVideoDecoderStatusProperty;			// shawn 2010.10.19
	ULONG				   m_nAnalogCrossbarVideoInputProperty;						// shawn 2010.10.25
	ULONG				   m_nAnalogCrossbarAudioInputProperty;
	ULONG				   m_nCustomAnalogVideoResolutionProperty;		// shawn 2010.11.04
	ULONG				   m_nCustomAnalogVideoFrameRateProperty;		// shawn 2010.11.04
	ULONG				   m_nCustomAnalogVideoInterleavedProperty;
//peter [2011.05.09]
	struct semaphore       m_sComponentAccessSemaphore;					// shawn 2010.07.26
	struct semaphore       m_sSGSemaphore;					// shawn 2010.07.26
	ULONG		           m_nDummy;//for m_sSGSemaphore access?????


//peter
	ULONG		          m_nCustomEncoderVideoResolutionProperty[ 32 ];
	ULONG		          m_nAnalogVideoCompressionKeyframeRateProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoBitRateModeProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoBitRateProperty[ 32 ];
	ULONG		          m_nAnalogVideoCompressionQualityProperty[ 32 ];
	ULONG		          m_nAnalogVideoCompressionOverrideKeyframeProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoBframesPerKeyframeProperty[ 32 ];
	//
	ULONG				  m_nCustomOsdTextLineProperty[ 32 ];
	CHAR				  m_pCustomOsdTextStringProperty[ 32 ][ 16 ][ 64 + 1 ];
	BOOLEAN				  m_bCustomOsdTextStringRefreshProperty[ 32 ][ 16 ];
	ULONG				  m_nCustomOsdTextColorProperty[ 32 ];


	ULONG				  m_nCustomAnalogVideoVbiLinesEnableProperty;
	ULONG				  m_nCustomAnalogVideoVbiLinesProperty;
	ULONG				  m_nCustomSpecialCameraInputProperty;
	ULONG				  m_nCustomCompanyIvsProperty;


//peter [2011.08.23]
	wait_queue_head_t	  m_wait_queue_head;
	wait_queue_t          m_wait_queue;
	ULONG				  m_FH8735_ready;
	struct semaphore      m_FH8735AccessSemaphore;					

	ULONG				  m_MZ0380_ready;
	struct semaphore      m_MZ0380AccessSemaphore;					

//	ULONG				  m_pcm_update[16];

	ULONG 				  CFG0;
	ULONG 				  CFGB;
	ULONG 				  CFG2;
	ULONG				  idVendor;      
	ULONG				  idProduct;     
	ULONG				  bcdDevice;    
	ULONG				  iManufacturer;
	ULONG				  iProduct;      
	ULONG				  iSerialNumber;

//
	ULONG				  m_nCustomMotionStatusProperty[ 32 ][ 8 ];
	ULONG				  m_nCustomMotionTemporalSensitivityProperty[ 32 ];
	ULONG				  m_nCustomMotionSpatialSensitivityProperty[ 32 ];
	ULONG				  m_nCustomMotionLevelSensitivityProperty[ 32 ];
	ULONG				  m_nCustomMotionSpeedProperty[ 32 ];
	ULONG				  m_nCustomSoftwareWatchdogResetProperty;

	ULONG				  m_nAnalogCopyProtMacrovisionEnableProperty;

	ULONG*				  m_SA7160_video_buffer[ SA7160_MAX_BUFFER ];//virtual address
	ULONG				  m_SA7160_video_buffer_physical_address[ SA7160_MAX_BUFFER ];

#ifdef TW6805_64bit
	ULONG*				  m_TW6816_video_buffer[ TW6816_MAX_BUFFER ];//virtual address
	ULONG				  m_TW6816_video_buffer_physical_address[ TW6816_MAX_BUFFER ];
#endif //TW6805_64bit

#ifdef CX2581_64bit
	ULONG*				  m_CX2581_video_buffer[ 8 ][ CX2581_MAX_BUFFER ];//virtual address
	ULONG				  m_CX2581_video_buffer_physical_address[ 8 ][ CX2581_MAX_BUFFER ];
#endif //CX2581_64bit

	ULONG				 m_IRQ_ready;
//FH8735
	ULONG				   m_nCustomAnalogVideo720OutputProperty;     //??
	ULONG		          m_nCustomEncoderVideoProfileProperty[ 32 ]; //??

	ULONG		          m_nCustomPreviewVideoResolutionProperty[ 32 ];
	ULONG				  m_nCustomRegionMaskParamsProperty[ 32 ][ 2 ][ 4 ];
	ULONG		          m_nCustomAnalogVideoDeinterlaceTypeProperty[ 16 ];
	ULONG		          m_nCustomAnalogVideoDenoiseTypeProperty[ 16 ];
	ULONG		          m_nCustomAnalogVideoDemiseTypeProperty[ 16 ];
	ULONG				  m_nCustomAnalogVideoAutoPhaseTypeProperty[ 16 ];
	ULONG		          m_nCustomEncoderVideoEntropyProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoAspectRatioProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoAvgFrameRateProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoSkipFrameRateProperty[ 32 ];

	ULONG				  m_nCustomAnalogVideoSkipFrameRateProperty[ 16 ];
	ULONG				  m_nCustomAnalogVideoAvgFrameRateProperty[ 16 ];

	BOOLEAN				  m_bCustomOsdTextEnableProperty;
	ULONG				  m_nHVScaleFactor;//preview
	ULONG				  m_nH264HVScaleFactor;//0x00 00 00 00, mainH mainV subH subV

	ULONG				  m_nCustom_H264_Level_Property;

	ULONG				  m_nCustomSoftwareWatchdogEnableProperty;

	ULONG				  m_Preview_Opened_counter[ MAX_SUB_DEVICE_NUM ];

	ULONG				  m_Preview_frame_rate_setting;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)

	struct v4l2_device		v4l2_dev;
#endif

	BOOLEAN				m_pControlThread_already_stop;

	ULONG				H264_main_frame_count;
	ULONG				audio_byte_count;
	
	struct timeval		previous_field_ts;

	ULONG				field_counter;

	ULONG				m_nCustomAnalogVideoVgaHActiveProperty;
	ULONG				m_nCustomAnalogVideoVgaAspectRatioProperty;
	ULONG				m_nCustomCompanyVision4CEProperty;
	ULONG				m_nCustomAnalogVideoColorRangeProperty;

	ULONG				m_MST3367_bank;

	ULONG				n_video_buffer_count;

	ULONG				m_CX2581_video_buffer_count[ 8 ];

	ULONG				m_No_Signal_black_pattern_property;

	ULONG				m_colorbar_pos;////position of colobar buffer
	ULONG				m_buf_pos;//position of desination buffer
	ULONG				m_src_pos;//position of source buffer

} CDevice;

#define BCD_TO_BINARY( counts ) (((counts) >> 4) * 10 + ((counts) & 0x0F)) 


#define TW6805_MAX_VIDEO_DMA_BUFFER_SIZE	(     3624 * 16)
#define TW6805_MAX_AUDIO_DMA_BUFFER_SIZE	(      24 * 128)
#define TW6805_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define TW6805_MAX_HX264_DMA_BUFFER_SIZE	(             0)
#define TW6805_MAX_MPEG4_DMA_BUFFER_SIZE	(             0)
#define TW6805_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

#define CX2581_MAX_VIDEO_DMA_BUFFER_SIZE	( 7020 *  2 * 8)
#define CX2581_MAX_AUDIO_DMA_BUFFER_SIZE	(   24 * 60 * 4)
#define CX2581_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define CX2581_MAX_HX264_DMA_BUFFER_SIZE	(             0)
#define CX2581_MAX_MPEG4_DMA_BUFFER_SIZE	(             0)
#define CX2581_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

//SHAWN 
#define CX2588_MAX_VIDEO_DMA_BUFFER_SIZE	( 7020 *  2 * 8)
#define CX2588_MAX_AUDIO_DMA_BUFFER_SIZE	(   24 * 60 * 4)
#define CX2588_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define CX2588_MAX_HX264_DMA_BUFFER_SIZE	(             0)
#define CX2588_MAX_MPEG4_DMA_BUFFER_SIZE	(             0)
#define CX2588_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

#define SA7160_MAX_VIDEO_DMA_BUFFER_SIZE	( 4096 *  2 *10)
#define SA7160_MAX_AUDIO_DMA_BUFFER_SIZE	( 4096 *  1 *10)//address to put audio data
#define SA7160_MAX_XXVBI_DMA_BUFFER_SIZE	( 4096 *  1 *10)//audio data
#define SA7160_MAX_HX264_DMA_BUFFER_SIZE	(			  0)
#define SA7160_MAX_MPEG4_DMA_BUFFER_SIZE	(			  0)
#define SA7160_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)
//

#define SL6010_MAX_VIDEO_DMA_BUFFER_SIZE	(0x000C6000 * 3)
#define SL6010_MAX_AUDIO_DMA_BUFFER_SIZE	(0x00010000 * 1)
#define SL6010_MAX_XXVBI_DMA_BUFFER_SIZE	(0x00010000 * 4)
#define SL6010_MAX_HX264_DMA_BUFFER_SIZE	(             0)
#define SL6010_MAX_MPEG4_DMA_BUFFER_SIZE	(0x00096000 * 8)
#define SL6010_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

#define AH8400_MAX_VIDEO_DMA_BUFFER_SIZE	(0x000CA800 * 2)
#define AH8400_MAX_AUDIO_DMA_BUFFER_SIZE	(0x00000500 * 4)
#define AH8400_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define AH8400_MAX_HX264_DMA_BUFFER_SIZE	(0x00096000 * 4)
#define AH8400_MAX_MPEG4_DMA_BUFFER_SIZE	(             0)
#define AH8400_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

#define TW5864_MAX_VIDEO_DMA_BUFFER_SIZE	(	 0x00097E00) //preview buffer 1, in case no continous memory to allocate 
#define TW5864_MAX_AUDIO_DMA_BUFFER_SIZE	(             0)
#define TW5864_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define TW5864_MAX_HX264_DMA_BUFFER_SIZE	(	1024 * 1024)//audio and H.264
#define TW5864_MAX_MPEG4_DMA_BUFFER_SIZE	(			  0)
#define TW5864_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)


//peter [2011.06.29]
#define FH35_MAX_BASE_SIZE					0xc00000
#define FH8735_MAX_VIDEO_DMA_BUFFER_SIZE	(0x00097E20 * 2) 
#define FH8735_MAX_HD_VIDEO_DMA_BUFFER_SIZE	(    0x002FD020) 
#define FH8735_MAX_AUDIO_DMA_BUFFER_SIZE	(            64)
#define FH8735_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define FH8735_MAX_HX264_DMA_BUFFER_SIZE	(0x00100000 * 3) //1024 * 1024)
#define FH8735_MAX_MPEG4_DMA_BUFFER_SIZE	(             0)
#define FH8735_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)


#define MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE		(0x00097F00 * 6) //HD
#define MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE	(0x000CA900 * 4)
#define MZ0380_MAX_VIDEO_SD_DMA_BUFFER_SIZE 	(0x00097F00 * 4)
#define MZ0380_MAX_VIDEO_SD_SUB_H264_BUFFER_SIZE (0x00032B00 * 4)
#define MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE		(      4096 * 4)
#define MZ0380_MAX_AUDIO_SD_DMA_BUFFER_SIZE		(	   8192 * 16)
#define MZ0380_MAX_VBI_DMA_BUFFER_SIZE			(            64)
#define MZ0380_MAX_MPEG_DMA_BUFFER_SIZE			(0x00097F00 * 6)


#define DEFAULT_MST3367_VESA_SIZE	75

#define DEFAULT_MST3367_EIA_SIZE	20

#define NTSTATUS		int

#define STATUS_SUCCESS		0x00000001
#define STATUS_UNSUCCESSFUL 0x00000000

#define SUPPORTED_ANALOG_VIDEO_STANDARDS_60HZ	V4L2_STD_525_60
#define SUPPORTED_ANALOG_VIDEO_STANDARDS_50HZ	V4L2_STD_625_50

#endif


// windows m_nStreamBaseCommonBufferLockNumber linux m_nDmaBaseCommonBufferCustomMiscProperty
// m_nStreamBaseCommonBufferFrameNumber m_nDmaBaseCommonBufferNumber[ 0 ]
// 
