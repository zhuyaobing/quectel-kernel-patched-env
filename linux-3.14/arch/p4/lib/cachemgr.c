/*
 *  Cachemgr handling
 * 
 *  Copyright (C) 2013 SYSGO AG
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation, version 2 of the
 *  License.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>

#include <asm/vmapi_calls.h>
#include <asm/host_misc.h>

static unsigned int cachemgr_id;

#define CACHE_L1 0x01
#define CACHE_L2 0x02
#define D_CACHE_INVALIDATE_BLOCK_SIZE  (8U * 1024UL)
#define D_CACHE_INVALIDATE_SANITY_SIZE (2U * 1024UL * 1024UL)

/*************************************************************************************/
int uk_inval_dcache_range(unsigned long start, unsigned long size)
/* Invalidate the dcache which forces the cpu to read new data from RAM instead of
 * using old data in the cache. Normally used after DMA has transferred new data to
 * RAM to enforce the cpu to read this new data instead of old cached data.
 *
 * Interrupt latency problems have been caused by this function when large sizes are used.
 *
 * The uk_dev_call to invalidate dcache function is a PikeOS kernel blocking function
 * that causes PikeOS to stop handling interrupts and other higher priority threads until
 * the function returns.
 * Testing on a VCM which is 792 MHz solo imx6 has given the following
 * interrupt latencies with different sizes.
 * Size            latency
 * 2 Meg            2.2 ms
 * 1 Meg            1.2 ms
 *  512k            1.2 ms
 *   16k             42 us
 *   12k             36 us
 *    8k             32 us
 *    4k             32 us
 *    1k             32 us
 *
 * The latency for 8k is probably 18 - 20 us since other causes are giving latencies.
 *
 * In csp, testing the time for calling uk_inval_dcache_range() gives
 * similar figures of
 * Size         latency
 *  4k            9.2 us
 *  8k           17.6 us
 * 16k           34.2 us
 *
 * By calling uk_dev_call invalidate dcache function in a for loop with 8k blocks means
 * the PikeOS blocking function repeatedly returns and thereby reduces
 * interrupt latency to the csp.
 *
/*************************************************************************************/
{
   int err;
   unsigned long err_addr;

   unsigned long i;
   unsigned long num_whole_blocks;
   unsigned long block_start;
   unsigned long block_size;
   unsigned long block_remainder;

   block_size = D_CACHE_INVALIDATE_BLOCK_SIZE;

   /*
    * In Alexandra 90% of sizes are <= 8k. Most common is 4k then 512, with the odd 255 and 8k.
    * Then comes 16k and 61440.
    * This function is called often so is optimised for normal case of <= block_size.
    *
    * The D_CACHE_FLUSH_SANITY_SIZE is to stop insane size from causing the for loop to run for
    * 21 sec and cause a linux CPU stuck error. Anything larger than the dcache size internally
    * calls a function to clear the whole dcache.
    *
    */
   if ((size <= block_size) || (size > D_CACHE_INVALIDATE_SANITY_SIZE)) {
      /* invalidates dcache range */
      err = uk_dev_call(cachemgr_id, 0, start, size, (unsigned long)&err_addr, 3);
      if (err) {
         pr_err("%s: failed to invalidate dcache range 0x%08lx-0x%08lx, err=%i, err_addr=0x%08lx\n",
                __func__, start, start+size-1, err, err_addr);
         return -EIO;
      }
   }
   else {
      num_whole_blocks = size / block_size;
      block_remainder = size % block_size;

      for (i = 0; i < num_whole_blocks; ++i) {
         block_start = start + (block_size * i);
         err = uk_dev_call(cachemgr_id, 0, block_start, block_size, (void*)&err_addr, CACHE_L1 | CACHE_L2);
         if (err) {
            pr_err("%s: failed to invalidate dcache range 0x%08lx-0x%08lx, err=%i, err_addr=0x%08lx\n",
                   __func__, start, start+size-1, err, err_addr);
            return -EIO;
         }
      }

      if (block_remainder != 0) {
         /* Extra part block */
         block_start = start + (block_size * num_whole_blocks);
         err = uk_dev_call(cachemgr_id, 0, block_start, block_remainder, (void*)&err_addr, CACHE_L1 | CACHE_L2);
         if (err) {
            pr_err("%s: failed to invalidate dcache range 0x%08lx-0x%08lx, err=%i, err_addr=0x%08lx\n",
                   __func__, start, start+size-1, err, err_addr);
            return -EIO;
         }
      }
   }

   return 0;
}

static int __init uk_cachedrv_init(void)
{
	unsigned int devid;
	int err;

	err = vmapi_property_device_grant("prop:/psp/driver/CACHEMGR", &devid);
	if (err != 0)
		panic("failed to get CACHEMGR device grant. Check your VMIT configuration!");

	err = uk_dev_call(devid, 0xffffffff, 0x434d4731, 0, 0, 0);
	if (err != 0)
		panic("invalid cachemgr version magic or driver not available!\n");

	cachemgr_id = devid;
	printk(KERN_DEBUG "PikeOS CACHEMGR driver initialized.\n");
	return 0;
}
arch_initcall(uk_cachedrv_init);
