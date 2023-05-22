#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/io.h>

#define LED_MAJOR   221
#define LED_Name    "LED_DRIVER"
#define BCM2011_PERI_BASE 0xFE000000
#define GPIO_BASE       (BCM2711_PERI_BASE+0x200000)
#define GPIO_SIZE   256

char led_usage = 0;
static void *led_map;
volatile unsigned *led;

static int led_init();
static void led_exit();
static int led_release(struct inode *, struct file *);
static int led_open(struct inode *, struct file *);
static int led_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations led_fops =
{
    .owner      = THIS_MODULE,
    .open       = led_open,
    .release    = led_release,
    .write      = led_write,
};

static int led_init(void) {
    int result;
    result = register_chrdev(LED_MAJOR,LED_NAME, &led_fops);
    if (result < 0) {
        printk(KERN_WARNING"Can't get any major!\n");
        return result;

    }
    return 0;
}

static void led_exit(void) {
    unregister _chrdev(LED_MAJOR, LED_NAME);
    printk("LED module removed.\n");

}

module_init(led_init);      //Register the module and its functions.
module_exit(led_exit);
MODULE_LICENSE ("GPL");

static int led_open(struct inode *inode, struct file *mfile)
{
    if (led_usage != 0)
        return -EBUSY;

    led_usage = 1;			// LED driver usage check

    led_map = ioremap(GPIO_BASE, GPIO_SIZE); //0x3F20 0000
						     // Physical addr mapping
    if (!led_map)			 // Mapping error handling
    {
        printk(“error: mapping gpio memory”);
        iounmap(led_map);
        return –EBUSY;
    }
    led = (volatile unsigned int *)led_map; // Set 4-byte pointer 
					          // to access the register(32bit)
    *(led + 1) &= ~(0x07 << (3 * 7)); 	// select GPIO 17 and clear function
    *(led + 1) |= (0x01 << (3 * 7)); 	// set GPIO17 as output mode

    return 0;
}

static int led_write(struct file *minode, const char *gdata, size_t length, loff_t *off_what)
{
    char tmp_buf;
    int result;

    result = copy_from_user(&tmp_buf, gdata, length) //1 byte data from
							  // application  pgm
	if (result < 0)	// Error handling
    {
        printk(“Error: copy from user”);
        return result;
    }
    
    printk(“data from user: %d\n”, tmp_buf);
    
    if (tmp_buf == 0)
        *(led + 7) = (0x01 << 17); 		// set GPIO17 as output 1
    else
        *(led + 10) = (0x01 << 17); 		// clear GPIO17 as output 0
    
    return length;
}

static int led_release(struct inode *minode, struct file *mfile)
{
    led_usage = 0;

    if (led)   iounmap(led);

    return 0;
}
