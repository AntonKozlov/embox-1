/**
 * @file 
 * @brief SD-Card driver for STM32F4-Discovery using SDIO.
 *
 * @author  Denis Deryugin
 * @date    26 Jan 2015
 */

#include <string.h>

#include <embox/block_dev.h>
#include "stm32f4_discovery_sdio_sd.h"
#include <kernel/time/ktime.h>
#include <kernel/thread/sync/mutex.h>

static int stm32f4_sd_init(void *arg);
static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size);
static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);
static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno);

#define STM32F4_SD_DEVNAME "stm32f4_sd_card"

block_dev_driver_t stm32f4_sd_driver = {
	.name = STM32F4_SD_DEVNAME,
	.ioctl = stm32f4_sd_ioctl,
	.read = stm32f4_sd_read,
	.write = stm32f4_sd_write,
};

EMBOX_BLOCK_DEV(STM32F4_SD_DEVNAME, &stm32f4_sd_driver, stm32f4_sd_init);

static int stm32f4_sd_init(void *arg) {
	block_dev_t *bdev;
	if (block_dev_lookup(STM32F4_SD_DEVNAME)) {
		bdev = block_dev_create("/dev/" STM32F4_SD_DEVNAME, &stm32f4_sd_driver, NULL);
		bdev->size = 2048 * 1024; /* XXX Hardcode */
		return -SD_Init();
	}
	return -1;
}

static int stm32f4_sd_ioctl(struct block_dev *bdev, int cmd, void *buf, size_t size) {
	switch (cmd) {
	case IOCTL_GETDEVSIZE:
		return 2048 * 1024; /* XXX Hardcode */
	case IOCTL_GETBLKSIZE:
		return 512;	/* XXX Hardcode */
	default:
		return -1;
	}
}

static struct mutex stm32f4_sd_mutex;

static int stm32f4_sd_read(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	int res;
	mutex_lock(&stm32f4_sd_mutex);
	while (SD_GetTransferState() != SD_TRANSFER_OK);
	res = SD_ReadBlock((uint8_t*) buf, blkno, SECTOR_SIZE) ? -1 : SECTOR_SIZE;
	mutex_unlock(&stm32f4_sd_mutex);
	return res;
}

static int stm32f4_sd_write(struct block_dev *bdev, char *buf, size_t count, blkno_t blkno) {
	int res;
	mutex_lock(&stm32f4_sd_mutex);
	res =  SD_WriteBlock((uint8_t*) buf, blkno, SECTOR_SIZE) ? -1 : SECTOR_SIZE;
	mutex_unlock(&stm32f4_sd_mutex);
	return res;
}

