#ifndef __MAIN_H__
#define __MAIN_H__

#include <types.h>
#include <armlib/vmsa/tt.h>
#include <armlib/vmsa/gen.h>

// SVC Mode Functions
result_t neuter_xn(void *fb, size_t size);
int system_call(void *p, void *params, size_t *ret_val);

// USR Mode Functions
size_t cross_usr_svc_mode_boundary(void *fb, size_t size);
result_t print_flat_binary(void *fb, size_t size, size_t limit);
result_t load_flat_binary(char *file, void **fb, size_t *size);
result_t patch_flat_binary_usr_mode(void *fb, size_t size);
result_t patch_flat_binary_svc_mode(void *fb, size_t size);
void usage(int argc, char *argv[]);
int main(int argc, char *argv[]);

// Missing system ldr prototype
extern result_t ldr_call_add_module(void *pointer, size_t size);


// Missing ioctl prototype
extern int ioctl(int fd, int command, ...);

void flush_entire_data_cache();

#endif //__MAIN_H__
