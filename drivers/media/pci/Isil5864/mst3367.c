#include "LINUXV4L2.h"
#include "sa7160.h"
#include "fh8735.h"
#include "mz0380.h"
#include "property.h"
#include "wrapper.h"

extern ULONG g_n_analog_decoder_brightness [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_contrast   [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_saturation [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_hue        [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_sharpness  [ 64 ][ 4 ] ;

BOOLEAN MST3367_ADC_INIT( CDevice * pDevice );

BOOLEAN MST3367_HDMI_INIT( CDevice * pDevice );

BOOLEAN MST3367_HDMI_RESET( CDevice * pDevice );

BOOLEAN MST3367_HDCP_RESET( CDevice * pDevice );

BOOLEAN MST3367_TMDS_HOT_PLUG( CDevice * pDevice, ULONG dwType );

VOID MST3367_AUDIO_SELECT( CDevice * pDevice, ULONG nAudioIn );

BYTE MST3367_GetRegister( CDevice * pDevice, BYTE nBank, BYTE nIndex )
{
//	return SA7160_GetAnalogVideoDecoderRegister( pDevice, nBank, nIndex );

	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) )
	{
		return SA7160_GetMST3367Register( pDevice, nBank, nIndex );
	}
	else if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) )
	{
		return FH8735_GetAnalogVideoDecoderRegister_EX2( pDevice, 0x00, 0x9C, nBank, nIndex );
	}
	else if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) )
	{
		return MZ0380_GetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, nBank, nIndex );
	}
	else
	{
		LINUXV4L2_DEBUG( KERN_INFO, "MST3367_GetRegister() error, not implemented for this ID\n" );
		return true;
	}

}

BOOLEAN MST3367_SetRegister( CDevice * pDevice, BYTE nBank, BYTE nIndex, BYTE nValue )
{
//	return SA7160_SetAnalogVideoDecoderRegister( pDevice, nBank, nIndex, nValue );
	
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) )
	{
		return SA7160_SetMST3367Register( pDevice, nBank, nIndex, nValue );
	}
	else if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) )
	{
		return FH8735_SetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, nBank, nIndex, nValue );
	}
	else if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) )
	{
		return MZ0380_SetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, nBank, nIndex, nValue );
	}
	else
	{
		LINUXV4L2_DEBUG( KERN_INFO, "MST3367_SetRegister() error, not implemented for this ID\n" );
		return true;
	}
}


enum {	RX_TMDS_HPD_OFF   = 0x00,
		
		RX_TMDS_A_HPD_ON  = 0x01,
		
		RX_TMDS_A_LINK_ON = 0x02,
		
		RX_TMDS_B_HPD_ON  = 0x10,
		
		RX_TMDS_B_LINK_ON = 0x20,
};

enum {	AUDIO_MSTAR       = 0x00,

		AUDIO_LINE_IN     = 0x01,
};

#include "MST3367_COLOR_ADJUST.C"

BOOLEAN MST3367_HwInitialize( CDevice * pDevice )
{

	LINUXV4L2_PRINT( KERN_INFO, "[%02d] MST3367_HwInitialize\n", pDevice->m_nKsDeviceNumber );

	#ifdef AME_SA7160
	
	SA7160_SYS_CFG * pSysCfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
	
	if( (pDevice->iManufacturer == 0x01 && pDevice->iProduct == 0x07) ||

		(pDevice->iManufacturer == 0x04 && pDevice->iProduct == 0x07) ||
				
		(pDevice->iManufacturer == 0x0A && pDevice->iProduct == 0x07) ||
	
		(pDevice->iManufacturer == 0x10) || 
	
		(pDevice->iManufacturer == 0x16) || 

		(pDevice->iManufacturer == 0x17)  ) {

		R0000E004 |= 0x00020000; // BYPASS.MODE
	}
	else {

		R0000E004 |= 0x00020000; // HDMI.SEL (0 = SDI / 1 = HDMI)
	}
	SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 & ~0x00010000 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) ); // GPIO.WR (HARDWARE.RESET)
	
	SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 |  0x00010000 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) ); // GPIO.WR (HARDWARE.RESET)
	
	SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 & ~0x00010000 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) ); // GPIO.WR (HARDWARE.RESET)

	if( pDevice->iManufacturer == 0x01 && pDevice->iProduct == 0x07 ) { SA7160_SetFpgaRegister( pDevice, 0x20, 0x00 ); }

	if( pDevice->iManufacturer == 0x04 && pDevice->iProduct == 0x07 ) { SA7160_SetFpgaRegister( pDevice, 0x20, 0x00 ); }

	if( pDevice->iManufacturer == 0x0A && pDevice->iProduct == 0x07 ) { SA7160_SetFpgaRegister( pDevice, 0x20, 0x00 ); }

	if( pDevice->iManufacturer == 0x10 ) { SA7160_SetFpgaRegister( pDevice, 0x20, 0x00 ); }

	if( pDevice->iManufacturer == 0x16 ) { SA7160_SetFpgaRegister( pDevice, 0x20, 0x00 ); }

	if( pDevice->iManufacturer == 0x17 ) { SA7160_SetFpgaRegister( pDevice, 0x20, 0x00 ); }

	#endif
	
	#ifdef AME_FH8735
	
	FH8735_SYS_CFG * pSysCfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	ULONG R00700000 = FH8735_GetRegister( pDevice, 1, 0x00700000 + 0x00000000 );

	FH8735_SetRegister( pDevice, 1, 0x00700000 + 0x00000000, R00700000 & ~0x00000100 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) ); // GPIO.WR (HARDWARE.RESET)

	FH8735_SetRegister( pDevice, 1, 0x00700000 + 0x00000000, R00700000 |  0x00000100 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) ); // GPIO.WR (HARDWARE.RESET)

	FH8735_SetRegister( pDevice, 1, 0x00700000 + 0x00000000, R00700000 & ~0x00000100 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) ); // GPIO.WR (HARDWARE.RESET)

	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * pSysCfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	MZ0380_SetGpioValue( pDevice, 9, 1 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) );

	MZ0380_SetGpioValue( pDevice, 9, 0 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) );

	MZ0380_SetGpioValue( pDevice, 9, 1 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) );

	MZ0380_SetGpioValue( pDevice, 8, 0 ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(50) );

	#endif


	pSysCfg->n_input_video_resolution_cx = 0;

	pSysCfg->n_input_video_resolution_cy = 0;

	pSysCfg->n_input_video_resolution_fps = 0;

	pSysCfg->n_input_video_resolution_fps_m = 0;

	pSysCfg->n_input_video_resolution_interleaved = 0;

	pSysCfg->n_input_video_resolution_vbi_lines = 0;

	pSysCfg->n_input_video_resolution_h_total = 0;

	pSysCfg->b_input_video_resolution_spliter_mode = FALSE;

	pSysCfg->b_input_video_resolution_external_sync = FALSE;

	pSysCfg->b_input_video_resolution_vga_period_us_counts = 0;

	pSysCfg->b_input_video_signal_changed = FALSE;

	pSysCfg->n_input_audio_sampling_frequency = 0;

	// -----> RxInit()

	// -----> RxTmdsHotPlug()
	//
	MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_HPD_OFF );

	// -----> RxGeneralInit() -----> RxGeneralInitTbl [1.35]
	// 
	MST3367_SetRegister( pDevice, 0x00, 0x41, 0x6F );

	MST3367_SetRegister( pDevice, 0x00, 0xB8, 0x00 );

	// -----> RxTmdsInit() -----> RxTmdsInitTbl [1.35]
	// 
	MST3367_SetRegister( pDevice, 0x01, 0x0F, 0x02 );

	MST3367_SetRegister( pDevice, 0x01, 0x16, 0x30 );

	MST3367_SetRegister( pDevice, 0x01, 0x17, 0x00 );

	MST3367_SetRegister( pDevice, 0x01, 0x18, 0x00 );

	MST3367_SetRegister( pDevice, 0x01, 0x19, 0x00 );
	
	MST3367_SetRegister( pDevice, 0x01, 0x1A, 0x50 );

	BYTE R012A = MST3367_GetRegister( pDevice, 0x01, 0x2A ); R012A &= ~0x07; R012A |= 0x07;

	MST3367_SetRegister( pDevice, 0x01, 0x2A, R012A );

	MST3367_SetRegister( pDevice, 0x02, 0x08, 0x03 );

	// -----> RxHdcpInit() -----> RxHdcpInitTbl [1.35]
	//

#ifdef MEDICAL
//disable HDCP
	MST3367_SetRegister( pDevice, 0x01, 0x24, 0x41 );
#else
//reveice HDCP
	MST3367_SetRegister( pDevice, 0x01, 0x24, 0x40 );
#endif

	MST3367_SetRegister( pDevice, 0x01, 0x25, 0x00 );

	if( MST3367_GetRegister( pDevice, 0x01, 0x24) & 1 ) {

		MST3367_SetRegister( pDevice, 0x01, 0x26, 0x00 );

		int count = 5;
		for( count = 5; count; --count ) {

			MST3367_SetRegister( pDevice, 0x01, 0x27, 0x00 );
		}
	}

	MST3367_SetRegister( pDevice, 0x01, 0x30, 0x80 );

	MST3367_SetRegister( pDevice, 0x01, 0x31, 0x00 );

	MST3367_SetRegister( pDevice, 0x01, 0x32, 0x00 );

	// -----> RxVideoInit() -----> RxVideoInitTbl [1.35]
	//
	MST3367_SetRegister( pDevice, 0x00, 0xB0, 0x14 );

	BYTE R00AE = MST3367_GetRegister( pDevice, 0x00, 0xAE ); R00AE &= ~0x04; R00AE |= 0x04;

	MST3367_SetRegister( pDevice, 0x00, 0xAE, R00AE );

	MST3367_SetRegister( pDevice, 0x00, 0xAD, 0x05 ); // ENABLE LOW.PASS FILTER (解決邊緣漏色問題)

	MST3367_SetRegister( pDevice, 0x00, 0xB1, 0xC0 );

	MST3367_SetRegister( pDevice, 0x00, 0xB2, 0x00 );

	MST3367_SetRegister( pDevice, 0x00, 0xB3, 0x00 );

	MST3367_SetRegister( pDevice, 0x00, 0xB4, 0x55 );

	// -----> RxAudioInit() -----> RxAudioInitTbl [1.35]
	// 
	BYTE R00B4 = MST3367_GetRegister( pDevice, 0x00, 0xB4 ); R00B4 &= ~0x03; R00B4 |= 0x00;

	MST3367_SetRegister( pDevice, 0x00, 0xB4, R00B4 );

	MST3367_SetRegister( pDevice, 0x02, 0x01, 0x61 ); // 0x51 -> 0x61

	MST3367_SetRegister( pDevice, 0x02, 0x02, 0xF5 );

	BYTE R0203 = MST3367_GetRegister( pDevice, 0x02, 0x03 ); R0203 &= ~0x02; R0203 |= 0x02;

	MST3367_SetRegister( pDevice, 0x02, 0x03, R0203 );

	MST3367_SetRegister( pDevice, 0x02, 0x04, 0x01 );

	MST3367_SetRegister( pDevice, 0x02, 0x05, 0x00 );

	MST3367_SetRegister( pDevice, 0x02, 0x06, 0x08 );

	MST3367_SetRegister( pDevice, 0x02, 0x1C, 0x1A );

	MST3367_SetRegister( pDevice, 0x02, 0x1D, 0x00 );

	MST3367_SetRegister( pDevice, 0x02, 0x1E, 0x00 );

	MST3367_SetRegister( pDevice, 0x02, 0x1F, 0x00 );

	BYTE R0225 = MST3367_GetRegister( pDevice, 0x02, 0x25 ); R0225 &= ~0xA2; R0225 |= 0xA2;

	MST3367_SetRegister( pDevice, 0x02, 0x25, R0225 );

	///////////////////////////////////////////////////////////////////

	BYTE R0202 = MST3367_GetRegister( pDevice, 0x02, 0x02 ); R0202 &= ~0x80; R0202 |= 0x80;

	MST3367_SetRegister( pDevice, 0x02, 0x02, R0202 );

	BYTE R0207 = MST3367_GetRegister( pDevice, 0x02, 0x07 ); R0207 &= ~0x04; R0207 |= 0x04;

	MST3367_SetRegister( pDevice, 0x02, 0x07, R0207 );

	MST3367_SetRegister( pDevice, 0x02, 0x17, 0xC0 );

	MST3367_SetRegister( pDevice, 0x02, 0x19, 0xFF );

	MST3367_SetRegister( pDevice, 0x02, 0x1A, 0xFF );

	MST3367_SetRegister( pDevice, 0x02, 0x1B, 0xFC );

	MST3367_SetRegister( pDevice, 0x02, 0x20, 0x00 );

	BYTE R0221 = MST3367_GetRegister( pDevice, 0x02, 0x21 ); R0221 &= ~0x03; R0221 |= 0x00;

	MST3367_SetRegister( pDevice, 0x02, 0x21, R0221 );

	MST3367_SetRegister( pDevice, 0x02, 0x22, 0x26 );

	MST3367_SetRegister( pDevice, 0x02, 0x27, 0x00 );

	BYTE R022E = MST3367_GetRegister( pDevice, 0x02, 0x2E ); R022E &= ~0xA1; R022E |= 0xA1;

	MST3367_SetRegister( pDevice, 0x02, 0x2E, R022E );

	// -----> RxSwitchSource()
	// 
	{	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

			pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { // DVI.DIGITAL (TMDS.A)

			MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_HPD_OFF );

			MST3367_HDCP_RESET( pDevice );

			MST3367_HDMI_RESET( pDevice );

			if( (pDevice->iProduct == 0xF5 && pDevice->iManufacturer == 0x07) || // MDIN.HDMI

				(pDevice->iManufacturer == 0x0C) ||  // SC500N1.MC.HDV

				(pDevice->iManufacturer == 0xA0) ||  // UB5A0N4.HDV
				
				(pDevice->iManufacturer == 0xAC) ||  // SC5A0N1.MC.HDV
				
				(pDevice->iManufacturer == 0xC0) ) { // SC5C0N1.HDV


				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x81 );

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_A_LINK_ON );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else if( (pDevice->iManufacturer == 0xA1) ||  // PD.PD5A0.MST3367
				
					 (pDevice->iManufacturer == 0xA7) ) { // PD.PD5A0.MST3367

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x89 );

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_A_LINK_ON );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else if( (pDevice->iManufacturer == 0x04 && pDevice->iProduct == 0x07) || // EURESYS.SC512N1.DVI.1ST
				
					 (pDevice->iManufacturer == 0x0A && pDevice->iProduct == 0x07) || // EURESYS.SC512N1.DVI.1ST
			
					 (pDevice->iManufacturer == 0x16) ||  // DVI.DIGITAL
				
					 (pDevice->iManufacturer == 0x17) ) { // DVI.DIGITAL

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x81 );

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_A_LINK_ON );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else if( (pDevice->iProduct == 0x75 && pDevice->iManufacturer == 0x81) ) { // ZONKEY.AD.BOARD.CH1

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x81 );

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_A_LINK_ON );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_MSTAR );
			}
			else if( (pDevice->iProduct == 0x75 && pDevice->iManufacturer == 0x80) ) { // ZONKEY.AD.BOARD.CH2

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x89 );

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_B_HPD_ON | RX_TMDS_B_LINK_ON );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_MSTAR );
			}
			else if( pDevice->idProduct == 0xF531 ) { // UB-530 DEMPA REV 1.0

		
				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x81 );

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_B_HPD_ON | RX_TMDS_B_LINK_ON );

				if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

					MST3367_AUDIO_SELECT( pDevice, AUDIO_MSTAR );
				}
				if( pDevice->m_nAnalogCrossbarAudioInputProperty == 1 ) {

					MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
				}
			}
			else {

				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ) { 

					MST3367_SetRegister( pDevice, 0x00, 0x51, 0x89 );

					MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_B_HPD_ON | RX_TMDS_B_LINK_ON );
				}
				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { 

					MST3367_SetRegister( pDevice, 0x00, 0x51, 0x81 );

					MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_A_LINK_ON );
				}
				if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

					MST3367_AUDIO_SELECT( pDevice, AUDIO_MSTAR );
				}
				if( pDevice->m_nAnalogCrossbarAudioInputProperty == 1 ) {

					MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
				}
			}
			wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(200) );

			MST3367_SetRegister( pDevice, 0x00, 0xB7, 0x00 );

			MST3367_HDMI_INIT( pDevice );
		}
		if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||  // COMPONENT

			pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

			if( (pDevice->iManufacturer == 0x04 && pDevice->iProduct == 0x07) || // EURESYS.SC512N1.DVI.1ST
				
				(pDevice->iManufacturer == 0x0A && pDevice->iProduct == 0x07) || // EURESYS.SC512N1.DVI.1ST

				(pDevice->iManufacturer == 0x16) ||  // DVI.DIGITAL
				
				(pDevice->iManufacturer == 0x17) ) { // DVI.DIGITAL

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_A_LINK_ON );
			}
			else if( pDevice->idVendor == 0x05CA && pDevice->idProduct == 0x0901 ) { // RICHO.SC530

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_A_HPD_ON | RX_TMDS_B_HPD_ON );
			}
			else {

				MST3367_TMDS_HOT_PLUG( pDevice, RX_TMDS_HPD_OFF );
			}
			MST3367_HDCP_RESET( pDevice );

			MST3367_HDMI_RESET( pDevice );

			if( (pDevice->iProduct == 0xF5 && pDevice->iManufacturer == 0x05) ||  // DVI.ANALOG

				(pDevice->iProduct == 0xF5 && pDevice->iManufacturer == 0x06) ) { // COMPONENT

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x01 );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else if( pDevice->iManufacturer == 0x0C ||
				
					 pDevice->iManufacturer == 0x16 ) { // MC.MST3367

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x01 );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else if( (pDevice->iProduct == 0x75 && pDevice->iManufacturer == 0x81) ) { // ZONKEY.AD.BOARD.CH1

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x01 );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else if( (pDevice->iManufacturer == 0xA0) ||  // UB5A0N4.HDV
				
					 (pDevice->iManufacturer == 0xA1) ||  // PD5A0N1.HDV
				
					 (pDevice->iManufacturer == 0xA7) ||  // PD5A0N1.HDV

					 (pDevice->iManufacturer == 0xAC) ||  // SC5A0N1.MC.HDV

					 (pDevice->iManufacturer == 0xC0) ) { // SC5C0N1.HDV

				MST3367_SetRegister( pDevice, 0x00, 0x51, 0x01 );

				MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
			}
			else {

				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ) { 

					//if( pDevice->m_nCustomAnalogVideoDviYCbCrProperty == 1 ) { 

					//	MST3367_SetRegister( pDevice, 0x00, 0x51, 0x01 );
					//}
					//else {

						MST3367_SetRegister( pDevice, 0x00, 0x51, 0x21 );
					//}
					MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
				}
				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { 

					MST3367_SetRegister( pDevice, 0x00, 0x51, 0x01 );

					MST3367_AUDIO_SELECT( pDevice, AUDIO_LINE_IN );
				}
			}
			MST3367_ADC_INIT( pDevice );
		}
	}
	

	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 || 
			
		pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) {

		LINUXV4L2_DEBUG( KERN_INFO, "MST3367 DELAY\n" ); wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( 1000 ) );

	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_HwInitialize() success\n", pDevice->m_nKsDeviceNumber );

	return TRUE;
}

ULONG MST3367_GET_FREQV( CDevice * pDevice )
{
    WORD vperiod = 0;
	
	WORD vfreq = 0;

	ULONG m = 0;

    vperiod = MST3367_GetRegister( pDevice, 0x00, 0x59 ) & 0x3F;
	
    vperiod = (vperiod << 8) + MST3367_GetRegister( pDevice, 0x00, 0x5A );
	
	if( vperiod > 0 ) {

		vfreq = (WORD)(1250000L / (DWORD)(vperiod));
	}
	if( vfreq > 1190 && vfreq < 1210 ) {

		if( vperiod <= 1042 ) {

			vfreq = 1200;
		}
		else {

			vfreq = 1199; m = 1;	
		}
	}
	if( vfreq > 990 && vfreq < 1010 ) { vfreq = 1000; }

	if( vfreq > 840 && vfreq < 860 ) { vfreq = 850;	}

	if( vfreq > 740 && vfreq < 760 ) { vfreq = 750;	}

	if( vfreq > 710 && vfreq < 730 ) { vfreq = 720;	}	

	if( vfreq > 690 && vfreq < 709 ) { vfreq = 700;	}

	if( vfreq > 590 && vfreq < 610 ) {

		if( vperiod < 2086 ) {

			vfreq = 600;
		}
		else {

			vfreq = 599; m = 1;
		}
	}
	if( vfreq > 550 && vfreq < 560 ) { vfreq = 560;	}

	if( vfreq > 490 && vfreq < 510 ) { vfreq = 500;	}		

	if( vfreq > 240 && vfreq < 260 ) { vfreq = 250;	}	

	if( vfreq > 235 && vfreq < 245 ) {

		if( vperiod > 5216 ) {

			vfreq = 239; m = 1;
		}
		else {

			vfreq = 240;
		}
	}
	if( vfreq > 290 && vfreq < 310 ) {

		if( vperiod > 4173 ) {

			vfreq = 299; m = 1;
		}
		else {

			vfreq = 300;
		}
	}
	return m;

//	if( vperiod == 0 ) {
//		
//		return 0;
//	}		
//	return vfreq;
}

ULONG MST3367_ADC_AUTO_PHASE( CDevice * pDevice )
{

	#ifdef AME_SA7160
	
	SA7160_SYS_CFG * pSysCfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_FH8735
	
	FH8735_SYS_CFG * pSysCfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * pSysCfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif


	if( pDevice->m_nAnalogCrossbarVideoInputProperty != 2 &&
		
		pDevice->m_nAnalogCrossbarVideoInputProperty != 3 ) { 

		return 0;
	}
	if( (pDevice->m_nCustomAnalogVideoAutoPhaseTypeProperty[ 0 ] & 0x80000000) == 0x00000000 ) {

		ULONG j = pDevice->m_nCustomAnalogVideoAutoPhaseTypeProperty[ 0 ] & 0x3FFFFFFF;

		MST3367_SetRegister( pDevice, 0x00, 0x04, (BYTE)(j << 2) );

		LINUXV4L2_DEBUG( KERN_INFO, "MST3367_ADC_AUTO_PHASE( %d )\n", j );

		return j;
	}

	ULONG n_good_phase_res = 0x00000000;

	ULONG j = 0;

	BYTE R1E = MST3367_GetRegister( pDevice, 0x00, 0x1E ); 

	BYTE R1F = MST3367_GetRegister( pDevice, 0x00, 0x1F ); 
	
	MST3367_SetRegister( pDevice, 0x00, 0x1E, 0x00 );

	MST3367_SetRegister( pDevice, 0x00, 0x1F, 0x00 );

	MST3367_SetRegister( pDevice, 0x00, 0xE0, 0x00 );

	// -----> RxAdcAutoPhase() [1.35]
	//
	ULONG i = 0;
	for( i = 0 ; i < 65 ; i += 4 ) {

		ULONG n_phase_res = 0;

		MST3367_SetRegister( pDevice, 0x00, 0x04, (BYTE)((i % 64) << 2) );

		MST3367_SetRegister( pDevice, 0x00, 0xE0, 0x80 );

		wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( 5 ) );

		MST3367_SetRegister( pDevice, 0x00, 0xE0, 0x40 );

		n_phase_res += (((ULONG)(MST3367_GetRegister( pDevice, 0x00, 0xE9 ) & 0x03)) << 24) +

					   (((ULONG)(MST3367_GetRegister( pDevice, 0x00, 0xE8 ) & 0xFF)) << 16) +

					   (((ULONG)(MST3367_GetRegister( pDevice, 0x00, 0xE7 ) & 0xFF)) <<  8) +

					   (((ULONG)(MST3367_GetRegister( pDevice, 0x00, 0xE6 ) & 0xFF)) <<  0);

		LINUXV4L2_DEBUG( KERN_INFO, "MST3367_ADC_AUTO_PHASE( %02d -> %02d )", i, n_phase_res );




		if( i == 0 ) { continue ; }

		if( n_phase_res > n_good_phase_res ) {

			n_good_phase_res = n_phase_res;

			j = (i % 64);
		}
	}
	MST3367_SetRegister( pDevice, 0x00, 0xE0, 0x00 );

	MST3367_SetRegister( pDevice, 0x00, 0x1E, R1E );

	MST3367_SetRegister( pDevice, 0x00, 0x1F, R1F );

	MST3367_SetRegister( pDevice, 0x00, 0x04, (BYTE)(j << 2) );

	pSysCfg->n_input_video_good_adc_phase = j;

	LINUXV4L2_DEBUG( KERN_INFO, "MST3367_ADC_AUTO_PHASE( %d )\n", j );


	return j;

}

BOOLEAN MST3367_ADC_INIT( CDevice * pDevice )
{
	#ifdef AME_SA7160
	
	SA7160_SYS_CFG * pSysCfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_FH8735
	
	FH8735_SYS_CFG * pSysCfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * pSysCfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif


	// -----> RxAdcInit() [1.35]
	//
	MST3367_SetRegister( pDevice, 0x00, 0x00, 0x80 );

	MST3367_SetRegister( pDevice, 0x00, 0x24, 0xC0 );

	MST3367_SetRegister( pDevice, 0x00, 0x00, 0x00 );

	BYTE R0021 = MST3367_GetRegister( pDevice, 0x00, 0x21 ); R0021 &= ~0x07; R0021 |= 0x01;

	MST3367_SetRegister( pDevice, 0x00, 0x21, R0021 );

	MST3367_SetRegister( pDevice, 0x00, 0x04, (BYTE)(pSysCfg->n_input_video_good_adc_phase << 2) );

	// [2014.01.13] [PATCH PROGRAM] 顏大哥建議
	//
	BYTE R0010 = MST3367_GetRegister( pDevice, 0x00, 0x10 ); R0010 &= 0x0F; R0010 |= 0xD0;

	MST3367_SetRegister( pDevice, 0x00, 0x10, R0010 );

	MST3367_SetRegister( pDevice, 0x00, 0x11, 0x2D );

	MST3367_SetRegister( pDevice, 0x00, 0x20, 0xC0 );

	MST3367_SetRegister( pDevice, 0x00, 0x21, 0x04 );

	// [2011.11.15] [PATCH PROGRAM] 顏大哥建議
	//
	BYTE R000E = MST3367_GetRegister( pDevice, 0x00, 0x0E ); R000E &= 0xCF;  R000E |= 0x20;

	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ) { // COMPONENT

		MST3367_SetRegister( pDevice, 0x00, 0x0E, R000E | 0x10 ); // SOG
	}
	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

		MST3367_SetRegister( pDevice, 0x00, 0x0E, R000E | 0x00 ); // HSYNC
	}

	return TRUE;
}

ULONGLONG MST3367_ADC_MODE_DETECT( CDevice * pDevice, ULONG* n_h_period_us, ULONG* n_v_period_us, ULONG* nHTotal, ULONG* nVTotal, ULONG* nHPeriod, ULONG* nVPeriod, ULONG* nDetectDelay )
{
	#ifdef AME_SA7160
	
	SA7160_SYS_CFG * pSysCfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_FH8735
	
	FH8735_SYS_CFG * pSysCfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * pSysCfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	// -----> RxAdcModeDetect() [1.35]
	//
	BYTE status = 0x00;

	BYTE R0014 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

	BYTE R0015 = MST3367_GetRegister( pDevice, 0x00, 0x15 );

	if( ((R0014 & 0x80) == 0x80) && ((R0014 & 0x10) == 0x10) ) { status = 1; }

	if( ((R0014 & 0x80) == 0x80) && ((R0014 & 0x10) == 0x00) ) { status = 2; }

	if( ((R0014 & 0x80) == 0x00) && ((R0014 & 0x10) == 0x00) && ((R0015 & 0x40) == 0x40) ) { status = 3; }

	if( status > 0 ) {

		ULONG R0057 = MST3367_GetRegister( pDevice, 0x00, 0x57 ) & 0x3F;

		ULONG R0058 = MST3367_GetRegister( pDevice, 0x00, 0x58 ) & 0xFF;

		ULONG R0059 = MST3367_GetRegister( pDevice, 0x00, 0x59 ) & 0x3F;

		ULONG R005A = MST3367_GetRegister( pDevice, 0x00, 0x5A ) & 0xFF;

		ULONG R005B = MST3367_GetRegister( pDevice, 0x00, 0x5B ) & 0x07;

		ULONG R005C = MST3367_GetRegister( pDevice, 0x00, 0x5C ) & 0xFF;

		ULONG R005D = MST3367_GetRegister( pDevice, 0x00, 0x5D ) & 0xFF;

		ULONG R005E = MST3367_GetRegister( pDevice, 0x00, 0x5E ) & 0xE0;

		ULONG R005F = MST3367_GetRegister( pDevice, 0x00, 0x5F ) & 0x02;

		ULONG hperiod_us = (R0057 << 8) | (R0058 << 0);

		ULONG vperiod_us = (R0059 << 8) | (R005A << 0);

		ULONGLONG wx = (((R0057 << 8) | (R0058 << 0)) > 0) ? ((1600000) / ((R0057 << 8) | (R0058 << 0))) : (0); // H.TOTAL (H.FREQ)

		ULONGLONG cx = (((R005D << 4) | (R005E >> 4))); // H.TOTAL

		ULONGLONG wy = (((R005B << 8) | (R005C << 0))); // V.TOTAL

		ULONGLONG fv = (((R0059 << 8) | (R005A << 0)) > 0) ? ((1250000) / ((R0059 << 8) | (R005A << 0))) : (0); // V.FREQUENCY (V.FREQ)

		ULONGLONG cs = (R005F & 0x02) ? 1 : 0; // I / P

		*n_h_period_us = hperiod_us;

		*n_v_period_us = vperiod_us;

		*nHTotal = (wx > 0) ? (ULONG)(((1650000 / ((ULONG)wx)) / 8) * 8) : (0);

		*nVTotal = (hperiod_us > 0) ? (ULONG)((vperiod_us * 16 * 80 + (hperiod_us / 2)) / hperiod_us) : (0);

		*nHPeriod = (ULONG)(wx);

		*nVPeriod = (ULONG)(fv);

		*nDetectDelay = (R0059 << 8) | (R005A << 0);

		*nDetectDelay = ((*nDetectDelay + 63) * 2) / (125); // unit : ms

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_ADC_MODE_DETECT( %d x %d x %d x %d x %d x status = %d )\n", pDevice->m_nKsDeviceNumber,(ULONG)(wx), (ULONG)(cx), (ULONG)(wy), (ULONG)(fv), (ULONG)(cs), status);

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_ADC_MODE_DETECT( ht:%d x vt:%d x hp:%d x vp:%d x delay:%d )\n", pDevice->m_nKsDeviceNumber, *nHTotal, *nVTotal, *nHPeriod, *nVPeriod, *nDetectDelay);

		return (0x8000000000000000) | (cs << 48) | (fv << 32) | (wx << 16) | (wy << 0);
	}
	else {

		nHTotal = 0;

		nVTotal = 0;

		nHPeriod = 0;

		nVPeriod = 0;

		nDetectDelay = 0;
	}
	return 0x0000000000000000;
}

ULONG MST3367_TMDS_GET_TYPE( CDevice * pDevice )
{
	// -----> RxTmdsGetType() [1.35]
	//
	ULONG type = 0x00000000; // RX_TMDS_DVI

	BYTE R0101 = MST3367_GetRegister( pDevice, 0x01, 0x01 );

	BYTE R0134 = MST3367_GetRegister( pDevice, 0x01, 0x34 );

	if( R0101 & 0x04 ) { type |= 0x00000001; } // RX_TMDS_HDMI

	if( R0101 & 0x01 ) { type |= 0x00000002; } // DVI / HDMI WITH HDCP

	if( R0134 & 0x80 ) { type |= 0x00000002; } // RX_TMDS_HDMI_HDCP / RX_TMDS_DVI_HDCP

	//LINUXV4L2_DEBUG( KERN_INFO, "MST3367_TMDS_GET_TYPE( %08X )\n", type );

	return type;
}

BOOLEAN MST3367_TMDS_HOT_PLUG( CDevice * pDevice, ULONG dwType )
{
	// -----> RxTmdsHotPlug() [1.35]
	//
	#ifdef AME_SA7160

	ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 );

	if( dwType & RX_TMDS_A_HPD_ON ) { R0000E004 &= ~(1 << 20); } else { R0000E004 |= (1 << 20); } // HPD0.CTRL

	if( dwType & RX_TMDS_B_HPD_ON ) { R0000E004 &= ~(1 << 26); } else { R0000E004 |= (1 << 26); } // HPD1.CTRL

	SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

	#endif

	#ifdef AME_FH8735

	ULONG R00700000 = FH8735_GetRegister( pDevice, 1, 0x00700000 + 0x00000000 );

	if( dwType & RX_TMDS_A_HPD_ON ) { R00700000 &= ~(1 <<  9); } else { R00700000 |= (1 <<  9); } // HPD0.CTRL

	if( dwType & RX_TMDS_B_HPD_ON ) { R00700000 &= ~(1 << 10); } else { R00700000 |= (1 << 10); } // HPD1.CTRL

	FH8735_SetRegister( pDevice, 1, 0x00700000 + 0x00000000, R00700000 );

	#endif

	#ifdef AME_MZ0380

	if( dwType & RX_TMDS_A_HPD_ON ) { MZ0380_SetGpioValue( pDevice, 1, 0 ); } else { MZ0380_SetGpioValue( pDevice, 1, 1 ); } // HPD0.CTRL

//	if( dwType & RX_TMDS_B_HPD_ON ) { MZ0380_SetGpioValue( pDevice, 2, 0 ); } else { MZ0380_SetGpioValue( pDevice, 2, 1 ); } // HPD1.CTRL

	#endif

	///////////////////////////////////////////////////////////////////////

	BYTE R00B7 = MST3367_GetRegister( pDevice, 0x00, 0xB7 ); R00B7 |= 0x02;

	if( dwType & RX_TMDS_A_LINK_ON ) { R00B7 &= ~0x02; }

	if( dwType & RX_TMDS_B_LINK_ON ) { R00B7 &= ~0x02; }

	MST3367_SetRegister( pDevice, 0x00, 0xB7, R00B7 );

	return TRUE;
}

VOID MST3367_AUDIO_SELECT( CDevice * pDevice, ULONG nAudioIn )
{
	#ifdef AME_CY3014

	BYTE FPGA_R20 = CY3014_GetFpgaRegister( pDevice, 0x20 );

	if( nAudioIn == AUDIO_LINE_IN ) {

		FPGA_R20  |=  (1 <<  4); // I2S.SEL = 1
	}
	else {

		FPGA_R20  &= ~(1 <<  4); // I2S.SEL = 0
	}
	CY3014_SetFpgaRegister( pDevice, 0x20, FPGA_R20 );

	#endif

	#ifdef AME_SA7160

	ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 );

	if( nAudioIn == AUDIO_LINE_IN ) {

		R0000E004 &= ~(1 <<  5); // I2S.SEL = 0
	}
	else {

		R0000E004 |=  (1 <<  5); // I2S.SEL = 1
	}
	SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

	#endif

	#ifdef AME_FH8735

	ULONG R00700000 = FH8735_GetRegister( pDevice, 1, 0x00700000 + 0x00000000 );

	if( nAudioIn == AUDIO_LINE_IN ) {

		R00700000 |=  (1 << 12); // I2S.SEL = 1
	}
	else {

		R00700000 &= ~(1 << 12); // I2S.SEL = 0
	}
	FH8735_SetRegister( pDevice, 1, 0x00700000 + 0x00000000, R00700000 );

	#endif


	#ifdef AME_MZ0380

	#endif

}


BOOLEAN MST3367_HDMI_INIT( CDevice * pDevice )
{
	// -----> RxHdmiInit() -----> RxHdmiInitTbl [1.35]
	// 
	BYTE R0201 = MST3367_GetRegister( pDevice, 0x02, 0x01 ); R0201 &= ~0xF0; R0201 |= 0x40; R0201 |= 0x20;

	MST3367_SetRegister( pDevice, 0x02, 0x01, R0201 );

	BYTE R0204 = MST3367_GetRegister( pDevice, 0x02, 0x04 ); R0204 &= ~0x01; R0204 |= 0x01;

	MST3367_SetRegister( pDevice, 0x02, 0x04, R0204 );

	MST3367_SetRegister( pDevice, 0x02, 0x06, 0x08 );

	BYTE R0209 = MST3367_GetRegister( pDevice, 0x02, 0x09 ); R0209 &= ~0x20; R0209 |= 0x20;

	MST3367_SetRegister( pDevice, 0x02, 0x09, R0209 );

	BYTE R0054 = MST3367_GetRegister( pDevice, 0x00, 0x54 ); R0054 &= ~0x10; R0054 |= 0x00;

	MST3367_SetRegister( pDevice, 0x00, 0x54, R0054 );

	BYTE R00AC = MST3367_GetRegister( pDevice, 0x00, 0xAC ); R00AC &= ~0x80; R00AC |= 0x80;

	MST3367_SetRegister( pDevice, 0x00, 0xAC, R00AC );

	BYTE R0000 = MST3367_GetRegister( pDevice, 0x00, 0x00 ); R0000 &= ~0x80; R0000 |= 0x80;

	MST3367_SetRegister( pDevice, 0x00, 0x00, R0000 );

	BYTE R00CE = MST3367_GetRegister( pDevice, 0x00, 0xCE ); R00CE &= ~0x80; R00CE |= 0x80;

	MST3367_SetRegister( pDevice, 0x00, 0xCE, R00CE );

	BYTE R00CF = MST3367_GetRegister( pDevice, 0x00, 0xCF ); R00CF &= ~0x07; R00CF |= 0x02;

	MST3367_SetRegister( pDevice, 0x00, 0xCF, R00CF );

	R0000 = MST3367_GetRegister( pDevice, 0x00, 0x00 ); R0000 &= ~0x80; R0000 |= 0x00;

	MST3367_SetRegister( pDevice, 0x00, 0x00, R0000 );

	return TRUE;
}

BOOLEAN MST3367_HDMI_RESET( CDevice * pDevice )
{
	// -----> RxHdmiReset() [1.35]
	//
	MST3367_SetRegister( pDevice, 0x02, 0x07, 0xF4 );

	MST3367_SetRegister( pDevice, 0x02, 0x07, 0x04 );

	return TRUE;
}

BOOLEAN MST3367_HDCP_RESET( CDevice * pDevice )
{
	// -----> RxHdcpReset() [1.35]
	//
	MST3367_SetRegister( pDevice, 0x00, 0xB8, 0x10 ); // HDCP.RESET

	MST3367_SetRegister( pDevice, 0x00, 0xB8, 0x00 );

	return TRUE;
}

ULONGLONG MST3367_HDMI_MODE_DETECT( CDevice * pDevice, ULONG* nHTotal, ULONG* nVTotal, ULONG* nHPeriod, ULONG* nVPeriod, ULONG* nDetectDelay )
{
	// -----> RxHdmiModeDetect() [1.35]
	//
	BYTE R0055 = MST3367_GetRegister( pDevice, 0x00, 0x55 );
	
	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_HDMI_MODE_DETECT, R0055(0x%x)\n", pDevice->m_nKsDeviceNumber, R0055 );

	if( R0055 & 0x3C ) {

		BYTE R006A = MST3367_GetRegister( pDevice, 0x00, 0x6A ) & 0x0F;

		BYTE R006B = MST3367_GetRegister( pDevice, 0x00, 0x6B ) & 0xFF;

		BYTE R0057 = MST3367_GetRegister( pDevice, 0x00, 0x57 ) & 0x3F;

		BYTE R0058 = MST3367_GetRegister( pDevice, 0x00, 0x58 ) & 0xFF;

		BYTE R0059 = MST3367_GetRegister( pDevice, 0x00, 0x59 ) & 0x3F;

		BYTE R005A = MST3367_GetRegister( pDevice, 0x00, 0x5A ) & 0xFF;

		BYTE R005B = MST3367_GetRegister( pDevice, 0x00, 0x5B ) & 0x07;

		BYTE R005C = MST3367_GetRegister( pDevice, 0x00, 0x5C ) & 0xFF;

		BYTE R005F = MST3367_GetRegister( pDevice, 0x00, 0x5F ) & 0x02;

		BYTE R0229 = MST3367_GetRegister( pDevice, 0x02, 0x29 ) & 0x1F;

		BYTE R0228 = MST3367_GetRegister( pDevice, 0x02, 0x28 ) & 0xFF;

		ULONGLONG wx = (R006A << 8) | (R006B << 0); // H.TOTAL

		ULONGLONG wy = (R005B << 8) | (R005C << 0); // V.TOTAL

		ULONGLONG cx = (R0229 << 8) | (R0228 << 0); // H.ACTIVE

		ULONGLONG fh = (((R0057 << 8) | (R0058 << 0)) > 0) ? ((1600000) / ((R0057 << 8) | (R0058 << 0))) : (0); // H.PERIOD

		ULONGLONG fv = (((R0059 << 8) | (R005A << 0)) > 0) ? ((1250000) / ((R0059 << 8) | (R005A << 0))) : (0); // V.PERIOD

		ULONGLONG fi = (R005F >> 1); // INTERLEAVED

		*nHTotal = (ULONG)(wx);

		*nVTotal = (ULONG)(wy);

		*nHPeriod = (ULONG)(fh);

		*nVPeriod = (ULONG)(fv);

		*nDetectDelay = (R0059 << 8) | (R005A << 0);

		*nDetectDelay = ((*nDetectDelay + 63) * 2) / (125); // unit : ms

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_HDMI_MODE_DETECT( %d x %d x %d x %d x %d x R0055 = %02X %c )\n", pDevice->m_nKsDeviceNumber, (ULONG)(wx), (ULONG)(cx), (ULONG)(wy), (ULONG)(fh), (ULONG)(fv), R0055, fi ? 'i' : 'p' );
		
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_HDMI_MODE_DETECT, bank 1, R0001(0x%x)\n", pDevice->m_nKsDeviceNumber, MST3367_GetRegister( pDevice, 0x01, 0x01 ) );//HDMI 1XX, DVI 0XX
/*
HDMI_MD 2
HDCP_OP_STS 1
HDCP_MD 0
0x0: DVI, without HDCP.
001: DVI OESS* + HDCP, without advance cipher.
011: DVI EESS** + HDCP, with advance cipher.
1x0: HDMI EESS, without HDCP.
101: HDMI EESS + HDCP, without advance cipher.
111: HDMI + HDCP EESS, with advance cipher.
*OESS: Original Encryption Status Signaling.
**EESS: Enhanced Encryption Status Signaling
*/

		return (0x8000000000000000) | (fi << 62) | (fv << 48) | (cx << 32) | (wx << 16) | (wy << 0); // 舊的相容
	}
	else {

		nHTotal = 0;

		nVTotal = 0;

		nHPeriod = 0;

		nVPeriod = 0;

		nDetectDelay = 0;
	}
	return 0x0000000000000000;
}

ULONG MST3367_HdmiGetPacketStatus( CDevice * pDevice )
{
	// -----> RxHdmiPacketStatus()
	// 
	ULONG status = 0x00000000;

	BYTE R020B = MST3367_GetRegister( pDevice, 0x02, 0x0B ) & 0xFF;

	BYTE R020C = MST3367_GetRegister( pDevice, 0x02, 0x0C ) & 0x3F;

	BYTE R020E = MST3367_GetRegister( pDevice, 0x02, 0x0E ) & 0x08;

	status = (R020C << 8) | (R020B << 0);

	if( R020E & 0x08 ) { status |= 0x8000; }

//	LINUXV4L2_DEBUG( KERN_INFO, "MST3367_HDMI_GET_PACKET_STATUS( %08X )\n", status );

	return status;
}

ULONG MST3367_HdmiGetPacketColor( CDevice * pDevice )
{
	// -----> RxHdmiPacketColor()
	//
	ULONG color = 0x00000000; // RX_INPUT_RGB

	BYTE R0248 = MST3367_GetRegister( pDevice, 0x02, 0x48 ); R0248 &= 0x60;

	if( R0248 == 0x00 ) { color = 0x00000000; } // RX_INPUT_RGB

	if( R0248 == 0x20 ) { color = 0x00000001; } // RX_INPUT_YUV422

	if( R0248 == 0x40 ) { color = 0x00000002; } // RX_INPUT_YUV444

//	LINUXV4L2_DEBUG( KERN_INFO, "MST3367_HDMI_GET_PACKET_COLOR( %08X )\n", color );

	return color;
}

BOOLEAN MST3367_VideoFormatDetection( CDevice * pDevice )
{
	#ifdef AME_SA7160
	
	SA7160_SYS_CFG * pSysCfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_FH8735
	
	FH8735_SYS_CFG * pSysCfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * pSysCfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif


	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||  // COMPONENT

		pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

		ULONG n_signal_h_period_us = 0;

		ULONG n_signal_v_period_us = 0;

		ULONG n_signal_h_total = 0;

		ULONG n_signal_v_total = 0;

		ULONG n_signal_h_start = 0;

		ULONG n_signal_v_start = 0;

		ULONG n_signal_h_period = 0; 
		
		ULONG n_signal_v_period = 0;

		ULONG n_signal_detect_delay = 0;

		ULONGLONG n_signal_lock_status = MST3367_ADC_MODE_DETECT( pDevice, &n_signal_h_period_us, &n_signal_v_period_us, &n_signal_h_total, &n_signal_v_total, &n_signal_h_period, &n_signal_v_period, &n_signal_detect_delay );

		BOOL b_is_signal_vaild = FALSE;

		BOOL b_is_signal_changed = FALSE;

		if( n_signal_lock_status & 0x8000000000000000 ) {

			ULONG wx = (ULONG)((n_signal_lock_status & 0x000000000FFF0000) >> 16); // H.FREQ

			ULONG wy = (ULONG)((n_signal_lock_status & 0x0000000000000FFF) >>  0);

			ULONG fv = (ULONG)((n_signal_lock_status & 0x00000FFF00000000) >> 32); // V.FREQ

			ULONG cs = (ULONG)((n_signal_lock_status & 0x0001000000000000) >> 48);

			ULONG x = 0;

			ULONG y = 0;

			ULONG fps = 0;

			
			if( (wx >= (179 - 5)) && (wx <= (179 + 5)) && (fv >= (2443 -  5)) && (fv <= (2443 +  5))            ) { x = 1280; y =  720; fps = 24; } // YCBCR

			if( (wx >= (179 - 5)) && (wx <= (179 + 4)) && (fv >= (3533 -  5)) && (fv <= (3533 +  5))            ) { x = 1280; y =  720; fps = 24; } // YCBCR

			if( (wx >= (187 - 4)) && (wx <= (187 + 5)) && (fv >= (3533 -  5)) && (fv <= (3533 +  5))            ) { x = 1280; y =  720; fps = 25; } // YCBCR

			if( (wx >= (187 - 4)) && (wx <= (187 + 5)) && (fv >= ( 256 -  5)) && (fv <= ( 256 +  5))            ) { x = 1280; y =  720; fps = 25; } // YCBCR

			if( (wx >= (187 - 4)) && (wx <= (187 + 5))                                                          ) { x = 1280; y =  720; fps = 25; } // YCBCR

			if( (wx >= (224 - 5)) && (wx <= (224 + 5)) && (fv >= ( 144 -  5)) && (fv <= ( 144 +  5))            ) { x = 1280; y =  720; fps = 30; } // YCBCR


			if( (wx >= (269 - 5)) && (wx <= (269 + 5)) && (fv >= (240 -  5)) && (fv <= (240 +  5))              ) { x = 1920; y = 1080; fps = 24; }

			if( (wx >= (281 - 5)) && (wx <= (281 + 5)) && (fv >= (250 -  5)) && (fv <= (250 +  5))              ) { x = 1920; y = 1080; fps = 25; }

			if( (wx >= (337 - 5)) && (wx <= (337 + 5)) && (fv >= (300 -  5)) && (fv <= (300 +  5))              ) { x = 1920; y = 1080; fps = 30; }

			if( (wx >= (312 - 5)) && (wx <= (312 + 5)) && (fv >= (300 -  5)) && (fv <= (300 +  5))              ) { x = 3840; y = 1024; fps = 30; }

			if( pDevice->m_nCustomCompanyVision4CEProperty == 1 ) {
		
				if( (wx >= (156 - 5)) && (wx <= (156 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))          ) { x =  768; y =  288; fps = 50; }
			}
			else {

				if( (wx >= (156 - 5)) && (wx <= (156 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))          ) { x =  720; y =  288; fps = 50; }
			}
			if( (wx >= (156 - 5)) && (wx <= (156 + 5)) && (fv >= (500 - 50)) && (fv <= (500 + 50)) && (cs == 0) ) { x = 1440; y =  288; fps = 50; }

			if( (wx >= (276 - 5)) && (wx <= (276 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x = 1440; y =  540; fps = 50; } // ->

			if( (wx >= (281 - 5)) && (wx <= (281 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x = 1920; y =  540; fps = 50; } // ->

			if( (wx >= (313 - 5)) && (wx <= (313 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x =  720; y =  576; fps = 50; }

			if( (wx >= (375 - 5)) && (wx <= (375 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x = 1280; y =  720; fps = 50; }

			if( (wx >= (396 - 5)) && (wx <= (396 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x = 1024; y =  768; fps = 50; }

			if( (wx >= (527 - 5)) && (wx <= (527 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x = 1280; y = 1024; fps = 50; }

			if( (wx >= (563 - 5)) && (wx <= (563 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))              ) { x = 1920; y = 1080; fps = 50; }

			if( (wx >= (617 - 5)) && (wx <= (617 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))				) { x = 1600; y = 1200; fps = 50; }

			if( (wx >= (617 - 5)) && (wx <= (617 + 5)) && (fv >= (500 - 10)) && (fv <= (500 + 10))				) { x = 1920; y = 1200; fps = 50; }


			if( (wx >= (314 - 5)) && (wx <= (314 + 5)) && (fv >= (550 - 10)) && (fv <= (550 + 10))              ) { x =  768; y =  512; fps = 55; }

			if( (wx >= (363 - 5)) && (wx <= (363 + 5)) && (fv >= (550 - 10)) && (fv <= (550 + 10))              ) { x =  800; y =  600; fps = 55; }

			if( (wx >= (351 - 5)) && (wx <= (351 + 5)) && (fv >= (560 - 10)) && (fv <= (560 + 10))              ) { x =  800; y =  600; fps = 56; }


			if( (wx >= (157 - 5)) && (wx <= (157 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x =  720; y =  240; fps = 60; }

			if( (wx >= (157 - 5)) && (wx <= (157 + 5)) && (fv >= (600 - 50)) && (fv <= (600 + 50)) && (cs == 0) ) { x = 1440; y =  240; fps = 60; }

		//	if( (wx >= (315 - 5)) && (wx <= (315 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x =  720; y =  480; fps = 60; } // ->

			if( (wx >= (337 - 5)) && (wx <= (337 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1920; y =  540; fps = 60; }

			if( (wx >= (372 - 5)) && (wx <= (372 + 2)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1024; y =  600; fps = 60; } // <-

			if( (wx >= (379 - 4)) && (wx <= (379 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x =  800; y =  600; fps = 60; } // <-

		//	if( (wx >= (450 - 5)) && (wx <= (450 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1280; y =  720; fps = 60; } // ->

			if( (wx >= (484 - 3)) && (wx <= (484 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1024; y =  768; fps = 60; }

			if( (wx >= (496 - 5)) && (wx <= (496 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1280; y =  800; fps = 60; }

			if( (wx >= (555 - 5)) && (wx <= (555 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1440; y =  900; fps = 60; }

			if( (wx >= (600 - 5)) && (wx <= (600 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1280; y =  960; fps = 60; }

			if( (wx >= (640 - 5)) && (wx <= (640 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) && (wy >= (1066 - 5)) && (wy <= (1066 + 5)) ) { x = 1280; y = 1024; fps = 60; }

			if( (wx >= (646 - 5)) && (wx <= (646 + 5)) && (fv >= (599 - 10)) && (fv <= (599 + 10)) && (wy >= (1080 - 5)) && (wy <= (1080 + 5)) ) { x = 1400; y = 1050; fps = 61; }

			if( (wx >= (652 - 5)) && (wx <= (652 + 5)) && (fv >= (599 - 10)) && (fv <= (599 + 10)) && (wy >= (1089 - 5)) && (wy <= (1089 + 5)) ) { x = 1400; y = 1050; fps = 60; }

		//	if( (wx >= (675 - 5)) && (wx <= (675 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))              ) { x = 1920; y = 1080; fps = 60; } // ->

		//	if( (wx >= (749 - 5)) && (wx <= (749 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10))				) { x = 1600; y = 1200; fps = 60; } // ->


			if( (wx >= (564 - 5)) && (wx <= (564 + 5)) && (fv >= (700 - 10)) && (fv <= (700 + 10))              ) { x = 1024; y =  768; fps = 70; }


			if( (wx >= (378 - 5)) && (wx <= (378 + 5)) && (fv >= (720 - 10)) && (fv <= (720 + 10))              ) { x =  640; y =  480; fps = 72; }

			if( (wx >= (480 - 5)) && (wx <= (480 + 5)) && (fv >= (720 - 10)) && (fv <= (720 + 10))              ) { x =  800; y =  600; fps = 72; }

			
			if( (wx >= (374 - 5)) && (wx <= (374 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x =  640; y =  480; fps = 75; }

			if( (wx >= (393 - 5)) && (wx <= (393 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x =  800; y =  500; fps = 75; }

			if( (wx >= (468 - 5)) && (wx <= (468 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x =  800; y =  600; fps = 75; }

			if( (wx >= (491 - 5)) && (wx <= (491 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x =  832; y =  624; fps = 75; }

			if( (wx >= (563 - 5)) && (wx <= (563 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1280; y =  720; fps = 75; }

		//	if( (wx >= (600 - 5)) && (wx <= (600 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1024; y =  768; fps = 75; } // ->

		//	if( (wx >= (600 - 5)) && (wx <= (600 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1280; y =  768; fps = 75; } // ->

			if( (wx >= (624 - 5)) && (wx <= (624 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1280; y =  800; fps = 75; }

			if( (wx >= (674 - 5)) && (wx <= (674 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1152; y =  864; fps = 75; }

			if( (wx >= (750 - 5)) && (wx <= (750 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1280; y =  960; fps = 75; }

			if( (wx >= (799 - 5)) && (wx <= (799 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1280; y = 1024; fps = 75; }

		//	if( (wx >= (600 - 5)) && (wx <= (600 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1360; y =  768; fps = 75; } // ->

			if( (wx >= (705 - 5)) && (wx <= (705 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1440; y =  900; fps = 75; }

			if( (wx >= (936 -10)) && (wx <= (936 +10)) && (fv >= (750 - 10)) && (fv <= (750 + 10))              ) { x = 1600; y = 1200; fps = 75; }


			if( (wx >= (379 - 5)) && (wx <= (379 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x =  720; y =  400; fps = 85; }

			if( (wx >= (432 - 5)) && (wx <= (432 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x =  640; y =  480; fps = 85; }

			if( (wx >= (536 - 5)) && (wx <= (536 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x =  800; y =  600; fps = 85; }

			if( (wx >= (642 - 5)) && (wx <= (642 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1280; y =  720; fps = 85; }

		//	if( (wx >= (685 - 5)) && (wx <= (685 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1024; y =  768; fps = 85; } // ->

		//	if( (wx >= (685 - 5)) && (wx <= (685 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1280; y =  768; fps = 85; } // ->

			if( (wx >= (714 - 5)) && (wx <= (714 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1280; y =  800; fps = 85; }

			if( (wx >= (859 - 5)) && (wx <= (859 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1280; y =  960; fps = 85; }

			if( (wx >= (910 - 5)) && (wx <= (910 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1280; y = 1024; fps = 85; }

		//	if( (wx >= (685 - 5)) && (wx <= (685 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1360; y =  768; fps = 85; } // ->
			
			if( (wx >= (803 - 5)) && (wx <= (803 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10))              ) { x = 1440; y =  900; fps = 85; }


			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if( (wx >= (279 - 5)) && (wx <= (279 + 5))  && (fv >= (500 - 5)) && (fv <= (500 + 5)) )
			{ 
				BYTE R14 = MST3367_GetRegister( pDevice, 0x00, 0x14 );
				
				if( (R14 & 0x24) == (0x04) ) {

					x = 1440; y = 540; fps = 50;
				}
				else {

					x = 1920; y = 540; fps = 50;
				}
			}
			if( (wx >= (247 - 5)) && (wx <= (247 + 5)) && (fv >= (600 - 50)) && (fv <= (600 + 50)) ) { 
				
				if( (wy >= (420 - 5)) && (wy <= (420 + 5)) ) { 

				//	x = 496; y = 384; fps = 60;
					
					x = 640; y = 384; fps = 60;
				}
				else {

					x = 640; y = 400; fps = 60; 
				}	
			}
			if( (wx >= (299 - 3)) && (wx <= (299 + 3)) && (fv >= (599 - 5)) && (fv <= (599 + 5)) ) {

				if( (wy >= (500 - 5)) && (wy <= (500 + 5)) ) {

					x = 848; y = 480; fps = 60;
				}
			}
			if( (wx >= (450 - 5)) && (wx <= (450 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { 
				
				BYTE R14 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

				if( (R14 & 0x24) == (0x04) ) {

					x = 1280; y =  720; fps = 61; 
				}
				else {					

					x = 1280; y =  720; fps = 60; 
				}
			}
			if( (wx >= (477 - 5)) && (wx <= (477 + 3)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) {
				
				BYTE R14 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

				if( (R14 & 0x24) == (0x24) ) {

					x = 1360; y = 768; fps = 60;
				}
				else if( (R14 & 0x24) == (0x00) ) {

					x = 1360; y = 768; fps = 61;
				}
				else {

					x = 1280; y = 768; fps = 60;
				}
			}
			if( (wx >= (675 - 15)) && (wx <= (675 + 10)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { 

				BYTE R14 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

				if( (R14 & 0x24) == (0x04) ) {

					x = 1920; y = 1080; fps = 61;
				}
				else {					

					x = 1920; y = 1080; fps = 60;
				}
			}
			if( (wx >= (749 - 10)) && (wx <= (749 + 10)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { 
				
				if( (n_signal_h_total >= (2200 - 100)) && (n_signal_h_total <= (2200 + 100)) ) {

					x = 1600; y = 1200; fps = 60; 
				}
				else {

					x = 1920; y = 1200; fps = 60; 
				}
			}
			if( (wx >= (315 - 5)) && (wx <= (315 + 5)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) {

				if( (pDevice->m_nCustomAnalogVideoResolutionProperty == 0x028001E0) ) { x = 640; y = 480; fps = 60; } // 已經偵測了，不用再測

				else if( (pDevice->m_nCustomAnalogVideoResolutionProperty == 0x02D001E0) ) { x = 720; y = 480; fps = 60; } // 已經偵測了，不用再測
				
				else {
					
					BYTE R0054_BACKUP = MST3367_GetRegister( pDevice, 0x00, 0x54 );
			
					BYTE R000E_BACKUP = MST3367_GetRegister( pDevice, 0x00, 0x0E );
			
					BYTE R0054 = R0054_BACKUP & (~0x10);
			
					BYTE R000E = R000E_BACKUP | ( 0x08);
			
					MST3367_SetRegister( pDevice, 0x00, 0x54, R0054 );
			
					MST3367_SetRegister( pDevice, 0x00, 0x0E, R000E );
			
					x = 720; y = 480; fps = 60;
			
					ULONG c = 0 ;
					for( c = 0 ; c < 10 ; c++ ) {
			
						MST3367_SetRegister( pDevice, 0x00, 0x54, MST3367_GetRegister( pDevice, 0x00, 0x54 ) & (~0x10) ); 
			
						MST3367_SetRegister( pDevice, 0x00, 0x0E, MST3367_GetRegister( pDevice, 0x00, 0x0E ) | ( 0x08) );

						MST3367_SetRegister( pDevice, 0x00, 0x0E, MST3367_GetRegister( pDevice, 0x00, 0x0E ) ^ ( 0x04) );
				
						ULONG VTOAL1 = (MST3367_GetRegister( pDevice, 0x00, 0x5B ) << 8) | (MST3367_GetRegister( pDevice, 0x00, 0x5C ));
			
						MST3367_SetRegister( pDevice, 0x00, 0x0E, MST3367_GetRegister( pDevice, 0x00, 0x0E ) ^ ( 0x04) );
			
						MST3367_SetRegister( pDevice, 0x00, 0x0E, R000E ^ ( 0x04) );
			
						ULONG VTOAL2 = (MST3367_GetRegister( pDevice, 0x00, 0x5B ) << 8) | (MST3367_GetRegister( pDevice, 0x00, 0x5C ));

						#define DIFF( a, b ) ((a > b) ? (a - b) : (b - a))

						ULONG offset = DIFF( VTOAL1, VTOAL2 );

						if( offset == 2 ) { x = 640; y = 480; fps = 60; break; }

						if( offset == 6 ) { x = 720; y = 480; fps = 60; break; }
					}
					MST3367_SetRegister( pDevice, 0x00, 0x54, R0054_BACKUP );
			
					MST3367_SetRegister( pDevice, 0x00, 0x0E, R000E_BACKUP );
				}
			}
			if( (wx >= (600 - 5)) && (wx <= (600 + 5)) && (fv >= (750 - 10)) && (fv <= (750 + 10)) ) {
				
				BYTE R14 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

				if( (R14 & 0x24) == (0x24) ) {

					x = 1024; y = 768; fps = 75;
				}
				else {					

					x = 1280; y = 768; fps = 75; 

				//	x = 1360; y = 768; fps = 75; 
				}
			}
			if( (wx >= (685 - 5)) && (wx <= (685 + 5)) && (fv >= (850 - 10)) && (fv <= (850 + 10)) ) {
				
				BYTE R14 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

				if( (R14 & 0x24) == (0x24) ) {

					x = 1024; y = 768; fps = 85;
				}
				else {					

					x = 1280; y = 768; fps = 85; 

				//	x = 1360; y = 768; fps = 85; 
				}
			}
			if( (fps == 0) || (x == 0) || (y == 0) ) { // 啟動顏大哥的自動演算偵測法

				if( pDevice->m_nAnalogCrossbarVideoInputProperty != 3 ) { goto EXIT_AUTO; }

				if( pSysCfg->b_input_video_resolution_vga_period_us_counts == 0 ) {

					pSysCfg->b_input_video_resolution_vga_h_period_us = n_signal_h_period_us;

					pSysCfg->b_input_video_resolution_vga_v_period_us = n_signal_v_period_us;

					pSysCfg->b_input_video_resolution_vga_h_period_us_sum = n_signal_h_period_us;

					pSysCfg->b_input_video_resolution_vga_v_period_us_sum = n_signal_v_period_us;

					pSysCfg->b_input_video_resolution_vga_period_us_counts = 1;
				}
				else {

					ULONG dx = 0;

					ULONG dy = 0;

					if( pSysCfg->b_input_video_resolution_vga_h_period_us > n_signal_h_period_us ) {

						dx = pSysCfg->b_input_video_resolution_vga_h_period_us - n_signal_h_period_us;
					}
					else {

						dx = n_signal_h_period_us - pSysCfg->b_input_video_resolution_vga_h_period_us;
					}
					if( pSysCfg->b_input_video_resolution_vga_v_period_us > n_signal_v_period_us ) {

						dy = pSysCfg->b_input_video_resolution_vga_v_period_us - n_signal_v_period_us;
					}
					else {

						dy = n_signal_v_period_us - pSysCfg->b_input_video_resolution_vga_v_period_us;
					}
					if( dx > 5 || 
						
						dy > 5 ) {

						pSysCfg->b_input_video_resolution_vga_period_us_counts = 0;

						pSysCfg->n_input_video_resolution_h_total = 0;

						MST3367_SetRegister( pDevice, 0x00, 0x04, 0x00 ); // PHASE
					}
					else if( pSysCfg->b_input_video_resolution_vga_period_us_counts < 10 ) {

						pSysCfg->b_input_video_resolution_vga_period_us_counts++;

						pSysCfg->b_input_video_resolution_vga_h_period_us_sum += n_signal_h_period_us;

						pSysCfg->b_input_video_resolution_vga_v_period_us_sum += n_signal_v_period_us;

						pSysCfg->b_input_video_resolution_vga_h_period_us = do_div(pSysCfg->b_input_video_resolution_vga_h_period_us_sum, pSysCfg->b_input_video_resolution_vga_period_us_counts);

						pSysCfg->b_input_video_resolution_vga_v_period_us = do_div(pSysCfg->b_input_video_resolution_vga_v_period_us_sum, pSysCfg->b_input_video_resolution_vga_period_us_counts);

					}
				}
#if 1
				//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] b_input_video_resolution_vga_period_us_counts( 0d%d )\n", pDevice->m_nKsDeviceNumber, pSysCfg->b_input_video_resolution_vga_period_us_counts );

				if( pSysCfg->b_input_video_resolution_vga_period_us_counts >= 10 ) {

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] n_input_video_resolution_h_total( 0d%d ) 11\n", pDevice->m_nKsDeviceNumber, pSysCfg->n_input_video_resolution_h_total );

					if( pSysCfg->n_input_video_resolution_h_total > 0 ) {

						x = pSysCfg->n_input_video_resolution_cx;

						y = pSysCfg->n_input_video_resolution_cy;

						fps = pSysCfg->n_input_video_resolution_fps;

						LINUXV4L2_DEBUG( KERN_INFO, "[%02d] x(0d%d) y(0d%d) fps(0d%d)\n", pDevice->m_nKsDeviceNumber, x, y, fps );
	
						goto EXIT_AUTO;
					}
					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] n_input_video_resolution_h_total( 0d%d ) 22\n", pDevice->m_nKsDeviceNumber, pSysCfg->n_input_video_resolution_h_total );

					{	ULONG hperiod_us = pSysCfg->b_input_video_resolution_vga_h_period_us;

						ULONG vperiod_us = pSysCfg->b_input_video_resolution_vga_v_period_us;
					
						ULONG wx = (hperiod_us > 0) ? (1600000 / hperiod_us) : (0); // H.TOTAL (H.FREQ)

						n_signal_h_total = (wx > 0) ? (ULONG)(((1650000 / wx) / 8) * 8) : (0);
						
						if(n_signal_h_total > 4000)
						{
							n_signal_h_total = 4000;
						}

						n_signal_v_total = (hperiod_us > 0) ? (ULONG)((vperiod_us * 16 * 80 + (hperiod_us / 2)) / hperiod_us) : (0);

						n_signal_v_period = (ULONG)(wx);
					}

					// -----> SetADParameters()
					//
					{	WORD dotclock;

						BYTE pllgain, clpdly, clpdur, hsopw, adc_bw0, adc_bw1;

						dotclock = (WORD)(((DWORD)n_signal_h_period * n_signal_h_total + 5000) / 10000);

						clpdly = 0x8;	

						if( dotclock < 30 ) {

							pllgain = 0x00 + 0x8;
							
							clpdur  = 0x08;
							
							hsopw   = 0x18;
							
							adc_bw0 = 0x44;
							
							adc_bw1 = 0x04;
						}
						else if( dotclock < 75 ) {

							pllgain = 0x40 + 0x18;
							
							clpdur = 0x20;
							
							hsopw = 0x30;
							
							adc_bw0 = 0x33;
							
							adc_bw1 = 0x03;
						}
						else if( dotclock < 100 ) {

							pllgain = 0x80 + 0x20;

							clpdur = 0x20;
							
							hsopw = 0x30;
							
							adc_bw0 = 0x11;
							
							adc_bw1 = 0x01;
						}
						else {

							pllgain = 0xC0 + 0x28;

							clpdur = 0x30;
							
							hsopw = 0x60;
							
							adc_bw0 = 0x00;
							
							adc_bw1 = 0x00;
						}
   						MST3367_SetRegister( pDevice, 0x00, 0x03, pllgain );

   						MST3367_SetRegister( pDevice, 0x00, 0x05, clpdly );

   						MST3367_SetRegister( pDevice, 0x00, 0x06, clpdur );

   						MST3367_SetRegister( pDevice, 0x00, 0x07, hsopw );

   						MST3367_SetRegister( pDevice, 0x00, 0x1E, adc_bw0 );

   						MST3367_SetRegister( pDevice, 0x00, 0x1F, adc_bw1 );

   						MST3367_SetRegister( pDevice, 0x00, 0xB5, 0x00 );
					}
					// -----> ADCSetAutoDetectFormat()
					//
					{	MST3367_SetRegister( pDevice, 0x00, 0x01, (BYTE)((n_signal_h_total - 1) >> 4) );

						MST3367_SetRegister( pDevice, 0x00, 0x02, (BYTE)((n_signal_h_total - 1) << 4) );

						MST3367_SetRegister( pDevice, 0x00, 0x0B, 0x80 ); // GRADE SCALE

						MST3367_SetRegister( pDevice, 0x00, 0x0C, 0x80 );

						MST3367_SetRegister( pDevice, 0x00, 0x0D, 0x80 );

						MST3367_SetRegister( pDevice, 0x00, 0x08, 0x80 );

						MST3367_SetRegister( pDevice, 0x00, 0x09, 0x80 );

						MST3367_SetRegister( pDevice, 0x00, 0x0A, 0x80 );
					}
					ULONG tempx = MST3367_GetRegister( pDevice, 0x00, 0x14 );

					ULONG hv_polarity = ((tempx & 0x20) >> 4) |
						
									    ((tempx & 0x04) >> 2);	  	

					hv_polarity |= (MST3367_GetRegister( pDevice, 0x00, 0x5F ) & 0x02) << 1;

	  				MST3367_SetRegister( pDevice, 0x00, 0xB2, (BYTE)((hv_polarity & 0x03) ^ 0x03) );

					MST3367_SetRegister( pDevice, 0x00, 0xE2, 0x80 ); // ENABLE AUTO POSITION

					MST3367_SetRegister( pDevice, 0x00, 0xE3, 0x00 ); // REPORT DIGITAL DE WINDOW

					{	ULONG i = 1;

						ULONG tempy = 0;

						while( TRUE ) {

	    					MST3367_SetRegister( pDevice, 0x00, 0xE4, (BYTE)(0x32 * i) );

							LONGLONG ts = n_signal_detect_delay;

							ts *= 10000;

							//DELAY_100NS( ts );
							wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( 1 ) );

							ULONG n_signal_test_h_start = ((MST3367_GetRegister( pDevice, 0x00, 0xF4 ) & 0x0F) << 8) | 

														  ((MST3367_GetRegister( pDevice, 0x00, 0xF3 ) & 0xFF) << 0);

							tempx = n_signal_test_h_start / 8;

							if( tempx != 0 ) {

								if( tempy >= tempx ) { break; }

								tempy = tempx;
							}
							i++;

							if( i > 5 ) { i = 5; break; }
						}
    					MST3367_SetRegister( pDevice, 0x00, 0xE4, (BYTE)(0x32 * i - 0x16) );

						LONGLONG ts = n_signal_detect_delay;

						ts *= 10000 * 2;

						//DELAY_100NS( ts );
						wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( 1 ) );

					}
					
					n_signal_h_start = ((MST3367_GetRegister( pDevice, 0x00, 0xF4 ) & 0x0F) << 8) | 

									   ((MST3367_GetRegister( pDevice, 0x00, 0xF3 ) & 0xFF) << 0);

					ULONG n_signal_h_end = ((MST3367_GetRegister( pDevice, 0x00, 0xF6 ) & 0x0F) << 8) | 

										   ((MST3367_GetRegister( pDevice, 0x00, 0xF5 ) & 0xFF) << 0);

					n_signal_v_start = ((MST3367_GetRegister( pDevice, 0x00, 0xF8 ) & 0x0F) << 8) | 

									   ((MST3367_GetRegister( pDevice, 0x00, 0xF7 ) & 0xFF) << 0);

					ULONG n_signal_v_end = ((MST3367_GetRegister( pDevice, 0x00, 0xFA ) & 0x0F) << 8) | 

										   ((MST3367_GetRegister( pDevice, 0x00, 0xF9 ) & 0xFF) << 0);

					n_signal_v_start = ((n_signal_v_start + 1));

					n_signal_v_end   = ((n_signal_v_end + 1));

					n_signal_h_start = ((n_signal_h_start + 4) / 8) * 8;

					n_signal_h_end   = ((n_signal_h_end + 4) / 8) * 8;

					ULONG    n_signal_h_width = ((n_signal_h_start + 12) / 24) * 8;

					ULONG n_calc_cx = n_signal_h_end - n_signal_h_start + 0;

					ULONG n_calc_cy = n_signal_v_end - n_signal_v_start + 1;

					ULONG    n_aspect_ratio = pDevice->m_nCustomAnalogVideoVgaAspectRatioProperty;

					ULONG temp = n_calc_cx;

					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_ADC_MODE_DETECT_AUTO( hs:%d x he:%d x vs:%d x ve:%d x hw:%d x ax:%d x ay:%d )", pDevice->m_nKsDeviceNumber, n_signal_h_start, n_signal_h_end, n_signal_v_start, n_signal_v_end, n_signal_h_width, (LONG)(n_calc_cx), (LONG)(n_calc_cy));

					if( n_aspect_ratio == 0 ) { // 4 : 3

						n_calc_cx = (((n_calc_cy *  4) / 3 + 4) / 8) * 8;
					}
					else if( n_aspect_ratio == 1 ) { // 5 : 4

						n_calc_cx = (((n_calc_cy * 5) / 4 + 4) / 8) * 8;
					}
					else if( n_aspect_ratio == 2 ) { // 16 : 9

						n_calc_cx = (((n_calc_cy * 16) / 9 + 4) / 8) * 8;
					}
					else if( n_aspect_ratio == 3 ) { // 16 : 10

						n_calc_cx = (((n_calc_cy * 16) / 10 + 4) / 8) * 8;
					}
					else if( n_aspect_ratio == 4 ) { // 3 : 2

						n_calc_cx = (((n_calc_cy * 3) / 2 + 4) / 8) * 8;
					}
					else if( n_aspect_ratio == 5 ) { // 1 : 1

						n_calc_cx = (((n_calc_cy * 1) / 1 + 4) / 8) * 8;
					}
					else {

						n_calc_cx = pDevice->m_nCustomAnalogVideoVgaHActiveProperty;

						if( n_calc_cx == 0 ) {

							n_calc_cx = (((n_calc_cy * 16) / 9 + 4) / 8) * 8;
						}
					}
				//	if( n_calc_cx > temp ) {
				//
				//		n_calc_cx = (((n_calc_cy * 5) / 4 + 4) / 8) * 8; // 5 : 4
				//	}
					if( n_calc_cx < 640 ) {

						n_calc_cx = 640;
					}
					if( temp != 0 ) {

	    				n_signal_h_start = (ULONG)((((n_signal_h_start * n_calc_cx) / temp + 4) / 8) * 8);

	    				n_signal_h_total = (ULONG)((((n_signal_h_total * n_calc_cx) / temp + 4) / 8) * 8);
					}
					LONGLONG n_calc_sz = n_calc_cx * n_calc_cy;

					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_ADC_MODE_DETECT_AUTO( ht:%d x vt:%d x hs:%d x vs:%d x aw:%d x ah:%d )", pDevice->m_nKsDeviceNumber, (ULONG)(n_signal_h_total), (ULONG)(n_signal_v_total), n_signal_h_start, n_signal_v_start, (LONG)(n_calc_cx), (LONG)(n_calc_cy));

					if( n_calc_sz <= 2304000 && // 1920 * 1200
									
						n_calc_cx > 1 &&
						
						n_calc_cy > 1 &&
						
					   (n_calc_cx & 0x07) == 0 ) { // 偵測成功

						x = (ULONG)(n_calc_cx);

						y = (ULONG)(n_calc_cy);

						fps = (fv + 5) / (10);

						pSysCfg->n_input_video_resolution_h_total = n_signal_h_total;

						if( (x * y * fps) > (80000000) ||
						
							(x * y) > (2073600) ) {

							pSysCfg->b_input_video_resolution_spliter_mode = TRUE;
						}
						else {

							pSysCfg->b_input_video_resolution_spliter_mode = FALSE;
						}
					}
					MST3367_SetRegister( pDevice, 0x00, 0xE2, 0x00 ); // DISABLE AUTO POSITION
				}
#endif
				;
EXIT_AUTO:
				;
			}
			else {

				pSysCfg->n_input_video_resolution_h_total = 0;

				pSysCfg->b_input_video_resolution_spliter_mode = FALSE;

				pSysCfg->b_input_video_resolution_vga_period_us_counts = 0;
			}
			if( x != 0 && y != 0 ) {

				if( pSysCfg->n_input_video_resolution_cx != x ||

					pSysCfg->n_input_video_resolution_cy != y ||
					
					pSysCfg->n_input_video_resolution_fps != fps ) {
					
					pSysCfg->n_input_video_resolution_cx = x;

					pSysCfg->n_input_video_resolution_cy = y;

					pSysCfg->n_input_video_resolution_fps = fps;

					pSysCfg->n_input_video_resolution_fps_m = MST3367_GET_FREQV( pDevice );

					if( (pSysCfg->n_input_video_resolution_cx ==  720 && pSysCfg->n_input_video_resolution_cy == 240) ||

						(pSysCfg->n_input_video_resolution_cx ==  720 && pSysCfg->n_input_video_resolution_cy == 288) ||

						(pSysCfg->n_input_video_resolution_cx ==  768 && pSysCfg->n_input_video_resolution_cy == 288) ||

						(pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy == 540) ||

						(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 540) ) {

						pSysCfg->n_input_video_resolution_interleaved = 1;
					}
					else {

						pSysCfg->n_input_video_resolution_interleaved = 0;
					}
					pSysCfg->n_input_audio_sampling_frequency = 48000;

					b_is_signal_changed = TRUE;
				}
				b_is_signal_vaild = TRUE;
			}
		}
		else {
					
			pSysCfg->n_input_video_resolution_cx = 0;

			pSysCfg->n_input_video_resolution_cy = 0;

			pSysCfg->n_input_video_resolution_fps = 0;

			pSysCfg->n_input_video_resolution_fps_m = 0;

			pSysCfg->n_input_video_resolution_interleaved = 0;

			pSysCfg->n_input_video_resolution_h_total = 0;

			pSysCfg->b_input_video_resolution_spliter_mode = FALSE;

			pSysCfg->b_input_video_resolution_vga_period_us_counts = 0;
		}
		if( b_is_signal_vaild ) {

			BYTE R005F = MST3367_GetRegister( pDevice, 0x00, 0x5F );

			pDevice->m_nCustomAnalogVideoResolutionProperty = (pSysCfg->n_input_video_resolution_cx << 16) |

															  (pSysCfg->n_input_video_resolution_cy <<  0);

			if( pSysCfg->n_input_video_resolution_fps == 59 ||

				pSysCfg->n_input_video_resolution_fps == 61 ) {

				pDevice->m_nCustomAnalogVideoFrameRateProperty = 60;
			}
			else if( pSysCfg->n_input_video_resolution_fps == 76 ) {

				pDevice->m_nCustomAnalogVideoFrameRateProperty = 75;
			}
			else {

				pDevice->m_nCustomAnalogVideoFrameRateProperty = pSysCfg->n_input_video_resolution_fps;
			}
			pDevice->m_nCustomAnalogVideoInterleavedProperty = pSysCfg->n_input_video_resolution_interleaved;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 48000;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 1;
#ifdef MEDICAL
			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
#else
			pDevice->m_nAnalogCopyProtMacrovisionProperty = (R005F & 0x10) ? 1 : 0;
#endif
		}
		else {


			pSysCfg->n_input_video_resolution_cx = 0;
			pSysCfg->n_input_video_resolution_cy = 0;
			pSysCfg->n_input_video_resolution_fps = 0;
			pDevice->m_nCustomAnalogVideoResolutionProperty = 0;
		
			pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

			pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
		}
		if( b_is_signal_changed ) {

			// -----> RxHandleVideoIn()
			//
			{	BYTE R00AB = MST3367_GetRegister( pDevice, 0x00, 0xAB ); R00AB &= 0x7F;

				MST3367_SetRegister( pDevice, 0x00, 0xAB, R00AB | 0x80 ); // BLANK.OUTPUT

				// [V] : VESA, [E] : EIA, [D] : DANIEL, [NV] : NVIDIA
				// 
				static ULONG  RX_ADC_SET_VALUES_EIA[ DEFAULT_MST3367_VESA_SIZE ][ 13 ] = { 
					
					{ 0x28, 0x08, 0x08, 0x30, 0x33, 0x03,  800,  525 / 1, 144 -      1, 35,  640,  480, 60 }, // [01] [E] [850.O] [850.O] [ ]  640 ×  480 @60
																	{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  858,  525 / 2, 119 -      1, 19, 1440,  240, 60 }, // [02] [E] [878.O] [878.O] [ ] 1440 ×  240 @60
																	{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  864,  625 / 2, 132 -      1, 23, 1440,  288, 50 }, // [03] [E] [904.O] [904.O] [ ] 1440 ×  288 @50
																	{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  858,  525 / 2, 119 -      1, 18,  720,  240, 60 }, // [04] [E] [872.T] [872.T] [ ]  720 ×  240 @60
																	{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  864,  625 / 2, 132 -      1, 22,  720,  288, 50 }, // [05] [E] [902.T] [902.T] [ ]  720 ×  288 @50
																	{ 0x20, 0x08, 0x08, 0x18, 0x33, 0x03,  858,  525 / 1, 122 -      1, 36,  720,  480, 60 }, // [06] [E] [853.O] [853.O] [ ]  720 ×  480 @60
																	{ 0x18, 0x08, 0x08, 0x18, 0x33, 0x03,  864,  625 / 1, 132 -      1, 44,  720,  576, 50 }, // [07] [E] [949.O] [949.O] [ ]  720 ×  576 @50
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 3300,  750 / 1, 260 + 40 - 1, 25, 1280,  720, 30 }, // [08] [E] [942.O] [942.O] [ ] 1280 ×  720 @30
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 3960,  750 / 1, 260 + 40 - 1, 25, 1280,  720, 25 }, // [09] [E] [942.O] [942.O] [ ] 1280 ×  720 @25
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 3300,  750 / 1, 260 + 40 - 1, 25, 1280,  720, 24 }, // [10] [E] [942.O] [942.O] [ ] 1280 ×  720 @24
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1650,  750 / 1, 260 + 40 - 1, 25, 1280,  720, 60 }, // [11] [E] [940.O] [940.O] [ ] 1280 ×  720 @60
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1980,  750 / 1, 260 + 40 - 1, 25, 1280,  720, 50 }, // [12] [E] [942.O] [942.O] [ ] 1280 ×  720 @50
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 2200, 1125 / 2, 192 + 44 - 1, 20, 1920,  540, 60 }, // [13] [E] [938.T] [938.T] [ ] 1920 ×  540 @60
																	{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 2640, 1125 / 2, 192 + 44 - 1, 20, 1920,  540, 50 }, // [14] [E] [939.T] [939.T] [ ] 1920 ×  540 @50
																	{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2200, 1125 / 1, 192 + 44 - 1, 41, 1920, 1080, 30 }, // [15] [E] [-----] [-----] [ ] 1920 × 1080 @30
																	{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2640, 1125 / 1, 192 + 44 - 1, 41, 1920, 1080, 25 }, // [16] [E] [-----] [-----] [ ] 1920 × 1080 @25
																	{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2750, 1125 / 1, 192 + 44 - 1, 41, 1920, 1080, 24 }, // [17] [E] [-----] [-----] [ ] 1920 × 1080 @24
																	{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2200, 1125 / 1, 192 + 44 - 1, 41, 1920, 1080, 60 }, // [18] [E] [935.O] [XXXXX] [ ] 1920 × 1080 @60
																	{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2640, 1125 / 1, 192 + 44 - 1, 41, 1920, 1080, 50 }, // [19] [E] [937.O] [XXXXX] [ ] 1920 × 1080 @50
				};
				static ULONG RX_ADC_SET_VALUES_VESA[ DEFAULT_MST3367_VESA_SIZE ][ 13 ] = { 
					
				//	{ 0x30, 0x08, 0x08, 0x30, 0x33, 0x03,  848,  440 / 1, 140    , 26    ,  640,  384, 60 }, // [01]  496 ×  384 @60 [      ]
					{ 0x30, 0x08, 0x08, 0x30, 0x33, 0x03,  848,  440 / 1, 140    , 26    ,  640,  384, 60 }, // [01]  640 ×  384 @60 [      ]
					{ 0x30, 0x08, 0x08, 0x30, 0x33, 0x03,  848,  440 / 1, 147 + 3, 40 - 6,  640,  400, 60 }, // [02]  640 ×  400 @60 [      ] // DEMPA OFFSET FOR NEC
					{ 0x28, 0x08, 0x08, 0x30, 0x33, 0x03,  800,  525 / 1, 144 + 0, 35 + 0,  640,  480, 60 }, // [03]  640 ×  480 @60 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x33, 0x03, 1056,  500 / 1, 186 + 0, 17 + 0,  848,  480, 60 }, // [--]  848 ×  480 @60 [      ]
					{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  858,  525 / 2, 119    , 19    , 1440,  240, 60 }, // [04] 1440 ×  240 @60 [      ]
					{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  864,  625 / 2, 132    , 23    , 1440,  288, 50 }, // [05] 1440 ×  288 @50 [      ]
					{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  858,  525 / 2, 119    , 18    ,  720,  240, 60 }, // [06]  720 ×  240 @60 [      ]
					{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  864,  625 / 2, 132    , 22    ,  720,  288, 50 }, // [07]  720 ×  288 @50 [      ]
					{ 0x20, 0x08, 0x08, 0x18, 0x33, 0x03,  858,  525 / 1, 122    , 36    ,  720,  480, 60 }, // [08]  720 ×  480 @60 [      ]
					{ 0x18, 0x08, 0x08, 0x18, 0x33, 0x03,  864,  625 / 1, 132    , 44    ,  720,  576, 50 }, // [09]  720 ×  576 @50 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x22, 0x02, 1056,  628 / 1, 216 + 0, 27 + 0,  800,  600, 60 }, // [10]  800 ×  600 @60 [      ]
					{ 0x60, 0x08, 0x08, 0x30, 0x22, 0x02, 1056,  625 / 1, 240 + 0, 24 + 0,  800,  600, 75 }, // [11]  800 ×  600 @75 [      ]
					{ 0x60, 0x08, 0x08, 0x30, 0x22, 0x02, 1048,  631 / 1, 216 + 0, 30 + 0,  800,  600, 85 }, // [12]  800 ×  600 @85 [      ]
					{ 0x70, 0x08, 0x20, 0x60, 0x11, 0x01, 1344,  806 / 1, 296 + 0, 35 + 0, 1024,  768, 60 }, // [13] 1024 ×  768 @60 [      ]
					{ 0x60, 0x08, 0x20, 0x60, 0x11, 0x01, 1312,  800 / 1, 272 + 0, 31 + 0, 1024,  768, 75 }, // [14] 1024 ×  768 @75 [      ]
					{ 0xA0, 0x08, 0x20, 0x60, 0x11, 0x01, 1376,  808 / 1, 304 + 0, 39 + 0, 1024,  768, 85 }, // [15] 1024 ×  768 @85 [      ]
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1712,  759 / 1, 338 + 0, 32 + 0, 1280,  720, 85 }, // [--] 1280 ×  720 @85 [      ]
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1696,  755 / 1, 338 + 0, 32 + 0, 1280,  720, 75 }, // [--] 1280 ×  720 @75 [      ]
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1668,  750 / 1, 330    , 25    , 1280,  720, 61 }, // [16] 1280 ×  720 @60 [      ] // VESA 720P
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1650,  750 / 1, 260 + 0, 25 + 0, 1280,  720, 60 }, // [17] 1280 ×  720 @60 [      ]
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 1980,  750 / 1, 260    , 25    , 1280,  720, 50 }, // [18] 1280 ×  720 @50 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1664,  798 / 1, 320 + 0, 27 + 0, 1280,  768, 60 }, // [19] 1280 ×  768 @60 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1696,  805 / 1, 335    , 34    , 1280,  768, 75 }, // [20] 1280 ×  768 @75 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1712,  809 / 1, 351    , 38    , 1280,  768, 85 }, // [21] 1280 ×  768 @85 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1680,  831 / 1, 328 + 1, 28 + 0, 1280,  800, 60 }, // [22] 1280 ×  800 @60 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1696,  838 / 1, 336    , 35    , 1280,  800, 75 }, // [23] 1280 ×  800 @75 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1712,  843 / 1, 352    , 40    , 1280,  800, 85 }, // [24] 1280 ×  800 @85 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1712,  994 / 1, 354 + 0, 33 + 0, 1280,  960, 60 }, // [25] 1280 ×  960 @60 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1728, 1002 / 1, 358    , 41    , 1280,  960, 75 }, // [26] 1280 ×  960 @75 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1728, 1011 / 1, 383    , 50    , 1280,  960, 85 }, // [27] 1280 ×  960 @85 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1688, 1066 / 1, 360 + 0, 41 + 0, 1280, 1024, 60 }, // [28] 1280 × 1024 @60 [      ]
					{ 0xE8, 0x08, 0x20, 0x60, 0x00, 0x00, 1688, 1066 / 1, 392    , 41    , 1280, 1024, 75 }, // [29] 1280 × 1024 @75 [      ]
					{ 0xE8, 0x08, 0x20, 0x60, 0x00, 0x00, 1728, 1072 / 1, 383    , 47    , 1280, 1024, 85 }, // [30] 1280 × 1024 @85 [      ]
					{ 0xE8, 0x08, 0x20, 0x60, 0x00, 0x00, 1792,  795 / 1, 368 + 0, 24 + 0, 1360,  768, 60 }, // [31] 1360 ×  768 @60 [      ]
					{ 0xE8, 0x08, 0x20, 0x60, 0x00, 0x00, 1808,  802 / 1, 368    , 24    , 1360,  768, 75 }, // [32] 1360 ×  768 @75 [      ]
					{ 0xE8, 0x08, 0x20, 0x60, 0x00, 0x00, 1824,  807 / 1, 368    , 24    , 1360,  768, 85 }, // [33] 1360 ×  768 @85 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1904,  934 / 1, 384    , 31    , 1440,  900, 60 }, // [34] 1440 ×  900 @60 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1936,  942 / 1, 400    , 39    , 1440,  900, 75 }, // [35] 1440 ×  900 @75 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1952,  948 / 1, 407    , 45    , 1440,  900, 85 }, // [36] 1440 ×  900 @85 [      ] 
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 2200, 1125 / 2, 192 + 0, 20 + 0, 1920,  540, 60 }, // [37] 1920 ×  540 @60 [      ]
					{ 0x58, 0x38, 0x20, 0x30, 0x11, 0x01, 2640, 1125 / 2, 192 + 0, 20 + 0, 1920,  540, 50 }, // [38] 1920 ×  540 @50 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2200, 1125 / 1, 192 + 0, 41 + 0, 1920, 1080, 30 }, // [39] 1920 × 1080 @30 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2640, 1125 / 1, 192 + 0, 41 + 0, 1920, 1080, 25 }, // [40] 1920 × 1080 @25 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2750, 1125 / 1, 192    , 41    , 1920, 1080, 24 }, // [41] 1920 × 1080 @24 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2580, 1125 / 1, 538    , 37    , 1920, 1080, 61 }, // [42] 1920 × 1080 @60 [      ] // VESA 1080P
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2200, 1125 / 1, 192    , 41    , 1920, 1080, 60 }, // [43] 1920 × 1080 @60 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2640, 1125 / 1, 192    , 41    , 1920, 1080, 50 }, // [44] 1920 × 1080 @50 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 4000, 1043 / 1, 113    , 16    , 3840, 1024, 30 }, // [45] 3840 × 1024 @30 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x33, 0x03,  832,  520 / 1, 168 + 0, 31 + 0,  640,  480, 72 }, // [46]  640 ×  480 @72 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x33, 0x03,  840,  500 / 1, 184 + 0, 19 + 0,  640,  480, 75 }, // [47]  640 ×  480 @75 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x33, 0x03,  832,  509 / 1, 136 + 0, 28 + 0,  640,  480, 85 }, // [48]  640 ×  480 @85 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x22, 0x02, 1024,  625 / 1, 200 + 0, 24 + 0,  800,  600, 56 }, // [49]  800 ×  600 @56 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x22, 0x02, 1040,  666 / 1, 184 + 0, 29 + 0,  800,  600, 72 }, // [50]  800 ×  600 @72 [      ]
					{ 0x60, 0x08, 0x20, 0x60, 0x11, 0x01, 1328,  806 / 1, 280 + 0, 35 + 0, 1024,  768, 70 }, // [51] 1024 ×  768 @70 [      ]
					{ 0x60, 0x08, 0x20, 0x60, 0x11, 0x01, 1312,  793 / 1, 248    , 22    , 1024,  768, 50 }, // [52] 1024 ×  768 @50 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1680, 1057 / 1, 328    , 30    , 1280, 1024, 50 }, // [53] 1280 × 1024 @50 [      ]
					{ 0x20, 0x08, 0x08, 0x18, 0x33, 0x03,  936,  446 / 1, 180    , 45    ,  720,  400, 85 }, // [54]  720 ×  400 @85 [      ]
					{ 0x20, 0x08, 0x08, 0x18, 0x33, 0x03, 1120,  654 / 1, 256    , 27    ,  832,  624, 75 }, // [55]  832 ×  624 @75 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1600,  900 / 1, 384    , 35    , 1152,  864, 75 }, // [56] 1152 ×  864 @75 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1864, 1089 / 1, 376 - 1, 36 + 0, 1400, 1050, 60 }, // [57] 1400 × 1050 @60 [      ]
					{ 0x70, 0x08, 0x20, 0x60, 0x11, 0x01, 1352,  624 / 1, 248    , 21    , 1064,  600, 60 }, // [58] 1064 ×  600 @60 [      ]
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1560, 1080 / 1, 112 + 0, 27 + 0, 1400, 1050, 61 }, // [59] 1400 × 1050 @61 [      ]
					{ 0x70, 0x08, 0x20, 0x60, 0x11, 0x01, 1312,  624 / 1, 250    , 21    , 1024,  600, 60 }, // [60] 1024 ×  600 @60 [      ]
					{ 0xE8, 0x08, 0x20, 0x60, 0x00, 0x00, 1776,  798 / 1, 346    , 27    , 1360,  768, 61 }, // [61] 1360 ×  768 @61 [      ] // SEGA CVT.1360
					{ 0xB0, 0x08, 0x20, 0x60, 0x11, 0x01, 1904, 1115 / 2, 384 + 0, 32    , 1440,  540, 50 }, // [62] 1440 ×  540 @50 [      ]
					{ 0xE8, 0x08, 0x30, 0xA0, 0x00, 0x00, 2160, 1250 / 1, 496 + 0, 49    , 1600, 1200, 60 }, // [63] 1600 × 1200 @60 [CVT.OK]
					{ 0xE8, 0x08, 0x30, 0xA0, 0x00, 0x00, 2128, 1238 / 1, 432 + 0, 35    , 1600, 1200, 50 }, // [64] 1600 × 1200 @50 [CVT.OK]
					{ 0xE8, 0x08, 0x30, 0xA0, 0x00, 0x00, 2592, 1245 / 1, 536 + 0, 42    , 1920, 1200, 60 }, // [65] 1920 × 1200 @60 [CVT.OK]
					{ 0xE8, 0x08, 0x30, 0xA0, 0x00, 0x00, 2560, 1238 / 1, 520 + 0, 35    , 1920, 1200, 50 }, // [66] 1920 × 1200 @50 [CVT.OK]
					{ 0x08, 0x08, 0x08, 0x18, 0x44, 0x04,  922,  625 / 2, 141    , 22    ,  768,  288, 50 }, // [67]  768 ×  288 @50 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x22, 0x02, 1064,  666 / 1, 224 + 0, 28 + 0,  800,  600, 55 }, // [68]  800 ×  600 @55 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x22, 0x02, 1104,  568 / 1, 264 + 0, 42 + 0,  768,  512, 55 }, // [69]  768 ×  512 @55 [      ]
					{ 0x28, 0x08, 0x08, 0x30, 0x22, 0x02, 1024,  525 / 1, 192 + 0, 22 + 0,  800,  500, 75 }, // [70]  800 ×  500 @75 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00, 2160, 1250 / 1, 496 + 0, 49    , 1600, 1200, 75 }, // [71] 1600 × 1200 @75 [      ]
					{ 0xE8, 0x38, 0x30, 0x60, 0x00, 0x00,    0,        0,       0,      0,    0,    0,  0 }, // [72] ---- ×  --- @--
				};

				// H.TOTAL = PERIOD (PIXELS)
				//
				// V.TOTAL = PERIOD (LINES)
				//
				// H.START = BLANK - PULSE.DELAY (PIXELS)
				//
				// V.START = BLANK - PULSE.DELAY (LINES)
				//
				// NVDIA RULES: DMT -> CVT -> Total Pixels (兩個值) -> Total Pixels - Active Pixels - Front Porch (兩個值)

				ULONG j = (pDevice->m_nAnalogCrossbarVideoInputProperty == 3) ? (DEFAULT_MST3367_VESA_SIZE - 1) : (DEFAULT_MST3367_EIA_SIZE - 1);

				if( pSysCfg->n_input_video_resolution_h_total > 0 ) {

					ULONG * p = NULL;

					if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

						p = &(RX_ADC_SET_VALUES_VESA[ j ][ 0 ]);

					}
					else { // COMPONENT

						p = &(RX_ADC_SET_VALUES_EIA[ j ][ 0 ]);
					}
					p[ 0 ] = 0xE8; 
					
					p[ 1 ] = 0x08; 
						
					p[ 2 ] = 0x30; 
						
					p[ 3 ] = 0x60; 
					
					p[ 4 ] = 0x00;
						
					p[ 5 ] = 0x00;
						
					p[ 6 ] = n_signal_h_total;
						
					p[ 7 ] = n_signal_v_total;
						
					p[ 8 ] = n_signal_h_start;
						
					p[ 9 ] = n_signal_v_start;
				}
				else {

					j--;

					for( ; j > 0 ; j-- ) {
	
						if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG
	
							if( RX_ADC_SET_VALUES_VESA[ j ][ 10 ] == pSysCfg->n_input_video_resolution_cx &&
								
								RX_ADC_SET_VALUES_VESA[ j ][ 11 ] == pSysCfg->n_input_video_resolution_cy &&
								
								RX_ADC_SET_VALUES_VESA[ j ][ 12 ] == pSysCfg->n_input_video_resolution_fps ) {

								LINUXV4L2_PRINT( KERN_INFO, "[%02d] DVI.ANALOG j(%d)\n", (int)(pDevice->m_nKsDeviceNumber), j );
	
								break;
							}
						}
						else { // COMPONENT
	
							if( RX_ADC_SET_VALUES_EIA[ j ][ 10 ] == pSysCfg->n_input_video_resolution_cx &&
								
								RX_ADC_SET_VALUES_EIA[ j ][ 11 ] == pSysCfg->n_input_video_resolution_cy &&
								
								RX_ADC_SET_VALUES_EIA[ j ][ 12 ] == pSysCfg->n_input_video_resolution_fps ) {
	
								break;
							}
						}
					}
				}
				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

					MST3367_SetRegister( pDevice, 0x00, 0x03, (BYTE)(RX_ADC_SET_VALUES_VESA[ j ][ 0 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x05, (BYTE)(RX_ADC_SET_VALUES_VESA[ j ][ 1 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x06, (BYTE)(RX_ADC_SET_VALUES_VESA[ j ][ 2 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x07, (BYTE)(RX_ADC_SET_VALUES_VESA[ j ][ 3 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x1E, (BYTE)(RX_ADC_SET_VALUES_VESA[ j ][ 4 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x1F, (BYTE)(RX_ADC_SET_VALUES_VESA[ j ][ 5 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x01, (BYTE)((RX_ADC_SET_VALUES_VESA[ j ][ 6 ] - 1) >> 4) );

					MST3367_SetRegister( pDevice, 0x00, 0x02, (BYTE)((RX_ADC_SET_VALUES_VESA[ j ][ 6 ] - 1) << 4) );

					MST3367_SetRegister( pDevice, 0x00, 0x0B, 0x7A ); // GRADE SCALE

					MST3367_SetRegister( pDevice, 0x00, 0x0C, 0x7A );

					MST3367_SetRegister( pDevice, 0x00, 0x0D, 0x7A );

					MST3367_SetRegister( pDevice, 0x00, 0x08, 0x9E );

					MST3367_SetRegister( pDevice, 0x00, 0x09, 0x9E ); 

					MST3367_SetRegister( pDevice, 0x00, 0x0A, 0x9E ); 

				//	MST3367_SetRegister( pDevice, 0x00, 0x1E, 0x44 ); 
				//
				//	MST3367_SetRegister( pDevice, 0x00, 0x1F, 0x04 ); 

					BYTE R0010 = MST3367_GetRegister( pDevice, 0x00, 0x10 ); R0010 &= ~0x05; R0010 |= 0x00;

					MST3367_SetRegister( pDevice, 0x00, 0x10, R0010 );

					MST3367_SetRegister( pDevice, 0x00, 0x18, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x19, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x1A, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x39, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x2C, 0xCC );
					
					/////////////////////////////////////////////////////////////////////////

					BYTE R0060 = MST3367_GetRegister( pDevice, 0x00, 0x60 );

					MST3367_SetRegister( pDevice, 0x00, 0x60, R0060 & ~0x04 );
				}
				else { // COMPONENT

					MST3367_SetRegister( pDevice, 0x00, 0x03, (BYTE)(RX_ADC_SET_VALUES_EIA[ j ][ 0 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x05, (BYTE)(RX_ADC_SET_VALUES_EIA[ j ][ 1 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x06, (BYTE)(RX_ADC_SET_VALUES_EIA[ j ][ 2 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x07, (BYTE)(RX_ADC_SET_VALUES_EIA[ j ][ 3 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x1E, (BYTE)(RX_ADC_SET_VALUES_EIA[ j ][ 4 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x1F, (BYTE)(RX_ADC_SET_VALUES_EIA[ j ][ 5 ]) );

					MST3367_SetRegister( pDevice, 0x00, 0x01, (BYTE)((RX_ADC_SET_VALUES_EIA[ j ][ 6 ] - 1) >> 4) );

					MST3367_SetRegister( pDevice, 0x00, 0x02, (BYTE)((RX_ADC_SET_VALUES_EIA[ j ][ 6 ] - 1) << 4) );

					BYTE R0010 = MST3367_GetRegister( pDevice, 0x00, 0x10 ); R0010 &= ~0x05; R0010 |= 0x05;

					MST3367_SetRegister( pDevice, 0x00, 0x10, R0010 );

					BYTE R000F = MST3367_GetRegister( pDevice, 0x00, 0x0F ); R000F &= ~0x20; R000F |= 0x20;

					MST3367_SetRegister( pDevice, 0x00, 0x0F, R000F );

					MST3367_SetRegister( pDevice, 0x00, 0x12, 0x04 );

					BYTE R0017 = MST3367_GetRegister( pDevice, 0x00, 0x17 ); R0017 &= ~0x02; R0017 |= 0x02;

					MST3367_SetRegister( pDevice, 0x00, 0x17, R0017 );

					MST3367_SetRegister( pDevice, 0x00, 0x0B, 0x80 ); // GRADE SCALE

					MST3367_SetRegister( pDevice, 0x00, 0x0C, 0x70 );

					MST3367_SetRegister( pDevice, 0x00, 0x0D, 0x80 );

					MST3367_SetRegister( pDevice, 0x00, 0x08, 0xC0 );

					MST3367_SetRegister( pDevice, 0x00, 0x09, 0xC0 ); 

					MST3367_SetRegister( pDevice, 0x00, 0x0A, 0xC0 ); 

				//	MST3367_SetRegister( pDevice, 0x00, 0x1E, 0x54 ); 
				//  
				//	MST3367_SetRegister( pDevice, 0x00, 0x1F, 0x05 ); 

					MST3367_SetRegister( pDevice, 0x00, 0x1B, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x1C, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x1D, 0x00 );

					MST3367_SetRegister( pDevice, 0x00, 0x18, 0x10 );

					MST3367_SetRegister( pDevice, 0x00, 0x19, 0x10 );

					MST3367_SetRegister( pDevice, 0x00, 0x1A, 0x10 );

					MST3367_SetRegister( pDevice, 0x00, 0x2D, 0x11 );

					MST3367_SetRegister( pDevice, 0x00, 0x2E, 0x11 );

					BYTE R002F = MST3367_GetRegister( pDevice, 0x00, 0x2F ); R002F &= ~0x02; R002F |= 0x02;

					MST3367_SetRegister( pDevice, 0x00, 0x2F, R002F );

					MST3367_SetRegister( pDevice, 0x00, 0x3A, 0x0C );

					MST3367_SetRegister( pDevice, 0x00, 0x3B, 0x08 );

					BYTE R0005 = MST3367_GetRegister( pDevice, 0x00, 0x05 );

					BYTE R0006 = MST3367_GetRegister( pDevice, 0x00, 0x06 );

					MST3367_SetRegister( pDevice, 0x00, 0x39, 0x88 + R0005 + R0006 );

					MST3367_SetRegister( pDevice, 0x00, 0x2C, 0x9D );

					/////////////////////////////////////////////////////////////////////////

					BYTE R0060 = MST3367_GetRegister( pDevice, 0x00, 0x60 );

					MST3367_SetRegister( pDevice, 0x00, 0x60, R0060 & ~0x04 );
				}
				 if( (pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy == 240 && pSysCfg->n_input_video_resolution_fps == 60) ||

						 (pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy == 288 && pSysCfg->n_input_video_resolution_fps == 50) ) {

					if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

						MST3367_SetRegister( pDevice, 0x00, 0xB1, 0xC1 ); // 支援 DEMPA RGB CONVERTER BOX (將錯就錯)
					}
					else { // COMPONENT

						MST3367_SetRegister( pDevice, 0x00, 0xB1, 0xC1 );
					}
				}
				else {
			
					MST3367_SetRegister( pDevice, 0x00, 0xB1, 0xC0 );
				}
				/////////////////////////////////////////////////////////////////////////

				BYTE R0014 = MST3367_GetRegister( pDevice, 0x00, 0x14 );

				BYTE R0080 = MST3367_GetRegister( pDevice, 0x00, 0x80 ); R0080 &= 0xF0;

				ULONG cx_start = (pDevice->m_nAnalogCrossbarVideoInputProperty == 3) ? (RX_ADC_SET_VALUES_VESA[ j ][ 8 ] - 1) : RX_ADC_SET_VALUES_EIA[ j ][ 8 ];

				ULONG cy_start = (pDevice->m_nAnalogCrossbarVideoInputProperty == 3) ? (RX_ADC_SET_VALUES_VESA[ j ][ 9 ] - 0) : RX_ADC_SET_VALUES_EIA[ j ][ 9 ];

				LONG  cx_offset = 0;

				LONG  cy_offset = 0;

				#if 1

				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) {

					cx_offset = pSysCfg->n_input_video_resolution_offset_x_start_vesa[ j ];

					cy_offset = pSysCfg->n_input_video_resolution_offset_y_start_vesa[ j ];
				}
				else {

					cx_offset = pSysCfg->n_input_video_resolution_offset_x_start_eia[ j ];

					cy_offset = pSysCfg->n_input_video_resolution_offset_y_start_eia[ j ];
				}
				pSysCfg->n_input_video_resolution_offset_x_start_fetch = cx_start;

				pSysCfg->n_input_video_resolution_offset_y_start_fetch = cy_start;

				pSysCfg->n_input_video_resolution_offset_index = j;

				#endif

				cx_start += cx_offset;

				cy_start += cy_offset;

				LINUXV4L2_DEBUG( KERN_INFO, "MST3367_ADC_GET_SYNC( %s x %02X )\n", (R0014 & 0x40) ? "SOG" : "HSYNC", R0014);

				MST3367_SetRegister( pDevice, 0x00, 0x80, R0080 | (BYTE)(cx_start >> 8) );

				MST3367_SetRegister( pDevice, 0x00, 0x81, (BYTE)(cx_start) );

				MST3367_SetRegister( pDevice, 0x00, 0x82, (BYTE)(pSysCfg->n_input_video_resolution_cx >> 8) );

				MST3367_SetRegister( pDevice, 0x00, 0x83, (BYTE)(pSysCfg->n_input_video_resolution_cx >> 0) );

				MST3367_SetRegister( pDevice, 0x00, 0x84, (BYTE)(cy_start) );

				MST3367_SetRegister( pDevice, 0x00, 0x85, (BYTE)(pSysCfg->n_input_video_resolution_cy >> 8) );

				if( pSysCfg->n_input_video_resolution_interleaved ) {

					BYTE R0085 = (BYTE)(pSysCfg->n_input_video_resolution_cy >> 8);

					MST3367_SetRegister( pDevice, 0x00, 0x85, R0085 |  0x20 );

					BYTE R0060 = MST3367_GetRegister( pDevice, 0x00, 0x60 );

					MST3367_SetRegister( pDevice, 0x00, 0x60, R0060 & ~0x04 );
				}
				if( pDevice->iProduct == 0xE5 ||
				 
				   (pDevice->idVendor == 0x05CA && pDevice->idProduct == 0x0901) ||
				
				   (pDevice->idProduct & 0xCFF0) == 0xC530 ) {

					if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) {

						if( (pSysCfg->n_input_video_resolution_cx == 720 && pSysCfg->n_input_video_resolution_cy == 240 && pSysCfg->n_input_video_resolution_fps == 60) ||

							(pSysCfg->n_input_video_resolution_cx == 720 && pSysCfg->n_input_video_resolution_cy == 288 && pSysCfg->n_input_video_resolution_fps == 50) ||

							(pSysCfg->n_input_video_resolution_cx == 768 && pSysCfg->n_input_video_resolution_cy == 288 && pSysCfg->n_input_video_resolution_fps == 50) ) { // 支援 DEMPA RGB CONVERTER BOX (將錯就錯)

							BYTE R0060 = MST3367_GetRegister( pDevice, 0x00, 0x60 );

							MST3367_SetRegister( pDevice, 0x00, 0x60, R0060 | 0x04 );
						}
					}
				}
				MST3367_SetRegister( pDevice, 0x00, 0x86, (BYTE)(pSysCfg->n_input_video_resolution_cy >> 0) );
				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ) { // COMPONENT

					if( (pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 720 && pSysCfg->n_input_video_resolution_fps == 30) || 
						
						(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 720 && pSysCfg->n_input_video_resolution_fps == 25) ||
						
						(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 720 && pSysCfg->n_input_video_resolution_fps == 24) ) {

						MST3367_SetRegister( pDevice, 0x00, 0x12, 0x00 );
					}
					else {

						MST3367_SetRegister( pDevice, 0x00, 0x12, 0x04 );
					}
				}
				MST3367_SetRegister( pDevice, 0x00, 0xAB, R00AB ); // NORMAL.OUTPUT
			}
			// -----> RxHandleVideoOut()
			//
			{	MST3367_SETUP_CSC_TABLE( pDevice );

				MST3367_ADC_AUTO_PHASE( pDevice );

//				MST3367_ADJUST_CSC_TABLE( pDevice, (BYTE)(pDevice->m_nAnalogVideoProcAmpBrightnessProperty[ 0 ] & 0xFF), 
					
//												   (BYTE)(pDevice->m_nAnalogVideoProcAmpContrastProperty[ 0 ] & 0xFF), 
												   
//												   (BYTE)(pDevice->m_nAnalogVideoProcAmpSaturationProperty[ 0 ] & 0xFF), 
												   
//												   (BYTE)(pDevice->m_nAnalogVideoProcAmpHueProperty[ 0 ] & 0xFF), 
												   
//												  ((BYTE)(pDevice->m_nAnalogVideoProcAmpSharpnessProperty[ 0 ] & 0xFF) >> 5) ); // 先簡單解, 正確應該要追究哪個 REG 解了SEGA 跳動問題

				MST3367_ADJUST_CSC_TABLE( pDevice, (BYTE)(g_n_analog_decoder_brightness[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF), 
					
												   (BYTE)(g_n_analog_decoder_contrast[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF), 
												   
												   (BYTE)(g_n_analog_decoder_saturation[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF), 
												   
												   (BYTE)(g_n_analog_decoder_hue[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF),

												   (BYTE)(g_n_analog_decoder_sharpness[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF));

			}
			pSysCfg->b_input_video_signal_changed = TRUE;

			#ifdef ENABLE_1920X1080PX60FPS // +SC500

			// FPGA
			// 
			if( (pDevice->iManufacturer == 0x01 && pDevice->iProduct == 0x07) ||
				
				(pDevice->iManufacturer == 0x04 && pDevice->iProduct == 0x07) ||
				
				(pDevice->iManufacturer == 0x0A && pDevice->iProduct == 0x07) ||

				(pDevice->iManufacturer == 0x10) ||
				
				(pDevice->iManufacturer == 0x16) ||
				
				(pDevice->iManufacturer == 0x17) ||
				
			   ((pDevice->iManufacturer & 0xF0) == 0x00 &&
			   
			    (pDevice->iProduct & 0x0F) == 0x05) ) {

				ULONG is_spliter_mode = FALSE;

				if( (pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  768 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  800 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  960 && pSysCfg->n_input_video_resolution_fps == 75) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  960 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 1024 && pSysCfg->n_input_video_resolution_fps == 75) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 1024 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1400 && pSysCfg->n_input_video_resolution_cy == 1050 && pSysCfg->n_input_video_resolution_fps >   0) ||

					(pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy ==  900 && pSysCfg->n_input_video_resolution_fps == 75) ||

					(pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy ==  900 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1680 && pSysCfg->n_input_video_resolution_cy == 1050 && pSysCfg->n_input_video_resolution_fps == 60) ||

					(pSysCfg->n_input_video_resolution_cx == 1680 && pSysCfg->n_input_video_resolution_cy == 1050 && pSysCfg->n_input_video_resolution_fps == 50) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1080 && pSysCfg->n_input_video_resolution_fps == 60) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1080 && pSysCfg->n_input_video_resolution_fps == 61) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1080 && pSysCfg->n_input_video_resolution_fps == 50) ||

					(pSysCfg->n_input_video_resolution_cx == 1600 && pSysCfg->n_input_video_resolution_cy == 1200 && pSysCfg->n_input_video_resolution_fps >   0) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1200 && pSysCfg->n_input_video_resolution_fps >   0) ||

					(pSysCfg->n_input_video_resolution_cx == 3840 && pSysCfg->n_input_video_resolution_cy == 1024 && pSysCfg->n_input_video_resolution_fps >   0) ) {

					if( pSysCfg->n_input_video_resolution_cy == 1200 ) {

						SA7160_SetFpgaRegister( pDevice, 0x24, 0x01 );
					}
					else {

						SA7160_SetFpgaRegister( pDevice, 0x24, 0x00 );
					}
					is_spliter_mode = TRUE;
				}
				else if( pSysCfg->n_input_video_resolution_h_total > 0 ) {

					is_spliter_mode = pSysCfg->b_input_video_resolution_spliter_mode;

					SA7160_SetFpgaRegister( pDevice, 0x24, 0x00 );
				}
				else {

					is_spliter_mode = FALSE;
				}
				if( ((pDevice->iManufacturer & 0xF0) == 0x00 &&
			   
					 (pDevice->iProduct & 0x0F) == 0x05) ) { // FOR ALL SC500/SC502

					if( is_spliter_mode ) {

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x01 ); // BYPASS.MODE

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x00 ); // SPLIT.MODE
					}
					else {

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x00 ); // SPLIT.MODE

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x01 ); // BYPASS.MODE
					}
				}
				else {

					if( is_spliter_mode ) {

						ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
						
						R0000E004 |=  0x00020000;

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

						R0000E004 &= ~0x00020000; // SPLITER.MODE

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
					}
					else {

						ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR

						R0000E004 &= ~0x00020000;

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

						R0000E004 |=  0x00020000; // BYPASS.MODE

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
					}
				}
			}
			#endif
		}
	}
	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

		pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { // DVI.DIGITAL (TMDS.A)

		ULONG n_signal_h_total = 0;
		
		ULONG n_signal_v_total = 0;
		
		ULONG n_signal_h_period = 0; 
		
		ULONG n_signal_v_period = 0;
		
		ULONG n_signal_detect_delay = 0;

		ULONGLONG n_signal_lock_status = MST3367_HDMI_MODE_DETECT( pDevice, &n_signal_h_total, &n_signal_v_total, &n_signal_h_period, &n_signal_v_period, &n_signal_detect_delay );


		BOOLEAN b_is_signal_vaild = FALSE;

		BOOLEAN b_is_signal_changed = FALSE;

		if( n_signal_lock_status & 0x8000000000000000 ) {

			BYTE  fi = (BYTE )((n_signal_lock_status & 0x4000000000000000) >> 62);

			ULONG fv = (ULONG)((n_signal_lock_status & 0x0FFF000000000000) >> 48); // n_signal_v_period

			ULONG cx = (ULONG)((n_signal_lock_status & 0x00000FFF00000000) >> 32); //

			ULONG wx = (ULONG)((n_signal_lock_status & 0x000000000FFF0000) >> 16); // n_signal_h_total

			ULONG wy = (ULONG)((n_signal_lock_status & 0x0000000000000FFF) >>  0); // n_signal_v_total

			ULONG x = 0;

			ULONG y = 0;

			ULONG fps = 0;

			if( (cx ==  720 && wy ==  262) ) { x = 720; y = 240; fps = 60; }

			if( (cx ==  720 && wy ==  263) ) { x = 720; y = 240; fps = 60; }

			if( (cx == 1440 && wy ==  262) ) { x = 720; y = 240; fps = 60; }

			if( (cx == 1440 && wy ==  263) ) { x = 720; y = 240; fps = 60; }

			if( (cx ==  720 && wy ==  312) ) { x = 720; y = 288; fps = 50; }

			if( (cx ==  720 && wy ==  313) ) { x = 720; y = 288; fps = 50; }

			if( (cx == 1440 && wy ==  312) ) { x = 720; y = 288; fps = 50; }

			if( (cx == 1440 && wy ==  313) ) { x = 720; y = 288; fps = 50; }

			if( (cx ==  720 && wy ==  525) ) { x = 720; y = 480; fps = 60; }

			if( (cx ==  720 && wy ==  625) ) { x = 720; y = 576; fps = 50; }

			if( (cx ==  720 && wy ==  446) ) { x = 720; y = 400; fps = 85; }

			if( (cx ==  832 && wy ==  654) ) { x = 832; y = 624; fps = 75; }

			if( (cx == 1440 && (wy & ~1) ==  262) ) { x = fi ? 720 : 1440; y = 240; fps = 60; }

			if( (cx == 1440 && (wy & ~1) ==  312) ) { x = fi ? 720 : 1440; y = 288; fps = 50; }

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 750 -  5)) && (wy <= ( 750 +  5)) && (fv >= (300 -  5)) && (fv <= (300 +  5)) ) { x = 1280; y  = 720; fps = 30; } 
			
			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 750 -  5)) && (wy <= ( 750 +  5)) && (fv >= (250 -  5)) && (fv <= (250 +  5)) ) { x = 1280; y  = 720; fps = 25; } 
			
			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 750 -  5)) && (wy <= ( 750 +  5)) && (fv >= (240 - 5)) && (fv <= (240 + 5)) ) { x = 1280; y  = 720; fps = 24; } 

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 778 -  5)) && (wy <= ( 778 +  5)) && (fv >= (590 - 5)) && (fv <= (590 + 5)) ) { x = 1280; y  = 720; fps = 59; } // TOSHIBA CAMERA

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 750 - 15)) && (wy <= ( 750 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1280; y  = 720; fps = 60; }

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 750 - 15)) && (wy <= ( 750 + 15)) && (fv >= (500 - 5)) && (fv <= (500 + 5)) ) { x = 1280; y  = 720; fps = 50; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= ( 562 - 15)) && (wy <= ( 562 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1920; y  = 540; fps = 60; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= ( 562 - 15)) && (wy <= ( 562 + 15)) && (fv >= (500 - 5)) && (fv <= (500 + 5)) ) { x = 1920; y =  540; fps = 50; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1125 - 15)) && (wy <= (1125 + 15)) && (fv >= (240 - 5)) && (fv <= (240 + 5)) ) { x = 1920; y = 1080; fps = 24; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1125 - 15)) && (wy <= (1125 + 15)) && (fv >= (300 - 5)) && (fv <= (300 + 5)) ) { x = 1920; y = 1080; fps = 30; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1125 - 15)) && (wy <= (1125 + 15)) && (fv >= (250 - 5)) && (fv <= (250 + 5)) ) { x = 1920; y = 1080; fps = 25; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1125 - 15)) && (wy <= (1125 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1920; y = 1080; fps = 60; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1125 - 15)) && (wy <= (1125 + 15)) && (fv >= (500 - 5)) && (fv <= (500 + 5)) ) { x = 1920; y = 1080; fps = 50; }

			BOOL is_cvte_lvds = FALSE;

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy ==         (0)) && (wy == (        0)) ) { x = 1920; y = 1080; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1080 -  5)) && (wy <= (1080 +  5)) ) { x = 1920; y = 1080; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= (1366 - 5)) && (cx <= (1366 + 5)) && (wy ==         (0)) && (wy == (        0)) ) { x = 1368; y =  768; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= (1366 - 5)) && (cx <= (1366 + 5)) && (wy >= ( 768 -  5)) && (wy <= ( 768 +  5)) ) { x = 1368; y =  768; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= (1440 - 5)) && (cx <= (1440 + 5)) && (wy ==         (0)) && (wy == (        0)) ) { x = 1440; y =  900; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= (1440 - 5)) && (cx <= (1440 + 5)) && (wy >= ( 900 -  5)) && (wy <= ( 900 +  5)) ) { x = 1440; y =  900; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= ( 960 - 5)) && (cx <= ( 960 + 5)) && (wy ==         (0)) && (wy == (        0)) ) { x =  960; y =  540; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= ( 960 - 5)) && (cx <= ( 960 + 5)) && (wy >= ( 540 -  5)) && (wy <= ( 540 +  5)) ) { x =  960; y =  540; fps = (fv + 5) / 10; is_cvte_lvds = true; } // LVDS (廣州視源)

			if( (cx >= ( 640 - 5)) && (cx <= ( 640 + 5)) && (wy >= ( 525 - 15)) && (wy <= ( 525 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x =  640; y =  480; fps = 60; }
			
			if( (cx >= ( 640 - 5)) && (cx <= ( 640 + 5)) && (wy >= ( 520 - 15)) && (wy <= ( 520 + 15)) && (fv >= (720 - 8)) && (fv <= (720 + 8)) ) { x =  640; y =  480; fps = 72; }
			
			if( (cx >= ( 640 - 5)) && (cx <= ( 640 + 5)) && (wy >= ( 500 - 15)) && (wy <= ( 500 + 15)) && (fv >= (750 - 5)) && (fv <= (750 + 5)) ) { x =  640; y =  480; fps = 75; }
			
			if( (cx >= ( 640 - 5)) && (cx <= ( 640 + 5)) && (wy >= ( 509 - 15)) && (wy <= ( 509 + 15)) && (fv >= (850 - 5)) && (fv <= (850 + 5)) ) { x =  640; y =  480; fps = 85; }
			

			if( (cx >= ( 800 - 5)) && (cx <= ( 800 + 5)) && (wy >= ( 625 - 15)) && (wy <= ( 625 + 15)) && (fv >= (560 - 5)) && (fv <= (560 + 5)) ) { x =  800; y =  600; fps = 56; }
			
			if( (cx >= ( 800 - 5)) && (cx <= ( 800 + 5)) && (wy >= ( 628 - 15)) && (wy <= ( 628 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x =  800; y =  600; fps = 60; }
			
			if( (cx >= ( 800 - 5)) && (cx <= ( 800 + 5)) && (wy >= ( 666 - 15)) && (wy <= ( 666 + 15)) && (fv >= (720 - 5)) && (fv <= (720 + 5)) ) { x =  800; y =  600; fps = 72; }
			
			if( (cx >= ( 800 - 5)) && (cx <= ( 800 + 5)) && (wy >= ( 625 - 15)) && (wy <= ( 625 + 15)) && (fv >= (750 - 5)) && (fv <= (750 + 5)) ) { x =  800; y =  600; fps = 75; }
			
			if( (cx >= ( 800 - 5)) && (cx <= ( 800 + 5)) && (wy >= ( 720 - 15)) && (wy <= ( 720 + 15)) && (fv >= (750 -  5)) && (fv <= (750 +  5)) ) { x =  800; y =  600; fps = 76; }
			
			if( (cx >= ( 800 - 5)) && (cx <= ( 800 + 5)) && (wy >= ( 631 - 15)) && (wy <= ( 631 + 15)) && (fv >= (850 - 5)) && (fv <= (850 + 5)) ) { x =  800; y =  600; fps = 85; }


			if( (cx >= (1024 - 5)) && (cx <= (1024 + 5)) && (wy >= ( 624 - 15)) && (wy <= ( 624 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1024; y =  600; fps = 60; }


			if( (cx >= (1024 - 5)) && (cx <= (1024 + 5)) && (wy >= ( 804 - 15)) && (wy <= ( 804 + 15)) && (fv >= (500 - 5)) && (fv <= (500 + 5)) ) { x = 1024; y =  768; fps = 50; }
			
			if( (cx >= (1024 - 5)) && (cx <= (1024 + 5)) && (wy >= ( 806 - 15)) && (wy <= ( 806 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1024; y =  768; fps = 60; }
			
			if( (cx >= (1024 - 5)) && (cx <= (1024 + 5)) && (wy >= ( 806 - 15)) && (wy <= ( 806 + 15)) && (fv >= (700 - 5)) && (fv <= (700 + 5)) ) { x = 1024; y =  768; fps = 70; }
			
			if( (cx >= (1024 - 5)) && (cx <= (1024 + 5)) && (wy >= ( 800 - 15)) && (wy <= ( 800 + 15)) && (fv >= (750 - 5)) && (fv <= (750 + 5)) ) { x = 1024; y =  768; fps = 75; }

			if( (cx >= (1024 - 5)) && (cx <= (1024 + 5)) && (wy >= ( 808 - 15)) && (wy <= ( 808 + 15)) && (fv >= (850 - 5)) && (fv <= (850 + 5)) ) { x = 1024; y =  768; fps = 85; }


			if( (cx >= (1064 - 5)) && (cx <= (1064 + 5)) && (wy >= ( 624 - 15)) && (wy <= ( 624 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1064; y =  600; fps = 60; }


			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 798 - 15)) && (wy <= ( 798 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1280; y =  768; fps = 60; }


			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= ( 831 - 15)) && (wy <= ( 831 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1280; y =  800; fps = 60; }


			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= (1000 - 15)) && (wy <= (1000 + 15)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1280; y =  960; fps = 60; }


			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= (1057 - 15)) && (wy <= (1057 + 15)) && (fv >= (500 - 5)) && (fv <= (500 + 5)) ) { x = 1280; y = 1024; fps = 50; }

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= (1082 -  5)) && (wy <= (1082 +  5)) && (fv >= (590 - 5)) && (fv <= (590 + 4)) ) { x = 1280; y = 1024; fps = 59; }

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= (1066 -  5)) && (wy <= (1066 +  5)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1280; y = 1024; fps = 60; }

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= (1066 -  5)) && (wy <= (1066 +  5)) && (fv >= (749 -  5)) && (fv <= (749 +  5)) ) { x = 1280; y = 1024; fps = 75; }

			if( (cx >= (1280 - 5)) && (cx <= (1280 + 5)) && (wy >= (1072 -  5)) && (wy <= (1072 +  5)) && (fv >= (849 -  5)) && (fv <= (849 +  5)) ) { x = 1280; y = 1024; fps = 85; }


			if( (cx >= (1360 - 5)) && (cx <= (1360 + 5)) && (wy >= ( 795 -  5)) && (wy <= ( 795 +  5)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { 

				if( (wx >= (1776 - 5)) && (wx <= (1776 + 5)) ) {

					x = 1360; y =  768; fps = 61; // SEGA

				}
				else {

					x = 1360; y =  768; fps = 60; 
				}
			}
			if( (cx >= (1152 - 5)) && (cx <= (1152 + 5)) && (wy >= ( 900 -  5)) && (wy <= ( 900 +  5)) && (fv >= (750 - 5)) && (fv <= (750 + 5)) ) { x = 1152; y =  864; fps = 75; }


			if( (cx >= (1400 - 5)) && (cx <= (1400 + 5)) && (wy >= (1089 -  5)) && (wy <= (1089 +  5)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1400; y = 1050; fps = 60; }


			if( (cx >= (1440 - 5)) && (cx <= (1440 + 5)) && (wy >= ( 934 -  5)) && (wy <= ( 934 +  5)) && (fv >= (600 - 5)) && (fv <= (600 + 5)) ) { x = 1440; y =  900; fps = 60; }


			if( (cx >= (1440 - 5)) && (cx <= (1440 + 5)) && (wy >= ( 557 -  5)) && (wy <= ( 557 +  5)) && (fv >= (500 - 5)) && (fv <= (500 + 5)) ) { x = 1440; y  = 540; fps = 50; }


			if( (cx >= (1680 - 5)) && (cx <= (1680 + 5)) && (wy >= (1080 - 10)) && (wy <= (1080 + 10)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { x = 1680; y = 1050; fps = 60; }

			if( (cx >= (1680 - 5)) && (cx <= (1680 + 5)) && (wy >= (1080 - 10)) && (wy <= (1080 + 10)) && (fv >= (500 - 10)) && (fv <= (500 + 10)) ) { x = 1680; y = 1050; fps = 50; }
			

			if( (cx >= (1600 - 5)) && (cx <= (1600 + 5)) && (wy >= (1221 - 10)) && (wy <= (1221 + 10)) && (fv >= (300 - 10)) && (fv <= (300 + 10)) ) { x = 1600; y = 1200; fps = 30; }

			if( (cx >= (1600 - 5)) && (cx <= (1600 + 5)) && (wy >= (1238 - 10)) && (wy <= (1238 + 10)) && (fv >= (500 - 10)) && (fv <= (500 + 10)) ) { x = 1600; y = 1200; fps = 50; }

			if( (cx >= (1600 - 5)) && (cx <= (1600 + 5)) && (wy >= (1250 - 10)) && (wy <= (1250 + 10)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { x = 1600; y = 1200; fps = 60; }
			
			
			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1221 - 10)) && (wy <= (1221 + 10)) && (fv >= (300 - 10)) && (fv <= (300 + 10)) ) { x = 1920; y = 1200; fps = 30; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1235 - 10)) && (wy <= (1235 + 10)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { x = 1920; y = 1200; fps = 60; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1250 - 10)) && (wy <= (1250 + 10)) && (fv >= (600 - 10)) && (fv <= (600 + 10)) ) { x = 1920; y = 1200; fps = 60; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1235 - 10)) && (wy <= (1235 + 10)) && (fv >= (500 - 10)) && (fv <= (500 + 10)) ) { x = 1920; y = 1200; fps = 50; }

			if( (cx >= (1920 - 5)) && (cx <= (1920 + 5)) && (wy >= (1250 - 10)) && (wy <= (1250 + 10)) && (fv >= (500 - 10)) && (fv <= (500 + 10)) ) { x = 1920; y = 1200; fps = 50; }

			if( (fps == 0) || (x == 0) || (y == 0) ) { // 啟動顏大哥的自動演算偵測法

				MST3367_SetRegister( pDevice, 0x00, 0xE2, 0x80 ); // ENABLE AUTO POSITION

				MST3367_SetRegister( pDevice, 0x00, 0xE3, 0x08 ); // REPORT DIGITAL DE WINDOW

				MST3367_SetRegister( pDevice, 0x00, 0xE4, 0x00 ); // POSITION THRESHOLD, DIGITAL SET 0 (10 BIT RESOLUTION)

				BYTE R6C = MST3367_GetRegister( pDevice, 0x00, 0x6C );

				BYTE R6D = MST3367_GetRegister( pDevice, 0x00, 0x6D );

				ULONG n_signal_v_width = (R6D & 0x1F) >> 1;

//				LONGLONG ts = n_signal_detect_delay;

//				ts *= 10000;

//				DELAY_100NS( ts );

				wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( n_signal_detect_delay ) );

				ULONG n_signal_h_start = ((MST3367_GetRegister( pDevice, 0x00, 0xF4 ) & 0x0F) << 8) | 

										 ((MST3367_GetRegister( pDevice, 0x00, 0xF3 ) & 0xFF) << 0);

				ULONG n_signal_h_end =   ((MST3367_GetRegister( pDevice, 0x00, 0xF6 ) & 0x0F) << 8) | 

										 ((MST3367_GetRegister( pDevice, 0x00, 0xF5 ) & 0xFF) << 0);

				ULONG n_signal_v_start = ((MST3367_GetRegister( pDevice, 0x00, 0xF8 ) & 0x0F) << 8) | 

										 ((MST3367_GetRegister( pDevice, 0x00, 0xF7 ) & 0xFF) << 0);

				ULONG n_signal_v_end =   ((MST3367_GetRegister( pDevice, 0x00, 0xFA ) & 0x0F) << 8) | 

										 ((MST3367_GetRegister( pDevice, 0x00, 0xF9 ) & 0xFF) << 0);

				ULONG n_signal_h_width = ((n_signal_h_start + 12) / 24) * 8;

				LONGLONG n_calc_cx = n_signal_h_end - n_signal_h_start + 1;

				LONGLONG n_calc_cy = n_signal_v_end - n_signal_v_start + 1;

				LONGLONG n_calc_sz = n_calc_cx * n_calc_cy;

				LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MST3367_HDMI_MODE_DETECT_AUTO( %d x %d x %d x %d x %d x %d x %d )\n", pDevice->m_nKsDeviceNumber, n_signal_h_start, n_signal_h_end, n_signal_v_start, n_signal_v_end, n_signal_h_width, (LONG)(n_calc_cx), (LONG)(n_calc_cy)  );

				if( n_calc_sz <= 2304000 && // 1920 * 1200
					
					n_calc_cx == cx &&
					
					n_calc_cx > 1 &&
					
					n_calc_cy > 1 &&
					
				   (n_calc_cx & 0x07) == 0 ) { // 偵測成功

					x = (ULONG)(n_calc_cx);

					y = (ULONG)(n_calc_cy);

					fps = (fv + 5) / (10);

					pSysCfg->n_input_video_resolution_h_total = n_signal_h_total;

					if( (x * y * fps) > 80000000 ) {

						pSysCfg->b_input_video_resolution_spliter_mode = TRUE;
					}
					else {

						pSysCfg->b_input_video_resolution_spliter_mode = FALSE;
					}
				//	MST3367_SetRegister( pDevice, 0x00, 0xE2, 0x00 ); // DISABLE AUTO POSITION
				}
			}
			else {

				pSysCfg->n_input_video_resolution_h_total = 0;

				pSysCfg->b_input_video_resolution_spliter_mode = FALSE;

				MST3367_SetRegister( pDevice, 0x00, 0xE2, 0x00 ); // DISABLE AUTO POSITION
			}
			//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] n_input_video_resolution_h_total(0d%d) \n", pDevice->m_nKsDeviceNumber, pSysCfg->n_input_video_resolution_h_total );

			if( is_cvte_lvds ) {

				pSysCfg->n_input_video_resolution_h_total = 1;

				pSysCfg->b_input_video_resolution_spliter_mode = TRUE;
			}


			if( x != 0 && y != 0 ) {

				if( pSysCfg->n_input_video_resolution_cx != x ||

					pSysCfg->n_input_video_resolution_cy != y ||
					
					pSysCfg->n_input_video_resolution_fps != fps ) {
					
					if( pSysCfg->n_input_video_resolution_cx == 0 || 
						
						pSysCfg->n_input_video_resolution_cy == 0 || 
						
						pSysCfg->n_input_video_resolution_fps == 0 ) {

						BYTE R0207 = MST3367_GetRegister( pDevice, 0x02, 0x07 ); R0207 |= 0x10; 
						
						MST3367_SetRegister( pDevice, 0x02, 0x07, R0207 );
						
						R0207 &= ~0x10;

						MST3367_SetRegister( pDevice, 0x02, 0x07, R0207 );
					}
					pSysCfg->n_input_video_resolution_cx = x;

					pSysCfg->n_input_video_resolution_cy = y;

					pSysCfg->n_input_video_resolution_fps = fps;

					pSysCfg->n_input_video_resolution_fps_m = MST3367_GET_FREQV( pDevice );

					if( (pSysCfg->n_input_video_resolution_cx ==  720 && pSysCfg->n_input_video_resolution_cy == 240) ||

						(pSysCfg->n_input_video_resolution_cx ==  720 && pSysCfg->n_input_video_resolution_cy == 288) ||

						(pSysCfg->n_input_video_resolution_cx ==  768 && pSysCfg->n_input_video_resolution_cy == 288) ||

						(pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy == 540) ||

						(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 540) ) {

						pSysCfg->n_input_video_resolution_interleaved = 1;
					}
					else {

						pSysCfg->n_input_video_resolution_interleaved = 0;
					}
					b_is_signal_changed = TRUE;
				}
				b_is_signal_vaild = TRUE;
			}
		}
		else {
					
			pSysCfg->n_input_video_resolution_cx = 0;

			pSysCfg->n_input_video_resolution_cy = 0;

			pSysCfg->n_input_video_resolution_fps = 0;

			pSysCfg->n_input_video_resolution_fps_m = 0;

			pSysCfg->n_input_video_resolution_interleaved = 0;

			pSysCfg->n_input_video_resolution_h_total = 0;

			pSysCfg->b_input_video_resolution_spliter_mode = FALSE;
		}
		if( b_is_signal_vaild ) {

			if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

				BYTE R029B = MST3367_GetRegister( pDevice, 0x02, 0x9B ) & 0xF0; 

				pSysCfg->n_input_audio_sampling_frequency = 48000;

				if( R029B == 0x00 ) { pSysCfg->n_input_audio_sampling_frequency = 44100; }

				if( R029B == 0x40 ) { pSysCfg->n_input_audio_sampling_frequency = 48000; }

				if( R029B == 0xC0 ) { pSysCfg->n_input_audio_sampling_frequency = 32000; }
			}
			else {

				pSysCfg->n_input_audio_sampling_frequency = 48000;
			}
		}
		if( b_is_signal_vaild ) {

			ULONG type = MST3367_TMDS_GET_TYPE( pDevice );

			pDevice->m_nCustomAnalogVideoResolutionProperty = (pSysCfg->n_input_video_resolution_cx << 16) |

															  (pSysCfg->n_input_video_resolution_cy <<  0);

			if( pSysCfg->n_input_video_resolution_fps == 59 || 
				
				pSysCfg->n_input_video_resolution_fps == 61 ) {

				pDevice->m_nCustomAnalogVideoFrameRateProperty = 60;
			}
			else if( pSysCfg->n_input_video_resolution_fps == 76 ) {

				pDevice->m_nCustomAnalogVideoFrameRateProperty = 75;
			}
			else {

				pDevice->m_nCustomAnalogVideoFrameRateProperty = pSysCfg->n_input_video_resolution_fps;
			}
			pDevice->m_nCustomAnalogVideoInterleavedProperty = pSysCfg->n_input_video_resolution_interleaved;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = pSysCfg->n_input_audio_sampling_frequency;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 1;

#ifdef MEDICAL
			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
#else
			pDevice->m_nAnalogCopyProtMacrovisionProperty = (type & 0x00000002) ? 1 : 0;
#endif
		}
		else {



			pSysCfg->n_input_video_resolution_cx = 0;
			pSysCfg->n_input_video_resolution_cy = 0;
			pSysCfg->n_input_video_resolution_fps = 0;
			pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

			pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

			pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

			pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

			pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

			pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
		}
		if( b_is_signal_vaild ) {

			ULONG n_packet_status = MST3367_HdmiGetPacketStatus( pDevice );

			if( n_packet_status & 0x00000008 ) {

//				BYTE R0214 = MST3367_GetRegister( pDevice, 0x02, 0x14 ); // -> RxMonitorHdmi
//	
//				if( R0214 & 0x40 ) { // -> RxMonitorHdmi
//	
//					MST3367_SetRegister( pDevice, 0x02, 0x14, R0214 & ~0x40 ); // -> RxMonitorHdmi
//				}
				ULONG n_packet_color = MST3367_HdmiGetPacketColor( pDevice );

				if( pSysCfg->n_input_video_colorspace != n_packet_color ) {

					pSysCfg->n_input_video_colorspace = n_packet_color;

					b_is_signal_changed = TRUE;
				}
			}
			else {

				ULONG n_packet_color = 0x00000000;

				if( pSysCfg->n_input_video_colorspace != n_packet_color ) {

					pSysCfg->n_input_video_colorspace = n_packet_color;

					b_is_signal_changed = TRUE;
				}
			}
		}
		if( b_is_signal_changed ) {

			// -----> RxHandleVideoIn()

			// -----> RxHandleVideoOut()
			//
			
			MST3367_SETUP_CSC_TABLE( pDevice );

			pSysCfg->b_input_video_signal_changed = TRUE;

			#ifdef ENABLE_1920X1080PX60FPS // +SC500

			// FPGA
			// 
			if( (pDevice->iManufacturer == 0x01 && pDevice->iProduct == 0x07) ||
				
				(pDevice->iManufacturer == 0x04 && pDevice->iProduct == 0x07) ||
				
				(pDevice->iManufacturer == 0x0A && pDevice->iProduct == 0x07) ||

				(pDevice->iManufacturer == 0x10) ||
				
				(pDevice->iManufacturer == 0x16) ||
				
				(pDevice->iManufacturer == 0x17) ||
				
			   ((pDevice->iManufacturer & 0xF0) == 0x00 &&
			   
			    (pDevice->iProduct & 0x0F) == 0x05) ) {

				ULONG is_spliter_mode = FALSE;

				if( (pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  768 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  800 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  960 && pSysCfg->n_input_video_resolution_fps == 75) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy ==  960 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 1024 && pSysCfg->n_input_video_resolution_fps == 75) ||

					(pSysCfg->n_input_video_resolution_cx == 1280 && pSysCfg->n_input_video_resolution_cy == 1024 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1400 && pSysCfg->n_input_video_resolution_cy == 1050 && pSysCfg->n_input_video_resolution_fps >   0) ||

					(pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy ==  900 && pSysCfg->n_input_video_resolution_fps == 75) ||

					(pSysCfg->n_input_video_resolution_cx == 1440 && pSysCfg->n_input_video_resolution_cy ==  900 && pSysCfg->n_input_video_resolution_fps == 85) ||

					(pSysCfg->n_input_video_resolution_cx == 1680 && pSysCfg->n_input_video_resolution_cy == 1050 && pSysCfg->n_input_video_resolution_fps == 60) ||

					(pSysCfg->n_input_video_resolution_cx == 1680 && pSysCfg->n_input_video_resolution_cy == 1050 && pSysCfg->n_input_video_resolution_fps == 50) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1080 && pSysCfg->n_input_video_resolution_fps == 60) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1080 && pSysCfg->n_input_video_resolution_fps == 61) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1080 && pSysCfg->n_input_video_resolution_fps == 50) ||

					(pSysCfg->n_input_video_resolution_cx == 1600 && pSysCfg->n_input_video_resolution_cy == 1200 && pSysCfg->n_input_video_resolution_fps >   0) ||

					(pSysCfg->n_input_video_resolution_cx == 1920 && pSysCfg->n_input_video_resolution_cy == 1200 && pSysCfg->n_input_video_resolution_fps >   0) ||

					(pSysCfg->n_input_video_resolution_cx == 3840 && pSysCfg->n_input_video_resolution_cy == 1024 && pSysCfg->n_input_video_resolution_fps >   0) ) {

					if( pSysCfg->n_input_video_resolution_cy == 1200 ) {

						SA7160_SetFpgaRegister( pDevice, 0x24, 0x01 ); // 只有在 SPLIT 模式下才有用
					}
					else {

						SA7160_SetFpgaRegister( pDevice, 0x24, 0x00 );
					}
					is_spliter_mode = TRUE;
				}
				else if( pSysCfg->n_input_video_resolution_h_total > 0 ) {

					is_spliter_mode = pSysCfg->b_input_video_resolution_spliter_mode;

					SA7160_SetFpgaRegister( pDevice, 0x24, 0x00 );
				}
				else {

					is_spliter_mode = FALSE;
				}

				if( ((pDevice->iManufacturer & 0xF0) == 0x00 &&
			   
					 (pDevice->iProduct & 0x0F) == 0x05) ) { // FOR ALL SC500/SC502

					if( is_spliter_mode ) {

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x01 ); // BYPASS.MODE

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x00 ); // SPLIT.MODE
					}
					else {

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x00 ); // SPLIT.MODE

						SA7160_SetFpgaRegister( pDevice, 0x21, 0x01 ); // BYPASS.MODE
					}
				}
				else {

					if( is_spliter_mode ) {

						ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR
						
						R0000E004 |=  0x00020000;

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

						R0000E004 &= ~0x00020000; // SPLITER.MODE

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
					}
					else {

						ULONG R0000E004 = SA7160_GetRegister( pDevice, 0x0000E000 + 0x0004 ); // GPIO.WR

						R0000E004 &= ~0x00020000;

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );

						R0000E004 |=  0x00020000; // BYPASS.MODE

						SA7160_SetRegister( pDevice, 0x0000E000 + 0x0004, R0000E004 );
					}
				}
			}
			#endif
		}
	}


	return TRUE;
}
