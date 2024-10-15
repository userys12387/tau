#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/ctype.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define PIN_MAX_COUNT 20
#define PIN_NAME_SIZE 16

#define GPIO1_NUM(x) (x + 480)
#define GPIO2_NUM(x) (x + 448)
#define GPIO3_NUM(x) (x + 416)
#define GPIO4_NUM(x) (x + 384)

#define AG1002AM_IO_SYSFS_DEFINE(sysfs_node, filename) \
    ssize_t ag1002am_io_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf); \
    ssize_t ag1002am_io_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count); \
    \
    struct device_attribute ag1002am_io_attr_##sysfs_node = { \
        .attr = { \
            .name = filename, \
            .mode = 0777, \
        }, \
	    .show  = ag1002am_io_show_##sysfs_node, \
        .store = ag1002am_io_store_##sysfs_node,\
    };

#define DEVICE_CREATE_FILE(dev, sysfs_node)                      \
    if (device_create_file(dev, &ag1002am_io_attr_##sysfs_node)) \
        dev_err(dev, "Unable to create sysfs entry: '%s'\n",     \
                ag1002am_io_attr_##sysfs_node.attr.name);


struct class *ag1002am_io_class;
struct device *ag1002am_io_device[PIN_MAX_COUNT];
static int major = 0;
static char *pin;

module_param(pin, charp, 0644);

ssize_t ag1002am_io_show_in(struct device *dev, struct device_attribute *attr, char *buf)
{
    int value = 0, gpio_num = 0;
    gpio_num = simple_strtol(dev->kobj.name, NULL, 10);
    value = gpio_get_value(gpio_num);
    return sprintf(buf, "%d\n", value);
}

ssize_t ag1002am_io_show_out(struct device *dev, struct device_attribute *attr, char *buf)
{
    int value = 0, gpio_num = 0;
    gpio_num = simple_strtol(dev->kobj.name, NULL, 10);
    value = gpio_get_value(gpio_num);
    return sprintf(buf, "%d\n", value);
}

ssize_t ag1002am_io_store_in(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    return count;
}

ssize_t ag1002am_io_store_out(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int value = 0, gpio_num = 0;

    value = buf[0] - '0';
    if (value > 0x1 || value < 0x0)
    {
        printk("[NOTICE]: module ag1002am_io, your input out of range");
        return 0;
    }
    gpio_num = simple_strtol(dev->kobj.name, NULL, 10);
    gpio_set_value(gpio_num, value);
    return count;
}

ssize_t ag1002am_io_store_direct(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int gpio_num = 0;

    gpio_num = simple_strtol(dev->kobj.name, NULL, 10);

    // 写1输入，写2输出
    if (strncmp("in", buf, 2) == 0 || strncmp("IN", buf, 2) == 0)
    {
        gpio_direction_input(gpio_num);
    }

    else if (strncmp("out", buf, 3) == 0 || strncmp("OUT", buf, 3) == 0)
    {
        gpio_direction_output(gpio_num, 0);
    }

    return count;
}

ssize_t ag1002am_io_show_direct(struct device *dev, struct device_attribute *attr, char *buf)
{
    return 0;
}

AG1002AM_IO_SYSFS_DEFINE(in, "in");
AG1002AM_IO_SYSFS_DEFINE(out, "out");
AG1002AM_IO_SYSFS_DEFINE(direct, "direct");

static int ag1002am_io_open(struct inode *inode, struct file *file)
{
    return 0;
}

int ag1002am_io_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t ag1002am_io_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    return 0;
}

static struct file_operations ag1002am_io_fops = {
    .owner = THIS_MODULE,
    .open = ag1002am_io_open,
    .write = ag1002am_io_write,
    .release = ag1002am_io_release,
};

static int __init ag1002am_io_init(void)
{
    int i = 0;
    char *seq = ",";
    char *token;
    char gpio_num[8] = {0};
    // int major_num = 0;
    int minor_num = 0;

    major = register_chrdev(0, "ag1002am_io", &ag1002am_io_fops);
    ag1002am_io_class = class_create(THIS_MODULE, "ag1002am_io");

    // token格式为gpio1.1
    while ((token = strsep(&pin, seq)))
    {
        memset(gpio_num, 0, sizeof(gpio_num));
        if (strncmp("1", token + 4, 1) == 0)
        {
            minor_num = simple_strtol(token + 6, NULL, 10);
            sprintf(gpio_num, "%d", GPIO1_NUM(minor_num));
        }
        else if (strncmp("2", token + 4, 1) == 0)
        {
            minor_num = simple_strtol(token + 6, NULL, 10);
            sprintf(gpio_num, "%d", GPIO2_NUM(minor_num));
        }
        else if (strncmp("3", token + 4, 1) == 0)
        {
            minor_num = simple_strtol(token + 6, NULL, 10);
            sprintf(gpio_num, "%d", GPIO3_NUM(minor_num));
        }
        else if (strncmp("4", token + 4, 1) == 0)
        {
            minor_num = simple_strtol(token + 6, NULL, 10);
            sprintf(gpio_num, "%d", GPIO4_NUM(minor_num));
        }
        else
        {
            printk("Illegal input parameter!\n");
            break;
        }

        ag1002am_io_device[i] = device_create(ag1002am_io_class, NULL, MKDEV(major, i), NULL, gpio_num);
        DEVICE_CREATE_FILE(ag1002am_io_device[i], in);
        DEVICE_CREATE_FILE(ag1002am_io_device[i], out);
        DEVICE_CREATE_FILE(ag1002am_io_device[i], direct);
        i++;
    }
    return 0;
}

static void __exit ag1002am_io_exit(void)
{
    int i;
    for (i = 0; i < PIN_MAX_COUNT; i++)
    {
        if (ag1002am_io_device[i] == NULL)
            break;
        device_unregister(ag1002am_io_device[i]);
    }

    class_destroy(ag1002am_io_class);
    unregister_chrdev(major, "ag1002am_io");
}

module_init(ag1002am_io_init);
module_exit(ag1002am_io_exit);

MODULE_AUTHOR("tkj");
MODULE_DESCRIPTION("ag1002am_io general io driver");
MODULE_LICENSE("GPL v2");