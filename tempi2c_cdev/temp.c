#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/i2c.h>


static int major = -1;

static struct i2c_client *global_client;


// Character Driver

static ssize_t temp_read(struct file *f, char __user *u, size_t s, loff_t *l)
{
	printk("temp_cdev - Read is called\n");
	return 0;
}


static ssize_t temp_write(struct file *f, const char __user *u, size_t s, loff_t *l)
{
	printk("temp_cdev - Write is called\n");
	return 0;
}

static int temp_open(struct inode *n, struct file *f)
{
	f->private_data = global_client;
	printk("temp_cdev - Open is called\n");
	return 0;
}

static int temp_release(struct inode *n, struct file *f)
{
	printk("temp_cdev - Release is called\n");
	return 0;
}


static struct file_operations fops = {
	.read = temp_read,
	.write = temp_write,
	.open = temp_open,
	.release = temp_release
};






// I2C driver

static const struct i2c_device_id temp_idtable[] = {
	{"temp", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, temp_idtable);

static int temp_probe(struct i2c_client *client)
{
	printk("temp - probe called\n");
	global_client = client;

	major = register_chrdev(0, "temp", &fops);
	if (major < 0) {
		printk("temp - failed to register char device\n");
		return major;
	}

	return 0;
}


static void temp_remove(struct i2c_client *client)
{
	printk("temp - remove called\n");
	if (major >= 0) {
		unregister_chrdev(major, "temp");	
	}
}

static struct i2c_driver temp_driver = {
        .driver = {
                .name = "temp",
        },
        .id_table       = temp_idtable,
        .probe          = temp_probe,
        .remove         = temp_remove,
};



module_i2c_driver(temp_driver);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Max Bispham");
MODULE_DESCRIPTION("A simple driver for registering a character device");
