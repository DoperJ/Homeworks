/*the scull.c*/
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h> 
//#include <asm/system.h> /* cli(), *_flags */
#include <linux/semaphore.h>
#include <asm/uaccess.h> /* copy_*_user */
#include "scull.h" /* local definitions */
//在装载时候设置的参数/
int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS; //scull 设备数
int scull_quantum = SCULL_QUANTUM; //量子大小
int scull_qset = SCULL_QSET; //量子集大小
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);
MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");
struct scull_dev *scull_devices;
////////////////////////////////////////////////////////////////////////////////////////////
//遍历链表,释放所由量子和量子集
int scull_trim(struct scull_dev *dev)
{
 struct scull_qset *next, *dptr;
 int qset = dev->qset;
 int i;
 for (dptr = dev->data; dptr; dptr = next) {
 if (dptr->data) {
 for (i = 0; i < qset; i++)
 kfree(dptr->data[i]);
 kfree(dptr->data);
 dptr->data = NULL;
 }
 next = dptr->next;
 kfree(dptr); 
 }
 dev->size = 0;
 dev->quantum = scull_quantum;
 dev->qset = scull_qset;
 dev->data = NULL;
 return 0;
}
//打开&释放设备

int scull_open(struct inode *inode, struct file *filp)
{
 struct scull_dev *dev;
 //从 inode 参数获取 cdev 结构
 dev = container_of(inode->i_cdev, struct scull_dev, cdev);
 filp->private_data = dev;
 if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
 if (down_interruptible(&dev->sem))//获取信号量
 return -ERESTARTSYS;
 scull_trim(dev);
 up(&dev->sem);//释放信号量
 }
 return 0;
}
int scull_release(struct inode *inode, struct file *filp)
{
 return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////
//沿链表前进,直到正确的位置
struct scull_qset *scull_follow(struct scull_dev *dev, int n)
{
 struct scull_qset *qs = dev->data;
 if (! qs) {
 qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
 if (qs == NULL)
 return NULL;
 memset(qs, 0, sizeof(struct scull_qset));
 }
 //查找位置
 while (n--) { 
 if (!qs->next) {
 qs->next = kmalloc(sizeof(struct scull_qset),GFP_KERNEL);
 if (qs->next == NULL)
 return NULL;
 memset(qs->next, 0, sizeof(struct scull_qset));
 }
 qs = qs->next;
 continue;
 }
 return qs;
}
//读写操作
ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
 loff_t *f_pos)
{
 struct scull_dev *dev = filp->private_data;
 struct scull_qset *dptr; //第一个链表项
 int quantum = dev->quantum, qset = dev->qset;
 int itemsize = quantum * qset; //该链表有多少字节
 int item, s_pos, q_pos, rest;
 ssize_t retval = 0;
 if (down_interruptible(&dev->sem))//获取信号量
 return -ERESTARTSYS;
 if (*f_pos >= dev->size)
 goto out;
 if (*f_pos + count > dev->size)
 count = dev->size - *f_pos;
 //在量子集中寻找链表项,qset 索引以及偏移量
 item = (long)*f_pos / itemsize;
 rest = (long)*f_pos % itemsize;
 s_pos = rest / quantum; q_pos = rest % quantum;
 dptr = scull_follow(dev, item);
 if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
 goto out;
 //读取该量子的数据直到结尾
 if (count > quantum - q_pos)
 count = quantum - q_pos; 
 //将数据复制到拥护空间
 if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
 retval = -EFAULT;
 goto out;
 }
 *f_pos += count;
 retval = count;
 out:
 up(&dev->sem);
 return retval;
}
ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
 loff_t *f_pos)
{
 struct scull_dev *dev = filp->private_data;
 struct scull_qset *dptr;
 int quantum = dev->quantum, qset = dev->qset;
 int itemsize = quantum * qset;
 int item, s_pos, q_pos, rest;
 ssize_t retval = -ENOMEM;
 if (down_interruptible(&dev->sem))
 return -ERESTARTSYS;
 //在量子集中寻找链表项,qset 索引以及偏移量
 item = (long)*f_pos / itemsize;
 rest = (long)*f_pos % itemsize;
 s_pos = rest / quantum; q_pos = rest % quantum;
 dptr = scull_follow(dev, item);
 if (dptr == NULL)
 goto out;
 if (!dptr->data) {
 dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
 if (!dptr->data)
 goto out;
 memset(dptr->data, 0, qset * sizeof(char *));
 }
 if (!dptr->data[s_pos]) {
 dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
 if (!dptr->data[s_pos])
 goto out; 
 }
 //将数据写入该量子,直到结尾
 if (count > quantum - q_pos)
 count = quantum - q_pos;
 if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
 retval = -EFAULT;
 goto out;
 }
 *f_pos += count;
 retval = count;
 //更新文件大小
 if (dev->size < *f_pos)
 dev->size = *f_pos;
 out:
 up(&dev->sem);
 return retval;
}
//设备定位
loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
 struct scull_dev *dev = filp->private_data;
 loff_t newpos;
 switch(whence) {
 case 0: /* SEEK_SET */
 newpos = off;
 break;
 case 1: /* SEEK_CUR */
 newpos = filp->f_pos + off;
 break;
 case 2: /* SEEK_END */
 newpos = dev->size + off;
 break;
 default: /* can't happen */
 return -EINVAL;
 }
 if (newpos < 0) return -EINVAL;
 filp->f_pos = newpos;
 return newpos; 
}
struct file_operations scull_fops = {
 .owner = THIS_MODULE,
 .llseek = scull_llseek,
 .read = scull_read,
 .write = scull_write,
 .open = scull_open,
 .release = scull_release,
};
//模块的卸载函数
void scull_cleanup_module(void)
{
 int i;
 dev_t devno= MKDEV(scull_major, scull_minor);
 if (scull_devices) {
 for (i = 0; i < scull_nr_devs; i++) {
 scull_trim(scull_devices + i);
 cdev_del(&scull_devices[i].cdev);//移除字符设备
 }
 kfree(scull_devices);
 }
 unregister_chrdev_region(devno,scull_nr_devs);
 printk(KERN_ALERT "The scull device has been uninstall\n");
}
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
 int err, devno = MKDEV(scull_major, scull_minor + index);
 cdev_init(&dev->cdev, &scull_fops);
 dev->cdev.owner = THIS_MODULE;
 dev->cdev.ops = &scull_fops;
 err = cdev_add (&dev->cdev, devno, 1);//把该结构信息加入内核
 if (err)
 printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}
//模块的初始化函数
int scull_init_module(void)
{
 int result, i; 
 dev_t dev = 0;
 //动态分配设备编号
 if (scull_major) {
 dev = MKDEV(scull_major, scull_minor);
 result = register_chrdev_region(dev, scull_nr_devs, "scull");
 } else {
 result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
 "scull");
 scull_major = MAJOR(dev);
 }
 if (result < 0) {
 printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
 return result;
 }
 //申请内存块,并初始化
 scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
 if (!scull_devices) {
 result = -ENOMEM;
 goto fail;
 }
 memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));
 //初始化每个设备
 for (i = 0; i < scull_nr_devs; i++) {
 scull_devices[i].quantum = scull_quantum;
 scull_devices[i].qset = scull_qset;
 sema_init(&scull_devices[i].sem, 1);
 //init_MUTEX(&scull_devices[i].sem);
 scull_setup_cdev(&scull_devices[i], i);
 }
 dev = MKDEV(scull_major, scull_minor + scull_nr_devs);
 printk(KERN_ALERT "The scull device has been install\n");
 return 0;
fail:
 scull_cleanup_module();
 return result;
}
module_init(scull_init_module);
module_exit(scull_cleanup_module);
MODULE_LICENSE("GPL");