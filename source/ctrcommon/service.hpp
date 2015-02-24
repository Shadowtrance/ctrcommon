#ifndef __CTRCOMMON_SERVICE_HPP__
#define __CTRCOMMON_SERVICE_HPP__

bool serviceInit();
void serviceCleanup();
bool serviceRequire(const std::string service);

#endif