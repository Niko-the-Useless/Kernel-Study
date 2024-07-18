#include <linux/device/class.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Niko the Useless");

dev_t myDeviceNum = 0; // this variable holds both device numbers
int value, newValue = 42;
char *name;
static struct class *dev_class; // device class variable
static struct cdev myDeviceCdev; // my device charcter device file

static int __init loader(void); //module init function
static void __exit exiter(void); // module exit function
static int cdevOpener(struct inode *inode, struct file *file); // function to open character device file
static int cdevRelease(struct inode *inode, struct file *file); // function to relese charcter device file
static ssize_t cdevRead(struct file *file, char __user *buf, size_t len, loff_t *off); // function to read fron device file 
static ssize_t cdevWrite(struct file *file, const char *buf, size_t len, loff_t *off); // function to write to device file
int setter(const char *val, const struct kernel_param *kerParam); // function to write parameter

static struct file_operations fops = { //file operations structure
	.owner = THIS_MODULE,
	.read = cdevRead,
	.write = cdevWrite,
	.open = cdevOpener,
	.release = cdevRelease,
};

// this will execute on loading module __init makes compiler mark this function so linker collects it to the front
static int __init loader(void) {
	pr_info("you like kissing devs dont you\n"); // using pr_info instead of printk() with KERN_INFO parameter used to set how important it is
  pr_info("old value =%d\n", value);
  pr_info("new value =%d\n", newValue);
  pr_info("name = %s\n", name);

	// alocating number - identifies the driver and device
  if ((alloc_chrdev_region(&myDeviceNum, 0, 1, "very good device name")) < 0) {
    pr_info("Cannot allocate major number for device :c\n");
    return -1;
  }
  pr_info("Major =%d Minor =%d \n", MAJOR(myDeviceNum), MINOR(myDeviceNum));

	//creating struct class it will create structure under /sys/class/
	dev_class = class_create("device_class");
	if(IS_ERR(dev_class)){
		pr_err("Cannot create struct class for device :<\n");
		unregister_chrdev_region(myDeviceNum,1);
		return -1;
	}
	//creating device this willcreate device under /dev/
	if(IS_ERR(device_create(dev_class,NULL,myDeviceNum,NULL,"myDevice"))){
		pr_err("Cannot create device :<\n");
		class_destroy(dev_class);
	}
	//creating cdev structure
	cdev_init(&myDeviceCdev, &fops);
	//adding character device file to system
	if((cdev_add(&myDeviceCdev, myDeviceNum, 1))<0){
		pr_err("Cannot add char device ;-;\n");
		unregister_chrdev_region(myDeviceNum,1);
		return -1;
	}

  pr_info("Module inserted succesfully ^-^\n");
  return 0;
}

static void __exit exiter(void) {
	device_destroy(dev_class,myDeviceNum); //guess
	class_destroy(dev_class);
	cdev_del(&myDeviceCdev);
  unregister_chrdev_region(myDeviceNum, 1); // unregistering device numbers
  pr_info("bailing out o7\n");
} // this on exit

module_init(loader); // this will be called during initialization
module_exit(exiter); // this during exit

module_param(value, int, S_IRUSR | S_IWUSR); // int in /sys/module/NikoModule/parameters
module_param(name, charp, S_IRUSR | S_IWUSR); // char

int setter(const char *val, const struct kernel_param *kerParam) { // i have to use char for some reason to set int param
  int result = param_set_int(val, kerParam); // using helper function to write variable param_set_int returns 0 if it was sucessfull
  if (result == 0) {
    pr_info("Callback function called...\nnew value of variable 'value'=%d\n", newValue);
    return 0;
  }
  return -1;
}

const struct kernel_param_ops kerParamOps = {
    .set = &setter, // use notification setter
};

module_param_cb(newValue, &kerParamOps, &newValue,S_IRUGO | S_IWUSR); // module parameters callback function, this will be called when something writes to newValue pseudofile (/sys/modules/greet/parameters/newValue)

static int cdevOpener(struct inode *inode, struct file *file){
	pr_info("Opening function called!\n");
	return 0;
}

static int cdevRelease(struct inode *inode, struct file *file){
	pr_info("Release function called!\n");
	return 0;
}

static ssize_t cdevRead(struct file *file, char __user *buf, size_t len, loff_t *off){
	pr_info("Read function called!\n");
	return 0;
}

static ssize_t cdevWrite(struct file *file, const char __user *buf, size_t len, loff_t *off){
	pr_info("Write function called!\n");
	return len;
}
