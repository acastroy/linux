#ifndef _FH8735_H
#define _FH8735_H

#include "LINUXV4L2.h"
#include "sys_cfg.h"
#define  BOOL BOOLEAN                 

ULONG FH8735_GetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex );
BOOLEAN FH8735_SetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG dwValue );

BOOLEAN FH8735_DELAY_100NS( CDevice * pDevice, ULONGLONG DELAY );
DWORD FH8735_GetEepromRegister( CDevice * pDevice, BYTE bDevAddr, BYTE bIndex );
VOID FH8735_I2C_SET_SCL( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY );
VOID FH8735_I2C_SET_SDA( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY );
VOID FH8735_I2C_SET_PIN( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY ) ;
BYTE FH8735_I2C_GET_SDA( CDevice * pDevice, ULONGLONG DELAY );
BOOL FH8735_I2C_START( CDevice * pDevice, ULONGLONG DELAY );
BOOL FH8735_I2C_STOP( CDevice * pDevice, ULONGLONG DELAY );
BOOL FH8735_I2C_WAIT_ACK( CDevice * pDevice, ULONGLONG DELAY );
BOOL FH8735_I2C_REPLY_ACK( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY );
BOOL FH8735_I2C_WRITE_BYTE( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY );
BYTE FH8735_I2C_READ_BYTE( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY );
VOID FH8735_GSPI_SET_SCLK( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY );
VOID FH8735_GSPI_SET_CS( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY );
VOID FH8735_GSPI_SET_SDIN( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY );
BYTE FH8735_GSPI_GET_SDOUT( CDevice * pDevice, ULONGLONG DELAY );
BOOL FH8735_GSPI_WRITE_WORD( CDevice * pDevice, USHORT wAddress, USHORT wValue, ULONGLONG DELAY );
USHORT FH8735_GSPI_READ_WORD( CDevice * pDevice, USHORT wAddress, ULONGLONG DELAY );
BOOLEAN FH8735_WaitInterruptComplete( CDevice * pDevice ); 

// ##############################################################################################################################################################################
BOOLEAN FH8735_WaitInterruptComplete( CDevice * pDevice ) ;
BOOLEAN FH8735_SendVendorCommand( CDevice * pDevice, ULONG nCommand, ULONG nParameter );
VOID FH8735_SetAnalogVideoProcAmpProperties( CDevice * pDevice );


BOOLEAN FH8735_HwInitialize( CDevice * pDevice );
BOOLEAN FH8735_HwUnInitialize( CDevice * pDevice );
BOOLEAN FH8735_HwAnalogComponentsInitialize( CDevice * pDevice );
BOOLEAN FH8735_HwAnalogComponentsUnInitialize( CDevice * pDevice );

BOOLEAN FH8735_UpdateFirmware( CDevice * pDevice );

BOOLEAN FH8735_PowerDownPeripherals( CDevice * pDevice );

BOOLEAN FH8735_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN FH8735_HwProcessAnalogPCIVideoFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

BOOLEAN FH8735_HwProcessAnalogPCIVideoEncoderFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

BOOLEAN FH8735_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN FH8735_CH_DOWNLOAD_OSD( CDevice * pDevice, ULONG j, char* pwszFileName, req_tosd * p_tosd_req );

BYTE FH8735_GetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex );
BYTE FH8735_GetAnalogVideoDecoderRegister_EX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BOOL * p_returns );
BYTE FH8735_GetAnalogVideoDecoderRegister_EX2( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex );

BOOLEAN FH8735_SetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex, BYTE bValue );
BOOLEAN FH8735_SetAnalogVideoDecoderRegister_EX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex, BYTE nValue );

VOID FH8735_SetAnalogCrossbarRouteProperty( CDevice * pDevice );

BYTE FH8735_GetGpioRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr );

BOOLEAN FH8735_SetGpioRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bValue );

int FH8735_StartControlPanelAnalysisThreadEX( CDevice * pDevice );
int FH8735_StopControlPanelAnalysisThreadEX( CDevice * pDevice );

NTSTATUS FH8735_SetAnalogAudioVoulmeProperty( CDevice * pDevice, ULONG nVolume, ULONG channel );

#endif
