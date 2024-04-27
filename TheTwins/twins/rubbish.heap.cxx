#include "stdafx.h"

// FAVORITES: from commander
#if 0 
[DirMenu]
menu1=Total Commander
cmd1=cd %COMMANDER_PATH%
menu2=-
menu3=-Program Files
menu4=Program Files
cmd4=cd %PROGRAMFILES%
menu5=-x64
menu6=Program Files
cmd6=cd C:\Program Files
menu7=Program Files (x86)
cmd7=cd %PROGRAMFILES%
menu8=--
menu9=--
menu10=-Windows
menu11=Windows
cmd11=cd %WINDIR%
menu12=System
cmd12=cd %WINDIR%\system
menu13=System32
cmd13=cd %WINDIR%\System32
menu14=-x64
menu15=SysWOW64
cmd15=cd %WINDIR%\SysWOW64
menu16=Sysnative
cmd16=cd %WINDIR%\Sysnative
menu17=--
menu18=Installer
cmd18=cd %WINDIR%\Installer
menu19=inf
cmd19=cd %WINDIR%\inf
menu20=Temp
cmd20=cd %WINDIR%\Temp
menu21=Загруженные обновления
cmd21=cd %WINDIR%\SoftwareDistribution\Download
menu22=Шрифты
cmd22=cd %WINDIR%\Fonts
menu23=Обои
cmd23=cd %WINDIR%\Web\Wallpaper
menu24=Звуки
cmd24=сd %WINDIR%\Media
menu25=Курсоры
cmd25=cd %WINDIR%\Cursors
menu26=Темы
cmd26=cd %WINDIR%\Resources
menu27=--
menu28=Temp
cmd28=cd %TEMP%
menu29=-Пользователь
menu30=Пользователь
cmd30=cd %USERPROFILE%
menu31=Мои документы
cmd31=cd %$PERSONAL%
menu32=Рабочий стол
cmd32=cd %$DESKTOP%
menu33=Главное меню
cmd33=cd %$STARTMENU%
menu34=Программы
cmd34=cd %$PROGRAMS%
menu35=Автозагрузка
cmd35=cd %$STARTUP%
menu36=-
menu37=Отправить в
cmd37=cd %USERPROFILE%\SendTo
menu38=Недавние файлы
cmd38=cd %USERPROFILE%\Recent
menu39=Быстрый запуск
cmd39=cd %$APPDATA%\Microsoft\Internet Explorer\Quick Launch
menu40=Закрепленные программы на Панели задач
cmd40=cd %$APPDATA%\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar
menu41=Закрепленные программы в меню Пуск
cmd41=cd %$APPDATA%\Microsoft\Internet Explorer\Quick Launch\User Pinned\StartMenu
menu42=-
menu43=Application Data
cmd43=cd %$APPDATA%
menu44=Local Application Data
cmd44=cd %$LOCAL_APPDATA%
menu45=--
menu46=-Общие настройки
menu47=Общие документы
cmd47=cd %$COMMON_DOCUMENTS%
menu48=Рабочий стол
cmd48=cd %$COMMON_DESKTOPDIRECTORY%
menu49=Главное меню
cmd49=cd %$COMMON_STARTMENU%
menu50=Программы
cmd50=cd %$COMMON_PROGRAMS%
menu51=Автозагрузка
cmd51=cd %$COMMON_STARTUP%
menu52=-
menu53=Application Data
cmd53=cd %$COMMON_APPDATA%
menu54=--
menu55=-
menu56=-Браузеры
menu57=-Internet Explorer
menu58=Избранное
cmd58=cd %USERPROFILE%\Favorites
menu59=Куки
cmd59=cd %USERPROFILE%\Cookies
menu60=Кэш
cmd60=cd %USERPROFILE%\Local Settings\Temporary Internet Files
menu61=--
menu62=-Opera
menu63=Настройки, куки
cmd63=cd %USERPROFILE%\Application Data\Opera\Opera
menu64=Кэш
cmd64=cd %USERPROFILE%\Local Settings\Application Data\Opera\Opera\cache
menu65=--
menu66=-Firefox
menu67=Обновления
cmd67=cd %USERPROFILE%\Local Settings\Application Data\Mozilla\Firefox\Mozilla Firefox\updates
menu68=Настройки, куки
cmd68=cd %USERPROFILE%\Local Settings\Application Data\Mozilla\Firefox\Profiles
menu69=Кэш
cmd69=cd %USERPROFILE%\Local Settings\Local Settings\Application Data\Mozilla\Firefox\Profiles
menu70=--
menu71=-Chrome
menu72=Приложение
cmd72=cd %USERPROFILE%\Local Settings\Application Data\Google\Chrome\Application
menu73=Профили, история
cmd73=cd %USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default
menu74=Кэш
cmd74=cd %USERPROFILE%\Local Settings\Application Data\Google\Chrome\User Data\Default\Cache
menu75=--
menu76=-Safari
menu77=Настройки
cmd77=cd %USERPROFILE%\Application Data\Apple Computer\Preferences
menu78=История
cmd78=cd %USERPROFILE%\Application Data\Apple Computer\Safari
menu79=Куки
cmd79=cd %USERPROFILE%\Application Data\Apple Computer\Safari\Cookies
menu80=Кэш
cmd80=cd %USERPROFILE%\Local Settings\Application Data\Apple Computer\Safari
menu81=--
menu82=--
#endif


/*
struct ReplaceCondition: boost::noncopyable
{
    typedef std::vector<String> StringVec;
    typedef StringVec::const_iterator Iterator;

    enum 
    {
        Confirm,
        Replace,
        Skip,
        Older,
        AutoRenameSource,
        AutoRenameDest,
    };

    static ReplaceCondition& Instance()
    {
        static ReplaceCondition inst;
        return inst;
    }

    int& Index() { return current_; }
    Iterator Begin() const { return vec_.begin(); }
    Iterator End() const { return vec_.end(); }

private:
    ReplaceCondition()
        : vec_()
        , current_(0)
    {
        Init();
    }

    ~ReplaceCondition()
    {
    }

    StringVec vec_;
    int current_;

    void Init()
    {
        if (vec_.empty())
        {
            vec_.push_back(_L("Запрос подтверждения"            ));
            vec_.push_back(_L("Замена"                          ));
            vec_.push_back(_L("Пропуск"                         ));
            vec_.push_back(_L("Замена более старого"            ));
            vec_.push_back(_L("Авто-переименовать исходный файл"));
            vec_.push_back(_L("Авто-переименовать целевой файл" ));
        }
    }
};
*/
