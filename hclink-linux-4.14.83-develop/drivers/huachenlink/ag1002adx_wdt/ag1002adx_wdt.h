#ifndef __AG1002ADX_WDT_H__
#define __AG1002ADX_WDT_H__

#include <linux/module.h>
#include <linux/init.h>

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

#define WDT_SWITCH GPIO1_NUM(30)


#define AG1002ADX_WDT_SYSFS_DEFINE(sysfs_node, filename)                                                                        \
    ssize_t ag1002adx_wdt_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf);                      \
    ssize_t  ag1002adx_wdt_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count); \
                                                                                                                              \
    struct device_attribute  ag1002adx_wdt_attr_##sysfs_node = {                                                                 \
        .attr = {                                                                                                             \
            .name = filename,                                                                                                 \
            .mode = 0777,                                                                                                     \
        },                                                                                                                    \
        .show =  ag1002adx_wdt_show_##sysfs_node,                                                                                \
        .store =  ag1002adx_wdt_store_##sysfs_node,                                                                              \
    };

#define DEVICE_CREATE_FILE(dev, sysfs_node)                      \
    if (device_create_file(dev, & ag1002adx_wdt_attr_##sysfs_node)) \
        dev_err(dev, "Unable to create sysfs entry: '%s'\n",     \
                 ag1002adx_wdt_attr_##sysfs_node.attr.name);

#endif
