#ifndef CX2588_H
#define CX2588_H

#include "LINUXV4L2.h"

BOOLEAN CX2588_HwInitialize( CDevice * pDevice );
BOOLEAN CX2588_HwUnInitialize( CDevice * pDevice );
BOOLEAN CX2588_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN CX2588_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN CX2588_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
BOOLEAN CX2588_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN CX2588_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, CVideo * pVideo );
BOOLEAN CX2588_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, CVideo * pVideo );

BOOLEAN CX2588_PowerDownPeripherals( CDevice * pDevice );

ULONG   CX2588_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes );
BOOLEAN CX2588_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes );

#endif

