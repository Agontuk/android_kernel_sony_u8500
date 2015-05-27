zram-y	:=	zcomp.o zram_drv.o

zram-$(CONFIG_ZRAM_LZO_COMPRESS) += zcomp_lzo.o
zram-$(CONFIG_ZRAM_LZ4_COMPRESS) += zcomp_lz4.o
zram-$(CONFIG_ZRAM_SNAPPY_COMPRESS) += zcomp_snappy.o

obj-$(CONFIG_ZRAM)	+=	zram.o
