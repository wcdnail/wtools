#include "stdafx.h"
#include "twins.h"
#include "brute_cast.h"
#include "res/resource.h"
#include "options.all.pages.h"
#include <twins.langs/twins.lang.strings.h>
#include <wtl.controls.helpers.h>
#include <atlconv.h>

namespace Twins
{
    /// Template option page //////////////////////////////////////////////////////////////////////

    _Template_Options::_Template_Options()
        : OptionPage((UINT)-1)
    {}

    _Template_Options::~_Template_Options() {}
    void _Template_Options::Init() {}
    bool _Template_Options::ApplyChanges() { return false; }
    bool _Template_Options::HaveChanges() const { return false; }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    /// File manager //////////////////////////////////////////////////////////////////////////////

    FileManagerOptions::FileManagerOptions() 
        : OptionPage(IDD_APPCONF_FILEMANAGER)
        , CbLang()
        , CkShowHidden()
        , CurrentLanguageId(App().LanguageId)
    {}

    FileManagerOptions::~FileManagerOptions()
    {}

    void FileManagerOptions::LoadLanguages()
    {
        struct LangDef
        {
            PCTSTR Name;
            unsigned Id;
        };

        LangDef lang[] = 
        { 
          { _LTS(StrId_Russian), MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT) }
        , { _LTS(StrId_English), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT) }
        };

        int current = 0;
        for (size_t i = 0; i<_countof(lang); i++)
        {
            int n = CbLang.AddString(lang[i].Name);
            CbLang.SetItemDataPtr(n, brute_cast<void*>(lang[i].Id));

            if (App().LanguageId == lang[i].Id)
            {
                CurrentLanguageId = lang[i].Id;
                current = n;
            }
        }

        Helpers::SetDefaultValue(CbLang, current);
    }

    void FileManagerOptions::Init()
    {
        CkShowHidden.Attach(GetDlgItem(IDC_CKSHOWHIDDEN));
        CbLang.Attach(GetDlgItem(IDC_CBLANG));

      //LocalizeControl(this->m_hWnd, IDC_STLANG);
      //LocalizeControl(CkShowHidden);

        LoadLanguages();
        Helpers::SetDefaultValue(CkShowHidden, !App().ShowHidden);
    }

    bool FileManagerOptions::ApplyChanges()
    {
        App().SetLocale(Helpers::GetCurrentValueDataAs<unsigned>(CbLang));
        App().ShowHidden = !Helpers::GetCurrentValue(CkShowHidden);
        return true;
    }

    bool FileManagerOptions::IsLanguageChaged() const
    {
        return CurrentLanguageId != App().LanguageId;
    }

    bool FileManagerOptions::HaveChanges() const
    {
        bool languageChanged = App().LanguageId != Helpers::GetCurrentValueDataAs<unsigned>(CbLang);
        bool hiddenSwitch = App().ShowHidden != (!Helpers::GetCurrentValue(CkShowHidden));

        return languageChanged || hiddenSwitch;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /// Appearance ////////////////////////////////////////////////////////////////////////////////

    AppearanceOptions::AppearanceOptions()
        : OptionPage(IDD_APPCONF_APPEARANCE)
    {}

    AppearanceOptions::~AppearanceOptions()
    {}

    void AppearanceOptions::Init()
    {}

    bool AppearanceOptions::ApplyChanges()
    {
        return false;
    }

    bool AppearanceOptions::HaveChanges() const
    {
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
}