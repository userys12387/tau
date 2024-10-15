#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/gpio.h>
#include <linux/gpio/machine.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/uaccess.h>
#include <asm/io.h>

#define SC1161Y_CNT 1
#define CLASS_NAME "sc1161y_class"
#define SC1161Y_NAME "sc1161y"
#define FAULT_CHANCE 3
#define RECV_BUF_SIZE 128
#define SEND_BUF_SIZE 128

#define Tcsl_us 20
#define Tcsd_us 3
#define Tcsh_us 10
#define Tclk_ns 50
#define Tb_us 1.2
#define Tq_us 20
#define Ttq_us 2000000

#define GPIO1_NUM(x) (x + 480)
#define GPIO2_NUM(x) (x + 448)
#define GPIO3_NUM(x) (x + 416)
#define GPIO4_NUM(x) (x + 384)

#define SSN GPIO1_NUM(30)
#define SCK GPIO1_NUM(29)
#define MISO GPIO1_NUM(26)
#define MOSI GPIO1_NUM(24)
#define POWER GPIO1_NUM(25)

static uint8_t *send_buf;
static int send_lenth;

static int major;
static dev_t dev_id;
static struct class *class;
static struct device *device;

static spinlock_t lock;
static int dev_status; // 0表示可以使用，1表示被占用

// static char hclink_device[20] = {0};

// static int __init ag1002ae_get_device(char *line)
// {
//     strncpy(hclink_device, line, strlen(line));

//     return 1;
// }

static uint8_t LRC(unsigned char *data, int data_len)
{
    int i;
    unsigned char lrc = 0;

    for (i = 0; i < data_len; i++)
    {
        lrc ^= data[i];
    }
    lrc = ~lrc;
    return lrc;
}

static int spi_request_gpio(void)
{
    int ret = 0;
    if (gpio_request(SSN, "spi_ncs") < 0)
    {
        printk("can not get ssn\n");
        ret = -1;
    }
    if (gpio_request(SCK, "spi_sclk") < 0)
    {
        printk("can not get spi_sclk\n");
        ret = -1;
    }
    if (gpio_request(MISO, "spi_miso") < 0)
    {
        printk("can not get spi_miso\n");
        ret = -1;
    }
    if (gpio_request(MOSI, "spi_mosi") < 0)
    {
        printk("can not get spi_mosi\n");
        ret = -1;
    }
    if (gpio_request(POWER, "spi_power") < 0)
    {
        printk("can not get spi_power\n");
        ret = -1;
    }
    return 0;
}

static void spi_gpio_free(void)
{
    gpio_free(SSN);
    gpio_free(SCK);
    gpio_free(MISO);
    gpio_free(MOSI);
    gpio_free(POWER);
}

/* SPI端口初始化 */
static void spi_init(void)
{
    gpio_direction_output(SSN, 1);
    gpio_direction_output(SCK, 1);
    gpio_direction_output(MOSI, 0);
    gpio_direction_input(MISO);
    gpio_direction_output(POWER, 0);
    gpio_set_value(SCK, 1);
    gpio_set_value(MOSI, 0);
    gpio_set_value(POWER, 0);
}

/* 片选只有在最开始才设置*/
static void spi_reinit(void)
{
    gpio_direction_output(SCK, 1);
    gpio_direction_output(MOSI, 0);
    gpio_direction_input(MISO);
    gpio_set_value(SCK, 1);
    gpio_set_value(MOSI, 0);
}

/* 从设备使能 */
void ss_enable(int enable)
{
    if (enable)
        gpio_set_value(SSN, 0); // SS低电平，从设备使能有效
    else
        gpio_set_value(SSN, 1); // SS高电平，从设备使能无效
}

/* SPI字节写 */
static void spi_write_byte(unsigned char b)
{
    int i;
    for (i = 7; i >= 0; i--)
    {
        gpio_set_value(SCK, 0);
        udelay(5);                          //延时
        gpio_set_value(MOSI, b & (1 << i)); //从高位7到低位0进行串行写入
        udelay(5);                          //延时
        gpio_set_value(SCK, 1);             // CPHA=1，在时钟的第一个跳变沿采样
        udelay(5);                          //延时
    }
}

/* SPI字节读 */
static unsigned char spi_read_byte(void)
{
    int i;
    unsigned char r = 0;
    for (i = 0; i < 8; i++)
    {
        gpio_set_value(SCK, 0);
        udelay(5);                           //延时
        gpio_set_value(SCK, 1);              // CPHA=1，在时钟的第一个跳变沿采样
        udelay(5);                           //延时
        r = (r << 1) | gpio_get_value(MISO); //从高位7到低位0进行串行读出
        udelay(5);                           //延时
    }
    return r;
}

static int spi_read(uint8_t *buf, int len)
{
    int i;
    spi_reinit();
    for (i = 0; i < len; i++)
    {
        buf[i] = spi_read_byte();
    }
    return 0;
}

static int spi_write(uint8_t *buf, int len)
{
    int i = 0;
    spi_reinit();
    for (i = 0; i < len; i++)
    {
        spi_write_byte(buf[i]);
    }
    return 0;
}

static int sc1161y_open(struct inode *inode, struct file *filp)
{
    spin_lock(&lock);
    if (dev_status) // 1不可用
    {
        spin_unlock(&lock);
        return -EBUSY;
    }
    spi_init();
    mdelay(100);
    dev_status++;
    spin_unlock(&lock);
    return 0;
}

ssize_t sc1161y_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *off)
{
    int ret;
    // memset(send_buf, 0, SEND_BUF_SIZE);
    send_buf = kzalloc(cnt, GFP_KERNEL);
    ret = copy_from_user(send_buf, buf, cnt);
    if (ret < 0)
    {
        printk("sc1161y_write failed.\n");
        kfree(send_buf);
        send_buf = NULL;
        return -EAGAIN;
    }
    send_lenth = cnt;
    return send_lenth;
}

//实际上这里的cnt是无效的
ssize_t sc1161y_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
    int ret;
    uint8_t *recv_buf;
    uint8_t first_byte;
    uint8_t LRC1;
    uint8_t LRC2;
    int delay_timer = 0; //延时计时器
    uint16_t data_lenth;
    uint16_t total_lenth;
    int fault_times = 0;

    if (send_buf == NULL)
    {
        printk("send_buf is null.\n");
        return -EIO;
    }
    ss_enable(1);
    udelay(Tcsl_us);

    spi_write(send_buf, send_lenth);

    udelay(Tcsd_us);
    ss_enable(0);
    udelay(Tcsh_us);
    recv_buf = kzalloc(cnt, GFP_KERNEL);
    while (fault_times < FAULT_CHANCE)
    {
        ss_enable(1);
        udelay(Tcsl_us);
        spi_read(&first_byte, 1);
        while (first_byte != 0x55)
        {
            // if (first_byte != 0x00)
            //     printk("first_byte: %d\n", first_byte);
            if (delay_timer > Ttq_us)
            {
                printk("sc1161y_recive_result overtime.\n");
                goto time_out;
            }
            udelay(Tq_us);
            delay_timer = delay_timer + Tq_us;
            spi_read(&first_byte, 1);
        }

        //连续接受4字节
        spi_read(recv_buf, 4);

        //计算数据长度
        data_lenth = (*(uint8_t *)(recv_buf + 2)) << 8 | (*(uint8_t *)(recv_buf + 3));

        //根据长度接受所有数据,需要多接受一个字节的LRC2
        spi_read(recv_buf + 4, data_lenth + 1);

        udelay(Tcsd_us);

        ss_enable(0);

        udelay(Tcsh_us);

        LRC2 = *(uint8_t *)(recv_buf + 4 + data_lenth);
        LRC1 = LRC(recv_buf, 4 + data_lenth);
        if (LRC2 != LRC1)
        {
            printk("LRC falut,reload.\n");
            fault_times++;
        }
        break;
    }

    if (fault_times >= FAULT_CHANCE)
    {
        printk("no chance.\n");
        goto time_out;
    }

    total_lenth = 4 + data_lenth + 1;

    ret = copy_to_user(buf, recv_buf, total_lenth);
    kfree(recv_buf);
    kfree(send_buf);
    send_buf = NULL;
    udelay(Tcsd_us);
    ss_enable(0);
    udelay(Tcsh_us);
    return total_lenth;
time_out:
    udelay(Tcsd_us);
    ss_enable(0);
    udelay(Tcsh_us);
    kfree(recv_buf);
    kfree(send_buf);
    send_buf = NULL;
    return -EAGAIN;
}

static int sc1161y_release(struct inode *inode, struct file *filp)
{
    spin_lock(&lock);
    if (dev_status)
        dev_status = 0;
    gpio_set_value(POWER, 1);
    spin_unlock(&lock);
    return 0;
}

struct file_operations sc1161y_fops = {
    .owner = THIS_MODULE,
    .open = sc1161y_open,
    .read = sc1161y_read,
    .write = sc1161y_write,
    .release = sc1161y_release,
};

static int __init sc1161y_init(void)
{
    int ret = 0;
    // if (strcmp(hclink_device, "ag1002ae"))
    // {
    //     /*判断是否为ag1002ae，不是的话直接返回就行了*/
    //     printk("sc1161y_init() failed.this is not ag1002ae.\n");
    //     return 0;
    // }
    printk("sc1161y inits.\n");
    if (spi_request_gpio() != 0)
    {
        printk("sc1161y can not get gpio.\n");
        return -1;
    }

    major = register_chrdev(0, SC1161Y_NAME, &sc1161y_fops);

    if (major < 0)
    {
        printk("sc1161y can not register.\n");
        return ret;
    }

    dev_id = MKDEV(major, 0);

    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class))
    {
        ret = PTR_ERR(class);
        goto fail_class;
    }

    device = device_create(class, NULL, dev_id, NULL, SC1161Y_NAME);
    if (IS_ERR(device))
    {
        ret = PTR_ERR(device);
        goto fail_device;
    }

    //初始化自旋锁
    spin_lock_init(&lock);
    dev_status = 0;
    return 0;
fail_device:
    class_destroy(class);
fail_class:
    unregister_chrdev(major, SC1161Y_NAME);
    return ret;
}

static void __exit sc1161y_exit(void)
{
    printk("sc1161y exits\n");
    spi_gpio_free();
    unregister_chrdev(major, SC1161Y_NAME);
    device_destroy(class, dev_id);
    class_destroy(class);
}

// __setup("device=", ag1002ae_get_device);

module_init(sc1161y_init);
module_exit(sc1161y_exit);
MODULE_LICENSE("GPL");