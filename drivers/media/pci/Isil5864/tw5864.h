#ifndef TW5864_H
#define TW5864_H

#include "LINUXV4L2.h"

BOOLEAN TW5864_HwInitialize( CDevice * pDevice );
BOOLEAN TW5864_HwUnInitialize( CDevice * pDevice );
BOOLEAN TW5864_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );
BOOLEAN TW5864_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );

BOOLEAN TW5864_PowerDownPeripherals( CDevice * pDevice );

BOOLEAN TW5864_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue );
ULONG   TW5864_GetRegister( CDevice * pDevice, ULONG dwIndex );

//peter [2011.05.18]
BOOLEAN TW5864_SetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue );
ULONG	TW5864_GetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG dwIndex );

BOOLEAN TW5864_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );
BOOLEAN TW5864_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN TW5864_HwProcessAnalogPCIVideoFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

BOOLEAN TW5864_HwProcessAnalogPCIVideoEncoderFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

#define _byteswap_ulong(x) \
     (u32) ( (( ((u32)(x)) & 0xff000000 ) >> 24) | \
                    (( ((u32)(x)) & 0x00ff0000 ) >> 8 ) | \
     (( ((u32)(x)) & 0x0000ff00 ) << 8 ) | \
     (( ((u32)(x)) & 0x000000ff ) << 24) \
              )


#define VLD_MAX_CW_LEN              33
#define VLD_MAX_LONG_CW_LEN         65

#define VLD_MAX_UVLC_CODE_NUM       65535         /* 2^16-1 */


typedef struct _bitbuffer_s
{
	/* Bit buffer memory */
	unsigned char  * bufAddr;
	int bufSize;

	/* Bit buffer state */
	unsigned bytePos;
	unsigned currentBits;
	unsigned  bitpos;
} bitbuffer_s;

#define BIT_PUT_BYTE(bitbuf , byte) bitbuf->bufAddr[bitbuf->bytePos++] = (byte)
#define vlcuMapSigned(sV)   (((sV) > 0) ? (2*(sV) - 1) : (2*(-(sV))))

#define ue_v(bitbuf, codeNum)  vlcuSendUVLC(bitbuf, codeNum)
#define se_v(bitbuf, codeNum)  vlcuSendUVLC(bitbuf, vlcuMapSigned((codeNum)))
#define u_n(bitbuf, bits, len) bibPutBits(bitbuf, bits, len)

#define bibGetBit(bitbuf, bit) \
	if ((bitbuf)->bitpos <= 0) { \
	(bitbuf)->currentBits = (bitbuf)->bufAddr[(bitbuf)->bytePos++]; \
	(bitbuf)->bitpos = 7; \
	*(bit) = ((bitbuf)->currentBits >> (bitbuf)->bitpos) & 1; \
	} \
  else { \
  (bitbuf)->bitpos--; \
  *(bit) = ((bitbuf)->currentBits >> (bitbuf)->bitpos) & 1; \
  }

#define bibGetMax16bits(bitbuf, n, bits) \
	if ((n) > (bitbuf)->bitpos) { \
	(bitbuf)->currentBits = ((bitbuf)->currentBits << 8) | (bitbuf)->bufAddr[(bitbuf)->bytePos++]; \
	(bitbuf)->currentBits = ((bitbuf)->currentBits << 8) | (bitbuf)->bufAddr[(bitbuf)->bytePos++]; \
	(bitbuf)->bitpos += 16; \
	(bitbuf)->bitpos -= (n); \
	*(bits) = ((bitbuf)->currentBits >> (bitbuf)->bitpos) & ~(((unsigned)-1)<<(n)); \
	} \
  else { \
  (bitbuf)->bitpos -= (n); \
  *(bits) = ((bitbuf)->currentBits >> (bitbuf)->bitpos) & ~(((unsigned)-1)<<(n)); \
  }



void bibInit(bitbuffer_s *bitbuf , int size , unsigned char * bufAddr);
int bibPutByte(bitbuffer_s *bitbuf, unsigned char byte);
int bibPutBits(bitbuffer_s *bitbuf, unsigned bits, unsigned len);
int bibByteAlign(bitbuffer_s *bitbuf);
int bibTrailingBits(bitbuffer_s *bitbuf);

int vlcuSendUVLC(bitbuffer_s *bitbuf, unsigned int codeNum);
int vlcuSendNalUnitStartCodePrefix(bitbuffer_s *bitbuf, int zeroByte);
int vlcuSendNalHead(bitbuffer_s *bitbuf, int nal_ref_idc, int nal_type);
unsigned int vldGetUVLC(bitbuffer_s *bitbuf);

void CBR_to_VBR( CVideo * pVideo, ULONG m_nSubDeviceNumber );

void TW5864_update_resolution_dynamic( CDevice * pDevice, CVideo * pVideo );						

typedef struct _TIME_FIELDS {
    USHORT Year;        // range [1601...]
    USHORT Month;       // range [1..12]
    USHORT Day;         // range [1..31]
    USHORT Hour;        // range [0..23]
    USHORT Minute;      // range [0..59]
    USHORT Second;      // range [0..59]
    USHORT Milliseconds;// range [0..999]
    USHORT Weekday;     // range [0..6] == [Sunday..Saturday]
} TIME_FIELDS;

typedef struct _TW5864_SYS_CFG
{
//	x264_encoder_t s_video_encoder[ 16 ];

	ULONG		   n_video_encoder_reference_buffer_index[ 16 ];

	ULONG		   n_video_encoder_input_buffer_index[ 16 ];

	ULONG		   n_video_encoder_selected_channel;

	ULONG		   n_video_encoder_frame_cx[ 16 ];

	ULONG		   n_video_encoder_frame_cy[ 16 ];

	ULONG		   n_video_encoder_frame_skip_fps[ 16 ];

	ULONG		   n_video_encoder_frame_avg_fps[ 16 ];

	ULONG		   n_video_encoder_frame_fps_mask[ 16 ];

	ULONG		   n_video_encoder_frame_fps_mask_length[ 16 ];

	ULONGLONG	   n_video_encoder_frame_number[ 16 ];

	ULONG		   n_video_encoder_gop[ 16 ];

	ULONG		   n_video_encoder_gop_step[ 16 ];
//??
	ULONG		   n_picture_number_to_user[ 16 ];

	ULONG		   n_video_encoder_record_mode[ 16 ];

	UINT		   n_video_encoder_init_qp[ 16 ];

	UINT		   n_video_encoder_qp[ 16 ];

	ULONG		   n_video_encoder_bitrate[ 16 ];

	ULONG		   n_viode_encoder_cbr_frame_size_window[ 16 ][ 256 ]; // CBR.WINDOW.SIZE = 256

	ULONG		   n_viode_encoder_cbr_frame_size_window_step[ 16 ];

	ULONGLONG	   n_viode_encoder_cbr_frame_size_sum[ 16 ];

	ULONG		   n_viode_encoder_cbr_qp_base[ 16 ];

	ULONG		   n_viode_encoder_cbr_qp_base_updated_damping[ 16 ];

	BYTE		   p_video_encoder_osd_punchanged_buf[ 88 * 32 ];

	BYTE		   p_video_encoder_osd_pchanged_buf[ 88 * 32 ];

	TIME_FIELDS	   s_video_encoder_osd_timer[ 16 ];

	ULONG          n_audio_encoder_reference_buffer_index[ 8 ];

	ULONG		   R00009801;
//TW5864 encode
	ULONG		   n_preview_video_satrt;
	ULONG		   n_H264_output_enable;

} TW5864_SYS_CFG;


#endif
