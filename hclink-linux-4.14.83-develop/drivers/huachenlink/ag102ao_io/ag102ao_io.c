#ifndef __AG102AO_IO_H__
#define __AG102AO_IO_H__
#endif
#include<linux/module.h>
#include<linux/init.h>

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <linux/kernel.h>       
#include <linux/platform_device.h>
#include <linux/cdev.h>         
#include <linux/types.h>        
#include <linux/errno.h>       
#include <linux/string.h>
#include <linux/ctype.h>

#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#if defined(MODULE) || defined(CONFIG_HOTPLUG)
#define __devexit_p(x) x
#else
#define __devexit_p(x) NULL
#endif


#define GPIO1_NUM(x) (x + 480)
#define GPIO2_NUM(x) (x + 448)
#define GPIO3_NUM(x) (x + 416)
#define GPIO4_NUM(x) (x + 384)

#define GPIO_WDT_RST GPIO1_NUM(30)
#define GPIO_5G1_RST GPIO1_NUM(31)
#define GPIO_5G2_RST GPIO2_NUM(12)

#define IO_SYSFS_DEFINE(sysfs_node, filename) \
    ssize_t io_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf); \
    ssize_t io_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count); \
    \
    struct device_attribute io_attr_##sysfs_node = { \
        .attr = { \
            .name = filename, \
            .mode = 0777, \
        }, \
	    .show  = io_show_##sysfs_node, \
        .store = io_store_##sysfs_node,\
    };

#define DEVICE_CREATE_FILE(dev, sysfs_node) \
    if (device_create_file(dev, &io_attr_##sysfs_node)) \
		dev_err(dev, "Unable to create sysfs entry: '%s'\n", \
				io_attr_##sysfs_node.attr.name);

// example: IO_OF_METHOD_DEFINE(in1, IN1) IN1为gpio的编号
#define IO_OF_METHOD_DEFINE(sysfs_node, lable) \
ssize_t io_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf) \
{ \
    int value = 0;     \
    value = gpio_get_value(lable); \
    return sprintf(buf, "%d\n", value); \
}\
ssize_t io_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) \
{     \
    int value = 0;\
    value = buf[0] - '0'; \
    if(value > 0x1 || value < 0x0) \
    { \
        printk("[NOTICE]: module io, your input out of range"); \
        return 0; \
    } \
    gpio_set_value(lable, value);\
    return count; \
}


struct class *hclink_io_class;
struct device *hclink_io_device;

IO_OF_METHOD_DEFINE(wdt_rst, GPIO_WDT_RST);
IO_OF_METHOD_DEFINE(5g1_rst, GPIO_5G1_RST);
IO_OF_METHOD_DEFINE(5g2_rst, GPIO_5G2_RST);

IO_SYSFS_DEFINE(wdt_rst, "wdt_rst");
IO_SYSFS_DEFINE(5g1_rst, "5g1_rst");
IO_SYSFS_DEFINE(5g2_rst, "5g2_rst");

static int io_open(struct inode *inode, struct file *file)
{	
	return 0;
}

int io_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t io_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
    return 0;
}

static struct file_operations io_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   io_open,     
	.write	=	io_write,
	.release = 	io_release,
};

static int __init hclink_io_init(void)
{
    int major;

    printk("liji says current device is ag102ao, load io driver...\n");

    major = register_chrdev(0, "hclink_io", &io_fops);

    hclink_io_class = class_create(THIS_MODULE, "hclink_io");
    hclink_io_device = device_create(hclink_io_class, NULL, MKDEV(major, 0), NULL, "hclink_io"); 

    printk("This is ag102ao io device driver, written by liji@huachen.link\n");

    /*对这四个gpio进行初始化*/    
    gpio_direction_output(GPIO_WDT_RST, 1);
    gpio_direction_output(GPIO_5G1_RST, 1);
    gpio_direction_output(GPIO_5G2_RST, 1);
    /*现在开始创建sysfs*/
    DEVICE_CREATE_FILE(hclink_io_device, wdt_rst);
    DEVICE_CREATE_FILE(hclink_io_device, 5g1_rst);
    DEVICE_CREATE_FILE(hclink_io_device, 5g2_rst);
    

    return 0;
}

static void __exit hclink_io_exit(void)
{  
    device_unregister(hclink_io_device); 
	class_destroy(hclink_io_class);
   	
}

module_init(hclink_io_init);
module_exit(hclink_io_exit);

MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("io general io driver");
MODULE_LICENSE("GPL v2");



