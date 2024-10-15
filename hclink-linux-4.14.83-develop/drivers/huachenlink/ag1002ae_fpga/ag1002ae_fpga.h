#ifndef __AG1002AE_FPGA__
#define __AG1002AE_FPGA__

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

#define FPGA_BASE_ADDR  0x7fb00000

typedef struct ag1002ae_fpga_reg_s{
    uint16_t fpga_version;         /*0-1 --- fpga版本号*/
    uint8_t pcb_version;            /*2 --- 硬件版本号*/
    uint8_t fpga_null;          /*3 --- null*/
    uint8_t key;                /*4 ---keys*/    
    uint8_t front_panel_led0;   /*5 --- 前面板灯0*/
    uint8_t front_panel_led1;   /*6 --- 前面板灯1*/
}ag1002ae_fpga_reg_t;

#define FPGA_REG_OF_METHOD_DEFINE(sysfs_node, reg_struct) \
ssize_t ag1002ae_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf) \
{ \
    int value = 0; \
    value = reg_struct->sysfs_node; \
    return sprintf(buf, "%d\n", value); \
} \
ssize_t ag1002ae_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) \
{ \
    int value = 0; \
    value = buf[0] - '0'; \
    if(value > 0xff || value < 0x0) \
    { \
        printk("[NOTICE]: module ldtea100, your input out of range"); \
        return 0; \
    } \
    \
    reg_struct->sysfs_node = value; \
    return count; \
}

#define LDTEA_SYSFS_DEFINE(sysfs_node, filename) \
    ssize_t ag1002ae_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf); \
    ssize_t ag1002ae_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count); \
    \
    struct device_attribute ag1002ae_attr_##sysfs_node = { \
        .attr = { \
            .name = filename, \
            .mode = 0777, \
        }, \
	    .show  = ag1002ae_show_##sysfs_node, \
        .store = ag1002ae_store_##sysfs_node,\
    };

#define DEVICE_CREATE_FILE(dev, sysfs_node) \
    if (device_create_file(dev, &ag1002ae_attr_##sysfs_node)) \
		dev_err(dev, "Unable to create sysfs entry: '%s'\n", \
				ag1002ae_attr_##sysfs_node.attr.name);

typedef struct front_panel_led0_s{
    uint8_t ai_led:2;
    uint8_t lte_5g_led:2;
    uint8_t wifi_led:2;
    uint8_t cloud_led:2;
}ag1002ae_front_panel_led0_t;

typedef struct front_panel_led1_s{
    uint8_t lte0_led:2;
    uint8_t lte1_led:2;
    uint8_t lte2_led:2;
    uint8_t null:2;
}ag1002ae_front_panel_led1_t;


#define LED_CHANGE_TO_STATUS(a, b, c) a->b = c
#define MAX_CMD_LENGTH      2000











#endif