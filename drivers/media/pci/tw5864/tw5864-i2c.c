#include "tw5864.h"

#define TW5864_IIC_RETRIES 30000

static int tw5864_smbus_xfer(struct i2c_adapter *adap, u16 addr,
			     unsigned short flags, char read_write, u8 command,
			     int size, union i2c_smbus_data *data);
static u32 tw5864_i2c_functionality(struct i2c_adapter *adap);

static const struct i2c_algorithm tw5864_i2c_algo = {
	.smbus_xfer    = tw5864_smbus_xfer,
	.functionality = tw5864_i2c_functionality,
};

static int tw5864_i2c_read_internal(struct tw5864_dev *dev, u8 devid, u8 addr,
				    u8 *buf)
{
	u32 val;
	int retries = TW5864_IIC_RETRIES;

	tw_writel(TW5864_IIC, BIT(24) | (devid | 0x01) << 16 | addr << 8);
	do {
		val = tw_readl(TW5864_IIC);
	} while (!(val & BIT(24)) && --retries);

	if (!retries) {
		dev_err(&dev->pci->dev,
			"tw5864_i2c_read() out of attempts on dev 0x%x, fn 0x%x\n",
			devid, addr);
		return -ETIMEDOUT;
	}

	*buf = (u8) val;
	return 0;
}

static int tw5864_i2c_write_internal(struct tw5864_dev *dev, u8 devid, u8 addr,
				     u8 buf)
{
	int retries = TW5864_IIC_RETRIES;

	tw_writel(TW5864_IIC, BIT(24) | (devid & 0xfe) << 16 | addr << 8 | buf);
	while (!(tw_readl(TW5864_IIC) & BIT(24)) && --retries)
		;

	if (!retries) {
		dev_err(&dev->pci->dev,
			"tw5864_i2c_write() out of attempts on dev 0x%x, fn 0x%x\n",
			devid, addr);
		return -ETIMEDOUT;
	}
	return 0;
}

int tw5864_i2c_multi_write(struct tw5864_dev *dev, u8 i2c_index, u8 addr,
			   const u8 *buf, u32 count)
{
	int i;
	int ret;

	BUG_ON(i2c_index > 3);

	for (i = 0; i < count; i++) {
		ret = tw5864_i2c_write(dev, i2c_index, addr + i, buf[i]);
		if (ret)
			return ret;
	}
	return 0;
}

int tw5864_i2c_scatter_write(struct tw5864_dev *dev, u8 i2c_index,
			     const u8 *buf, u32 count)
{
	int i;
	u32 ret;

	BUG_ON(i2c_index > 3);

	for (i = 0; i < count; i++) {
		ret = tw5864_i2c_write(dev, i2c_index,
				       buf[2 * i], buf[2 * i + 1]);
		if (ret)
			return ret;
	}
	return 0;
}

static int tw5864_smbus_xfer(struct i2c_adapter *adap, u16 addr,
			     unsigned short flags, char read_write, u8 command,
			     int size, union i2c_smbus_data *data)
{
	struct tw5864_i2c_adap *ctx = adap->algo_data;
	struct tw5864_dev *dev = ctx->dev;
	int ret;

	if (size != I2C_SMBUS_BYTE_DATA)
		return -EIO;

	BUG_ON(!adap);
	BUG_ON(adap->algo != &tw5864_i2c_algo);

	mutex_lock(&dev->i2c_lock);
	/* TODO Merge tw5864_i2c_*_internal() into here */
	if (read_write == I2C_SMBUS_READ)
		ret = tw5864_i2c_read_internal(dev, ctx->devid, command,
					       &data->byte);
	else
		ret = tw5864_i2c_write_internal(dev, ctx->devid, command,
						data->byte);
	mutex_unlock(&dev->i2c_lock);

	return ret;
}

int tw5864_i2c_read(struct tw5864_dev *dev, u8 i2c_index, u8 offset, u8 *data)
{
	struct i2c_client *client = &dev->i2c[i2c_index].client;
	s32 ret;

	ret = i2c_smbus_read_byte_data(client, offset);
	*data = ret;
	return ret;
}

int tw5864_i2c_write(struct tw5864_dev *dev, u8 i2c_index, u8 offset, u8 data)
{
	struct i2c_client *client = &dev->i2c[i2c_index].client;

	return i2c_smbus_write_byte_data(client, offset, data);
}

static u32 tw5864_i2c_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_BYTE_DATA;
}

void tw5864_i2c_fini(struct tw5864_dev *dev)
{
	struct i2c_adapter *adap;
	int i;

	for (i = 0; i < 4; i++) {
		adap = &dev->i2c[i].adap;
		if (adap->algo_data) {
			i2c_del_adapter(adap);
			adap->algo_data = NULL;
		}
	}

	mutex_destroy(&dev->i2c_lock);
}

int tw5864_i2c_init(struct tw5864_dev *dev)
{
	int ret;
	int i;
	struct tw5864_i2c_adap *ctx;
	struct i2c_adapter *adap;
	struct i2c_client *client;

	tw_writel(TW5864_IIC_ENB, 1);
	tw_writel(TW5864_I2C_PHASE_CFG, 1);

	mutex_init(&dev->i2c_lock);

	dev->i2c[0].devid = 0x50; /* tw2865 */
	dev->i2c[1].devid = 0x52; /* tw2864 */
	dev->i2c[2].devid = 0x54; /* tw2864 */
	dev->i2c[3].devid = 0x56; /* tw2864 */

	for (i = 0; i < 4; i++) {
		ctx = &dev->i2c[i];
		adap = &ctx->adap;
		client = &ctx->client;

		ctx->dev = dev;
		snprintf(adap->name, sizeof(adap->name),
			 "tw5864 0x%02x", ctx->devid);
		adap->algo = &tw5864_i2c_algo;
		adap->algo_data = ctx;
		adap->timeout = msecs_to_jiffies(1000);
		adap->retries = 3;
		adap->dev.parent = &dev->pci->dev;

		ret = i2c_add_adapter(adap);
		if (ret) {
			adap->algo_data = NULL;
			break;
		}

		client->adapter = adap;
		client->addr = ctx->devid;
	}

	if (ret)
		tw5864_i2c_fini(dev);

	return ret;
}
