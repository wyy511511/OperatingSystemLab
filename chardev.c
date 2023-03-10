#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>


#define DEVICE_NAME "chardev" // Name of device in /dev
#define BUFFER_SIZE 756

static char *device_buffer;
static int Major = 240;



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
    if (*offset + length > BUFFER_SIZE)
    {
        printk(KERN_ALERT "Failed to read from chardev device file: read operation exceeds device file size\n");
        return -EINVAL;
    }

    if (copy_to_user(buffer, device_buffer + *offset, length) != 0)
    {
        printk(KERN_ALERT "Failed to read from chardev device file\n");
        return -EFAULT;
    }

    *offset += length;

    return length;
}

// static ssize_t chardev_read(struct file *file, char *buffer, size_t length, loff_t *offset)
// {
//     int nbytes = BUFFER_SIZE - *offset;
//     if (nbytes > length)
//     {
//         nbytes = length;
//     }
//     if (nbytes < 0)
//     {
//         nbytes = 0;
//     }

//     if (*offset + nbytes > BUFFER_SIZE)
//     {
//         printk(KERN_ALERT "Failed to read from chardev device file: read operation exceeds device file size\n");
//         return -EINVAL;
//     }

//     if (copy_to_user(buffer, device_buffer + *offset, nbytes) != 0)
//     {
//         printk(KERN_ALERT "Failed to read from chardev device file\n");
//         return -EFAULT;
//     }

//     *offset += nbytes;

//     if (*offset + length > BUFFER_SIZE)
//     {
//         return -1;
//     }

//     return nbytes;
// }

static ssize_t chardev_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    if (length + *offset > BUFFER_SIZE)
    {
        printk(KERN_ALERT "Failed to write to chardev device file: write operation exceeds device file size\n");
        return -1;
    }

    if (copy_from_user(device_buffer + *offset, buffer, length) != 0)
    {
        printk(KERN_ALERT "Failed to write to chardev device file\n");
        return -EFAULT;
    }

    *offset += length;

    return length;
}


static loff_t chardev_llseek(struct file *file, loff_t offset, int whence)
{
    loff_t ret = 0;
    loff_t newpos;

    switch (whence)
    {
        case 0: // SEEK_SET
            // if(offset < 0){
            //     ret = -EINVAL;
            //     break;
            // }
            // if ((unsigned int)offset >BUFFER_SIZE ){
            //     ret = -EINVAL;
            //     break;
            // }

            newpos = offset % BUFFER_SIZE;
            if (newpos < 0) {
                newpos += BUFFER_SIZE;
            }
            file->f_pos = newpos;
            ret = file->f_pos;
            break;



        case 1: // SEEK_CUR
        // if (file->f_pos + offset < 0){
        //         ret = -EINVAL;
        //         break;

        // }

                
            newpos = file->f_pos + offset ;
            newpos %= BUFFER_SIZE;
            if (newpos < 0) {
                newpos += BUFFER_SIZE;
            }
            file->f_pos = newpos;
            ret = file->f_pos;
            break;



        // if (file->f_pos + offset > BUFFER_SIZE){
                
        //     newpos = file->f_pos + offset ;
        //     newpos %= BUFFER_SIZE;
        //     if (newpos < 0) {
        //         newpos += BUFFER_SIZE;
        //     }
        //     file->f_pos = newpos;
        //     ret = file->f_pos;
        //     break;

        // }
        // file->f_pos += offset;
        // ret = file->f_pos;
        // break;



        case 2: // SEEK_END
            newpos = BUFFER_SIZE + offset;

            if(newpos < 0){
                ret =  -EINVAL;
                break;

            }
            newpos %= BUFFER_SIZE;
            file->f_pos = newpos;
            ret = file->f_pos;
            break;



        default:
            ret =  -EINVAL;
            break;
    }



    return ret;
}




// static loff_t chardev_llseek(struct file *file, loff_t offset, int whence)
// {
//     loff_t newpos;

//     switch (whence)
//     {
//         case 0: // SEEK_SET
//             newpos = offset;
//             break;

//         case 1: // SEEK_CUR
//             newpos = file->f_pos + offset;
//             break;

//         case 2: // SEEK_END
//             newpos = BUFFER_SIZE + offset;
//             break;

//         default:
//             return -EINVAL;
//     }


//     if (newpos < 0 || (whence == 1 || whence == 2) && newpos > BUFFER_SIZE)
//     {
//         return -EINVAL;
//     }

//     newpos %= BUFFER_SIZE;


//     file->f_pos = newpos;

//     return newpos;
// }



// static loff_t chardev_llseek(struct file *file, loff_t offset, int whence)
// {
//     loff_t newpos;

//     switch (whence)
//     {
//         case 0: // SEEK_SET
//             newpos = offset;
//             break;

//         case 1: // SEEK_CUR
//             newpos = file->f_pos + offset;
//             break;

//         case 2: // SEEK_END
//             newpos = BUFFER_SIZE + offset;
//             break;

//         default:
//             return -EINVAL;
//     }

//     if (newpos < 0 || (whence == 1 || whence == 2) && newpos > BUFFER_SIZE)
//     {
//         return -EINVAL;
//     }


//     newpos %= BUFFER_SIZE;

//     file->f_pos = newpos;

//     return newpos;
// }


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
int ret = 0;
 printk(KERN_INFO "Initializing chardev\n");  
   device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL); 
 if (!device_buffer) 
 { 
 printk(KERN_ALERT "Failed to allocate memory for device buffer\n"); 
 return -ENOMEM; 
 } 
     ret = register_chrdev(Major, DEVICE_NAME, &file_ops); 
 if (ret < 0) 
 { 
 printk(KERN_ALERT "Failed to register character device driver\n"); 
 kfree(device_buffer); 
 return ret; 
 } 
   printk(KERN_INFO "Registered chardev with major number %d\n", Major); 
   return 0; 
 } 


static void __exit chardev_exit(void)
{
printk(KERN_INFO "Exiting chardev\n");
unregister_chrdev(Major, DEVICE_NAME);
kfree(device_buffer);
}

module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amber Wu");
