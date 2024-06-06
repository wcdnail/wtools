#include "stdafx.h"
#include "dialogz.find.h"
#include "twins.state.h"
#include "file.list.h"
#include "brute_cast.h"
#include <shell.imagelist.h>
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>
#include <wtl.controls.helpers.h>
#include <atldlgs.h>
#include <atlconv.h>
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "res/resource.h"

namespace Twins
{
    enum Hardcoded
    {
        /* UTF-8             */ CodepageUtf8 = 0,
        /* UTF-16            */ CodepageUtf16,
        /* Hexadecimal       */ CodepageHex,
        
        /* секунды(секунд)   */ NotOlderSeconds = 0,
        /* минуты(минут)     */ NotOlderMinutes,
        /* часа(ов)          */ NotOlderHours,
        /* дня(дней)         */ NotOlderDays,
        /* недели(недель)    */ NotOlderWeeks,
        /* месяца(ев)        */ NotOlderMonths,
        /* года(лет)         */ NotOlderYears,
                             
        /* =                 */ SizeEq = 0,
        /* >                 */ SizeGreater,
        /* <                 */ SizeLess,
                             
        /* байт              */ SizeInBytes = 1,
        /* килобайт          */ SizeInKils = 1024,
        /* мегабайт          */ SizeInMegs = SizeInKils * SizeInKils,
        /* гигабайт          */ SizeInGigs = SizeInMegs * SizeInKils,

        /*                   */ TextPatRegExBasic = boost::regex_constants::basic,
        /*                   */ TextPatRegExExtended = boost::regex_constants::extended,
        /*                   */ TextPatRegExNormal = boost::regex_constants::normal,
        /*                   */ TextPatRegExEmacs = boost::regex_constants::emacs,
        /*                   */ TextPatRegExAwk = boost::regex_constants::awk,
        /*                   */ TextPatRegExGrep = boost::regex_constants::grep,
        /*                   */ TextPatRegExEgrep = boost::regex_constants::egrep,
    };

    FindDialog::FindDialog(PCWSTR dirpath, Config& config, AppState& state)
        : Super(IDD_FIND, _LS(StrId_Search))
        , Conf(config)
        , State(state)
        , InitialDirpath(dirpath)
        , PatternString(L"*")
        , Pattern()
        , Path()
        , InitialMoreText()
        , UseTextPattern(FALSE)
        , TextPattern()
        , TextPatternRxType()
        , TextPatternString()
        , UseCodepage(FALSE)
        , Codepage()
        , SearchByWholeWordsOnly(FALSE)
        , TextSearchIgnoringCase(FALSE)
        , UseDatetimeRange(FALSE)
        , BegDate()
        , BegTime()
        , EndDate()
        , EndTime()
        , UseNotOlder(FALSE)
        , NotOlder()
        , NotOlderUnit()
        , UseSize(FALSE)
        , SizeOp()
        , Size()
        , SizeUnit()
        , UseAttrs(FALSE)
        , IsReadOnly(FALSE)
        , IsDirectory(FALSE)
        , IsArchive(FALSE)
        , IsSystem(FALSE)
        , IsCompressed(FALSE)
        , IsHidden(FALSE)
        , IsEncrypted(FALSE)
        , SearchDups(FALSE)
        , DupByName(FALSE)
        , DupBySize(FALSE)
        , DupByContent(FALSE)
        , ResultsCaption()
        , ResultsTree()
        , Searcher(*this)
        , SearchThread()
        , PatternRx()
        , CaseSensitive(FALSE)
        , PatternIsRegExp(FALSE)
        , UseNameOnly(TRUE)
        , MinDatetime()
        , MaxDatetime()
        , NotOlderTime()
        , SizeComparand(0)
        , SizeOperator(0)
        , TextSearchRxFlags(0)
        , TextSearchRxPattern("")
    {}

    FindDialog::~FindDialog()
    {}

    void FindDialog::ToggleAllControls(BOOL y)
    {
        ToggleEnable(y, IDC_SEARCH_PATTERN_CAPTION);
        ToggleEnable(y, IDC_SEARCH_PATH_STARTFROM_CAPTION);
        ToggleEnable(y, IDC_SEARCH_PATTERN);
        ToggleEnable(y, IDC_PATTERN_IS_REGEXP);
        ToggleEnable(y, IDC_PATTERN_CASE_SENSITIVE);
        ToggleEnable(y, IDC_PATTERN_CMP_WITH_NAME);
        ToggleEnable(y, IDC_SEARCH_PATH_STARTFROM);
        ToggleEnable(y, IDC_ADJUST_PATH_FROM);
        ToggleEnable(y, IDC_USE_TEXT_PATTERN);
        ToggleEnable(y, IDC_TEXT_PATTERN);
        ToggleEnable(y, IDC_ST_STEXT_REGEXTYPE);
        ToggleEnable(y, IDC_CB_STEXT_REGEXTYPE);
        ToggleEnable(y, IDC_CK_CODEPAGE);
        ToggleEnable(y, IDC_CB_CODEPAGE);
        ToggleEnable(y, IDC_CK_WHOLETEXT);
        ToggleEnable(y, IDC_CK_CASESENS);
        ToggleEnable(y, IDC_USE_DATETIME_RANGE);
        ToggleEnable(y, IDC_DP_BEGDATE);
        ToggleEnable(y, IDC_DP_BEGTIME);
        ToggleEnable(y, IDC_DATETIME_SINCE_CAPTION);
        ToggleEnable(y, IDC_DP_ENDDATE);
        ToggleEnable(y, IDC_DP_ENDTIME);
        ToggleEnable(y, IDC_USE_NOT_OLDER);
        ToggleEnable(y, IDC_EB_NOTOLDER);
        ToggleEnable(y, IDC_CB_NOTOLDER);
        ToggleEnable(y, IDC_USE_SIZE);
        ToggleEnable(y, IDC_CB_SIZEOP);
        ToggleEnable(y, IDC_EB_SIZE);
        ToggleEnable(y, IDC_CB_SIZEUNIT);
        ToggleEnable(y, IDC_USE_ATTRIBUTES);
        ToggleEnable(y, IDC_CK_ARDONLY);
        ToggleEnable(y, IDC_CK_ADIR);
        ToggleEnable(y, IDC_CK_AARCHIVE);
        ToggleEnable(y, IDC_CK_ASYS);
        ToggleEnable(y, IDC_CK_ACOMPRESSED);
        ToggleEnable(y, IDC_CK_AHIDDEN);
        ToggleEnable(y, IDC_CK_AENCRYPTED);
        ToggleEnable(y, IDC_SEARCH_DUPS);
        ToggleEnable(y, IDC_CK_DUP_BYNAME);
        ToggleEnable(y, IDC_CK_DUP_BYSIZE);
        ToggleEnable(y, IDC_CK_DUP_BYCONTENT);
    }

    void FindDialog::ToggleOptionControls()
    {
        ToggleControlsOn(IDC_USE_TEXT_PATTERN);
        ToggleControlsOn(IDC_USE_DATETIME_RANGE);
        ToggleControlsOn(IDC_USE_NOT_OLDER);
        ToggleControlsOn(IDC_USE_SIZE);
        ToggleControlsOn(IDC_USE_ATTRIBUTES);
    }

    BOOL FindDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        DDX_CONTROL_HANDLE(IDC_SEARCH_PATTERN, Pattern);
        DDX_CHECK(IDC_PATTERN_IS_REGEXP, PatternIsRegExp);
        DDX_CHECK(IDC_PATTERN_CASE_SENSITIVE, CaseSensitive);
        DDX_CHECK(IDC_PATTERN_CMP_WITH_NAME, UseNameOnly);
        DDX_CONTROL_HANDLE(IDC_SEARCH_PATH_STARTFROM, Path);
        DDX_CHECK(IDC_USE_TEXT_PATTERN, UseTextPattern);
        DDX_CONTROL_HANDLE(IDC_TEXT_PATTERN, TextPattern);
        DDX_CONTROL_HANDLE(IDC_CB_STEXT_REGEXTYPE, TextPatternRxType)
        DDX_CHECK(IDC_CK_CODEPAGE, UseCodepage);
        DDX_CONTROL_HANDLE(IDC_CB_CODEPAGE, Codepage);
        DDX_CHECK(IDC_CK_WHOLETEXT, SearchByWholeWordsOnly);
        DDX_CHECK(IDC_CK_CASESENS, TextSearchIgnoringCase);
        DDX_CHECK(IDC_USE_DATETIME_RANGE, UseDatetimeRange);
        DDX_CONTROL_HANDLE(IDC_DP_BEGDATE, BegDate);
        DDX_CONTROL_HANDLE(IDC_DP_BEGTIME, BegTime);
        DDX_CONTROL_HANDLE(IDC_DP_ENDDATE, EndDate);
        DDX_CONTROL_HANDLE(IDC_DP_ENDTIME, EndTime);
        DDX_CHECK(IDC_USE_NOT_OLDER, UseNotOlder);
        DDX_CONTROL_HANDLE(IDC_EB_NOTOLDER, NotOlder);
        DDX_CONTROL_HANDLE(IDC_CB_NOTOLDER, NotOlderUnit);
        DDX_CHECK(IDC_USE_SIZE, UseSize);
        DDX_CONTROL_HANDLE(IDC_CB_SIZEOP, SizeOp);
        DDX_CONTROL_HANDLE(IDC_EB_SIZE, Size);
        DDX_CONTROL_HANDLE(IDC_CB_SIZEUNIT, SizeUnit);
        DDX_CHECK(IDC_USE_ATTRIBUTES, UseAttrs);
        DDX_CHECK(IDC_CK_ARDONLY, IsReadOnly);
        DDX_CHECK(IDC_CK_ADIR, IsDirectory);
        DDX_CHECK(IDC_CK_AARCHIVE, IsArchive);
        DDX_CHECK(IDC_CK_ASYS, IsSystem);
        DDX_CHECK(IDC_CK_ACOMPRESSED, IsCompressed);
        DDX_CHECK(IDC_CK_AHIDDEN, IsHidden);
        DDX_CHECK(IDC_CK_AENCRYPTED, IsEncrypted);
        DDX_CHECK(IDC_SEARCH_DUPS, SearchDups);
        DDX_CHECK(IDC_CK_DUP_BYNAME, DupByName);
        DDX_CHECK(IDC_CK_DUP_BYSIZE, DupBySize);
        DDX_CHECK(IDC_CK_DUP_BYCONTENT, DupByContent);
        DDX_CONTROL_HANDLE(IDC_SEARCH_RESULTS_CAPTION, ResultsCaption);
        DDX_CONTROL_HANDLE(IDC_SEARCH_RESULT_TREE, ResultsTree);

        return TRUE;
    }

    BOOL FindDialog::OnInitDialog(HWND, LPARAM lParam)
    {
        DoDataExchange(DDX_LOAD);

        if (InitialDirpath.empty())
        {
            wchar_t temp[65536] = {0};
            DWORD lr = ::GetCurrentDirectoryW(_countof(temp)-1, temp);
            InitialDirpath = temp;
        }

        Conf.Paths.LoadTo(Path);

        if (PatternString.empty())
            PatternString = L"*";

        if (Conf.Patterns.Storage.empty())
            Conf.Patterns.Push(PatternString);

        Conf.Patterns.LoadTo(Pattern);
        Pattern.SetCurSel(0);

        CString tempPattern;
        Pattern.GetLBText(0, tempPattern);
        PatternString = tempPattern;

        Path.SetWindowText(InitialDirpath.c_str());
        Pattern.SetWindowText(PatternString.c_str());

        // Search by text pattern
        Conf.Texts.LoadTo(TextPattern);
        Helpers::SetDefaultValue(TextPattern, 0);

        Helpers::AddTo(TextPatternRxType, TextPatRegExBasic, _LS(StrId_Simple));
        Helpers::AddTo(TextPatternRxType, TextPatRegExExtended, _LS(StrId_Extended)); 
        Helpers::AddTo(TextPatternRxType, TextPatRegExNormal, _LS(StrId_Normal));
        Helpers::AddTo(TextPatternRxType, TextPatRegExEmacs, _LS(StrId_Emacs));
        Helpers::AddTo(TextPatternRxType, TextPatRegExAwk, _LS(StrId_Awk));
        Helpers::AddTo(TextPatternRxType, TextPatRegExGrep, _LS(StrId_Grep));
        Helpers::AddTo(TextPatternRxType, TextPatRegExEgrep, _LS(StrId_Egrep));
        Helpers::SetDefaultValue(TextPatternRxType, 0);

        Helpers::AddTo(Codepage, CodepageUtf8, L"UTF-8");
        Helpers::AddTo(Codepage, CodepageUtf16, L"UTF-16");
        Helpers::AddTo(Codepage, CodepageHex, L"Hexadecimal");
        Helpers::SetDefaultValue(Codepage, 0);

        // Search by Date-time range
        Helpers::AddTo(NotOlderUnit, NotOlderSeconds, _LS(StrId_Seconds));
        Helpers::AddTo(NotOlderUnit, NotOlderMinutes, _LS(StrId_Minutes));
        Helpers::AddTo(NotOlderUnit, NotOlderHours, _LS(StrId_Hours));
        Helpers::AddTo(NotOlderUnit, NotOlderDays, _LS(StrId_Days));
        Helpers::AddTo(NotOlderUnit, NotOlderWeeks, _LS(StrId_Weeks));
        Helpers::AddTo(NotOlderUnit, NotOlderMonths, _LS(StrId_Months));
        Helpers::AddTo(NotOlderUnit, NotOlderYears, _LS(StrId_Years));
        Helpers::SetDefaultValue(NotOlderUnit, 0);

        // Search by NotOlder
        SetDlgItemInt(IDC_EB_NOTOLDER, 1);

        // Search by Size
        Helpers::AddTo(SizeOp, SizeEq, L"=");
        Helpers::AddTo(SizeOp, SizeGreater, L">");
        Helpers::AddTo(SizeOp, SizeLess, L"<");
        Helpers::SetDefaultValue(SizeOp, 0);

        Helpers::AddTo(SizeUnit, SizeInBytes, _LS(StrId_Bytes));
        Helpers::AddTo(SizeUnit, SizeInKils, _LS(StrId_Kilobytes));
        Helpers::AddTo(SizeUnit, SizeInMegs, _LS(StrId_Megabytes));
        Helpers::AddTo(SizeUnit, SizeInGigs, _LS(StrId_Gigabytes));
        Helpers::SetDefaultValue(SizeUnit, 0);

        SetDlgItemInt(IDC_EB_SIZE, 1);

        LocalizeDlgControl(m_hWnd, IDC_SEARCH_PATTERN_CAPTION, StrId_Pattern);
        LocalizeDlgControl(m_hWnd, IDC_PATTERN_IS_REGEXP, StrId_Regularexpression);
        LocalizeDlgControl(m_hWnd, IDC_PATTERN_CASE_SENSITIVE, StrId_Casesensitive);
        LocalizeDlgControl(m_hWnd, IDC_PATTERN_CMP_WITH_NAME, 0);
        LocalizeDlgControl(m_hWnd, IDC_SEARCH_PATH_STARTFROM_CAPTION, 0);
        LocalizeDlgControl(m_hWnd, IDC_ADJUST_PATH_FROM, 0);
        LocalizeDlgControl(m_hWnd, IDC_USE_TEXT_PATTERN, 0);
        LocalizeDlgControl(m_hWnd, IDC_ST_STEXT_REGEXTYPE, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_CODEPAGE, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_WHOLETEXT, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_CASESENS, 0);
        LocalizeDlgControl(m_hWnd, IDC_USE_DATETIME_RANGE, 0);
        LocalizeDlgControl(m_hWnd, IDC_DATETIME_SINCE_CAPTION, 0);
        LocalizeDlgControl(m_hWnd, IDC_USE_NOT_OLDER, 0);
        LocalizeDlgControl(m_hWnd, IDC_USE_SIZE, 0);
        LocalizeDlgControl(m_hWnd, IDC_USE_ATTRIBUTES, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_ARDONLY, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_ADIR, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_AARCHIVE, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_ASYS, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_ACOMPRESSED, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_AHIDDEN, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_AENCRYPTED, 0);
        LocalizeDlgControl(m_hWnd, IDC_SEARCH_DUPS, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_DUP_BYNAME, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_DUP_BYSIZE, 0);
        LocalizeDlgControl(m_hWnd, IDC_CK_DUP_BYCONTENT, 0);
        LocalizeDlgControl(m_hWnd, IDC_SEARCH_RESULTS_CAPTION, 0);

        AddButton(_LS(StrId_Start), ResultStart);
        AddButton(_LS(StrId_View), ResultViewFile);
        AddButton(_LS(StrId_Goto), ResultGotoEntry);
        AddButton(_LS(StrId_Topanel), ResultEntryesToPanel);
        AddButton(_LS(StrId_Cancel), ResultCancel);

        ToggleOptionControls();

        ResultsTree.ModifyStyleEx(0, 0x0004 // TVS_EX_DOUBLEBUFFER
                                   | 0x0040 // TVS_EX_FADEINOUTEXPANDOS
                                   | 0x0200 // TVS_EX_DIMMEDCHECKBOXES 
                                   );

        //ResultsTree.ModifyStyle(TVS_HASLINES, TVS_FULLROWSELECT);
        ResultsTree.ModifyStyle(TVS_FULLROWSELECT, TVS_HASLINES);
        ResultsTree.SetImageList(Conf.Icons.Get(), LVSIL_NORMAL);

        DlgResize_Init(false, true);
        return TRUE;
    }

    void FindDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        Choosen = GetSelected();
        Super::OnDestroy();
    }

    void FindDialog::ToggleEnable(BOOL y, UINT id) 
    {
        ::EnableWindow(::GetDlgItem(m_hWnd, id), y); 
    }

    void FindDialog::ToggleControlsOn(UINT id)
    {
        switch (id)
        {
        case IDC_CK_CODEPAGE:
            ToggleEnable(UseCodepage, IDC_CB_CODEPAGE);
            break;

        case IDC_USE_TEXT_PATTERN:
            ToggleEnable(UseTextPattern, IDC_TEXT_PATTERN);
            ToggleEnable(UseTextPattern, IDC_ST_STEXT_REGEXTYPE);
            ToggleEnable(UseTextPattern, IDC_CB_STEXT_REGEXTYPE);
            ToggleEnable(UseTextPattern, IDC_CK_CODEPAGE);
            ToggleEnable(UseCodepage, IDC_CB_CODEPAGE);
            ToggleEnable(UseTextPattern, IDC_CK_WHOLETEXT);
            ToggleEnable(UseTextPattern, IDC_CK_CASESENS);
            break;

        case IDC_USE_DATETIME_RANGE:
            ToggleEnable(UseDatetimeRange, IDC_DP_BEGDATE);
            ToggleEnable(UseDatetimeRange, IDC_DP_BEGTIME);
            ToggleEnable(UseDatetimeRange, IDC_DATETIME_SINCE_CAPTION);
            ToggleEnable(UseDatetimeRange, IDC_DP_ENDDATE);
            ToggleEnable(UseDatetimeRange, IDC_DP_ENDTIME);
            break;

        case IDC_USE_NOT_OLDER:
            ToggleEnable(UseNotOlder, IDC_EB_NOTOLDER);
            ToggleEnable(UseNotOlder, IDC_SN_NOTOLDER);
            ToggleEnable(UseNotOlder, IDC_CB_NOTOLDER);
            break;

        case IDC_USE_SIZE:
            ToggleEnable(UseSize, IDC_CB_SIZEOP);
            ToggleEnable(UseSize, IDC_EB_SIZE);
            ToggleEnable(UseSize, IDC_SN_SIZE);
            ToggleEnable(UseSize, IDC_CB_SIZEUNIT);
            break;

        case IDC_USE_ATTRIBUTES:
            ToggleEnable(UseAttrs, IDC_CK_ARDONLY);
            ToggleEnable(UseAttrs, IDC_CK_ADIR);
            ToggleEnable(UseAttrs, IDC_CK_AARCHIVE);
            ToggleEnable(UseAttrs, IDC_CK_ASYS);
            ToggleEnable(UseAttrs, IDC_CK_ACOMPRESSED);
            ToggleEnable(UseAttrs, IDC_CK_AHIDDEN);
            ToggleEnable(UseAttrs, IDC_CK_AENCRYPTED);
            break;

        case IDC_SEARCH_DUPS:
            ToggleAllControls(!SearchDups);
            ToggleOptionControls();

            ToggleEnable(TRUE, IDC_SEARCH_DUPS);
            ToggleEnable(SearchDups, IDC_CK_DUP_BYNAME);
            ToggleEnable(SearchDups, IDC_CK_DUP_BYNAME);
            ToggleEnable(SearchDups, IDC_CK_DUP_BYSIZE);
            ToggleEnable(SearchDups, IDC_CK_DUP_BYCONTENT);
            break;
        }
    }

    void FindDialog::CheckControlsOn(UINT id)
    {
        DoDataExchange(TRUE, id);
        ToggleControlsOn(id);
    }

    void FindDialog::OnCommand(UINT code, int id, CWindow)
    {
        if ((IDCANCEL == id) && SearchThread.joinable())
        {
            Searcher.Cancel();
            return ;
        }

        switch (code)
        {
        case BN_CLICKED:
            OnCommandById(id);

            switch (id)
            {
            case IDC_ADJUST_PATH_FROM: 
            {
                CString temp;
                Path.GetWindowText(temp);
                InitialDirpath = temp;

                WTL::CFolderDialog dlg(*this);
                dlg.m_lpstrInitialFolder = InitialDirpath.c_str();
                INT_PTR dr = dlg.DoModal(*this);
                if (IDOK == dr)
                {
                    InitialDirpath = dlg.m_szFolderPath;
                    Path.SetWindowText(InitialDirpath.c_str());
                }
                break;
            }

            default: 
                CheckControlsOn(id);
            }

            break;
        }
    }

    bool FindDialog::Cancel()
    {
        if (SearchThread.joinable())
        {
            Searcher.Cancel();
            return true;
        }

        return false;
    }

    int FindDialog::OnCommandById(int id)
    {
        if (ResultStart == id || IDOK == id)
        {
            if (!SearchThread.joinable())
            {
                DoDataExchange(TRUE);
                Search();
            }
            return id;
        }
        else if (ResultCancel == id)
        {
            if (Cancel())
                return id;
        }
        else if (ResultViewFile == id)
        {
            Fl::List files;
            //if (GetSelected(files) > 0)
                //State.ViewFiles(files, nullptr);
        }
        else if (ResultGotoEntry == id || ResultEntryesToPanel == id)
        {
            EndDialog(id);
            return id;
        }
        
        return Super::OnCommandById(id);
    }

    void FindDialog::ResetResults()
    {
        Cancel();
        ResultsCaption.SetWindowText(_T(""));
        ResultsTree.LockWindowUpdate(TRUE);
        ResultsTree.DeleteAllItems();
        ResultsTree.LockWindowUpdate(FALSE);
        Fl::Entry().Swap(Choosen);
        Files.Clear();
        Searcher.Reset(TVI_ROOT);
    }

    static void EscapeRegex(WString& pattern)
    {
        boost::replace_all(pattern, L"\\", L"\\\\");
        boost::replace_all(pattern, L"^",  L"\\^");
        boost::replace_all(pattern, L".",  L"\\.");
        boost::replace_all(pattern, L"$",  L"\\$");
        boost::replace_all(pattern, L"|",  L"\\|");
        boost::replace_all(pattern, L"(",  L"\\(");
        boost::replace_all(pattern, L")",  L"\\)");
        boost::replace_all(pattern, L"[",  L"\\[");
        boost::replace_all(pattern, L"]",  L"\\]");
        boost::replace_all(pattern, L"*",  L"\\*");
        boost::replace_all(pattern, L"+",  L"\\+");
        boost::replace_all(pattern, L"?",  L"\\?");
        boost::replace_all(pattern, L"/",  L"\\/");
    }

    static WString RxFromWildcards(std::wstring const& pattern)
    {
        WString rx = pattern;

        EscapeRegex(rx);

        boost::replace_all(rx, L"\\?", L".");
        boost::replace_all(rx, L"\\*", L".*");

        rx = L"^" + rx + L"$";
        return rx;
    }

    static bool ContainWildcards(std::wstring const& pattern)
    {
        // ##TODO: Find full wildcard list."))
        return std::wstring::npos != pattern.find_first_of(L"*?");
    }

    static ATL::CTime GetTimeFrom(WTL::CDateTimePickerCtrl const& datePick, WTL::CDateTimePickerCtrl const& timePick)
    {
        SYSTEMTIME date{0};
        datePick.GetSystemTime(&date);

        SYSTEMTIME time{0};
        timePick.GetSystemTime(&time);

        return ATL::CTime{static_cast<int>(date.wYear),
                          static_cast<int>(date.wMonth),
                          static_cast<int>(date.wDay),
                          static_cast<int>(time.wHour),
                          static_cast<int>(time.wMinute),
                          static_cast<int>(time.wSecond),
                          -1};
    }

    static void AdjustTimeRange(WTL::CDateTimePickerCtrl& picker, LPNMDATETIMECHANGE nm, bool greater)
    {
        SYSTEMTIME st = {0};
        picker.GetSystemTime(&st);

        bool adjust = greater 
                    ? (CTime(st) > CTime(nm->st)) 
                    : (CTime(st) < CTime(nm->st))
                    ;

        if (adjust)
            picker.SetSystemTime(0, &nm->st);
    }

    LRESULT FindDialog::OnBegDateChanged(LPNMHDR pnmh)
    {
        AdjustTimeRange(EndDate, reinterpret_cast<LPNMDATETIMECHANGE>(pnmh), false);
        return 0;
    }

    LRESULT FindDialog::OnEndDateChanged(LPNMHDR pnmh)
    {
        AdjustTimeRange(BegDate, reinterpret_cast<LPNMDATETIMECHANGE>(pnmh), true);
        return 0;
    }

    LRESULT FindDialog::OnBegTimeChanged(LPNMHDR pnmh)
    {
        AdjustTimeRange(EndTime, reinterpret_cast<LPNMDATETIMECHANGE>(pnmh), false);
        return 0;
    }

    LRESULT FindDialog::OnEndTimeChanged(LPNMHDR pnmh)
    {
        AdjustTimeRange(BegTime, reinterpret_cast<LPNMDATETIMECHANGE>(pnmh), false);
        return 0;
    }

    void FindDialog::IncrementValue(UINT controlId, int delta)
    {
        int value = GetDlgItemInt(controlId);

        value -= delta;

        if (value < 0)
            value = 0;

        SetDlgItemInt(controlId, value);
    }

    LRESULT FindDialog::OnIncSize(LPNMHDR pnmh)
    {
        IncrementValue(IDC_EB_SIZE, reinterpret_cast<LPNMUPDOWN>(pnmh)->iDelta);
        return 0;
    }

    LRESULT FindDialog::OnIncNotOlder(LPNMHDR pnmh)
    {
        IncrementValue(IDC_EB_NOTOLDER, reinterpret_cast<LPNMUPDOWN>(pnmh)->iDelta);
        return 0;
    }

    void FindDialog::PreparePatternAndPath()
    {
        CString temp;
        Path.GetWindowText(temp);
        InitialDirpath = temp;

        Pattern.GetWindowText(temp);
        PatternString = temp;

        Conf.Patterns.Push(PatternString);
        Conf.Paths.Push(InitialDirpath);

        Conf.Paths.LoadTo(Path);
        Conf.Patterns.LoadTo(Pattern);
    }

    void FindDialog::PreparePatternRegExp()
    {
        PatternRx.clear();
        if (PatternIsRegExp)
            PatternRx = WString(PatternString);

        else if (ContainWildcards(PatternString))
            PatternRx = RxFromWildcards(PatternString);
    }

    void FindDialog::PrepareTextPattern()
    {
        if (UseTextPattern)
        {
            CString temp;
            TextPattern.GetWindowText(temp);
            TextPatternString = temp;

            Conf.Texts.Push(TextPatternString);

            Conf.Texts.LoadTo(TextPattern);

            TextSearchRxFlags = cast_from_ptr<unsigned>(TextPatternRxType.GetItemDataPtr(TextPatternRxType.GetCurSel()));

            if (TextSearchIgnoringCase)
                TextSearchRxFlags |= boost::regex_constants::icase;

            TextSearchRxPattern = CW2A(TextPatternString.c_str()).m_psz;
        }
    }

    void FindDialog::PrepareDatetimeRange()
    {
        if (UseDatetimeRange)
        {
            ATL::CTime const minTime{GetTimeFrom(BegDate, BegTime)};
            ATL::CTime const maxTime{GetTimeFrom(EndDate, EndTime)};
            DH::TPrintf(L"Searching", L"Datetime range in [%s .. %s]\n",
                minTime.Format(L"%d.%m.%Y %H:%M:%S"),
                maxTime.Format(L"%d.%m.%Y %H:%M:%S"));
            MinDatetime = minTime;
            MaxDatetime = maxTime;
        }
    }

    void FindDialog::PrepareNotOlder() {
        if (UseNotOlder) {
            int const   unit{cast_from_ptr<int>(NotOlderUnit.GetItemDataPtr(NotOlderUnit.GetCurSel()))};
            UINT const value{GetDlgItemInt(IDC_EB_NOTOLDER, nullptr, FALSE)};

            ATL::CTimeSpan span;
            switch (unit) {
            case NotOlderYears:   span = CTimeSpan(365 * value, 0, 0, 0); break;
            case NotOlderMonths:  span = CTimeSpan(30 * value, 0, 0, 0); break;
            case NotOlderWeeks:   span = CTimeSpan(7 * value, 0, 0, 0); break;
            case NotOlderDays:    span = CTimeSpan(value, 0, 0, 0); break;
            case NotOlderHours:   span = CTimeSpan(0, value, 0, 0); break;
            case NotOlderMinutes: span = CTimeSpan(0, 0, value, 0); break;
            case NotOlderSeconds: span = CTimeSpan(0, 0, 0, value); break;
            }
            NotOlderTime = CTime::GetCurrentTime() - span;
            DH::TPrintf(L"Searching", L"NotOlderTime %s\n", NotOlderTime.Format(L"%d.%m.%Y %H:%M:%S"));
        }
    }

    void FindDialog::PrepareSize()
    {
        if (UseSize) {
            int const   optype{cast_from_ptr<int>(SizeOp.GetItemDataPtr(SizeOp.GetCurSel()))};
            int const     unit{cast_from_ptr<int>(SizeUnit.GetItemDataPtr(SizeUnit.GetCurSel()))};
            uint64_t const size{(uint64_t)GetDlgItemInt(IDC_EB_SIZE, nullptr, FALSE)};
            SizeComparand = size * unit;
            SizeOperator  = optype;
        }
    }

    void FindDialog::PrepareSearchParams()
    {
        PreparePatternAndPath();
        PreparePatternRegExp();
        PrepareTextPattern();
        PrepareDatetimeRange();
        PrepareNotOlder();
        PrepareSize();
    }

    void FindDialog::Search()
    {
        PrepareSearchParams();
        ResetResults();
        ResultsTree.SetFocus();
        SearchThread = std::thread(&FindDialog::SearchProc, this);
    }

    void FindDialog::SearchProc()
    {
        DH::ScopedThreadLog lg(0, L"Searchin: %s", __FUNCTIONW__);

        ToggleAllControls(FALSE);

        SearchInDir(Searcher, InitialDirpath, L"*", TVI_ROOT, false);

        if (SearchDups)
            ToggleControlsOn(IDC_SEARCH_DUPS);

        else
        {
            ToggleAllControls(TRUE);
            ToggleOptionControls();
        }

        SearchThread.detach();

        Pattern.SetFocus();
    }

    static int GetShellIconIndex(WString const& path, UINT flags)
    {
        SHFILEINFO info = {0};
        ::SHGetFileInfoW(path.c_str(), -1, &info, sizeof(info), SHGFI_LINKOVERLAY | SHGFI_SYSICONINDEX | flags);
        return info.iIcon;
    }

    void FindDialog::FindEnumerator::Reset(HTREEITEM rootItem)
    {
        Enumerator::Reset();
        RootItem = rootItem;
        LastDirItem = TVI_FIRST;
        Inner.clear();
    }

    void FindDialog::FindEnumerator::Cancel()
    {
        if (!Inner.empty())
        {
            for (InnerDeque::const_iterator it = Inner.begin(); it != Inner.end(); ++it)
                (*it)->Cancel();
        }

        Enumerator::Cancel();
    }

    void FindDialog::SearchInDir(FindEnumerator& searcher, std::wstring const& path, std::wstring const& mask, HTREEITEM rootItem, bool inner)
    {
        double elapsed = -1.;
        Enumerator::Ec error;
        CStringW status;

        try
        {
            DH::Timer timer;
            Enumerator::LoadArgs params(path, mask, false, true, false);
            searcher.Reset(rootItem);
            error = searcher.Load(params);
            elapsed = timer.Seconds();

            if (TVI_ROOT != rootItem)
                ResultsTree.Expand(rootItem, TVE_EXPAND);

            if (!inner)
            {
                if (error)
                    status.Format(_LS(StrId_ErrordS), error.value(), error.message().c_str());
                else
                    status = _LS(StrId_Ready);

                status.AppendFormat(_LS(StrId_8fsec), elapsed);
                ResultsCaption.SetWindowTextW(status);
            }
        }
        catch (std::exception const& ex)
        {
            status = CA2W(ex.what());
            ResultsCaption.SetWindowTextW(status);
        }
          
        if (error) {
            DH::TPrintf(L"Searching", L"%d -> `%s` (%S)\n", error.value(), path.c_str(), error.message().c_str());
        }
    }

    HTREEITEM FindDialog::AppendToTree(HTREEITEM rootItem, HTREEITEM insertAfter, std::wstring const& name, int iconIndex)
    {
        TVINSERTSTRUCTW it = {0};

        it.hParent = rootItem;
        it.hInsertAfter = insertAfter;
        it.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        it.item.pszText = (PWSTR)name.c_str();
        it.item.cchTextMax = (int)name.length();
        it.item.iImage = iconIndex;
        it.item.iSelectedImage = iconIndex;

        return ResultsTree.InsertItem(&it);
    }

    static int GetChildCount(WTL::CTreeViewCtrl& tree, HTREEITEM item)
    {
        int count = 0;

        if (tree.ItemHasChildren(item))
        {
            item = tree.GetNextItem(item, TVGN_CHILD);
            while (item)
            {
                ++count;
                item = tree.GetNextItem(item, TVGN_NEXT);
            }
        }

        return count;
    }

    void FindDialog::FindEnumerator::OnEntry(Item const& entry) 
    {
        HTREEITEM inserted = nullptr;
        bool isEntryDirectory = entry.IsDir();

        FindDialog::FsPath entryPath = entry.Args.Root.c_str();
        entryPath /= entry.Name.c_str();
        int iconIndex = GetShellIconIndex(entryPath.c_str(), 0);

        if (isEntryDirectory)
        {
            HTREEITEM dirRoot = Owner.AppendToTree(RootItem, LastDirItem, entry.Name, iconIndex);

            InnerPtr inner(new FindEnumerator(Owner));
            Inner.push_front(inner);

            Owner.SearchInDir(*inner, entryPath.c_str(), entry.Args.Mask, dirRoot, true);

            if (GetChildCount(Owner.ResultsTree, dirRoot) < 1)
                Owner.ResultsTree.DeleteItem(dirRoot);
            else
            {
                LastDirItem = dirRoot;
                Owner.StoreFileData(dirRoot, entryPath);
            }
        }
        else if (Owner.IsMatchingFindCriteria(entryPath, entry))
        {
            HTREEITEM it = Owner.AppendToTree(RootItem, TVI_LAST, entry.Name, iconIndex);

            if ((TVI_ROOT != RootItem) && (TVIS_EXPANDED != Owner.ResultsTree.GetItemState(RootItem, TVIS_EXPANDED)))
                Owner.ResultsTree.Expand(RootItem, TVE_EXPAND);

            Owner.StoreFileData(it, entryPath);
        }
    }

    static std::wstring SelectName(FindDialog::FsPath const& entryPath, Enumerator::Item const& entry, BOOL OnlyName)
    {
        return OnlyName ? entry.Name : entryPath.wstring();
    }

    bool FindDialog::MatchByPattern(std::wstring const& name) const
    {
        bool result = false;

        if (PatternRx.empty())
        {
            wchar_t* subst = CaseSensitive 
                           ? ::wcsstr((wchar_t*)name.c_str(), PatternString.c_str()) 
                           : ::StrStrIW((wchar_t*)name.c_str(), PatternString.c_str())
                           ;
            result = nullptr != subst;
        }
        else
        {
            boost::wregex pattern(PatternRx, CaseSensitive ? boost::regex::normal : boost::regex::icase);
            result = boost::regex_match(name, pattern);
        }

        return result;
    }

    bool FindDialog::MatchByDatetimeRange(FsPath const& entryPath, Enumerator::Item const& entry) const
    {
        CTime fileTime = entry.GetAtlTime();
        return (fileTime >= MinDatetime) && (fileTime <= MaxDatetime);
    }

    bool FindDialog::MatchByNotOlder(FsPath const& entryPath, Enumerator::Item const& entry) const
    {
        CTime fileTime = entry.GetAtlTime();
        return fileTime >= NotOlderTime;
    }

    bool FindDialog::MatchBySize(FsPath const& entryPath, Enumerator::Item const& entry) const
    {
        switch (SizeOperator)
        {
        case SizeEq:        return SizeComparand == entry.Size;
        case SizeGreater:   return SizeComparand < entry.Size;
        case SizeLess:      return SizeComparand > entry.Size;
        }

        return false;
    }

    bool FindDialog::MatchByAttrs(FsPath const& entryPath, Enumerator::Item const& entry) const
    {
        unsigned attr = 0;

        if (IsReadOnly) 
            attr |= (FILE_ATTRIBUTE_READONLY & entry.Int);

        if (IsDirectory)
            attr |= (FILE_ATTRIBUTE_DIRECTORY & entry.Int);

        if (IsArchive)
            attr |= (FILE_ATTRIBUTE_ARCHIVE & entry.Int);

        if (IsSystem)
            attr |= (FILE_ATTRIBUTE_SYSTEM & entry.Int);

        if (IsCompressed)
            attr |= (FILE_ATTRIBUTE_COMPRESSED & entry.Int);

        if (IsHidden)
            attr |= (FILE_ATTRIBUTE_HIDDEN & entry.Int);

        if (IsEncrypted)
            attr |= (FILE_ATTRIBUTE_ENCRYPTED & entry.Int);

        return 0 != attr;
    }

    bool FindDialog::IsMatchingFindCriteria(FsPath const& entryPath, Enumerator::Item const& entry) const
    {
        bool result = (PatternIsRegExp ? false : (PatternString == L"*"));

        if (!result)
        {
            WString name = SelectName(entryPath, entry, UseNameOnly);
            result = MatchByPattern(name);
        }

        if (result && UseTextPattern)
            result = MatchByTextPattern(entryPath, entry);

        if (result && UseDatetimeRange)
            result = MatchByDatetimeRange(entryPath, entry);

        if (result && UseNotOlder)
            result = MatchByNotOlder(entryPath, entry);

        if (result && UseSize)
            result = MatchBySize(entryPath, entry);

        if (result && UseAttrs)
            result = MatchByAttrs(entryPath, entry);

        return result;
    }

    bool FindDialog::MatchByTextPattern(FsPath const& entryPath, Enumerator::Item const& entry) const
    {
        bool result = false;
        std::ifstream input(entryPath.c_str());

        if (input.is_open())
        {
            boost::regex rx;
            rx.assign(TextSearchRxPattern, (boost::regex_constants::syntax_option_type)TextSearchRxFlags);

            int linenum = 1;
            std::string line;
            while(std::getline(input, line))
            {
                boost::smatch what;
                result = boost::regex_search(line, what, rx);
                if (result)
                {
                    //DH::TPrintf("Searchin: `%s` at %d:%d `%s`\n"
                    //    , TextSearchRxPattern.c_str(), linenum, what.position(), line.c_str());
                    break;
                }

                ++linenum;
            }
        }

        return result;
    }

    void FindDialog::StoreFileData(HTREEITEM it, FsPath const& path)
    {
        if (nullptr != it)
        {
            Files.PushBack(path);
            int dex = Files.Count()-1;
            ResultsTree.SetItemData(it, (DWORD_PTR)dex);
        }
    }

    int FindDialog::GetSelected(Fl::List& files) const
    {
#if 0
        int count = ResultsTree.GetSelectedCount();

        Fl::List temp;
        temp.Reserve((size_t)count);

        for (int i=0; i<count; i++)
        {
            HTREEITEM it = ResultsTree.GetSelectedItem();
            if (!it)
                break;

            int dex = (int)ResultsTree.GetItemData(it);
            temp.PushBack(Files[dex]);
        }

        temp.Swap(files);
#endif // 0
        return files.Count();
    }

    Fl::Entry const& FindDialog::GetSelected() const
    {
        HTREEITEM it = ResultsTree.GetSelectedItem();
        int dex = it ? (int)ResultsTree.GetItemData(it) : -1;
        return Files[dex];
    }
}
