#include "linux/device/class.h"
#include "linux/gfp_types.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/err.h>

#define mem_size 1020

dev_t virtDeviceNum =0;
static struct class *dev_class;
static struct cdev virtCharDevice;
uint8_t *kernel_buffer;

static int __init loader(void);
static void __exit exiter(void);
static int cDevOpen(struct inode *inode, struct file *file);
static int cDevClose(struct inode *inode, struct file *file);
static ssize_t cDevRead(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t cDevWrite(struct file *file, const char *buf, size_t len, loff_t *off);
void classErr(void);
void deviceErr(void);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = cDevRead,
	.write = cDevWrite,
	.open = cDevOpen,
	.release = cDevClose,
};

void classErr(void){
	unregister_chrdev_region(virtDeviceNum, 1);
}
void deviceErr(void){
	class_destroy(dev_class);
}

static int cDevOpen(struct inode *inode, struct file *file){
	pr_info("Device file opened\n");
	return 0;
}


static int cDevClose(struct inode *inode, struct file *file){
	pr_info("Device file closed\n");
	return 0;
}

static ssize_t cDevRead(struct file *file, char __user *buf, size_t len, loff_t *off){//Copying data to userspace
	if(copy_to_user(buf, kernel_buffer, mem_size)){
		pr_err("Data read err!\n");
	}
	pr_info("Data read successfull\n");
	return mem_size;
}

static ssize_t cDevWrite(struct file *file, const char *buf, size_t len, loff_t *off){//Copying data from userspace
	if(copy_from_user(kernel_buffer, buf, len)){
		pr_err("Data write err!\n");
	}
	pr_info("Data write successfull\n");
	return len;
}

static int __init loader(void){
	//Allocating Major number
	if((alloc_chrdev_region(&virtDeviceNum, 0, 1, "virtDevice"))<0){
		pr_err("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major =%d Minor =%d\n",MAJOR(virtDeviceNum),MINOR(virtDeviceNum));

	//Creating cdev struct
	cdev_init(&virtCharDevice,&fops);

	//Adding char device to system
	if((cdev_add(&virtCharDevice,virtDeviceNum,1))<0){
		pr_err("Cannot add virtual device to system\n");
		classErr();
		return -1;
	}

	//Creating struct class
	if(IS_ERR(dev_class = class_create("virtDevClass"))){
		pr_err("Cannot create struct class\n");
		classErr();
		return -1;
	}

	//Creating device
	if(IS_ERR(device_create(dev_class, NULL, virtDeviceNum, NULL, "virtDevice"))){
		pr_err("Cannot create device\n");
		deviceErr();
	}

	//Creating physical memory
	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL))==0){
		pr_err("Cannot allocate memory in kernel\n");
		deviceErr();
	}

	strcpy(kernel_buffer, "Hi Userspace ^o^"); // initializing kernel buffer with default data
	pr_info("Device driver inserted ^-^\n");
	return 0;
}

static void __exit exiter(void){
	kfree(kernel_buffer);
	device_destroy(dev_class, virtDeviceNum);
	class_destroy(dev_class);
	cdev_del(&virtCharDevice);
	unregister_chrdev_region(virtDeviceNum, 1);
	pr_info("Virtual device driver bailing out o7\n");
}

module_init(loader);
module_exit(exiter);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Niko the Useless");
MODULE_DESCRIPTION("Very simple virtual device driver");
MODULE_VERSION("1.0");
