#ifndef CX2581_H
#define CX2581_H

#include "LINUXV4L2.h"

BOOLEAN CX2581_HwInitialize( CDevice * pDevice );
BOOLEAN CX2581_HwUnInitialize( CDevice * pDevice );
BOOLEAN CX2581_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN CX2581_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN CX2581_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
BOOLEAN CX2581_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN CX2581_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, CVideo * pVideo );
BOOLEAN CX2581_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, CVideo * pVideo );

BOOLEAN CX2581_PowerDownPeripherals( CDevice * pDevice );

//shawn 2010.11.26
DWORD   CX2581_GetAnalogVideoDecoderRegister( CDevice * pDevice, USHORT wIndex, ULONG nBytes );
BOOLEAN CX2581_SetAnalogVideoDecoderRegister( CDevice * pDevice, USHORT wIndex, DWORD dwValue, ULONG nBytes );
//

ULONG   CX2581_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes );
BOOLEAN CX2581_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes );

#endif
