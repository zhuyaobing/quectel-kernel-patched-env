#ifndef __P4_GART_H
#define __P4_GART_H

/* CONFIG_GART_IOMMU = n */
/* taken from x86 */

#define gart_iommu_aperture            0
#define gart_iommu_aperture_allowed    0
#define gart_iommu_aperture_disabled   1

static inline void early_gart_iommu_check(void)
{
}
static inline void gart_iommu_init(void)
{
}
static inline void gart_iommu_shutdown(void)
{
}
static inline void gart_parse_options(char *options)
{
}
static inline void gart_iommu_hole_init(void)
{
}

#endif
