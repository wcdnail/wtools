#pragma once

#ifdef _DEBUG
#include <map>
#define _MAP_TYPE std::map
#else
#include <boost/unordered_map.hpp>
#define _MAP_TYPE boost::unordered_map
#endif
