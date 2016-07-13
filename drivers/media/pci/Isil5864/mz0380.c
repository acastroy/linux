#include "LINUXV4L2.h"
#include "property.h"
#include "wrapper.h"
#include "mz0380.h"
#include "mst3367.h"

//#define MZ0380_COPYPORTECT_ARCHITECTURE

#ifdef MZ0380_COPYPORTECT_ARCHITECTURE

extern ULONG g_copy_protect_unlock_boradsA[ 16 ];

extern ULONG g_copy_protect_unlock_boradsB[ 16 ];

#endif

ULONG MZ0380_GetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex )
{
/*
	if( nBytes == 4 ) { return READ_REGISTER_ULONG( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ); }

	if( nBytes == 1 ) { return READ_REGISTER_UCHAR( (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ); }
*/
	if(nBase != 0)
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_GetRegister() nBase(%d) out of ramge, error\n", pDevice->m_nKsDeviceNumber, nBase );
		return 0x00000000;
	}

	{ return readl( (pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ) & 0xFFFFFFFF; }

	return 0x00000000;
}
ULONG MZ0380_GetRegister_EX( CDevice * pDevice, ULONG nBase, ULONG dwIndex,  ULONG nBytes)
{
/*
	if( nBytes == 4 ) { return READ_REGISTER_ULONG( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ); }

	if( nBytes == 1 ) { return READ_REGISTER_UCHAR( (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ); }
*/
	if(nBase != 0)
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_GetRegister() nBase(%d) out of ramge, error\n", pDevice->m_nKsDeviceNumber, nBase );
		return 0x00000000;
	}
	if( nBytes == 4 )
	{
		return readl( (pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ) & 0xFFFFFFFF;
	}
	if( nBytes == 1 )
	{
		return readb( (pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex) ) & 0xFF;
	}
	return 0x00000000;
}

BOOLEAN MZ0380_SetRegister( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG dwValue )
{
/*
	if( nBytes == 4 ) { WRITE_REGISTER_ULONG( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex), (DWORD)(dwValue) ); }
	
	if( nBytes == 1 ) { WRITE_REGISTER_UCHAR( (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex), (UCHAR)(dwValue) ); }
*/

	if(nBase == 1)
	{
		writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 5 ] + dwIndex) ); 
	}
	else if(nBase == 0)
	{
		writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); 
	}
	else
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_SetRegister() nBase(%d) out of ramge, error\n", pDevice->m_nKsDeviceNumber, nBase );
		return 0x00000000;
	}

	return TRUE;
}

BOOLEAN MZ0380_SetRegister_EX( CDevice * pDevice, ULONG nBase, ULONG dwIndex, ULONG dwValue, ULONG nBytes )
{
/*
	if( nBytes == 4 ) { WRITE_REGISTER_ULONG( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex), (DWORD)(dwValue) ); }
	
	if( nBytes == 1 ) { WRITE_REGISTER_UCHAR( (UCHAR *)(pDevice->m_pRegBaseCommonBuffer[ nBase ] + dwIndex), (UCHAR)(dwValue) ); }
*/

	if(nBase == 1)
	{
		if( nBytes == 4 )
		{
			writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 5 ] + dwIndex) ); 
		}
		if( nBytes == 1 )
		{
			writeb( (dwValue & 0xFF), (pDevice->m_pRegBaseCommonBuffer[ 5 ] + dwIndex) ); 
		}
	}
	else if(nBase == 0)
	{
		if( nBytes == 4 )
		{
			writel( (dwValue & 0xFFFFFFFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); 
		}
		if( nBytes == 1 )
		{
			writeb( (dwValue & 0xFF), (pDevice->m_pRegBaseCommonBuffer[ 0 ] + dwIndex) ); 
		}
	}
	else
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_SetRegister() nBase(%d) out of ramge, error\n", pDevice->m_nKsDeviceNumber, nBase );
		return 0x00000000;
	}

	return TRUE;
}

ULONGLONG MZ0380_KsQuerySystemTime( CDevice * pDevice ) 
{ 
	return 0;
}

BOOLEAN MZ0380_DELAY_100NS( CDevice * pDevice, ULONG DELAY )
{
//	wrapper_ndelay( DELAY * 100 );

	//do not use busy delay
	
	wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( DELAY/10000 ) );

	return TRUE;
}

BOOLEAN DELAY_100NS( ULONG DELAY )
{
	//wrapper_ndelay( DELAY * 100 );

	//do not use busy delay
	
	wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies( DELAY/10000 ) );
	
	return TRUE;
}

//BOOLEAN MZ0380_WaitInterruptComplete( CDevice * pDevice, DWORD command, LONGLONG DELAY = 50000000 );

BOOLEAN MZ0380_WaitInterruptComplete( CDevice * pDevice, DWORD command, ULONG DELAY ) 
{
	
	long ret = 0;

	ret = wait_event_interruptible_timeout( pDevice->m_wait_queue_head, pDevice->m_MZ0380_ready == 1, wrapper_msecs_to_jiffies( DELAY/10000 ));
	
	//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_WaitInterruptComplete() m_MZ0380_ready(0x%x) ret(0d%d)\n", pDevice->m_nKsDeviceNumber, pDevice->m_MZ0380_ready, ret );

	if( ret <= 0 ){

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_WaitInterruptComplete() command(0x%x) time out, ret(0d%d)---------------------------\n", pDevice->m_nKsDeviceNumber, command, ret );
		
		return FALSE;
	} 
	else { 
		
		if(pDevice->m_MZ0380_ready == 0)
		{
			LINUXV4L2_PRINT( KERN_INFO, "[%02d] MZ0380_WaitInterruptComplete() m_MZ0380_ready(0x%x), error----------------------\n", pDevice->m_nKsDeviceNumber );
		}
		else
		{
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_WaitInterruptComplete() success, m_MZ0380_ready(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_MZ0380_ready );
		}
		
		wrapper_down( &(pDevice->m_MZ0380AccessSemaphore) ); 

		pDevice->m_MZ0380_ready = 0;

		wrapper_up( &(pDevice->m_MZ0380AccessSemaphore) ); 

		return TRUE;
	} 

/*
	LARGE_INTEGER wait_time; wait_time.QuadPart = -DELAY; // 5S

	if( STATUS_TIMEOUT == KeWaitForSingleObject( &pDevice->m_hIRQAccessSemaphore, Executive, KernelMode, FALSE, &wait_time ) ) { 
	
		LINUXV4L2_PRINT( KERN_INFO, "MZ0380 COMMAND TIMEOUT( %d × %08X )\n", pDevice->m_nKsDeviceNumber, command);

		return FALSE;
	} 
	else { 
		
		return TRUE;
	} 
*/
//	return TRUE;

}

BOOLEAN MZ0380_SendVendorCommand_P3( CDevice * pDevice, DWORD config[ 12 ], ULONG length )
{
	return MZ0380_SendVendorCommand_P5( pDevice, config, length, TRUE, 50000000 );
}

BOOLEAN MZ0380_SendVendorCommand( CDevice * pDevice, DWORD config[ 12 ], ULONG length, BOOL wait_semaphore )
{
	return MZ0380_SendVendorCommand_P5( pDevice, config, length, wait_semaphore, 50000000 );
}


BOOLEAN MZ0380_SendVendorCommand_P5( CDevice * pDevice, DWORD config[ 12 ], ULONG length, BOOL wait_semaphore, LONGLONG DELAY )
{
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( length <= 1 ) { return FALSE; }

	if( p_sys_cfg->n_firmware_timeouts >= 5 ) { // 避免無法進入系統
		
		LINUXV4L2_PRINT( KERN_INFO, "MZ0380_SEND_COMMAND( %08X, %d ) TIMEOUT!!\n", config[ 1 ], p_sys_cfg->n_firmware_timeouts);

		return FALSE;
	}

	if( wait_semaphore ) {
		//use m_MZ0380AccessSemaphore, already done in modinit
		/*
		KeInitializeSemaphore( &(pDevice->m_hIRQAccessSemaphore), 0, 0x7FFFFFFF );
		*/
	}
	else {

		MZ0380_SetRegister( pDevice, 0, (0x00000004 * 11), 0x00000000 );
	}
	ULONG i = 1;
	for( i = 1 ; i < length ; i++ ) {

		MZ0380_SetRegister( pDevice, 0, (0x00000004 * i), config[ i ] );
	}
	MZ0380_SetRegister( pDevice, 0, 0x00000000, 0x00000800 );

	if( (config[ 1 ] != 0x00000018) && (config[ 1 ] != 0x0000001A) && (config[ 1 ] != 0x0000001B) )// MZ0380_CMD_GET_SPI_DATA,  MZ0380_CMD_GET_I2C_DATA, MZ0380_CMD_SET_I2C_DATA
	{
		LINUXV4L2_PRINT( KERN_INFO, "[%02d] MZ0380_SendVendorCommand( 0x%08X ) \n", pDevice->m_nKsDeviceNumber, config[ 1 ]);
	}

	if( wait_semaphore ) {

		if(config[ 1 ] == 0x00000007) // MZ0380_CMD_STOP_DMA
		{
			int wait_stop_dma_counter = 0;

			while(pDevice->m_MZ0380_ready == 0)
			{
				//DELAY_100NS(1000000);//some interrupt occur
				wrapper_schedule_timeout_uninterruptible( wrapper_msecs_to_jiffies( 10 ) );

				//LINUXV4L2_DEBUG( KERN_INFO, "wait MZ0380_CMD_STOP_DMA(%d)--\n", wait_stop_dma_counter);
				
				wait_stop_dma_counter ++;
				
				if(wait_stop_dma_counter > 1000)
				{
					break;
				}
			}
		}
		

		return MZ0380_WaitInterruptComplete( pDevice, config[ 1 ], DELAY );
	}
	else { // FOR GOIP/I2C/SPI

		if( DELAY == 0 ) { return TRUE; }

		ULONG counts = 500;
		ULONG i = 0;
		for( i = 0 ; i < counts ; i++ ) {

	//	for( ULONG i = 0 ; i < 100 ; i++ ) {

			ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 11) );

			if( R & 0x01 ) {

				p_sys_cfg->n_firmware_timeouts = 0;

				return TRUE;
			}
			DELAY_100NS( 1000 );
		}
	}
	LINUXV4L2_PRINT( KERN_INFO, "MZ0380_SEND_COMMAND( %08X, %d ) ERROR!!\n", config[ 1 ], p_sys_cfg->n_firmware_timeouts++);

	return FALSE;
}

BYTE MZ0380_GetAnalogAudioDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex )
{	
	BOOLEAN returns = TRUE;

	if( returns ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
		
		command[ 1 ] = 0x0000001A; // MZ0380_CMD_GET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = bIndex;

		command[ 4 ] = 0x00000000;

		if( MZ0380_SendVendorCommand( pDevice, command, 5, FALSE ) ) {

			ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 4) );

			return (BYTE)(R);
		}
	}
	return 0x00;
}

BOOLEAN MZ0380_SetAnalogAudioDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex, BYTE bValue )
{
	BOOLEAN returns = TRUE;

	if( returns ) {
		
		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
		command[ 1 ] = 0x0000001B; // MZ0380_CMD_SET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = bIndex;

		command[ 4 ] = bValue;

		return MZ0380_SendVendorCommand( pDevice, command, 5, FALSE );
	}
	return returns;
}

BYTE MZ0380_GetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex )
{	
	BOOLEAN returns = TRUE;

	if( returns ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
		
		command[ 1 ] = 0x0000001A; // MZ0380_CMD_GET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = bIndex;

		command[ 4 ] = 0x00000000;

		if( MZ0380_SendVendorCommand( pDevice, command, 5, FALSE ) ) {

			ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 4) );

			return (BYTE)(R);
		}
	}
	return 0x00;
}

BOOLEAN MZ0380_SetAnalogVideoDecoderRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex, BYTE bValue )
{
	BOOLEAN returns = TRUE;

	if( returns ) {
		
		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
		command[ 1 ] = 0x0000001B; // MZ0380_CMD_SET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = bIndex;

		command[ 4 ] = bValue;

		return MZ0380_SendVendorCommand( pDevice, command, 5, FALSE );
	}
	return returns;
}

BYTE MZ0380_GetAnalogVideoDecoderRegister_EX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex )
{	
	BOOLEAN returns = TRUE;

	if( returns ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
			
		command[ 1 ] = 0x0000001B; // MZ0380_CMD_SET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = 0x00;

		command[ 4 ] = nBank;

		returns = MZ0380_SendVendorCommand( pDevice, command, 5, FALSE );
	}
	if( returns ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
		
		command[ 1 ] = 0x0000001A; // MZ0380_CMD_GET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = bIndex;

		command[ 4 ] = 0x00000000;

		if( MZ0380_SendVendorCommand( pDevice, command, 5, FALSE ) ) {

			ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 4) );

			return (BYTE)(R);
		}
	}
	return 0x00;
}

BOOLEAN MZ0380_SetAnalogVideoDecoderRegister_EX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex, BYTE bValue )
{
	BOOLEAN returns = TRUE;

	if( returns ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
			
		command[ 1 ] = 0x0000001B; // MZ0380_CMD_SET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = 0x00;

		command[ 4 ] = nBank;

		returns = MZ0380_SendVendorCommand( pDevice, command, 5, FALSE );
	}
	if( returns ) {
		
		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
		command[ 1 ] = 0x0000001B; // MZ0380_CMD_SET_I2C_DATA

		command[ 2 ] = bDevAddr;

		command[ 3 ] = bIndex;

		command[ 4 ] = bValue;

		return MZ0380_SendVendorCommand( pDevice, command, 5, FALSE );
	}
	return returns;
}

USHORT MZ0380_GetSpiRegister( CDevice * pDevice, USHORT wIndex )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x00000018; // MZ0380_CMD_GET_SPI_DATA

	command[ 2 ] = wIndex;

	command[ 3 ] = 0x00000000;

	if( MZ0380_SendVendorCommand( pDevice, command, 4, FALSE ) ) {

		ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 3) );

		return (USHORT)(R);
	}
	return 0x0000;
}

BOOLEAN MZ0380_SetSpiRegister( CDevice * pDevice, USHORT wIndex, USHORT wValue )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x00000019; // MZ0380_CMD_SET_SPI_DATA

	command[ 2 ] = wIndex;

	command[ 3 ] = wValue;

	return MZ0380_SendVendorCommand( pDevice, command, 4, FALSE );
}

DWORD MZ0380_GetEncryptRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
		
	command[ 1 ] = 0x0000001C; // MZ0380_CMD_GET_I2C_DATA_ENCRYPT

	command[ 2 ] = bDevAddr;

	if( MZ0380_SendVendorCommand( pDevice, command, 3, FALSE ) ) {

		ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 4) );

		return R;
	}
	return 0x00000000;
}

BOOLEAN MZ0380_SetEncryptRegister( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE bIndex, BYTE pValue[ 4 ] )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x0000001D; // MZ0380_CMD_SET_I2C_DATA_ENCRYPT

	command[ 2 ] = bDevAddr;

	command[ 3 ] = bIndex;

	command[ 4 ] = sizeof(pValue); // SIZE

	command[ 5 ] = ((DWORD)(pValue[ 0 ]) <<  0) |
		
				   ((DWORD)(pValue[ 1 ]) <<  8) |
		
				   ((DWORD)(pValue[ 2 ]) << 16) |
		
				   ((DWORD)(pValue[ 3 ]) << 24);

	return MZ0380_SendVendorCommand( pDevice, command, 6, FALSE );
}

BYTE MZ0380_GetGpioValue( CDevice * pDevice, BYTE bBitNum )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x00000014; // MZ0380_CMD_GET_GPIO_PIN_DATA

	command[ 2 ] = 1 << bBitNum;

	command[ 3 ] = 0x00000000;

	if( MZ0380_SendVendorCommand( pDevice, command, 4, FALSE ) ) {

		ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 3) );

		return (BYTE)((R >> bBitNum) & 0x01);
	}
	return 0x00;
}

BOOLEAN MZ0380_SetGpioValue( CDevice * pDevice, BYTE bBitNum, BYTE bValue )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x00000015; // MZ0380_CMD_SET_GPIO_PIN_DATA

	command[ 2 ] = 1 << bBitNum;

	command[ 3 ] = bValue << bBitNum;

	return MZ0380_SendVendorCommand( pDevice, command, 4, FALSE );
}

BYTE MZ0380_GetGpioDirection( CDevice * pDevice, BYTE bBitNum )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x00000016; // MZ0380_CMD_GET_GPIO_PIN_DIRECTION

	command[ 2 ] = 1 << bBitNum;

	command[ 3 ] = 0x00000000;

	if( MZ0380_SendVendorCommand( pDevice, command, 4, FALSE ) ) {

		ULONG R = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 3) );

		return (BYTE)((R >> bBitNum) & 0x01);
	}
	return 0x00;
}

BOOLEAN MZ0380_SetGpioDirection( CDevice * pDevice, BYTE bBitNum, BYTE bDirection )
{
	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
	command[ 1 ] = 0x00000017; // MZ0380_CMD_SET_GPIO_PIN_DIRECTION

	command[ 2 ] = 1 << bBitNum;

	command[ 3 ] = bDirection << bBitNum; // 0 : OUTPUT, 1 : INPUT

	return MZ0380_SendVendorCommand( pDevice, command, 4, FALSE );
}

BOOLEAN MZ0380_GetFirmwareVersion( CDevice * pDevice, BYTE* pwszFileName, BYTE * pMajorVersion, BYTE * pMinorVersion )
{
/*
	WCHAR			  wszPathName[ 255 ];

	OBJECT_ATTRIBUTES oObjectAttributes;

	UNICODE_STRING    oFileName;

	IO_STATUS_BLOCK   oIoStatus;

	HANDLE            hFileHandle = NULL;

	BYTE *            pFileBuffer = NULL;

	ULONG             nFileSize = 0;

	NTSTATUS          status = STATUS_SUCCESS;

	BYTE			  parms[ 64 ];

	BOOLEAN			  returns = TRUE;

	swprintf( wszPathName, L"%s%s", pDevice->m_pCustomDeviceInfoDirectoryProperty, pwszFileName );

	LINUXV4L2_PRINT( KERN_INFO, "MZ0380_GetFirmwareVersion( \"%ws\" )\n", wszPathName);

	// 1.0 OPEN FONT LIBRARY *.DAT FILE
	// 
	oFileName.MaximumLength = wcslen(wszPathName) * 2 + sizeof(UNICODE_NULL);

	oFileName.Buffer = (PWSTR)(ExAllocatePool( NonPagedPool, oFileName.MaximumLength ));

	oFileName.Length = 0;

    if( oFileName.Buffer == NULL ) {

        return FALSE;
    }
	else {

		RtlZeroMemory( oFileName.Buffer, oFileName.MaximumLength );
	}
	status = RtlAppendUnicodeToString( &oFileName, wszPathName );

	InitializeObjectAttributes( (POBJECT_ATTRIBUTES)(&oObjectAttributes), (PUNICODE_STRING)(&oFileName), OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL );

	status = ZwCreateFile( &hFileHandle, FILE_READ_DATA, &oObjectAttributes, &oIoStatus, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0 );

	FREE_MEMORY( oFileName.Buffer );

	if( NT_SUCCESS(status) ) {

		FILE_STANDARD_INFORMATION oFileStandardInfomation;

		ZwQueryInformationFile( hFileHandle, &oIoStatus, &oFileStandardInfomation, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );

		if( oFileStandardInfomation.Directory == TRUE ) {

			ZwClose( hFileHandle );

			return FALSE;
		}
		nFileSize = (ULONG)(oFileStandardInfomation.EndOfFile.QuadPart);

		pFileBuffer = (BYTE *)(ExAllocatePool( NonPagedPool, nFileSize ));

		if( pFileBuffer ) {

			RtlZeroMemory( pFileBuffer, nFileSize );

			ZwReadFile( hFileHandle, NULL, NULL, NULL, &oIoStatus, pFileBuffer, nFileSize, NULL, NULL );

			ZwClose( hFileHandle );
		}
		else {

			ZwClose( hFileHandle );

			return FALSE;
		}
	}
	else {

		return FALSE;
	}
	ULONGLONG start_times = KeQueryInterruptTime();

	ULONGLONG stop_times = 0;

	if( nFileSize >= 5 ) {

		*pMajorVersion  = (pFileBuffer[ 0 ] - '0') * 10;

		*pMajorVersion += (pFileBuffer[ 1 ] - '0') *  1;

		*pMinorVersion  = (pFileBuffer[ 3 ] - '0') * 10;

		*pMinorVersion += (pFileBuffer[ 4 ] - '0') *  1;
	}
	stop_times = KeQueryInterruptTime();

	if( returns ) {

		LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS) (%d.%d)\n", (ULONG)(stop_times - start_times), nFileSize, *pMajorVersion, *pMinorVersion);
	}
	else {

		LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}

	// 3.0 FREE WORKING BUFFER
	//
	FREE_MEMORY( pFileBuffer );

	return returns;
*/
	const struct firmware *fw;
	int ret;

	ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[ 0 ]->dev));

	if (ret) {
		//printk(KERN_ERR "xc5000: Upload failed. (file not found?)\n");

		LINUXV4L2_PRINT( KERN_INFO,"[%d]MZ0380_GetFirmwareVersion() Upload failed. (file not found?)\n", pDevice->m_nKsDeviceNumber );

		return false;

	} else {
		//printk(KERN_DEBUG "xc5000: firmware read %d bytes.\n", fw->size);

		//printk(KERN_DEBUG " %x %x %x %x\n", *(fw->data + 0), *(fw->data + 1), *(fw->data + 3), *(fw->data + 4));

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MZ0380_GetFirmwareVersion() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, fw->size);
		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MZ0380_GetFirmwareVersion()  %x %x %x %x\n", pDevice->m_nKsDeviceNumber, *(fw->data + 0), *(fw->data + 1), *(fw->data + 3), *(fw->data + 4) );


		if( fw->size >= 5 ) {

			//*pMajorVersion  = (pFileBuffer[ 0 ] - '0') * 10;

			//*pMajorVersion += (pFileBuffer[ 1 ] - '0') *  1;

			//*pMinorVersion  = (pFileBuffer[ 3 ] - '0') * 10;

			//*pMinorVersion += (pFileBuffer[ 4 ] - '0') *  1;

			*pMajorVersion  = (*(fw->data + 0) - '0') * 10;
			*pMajorVersion += (*(fw->data + 1) - '0') *  1;
			*pMinorVersion  = (*(fw->data + 3) - '0') * 10;
			*pMinorVersion += (*(fw->data + 4) - '0') *  1;

		}

		return true;
	
	}

}

BOOLEAN MZ0380_DownloadFirmware( CDevice * pDevice, BYTE* pwszFileName )
{

	const struct firmware *fw;
	int ret;
	BYTE* pFileBuffer = NULL;
	ULONG             nFileSize = 0;
	BOOLEAN			  returns = TRUE;
	struct timeval ts;
	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[ 0 ]->dev));

	if (ret) {

		LINUXV4L2_PRINT( KERN_INFO,"[%02d] MZ0380_DownloadFirmware() Upload failed. (file not found?)\n", pDevice->m_nKsDeviceNumber );

		return false;

	} else {

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MZ0380_DownloadFirmware() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, fw->size);

		if(fw->size > 0)
		{
			nFileSize = fw->size;
			
			pFileBuffer = wrapper_vmalloc( fw->size + 4);//?? +4 ??

			if(pFileBuffer)
			{
				memset( pFileBuffer, 0x00, fw->size + 4);

				memcpy(pFileBuffer, fw->data, fw->size);
			}
			else
			{
				LINUXV4L2_PRINT( KERN_INFO,"[%d]MZ0380_DownloadFirmware() no buffer\n", pDevice->m_nKsDeviceNumber);

				return false;
			}

		}
	}


	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

	command[ 1 ] = 0x0000000B; // MZ0380_CMD_BEGIN_FIRMWARE_DOWNLOAD

	command[ 2 ] = nFileSize;

	if( MZ0380_SendVendorCommand_P3( pDevice, command, 3 ) == TRUE ) {

		{	
			do_gettimeofday( &ts );

			start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

			//WRITE_REGISTER_BUFFER_ULONG( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000060), (PULONG)(pFileBuffer), (nFileSize + 3) / (4) );
			memcpy( (UINT *)(pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000060), (UINT *)(pFileBuffer), (nFileSize + 3) );

			do_gettimeofday( &ts );

			stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

			LINUXV4L2_PRINT( KERN_INFO, "[%d] TOTAL DOWNLOAD TIMES (BUFFER) = %08d (%d BYTES)\n", pDevice->m_nKsDeviceNumber, (ULONG)(stop_times - start_times), nFileSize);
		}
		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

		command[ 1 ] = 0x0000000C; // MZ0380_CMD_END_FIRMWARE_DOWNLOAD

		command[ 2 ] = 0x00000001;

		if( MZ0380_SendVendorCommand_P5( pDevice, command, 3, TRUE, 600000000 ) == TRUE ) {

			DELAY_100NS( 1000000 );

			if( MZ0380_GetRegister( pDevice, 0, (0x00000004 * 2) ) == 0 ) {
				
				returns = TRUE;
			}
			else {

				returns = FALSE;
			}
		}
		else {

			returns = FALSE;
		}
	}
	else {

		returns = FALSE;
	}
	do_gettimeofday( &ts );

	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);

	if( returns ) {

		LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", pDevice->m_nKsDeviceNumber, (ULONG)(stop_times - start_times), nFileSize);

	}
	else {

		LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", pDevice->m_nKsDeviceNumber, (ULONG)(stop_times - start_times), nFileSize);
	}
	
	if(pFileBuffer)
	{
		vfree( pFileBuffer );
	}	
	return returns;


/*
	WCHAR			  wszPathName[ 255 ];

	OBJECT_ATTRIBUTES oObjectAttributes;

	UNICODE_STRING    oFileName;

	IO_STATUS_BLOCK   oIoStatus;

	HANDLE            hFileHandle = NULL;

	BYTE *            pFileBuffer = NULL;

	ULONG             nFileSize = 0;

	NTSTATUS          status = STATUS_SUCCESS;

	BYTE			  parms[ 64 ];

	BOOLEAN			  returns = TRUE;

	swprintf( wszPathName, L"%s%s", pDevice->m_pCustomDeviceInfoDirectoryProperty, pwszFileName );

	LINUXV4L2_PRINT( KERN_INFO, "MZ0380_DownloadFirmware( \"%ws\" )\n", wszPathName);

	// 1.0 OPEN FONT LIBRARY *.DAT FILE
	// 
	oFileName.MaximumLength = wcslen(wszPathName) * 2 + sizeof(UNICODE_NULL);

	oFileName.Buffer = (PWSTR)(ExAllocatePool( NonPagedPool, oFileName.MaximumLength ));

	oFileName.Length = 0;

    if( oFileName.Buffer == NULL ) {

        return FALSE;
    }
	else {

		RtlZeroMemory( oFileName.Buffer, oFileName.MaximumLength );
	}
	status = RtlAppendUnicodeToString( &oFileName, wszPathName );

	InitializeObjectAttributes( (POBJECT_ATTRIBUTES)(&oObjectAttributes), (PUNICODE_STRING)(&oFileName), OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL );

	status = ZwCreateFile( &hFileHandle, FILE_READ_DATA, &oObjectAttributes, &oIoStatus, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0 );

	FREE_MEMORY( oFileName.Buffer );

	if( NT_SUCCESS(status) ) {

		FILE_STANDARD_INFORMATION oFileStandardInfomation;

		ZwQueryInformationFile( hFileHandle, &oIoStatus, &oFileStandardInfomation, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation );

		if( oFileStandardInfomation.Directory == TRUE ) {

			ZwClose( hFileHandle );

			return FALSE;
		}
		nFileSize = (ULONG)(oFileStandardInfomation.EndOfFile.QuadPart);

		pFileBuffer = (BYTE *)(ExAllocatePool( NonPagedPool, nFileSize + 4 ));

		if( pFileBuffer ) {

			RtlZeroMemory( pFileBuffer, nFileSize + 4 );

			ZwReadFile( hFileHandle, NULL, NULL, NULL, &oIoStatus, pFileBuffer, nFileSize, NULL, NULL );

			ZwClose( hFileHandle );
		}
		else {

			ZwClose( hFileHandle );

			return FALSE;
		}
	}
	else {

		return FALSE;
	}
	ULONGLONG start_times = KeQueryInterruptTime();

	ULONGLONG stop_times = 0;

	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

	command[ 1 ] = 0x0000000B; // MZ0380_CMD_BEGIN_FIRMWARE_DOWNLOAD

	command[ 2 ] = nFileSize;

	if( MZ0380_SendVendorCommand( pDevice, command, 3 ) == TRUE ) {

		{	ULONGLONG start_times = KeQueryInterruptTime();

			ULONGLONG stop_times = 0;

			WRITE_REGISTER_BUFFER_ULONG( (ULONG *)(pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000060), (PULONG)(pFileBuffer), (nFileSize + 3) / (4) );

			stop_times = KeQueryInterruptTime();

			LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES (BUFFER) = %08d (%d BYTES)\n", (ULONG)(stop_times - start_times), nFileSize);
		}
		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

		command[ 1 ] = 0x0000000C; // MZ0380_CMD_END_FIRMWARE_DOWNLOAD

		command[ 2 ] = 0x00000001;

		if( MZ0380_SendVendorCommand( pDevice, command, 3, TRUE, 600000000 ) == TRUE ) {

			if( MZ0380_GetRegister( pDevice, 0, (0x00000004 * 2) ) == 0 ) {
				
				returns = TRUE;
			}
			else {

				returns = FALSE;
			}
		}
		else {

			returns = FALSE;
		}
	}
	else {

		returns = FALSE;
	}
	stop_times = KeQueryInterruptTime();

	if( returns ) {

		LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT( KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}

	// 3.0 FREE WORKING BUFFER
	//
	FREE_MEMORY( pFileBuffer );

	return returns;
*/
}

BOOL MZ0380_GV7601_HwInitialize( CDevice * pDevice ) 
{
	LINUXV4L2_PRINT( KERN_INFO, "[%02d] MZ0380_GV7601_HwInitialize()\n", pDevice->m_nKsDeviceNumber );

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	MZ0380_SetGpioValue( pDevice, 9, 1 ); DELAY_100NS( 500000 );

	MZ0380_SetGpioValue( pDevice, 9, 0 ); DELAY_100NS( 500000 );

	MZ0380_SetGpioValue( pDevice, 9, 1 ); DELAY_100NS( 500000 );

	MZ0380_SetGpioValue( pDevice, 8, 1 ); DELAY_100NS( 500000 );

	MZ0380_SetSpiRegister( pDevice, 0x0009, (11 << 10) | (4 << 5) | (11 << 0) );

	MZ0380_SetSpiRegister( pDevice, 0x0000, (1 << 6) | (1 << 7) );

	MZ0380_SetSpiRegister( pDevice, 0x000A, 0x0004 );

	p_sys_cfg->n_input_video_resolution_cx = 0;

	p_sys_cfg->n_input_video_resolution_cy = 0;

	p_sys_cfg->n_input_video_resolution_fps = 0;

	p_sys_cfg->n_input_video_resolution_fps_m = 0;

	p_sys_cfg->n_input_video_resolution_interleaved = 0;

	p_sys_cfg->b_input_video_signal_changed = FALSE;

	p_sys_cfg->n_input_audio_sampling_frequency = 0;

	return TRUE;
}

BOOL MZ0380_TW2968_HwInitialize( CDevice * pDevice ) 
{
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	MZ0380_SetGpioValue( pDevice, 9, 1 ); DELAY_100NS( 500000 );

	MZ0380_SetGpioValue( pDevice, 9, 0 ); DELAY_100NS( 500000 );

	MZ0380_SetGpioValue( pDevice, 9, 1 ); DELAY_100NS( 500000 );

	BYTE page = 0;
	for( page = 0 ; page < 2 ; page++ ) {

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, page );

		// STANDARD DETECT
		//
		BYTE i = 0;
		{	for( i = 0 ; i < 4 ; i++ ) {
		
				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x08 + 0x10 * i, 0x14 );

				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0E + 0x10 * i, 0x07 );

				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0F + 0x10 * i, 0x7F );
			}
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x97, 0x85 );
		}
//		// STANDARD
//		//
//		{	for( BYTE i = 0 ; i < 4 ; i++ ) {
//		
//				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x08 + 0x10 * i, 0x14 );
//
//				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0E + 0x10 * i, 0x00 );
//
//				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0F + 0x10 * i, 0x01 );
//			}
//			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x97, 0x85 );
//		}
//		// STANDARD
//		//
//		{	for( BYTE i = 0 ; i < 4 ; i++ ) {
//		
//				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x08 + 0x10 * i, 0x18 );
//
//				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0E + 0x10 * i, 0x01 );
//
//				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0F + 0x10 * i, 0x02 );
//			}
//			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x97, 0xC5 );
//		}
		for( i = 0 ; i < 4 ; i++ ) {
		
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x01 + 0x10 * i, 0xF6 ); // BRIGHTNESS
			
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x02 + 0x10 * i, 0x6A ); // CONTRAST

			BYTE R03 = MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x03 + 0x10 * i ); // SHARPNESS

			R03 = (R03 & (~0x3F)) | 0x21;

			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x03 + 0x10 * i, R03 ); // SHARPNESS

			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x04 + 0x10 * i, 0x90 ); // SATURATION U

			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x05 + 0x10 * i, 0x90 ); // SATURATION V
		}
		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xAF, 0x00 ); // VSHP

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xB0, 0x00 ); // VSHP
	}
	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 );

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x96, 0xE6 ); // ENABLE BLUE COLOR

	BYTE RFB = MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFB );

	RFB = (RFB & (~0x30)) | 0x2F;

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFB, RFB );

	BYTE RFC = MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFC );

	RFC = (RFC & (~0x0F)) | 0x0F;

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFC, RFC );

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 );

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFB, 0x2F ); // VIDEO AND AUDIO DETECTION MODE, ENABLE IRQ

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x9E, 0x52 ); // ENABLE CHANNEL ID

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xCD, 0xE4 );

	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x5B, 0x03 ); // CLOCK DRIVE

	BOOL is_wd1_mode = TRUE;

	if( (pDevice->iManufacturer & 0x0F) == 0x0C ) {

		is_wd1_mode = FALSE;
	}
	else {
		
		is_wd1_mode = TRUE;
	}
	if( is_wd1_mode ) {

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x6F, 0xF0 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xE7, 0xAA );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xE8, 0x10 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xE9, 0x32 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xEA, 0x54 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xEB, 0x76 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xCC, 0xFF );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xCD, 0xFF );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xF9, 0x03 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFA, 0x4A );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFB, 0x2F );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x9F, 0x33 );
	}
	else {

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 ) ;

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x6F, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xE7, 0xAA );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xE8, 0x10 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xE9, 0x32 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xEA, 0x54 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xEB, 0x76 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xCC, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xCD, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xF9, 0x00 );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFA, 0x4A );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0xFB, 0x2F );

		MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x9F, 0x00 );
	}

	BYTE i = 0;
	for( i = 0 ; i < 4 ; i++ ) {

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x00, 0x00 ); // SWITCH TO PAGE@0

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x01, 0x01 ); //

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x12, 0x81 ); // NADC        = 12.288 MHZ

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x13, 0x82 ); // MADC /   2  =   6144 KHZ

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x14, 0x80 ); // AOSR / 128  =     48 KHZ

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x1E, 0x88 ); // ADC_CLK     =  1.536 MHZ = 48K X STEREO X 16BIT

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x1B, 0x0C ); // [4:5] 0: 16 BITS, 1: 20 BITS, 2: 24BITS, 3: 36BITS

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x00, 0x00 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x00, 0x01 ); // SWITCH TO PAGE@1

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x00, 0x01 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x33, 0x00 ); // MICBIAS1, MICBIAS2 IS POWERED DOWN

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x34, 0xFC ); // LEFT ADC INPUT SELECTION FOR LEFT PGA, LCH_SEL1; USED FOR THE IN1L(P) PIN, WHICH IS SINGLE-ENDED
		
		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x37, 0xFC ); // RIGHT ADC INPUT SELECTION FOR RIGHT PGA, RCH_SEL1; USED FOR THE IN1R(M) PIN, WHICH IS SINGLE-ENDED

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x3B, 0x00 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x3C, 0x00 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x00, 0x00 ); // SWITCH TO PAGE@0

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x3D, 0x02 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x51, 0x00 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x00, 0x00 ); // SWITCH TO PAGE@0

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x51, 0xC2 );

		MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30 + i * 0x02, 0x52, 0x00 );
	}
	return TRUE;
}

BOOL MZ0380_TW9910_HwInitialize( CDevice * pDevice ) 
{
	if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1
	
	 	(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

		;
	}
	else if( (pDevice->iManufacturer == 0xC0) ) { // HD

		if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||  // COMPOSITE

			pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) { // SVIDEO

			MZ0380_SetGpioValue( pDevice, 9, 1 ); DELAY_100NS( 500000 );

			MZ0380_SetGpioValue( pDevice, 9, 0 ); DELAY_100NS( 500000 );

			MZ0380_SetGpioValue( pDevice, 9, 1 ); DELAY_100NS( 500000 );

			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x06, 0x00 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x03, 0xA2 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x05, 0x01 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x08, 0x14 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x09, 0xF2 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0A, 0x0B );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0B, 0xD2 );
		//	MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x19, 0x57 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x19, 0xDE );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1A, 0x0F );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1B, 0x00 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x28, 0x0E );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x29, 0x03 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x2D, 0x07 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x2F, 0x06 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x4C, 0x0D );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x55, 0x00 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x6B, 0x26 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x6C, 0x36 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x6D, 0xF0 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x6E, 0x28 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x06, 0x80 );

			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x6F, 0x98 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x64, 0x55 );
			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x06, 0x80 );

			MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

			p_sys_cfg->n_input_video_resolution_cx = 0;

			p_sys_cfg->n_input_video_resolution_cy = 0;

			p_sys_cfg->n_input_video_resolution_fps = 0;

			p_sys_cfg->n_input_video_resolution_fps_m = 0;

			p_sys_cfg->n_input_video_resolution_interleaved = 0;

			p_sys_cfg->b_input_video_signal_changed = FALSE;

			p_sys_cfg->n_input_audio_sampling_frequency = 0;	
		}
	}
 	return TRUE;
}

NTSTATUS MZ0380_SetAnalogVideoDecoderStandardProperty( CDevice * pDevice, ULONG nStandard, BOOL THREAD )
{
	if( ((pDevice->iProduct & 0x0F) == 0x02 && (pDevice->iManufacturer & 0x0F) >= 0x0A ) ||	// 960H
	
		((pDevice->iProduct & 0x0F) == 0x03 && (pDevice->iManufacturer & 0x0F) >= 0x0A ) ) { // 960H

		if( THREAD == FALSE ) { MZ0380_CLOSE_THREAD(); }

		MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

		ULONG counts = 2;

		if( (pDevice->iManufacturer & 0x0F) == 0x0B	||
		
		 	(pDevice->iManufacturer & 0x0F) == 0x0C ) {

			counts = 1;
		}
		else {

			counts = 2;
		}
		BYTE page = 0;
		for( page = 0 ; page < counts ; page++ ) {

			MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, page );

			if( pDevice->m_nCustomVideoStandardProperty & SUPPORTED_ANALOG_VIDEO_STANDARDS_60HZ ) { // STANDARD

				BYTE i = 0;
				for( i = 0 ; i < 4 ; i++ ) {
			
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x08 + 0x10 * i, 0x14 );

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0E + 0x10 * i, 0x00 );

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0F + 0x10 * i, 0x01 );
				}
				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x97, 0x85 );

				if( (pDevice->iManufacturer & 0x0F) == 0x0A ||  // 960H
					
					(pDevice->iManufacturer & 0x0F) == 0x0B ) { // 960H

					p_sys_cfg->n_input_video_resolution_cx = 960;
				}
				else if( (pDevice->iManufacturer & 0x0F) == 0x0C ) { // 960H -> D1

					p_sys_cfg->n_input_video_resolution_cx = 720;
				}
				else { //

					p_sys_cfg->n_input_video_resolution_cx = 720;
				}
				p_sys_cfg->n_input_video_resolution_cy = 240;

				p_sys_cfg->n_input_video_resolution_fps = 60;

				p_sys_cfg->n_input_video_resolution_fps_m = 1;

				p_sys_cfg->n_input_video_resolution_interleaved = 1;

				p_sys_cfg->b_input_video_signal_changed = TRUE;

				p_sys_cfg->n_input_audio_sampling_frequency = 48000;

				pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) | 

																  (p_sys_cfg->n_input_video_resolution_cy <<  0);

				pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);

				pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);

				pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 48000;
			}
			if( pDevice->m_nCustomVideoStandardProperty & SUPPORTED_ANALOG_VIDEO_STANDARDS_50HZ ) { // STANDARD

				BYTE i = 0;
				for( i = 0 ; i < 4 ; i++ ) {
			
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x08 + 0x10 * i, 0x18 );

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0E + 0x10 * i, 0x01 );

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x0F + 0x10 * i, 0x02 );
				}
				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x97, 0xC5 );

				if( (pDevice->iManufacturer & 0x0F) == 0x0A ||  // 960H
					
					(pDevice->iManufacturer & 0x0F) == 0x0B ) { // 960H

					p_sys_cfg->n_input_video_resolution_cx = 960;
				}
				else if( (pDevice->iManufacturer & 0x0F) == 0x0C ) { // 960H -> D1

					p_sys_cfg->n_input_video_resolution_cx = 720;
				}
				else { //

					p_sys_cfg->n_input_video_resolution_cx = 720;
				}
				p_sys_cfg->n_input_video_resolution_cy = 288;

				p_sys_cfg->n_input_video_resolution_fps = 50;

				p_sys_cfg->n_input_video_resolution_fps_m = 1;

				p_sys_cfg->n_input_video_resolution_interleaved = 1;

				p_sys_cfg->b_input_video_signal_changed = TRUE;

				p_sys_cfg->n_input_audio_sampling_frequency = 48000;

				pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) | 

																  (p_sys_cfg->n_input_video_resolution_cy <<  0);

				pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);

				pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);

				pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 48000;
			}
		}
		if( THREAD == FALSE ) { MZ0380_START_THREAD(); }
	}
	else if( (pDevice->iProduct & 0x0F) == 0x02 ||  //
		
			 (pDevice->iProduct & 0x0F) == 0x03 ) { //

	}
	else { // HD
		
		if( pDevice->iManufacturer == 0xC0 ) {
		
			if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||  // COMPOSITE
	
				pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) { // SVIDEO
	
				if( THREAD == FALSE ) { MZ0380_CLOSE_THREAD(); }
	
				MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
				BYTE R0C = (BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C )) & 0xEF;
	
				switch( pDevice->m_nCustomVideoStandardProperty ) {
				
				case V4L2_STD_NTSC_M:      MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x00 );  break;
				case V4L2_STD_NTSC_M_JP:   MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x00 );  break;
				case V4L2_STD_NTSC_443:    MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x03 );  break;
				case V4L2_STD_PAL_60:      MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x07 );  break;
				case V4L2_STD_PAL_M:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x04 );  break;
				case V4L2_STD_PAL_B:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x01 );  break;
				case V4L2_STD_PAL_D:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x01 );  break;
				case V4L2_STD_PAL_G:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x01 );  break;
				case V4L2_STD_PAL_H:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x01 );  break;
				case V4L2_STD_PAL_I:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x01 );  break;
				case V4L2_STD_PAL_N:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x05 );  break;
				case V4L2_STD_PAL_Nc: 	   MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_B:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_G:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_H:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_D:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_K:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_K1:    MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_L:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				case V4L2_STD_SECAM_LC:    MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x1C, 0x02 );  break;
				}
				switch( pDevice->m_nCustomVideoStandardProperty ) {
				
				case V4L2_STD_NTSC_M:      MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x10 );  break;
				case V4L2_STD_NTSC_M_JP:   MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_NTSC_443:    MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x10 );  break;
				case V4L2_STD_PAL_60:      MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_PAL_M:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x10 );  break;
				case V4L2_STD_PAL_B:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_PAL_D:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_PAL_G:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_PAL_H:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_PAL_I:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_PAL_N:       MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x10 );  break;
				case V4L2_STD_PAL_Nc:	   MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_B:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_G:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_H:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_D:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_K:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_K1:    MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_L:     MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				case V4L2_STD_SECAM_LC:    MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0C, R0C | 0x00 );  break;
				}
				if( pDevice->m_nCustomVideoStandardProperty & SUPPORTED_ANALOG_VIDEO_STANDARDS_60HZ ) {
	
					BYTE R07 = (BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x07 )) & 0x0F;
					
					BYTE R55 = (BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x55 )) & 0xEF;
	
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x07, 0x00 | R07 );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x08, 0x14       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x09, 0xF2       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0A, 0x0B       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0B, 0xD2       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x55, 0x10 | R55 );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0D, 0x00       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0E, 0x11       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0F, 0x00       );
	
					p_sys_cfg->n_input_video_resolution_cx = 720;
	
					p_sys_cfg->n_input_video_resolution_cy = 240;
	
					p_sys_cfg->n_input_video_resolution_fps = 60;
	
					p_sys_cfg->n_input_video_resolution_fps_m = 1;
	
					p_sys_cfg->n_input_video_resolution_interleaved = 1;
	
					p_sys_cfg->b_input_video_signal_changed = TRUE;
	
					p_sys_cfg->n_input_audio_sampling_frequency = 48000;
	
					pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) | 
	
																	  (p_sys_cfg->n_input_video_resolution_cy <<  0);
	
					pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);
	
					pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);
	
					pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 48000;
				}
				if( pDevice->m_nCustomVideoStandardProperty & SUPPORTED_ANALOG_VIDEO_STANDARDS_50HZ ) {
	
					BYTE R07 = (BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x07 )) & 0x0F;
					
					BYTE R55 = (BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x55 )) & 0xEF;
	
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x07, 0x10 | R07 );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x08, 0x19 +   0 );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x09, 0x20       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0A, 0x0A       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0B, 0xD2       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x55, 0x00 | R55 );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0D, 0x00       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0E, 0x11       );
					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x0F, 0x00       );	
	
					p_sys_cfg->n_input_video_resolution_cx = 720;
	
					p_sys_cfg->n_input_video_resolution_cy = 288;
	
					p_sys_cfg->n_input_video_resolution_fps = 50;
	
					p_sys_cfg->n_input_video_resolution_fps_m = 1;
	
					p_sys_cfg->n_input_video_resolution_interleaved = 1;
	
					p_sys_cfg->b_input_video_signal_changed = TRUE;
	
					p_sys_cfg->n_input_audio_sampling_frequency = 48000;
	
					pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) | 
	
																	  (p_sys_cfg->n_input_video_resolution_cy <<  0);
	
					pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);
	
					pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);
	
					pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 48000;
				}
				if( THREAD == FALSE ) { MZ0380_START_THREAD(); }
			}
		}
	}
	return STATUS_SUCCESS; 
}

NTSTATUS MZ0380_SetAnalogCrossbarRouteProperty( CDevice * pDevice, ULONG nVideoInput, ULONG nVideoOutput, ULONG nAudioInput, ULONG nAudioOutput, BOOL THREAD )
{
	if( THREAD == FALSE ) { MZ0380_CLOSE_THREAD(); }

	if( nVideoInput != -1 ) {

		if( (pDevice->iProduct & 0x0F) == 0x02 ||   // 960H & D1
			
			(pDevice->iProduct & 0x0F) == 0x03  ) { // 960H & D1

			if( (pDevice->iManufacturer & 0x0F) >= 0x0A ) {

				MZ0380_TW2968_HwInitialize( pDevice );
		}
			else {

				;
			}
		}
		else if( pDevice->iManufacturer == 0xC4 ||  // SC510N1 MC SDI
			
				 pDevice->iManufacturer == 0xCB ) { // SC510N4 SDI

			MZ0380_GV7601_HwInitialize( pDevice );
		}
		else if( pDevice->iManufacturer == 0xCC ) { // MC HDV

			MST3367_HwInitialize( pDevice );
		}
		else { // HD

			if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

				pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) {

				MZ0380_TW9910_HwInitialize( pDevice );

				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ) { // COMPOSITE

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x02, 0x0040 );	

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x06, 0x0042 );	
				}
				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) { // SVIDEO

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x02, 0x0054 );	

					MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x06, 0x0040 );	
				}
			}
			else if( pDevice->m_nAnalogCrossbarVideoInputProperty == 4 ) { // HD
				
				MZ0380_GV7601_HwInitialize( pDevice );

				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x03, 0xA7 ); // TRI-STATE
			}
			else {

				MST3367_HwInitialize( pDevice );

				MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x03, 0xA7 ); // TRI-STATE
			}
		}
		pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

		pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

		pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

		pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

		pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

		pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;	
		
		if( pDevice->iManufacturer == 0xC0 ) {

			if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

				pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) {

				MZ0380_SetAnalogVideoDecoderStandardProperty( pDevice, NULL, TRUE );
			}
		}
	}
	if( nAudioInput != -1 ) {

		BYTE  I2S_SEL = 0;

		if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

			pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ||  // DVI.DIGITAL (TMDS.A)

			pDevice->m_nAnalogCrossbarVideoInputProperty == 4 ) { // SDI

			if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

				I2S_SEL = 0;
			}
			if( pDevice->m_nAnalogCrossbarAudioInputProperty == 1 ) {

				I2S_SEL = 1;
			}
			if( pDevice->m_nAnalogCrossbarAudioInputProperty >= 2 ) {

				I2S_SEL = 2;
			}
		}
		if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||  // COMPONENT

			pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ||  // DVI.ANALOG

			pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||  // CMOPOSITE

			pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) { // SVIDEO

			if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

				I2S_SEL = 1;
			}
			if( pDevice->m_nAnalogCrossbarAudioInputProperty == 1 ) {

				I2S_SEL = 1;
			}
			if( pDevice->m_nAnalogCrossbarAudioInputProperty >= 2 ) {

				I2S_SEL = 2;
			}
		}
		if( (pDevice->iProduct & 0x0F) == 0x02 ||
			
			(pDevice->iProduct & 0x0F) == 0x03 ) {

			;
		}
		else {
			
			if( pDevice->iManufacturer == 0xC0 ||  // SC5C0N1 FULL
				
				pDevice->iManufacturer == 0xC4 ||  // SC5C0N1 MC SDI
				
				pDevice->iManufacturer == 0xCB ||  // SC5C0N4 SDI

				pDevice->iManufacturer == 0xCC ) { // SC5C0N1 MC HDV

				if( I2S_SEL == 0 ) {

					if( pDevice->iManufacturer == 0xC4 ||
						
						pDevice->iManufacturer == 0xCB ) {
						
						MZ0380_SetGpioValue( pDevice, 10, 1 ); // I2S.SEL

						MZ0380_SetGpioValue( pDevice, 11, 0 ); // I2S.TRI
					}
					else if( pDevice->iManufacturer == 0xCC ) {
						
						MZ0380_SetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, 0x02, 0x27, 0x00 );

						MZ0380_SetGpioValue( pDevice, 10, 1 ); // I2S.SEL

						MZ0380_SetGpioValue( pDevice, 11, 1 ); // I2S.TRI
					}
					else {
					
						if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

							pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { // DVI.DIGITAL (TMDS.A)

							MZ0380_SetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, 0x02, 0x27, 0x00 );

							MZ0380_SetGpioValue( pDevice, 10, 1 ); // I2S.SEL

							MZ0380_SetGpioValue( pDevice, 11, 1 ); // I2S.TRI
						}
						else { // SDI
				
							MZ0380_SetGpioValue( pDevice, 10, 1 ); // I2S.SEL

							MZ0380_SetGpioValue( pDevice, 11, 0 ); // I2S.TRI
						}
					}
				}
				else {

					if( pDevice->iManufacturer == 0xC4 ||
						
						pDevice->iManufacturer == 0xCB ) {
						
						MZ0380_SetGpioValue( pDevice, 10, 0 ); // I2S.SEL

						MZ0380_SetGpioValue( pDevice, 11, 0 ); // I2S.TRI
					}
					else if( pDevice->iManufacturer == 0xCC ) {
						
						MZ0380_SetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, 0x02, 0x27, 0xFF ); // TRI-STATE

						MZ0380_SetGpioValue( pDevice, 10, 0 ); // I2S.SEL

						MZ0380_SetGpioValue( pDevice, 11, 0 ); // I2S.TRI
					}
					else {

						MZ0380_SetAnalogVideoDecoderRegister_EX( pDevice, 0x00, 0x9C, 0x02, 0x27, 0xFF ); // TRI-STATE
					
						MZ0380_SetGpioValue( pDevice, 10, 0 ); // I2S.SEL

						MZ0380_SetGpioValue( pDevice, 11, 0 ); // I2S.TRI
					}
					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x00 ); // SWITCH TO PAGE@0

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x01, 0x01 ); //

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x12, 0x81 ); // NADC        = 12.288 MHZ

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x13, 0x82 ); // MADC /   2  =   6144 KHZ

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x14, 0x80 ); // AOSR / 128  =     48 KHZ

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x1E, 0x88 ); // ADC_CLK     =  1.536 MHZ = 48K X STEREO X 16BIT

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x1B, 0x0C ); // [4:5] 0: 16 BITS, 1: 20 BITS, 2: 24BITS, 3: 36BITS

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x00 );

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x01 ); // SWITCH TO PAGE@1

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x01 );

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x33, 0x00 ); // MICBIAS1, MICBIAS2 IS POWERED DOWN

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x34, 0xFC ); // LEFT ADC INPUT SELECTION FOR LEFT PGA, LCH_SEL1; USED FOR THE IN1L(P) PIN, WHICH IS SINGLE-ENDED

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x37, 0xFC ); // RIGHT ADC INPUT SELECTION FOR RIGHT PGA, RCH_SEL1; USED FOR THE IN1R(M) PIN, WHICH IS SINGLE-ENDED

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x3B, 0x00 );

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x3C, 0x00 );

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x00 ); // SWITCH TO PAGE@0

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x3D, 0x02 );

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x51, 0x00 );

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x00 ); // SWITCH TO PAGE@0

					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x51, 0xC2 );

					if( pDevice->iManufacturer == 0xCB ) {

						if( I2S_SEL == 1 ) {

							BYTE RTEMP = ((0x00 << 4) | 0xCF); // ADC3101_SIGNAL_IN3
	
							MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x01 ); // SWITCH TO PAGE@1

							MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x34, RTEMP );
	
							MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x37, RTEMP );
						}
						else {

							BYTE RTEMP = (( 0x00 << 0 ) | 0xFC); // ADC3101_SINGLE_IN1

							MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x00, 0x01 ); // SWITCH TO PAGE@1

							MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x34, RTEMP );
	
							MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x37, RTEMP );
						}
					}
					MZ0380_SetAnalogAudioDecoderRegister( pDevice, 0, 0x30, 0x52, 0x00 );
				}
			}
		}
	}
	if( THREAD == FALSE ) { MZ0380_START_THREAD(); }

	return STATUS_SUCCESS;
}
extern void MST3367_ADJUST_CSC_TABLE( CDevice * pDevice, BYTE bright, BYTE contrast, BYTE saturation, BYTE hue, BYTE sharpness );

NTSTATUS MZ0380_SetAnalogVideoProcAmpProperties( CDevice * pDevice )
{
	MZ0380_CLOSE_THREAD();

	if( (pDevice->iProduct & 0x0F) == 0x02 ||
		
		(pDevice->iProduct & 0x0F) == 0x03 ) {

	}
	else {
		
		if( pDevice->iManufacturer == 0xC4 ||
			
			pDevice->iManufacturer == 0xCB ) {

			;
		}
		else {

			if( pDevice->m_nAnalogCrossbarVideoInputProperty <= 3 ) {
/*
			MST3367_ADJUST_CSC_TABLE( pDevice, (BYTE)(pDevice->m_nAnalogVideoProcAmpBrightnessProperty[ 0 ] & 0xFF), 
				
											   (BYTE)(pDevice->m_nAnalogVideoProcAmpContrastProperty[ 0 ] & 0xFF), 
											   
											   (BYTE)(pDevice->m_nAnalogVideoProcAmpSaturationProperty[ 0 ] & 0xFF), 
											   
											   (BYTE)(pDevice->m_nAnalogVideoProcAmpHueProperty[ 0 ] & 0xFF), 

											  ((BYTE)(pDevice->m_nAnalogVideoProcAmpSharpnessProperty[ 0 ] & 0xFF) >> 5) );
*/
			MST3367_ADJUST_CSC_TABLE( pDevice, (BYTE)(g_n_analog_decoder_brightness[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF), 
				
											   (BYTE)(g_n_analog_decoder_contrast[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF), 
											   
											   (BYTE)(g_n_analog_decoder_saturation[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF), 
											   
											   (BYTE)(g_n_analog_decoder_hue[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ]  & 0xFF), 
												   
											  ((BYTE)(g_n_analog_decoder_sharpness[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF) >> 5) );

			}
		}
	}
	MZ0380_START_THREAD();

	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_StopFirmware( CDevice * pDevice )
{
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

	command[ 1 ] = 0x00000007; // MZ0380_CMD_STOP_DMA

	command[ 2 ] = 0xFFFFFFFF;

	MZ0380_SendVendorCommand_P3( pDevice, command, 3 );

	p_sys_cfg->n_is_firmware_ready &= 0x000000FF;

	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_StartFirmware( CDevice * pDevice )
{
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_StartFirmware( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

	ULONG counts = 1;

	if( (pDevice->iProduct & 0x0F) == 0x02 ||
		
		(pDevice->iProduct & 0x0F) == 0x03 ) {

		if( (pDevice->iManufacturer & 0x0F) == 0x01 ||
			
			(pDevice->iManufacturer & 0x0F) == 0x0B ||
			
			(pDevice->iManufacturer & 0x0F) == 0x0C ) {

			counts = 4;
		}
		else {

			counts = 8;
		}
	}

	LINUXV4L2_PRINT( KERN_INFO,"------------------------------------ n_is_firmware_ready(%08X) %d\n", p_sys_cfg->n_is_firmware_ready, counts );

	if( (p_sys_cfg->n_is_firmware_ready & 0x0001FFF) == (0x000000F1) ) {

		MZ0380_SetRegister( pDevice, 0, 0x00000050, 0x00000000 );

		MZ0380_SetRegister( pDevice, 0, 0x00000054, 0x00000000 );

		MZ0380_SetRegister( pDevice, 0, 0x00000058, 0x00000000 );

		MZ0380_SetRegister( pDevice, 0, 0x0000005C, 0x00000000 );

		ULONG i = 0;
		for( i = 0 ; i < counts ; i++ ) {

			ULONG deinterlaces[ 2 ] = { pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ i + 0 ] & 0x77FFFFFF,   // 0 = OFF / 1 = ON
					
										pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ i + 8 ] & 0x77FFFFFF }; // 0 = OFF / 1 = ON
	
			ULONG profiles[ 2 ] = { pDevice->m_nCustomEncoderVideoProfileProperty[ i + 0 ] & 0x77FFFFFF,   // 0 = HIGH PROFILE / 1 = BASELINE / 2 = MAIN PROFILE / 3 = HIGH PROFILE
					
									pDevice->m_nCustomEncoderVideoProfileProperty[ i + 8 ] & 0x77FFFFFF }; // 0 = HIGH PROFILE / 1 = BASELINE / 2 = MAIN PROFILE / 3 = HIGH PROFILE
	
			ULONG entropys[ 2 ] = { pDevice->m_nCustomEncoderVideoEntropyProperty[ i + 0 ] & 0x77FFFFFF,   // 0 = CABAC / 1 = CAVLC / 2 = CABAC
					
									pDevice->m_nCustomEncoderVideoEntropyProperty[ i + 8 ] & 0x77FFFFFF }; // 0 = CABAC / 1 = CAVLC / 2 = CABAC
	
			ULONG aspect_ratios_x[ 2 ] = { (pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + 0 ] & 0x00FFF000) >> 12, 
					
											(pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + 8 ] & 0x00FFF000) >> 12 }; 
	
			ULONG aspect_ratios_y[ 2 ] = { (pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + 0 ] & 0x00000FFF) >> 0, 
				
										(pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + 8 ] & 0x00000FFF) >> 0 }; 

			ULONG resolutions_x[ 2 ] = { (pDevice->m_nCustomEncoderVideoResolutionProperty[ i + 0 ] & 0x00FFF000) >> 12,
					
									     (pDevice->m_nCustomEncoderVideoResolutionProperty[ i + 8 ] & 0x00FFF000) >> 12 };

			ULONG resolutions_y[ 2 ] = { (pDevice->m_nCustomEncoderVideoResolutionProperty[ i + 0 ] & 0x00000FFF) >> 0,
					
									     (pDevice->m_nCustomEncoderVideoResolutionProperty[ i + 8 ] & 0x00000FFF) >> 0 };

			ULONG avgs[ 4 ] = { pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + 0 ] & 0x77FFFFFF,
					
								pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + 8 ] & 0x77FFFFFF,
					
								pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 0 ] & 0x77FFFFFF,
					
								pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 8 ] & 0x77FFFFFF };

			ULONG skips[ 4 ] = { (pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + 0 ] & 0x77FFFFFF) + (1), 
					
								 (pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + 8 ] & 0x77FFFFFF) + (1), 
					
								 (pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 0 ] & 0x77FFFFFF) + (1), 
					
								 (pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 8 ] & 0x77FFFFFF) + (1) };
	
			ULONG modes[ 2 ] = { pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + 0 ] & 0x77FFFFFF,
					
									pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + 8 ] & 0x77FFFFFF };
	
			ULONG qualitys[ 2 ] = { pDevice->m_nAnalogVideoCompressionQualityProperty[ i + 0 ] & 0x77FFFFFF,
	
									pDevice->m_nAnalogVideoCompressionQualityProperty[ i + 8 ] & 0x77FFFFFF };
	
			ULONG bitrates[ 2 ] = { pDevice->m_nCustomEncoderVideoBitRateProperty[ i + 0 ] & 0x77FFFFFF,
					
									pDevice->m_nCustomEncoderVideoBitRateProperty[ i + 8 ] & 0x77FFFFFF };
	
			ULONG gops[ 2 ] = { pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + 0 ] & 0x77FFFFFF,
					
								pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + 8 ] & 0x77FFFFFF };
	
			ULONG cx = p_sys_cfg->n_input_video_resolution_cx;
	
			ULONG cy = p_sys_cfg->n_input_video_resolution_cy;
	
			ULONG fps = p_sys_cfg->n_input_video_resolution_fps;
	
			ULONG interleaved = p_sys_cfg->n_input_video_resolution_interleaved;

			ULONG freq = p_sys_cfg->n_input_audio_sampling_frequency;
	
				BOOL  is_down_scaling = FALSE;
	
			pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ i + 8 ] &= 0x77FFFFFF;

			pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 0 ] &= 0x77FFFFFF;

			pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 8 ] &= 0x77FFFFFF;

			pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 0 ] &= 0x77FFFFFF;

			pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 8 ] &= 0x77FFFFFF;

			pDevice->m_nCustomEncoderVideoProfileProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoProfileProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoEntropyProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoEntropyProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + 8 ] &= 0x77FFFFFF;

			pDevice->m_nCustomEncoderVideoResolutionProperty[ i + 0 ] &= 0x77FFFFFF;

			pDevice->m_nCustomEncoderVideoResolutionProperty[ i + 8 ] &= 0x77FFFFFF;

			pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoBitRateProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nCustomEncoderVideoBitRateProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nAnalogVideoCompressionQualityProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nAnalogVideoCompressionQualityProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + 0 ] &= 0x77FFFFFF;
	
			pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + 8 ] &= 0x77FFFFFF;
	
			pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ i + 0 ] = 0;
	
			pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ i + 8 ] = 0;
	
			ULONG j = 0;
			for( j = 0 ; j < 2 ; j++ ) {
	
				if( skips[ j ] == 1 ) {
						
					skips[ j ] = 0;
				}		
				if( avgs[ j ] > 0 ) {
	
					skips[ j ] = 0;
				}
		//		{	qualitys[ j ] *= 51;
		//
		//			qualitys[ j ] /= 10000;
		//
		//			if( qualitys[ j ] > 51 ) { qualitys[ j ] = 51; }
		//
		//			qualitys[ j ] = 51 - qualitys[ j ];
		//		}
				{	qualitys[ j ] /= 250; // QP RANGE: 8 ~ 48
	
					if( qualitys[ j ] > 40 ) { qualitys[ j ] = 40; }
	
					qualitys[ j ] = (40 - qualitys[ j ]) + 8;
				}
			}
			for( j = 2 ; j < 4 ; j++ ) {

				if( skips[ j ] == 1 ) {
						
					skips[ j ] = 0;
				}		
				if( avgs[ j ] > 0 ) {

					skips[ j ] = 0;
				}
			}
			if( interleaved ) {
	
				cy <<= 1;
	
				fps >>= 1;
			}
			if( cx == 0 ||
	
				cy == 0 ||
	
				fps == 0 ||
					
				freq == 0 ) {
	
			//	cx = 1920; cy = 1080; fps = 60; freq = 48000;
	
				cx = 0; cy = 0; fps = 0; freq = 0;
			}
			if( (pDevice->iProduct & 0x0F) == 0x05 ) {
	
				if( pDevice->m_nAnalogVideoDecoderStatusProperty == 0 ) {
	
					cx = 0; cy = 0; fps = 0; freq = 0;
				}
			}
			if( resolutions_x[ 0 ] > 0 &&
				
				resolutions_y[ 0 ] > 0 ) {

				if( resolutions_x[ 0 ] <= cx &&
				
					resolutions_y[ 0 ] <= cy ) {

					is_down_scaling = TRUE;
				}
				if( resolutions_x[ 0 ] == cx &&
				
					resolutions_y[ 0 ] == cy ) {

					is_down_scaling = FALSE;
				}
			}
			if( is_down_scaling == FALSE ) {

				resolutions_x[ 0 ] = 0;

				resolutions_y[ 0 ] = 0;

				resolutions_x[ 1 ] = 0;

				resolutions_y[ 1 ] = 0;
			}
			if( cx > 0 &&
	
				cy > 0 ) {
	
				{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
					ULONG fw = 0;
	
					ULONG vi = 6; // 1:8-BITS RAW, 2:CCIR656I, 3:CCIR656P, 4: BAYER, 5:16-BITS RAW, 6: BT1120P, 7: BT1120I
	
					if( (pDevice->iProduct & 0x0F) == 0x02 ||
						
						(pDevice->iProduct & 0x0F) == 0x03 ) {

						if( (pDevice->iManufacturer & 0x0F) == 0x0C ) {
	
						fw = 4;
	
						vi = 2;
					}
					else {
	
							fw = 5;

							vi = 2;
						}
					}
					else {

						if( (cx * cy * fps) > (1920 * 1200 * 30) ) { 
	
								fw = (is_down_scaling) ? 5 : 7;
						}
						else {
	
							fw = 5;
						}
						if( (pDevice->m_nAnalogCrossbarVideoInputProperty == 4 && 
	
								cx == 720 &&
								
								interleaved == 1) ) { // SDI.SD
	
							vi = 2;
						}
						else if( (pDevice->m_nAnalogCrossbarVideoInputProperty == 5 && interleaved == 1) ||
	
									(pDevice->m_nAnalogCrossbarVideoInputProperty == 6 && interleaved == 1) ) {
	
							vi = 2;
						}
						else if( interleaved == 1 ) {
	
							vi = 7;
						}
						else {
	
							vi = 6;
						}
					}
					p_sys_cfg->n_output_video_resolution_vi_fw = fw;
					
					p_sys_cfg->n_output_video_resolution_cx = cx;

					p_sys_cfg->n_output_video_resolution_cy = cy;

					command[ 1 ] = 0x00000029; // MZ0380_CMD_SET_VIC_PARAMS
	
					command[ 2 ] = (vi << 24) | (fw << 16) | (fps << 8) | (i << 0);
	
					command[ 3 ] = (cy << 16) | (cx << 0);
	
					if( MZ0380_SendVendorCommand_P3( pDevice, command, 4 ) ) {
	
						p_sys_cfg->n_is_firmware_ready |= 0x00000100;
					}
				}
				{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
					ULONG chs = 2;
	
					ULONG bits = 16;
	
					if( (pDevice->iProduct & 0x0F) == 0x02 ||
						
						(pDevice->iProduct & 0x0F) == 0x03 ) {
	
						chs = 1;
					}
					else {
	
						chs = 2;
					}
					command[ 1 ] = 0x0000002A; // MZ0380_CMD_SET_AIC_PARAMS
	
					command[ 2 ] = (bits << 16) | (chs << 8) | (i << 0);
	
					command[ 3 ] = (freq << 0);
	
					if( MZ0380_SendVendorCommand_P3( pDevice, command, 4 ) ) {
	
						p_sys_cfg->n_is_firmware_ready |= 0x00000200;
					}
				}
				{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
					ULONG main_or_sub = 0;
	
					ULONG sar_x = aspect_ratios_x[ 0 ] * ((resolutions_y[ 0 ] > 0) ? resolutions_y[ 0 ] : cy);
	
					ULONG sar_y = aspect_ratios_y[ 0 ] * ((resolutions_x[ 0 ] > 0) ? resolutions_x[ 0 ] : cx);
	
					if( ((sar_x % 4) == 0) && ((sar_y % 4) == 0) ) { sar_x /= 4; sar_y /= 4; }
	
					if( ((sar_x % 2) == 0) && ((sar_y % 2) == 0) ) { sar_x /= 2; sar_y /= 2; }
	
					if( ((sar_x % 3) == 0) && ((sar_y % 3) == 0) ) { sar_x /= 3; sar_y /= 3; }
	
					if( ((sar_x % 5) == 0) && ((sar_y % 5) == 0) ) { sar_x /= 5; sar_y /= 5; }
	
					command[ 1 ] = 0x0000002D; // MZ0380_CMD_SET_ENC_PARAMS
	
					command[ 2 ] = 0x00000FFF;
	
					command[ 3 ] = (gops[ 0 ] << 24) | (fps << 16) | (main_or_sub << 8) | (i << 0);
	
					command[ 4 ] = (1 << 24) | (modes[ 0 ] << 16) | (profiles[ 0 ] << 8) | (qualitys[ 0 ] << 0);
	
					command[ 5 ] = (bitrates[ 0 ] << 0);
	
					command[ 6 ] = (entropys[ 0 ] << 16) | (avgs[ 0 ] << 8) | (skips[ 0 ] << 0);
	
					command[ 7 ] = (sar_x << 16) | (sar_y << 0);
	
					command[ 8 ] = (resolutions_x[ 0 ] << 16) | (resolutions_y[ 0 ] << 0);

					LINUXV4L2_PRINT( KERN_INFO, "mask = %08X, i = %d, main_or_sub = %d, fps = %d, gop = %d, quality = %d, profile = %d, entropy = %d, mode = %d, force_key_frame = %d, bitrate = %d, skip = %d, avg = %d, aspect_ratio = %d.%d.%d.%d, res = %d.%d\n", command[ 2 ], i, main_or_sub, fps, gops[ 0 ], qualitys[ 0 ], profiles[ 0 ], entropys[ 0 ], modes[ 0 ], 1, bitrates[ 0 ], skips[ 0 ], avgs[ 0 ], aspect_ratios_x[ 0 ], aspect_ratios_y[ 0 ], sar_x, sar_y, resolutions_x[ 0 ], resolutions_y[ 0 ]);

					if( MZ0380_SendVendorCommand_P3( pDevice, command, 9 ) ) {

						p_sys_cfg->n_is_firmware_ready |= 0x00000400;
					}
				}
				{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
	
					ULONG main_or_sub = 1;
										
					ULONG sar_x = aspect_ratios_x[ 1 ] * ((resolutions_y[ 1 ] > 0) ? resolutions_y[ 1 ] : cy);
	
					ULONG sar_y = aspect_ratios_y[ 1 ] * ((resolutions_x[ 1 ] > 0) ? resolutions_x[ 1 ] : cx);
	
					if( ((sar_x % 4) == 0) && ((sar_y % 4) == 0) ) { sar_x /= 4; sar_y /= 4; }
	
					if( ((sar_x % 2) == 0) && ((sar_y % 2) == 0) ) { sar_x /= 2; sar_y /= 2; }
	
					if( ((sar_x % 3) == 0) && ((sar_y % 3) == 0) ) { sar_x /= 3; sar_y /= 3; }
	
					if( ((sar_x % 5) == 0) && ((sar_y % 5) == 0) ) { sar_x /= 5; sar_y /= 5; }
	
					command[ 1 ] = 0x0000002D; // MZ0380_CMD_SET_ENC_PARAMS

					command[ 2 ] = 0x00000FFF;

					command[ 3 ] = (gops[ 1 ] << 24) | (fps << 16) | (main_or_sub << 8) | (i << 0);
	
					command[ 4 ] = (1 << 24) | (modes[ 1 ] << 16) | (profiles[ 1 ] << 8) | (qualitys[ 1 ] << 0);
	
					command[ 5 ] = (bitrates[ 1 ] << 0);
	
					command[ 6 ] = (entropys[ 1 ] << 16) | (avgs[ 1 ] << 8) | (skips[ 1 ] << 0);
	
					command[ 7 ] = (sar_x << 16) | (sar_y << 0);
	
					command[ 8 ] = (resolutions_x[ 1 ] << 16) | (resolutions_y[ 1 ] << 0);

					LINUXV4L2_PRINT( KERN_INFO, "mask = %08X, i = %d, main_or_sub = %d, fps = %d, gop = %d, quality = %d, profile = %d, entropy = %d, mode = %d, force_key_frame = %d, bitrate = %d, skip = %d, avg = %d, aspect_ratio = %d.%d.%d.%d, res = %d.%d\n", command[ 2 ], i, main_or_sub, fps, gops[ 1 ], qualitys[ 1 ], profiles[ 1 ], entropys[ 1 ], modes[ 1 ], 1, bitrates[ 1 ], skips[ 1 ], avgs[ 1 ], aspect_ratios_x[ 1 ], aspect_ratios_y[ 1 ], sar_x, sar_y, resolutions_x[ 1 ], resolutions_y[ 1 ]);

					if( MZ0380_SendVendorCommand_P3( pDevice, command, 9 ) ) {
	
						p_sys_cfg->n_is_firmware_ready |= 0x00000800;
					}
				}
				{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

					command[ 1 ] = 0x00000031; // MZ0380_CMD_SET_PRE_PARAMS

					command[ 2 ] = 0x00000003;

					command[ 3 ] = (avgs[ 2 ] << 24) | (skips[ 2 ] << 16) | (fps << 8) | (i << 0);

					LINUXV4L2_PRINT( KERN_INFO, "mask = %08X, i = %d, fps = %d, skip = %d, avg = %d\n", command[ 2 ], i, fps, skips[ 2 ], avgs[ 2 ]);

					if( MZ0380_SendVendorCommand_P3( pDevice, command, 4 ) ) {

						p_sys_cfg->n_is_firmware_ready |= 0x00001000;
					}
				}

			}
			else
			{
				LINUXV4L2_PRINT( KERN_INFO, "[%02d] MZ0380_StartFirmware( ) cx(%d) cy(%d), error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", (int)(pDevice->m_nKsDeviceNumber), cx, cy );
			}
		}
	}
	if( p_sys_cfg->n_is_firmware_ready & 0x0000300 ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

		command[ 1 ] = 0x00000006; // MZ0380_CMD_START_DMA

		command[ 2 ] = 0xFFFFFFFF;
			
		if( MZ0380_SendVendorCommand_P3( pDevice, command, 3 ) ) {
			
			p_sys_cfg->b_input_video_signal_changed = FALSE;

			p_sys_cfg->n_is_firmware_ready |= 0x000010000;
		}
	}
	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_UpdateFirmware( CDevice * pDevice, BOOL THREAD )
{
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_is_firmware_ready & 0x000010000 ) {

		if( THREAD == FALSE ) { MZ0380_CLOSE_THREAD(); } 

		ULONG cx = p_sys_cfg->n_input_video_resolution_cx;

		ULONG cy = p_sys_cfg->n_input_video_resolution_cy;

		ULONG fps = p_sys_cfg->n_input_video_resolution_fps;

		ULONG interleaved = p_sys_cfg->n_input_video_resolution_interleaved;

		ULONG counts = 1;

		if( (pDevice->iProduct & 0x0F) == 0x02 ||
			
			(pDevice->iProduct & 0x0F) == 0x03 ) {

			if( (pDevice->iManufacturer & 0x0F) == 0x01 ||
				
				(pDevice->iManufacturer & 0x0F) == 0x0B ||
				
				(pDevice->iManufacturer & 0x0F) == 0x0C ) {

				counts = 4;
			}
			else {

				counts = 8;
			}
		}
		if( interleaved ) {

			cy <<= 1;

			fps >>= 1;
		}
		ULONG i = 0;
		for( i = 0 ; i < counts ; i++ ) {

			{	ULONG mask = 0x00000000;
					
				if( pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 0 ] & 0x88000000 )      { mask |= 0x00000002; pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 0 ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 0 ] & 0x88000000 )     { mask |= 0x00000001; pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 0 ] &= 0x77FFFFFF; }
					
				if( mask != 0x00000000 ) {

					ULONG avg = (pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i + 0 ] & 0x77FFFFFF);

					ULONG skip = (pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i + 0 ] & 0x77FFFFFF) + (1);

					if( skip == 1 ) {
						
						skip = 0;
					}
					if( avg > 0 ) {

						skip = 0;
					}
					{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

						command[ 1 ] = 0x00000031; // MZ0380_CMD_SET_PRE_PARAMS

						command[ 2 ] = (mask);

						command[ 3 ] = (avg << 24) | (skip << 16) | (fps << 8) | (i << 0);

						LINUXV4L2_PRINT( KERN_INFO, ">>>>> mask = %08X, i = %d, fps = %d, skip = %d, avg = %d >>>>>\n", mask, i, fps, skip, avg);

						if( MZ0380_SendVendorCommand_P3( pDevice, command, 4 ) ) {

							;
						}
						LINUXV4L2_PRINT( KERN_INFO, "<<<<< mask = %08X, i = %d, fps = %d, skip = %d, avg = %d <<<<<\n", mask, i, fps, skip, avg);
					}
				}
			}
		}

		for( i = 0 ; i < counts ; i++ ) {

			ULONG j = 0;
			for( j = 0 ; j < 16 ; j += 8 ) {

				ULONG mask = 0x00000000;
		
				if( pDevice->m_nCustomEncoderVideoProfileProperty[ i + j ] & 0x88000000 )          { mask |= 0x00000008; pDevice->m_nCustomEncoderVideoProfileProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nCustomEncoderVideoEntropyProperty[ i + j ] & 0x88000000 )          { mask |= 0x00000200; pDevice->m_nCustomEncoderVideoEntropyProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + j ] & 0x88000000 )      { mask |= 0x00000400; pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + j ] &= 0x77FFFFFF; }

				if( pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + j ] & 0x88000000 )     { mask |= 0x00000100; pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + j ] & 0x88000000 )    { mask |= 0x00000080; pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + j ] & 0x88000000 )      { mask |= 0x00000010; pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nCustomEncoderVideoBitRateProperty[ i + j ] & 0x88000000 )          { mask |= 0x00000040; pDevice->m_nCustomEncoderVideoBitRateProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nAnalogVideoCompressionQualityProperty[ i + j ] & 0x88000000 )      { mask |= 0x00000004; pDevice->m_nAnalogVideoCompressionQualityProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + j ] & 0x88000000 ) { mask |= 0x00000002; pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + j ] &= 0x77FFFFFF; }
					
				if( pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ i + j ] > 0 )      { mask |= 0x00000020; pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ i + j ] = 0; }

				if( mask != 0x00000000 ) {
					
					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_UpdateFirmware( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

					ULONG deinterlace = pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ i + j ] & 0x77FFFFFF; // 0 = OFF / 1 = ON

					ULONG profile = pDevice->m_nCustomEncoderVideoProfileProperty[ i + j ] & 0x77FFFFFF; // 0 = HIGH PROFILE / 1 = BASELINE / 2 = MAIN PROFILE / 3 = HIGH PROFILE

					ULONG entropy = pDevice->m_nCustomEncoderVideoEntropyProperty[ i + j ] & 0x77FFFFFF; // 0 = CABAC / 1 = CAVLC / 2 = CABAC

					ULONG resolutions_x = (pDevice->m_nCustomEncoderVideoResolutionProperty[ i + j ] & 0x00FFF000) >> 12;

					ULONG resolutions_y = (pDevice->m_nCustomEncoderVideoResolutionProperty[ i + j ] & 0x00000FFF) >> 0;

					ULONG aspect_ratios_x = (pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + j ] & 0x00FFF000) >> 12; 

					ULONG aspect_ratios_y = (pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i + j ] & 0x00000FFF) >> 0; 

					ULONG avg = pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i + j ] & 0x77FFFFFF;

					ULONG skip = (pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i + j ] & 0x77FFFFFF) + (1);

					ULONG mode = pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i + j ] & 0x77FFFFFF;

					ULONG bitrate = pDevice->m_nCustomEncoderVideoBitRateProperty[ i + j ] & 0x77FFFFFF;

					ULONG quality = pDevice->m_nAnalogVideoCompressionQualityProperty[ i + j ] & 0x77FFFFFF;

					ULONG gop = pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i + j ] & 0x77FFFFFF;

					ULONG force_key_frame = (mask & 0x00000020) ? 1 : 0;

					BOOL is_down_scaling = FALSE;

					if( resolutions_x > 0 &&
				
						resolutions_y > 0 ) {

						if( resolutions_x <= cx &&
				
							resolutions_y <= cy ) {

							is_down_scaling = TRUE;
						}
						if( resolutions_x == cx &&
				
							resolutions_y == cy ) {

							is_down_scaling = FALSE;
						}
					}
					if( is_down_scaling == FALSE ) {

						resolutions_x = 0;

						resolutions_y = 0;
					}
					if( skip == 1 ) {
						
						skip = 0;
					}
					if( avg > 0 ) {

						skip = 0;
					}
//					{	quality *= 51;
//
//						quality /= 10000;
//
//						if( quality > 51 ) { quality = 51; }
//
//						quality = 51 - quality;
//					}
					{	quality /= 250; // QP RANGE: 8 ~ 48

						if( quality > 40 ) { quality = 40; }

						quality = (40 - quality) + 8;
					}
					{	DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

						ULONG main_or_sub = j / 8;

						ULONG sar_x = aspect_ratios_x * ((resolutions_y > 0) ? resolutions_y : cy);

						ULONG sar_y = aspect_ratios_y * ((resolutions_x > 0) ? resolutions_x : cx);

						if( ((sar_x % 4) == 0) && ((sar_y % 4) == 0) ) { sar_x /= 4; sar_y /= 4; }

						if( ((sar_x % 2) == 0) && ((sar_y % 2) == 0) ) { sar_x /= 2; sar_y /= 2; }

						if( ((sar_x % 3) == 0) && ((sar_y % 3) == 0) ) { sar_x /= 3; sar_y /= 3; }

						if( ((sar_x % 5) == 0) && ((sar_y % 5) == 0) ) { sar_x /= 5; sar_y /= 5; }

						command[ 1 ] = 0x0000002D; // MZ0380_CMD_SET_ENC_PARAMS

						command[ 2 ] = (mask);

						command[ 3 ] = (gop << 24) | (fps << 16) | (main_or_sub << 8) | (i << 0);

						command[ 4 ] = (force_key_frame << 24) | (mode << 16) | (profile << 8) | (quality << 0);

						command[ 5 ] = (bitrate << 0);

						command[ 6 ] = (entropy << 16) | (avg << 8) | (skip << 0);

						command[ 7 ] = (sar_x << 16) | (sar_y << 0);

						command[ 8 ] = (0 << 16) | (0 << 0);

						LINUXV4L2_PRINT( KERN_INFO, ">>>>> mask = %08X, i = %d, main_or_sub = %d, fps = %d, gop = %d, quality = %d, profile = %d, entropy = %d, mode = %d, force_key_frame = %d, bitrate = %d, skip = %d, avg = %d, aspect_ratio = %d.%d.%d.%d, res = %d.%d >>>>>\n", mask, i, main_or_sub, fps, gop, quality, profile, entropy, mode, force_key_frame, bitrate, skip, avg, aspect_ratios_x, aspect_ratios_y, sar_x, sar_y, 0, 0);

						if( MZ0380_SendVendorCommand_P3( pDevice, command, 9 ) ) {

							;
						}
						LINUXV4L2_PRINT( KERN_INFO, "<<<<< mask = %08X, i = %d, main_or_sub = %d, fps = %d, gop = %d, quality = %d, profile = %d, entropy = %d, mode = %d, force_key_frame = %d, bitrate = %d, skip = %d, avg = %d, aspect_ratio = %d.%d.%d.%d, res = %d.%d <<<<<\n", mask, i, main_or_sub, fps, gop, quality, profile, entropy, mode, force_key_frame, bitrate, skip, avg, aspect_ratios_x, aspect_ratios_y, sar_x, sar_y, 0, 0);
					}
				}
			}
		}
		if( THREAD == FALSE ) { MZ0380_START_THREAD(); }
	}
	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_HwAnalogComponentsInitialize( CDevice * pDevice )
{	
//	LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MZ0380_HwAnalogComponentsInitialize()\n", pDevice->m_nKsDeviceNumber );

	if( pDevice->m_nAnalogCaptureStreamPowerReference != 0 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference++;

	}
	else { 
		
		pDevice->m_nAnalogCaptureStreamPowerReference = 1;
	}

	pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 0;

#ifdef MZ0380_COPYPORTECT_ARCHITECTURE

	if( g_copy_protect_unlock_boradsA[ pDevice->m_nKsDeviceBusNumber ] == 0 ) { return STATUS_SUCCESS; }

	if( g_copy_protect_unlock_boradsB[ pDevice->m_nKsDeviceBusNumber ] == 0 ) { return STATUS_SUCCESS; }

#endif

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 1 ) {

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MZ0380_HwAnalogComponentsInitialize()\n", pDevice->m_nKsDeviceNumber );

		MZ0380_CLOSE_THREAD();

		pDevice->m_hInterruptAccessLock = 0x00000001;//enable ISR

		pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] = 0;

		pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] = 0;

		pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] = 0;

		pDevice->m_nDmaBaseCommonBufferNumber[ 3 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 0;

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0;

		MZ0380_StartFirmware( pDevice );

		pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 1;

		MZ0380_START_THREAD();
	}
	if( pDevice->m_nAnalogCaptureStreamPowerReference > 1 ) {

		MZ0380_UpdateFirmware( pDevice, FALSE );
	}
	// ...

	// ...

	// ...

	LINUXV4L2_PRINT( KERN_INFO, "<-------------------------------------------------------------------------------------- START [%d]\n", pDevice->m_nKsDeviceNumber );

	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_HwAnalogComponentsUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_PRINT( KERN_INFO, "--------------------------------------------------------------------------------------> STOP [%d]\n", pDevice->m_nKsDeviceNumber );

	if( pDevice->m_nAnalogCaptureStreamPowerReference == 0 ) return STATUS_SUCCESS;
	
	if( pDevice->m_nAnalogCaptureStreamPowerReference != 1 ) {

		pDevice->m_nAnalogCaptureStreamPowerReference--;

		return STATUS_SUCCESS;
	}
	pDevice->m_nAnalogCaptureStreamPowerReference = 0;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_HwAnalogComponentsUnInitialize()\n", pDevice->m_nKsDeviceNumber);

	MZ0380_CLOSE_THREAD();

	pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] = 0;

	pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] = 0;

	pDevice->m_nDmaBaseCommonBufferNumber[ 2 ] = 0;

	pDevice->m_nDmaBaseCommonBufferNumber[ 3 ] = 0;

	pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 0 ] = 0;

	pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 1 ] = 0;

	pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 2 ] = 0;

	pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] = 0;

	MZ0380_StopFirmware( pDevice );

	pDevice->m_hInterruptAccessLock = 0x00000000;//disable ISR

	MZ0380_START_THREAD();

	// ...

	// ...

	LINUXV4L2_PRINT( KERN_INFO, "<-------------------------------------------------------------------------------------- STOP [%d]\n", pDevice->m_nKsDeviceNumber );

	return MZ0380_PowerDownPeripherals( pDevice );
}

NTSTATUS MZ0380_PowerDownPeripherals( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "MZ0380_PowerDownPeripherals()\n");

	return true;
}


NTSTATUS MZ0380_HwInitialize( CDevice * pDevice )
{
	NTSTATUS status = STATUS_SUCCESS;

	if( (pDevice->idVendor == 0x0907 && (pDevice->idProduct & 0xFFFF) == 0x0100) ||
		(pDevice->idVendor == 0x12AB && (pDevice->idProduct & 0xFFFF) == 0x0380) ) {

		;
	}
	else { 
		
		MZ0380_START_THREAD();

		return STATUS_UNSUCCESSFUL;
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_HwInitialize()\n", pDevice->m_nKsDeviceNumber );

	pDevice->m_pCustomSystemConfigProperty = (MZ0380_SYS_CFG *)wrapper_kmalloc( sizeof(MZ0380_SYS_CFG), GFP_KERNEL );

	memset( pDevice->m_pCustomSystemConfigProperty, 0, sizeof(MZ0380_SYS_CFG) );

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	//ULONG nDevicePropertyBusNumber = 0;

	ULONG nBufferLength;

	//IoGetDeviceProperty( pDevice->m_pPDO, DevicePropertyBusNumber, sizeof(ULONG), &nDevicePropertyBusNumber, &nBufferLength );

	//pDevice->m_nKsDeviceBusNumber = nDevicePropertyBusNumber % 16;

	//LINUXV4L2_PRINT( KERN_INFO, "QP020X_HwInitialize( %d, %d )\n", pDevice->m_nKsDeviceBusNumber, pDevice->m_nKsDeviceNumber);

	//LINUXV4L2_PRINT( KERN_INFO, "QP020X_HwInitialize( %d, %04X%02X%02X )\n", pDevice->m_nKsDeviceBusNumber, pDevice->bcdDevice, pDevice->iProduct, pDevice->iManufacturer);

	MZ0380_SetRegister( pDevice, 1, 0x000000DC, 0x00000002 ); // CLR INTERRUT

	MZ0380_SetRegister( pDevice, 0, 0x00000030, 0x00000000 ); // CLR INTERRUT

	MZ0380_SetRegister( pDevice, 0, 0x00000000, 0x00000400 ); // CLR INTERRUT 

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_HwInitialize() 1\n", pDevice->m_nKsDeviceNumber );

	wrapper_init_waitqueue_head( &pDevice->m_wait_queue_head );

	pDevice->m_hInterruptAccessLock = 0x00000001;//ready for ISR

	p_sys_cfg->n_is_firmware_ready = 0x00000000;

	p_sys_cfg->n_firmware_timeouts = 0;


	ULONG i = 0;
	for( i = 0 ; i < 100 ; i++ ) {

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

		command[ 1 ] = 0x00000001; // MZ0380_CMD_GET_FIRMWARE_READY

		if( MZ0380_SendVendorCommand_P5( pDevice, command, 2, TRUE, 5000000 ) == TRUE ) {

			p_sys_cfg->n_is_firmware_ready |= 0x00000001;

			break ;
		}
	}
	MZ0380_SetRegister( pDevice, 1, 0x000000DC, 0x00000002 ); // CLR INTERRUT

	MZ0380_SetRegister( pDevice, 0, 0x00000030, 0x00000000 ); // CLR INTERRUT

	MZ0380_SetRegister( pDevice, 0, 0x00000000, 0x00000400 ); // CLR INTERRUT 

	if( p_sys_cfg->n_is_firmware_ready == 0x00000000 ) {
		
		MZ0380_START_THREAD();

		return MZ0380_PowerDownPeripherals( pDevice );
	}

	// FIRMWARE UPDATE
	//
	BYTE bMajorVersion = 0;

	BYTE bMinorVersion = 0;



	if( MZ0380_GetFirmwareVersion( pDevice, "MZ0380.FW.TXT", &bMajorVersion, &bMinorVersion ) ) {
		
		LINUXV4L2_PRINT( KERN_INFO, "FIRMWARE VERSION: %d.%d\n", bMajorVersion, bMinorVersion);

		DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

		command[ 1 ] = 0x0000000A; // MZ0380_CMD_GET_FIRMWARE_VERSION

		command[ 2 ] = 0x00000000;

		command[ 3 ] = 0x00000000;

		p_sys_cfg->n_firmware_major_version = bMajorVersion;

		p_sys_cfg->n_firmware_minor_version = bMinorVersion;
		
		if( MZ0380_SendVendorCommand_P3( pDevice, command, 4 ) == TRUE ) {

			DELAY_100NS( 1000000 );

			ULONG R00000008 = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 2) );

			ULONG R0000000C = MZ0380_GetRegister( pDevice, 0, (0x00000004 * 3) );

			LINUXV4L2_PRINT( KERN_INFO, "BOARD VERSION: %d.%d / %d.%d", MZ0380_GetRegister( pDevice, 0, (0x00000004 * 2) ), MZ0380_GetRegister( pDevice, 0, (0x00000004 * 3) ), R00000008, R0000000C);

			
			p_sys_cfg->n_board_major_version = R00000008;

			p_sys_cfg->n_board_minor_version = R0000000C;

			if( MZ0380_GetRegister( pDevice, 0, (0x00000004 * 2) ) != bMajorVersion ||

				MZ0380_GetRegister( pDevice, 0, (0x00000004 * 3) ) != bMinorVersion ) {

				if( MZ0380_DownloadFirmware( pDevice, ((pDevice->iProduct & 0x0F) != 0x05) ? "MZ0380.SD.HEX" : "MZ0380.HD.HEX" ) ) {

					MZ0380_START_THREAD(); // 需要重新開機

					return MZ0380_PowerDownPeripherals( pDevice );
				}
			}
		}
	}

	if( ((pDevice->iProduct & 0x0F) == 0x02 && (pDevice->iManufacturer & 0x0F) == 0x0A) ||	// 960H
	
		((pDevice->iProduct & 0x0F) == 0x03 && (pDevice->iManufacturer & 0x0F) == 0x0A) ) { // 960H
		
		//PHYSICAL_ADDRESS memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer;
		ULONG i = 0;
		for( i = 0 ; i < 8 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ];
		
			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000002; // MZ0380_CMD_SET_PREVIEW_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x000CA900;

			command[ 4 ] =0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000010;
			}
		}

		UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 8 ];

		for( i = 0 ; i < 8 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000003; // MZ0380_CMD_SET_AUDIO_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 4096;

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			memory_physical_address += 4096;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000020;
			}
		}

		for( i = 0 ; i < 8 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i + 16 ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000004; // MZ0380_CMD_SET_MAIN_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x000CA900;

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000040;
			}
		}

		
		for( i = 0 ; i < 8 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i + 24 ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000005; // MZ0380_CMD_SET_SUB_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x00032B00;

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000080;
			}
		}
	}

	else if( ((pDevice->iProduct & 0x0F) == 0x02 && (pDevice->iManufacturer & 0x0F) == 0x0C) ||	// 960H
	
			((pDevice->iProduct & 0x0F) == 0x03 && (pDevice->iManufacturer & 0x0F) == 0x0B) ) { // 960H
		
		//PHYSICAL_ADDRESS memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer;
		ULONG i = 0;
		for( i = 0 ; i < 4 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ];
		
			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000002; // MZ0380_CMD_SET_PREVIEW_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x000CA900;

			command[ 4 ] =0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000010;
			}
		}

		UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 8 ];

		for( i = 0 ; i < 4 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000003; // MZ0380_CMD_SET_AUDIO_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 4096;

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			memory_physical_address += 4096;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000020;
			}
		}

		for( i = 0 ; i < 4 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i + 16 ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000004; // MZ0380_CMD_SET_MAIN_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x000CA900;

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x000CA900;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000040;
			}
		}

		//this is a fake, correct way is to enable below 
		p_sys_cfg->n_is_firmware_ready |= 0x00000080;

		/*
		for( i = 0 ; i < 4 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i + 24 ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000005; // MZ0380_CMD_SET_SUB_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x00032B00;

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000080;
			}
		}
		*/
	}
	else if( (pDevice->iProduct & 0x0F) == 0x02 ||  //
		
			 (pDevice->iProduct & 0x0F) == 0x03 ) { //

		ULONG i = 0;
		for( i = 0 ; i < 8 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000002; // MZ0380_CMD_SET_PREVIEW_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x00097F00; 

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x00097F00;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x00097F00;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x00097F00;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000010;
			}
		}

		UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 8 ];

		for( i = 0 ; i < 8 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000003; // MZ0380_CMD_SET_AUDIO_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 4096; 

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;
	
			memory_physical_address += 4096;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;
	
			memory_physical_address += 4096;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;
	
			memory_physical_address += 4096;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			memory_physical_address += 4096;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000020;
			}

		}

		for( i = 0 ; i < 8 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i + 16 ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000004; // MZ0380_CMD_SET_MAIN_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] =0x00097F00; 

			command[ 4 ] = 0;

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x00097F00;

			command[ 6 ] = 0;

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x00097F00;

			command[ 8 ] = 0;

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x00097F00;

			command[ 10 ] = 0;

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000040;
			}
		}

		for( i = 0 ; i < 8 ; i++ ) {

			UINT memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ i + 24 ];

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000005; // MZ0380_CMD_SET_SUB_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x00032B00; 

			command[ 4 ] = 0; 

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 6 ] = 0; 

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 8 ] = 0; 

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 0x00032B00;

			command[ 10 ] = 0; 

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000080;
			}
		}

	}
	else { // HD

		dma_addr_t memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 0 ];

		ULONG i = 0;
		for( i = 0 ; i < 1 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000002; // MZ0380_CMD_SET_PREVIEW_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x0034BD00; 

			command[ 4 ] = 0;

			command[ 5 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 1 ];

			command[ 6 ] = 0;

			command[ 7 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 2 ];

			command[ 8 ] = 0;

			command[ 9 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 3 ];

			command[ 10 ] = 0;

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000010;
			}
		}

		memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 4 ];

		for( i = 0 ; i < 1 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000003; // MZ0380_CMD_SET_AUDIO_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 4096;

			command[ 4 ] = 0;

			command[ 5 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 6 ] = 0;

			command[ 7 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 8 ] = 0;

			command[ 9 ] = memory_physical_address;

			memory_physical_address += 4096;

			command[ 10 ] = 0;

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000020;
			}
		}

		memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 5 ];
			
		for( i = 0 ; i < 1 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000004; // MZ0380_CMD_SET_MAIN_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x0034BD00;

			command[ 4 ] = 0;

			command[ 5 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 6 ];
	
			command[ 6 ] = 0;

			command[ 7 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 7 ];

			command[ 8 ] = 0;

			command[ 9 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 8 ];

			command[ 10 ] = 0;

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000040;
			}
		}

		memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 9 ];
			
		for( i = 0 ; i < 1 ; i++ ) {

			DWORD command[ 12 ] = { 0x00000800, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

			command[ 1 ] = 0x00000005; // MZ0380_CMD_SET_SUB_ENCODER_BUFFER

			command[ 2 ] = i;

			command[ 3 ] = 0x000D3000;

			command[ 4 ] = 0;

			command[ 5 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 10 ];

			command[ 6 ] = 0;

			command[ 7 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 11 ];

			command[ 8 ] = 0;

			command[ 9 ] = memory_physical_address;

			memory_physical_address = pDevice->m_pDmaBaseCommonPhysicalBuffer[ 12 ];

			command[ 10 ] = 0;

			command[ 11 ] = memory_physical_address;

			if( MZ0380_SendVendorCommand_P3( pDevice, command, 12 ) ) {

				p_sys_cfg->n_is_firmware_ready |= 0x00000080;
			}
		}
	}
	LINUXV4L2_PRINT( KERN_INFO, "MZ0380_HwInitialize( %04X%02X%02X )\n", pDevice->bcdDevice, pDevice->iProduct, pDevice->iManufacturer);

	if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1 
		
		(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1 

		MZ0380_SetGpioDirection( pDevice,  9, 0 ); //
	}
	else { // HD

		MZ0380_SetGpioDirection( pDevice,  1, 0 ); // HPD0.CTRL

		MZ0380_SetGpioDirection( pDevice,  8, 0 ); // VID.SEL

		MZ0380_SetGpioDirection( pDevice,  9, 0 ); //

		MZ0380_SetGpioDirection( pDevice, 10, 0 ); // I2S.SEL

		MZ0380_SetGpioDirection( pDevice, 11, 0 ); // I2S.TRI
	}
	//MZ0380_SetAnalogCrossbarRouteProperty( pDevice, pDevice->m_nAnalogCrossbarVideoInputProperty, pDevice->m_nAnalogCrossbarVideoOutputProperty, pDevice->m_nAnalogCrossbarAudioInputProperty, pDevice->m_nAnalogCrossbarAudioOutputProperty, TRUE );
	
	MZ0380_SetAnalogCrossbarRouteProperty( pDevice, pDevice->m_nAnalogCrossbarVideoInputProperty, 0, pDevice->m_nAnalogCrossbarAudioInputProperty, 0, TRUE );

	MZ0380_SetAnalogVideoDecoderStandardProperty( pDevice, NULL, TRUE );

	//init all parameter here

	for( i = 0; i < 16; i++)
	{
		pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ i ] = 1;

		pDevice->m_nCustomAnalogVideoSkipFrameRateProperty[ i ] = 0;
		pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i ] = 0;
	}

	for( i = 0; i < 32; i++)
	{
		pDevice->m_nCustomEncoderVideoProfileProperty[ i ] = V4L2_MPEG_VIDEO_H264_PROFILE_MAIN;

		pDevice->m_nCustomEncoderVideoEntropyProperty[ i ] = 0;

		pDevice->m_nCustomEncoderVideoAspectRatioProperty[ i ] = 0;

		pDevice->m_nCustomEncoderVideoResolutionProperty[ i ] = 0;

		pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ i ] = 0;

		pDevice->m_nCustomEncoderVideoSkipFrameRateProperty[ i ] = 0;

		pDevice->m_nCustomEncoderVideoBitRateModeProperty[ i ] = 0;

		pDevice->m_nAnalogVideoCompressionQualityProperty[ i ] = 6000; 

		pDevice->m_nCustomEncoderVideoBitRateProperty[ i ] = 4000000; 

		pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ i ] = 30; 

	}
	
	MZ0380_START_THREAD();

	pDevice->m_hInterruptAccessLock = 0x00000000;//disable ISR

	return MZ0380_PowerDownPeripherals( pDevice );

//RETURN_ERROR:

//	MZ0380_START_THREAD();

//	return status;
}

NTSTATUS MZ0380_HwUnInitialize( CDevice * pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_HwUnInitialize()\n", pDevice->m_nKsDeviceNumber);

	//
	//
	MZ0380_SetRegister( pDevice, 1, 0x000000DC, 0x00000002 ); // CLR INTERRUT

	MZ0380_SetRegister( pDevice, 0, 0x00000030, 0x00000000 ); // CLR INTERRUT

	MZ0380_SetRegister( pDevice, 0, 0x00000000, 0x00000400 ); // CLR INTERRUT 

	MZ0380_CLOSE_THREAD();

	FREE( pDevice->m_pCustomSystemConfigProperty );

	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_HwProcessAnalogPCIPacket( CDevice * pDevice, ULONG status, ULONG mask, CVideo * pVideos[ MAX_SUB_DEVICE_NUM_X_2 ], CVideoBuffer * pVideoBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{

	//LINUXV4L2_DEBUG( KERN_INFO,"[%d] MZ0380_HwProcessAnalogPCIPacket() \n", pDevice->m_nKsDeviceNumber );

	if( status & 0x000000FF ) { // VIDEO STREAMS

		ULONG R00000040 = MZ0380_GetRegister( pDevice, 0, 0x00000040 );

		ULONG R00000044 = MZ0380_GetRegister( pDevice, 0, 0x00000044 );

		//preview + H264 is status(0x1) R00000040(0x1010101)
		//audio is status(0x1) R00000040(0x1010000)

		ULONG counts = 1;

		if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1
			
			(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

			if( (pDevice->iManufacturer & 0x0F) == 0x01 ||
				
				(pDevice->iManufacturer & 0x0F) == 0x0B ||
				
				(pDevice->iManufacturer & 0x0F) == 0x0C ) {

				counts = 4;
			}
			else {

				counts = 8;
			}
		}
		else { // HD

			//if( pDevice->m_nCustomCloneFiltersProperty >= 2 ) {

			//	counts = (pDevice->m_nCustomCloneFiltersProperty >= 4) ? (4) : (pDevice->m_nCustomCloneFiltersProperty); // 最多四組
			//}
			//else {

				counts = 1;
			//}
		}
		ULONG i = 0;
		
		for( i = 0 ; i < counts ; i++ ) {

			CVideoBuffer * pVideoBuffer = pVideoBuffers[ i ];
			
			BOOL  is_ready = 0;

			ULONG s = i;

			if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1

				(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

				is_ready = (status & (1 << (i + 0))) ? TRUE : FALSE;

				s = i;
			}
			else { // HD

				is_ready = (status & (1 << (0 + 0))) ? TRUE : FALSE;

				s = 0;
			}
			if( is_ready ) {

				ULONG j = 0;

				if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1 (RING.BUFFER) ||

					(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1 (RING.BUFFER)

					j = (R00000040 >> (i * 4)) & 0x03;
				}
				else { // HD (RING.BUFFER)

					j = (R00000040 >> (0 * 4)) & 0x03;
				}

				if( pVideos[ i ] && pVideoBuffers[ i ] ) {

					pVideoBuffers[ i ]->m_sKsBuffer.input = j;

					p_is_drops[ i ] = FALSE;
					
					p_is_completes[ i ] = TRUE;

					BYTE * po = NULL;

					BYTE * pe = wrapper_videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer );

					if( pe == NULL ) { LINUXV4L2_DEBUG( KERN_INFO,"[%02d] MZ0380_HwProcessAnalogPCIPacket() pe == NULL \n", pDevice->m_nKsDeviceNumber ); return TRUE; }

					if( pVideoBuffer->m_sKsBuffer.state != VIDEOBUF_QUEUED ) { LINUXV4L2_PRINT( KERN_INFO, "pe state error(%x)\n", pVideoBuffer->m_sKsBuffer.state ); return TRUE; }


					//if( (pVideoBuffer->m_sKsBuffer.input % 2) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ]; }  

					//if( (pVideoBuffer->m_sKsBuffer.input % 2) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ]; } 

					if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1

						(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1
						/*
						switch( j % 2 ) {

						case 0: po = pDevice->m_pDmaBaseCommonBuffer + (0x000CA900 * 2 * s) + (0x000CA900 * 0); break;

						case 1: po = pDevice->m_pDmaBaseCommonBuffer + (0x000CA900 * 2 * s) + (0x000CA900 * 1); break;
						}
						*/
						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ s ]; }  

						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ s ] + (0x000CA900 * 1); } 
						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (2) ) { po = pDevice->m_pDmaBaseCommonBuffer[ s ] + (0x000CA900 * 2); } 
						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (3) ) { po = pDevice->m_pDmaBaseCommonBuffer[ s ] + (0x000CA900 * 3); } 

					}
					else { // HD
						/*
						switch( j % 2 ) {

						case 0: po = pDevice->m_pDmaBaseCommonBuffer + (0x0034BD00 * 2 * s) + (0x0034BD00 * 0); break;

						case 1: po = pDevice->m_pDmaBaseCommonBuffer + (0x0034BD00 * 2 * s) + (0x0034BD00 * 1); break;
						}
						*/

						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (0) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 0 ]; }  

						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (1) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 1 ]; } 

						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (2) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 2 ]; } 

						if( (pVideoBuffer->m_sKsBuffer.input % 4) == (3) ) { po = pDevice->m_pDmaBaseCommonBuffer[ 3 ]; } 
					}
					UINT * p_size = (UINT *)(po);

					ULONG size = *p_size;

					MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

					if( p_sys_cfg->n_output_video_resolution_vi_fw == 7 ) {

						size = po[ 4 ] & 0x01;
					}
	
					if( size > 0 ) {
						
						//LINUXV4L2_DEBUG( KERN_INFO,"[%02d] preview size(%d) i(0d%d) ring(%d)\n", pDevice->m_nKsDeviceNumber, size, i, pVideoBuffer->m_sKsBuffer.input );

						ULONG  cxo = p_sys_cfg->n_input_video_resolution_cx;

						ULONG  cwo = ((cxo + 15) / 16) * 16;

						ULONG  cyo = p_sys_cfg->n_input_video_resolution_cy;

						if( p_sys_cfg->n_input_video_resolution_interleaved ) {

							cyo <<= 1;
						}

						ULONG  cxe = pVideoBuffer->m_sKsBuffer.width;

						ULONG  cwe = pVideoBuffer->m_sKsBuffer.width;

						ULONG  cye = pVideoBuffer->m_sKsBuffer.height;

						if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1
				
							(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

							if( (pDevice->iManufacturer & 0x0F) == 0x0C ) { // 960H -> D1

								cxo = ((cxo /  2) / 16) * 16;//360

								cwo = ((cxo + 15) / 16) * 16;//368

								cyo >>= 1;//240
							}
						}

						if( pe && po && (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED) ) {

							BYTE * po_y = po + (4);//??

							BYTE * po_u = po_y + (cwo * cyo);

							BYTE * po_v = po_u + ((cwo * cyo) >> 2);

							BYTE * pe_y = pe;

							BYTE * pe_u = pe + (cwe * cye);

							BYTE * pe_v = pe_u + ((cwe * cye) >> 2);

							memcpy( pe_y, po_y, cxe * cye );

							memcpy(pe_u, po_v, cxe * cye / 4);

							memcpy(pe_v, po_u, cxe * cye / 4);

						}
						else
						{
							LINUXV4L2_DEBUG( KERN_INFO,"[%02d] pe(0x%x) po(0x%x)\n", pDevice->m_nKsDeviceNumber, pe, po );
						}

						pVideoBuffer->m_sKsBuffer.input = 0xFFFFFFFF; //
					}//size
					else
					{
						
						//LINUXV4L2_DEBUG( KERN_INFO,"[%02d] preview size(%d) i(0d%d)---------------------\n", pDevice->m_nKsDeviceNumber, size, i );

						p_is_drops[ i ] = TRUE;
					}
				}//bufer
				else
				{
					//LINUXV4L2_DEBUG( KERN_INFO,"[%02d] i(0d%d) pVideos[ i ](0x%x)  pVideoBuffers[ i ](0x%x) \n", pDevice->m_nKsDeviceNumber, i, pVideos[ i ], pVideoBuffers[ i ] );
				}
			}//ready
		}//end of for
		
		
		for( i = 0 ; i < counts ; i++ ) {

			BYTE * po = NULL;

			CVideoBuffer * pVideoBuffer = pVideoBuffers[ i + 16 ];

			BOOL  is_ready = 0;

			ULONG s = 0;

			if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1
				
				(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

				is_ready = (status & (1 << (i + 0))) ? TRUE : FALSE;

				s = i;
			}
			else {

				is_ready = (status & (1 << (0 + 0))) ? TRUE : FALSE;

				s = 0;
			}
			if( is_ready ) {

				ULONG j = 0;

				if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1 (RING.BUFFER)

					(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1 (RING.BUFFER)

					j = (R00000044 >> (i * 4)) & 0x03;
				}
				else { // HD (RING.BUFFER)

					j = (R00000044 >> (0 * 4)) & 0x03;
				}

				if( pVideos[ 16 + i ] && pVideoBuffers[ 16 + i ] ) {

					if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1

						(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

						switch( j % 4 ) {
						
						case 0: po = pDevice->m_pDmaBaseCommonBuffer[ s + 16 ]; break;
						
						case 1: po = pDevice->m_pDmaBaseCommonBuffer[ s + 16 ] + (0x000CA900 * 1); break;
						case 2: po = pDevice->m_pDmaBaseCommonBuffer[ s + 16 ] + (0x000CA900 * 2); break;
						case 3: po = pDevice->m_pDmaBaseCommonBuffer[ s + 16 ] + (0x000CA900 * 3); break;
						}
					}
					else { // HD

						switch( j % 4 ) {
						
						case 0: po = pDevice->m_pDmaBaseCommonBuffer[ 5 ]; break;
						case 1: po = pDevice->m_pDmaBaseCommonBuffer[ 6 ]; break;
						case 2: po = pDevice->m_pDmaBaseCommonBuffer[ 7 ]; break;
						case 3: po = pDevice->m_pDmaBaseCommonBuffer[ 8 ]; break;
						}
					}

					BYTE * pe = wrapper_videobuf_to_vmalloc( &pVideoBuffer->m_sKsBuffer );

					if( pe == NULL ) { 	
						
						LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_HwProcessAnalogPCIVideoPacket() no dest buffer\n", pDevice->m_nKsDeviceNumber ); 

						ULONG R50 = MZ0380_GetRegister_EX( pDevice, 0, 0x00000050 + i, 1 );

						if( R50 == 0x01 ) {

							MZ0380_SetRegister_EX( pDevice, 0, 0x00000050 + i, 0x02, 1 ); // 告知韌體上面很忙, 不要傻傻等待
						}
						
						return STATUS_UNSUCCESSFUL; 
					}

					if( pVideoBuffer->m_sKsBuffer.state != VIDEOBUF_QUEUED ) { LINUXV4L2_PRINT( KERN_INFO, "pe state error(%x)\n", pVideoBuffer->m_sKsBuffer.state ); return STATUS_UNSUCCESSFUL; }

					//there are 8 bytes headers in H264 stream form MZ0380
					//byte 0~3 is size, byte 4 is I/P

					UINT * p_size = (UINT *)(po);

					ULONG size = *(p_size + 0);

					ULONG info = *(p_size + 1); //??

					if( size > pVideoBuffer->m_sKsBuffer.bsize)
					{
						size = pVideoBuffer->m_sKsBuffer.bsize - 4;
					}

					if(size > 0)
					{
						//LINUXV4L2_DEBUG( KERN_INFO,"[%02d] H264 size(%d) i(0d%d) ring(%d)\n", pDevice->m_nKsDeviceNumber, size, i, j );

						{  *p_size = 0; // 避免重複抓取

							MZ0380_SetRegister_EX( pDevice, 0, 0x00000050 + i, 0x00, 1 ); // 可以進行下一張
						}

						#ifdef FFMPEG //only for SC5C0, not for SC3C0
										
							if( pVideos[ 16 + i ]->n_main_H264_left > 0)
							{
								if(pDevice->m_pDmaBaseCommonBuffer[13 ])
								{
									if(pDevice->m_nAnalogCopyProtMacrovisionProperty == 0 )
									{
										if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffers[ 16 + i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
										{
											memcpy( pe, pDevice->m_pDmaBaseCommonBuffer[13 ], pVideos[ 16 + i ]->n_main_H264_left );
										}
									}
								}
								pVideos[ 16 + i ]->n_main_H264_count = pVideos[ 16 + i ]->n_main_H264_left;

								pVideos[ 16 + i ]->n_main_H264_left = 0;
							
							}
						
							if( (pVideos[ 16 + i ]->n_main_H264_count + size) < pVideoBuffers[ 16 + i ]->m_sKsBuffer.bsize )
							{
								if(pDevice->m_nAnalogCopyProtMacrovisionProperty == 0 )
								{
									if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffers[ 16 + i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
									{
										memcpy( pe + pVideos[ 16 + i ]->n_main_H264_count, po, size );
									}
								}
							
								pVideos[ 16 + i ]->n_main_H264_count += size;

							}
							else if( (pVideos[ 16 + i ]->n_main_H264_count + size) == pVideoBuffers[ 16 + i ]->m_sKsBuffer.bsize )
							{
								if(pDevice->m_nAnalogCopyProtMacrovisionProperty == 0 )
								{
									if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffers[ 16 + i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
									{
										memcpy( pe + pVideos[ 16 + i ]->n_main_H264_count, po, size );
									}
								}
								pVideos[ 16 + i ]->n_main_H264_count = 0;

								p_is_completes[ 16 + i ] = TRUE;

								p_is_drops[ 16 + i ] = FALSE;
							}
							else
							{
							
								pVideos[ 16 + i ]->n_main_H264_left = size - ( pVideoBuffers[ 16 + i ]->m_sKsBuffer.bsize - pVideos[ 16 + i ]->n_main_H264_count );

								if(pDevice->m_nAnalogCopyProtMacrovisionProperty == 0 )
								{
									if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffers[ 16 + i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
									{
										memcpy( pe + pVideos[ 16 + i ]->n_main_H264_count, po, size - pVideos[ 16 + i ]->n_main_H264_left );
									}
								}
								//copy residue to temp buffer
								if(pDevice->m_pDmaBaseCommonBuffer[13 ])
								{
									if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffers[ 16 + i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
									{
										memcpy(pDevice->m_pDmaBaseCommonBuffer[ 13 ], po + size - pVideos[ 16 + i ]->n_main_H264_left, pVideos[ 16 + i ]->n_main_H264_left );
									}
								}

								p_is_completes[ 16 + i ] = TRUE;

								p_is_drops[ 16 + i ] = FALSE;

							}


						#else
							H264_STREAM_HEADER hdr;
											
							memset( &hdr, 0x00, sizeof(H264_STREAM_HEADER) );

							if(pDevice->m_nAnalogCopyProtMacrovisionProperty == 0 )
							{
								hdr.nFrameBufferSize = size;
							}
							else
							{
								hdr.nFrameBufferSize = 0;
							}

							hdr.nFrameType = (*( po + 4 ) & 0x1) ? (0) : (1);

							hdr.nReserved = 0x00;


							if(pDevice->m_nAnalogCopyProtMacrovisionProperty == 0 )
							{
								if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
								{
									memcpy( pe + 4, po + 8, size );
								}
							}

							if( (pDevice->m_nAnalogCaptureStreamPowerReference != 0) && (pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED) )
							{
								memcpy( pe, &hdr, sizeof(H264_STREAM_HEADER) );
							}

							p_is_drops[ 16 + i ] = FALSE;
						
							p_is_completes[ 16 + i ] = TRUE;

						#endif //FFMPEG
					}//size
					else
					{
						//LINUXV4L2_PRINT( KERN_INFO, "H264 size 0, i (0d%d)!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", i );

						if( (BYTE)(MZ0380_GetRegister_EX( pDevice, 0, 0x00000050 + i, 1 )) != 0 ) {

							MZ0380_SetRegister_EX( pDevice, 0, 0x00000050 + i, 0x00, 1 ); // 容錯, 可以進行下一張
							
						}

						p_is_drops[ 16 + i ] = TRUE;
					}

				}
				else { // 沒有開編碼

					MZ0380_SetRegister_EX( pDevice, 0, 0x00000050 + i, 0x00, 1 ); // 可以進行下一張
				}
			}
		}
	}
	

	if( status & 0x00000800 ) {

		//KeReleaseSemaphore( &(pPCIBaseDevice->m_hIRQAccessSemaphore), 0, 1, FALSE );

		wrapper_down( &(pDevice->m_MZ0380AccessSemaphore) ); 

		pDevice->m_MZ0380_ready = 1;

		wrapper_up( &(pDevice->m_MZ0380AccessSemaphore) ); 

		wrapper_wake_up( &(pDevice->m_wait_queue_head) ); 
	}

	return STATUS_SUCCESS;
}


NTSTATUS MZ0380_HwProcessAnalogRawVideoFrame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer )
{
	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_HwProcessAnalogRawH264Frame( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffers )
{
	return STATUS_SUCCESS;
}

NTSTATUS MZ0380_HwProcessAnalogRawAudioFrame( CDevice * pDevice, ULONG status, ULONG mask, CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ], CAudioBuffer * pAudioBuffers[ MAX_SUB_DEVICE_NUM_X_2 ], BOOLEAN * p_is_completes, BOOLEAN * p_is_drops )
{

	CAudio * pAudio = NULL;

	BYTE * po  = NULL;

	BYTE * pe  = NULL;

	if( status & 0x00FF0000 ) { // AUDIO STREAMS

		ULONG R0000004C = MZ0380_GetRegister( pDevice, 0, 0x0000004C );

		ULONG counts = 1;

		if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1

			(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

			if( (pDevice->iManufacturer & 0x0F) == 0x01 ||
				
				(pDevice->iManufacturer & 0x0F) == 0x0B ||
				
				(pDevice->iManufacturer & 0x0F) == 0x0C ) {

				counts = 4;
			}
			else {

				counts = 8;
			}
		}
		else { // HD
			/*
			if( pDevice->m_nCustomCloneFiltersProperty >= 2 ) {

				counts = (pDevice->m_nCustomCloneFiltersProperty >= 4) ? (4) : (pDevice->m_nCustomCloneFiltersProperty); // 最多四組
			}
			else {

				counts = 1;
			}
			*/
			counts = 1;

		}
		ULONG i = 0;
		for( i = 0 ; i < counts ; i++ ) {

			BOOL  is_ready = FALSE;

			ULONG m = i >> 1;

			ULONG s = 0;

			if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1
				
				(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

				is_ready = (status & (1 << ((m * 2) + 16))) ? TRUE : FALSE;

				s = i;
			}
			else {

				is_ready = (status & (1 << ((0 * 2) + 16))) ? TRUE : FALSE;

				s = 0;
			}
			if( is_ready ) {

				ULONG j = 0;

				if( (pDevice->iProduct & 0x0F) == 0x02  || // 960H & D1 (RING.BUFFER)
					
					(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1 (RING.BUFFER)

					j = (R0000004C >> ((m * 2) * 4)) & 0x03;
				}
				else { // HD (RING.BUFFER)

					j = (R0000004C >> ((0 * 2) * 4)) & 0x03;
				}

				pAudio = pAudios[ i ];

				if( pAudio == NULL )
				{
					continue;
				}
				
				if(pAudio->m_pKsSubStream)
				{
					if(pAudio->m_pKsSubStream->runtime)
					{
						if(pAudio->m_pKsSubStream->runtime->dma_area)
						{
							pe = pAudio->m_pKsSubStream->runtime->dma_area;				
	
							pe += pAudio->m_nFrameNumber * pAudio->m_nFrameSize; 

						}				
						else
						{
							continue;
						}
					}		
					else
					{
						continue;
					}
				}
				else
				{
					continue;
				}

				if( ((pDevice->iProduct & 0x0F) == 0x02) || ((pDevice->iProduct & 0x0F) == 0x03)  ) { // 960H & D1
					
					switch( j % 4 ) {

					case 0: po = pDevice->m_pDmaBaseCommonBuffer[ 8 ] + 8192 * ((m << 2) + 0); break;
							
					case 1: po = pDevice->m_pDmaBaseCommonBuffer[ 8 ] + 8192 * ((m << 2) + 1); break;
							
					case 2: po = pDevice->m_pDmaBaseCommonBuffer[ 8 ] + 8192 * ((m << 2) + 2); break;
							
					case 3: po = pDevice->m_pDmaBaseCommonBuffer[ 8 ] + 8192 * ((m << 2) + 3); break;

					}
					
					SHORT * po_s = (SHORT *)(po);
					
					SHORT * pe_s = (SHORT *)(pe);

					if( s & 0x01 ) { po_s++; }

					ULONG j = 0;
					for( j = 0 ; j < 2048 ; j++ ) {

					   *pe_s = *po_s++;

						po_s++;

						pe_s++;
					}
					
					p_is_completes[ i ] = TRUE;

					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] audio complete(0x%x)\n", pDevice->m_nKsDeviceNumber, i);

				}
				else {

					switch( j % 4 ) {

					case 0: po = (BYTE*) pDevice->m_pDmaBaseCommonBuffer[ 4 ] + (4096 * 0); break;
					case 1: po = (BYTE*) pDevice->m_pDmaBaseCommonBuffer[ 4 ] + (4096 * 1); break;
					case 2: po = (BYTE*) pDevice->m_pDmaBaseCommonBuffer[ 4 ] + (4096 * 2); break;
					case 3: po = (BYTE*) pDevice->m_pDmaBaseCommonBuffer[ 4 ] + (4096 * 3); break;
					
					}

					memcpy(pe, po, 4096);

					p_is_completes[ 0 ] = TRUE;

				}

			}// end of if ready
		}//end of for
	}
EXIT:

	return STATUS_SUCCESS;
}

static int MZ0380_StartControlPanelAnalysisThread( CDevice * pDevice )
{
	if( pDevice->m_pControlThread == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_StartControlPanelAnalysisThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

		pDevice->m_pControlThread = wrapper_kthread_run( MZ0380_OnControlPanelAnalysisThread, pDevice, "MZ0380_OnControlPanelAnalysisThread" );

		if( wrapper_IS_ERR( pDevice->m_pControlThread ) ) {

			int ret = wrapper_PTR_ERR( pDevice->m_pControlThread );

			pDevice->m_pControlThread = NULL;

			return ret;
		}
	}

	return STATUS_SUCCESS;
}

static int MZ0380_StopControlPanelAnalysisThread( CDevice * pDevice )
{
	int thread_stop_counter = 0;

	if( pDevice->m_pControlThread ) {

		if( pDevice->m_ReadyToStopControlThread == 0 )
		{
			pDevice->m_ReadyToStopControlThread = 1;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_StopControlPanelAnalysisThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );
		
			wrapper_kthread_stop( pDevice->m_pControlThread ); // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "STOP" SYNC.

			while( pDevice->m_pControlThread_already_stop == FALSE )
			{
				wrapper_schedule_timeout_interruptible( wrapper_msecs_to_jiffies(200) );

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_StopControlPanelAnalysisThread( ) ---\n", (int)(pDevice->m_nKsDeviceNumber) );

				thread_stop_counter++;

				if(thread_stop_counter > 5)
				{
					break;
				}
			}

			pDevice->m_pControlThread = NULL;

			pDevice->m_ReadyToStopControlThread = 0;
		
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_StopControlPanelAnalysisThread( ) end\n", (int)(pDevice->m_nKsDeviceNumber) );
		}
	}

	return STATUS_SUCCESS;
}

static int MZ0380_OnControlPanelAnalysisThread( void * p_context )
{	
	CDevice * pDevice = (CDevice *)p_context;

	struct timeval ts;

	ULONG counts = 0;

	BYTE i = 0;

	pDevice->m_pControlThread_already_stop = FALSE;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_OnControlPanelAnalysisThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );


	while( wrapper_kthread_should_stop() == FALSE ) {

		MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

		if( (p_sys_cfg->n_is_firmware_ready & 0x000000FF) == 0x000000F1 ) {

			if( (counts % 10) == 0 ) { 

				if( (pDevice->iProduct & 0x0F) == 0x02 ||  // 960H & D1
					
					(pDevice->iProduct & 0x0F) == 0x03 ) { // 960H & D1

					ULONG chs = 8;

					if( (pDevice->iManufacturer & 0x0F) == 0x01 ||
						
						(pDevice->iManufacturer & 0x0F) == 0x0B ||
						
						(pDevice->iManufacturer & 0x0F) == 0x0C ) {
	
						chs = 4;
					}
					else {
	
						chs = 8;
					}
					for( i = 0 ; i < chs ; i++ ) {

						BYTE R00 = 0x00;

						if( i < 4 ) {

							MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x00 );

							R00 = MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x00 + 0x10 * (i - 0) );
						}
						else {

							MZ0380_SetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x40, 0x01 );

							R00 = MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x50, 0x00 + 0x10 * (i - 4) );
						}
						if( (R00 & 0x88) == 0x08 ) {

							pDevice->m_nAnalogVideoDecoderStatusProperty |=  (1 << i);
						}
						else {

							pDevice->m_nAnalogVideoDecoderStatusProperty &= ~(1 << i);
						}
					}
					//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] m_nAnalogVideoDecoderStatusProperty( 0x%x )\n", (int)(pDevice->m_nKsDeviceNumber), pDevice->m_nAnalogVideoDecoderStatusProperty );
				}
				else if( pDevice->iManufacturer == 0xC4 || // SC510N1 MC SDI
				
						 pDevice->iManufacturer == 0xCB || // SC510N4 SDI
	
						 pDevice->m_nAnalogCrossbarVideoInputProperty == 4 ) {  // HD

					// VIDEO FORAMT DETECTION
					// 
					USHORT R00006 = 0x0000;

					USHORT R0001F = 0x0000;

					USHORT R00022 = 0x0000;

					R00006 = MZ0380_GetSpiRegister( pDevice, 0x0006 ) & 0x3F00; R00006 >>= 8; // VD_STD

					R0001F = MZ0380_GetSpiRegister( pDevice, 0x001F ) & 0x3FFF; // WORDS PER ACTIVE LINE

					R00022 = MZ0380_GetSpiRegister( pDevice, 0x0022 ) & 0x2000; // M

					ULONG x = 0;

					ULONG y = 0;

					ULONG fps = 0;

					ULONG m = (R00022 & 0x2000) ? 1 : 0; // EX: 29.97 / 30.00

					if( R00006 == 0x16 || R00006 == 0x17 ) { x =  720; y =  240; fps = 60; }

					if( R00006 == 0x19 || R00006 == 0x1B ) { x =  720; y =  240; fps = 60; }

					if( R00006 == 0x18 || R00006 == 0x1A ) { x =  720; y =  288; fps = 50; }

					if( R00006 == 0x1E ) { 
						
						USHORT R00021 = MZ0380_GetSpiRegister( pDevice, 0x0021 )  & 0x07FF; // LINES PER FRAME 

						if( R00021 == 525 ) {

							x = 720; y = 288; fps = 50; 
						}
						else {

							x = 720; y = 240; fps = 60; 
						}
					}
					if( R00006 == 0x20 || R00006 == 0x00 ) { x = 1280; y =  720; fps = 60; }

					if( R00006 == 0x24 || R00006 == 0x04 ) { x = 1280; y =  720; fps = 50; }

					if( R00006 == 0x22 || R00006 == 0x02 ) { x = 1280; y =  720; fps = 30; }

					if( R00006 == 0x26 || R00006 == 0x06 ) { x = 1280; y =  720; fps = 25; }

					if( R00006 == 0x28 || R00006 == 0x08 ) { x = 1280; y =  720; fps = 24; }

					if( R00006 == 0x2A || R00006 == 0x0A ) { x = 1920; y =  540; fps = 60; }

					if( R00006 == 0x2C || R00006 == 0x0C ) { x = 1920; y =  540; fps = 50; }

					if( R00006 == 0x11                   ) { x = 1920; y =  540; fps = 48; }

					if( R00006 == 0x2B || R00006 == 0x0B ) { x = 1920; y = 1080; fps = 30; }

					if( R00006 == 0x2D || R00006 == 0x0D ) { x = 1920; y = 1080; fps = 25; }

					if( R00006 == 0x30 || R00006 == 0x10 ) { x = 1920; y = 1080; fps = 24; }

					if( R00006 == 0x2B && R0001F == 1920 ) { x = 1920; y = 1080; fps = 60; }

					if( R00006 == 0x2D && R0001F == 1920 ) { x = 1920; y = 1080; fps = 50; }

					if( x != 0 && y != 0 ) {

						if( p_sys_cfg->n_input_video_resolution_cx != x ||

							p_sys_cfg->n_input_video_resolution_cy != y ||

							p_sys_cfg->n_input_video_resolution_fps != fps ||
	
							p_sys_cfg->n_input_video_resolution_fps_m != m ) {

							p_sys_cfg->n_input_video_resolution_cx = x;

							p_sys_cfg->n_input_video_resolution_cy = y;

							p_sys_cfg->n_input_video_resolution_fps = fps;

							p_sys_cfg->n_input_video_resolution_fps_m = m;

							if( p_sys_cfg->n_input_video_resolution_cy == 240 ||

								p_sys_cfg->n_input_video_resolution_cy == 288 ||

								p_sys_cfg->n_input_video_resolution_cy == 540 ) {

								p_sys_cfg->n_input_video_resolution_interleaved = 1;
							}
							else {

								p_sys_cfg->n_input_video_resolution_interleaved = 0;
							}
							p_sys_cfg->b_input_video_signal_changed = TRUE;

							p_sys_cfg->n_input_audio_sampling_frequency = 48000;
						}

						LINUXV4L2_PRINT( KERN_INFO, "[%02d] GV7601_SDI_MODE_DETECT( %d x %d x %d )\n", pDevice->m_nKsDeviceNumber, (ULONG)(x), (ULONG)(y), fps);

						if( p_sys_cfg->n_input_video_resolution_fps > 50 ) {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_NTSC_M;
						}
						else if( p_sys_cfg->n_input_video_resolution_fps > 30 ) {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_PAL_B;
						}
						else if( p_sys_cfg->n_input_video_resolution_fps > 25 ) {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_NTSC_M;
						}
						else {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_PAL_B;
						}
						pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) |

																		  (p_sys_cfg->n_input_video_resolution_cy <<  0);

						pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);

						pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);
						
						pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 48000;

						pDevice->m_nAnalogVideoDecoderStatusProperty = 1;

						pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
					}
					else {

						p_sys_cfg->n_input_video_resolution_cx = 0;
	
						p_sys_cfg->n_input_video_resolution_cy = 0;
	
						p_sys_cfg->n_input_video_resolution_fps = 0;
	
						p_sys_cfg->n_input_video_resolution_fps_m = 0;

						pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

						pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

						pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

						pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

						pDevice->m_nAnalogVideoDecoderStatusProperty = 0;
						
						pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
					}
				}
				else {	// HD

					if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 || 
						
						pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) {
							
						if( pDevice->m_nCustomVideoStandardProperty & SUPPORTED_ANALOG_VIDEO_STANDARDS_60HZ ) {

							pDevice->m_nAnalogVideoDecoderStatusProperty = ( ((BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x01 )) & 0xC9) == 0x48 ) ? 1 : 0; DELAY_100NS( 1 );
						}
						if( pDevice->m_nCustomVideoStandardProperty & SUPPORTED_ANALOG_VIDEO_STANDARDS_50HZ ) {

							pDevice->m_nAnalogVideoDecoderStatusProperty = ( ((BYTE)(MZ0380_GetAnalogVideoDecoderRegister( pDevice, 0, 0x88, 0x01 )) & 0xC9) == 0x49 ) ? 1 : 0; DELAY_100NS( 1 );
						}
						pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;
					}
					else {

						MST3367_VideoFormatDetection( pDevice );
					}
				}
			}
		}
		else
		{
			
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] n_is_firmware_ready(0x%x) \n", (int)(pDevice->m_nKsDeviceNumber), p_sys_cfg->n_is_firmware_ready );
		}

		if( pDevice->m_nAnalogCaptureStreamPowerReference > 0 ) {

			if( pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ] == 1 )
			{

				if( (pDevice->iProduct & 0x0F) == 0x05 ) { // HD PRODUCTS

					if( p_sys_cfg->b_input_video_signal_changed == TRUE ) {
		
						p_sys_cfg->b_input_video_signal_changed = FALSE;
		
						MZ0380_StopFirmware( pDevice );
		
						MZ0380_StartFirmware( pDevice );
					}
				}
				MZ0380_UpdateFirmware( pDevice, TRUE );
			}
		}
		if( (counts % 1000) == 0 ) { 

			LINUXV4L2_PRINT( KERN_INFO, "CH%02d, BOARD VERSION: %d.%d, FIRMWARE VERSION: %d.%d\n", pDevice->m_nKsDeviceNumber, p_sys_cfg->n_board_major_version, p_sys_cfg->n_board_minor_version, p_sys_cfg->n_firmware_major_version, p_sys_cfg->n_firmware_minor_version);
		}
		counts++;
	
		for( i = 0 ; i < 10 ; i++ ) {

			if( wrapper_kthread_should_stop() ) {

				break;
			}
			DELAY_100NS( 100000 );
		}

	}
	//end of while
	pDevice->m_pControlThread_already_stop = TRUE;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MZ0380_OnControlPanelAnalysisThread( exit ) - %08X\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(pDevice->m_pControlThread) );

	return 0;	

}
