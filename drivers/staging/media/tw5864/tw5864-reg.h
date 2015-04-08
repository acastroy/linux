/*
 *  tw5864-reg.h - TW5864xx register offsets
 *
 *  Much of this code is derived from the cx88 and sa7134 drivers, which
 *  were in turn derived from the bt87x driver.  The original work was by
 *  Gerd Knorr; more recently the code was enhanced by Mauro Carvalho Chehab,
 *  Hans Verkuil, Andy Walls and many others.  Their work is gratefully
 *  acknowledged.  Full credit goes to them - any problems within this code
 *  are mine.
 *
 *  Copyright (C) William M. Brack
 *
 *  Refactored and updated to the latest v4l core frameworks:
 *
 *  Copyright (C) 2014 Hans Verkuil <hverkuil@xs4all.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
*/

#ifndef _TW5864_REG_H_
#define _TW5864_REG_H_

/* ---------------------------------------------------------------------- */
#define	TW5864_DMAC		0x000
#define	TW5864_DMAP_SA		0x004
#define	TW5864_DMAP_EXE		0x008
#define	TW5864_DMAP_PP		0x00c
#define	TW5864_VBIC		0x010
#define	TW5864_SBUSC		0x014
#define	TW5864_SBUSSD		0x018
#define	TW5864_INTSTAT		0x01C
#define	TW5864_INTMASK		0x020
#define	TW5864_GPIOC		0x024
#define	TW5864_GPOE		0x028
#define	TW5864_TESTREG		0x02C
#define	TW5864_SBUSRD		0x030
#define	TW5864_SBUS_TRIG		0x034
#define	TW5864_CAP_CTL		0x040
#define	TW5864_SUBSYS		0x054
#define	TW5864_I2C_RST		0x064
#define	TW5864_VBIINST		0x06C
/* define bits in FIFO and DMAP Control reg */
#define	TW5864_DMAP_EN		(1 << 0)
#define	TW5864_FIFO_EN		(1 << 1)
/* define the Interrupt Status Register bits */
#define	TW5864_SBDONE		(1 << 0)
#define	TW5864_DMAPI		(1 << 1)
#define	TW5864_GPINT		(1 << 2)
#define	TW5864_FFOF		(1 << 3)
#define	TW5864_FDMIS		(1 << 4)
#define	TW5864_DMAPERR		(1 << 5)
#define	TW5864_PABORT		(1 << 6)
#define	TW5864_SBDONE2		(1 << 12)
#define	TW5864_SBERR2		(1 << 13)
#define	TW5864_PPERR		(1 << 14)
#define	TW5864_FFERR		(1 << 15)
#define	TW5864_DET50		(1 << 16)
#define	TW5864_FLOCK		(1 << 17)
#define	TW5864_CCVALID		(1 << 18)
#define	TW5864_VLOCK		(1 << 19)
#define	TW5864_FIELD		(1 << 20)
#define	TW5864_SLOCK		(1 << 21)
#define	TW5864_HLOCK		(1 << 22)
#define	TW5864_VDLOSS		(1 << 23)
#define	TW5864_SBERR		(1 << 24)
/* define the i2c control register bits */
#define	TW5864_SBMODE		(0)
#define	TW5864_WREN		(1)
#define	TW5864_SSCLK		(6)
#define	TW5864_SSDAT		(7)
#define	TW5864_SBCLK		(8)
#define	TW5864_WDLEN		(16)
#define	TW5864_RDLEN		(20)
#define	TW5864_SBRW		(24)
#define	TW5864_SBDEV		(25)

#define	TW5864_SBMODE_B		(1 << TW5864_SBMODE)
#define	TW5864_WREN_B		(1 << TW5864_WREN)
#define	TW5864_SSCLK_B		(1 << TW5864_SSCLK)
#define	TW5864_SSDAT_B		(1 << TW5864_SSDAT)
#define	TW5864_SBRW_B		(1 << TW5864_SBRW)

#define	TW5864_GPDATA		0x100
#define	TW5864_STATUS1		0x204
#define	TW5864_INFORM		0x208
#define	TW5864_OPFORM		0x20C
#define	TW5864_HSYNC		0x210
#define	TW5864_ACNTL		0x218
#define	TW5864_CROP_HI		0x21C
#define	TW5864_VDELAY_LO		0x220
#define	TW5864_VACTIVE_LO		0x224
#define	TW5864_HDELAY_LO		0x228
#define	TW5864_HACTIVE_LO		0x22C
#define	TW5864_CNTRL1		0x230
#define	TW5864_VSCALE_LO		0x234
#define	TW5864_SCALE_HI		0x238
#define	TW5864_HSCALE_LO		0x23C
#define	TW5864_BRIGHT		0x240
#define	TW5864_CONTRAST		0x244
#define	TW5864_SHARPNESS		0x248
#define	TW5864_SAT_U		0x24C
#define	TW5864_SAT_V		0x250
#define	TW5864_HUE		0x254
#define	TW5864_SHARP2		0x258
#define	TW5864_VSHARP		0x25C
#define	TW5864_CORING		0x260
#define	TW5864_VBICNTL		0x264
#define	TW5864_CNTRL2		0x268
#define	TW5864_CC_DATA		0x26C
#define	TW5864_SDT		0x270
#define	TW5864_SDTR		0x274
#define	TW5864_RESERV2		0x278
#define	TW5864_RESERV3		0x27C
#define	TW5864_CLMPG		0x280
#define	TW5864_IAGC		0x284
#define	TW5864_AGCGAIN		0x288
#define	TW5864_PEAKWT		0x28C
#define	TW5864_CLMPL		0x290
#define	TW5864_SYNCT		0x294
#define	TW5864_MISSCNT		0x298
#define	TW5864_PCLAMP		0x29C
#define	TW5864_VCNTL1		0x2A0
#define	TW5864_VCNTL2		0x2A4
#define	TW5864_CKILL		0x2A8
#define	TW5864_COMB		0x2AC
#define	TW5864_LDLY		0x2B0
#define	TW5864_MISC1		0x2B4
#define	TW5864_LOOP		0x2B8
#define	TW5864_MISC2		0x2BC
#define	TW5864_MVSN		0x2C0
#define	TW5864_STATUS2		0x2C4
#define	TW5864_HFREF		0x2C8
#define	TW5864_CLMD		0x2CC
#define	TW5864_IDCNTL		0x2D0
#define	TW5864_CLCNTL1		0x2D4

/* Audio */
#define	TW5864_ACKI1		0x300
#define	TW5864_ACKI2		0x304
#define	TW5864_ACKI3		0x308
#define	TW5864_ACKN1		0x30C
#define	TW5864_ACKN2		0x310
#define	TW5864_ACKN3		0x314
#define	TW5864_SDIV		0x318
#define	TW5864_LRDIV		0x31C
#define	TW5864_ACCNTL		0x320

#define	TW5864_VSCTL		0x3B8
#define	TW5864_CHROMAGVAL		0x3BC

#define	TW5864_F2CROP_HI		0x3DC
#define	TW5864_F2VDELAY_LO	0x3E0
#define	TW5864_F2VACTIVE_LO	0x3E4
#define	TW5864_F2HDELAY_LO	0x3E8
#define	TW5864_F2HACTIVE_LO	0x3EC
#define	TW5864_F2CNT		0x3F0
#define	TW5864_F2VSCALE_LO	0x3F4
#define	TW5864_F2SCALE_HI		0x3F8
#define	TW5864_F2HSCALE_LO	0x3FC

#define	RISC_INT_BIT		0x08000000
#define	RISC_SYNCO		0xC0000000
#define	RISC_SYNCE		0xD0000000
#define	RISC_JUMP		0xB0000000
#define	RISC_LINESTART		0x90000000
#define	RISC_INLINE		0xA0000000

#define VideoFormatNTSC		 0
#define VideoFormatNTSCJapan	 0
#define VideoFormatPALBDGHI	 1
#define VideoFormatSECAM	 2
#define VideoFormatNTSC443	 3
#define VideoFormatPALM		 4
#define VideoFormatPALN		 5
#define VideoFormatPALNC	 5
#define VideoFormatPAL60	 6
#define VideoFormatAuto		 7

#define ColorFormatRGB32	 0x00
#define ColorFormatRGB24	 0x10
#define ColorFormatRGB16	 0x20
#define ColorFormatRGB15	 0x30
#define ColorFormatYUY2		 0x40
#define ColorFormatBSWAP         0x04
#define ColorFormatWSWAP         0x08
#define ColorFormatGamma         0x80
#endif
