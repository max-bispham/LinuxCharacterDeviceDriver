#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/i2c.h>


static int major = -1;

static struct i2c_client *global_client;


// Character Driver

static ssize_t sht30_read(struct file *f, char __user *u, size_t s, loff_t *l)
{
	printk("sht30_cdev - Read is called\n");
	return 0;
}


static ssize_t sht30_write(struct file *f, const char __user *u, size_t s, loff_t *l)
{
	printk("sht30_cdev - Write is called\n");
	return 0;
}

static int sht30_open(struct inode *n, struct file *f)
{
	f->private_data = global_client;
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

static const struct i2c_device_id sht30_idtable[] = {
	{"adafruit,sht30", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, sht30_idtable);

static int sht30_probe(struct i2c_client *client)
{
	printk("sht30 - probe called\n");
	global_client = client;

	major = register_chrdev(0, "sht30", &fops);
	if (major < 0) {
		printk("sht30 - failed to register char device\n");
		return major;
	}

	return 0;
}


static void sht30_remove(struct i2c_client *client)
{
	printk("sht30 - remove called\n");
	if (major >= 0) {
		unregister_chrdev(major, "sht30");	
	}
}

static struct i2c_driver sht30_driver = {
        .driver = {
                .name = "sht30",
        },
        .id_table       = sht30_idtable,
        .probe          = sht30_probe,
        .remove         = sht30_remove,
};



module_i2c_driver(sht30_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Bispham");
MODULE_DESCRIPTION("A character driver for the adafruit sht30 i2c temperature sensor");
