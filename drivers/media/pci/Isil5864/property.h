#ifndef PROPERTY_H
#define PROPERTY_H

extern ULONG g_n_analog_decoder_brightness                    [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_contrast                      [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_saturation                    [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_hue                           [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_sharpness                     [ 64 ][ 4 ];

extern ULONG g_n_analog_decoder_gain_auto                     [ 64 ];

extern ULONG g_n_analog_decoder_gain                          [ 64 ];

extern ULONG g_n_analog_decoder_lock_status                   [ 64 ];


extern ULONG g_n_multiplexer_motion_status                    [ 64 ];

extern ULONG g_n_multiplexer_motion_mask_cx                   [ 64 ];

extern ULONG g_n_multiplexer_motion_mask_cy                   [ 64 ];

extern ULONG g_n_multiplexer_motion_sensitivity_low_threshold [ 64 ];

extern ULONG g_n_multiplexer_motion_sensitivity_high_threshold[ 64 ];


extern ULONG g_n_encoder_osd_pos_x							  [ 64 ];

extern ULONG g_n_encoder_osd_pos_y							  [ 64 ];

extern ULONG g_n_encoder_osd_color							  [ 64 ];

extern CHAR  g_p_encoder_osd_text 							  [ 64 ][ 64 + 1 ];


extern ULONG g_n_gpio_direction                               [ 64 ];

extern ULONG g_n_gpio_data                                    [ 64 ];

void common_pci_driver_on_tasklet_EX( unsigned long p_context );

//below is from kernel 3.2


#endif

