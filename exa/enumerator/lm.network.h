#pragma once

#include <lm.h>

namespace Net
{
    void DumpInfo(SERVER_INFO_101 const& info101);
    NET_API_STATUS EnumerateHosts(PCWSTR domainName);
    NET_API_STATUS EnumerateDomains();
}
