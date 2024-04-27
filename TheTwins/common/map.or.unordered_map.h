#pragma once

#ifdef _DEBUG
#include <map>
#define _MAP_TYPE std::map
#else
#include <unordered_map>
#define _MAP_TYPE std::unordered_map
#endif
