#ifndef AH8400_H
#define AH8400_H

#include "LINUXV4L2.h"

#define AH8400_ENTER_PCI_LIVE_DMA_ACCESS() { AH8400_SetRegister( pDevice, 3, 0x007D6000 + 0x00000004, 0x00000201, 4 ); while( AH8400_GetRegister( pDevice, 3, 0x007D6000 + 0x00000004, 4 ) != 0x00000201 ) { AH8400_SetRegister( pDevice, 3, 0x007D6000 + 0x00000004, 0x00000201, 4 ); } }

#define AH8400_LEAVE_PCI_LIVE_DMA_ACCESS() { AH8400_SetRegister( pDevice, 3, 0x007D6000 + 0x00000004, 0x00000200, 4 ); while( AH8400_GetRegister( pDevice, 3, 0x007D6000 + 0x00000004, 4 ) != 0x00000200 ) { AH8400_SetRegister( pDevice, 3, 0x007D6000 + 0x00000004, 0x00000200, 4 ); } }

BOOLEAN AH8400_HwInitialize( CDevice * pDevice );

BOOLEAN AH8400_HwUnInitialize( CDevice * pDevice );

BOOLEAN AH8400_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );

BOOLEAN AH8400_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio );

BOOLEAN AH8400_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN AH8400_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops );

BOOLEAN AH8400_HwProcessAnalogPCIVideoFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

BOOLEAN AH8400_HwProcessAnalogPCIVideoEncoderFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

ULONG AH8400_GetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG nBytes );

BOOLEAN AH8400_SetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG dwValue, ULONG nBytes );

typedef struct g72x_state
{
	long yl;	/* Locked or steady state step size multiplier. */
	short yu;	/* Unlocked or non-steady state step size multiplier. */
	short dms;	/* Short term energy estimate. */
	short dml;	/* Long term energy estimate. */
	short ap;	/* Linear weighting coefficient of 'yl' and 'yu'. */

	short a[2];	/* Coefficients of pole portion of prediction filter. */
	short b[6];	/* Coefficients of zero portion of prediction filter. */
	short pk[2];	/*
			 * Signs of previous two samples of a partially
			 * reconstructed signal.
			 */
	short dq[6];	/*
			 * Previous 6 samples of the quantized difference
			 * signal represented in an internal floating point
			 * format.
			 */
	short sr[2];	/*
			 * Previous 2 samples of the quantized difference
			 * signal represented in an internal floating point
			 * format.
			 */
	char td;	/* delayed tone detect, new in 1988 version */
}g72x_state;

typedef struct CG721Coder
{
	struct g72x_state m_encState;
	struct g72x_state m_decState;

}CG721Coder;

#define	AUDIO_ENCODING_LINEAR	(3)	/* PCM 2's-complement (0-center) */
#define _AUDIO_LITTLE_ENDIAN


#endif
