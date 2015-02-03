#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>

#include <string>

#include <3ds.h>

static bool socInit;
static void* socBuffer;

bool sockets_init() {
	if(!socInit) {
		socBuffer = memalign(0x1000, 0x100000);
		if(socBuffer == NULL) {
			return false;
		}

		if(SOC_Initialize((u32*) socBuffer, 0x100000) != 0) {
			free(socBuffer);
			socBuffer = NULL;
			return false;
		}

		socInit = true;
	}

	return socInit;
}

void sockets_cleanup() {
	if(!socInit) {
		return;
	}

	socInit = false;
	free(socBuffer);
	socBuffer = NULL;
	SOC_Shutdown();
}

int socket_listen(u16 port) {
	if(!sockets_init()) {
		return -1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	if(bind(fd, (struct sockaddr*) &address, sizeof(address)) != 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	int flags = fcntl(fd, F_GETFL);
	if(flags == -1) {
		errno = SOC_GetErrno();
		return -1;
	}

	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	if(listen(fd, 10) != 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	return fd;
}

int socket_accept(int fd) {
	if(!sockets_init()) {
		return -1;
	}

	int afd = accept(fd, (struct sockaddr*)NULL, NULL);
	if(afd < 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	int flags = fcntl(fd, F_GETFL);
	if(flags == -1) {
		errno = SOC_GetErrno();
		return -1;
	}

	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	return afd;
}

int socket_connect(const std::string ipAddress, u16 port) {
	if(!sockets_init()) {
		return -1;
	}

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	if(inet_aton(ipAddress.c_str(), &address.sin_addr) <= 0) {
		return -1;
	}

	if(connect(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	int flags = fcntl(fd, F_GETFL);
	if(flags == -1) {
		errno = SOC_GetErrno();
		return -1;
	}

	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
		errno = SOC_GetErrno();
		return -1;
	}

	return fd;
}

void socket_close(int fd) {
	if(!sockets_init()) {
		return;
	}

	closesocket(fd);
}