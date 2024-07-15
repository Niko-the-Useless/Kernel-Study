#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Niko the Useless");

int value, newValue = 42;
char *name;

static int __init
loader(void) { // this will execute on loading module __init makes compiler mark
               // this function so linker collects it to the front
  pr_info(
      "you like kissing devs dont you\n"); // using pr_info instead of printk()
                                           // with KERN_INFO parameter used to
                                           // set how important it is
  pr_info("old value =%d\n", value);
  pr_info("new value =%d\n", newValue);
  pr_info("name = %s\n", name);
  return 0;
}

static void __exit exiter(void) { pr_info("bailing out o7\n"); } // this on exit

module_init(loader); // this will be called during initialization
module_exit(exiter); // this during exit

module_param(value, int,
             S_IRUSR | S_IWUSR); // int in /sys/module/NikoModule/parameters
module_param(name, charp, S_IRUSR | S_IWUSR); // char

int setter(const char *val,
           const struct kernel_param *kerParam) { // i have to use char for some
                                                  // reason to set int param
  int result = param_set_int(
      val, kerParam); // using helper function to write variable param_set_int
                      // returns 0 if it was sucessfull
  if (result == 0) {
    pr_info("Callback function called...\nnew value of variable 'value'=%d\n",
            newValue);
    return 0;
  }
  return -1;
}

const struct kernel_param_ops kerParamOps = {
    .set = &setter, // use notification setter
};

module_param_cb(
    newValue, &kerParamOps, &newValue,
    S_IRUGO | S_IWUSR); // module parameters callback function, this will be
                        // called when something writes to newValue pseudofile
                        // (/sys/modules/greet/parameters/newValue)
