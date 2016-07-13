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


//��Ƶ����������ݽṹ����
/*ע1��vou _win_topleftx+ vou _win_width��Ӧ�ô���D1ͼ��Ŀ�ȣ�720�������򣬳����Ĳ��ֻ��Զ�������
��2��vou _win_toplefty+ vou _win_heigth��Ӧ�ô���D1ͼ��ĸ߶ȣ�PAL��ʽΪ576��NTSC��ʽΪ480�����������ֻ��Զ�������
*/
typedef struct
{
	uint_32 flags;                           /*�ṹ����Ч��ʶ��*/
	int_32  vou_pic_type;                   /*NTSC����PAL��ʽѡ��0��PAL�ƣ�1��NTSC��*/
	int_32  vou_win_topleftx1;           /*��Ƶ�����������ʾ��Ļ�����Ͻǵĺ����ꡣ��4����Ϊ��λ*/
	int_32  vou_win_toplefty2;           /*��Ƶ�����������ʾ��Ļ�����Ͻǵ������ꡣ��4����Ϊ��λ*/
	int_32  vou_win_width;               /*��Ƶ������ڿ���4����Ϊ��λ*/
	int_32  vou_win_heigth;              /*��Ƶ������ڸߡ���4����Ϊ��λ*/
	int_32  background_y;                /*�������ȷ���,ȡֵ��Χ[16, 235]*/
	int_32  background_cb;               /*����ɫ�ȷ���,ȡֵ��Χ[16, 240]*/
	int_32  background_cr;               /*����ɫ�ȷ���,ȡֵ��Χ[16, 240]*/
}vou_sys_cfg_t;

//��Ƶ�����������ݽṹ����
typedef struct
{
	uint_32 flags;                 /*�ṹ����Ч��ʶ��*/
	int_32   input_en[MAX_CH]; /*�Ƿ���Ҫ������Ƶ�ɼ���       0������Ҫ��1����Ҫ*/
	int_32   out_channel;            /*�Ƿ���Ҫ������Ƶ������ݡ�0������Ҫ����Ƶ���ͨ���ţ�CH��1��8*/
	int_32   sample_rate;            /*��Ƶ����������*/
}aiu_cfg_t;

/*��Ƶ�����������ݽṹ����*/
typedef struct
{
	uint_32 flags;                /*�ṹ����Ч��ʶ��*/
	int_32  input_mode;         /*��Ƶ����ģʽ��Stream0��stream7������ģʽ��ͬ��stream8��stream15������ģʽ��ͬ��ֻ��stream0��stream8�Ĵ�ֵ��Ч��
							 0����׼BT.656
							 1����ǿBT.656
							 2��������Ƶ
							 3��TW2815 TM
							 */
	int_32  frame_rate;         /*��Ƶ����֡�ʡ�ȡֵ��Χ��[1, ������Ƶ֡��]*/
	int_32  input_en;             /*ͨ���Ƿ�ѡͨ��0����ѡͨ��1��ѡͨ*/
}_viu_cfg_t;


//ͼ��OSD�������ݽṹ����
typedef struct
{
	int_32  osd_graphic_en;              /*�Ƿ�������ͼ��OSD���ӡ�0����������1������*/
	int_32  osd_graph_top_left_x;        /*ͼ�δ������Ͻ�ˮƽ����X���Ժ��Ϊ��λ��ȡֵ��Χ: [0, 127]*/
	int_32  osd_graph_top_left_y;        /*ͼ�δ������ϽǴ�ֱ����Y���Ժ��Ϊ��λ		��ȡֵ��Χ: [0, 127]*/
	int_32  osd_graph_width;             /*ͼ�δ��ڿ�ȡ��Ժ��Ϊ��λ��ȡֵ��Χ: [0, 127]*/
	int_32  osd_graph_height;            /*ͼ�δ��ڸ߶ȡ��Ժ��Ϊ��λ��ȡֵ��Χ: [0, 127]*/
	int_32  osd_graph_clut_type;         /*����ͼ���͡�2����λ����ͼ1����λ����ͼ0����λ����ͼ*/
	uint_8	index_data[0x18C00];    //����ͼ
	uint_8	lut_data[0x400];         //��ɫ���ұ�
}osd_graph_t;

/*�ı�OSD�������ݽṹ����*/
typedef struct
{
	int_32  osd_txt_en;                  /*����TEXT�������� 0��������1������*/
	int_32  brightness;                  /*�ַ����ȡ�0����ɫ��1����ɫ��2���Զ���ɫ*/
	int_32  scalar;                      /*����Ŵ�ϵ��*/
	int_32  osd_txt_transparency;        /*TEXT��͸���ȡ�0����͸��1��50%͸����*/
	int_32  text_length;                 /*�ı�������ByteΪ��λ�ĳ���*/
	int_32  text_line;                   /*�ı����ݵľ���������*/
	int_32  osd_text_addr;               /*ͼ�񴰿�������Ϣ��ַ*/
	uint_8	data[TOSD_LEN];             //�ı�OSD����
}osd_txt_t;

typedef struct
{
	uint_32 flags;                               /*�ṹ����Ч��ʶ��*/
	osd_txt_t   osd_txt;                    /*��һ��TOSD*/
	osd_graph_t osd_graph[2];         /*VPU��������GOSD*/
	
}osd_cfg_t;

//��ƵԤ�����������ݽṹ����
typedef struct
{
	uint_32 flags;                       /*�ṹ����Ч��ʶ��*/
	int_32  Deinterlace_mode;            /*Deinterlace algorithm select 0: Off, 1: Median, 2: Enhanced Median*/
	int_32  DeintlaceMDThr;              /*Deinterlace motion detection threshold. ȡֵ��Χ[1:60]*/

	int_32  DenoiseL_Coeff ;
	/*����ȥ���㷨ѡ��0:off
	Y����ȥ���˲�ǿ��
	0�����˲�
	1�������˲���1��14��1��
	2�����˲���1��6��1��
	3��ǿ�˲���1��2��1��
	*/

	int_32  DenoiseC_Coeff ;
	/*ɫ��ȥ���㷨ѡ��0:off
	UV����ȥ���˲�ǿ��
	0�����˲�
	1�������˲���1��14��1��
	2�����˲���1��6��1��
	3��ǿ�˲���1��2��1��
	*/

	int_32  MainScaler;                  /*������scalar���enable 0: off 1: on */
	int_32  MScalerPictType;             /*������scalar�����ʽ�� 0: Frame  1: Field */
	int_32  HCoeff;                      /*horizontal scale fator 0��ԭʼ�ߴ� 1: ��С��ԭʼ�ߴ��3/4 2: ��С��ԭʼ�ߴ��1/2 3: ��С��ԭʼ�ߴ��1/4*/
	int_32  VCoeff;                      /*Vertical scale fator
									  0��ԭʼ�ߴ�
									  1: ��С��ԭʼ�ߴ��3/4
									  2: ��С��ԭʼ�ߴ��1/2
									  3: ��С��ԭʼ�ߴ��1/4    */
	int_32  AuxScaler;                   /*�α������enable
									  0: off
									  1: on
									  */
	int_32  Aux_HCoeff;                  /*horizontal scale fator
									  0��ԭʼ�ߴ�
									  1: ��С��ԭʼ�ߴ��3/4
									  2: ��С��ԭʼ�ߴ��1/2
									  3: ��С��ԭʼ�ߴ��1/4
									  */
	int_32  Aux_VCoeff;                  /*Vertical scale fator
									  0��ԭʼ�ߴ�
									  1: ��С��ԭʼ�ߴ��3/4
									  2: ��С��ԭʼ�ߴ��1/2
									  3: ��С��ԭʼ�ߴ��1/4
									  */
	int_32  PrvScaler;                   /*Ԥ�����enable
									  0: off
									  1: on
									  */
#define PRV_420 0
#define PRV_422 1
	int_32  PrvPicType;                      /*Ԥ�����ģʽ��
												0: 422ģʽ
												1: 420ģʽ
												*/
	int_32  PScalerPictType;             /*Ԥ�����֡��ģʽѡ��
									  0: Frame
									  1: Field
									  */
	int_32  PrvScalerMode;               /*Ԥ��scalarģʽѡ��
									  0: Mean
									  1: Cubic + Linear
									  */
	int_32  Prv_H_N;                     /*Scaler numerator of horizontal scale fator
									  ��Χ��[1 8]
									  */
	int_32  Prv_H_M;                     /*Scaler denominator of horizontal scale fator
									  ��Χ��[1 8]
									  */
	int_32  Prv_V_N ;                    /*Scaler numerator of Vertical scale fator
									  ��Χ��[1 8]
									  */
	int_32  Prv_V_M;                     /*Scaler denominator of Vertical scale fator
									  ��Χ��[1 8]
									  */
	int_32  PrvScalerTLX;                /*Ԥ��scalar����ͼ������������X�����Ϊ��λ��*/
	int_32  PrvScalerTLY;                /*Ԥ��scalar����ͼ������������Y�����Ϊ��λ��*/
	int_32  PrvScalerDRX;                /*Ԥ��scalar����ͼ������������X�����Ϊ��λ��*/
	int_32  PrvScalerDRY;                /*Ԥ��scalar����ͼ������������X�����Ϊ��λ��*/
	int_32  OSDRegionCover;              /*���������Ƿ����á��ݶ�������������0: off
									  1: on*/
	int_32  RegionCoverMode;             /*���εĸ�ʽ��
									  0: ��ɫ
									  1: ������
									  */
	int_32  cover_top_left_x0;           /*�����������Ͻ�ˮƽ����X�����Ϊ��λ��*/
	int_32  cover_top_left_y0;           /*�����������ϽǴ�ֱ����Y�����Ϊ��λ��*/
	int_32  cover_width0;                /*���������ȡ����Ϊ��λ��*/
	int_32  cover_height0;               /*��������߶ȡ����Ϊ��λ��*/

	int_32  cover_top_left_x1;            /*�����������Ͻ�ˮƽ����X�����Ϊ��λ��*/
	int_32  cover_top_left_y1;            /*�����������ϽǴ�ֱ����Y�����Ϊ��λ��*/
	int_32  cover_width1;                /*���������ȡ����Ϊ��λ��*/
	int_32  cover_height1;               /*��������߶ȡ����Ϊ��λ��*/
}vpu_cfg_t;

//�����������ݽṹ����
typedef struct
{
	uint_32 flags;                       /*�ṹ����Ч��ʶ��*/
	int_32  output_format;               /*���������ʽ 0: ES 2��ASF*/
	int_32  audio_enc_en;                /*��Ƶ�����Ƿ�����Ƶ����*/
	int_32  audio_format;                /*��Ƶ�����ʽ*/
	int_32  audio_ch_num;                /*��ǰ������Ƶͨ����Ӧ����Ƶͨ����*/
	int_32  img_width;                   /*����ͼ���*/
	int_32  img_height;                  /*����ͼ���*/
	int_32  frame_rate;                  /*����ͼ���֡�ʡ�ȡֵ��Χ[0, 30]*/
	int_32  crop_enable;                  /*Crop����ʹ�ܡ�*/
	int_32  Enc_img_leftup_x;            /*����ͼ����ʼ�����������X������Ϊ��λ*/
	int_32  Enc_img_leftup_y;            /*����ͼ����ʼ�����������Y������Ϊ��λ*/
	int_32  Enc_img_width;               /*����ͼ��Ŀ�����Ϊ��λ*/
	int_32  Enc_img_height;              /*����ͼ��ĸߡ�����Ϊ��λ*/
	int_32  profile ;                          /*profile 1: baseline, 0: main profile */
	int_32  level_idc;                       /*Level IDC   (e.g. 20 = level 2.0)*/
	int_32  pic_interlace;                 /*���뷽ʽ   0: ֡����1: ������; */

	int_32  entropy_mode ;             /*�ر��뷽�� 0: CAVLC 1: CABAC */
	int_32  GOP_length;                  /*GOP���ȣ�I֮֡��ļ�� ȡֵ��Χ: [1, 0x7ff]*/

	int_32  b_frame ;                    /*B֡���� ȡֵ��Χ: [0, 1]*/
	int_32  scene_detect ;               /*�Ƿ���г�������־*/
	int_32  x_search_type ;              /*ˮƽ������Χ����0: [-16: +15] 1: [-24: +23] 2: [-32: +31]*/
	int_32  y_search_type ;              /*��ֱ������Χ0: [-16: +15] 1: [-24: +23] 2: [-32: +31]*/
	int_32  ref_number;                  /*�ο�֡����ȡֵ: 1*/
	int_32  slice_number;                /*֧�ֶ�slice һ֡��slice����ȡֵ��Χ [0, 8]*/

	int_32  loop_filter_disable;        /*�����˲����ñ�־��0�����˲���1�������˲���*/
	int_32  alpha_c0_offset;             /*�����˲�Alpha ƫ��ֵ ȡֵ��Χ[-6, +6]*/
	int_32  beta_offset;                 /*�����˲�Beta ƫ��ֵ ȡֵ��Χ[-6, +6]*/

	int_32  rc_mode;                     /*���ʿ��Ʒ�ʽ. 0: �̶�QP
									  1:�㶨������CBR(Constant Bit Rate��
									  2:ƽ��������VBR(Variable Bit Rate��
									  */
	int_32  target_bitrate;              /*Ŀ��ѹ����������(��λ: kbps)ȡֵ��Χ: [32, 15000] ��CBR��VRBģʽ��Ч��*/
	int_32  init_qp;                     /*��ʼQP (VBRģʽ��ƽ��QP)ȡֵ��Χ: [0, 51]*/
	int_32  i_qp ;                       /*I֡ѹ��ʱ������ϵ��ȡֵ��Χ: [0, 51].�̶�QPģʽ��Ч��*/
	int_32  p_qp ;                       /*P֡ѹ��ʱ������ϵ��ȡֵ��Χ: [0, 51] .�̶�QPģʽ��Ч��*/
	int_32  b_qp ;                       /*B֡ѹ��ʱ������ϵ��ȡֵ��Χ: [0, 51] .�̶�QPģʽ��Ч��*/

	int_32 IME_type;	/* 	0: IME�ṩ4�����ģʽ
					1: IME�ṩ1�����ģʽ
					2: IME�ṩ2�����ģʽ
					3: IME�ṩ3�����ģʽ*/

	int_32 FME_type;	/*  	0: FME 1/2,1/4��ֵģʽ����
					1: FMEֻ��1/2��ֵģʽ*/

	int_32 fast_intra_mode_type; /*����Intraģʽѡ��
							  0: ����ģʽ
							  1: Intra4x4 ֻ��5��(Vertical, Horizontal, DC,  Down_left, Down_right )����Intra16x16
							  2: Inra4x4 ��9�֣�Intra16x16������
							  3:  Intra4x4 ֻ��5�֣�ͬʱIntra16x16����
							  4 ��ֻ��Intra16x16*/

	int_32  enable_watermark;            /*��Ƶ�������Ƿ���ˮӡ*/
	int_8  watermark_info[256];        /*Ҫд���ˮӡ��Ϣ����*/
	int_32  Generate_VUI;                   /*�Ƿ�ʹ��vui��Ϣ 0����ʹ�ã�1��ʹ��*/
	int_32 sar_height;                         /*sample aspect ratio��SAR����*/
	int_32 sar_width;                          /*SAR��*/

	int_32  sub_channel_en;                /*�Ƿ���дα���*/
	int_32  sub_output_format;           /*�����������ʽ 0: ES PES PS: TS FS */
	int_32  sub_img_width;               /*�α�������ͼ���*/
	int_32  sub_img_height;               /*�α�������ͼ���*/
	int_32  sub_frame_rate;                 /*�α���ͼ���֡�ʡ�ȡֵ��Χ[0, 30]*/
	int_32  sub_gop_length;             
	int_32  subEnc_img_leftup_x;          /*�α���ͼ����ʼ�����������X������Ϊ��λ*/
	int_32  subEnc_img_leftup_y;         /*�α���ͼ����ʼ�����������Y������Ϊ��λ*/
	int_32  subEnc_img_width;            /*�α���ͼ��Ŀ�����Ϊ��λ*/
	int_32  subEnc_img_height;           /*�α���ͼ��ĸߡ�����Ϊ��λ*/
	int_32  sub_rc_mode;                    /*���ʿ��Ʒ�ʽ. 0: �̶�QP  1:�㶨������CBR(Constant Bit Rate��2:�ɱ������VBR(Variable Bit Rate)*/
	int_32  sub_target_bitrate;            /*Ŀ��ѹ����������(��λ: kbps)ȡֵ��Χ: [32, 15000] ��CBR��VRBģʽ��Ч��*/
	int_32  sub_init_qp;                       /*��ʼQP (VBRģʽ��ƽ��QP)ȡֵ��Χ: [0, 51]*/
	int_32  sub_i_qp;                           /*I֡ѹ��ʱ������ϵ��ȡֵ��Χ: [0, 51].�̶�QPģʽ��Ч��*/
	int_32  sub_p_qp;                           /*P֡ѹ��ʱ������ϵ��ȡֵ��Χ: [0, 51] .�̶�QPģʽ��Ч��*/
	int_32  sub_b_qp;                          /*B֡ѹ��ʱ������ϵ��ȡֵ��Χ: [0, 51] .�̶�QPģʽ��Ч��*/


}enc_cfg_t;

//�����������ݽṹ����
typedef struct
{
	uint_32 flags;                   /*�ṹ����Ч��ʶ��*/
	int_32  Motion_Detection;            /*	�˶���ⱨ����������
									  0: off
									  1: on
									  */
	int_32  MD_ref_dist;                 /*�˶������ȡֵ��Χ�� [5, 60]*/
	int_32  md_gate_level;               /*Cell�˶��ж����ޣ�ȡֵ��Χ�� [1, 255]*/
	int_32  Blind_Detection;             /*�ڵ�������������
									  0:off
									  1:on
									  */
	int_32  Blind_sens;                  /*�ڵ��������жȡ�ȡֵ��Χ�� [1, 255]*/
	int_32  Blind_area_per;              /*�ڵ���������ռ����ͼ��ٷֱȣ�ȡֵ��Χ�� [1, 100]  */
	int_32  no_video_alarm_en;           /*����Ƶ���뱨����������
									  0: off
									  1: on
									  */
}alarm_cfg_t;

//���ò���ͷ�ṹ����
typedef struct
{
	unsigned int	size;					// ���ò����ṹ��ĳ��ȣ�����usr_cfg_head_t�ĳ��ȣ�
	unsigned int   cksum;                  //checksum of the whole parameter area, see function cksum_calc
	unsigned int	type;					// 0:	����Chip
	// 1:	����Channel
	// 2:   ����Audio ����
	int_8			name[16];				// ���ò�������
	int_8			desc[64];				// ���ò�������
	unsigned int	flags;					// ��Ч��־
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
	/* 0: ��ͬ���� 1�� ��ͬ�� */
#define VI_HD_SYNC_INT (0)
#define VI_HD_SYNC_EXT (1)
	int_32 sync_mode;

#define VI_HD_16BIT_BT656 (0)
#define VI_HD_16BIT_YCBCR (1)
#define VI_HD_24BIT_YCBCR (2)
#define VI_HD_24BIT_RGB	  (3)
	int_32 crin_mode;

	/*0=CbCrͬʱ������1=CbCr�������*/
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

//��Ƶ�����������ݽṹ����
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
	uint_32 rx_en[8]; 		 /* 0: disable�� 1: enable */
	uint_32 fs_mode;                 /* typical : HD_board: 64, SD_board: 256*/
	
	uint_32 polarity[8]; 	 /* stereo 0: left first 1: right first */
                                     /* mono   0: left       1: right */
	
	uint_32 tx_en;			         /* 0: disable  1: enable */
	uint_32 tx_i2s_id; 		         /* 0~MAX_I2S -1,  the tx I2S id */
	uint_32 frame_len;		         /* sample numbers in one audio frame�� 0~MAX_AI_FRMAE_LEN��*/ 
} i2s_attr_t;

typedef struct
{
	/*ϵͳ���ò�����*/
	/* for video input mode, groupA and groupB, default is EBT656 */
	uint_32 vi_input_mode[2];

	/* only valide for hd mode */
	vi_hd_attr_t hd_attr[2];

	/* default is AD_NULL */
	uint_32 ad_type;      
	uint_32 ddr_num;                 /*DDR�洢������*/
	uint_32 ddr_cap0;                /*DDR-0�洢������*/
	uint_32 ddr_cap1;                /*DDR-1�洢������*/

	uint_32 reserved[4];

	vou_sys_cfg_t   vou_cfg;          /*VOU���ò���������Ԥ��ΪVOUԤ��ʱ��Ч*/
	aiu_cfg_t       aiu_cfg ;         /*��Ƶ���ò���*/

	uint_8 ASC[0x2100];				    //Ӣ���ֿ�
	uint_8 CHN[0xFFA00];				//�����ֿ�
}chip_cfg_t;

typedef struct
{
	viu_ch_cfg_t	viu_cfg;			/*VIU���ò�������Ч������ͨ����Ч���й�*/
	osd_cfg_t		osd_cfg;			/*OSD���ò���*/
	vpu_cfg_t		vpu_cfg;			/*��ƵԤ�������ò���*/
	enc_cfg_t		enc_cfg;			/*�������ò���*/
	alarm_cfg_t		alarm_cfg;			/* �������ò���, NOTE: not support!!! */

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

	uint_32 hcoeff;               	/* 0��ԭʼ�ߴ� 1: ��С��ԭʼ�ߴ��3/4 2: ��С��ԭʼ�ߴ��1/2 3: ��С��ԭʼ�ߴ��1/4 */
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
	uint_8  ASC[0x2100];				    //Ӣ���ֿ�
	uint_32 chn_len;
	uint_8  CHN[0];				//�����ֿ�
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

