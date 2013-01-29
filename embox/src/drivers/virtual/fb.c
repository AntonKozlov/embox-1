/**
 * @file
 * @brief Creates file /dev/fbX
 *
 * @date 29.01.13
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <fs/file_operation.h>
#include <fs/node.h>
#include <embox/device.h>
#include <drivers/video/display.h>
#include <errno.h>
#include <string.h>

static struct kfile_operations fb_ops; /* forward declaration */

EMBOX_DEVICE("fb0", &fb_ops, NULL);

static int fb_open(struct node *node, struct file_desc *file_desc, int flags) {
	const struct display *displ;

	displ = display_get_default();
	if (displ == NULL) {
		return -ENODEV;
	}

	file_desc->file_info = (void *)displ;

	return 0;
}

static int fb_close(struct file_desc *desc) {
	return 0;
}

static size_t fb_read(struct file_desc *desc, void *buf, size_t size) {
	const struct display *displ;
	size_t available;

	displ = (struct display *)desc->file_info;
	available = displ->width * displ->height
			* (displ->mode & DISPLAY_MODE_DEPTH16 ? 2
				: displ->mode & DISPLAY_MODE_DEPTH32 ? 4 : 1);
	size = size < available ? size : available;
	memcpy(buf, displ->vga_regs, size);
	return size;
}

static size_t fb_write(struct file_desc *desc, void *buf, size_t size) {
	const struct display *displ;

	displ = (struct display *)desc->file_info;
	memcpy(displ->vga_regs, buf, size);
	return size;
}

#if 0
static int fb_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}
#endif

static struct kfile_operations fb_ops = {
	.open = fb_open,
	.close = fb_close,
	.read = fb_read,
	.write = fb_write,
//	.ioctl = fb_ioctl
};
