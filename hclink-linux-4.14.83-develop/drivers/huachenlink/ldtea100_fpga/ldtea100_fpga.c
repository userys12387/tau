#include "ldtea100_fpga.h"

struct class *ldtea_100_class;
struct device *ldtea_100_device;
static ldtea_100_fpga_reg_t *ldtea_100_fpga_reg;

static ldtea_100_front_panel_led0_reg_t *ldtea_100_front_panel_led0_reg;
static ldtea_100_front_panel_led1_reg_t *ldtea_100_front_panel_led1_reg;
static ldtea_100_front_panel_led2_reg_t *ldtea_100_front_panel_led2_reg;
static ldtea_100_fx_led_yx_a_reg_t *ldtea_100_fx_led_yx_a_reg;
static ldtea_100_fx_led_yx_b_reg_t *ldtea_100_fx_led_yx_b_reg;
static ldtea_100_ifc_par_yk_reg_t *ldtea_100_ifc_par_yk_reg;
static ldtea_100_fx_led4_uart2_en_reg_t *ldtea_100_fx_led4_uart2_en_reg;
static ldtea_100_fx_led5_uart3_en_reg_t *ldtea_100_fx_led5_uart3_en_reg;
static ldtea_100_fx_led6_uart5_dir_reg_t *ldtea_100_fx_led6_uart5_dir_reg;
static ldtea_100_fx_led7_uart6_dir_reg_t *ldtea_100_fx_led7_uart6_dir_reg;
static ldtea_100_usb_reset_reg_t *ldtea_100_usb_reset_reg;
static ldtea_100_front_panel_led3_reg_t *ldtea_100_front_panel_led3_reg;

/*按照fpga说明定义的sysfs*/
LDTEA_SYSFS_DEFINE(fpga_version, "fpga_version")
LDTEA_SYSFS_DEFINE(pcb_version, "pcb_version")

LDTEA_SYSFS_DEFINE(cloud_led, "cloud_led")
LDTEA_SYSFS_DEFINE(lora_led, "lora_led")
LDTEA_SYSFS_DEFINE(wifi_led, "wifi_led")

LDTEA_SYSFS_DEFINE(lte0_led, "lte0_led")
LDTEA_SYSFS_DEFINE(lte1_led, "lte1_led")
LDTEA_SYSFS_DEFINE(lte2_led, "lte2_led")

LDTEA_SYSFS_DEFINE(ext_485_led_1, "ext_485_led_1")
LDTEA_SYSFS_DEFINE(ext_485_led_2, "ext_485_led_2")
LDTEA_SYSFS_DEFINE(ext_485_led_3, "ext_485_led_3")
LDTEA_SYSFS_DEFINE(ext_ai_led, "ext_ai_led")

LDTEA_SYSFS_DEFINE(yxa_yx1, "yxa_yx1")
LDTEA_SYSFS_DEFINE(yxa_yx2, "yxa_yx2")
LDTEA_SYSFS_DEFINE(yxa_yx3, "yxa_yx3")
LDTEA_SYSFS_DEFINE(yxa_yx4, "yxa_yx4")
LDTEA_SYSFS_DEFINE(yxb_yx1, "yxb_yx1")
LDTEA_SYSFS_DEFINE(ifc_par_yk_yx1, "ifc_par_yk_yx1")
LDTEA_SYSFS_DEFINE(ifc_par_yk_yx2, "ifc_par_yk_yx2")
LDTEA_SYSFS_DEFINE(fx_led4_uart2_en, "fx_led4_uart2_en")
LDTEA_SYSFS_DEFINE(fx_led5_uart3_en, "fx_led5_uart3_en")
LDTEA_SYSFS_DEFINE(fx_led6_uart5_dir, "fx_led6_uart5_dir")
LDTEA_SYSFS_DEFINE(fx_led7_uart6, "fx_led7_uart6")
LDTEA_SYSFS_DEFINE(usb_to_tf_rst, "usb_to_tf_rst")
LDTEA_SYSFS_DEFINE(securechip_rst, "securechip_rst")
LDTEA_SYSFS_DEFINE(usb_to_uart_rst, "usb_to_uart_rst")

LDTEA_SYSFS_DEFINE(lte_5g_led, "lte_5g_led")
LDTEA_SYSFS_DEFINE(ext_485_led_0, "ext_485_led_0")
LDTEA_SYSFS_DEFINE(ext_zigbee_led, "ext_zigbee_led")
LDTEA_SYSFS_DEFINE(ext_ble_led, "ext_ble_led")

/*定义fpga reg功能*/
FPGA_REG_OF_METHOD_DEFINE(fpga_version, ldtea_100_fpga_reg);
FPGA_REG_OF_METHOD_DEFINE(pcb_version, ldtea_100_fpga_reg);

FPGA_REG_OF_METHOD_DEFINE(cloud_led, ldtea_100_front_panel_led0_reg);
FPGA_REG_OF_METHOD_DEFINE(lora_led, ldtea_100_front_panel_led0_reg);
FPGA_REG_OF_METHOD_DEFINE(wifi_led, ldtea_100_front_panel_led0_reg);

FPGA_REG_OF_METHOD_DEFINE(lte0_led, ldtea_100_front_panel_led1_reg);
FPGA_REG_OF_METHOD_DEFINE(lte1_led, ldtea_100_front_panel_led1_reg);
FPGA_REG_OF_METHOD_DEFINE(lte2_led, ldtea_100_front_panel_led1_reg);

FPGA_REG_OF_METHOD_DEFINE(ext_485_led_1, ldtea_100_front_panel_led2_reg);
FPGA_REG_OF_METHOD_DEFINE(ext_485_led_2, ldtea_100_front_panel_led2_reg);
FPGA_REG_OF_METHOD_DEFINE(ext_485_led_3, ldtea_100_front_panel_led2_reg);
FPGA_REG_OF_METHOD_DEFINE(ext_ai_led, ldtea_100_front_panel_led2_reg);

FPGA_REG_OF_METHOD_DEFINE(yxa_yx1, ldtea_100_fx_led_yx_a_reg);
FPGA_REG_OF_METHOD_DEFINE(yxa_yx2, ldtea_100_fx_led_yx_a_reg);
FPGA_REG_OF_METHOD_DEFINE(yxa_yx3, ldtea_100_fx_led_yx_a_reg);
FPGA_REG_OF_METHOD_DEFINE(yxa_yx4, ldtea_100_fx_led_yx_a_reg);
FPGA_REG_OF_METHOD_DEFINE(yxb_yx1, ldtea_100_fx_led_yx_b_reg);
FPGA_REG_OF_METHOD_DEFINE(ifc_par_yk_yx1, ldtea_100_ifc_par_yk_reg);
FPGA_REG_OF_METHOD_DEFINE(ifc_par_yk_yx2, ldtea_100_ifc_par_yk_reg);
FPGA_REG_OF_METHOD_DEFINE(fx_led4_uart2_en, ldtea_100_fx_led4_uart2_en_reg);
FPGA_REG_OF_METHOD_DEFINE(fx_led5_uart3_en, ldtea_100_fx_led5_uart3_en_reg);
FPGA_REG_OF_METHOD_DEFINE(fx_led6_uart5_dir, ldtea_100_fx_led6_uart5_dir_reg);
FPGA_REG_OF_METHOD_DEFINE(fx_led7_uart6, ldtea_100_fx_led7_uart6_dir_reg);
FPGA_REG_OF_METHOD_DEFINE(usb_to_tf_rst, ldtea_100_usb_reset_reg);
FPGA_REG_OF_METHOD_DEFINE(securechip_rst, ldtea_100_usb_reset_reg);
FPGA_REG_OF_METHOD_DEFINE(usb_to_uart_rst, ldtea_100_usb_reset_reg);

FPGA_REG_OF_METHOD_DEFINE(lte_5g_led, ldtea_100_front_panel_led3_reg)
FPGA_REG_OF_METHOD_DEFINE(ext_485_led_0, ldtea_100_front_panel_led3_reg)
FPGA_REG_OF_METHOD_DEFINE(ext_zigbee_led, ldtea_100_front_panel_led3_reg)
FPGA_REG_OF_METHOD_DEFINE(ext_ble_led, ldtea_100_front_panel_led3_reg)

static int ldtea_100_open(struct inode *inode, struct file *file)
{	
	return 0;
}

int ldtea_100_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t ldtea_100_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
    return 0;
}

static struct file_operations ldtea_100_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   ldtea_100_open,     
	.write	=	ldtea_100_write,
	.release = 	ldtea_100_release,
};

static int __init ldtea_100_init(void)
{
    int major;
    major = register_chrdev(0, "ldtea_100", &ldtea_100_fops);

    ldtea_100_class = class_create(THIS_MODULE, "ldtea100");
    ldtea_100_device = device_create(ldtea_100_class, NULL, MKDEV(major, 0), NULL, "ldtea_100"); 

    ldtea_100_fpga_reg = ioremap(FPGA_BASE_ADDR, sizeof(ldtea_100_fpga_reg_t));
    ldtea_100_front_panel_led0_reg = (ldtea_100_front_panel_led0_reg_t *)&(ldtea_100_fpga_reg->front_panel_led0);
    ldtea_100_front_panel_led1_reg = (ldtea_100_front_panel_led1_reg_t *)&(ldtea_100_fpga_reg->front_panel_led1);
    ldtea_100_front_panel_led2_reg = (ldtea_100_front_panel_led2_reg_t *)&(ldtea_100_fpga_reg->front_panel_led2);
    ldtea_100_fx_led_yx_a_reg = (ldtea_100_fx_led_yx_a_reg_t *)&(ldtea_100_fpga_reg->fx_led_yx_a);
    ldtea_100_fx_led_yx_b_reg = (ldtea_100_fx_led_yx_b_reg_t *)&(ldtea_100_fpga_reg->fx_led_yx_b);
    ldtea_100_ifc_par_yk_reg = (ldtea_100_ifc_par_yk_reg_t *)&(ldtea_100_fpga_reg->ifc_par_yk);
    ldtea_100_fx_led4_uart2_en_reg = (ldtea_100_fx_led4_uart2_en_reg_t *)&(ldtea_100_fpga_reg->fx_led4_uart2_en);
    ldtea_100_fx_led5_uart3_en_reg = (ldtea_100_fx_led5_uart3_en_reg_t *)&(ldtea_100_fpga_reg->fx_led5_uart3_en);
    ldtea_100_fx_led6_uart5_dir_reg = (ldtea_100_fx_led6_uart5_dir_reg_t *)&(ldtea_100_fpga_reg->fx_led6_uart5_dir);
    ldtea_100_fx_led7_uart6_dir_reg = (ldtea_100_fx_led7_uart6_dir_reg_t *)&(ldtea_100_fpga_reg->fx_led7_uart6_dir);
    ldtea_100_usb_reset_reg = (ldtea_100_usb_reset_reg_t *)&(ldtea_100_fpga_reg->usb_reset);
    ldtea_100_front_panel_led3_reg = (ldtea_100_front_panel_led3_reg_t *)&(ldtea_100_fpga_reg->front_panel_led3);

    printk("This is LDTEA100 device FPGA driver, written by liji@huachen.link\n");

    /*现在开始创建sysfs*/
    DEVICE_CREATE_FILE(ldtea_100_device, fpga_version);
    DEVICE_CREATE_FILE(ldtea_100_device, pcb_version);

    DEVICE_CREATE_FILE(ldtea_100_device, cloud_led);
    DEVICE_CREATE_FILE(ldtea_100_device, lora_led);
    DEVICE_CREATE_FILE(ldtea_100_device, wifi_led);
    
    DEVICE_CREATE_FILE(ldtea_100_device, lte0_led);
    DEVICE_CREATE_FILE(ldtea_100_device, lte1_led);
    DEVICE_CREATE_FILE(ldtea_100_device, lte2_led);
    
    DEVICE_CREATE_FILE(ldtea_100_device, ext_485_led_1);
    DEVICE_CREATE_FILE(ldtea_100_device, ext_485_led_2);
    DEVICE_CREATE_FILE(ldtea_100_device, ext_485_led_3);
    DEVICE_CREATE_FILE(ldtea_100_device, ext_ai_led);

    DEVICE_CREATE_FILE(ldtea_100_device, yxa_yx1);
    DEVICE_CREATE_FILE(ldtea_100_device, yxa_yx2);
    DEVICE_CREATE_FILE(ldtea_100_device, yxa_yx3);
    DEVICE_CREATE_FILE(ldtea_100_device, yxa_yx4);
    DEVICE_CREATE_FILE(ldtea_100_device, yxb_yx1);
    DEVICE_CREATE_FILE(ldtea_100_device, ifc_par_yk_yx1);
    DEVICE_CREATE_FILE(ldtea_100_device, ifc_par_yk_yx2);
    DEVICE_CREATE_FILE(ldtea_100_device, fx_led4_uart2_en);
    DEVICE_CREATE_FILE(ldtea_100_device, fx_led5_uart3_en);
    DEVICE_CREATE_FILE(ldtea_100_device, fx_led6_uart5_dir);
    DEVICE_CREATE_FILE(ldtea_100_device, fx_led7_uart6);
    DEVICE_CREATE_FILE(ldtea_100_device, usb_to_tf_rst);
    DEVICE_CREATE_FILE(ldtea_100_device, securechip_rst);
    DEVICE_CREATE_FILE(ldtea_100_device, usb_to_uart_rst);

    DEVICE_CREATE_FILE(ldtea_100_device, lte_5g_led);
    DEVICE_CREATE_FILE(ldtea_100_device, ext_485_led_0);
    DEVICE_CREATE_FILE(ldtea_100_device, ext_zigbee_led);
    DEVICE_CREATE_FILE(ldtea_100_device, ext_ble_led);

    return 0;
}

static void __exit ldtea_100_exit(void)
{
    iounmap(ldtea_100_fpga_reg);
    device_unregister(ldtea_100_device); 
	class_destroy(ldtea_100_class);	
}

module_init(ldtea_100_init);
module_exit(ldtea_100_exit);

MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("ldtea_100 general io driver");
MODULE_LICENSE("GPL v2");




