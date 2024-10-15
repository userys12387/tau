#include "ag1002adx_wdt.h"

static struct class *ag1002adx_wdt_class;
static struct device *ag1002adx_wdt_device;

static char hclink_device[20] = {0};

static int __init ag1002ad_get_device(char *line)
{
    strncpy(hclink_device, line, strlen(line));

    return 1;
}

ssize_t ag1002adx_wdt_show_wdt_switch(struct device *dev, struct device_attribute *attr, char *buf)
{
    int value = 0;
    value = gpio_get_value(WDT_SWITCH);
    return sprintf(buf, "%d\n", value);
}

ssize_t ag1002adx_wdt_store_wdt_switch(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int value = 0;
    value = buf[0] - '0';
    if (value > 0x1 || value < 0x0)
    {
        printk("[NOTICE]: module ag1002adx_wdt, your input out of range");
        return -EINVAL;
    }
    gpio_set_value(WDT_SWITCH, value);
    return count;
}

AG1002ADX_WDT_SYSFS_DEFINE(wdt_switch, "wdt_switch");

static int ag1002adx_wdt_open(struct inode *inode, struct file *file)
{
    return 0;
}

int ag1002adx_wdt_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t ag1002adx_wdt_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int value = 0;
    unsigned char val[3];
    if (copy_from_user(val, buf, count))
        return -EINVAL;
    value = val[0] - '0';
    /* 写0开启 写1关闭 */
    if (value > 0x1 || value < 0x0)
    {
        printk("[NOTICE]: module ag1002adx_wdt, your input out of range");
        return -EINVAL;
    }
    gpio_set_value(WDT_SWITCH, value);
    return count;
}

static struct file_operations ag1002adx_wdt_fops = {
    .owner = THIS_MODULE,
    .open = ag1002adx_wdt_open,
    .write = ag1002adx_wdt_write,
    .release = ag1002adx_wdt_release,
};

static int __init ag1002adx_wdt_init(void)
{
    int major;
    if (strcmp(hclink_device, "ag1002ad"))
    {
        printk("ag1002adx: %s.\n", hclink_device);
        return 0;
    }
    major = register_chrdev(0, "ag1002adx_wdt", &ag1002adx_wdt_fops);

    ag1002adx_wdt_class = class_create(THIS_MODULE, "ag1002adx_wdt");
    ag1002adx_wdt_device = device_create(ag1002adx_wdt_class, NULL, MKDEV(major, 0), NULL, "ag1002adx_wdt");

    printk("This is ag1002adx wdt device driver.\n");

    /*默认情况下看门狗关闭*/
    gpio_direction_output(WDT_SWITCH, 1);

    DEVICE_CREATE_FILE(ag1002adx_wdt_device, wdt_switch);
    return 0;
}

static void __exit ag1002adx_wdt_exit(void)
{
    if (!strcmp(hclink_device, "ag1002ad"))
    {
        device_unregister(ag1002adx_wdt_device);
        class_destroy(ag1002adx_wdt_class);
    }
}

__setup("device=", ag1002ad_get_device);

module_init(ag1002adx_wdt_init);
module_exit(ag1002adx_wdt_exit);

MODULE_DESCRIPTION("ag1002adx_wdt_gpio_driver");
MODULE_LICENSE("GPL v2");