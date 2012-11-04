#include <stdio.h>
#include <mach/mach.h>
#include <mach/vm_prot.h>

#include <check.h>
#include <defines.h>
#include <types.h>
#include <kern.h>

result_t kern_get_base(void **address) {

	u8_t buffer[0x100];
	size_t header_magic = 0xFEEDFACE;
	size_t size;
	size_t i, j;

	memset(buffer, 0, 0x100);

	// Assume the kernel is loaded at the 2GB line or higher
	for(i = 0x80000000;i != 0x00000000; i += 0x40000000) {
		// Assumption: The kernel is loaded at a page aligned address
		for(j = 0x1000; j < 0x10000; j+=0x1000) {
			if(kern_read((void *)i+j, 0x100, buffer, &size) == SUCCESS) {
				if(memcmp(buffer, &header_magic, (sizeof(u8_t) * 4)) == 0) {
					printf("[%%] kernel loaded at %08x\n", (int)(i+j));
					*address = (void *)i+j;
					return SUCCESS;
				}
			}
		}
	}

	return FAILURE;
}

result_t kern_get_task(mach_port_t *task) {

	if(task_for_pid(mach_task_self(), 0, task) != KERN_SUCCESS) {
		printf("[-] failed to get kernel task.\n");
		return FAILURE;
	}

	return SUCCESS;
}

result_t kern_read(void *address, size_t length, void *buffer, size_t *size) {

	mach_port_t task;
	size_t retval;

	CHECK_SUCCESS(kern_get_task(&task), "[-] kern_get_task failed");

	retval = vm_read_overwrite(task, (vm_address_t)address, (vm_size_t)length, (vm_address_t)buffer, (vm_size_t *)size);

	if(retval != KERN_SUCCESS) {
		printf("[-] failed to read kernel memory: %08x\n", (int)retval);
		return FAILURE;
	}

	return SUCCESS;
}

result_t kern_write(void *address, void *buffer, size_t size) {

	mach_port_t task;

	CHECK_SUCCESS(kern_get_task(&task), "[-] kern_get_task failed");

	if (vm_write(task, (vm_address_t)address, (vm_address_t)buffer, size) != KERN_SUCCESS) {
		 printf("[-] failed to write kernel memory");
		 return FAILURE;
	}

	return SUCCESS;
}

result_t kern_allocate(void **pointer, size_t size, size_t flags) {

	mach_port_t task;
	vm_address_t address;

	CHECK_SUCCESS(kern_get_task(&task), "[-] kern_get_task failed");

	if (vm_allocate(task, &address, size, flags) != KERN_SUCCESS) {
		printf("[-] failed to allocate kernel memory");
		return FAILURE;
	}

	*pointer = (void *)address;

	return SUCCESS;
}

result_t kern_protect(void *address, size_t size, bool_t set_maximum, vm_prot_t attributes) {

	mach_port_t task;

	CHECK_SUCCESS(kern_get_task(&task), "[-] kern_get_task failed");

	if(vm_protect(task, (vm_address_t)address, (vm_size_t)size, set_maximum, attributes) != KERN_SUCCESS) {
		printf("[-] failed to protect kernel memory\n");
		return FAILURE;
	}

	return SUCCESS;
}

result_t kern_machine_attribute(void *address, size_t size, vm_machine_attribute_t attribute, vm_machine_attribute_val_t value) {

	mach_port_t task;

	CHECK_SUCCESS(kern_get_task(&task), "[-] kern_get_task failed");

	if(vm_machine_attribute(task, (vm_address_t)address, (vm_size_t)size, attribute, &value) != KERN_SUCCESS) {
		printf("[-] failed to get or set machine attribute\n");
		return FAILURE;
	}

	return SUCCESS;
}

result_t kern_wire(void *address, size_t size, vm_prot_t desired_access) {

	mach_port_t task;

	CHECK_SUCCESS(kern_get_task(&task), "[-] kern_get_task failed");

	if (vm_wire(mach_host_self(), task, (vm_address_t)address, size, desired_access) != KERN_SUCCESS) {
		printf("[-] failed to wire kernel memory");
		return FAILURE;
	}

	return SUCCESS;
}
result_t kern_get_sysent(sysent_t *sysent, size_t nsysent) {

	size_t size;

	CHECK_SUCCESS(kern_read((void *)KERN_SYSENT_ADDRESS, nsysent, sysent, &size), "[-] unable to read kernel memory");

	return SUCCESS;
}

result_t kern_set_sysent(sysent_t *sysent, size_t nsysent) {

	CHECK_SUCCESS(kern_write((void *)KERN_SYSENT_ADDRESS, sysent, nsysent), "[-] unable to write kernel memory");

	return SUCCESS;
}

void kern_print_sysent(sysent_t *sysent) {

	printf("[%%] sy_narg:%d, sy_resv:%d, sy_flags:%08x, sy_call:%08x\n", (unsigned int)(sysent->sy_narg),
			(unsigned int)(sysent->sy_resv), (unsigned int)(sysent->sy_flags), (unsigned int)(sysent->sy_call));

	return;
}

result_t kern_get_nsysent(size_t *nsysent) {

	size_t size;

	CHECK_SUCCESS(kern_read((void *)KERN_NSYSENT_ADDRESS, sizeof(size_t), nsysent, &size), "[-] unable to read kernel memory");

	return SUCCESS;
}





