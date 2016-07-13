#ifndef  SA7160_H
#define  SA7160_H

#include "LINUXV4L2.h"

#include "ite6603.h"

//#define  SA7160_START_THREAD() { SA7160_StartControlPanelAnalysisThread( pDevice ); }
//#define  SA7160_CLOSE_THREAD() { SA7160_StopControlPanelAnalysisThread( pDevice ); }

#define C_CGU_CLKS 14
typedef struct _SA7160_SYS_CFG
{
	ULONG     n_boot_clock_divider[ C_CGU_CLKS ];

	ULONG     n_current_clock_divider[ C_CGU_CLKS ];

	ULONG     n_frequency_table[ C_CGU_CLKS ];

	CSCMatrix o_csc_matrix;

	CSCMatrix o_csc_original;

	ULONG     n_min_frequency;

	ULONG     n_max_frequency;

	ULONG     n_input_video_resolution_cx;

	ULONG     n_input_video_resolution_cy;

	ULONG     n_input_video_resolution_fps;

	ULONG     n_input_video_resolution_fps_m;
	
	ULONG     n_input_video_resolution_interleaved;
	
	ULONG     n_input_video_resolution_vbi_lines;

	ULONG     n_input_video_resolution_h_total;

	BOOL      b_input_video_resolution_spliter_mode;

	BOOL      b_input_video_resolution_external_sync;

	ULONG     b_input_video_resolution_vga_h_period_us;

	ULONG     b_input_video_resolution_vga_v_period_us;

	ULONGLONG b_input_video_resolution_vga_h_period_us_sum;

	ULONGLONG b_input_video_resolution_vga_v_period_us_sum;

	ULONG     b_input_video_resolution_vga_period_us_counts;

	LONG	  n_input_video_resolution_offset_x_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	  n_input_video_resolution_offset_y_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	  n_input_video_resolution_offset_x_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	  n_input_video_resolution_offset_y_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	  n_input_video_resolution_offset_index;

	LONG	  n_input_video_resolution_offset_x_start_fetch;

	LONG	  n_input_video_resolution_offset_y_start_fetch;

	ULONG     n_input_video_colorspace;

	BOOL      b_input_video_signal_changed;

	ULONG     n_input_video_good_adc_phase;

	ULONG     n_input_audio_sampling_frequency;

	BYTE	  n_input_audio_adc_register_pages[ 4 ];

	CIT6603   o_input_ite6603;

	BOOL	  b_input_disable_device;

	ULONG     m_STDI_LINE_COUNT_MODE; // ADV7181

	ULONG     m_blBackup; // ADV7181

	ULONG     m_cpVideoStd; // ADV7181

	ULONG     m_filterSrc; // ADV7181


	ULONG	n_previous_audio_channel;

	ULONG	n_video_buffer_count;


	ULONG	previous_x; 
	ULONG	previous_y; 
	ULONG	previous_fps;
	ULONG	updating_x;
	ULONG	updating_y;
	ULONG	updating_fps;
	ULONG	Decoder_status_counter;

} SA7160_SYS_CFG;

BOOLEAN SA7160_HwInitialize( CDevice * pDevice );
BOOLEAN SA7160_HwUnInitialize( CDevice * pDevice );
BOOLEAN SA7160_HwAnalogComponentsInitialize( CDevice * pDevice, SA7160_SYS_CFG * p_brother_sys_cfg, BOOL is_ite_table);
BOOLEAN SA7160_HwAnalogComponentsUnInitialize( CDevice * pDevice, SA7160_SYS_CFG * p_brother_sys_cfg );
BOOLEAN SA7160_HwProcessAnalogPCIVideoPacket( CDevice *pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
BOOLEAN	SA7160_HwProcessAnalogPCIAudioPacket( CDevice *pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

//BOOLEAN SA7160_HwProcessAnalogPCIVideoPacket(  PKSPIN pStreamKsPin[ 32 ], PKSSTREAM_POINTER pStreamPointer[ 32 ], BYTE * pPacketDataBuffer, ULONG nPacketDataBufferSize, BYTE * pRawFrameBuffer[ 32 ], ULONG nRawFrameLength[ 32 ], LONGLONG nRawFrameNumber[ 32 ], BOOLEAN bRawFrameComplete[ 32 ], BOOLEAN bRawFrameDrop[ 32 ], PVOID pArgumentA, PVOID pArgumentB, PVOID pArgumentC );
//BOOLEAN SA7160_HwProcessAnalogRawVideoFrame( CStreamPin * pStreamPin, BYTE * pRawFrameBuffer, ULONG nRawFrameLength, ULONG nRawFrameActualLength, BYTE * pFrameBuffer, ULONG nFrameLength, LONGLONG nFrameNumber, BOOLEAN bFrameDrop, PKSSTREAM_HEADER pStreamHeader );
//BOOLEAN SA7160_HwProcessAnalogRawAudioFrame( CStreamPin * pStreamPin, BYTE * pRawFrameBuffer, ULONG nRawFrameLength, ULONG nRawFrameActualLength, BYTE * pFrameBuffer, ULONG nFrameLength, LONGLONG nFrameNumber, BOOLEAN bFrameDrop, PKSSTREAM_HEADER pStreamHeader );

BOOLEAN SA7160_SetAnalogVideoProcAmpProperties( CDevice * pDevice );
BOOLEAN SA7160_SetAnalogCrossbarRouteProperty( CDevice * pDevice );
BOOLEAN SA7160_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, ULONG nStandard );

BOOLEAN SA7160_PowerDownPeripherals( CDevice * pDevice );
//BOOLEAN SA7160_StartControlPanelAnalysisThread( CDevice * pDevice );
//BOOLEAN SA7160_StopControlPanelAnalysisThread( CDevice * pDevice );
//VOID     SA7160_OnControlPanelAnalysisThread( CDevice * pDevice );

BYTE     SA7160_GetAnalogVideoDecoderRegister( CDevice * pDevice, BYTE nIndex );
BOOLEAN  SA7160_SetAnalogVideoDecoderRegister( CDevice * pDevice, BYTE nIndex, BYTE bValue );

BYTE     SA7160_GetMST3367Register( CDevice * pDevice, BYTE nBank, BYTE nIndex );
BOOLEAN  SA7160_SetMST3367Register( CDevice * pDevice, BYTE nBank, BYTE nIndex, BYTE bValue );

BOOLEAN  SA7160_GetSlaveDeviceRegister( CDevice * pDevice, ULONG nBus, BYTE * pIndex, ULONG nIndexLen, BYTE * pValue, ULONG nValueLen );
BOOLEAN  SA7160_SetSlaveDeviceRegister( CDevice * pDevice, ULONG nBus, BYTE * pIndexVlaue, ULONG nIndexVlaueLen );
BOOLEAN  SA7160_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue );
ULONG    SA7160_GetRegister( CDevice * pDevice, ULONG dwIndex );

BYTE SA7160_GetFpgaRegister( CDevice * pDevice, BYTE nIndex );
BOOLEAN SA7160_SetFpgaRegister( CDevice * pDevice, BYTE nIndex, BYTE nValue );

BOOLEAN SA7160_HARDWARE_I2C_RESET( CDevice * pDevice );

static int SA7160_StopControlPanelAnalysisThread( CDevice * pDevice );
static int SA7160_StartControlPanelAnalysisThread( CDevice * pDevice );


#endif
