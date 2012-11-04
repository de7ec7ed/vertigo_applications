#ifndef __MAIN_H__
#define __MAIN_H__

#include <types.h>
#include <armlib/vmsa/tt.h>
#include <armlib/vmsa/gen.h>

void usage(int argc, char *argv[]);
result_t remove_module(u8_t *string, size_t argc, u8_t *argv[]);
int main(int argc, char *argv[]);

// Missing system ldr prototype
extern result_t ldr_call_remove_module(u8_t *string, size_t argc, u8_t *argv[]);

#endif //__MAIN_H__
