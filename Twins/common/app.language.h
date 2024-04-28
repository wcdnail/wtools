#ifndef _CF_app_language_h__
#define _CF_app_language_h__

#ifdef _WIN32
#include <WinNls.h>
#endif

namespace CF  // Common framework
{
    class Language
    {
    public:
        Language(unsigned int id = GetSystemLanguage());
        ~Language();

        void SetId(unsigned int id) { id_ = id; }
        unsigned int GetId() const { return id_; }
        unsigned int& GetIdRef() { return id_; }

        static unsigned int GetSystemLanguage();

    private:
        unsigned int id_;

    private:
        Language(Language const&);
        Language& operator = (Language const&);
    };

    inline Language::Language(unsigned int id /* = 0 */)
        : id_(id)
    {}

    inline Language::~Language()
    {}

    inline unsigned int Language::GetSystemLanguage()
    {
#ifdef _WIN32
#ifdef _DEBUG
        LANGID temp = ::GetUserDefaultLangID();
        WORD priId = PRIMARYLANGID(temp);
        WORD subId = SUBLANGID(temp);
        return (unsigned int)MAKELANGID(priId, subId);
#else
        return (unsigned int)::GetUserDefaultLangID();
#endif
#else
        return 0;
#endif
    }

    inline Language& AppLanguage()
    {
        static Language appLang;
        return appLang;
    }
}

#endif // _CF_app_language_h__
