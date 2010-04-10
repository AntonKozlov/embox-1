/**
 * @file
 *
 * @date 28.02.2009
 * @author Eldar Abusalimov
 */
#include "screen.h"
#include <assert.h>
#include <embox/kernel.h>
#include "console.h"
#include <kernel/sys.h>
#include <kernel/diag.h>
#include <kernel/softirq.h>

#define UART_SOFTIRQ_NR 31

/*#define FIRE_CALLBACK(cb, func, view, args...)	((cb->func != NULL) ? cb->func(cb, view, ##args) : 0)*/
#define FIRE_CALLBACK(cb, func, view, ...)	do {((cb->func != NULL) ? cb->func(cb, view, ## __VA_ARGS__) : 0) ;} while (0)
extern CONSOLE *cur_console;

static void handle_char_token(SCREEN *this, TERMINAL_TOKEN ch) {
	SCREEN_CALLBACK *cb = this->callback;
	if (cb == NULL) {
		return;
	}

	FIRE_CALLBACK(cb, on_char, this, ch);
}

static void handle_ctrl_token(SCREEN *this, TERMINAL_TOKEN token,
		TERMINAL_TOKEN_PARAMS *params) {
	SCREEN_CALLBACK *cb = this->callback;
	static TERMINAL_TOKEN prev_token = TERMINAL_TOKEN_EMPTY;
	if (cb == NULL) {
		return;
	}

	switch (token) {
	case TERMINAL_TOKEN_CURSOR_LEFT:
		FIRE_CALLBACK(cb, on_cursor_left, this, 1);
		break;
	case TERMINAL_TOKEN_CURSOR_RIGHT:
		FIRE_CALLBACK(cb, on_cursor_right, this, 1);
		break;
	case TERMINAL_TOKEN_CURSOR_UP:
		FIRE_CALLBACK(cb, on_cursor_up, this, 1);
		break;
	case TERMINAL_TOKEN_CURSOR_DOWN:
		FIRE_CALLBACK(cb, on_cursor_down, this, 1);
		break;
	case TERMINAL_TOKEN_BS:
		FIRE_CALLBACK(cb, on_backspace, this, 0);
		break;
	case TERMINAL_TOKEN_DEL:
		FIRE_CALLBACK(cb, on_delete, this, 0);
		break;
	case TERMINAL_TOKEN_END:
		/* TODO: strange char 'F' */
		FIRE_CALLBACK(cb, on_end, this, 0);
		break;
	case TERMINAL_TOKEN_ETX:
		FIRE_CALLBACK(cb, on_etx, this, 0);
		break;
	case TERMINAL_TOKEN_EOT:
		FIRE_CALLBACK(cb, on_eot, this, 0);
		break;
	case TERMINAL_TOKEN_DC2:
		FIRE_CALLBACK(cb, on_dc2, this, 0);
		break;
	case TERMINAL_TOKEN_DC4:
		FIRE_CALLBACK(cb, on_dc4, this, 0);
		break;
	case TERMINAL_TOKEN_LF:
		if (prev_token == TERMINAL_TOKEN_CR) {
			break;
		}
	case TERMINAL_TOKEN_CR:
		FIRE_CALLBACK(cb, on_new_line, this, 0);
		break;
	case TERMINAL_TOKEN_PRIVATE:
		if (params->length == 0) {
			break;
		}
		switch (params->data[0]) {
		case TERMINAL_TOKEN_PARAM_PRIVATE_DELETE:
			FIRE_CALLBACK(cb, on_delete, this, 0);
			break;
#if 0
		case TERMINAL_TOKEN_PARAM_PRIVATE_END:
			FIRE_CALLBACK(cb, on_end, this);
			break;
#endif
		case TERMINAL_TOKEN_PARAM_PRIVATE_HOME:
			FIRE_CALLBACK(cb, on_home, this, 0);
			break;
		case TERMINAL_TOKEN_PARAM_PRIVATE_INSERT:
			FIRE_CALLBACK(cb, on_insert, this, 0);
			break;
		default:
			break;
		}
		break;
	case TERMINAL_TOKEN_HT:
		FIRE_CALLBACK(cb, on_tab, this, 0);
		break;
	default:
		break;
	}

	prev_token = token;
}

void uart_softirq_handler(softirq_nr_t softirq_nr, void *data) {
	char ch;
	static TERMINAL_TOKEN token;
	static TERMINAL_TOKEN_PARAMS params[1];
	SCREEN *this;
//	if (!sys_exec_is_started()) {
//		return;
//	}
	this = cur_console->view;
	terminal_receive(this->terminal, &token, params);
	ch = token & 0xFF;
	/*TODO:*/
	if (ch == token) {
		handle_char_token(this, token);
	} else {
		handle_ctrl_token(this, token, params);
	}
}

static void uart_irq_handler(int irq_num, void *dev_id, struct pt_regs *regs) {
	softirq_raise(UART_SOFTIRQ_NR);
}

void screen_in_start(SCREEN *this, SCREEN_CALLBACK *cb) {
	static TERMINAL_TOKEN token;
	static TERMINAL_TOKEN_PARAMS params[1];
	char ch;
	if ((this == NULL) || this->running) {
		return;
	}
	this->running = true;

	this->callback = cb;
	softirq_install(UART_SOFTIRQ_NR, uart_softirq_handler, NULL);
	uart_set_irq_handler(uart_irq_handler);
	while (this->callback != NULL && terminal_receive(this->terminal, &token,
			params)) {
		ch = token & 0xFF;
		if (ch == token) {
			handle_char_token(this, token);
		} else {
			handle_ctrl_token(this, token, params);
		}
	}
	assert(this->callback == NULL);
	this->running = false;
}

void screen_in_stop(SCREEN *this) {
	if (this == NULL) {
		return;
	}
	uart_remove_irq_handler();
	this->callback = NULL;
}
