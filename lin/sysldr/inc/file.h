#ifndef __FILE_H__
#define __FILE_H__

struct file* file_open(const char* path, int flags, int rights);

void file_close(struct file* file) ;

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);

#endif //__FILE_H__
