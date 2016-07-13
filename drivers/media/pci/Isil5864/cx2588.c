#include "LINUXV4L2.h"
#include "property.h"
#include "cx2588.h"

ULONG CX2588_GetRegister( CDevice * pDevice, ULONG dwIndex, ULONG nBytes )
{
	if( nBytes == 4 ) { return readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0xFFFFFFFF; }

	if( nBytes == 2 ) { return readw( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x0000FFFF; }

	if( nBytes == 1 ) { return readb( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x000000FF; }

	return 0x00000000;
}

BOOLEAN CX2588_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue, ULONG nBytes )
{
	if( nBytes == 4 ) { writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 2 ) { writew( (dwValue & 0x0000FFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

	if( nBytes == 1 ) { writeb( (dwValue & 0x000000FF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

//	LINUXV4L2_DEBUG( KERN_INFO, " WRITE_REGISTER_ULONG_PRINT: register(0x%08X) = 0x%08X \n", dwIndex, dwValue );

	return TRUE;
}

DWORD CX2588_GetAnalogVideoDecoderRegister( CDevice * pDevice, USHORT wIndex, ULONG nBytes )
{
	BYTE  pTransferBuffer[ 12 ];

	ULONG status = 0x00000000;

	DWORD returns = 0x00000000;

	ULONG i = 0; 

	if( nBytes == 1 ) { pTransferBuffer[  0 ] = (BYTE)(wIndex >> 0);
						pTransferBuffer[  1 ] = (BYTE)(wIndex >> 8);
						pTransferBuffer[  2 ] = (0x00);
						pTransferBuffer[  3 ] = (0x60);					// I2C DEVICE ADDRESS
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
						pTransferBuffer[  3 ] = (0x60);					// I2C DEVICE ADDRESS
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

	writel( *((DWORD *)(pTransferBuffer + 0)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0000) );

	writel( *((DWORD *)(pTransferBuffer + 8)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0008) );

	for( i = 0 ; i < 20 ; i++ ) {

		schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

		status = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0010) );

		if( (status & 0x00000002) == 0x00000000 ) { break; }
	}
	if( (status & 0x00000002) == 0x00000002 ||

		(status & 0x00000001) == 0x00000000 ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_GetAnalogVideoDecoderRegister(): GET REGISTER %04X RROR!!\n", pDevice->m_nKsDeviceNumber, wIndex );

		up( &(pDevice->m_sI2CAccessSemaphore) );

		return 0x00000000;
	}
	returns = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A000C) ) & ((nBytes == 1) ? 0x000000FF : 0xFFFFFFFF);

	up( &(pDevice->m_sI2CAccessSemaphore) );

	return returns;
}

BOOLEAN CX2588_SetAnalogVideoDecoderRegister( CDevice * pDevice, USHORT wIndex, DWORD dwValue, ULONG nBytes )
{
	BYTE  pTransferBuffer[ 12 ];

	ULONG status = 0x00000000;

	ULONG i = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, " WRITE_REGISTER_UCHAR_PRINT: register(0x%08X) = 0x%08X \n", wIndex, dwValue );

	if( nBytes == 1 ) { pTransferBuffer[  0 ] = (0x00); 
						pTransferBuffer[  1 ] = (0x00); 
						pTransferBuffer[  2 ] = (0x00); 
						pTransferBuffer[  3 ] = (0x60);					// I2C DEVICE ADDRESS
						pTransferBuffer[  4 ] = (BYTE)(wIndex  >>  8);	// I2C SUB-DEVICE ADDRESS
						pTransferBuffer[  5 ] = (BYTE)(wIndex  >>  0);	// I2C SUB-DEVICE ADDRESS
						pTransferBuffer[  6 ] = (BYTE)(dwValue >>  0);
						pTransferBuffer[  7 ] = (0x00);
						pTransferBuffer[  8 ] = (0x04);
						pTransferBuffer[  9 ] = (0x30);
						pTransferBuffer[ 10 ] = (0x00);
						pTransferBuffer[ 11 ] = (0x07);
	}
	if( nBytes == 4 ) { pTransferBuffer[  0 ] = (BYTE)(dwValue >>  0);	// I2C DATA 1st BYTE
						pTransferBuffer[  1 ] = (BYTE)(wIndex  >>  0);	// I2C SUB-DEVICE ADDRESS
						pTransferBuffer[  2 ] = (BYTE)(wIndex  >>  8);	// I2C SUB-DEVICE ADDRESS
						pTransferBuffer[  3 ] = (0x60);					// I2C DEVICE ADDRESS
						pTransferBuffer[  4 ] = (BYTE)(dwValue >>  8);	// I2C DATA 2nd BYTE
						pTransferBuffer[  5 ] = (BYTE)(dwValue >> 16);	// I2C DATA 3rd BYTE
						pTransferBuffer[  6 ] = (BYTE)(dwValue >> 24);	// I2C DATA 4th BYTE
						pTransferBuffer[  7 ] = (0x00);
						pTransferBuffer[  8 ] = (0x04);
						pTransferBuffer[  9 ] = (0x33);
						pTransferBuffer[ 10 ] = (0x00);
						pTransferBuffer[ 11 ] = (0x07);
	}
	
	down( &(pDevice->m_sI2CAccessSemaphore) );
	
	writel( *((DWORD *)(pTransferBuffer + 0)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0000) );

	writel( *((DWORD *)(pTransferBuffer + 4)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0004) );

	writel( *((DWORD *)(pTransferBuffer + 8)), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0008) );

	for( i = 0 ; i < 20 ; i++ ) {

		schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

		status = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x001A0010) );

		if( (status & 0x00000002) == 0x00000000 ) { break; }
	}
	up( &(pDevice->m_sI2CAccessSemaphore) );

	if( (status & 0x00000002) == 0x00000002 ||

		(status & 0x00000001) == 0x00000000 ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_SetAnalogVideoDecoderRegister(): SET REGISTER %04X กั %08X ERROR!!\n", pDevice->m_nKsDeviceNumber, wIndex, (unsigned int)(dwValue) );

		return FALSE;
	}
	return TRUE;
}

BOOLEAN CX2588_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, CVideo * pVideo )
{
//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_SetAnalogVideoDecoderProcAmpProperties()\n", pDevice->m_nKsDeviceNumber );

	ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1;

	ULONG R00121014 = 0x00000000;

	ULONG R00121018 = 0x00000000;

	if( g_n_analog_decoder_brightness[ i ][ 0 ] & 0x80000000 ) { 

		R00121014 = CX2588_GetRegister( pDevice, 0x00121014 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ); R00121014 &= 0xFFFFFF00;

		R00121014 |= (BYTE)(((g_n_analog_decoder_brightness[ i ][ 0 ] & 0xFF) >> 0) - 128) | 0x00 ;

		CX2588_SetRegister( pDevice, 0x00121014 + (0x0200 * pVideo->m_nSubDeviceNumber), R00121014, 4 ); 
		
		g_n_analog_decoder_brightness[ i ][ 0 ] &= 0x7FFFFFFF; 

		LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_brightness[ %ld ][ 0 ]=0x%08lX \n", i, CX2588_GetRegister( pDevice, 0x00121014 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ) );
	}

	if( g_n_analog_decoder_contrast[ i ][ 0 ] & 0x80000000 ) { 
		
		R00121014 = CX2588_GetRegister( pDevice, 0x00121014 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ); R00121014 &= 0xFFFF00FF;

		R00121014 |= (BYTE)(((g_n_analog_decoder_contrast[ i ][ 0 ]  & 0xFF) >> 0) - 0 ) << 8;

		CX2588_SetRegister( pDevice, 0x00121014 + (0x0200 * pVideo->m_nSubDeviceNumber), R00121014, 4 ); 
		
		g_n_analog_decoder_contrast[ i ][ 0 ] &= 0x7FFFFFFF; 

		LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_contrast[ %ld ][ 0 ]=0x%08lX \n", i, CX2588_GetRegister( pDevice, 0x00121014 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ) );
	}
	
	if( g_n_analog_decoder_saturation[ i ][ 0 ] & 0x80000000 ) { 

		R00121018 = CX2588_GetRegister( pDevice, 0x00121018 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ); R00121018 &= 0xFFFF0000;

		R00121018 |= (BYTE)(((g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) >> 0) - 0) << 0;

		R00121018 |= (BYTE)(((g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) >> 0) - 0) << 8;
		
		CX2588_SetRegister( pDevice, 0x00121018 + (0x0200 * pVideo->m_nSubDeviceNumber), R00121018, 4 );
		
		g_n_analog_decoder_saturation[ i ][ 0 ] &= 0x7FFFFFFF; 

		LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_saturation[ %ld ][ 0 ]=0x%08lX \n", i, CX2588_GetRegister( pDevice, 0x00121018 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ) );
	
	}
	
	if( g_n_analog_decoder_hue[ i ][ 0 ] & 0x80000000 ) { 
		
		R00121018 = CX2588_GetRegister( pDevice, 0x00121018 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ); R00121018 &= 0xFF00FFFF;

		R00121018 |= (BYTE)(((g_n_analog_decoder_hue[ i ][ 0 ] & 0xFF) >> 0) - 128) << 16;

		CX2588_SetRegister( pDevice, 0x00121018 + (0x0200 * pVideo->m_nSubDeviceNumber), R00121018, 4 ); 
		
		g_n_analog_decoder_hue[ i ][ 0 ] &= 0x7FFFFFFF; 

		LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_hue[ %ld ][ 0 ]=0x%08lX \n", i, CX2588_GetRegister( pDevice, 0x00121018 + (0x0200 * pVideo->m_nSubDeviceNumber), 4 ) );
	
	}

	if( g_n_analog_decoder_sharpness[ i ][ 0 ]  & 0x80000000 ) {
	
	//	These are in order from least sharp to most sharp
		BYTE sharpness_mappings[5] =
		{
			0x00,   // sharpness enhancement (aka luma peaking) disabled
			0x04,   // +2.0dB response @ center freq
			0x05,   // +3.5dB response @ center freq
			0x06,   // +5.0dB response @ center freq
			0x07    // +6.0dB response @ center freq
		};

		BYTE sharp_value = 0x00;

		BYTE index = 0x00;

		sharp_value = CX2588_GetRegister( pDevice, 0x00121016, 1);

		sharp_value &= ~0x07;
	
		index = g_n_analog_decoder_sharpness[ i ][ 0 ] / 51;

		if( index > 4 ) { index = 4; }

		sharp_value |= sharpness_mappings[ index ];

		CX2588_SetRegister( pDevice, 0x00121016, sharp_value, 1);
	
		g_n_analog_decoder_sharpness[ i ][ 0 ]  &= 0x7FFFFFFF; 
	
	}

//	{	ULONG R00121010 = CX2588_GetRegister( pDevice, (USHORT)(0x00121010 + (0x0200 * pVideo->m_nSubDeviceNumber)), 1 ); 
//
//		pVideo->m_nLockStatus = (R00121010 & 0x00000010) ? (1) : (0);
//	}

	return TRUE;
}

BOOLEAN CX2588_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, CVideo * pVideo )
{
	ULONG e_v4l2_std = pDevice->m_nCustomVideoStandardProperty;

	ULONG cx = (pVideo) ? (pVideo->m_nSrcFrameWidth) : (720);

	ULONG i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_SetAnalogVideoDecoderStandardProperty( S:%08lX, cx=%ld )\n", pDevice->m_nKsDeviceNumber, e_v4l2_std, cx );

	if( e_v4l2_std & V4L2_STD_525_60 ) {

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00121000 = 0x00000000;

			ULONG R00121044 = 0x00000000;

			ULONG R00121018 = 0x00000000;
			
			ULONG R00121088 = 0x00000000;

			ULONG R00121020 = 0x00000000;

			ULONG R00121028 = 0x00000000;

			BYTE R00121004 = 0x00;

			BYTE R00121008 = 0x00;

			ULONG R00121024 = 0x00;

			if( pVideo ) { 
				
				if( pVideo->m_nSubDeviceNumber != i ) { 
					
					continue ; 
				} 
			}

			// SOFT RESET
			CX2588_SetRegister( pDevice, 0x001211FC + (0x0200 * i), 0x00008000, 4 );

			schedule_timeout_interruptible( msecs_to_jiffies( 20 ) );

			CX2588_SetRegister( pDevice, 0x001211FC + (0x0200 * i), 0x00000000, 4 );

			//initialize_NTSC()

			// set video format NTSC-M		
			R00121000 = CX2588_GetRegister( pDevice, 0x00121000 + (0x0200 * i), 4 ); R00121000 &= 0xFFFFFFF0; R00121000 |= 0x00010001;

			CX2588_SetRegister( pDevice, 0x00121000 + (0x0200 * i), R00121000, 4 );

			// Disable clamp gate
			R00121044 = CX2588_GetRegister( pDevice, 0x00121044 + (0x0200 * i), 4 ); R00121044 |= 0x40000000;

			CX2588_SetRegister( pDevice, 0x00121044 + (0x0200 * i), R00121044, 4 );
			
			// Set luma range = 1 ( luma range = 4:1016 )
			CX2588_SetRegister( pDevice, 0x00121014 + (0x0200 * i), 0x00108000, 4 );
       
			// Set c_ext_rng_en = 1 (CrCb range = 4-252 (8b).), set c_core_sel = 1 to improve flat field SNR.
			R00121018 = CX2588_GetRegister( pDevice, 0x00121018 + (0x0200 * i), 4 ); R00121018 |= 0x21000000;

			CX2588_SetRegister( pDevice, 0x00121018 + (0x0200 * i), R00121018, 4 );

			// Set to 10-bit output, id_ins_extend_en = 0x1
			R00121004 = CX2588_GetRegister( pDevice, 0x00121004 + (0x0200 * i), 1 ); R00121004 |= 0x14;      

			CX2588_SetRegister( pDevice, 0x00121004 + (0x0200 * i), R00121004, 1 );

			// Set to 10-bit output, id_ins_extend_en_ns = 0x1
			R00121008 = CX2588_GetRegister( pDevice, 0x00121008 + (0x0200 * i), 1 ); R00121008 |= 0x14;      

			CX2588_SetRegister( pDevice, 0x00121008 + (0x0200 * i), R00121008, 1 );

			// Change field_phase_limit from 0xC to 0xB. It makes vcr_detect bit more reliable
			R00121088 = CX2588_GetRegister( pDevice, 0x00121088 + (0x0200 * i), 4 ); R00121088 &= 0xFFFFFF0F; R00121088 |= 0xB0;    

			CX2588_SetRegister( pDevice, 0x00121088 + (0x0200 * i), R00121088, 4);

			// Set htl_kd = 0x20, htl_ki = 0x40
			CX2588_SetRegister( pDevice, 0x00121054 + (0x0200 * i), 0x00002040, 4 );
 
			// Increase sync_tip_reduce
			CX2588_SetRegister( pDevice, 0x0012101C + (0x0200 * i), 0x001C1E0F, 4 );

			// Default value for NTSC, sc_step=0x43E00000
			CX2588_SetRegister( pDevice, 0x0012105C + (0x0200 * i), 0x43E00000, 4 );

			// Default value for NTSC, bgdel=0x61,hactive=0x2d0, hblank=0x074
			R00121020 = CX2588_GetRegister( pDevice, 0x00121020 + (0x200 * i), 4 ); R00121020 &= 0x00C00C00; R00121020 |= 0x5E2D0074;

			CX2588_SetRegister( pDevice, 0x00121020 + (0x0200 * i), 0x5E2D0074, 4 );

			R00121024 = CX2588_GetRegister( pDevice, 0x00121024 + (0x0200 * i), 4 ); R00121024 &= 0x00C00C00; R00121024 |= 0x1E1E001A; // vblank_cnt + 2 to get camera ID
			
			CX2588_SetRegister( pDevice, 0x00121024 + (0x0200 * i), R00121024, 4 );

			// COMB FILTER SETTINGS BELOW
			CX2588_SetRegister( pDevice, 0x00121068 + (0x0200 * i), 0x00001453, 4 );
			CX2588_SetRegister( pDevice, 0x0012106C + (0x0200 * i), 0x00001453, 4 );
			CX2588_SetRegister( pDevice, 0x00121070 + (0x0200 * i), 0x00001029, 4 );
			CX2588_SetRegister( pDevice, 0x00121074 + (0x0200 * i), 0x414F4F4F, 4 );
			CX2588_SetRegister( pDevice, 0x00121078 + (0x0200 * i), 0x02C00A0E, 4 );
			CX2588_SetRegister( pDevice, 0x00121080 + (0x0200 * i), 0x00000000, 4 );

			// Macrovision setting
			CX2588_SetRegister( pDevice, 0x00121094 + (0x0200 * i), 0x00000A26, 4 );
			CX2588_SetRegister( pDevice, 0x00121098 + (0x0200 * i), 0x5C450A00, 4 );
			CX2588_SetRegister( pDevice, 0x0012109C + (0x0200 * i), 0x80000000, 4 );

			R00121028 = CX2588_GetRegister( pDevice, 0x00121028 + (0x0200*i), 4 ); R00121028 &= 0xFFFFFFC0; 

			CX2588_SetRegister( pDevice, 0x00121028 + (0x0200*i), R00121028, 4 );

			switch( cx ) { // VDEC_HORIZ_TIM_CTRL
			case 720: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x00000074, 4 ); break;
			case 704: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;
			case 640: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;		// 9c
			case 360: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x00000074, 4 ); break;
			case 352: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;
			case 320: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;		// 9c
			}

			switch( cx ) { // VDEC_HSCALE_CTRL
			case 720: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00000000, 4 ); break;
			case 704: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00005D18, 4 ); break;
			case 640: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00020000, 4 ); break;
			case 360: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00100000, 4 ); break;
			case 352: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x0010BA2F, 4 ); break;
			case 320: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00140000, 4 ); break;	
			}

		//	CX2588_SetRegister( pDevice, (0x00121034 + (0x0200 * i)), 0x00000000, 4 ); // VDECA_VSCALE_CTRL

		}
		//////////////////////////////////////////////////////////////////////////////////

	}
	if( e_v4l2_std & V4L2_STD_625_50 ) {

		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00121000 = 0x00000000;

			ULONG R00121044 = 0x00000000;

			ULONG R00121018 = 0x00000000;
			
			ULONG R00121088 = 0x00000000;

			ULONG R00121020 = 0x00000000;

			ULONG R00121054 = 0x00000000;

			ULONG R00121028 = 0x00000000;

			BYTE R00121004 = 0x00;

			BYTE R00121008 = 0x00;

			ULONG R00121024 = 0x00;

			if( pVideo ) { 
				
				if( pVideo->m_nSubDeviceNumber != i ) { 
					
					continue ; 
				} 
			}

			//initialize_PAL()

			// SOFT RESET
			CX2588_SetRegister( pDevice, 0x001211FC + (0x0200 * i), 0x00008000, 4 );

			schedule_timeout_interruptible( msecs_to_jiffies( 20 ) );
			
			CX2588_SetRegister( pDevice, 0x001211FC + (0x0200 * i), 0x00000000, 4 );
			//

			// set video format PAL-BDGHI	
			R00121000 = CX2588_GetRegister( pDevice, 0x00121000 + (0x0200 * i), 4 ); R00121000 &= 0xFFFFFFF0; R00121000 |= 0x00010004;

			CX2588_SetRegister( pDevice, 0x00121000 + (0x0200 * i), R00121000, 4 );

			// Disable clamp gate
			R00121044 = CX2588_GetRegister( pDevice, 0x00121044 + (0x0200 * i), 4 ); R00121044 |= 0x40000000;

			CX2588_SetRegister( pDevice, 0x00121044 + (0x0200 * i), R00121044, 4 );
			
			// Set luma range = 1 ( luma range = 4:1016 )
			CX2588_SetRegister( pDevice, 0x00121014 + (0x0200 * i), 0x00108000, 4 );
       
			// Set c_ext_rng_en = 1 (CrCb range = 4-252 (8b).), set c_core_sel = 1 to improve flat field SNR.
			R00121018 = CX2588_GetRegister( pDevice, 0x00121018 + (0x0200 * i), 4 ); R00121018 |= 0x21000000;

			CX2588_SetRegister( pDevice, 0x00121018 + (0x0200 * i), R00121018, 4 );

			// Set to 10-bit output, id_ins_extend_en = 0x1
			R00121004 = CX2588_GetRegister( pDevice, 0x00121004 + (0x0200 * i), 1 ); R00121004 |= 0x14;      

			CX2588_SetRegister( pDevice, 0x00121004 + (0x0200 * i), R00121004, 1 );

			// Set to 10-bit output, id_ins_extend_en_ns = 0x1
			R00121008 = CX2588_GetRegister( pDevice, 0x00121008 + (0x0200 * i), 1 ); R00121008 |= 0x14;      

			CX2588_SetRegister( pDevice, 0x00121008 + (0x0200 * i), R00121008, 1 );

			// Change field_phase_limit from 0xC to 0xB. It makes vcr_detect bit more reliable
			R00121088 = CX2588_GetRegister( pDevice, 0x00121088 + (0x0200 * i), 4 ); R00121088 &= 0xFFFFFF0F; R00121088 |= 0xB0;    

			CX2588_SetRegister( pDevice, 0x00121088 + (0x0200 * i), R00121088, 4);

			// Set htl_kd = 0x20, htl_ki = 0x40
			R00121054 = CX2588_GetRegister( pDevice, 0x00121054 + (0x0200 * i), 4 ); R00121054 &= 0xFFFF0000; R00121054 |= 0x2040;      
			CX2588_SetRegister( pDevice, 0x00121054 + (0x0200 * i), R00121054, 4 );
 
			// Increase sync_tip_reduce
			CX2588_SetRegister( pDevice, 0x0012101C + (0x0200 * i), 0x001C1E0F, 4 );

			// chroma subcarrier step size
			CX2588_SetRegister( pDevice, 0x0012105C + (0x0200 * i), 0x5411E2D0, 4 );

			// resolution PAL 720x576  HORIZ_TIM_CTRL
			R00121020 = CX2588_GetRegister( pDevice, 0x00121020 + (0x200 * i), 4 ); R00121020 &= 0x00C00C00; R00121020 |= 0x612D007D;

			CX2588_SetRegister( pDevice, 0x00121020 + (0x0200 * i), 0x5E2D0074, 4 );
#if 1		// VERT_TIM_CTRL
			R00121024 = CX2588_GetRegister( pDevice, 0x00121024 + (0x0200 * i), 4 ); R00121024 &= 0x00C00C00; R00121024 |= 0x26240022; // vblank_cnt + 2 to get camera ID
			
			CX2588_SetRegister( pDevice, 0x00121024 + (0x0200 * i), R00121024, 4 );
#endif
			// COMB FILTER SETTINGS BELOW
			CX2588_SetRegister( pDevice, 0x00121068 + (0x0200 * i), 0x0000142F, 4 );
			CX2588_SetRegister( pDevice, 0x0012106C + (0x0200 * i), 0x0000302F, 4 );
			CX2588_SetRegister( pDevice, 0x00121070 + (0x0200 * i), 0x00001023, 4 );
			CX2588_SetRegister( pDevice, 0x00121074 + (0x0200 * i), 0x410F2F0F, 4 );
			CX2588_SetRegister( pDevice, 0x00121078 + (0x0200 * i), 0x02100A0E, 4 );
			CX2588_SetRegister( pDevice, 0x00121080 + (0x0200 * i), 0x00000000, 4 );

			// Macrovision setting
			CX2588_SetRegister( pDevice, 0x00121094 + (0x0200 * i), 0x00000000, 4 );
			CX2588_SetRegister( pDevice, 0x00121098 + (0x0200 * i), 0x00000000, 4 );
			CX2588_SetRegister( pDevice, 0x0012109C + (0x0200 * i), 0x00000000, 4 );

			// SET DECODER FPS
			R00121028 = CX2588_GetRegister( pDevice, 0x00121028 + (0x0200*i), 4 ); R00121028 &= 0xFFFFFFC0;

			CX2588_SetRegister( pDevice, 0x00121028 + (0x0200*i), R00121028, 4 );
			//

			switch( cx ) { // VDEC_HORIZ_TIM_CTRL
			case 720: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x00000074, 4 ); break;
			case 704: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;
			case 640: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;		// 9c
			case 360: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x00000074, 4 ); break;
			case 352: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;
			case 320: CX2588_SetRegister( pDevice, (0x00121020 + (0x0200 * i)), (R00121020 & 0xFFFFFC00) | 0x0000007C, 4 ); break;		// 9c
			}

			switch( cx ) { // VDEC_HSCALE_CTRL
			case 720: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00000000, 4 ); break;
			case 704: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00005D18, 4 ); break;
			case 640: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00020000, 4 ); break;
			case 360: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00100000, 4 ); break;
			case 352: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x0010BA2F, 4 ); break;
			case 320: CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00140000, 4 ); break;	
			}
		}
	}

	return TRUE;
}

BOOLEAN CX2588_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG i = 0;

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 0 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}
	else { 

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_HwAnalogComponentsInitialize( %02ld )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCaptureStreamPowerReference );
	
	if( pDevice->m_nAnalogCaptureStreamPowerReference >= 1 ) { 

	//	for( i = 0; i < 8; i++ ){
	//		CX2588_SetRegister( pDevice, (0x00121030 + (0x0200 * i)), 0x00000000, 4 ); 
	//		CX2588_SetRegister( pDevice, (0x00121034 + (0x0200 * i)), 0x00000000, 4 ); 
	//	}
		
#if 1
		if( pVideo ) { 

			ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1;

			ULONG R00130084 = 0x00000000;

		//	ULONG R00123058 = 0x00000000;

			g_n_analog_decoder_brightness[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_contrast[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_saturation[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_hue[ i ][ 0 ] |= 0x80000000;

			g_n_analog_decoder_sharpness[ i ][ 0 ] |= 0x80000000;

			pVideo->m_nFrameCopyPolarity = 0x00000000;

			pVideo->m_nLockStatus = 0x00000000;

			//SetPixelFormat
			R00130084 = CX2588_GetRegister( pDevice, 0x00130084 + (0x00000100 * pVideo->m_nSubDeviceNumber), 4 ); R00130084 &= 0xFFFF00F8;

			if( pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_GREY ) { R00130084 |= 0x00000006; }

			if( pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_Y41P ) { R00130084 |= 0x00000005; }
			
			if( pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV ) { R00130084 |= 0x00000004; }
			
			R00130084 |= (0xB3 << 8); 

			CX2588_SetRegister( pDevice, 0x00130084 + (0x00000100 * pVideo->m_nSubDeviceNumber), R00130084, 4 );

			LINUXV4L2_DEBUG( KERN_INFO, "R00130084 = 0x%08lX \n", CX2588_GetRegister( pDevice, 0x00130084 + (0x00000100 * pVideo->m_nSubDeviceNumber), 4 ) );
			
		//	SetVIPMode
		//	R00123058 = CX2588_GetRegister( pDevice, 0x00123058 + (0x00000100 * pVideo->m_nSubDeviceNumber), 4 ); //R00123058 |= 0x0001 << pVideo->m_nSubDeviceNumber;

		//	CX2588_SetRegister( pDevice, 0x00123058 + (0x00000100 * pVideo->m_nSubDeviceNumber), 0x00000000/*R00123058*/, 4 );

		}
#endif
#if 1
		if( pAudio ){
		
			ULONG R00120288 = 0x00000000;

		//	ULONG i = pAudio->m_pDevice->m_nKsDeviceNumber * 8 + pAudio->m_nSubDeviceNumber * 1;

			R00120288 = CX2588_GetRegister( pDevice, 0x00120288, 4 ); R00120288 &= 0xFFF8EEEE; // DISABLE ADC1,2,3,4

			CX2588_SetRegister( pDevice, 0x00120288, R00120288, 4 );	

			if( pAudio->m_nRate ==  8000 ) { R00120288 |= 0x00030000; }

			if( pAudio->m_nRate == 16000 ) { R00120288 |= 0x00020000; }

		//	if( pAudio->m_nRate == 24000 ) { R00120288 |= 0x00000000; }

			if( pAudio->m_nRate == 32000 ) { R00120288 |= 0x00010000; }

		//	if( pAudio->m_nRate == 40000 ) { R00120288 |= 0x00000000; }

			if( pAudio->m_nRate == 48000 ) { R00120288 |= 0x00000000; }

			CX2588_SetRegister( pDevice, 0x00120288, R00120288, 4 );		

			R00120288 |= 0x00001111; // RE-ENABLE ADC1,2,3,4
			
			CX2588_SetRegister( pDevice, 0x00120288, R00120288, 4 );	
		}
#endif
	}

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 
#if 1
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010000 + i, 0x00000000, 4 ); } // VIDEO_A_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010050 + i, 0x00000000, 4 ); } // VIDEO_B_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x000100A0 + i, 0x00000000, 4 ); } // VIDEO_C_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x000100F0 + i, 0x00000000, 4 ); } // VIDEO_D_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010140 + i, 0x00000000, 4 ); } // VIDEO_E_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010190 + i, 0x00000000, 4 ); } // VIDEO_F_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x000101E0 + i, 0x00000000, 4 ); } // VIDEO_G_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010230 + i, 0x00000000, 4 ); } // VIDEO_H_DOWN_CMDS

		// 
		//
		CX2588_SetRegister( pDevice, 0x00100100, (1440 / 16) - 1, 4 ); // DMA1_CNT1
		CX2588_SetRegister( pDevice, 0x00100104, (1440 / 16) - 1, 4 ); // DMA2_CNT1
		CX2588_SetRegister( pDevice, 0x00100108, (1440 / 16) - 1, 4 ); // DMA3_CNT1
		CX2588_SetRegister( pDevice, 0x0010010C, (1440 / 16) - 1, 4 ); // DMA4_CNT1
		CX2588_SetRegister( pDevice, 0x00100110, (1440 / 16) - 1, 4 ); // DMA5_CNT1
		CX2588_SetRegister( pDevice, 0x00100114, (1440 / 16) - 1, 4 ); // DMA6_CNT1
		CX2588_SetRegister( pDevice, 0x00100118, (1440 / 16) - 1, 4 ); // DMA7_CNT1
		CX2588_SetRegister( pDevice, 0x0010011C, (1440 / 16) - 1, 4 ); // DMA8_CNT1

		CX2588_SetRegister( pDevice, 0x00010C00 + (16 * 0), 0x00000040 + (1440 *  0), 4 ); // VID_A_CDT
		CX2588_SetRegister( pDevice, 0x00010C00 + (16 * 1), 0x00000040 + (1440 *  1), 4 );
		CX2588_SetRegister( pDevice, 0x00010C00 + (16 * 2), 0x00000040 + (1440 *  2), 4 );
		CX2588_SetRegister( pDevice, 0x00010C00 + (16 * 3), 0x00000040 + (1440 *  3), 4 );
		CX2588_SetRegister( pDevice, 0x00010C40 + (16 * 0), 0x00000040 + (1440 *  4), 4 ); // VID_B_CDT
		CX2588_SetRegister( pDevice, 0x00010C40 + (16 * 1), 0x00000040 + (1440 *  5), 4 );
		CX2588_SetRegister( pDevice, 0x00010C40 + (16 * 2), 0x00000040 + (1440 *  6), 4 );
		CX2588_SetRegister( pDevice, 0x00010C40 + (16 * 3), 0x00000040 + (1440 *  7), 4 );
		CX2588_SetRegister( pDevice, 0x00010C80 + (16 * 0), 0x00000040 + (1440 *  8), 4 ); // VID_C_CDT
		CX2588_SetRegister( pDevice, 0x00010C80 + (16 * 1), 0x00000040 + (1440 *  9), 4 );
		CX2588_SetRegister( pDevice, 0x00010C80 + (16 * 2), 0x00000040 + (1440 * 10), 4 );
		CX2588_SetRegister( pDevice, 0x00010C80 + (16 * 3), 0x00000040 + (1440 * 11), 4 );
		CX2588_SetRegister( pDevice, 0x00010CC0 + (16 * 0), 0x00000040 + (1440 * 12), 4 ); // VID_D_CDT
		CX2588_SetRegister( pDevice, 0x00010CC0 + (16 * 1), 0x00000040 + (1440 * 13), 4 );
		CX2588_SetRegister( pDevice, 0x00010CC0 + (16 * 2), 0x00000040 + (1440 * 14), 4 );
		CX2588_SetRegister( pDevice, 0x00010CC0 + (16 * 3), 0x00000040 + (1440 * 15), 4 );
		CX2588_SetRegister( pDevice, 0x00010D00 + (16 * 0), 0x00000040 + (1440 * 16), 4 ); // VID_E_CDT
		CX2588_SetRegister( pDevice, 0x00010D00 + (16 * 1), 0x00000040 + (1440 * 17), 4 );
		CX2588_SetRegister( pDevice, 0x00010D00 + (16 * 2), 0x00000040 + (1440 * 18), 4 );
		CX2588_SetRegister( pDevice, 0x00010D00 + (16 * 3), 0x00000040 + (1440 * 19), 4 );
		CX2588_SetRegister( pDevice, 0x00010D40 + (16 * 0), 0x00000040 + (1440 * 20), 4 ); // VID_F_CDT
		CX2588_SetRegister( pDevice, 0x00010D40 + (16 * 1), 0x00000040 + (1440 * 21), 4 );
		CX2588_SetRegister( pDevice, 0x00010D40 + (16 * 2), 0x00000040 + (1440 * 22), 4 );
		CX2588_SetRegister( pDevice, 0x00010D40 + (16 * 3), 0x00000040 + (1440 * 23), 4 );
		CX2588_SetRegister( pDevice, 0x00010D80 + (16 * 0), 0x00000040 + (1440 * 24), 4 ); // VID_G_CDT
		CX2588_SetRegister( pDevice, 0x00010D80 + (16 * 1), 0x00000040 + (1440 * 25), 4 );
		CX2588_SetRegister( pDevice, 0x00010D80 + (16 * 2), 0x00000040 + (1440 * 26), 4 );
		CX2588_SetRegister( pDevice, 0x00010D80 + (16 * 3), 0x00000040 + (1440 * 27), 4 );
		CX2588_SetRegister( pDevice, 0x00010DC0 + (16 * 0), 0x00000040 + (1440 * 28), 4 ); // VID_H_CDT
		CX2588_SetRegister( pDevice, 0x00010DC0 + (16 * 1), 0x00000040 + (1440 * 29), 4 );
		CX2588_SetRegister( pDevice, 0x00010DC0 + (16 * 2), 0x00000040 + (1440 * 30), 4 );
		CX2588_SetRegister( pDevice, 0x00010DC0 + (16 * 3), 0x00000040 + (1440 * 31), 4 );

		for( i = 0 ; i < 8 ; i ++ ) { 

			CX2588_SetRegister( pDevice, 0x00010000 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2588_SetRegister( pDevice, 0x00010004 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2588_SetRegister( pDevice, 0x00010010 + (0x00000050 * i), 0x00011000 + (0x00000040 * i), 4 ); // VID_A_IQ
			CX2588_SetRegister( pDevice, 0x00010014 + (0x00000050 * i), 0x00000040 / (             4), 4 );
			CX2588_SetRegister( pDevice, 0x00010008 + (0x00000050 * i), 0x00010C00 + (0x00000040 * i), 4 ); // VID_A_CDT
			CX2588_SetRegister( pDevice, 0x00100080 + (0x00000004 * i), 0x00010C00 + (0x00000040 * i), 4 ); // DMA1_PTR2 -> VID_A_CDT
			CX2588_SetRegister( pDevice, 0x0001000C + (0x00000050 * i), 0x00000040 / (             8), 4 );
			CX2588_SetRegister( pDevice, 0x00100180 + (0x00000004 * i), 0x00000040 / (             8), 4 ); // DMA1_CNT2

		}
#endif
#if 1
		// initAudioSram
		
		for( i = 0 ; i < 4 ; i ++ ) { CX2588_SetRegister( pDevice, 0x000400C0 +  (0x00000010 * i), 0x00000000, 4  ); }

		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010500 + i, 0x00000000, 4  ); } // AUD_A_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x00010550 + i, 0x00000000, 4  ); } // AUD_B_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x000105A0 + i, 0x00000000, 4  ); } // AUD_C_DOWN_CMDS
		for( i = 0 ; i < 80 ; i += 4 ) { CX2588_SetRegister( pDevice, 0x000105F0 + i, 0x00000000, 4  ); } // AUD_D_DOWN_CMDS

		CX2588_SetRegister( pDevice, 0x00100140, (128 /  8) - 1, 4 ); // DMA17_CNT1
		CX2588_SetRegister( pDevice, 0x00100144, (128 /  8) - 1, 4 ); // DMA18_CNT1
		CX2588_SetRegister( pDevice, 0x00100148, (128 /  8) - 1, 4 ); // DMA19_CNT1
		CX2588_SetRegister( pDevice, 0x0010014C, (128 /  8) - 1, 4 ); // DMA20_CNT1

		CX2588_SetRegister( pDevice, 0x00010E80 + (16 * 0), 0x0000B500 + (128 *  0), 4 ); // AUD_A_CDT
		CX2588_SetRegister( pDevice, 0x00010E80 + (16 * 1), 0x0000B500 + (128 *  1), 4 ); 
		CX2588_SetRegister( pDevice, 0x00010E80 + (16 * 2), 0x0000B500 + (128 *  2), 4 ); 
		CX2588_SetRegister( pDevice, 0x00010EB0 + (16 * 0), 0x0000B500 + (128 *  3), 4 ); // AUD_B_CDT
		CX2588_SetRegister( pDevice, 0x00010EB0 + (16 * 1), 0x0000B500 + (128 *  4), 4 ); 
		CX2588_SetRegister( pDevice, 0x00010EB0 + (16 * 2), 0x0000B500 + (128 *  5), 4 ); 
		CX2588_SetRegister( pDevice, 0x00010EE0 + (16 * 0), 0x0000B500 + (128 *  6), 4 ); // AUD_C_CDT
		CX2588_SetRegister( pDevice, 0x00010EE0 + (16 * 1), 0x0000B500 + (128 *  7), 4 );
		CX2588_SetRegister( pDevice, 0x00010EE0 + (16 * 2), 0x0000B500 + (128 *  8), 4 );
		CX2588_SetRegister( pDevice, 0x00010F10 + (16 * 0), 0x0000B500 + (128 *  9), 4 ); // AUD_D_CDT
		CX2588_SetRegister( pDevice, 0x00010F10 + (16 * 1), 0x0000B500 + (128 * 10), 4 );
		CX2588_SetRegister( pDevice, 0x00010F10 + (16 * 2), 0x0000B500 + (128 * 11), 4 );

		for( i = 0 ; i < 4 ; i ++ ) { 

		//	CX2588_SetRegister( pDevice, 0x00010500 + (0x00000050 * i), 0x00000000                   , 4 );
		//	CX2588_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), 0x00000000                   , 4 );
		//	CX2588_SetRegister( pDevice, 0x00010508 + (0x00000050 * i), 0x00010E80 + (0x00000030 * i), 4 );
		//	CX2588_SetRegister( pDevice, 0x0001050C + (0x00000050 * i), 0x00000030 / (             8), 4 );
		//	CX2588_SetRegister( pDevice, 0x00010510 + (0x00000050 * i), 0x00011280 + (0x00000040 * i), 4 );
		//	CX2588_SetRegister( pDevice, 0x00010514 + (0x00000050 * i), 0x00000040 / (             4), 4 );
		//	CX2588_SetRegister( pDevice, 0x001000C0 + (0x00000004 * i), 0x00010E80 + (0x00000030 * i), 4 );
		//	CX2588_SetRegister( pDevice, 0x001001C0 + (0x00000004 * i), 0x00000030 / (             8), 4 );

			CX2588_SetRegister( pDevice, 0x00010500 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2588_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), 0x00000000                   , 4 );
			CX2588_SetRegister( pDevice, 0x00010510 + (0x00000050 * i), 0x00011280 + (0x00000040 * i), 4 );
			CX2588_SetRegister( pDevice, 0x00010514 + (0x00000050 * i), 0x00000040 / (             4), 4 );
			CX2588_SetRegister( pDevice, 0x00010508 + (0x00000050 * i), 0x00010E80 + (0x00000030 * i), 4 );
			CX2588_SetRegister( pDevice, 0x0001050C + (0x00000050 * i), 0x00000030 / (             8), 4 );		
			CX2588_SetRegister( pDevice, 0x001000C0 + (0x00000004 * i), 0x00010E80 + (0x00000030 * i), 4 );
			CX2588_SetRegister( pDevice, 0x001001C0 + (0x00000004 * i), 0x00000030 / (             8), 4 );

		}
		// 
#endif
#if 1
		for( i = 0 ; i < 8 ; i ++ ) {

			dma_addr_t pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
			dma_addr_t pDmaBaseCommonPhysicalBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];
			DWORD *    pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);
			DWORD *    pDmaBaseCommonBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 0 ]);

			pDmaBaseCommonPhysicalStartBuffer += (CX2588_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (1);
			pDmaBaseCommonPhysicalBuffer      += (CX2588_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (1);
			pDmaBaseCommonStartBuffer         += (CX2588_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (4);
			pDmaBaseCommonBuffer              += (CX2588_MAX_VIDEO_DMA_BUFFER_SIZE / 8) * (i) / (4);

			CX2588_SetRegister( pDevice, 0x00010000 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >>  0, 4 ); //
		//	CX2588_SetRegister( pDevice, 0x00010004 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >> 32, 4 ); //
			CX2588_SetRegister( pDevice, 0x00010004 + (0x00000050 * i), 0x00000000, 4 );

			//
			// 
			memset( pDmaBaseCommonBuffer, 0x00, CX2588_MAX_VIDEO_DMA_BUFFER_SIZE / 8 );

			pDmaBaseCommonPhysicalBuffer += 80;

			pDmaBaseCommonBuffer += 0 >> 2;
			
			pDmaBaseCommonBuffer[  0 ] = 0x80008000; // SYNC | SYNC_ODD | RESYNC
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
			pDmaBaseCommonBuffer[ 17 ] = 0x70000000; // JUMP
			pDmaBaseCommonBuffer[ 18 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
		//	pDmaBaseCommonBuffer[ 19 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
			pDmaBaseCommonBuffer[ 19 ] = 0x00000000;
			
			pDmaBaseCommonPhysicalBuffer += 6940;

			pDmaBaseCommonBuffer += 80 >> 2;

			pDmaBaseCommonBuffer[  0 ] = 0x2C0005A0; // SKIP | SOL  | EOL
			pDmaBaseCommonBuffer[  1 ] = 0x71030000; // JUMP | IRQ1 | CNT_RESET
		//	pDmaBaseCommonBuffer[  1 ] = 0x71010000; // JUMP | IRQ1 | CNT_RESET
			pDmaBaseCommonBuffer[  2 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
		//	pDmaBaseCommonBuffer[  3 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
			pDmaBaseCommonBuffer[  3 ] = 0x00000000;
			
			pDmaBaseCommonPhysicalBuffer += 80;

			pDmaBaseCommonBuffer += 6940 >> 2;

			pDmaBaseCommonBuffer[  0 ] = 0x80008200; // SYNC | SYNC_EVEN
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
			pDmaBaseCommonBuffer[ 17 ] = 0x70000000; // JUMP
			pDmaBaseCommonBuffer[ 18 ] = pDmaBaseCommonPhysicalBuffer >>  0; //
		//	pDmaBaseCommonBuffer[ 19 ] = pDmaBaseCommonPhysicalBuffer >> 32; //
			pDmaBaseCommonBuffer[ 19 ] = 0x00000000;
			
			pDmaBaseCommonPhysicalBuffer += 6940;

			pDmaBaseCommonBuffer += 80 >> 2;

			pDmaBaseCommonBuffer[  0 ] = 0x2C0005A0; // SKIP | SOL  | EOL
			pDmaBaseCommonBuffer[  1 ] = 0x71010000; // JUMP | IRQ1 | CNT_INC
			pDmaBaseCommonBuffer[  2 ] = pDmaBaseCommonPhysicalStartBuffer >>  0; //
		//	pDmaBaseCommonBuffer[  3 ] = pDmaBaseCommonPhysicalStartBuffer >> 32; //
			pDmaBaseCommonBuffer[  3 ] = 0x00000000;
		}
#endif
#if 1
		for( i = 0 ; i < 4 ; i ++ ) {

			dma_addr_t pDmaBaseCommonPhysicalStartBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];
			dma_addr_t pDmaBaseCommonPhysicalBuffer = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];
			DWORD *    pDmaBaseCommonStartBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 1 ]);
			DWORD *    pDmaBaseCommonBuffer = (DWORD *)(pDevice->m_pDmaBaseCommonBuffer[ 1 ]);
			ULONG      j = 0;

			pDmaBaseCommonPhysicalStartBuffer += (CX2588_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (1);
			pDmaBaseCommonPhysicalBuffer      += (CX2588_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (1);
			pDmaBaseCommonStartBuffer         += (CX2588_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (4);
			pDmaBaseCommonBuffer              += (CX2588_MAX_AUDIO_DMA_BUFFER_SIZE / 4) * (i) / (4);

			CX2588_SetRegister( pDevice, 0x00010500 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >>  0, 4 ); //
		//	CX2588_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), pDmaBaseCommonPhysicalStartBuffer >> 32, 4 ); //
			CX2588_SetRegister( pDevice, 0x00010504 + (0x00000050 * i), 0x00000000, 4 );

			memset( pDmaBaseCommonBuffer, 0x00, CX2588_MAX_AUDIO_DMA_BUFFER_SIZE / 4 );

			for( j = 0 ; j < 4 ; j++ ) {

				dma_addr_t physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 2 ];

				physical_address += (1920 * 4) * (i);

				physical_address += (1920) * (j);

				pDmaBaseCommonPhysicalBuffer += 24;

				pDmaBaseCommonBuffer[ 0 ] = 0x1C000000 + (128); // RISC_WRITE | RISC_SOL | RISC_EOL
				pDmaBaseCommonBuffer[ 1 ] = physical_address >>  0; //
			//	pDmaBaseCommonBuffer[ 2 ] = physical_address >> 32; //
				pDmaBaseCommonBuffer[ 2 ] = 0x00000000;
				pDmaBaseCommonBuffer[ 3 ] = 0x70000000; // RISC_JUMP
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
			//	pDmaBaseCommonBuffer[ 3 ] = (j == 0) ? 0x71030000 : 0x71010000; // RISC_jUMP | RISC_IRQ1 | (RISC_RESET/RISC_INC)
				pDmaBaseCommonBuffer[ 3 ] = (j == 0) ? 0x71030000 : 0x71010000; // RISC_jUMP | RISC_IRQ1 | (RISC_RESET/RISC_INC)
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
#endif		
#if 1
		for( i = 0 ; i < 8 ; i++ ) { 

		//	ULONG R00121004 = 0x0000000;
			
			CX2588_SetRegister( pDevice, 0x00130030 + (0x00000100 * i), 0x00000003, 4 ); // VIDEO X COUNTER TO 0

		//	CX2588_SetRegister( pDevice, 0x00130080 + (0x00000100 * i), 0x00000000, 4 );
			
		//	R00121004 = CX2588_GetRegister( pDevice, 0x00121004 + (0x00000200 * i), 4 ); R00121004 |= 0x00000002;
			
		//	CX2588_SetRegister( pDevice, 0x00121004 + (0x00000200 * i), R00121004, 4 );
		}
#endif
#if 1
		for( i = 0 ; i < 4 ; i++ ) { 

			ULONG R0014011C = 0x00000000;

			ULONG R00122004 = 0x00000000;

			CX2588_SetRegister( pDevice, 0x00140114 + (0x00000100 * i), 0x00000003, 4 );			// AUD_X_DST_GPCNT_CTL

			CX2588_SetRegister( pDevice, 0x00140118 + (0x00000100 * i), (128) & 0x00000FFF, 4 );	// AUD_X_DST_LNGTH

			R0014011C = CX2588_GetRegister( pDevice, 0x0014011C + (0x00000100 * i), 4 ); R0014011C |= 0x00004000;

			CX2588_SetRegister( pDevice, 0x0014011C + (0x00000100 * i), R0014011C, 4 );				// AUD_X_DST_CFG

			R00122004 = CX2588_GetRegister( pDevice, 0x00122004 + (0x04 * i), 4 ); R00122004 |= 0x00010202;

			CX2588_SetRegister( pDevice, 0x00122004 + (0x04 * i), R00122004, 4 );					// AP_AUD_X_CFG
			
		}

		{
			ULONG R00120210 = 0x00000000;

			R00120210 = CX2588_GetRegister( pDevice, 0x00120210, 4 ); R00120210 |= 0x0000000F;

			CX2588_SetRegister( pDevice, 0x00120210, R00120210, 4 );								// AUD_VID_CLK_CTRL
		}
#endif
	}
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		pDevice->m_hInterruptAccessLock = FALSE;
#if 1
		for( i = 0 ; i < 8 ; i++ ) {

			ULONG R00130040 = 0x00000000;

			// ...
			//
		//	CX2588_SetRegister( pDevice, 0x00040024 + (0x00000010 * i), 0x00000001, 4 );
			CX2588_SetRegister( pDevice, 0x00040024 + (0x00000010 * i), 0x00011101, 4 );


			// ...
			//
		//	CX2588_SetRegister( pDevice, 0x00040020 + (0x00000010 * i), 0x00000001, 4 );
			CX2588_SetRegister( pDevice, 0x00040020 + (0x00000010 * i), 0x00011101, 4 );

			// ...
			// 
			R00130040 = CX2588_GetRegister( pDevice, 0x00130040 + (0x00000100 * i), 4 ) & 0xFFFFFFEE;

			CX2588_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000001, 4 );

			CX2588_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000011, 4 );

		}
#endif
#if 1
		for( i = 0 ; i < 4 ; i++ ) {

			ULONG R00140108 = 0x00000000;

			// ...
			// 
		//	CX2588_SetRegister( pDevice, 0x000400C4 + (0x00000010 * i), 0x00000001, 4 );
			CX2588_SetRegister( pDevice, 0x000400C4 + (0x00000010 * i), 0x00000000, 4 );

			//
			// 
			CX2588_SetRegister( pDevice, 0x000400C0 + (0x00000010 * i), 0x00011101, 4 );
		//	CX2588_SetRegister( pDevice, 0x000400C0 + (0x00000010 * i), 0x00000001, 4 );
			
			//
			// 
			R00140108 = CX2588_GetRegister( pDevice, (0x00140108 + 0x0100*i), 4 ) & 0xFFFFFFEE;

			CX2588_SetRegister( pDevice, (0x00140108 + 0x0100*i), R00140108 | 0x00000001, 4 );

			CX2588_SetRegister( pDevice, (0x00140108 + 0x0100*i), R00140108 | 0x00000011, 4 ); 

			schedule_timeout_interruptible( msecs_to_jiffies( 500 ) );
		}
#endif
		pDevice->m_hInterruptAccessLock = TRUE;
	}
	return TRUE;
}

BOOLEAN CX2588_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_HwAnalogComponentsUnInitialize( %02ld )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCaptureStreamPowerReference );

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
		R00130040 = CX2588_GetRegister( pDevice, 0x00130040 + (0x00000100 * i), 4 ) & 0xFFFFFFEE;

		CX2588_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000001, 4 );

		CX2588_SetRegister( pDevice, 0x00130040 + (0x00000100 * i), R00130040 | 0x00000000, 4 );

		// ...
		//
		CX2588_SetRegister( pDevice, 0x00040020 + (0x00000010 * i), 0x00000000, 4 );
	}

	for( i = 0 ; i < 4 ; i++ ) {

		ULONG R00140108 = 0x00000000;

		// ...
		//			
		R00140108 = CX2588_GetRegister( pDevice, (0x00140108 + 0x0100*i), 4 ) & 0xFFFFFFEE; 

		CX2588_SetRegister( pDevice, 0x00140108, R00140108, 4 );

		// ...
		// 
		CX2588_SetRegister( pDevice, 0x000400C0 + (0x00000010 * i), 0x00000000, 4 ); 
	}
	pDevice->m_hInterruptAccessLock = FALSE;

	return TRUE;
}

BOOLEAN CX2588_HwUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_HwUnInitialize()\n", pDevice->m_nKsDeviceNumber );

	return TRUE;
}

BOOLEAN CX2588_HwInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_HwInitialize()\n", pDevice->m_nKsDeviceNumber );

	{	ULONG R00050318 = 0x00000000;

		ULONG R00000400 = 0x00000000;

		ULONG i = 0;

		// ENABLE RUN_RISC IN PECOS
		CX2588_SetRegister( pDevice, 0x00040000, 0x00000020, 4 );

	    // SET THE MASTER PCI INTERRUPT MASKS TO ENABLE VIDEO, AUDIO, MBIF, AND GPIO INTERRUPTS
		// I2C INTERRUPT MASKING IS HANDLED BY THE I2C OBJECTS THEMSELVES.
		CX2588_SetRegister( pDevice, 0x00040010, 0x2007FFFF, 4 );

		R00050318 = CX2588_GetRegister( pDevice, 0x00050318, 4 ); R00050318 &= (~0x00000010);

		CX2588_SetRegister( pDevice, 0x00050318, R00050318, 4 );

		// SET MSK FOR RISC AND DMAC
	    CX2588_SetRegister( pDevice, 0x00040140, 0xFFFFFFFF, 4 );    
		CX2588_SetRegister( pDevice, 0x00040150, 0xFFFFFFFF, 4 );


LINUXV4L2_DEBUG( KERN_INFO, "initColibri: start\n");
		// INIT COLIBRI REGISTERS
		do {
			CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0400, 0x41, 1); // Set TUNE1,TUNE2 ref_count = 0x1F8

			R00000400 = CX2588_GetAnalogVideoDecoderRegister( pDevice, 0x0400, 1 );
    
			// sleep 50ms
			schedule_timeout_interruptible( msecs_to_jiffies( 50 ) );

		} while ( (R00000400 != 0x41) && (++i < 10) );

		if ( R00000400 != 0x41 )
			LINUXV4L2_DEBUG( KERN_INFO, "init initColibri - failed to write after retried %ld, 0x0400=%lX\n", i, R00000400 );

		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0401, 0xF8, 1 ); 
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0403, 0x0F, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0404, 0x1F, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0405, 0x0F, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0406, 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0407, 0x13, 1 ); 
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x040A, 0x00, 1 ); 

		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0500, 0x41, 1 ); 
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0501, 0xF8, 1 ); 
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0503, 0x0F, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0504, 0x1F, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0505, 0x0F, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0506, 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0507, 0x13, 1 ); 
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x050A, 0x00, 1 );

		// enable pll
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0408, 0x18, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0508, 0x18, 1 );

		// start tuning filter
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0402, 0x40, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0502, 0x40, 1 );

		// exit tuning
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0402, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0502, 0x00, 1 );

		// power up all channels, clear pd_buffer
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0423,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0443,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0463,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0483,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0523,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0543,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0563,  0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0583,  0x00, 1 );

		// Enable channel calibration
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x040B, 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x050B, 0x02, 1 );

		// force modulator reset
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0427,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0447,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0467,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0487,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0527,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0547,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0567,  0x17, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0587,  0x17, 1 );

		schedule_timeout_interruptible( msecs_to_jiffies( 5 ) );

		// start quantizer calibration
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0422, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0442, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0462, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0482, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0522, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0542, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0562, 0x10, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0582, 0x10, 1 );

		schedule_timeout_interruptible( msecs_to_jiffies( 5 ) );

		// exit modulator (fb) reset
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0427, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0447, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0467, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0487, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0527, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0547, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0567, 0x07, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0587, 0x07, 1 );

		// Select Vin1_N as single-ended input
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0428, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0448, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0468, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0488, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0528, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0548, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0568, 0x00, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0588, 0x00, 1 );

		// dac23_gain from 0x1 above to 0x3 below
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0425 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0445 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0465 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0485 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0525 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0545 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0565 , 0x05, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0585 , 0x05, 1 );

		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0424 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0444 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0464 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0484 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0524 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0544 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0564 , 0x67, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0584 , 0x67, 1 );

		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0426 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0446 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0466 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0486 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0526 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0546 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0566 , 0x03, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0586 , 0x03, 1 );

		// enable the pre_clamp in each channel for single-ended input
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0429 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0449 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0469 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0489 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0529 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0549 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0569 , 0xF0, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x0589 , 0xF0, 1 );

		// use diode instead of resistor, so set term_en to 0, res_en to 0
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x042A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x044A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x046A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x048A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x052A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x054A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x056A , 0x02, 1 );
		CX2588_SetAnalogVideoDecoderRegister( pDevice, 0x058A , 0x02, 1 );
	}
LINUXV4L2_DEBUG( KERN_INFO, "initColibri: exit\n");

	//


	// ENABLE BLUEFIELD IF NO VIDEO	
//	ULONG R00121204 = 0x00000000;
//	ULONG R00121208 = 0x00000000;
//	ULONG i;
//	for( i = 0; i < 8; i++ ) {
//		
//		R00121204 = CX2588_GetRegister( pDevice, 0x00121204 + (0x0200 * i), 4 ); R00121204 &= 0xFFFFFF7F; R00121204 |= 0x00000080;
//		CX2588_SetRegister( pDevice, 0x00121204 + (0x0200 * i), R00121204, 4 );
//
//		R00121204 = CX2588_GetRegister( pDevice, 0x00121208 + (0x0200 * i), 4 ); R00121208 &= 0xFFFFFF7F; R00121208 |= 0x00000080;
//		CX2588_SetRegister( pDevice, 0x00121208 + (0x0200 * i), R00121208, 4 );
//	}
	//

	CX2588_SetAnalogVideoDecoderStandardProperty( pDevice, NULL );
#if 1
	{	ULONG R00120208 = 0x00000000;

		//initAudioChannels
		
//	LINUXV4L2_DEBUG( KERN_INFO, "initAudioChannels: start\n");
		
		//ADC
		CX2588_SetRegister( pDevice, 0x00120288, 0x00001111, 4);                // 48KHz Sample Rate, Enable ADC1,2,3 and 4
		CX2588_SetRegister( pDevice, 0x00120290, 0xF1005555, 4);                // Power on all ADCs, DSM dither enable
		CX2588_SetRegister( pDevice, 0x00120294, 0x00000000, 4);                // Default value
		CX2588_SetRegister( pDevice, 0x00120298, 0x00000000, 4);                // Default value

		// Power on PLL, PLL frequency = (48e6 * 12.288)/6 = 98.304 MHz
		CX2588_SetRegister( pDevice, 0x00120284, 0x40522716, 4);                // Power on PLL, PLL frequency = 98.304MHz
		CX2588_SetRegister( pDevice, 0x00120280, 0x189374BC, 4);                // Default Value

		// I2S
		CX2588_SetRegister( pDevice, 0x122000, 0x00000000, 4);                  // frame_sync_delay = 0,frame_sync_width = 0,  i2s_mode = 0
		CX2588_SetRegister( pDevice, 0x122004, 0x00010103, 4);                  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)
		CX2588_SetRegister( pDevice, 0x122008, 0x00010103, 4);                  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)
		CX2588_SetRegister( pDevice, 0x12200C, 0x00010103, 4);                  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)
		CX2588_SetRegister( pDevice, 0x122010, 0x00010103, 4);                  // no encode, adc output to I2S pin, I2S Master/(Sony = Left Justified mode)

		// Enable MCLK
		R00120208 = CX2588_GetRegister( pDevice, 0x00120208, 4); R00120208 |= 0x80000000;
		CX2588_SetRegister( pDevice, 0x00120208, R00120208, 4 );
//LINUXV4L2_DEBUG( KERN_INFO, "initAudioChannels: end\n");
		//
	}
#endif
	{
		ULONG R00123000 = 0x00000000;

		ULONG R00123050 = 0x00000000;

		ULONG R00123054 = 0x00000000;
		
		ULONG i = 0x00000000;
		
		for ( i = 0; i < 4; i++)
		{ 
			// Loop 4 times for 4 video processors
			R00123000 = CX2588_GetRegister( pDevice, 0x00123000 + (0x0010*i), 4 ); R00123000 &= 0x0000F0F0; R00123000 |= 0x64200103;
			CX2588_SetRegister( pDevice, 0x00123000+(0x10*i), R00123000, 4 );
		}

		// VP_DMA_DST_MUX1
		R00123050 = CX2588_GetRegister( pDevice, 0x00123050, 4 ); R00123050 &= 0xE0E0E0E0;

		CX2588_SetRegister( pDevice, 0x00123050, R00123050, 4 );

		// VP_DMA_DST_MUX2
		R00123054 = CX2588_GetRegister( pDevice, 0x00123054, 4 ); R00123054 &= 0xE0E0E0E0;

		CX2588_SetRegister( pDevice, 0x00123054, R00123054, 4 );

	}


	return CX2588_PowerDownPeripherals( pDevice );
}

BOOLEAN CX2588_PowerDownPeripherals( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_PowerDownPeripherals()\n", pDevice->m_nKsDeviceNumber );

	return TRUE;
}


BOOLEAN CX2588_HwProcessAnalogPCIVideoPacket( CDevice *pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	DWORD R00130020 = 0x00000000;

	ULONG y = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_HwProcessAnalogPCIVideoPacket( %08X:%08X )\n", pDevice->m_nKsDeviceNumber, status, mask );

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
		R00130020 = CX2588_GetRegister( pDevice, 0x00130020 + (0x00000100 * y), 4 ); 
		
		if( pVideo ) {

			if( pVideo->m_nSrcFrameHeight > 288 ) { //

				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 0 + 80 * 1); pDmaBaseCommonStartBuffer += (6940 * 0 + 80 * 1) >> 2; is_copy = 1; is_complete = 0; is_reset = 1; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 1 + 80 * 2); pDmaBaseCommonStartBuffer += (6940 * 1 + 80 * 2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }

				if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 1 + 80 * 1); pDmaBaseCommonJumpBuffer  += (6940 * 1 + 80 * 1) >> 2; }
				if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 0 + 80 * 0); pDmaBaseCommonJumpBuffer  += (6940 * 0 + 80 * 0) >> 2; } //
			}
		//	if( pVideo->m_nSrcFrameHeight <= 288 ) { //
		//
		//		if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 1 + 80 * 2); pDmaBaseCommonStartBuffer += (6940 * 1 + 80 * 2) >> 2; is_copy = 1; is_complete = 1; is_reset = 0; }
		//		if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalStartBuffer += (6940 * 0 + 80 * 1); pDmaBaseCommonStartBuffer += (6940 * 0 + 80 * 1) >> 2; is_copy = 1; is_complete = 1; is_reset = 1; }
		//
		//		if( (R00130020 & 0x0000FFFF) == 0x00000000 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 0 + 80 * 0); pDmaBaseCommonJumpBuffer  += (6940 * 0 + 80 * 0) >> 2; } //
		//		if( (R00130020 & 0x0000FFFF) == 0x00000001 ) { pDmaBaseCommonPhysicalJumpBuffer  += (6940 * 1 + 80 * 1); pDmaBaseCommonJumpBuffer  += (6940 * 1 + 80 * 1) >> 2; }
		//	}
			if( pVideo->m_nSrcFrameHeight <= 288 ) { //
		
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

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

			struct videobuf_dmabuf * pKsDMABuffer = videobuf_to_dma( &pVideoBuffer->m_sKsBuffer );
				
			#else

			struct videobuf_dmabuf * pKsDMABuffer = &(pVideoBuffer->m_sKsBuffer.dma);

			#endif

			struct scatterlist * p_sglist = (struct scatterlist *)(pKsDMABuffer->sglist);
			
			dma_addr_t physical_address = sg_dma_address( p_sglist );
			
			ULONG byte_count = sg_dma_len( p_sglist );

			ULONG mapping_count = pKsDMABuffer->sglen;

			ULONG pitch = pVideo->m_nDstFrameWidth;

			ULONG lines = 0;

			ULONG count = 0;

			ULONG i = 0;

			if( pVideo->m_nSrcFrameHeight == 240 ) { lines = 240; }

			if( pVideo->m_nSrcFrameHeight == 480 ) { lines = 240; }

			if( pVideo->m_nSrcFrameHeight == 288 ) { lines = 288; }

			if( pVideo->m_nSrcFrameHeight == 576 ) { lines = 288; }

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

								p_sglist++;

								count++;

								if( count < mapping_count ) {

									physical_address = sg_dma_address( p_sglist );

									byte_count = sg_dma_len( p_sglist );
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

						p_sglist++;

						count++;

						if( count < mapping_count ) {

							physical_address = sg_dma_address( p_sglist );

							byte_count = sg_dma_len( p_sglist );
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

						p_sglist++;

						count++;

						if( count < mapping_count ) {

							physical_address = sg_dma_address( p_sglist );

							byte_count = sg_dma_len( p_sglist );
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
	}

	return TRUE;
}

BOOLEAN CX2588_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	ULONG i = 0;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2588_HwProcessAnalogPCIAudioPacket( %08X:%08X )\n", pDevice->m_nKsDeviceNumber, status, mask );

	for( i = 0 ; i < 8 ; i++ ) {

		ULONG R00140110 = 0x00000000;

	//	if( (i == 0) && ((status & 0x00000100) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 0), 4 ); }	
	//	if( (i == 1) && ((status & 0x00000100) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 0), 4 ); }
	//	if( (i == 2) && ((status & 0x00000200) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 1), 4 ); }
	//	if( (i == 3) && ((status & 0x00000200) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 1), 4 ); }
	//	if( (i == 4) && ((status & 0x00000400) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 2), 4 ); }	
	//	if( (i == 5) && ((status & 0x00000400) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 2), 4 ); }	
	//	if( (i == 6) && ((status & 0x00000800) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 3), 4 ); }
	//	if( (i == 7) && ((status & 0x00000800) == 0x00000000) ) { continue ; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 3), 4 ); }

		switch(i){
		case 0: if((status & 0x00000100) == 0x00000000 ){ 
					continue; 
				} 
				else {
					R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 0), 4 ); 
				} 
				break;
		case 1: if((status & 0x00000100) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 0), 4 ); } break;
		case 2: if((status & 0x00000200) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 1), 4 ); } break;
		case 3: if((status & 0x00000200) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 1), 4 ); } break;
		case 4: if((status & 0x00000400) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 2), 4 ); } break;
		case 5: if((status & 0x00000400) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 2), 4 ); } break;
		case 6: if((status & 0x00000800) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 3), 4 ); } break;
		case 7: if((status & 0x00000800) == 0x00000000){ continue; } else { R00140110 = CX2588_GetRegister( pDevice, 0x00140110 + (0x00000100 * 3), 4 ); } break;
		}

		if( pAudios[ i ] ) {

			CAudio * pAudio = pAudios[ i ];

			BOOLEAN is_complete = FALSE;

			BOOLEAN is_copy = FALSE;

			BYTE *  po = NULL;

			BYTE *  pe = pAudio->m_pKsSubStream->runtime->dma_area;

			ULONG   sz = pAudio->m_nFrameSize;

			ULONG	j = 0;

			ULONG	offset = ( i%2 == 0 ) ? (0) : (2); 

			if( (R00140110 & 0x000000FF) == 0x00000000 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 0 + offset; is_copy = TRUE; is_complete = TRUE; }
			
			if( (R00140110 & 0x000000FF) == 0x00000001 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 1 + offset; is_copy = TRUE; is_complete = TRUE; }
			
			if( (R00140110 & 0x000000FF) == 0x00000002 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 2 + offset; is_copy = TRUE; is_complete = TRUE; }
			
			if( (R00140110 & 0x000000FF) == 0x00000003 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (1920 * 4 * (i >> 1)) + 1920 * 3 + offset; is_copy = TRUE; is_complete = TRUE; }

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

