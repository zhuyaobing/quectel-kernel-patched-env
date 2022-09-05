#include <linux/io.h>
#include <linux/pci.h>
#include <asm/pci.h>

#ifdef CONFIG_PCI_VIRT
/** unneeded for virtual PCI bus */
int pci_legacy_init(void)
{
    return 0;
}
#endif

static const struct vm_operations_struct pci_mmap_ops = {
	.access = generic_access_phys,
};

int pci_mmap_page_range(struct pci_dev *dev, struct vm_area_struct *vma,
			enum pci_mmap_state mmap_state, int write_combine)
{
	unsigned long prot;

	/* I/O space cannot be accessed via normal processor loads and
	 * stores on this platform.
	 */
	if (mmap_state == pci_mmap_io)
		return -EINVAL;

	prot = pgprot_val(vma->vm_page_prot);

	/* we don't suppport write combine and PAT */
	if (write_combine)
		return -EINVAL;

#if 0
	/*
 	 * Return error if pat is not enabled and write_combine is requested.
 	 * Caller can followup with UC MINUS request and add a WC mtrr if there
 	 * is a free mtrr slot.
 	 */
	if (!pat_enabled && write_combine)
		return -EINVAL;

	if (pat_enabled && write_combine)
		prot |= _PAGE_CACHE_WC;
	else if (pat_enabled || boot_cpu_data.x86 > 3)
		/*
		 * ioremap() and ioremap_nocache() defaults to UC MINUS for now.
		 * To avoid attribute conflicts, request UC MINUS here
		 * as well.
		 */
		prot |= _PAGE_CACHE_UC_MINUS;
#endif
	prot |= _PAGE_IOMAP;	/* creating a mapping for IO */

	vma->vm_page_prot = __pgprot(prot);

	if (io_remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
			       vma->vm_end - vma->vm_start,
			       vma->vm_page_prot))
		return -EAGAIN;

	vma->vm_ops = &pci_mmap_ops;

	return 0;
}
