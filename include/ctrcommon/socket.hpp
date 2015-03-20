#ifndef __CTRCOMMON_SOCKET_HPP__
#define __CTRCOMMON_SOCKET_HPP__

#include "ctrcommon/types.hpp"

#include <stdio.h>

#include <string>

u64 htonll(u64 value);
u64 ntohll(u64 value);
u32 socket_get_host_ip();
int socket_listen(u16 port);
FILE* socket_accept(int listeningSocket);
FILE* socket_connect(const std::string ipAddress, u16 port);

#endif
