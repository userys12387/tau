#ifndef __AG1002AD_IO_H__
#define __AG1002AD_IO_H__

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

#define IN1     GPIO1_NUM(24)
#define OUT1    GPIO1_NUM(26)
#define IN2     GPIO1_NUM(23)
#define OUT2    GPIO1_NUM(25)


#define AG1002AD_IO_OF_METHOD_DEFINE(sysfs_node, reg_struct) \
ssize_t ag1002ad_io_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf) \
{ \
    int value = 0; \
    value = reg_struct->sysfs_node; \
    gpio_direction_output(); \
    return sprintf(buf, "%d\n", value); \
} \
ssize_t ag1002ad_io_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) \
{ \
    int value = 0; \
    value = buf[0] - '0'; \
    if(value > 0x1 || value < 0x0) \
    { \
        printk("[NOTICE]: module ag1002ad_io, your input out of range"); \
        return 0; \
    } \
    \
    reg_struct->sysfs_node = value; \
    return count; \
}

#define AG1002AD_IO_SYSFS_DEFINE(sysfs_node, filename) \
    ssize_t ag1002ad_io_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf); \
    ssize_t ag1002ad_io_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count); \
    \
    struct device_attribute ag1002ad_io_attr_##sysfs_node = { \
        .attr = { \
            .name = filename, \
            .mode = 0777, \
        }, \
	    .show  = ag1002ad_io_show_##sysfs_node, \
        .store = ag1002ad_io_store_##sysfs_node,\
    };

#define DEVICE_CREATE_FILE(dev, sysfs_node) \
    if (device_create_file(dev, &ag1002ad_io_attr_##sysfs_node)) \
		dev_err(dev, "Unable to create sysfs entry: '%s'\n", \
				ag1002ad_io_attr_##sysfs_node.attr.name);




































#endif




