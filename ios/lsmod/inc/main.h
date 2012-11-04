#ifndef __MAIN_H__
#define __MAIN_H__

#include <types.h>
#include <armlib/vmsa/tt.h>
#include <armlib/vmsa/gen.h>
#include <hdrlib/mod.h>

void usage(int argc, char *argv[]);
result_t copy_module_header(size_t index, mod_header_t *hdr, size_t size);
int main(int argc, char *argv[]);

// Missing system ldr prototype
extern result_t ldr_call_copy_module_header(size_t index, mod_header_t *hdr, size_t size);

#endif //__MAIN_H__
