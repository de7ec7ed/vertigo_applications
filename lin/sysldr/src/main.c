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

#include <linux/init.h>
#include <linux/module.h>   // Needed by all modules
#include <linux/kernel.h>   // Needed for KERN_ALERT
#include <linux/fcntl.h>
#include <linux/slab.h>     // Needed for kmalloc and krealloc



#include <linux/cpu.h>
#include <linux/smp.h>
#include <linux/delay.h>

#include <armlib/vmsa/gen.h>
#include <armlib/int.h>

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

static int __init main_init(void) {

  int size = 0;
  int result = 0;

  printk(KERN_ALERT "\t------------------------------\n");
  printk(KERN_ALERT "\t-    v*v  0xde7ec7ed  v*v    -\n");
  printk(KERN_ALERT "\t-    *^* Linux sysldr *^*    -\n");
  printk(KERN_ALERT "\t------------------------------\n");

  printk(KERN_ALERT "main_init start\n");

  printk(KERN_ALERT "smp_processor_id: %08x\n", smp_processor_id());
  // take down all the CPUs other than CPU 0, this should effectively reschedule us
  // on CPU 0 if we started on another core.
  cpu_down(1);
  printk(KERN_ALERT "cpu_present_bits: %08x", *(unsigned int *)cpu_present_mask);
  // yeah yeah its a read only variable, not anymore!
  *(unsigned int *)(cpu_present_mask) = 0x1;
  printk(KERN_ALERT "cpu_possible_mask: %08x", *(unsigned int *)cpu_possible_mask);
  // yeah yeah its a read only variable, not anymore!
  *(unsigned int *)(cpu_possible_mask) = 0x1;
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

  binary_neuter_xn(buffer, BINARY_DEFAULT_BUFFER_SIZE);

  sync_touch((unsigned int)buffer, BINARY_DEFAULT_BUFFER_SIZE);

  sync_kernel_pages_in_all_processes();

  binary_patch(buffer);

  binary_print((void *)buffer, size);

  result = binary_execute((void *)buffer);

  printk(KERN_ALERT "result: %08x\n", result);
  
  printk(KERN_ALERT "main_init finished\n");
    
  return 0;
}

static void __exit main_exit(void) {
	printk(KERN_ALERT "main_exit\n");

	binary_unload(buffer);

	return;
}
