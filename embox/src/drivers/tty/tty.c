/**
 * @file
 *
 * @brief Serves all tty devices
 *
 * @date 12.11.2010
 * @author Anton Bondarev
 * @author Fedor Burdun
 */

#include <string.h>
#include <drivers/tty.h>
#include <kernel/uart.h>

tty_device_t *cur_tty;
#ifdef CONFIG_TTY_WITH_VTPARSE
struct vtparse tty_vtparse[1];
struct vtbuild tty_vtbuild[1];

const struct vt_token TOKEN_LEFT[1] = {{ 
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'D'
}};

void tty_vtparse_callback(struct vtparse *tty_vtparse, struct vt_token *token) {
#if 0
	printf("action: %d; char: %d %c; attrs_len: %d; params_len: %d\n", token->action , token->ch, token->ch,
		token->attrs_len, token->params_len);
#endif

	switch (token->action) {
		case VT_ACTION_PRINT: 
			if (cur_tty->rx_cur < TTY_RXBUFF_SIZE) {
				cur_tty->rx_buff[cur_tty->rx_cur++] = token->ch;
				if ( cur_tty->rx_cur > cur_tty->rx_cnt ) {
					cur_tty->rx_cnt = cur_tty->rx_cur;
				}
				vtbuild(tty_vtbuild, token);
			}
		break;

		case VT_ACTION_CSI_DISPATCH:
			switch (token->ch) {
				case 'D': /* LEFT */
					if (cur_tty->rx_cur>0) {
						--cur_tty->rx_cur;
						vtbuild(tty_vtbuild, token);
					}
				break;
				case 'C': /* RIGHT */
					if (cur_tty->rx_cur < cur_tty->rx_cnt) {
						++cur_tty->rx_cur;
						vtbuild(tty_vtbuild, token);
					}
				break;
			}
		break;

		case VT_ACTION_EXECUTE:
			switch (token->ch) {
				case '\n':
					if (cur_tty->out_busy) break;
					/*add string to output buffer*/
					memcpy((void*) cur_tty->out_buff,(const void*) 
						cur_tty->rx_buff, cur_tty->rx_cnt);
					cur_tty->out_buff[cur_tty->rx_cnt] = '\0';
					cur_tty->rx_cnt = 0;
					cur_tty->rx_cur = 0;
					cur_tty->out_busy = true;
					vtbuild(tty_vtbuild, token);
				break;

				case 127: /* backspace */
					if (cur_tty->rx_cur>0) {
						--cur_tty->rx_cur;
						vtbuild(tty_vtbuild, TOKEN_LEFT); /* cursor move left */
						/* or use: uart_putc(8); */
						uint32_t i;
						for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt-1; ++i) {
							cur_tty->rx_buff[i] = cur_tty->rx_buff[i+1];
							uart_putc(cur_tty->rx_buff[i]);
						}
						uart_putc(' ');
						for (i=cur_tty->rx_cur; i<cur_tty->rx_cnt; ++i) {
							vtbuild(tty_vtbuild, TOKEN_LEFT); /* cursor move left */
							/* or use: uart_putc(8); */
						}
						--cur_tty->rx_cnt;
					}
				break;

				case 4: /* ^D */
				break;

				default:
				break;
			}
		default:
		break;
	}
}

void tty_vtbuild_callback(struct vtparse *tty_vtbuild, char ch) {
	#warning NOT SURE, THAT IT WORKS FOR ALL PLATFORMS. # uart_putc(ch) 
	uart_putc(ch);
}

static int tty_init_flag = 0;
#endif

int tty_init(void) {
#ifdef CONFIG_TTY_WITH_VTPARSE
	if (NULL == vtparse_init(tty_vtparse, tty_vtparse_callback)) {
		LOG_ERROR("Error while initialization vtparse.\n");
	}
	if (NULL == vtbuild_init(tty_vtbuild, tty_vtbuild_callback)) {
		LOG_ERROR("Error while initialization vtbuild.\n");
	}
	cur_tty->rx_cur = 0;
	cur_tty->rx_cnt = 0;
#endif
	return 0;
}

int tty_register(tty_device_t *tty) {
	cur_tty = tty;
	return 0;
}

int tty_unregister(tty_device_t *tty) {
	tty->out_busy = false;
	tty->rx_cnt = 0;
	cur_tty = tty;
	return 0;
}

int tty_get_uniq_number(void) {
	return 0;
}
/*
 * add parsed char to receive buffer
 */
int tty_add_char(tty_device_t *tty, int ch) {
#ifdef CONFIG_TTY_WITH_VTPARSE
	if (!tty_init_flag) {
		tty_init_flag = 1;
		tty_init();
	}
	vtparse(tty_vtparse, ch);
#else /* CONFIG_TTY_WITH_VTPARSE */
	if ('\n' == ch && !tty->out_busy) {
		/*add string to output buffer*/
		memcpy((void*) tty->out_buff,(const void*) tty->rx_buff, tty->rx_cnt);
		tty->out_buff[tty->rx_cnt] = '\0';
		tty->rx_cnt = 0;
		tty->out_busy = true;
		uart_putc(ch);
		return 1;
	}
	if (tty->rx_cnt >= TTY_RXBUFF_SIZE) {
		/*buffer is full. drop this symbol*/
		return -1;
	}
	tty->rx_buff[tty->rx_cnt++] = ch;
	uart_putc(ch);
	return 0;
#endif /* CONFIG_TTY_WITH_VTPARSE */
}

uint8_t* tty_readline(tty_device_t *tty) {
	while (!tty->out_busy);
	tty->out_busy = false;
	return (uint8_t*) tty->out_buff;
}

static uint32_t tty_scanline(uint8_t line[TTY_RXBUFF_SIZE + 1], uint32_t size) {
	int i = 0;
	while ('\n' == line[i++]) {
		if (0 == (size --))
			return 0;
	}
	return (uint32_t)(i - 1);
}

void tty_freeline(tty_device_t *tty, uint8_t *line) {
	uint32_t line_size;
	if (0 != tty->rx_cnt) {
		line_size = tty_scanline((uint8_t*) tty->rx_buff, tty->rx_cnt);
	}
}


