///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <cat6023.h>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2010/08/10
//   @fileversion: CAT6023_SRC_1.15
//******************************************/
#ifndef IT6603_H
#define IT6603_H

#include "LINUXV4L2.h"

typedef BOOLEAN BOOL;

typedef struct _REGPAIR {
    BYTE ucAddr ;
    BYTE ucValue ;
} REGPAIR ;

#ifdef GET_PACKAGE
#undef GET_PACKAGE
#endif

#ifdef OUTPUT_YUV444
#undef OUTPUT_YUV444
#endif

#ifdef OUTPUT_YUV422
#undef OUTPUT_YUV422
#endif

#ifdef OUTPUT_RGB444
#undef OUTPUT_RGB444
#endif

#ifdef OUTPUT_RGB
#undef OUTPUT_RGB
#endif

#ifdef OUTPUT_YUV
#undef OUTPUT_YUV
#endif

#define OUTPUT_16BIT_YUV422 1//YUAN SC510

#define GET_PACKAGE
#define OUTPUT_YUV444
#define OUTPUT_YUV422
#define OUTPUT_RGB444
#define _HBR_I2S_

#if (defined OUTPUT_YUV444)||(defined OUTPUT_YUV422)
#define OUTPUT_YUV

#pragma message("Ouptut is YUV Format.")
#endif

#if defined OUTPUT_RGB444
#define OUTPUT_RGB
#pragma message("Output is RGB Format.")
#endif

#define HDMI_RX_I2C_SLAVE_ADDR 0x90

#define REG_RX_VID_L	0x00
#define REG_RX_VID_H	0x01
#define REG_RX_DEVID_L	0x02
#define REG_RX_DEVID_H	0x03
#define REG_RX_DEVREV	0x04
#define REG_RX_RST_CTRL	0x05

#define B_CDRRST        (1<<7)
#define B_EN_CDRAUTORST (1<<6)
#define B_EN_AUTOVDORST (1<<5)
#define B_REGRST        (1<<4)
#define B_HDCPRST       (1<<3)
#define B_AUDRST        (1<<2)
#define B_VDORST        (1<<1)
#define B_SWRST         (1<<0)

#define REG_RX_PWD_CTRL0	0x06
#define B_PWD_ALL       (1<<0)

#define REG_RX_PWD_CTRL1	0x07
#define B_PORT_SEL_B      (1<<4)
#define B_PORT_SEL_A      (0<<4)
#define B_PORT_MASK     (1<<7)
#define B_PWD_AFEALL	(1<<3)
#define B_PWDC_ETC	(1<<2)
#define B_PWDC_SRV	(1<<1)
#define B_EN_AUTOPWD	(1<<0)

#define REG_RX_VIO_CTRL	0x08
#define REG_RX_AIO_CTRL	0x09
#define REG_RX_OTP_CTRL	0x0A
#define REG_RX_OTP_XOR	0x0B
#define REG_RX_BIST_CTRL	0x0C
#define REG_RX_BIST_RESULT1	0x0D
#define REG_RX_BIST_RESULT2	0x0E
#define REG_RX_BLOCK_SEL	0x0F

#define REG_RX_BANK 0x0F

#define REG_RX_SYS_STATE	0x10

#define B_RXPLL_LOCK (1<<7)
#define B_RXCK_SPEED (1<<6)
#define B_RXCK_VALID (1<<5)
#define B_HDMIRX_MODE (1<<4)

#define B_SCDT (1<<2)
#define B_VCLK_DET (1<<1)
#define B_PWR5V_DET (1<<0)
#define B_PWR5V_DET_PORTA (1<<0)
#define B_PWR5V_DET_PORTB (1<<3)

#define REG_RX_HDCP_CTRL	0x11
    #define B_EXTROM     (1<<7)
    #define B_HDCP_STABLECNT     (1<<5)
    #define M_HDCP_STABLECNT     (1<<5)
    #define B_HDCP_RDKSV     (1<<4)
    #define B_HDCP_ROMDISWR     (1<<3)
    #define B_HDCP_A0     (1<<2)
    #define B_FASTHDCPMODE     (1<<1)
    #define B_HDCP_EN     (1<<0)

#define REG_RX_HDCP_STATUS	0x12
#define B_R0_READY	(1<<6)
#define REG_RX_INTERRUPT1	0x13
#define REG_RX_INTERRUPT_MASK1	0x16

#define B_VIDMODE_CHG   (1<<5)
#define B_HDMIMODE_CHG  (1<<4)
#define B_SCDTOFF       (1<<3)
#define B_SCDTON        (1<<2)
#define B_PWR5VOFF      (1<<1)
#define B_PWR5VON       (1<<0)

#define REG_RX_INTERRUPT2	0x14
#define REG_RX_INTERRUPT_MASK2	0x17
#define B_PKT_CLR_MUTE (1<<7)
#define B_NEW_AUD_PKT (1<<6)
#define B_NEW_ACP_PKT (1<<5)
#define B_NEW_SPD_PKT (1<<4)
#define B_NEW_MPG_PKG (1<<3)
#define B_NEW_AVI_PKG (1<<2)
#define B_NO_AVI_RCV (1<<1)
#define B_PKT_SET_MUTE (1<<0)

#define REG_RX_INTERRUPT3	0x15
#define REG_RX_INTERRUPT_MASK3	0x18
#define B_AUTOAUDMUTE (1<<4)
#define B_AUDFIFOERR (1<<3)
#define B_ECCERR (1<<2)
#define B_R_AUTH_DONE (1<<1)
#define B_R_AUTH_START (1<<0)

#define REG_RX_INTERRUPT4 0x8B
#define B_RXCK_CHG      (1<<7)
#define B_RXCKON_DET      (1<<6)
#define B_HDCP_OFF      (1<<5)

#define REG_RX_INTERRUPT_MASK4 0x8C
#define B_M_RXCKON_DET      (1<<5)
#define B_M_HDCP_OFF      (1<<6)

#define B_CD_DET (1<<4)
#define B_GENPKT_DET (1<<2)
#define B_ISRC2_DET (1<<1)
#define B_ISRC1_DET 1

#define B_SYM_ERR (1<<3)

#define REG_RX_INTERRUPT_CTRL1	0x18
#define B_CLR_HDCP_INT  (1<<7)

#define REG_RX_INTERRUPT_CTRL	0x19

#define	B_CLR_MODE_INT	(1<<0)
#define	B_CLR_PKT_INT	(1<<1)
#define	B_CLR_ECC_INT	(1<<2)
#define	B_CLR_AUDIO_INT	(1<<3)
#define	B_INTPOL	    (1<<4)
#define	B_INTROUTTYPE	(1<<5)
#define B_CLR_MUTECLR_INT (1<<6)
#define B_CLR_MUTESET_INT (1<<7)

#define LO_ACTIVE TRUE
#define HI_ACTIVE FALSE

#define REG_RX_MISC_CTRL	0x1A

#define B_RXCK_FILT (1<<5)
#define B_TIMEOUT_EN (1<<4)
#define B_EN_DEBUG (1<<3)
#define B_DE_POL (1<<2)
#define B_VSYNC_OUT_POL (1<<1)
#define B_HSYNC_OUT_POL (1<<0)

#define REG_RX_VIDEO_MAP	0x1B
#define B_OUTPUT_16BIT (1<<5)
#define B_SWAP_CH422 (1<<4)
#define B_SWAP_OUTRB (1<<3)
#define B_SWAP_ML (1<<2)
#define B_SWAP_POL (1<<1)
#define B_SWAP_RB (1<<0)

#define REG_RX_VIDEO_CTRL1	0x1C
#define B_RX_DNFREE_GO (1<<7)
#define B_SYNC_EMBEDDED (1<<6)
#define B_RX_EN_DITHER (1<<5)
#define B_RX_EN_UDFILTER (1<<4)
#define B_DDR_OUTPUT (1<<3)
#define B_CCIR565    (1<<2)
#define B_565FFRST   (1<<1)
#define B_EN_AVMUTE_RST (1<<0)

#define REG_RX_VCLK_CTRL	0x1D
#define REG_RX_I2CIO_CTRL	0x1E
#define REG_RX_REGPKTFLAG_CTRL	0x1F
#define B_INT_EVERYAVI	1

#define REG_RX_CSC_CTRL	0x20
#define B_VDIO_GATTING   (1<<7)
#define B_VDIO_DISABLE   (1<<6)
#define B_VIO_SEL       (1<<5)

#define B_FORCE_COLOR_MODE (1<<4)
#define O_INPUT_COLOR_MODE 2
#define M_INPUT_COLOR_MASK 3
#define B_INPUT_RGB24      0
#define B_INPUT_YUV422     1
#define B_INPUT_YUV444     2

#define O_CSC_SEL          0
#define M_CSC_SEL_MASK     3
#define B_CSC_BYPASS       0
#define B_CSC_RGB2YUV      2
#define B_CSC_YUV2RGB      3

#define REG_RX_CSC_YOFF	0x21
#define REG_RX_CSC_COFF	0x22
#define REG_RX_CSC_RGBOFF	0x23
#define REG_RX_CSC_MTX11_L	0x24
#define REG_RX_CSC_MTX11_H	0x25
#define REG_RX_CSC_MTX12_L	0x26
#define REG_RX_CSC_MTX12_H	0x27
#define REG_RX_CSC_MTX13_L	0x28
#define REG_RX_CSC_MTX13_H	0x29
#define REG_RX_CSC_MTX21_L	0x2A
#define REG_RX_CSC_MTX21_H	0x2B
#define REG_RX_CSC_MTX22_L	0x2C
#define REG_RX_CSC_MTX22_H	0x2D
#define REG_RX_CSC_MTX23_L	0x2E
#define REG_RX_CSC_MTX23_H	0x2F
#define REG_RX_CSC_MTX31_L	0x30
#define REG_RX_CSC_MTX31_H	0x31
#define REG_RX_CSC_MTX32_L	0x32
#define REG_RX_CSC_MTX32_H	0x33
#define REG_RX_CSC_MTX33_L	0x34
#define REG_RX_CSC_MTX33_H	0x35

#define REG_RX_FRAMEPKT_ERR 0x36
#define REG_RX_SKNDLY       0x37
#define REG_RX_RXPLL2_LOCK  0x38
#define REG_RX_OUTBIT       0x39
#define REG_RX_AUTO_EQ_CTRL 0x3A
#define REG_RX_DESKEW_CTRL  0x3B

#define REG_RX_PG_CTRL1	0x3C
#define REG_RX_PG_CTRL2	0x3D

#define O_OUTPUT_COLOR_MODE 6
#define M_OUTPUT_COLOR_MASK 3
#define B_OUTPUT_RGB24      0
#define B_OUTPUT_YUV422     1
#define B_OUTPUT_YUV444     2

#define REG_RX_PG_COLR	0x3E
#define REG_RX_PG_COLG	0x3F
#define REG_RX_PG_COLB	0x40
#define REG_RX_PG_COLBLANK	0x41
#define REG_RX_PG_COLBLANKY	0x42
#define REG_RX_PG_HACTST_L	0x43
#define REG_RX_PG_HACTST_H	0x44
#define REG_RX_PG_HACTED_H	0x45
#define REG_RX_PG_VACTST_L	0x46
#define REG_RX_PG_VACTST_H	0x47
#define REG_RX_PG_VACTED_H	0x48
#define REG_RX_PG_VACTST2ND_L	0x49
#define REG_RX_PG_VACTST2ND_H	0x4A
#define REG_RX_PG_VACTED2ND_H	0x4B
#define REG_RX_PG_HTOTAL_L	0x4C
#define REG_RX_PG_HTOTAL_H	0x4D
#define REG_RX_PG_HSYNCST_H	0x4E
#define REG_RX_PG_HSYNCED_L	0x4F
#define REG_RX_PG_VTOTAL_L	0x50
#define REG_RX_PG_VTOTAL_H	0x51
#define REG_RX_PG_VSYNCST_H	0x52
#define REG_RX_PG_VSYNCED	0x53
#define REG_RX_PG_CHINC	0x54
#define REG_RX_PG_CVINC	0x55

#define REG_RX_I2C_CTRL    0x57
#define B_FILT_TAP      (1<<5)
#define B_FILT_TYPE     (1<<4)
#define B_DEGLITCH      (1<<3)
#define B_CMD_FILTTAP   (1<<2)
#define B_CMD_FILTTYPE  (1<<1)
#define B_CMD_DEGLITCH  (1<<0)

#define REG_RX_VID_MODE    0x58
#define B_PX_VIDEOSTABLE (1<<3)
#define B_VIDFIELD       (1<<2)
#define B_INTERLACE      (1<<1)

#define REG_RX_VID_HTOTAL_L     0x59
#define REG_RX_VID_HTOTAL_H     0x5A

#define O_HTOTAL_H 0
#define M_HTOTAL_H 0xF
#define O_HACT_H   4
#define M_HACT_H   0x7

#define REG_RX_VID_HACT_L       0x5B

#define REG_RX_VID_HSYNC_WID_L  0x5C
#define REG_RX_VID_HSYNC_WID_H  0x5D

#define O_HSYNC_WID_H 0
#define M_HSYNC_WID_H 1
#define O_H_FT_PORCH 4
#define M_H_FT_PORCH		3

#define REG_RX_VID_H_FT_PORCH_L 0x5E
#define REG_RX_VID_VTOTAL_L     0x5F
#define REG_RX_VID_VTOTAL_H     0x60
#define O_VTOTAL_H 0
#define M_VTOTAL_H 0xF
#define O_VACT_H   4
#define M_VACT_H   0x7

#define REG_RX_VID_VACT_L       0x61
#define REG_RX_VID_VSYNC2DE     0x62
#define REG_RX_VID_V_FT_PORCH   0x63
#define REG_RX_VID_XTALCNT_128PEL 0x64
#define REG_RX_VID_INPUT_ST     0x65

#define O_PIXEL_REPEAT 4
#define M_PIXEL_REPEAT 0xF
#define B_AVMUTE (1<<2)
#define B_VSYNC_IN_POL (1<<1)
#define B_HSYNC_IN_POL (1<<0)

#define REG_RX_DEBUG_CTRL	0x67
#define REG_RX_PLL_CTRL	0x68
#define REG_RX_TERM_CTRL1	0x69
#define REG_RX_TERM_CTRL2	0x6A

#define REG_RX_EQUAL_CTRL1	0x6B
#define REG_RX_EQUAL_CTRL2	0x6C
#define REG_RX_DES_CTRL1	0x6D
#define REG_RX_DES_CTRL2	0x6E
#define REG_RX_DES_PHASE_B	0x6F
#define REG_RX_DES_PHASE_G	0x70
#define REG_RX_DES_PHASE_R	0x71
#define REG_RX_BCH_MODE	0x72
#define O_BCH_THRSH 3
#define M_BCH_THRSH ((1<<(6-3))-1)

#define B_TERC_INV (1<<2)
#define B_TERC_CHECK (1<<1)
#define B_DIS_BCH (1<<0)

#define REG_RX_CDEPTH_CTRL	0x73
#define B_EN_DefPhase 			(1<<0)
#define B_DIS_DEEP_MODE 		(1<<1)
#define B_SEL_CD_DEBUG 			(1<<2)
#define B_RSTCD 				(1<<3)
#define B_ENABLE_FAST 			(1<<4)
#define B_ENABLE_FEATURE_1P1 	(1<<5)
#define B_KSV_READY 			(1<<6)
#define B_ENABLE_REPEATER 		(1<<7)

#define REG_RX_APLL_CTRL	0x74
#define B_APLL_ENI2A (1<<7)
#define B_APLL_EC1 (1<<6)
#define B_APLL_ENIA (1<<5)
#define B_RSVED (1<<4)
#define B_DIEN (1<<3)
#define B_DIS_SDM (1<<2)
#define B_APLL_REFSW (1<<1)
#define B_APLL_AUTOSW (1<<0)

#define REG_RX_I2S_CTRL	0x75

#define REG_RX_I2S_MAP	0x76
#define O_I2S_CH3SEL 6
#define O_I2S_CH2SEL 4
#define O_I2S_CH1SEL 2
#define O_I2S_CH0SEL 0

#define M_I2S_CHSEL 3
#define B_I2S_CHSEL0 0
#define B_I2S_CHSEL1 1
#define B_I2S_CHSEL2 2
#define B_I2S_CHSEL3 3

#define REG_RX_AUDIO_CTRL	0x77

#define B_EN_I2S_NLPCM (1<<7)

#define B_AUDCK_BBEN (1<<7)
#define B_FORCE_FS (1<<6)

#define B_DIS_FALSE_DE (1<<5)
#define B_AUD_INFO_FORCE (1<<4)
#define B_AVMUTE_VALUE (1<<3)
#define B_FORCE_AVMUTE (1<<2)
#define B_DIS_VDO_MUTE (1<<1)
#define B_DIS_AUD_MUTE (1<<0)

#define REG_RX_MCLK_CTRL	0x78
#define B_OSC_EN (1<<7)
#define B_OSCSEL (1<<6)

#define B_CTSINI_EN (1<<5)

#define B_FORCE_CTS (1<<4)
#define B_FORCE_CTSMODE (1<<3)
#define O_MCLKSEL 0
#define M_MCLKSEL 0x07
#define B_128FS 0
#define B_256FS 1
#define B_512FS 3
#define B_1024FS 7

#define REG_RX_N_SET1	0x79
#define REG_RX_N_SET2	0x7A
#define REG_RX_N_SET3	0x7B
#define REG_RX_CTS_SET2	0x7C
#define REG_RX_CTS_SET3	0x7D
#define REG_RX_FS_SET	0x7E
#define REG_RX_N_RCV1	0x7F
#define REG_RX_N_RCV2	0x80
#define REG_RX_N_RCV3	0x81
#define REG_RX_CTS_RCV2	0x82
#define REG_RX_CTS_RCV3	0x83
#define REG_RX_FS	0x84
    #define M_GCP_CD 0xF0
    #define O_GCP_CD 4

    #define M_Fs 0xF
    #define B_Fs_44p1KHz    0
    #define B_Fs_48KHz  2
    #define B_Fs_32KHz  3
    #define B_Fs_88p2KHz    8
    #define B_Fs_96KHz  0xA
    #define B_Fs_176p4KHz   0xC
    #define B_Fs_192KHz 0xE
    #define B_Fs_768KHz 0x9
    #define B_Fs_HBR 0x9

#define REG_RX_CHANNEL_ERR	0x85
    #define B_PKT_2BERR (1<<7)
    #define B_PKT_1BERR (1<<6)
    #define O_CH2_ERR 4
    #define O_CH1_ERR 2
    #define O_CH0_ERR 0
#define REG_RX_HWMUTE_RATE	0x86

#define REG_RX_HWMUTE_CTRL	0x87
#define B_HW_FORCE_MUTE (1<<6)
#define B_HW_AUDMUTE_CLR_MODE (1<<5)
#define B_HW_MUTE_CLR (1<<4)
#define B_HW_MUTE_EN (1<<3)

#define M_HWMUTE_RATE_MASK 7

#define REG_RX_HWAMP_CTRL	0x88
#define B_HBR_SPDIF (1<<4)
#define M_HWAMP_CTRL_MASK 3

#define REG_RX_TRISTATE_CTRL	0x89
#define B_VDO_MUTE_DISABLE  (1<<7)
#define O_TRI_VIDEOIO 6
#define O_TRI_VIDEO 5
#define O_TRI_SPDIF 4
#define O_TRI_I2S3 3
#define O_TRI_I2S2 2
#define O_TRI_I2S1 1
#define O_TRI_I2S0 0

#define B_TRI_VIDEOIO (1<<6)
#define B_TRI_VIDEO (1<<5)
#define B_TRI_SPDIF (1<<4)
#define B_TRI_I2S3 (1<<3)
#define B_TRI_I2S2 (1<<2)
#define B_TRI_I2S1 (1<<1)
#define B_TRI_I2S0 (1<<0)

#define B_TRI_ALL  (B_TRI_VIDEOIO|B_TRI_VIDEO|B_TRI_SPDIF|B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0)
#define B_TRI_AUDIO  (B_TRI_SPDIF|B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0)
#define B_TRI_MASK  ~(B_TRI_VIDEOIO|B_TRI_VIDEO|B_TRI_SPDIF|B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0)

#define REG_RX_AUDIO_CH_STAT 0x8A
    #define B_AUDIO_ON    (1<<7)
    #define B_HBRAUDIO    (1<<6)
    #define B_DSDAUDIO    (1<<5)
    #define B_AUDIO_LAYOUT     (1<<4)
    #define M_AUDIO_CH         0xF
    #define B_AUDIO_SRC_VALID_3 (1<<3)
    #define B_AUDIO_SRC_VALID_2 (1<<2)
    #define B_AUDIO_SRC_VALID_1 (1<<1)
    #define B_AUDIO_SRC_VALID_0 (1<<0)

#define F_AUDIO_ON  (1<<7)
#define F_AUDIO_HBR (1<<6)
#define F_AUDIO_DSD (1<<5)
#define F_AUDIO_NLPCM (1<<4)
#define F_AUDIO_LAYOUT_1 (1<<3)
#define F_AUDIO_LAYOUT_0 (0<<3)

#define T_AUDIO_MASK 0xF0
#define T_AUDIO_OFF 0
#define T_AUDIO_HBR (F_AUDIO_ON|F_AUDIO_HBR)
#define T_AUDIO_DSD (F_AUDIO_ON|F_AUDIO_DSD)
#define T_AUDIO_NLPCM (F_AUDIO_ON|F_AUDIO_NLPCM)
#define T_AUDIO_LPCM (F_AUDIO_ON)

#define REG_RX_RDROM_OFFSET	0x8F
#define REG_RX_RDROM_REQBYTE	0x90
#define REG_RX_RDROM_CTRL	0x91

#define REG_RX_RDROM_STATUS	0x92
#define B_ROMIDLE   1
#define REG_RX_RDROM_CLKCTRL	0x9B
#define B_ROM_CLK_SEL_REG   (1<<7)
#define B_ROM_CLK_VALUE     (1<<6)

#define B_CLKRST_SEL (1<<5)
#define B_HDCP_NOAVMUTE (1<<4)
#define O_ANALOG_BACKUP  2
#define O_DIGITAL_BACKUP 0

#define REG_RX_AUD_CHSTAT0 0x9C
#define B_AUD_NLPCM (1<<1)
#define B_SW_COPYRIGHT_ASSERT (1<<2)

#define REG_RX_AUD_CHSTAT1 0x9D

#define REG_RX_AUD_CHSTAT2 0x9E
#define M_CH_NUM    0xF0
#define O_CH_NUM    4
#define M_SRC_NUM   0x0F
#define O_SRC_NUM   0

#define REG_RX_GEN_PKT_TYPE 0xA8

#define REG_RX_AUD_CHSTAT3 0x9F
#define M_CLK_ACCURANCE 0x03
#define M_SW_LEN    0xF0
#define O_SW_LEN    4
#define REG_RX_PKT_REC_TYPE	0xAA

#define SPD_INFOFRAME           0
#define ISRC1_PKT               1
#define ISRC2_PKT               2
#define DSD_PKT                 3
#define AVI_INFOFRAME           4
#define AUDIO_INFOFRAME         5
#define MPEG_INFOFRAME          6
#define VENDORSPEC_INFOFRAME    7
#define ACP_PKT                 8
#define UNRECOGNIZED_PKT        9
#define AUDIO_SAMPLE_PKT        10
#define GENERAL_CONTROL_PKT     11

#define REG_RX_AVI_VER	0xAC
#define REG_RX_AVI_DB0	0xAD

#define O_AVI_COLOR_MODE 5
#define M_AVI_COLOR_MASK   3
#define B_AVI_COLOR_RGB24  0
#define B_AVI_COLOR_YUV422 1
#define B_AVI_COLOR_YUV444 2

#define B_AVI_PRESENT (1<<4)

#define O_AVI_BAR_INFO 2
#define M_AVI_BAR_INFO_MASK 3
#define B_AVI_BAR_NOT_VALID 0
#define B_AVI_BAR_VINFO_VALID 1
#define B_AVI_BAR_HINFO_VALID 2
#define B_AVI_BAR_VHINFO_VALID 3

#define O_AVI_SCAN_INFO 0
#define M_AVI_SCAN_INFO_MASK 3
#define M_AVI_SCAN_NODATA    0
#define M_AVI_SCAN_OVER_SCAN 1
#define M_AVI_SCAN_UNDER_SCAN 2
#define REG_RX_AVI_DB1	0xAE

#define O_AVI_CLRMET 6
#define M_AVI_CLRMET_MASK 3
#define B_AVI_CLRMET_NODATA 0
#define B_AVI_CLRMET_ITU601  1
#define B_AVI_CLRMET_ITU709  2

#define O_AVI_PIC_ASRATIO 4
#define M_AVI_PIC_ASRATIO_MASK 3
#define B_AVI_PIC_ASRATIO_NODATA 0
#define B_AVI_PIC_ASRATIO_4_3    1
#define B_AVI_PIC_ASRATIO_16_9   2

#define O_AVI_FMT_ASRATIO 0
#define M_AVI_FMT_ASRATIO_MASK  0xF
#define M_AVI_FMT_ASRATIO_EQPIC 0x8
#define M_AVI_FMT_ASRATIO_4_3 0x9
#define M_AVI_FMT_ASRATIO_16_9 0xA
#define M_AVI_FMT_ASRATIO_14_9 0xB
#define REG_RX_AVI_DB2	0xAF

#define O_AVI_NUNI_SCALE 0
#define M_AVI_NUNI_SCALE_MASK 3
#define B_AVI_NUNI_SCALE_NODATA 0
#define B_AVI_NUNI_SCALE_HORZ 1
#define B_AVI_NUNI_SCALE_VERT 2
#define B_AVI_NUNI_SCALE_HORZVERT 3

#define REG_RX_AVI_DB3	0xB0

#define O_AVI_VIDCODE 0
#define O_AVI_VIDCODE_MASK 0x7f
#define REG_RX_AVI_DB4	0xB1

#define O_AVI_PIXREPT 0
#define M_AVI_PIXREPT_MASK 0xF

#define REG_RX_AVI_DB5	0xB2
#define REG_RX_AVI_DB6	0xB3
#define REG_RX_AVI_DB7	0xB4
#define REG_RX_AVI_DB8	0xB5
#define REG_RX_AVI_DB9	0xB6
#define REG_RX_AVI_DB10	0xB7
#define REG_RX_AVI_DB11	0xB8
#define REG_RX_AVI_DB12	0xB9
#define REG_RX_AVI_DB13	0xBA
#define REG_RX_AVI_DB14	0xBB
#define REG_RX_AVI_DB15	0xBC
#define REG_RX_GENPKT_HB0	0xBD
#define REG_RX_GENPKT_HB1	0xBE
#define REG_RX_GENPKT_HB2	0xBF
#define REG_RX_GENPKT_DB0	0xC0
#define REG_RX_GENPKT_DB1	0xC1
#define REG_RX_GENPKT_DB2	0xC2
#define REG_RX_GENPKT_DB3	0xC3
#define REG_RX_GENPKT_DB4	0xC4
#define REG_RX_GENPKT_DB5	0xC5
#define REG_RX_GENPKT_DB6	0xC6
#define REG_RX_GENPKT_DB7	0xC7
#define REG_RX_GENPKT_DB8	0xC8
#define REG_RX_GENPKT_DB9	0xC9
#define REG_RX_GENPKT_DB10	0xCA
#define REG_RX_GENPKT_DB11	0xCB
#define REG_RX_GENPKT_DB12	0xCC
#define REG_RX_GENPKT_DB13	0xCD
#define REG_RX_GENPKT_DB14	0xCE
#define REG_RX_GENPKT_DB15	0xCF
#define REG_RX_GENPKT_DB16	0xD0
#define REG_RX_GENPKT_DB17	0xD1
#define REG_RX_GENPKT_DB18	0xD2
#define REG_RX_GENPKT_DB19	0xD3
#define REG_RX_GENPKT_DB20	0xD4
#define REG_RX_GENPKT_DB21	0xD5
#define REG_RX_GENPKT_DB22	0xD6
#define REG_RX_GENPKT_DB23	0xD7
#define REG_RX_GENPKT_DB24	0xD8
#define REG_RX_GENPKT_DB25	0xD9
#define REG_RX_GENPKT_DB26	0xDA
#define REG_RX_GENPKT_DB27	0xDB
#define REG_RX_AUDIO_VER	0xDC
#define REG_RX_AUDIO_DB0	0xDD

#define O_AUD_CODTYPE 4
#define M_AUD_CODTYPE_MASK 0xF
#define B_AUD_CODTYPE_STREAMDEF 0
#define B_AUD_CODTYPE_IEC60958  1
#define B_AUD_CODTYPE_AC3       2
#define B_AUD_CODTYPE_MPEG1     3
#define B_AUD_CODTYPE_MP3       4
#define B_AUD_CODTYPE_MPEG2     5
#define B_AUD_CODTYPE_AAC       6
#define B_AUD_CODTYPE_DTS       7
#define B_AUD_CODTYPE_ATRAC     8

#define O_AUD_CHCNT 0
#define M_AUD_CHCNT_MASK 0x7
#define B_AUD_CHCNT_STREAMDEF 0
#define B_AUD_CHCNT_CH2       1
#define B_AUD_CHCNT_CH3       2
#define B_AUD_CHCNT_CH4       3
#define B_AUD_CHCNT_CH5       4
#define B_AUD_CHCNT_CH6       5
#define B_AUD_CHCNT_CH7       6
#define B_AUD_CHCNT_CH8       7
#define REG_RX_AUDIO_DB1	0xDE

#define O_AUD_SMP_FREQ  2
#define M_AUD_SMP_FREQ_MASK 7
#define B_AUD_SMP_FREQ_STREAMDEF 0
#define B_AUD_SMP_32K            1
#define B_AUD_SMP_44K            2
#define B_AUD_SMP_48K            3
#define B_AUD_SMP_88K            4
#define B_AUD_SMP_96K            5
#define B_AUD_SMP_176K           6
#define B_AUD_SMP_192K           7

#define O_AUD_SMP_SIZE 0
#define O_AUD_SMP_SIZE_MASK 0x3
#define O_AUD_SMP_SIZE_STREAMDEF 0
#define O_AUD_SMP_16BIT 1
#define O_AUD_SMP_20BIT 2
#define O_AUD_SMP_24BIT 3
#define REG_RX_AUDIO_DB2	0xDF

#define REG_RX_AUDIO_DB3	0xE0

#define REG_RX_AUDIO_DB4	0xE1

#define B_PROHIBITED (1<<7)

#define O_AUD_LEVEL_SHIFT_VALUE 3
#define O_AUD_LEVEL_SHIFT_MASK 0xF
#define REG_RX_AUDIO_DB5	0xE2
#define REG_RX_MPEG_VER	0xE3
#define REG_RX_MPEG_DB0	0xE4
#define REG_RX_MPEG_DB1	0xE5
#define REG_RX_MPEG_DB2	0xE6
#define REG_RX_MPEG_DB3	0xE7
#define REG_RX_MPEG_DB4	0xE8
#define REG_RX_MPEG_DB5	0xE9
#define REG_RX_VS_VER	0xEA

#define REG_RX_VS_DB0	0xEB
#define REG_RX_VS_DB1	0xEC
#define REG_RX_VS_DB2	0xED
#define REG_RX_VS_DB3	0xEE
#define REG_RX_VS_DB4	0xEF
#define REG_RX_VS_DB5	0xF0
#define REG_RX_VS_DB6	0xF1
#define REG_RX_VS_DB7	0xF2
#define REG_RX_VS_DB8	0xF3
#define REG_RX_ACP_TYPE	0xF4
#define REG_RX_ACP_DB0	0xF5
#define REG_RX_ACP_DB1	0xF6
#define REG_RX_ACP_DB2	0xF7
#define REG_RX_ACP_DB3	0xF8
#define REG_RX_ACP_DB4	0xF9
#define REG_RX_ACP_DB5	0xFA
#define REG_RX_ACP_DB6	0xFB
#define REG_RX_ACP_DB7	0xFC
#define REG_RX_ACP_DB8	0xFD
#define REG_RX_SRC_VER	0xFE
#define REG_RX_SRC_PB25	0xFF

#define REG_RX_ISRC1_HB0 0xA0
#define REG_RX_ISRC1_HB1 0xA1
#define REG_RX_ISRC1_HB2 0xA2
#define REG_RX_ISRC1_DB0 0xA3
#define REG_RX_ISRC1_DB1 0xA4
#define REG_RX_ISRC1_DB2 0xA5
#define REG_RX_ISRC1_DB3 0xA6
#define REG_RX_ISRC1_DB4 0xA7
#define REG_RX_ISRC1_DB5 0xA8
#define REG_RX_ISRC1_DB6 0xA9
#define REG_RX_ISRC1_DB7 0xAA
#define REG_RX_ISRC1_DB8 0xAB
#define REG_RX_ISRC1_DB9 0xAC
#define REG_RX_ISRC1_DB10 0xAD
#define REG_RX_ISRC1_DB11 0xAE
#define REG_RX_ISRC1_DB12 0xAF
#define REG_RX_ISRC1_DB13 0xB0
#define REG_RX_ISRC1_DB14 0xB1
#define REG_RX_ISRC1_DB15 0xB2
#define REG_RX_ISRC1_REC_TYPE 0xB3
#define REG_RX_ISRC2_HB0 0xB4
#define REG_RX_ISRC2_HB1 0xB5
#define REG_RX_ISRC2_HB2 0xB6
#define REG_RX_ISRC2_DB0 0xB7
#define REG_RX_ISRC2_DB1 0xB8
#define REG_RX_ISRC2_DB2 0xB9
#define REG_RX_ISRC2_DB3 0xBA
#define REG_RX_ISRC2_DB4 0xBB
#define REG_RX_ISRC2_DB5 0xBC
#define REG_RX_ISRC2_DB6 0xBD
#define REG_RX_ISRC2_DB7 0xBE
#define REG_RX_ISRC2_DB8 0xBF
#define REG_RX_ISRC2_REC_TYPE 0xC0
#define REG_RX_KSV_FIFO00 0xC1
#define REG_RX_KSV_FIFO01 0xC2
#define REG_RX_KSV_FIFO02 0xC3
#define REG_RX_KSV_FIFO03 0xC4
#define REG_RX_KSV_FIFO04 0xC5
#define REG_RX_KSV_FIFO10 0xC6
#define REG_RX_KSV_FIFO11 0xC7
#define REG_RX_KSV_FIFO12 0xC8
#define REG_RX_KSV_FIFO13 0xC9
#define REG_RX_KSV_FIFO14 0xCA
#define REG_RX_KSV_FIFO20 0xCB
#define REG_RX_KSV_FIFO21 0xCC
#define REG_RX_KSV_FIFO22 0xCD
#define REG_RX_KSV_FIFO23 0xCE
#define REG_RX_KSV_FIFO24 0xCF
#define REG_RX_KSV_FIFO30 0xD0
#define REG_RX_KSV_FIFO31 0xD1
#define REG_RX_KSV_FIFO32 0xD2
#define REG_RX_KSV_FIFO33 0xD3
#define REG_RX_KSV_FIFO34 0xD4
#define REG_RX_KSV_FIFO40 0x80
#define REG_RX_KSV_FIFO41 0x81
#define REG_RX_KSV_FIFO42 0x82
#define REG_RX_KSV_FIFO43 0x83
#define REG_RX_KSV_FIFO44 0x84
#define REG_RX_KSV_FIFO50 0x85
#define REG_RX_KSV_FIFO51 0x86
#define REG_RX_KSV_FIFO52 0x87
#define REG_RX_KSV_FIFO53 0x88
#define REG_RX_KSV_FIFO54 0x89
#define REG_RX_KSV_FIFO60 0x8A
#define REG_RX_KSV_FIFO61 0x8B
#define REG_RX_KSV_FIFO62 0x8C
#define REG_RX_KSV_FIFO63 0x8D
#define REG_RX_KSV_FIFO64 0x8E
#define REG_RX_KSV_FIFO70 0x8F
#define REG_RX_KSV_FIFO71 0x90
#define REG_RX_KSV_FIFO72 0x91
#define REG_RX_KSV_FIFO73 0x92
#define REG_RX_KSV_FIFO74 0x93

#define REG_RX_BSTATUSL 0xD5
#define REG_RX_BSTATUSH 0xD6
    #define B_CAP_HDMI_MODE (1<<12)
    #define B_CAP_DVI_MODE (0<<12)
    #define B_MAX_CASCADE_EXCEEDED  (1<<11)
    #define M_REPEATER_DEPTH    (0x7<<8)
    #define O_REPEATER_DEPTH    8
    #define B_DOWNSTREAM_OVER   (1<<7)
    #define M_DOWNSTREAM_COUNT  0x7F
#define REG_RX_SHA1_H00 0xD7
#define REG_RX_SHA1_H01 0xD8
#define REG_RX_SHA1_H02 0xD9
#define REG_RX_SHA1_H03 0xDA
#define REG_RX_SHA1_H10 0xDB
#define REG_RX_SHA1_H11 0xDC
#define REG_RX_SHA1_H12 0xDD
#define REG_RX_SHA1_H13 0xDE
#define REG_RX_SHA1_H20 0xDF
#define REG_RX_SHA1_H21 0xE0
#define REG_RX_SHA1_H22 0xE1
#define REG_RX_SHA1_H23 0xE2
#define REG_RX_SHA1_H30 0xE3
#define REG_RX_SHA1_H31 0xE4
#define REG_RX_SHA1_H32 0xE5
#define REG_RX_SHA1_H33 0xE6
#define REG_RX_SHA1_H40 0xE7
#define REG_RX_SHA1_H41 0xE8
#define REG_RX_SHA1_H42 0xE9
#define REG_RX_SHA1_H43 0xEA
#define REG_RX_M0_B0 0xEB
#define REG_RX_M0_B1 0xEC
#define REG_RX_M0_B2 0xED
#define REG_RX_M0_B3 0xEE
#define REG_RX_M0_B4 0xEF
#define REG_RX_M0_B5 0xF0
#define REG_RX_M0_B6 0xF1
#define REG_RX_M0_B7 0xF2

#define CAPS_MIN_PROGRESS_VFREQ 2300
#define CAPS_MAX_PROGRESS_VFREQ 10000
#define CAPS_MIN_PROGRESS_HFREQ 300
#define CAPS_MAX_PROGRESS_HFREQ 1000
#define CAPS_MIN_INTERLACE_VFREQ 2500
#define CAPS_MAX_INTERLACE_VFREQ 6000
#define CAPS_MIN_INTERLACE_HFREQ 150
#define CAPS_MAX_INTERLACE_HFREQ 550

#define HDMI_RX_I2C_SLAVE_ADDR 0x90

#define PORT_PRN_DATA 0x378
#define PORT_PRN_STAT 0x379
#define PORT_PRN_CTRL 0x37A

#define FLAG_CLEAR_INT_MODE 1
#define FLAG_CLEAR_INT_PKG 2
#define FLAG_CLEAR_INT_ECC 4
#define FLAG_CLEAR_INT_AUDIO 8
#define FLAG_CLEAR_INT_MUTECLR (1<<6)
#define FLAG_CLEAR_INT_MUTESET (1<<7)
#define FLAG_CLEAR_INT_ALL  (FLAG_CLEAR_INT_MUTESET|FLAG_CLEAR_INT_MUTECLR|FLAG_CLEAR_INT_MODE|FLAG_CLEAR_INT_PKG|FLAG_CLEAR_INT_ECC|FLAG_CLEAR_INT_AUDIO)
#define FLAG_CLEAR_INT_MASK (~FLAG_CLEAR_INT_ALL)

#define FLAG_CLEAR_INT_HDCP (1<<7)
#define B_CAP_AUDIO_ON  (1<<7)
#define B_CAP_HBR_AUDIO (1<<6)
#define B_CAP_DSD_AUDIO (1<<5)
#define B_LAYOUT        (1<<4)
#define B_MULTICH       (1<<4)
#define B_HBR_BY_SPDIF  (1<<3)
#define B_SPDIF         (1<<2)
#define B_CAP_LPCM      (1<<0)

typedef struct {
    BYTE AudioFlag ;
    BYTE AudSrcEnable ;
    BYTE SampleFreq ;
    BYTE ChStat[5] ;
} AUDIO_CAPS ;

void Switch_HDMIRX_Bank(CDevice * p_Device, BYTE x);//   HDMIRX_WriteI2C_Byte(p_Device, 0x0f,(x)&1)

#define CAT_HDMI_PORTA 0
#define CAT_HDMI_PORTB 1
#define CAT6023_MAX_KSV 6



#define HDCP_RECEIVER   0
#define HDCP_REPEATER   1
#define HDCP_RDY_TIMEOUT    (1<<1)
#define HDCP_INVALID_V      (1<<2)
#define HDCP_OVER_DOWNSTREAM    (1<<3)
#define HDCP_OVER_CASCADE   (1<<4)

#define EVENT_RXHDCP_AUTH_START   (1<<0)
#define EVENT_RXHDCP_AUTH_DONE   (1<<1)




///////////////////////////////////////////////////////////////////
//below is from typedef.h
#define ON 1
#define OFF 0

typedef enum _SYS_STATUS {
    ER_SUCCESS = 0,
    ER_FAIL,
    ER_RESERVED
} SYS_STATUS ;

#define ABS(x) (((x)>=0)?(x):(-(x)))

typedef struct {
    WORD HActive ;
    WORD VActive ;
    WORD HTotal ;
    WORD VTotal ;
    LONG PCLK ;
    BYTE xCnt ;
    WORD HFrontPorch ;
    WORD HSyncWidth ;
    WORD HBackPorch ;
    BYTE VFrontPorch ;
    BYTE VSyncWidth ;
    BYTE VBackPorch ;
    BYTE ScanMode:1 ;
    BYTE VPolarity:1 ;
    BYTE HPolarity:1 ;
} VTiming ;

#define PROG 1
#define INTERLACE 0
#define Vneg 0
#define Hneg 0
#define Vpos 1
#define Hpos 1

typedef enum _Video_State_Type {
    VSTATE_PwrOff = 0,
    VSTATE_SyncWait ,
    VSTATE_SWReset,
    VSTATE_SyncChecking,
    VSTATE_HDCPSet,
    VSTATE_HDCP_Reset,
    VSTATE_ModeDetecting,
    VSTATE_VideoOn,
    VSTATE_Reserved
} Video_State_Type ;

typedef enum _Audio_State_Type {
    ASTATE_AudioOff = 0,
    ASTATE_RequestAudio ,
    ASTATE_ResetAudio,
    ASTATE_WaitForReady,
    ASTATE_AudioOn ,
    ASTATE_Reserved
} Audio_State_Type ;

typedef enum _TXVideo_State_Type {
    TXVSTATE_Unplug = 0,
    TXVSTATE_HPD,
    TXVSTATE_WaitForMode,
    TXVSTATE_WaitForVStable,
    TXVSTATE_VideoInit,
    TXVSTATE_VideoSetup,
    TXVSTATE_VideoOn,
    TXVSTATE_Reserved
} TXVideo_State_Type ;

typedef enum _TXAudio_State_Type {
    TXASTATE_AudioOff = 0,
    TXASTATE_AudioPrepare,
    TXASTATE_AudioOn,
    TXASTATE_AudioFIFOFail,
    TXASTATE_Reserved
} TXAudio_State_Type ;

typedef enum _RXHDCP_State_Type {
    RXHDCP_Reset = 0,
    RXHDCP_AuthStart,
    RXHDCP_AuthDone,
    RXHDCP_UpdateKSVList,
    RXHDCP_Ready,
    RXHDCP_FailReady,
    RXHDCP_Reserved
} RXHDCP_State_Type ;

typedef enum {
    PCLK_LOW = 0 ,
    PCLK_MEDIUM,
    PCLK_HIGH
} VIDEOPCLKLEVEL ;

#define F_MODE_RGB24  0
#define F_MODE_RGB444  0
#define F_MODE_YUV422 1
#define F_MODE_YUV444 2
#define F_MODE_CLRMOD_MASK 3

#define F_MODE_INTERLACE  1

#define F_MODE_ITU709  (1<<4)
#define F_MODE_ITU601  0

#define F_MODE_0_255   0
#define F_MODE_16_235  (1<<5)

#define F_MODE_EN_UDFILT (1<<6)
#define F_MODE_EN_DITHER (1<<7)

typedef union _VideoFormatCode
{
    struct _VFC
    {
        BYTE colorfmt:2 ;
        BYTE interlace:1 ;
        BYTE Colorimetry:1 ;
        BYTE Quantization:1 ;
        BYTE UpDownFilter:1 ;
        BYTE Dither:1 ;
    } VFCCode ;
    unsigned char VFCByte ;
} VideoFormatCode ;

#define T_MODE_CCIR656 (1<<0)
#define T_MODE_SYNCEMB (1<<1)
#define T_MODE_INDDR   (1<<2)
#define T_MODE_PCLKDIV2 (1<<3)

#define AUDFS_22p05KHz  4
#define AUDFS_44p1KHz 0
#define AUDFS_88p2KHz 8
#define AUDFS_176p4KHz    12

#define AUDFS_24KHz  6
#define AUDFS_48KHz  2
#define AUDFS_96KHz  10
#define AUDFS_192KHz 14

#define DEFAULT_START_FIXED_AUD_SAMPLEFREQ AUDFS_192KHz


#define AUDFS_32KHz  3
#define AUDFS_OTHER    1

#define ENABLE_SPDIF    (1<<4)
#define ENABLE_I2S_SRC3  (1<<3)
#define ENABLE_I2S_SRC2  (1<<2)
#define ENABLE_I2S_SRC1  (1<<1)
#define ENABLE_I2S_SRC0  (1<<0)

#define AUD_SWL_NOINDICATE  0x0
#define AUD_SWL_16          0x2
#define AUD_SWL_17          0xC
#define AUD_SWL_18          0x4
#define AUD_SWL_20          0xA
#define AUD_SWL_21          0xD
#define AUD_SWL_22          0x5
#define AUD_SWL_23          0x9
#define AUD_SWL_24          0xB

#define VENDORSPEC_INFOFRAME_TYPE 0x01
#define AVI_INFOFRAME_TYPE  0x02
#define SPD_INFOFRAME_TYPE 0x03
#define AUDIO_INFOFRAME_TYPE 0x04
#define MPEG_INFOFRAME_TYPE 0x05

#define VENDORSPEC_INFOFRAME_VER 0x01
#define AVI_INFOFRAME_VER  0x02
#define SPD_INFOFRAME_VER 0x01
#define AUDIO_INFOFRAME_VER 0x01
#define MPEG_INFOFRAME_VER 0x01

#define VENDORSPEC_INFOFRAME_LEN 8
#define AVI_INFOFRAME_LEN 13
#define SPD_INFOFRAME_LEN 25
#define AUDIO_INFOFRAME_LEN 10
#define MPEG_INFOFRAME_LEN 10

#define ACP_PKT_LEN 9
#define ISRC1_PKT_LEN 16
#define ISRC2_PKT_LEN 16

typedef union _AVI_InfoFrame
{
    struct {
        BYTE Type ;
        BYTE Ver ;
        BYTE Len ;

        BYTE Scan:2 ;
        BYTE BarInfo:2 ;
        BYTE ActiveFmtInfoPresent:1 ;
        BYTE ColorMode:2 ;
        BYTE FU1:1 ;

        BYTE ActiveFormatAspectRatio:4 ;
        BYTE PictureAspectRatio:2 ;
        BYTE Colorimetry:2 ;

        BYTE Scaling:2 ;
        BYTE FU2:6 ;

        BYTE VIC:7 ;
        BYTE FU3:1 ;

        BYTE PixelRepetition:4 ;
        BYTE FU4:4 ;

        SHORT Ln_End_Top ;
        SHORT Ln_Start_Bottom ;
        SHORT Pix_End_Left ;
        SHORT Pix_Start_Right ;
    } info ;
    struct {
        BYTE AVI_HB[3] ;
        BYTE AVI_DB[AVI_INFOFRAME_LEN] ;
    } pktbyte ;
} AVI_InfoFrame ;

typedef union _Audio_InfoFrame {

    struct {
        BYTE Type ;
        BYTE Ver ;
        BYTE Len ;

        BYTE AudioChannelCount:3 ;
        BYTE RSVD1:1 ;
        BYTE AudioCodingType:4 ;

        BYTE SampleSize:2 ;
        BYTE SampleFreq:3 ;
        BYTE Rsvd2:3 ;

        BYTE FmtCoding ;

        BYTE SpeakerPlacement ;

        BYTE Rsvd3:3 ;
        BYTE LevelShiftValue:4 ;
        BYTE DM_INH:1 ;
    } info ;

    struct {
        BYTE AUD_HB[3] ;
        BYTE AUD_DB[AUDIO_INFOFRAME_LEN] ;
    } pktbyte ;

} Audio_InfoFrame ;

typedef union _MPEG_InfoFrame {
    struct {
        BYTE Type ;
        BYTE Ver ;
        BYTE Len ;

        ULONG MpegBitRate ;

        BYTE MpegFrame:2 ;
        BYTE Rvsd1:2 ;
        BYTE FieldRepeat:1 ;
        BYTE Rvsd2:3 ;
    } info ;
    struct {
        BYTE MPG_HB[3] ;
        BYTE MPG_DB[MPEG_INFOFRAME_LEN] ;
    } pktbyte ;
} MPEG_InfoFrame ;

typedef union _SPD_InfoFrame {
    struct {
        BYTE Type ;
        BYTE Ver ;
        BYTE Len ;

        char VN[8] ;
        char PD[16] ;
        BYTE SourceDeviceInfomation ;
    } info ;
    struct {
        BYTE SPD_HB[3] ;
        BYTE SPD_DB[SPD_INFOFRAME_LEN] ;
    } pktbyte ;
} SPD_InfoFrame ;

struct VideoTiming {
    ULONG VideoPixelClock ;
    BYTE VIC ;
    BYTE pixelrep ;
	BYTE outputVideoMode ;
} ;

#define F_VIDMODE_ITU709  (1<<4)
#define F_VIDMODE_ITU601  0

#define F_VIDMODE_0_255   0
#define F_VIDMODE_16_235  (1<<5)

#define F_VIDMODE_EN_UDFILT (1<<6)
#define F_VIDMODE_EN_DITHER (1<<7)

#define T_MODE_CCIR656 (1<<0)
#define T_MODE_SYNCEMB (1<<1)
#define T_MODE_INDDR (1<<2)
#define T_MODE_DEGEN (1<<3)
#define T_MODE_SYNCGEN (1<<4)

#define FS_22K05  4
#define FS_44K1 0
#define FS_88K2 8
#define FS_176K4    12

#define FS_24K  6
#define FS_48K  2
#define FS_96K  10
#define FS_192K 14

#define FS_32K  3
#define FS_OTHER    1

#define ENABLE_SPDIF    (1<<4)
#define ENABLE_I2S_SRC3  (1<<3)
#define ENABLE_I2S_SRC2  (1<<2)
#define ENABLE_I2S_SRC1  (1<<1)
#define ENABLE_I2S_SRC0  (1<<0)

#define AUD_SWL_NOINDICATE  0x0
#define AUD_SWL_16          0x2
#define AUD_SWL_17          0xC
#define AUD_SWL_18          0x4
#define AUD_SWL_20          0xA
#define AUD_SWL_21          0xD
#define AUD_SWL_22          0x5
#define AUD_SWL_23          0x9
#define AUD_SWL_24          0xB

#define VENDORSPEC_INFOFRAME_TYPE 0x01
#define AVI_INFOFRAME_TYPE  0x02
#define SPD_INFOFRAME_TYPE 0x03
#define AUDIO_INFOFRAME_TYPE 0x04
#define MPEG_INFOFRAME_TYPE 0x05

#define VENDORSPEC_INFOFRAME_VER 0x01
#define AVI_INFOFRAME_VER  0x02
#define SPD_INFOFRAME_VER 0x01
#define AUDIO_INFOFRAME_VER 0x01
#define MPEG_INFOFRAME_VER 0x01

#define VENDORSPEC_INFOFRAME_LEN 8
#define AVI_INFOFRAME_LEN 13
#define SPD_INFOFRAME_LEN 25
#define AUDIO_INFOFRAME_LEN 10
#define MPEG_INFOFRAME_LEN 10

#define ACP_PKT_LEN 9
#define ISRC1_PKT_LEN 16
#define ISRC2_PKT_LEN 16

typedef enum tagHDMI_Video_Type {
    HDMI_Unkown = 0 ,
    HDMI_640x480p60 = 1 ,
    HDMI_480p60,
    HDMI_480p60_16x9,
    HDMI_720p60,
    HDMI_1080i60,
    HDMI_480i60,
    HDMI_480i60_16x9,
    HDMI_240p60,
    HDMI_1440x480p60,
    HDMI_1080p60 = 16,
    HDMI_576p50,
    HDMI_576p50_16x9,
    HDMI_720p50,
    HDMI_1080i50,
    HDMI_576i50,
    HDMI_576i50_16x9,
    HDMI_288p50,
    HDMI_1440x576p50,
    HDMI_1080p50 = 31,
    HDMI_1080p24,
    HDMI_1080p25,
    HDMI_1080p30,
} HDMI_Video_Type ;

typedef enum tagHDMI_Aspec {
    HDMI_4x3 ,
    HDMI_16x9
} HDMI_Aspec;

typedef enum tagHDMI_OutputColorMode {
    HDMI_RGB444,
    HDMI_YUV444,
    HDMI_YUV422
} HDMI_OutputColorMode ;

typedef enum tagHDMI_Colorimetry {
    HDMI_ITU601,
    HDMI_ITU709
} HDMI_Colorimetry ;

typedef enum tagMODE_ID{
	CEA_640x480p60,
	CEA_720x480p60,
	CEA_1280x720p60,
	CEA_1920x1080i60,
	CEA_720x480i60,
	CEA_720x240p60,
	CEA_1440x480i60,
	CEA_1440x240p60,
	CEA_2880x480i60,
	CEA_2880x240p60,
	CEA_1440x480p60,
	CEA_1920x1080p60,
	CEA_720x576p50,
	CEA_1280x720p50,
	CEA_1920x1080i50,
	CEA_720x576i50,
	CEA_1440x576i50,
	CEA_720x288p50,
	CEA_1440x288p50,
	CEA_2880x576i50,
	CEA_2880x288p50,
	CEA_1440x576p50,
	CEA_1920x1080p50,
	CEA_1920x1080p24,
	CEA_1920x1080p25,
	CEA_1920x1080p30,
	VESA_640x350p85,
	VESA_640x400p85,
	VESA_720x400p85,
	VESA_640x480p60,
	VESA_640x480p72,
	VESA_640x480p75,
	VESA_640x480p85,
	VESA_800x600p56,
	VESA_800x600p60,
	VESA_800x600p72,
	VESA_800x600p75,
	VESA_800X600p85,
	VESA_840X480p60,
	VESA_1024x768p60,
	VESA_1024x768p70,
	VESA_1024x768p75,
	VESA_1024x768p85,
	VESA_1152x864p75,
	VESA_1280x768p60R,
	VESA_1280x768p60,
	VESA_1280x768p75,
	VESA_1280x768p85,
	VESA_1280x960p60,
	VESA_1280x960p85,
	VESA_1280x1024p60,
	VESA_1280x1024p75,
	VESA_1280X1024p85,
	VESA_1360X768p60,
	VESA_1400x768p60R,
	VESA_1400x768p60,
	VESA_1400x1050p75,
	VESA_1400x1050p85,
	VESA_1440x900p60R,
	VESA_1440x900p60,
	VESA_1440x900p75,
	VESA_1440x900p85,
	VESA_1600x1200p60,
	VESA_1600x1200p65,
	VESA_1600x1200p70,
	VESA_1600x1200p75,
	VESA_1600x1200p85,
	VESA_1680x1050p60R,
	VESA_1680x1050p60,
	VESA_1680x1050p75,
	VESA_1680x1050p85,
	VESA_1792x1344p60,
	VESA_1792x1344p75,
	VESA_1856x1392p60,
	VESA_1856x1392p75,
	VESA_1920x1200p60R,
	VESA_1920x1200p60,
	VESA_1920x1200p75,
	VESA_1920x1200p85,
	VESA_1920x1440p60,
	VESA_1920x1440p75,
	UNKNOWN_MODE
} MODE_ID;

//////////////////////////////////////////////////////////
#define DEBUG 1

#ifdef DEBUG

//#define LOOP_MSEC 32
#define LOOP_MSEC 500
#define MS_TimeOut(x) (((x)+LOOP_MSEC-1)/LOOP_MSEC)

//peter [2011.06.16]
//do not shorten this, otherwise Sony camera hard to lock
//8000 is for format detction thread at 100ms freq
#define VSTATE_MISS_SYNC_COUNT MS_TimeOut(8000)

#define VSATE_CONFIRM_SCDT_COUNT MS_TimeOut(150)
#define AUDIO_READY_TIMEOUT MS_TimeOut(200)
#define AUDIO_CLEARERROR_TIMEOUT MS_TimeOut(1000)
#define MUTE_RESUMING_TIMEOUT MS_TimeOut(2500)
#define HDCP_WAITING_TIMEOUT MS_TimeOut(3000)
#define CDRRESET_TIMEOUT MS_TimeOut(3000)
#define VSTATE_SWRESET_COUNT MS_TimeOut(500)
#define FORCE_SWRESET_TIMEOUT  MS_TimeOut(15000)
#define VIDEO_TIMER_CHECK_COUNT MS_TimeOut(250)

#define SCDT_LOST_TIMEOUT  15


//peter 2010.12.03
typedef enum _HDMI_Receiver_Type_t
{
    MST3367 = 0,
    IT6603
} HDMI_Receiver_Type_t;

#endif//DEBUG
/*
#define SetForceHWMute() { SetHWMuteCTRL(p_Device, (~B_HW_FORCE_MUTE),(B_HW_FORCE_MUTE)) ; }
#define SetHWMuteClrMode() { SetHWMuteCTRL(p_Device, (~B_HW_AUDMUTE_CLR_MODE),(B_HW_AUDMUTE_CLR_MODE)) ;}
#define SetHWMuteClr() { SetHWMuteCTRL(p_Device, (~B_HW_MUTE_CLR),(B_HW_MUTE_CLR)) ; }
#define SetHWMuteEnable() { SetHWMuteCTRL(p_Device, (~B_HW_MUTE_EN),(B_HW_MUTE_EN)) ; }
#define ClearForceHWMute() { SetHWMuteCTRL(p_Device, (~B_HW_FORCE_MUTE),0) ; }
#define ClearHWMuteClrMode() { SetHWMuteCTRL(p_Device, (~B_HW_AUDMUTE_CLR_MODE),0) ; }
#define ClearHWMuteClr() { SetHWMuteCTRL(p_Device, (~B_HW_MUTE_CLR),0) ; }
#define ClearHWMuteEnable() { SetHWMuteCTRL(p_Device, (~B_HW_MUTE_EN),0) ;}
*/
void SetHWMuteClrMode(CDevice * p_Device);// { SetHWMuteCTRL(p_Device, (~B_HW_AUDMUTE_CLR_MODE),(B_HW_AUDMUTE_CLR_MODE)) ;}
void SetHWMuteClr(CDevice * p_Device);// { SetHWMuteCTRL(p_Device, (~B_HW_MUTE_CLR),(B_HW_MUTE_CLR)) ; }
void ClearHWMuteClr(CDevice * p_Device);// { SetHWMuteCTRL(p_Device, (~B_HW_MUTE_CLR),0) ; }
void SetALLMute(CDevice * p_Device);// SetMUTE(B_VDO_MUTE_DISABLE,(B_VDO_MUTE_DISABLE|B_TRI_ALL))


BYTE Get_IT6603_Initialization_Status(CDevice * p_Device);
void Check_HDMInterrupt(CDevice * p_Device);

BYTE HDMIRX_ReadI2C_Byte(CDevice * p_Device, BYTE RegAddr);
BOOL HDMIRX_WriteI2C_Byte(CDevice * p_Device, BYTE RegAddr,BYTE d);
BOOL HDMIRX_ReadI2C_ByteN(CDevice * p_Device, BYTE RegAddr,BYTE *pData,int N);
BOOL HDMIRX_WriteI2C_ByteN(CDevice * p_Device, BYTE RegAddr,BYTE *pData,int N);

BOOL bGetSyncInfo(CDevice * p_Device) ;
BOOL CheckHDMIRX(CDevice * p_Device) ;
void SelectHDMIPort(CDevice * p_Device, BYTE ucPort);
BYTE GetCurrentHDMIPort(CDevice * p_Device);

//void DumpHDMIRX() ;

void Interrupt_Handler(CDevice * p_Device) ;
void Timer_Handler(CDevice * p_Device) ;
void Video_Handler(CDevice * p_Device) ;

//void HWReset_HDMIRX() ;
void SWReset_HDMIRX(CDevice * p_Device) ;
void Terminator_Reset(CDevice * p_Device) ;
void Terminator_Off(CDevice * p_Device) ;
void Terminator_On(CDevice * p_Device) ;

void Check_RDROM(CDevice * p_Device) ;
void RDROM_Reset(CDevice * p_Device) ;
void SetDefaultRegisterValue(CDevice * p_Device) ;

void LoadCustomizeDefaultSetting(CDevice * p_Device) ;
void SetupAudio(CDevice * p_Device) ;

//BOOL ReadRXIntPin() ;

void ClearIntFlags(CDevice * p_Device, BYTE flag) ;
void ClearHDCPIntFlags(CDevice * p_Device) ;
BOOL IsSCDT(CDevice * p_Device) ;
BOOL CheckPlg5VPwr(CDevice * p_Device) ;

//BOOL CheckHDCPFail() ;
void SetMUTE(CDevice * p_Device, BYTE AndMask, BYTE OrMask) ;
void SetMCLKInOUt(BYTE MCLKSelect) ;
void SetIntMask1(CDevice * p_Device, BYTE AndMask,BYTE OrMask) ;
void SetIntMask2(CDevice * p_Device, BYTE AndMask,BYTE OrMask) ;
void SetIntMask3(CDevice * p_Device, BYTE AndMask,BYTE OrMask) ;
void SetIntMask4(CDevice * p_Device, BYTE AndMask,BYTE OrMask) ;
void SetGeneralPktType(CDevice * p_Device, BYTE type) ;
BOOL IsCAT6023HDMIMode(CDevice * p_Device) ;
void EnableAudio(CDevice * p_Device) ;


void RXINT_5V_PwrOn(CDevice * p_Device) ;
void RXINT_5V_PwrOff(CDevice * p_Device) ;
void RXINT_SCDT_On(CDevice * p_Device) ;
void RXINT_SCDT_Off(CDevice * p_Device) ;
void RXINT_RXCKON(CDevice * p_Device) ;
void RXINT_VideoMode_Chg(CDevice * p_Device) ;
void RXINT_HDMIMode_Chg(CDevice * p_Device) ;
void RXINT_AVMute_Set(CDevice * p_Device) ;
void RXINT_AVMute_Clear(CDevice * p_Device) ;
void RXINT_SetNewAVIInfo(CDevice * p_Device) ;
void RXINT_ResetAudio(CDevice * p_Device) ;
void RXINT_ResetHDCP(CDevice * p_Device) ;
//void TimerServiceISR() ;
void VideoTimerHandler(CDevice * p_Device) ;
void AudioTimerHandler(CDevice * p_Device) ;
void MuteProcessTimerHandler(CDevice * p_Device) ;

void AssignVideoTimerTimeout(CDevice * p_Device, USHORT TimeOut) ;
void ResetVideoTimerTimeout(CDevice * p_Device) ;
void SwitchVideoState(CDevice * p_Device, Video_State_Type state) ;

void AssignAudioTimerTimeout(CDevice * p_Device, USHORT TimeOut) ;
void ResetAudioTimerTimeout(CDevice * p_Device) ;
void SwitchAudioState(CDevice * p_Device, Audio_State_Type state) ;

//void EnableMuteProcessTimer() { MuteResumingTimer = MuteByPKG?MUTE_RESUMING_TIMEOUT:0 ; }
//void DisableMuteProcessTimer() { MuteResumingTimer = 0 ; }

void DumpSyncInfo(CDevice * p_Device, VTiming *pVTiming) ;
//void StartAutoMuteOffTimer() { MuteAutoOff = ON ; }
//void EndAutoMuteOffTimer() { MuteAutoOff = OFF ; }
void CDR_Reset(CDevice * p_Device) ;
//void Reset_SCDTOFF() ;

void SetVideoInputFormatWithoutInfoFrame(CDevice * p_Device, BYTE bInMode) ;
void SetColorimetryByMode(CDevice * p_Device/* PSYNC_INFO pSyncInfo */) ;
void SetVideoInputFormatWithInfoFrame(CDevice * p_Device) ;
BOOL SetColorimetryByInfoFrame(CDevice * p_Device) ;
void SetColorSpaceConvert(CDevice * p_Device) ;

//void HDCP_Reset() ;
void SetDVIVideoOutput(CDevice * p_Device) ;
void SetNewInfoVideoOutput(CDevice * p_Device) ;
void SetCAT6023VideoOutputFormat(CDevice * p_Device, BYTE bOutputMapping, BYTE bOutputType, BYTE bOutputColorMode);
void ResetAudio(CDevice * p_Device) ;
void SetHWMuteCTRL(CDevice * p_Device, BYTE AndMask, BYTE OrMask) ;
void SetAudioMute(CDevice * p_Device, BOOL bMute) ;
void SetVideoMute(CDevice * p_Device, BOOL bMute) ;






void InitCAT6023(CDevice * p_Device);
void PowerDownHDMI(CDevice * p_Device);
BOOL IsCAT6023Interlace(CDevice * p_Device);
WORD getCAT6023HorzTotal(CDevice * p_Device);
WORD getCAT6023HorzActive(CDevice * p_Device);
WORD getCAT6023HorzFrontPorch(CDevice * p_Device);
WORD getCAT6023HorzSyncWidth(CDevice * p_Device);
WORD getCAT6023HorzBackPorch(CDevice * p_Device);
WORD getCAT6023VertTotal(CDevice * p_Device);
WORD getCAT6023VertActive(CDevice * p_Device);
WORD getCAT6023VertFrontPorch(CDevice * p_Device);
WORD getCAT6023VertSyncToDE(CDevice * p_Device);
WORD getCAT6023VertSyncWidth(CDevice * p_Device);
WORD getCAT6023VertSyncBackPorch(CDevice * p_Device);
BYTE getCAT6023xCnt(CDevice * p_Device);
BOOL getCAT6023AudioInfo(CDevice * p_Device, BYTE *pbAudioSampleFreq, BYTE *pbValidCh);

BOOL GetAVIInfoFrame(CDevice * p_Device, BYTE *pData);
BOOL GetAudioInfoFrame(CDevice * p_Device, BYTE *pData);
BOOL GetMPEGInfoFrame(CDevice * p_Device, BYTE *pData);
BOOL GetVENDORSPECInfoFrame(CDevice * p_Device, BYTE *pData);
BOOL GetACPPacket(CDevice * p_Device, BYTE *pData);



void getCAT6023Regs(CDevice * p_Device, BYTE *pData);
BYTE getCAT6023OutputColorMode(CDevice * p_Device);
BYTE getCAT6023OutputColorDepth(CDevice * p_Device);


BYTE    CAT6023FsGet(CDevice * p_Device);
BOOL    CAT6023HDAudioGet(CDevice * p_Device);
BOOL    CAT6023MultiPCM(CDevice * p_Device);
BYTE    CAT6023AudioChannelNum(CDevice * p_Device);
void    CAT6023HBRMclkSet(CDevice * p_Device, BYTE cFs);


void getCAT6023InputAudio(CDevice * p_Device, AUDIO_CAPS *pAudioCaps);
void getCAT6023InputChStat(CDevice * p_Device, AUDIO_CAPS *pAudioCaps);
BYTE getCAT6023AudioStatus(CDevice * p_Device);
BOOL getCAT6023AudioChannelStatus(CDevice * p_Device, BYTE ucIEC60958ChStat[]);
void setCAT6023_HBROutput(CDevice * p_Device, BOOL HBR_SPDIF);
void setCAT6023_SPDIFOutput(CDevice * p_Device);
void setCAT6023_I2SOutput(CDevice * p_Device, BYTE src_enable);
void DumpCat6023Reg(CDevice * p_Device);



//private:
typedef struct _CIT6603 {

	BYTE					IT6603_initialized;// = 0;
	BYTE					ucDVISCDToffCNT;// = 0;
	Video_State_Type		VState;// = VSTATE_PwrOff;
	Audio_State_Type		AState;// = ASTATE_AudioOff ;
	USHORT					VideoCountingTimer;// = 0 ;
	USHORT					AudioCountingTimer;// = 0 ;
	USHORT					MuteResumingTimer;// = 0 ;
	BOOL					MuteAutoOff;// = FALSE;
	BYTE					bGetSyncFailCount;// = 0 ;
	BYTE					bOutputVideoMode;// = F_MODE_EN_UDFILT | F_MODE_RGB24 ;
	BOOL					EnaSWCDRRest;// = FALSE ;
	BYTE					bDisableAutoAVMute;// = 0 ;

	BYTE					bHDCPMode;// = 0 ;

	USHORT					SWResetTimeOut;// = FORCE_SWRESET_TIMEOUT;

	BYTE					ucHDMIAudioErrorCount;// = 0 ;

	BYTE					ucAudioSampleClock;// = DEFAULT_START_FIXED_AUD_SAMPLEFREQ ;

	BOOL					bIntPOL;// = FALSE ;
	BOOL					NewAVIInfoFrameF;// = FALSE ;
	BOOL					MuteByPKG;// = OFF ;
	BYTE					bInputVideoMode;// = 0;

	BYTE					prevAVIDB1;// = 0 ;
	BYTE					prevAVIDB2;// = 0 ;

	USHORT					currHTotal;// = 0;// ;
	BYTE					currXcnt;// = 0;// ;
	BOOL					currScanMode;// = 0;// ;

	AUDIO_CAPS				AudioCaps ;

	VTiming					s_CurrentVM ;

	BYTE					SCDTErrorCnt;// = 0;
	BYTE					ucCurrentHDMIPort;// = 0 ;
	BOOL					AcceptCDRReset;// = TRUE ;
	ULONG					previous_fps;// = 0;
//add
	ULONG					previous_audio_sampling_freq;// = 0;

} CIT6603;

void EnableMuteProcessTimer(CDevice * p_Device);
void DisableMuteProcessTimer(CDevice * p_Device);

void StartAutoMuteOffTimer(CDevice * p_Device);
void EndAutoMuteOffTimer(CDevice * p_Device);


#endif
