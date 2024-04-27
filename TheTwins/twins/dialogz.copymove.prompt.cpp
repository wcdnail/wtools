#include "stdafx.h"
#if 0
#include "dialogz.copymove.prompt.h"
#include "theme.h"
#include <locale.helper.h>
#include "res/resource.h"

namespace Twins
{
    CopyMovePromptDialog::CopyMovePromptDialog(PCWSTR caption, PCWSTR prompt, PCWSTR targetPath)
        : Super(IDD_COPYMOVEPROMPT, caption)
        , prompt_(prompt)
        , target_(targetPath)
    {
    }

    CopyMovePromptDialog::~CopyMovePromptDialog()
    {
    }

    BOOL CopyMovePromptDialog::DoDataExchange(BOOL bSaveAndValidate /*= FALSE*/, UINT nCtlID /*= (UINT)-1*/)
    {
        DDX_TEXT(IDC_PROMPTHINT, prompt_);
        DDX_TEXT(IDC_TARGETEDIT, target_);

        return TRUE;
    }

    BOOL CopyMovePromptDialog::OnInitDialog(HWND, LPARAM param)
    {
        DoDataExchange(DDX_LOAD);

        AddButton(_LS("Старт"), ResultStart, true);
        AddButton(_LS("В фоне"), ResultStartInIdle);
        AddButton(_LS("Отмена"), ResultCancel);
        AddButton(_LS("Дополнительно"));

        /*
        Qd::wdialog config(GetCaption(opType_));

        config << GetOperationPrompt()
                << Qd::wedit(destPathName_)
                << Qd::wcheck_box(_L("Копировать права доступа NTFS"), copyNtfsAccessRights_)
                <<  -( Qd::group(DefaultUiWidth / 2, 23) 
                    | Qd::wbutton(L"&Ok", Qd::qdOK, true)
                    | Qd::wbutton(_L("Отмена"), Qd::qdCancel)
                    )
                <<  -( Qd::group(DefaultUiWidth, 23) 
                    | ( Qd::wgroupbox(_L("Дополнительно"))
                        << Qd::wtext(_L("Перезапись"))
                        << Qd::wcombo_box(ReplaceCondition::Instance().Index(), ReplaceCondition::Instance().Begin(), ReplaceCondition::Instance().End())
                        << Qd::wcheck_box(_L("Пропустить недоступные"), skipInaccessible_)
                        << Qd::wcheck_box(_L("Замещать/удалять системные/только для чтения"), dontCareBoutSystem_)
                    ))
                ;

        return Qd::qdOK == config.show(parent, DefaultUiWidth);
        */

        return TRUE;
    }

    void CopyMovePromptDialog::OnDestroy()
    {
        DoDataExchange(DDX_SAVE);
        SetMsgHandled(FALSE);
    }
}
#endif // 0
