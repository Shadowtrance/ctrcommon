#ifndef __CTRCOMMON_FS_HPP__
#define __CTRCOMMON_FS_HPP__

#include "ctrcommon/types.hpp"

#include <string>

typedef enum {
    NAND,
    SD
} MediaType;

u64 fsGetFreeSpace(MediaType mediaType);
bool fsExists(const std::string path);
bool fsIsDirectory(const std::string path);
bool fsDelete(const std::string path);

#endif
