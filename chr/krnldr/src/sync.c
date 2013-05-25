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

//Source Code from http://osdevnotes.blogspot.com/2010/05/linux-synchronizing-kernel-mappings.html

/*
 * I want to write a hypervisor for ARM. I decided that my launch vehicle is the Linux
 * kernel, in fact, I am effectively turning the Linux kernel into the hypervisor. I
 * want to keep it as a loadable kernel module. Mainly, because I don't want to modify
 * the Linux kernel. This means I should be theoretically able to load it (and unload it)
 * anywhere where there is hardware support, root access, and a kernel source tree to build
 * the module against.
 *
 * The particular ISA extensions I am targeting add another CPU mode, in which you can intercept
 * hypercalls, IRQs, FIQs, and certain types of aborts. The first time I tried to intercept an IRQ
 * the kernel died with in the prefetch abort handler with a "Bad mode in prefetch abort handler
 * detected" message.
 *
 * Huh?
 *
 * The problem stems from the way memory is mapped within the kernel. Anytime vmalloc() is called
 * (and loaded modules go into vmalloc()ed memory), the kernel simply updates the master kernel page
 * tables (init_mm->pgd). When a page fault occurs, the kernel page fault handler checks if the address
 * falls within VMALLOC_START and VMALLOC_END, and propagates the page table entries from init_mm->pgd to the
 * current active translation tables.  On ARM, page faults come in as either data aborts or prefetch
 * (instruction access) aborts. When the module loaded, this happened in the context of the insmod process,
 * whose page tables then became fixed up in the page fault handler when the module was initialized. When
 * the IRQ arrived, the CPU went into the hypervisor mode, and the hypervisor IRQ vector tried to pass control
 * to a piece of code located in the module. Because a different process was executing at the time of the IRQ,
 * a page fault occurred, that should have resulted in patching up the page table, but didn't, because the abort
 * handler didn't expect the previous CPU state to be this new unknown-to-Linux state.
 * Fixing this means ensuring that no matter what context we're executing in, a page fault never happens while
 * accessing my module's data or code. We can't access init_mm (and the list lock), because these symbols are not
 * exported by the kernel. However, we /can/ propagate changes in the VMALLOC_START-VMALLOC_END range from the
 * current processes address space to all the other process address spaces, as the last part of initializing the
 * module. But we do need to be sure that the current address space contains all the mappings for the module's
 * code and data.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/tlbflush.h>
#include <linux/mm.h>

#include <defines.h>
#include <types.h>

/*!
\brief synchronizes page table entries describing a specific virtual address
  This is specific to ARM page tables.
\param source pointer to top level page directory to sync from
  \param destination pointer to top level page directory to sync to
  \param address virtual address to sync page table entries for
  \retval 0 success
\retval -EINVAL address is not in kernel space
 */
int sync_kernel_page_table_entries(pgd_t *source, pgd_t *destination, unsigned long address) {

	unsigned int index;
	pmd_t *pmd_s, *pmd_d;
	pud_t *pud_s, *pud_d;

	BUG_ON(source == NULL);
	BUG_ON(destination == NULL);
	BUG_ON(destination == source);
	BUG_ON((address & PAGE_MASK) != address);

	if (address < TASK_SIZE) {
		/* Address not in kernel space. */
		return -EINVAL;
	}

	index = pgd_index(address);
	destination += index;
	source += index;

	if (pgd_none(*source)) {
		return 0;
	}

	if(!pgd_present(*destination)) {
		set_pgd(destination, *source);
	}

	pud_s = pud_offset(source, address);
	pud_d = pud_offset(destination, address);

	pmd_s = pmd_offset(pud_s, address);
	pmd_d = pmd_offset(pud_d, address);

	if(pmd_none(*pmd_s)) {
		return 0;
	}

	copy_pmd(pmd_d, pmd_s);

	return 0;
}

/*!
\brief synchronizes kernel mappings from one address space to another
  Synchronizes kernel mappings falling between VMALLOC_START and VMALLOC_END from
  source address space to destination address space.
  \param source source address space to sync from
  \param destination destination address space to sync to
  \retval 0 success
*/
int sync_kernel_pages(struct mm_struct *source, struct mm_struct *destination, void *address, int size) {
	int status;
	unsigned long tmp;
	pgd_t *pgd_s = source->pgd;
	pgd_t *pgd_d = destination->pgd;
	BUG_ON(source == NULL);
	BUG_ON(destination == NULL);
	BUG_ON(destination == source);

	for (tmp = (unsigned long)address; tmp < ((unsigned long)address + size); tmp += PAGE_SIZE) {
		BUG_ON((tmp & PAGE_MASK) != tmp);
		status = sync_kernel_page_table_entries(pgd_s, pgd_d, tmp);
		if (unlikely(status != 0)) {
			return status;
		}
	}
	return 0;
}

/*!
\brief synchronizes kernel mappings to all address spaces
  Synchronizes kernel mappings from current address space to all other
  address spaces belonging to other processes.
  \retval 0 success
*/
int sync_kernel_pages_in_all_processes(void *address, int size) {
	int status;
	struct task_struct *task;

	task_lock(current);

	for_each_process(task) {
		if (unlikely(task == current)) {
			continue;
		}
		/* Kernel threads don't have their own address space. */
		if (unlikely(task->flags & PF_KTHREAD)) {
			continue;
		}
		task_lock(task);

		if(task->mm != NULL) {
			BUG_ON(task->mm == current->active_mm);
			status = sync_kernel_pages(current->active_mm, task->mm, address, size);
		}

		task_unlock(task);
		if (unlikely(status != 0)) {
			break;
		}
	}

	task_unlock(current);
	return status;
}

/*!
\brief ensures pages backing this module's memory have page table entries in current address space
 \retval 0 success
*/
int sync_touch(unsigned long start, unsigned long end) {

	u32_t address;
	u32_t scratch = 0;

	address = start;

	while(address < end) {
		scratch = *(volatile u32_t *)address;
		address += PAGE_SIZE;
	}

	return 0;
}
