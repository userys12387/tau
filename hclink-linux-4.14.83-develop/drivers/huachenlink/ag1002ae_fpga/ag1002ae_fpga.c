/*****************************************************************************************************
 * offset 0 -- fpga version high addr RO
 * |-----------------------------------------------------|
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |--------------fpga version high----------------------|
 * 
 * offset 1 -- fpga version low addr RO
 * |-----------------------------------------------------|
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |--------------fpga version low-----------------------|
 * 
 * offset 2 -- pcb hardware version RO
 * |-----------------------------------------------------|
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |--------------pcb hardware version ------------------|
 * 
  * offset 4 -- key RO
 * |-----------------------------------------------------|
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |--------------------key register---------------------|
 * value 1 -- 1~3s
 * value 2 -- 3~5s
 * value 3 -- 5~8s
 * value 4 -- 10s or above
 * 
 * offset 5 -- led front 0
 * |-----------------------------------------------------|
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |   CLOUD   |    WIFI   |   LTE/5G  |    AI     |
 * |   00:off  |   00:off  |   00:off  |   00:off  |
 * |   01:on   |   01:on   |   01:on   |   01:on   |
 * |   10:slow |   10:slow |   10:slow |   10:slow |
 * |   11:fast |   11:fast |   11:fast |   11:fast |     
 * |--------------led front 0----------------------------|
 * 
 * offset 6 -- led front 1
 * |-----------------------------------------------------|
 * |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |           |   LTE_2   |   LTE_1   |    LTE_0  |
 * |           |   00:off  |   00:off  |   00:off  |
 * |           |   01:on   |   01:on   |   01:on   |
 * |           |   10:slow |   10:slow |   10:slow |
 * |           |   11:fast |   11:fast |   11:fast |     
 * |--------------led front 1----------------------------|
 ******************************************************************************************************/

#include "ag1002ae_fpga.h"

struct class *ag1002ae_class;
struct device *ag1002ae_device;
static ag1002ae_fpga_reg_t *ag1002ae_fpga_reg;
static ag1002ae_front_panel_led0_t *ag1002ae_front_panel_led0;
static ag1002ae_front_panel_led1_t *ag1002ae_front_panel_led1;

/*定义sysfs*/
LDTEA_SYSFS_DEFINE(fpga_version, "fpga_version")
LDTEA_SYSFS_DEFINE(pcb_version, "pcb_version")
LDTEA_SYSFS_DEFINE(key, "key")
LDTEA_SYSFS_DEFINE(ai_led, "ai_led")
LDTEA_SYSFS_DEFINE(lte_5g_led, "lte_5g_led")
LDTEA_SYSFS_DEFINE(wifi_led, "wifi_led")
LDTEA_SYSFS_DEFINE(cloud_led, "cloud_led")
LDTEA_SYSFS_DEFINE(lte0_led, "lte0_led")
LDTEA_SYSFS_DEFINE(lte1_led, "lte1_led")
LDTEA_SYSFS_DEFINE(lte2_led, "lte2_led")

/*定义fpga reg功能*/
FPGA_REG_OF_METHOD_DEFINE(fpga_version, ag1002ae_fpga_reg);
FPGA_REG_OF_METHOD_DEFINE(pcb_version, ag1002ae_fpga_reg);
FPGA_REG_OF_METHOD_DEFINE(key, ag1002ae_fpga_reg);
FPGA_REG_OF_METHOD_DEFINE(ai_led, ag1002ae_front_panel_led0);
FPGA_REG_OF_METHOD_DEFINE(lte_5g_led, ag1002ae_front_panel_led0);
FPGA_REG_OF_METHOD_DEFINE(wifi_led, ag1002ae_front_panel_led0);
FPGA_REG_OF_METHOD_DEFINE(cloud_led, ag1002ae_front_panel_led0);
FPGA_REG_OF_METHOD_DEFINE(lte0_led, ag1002ae_front_panel_led1);
FPGA_REG_OF_METHOD_DEFINE(lte1_led, ag1002ae_front_panel_led1);
FPGA_REG_OF_METHOD_DEFINE(lte2_led, ag1002ae_front_panel_led1);

static char hclink_device[20] = {0};

static int __init ag1002ae_get_device(char *line)
{
    strncpy(hclink_device, line, strlen(line));

    return 1;
}

static void fpga_led_handle(char *led_name, char *led_status)
{    
    if(!strcmp(led_name, "CLOUD"))
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, cloud_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, cloud_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, cloud_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, cloud_led, 3);
    }  
    if(!strcmp(led_name, "WIFI"))
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, wifi_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, wifi_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, wifi_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, wifi_led, 3);
    }  
    if(!strcmp(led_name, "5G") || !strcmp(led_name, "LTE"))    
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, lte_5g_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, lte_5g_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, lte_5g_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, lte_5g_led, 3);
    }  
    if(!strcmp(led_name, "AI"))    
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, ai_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, ai_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, ai_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led0, ai_led, 3);
    }  
    if(!strcmp(led_name, "5G_2") || !strcmp(led_name, "LTE_2"))  
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte2_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte2_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte2_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte2_led, 3);
    }  
    if(!strcmp(led_name, "5G_1") || !strcmp(led_name, "LTE_1"))  
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte1_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte1_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte1_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte1_led, 3);
    }  
    if(!strcmp(led_name, "5G_0") || !strcmp(led_name, "LTE_0"))  
    {
        if(!strcmp(led_status, "OFF"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte0_led, 0);
        if(!strcmp(led_status, "ON"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte0_led, 1);
        if(!strcmp(led_status, "SLOW"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte0_led, 2);
        if(!strcmp(led_status, "FAST"))  LED_CHANGE_TO_STATUS(ag1002ae_front_panel_led1, lte0_led, 3);
    }
    else
    {
        printk("[WARNING] YOU MUST ECHO RIGHT TO /dev/ag1002ae_fpga\n");
        printk("example:\n" \
               "echo \"CLOUD OFF\" > /dev/ag1002ae_fpga\n" \
               "echo \"WIFI ON\" > /dev/ag1002ae_fpga\n" \
               "echo \"5G/LTE SLOW\" > /dev/ag1002ae_fpga\n" \
               "echo \"AI FAST\" > /dev/ag1002ae_fpga\n" \
               "echo \"5G_2/LTE_2 FAST\" > /dev/ag1002ae_fpga\n" \
               "echo \"5G_1/LTE_1 SLOW\" > /dev/ag1002ae_fpga\n" \
               "echo \"5G_0/LTE_0 ON\" > /dev/ag1002ae_fpga\n" \
               );
    }
}

/*处理传入的字符串，并置led的状态*/
static void fpga_handle_string(char *strings)
{
    char led_name[10];
    char led_status[10];

    memset(led_name, 0, sizeof(led_name));
    memset(led_status, 0, sizeof(led_status));

    sscanf(strings, "%s %s", led_name, led_status);

    /*调用相关函数执行*/
    fpga_led_handle(led_name, led_status);
}

static int ag1002ae_open(struct inode *inode, struct file *file)
{	
	return 0;
}

int ag1002ae_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t ag1002ae_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
    int length;
	char get_echo_string[MAX_CMD_LENGTH];

	memset(get_echo_string, 0, sizeof(get_echo_string));
    length = strlen(buf);
	simple_write_to_buffer(get_echo_string, length, ppos, buf, count);

    printk("%s\n", get_echo_string);
	fpga_handle_string(get_echo_string);

	return count;
}

static struct file_operations ag1002ae_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   ag1002ae_open,     
	.write	=	ag1002ae_write,
	.release = 	ag1002ae_release,
};

static int __init ag1002ae_init(void)
{
    int major;

    if(strcmp(hclink_device, "ag1002ae"))
    {
        /*判断是否为ag1002ae，不是的话直接返回就行了*/
        return 0;
    }
    printk("liji says current device is ag1002ae, load fpga driver...\n");

    major = register_chrdev(0, "ag1002ae", &ag1002ae_fops);

    ag1002ae_class = class_create(THIS_MODULE, "ag1002ae");
    ag1002ae_device = device_create(ag1002ae_class, NULL, MKDEV(major, 0), NULL, "ag1002ae"); 

    ag1002ae_fpga_reg = ioremap(FPGA_BASE_ADDR, sizeof(ag1002ae_fpga_reg_t));
    ag1002ae_front_panel_led0 = (ag1002ae_front_panel_led0_t *)&(ag1002ae_fpga_reg->front_panel_led0);
    ag1002ae_front_panel_led1 = (ag1002ae_front_panel_led1_t *)&(ag1002ae_fpga_reg->front_panel_led1);
    
    printk("This is ag1002ae device FPGA driver, written by liji@huachen.link\n");

    /*现在开始创建sysfs*/
    DEVICE_CREATE_FILE(ag1002ae_device, fpga_version);
    DEVICE_CREATE_FILE(ag1002ae_device, pcb_version);
    DEVICE_CREATE_FILE(ag1002ae_device, ai_led);
    DEVICE_CREATE_FILE(ag1002ae_device, lte_5g_led);
    DEVICE_CREATE_FILE(ag1002ae_device, wifi_led);
    DEVICE_CREATE_FILE(ag1002ae_device, cloud_led);
    DEVICE_CREATE_FILE(ag1002ae_device, lte0_led);
    DEVICE_CREATE_FILE(ag1002ae_device, lte1_led);
    DEVICE_CREATE_FILE(ag1002ae_device, lte2_led);

    return 0;
}

/*使用setup后此函数就不会再使用了*/
static void __exit ag1002ae_exit(void)
{
    if(strcmp(hclink_device, "ag1002ae"))        
        iounmap(ag1002ae_fpga_reg);
    
    device_unregister(ag1002ae_device); 
	class_destroy(ag1002ae_class);	
}

__setup("device=", ag1002ae_get_device);

module_init(ag1002ae_init);
module_exit(ag1002ae_exit);

MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("ag1002ae general io driver");
MODULE_LICENSE("GPL v2");


