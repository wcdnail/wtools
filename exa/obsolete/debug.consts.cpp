#include "stdafx.h"
#include "debug.consts.h"

namespace Debug
{
    namespace Consts
    {
        #define _RET_WSTRING(N) case N: return L#N

        PCWSTR GetSysColorName(int i)
        {
            switch (i)
            {
                _RET_WSTRING(COLOR_SCROLLBAR         );
                _RET_WSTRING(COLOR_BACKGROUND        );
                _RET_WSTRING(COLOR_ACTIVECAPTION     );
                _RET_WSTRING(COLOR_INACTIVECAPTION   );
                _RET_WSTRING(COLOR_MENU              );
                _RET_WSTRING(COLOR_WINDOW            );
                _RET_WSTRING(COLOR_WINDOWFRAME       );
                _RET_WSTRING(COLOR_MENUTEXT          );
                _RET_WSTRING(COLOR_WINDOWTEXT        );
                _RET_WSTRING(COLOR_CAPTIONTEXT       );
                _RET_WSTRING(COLOR_ACTIVEBORDER      );
                _RET_WSTRING(COLOR_INACTIVEBORDER    );
                _RET_WSTRING(COLOR_APPWORKSPACE      );
                _RET_WSTRING(COLOR_HIGHLIGHT         );
                _RET_WSTRING(COLOR_HIGHLIGHTTEXT     );
                _RET_WSTRING(COLOR_BTNFACE           );
                _RET_WSTRING(COLOR_BTNSHADOW         );
                _RET_WSTRING(COLOR_GRAYTEXT          );
                _RET_WSTRING(COLOR_BTNTEXT           );
                _RET_WSTRING(COLOR_INACTIVECAPTIONTEXT);
                _RET_WSTRING(COLOR_BTNHIGHLIGHT      );

#if(WINVER >= 0x0400)
                _RET_WSTRING(COLOR_3DDKSHADOW        );
                _RET_WSTRING(COLOR_3DLIGHT           );
                _RET_WSTRING(COLOR_INFOTEXT          );
                _RET_WSTRING(COLOR_INFOBK            );
#endif

#if(WINVER >= 0x0500)
                _RET_WSTRING(COLOR_HOTLIGHT          );
                _RET_WSTRING(COLOR_GRADIENTACTIVECAPTION);
                _RET_WSTRING(COLOR_GRADIENTINACTIVECAPTION);
#if(WINVER >= 0x0501)
                _RET_WSTRING(COLOR_MENUHILIGHT       );
                _RET_WSTRING(COLOR_MENUBAR           );
#endif
#endif
            }

            return L"";
        }
    }
}
