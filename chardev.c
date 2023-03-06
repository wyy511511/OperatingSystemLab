#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEVICE_NAME "chardev" // Name of device in /dev
#define BUFFER_SIZE 756 
// #define Major 240 

static char *device_buffer; 



// File operations for the character device driver
// Called when the device file is opened
static int chardev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Opened chardev device file\n");
    return 0;
}
static int chardev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Closed chardev device file\n");
    return 0;
}


static ssize_t chardev_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    
    int nbytes = BUFFER_SIZE - *offset;
    if (nbytes > length)
    {
        nbytes = length;
    }
    if (nbytes < 0)
    {
        nbytes = 0;
    }

    // Check if the read operation will exceed the device file size
    if (*offset + nbytes > BUFFER_SIZE)
    {
        printk(KERN_ALERT "Failed to read from chardev device file: read operation exceeds device file size\n");
        return -EINVAL;
    }

    // Copy data from the device buffer to the user buffer
    if (copy_to_user(buffer, device_buffer + *offset, nbytes) != 0)
    {
        printk(KERN_ALERT "Failed to read from chardev device file\n");
        return -EFAULT;
    }

    // Update the current position
    *offset += nbytes;

    return nbytes;
}


static ssize_t chardev_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    // Calculate the number of bytes to write
    int nbytes = BUFFER_SIZE - *offset;
    if (nbytes > length)
    {
        nbytes = length;
    }

    // Check if the write operation will exceed the device file size
    if (nbytes <= 0)
    {
        printk(KERN_ALERT "Failed to write to chardev device file: write operation exceeds device file size\n");
        return -EINVAL;
    }

    // Copy data from the user buffer to the device buffer
    if (copy_from_user(device_buffer + *offset, buffer, nbytes) != 0)
    {
        printk(KERN_ALERT "Failed to write to chardev device file\n");
        return -EFAULT;
    }

    // Update the current position
    *offset += nbytes;

    return nbytes; 
}
 


static loff_t chardev_llseek(struct file *file, loff_t offset, int whence)
{
    loff_t newpos;

    switch (whence)
    {
        case 0: // SEEK_SET
            newpos = offset;
            break;

        case 1: // SEEK_CUR
            newpos = file->f_pos + offset;
            break;

        case 2: // SEEK_END
            newpos = BUFFER_SIZE + offset;
            break;

        default:
            return -EINVAL;
    }

    // Check if the seek operation will exceed the device file size
    if (newpos < 0 || (whence == 1 || whence == 2) && newpos > BUFFER_SIZE)
    {
        return -EINVAL;
    }
    newpos %= BUFFER_SIZE;

    file->f_pos = newpos;

    return newpos;
}






static struct file_operations file_ops = {
    .owner = THIS_MODULE,
    .read = chardev_read,
    .write = chardev_write,
    .open = chardev_open,
    .release = chardev_release,
    .llseek = chardev_llseek,
};

static int __init chardev_init(void)
{
    printk(KERN_INFO "Initializing chardev\n");


    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer)
    {
        printk(KERN_ALERT "Failed to allocate memory for device buffer\n");
        return -ENOMEM;
    }


    Major = register_chrdev(240, DEVICE_NAME, &file_ops);
    if (Major < 0)
    {
        printk(KERN_ALERT "Failed to register character device driver\n");
        kfree(device_buffer);
        return Major;
    }
    if (Major != 240){
        printk(KERN_ALERT "fail240\n");
        }

    printk(KERN_INFO "Registered chardev with major number %d\n", Major);

    return 0;
    }

// Called when the module is unloaded
static void __exit chardev_exit(void)
{
    printk(KERN_INFO "Exiting chardev\n");

    // unregister_chrdev(Major, DEVICE_NAME);
    unregister_chrdev(240, DEVICE_NAME);


    kfree(device_buffer);
}

module_init(chardev_init);
module_exit(chardev_exit);
