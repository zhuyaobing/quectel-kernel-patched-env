#
# Makefile for the CAAM backend and dependent components
#
ifeq ($(CONFIG_CRYPTO_DEV_FSL_CAAM_DEBUG), y)
	EXTRA_CFLAGS := -DDEBUG
endif

obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM) += caam.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_JR) += caam_jr.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_CRYPTO_API) += caamalg.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_AHASH_API) += caamhash.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_RNG_API) += caamrng.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_SM) += sm_store.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_SM_TEST) += sm_test.o
obj-$(CONFIG_CRYPTO_DEV_FSL_CAAM_SECVIO) += secvio.o
obj-m += sm_key_blob.o

caam-objs := ctrl.o
caam_jr-objs := jr.o key_gen.o error.o
