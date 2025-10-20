#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>

static int __init entry(void) {
  pr_info("loading sysuptime module\n");
  return 0;
}

static void __exit clean(void) {
  pr_info("unloading systemuptime module o7\n");
}

module_init(entry);
module_exit(clean);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Niko_the_Useless");
MODULE_DESCRIPTION("System uptime timer");
