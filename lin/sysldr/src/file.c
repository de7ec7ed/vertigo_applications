#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#include <file.h>

struct file* file_open(const char* path, int flags, int rights) {
  
  struct file* filp = NULL;
  mm_segment_t oldfs;
  int err = 0;
  
  printk(KERN_ALERT "file__open\n");

  oldfs = get_fs();
  set_fs(get_ds());
  filp = filp_open(path, flags, rights);
  set_fs(oldfs);
 
  if(IS_ERR(filp)) {
    err = PTR_ERR(filp);
    return NULL;
  }

  return filp;
}

void file_close(struct file* file) {
  
  printk(KERN_ALERT "file__close\n");
    
  filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
  
  mm_segment_t oldfs;
  int ret;

  printk(KERN_ALERT "file__read\n");
  
  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_read(file, data, size, &offset);
  if (ret < 0) {
	  printk(KERN_ALERT "file__read error:%d\n", ret);
  }
  set_fs(oldfs);
  return ret;
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
  
  mm_segment_t oldfs;
  int ret;

  printk(KERN_ALERT "file__write\n");
  
  oldfs = get_fs();
  set_fs(get_ds());

  ret = vfs_write(file, data, size, &offset);

  set_fs(oldfs);
  return ret;
}
