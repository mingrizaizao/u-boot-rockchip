/*
 * Simple malloc implementation
 *
 * Copyright (c) 2014 Google, Inc
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <mapmem.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

// void *malloc_simple(size_t bytes)
// {
// 	ulong new_ptr;
// 	void *ptr;

// 	new_ptr = gd->malloc_ptr + bytes;
// 	debug("%s: size=0x%lx, ptr=0x%lx, limit=0x%lx: ", __func__, (unsigned long)bytes, new_ptr,
// 	      gd->malloc_limit);
// 	if (new_ptr > gd->malloc_limit) {
// 		debug("space exhausted\n");
// 		return NULL;
// 	}
// 	ptr = map_sysmem(gd->malloc_base + gd->malloc_ptr, bytes);
// 	gd->malloc_ptr = ALIGN(new_ptr, sizeof(new_ptr));
// 	debug("0x%lx\n", (ulong)ptr);

// 	return ptr;
// }


void *malloc_simple(size_t bytes)
{
#define BLINK_M(n) do { \
    volatile unsigned int *dr  = (volatile unsigned int *)0x2007c000; \
    volatile unsigned int *ddr = (volatile unsigned int *)0x2007c004; \
    volatile int _i, _j; \
    *ddr |= (1 << 26); \
    for (_j = 0; _j < (n); _j++) { \
        *dr &= ~(1 << 26); \
        for (_i = 0; _i < 500000; _i++); \
        *dr |= (1 << 26); \
        for (_i = 0; _i < 500000; _i++); \
    } \
} while(0)

    ulong new_ptr;
    void *ptr;

    // BLINK_M(1);
    new_ptr = gd->malloc_ptr + bytes;
    if (new_ptr > gd->malloc_limit) {
        BLINK_M(2);  /* 空间耗尽 */
        return NULL;
    }
    // BLINK_M(3);
    ptr = map_sysmem(gd->malloc_base + gd->malloc_ptr, bytes);
    gd->malloc_ptr = ALIGN(new_ptr, sizeof(new_ptr));
    return ptr;
}

void *memalign_simple(size_t align, size_t bytes)
{
	ulong addr, new_ptr;
	void *ptr;

	addr = ALIGN(gd->malloc_base + gd->malloc_ptr, align);
	new_ptr = addr + bytes - gd->malloc_base;
	if (new_ptr > gd->malloc_limit) {
		printf("sys malloc pool space exhausted\n");
		return NULL;
	}

	ptr = map_sysmem(addr, bytes);
	gd->malloc_ptr = ALIGN(new_ptr, sizeof(new_ptr));
	debug("%lx\n", (ulong)ptr);

	return ptr;
}

#if CONFIG_IS_ENABLED(SYS_MALLOC_SIMPLE)
void *calloc(size_t nmemb, size_t elem_size)
{
	size_t size = nmemb * elem_size;
	void *ptr;

	ptr = malloc(size);
	memset(ptr, '\0', size);

	return ptr;
}


#endif
