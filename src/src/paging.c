#include <paging.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <balloc.h>
#include <debug.h>


struct page_pool {
	uintptr_t (*get_page)(struct page_pool *);
	void (*put_page)(struct page_pool *, uintptr_t);
};


static uintptr_t pool_get_page(struct page_pool *pool)
{
	return pool->get_page(pool);
}

static void pool_put_page(struct page_pool *pool, uintptr_t page)
{
	pool->put_page(pool, page);
}

static int pml_shift(int level)
{
	static const int offs[] = {0, 12, 21, 30, 39};

	BUG_ON(level < 1 || level > 4);
	return offs[level];
}

static int pml_offs(uintptr_t addr, int level)
{
	static const int mask[] = {0, 0xfff, 0x1ff, 0x1ff, 0x1ff};

	BUG_ON(level < 1 || level > 4);
	return (addr >> pml_shift(level)) & mask[level];
}

static uintptr_t pml_size(int level)
{
	BUG_ON(level < 1 || level > 4);
	return (uintptr_t)1 << pml_shift(level);
}

static int __pt_count_pages(uintptr_t begin, uintptr_t end, uintptr_t phys,
			int lvl)
{
	const uintptr_t size = pml_size(lvl);
	const uintptr_t mask = size - 1;

	const int from = pml_offs(begin, lvl);
	const int to = pml_offs(end - 1, lvl) + 1;
	const int notaligned = (begin & mask) || (phys & mask);

	int count = 1;

	if (lvl == 1)
		return count;

	for (int i = from; i != to; ++i) {
		const uintptr_t entry_end = (begin + size) & ~mask;
		const uintptr_t map_begin = begin;
		const uintptr_t map_end = entry_end < end ? entry_end : end;
		const uintptr_t tomap = map_end - map_begin;

		if (lvl == 4 || notaligned)
			count += __pt_count_pages(map_begin, map_end, phys,
						lvl - 1);

		begin += tomap;
		phys += tomap;
	}

	return count;
}

static int pt_count_pages(uintptr_t begin, uintptr_t end, uintptr_t phys)
{
	return __pt_count_pages(begin, end, phys, 4);
}

/* Well, i have no much to say this function is a complete mess. */
static void __pt_map_pages(pte_t *pml, uintptr_t begin, uintptr_t end,
			uintptr_t phys, pte_t flags,
			int lvl, struct page_pool *pool)
{
	const pte_t pde_flags = __PTE_PRESENT | PTE_WRITE | PTE_USER;
	const uintptr_t size = pml_size(lvl);
	const uintptr_t mask = size - 1;

	const int from = pml_offs(begin, lvl);
	const int to = pml_offs(end, lvl);
	const pte_t pte_large = (lvl == 3 || lvl == 2) ? __PTE_LARGE : 0;

	for (int i = from; i <= to; ++i) {
		const int notaligned = (begin & mask) || (phys & mask);
		const uintptr_t entry_end = ((begin + size) & ~mask) - 1;
		const uintptr_t map_end = entry_end < end ? entry_end : end;

		const uintptr_t map_begin = begin;
		const uintptr_t tomap = map_end - map_begin + 1;

		pte_t pte = pml[i];

		if (!(pte & __PTE_PRESENT)) {
			if (lvl != 4 && tomap == size && !notaligned) {
				pml[i] = (pte_t)phys | flags | pte_large;
				begin += tomap;
				phys += tomap;
				continue;
			}
			BUG_ON(lvl == 1);

			const uintptr_t page = pool_get_page(pool);

			/* I prefer to preallocate all required pages
			 * beforehand, so that mapping itself cannot fail,
			 * and thus this BUG_ON. */
			BUG_ON(!page);
			pte = pml[i] = (pte_t)page | pde_flags;
		}

		BUG_ON(lvl == 1);
		__pt_map_pages((pte_t *)(pte & PTE_PHYS_MASK),
					map_begin, map_end,
					phys, flags, lvl - 1, pool);
		begin += tomap;
		phys += tomap;
	}
}

static void pt_map_pages(pte_t *pml4, uintptr_t begin, uintptr_t end,
			uintptr_t phys, pte_t flags, struct page_pool *pool)
{
	BUG_ON(begin & PAGE_MASK);
	BUG_ON(end & PAGE_MASK);

	/* We pass end - 1 to avoid overflows, and __pt_map_pages assumes
	 * both ends of the interval are included in the interval. */
	__pt_map_pages(pml4, begin, end - 1, phys, flags | __PTE_PRESENT,
				4, pool);
}

static uintptr_t paging_setup_get_page(struct page_pool *pool)
{
	(void) pool;

	/* Since we can't work without propper page table, when
	 * we setup our first page table we can allocate pages
	 * one by one and just fail if allocation failed. */
	const uintptr_t page = __balloc_alloc(
				/* size = */PAGE_SIZE,
				/* align =  */PAGE_SIZE,
				/* from = */0,
				/* to =  */BOOTSTRAP_MEM);
	pte_t *pte = va(page);

	BUG_ON(page == BOOTSTRAP_MEM &&
			"Not enough free memory to setup initial page table");
	memset(pte, 0, PAGE_SIZE);
	return page;
}

void paging_setup(void)
{
	const uintptr_t phys_mem_limit = balloc_memory() & ~PAGE_MASK;
	const uintptr_t mem_size = phys_mem_limit < MAX_PMEM_SIZE
				? phys_mem_limit : MAX_PMEM_SIZE;

	struct page_pool pool = { &paging_setup_get_page, 0 };
	const uintptr_t pml4 = pool_get_page(&pool);
	pte_t *pte = va(pml4);

	pt_map_pages(pte, HIGHER_BASE, HIGHER_BASE + mem_size, 0,
				PTE_WRITE, &pool);
	pt_map_pages(pte, VIRTUAL_BASE, 0, 0,
				PTE_WRITE, &pool);
	__asm__ ("movq %0, %%cr3" : : "r"(pml4));

	/* Not used yet, so we need to shut up compiler. */
	(void) pool_put_page;
	(void) pt_count_pages;
}
