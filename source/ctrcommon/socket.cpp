#include "ctrcommon/common.hpp"

#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>

#include <3ds.h>

static bool socInit;
static void *socBuffer;

bool sockets_init() {
    if(!socInit) {
        socBuffer = memalign(0x1000, 0x100000);
        if(socBuffer == NULL) {
            return false;
        }

        if(SOC_Initialize((u32 *) socBuffer, 0x100000) != 0) {
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

u64 htonll(u64 value) {
    static const int num = 42;
    if(*((char *) &num) == num) {
        return (((uint64_t) htonl((u32) value)) << 32) + htonl((u32) (value >> 32));
    } else {
        return value;
    }
}

u64 ntohll(u64 value) {
    return htonll(value);
}

u32 socket_get_host_ip() {
    if(!sockets_init()) {
        return 0;
    }

    return (u32) gethostid();
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

    if(bind(fd, (struct sockaddr *) &address, sizeof(address)) != 0) {
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

    int afd = accept(fd, (struct sockaddr *) NULL, NULL);
    if(afd < 0) {
        errno = SOC_GetErrno();
        return -1;
    }

    int flags = fcntl(afd, F_GETFL);
    if(flags == -1) {
        errno = SOC_GetErrno();
        return -1;
    }

    if(fcntl(afd, F_SETFL, flags | O_NONBLOCK) != 0) {
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

int socket_read(int fd, void *buffer, u32 bufferSize) {
    if(!sockets_init()) {
        return -1;
    }

    u8 *orig = (u8 *) buffer;
    u8 *ptr = orig;
    u32 currSize = bufferSize;
    while(ptr != orig + bufferSize) {
        int len = recv(fd, ptr, currSize, 0);
        if(len < 0) {
            if(platform_is_io_waiting()) {
                continue;
            }

            errno = SOC_GetErrno();
            return -1;
        } else if(len == 0) {
            break;
        }

        ptr += len;
        currSize -= len;
    }

    return (int) (bufferSize - currSize);
}

int socket_write(int fd, void *buffer, u32 bufferSize) {
    if(!sockets_init()) {
        return -1;
    }

    u8 *orig = (u8 *) buffer;
    u8 *ptr = orig;
    u32 currSize = bufferSize;
    while(ptr != orig + bufferSize) {
        int len = send(fd, ptr, currSize, 0);
        if(len < 0) {
            if(platform_is_io_waiting()) {
                continue;
            }

            errno = SOC_GetErrno();
            return -1;
        }

        ptr += len;
        currSize -= len;
    }

    return (int) (bufferSize - currSize);
}

void socket_close(int fd) {
    if(!sockets_init()) {
        return;
    }

    closesocket(fd);
}