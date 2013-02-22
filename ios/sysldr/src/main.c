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

// System Includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <mach/mach.h>
#include <mach/vm_prot.h>
#include <mach/vm_attributes.h>
#include <mach/vm_statistics.h>

// Global Includes
#include <defines.h>
#include <types.h>

// Local Includes
#include <check.h>
#include <main.h>
#include <kern.h>

// Library Includes
#include <armlib/cmsa/cac.h>
#include <armlib/vmsa/tt.h>
#include <armlib/vmsa/gen.h>
#include <armlib/vmsa/tlb.h>

#include <hdrlib/gen.h>
#include <hdrlib/sys.h>

#define HEADER__MEMORY_REGION_TABLE_SIZE 8
#define HEADER__MEMORY_REGION_DEFAULT_SIZE (4 * ONE_MEGABYTE)
#define HEADER__VIRTUAL_ADDRESS_TABLE_OFFSET 20
#define FLAT_BINARY_FILE_MAX_SIZE (ONE_MEGABYTE * 4)

int system_call(void *p, void *params, size_t *ret_val) {

	typedef __attribute__((long_call)) size_t (*function_t)();

	void *fb;
	size_t size;
	function_t function;

	fb = (void *)((u32_t *)params)[0];
	size = ((u32_t *)params)[1];

	function = (function_t)fb;
    
    //*ret_val = neuter_xn(fb, size);
    //return 0;
    
	if(neuter_xn(fb, size) != SUCCESS) {
		*ret_val = FAILURE;
		return 0;
	}

	if(patch_flat_binary_svc_mode(fb, size) != SUCCESS) {
		*ret_val = FAILURE;
		return 0;
	}

	// Lets make sure the caches and memory are sync'ed
	// This has been trouble in the past and the the
	// page tables where just modified. So, be safe!
	cac_flush_entire_data_cache();

	*ret_val = function();

	return 0;
}

result_t neuter_xn(void *fb, size_t size) {

	tt_virtual_address_t va, l1, l2, start, end;
	tt_physical_address_t pa;
	tt_translation_table_base_register_t ttbr;
	tt_first_level_descriptor_t fld;
	tt_second_level_descriptor_t sld;
	size_t tmp;


	va.all = (u32_t)fb;

	// set some limits on the range for the brute force
	// pa to va translations. It is assumed that kernel
	// memory is located above the 2GB line. So the low
	// end is 2GB the high end is 3GB assuming that the
	// kernel page tables are usually located just above
	// the 2GB line. This limit can be increased to whatever
	// but keep in mind it will take potentially longer to
	// perform the translation.

	start.all = 2 * (u32_t)ONE_GIGABYTE;
	end.all = 3 * (u32_t)ONE_GIGABYTE;

	// TODO: dynamically find out which translation tables
	// the flat binary was mapped in under. use TTBRC
	// think ~(~0 << 32-N) & va.all if 0 use ttbr0 else
	// use ttbr1. i think. if ttbr0 mask off N bits from the
	// top of the va. if ttbr1 do not.

	ttbr = tt_get_ttbr1();

	tt_ttbr_to_pa(ttbr, &pa);

	gen_pa_to_va(pa, start, end, &l1);
    
	while(va.all < ((u32_t)fb + size)) {
		tt_get_fld(va, l1, &fld);

		if(tt_fld_is_supersection(fld) == TRUE) {
			fld.supersection.fields.xn = FALSE;
			tmp = TT_SUPERSECTION_SIZE;
			tt_set_fld(va, l1, fld);
		}
		else if(tt_fld_is_section(fld) == TRUE) {
			fld.section.fields.xn = FALSE;
			tmp = TT_SECTION_SIZE;
			tt_set_fld(va, l1, fld);
		}
		else if(tt_fld_is_page_table(fld) == TRUE) {
			tt_fld_to_pa(fld, &pa);
			gen_pa_to_va(pa, start, end, &l2);

			tt_get_sld(va, l2, &sld);

			if(tt_sld_is_large_page(sld) == TRUE) {
				sld.large_page.fields.xn = FALSE;
				tmp = TT_LARGE_PAGE_SIZE;
			}
			else if(tt_sld_is_small_page(sld) == TRUE) {
				sld.small_page.fields.xn = FALSE;
				tmp = TT_SMALL_PAGE_SIZE;
			}
			else {
				return FAILURE;
			}

			tt_set_sld(va, l2, sld);
		}
		else {
			return FAILURE;
		}

		va.all += tmp;
	}

	tlb_invalidate_entire_unified_tlb();

	return SUCCESS;
}

result_t print_flat_binary(void *fb, size_t size, size_t limit) {

	size_t i;
	size_t tmp_size;
	void *tmp;

	tmp = malloc(FLAT_BINARY_FILE_MAX_SIZE);

	CHECK_SUCCESS(kern_read(fb, size, tmp, &tmp_size), "[-] unable to copy flat binary from kernel space\n");

	printf("0:\t");

	for(i = 0; i < size && i < limit; i++) {
		printf("%02x", ((unsigned char *)tmp)[i]);
		if((((i+1) % 8) == 0) && ((i + 1) != size)) {
			printf("\n%x:\t", (unsigned int)i+1);
		}
		else {
			printf(" ");
		}
	}

	if(i == limit && i < size) {
		printf("\n\t\t...");
	}

	printf("\n");

	free(tmp);

	return SUCCESS;
}

result_t load_flat_binary(char *file, void **fb, size_t *size) {

	FILE *fp;
	void *tmp;

	printf("[%%] file: %s\n", file);

	tmp = malloc(FLAT_BINARY_FILE_MAX_SIZE);

	CHECK_NOT_NULL(tmp, "[-] unable to malloc space for the tmp flat binary buffer\n");

	// possible kernel memory leak but who cares
	// the system would probably be better off if this doesn't get loaded and just causes a small memory leak.
	CHECK_SUCCESS(kern_allocate(fb, FLAT_BINARY_FILE_MAX_SIZE, VM_FLAGS_ANYWHERE), "[-] unable to allocate space for the flat binary\n");

	fp = fopen(file, "r");

	CHECK_NOT_NULL(fp, "[-] unable to open file\n");

	*size = fread(tmp, sizeof(u8_t), FLAT_BINARY_FILE_MAX_SIZE, fp);

	if(*size <= 0) {
		printf("[-] unable to read file\n");
		return FAILURE;
	}

	CHECK_SUCCESS(kern_protect(*fb, FLAT_BINARY_FILE_MAX_SIZE, TRUE, VM_PROT_ALL), "[-] unable to set the flat binary's memory as readable, writable and executable\n");

	CHECK_SUCCESS(kern_protect(*fb, FLAT_BINARY_FILE_MAX_SIZE, FALSE, VM_PROT_DEFAULT), "[-] unable to set the flat binary's memory as readable, writable and executable\n");

	CHECK_SUCCESS(kern_write(*fb, tmp, FLAT_BINARY_FILE_MAX_SIZE), "[-] unable to copy flat binary to kernel space\n");

	CHECK_SUCCESS(kern_wire(*fb, FLAT_BINARY_FILE_MAX_SIZE, VM_PROT_DEFAULT), "[-] unable to wire the flat binary's memory\n");

	CHECK_SUCCESS(kern_machine_attribute(*fb, FLAT_BINARY_FILE_MAX_SIZE, MATTR_CACHE, MATTR_VAL_CACHE_FLUSH), "[-] unable to flush the cache associated with the flat binary\n");

	free(tmp);

	return SUCCESS;
}

result_t patch_flat_binary_svc_mode(void *fb, size_t size) {

	sys_header_t *hdr;
	sys_import_header_t *imp_hdr;
	tt_virtual_address_t va;
	tt_physical_address_t pa;

	hdr = fb;
	imp_hdr = (sys_import_header_t *)((size_t)(hdr->import) + (size_t)fb);

	if(hdr->callsign != CALLSIGN) {
		return FAILURE;
	}

	va.all = (u32_t)fb;

	if(gen_va_to_pa(va, &pa) != SUCCESS) {
		return FAILURE;
	}

	imp_hdr->physical_address = pa.all;

	return SUCCESS;
}

result_t patch_flat_binary_usr_mode(void *fb, size_t size) {

	sys_header_t *hdr;
	sys_import_header_t *imp_hdr;
	size_t tmp;

	hdr = fb;

	CHECK_SUCCESS(kern_read(&(hdr->import), sizeof(imp_hdr), &imp_hdr, &tmp), "[-] unable to import header\n");

	imp_hdr = (sys_import_header_t *)((size_t)hdr + (size_t)imp_hdr);

	tmp = GEN_IMPORT_OPERATING_SYSTEM_IOS;
	CHECK_SUCCESS(kern_write(&(imp_hdr->operating_system), &tmp, sizeof(tmp)), "[-] unable to  set operating_system\n");

	CHECK_SUCCESS(kern_write(&(imp_hdr->virtual_address), (void *)&fb, sizeof(tmp)), "[-] unable to set virtual_address\n");

	tmp = FLAT_BINARY_FILE_MAX_SIZE;
	CHECK_SUCCESS(kern_write(&(imp_hdr->size), &tmp, sizeof(tmp)), "[-] unable to set size\n");

	CHECK_SUCCESS(kern_machine_attribute(fb, FLAT_BINARY_FILE_MAX_SIZE, MATTR_CACHE, MATTR_VAL_CACHE_FLUSH), "[-] unable to flush the cache associated with the flat binary\n");

	return SUCCESS;
}

void usage(int argc, char *argv[]) {

	printf("%s [file]\n", argv[0]);

	return;
}

result_t cross_usr_svc_mode_boundary(void *fb, size_t size) {

	size_t i;
	size_t ret_val;
	size_t nsysent;
	sysent_t *sysent;
	sysent_t old_entry;
	sysent_t new_entry = {2, 0, 0, (sy_call_t *)system_call, NULL, NULL, _SYSCALL_RET_INT_T, 0};

	// get the number of sysents
	CHECK_SUCCESS(kern_get_nsysent(&nsysent), "[-] unable to get kernel number of system entries");

	printf("[+] got nsysent\n");

	printf("[%%] nsysent: 0x%08x\n", (unsigned int)nsysent);

	// get space to store the sysent table
	sysent = malloc(nsysent * sizeof(sysent_t));

	CHECK_NOT_NULL(sysent, "[-] unable to malloc space for sysent");

	printf("[+] got space for sysent\n");

	// get the syscall table
	CHECK_SUCCESS(kern_get_sysent(sysent, nsysent), "unable to get kernel system entries");

	printf("[+] read the sysent table\n");

	// backup the old sysent
	memcpy(&old_entry, &(sysent[0]), sizeof(sysent_t));

	printf("[+] backed up the old sysent\n");

	// replace it with the new sysent
	memcpy(&(sysent[0]), &new_entry, sizeof(sysent_t));

	printf("[+] replaced the sysent\n");

	CHECK_SUCCESS(kern_set_sysent(sysent, nsysent), "unable to set kernel system entries");

	printf("[+] wrote the sysent table\n");

	printf("[%%] system_call address: 0x%08x\n", (unsigned int)system_call);

	printf("[%%] making syscall in : ");
	fflush(stdout);

	// slow down for a minute and let stdout catch up
	for(i = 3; i > 0; i--) {
		printf("%d ", (int)i);
		fflush(stdout);
		sleep(1);
	}

	printf("\n");
	fflush(stdout);

	ret_val = syscall(0, fb, size);

	if(ret_val != SUCCESS) {
		printf("[-] syscall returned failure: 0x%08x\n", (unsigned int)ret_val);
	}
	else {
		printf("[+] syscall returned success: 0x%08x\n", (unsigned int)ret_val);
	}

	memcpy(&(sysent[0]), &old_entry, sizeof(sysent_t));

	printf("[+] restoring the old sysent\n");

	CHECK_SUCCESS(kern_set_sysent(sysent, nsysent), "unable to set kernel system entries");

	printf("[+] wrote the sysent table\n");

	free(sysent);

	return ret_val;
}

result_t initialize_serial_port(void) {

	int fd;
	struct termios attributes;

    // Open the serial port read/write, with no controlling terminal, and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to be non-blocking.
    // See open(2) ("man 2 open") for details.
	fd = open("/dev/tty.iap", O_RDWR | O_NOCTTY | O_NONBLOCK);

	if(fd == -1) {
		printf("[-] unable to open the serial port\n");
		return FAILURE;
	}

	printf("[+] serial port opened\n");

	// Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
	// unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
	// processes.
	// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
	if (ioctl(fd, TIOCEXCL) == -1) {
		printf("[-] unable to restrict usage of the serial port\n");
		return FAILURE;
	}

	printf("[+] restricted usage of the serial port\n");

	// Now that the device is open, clear the O_NONBLOCK flag so subsequent I/O will block.
	// See fcntl(2) ("man 2 fcntl") for details.
	if (fcntl(fd, F_SETFL, 0) == -1) {
		printf("[-] unable to clear non blocking state on the serial port");
		return FAILURE;
	}

	printf("[+] cleared the non blocking state on the serial port\n");

	// Get the current options and save them so we can restore the default settings later.
    if (tcgetattr(fd, &attributes) == -1) {
    	printf("[-] unable to get the serial port's attributes\n");
    	return FAILURE;
    }


    // Set raw input (non-canonical) mode, with reads blocking until either a single character
    // has been received or a one second timeout expires.
    // See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.

    cfmakeraw(&attributes);
    attributes.c_cc[VMIN] = 1;
    attributes.c_cc[VTIME] = 10;

    // The baud rate, word length, and handshake options can be set as follows:

    cfsetspeed(&attributes, B115200);
    attributes.c_cflag |= (CS8);  // RTS flow control of input

    printf("[%%] set input baud rate to: %d\n", (int) cfgetispeed(&attributes));
    printf("[%%] set output baud rate to: %d\n", (int) cfgetospeed(&attributes));

    // Cause the new attributes to take effect immediately.
    if (tcsetattr(fd, TCSANOW, &attributes) == -1) {
    	printf("[-] unable to make attributes take effect\n");
    	return FAILURE;
    }

    printf("[+] set attributes to take effect immediately\n");

    return SUCCESS;
}

int main(int argc, char *argv[]) {

	void *address;
	void *fb;
	size_t size;

	printf("\t------------------------------\n");
	printf("\t-    v*v  0xde7ec7ed  v*v    -\n");
	printf("\t-    *^*  iOS sysldr  *^*    -\n");
	printf("\t------------------------------\n");

	if(argc < 2) {
		usage(argc, argv);
		return FAILURE;
	}

	// don't really need to do this but it is a good check to make sure we are all on
	// the same page for the kernel load address. (get it same page :))
	// if this returns failure. the hard coded address for nsysent and sysent should be
	// reverified by REing the kernelcache file.

	// this could be used in the future to parse the mach-o header to dynamically link
	// in kernel API.

	CHECK_SUCCESS(initialize_serial_port(), "[-] unable to initialize the serial port\n");

	printf("[+] serial port initialized\n");

	CHECK_SUCCESS(kern_get_base(&address), "[-] unable to get kernel base address\n");

	CHECK_SUCCESS(load_flat_binary(argv[1], &fb, &size), "[-] unable to load the flat binary\n");

	printf("[+] loaded the flat binary file\n");
	printf("[%%] flat binary loaded at kernel address: 0x%08x\n", (unsigned int)fb);
	printf("[%%] flat binary size: %d\n", (unsigned int)size);

	CHECK_SUCCESS(patch_flat_binary_usr_mode(fb, size), "[-] unable to patch the flat binary\n");

	printf("[+] patched the flat binary file\n");

	print_flat_binary(fb, size, 127);

	// FLAT_BINARY_FILE_MAX_SIZE used for size so the neuter_xn function will make the whole buffer executable
	CHECK_SUCCESS(cross_usr_svc_mode_boundary(fb, FLAT_BINARY_FILE_MAX_SIZE), "[-] unable to cross boundary\n");

	printf("[+] crossed boundary and launched\n");

	printf("[+] entering infinite loop press CTRL^Z to return to the prompt\n");

	// spin so the UART will stay configured correctly. I know really cheap but quick and easy.
	while(1) { sleep(~0); }

	return SUCCESS;
}
