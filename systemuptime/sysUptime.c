#include "linux/timekeeping.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define procfs_name "sysUptime"

static struct proc_dir_entry *sysUptime_file;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset) {

  char s[64];
  int len;
  s64 uptime_sec;

  if (*offset > 0) {
    return 0;
  }

  uptime_sec = ktime_get_boottime_seconds();

  len = snprintf(s, sizeof(s), "%lld\n", uptime_sec);

  if (copy_to_user(buffer, s, len)) {
    pr_warn("copy to user failed\n");
    return -EFAULT;
  }
  pr_info("procfile read %s\n", file_pointer->f_path.dentry->d_name.name);
  *offset += len;
  return len;
}

static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};

static int __init entry(void) {
  pr_info("loading sysuptime module\n");
  sysUptime_file = proc_create(procfs_name, 0644, NULL, &proc_file_fops);
  if (NULL == sysUptime_file) {
    pr_alert("Error: could not initialize /proc/%s\n", procfs_name);
    return -ENOMEM;
  }
  pr_info("/proc/%s creater\n", procfs_name);
  return 0;
}

static void __exit clean(void) {
  pr_info("unloading systemuptime module o7\n");
  proc_remove(sysUptime_file);
  pr_info("/proc/%s removed\n", procfs_name);
}

module_init(entry);
module_exit(clean);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Niko_the_Useless");
MODULE_DESCRIPTION("System uptime timer");
