/*
 * Phoenix-RTOS
 *
 * Host implementation of messages
 *
 * Copyright 2025 Phoenix Systems
 * Author: Mateusz Piasecki
 *
 * %LICENSE%
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/cdefs.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <posix/idtree.h>
#include <sys/list.h>
#include <sys/rb.h>
#include <sys/msg.h>


/* Special message type for lookup handling */
#define MT_PING -1

#define HASH_LEN 5

#define LOCAL_LISTENER_NAME_LEN 32


#define idtreeof(type, node_field, node) ({ \
	(type *)((char *)(node) - offsetof(type, node_field)); \
})


typedef struct _message_entry_t {
	idnode_t linkage;
	struct _message_entry_t_t *next;
	struct _message_entry_t_t *prev;

	msg_t msg;
	int sockfd;
	void *data_i;
	void *data_o;
} message_entry_t;


typedef struct _port_node_t {
	idnode_t linkage;
	struct _port_node_t *next;
	struct _port_node_t *prev;

	bool local;
	bool closed;
	int references;

	/* msg queue */
	message_entry_t *messages;

	pthread_cond_t msgReceived;
	idtree_t recMessages;

	pthread_mutex_t lock;
	char socketName[];
} port_node_t;


typedef struct {
	int fd;
	uint32_t port;
	oid_t oid;
	char *name;
} listener_t;


typedef struct {
	pthread_t thread;

	listener_t *listeners;
	int nlisteners;
	int listenersCap;

	int eventfd;
	bool running;
	pthread_mutex_t lock;
} receiver_t;


struct {
	idtree_t ports;

	receiver_t rec;

	pthread_mutex_t lock;
} common;


static void signalHandler(int signum);
static int _msgRespond(message_entry_t *entry);


static idnode_t *idtreeMinimum(rbnode_t *node)
{
	rbnode_t *x = node;

	if (x != NULL) {
		while (x->left != NULL) {
			x = x->left;
		}
	}

	return lib_treeof(idnode_t, linkage, x);
}


static void initPortNode(port_node_t *port)
{
	port->closed = false;
	port->messages = NULL;
	port->references = 0;
	pthread_cond_init(&port->msgReceived, NULL);
	idtree_init(&port->recMessages);
	pthread_mutex_init(&port->lock, NULL);
	port->socketName[0] = '\0';
}


static int setNonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL, 0);

	if (fl == -1) {
		return -1;
	}
	return fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}


static int setBlock(int fd)
{
	int fl = fcntl(fd, F_GETFL, 0);

	if (fl == -1) {
		return -1;
	}
	return fcntl(fd, F_SETFL, fl & ~O_NONBLOCK);
}


static int addListener(int serverfd, uint32_t port, oid_t *oid, char *name)
{
	size_t cap;
	listener_t *buff;
	uint64_t u = 1;

	pthread_mutex_lock(&common.rec.lock);

	if (common.rec.nlisteners >= common.rec.listenersCap) {
		/* Double capacity (or at least 1 if starting from zero) */
		cap = common.rec.listenersCap ? common.rec.listenersCap * 2 : 1;
		buff = realloc(common.rec.listeners, cap * sizeof(listener_t));
		if (!buff) {
			pthread_mutex_unlock(&common.rec.lock);
			return -ENOMEM;
		}
		common.rec.listeners = buff;
		common.rec.listenersCap = cap;
	}

	common.rec.listeners[common.rec.nlisteners++] = (listener_t) { serverfd, port, *oid, name };

	pthread_mutex_unlock(&common.rec.lock);

	return write(common.rec.eventfd, &u, sizeof(uint64_t)) != sizeof(uint64_t) ? -1 : 0;
}


static int removeListeners(uint32_t port)
{
	int i;
	bool removed = false;
	int clientfd;
	char buffer[128];
	size_t cap;
	listener_t *buff;
	uint64_t u = 1;

	pthread_mutex_lock(&common.rec.lock);
	for (i = 0; i < common.rec.nlisteners; ++i) {
		if (common.rec.listeners[i].port == port) {
			shutdown(common.rec.listeners[i].fd, SHUT_RDWR);

			while ((clientfd = accept(common.rec.listeners[i].fd, NULL, NULL)) != -1) {
				while (read(clientfd, buffer, sizeof(buffer)) > 0) { }
				shutdown(clientfd, SHUT_RDWR);
				close(clientfd);
			}

			close(common.rec.listeners[i].fd);
			unlink(common.rec.listeners[i].name);
			free(common.rec.listeners[i].name);
			common.rec.listeners[i] = common.rec.listeners[common.rec.nlisteners - 1];
			common.rec.nlisteners--;
			--i;

			removed = true;
		}
	}
	pthread_mutex_unlock(&common.rec.lock);

	if (removed) {
		pthread_mutex_lock(&common.rec.lock);
		if (common.rec.nlisteners < common.rec.listenersCap / 2) {
			cap = common.rec.listenersCap / 2;
			buff = realloc(common.rec.listeners, cap * sizeof(listener_t));
			if (!buff) {
				pthread_mutex_unlock(&common.rec.lock);
				return -ENOMEM;
			}
			common.rec.listeners = buff;
			common.rec.listenersCap = cap;
		}
		pthread_mutex_unlock(&common.rec.lock);

		if (write(common.rec.eventfd, &u, sizeof(uint64_t)) != sizeof(uint64_t)) {
			return -1;
		}
	}

	return removed ? 0 : -1;
}

ssize_t writeFull(int sockfd, const void *buffer, size_t length)
{
	size_t total_read = 0;
	char *buf = (char *)buffer;
	ssize_t num_read;

	while (total_read < length) {
		num_read = write(sockfd, buf + total_read, length - total_read);
		if (num_read < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		total_read += num_read;
	}

	return total_read;
}


ssize_t readFull(int sockfd, void *buffer, size_t length)
{
	size_t total_read = 0;
	char *buf = (char *)buffer;
	ssize_t num_read;

	while (total_read < length) {
		num_read = read(sockfd, buf + total_read, length - total_read);
		if (num_read < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		else if (num_read == 0) {
			break;
		}
		total_read += num_read;
	}

	return total_read;
}


static int createConnection(const char *name, int sockfd)
{
	struct sockaddr_un addr;

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);

	return connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
}


static int startServer(const char *name)
{
	int sockfd;
	struct sockaddr_un addr;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return -1;
	}

	unlink(name);

	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);


	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(sockfd);
		return -1;
	}

	if (listen(sockfd, 5) < 0) {
		unlink(name);
		close(sockfd);
		return -1;
	}

	setNonblock(sockfd);

	return sockfd;
}


static port_node_t *portAcquire(int id)
{
	port_node_t *port;

	pthread_mutex_lock(&common.lock);
	port = idtreeof(port_node_t, linkage, idtree_find(&common.ports, id));
	if (port == NULL) {
		pthread_mutex_unlock(&common.lock);
		return NULL;
	}
	pthread_mutex_lock(&port->lock);
	port->references++;
	pthread_mutex_unlock(&port->lock);
	pthread_mutex_unlock(&common.lock);

	pthread_mutex_lock(&port->lock);
	if (port->closed && port->references <= 1) {
		pthread_mutex_unlock(&port->lock);
		free(port);
		port = NULL;
	}
	else if (port->closed) {
		port->references--;
		pthread_mutex_unlock(&port->lock);
		port = NULL;
	}

	return port;
}


static void portRelease(port_node_t *port)
{
	if (port == NULL) {
		return;
	}

	port->references--;
	if (port->closed && port->references <= 0) {
		pthread_mutex_unlock(&port->lock);
		free(port);
	}
	else {
		pthread_mutex_unlock(&port->lock);
	}

	return;
}


static message_entry_t *messageGet(port_node_t *p, int id)
{
	message_entry_t *entry;

	if (p == NULL) {
		return NULL;
	}

	entry = idtreeof(message_entry_t, linkage, idtree_find(&p->recMessages, id));

	return entry;
}


static void setupLocalSockName(char *buf, size_t size, int port)
{
	size_t len;
	pid_t pid;

	pid = getpid();
	snprintf(buf, size, "%dloc%u", pid, port);
	len = strlen(buf);
	while (len < size - 1) {
		buf[len++] = 'x';
	}
	buf[len] = '\0';
}


static int handleClient(int clientfd, listener_t *listener)
{
	message_entry_t *entry;
	ssize_t num_read;
	port_node_t *p;
	int err = 0;

	entry = malloc(sizeof(message_entry_t));
	if (entry == NULL) {
		close(clientfd);
		return -ENOMEM;
	}

	num_read = readFull(clientfd, &entry->msg, sizeof(msg_t));
	if (num_read != sizeof(msg_t)) {
		free(entry);
		close(clientfd);
		return -1;
	}

	entry->sockfd = clientfd;
	entry->data_i = NULL;
	entry->data_o = NULL;

	if (entry->msg.i.data) {
		entry->data_i = malloc(entry->msg.i.size);
		if (entry->data_i == NULL) {
			free(entry);
			close(clientfd);
			return -ENOMEM;
		}

		num_read = readFull(clientfd, entry->data_i, entry->msg.i.size);
		if (num_read != entry->msg.i.size) {
			free(entry->data_i);
			free(entry);
			close(clientfd);
			return -1;
		}
	}

	shutdown(entry->sockfd, SHUT_RD);

	if (entry->msg.o.data) {
		entry->data_o = malloc(entry->msg.o.size);
		if (entry->data_o == NULL) {
			free(entry->data_i);
			free(entry);
			close(clientfd);
			return -ENOMEM;
		}
	}

	if (entry->msg.type == MT_PING) {
		entry->msg.o.lookup.dev = listener->oid;
		entry->msg.o.lookup.fil = listener->oid;
		err = _msgRespond(entry);
		free(entry);
	}
	else {
		p = portAcquire(listener->port);

		if (p == NULL) {
			free(entry->data_i);
			free(entry->data_o);
			free(entry);
			close(clientfd);
			return -1;
		}

		LIST_ADD(&p->messages, entry);

		pthread_cond_signal(&p->msgReceived);

		portRelease(p);
	}

	return err;
}


static void *receiverThread(void *arg)
{
	struct pollfd *pfds = NULL;
	listener_t *llookup = NULL;
	uint64_t u;
	int nlocal;
	int i;
	int ret;


	while (1) {
		free(pfds);
		free(llookup);

		if (common.rec.running == false) {
			break;
		}

		pthread_mutex_lock(&common.rec.lock);
		nlocal = common.rec.nlisteners;

		pfds = malloc((nlocal + 1) * sizeof(struct pollfd));
		if (pfds == NULL) {
			pthread_mutex_unlock(&common.rec.lock);
			continue;
		}
		llookup = malloc((nlocal) * sizeof(listener_t));
		if (llookup == NULL) {
			pthread_mutex_unlock(&common.rec.lock);
			free(pfds);
			continue;
		}

		for (i = 0; i < nlocal; ++i) {
			pfds[i].fd = common.rec.listeners[i].fd;
			pfds[i].events = POLLIN;
			llookup[i] = common.rec.listeners[i];
		}
		pthread_mutex_unlock(&common.rec.lock);
		pfds[nlocal].fd = common.rec.eventfd;
		pfds[nlocal].events = POLLIN;

		ret = poll(pfds, 1 + nlocal, -1);
		if (ret < 0) {
			continue;
		}

		if (pfds[nlocal].revents & POLLIN) {
			for (;;) {
				if (read(common.rec.eventfd, &u, sizeof(uint64_t)) != sizeof(uint64_t)) {
					break;
				}
			}
		}

		for (int i = 0; i < nlocal; ++i) {
			if (!(pfds[i].revents & POLLIN)) {
				continue;
			}

			for (;;) {
				int clientfd = accept(pfds[i].fd, NULL, NULL);

				if (clientfd == -1) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						break;
					}

					/* Socket most likely belonged to a port that has been closed */

					break;
				}

				setBlock(clientfd);

				handleClient(clientfd, &llookup[i]);
			}
		}
	}

	return NULL;
}


static void ensureReceiverStarted(void)
{
	pthread_mutex_lock(&common.rec.lock);

	if (common.rec.thread == 0) {
		pthread_create(&common.rec.thread, NULL, receiverThread, NULL);
	}

	pthread_mutex_unlock(&common.rec.lock);
}


static int initReceiverSrv(void)
{
	struct sigaction sa;

	common.rec.listeners = NULL;
	common.rec.nlisteners = 0;
	common.rec.listenersCap = 0;

	if (pthread_mutex_init(&common.rec.lock, NULL) != 0) {
		return -1;
	}

	common.rec.eventfd = eventfd(0, EFD_NONBLOCK);
	if (common.rec.eventfd < 0) {
		return -1;
	}

	common.rec.running = true;

	common.rec.thread = 0;

	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		return -1;
	}

	return 0;
}


static int destrReceiverSrv(void)
{
	idnode_t *it;
	port_node_t *port;
	uint64_t u = 1;

	if (common.rec.thread) {
		common.rec.running = false;
		if (write(common.rec.eventfd, &u, sizeof(uint64_t)) == sizeof(uint64_t)) {
			pthread_join(common.rec.thread, NULL);
		}
	}

	it = idtreeMinimum(common.ports.root);
	while (it != NULL) {
		port = idtreeof(port_node_t, linkage, it);

		portDestroy(port->linkage.id);

		it = idtreeMinimum(common.ports.root);
	}

	close(common.rec.eventfd);

	return 0;
}


static int _msgSend(const char *name, msg_t *m)
{
	int sockfd;
	ssize_t num_read;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return -EINVAL;
	}

	if (createConnection(name, sockfd) < 0) {
		close(sockfd);
		return -EINVAL;
	}

	if (writeFull(sockfd, m, sizeof(msg_t)) != sizeof(msg_t)) {
		close(sockfd);
		return -EINVAL;
	}

	if (m->i.data) {
		if (writeFull(sockfd, m->i.data, m->i.size) != m->i.size) {
			close(sockfd);
			return -EINVAL;
		}
	}

	shutdown(sockfd, SHUT_WR);

	num_read = readFull(sockfd, m, sizeof(msg_t));
	if (num_read != sizeof(msg_t)) {
		close(sockfd);
		return -EINVAL;
	}

	if (m->o.data) {
		num_read = readFull(sockfd, m->o.data, m->o.size);
		if (num_read != m->o.size) {
			close(sockfd);
			return -EINVAL;
		}
	}

	close(sockfd);
	return 0;
}


int msgSend(uint32_t port, msg_t *m)
{
	port_node_t *p;
	char *name;
	int ret;

	if (m == NULL) {
		return -EINVAL;
	}

	p = portAcquire(port);

	if (p == NULL) {
		return -EINVAL;
	}

	name = malloc(strlen(p->socketName) + 1);
	strcpy(name, p->socketName);

	portRelease(p);

	ret = _msgSend(name, m);
	free(name);
	return ret;
}


static char *tmpifyName(const char *name)
{
	char *newName;
	size_t len;

	len = strlen(name);
	newName = malloc(len + 5);

	if (newName == NULL) {
		return NULL;
	}

	strcpy(newName, "/tmp");
	strcpy(newName + 4, name);

	return newName;
}


static char *rootifyName(void)
{
	char *newName;

	newName = malloc(8);

	if (newName == NULL) {
		return NULL;
	}

	strcpy(newName, "/tmp/fs");

	return newName;
}


int lookup(const char *name, oid_t *file, oid_t *dev)
{
	msg_t msg;
	int port;
	port_node_t *entry;
	char *newName = NULL;

	if (name == NULL || (file == NULL && dev == NULL)) {
		return -EINVAL;
	}

	if (name[0] == '/' && name[1] == 0) {
		newName = rootifyName();
		name = newName;
	}
	else if (name[0] == '/' && memcmp(name, "/tmp/", 5) != 0) {
		newName = tmpifyName(name);
		name = newName;
	}

	if (name == NULL) {
		return -ENOMEM;
	}

	msg.type = MT_PING;
	msg.i.data = NULL;
	msg.i.size = 0;
	msg.o.data = NULL;
	msg.i.size = 0;

	if (_msgSend(name, &msg)) {
		free(newName);
		return -EINVAL;
	}

	if ((entry = malloc(sizeof(port_node_t) + strlen(name) + 1)) == NULL) {
		free(newName);
		return -ENOMEM;
	}
	initPortNode(entry);
	entry->local = false;
	strcpy(entry->socketName, name);

	free(newName);

	pthread_mutex_lock(&common.lock);
	if (idtree_alloc(&common.ports, &entry->linkage) < 0) {
		pthread_mutex_unlock(&common.lock);
		free(entry);
		return -ENOMEM;
	}

	port = (uint32_t)entry->linkage.id;
	pthread_mutex_unlock(&common.lock);

	if (file != NULL) {
		*file = msg.o.lookup.fil;
		file->port = port;
	}
	if (dev != NULL) {
		*dev = msg.o.lookup.dev;
		dev->port = port;
	}

	return 0;
}


int portCreate(uint32_t *port)
{
	char name[LOCAL_LISTENER_NAME_LEN];
	port_node_t *entry;
	oid_t oid = { 0, 0 };

	ensureReceiverStarted();

	entry = malloc(sizeof(port_node_t) + LOCAL_LISTENER_NAME_LEN);
	if (entry == NULL) {
		return -ENOMEM;
	}

	initPortNode(entry);
	entry->local = true;

	pthread_mutex_lock(&common.lock);

	if (idtree_alloc(&common.ports, &entry->linkage) < 0) {
		pthread_mutex_unlock(&common.lock);
		free(entry);
		return -ENOMEM;
	}

	pthread_mutex_lock(&entry->lock);

	*port = (uint32_t)entry->linkage.id;

	setupLocalSockName(name, LOCAL_LISTENER_NAME_LEN, *port);

	strncpy(entry->socketName, name, LOCAL_LISTENER_NAME_LEN);

	pthread_mutex_unlock(&entry->lock);

	pthread_mutex_unlock(&common.lock);

	/* Create local listener */
	if (portRegister(*port, name, &oid)) {
		free(entry);
		return -1;
	}

	return 0;
}


void portDestroy(uint32_t port)
{
	port_node_t *p;
	message_entry_t *message;
	idnode_t *it;
	void *tmp;
	bool freePort;

	pthread_mutex_lock(&common.lock);
	p = idtreeof(port_node_t, linkage, idtree_find(&common.ports, port));
	if (p == NULL) {
		pthread_mutex_unlock(&common.lock);
		return;
	}

	idtree_remove(&common.ports, &p->linkage);

	pthread_mutex_lock(&p->lock);
	p->references++;
	pthread_mutex_unlock(&p->lock);

	pthread_mutex_unlock(&common.lock);

	pthread_mutex_lock(&p->lock);
	if (p->closed) {
		pthread_mutex_unlock(&p->lock);
		return;
	}

	p->closed = true;

	/* This allow all stuck msgRecv to exit */
	pthread_cond_broadcast(&p->msgReceived);

	if (p->local) {
		removeListeners(port);

		while (p->messages) {
			tmp = (void *)p->messages;

			close(p->messages->sockfd);
			free(p->messages->data_i);
			free(p->messages->data_o);

			LIST_REMOVE(&p->messages, p->messages);

			free(tmp);
		}

		it = idtreeMinimum(p->recMessages.root);
		while (it != NULL) {
			message = idtreeof(message_entry_t, linkage, it);

			close(message->sockfd);
			free(message->data_i);
			free(message->data_o);

			idtree_remove(&p->recMessages, it);

			free(message);

			it = idtreeMinimum(p->recMessages.root);
		}
	}

	p->references--;

	freePort = false;

	/*
		If this condition is not true it means that the port was acquired by other thread, most
		likely in msgRecv which is waiting for a message. The thread who acquired the port is
		responsible for freeing it then.
	*/
	if (p->references <= 0) {
		freePort = true;
	}

	pthread_mutex_unlock(&p->lock);

	if (freePort) {
		free(p);
	}

	return;
}


int portRegister(uint32_t port, const char *name, oid_t *oid)
{
	port_node_t *p;
	int serverfd;
	char *newName = NULL;
	oid_t newOID;

	if (name[0] == '/' && name[1] == 0) {
		newName = rootifyName();
	}
	else if (name[0] == '/' && memcmp(name, "/tmp/", 5) != 0) {
		newName = tmpifyName(name);
	}
	else {
		newName = malloc(strlen(name) + 1);
		strcpy(newName, name);
	}

	if (newName == NULL) {
		return -ENOMEM;
	}

	serverfd = startServer(newName);

	if (serverfd < 0) {
		free(newName);
		return -1;
	}

	newOID.port = port;
	if (oid != NULL) {
		newOID.id = oid->id;
	}

	p = portAcquire(port);

	if (p == NULL) {
		portRelease(p);
		close(serverfd);
		free(newName);
		return -EINVAL;
	}

	if (!p->local) {
		portRelease(p);
		close(serverfd);
		free(newName);
		return 0;
	}

	if (addListener(serverfd, port, &newOID, newName)) {
		portRelease(p);
		close(serverfd);
		free(newName);
		return -1;
	}

	portRelease(p);

	return 0;
}


int msgRecv(uint32_t port, msg_t *m, msg_rid_t *rid)
{
	port_node_t *p;
	message_entry_t *entry;

	p = portAcquire(port);

	if (p == NULL) {
		return -EINVAL;
	}

	if (!p->local) {
		portRelease(p);
		return -EINVAL;
	}

	while ((p->messages == NULL) && !p->closed) {
		/* Reason why there may be multiple references to the same port */
		pthread_cond_wait(&p->msgReceived, &p->lock);
	}
	if (p->closed) {
		portRelease(p);
		return -EINVAL;
	}

	entry = p->messages;

	memcpy(m, &entry->msg, sizeof(*m));
	m->i.data = entry->data_i;
	m->o.data = entry->data_o;

	if (idtree_alloc(&p->recMessages, &entry->linkage) < 0) {
		portRelease(p);
		return -ENOMEM;
	}
	*rid = (msg_rid_t)entry->linkage.id;

	LIST_REMOVE(&p->messages, entry);

	portRelease(p);

	return 0;
}


static int _msgRespond(message_entry_t *entry)
{
	if (writeFull(entry->sockfd, &entry->msg, sizeof(msg_t)) != sizeof(msg_t)) {
		free(entry->data_i);
		free(entry->data_o);
		close(entry->sockfd);
		return -1;
	}

	if (entry->msg.o.data) {
		if (writeFull(entry->sockfd, entry->data_o, entry->msg.o.size) != entry->msg.o.size) {
			free(entry->data_i);
			free(entry->data_o);
			close(entry->sockfd);
			return -1;
		}
	}

	free(entry->data_i);
	free(entry->data_o);

	shutdown(entry->sockfd, SHUT_WR);
	close(entry->sockfd);

	return 0;
}


int msgRespond(uint32_t port, msg_t *m, msg_rid_t rid)
{
	port_node_t *p;
	message_entry_t *entry;
	int err = 0;

	/* TODO: use a mutex to ensure two threads dont respond to the same message - unlikely but possible */

	p = portAcquire(port);

	if (p == NULL) {
		return -EINVAL;
	}

	if (!p->local) {
		portRelease(p);
		return -EINVAL;
	}

	entry = messageGet(p, rid);

	if (entry == NULL) {
		portRelease(p);
		return -ENOENT;
	}

	memcpy(&entry->msg.o.raw, &m->o.raw, sizeof(m->o.raw));
	entry->msg.o.err = m->o.err;

	err = _msgRespond(entry);

	idtree_remove(&p->recMessages, &entry->linkage);

	free(entry);

	portRelease(p);

	return err;
}


static void signalHandler(int signum)
{
	exit(signum);
}


static void __attribute__((constructor)) init_msg(void)
{
	/* Ignore broken pipe signal */
	signal(SIGPIPE, SIG_IGN);

	idtree_init(&common.ports);
	pthread_mutex_init(&common.lock, NULL);

	initReceiverSrv();
}

static void __attribute__((destructor)) destr_msg(void)
{
	destrReceiverSrv();
}
