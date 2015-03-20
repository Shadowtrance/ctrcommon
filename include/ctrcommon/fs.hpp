#ifndef __CTRCOMMON_FS_HPP__
#define __CTRCOMMON_FS_HPP__

#include "ctrcommon/types.hpp"

#include <string>

typedef enum {
    NAND,
    SD
} MediaType;

u64 fs_get_free_space(MediaType mediaType);
bool fs_exists(const std::string path);
bool fs_delete(const std::string path);

#endif
