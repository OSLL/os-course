#ifndef __MEMORY_H__
#define __MEMORY_H__

#define VIRTUAL_BASE	0xffffffff80000000
#define HIGHER_BASE	0xffff800000000000
#define MAX_PMEM_SIZE	0x00007fff00000000 // (256 * 512 - 4) GB
#define BOOTSTRAP_MEM	0x100000000
#define PAGE_SIZE	0x1000
#define PAGE_MASK	(PAGE_SIZE - 1)
#define KERNEL_CS	0x08
#define KERNEL_DS	0x10

#ifndef __ASM_FILE__

static inline void *va(uintptr_t phys)
{ return (void *)(phys + HIGHER_BASE); }

static inline uintptr_t pa(const void *virt)
{ return (uintptr_t)virt - HIGHER_BASE; }

#endif /*__ASM_FILE__*/

#endif /*__MEMORY_H__*/
