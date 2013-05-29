/**
 * @file
 * @brief Socket interface function for kernel mode.
 *
 * @date 08.02.10
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef NET_KSOCKET_H_
#define NET_KSOCKET_H_

#include <sys/socket.h>
#include <net/socket.h>
#include <net/sock.h>
#include <stddef.h>

struct socket;
struct sock;
struct msghdr;

/**
 * Create socket method in kernel layer.
 * Allocate memory pick the protocol-type methods structure if
 * present, call native initialization routine and add a node to
 * socket registry to keep track of an entity. Sets socket
 * connection state to UNCONNECTED
 *
 * @param family - a protocol family which will be used for
 *                 communication.
 * @param type - socket type (i.e. SOCK_STREAM, SOCK_DGRAM,
 *               SOCK_RAW).
 * @param protocol - a particular protocol to be used with the
 *                   socket.
 * @param psock - pointer to pointer to the socket structure
 * @param sk - socket to wrap. if NULL created from scratch
 * @param sk_ops - proto struct pointer for sk socket type
 * @return On success, 0 otherwise result equal to minus posix
 * errno for the error reason.
 */
extern int ksocket(int family, int type, int protocol,
		struct socket **out_sock);

/**
 * Close socket method in kernel layer.
 * Calls socket's native release method if present and
 * frees up it's memory after. Removes socket entity from registry
 *
 * @param sock - socket to close
 * @return 0 on success, otherwise result equal to minus posix
 * errno for the error reason.
 */
extern int ksocket_close(struct socket *sock);

/**
 * Bind socket to a local address.
 * Performs various verifications of input parameters and calls
 * protocol realization of bind, which should perform the
 * specific for the protocol binding procedure. Upon success
 * socket connection state is set to BOUND else it is left to be
 * UNCONNECTED.
 *
 * @param sock - pointer to the socket structure
 * @param addr - address
 * @param addrlen - size, in bytes, of the address structure pointed to by addr.
 * @return 0 on success, minus posix errno indicating the reason
 */
extern int kbind(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen);

/**
 * Listen for connections on a connection-mode socket.
 * Sets the state of a bound socket to listening, if is made sure
 * to be able to do so for a particular socket, by calling native
 * protocol listen method.
 *
 * @param sock - pointer to the socket structure
 * @param backlog - maximum length to which the queue of pending
 *                  connections for sock may grow
 * @return 0 on success, minus posix errno indicating the reason
 * of an error on a failure
 */
extern int klisten(struct socket *sock, int backlog);

/**
 * Accept a connection on a connection-mode socket.
 * Call accept native method of a protocol to which the socket
 * belongs, if is made sure to be able to do so. On acception a
 * new socket with the same type and protocol is created. The new
 * socket handles the particular connection, while the listening
 * sockets stays in it's state listening to incoming connections.
 *
 * @param sock - pointer to the socket structure
 * @param out_sock - pointer to new socket
 * @param out_addr - sockaddr structure representing incoming
 *                   connection address if available
 * @param *out_addrlen - is set to the length of address structure
 *                       if it is available
 * @return 0 on success, minus posix errno on failure
 */
extern int kaccept(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen, int flags, struct socket **out_sock);

/**
 * Initiate a connection from a connection-mode socket.
 * Perform verifications needed to be be able to connect and call
 * native protocol method.
 *
 * @param sock - pointer to the socket structure
 * @param addr - address to connect
 * @param addrlen - the size of addr
 * @param flags
 * @return 0 on success, minus posix errno on failure
 */
extern int kconnect(struct socket *sock, const struct sockaddr *addr,
		socklen_t addrlen, int flags);

/**
 * Get socket name.
 * Note: not realized.
 */
extern int kgetsockname(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen);

/**
 * Get name of connected peer socket.
 * Note: not realized.
 */
extern int kgetpeername(struct socket *sock, struct sockaddr *addr,
		socklen_t *addrlen);

/**
 * Get socket options.
 * Note: not realized.
 */
extern int kgetsockopt(struct socket *sock, int level, int optname,
		void *optval, socklen_t *optlen);

/**
 * Set socket options.
 * Note: not realized.
 */
extern int ksetsockopt(struct socket *sock, int level, int optname,
		const void *optval, socklen_t optlen);

/**
 * Send a message on a socket.
 * Call sendmsg callback from proto_ops.
 *
 * @param sock - pointer to the socket structure
 * @param msg
 * @return error code
 */
extern int ksendmsg(struct socket *sock, struct msghdr *msg,
		int flags);

/**
 * Receive a message from a socket.
 * Call recvmsg callback from proto_ops.
 *
 * @param sock - pointer to the socket structure
 * @param msg
 * @param flags
 * @return error code
 */
extern int krecvmsg(struct socket *sock, struct msghdr *msg,
		int flags);

extern int kshutdown(struct socket *sock, int how);

#endif /* NET_KSOCKET_H_ */
