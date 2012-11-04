#ifndef __SYNC_H__
#define __SYNC_H__

int sync_kernel_page_table_entries(pgd_t *source, pgd_t *destination, unsigned long address);
int sync_kernel_pages(struct mm_struct *source, struct mm_struct *destination);
int sync_kernel_pages_in_all_processes(void);
int sync_touch(unsigned long start, unsigned long end);
#endif //__SYNC_H__
