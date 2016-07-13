#include "LINUXV4L2.h"
#include "property.h"
#include "tw6805.h"

ULONG TW6805_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes )
{
	if( nBytes == 4 ) { return readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0xFFFFFFFF; }

	if( nBytes == 2 ) { return readw( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x0000FFFF; }

	if( nBytes == 1 ) { return readb( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x000000FF; }

	return 0x00000000;
}

BOOLEAN TW6805_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes )
{
	if( nBytes == 4 ) { writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 2 ) { writew( (dwValue & 0x0000FFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 1 ) { writeb( (dwValue & 0x000000FF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	return TRUE;
}
BOOLEAN TW6805_DELAY_100NS( CDevice * pDevice, ULONGLONG DELAY )
{

	ndelay( DELAY * 100 );

	return TRUE;
}

VOID I2C_SET_SCL( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{

	TW6805_SetRegister( pDevice, 0x00000014, (TW6805_GetRegister( pDevice, 0x00000014, 1 ) & 0x000000BF) | (DATA << 6), 1 );

	TW6805_DELAY_100NS( pDevice, DELAY );	

}

VOID I2C_SET_SDA( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{

	TW6805_SetRegister( pDevice, 0x00000014, (TW6805_GetRegister( pDevice, 0x00000014, 1 ) & 0x0000007F) | (DATA << 7), 1 );

	TW6805_DELAY_100NS( pDevice, DELAY );	
}

VOID I2C_SET_PIN( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY ) 
{

	TW6805_SetRegister( pDevice, 0x00000014, (DATA << 6), 1 );

	TW6805_DELAY_100NS( pDevice, DELAY );	
}

BYTE I2C_GET_SDA( CDevice * pDevice, ULONGLONG DELAY ) 
{

	BYTE DATA = (BYTE)(TW6805_GetRegister( pDevice, 0x00000014, 1 ) & 0x00000080) >> 7;

	TW6805_DELAY_100NS( pDevice, DELAY );	

	return DATA;
}

BOOLEAN I2C_START( CDevice * pDevice, ULONGLONG DELAY )
{
	I2C_SET_SDA( pDevice, 1, DELAY );

	I2C_SET_PIN( pDevice, 3, DELAY );

	I2C_SET_PIN( pDevice, 1, DELAY );

	I2C_SET_PIN( pDevice, 0, DELAY );

	return TRUE;
}

BOOLEAN I2C_STOP( CDevice * pDevice, ULONGLONG DELAY )
{
	I2C_SET_SCL( pDevice, 0, DELAY );

	I2C_SET_PIN( pDevice, 0, DELAY );

	I2C_SET_PIN( pDevice, 1, DELAY );

	I2C_SET_PIN( pDevice, 3, DELAY );

	return TRUE;
}

BOOLEAN I2C_WAIT_ACK( CDevice * pDevice, ULONGLONG DELAY )
{	
	BYTE ACK = 0x00;

	I2C_SET_PIN( pDevice, 3, DELAY );   

	ACK = I2C_GET_SDA( pDevice, DELAY );

	I2C_SET_PIN( pDevice, 0, DELAY );   

	return (ACK == 0) ? TRUE : FALSE;   
}

BOOLEAN I2C_REPLY_ACK( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY )
{
	I2C_SET_PIN( pDevice, (ACK) ? 2 : 0, DELAY );

	I2C_SET_PIN( pDevice, (ACK) ? 3 : 1, DELAY );

	I2C_SET_PIN( pDevice, (ACK) ? 2 : 0, DELAY );

	return TRUE;
}

BOOLEAN I2C_WRITE_BYTE( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONG i = 0;

	for( i = 0 ; i < 8 ; i++, DATA <<= 1 ) {                 

		I2C_SET_PIN( pDevice, (DATA & 0x80) ? 2 : 0, DELAY );

		I2C_SET_PIN( pDevice, (DATA & 0x80) ? 3 : 1, DELAY );

		I2C_SET_PIN( pDevice, (DATA & 0x80) ? 2 : 0, DELAY );
	}
	return I2C_WAIT_ACK( pDevice, DELAY );                   
}

BYTE I2C_READ_BYTE( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY )
{	
	BYTE DATA = 0x00;

	ULONG i = 0;

	for( i = 0 ; i < 8 ; i++ ) {

		I2C_SET_PIN( pDevice, 2, DELAY );

		I2C_SET_PIN( pDevice, 3, DELAY );				

		DATA = (DATA << 1) | I2C_GET_SDA( pDevice, DELAY );

		I2C_SET_PIN( pDevice, 0, DELAY );				   
	}
	I2C_REPLY_ACK( pDevice, ACK, DELAY );                  

	return DATA;
}

static ULONG g_copy_protect_unlock_boradsA[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static ULONG g_copy_protect_unlock_boradsB[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//shawn 2010.08.11 WATCHDOG

static int TW6805_OnControlPanelAnalysisThread( void * p_context )
{
	ULONG counts = 0;

	CDevice * pDevice = p_context;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] TW6805_OnControlPanelAnalysisThread( enter ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pDevice->m_pControlThread) );

	while( kthread_should_stop() == FALSE ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ){
			
			if( pDevice->m_nCustomSoftwareWatchdogResetProperty & 0x00000001 ) {
		
				if( pDevice->m_nCustomSoftwareWatchdogResetProperty & 0x80000000 ) { // WATCHDOG
		
					I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); I2C_WRITE_BYTE( pDevice, 0x12, 125 ); I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); I2C_STOP( pDevice, 125 );
		
					pDevice->m_nCustomSoftwareWatchdogResetProperty = 0x00000001;
				}
			}
			else {

				if( (counts++ % 5) == 0 ) { // HARDWARE RESET WATCHDOG

					I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); I2C_WRITE_BYTE( pDevice, 0x12, 125 ); I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); I2C_STOP( pDevice, 125 );
				}
			}
		}

		schedule_timeout_interruptible( msecs_to_jiffies( 1000 ) ); //
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] TW6805_OnControlPanelAnalysisThread( exit ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pDevice->m_pControlThread) );

	return 0;	
}

static int TW6805_StartControlPanelAnalysisThread( CDevice * pDevice )
{

	if( pDevice->m_pControlThread == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] TW6805_StartControlPanelAnalysisThread()\n", (int)(pDevice->m_nKsDeviceNumber) );

		pDevice->m_pControlThread = kthread_run( TW6805_OnControlPanelAnalysisThread, pDevice, "TW6805_OnControlPanelAnalysisThread" );

		if( IS_ERR( pDevice->m_pControlThread ) ) {

			int ret = PTR_ERR( pDevice->m_pControlThread );

			pDevice->m_pControlThread = NULL;

			return ret;
		}
	}
	return 0;
}

static int TW6805_StopControlPanelAnalysisThread( CDevice * pDevice )
{

	if( pDevice->m_pControlThread ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] TW6805_StopControlPanelAnalysisThread()\n", (int)(pDevice->m_nKsDeviceNumber) );

		kthread_stop( pDevice->m_pControlThread ); // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "STOP" SYNC.

		pDevice->m_pControlThread = NULL;
	}
	return 0;

}

//

BOOLEAN OnSetCustomAnalogVideoFrameRateProperty( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "OnSetCustomAnalogVideoFrameRateProperty() value(0x%x)\n", pDevice->m_nCustomAnalogVideoFrameRateProperty);

	ULONG R0000004C = 0;

	ULONG R0000004D = 0;

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) )
	{
		R0000004C = (0x00000100) / (pDevice->m_nCustomAnalogVideoFrameRateProperty + 1);

		R0000004D = (0x00000100) / (pDevice->m_nCustomAnalogVideoFrameRateProperty + 1);
	}
	else
	{
		R0000004C = pDevice->m_nCustomAnalogVideoFrameRateProperty;

		R0000004D = pDevice->m_nCustomAnalogVideoFrameRateProperty;
	}

	TW6805_SetRegister( pDevice, 0x0000004C, (BYTE)(R0000004C & 0xFF), 1 );

	TW6805_SetRegister( pDevice, 0x0000004D, (BYTE)(R0000004D & 0xFF), 1 );

	return TRUE;
}


BOOLEAN TW6805_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, CVideo * pVideo, ULONG nInputNumber )
{
//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_SetAnalogVideoDecoderProcAmpProperties()\n", pDevice->m_nKsDeviceNumber );

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0;

		ULONG j = nInputNumber % 4;

		//
		// 
		if( g_n_analog_decoder_brightness[ i ][ j ] & 0x80000000 ) { TW6805_SetRegister( pDevice, 0x00000240, (BYTE)(((g_n_analog_decoder_brightness[ i ][ j ] & 0xFF) >> 0) - 128) | 0x00, 1 ); g_n_analog_decoder_brightness[ i ][ j ] &= 0x7FFFFFFF; }

		if( g_n_analog_decoder_contrast[ i ][ j ]   & 0x80000000 ) { TW6805_SetRegister( pDevice, 0x00000244, (BYTE)(((g_n_analog_decoder_contrast[ i ][ j ]   & 0xFF) >> 0) -   0) | 0x00, 1 ); g_n_analog_decoder_contrast[ i ][ j ]   &= 0x7FFFFFFF; }

		if( g_n_analog_decoder_saturation[ i ][ j ] & 0x80000000 ) { TW6805_SetRegister( pDevice, 0x0000024C, (BYTE)(((g_n_analog_decoder_saturation[ i ][ j ] & 0xFF) >> 0) -   0) | 0x00, 1 ); 
		
															         TW6805_SetRegister( pDevice, 0x00000250, (BYTE)(((g_n_analog_decoder_saturation[ i ][ j ] & 0xFF) >> 0) -   0) | 0x00, 1 ); g_n_analog_decoder_saturation[ i ][ j ] &= 0x7FFFFFFF; }

		if( g_n_analog_decoder_hue[ i ][ j ]        & 0x80000000 ) { TW6805_SetRegister( pDevice, 0x00000254, (BYTE)(((g_n_analog_decoder_hue[ i ][ j ]        & 0xFF) >> 0) - 128) | 0x00, 1 ); g_n_analog_decoder_hue[ i ][ j ]        &= 0x7FFFFFFF; }

		if( g_n_analog_decoder_sharpness[ i ][ j ]  & 0x80000000 ) { TW6805_SetRegister( pDevice, 0x00000248, (BYTE)(((g_n_analog_decoder_sharpness[ i ][ j ]  & 0xFF) >> 4) -   0) | 0x50, 1 ); g_n_analog_decoder_sharpness[ i ][ j ]  &= 0x7FFFFFFF; }

		//
		// 
		if( g_n_analog_decoder_gain_auto[ i ] & 0x80000000 ||

			g_n_analog_decoder_gain[ i ]      & 0x80000000 ) {
			
			if( (g_n_analog_decoder_gain_auto[ i ] & 0x7FFFFFFF) > 0x00000000 ) { //

				BYTE R00000284 = (BYTE)(TW6805_GetRegister( pDevice, 0x00000284, 1 ));

				TW6805_SetRegister( pDevice, 0x00000284, R00000284 & 0xFE, 1 );

				TW6805_SetRegister( pDevice, 0x00000288, 0xF0, 1 );

				TW6805_SetRegister( pDevice, 0x00000218, 0x00, 1 ); 
			}
			else { //

				BYTE R00000284 = (BYTE)(TW6805_GetRegister( pDevice, 0x00000284, 1 ));

				TW6805_SetRegister( pDevice, 0x00000284, (R00000284 & 0xFE) | ((g_n_analog_decoder_gain[ i ] & 0x00000100) >> 8), 1 );

				TW6805_SetRegister( pDevice, 0x00000288, (g_n_analog_decoder_gain[ i ] & 0xFF), 1 );

				TW6805_SetRegister( pDevice, 0x00000218, 0x10, 1 );
			}
			g_n_analog_decoder_gain_auto[ i ] &= 0x7FFFFFFF;

			g_n_analog_decoder_gain[ i ] &= 0x7FFFFFFF;
		}
	}
	return TRUE;
}

BOOLEAN TW6805_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice )
{
	ULONG e_v4l2_std = pDevice->m_nCustomVideoStandardProperty;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_SetAnalogVideoDecoderStandardProperty( S:%08lX )\n", pDevice->m_nKsDeviceNumber, e_v4l2_std );

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		BYTE R0230 = (BYTE)(TW6805_GetRegister( pDevice, 0x00000230, 1 ) & 0xEF);

		switch( e_v4l2_std ) {

		case V4L2_STD_NTSC_M:    TW6805_SetRegister( pDevice, 0x00000270, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x10 | R0230, 1 ); break;
		
		case V4L2_STD_NTSC_M_JP: TW6805_SetRegister( pDevice, 0x00000270, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_NTSC_443:  TW6805_SetRegister( pDevice, 0x00000270, 0x03, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x10 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_60:    TW6805_SetRegister( pDevice, 0x00000270, 0x06, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_M:     TW6805_SetRegister( pDevice, 0x00000270, 0x04, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x10 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_B:     TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_B1:    TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_D:     TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_D1:    TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_G:     TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_H:     TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_I:     TW6805_SetRegister( pDevice, 0x00000270, 0x01, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_N:     TW6805_SetRegister( pDevice, 0x00000270, 0x05, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x10 | R0230, 1 ); break;
		
		case V4L2_STD_PAL_Nc:    TW6805_SetRegister( pDevice, 0x00000270, 0x05, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_B:   TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_G:   TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_H:   TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_D:   TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_K:   TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_K1:  TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_L:   TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		
		case V4L2_STD_SECAM_LC:  TW6805_SetRegister( pDevice, 0x00000270, 0x02, 1 ); TW6805_SetRegister( pDevice, 0x00000230, 0x00 | R0230, 1 ); break;
		}
	}
	return TRUE;
}

BOOLEAN TW6805_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	dma_addr_t pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
	
	dma_addr_t pDmaBaseCommonPhysicalBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
	
	DWORD *    pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);
	
	DWORD *    pDmaBaseCommonBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);
	
	ULONG      R00000000 = 0x00000000;
	
	ULONG      i = 0;
	
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 0 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}
	else { 

		pDevice->m_nAnalogCaptureStreamPowerReference++;

		return TRUE;
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwAnalogComponentsInitialize()\n", pDevice->m_nKsDeviceNumber );
	
//	{	if( g_copy_protect_unlock_boradsA[ pDevice->m_pKsDevice->bus->number ] == 0 ) { return TRUE; }
//	}
//	{	if( g_copy_protect_unlock_boradsB[ pDevice->m_pKsDevice->bus->number ] == 0 ) { return TRUE; }
//	}

	// 
	//
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0;

		ULONG j = 0;

		TW6805_SetAnalogVideoDecoderStandardProperty( pDevice );

		for( j = 0 ; j < 4 ; j++ ) {

			g_n_analog_decoder_brightness[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_contrast[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_saturation[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_hue[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_sharpness[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_gain_auto[ i ] |= 0x80000000;

			g_n_analog_decoder_gain[ i ] |= 0x80000000;

			TW6805_SetAnalogVideoDecoderProcAmpProperties( pDevice, pVideo, j );
		}
	}

	pDmaBaseCommonPhysicalStartBuffer += 0;

	pDmaBaseCommonPhysicalBuffer += 0;

	pDmaBaseCommonStartBuffer += 0;

	pDmaBaseCommonBuffer += 0;

	TW6805_SetRegister( pDevice, 0x00000004, pDmaBaseCommonPhysicalStartBuffer, 4 );
	
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {
	
		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {
			
			TW6805_SetRegister( pDevice, 0x00000224, 0xF0, 1 );
			
			TW6805_SetRegister( pDevice, 0x0000021C, 0x02, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E4, 0xF0, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003DC, 0x02, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002CC, 0x85, 1 );
		}
		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {
			
			TW6805_SetRegister( pDevice, 0x00000224, 0x20, 1 );
			
			TW6805_SetRegister( pDevice, 0x0000021C, 0x12, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E4, 0x20, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003DC, 0x12, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002CC, 0xC5, 1 );
		}

		pDmaBaseCommonPhysicalBuffer += 24;
		
		pDmaBaseCommonBuffer += 0 >> 2;
		
		pDmaBaseCommonBuffer[ 0 ] = 0xC0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;
		
		pDmaBaseCommonPhysicalBuffer += 3600;
		
		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBA000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 24;
		
		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xD0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;
		
		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xC0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xD0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		if( pVideo->m_nSrcFrameHeight <= 288 ) { pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalStartBuffer; } //

		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xC0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xD0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		if( pVideo->m_nSrcFrameHeight <= 288 ) { pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalStartBuffer; } //

		if( pVideo->m_nSrcFrameHeight  > 288 ) { pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalStartBuffer; } //
		
		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xC0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xD0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;
		
		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xC0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 24;

		pDmaBaseCommonBuffer += 3600 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xD0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0xB0000000;
		
		pDmaBaseCommonBuffer[ 3 ] = pDmaBaseCommonPhysicalBuffer;
		
		pDmaBaseCommonBuffer[ 4 ] = 0xE8000000;

		pDmaBaseCommonPhysicalBuffer += 3600;

		pDmaBaseCommonBuffer += 24 >> 2;

		pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
		
		pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
		
		pDmaBaseCommonBuffer[ 2 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 3 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 4 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 5 ] = 0x90000000;
		
		pDmaBaseCommonBuffer[ 6 ] = 0xBB000000;
		
		pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalStartBuffer;
		
		pDmaBaseCommonBuffer[ 8 ] = 0xE8000000;

		//shawn 2010.10.04
		if( pDevice->m_nCustomAnalogVideoScaleOutputProperty == 0 ) {

			if( pVideo->m_nDstFrameWidth == 720 || pVideo->m_nDstFrameWidth == 704 || pVideo->m_nDstFrameWidth == 640) {

				TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); //
				TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); 
				TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); //
				TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 );
			}
			if( pVideo->m_nDstFrameWidth == 360 || pVideo->m_nDstFrameWidth == 352 || pVideo->m_nDstFrameWidth == 320) {

				TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); //
				TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); 
				TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); //
				TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 );
			}
		}
		if( pDevice->m_nCustomAnalogVideoScaleOutputProperty == 1 ) {

			if( pVideo->m_nDstFrameHeight == 480 || pVideo->m_nDstFrameHeight == 240 ) {

				if( pVideo->m_nDstFrameWidth == 720 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 704 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 640 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x1A, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x1A, 1 ); }
				if( pVideo->m_nDstFrameWidth == 360 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 352 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 320 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x34, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x34, 1 ); }
			}
			if( pVideo->m_nDstFrameHeight == 576 || pVideo->m_nDstFrameHeight == 288 ) {

				if( pVideo->m_nDstFrameWidth == 720 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 704 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 640 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x17, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x17, 1 ); }
				if( pVideo->m_nDstFrameWidth == 360 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 352 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 ); }
				if( pVideo->m_nDstFrameWidth == 320 ) { TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x0000023C, 0x2B, 1 ); TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 ); TW6805_SetRegister( pDevice, 0x000003FC, 0x2B, 1 ); }
			}
		}
		//

		if( pVideo->m_nSrcFrameHeight > 288 ) { pDmaBaseCommonBuffer[ 7 ] = pDmaBaseCommonPhysicalStartBuffer; } //

		if( pVideo->m_nSrcFrameWidth == 720 || pVideo->m_nSrcFrameWidth == 704 || pVideo->m_nSrcFrameWidth == 640 ) {

			TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 );

			TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 );
		}
		if( pVideo->m_nSrcFrameWidth == 360 || pVideo->m_nSrcFrameWidth == 352 || pVideo->m_nSrcFrameWidth == 320 ) {

			TW6805_SetRegister( pDevice, 0x00000238, 0x12, 1 );

			TW6805_SetRegister( pDevice, 0x000003F8, 0x12, 1 );
		}
		if( pVideo->m_nSrcFrameHeight== 480 || pVideo->m_nSrcFrameHeight == 240 ) {

			TW6805_SetRegister( pDevice, 0x00000228, 0x0A, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E8, 0x0A, 1 );
			
			TW6805_SetRegister( pDevice, 0x00000220, 0x14, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E0, 0x14, 1 );
			
			TW6805_SetRegister( pDevice, 0x00000224, 0xF0, 1 );
			
			TW6805_SetRegister( pDevice, 0x0000021C, 0x02, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E4, 0xF0, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003DC, 0x02, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002CC, 0x85, 1 );
		}
		if( pVideo->m_nSrcFrameHeight== 576 || pVideo->m_nSrcFrameHeight == 288 ) {

			TW6805_SetRegister( pDevice, 0x00000228, 0x07, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E8, 0x07, 1 );
			
			TW6805_SetRegister( pDevice, 0x00000220, 0x18, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E0, 0x18, 1 );
			
			TW6805_SetRegister( pDevice, 0x00000224, 0x20, 1 );
			
			TW6805_SetRegister( pDevice, 0x0000021C, 0x12, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003E4, 0x20, 1 );
			
			TW6805_SetRegister( pDevice, 0x000003DC, 0x12, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002CC, 0xC5, 1 );
		}
		if( (pVideo->m_nLockStatusMask_Spliter == 0x00000001) ||
			
			(pVideo->m_nLockStatusMask_Spliter == 0x00000002) ||
			
			(pVideo->m_nLockStatusMask_Spliter == 0x00000004) ||
			
			(pVideo->m_nLockStatusMask_Spliter == 0x00000008) ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x02, 1 );

			TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002BC, 0xE6, 1 );
		}
		else {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x0C, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 );
			
			TW6805_SetRegister( pDevice, 0x000002BC, 0x06, 1 );
		}
		if( pVideo->m_pSwitchTable_Spliter[ 0 ] == 0x00 ) { TW6805_SetRegister( pDevice, 0x00000208, 0x40, 1 ); }

		if( pVideo->m_pSwitchTable_Spliter[ 0 ] == 0x01 ) { TW6805_SetRegister( pDevice, 0x00000208, 0x44, 1 ); }

		if( pVideo->m_pSwitchTable_Spliter[ 0 ] == 0x02 ) { TW6805_SetRegister( pDevice, 0x00000208, 0x48, 1 ); }

		if( pVideo->m_pSwitchTable_Spliter[ 0 ] == 0x03 ) { TW6805_SetRegister( pDevice, 0x00000208, 0x4C, 1 ); }

		pVideo->m_nLockStatus = pVideo->m_nLockStatusMask_Spliter;

		pVideo->m_nLockAccumulatorCounter_Spliter = 16;

		pVideo->m_nDropDecumulatorCounter_Spliter = 0;

		pVideo->m_nSwitchNumber_Spliter = 0;

		pVideo->m_nFrameCopyPolarity = 0;

	}

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) {
	
		for( i = 0 ; i < 128 ; i++ ) {

			dma_addr_t n_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];

			ULONG n_copy_bytes = pAudio->m_nFrameSize;

			n_physical_address += (1920) * (i % 4);

			pDmaBaseCommonPhysicalBuffer += 24;

			pDmaBaseCommonBuffer[ 0 ] = 0xE0000000;
			
			pDmaBaseCommonBuffer[ 1 ] = 0x00000000;
			
			pDmaBaseCommonBuffer[ 2 ] = 0x90000000 + n_copy_bytes;
			
			pDmaBaseCommonBuffer[ 3 ] = n_physical_address;
			
			pDmaBaseCommonBuffer[ 4 ] = (i % 4) ? 0xBB000000 : 0xBA000000;
			
			pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer;

			pDmaBaseCommonBuffer += 24 >> 2;
		}
		pDmaBaseCommonBuffer[ -1 ] = pDmaBaseCommonPhysicalStartBuffer;
	}

	pDevice->m_hInterruptAccessLock = FALSE;

	// ...
	//
	TW6805_SetRegister( pDevice, 0x0000001C, 0xFFFFFFFF, 4 );

	// ...
	//
	TW6805_SetRegister( pDevice, 0x00000020, 0x00000022, 4 );

	// ...
	// 
	R00000000 = TW6805_GetRegister( pDevice, 0x00000000, 4 ) & 0xFFFFFFFC;

	TW6805_SetRegister( pDevice, 0x00000000, R00000000 | 0x00000002, 4 );

	TW6805_SetRegister( pDevice, 0x00000000, R00000000 | 0x00000003, 4 );

	pDevice->m_hInterruptAccessLock = TRUE;

	return TRUE;
}

BOOLEAN TW6805_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG R00000000 = 0x00000000;

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference = 0;
	}
	else if( pDevice->m_nAnalogCaptureStreamPowerReference == 0 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference = 0;

		return TRUE;
	}
	else { 

		pDevice->m_nAnalogCaptureStreamPowerReference--;

		return TRUE;
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwAnalogComponentsUnInitialize()\n", pDevice->m_nKsDeviceNumber );

	// ...
	//
	TW6805_SetRegister(pDevice, 0x00000020, 0x00000000, 4 );

	pDevice->m_hInterruptAccessLock = FALSE;

	// ...
	//
	R00000000 = TW6805_GetRegister( pDevice, 0x00000000, 4 ) & 0xFFFFFFFC;

	TW6805_SetRegister( pDevice, 0x00000000, R00000000 | 0x00000002, 4 );

	TW6805_SetRegister( pDevice, 0x00000000, R00000000 | 0x00000000, 4 );


	return TRUE;
}

BOOLEAN TW6805_HwInitialize( CDevice * pDevice )
{
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		ULONG i = pDevice->m_nKsDeviceNumber;

		BYTE CFGD = 0xFF; pci_write_config_byte( pDevice->m_pKsDevice, 0x0000000D, CFGD );

		LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwInitialize( %08lX )\n", pDevice->m_nKsDeviceNumber, TW6805_GetRegister( pDevice, 0x00000000, 4 ) );

		TW6805_SetRegister( pDevice, 0x00000000, 0x00005040, 4 );
		
		TW6805_SetRegister( pDevice, 0x0000001C, 0xFFFFFFFF, 4 );
		
		TW6805_SetRegister( pDevice, 0x0000001C, 0x00000000, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000020, 0x00000000, 4 );

		if( pDevice->bcdDevice == 0x6804 && //N16 DVI
			
			pDevice->iManufacturer == 0x97 &&

			pDevice->iProduct == 0x17 ) {

			TW6805_SetRegister( pDevice, 0x00000024, 0x0000084C, 4 );
		}

		TW6805_SetRegister( pDevice, 0x00000028, g_n_gpio_direction[ i ], 4 );

		TW6805_SetRegister( pDevice, 0x00000100, g_n_gpio_data[ i ], 4 );

		TW6805_SetRegister( pDevice, 0x00000218, 0x00, 1 );

		TW6805_SetRegister( pDevice, 0x00000208, 0x40, 1 );
		
		TW6805_SetRegister( pDevice, 0x0000020C, 0x04, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000210, 0x00, 1 );
		
		TW6805_SetRegister( pDevice, 0x000003F0, 0x01, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000228, 0x0F, 1 );
	
		TW6805_SetRegister( pDevice, 0x000003E8, 0x0F, 1 );
	
		TW6805_SetRegister( pDevice, 0x0000022C, 0xD0, 1 );
	
		TW6805_SetRegister( pDevice, 0x000003EC, 0xD0, 1 );

		TW6805_SetRegister( pDevice, 0x00000220, 0x12, 1 );

		TW6805_SetRegister( pDevice, 0x000003E0, 0x12, 1 );

		TW6805_SetRegister( pDevice, 0x00000224, 0xF0, 1 );
		
		TW6805_SetRegister( pDevice, 0x0000021C, 0x02, 1 );
		
		TW6805_SetRegister( pDevice, 0x000003E4, 0xF0, 1 );
		
		TW6805_SetRegister( pDevice, 0x000003DC, 0x02, 1 );
		
		TW6805_SetRegister( pDevice, 0x0000023C, 0x00, 1 );
	
		TW6805_SetRegister( pDevice, 0x000003FC, 0x00, 1 );
	
		TW6805_SetRegister( pDevice, 0x00000234, 0x00, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000238, 0x11, 1 );
		
		TW6805_SetRegister( pDevice, 0x000003F4, 0x00, 1 );
		
		TW6805_SetRegister( pDevice, 0x000003F8, 0x11, 1 ); 
		
		TW6805_SetRegister( pDevice, 0x00000240, 0x00, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000244, 0x64, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000248, 0x51, 1 );
		
		TW6805_SetRegister( pDevice, 0x0000024C, 0x80, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000250, 0x80, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000254, 0x00, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000268, 0x0A, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000270, 0x00, 1 );
		
		TW6805_SetRegister( pDevice, 0x000002A0, 0x02, 1 );
		
		TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 );
		
		TW6805_SetRegister( pDevice, 0x000002BC, 0xE6, 1 ); 
		
		TW6805_SetRegister( pDevice, 0x000002D4, 0x20, 1 );
		
		TW6805_SetRegister( pDevice, 0x00000218, 0x80, 1 );
	}
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) {

		DWORD R00000078 = 0x00000000;

		BYTE CFGD = 0xFF; pci_write_config_byte( pDevice->m_pKsDevice, 0x0000000D, CFGD );

		LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwInitialize() R00000078( %08lX )\n", pDevice->m_nKsDeviceNumber, TW6805_GetRegister( pDevice, 0x00000078, 4 ) );

		TW6805_SetRegister( pDevice, 0x00000000, 0x00000000, 4 );
		
		TW6805_SetRegister( pDevice, 0x0000001C, 0xFFFFFFFF, 4 );
		
		TW6805_SetRegister( pDevice, 0x0000001C, 0x00000000, 4  );
		
		TW6805_SetRegister( pDevice, 0x00000020, 0x00000000, 4  );

		TW6805_SetRegister( pDevice, 0x00000030, 0x00000000, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000010, 0x00280000, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000014, 0x00000000, 4 ); 
		
		TW6805_SetRegister( pDevice, 0x00000018, 0x00000011, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000034, 0x00000000, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000038, 0x0007F19A, 4 );
		
		TW6805_SetRegister( pDevice, 0x0000003C, 0x0009B583, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000040, 0x00000048, 4 );
		
		TW6805_SetRegister( pDevice, 0x00000048, 0x00010820, 4 ); 

		R00000078 = TW6805_GetRegister( pDevice, 0x00000078, 4 );


		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) )
		{
			if( R00000078 & 0x00000001 ) {

				TW6805_SetRegister( pDevice, 0x00000038, 0x005F5338, 4 );// [2008.04.22] [HUENGPEI] 0x00000048 -> 0x00000048 (PCI.BUS.CLOCK) (48000 HZ กั 16 BIT)

				TW6805_SetRegister( pDevice, 0x00000040, 0x00000048, 4 );

				TW6805_SetRegister( pDevice, 0x00000030, 0x00000001, 4 );
			}
			else {

				TW6805_SetRegister( pDevice, 0x0000003C, 0x00748224, 4 );// [2008.04.22] [HUENGPEI] 0x00000048 -> 0x00000049 (CRISTAL.CLOCK) (48000 HZ กั 16 BIT)

				TW6805_SetRegister( pDevice, 0x00000040, 0x00000049, 4 );

				TW6805_SetRegister( pDevice, 0x00000030, 0x00000001, 4 );
			}
		}
		else
		{
			TW6805_SetRegister( pDevice, 0x0000003C, 0x00748224, 4 );// [2008.04.22] [HUENGPEI] 0x00000048 -> 0x00000049 (CRISTAL.CLOCK) (48000 HZ กั 16 BIT)
			TW6805_SetRegister( pDevice, 0x00000040, 0x00000049, 4 ); 
			TW6805_SetRegister( pDevice, 0x00000030, 0x00000001, 4 );
		}
	}
/*
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		ULONGLONG delay = 10;

		struct timeval ts; do_gettimeofday( &ts );

		{	BYTE codes[ 4 ] = { ((ts.tv_usec / 1) % 0xFF), ((ts.tv_usec / 10) % 0xFF), ((ts.tv_usec / 100) % 0xFF), ((ts.tv_usec / 1000) % 0xFF) };

			BYTE keys[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };
	
			I2C_START( pDevice, delay ); I2C_WRITE_BYTE( pDevice, 0xA2, delay ); I2C_WRITE_BYTE( pDevice, 0x13, delay ); I2C_WRITE_BYTE( pDevice, codes[ 0 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 1 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 2 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 3 ], delay ); I2C_STOP( pDevice, delay );
	
			schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	
			I2C_START( pDevice, delay ); I2C_WRITE_BYTE( pDevice, 0xA3, delay ); keys[ 0 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 1 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 2 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 3 ] = I2C_READ_BYTE( pDevice, 0, delay ); I2C_STOP( pDevice, delay );
	
			if( (*((DWORD *)(keys)) != 0x00000000) && (*((DWORD *)(keys)) != 0xFFFFFFFF) ) {
	
				g_copy_protect_unlock_boradsA[ pDevice->m_pKsDevice->bus->number ] = 1; 
				
				g_copy_protect_unlock_boradsB[ pDevice->m_pKsDevice->bus->number ] = 1;
			}
		}
	}
	// shawn 2010.09.23
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		ULONGLONG delay = 12;

		struct timeval ts; do_gettimeofday( &ts );

		{	BYTE codes[ 4 ] = { ((ts.tv_usec / 1) % 0xFF), ((ts.tv_usec / 10) % 0xFF), ((ts.tv_usec / 100) % 0xFF), ((ts.tv_usec / 1000) % 0xFF) };

			BYTE keys[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };
	
			I2C_START( pDevice, delay ); I2C_WRITE_BYTE( pDevice, 0xA2, delay ); I2C_WRITE_BYTE( pDevice, 0x13, delay ); I2C_WRITE_BYTE( pDevice, codes[ 0 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 1 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 2 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 3 ], delay ); I2C_STOP( pDevice, delay );
	
			schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	
			I2C_START( pDevice, delay ); I2C_WRITE_BYTE( pDevice, 0xA3, delay ); keys[ 0 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 1 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 2 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 3 ] = I2C_READ_BYTE( pDevice, 0, delay ); I2C_STOP( pDevice, delay );
	
			if( (*((DWORD *)(keys)) != 0x00000000) && (*((DWORD *)(keys)) != 0xFFFFFFFF) ) {
	
				g_copy_protect_unlock_boradsA[ pDevice->m_pKsDevice->bus->number ] = 1; 
				
				g_copy_protect_unlock_boradsB[ pDevice->m_pKsDevice->bus->number ] = 1;

			}
		}
	}
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		ULONGLONG delay = 15;

		struct timeval ts; do_gettimeofday( &ts );

		{	BYTE codes[ 4 ] = { ((ts.tv_usec / 1) % 0xFF), ((ts.tv_usec / 10) % 0xFF), ((ts.tv_usec / 100) % 0xFF), ((ts.tv_usec / 1000) % 0xFF) };

			BYTE keys[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };
	
			I2C_START( pDevice, delay ); I2C_WRITE_BYTE( pDevice, 0xA2, delay ); I2C_WRITE_BYTE( pDevice, 0x13, delay ); I2C_WRITE_BYTE( pDevice, codes[ 0 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 1 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 2 ], delay ); I2C_WRITE_BYTE( pDevice, codes[ 3 ], delay ); I2C_STOP( pDevice, delay );
	
			schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	
			I2C_START( pDevice, delay ); I2C_WRITE_BYTE( pDevice, 0xA3, delay ); keys[ 0 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 1 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 2 ] = I2C_READ_BYTE( pDevice, 0, delay ); keys[ 3 ] = I2C_READ_BYTE( pDevice, 0, delay ); I2C_STOP( pDevice, delay );
	
			if( (*((DWORD *)(keys)) != 0x00000000) && (*((DWORD *)(keys)) != 0xFFFFFFFF) ) {

				g_copy_protect_unlock_boradsA[ pDevice->m_pKsDevice->bus->number ] = 1; 
				
				g_copy_protect_unlock_boradsB[ pDevice->m_pKsDevice->bus->number ] = 1;

			}
		}
	}
*/
	//
	//shawn 2010.08.11
	//
	// 
	if( pDevice->m_nKsDeviceNumber == 0 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

				BYTE R11 = (BYTE)(pDevice->m_nCustomSoftwareWatchdogDurationProperty);
				
				I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); I2C_WRITE_BYTE( pDevice, 0x11, 125 ); I2C_WRITE_BYTE( pDevice,  R11, 125 ); I2C_STOP( pDevice, 125 ); //

				I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); I2C_WRITE_BYTE( pDevice, 0x12, 125 ); I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); I2C_STOP( pDevice, 125 );

				I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); I2C_WRITE_BYTE( pDevice, 0x10, 125 ); I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); I2C_STOP( pDevice, 125 );
		}

		TW6805_StartControlPanelAnalysisThread( pDevice );
	}

	//
	return TW6805_PowerDownPeripherals( pDevice );
}

BOOLEAN TW6805_PowerDownPeripherals( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_PowerDownPeripherals()\n", pDevice->m_nKsDeviceNumber );

	return TRUE;
}

BOOLEAN TW6805_HwUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwUnInitialize()\n", pDevice->m_nKsDeviceNumber );

	TW6805_StopControlPanelAnalysisThread( pDevice );

	return TRUE;
}
ULONGLONG ch0_previous_times = 0;
ULONGLONG ch1_previous_times = 0;
ULONGLONG ch2_previous_times = 0;
ULONGLONG ch3_previous_times = 0;
ULONGLONG ch4_previous_times = 0;
ULONGLONG ch5_previous_times = 0;
ULONGLONG ch6_previous_times = 0;
ULONGLONG ch7_previous_times = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////
//custom fps
DWORD TW6805_CALCULATE_FPS( CDevice * pDevice, ULONG fps )
{
	ULONG frames = 0;

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

		frames = 30;
	}

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {

		frames = 25;
	}
	ULONG R = 0x00000000;

	ULONG counts = 0;

	ULONG i = 0;

	for( i = 0 ; i < frames ; i++ ) {

		counts += fps;

		if( counts >= frames ) {

			counts -= frames;

			R |= (1 << i);
		}
	}
	return R;
}


BOOLEAN TW6805_HwProcessAnalogPCIVideoPacket( CDevice *pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	CVideo * pVideo = pVideos[ 0 ];

	CVideoBuffer * pVideoBuffer = pVideoBuffers[ 0 ];

	dma_addr_t pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];

	dma_addr_t pDmaBaseCommonPhysicalJumpBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];

	DWORD *    pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);

	DWORD *    pDmaBaseCommonJumpBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);

	BOOLEAN    is_complete = FALSE;

	BOOLEAN    is_copy = FALSE;

	BOOLEAN    is_reset = FALSE;

	BOOLEAN    is_even = FALSE;

	BOOLEAN    is_fast_switching = FALSE;

	ULONG	   n_switch_state_machine = 0x00;

	BYTE       SWITCH_CHANNEL_TABLE[ 12 ] = { 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03 };

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwProcessAnalogPCIVideoPacket( %08X:%08X )\n", pDevice->m_nKsDeviceNumber, status, mask );

	if( pVideo == NULL ) { return TRUE; }

	memcpy( SWITCH_CHANNEL_TABLE, pVideo->m_pSwitchTable_Spliter, 12 );

	if( ((pVideo->m_nLockStatus & 0x0000000F) == (pVideo->m_nLockStatusMask_Spliter & 0x0000000F)) &&
		
		((pVideo->m_nLockAccumulatorCounter_Spliter >= 16)) &&

		((pVideo->m_nSwitchSpeed_Spliter >= 1)) ) {

		is_fast_switching = TRUE;
	}
	if( is_fast_switching ) {

		if( pVideo->m_nSrcFrameHeight > 288 ) { //

			if( (pVideo->m_nLockStatusMask_Spliter == 0x00000001) || (pVideo->m_nLockStatusMask_Spliter == 0x00000002) || (pVideo->m_nLockStatusMask_Spliter == 0x00000004) || (pVideo->m_nLockStatusMask_Spliter == 0x00000008) ) {

				n_switch_state_machine = 0;
			}
			else {

				n_switch_state_machine = 1;
			}
		}
		else { //

			if( (pVideo->m_nLockStatusMask_Spliter == 0x00000001) || (pVideo->m_nLockStatusMask_Spliter == 0x00000002) || (pVideo->m_nLockStatusMask_Spliter == 0x00000004) || (pVideo->m_nLockStatusMask_Spliter == 0x00000008) ) {

				n_switch_state_machine = 4;	
			}
			else if( (pVideo->m_nSwitchSpeed_Spliter == 1) ) { 
				
				n_switch_state_machine = 5;
			}
			else {

				n_switch_state_machine = 6;
			}
		}
	}
	else {

		if( pVideo->m_nSrcFrameHeight > 288 ) { //

			if( (pVideo->m_nLockStatusMask_Spliter == 0x00000001) || (pVideo->m_nLockStatusMask_Spliter == 0x00000002) || (pVideo->m_nLockStatusMask_Spliter == 0x00000004) || (pVideo->m_nLockStatusMask_Spliter == 0x00000008) ) {

				n_switch_state_machine = 2;
			}
			else {

				n_switch_state_machine = 3;
			}
		}
		else { //

			if( (pVideo->m_nLockStatusMask_Spliter == 0x00000001) || (pVideo->m_nLockStatusMask_Spliter == 0x00000002) || (pVideo->m_nLockStatusMask_Spliter == 0x00000004) || (pVideo->m_nLockStatusMask_Spliter == 0x00000008) ) {

				n_switch_state_machine = 7;
			}
			else {

				n_switch_state_machine = 8;
			}
		}
	}
	if( n_switch_state_machine == 0 ||

		n_switch_state_machine == 2 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) != 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x02, 1 );
						
			TW6805_SetRegister( pDevice, 0x000002BC, 0xE6, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 1; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 1; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 6 + 24 *  7); pDmaBaseCommonStartBuffer += (3600 * 6 + 24 *  7) >> 2; is_copy = 1; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 7 + 24 *  8); pDmaBaseCommonStartBuffer += (3600 * 7 + 24 *  8) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000600 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 8 + 24 *  9); pDmaBaseCommonStartBuffer += (3600 * 8 + 24 *  9) >> 2; is_copy = 1; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000700 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 9 + 24 * 10); pDmaBaseCommonStartBuffer += (3600 * 9 + 24 * 10) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000800 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 0; is_reset = 1; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000900 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }

		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 4 + 24 *  4); pDmaBaseCommonJumpBuffer  += (3600 * 4 + 24 *  4) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 6 + 24 *  6); pDmaBaseCommonJumpBuffer  += (3600 * 6 + 24 *  6) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 7 + 24 *  7); pDmaBaseCommonJumpBuffer  += (3600 * 7 + 24 *  7) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 8 + 24 *  8); pDmaBaseCommonJumpBuffer  += (3600 * 8 + 24 *  8) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000600 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 9 + 24 *  9); pDmaBaseCommonJumpBuffer  += (3600 * 9 + 24 *  9) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000700 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); } // //
		if( (status & 0x00000F00) == 0x00000800 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000900 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
	}
	if( n_switch_state_machine == 1 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) == 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x0C, 1 );

			TW6805_SetRegister( pDevice, 0x000002BC, 0x06, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 1; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 0; is_reset = 1; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000600 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 6 + 24 *  7); pDmaBaseCommonStartBuffer += (3600 * 6 + 24 *  7) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000700 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 7 + 24 *  8); pDmaBaseCommonStartBuffer += (3600 * 7 + 24 *  8) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000800 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 8 + 24 *  9); pDmaBaseCommonStartBuffer += (3600 * 8 + 24 *  9) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000900 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 9 + 24 * 10); pDmaBaseCommonStartBuffer += (3600 * 9 + 24 * 10) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 0; }

		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 4 + 24 *  4); pDmaBaseCommonJumpBuffer  += (3600 * 4 + 24 *  4) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); } // //
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000600 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 7 + 24 *  7); pDmaBaseCommonJumpBuffer  += (3600 * 7 + 24 *  7) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); } 
		if( (status & 0x00000F00) == 0x00000700 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 8 + 24 *  8); pDmaBaseCommonJumpBuffer  += (3600 * 8 + 24 *  8) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000800 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 9 + 24 *  9); pDmaBaseCommonJumpBuffer  += (3600 * 9 + 24 *  9) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000900 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
	}
	if( n_switch_state_machine == 3 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) == 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x0C, 1 );

			TW6805_SetRegister( pDevice, 0x000002BC, 0x06, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 1; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 6 + 24 *  7); pDmaBaseCommonStartBuffer += (3600 * 6 + 24 *  7) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 7 + 24 *  8); pDmaBaseCommonStartBuffer += (3600 * 7 + 24 *  8) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 8 + 24 *  9); pDmaBaseCommonStartBuffer += (3600 * 8 + 24 *  9) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 9 + 24 * 10); pDmaBaseCommonStartBuffer += (3600 * 9 + 24 * 10) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 0; is_reset = 1; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000600 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000700 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 0; }
		if( (status & 0x00000F00) == 0x00000800 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 1; }
		if( (status & 0x00000F00) == 0x00000900 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; is_even = 0; }

		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 6 + 24 *  6); pDmaBaseCommonJumpBuffer  += (3600 * 6 + 24 *  6) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 7 + 24 *  7); pDmaBaseCommonJumpBuffer  += (3600 * 7 + 24 *  7) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 8 + 24 *  8); pDmaBaseCommonJumpBuffer  += (3600 * 8 + 24 *  8) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 9 + 24 *  9); pDmaBaseCommonJumpBuffer  += (3600 * 9 + 24 *  9) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); } //
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000600 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000700 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000800 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 4 + 24 *  4); pDmaBaseCommonJumpBuffer  += (3600 * 4 + 24 *  4) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000900 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
	}
	if( n_switch_state_machine == 4 ||

		n_switch_state_machine == 7 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) != 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x02, 1 );
						
			TW6805_SetRegister( pDevice, 0x000002BC, 0xE6, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 1; is_reset = 1; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 4 + 24 *  4); pDmaBaseCommonJumpBuffer  += (3600 * 4 + 24 *  4) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); } // //
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xA5, 1 ); }
	}
	if( n_switch_state_machine == 5 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) == 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x0C, 1 );

			TW6805_SetRegister( pDevice, 0x000002BC, 0x06, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 1; is_reset = 1; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 4 + 24 *  4); pDmaBaseCommonJumpBuffer  += (3600 * 4 + 24 *  4) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); } // //
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
	}
	if( n_switch_state_machine == 6 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) == 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x0C, 1 );

			TW6805_SetRegister( pDevice, 0x000002BC, 0x06, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 0; is_reset = 1; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 0; is_complete = 1; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }

		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 4 + 24 *  4); pDmaBaseCommonJumpBuffer  += (3600 * 4 + 24 *  4) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); } // //
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0xFD, 1 ); }
	}
	if( n_switch_state_machine == 8 ) {

		if(	TW6805_GetRegister( pDevice, 0x000002B8, 1 ) == 0xA5 ) {

			TW6805_SetRegister( pDevice, 0x000002A0, 0x0C, 1 );

			TW6805_SetRegister( pDevice, 0x000002BC, 0x06, 1 );
		}
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 0 + 24 *  1); pDmaBaseCommonStartBuffer += (3600 * 0 + 24 *  1) >> 2; is_copy = 1; is_complete = 1; is_reset = 1; }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 1 + 24 *  2); pDmaBaseCommonStartBuffer += (3600 * 1 + 24 *  2) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 2 + 24 *  3); pDmaBaseCommonStartBuffer += (3600 * 2 + 24 *  3) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 3 + 24 *  4); pDmaBaseCommonStartBuffer += (3600 * 3 + 24 *  4) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 4 + 24 *  5); pDmaBaseCommonStartBuffer += (3600 * 4 + 24 *  5) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalStartBuffer += (3600 * 5 + 24 *  6); pDmaBaseCommonStartBuffer += (3600 * 5 + 24 *  6) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		
		if( (status & 0x00000F00) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 1 + 24 *  1); pDmaBaseCommonJumpBuffer  += (3600 * 1 + 24 *  1) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000100 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 2 + 24 *  2); pDmaBaseCommonJumpBuffer  += (3600 * 2 + 24 *  2) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000200 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 3 + 24 *  3); pDmaBaseCommonJumpBuffer  += (3600 * 3 + 24 *  3) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000300 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); } // //
		if( (status & 0x00000F00) == 0x00000400 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 5 + 24 *  5); pDmaBaseCommonJumpBuffer  += (3600 * 5 + 24 *  5) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
		if( (status & 0x00000F00) == 0x00000500 ) { pDmaBaseCommonPhysicalJumpBuffer  += (3600 * 0 + 24 *  0); pDmaBaseCommonJumpBuffer  += (3600 * 0 + 24 *  0) >> 2; TW6805_SetRegister( pDevice, 0x000002B8, 0x3D, 1 ); }
	}

//skip frame here
	// CALCULATE OUTPUT FPS 
	//
	ULONG n_video_encoder_frame_fps_mask = 0;
	ULONG n_video_encoder_frame_fps_mask_length = 0;

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

		n_video_encoder_frame_fps_mask = TW6805_CALCULATE_FPS( pDevice, pVideo->m_nFrameRate/1000 );

		n_video_encoder_frame_fps_mask_length = 30;
	}

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {

		n_video_encoder_frame_fps_mask = TW6805_CALCULATE_FPS( pDevice, pVideo->m_nFrameRate/1000 );

		n_video_encoder_frame_fps_mask_length = 25;
	}

	ULONG is_skip = FALSE;

	ULONG bit = (ULONG)(pVideo->m_video_preview_frame_number ) % (n_video_encoder_frame_fps_mask_length);

	is_skip = (((1 << bit) & (n_video_encoder_frame_fps_mask)) == 0) ? TRUE : FALSE;

	//error, audio interrupt will also increase this parameter
	pVideo->m_video_preview_frame_number++;

//	if( is_skip ) 
	if( 0 ) 
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] skip frame\n", pDevice->m_nKsDeviceNumber );
	}
	else
	{
		if( is_complete ) {
			
			ULONG nLockStatus = pVideo->m_nLockStatus;

			DWORD R00000204 = TW6805_GetRegister( pDevice, 0x00000204, 4 );

			ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0;
			
			ULONG j = SWITCH_CHANNEL_TABLE[ pVideo->m_nSwitchNumber_Spliter ] % 4;

			switch( j ) {
			
			case 0: if( (R00000204 & 0x000000C0) != 0x00000040 ) { pVideo->m_nLockStatus &= 0xFFFFFFFE; } else { pVideo->m_nLockStatus |= 0x00000001; } break;
			
			case 1: if( (R00000204 & 0x000000C0) != 0x00000040 ) { pVideo->m_nLockStatus &= 0xFFFFFFFD; } else { pVideo->m_nLockStatus |= 0x00000002; } break;
			
			case 2: if( (R00000204 & 0x000000C0) != 0x00000040 ) { pVideo->m_nLockStatus &= 0xFFFFFFFB; } else { pVideo->m_nLockStatus |= 0x00000004; } break;
			
			case 3: if( (R00000204 & 0x000000C0) != 0x00000040 ) { pVideo->m_nLockStatus &= 0xFFFFFFF7; } else { pVideo->m_nLockStatus |= 0x00000008; } break;
			}
			if( (pVideo->m_nLockStatusMask_Spliter == nLockStatus) &&
				
				(pVideo->m_nLockStatusMask_Spliter != pVideo->m_nLockStatus) ) { 
				
				 if( (pVideo->m_nLockStatusMask_Spliter != 0x00000001) && 
					 
					 (pVideo->m_nLockStatusMask_Spliter != 0x00000002) && 
					 
					 (pVideo->m_nLockStatusMask_Spliter != 0x00000004) && 
					 
					 (pVideo->m_nLockStatusMask_Spliter != 0x00000008) ) {

					pVideo->m_nDropDecumulatorCounter_Spliter = 3; //
				 }
				 LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] SPEED FAST -> SLOW\n", pDevice->m_nKsDeviceNumber ); //
			}
			if( (pVideo->m_nLockStatusMask_Spliter != pVideo->m_nLockStatus) ) { 
				
				 pVideo->m_nLockAccumulatorCounter_Spliter = 0; 
			} 
			else { 
				
				 pVideo->m_nLockAccumulatorCounter_Spliter++; 
			}
			if( (pVideo->m_nLockAccumulatorCounter_Spliter == 16) ) { 
				
				 if( (pVideo->m_nLockStatusMask_Spliter != 0x00000001) && 
					 
					 (pVideo->m_nLockStatusMask_Spliter != 0x00000002) && 
					 
					 (pVideo->m_nLockStatusMask_Spliter != 0x00000004) && 
					 
					 (pVideo->m_nLockStatusMask_Spliter != 0x00000008) ) {

					pVideo->m_nDropDecumulatorCounter_Spliter = 3; //
				 }
				 LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] SPEED SLOW -> FAST\n", pDevice->m_nKsDeviceNumber ); //
			}
			
			pVideo->m_nSwitchNumber_Spliter +=  1;

			pVideo->m_nSwitchNumber_Spliter %= 12;

			j = SWITCH_CHANNEL_TABLE[ pVideo->m_nSwitchNumber_Spliter ] % 4;

			//
			//
			switch( j ) {
			
			case 0: TW6805_SetRegister( pDevice, 0x00000208, 0x40, 1 ); break;
			
			case 1: TW6805_SetRegister( pDevice, 0x00000208, 0x44, 1 ); break;
			
			case 2: TW6805_SetRegister( pDevice, 0x00000208, 0x48, 1 ); break;
			
			case 3: TW6805_SetRegister( pDevice, 0x00000208, 0x4C, 1 ); break;
			}
			//
			//
			g_n_analog_decoder_brightness[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_contrast[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_saturation[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_hue[ i ][ j ] |= 0x80000000;

			g_n_analog_decoder_sharpness[ i ][ j ] |= 0x80000000;

			TW6805_SetAnalogVideoDecoderProcAmpProperties( pDevice, pVideo, j );
		}

		//
		//
		if( pVideo->m_nSrcFrameHeight > 288 ) {

			if( is_copy == 1 && is_complete == 0 ) { //
				
				pVideo->m_nFrameCopyPolarity = (pVideoBuffer) ? (0x00000001) : (0x00000000);
			}
			if( is_copy == 1 && is_complete == 1 ) { //
			
				if( pVideo->m_nFrameCopyPolarity == 0x00000001 ) {

					if( pVideoBuffer ) { pVideoBuffer->m_sKsBuffer.input = 0x00000000; } //
				}
				else {

					if( pVideoBuffer ) { pVideoBuffer->m_sKsBuffer.input = 0x80000000; } //
				}
				pVideo->m_nFrameCopyPolarity = 0x00000000;
			}
		}
		{  *pDmaBaseCommonStartBuffer++ = 0xE0000000;

		   *pDmaBaseCommonStartBuffer++ = 0x00000000;
		}
		if( pVideoBuffer && is_copy ) {

#ifdef TW6805_64bit

			dma_addr_t physical_address = pDevice->m_TW6816_video_buffer_physical_address[ pDevice->n_video_buffer_count ];

			ULONG byte_count = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 2;
			
			ULONG mapping_count = 1;
#else
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			struct videobuf_dmabuf * pKsDMABuffer = videobuf_to_dma( &pVideoBuffer->m_sKsBuffer );

			#else

			struct videobuf_dmabuf * pKsDMABuffer = &(pVideoBuffer->m_sKsBuffer.dma);

			#endif

			struct scatterlist * p_sglist = (struct scatterlist *)(pKsDMABuffer->sglist);
			
			dma_addr_t physical_address = sg_dma_address( p_sglist );

			ULONG byte_count = sg_dma_len( p_sglist );

			ULONG mapping_count = pKsDMABuffer->sglen;

#endif

			ULONG pitch = pVideo->m_nDstFrameWidth;

			ULONG lines = 0;

			ULONG count = 0;

			ULONG i = 0;

			if( pVideo->m_nSrcFrameHeight == 240 ) { lines = 240; }

			if( pVideo->m_nSrcFrameHeight == 480 ) { lines = 240; }

			if( pVideo->m_nSrcFrameHeight == 288 ) { lines = 288; }

			if( pVideo->m_nSrcFrameHeight == 576 ) { lines = 288; }

			{	

				pVideoBuffer->m_sKsBuffer.input &= 0xFFFFFFFC;

				pVideoBuffer->m_sKsBuffer.input |= SWITCH_CHANNEL_TABLE[ pVideo->m_nSwitchNumber_Spliter ] % 4; //

			//	if( (pVideoBuffer->m_sKsBuffer.input & 0x00000003) != (0x00) )  {
			//
			//	   p_is_drops[ 0 ] = TRUE;
			//	}
			}
			for( i = 0 ; i < lines ; i++ ) {

				ULONG remains = 0;

				ULONG start = 0;

				//
				//
				if( pVideo->m_nSrcFrameHeight > 288 ) {

					if( (i == 0) && (is_even == 1) ) { 

						remains = (pitch) << 1;
						
						while( remains ) {

							if( byte_count > remains ) {

								physical_address += remains;

								byte_count -= remains;

								remains -= remains;
							}
							else {

								remains -= byte_count;

								#ifndef TW6805_64bit
								p_sglist++;
								#endif

								count++;

								if( count < mapping_count ) {

									#ifdef TW6805_64bit
									
									physical_address += 0;

									byte_count = 0;
									
									#else
									physical_address = sg_dma_address( p_sglist );

									byte_count = sg_dma_len( p_sglist );
									#endif
								}
								else {

									physical_address = 0;

									byte_count = 0;

									goto LOOP_END_LINES;
								}
							
							}
						}
					}
				}
				
				//
				//
				remains = (pVideo->m_nSrcFrameWidth) << 1;

				start = 0;

				while( remains ) {

					if( byte_count > remains ) {
						//shawn 2010.10.04
						if( pDevice->m_nCustomAnalogVideoScaleOutputProperty == 0 ) {

							switch( pVideo->m_nSrcFrameWidth ) {
							
							case 720: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;
							
							case 360: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;
							
							case 704: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90010000 : 0xA0010000); break;
							
							case 352: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90008000 : 0xA0008000); break;
							
							case 640: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90050000 : 0xA0050000); break;
							
							case 320: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90028000 : 0xA0028000); break;
							}
						}
						if( pDevice->m_nCustomAnalogVideoScaleOutputProperty == 1 ) {

							if( lines == 240 ) {

								switch( pVideo->m_nSrcFrameWidth ) {

								case 720: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 360: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 704: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x9000C000 : 0xA000C000); break;

								case 352: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90004000 : 0xA0004000); break;

								case 640: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x9000C000 : 0xA000C000); break;

								case 320: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90004000 : 0xA0004000); break;
								}
							}
							if( lines == 288 ) {

								switch( pVideo->m_nSrcFrameWidth ) {

								case 720: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 360: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 704: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90010000 : 0xA0010000); break;

								case 352: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90008000 : 0xA0008000); break;

								case 640: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x90014000 : 0xA0014000); break;

								case 320: *pDmaBaseCommonStartBuffer++ = remains + (start << 12) + ((start == 0) ? 0x9000C000 : 0xA000C000); break;
								}
							}
						}
						//
					   *pDmaBaseCommonStartBuffer++ = physical_address;

					   physical_address += remains;

						byte_count -= remains;

						remains -= remains;

						start = 0;
					}
					else {
						//shawn 2010.10.04
						if( pDevice->m_nCustomAnalogVideoScaleOutputProperty == 0 ) {

							switch( pVideo->m_nSrcFrameWidth ) {
							
							case 720: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;
							
							case 360: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;
							
							case 704: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90010000 : 0xA0010000); break;
							
							case 352: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90008000 : 0xA0008000); break;
							
							case 640: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90050000 : 0xA0050000); break;
							
							case 320: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90028000 : 0xA0028000); break;
							}
						}
						if( pDevice->m_nCustomAnalogVideoScaleOutputProperty == 1 ) {

							if( lines == 240 ) {

								switch( pVideo->m_nSrcFrameWidth ) {

								case 720: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 360: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 704: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x9000C000 : 0xA000C000); break;

								case 352: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90004000 : 0xA0004000); break;

								case 640: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x9000C000 : 0xA000C000); break;

								case 320: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90004000 : 0xA0004000); break;
								}
							}
							if( lines == 288 ) {

								switch( pVideo->m_nSrcFrameWidth ) {

								case 720: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 360: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90000000 : 0xA0000000); break;

								case 704: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90010000 : 0xA0010000); break;

								case 352: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90008000 : 0xA0008000); break;

								case 640: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x90014000 : 0xA0014000); break;

								case 320: *pDmaBaseCommonStartBuffer++ = byte_count + (start << 12) + ((start == 0) ? 0x9000C000 : 0xA000C000); break;
								}
							}
						}
						//
					   *pDmaBaseCommonStartBuffer++ = physical_address;

					   remains -= byte_count;

						start += byte_count;

						#ifndef TW6805_64bit
						p_sglist++;
						#endif

						count++;

						if( count < mapping_count ) {

							#ifdef TW6805_64bit
							physical_address = 0;

							byte_count = 0;
							#else
							physical_address = sg_dma_address( p_sglist );

							byte_count = sg_dma_len( p_sglist );
							#endif
						}
						else {

							physical_address = 0;

							byte_count = 0;

							goto LOOP_END_LINES;
						}
					}
				}

				//
				//
				if( pVideo->m_nSrcFrameHeight > 288 ) {

					remains = (pitch * 2 - pVideo->m_nSrcFrameWidth) << 1;
				}
				else {

					remains = (pitch * 1 - pVideo->m_nSrcFrameWidth) << 1;
				}

				while( remains ) {

					if( byte_count > remains ) {

						physical_address += remains;

						byte_count -= remains;

						remains -= remains;
					}
					else {

						remains -= byte_count;

						#ifndef TW6805_64bit
						p_sglist++;
						#endif

						count++;

						if( count < mapping_count ) {

							#ifdef TW6805_64bit
							physical_address = 0;

							byte_count = 0;
							#else
							physical_address = sg_dma_address( p_sglist );

							byte_count = sg_dma_len( p_sglist );
							#endif
						}
						else {

							physical_address = 0;

							byte_count = 0;

							goto LOOP_END_LINES;
						}
					}
				}
			}
		}
		else {

			if( is_complete ) {

				if( pVideoBuffer ) {

					pVideoBuffer->m_sKsBuffer.input &= 0xFFFFFFFC;

					pVideoBuffer->m_sKsBuffer.input |= SWITCH_CHANNEL_TABLE[ pVideo->m_nSwitchNumber_Spliter ] % 4; //

				//	if( (pVideoBuffer->m_sKsBuffer.input & 0x00000003) != (0x00) )  {
				//
				//	   p_is_drops[ 0 ] = TRUE;
				//	}
				}
				else
				{
					LINUXV4L2_DEBUG( KERN_INFO, "is_complete but no pVideoBuffer\n" );
				}
			}
			*pDmaBaseCommonStartBuffer++ = 0x90000000;
			
			*pDmaBaseCommonStartBuffer++ = 0x90000000;
			
			*pDmaBaseCommonStartBuffer++ = 0x90000000;
			
			*pDmaBaseCommonStartBuffer++ = 0x90000000;
		}
	}

LOOP_END_LINES:

	if( is_reset ) {

		*pDmaBaseCommonStartBuffer++ = 0xBA000000;

		*pDmaBaseCommonStartBuffer++ = pDmaBaseCommonPhysicalJumpBuffer;

		*pDmaBaseCommonStartBuffer++ = 0xE8000000;
	}
	else {

		*pDmaBaseCommonStartBuffer++ = 0xBB000000;

		*pDmaBaseCommonStartBuffer++ = pDmaBaseCommonPhysicalJumpBuffer;

		*pDmaBaseCommonStartBuffer++ = 0xE8000000;
	}
	
	if( is_complete ) {

		//
		// 
		if( pVideo->m_nDropDecumulatorCounter_Spliter > 0 ) {
			
			pVideo->m_nDropDecumulatorCounter_Spliter--;
			
		    p_is_drops[ 0 ] = TRUE;
		}

		//
		//
		if( pVideoBuffer ) {

			if( pVideo->m_nSrcFrameHeight > 288 ) {

				if( pVideoBuffer->m_sKsBuffer.input & 0x80000000 ) {

					p_is_drops[ 0 ] = TRUE;
				}
			}
		//	pVideoBuffer->m_sKsBuffer.input &= 0x00000003;
		//if vlc, enable below
			pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF;
			
		}

		//
	}
	
	p_is_completes[ 0 ] = is_complete;


	#ifdef TW6805_64bit

	if( is_complete && (p_is_drops[ 0 ] != TRUE)) {

		if( pVideoBuffer )
		{
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			BYTE* pe = videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer ); //NOTE: should use videobuf_queue_vmalloc_init, not videobuf_queue_sg_init
			
			pDevice->n_video_buffer_count = (pDevice->n_video_buffer_count + 1) % TW6816_MAX_BUFFER;

			BYTE* po = pDevice->m_TW6816_video_buffer[ pDevice->n_video_buffer_count ]; 

			if( pe == NULL ) { LINUXV4L2_DEBUG( KERN_INFO, "can not allocate pe()\n" ); return TRUE; }

			ULONG cxe = pVideo->m_nDstFrameWidth;

			ULONG cye = pVideo->m_nDstFrameHeight;

			if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
			{
				memcpy( pe, po, (cxe * cye) * 2 ); 
			}
			
			//LINUXV4L2_DEBUG( KERN_INFO, "copy(%d)\n", (cxe * cye) * 2 );

			//if vlc, enable below
			pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //VLC

			#else
		
			struct videobuf_dmabuf * pKsDMABuffer = &(pVideoBuffer->m_sKsBuffer.dma);

			struct scatterlist * p_sglist = (struct scatterlist *)(pKsDMABuffer->sglist);

			ULONG  mapping_count = pKsDMABuffer->sglen;

			ULONG  pages = 0;

			ULONG  byte_count = sg_dma_len( p_sglist );

			ULONG  remains = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 2;

			pDevice->n_video_buffer_count = (pDevice->n_video_buffer_count + 1) % TW6816_MAX_BUFFER;

			BYTE * po = pDevice->m_TW6816_video_buffer[ pDevice->n_video_buffer_count ];

			BYTE * pe = kmap_atomic( p_sglist->page, KM_USER0 ); //

			if( pVideoBuffer->m_sKsBuffer.state != VIDEOBUF_QUEUED ) { LINUXV4L2_PRINT( KERN_INFO, "pe state error(%x)\n", pVideoBuffer->m_sKsBuffer.state ); return TRUE; }

			while( remains > 0 ) {

				if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
				{	
					memcpy( pe, po + pages * 4096, byte_count ); 
				}

				kunmap_atomic( p_sglist->page, KM_USER0 );

				pages++;

				remains -= byte_count;

				p_sglist++;
					
				if( pages < mapping_count ) {

					pe = kmap_atomic( p_sglist->page, KM_USER0 );

					byte_count = sg_dma_len( p_sglist );
				}
				else
				{
					break;
				}
			}

			//if vlc, enable below
			pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //VLC

#endif
		}

	}
	#endif


	return TRUE;
}

BOOLEAN TW6805_HwProcessAnalogPCIAudioPacket( CDevice *pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	CAudio * pAudio = pAudios[ 0 ];

//	CAudioBuffer * pAudioBuffer = pAudioBuffers[ 0 ];

	BOOLEAN is_complete = FALSE;

	BOOLEAN is_copy = FALSE;

	BYTE *  po = NULL;

	BYTE *  pe = pAudio->m_pKsSubStream->runtime->dma_area;

	ULONG   sz = pAudio->m_nFrameSize;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] TW6805_HwProcessAnalogPCIAudioPacket( %08X:%08X )\n", pDevice->m_nKsDeviceNumber, status, mask );

	if( (status & 0x00000F02) == 0x00000002 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 1 ] + 1920 * 0; is_copy = TRUE; is_complete = TRUE; }
	
	if( (status & 0x00000F02) == 0x00000102 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 1 ] + 1920 * 1; is_copy = TRUE; is_complete = TRUE; }
	
	if( (status & 0x00000F02) == 0x00000202 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 1 ] + 1920 * 2; is_copy = TRUE; is_complete = TRUE; }
	
	if( (status & 0x00000F02) == 0x00000302 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 1 ] + 1920 * 3; is_copy = TRUE; is_complete = TRUE; }

	if( is_copy ) {

		if( pe ) {

			pe += pAudio->m_nFrameNumber * pAudio->m_nFrameSize;
			
			memcpy( pe, po, sz );
		}
	}
	p_is_completes[ 0 ] = is_complete;

	return TRUE;
}

ULONG TW6816_SET_GPIO_DATA( CDevice * pDevice, BYTE R )
{
	TW6805_StopControlPanelAnalysisThread( pDevice );

	I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0x40, 125 ); I2C_WRITE_BYTE( pDevice, R, 125 ); I2C_STOP( pDevice, 125 );

//	DELAY_100NS( 1250000 );
	//schedule_timeout_interruptible( msecs_to_jiffies( 125 ) );

	TW6805_StartControlPanelAnalysisThread( pDevice );

	return true;
}

ULONG TW6816_GET_GPIO_DATA( CDevice * pDevice, BYTE R )
{
	BYTE ret = 0;

	TW6805_StopControlPanelAnalysisThread( pDevice );

	I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0x40, 125 ); I2C_WRITE_BYTE( pDevice, R, 125 ); I2C_STOP( pDevice, 125 );

	I2C_START( pDevice, 125 ); I2C_WRITE_BYTE( pDevice, 0x41, 125 ); ret = I2C_READ_BYTE( pDevice, 1, 125 ); I2C_STOP( pDevice, 125 );

	//schedule_timeout_interruptible( msecs_to_jiffies( 125 ) );

	TW6805_StartControlPanelAnalysisThread( pDevice );

	return ret;
}
