
#include<linux/init.h>
#include<linux/module.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "chardev" // Name of device in /dev
#define BUFFER_SIZE 756 // Size of the kernel buffer

static int Major; // Major number of the device driver
static char *device_buffer; // Pointer to the device buffer

// Called when the module is loaded
static int __init chardev_init(void)
{
    printk(KERN_INFO "Initializing chardev\n");

    // Allocate memory for the device buffer
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer)
    {
        printk(KERN_ALERT "Failed to allocate memory for device buffer\n");
        return -ENOMEM;
    }

    // Register the character device driver
    //Major = register_chrdev(0, DEVICE_NAME, &file_ops);
    Major = register_chrdev(240, DEVICE_NAME, &file_ops);
    if (Major < 0)
    {
        printk(KERN_ALERT "Failed to register character device driver\n");
        kfree(device_buffer);
        return Major;
    }

    printk(KERN_INFO "Registered chardev with major number %d\n", Major);

    return 0;
}

// Called when the module is unloaded
static void __exit chardev_exit(void)
{
    printk(KERN_INFO "Exiting chardev\n");

    // Unregister the character device driver
    unregister_chrdev(Major, DEVICE_NAME);

    // Free the memory allocated for the device buffer
    kfree(device_buffer);
}

// File operations for the character device driver
static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = chardev_read,
    .write = chardev_write,
    .open = chardev_open,
    .release = chardev_release,
};

// Called when the device file is opened
static int chardev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Opened chardev device file\n");
    return 0;
}

// Called when the device file is closed
static int chardev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Closed chardev device file\n");
    return 0;
}

// Called when data is read from the device file
static ssize_t chardev_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    // Copy data from the device buffer to the user buffer
    if (copy_to_user(buffer, device_buffer, BUFFER_SIZE) != 0)
    {
        printk(KERN_ALERT "Failed to read from chardev device file\n");
        return -EFAULT;
    }

    return BUFFER_SIZE;
}

// Called when data is written to the device file
static ssize_t chardev_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    // Copy data from the user buffer to the device buffer
    if (copy_from_user(device_buffer, buffer, BUFFER_SIZE) != 0)
    {
        printk(KERN_ALERT "Failed to write to chardev device file\n");
        return -EFAULT;
    }

    return BUFFER_SIZE;
}

// Register the init and exit functions with the kernel
module_init(chardev_init);
module_exit(chardev_exit);

