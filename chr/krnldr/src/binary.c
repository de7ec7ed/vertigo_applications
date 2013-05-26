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
#include <linux/kernel.h>
#include <linux/module.h>   // Needed by all modules
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>     // Needed for kmalloc and krealloc
#include <asm/cacheflush.h> // Needed for flush_cache_all

#include <defines.h>
#include <types.h>

#include <binary.h>
#include <file.h>

#include <hdrlib/gen.h>
#include <hdrlib/krn.h>

#include <armv7lib/vmsa/tt.h>
#include <armv7lib/vmsa/gen.h>
#include <armv7lib/vmsa/tlb.h>

#define MEMORY_REGION_SIZE (2 * ONE_MEGABYTE)

result_t binary_load(const u8_t *path_and_name, void **buffer, size_t *size) {
  
  struct file* fp = NULL;
  size_t buffer_size = BINARY_DEFAULT_BUFFER_SIZE;
  size_t data_size = 0;
  size_t temp = 42;

  printk(KERN_ALERT "binary_load\n");

  fp = file_open(path_and_name, O_RDONLY, 0);

  if (fp) {
    *buffer = (u8_t *)vmalloc(sizeof(u8_t) * buffer_size);
	//*buffer = (u8_t *)kmalloc(sizeof(u8_t) * buffer_size, GFP_KERNEL);
    if (*buffer == NULL) {
    	printk(KERN_ALERT "binary__load: Unable to kmalloc %d", buffer_size);
    	return FAILURE;
    }
    
    memset(*buffer, 0, sizeof(u8_t) * buffer_size);

    // keep reading while data is still available
    do {
      // read up to the default buffer size limit;
      temp = file_read(fp, data_size, &(((u8_t *)*buffer)[data_size]), (buffer_size - data_size));
      if (temp < 0) {
    	  printk(KERN_ALERT "binary__load: Failed read in file\n");
    	  kfree(*buffer);
    	  file_close(fp);
    	  *buffer = NULL;
    	  *size = 0;
    	  return FAILURE;
      }

      data_size += temp;
      
      // check to see if the buffer is out of space
      if ((buffer_size - data_size) == 0){
    	  printk(KERN_ALERT "binary__load: increase BINARY__DEFAULT_BUFFER_SIZE in binary.h");
    	  kfree(*buffer);
    	  file_close(fp);
    	  *buffer = NULL;
    	  *size = 0;
    	  return FAILURE;
	}
      // check to see if fread actual had data available
    } while (temp > 0);
    
    file_close(fp);

    *size = data_size;
    
    return SUCCESS;
  }
  else {
    return FAILURE;
  }
}

result_t binary_execute(void *buffer) {
  
  int (*function)(void);
  int result = 0;
  
  printk(KERN_ALERT "binary_execute\n");

  function = buffer;
  flush_cache_all();

  result = function();

  return result;
}

result_t binary_neuter_xn(void *fb, size_t size) {

	tt_virtual_address_t va, l1, l2, start, end;
	tt_physical_address_t pa;
	tt_translation_table_base_register_t ttbr, ttbr0, ttbr1;
	tt_translation_table_base_control_register_t ttbcr;
	tt_first_level_descriptor_t fld;
	tt_second_level_descriptor_t sld;
	size_t tmp;

	printk(KERN_ALERT "binary_neuter_xn\n");

	printk(KERN_ALERT "this could take some time\n");

	va.all = (u32_t)fb;

	// set some limits on the range for the brute force
	// pa to va translations. It is assumed that kernel
	// memory is located above the 3GB line. So the low
	// end is 3GB the high end is 4GB assuming that the
	// kernel page tables are usually located just above
	// the 2GB line. This limit can be increased to whatever
	// but keep in mind it will take potentially longer to
	// perform the translation.

	start.all = 3 * (u32_t)ONE_GIGABYTE;
	end.all = (4 * (u32_t)ONE_GIGABYTE) - 1;

	// the linux kernel uses ttbr1 for the base of the kernel
	// page tables, would be great if they would just use ttbcr
	// to leverage it as well. it would get rid of all this
	// page table sync stuff.

	ttbr0 = tt_get_ttbr0();
	ttbr1 = tt_get_ttbr1();
	ttbcr = tt_get_ttbcr();
	tt_select_ttbr(va, ttbr0, ttbr1, ttbcr, &ttbr);


	//printk(KERN_ALERT "ttbcr:%08x\n", ttbcr.all);
	//printk(KERN_ALERT "ttbr:%08x\n", ttbr.all);

	tt_ttbr_to_pa(ttbr, &pa);

	if(gen_pa_to_va(pa, start, end, &l1) == SUCCESS) {

		while(va.all < ((u32_t)fb + size)) {
			tt_get_fld(va, l1, &fld);
			if(tt_fld_is_supersection(fld) == TRUE) {
				fld.supersection.fields.xn = FALSE;
				tt_set_fld(va, l1, fld);
				tmp = TT_SUPERSECTION_SIZE;
				//printk(KERN_ALERT "fld is a supersection\n");
			}
			else if(tt_fld_is_section(fld) == TRUE) {
				fld.section.fields.xn = FALSE;
				tt_set_fld(va, l1, fld);
				tmp = TT_SECTION_SIZE;
				//printk(KERN_ALERT "fld is a section\n");
			}
			else if(tt_fld_is_page_table(fld) == TRUE) {
				//printk(KERN_ALERT "fld:%08x\n", fld.all);

				tt_fld_to_pa(fld, &pa);
				if(gen_pa_to_va(pa, start, end, &l2) == SUCCESS) {

					tt_get_sld(va, l2, &sld);

					//printk(KERN_ALERT "sld:%08x\n", sld.all);

					if(tt_sld_is_large_page(sld) == TRUE) {
						//printk(KERN_ALERT "sld is a large page\n");
						sld.large_page.fields.xn = FALSE;
						tmp = TT_LARGE_PAGE_SIZE;
					}
					else if(tt_sld_is_small_page(sld) == TRUE) {
						//printk(KERN_ALERT "sld is a small page\n");
						sld.small_page.fields.xn = FALSE;
						tmp = TT_SMALL_PAGE_SIZE;
					}
					else {
						return FAILURE;
					}

					tt_set_sld(va, l2, sld);
				}
				else {
					printk(KERN_ALERT "unable to translate sld pa to va\n");
					return FAILURE;
				}
			}
			else {
				printk(KERN_ALERT "unable to translate fld pa to va\n");
				return FAILURE;
			}

			va.all += tmp;
		}
	}

	tlb_invalidate_entire_unified_tlb();
	flush_cache_all();

	return SUCCESS;
}

result_t binary_patch(void *buffer) {

	krn_header_t *syshdr;
	krn_import_header_t *imphdr;
	tt_virtual_address_t va;
	tt_physical_address_t pa;

	va.all = (size_t)buffer;

	syshdr = (krn_header_t *)buffer;

	imphdr = (krn_import_header_t *)((size_t)buffer + (size_t)(syshdr->import));

	imphdr->virtual_address = va.all;

	gen_va_to_pa(va, &pa);

	imphdr->physical_address = pa.all;

	imphdr->size = BINARY_DEFAULT_BUFFER_SIZE;

	imphdr->operating_system = GEN_IMPORT_OPERATING_SYSTEM_ANDROID;

	return SUCCESS;

}

result_t binary_print(void *buffer, size_t size) {
  
  size_t i;
  tt_virtual_address_t va, start, end;
  tt_physical_address_t pa;

  printk(KERN_ALERT "binary_print\n");


  start.all = 2 * (u32_t)ONE_GIGABYTE;
  end.all = (4 * (u32_t)ONE_GIGABYTE) - 1;
  pa.all = 0x12C30000;

  gen_pa_to_va(pa, start, end, &va);

  printk(KERN_ALERT "uart_address: %08x\n", va.all);

  printk(KERN_ALERT "binary address: %08x\n", (size_t)buffer);

  printk(KERN_ALERT "binary size: %d\n", size);

  size = 150;
  
  printk(KERN_ALERT "buffer: \n");
  
  // This runs the risk of printing a little more of the buffer then
  // is actually used. Luckly I did some lazy loading and allocated
  // large chunks so we will not overrun. (It was intentional ;))
  
  for(i = 0; i <= size; i += 8) {
    printk(KERN_ALERT "\t%02x%02x %02x%02x %02x%02x %02x%02x\n",
      ((char *)buffer)[i] & 0xff, ((char *)buffer)[i + 1] & 0xff,
      ((char *)buffer)[i + 2] & 0xff, ((char *)buffer)[i + 3] & 0xff,
      ((char *)buffer)[i + 4] & 0xff, ((char *)buffer)[i + 5] & 0xff,
      ((char *)buffer)[i + 6] & 0xff, ((char *)buffer)[i + 7] & 0xff);
  }

  printk(KERN_ALERT "\t     .... ....\n");
  
  return SUCCESS;
}

result_t binary_unload(void *buffer) {
	printk(KERN_ALERT "binary_unload\n");
	return SUCCESS;
}
