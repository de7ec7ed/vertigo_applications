#ifndef __MAIN_H__
#define __MAIN_H__

#define MAIN__DEFAULT_BUFFER_SIZE 512
#define MAIN__DEFAULT_BUFFER_THRESHOLD 128

//#define MAIN__FILE_TO_LOAD "/data/data/siege.technologies.project/hypervisor.bin"

#define MAIN__FILE_TO_LOAD "trebuchet.bin"

static int __init main_init(void);

static void __exit main_exit(void);

#endif //__MAIN_H__
