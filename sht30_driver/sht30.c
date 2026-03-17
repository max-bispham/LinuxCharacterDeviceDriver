#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/delay.h>


// initializes major to negative
static int major = -1;

// This struct holds the kernel's client pointer
struct sht30_data {
	struct i2c_client *client;
};

static struct sht30_data *global_data;

// Device class struct
static struct class *sht30_class;

// Device device struct
static struct device *sht30_device;

// Struct to return reading
struct sht30_reading {
	u16 temperature;
	u16 humidity;
};


// Character Driver

static ssize_t sht30_read(struct file *f, char __user *u, size_t s, loff_t *l)
{
	struct sht30_data *data = f->private_data;
	struct i2c_client *client = data->client;
	struct sht30_reading reading;
	u8 cmd[2] = {0x2c, 0x06};
	u8 raw[6];
	int ret;
	
	printk("sht30_cdev - Read is called\n");

	// Send measurement command
	ret = i2c_master_send(client, cmd, 2);
	if (ret < 0)
	{
		printk("sht30 - failed to send measurement command\n");
		return ret;
	}

	// Delay 15 miliseconds for sht30 to respond
	msleep(15);

	// Read bytes back
	ret = i2c_master_recv(client, raw, 6);
	if (ret < 0)
	{
		printk("sht30 - failed to read measurement command\n");
		return ret;
	}

	// Data conversion
	u16  raw_temp = (raw[0] << 8 ) | raw[1];
	u16 raw_hum = (raw[3] << 8) | raw[4];

	reading.temperature = raw_temp;
	reading.humidity = raw_hum;

	if (copy_to_user(u, &reading, sizeof(reading)))
	{
		return -EFAULT;
	}

	return sizeof(reading);
}


static ssize_t sht30_write(struct file *f, const char __user *u, size_t s, loff_t *l)
{
	printk("sht30_cdev - Write is called\n");
	return 0;
}

static int sht30_open(struct inode *n, struct file *f)
{
	f->private_data = global_data;
	printk("sht30_cdev - Open is called\n");
	return 0;
}

static int sht30_release(struct inode *n, struct file *f)
{
	printk("sht30_cdev - Release is called\n");
	return 0;
}


static struct file_operations fops = {
	.read = sht30_read,
	.write = sht30_write,
	.open = sht30_open,
	.release = sht30_release
};






// I2C driver


static const struct of_device_id sht30_of_match[] = {
	{ .compatible = "adafruit,sht30" },
	{}
};

MODULE_DEVICE_TABLE(of, sht30_of_match);


static int sht30_probe(struct i2c_client *client)
{

	u8 cmd[2] = {0xF3, 0x2D};
	u8 status[3];
	int ret;

	struct sht30_data *data;


	printk("sht30 - probe called\n");

	// Check to make sure device is there
	ret = i2c_master_send(client, cmd, 2);
	if (ret < 0)
	{
		printk("sht30 - device not found\n");
		return ret;
	}

	ret = i2c_master_recv(client, status, 3);
	if (ret < 0)
	{
		printk("sht30 - failed to read status\n");
		return ret;
	}

	// Allocating private data

	data = devm_kzalloc(&client->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
	{
		return -ENOMEM;
	}
	data->client = client;
	i2c_set_clientdata(client, data);
	global_data = data;

	major = register_chrdev(0, "sht30", &fops);
	if (major < 0) {
		printk("sht30 - failed to register char device\n");
		return major;
	}

	// Creating a class
        sht30_class = class_create("sht30");
        if (IS_ERR(sht30_class))
        {       
        	unregister_chrdev(major, "sht30");
		return PTR_ERR(sht30_class);
        }

	// Creating a device
	sht30_device = device_create(sht30_class, NULL, MKDEV(major, 0), NULL, "sht30");
	if (IS_ERR(sht30_device))
	{
		class_destroy(sht30_class);
		unregister_chrdev(major, "sht30");
		return PTR_ERR(sht30_device);
	}

	return 0;
}


static void sht30_remove(struct i2c_client *client)
{
	printk("sht30 - remove called\n");

	device_destroy(sht30_class, MKDEV(major, 0));
	class_destroy(sht30_class);
	
	if (major >= 0)
	{
		unregister_chrdev(major, "sht30");
	}
}

static struct i2c_driver sht30_driver = {
        .driver = {
                .name = "sht30",
		.of_match_table = sht30_of_match,
        },
        .probe          = sht30_probe,
        .remove         = sht30_remove,
};



module_i2c_driver(sht30_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Bispham");
MODULE_DESCRIPTION("A character driver for the adafruit sht30 i2c temperature sensor");
