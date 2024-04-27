#pragma once

#include "dialogz.common.h"
#include "basic.enumerator.h"
#include "file.list.h"
#include <history.deque.h>
#include <atltypes.h>
#include <atlddx.h>
#include <atlctrls.h>
#include <atltime.h>
#include <map>
#include <string>
#include <thread>

namespace Sh { class Imagelist; }

namespace Twins
{
    class FindEnumerator;
    class AppState;

    class FindDialog: CommonDialog
                    , CWinDataExchange<FindDialog>
                    , CDialogResize<FindDialog>
    {
    private:
        typedef CommonDialog Super;
        typedef CWinDataExchange<FindDialog> WinDdx;
        typedef CDialogResize<FindDialog> Resizer;
        
    public:
        typedef std::filesystem::path FsPath;

        enum 
        {
            ResultViewFile = CommonDialog::ResultLast + 1,
            ResultGotoEntry,
            ResultEntryesToPanel,
        };

        struct Config
        {
            Sh::Imagelist const& Icons;
            HistoryDeque Patterns;
            HistoryDeque Paths;
            HistoryDeque Texts;

            Config(Sh::Imagelist const& icons)
                : Icons(icons)
                , Patterns()
                , Paths()
                , Texts()
            {}
        };

        FindDialog(PCWSTR dirpath, Config& config, AppState& state);
        ~FindDialog();

        using Super::DoModal;
        bool Cancel();

        Fl::List& GetFiles();
        Fl::Entry const& GetChoosen() const;

    private:
        friend class WinDdx;
        friend class Resizer;
        
        class FindEnumerator: public Enumerator
        {
        public:
            ~FindEnumerator() 
            {}

            FindEnumerator(FindDialog& owner)
                : Owner(owner)
                , RootItem(TVI_ROOT)
                , LastDirItem(TVI_FIRST)
                , Inner()
            {}

            void Reset(HTREEITEM rootItem);
            void Cancel();

        private:
            typedef boost::shared_ptr<FindEnumerator> InnerPtr;
            typedef std::deque<InnerPtr> InnerDeque;

            FindDialog& Owner;
            HTREEITEM RootItem;
            HTREEITEM LastDirItem;
            InnerDeque Inner;
            
            virtual void OnEntry(Item const& file);
        };

        friend class FindEnumerator;

        Config& Conf;
        AppState& State;
        std::wstring InitialDirpath;
        std::wstring PatternString;
        WTL::CComboBox Pattern;
        WTL::CComboBox Path;
        std::wstring InitialMoreText;
        BOOL UseTextPattern;
        WTL::CComboBox TextPattern;
        WTL::CComboBox TextPatternRxType;
        std::wstring TextPatternString;
        BOOL UseCodepage;
        WTL::CComboBox Codepage;
        BOOL SearchByWholeWordsOnly;
        BOOL TextSearchIgnoringCase;
        BOOL UseDatetimeRange;
        WTL::CDateTimePickerCtrl BegDate;
        WTL::CDateTimePickerCtrl BegTime;
        WTL::CDateTimePickerCtrl EndDate;
        WTL::CDateTimePickerCtrl EndTime;
        BOOL UseNotOlder;
        WTL::CEdit NotOlder;
        WTL::CComboBox NotOlderUnit;
        BOOL UseSize;
        WTL::CComboBox SizeOp;
        WTL::CEdit Size;
        WTL::CComboBox SizeUnit;
        BOOL UseAttrs;
        BOOL IsReadOnly;
        BOOL IsDirectory;
        BOOL IsArchive;
        BOOL IsSystem;
        BOOL IsCompressed;
        BOOL IsHidden;
        BOOL IsEncrypted;
        BOOL SearchDups;
        BOOL DupByName;
        BOOL DupBySize;
        BOOL DupByContent;
        WTL::CStatic ResultsCaption;
        WTL::CTreeViewCtrl ResultsTree;
        FindEnumerator Searcher;
        std::thread SearchThread;
        WidecharString PatternRx;
        BOOL CaseSensitive;
        BOOL PatternIsRegExp;
        BOOL UseNameOnly;
        ATL::CTime MinDatetime;
        ATL::CTime MaxDatetime;
        ATL::CTime NotOlderTime;
        uint64_t SizeComparand;
        int SizeOperator;
        unsigned TextSearchRxFlags;
        std::string TextSearchRxPattern;
        Fl::List Files;
        Fl::Entry Choosen;

        virtual int OnCommandById(int id);
        BOOL DoDataExchange(BOOL = FALSE, UINT = (UINT)-1);
        BOOL OnInitDialog(HWND, LPARAM lParam);
        void OnDestroy();
        void OnCommand(UINT code, int id, CWindow);
        void CheckControlsOn(UINT id);
        void ToggleControlsOn(UINT id);
        void ToggleEnable(BOOL y, UINT id);
        void ShowResultsControls();
        void PrepareSearchParams();
        void PreparePatternAndPath();
        void PreparePatternRegExp();
        void PrepareTextPattern();
        void PrepareDatetimeRange();
        void PrepareNotOlder();
        void PrepareSize();
        void Search();
        void SearchProc();
        void ResetResults();
        void ToggleAllControls(BOOL y);
        void ToggleOptionControls();
        bool IsMatchingFindCriteria(FsPath const& entryPath, Enumerator::Item const& entry) const;
        HTREEITEM AppendToTree(HTREEITEM rootItem, HTREEITEM insertAfter, std::wstring const& name, int iconIndex);
        void SearchInDir(FindEnumerator& searcher, std::wstring const& path, std::wstring const& mask, HTREEITEM rootItem, bool inner);
        bool MatchByPattern(std::wstring const& name) const;
        bool MatchByTextPattern(FsPath const& entryPath, Enumerator::Item const& entry) const;
        bool MatchByDatetimeRange(FsPath const& entryPath, Enumerator::Item const& entry) const;
        bool MatchByNotOlder(FsPath const& entryPath, Enumerator::Item const& entry) const;
        bool MatchBySize(FsPath const& entryPath, Enumerator::Item const& entry) const;
        bool MatchByAttrs(FsPath const& entryPath, Enumerator::Item const& entry) const;
        LRESULT OnBegDateChanged(LPNMHDR pnmh);
        LRESULT OnEndDateChanged(LPNMHDR pnmh);
        LRESULT OnBegTimeChanged(LPNMHDR pnmh);
        LRESULT OnEndTimeChanged(LPNMHDR pnmh);
        void IncrementValue(UINT controlId, int delta);
        LRESULT OnIncNotOlder(LPNMHDR pnmh);
        LRESULT OnIncSize(LPNMHDR pnmh);
        int GetSelected(Fl::List& files) const;
        void StoreFileData(HTREEITEM it, FsPath const& path);
        Fl::Entry const& GetSelected() const;

        BEGIN_MSG_MAP_EX(FindDialog)
            MSG_WM_COMMAND(OnCommand)
            CHAIN_MSG_MAP(Super)
            MSG_WM_INITDIALOG(OnInitDialog)
            MSG_WM_DESTROY(OnDestroy)
            NOTIFY_HANDLER_EX(IDC_DP_ENDDATE, DTN_DATETIMECHANGE, OnEndDateChanged)
            NOTIFY_HANDLER_EX(IDC_DP_BEGDATE, DTN_DATETIMECHANGE, OnBegDateChanged)
            NOTIFY_HANDLER_EX(IDC_DP_ENDTIME, DTN_DATETIMECHANGE, OnEndTimeChanged)
            NOTIFY_HANDLER_EX(IDC_DP_BEGTIME, DTN_DATETIMECHANGE, OnBegTimeChanged)
            NOTIFY_HANDLER_EX(IDC_SN_NOTOLDER, UDN_DELTAPOS, OnIncNotOlder)
            NOTIFY_HANDLER_EX(IDC_SN_SIZE, UDN_DELTAPOS, OnIncSize)
            CHAIN_MSG_MAP(Resizer)
        END_MSG_MAP()

        BEGIN_DLGRESIZE_MAP(Resizer)
            DLGRESIZE_CONTROL(ID_BUTTONS, DLSZ_MOVE_Y | DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(IDC_SEARCH_RESULTS_CAPTION, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(IDC_SEARCH_RESULT_TREE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        END_DLGRESIZE_MAP()
    };

    inline Fl::List& FindDialog::GetFiles() { return Files; }
    inline Fl::Entry const& FindDialog::GetChoosen() const { return Choosen; }
}
