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

#include <linux/init.h>
#include <linux/module.h>   // Needed by all modules
#include <linux/kernel.h>   // Needed for KERN_ALERT
#include <linux/fcntl.h>
#include <linux/slab.h>     // Needed for kmalloc and krealloc
#include <linux/mm.h>

#include <asm/io.h>

#include <linux/cpu.h>
#include <linux/smp.h>
#include <linux/delay.h>

#include <linux/sched.h>
#include <linux/proc_fs.h>

#include <armv7lib/vmsa/gen.h>
#include <armv7lib/int.h>

#include "main.h"
#include "file.h"
#include "binary.h"
#include "sync.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirk Swidowski <Kirk@Swidowski.com>");
MODULE_DESCRIPTION("0xde7ec7ed sysldr");
//MODULE_VERSION("proc");

static char *file = NULL;
module_param(file, charp, 0000);
MODULE_PARM_DESC(file, "a filename with absolute path");


module_init(main_init);
module_exit(main_exit);

char *buffer;

void main_print_bytes(unsigned char *buffer, unsigned int size) {

	unsigned int i;

	for(i = 0; i < size; i += 4) {
		printk(KERN_ALERT "%02x %02x %02x %02x\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]);
	}

	return;
}

void main_print_functions(void) {

	printk(KERN_ALERT "printk :%p\n", printk);
	main_print_bytes((unsigned char *)printk, 32);

	printk(KERN_ALERT "call_usermodehelper_setup :%p\n", call_usermodehelper_setup);
	main_print_bytes((unsigned char *)call_usermodehelper_setup, 32);

	printk(KERN_ALERT "call_usermodehelper_exec :%p\n", call_usermodehelper_exec);
	main_print_bytes((unsigned char *)call_usermodehelper_exec, 32);

	printk(KERN_ALERT "schedule :%p\n", schedule);
	main_print_bytes((unsigned char *)schedule, 32);

	printk(KERN_ALERT "create_proc_entry :%p\n", create_proc_entry);
	main_print_bytes((unsigned char *)create_proc_entry, 32);

	printk(KERN_ALERT "remove_proc_entry :%p\n", remove_proc_entry);
	main_print_bytes((unsigned char *)remove_proc_entry, 32);

	printk(KERN_ALERT "remap_pfn_range :%p\n", remap_pfn_range);
	main_print_bytes((unsigned char *)remap_pfn_range, 32);

	printk(KERN_ALERT "kmalloc :%p\n", kmalloc);
	main_print_bytes((unsigned char *)kmalloc, 32);

	printk(KERN_ALERT "kfree :%p\n", kfree);
	main_print_bytes((unsigned char *)kfree, 32);

	return;
}

static int __init main_init(void) {

  int size = 0;
  int result = 0;

  printk(KERN_ALERT "\t-------------------------------\n");
  printk(KERN_ALERT "\t-    v*v   de7ec7ed    v*v    -\n");
  printk(KERN_ALERT "\t-    *^* chrome krnldr *^*    -\n");
  printk(KERN_ALERT "\t-------------------------------\n");

  printk(KERN_ALERT "main_init start\n");

  printk(KERN_ALERT "smp_processor_id: %08x\n", smp_processor_id());

  // take down all the CPUs other than CPU 0, this should effectively reschedule us
  // on CPU 0 if we started on another core.
  cpu_down(1);

  printk(KERN_ALERT "smp_processor_id: %08x\n", smp_processor_id());

  if(file == NULL) {
    printk(KERN_ALERT "filename not specified\n");
    printk(KERN_ALERT "use sudo insmod linux_kernel_loader.ko file=\"filename with absolute path\"\n");
    return -1;
  }

  printk(KERN_ALERT "virtual to physical offset: %08x\n", (unsigned int)virt_to_phys(0));

  buffer = NULL;
  if((binary_load(file, (void **)&buffer, &size) == -1) || size == 0) {
    return -1;
  }

  msleep(1);

  if(binary_neuter_xn(buffer, BINARY_DEFAULT_BUFFER_SIZE) != SUCCESS) {
	  binary_unload(buffer);
	  return -1;
  }

  //binary_neuter_xn((void *)0xF7030000, FOUR_KILOBYTES);

  sync_touch((unsigned int)buffer, BINARY_DEFAULT_BUFFER_SIZE);

  sync_kernel_pages_in_all_processes((void *)VMALLOC_START, (VMALLOC_END - VMALLOC_START));

  binary_patch(buffer);

  binary_print((void *)buffer, size);

  result = binary_execute((void *)buffer);

  printk(KERN_ALERT "result: %08x\n", result);
  
  printk(KERN_ALERT "main_init finished\n");

  //main_print_functions();

  return 0;
}

static void __exit main_exit(void) {
	printk(KERN_ALERT "main_exit\n");

	binary_unload(buffer);

	return;
}
