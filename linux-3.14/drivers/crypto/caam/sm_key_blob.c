/*
 * Copyright (C) 2016 OpenSynergy GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/gfp.h>
#include <linux/err.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <crypto/hash.h>
#include <linux/genalloc.h>
#include <asm/uaccess.h>
#include "compat.h"
#include "intern.h"
#include "secvio.h"
#include "snvsregs.h"
#include "regs.h"
#include "sm.h"
#include "sm_key_blob.h"

#define SM_BLOB_KEY_NAME "sm_key_blob"

/*
 *#define KERNEL_DEBUG
*/

#define CAAM_SM_ADDRESS 0x50100000

static u8 skeymod[] = {
	0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};
struct device_node *dev_node;
struct platform_device *pdev;
struct caam_drv_private *ctrlpriv;
struct caam_drv_private_sm *kspriv;
struct device *ctrldev, *ksdev;
static u32 unit;

static struct cdev *sm_key_blob_cdev;
static struct class *sm_key_blob_class;
static struct device *sm_key_blob_device;
static dev_t sm_key_blob_dev;

#ifdef KERNEL_DEBUG
static void key_display(struct device *dev, u8 *label, u16 size, u8 *key)
{
  unsigned i;

  dev_info(dev, label);
  for (i = 0; i < size; i += 8)
    dev_info(dev,
            "[%04d] %02x %02x %02x %02x %02x %02x %02x %02x\n",
            i, key[i], key[i + 1], key[i + 2], key[i + 3],
            key[i + 4], key[i + 5], key[i + 6], key[i + 7]);
}
#else
#define key_display(dev, label, size, key)
#endif


static int sm_key_blob_encrypt(struct sm_key_blob *key_blob)
{
  int ret = 0;
  u8* crypto_buffer = NULL;

  crypto_buffer = kmalloc(MAX_BLOB_LENGTH, GFP_KERNEL);
  if (!crypto_buffer)
  {
    ret = -ENOMEM;
    goto exit;
  }
  memset(crypto_buffer, 0 , MAX_BLOB_LENGTH);

  key_display(ksdev, "plain data:", key_blob->length, key_blob->buffer);

  ret = sm_keystore_slot_load(ksdev, key_blob->_internal_unit,
                                      key_blob->_internal_keyslot,
                                        key_blob->buffer, key_blob->length);

  if (ret)
  {
    pr_err("sm_keystore_slot_load failed with %d\n", ret);
    goto exit;
  }

  ret = sm_keystore_slot_export(ksdev, key_blob->_internal_unit,
                                        key_blob->_internal_keyslot,
                                        RED_KEY, KEY_COVER_ECB, crypto_buffer,
                                            key_blob->length, skeymod);

  if (ret)
  {
    pr_err("sm_keystore_slot_export failed with %d\n", ret);
    goto exit;
  }

  key_blob->length = key_blob->length + BLOB_OVERHEAD_BYTE;
  memcpy(key_blob->buffer, crypto_buffer, key_blob->length);

exit:
  if (crypto_buffer)
    kfree(crypto_buffer);

  return ret;
}


static int sm_key_blob_decrypt(struct sm_key_blob *key_blob)
{
  int ret = 0;
  u8* plain_data = NULL;
  uint32_t plain_len = 0;

  plain_data = kmalloc(MAX_BLOB_LENGTH, GFP_KERNEL);
  if (!plain_data)
  {
    ret = -ENOMEM;
    goto out;
  }
  memset(plain_data, 0, MAX_BLOB_LENGTH);

  key_display(ksdev, "red key in blob:", key_blob->length, key_blob->buffer);

  plain_len = key_blob->length - BLOB_OVERHEAD_BYTE;
  sm_keystore_slot_import(ksdev, key_blob->_internal_unit,
                          key_blob->_internal_keyslot, RED_KEY,
                            KEY_COVER_ECB, key_blob->buffer,
                              plain_len, skeymod);

  if (ret)
  {
    pr_err("sm_keystore_slot_import failed with %d\n", ret);
    goto out;
	}

  sm_keystore_slot_read(ksdev, key_blob->_internal_unit,
                                key_blob->_internal_keyslot,
                                  AES_BLOCK_PAD(plain_len), plain_data);
  if (ret)
  {
    pr_err("sm_keystore_slot_read failed with %d\n", ret);
    goto out;
  }

  key_display(ksdev, "restored plain key:",
            AES_BLOCK_PAD(key_blob->length), plain_data);

  /* Copy to out */
  memcpy(key_blob->buffer, plain_data, key_blob->length);

  //Update length to account for padding length
  key_blob->length = key_blob->length - BLOB_OVERHEAD_BYTE;
out:
  if (plain_data)
    kfree(plain_data);

  return ret;
}


int sm_key_blob_init(struct sm_key_blob *key_blob)
{
  u32 units;
  dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec-v4.0");
  if (!dev_node)
  {
    dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec4.0");
    if (!dev_node)
    return -ENODEV;
  }

  pdev = of_find_device_by_node(dev_node);
  if (!pdev)
    return -ENODEV;

  of_node_put(dev_node);

  ctrldev = &pdev->dev;
  ctrlpriv = dev_get_drvdata(ctrldev);
  ksdev = ctrlpriv->smdev;
  kspriv = dev_get_drvdata(ksdev);
  if (kspriv == NULL)
    return -ENODEV;

  units = sm_detect_keystore_units(ksdev);

  if (units < 2)
	 return -ENODEV;

  if (unit < units)
  {
    unit++;
    key_blob->_internal_unit = unit;
  }
  else
  {
    pr_err("no unit available\n");
    return -ENODEV;
  }
  sm_establish_keystore(ksdev, key_blob->_internal_unit);
  sm_keystore_slot_alloc(ksdev, key_blob->_internal_unit,
                AES_BLOCK_PAD(key_blob->length), &key_blob->_internal_keyslot);
  return 0;
}


static int sm_key_blob_deinit(struct sm_key_blob *key_blob)
{
  sm_keystore_slot_dealloc(ksdev, key_blob->_internal_unit, key_blob->_internal_keyslot);
  sm_release_keystore(ksdev, key_blob->_internal_unit);
  unit--;
  return 0;
}


static long sm_key_blob_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  long ret = 0;
  struct sm_key_blob __user *argp = (struct sm_key_blob __user *)arg;
  struct sm_key_blob *key_blob = NULL;

  switch (cmd)
  {

    case IOC_KEY_ENCRYPT:
      key_blob = kmalloc(sizeof(struct sm_key_blob), GFP_KERNEL);
      if (!key_blob)
      {
        pr_err("Failed to allocate memory\n");
        ret = -ENOMEM;
        goto out;
      }
      if (copy_from_user(key_blob, argp, sizeof(struct sm_key_blob)))
      {
        pr_err("Failed to copy struct from user\n");
        ret = -EACCES;
        goto out;
      }
      //Init
      ret = sm_key_blob_init(key_blob);
      if (ret)
      {
        pr_err("Failed to initalize\n");
        goto out;
      }
      //********** Encrypt ******/
      if (key_blob->length > MAX_DATA_LENGTH)
      {
        ret = -EFAULT;
        pr_err("unexpected key length value\n");
        goto out;
      }
      //Account for the padding
      if (key_blob->length + BLOB_OVERHEAD_BYTE > MAX_BLOB_LENGTH)
      {
        ret = -ENOMEM;
        pr_err("Unexpected blob length value\n");
        goto out;
      }

      ret = sm_key_blob_encrypt(key_blob);
      if (ret)
      {
        pr_err("Failed to encrypt blob\n");
        goto out;
      }
      if (copy_to_user(argp, key_blob, sizeof(struct sm_key_blob)))
      {
        pr_err("Failed to struct to user\n");
        ret = -EACCES;
        goto out;
      }
      // Deinit
      ret = sm_key_blob_deinit(key_blob);
      if (ret)
      {
        pr_err("Failed to deinitalize\n");
        goto out;
      }
      break;

    case IOC_KEY_DECRYPT:
      key_blob = kmalloc(sizeof(struct sm_key_blob), GFP_KERNEL);
      if (!key_blob)
      {
        ret = -ENOMEM;
        goto out;
      }
      if (copy_from_user(key_blob, argp, sizeof(struct sm_key_blob)))
      {
        ret = -EACCES;
        goto out;
      }
      //In len has acounted for the padding
      if (key_blob->length > MAX_BLOB_LENGTH)
      {
        ret = -EFAULT;
        pr_err("Unexpected blob length value\n");
        goto out;
      }
      //Account for the padding
      if (key_blob->length - BLOB_OVERHEAD_BYTE > MAX_DATA_LENGTH)
      {
        ret = -EFAULT;
        pr_err("Unexpected key length value\n");
        goto out;
      }
      // Init
      key_blob->length -= BLOB_OVERHEAD_BYTE; //Init with plain length
      ret = sm_key_blob_init(key_blob);
      if (ret)
      {
        pr_err("Failed to initalize\n");
        goto out;
      }
      key_blob->length += BLOB_OVERHEAD_BYTE; //Restore
      //Decrypt
      ret = sm_key_blob_decrypt(key_blob);
      if (ret)
      {
        pr_err("Failed to decrypt\n");
        goto out;
      }
      if (copy_to_user(argp, key_blob, sizeof(struct sm_key_blob)))
      {
        pr_err("Failed to struct to user\n");
        ret = -EACCES;
        goto out;
      }
      // Deinit
      ret = sm_key_blob_deinit(key_blob);
      if (ret)
      {
        pr_err("Failed to deinitalize\n");
        goto out;
      }
      break;
    default:
      pr_info("[sm_key_blob_ioctl]: Invalid ioctl!\n");
			ret = -EINVAL;
  }

out:
  if (key_blob)
  {
    kfree(key_blob);
  }
  return ret;
}


static int sm_key_blob_open(struct inode *inode, struct file *file)
{
  return 0;
}


static int sm_key_blob_release(struct inode *inode, struct file *file)
{
  return 0;
}

static int sm_key_blob_mmap(struct file *file, struct vm_area_struct *vma)
{
        unsigned long sm_key_blob_addr;

        if (vma->vm_end - vma->vm_start != PAGE_SIZE)
                return -EINVAL;

        if (vma->vm_flags & VM_WRITE)
                return -EPERM;

        if (PAGE_SIZE > (1 << 16))
                return -ENOSYS;

        vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

        sm_key_blob_addr = __pa(CAAM_SM_ADDRESS);
        sm_key_blob_addr &= ~(PAGE_SIZE - 1);
        sm_key_blob_addr &= 0xfffffffffffffffUL;

        if (io_remap_pfn_range(vma, vma->vm_start, sm_key_blob_addr >> PAGE_SHIFT,
                                        PAGE_SIZE, vma->vm_page_prot)) {
                pr_err("remap_pfn_range failed in sm_key_blob\n");
                return -EAGAIN;
        }

        return 0;
}
static const struct file_operations sm_key_blob_fops = {
	   .owner		= THIS_MODULE,
	   .unlocked_ioctl = sm_key_blob_ioctl,
	   .open		 = sm_key_blob_open,
	   .release	   = sm_key_blob_release,
	   .mmap	= sm_key_blob_mmap,
};

static void __exit sm_key_blob_drv_exit(void)
{
  cdev_del(sm_key_blob_cdev);
	device_destroy(sm_key_blob_class, sm_key_blob_dev);
	class_destroy(sm_key_blob_class);
	unregister_chrdev_region(sm_key_blob_dev, 1);
}

static int sm_key_blob_setup_cdev(void)
{
	  int ret = 0;

	  sm_key_blob_cdev = cdev_alloc();

	  if (sm_key_blob_cdev == NULL) {
		pr_err("Could not allocate cdev structure\n");
		return -ENODEV;
	  }

	  sm_key_blob_cdev->owner = THIS_MODULE;
	  sm_key_blob_cdev->ops = &sm_key_blob_fops;

	  ret = cdev_add(sm_key_blob_cdev, sm_key_blob_dev, 1);

	  if (ret < 0)
		pr_err("Could not add cdev - %d\n", ret);

	  return ret;
}

static int __init sm_key_blob_drv_init(void)
{
	int ret;
#ifdef KERNEL_DEBUG
	struct sm_key_blob *key_blob;
#endif

	ret = alloc_chrdev_region(&sm_key_blob_dev, 0, 1, SM_BLOB_KEY_NAME);

	if (ret < 0) {
		pr_err("Could not allocate major number for device %s - %d\n",
			SM_BLOB_KEY_NAME, ret);
	return ret;
	}

	sm_key_blob_class = class_create(THIS_MODULE, SM_BLOB_KEY_NAME);

	if (sm_key_blob_class == NULL) {
		pr_err("Could not create class %s\n", SM_BLOB_KEY_NAME);
		unregister_chrdev_region(sm_key_blob_dev, 1);
	return -ENODEV;
	}

	sm_key_blob_device = device_create(sm_key_blob_class,
				NULL, sm_key_blob_dev, NULL, SM_BLOB_KEY_NAME);

	if (sm_key_blob_device == NULL) {
		pr_err("Could not create device for class %s - %d\n",
			SM_BLOB_KEY_NAME, ret);
		class_destroy(sm_key_blob_class);
		unregister_chrdev_region(sm_key_blob_dev, 1);
	return ret;
	}

	ret = sm_key_blob_setup_cdev();

	if (ret < 0) {
		device_destroy(sm_key_blob_class, sm_key_blob_dev);
		class_destroy(sm_key_blob_class);
		unregister_chrdev_region(sm_key_blob_dev, 1);
	return ret;
	}

#ifdef KERNEL_DEBUG

	key_blob = kmalloc(sizeof(struct sm_key_blob), GFP_KERNEL | GFP_DMA);
	if (key_blob == NULL)
		return -ENOMEM;
	key_blob->length = 512;
	sm_key_blob_init(key_blob);
	sm_key_blob_encrypt(key_blob);
	sm_key_blob_decrypt(key_blob);

	sm_key_blob_deinit(key_blob);
	kfree(key_blob);
#endif
	   return ret;
}

module_init(sm_key_blob_drv_init);
module_exit(sm_key_blob_drv_exit);

MODULE_AUTHOR("Opensynergy GmbH");
MODULE_DESCRIPTION("CAAM RED KEY BLOB encapsulation/Decapsulation APIs");
MODULE_LICENSE("GPL");
