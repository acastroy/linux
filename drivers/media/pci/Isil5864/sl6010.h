#ifndef SL6010_H
#define SL6010_H

#include "LINUXV4L2.h"

BOOLEAN SL6010_HwInitialize( CDevice * pDevice );
BOOLEAN SL6010_HwUnInitialize( CDevice * pDevice );
BOOLEAN SL6010_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN SL6010_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );

BOOLEAN SL6010_PowerDownPeripherals( CDevice * pDevice );

BOOLEAN SL6010_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes );
ULONG   SL6010_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes );

#endif
