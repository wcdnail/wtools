#pragma once

#include "options.page.h"

namespace Twins
{
    /// Template option page //////////////////////////////////////////////////////////////////////

    class _Template_Options: public OptionPage
    {
    public:
        _Template_Options();
        virtual ~_Template_Options();

    private:
        virtual void Init();
        virtual bool ApplyChanges();
        virtual bool HaveChanges() const;
    };

    /// File manager //////////////////////////////////////////////////////////////////////////////

    class FileManagerOptions: public OptionPage
    {
    public:
        FileManagerOptions();
        virtual ~FileManagerOptions();

        bool IsLanguageChaged() const;

    private:
        CComboBox CbLang;
        CButton CkShowHidden;
        unsigned CurrentLanguageId;

        virtual void Init();
        virtual bool ApplyChanges();
        virtual bool HaveChanges() const;
        void LoadLanguages();
    };

    /// Appearance ////////////////////////////////////////////////////////////////////////////////

    class AppearanceOptions: public OptionPage
    {
    public:
        AppearanceOptions();
        virtual ~AppearanceOptions();

    private:
        virtual void Init();
        virtual bool ApplyChanges();
        virtual bool HaveChanges() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
}
