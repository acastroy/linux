/*************************************************
Fullhan Microelectronics Co., Ltd.
Copyright (C), 2007.All Rights Reserved
File name:      Cfg.h
Author:         William
Project:        FH35 Firmware
Description:    FH35_Firmwire user config structures
Revision  History:
v0.1
-Date of Modify    : 2007-09-13
-Modifier          : William
-Description       : initial version
*************************************************/

#ifndef __sys_cfg_h__
#define __sys_cfg_h__

#include "ite6603.h"

#define TOSD_LEN 0xca8

// [PATCH PROGRAM] [HUENGPEI]
//
#define MAX_CH               16

#define FH8735_USR_CFG_HEAD  usr_cfg_head_t

#define FH8735_CHIP_CFG      chip_cfg_t

#define FH8735_CH_CFG        ch_cfg_t

#define FH8735_I2S_CFG       i2s_attr_t

#define FH8735_SYS_CFG       sys_cfg_t

#define FH8735_SYS_CFG_EX    sys_cfg_ex_t

typedef UINT                uint_32;

typedef int                int_32;

typedef BYTE                uint_8;

typedef CHAR                 int_8;

//typedef BOOLEAN                 BOOL;


//视频输出配置数据结构定义
/*注1：vou _win_topleftx+ vou _win_width不应该大于D1图像的宽度（720），否则，超过的部分会自动丢掉。
主2：vou _win_toplefty+ vou _win_heigth不应该大于D1图像的高度（PAL制式为576，NTSC制式为480）。超过部分会自动丢掉。
*/
typedef struct
{
	uint_32 flags;                           /*结构体有效标识符*/
	int_32  vou_pic_type;                   /*NTSC或者PAL制式选择。0：PAL制；1：NTSC制*/
	int_32  vou_win_topleftx1;           /*视频输出窗口在显示屏幕中左上角的横坐标。以4像素为单位*/
	int_32  vou_win_toplefty2;           /*视频输出窗口在显示屏幕中左上角的纵坐标。以4像素为单位*/
	int_32  vou_win_width;               /*视频输出窗口宽。以4像素为单位*/
	int_32  vou_win_heigth;              /*视频输出窗口高。以4像素为单位*/
	int_32  background_y;                /*背景亮度分量,取值范围[16, 235]*/
	int_32  background_cb;               /*背景色度分量,取值范围[16, 240]*/
	int_32  background_cr;               /*背景色度分量,取值范围[16, 240]*/
}vou_sys_cfg_t;

//音频输入配置数据结构定义
typedef struct
{
	uint_32 flags;                 /*结构体有效标识符*/
	int_32   input_en[MAX_CH]; /*是否需要进行音频采集。       0：不需要；1：需要*/
	int_32   out_channel;            /*是否需要进行音频输出数据。0：不需要；音频输出通道号，CH：1～8*/
	int_32   sample_rate;            /*音频采样率设置*/
}aiu_cfg_t;

/*视频输入配置数据结构定义*/
typedef struct
{
	uint_32 flags;                /*结构体有效标识符*/
	int_32  input_mode;         /*视频输入模式。Stream0到stream7的输入模式相同；stream8到stream15的输入模式相同。只有stream0和stream8的此值有效。
							 0：标准BT.656
							 1：增强BT.656
							 2：高清视频
							 3：TW2815 TM
							 */
	int_32  frame_rate;         /*视频接收帧率。取值范围：[1, 输入视频帧率]*/
	int_32  input_en;             /*通道是否选通。0：不选通；1：选通*/
}_viu_cfg_t;


//图形OSD配置数据结构定义
typedef struct
{
	int_32  osd_graphic_en;              /*是否启动本图形OSD叠加。0：不启动；1：启动*/
	int_32  osd_graph_top_left_x;        /*图形窗口左上角水平坐标X。以宏块为单位，取值范围: [0, 127]*/
	int_32  osd_graph_top_left_y;        /*图形窗口左上角垂直坐标Y。以宏块为单位		，取值范围: [0, 127]*/
	int_32  osd_graph_width;             /*图形窗口宽度。以宏块为单位，取值范围: [0, 127]*/
	int_32  osd_graph_height;            /*图形窗口高度。以宏块为单位，取值范围: [0, 127]*/
	int_32  osd_graph_clut_type;         /*索引图类型。2：两位索引图1：四位索引图0：八位索引图*/
	uint_8	index_data[0x18C00];    //索引图
	uint_8	lut_data[0x400];         //颜色查找表
}osd_graph_t;

/*文本OSD配置数据结构定义*/
typedef struct
{
	int_32  osd_txt_en;                  /*叠加TEXT启动控制 0：不启动1：启动*/
	int_32  brightness;                  /*字符亮度。0：黑色；1：白色；2：自动变色*/
	int_32  scalar;                      /*字体放大系数*/
	int_32  osd_txt_transparency;        /*TEXT的透明度。0：不透明1：50%透明度*/
	int_32  text_length;                 /*文本内容以Byte为单位的长度*/
	int_32  text_line;                   /*文本内容的具体行数。*/
	int_32  osd_text_addr;               /*图像窗口数据信息地址*/
	uint_8	data[TOSD_LEN];             //文本OSD数据
}osd_txt_t;

typedef struct
{
	uint_32 flags;                               /*结构体有效标识符*/
	osd_txt_t   osd_txt;                    /*和一个TOSD*/
	osd_graph_t osd_graph[2];         /*VPU具有两个GOSD*/
	
}osd_cfg_t;

//视频预处理配置数据结构定义
typedef struct
{
	uint_32 flags;                       /*结构体有效标识符*/
	int_32  Deinterlace_mode;            /*Deinterlace algorithm select 0: Off, 1: Median, 2: Enhanced Median*/
	int_32  DeintlaceMDThr;              /*Deinterlace motion detection threshold. 取值范围[1:60]*/

	int_32  DenoiseL_Coeff ;
	/*亮度去噪算法选择：0:off
	Y分量去噪滤波强度
	0：不滤波
	1：极弱滤波（1，14，1）
	2：弱滤波（1，6，1）
	3：强滤波（1，2，1）
	*/

	int_32  DenoiseC_Coeff ;
	/*色度去噪算法选择：0:off
	UV分量去噪滤波强度
	0：不滤波
	1：极弱滤波（1，14，1）
	2：弱滤波（1，6，1）
	3：强滤波（1，2，1）
	*/

	int_32  MainScaler;                  /*主编码scalar输出enable 0: off 1: on */
	int_32  MScalerPictType;             /*主编码scalar输出格式。 0: Frame  1: Field */
	int_32  HCoeff;                      /*horizontal scale fator 0：原始尺寸 1: 缩小到原始尺寸的3/4 2: 缩小到原始尺寸的1/2 3: 缩小到原始尺寸的1/4*/
	int_32  VCoeff;                      /*Vertical scale fator
									  0：原始尺寸
									  1: 缩小到原始尺寸的3/4
									  2: 缩小到原始尺寸的1/2
									  3: 缩小到原始尺寸的1/4    */
	int_32  AuxScaler;                   /*次编码输出enable
									  0: off
									  1: on
									  */
	int_32  Aux_HCoeff;                  /*horizontal scale fator
									  0：原始尺寸
									  1: 缩小到原始尺寸的3/4
									  2: 缩小到原始尺寸的1/2
									  3: 缩小到原始尺寸的1/4
									  */
	int_32  Aux_VCoeff;                  /*Vertical scale fator
									  0：原始尺寸
									  1: 缩小到原始尺寸的3/4
									  2: 缩小到原始尺寸的1/2
									  3: 缩小到原始尺寸的1/4
									  */
	int_32  PrvScaler;                   /*预览输出enable
									  0: off
									  1: on
									  */
#define PRV_420 0
#define PRV_422 1
	int_32  PrvPicType;                      /*预览输出模式：
												0: 422模式
												1: 420模式
												*/
	int_32  PScalerPictType;             /*预览输出帧场模式选择
									  0: Frame
									  1: Field
									  */
	int_32  PrvScalerMode;               /*预览scalar模式选择
									  0: Mean
									  1: Cubic + Linear
									  */
	int_32  Prv_H_N;                     /*Scaler numerator of horizontal scale fator
									  范围：[1 8]
									  */
	int_32  Prv_H_M;                     /*Scaler denominator of horizontal scale fator
									  范围：[1 8]
									  */
	int_32  Prv_V_N ;                    /*Scaler numerator of Vertical scale fator
									  范围：[1 8]
									  */
	int_32  Prv_V_M;                     /*Scaler denominator of Vertical scale fator
									  范围：[1 8]
									  */
	int_32  PrvScalerTLX;                /*预览scalar输入图像的左上坐标的X。宏块为单位。*/
	int_32  PrvScalerTLY;                /*预览scalar输入图像的左上坐标的Y。宏块为单位。*/
	int_32  PrvScalerDRX;                /*预览scalar输入图像的右下坐标的X。宏块为单位。*/
	int_32  PrvScalerDRY;                /*预览scalar输入图像的右下坐标的X。宏块为单位。*/
	int_32  OSDRegionCover;              /*区域屏蔽是否启用。暂定两个屏蔽区域。0: off
									  1: on*/
	int_32  RegionCoverMode;             /*屏蔽的格式。
									  0: 黑色
									  1: 马赛克
									  */
	int_32  cover_top_left_x0;           /*屏蔽区域左上角水平坐标X。宏块为单位。*/
	int_32  cover_top_left_y0;           /*屏蔽区域左上角垂直坐标Y。宏块为单位。*/
	int_32  cover_width0;                /*屏蔽区域宽度。宏块为单位。*/
	int_32  cover_height0;               /*屏蔽区域高度。宏块为单位。*/

	int_32  cover_top_left_x1;            /*屏蔽区域左上角水平坐标X。宏块为单位。*/
	int_32  cover_top_left_y1;            /*屏蔽区域左上角垂直坐标Y。宏块为单位。*/
	int_32  cover_width1;                /*屏蔽区域宽度。宏块为单位。*/
	int_32  cover_height1;               /*屏蔽区域高度。宏块为单位。*/
}vpu_cfg_t;

//编码配置数据结构定义
typedef struct
{
	uint_32 flags;                       /*结构体有效标识符*/
	int_32  output_format;               /*输出码流格式 0: ES 2：ASF*/
	int_32  audio_enc_en;                /*视频编码是否有音频编码*/
	int_32  audio_format;                /*音频编码格式*/
	int_32  audio_ch_num;                /*当前编码视频通道对应的音频通道号*/
	int_32  img_width;                   /*输入图像宽*/
	int_32  img_height;                  /*输入图像高*/
	int_32  frame_rate;                  /*编码图象的帧率。取值范围[0, 30]*/
	int_32  crop_enable;                  /*Crop功能使能。*/
	int_32  Enc_img_leftup_x;            /*编码图像起始点的左上坐标X。像素为单位*/
	int_32  Enc_img_leftup_y;            /*编码图像起始点的左上坐标Y。像素为单位*/
	int_32  Enc_img_width;               /*编码图像的宽。像素为单位*/
	int_32  Enc_img_height;              /*编码图像的高。像素为单位*/
	int_32  profile ;                          /*profile 1: baseline, 0: main profile */
	int_32  level_idc;                       /*Level IDC   (e.g. 20 = level 2.0)*/
	int_32  pic_interlace;                 /*编码方式   0: 帧编码1: 场编码; */

	int_32  entropy_mode ;             /*熵编码方法 0: CAVLC 1: CABAC */
	int_32  GOP_length;                  /*GOP长度，I帧之间的间隔 取值范围: [1, 0x7ff]*/

	int_32  b_frame ;                    /*B帧数量 取值范围: [0, 1]*/
	int_32  scene_detect ;               /*是否进行场景检测标志*/
	int_32  x_search_type ;              /*水平搜索范围类型0: [-16: +15] 1: [-24: +23] 2: [-32: +31]*/
	int_32  y_search_type ;              /*垂直搜索范围0: [-16: +15] 1: [-24: +23] 2: [-32: +31]*/
	int_32  ref_number;                  /*参考帧个数取值: 1*/
	int_32  slice_number;                /*支持多slice 一帧里slice个数取值范围 [0, 8]*/

	int_32  loop_filter_disable;        /*环形滤波禁用标志（0：做滤波，1：不做滤波）*/
	int_32  alpha_c0_offset;             /*环形滤波Alpha 偏移值 取值范围[-6, +6]*/
	int_32  beta_offset;                 /*环形滤波Beta 偏移值 取值范围[-6, +6]*/

	int_32  rc_mode;                     /*码率控制方式. 0: 固定QP
									  1:恒定比特率CBR(Constant Bit Rate）
									  2:平均比特率VBR(Variable Bit Rate）
									  */
	int_32  target_bitrate;              /*目标压缩数据码率(单位: kbps)取值范围: [32, 15000] 。CBR和VRB模式有效。*/
	int_32  init_qp;                     /*初始QP (VBR模式的平均QP)取值范围: [0, 51]*/
	int_32  i_qp ;                       /*I帧压缩时的量化系数取值范围: [0, 51].固定QP模式有效。*/
	int_32  p_qp ;                       /*P帧压缩时的量化系数取值范围: [0, 51] .固定QP模式有效。*/
	int_32  b_qp ;                       /*B帧压缩时的量化系数取值范围: [0, 51] .固定QP模式有效。*/

	int_32 IME_type;	/* 	0: IME提供4种最好模式
					1: IME提供1种最好模式
					2: IME提供2种最好模式
					3: IME提供3种最好模式*/

	int_32 FME_type;	/*  	0: FME 1/2,1/4插值模式都作
					1: FME只做1/2插值模式*/

	int_32 fast_intra_mode_type; /*快速Intra模式选择
							  0: 所有模式
							  1: Intra4x4 只做5种(Vertical, Horizontal, DC,  Down_left, Down_right )　做Intra16x16
							  2: Inra4x4 做9种，Intra16x16不做，
							  3:  Intra4x4 只做5种，同时Intra16x16不做
							  4 ：只做Intra16x16*/

	int_32  enable_watermark;            /*视频编码中是否有水印*/
	int_8  watermark_info[256];        /*要写入的水印信息内容*/
	int_32  Generate_VUI;                   /*是否使用vui信息 0：不使用，1：使用*/
	int_32 sar_height;                         /*sample aspect ratio（SAR）宽*/
	int_32 sar_width;                          /*SAR高*/

	int_32  sub_channel_en;                /*是否进行次编码*/
	int_32  sub_output_format;           /*次输出码流格式 0: ES PES PS: TS FS */
	int_32  sub_img_width;               /*次编码输入图像宽*/
	int_32  sub_img_height;               /*次编码输入图像高*/
	int_32  sub_frame_rate;                 /*次编码图象的帧率。取值范围[0, 30]*/
	int_32  sub_gop_length;             
	int_32  subEnc_img_leftup_x;          /*次编码图像起始点的左上坐标X。像素为单位*/
	int_32  subEnc_img_leftup_y;         /*次编码图像起始点的左上坐标Y。像素为单位*/
	int_32  subEnc_img_width;            /*次编码图像的宽。像素为单位*/
	int_32  subEnc_img_height;           /*次编码图像的高。像素为单位*/
	int_32  sub_rc_mode;                    /*码率控制方式. 0: 固定QP  1:恒定比特率CBR(Constant Bit Rate）2:可变比特率VBR(Variable Bit Rate)*/
	int_32  sub_target_bitrate;            /*目标压缩数据码率(单位: kbps)取值范围: [32, 15000] 。CBR和VRB模式有效。*/
	int_32  sub_init_qp;                       /*初始QP (VBR模式的平均QP)取值范围: [0, 51]*/
	int_32  sub_i_qp;                           /*I帧压缩时的量化系数取值范围: [0, 51].固定QP模式有效。*/
	int_32  sub_p_qp;                           /*P帧压缩时的量化系数取值范围: [0, 51] .固定QP模式有效。*/
	int_32  sub_b_qp;                          /*B帧压缩时的量化系数取值范围: [0, 51] .固定QP模式有效。*/


}enc_cfg_t;

//报警配置数据结构定义
typedef struct
{
	uint_32 flags;                   /*结构体有效标识符*/
	int_32  Motion_Detection;            /*	运动检测报警启动控制
									  0: off
									  1: on
									  */
	int_32  MD_ref_dist;                 /*运动检测间隔取值范围： [5, 60]*/
	int_32  md_gate_level;               /*Cell运动判断门限，取值范围： [1, 255]*/
	int_32  Blind_Detection;             /*遮挡报警启动控制
									  0:off
									  1:on
									  */
	int_32  Blind_sens;                  /*遮挡报警敏感度。取值范围： [1, 255]*/
	int_32  Blind_area_per;              /*遮挡报警区域占整幅图像百分比，取值范围： [1, 100]  */
	int_32  no_video_alarm_en;           /*无视频输入报警启动控制
									  0: off
									  1: on
									  */
}alarm_cfg_t;

//配置参数头结构定义
typedef struct
{
	unsigned int	size;					// 配置参数结构体的长度（包括usr_cfg_head_t的长度）
	unsigned int   cksum;                  //checksum of the whole parameter area, see function cksum_calc
	unsigned int	type;					// 0:	配置Chip
	// 1:	配置Channel
	// 2:   配置Audio 数据
	int_8			name[16];				// 配置参数名称
	int_8			desc[64];				// 配置参数描述
	unsigned int	flags;					// 有效标志
}usr_cfg_head_t;

/* for picture crop function */
typedef struct{
	int_32 x_crop_en;         
	int_32 x_start;			 
	int_32 x_size_in_mb;     
	int_32 y_crop_en;        
	int_32 y_start;          
	int_32 y_size_in_mb;	 
}vi_crop_attr_t;

/* for video input drop frame function */
typedef struct{
	int_32 drop_mask;      /* 0x1 ~ 0x3FFFFFFF Bit 1: Receive, 0: Drop. */
	int_32 drop_mask_len;  /* Limited [1 ~  30] */
}vi_drop_mask_attr_t;

typedef struct {
	/* 0: 内同步， 1： 外同步 */
#define VI_HD_SYNC_INT (0)
#define VI_HD_SYNC_EXT (1)
	int_32 sync_mode;

#define VI_HD_16BIT_BT656 (0)
#define VI_HD_16BIT_YCBCR (1)
#define VI_HD_24BIT_YCBCR (2)
#define VI_HD_24BIT_RGB	  (3)
	int_32 crin_mode;

	/*0=CbCr同时采样，1=CbCr交错采样*/
#define VI_HD_SAMPLE_CBCR_SAM (0) 
#define VI_HD_SAMPLE_CBCR_INT (1)
	int_32 sample_mode;         

	/*0=NTSC,1=HDTV*/ 
#define VI_HD_RGBMODE_NTSC (0)
#define VI_HD_RGBMODE_HDTV (1)
	int_32 rgb_mode;            

	/*0=studio RGB(16-235)
	  1=computer RGB(0-255)
	*/
#define VI_HD_RGB_RANGE_STUDIO   (0)
#define VI_HD_RGB_RANGE_COMPUTER (1)
	int_32 rgb_range;           
}vi_hd_attr_t;

//视频输入配置数据结构定义
typedef struct 
{
	/* for drop frame function */
	vi_drop_mask_attr_t drop;

	/* for firmware buffer allocation, only for HD */
	uint_32 max_pic_width;
	uint_32 max_pic_height;

	/* for crop picture */
	vi_crop_attr_t crop_info;

	/* other functions. */
#define VI_FEATURE_DISABLE (0)
#define VI_FEATURE_ENABLE  (1)
	uint_32 av_verify;
	uint_32 mode_check;
	uint_32 size_check;

	/* NOTE: current only support FARME mode*/
#define VI_PIC_STORE_IN_FRAME (0)
#define VI_PIC_STORE_IN_FIELD (1)
	uint_32 pic_out_mode;

	uint_32 reserved[2];
}viu_ch_cfg_t;

typedef struct {
	uint_32 sync_mode;   			 /* 0: master 1: slave  */
	uint_32 is_stereo;  			 /* 0: mono   1: stereo */ 
	uint_32 bitwidth;    			 /* 8~32 */
	uint_32 sample_rate; 			 /*	only valid in master mode */
	uint_32 rx_en[8]; 		 /* 0: disable， 1: enable */
	uint_32 fs_mode;                 /* typical : HD_board: 64, SD_board: 256*/
	
	uint_32 polarity[8]; 	 /* stereo 0: left first 1: right first */
                                     /* mono   0: left       1: right */
	
	uint_32 tx_en;			         /* 0: disable  1: enable */
	uint_32 tx_i2s_id; 		         /* 0~MAX_I2S -1,  the tx I2S id */
	uint_32 frame_len;		         /* sample numbers in one audio frame， 0~MAX_AI_FRMAE_LEN，*/ 
} i2s_attr_t;

typedef struct
{
	/*系统配置参数区*/
	/* for video input mode, groupA and groupB, default is EBT656 */
	uint_32 vi_input_mode[2];

	/* only valide for hd mode */
	vi_hd_attr_t hd_attr[2];

	/* default is AD_NULL */
	uint_32 ad_type;      
	uint_32 ddr_num;                 /*DDR存储器个数*/
	uint_32 ddr_cap0;                /*DDR-0存储器容量*/
	uint_32 ddr_cap1;                /*DDR-1存储器容量*/

	uint_32 reserved[4];

	vou_sys_cfg_t   vou_cfg;          /*VOU配置参数，仅在预览为VOU预览时有效*/
	aiu_cfg_t       aiu_cfg ;         /*音频配置参数*/

	uint_8 ASC[0x2100];				    //英文字库
	uint_8 CHN[0xFFA00];				//中文字库
}chip_cfg_t;

typedef struct
{
	viu_ch_cfg_t	viu_cfg;			/*VIU配置参数，有效数量与通道有效数有关*/
	osd_cfg_t		osd_cfg;			/*OSD配置参数*/
	vpu_cfg_t		vpu_cfg;			/*视频预处理配置参数*/
	enc_cfg_t		enc_cfg;			/*编码配置参数*/
	alarm_cfg_t		alarm_cfg;			/* 报警配置参数, NOTE: not support!!! */

}ch_cfg_t;

typedef struct
{
	usr_cfg_head_t  cfg_hdr; /* parameter header */

	chip_cfg_t chip_cfg;

	ch_cfg_t ch_cfg[MAX_CH];

	// [PATCH PROGRAM] [HUENGPEI]
	// 
	i2s_attr_t i2s_cfg;

	int_32     para_addr;

	int_32     para_len;

}sys_cfg_ex_t;

typedef struct
{
// do not change anything---------------
	usr_cfg_head_t  cfg_hdr; /* parameter header */

	chip_cfg_t chip_cfg;

	ch_cfg_t ch_cfg[MAX_CH];

	// [PATCH PROGRAM] [HUENGPEI]
	// 
	i2s_attr_t i2s_cfg;

	int_32 para_addr;

	int_32 para_len;
//--------------------------------
	CSCMatrix  o_csc_matrix;

	CSCMatrix  o_csc_original;

	ULONG n_input_video_resolution_cx;

	ULONG n_input_video_resolution_cy;

	ULONG n_input_video_resolution_fps;

	ULONG      n_input_video_resolution_fps_m;

	ULONG      n_input_video_resolution_interleaved;

	ULONG      n_input_video_resolution_ch_info;

	ULONG      n_input_video_resolution_vbi_lines;

	ULONG      n_input_video_resolution_h_total;

	BOOL    b_input_video_resolution_spliter_mode;

	BOOL	   b_input_video_resolution_external_sync;

	ULONG      b_input_video_resolution_vga_h_period_us;

	ULONG      b_input_video_resolution_vga_v_period_us;

	ULONGLONG	  b_input_video_resolution_vga_h_period_us_sum;

	ULONGLONG	  b_input_video_resolution_vga_v_period_us_sum;

	ULONG      b_input_video_resolution_vga_period_us_counts;

	LONG	   n_input_video_resolution_offset_x_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_y_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_x_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_y_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_index;

	LONG	   n_input_video_resolution_offset_x_start_fetch;

	LONG	   n_input_video_resolution_offset_y_start_fetch;

	ULONG 	   n_input_video_colorspace;

	BOOL    b_input_video_signal_changed;

	ULONG 	   n_input_video_good_adc_phase;

	ULONG 	   n_input_audio_sampling_frequency;

	BYTE	   n_input_audio_adc_register_pages[ 4 ];

	CIT6603    o_input_ite6603;

	ULONG	   n_video_preview_frame_fps_mask;

	ULONG	   n_video_preview_frame_fps_mask_length;

	ULONG	   n_main_H264_count;//number of bytes in AP buffer
	ULONG	   n_main_H264_left;//number of bytes in temp buffer

	ULONG	   n_sub_H264_count;//number of bytes in AP buffer
	ULONG	   n_sub_H264_left;//number of bytes in temp buffer


}sys_cfg_t;

typedef struct
{
	usr_cfg_head_t  cfg_hdr;
	uint_8	cfg_data[0x200000];
}user_cfg_t;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 enc_type;

	uint_32 mask;
	uint_32 mask_len;
}req_enc_drop_mask;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 enc_type;

	uint_32 hcoeff;               	/* 0：原始尺寸 1: 缩小到原始尺寸的3/4 2: 缩小到原始尺寸的1/2 3: 缩小到原始尺寸的1/4 */
	uint_32 vcoeff;
}req_enc_coeff;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 enc_type;

	uint_32 frame_rate;
}req_enc_frame_rate;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 enc_type;

	uint_32 bitrate;
	uint_32 rc_mode;
	uint_32 init_qp;
	uint_32 i_qp;
	uint_32 p_qp;
	uint_32 b_qp;
}req_enc_bitrate;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 enc_type;

	uint_32 gop;
}req_enc_gop;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 DenoiseL_Coeff;
	uint_32 DenoiseC_Coeff;
}req_denoise;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 Deinterlace_mode;
	uint_32 DeintlaceMDThr;
}req_deinterlace;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 osd_txt_en;
	uint_32 brightness;
	uint_32 osd_txt_transparency;
	uint_32 len;
	char    osd_txt[TOSD_LEN];
}req_tosd;

typedef struct {
	uint_32 cmd;
	uint_32 groupAmode;
	uint_32 groupBmode;
}req_input_mode;

typedef struct {
	uint_32 cmd;
	uint_32 group;
	vi_hd_attr_t hd_attr;
}req_hd_mode;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	viu_ch_cfg_t viu_cfg;
}req_channel_vi_config;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	uint_32 PrvScalerMode;
	uint_32 Prv_H_N;
	uint_32 Prv_H_M;
	uint_32 Prv_V_N;
	uint_32 Prv_V_M;
}req_prv_size;

typedef struct {
	uint_32 cmd;
	uint_32 asc_len;
	uint_8  ASC[0x2100];				    //英文字库
	uint_32 chn_len;
	uint_8  CHN[0];				//中文字库
}req_font_lib;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	osd_graph_t osd_graph[2]; 
}req_gosd;

typedef struct {
	uint_32 cmd;
	uint_32 channel;

	int_32 OSDRegionCover;
	int_32 RegionCoverMode;
	int_32 cover_top_left_x0;
	int_32 cover_top_left_y0;
	int_32 cover_width0;
	int_32 cover_height0;
	int_32 cover_top_left_x1;
	int_32 cover_top_left_y1;
	int_32 cover_width1;
	int_32 cover_height1;
}req_region_mask;

typedef struct {
	uint_32 cmd;
	uint_32 channel;

	int_32 Generate_VUI;
	int_32 sar_height;
	int_32 sar_width;
}req_UVI;

typedef struct {
	uint_32 cmd;
	uint_32 enc_type;
	
	uint_32 enc_mask;
}req_enc_ex;

typedef struct {
	uint_32 cmd;
	uint_32 channel;
	
	uint_32 profile;
	uint_32 level_idc;
	uint_32 entropy_mode;
	uint_32 b_frame;
}req_set_enc_config;

#endif /*__sys_cfg_h__*/

