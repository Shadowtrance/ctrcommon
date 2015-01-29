#include "ctrcommon/common.hpp"

#include <3ds.h>

u64 fs_get_free_space(MediaType mediaType) {
    u32 clusterSize;
    u32 freeClusters;
    Result res = 0;
    if(mediaType == NAND) {
        res = FSUSER_GetNandArchiveResource(NULL, NULL, &clusterSize, NULL, &freeClusters);
    } else {
        res = FSUSER_GetSdmcArchiveResource(NULL, NULL, &clusterSize, NULL, &freeClusters);
    }

    if(res != 0) {
        return 0;
    }

    return clusterSize * freeClusters;
}