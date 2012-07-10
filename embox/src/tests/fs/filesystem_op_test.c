/**
 * @file
 * @brief
 *
 * @date 05.07.2012
 * @author Andrey Gazukin
 */

#include <fcntl.h>
#include <cmd/mkfs.h>
#include <cmd/mount.h>
#include <drivers/ramdisk.h>
#include <embox/test.h>
#include <fs/vfs.h>

EMBOX_TEST_SUITE("fs/filesystem test");

static mkfs_params_t mkfs_params;
static ramdisk_params_t ramd_params;
static mount_params_t mount_param;
static fs_drv_t *fs_drv;

TEST_SETUP_SUITE(setup_suite);

TEST_TEARDOWN_SUITE(teardown_suite);

#define FS_NAME  "vfat"
#define FS_DEV  "/dev/ramdisk"
#define FS_TYPE  16
#define FS_BLOCKS  256

TEST_CASE("Create fat filesystem") {

	test_assert_zero(fs_drv->fsop->format((void *)&ramd_params));
}

#define FS_DIR  "/test_fsop"
TEST_CASE("Mount fat filesystem") {

	test_assert_zero(fs_drv->fsop->mount((void *) &mount_param));
}

#define FS_FILE1  "/test_fsop/1/2/3/1.txt"
TEST_CASE("Create file 1") {
	test_assert_zero(creat(FS_FILE1, 0));
}

#define FS_FILE2  "/test_fsop/1/2/2.txt"
TEST_CASE("Create file 2") {
	test_assert_zero(creat(FS_FILE2, 0));
}

TEST_CASE("Delete file") {
	test_assert_zero(remove(FS_FILE1));
	test_assert_zero(remove(FS_FILE2));
	test_assert(0 > open(FS_FILE1, O_RDONLY));
	test_assert(0 > open(FS_FILE2, O_RDONLY));
}

static int setup_suite(void) {
	mkfs_params.blocks = FS_BLOCKS;
	mkfs_params.fs_type = FS_TYPE;
	strcpy((void *)&mkfs_params.fs_name, FS_NAME);
	strcpy((void *)&mkfs_params.path, FS_DEV);

	if (0 != ramdisk_create((void *)&mkfs_params)) {
		return -1;
	}

	/* set filesystem attribute to ramdisk */
	strcpy((void *)ramd_params.path, (const void *)mkfs_params.path);
	strcpy((void *)ramd_params.fs_name,
				(const void *)mkfs_params.fs_name);
	ramd_params.fs_type = mkfs_params.fs_type;

	if(NULL == (fs_drv =
			filesystem_find_drv((const char *) &mkfs_params.fs_name))) {
		return -1;
	}

	mount_param.dev = FS_DEV;
	mount_param.dir = FS_DIR;

	if(NULL ==	(mount_param.dev_node =
			vfs_find_node(mount_param.dev, NULL))) {
		return -1;
	}
	/* set created ramdisc attribute from dev_node */
	memcpy(&ramd_params, mount_param.dev_node->attr, sizeof(ramd_params));

	return 0;
}

static int teardown_suite(void) {

	if (ramdisk_delete(FS_DEV)) {
		return -1;
	}
	return 0;
}
