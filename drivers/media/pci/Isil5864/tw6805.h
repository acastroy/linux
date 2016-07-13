#ifndef TW6805_H
#define TW6805_H

#include "LINUXV4L2.h"

BOOLEAN TW6805_HwInitialize( CDevice * pDevice );
BOOLEAN TW6805_HwUnInitialize( CDevice * pDevice );
BOOLEAN TW6805_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN TW6805_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN TW6805_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
BOOLEAN TW6805_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN TW6805_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, CVideo * pVideo, ULONG nInputNumber );
BOOLEAN TW6805_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice );

BOOLEAN TW6805_PowerDownPeripherals( CDevice * pDevice );

ULONG   TW6805_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes );
BOOLEAN TW6805_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes );

//static int TW6805_StartControlPanelAnalysisThread( CDevice * pDevice );
//static int TW6805_StopControlPanelAnalysisThread( CDevice * pDevice );
//static int TW6805_OnControlPanelAnalysisThread( CDevice * pDevice );

BOOLEAN OnSetCustomAnalogVideoFrameRateProperty( CDevice * pDevice );

ULONG TW6816_SET_GPIO_DATA( CDevice * pDevice, BYTE R );
ULONG TW6816_GET_GPIO_DATA( CDevice * pDevice, BYTE R );

#endif
