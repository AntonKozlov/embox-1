/**
 * @file
 *
 * @date 12.09.11
 * @author Anton Kozlov 
 */

#include <types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <util/math.h>
#include <fs/fs_drv.h>
#include <kernel/file.h>
#include <fs/ioctl.h>
#include <fs/file_desc.h>
#include <kernel/thread/api.h>
#include <lib/linenoise.h>
#include <drivers/tty_ng.h>
#include <embox/unit.h>
#include <fcntl.h>
#include <kernel/diag.h>

#include <fs/posix.h>

EMBOX_UNIT_INIT(tty_ng_manager_init);

struct param {
	struct tty_buf *tty;
	void (*run)(void);
};

static int _canon_read(void *data, void *buf, size_t size);
static int _write(void *data, const void *buf, size_t nbyte);
static int _ioctl(void *data, int request, va_list args);
static int _close (void *data);

const struct task_res_ops task_res_ops_tty = {
	.close = _close,
	.read = _canon_read,
	.write = _write,
	.ioctl = _ioctl,
	.type = TASK_RES_OPS_TTY,
};

static struct tty_buf *current_tty;

static size_t _read(void *buf, size_t size, void *data) {
	char *ch_buf = (char *) buf;

	struct tty_buf *tty = (struct tty_buf *) data;

	int i = size;

//	mutex_lock(tty->inp_mutex);

	while (i--) {
		while (tty->inp_len == 0) {
//			mutex_unlock(tty->inp_mutex);
			sleep(0);
//			mutex_lock(tty->inp_mutex);
		}

		tty->inp_len -= 1;
		*(ch_buf++) = tty->inp[tty->inp_begin]; 
		tty->inp_begin = (tty->inp_begin + 1) % TTY_INP_Q_LEN;

//		mutex_unlock(tty->inp_mutex);
	}
	return size;
}

static int _canon_read(void *data, void *buf, size_t size) {
	struct tty_buf *tty = (struct tty_buf *) data;
	if (tty->canonical) {
		int to_write;
		if (tty->canon_left == 0) {
			tty->canon_left = linenoise("", tty->canon_inp, TTY_CANON_INP_LEN, NULL, NULL); 
			tty->canon_pos = 0;
		}
		to_write = min(size, tty->canon_left);
		memcpy(buf, tty->canon_inp + tty->canon_pos, to_write);
		tty->canon_left -= to_write;
		tty->canon_pos += to_write;
		return to_write;
	}	
	return _read(buf, size, data);
}

static int _write(void *data, const void *buf, size_t size) {
	size_t cnt = 0;
	char *b = (char*) buf;
	struct tty_buf *tty = (struct tty_buf *) data; 
	while (cnt != size) {
		tty->putc(tty, b[cnt++]);
	}
	return size;
}

static void tty_putc_buf(struct tty_buf *tty, char ch) {
	  
//	mutex_lock(tty->inp_mutex);

	while (tty->inp_len >= TTY_INP_Q_LEN) {
//		mutex_unlock(tty->inp_mutex);
		sleep(0);
//		mutex_lock(tty->inp_mutex);
	}

	tty->inp[tty->inp_end] = ch;
	tty->inp_end = (tty->inp_end + 1) % TTY_INP_Q_LEN;
	tty->inp_len += 1;

//	mutex_unlock(tty->inp_mutex);
}

static void *thread_handler(void* args) {
	struct param *p = (struct param *) args;
	struct idx_desc *cidx = task_idx_desc_alloc(&task_res_ops_tty, p->tty);
	
	close(0);
	close(1);
	close(2);
	
	task_self_idx_set(0, cidx);
	task_self_idx_set(1, cidx);
	task_self_idx_set(2, cidx);

	p->run();
	return NULL;
}	

static int _ioctl(void *data, int request, va_list args) {
	struct tty_buf *tty = (struct tty_buf *) data; 
	switch (request) {
	case TTY_IOCTL_SET_RAW:
		tty->canonical = 0;
		break;
	case TTY_IOCTL_SET_CANONICAL:
		tty->canonical = 1;
		break;
	case TTY_IOCTL_REQUEST_MODE:
		return tty->canonical;
	default:
		break;
	}
	return 0;
}

static int _close (void *data) {
	return 0;
}

static void tty_init(struct tty_buf *tty) {
	tty->inp_begin = tty->inp_end = tty->inp_len = 0;

	tty->canon_pos = tty->canon_left = 0;

	tty->canonical = 1;
}

extern fs_drv_t *devfs_get_fs(void);

void tty_ng_manager(int count, void (*init)(struct tty_buf *tty), void (*run)(void)) {
	struct thread *thds[count];
	struct tty_buf ttys[count];
	struct param params[count];
	char ch;

	if (count <= 0) {
		return;
	}

	for (size_t i = 0; i < count; i++) {
		init(&ttys[i]);
		tty_init(&ttys[i]);

		params[i].tty = &ttys[i];
		params[i].run = run;
		thread_create(&thds[i], THREAD_FLAG_IN_NEW_TASK, thread_handler, &params[i]);
	}

	current_tty = &ttys[0];
	current_tty->make_active(current_tty);

	//fioctl(stdin, O_NONBLOCK_SET, NULL);
	while (1) {
		//diag_putc('!');
		while (-EAGAIN == read(0, &ch, 1)) {
			sleep(0);
		}
		if (ch == '`') {
			while (-EAGAIN == read(0, &ch, 1)) {
				sleep(0);
			}
			if ('0' <= ch && ch <= '9') {
				int n = ch - '0';
				if (n < count) {
					current_tty->make_inactive(current_tty);
					current_tty = &ttys[n];
					current_tty->make_active(current_tty);
				}
				continue;
			}
		}

		tty_putc_buf(current_tty, ch);
	}
}

static int tty_ng_manager_init(void) {
	return 0;
}

