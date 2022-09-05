#include <linux/pci.h>
#include <linux/init.h>
#include <asm/iomem.h>
#include <asm/vmapi_calls.h>
#include <kernel/p4pci_types.h>
#include <linux/irq.h>

static int pci_msi_enable = 1;

void pci_restore_msi_state(struct pci_dev *dev) {

}

/**
  * pci_msix_table_size - return the number of device's MSI-X table entries
  * @dev: pointer to the pci_dev data structure of MSI-X device function
  */
int pci_msix_table_size(struct pci_dev *dev)
{
    int *fds = dev->bus->sysdata;
    int ret;
    unsigned int msix_avail;

    ret = vmapi_pci_msi_msix_stat(fds[dev->devfn], &msix_avail, NULL);

    if (ret == 0) {
        ret = msix_avail;
    } else {
        ret = 0;
    }
    return ret;
}

/**
  * pci_msi_vec_count - Return the number of MSI vectors a device can send
  * @dev: device to report about
  *
  * This function returns the number of MSI vectors a device requested via
  * Multiple Message Capable register. It returns a negative errno if the
  * device is not capable sending MSI interrupts. Otherwise, the call succeeds
  * and returns a power of two, up to a maximum of 2^5 (32), according to the
  * MSI specification.
**/

int pci_msi_vec_count(struct pci_dev *dev)
{
    int *fds = dev->bus->sysdata;
    int ret;
    unsigned int msi_avail;

    ret = vmapi_pci_msi_msix_stat(fds[dev->devfn], NULL, &msi_avail);

    if (ret == 0) {
        if (msi_avail > 0)
            ret = msi_avail;
        else {
            ret = -EINVAL;
        }
    }

    return ret;
}
EXPORT_SYMBOL(pci_msi_vec_count);

/**
 * pci_enable_msi_block - configure device's MSI capability structure
 * @dev: device to configure
 * @nvec: number of interrupts to configure
 *
 * Allocate IRQs for a device with the MSI capability.
 * This function returns a negative errno if an error occurs.  If it
 * is unable to allocate the number of interrupts requested, it returns
 * the number of interrupts it might be able to allocate.  If it successfully
 * allocates at least the number of interrupts requested, it returns 0 and
 * updates the @dev's irq member to the lowest new interrupt number; the
 * other interrupt numbers allocated to this device are consecutive.
 */
int pci_enable_msi_block(struct pci_dev *dev, unsigned int nvec)
{
	int ret;
	int *fds;
	unsigned int base;

	WARN_ON(!!dev->msi_enabled);

	if (!pci_msi_enable)
		return -EINVAL;

	/* Check whether driver already requested MSI-X irqs */
	if (dev->msix_enabled) {
		dev_info(&dev->dev, "can't enable MSI "
			 "(MSI-X already enabled)\n");
		return -EINVAL;
	}

	fds = dev->bus->sysdata;

	ret = vmapi_pci_msi_enable(fds[dev->devfn], &base, nvec);

	if (ret == 0) {
		struct irq_desc *d;
		unsigned long flags;
		int i;
		/* if we got what we wanted */
		dev->msi_enabled = 1;
		dev->irq = base;

		/* set IRQ name */
		for (i=0;i<nvec;i++) {
			d = irq_to_desc(base + i);
			raw_spin_lock_irqsave(&d->lock, flags);
			d->name = "MSI";
			raw_spin_unlock_irqrestore(&d->lock, flags);
		}
	}

	return ret;
}
EXPORT_SYMBOL(pci_enable_msi_block);

int pci_enable_msi_block_auto(struct pci_dev *dev, unsigned int *maxvec)
{
	int ret, nvec;

	if (!pci_msi_enable)
		return -EINVAL;

	nvec = pci_msi_vec_count(dev);

	if (nvec < 0)
		return nvec;

	if (maxvec)
		*maxvec = nvec;

	ret = pci_enable_msi_block(dev, nvec);

	if (ret != 0)
		return -EINVAL;
	return nvec;
}
EXPORT_SYMBOL(pci_enable_msi_block_auto);

void pci_disable_msi(struct pci_dev *dev)
{
	pci_msi_shutdown(dev);
}

EXPORT_SYMBOL(pci_disable_msi);

/**
 * pci_enable_msix - configure device's MSI-X capability structure
 * @dev: pointer to the pci_dev data structure of MSI-X device function
 * @entries: pointer to an array of MSI-X entries
 * @nvec: number of MSI-X irqs requested for allocation by device driver
 *
 * Setup the MSI-X capability structure of device function with the number
 * of requested irqs upon its software driver call to request for
 * MSI-X mode enabled on its hardware device function. A return of zero
 * indicates the successful configuration of MSI-X capability structure
 * with new allocated MSI-X irqs. A return of < 0 indicates a failure.
 * Or a return of > 0 indicates that driver request is exceeding the number
 * of irqs or MSI-X vectors available. Driver should use the returned value to
 * re-send its request.
 **/
int pci_enable_msix(struct pci_dev *dev, struct msix_entry *entries, int nvec)
{
	int ret;
	int *fds;
	WARN_ON(!!dev->msix_enabled);

	if (!pci_msi_enable)
		return -EINVAL;

	/* Check whether driver already requested for MSI irq */
	if (dev->msi_enabled)  {
		dev_info(&dev->dev, "can't enable MSI-X "
		       "(MSI IRQ already assigned)\n");
		return -EINVAL;
	}

	fds = dev->bus->sysdata;
	ret = vmapi_pci_msix_enable(fds[dev->devfn], entries, nvec);

	if (ret == 0) {
		struct irq_desc *d;
		unsigned long flags;
		int i;
		/* set IRQ name */
		for (i=0;i<nvec;i++) {
			d = irq_to_desc(entries[i].vector);
			raw_spin_lock_irqsave(&d->lock, flags);
			d->name = "MSI-X";
			raw_spin_unlock_irqrestore(&d->lock, flags);
		}
		dev->msix_enabled = 1;
	}

	return ret;
}

EXPORT_SYMBOL(pci_enable_msix);

void pci_msi_shutdown(struct pci_dev *dev)
{
	int *fds;
	
	if (!pci_msi_enable || !dev || !dev->msi_enabled)
		return;

	dev->msi_enabled = 0;
	fds = dev->bus->sysdata;
	vmapi_pci_msi_disable(fds[dev->devfn]);
	dev->irq = vmapi_pci_get_irq(fds[dev->devfn]);
}

void pci_msix_shutdown(struct pci_dev *dev)
{
	int *fds;
	if (!pci_msi_enable || !dev || !dev->msix_enabled)
		return;

	dev->msix_enabled = 0;

	fds = dev->bus->sysdata;
	vmapi_pci_msix_disable(fds[dev->devfn]);
}

void pci_disable_msix(struct pci_dev *dev)
{
	pci_msix_shutdown(dev);
}
EXPORT_SYMBOL(pci_disable_msix);

/**
 * msi_remove_pci_irq_vectors - reclaim MSI(X) irqs to unused state
 * @dev: pointer to the pci_dev data structure of MSI(X) device function
 *
 * Being called during hotplug remove, from which the device function
 * is hot-removed. All previous assigned MSI/MSI-X irqs, if
 * allocated for this device function, are reclaimed to unused state,
 * which may be used later on.
 **/
void msi_remove_pci_irq_vectors(struct pci_dev *dev)
{
	if (!pci_msi_enable || !dev)
		return;

	pci_disable_msi(dev);
	pci_disable_msix(dev);
}

void pci_no_msi(void)
{
	pci_msi_enable = 0;
}

/**
 * pci_msi_enabled - is MSI enabled?
 *
 * Returns true if MSI has not been disabled by the command-line option
 * pci=nomsi.
 **/
int pci_msi_enabled(void)
{
	return pci_msi_enable;
}
EXPORT_SYMBOL(pci_msi_enabled);

void pci_msi_init_pci_dev(struct pci_dev *dev)
{

}

