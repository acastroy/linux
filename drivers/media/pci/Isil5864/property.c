#include "LINUXV4L2.h"
#include "property.h"
#include "wrapper.h"
#include "tw6805.h"
#include "cx2581.h"
#include "cx2588.h"
#include "sa7160.h"
#include "sl6010.h"
#include "ah8400.h"
#include "tw5864.h"
#include "fh8735.h"
#include "mz0380.h"

static void common_pci_driver_on_tasklet( unsigned long p_context )
{
	CDevice * pDevice = (CDevice *)(p_context);

	ULONG interrupt_status = pDevice->m_nTaskletExtraParameterA;

	ULONG interrupt_params = pDevice->m_nTaskletExtraParameterB;

	ULONG i = 0;

	unsigned long flags = 0x00000000;

//	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_tasklet( S:%08X, M:%08X )\n", (int)(pDevice->m_nKsDeviceNumber), (unsigned int)(interrupt_status), (unsigned int)(interrupt_params) );

	wrapper_spin_lock_irqsave( &pDevice->m_sTaskletAccessSpinLock, flags );

	//
	//
	{	CVideo *       pVideo[ MAX_SUB_DEVICE_NUM_X_2 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

		CVideoBuffer * pLeadVideoBuffer[ MAX_SUB_DEVICE_NUM_X_2 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

		CVideoBuffer * pWorkVideoBuffer[ MAX_SUB_DEVICE_NUM_X_2 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

		BOOLEAN        is_complete[ MAX_SUB_DEVICE_NUM_X_2 ] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

		BOOLEAN        is_drop[ MAX_SUB_DEVICE_NUM_X_2 ] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

//		BOOLEAN        is_drop[ MAX_SUB_DEVICE_NUM_X_2 ] = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE };

		BOOLEAN        is_use_double_lists = FALSE;

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			if( pDevice->m_eVideoStreamState[ i ] == 0x00000000 ) { continue ; } //

			if( pDevice->m_pVideo[ i ] == NULL ) { continue ; }

			pVideo[ i ] = pDevice->m_pVideo[ i ];

			//
			// 
			if( wrapper_list_empty( &pVideo[ i ]->m_sKsBufferList ) == 0 ) { //not empty
			
				unsigned long flags = 0x00000000;

				wrapper_spin_lock_irqsave( &pVideo[ i ]->m_sKsBufferListSpinLock, flags );

				pLeadVideoBuffer[ i ] = wrapper_list_first_entry( &pVideo[ i ]->m_sKsBufferList, CVideoBuffer, m_sKsBuffer.queue ); //
				
				pWorkVideoBuffer[ i ] = wrapper_list_first_entry( &pVideo[ i ]->m_sKsBufferList, CVideoBuffer, m_sKsBuffer.queue ); //

				wrapper_spin_unlock_irqrestore( &pVideo[ i ]->m_sKsBufferListSpinLock, flags );

				if( pLeadVideoBuffer[ i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED )
				{
					NULL;
				}
				else
				{
					pLeadVideoBuffer[ i ] = NULL;

					pWorkVideoBuffer[ i ] = NULL;

					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pLeadVideoBuffer[ 0d%d ] error, not VIDEOBUF_QUEUED\n", (int)(pDevice->m_nKsDeviceNumber), i );

				}
				//	waitqueue_active( &pLeadVideoBuffer->m_sKsBuffer.done ); // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "RELEASE" SYNC.

				; //
			}
			else {

				pLeadVideoBuffer[ i ] = NULL;

				pWorkVideoBuffer[ i ] = NULL;

				//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] list empty, i(0x%x)\n", (int)(pDevice->m_nKsDeviceNumber), i );
			}
		}
		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			if( pDevice->m_eVideoEncoderStreamState[ i ] == 0x00000000 ) { continue ; } //

			if( pDevice->m_pVideoEncoder[ i ] == NULL ) { continue ; }

			pVideo[ i + 16 ] = pDevice->m_pVideoEncoder[ i ];

			//
			// 
			if( wrapper_list_empty( &pVideo[ i + 16 ]->m_sKsBufferList ) == 0 ) {
			
				unsigned long flags = 0x00000000;

				wrapper_spin_lock_irqsave( &pVideo[ i + 16 ]->m_sKsBufferListSpinLock, flags );

				pLeadVideoBuffer[ i + 16 ] = wrapper_list_first_entry( &pVideo[ i + 16 ]->m_sKsBufferList, CVideoBuffer, m_sKsBuffer.queue ); //
				
				pWorkVideoBuffer[ i + 16 ] = wrapper_list_first_entry( &pVideo[ i + 16 ]->m_sKsBufferList, CVideoBuffer, m_sKsBuffer.queue ); //

				wrapper_spin_unlock_irqrestore( &pVideo[ i + 16 ]->m_sKsBufferListSpinLock, flags );

				if( pLeadVideoBuffer[ i + 16 ]->m_sKsBuffer.state == VIDEOBUF_QUEUED )
				{
					NULL;
				}
				else
				{
					pLeadVideoBuffer[ i + 16 ] = NULL;

					pWorkVideoBuffer[ i + 16 ] = NULL;

					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pLeadVideoBuffer[ 0d%d ] error, not VIDEOBUF_QUEUED --\n", (int)(pDevice->m_nKsDeviceNumber), i + 16 );

				}

				
				//	waitqueue_active( &pLeadVideoBuffer->m_sKsBuffer.done ); // GOOD NEWS!! LINUX HAD ALREADY HELPED TO FINISH THE "RELEASE" SYNC.

				; //

//				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pWorkVideoBuffer[ 0d%d ](0x%x)\n", (int)(pDevice->m_nKsDeviceNumber), i + 16, pWorkVideoBuffer[ i + 16 ] );

			}
			else {

				pLeadVideoBuffer[ i + 16 ] = NULL;

				pWorkVideoBuffer[ i + 16 ] = NULL;
				
				//LINUXV4L2_DEBUG( KERN_INFO, "[%02d] error pWorkVideoBuffer[ 0d%d ](0x%x)\n", (int)(pDevice->m_nKsDeviceNumber), i + 16, pWorkVideoBuffer[ i + 16 ] );
			}


		}
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {
			
			TW6805_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); 
			
			#ifdef TW6805_64bit		
				is_use_double_lists = FALSE;
			#else
				is_use_double_lists = TRUE;
			#endif
		}
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) {
			
			CX2581_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); 
			
			#ifdef CX2581_64bit		
				is_use_double_lists = FALSE;
			#else
				is_use_double_lists = TRUE;
			#endif
		}
		//shawn 2010.07.12
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) {
			
			CX2588_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = TRUE;
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) {
			
			SA7160_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = FALSE;
		}
		//
		if( (pDevice->m_pKsDevice->vendor == 0x9413 && pDevice->m_pKsDevice->device == 0x6010) ) {

		//	SL6010_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = FALSE;
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) {

			AH8400_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = FALSE;
		}

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) {

			TW5864_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = FALSE;
		}

		if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) {

			FH8735_HwProcessAnalogPCIVideoPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = FALSE;
		}

		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) {

			MZ0380_HwProcessAnalogPCIPacket( pDevice, interrupt_status, interrupt_params, pVideo, pWorkVideoBuffer, is_complete, is_drop ); is_use_double_lists = FALSE;
		}

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM_X_2 ; i++ ) {

			if( pVideo[ i ] == NULL ) { continue ; }

			if( is_complete[ i ] == FALSE ) { continue ; }

			if( is_use_double_lists ) { //

				int is_empty = wrapper_list_empty( &pVideo[ i ]->m_sKsBufferListList ); 

				if( pLeadVideoBuffer[ i ] ) { //

					if( is_drop[ i ] == FALSE ) {
						
						unsigned long flags = 0x00000000;

						wrapper_spin_lock_irqsave( &pVideo[ i ]->m_sKsBufferListSpinLock, flags );
					
						if( pLeadVideoBuffer[ i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED )
						{
							wrapper_list_del( &pLeadVideoBuffer[ i ]->m_sKsBuffer.queue );
						}
						else
						{
							LINUXV4L2_PRINT( KERN_INFO, "[%02d] pe state error, i(0d%d) (%x)\n", pDevice->m_nKsDeviceNumber, i, pLeadVideoBuffer[ i ]->m_sKsBuffer.state );
						}

						wrapper_list_add_tail( &pLeadVideoBuffer[ i ]->m_sKsBuffer.queue, &pVideo[ i ]->m_sKsBufferListList );
						
						wrapper_spin_unlock_irqrestore( &pVideo[ i ]->m_sKsBufferListSpinLock, flags );
					}
				}
				if( is_empty == 0 ) { //

					struct timeval ts;

					CVideoBuffer * pVideoBuffer = wrapper_list_first_entry( &pVideo[ i ]->m_sKsBufferListList, CVideoBuffer, m_sKsBuffer.queue ); //


					if( pVideoBuffer->m_sKsBuffer.state == VIDEOBUF_QUEUED )
					{
						wrapper_list_del( &pVideoBuffer->m_sKsBuffer.queue );
					}
					else
					{
						LINUXV4L2_PRINT( KERN_INFO, "[%02d] pe state error, i(0d%d) (%x)\n", pDevice->m_nKsDeviceNumber, i, pVideoBuffer->m_sKsBuffer.state );
					}

					wrapper_do_gettimeofday( &ts );

					pVideoBuffer->m_sKsBuffer.ts = ts;

					pVideoBuffer->m_sKsBuffer.field_count = pVideo[ i ]->m_nFrameNumber++;

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

					pVideoBuffer->m_sKsBuffer.state = VIDEOBUF_DONE;

					#else

					pVideoBuffer->m_sKsBuffer.state = STATE_DONE;

					#endif

					wrapper_wake_up( &pVideoBuffer->m_sKsBuffer.done );
				}
			}
			else { //

				if( pLeadVideoBuffer[ i ] ) {

					if( is_drop[ i ] == FALSE ) {

						struct timeval ts;
		
						wrapper_spin_lock_irqsave( &pVideo[ i ]->m_sKsBufferListSpinLock, flags );
						
						//use killall -9 vlc will change buffer state long before any driver function
						//I do not know how this be done, I can only check here
						
						if( pLeadVideoBuffer[ i ]->m_sKsBuffer.state == VIDEOBUF_QUEUED )
						{
							wrapper_list_del( &pLeadVideoBuffer[ i ]->m_sKsBuffer.queue );
						}
						else
						{
							LINUXV4L2_PRINT( KERN_INFO, "[%02d] pe state error, i(0d%d) (%x)\n", pDevice->m_nKsDeviceNumber, i, pLeadVideoBuffer[ i ]->m_sKsBuffer.state );
						}

						wrapper_spin_unlock_irqrestore( &pVideo[ i ]->m_sKsBufferListSpinLock, flags );
		
						if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) {

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000001 ) { AH8400_HwProcessAnalogPCIVideoFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000002 ) { AH8400_HwProcessAnalogPCIVideoEncoderFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }
						}

						if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) {

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000001 ) { TW5864_HwProcessAnalogPCIVideoFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }

							if( i <= 23 )//neglect audio node 
							{
								if( pVideo[ i ]->m_nSubDeviceType == 0x00000002 ) { TW5864_HwProcessAnalogPCIVideoEncoderFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }
							}
						}

						if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) {

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000001 ) { FH8735_HwProcessAnalogPCIVideoFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000002 ) { FH8735_HwProcessAnalogPCIVideoEncoderFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }
						}

						if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) {

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000001 ) { MZ0380_HwProcessAnalogRawVideoFrame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }

							if( pVideo[ i ]->m_nSubDeviceType == 0x00000002 ) { MZ0380_HwProcessAnalogRawH264Frame( pDevice, pVideo[ i ], pLeadVideoBuffer[ i ] ); }
						}

						wrapper_do_gettimeofday( &ts );
		
						pLeadVideoBuffer[ i ]->m_sKsBuffer.ts = ts;
		
						pLeadVideoBuffer[ i ]->m_sKsBuffer.field_count = pVideo[ i ]->m_nFrameNumber++;

						#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)

						pLeadVideoBuffer[ i ]->m_sKsBuffer.state = VIDEOBUF_DONE;

						#else

						pLeadVideoBuffer[ i ]->m_sKsBuffer.state = STATE_DONE;

						#endif

						wrapper_wake_up( &pLeadVideoBuffer[ i ]->m_sKsBuffer.done );

						
						if( pVideo[ i ] == pDevice->m_pVideo[ 0 ])
						{
							//LINUXV4L2_PRINT( KERN_INFO, "[%02d] video0\n", pDevice->m_nKsDeviceNumber );
						}
					}
					else
					{
						if( pVideo[ i ] == pDevice->m_pVideo[ 0 ])
						{
							//LINUXV4L2_PRINT( KERN_INFO, "[%02d] video0, drop ture\n", pDevice->m_nKsDeviceNumber );
						}
					}
				}
				else
				{
					
					if( pVideo[ i ] == pDevice->m_pVideo[ 0 ])
					{
						//LINUXV4L2_PRINT( KERN_INFO, "[%02d] video0, no lead buffer\n", pDevice->m_nKsDeviceNumber );
					}
				}
			}
		}
	}

	//
	//
	{	CAudio * pAudio[ MAX_SUB_DEVICE_NUM_X_2 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	
		BOOLEAN  is_complete[ MAX_SUB_DEVICE_NUM_X_2 ] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

		BOOLEAN  is_drop[ MAX_SUB_DEVICE_NUM_X_2 ] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			if( pDevice->m_eAudioStreamState[ i ] == 0x00000000 ) { continue ; } //

			if( pDevice->m_pAudio[ i ] == NULL ) { continue ; }

			pAudio[ i ] = pDevice->m_pAudio[ i ];
		}
		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			if( pDevice->m_eAudioEncoderStreamState[ i ] == 0x00000000 ) { continue ; } //

			if( pDevice->m_pAudioEncoder[ i ] == NULL ) { continue ; }

			pAudio[ i + 16 ] = pDevice->m_pAudioEncoder[ i ];
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) {

			if( TW6805_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop ) ) {

				if( is_complete[ 0 ] ) {

					if( pAudio[ 0 ] ) {

						pAudio[ 0 ]->m_nFrameNumber = (pAudio[ 0 ]->m_nFrameNumber + 1) % (pAudio[ 0 ]->m_nFrames);

						wrapper_snd_pcm_period_elapsed( pAudio[ 0 ]->m_pKsSubStream );
					}
				}
			}
		}
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) {

			CX2581_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop );
		}
		//shawn 2010.07.12
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) {

			CX2588_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop );
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) {

			if( SA7160_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop ) )
			{			

				if( is_complete[ 0 ] )
				{
					if( pAudio[ 0 ] ) {

						pAudio[ 0 ]->m_nFrameNumber = (pAudio[ 0 ]->m_nFrameNumber + 1) % (pAudio[ 0 ]->m_nFrames);

						wrapper_snd_pcm_period_elapsed( pAudio[ 0 ]->m_pKsSubStream );
					}
				}
			}

		}
		//
		if( (pDevice->m_pKsDevice->vendor == 0x9413 && pDevice->m_pKsDevice->device == 0x6010) ) {
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) {

			if( AH8400_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop ) )
			{

				for(i = 0 ; i < 4 ; i++ )
				{
					if( is_complete[ i ] ) {

						if( pAudio[ i ] ) {

							pAudio[ i ]->m_nFrameNumber = (pAudio[ i ]->m_nFrameNumber + 1) % (pAudio[ i ]->m_nFrames);
				
							wrapper_snd_pcm_period_elapsed( pAudio[ i ]->m_pKsSubStream );
						}
					}
				}

			}
		}
//peter [2011.07.15]
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) {
#ifndef VTC
			if( TW5864_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop ) ) {
	
				for(i = 0 ; i < 4 ; i++ )
				{
					if( is_complete[ i ] ) {

						if( pAudio[ i ] ) {

							pAudio[ i ]->m_nFrameNumber = (pAudio[ i ]->m_nFrameNumber + 1) % (pAudio[ i ]->m_nFrames);
				
							wrapper_snd_pcm_period_elapsed( pAudio[ i ]->m_pKsSubStream );
						}
					}
				}
			}
#endif //not VTC
		}

		if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) {

			if( FH8735_HwProcessAnalogPCIAudioPacket( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop ) ) {
	
				int num = 1;

				if( (pDevice->iProduct & 0x0F) == (0x03) ||	(pDevice->iManufacturer == 0x85) )
				{
					num = 4;
				}
				else if( (pDevice->iProduct & 0x0F) == (0x05) || (pDevice->iProduct & 0x0F) == (0x07) )
				{
					num = 1;
				}
				else
				{
					num = 4;
				}
				
				for(i = 0 ; i < num ; i++ )
				{
					if( is_complete[ i ] ) {

						if( pAudio[ i ] ) {

							pAudio[ i ]->m_nFrameNumber = (pAudio[ i ]->m_nFrameNumber + 1) % (pAudio[ i ]->m_nFrames);
				
							wrapper_snd_pcm_period_elapsed( pAudio[ i ]->m_pKsSubStream );
						}
					}
				}

			}
		}

		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) {

			if( MZ0380_HwProcessAnalogRawAudioFrame( pDevice, interrupt_status, interrupt_params, pAudio, NULL, is_complete, is_drop ) ) {

				int num = 1;

				if( ((pDevice->iProduct & 0x0F) == (0x02)) || ((pDevice->iProduct & 0x0F) == (0x03)))
				{
					if( (pDevice->iManufacturer & 0x0F) == 0x0B	|| (pDevice->iManufacturer & 0x0F) == 0x0C )
					{
						num = 4;
					}
					else
					{
						num = 8;
					}

				}
				else
				{
					num = 1;
				}
				
				for(i = 0 ; i < num ; i++ )
				{
					if( is_complete[ i ] ) {

						if( pAudio[ i ] ) {

							pAudio[ i ]->m_nFrameNumber = (pAudio[ i ]->m_nFrameNumber + 1) % (pAudio[ i ]->m_nFrames);
				
							wrapper_snd_pcm_period_elapsed( pAudio[ i ]->m_pKsSubStream );
						}
					}
				}

			}
		}
	}

	// ...
	//
	pDevice->m_hInterruptAccessLock = TRUE;

	if( pDevice->m_nAnalogCaptureStreamPowerReference > 0 ) 
	
	{

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ||
			(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) {

			writel( 0x00000022, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000020 );
		}
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) {
			
			for( i = 0 ; i < 8 ; i++ ) {
				
				writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );
			}
			for( i = 0 ; i < 4 ; i++ ) {
				
				writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );
			}
		}
		//shawn 2010.07.12
		if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) {
			
			for( i = 0 ; i < 8 ; i++ ) {
				
			//	writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );
				writel( 0x00011101, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00040020 + (0x00000010 * i) );

			}
			for( i = 0 ; i < 4 ; i++ ) {
				
			//	writel( 0x00000001, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );
				writel( 0x00011101, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x000400C0 + (0x00000010 * i) );
			}

		}
		if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) {
			
		}
		//
		if( (pDevice->m_pKsDevice->vendor == 0x9413 && pDevice->m_pKsDevice->device == 0x6010) ) {

			//writel( 0x001E4409, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000014 ); { USHORT val = 0x0000; pci_read_config_word( pDevice->m_pKsDevice, PCI_STATUS, &val ); }
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) {
			
			writel( 0x000000FF, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00000104 );
		}
		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) {
			
			writel( 0x0000FF77, pDevice->m_pRegBaseCommonBuffer[ 0 ] + 0x00018004 );
		}
//peter [2011.06.29]
		if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) {
//already done at the end of ISR			
//			writel( 0x00000000, pDevice->m_pRegBaseCommonBuffer[ 1 ] + 0x0040001C );

//			LINUXV4L2_DEBUG( KERN_INFO, "FH8735() clear interrupt -------- 3 %08X\n", pDevice->m_pRegBaseCommonBuffer[ 1 ] );
		}

		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) {
		}

	}
	
	wrapper_spin_unlock_irqrestore( &pDevice->m_sTaskletAccessSpinLock, flags );
}

void common_pci_driver_on_tasklet_EX( unsigned long p_context )
{
	common_pci_driver_on_tasklet( p_context );
}

