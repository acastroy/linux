#include "LINUXV4L2.h"
#include "property.h"
#include "cx2581.h"

ULONG CX2581_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes )
{
	if( nBytes == 4 ) { return readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0xFFFFFFFF; }

	if( nBytes == 2 ) { return readw( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x0000FFFF; }

	if( nBytes == 1 ) { return readb( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x000000FF; }

	return 0x00000000;
}

BOOLEAN CX2581_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes )
{
	if( nBytes == 4 ) { writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 2 ) { writew( (dwValue & 0x0000FFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 1 ) { writeb( (dwValue & 0x000000FF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	return TRUE;
}

DWORD CX2581_GetAnalogVideoDecoderRegister( CDevice * pDevice, USHORT wIndex, ULONG nBytes )
{
	BYTE  pTransferBuffer[ 12 ];

	ULONG status = 0x00000000;

	DWORD returns = 0x00000000;

	ULONG i = 0; 

	if( nBytes == 1 ) { pTransferBuffer[  0 ] = (BYTE)(wIndex >> 0);
						pTransferBuffer[  1 ] = (BYTE)(wIndex >> 8);
						pTransferBuffer[  2 ] = (0x00);
						pTransferBuffer[  3 ] = (0x88);
						pTransferBuffer[  4 ] = (0x00);
						pTransferBuffer[  5 ] = (0x00);
						pTransferBuffer[  6 ] = (0x00);
						pTransferBuffer[  7 ] = (0x00);
						pTransferBuffer[  8 ] = (0x07);
						pTransferBuffer[  9 ] = (0x12);
						pTransferBuffer[ 10 ] = (0x00);
						pTransferBuffer[ 11 ] = (0x07);
	}
	if( nBytes == 4 ) { pTransferBuffer[  0 ] = (BYTE)(wIndex >> 0);
						pTransferBuffer[  1 ] = (BYTE)(wIndex >> 8);
						pTransferBuffer[  2 ] = (0x00);
						pTransferBuffer[  3 ] = (0x88);
						pTransferBuffer[  4 ] = (0x00);
						pTransferBuffer[  5 ] = (0x00);
						pTransferBuffer[  6 ] = (0x00);
						pTransferBuffer[  7 ] = (0x00);
						pTransferBuffer[  8 ] = (0x07);
						pTransferBuffer[  9 ] = (0x42);
						pTransferBuffer[ 10 ] = (0x00);
						pTransferBuffer[ 11 ] = (0x07);
	}

	down( &(pDevice->m_sI2CAccessSemaphore) );

	writel( *((DWORD *)(pTransferBuffer + 0)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180000) );

	writel( *((DWORD *)(pTransferBuffer + 8)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180008) );

	for( i = 0 ; i < 20 ; i++ ) {

		schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

		status = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180010) );

		if( (status & 0x00000002) == 0x00000000 ) { break; }
	}
	if( (status & 0x00000002) == 0x00000002 ||

		(status & 0x00000001) == 0x00000000 ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_GetAnalogVideoDecoderRegister(): GET REGISTER %04X RROR!!\n", pDevice->m_nKsDeviceNumber, wIndex );

		up( &(pDevice->m_sI2CAccessSemaphore) );

		return 0x00000000;
	}
	returns = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x0018000C) ) & ((nBytes == 1) ? 0x000000FF : 0xFFFFFFFF);

	up( &(pDevice->m_sI2CAccessSemaphore) );

	return returns;
}

BOOLEAN CX2581_SetAnalogVideoDecoderRegister( CDevice * pDevice, USHORT wIndex, DWORD dwValue, ULONG nBytes )
{
	BYTE  pTransferBuffer[ 12 ];

	ULONG status = 0x00000000;

	ULONG i = 0;

	if( nBytes == 1 ) { pTransferBuffer[  0 ] = (0x00);
						pTransferBuffer[  1 ] = (0x00);
						pTransferBuffer[  2 ] = (0x00);
						pTransferBuffer[  3 ] = (0x88);
						pTransferBuffer[  4 ] = (BYTE)(wIndex  >>  8);
						pTransferBuffer[  5 ] = (BYTE)(wIndex  >>  0);
						pTransferBuffer[  6 ] = (BYTE)(dwValue >>  0);
						pTransferBuffer[  7 ] = (0x00);
						pTransferBuffer[  8 ] = (0x04);
						pTransferBuffer[  9 ] = (0x30);
						pTransferBuffer[ 10 ] = (0x00);
						pTransferBuffer[ 11 ] = (0x07);
	}
	if( nBytes == 4 ) { pTransferBuffer[  0 ] = (BYTE)(dwValue >>  0);
						pTransferBuffer[  1 ] = (BYTE)(wIndex  >>  0);
						pTransferBuffer[  2 ] = (BYTE)(wIndex  >>  8);
						pTransferBuffer[  3 ] = (0x88);
						pTransferBuffer[  4 ] = (BYTE)(dwValue >>  8);
						pTransferBuffer[  5 ] = (BYTE)(dwValue >> 16);
						pTransferBuffer[  6 ] = (BYTE)(dwValue >> 24);
						pTransferBuffer[  7 ] = (0x00);
						pTransferBuffer[  8 ] = (0x04);
						pTransferBuffer[  9 ] = (0x33);
						pTransferBuffer[ 10 ] = (0x00);
						pTransferBuffer[ 11 ] = (0x07);
	}
	
	down( &(pDevice->m_sI2CAccessSemaphore) );
	
	writel( *((DWORD *)(pTransferBuffer + 0)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180000) );

	writel( *((DWORD *)(pTransferBuffer + 4)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180004) );

	writel( *((DWORD *)(pTransferBuffer + 8)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180008) );

	for( i = 0 ; i < 20 ; i++ ) {

		schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

		status = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00180010) );

		if( (status & 0x00000002) == 0x00000000 ) { break; }
	}
	up( &(pDevice->m_sI2CAccessSemaphore) );

	if( (status & 0x00000002) == 0x00000002 ||

		(status & 0x00000001) == 0x00000000 ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_SetAnalogVideoDecoderRegister(): SET REGISTER %04X กั %08X ERROR!!\n", pDevice->m_nKsDeviceNumber, wIndex, (unsigned int)(dwValue) );

		return FALSE;
	}
	return TRUE;
}

VOID CX2581_I2C_SET_SCL( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	ULONG R00190008 = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) ); R00190008 &= 0xFFFDFFFF;

	writel( R00190008 | 0x00000020 | (DATA << 17), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

VOID CX2581_I2C_SET_SDA( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	ULONG R00190008 = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) ); R00190008 &= 0xFFFEFFFF;

	writel( R00190008 | 0x00000020 | (DATA << 16), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

VOID CX2581_I2C_SET_PIN( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY ) 
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	BYTE SCL = (DATA >> 0) & (0x01);

	BYTE SDA = (DATA >> 1) & (0x01);

	ULONG R00190008 = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) ); R00190008 &= 0xFFFCFFFF;

	writel( R00190008 | 0x00000020 | (SDA << 16) | (SCL << 17), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

BYTE CX2581_I2C_GET_SDA( CDevice * pDevice, ULONGLONG DELAY ) 
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	ULONG R00190008 = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00190008) ); R00190008 >>= 20; R00190008 &= 0x00000001; 

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
	return (BYTE)(R00190008);
}

BOOLEAN CX2581_I2C_START( CDevice * pDevice, ULONGLONG DELAY )
{
	CX2581_I2C_SET_SDA( pDevice, 1, DELAY );

	CX2581_I2C_SET_PIN( pDevice, 3, DELAY );

	CX2581_I2C_SET_PIN( pDevice, 1, DELAY );

	CX2581_I2C_SET_PIN( pDevice, 0, DELAY );

	return TRUE;
}

BOOLEAN CX2581_I2C_STOP( CDevice * pDevice, ULONGLONG DELAY )
{
	CX2581_I2C_SET_SCL( pDevice, 0, DELAY );

	CX2581_I2C_SET_PIN( pDevice, 0, DELAY );

	CX2581_I2C_SET_PIN( pDevice, 1, DELAY );

	CX2581_I2C_SET_PIN( pDevice, 3, DELAY );

	return TRUE;
}

BOOLEAN CX2581_I2C_WAIT_ACK( CDevice * pDevice, ULONGLONG DELAY )
{	
	BYTE ACK = 0;

	CX2581_I2C_SET_PIN( pDevice, 3, DELAY );    

	ACK = CX2581_I2C_GET_SDA( pDevice, DELAY ); 

	CX2581_I2C_SET_PIN( pDevice, 0, DELAY );    

	return (ACK == 0) ? TRUE : FALSE;           
}

BOOLEAN CX2581_I2C_REPLY_ACK( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY )
{
	CX2581_I2C_SET_PIN( pDevice, (ACK) ? 2 : 0, DELAY );

	CX2581_I2C_SET_PIN( pDevice, (ACK) ? 3 : 1, DELAY );

	CX2581_I2C_SET_PIN( pDevice, (ACK) ? 2 : 0, DELAY );

	return TRUE;
}

BOOLEAN CX2581_I2C_WRITE_BYTE( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONG i = 0;

	for( i = 0 ; i < 8 ; i++, DATA <<= 1 ) {                        

		CX2581_I2C_SET_PIN( pDevice, (DATA & 0x80) ? 2 : 0, DELAY );

		CX2581_I2C_SET_PIN( pDevice, (DATA & 0x80) ? 3 : 1, DELAY );

		CX2581_I2C_SET_PIN( pDevice, (DATA & 0x80) ? 2 : 0, DELAY );
	}
	return CX2581_I2C_WAIT_ACK( pDevice, DELAY );                   
}

BYTE CX2581_I2C_READ_BYTE( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY )
{	
	BYTE DATA = 0x00;

	ULONG i = 0;

	for( i = 0 ; i < 8 ; i++ ) {		
		
		CX2581_I2C_SET_PIN( pDevice, 2, DELAY );				   

		CX2581_I2C_SET_PIN( pDevice, 3, DELAY );				   

		DATA = (DATA << 1) | CX2581_I2C_GET_SDA( pDevice, DELAY ); 

		CX2581_I2C_SET_PIN( pDevice, 0, DELAY );				   
	}
	CX2581_I2C_REPLY_ACK( pDevice, ACK, DELAY );                   

	return DATA;
}

static ULONG g_copy_protect_unlock_boradsA[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static ULONG g_copy_protect_unlock_boradsB[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

BOOLEAN CX2581_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, CVideo * pVideo )
{
//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_SetAnalogVideoDecoderProcAmpProperties()\n", pDevice->m_nKsDeviceNumber );

	ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1;

	if( g_n_analog_decoder_brightness[ i ][ 0 ] & 0x80000000 ) { CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1014 + (0x0200 * pVideo->m_nSubDeviceNumber), (BYTE)(((g_n_analog_decoder_brightness[ i ][ 0 ] & 0xFF) >> 0) - 128) | 0x00, 1 ); g_n_analog_decoder_brightness[ i ][ 0 ] &= 0x7FFFFFFF; }
	
	if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x80000000 ) { CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1015 + (0x0200 * pVideo->m_nSubDeviceNumber), (BYTE)(((g_n_analog_decoder_contrast[ i ][ 0 ]   & 0xFF) >> 0) -   0) | 0x00, 1 ); g_n_analog_decoder_contrast[ i ][ 0 ]   &= 0x7FFFFFFF; }
	
	if( g_n_analog_decoder_saturation[ i ][ 0 ] & 0x80000000 ) { CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1018 + (0x0200 * pVideo->m_nSubDeviceNumber), (BYTE)(((g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) >> 0) -   0) | 0x00, 1 );
	
																 CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1019 + (0x0200 * pVideo->m_nSubDeviceNumber), (BYTE)(((g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) >> 0) -   0) | 0x00, 1 ); g_n_analog_decoder_saturation[ i ][ 0 ] &= 0x7FFFFFFF; }
	
	if( g_n_analog_decoder_hue[ i ][ 0 ]        & 0x80000000 ) { CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x101A + (0x0200 * pVideo->m_nSubDeviceNumber), (BYTE)(((g_n_analog_decoder_hue[ i ][ 0 ]        & 0xFF) >> 0) - 128) | 0x00, 1 ); g_n_analog_decoder_hue[ i ][ 0 ]        &= 0x7FFFFFFF; }

	if( g_n_analog_decoder_sharpness[ i ][ 0 ]  & 0x80000000 ) {                                                                                                                                                                                   g_n_analog_decoder_sharpness[ i ][ 0 ]  &= 0x7FFFFFFF; }

	{	ULONG R0000100C = CX2581_GetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x100C + (0x0200 * pVideo->m_nSubDeviceNumber)), 1 ); 

		pVideo->m_nLockStatus = (R0000100C & 0x00000010) ? (1) : (0);
	}
	return TRUE;
}

BOOLEAN CX2581_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, CVideo * pVideo )
{
	ULONG e_v4l2_std = pDevice->m_nCustomVideoStandardProperty;

	ULONG cx = (pVideo) ? (pVideo->m_nSrcFrameWidth) : (720);

	ULONG i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_SetAnalogVideoDecoderStandardProperty( S:%08lX )\n", pDevice->m_nKsDeviceNumber, e_v4l2_std );

	if( e_v4l2_std & V4L2_STD_525_60 ) {

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00001000 = 0x00000000;

			ULONG R00001004 = 0x00000000;

			ULONG R00001008 = 0x00000000;

			ULONG R00001014 = 0x00000000;

			ULONG R00001020 = 0x00000000;

			ULONG R00001024 = 0x00000000;

			ULONG R00001028 = 0x00000000;

			ULONG R00001040 = 0x00000000;

			ULONG R00001038 = 0x00000000;

			ULONG R0000103C = 0x00000000;

			if( pVideo ) { 
				
				if( pVideo->m_nSubDeviceNumber != i ) { 
					
					continue ; 
				} 
			}
			R00001000 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1000 + (0x0200 * i), 4 ); R00001000 &= 0xFFFFFFF0; R00001000 |= 0x00010001;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1000 + (0x0200 * i), R00001000, 4 );

			R00001004 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1004 + (0x0200 * i), 4 ); R00001004 |= 0x00040080;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1004 + (0x0200 * i), R00001004, 4 );

			R00001008 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1008 + (0x0200 * i), 4 ); R00001008 |= 0x00040080;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1008 + (0x0200 * i), R00001008, 4 );
			
			R00001020 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1020 + (0x0200 * i), 4 ); R00001020 &= 0x00C00C00; R00001020 |= 0x612D0074;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1020 + (0x0200 * i), R00001020, 4 );

			R00001024 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1024 + (0x0200 * i), 4 ); R00001024 &= 0x00C00C00; R00001024 |= 0x1E1E801A;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1024 + (0x0200 * i), R00001024, 4 );

			R00001028 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1028 + (0x0200 * i), 4 ); R00001028 &= 0xFFFF7FFF; R00001028 |= 0x00004000;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1028 + (0x0200 * i), R00001028, 4 );

			R00001040 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1040 + (0x0200 * i), 4 ); R00001040 &= 0xDFFFFFFF;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1040 + (0x0200 * i), R00001040, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x105C + (0x0200 * i), 0x43E00000, 4 );

			R00001038 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1038 + (0x0200 * i), 4 ); R00001038 &= 0x7FFF0000; R00001038|= 0x80002020;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1038 + (0x0200 * i), R00001038, 4 );

			R0000103C = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x103C + (0x0200 * i), 4 ); R0000103C |= 0x80000000;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x103C + (0x0200 * i), R0000103C, 4 );


			R00001014 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1014 + (0x0200 * i), 4 ); R00001014 &= 0xFFF8FFFF; R00001014 |= 0x00010000;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1014 + (0x0200 * i), R00001014, 4 );

			switch( cx ) {
			
			case 720: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x00000073, 4 ); break;
			
			case 704: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x0000007A, 4 ); break;
			
			case 640: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x0000009A, 4 ); break;
			
			case 360: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x00000073, 4 ); break;
			
			case 352: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x0000007B, 4 ); break;
			
			case 320: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x0000009B, 4 ); break;
			}
			switch( cx ) {
			
			case 720: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x00000000, 4 ); break;
			
			case 704: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x00000000, 4 ); break;
			
			case 640: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x00000000, 4 ); break;
			
			case 360: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x000FF000, 4 ); break;
			
			case 352: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x000FF000, 4 ); break;
			
			case 320: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x000FF000, 4 ); break;
			}
			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1068 + (0x0200 * i)), 0x00001453, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x106C + (0x0200 * i)), 0x00001453, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1070 + (0x0200 * i)), 0x00001029, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1074 + (0x0200 * i)), 0x414F4F4F, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1078 + (0x0200 * i)), 0x02C00A0E, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x107C + (0x0200 * i)), 0x00030000, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1080 + (0x0200 * i)), 0x00000000, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1084 + (0x0200 * i)), 0x503005D4, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1088 + (0x0200 * i)), 0xC0F00AC8, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x108C + (0x0200 * i)), 0x05000023, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1090 + (0x0200 * i)), 0x000F0000, 4 );
		}
	}
	if( e_v4l2_std & V4L2_STD_625_50 ) {

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00001000 = 0x00000000;

			ULONG R00001004 = 0x00000000;

			ULONG R00001008 = 0x00000000;

			ULONG R00001014 = 0x00000000;

			ULONG R00001020 = 0x00000000;

			ULONG R00001024 = 0x00000000;

			ULONG R00001028 = 0x00000000;

			ULONG R00001040 = 0x00000000;

			ULONG R00001038 = 0x00000000;

			ULONG R0000103C = 0x00000000;

			if( pVideo ) { 
				
				if( pVideo->m_nSubDeviceNumber != i ) { 
					
					continue ; 
				} 
			}
			R00001000 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1000 + (0x0200 * i), 4 ); R00001000 &= 0xFFFFFFF0; R00001000 |= 0x00010004;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1000 + (0x0200 * i), R00001000, 4 );

			R00001004 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1004 + (0x0200 * i), 4 ); R00001004 |= 0x00040080;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1004 + (0x0200 * i), R00001004, 4 );

			R00001008 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1008 + (0x0200 * i), 4 ); R00001008 |= 0x00040080;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1008 + (0x0200 * i), R00001008, 4 );
			
			R00001020 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1020 + (0x0200 * i), 4 ); R00001020 &= 0x00C00C00; R00001020 |= 0x632D007D;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1020 + (0x0200 * i), R00001020, 4 );

			R00001024 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1024 + (0x0200 * i), 4 ); R00001024 &= 0x00C00C00; R00001024 |= 0x28240028;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1024 + (0x0200 * i), R00001024, 4 );

			R00001028 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1028 + (0x0200 * i), 4 ); R00001028 &= 0xFFFF7FFF; R00001028 |= 0x00004000;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1028 + (0x0200 * i), R00001028, 4 );

			R00001040 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1040 + (0x0200 * i), 4 ); R00001040 &= 0xDFFFFFFF;
			
			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1040 + (0x0200 * i), R00001040, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x105C + (0x0200 * i), 0x5411E2D0, 4 );

			R00001038 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1038 + (0x0200 * i), 4 ); R00001038 &= 0x7FFF0000; R00001038|= 0x80002020;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1038 + (0x0200 * i), R00001038, 4 );

			R0000103C = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x103C + (0x0200 * i), 4 ); R0000103C |= 0x80000000;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x103C + (0x0200 * i), R0000103C, 4 );

			R00001014 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x1014 + (0x0200 * i), 4 ); R00001014 &= 0xFFF8FFFF; R00001014 |= 0x00010000;

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x1014 + (0x0200 * i), R00001014, 4 );

			switch( cx ) {
			
			case 720: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x0000007F, 4 ); break;
			
			case 704: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x00000087, 4 ); break;
			
			case 640: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x000000A6, 4 ); break;
			
			case 360: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x00000080, 4 ); break;
			
			case 352: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x00000088, 4 ); break;
			
			case 320: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1020 + (0x0200 * i)), (R00001020 & 0xFFFFFC00) | 0x000000A6, 4 ); break;
			}
			switch( cx ) {
			
			case 720: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x00000000, 4 ); break;
			
			case 704: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x00000000, 4 ); break;
			
			case 640: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x00000000, 4 ); break;
			
			case 360: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x000FF000, 4 ); break;
			
			case 352: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x000FF000, 4 ); break;
			
			case 320: CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1030 + (0x0200 * i)), 0x000FF000, 4 ); break;
			}
			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1068 + (0x0200 * i)), 0x0000142F, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x106C + (0x0200 * i)), 0x0000302F, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1070 + (0x0200 * i)), 0x00001023, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1074 + (0x0200 * i)), 0x410F2F0F, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1078 + (0x0200 * i)), 0x02100A0E, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x107C + (0x0200 * i)), 0x00030000, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1080 + (0x0200 * i)), 0x00000000, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1084 + (0x0200 * i)), 0x503005D4, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1088 + (0x0200 * i)), 0xC0F00AB8, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x108C + (0x0200 * i)), 0x05000023, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x1090 + (0x0200 * i)), 0x000F0000, 4 );
		}
	}

	return TRUE;
}

BOOLEAN CX2581_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG i = 0;

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 0 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}
	else { 

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_HwAnalogComponentsInitialize( %02ld )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCaptureStreamPowerReference );
	
	{	if( g_copy_protect_unlock_boradsA[ 0 ] == 0 ) { return TRUE; } // shawn 2010.07.26
	}
	{	if( g_copy_protect_unlock_boradsB[ 0 ] == 0 ) { return TRUE; } // shawn 2010.07.26
	}	
	
	if( pDevice->m_nAnalogCaptureStreamPowerReference >= 1 ) { 

		if( pVideo ) { 

			ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1;

			g_n_analog_decoder_brightness[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_contrast[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_saturation[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_hue[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_sharpness[ i ][ 0 ] |= 0x80000000;

			pVideo->m_nFrameCopyPolarity = 0x00000000;

			pVideo->m_nLockStatus = 0x00000000;
			
			if( TRUE != CX2581_SetAnalogVideoDecoderStandardProperty( pDevice, pVideo ) ) { 
				
				return TRUE; 
			}

			if( TRUE != CX2581_SetAnalogVideoDecoderProcAmpProperties( pDevice, pVideo ) ) { 
				
				return TRUE; 
			}

			if( pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_GREY ) { CX2581_SetRegister( pDevice, 0x00130084 + (0x00000100 * pVideo->m_nSubDeviceNumber), 0x00000006, 4 ); }

			if( pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_Y41P ) { CX2581_SetRegister( pDevice, 0x00130084 + (0x00000100 * pVideo->m_nSubDeviceNumber), 0x00000005, 4 ); }

			if( pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV ) { CX2581_SetRegister( pDevice, 0x00130084 + (0x00000100 * pVideo->m_nSubDeviceNumber), 0x00000004, 4 ); }
				
		}
		if( pAudio ) { 

			ULONG R00110088 = 0x00000000;

		//	ULONG i = pAudio->m_pDevice->m_nKsDeviceNumber * 8 + pAudio->m_nSubDeviceNumber * 1;

			if( pAudio->m_nRate ==  8000 ) { CX2581_SetRegister( pDevice, 0x00110088, 0x1205BC28, 4 ); CX2581_SetRegister( pDevice, 0x0011008C, 0x800001BF, 4 ); }

			if( pAudio->m_nRate == 16000 ) { CX2581_SetRegister( pDevice, 0x00110088, 0x240B7850, 4 ); CX2581_SetRegister( pDevice, 0x0011008C, 0x800001BF, 4 ); }

			if( pAudio->m_nRate == 24000 ) { CX2581_SetRegister( pDevice, 0x00110088, 0x1807A58B, 4 ); CX2581_SetRegister( pDevice, 0x0011008C, 0x8000019C, 4 ); }

			if( pAudio->m_nRate == 32000 ) { CX2581_SetRegister( pDevice, 0x00110088, 0x249DF010, 4 ); CX2581_SetRegister( pDevice, 0x0011008C, 0x800001B2, 4 ); }

			if( pAudio->m_nRate == 40000 ) { CX2581_SetRegister( pDevice, 0x00110088, 0x16E2B60A, 4 ); CX2581_SetRegister( pDevice, 0x0011008C, 0x80000190, 4 ); }

			if( pAudio->m_nRate == 48000 ) { CX2581_SetRegister( pDevice, 0x00110088, 0x2931AE12, 4 ); CX2581_SetRegister( pDevice, 0x0011008C, 0x80000198, 4 ); }

			R00110088 = CX2581_GetRegister( pDevice, 0x00110088, 4 ); R00110088 &= 0x7FFFFFFF;

			CX2581_SetRegister( pDevice, 0x00110088, R00110088, 4 );
		}
	}
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010000 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010050 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x000100A0 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x000100F0 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010140 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010190 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x000101E0 + i, 0x00000000, 4 ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010230 + i, 0x00000000, 4 ); }

		// 
		//
		CX2581_SetRegister( pDevice, 0x00100100, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100104, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100108, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x0010010C, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100110, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100114, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100118, (1440 / 16) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x0010011C, (1440 / 16) - 1, 4 );

		CX2581_SetRegister( pDevice, 0x00010C00 + (16 * 0), 0x00000040 + (1440 *  0), 4 );
		CX2581_SetRegister( pDevice, 0x00010C00 + (16 * 1), 0x00000040 + (1440 *  1), 4 );
		CX2581_SetRegister( pDevice, 0x00010C00 + (16 * 2), 0x00000040 + (1440 *  2), 4 );
		CX2581_SetRegister( pDevice, 0x00010C00 + (16 * 3), 0x00000040 + (1440 *  3), 4 );
		CX2581_SetRegister( pDevice, 0x00010C40 + (16 * 0), 0x00000040 + (1440 *  4), 4 );
		CX2581_SetRegister( pDevice, 0x00010C40 + (16 * 1), 0x00000040 + (1440 *  5), 4 );
		CX2581_SetRegister( pDevice, 0x00010C40 + (16 * 2), 0x00000040 + (1440 *  6), 4 );
		CX2581_SetRegister( pDevice, 0x00010C40 + (16 * 3), 0x00000040 + (1440 *  7), 4 );
		CX2581_SetRegister( pDevice, 0x00010C80 + (16 * 0), 0x00000040 + (1440 *  8), 4 );
		CX2581_SetRegister( pDevice, 0x00010C80 + (16 * 1), 0x00000040 + (1440 *  9), 4 );
		CX2581_SetRegister( pDevice, 0x00010C80 + (16 * 2), 0x00000040 + (1440 * 10), 4 );
		CX2581_SetRegister( pDevice, 0x00010C80 + (16 * 3), 0x00000040 + (1440 * 11), 4 );
		CX2581_SetRegister( pDevice, 0x00010CC0 + (16 * 0), 0x00000040 + (1440 * 12), 4 );
		CX2581_SetRegister( pDevice, 0x00010CC0 + (16 * 1), 0x00000040 + (1440 * 13), 4 );
		CX2581_SetRegister( pDevice, 0x00010CC0 + (16 * 2), 0x00000040 + (1440 * 14), 4 );
		CX2581_SetRegister( pDevice, 0x00010CC0 + (16 * 3), 0x00000040 + (1440 * 15), 4 );
		CX2581_SetRegister( pDevice, 0x00010D00 + (16 * 0), 0x00000040 + (1440 * 16), 4 );
		CX2581_SetRegister( pDevice, 0x00010D00 + (16 * 1), 0x00000040 + (1440 * 17), 4 );
		CX2581_SetRegister( pDevice, 0x00010D00 + (16 * 2), 0x00000040 + (1440 * 18), 4 );
		CX2581_SetRegister( pDevice, 0x00010D00 + (16 * 3), 0x00000040 + (1440 * 19), 4 );
		CX2581_SetRegister( pDevice, 0x00010D40 + (16 * 0), 0x00000040 + (1440 * 20), 4 );
		CX2581_SetRegister( pDevice, 0x00010D40 + (16 * 1), 0x00000040 + (1440 * 21), 4 );
		CX2581_SetRegister( pDevice, 0x00010D40 + (16 * 2), 0x00000040 + (1440 * 22), 4 );
		CX2581_SetRegister( pDevice, 0x00010D40 + (16 * 3), 0x00000040 + (1440 * 23), 4 );
		CX2581_SetRegister( pDevice, 0x00010D80 + (16 * 0), 0x00000040 + (1440 * 24), 4 );
		CX2581_SetRegister( pDevice, 0x00010D80 + (16 * 1), 0x00000040 + (1440 * 25), 4 );
		CX2581_SetRegister( pDevice, 0x00010D80 + (16 * 2), 0x00000040 + (1440 * 26), 4 );
		CX2581_SetRegister( pDevice, 0x00010D80 + (16 * 3), 0x00000040 + (1440 * 27), 4 );
		CX2581_SetRegister( pDevice, 0x00010DC0 + (16 * 0), 0x00000040 + (1440 * 28), 4 );
		CX2581_SetRegister( pDevice, 0x00010DC0 + (16 * 1), 0x00000040 + (1440 * 29), 4 );
		CX2581_SetRegister( pDevice, 0x00010DC0 + (16 * 2), 0x00000040 + (1440 * 30), 4 );
		CX2581_SetRegister( pDevice, 0x00010DC0 + (16 * 3), 0x00000040 + (1440 * 31), 4 );

		for( i = 0 ; i < 8 ; i ++ ) { 

			CX2581_SetRegister( pDevice, 0x00010000 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2581_SetRegister( pDevice, 0x00010004 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2581_SetRegister( pDevice, 0x00010008 + (0x00000050 * i), 0x00010C00 + (0x00000040 * i), 4 );
			CX2581_SetRegister( pDevice, 0x0001000C + (0x00000050 * i), 0x00000040 / (             8), 4 );
			CX2581_SetRegister( pDevice, 0x00010010 + (0x00000050 * i), 0x00011000 + (0x00000040 * i), 4 );
			CX2581_SetRegister( pDevice, 0x00010014 + (0x00000050 * i), 0x00000040 / (             4), 4 );
			CX2581_SetRegister( pDevice, 0x00100080 + (0x00000004 * i), 0x00010C00 + (0x00000040 * i), 4 );
			CX2581_SetRegister( pDevice, 0x00100180 + (0x00000004 * i), 0x00000040 / (             8), 4 );
		}
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010500 + i, 0x00000000, 4  ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x00010550 + i, 0x00000000, 4  ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x000105A0 + i, 0x00000000, 4  ); }
		for( i = 0 ; i < 80 ; i += 4 ) { CX2581_SetRegister( pDevice, 0x000105F0 + i, 0x00000000, 4  ); }

		CX2581_SetRegister( pDevice, 0x00100140, (128 /  8) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100144, (128 /  8) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x00100148, (128 /  8) - 1, 4 );
		CX2581_SetRegister( pDevice, 0x0010014C, (128 /  8) - 1, 4 );

		CX2581_SetRegister( pDevice, 0x00010E80 + (16 * 0), 0x0000B500 + (128 *  0), 4 );
		CX2581_SetRegister( pDevice, 0x00010E80 + (16 * 1), 0x0000B500 + (128 *  1), 4 );
		CX2581_SetRegister( pDevice, 0x00010E80 + (16 * 2), 0x0000B500 + (128 *  2), 4 );
		CX2581_SetRegister( pDevice, 0x00010EB0 + (16 * 0), 0x0000B500 + (128 *  3), 4 );
		CX2581_SetRegister( pDevice, 0x00010EB0 + (16 * 1), 0x0000B500 + (128 *  4), 4 );
		CX2581_SetRegister( pDevice, 0x00010EB0 + (16 * 2), 0x0000B500 + (128 *  5), 4 );
		CX2581_SetRegister( pDevice, 0x00010EE0 + (16 * 0), 0x0000B500 + (128 *  6), 4 );
		CX2581_SetRegister( pDevice, 0x00010EE0 + (16 * 1), 0x0000B500 + (128 *  7), 4 );
		CX2581_SetRegister( pDevice, 0x00010EE0 + (16 * 2), 0x0000B500 + (128 *  8), 4 );
		CX2581_SetRegister( pDevice, 0x00010F10 + (16 * 0), 0x0000B500 + (128 *  9), 4 );
		CX2581_SetRegister( pDevice, 0x00010F10 + (16 * 1), 0x0000B500 + (128 * 10), 4 );
		CX2581_SetRegister( pDevice, 0x00010F10 + (16 * 2), 0x0000B500 + (128 * 11), 4 );

		for( i = 0 ; i < 4 ; i ++ ) { 

			CX2581_SetRegister( pDevice, 0x00010500 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2581_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2581_SetRegister( pDevice, 0x00010508 + (0x00000050 * i), 0x00010E80 + (0x00000030 * i), 4 );
			CX2581_SetRegister( pDevice, 0x0001050C + (0x00000050 * i), 0x00000030 / (             8), 4 );
			CX2581_SetRegister( pDevice, 0x00010510 + (0x00000050 * i), 0x00011280 + (0x00000040 * i), 4 );
			CX2581_SetRegister( pDevice, 0x00010514 + (0x00000050 * i), 0x00000040 / (             4), 4 );
			CX2581_SetRegister( pDevice, 0x001000C0 + (0x00000004 * i), 0x00010E80 + (0x00000030 * i), 4 );
			CX2581_SetRegister( pDevice, 0x001001C0 + (0x00000004 * i), 0x00000030 / (             8), 4 );
		}
		for( i = 0 ; i < 8 ; i ++ ) {

			dma_addr_t pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
			dma_addr_t pDmaBaseCommonPhysicalBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
			DWORD *    pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);
			DWORD *    pDmaBaseCommonBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);

			pDmaBaseCommonPhysicalStartBuffer += (CX2581_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (1);
			pDmaBaseCommonPhysicalBuffer      += (CX2581_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (1);
			pDmaBaseCommonStartBuffer         += (CX2581_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (4);
			pDmaBaseCommonBuffer              += (CX2581_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (4);

			CX2581_SetRegister( pDevice, 0x00010000 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >>  0, 4 ); //
		//	CX2581_SetRegister( pDevice, 0x00010004 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >> 32, 4 ); //
			CX2581_SetRegister( pDevice, 0x00010004 + (0x00000050 * i), 0x00000000, 4 );

			//
			// 
			memset( pDmaBaseCommonBuffer, 0x00, CX2581_MAX_VIDEO_DMA_BUFFER_SIZE / 8 );

			pDmaBaseCommonPhysicalBuffer += 80;

			pDmaBaseCommonBuffer += 0 >> 2;
		
			pDmaBaseCommonBuffer[  0 ] = 0x80008000;
			pDmaBaseCommonBuffer[  1 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  2 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  3 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  4 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  5 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  6 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  7 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  8 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  9 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 10 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 11 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 12 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 13 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 14 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 15 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 16 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 17 ] = 0x70000000;
			pDmaBaseCommonBuffer[ 18 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
		//	pDmaBaseCommonBuffer[ 19 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
			pDmaBaseCommonBuffer[ 19 ] = 0x00000000;
			
			pDmaBaseCommonPhysicalBuffer += 6940;

			pDmaBaseCommonBuffer += 80 >> 2;

			pDmaBaseCommonBuffer[  0 ] = 0x2C0005A0;
			pDmaBaseCommonBuffer[  1 ] = 0x71030000;
			pDmaBaseCommonBuffer[  2 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
		//	pDmaBaseCommonBuffer[  3 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
			pDmaBaseCommonBuffer[  3 ] = 0x00000000;
			
			pDmaBaseCommonPhysicalBuffer += 80;

			pDmaBaseCommonBuffer += 6940 >> 2;

			pDmaBaseCommonBuffer[  0 ] = 0x80008200;
			pDmaBaseCommonBuffer[  1 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  2 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  3 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  4 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  5 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  6 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  7 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  8 ] = 0xF0000000;
			pDmaBaseCommonBuffer[  9 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 10 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 11 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 12 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 13 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 14 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 15 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 16 ] = 0xF0000000;
			pDmaBaseCommonBuffer[ 17 ] = 0x70000000; 
			pDmaBaseCommonBuffer[ 18 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
		//	pDmaBaseCommonBuffer[ 19 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
			pDmaBaseCommonBuffer[ 19 ] = 0x00000000;
			
			pDmaBaseCommonPhysicalBuffer += 6940;

			pDmaBaseCommonBuffer += 80 >> 2;

			pDmaBaseCommonBuffer[  0 ] = 0x2C0005A0;
			pDmaBaseCommonBuffer[  1 ] = 0x71010000;
			pDmaBaseCommonBuffer[  2 ] = pDmaBaseCommonPhysicalStartBuffer >>  0; //
		//	pDmaBaseCommonBuffer[  3 ] = pDmaBaseCommonPhysicalStartBuffer >> 32; //
			pDmaBaseCommonBuffer[  3 ] = 0x00000000;

		}
	
		for( i = 0 ; i < 4 ; i ++ ) {

			dma_addr_t pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];
			dma_addr_t pDmaBaseCommonPhysicalBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];
			DWORD *    pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 1 ]);
			DWORD *    pDmaBaseCommonBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 1 ]);
			ULONG      j = 0;

			pDmaBaseCommonPhysicalStartBuffer += (CX2581_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (1);
			pDmaBaseCommonPhysicalBuffer      += (CX2581_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (1);
			pDmaBaseCommonStartBuffer         += (CX2581_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (4);
			pDmaBaseCommonBuffer              += (CX2581_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (4);

			CX2581_SetRegister( pDevice, 0x00010500 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >>  0, 4 ); //
		//	CX2581_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >> 32, 4 ); //
			CX2581_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), 0x00000000, 4 );

			memset( pDmaBaseCommonBuffer, 0x00, CX2581_MAX_AUDIO_DMA_BUFFER_SIZE / 4 );

			for( j = 0 ; j < 4 ; j++ ) {

				dma_addr_t physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 2 ];

				physical_address += (1920 * 4) * (i);

				physical_address += (1920) * (j);

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;
				
				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;
				
				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;
				
				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;
				
				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128);
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = (j == 0) ? 0x71030000 : 0x71010000;
				pDmaBaseCommonBuffer[ 4 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
			//	pDmaBaseCommonBuffer[ 5 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
				pDmaBaseCommonBuffer[ 5 ] = 0x00000000;

				physical_address += 128;

				pDmaBaseCommonBuffer += 24 >> 2;
			}
			pDmaBaseCommonBuffer[ -2 ] = pDmaBaseCommonPhysicalStartBuffer >>  0; //
		//	pDmaBaseCommonBuffer[ -1 ] = pDmaBaseCommonPhysicalStartBuffer >> 32; //
			pDmaBaseCommonBuffer[ -1 ] = 0x00000000;
		}
		for( i = 0 ; i < 8 ; i++ ) { 

			CX2581_SetRegister( pDevice, 0x00130030 + (0x00000100 * i), 0x00000003, 4 );

			CX2581_SetRegister( pDevice, 0x00130080 + (0x00000100 * i), 0x00000000, 4 );
		}
		for( i = 0 ; i < 4 ; i++ ) { 

			ULONG R0014001C = 0x00000000;

			CX2581_SetRegister( pDevice, 0x00140014 + (0x00000100 * i), 0x00000003, 4 );

			CX2581_SetRegister( pDevice, 0x00140018 + (0x00000100 * i), (128), 4 );

			R0014001C = CX2581_GetRegister( pDevice, 0x0014001C + (0x00000100 * i), 4 ); R0014001C |= 0x00024200;

			CX2581_SetRegister( pDevice, 0x0014001C + (0x00000100 * i), R0014001C, 4 );
		}
	}
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		pDevice->m_hInterruptAccessLock = FALSE;

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00130040 = 0x00000000;

			// ...
			//
			CX2581_SetRegister( pDevice, 0x00040024 + (0x00000010 * i), 0x00000001, 4 );

			// ...
			//
			CX2581_SetRegister( pDevice, 0x00040020 + (0x00000010 * i), 0x00000001, 4 );

			// ...
			// 
			R00130040 = CX2581_GetRegister( pDevice, 0x00130040 + (0x00000100 * i), 4 ) & 0xFFFFFFEE;

			CX2581_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000001, 4 );

			CX2581_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000011, 4 );
		}

		for( i = 0 ; i < 4 ; i++ ) {

			// ...
			// 
			CX2581_SetRegister( pDevice, 0x000400C4 + (0x00000010 * i), 0x00000001, 4 );

			//
			// 
			CX2581_SetRegister( pDevice, 0x000400C0 + (0x00000010 * i), 0x00000001, 4 );
		}
		{	//
			// 
			ULONG R00140500 = CX2581_GetRegister( pDevice, 0x00140500, 4 ) & 0xFFFFF0F0;

			CX2581_SetRegister( pDevice, 0x00140500, R00140500 | 0x0000000F, 4 );

			CX2581_SetRegister( pDevice, 0x00140500, R00140500 | 0x00000F0F, 4 ); 
		}

		pDevice->m_hInterruptAccessLock = TRUE;
	}

	return TRUE;
}

BOOLEAN CX2581_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_HwAnalogComponentsUnInitialize( %02ld )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCaptureStreamPowerReference );

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

	pDevice->m_hInterruptAccessLock = FALSE;

	for( i = 0 ; i < 8 ; i++ ) { 
		
		ULONG R00130040 = 0x00000000;

		// ...
		//
		R00130040 = CX2581_GetRegister( pDevice, 0x00130040 + (0x00000100 * i), 4 ) & 0xFFFFFFEE;

		CX2581_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000001, 4 );

		CX2581_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000000, 4 );

		// ...
		//
		CX2581_SetRegister( pDevice, 0x00040020 + (0x00000010 * i), 0x00000000, 4 );
	}
	{	// ...
		//
		ULONG R00140500 = CX2581_GetRegister( pDevice, 0x00140500, 4 ) & 0xFFFFF0F0;

		CX2581_SetRegister( pDevice, 0x00140500, R00140500 | 0x0000000F, 4 );

		CX2581_SetRegister( pDevice, 0x00140500, R00140500 | 0x00000000, 4 );
	}
	for( i = 0 ; i < 4 ; i++ ) {

		// ...
		// 
		CX2581_SetRegister( pDevice, 0x000400C0 + (0x00000010 * i), 0x00000000, 4 ); 
	}
	pDevice->m_hInterruptAccessLock = FALSE;

	return TRUE;
}
//peter 2011.10.24 WATCHDOG

static int CX2581_OnControlPanelAnalysisThread( void * p_context )
{
	ULONG counts = 0;

	CDevice * pDevice = p_context;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] CX2581_OnControlPanelAnalysisThread( enter ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pDevice->m_pControlThread) );

	while( kthread_should_stop() == FALSE ) {

		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ){

			if( pDevice->m_nCustomSoftwareWatchdogResetProperty & 0x00000001 ) {
		
				if( pDevice->m_nCustomSoftwareWatchdogResetProperty & 0x80000000 ) { // WATCHDOG
		
					CX2581_I2C_START( pDevice, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x12, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); CX2581_I2C_STOP( pDevice, 125 );
		
					pDevice->m_nCustomSoftwareWatchdogResetProperty = 0x00000001;
				}
			}
			else {
		
				if( (counts++ % 5) == 0 ) { // HARDWARE RESET WATCHDOG

					CX2581_I2C_START( pDevice, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x12, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); CX2581_I2C_STOP( pDevice, 125 );
				}
			}
		}

		schedule_timeout_interruptible( msecs_to_jiffies( 1000 ) ); //
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] CX2581_OnControlPanelAnalysisThread( exit ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pDevice->m_pControlThread) );

	return 0;	
}

static int CX2581_StartControlPanelAnalysisThread( CDevice * pDevice )
{

	if( pDevice->m_pControlThread == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] CX2581_StartControlPanelAnalysisThread()\n", (int)(pDevice->m_nKsDeviceNumber) );

		pDevice->m_pControlThread = kthread_run( CX2581_OnControlPanelAnalysisThread, pDevice, "CX2581_OnControlPanelAnalysisThread" );

		if( IS_ERR( pDevice->m_pControlThread ) ) {

			int ret = PTR_ERR( pDevice->m_pControlThread );

			pDevice->m_pControlThread = NULL;

			return ret;
		}
	}
	return 0;
}

static int CX2581_StopControlPanelAnalysisThread( CDevice * pDevice )
{

	if( pDevice->m_pControlThread ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] CX2581_StopControlPanelAnalysisThread()\n", (int)(pDevice->m_nKsDeviceNumber) );

		kthread_stop( pDevice->m_pControlThread ); // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "STOP" SYNC.

		pDevice->m_pControlThread = NULL;
	}
	return 0;

}


BOOLEAN CX2581_HwUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_HwUnInitialize()\n", pDevice->m_nKsDeviceNumber );

	CX2581_StopControlPanelAnalysisThread( pDevice );

	return TRUE;
}

BOOLEAN CX2581_HwInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_HwInitialize()\n", pDevice->m_nKsDeviceNumber );

	{	ULONG R00050318 = 0x00000000;

		ULONG R00110088 = 0x00000000;

		ULONG R00110090 = 0x00000000;

		ULONG R00110098 = 0x00000000;

		ULONG R001100A0 = 0x00000000;

		ULONG R0011007C = 0x00000000;

		ULONG R00110078 = 0x00000000;

		ULONG R0011002C = 0x00000000;

		ULONG R00110068 = 0x00000000;

		ULONG R00110018 = 0x00000000;
		
		CX2581_SetRegister( pDevice, 0x00040000, 0x00000020, 4 );

		CX2581_SetRegister( pDevice, 0x00040010, 0x00007FFF, 4 );

		R00050318 = CX2581_GetRegister( pDevice, 0x00050318, 4 ) & (~0x00000010);

		CX2581_SetRegister( pDevice, 0x00050318, R00050318, 4 );
		//8K
/*
		CX2581_SetRegister( pDevice, 0x00110088, 0x1205BC28, 4 );

		CX2581_SetRegister( pDevice, 0x0011008C, 0x800001BF, 4 );

		R00110088 = CX2581_GetRegister( pDevice, 0x00110088, 4 ); R00110088 &= 0x7FFFFFFF;

		CX2581_SetRegister( pDevice, 0x00110088, R00110088, 4 );
*/
		//48K
		CX2581_SetRegister( pDevice, 0x00110088,  0x2931AE12, 4 );

		CX2581_SetRegister( pDevice, 0x0011008C, 0x80000198, 4 );

		R00110088 = CX2581_GetRegister( pDevice, 0x00110088, 4 ); R00110088 &= 0x7FFFFFFF;

		CX2581_SetRegister( pDevice, 0x00110088, R00110088, 4 );

		CX2581_SetRegister( pDevice, 0x00110090, 0x9883A86F, 4 );

		CX2581_SetRegister( pDevice, 0x00110094, 0x8000018D, 4 );

		R00110090 = CX2581_GetRegister( pDevice, 0x00110090, 4 ); R00110090 &= 0x7FFFFFFF;

		CX2581_SetRegister( pDevice, 0x00110090, R00110090, 4 );

		CX2581_SetRegister( pDevice, 0x00110098, 0x96A0EA3F, 4 );

		CX2581_SetRegister( pDevice, 0x0011009C, 0x80000103, 4 );

		R00110098 = CX2581_GetRegister( pDevice, 0x00110098, 4 ); R00110098 &= 0x7FFFFFFF;

		CX2581_SetRegister( pDevice, 0x00110098, R00110098, 4 );

 		CX2581_SetRegister( pDevice, 0x001100A0, 0x98757F5B, 4 );

		CX2581_SetRegister( pDevice, 0x001100A4, 0x80000113, 4 );

		R001100A0 = CX2581_GetRegister( pDevice, 0x001100A0, 4 ); R001100A0 &= 0x7FFFFFFF;

		CX2581_SetRegister( pDevice, 0x001100A0, R001100A0, 4 );

		R0011007C = CX2581_GetRegister( pDevice, 0x0011007C, 4 ); R0011007C &= 0x00FFFFFF; R0011007C |= 0x24000000;

		CX2581_SetRegister( pDevice, 0x0011007C, R0011007C, 4 );

		R00110078 = CX2581_GetRegister( pDevice, 0x00110078, 4 ); R00110078 |= 0x1B000000;

		CX2581_SetRegister( pDevice, 0x00110078, R00110078, 4 );

		R0011002C = CX2581_GetRegister( pDevice, 0x0011002C, 4 ); R0011002C |= 0x00004000; R0011002C &= ~(0x00001000 | 0x00002000);

		CX2581_SetRegister( pDevice, 0x0011002C, R0011002C, 4 );

		R00110068 = CX2581_GetRegister( pDevice, 0x00110068, 4 ); R00110068 &= 0xFFFFFF80; R00110068 |= 0x00000036;

		CX2581_SetRegister( pDevice, 0x00110068, R00110068, 4 );

		R00110018 = CX2581_GetRegister( pDevice, 0x00110018, 4 ); R00110018 &= 0xFFFFFFFE;

		CX2581_SetRegister( pDevice, 0x00110018, R00110018, 4 );

	}

	{	ULONG R000001A4 = 0x00000000;

		ULONG R0000011C = 0x00000000;

		ULONG R00000100 = 0x00000000;

		ULONG R00000164 = 0x00000000;

		ULONG R0000013C = 0x00000000;

		ULONG i = 0;

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R0000103C = CX2581_GetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x103C + (0x0200 * i)), 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x103C + (0x0200 * i)), R0000103C | 0x80000000, 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x11FC + (0x0200 * i), 0x00008000, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x11FC + (0x0200 * i), 0x00000000, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );

			R0000103C = CX2581_GetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x103C + (0x0200 * i)), 4 );

			CX2581_SetAnalogVideoDecoderRegister( pDevice, (USHORT)(0x103C + (0x0200 * i)), R0000103C | 0x80000000, 4 );
		}
		R000001A4 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x01A4, 4 ); R000001A4 &= 0x00FFFFFF;

	//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] R000001A4 = %08lX\n", pDevice->m_nKsDeviceNumber, R000001A4 );

		R0000011C = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x011C, 4 ); R0000011C &= 0xFFFF00DB;

		CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x011C, R0000011C, 4 );
		
		R00000100 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x0100, 4 ); R00000100 &= 0x00FFFFFF;

		CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x0100, R00000100, 4 );

		R00000164 = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x0164, 4 ); R00000164 &= 0x83FFFFFF; R00000164 |= 0x10000000;

		CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x0164, R00000164, 4 );

		R0000013C = CX2581_GetAnalogVideoDecoderRegister( pDevice, 0x013C, 4 ); R0000013C &= 0xFEF0FE00; R0000013C |= 0x010001FF;

		CX2581_SetAnalogVideoDecoderRegister( pDevice, 0x013C, R0000013C, 4 );

		CX2581_SetAnalogVideoDecoderStandardProperty( pDevice, NULL );

	}

	{	ULONG i = pDevice->m_nKsDeviceNumber * 8;
	
		ULONG R00110018 = 0x00000000;

		ULONG R0011001C = 0x00000000;

		ULONG R00110010 = 0x00000000;

		ULONG R00110014 = 0x00000000;

		R00110018 = CX2581_GetRegister( pDevice, 0x00110018, 4 ); R00110018 &= 0x000000FF; R00110018 |= (((~g_n_gpio_direction[ i ]) & 0x00FFFFFF) << ( 8));

		CX2581_SetRegister( pDevice, 0x00110018, R00110018, 4 );

		R0011001C = CX2581_GetRegister( pDevice, 0x0011001C, 4 ); R0011001C &= 0xFFFFFF00; R0011001C |= (((~g_n_gpio_direction[ i ]) & 0xFF000000) >> (24));

		CX2581_SetRegister( pDevice, 0x0011001C, R0011001C, 4 );

		R00110010 = CX2581_GetRegister( pDevice, 0x00110010, 4 ); R00110010 &= 0x000000FF; R00110010 |= ((g_n_gpio_data[ i ] & 0x00FFFFFF) << ( 8));

		CX2581_SetRegister( pDevice, 0x00110010, R00110010, 4 );

		R00110014 = CX2581_GetRegister( pDevice, 0x00110014, 4 ); R00110014 &= 0xFFFFFF00; R00110014 |= ((g_n_gpio_data[ i ] & 0xFF000000) >> (24));

		CX2581_SetRegister( pDevice, 0x00110014, R00110014, 4 );

	}

	{	ULONGLONG delay = 10;

		struct timeval ts; do_gettimeofday( &ts );

		{	BYTE codes[ 4 ] = { ((ts.tv_usec / 1) % 0xFF), ((ts.tv_usec / 10) % 0xFF), ((ts.tv_usec / 100) % 0xFF), ((ts.tv_usec / 1000) % 0xFF) };

			BYTE keys[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };
	
			CX2581_I2C_START( pDevice, delay ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA2, delay ); CX2581_I2C_WRITE_BYTE( pDevice, 0x13, delay ); CX2581_I2C_WRITE_BYTE( pDevice, codes[ 0 ], delay ); CX2581_I2C_WRITE_BYTE( pDevice, codes[ 1 ], delay ); CX2581_I2C_WRITE_BYTE( pDevice, codes[ 2 ], delay ); CX2581_I2C_WRITE_BYTE( pDevice, codes[ 3 ], delay ); CX2581_I2C_STOP( pDevice, delay );
	
			schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	
			CX2581_I2C_START( pDevice, delay ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA3, delay ); keys[ 0 ] = CX2581_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 1 ] = CX2581_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 2 ] = CX2581_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 3 ] = CX2581_I2C_READ_BYTE( pDevice, 0, delay ); CX2581_I2C_STOP( pDevice, delay );
	
			if( (*((DWORD *)(keys)) != 0x00000000) && (*((DWORD *)(keys)) != 0xFFFFFFFF) ) {
	
				g_copy_protect_unlock_boradsA[ 0 ] = 1; 
				
				g_copy_protect_unlock_boradsB[ 0 ] = 1;

			}
		}
	}

	//
	//shawn 2010.08.11
	//
	// 
	if( pDevice->m_nKsDeviceNumber == 0 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ){

				BYTE R11 = (BYTE)(pDevice->m_nCustomSoftwareWatchdogDurationProperty);
				
				CX2581_I2C_START( pDevice, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x11, 125 ); CX2581_I2C_WRITE_BYTE( pDevice,  R11, 125 ); CX2581_I2C_STOP( pDevice, 125 ); //

				CX2581_I2C_START( pDevice, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x12, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); CX2581_I2C_STOP( pDevice, 125 );

				CX2581_I2C_START( pDevice, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0xA2, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x10, 125 ); CX2581_I2C_WRITE_BYTE( pDevice, 0x5A, 125 ); CX2581_I2C_STOP( pDevice, 125 );
		}

		CX2581_StartControlPanelAnalysisThread( pDevice );
	}

	return CX2581_PowerDownPeripherals( pDevice );
}

BOOLEAN CX2581_PowerDownPeripherals( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_PowerDownPeripherals()\n", pDevice->m_nKsDeviceNumber );

	return TRUE;
}

BOOLEAN CX2581_HwProcessAnalogPCIVideoPacket( CDevice *pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	DWORD R00130020 = 0x00000000;

	ULONG y = 0;

	for( y = 0 ; y < 8 ; y++ ) {

		CVideo *       pVideo = pVideos[ y ];
		
		CVideoBuffer * pVideoBuffer = pVideoBuffers[ y ];
		
		dma_addr_t     pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
		
		dma_addr_t     pDmaBaseCommonPhysicalJumpBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
		
		DWORD *        pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);
		
		DWORD *        pDmaBaseCommonJumpBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);
		
		BOOLEAN        is_complete = FALSE;
		
		BOOLEAN        is_copy = FALSE;
		
		BOOLEAN        is_reset = FALSE;
		
		pDmaBaseCommonPhysicalStartBuffer += (7020 * 2) * (y) / (1);

		pDmaBaseCommonPhysicalJumpBuffer  += (7020 * 2) * (y) / (1);

		pDmaBaseCommonStartBuffer         += (7020 * 2) * (y) / (4);

		pDmaBaseCommonJumpBuffer          += (7020 * 2) * (y) / (4);

		if( (y == 0) && ((status & 0x00000001) == 0x00000000) ) { continue ; }
		
		if( (y == 1) && ((status & 0x00000002) == 0x00000000) ) { continue ; }
		
		if( (y == 2) && ((status & 0x00000004) == 0x00000000) ) { continue ; }
		
		if( (y == 3) && ((status & 0x00000008) == 0x00000000) ) { continue ; }
		
		if( (y == 4) && ((status & 0x00000010) == 0x00000000) ) { continue ; }
		
		if( (y == 5) && ((status & 0x00000020) == 0x00000000) ) { continue ; }
		
		if( (y == 6) && ((status & 0x00000040) == 0x00000000) ) { continue ; }
		
		if( (y == 7) && ((status & 0x00000080) == 0x00000000) ) { continue ; }

		if( (y == 0) && ((status & 0x00000001) == 0x00000001) ) { ; }
		
		if( (y == 1) && ((status & 0x00000002) == 0x00000002) ) { ; }
		
		if( (y == 2) && ((status & 0x00000004) == 0x00000004) ) { ; }
		
		if( (y == 3) && ((status & 0x00000008) == 0x00000008) ) { ; }
		
		if( (y == 4) && ((status & 0x00000010) == 0x00000010) ) { ; }
		
		if( (y == 5) && ((status & 0x00000020) == 0x00000020) ) { ; }
		
		if( (y == 6) && ((status & 0x00000040) == 0x00000040) ) { ; }
		
		if( (y == 7) && ((status & 0x00000080) == 0x00000080) ) { ; }

		//
		//
		R00130020 = CX2581_GetRegister( pDevice, 0x00130020 + (0x00000100 * y), 4 ); 
		
		if( pVideo ) {

			if( pVideo->m_nSrcFrameHeight > 288 ) { //

				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 0 + 80 * 1); pDmaBaseCommonStartBuffer += (6940 * 0 + 80 * 1) >> 2; is_copy = 1; is_complete = 0; is_reset = 1; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 1 + 80 * 2); pDmaBaseCommonStartBuffer += (6940 * 1 + 80 * 2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }

				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 1 + 80 * 1); pDmaBaseCommonJumpBuffer  += (6940 * 1 + 80 * 1) >> 2; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 0 + 80 * 0); pDmaBaseCommonJumpBuffer  += (6940 * 0 + 80 * 0) >> 2; } //
			}
			//shawn 2010.10.25
			//
/*	
			if( pVideo->m_nSrcFrameWidth > 360 && pVideo->m_nSrcFrameHeight <= 288 ) { //

				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 1 + 80 * 2); pDmaBaseCommonStartBuffer += (6940 * 1 + 80 * 2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 0 + 80 * 1); pDmaBaseCommonStartBuffer += (6940 * 0 + 80 * 1) >> 2; is_copy = 1; is_complete = 1; is_reset = 1; }
		
				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 0 + 80 * 0); pDmaBaseCommonJumpBuffer  += (6940 * 0 + 80 * 0) >> 2; } //
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 1 + 80 * 1); pDmaBaseCommonJumpBuffer  += (6940 * 1 + 80 * 1) >> 2; }
			}
			else 
*/			
			if( pVideo->m_nSrcFrameHeight <= 288 ) { //
			//
				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 0 + 80 * 1); pDmaBaseCommonStartBuffer += (6940 * 0 + 80 * 1) >> 2; is_copy = 1; is_complete = 1; is_reset = 1; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 1 + 80 * 2); pDmaBaseCommonStartBuffer += (6940 * 1 + 80 * 2) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }
		
				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 1 + 80 * 1); pDmaBaseCommonJumpBuffer  += (6940 * 1 + 80 * 1) >> 2; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 0 + 80 * 0); pDmaBaseCommonJumpBuffer  += (6940 * 0 + 80 * 0) >> 2; } //
			}
		}
		else {

			if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 0 + 80 * 1); pDmaBaseCommonStartBuffer += (6940 * 0 + 80 * 1) >> 2; is_copy = 0; is_complete = 0; is_reset = 1; }
			if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 1 + 80 * 2); pDmaBaseCommonStartBuffer += (6940 * 1 + 80 * 2) >> 2; is_copy = 0; is_complete = 0; is_reset = 0; }

			if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 1 + 80 * 1); pDmaBaseCommonJumpBuffer  += (6940 * 1 + 80 * 1) >> 2; }
			if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 0 + 80 * 0); pDmaBaseCommonJumpBuffer  += (6940 * 0 + 80 * 0) >> 2; } //
		}
		//
		//
		if( pVideo ) {

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
		}
		if( pVideo && pVideoBuffer && is_copy ) {

#ifdef CX2581_64bit

			ULONG physical_address = pDevice->m_CX2581_video_buffer_physical_address[ y ][ pDevice->m_CX2581_video_buffer_count[ y ] % CX2581_MAX_BUFFER ];
			
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

			//LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_HwProcessAnalogPCIVideoPacket() lines(0x%x)\n", pDevice->m_nKsDeviceNumber, lines );

			for( i = 0 ; i < lines ; i++ ) {

				ULONG remains = 0;

				ULONG start = 0;

				//
				//
				if( pVideo->m_nSrcFrameHeight > 288 ) { //

					if( (R00130020 & 0x0000FFFF) == 0x00000001 && (i == 0) ) { 

						if( pVideo->m_nSrcFrameBitCount == 12 ) { remains = (pitch) * (3) / (2); }

						if( pVideo->m_nSrcFrameBitCount == 16 ) { remains = (pitch) << 1; }

						while( remains ) {

							if( byte_count > remains ) {

								physical_address += remains;

								byte_count -= remains;

								remains -= remains;
							}
							else {

								remains -= byte_count;

								#ifndef CX2581_64bit
								p_sglist++;
								#endif

								count++;

								if( count < mapping_count ) {

									#ifdef CX2581_64bit
									
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
				if( pVideo->m_nSrcFrameBitCount == 12 ) { remains = (pVideo->m_nSrcFrameWidth) * (3) / (2); }
				
				if( pVideo->m_nSrcFrameBitCount == 16 ) { remains = (pVideo->m_nSrcFrameWidth) << 1; }

				start = 0;

				while( remains ) {

					if( byte_count > remains ) {

						if( pVideo->m_nSrcFrameBitCount == 12 ) { 

							switch( pVideo->m_nSrcFrameWidth ) {

							case 720: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains == 1080) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 360: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  540) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 704: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains == 1056) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 352: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  528) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 640: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  960) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 320: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  480) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							}
						}
						if( pVideo->m_nSrcFrameBitCount == 16 ) { 

							switch( pVideo->m_nSrcFrameWidth ) {

							case 720: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains == 1440) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 360: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  720) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 704: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains == 1408) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 352: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  704) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 640: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains == 1280) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 320: *pDmaBaseCommonStartBuffer++ = remains + ((start == 0) ? ((remains ==  640) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							}
						}
					   *pDmaBaseCommonStartBuffer++ = physical_address >>  0; //

				    // *pDmaBaseCommonStartBuffer++ = physical_address >> 32; //

					   *pDmaBaseCommonStartBuffer++ = 0x00000000;

						physical_address += remains;

						byte_count -= remains;

						remains -= remains;

						start = 0;
					}
					else {

						if( pVideo->m_nSrcFrameBitCount == 12 ) { 

							switch( pVideo->m_nSrcFrameWidth ) {

							case 720: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count == 1080) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 360: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  540) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 704: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count == 1056) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 352: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  528) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 640: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  960) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 320: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  480) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							}
						}
						if( pVideo->m_nSrcFrameBitCount == 16 ) { 

							switch( pVideo->m_nSrcFrameWidth ) {

							case 720: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count == 1440) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 360: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  720) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 704: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count == 1408) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 352: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  704) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 640: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count == 1280) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							case 320: *pDmaBaseCommonStartBuffer++ = byte_count + ((start == 0) ? ((byte_count ==  640) ? 0x1C000000 : 0x18000000) : (0x14000000)); break;
							}
						}
					   *pDmaBaseCommonStartBuffer++ = physical_address >>  0; //

				    // *pDmaBaseCommonStartBuffer++ = physical_address >> 32; //

					   *pDmaBaseCommonStartBuffer++ = 0x00000000;

						remains -= byte_count;

						start += byte_count;

						#ifndef CX2581_64bit
						p_sglist++;
						#endif
						count++;

						if( count < mapping_count ) {

							#ifdef CX2581_64bit
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
				if( pVideo->m_nSrcFrameHeight > 288 ) { //

					if( pVideo->m_nSrcFrameBitCount == 12 ) { remains = (pitch * 2 - pVideo->m_nSrcFrameWidth) * (3) / (2); }

					if( pVideo->m_nSrcFrameBitCount == 16 ) { remains = (pitch * 2 - pVideo->m_nSrcFrameWidth) << 1; }
				}
				else { //

					if( pVideo->m_nSrcFrameBitCount == 12 ) { remains = (pitch * 1 - pVideo->m_nSrcFrameWidth) * (3) / (2); }

					if( pVideo->m_nSrcFrameBitCount == 16 ) { remains = (pitch * 1 - pVideo->m_nSrcFrameWidth) << 1; }
				}

				while( remains ) {

					if( byte_count > remains ) {

						physical_address += remains;

						byte_count -= remains;

						remains -= remains;
					}
					else {

						remains -= byte_count;

						#ifndef CX2581_64bit
						p_sglist++;
						#endif

						count++;

						if( count < mapping_count ) {

							#ifdef CX2581_64bit
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

LOOP_END_LINES:

			if( is_complete ) { 
			
				//
				//
				if( pVideo && pVideoBuffer ) {

					if( pVideo->m_nSrcFrameHeight > 288 ) {

						if( pVideoBuffer->m_sKsBuffer.input & 0x80000000 ) {

							p_is_drops[ y ] = TRUE;
						}
					}
					//SHAWN 2010.10.25
					//
					else{
						if( pVideo->m_nSrcFrameWidth > 360 ){
							if( is_complete == 1 && is_reset == 0 )
								pVideoBuffer->m_sKsBuffer.field = V4L2_FIELD_TOP;	
							else
								pVideoBuffer->m_sKsBuffer.field = V4L2_FIELD_BOTTOM;	
						}
						else {
							pVideoBuffer->m_sKsBuffer.field = V4L2_FIELD_TOP;	
						}
					}
					//
					pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //
				}
				p_is_completes[ y ] = TRUE; 
			}
		}
		else {

			if( is_complete ) { 
				
				//
				//
				if( pVideo && pVideoBuffer ) {

					if( pVideo->m_nSrcFrameHeight > 288 ) {

						if( pVideoBuffer->m_sKsBuffer.input & 0x80000000 ) {

							p_is_drops[ y ] = TRUE;
						}
					}
					pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //
				}
				p_is_completes[ y ] = TRUE; 
			}

		   *pDmaBaseCommonStartBuffer++ = 0x2C0005A0;
		}
		if( is_reset ) {

		   *pDmaBaseCommonStartBuffer++ = 0x71030000;

		   *pDmaBaseCommonStartBuffer++ = pDmaBaseCommonPhysicalJumpBuffer >>  0; //

	    // *pDmaBaseCommonStartBuffer++ = pDmaBaseCommonPhysicalJumpBuffer >> 32; //

		   *pDmaBaseCommonStartBuffer++ = 0x00000000;
		}
		else {

		   *pDmaBaseCommonStartBuffer++ = 0x71010000;

		   *pDmaBaseCommonStartBuffer++ = pDmaBaseCommonPhysicalJumpBuffer >>  0; //

		// *pDmaBaseCommonStartBuffer++ = pDmaBaseCommonPhysicalJumpBuffer >> 32; //

		   *pDmaBaseCommonStartBuffer++ = 0x00000000;
		}

		#ifdef CX2581_64bit

		if( p_is_completes[ y ] == TRUE ) {

			if( pVideoBuffer )
			{
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

				BYTE* pe = videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer ); //NOTE: should use videobuf_queue_vmalloc_init, not videobuf_queue_sg_init
				
				pDevice->m_CX2581_video_buffer_count[ y ] = (pDevice->m_CX2581_video_buffer_count[ y ] + 1) % CX2581_MAX_BUFFER;

				BYTE* po = pDevice->m_CX2581_video_buffer[ y ][ pDevice->m_CX2581_video_buffer_count[y] ]; 

				if( pe == NULL ) { LINUXV4L2_DEBUG( KERN_INFO, "can not allocate pe()\n" ); return TRUE; }

				ULONG cxe = pVideo->m_nDstFrameWidth;

				ULONG cye = pVideo->m_nDstFrameHeight;

				if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
				{
					memcpy( pe, po, (cxe * cye) * 2 ); 
				}
				
				//LINUXV4L2_DEBUG( KERN_INFO, "copy(%d) y(0d%d)\n", (cxe * cye) * 2, y );

				//if vlc, enable below
				pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //VLC

				#else
			
				struct videobuf_dmabuf * pKsDMABuffer = &(pVideoBuffer->m_sKsBuffer.dma);

				struct scatterlist * p_sglist = (struct scatterlist *)(pKsDMABuffer->sglist);

				ULONG  mapping_count = pKsDMABuffer->sglen;

				ULONG  pages = 0;

				ULONG  byte_count = sg_dma_len( p_sglist );

				ULONG  remains = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 2;

				pDevice->m_CX2581_video_buffer_count[ y ] = (pDevice->m_CX2581_video_buffer_count[ y ] + 1) % CX2581_MAX_BUFFER;

				BYTE* po = pDevice->m_CX2581_video_buffer[ y ][ pDevice->m_CX2581_video_buffer_count[y] ]; 

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
	}	

	return TRUE;
}

BOOLEAN CX2581_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	ULONG i = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_HwProcessAnalogPCIAudioPacket( %08X:%08X )\n", pDevice->m_nKsDeviceNumber, status, mask );

	for( i = 0 ; i < 8 ; i++ ) {

		ULONG R00140010 = 0x00000000;

		if( (i == 0) && ((status & 0x00000100) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 0), 4 ); }
		
		if( (i == 1) && ((status & 0x00000100) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 0), 4 ); }
		
		if( (i == 2) && ((status & 0x00000200) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 1), 4 ); }
		
		if( (i == 3) && ((status & 0x00000200) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 1), 4 ); }
		
		if( (i == 4) && ((status & 0x00000400) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 2), 4 ); }
		
		if( (i == 5) && ((status & 0x00000400) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 2), 4 ); }
		
		if( (i == 6) && ((status & 0x00000800) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 3), 4 ); }

		if( (i == 7) && ((status & 0x00000800) == 0x00000000) ) { continue ; } else { R00140010 = CX2581_GetRegister( pDevice, 0x00140010 + (0x00000100 * 3), 4 ); }

		if( pAudios[ i ] ) {

			CAudio * pAudio = pAudios[ i ];

			BOOLEAN is_complete = FALSE;

			BOOLEAN is_copy = FALSE;

			BYTE *  po = NULL;

			BYTE *  pe = pAudio->m_pKsSubStream->runtime->dma_area;

			ULONG   sz = pAudio->m_nFrameSize;

			ULONG	j = 0;

			//for odd channel (ch1, ch3, ch5, ch7),  DMA + 2

			if( (R00140010 & 0x000000FF) == 0x00000000 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 0 + (i % 2) * 2; is_copy = TRUE; is_complete = TRUE; }
			
			if( (R00140010 & 0x000000FF) == 0x00000001 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 1 + (i % 2) * 2; is_copy = TRUE; is_complete = TRUE; }
			
			if( (R00140010 & 0x000000FF) == 0x00000002 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 2 + (i % 2) * 2; is_copy = TRUE; is_complete = TRUE; }
			
			if( (R00140010 & 0x000000FF) == 0x00000003 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 3 + (i % 2) * 2; is_copy = TRUE; is_complete = TRUE; }

			if( is_copy ) {

			if( pe ) {

				BYTE * pe_ptr = (BYTE *)(pe + pAudio->m_nFrameNumber * pAudio->m_nFrameSize);

				BYTE * po_ptr = (BYTE *)(po);

				for( j = 0 ; j < sz ; j += 2 ) {

				    pe_ptr[ 0 ] = *po_ptr++;

				    pe_ptr[ 1 ] = *po_ptr++;
					
					po_ptr++;

					po_ptr++;

					pe_ptr++;

					pe_ptr++;
				}
				pAudio->m_nFrameNumber = (pAudio->m_nFrameNumber + 1) % (pAudio->m_nFrames);

				snd_pcm_period_elapsed( pAudio->m_pKsSubStream );
			}
			}
			p_is_completes[ i ] = is_complete;
		}
	}
	return TRUE;
}
