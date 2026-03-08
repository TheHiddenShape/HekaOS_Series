#ifndef PRINTK_H
#define PRINTK_H

#include <stddef.h>

int printk (const char *format, ...);

// log levels (linux-compatible)
#define KERN_EMERG   "<0>" // system is unusable
#define KERN_ALERT   "<1>" // action must be taken immediately
#define KERN_CRIT    "<2>" // critical conditions
#define KERN_ERR     "<3>" // error conditions
#define KERN_WARNING "<4>" // warning conditions
#define KERN_NOTICE  "<5>" // normal but significant condition
#define KERN_INFO    "<6>" // informational
#define KERN_DEBUG   "<7>" // debug-level messages

#define pr_emerg(fmt, ...)   printk(KERN_EMERG fmt, ##__VA_ARGS__)
#define pr_alert(fmt, ...)   printk(KERN_ALERT fmt, ##__VA_ARGS__)
#define pr_crit(fmt, ...)    printk(KERN_CRIT fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...)     printk(KERN_ERR fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...)    printk(KERN_WARNING fmt, ##__VA_ARGS__)
#define pr_notice(fmt, ...)  printk(KERN_NOTICE fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...)    printk(KERN_INFO fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...)   printk(KERN_DEBUG fmt, ##__VA_ARGS__)

void dmesg (void);
void console_flush (void);
size_t get_log_buf (char *buf, size_t len);

#endif
