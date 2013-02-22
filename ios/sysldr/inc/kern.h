/* This file is part of VERTIGO.
 *
 * (C) Copyright 2013, Siege Technologies <http://www.siegetechnologies.com>
 * (C) Copyright 2013, Kirk Swidowski <http://www.swidowski.com>
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

#ifndef __KERN_H__
#define __KERN_H__

#include <types.h>
#include <mach/mach.h>
#include <mach/vm_prot.h>

// This whole process could be automated by that is really not
// very interesting or needed at this point. The kernel is just a
// Mach-O executable

#define __IPHONE_4_IOS_511__

#ifdef __IPHONE_4_IOS_511__
// the hard address here are for iOS 5.1.1 iPhone 4
#define KERN_SYSENT_ADDRESS 0x802CDBAC
#define KERN_NSYSENT_ADDRESS 0x802D04D4
#endif //__IPHONE_4_IOS_511__

#ifdef __IPHONE_4_IOS_501__
// the hard address here are for iOS 5.0.1 iPhone 4
#define KERN_SYSENT_ADDRESS 0x802CCBAC
#define KERN_NSYSENT_ADDRESS 0x802CF404
#endif //__IPHONE_4_IOS_501__

typedef struct sysent sysent_t;

// Start copy from sysent.h Apple XNU Kernel & Comex White
struct proc;
typedef int32_t sy_call_t(struct proc *, void *, int *);
typedef void sy_munge_t(const void *, void *);

struct sysent {                     // system call table
    int16_t     sy_narg;            // number of args
    int8_t      sy_resv;            // reserved
    int8_t      sy_flags;           // flags
    sy_call_t   *sy_call;           // implementing function
    sy_munge_t  *sy_arg_munge32;    // system call arguments munger for 32-bit process
    sy_munge_t  *sy_arg_munge64;    // system call arguments munger for 64-bit process
    int32_t     sy_return_type;     // system call return types
    uint16_t    sy_arg_bytes;       // total size of arguments in bytes for 32-bit system calls

};
// End copy from sysent.h Apple XNU Kernel & Comex White

#define _SYSCALL_RET_INT_T     1

result_t kern_get_base(void **address);
result_t kern_get_task(mach_port_t *task);
result_t kern_read(void *address, size_t length, void *buffer, size_t *size);
result_t kern_write(void *address, void *buffer, size_t size);
result_t kern_allocate(void **pointer, size_t size, size_t flags);
result_t kern_wire(void *address, size_t size, vm_prot_t desired_access);
result_t kern_protect(void *address, size_t size, bool_t set_maximum, vm_prot_t attributes);
result_t kern_machine_attribute(void *address, size_t size, vm_machine_attribute_t attribute, vm_machine_attribute_val_t value);
result_t kern_get_sysent(sysent_t *sysent, size_t nsysent);
result_t kern_set_sysent(sysent_t *sysent, size_t nsysent);
void kern_print_sysent(sysent_t *sysent);
result_t kern_get_nsysent(size_t *nsysent);

#endif //__KERN_H__
