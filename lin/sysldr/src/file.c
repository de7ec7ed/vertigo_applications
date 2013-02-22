/* This file is part of VERTIGO.
 *
 * (C) Copyright 2013, Siege Technologies <http://www.siegetechnologies.com>
 *
 * VERTIGO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VERTIGO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VERTIGO. If not, see <http://www.gnu.org/licenses/>.
 *
 * Written by Kirk Swidowski <kirk@swidowski.com>
 */

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
