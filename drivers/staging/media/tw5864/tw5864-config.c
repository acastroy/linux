//#include   <isil5864/isil_common.h>
#include "tw5864.h"
#include "tw5864-reg.h"

//#define PLATFORM_FPGA_155
#define PLATFORM_FPGA_330

#define TW5864_IIC_TIMEOUT  (30000)

static unsigned char tbl_pal_tw2864_common[] __used = {
	0x00, 0x00, 0x64, 0x11,
	0x80, 0x80, 0x00, 0x12,
	0x12, 0x20, 0x0a, 0xD0,
	0x00, 0x00, 0x07, 0x7F,
};

static unsigned char tbl_ntsc_tw2864_common[] __used = {
	0x00, 0x00, 0x64, 0x11,
	0x80, 0x80, 0x00, 0x02,
	0x12, 0xF0, 0x0C, 0xD0,
	0x00, 0x00, 0x07, 0x7F
};

static unsigned char tbl_pal_tw2864_common2[] __used = {
	0x00, 0x22, 0x00, 0x00,
	0x22, 0x00, 0x00, 0x22,
	0x00, 0x00, 0x22, 0x00,
};

static unsigned char tbl_tw2864_other[] __used = {
	0xfb, 0x6f, 0xfc, 0xff,
	0xdb, 0xc1, 0xd2, 0x01,
	0xdd, 0x00, 0xde, 0x00,
	0xe1, 0xc0, 0xe2, 0xaa,
	0xe3, 0xaa, 0xf8, 0x64,
	0xf9, 0x11, 0xaa, 0x00,
	0x9e, 0x72, 0x9c, 0x20,
	0x94, 0x14, 0xca, 0xaa,
	0xcb, 0x00, 0x89, 0x02,
	0xfa, 0xc6, 0xcf, 0x83,
#ifdef PLATFORM_FPGA_155
	0x9f, 0x07, 0x96, 0x02,
#endif
#ifdef PLATFORM_FPGA_330
	0x9f, 0x00, 0xb1, 0x2a,	//330
	0x9e, 0x7a,
#endif
};

static unsigned char tbl_pal_tw2865_common[] __used = {
	0x00, 0x00, 0x64, 0x11,
	0x80, 0x80, 0x00, 0x12,
	0x17, 0x20, 0x0C, 0xD0,
	0x00, 0x00, 0x07, 0x7F,
};

static unsigned char tbl_ntsc_tw2865_common[] __used = {
	0x00, 0x00, 0x64, 0x11,
	0x80, 0x80, 0x00, 0x02,
	0x12, 0xF0, 0x0C, 0xD0,
	0x00, 0x00, 0x07, 0x7F
};

static unsigned char tbl_tw2865_other1[] __used = {
	0xfa, 0x4a, 0xfb, 0x6f,
#ifdef PLATFORM_FPGA_155
	0xfc, 0xff, 0x9c, 0xa0,
#endif
#ifdef PLATFORM_FPGA_330
	0xfc, 0xff, 0x9c, 0x20,	//330
#endif
	0x9e, 0x72, 0xca, 0x02,
	0xf9, 0x51, 0xaa, 0x00,
	0x41, 0xd4, 0x43, 0x08,
	0x6b, 0x0f, 0x6c, 0x0f,
	0x61, 0x02, 0x96, 0xe6,	//0xe6
#ifdef PLATFORM_FPGA_155
	0x97, 0xc3, 0x9f, 0x77,
#endif
#ifdef PLATFORM_FPGA_330
	0x97, 0xc3, 0x9f, 0x03,	//330
	0xb1, 0x2a, 0x9e, 0x7a,
	0x18, 0x19, 0x1a, 0x06,
	0x28, 0x19, 0x2a, 0x06,
	0x38, 0x19, 0x3a, 0x06,
	0x60, 0x15,
#endif
};

static unsigned char tbl_tw2866_other1[] __used = {
	0xfa, 0x4a, 0xfb, 0x6f,
#ifdef PLATFORM_FPGA_155
	0xfc, 0xff, 0x9c, 0xa0,
#endif
#ifdef PLATFORM_FPGA_330
	0xfc, 0xff, 0x9c, 0x20,	//330
#endif
	0x9e, 0x72, 0xca, 0x02,
	0xf9, 0x51, 0xaa, 0x00,
	0x41, 0xd4, 0x43, 0x08,
	0x6b, 0x0f, 0x6c, 0x0f,
	0x61, 0x02, 0x96, 0xe6,	//0xe6
#ifdef PLATFORM_FPGA_155
	0x97, 0xc3, 0x9f, 0x77,
#endif
#ifdef PLATFORM_FPGA_330
	0x97, 0xc3, 0x9f, 0x00,	//330
	0xb1, 0x2a, 0x9e, 0x7a,
	0x5b, 0xff, 0x08, 0x19,
	0x0a, 0x06, 0x18, 0x19,
	0x1a, 0x06, 0x28, 0x19,
	0x2a, 0x06, 0x38, 0x19,
	0x3a, 0x06, 0x60, 0x15,
#endif
};

static unsigned char tbl_tw2865_other2[] __used = {
	0x73, 0x01, 0xf8, 0xc4,
	0xf9, 0x51, 0x70, 0x08,
	0x7f, 0x80, 0xcf, 0x80
};

static unsigned char tbl_tw2865_other3[] __used = {
	0x89, 0x05, 0x7e, 0xc0,
	0xe0, 0x00
};

static unsigned char audio_tw2865_common[] __used = {
	0x33, 0x33, 0x03, 0x31,
	0x75, 0xb9, 0xfd, 0x20,
	0x64, 0xa8, 0xec, 0xC1,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0xC0, 0xAA, 0xAA
};

static unsigned char audio_tbl_pal_tw2865_8KHz[] __used = {
	0x83, 0xB5, 0x09, 0x00,
	0xA0, 0x00
};

static unsigned char audio_tbl_pal_tw2865_16KHz[] __used = {
	0x07, 0x6B, 0x13, 0x00, 0x40, 0x01
};

static unsigned char audio_tbl_ntsc_tw2865_8KHz[] __used = {
	0x83, 0xB5, 0x09, 0x78, 0x85, 0x00
};

static unsigned char audio_tbl_ntsc_tw2865_16KHz[] __used = {
	0x07, 0x6B, 0x13, 0xEF, 0x0A, 0x01
};

int pci_i2c_read(struct tw5864_dev *dev, u8 devid, u8 devfn, u8 *buf);
int pci_i2c_multi_read(struct tw5864_dev *dev, u8 devid, u8 devfn, u8 *buf,
		       u32 count)
{
	int i = 0;
	u32 val = 0;
	int timeout = TW5864_IIC_TIMEOUT;
	unsigned long flags;

	local_irq_save(flags);
	for (i = 0; i < count; i++) {
		val = (1 << 24) | ((devid | 0x01) << 16) | ((devfn + i) << 8);

		tw_writel(TW5864_IIC, val);

		do {
			val = tw_readl(TW5864_IIC) & (0x01000000);
		} while ((!val) && (--timeout));
		if (!timeout) {
			local_irq_restore(flags);
			pr_err("dev 0x%x, fn 0x%x\n", devid, devfn);
			return -ETIMEDOUT;
		}
		buf[i] = (u8)tw_readl(TW5864_IIC);
	}
	local_irq_restore(flags);

	return 0;
}

int pci_i2c_multi_write(struct tw5864_dev *dev, u8 devid, u8 devfn, u8 *buf,
			u32 count)
{
	int i = 0;
	u32 val = 0;
	int timeout = TW5864_IIC_TIMEOUT;
	unsigned long flags;

	local_irq_save(flags);
	for (i = 0; i < count; i++) {
		val =
		    (1 << 24) | ((devid & 0xfe) << 16) | ((devfn + i) << 8) |
		    buf[i];
		tw_writel(TW5864_IIC, val);
		do {
			val = tw_readl(TW5864_IIC) & (0x01000000);
		} while ((!val) && (--timeout));
		if (!timeout) {
			local_irq_restore(flags);
			pr_err("dev 0x%x, fn 0x%x, 0x%x\n", devid, devfn,
			       buf[i]);
			return -ETIMEDOUT;
		}
	}
	local_irq_restore(flags);

	return 0;
}

int pci_i2c_read(struct tw5864_dev *dev, u8 devid, u8 devfn, u8 *buf)
{
	u32 val = 0;
	int timeout = TW5864_IIC_TIMEOUT;
	unsigned long flags;

	local_irq_save(flags);
	val = (1 << 24) | ((devid | 0x01) << 16) | (devfn << 8);

	tw_writel(TW5864_IIC, val);
	do {
		val = tw_readl(TW5864_IIC) & (0x01000000);
	} while ((!val) && (--timeout));
	if (!timeout) {
		local_irq_restore(flags);
		pr_err("dev 0x%x, fn 0x%x\n", devid, devfn);
		return -ETIMEDOUT;
	}

	*buf = (u8)tw_readl(TW5864_IIC);
	local_irq_restore(flags);

	return 0;
}

int pci_i2c_write(struct tw5864_dev *dev, u8 devid, u8 devfn, u8 buf)
{
	u32 val = 0;
	int timeout = TW5864_IIC_TIMEOUT;
	unsigned long flags;

	local_irq_save(flags);
	val = (1 << 24) + ((devid & 0xfe) << 16) + (devfn << 8) + buf;
	tw_writel(TW5864_IIC, val);
	do {
		val = tw_readl(TW5864_IIC) & (0x01000000);
	} while ((!val) && (--timeout));
	local_irq_restore(flags);
	if (!timeout) {
		pr_err("dev 0x%x, fn 0x%x, 0x%x\n", devid, devfn, buf);
		return -ETIMEDOUT;
	}

	return 0;
}

int pci_i2c_wscatter(struct tw5864_dev *dev, u8 devid, u8 *buf, u32 count)
{
	int i = 0;
	u32 val = 0;
	int timeout = TW5864_IIC_TIMEOUT;
	unsigned long flags;

	local_irq_save(flags);
	for (i = 0; i < count; i++) {
		val =
		    (1 << 24) + ((devid & 0xfe) << 16) + (buf[i * 2 + 0] << 8) +
		    buf[i * 2 + 1];
		tw_writel(TW5864_IIC, val);
		do {
			val = tw_readl(TW5864_IIC) & (0x01000000);
		} while ((!val) && (--timeout));
		if (!timeout) {
			local_irq_restore(flags);
			pr_err("dev 0x%x, fn 0x%x, 0x%x\n", devid, buf[i * 2],
			       buf[i * 2 + 1]);
			return -ETIMEDOUT;
		}
	}
	local_irq_restore(flags);

	return 0;
}

void pci_init_tw2864(struct tw5864_dev *dev, u8 iic)
{
	u32 ch;

	for (ch = 0; ch < 4; ch++) {
		pci_i2c_multi_write(dev, iic, ch * 0x10, tbl_pal_tw2864_common,
				    16);
	}
#ifdef PLATFORM_FPGA_155
	pci_i2c_wscatter(dev, iic, tbl_tw2864_other, 22);
#else
	pci_i2c_wscatter(dev, iic, tbl_tw2864_other, 23);
#endif
	pci_i2c_write(dev, iic, 0xcf, 0x83);
	pci_i2c_write(dev, iic, 0xe0, 0x00);
}

static __used void pci_init_tw2865(struct tw5864_dev *dev, u8 iic)
{
	u32 ch;

	for (ch = 0; ch < 4; ch++) {
		pci_i2c_multi_write(dev, iic, ch * 0x10, tbl_pal_tw2865_common,
				    16);
	}
#ifdef PLATFORM_FPGA_155
	pci_i2c_wscatter(dev, iic, tbl_tw2865_other1,
			 sizeof(tbl_tw2865_other1) >> 1);
#else
	pci_i2c_wscatter(dev, iic, tbl_tw2865_other1,
			 sizeof(tbl_tw2865_other1) >> 1);
#endif
	pci_i2c_multi_write(dev, iic, 0xd0, audio_tw2865_common, 20);
	pci_i2c_wscatter(dev, iic, tbl_tw2865_other2, 6);
	pci_i2c_multi_write(dev, iic, 0xf0, audio_tbl_pal_tw2865_8KHz, 6);
	pci_i2c_wscatter(dev, iic, tbl_tw2865_other3, 3);
	pci_i2c_write(dev, iic, 0xe0, 0x10);
}

#define ISIL_PHY_VD_CHAN_NUMBER   (16)

/*auto detect CLKP_DEL delay*/
static int tw28xx_clkp_delay(struct tw5864_dev *dev, u8 devid, u32 base_ch,
			     u32 limit)
{
	if (dev && (base_ch < ISIL_PHY_VD_CHAN_NUMBER)
	    && (limit <= (ISIL_PHY_VD_CHAN_NUMBER >> 2))) {
		int delay;
		u8 flags;

		delay = -1;
		pci_i2c_read(dev, devid, 0x9f, &flags);
		while ((++delay) < 0x10) {
			pci_i2c_write(dev, devid, 0x9f, delay);
			/*only bus0 can detect colume and line */
			tw_writel(TW5864_H264EN_BUS0_MAP, base_ch);
			tw_writel(0x9218, 0x1);	/*clear error flags */
			mdelay(100);
			if (tw_readl(0x9218)) {
				continue;
			}
			printk("auto detect CLKP_DEL = %02x\n", delay);
			break;
		}
		if (delay >= 0x10) {
			printk
			    ("can't find suitable clkp_del for devid 0x%02x\n",
			     devid);
			pci_i2c_write(dev, devid, 0x9f, flags);

			return -EFAULT;
		}
		return 0;
	}

	return 1;
}

void pci_init_ad(struct tw5864_dev *dev)
{
	unsigned int val;

	val = tw_readl(TW5864_IIC_ENB);
	val |= 0x01;
	tw_writel(TW5864_IIC_ENB, val);
	tw_writel(TW5864_I2C_PHASE_CFG, 0x01);

	pci_init_tw2864(dev, 0x52);
	tw28xx_clkp_delay(dev, 0x52, 4, 4);
	pci_init_tw2864(dev, 0x54);
	tw28xx_clkp_delay(dev, 0x54, 8, 4);
	pci_init_tw2864(dev, 0x56);
	tw28xx_clkp_delay(dev, 0x56, 12, 4);
	pci_init_tw2865(dev, 0x50);
}

//#endif //POWERPC_PCI_PLATFORM
