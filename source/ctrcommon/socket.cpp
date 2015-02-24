#include "ctrcommon/common.hpp"
#include "service.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include <3ds.h>
#include <stdio.h>

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
    if(!serviceRequire("soc")) {
        return 0;
    }

    return (u32) gethostid();
}

int socket_listen(u16 port) {
    if(!serviceRequire("soc")) {
        return -1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        return -1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if(bind(fd, (struct sockaddr *) &address, sizeof(address)) != 0) {
        return -1;
    }

    int flags = fcntl(fd, F_GETFL);
    if(flags == -1) {
        return -1;
    }

    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
        return -1;
    }

    if(listen(fd, 10) != 0) {
        return -1;
    }

    return fd;
}

FILE* socket_accept(int listeningSocket) {
    if(!serviceRequire("soc")) {
        return NULL;
    }

    int afd = accept(listeningSocket, (struct sockaddr *) NULL, NULL);
    if(afd < 0) {
        return NULL;
    }

    int flags = fcntl(afd, F_GETFL);
    if(flags == -1) {
        return NULL;
    }

    if(fcntl(afd, F_SETFL, flags | O_NONBLOCK) != 0) {
        return NULL;
    }

    return fdopen(afd, "rw");
}

FILE* socket_connect(const std::string ipAddress, u16 port) {
    if(!serviceRequire("soc")) {
        return NULL;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        return NULL;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if(inet_aton(ipAddress.c_str(), &address.sin_addr) <= 0) {
        return NULL;
    }

    if(connect(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        return NULL;
    }

    int flags = fcntl(fd, F_GETFL);
    if(flags == -1) {
        return NULL;
    }

    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
        return NULL;
    }

    return fdopen(fd, "rw");
}