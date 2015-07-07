static void w_func(struct tw5864_dev *dev, u32 addr, u32 value)
{
    if (addr == TW5864_IND_CTL || addr == TW5864_IND_DATA) {
        int timeout = 30000;

        while ((tw_readl(TW5864_IND_CTL) >> 31) && (timeout--))
            ;
        if (!timeout)
            dev_err(&dev->pci->dev, "tw_indir_writel() timeout before reading\n");

        tw_writel(addr, value);
    } else if (addr == TW5864_IIC) {
        u32 val = 0;
        int timeout = 30000;
        unsigned long flags;
        local_irq_save(flags);

        tw_writel(addr, value);

        do{
            val = tw_readl(TW5864_IIC) & (0x01000000);
        } while ((!val) && (--timeout));
        local_irq_restore(flags);
    } else {

        tw_writel(addr, value);
    }

    if (!in_interrupt())
	    mdelay(10);
}

#define w(addr, value) w_func(dev, addr, value)
