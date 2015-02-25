#include "ctrcommon/common.hpp"
#include "service.hpp"

#include <sys/dirent.h>
#include <sys/syslimits.h>
#include <sys/unistd.h>
#include <stdio.h>

#include <sstream>

#include <3ds.h>

static FS_archive sdmcArchive = {ARCH_SDMC, {PATH_EMPTY, 1, (u8*) ""}};

const std::string fs_fix_path(const std::string path) {
    std::string fixed = path;
    std::string::size_type colon = fixed.find(':');
    if(colon != std::string::npos) {
        fixed = fixed.substr(colon + 1, std::string::npos);
    }

    if(fixed.find(':') != std::string::npos) {
        return NULL;
    }

    if(fixed[0] == '/') {
        return path;
    }

    static char cwdBuf[PATH_MAX + 1];
    getcwd(cwdBuf, PATH_MAX);

    std::stringstream stream;
    stream << cwdBuf << fixed;
    return stream.str();
}

u64 fs_get_free_space(MediaType mediaType) {
    if(!service_require("fs")) {
        return 0;
    }

    u32 clusterSize;
    u32 freeClusters;
    Result res = 0;
    if(mediaType == NAND) {
        res = FSUSER_GetNandArchiveResource(NULL, NULL, &clusterSize, NULL, &freeClusters);
    } else {
        res = FSUSER_GetSdmcArchiveResource(NULL, NULL, &clusterSize, NULL, &freeClusters);
    }

    if(res != 0) {
        platform_set_error(service_parse_error((u32) res));
        return 0;
    }

    return clusterSize * freeClusters;
}

bool fs_exists(const std::string path) {
    FILE* fd = fopen(path.c_str(), "r");
    if(fd) {
        fclose(fd);
        return true;
    }

    DIR* dir = opendir(path.c_str());
    if(dir) {
        closedir(dir);
        return true;
    }

    return false;
}

bool fs_delete(const std::string path) {
    Result res = FSUSER_DeleteFile(NULL, sdmcArchive, FS_makePath(PATH_CHAR, fs_fix_path(path).c_str()));
    if(res != 0) {
        platform_set_error(service_parse_error((u32) res));
    }

    return res == 0;
}