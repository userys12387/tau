#ifndef __LDTEA_H__
#define __LDTEA_H__

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

/*描述FPGA上的地址映射*/
typedef struct ldtea_100_fpga_reg_s{
    uint16_t fpga_version;         /*0-1 --- fpga版本号*/
    uint8_t pcb_version;            /*2 --- 硬件版本号*/
    uint8_t fpga_null;          /*3 --- null*/
    uint8_t front_panel_led2;   /*4 --- 485 leds*/    
    uint8_t front_panel_led0;   /*5 --- 前面板灯0*/
    uint8_t front_panel_led1;   /*6 --- 前面板灯1*/
    uint8_t fpga_null1[9];      /*7-9 --- null*/
    uint8_t fx_led_yx_a;         /*16 --- yx*/
    uint8_t fx_led_yx_b;         /*17 --- yx*/
    uint8_t ifc_par_yk;         /*18*/
    uint8_t fx_led4_uart2_en;   /*19*/
    uint8_t fx_led5_uart3_en;   /*20*/
    uint8_t fx_led6_uart5_dir;   /*21*/
    uint8_t fx_led7_uart6_dir;   /*22*/
    uint8_t usb_reset;   /*23*/
    uint8_t front_panel_led3;   /*24*/   
}ldtea_100_fpga_reg_t;

typedef struct ldtea_100_front_panel_led0_reg_s{
    uint8_t cloud_led:2;
    uint8_t lora_led:2;
    uint8_t wifi_led:2;
    uint8_t null:2;
}ldtea_100_front_panel_led0_reg_t;

typedef struct ldtea_100_front_panel_led1_reg_s{
    uint8_t lte0_led:2;
    uint8_t lte1_led:2;
    uint8_t lte2_led:2;
    uint8_t null:2;
}ldtea_100_front_panel_led1_reg_t;

typedef struct ldtea_100_front_panel_led2_reg_s{
    uint8_t ext_485_led_1:2;
    uint8_t ext_485_led_2:2;
    uint8_t ext_485_led_3:2;
    uint8_t ext_ai_led:2;
}ldtea_100_front_panel_led2_reg_t;

typedef struct ldtea_100_fx_led_yx_a_reg_s{
    uint8_t yxa_yx1:1;
    uint8_t yxa_yx2:1;
    uint8_t yxa_yx3:1;
    uint8_t yxa_yx4:1;
    uint8_t null:4;
}ldtea_100_fx_led_yx_a_reg_t;

typedef struct ldtea_100_fx_led_yx_b_reg_s{
    uint8_t yxb_yx1:1;
    uint8_t null:7;
}ldtea_100_fx_led_yx_b_reg_t;

typedef struct ldtea_100_ifc_par_yk_reg_s{
    uint8_t ifc_par_yk_yx1:1;
    uint8_t ifc_par_yk_yx2:1;
    uint8_t null:6;
}ldtea_100_ifc_par_yk_reg_t;

typedef struct ldtea_100_fx_led4_uart2_en_reg_s{
    uint8_t fx_led4_uart2_en:1;
    uint8_t null:7;
}ldtea_100_fx_led4_uart2_en_reg_t;

typedef struct ldtea_100_fx_led5_uart3_en_reg_s{
    uint8_t fx_led5_uart3_en:1;
    uint8_t null:7;
}ldtea_100_fx_led5_uart3_en_reg_t;

typedef struct ldtea_100_fx_led6_uart5_dir_reg_s{
    uint8_t fx_led6_uart5_dir:1;
    uint8_t null:7;
}ldtea_100_fx_led6_uart5_dir_reg_t;

typedef struct ldtea_100_fx_led7_uart6_dir_reg_s{
    uint8_t fx_led7_uart6:1;
    uint8_t null:7;
}ldtea_100_fx_led7_uart6_dir_reg_t;

typedef struct ldtea_100_usb_reset_reg_s{
    uint8_t usb_to_tf_rst:1;
    uint8_t securechip_rst:1;
    uint8_t usb_to_uart_rst:1;    
    uint8_t null:5;
}ldtea_100_usb_reset_reg_t;

typedef struct ldtea_100_front_panel_led3_reg_s{
    uint8_t lte_5g_led:2;
    uint8_t ext_485_led_0:2;
    uint8_t ext_zigbee_led:2;
    uint8_t ext_ble_led:2;
}ldtea_100_front_panel_led3_reg_t;

extern struct class *ldtea_100_class;
extern struct device *ldtea_100_device;

#define FPGA_REG_OF_METHOD_DEFINE(sysfs_node, reg_struct) \
ssize_t ldtea_100_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf) \
{ \
    int value = 0; \
    value = reg_struct->sysfs_node; \
    return sprintf(buf, "%d\n", value); \
} \
ssize_t ldtea_100_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) \
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
    ssize_t ldtea_100_show_##sysfs_node(struct device *dev, struct device_attribute *attr, char *buf); \
    ssize_t ldtea_100_store_##sysfs_node(struct device *dev, struct device_attribute *attr, const char *buf, size_t count); \
    \
    struct device_attribute ldtea_100_attr_##sysfs_node = { \
        .attr = { \
            .name = filename, \
            .mode = 0777, \
        }, \
	    .show  = ldtea_100_show_##sysfs_node, \
        .store = ldtea_100_store_##sysfs_node,\
    };

#define DEVICE_CREATE_FILE(dev, sysfs_node) \
    if (device_create_file(dev, &ldtea_100_attr_##sysfs_node)) \
		dev_err(dev, "Unable to create sysfs entry: '%s'\n", \
				ldtea_100_attr_##sysfs_node.attr.name);





#endif