#ifndef __CTRCOMMON_SERVICE_HPP__
#define __CTRCOMMON_SERVICE_HPP__

#include "ctrcommon/platform.hpp"
#include "ctrcommon/types.hpp"

#include <string>

bool service_init();
void service_cleanup();
bool service_require(const std::string service);
Error service_parse_error(u32 error);

#endif