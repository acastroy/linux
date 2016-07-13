#include "LINUXV4L2.h"
#include "property.h"
#include "sl6010.h"

BOOLEAN SL6010_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes )
{
	if( nBytes == 4 ) { writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 2 ) { writew( (dwValue & 0x0000FFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 1 ) { writeb( (dwValue & 0x000000FF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	return TRUE;
}

ULONG SL6010_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes )
{
	if( nBytes == 4 ) { return readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0xFFFFFFFF; }

	if( nBytes == 2 ) { return readw( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x0000FFFF; }

	if( nBytes == 1 ) { return readb( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x000000FF; }

	return 0x00000000;
}

BOOLEAN SL6010_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	LINUXV4L2_DEBUG( KERN_INFO, "SL6010_HwAnalogComponentsInitialize()\n" );

	return TRUE;
}

BOOLEAN SL6010_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	LINUXV4L2_DEBUG( KERN_INFO, "SL6010_HwAnalogComponentsUnInitialize()\n" );

	return TRUE;
}

BOOLEAN SL6010_HwUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "SL6010_HwUnInitialize()\n" );

	return TRUE;
}

BOOLEAN SL6010_HwInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "SL6010_HwInitialize( %08X )\n", (unsigned int)(SL6010_GetRegister( pDevice, 0x0000, 4 )) );

	return SL6010_PowerDownPeripherals( pDevice );
}

BOOLEAN SL6010_PowerDownPeripherals( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "SL6010_PowerDownPeripherals()\n" );

	return TRUE;
}
