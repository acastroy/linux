// HDTV R2Y color space transfer matrix

static const BYTE ColorMatrix1TO1[] =
{
    0x10, 0x00,    0x00, 0x00,    0x00, 0x00, // M11, M12, M13
    0x00, 0x00,    0x10, 0x00,    0x00, 0x00, // M21, M22, M23
    0x00, 0x00,    0x00, 0x00,    0x10, 0x00, // M31, M32, M33
    0x20, 0x00,    0x04, 0x00,    0x20, 0x00  //  A1,  A2,  A3
};

// Sine 0 to Sine 45

static const SHORT SineTable[ 34 ] = {

   0,  348,  697, 1045, 1391, 1736, 2079, 2419, 2756, 3090, 3420, 3746, 4067,
4383, 4694, 4999, 5299, 5591, 5877, 6156, 6427, 6691, 6946, 7009, 7193, 7431,
7660, 7880, 8090, 8290, 8480, 8660, 8829, 8987

};

// do Hue adjustment from -45 to +45

// Cosine 0 to Cosine 45,

static const SHORT CosineTable[ 34 ] = {

10000,9993, 9975, 9945, 9902, 9848, 9781, 9702, 9612, 9510, 9396, 9271, 9135,
8987, 8829, 8660, 8480, 8290, 8090, 7880, 7660, 7431, 7193, 7132, 6946, 6691,
6427, 6156, 5877, 5591, 5299, 5000, 4694, 4383

};

static USHORT Cosine, Sine, Rate;

static USHORT SwapWord( USHORT src )
{
    BYTE temp, temp1;

	temp = (src & 0xff);

	temp1 = (BYTE)(src >> 8);

	return (USHORT)(temp << 8)|temp1;
}

#define MUL_RATE	5

#define DIV_RATE	4

#define RATE		10000

#define HALF_RATE	(RATE / 2)

static USHORT FixedPointAdd( USHORT src, BYTE adder )
{
    USHORT tempword;

	USHORT gap;

//alwayse positive for SRC input

	tempword = SwapWord(src);

	tempword = tempword >> 6;

	gap = (((USHORT)adder - 0x80)*MUL_RATE)/DIV_RATE;

	tempword = tempword + gap;

	tempword = tempword << 6;

	tempword = tempword & 0x7fff;

	tempword |= 1;			//to already did flag

	return SwapWord( tempword );
}

static USHORT FixedPointMul( USHORT src )
{
	USHORT tempword;

	tempword = SwapWord(src);

	tempword = tempword << 1;

	tempword = (USHORT)(((int)(short)tempword*Rate)/RATE);

	tempword = tempword >> 1;

	if((src & 0x40) == 0)
	{
		//positive
		if(tempword >= 0x3fff) tempword = 0x3fff;
	}
	else
	{
		//negative
		if(tempword <= 0x3fff) tempword = 0x4000;
	}
	return SwapWord(tempword);
}


static void GetCoSine(BYTE hue)
{
	USHORT ret, ret1;

	BYTE tempbyte, neg;

	neg = 0;

	if( hue >= 128 )
	{
		hue = hue - 128;
	}
	else
	{
		hue = 128 - hue;
		neg = 1;
	}
	tempbyte = hue / 4;

	ret  = SineTable[ tempbyte ];

	ret1 = SineTable[ tempbyte + 1 ];

	ret1 = ret + ((ret1-ret)*(hue&3))/4;

	Sine = ret1;

	if( neg )
	{
	    Sine = 0xffff - ret1 + 1;
	}
	ret  = CosineTable[tempbyte];

	ret1 = CosineTable[tempbyte+1];

   	ret1 = ret + ((ret1-ret)*(hue&3))/4;

	Cosine = ret1;
}

// Cb' = CbCosX + CrSinX

// Cr' = CrCosX - CbSinX

static USHORT FixedPointMul2( USHORT cb_src, USHORT cr_src, BYTE is_cb )
{
	USHORT tempcb, tempcr, tempword;

	tempcb = SwapWord(cb_src)<<1;

	tempcr = SwapWord(cr_src)<<1;

	if( is_cb == 1 )
	{
		tempword = (USHORT)(((int)((short)tempcb)*(short)Cosine + (int)((short)tempcr)*(short)Sine)/RATE);
	}
	else
	{
		tempword = (USHORT)(((int)((short)tempcr)*(short)Cosine - (int)((short)tempcb)*(short)Sine)/RATE);
	}
	tempword = tempword >> 1;

	return SwapWord( tempword );
}

void FixedPointMatrixTo3367( SHORT * pSrc, BYTE * pDst )
{
	int i = 0;
	for( i = 0; i < 12; ++i ) {

		pDst[ 2*i   ] = (BYTE)( pSrc[ i ] >> 8 );
		pDst[ 2*i+1 ] = (BYTE)( pSrc[ i ] );
	}
}

// Matrix multiplication of result fractional 12-bit from fractional 12-bit scalar in place
void FixedPointMatrix_12_Mul_12( SHORT * pDst, SHORT nSrc )
{
	int i = 0;
	for( i = 0; i < 9; ++i ) {

		pDst[ i ] = (SHORT)( ((LONG)pDst[ i ] * (LONG)nSrc) >> 12 );
	}
}

void MST3367_ADJUST_CSC_TABLE( CDevice * pDevice, BYTE bright, BYTE contrast, BYTE saturation, BYTE hue, BYTE sharpness )
{
	
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] MST3367_ADJUST_CSC_TABLE() bright(%d) contrast(%d) saturation(%d) hue(%d) sharpness(%d)\n", pDevice->m_nKsDeviceNumber, bright, contrast, saturation, hue, sharpness);

	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif


	if( saturation == 0 ) saturation = 1;

	int init = 0;

	BYTE tempbyte = MST3367_GetRegister( pDevice, 0, 0x92 );

	if( (tempbyte & 0x40) == 0) {

	    init = 1;

		memcpy( p_sys_cfg->o_csc_original.b.data, ColorMatrix1TO1, sizeof(ColorMatrix1TO1) );
	}
	else {

	    tempbyte = MST3367_GetRegister( pDevice, 0, 0xAA ) & 1; // low byte of CSC_A3

		if( tempbyte == 0 ) {

			int i = 0;
			for( i = 0 ; i < sizeof(ColorMatrix1TO1) ; i++ ) {

    			p_sys_cfg->o_csc_original.b.data[ i ] = MST3367_GetRegister( pDevice, 0, 0x93 + i );
			}
		}
	}

	if((p_sys_cfg->o_csc_original.b.data[sizeof(ColorMatrix1TO1)-1]&0x1) == 0)
	{
		p_sys_cfg->o_csc_original.b.data[sizeof(ColorMatrix1TO1)-1] |= 1;
		memcpy(p_sys_cfg->o_csc_matrix.b.data, p_sys_cfg->o_csc_original.b.data, sizeof(ColorMatrix1TO1));
	}
//	if(bright != 0x80)
	{
//Yout = [(CON/128)*(Y-128)]+BRI
		p_sys_cfg->o_csc_matrix.w.CSC_A2 = FixedPointAdd(p_sys_cfg->o_csc_original.w.CSC_A2, bright);
	}

//add video parameter
	{
		MST3367_SetRegister( pDevice, 0x00, 0xAB, 0x15 ); // [2012.05.08] [COLOR.RANGE] 0x15

		BYTE R00AC = MST3367_GetRegister( pDevice, 0x00, 0xAC );

		MST3367_SetRegister( pDevice, 0x00, 0xAC, R00AC & ~0x3F | 0x15 ); // [2012.05.08] [COLOR.RANGE] 0x15
	}

//	if(contrast != 0x80)
	{
		Rate = (USHORT)(((unsigned int)contrast*RATE)/128);
		p_sys_cfg->o_csc_matrix.w.CSC_M12 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M12);
		p_sys_cfg->o_csc_matrix.w.CSC_M22 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M22);
		p_sys_cfg->o_csc_matrix.w.CSC_M32 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M32);
	}
	if(saturation != 0x80)
	{
//(Cb, Cr)out = [(SAT/128)*(Cb, Cr -128)]+(Cb-hue+128, Cr+hue+128)
		Rate = (USHORT)(((unsigned int)saturation*RATE)/128);
		p_sys_cfg->o_csc_matrix.w.CSC_M11 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M11);
		p_sys_cfg->o_csc_matrix.w.CSC_M21 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M21);
		p_sys_cfg->o_csc_matrix.w.CSC_M31 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M31);
		p_sys_cfg->o_csc_matrix.w.CSC_M13 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M13);
		p_sys_cfg->o_csc_matrix.w.CSC_M23 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M23);
		p_sys_cfg->o_csc_matrix.w.CSC_M33 = FixedPointMul(p_sys_cfg->o_csc_original.w.CSC_M33);
	}
	else
	{
		p_sys_cfg->o_csc_matrix.w.CSC_M11 = p_sys_cfg->o_csc_original.w.CSC_M11;
		p_sys_cfg->o_csc_matrix.w.CSC_M21 = p_sys_cfg->o_csc_original.w.CSC_M21;
		p_sys_cfg->o_csc_matrix.w.CSC_M31 = p_sys_cfg->o_csc_original.w.CSC_M31;
		p_sys_cfg->o_csc_matrix.w.CSC_M13 = p_sys_cfg->o_csc_original.w.CSC_M13;
		p_sys_cfg->o_csc_matrix.w.CSC_M23 = p_sys_cfg->o_csc_original.w.CSC_M23;
		p_sys_cfg->o_csc_matrix.w.CSC_M33 = p_sys_cfg->o_csc_original.w.CSC_M33;
	}
//	if(hue != 0x80)
	{
	    WORD temp11, temp21, temp31, temp13, temp23, temp33;

        if(hue == 0)
		    hue = 1;
        hue = 256 - hue;
		GetCoSine(hue);
		temp11 = FixedPointMul2(p_sys_cfg->o_csc_matrix.w.CSC_M13, p_sys_cfg->o_csc_matrix.w.CSC_M11, 0);
		temp21 = FixedPointMul2(p_sys_cfg->o_csc_matrix.w.CSC_M23, p_sys_cfg->o_csc_matrix.w.CSC_M21, 0);
		temp31 = FixedPointMul2(p_sys_cfg->o_csc_matrix.w.CSC_M33, p_sys_cfg->o_csc_matrix.w.CSC_M31, 0);
		temp13 = FixedPointMul2(p_sys_cfg->o_csc_matrix.w.CSC_M13, p_sys_cfg->o_csc_matrix.w.CSC_M11, 1);
		temp23 = FixedPointMul2(p_sys_cfg->o_csc_matrix.w.CSC_M23, p_sys_cfg->o_csc_matrix.w.CSC_M21, 1);
		temp33 = FixedPointMul2(p_sys_cfg->o_csc_matrix.w.CSC_M33, p_sys_cfg->o_csc_matrix.w.CSC_M31, 1);

		p_sys_cfg->o_csc_matrix.w.CSC_M11 = temp11;
		p_sys_cfg->o_csc_matrix.w.CSC_M21 = temp21;
		p_sys_cfg->o_csc_matrix.w.CSC_M31 = temp31;
		p_sys_cfg->o_csc_matrix.w.CSC_M13 = temp13;
		p_sys_cfg->o_csc_matrix.w.CSC_M23 = temp23;
		p_sys_cfg->o_csc_matrix.w.CSC_M33 = temp33;
	}
	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||

		pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // ANALOG SHARPNESS

		MST3367_SetRegister( pDevice, 0x00, 0xAD, 0x05 );

		MST3367_SetRegister( pDevice, 0x00, 0x1E, (sharpness << 4) | (sharpness << 0) );

		MST3367_SetRegister( pDevice, 0x00, 0x1F, (sharpness << 0) );
	}
	else { // DIGITAL SHARPNESS

		if( sharpness == 0 ) { 
			
			MST3367_SetRegister( pDevice, 0x00, 0xAD, 0x07 ); 
		}
		else if( sharpness <= 128  ) { 
			
			MST3367_SetRegister( pDevice, 0x00, 0xAD, 0x05 ); 
		}
		else { 
			
			MST3367_SetRegister( pDevice, 0x00, 0xAD, 0x00 ); 
		}
		MST3367_SetRegister( pDevice, 0x00, 0x1E, (1 << 4) | (1 << 0) );

		MST3367_SetRegister( pDevice, 0x00, 0x1F, (1 << 0) );
	}
	int i = 0 ;
	for( i = 0;i < sizeof(ColorMatrix1TO1);i++ )
	{
		MST3367_SetRegister( pDevice, 0, 0x93+i, p_sys_cfg->o_csc_matrix.b.data[i] );
	}
	if(init == 1)
	{
		MST3367_SetRegister( pDevice, 0, 0x92, 0x66 );
	}
}

VOID MST3367_SETUP_CSC_TABLE( CDevice * pDevice )
{
	#ifdef AME_SA7160

	SA7160_SYS_CFG * p_sys_cfg = (SA7160_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif
	
	#ifdef AME_FH8735

	FH8735_SYS_CFG * p_sys_cfg = (FH8735_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	#ifdef AME_MZ0380
	
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	
	#endif

	
	BOOL VideoInputRGB = FALSE;

	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

		VideoInputRGB = TRUE;
	}
	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

	    pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { // DVI.DIGITAL (TMDS.A)

		VideoInputRGB = ( p_sys_cfg->n_input_video_colorspace == 0 );
	}
	ULONG ColorRangeControl = pDevice->m_nCustomAnalogVideoColorRangeProperty;

	ULONG mask = 0;

	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

	    pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { // DVI.DIGITAL (TMDS.A)

		BYTE R024A = MST3367_GetRegister( pDevice, 0x02, 0x4A );

		switch( (R024A & 0x0C) >> 2 ) {

		case 0: ColorRangeControl >>= 0; mask = 0; break; // Default(depend on video format)

		case 1: ColorRangeControl >>= 4; mask = 1; break; // Limited range

		case 2: ColorRangeControl >>= 8; mask = 2; break; // Full range
		}
	}
	if ( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ) { // COMPONENT

		ColorRangeControl >>= 12;

		mask = 3; 
	}
	if ( pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ) { // DVI.ANALOG

		ColorRangeControl >>= 16;

		mask = 4;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "COLOR RANGE = %d, %d, %02X, %08X\n", mask, VideoInputRGB, ColorRangeControl, pDevice->m_nCustomAnalogVideoColorRangeProperty);

	BOOL  Shrink = (ColorRangeControl & (1 << 0));

	BOOL  Expand = (ColorRangeControl & (1 << 1));

	SHORT Matrix[ 12 ];

	SHORT Matrix_I[  ]   = {  4096,     0,     0,     0,  4096,     0,     0,     0,  4096,  8192,     0,  8192 };

	SHORT Matrix_R2YS[ ] = {  2050,  1221,  -691, -1718,  2395, -1359,  -332,   466,  2050 };

	SHORT Matrix_R2YH[ ] = {  2050,   869,  -472, -1863,  2917, -1578,  -187,   295,  2050 };

	SHORT Scalar_Expand  = 4766;

	SHORT Scalar_Shrink  = 3520;

	BYTE R0092 = 0x62;

	memcpy( Matrix, Matrix_I, sizeof(Matrix) );

	if( VideoInputRGB ) {

		R0092 &= ~0x33; // set PreOffset[{0,2}] to 0

		if( (p_sys_cfg->n_input_video_resolution_cx == 640) ||

			(p_sys_cfg->n_input_video_resolution_cx == 720) ||

			(p_sys_cfg->n_input_video_resolution_cx == 768) ) {

			memcpy( Matrix, Matrix_R2YS, sizeof(Matrix_R2YS) );
		}
		else {

			memcpy( Matrix, Matrix_R2YH, sizeof(Matrix_R2YH) );
		}
	}
	if( Expand ) {

		R0092 |= 0x04; // set PreOffset[1] to 16

		if( !(R0092 & 0x33) ) { // if PreOffset[{0,2}] are {0,0}

			R0092 |= 0x11; // set PreOffset[{0,2}] = 16
		}
		FixedPointMatrix_12_Mul_12( Matrix, Scalar_Expand );
	}
	else if ( Shrink ) {

		Matrix[ 10 ] = (16 << 6); // set PostOffset[1] to 16

		FixedPointMatrix_12_Mul_12( Matrix, Scalar_Shrink );
	}

	BYTE Table[ 24 ];

	FixedPointMatrixTo3367( Matrix, Table );

	BYTE R00AB = MST3367_GetRegister( pDevice, 0x00, 0xAB ) & 0x7F;

	MST3367_SetRegister( pDevice, 0x00, 0xAB, R00AB | 0x80 ); // BLANK.OUTPUT

	MST3367_SetRegister( pDevice, 0x00, 0x90, 0x15 ); // [2012.05.08] [COLOR.RANGE] 

	MST3367_SetRegister( pDevice, 0x00, 0x91, 0x15 ); // [2012.05.08] [COLOR.RANGE] 

	MST3367_SetRegister( pDevice, 0x00, 0x92, R0092 );

	int i = 0;
	for( i = 0 ; i < sizeof(Table) ; i++ ) {

		MST3367_SetRegister( pDevice, 0x00, (BYTE)(0x93 + i), Table[ i ] ); 
	}
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
									   
									   (BYTE)(g_n_analog_decoder_hue[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF),

									   (BYTE)(g_n_analog_decoder_sharpness[ pDevice->m_nKsDeviceNumber * 1 ][ 0 ] & 0xFF));


	BYTE R00B0 = MST3367_GetRegister( pDevice, 0x00, 0xB0 ); R00B0 &= 0xC2;

	//#if( AMEBDAD_PRODUCT == AME_SA7160 )
	#ifdef AME_SA7160

	if( pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||  // HDMI (TMDS.B)

	    pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ) { // DVI.DIGITAL (TMDS.A)

		if( ((p_sys_cfg->n_input_video_resolution_cx == 1280) && (p_sys_cfg->n_input_video_resolution_cy == 720) && (p_sys_cfg->n_input_video_resolution_fps == 30)) ||

		    ((p_sys_cfg->n_input_video_resolution_cx == 1280) && (p_sys_cfg->n_input_video_resolution_cy == 720) && (p_sys_cfg->n_input_video_resolution_fps == 25)) ||

		    ((p_sys_cfg->n_input_video_resolution_cx == 1280) && (p_sys_cfg->n_input_video_resolution_cy == 720) && (p_sys_cfg->n_input_video_resolution_fps == 24)) ) {

			p_sys_cfg->b_input_video_resolution_external_sync = TRUE;
		}
		else {

			p_sys_cfg->b_input_video_resolution_external_sync = FALSE;
		}
	}
	else {

		p_sys_cfg->b_input_video_resolution_external_sync = FALSE;
	}
	if( p_sys_cfg->b_input_video_resolution_external_sync ) {

	//	MST3367_SetRegister( pDevice, 0x00, 0xB0, R00B0 | 0x24 ); // RX_OUTPUT_YUV422 / 10.BITS / EXTERNAL SYNC

		MST3367_SetRegister( pDevice, 0x00, 0xB0, R00B0 | 0x20 ); // RX_OUTPUT_YUV422 / 08.BITS / EXTERNAL SYNC
	}
	else {

	//	MST3367_SetRegister( pDevice, 0x00, 0xB0, R00B0 | 0x25 ); // RX_OUTPUT_YUV422 / 10.BITS / EMBEDDED SYNC

		MST3367_SetRegister( pDevice, 0x00, 0xB0, R00B0 | 0x21 ); // RX_OUTPUT_YUV422 / 08.BITS / EMBEDDED SYNC
	}

	#endif

	//#if( AMEBDAD_PRODUCT == AME_FH8735 )
	#ifdef AME_FH8735

	MST3367_SetRegister( pDevice, 0x00, 0xB0, R00B0 | 0x21 ); // RX_OUTPUT_YUV422 / 08.BITS / EMBEDDED SYNC

	#endif

	//#if( AMEBDAD_PRODUCT == AME_MZ0380 )
	#ifdef AME_MZ0380

	MST3367_SetRegister( pDevice, 0x00, 0xB0, R00B0 | 0x21 ); // RX_OUTPUT_YUV422 / 08.BITS / EMBEDDED SYNC

	#endif


	MST3367_SetRegister( pDevice, 0x00, 0xAB, R00AB ); // NORMAL.OUTPUT

}

