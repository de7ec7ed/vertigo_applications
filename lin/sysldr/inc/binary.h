#ifndef __BINARY_H__
#define __BINARY_H__

#include <defines.h>
#include <types.h>

#define BINARY_DEFAULT_BUFFER_SIZE ONE_MEGABYTE

result_t binary_load(const u8_t *path_and_name, void **buffer, size_t *size);
result_t binary_neuter_xn(void *fb, size_t size);
result_t binary_patch(void *buffer);
result_t binary_execute(void *buffer);
result_t binary_print(void *buffer, size_t size);
result_t binary_unload(void *buffer);

#endif //__BINARY_H__
