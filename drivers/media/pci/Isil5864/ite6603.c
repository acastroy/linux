///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <cat6023.c>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2010/08/10
//   @fileversion: CAT6023_SRC_1.15
//******************************************/


/*********************************************************************************
 * CAT6023 HDMI RX sample code                                                   *
 *********************************************************************************/
#include "ite6603.h"
#include "sa7160.h"
#include "fh8735.h"
#include "property.h"
#include "wrapper.h"

//only for compile
#ifdef AME_MZ0380
FH8735_SYS_CFG * p_sys_cfg;
#endif //AME_MZ0380

#ifdef AME_SA7160
BYTE    SA7160_GetITE6603Register( CDevice * pDevice, BYTE nIndex );

BOOL    SA7160_GetITE6603Register_EX( CDevice * pDevice, BYTE nIndex, ULONG bBytes, BYTE * pValue );

BOOLEAN SA7160_SetITE6603Register( CDevice * pDevice, BYTE nIndex, BYTE bValue );

BOOLEAN SA7160_SetITE6603Register_EX( CDevice * pDevice, BYTE nIndex, ULONG bBytes, BYTE * pValue );
#endif

#ifdef AME_FH8735
BYTE    FH8735_GetITE6603Register( CDevice * pDevice, BYTE nIndex );

BOOL    FH8735_GetITE6603Register_EX( CDevice * pDevice, BYTE nIndex, ULONG bBytes, BYTE * pValue );

BOOLEAN FH8735_SetITE6603Register( CDevice * pDevice, BYTE nIndex, BYTE bValue );

BOOLEAN FH8735_SetITE6603Register_EX( CDevice * pDevice, BYTE nIndex, ULONG bBytes, BYTE * pValue );
#endif

BYTE GetVerticalFreq( CDevice * p_Device, WORD *Vreq  );

#define MAX_IT6003_i2C_NUM 20

#ifndef DEBUG_PORT_ENABLE
#define DEBUG_PORT_ENABLE 0
#else
#pragma message("DEBUG_PORT_ENABLE defined\n")
#endif
/*
#define SetSPDIFMUTE(x) SetMUTE(~(1<<O_TRI_SPDIF),(x)?(1<<O_TRI_SPDIF):0)
#define SetI2S3MUTE(x) SetMUTE(~(1<<O_TRI_I2S3),  (x)?(1<<O_TRI_I2S3):0)
#define SetI2S2MUTE(x) SetMUTE(~(1<<O_TRI_I2S2),  (x)?(1<<O_TRI_I2S2):0)
#define SetI2S1MUTE(x) SetMUTE(~(1<<O_TRI_I2S1),  (x)?(1<<O_TRI_I2S1):0)
#define SetI2S0MUTE(x) SetMUTE(~(1<<O_TRI_I2S0),  (x)?(1<<O_TRI_I2S0):0)
#define SetALLMute() SetMUTE(B_VDO_MUTE_DISABLE,(B_VDO_MUTE_DISABLE|B_TRI_ALL))
*/

//#define Switch_HDMIRX_Bank(x) HDMIRX_WriteI2C_Byte( p_Device, REG_RX_BANK, (x)&1)

char * VStateStr[] = {
    "VSTATE_PwrOff",
    "VSTATE_SyncWait ",
    "VSTATE_SWReset",
    "VSTATE_SyncChecking",
    "VSTATE_HDCPSet",
    "VSTATE_HDCP_Reset",
    "VSTATE_ModeDetecting",
    "VSTATE_VideoOn",
    "VSTATE_Reserved"
} ;

char *AStateStr[] = {
    "ASTATE_AudioOff",
    "ASTATE_RequestAudio",
    "ASTATE_ResetAudio",
    "ASTATE_WaitForReady",
    "ASTATE_AudioOn",
    "ASTATE_Reserved"
};

#if defined(OUTPUT_CCIR656)
    #pragma message("OUTPUT_CCIR656 defined")
    #define HDMIRX_OUTPUT_MAPPING   (B_OUTPUT_16BIT)
    #define HDMIRX_OUTPUT_TYPE (B_SYNC_EMBEDDED|B_CCIR565)
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_SYNC_EMBEDDED)
    #pragma message("OUTPUT_SYNC_EMBEDDED defined")
    #define HDMIRX_OUTPUT_MAPPING   (B_OUTPUT_16BIT)
    #define HDMIRX_OUTPUT_TYPE (B_SYNC_EMBEDDED)
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_16BIT_YUV422)
    #pragma message("OUTPUT_16BIT_YUV422 defined")
    #define HDMIRX_OUTPUT_MAPPING   (B_OUTPUT_16BIT)
//    #define HDMIRX_OUTPUT_TYPE 0
    #define HDMIRX_OUTPUT_TYPE 0x50 //embedded sync

    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_24BIT_YUV422)
    #pragma message("OUTPUT_24BIT_YUV422 defined")
    #define HDMIRX_OUTPUT_MAPPING   0
    #define HDMIRX_OUTPUT_TYPE  0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_24BIT_YUV444)
    #pragma message("OUTPUT_24BIT_YUV444 defined")
    #define HDMIRX_OUTPUT_MAPPING   0
    #define HDMIRX_OUTPUT_TYPE  0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV444<<O_OUTPUT_COLOR_MODE)
#else
    #pragma message("OUTPUT_24BIT_RGB444 defined")
    #define HDMIRX_OUTPUT_MAPPING   0
    #define HDMIRX_OUTPUT_TYPE  0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_RGB24<<O_OUTPUT_COLOR_MODE)
#endif

#define DEFAULT_START_FIXED_AUD_SAMPLEFREQ AUDFS_192KHz

//BYTE	ucDVISCDToffCNT=0;
//Video_State_Type VState = VSTATE_PwrOff ;
//Audio_State_Type AState = ASTATE_AudioOff ;

//static _IDATA USHORT VideoCountingTimer = 0 ;
//static _IDATA USHORT AudioCountingTimer = 0 ;
//static _IDATA USHORT MuteResumingTimer = 0 ;
//static BOOL MuteAutoOff = FALSE ;
//static _IDATA BYTE bGetSyncFailCount = 0 ;
//static BYTE _IDATA bOutputVideoMode = F_MODE_EN_UDFILT | F_MODE_RGB24 ;
//static BOOL EnaSWCDRRest = FALSE ;
//BYTE _XDATA bDisableAutoAVMute = 0 ;

//BYTE _XDATA bHDCPMode = 0 ;

/*
#define LOOP_MSEC 32
#define MS_TimeOut(x) (((x)+LOOP_MSEC-1)/LOOP_MSEC)

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
*/


//static _XDATA USHORT SWResetTimeOut = FORCE_SWRESET_TIMEOUT;

//static _XDATA BYTE ucHDMIAudioErrorCount = 0 ;

//static _XDATA BYTE ucAudioSampleClock = DEFAULT_START_FIXED_AUD_SAMPLEFREQ ;

//BOOL bIntPOL = FALSE ;
//static BOOL NewAVIInfoFrameF = FALSE ;
//static BOOL MuteByPKG = OFF ;
//static _XDATA BYTE bInputVideoMode ;

//static _XDATA BYTE prevAVIDB1 = 0 ;
//static _XDATA BYTE prevAVIDB2 = 0 ;

//static _XDATA USHORT currHTotal ;
//static _XDATA BYTE currXcnt ;
//static BOOL currScanMode ;

//static BOOL bGetSyncInfo() ;

//static _XDATA AUDIO_CAPS AudioCaps ;

//_XDATA VTiming s_CurrentVM ;

//BYTE SCDTErrorCnt = 0;

//#define USE_MODE_TABLE 1

#ifdef USE_MODE_TABLE
static VTiming s_VMTable[] = {
    {640,480,800,525,25175L,0x89,16,96,48,10,2,33,PROG,Vneg,Hneg},
    {720,480,858,525,27000L,0x80,16,62,60,9,6,30,PROG,Vneg,Hneg},
    {1280,720,1650,750,74000L,0x2E,110,40,220,5,5,20,PROG,Vpos,Hpos},
    {1920,540,2200,562,74000L,0x2E,88,44,148,2,5,15,INTERLACE,Vpos,Hpos},
    {720,240,858,262,13500L,0xFF,19,62,57,4,3,15,INTERLACE,Vneg,Hneg},
    {720,240,858,262,13500L,0xFF,19,62,57,4,3,15,PROG,Vneg,Hneg},
    {1440,240,1716,262,27000L,0x80,38,124,114,5,3,15,INTERLACE,Vneg,Hneg},
    {1440,240,1716,263,27000L,0x80,38,124,114,5,3,15,PROG,Vneg,Hneg},
    {2880,240,3432,262,54000L,0x40,76,248,288,4,3,15,INTERLACE,Vneg,Hneg},
    {2880,240,3432,262,54000L,0x40,76,248,288,4,3,15,PROG,Vneg,Hneg},
    {2880,240,3432,263,54000L,0x40,76,248,288,5,3,15,PROG,Vneg,Hneg},
    {1440,480,1716,525,54000L,0x40,32,124,120,9,6,30,PROG,Vneg,Hneg},
    {1920,1080,2200,1125,148352L,0x17,88,44,148,4,5,36,PROG,Vpos,Hpos},
    {720,576,864,625,27000L,0x80,12,64,68,5,5,36,PROG,Vneg,Hneg},
    {1280,720,1980,750,74000L,0x2E,440,40,220,5,5,20,PROG,Vpos,Hpos},
    {1920,540,2640,562,74000L,0x2E,528,44,148,2,5,15,INTERLACE,Vpos,Hpos},
    {1440/2,288,1728/2,312,13500L,0xFF,24/2,126/2,138/2,2,3,19,INTERLACE,Vneg,Hneg},
    {1440,288,1728,312,27000L,0x80,24,126,138,2,3,19,INTERLACE,Vneg,Hneg},
    {1440/2,288,1728/2,312,13500L,0xFF,24/2,126/2,138/2,2,3,19,PROG,Vneg,Hneg},
    {1440,288,1728,313,27000L,0x80,24,126,138,3,3,19,PROG,Vneg,Hneg},
    {1440,288,1728,314,27000L,0x80,24,126,138,4,3,19,PROG,Vneg,Hneg},
    {2880,288,3456,312,54000L,0x40,48,252,276,2,3,19,INTERLACE,Vneg,Hneg},
    {2880,288,3456,312,54000L,0x40,48,252,276,2,3,19,PROG,Vneg,Hneg},
    {2880,288,3456,313,54000L,0x40,48,252,276,3,3,19,PROG,Vneg,Hneg},
    {2880,288,3456,314,54000L,0x40,48,252,276,4,3,19,PROG,Vneg,Hneg},
    {1440,576,1728,625,54000L,0x40,24,128,136,5,5,39,PROG,Vpos,Hneg},
    {1920,1080,2640,1125,148000L,0x17,528,44,148,4,5,36,PROG,Vpos,Hpos},
    {1920,1080,2750,1125,74000L,0x2E,638,44,148,4,5,36,PROG,Vpos,Hpos},
    {1920,1080,2640,1125,74000L,0x2E,528,44,148,4,5,36,PROG,Vpos,Hpos},
    {1920,1080,2200,1125,74000L,0x2E,88,44,148,4,5,36,PROG,Vpos,Hpos},

    {640,350,832,445,31500L,0x6D,32,64,96,32,3,60,PROG,Vneg,Hpos},
    {640,400,832,445,31500L,0x6D,32,64,96,1,3,41,PROG,Vneg,Hneg},
    {832,624,1152,667,57283L,0x3C,32,64,224,1,3,39,PROG,Vneg,Hneg},
    {720,350,900,449,28322L,0x7A,18,108,54,59,2,38,PROG,Vneg,Hneg},
    {720,400,900,449,28322L,0x7A,18,108,54,13,2,34,PROG,Vpos,Hneg},
    {720,400,936,446,35500L,0x61,36,72,108,1,3,42,PROG,Vpos,Hneg},
    {640,480,800,525,25175L,0x89,16,96,48,10,2,33,PROG,Vneg,Hneg},
    {640,480,832,520,31500L,0x6D,24,40,128,9,3,28,PROG,Vneg,Hneg},
    {640,480,840,500,31500L,0x6D,16,64,120,1,3,16,PROG,Vneg,Hneg},
    {640,480,832,509,36000L,0x60,56,56,80,1,3,25,PROG,Vneg,Hneg},
    {800,600,1024,625,36000L,0x60,24,72,128,1,2,22,PROG,Vpos,Hpos},
    {800,600,1056,628,40000L,0x56,40,128,88,1,4,23,PROG,Vpos,Hpos},
    {800,600,1040,666,50000L,0x45,56,120,64,37,6,23,PROG,Vpos,Hpos},
    {800,600,1056,625,49500L,0x45,16,80,160,1,3,21,PROG,Vpos,Hpos},
    {800,600,1048,631,56250L,0x3D,32,64,152,1,3,27,PROG,Vpos,Hpos},
    {848,480,1088,517,33750L,0x66,16,112,112,6,8,23,PROG,Vpos,Hpos},
    {1024,384,1264,408,44900L,0x4d,8,176,56,0,4,20,INTERLACE,Vpos,Hpos},
    {1024,768,1344,806,65000L,0x35,24,136,160,3,6,29,PROG,Vneg,Hneg},
    {1024,768,1328,806,75000L,0x2E,24,136,144,3,6,29,PROG,Vneg,Hneg},
    {1024,768,1312,800,78750L,0x2B,16,96,176,1,3,28,PROG,Vpos,Hpos},
    {1024,768,1376,808,94500L,0x24,48,96,208,1,3,36,PROG,Vpos,Hpos},
    {1152,864,1600,900,108000L,0x20,64,128,256,1,3,32,PROG,Vpos,Hpos},
    {1280,768,1440,790,68250L,0x32,48,32,80,3,7,12,PROG,Vneg,Hpos},
    {1280,768,1664,798,79500L,0x2B,64,128,192,3,7,20,PROG,Vpos,Hneg},
    {1280,768,1696,805,102250L,0x21,80,128,208,3,7,27,PROG,Vpos,Hneg},
    {1280,768,1712,809,117500L,0x1D,80,136,216,3,7,31,PROG,Vpos,Hneg},
    {1280,960,1800,1000,108000L,0x20,96,112,312,1,3,36,PROG,Vpos,Hpos},
    {1280,960,1728,1011,148500L,0x17,64,160,224,1,3,47,PROG,Vpos,Hpos},
    {1280,1024,1688,1066,108000L,0x20,48,112,248,1,3,38,PROG,Vpos,Hpos},
    {1280,1024,1688,1066,135000L,0x19,16,144,248,1,3,38,PROG,Vpos,Hpos},
    {1280,1024,1728,1072,157500L,0x15,64,160,224,1,3,44,PROG,Vpos,Hpos},
    {1360,768,1792,795,85500L,0x28,64,112,256,3,6,18,PROG,Vpos,Hpos},
    {1400,1050,1560,1080,101000L,0x22,48,32,80,3,4,23,PROG,Vneg,Hpos},
    {1400,1050,1864,1089,121750L,0x1C,88,144,232,3,4,32,PROG,Vpos,Hneg},
    {1400,1050,1896,1099,156000L,0x16,104,144,248,3,4,42,PROG,Vpos,Hneg},
    {1400,1050,1912,1105,179500L,0x13,104,152,256,3,4,48,PROG,Vpos,Hneg},
    {1440,900,1600,926,88750L,0x26,48,32,80,3,6,17,PROG,Vneg,Hpos},
    {1440,900,1904,934,106500L,0x20,80,152,232,3,6,25,PROG,Vpos,Hneg},
    {1440,900,1936,942,136750L,0x19,96,152,248,3,6,33,PROG,Vpos,Hneg},
    {1440,900,1952,948,157000L,0x16,104,152,256,3,6,39,PROG,Vpos,Hneg},
    {1600,1200,2160,1250,162000L,0x15,64,192,304,1,3,46,PROG,Vpos,Hpos},
    {1600,1200,2160,1250,175500L,0x13,64,192,304,1,3,46,PROG,Vpos,Hpos},
    {1600,1200,2160,1250,189000L,0x12,64,192,304,1,3,46,PROG,Vpos,Hpos},
    {1600,1200,2160,1250,202500L,0x11,64,192,304,1,3,46,PROG,Vpos,Hpos},
    {1600,1200,2160,1250,229500L,0x0F,64,192,304,1,3,46,PROG,Vpos,Hpos},
    {1680,1050,1840,1080,119000L,0x1D,48,32,80,3,6,21,PROG,Vneg,Hpos},
    {1680,1050,2240,1089,146250L,0x17,104,176,280,3,6,30,PROG,Vpos,Hneg},
    {1680,1050,2272,1099,187000L,0x12,120,176,296,3,6,40,PROG,Vpos,Hneg},
    {1680,1050,2288,1105,214750L,0x10,128,176,304,3,6,46,PROG,Vpos,Hneg},
    {1792,1344,2448,1394,204750L,0x10,128,200,328,1,3,46,PROG,Vpos,Hneg},
    {1792,1344,2456,1417,261000L,0x0D,96,216,352,1,3,69,PROG,Vpos,Hneg},
    {1856,1392,2528,1439,218250L,0x0F,96,224,352,1,3,43,PROG,Vpos,Hneg},
    {1856,1392,2560,1500,288000L,0x0C,128,224,352,1,3,104,PROG,Vpos,Hneg},
    {1920,1200,2080,1235,154000L,0x16,48,32,80,3,6,26,PROG,Vneg,Hpos},
    {1920,1200,2592,1245,193250L,0x11,136,200,336,3,6,36,PROG,Vpos,Hneg},
    {1920,1200,2608,1255,245250L,0x0E,136,208,344,3,6,46,PROG,Vpos,Hneg},
    {1920,1200,2624,1262,281250L,0x0C,144,208,352,3,6,53,PROG,Vpos,Hneg},
    {1920,1440,2600,1500,234000L,0x0E,128,208,344,1,3,56,PROG,Vpos,Hneg},
    {1920,1440,2640,1500,297000L,0x0B,144,224,352,1,3,56,PROG,Vpos,Hneg},
};

#define     SizeofVMTable    (sizeof(s_VMTable)/sizeof(VTiming))
#else
#define     SizeofVMTable    0
#endif

static BYTE bCSCOffset_16_235[] =
{
    0x00, 0x80, 0x00
};

static BYTE bCSCOffset_0_255[] =
{
    0x10, 0x80, 0x10
};

#ifdef OUTPUT_YUV
    BYTE bCSCMtx_RGB2YUV_ITU601_16_235[] =
    {
        0xB2,0x04,0x64,0x02,0xE9,0x00,
        0x93,0x3C,0x18,0x04,0x56,0x3F,
        0x49,0x3D,0x9F,0x3E,0x18,0x04
    } ;

    BYTE bCSCMtx_RGB2YUV_ITU601_0_255[] =
    {
        0x09,0x04,0x0E,0x02,0xC8,0x00,
        0x0E,0x3D,0x84,0x03,0x6E,0x3F,
        0xAC,0x3D,0xD0,0x3E,0x84,0x03
    } ;

    BYTE bCSCMtx_RGB2YUV_ITU709_16_235[] =
    {
        0xB8,0x05,0xB4,0x01,0x93,0x00,
        0x49,0x3C,0x18,0x04,0x9F,0x3F,
        0xD9,0x3C,0x10,0x3F,0x18,0x04
    } ;

    BYTE bCSCMtx_RGB2YUV_ITU709_0_255[] =
    {
        0xE5,0x04,0x78,0x01,0x81,0x00,
        0xCE,0x3C,0x84,0x03,0xAE,0x3F,
        0x49,0x3D,0x33,0x3F,0x84,0x03
    } ;
#endif

#ifdef OUTPUT_RGB

    BYTE bCSCMtx_YUV2RGB_ITU601_16_235[] =
    {
        0x00,0x08,0x6A,0x3A,0x4F,0x3D,
        0x00,0x08,0xF7,0x0A,0x00,0x00,
        0x00,0x08,0x00,0x00,0xDB,0x0D
    } ;

    BYTE bCSCMtx_YUV2RGB_ITU601_0_255[] =
    {
        0x4F,0x09,0x81,0x39,0xDF,0x3C,
        0x4F,0x09,0xC2,0x0C,0x00,0x00,
        0x4F,0x09,0x00,0x00,0x1E,0x10
    } ;

    BYTE bCSCMtx_YUV2RGB_ITU709_16_235[] =
    {
        0x00,0x08,0x53,0x3C,0x89,0x3E,
        0x00,0x08,0x51,0x0C,0x00,0x00,
        0x00,0x08,0x00,0x00,0x87,0x0E
    } ;

    BYTE bCSCMtx_YUV2RGB_ITU709_0_255[] =
    {
        0x4F,0x09,0xBA,0x3B,0x4B,0x3E,
        0x4F,0x09,0x56,0x0E,0x00,0x00,
        0x4F,0x09,0x00,0x00,0xE7,0x10
    } ;
#endif

//static BYTE ucCurrentHDMIPort = 0 ;
//static BOOL AcceptCDRReset = TRUE ;


#ifndef _MCU_
void delay1ms(USHORT ms) ;
void ErrorF(char *fmt,...) ;
#endif


BYTE 
Get_IT6603_Initialization_Status(CDevice * p_Device)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	return (BYTE)(p_sys_cfg->o_input_ite6603.IT6603_initialized);
}


void
Check_HDMInterrupt(CDevice * p_Device)
{
	Interrupt_Handler(p_Device) ;
}

BYTE HDMIRX_ReadI2C_Byte(CDevice * pDevice, BYTE RegAddr)
{
	BYTE p_data = 0;

	if(pDevice)
	{
		#ifdef AME_SA7160
		p_data = SA7160_GetITE6603Register( pDevice, RegAddr );
		#endif

		#ifdef AME_FH8735
		p_data = FH8735_GetITE6603Register( pDevice, RegAddr );
		#endif
	}

	return p_data;
}


BOOL HDMIRX_WriteI2C_Byte(CDevice * pDevice, BYTE RegAddr,BYTE d)
{
	BOOL flag = 0;

	if(pDevice)
	{
		#ifdef AME_SA7160
		flag = SA7160_SetITE6603Register( pDevice, RegAddr, d );
		#endif

		#ifdef AME_FH8735
		flag = FH8735_SetITE6603Register( pDevice, RegAddr, d );
		#endif
	}

	return flag;
}

BOOL HDMIRX_ReadI2C_ByteN(CDevice * pDevice, BYTE RegAddr,BYTE *pData,int N)
{
	BOOL flag = 0;

	if(pDevice)
	{
		#ifdef AME_SA7160
		flag = SA7160_GetITE6603Register_EX( pDevice, RegAddr, N, pData );
		#endif

		#ifdef AME_FH8735
		flag = FH8735_GetITE6603Register_EX( pDevice, RegAddr, N, pData );
		#endif
	}

	return flag;
}

BOOL HDMIRX_WriteI2C_ByteN(CDevice * pDevice, BYTE RegAddr,BYTE *pData,int N)
{
	BOOL flag = 0;

	if(pDevice)
	{
		#ifdef AME_SA7160
		flag = SA7160_SetITE6603Register_EX( pDevice, RegAddr, N, pData );
		#endif

		#ifdef AME_FH8735
		flag = FH8735_SetITE6603Register_EX( pDevice, RegAddr, N, pData );
		#endif

	}
	return flag;
}
void Switch_HDMIRX_Bank(CDevice * p_Device, BYTE x)
{
	HDMIRX_WriteI2C_Byte(p_Device, 0x0f,(x)&1);
}
void SetHWMuteClrMode(CDevice * p_Device)
{
	{ SetHWMuteCTRL(p_Device, (~B_HW_AUDMUTE_CLR_MODE),(B_HW_AUDMUTE_CLR_MODE)) ;}
}	
void SetHWMuteClr(CDevice * p_Device)
{
	{ SetHWMuteCTRL(p_Device, (~B_HW_MUTE_CLR),(B_HW_MUTE_CLR)) ; }
}
void ClearHWMuteClr(CDevice * p_Device)
{
	{ SetHWMuteCTRL(p_Device, (~B_HW_MUTE_CLR),0) ; }
}
void SetALLMute(CDevice * p_Device)
{
	SetMUTE(p_Device, B_VDO_MUTE_DISABLE,(B_VDO_MUTE_DISABLE|B_TRI_ALL));
}

void delay1ms(USHORT ms) 
{
	wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( ms ) );
}

BOOL
CheckHDMIRX(CDevice * p_Device)
{
	
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif
	
    Timer_Handler(p_Device) ;
    Video_Handler(p_Device) ;

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn && (!p_sys_cfg->o_input_ite6603.MuteByPKG))
    {
        return TRUE ;
    }

    return FALSE ;
}

void
SelectHDMIPort(CDevice * p_Device, BYTE ucPort)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    if(ucPort != CAT_HDMI_PORTA)
    {
        ucPort = CAT_HDMI_PORTB ;
    }

    if( ucPort != p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort )
    {
        p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort = ucPort ;
    }

    //LINUXV4L2_DEBUG( KERN_INFO, "SelectHDMIPort ucPort = %d, ucCurrentHDMIPort = %d\n",ucPort, p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort);

}

BYTE
GetCurrentHDMIPort(CDevice * p_Device)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	return (BYTE)(p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort) ;
}

void
InitCAT6023(CDevice * p_Device)
{
    LINUXV4L2_DEBUG( KERN_INFO, "InitCAT6023()\n");

	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif
/////////////////////////////////////////////////////////////////////////////
	p_sys_cfg->o_input_ite6603.IT6603_initialized = 0;
	p_sys_cfg->o_input_ite6603.ucDVISCDToffCNT = 0;
	p_sys_cfg->o_input_ite6603.VState = VSTATE_PwrOff;
	p_sys_cfg->o_input_ite6603.AState = ASTATE_AudioOff ;
	p_sys_cfg->o_input_ite6603.VideoCountingTimer = 0 ;
	p_sys_cfg->o_input_ite6603.AudioCountingTimer = 0 ;
	p_sys_cfg->o_input_ite6603.MuteResumingTimer = 0 ;
	p_sys_cfg->o_input_ite6603.MuteAutoOff = FALSE;
	p_sys_cfg->o_input_ite6603.bGetSyncFailCount = 0 ;
	p_sys_cfg->o_input_ite6603.bOutputVideoMode = F_MODE_EN_UDFILT | F_MODE_RGB24 ;
	p_sys_cfg->o_input_ite6603.EnaSWCDRRest = FALSE ;
	p_sys_cfg->o_input_ite6603.bDisableAutoAVMute = 0 ;

	p_sys_cfg->o_input_ite6603.bHDCPMode = 0 ;

	p_sys_cfg->o_input_ite6603.SWResetTimeOut = FORCE_SWRESET_TIMEOUT;

	p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount = 0 ;

	p_sys_cfg->o_input_ite6603.ucAudioSampleClock = DEFAULT_START_FIXED_AUD_SAMPLEFREQ ;

	p_sys_cfg->o_input_ite6603.bIntPOL = FALSE ;
	p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF = FALSE ;
	p_sys_cfg->o_input_ite6603.MuteByPKG = OFF ;
//	p_sys_cfg->o_input_ite6603.bInputVideoMode = 0;

	p_sys_cfg->o_input_ite6603.prevAVIDB1 = 0 ;
	p_sys_cfg->o_input_ite6603.prevAVIDB2 = 0 ;

	p_sys_cfg->o_input_ite6603.currHTotal = 0;// ;
	p_sys_cfg->o_input_ite6603.currXcnt = 0;// ;
	p_sys_cfg->o_input_ite6603.currScanMode = 0;// ;


	p_sys_cfg->o_input_ite6603.SCDTErrorCnt = 0;
	p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort = 0 ;
	p_sys_cfg->o_input_ite6603.AcceptCDRReset = TRUE ;
	p_sys_cfg->o_input_ite6603.previous_fps = 0;
	p_sys_cfg->o_input_ite6603.previous_audio_sampling_freq = 0;
///////////////////////////////////////////////////////////////////////////

	BYTE uc ;

    //HWReset_HDMIRX() ;

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PWD_CTRL0, 0) ;

    #if 1
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_REGRST) ;
    delay1ms(1) ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, (REG_RX_HDCP_CTRL)) ;

    #endif
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_SWRST|B_CDRRST) ;
    delay1ms(1) ;

	if (p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort==CAT_HDMI_PORTA)
	{
		uc = B_PORT_SEL_A|B_PWD_AFEALL|B_PWDC_ETC ;
	}
	else
	{
		uc = B_PORT_SEL_B|B_PWD_AFEALL|B_PWDC_ETC ;
	}
	HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PWD_CTRL1, uc);
    LINUXV4L2_DEBUG( KERN_INFO, "InitCAT6023(): reg07 = %02X, ucCurrentHDMIPort = %d\n", (int)HDMIRX_ReadI2C_Byte(p_Device, 07), (int)p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort);

    SetIntMask1(p_Device, 0,B_PWR5VON|B_SCDTON|B_PWR5VOFF|B_SCDTOFF) ;
    SetIntMask2(p_Device, 0,B_NEW_AVI_PKG|B_PKT_SET_MUTE|B_PKT_CLR_MUTE) ;
    SetIntMask3(p_Device, 0,B_ECCERR|B_R_AUTH_DONE|B_R_AUTH_START) ;
    SetIntMask4(p_Device, 0,B_M_RXCKON_DET) ;

    SetDefaultRegisterValue(p_Device) ;
    LoadCustomizeDefaultSetting(p_Device) ;

    SetALLMute(p_Device) ;

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0) ;
    p_sys_cfg->o_input_ite6603.bDisableAutoAVMute = FALSE ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, 0x04) ;
    if( uc == 0xA2 )
    {
		#ifdef MEDICAL
		{

			HDMIRX_WriteI2C_Byte(p_Device, REG_RX_HDCP_CTRL, 0x08) ;

			//HDMIRX_PRINTF(("Check register 0x11(0x%x)\n", HDMIRX_ReadI2C_Byte(p_Device, REG_RX_HDCP_CTRL)));

			HDMIRX_WriteI2C_Byte(p_Device, REG_RX_HDCP_CTRL, 0x18) ;

			//HDMIRX_PRINTF(("Check register 0x11(0x%x)\n", HDMIRX_ReadI2C_Byte(p_Device, REG_RX_HDCP_CTRL)));
		}
		#else 
		{

			HDMIRX_WriteI2C_Byte(p_Device, REG_RX_HDCP_CTRL, 0x09) ;

			//HDMIRX_PRINTF(("Check register 0x11(0x%x)\n", HDMIRX_ReadI2C_Byte(p_Device, REG_RX_HDCP_CTRL)));

			HDMIRX_WriteI2C_Byte(p_Device, REG_RX_HDCP_CTRL, 0x19) ;

			//HDMIRX_PRINTF(("Check register 0x11(0x%x)\n", HDMIRX_ReadI2C_Byte(p_Device, REG_RX_HDCP_CTRL)));
		}
		#endif
    }
    else
    {
		#ifdef MEDICAL
		{

    		uc = B_EXTROM | B_HDCP_ROMDISWR | B_HDCP_EN ;

			uc &= ~(B_HDCP_EN);

			HDMIRX_WriteI2C_Byte(p_Device, REG_RX_HDCP_CTRL, uc) ;
		}
		#else
		{

    		uc = B_EXTROM | B_HDCP_ROMDISWR | B_HDCP_EN ;

			HDMIRX_WriteI2C_Byte(p_Device, REG_RX_HDCP_CTRL, uc) ;
		}
		#endif
    }
	delay1ms(200) ;

    RDROM_Reset(p_Device) ;

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_SWRST) ;
    delay1ms(1) ;
    SetALLMute(p_Device) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0) ;

    Terminator_Reset(p_Device) ;

	SwitchVideoState(p_Device, VSTATE_PwrOff) ;

#ifdef SUPPORT_REPEATER
	RxHDCPRepeaterCapabilityClear(B_ENABLE_FEATURE_1P1|B_ENABLE_FAST);
    if( p_sys_cfg->o_input_ite6603.bHDCPMode & HDCP_REPEATER )
    {
	    RxHDCPRepeaterCapabilitySet(B_ENABLE_REPEATER);
	    RxHDCPRepeaterCapabilityClear(B_KSV_READY);
	}
	else
	{
	    RxHDCPRepeaterCapabilityClear(B_KSV_READY|B_ENABLE_REPEATER);
        SetIntMask3(p_Device, ~(B_R_AUTH_DONE|B_R_AUTH_START),B_ECCERR) ;
	}
#else
	HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CDEPTH_CTRL,HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CDEPTH_CTRL)&0xF);
#endif
    p_sys_cfg->o_input_ite6603.AcceptCDRReset = TRUE;
	
	p_sys_cfg->o_input_ite6603.IT6603_initialized = 1;
}

void PowerDownHDMI(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "PowerDownHDMI()\n");

	HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PWD_CTRL1, B_PWD_AFEALL|B_PWDC_ETC|B_PWDC_SRV|B_EN_AUTOPWD) ;
	HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PWD_CTRL0, B_PWD_ALL) ;
}

BOOL IsCAT6023Interlace(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "IsCAT6023Interlace()\n");

	if(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_MODE)&B_INTERLACE)
    {
        return TRUE ;
    }
    return FALSE ;
}

WORD getCAT6023HorzTotal(CDevice * p_Device)
{
    BYTE uc[2] ;
	WORD hTotal ;

	uc[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HTOTAL_L) ;
	uc[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HTOTAL_H) ;
	hTotal = (WORD)(uc [1] & M_HTOTAL_H) ;
	hTotal <<= 8 ;
	hTotal |= (WORD)uc[0] ;

	//LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023HorzTotal() hTotal(%d)\n", hTotal);

	return hTotal ;
}

WORD getCAT6023HorzActive(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023HorzActive()\n");
    BYTE uc[3] ;

	WORD hTotal, hActive ;

	uc[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HTOTAL_L) ;
	uc[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HTOTAL_H) ;
	uc[2] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HACT_L) ;

	hTotal = (WORD)(uc [1] & M_HTOTAL_H) ;
	hTotal <<= 8 ;
	hTotal |= (WORD)uc[0] ;

	hActive = (WORD)(uc[1] >> O_HACT_H)& M_HACT_H ;
	hActive <<= 8 ;
	hActive |= (WORD)uc[2] ;

	if( (hActive | (1<<11)) < hTotal )
	{
		hActive |= 1<<11 ;
	}

	return hActive ;

}

WORD getCAT6023HorzFrontPorch(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023HorzFrontPorch()\n");

	BYTE uc[2] ;
	WORD hFrontPorch ;

	uc[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_H_FT_PORCH_L) ;
	uc[1] = (HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HSYNC_WID_H) >> O_H_FT_PORCH ) & M_H_FT_PORCH ;
	hFrontPorch = (WORD)uc[1] ;
	hFrontPorch <<= 8 ;
	hFrontPorch |= (WORD)uc[0] ;

	return hFrontPorch ;
}

WORD getCAT6023HorzSyncWidth(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023HorzSyncWidth()\n");

	BYTE uc[2] ;
	WORD hSyncWidth ;

	uc[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HSYNC_WID_L) ;
	uc[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HSYNC_WID_H)  & M_HSYNC_WID_H ;

	hSyncWidth = (WORD)uc[1] ;
	hSyncWidth <<= 8 ;
	hSyncWidth |= (WORD)uc[0] ;

	return hSyncWidth ;
}

WORD getCAT6023HorzBackPorch(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023HorzBackPorch()\n");

	WORD hBackPorch ;

	hBackPorch = getCAT6023HorzTotal(p_Device) - getCAT6023HorzActive(p_Device) - getCAT6023HorzFrontPorch(p_Device) - getCAT6023HorzSyncWidth(p_Device) ;

	return hBackPorch ;
}

WORD getCAT6023VertTotal(CDevice * p_Device)
{
    BYTE uc[3] ;
	WORD vTotal, vActive ;
	uc[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VTOTAL_L) ;
	uc[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VTOTAL_H) ;
	uc[2] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VACT_L) ;

	vTotal = (WORD)uc[1] & M_VTOTAL_H ;
	vTotal <<= 8 ;
	vTotal |= (WORD)uc[0] ;

	vActive = (WORD)(uc[1] >> O_VACT_H ) & M_VACT_H ;
	vActive |= (WORD)uc[2] ;

	if( vTotal > (vActive | (1<<10)))
	{
		vActive |= 1<<10 ;
	}

	#if 0
	if( vActive == 600 && vTotal == 634 )
	{
		vTotal = 666 ;
	}
	#endif

	//LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023VertTotal() vTotal(%d)\n", vTotal);

	return vTotal ;
}

WORD getCAT6023VertActive(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023VertActive()\n");

	BYTE uc[3] ;
	WORD vTotal, vActive ;
	uc[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VTOTAL_L) ;
	uc[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VTOTAL_H) ;
	uc[2] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VACT_L) ;

	vTotal = (WORD)uc[1] & M_VTOTAL_H ;
	vTotal <<= 8 ;
	vTotal |= (WORD)uc[0] ;

	vActive = (WORD)(uc[1] >> O_VACT_H ) & M_VACT_H ;
	vActive <<= 8 ;
	vActive |= (WORD)uc[2] ;

	if( vTotal > (vActive | (1<<10)))
	{
		vActive |= 1<<10 ;
	}

	return vActive ;
}

WORD getCAT6023VertFrontPorch(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023VertFrontPorch()\n");

	WORD vFrontPorch ;

	vFrontPorch = (WORD)HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_V_FT_PORCH) & 0xF ;

	if( getCAT6023VertActive(p_Device) == 600 && getCAT6023VertTotal(p_Device) == 666 )
	{
		vFrontPorch |= 0x20 ;
	}

	return vFrontPorch ;

}

WORD getCAT6023VertSyncToDE(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023VertSyncToDE()\n");
    WORD vSync2DE ;

    vSync2DE = (WORD)HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_VSYNC2DE) ;
    return vSync2DE ;
}

WORD getCAT6023VertSyncWidth(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023VertSyncWidth()\n");
    WORD vSync2DE ;
    WORD vTotal, vActive, hActive  ;

    vSync2DE = getCAT6023VertSyncToDE(p_Device) ;
    vTotal = getCAT6023VertTotal(p_Device) ;
    vActive = getCAT6023VertActive(p_Device) ;
    hActive = getCAT6023HorzActive(p_Device) ;
#ifndef CAT6023_A1

    if( vActive < 300 )
    {
    	return 3 ;
    }

    if( hActive == 640 && hActive == 480 )
    {
    	if( HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_XTALCNT_128PEL) < 0x80 )
    	{
    		return 3 ;
    	}

    	return 2;
    }

    return 5 ;
#endif
}

WORD getCAT6023VertSyncBackPorch(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023VertSyncBackPorch()\n");
    WORD vBackPorch ;

    vBackPorch = getCAT6023VertSyncToDE(p_Device) - getCAT6023VertSyncWidth(p_Device) ;
    return vBackPorch ;
}

BYTE getCAT6023xCnt(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023xCnt()\n");
    return HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_XTALCNT_128PEL) ;
}

BOOL getCAT6023AudioInfo(CDevice * p_Device, BYTE *pbAudioSampleFreq, BYTE *pbValidCh)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023AudioInfo()\n");

	if(IsCAT6023HDMIMode(p_Device))
    {
        if( pbAudioSampleFreq )
        {
            *pbAudioSampleFreq = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS) & M_Fs ;
        }

        if( pbValidCh )
        {
            *pbValidCh = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT) ;
            if( *pbValidCh & B_AUDIO_LAYOUT )
            {
                *pbValidCh &= M_AUDIO_CH ;
            }
            else
            {
                *pbValidCh = B_AUDIO_SRC_VALID_0 ;
            }
        }
        return TRUE ;
    }
    else
    {
        return FALSE ;
    }
}

#ifdef GET_PACKAGE

BOOL
GetAVIInfoFrame(CDevice * p_Device, BYTE *pData)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "GetAVIInfoFrame()\n");

    if( pData == NULL )
    {
        return ER_FAIL ;
    }

    pData[0] = AVI_INFOFRAME_TYPE ;
    pData[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_VER) ;
    pData[2] = AVI_INFOFRAME_LEN ;

    HDMIRX_ReadI2C_ByteN(p_Device, REG_RX_AVI_DB1, pData+3,AVI_INFOFRAME_LEN) ;

    return ER_SUCCESS ;
}

BOOL
GetAudioInfoFrame(CDevice * p_Device, BYTE *pData)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "GetAudioInfoFrame()\n");

    if( pData == NULL )
    {
        return FALSE ;
    }

    pData[0] = AUDIO_INFOFRAME_TYPE ;
    pData[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_VER) ;
    pData[2] = AUDIO_INFOFRAME_LEN ;

    HDMIRX_ReadI2C_ByteN(p_Device, REG_RX_AUDIO_DB1, pData+3,AUDIO_INFOFRAME_LEN) ;

    return TRUE ;
}

BOOL
GetMPEGInfoFrame(CDevice * p_Device, BYTE *pData)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "GetMPEGInfoFrame()\n");

    if( pData == NULL )
    {
        return FALSE ;
    }

    pData[0] = MPEG_INFOFRAME_TYPE ;
    pData[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MPEG_VER) ;
    pData[2] = MPEG_INFOFRAME_LEN ;

    HDMIRX_ReadI2C_ByteN(p_Device, REG_RX_MPEG_DB1, pData+3,MPEG_INFOFRAME_LEN) ;

    return TRUE ;
}

BOOL
GetVENDORSPECInfoFrame(CDevice * p_Device, BYTE *pData)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "GetVENDORSPECInfoFrame()\n");

    if( pData == NULL )
    {
        return FALSE ;
    }

    pData[0] = VENDORSPEC_INFOFRAME_TYPE ;
    pData[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VS_VER) ;
    pData[2] = VENDORSPEC_INFOFRAME_LEN ;

    HDMIRX_ReadI2C_ByteN(p_Device, REG_RX_VS_DB1, pData+3,VENDORSPEC_INFOFRAME_LEN) ;

    return TRUE ;
}

BOOL
GetACPPacket(CDevice * p_Device, BYTE *pData)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "GetACPPacket()\n");

    if( pData == NULL )
    {
        return FALSE ;
    }

    HDMIRX_ReadI2C_ByteN(p_Device, REG_RX_ACP_TYPE, pData,ACP_PKT_LEN) ;

    return TRUE ;
}

#endif

void
getCAT6023Regs(CDevice * p_Device, BYTE *pData)
{
    int i, j ;

    Switch_HDMIRX_Bank(p_Device, 0) ;
    for( i = j = 0 ; i < 256 ; i++,j++ )
    {
        pData[j] = HDMIRX_ReadI2C_Byte( p_Device, (BYTE)(i&0xFF)) ;
    }
    Switch_HDMIRX_Bank(p_Device, 1) ;
    for( i = 0xA0 ; i <= 0xF2 ; i++, j++ )
    {
        pData[j] = HDMIRX_ReadI2C_Byte( p_Device, (BYTE)(i&0xFF)) ;
    }
    Switch_HDMIRX_Bank(p_Device, 0) ;
}

BYTE
getCAT6023OutputColorMode(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023OutputColorMode()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif
 
	return (BYTE)(p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_CLRMOD_MASK) ;
}

BYTE
getCAT6023OutputColorDepth(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023OutputColorDepth()\n");
    BYTE uc ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS) & M_GCP_CD ;
    return uc >> O_GCP_CD ;
}

void
Terminator_Off(CDevice * p_Device)
{
    BYTE uc ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_PWD_CTRL1) | (B_PWD_AFEALL|B_PWDC_ETC);
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PWD_CTRL1, uc ) ;
    //LINUXV4L2_DEBUG( KERN_INFO, "Terminator_Off, reg07 = %02x\n",(int)uc);
}

void
Terminator_On(CDevice * p_Device)
{
    BYTE uc ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_PWD_CTRL1) & ~(B_PWD_AFEALL|B_PWDC_ETC);
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PWD_CTRL1, uc) ;
    //LINUXV4L2_DEBUG( KERN_INFO, "Terminator_On, reg07 = %02x\n",(int)uc);
}

void
Terminator_Reset(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "Terminator_Reset()\n");
    Terminator_Off(p_Device) ;
    delay1ms(500) ;
    Terminator_On(p_Device) ;
}

void
RDROM_Reset(CDevice * p_Device)
{
    BYTE i ;
    BYTE uc ;

    //LINUXV4L2_DEBUG( KERN_INFO, "RDROM_Reset()\n");

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_RDROM_CLKCTRL) & ~(B_ROM_CLK_SEL_REG|B_ROM_CLK_VALUE) ;
    for(i=0 ;i < 16 ; i++ )
    {
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RDROM_CLKCTRL, B_ROM_CLK_SEL_REG|uc) ;
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RDROM_CLKCTRL, B_ROM_CLK_SEL_REG|B_ROM_CLK_VALUE|uc) ;
    }

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RDROM_CLKCTRL,uc) ;

}

void
Check_RDROM(CDevice * p_Device)
{
    BYTE uc ;
    //LINUXV4L2_DEBUG( KERN_INFO, "Check_HDCP_RDROM()\n");

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0) ;

    if( IsSCDT(p_Device) )
    {
        int count ;

        {
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_RDROM_STATUS) ;
            if( (uc & 0xF) != 0x9 )
            {
                RDROM_Reset(p_Device) ;
            }
            //LINUXV4L2_DEBUG( KERN_INFO, "Check_HDCP_RDROM() done.\n");
            return ;
        }
    }
}

void
SWReset_HDMIRX(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SWReset_HDMIRX()\n");

	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	Check_RDROM(p_Device) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_SWRST) ;
    delay1ms(1) ;
    SetALLMute(p_Device) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0) ;

    Terminator_Off(p_Device) ;
    SwitchVideoState(p_Device, VSTATE_SWReset) ;

    p_sys_cfg->o_input_ite6603.SCDTErrorCnt = 0;

}


static REGPAIR acCustomizeValue[] =
{
//original
//    {REG_RX_VCLK_CTRL, 0x30},
//    {REG_RX_VCLK_CTRL, 0x10},
//    {REG_RX_VCLK_CTRL, 0x00},//OK for 1080P60
//    {REG_RX_VCLK_CTRL, 0x20},////OK for 1080P60

	{REG_RX_VCLK_CTRL, 0x30},////OK for SC580HDMIN4


//    {REG_RX_I2S_CTRL,0x61},
//00:I2S mode
//01:right justified mode
//10:left justified mode

    {REG_RX_I2S_CTRL,0x60},
	
	
	{REG_RX_PG_CTRL2,HDMIRX_OUTPUT_COLORMODE},
	{REG_RX_VIDEO_MAP,HDMIRX_OUTPUT_MAPPING},
    {REG_RX_VIDEO_CTRL1,HDMIRX_OUTPUT_TYPE},
    {REG_RX_MCLK_CTRL, 0xC1},
    {0xFF,0xFF}
} ;

void
LoadCustomizeDefaultSetting(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "LoadCustomizeDefaultSetting()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	BYTE i, uc ;
    for( i = 0 ; acCustomizeValue[i].ucAddr != 0xFF ; i++ )
    {
        HDMIRX_WriteI2C_Byte( p_Device, acCustomizeValue[i].ucAddr,acCustomizeValue[i].ucValue) ;
    }

    /*
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_PG_CTRL2) & ~(M_OUTPUT_COLOR_MASK<<O_OUTPUT_COLOR_MODE);
    switch(p_sys_cfg->o_input_ite6603.bOutputVideoMode&F_MODE_CLRMOD_MASK)
    {
    case F_MODE_YUV444:
        uc |= B_OUTPUT_YUV444 << O_OUTPUT_COLOR_MODE ;
        break ;
    case F_MODE_YUV422:
        uc |= B_OUTPUT_YUV422 << O_OUTPUT_COLOR_MODE ;
        break ;
    }
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PG_CTRL2, uc) ;
    */
    p_sys_cfg->o_input_ite6603.bOutputVideoMode&=~F_MODE_CLRMOD_MASK;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_PG_CTRL2) & (M_OUTPUT_COLOR_MASK<<O_OUTPUT_COLOR_MODE);

    switch(uc)
    {
    case (B_OUTPUT_YUV444 << O_OUTPUT_COLOR_MODE): p_sys_cfg->o_input_ite6603.bOutputVideoMode|=F_MODE_YUV444; break ;
    case (B_OUTPUT_YUV422 << O_OUTPUT_COLOR_MODE): p_sys_cfg->o_input_ite6603.bOutputVideoMode|=F_MODE_YUV422; break ;
    case 0: p_sys_cfg->o_input_ite6603.bOutputVideoMode|=F_MODE_RGB444; break ;
    default: p_sys_cfg->o_input_ite6603.bOutputVideoMode|=F_MODE_RGB444; break ;
    }
    p_sys_cfg->o_input_ite6603.bIntPOL = (HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL) & B_INTPOL)?LO_ACTIVE:HI_ACTIVE ;
}

static REGPAIR acDefaultValue[] =
{
    {0x0F,0x00},
    {REG_RX_VIO_CTRL,0xAE},

    {REG_RX_DESKEW_CTRL, 0x40},

    {REG_RX_PLL_CTRL,0x03},

	{REG_RX_EQUAL_CTRL1,0x11},

    {REG_RX_EQUAL_CTRL2, 0x00},

    {0x93,0x43},
    {0x94,0x4F},
    {0x95,0x87},
    {0x96,0x33},

    {0xFF,0xFF}

} ;

void
SetDefaultRegisterValue(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetDefaultRegisterValue()\n");
    BYTE i ;

    for( i = 0 ; acDefaultValue[i].ucAddr != 0xFF ; i++ )
    {
        HDMIRX_WriteI2C_Byte( p_Device, acDefaultValue[i].ucAddr, acDefaultValue[i].ucValue ) ;
    }

}

void
ClearIntFlags(CDevice * p_Device, BYTE flag)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "ClearIntFlags()\n");
    BYTE uc ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL) ;
    uc &= FLAG_CLEAR_INT_MASK ;
    uc |= flag ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL,uc) ;
    delay1ms(1);
    uc &= FLAG_CLEAR_INT_MASK ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL,uc) ;
    delay1ms(1);

}

void
ClearHDCPIntFlags(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "ClearHDCPIntFlags()\n");
    BYTE uc ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL1) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL1, (BYTE)B_CLR_HDCP_INT|uc ) ;
    delay1ms(1);
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_CTRL1, uc&((BYTE)~B_CLR_HDCP_INT) ) ;
}

BOOL
IsSCDT(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "IsSCDT()\n");
    BYTE uc ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE) & (B_SCDT|B_VCLK_DET/*|B_PWR5V_DET*/) ;
    return (uc==(B_SCDT|B_VCLK_DET/*|B_PWR5V_DET*/))?TRUE:FALSE ;
}

#if 0

#endif

BOOL
CheckPlg5VPwr(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "CheckPlg5VPwr()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	BYTE uc ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE) ;

    if( p_sys_cfg->o_input_ite6603.ucCurrentHDMIPort == CAT_HDMI_PORTB )
    {
        return (uc&B_PWR5V_DET_PORTB)?TRUE:FALSE ;

    }

    return (uc&B_PWR5V_DET_PORTA)?TRUE:FALSE ;
}

#if 0

#endif

void
SetMUTE(CDevice * p_Device, BYTE AndMask, BYTE OrMask)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetMUTE()\n");
    BYTE uc ;

    if( AndMask )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;

    }
    uc &= AndMask ;
    uc |= OrMask ;
    #ifdef HDMI_REPEATER
    #pragma message("HDMI Repeating TTL to next stage, do not gatting the video sync.")
    uc &= 0x1F ;
    uc |= 0x80 ;
    #endif
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL,uc) ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;

}

#if 0

#endif

void
SetIntMask1(CDevice * p_Device, BYTE AndMask,BYTE OrMask)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetIntMask1()\n");
    BYTE uc ;
    if( AndMask != 0 )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK1) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK1, uc) ;
}

void
SetIntMask2(CDevice * p_Device, BYTE AndMask,BYTE OrMask)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetIntMask2()\n");
    BYTE uc ;
    if( AndMask != 0 )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK2) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK2, uc) ;
}

void
SetIntMask3(CDevice * p_Device, BYTE AndMask,BYTE OrMask)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetIntMask3()\n");
    BYTE uc ;
    if( AndMask != 0 )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK3) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK3, uc) ;
}

void
SetIntMask4(CDevice * p_Device, BYTE AndMask,BYTE OrMask)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetIntMask4()\n");
    BYTE uc ;
    if( AndMask != 0 )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK4) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_INTERRUPT_MASK4, uc) ;
}

#if 0
void
SetGeneralPktType(CDevice * p_Device, BYTE type)
{
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PKT_REC_TYPE,type) ;
}
#endif

BOOL
IsCAT6023HDMIMode(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "IsCAT6023HDMIMode()\n");
    BYTE uc ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE) ;

    return (uc&B_HDMIRX_MODE)?TRUE:FALSE ;
}

void
Interrupt_Handler(CDevice * p_Device)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	BYTE int1data = 0 ;
	BYTE int2data = 0 ;
	BYTE int3data = 0 ;
	BYTE int4data = 0 ;
	BYTE sys_state ;
	BYTE flag = FLAG_CLEAR_INT_ALL;
//PETER [2011.03.14]
    LINUXV4L2_DEBUG( KERN_INFO, "[%d]Interrupt_Handler() VState(0d%d) Resolution(0x%x) FrameRate(%d)\n", p_Device->m_nKsDeviceNumber, p_sys_cfg->o_input_ite6603.VState, p_Device->m_nCustomAnalogVideoResolutionProperty, p_Device->m_nCustomAnalogVideoFrameRateProperty );


//fix lock status
	if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
	{
		p_Device->m_nAnalogVideoDecoderStatusProperty = 1;
	}

	if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SWReset )
    {
    	return ;
    }

    sys_state = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE) ;

    int4data = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT4) ;
    int1data = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT1) ;


    if( int1data || (int4data&B_RXCKON_DET) )
    {
        //LINUXV4L2_DEBUG( KERN_INFO, "system state = %02X\n",(int)sys_state);
		//LINUXV4L2_DEBUG( KERN_INFO, "Interrupt 1 = %02X\n",(int)int1data);
        //LINUXV4L2_DEBUG( KERN_INFO, "Interrupt 4 = %02X\n",(int)int4data);
        ClearIntFlags(p_Device, B_CLR_MODE_INT) ;

		if(!CheckPlg5VPwr(p_Device))
		{
			if( p_sys_cfg->o_input_ite6603.VState != VSTATE_SWReset && p_sys_cfg->o_input_ite6603.VState != VSTATE_PwrOff )
			{
				SWReset_HDMIRX(p_Device) ;
				return ;
			}
		}

        if( int1data & B_PWR5VOFF )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "5V Power Off interrupt\n");
            RXINT_5V_PwrOff(p_Device) ;
        }

        if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SWReset )
        {
        	return ;
        }

        if( int1data & B_SCDTOFF )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "SCDT Off interrupt\n");
            RXINT_SCDT_Off(p_Device) ;
        }

        if( int1data & B_PWR5VON )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "5V Power On interrupt\n");
            RXINT_5V_PwrOn(p_Device) ;
        }

        if( int1data & B_VIDMODE_CHG )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "Video mode change interrupt.\n:");
            RXINT_VideoMode_Chg(p_Device) ;
            if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SWReset )
            {
            	return ;
            }
        }

        if( int1data & B_HDMIMODE_CHG )
        {
           // LINUXV4L2_DEBUG( KERN_INFO, "HDMI Mode change interrupt.\n");
            RXINT_HDMIMode_Chg(p_Device) ;
        }

        if( int1data & B_SCDTON )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "SCDT On interrupt\n");
			if((sys_state & 0x4) == 0)
				RXINT_VideoMode_Chg(p_Device);
			else
            RXINT_SCDT_On(p_Device) ;
        }

        if( int4data & B_RXCKON_DET )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "RXCKON DET interrupt\n");
            RXINT_RXCKON(p_Device) ;
        }
    }

    int2data = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_INTERRUPT2) ;
    if( int2data )
    {
        BYTE vid_stat = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_INPUT_ST) ;
        //LINUXV4L2_DEBUG( KERN_INFO, "Interrupt 2 = %02X\n",(int)int2data);
        ClearIntFlags(p_Device, B_CLR_PKT_INT|B_CLR_MUTECLR_INT|B_CLR_MUTESET_INT) ;

        if( int2data & B_PKT_SET_MUTE )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "AVMute set interrupt.\n" );
            RXINT_AVMute_Set(p_Device) ;
        }

        if( int2data & B_NEW_AVI_PKG )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "New AVI Info Frame Change interrupt\n");
            RXINT_SetNewAVIInfo(p_Device) ;
        }

        if( ( int2data & B_PKT_CLR_MUTE ))
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "AVMute clear interrupt.\n" );
            RXINT_AVMute_Clear(p_Device) ;
        }
    }

    int3data = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_INTERRUPT3) ;
    if( int3data &(B_R_AUTH_DONE|B_R_AUTH_START))
    {
        ClearHDCPIntFlags(p_Device) ;
    #ifdef SUPPORT_REPEATER
        if( p_sys_cfg->o_input_ite6603.bHDCPMode & HDCP_REPEATER )
        {
	        if( int3data & B_R_AUTH_START )
	        {
	            //LINUXV4L2_DEBUG( KERN_INFO, " B_R_AUTH_START\n");
	            SwitchRxHDCPState(RXHDCP_AuthStart) ;
	        }
	        if( int3data & B_R_AUTH_DONE )
	        {
	            //LINUXV4L2_DEBUG( KERN_INFO, "B_R_AUTH_DONE \n");
	            SwitchRxHDCPState(RXHDCP_AuthDone) ;
	        }
        }
    #endif
    }

	if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn || p_sys_cfg->o_input_ite6603.VState == VSTATE_HDCP_Reset)
	{

	    if( int3data &(B_ECCERR|B_AUDFIFOERR|B_AUTOAUDMUTE) )
	    {
	        ClearIntFlags(p_Device, B_CLR_AUDIO_INT|B_CLR_ECC_INT) ;
	        if( p_sys_cfg->o_input_ite6603.AState != ASTATE_AudioOff)
	        {
		        //LINUXV4L2_DEBUG( KERN_INFO, "Interrupt 3 = %02X\n",(int)int3data);
		        if( int3data & (B_AUTOAUDMUTE|B_AUDFIFOERR))
		        {
		            //LINUXV4L2_DEBUG( KERN_INFO, "Audio Error interupt\n");
		            RXINT_ResetAudio(p_Device) ;
		            SetIntMask3(p_Device, ~(B_AUTOAUDMUTE|B_AUDFIFOERR),0) ;
		        }
	        }

	        if( int3data & B_ECCERR )
	        {
	            //LINUXV4L2_DEBUG( KERN_INFO, "ECC error interrupt\n");
	            RXINT_ResetHDCP(p_Device) ;
	        }
	    }
	}

	// [PETER] [2011.01.24] DETECT HDCP
	//
	{
		if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
		{
		    BYTE HDCP_status = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_HDCP_STATUS);

            //LINUXV4L2_DEBUG( KERN_INFO, "HDCP_status(0x%x)\n", HDCP_status);

			if(HDCP_status & 0x1)
			{
				p_Device->m_nAnalogCopyProtMacrovisionProperty = 1;

				#ifdef MEDICAL
				{
					p_Device->m_nAnalogCopyProtMacrovisionProperty = 0;
				}
				#endif

			}
			else
			{
				p_Device->m_nAnalogCopyProtMacrovisionProperty = 0;
			}

		}
	}

	#ifdef DEBUG
    if( int1data | int2data )
    {
        int1data = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_INTERRUPT1);
        int2data = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_INTERRUPT2);
        int3data = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_INTERRUPT3);
        sys_state = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_SYS_STATE);
        /*
		LINUXV4L2_DEBUG( KERN_INFO, "%02X %02X %02X %02X\n",
            (int)int1data,
            (int)int2data,
            (int)int3data,
            (int)sys_state) ;

		*/
    }
    #endif

}

void
RXINT_5V_PwrOn(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_5V_PwrOn()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_PwrOff )
    {

        if( CheckPlg5VPwr(p_Device) )
        {
            SwitchVideoState(p_Device, VSTATE_SyncWait) ;
        }
    }
}

void
RXINT_5V_PwrOff(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_5V_PwrOff()\n");
    BYTE sys_state ;

    sys_state = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_SYS_STATE) ;

    SWReset_HDMIRX(p_Device) ;
}

void
RXINT_SCDT_On(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_SCDT_On()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SyncWait )
    {
        if(IsSCDT(p_Device))
        {
            SwitchVideoState(p_Device, VSTATE_SyncChecking) ;
        }
    }
}

void CDR_Reset(CDevice * p_Device)
{
    BYTE uc;
	//LINUXV4L2_DEBUG( KERN_INFO, "CDR_Reset()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    SetIntMask4(p_Device, 0,0) ;
    SetIntMask1(p_Device, 0,0) ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, 0x97) ;
    HDMIRX_WriteI2C_Byte( p_Device, 0x97,uc|0x20);

    if( p_sys_cfg->o_input_ite6603.EnaSWCDRRest )
    {
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_SWRST | B_CDRRST ) ;
    }
    else
    {
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_VDORST | B_CDRRST ) ;
    }

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CDEPTH_CTRL) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CDEPTH_CTRL,uc |B_RSTCD);

    uc = HDMIRX_ReadI2C_Byte( p_Device, 0x97) ;
    HDMIRX_WriteI2C_Byte( p_Device, 0x97,uc&(~0x20));

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0 ) ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CDEPTH_CTRL) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CDEPTH_CTRL,uc &(~B_RSTCD));

#ifdef SUPPORT_REPEATER
    RxHDCPRepeaterCapabilityClear(B_KSV_READY);
#endif

    SetIntMask4(p_Device, 0,B_M_RXCKON_DET) ;
    SetIntMask1(p_Device, 0,B_PWR5VON|B_SCDTON|B_PWR5VOFF) ;
    ClearIntFlags(p_Device, B_CLR_MODE_INT) ;

    p_sys_cfg->o_input_ite6603.AcceptCDRReset = FALSE ;
	p_sys_cfg->o_input_ite6603.ucDVISCDToffCNT=0;
}

void
RXINT_SCDT_Off(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_SCDT_Off()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    if( p_sys_cfg->o_input_ite6603.VState != VSTATE_PwrOff )
    {
        //LINUXV4L2_DEBUG( KERN_INFO, "GetSCDT OFF\n");
        SwitchVideoState(p_Device, VSTATE_SyncWait) ;

        p_sys_cfg->o_input_ite6603.SCDTErrorCnt++;
    }
}

void
RXINT_VideoMode_Chg(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_VideoMode_Chg()\n");
    BYTE sys_state ;

    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_VideoMode_Chg\n");

    sys_state = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE) ;

    if(CheckPlg5VPwr(p_Device))
    {
		CDR_Reset(p_Device);
        SwitchVideoState(p_Device, VSTATE_SyncWait) ;
    }
    else
    {
        SWReset_HDMIRX(p_Device) ;
    }
}

void
RXINT_HDMIMode_Chg(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_HDMIMode_Chg()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

   if(p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
    {
        if( IsCAT6023HDMIMode(p_Device) )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "HDMI Mode.\n");
            SwitchAudioState(p_Device, ASTATE_RequestAudio) ;

        }
        else
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "DVI Mode.\n");
            SwitchAudioState(p_Device, ASTATE_AudioOff) ;
            p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF = FALSE ;

            SetDVIVideoOutput(p_Device) ;

        }
    }
}

void RXINT_RXCKON(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_RXCKON()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    if( p_sys_cfg->o_input_ite6603.AcceptCDRReset == TRUE )
    {
        if((HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE)&(B_VCLK_DET|B_RXCK_VALID)) == (B_VCLK_DET|B_RXCK_VALID))
        {
            CDR_Reset(p_Device) ;
        }
    }
}

void
RXINT_AVMute_Set(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_AVMute_Set()\n");
	BYTE uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    p_sys_cfg->o_input_ite6603.MuteByPKG = ON ;

    SetAudioMute(p_Device, ON) ;
    SetVideoMute(p_Device, ON) ;
    StartAutoMuteOffTimer(p_Device) ;
    SetIntMask2(p_Device, ~(B_PKT_CLR_MUTE),(B_PKT_CLR_MUTE)) ;

    p_sys_cfg->o_input_ite6603.bDisableAutoAVMute = 0 ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
    uc &= ~B_VDO_MUTE_DISABLE ;

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;
}

void
RXINT_AVMute_Clear(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_AVMute_Clear()\n");
	BYTE uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

   p_sys_cfg->o_input_ite6603.MuteByPKG = OFF ;

    p_sys_cfg->o_input_ite6603.bDisableAutoAVMute = 0 ;

    uc =  HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
    uc &= ~B_VDO_MUTE_DISABLE ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;

    EndAutoMuteOffTimer(p_Device) ;

    if(p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
    {
        SetVideoMute(p_Device, OFF) ;

    }

    if(p_sys_cfg->o_input_ite6603.AState == ASTATE_AudioOn )
    {
        SetHWMuteClr(p_Device) ;
        ClearHWMuteClr(p_Device) ;

        SetAudioMute(p_Device, OFF) ;
    }
    SetIntMask2(p_Device, ~(B_PKT_CLR_MUTE),0) ;
}

void
RXINT_SetNewAVIInfo(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_SetNewAVIInfo()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

   p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF = TRUE ;

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
    {
        SetNewInfoVideoOutput(p_Device) ;
    }

    p_sys_cfg->o_input_ite6603.prevAVIDB1 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB1) ;
    p_sys_cfg->o_input_ite6603.prevAVIDB2 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB2) ;

}

void
RXINT_ResetAudio(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_ResetAudio()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    if(p_sys_cfg->o_input_ite6603.AState != ASTATE_AudioOff)
    {
        SetAudioMute(p_Device, ON) ;
        SwitchAudioState(p_Device, ASTATE_RequestAudio) ;
    }
}

void
RXINT_ResetHDCP(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "RXINT_ResetHDCP()\n");
	BYTE uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
    {
        ClearIntFlags(p_Device, B_CLR_ECC_INT) ;
        delay1ms(1) ;
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT3) ;

        if( uc & B_ECCERR )
        {

    		SwitchVideoState(p_Device, VSTATE_HDCP_Reset) ;

        }

    }
}

void
Timer_Handler(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "Timer_Handler() reg0x11(0x%x)\n", HDMIRX_ReadI2C_Byte( p_Device, 0x11));

	Interrupt_Handler(p_Device) ;
    VideoTimerHandler(p_Device) ;
    MuteProcessTimerHandler(p_Device) ;
    AudioTimerHandler(p_Device) ;
#ifdef SUPPORT_REPEATER
    RxHDCP_Handler(p_Device) ;
#endif
}

void
VideoTimerHandler(CDevice * p_Device)
{
	UCHAR uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	if (p_sys_cfg->o_input_ite6603.SCDTErrorCnt>= SCDT_LOST_TIMEOUT)
    {
		SWReset_HDMIRX(p_Device) ;
	}

	if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SWReset )
	{
		if(p_sys_cfg->o_input_ite6603.VideoCountingTimer==0)
		{
			Terminator_On(p_Device) ;
			SwitchVideoState(p_Device, VSTATE_PwrOff) ;
			return ;
		}
		
//		LINUXV4L2_DEBUG( KERN_INFO, "VideoTimerHandler() VSTATE_SWReset p_sys_cfg->o_input_ite6603.VideoCountingTimer(0d%d)\n", p_sys_cfg->o_input_ite6603.VideoCountingTimer);

		p_sys_cfg->o_input_ite6603.VideoCountingTimer-- ;

		return ;
	}

	if( p_sys_cfg->o_input_ite6603.VState == VSTATE_PwrOff )
	{
//		LINUXV4L2_DEBUG( KERN_INFO, "VideoTimerHandler() VSTATE_PwrOff \n");

		if(CheckPlg5VPwr(p_Device))
	    {
            SwitchVideoState(p_Device, VSTATE_SyncWait) ;
            return ;
	    }
	}

	if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SyncWait)
	{
		//LINUXV4L2_DEBUG( KERN_INFO, "VideoTimerHandler() VSTATE_SyncWait \n");

		if( p_sys_cfg->o_input_ite6603.AcceptCDRReset == TRUE )
        {
            if((HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE)&(B_VCLK_DET|B_RXCK_VALID)) == (B_VCLK_DET|B_RXCK_VALID))
            {
            	p_sys_cfg->o_input_ite6603.AcceptCDRReset = FALSE;
            	p_sys_cfg->o_input_ite6603.EnaSWCDRRest = FALSE;
            	CDR_Reset(p_Device) ;
            }
        }
	}

	if((p_sys_cfg->o_input_ite6603.VState != VSTATE_PwrOff)&&(p_sys_cfg->o_input_ite6603.VState != VSTATE_SyncWait)&&(p_sys_cfg->o_input_ite6603.VState != VSTATE_SWReset))
	{
		if(!IsSCDT(p_Device))
	    {
            SwitchVideoState(p_Device, VSTATE_SyncWait) ;
            return ;
	    }
	}
	else if ((p_sys_cfg->o_input_ite6603.VState != VSTATE_PwrOff)&&(p_sys_cfg->o_input_ite6603.VState != VSTATE_SWReset))
	{
	    if(!CheckPlg5VPwr(p_Device))
	    {

            SWReset_HDMIRX(p_Device) ;
            return ;
	    }
	}

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SyncWait || p_sys_cfg->o_input_ite6603.VState == VSTATE_SyncChecking )
    {
        p_sys_cfg->o_input_ite6603.SWResetTimeOut-- ;
        if( p_sys_cfg->o_input_ite6603.SWResetTimeOut == 0 )
        {
            SWReset_HDMIRX(p_Device) ;
            return ;
        }
    }

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SyncWait )
    {

        if( p_sys_cfg->o_input_ite6603.VideoCountingTimer == 0 )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "VsyncWaitResetTimer up, call SWReset_HDMIRX()\n",p_sys_cfg->o_input_ite6603.VideoCountingTimer);
            SWReset_HDMIRX(p_Device) ;
            return ;

        }
        else
        {

			uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE);
		    //LINUXV4L2_DEBUG( KERN_INFO, "REG_RX_SYS_STATE = %X\r",(int)uc);
			uc &= (B_RXPLL_LOCK|B_RXCK_VALID|B_SCDT|B_VCLK_DET);

			if(uc == (B_RXPLL_LOCK|B_RXCK_VALID|B_SCDT|B_VCLK_DET)	)
            {
                SwitchVideoState(p_Device, VSTATE_SyncChecking) ;
                return ;
            }

			else
			{

				uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_SYS_STATE);
				uc &= (B_RXCK_VALID|B_VCLK_DET|B_HDMIRX_MODE);

				if(uc == (B_RXCK_VALID|B_VCLK_DET))
				{
					if(p_sys_cfg->o_input_ite6603.ucDVISCDToffCNT++>100)
					{
							HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_SWRST);
							//LINUXV4L2_DEBUG( KERN_INFO, "\n\nrst_ctrl (1)= %X \n",(int)HDMIRX_ReadI2C_Byte( p_Device, REG_RX_RST_CTRL));
							delay1ms(1);
							HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0);
							//LINUXV4L2_DEBUG( KERN_INFO, "rst_ctrl (2)= %X \n\n",(int)HDMIRX_ReadI2C_Byte( p_Device, REG_RX_RST_CTRL));

							p_sys_cfg->o_input_ite6603.ucDVISCDToffCNT=0;
					}
				}else
				{
					p_sys_cfg->o_input_ite6603.ucDVISCDToffCNT=0;
				}
			}

			//LINUXV4L2_DEBUG( KERN_INFO, "VideoTimerHandler() VSTATE_SyncWait p_sys_cfg->o_input_ite6603.VideoCountingTimer(0d%d)\n", p_sys_cfg->o_input_ite6603.VideoCountingTimer);

            p_sys_cfg->o_input_ite6603.VideoCountingTimer-- ;
        }
    }

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_SyncChecking )
    {

        if( p_sys_cfg->o_input_ite6603.VideoCountingTimer == 0)
        {
            SwitchVideoState(p_Device, VSTATE_ModeDetecting) ;
            return ;
        }
        else
        {
			//LINUXV4L2_DEBUG( KERN_INFO, "VideoTimerHandler() VSTATE_SyncChecking p_sys_cfg->o_input_ite6603.VideoCountingTimer(0d%d)\n", p_sys_cfg->o_input_ite6603.VideoCountingTimer);

			p_sys_cfg->o_input_ite6603.VideoCountingTimer-- ;
        }
    }

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_HDCP_Reset )
    {

        if( --p_sys_cfg->o_input_ite6603.VideoCountingTimer == 0)
        {
        	///LINUXV4L2_DEBUG( KERN_INFO, "HDCP timer reach, reset !!\n");

            SWReset_HDMIRX(p_Device) ;
            return ;
        }
        else
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "VideoTimerHandler[VSTATE_HDCP_Reset](%d)\n",p_sys_cfg->o_input_ite6603.VideoCountingTimer);
            do {
	        	ClearIntFlags(p_Device, B_CLR_ECC_INT) ;
	        	delay1ms(1) ;
	        	uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT3) ;
	        	if(uc & B_ECCERR)
	        	{
	                break ;
	        	}
	        	delay1ms(1) ;
	        	ClearIntFlags(p_Device, B_CLR_ECC_INT) ;
	        	delay1ms(1) ;
	        	uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT3) ;
	        	if(!(uc & B_ECCERR))
	        	{
	                SwitchVideoState(p_Device, VSTATE_VideoOn) ;
	                return ;
	        	}
	        }while(0) ;
        }
    }

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
    {
		char diff ;
		unsigned short HTotal ;
		unsigned char xCnt ;
		BOOL bVidModeChange = FALSE ;
		BOOL ScanMode ;

		HTotal = (unsigned short)HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HTOTAL_L) ;
		HTotal |= (unsigned short)(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_HTOTAL_H)&M_HTOTAL_H) << 8 ;
		if(ABS((int)HTotal -(int)p_sys_cfg->o_input_ite6603.currHTotal)>4)
		{
			bVidModeChange = TRUE ;
			//LINUXV4L2_DEBUG( KERN_INFO, "HTotal changed.\n");
		}

		if(!bVidModeChange)
		{
			xCnt = (unsigned char)HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_XTALCNT_128PEL) ;

			diff = (char)p_sys_cfg->o_input_ite6603.currXcnt - (char)xCnt ;

			if( xCnt > 0x80 )
			{
				if( ABS(diff) > 6 )
				{
					//LINUXV4L2_DEBUG( KERN_INFO, "Xcnt changed. %02x -> %02x ",(int)xCnt,(int)p_sys_cfg->o_input_ite6603.currXcnt);
					//LINUXV4L2_DEBUG( KERN_INFO, "diff = %d\r\n",(int)diff);
					bVidModeChange = TRUE ;
				}
			}
			else if ( xCnt > 0x40 )
			{
				if( ABS(diff) > 4 )
				{
					//LINUXV4L2_DEBUG( KERN_INFO, "Xcnt changed. %02x -> %02x ",(int)xCnt,(int)p_sys_cfg->o_input_ite6603.currXcnt);
					//LINUXV4L2_DEBUG( KERN_INFO, "diff = %d\r\n",(int)diff);
					bVidModeChange = TRUE ;
				}
			}
			else if ( xCnt > 0x20 )
			{
				if( ABS(diff) > 2 )
				{
					//LINUXV4L2_DEBUG( KERN_INFO, "Xcnt changed. %02x -> %02x ",(int)xCnt,(int)p_sys_cfg->o_input_ite6603.currXcnt);
					//LINUXV4L2_DEBUG( KERN_INFO, "diff = %d\n\r",(int)diff);
					bVidModeChange = TRUE ;
				}
			}
			else
			{
				if( ABS(diff) > 1 )
				{
					//LINUXV4L2_DEBUG( KERN_INFO, "Xcnt changed. %02x -> %02x ",(int)xCnt,(int)p_sys_cfg->o_input_ite6603.currXcnt);
					//LINUXV4L2_DEBUG( KERN_INFO, "diff = %d\r\n",(int)diff);
					bVidModeChange = TRUE ;
				}
			}
		}

        if(p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive < 300)
        {
    		if( !bVidModeChange )
    		{
    			ScanMode = (HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_MODE)&B_INTERLACE)?INTERLACE:PROG ;
    			if( ScanMode != p_sys_cfg->o_input_ite6603.currScanMode )
    			{
    				//LINUXV4L2_DEBUG( KERN_INFO, "ScanMode change.\r\n");
    				bVidModeChange = TRUE ;
    			}
    		}
        }

		if(bVidModeChange)
		{

			SwitchVideoState(p_Device, VSTATE_SyncWait) ;
			return ;
		}
        else
        {
            unsigned char currAVI_DB1, currAVI_DB2 ;

            currAVI_DB1 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB1) ;
            currAVI_DB2 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB2) ;

            if( IsCAT6023HDMIMode(p_Device)){
                if( (currAVI_DB1 != p_sys_cfg->o_input_ite6603.prevAVIDB1)||(currAVI_DB2 != p_sys_cfg->o_input_ite6603.prevAVIDB2)){
                    RXINT_SetNewAVIInfo(p_Device) ;
                }
            }
            p_sys_cfg->o_input_ite6603.prevAVIDB1 = currAVI_DB1 ;
            p_sys_cfg->o_input_ite6603.prevAVIDB2 = currAVI_DB2 ;
        }

		if(p_sys_cfg->o_input_ite6603.VideoCountingTimer>0)
		{
            p_sys_cfg->o_input_ite6603.VideoCountingTimer-- ;
            if(p_sys_cfg->o_input_ite6603.VideoCountingTimer == 0)
            {
                p_sys_cfg->o_input_ite6603.AcceptCDRReset = TRUE ;
            }
		}

    }
}

void
SetupAudio(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "SetupAudio()\n");
	BYTE uc ;
    BYTE RxAudioCtrl ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    getCAT6023InputAudio(p_Device, &p_sys_cfg->o_input_ite6603.AudioCaps) ;

    if(p_sys_cfg->o_input_ite6603.AudioCaps.AudioFlag & B_CAP_AUDIO_ON)
    {

        uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL) & 0xF8;
        uc |=0x1;
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_MCLK_CTRL,uc);

        uc=(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS_SET)&0xCF);

        uc |= 0x70 ;

        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET,uc);

        if(p_sys_cfg->o_input_ite6603.AudioCaps.AudioFlag& B_CAP_HBR_AUDIO)
        {

            Switch_HDMIRX_Bank(p_Device, 0) ;

            #ifdef _HBR_I2S_
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_HWAMP_CTRL) ;
            uc &= ~(1<<4) ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWAMP_CTRL, uc) ;
            #else
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWAMP_CTRL,(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_HWAMP_CTRL)|0x10));
            #endif

        #if 1
            uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL) & 0xF8;
            uc |=0x0;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_MCLK_CTRL,uc);
        #endif

            SetHWMuteClrMode(p_Device);
            ResetAudio(p_Device);
        }
        else if(p_sys_cfg->o_input_ite6603.AudioCaps.AudioFlag& B_CAP_DSD_AUDIO )
        {

            SetHWMuteClrMode(p_Device);
            ResetAudio(p_Device);
        }
        else
        {
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_HWAMP_CTRL) ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWAMP_CTRL,uc &(~0x10));

            p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount++;
            RxAudioCtrl=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CTRL);

            if(p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount > 10)
            {
                p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount=0;

                if ( RxAudioCtrl & B_FORCE_FS )
                {
                    RxAudioCtrl &= ~B_FORCE_FS;
                }

                else
                {

                    switch(p_sys_cfg->o_input_ite6603.ucAudioSampleClock)
                    {
                    case AUDFS_192KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_48KHz;break ;

                    case AUDFS_48KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_44p1KHz;break ;
                    case AUDFS_44p1KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_32KHz;break ;
                    case AUDFS_32KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_96KHz;break ;

                #ifndef SUPPORT_FORCE_88p2_176p4

                    case AUDFS_96KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_192KHz;break ;

                #else

                    case AUDFS_88p2KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_176p4KHz;break ;
                    case AUDFS_96KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_88p2KHz;break ;
                    case AUDFS_176p4KHz: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_192KHz;break ;

                #endif

                    default: p_sys_cfg->o_input_ite6603.ucAudioSampleClock=AUDFS_48KHz;break;
                    }
                    //LINUXV4L2_DEBUG( KERN_INFO, "===[Audio FS Error ]===\n");
                    RxAudioCtrl |=B_FORCE_FS;
                }

            }
            RxAudioCtrl |= B_EN_I2S_NLPCM ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_AUDIO_CTRL,RxAudioCtrl);

            uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL);
            if(RxAudioCtrl & B_FORCE_FS)
            {

                uc|=B_CTSINI_EN;
            }
            else
            {
                uc &= ~B_CTSINI_EN;
            }
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_MCLK_CTRL, uc) ;

            SetHWMuteClrMode(p_Device);

            ResetAudio(p_Device);

            if( RxAudioCtrl & B_FORCE_FS)
            {

                uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL) ;
                uc &= 0xF0 ;
                uc |= p_sys_cfg->o_input_ite6603.ucAudioSampleClock & 0xF ;
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET,uc);
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET,uc);
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET,uc);
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET,uc);
            }
            SetIntMask3(p_Device, ~(B_AUTOAUDMUTE|B_AUDFIFOERR),(B_AUTOAUDMUTE|B_AUDFIFOERR)) ;
        }
        /*
        else
        {
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_HWAMP_CTRL) ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWAMP_CTRL,uc &(~0x10));
            SetHWMuteClrMode(p_Device);
            ResetAudio(p_Device);

        }
        */
        ClearIntFlags(p_Device, B_CLR_AUDIO_INT);
        SetIntMask3(p_Device, ~(B_AUTOAUDMUTE|B_AUDFIFOERR),(B_AUTOAUDMUTE|B_AUDFIFOERR));
        SwitchAudioState(p_Device, ASTATE_WaitForReady);

    }
    else
    {
        p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount = 0 ;

        p_sys_cfg->o_input_ite6603.ucAudioSampleClock=DEFAULT_START_FIXED_AUD_SAMPLEFREQ ;

        uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CTRL);
        uc &= ~B_FORCE_FS ;
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_AUDIO_CTRL, uc);
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL) ;
        uc &= ~B_CTSINI_EN;
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_MCLK_CTRL, uc) ;
//        LINUXV4L2_DEBUG( KERN_INFO, "Audio Off, clear Audio Error Count.\n");
    }
}

void
EnableAudio(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "EnableAudio()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    SetupAudio(p_Device) ;

    delay1ms(5) ;

    if(p_sys_cfg->o_input_ite6603.AudioCaps.AudioFlag & B_CAP_AUDIO_ON)
    {
        if( HDMIRX_ReadI2C_Byte( p_Device, REG_RX_INTERRUPT3) & (B_AUTOAUDMUTE|B_AUDFIFOERR) )
        {
            SwitchAudioState(p_Device, ASTATE_RequestAudio) ;
        }
        else
        {
            SwitchAudioState(p_Device, ASTATE_AudioOn) ;
        }
    }
    else
    {
        SwitchAudioState(p_Device, ASTATE_RequestAudio) ;
    }

}

void AudioTimerHandler(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "AudioTimerHandler()\n");
	BYTE uc;
    AUDIO_CAPS CurAudioCaps ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    switch(p_sys_cfg->o_input_ite6603.AState)
    {
    case ASTATE_RequestAudio:
        SetupAudio(p_Device) ;
        break;

    case ASTATE_WaitForReady:
        if(p_sys_cfg->o_input_ite6603.AudioCountingTimer==0)
        {
            SwitchAudioState(p_Device, ASTATE_AudioOn);
        }
        else
        {
            p_sys_cfg->o_input_ite6603.AudioCountingTimer --;
        }

        break;

    case ASTATE_AudioOn:
        getCAT6023InputAudio(p_Device, &CurAudioCaps) ;

        if(p_sys_cfg->o_input_ite6603.AudioCaps.AudioFlag != CurAudioCaps.AudioFlag
           /* || p_sys_cfg->o_input_ite6603.AudioCaps.AudSrcEnable != CurAudioCaps.AudSrcEnable
           || p_sys_cfg->o_input_ite6603.AudioCaps.SampleFreq != CurAudioCaps.SampleFreq */ )
        {

            p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount=0;

            p_sys_cfg->o_input_ite6603.ucAudioSampleClock = DEFAULT_START_FIXED_AUD_SAMPLEFREQ;

            uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CTRL);
            uc &= ~B_FORCE_FS ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_AUDIO_CTRL, uc);
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL) ;
            uc &= ~B_CTSINI_EN;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_MCLK_CTRL, uc) ;

            //LINUXV4L2_DEBUG( KERN_INFO, "Audio change, clear Audio Error Count.\n");

            SetAudioMute(p_Device, ON);
            SwitchAudioState(p_Device, ASTATE_RequestAudio);

        }

		if ( p_sys_cfg->o_input_ite6603.AudioCountingTimer != 0 )
        {
            p_sys_cfg->o_input_ite6603.AudioCountingTimer -- ;
            if ( p_sys_cfg->o_input_ite6603.AudioCountingTimer == 0 )
            {
                p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount=0 ;
    			//LINUXV4L2_DEBUG( KERN_INFO, "Audio On, clear Audio Error Count.\n");
            }
        }

        break;
    }
}

BYTE    CAT6023FsGet(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "CAT6023FsGet()\n");
    BYTE RxFS ;
    RxFS=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS) & 0x0F;
    return    RxFS;
}

BOOL    CAT6023HDAudioGet(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "CAT6023HDAudioGet()\n");
    BOOL bRxHBR ;
    bRxHBR=(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT)&(1<<6))?TRUE:FALSE;
    return    bRxHBR;
}

BOOL    CAT6023MultiPCM(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "CAT6023MultiPCM()\n");
    BOOL bRxMultiCh ;
    bRxMultiCh=(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT) & (1<<4))?TRUE:FALSE;
    return    bRxMultiCh;
}

BYTE    CAT6023AudioChannelNum(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "CAT6023AudioChannelNum()\n");
    BYTE RxChEn ;
    RxChEn=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT) & M_AUDIO_CH;

    return     RxChEn;
}

void    CAT6023HBRMclkSet(CDevice * p_Device, BYTE cFs)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "CAT6023HBRMclkSet()\n");
    BYTE    uc;
    uc=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_MCLK_CTRL) & 0xF8;

    switch(cFs)
    {
    case    9:
        uc |=0x0;
        break;
    default:
        uc |=0x1;
        break;

    }
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_MCLK_CTRL,uc);

}

void getCAT6023InputAudio(CDevice * p_Device, AUDIO_CAPS *pAudioCaps)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023InputAudio()\n");
    BYTE uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    if( !pAudioCaps )
    {
        return ;
    }
    Switch_HDMIRX_Bank(p_Device, 0) ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS) ;
    pAudioCaps->SampleFreq=uc&M_Fs ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT) ;
    pAudioCaps->AudioFlag = uc & 0xF0 ;
    pAudioCaps->AudSrcEnable=uc&M_AUDIO_CH ;
    delay1ms(1) ;
    pAudioCaps->AudSrcEnable|=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT)&M_AUDIO_CH ;
    delay1ms(1) ;
    pAudioCaps->AudSrcEnable|=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT)&M_AUDIO_CH ;
    delay1ms(1) ;
    pAudioCaps->AudSrcEnable|=HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT)&M_AUDIO_CH ;

    if( (uc & (B_HBRAUDIO|B_DSDAUDIO)) == 0)
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT0) ;

        if( (uc & B_AUD_NLPCM ) == 0 )
        {
            pAudioCaps->AudioFlag |= B_CAP_LPCM;
        }
    }

//add
	{	
		
		BYTE bAudioSampleFreq = 0;

		BYTE bValidCh = 0;

		BOOL ret = getCAT6023AudioInfo( p_Device, &bAudioSampleFreq, &bValidCh );

//		LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023InputAudio() bAudioSampleFreq(0d%d) bValidCh(0d%d) ret(0d%d)\n", bAudioSampleFreq, bValidCh, ret);

		if( p_sys_cfg->o_input_ite6603.previous_audio_sampling_freq != p_sys_cfg->n_input_audio_sampling_frequency ) {

			p_sys_cfg->o_input_ite6603.previous_audio_sampling_freq = p_sys_cfg->n_input_audio_sampling_frequency;

			p_sys_cfg->b_input_video_signal_changed = TRUE;

		}
	}
}

void getCAT6023InputChStat(CDevice * p_Device, AUDIO_CAPS *pAudioCaps)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023InputChStat()\n");
    BYTE uc ;

    if( !pAudioCaps )
    {
        return ;
    }

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT0) ;
    pAudioCaps->ChStat[0] = uc;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT1) ;
    pAudioCaps->ChStat[1] = uc;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT2) ;
    pAudioCaps->ChStat[2] = uc;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT3) ;

    pAudioCaps->ChStat[3] = uc & M_CLK_ACCURANCE;
    pAudioCaps->ChStat[3] <<= 4 ;
    pAudioCaps->ChStat[3] |= ((BYTE)pAudioCaps->SampleFreq)&0xF ;

    pAudioCaps->ChStat[4] = (~((BYTE)pAudioCaps->SampleFreq))&0xF ;
    pAudioCaps->ChStat[4] <<= 4 ;
    pAudioCaps->ChStat[4] |= (uc & M_SW_LEN )>>O_SW_LEN;

}

void
MuteProcessTimerHandler(CDevice * p_Device)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "MuteProcessTimerHandler()\n");
    BYTE uc ;
    BOOL TurnOffMute = FALSE ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    if( p_sys_cfg->o_input_ite6603.MuteByPKG == ON )
    {

        if( (p_sys_cfg->o_input_ite6603.MuteResumingTimer > 0)&&(p_sys_cfg->o_input_ite6603.AState == ASTATE_AudioOn))
        {
            p_sys_cfg->o_input_ite6603.MuteResumingTimer -- ;
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_INPUT_ST) ;
            //LINUXV4L2_DEBUG( KERN_INFO, "p_sys_cfg->o_input_ite6603.MuteResumingTimer = %d uc = %02X\n",p_sys_cfg->o_input_ite6603.MuteResumingTimer , (int)uc);

            if(!(uc&B_AVMUTE))
            {
                TurnOffMute = TRUE ;
                p_sys_cfg->o_input_ite6603.MuteByPKG = OFF ;
            }
            else if((p_sys_cfg->o_input_ite6603.MuteResumingTimer == 0))
            {
                p_sys_cfg->o_input_ite6603.bDisableAutoAVMute = B_VDO_MUTE_DISABLE ;

                uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
                uc |= B_VDO_MUTE_DISABLE ;
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;

                TurnOffMute = TRUE ;
                p_sys_cfg->o_input_ite6603.MuteByPKG = OFF ;
            }
        }

        if ( p_sys_cfg->o_input_ite6603.MuteAutoOff )
        {
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_INPUT_ST) ;
            if(!(uc & B_AVMUTE))
            {
                EndAutoMuteOffTimer(p_Device) ;
                TurnOffMute = TRUE ;
            }
        }
    }

    if( TurnOffMute )
    {
        if(p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
        {
            SetVideoMute(p_Device, OFF) ;
            if(p_sys_cfg->o_input_ite6603.AState == ASTATE_AudioOn )
            {
                SetAudioMute(p_Device, OFF) ;
            }
        }
    }
}

void
AssignVideoTimerTimeout(CDevice * p_Device, USHORT TimeOut)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "AssignVideoTimerTimeout() TimeOut(%d)\n", TimeOut);
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    
	p_sys_cfg->o_input_ite6603.VideoCountingTimer = TimeOut ;
}

void
AssignAudioTimerTimeout(CDevice * p_Device, USHORT TimeOut)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "AssignAudioTimerTimeout()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    p_sys_cfg->o_input_ite6603.AudioCountingTimer = TimeOut ;

}

#if 0
void
ResetVideoTimerTimeout(CDevice * p_Device)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    p_sys_cfg->o_input_ite6603.VideoCountingTimer = 0 ;
}

void
ResetAudioTimerTimeout(CDevice * p_Device)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    p_sys_cfg->o_input_ite6603.AudioCountingTimer = 0 ;
}
#endif

void
SwitchVideoState(CDevice * pDevice, Video_State_Type state)
{
//    LINUXV4L2_DEBUG( KERN_INFO, "SwitchVideoState() p_sys_cfg->o_input_ite6603.VState(%s)\n",VStateStr[state]);
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif


	if( p_sys_cfg->o_input_ite6603.VState == state )
	{
		return ;
	}

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn && state != VSTATE_VideoOn)
    {

        SwitchAudioState(pDevice, ASTATE_AudioOff) ;

	}

    p_sys_cfg->o_input_ite6603.VState = state ;

    LINUXV4L2_DEBUG( KERN_INFO, "SwitchVideoState p_sys_cfg->o_input_ite6603.VState -> %s\n",VStateStr[p_sys_cfg->o_input_ite6603.VState]);

    if( p_sys_cfg->o_input_ite6603.VState != VSTATE_SyncWait && p_sys_cfg->o_input_ite6603.VState != VSTATE_SyncChecking )
    {
        p_sys_cfg->o_input_ite6603.SWResetTimeOut = FORCE_SWRESET_TIMEOUT;

    }

    switch(p_sys_cfg->o_input_ite6603.VState)
    {
    case VSTATE_PwrOff:
        p_sys_cfg->o_input_ite6603.AcceptCDRReset = TRUE ;

		// [HUENGPEI] [2011.01.20]
		//
		{	pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

			p_sys_cfg->n_input_video_resolution_cx = 0;
			p_sys_cfg->n_input_video_resolution_cy = 0;
			p_sys_cfg->n_input_video_resolution_fps = 0;


			pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

			pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;

			p_sys_cfg->n_input_video_resolution_h_total = 0;

		}
		if( (pDevice->iManufacturer == 0x1A && 
			 
			 pDevice->m_nCustomGpioSupportProperty == 8) ) { // +SC510N4

			if( p_sys_cfg->n_input_video_resolution_cx != 0 || 
				
				p_sys_cfg->n_input_video_resolution_cy != 0 ) {

				p_sys_cfg->n_input_video_resolution_cx = 0;

				p_sys_cfg->n_input_video_resolution_cy = 0;

				p_sys_cfg->n_input_video_resolution_fps = 0;

				p_sys_cfg->n_input_video_resolution_fps_m = 0;

				p_sys_cfg->n_input_video_resolution_interleaved = 0;

				ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
				
				if( (R0000E004 & 0x00000004) == 0x00000000 ) {

					R0000E004 &= ~0x00000004; // SPLIT.MODE

					SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

					R0000E004 |=  0x00000004; // BYPASS.MODE

					SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
				}
			}
		}

		break ;
    case VSTATE_SWReset:
        HDMIRX_WriteI2C_Byte( pDevice, REG_RX_GEN_PKT_TYPE, 0x03) ;
        AssignVideoTimerTimeout(pDevice, VSTATE_SWRESET_COUNT);

		// [HUENGPEI] [2011.01.20]
		//
		{	pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

			p_sys_cfg->n_input_video_resolution_cx = 0;
			p_sys_cfg->n_input_video_resolution_cy = 0;
			p_sys_cfg->n_input_video_resolution_fps = 0;

			pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

			pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;

			p_sys_cfg->n_input_video_resolution_h_total = 0;
		}
		if( (pDevice->iManufacturer == 0x1A && 
					 
			 pDevice->m_nCustomGpioSupportProperty == 8) ) { // +SC510N4

			if( p_sys_cfg->n_input_video_resolution_cx != 0 || 
				
				p_sys_cfg->n_input_video_resolution_cy != 0 ) {

				p_sys_cfg->n_input_video_resolution_cx = 0;

				p_sys_cfg->n_input_video_resolution_cy = 0;

				p_sys_cfg->n_input_video_resolution_fps = 0;

				p_sys_cfg->n_input_video_resolution_fps_m = 0;

				p_sys_cfg->n_input_video_resolution_interleaved = 0;

				ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
				
				if( (R0000E004 & 0x00000004) == 0x00000000 ) {

					R0000E004 &= ~0x00000004; // SPLIT.MODE

					SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

					R0000E004 |=  0x00000004; // BYPASS.MODE

					SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
				}
			}
		}
    	break ;

    case VSTATE_SyncWait:

        SetIntMask1(pDevice, ~(B_SCDTOFF|B_VIDMODE_CHG),0) ;
        HDMIRX_WriteI2C_Byte( pDevice, REG_RX_GEN_PKT_TYPE, 0x03) ;
        SetVideoMute(pDevice, ON) ;
        AssignVideoTimerTimeout(pDevice, VSTATE_MISS_SYNC_COUNT);

		// [HUENGPEI] [2011.01.20]
		//
		{	pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

			p_sys_cfg->n_input_video_resolution_cx = 0;
			p_sys_cfg->n_input_video_resolution_cy = 0;
			p_sys_cfg->n_input_video_resolution_fps = 0;

			pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

			pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;

			p_sys_cfg->n_input_video_resolution_h_total = 0;
		}
		if( (pDevice->iManufacturer == 0x1A && 
					 
			 pDevice->m_nCustomGpioSupportProperty == 8) ) { // +SC510N4

			if( p_sys_cfg->n_input_video_resolution_cx != 0 || 
				
				p_sys_cfg->n_input_video_resolution_cy != 0 ) {

				p_sys_cfg->n_input_video_resolution_cx = 0;

				p_sys_cfg->n_input_video_resolution_cy = 0;

				p_sys_cfg->n_input_video_resolution_fps = 0;

				p_sys_cfg->n_input_video_resolution_fps_m = 0;

				p_sys_cfg->n_input_video_resolution_interleaved = 0;

				ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
				
				if( (R0000E004 & 0x00000004) == 0x00000000 ) {

					R0000E004 &= ~0x00000004; // SPLIT.MODE

					SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

					R0000E004 |=  0x00000004; // BYPASS.MODE

					SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
				}
			}
		}
		break ;
    case VSTATE_SyncChecking:
        HDMIRX_WriteI2C_Byte( pDevice, REG_RX_GEN_PKT_TYPE, 0x03) ;
        SetIntMask1(pDevice, ~(B_SCDTOFF|B_VIDMODE_CHG),(B_SCDTOFF|B_VIDMODE_CHG)) ;
        AssignVideoTimerTimeout(pDevice, VSATE_CONFIRM_SCDT_COUNT);
        break ;
	case VSTATE_HDCP_Reset:
        SetVideoMute(pDevice, ON) ;
		AssignVideoTimerTimeout(pDevice, HDCP_WAITING_TIMEOUT);
		break ;
    case VSTATE_VideoOn:
        HDMIRX_WriteI2C_Byte( pDevice, REG_RX_GEN_PKT_TYPE, 0x81) ;

        AssignVideoTimerTimeout(pDevice, CDRRESET_TIMEOUT);
        if(!p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF)
        {
            SetVideoInputFormatWithoutInfoFrame(pDevice, F_MODE_RGB24) ;
            SetColorimetryByMode(pDevice/*&SyncInfo*/) ;
            SetColorSpaceConvert(pDevice) ;
        }

        if( !IsCAT6023HDMIMode(pDevice))
        {
            SetIntMask1(pDevice, ~(B_SCDTOFF|B_PWR5VOFF),(B_SCDTOFF|B_PWR5VOFF)) ;
            SetVideoMute(pDevice, OFF) ;
            SwitchAudioState(pDevice, ASTATE_AudioOff) ;
            p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF = FALSE ;
        }
        else
        {
        	BYTE uc ;

            if( p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF )
            {
                SetNewInfoVideoOutput(pDevice) ;
            }

        #ifdef SUPPORT_REPEATER
            if( p_sys_cfg->o_input_ite6603.bHDCPMode & HDCP_REPEATER )
            {
                SetIntMask3(pDevice, 0,B_ECCERR|B_R_AUTH_DONE|B_R_AUTH_START) ;
        	}
        	else
        #endif
        	{
                SetIntMask3(pDevice, ~(B_R_AUTH_DONE|B_R_AUTH_START),B_ECCERR) ;
        	}
            SetIntMask2(pDevice, ~(B_NEW_AVI_PKG|B_PKT_SET_MUTE|B_PKT_CLR_MUTE),(B_NEW_AVI_PKG|B_PKT_SET_MUTE|B_PKT_CLR_MUTE)) ;
            SetIntMask1(pDevice, ~(B_SCDTOFF|B_PWR5VOFF),(B_SCDTOFF|B_PWR5VOFF)) ;
            SetIntMask4(pDevice, 0,B_M_RXCKON_DET) ;

            p_sys_cfg->o_input_ite6603.MuteByPKG =  (HDMIRX_ReadI2C_Byte( pDevice, REG_RX_VID_INPUT_ST) & B_AVMUTE)?TRUE:FALSE ;

            SetVideoMute(pDevice, p_sys_cfg->o_input_ite6603.MuteByPKG) ;

            p_sys_cfg->o_input_ite6603.ucHDMIAudioErrorCount = 0 ;

            p_sys_cfg->o_input_ite6603.ucAudioSampleClock=DEFAULT_START_FIXED_AUD_SAMPLEFREQ ;

            uc = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_AUDIO_CTRL) ;
            uc &= ~B_FORCE_FS ;
            HDMIRX_WriteI2C_Byte( pDevice, REG_RX_AUDIO_CTRL, uc) ;

            uc = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_MCLK_CTRL) & (~B_CTSINI_EN);
            HDMIRX_WriteI2C_Byte( pDevice, REG_RX_MCLK_CTRL, uc) ;

            //LINUXV4L2_DEBUG( KERN_INFO, "[%s:%d] reg%02X = %02X\n",__FILE__,__LINE__,(int)REG_RX_AUDIO_CTRL, (int)uc);

			EnableAudio(pDevice) ;
        }

		p_sys_cfg->o_input_ite6603.currHTotal = p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal ;
		p_sys_cfg->o_input_ite6603.currXcnt = p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt ;
		p_sys_cfg->o_input_ite6603.currScanMode = p_sys_cfg->o_input_ite6603.s_CurrentVM.ScanMode ;

        break ;
    }
}

void
SwitchAudioState(CDevice * p_Device, Audio_State_Type state)
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    p_sys_cfg->o_input_ite6603.AState = state ;
    //LINUXV4L2_DEBUG( KERN_INFO, "p_sys_cfg->o_input_ite6603.AState -> %s\n",AStateStr[p_sys_cfg->o_input_ite6603.AState]);

    switch(p_sys_cfg->o_input_ite6603.AState)
    {
    case ASTATE_AudioOff:
        SetAudioMute(p_Device, TRUE) ;
        break ;

    case ASTATE_WaitForReady:
        AssignAudioTimerTimeout(p_Device, AUDIO_READY_TIMEOUT) ;
        break ;
    case ASTATE_AudioOn:
        SetAudioMute(p_Device, p_sys_cfg->o_input_ite6603.MuteByPKG) ;
        AssignAudioTimerTimeout(p_Device, AUDIO_CLEARERROR_TIMEOUT) ;
        if( p_sys_cfg->o_input_ite6603.MuteByPKG )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "AudioOn, but still in mute.\n");
            EnableMuteProcessTimer(p_Device) ;
        }
        break ;
    }
}

void
DumpSyncInfo(CDevice * pDevice, VTiming *pVTiming)
{
	LONG VFreq ;

	WORD _vfreq ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	p_sys_cfg->b_input_video_signal_changed = FALSE;

//peter [2011.03.14] enbale debug message
	LINUXV4L2_DEBUG( KERN_INFO, "HActive %4d,",pVTiming->HActive);
    LINUXV4L2_DEBUG( KERN_INFO, "VActive %4d,",pVTiming->VActive);
    LINUXV4L2_DEBUG( KERN_INFO, "HTotal %4d,",pVTiming->HTotal);
    LINUXV4L2_DEBUG( KERN_INFO, "VTotal %4d,",pVTiming->VTotal);
    LINUXV4L2_DEBUG( KERN_INFO, "PCLK %8ld,",pVTiming->PCLK);
    LINUXV4L2_DEBUG( KERN_INFO, "xCnt 0x%02x,",pVTiming->xCnt);
    LINUXV4L2_DEBUG( KERN_INFO, "HFrontPorch %3d,",pVTiming->HFrontPorch);
    LINUXV4L2_DEBUG( KERN_INFO, "HSyncWidth %3d,",pVTiming->HSyncWidth);
    LINUXV4L2_DEBUG( KERN_INFO, "HBackPorch %3d,",pVTiming->HBackPorch);
    LINUXV4L2_DEBUG( KERN_INFO, "VFrontPorch %2d,",pVTiming->VFrontPorch);
    LINUXV4L2_DEBUG( KERN_INFO, "VSyncWidth %2d,",pVTiming->VSyncWidth);
    LINUXV4L2_DEBUG( KERN_INFO, "VBackPorch %2d,",pVTiming->VBackPorch);
    LINUXV4L2_DEBUG( KERN_INFO, "%s,",pVTiming->ScanMode?"PROG":"INTERLACE");
    LINUXV4L2_DEBUG( KERN_INFO, "%s,",pVTiming->VPolarity?"Vpos":"Vneg");
    LINUXV4L2_DEBUG( KERN_INFO, "%s,",pVTiming->HPolarity?"Hpos":"Hneg");

    VFreq = (ULONGLONG)pVTiming->PCLK ;
   
	VFreq *= 1000 ;

	VFreq /= pVTiming->HTotal ;
    
    VFreq /= pVTiming->VTotal ;

	LINUXV4L2_DEBUG( KERN_INFO,"VFreq %d\n",VFreq );


	if( p_sys_cfg->n_input_video_resolution_cx != pVTiming->HActive ) {

		p_sys_cfg->n_input_video_resolution_cx = pVTiming->HActive;

		p_sys_cfg->b_input_video_signal_changed = TRUE;
	}
	if( p_sys_cfg->n_input_video_resolution_cy != pVTiming->VActive ) {

		p_sys_cfg->n_input_video_resolution_cy = pVTiming->VActive;

		p_sys_cfg->b_input_video_signal_changed = TRUE;
	}
	p_sys_cfg->n_input_video_resolution_fps = (LONG)(VFreq);

	p_sys_cfg->n_input_video_resolution_fps_m = (GetVerticalFreq( pDevice, &_vfreq ) == 101) ? 1 : 0;

	p_sys_cfg->n_input_video_resolution_interleaved = pVTiming->ScanMode ? 0 : 1;
	
	p_sys_cfg->n_input_video_resolution_h_total = pVTiming->HTotal;

//decide fps according to resolution
	if( p_sys_cfg->n_input_video_resolution_cx * 
		
		p_sys_cfg->n_input_video_resolution_cy * 
		
		p_sys_cfg->n_input_video_resolution_fps > 80000000 ) {

		p_sys_cfg->b_input_video_resolution_spliter_mode = TRUE;
	}
	else {

		p_sys_cfg->b_input_video_resolution_spliter_mode = FALSE;
	}
	if( p_sys_cfg->n_input_video_resolution_cy == 1200 ) {

		p_sys_cfg->b_input_video_resolution_spliter_mode = TRUE;
	}
	if( ABS(pVTiming->HTotal - 1650) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy == 720)                                          { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( ABS(pVTiming->HTotal - 1980) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy == 720)                                          { p_sys_cfg->n_input_video_resolution_fps = 50; }

	if( ABS(pVTiming->HTotal - 2276) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy == 720 && ABS(pVTiming->PCLK - 59586) < 3000 )   { p_sys_cfg->n_input_video_resolution_fps = 24; }

	if( ABS(pVTiming->HTotal - 2276) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy == 720 && ABS(pVTiming->PCLK - 73531) < 3000 )   { p_sys_cfg->n_input_video_resolution_fps = 30; }

	if( ABS(pVTiming->HTotal - 1912) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy == 720                                       )   { p_sys_cfg->n_input_video_resolution_fps = 25; }

	if( ABS(pVTiming->HTotal - 2200) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 540)                                          { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( ABS(pVTiming->HTotal - 2640) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 540)                                          { p_sys_cfg->n_input_video_resolution_fps = 50; }

	if( ABS(pVTiming->PCLK -  74000) <  4440 && ABS(pVTiming->HTotal - 2750) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080) { p_sys_cfg->n_input_video_resolution_fps = 24; }

	if( ABS(pVTiming->PCLK -  74000) <  4440 && ABS(pVTiming->HTotal - 2640) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080) { p_sys_cfg->n_input_video_resolution_fps = 25; }

	if( ABS(pVTiming->PCLK -  74000) <  4440 && ABS(pVTiming->HTotal - 2200) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080) { p_sys_cfg->n_input_video_resolution_fps = 30; }

	if( ABS(pVTiming->PCLK - 148000) < 14440 && ABS(pVTiming->HTotal - 2640) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080) { p_sys_cfg->n_input_video_resolution_fps = 50; }

	if( ABS(pVTiming->PCLK - 148352) < 14440 && ABS(pVTiming->HTotal - 2200) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( ABS(pVTiming->PCLK - 138240) < 14440 && ABS(pVTiming->HTotal - 2080) < 5 && p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx ==  720 && p_sys_cfg->n_input_video_resolution_cy ==  240 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx ==  720 && p_sys_cfg->n_input_video_resolution_cy ==  288 ) { p_sys_cfg->n_input_video_resolution_fps = 50; }

	if( p_sys_cfg->n_input_video_resolution_cx ==  720 && p_sys_cfg->n_input_video_resolution_cy ==  480 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx ==  720 && p_sys_cfg->n_input_video_resolution_cy ==  576 ) { p_sys_cfg->n_input_video_resolution_fps = 50; }

//dempa
	if( p_sys_cfg->n_input_video_resolution_cx ==  640 && p_sys_cfg->n_input_video_resolution_cy ==  480 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx ==  800 && p_sys_cfg->n_input_video_resolution_cy ==  600 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx == 1024 && p_sys_cfg->n_input_video_resolution_cy ==  768 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy ==  960 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx == 1280 && p_sys_cfg->n_input_video_resolution_cy == 1024 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( p_sys_cfg->n_input_video_resolution_cx == 1440 && p_sys_cfg->n_input_video_resolution_cy ==  900 ) { p_sys_cfg->n_input_video_resolution_fps = 60; }

	if( (p_sys_cfg->n_input_video_resolution_cx ==  720 && p_sys_cfg->n_input_video_resolution_cy == 240) ||

		(p_sys_cfg->n_input_video_resolution_cx ==  720 && p_sys_cfg->n_input_video_resolution_cy == 288) ||

		(p_sys_cfg->n_input_video_resolution_cx ==  768 && p_sys_cfg->n_input_video_resolution_cy == 288) ||

		(p_sys_cfg->n_input_video_resolution_cx == 1440 && p_sys_cfg->n_input_video_resolution_cy == 540) ||

		(p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 540) ) {

		p_sys_cfg->n_input_video_resolution_interleaved = 1;
	}
	else {

		p_sys_cfg->n_input_video_resolution_interleaved = 0;
	}
	if( p_sys_cfg->n_input_video_resolution_fps != p_sys_cfg->o_input_ite6603.previous_fps ) {

		p_sys_cfg->o_input_ite6603.previous_fps = p_sys_cfg->n_input_video_resolution_fps;

		p_sys_cfg->b_input_video_signal_changed = TRUE;
	}
	LINUXV4L2_DEBUG( KERN_INFO, "n_input_video_resolution_cx(%d) n_input_video_resolution_cy(%d) n_input_video_resolution_fps(%d)\n", p_sys_cfg->n_input_video_resolution_cx, p_sys_cfg->n_input_video_resolution_cy,p_sys_cfg->n_input_video_resolution_fps );

	LINUXV4L2_DEBUG( KERN_INFO, "b_input_video_resolution_spliter_mode(%d) n_input_video_resolution_h_total (%d)\n", p_sys_cfg->b_input_video_resolution_spliter_mode, p_sys_cfg->n_input_video_resolution_h_total );

	if( p_sys_cfg->b_input_video_signal_changed )
	{
		if( pDevice->iManufacturer == 0x18 ||

		   (pDevice->iManufacturer == 0x1A &&

			pDevice->m_nCustomGpioSupportProperty == 8) ) {

			if( ((p_sys_cfg->n_input_video_resolution_cx == 1280) && (p_sys_cfg->n_input_video_resolution_cy == 720) && ( p_sys_cfg->n_input_video_resolution_fps == 24)) ||
				
				((p_sys_cfg->n_input_video_resolution_cx == 1280) && (p_sys_cfg->n_input_video_resolution_cy == 720) && ( p_sys_cfg->n_input_video_resolution_fps == 25)) ||

				((p_sys_cfg->n_input_video_resolution_cx == 1280) && (p_sys_cfg->n_input_video_resolution_cy == 720) && ( p_sys_cfg->n_input_video_resolution_fps == 30)) 	)
			{
				p_sys_cfg->b_input_video_resolution_spliter_mode = TRUE;

				BYTE uc = 0;

				uc = HDMIRX_ReadI2C_Byte( pDevice, REG_RX_VIDEO_CTRL1) ;

				uc &= 0xBF;//external sync
				
				HDMIRX_WriteI2C_Byte( pDevice, REG_RX_VIDEO_CTRL1, uc) ;

				uc = HDMIRX_ReadI2C_Byte( pDevice, REG_RX_PG_CTRL1) ;

				uc |= 0x04;//by pass
				
				HDMIRX_WriteI2C_Byte( pDevice, REG_RX_PG_CTRL1, uc) ;

				SA7160_SetFpgaRegister( pDevice, 0x25, 0x01 );
			}
			else
			{
				BYTE uc = 0;

				uc = HDMIRX_ReadI2C_Byte( pDevice, REG_RX_VIDEO_CTRL1) ;

				uc |= 0x40;//embedded sync
				
				HDMIRX_WriteI2C_Byte( pDevice, REG_RX_VIDEO_CTRL1, uc) ;

				uc = HDMIRX_ReadI2C_Byte( pDevice, REG_RX_PG_CTRL1) ;

				uc &= 0xFB;//
				
				HDMIRX_WriteI2C_Byte( pDevice, REG_RX_PG_CTRL1, uc) ;

				SA7160_SetFpgaRegister( pDevice, 0x25, 0x00 );
			}
		}

		#ifdef ENABLE_1920X1080PX60FPS

		// FPGA (SPLITER.MODE)
		// 
		if( pDevice->iManufacturer == 0x18 ||
			
		   (pDevice->iManufacturer == 0x1A &&
			
			pDevice->m_nCustomGpioSupportProperty == 8) ) {

			if( (p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1080 && p_sys_cfg->n_input_video_resolution_fps >= 50) ||

				(p_sys_cfg->n_input_video_resolution_cx == 1920 && p_sys_cfg->n_input_video_resolution_cy == 1200 && p_sys_cfg->n_input_video_resolution_fps >   0) ||
				
				 p_sys_cfg->b_input_video_resolution_spliter_mode == TRUE ) {

				if( p_sys_cfg->n_input_video_resolution_cy == 1200 ) {

					SA7160_SetFpgaRegister( pDevice, 0x24, 0x01 );
				}
				else {

					SA7160_SetFpgaRegister( pDevice, 0x24, 0x00 );
				}
				ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
									
				R0000E004 |=  0x00000004;

				SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

				R0000E004 &= ~0x00000004;

				SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
			}
			else {

				ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
				
				R0000E004 &= ~0x00000004;

				SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

				R0000E004 |=  0x00000004;

				SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
			}
		}
		#endif
	}

	// [HUENGPEI] [2011.01.20]
	// 
	{	
		pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) | 

														  (p_sys_cfg->n_input_video_resolution_cy <<  0);

		pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);
		
		pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);

		pDevice->m_nAnalogVideoDecoderStatusProperty = 1;

		{	BYTE bAudioSampleFreq = 0;

			BYTE bValidCh = 0;
	
			BOOL ret = getCAT6023AudioInfo( pDevice, &bAudioSampleFreq, &bValidCh );
	
			LINUXV4L2_DEBUG( KERN_INFO, "DumpSyncInfo() bAudioSampleFreq(0d%d) bValidCh(0d%d) ret(0d%d)\n", bAudioSampleFreq, bValidCh, ret);
	
			p_sys_cfg->n_input_audio_sampling_frequency = 48000;

			if( ret ) {

				switch( bAudioSampleFreq ) {

				case B_Fs_44p1KHz:  p_sys_cfg->n_input_audio_sampling_frequency =  44100; break;

				case B_Fs_48KHz:    p_sys_cfg->n_input_audio_sampling_frequency =  48000; break;

				case B_Fs_32KHz:    p_sys_cfg->n_input_audio_sampling_frequency =  32000; break;

//				case B_Fs_88p2KHz:  p_sys_cfg->n_input_audio_sampling_frequency =  88000; break;

//				case B_Fs_96KHz:    p_sys_cfg->n_input_audio_sampling_frequency =  96000; break;

//				case B_Fs_176p4KHz: p_sys_cfg->n_input_audio_sampling_frequency = 176000; break;

//				case B_Fs_192KHz:   p_sys_cfg->n_input_audio_sampling_frequency = 192000; break;

//				case B_Fs_768KHz:   p_sys_cfg->n_input_audio_sampling_frequency = 768000; break;
				};
			}
			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = p_sys_cfg->n_input_audio_sampling_frequency;
//add
			if( p_sys_cfg->o_input_ite6603.previous_audio_sampling_freq != p_sys_cfg->n_input_audio_sampling_frequency ) {

				p_sys_cfg->o_input_ite6603.previous_audio_sampling_freq = p_sys_cfg->n_input_audio_sampling_frequency;

				p_sys_cfg->b_input_video_signal_changed = TRUE;

			}
		}
	}
}


BOOL
CheckSignalStable( CDevice * pDevice)
{
    BYTE uc1, uc2, uc3 ;
	VTiming					s_TempVM;  
	long diff ;


	LINUXV4L2_DEBUG( KERN_INFO, "CheckSignalStable() \n" );

	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	int index = 0;
	for( index = 0; index < 20; index++ )
	{
		uc1 = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_VID_HTOTAL_L) ;
		uc2 = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_VID_HTOTAL_H) ;
		uc3 = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_VID_HACT_L) ;

		s_TempVM.HTotal = ((WORD)(uc2&0xF)<<8) | (WORD)uc1;
		s_TempVM.HActive = ((WORD)(uc2 & 0x70)<<4) | (WORD)uc3 ;
		if( (s_TempVM.HActive | (1<<11)) <s_TempVM.HTotal )
		{
			s_TempVM.HActive |= (1<<11) ;
		}

		if( s_TempVM.HActive != p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive )
		{
			LINUXV4L2_DEBUG( KERN_INFO, "CheckSignalStable() s_TempVM.HActive(%d) s_CurrentVM.HActive(%d)\n", s_TempVM.HActive, p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive  );

			return FALSE;
		}

		diff = s_TempVM.HTotal - p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal;

		if( ABS(diff) > 4 )
		{
			LINUXV4L2_DEBUG( KERN_INFO, "CheckSignalStable() s_TempVM.HTotal(%d) s_CurrentVM.HTotal(%d)\n", s_TempVM.HTotal, p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal  );

			return FALSE;
		}

		uc1 = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_VID_VTOTAL_L) ;
		uc2 = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_VID_VTOTAL_H) ;
		uc3 = HDMIRX_ReadI2C_Byte(pDevice, REG_RX_VID_VACT_L) ;

		s_TempVM.VTotal = ((WORD)(uc2&0x7)<<8) | (WORD)uc1;
		s_TempVM.VActive = ((WORD)(uc2 & 0x30)<<4) | (WORD)uc3 ;
		if( (s_TempVM.VActive | (1<<10)) <s_TempVM.VTotal )
		{
			s_TempVM.VActive |= (1<<10) ;
		}

		diff = s_TempVM.VActive - p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive;

		if( ABS(diff) > 10 )
		{
			LINUXV4L2_DEBUG( KERN_INFO, "CheckSignalStable() s_TempVM.VActive(%d) s_CurrentVM.VActive(%d)\n", s_TempVM.VActive, p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive  );
			return FALSE;
		}

		if( s_TempVM.HActive * s_TempVM.VActive > 1920 * 1200 )
		{
			return FALSE;
		}

/*
		s_TempVM.xCnt = HDMIRX_ReadI2C_Byte(REG_RX_VID_XTALCNT_128PEL) ;

		if(  s_TempVM.xCnt )
		{
			s_TempVM.PCLK = 128L * 27000L / s_TempVM.xCnt ;
		}
		else
		{
			s_TempVM.PCLK = 1234 ;

			return FALSE ;
		}

        diff = ABS(s_TempVM.PCLK - s_CurrentVM.PCLK) ;
        diff *= 100 ;
        diff /= s_CurrentVM.PCLK ;

        if( diff > 3 )
		{
			HDMIRX_PRINTF(( "CheckSignalStable() s_TempVM.PCLK(%d) s_CurrentVM.PCLK(%d)\n", s_TempVM.PCLK, s_CurrentVM.PCLK  ));
			return FALSE;
		}
*/
	    LINUXV4L2_DEBUG( KERN_INFO, "CheckSignalStable() success(%d)\n", index );
		//fix replug
		if(index >= 9)
		{
			if((s_TempVM.HActive <= 1920) && (s_TempVM.VActive <= 1080))
			{
				return TRUE;
			}
		}


		delay1ms(100);
	}
	return FALSE;
}

BOOL
bGetSyncInfo(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "bGetSyncInfo()\n");
    long diff ;

    BYTE uc1, uc2, uc3 ;
    int i ;

	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    uc1 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_HTOTAL_L) ;
    uc2 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_HTOTAL_H) ;
    uc3 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_HACT_L) ;

    p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal = ((WORD)(uc2&0xF)<<8) | (WORD)uc1;
    p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive = ((WORD)(uc2 & 0x70)<<4) | (WORD)uc3 ;
    if( (p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive | (1<<11)) <p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal )
    {
        p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive |= (1<<11) ;
    }
    uc1 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_HSYNC_WID_L) ;
    uc2 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_HSYNC_WID_H) ;
    uc3 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_H_FT_PORCH_L) ;

    p_sys_cfg->o_input_ite6603.s_CurrentVM.HSyncWidth = ((WORD)(uc2&0x1)<<8) | (WORD)uc1;
    p_sys_cfg->o_input_ite6603.s_CurrentVM.HFrontPorch = ((WORD)(uc2 & 0xf0)<<4) | (WORD)uc3 ;
    p_sys_cfg->o_input_ite6603.s_CurrentVM.HBackPorch = p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal - p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive - p_sys_cfg->o_input_ite6603.s_CurrentVM.HSyncWidth - p_sys_cfg->o_input_ite6603.s_CurrentVM.HFrontPorch ;

    uc1 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_VTOTAL_L) ;
    uc2 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_VTOTAL_H) ;
    uc3 = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_VACT_L) ;

    p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal = ((WORD)(uc2&0x7)<<8) | (WORD)uc1;
    p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive = ((WORD)(uc2 & 0x30)<<4) | (WORD)uc3 ;
    if( (p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive | (1<<10)) <p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal )
    {
        p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive |= (1<<10) ;
    }

    p_sys_cfg->o_input_ite6603.s_CurrentVM.VBackPorch = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_VSYNC2DE) ;
    p_sys_cfg->o_input_ite6603.s_CurrentVM.VFrontPorch = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_V_FT_PORCH) ;
    p_sys_cfg->o_input_ite6603.s_CurrentVM.VSyncWidth = 0 ;

    p_sys_cfg->o_input_ite6603.s_CurrentVM.ScanMode = (HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_MODE)&B_INTERLACE)?INTERLACE:PROG ;

    p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_VID_XTALCNT_128PEL) ;

    if(  p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt )
    {
        p_sys_cfg->o_input_ite6603.s_CurrentVM.PCLK = 128L * 27000L / p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt ;
    }
    else
    {
        //LINUXV4L2_DEBUG( KERN_INFO, "p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt == %02x\n",p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt);
        p_sys_cfg->o_input_ite6603.s_CurrentVM.PCLK = 1234 ;
        /*
        for( i = 0x58 ; i < 0x66 ; i++ )
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "HDMIRX_ReadI2C_Byte(%02x) = %02X\n",i,(int)HDMIRX_ReadI2C_Byte(p_Device, i)));
        }
        */
        return FALSE ;
    }

#ifndef USE_MODE_TABLE
	if( (p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive > 200)&&(p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal>p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive )&&
		(p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive > 300)&&(p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal>p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive ))
	{
		return CheckSignalStable( p_Device );

	}
#else

    for( i = 0 ; i < SizeofVMTable ; i++ )
    {

        diff = ABS(s_VMTable[i].PCLK - p_sys_cfg->o_input_ite6603.s_CurrentVM.PCLK) ;
        diff *= 100 ;
        diff /= s_VMTable[i].PCLK ;

        if( diff > 3 )
        {

            continue ;
        }

        if( s_VMTable[i].HActive != p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive )
        {
            continue ;
        }

        diff = (long)s_VMTable[i].HTotal - (long)p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal ;
        if( ABS(diff)>4)
        {
            continue ;
        }

        diff = (long)s_VMTable[i].VActive - (long)p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive ;
        if( ABS(diff)>10)
        {
            continue ;
        }

        diff = (long)s_VMTable[i].VTotal - (long)p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal ;
        if( ABS(diff)>40)
        {
            continue ;
        }

        if( s_VMTable[i].ScanMode != p_sys_cfg->o_input_ite6603.s_CurrentVM.ScanMode )
        {
            continue ;
        }

        p_sys_cfg->o_input_ite6603.s_CurrentVM = s_VMTable[i] ;

        return TRUE ;
    }

    for( i = 0 ; i < SizeofVMTable ; i++ )
    {

        diff = ABS(s_VMTable[i].PCLK - p_sys_cfg->o_input_ite6603.s_CurrentVM.PCLK) ;
        diff *= 100 ;
        diff /= s_VMTable[i].PCLK ;

        if( diff > 3 )
        {

            continue ;
        }

        if( s_VMTable[i].HActive != p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive )
        {
            continue ;
        }

        diff = (long)s_VMTable[i].HTotal - (long)p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal ;
        if( ABS(diff)>4)
        {
            continue ;
        }

        diff = (long)s_VMTable[i].VActive - (long)p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive ;
        if( ABS(diff)>10)
        {
            continue ;
        }

        diff = (long)s_VMTable[i].VTotal - (long)p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal ;
        if( ABS(diff)>40)
        {
            continue ;
        }
        p_sys_cfg->o_input_ite6603.s_CurrentVM = s_VMTable[i] ;

        return TRUE ;
    }
#endif
    return FALSE ;
}

#define SIZE_OF_CSCOFFSET (REG_RX_CSC_RGBOFF - REG_RX_CSC_YOFF + 1)
#define SIZE_OF_CSCMTX  (REG_RX_CSC_MTX33_H - REG_RX_CSC_MTX11_L + 1)
#define SIZE_OF_CSCGAIN (REG_RX_CSC_GAIN3V_H - REG_RX_CSC_GAIN1V_L + 1)

void
Video_Handler(CDevice * pDevice)
{

    BOOL bHDMIMode;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif


    if(p_sys_cfg->o_input_ite6603.VState == VSTATE_ModeDetecting)
    {
        //LINUXV4L2_DEBUG( KERN_INFO, "Video_Handler, p_sys_cfg->o_input_ite6603.VState = VSTATE_ModeDetecting.\n");

        ClearIntFlags(pDevice, B_CLR_MODE_INT) ;

        if(!bGetSyncInfo(pDevice))
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "Current Get: "); //DumpSyncInfo(pDevice, &p_sys_cfg->o_input_ite6603.s_CurrentVM) ;

            SwitchVideoState(pDevice, VSTATE_SyncWait) ;
            p_sys_cfg->o_input_ite6603.bGetSyncFailCount ++ ;
            //LINUXV4L2_DEBUG( KERN_INFO, "bGetSyncInfo() fail, p_sys_cfg->o_input_ite6603.bGetSyncFailCount = %d ", p_sys_cfg->o_input_ite6603.bGetSyncFailCount);
            if( p_sys_cfg->o_input_ite6603.bGetSyncFailCount % 32 == 31 )
            {
                //LINUXV4L2_DEBUG( KERN_INFO, " called SWReset\n");
                SWReset_HDMIRX(pDevice) ;
            }
            else if( p_sys_cfg->o_input_ite6603.bGetSyncFailCount % 8 == 7)
            {
                //LINUXV4L2_DEBUG( KERN_INFO, " reset video.\n");

                HDMIRX_WriteI2C_Byte( pDevice, REG_RX_RST_CTRL, B_VDORST ) ;
                delay1ms(1) ;
                HDMIRX_WriteI2C_Byte( pDevice, REG_RX_RST_CTRL, 0) ;

            }
            else
            {
                LINUXV4L2_DEBUG( KERN_INFO, "\n");
            }

			// [HUENGPEI] [2011.01.20]
			//
			{	pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

				p_sys_cfg->n_input_video_resolution_cx = 0;
				p_sys_cfg->n_input_video_resolution_cy = 0;
				p_sys_cfg->n_input_video_resolution_fps = 0;

				pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

				pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

				pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

				pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

				pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;

				p_sys_cfg->n_input_video_resolution_h_total = 0;

			}
			if( (pDevice->iManufacturer == 0x1A && 
					 
				 pDevice->m_nCustomGpioSupportProperty == 8) ) { // +SC510N4

					if( p_sys_cfg->n_input_video_resolution_cx != 0 || 
						
					p_sys_cfg->n_input_video_resolution_cy != 0 ) {

					p_sys_cfg->n_input_video_resolution_cx = 0;

					p_sys_cfg->n_input_video_resolution_cy = 0;

					p_sys_cfg->n_input_video_resolution_fps = 0;

					p_sys_cfg->n_input_video_resolution_fps_m = 0;

					p_sys_cfg->n_input_video_resolution_interleaved = 0;

					ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
						
					if( (R0000E004 & 0x00000004) == 0x00000000 ) {

						R0000E004 &= ~0x00000004; // SPLIT.MODE

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

						R0000E004 |=  0x00000004; // BYPASS.MODE

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
					}
				}
			}

            return ;
        }
        else
        {

            LINUXV4L2_DEBUG( KERN_INFO, "Matched Result: "); DumpSyncInfo(pDevice, &p_sys_cfg->o_input_ite6603.s_CurrentVM) ;
            p_sys_cfg->o_input_ite6603.bGetSyncFailCount = 0 ;
        }

        SetDefaultRegisterValue(pDevice) ;

        bHDMIMode = IsCAT6023HDMIMode(pDevice) ;

        if(!bHDMIMode)
        {

            LINUXV4L2_DEBUG( KERN_INFO, "This is DVI Mode.\n");
            p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF = FALSE ;
        }

        if( HDMIRX_ReadI2C_Byte(pDevice, REG_RX_INTERRUPT1) & (B_VIDMODE_CHG|B_SCDTOFF|B_PWR5VOFF))
        {
            SwitchVideoState(pDevice, VSTATE_SyncWait) ;

        }
        else
        {

            SwitchVideoState(pDevice, VSTATE_VideoOn) ;
        }

        return ;
    }
}

void
SetVideoInputFormatWithoutInfoFrame(CDevice * p_Device, BYTE bInMode)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetVideoInputFormatWithoutInfoFrame()\n");
    BYTE uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


    uc = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_CSC_CTRL) ;
    uc |= B_FORCE_COLOR_MODE ;
    p_sys_cfg->o_input_ite6603.bInputVideoMode &= ~F_MODE_CLRMOD_MASK ;

    switch(bInMode)
    {
    case F_MODE_YUV444:
        uc &= ~(M_INPUT_COLOR_MASK<<O_INPUT_COLOR_MODE) ;
        uc |= B_INPUT_YUV444 << O_INPUT_COLOR_MODE ;
        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_YUV444 ;
        break ;
    case F_MODE_YUV422:
        uc &= ~(M_INPUT_COLOR_MASK<<O_INPUT_COLOR_MODE) ;
        uc |= B_INPUT_YUV422 << O_INPUT_COLOR_MODE ;
        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_YUV422 ;
        break ;
    case F_MODE_RGB24:
        uc &= ~(M_INPUT_COLOR_MASK<<O_INPUT_COLOR_MODE) ;
        uc |= B_INPUT_RGB24 << O_INPUT_COLOR_MODE ;
        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_RGB24 ;
//peter	2010.12.20	
		p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_0_255;
        break ;
    default:
        //LINUXV4L2_DEBUG( KERN_INFO, "Invalid Color mode %d, ignore.\n", bInMode);
        return ;
    }
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CSC_CTRL, uc) ;

}

void
SetColorimetryByMode(CDevice * p_Device/*PSYNC_INFO pSyncInfo*/)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetColorimetryByMode()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    p_sys_cfg->o_input_ite6603.bInputVideoMode &= ~F_MODE_ITU709 ;

    if((p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive == 1920)||(p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive == 1280 && p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive == 720) )
    {

        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_ITU709 ;
    }
    else
    {

        p_sys_cfg->o_input_ite6603.bInputVideoMode &= ~F_MODE_ITU709 ;
    }
}

void
SetVideoInputFormatWithInfoFrame(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetVideoInputFormatWithInfoFrame()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    BYTE uc ;
    BOOL bAVIColorModeIndicated = FALSE ;
    BOOL bOldInputVideoMode = p_sys_cfg->o_input_ite6603.bInputVideoMode ;

    //LINUXV4L2_DEBUG( KERN_INFO, "SetVideoInputFormatWithInfoFrame(): ");

    uc = HDMIRX_ReadI2C_Byte(p_Device, REG_RX_AVI_DB1) ;
    //LINUXV4L2_DEBUG( KERN_INFO, "REG_RX_AVI_DB1 %02X get uc %02X ",(int)REG_RX_AVI_DB1,(int)uc);

    p_sys_cfg->o_input_ite6603.prevAVIDB1 = uc ;
    p_sys_cfg->o_input_ite6603.bInputVideoMode &= ~F_MODE_CLRMOD_MASK ;

    switch((uc>>O_AVI_COLOR_MODE)&M_AVI_COLOR_MASK)
    {
    case B_AVI_COLOR_YUV444:
        LINUXV4L2_DEBUG( KERN_INFO, "input YUV444 mode ");
        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_YUV444 ;
        break ;
    case B_AVI_COLOR_YUV422:
        LINUXV4L2_DEBUG( KERN_INFO, "input YUV422 mode ");
        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_YUV422 ;
        break ;
    case B_AVI_COLOR_RGB24:
        LINUXV4L2_DEBUG( KERN_INFO, "input RGB24 mode ");
        p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_RGB24 ;
//peter	2010.12.20	
		p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_0_255;
        break ;
    default:
        LINUXV4L2_DEBUG( KERN_INFO, "Invalid input color mode, ignore.\n");
        return ;
    }

    if( (p_sys_cfg->o_input_ite6603.bInputVideoMode & F_MODE_CLRMOD_MASK)!=(bOldInputVideoMode & F_MODE_CLRMOD_MASK))
    {
        LINUXV4L2_DEBUG( KERN_INFO, "Input Video mode changed.");
    }

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CSC_CTRL) ;
    uc &= ~B_FORCE_COLOR_MODE ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CSC_CTRL, uc) ;

    LINUXV4L2_DEBUG( KERN_INFO, "\n");
}

BOOL
SetColorimetryByInfoFrame(CDevice * p_Device)
{
    BYTE uc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    BOOL bOldInputVideoMode = p_sys_cfg->o_input_ite6603.bInputVideoMode ;

    //LINUXV4L2_DEBUG( KERN_INFO, "SetColorimetryByInfoFrame: p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF = %s ",p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF?"TRUE":"FALSE");

    if(p_sys_cfg->o_input_ite6603.NewAVIInfoFrameF)
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB2) ;
        uc &= M_AVI_CLRMET_MASK<<O_AVI_CLRMET ;
        if(uc == (B_AVI_CLRMET_ITU601<<O_AVI_CLRMET))
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "F_MODE_ITU601\n");
            p_sys_cfg->o_input_ite6603.bInputVideoMode &= ~F_MODE_ITU709 ;
            return TRUE ;
        }
        else if(uc == (B_AVI_CLRMET_ITU709<<O_AVI_CLRMET))
        {
            //LINUXV4L2_DEBUG( KERN_INFO, "F_MODE_ITU709\n");
            p_sys_cfg->o_input_ite6603.bInputVideoMode |= F_MODE_ITU709 ;
            return TRUE ;
        }

        if( (p_sys_cfg->o_input_ite6603.bInputVideoMode & F_MODE_ITU709)!=(bOldInputVideoMode & F_MODE_ITU709))
        {
            LINUXV4L2_DEBUG( KERN_INFO, "Input Video mode changed.");

        }
    }
    LINUXV4L2_DEBUG( KERN_INFO, "\n");
    return FALSE ;
}

void
SetColorSpaceConvert(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetColorSpaceConvert()\n");
    BYTE uc, csc ;
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

   BYTE filter = 0 ;

    switch(p_sys_cfg->o_input_ite6603.bOutputVideoMode&F_MODE_CLRMOD_MASK)
    {
	#ifdef OUTPUT_YUV444
    case F_MODE_YUV444:

	    switch(p_sys_cfg->o_input_ite6603.bInputVideoMode&F_MODE_CLRMOD_MASK)
	    {
	    case F_MODE_YUV444:

	        csc = B_CSC_BYPASS ;
	        break ;
	    case F_MODE_YUV422:

            csc = B_CSC_BYPASS ;
            if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_UDFILT)
            {
                filter |= B_RX_EN_UDFILTER ;
            }

            if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_DITHER)
            {
                filter |= B_RX_EN_UDFILTER | B_RX_DNFREE_GO ;
            }

            break ;
	    case F_MODE_RGB24:

            csc = B_CSC_RGB2YUV ;
            break ;
	    }
        break ;
	#endif
	#ifdef OUTPUT_YUV422
    case F_MODE_YUV422:
	    switch(p_sys_cfg->o_input_ite6603.bInputVideoMode&F_MODE_CLRMOD_MASK)
	    {
	    case F_MODE_YUV444:

	        if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_UDFILT)
	        {
	            filter |= B_RX_EN_UDFILTER ;
	        }
	        csc = B_CSC_BYPASS ;
	        break ;
	    case F_MODE_YUV422:

            csc = B_CSC_BYPASS ;

            if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_DITHER)
            {
                filter |= B_RX_EN_UDFILTER | B_RX_DNFREE_GO ;
            }
	    	break ;
	    case F_MODE_RGB24:

            if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_UDFILT)
            {
                filter |= B_RX_EN_UDFILTER ;
            }
            csc = B_CSC_RGB2YUV ;
	    	break ;
	    }
	    break ;
	#endif
	#ifdef OUTPUT_RGB444
    case F_MODE_RGB24:

	    switch(p_sys_cfg->o_input_ite6603.bInputVideoMode&F_MODE_CLRMOD_MASK)
	    {
	    case F_MODE_YUV444:

	        csc = B_CSC_YUV2RGB ;
	        break ;
	    case F_MODE_YUV422:

            csc = B_CSC_YUV2RGB ;
            if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_UDFILT)
            {
                filter |= B_RX_EN_UDFILTER ;
            }
            if( p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_EN_DITHER)
            {
                filter |= B_RX_EN_UDFILTER | B_RX_DNFREE_GO ;
            }
	    	break ;
	    case F_MODE_RGB24:

            csc = B_CSC_BYPASS ;
	    	break ;
	    }
	    break ;
	#endif
    }

	#ifdef OUTPUT_YUV

    if( csc == B_CSC_RGB2YUV )
    {

        if(p_sys_cfg->o_input_ite6603.bInputVideoMode & F_MODE_ITU709)
        {
            LINUXV4L2_DEBUG( KERN_INFO, "ITU709 ");

            if(p_sys_cfg->o_input_ite6603.bInputVideoMode & F_MODE_16_235)
            {
                LINUXV4L2_DEBUG( KERN_INFO, " 16-235\n");
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_16_235,sizeof(bCSCOffset_16_235)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU709_16_235,sizeof(bCSCMtx_RGB2YUV_ITU709_16_235)) ;
            }
            else
            {
                LINUXV4L2_DEBUG( KERN_INFO, " 0-255\n");
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_0_255,sizeof(bCSCOffset_0_255)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU709_0_255,sizeof(bCSCMtx_RGB2YUV_ITU709_0_255)) ;
            }
        }
        else
        {
            LINUXV4L2_DEBUG( KERN_INFO, "ITU601 ");
            if(p_sys_cfg->o_input_ite6603.bInputVideoMode & F_MODE_16_235)
            {
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_16_235,sizeof(bCSCOffset_16_235)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU601_16_235,sizeof(bCSCMtx_RGB2YUV_ITU601_16_235)) ;
                LINUXV4L2_DEBUG( KERN_INFO, " 16-235\n");
            }
            else
            {
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_0_255,sizeof(bCSCOffset_0_255)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU601_0_255,sizeof(bCSCMtx_RGB2YUV_ITU601_0_255)) ;
                LINUXV4L2_DEBUG( KERN_INFO, " 0-255\n");
            }
        }
    }
	#endif

	#ifdef OUTPUT_RGB
	if ( csc == B_CSC_YUV2RGB )
    {
        LINUXV4L2_DEBUG( KERN_INFO, "CSC = YUV2RGB ");
        if(p_sys_cfg->o_input_ite6603.bInputVideoMode & F_MODE_ITU709)
        {
            LINUXV4L2_DEBUG( KERN_INFO, "ITU709 ");
            if(p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_16_235)
            {
                LINUXV4L2_DEBUG( KERN_INFO, "16-235\n");
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_16_235,sizeof(bCSCOffset_16_235)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU709_16_235,sizeof(bCSCMtx_YUV2RGB_ITU709_16_235)) ;
            }
            else
            {
                LINUXV4L2_DEBUG( KERN_INFO, "0-255\n");
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_0_255,sizeof(bCSCOffset_0_255)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU709_0_255,sizeof(bCSCMtx_YUV2RGB_ITU709_0_255)) ;
            }
        }
        else
        {
            LINUXV4L2_DEBUG( KERN_INFO, "ITU601 ");
            if(p_sys_cfg->o_input_ite6603.bOutputVideoMode & F_MODE_16_235)
            {
                LINUXV4L2_DEBUG( KERN_INFO, "16-235\n");
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_16_235,sizeof(bCSCOffset_16_235)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU601_16_235,sizeof(bCSCMtx_YUV2RGB_ITU601_16_235)) ;
            }
            else
            {
                LINUXV4L2_DEBUG( KERN_INFO, "0-255\n");
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_YOFF,bCSCOffset_0_255,sizeof(bCSCOffset_0_255)) ;
                HDMIRX_WriteI2C_ByteN( p_Device, REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU601_0_255,sizeof(bCSCMtx_YUV2RGB_ITU601_0_255)) ;
            }
        }

    }
	#endif

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CSC_CTRL) ;
    uc = (uc & ~M_CSC_SEL_MASK)|csc ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CSC_CTRL,uc) ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VIDEO_CTRL1) ;
//    uc &= ~(B_RX_DNFREE_GO|B_RX_EN_DITHER|B_RX_EN_UDFILTER) ;

	uc &= ~(B_RX_DNFREE_GO|B_RX_EN_DITHER) ;

	uc |= filter ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_VIDEO_CTRL1, uc) ;
}

void
SetDVIVideoOutput(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetDVIVideoOutput()\n");

    SetVideoInputFormatWithoutInfoFrame(p_Device, F_MODE_RGB24) ;
    SetColorimetryByMode(p_Device/*&SyncInfo*/) ;
    SetColorSpaceConvert(p_Device) ;
}

void
SetNewInfoVideoOutput(CDevice * p_Device)
{
    /*
    BYTE db1,db2,db3 ;

    do {
        delay1ms(10) ;
        db1 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB1) ;
        delay1ms(10) ;
        db2 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB1) ;
        delay1ms(10) ;
        db3 = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AVI_DB1) ;
        LINUXV4L2_DEBUG( KERN_INFO, "SetNewInfoVideoOutput(): %02X %02X %02X\n",(int)db1,(int)db2,(int)db3));
    } while ( (db1 != db2)||(db2!=db3)) ;
    */
    //LINUXV4L2_DEBUG( KERN_INFO, "SetNewInfoVideoOutput()\n");

    SetVideoInputFormatWithInfoFrame(p_Device) ;
    SetColorimetryByInfoFrame(p_Device) ;
    SetColorSpaceConvert(p_Device) ;
    //DumpCat6023Reg(p_Device);
}

void
SetCAT6023VideoOutputFormat(CDevice * p_Device, BYTE bOutputMapping, BYTE bOutputType, BYTE bOutputColorMode)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetCAT6023VideoOutputFormat()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    BYTE uc ;
    SetVideoMute(p_Device, ON) ;
    //LINUXV4L2_DEBUG( KERN_INFO, "%02X %02X %02X\n",(int)bOutputMapping,(int)bOutputType,(int)bOutputColorMode);
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_VIDEO_CTRL1,bOutputType) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_VIDEO_MAP,bOutputMapping) ;
    p_sys_cfg->o_input_ite6603.bOutputVideoMode&=~F_MODE_CLRMOD_MASK;

    p_sys_cfg->o_input_ite6603.bOutputVideoMode |= bOutputColorMode&F_MODE_CLRMOD_MASK ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_PG_CTRL2) & ~(M_OUTPUT_COLOR_MASK<<O_OUTPUT_COLOR_MODE);

    switch(p_sys_cfg->o_input_ite6603.bOutputVideoMode&F_MODE_CLRMOD_MASK)
    {
    case F_MODE_YUV444:
        uc |= B_OUTPUT_YUV444 << O_OUTPUT_COLOR_MODE ;
        break ;
    case F_MODE_YUV422:
        uc |= B_OUTPUT_YUV422 << O_OUTPUT_COLOR_MODE ;
        break ;
    }
    //LINUXV4L2_DEBUG( KERN_INFO, "write %02X %02X\n",(int)REG_RX_PG_CTRL2,(int)uc);
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_PG_CTRL2, uc) ;

    if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
    {
        if( IsCAT6023HDMIMode(p_Device) )
        {
            SetNewInfoVideoOutput(p_Device);
        }
        else
        {
            SetDVIVideoOutput(p_Device);
        }
        SetVideoMute(p_Device, p_sys_cfg->o_input_ite6603.MuteByPKG) ;
    }

}

void
ResetAudio(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "ResetAudio()\n");

    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, B_AUDRST) ;
    delay1ms(1) ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_RST_CTRL, 0) ;

}

void
SetHWMuteCTRL(CDevice * p_Device, BYTE AndMask, BYTE OrMask)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetHWMuteCTRL()\n");
    BYTE uc ;

    if( AndMask )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_HWMUTE_CTRL) ;
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWMUTE_CTRL,uc) ;

}

void
SetVideoMute(CDevice * p_Device, BOOL bMute)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetVideoMute()\n");
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

    BYTE uc ;
#ifdef SUPPORT_REPEATER
    if( p_sys_cfg->o_input_ite6603.bHDCPMode & HDCP_REPEATER )
    {
        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
        uc &= ~(B_TRI_VIDEO | B_TRI_VIDEOIO) ;
        uc |= B_VDO_MUTE_DISABLE ;
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;
        return ;
    }

#endif
    if( bMute )
    {

		uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CSC_CTRL) ;
		uc |= B_VDIO_GATTING ;
		HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CSC_CTRL, uc) ;

        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
        uc &= ~(B_TRI_VIDEO | B_TRI_VIDEOIO) ;
        uc |= B_VDO_MUTE_DISABLE ;
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;

    }
    else
    {
        if( p_sys_cfg->o_input_ite6603.VState == VSTATE_VideoOn )
        {
            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VIDEO_CTRL1) ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_VIDEO_CTRL1,uc|B_565FFRST) ;
            HDMIRX_WriteI2C_Byte( p_Device, REG_RX_VIDEO_CTRL1,uc&(~B_565FFRST)) ;

            uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
            uc &= ~(B_TRI_VIDEO | B_TRI_VIDEOIO) ;
            if(HDMIRX_ReadI2C_Byte( p_Device, REG_RX_VID_INPUT_ST)&B_AVMUTE)
            {
                uc |= B_VDO_MUTE_DISABLE ;
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;
            }
            else
            {
                uc &= ~B_VDO_MUTE_DISABLE ;
                HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;

        		uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL) ;
        		uc |= B_TRI_VIDEOIO ;
        		HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;
        		//LINUXV4L2_DEBUG( KERN_INFO, "reg %02X <- %02X = %02X\n",REG_RX_TRISTATE_CTRL,uc, HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL));
        		uc &= ~B_TRI_VIDEOIO ;
        		HDMIRX_WriteI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL, uc) ;
        		//LINUXV4L2_DEBUG( KERN_INFO, "reg %02X <- %02X = %02X\n",REG_RX_TRISTATE_CTRL,uc, HDMIRX_ReadI2C_Byte( p_Device, REG_RX_TRISTATE_CTRL));

        		uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CSC_CTRL) ;
        		uc |= B_VDIO_GATTING ;
        		HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CSC_CTRL, uc) ;
        		//LINUXV4L2_DEBUG( KERN_INFO, "reg %02X <- %02X = %02X\n",REG_RX_CSC_CTRL,uc, HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CSC_CTRL));
        		uc &= ~B_VDIO_GATTING ;
        		HDMIRX_WriteI2C_Byte( p_Device, REG_RX_CSC_CTRL, uc) ;
        		//LINUXV4L2_DEBUG( KERN_INFO, "reg %02X <- %02X = %02X\n",REG_RX_CSC_CTRL,uc, HDMIRX_ReadI2C_Byte( p_Device, REG_RX_CSC_CTRL));
        	}

        }
    }
}

void
SetAudioMute(CDevice * p_Device, BOOL bMute)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "SetAudioMute()\n");
    if( bMute )
    {
        SetMUTE(p_Device, ~B_TRI_AUDIO, B_TRI_AUDIO) ;
    }
    else
    {

        SetMUTE(p_Device, ~B_TRI_AUDIO, 0) ;
    }
}

BYTE
getCAT6023AudioStatus(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023AudioStatus()\n");
    BYTE uc,audio_status ;

    Switch_HDMIRX_Bank(p_Device, 0) ;

    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUDIO_CH_STAT) ;
    audio_status = 0 ;

    if( (uc & (B_AUDIO_ON|B_HBRAUDIO|B_DSDAUDIO)) == (BYTE)(B_AUDIO_ON|B_HBRAUDIO) )
    {
        audio_status = T_AUDIO_HBR ;
    }
    else if( (uc & (B_AUDIO_ON|B_HBRAUDIO|B_DSDAUDIO)) == (BYTE)(B_AUDIO_ON|B_DSDAUDIO) )
    {
        audio_status = T_AUDIO_DSD ;
    }
    else if( uc & B_AUDIO_ON)
    {
        if( HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT0) & (1<<1) )
        {

            audio_status = T_AUDIO_NLPCM ;
        }
        else
        {
            audio_status = T_AUDIO_LPCM ;
        }

        if( uc & B_AUDIO_LAYOUT )
        {
            audio_status |= F_AUDIO_LAYOUT_1 ;
        }

        if( uc & (1<<3) )
        {
            audio_status |= 4 ;
        }
        else if( uc & (1<<2) )
        {
            audio_status |= 3 ;
        }
        else if( uc & (1<<1) )
        {
            audio_status |= 2 ;
        }
        else if( uc & (1<<0) )
        {
            audio_status |= 1 ;
        }
    }

    return audio_status ;
}

BOOL
getCAT6023AudioChannelStatus(CDevice * p_Device, BYTE ucIEC60958ChStat[])
{
    //LINUXV4L2_DEBUG( KERN_INFO, "getCAT6023AudioChannelStatus()\n");
    BYTE fs,audio_status ;

    audio_status = getCAT6023AudioStatus(p_Device) ;

    if( ((audio_status & T_AUDIO_MASK) == T_AUDIO_OFF) ||
        ((audio_status & T_AUDIO_MASK) == T_AUDIO_DSD) )
    {

        return FALSE ;
    }

    Switch_HDMIRX_Bank(p_Device, 0) ;
    ucIEC60958ChStat[0] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT0) ;
    ucIEC60958ChStat[1] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT1) ;
    ucIEC60958ChStat[2] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT2) ;
    fs = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS) & M_Fs ;

    if( (audio_status & T_AUDIO_MASK) == T_AUDIO_HBR )
    {
        fs = B_Fs_HBR ;
        ucIEC60958ChStat[0] |= B_AUD_NLPCM ;
    }

    ucIEC60958ChStat[3] = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_AUD_CHSTAT3) ;

    ucIEC60958ChStat[4] = (ucIEC60958ChStat[3] >> 4) & 0xF ;
    ucIEC60958ChStat[4] |= ((~fs) & 0xF)<<4 ;

    ucIEC60958ChStat[3] &= 3 ;
    ucIEC60958ChStat[3] <<= 4 ;
    ucIEC60958ChStat[3] |= fs & 0xF ;

    return TRUE ;
}

void
setCAT6023_HBROutput(CDevice * p_Device, BOOL HBR_SPDIF)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "setCAT6023_HBROutput()\n");
    BYTE uc ;
    Switch_HDMIRX_Bank(p_Device, 0) ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_HWAMP_CTRL) ;

    if(HBR_SPDIF)
    {
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWAMP_CTRL, uc | B_HBR_SPDIF ) ;

        uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS_SET) ;
        uc &= ~0x30 ;
        uc |= 0x20 ;
        uc = (BYTE)HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET, uc) ;
        SetMUTE(p_Device, ~B_TRI_AUDIO, B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0) ;

    }
    else
    {
        HDMIRX_WriteI2C_Byte( p_Device, REG_RX_HWAMP_CTRL, uc | B_HBR_SPDIF ) ;
        SetMUTE(p_Device, ~B_TRI_AUDIO, B_TRI_SPDIF) ;

    }
}

void
setCAT6023_SPDIFOutput(CDevice * p_Device)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "setCAT6023_SPDIFOutput()\n");
    BYTE uc ;
    Switch_HDMIRX_Bank(p_Device, 0) ;
    uc = HDMIRX_ReadI2C_Byte( p_Device, REG_RX_FS_SET) ;
    uc &= ~0x30 ;
    uc |= 0x20 ;
    uc = (BYTE)HDMIRX_WriteI2C_Byte( p_Device, REG_RX_FS_SET, uc) ;
    SetMUTE(p_Device, ~B_TRI_AUDIO, B_TRI_I2S3|B_TRI_I2S2|B_TRI_I2S1|B_TRI_I2S0) ;
}

void
setCAT6023_I2SOutput(CDevice * p_Device, BYTE src_enable)
{
    //LINUXV4L2_DEBUG( KERN_INFO, "setCAT6023_I2SOutput()\n");
    Switch_HDMIRX_Bank(p_Device, 0) ;

    src_enable &= 0xF ;
    src_enable ^= 0xF ;
    SetMUTE(p_Device, ~B_TRI_AUDIO, B_TRI_SPDIF|src_enable) ;
}
void EnableMuteProcessTimer(CDevice * p_Device) 
{ 	
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	p_sys_cfg->o_input_ite6603.MuteResumingTimer = p_sys_cfg->o_input_ite6603.MuteByPKG?MUTE_RESUMING_TIMEOUT:0 ; 
}
void DisableMuteProcessTimer(CDevice * p_Device) 
{ 
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif
	p_sys_cfg->o_input_ite6603.MuteResumingTimer = 0 ; 
}

void StartAutoMuteOffTimer(CDevice * p_Device) 
{ 
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif
	p_sys_cfg->o_input_ite6603.MuteAutoOff = ON ; 
}
void EndAutoMuteOffTimer(CDevice * p_Device) 
{ 
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif
	p_sys_cfg->o_input_ite6603.MuteAutoOff = OFF ; 
}


void
DumpCat6023Reg(CDevice * p_Device)
{
    int i,j ;
    BYTE ucData ;

    LINUXV4L2_DEBUG( KERN_INFO, "       ");
    for( j = 0 ; j < 16 ; j++ )
    {
        LINUXV4L2_DEBUG( KERN_INFO, " %02X",(int)j);
        if( (j == 3)||(j==7)||(j==11))
        {
            LINUXV4L2_DEBUG( KERN_INFO, "  ");
        }
    }
    LINUXV4L2_DEBUG( KERN_INFO, "\n        -----------------------------------------------------\n");

    Switch_HDMIRX_Bank(p_Device, 0);

    for(i = 0 ; i < 0x100 ; i+=16 )
    {
        LINUXV4L2_DEBUG( KERN_INFO, "[%3X]  ",(int)i);
        for( j = 0 ; j < 16 ; j++ )
        {
            ucData = HDMIRX_ReadI2C_Byte( p_Device, (BYTE)((i+j)&0xFF) );
            LINUXV4L2_DEBUG( KERN_INFO, " %02X",(int)ucData);
            if( (j == 3)||(j==7)||(j==11))
            {
                LINUXV4L2_DEBUG( KERN_INFO, " -");
            }
        }
        LINUXV4L2_DEBUG( KERN_INFO, "\n");
        if( (i % 0x40) == 0x30)
        {
            LINUXV4L2_DEBUG( KERN_INFO, "        -----------------------------------------------------\n");
        }
    }

    Switch_HDMIRX_Bank(p_Device, 1);
    for(i = 0x180; i < 0x200 ; i+=16 )
    {
        LINUXV4L2_DEBUG( KERN_INFO, "[%3X]  ",(int)i);
        for( j = 0 ; j < 16 ; j++ )
        {
            ucData = HDMIRX_ReadI2C_Byte( p_Device, (BYTE)((i+j)&0xFF));
            LINUXV4L2_DEBUG( KERN_INFO, " %02X",(int)ucData);
            if( (j == 3)||(j==7)||(j==11))
            {
                LINUXV4L2_DEBUG( KERN_INFO, " -");
            }
        }
        LINUXV4L2_DEBUG( KERN_INFO, "\n");
        if( (i % 0x40) == 0x30)
        {
            LINUXV4L2_DEBUG( KERN_INFO, "        -----------------------------------------------------\n");
        }

    }

    Switch_HDMIRX_Bank(p_Device, 0);
}

BYTE 
GetVerticalFreq( CDevice * p_Device, WORD *Vreq  )
{
	
 unsigned int audioN;
	
 unsigned char tempbyte, temp1;
	
 WORD HTOT, VACT, VTOT, SCANMODE, HACT;
	
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(p_Device->m_pCustomSystemConfigProperty);
	
	#endif


 VTOT			= p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal;
 
 HACT 			= p_sys_cfg->o_input_ite6603.s_CurrentVM.HActive;	
 
 VACT			= p_sys_cfg->o_input_ite6603.s_CurrentVM.VActive;
		
 HTOT			=	p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal;

 SCANMODE		=	p_sys_cfg->o_input_ite6603.s_CurrentVM.ScanMode;
 
 if( p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal == 0 ) return 0;

 if( p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal == 0 ) return 0;

 if( p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt == 0 ) return 0;

 audioN = ((128/2)*27000000)/(int)(p_sys_cfg->o_input_ite6603.s_CurrentVM.HTotal/2);
 audioN *= 100;
 audioN = audioN/((int)p_sys_cfg->o_input_ite6603.s_CurrentVM.VTotal * p_sys_cfg->o_input_ite6603.s_CurrentVM.xCnt);
 temp1 = (unsigned char)(audioN%100);
 tempbyte = audioN/100;
 if(temp1 > 20)
	 tempbyte += 1;
 
 if(audioN < 2600)
 {	
		 if(	HTOT == 2750 && VACT == 1080		){			//	1080p24
			 
				*Vreq  = 24;
			 
				goto end;
		 }
 }
 if( audioN > 5400 && audioN < 5800 ){

			if(	HTOT == 1024 && VACT == 600		){			//	800x600p56
				
					*Vreq  = 56;
					
					goto end;
			}
 }
 if( audioN > 4700 && audioN < 4900 ){
		
		*Vreq  = 48;
	 
	  if((( HACT == 1920 ) && ( VACT == 1080 ) && ( SCANMODE == 1 ) ) ||
			 (( HACT == 720  ) && ( VACT == 480  ) && ( SCANMODE == 1 ) ) ||
			 (( HACT == 1280 ) && ( VACT == 720  ) && ( SCANMODE == 1 ) )){
			
				*Vreq  = 50;
		}
			
		goto end;	 
 }
 if( audioN > 7000 && audioN < 7400 ){
	 
		 if( HTOT == 832 && VTOT == 520 	){				//	640x480	72 Hz
					
					*Vreq  = 72;
			 
					goto end;
		 }
		 if( HTOT == 1040 && VTOT == 666 ){					//	800x600 72Hz
			
					*Vreq  = 72;
			 
					goto end;
		 }
 }
 if( audioN > 8475 && audioN < 8885 ){
	 
		 if( HTOT == 1728 && VTOT == 1072 	){				//	640x480	72 Hz
					
					*Vreq  = 85;
			 
					goto end;
		}	 
 }
 {
		temp1 = tempbyte%5;
	 
		tempbyte = tempbyte - temp1;
 
		if(temp1 >= 3)
			tempbyte += 5;
		
		*Vreq  = tempbyte;
 }
 
end:
 
 Switch_HDMIRX_Bank(p_Device, 0) ;
 
 audioN = 0;
 
 audioN |= HDMIRX_ReadI2C_Byte(p_Device, REG_RX_N_RCV1);
 
 audioN |= ((unsigned int)HDMIRX_ReadI2C_Byte(p_Device, REG_RX_N_RCV2))<<8;
 
 audioN |= ((unsigned int)(HDMIRX_ReadI2C_Byte(p_Device, REG_RX_N_RCV3)&0xf))<<16;
 
 {
 
#if 0	 
	  DWORD CTS = 0;
	 			 
		CTS |= ( HDMIRX_ReadI2C_Byte( 0x81 ) >> 4) & 0x0F;
	 
		CTS |= HDMIRX_ReadI2C_Byte( 0x82 ) << 4;
	 
		CTS |= HDMIRX_ReadI2C_Byte( 0x83 ) << 12;
	 
		if( CTS != 0x91 )
			return 101;	 
#else
		//BYTE R083 = 0;
		
		//R083 = HDMIRX_ReadI2C_Byte( 0x83 );
	
		//if( R083 != 0x91 )
		//	return 101;	 
		
#endif		
 }
 while((audioN & 0xfff000) > 0x1000)
     audioN /= 2;
// 32K, 44.1K, 48KHz
 if((audioN & 0x7f) == 0)
     return 100;						// FREQ IS 1000

 return 101;							// FREQ IS 1001
}
