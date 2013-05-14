/**
 * @file
 * @brief Generic socket support routines.
 * @details Contains functions for working with all type of structure sock.
 *			It includes free sock pull.
 *
 * @date 12.08.09
 * @author Nikolay Korotky
 */

#include <errno.h>
#include <string.h>
#include <net/skbuff.h>
#include <net/sock.h>
#include <mem/misc/pool.h>
#include <hal/ipl.h>
#include <kernel/softirq_lock.h>
#include <kernel/event.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <framework/mod/options.h>

#include <kernel/task/io_sync.h>
#include <mem/misc/slab.h>

#define MODOPS_MIN_AMOUNT_SOCK OPTION_GET(NUMBER, min_amount_sock)

/* allocates proto structure for specified protocol*/
static struct sock * sk_prot_alloc(struct proto *prot) {
	struct sock *sk;
	ipl_t sp;

	assert(prot != NULL);

	sk = NULL;

	sp = ipl_save();
	{
		if (prot->sock_alloc != NULL) {
			assert(prot->sock_free != NULL);
			sk = prot->sock_alloc();
		}
		else {
			if (prot->cachep == NULL) {
				prot->cachep = cache_create(prot->name, prot->obj_size,
						MODOPS_MIN_AMOUNT_SOCK);
			}
			if (prot->cachep != NULL) {
				sk = cache_alloc(prot->cachep);
			}
		}
	}
	ipl_restore(sp);

	if (sk == NULL) {
		return NULL;
	}

	memset(&sk->sk_lock, 0, sizeof sk->sk_lock);

	return sk;
}

/* returns specified structure sock into pull */
static void sk_prot_free(const struct proto *prot, struct sock *sk) {
	ipl_t sp;

	assert(prot != NULL);
	assert(sk != NULL);

	sp = ipl_save();
	if (prot->sock_free != NULL) {
		assert(prot->sock_alloc != NULL);
		prot->sock_free(sk);
	} else {
		assert(prot->cachep != NULL);
		cache_free(prot->cachep, sk);
	}
	ipl_restore(sp);
}

struct sock * sk_alloc(int family, struct proto *prot) {
	struct sock *sk;

	assert(prot != NULL);

	sk = sk_prot_alloc(prot);
	if (sk == NULL) {
		return NULL;
	}

	sk->sk_receive_queue = skb_queue_alloc();
	if (sk->sk_receive_queue == NULL) {
		sk_prot_free(prot, sk);
		return NULL;
	}

	sk->sk_write_queue = skb_queue_alloc();
	if (sk->sk_write_queue == NULL) {
		skb_queue_free(sk->sk_receive_queue);
		sk_prot_free(prot, sk);
		return NULL;
	}

	if (prot->init != NULL) {
		if (prot->init(sk) < 0) {
			skb_queue_free(sk->sk_receive_queue);
			skb_queue_free(sk->sk_write_queue);
			sk_prot_free(prot, sk);
			return NULL;
		}
	}

#if 0
	sk->sk_destruct = NULL;
#endif
	sk->sk_family = family;
	sk->sk_prot = prot;

	/* FIXME in tcp.c tcp_default_sock is created without call socket() */
	event_init(&sk->sock_is_not_empty, "sock_is_not_empty");
#if 0
	event_init(&sk->sock_is_ready, "sock_is_ready");
#endif

	sk->sk_shutdown = 0;

	if (prot->hash != NULL) {
		prot->hash(sk);
	}

	return sk;
}

void sk_free(struct sock *sk) {
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);

	if (sk->sk_prot->unhash != NULL) {
		sk->sk_prot->unhash(sk);
	}
#if 0
	if (sk->sk_destruct != NULL) {
		sk->sk_destruct(sk);
	}
#endif
	sk_prot_free(sk->sk_prot, sk);
}

int sock_no_listen(struct socket *sock, int backlog) {
	assert(sock != NULL);
	return -EOPNOTSUPP;
}

int sock_no_accept(struct socket *sock, struct socket *newsock, int flags) {
	assert(sock != NULL);
	return -EOPNOTSUPP;
}

void sock_queue_rcv_skb(struct sock *sk, struct sk_buff *skb) {
	assert(sk != NULL);

	if (sk->sk_shutdown & (SHUT_RD + 1)) {
		return;
	}

	skb_queue_push(sk->sk_receive_queue, skb);
	event_notify(&sk->sock_is_not_empty);
	idx_io_enable(sk->sk_socket->desc_data, IDX_IO_READING);
}

// TODO remove this
int sock_common_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *msg, size_t size, int flags) {
	assert(sock != NULL);
	assert(sock->sk != NULL);
	assert(sock->sk->sk_prot != NULL);
	assert(sock->sk->sk_prot->recvmsg != NULL);

	return sock->sk->sk_prot->recvmsg(iocb, sock->sk, msg, size, flags);
}

void sk_common_release(struct sock *sk) {
	assert(sk != NULL);
	assert(sk->sk_prot != NULL);
	assert(sk->sk_receive_queue);
	assert(sk->sk_write_queue);

#if 0
	if (sk->sk_prot->destroy != NULL) {
		sk->sk_prot->destroy(sk);
	}
#endif

	skb_queue_free(sk->sk_receive_queue);
	skb_queue_free(sk->sk_write_queue);

	sk_free(sk);
}

int sock_lock(struct sock **psk) {
	struct sock *sk;
	spinlock_t is_locked;

	assert(psk != NULL);

	do {
		softirq_lock();
		{
			sk = *psk;
			if (sk != NULL) {
				is_locked = sk->sk_lock.slock;
				sk->sk_lock.slock = 1;
			}
		}
		softirq_unlock();
	} while ((sk != NULL) && is_locked);

	return sk != NULL ? 1 : 0;
}

void sock_unlock(struct sock *sk) {
	assert(sk != NULL);

	sk->sk_lock.slock = 0;
}
