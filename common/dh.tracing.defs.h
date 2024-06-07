#pragma once

#define DH__STRINGIZE(V)     #V
#define DH_STRINGIZE(V)      DH__STRINGIZE(V)
#define DH__WSTRINGIZE(V)    L#V
#define DH_WSTRINGIZE(V)     DH__WSTRINGIZE(V)

