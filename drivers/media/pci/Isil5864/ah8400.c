#include "LINUXV4L2.h"
#include "property.h"
#include "ah8400.h"

//#define BCD_TO_BINARY( counts ) (((counts) >> 4) * 10 + ((counts) & 0x0F)) 

int Init(int ch);
struct CG721Coder  CG721[16]; 
extern CDevice * g_pDevice[ 128 ];

ULONG AH8400_GetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG nBytes )
{
	ULONG R = 0x00000000;

	if( nBase == 3 ) {

		if( nBytes == 4 ) { R = readl( (pDevice->m_pRegBaseCommonBuffer[ 1 ] + dwIndex) ) & 0xFFFFFFFF; }

		if( nBytes == 2 ) { R = readw( (pDevice->m_pRegBaseCommonBuffer[ 1 ] + dwIndex) ) & 0x0000FFFF; }

		if( nBytes == 1 ) { R = readb( (pDevice->m_pRegBaseCommonBuffer[ 1 ] + dwIndex) ) & 0x000000FF; }
	}
	if( nBase == 2 ) {

		if( nBytes == 4 ) { R = readl( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0xFFFFFFFF; }

		if( nBytes == 2 ) { R = readw( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x0000FFFF; }

		if( nBytes == 1 ) { R = readb( (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ) & 0x000000FF; }
	}
	return R;
}

BOOLEAN AH8400_SetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG dwValue, ULONG nBytes )
{
	if( nBase == 3 ) {

		if( nBytes == 4 ) { writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 1 ] + dwIndex) ); }
		
		if( nBytes == 2 ) { writew( (dwValue & 0x0000FFFF), (pDevice->m_pRegBaseCommonBuffer[ 1 ] + dwIndex) ); }

		if( nBytes == 1 ) { writeb( (dwValue & 0x000000FF), (pDevice->m_pRegBaseCommonBuffer[ 1 ] + dwIndex) ); }
	}
	if( nBase == 2 ) {

		if( nBytes == 4 ) { writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }
		
		if( nBytes == 2 ) { writew( (dwValue & 0x0000FFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }

		if( nBytes == 1 ) { writeb( (dwValue & 0x000000FF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); }
	}
	return TRUE;
}

VOID AH8400_KeStallExecutionProcessor( CDevice * pDevice, ULONGLONG delay_us )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > delay_us ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

BYTE AH8400_GetAnalogVideoDecoderRegister( CDevice * pDevice, BYTE bDevAddr, BYTE bIndex )
{
	BYTE status = 0x00;

	BYTE R = 0;

	BYTE i = 0;

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bDevAddr & 0xFE, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000080 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 1 ): GET REGISTER %02X × %02X ERROR!!\n", bDevAddr, bIndex ); return 0xFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bIndex, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 2 ): GET REGISTER %02X × %02X ERROR!!\n", bDevAddr, bIndex ); return 0xFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bDevAddr | 0x01, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000080 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 3 ): GET REGISTER %02X × %02X ERROR!!\n", bDevAddr, bIndex ); return 0xFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000040 | 0x00000020 | 0x00000008, 4 );
		
	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 4 ): GET REGISTER %02X × %02X ERROR!!\n", bDevAddr, bIndex ); return 0xFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}

	R = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, 1 ));

	return R;
}

BOOLEAN AH8400_SetAnalogVideoDecoderRegister( CDevice * pDevice, BYTE bDevAddr, BYTE bIndex, BYTE bValue )
{
	BYTE status = 0x00;

	BYTE i = 0;

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bDevAddr & 0xFE, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000080 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 1 ): SET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndex, bValue); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bIndex, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 2 ): SET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndex, bValue); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bValue, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000040 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 3 ): SET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndex, bValue); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 256 );
	}
	return TRUE;
}

USHORT AH8400_GetAnalogVideoMultiplexerRegister( CDevice * pDevice, BYTE bDevAddr, BYTE bIndexH, BYTE bIndexL )
{
	BYTE status = 0x00;

	BYTE RH = 0x00;
		
	BYTE RL= 0x00; 
		
	BYTE i = 0;

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, (bDevAddr & 0xFE) | (bIndexH << 6), 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000080 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 1 ): GET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL ); return 0xFFFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bIndexL, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 2 ): GET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL ); return 0xFFFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bDevAddr | 0x01, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000080 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 3 ): GET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL ); return 0xFFFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000020, 4 );
		
	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 4 ): GET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL ); return 0xFFFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	RH = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, 1 ));

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000040 | 0x00000020 | 0x00000008, 4 );
		
	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_GetAnalogVideoDecoderRegister( 5 ): GET REGISTER %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL ); return 0xFFFF; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	RL = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, 1 ));

	return (RH << 8) | (RL << 0);
}

BOOLEAN AH8400_SetAnalogVideoMultiplexerRegister( CDevice * pDevice, BYTE bDevAddr, BYTE bIndexH, BYTE bIndexL, BYTE bValueH, BYTE bValueL )
{
	BYTE status = 0x00;

	BYTE i = 0;

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, (bDevAddr & 0xFE) | (bIndexH << 6), 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000080 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 1 ): SET REGISTER %02X × %02X × %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL, bValueH, bValueL ); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bIndexL, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 2 ): SET REGISTER %02X × %02X × %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL, bValueH, bValueL ); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bValueH, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 3 ): SET REGISTER %02X × %02X × %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL, bValueH, bValueL ); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x0000000C, bValueL, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 0x00000040 | 0x00000010, 4 );

	for( i = 0 ; i < 100 ; i++ ) {

		status = (BYTE)(AH8400_GetRegister( pDevice, 3, 0x007D5000 + 0x00000010, 1 ));

		if( (status & 0x02) == 0x00 ) { break; }

		if( (i == 99) ) { LINUXV4L2_DEBUG( KERN_INFO, "AH8400_SetAnalogVideoDecoderRegister( 4 ): SET REGISTER %02X × %02X × %02X × %02X × %02X ERROR!!\n", bDevAddr, bIndexH, bIndexL, bValueH, bValueL ); return FALSE; }

		AH8400_KeStallExecutionProcessor( pDevice, 512 );
	}
	return TRUE;
}

VOID AH8400_I2C_SET_SCL( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||

		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		ULONG R007D23FC = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, 4 ); R007D23FC &= 0xFFFFFFBF;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, R007D23FC | (DATA << 6), 4 );
	}
	else {

		ULONG R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= 0xFFFFFFF7;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC | (DATA << 3), 4 );
	}
	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

VOID AH8400_I2C_SET_SDA( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||

		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		ULONG R007D23FC = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, 4 ); R007D23FC &= 0xFFFFFF7F;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, R007D23FC | (DATA << 7), 4 );
	}
	else {

		ULONG R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= 0xFFFFFFEF;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC | (DATA << 4), 4 );
	}
	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

VOID AH8400_I2C_SET_PIN( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY ) 
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	BYTE SCL = (DATA >> 0) & (0x01);

	BYTE SDA = (DATA >> 1) & (0x01);

	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||

		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		ULONG R007D23FC = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, 4 ); R007D23FC &= 0xFFFFFF3F;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, R007D23FC | (SDA << 7) | (SCL << 6), 4 );
	}
	else {

		ULONG R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= 0xFFFFFFE7;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC | (SDA << 4) | (SCL << 3), 4 );
	}
	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
}

BYTE AH8400_I2C_GET_SDA( CDevice * pDevice, ULONGLONG DELAY ) 
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	BYTE DATA = 0x00;

	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||

		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		ULONG R007D2400 = 0x00000000;

		ULONG R007D23FC = 0x00000000;

		R007D2400 = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x00000400, 4 ); R007D2400 &= 0xFFFFFF7F;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x00000400, R007D2400, 4 );

		R007D23FC = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, 4 ); R007D23FC >>= 7; R007D23FC &= 0x00000001; DATA = (BYTE)(R007D23FC);

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x00000400, R007D2400 | 0x00000080, 4 );
	}	
	else {

		ULONG R007D0400 = 0x00000000;

		ULONG R007D03FC = 0x00000000;

		R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 &= 0xFFFFFFEF;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

		R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC >>= 4; R007D03FC &= 0x00000001; DATA = (BYTE)(R007D03FC);

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400 | 0x00000010, 4 );
	}
	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
	return (BYTE)(DATA);
}

BOOLEAN AH8400_I2C_START( CDevice * pDevice, ULONGLONG DELAY )
{
	AH8400_I2C_SET_SDA( pDevice, 1, DELAY );

	AH8400_I2C_SET_PIN( pDevice, 3, DELAY );

	AH8400_I2C_SET_PIN( pDevice, 1, DELAY );

	AH8400_I2C_SET_PIN( pDevice, 0, DELAY );

	return TRUE;
}

BOOLEAN AH8400_I2C_STOP( CDevice * pDevice, ULONGLONG DELAY )
{
	AH8400_I2C_SET_SCL( pDevice, 0, DELAY );

	AH8400_I2C_SET_PIN( pDevice, 0, DELAY );

	AH8400_I2C_SET_PIN( pDevice, 1, DELAY );

	AH8400_I2C_SET_PIN( pDevice, 3, DELAY );

	return TRUE;
}

BOOLEAN AH8400_I2C_WAIT_ACK( CDevice * pDevice, ULONGLONG DELAY )
{	
	BYTE ACK = 0;

	AH8400_I2C_SET_PIN( pDevice, 3, DELAY );    

	ACK = AH8400_I2C_GET_SDA( pDevice, DELAY ); 

	AH8400_I2C_SET_PIN( pDevice, 0, DELAY );    

	return (ACK == 0) ? TRUE : FALSE;           
}

BOOLEAN AH8400_I2C_REPLY_ACK( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY )
{
	AH8400_I2C_SET_PIN( pDevice, (ACK) ? 2 : 0, DELAY );

	AH8400_I2C_SET_PIN( pDevice, (ACK) ? 3 : 1, DELAY );

	AH8400_I2C_SET_PIN( pDevice, (ACK) ? 2 : 0, DELAY );

	return TRUE;
}

BOOLEAN AH8400_I2C_WRITE_BYTE( CDevice * pDevice, BYTE DATA, ULONGLONG DELAY )
{
	ULONG i = 0;

	for( i = 0 ; i < 8 ; i++, DATA <<= 1 ) {                         

		AH8400_I2C_SET_PIN( pDevice, (DATA & 0x80) ? 2 : 0, DELAY ); 

		AH8400_I2C_SET_PIN( pDevice, (DATA & 0x80) ? 3 : 1, DELAY ); 

		AH8400_I2C_SET_PIN( pDevice, (DATA & 0x80) ? 2 : 0, DELAY ); 
	}
	return AH8400_I2C_WAIT_ACK( pDevice, DELAY );                    
}

BYTE AH8400_I2C_READ_BYTE( CDevice * pDevice, BYTE ACK, ULONGLONG DELAY )
{	
	BYTE DATA = 0x00;

	ULONG i = 0;

	for( i = 0 ; i < 8 ; i++ ) {				                  

		AH8400_I2C_SET_PIN( pDevice, 1, DELAY );				  

		DATA = (DATA << 1) | AH8400_I2C_GET_SDA( pDevice, DELAY );

		AH8400_I2C_SET_PIN( pDevice, 0, DELAY );				  
	}
	AH8400_I2C_REPLY_ACK( pDevice, ACK, DELAY );                  

	return DATA;
}

BOOLEAN AH8400_HPI_WRITE_BYTE_EX( CDevice * pDevice, BYTE DEV, BYTE ADDR, BYTE DATA, ULONGLONG DELAY )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	ULONG R007D13FC = (ADDR << 0) | (DEV << 3);

	ULONG R007D03FC = (DATA << 0);

	ULONG R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x000000FF;

	AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x000000E0, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x00000060, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x00000020, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC | 0x00000000, 4 );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x00000060, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x000000E0, 4 );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
	return TRUE;
}

BYTE AH8400_HPI_READ_BYTE_EX( CDevice * pDevice, BYTE DEV, BYTE ADDR, ULONGLONG DELAY )
{
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	struct timeval ts;

	ULONG R007D03FC = 0x00000000;

	ULONG R007D13FC = (ADDR << 0) | (DEV << 3);

	ULONG R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 &= 0xFFFFFF00;

	AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x000000E0, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x00000060, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x00000040, 4 );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
	R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= 0x000000FF;

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x00000060, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x000003FC, R007D13FC | 0x000000E0, 4 );

	do_gettimeofday( &ts );

	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	stop_times = start_times;

	while( TRUE ) {

		if( stop_times - start_times > DELAY ) { break; }

		do_gettimeofday( &ts );

		stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
	}
	return (BYTE)(R007D03FC);
}


BOOLEAN AH8400_HPI_WRITE_BYTE( CDevice * pDevice, BYTE DEV, BYTE BANK, BYTE ADDR, BYTE DATA, ULONGLONG DELAY )
{
	AH8400_HPI_WRITE_BYTE_EX( pDevice, DEV, 0x00, BANK, DELAY );

	AH8400_HPI_WRITE_BYTE_EX( pDevice, DEV, 0x02, ADDR, DELAY );

	AH8400_HPI_WRITE_BYTE_EX( pDevice, DEV, 0x01, DATA, DELAY );

	return TRUE;
}

BYTE AH8400_HPI_READ_BYTE( CDevice * pDevice, BYTE DEV, BYTE BANK, BYTE ADDR, ULONGLONG DELAY )
{
	AH8400_HPI_WRITE_BYTE_EX( pDevice, DEV, 0x00, BANK, DELAY );

	AH8400_HPI_WRITE_BYTE_EX( pDevice, DEV, 0x02, ADDR, DELAY );

	return AH8400_HPI_READ_BYTE_EX( pDevice, DEV, 0x01, DELAY );
}

BOOLEAN AH8400_HPI_SET_RECORD_TABLE( CDevice * pDevice, BYTE DEV, BYTE DATA, ULONGLONG DELAY )
{
	AH8400_HPI_WRITE_BYTE_EX( pDevice, DEV, 0x03, DATA, DELAY );

	return TRUE;
}

static ULONG g_copy_protect_unlock_boradsA[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static ULONG g_copy_protect_unlock_boradsB[ 16 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

BOOLEAN AH8400_SetAnalogVideoDecoderProcAmpProperties( CDevice * pDevice, ULONG nSubDeviceNumber )
{
//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] CX2581_SetAnalogVideoDecoderProcAmpProperties()\n", pDevice->m_nKsDeviceNumber );

	ULONG i = pDevice->m_nKsDeviceNumber * 4 + nSubDeviceNumber * 1;

	ULONG j = nSubDeviceNumber;

	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000001 ) {

		if( g_n_analog_decoder_brightness[ i ][ 0 ] & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x01), ((BYTE)(g_n_analog_decoder_brightness[ i ][ 0 ] & 0xFF) - 128) | 0x00 ); g_n_analog_decoder_brightness[ i ][ 0 ] &= 0x7FFFFFFF; }
		
		if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x02), ((BYTE)(g_n_analog_decoder_contrast[ i ][ 0 ]   & 0xFF) -  28) | 0x00 ); g_n_analog_decoder_contrast[ i ][ 0 ]   &= 0x7FFFFFFF; }
		
		if( g_n_analog_decoder_saturation[ i ][ 0 ] & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x04), ((BYTE)(g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) -   0) | 0x00 );
		
																	 AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x05), ((BYTE)(g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) -   0) | 0x00 ); g_n_analog_decoder_saturation[ i ][ 0 ] &= 0x7FFFFFFF; }
		
		if( g_n_analog_decoder_hue[ i ][ 0 ]        & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x06), ((BYTE)(g_n_analog_decoder_hue[ i ][ 0 ]        & 0xFF) - 128) | 0x00 ); g_n_analog_decoder_hue[ i ][ 0 ]        &= 0x7FFFFFFF; }

		if( g_n_analog_decoder_sharpness[ i ][ 0 ]  & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x03), ((BYTE)(g_n_analog_decoder_sharpness[ i ][ 0 ]  & 0xFF) >>  4) | 0x10 ); g_n_analog_decoder_sharpness[ i ][ 0 ]  &= 0x7FFFFFFF; }

		{//	ULONG R00 = AH8400_GetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x00) );

		//	g_n_analog_decoder_lock_status[ i ] = ((R00 & 0xC0) == 0x40) ? (1) : (0);
		}
	}
	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000002 ) {

		CDevice * pBrotherDevice = g_pDevice[ pDevice->m_nKsDeviceNumber + 1 ];

		if( pBrotherDevice ) {
	
			i += 4;

			if( g_n_analog_decoder_brightness[ i ][ 0 ] & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x01), ((BYTE)(g_n_analog_decoder_brightness[ i ][ 0 ] & 0xFF) - 128) | 0x00 ); g_n_analog_decoder_brightness[ i ][ 0 ] &= 0x7FFFFFFF; }
			
			if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x02), ((BYTE)(g_n_analog_decoder_contrast[ i ][ 0 ]   & 0xFF) -  28) | 0x00 ); g_n_analog_decoder_contrast[ i ][ 0 ]   &= 0x7FFFFFFF; }
			
			if( g_n_analog_decoder_saturation[ i ][ 0 ] & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x04), ((BYTE)(g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) -   0) | 0x00 );
			
																		 AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x05), ((BYTE)(g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) -   0) | 0x00 ); g_n_analog_decoder_saturation[ i ][ 0 ] &= 0x7FFFFFFF; }
			
			if( g_n_analog_decoder_hue[ i ][ 0 ]        & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x06), ((BYTE)(g_n_analog_decoder_hue[ i ][ 0 ]        & 0xFF) - 128) | 0x00 ); g_n_analog_decoder_hue[ i ][ 0 ]        &= 0x7FFFFFFF; }

			if( g_n_analog_decoder_sharpness[ i ][ 0 ]  & 0x80000000 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x03), ((BYTE)(g_n_analog_decoder_sharpness[ i ][ 0 ]  & 0xFF) >>  4) | 0x10 ); g_n_analog_decoder_sharpness[ i ][ 0 ]  &= 0x7FFFFFFF; }

			{//	ULONG R00 = AH8400_GetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x00) );

			//	g_n_analog_decoder_lock_status[ i ] = ((R00 & 0xC0) == 0x40) ? (1) : (0);
			}
		}
	}
	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) {

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { 

			if( g_n_analog_decoder_brightness[ i ][ 0 ] & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x21 + (0x10 * j)), (BYTE)(g_n_analog_decoder_brightness[ i ][ 0 ] & 0xFF) - 128, 10 ); }

			if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x22 + (0x10 * j)), (BYTE)(g_n_analog_decoder_contrast[ i ][ 0 ]   & 0xFF) +  39, 10 ); }

			if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000040 ) { // NVP1114MXA
					
				if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x22 + (0x10 * j)), (BYTE)(g_n_analog_decoder_contrast[ i ][ 0 ] & 0xFF) - 128 - 85, 10 ); }
			}

			if( g_n_analog_decoder_hue[ i ][ 0 ]        & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x23 + (0x10 * j)), (BYTE)(g_n_analog_decoder_hue[ i ][ 0 ]        & 0xFF) - 128, 10 ); }

			if( g_n_analog_decoder_saturation[ i ][ 0 ] & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x24 + (0x10 * j)), (BYTE)(g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) +  12, 10 ); }
		}
		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { 

			if( g_n_analog_decoder_brightness[ i ][ 0 ] & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x21 + (0x10 * j)), (BYTE)(g_n_analog_decoder_brightness[ i ][ 0 ] & 0xFF) - 128, 10 ); }

			if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x22 + (0x10 * j)), (BYTE)(g_n_analog_decoder_contrast[ i ][ 0 ]   & 0xFF) +  62, 10 ); }

			if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000040 ) { // NVP1114MXA
					
				if( g_n_analog_decoder_contrast[ i ][ 0 ]   & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x22 + (0x10 * j)), (BYTE)(g_n_analog_decoder_contrast[ i ][ 0 ] & 0xFF) - 128 - 85, 10 ); }
			}

			if( g_n_analog_decoder_hue[ i ][ 0 ]        & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x23 + (0x10 * j)), (BYTE)(g_n_analog_decoder_hue[ i ][ 0 ]        & 0xFF) - 128, 10 ); }
		
			if( g_n_analog_decoder_saturation[ i ][ 0 ] & 0x08000000 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(0x24 + (0x10 * j)), (BYTE)(g_n_analog_decoder_saturation[ i ][ 0 ] & 0xFF) +   0, 10 ); }
		}
		{	;
		}
		g_n_analog_decoder_brightness[ i ][ 0 ] &= 0xF7FFFFFF;

		g_n_analog_decoder_contrast[ i ][ 0 ] &= 0xF7FFFFFF;

		g_n_analog_decoder_hue[ i ][ 0 ] &= 0xF7FFFFFF;

		g_n_analog_decoder_saturation[ i ][ 0 ] &= 0xF7FFFFFF;

		g_n_analog_decoder_sharpness[ i ][ 0 ] &= 0xF7FFFFFF;
	}
	return TRUE;
}

BOOLEAN AH8400_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, CVideo * pVideo )
{
	ULONG e_v4l2_std = pDevice->m_nCustomVideoStandardProperty;

	BOOLEAN is_tw2864_exist = FALSE;

	BOOLEAN is_ah8816_exist = FALSE;

	BOOLEAN is_nvp1114mx_exist = FALSE;

	ULONG i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] AH8400_SetAnalogVideoDecoderStandardProperty( S:%08lX )\n", pDevice->m_nKsDeviceNumber, e_v4l2_std );

	for( i = 0 ; i < 4 ; i++ ) {

		BYTE TW2864_REGISTER_TABLES[ 0x02 ][ 0x0100 ] = { { 0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x02,
															0x14, 0xF2, 0x0A, 0xD0, 0x00, 0x00, 0x00, 0x7F,
															0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x02,
															0x14, 0xF2, 0x0A, 0xD0, 0x00, 0x00, 0x00, 0x7F,
															0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x02,
															0x14, 0xF2, 0x0A, 0xD0, 0x00, 0x00, 0x00, 0x7F,
															0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x02,
															0x14, 0xF2, 0x0A, 0xD0, 0x00, 0x00, 0x00, 0x7F,

															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

															0x00, 0x02, 0x00, 0xCC, 0x00, 0x80, 0x44, 0x50,
															0x22, 0xF1, 0xD8, 0xBC, 0xB8, 0x44, 0x38, 0x00,
															0x00, 0x78, 0x44, 0x30, 0x14, 0xA5, 0xE6, 0x05,
															0x00, 0x28, 0x44, 0x44, 0x20, 0x8A, 0x42, 0x00,
															0x08, 0x08, 0x08, 0x08, 0x1A, 0x1A, 0x1A, 0x1A,
															0x88, 0x88, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0x22,
															0x22, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x39, 0xE4, 0x00, 0x00,
															0x88, 0x88, 0x01, 0x10, 0x32, 0x54, 0x76, 0x98,
															0xBA, 0xDC, 0xFE, 0xC1, 0x00, 0x00, 0x00, 0x40,
															0x14, 0xC0, 0xAA, 0xAA, 0x00, 0x11, 0x00, 0x00,
															0x11, 0x00, 0x00, 0x11, 0x00, 0x00, 0x11, 0x00,
															0x07, 0x6B, 0x13, 0xEF, 0x0A, 0x01, 0x01, 0x20,
															0x64, 0x11, 0x40, 0x8F, 0xFF, 0x00, 0x00, 0x30 
														  },
														  { 0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x12,
															0x18, 0x20, 0x06, 0xD0, 0x00, 0x00, 0x01, 0x7F,
															0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x12,
															0x18, 0x20, 0x06, 0xD0, 0x00, 0x00, 0x01, 0x7F,
															0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x12,
															0x18, 0x20, 0x06, 0xD0, 0x00, 0x00, 0x01, 0x7F,
															0x00, 0x00, 0x64, 0x11, 0x80, 0x80, 0x00, 0x12,
															0x18, 0x20, 0x06, 0xD0, 0x00, 0x00, 0x01, 0x7F,

															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

															0x00, 0x02, 0x00, 0xCC, 0x00, 0x80, 0x44, 0x50,
															0x22, 0xF1, 0xD8, 0xBC, 0xB8, 0x44, 0x38, 0x00,
															0x00, 0x78, 0x44, 0x3E, 0x14, 0xA5, 0xE6, 0x05,
															0x00, 0x28, 0x44, 0x44, 0x20, 0x90, 0x42, 0x00,
															0x08, 0x08, 0x08, 0x08, 0x1A, 0x1A, 0x1A, 0x1A,
															0x88, 0x88, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0x22,
															0x22, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x39, 0xE4, 0x00, 0x00,
															0x88, 0x88, 0x01, 0x10, 0x32, 0x54, 0x76, 0x98,
															0xBA, 0xDC, 0xFE, 0xC1, 0x00, 0x00, 0x00, 0x40,
															0x14, 0xC0, 0xAA, 0xAA, 0x00, 0x11, 0x00, 0x00,
															0x11, 0x00, 0x00, 0x11, 0x00, 0x00, 0x11, 0x00,
															0x07, 0x6B, 0x13, 0x00, 0x40, 0x01, 0x01, 0x20,
															0x64, 0x11, 0x40, 0x8F, 0xFF, 0x00, 0x00, 0x00
														  }
		};
		
		LONG j = 0 ;
	
		if( AH8400_GetAnalogVideoDecoderRegister( pDevice, 0x50 + (BYTE)(i * 0x02), 0xFE ) != 0x00 ) { continue ; }

		if( AH8400_GetAnalogVideoDecoderRegister( pDevice, 0x50 + (BYTE)(i * 0x02), 0xFF ) != 0x63 ) { continue ; }

		pDevice->m_nCustomHardwareTopologyProperty |= (0x00000001 << i);

		is_tw2864_exist = TRUE;

		for( j = 0 ; j < 0x0100 ; j++ ) { 
			
			if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50 + (BYTE)(i * 0x02), (BYTE)(j), TW2864_REGISTER_TABLES[ 0 ][ j ] ); }

			if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x50 + (BYTE)(i * 0x02), (BYTE)(j), TW2864_REGISTER_TABLES[ 1 ][ j ] ); }
		}
	}
	if( is_tw2864_exist == FALSE ) {

		BYTE AM8816_REGISTER_TABLES[ 0x02 ][ 0x0180 ] = { {	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x32, 0x10, 0x00, 0x00, 0xFF, 0xFF,
															0x32, 0x10, 0x76, 0x54, 0xBA, 0x98, 0xFE, 0xDC, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
															0x00, 0x00, 0x00, 0x2D, 0x00, 0x5A, 0x00, 0x87, 0x1E, 0x00, 0x1E, 0x2D, 0x1E, 0x5A, 0x1E, 0x87,
															0x3C, 0x00, 0x3C, 0x2D, 0x3C, 0x5A, 0x3C, 0x87, 0x5A, 0x00, 0x5A, 0x2D, 0x5A, 0x5A, 0x5A, 0x87,
															0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D,
															0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D,
															0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
															0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,	
															0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x14, 0x14, 0x78, 0xB4, 0x00, 0xC0,
															0x00, 0x00, 0x00, 0x00, 0xC0, 0x10, 0x80, 0x80, 0x80, 0x80, 0x88, 0x94, 0x00, 0x6F, 0x02, 0x09,
															0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x0F, 0x00, 0xF0, 0x0F, 0x00, 0xF0, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x5A, 0x3C, 0x5A, 0x3C, 0x5A, 0x3C, 0x5A,
															0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x35, 0x00, 0x35, 0x00, 0x35, 0x00, 0x35, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
															0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0xAA, 0xAA, 0x00, 0x04, 0x00, 0x08,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x59, 0x3C, 0x59, 0x3C, 0x59, 0x3C, 0x59,
															0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10,
															0x31, 0x00, 0x31, 0x00, 0x31, 0x00, 0x31, 0x00, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
															0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x78, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
														  },
														  {	0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x32, 0x10, 0x00, 0x00, 0xFF, 0xFF,
															0x32, 0x10, 0x76, 0x54, 0xBA, 0x98, 0xFE, 0xDC, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
															0x00, 0x00, 0x00, 0x2D, 0x00, 0x5A, 0x00, 0x87, 0x24, 0x00, 0x24, 0x2D, 0x24, 0x5A, 0x24, 0x87,
															0x48, 0x00, 0x48, 0x2D, 0x48, 0x5A, 0x48, 0x87, 0x6C, 0x00, 0x6C, 0x2D, 0x6C, 0x5A, 0x6C, 0x87,
															0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D,
															0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1A, 0x19, 0x90, 0xB4, 0x00, 0xC0,
															0x00, 0x00, 0x00, 0x00, 0xC0, 0x10, 0x80, 0x80, 0x80, 0x80, 0x88, 0x94, 0x00, 0x6F, 0x00, 0x00,
															0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x0F, 0x00, 0xF0, 0x0F, 0x00, 0xF0, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x5A, 0x48, 0x5A, 0x48, 0x5A, 0x48, 0x5A,
															0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19,
															0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x58, 0x48, 0x58, 0x48, 0x58, 0x48, 0x58,
															0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
															0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x1A, 0x19, 0x1A, 0x19, 0x1A, 0x19, 0x1A, 0x19,
															0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x90, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	
														  }
		};
		USHORT R0005 = 0x0000;

		AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, 0x05, 0x32, 0x10 );

		R0005 = AH8400_GetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, 0x05 );

		if( R0005 == 0x3210 ) { 
			
			pDevice->m_nCustomHardwareTopologyProperty |= 0x00000010;

			is_ah8816_exist = TRUE; 
		}
		if( is_ah8816_exist ) {

			for( i = 0 ; i < 0x0180 ; i += 2 ) { 

				if( e_v4l2_std & V4L2_STD_525_60 ) {  AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(i >> 1), AM8816_REGISTER_TABLES[ 0 ][ i + 0 ], AM8816_REGISTER_TABLES[ 0 ][ i + 1 ] ); }

				if( e_v4l2_std & V4L2_STD_625_50 ) {  AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(i >> 1), AM8816_REGISTER_TABLES[ 1 ][ i + 0 ], AM8816_REGISTER_TABLES[ 1 ][ i + 1 ] ); }
			}
			for( i = 0 ; i < 4 ; i++ ) { 
		
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x7C + i), 0xFF, 0x00 );
		
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x80 + i), 0x00, 0x03 );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x50 + i), 0x84, 0x44 );

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0xBC + i), 0xFF, 0x00 );
		
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x84 + i), 0x00, 0x03 );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x90 + i), 0x84, 0x44 );
			}
			{	BYTE AM8816_REGISTER_TABLES_POS[ 0x02 ][ 0x0060 ] = { { 0x00, 0x00, 0x00, 0x2D, 0x00, 0x5A, 0x00, 0x87,
																		0x1E, 0x00, 0x1E, 0x2D, 0x1E, 0x5A, 0x1E, 0x87,
																		0x3C, 0x00, 0x3C, 0x2D, 0x3C, 0x5A, 0x3C, 0x87,
																		0x5A, 0x00, 0x5A, 0x2D, 0x5A, 0x5A, 0x5A, 0x87,
																		0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D,
																		0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D,
																		0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D,
																		0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D, 0x1E, 0x2D,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																	  },
																	  { 0x00, 0x00, 0x00, 0x2D, 0x00, 0x5A, 0x00, 0x87,
																		0x24, 0x00, 0x24, 0x2D, 0x24, 0x5A, 0x24, 0x87,
																		0x48, 0x00, 0x48, 0x2D, 0x48, 0x5A, 0x48, 0x87,
																		0x6C, 0x00, 0x6C, 0x2D, 0x6C, 0x5A, 0x6C, 0x87,
																		0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D,
																		0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D,
																		0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D,
																		0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D, 0x24, 0x2D,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																	  }
				};
				BYTE AM8816_REGISTER_TABLES_SEL[ 8 ] = { 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE };
		
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, 0x02, 0x00, 0x00 );

				for( i = 0 ; i < 16 ; i++ ) { 
		
					if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x10 + i), AM8816_REGISTER_TABLES_POS[ 0 ][ 0x00 + i * 2 + 0 ], AM8816_REGISTER_TABLES_POS[ 0 ][ 0x00 + i * 2 + 1 ] ); }
		
					if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x20 + i), AM8816_REGISTER_TABLES_POS[ 0 ][ 0x20 + i * 2 + 0 ], AM8816_REGISTER_TABLES_POS[ 0 ][ 0x20 + i * 2 + 1 ] ); }
		
					if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x30 + i), AM8816_REGISTER_TABLES_POS[ 0 ][ 0x40 + i * 2 + 0 ], AM8816_REGISTER_TABLES_POS[ 0 ][ 0x40 + i * 2 + 1 ] ); }
		
					if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x10 + i), AM8816_REGISTER_TABLES_POS[ 1 ][ 0x00 + i * 2 + 0 ], AM8816_REGISTER_TABLES_POS[ 1 ][ 0x00 + i * 2 + 1 ] ); }
		
					if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x20 + i), AM8816_REGISTER_TABLES_POS[ 1 ][ 0x20 + i * 2 + 0 ], AM8816_REGISTER_TABLES_POS[ 1 ][ 0x20 + i * 2 + 1 ] ); }
		
					if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x30 + i), AM8816_REGISTER_TABLES_POS[ 1 ][ 0x40 + i * 2 + 0 ], AM8816_REGISTER_TABLES_POS[ 1 ][ 0x40 + i * 2 + 1 ] ); }
				}
				for( i = 0 ; i < 4 ; i++ ) { 
		
					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x08 + i), AM8816_REGISTER_TABLES_SEL[ i * 2 + 1 ], AM8816_REGISTER_TABLES_SEL[ i * 2 + 0 ] );
		
					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x0C + i), 0x33, 0x33 );
				}
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, 0x02, 0xFF, 0xFF );
			}
			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF0, 0xFF, 0xFF ); 

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF2, 0x00, 0x00 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF3, 0xFF, 0xFF );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF4, 0x00, 0x00 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF5, 0xFF, 0xFF );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF6, 0xE0, 0xE0 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF7, 0xC0, 0x3C );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF8, 0x40, 0x40 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF9, 0x60, 0x00 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFA, 0x00, 0x00 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFB, 0x00, 0x00 );

			AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFE, 0x80, 0x81 );

			schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

			// [2009.09.07] SETUP MOTION DETECTION REGISTER
			// 
			{	AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFA, 0xFF, 0xFF ); //
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF0, 0xFF, 0xFF ); //
			//	AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF1, 0x00, 0x00 ); //
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF1, 0xFF, 0xFF ); //
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF2, 0x00, 0x00 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF3, 0xFF, 0xFF );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF4, 0x00, 0x00 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF5, 0xFF, 0xFF );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF6, 0x90, 0x90 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF7, 0x00, 0x7F );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF8, 0x60, 0x60 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF9, 0x60, 0x00 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFB, 0x00, 0x00 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFE, 0x80, 0x81 );
    
				schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x20, 0x03, 0x01 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x21, 0x03, 0xDE );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x22, 0x00, 0x70 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x26, 0x00, 0xC0 ); //
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x27, 0x00, 0x03 );
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x8A, 0x00, 0x00 );

				for ( i = 0; i < 24 ; i++ ) {

					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0328 + i * 0x02) >> 8, (0x0328 + i * 0x02) & 0xFF, 0x7F, 0xFF ); 
					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0329 + i * 0x02) >> 8, (0x0329 + i * 0x02) & 0xFF, 0x7F, 0xFF ); 
					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0390 + i * 0x02) >> 8, (0x0390 + i * 0x02) & 0xFF, 0x00, 0x00 ); 
					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0391 + i * 0x02) >> 8, (0x0391 + i * 0x02) & 0xFF, 0x00, 0x00 ); 
				}
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF0, 0x00, 0x00 ); // 
			}
			for( i = 0 ; i < 4 ; i++ ) { 

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x02, (BYTE)(0xD0 + i), 0x00, 0x60 + (BYTE)(4 * i + 0) );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x02, (BYTE)(0xD0 + i), 0x01, 0x60 + (BYTE)(4 * i + 1) );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x02, (BYTE)(0xD0 + i), 0x02, 0x60 + (BYTE)(4 * i + 2) );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x02, (BYTE)(0xD0 + i), 0x03, 0x60 + (BYTE)(4 * i + 3) );

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x7C + i), 0x03, 0x00 ); 

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x80 + i), 0x00, 0x03 );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x50 + i), 0x8C, 0x44 );

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x02, (BYTE)(0xE0 + i), 0x00, 0x00 );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x02, (BYTE)(0xE0 + i), 0x01, 0x40 );
				
				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0xBC + i), 0x01, 0x00 ); 

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x84 + i), 0x00, 0x09 ); 

				if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { 

					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x90 + i), 0x8C, 0x44 );
				}
				if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { 

					AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, (BYTE)(0x90 + i), 0x8C, 0x54 );
				}
			}
		}
	}	
	if( TRUE ) {

		BYTE CS495X_REGISTER_TABLES[ 0x02 ][ 0x34 ] = { { 0x0B, 0x12, 0x40, 0x00, 0x07, 0x20, 0x00, 0x00, 
														  0x03, 0x00, 0x00, 0x00, 0x00, 0x90, 0xF4, 0x18, 
														  0x1C, 0x3E, 0xF8, 0xE0, 0x43, 0x00, 0x00, 0x00, 
														  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
														  0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
														  0x00, 0xA1, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
														  0x00, 0x00, 0x00, 0x00,
														},
														{ 0x4B, 0x30, 0x40, 0x00, 0x07, 0x20, 0x00, 0x00, 
														  0x03, 0x00, 0x00, 0x00, 0x00, 0x90, 0xF4, 0x18,
														  0x1C, 0x96, 0x15, 0x13, 0x54, 0x00, 0x00, 0x00, 
														  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
														  0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
														  0x00, 0xA1, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
														  0x00, 0x00, 0x00, 0x00,
														}
		};
		AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x00, 0x0F, 0x18 );

		for( i = 0 ; i < 0x34 ; i++ ) {

			if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x30, (BYTE)(i), CS495X_REGISTER_TABLES[ 0 ][ i ] ); }

			if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_SetAnalogVideoDecoderRegister( pDevice, 0x30, (BYTE)(i), CS495X_REGISTER_TABLES[ 1 ][ i ] ); }
		}
	}
	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) {

		BYTE NVP1114MX_REGISTER_TABLES_DECODER[ 0x02 ][ 128 ] = { { 0x40, 0x00, 0xA7, 0x02, 0x8C, 0x00, 0x00, 0x00, 0x06, 0x2F, 0x92, 0x02, 0x00, 0x44, 0x10, 0x00,
																	0x40, 0x00, 0xA7, 0x02, 0x8C, 0x00, 0x00, 0x00, 0x06, 0x2F, 0x92, 0x02, 0x00, 0x44, 0x10, 0x11,
																	0x40, 0x00, 0xA7, 0x02, 0x8C, 0x00, 0x00, 0x00, 0x06, 0x2F, 0x92, 0x02, 0x00, 0x44, 0x10, 0x22,
																	0x40, 0x00, 0xA7, 0x02, 0x8C, 0x00, 0x00, 0x00, 0x06, 0x2F, 0x92, 0x02, 0x00, 0x44, 0x10, 0x33,
																	0xD0, 0x80, 0x40, 0x7C, 0x9F, 0x00, 0x20, 0x40, 0x80, 0x50, 0x38, 0x0F, 0x0C, 0x01, 0x15, 0x0A,
																	0x80, 0x23, 0x88, 0x04, 0x2A, 0xCC, 0xE0, 0x26, 0x57, 0x43, 0x3F, 0x88, 0x82, 0x63, 0x01, 0x00,
																	0x71, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x2E, 0x00, 0x30, 0xB8, 0x01,
																	0x06, 0x06, 0x11, 0xB9, 0xB2, 0x05, 0x00, 0x28, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																  },
																  { 0x9D, 0x00, 0xBE, 0x00, 0x7E, 0x00, 0x07, 0x00, 0x07, 0x2F, 0xA8, 0x03, 0x00, 0x48, 0x10, 0x00,
																	0x9D, 0x00, 0xBE, 0x00, 0x7E, 0x00, 0x07, 0x00, 0x07, 0x2F, 0xA8, 0x03, 0x00, 0x48, 0x10, 0x11,
																	0x9D, 0x00, 0xBE, 0x00, 0x7E, 0x00, 0x07, 0x00, 0x07, 0x2F, 0xA8, 0x03, 0x00, 0x48, 0x10, 0x22,
																	0x9D, 0x00, 0xBE, 0x00, 0x7E, 0x00, 0x07, 0x00, 0x07, 0x2F, 0xA8, 0x03, 0x00, 0x48, 0x10, 0x33,
																	0xD0, 0x80, 0x40, 0x7C, 0x9F, 0x00, 0x20, 0x40, 0x80, 0x50, 0x08, 0x0F, 0x0C, 0x01, 0x15, 0x0A,
																	0x89, 0x01, 0x88, 0x04, 0x2A, 0xCC, 0xE0, 0x06, 0x57, 0x43, 0x3F, 0x88, 0x01, 0x63, 0x01, 0x00,
																	0x70, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04, 0x2E, 0x00, 0x30, 0xD8, 0x01,
																	0x06, 0x06, 0x11, 0xB9, 0xB2, 0x05, 0x00, 0x28, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
																  }
		};

		const BYTE NVP1114MXA_R1_NTSC_DEC_BUF[] = {

					// 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F
					   0x00,0x00,0x00,0x00,0x00,0x10,0x32,0x10,0x32,0x10,0x32,0x10,0x32,0x00,0x00,0x00,
			  /*1*/    0x00,0xEF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			  /*2*/    0x60,0x00,0xA7,0x02,0x9F,0x00,0x00,0x00,0x06,0x10,0x96,0x01,0x00,0x8A,0x10,0x00,
			  /*3*/    0x60,0x00,0xA7,0x02,0x9F,0x00,0x00,0x00,0x06,0x10,0x96,0x01,0x00,0x8A,0x10,0x11,
			  /*4*/    0x60,0x00,0xA7,0x02,0x9F,0x00,0x00,0x00,0x06,0x10,0x96,0x01,0x00,0x8A,0x10,0x22,
			  /*5*/    0x60,0x00,0xA7,0x02,0x9F,0x00,0x00,0x00,0x06,0x10,0x96,0x01,0x00,0x8A,0x10,0x33,
			  /*6*/    0xD0,0x80,0x40,0x7C,0x9F,0x00,0x20,0x40,0x80,0x50,0x38,0x0F,0x0C,0x01,0x15,0x0A,
			  /*7*/    0x80,0x23,0x88,0x04,0x2A,0xCC,0xF0,0x2F,0x57,0x43,0x10,0x88,0x82,0x63,0x01,0x00,
			  /*8*/    0x80,0x00,0x00,0xE1,0x01,0x00,0x00,0x00,0x00,0x20,0x04,0x2E,0x00,0x30,0xB8,0x01,
			  /*9*/    0x06,0x06,0x11,0xB9,0xB2,0x05,0x00,0x28,0x50,0x51,0xB5,0x13,0x03,0x22,0xFF,0x00,
			  /*A*/    0x00,0x00,0x22,0x88,0x88,0x84,0x33,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x04,
			  /*B*/    0x00,0x88,0x88,0x88,0x14,0x0F,0xAA,0xAA,0x02,0x00,0x80,0x00,0x00,0xC9,0x0F,0x18,
			  /*C*/    0x13,0x13,0x13,0x13,0x00,0x00,0x71,0x71,0x71,0x71,0x1C,0x1C,0x1C,0x1C,0x87,0x87,
			  /*D*/    0x87,0x87,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x10,0x80,0x10,0x00,0x00,
			  /*E*/    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x11,0x01,0x40,0x00,0x00,0x00,0x00,
			  /*F*/    0x00,0x00,0xA0,0x04,0x00,0x00,0x80,0x58,0x69,0x20,0x0F,0x80,0x80,0x49,0x37,0x00

		};
		const BYTE NVP1114MXA_R1_PAL_DEC_BUF[] = {

					// 0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F
					   0x00,0x00,0x00,0x00,0x00,0x10,0x32,0x10,0x32,0x10,0x32,0x10,0x32,0x00,0x00,0x00,
			  /*1*/    0x00,0xEF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			  /*2*/    0xBD,0x00,0xBC,0x00,0x7E,0x00,0x00,0x00,0x07,0x10,0xC4,0x01,0x00,0x0F,0x04,0x00,
			  /*3*/    0xBD,0x00,0xBC,0x00,0x7E,0x00,0x00,0x00,0x07,0x10,0xC4,0x01,0x00,0x0F,0x04,0x11,
			  /*4*/    0xBD,0x00,0xBC,0x00,0x7E,0x00,0x00,0x00,0x07,0x10,0xC4,0x01,0x00,0x0F,0x04,0x22,
			  /*5*/    0xBD,0x00,0xBC,0x00,0x7E,0x00,0x00,0x00,0x07,0x10,0xC4,0x01,0x00,0x0F,0x04,0x33,
			  /*6*/    0xD0,0x80,0x40,0x7C,0x9F,0x00,0x20,0x40,0x80,0x50,0x38,0x0F,0x0C,0x01,0x15,0x0A,
			  /*7*/    0x89,0x23,0x88,0x04,0x2A,0xCC,0xA0,0x2F,0x57,0x43,0x3F,0x88,0x01,0x63,0x01,0x00,
			  /*8*/    0x80,0x00,0x00,0xE1,0x00,0x00,0x00,0x00,0x00,0x20,0x04,0x2E,0x00,0x30,0xD8,0x01,
			  /*9*/    0x06,0x06,0x11,0xB9,0xB2,0x05,0x00,0x28,0x50,0x01,0xA8,0x13,0x03,0x22,0x00,0x00,
			  /*A*/    0x00,0x00,0x22,0x88,0x88,0x84,0x33,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x04,
			  /*B*/    0x00,0x88,0x88,0x88,0x14,0x0F,0xAA,0xAA,0x02,0x00,0x80,0x00,0x00,0xC9,0x0F,0x18,
			  /*C*/    0x13,0x13,0x13,0x13,0x00,0x00,0x71,0x71,0x71,0x71,0x1C,0x1C,0x1C,0x1C,0x87,0x87,
			  /*D*/    0x87,0x87,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x10,0x80,0x10,0x00,0x00,
			  /*E*/    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x10,0x24,0x01,0x4B,0x00,0x00,0x00,0x00,
			  /*F*/    0x00,0x00,0xA0,0x04,0x00,0x00,0x80,0x46,0x7B,0x20,0x0F,0x80,0x80,0x49,0x37,0x00
		};

		BYTE R0701 = 0x00;

		BYTE R0708 = 0x00;

		BYTE R0709 = 0x00;

		BYTE R0045 = 0x00;

		BYTE NVP1114MX_REGISTER_TABLES_ENCODER[ 0x02 ][ 9 ] = { { 0x00, 0x08, 0x00, 0x00, 0x86, 0x7A, 0x7E, 0x7A, 0x00 }, 

															    { 0x0D, 0x08, 0x00, 0x01, 0x90, 0x70, 0x7E, 0x89, 0x00 }
		};

		if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000040 )
		{
			LINUXV4L2_DEBUG( KERN_INFO, "NVP1114MXA\n" );

			int i = 0;
			for( i = 0x00 ; i < 0x100 ; i++ ) {

				if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(i), NVP1114MXA_R1_NTSC_DEC_BUF[ i ], 10 ); }

				if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(i), NVP1114MXA_R1_PAL_DEC_BUF[ i ], 10 ); }
			}
		}
		else
		{
			LINUXV4L2_DEBUG( KERN_INFO, "NVP1114MX\n" );

			for( i = 0x20 ; i < 0xA0 ; i++ ) {

				if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(i), NVP1114MX_REGISTER_TABLES_DECODER[ 0 ][ i - 0x20 ], 10 ); }

				if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x0F, (BYTE)(i), NVP1114MX_REGISTER_TABLES_DECODER[ 1 ][ i - 0x20 ], 10 ); }
			}
		}
		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x03, 0x00, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x03, 0x10, 0x00, 10 );

		schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );

		for( i = 0x00 ; i < 0x09 ; i++ ) {

			if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x03, (BYTE)(i + 0x00), NVP1114MX_REGISTER_TABLES_ENCODER[ 0 ][ i ], 10 ); }

			if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x03, (BYTE)(i + 0x10), NVP1114MX_REGISTER_TABLES_ENCODER[ 0 ][ i ], 10 ); }

			if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x03, (BYTE)(i + 0x00), NVP1114MX_REGISTER_TABLES_ENCODER[ 1 ][ i ], 10 ); }

			if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x03, (BYTE)(i + 0x10), NVP1114MX_REGISTER_TABLES_ENCODER[ 1 ][ i ], 10 ); }
		}
		if( e_v4l2_std & V4L2_STD_525_60 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x04, 0x27, 10 ); }

		if( e_v4l2_std & V4L2_STD_625_50 ) { AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x04, 0xA7, 10 ); }

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x06, 0x20, 10 );

		R0701 = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x07, 0x01, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x01, R0701 & 0x1F, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x00, 0x11, 10 );

		R0708 = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x07, 0x08, 10 );

		if( R0708 & (1 << 6) ) {

			R0708 &= 0x7F;
		}
		else {

			R0708 |= 0x80;
		}
		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x08, R0708, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x08, R0708 | 0x01, 10 );

		R0709 = 0x00;

		for( i = 0 ; i < 100 ; i++ ) {

			R0709 = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x07, 0x09, 10 );

			if( R0709 & 0x01 ) { break ; }

			schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );
		}
		if( (R0709 & 0x01) == 0x00 ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] R0709 ... LOSS (3)!!\n", pDevice->m_nKsDeviceNumber ); }

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x0A, 0x80, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x0A, 0x00, 10 );

		AH8400_HPI_SET_RECORD_TABLE( pDevice, 0x00, 0x2C, 10 );

		AH8400_HPI_SET_RECORD_TABLE( pDevice, 0x00, 0x3C, 10 );

		AH8400_HPI_SET_RECORD_TABLE( pDevice, 0x00, 0x0C, 10 );

		AH8400_HPI_SET_RECORD_TABLE( pDevice, 0x00, 0x1C, 10 );

		if( R0708 & (1 << 7) ) {

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x07, 0x00, 10 );

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x06, 0x04, 10 );
		}
		else {

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x05, 0x00, 10 );

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x04, 0x04, 10 );
		}
		R0708 = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x07, 0x08, 10 );

		if( R0708 & (1 << 6) ) {

			R0708 &= 0xBF;
		}
		else {

			R0708 |= 0x40;
		}
		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x08, R0708, 10 ); R0708 &= ~0x01;

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x08, R0708, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x08, R0708 & 0x10, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x08, R0708, 10 );

		R0045 = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x45, 10 );
		
		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x45, R0045 | 0x08, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x4B, 0x65, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x00, 0x12, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x00, 0x13, 10 );

		if( e_v4l2_std & V4L2_STD_525_60 ) { 

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x6A, 0x00, 10 );

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x6B, 0x00, 10 );
		}
		if( e_v4l2_std & V4L2_STD_625_50) { 

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x6A, 0x22, 10 );

			AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x07, 0x6B, 0x22, 10 );
		}
		is_nvp1114mx_exist = TRUE;
	}
	if( e_v4l2_std & V4L2_STD_525_60 ) { 

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000614, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000014, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000114, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000214, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000314, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000010, 0x0000000C, 4 );
	}
	if( e_v4l2_std & V4L2_STD_625_50 ) { 
		
		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000614, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000014, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000114, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000214, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000314, 0x000101FF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000010, 0x0000000E, 4 ); 
	}
	return TRUE;
}

BOOLEAN AH8400_HwAnalogComponentsInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG i = 0;

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 0 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}
	else { 

		pDevice->m_nAnalogCaptureStreamPowerReference++;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] AH8400_HwAnalogComponentsInitialize( %02ld )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCaptureStreamPowerReference );

	{	if( g_copy_protect_unlock_boradsA[ 0 ] == 0 ) { return TRUE; }
	}
	{	if( g_copy_protect_unlock_boradsB[ 0 ] == 0 ) { return TRUE; }
	}

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) {

		pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] = 0;

		pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] = 0;

		pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] = 0;

		pDevice->m_nDmaBaseCommonBufferNumber[ 3 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0;
	}
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) {

		if( pVideo ) {

			ULONG i = pVideo->m_pDevice->m_nKsDeviceNumber;

			if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000010 ) {

//dump2
				USHORT R0020 = AH8400_GetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x00, 0x20 );

				if( (R0020 != 0x1E2D) && (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60) ) { AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, pVideo ); }

				if( (R0020 != 0x242D) && (pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50) ) { AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, pVideo ); }
			}
			else if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000040 )// NVP1114MXA
			{
				AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, pVideo );
			}
			else {

				AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, pVideo );

				AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, pVideo );
			}
			for( i = 0 ; i < 4 ; i++ ) {

				g_n_analog_decoder_brightness[ pVideo->m_pDevice->m_nKsDeviceNumber * 4 + i ][ 0 ] |= 0x80000000;

				g_n_analog_decoder_contrast[ pVideo->m_pDevice->m_nKsDeviceNumber * 4 + i ][ 0 ] |= 0x80000000;

				g_n_analog_decoder_saturation[ pVideo->m_pDevice->m_nKsDeviceNumber * 4 + i ][ 0 ] |= 0x80000000;

				g_n_analog_decoder_hue[ pVideo->m_pDevice->m_nKsDeviceNumber * 4 + i ][ 0 ] |= 0x80000000;

				g_n_analog_decoder_sharpness[ pVideo->m_pDevice->m_nKsDeviceNumber * 4 + i ][ 0 ] |= 0x80000000;

				AH8400_SetAnalogVideoDecoderProcAmpProperties( pDevice, i );
			}
		}
	}
	//
	// 
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x00000000, 4 );

		AH8400_ENTER_PCI_LIVE_DMA_ACCESS();

		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x000000FF, 4 );

		AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();
	}
	// INITIALIZE LIVE.DMA -----> StartLiveDMA() [2009.09.04]
	// 
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		ULONG R007D8008 = 0x00000000;

		ULONG R00760038 = 0x00000000;

		R007D8008 = AH8400_GetRegister( pDevice, 3, 0x007D8000 + 0x00000008, 4 ); R007D8008 |= 0x00000200;

		AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x00000008, R007D8008, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 |= 0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 &= ~0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000004, 0x02800000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000008, 0x02800000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000000, 0x002D0000, 4 );
		
		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x0000000C, 0x000002D0, 4 );

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000024, 0x00002CF0, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000090, 0x000000F0, 4 );
		}
		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000024, 0x00002D20, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000090, 0x00000120, 4 );
		}
		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000030, 0x000007D8, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, 0x00CF0003, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x0000003C, 0x08080014, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F4, 0x00020001, 4 );
	}
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) {

		for( i = 0 ; i < 4 ; i++ ) {

			AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000000, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000000, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000110, 4 );
				
			AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038 + (i * 0x00000100), 0x0000FBE1, 4 );
		}
		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000200, 0x0055FFFF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000214, 0x00000000, 4 ); //

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000218, 0x00000001, 4 ); //

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000224, 0x00001C1C, 4 ); //

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000228, 0x10001E00, 4 ); //

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000220, 0x0000001E, 4 ); //
	}
	//
	//

	if( pDevice->m_nAnalogCaptureStreamPowerReference >= 1 ) { 
		
		ULONG R00000214 = 0x00000000;

		ULONG R00000218 = 0x00000000;

		ULONG R00000224 = 0x00000000;

		ULONG R00000228 = 0x00000000;

		ULONG R00000220 = 0x00000000;

		for( i = 0 ; i < 4 ; i++ ) {

			if( pVideo == NULL ) { continue ; }

			if( pVideo->m_nSubDeviceNumber != i ) { continue ; }

			if( pVideo->m_nSubDeviceType != 0x00000002 ) { continue ; }

			if( pVideo->m_nSrcFrameWidth  == 720 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 704 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000001, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 640 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 360 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 352 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 320 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018 + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 720 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 704 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000001, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 640 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 360 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 352 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000001, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 320 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C + (i * 0x00000100), 0x00000000, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 720 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000110, 4 ); }
			
			if( pVideo->m_nSrcFrameWidth  == 704 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000110, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 640 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000118, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 360 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000114, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 352 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000114, 4 ); }

			if( pVideo->m_nSrcFrameWidth  == 320 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x0000011C, 4 ); }

			if( pVideo->m_nSrcFrameHeight == 480 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038 + (i * 0x00000100), 0x0000FBE1, 4 ); }

			if( pVideo->m_nSrcFrameHeight == 576 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038 + (i * 0x00000100), 0x0000FBE1, 4 ); }

			if( pVideo->m_nSrcFrameHeight == 240 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038 + (i * 0x00000100), 0x0000FBE3, 4 ); }

			if( pVideo->m_nSrcFrameHeight == 288 ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038 + (i * 0x00000100), 0x0000FBE3, 4 ); }

			if( pVideo->m_nFrameRate >= 60000 ) { R00000214 = 0x00000000; } else //

			if( pVideo->m_nFrameRate >= 50000 ) { R00000214 = 0x00000000; } else //B
			
			if( pVideo->m_nFrameRate >= 30000 ) { R00000214 = 0x00000000; } else //
			
			if( pVideo->m_nFrameRate >= 25000 ) { R00000214 = 0x00000000; } else //B
			
			if( pVideo->m_nFrameRate >= 15000 ) { R00000214 = 0x00000001; } else //
			
			if( pVideo->m_nFrameRate >= 12500 ) { R00000214 = 0x00000001; } else //B
			
			if( pVideo->m_nFrameRate >=  7500 ) { R00000214 = 0x00000003; } else //
			
			if( pVideo->m_nFrameRate >=  6250 ) { R00000214 = 0x00000003; } else //B
			
			if( pVideo->m_nFrameRate >=  3750 ) { R00000214 = 0x00000007; } else //
			
			if( pVideo->m_nFrameRate >=  3125 ) { R00000214 = 0x00000007; } else //B 

												{ R00000214 = 0x00000000; } 

			//
			// 
			{	ULONG CBRBPS = pVideo->m_nEncoderBitRate;

				ULONG RCTYPE = 0;

				if( pVideo->m_nEncoderMode == V4L2_BITRATE_VBR ) { RCTYPE = 0x00000000; }

				if( pVideo->m_nEncoderMode == V4L2_BITRATE_CBR ) { RCTYPE = 0x00000001; }

				CBRBPS /= 1024;

				if( CBRBPS == 0x00000000 ) { CBRBPS = 0x00000000; }

				if( CBRBPS >= 0x0000FFFF ) { CBRBPS = 0x0000FFFF; }

				R00000228 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000228, 4 ) & 0x0000FFFC;

				R00000228 |= (CBRBPS << 16);

				R00000228 |= (RCTYPE <<  0);
			
				R00000224 = pVideo->m_nEncoderQuality;

				R00000220 = pVideo->m_nEncoderGop;

				R00000218 = pVideo->m_nEncoderQPStep;
			}
			if( R00000224 <=     0 ) { R00000224 = 0x00003333; } else

			if( R00000224 <=   500 ) { R00000224 = 0x00003131; } else 

			if( R00000224 <=  1000 ) { R00000224 = 0x00002F2F; } else

			if( R00000224 <=  1500 ) { R00000224 = 0x00002D2D; } else

			if( R00000224 <=  2000 ) { R00000224 = 0x00002B2B; } else

			if( R00000224 <=  2500 ) { R00000224 = 0x00002929; } else

			if( R00000224 <=  3000 ) { R00000224 = 0x00002727; } else 

			if( R00000224 <=  3500 ) { R00000224 = 0x00002525; } else

			if( R00000224 <=  4000 ) { R00000224 = 0x00002323; } else

			if( R00000224 <=  4500 ) { R00000224 = 0x00002121; } else

			if( R00000224 <=  5000 ) { R00000224 = 0x00001F1F; } else

			if( R00000224 <=  5500 ) { R00000224 = 0x00001D1D; } else 

			if( R00000224 <=  6000 ) { R00000224 = 0x00001B1B; } else

			if( R00000224 <=  6500 ) { R00000224 = 0x00001919; } else

			if( R00000224 <=  7000 ) { R00000224 = 0x00001616; } else 

			if( R00000224 <=  7500 ) { R00000224 = 0x00001313; } else

			if( R00000224 <=  8000 ) { R00000224 = 0x00001010; } else

			if( R00000224 <=  8500 ) { R00000224 = 0x00000C0C; } else

			if( R00000224 <=  9000 ) { R00000224 = 0x00000808; } else

			if( R00000224 <=  9500 ) { R00000224 = 0x00000404; } else

			if( R00000224 <= 10000 ) { R00000224 = 0x00000000; }

			if( R00000220 ==     0 ) { R00000220 = 0x00000001; }
		
			if( R00000220 >=   255 ) { R00000220 = 0x000000FF; }
	
			if( R00000218 ==     0 ) { R00000218 = 0x00000001; }
		
			if( R00000218 >=   255 ) { R00000218 = 0x000000FF; }

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000200, (0x00010000 << (i * 2)) | (0x00000001 << (i * 1)), 4 );

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000214, R00000214, 4 ); //

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000218, R00000218, 4 ); //

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000224, R00000224, 4 ); //

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000228, R00000228, 4 ); //

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000220, R00000220, 4 ); //
		}
	}
	//
	//
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		ULONG R007D8008 = 0x00000000;

		for( i = 0 ; i < 4 ; i++ ) {

			ULONG R00780300 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000300 + (i * 0x00000040), 4 ); R00780300 &= (~0x00000003);

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000300 + (i * 0x00000040), R00780300, 4 );
		}
		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000100, 0x01555500, 4 );

		for( i = 0 ; i < 4 ; i++ ) {
		
			ULONG AH8400_REGISTER_TABLES_BITSTREAM_START_ADDRESS[ 4 ] = { 0x01723200, 0x01B23200, 0x01723200, 0x01B23200 };

			ULONG AH8400_REGISTER_TABLES_BITSTREAM_END_ADDRESS[ 4 ] = { 0x01B23200, 0x01F23200, 0x01B23200, 0x01F23200 };

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 0x00008003, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000118 + (i * 0x00000020), AH8400_REGISTER_TABLES_BITSTREAM_START_ADDRESS[ i ], 4 );

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x0000011C + (i * 0x00000020), AH8400_REGISTER_TABLES_BITSTREAM_END_ADDRESS[ i ], 4 );
		}
		R007D8008 = AH8400_GetRegister( pDevice, 3, 0x007D8000 + 0x00000008, 4 ); R007D8008 |= 0x00010000;

		AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x00000008, R007D8008, 4 );
	}
	// -----> ADPCMStart() [2009.09.04]
	//
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		{
			memset( CG721, 0x00, 16 * sizeof(struct CG721Coder) );
		}

		for(i = 0; i < 16; i++)
		{
			Init( i );
		}

		ULONG R007D8008 = 0x00000000;

		AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x0000010C, 0x00000080, 4 );
		
		AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000004, 0x8000FFFF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000090, 0x0000FFFF, 4 );

		R007D8008 = AH8400_GetRegister( pDevice, 3, 0x007D8000 + 0x00000008, 4 ); R007D8008 |= 0x00020000;

		AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x00000008, R007D8008, 4 );
	}
	//
	// 
	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		ULONG R007D8008 = AH8400_GetRegister( pDevice, 3, 0x007D8000 + 0x00000008, 4 ); R007D8008 |= 0x00000100;

		AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x00000008, R007D8008, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000000, 0x80000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000014, 0x08180000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000024, 0x08180000, 4 );
	}
	// -----> ADPCMStart() [2009.09.04]
	//

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) { 

		pDevice->m_hInterruptAccessLock = FALSE;

		//

		// ...
		// 
		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x000000FF, 4 );

		//

		//
		//
		pDevice->m_hInterruptAccessLock = TRUE;
	}
	return TRUE;
}

BOOLEAN AH8400_HwAnalogComponentsUnInitialize( CDevice * pDevice, CVideo * pVideo, CAudio * pAudio )
{
	ULONG i = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] AH8400_HwAnalogComponentsUnInitialize( %02ld )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCaptureStreamPowerReference );

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
	// UNINITIALIZE LIVE.DMA -----> LiveDMAStop() [2009.09.04]
	// 
	{	ULONG R000002A4 = 0x00000000;

		ULONG R00000140 = 0x00000000;

		ULONG R00760038 = 0x00000000;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F4, 0x00020001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, 0x00CF0000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000030, 0x00000000, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 |= 0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 &= ~0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F4, 0x00000000, 4 );

		AH8400_ENTER_PCI_LIVE_DMA_ACCESS();

		R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 &= ~(0x000009C9); R000002A4 &= ~(0x00004000);

		AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000001);

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000004;

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000008;

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x00002000;

		AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

		AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();
	}
	// UNINITIALIZE H.264 CODEC -----> StopCodec() [2009.09.04]
	// 
	{	AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x0000000C, 0x00010000, 4 );

		for( i = 0 ; i < 4 ; i++ ) {

			ULONG R00780300 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000300 + (i * 0x00000040), 4 ); R00780300 |= 0x00000003;

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000300 + (i * 0x00000040), R00780300, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 0x00000000, 4 );
		}
		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000100, 0x00000000, 4 );
	}
	// UNINITIALIZE G.721 CODEC -----> ADPCMStop() [2009.09.04]
	//
	{	AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x0000000C, 0x00020000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000004, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000090, 0x00000000, 4 );
	}
	// UNINITIALIZE HDMA -----> StopHDMA() [2009.09.04]
	//
	{	AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x0000000C, 0x00000100, 4 );
	
		AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000000, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000014, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000024, 0x00000000, 4 );
	}
	//
	// 
	{	ULONG R000002A4 = 0x00000000;

		ULONG R00000140 = 0x00000000;

		R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 &= ~(0x000009C9); R000002A4 &= ~(0x00004000);

		AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000001);

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000004;

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000008;

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x00002000;

		AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );
	}
	{	pDevice->m_hInterruptAccessLock = FALSE;

		// ...

		// ...
		//
		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x00000000, 4 );

		pDevice->m_hInterruptAccessLock = FALSE;
	}
	return TRUE;

}

BOOLEAN NVP1114MX_HwInitialize( CDevice * pDevice )
{	
	ULONG R007D0400, R007D1400, R007D2400, R007D23FC, i;
	
	R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x000000FF;

	AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

	R007D1400 = AH8400_GetRegister( pDevice, 3, 0x007D1000 + 0x00000400, 4 ); R007D1400 |= 0x000000FF;

	AH8400_SetRegister( pDevice, 3, 0x007D1000 + 0x00000400, R007D1400, 4 );

	R007D2400 = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x00000400, 4 ); R007D2400 |= 0x00000002;

	AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x00000400, R007D2400, 4 );

	////////////////////////////////////////////////////////////////////////

	R007D23FC = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, R007D23FC & 0xFFFFFFFD, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

	AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, R007D23FC | 0x00000002, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

	if( AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x00, 10 ) == 0x24 && 
		
		AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x01, 10 ) == 0x13 && 
		
		AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x02, 10 ) == 0x14 && 
		
		AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x03, 10 ) == 0x11 ) {
		
		pDevice->m_nCustomHardwareTopologyProperty |= 0x00000020;

		if( AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x1C, 10 ) & 0x20 ) {

			pDevice->m_nCustomHardwareTopologyProperty |= 0x00000040; // NVP1114MXA
		}

	}
	LINUXV4L2_DEBUG( KERN_INFO, "m_nCustomHardwareTopologyProperty(0x%x)\n", pDevice->m_nCustomHardwareTopologyProperty);

	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) {

		BYTE R001C = 0x00;

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x17, 0x0E, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x18, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x19, 0x20, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x1A, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x1B, 0x00, 10 );

		for( i = 0 ; i < 100 ; i++ ) {

			R001C = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x1C, 10 );
			
			if( R001C & 0x01 ) { break ; }

			schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );
		}
		if( (R001C & 0x01) == 0x00 ) { LINUXV4L2_DEBUG( KERN_INFO, "R001C ... LOSS (1)!!\n" ); }	

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x0C, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x0D, 0x31, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x0E, 0x2A, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x0F, 0xAA, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA1, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA2, 0xDE, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA5, 0x1C, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA6, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA7, 0x38, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA8, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA0, 0x23, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0x9F, 0x00, 10 );

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x08, 0xA0, 0x20, 10 );

		/////////////////////////////////////////////////////////////

		AH8400_HPI_WRITE_BYTE( pDevice, 0x00, 0x00, 0x10, 0x01, 10 );

		for( i = 0 ; i < 100 ; i++ ) {

			R001C = AH8400_HPI_READ_BYTE( pDevice, 0x00, 0x00, 0x1C, 10 );

			if( R001C & 0x01 ) { break ; }

			schedule_timeout_interruptible( msecs_to_jiffies( 10 ) );
		}
		if( (R001C & 0x01) == 0x00 ) { LINUXV4L2_DEBUG( KERN_INFO, "R001C ... LOSS (2)!!\n" ); }
	}
	return TRUE;
}

BOOLEAN AH8400_HwInitialize( CDevice * pDevice )
{
	ULONG cx = 720;

	ULONG cy = 480;

	ULONG e_v4l2_std = pDevice->m_nCustomVideoStandardProperty;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] AH8400_HwInitialize()\n", pDevice->m_nKsDeviceNumber );
	
	g_pDevice[ pDevice->m_nKsDeviceNumber ] = pDevice;

	//
	//
//	AH8400_SetRegister( pDevice, 2, 0x00000280, 0x00000000, 4 );

	AH8400_SetRegister( pDevice, 2, 0x00000284, 0x7F800000, 4 );

	//

	//
	// 
	AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F0, 0x0FF80003, 4 );

	AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000058, 0x00002A48, 4 );

	{ DWORD CFG4 = 0x02000007; pci_write_config_dword( pDevice->m_pKsDevice, 0x00000004, CFG4 ); }

	{ DWORD CFGC = 0x00004000; pci_write_config_dword( pDevice->m_pKsDevice, 0x0000000C, CFGC ); }

	AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000058, 0x09000000, 4 ); 
	
	schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

	AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F0, 0x0FF80010, 4 );

	//
	// 
	NVP1114MX_HwInitialize( pDevice );

	{	//
		// 
		ULONG R007D0400 = 0x00000000;

		ULONG R007D03FC = 0x00000000;

		R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x00000002;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

		R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= ~0x00000002;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC, 4 ); R007D03FC |= 0x00000002; schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

		//
		// 
		R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x00000001;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

		R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= ~0x00000001;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC, 4 ); R007D03FC |= 0x00000001; schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );
		
		//
		// 
		R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x00000004;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

		R007D03FC = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, 4 ); R007D03FC &= ~0x00000004;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC, 4 ); R007D03FC |= 0x00000004; schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x000003FC, R007D03FC, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

		schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	}
	//
	// 
	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000008, 0x00000000, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000000, 0x00000041, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000004, 0x00000000, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007D5000 + 0x00000008, 0x00000080, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );

	//
	// 
	//
	// 
	//
	// 	
	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000040 )// NVP1114MXA
	{
		AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, NULL );
	}
	else
	{
		AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, NULL );
		//added according to windows driver
		AH8400_SetAnalogVideoDecoderStandardProperty( pDevice, NULL );
	}

	//
	// 
	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000001 ) { cx = 720; }	//

	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000010 ) { cx = 720; }	//

	if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) { cx = 720; }	//
			
	if( e_v4l2_std & V4L2_STD_525_60 ) { cy = 480; }

	if( e_v4l2_std & V4L2_STD_625_50 ) { cy = 576; }

	if( cx == 720 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000610, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000618, 0x00000000, 4 );
	}
	if( cx == 704 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000610, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000618, 0x00000001, 4 );
	}
	if( cx == 640 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000610, 0x00000008, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000618, 0x00000000, 4 );
	}
	AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000060C, 0x00000000, 4 );

//	if( cx == 720 ) { AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000020, 0x00168168, 4 ); } //

//	if( cx == 704 ) { AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000020, 0x00160160, 4 ); } //

//	if( cx == 640 ) { AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000020, 0x00140140, 4 ); } //

	if( cx == 720 ) { AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000020, 0x00000168, 4 ); } //

	if( cx == 704 ) { AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000020, 0x00000160, 4 ); } //

	if( cx == 640 ) { AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000020, 0x00000140, 4 ); } //

	{	AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000000C, 0x06060F05, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000010C, 0x06061F05, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000020C, 0x06062F05, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000030C, 0x06063F05, 4 );
	}
	if( cx == 720 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000118, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000011C, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000218, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000021C, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000318, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000031C, 0x00000001, 4 );
	}
	if( cx == 704 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000118, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000011C, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000218, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000021C, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000318, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000031C, 0x00000001, 4 );
	}
	if( cx == 640 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000018, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000001C, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000118, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000011C, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000218, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000021C, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000318, 0x00000000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000031C, 0x00000000, 4 );
	}
	if( cx == 720 ) { ULONG i = 0; for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000110, 4 ); } }

	if( cx == 704 ) { ULONG i = 0; for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000110, 4 ); } }

	if( cx == 640 ) { ULONG i = 0; for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000118, 4 ); } }

	if( cx == 360 ) { ULONG i = 0; for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000114, 4 ); } }

	if( cx == 352 ) { ULONG i = 0; for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x00000114, 4 ); } }

	if( cx == 320 ) { ULONG i = 0; for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000010 + (i * 0x00000100), 0x0000011C, 4 ); } }

	if( cy == 480 || cy == 576 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038, 0x0000FBE1, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000138, 0x0000FBE1, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000238, 0x0000FBE1, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000338, 0x0000FBE1, 4 );
	}
	if( cy == 240 || cy == 288 ) {

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000038, 0x0000FBE3, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000138, 0x0000FBE3, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000238, 0x0000FBE3, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000338, 0x0000FBE3, 4 );
	}
	if( cy == 480 || cy == 240 ) { //

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000614, 0x000100F0, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000014, 0x000100F0, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000114, 0x000100F0, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000214, 0x000100F0, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000314, 0x000100F0, 4 );
	}
	if( cy == 576 || cy == 288 ) { //

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000614, 0x00010120, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000014, 0x00010120, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000114, 0x00010120, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000214, 0x00010120, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000314, 0x00010120, 4 );
	}
	AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000730, 0x83178AF0, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000734, 0x83178AF0, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x00000738, 0x83178AF0, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x00790000 + 0x0000073C, 0x83178AF0, 4 ); //

	//
	// 
	{	ULONG R00780018 = 0x00000000;

		ULONG R007D600C = 0x00000000;

	//	AH8400_SetRegister( pDevice, 2, 0x00000104, 0x000000FF, 4 ); //

		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x00000000, 4 ); //

		AH8400_ENTER_PCI_LIVE_DMA_ACCESS();

		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x000000FF, 4 ); //

		AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();

		//
		// 
		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000020, 0x00000001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000010, 0x00000001, 4 ); schedule_timeout_interruptible( msecs_to_jiffies( 30 ) );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000010, 0x00000000, 4 );

		if( cy == 480 || cy == 240 ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000010, 0x0000000C, 4 ); } //

		if( cy == 576 || cy == 288 ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000010, 0x0000000E, 4 ); } //

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000014, 0x0004927A, 4 );

		#define INTERNAL_SYSTEM_CLOCK (131)

		R00780018 = (64000 * INTERNAL_SYSTEM_CLOCK) / (4096);

		R00780018 |= (R00780018 << 16);

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000018, R00780018, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x0000001C, R00780018, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000028, 131625, 4 ); //

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000200, 0x0055FFFF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000234, 0x01F23200, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000238, 0x00000041, 4 );

		R007D600C = AH8400_GetRegister( pDevice, 3, 0x007D6000 + 0x0000000C, 4 ); R007D600C &= 0xFFFFFFFC;

		AH8400_SetRegister( pDevice, 3, 0x007D6000 + 0x0000000C, R007D600C, 4 );

		{	UCHAR custom_codes[ 32 ] = { 0x19, 0x97, 0x09, 0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, };

			UCHAR i = 0;

			AH8400_SetRegister( pDevice, 2, 0x00000284, 0x11800000, 4 );

			for( i = 0 ; i < 32 ; i++ ) { writeb( custom_codes[ i ], (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ 1 ] + (0x01F23200 & 0x007FFFFF)) + (i) ); }

			AH8400_SetRegister( pDevice, 2, 0x00000284, 0x11800000, 4 );

			for( i = 0 ; i < 32 ; i++ ) { writeb( custom_codes[ i ], (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ 1 ] + (0x01F23200 & 0x007FFFFF)) + (i) ); }

			AH8400_SetRegister( pDevice, 2, 0x00000284, 0x19800000, 4 );

			for( i = 0 ; i < 32 ; i++ ) { writeb( custom_codes[ i ], (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ 1 ] + (0x01F23200 & 0x007FFFFF)) + (i) ); }

			AH8400_SetRegister( pDevice, 2, 0x00000284, 0x19800000, 4 );

			for( i = 0 ; i < 32 ; i++ ) { writeb( custom_codes[ i ], (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ 1 ] + (0x01F23200 & 0x007FFFFF)) + (i) ); }

			AH8400_SetRegister( pDevice, 2, 0x00000284, 0x7F800000, 4 );
		}
		R007D600C = AH8400_GetRegister( pDevice, 3, 0x007D6000 + 0x0000000C, 4 ); R007D600C |= 0x00000001;

		AH8400_SetRegister( pDevice, 3, 0x007D6000 + 0x0000000C, R007D600C, 4 );
	}
	//
	// 
	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000E0, 0x00000400, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000E0, 0x00000000, 4 ); //

//	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000E0, 0x00000281, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000E0, 0x00000241, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000E8, 0x78787878, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000EC, 0x78787878, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000F0, 0x78787878, 4 ); //

	AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x000000F4, 0x78787878, 4 ); //

	//
	// 
	{	
//	    static USHORT OSD_REGISTER_TABLES_06X08[ 640 ]  = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  
//															0x0154, 0x0401, 0x0405, 0x0411, 0x0441, 0x0501, 0x0401, 0x0154, // 0
//															0x0010, 0x0050, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0054, // 1
//															0x0154, 0x0501, 0x0001, 0x0004, 0x0010, 0x0040, 0x0100, 0x0555, // 2
//															0x0154, 0x0501, 0x0001, 0x0054, 0x0001, 0x0001, 0x0501, 0x0154, // 3
//															0x0004, 0x0014, 0x0044, 0x0104, 0x0404, 0x0555, 0x0004, 0x0004, // 4
//															0x0555, 0x0400, 0x0400, 0x0554, 0x0001, 0x0001, 0x0501, 0x0154, // 5
//															0x0154, 0x0405, 0x0400, 0x0554, 0x0401, 0x0401, 0x0401, 0x0154, // 6
//															0x0555, 0x0401, 0x0001, 0x0004, 0x0010, 0x0010, 0x0010, 0x0010, // 7
//															0x0154, 0x0401, 0x0401, 0x0154, 0x0401, 0x0401, 0x0401, 0x0154, // 8
//															0x0154, 0x0401, 0x0401, 0x0401, 0x0155, 0x0001, 0x0501, 0x0154, // 9
//															0x0154, 0x0401, 0x0401, 0x0401, 0x0555, 0x0401, 0x0401, 0x0000, // A
//															0x0554, 0x0401, 0x0401, 0x0554, 0x0401, 0x0401, 0x0554, 0x0000, // B
//															0x0154, 0x0401, 0x0400, 0x0400, 0x0400, 0x0401, 0x0154, 0x0000, // C
//															0x0554, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0554, 0x0000, // D
//															0x0555, 0x0400, 0x0400, 0x0554, 0x0400, 0x0400, 0x0555, 0x0000, // E
//															0x0555, 0x0400, 0x0400, 0x0554, 0x0400, 0x0400, 0x0400, 0x0000, // F
//															0x0154, 0x0401, 0x0400, 0x0415, 0x0401, 0x0401, 0x0154, 0x0000, // G
//															0x0401, 0x0401, 0x0401, 0x0555, 0x0401, 0x0401, 0x0401, 0x0000, // H
//															0x0150, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0150, 0x0000, // I
//															0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0104, 0x0050, 0x0000, // J
//															0x0401, 0x0404, 0x0410, 0x0440, 0x0510, 0x0404, 0x0401, 0x0000, // K
//															0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0555, 0x0000, // L
//															0x0401, 0x0505, 0x0451, 0x0401, 0x0401, 0x0401, 0x0401, 0x0000, // M
//															0x0401, 0x0501, 0x0541, 0x0451, 0x0415, 0x0405, 0x0401, 0x0000, // N
//															0x0154, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0154, 0x0000, // O
//															0x0554, 0x0401, 0x0401, 0x0554, 0x0400, 0x0400, 0x0400, 0x0000, // P
//															0x0154, 0x0401, 0x0401, 0x0401, 0x0411, 0x0405, 0x0154, 0x0000, // Q
//															0x0554, 0x0401, 0x0401, 0x0554, 0x0410, 0x0404, 0x0401, 0x0000, // R
//															0x0154, 0x0401, 0x0400, 0x0154, 0x0001, 0x0401, 0x0154, 0x0000, // S
//															0x0155, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0000, // T
//															0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0154, 0x0000, // U
//															0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0104, 0x0050, 0x0000, // V
//															0x0404, 0x0404, 0x0404, 0x0444, 0x0444, 0x0444, 0x0110, 0x0000, // W
//															0x0401, 0x0401, 0x0104, 0x0050, 0x0104, 0x0401, 0x0401, 0x0000, // X
//															0x0404, 0x0404, 0x0110, 0x0040, 0x0040, 0x0040, 0x0040, 0x0000, // Y
//															0x0555, 0x0001, 0x0004, 0x0010, 0x0040, 0x0100, 0x0555, 0x0000, // Z
//															0x0000, 0x0000, 0x0150, 0x0004, 0x0154, 0x0404, 0x0151, 0x0000, // a
//															0x0400, 0x0400, 0x0550, 0x0404, 0x0404, 0x0404, 0x0550, 0x0000, // b
//															0x0000, 0x0000, 0x0150, 0x0404, 0x0400, 0x0404, 0x0150, 0x0000, // c
//															0x0004, 0x0004, 0x0154, 0x0404, 0x0404, 0x0404, 0x0154, 0x0000, // d
//															0x0000, 0x0000, 0x0150, 0x0404, 0x0554, 0x0400, 0x0154, 0x0000, // e
//															0x0054, 0x0040, 0x0040, 0x0554, 0x0040, 0x0040, 0x0040, 0x0000, // f
//															0x0000, 0x0000, 0x0150, 0x0404, 0x0404, 0x0154, 0x0004, 0x0550, // g
//															0x0400, 0x0400, 0x0550, 0x0404, 0x0404, 0x0404, 0x0404, 0x0000, // h
//															0x0040, 0x0000, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0000, // i
//															0x0004, 0x0000, 0x0004, 0x0004, 0x0004, 0x0004, 0x0104, 0x0050, // j
//															0x0400, 0x0400, 0x0404, 0x0410, 0x0440, 0x0510, 0x0404, 0x0000, // k
//															0x0140, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0150, 0x0000, // l
//															0x0000, 0x0000, 0x0110, 0x0444, 0x0444, 0x0404, 0x0404, 0x0000, // m
//															0x0000, 0x0000, 0x0450, 0x0504, 0x0404, 0x0404, 0x0404, 0x0000, // n
//															0x0000, 0x0000, 0x0150, 0x0404, 0x0404, 0x0404, 0x0150, 0x0000, // o
//															0x0000, 0x0000, 0x0550, 0x0404, 0x0404, 0x0550, 0x0400, 0x0400, // p
//															0x0000, 0x0000, 0x0154, 0x0404, 0x0404, 0x0154, 0x0004, 0x0004, // q
//															0x0000, 0x0000, 0x0450, 0x0504, 0x0400, 0x0400, 0x0400, 0x0000, // r
//															0x0000, 0x0000, 0x0154, 0x0400, 0x0150, 0x0004, 0x0550, 0x0000, // s
//															0x0040, 0x0040, 0x0554, 0x0040, 0x0040, 0x0040, 0x0054, 0x0000, // t
//															0x0000, 0x0000, 0x0404, 0x0404, 0x0404, 0x0404, 0x0150, 0x0000, // u
//															0x0000, 0x0000, 0x0404, 0x0404, 0x0404, 0x0110, 0x0040, 0x0000, // v
//															0x0000, 0x0000, 0x0404, 0x0404, 0x0444, 0x0444, 0x0110, 0x0000, // w
//															0x0000, 0x0000, 0x0404, 0x0110, 0x0040, 0x0110, 0x0404, 0x0000, // x
//															0x0000, 0x0000, 0x0404, 0x0404, 0x0154, 0x0004, 0x0404, 0x0150, // y
//															0x0000, 0x0000, 0x0554, 0x0010, 0x0040, 0x0100, 0x0554, 0x0000, // z
//															0x0000, 0x0050, 0x0050, 0x0000, 0x0000, 0x0000, 0x0050, 0x0050, // :
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//															0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
//		};
		static USHORT OSD_REGISTER_TABLES_08X10[ 640 ]  = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 
															0x0000, 0x0154, 0x0401, 0x0405, 0x0411, 0x0441, 0x0501, 0x0401, 0x0154, 0x0000, // 0
															0x0000, 0x0010, 0x0050, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0054, 0x0000, // 1
															0x0000, 0x0154, 0x0501, 0x0001, 0x0004, 0x0010, 0x0040, 0x0100, 0x0555, 0x0000, // 2
															0x0000, 0x0154, 0x0501, 0x0001, 0x0054, 0x0001, 0x0001, 0x0501, 0x0154, 0x0000, // 3
															0x0000, 0x0004, 0x0014, 0x0044, 0x0104, 0x0404, 0x0555, 0x0004, 0x0004, 0x0000, // 4
															0x0000, 0x0555, 0x0400, 0x0400, 0x0554, 0x0001, 0x0001, 0x0501, 0x0154, 0x0000, // 5
															0x0000, 0x0154, 0x0405, 0x0400, 0x0554, 0x0401, 0x0401, 0x0401, 0x0154, 0x0000, // 6
															0x0000, 0x0555, 0x0401, 0x0001, 0x0004, 0x0010, 0x0010, 0x0010, 0x0010, 0x0000, // 7
															0x0000, 0x0154, 0x0401, 0x0401, 0x0154, 0x0401, 0x0401, 0x0401, 0x0154, 0x0000, // 8
															0x0000, 0x0154, 0x0401, 0x0401, 0x0401, 0x0155, 0x0001, 0x0501, 0x0154, 0x0000, // 9
															0x0000, 0x0154, 0x0401, 0x0401, 0x0401, 0x0555, 0x0401, 0x0401, 0x0000, 0x0000, // A
															0x0000, 0x0554, 0x0401, 0x0401, 0x0554, 0x0401, 0x0401, 0x0554, 0x0000, 0x0000, // B
															0x0000, 0x0154, 0x0401, 0x0400, 0x0400, 0x0400, 0x0401, 0x0154, 0x0000, 0x0000, // C
															0x0000, 0x0554, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0554, 0x0000, 0x0000, // D
															0x0000, 0x0555, 0x0400, 0x0400, 0x0554, 0x0400, 0x0400, 0x0555, 0x0000, 0x0000, // E
															0x0000, 0x0555, 0x0400, 0x0400, 0x0554, 0x0400, 0x0400, 0x0400, 0x0000, 0x0000, // F
															0x0000, 0x0154, 0x0401, 0x0400, 0x0415, 0x0401, 0x0401, 0x0154, 0x0000, 0x0000, // G
															0x0000, 0x0401, 0x0401, 0x0401, 0x0555, 0x0401, 0x0401, 0x0401, 0x0000, 0x0000, // H
															0x0000, 0x0150, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0150, 0x0000, 0x0000, // I
															0x0000, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0104, 0x0050, 0x0000, 0x0000, // J
															0x0000, 0x0401, 0x0404, 0x0410, 0x0440, 0x0510, 0x0404, 0x0401, 0x0000, 0x0000, // K
															0x0000, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0555, 0x0000, 0x0000, // L
															0x0000, 0x0401, 0x0505, 0x0451, 0x0401, 0x0401, 0x0401, 0x0401, 0x0000, 0x0000, // M
															0x0000, 0x0401, 0x0501, 0x0541, 0x0451, 0x0415, 0x0405, 0x0401, 0x0000, 0x0000, // N
															0x0000, 0x0154, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0154, 0x0000, 0x0000, // O
															0x0000, 0x0554, 0x0401, 0x0401, 0x0554, 0x0400, 0x0400, 0x0400, 0x0000, 0x0000, // P
															0x0000, 0x0154, 0x0401, 0x0401, 0x0401, 0x0411, 0x0405, 0x0154, 0x0000, 0x0000, // Q
															0x0000, 0x0554, 0x0401, 0x0401, 0x0554, 0x0410, 0x0404, 0x0401, 0x0000, 0x0000, // R
															0x0000, 0x0154, 0x0401, 0x0400, 0x0154, 0x0001, 0x0401, 0x0154, 0x0000, 0x0000, // S
															0x0000, 0x0155, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0000, 0x0000, // T
															0x0000, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0154, 0x0000, 0x0000, // U
															0x0000, 0x0401, 0x0401, 0x0401, 0x0401, 0x0401, 0x0104, 0x0050, 0x0000, 0x0000, // V
															0x0000, 0x0404, 0x0404, 0x0404, 0x0444, 0x0444, 0x0444, 0x0110, 0x0000, 0x0000, // W
															0x0000, 0x0401, 0x0401, 0x0104, 0x0050, 0x0104, 0x0401, 0x0401, 0x0000, 0x0000, // X
															0x0000, 0x0404, 0x0404, 0x0110, 0x0040, 0x0040, 0x0040, 0x0040, 0x0000, 0x0000, // Y
															0x0000, 0x0555, 0x0001, 0x0004, 0x0010, 0x0040, 0x0100, 0x0555, 0x0000, 0x0000, // Z
															0x0000, 0x0000, 0x0000, 0x0150, 0x0004, 0x0154, 0x0404, 0x0151, 0x0000, 0x0000, // a
															0x0000, 0x0400, 0x0400, 0x0550, 0x0404, 0x0404, 0x0404, 0x0550, 0x0000, 0x0000, // b
															0x0000, 0x0000, 0x0000, 0x0150, 0x0404, 0x0400, 0x0404, 0x0150, 0x0000, 0x0000, // c
															0x0000, 0x0004, 0x0004, 0x0154, 0x0404, 0x0404, 0x0404, 0x0154, 0x0000, 0x0000, // d
															0x0000, 0x0000, 0x0000, 0x0150, 0x0404, 0x0554, 0x0400, 0x0154, 0x0000, 0x0000, // e
															0x0000, 0x0054, 0x0040, 0x0040, 0x0554, 0x0040, 0x0040, 0x0040, 0x0000, 0x0000, // f
															0x0000, 0x0000, 0x0000, 0x0150, 0x0404, 0x0404, 0x0154, 0x0004, 0x0550, 0x0000, // g
															0x0000, 0x0400, 0x0400, 0x0550, 0x0404, 0x0404, 0x0404, 0x0404, 0x0000, 0x0000, // h
															0x0000, 0x0040, 0x0000, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0000, 0x0000, // i
															0x0000, 0x0004, 0x0000, 0x0004, 0x0004, 0x0004, 0x0004, 0x0104, 0x0050, 0x0000, // j
															0x0000, 0x0400, 0x0400, 0x0404, 0x0410, 0x0440, 0x0510, 0x0404, 0x0000, 0x0000, // k
															0x0000, 0x0140, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0150, 0x0000, 0x0000, // l
															0x0000, 0x0000, 0x0000, 0x0110, 0x0444, 0x0444, 0x0404, 0x0404, 0x0000, 0x0000, // m
															0x0000, 0x0000, 0x0000, 0x0450, 0x0504, 0x0404, 0x0404, 0x0404, 0x0000, 0x0000, // n
															0x0000, 0x0000, 0x0000, 0x0150, 0x0404, 0x0404, 0x0404, 0x0150, 0x0000, 0x0000, // o
															0x0000, 0x0000, 0x0000, 0x0550, 0x0404, 0x0404, 0x0550, 0x0400, 0x0400, 0x0000, // p
															0x0000, 0x0000, 0x0000, 0x0154, 0x0404, 0x0404, 0x0154, 0x0004, 0x0004, 0x0000, // q
															0x0000, 0x0000, 0x0000, 0x0450, 0x0504, 0x0400, 0x0400, 0x0400, 0x0000, 0x0000, // r
															0x0000, 0x0000, 0x0000, 0x0154, 0x0400, 0x0150, 0x0004, 0x0550, 0x0000, 0x0000, // s
															0x0000, 0x0040, 0x0040, 0x0554, 0x0040, 0x0040, 0x0040, 0x0054, 0x0000, 0x0000, // t
															0x0000, 0x0000, 0x0000, 0x0404, 0x0404, 0x0404, 0x0404, 0x0150, 0x0000, 0x0000, // u
															0x0000, 0x0000, 0x0000, 0x0404, 0x0404, 0x0404, 0x0110, 0x0040, 0x0000, 0x0000, // v
															0x0000, 0x0000, 0x0000, 0x0404, 0x0404, 0x0444, 0x0444, 0x0110, 0x0000, 0x0000, // w
															0x0000, 0x0000, 0x0000, 0x0404, 0x0110, 0x0040, 0x0110, 0x0404, 0x0000, 0x0000, // x
															0x0000, 0x0000, 0x0000, 0x0404, 0x0404, 0x0154, 0x0004, 0x0404, 0x0150, 0x0000, // y
															0x0000, 0x0000, 0x0000, 0x0554, 0x0010, 0x0040, 0x0100, 0x0554, 0x0000, 0x0000, // z
															0x0000, 0x0000, 0x0050, 0x0050, 0x0000, 0x0000, 0x0000, 0x0050, 0x0050, 0x0000, // :
		};

	//	CHAR psz[ 32 ] = "  000000 00:00:00";

		ULONG i = 0, R007E7014, R007E700C, R007E7028, R007E7020;

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x000000C0, 0x0000000F, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x0000000C, 0x00000003, 4 );

		for( i = 0 ; i < 640 ; i ++ ) {
			
			AH8400_SetRegister( pDevice, 3, 0x007E0000 + (i << 2), OSD_REGISTER_TABLES_08X10[ i ], 2 );
		}
		for( i = 0 ; i < 32 ; i++ ) {
		
			AH8400_SetRegister( pDevice, 3, 0x007E3000 + (i << 2), 0x0000, 2 );
		}
		for( i = 0 ; i < 40 ; i++ ) {
		
			AH8400_SetRegister( pDevice, 3, 0x007E1000 + (i << 2), 0x0000, 2 );

			AH8400_SetRegister( pDevice, 3, 0x007E2000 + (i << 2), 0x0000, 2 );
		}

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x000000C0, 0x00000000, 4 );

		R007E7014 = AH8400_GetRegister( pDevice, 3, 0x007E7000 + 0x00000014, 4 ); //

		R007E7014 = (R007E7014 & 0x0000FE00) | (0x00000004 << 0);

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000014, R007E7014, 4 );

		R007E700C = AH8400_GetRegister( pDevice, 3, 0x007E7000 + 0x0000000C, 4 ); //

		R007E700C = (R007E700C & 0x000000FF) | (0x00000010 << 8);

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x0000000C, R007E700C, 4 );

		R007E7028 = AH8400_GetRegister( pDevice, 3, 0x007E7000 + 0x00000028, 4 ); //

		R007E7028 = (R007E7028 & 0x0000FE00) | (0x00000004 << 0);

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000028, R007E7028, 4 );

		R007E7020 = AH8400_GetRegister( pDevice, 3, 0x007E7000 + 0x00000020, 4 ); //

		R007E7020 = (R007E7020 & 0x0000FF00) | (0x00000018 << 0);

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000020, R007E7020, 4 );
		//

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000034, 0x00002808, 4 );//

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000038, 0x00000000, 4 );
		
		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x0000003C, 0x00000000, 4 );
		
		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000040, 0x00000000, 4 );
		
		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000044, 0x00000000, 4 );

		//
		// 
		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000000, 0x000F0F0F, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000004, 0x0000FFFF, 4 );

		AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000008, 0x0000FFFF, 4 );

	//	//
	}

	//
	//  
	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||

		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		ULONG R007D2400 = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x00000400, 4 ); R007D2400 |= 0x000000C0;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x00000400, R007D2400, 4 );
	}
	else {

		ULONG R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x00000018;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );
	}
	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||
		
		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		ULONG R007D2400 = 0x00000000;

		ULONGLONG delay = 10;

		struct timeval ts; do_gettimeofday( &ts );

		AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x00000400, 4 ); R007D2400 |= 0x000000C0;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x00000400, R007D2400, 4 );

		{	BYTE codes[ 4 ] = { ((ts.tv_usec / 1) % 0xFF), ((ts.tv_usec / 10) % 0xFF), ((ts.tv_usec / 100) % 0xFF), ((ts.tv_usec / 1000) % 0xFF) };

			BYTE keys[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };
	
			AH8400_I2C_START( pDevice, delay ); AH8400_I2C_WRITE_BYTE( pDevice, 0xA2, delay ); AH8400_I2C_WRITE_BYTE( pDevice, 0x13, delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 0 ], delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 1 ], delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 2 ], delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 3 ], delay ); AH8400_I2C_STOP( pDevice, delay );
	
			schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	
			AH8400_I2C_START( pDevice, delay ); AH8400_I2C_WRITE_BYTE( pDevice, 0xA3, delay ); keys[ 0 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 1 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 2 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 3 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); AH8400_I2C_STOP( pDevice, delay );
	
			if( (*((DWORD *)(keys)) != 0x00000000) && (*((DWORD *)(keys)) != 0xFFFFFFFF) ) {
	
				g_copy_protect_unlock_boradsA[ 0 ] = 1; 
				
				g_copy_protect_unlock_boradsB[ 0 ] = 1;

			}
		}
	}
	else {

		ULONG R007D0400 = 0x00000000;

		ULONGLONG delay = 10;

		struct timeval ts; do_gettimeofday( &ts );

		R007D0400 = AH8400_GetRegister( pDevice, 3, 0x007D0000 + 0x00000400, 4 ); R007D0400 |= 0x00000018;

		AH8400_SetRegister( pDevice, 3, 0x007D0000 + 0x00000400, R007D0400, 4 );

		{	BYTE codes[ 4 ] = { ((ts.tv_usec / 1) % 0xFF), ((ts.tv_usec / 10) % 0xFF), ((ts.tv_usec / 100) % 0xFF), ((ts.tv_usec / 1000) % 0xFF) };

			BYTE keys[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };
	
			AH8400_I2C_START( pDevice, delay ); AH8400_I2C_WRITE_BYTE( pDevice, 0xA2, delay ); AH8400_I2C_WRITE_BYTE( pDevice, 0x13, delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 0 ], delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 1 ], delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 2 ], delay ); AH8400_I2C_WRITE_BYTE( pDevice, codes[ 3 ], delay ); AH8400_I2C_STOP( pDevice, delay );
	
			schedule_timeout_interruptible( msecs_to_jiffies( 100 ) );
	
			AH8400_I2C_START( pDevice, delay ); AH8400_I2C_WRITE_BYTE( pDevice, 0xA3, delay ); keys[ 0 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 1 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 2 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); keys[ 3 ] = AH8400_I2C_READ_BYTE( pDevice, 0, delay ); AH8400_I2C_STOP( pDevice, delay );
		
			if( (*((DWORD *)(keys)) != 0x00000000) && (*((DWORD *)(keys)) != 0xFFFFFFFF) ) {

				g_copy_protect_unlock_boradsA[ 0 ] = 1; 
				
				g_copy_protect_unlock_boradsB[ 0 ] = 1;
			}
		}
	}

	//
	// 
	if( (pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x95 ||

		(pDevice->m_pKsDevice->subsystem_vendor & 0x00FF) == 0x96 ||
		
		(pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ) {

		LONG i = pDevice->m_nKsDeviceNumber * 4;

		ULONG R007D3400 = 0x00000000;
		
		ULONG R007D33FC = 0x00000000;
		
		AH8400_GetRegister( pDevice, 3, 0x007D3000 + 0x00000400, 1 ); R007D3400 &= 0xFFFFFF00;

		R007D3400 |= g_n_gpio_direction[ i ] & 0x000000FF;

		AH8400_SetRegister( pDevice, 3, 0x007D3000 + 0x00000400, R007D3400, 1 );

		R007D33FC = AH8400_GetRegister( pDevice, 3, 0x007D3000 + 0x000003FC, 1 ); R007D33FC &= 0xFFFFFF00;

		R007D33FC |= g_n_gpio_data[ i ] & 0x000000FF;

		AH8400_SetRegister( pDevice, 3, 0x007D3000 + 0x000003FC, R007D33FC, 1 );
	}
	else {

		LONG i = pDevice->m_nKsDeviceNumber * 4;

		ULONG R007D2400 = 0x00000000;

		ULONG R007D3400 = 0x00000000;

		ULONG R007D23FC = 0x00000000;

		ULONG R007D33FC = 0x00000000;

		R007D2400 = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x00000400, 1 ); R007D2400 &= 0xFFFFFF00;

		R007D3400 = AH8400_GetRegister( pDevice, 3, 0x007D3000 + 0x00000400, 1 ); R007D3400 &= 0xFFFFFF00;

		R007D2400 |= (g_n_gpio_direction[ i ] >> 0) & 0x000000FF;

		R007D3400 |= (g_n_gpio_direction[ i ] >> 8) & 0x000000FF;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x00000400, R007D2400, 1 );

		AH8400_SetRegister( pDevice, 3, 0x007D3000 + 0x00000400, R007D3400, 1 );

		R007D23FC = AH8400_GetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, 1 ); R007D23FC &= 0xFFFFFF00;

		R007D33FC = AH8400_GetRegister( pDevice, 3, 0x007D3000 + 0x000003FC, 1 ); R007D33FC &= 0xFFFFFF00;

		R007D23FC |= (g_n_gpio_data[ i ] >> 0) & 0x000000FF;

		R007D33FC |= (g_n_gpio_data[ i ] >> 8) & 0x000000FF;

		AH8400_SetRegister( pDevice, 3, 0x007D2000 + 0x000003FC, R007D23FC, 1 );

		AH8400_SetRegister( pDevice, 3, 0x007D3000 + 0x000003FC, R007D33FC, 1 );
	}
	return TRUE;
}

BOOLEAN AH8400_HwUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] AH8400_HwUnInitialize()\n", pDevice->m_nKsDeviceNumber );

	AH8400_SetRegister( pDevice, 2, 0x00000104, 0x00000000, 4 ); //

	AH8400_ENTER_PCI_LIVE_DMA_ACCESS();

	AH8400_SetRegister( pDevice, 2, 0x00000104, 0x00000000, 4 ); //

	AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();

	AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x0000000C, 0xFFFFFFFF, 4 );

	return TRUE;
}

// ##############################################################################################################################################################################
//
BOOLEAN AH8400_ReStartLiveDMA( CDevice * pDevice )
{
	ULONG R00760038 = 0x00000000;
	
	ULONG R000002A4 = 0x00000000; 
		
	ULONG R00000140 = 0x00000000;
		
	ULONG R007D8008 = 0x00000000;

	{	AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F4, 0x00020001, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, 0x00CF0000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000030, 0x00000000, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 |= 0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 &= ~0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F4, 0x00000000, 4 );
	}
	{	AH8400_ENTER_PCI_LIVE_DMA_ACCESS();

		R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 &= ~(0x000009C9); R000002A4 &= ~(0x00004000); //

		AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000001);

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000004;

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000008;

		AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

		R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x00002000;

		AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

		AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();
	}
	{	AH8400_ENTER_PCI_LIVE_DMA_ACCESS();

		AH8400_SetRegister( pDevice, 2, 0x00000104, 0x000000FF, 4 );

		AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();
	}
	{	R007D8008 = AH8400_GetRegister( pDevice, 3, 0x007D8000 + 0x00000008, 4 ); R007D8008 |= 0x00000200;

		AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x00000008, R007D8008, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 |= 0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 &= ~0x50000800;

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000004, 0x02800000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000008, 0x02800000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000000, 0x002D0000, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x0000000C, 0x000002D0, 4 );

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000024, 0x00002CF0, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000090, 0x000000F0, 4 );
		}
		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000024, 0x00002D20, 4 );

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000090, 0x00000120, 4 );
		}
		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000030, 0x000007D8, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, 0x00CF0003, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x0000003C, 0x08080014, 4 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x000000F4, 0x00020001, 4 );
	}
	return TRUE;
}

BOOLEAN AH8400_ReStartHDMA( CDevice * pDevice )
{
	ULONG R000002A4 = 0x00000000;
	
	ULONG R00000140 = 0x00000000; 
		
	ULONG R007D8008 = 0x00000000;

	//
	//
	AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x0000000C, 0x00000100, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000000, 0x00000000, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000014, 0x00000000, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000024, 0x00000000, 4 );

	//
	// 
	R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 &= ~(0x000009C9); R000002A4 &= ~(0x00004000); //

	AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

	R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000001);

	AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

	R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000004;

	AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

	R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000008;

	AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );

	R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x00002000;

	AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );

	//
	// 
	R007D8008 = AH8400_GetRegister( pDevice, 3, 0x007D8000 + 0x00000008, 4 ); R007D8008 |= 0x00000100;

	AH8400_SetRegister( pDevice, 3, 0x007D8000 + 0x00000008, R007D8008, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000000, 0x80000000, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000014, 0x08180000, 4 );

	AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000024, 0x08180000, 4 );

	return TRUE;
}

struct rtc_time now;

BOOLEAN AH8400_HwProcessAnalogPCIVideoPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{
	if( mask & 0x00000200 ) { //

		ULONG R00760034 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000034, 4 ); R00760034 &= 0x000007FC;

//		LINUXV4L2_DEBUG( KERN_INFO, "AH8400_PCIVideoPacket() m_nKsDeviceNumber(0x%x) R00760034(0x%x)\n", pDevice->m_nKsDeviceNumber, R00760034 );

		AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000034, R00760034, 4 ); //

		if( R00760034 & (~0x0000005C) ) { //

			LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH0X] [UYVY] [HARDWARE.RESET R00760034 = %08lX]\n", pDevice->m_nKsDeviceNumber, R00760034 ); 

			AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000034, 0x02000000, 4 );
			
			AH8400_ReStartLiveDMA( pDevice );

			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] >= 0x00000001 ) {
				
				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0x00000000;

				pDevice->m_nDmaBaseCommonBufferNumber[ 0 ]++;

				pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] %= 2;
			}
		}
		if( R00760034 & 0x00000040 ) { //

			ULONG i = 0;

			for( i = 0 ; i < 4 ; i ++ ) {
				
//				LINUXV4L2_DEBUG( KERN_INFO, "AH8400_PCIVideoPacket()  pVideos[%d](0x%x) pVideoBuffers[%d](0x%x)\n", i, i, pVideos[ i ], pVideoBuffers[ i ] );

				if( pVideos[ i ] && pVideoBuffers[ i ] ) {

					if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] == 0x00000002 ) {
					
						pVideoBuffers[ i ]->m_sKsBuffer.input = pDevice->m_nDmaBaseCommonBufferNumber[ 0 ];

						p_is_completes[ i ] = TRUE;

						p_is_drops[ i ] = FALSE;
					}
				}
			}
			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] == 0x00000001 ) { 
				
				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0x00000000; 
			}
			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] >= 0x00000002 ) { 
				
				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0x00000000;

				pDevice->m_nDmaBaseCommonBufferNumber[ 0 ]++;	

				pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] %= 2;
			}
		}
	//	if( R00760034 & 0x00000004 ) { //

		if( R00760034 & 0x00000018 ) { //

			if( pVideos[ 0 ] ||
				
				pVideos[ 1 ] ||
				
				pVideos[ 2 ] ||
				
				pVideos[ 3 ] ) {

//				LINUXV4L2_DEBUG( KERN_INFO, "m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] (0x%x)\n", pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ]);

				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] == 0x00000000 ) { //

					ULONG memory_physical_address = (ULONG)(pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ] + 0x00000000);

					ULONG field_info = 0x00000000;

					ULONG cxe = 0;

					ULONG cye = 0;

					if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) { //

						if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { cxe = 720; cye = 480; field_info = (R00760034 & 0x00000008) ? 0 : 1; } //
						
						if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { cxe = 720; cye = 576; field_info = (R00760034 & 0x00000008) ? 0 : 1; } //
					}
					else { //

						if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { cxe = 720; cye = 480; field_info = (R00760034 & 0x00000008) ? 0 : 1; } //

						if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { cxe = 720; cye = 576; field_info = (R00760034 & 0x00000008) ? 1 : 0; } //
					}
					switch( pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] % 2 ) {

					case 0: memory_physical_address += (1440 * 576) * 0; break;

					case 1: memory_physical_address += (1440 * 576) * 1; break;
					}
					if( field_info == 0 ) { //

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 1;

						memory_physical_address += (1440 * 288) * 0;
					}
					if( field_info == 1 ) { //

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 2;

						memory_physical_address += (1440 * 288) * 1;
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////

					AH8400_ENTER_PCI_LIVE_DMA_ACCESS(); //
					{	
						ULONG R00000140 = 0x00000000;
						
						AH8400_SetRegister( pDevice, 2, 0x00000144, memory_physical_address, 4 );

						AH8400_SetRegister( pDevice, 2, 0x00000148, cxe << 1, 4 );

						R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000F00); R00000140 |= 0x00030701;

						AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
					}
					AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();

					///////////////////////////////////

					{	ULONG R00760038 = 0x00000000;
						
						ULONG R00760024 = 0x00000000;
						
						R00760038 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000038, 4 ); R00760038 |= 0x000F0000;

						AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000038, R00760038, 4 );

						R00760024 = AH8400_GetRegister( pDevice, 3, 0x00760000 + 0x00000024, 4 ); R00760024 |= 0x00002000;

						AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000024, R00760024, 4 );

						AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000014, memory_physical_address, 4 );
					}
					///////////////////////////////////////////////////////////////////////////////////

					AH8400_ENTER_PCI_LIVE_DMA_ACCESS(); //
					{
						ULONG R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x000009C9; R000002A4 &= ~(0x00004000);

						AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );
					}
					AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();

					///////////////////////////////////

					AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000034, 0x82000000, 4 );

					//////////////////////////////////////////////////////////////////////

					AH8400_ENTER_PCI_LIVE_DMA_ACCESS();
					{	
						ULONG R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000002;

						AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
					}
					AH8400_LEAVE_PCI_LIVE_DMA_ACCESS();
				}
				else {

					AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000034, 0x02000000, 4 );

					if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] == 3 ) {

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 4;
					}
					else if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] == 4 ) {

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0;

						LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH0X] [UYVY] [SOFTWARE.RESET]\n", pDevice->m_nKsDeviceNumber );
					}
					else {

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 3;
					}
	
//					LINUXV4L2_DEBUG( KERN_INFO, "m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] (0x%x)\n", pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ]);

				}
			}
			else {

//				LINUXV4L2_DEBUG( KERN_INFO, "interrupt 0x10 |0x08 no pVideos\n");

				AH8400_SetRegister( pDevice, 3, 0x00760000 + 0x00000034, 0x02000000, 4 );
			}

			//
			//
			if( (pDevice->m_nDmaBaseCommonBufferNumber[ 3 ]++ % 5) == 0 ) {
			
				ULONG i, j, y, flags;

//				struct rtc_time now;

				spin_lock_irqsave(&rtc_lock, flags);
	
				BYTE uip = (CMOS_READ(RTC_FREQ_SELECT) & RTC_UIP);// 0x80 is not ready
	
				if(uip != 0x80)
				{
				now.tm_sec = BCD_TO_BINARY(CMOS_READ(RTC_SECONDS));
				
				now.tm_min = BCD_TO_BINARY(CMOS_READ(RTC_MINUTES));
				
				now.tm_hour = BCD_TO_BINARY(CMOS_READ(RTC_HOURS));
				
				now.tm_mday = BCD_TO_BINARY(CMOS_READ(RTC_DAY_OF_MONTH));
				
				now.tm_mon = BCD_TO_BINARY(CMOS_READ(RTC_MONTH));
				
				now.tm_year = BCD_TO_BINARY(CMOS_READ(RTC_YEAR));

				now.tm_year += 1900;

				if( now.tm_year < 1970 ) { now.tm_year += 100; }
				}

				spin_unlock_irqrestore( &rtc_lock, flags );

				for( i = 0 ; i < 4 ; i++ ) {

					CVideo * pVideo = NULL;

					ULONG index = pDevice->m_nKsDeviceNumber * 4 + i;

					if( i == 0 ) { pVideo = pVideos[ 16 + 0 ]; }

					if( i == 1 ) { pVideo = pVideos[ 16 + 1 ]; }

					if( i == 2 ) { pVideo = pVideos[ 16 + 2 ]; }

					if( i == 3 ) { pVideo = pVideos[ 16 + 3 ]; }

					AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x000000C0, 0x00000001 << i, 4 );

					AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x00000038 + (i * 4), g_n_encoder_osd_color[ index ] % 16, 4 );

					for( y = 0 ; y < 1 ; y++ ) {

						CHAR psz[ 32 + 1 ] = "01234567890123456789012345678901"; memset( psz, 0, 32 + 1 );

   						if( strlen( g_p_encoder_osd_text[ index ] ) != 0 ) {

							if( pVideo ) {
									
									if( y == 0 && pVideo->m_nSrcFrameWidth == 720 ) { memcpy( psz + 0, &g_p_encoder_osd_text[ index ][ 0 ], 32 ); } 

									if( y == 0 && pVideo->m_nSrcFrameWidth == 704 ) { memcpy( psz + 0, &g_p_encoder_osd_text[ index ][ 0 ], 32 ); } 

									if( y == 0 && pVideo->m_nSrcFrameWidth == 640 ) { memcpy( psz + 0, &g_p_encoder_osd_text[ index ][ 0 ], 32 ); } 

									if( y == 0 && pVideo->m_nSrcFrameWidth == 352 ) { memcpy( psz + 2, &g_p_encoder_osd_text[ index ][ 0 ], 15 ); } 

									if( y == 0 && pVideo->m_nSrcFrameWidth == 320 ) { memcpy( psz + 0, &g_p_encoder_osd_text[ index ][ 0 ], 15 ); }
							}
						}
						else {

							if( y == 0 ) { 

								if( pVideo ) {

									if( pVideo->m_nSrcFrameWidth == 720 ) { sprintf( psz, "%04d%02d%02d %02d:%02d:%02d",  now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec ); }

									if( pVideo->m_nSrcFrameWidth == 704 ) { sprintf( psz, "%04d%02d%02d %02d:%02d:%02d",  now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec ); }

									if( pVideo->m_nSrcFrameWidth == 640 ) { sprintf( psz, "%04d%02d%02d %02d:%02d:%02d",  now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec ); }

									if( pVideo->m_nSrcFrameWidth == 352 ) { sprintf( psz, "  %02d%02d%02d %02d:%02d:%02d", now.tm_year % 100, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec ); }

									if( pVideo->m_nSrcFrameWidth == 320 ) { sprintf( psz, "%02d%02d%02d %02d:%02d:%02d", now.tm_year % 100, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec ); }
								}
							}
						}
						if( y == 0 ) { 

							for( j = 0 ; j < 32 ; j++ ) { 

								if( (psz[ j ] >= '0') && (psz[ j ] <= '9') ) { AH8400_SetRegister( pDevice, 3, 0x007E3000 + (j << 2), psz[ j ] - '0' +  1, 2 ); } else

								if( (psz[ j ] >= 'A') && (psz[ j ] <= 'Z') ) { AH8400_SetRegister( pDevice, 3, 0x007E3000 + (j << 2), psz[ j ] - 'A' + 11, 2 ); } else

								if( (psz[ j ] >= 'a') && (psz[ j ] <= 'z') ) { AH8400_SetRegister( pDevice, 3, 0x007E3000 + (j << 2), psz[ j ] - 'a' + 37, 2 ); } else

								if( (psz[ j ] == ':')                      ) { AH8400_SetRegister( pDevice, 3, 0x007E3000 + (j << 2),                  63, 2 ); } else

																			 { AH8400_SetRegister( pDevice, 3, 0x007E3000 + (j << 2),                   0, 2 ); }
							}
						}				
					}
					AH8400_SetRegister( pDevice, 3, 0x007E7000 + 0x000000C0, 0x00000000, 4 );
				}
			}

			//
			//
			{	AH8400_SetAnalogVideoDecoderProcAmpProperties( pDevice, 0 );

				AH8400_SetAnalogVideoDecoderProcAmpProperties( pDevice, 1 );

				AH8400_SetAnalogVideoDecoderProcAmpProperties( pDevice, 2 );

				AH8400_SetAnalogVideoDecoderProcAmpProperties( pDevice, 3 );
			}
			if( (pDevice->m_nDmaBaseCommonBufferNumber[ 3 ] % 30) == 0 ) {

				ULONG j = 0;				
				for( j = 0 ; j < 4 ; j++ ) {

					ULONG i = pDevice->m_nKsDeviceNumber * 4 + j * 1;

					if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000001 ) {

						ULONG R00 = AH8400_GetAnalogVideoDecoderRegister( pDevice, 0x50, (BYTE)((0x10 * j) + 0x00) );

//						g_n_analog_decoder_lock_status[ i ] = ((R00 & 0xC0) == 0x40) ? (1) : (0);
//peter [2011.04.12]
						g_n_analog_decoder_lock_status[ i ] = ((R00 & 0xC0) == 0x40) ? (0) : (0x103);

//						LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_lock_status[%d](0x%x) R00(0x%x)\n", i, g_n_analog_decoder_lock_status[ i ], R00); 

					}
					if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000002 ) {

						ULONG R00 = AH8400_GetAnalogVideoDecoderRegister( pDevice, 0x52, (BYTE)((0x10 * j) + 0x00) );

//						g_n_analog_decoder_lock_status[ i + 4 ] = ((R00 & 0xC0) == 0x40) ? (1) : (0);
//peter [2011.04.12]
						g_n_analog_decoder_lock_status[ i + 4 ] = ((R00 & 0xC0) == 0x40) ? (0) : (0x103);

//						LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_lock_status[%d](0x%x) R00(0x%x)\n", i+4, g_n_analog_decoder_lock_status[ i+4 ], R00); 
					}
//peter [2011.04.15] SC290
					if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000020 ) {

						UCHAR i = (~(AH8400_HPI_READ_BYTE(pDevice, 0x00, 0x0F, 0x00, 10) >> 4)) & 0x0F;;

						g_n_analog_decoder_lock_status[ 0 ] = (i & 0x01) ? (0) : (0x103);
						g_n_analog_decoder_lock_status[ 1 ] = (i & 0x02) ? (0) : (0x103);
						g_n_analog_decoder_lock_status[ 2 ] = (i & 0x04) ? (0) : (0x103);
						g_n_analog_decoder_lock_status[ 3 ] = (i & 0x08) ? (0) : (0x103);
						
//						LINUXV4L2_DEBUG( KERN_INFO, "g_n_analog_decoder_lock_status[] (0x%x)\n", i); 

					}
				}
			}

/*			//
			//
			if( (pDevice->m_nDmaBaseCommonBufferNumber[ 3 ] % 5) == 0 ) {

			if( pDevice->m_nCustomHardwareTopologyProperty & 0x00000010 ) {
			
				ULONG i = (pDevice->m_nKsDeviceNumber - 1) * 4; //

				ULONG j = 0;

				USHORT motion_status = 0x0000;

				AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFA, 0x00, 0x00 );

				motion_status = AH8400_GetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFC );

				for( j = 0 ; j < 16 ; j++ ) {

					BOOLEAN is_update_threshold = FALSE;

					BOOLEAN is_update_mask = FALSE;

					g_n_multiplexer_motion_status[ i + j ] = (motion_status >> j) & 0x01;

					if(	g_n_multiplexer_motion_sensitivity_high_threshold[ i + j ] & 0x80000000 ) { 

						g_n_multiplexer_motion_sensitivity_high_threshold[ i + j ] &= 0x7FFFFFFF;

						is_update_threshold = TRUE;
					}
					if(	g_n_multiplexer_motion_sensitivity_low_threshold[ i + j ] & 0x80000000 ) { 

						g_n_multiplexer_motion_sensitivity_low_threshold[ i + j ] &= 0x7FFFFFFF;

						is_update_threshold = TRUE;
					}
					if(	(g_n_multiplexer_motion_mask_cx[ i + j ] & 0x80000000) ) { 

						g_n_multiplexer_motion_mask_cx[ i + j ] &= 0x7FFFFFFF;

						is_update_mask = TRUE;
					}
					if(	(g_n_multiplexer_motion_mask_cy[ i + j ] & 0x80000000) ) { 

						g_n_multiplexer_motion_mask_cy[ i + j ] &= 0x7FFFFFFF;

						is_update_mask = TRUE;
					}
					if( is_update_threshold || is_update_mask ) {

						AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF0, 0xFF, 0xFF );

						AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xFA, (1 << j) >> 8, (1 << j) & 0xFF );

					//	AH8400_KeStallExecutionProcessor( pDevice, 100000 );

						if( is_update_threshold ) {

							AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0x20, g_n_multiplexer_motion_sensitivity_low_threshold[ i + j ] & 0x7F, g_n_multiplexer_motion_sensitivity_high_threshold[ i + j ] & 0x7F ); 
						}
						if( is_update_mask ) {

							ULONG y = 0;

							for( y = 0 ; y < 24 ; y++ ) {

								BYTE R0328H = 0;
								
								BYTE R0328L = 0;
								
								BYTE R0329H = 0;
								
								BYTE R0329L = 0;

								if( (g_n_multiplexer_motion_mask_cy[ i + j ] >> y) & 0x00000001 ) {

									R0328H = (g_n_multiplexer_motion_mask_cx[ i + j ] >> 23) & 0x0000007F;

									R0328L = (g_n_multiplexer_motion_mask_cx[ i + j ] >> 15) & 0x000000FF;

									R0329H = (g_n_multiplexer_motion_mask_cx[ i + j ] >>  8) & 0x0000007F;

									R0329L = (g_n_multiplexer_motion_mask_cx[ i + j ] >>  0) & 0x000000FF;
								}
								AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0328 + y * 0x02) >> 8, (0x0328 + y * 0x02) & 0xFF, R0328H, R0328L );

								AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0329 + y * 0x02) >> 8, (0x0329 + y * 0x02) & 0xFF, R0329H, R0329L );

								AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0390 + y * 0x02) >> 8, (0x0390 + y * 0x02) & 0xFF, 0x00, 0x00 ); 

								AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, (0x0391 + y * 0x02) >> 8, (0x0391 + y * 0x02) & 0xFF, 0x00, 0x00 ); 

								
							}
						}
						AH8400_SetAnalogVideoMultiplexerRegister( pDevice, 0x1C, 0x03, 0xF0, 0x00, 0x00 );
					}
				}
			}
			}
*/
			// OVERRIDE KEY FRAME
			//
			{	if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 0x00000002 ) {

					pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0x00000003;
				}
				else if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 0x00000003 ) {

					pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0x00000000;
				
					ULONG i = 0;
					for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 0x00008003, 4 ); }

					AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000100, 0x01555500, 4 );

					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] KEY.FRAME QUERY (2)\n", pDevice->m_nKsDeviceNumber );
				}
			}
		}
	}
	if( mask & 0x00000100 ) { //
	
		BOOLEAN is_reset = FALSE;

		if( pVideos[ 16 ] && pVideoBuffers[ 16 ] == NULL && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 1 ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH%02d] [H264] [NULL.STREAM.BUFFER]\n", pDevice->m_nKsDeviceNumber, 0 ); is_reset = TRUE; }
		
		if( pVideos[ 17 ] && pVideoBuffers[ 17 ] == NULL && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 2 ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH%02d] [H264] [NULL.STREAM.BUFFER]\n", pDevice->m_nKsDeviceNumber, 1 ); is_reset = TRUE; }
		
		if( pVideos[ 18 ] && pVideoBuffers[ 18 ] == NULL && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 3 ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH%02d] [H264] [NULL.STREAM.BUFFER]\n", pDevice->m_nKsDeviceNumber, 2 ); is_reset = TRUE; }
		
		if( pVideos[ 19 ] && pVideoBuffers[ 19 ] == NULL && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 4 ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH%02d] [H264] [NULL.STREAM.BUFFER]\n", pDevice->m_nKsDeviceNumber, 3 ); is_reset = TRUE; }

		// OVERRIDE KEY FRAME
		//
		if( is_reset ) {

			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 0x00000000 ) {

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0x00000001;
			}
		}
		if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] > 0 ) {

			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 0x00000001 ) {

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0x00000002;

				ULONG i = 0;
				for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 0x00000000, 4 ); }

				AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000100, 0x00555500, 4 );

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] KEY.FRAME QUERY (1)\n", pDevice->m_nKsDeviceNumber );
			}
		}

		if( pVideos[ 16 ] && pVideoBuffers[ 16 ] && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 1 ) { 

			pVideoBuffers[ 16 ]->m_sKsBuffer.input = pDevice->m_nDmaBaseCommonBufferNumber[ 1 ];

			pVideoBuffers[ 16 ]->m_sKsBuffer.size = pVideos[ 16 ]->m_nSrcFrameSize;

			p_is_completes[ 16 ] = TRUE;

			p_is_drops[ 16 ] = FALSE;
		}
		if( pVideos[ 17 ] && pVideoBuffers[ 17 ] && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 2 ) { 

			pVideoBuffers[ 17 ]->m_sKsBuffer.input = pDevice->m_nDmaBaseCommonBufferNumber[ 1 ];

			pVideoBuffers[ 17 ]->m_sKsBuffer.size = pVideos[ 17 ]->m_nSrcFrameSize;

			p_is_completes[ 17 ] = TRUE;

			p_is_drops[ 17 ] = FALSE;
		}
		if( pVideos[ 18 ] && pVideoBuffers[ 18 ] && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 3 ) { 

			pVideoBuffers[ 18 ]->m_sKsBuffer.input = pDevice->m_nDmaBaseCommonBufferNumber[ 1 ];

			pVideoBuffers[ 18 ]->m_sKsBuffer.size = pVideos[ 18 ]->m_nSrcFrameSize;

			p_is_completes[ 18 ] = TRUE;

			p_is_drops[ 18 ] = FALSE;
		}	
		if( pVideos[ 19 ] && pVideoBuffers[ 19 ] && pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 4 ) { 

			pVideoBuffers[ 19 ]->m_sKsBuffer.input = pDevice->m_nDmaBaseCommonBufferNumber[ 1 ];

			pVideoBuffers[ 19 ]->m_sKsBuffer.size = pVideos[ 19 ]->m_nSrcFrameSize;

			p_is_completes[ 19 ] = TRUE;

			p_is_drops[ 19 ] = FALSE;
		}	
		if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] > 0 ) { //

			ULONG R00780110 = 0x00000000;

			ULONG R007C0010 = 0x00000000;

			ULONG R007C0014 = 0x00000000;

			ULONG i = pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] - 1;

			pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = 0;

			pDevice->m_nDmaBaseCommonBufferNumber[ 1 ]++;

			///////////////////////////////////////////////////// 

			R00780110 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 4 ); R00780110 &= (~0x00010000);

			AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), R00780110, 4 ); 

			{	ULONG R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 &= ~(0x000009C9); R000002A4 &= ~(0x00004000); //

				AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );
			}
			{	ULONG R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000008;

				AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
			}
			////////////////////////////////////////////////////////////

			R007C0010 = AH8400_GetRegister( pDevice, 3, 0x007C0000 + 0x00000010, 4 ); R007C0010 &= (~0x80000000);

			AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000010, R007C0010, 4 );

			R007C0014 = AH8400_GetRegister( pDevice, 3, 0x007C0000 + 0x00000014, 4 ); 
			
			if( (R007C0014 & 0x00070000) == 0x00050000 ) { 
				
				R007C0014 &= (~0x00070000);

				AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000014, R007C0014, 4 );
			}
			/////////////////////////////////////////////////////////////////////////

			{	ULONG R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= (~0x00000002);

				AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
			}
			////////////////////////////////////////////////////////////

			if( (R007C0014 & 0x00070000) != 0x00000000 ) { //
				
				LINUXV4L2_DEBUG( KERN_INFO, "[CH%02ld] [H264] [HARDWARE.RESET R007C0014 = %08lX]\n", pDevice->m_nKsDeviceNumber, R007C0014 ); 

				AH8400_ReStartHDMA( pDevice );

				if( pVideos[ 16 ] && pVideoBuffers[ 16 ] && i == 0 ) { p_is_completes[ 16 ] = FALSE; }

				if( pVideos[ 17 ] && pVideoBuffers[ 17 ] && i == 1 ) { p_is_completes[ 17 ] = FALSE; }

				if( pVideos[ 18 ] && pVideoBuffers[ 18 ] && i == 2 ) { p_is_completes[ 18 ] = FALSE; }

				if( pVideos[ 19 ] && pVideoBuffers[ 19 ] && i == 3 ) { p_is_completes[ 19 ] = FALSE; }

				// OVERRIDE KEY FRAME
				//
				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 0x00000000 ) {

					pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0x00000002;

					ULONG i = 0;
					for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 0x00000000, 4 ); }

					AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000100, 0x00555500, 4 );

					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] KEY.FRAME QUERY (1)\n", pDevice->m_nKsDeviceNumber );
				}
			}
		}
	}
	if( mask & 0x00010000 ) { //

		if( (pVideos[ 16 ]) ||

			(pVideos[ 17 ]) ||

			(pVideos[ 18 ]) ||

			(pVideos[ 19 ]) ) {

			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 0 &&

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] == 0 ) { //

				ULONG R00780030 = 0x00000000;

				ULONG R0078030C = 0x00000000;

				ULONG R00780318 = 0x00000000;
				
				ULONG R00780310 = 0x00000000;
				
				ULONG R00780304 = 0x00000000;

				ULONG R007C0010 = 0x00000000;

				ULONG i = 0;

				R00780030 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000030, 4 );

				for( i = 0 ; i < 4 ; i++ ) {

					ULONG R00780110 = 0x00000000;

					ULONG R00780314 = 0x00000000;

					if( i == 0 ) { if( (R00780030 & 0x00000001) == 0x00000000 ) continue ; }

					if( i == 1 ) { if( (R00780030 & 0x00000004) == 0x00000000 ) continue ; }

					if( i == 2 ) { if( (R00780030 & 0x00000010) == 0x00000000 ) continue ; }

					if( i == 3 ) { if( (R00780030 & 0x00000040) == 0x00000000 ) continue ; }

					R0078030C = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x0000030C + (i * 0x00000040), 4 ); //

					R00780318 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000318 + (i * 0x00000040), 4 ); //

					R00780310 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000310 + (i * 0x00000040), 4 ); //

					///////////////////////////////////////////////////////////////////////////////////////////////

					R00780304 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 4 );

					if( R00780304 & 0x00000002 ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 0x00000002, 4 ); }

					AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 0x00000001, 4 );

					/////////////////////////////////////////////////////////////////////////////////////////

					if( (i == 0) && (pVideos[ 16 ] == NULL) ) { continue ; }

					if( (i == 1) && (pVideos[ 17 ] == NULL) ) { continue ; }

					if( (i == 2) && (pVideos[ 18 ] == NULL) ) { continue ; }

					if( (i == 3) && (pVideos[ 19 ] == NULL) ) { continue ; }

					////////////////////////////////////////////////////////////////////////////////////////////////

					{	R00780110 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 4 ); R00780110 |= 0x00010000;

						AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), R00780110, 4 );

						R00780314 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000314 + (i * 0x00000040), 4 ); 
						
						R00780314 |= 0x10000000 + 0x08000000 * (i / 2); //
					}
					//////////////////////////////////////////////////////////////////

					{	ULONG  memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 2 ];

						BYTE * memory_buffer = pDevice->m_pDmaBaseCommonBuffer[ 2 ];

						ULONG  memory_buffer_size = R00780318 * 4; //

						if( memory_buffer_size >= (0x00096000 - 4) ) { memory_buffer_size = (0x00096000 - 4); }

						switch( pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] % 4 ) { //

						case 0: memory_physical_address += (0x00096000) * 0; memory_buffer += (0x00096000) * 1 - 4; break;

						case 1: memory_physical_address += (0x00096000) * 1; memory_buffer += (0x00096000) * 2 - 4; break;

						case 2: memory_physical_address += (0x00096000) * 2; memory_buffer += (0x00096000) * 3 - 4; break;

						case 3: memory_physical_address += (0x00096000) * 3; memory_buffer += (0x00096000) * 4 - 4; break;
						}
						if( i == 0 ) { pVideos[ 16 ]->m_nSrcFrameSize = memory_buffer_size; }

						if( i == 1 ) { pVideos[ 17 ]->m_nSrcFrameSize = memory_buffer_size; }

						if( i == 2 ) { pVideos[ 18 ]->m_nSrcFrameSize = memory_buffer_size; }

						if( i == 3 ) { pVideos[ 19 ]->m_nSrcFrameSize = memory_buffer_size; }

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = (i + 1);

						memory_buffer[ 0 ] = (BYTE)((R00780310 & 0x000000FF) >>  0); //

						memory_buffer[ 1 ] = (BYTE)((R00780310 & 0x0000FF00) >>  8);

						memory_buffer[ 2 ] = (BYTE)((R00780310 & 0x00FF0000) >> 16);

						memory_buffer[ 3 ] = (BYTE)((R00780310 & 0xFF000000) >> 24);

						memory_buffer[ 2 ] = (R00780310 & 0x00200000) ? 0x40 : 0x00; //

						// 動態屬性更新 [缺]

						//////////////////////////////////////////////////////////////////////////////////////////

						R007C0010 = AH8400_GetRegister( pDevice, 3, 0x007C0000 + 0x00000010, 4 ); //

						AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000018, R00780314, 4 ); //

						AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x0000001C, 0x00800000, 4 );

						R007C0010 |= 0x80000000;

						R007C0010 &= 0xE0FFFFFF;

						R007C0010 |= 0x0E000000;

						R007C0010 &= 0xFFF0FFFF;

						R007C0010 &= 0xFF0FFFFF;

						R007C0010 |= 0x00D00000;

						R007C0010 &= 0xFFFF0000;

						R007C0010 |= (R00780318 + 7) / (8) - (1);

						AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000010, R007C0010, 4 );

						{	ULONG R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x000009C9; R000002A4 &= ~(0x00004000);

							AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );
						}
						{	ULONG R00000140 = 0x00000000;

							AH8400_SetRegister( pDevice, 2, 0x00000144, memory_physical_address, 4 );

							AH8400_SetRegister( pDevice, 2, 0x00000148, memory_buffer_size, 4 );

							R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000F00); R00000140 |= 0x00030701;

							AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
						}
						{	ULONG R00000140 = 0x00000000; 
							
							R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000002;

							AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
						}
					}
					break;
				}
			}
			else {

				ULONG R00780030 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000030, 4 );

				ULONG is_reset = 0x00000000;

				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 1 && (R00780030 & 0x00000001) ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH00] [H264] [SOFTWARE.RESET R00780030 = %08lX]\n", pDevice->m_nKsDeviceNumber, R00780030 ); is_reset = 0x00000001; }

				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 2 && (R00780030 & 0x00000004) ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH01] [H264] [SOFTWARE.RESET R00780030 = %08lX]\n", pDevice->m_nKsDeviceNumber, R00780030 ); is_reset = 0x00000002; }

				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 3 && (R00780030 & 0x00000010))  { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH02] [H264] [SOFTWARE.RESET R00780030 = %08lX]\n", pDevice->m_nKsDeviceNumber, R00780030 ); is_reset = 0x00000003; }

				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 4 && (R00780030 & 0x00000040) ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH03] [H264] [SOFTWARE.RESET R00780030 = %08lX]\n", pDevice->m_nKsDeviceNumber, R00780030 ); is_reset = 0x00000004; }

				if( is_reset ) {
					
					ULONG i = 0;

					pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = 0;

					for( i = 0 ; i < 4 ; i++ ) {

						ULONG R00780304 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 4 );

						if( R00780304 & 0x00000002 ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 0x00000002, 4 ); }

						AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 0x00000001, 4 );
					}
					AH8400_ReStartHDMA( pDevice );

					// OVERRIDE KEY FRAME
					// 
					if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 0x00000000 ) {

						pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0x00000002;

						ULONG i = 0;
						for( i = 0 ; i < 4 ; i++ ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000110 + (i * 0x00000020), 0x00000000, 4 ); }

						AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000100, 0x00555500, 4 );

						LINUXV4L2_DEBUG( KERN_INFO, "[%02d] KEY.FRAME QUERY (1)\n", pDevice->m_nKsDeviceNumber );
					}
				}
			}
		}
		else {

			ULONG R00780030 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000030, 4 );

			ULONG R00780304 = 0x00000000;

			ULONG i = 0;

			for( i = 0 ; i < 4 ; i++ ) {

				if( i == 0 ) { if( (R00780030 & 0x00000001) == 0x00000000 ) continue ; } 

				if( i == 1 ) { if( (R00780030 & 0x00000004) == 0x00000000 ) continue ; }

				if( i == 2 ) { if( (R00780030 & 0x00000010) == 0x00000000 ) continue ; }

				if( i == 3 ) { if( (R00780030 & 0x00000040) == 0x00000000 ) continue ; }

				R00780304 = AH8400_GetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 4 );

				if( R00780304 & 0x00000002 ) { AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 0x00000002, 4 ); }

				AH8400_SetRegister( pDevice, 3, 0x00780000 + 0x00000304 + (i * 0x00000040), 0x00000001, 4 );
			}
		}
	}
	return TRUE;
}
/////// G271 start/////////////////////////////////////////////////////////////////////////////////////////////////////////////

const short power2[15] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80,
			0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000};

static short qtab_721[7] = {-124, 80, 178, 246, 300, 349, 400};
/*
 * Maps G.721 code word to reconstructed scale factor normalized log
 * magnitude values.
 */
static short	_dqlntab[16] = {-2048, 4, 135, 213, 273, 323, 373, 425,
				425, 373, 323, 273, 213, 135, 4, -2048};

/* Maps G.721 code word to log of scale factor multiplier. */
static short	_witab[16] = {-12, 18, 41, 64, 112, 198, 355, 1122,
				1122, 355, 198, 112, 64, 41, 18, -12};
/*
 * Maps G.721 code words to a set of values whose long and short
 * term averages are computed and then compared to give an indication
 * how stationary (steady state) the signal is.
 */
static short	_fitab[16] = {0, 0, 0, 0x200, 0x200, 0x200, 0x600, 0xE00,
				0xE00, 0x600, 0x200, 0x200, 0x200, 0, 0, 0};

int g721_decoder(int i, int out_coding, struct g72x_state* state_ptr);
int g721_encoder(int sl, int in_coding,struct g72x_state* state_ptr);


/*
 * g72x_init_state()
 *
 * This routine initializes and/or resets the g72x_state structure
 * pointed to by 'state_ptr'.
 * All the initial state values are specified in the CCITT G.721 document.
 */
void g72x_init_state (struct g72x_state *state_ptr)
{
	int		cnta;

	state_ptr->yl = 34816;
	state_ptr->yu = 544;
	state_ptr->dms = 0;
	state_ptr->dml = 0;
	state_ptr->ap = 0;
	for (cnta = 0; cnta < 2; cnta++) {
		state_ptr->a[cnta] = 0;
		state_ptr->pk[cnta] = 0;
		state_ptr->sr[cnta] = 32;
	}
	for (cnta = 0; cnta < 6; cnta++) {
		state_ptr->b[cnta] = 0;
		state_ptr->dq[cnta] = 32;
	}
	state_ptr->td = 0;
}


int Init(int ch)
{
//	g72x_init_state (&m_encState);
//	g72x_init_state (&m_decState);
	
	g72x_init_state( &(CG721[ ch ].m_encState) );
	g72x_init_state( &(CG721[ ch ].m_decState) );

	return 0;
}


int Encode (short *bufOrgData, int lenOrgData, unsigned char *bufEncCode, int lenEncData, int ch)
{
	if (bufEncCode == NULL)
	{
		return lenOrgData *3 /8;
	}
	if (lenEncData < (lenOrgData *3 /8))
	{
		return -1;
	}

//	unsigned char *bufEachEncCode = new unsigned char[lenOrgData];

	unsigned char bufEachEncCode[ 1024 ];

	int lenEncCode;

	int i, j;
	for (i=0; i<lenOrgData; i++)
	{
		bufEachEncCode[i] = (unsigned char)g721_encoder (bufOrgData[i], AUDIO_ENCODING_LINEAR, &(CG721[ ch ].m_encState));
	}

	lenEncCode = lenOrgData *3 /8;

	for (i=0, j=0; i<lenEncCode /3 *3; i+=3, j+=8)
	{
#ifdef _AUDIO_LITTLE_ENDIAN
		bufEncCode[i] = bufEachEncCode[j +0] +(unsigned char)(bufEachEncCode[j +1] <<3) +(unsigned char)((bufEachEncCode[j +2] &0x3) <<6);
		bufEncCode[i +1] = (bufEachEncCode[j +2] >>2) +(unsigned char)(bufEachEncCode[j +3] <<1)+(unsigned char)(bufEachEncCode[j +4] <<4) +(unsigned char)((bufEachEncCode[j +5] &0x1) <<7);
		bufEncCode[i +2] = (bufEachEncCode[j +5] >>1) +(unsigned char)(bufEachEncCode[j +6] <<2) +(unsigned char)(bufEachEncCode[j +7] <<5);
#else
		bufEncCode[i] = (bufEachEncCode[j +0] <<5) +(bufEachEncCode[j +1] <<2) +(bufEachEncCode[j +2] >>1);
		bufEncCode[i +1] = ((bufEachEncCode[j +2] &0x1) <<7) +(bufEachEncCode[j +3] <<4)
			+(bufEachEncCode[j +4] <<1) +(bufEachEncCode[j +5] >>2);
		bufEncCode[i +2] = ((bufEachEncCode[j +5] &0x3) <<6) +(bufEachEncCode[j +6] <<3) +(bufEachEncCode[j +7]);
#endif
	}

//	delete[] bufEachEncCode;

	return lenEncCode;
}

static int quan (int val, const short *table, int size);
static int fmult (int an, int srn);
static int predictor_zero (struct g72x_state *state_ptr);
static int predictor_pole (struct g72x_state *state_ptr);
static int step_size (struct g72x_state *state_ptr);
static int quantize (int d,		/* Raw difference signal sample */
			  int y,		/* Step size multiplier */
			  const short *table,	/* quantization table */
			  int size);	/* table size of short integers */
static int reconstruct (int sign,	/* 0 for non-negative value */
				 int dqln,	/* G.72x codeword */
				 int y);	/* Step size multiplier */
static void update (int code_size,	/* distinguish 723_40 with others */
			 int y,			/* quantizer step size */
			 int wi,		/* scale factor multiplier */
			 int fi,		/* for long/short term energies */
			 int dq,		/* quantized prediction difference */
			 int sr,		/* reconstructed signal */
			 int dqsez,		/* difference from 2-pole predictor */
			 struct g72x_state *state_ptr);	/* coder state pointer */

/*
 * g721_encoder()
 *
 * Encodes the input vale of linear PCM, A-law or u-law data sl and returns
 * the resulting code. -1 is returned for unknown input coding value.
 */
int
g721_encoder(int sl, int in_coding,struct g72x_state* state_ptr)
{
	short		sezi, se, sez;		/* ACCUM */
	short		d;			/* SUBTA */
	short		sr;			/* ADDB */
	short		y;			/* MIX */
	short		dqsez;			/* ADDC */
	short		dq, i;

	switch (in_coding) {	/* linearize input sample to 14-bit PCM */
	case AUDIO_ENCODING_LINEAR:
		sl >>= 2;			/* 14-bit dynamic range */
		break;
	default:
		return (-1);
	}

	sezi = (short)(predictor_zero(state_ptr));
	sez = sezi >> 1;
	se = (sezi + (short)(predictor_pole(state_ptr))) >> 1;	/* estimated signal */

	d = sl - se;				/* estimation difference */

	/* quantize the prediction difference */
	y = (short)(step_size(state_ptr));		/* quantizer step size */
	i = (short)(quantize(d, y, qtab_721, 7));	/* i = ADPCM code */

	dq = (short)(reconstruct(i & 8, _dqlntab[i], y));	/* quantized est diff */

	sr = (dq < 0) ? se - (dq & 0x3FFF) : se + dq;	/* reconst. signal */

	dqsez = sr + sez - se;			/* pole prediction diff. */

	update(4, y, _witab[i] << 5, _fitab[i], dq, sr, dqsez, state_ptr);

	return (i);
}

/*
 * g721_decoder()
 *
 * Description:
 *
 * Decodes a 4-bit code of G.721 encoded data of i and
 * returns the resulting linear PCM, A-law or u-law value.
 * return -1 for unknown out_coding value.
 */
int
g721_decoder(int i, int out_coding, struct g72x_state* state_ptr)
{
	short		sezi, sei, sez, se;	/* ACCUM */
	short		y;			/* MIX */
	short		sr;			/* ADDB */
	short		dq;
	short		dqsez;

	i &= 0x0f;			/* mask to get proper bits */
	sezi = (short)(predictor_zero(state_ptr));
	sez = sezi >> 1;
	sei = sezi + (short)(predictor_pole(state_ptr));
	se = sei >> 1;			/* se = estimated signal */

	y = (short)(step_size(state_ptr));	/* dynamic quantizer step size */

	dq = (short)(reconstruct(i & 0x08, _dqlntab[i], y)); /* quantized diff. */

	sr = (dq < 0) ? (se - (dq & 0x3FFF)) : se + dq;	/* reconst. signal */

	dqsez = sr - se + sez;			/* pole prediction diff. */

	update(4, y, _witab[i] << 5, _fitab[i], dq, sr, dqsez, state_ptr);

	switch (out_coding) {
	(sr, se, y, i, 8, qtab_721);
	case AUDIO_ENCODING_LINEAR:
		return (sr << 2);	/* sr was 14-bit dynamic range */
	default:
		return (-1);
	}
}

static int quan (int val, const short *table, int size)
{
	int		i;

	for (i = 0; i < size; i++)
		if (val < *table++)
			break;
	return (i);
}

/*
 * fmult()
 *
 * returns the integer product of the 14-bit integer "an" and
 * "floating point" representation (4-bit exponent, 6-bit mantessa) "srn".
 */
static int fmult (int an, int srn)
{
	short		anmag, anexp, anmant;
	short		wanexp, wanmant;
	short		retval;

	anmag = (an > 0) ? an : ((-an) & 0x1FFF);
	anexp = quan(anmag, power2, 15) - 6;
	anmant = (anmag == 0) ? 32 :
	    (anexp >= 0) ? anmag >> anexp : anmag << -anexp;
	wanexp = anexp + ((srn >> 6) & 0xF) - 13;

	wanmant = (anmant * (srn & 077) + 0x30) >> 4;
	retval = (wanexp >= 0) ? ((wanmant << wanexp) & 0x7FFF) :
	    (wanmant >> -wanexp);

	return (((an ^ srn) < 0) ? -retval : retval);
}

/*
 * g72x_init_state()
 *
 * This routine initializes and/or resets the g72x_state structure
 * pointed to by 'state_ptr'.
 * All the initial state values are specified in the CCITT G.721 document.
 */
/*
void g72x_init_state (struct g72x_state *state_ptr)
{
	int		cnta;

	state_ptr->yl = 34816;
	state_ptr->yu = 544;
	state_ptr->dms = 0;
	state_ptr->dml = 0;
	state_ptr->ap = 0;
	for (cnta = 0; cnta < 2; cnta++) {
		state_ptr->a[cnta] = 0;
		state_ptr->pk[cnta] = 0;
		state_ptr->sr[cnta] = 32;
	}
	for (cnta = 0; cnta < 6; cnta++) {
		state_ptr->b[cnta] = 0;
		state_ptr->dq[cnta] = 32;
	}
	state_ptr->td = 0;
}
*/
/*
 * predictor_zero()
 *
 * computes the estimated signal from 6-zero predictor.
 *
 */
int predictor_zero (struct g72x_state *state_ptr)
{
	int		i;
	int		sezi;

	sezi = fmult(state_ptr->b[0] >> 2, state_ptr->dq[0]);
	for (i = 1; i < 6; i++)			/* ACCUM */
		sezi += fmult(state_ptr->b[i] >> 2, state_ptr->dq[i]);
	return (sezi);
}
/*
 * predictor_pole()
 *
 * computes the estimated signal from 2-pole predictor.
 *
 */
int predictor_pole (struct g72x_state *state_ptr)
{
	return (fmult(state_ptr->a[1] >> 2, state_ptr->sr[1]) +
	    fmult(state_ptr->a[0] >> 2, state_ptr->sr[0]));
}
/*
 * step_size()
 *
 * computes the quantization step size of the adaptive quantizer.
 *
 */
int step_size (struct g72x_state *state_ptr)
{
	int		y;
	int		dif;
	int		al;

	if (state_ptr->ap >= 256)
		return (state_ptr->yu);
	else {
		y = state_ptr->yl >> 6;
		dif = state_ptr->yu - y;
		al = state_ptr->ap >> 2;
		if (dif > 0)
			y += (dif * al) >> 6;
		else if (dif < 0)
			y += (dif * al + 0x3F) >> 6;
		return (y);
	}
}

/*
 * quantize()
 *
 * Given a raw sample, 'd', of the difference signal and a
 * quantization step size scale factor, 'y', this routine returns the
 * ADPCM codeword to which that sample gets quantized.  The step
 * size scale factor division operation is done in the log base 2 domain
 * as a subtraction.
 */
int quantize (int d,		/* Raw difference signal sample */
			  int y,		/* Step size multiplier */
			  const short *table,	/* quantization table */
			  int size)		/* table size of short integers */
{
	short		dqm;	/* Magnitude of 'd' */
	short		exp;	/* Integer part of base 2 log of 'd' */
	short		mant;	/* Fractional part of base 2 log */
	short		dl;	/* Log of magnitude of 'd' */
	short		dln;	/* Step size scale factor normalized log */
	int		i;

	/*
	 * LOG
	 *
	 * Compute base 2 log of 'd', and store in 'dl'.
	 */
	dqm = (short)(abs(d));
	exp = (short)(quan(dqm >> 1, power2, 15));
	mant = ((dqm << 7) >> exp) & 0x7F;	/* Fractional portion. */
	dl = (exp << 7) + mant;

	/*
	 * SUBTB
	 *
	 * "Divide" by step size multiplier.
	 */
	dln = dl - (y >> 2);

	/*
	 * QUAN
	 *
	 * Obtain codword i for 'd'.
	 */
	i = quan(dln, table, size);
	if (d < 0)			/* take 1's complement of i */
		return ((size << 1) + 1 - i);
	else if (i == 0)		/* take 1's complement of 0 */
		return ((size << 1) + 1); /* new in 1988 */
	else
		return (i);
}
/*
 * reconstruct()
 *
 * Returns reconstructed difference signal 'dq' obtained from
 * codeword 'i' and quantization step size scale factor 'y'.
 * Multiplication is performed in log base 2 domain as addition.
 */
int reconstruct (int sign,	/* 0 for non-negative value */
				 int dqln,	/* G.72x codeword */
				 int y)		/* Step size multiplier */
{
	short		dql;	/* Log of 'dq' magnitude */
	short		dex;	/* Integer part of log */
	short		dqt;
	short		dq;	/* Reconstructed difference signal sample */

	dql = dqln + (y >> 2);	/* ADDA */

	if (dql < 0) {
		return ((sign) ? -0x8000 : 0);
	} else {		/* ANTILOG */
		dex = (dql >> 7) & 15;
		dqt = 128 + (dql & 127);
		dq = (dqt << 7) >> (14 - dex);
		return ((sign) ? (dq - 0x8000) : dq);
	}
}


/*
 * update()
 *
 * updates the state variables for each output code
 */
void update (int code_size,	/* distinguish 723_40 with others */
			 int y,			/* quantizer step size */
			 int wi,		/* scale factor multiplier */
			 int fi,		/* for long/short term energies */
			 int dq,		/* quantized prediction difference */
			 int sr,		/* reconstructed signal */
			 int dqsez,		/* difference from 2-pole predictor */
			 struct g72x_state *state_ptr)	/* coder state pointer */
{
	int		cnt;
	short		mag, exp;	/* Adaptive predictor, FLOAT A */
	short		a2p;		/* LIMC */
	short		a1ul;		/* UPA1 */
	short		pks1;	/* UPA2 */
	short		fa1;
	char		tr;		/* tone/transition detector */
	short		ylint, thr2, dqthr;
	short  		ylfrac, thr1;
	short		pk0;

	pk0 = (dqsez < 0) ? 1 : 0;	/* needed in updating predictor poles */

	mag = dq & 0x7FFF;		/* prediction difference magnitude */
	/* TRANS */
	ylint = (short)(state_ptr->yl >> 15);	/* exponent part of yl */
	ylfrac = (short)((state_ptr->yl >> 10) & 0x1F);	/* fractional part of yl */
	thr1 = (32 + ylfrac) << ylint;		/* threshold */
	thr2 = (short)((ylint > 9) ? 31 << 10 : thr1);	/* limit thr2 to 31 << 10 */
	dqthr = (thr2 + (thr2 >> 1)) >> 1;	/* dqthr = 0.75 * thr2 */
	if (state_ptr->td == 0)		/* signal supposed voice */
		tr = 0;
	else if (mag <= dqthr)		/* supposed data, but small mag */
		tr = 0;			/* treated as voice */
	else				/* signal is data (modem) */
		tr = 1;

	/*
	 * Quantizer scale factor adaptation.
	 */

	/* FUNCTW & FILTD & DELAY */
	/* update non-steady state step size multiplier */
	state_ptr->yu = y + ((wi - y) >> 5);

	/* LIMB */
	if (state_ptr->yu < 544)	/* 544 <= yu <= 5120 */
		state_ptr->yu = 544;
	else if (state_ptr->yu > 5120)
		state_ptr->yu = 5120;

	/* FILTE & DELAY */
	/* update steady state step size multiplier */
	state_ptr->yl += state_ptr->yu + ((-state_ptr->yl) >> 6);

	/*
	 * Adaptive predictor coefficients.
	 */
	if (tr == 1) {			/* reset a's and b's for modem signal */
		state_ptr->a[0] = 0;
		state_ptr->a[1] = 0;
		state_ptr->b[0] = 0;
		state_ptr->b[1] = 0;
		state_ptr->b[2] = 0;
		state_ptr->b[3] = 0;
		state_ptr->b[4] = 0;
		state_ptr->b[5] = 0;
	} else {			/* update a's and b's */
		pks1 = pk0 ^ state_ptr->pk[0];		/* UPA2 */

		/* update predictor pole a[1] */
		a2p = state_ptr->a[1] - (state_ptr->a[1] >> 7);
		if (dqsez != 0) {
			fa1 = (pks1) ? state_ptr->a[0] : -state_ptr->a[0];
			if (fa1 < -8191)	/* a2p = function of fa1 */
				a2p -= 0x100;
			else if (fa1 > 8191)
				a2p += 0xFF;
			else
				a2p += fa1 >> 5;

			if (pk0 ^ state_ptr->pk[1])
				/* LIMC */
				if (a2p <= -12160)
					a2p = -12288;
				else if (a2p >= 12416)
					a2p = 12288;
				else
					a2p -= 0x80;
			else if (a2p <= -12416)
				a2p = -12288;
			else if (a2p >= 12160)
				a2p = 12288;
			else
				a2p += 0x80;
		}

		/* TRIGB & DELAY */
		state_ptr->a[1] = a2p;

		/* UPA1 */
		/* update predictor pole a[0] */
		state_ptr->a[0] -= state_ptr->a[0] >> 8;
		if (dqsez != 0)
			if (pks1 == 0)
				state_ptr->a[0] += 192;
			else
				state_ptr->a[0] -= 192;

		/* LIMD */
		a1ul = 15360 - a2p;
		if (state_ptr->a[0] < -a1ul)
			state_ptr->a[0] = -a1ul;
		else if (state_ptr->a[0] > a1ul)
			state_ptr->a[0] = a1ul;

		/* UPB : update predictor zeros b[6] */
		for (cnt = 0; cnt < 6; cnt++) {
			if (code_size == 5)		/* for 40Kbps G.723 */
				state_ptr->b[cnt] -= state_ptr->b[cnt] >> 9;
			else			/* for G.721 and 24Kbps G.723 */
				state_ptr->b[cnt] -= state_ptr->b[cnt] >> 8;
			if (dq & 0x7FFF) {			/* XOR */
				if ((dq ^ state_ptr->dq[cnt]) >= 0)
					state_ptr->b[cnt] += 128;
				else
					state_ptr->b[cnt] -= 128;
			}
		}
	}

	for (cnt = 5; cnt > 0; cnt--)
		state_ptr->dq[cnt] = state_ptr->dq[cnt-1];
	/* FLOAT A : convert dq[0] to 4-bit exp, 6-bit mantissa f.p. */
	if (mag == 0) {
		state_ptr->dq[0] = (dq >= 0) ? 0x20 : 0xFC20;
	} else {
		exp = (short)(quan(mag, power2, 15));
		state_ptr->dq[0] = (dq >= 0) ?
		    (exp << 6) + ((mag << 6) >> exp) :
		    (exp << 6) + ((mag << 6) >> exp) - 0x400;
	}

	state_ptr->sr[1] = state_ptr->sr[0];
	/* FLOAT B : convert sr to 4-bit exp., 6-bit mantissa f.p. */
	if (sr == 0) {
		state_ptr->sr[0] = 0x20;
	} else if (sr > 0) {
		exp = (short)(quan(sr, power2, 15));
		state_ptr->sr[0] = (exp << 6) + ((sr << 6) >> exp);
	} else if (sr > -32768) {
		mag = -sr;
		exp = (short)(quan(mag, power2, 15));
		state_ptr->sr[0] =  (exp << 6) + ((mag << 6) >> exp) - 0x400;
	} else
		state_ptr->sr[0] = (short)(0xFC20);

	/* DELAY A */
	state_ptr->pk[1] = state_ptr->pk[0];
	state_ptr->pk[0] = pk0;

	/* TONE */
	if (tr == 1)		/* this sample has been treated as data */
		state_ptr->td = 0;	/* next one will be treated as voice */
	else if (a2p < -11776)	/* small sample-to-sample correlation */
		state_ptr->td = 1;	/* signal may be data */
	else				/* signal is voice */
		state_ptr->td = 0;

	/*
	 * Adaptation speed control.
	 */
	state_ptr->dms += (fi - state_ptr->dms) >> 5;		/* FILTA */
	state_ptr->dml += (((fi << 2) - state_ptr->dml) >> 7);	/* FILTB */

	if (tr == 1)
		state_ptr->ap = 256;
	else if (y < 1536)					/* SUBTC */
		state_ptr->ap += (0x200 - state_ptr->ap) >> 4;
	else if (state_ptr->td == 1)
		state_ptr->ap += (0x200 - state_ptr->ap) >> 4;
	else if (abs((state_ptr->dms << 2) - state_ptr->dml) >=
	    (state_ptr->dml >> 3))
		state_ptr->ap += (0x200 - state_ptr->ap) >> 4;
	else
		state_ptr->ap += (-state_ptr->ap) >> 4;
}
int Decode (unsigned char *bufEncCode, int lenEncCode, short *bufDecData, int lenDecData, int nChannels, int ch)
{
	if (bufDecData == NULL)
	{
		return lenEncCode * 2;
	}

//	if (lenDecData < (lenEncCode * 2))
//	{
//		return -1;
//	}

	int szEachEncCode = (lenEncCode * 2);

//	unsigned char *bufEachEncCode = new unsigned char[szEachEncCode];

	unsigned char bufEachEncCode[ 160 ];

	int i, j;
	for (i=0, j=0; i<lenEncCode /2 *2; i+=1, j+=2)
	{
#ifdef _AUDIO_LITTLE_ENDIAN
		bufEachEncCode[j +0] = ((bufEncCode[i + 0] >>0) &0xf);
		bufEachEncCode[j +1] = ((bufEncCode[i + 0] >>4) &0xf);
#else
		bufEachEncCode[j +0] = ((bufEncCode[i] >>5) &0x7);
		bufEachEncCode[j +1] = ((bufEncCode[i] >>2) &0x7);
		bufEachEncCode[j +2] = (((bufEncCode[i +1] >>7) &0x1) <<0) +((bufEncCode[i] &0x3) <<1);
		bufEachEncCode[j +3] = ((bufEncCode[i +1] >>4) &0x7);
		bufEachEncCode[j +4] = ((bufEncCode[i +1] >>1) &0x7);
		bufEachEncCode[j +5] = (((bufEncCode[i +2] >>6) &0x3) <<0) +((bufEncCode[i +1] &0x1) <<2);
		bufEachEncCode[j +6] = ((bufEncCode[i +2] >>3) &0x7);
		bufEachEncCode[j +7] = ((bufEncCode[i +2] >>0) &0x7);
#endif
	}

	short decCode;
	for (i=0; i<szEachEncCode; i++)
	{
		decCode = (short)g721_decoder (bufEachEncCode[i], AUDIO_ENCODING_LINEAR, &(CG721[ ch ].m_decState));
		
		if( nChannels == 1 ) { bufDecData[ i * 1 ] = decCode; } // [OTHER]

		if( nChannels == 2 ) { bufDecData[ i * 2 ] = decCode; } // [COBAN]
	}

//	delete[] bufEachEncCode;

	return szEachEncCode;
}

/////// G271 end/////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOLEAN AH8400_HwProcessAnalogPCIAudioPacket( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{

	BYTE *  po = NULL;

	BYTE *  pe = NULL;

	CAudio * pAudio = NULL;

	ULONG sz = 80;

	ULONG i = 0;

	if( mask & 0x00000100 ) { //

		if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] > 0 ) {

			ULONG R007C0010 = 0x00000000;

			ULONG R007C0014 = 0x00000000;

			pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 0;

			{	ULONG R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 &= ~(0x000009C9); R000002A4 &= ~(0x00004000);// -----> AHB_SlaveEnable() [2009.02.24

				AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );
			}
			{	ULONG R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000008;

				AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
			}
			R007C0010 = AH8400_GetRegister( pDevice, 3, 0x007C0000 + 0x00000010, 4 ); R007C0010 &= (~0x80000000);

			AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000010, R007C0010, 4 );

			R007C0014 = AH8400_GetRegister( pDevice, 3, 0x007C0000 + 0x00000014, 4 ); 
			
			if( (R007C0014 & 0x00070000) == 0x00050000 ) { 
				
				R007C0014 &= (~0x00070000);

				AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000014, R007C0014, 4 );
			}
			{	ULONG R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= (~0x00000002);

				AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
			}
			if( (R007C0014 & 0x00070000) != 0x00000000 ) {
				
				LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH0X] [G721] [HARDWARE.RESET R007C0014 = %08lX]\n", pDevice->m_nKsDeviceNumber, R007C0014 ); 

				AH8400_ReStartHDMA( pDevice );
			}


			for(i = 0; i < 4; i++)
			{

				if( pAudios[ i ] ) {

					pAudio = pAudios[ i ];

					if(pAudio->m_pKsSubStream)
					{
						if(pAudio->m_pKsSubStream->runtime)
						{
							if(pAudio->m_pKsSubStream->runtime->dma_area)
							{
								pe = pAudio->m_pKsSubStream->runtime->dma_area;				
							}
							else
							{
								goto EXIT;
							}
						}
						else
						{
							goto EXIT;
						}
					}
					else
					{
						goto EXIT;
					}

					po = pDevice->m_pDmaBaseCommonBuffer[ 1 ] + ( pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] % 4) * 1280 + 80 * ( i % 4 );


		
					if( pe ) {
			
						pe += pAudio->m_nFrameNumber * pAudio->m_nFrameSize;

//						memcpy(pe, po, sz);

						Decode( (BYTE *) po , 80, (SHORT *) pe, 160, 1, i + (pDevice->m_nKsDeviceNumber) * 4 );// 1 is mono 
			
						p_is_drops[ i ] = FALSE;

						p_is_completes[ i ] = TRUE;

					}
					else
					{
						goto EXIT;
					}

				}
			}

			pDevice->m_nDmaBaseCommonBufferNumber[ 2 ]++;

			pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] %= 4;


		}

	}	
		
		
	if( mask & 0x00020000 ) {

		if( (pAudios[  0 ]) ||

			(pAudios[  1 ]) ||

			(pAudios[  2 ]) ||

			(pAudios[  3 ]) ||

			(pAudios[  4 ]) ||

			(pAudios[  5 ]) ||

			(pAudios[  6 ]) ||

			(pAudios[  7 ]) ||

			(pAudios[  8 ]) ||

			(pAudios[  9 ]) ||

			(pAudios[ 10 ]) ||

			(pAudios[ 11 ]) ||

			(pAudios[ 12 ]) ||

			(pAudios[ 13 ]) ||

			(pAudios[ 14 ]) ||

			(pAudios[ 15 ]) ) {
//1 is H.264, 2 is audio					
			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] == 0 &&  

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] == 0 ) {

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 1;

				ULONG R007A0094 = 0x00000000;

				R007A0094 = AH8400_GetRegister( pDevice, 3, 0x007A0000 + 0x00000094, 4 );
				
				AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000094, 0x0000FFFF, 4 );

				{	ULONG R007C0010 = AH8400_GetRegister( pDevice, 3, 0x007C0000 + 0x00000010, 4 );

					AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000018, 0x7F800000 + 0x007B0000, 4 );

					AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x0000001C, 0x00800000, 4 );

					R007C0010 |= 0x80000000;

					R007C0010 &= 0xE0FFFFFF;

					R007C0010 |= 0x0E000000;

					R007C0010 &= 0xFFF0FFFF;

					R007C0010 &= 0xFF0FFFFF;

					R007C0010 |= 0x00D00000;

					R007C0010 &= 0xFFFF0000;

					R007C0010 |= (1280 / 4 + 7) / (8) - (1);

					AH8400_SetRegister( pDevice, 3, 0x007C0000 + 0x00000010, R007C0010, 4 );

					{	ULONG R000002A4 = AH8400_GetRegister( pDevice, 2, 0x000002A4, 4 ); R000002A4 |= 0x000009C9; R000002A4 &= ~(0x00004000);

						AH8400_SetRegister( pDevice, 2, 0x000002A4, R000002A4, 4 );
					}
					{	ULONG R00000140 = 0x00000000;
						
						ULONG memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];
						
						switch( pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] % 4 ) {

						case 0: memory_physical_address += (1280) * 0; break;

						case 1: memory_physical_address += (1280) * 1; break;

						case 2: memory_physical_address += (1280) * 2; break;

						case 3: memory_physical_address += (1280) * 3; break;
						}
						AH8400_SetRegister( pDevice, 2, 0x00000144, memory_physical_address, 4 );

						AH8400_SetRegister( pDevice, 2, 0x00000148, 1280, 4 );

						R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 &= ~(0x00000F00); R00000140 |= 0x00030701;

						AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
					}
					{	ULONG R00000140 = AH8400_GetRegister( pDevice, 2, 0x00000140, 4 ); R00000140 |= 0x00000002;

						AH8400_SetRegister( pDevice, 2, 0x00000140, R00000140, 4 );
					}
				}

			}
			else {

				ULONG is_reset = 0x00000000;

				if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] == 1 ) { LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] [CH0X] [G721] [SOFTWARE.RESET R00000000 = %08X]\n", pDevice->m_nKsDeviceNumber, 0x00000000 ); is_reset = 0x00000001; }

				if( is_reset ) {
					
					pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 0;

					AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000094, 0x0000FFFF, 4 );

					AH8400_ReStartHDMA( pDevice );
				}
			}
		}
		else {

			AH8400_SetRegister( pDevice, 3, 0x007A0000 + 0x00000094, 0x0000FFFF, 4 );
		}
	}
	
	return TRUE;

EXIT:

	return FALSE;
}

// shawn 2010.10.13
//
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)

BOOLEAN COPY_LINE( BYTE ** s_po, BYTE ** s_pe, struct scatterlist ** s_p_sglist, ULONG cxo, ULONG cwo, ULONG cwe, ULONG * byte_count, ULONG * count, ULONG mapping_count, ULONG offset_o, ULONG offset_e ){

	//	copy_to_user( pe, po, cxo ); == COPY LINE
	//
	//	po += cwo; == SKIP PITCH (SRC)
	//
	//	pe += cwe; == SKIP PITCH (DST)

	BYTE * po = *s_po;

	BYTE * pe = *s_pe;

	ULONG remains = 0;

	ULONG i = 0;

	ULONG copy = (cxo > 360) ? (2) : (1);	// IF cxo > 360 , COPY 1 LINE, OR COPY HALF LINE

	struct scatterlist * p_sglist = *s_p_sglist;

	for( i = 0; i < copy; i++ ) {

		// COPY HALF LINE 
		//
		remains = cxo + ((cxo > 360) ? (0) : (cxo));	// IF COPY 4 CH (cxo > 360 ) -> cxo VALUE IS 1CH
														// IF COPY 1 CH (cxo <=360 ) -> DOUBLE cxo VALUE TO MAKE 1 CH
		while( remains ) {

			if( *byte_count > remains ) {

				memcpy( pe, po, remains );

				po += remains;

				pe += remains;

				*byte_count -= remains;

				remains -= remains;
			}
			else {

				memcpy( pe, po, *byte_count );

				po += *byte_count;

				remains -= *byte_count;

				p_sglist++;

				(*count)++;

				if( *count < mapping_count ) {

					pe = kmap( p_sglist->page );

					*byte_count = sg_dma_len( p_sglist );
				}
				else {

					pe = NULL;

					*byte_count = 0;

					*s_po = po; *s_pe = pe; *s_p_sglist = p_sglist;

					return FALSE;
				}
			}
		}

		//
		// 
		po += (offset_o << 1) + ((cxo > 360) ? (0) : (cwo)); // COPY CIF(HALF_LINE): SKIP cwo(720) + OFFSET

		//
		// 
		remains = offset_e;

		while( remains ) {

			if( *byte_count > remains ) {

				pe += remains;

				*byte_count -= remains;

				remains -= remains;
			}
			else {

				remains -= *byte_count;

				p_sglist++;

				(*count)++;

				if( *count < mapping_count ) {

					pe = kmap( p_sglist->page );

					*byte_count = sg_dma_len( p_sglist );
				}
				else {

					pe = NULL;

					*byte_count = 0;

					*s_po = po; *s_pe = pe; *s_p_sglist = p_sglist;

					return FALSE;
				}
			}
		}
	}

	*s_po = po;

	*s_pe = pe;

	*s_p_sglist = p_sglist;

	return TRUE;
}
/*
BOOLEAN COPY_HALF_LINE( BYTE ** s_po, BYTE ** s_pe, struct scatterlist ** s_p_sglist, ULONG cxo, ULONG cwo, ULONG cwe, ULONG * byte_count, ULONG * count, ULONG mapping_count, ULONG offset_o, ULONG offset_e ){

	//	copy_to_user( pe, po, cxo ); == COPY LINE
	//
	//	po += cwo; == SKIP PITCH (SRC)
	//
	//	pe += cwe; == SKIP PITCH (DST)

	BYTE * po = *s_po;

	BYTE * pe = *s_pe;

	ULONG remains = 0;

	struct scatterlist * p_sglist = *s_p_sglist;

	remains = cxo << 1;

	while( remains ) {

		if( *byte_count > remains ) {

			memcpy( pe, po, remains );

			po += remains;

			pe += remains;

			*byte_count -= remains;

			remains -= remains;
		}
		else {

			memcpy( pe, po, *byte_count );

			po += *byte_count;

			remains -= *byte_count;

			p_sglist++;

			(*count)++;

			if( *count < mapping_count ) {

				pe = kmap( p_sglist->page );

				*byte_count = sg_dma_len( p_sglist );
			}
			else {

				pe = NULL;

				*byte_count = 0;

				*s_po = po; *s_pe = pe; *s_p_sglist = p_sglist;

				return FALSE;
			}
		}
	}

	//
	// 
	po += offset_o + cwo + offset_o;

	//
	// 
	remains = offset_e; // 0

	while( remains ) {

		if( *byte_count > remains ) {

			pe += remains;

			*byte_count -= remains;

			remains -= remains;
		}
		else {

			remains -= *byte_count;

			p_sglist++;

			(*count)++;

			if( *count < mapping_count ) {

				pe = kmap( p_sglist->page );

				*byte_count = sg_dma_len( p_sglist );
			}
			else {

				pe = NULL;

				*byte_count = 0;

				*s_po = po; *s_pe = pe; *s_p_sglist = p_sglist;

				return FALSE;
			}
		}
	}

	*s_po = po;

	*s_pe = pe;

	*s_p_sglist = p_sglist;

	return TRUE;
}
*/


#endif
//

BOOLEAN AH8400_HwProcessAnalogPCIVideoFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer )
{

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

	BYTE * po = NULL;

	#ifdef LINUXV4L2_VMALLOC_MODULE

	BYTE * pe = videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer );

	#else

	BYTE * pe = NULL;

	#endif

	BYTE * pFrameBuffer = pe;

	ULONG  cxo = pVideo->m_nDstFrameWidth; // shawn 2010.10.13

	ULONG  cwe = pVideo->m_nDstFrameWidth; // shawn 2010.10.13

	ULONG  cwo = 720; // shawn 2010.10.13

	ULONG  cyo = 0;

	ULONG  cye = pVideo->m_nDstFrameHeight;

	ULONG  cif_pre_offset = 0;

	ULONG  i = 0;

	if( pe == NULL ) { return TRUE; }

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { cyo = 240; }

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { cyo = 288; }

	if( cxo >= 720 ) {						

		cxo = 720;
	
		if( cye > cyo * 2 ) { cye = cyo * 2; }

		if( cye < cyo * 1 ) { cyo = cye; }
	}
	else if( cxo == 704 || cxo == 640) {
	
		if( cye > cyo * 2 ) { cye = cyo * 2; }

		if( cye < cyo * 1 ) { cyo = cye; }
	}
	else { //

		if( cxo > 360 ) { cxo = 360; }

		if( cye > cyo ) { cye = cyo; }

		if( cye < cyo ) { cyo = cye; }

		if( pVideo->m_nSubDeviceNumber == 0 ) { cif_pre_offset =                0; }

		if( pVideo->m_nSubDeviceNumber == 1 ) { cif_pre_offset =              cwo; }

		if( pVideo->m_nSubDeviceNumber == 2 ) { cif_pre_offset = 1440 * 288 +   0; }

		if( pVideo->m_nSubDeviceNumber == 3 ) { cif_pre_offset = 1440 * 288 + cwo; }
	}


	// COPY VIDEO FRAME TO VIDEO BUFFER 
	//
	if( cxo == 720 || cxo == 704 || cxo == 640 ) { // DISPLAY D1 (4 CH)
		
		ULONG i, j;

		ULONG tx = 0;

		if( cxo == 720 ) { tx =  0; }

		if( cxo == 704 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? ( 8) : ( 0) ); } // / SC390

		if( cxo == 640 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? (40) : (32) ); } // / SC390

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { tx = 0; } // FIX CHIPSET OFFSET ISSUE
		
		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (0) ) {

			for( i = 0; i < 4; i++ ){
				
				if( i == 0 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 0 +   0 + tx; } // CH01
				if( i == 1 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 0 + cwo + tx; } // CH02
				if( i == 2 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 1 +   0 + tx; } // CH03
				if( i == 3 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 1 + cwo + tx; } // CH04

				if( i == 0 ) { pe = pFrameBuffer + (cwe * cye ) * (0) +   0 ; }
				if( i == 1 ) { pe = pFrameBuffer + (cwe * cye ) * (0) + cwe ; }  
				if( i == 2 ) { pe = pFrameBuffer + (cwe * cye ) * (1) +   0 ; }
				if( i == 3 ) { pe = pFrameBuffer + (cwe * cye ) * (1) + cwe ; }  

				for( j = 0 ; j < cyo ; j++ ) {
				
					memcpy( pe, po, cxo );

					po += (cwo << 1); 

					pe += (cwe << 1);
				}
			}
		}
		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (1) ) {
			
			for( i = 0; i < 4; i++ ){
				
				if( i == 0 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 0 +   0 + tx; } // CH01
				if( i == 1 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 0 + cwo + tx; } // CH02
				if( i == 2 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 1 +   0 + tx; } // CH03
				if( i == 3 ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 1 + cwo + tx; } // CH04

				if( i == 0 ) { pe = pFrameBuffer + (cwe * cye ) * (0) +   0 ; }
				if( i == 1 ) { pe = pFrameBuffer + (cwe * cye ) * (0) + cwe ; }  
				if( i == 2 ) { pe = pFrameBuffer + (cwe * cye ) * (1) +   0 ; }
				if( i == 3 ) { pe = pFrameBuffer + (cwe * cye ) * (1) + cwe ; }  

				for( j = 0 ; j < cyo ; j++ ) {
				
					memcpy( pe, po, cxo );

					po += (cwo << 1); 

					pe += (cwe << 1);
				}
			}
		}
	}
	if( cxo == 360 || cxo == 352 || cxo == 320 ){ // DISPLAY CIF (1 CH)

		ULONG tx = 0;

		ULONG i  = 0;

		if( cxo == 360 ) { tx = 0; }

		if( cxo == 352 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? ( 8) : ( 0) ); } // / SC390

		if( cxo == 320 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? (40) : (32) ); } // / SC390

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { tx = 0; } // FIX CHIPSET OFFSET ISSUE

		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 0 + tx; }

		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 0 + tx; }

		po += cif_pre_offset;

		for( i = 0 ; i < cye ; i++ ) { 
			
			memcpy( pe, po, (cxo << 1) );

			po += (cwo << 1);
			
			pe += (cwe << 1); 
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	#else

	struct videobuf_dmabuf * pKsDMABuffer = &(pVideoBuffer->m_sKsBuffer.dma);

	struct scatterlist * p_sglist = (struct scatterlist *)(pKsDMABuffer->sglist);

//	struct scatterlist * p_start_sglist = p_sglist; // shawn 2010.10.13

	ULONG  mapping_count = pKsDMABuffer->sglen;

	ULONG  count = 0;

	ULONG  byte_count = sg_dma_len( p_sglist );

//	ULONG  remains = 0;

	BYTE * po = NULL;

//	BYTE * pe = pVideoBuffer->m_sKsBuffer.dma.varea; //

	BYTE * pe = kmap( p_sglist->page ); //

	BYTE * pFrameBuffer = pe; // shawn 2010.10.13

	ULONG  cxo = pVideo->m_nDstFrameWidth; // shawn 2010.10.13

	ULONG  cwe = pVideo->m_nDstFrameWidth; // shawn 2010.10.13

	ULONG  cwo = 720; // shawn 2010.10.13

	ULONG  cyo = 0;

	ULONG  cye = pVideo->m_nDstFrameHeight;

	ULONG  cif_pre_offset = 0;

	ULONG  i = 0;

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) { cyo = 240; }

	if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { cyo = 288; }

	//SHAWN 2010.10.13
	//
	if( TRUE ) {  //

		if( cxo >= 720 ) {	

			cxo = 720;
		
			if( cye > cyo * 2 ) { cye = cyo * 2; }

			if( cye < cyo ) { cyo = cye; }
		}
		else if( cxo == 704 || cxo == 640 ) {
		
			if( cye > cyo * 2 ) { cye = cyo * 2; }

			if( cye < cyo ) { cyo = cye; }

		}
		else { //

			if( cxo > 360 ) { cxo = 360; }

			if( cye > cyo ) { cye = cyo; }

			if( cye < cyo ) { cyo = cye; }

			if( pVideo->m_nSubDeviceNumber == 0 ) { cif_pre_offset =                0; }

			if( pVideo->m_nSubDeviceNumber == 1 ) { cif_pre_offset =              720; }

			if( pVideo->m_nSubDeviceNumber == 2 ) { cif_pre_offset = 1440 * 288 +   0; }

			if( pVideo->m_nSubDeviceNumber == 3 ) { cif_pre_offset = 1440 * 288 + 720; }
		}
	}
	else {  //

		if( cxo >= 704 ) { 

			cxo = 704;

			cwo = 704;

			if( cye > cyo * 2 ) { cye = cyo * 2; }

			if( cye < cyo ) { cyo = cye; }
		}
		else if( cxo == 640 ) {

			cwo = 704;

			if( cye > cyo * 2 ) { cye = cyo * 2; }

			if( cye < cyo ) { cyo = cye; }

		}
		else { //

			cwo = 704;  cwe = 704;

			if( cxo > 352 ) { cxo = 352; }

			if( cye > cyo ) { cye = cyo; }

			if( cye < cyo ) { cyo = cye; }

			if( pVideo->m_nSubDeviceNumber == 0 ) { cif_pre_offset =                0; }

			if( pVideo->m_nSubDeviceNumber == 1 ) { cif_pre_offset =              cwo; }

			if( pVideo->m_nSubDeviceNumber == 2 ) { cif_pre_offset = 1440 * 288 +   0; }

			if( pVideo->m_nSubDeviceNumber == 3 ) { cif_pre_offset = 1440 * 288 + cwo; }
		}
	}

	//shawn 2010.10.13

	// COPY VIDEO FRAME TO VIDEO BUFFER 
	//
	if( cxo == 720 || cxo == 704 || cxo == 640 ){ // DISPLAY D1 (4 CH)
		
		ULONG j = 0;

		ULONG tx = 0;

		if( cxo == 720 ) { tx =  0; }

		if( cxo == 704 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? ( 8) : ( 0) ); } // / SC390

		if( cxo == 640 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? (40) : (32) ); } // / SC390

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { tx = 0; } // FIX CHIPSET OFFSET ISSUE
		
		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (0) ) {

			// COPY 1st TOP FIELD
			//
			po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 0 + 0 + tx; 
			pe = pFrameBuffer + (cwe * cye ) * (0) + 0 ; 

			for( j = 0 ; j < cyo ; j++ ) {

				ULONG offset_o = (cwo - cxo) >> 1; // 720 - 704 = 16 / 2 = 8

				ULONG offset_e = (cwe - cxo) >> 1; // (720 - 720) >> 1 = 0

				if( COPY_LINE( &po, &pe, &p_sglist, cxo, cwo, cwe, &byte_count, &count, mapping_count, offset_o, offset_e ) == FALSE ) { goto LIVE_LOOP_END_LINES; }
			}

			// COPY 2nd BOTTOM FIELD
			//
			po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 1 + 0 + tx; 

			for( j = 0 ; j < cyo ; j++ ) {

				ULONG offset_o = (cwo - cxo) >> 1; // 720 - 704 = 16 / 2 = 8

				ULONG offset_e = (cwe - cxo) >> 1; // (720 - 720) >> 1 = 0

				if( COPY_LINE( &po, &pe, &p_sglist, cxo, cwo, cwe, &byte_count, &count, mapping_count, offset_o, offset_e ) == FALSE ) { goto LIVE_LOOP_END_LINES; }
			}

		}
		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (1) ) {				

			// COPY 1st TOP FIELD
			//					
			po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 0 + 0 + tx; 
			pe = pFrameBuffer + (cwe * cye ) * (0) + 0 ; 

			for( j = 0 ; j < cyo ; j++ ) {

				ULONG offset_o = (cwo - cxo) >> 1; // 720 - 704 = 16 / 2 = 8

				ULONG offset_e = (cwe - cxo) >> 1; // (720 - 720) >> 1 = 0

				if( COPY_LINE( &po, &pe, &p_sglist, cxo, cwo, cwe, &byte_count, &count, mapping_count, offset_o, offset_e ) == FALSE ) { goto LIVE_LOOP_END_LINES; }
			}

			// COPY 2nd BOTTOM FIELD
			//
			po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 1 + 0 + tx; 

			for( j = 0 ; j < cyo ; j++ ) {

				ULONG offset_o = (cwo - cxo) >> 1; // 720 - 704 = 16 / 2 = 8

				ULONG offset_e = (cwe - cxo) >> 1; // (720 - 720) >> 1 = 0

				if( COPY_LINE( &po, &pe, &p_sglist, cxo, cwo, cwe, &byte_count, &count, mapping_count, offset_o, offset_e ) == FALSE ) { goto LIVE_LOOP_END_LINES; }
			}
		}
	}
	if( cxo == 360 || cxo == 352 || cxo == 320 ) { // DISPLAY CIF (1 CH)

		ULONG tx = 0;

		if( cxo == 360 ) { tx =  0; }

		if( cxo == 352 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? ( 8) : ( 0) ); } // / SC390

		if( cxo == 320 ) { tx = ( (pVideo->m_pDevice->m_nCustomHardwareTopologyProperty & 0x00000020) ? (40) : (32) ); } // / SC390

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) { tx = 0; } // FIX CHIPSET OFFSET ISSUE

		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 0 + (1440 * 288) * 0 + tx; }

		if( (pVideoBuffer->m_sKsBuffer.input % 2) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ] + (1440 * 576) * 1 + (1440 * 288) * 0 + tx; }

		po += cif_pre_offset;

		{
			//
			//
			ULONG offset_o = (cwo - (cxo << 1) ) >> 1; // 720 - 352*2 = 16 / 2 = 8

			ULONG offset_e = 0; // (cwe - cxo) >> 1; 

			for( i = 0 ; i < cyo ; i++ ) {
			
				if( COPY_LINE( &po, &pe, &p_sglist, cxo, cwo, cwe, &byte_count, &count, mapping_count, offset_o, offset_e ) == FALSE ) { goto LIVE_LOOP_END_LINES; }
			}
		}

	}

LIVE_LOOP_END_LINES:

	#endif

	pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //

	return TRUE;
}

typedef struct _AH8400_H264_STREAM_HEADER
{
	DWORD nFrameBufferSize : 24; //

	DWORD nFrameType       :  1; //

	DWORD nMotionStatus    :  1; //

	DWORD nLockStatus      :  1; //

	DWORD nReserved        :  5;
		
} AH8400_H264_STREAM_HEADER;

BOOLEAN AH8400_HwProcessAnalogPCIVideoEncoderFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer )
{	
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

	BYTE * po = NULL;

	#ifdef LINUXV4L2_VMALLOC_MODULE

	BYTE * pe = videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer );

	#else

	BYTE * pe = NULL;

	#endif

	ULONG  co = pVideoBuffer->m_sKsBuffer.size;

	ULONG  ce = pVideoBuffer->m_sKsBuffer.bytesperline * pVideoBuffer->m_sKsBuffer.height;

	if( pe == NULL ) { return TRUE; }

	if( (co + 4) > (ce) ) { co = (ce - 4); }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 1 - 4; }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 2 - 4; }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (2) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 3 - 4; }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (3) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 4 - 4; }

	//
	//
	{	ULONG i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;
		
		AH8400_H264_STREAM_HEADER hdr;

		hdr.nFrameBufferSize = co & 0x00FFFFFF;

		hdr.nFrameType = (po[ 2 ] & 0x40) ? (1) : (0);

		hdr.nMotionStatus = g_n_multiplexer_motion_status[ i ];

		hdr.nLockStatus = g_n_analog_decoder_lock_status[ i ];

		hdr.nReserved = 0x00;

		memcpy( pe, &hdr, 4 );

		po += 4;

	//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] H.264 HEADER %02X %02X %02X %02X\n", pDevice->m_nKsDeviceNumber, pe[ -1 ], pe[ -2 ], pe[ -3 ], pe[ -4 ] );
	}
	//
	//
	{	po -= 0x00096000;

		memcpy( pe + 4, po, co );
	}
	#else

	struct videobuf_dmabuf * pKsDMABuffer = &(pVideoBuffer->m_sKsBuffer.dma);

	struct scatterlist * p_sglist = (struct scatterlist *)(pKsDMABuffer->sglist);

	ULONG  mapping_count = pKsDMABuffer->sglen;

	ULONG  count = 0;

	ULONG  byte_count = sg_dma_len( p_sglist );

	ULONG  remains = 0;

	BYTE * po = NULL;

//	BYTE * pe = pVideoBuffer->m_sKsBuffer.dma.varea; //

	BYTE * pe = kmap( p_sglist->page ); //

	ULONG  co = pVideoBuffer->m_sKsBuffer.size;

	ULONG  ce = pVideoBuffer->m_sKsBuffer.bytesperline * pVideoBuffer->m_sKsBuffer.height;

	if( (co + 4) > (ce) ) { co = (ce - 4); }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 1 - 4; }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 2 - 4; }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (2) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 3 - 4; }

	if( (pVideoBuffer->m_sKsBuffer.input % 4) == (3) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ] + (0x00096000) * 4 - 4; }

	//
	//
	{	ULONG i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;
	
		AH8400_H264_STREAM_HEADER hdr;

		BYTE * pm = po;

		hdr.nFrameBufferSize = co & 0x00FFFFFF;

		hdr.nFrameType = (po[ 2 ] & 0x40) ? (1) : (0);

		hdr.nMotionStatus = g_n_multiplexer_motion_status[ i ];

		hdr.nLockStatus = g_n_analog_decoder_lock_status[ i ];

		hdr.nReserved = 0x00;

		po = (BYTE *)(&hdr);

		remains = 4;

		while( remains ) { //

			if( byte_count > remains ) {

				memcpy( pe, po, remains );

				po += remains;

				pe += remains;

				byte_count -= remains;

				remains -= remains;
			}
			else {

				memcpy( pe, po, byte_count );

				po += byte_count;

				remains -= byte_count;

				p_sglist++;

				count++;

				if( count < mapping_count ) {

					pe = kmap( p_sglist->page );

					byte_count = sg_dma_len( p_sglist );
				}
				else {

					pe = NULL;

					byte_count = 0;

					goto H264_LOOP_END_LINES;
				}
			}
		}

		po = pm + 4;

	//	LINUXV4L2_DEBUG( KERN_INFO, "[%02ld] H.264 HEADER %02X %02X %02X %02X\n", pDevice->m_nKsDeviceNumber, pe[ -1 ], pe[ -2 ], pe[ -3 ], pe[ -4 ] );
	}

	//
	//
	{	po -= 0x00096000;
	
		remains = co;

		while( remains ) { //

			if( byte_count > remains ) {

				memcpy( pe, po, remains );

				po += remains;

				pe += remains;

				byte_count -= remains;

				remains -= remains;
			}
			else {

				memcpy( pe, po, byte_count );

				po += byte_count;

				remains -= byte_count;

				p_sglist++;

				count++;

				if( count < mapping_count ) {

					pe = kmap( p_sglist->page );

					byte_count = sg_dma_len( p_sglist );
				}
				else {

					pe = NULL;

					byte_count = 0;

					goto H264_LOOP_END_LINES;
				}
			}
		}
	}

H264_LOOP_END_LINES:

	#endif

	pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //

	pVideoBuffer->m_sKsBuffer.size = ce; //

	return TRUE;
}
