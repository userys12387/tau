/*
* 对于普通gpio的驱动，本来是放到了rc.local判断设备加载，但是APP同事竟然会清理掉我的加载过程。
* 所以自己定义了设备树的格式，然后使用platform driver + char driver + sysfs来实现这个功能。
* 这样的话针对不同设备树中的定义，实现模块的自动判断及加载，也不会影响其它设备。
* 由于时间太过仓促，对细节考虑或许有不周全的地方，对于代码的整理也不是很好，留着以后再继续完善吧。
* 
* AG1002AD需求:有两组IN1/OUT1 IN2/OUT2，需要提供统一的接口管理。
* IN1 : GPIO1_24
* OUT1 : GPIO1_26
* IN2 : GPIO1_23
* OUT2 : GPIO1_25
* 
* 2021/5/26 <liji@huachen.link>
*/
#include "ag1002ad_io.h"

struct class *ag1002ad_io_class;
struct device *ag1002ad_io_device;

static char hclink_device[20] = {0};

static int __init ag1002ad_get_device(char *line)
{
    strncpy(hclink_device, line, strlen(line));

    return 1;
}

ssize_t ag1002ad_io_show_in1(struct device *dev, struct device_attribute *attr, char *buf) 
{ 
    int value = 0;     
    value = gpio_get_value(IN1); 
    return sprintf(buf, "%d\n", value); 
} 
ssize_t ag1002ad_io_store_in1(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) 
{      
    return count; 
}

ssize_t ag1002ad_io_show_out1(struct device *dev, struct device_attribute *attr, char *buf) 
{ 
    int value = 0;     
    value = gpio_get_value(OUT1); 
    return sprintf(buf, "%d\n", value); 
} 
ssize_t ag1002ad_io_store_out1(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) 
{     
    int value = 0;
    value = buf[0] - '0'; 
    if(value > 0x1 || value < 0x0) 
    { 
        printk("[NOTICE]: module ag1002ad_io, your input out of range"); 
        return 0; 
    } 
    gpio_set_value(OUT1, value);
    return count; 
}

ssize_t ag1002ad_io_show_in2(struct device *dev, struct device_attribute *attr, char *buf) 
{ 
    int value = 0;     
    value = gpio_get_value(IN2); 
    return sprintf(buf, "%d\n", value); 
} 
ssize_t ag1002ad_io_store_in2(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) 
{      
    return count; 
}

ssize_t ag1002ad_io_show_out2(struct device *dev, struct device_attribute *attr, char *buf) 
{ 
    int value = 0;     
    value = gpio_get_value(OUT2); 
    return sprintf(buf, "%d\n", value); 
} 
ssize_t ag1002ad_io_store_out2(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) 
{     
    int value = 0;
    value = buf[0] - '0'; 
    if(value > 0x1 || value < 0x0) 
    { 
        printk("[NOTICE]: module ag1002ad_io, your input out of range"); 
        return 0; 
    } 
    gpio_set_value(OUT2, value);
    return count; 
}

AG1002AD_IO_SYSFS_DEFINE(in1, "in1");
AG1002AD_IO_SYSFS_DEFINE(out1, "out1");
AG1002AD_IO_SYSFS_DEFINE(in2, "in2");
AG1002AD_IO_SYSFS_DEFINE(out2, "out2");

static int ag1002ad_io_open(struct inode *inode, struct file *file)
{	
	return 0;
}

int ag1002ad_io_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t ag1002ad_io_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
    return 0;
}

static struct file_operations ag1002ad_io_fops = {
    .owner  =   THIS_MODULE,    
    .open   =   ag1002ad_io_open,     
	.write	=	ag1002ad_io_write,
	.release = 	ag1002ad_io_release,
};

static int __init ag1002ad_io_init(void)
{
    int major;

    if(strcmp(hclink_device, "ag1002ad"))
    {
        /*判断是否为ag1002ae，不是的话直接返回就行了*/
        return 0;
    }
    printk("liji says current device is ag1002ad, load io driver...\n");

    major = register_chrdev(0, "ag1002ad_io", &ag1002ad_io_fops);

    ag1002ad_io_class = class_create(THIS_MODULE, "ag1002ad_io");
    ag1002ad_io_device = device_create(ag1002ad_io_class, NULL, MKDEV(major, 0), NULL, "ag1002ad_io"); 

    printk("This is ag1002ad io device driver, written by liji@huachen.link\n");

    /*对这四个gpio进行初始化*/
    gpio_direction_input(IN1);
    gpio_direction_input(IN2);
    gpio_direction_output(OUT1, 0);
    gpio_direction_output(OUT2, 0);
    /*现在开始创建sysfs*/
    DEVICE_CREATE_FILE(ag1002ad_io_device, in1);
    DEVICE_CREATE_FILE(ag1002ad_io_device, out1);
    DEVICE_CREATE_FILE(ag1002ad_io_device, in2);
    DEVICE_CREATE_FILE(ag1002ad_io_device, out2);
    

    return 0;
}

static void __exit ag1002ad_io_exit(void)
{    
    if(!strcmp(hclink_device, "ag1002ad"))
    {
        device_unregister(ag1002ad_io_device); 
	    class_destroy(ag1002ad_io_class);
    }    	
}

__setup("device=", ag1002ad_get_device);

module_init(ag1002ad_io_init);
module_exit(ag1002ad_io_exit);

MODULE_AUTHOR("liji");
MODULE_DESCRIPTION("ag1002ad_io general io driver");
MODULE_LICENSE("GPL v2");



