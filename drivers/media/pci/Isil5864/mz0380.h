#ifndef _MZ0380_H
#define _MZ0380_H

#include "LINUXV4L2.h"
#define  BOOL BOOLEAN                 

typedef struct _MZ0380_SYS_CFG
{
	CSCMatrix  o_csc_matrix;

	CSCMatrix  o_csc_original;

	ULONG      n_output_video_resolution_vi_fw;

	ULONG      n_output_video_resolution_cx;

	ULONG      n_output_video_resolution_cy;

	ULONG      n_input_video_resolution_cx;

	ULONG      n_input_video_resolution_cy;

	ULONG      n_input_video_resolution_fps;

	ULONG      n_input_video_resolution_fps_m;

	ULONG      n_input_video_resolution_interleaved;

	ULONG      n_input_video_resolution_ch_info;

	ULONG      n_input_video_resolution_vbi_lines;

	ULONG      n_input_video_resolution_h_total;

	ULONG      n_input_video_resolution_v_total;

	BOOL       b_input_video_resolution_spliter_mode;

	BOOL	   b_input_video_resolution_external_sync;

	ULONG      b_input_video_resolution_vga_h_period_us;

	ULONG      b_input_video_resolution_vga_v_period_us;

	ULONGLONG  b_input_video_resolution_vga_h_period_us_sum;

	ULONGLONG  b_input_video_resolution_vga_v_period_us_sum;

	ULONG      b_input_video_resolution_vga_period_us_counts;

	LONG	   n_input_video_resolution_offset_x_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_y_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_x_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_y_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_index;

	LONG	   n_input_video_resolution_offset_x_start_fetch;

	LONG	   n_input_video_resolution_offset_y_start_fetch;

	ULONG      n_input_video_colorspace;

	BOOL       b_input_video_signal_changed;

	ULONG      n_input_video_good_adc_phase;

	ULONG      n_input_audio_sampling_frequency;

	BYTE	   n_input_audio_adc_register_pages[ 4 ];

	ULONG	   n_is_firmware_ready;

	ULONG	   n_board_major_version;

	ULONG	   n_board_minor_version;

	ULONG	   n_firmware_major_version;

	ULONG	   n_firmware_minor_version;

	ULONG	   n_firmware_timeouts;

} MZ0380_SYS_CFG;

#define  MZ0380_START_THREAD() { MZ0380_StartControlPanelAnalysisThread( pDevice ); }
#define  MZ0380_CLOSE_THREAD() { MZ0380_StopControlPanelAnalysisThread( pDevice ); }

NTSTATUS MZ0380_HwInitialize( CDevice * pDevice );
NTSTATUS MZ0380_HwUnInitialize( CDevice * pDevice );
NTSTATUS MZ0380_HwAnalogComponentsInitialize( CDevice * pDevice );
NTSTATUS MZ0380_HwAnalogComponentsUnInitialize( CDevice * pDevice );
NTSTATUS MZ0380_HwProcessAnalogPCIPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
NTSTATUS MZ0380_HwProcessAnalogRawVideoFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );
NTSTATUS MZ0380_HwProcessAnalogRawAudioFrame( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
NTSTATUS MZ0380_HwProcessAnalogRawH264Frame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

NTSTATUS MZ0380_SetAnalogVideoProcAmpProperties( CDevice * pDevice );
NTSTATUS MZ0380_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, ULONG nStandard, BOOL THREAD );
NTSTATUS MZ0380_SetAnalogCrossbarRouteProperty( CDevice * pDevice, ULONG nVideoInput, ULONG nVideoOutput, ULONG nAudioInput, ULONG nAudioOutput, BOOL THREAD );

NTSTATUS MZ0380_PowerDownPeripherals( CDevice * pDevice );
static NTSTATUS MZ0380_StartControlPanelAnalysisThread( CDevice * pDevice );
static NTSTATUS MZ0380_StopControlPanelAnalysisThread( CDevice * pDevice );
static int     MZ0380_OnControlPanelAnalysisThread( void * p_context );

BOOLEAN  MZ0380_SetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex, BYTE bValue );
BYTE     MZ0380_GetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex );
BOOLEAN	 MZ0380_SetAnalogVideoDecoderRegister_EX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex, BYTE nValue );
BYTE     MZ0380_GetAnalogVideoDecoderRegister_EX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex );
BYTE	 MZ0380_GetGpioDirection( CDevice * pDevice, BYTE bBitNum );
BOOLEAN  MZ0380_SetGpioDirection( CDevice * pDevice, BYTE bBitNum, BYTE bDirection );
BYTE	 MZ0380_GetGpioValue( CDevice * pDevice, BYTE bBitNum );
BOOLEAN	 MZ0380_SetGpioValue( CDevice * pDevice, BYTE bBitNum, BYTE bValue );
BOOLEAN  MZ0380_SetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG dwValue );
ULONG    MZ0380_GetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex );

BOOLEAN MZ0380_SendVendorCommand_P3( CDevice * pDevice, DWORD config[ 12 ], ULONG length );

BOOLEAN MZ0380_SendVendorCommand( CDevice * pDevice, DWORD config[ 12 ], ULONG length, BOOL wait_semaphore );


BOOLEAN MZ0380_SendVendorCommand_P5( CDevice * pDevice, DWORD config[ 12 ], ULONG length, BOOL wait_semaphore, LONGLONG DELAY );

#endif //_MZ0380_H