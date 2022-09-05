#include <linux/pci.h>
#include <linux/init.h>
#include <asm/iomem.h>
#include <asm/vmapi_calls.h>
#include <asm/pikeos_version.h>

#ifdef DEBUG
#define DPRINTK(x...)               pr_debug("vPCI: " x)
#else
#define DPRINTK(x...)               do {} while(0)
#endif

#ifdef CONFIG_PCI_DOMAINS
#define HOST_CTRL_MAX               3
#else
#define HOST_CTRL_MAX               1
#endif

#define P4_VIRTUAL_ROOT_BUS         0
#define P4_PREFIX                   "pci:"
#define P4_PREFIX_LEN               4
#define P4_NAME_LEN                 128
#define MAX_BUSSES                  255
#define MAX_DEVICES                 32
#define MAX_FUNCTIONS               8
#define INVALID_FD                  -1

typedef struct bar_to_virt_str {
	struct list_head map_list;
	unsigned long bar;
	unsigned long virt;
	unsigned long size;
} bar_to_virt_t;

static LIST_HEAD(bar_map_list);

static int prop_devs(int *fds, unsigned int *dev_nums);
static int iterate_devs(int *fds, unsigned int *dev_nums);
static int scan_devs(int *fds, unsigned int *dev_nums);

/** see create_bridged_child_bus() hack */
extern struct rw_semaphore pci_bus_sem;

/** called by iomem_find_io when static mappings (VMIT) are not present */
extern unsigned long p4_pci_find_bar(unsigned long bar, unsigned long size)
{
	bar_to_virt_t *m;
	struct list_head *item;

	list_for_each(item, &bar_map_list) {
		m = list_entry(item, bar_to_virt_t, map_list);
		if ((bar >= m->bar) &&
			((bar + size) <= (m->bar + m->size)))
			/* fix offset, bar may refer to any address within an actual BAR, return nearest 4K boundary */
			return (m->virt + bar - m->bar);
	}
	return 0;
}

/** called by ioremap to check if mapping is a PCI bar */
extern int p4_pci_is_bar_mapping(unsigned long virt)
{
	bar_to_virt_t *m;
	struct list_head *item;

	list_for_each(item, &bar_map_list) {
		m = list_entry(item, bar_to_virt_t, map_list);
		if ((virt >= m->virt) &&
			(virt <= (m->virt + m->size)))
			return 1;
	}
	return 0;
}

static int p4_pci_read(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 *value)
{
	int *fds = bus->sysdata;
	return vmapi_pci_read_config(fds[devfn], (unsigned int)where,
				(unsigned int)size, value);
}

static int p4_pci_write(struct pci_bus *bus, unsigned int devfn,
	int where, int size, u32 value)
{
	int *fds = bus->sysdata;
	return vmapi_pci_write_config(fds[devfn], (unsigned int)where,
				(unsigned int)size, value);
}

static struct pci_ops p4_pci_root_ops = {
	.read = p4_pci_read,
	.write = p4_pci_write,
};

static void __init p4_allocate_resources(struct pci_dev *dev, int fd)
{
	int i;
	struct resource *res;
	unsigned long virt;
	bar_to_virt_t *mapping;

	/** allocate 5 standard resources + EEPROM expansion */
	for (i=PCI_STD_RESOURCES; i<=PCI_ROM_RESOURCE; i++)
	{
		res = &dev->resource[i];
		if (!res->flags)
			continue;

		/**
		* Add resources to bridge before claiming them. They should
		* already be added to the bus when the bus is created with a bridge.
		* Since we created in 2 steps (bus then bridge), the bus is empty
		* from a resources point of view
		*/
		if (!pci_find_parent_resource(dev, res))
			pci_bus_add_resource(dev->bus, res, res->flags);

		if (!res->start || pci_claim_resource(dev, i) < 0)
		{
			dev_err(&dev->dev, "could not claim resource %pR\n", res);
			res->start = 0;
			res->end = 0;
			res->flags = 0;
		}
		else if (vmapi_pci_map_resource(fd, i, res, &virt) < 0)
		{
			dev_err(&dev->dev, "could not map resource %pR\n", res);
		}
		else if (virt != 0)
		{
			/** store mappings for ioremap calls */ 
			mapping = kmalloc(sizeof(bar_to_virt_t), GFP_KERNEL);
			if (mapping != NULL)
			{
				mapping->bar = ((unsigned long)res->start) & PAGE_MASK;
				mapping->virt = virt;
				mapping->size = roundup(resource_size(res), PAGE_SIZE);
				list_add(&mapping->map_list, &bar_map_list);
				dev_dbg(&dev->dev, "add resource 0x%lx <-> 0x%lx (0x%lx)\n",
						mapping->virt, mapping->bar, mapping->size);
			}
			else
			{
				dev_err(&dev->dev, "could not save mapping: no memory available\n");
			}
		}
	}
}

static struct pci_bus *create_bridged_child_bus(struct pci_bus *parent, 
	int bus_id, void *bus_data)
{
	struct pci_bus *child;
	struct pci_dev *bridge;

	/** create child bus from parent one */
	child = pci_add_new_bus(parent, NULL, bus_id);
	if (NULL != child)
	{
		/** fix inherit specific data */
		child->sysdata = bus_data;
		DPRINTK("created new child bus(%d)\n", bus_id);

		/** create a virtual bridge */
		bridge = alloc_pci_dev();

		/** add bridge to parent (pci_device_add()-like) */
		down_write(&pci_bus_sem);
		list_add_tail(&bridge->bus_list, &parent->devices);
		up_write(&pci_bus_sem);

		/** link to child */
		bridge->subordinate = child;
		DPRINTK("bridged bus(%d) <-> bus(%d)\n", parent->number, bus_id);

		/** dirty hack to prevent any initilization accessing hardware */
		bridge->is_added = 1;
		bridge->hdr_type = PCI_HEADER_TYPE_BRIDGE;
		bridge->class = PCI_CLASS_BRIDGE_PCI << 8;
		atomic_set(&bridge->enable_cnt, 1);
	}
	return child;
}

static void get_devices_fd(int *fds, unsigned int *dev_nums)
{
    int done;

	/** let's discover PCI devices */
	printk(KERN_INFO "vPCI: Probing PCI hardware\n");

	/*
	 * Use 1st available method:
	 * 1. Device list in property file system
	 * 2. PCI Manager device iteration (PikeOS 3.4 only)
	 * 3. Scan all possible devices
	 */
    done = prop_devs(fds, dev_nums);
    if (done == 0) {
        done = iterate_devs(fds, dev_nums);
    }
    if (done == 0) {
        done = scan_devs(fds, dev_nums);
    }
    return;
}

static int prop_devs(int *fds, unsigned int *dev_nums)
{
	int err, ctrl_id, bus_id, tmp_fd, i, fd;
	char prop_name[P4_NAME_LEN], pci_name[P4_NAME_LEN],
		name[P4_NAME_LEN];
	unsigned int devfn, ctrl_dev_num;

	/** check if there are configured devices in property tree */
	sprintf(prop_name, "prop:/app/%s/devices", vmapi_get_partname());
	fd = vmapi_file_open(prop_name, VMAPI_O_R, NULL);
	if (fd < 0)
	{
	    return 0;
	}

    DPRINTK("from the property tree only\n");
    for (i=0; ; i++)
    {
        err = vmapi_file_read_at(fd, name, sizeof(name), i);
        if (err <= 0) {
            /* no more elements, abort */
            break;
        }

        if (err > 0) {
            sprintf(prop_name, "prop:/app/%s/devices/%s", vmapi_get_partname(),
		    name);
            err = vmapi_property_read(prop_name, pci_name, sizeof(pci_name));
        }

        if (err > 0 && strncmp(pci_name, P4_PREFIX, P4_PREFIX_LEN) == 0)
        {
            /*
             * @TODO
             * Should we loop in case file name contains wildcard?
             * There is common code, needs refactoring
             */
            tmp_fd = vmapi_file_open(pci_name, VMAPI_O_R | VMAPI_O_W, NULL);
            if (tmp_fd >= 0)
            {
                /** retrieve ctrl id, bus id and devfn info */
                /* @TODO can this fail? */
                vmapi_pci_get_info(tmp_fd, &ctrl_id, &bus_id, &devfn);
                ctrl_dev_num = (ctrl_id*VMAPI_MAX_FILES) + dev_nums[ctrl_id];
                fds[ctrl_dev_num] = tmp_fd;
                dev_nums[ctrl_id]++;
                DPRINTK("Found %s: %s\n", name, pci_name);
            }
            else
            {
                pr_err("vPCI: Could not open %s: %s (err=%d)\n", name,
                       pci_name, tmp_fd);
            }
        }
    }
    vmapi_file_close(fd);
    return 1;
}

static int scan_devs(int *fds, unsigned int *dev_nums)
{
    int tmp_fd;
    char pci_name[P4_NAME_LEN];
    unsigned int devfn, ctrl_dev_num, ctrl, bus;

    pr_warn("**WARNING** probing all devices will take some time\n");

    for (ctrl=0; ctrl<HOST_CTRL_MAX; ctrl++)
    {
        for (bus=0; bus<MAX_BUSSES; bus++)
        {
            for (devfn=0; devfn<(MAX_DEVICES*MAX_FUNCTIONS); devfn++)
            {
                sprintf(pci_name, "pci:bybus/%04x/%04x/%04x/%04x",
                    ctrl, bus, PCI_SLOT(devfn), PCI_FUNC(devfn));

                tmp_fd = vmapi_file_open(pci_name, VMAPI_O_R | VMAPI_O_W,
                                        NULL);
                if (tmp_fd >= 0)
                {
                    /** found valid device, save id in ctrl list */
                    ctrl_dev_num = (ctrl*VMAPI_MAX_FILES) + dev_nums[ctrl];
                    fds[ctrl_dev_num] = tmp_fd;
                    dev_nums[ctrl]++;
                    DPRINTK("Found %s\n", pci_name);
                }
                else if (-EMFILE == tmp_fd)
                {
                    pr_err("vPCI: Could not open %s: No more file descriptor "
                        "available\n", pci_name);
                }
                else if (-EPERM == tmp_fd) {
                    DPRINTK("%s skipping...\n", pci_name);
                }
            }
        }
    }
    return 1;
}

static int iterate_devs(int *fds, unsigned int *dev_nums)
{
    int ctrl_id, bus_id, tmp_fd, iter_fd, done;
    char pci_name[P4_NAME_LEN];
    unsigned int devfn, ctrl_dev_num;

    iter_fd = vmapi_file_open("pci:iterate", VMAPI_O_R | VMAPI_O_W, NULL);
    if (iter_fd < 0)
    {
        return 0;
    }
    do {
        /*
         * The iteration device returns fully qualified pathnames that can
         * then be used in open call.
         */
        done = vmapi_pci_iterate(iter_fd, pci_name, sizeof(pci_name));
        if (done == 0) {
            tmp_fd = vmapi_file_open(pci_name, VMAPI_O_R | VMAPI_O_W, NULL);
            if (tmp_fd >= 0)
            {
                /** retrieve ctrl id, bus id and devfn info */
                /* @TODO can this fail? */
                vmapi_pci_get_info(tmp_fd, &ctrl_id, &bus_id, &devfn);
                ctrl_dev_num = (ctrl_id*VMAPI_MAX_FILES) + dev_nums[ctrl_id];
                fds[ctrl_dev_num] = tmp_fd;
                dev_nums[ctrl_id]++;
                DPRINTK("Found %s\n", pci_name);
            }
        }
    } while (done == 0);

    vmapi_file_close(iter_fd);
    return 1;
}

static int host_controller_init(int *ctrl_fds, unsigned int dev_num)
{
	int ctrl_id, bus_id, id, devfn, fd, *fds;
	struct pci_bus *bus, *root;
	struct pci_dev *pdev;
	unsigned int i, dev_added;
	LIST_HEAD(resources);

	/** create virtual PCI bus */
	bus_id = P4_VIRTUAL_ROOT_BUS;
	fds = (int*)kmalloc(MAX_DEVICES * MAX_FUNCTIONS * sizeof(int), GFP_KERNEL);
	if (!fds)
		return -ENOMEM;

	x86_pci_root_bus_resources(bus_id, &resources);
	root = pci_create_root_bus(NULL, bus_id, &p4_pci_root_ops, fds, &resources);
	if (!root)
	{
		/** 
		* this is not an error if 'pci_domain_nr()' returns a value not 
		* handled by HOST_CTRL_MAX. this function is architecture dependent 
		* and must be implemented in arch-XXX. Check 'pci_root_buses' variable.
		*/
		kfree(fds);
		pci_free_resource_list(&resources);
		return -1;
	}
	DPRINTK("created P4 virtual root bus\n");

	bus = root;
	for (dev_added=0; dev_added<dev_num; )
	{
		for (i=0; i<VMAPI_MAX_FILES; i++)
		{
			fd = ctrl_fds[i];
			if (INVALID_FD != fd)
			{
				/** retrieve ctrl id, bus id and devfn info */
				vmapi_pci_get_info(fd, &ctrl_id, &id, &devfn);
				if (id == bus_id)
				{
					/** save fd */
					fds[devfn] = fd;
					/** add newly discovered device to 'bus->devices' list */
					pdev = pci_scan_single_device(bus, devfn);
					/** resources MUST not be re-allocated, claim them all */
					p4_allocate_resources(pdev, fd);
					/** grant IRQ for "request_irq()" to succeed */
					if (vmapi_pci_request_irq(fd, pdev) != 0)
						printk("vPCI: could not grant IRQ\n");

					dev_info(&pdev->dev, "new device on bus %d with devfn " \
						"0x%X added", id, devfn);
					dev_added++;
				}
			}
		}

		bus_id++;
		fds = (int*)kmalloc(MAX_DEVICES*MAX_FUNCTIONS*sizeof(int), GFP_KERNEL);
		if (!fds)
			return -ENOMEM;

		bus = create_bridged_child_bus(root, bus_id, fds);
		if (!bus)
		{
			/** if any severe problems occur, break when bus_id equal 255 */
			kfree(fds);
			pr_err("vPCI: Could not allocate child bus\n");
			return -1;
		}
	}

	/** add devices list to the global PCI device list */
	DPRINTK("adding devices to the global PCI list\n");
	pci_bus_add_devices(root);

	return 0;
}

int __init p4_pci_init(void)
{
	int fd[VMAPI_MAX_FILES*HOST_CTRL_MAX], i, ret;
	unsigned int dev_num[HOST_CTRL_MAX];

	/** 
	* some architectures (x86), try to re-assigned resources
	* giving them a "chance" to be detected (like ROM expansion)
	* disable this since we will allocate everything ourself
	*/
	pcibios_setup("rom");

	memset(fd, INVALID_FD, sizeof(fd));
	memset(dev_num, 0, sizeof(dev_num));

	get_devices_fd(fd, dev_num);
	for (i=0; i<HOST_CTRL_MAX; i++)
	{
		ret = host_controller_init(&fd[i*VMAPI_MAX_FILES], dev_num[i]);
		if (0 != ret)
			continue;
	}

	return 0;
}
arch_initcall(p4_pci_init);

