#include "stdafx.h"
#include "dialogz.selunsel.h"
#include <twins.lang.strings.h>

namespace Twins
{
    SelUnselDialog::SelUnselDialog(PCWSTR caption, PCWSTR rx, StringVec const& history)
        : Super(IDD_SELUNSEL, caption)
        , rx_(rx)
        , rxBox_()
        , rxList_()
        , ignoreCase_(TRUE)
        , selectFiles_(TRUE)
        , selectDirs_(TRUE)
        , closeAfterSelectTemplate_(TRUE)
        , history_(history)
        , MyFixedFont(CreateFont(-13, 0, 0, 0, FW_MEDIUM, 0, 0, 0, RUSSIAN_CHARSET, 0, 0, DEFAULT_QUALITY, 0, _T("Courier New")))
    {
    }
        
    SelUnselDialog::~SelUnselDialog()
    {
    }

    BOOL SelUnselDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        DDX_TEXT(IDC_RXCOMBO, rx_);
        DDX_CHECK(IDC_IGNORECASE, ignoreCase_);
        DDX_CHECK(IDC_FILES, selectFiles_);
        DDX_CHECK(IDC_DIRS, selectDirs_);
        DDX_CHECK(IDC_CLOSEDLG, closeAfterSelectTemplate_);
        
        return TRUE;
    }

    void SelUnselDialog::InitTemplates()
    {
        rxList_.AddString(_LS(StrId_Exts_All));
        rxList_.AddString(_LS(StrId_Exts_Withoutextension));
        rxList_.AddString(_LS(StrId_Exts_Executables));
        rxList_.AddString(_LS(StrId_Exts_Components));
        rxList_.AddString(_LS(StrId_Exts_Pictures));
        rxList_.AddString(_LS(StrId_Exts_Audio));
        rxList_.AddString(_LS(StrId_Exts_Video));
        rxList_.AddString(_LS(StrId_Exts_Sources));
        rxList_.AddString(_LS(StrId_Exts_SourcesC));
        rxList_.AddString(_LS(StrId_Exts_SourcesCC));
    }

    void SelUnselDialog::InitHistory()
    {
        for (StringVec::const_iterator it = history_.begin(); it != history_.end(); ++it)
            rxBox_.AddString(it->c_str());
    }

    BOOL SelUnselDialog::OnInitDialog(HWND, LPARAM param)
    {
        DoDataExchange(DDX_LOAD);

        LocalizeControl(m_hWnd, IDC_PROMPTHINT);
        LocalizeControl(m_hWnd, IDC_IGNORECASE);
        LocalizeControl(m_hWnd, IDC_FILES);
        LocalizeControl(m_hWnd, IDC_DIRS);
        LocalizeControl(m_hWnd, IDC_CLOSEDLG);
        LocalizeControl(m_hWnd, IDC_GPTEMPLATES);

        rxBox_.Attach(GetDlgItem(IDC_RXCOMBO));
        rxList_.Attach(GetDlgItem(IDC_LSTEMPLATES));

        InitHistory();

        rxList_.SetFont(MyFixedFont, FALSE);
        InitTemplates();

        AddButton(L"Ok", IDOK);
        AddButton(_LS(StrId_Cancel), IDCANCEL);

        return TRUE;
    }

    void SelUnselDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        SetMsgHandled(FALSE);
    }

    void SelUnselDialog::OnTemplate(UINT, int, HWND)
    {
        int i = rxList_.GetCurSel();
        if (LB_ERR != i)
        {
            CString text;
            rxList_.GetText(i, text);
            int s = text.Find(L'|');
            if (s > 0)
            {
                s += 2;
                rx_ = text.Right(text.GetLength() - s);

                DoDataExchange(DDX_LOAD, IDC_RXCOMBO);
                DoDataExchange(DDX_SAVE, IDC_CLOSEDLG);

                if (closeAfterSelectTemplate_)
                    EndDialog(ResultOk);
            }
        }
    }

    void SelUnselDialog::GetValues(std::wstring& rx, bool& selectFiles, bool& selectDirs, bool& ignoreCase) const
    {
        rx = (PCWSTR)rx_;
        selectFiles = TRUE == selectFiles_;
        selectDirs = TRUE == selectDirs_;
        ignoreCase = TRUE == ignoreCase_;
    }


#if 0
Аудио_SearchFor=*.AAC; *.AC3; *.AIF; .AIMPPL; *.APE; *.AU; *.CDA; *.CUE; *.DTS; *.FLA; *.FLAC; *.IT; *.KAR; *.M3U; *.M3U8; *.M4A; *.M4B; *.MAC; *.MED; *.MID; *.MIDI; *.MO3; *.MOD; *.MP+; *.MP1; *.MP2; *.MP3; *.MPA; *.MPC; *.MPP; *.MTM; *.OFS; *.OFR; *.OGA; *.OGG; *.PLC; *.RA; *.RMI; *.RMX; *.RV; *.S3M; *.SND; *.SPC; *.SPX; *.TAK; *.TTA; *.UMX; *.VOC; *.VVS; *.WAV; *.WMA; *.WV; *.XM; *.XSPF
Аудио_SearchIn=
Аудио_SearchText=
Аудио_SearchFlags=0|00000200||||||||22220|0000
Видео_SearchFor=*.3G2; *.3GP2; *.3GP; *.3GPP; *.amr; *.amv; *.asf; *.avi; *.bik; *.divx; *.dpg; *.dvr-ms; *.evo; *.ifo; *.flv; *.k3g; *.m1v; *.m2v; *.m2t; *.m2ts; *.m4v; *.m4p; *.m4b; *.mkv; *.mov; *.mp4; *.mpeg; *.mpg; *.mpe; *.mpv2; *.mp2v; *.mts; *.mxf; *.nsr; *.nsv; *.ogm; *.ogv; *.rm; *.ram; *.rmvb; *.rpm; *.rv; *.qt; *.skm; *.swf; *.ts; *.tp; *.tpr; *.trp; *.vob; *.wm; *.wmp; *.wmv
Видео_SearchIn=
Видео_SearchText=
Видео_SearchFlags=0|00000200||||||||22220|0000
Книги_SearchFor=*.CBR; *.CBZ; *.DCX; *.DJV; *.DJVU; *.FB2; *.FB2.ZIP; *.JB2; *.JBIG2; *.PCX; *.PDB; *.PDF; *.TCR; *.XPS; *.WWF
Книги_SearchIn=
Книги_SearchText=
Книги_SearchFlags=0|00000200||||||||22220|0000
Изображения_SearchFor=*.ARW; *.BMP; *.CR2; *.CRW; *.CUR; *.DNG; *.EPS; *.GIF; *.J2K; *.JP2; *.JPG; *.JPE; *.JPEG; *.ICO; *.MRW; *.NEF; *.ORF; *.PCX; *.PEF; *.PNG; *.PPM; *.PSD; *.RAF; *.RW2; *.SR2; *.SRF; *.TGA; *.TIF; *.TIFF; *.WMF; *.X3F
Изображения_SearchIn=
Изображения_SearchText=
Изображения_SearchFlags=0|00000200||||||||22220|0000
Интернет_SearchFor=*.CSS; *.HTM; *.HTML; *.JHTML; *.MHT; *.PHP; *.PHP3; *.PHTM; *.PHTML; *.SHT; *.SHTM; *.SHTML; *.URL; *.WIN; *.XHTM; *.XHTML; *.XML
Интернет_SearchIn=
Интернет_SearchText=
Интернет_SearchFlags=0|00000200002|||7|1||||22220|0000
Торрент_SearchFor=*.torrent; *.download
Торрент_SearchIn=
Торрент_SearchText=
Торрент_SearchFlags=0|00000200002|||7|1||||22220|0000

Архивы_SearchFor=*.7z;*.a0?;*.ace;*.arj;*.bz2;*.cab;*.cpio;*.edc;*.gz;*.GZip;*.ha;*.ice;*.iso;*.jar;*.lfd;*.lha;*.lof;*.lst;*.lzh;*.pk3;*.r0?;*.rar;*.rpm;*.tar;*.uc2;.uha;*.uue;*.z;*.zip;*.zoo;*.zz
Архивы_SearchIn=
Архивы_SearchText=
Архивы_SearchFlags=0|00000200||||||||22220|0000
Базы данных_SearchFor=*.db;*.dbf;*.mdb;*.xls
Базы данных_SearchIn=
Базы данных_SearchText=
Базы данных_SearchFlags=0|00000200||||||||22220|0000
Временные файлы_SearchFor=~*.*;*.~~~;$*.*;*.$??;*.bak;*.log;*.old;*.org;*.pk;*.swp;*.tmp;*.temp;*.wbk
Временные файлы_SearchIn=
Временные файлы_SearchText=
Временные файлы_SearchFlags=0|00000200002|||||||||0000
Загрузочные файлы_SearchFor=boot.bak;boot.ini;bootfont.bin;cmldr;ntdetect.com;ntldr
Загрузочные файлы_SearchIn=
Загрузочные файлы_SearchText=
Загрузочные файлы_SearchFlags=0|00000200002||||||||22220|0000
Графика_SearchFor=*.ani;*.cdr;*.clp;*.cgm;*.dib;*.drw;*.dxf;*.emf;*.icl;*.iff;*img;*.lbm;*.ldf;*.lwf;*.pbm;*.pcd;*.pgm;*.pic;*.pct;*.psp;*.qtif;*.ras;*.rgb;*.rle;*.sfw;*.wpg
Графика_SearchIn=
Графика_SearchText=
Графика_SearchFlags=0|00000200||||||||22220|0000
Исполняемые файлы_SearchFor=*.bat;*.cmd;*.com;*.cpl;*.exe;*.js;*.lnk;*.msi;*.pif;*.scr;*.vbs;*.wsf
Исполняемые файлы_SearchIn=
Исполняемые файлы_SearchText=
Исполняемые файлы_SearchFlags=0|00000200002||||||||22220|0000
Каталоги_SearchFor=
Каталоги_SearchIn=
Каталоги_SearchText=
Каталоги_SearchFlags=0|00000200002||||||||22221|0000
Плагины командера_SearchFor=*.wcx;*.wlx;*.wfx;*.wdx
Плагины командера_SearchIn=
Плагины командера_SearchText=
Плагины командера_SearchFlags=0|00000200002||||||||22220|0000
Работающие сервисы_SearchFor=*.Running
Работающие сервисы_SearchIn=\\\Services\
Работающие сервисы_SearchText=
Работающие сервисы_SearchFlags=0|00000200002|||||||||0000
Системные файлы_SearchFor=*.386;*.bin;*.cpl;*.dll;*.drv;*.ocx;*.msc;*.swp;*.sys;*.vxd
Системные файлы_SearchIn=
Системные файлы_SearchText=
Системные файлы_SearchFlags=0|00000200||||||||22220|0000
Справочные файлы_SearchFor=*.hlp;*.chm
Справочные файлы_SearchIn=
Справочные файлы_SearchText=
Справочные файлы_SearchFlags=0|00000200|||||||||0000
Страницы интернета_SearchFor=*.css;*.htm;*.html;*.jhtml;*.mht;*.php;*.php3;*.phtm;*.phtml;*.sht;*.shtm;*.shtml;*.url;*.xhtml;*.xml
Страницы интернета_SearchIn=
Страницы интернета_SearchText=
Страницы интернета_SearchFlags=0|00000200002|||7|1||||22220|0000
Текстовые файлы_SearchFor=*.bbs;*.cfg;*.doc;*.dir;*.diz;*.htm;*.html;*.inf;*.ini;*.ion;*.log;*.me;*.nfo;*.reg;*.rtf;*.rus;*.scp;*.txt;*.wri;*.!!!
Текстовые файлы_SearchIn=
Текстовые файлы_SearchText=
Текстовые файлы_SearchFlags=0|00000200||||||||22220|0000
Файлы более 2Гб_SearchFor=
Файлы более 2Гб_SearchIn=
Файлы более 2Гб_SearchText=
Файлы более 2Гб_SearchFlags=0|00000200002|||||1|2048|2|22220|0000
Файлы для чтения_SearchFor=
Файлы для чтения_SearchIn=
Файлы для чтения_SearchText=
Файлы для чтения_SearchFlags=0|00000200002||||||||21220|0000
Не старше 1 часа_SearchFor=
Не старше 1 часа_SearchIn=
Не старше 1 часа_SearchText=
Не старше 1 часа_SearchFlags=0|000002000020|||1|0|||||0000|
Не старше 1 дня_SearchFor=
Не старше 1 дня_SearchIn=
Не старше 1 дня_SearchText=
Не старше 1 дня_SearchFlags=0|000002000020|||1|1|||||0000|
Не старше 3 дней_SearchFor=
Не старше 3 дней_SearchIn=
Не старше 3 дней_SearchText=
Не старше 3 дней_SearchFlags=0|000002000020|||3|1|||||0000|
#endif // 0

}
