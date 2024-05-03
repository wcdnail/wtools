#pragma once

inline HIMAGELIST GetShellImageList(UINT flags = SHGFI_SYSICONINDEX | SHGFI_SMALLICON)
{
    SHFILEINFO info = {0};
    return (HIMAGELIST)::SHGetFileInfoW(L"C:\\", 0, &info, sizeof(info), flags);
}

#define _store_mask(flag, mask, result, adder)  \
    if (mask == (mask & flag))                  \
    {                                           \
        if (!result.IsEmpty())                  \
            result += L" ";                     \
                                                \
        result += adder;                        \
    }

static inline CStringW ShAttributesToString(DWORD attrs)
{
    CStringW rv;
                                                                   
    _store_mask(attrs, SFGAO_CANCOPY           , rv, L"cancopy"); // Objects can be copied    (0x1)
    _store_mask(attrs, SFGAO_CANMOVE           , rv, L"canmove"); // Objects can be moved     (0x2)
    _store_mask(attrs, SFGAO_CANLINK           , rv, L"canlink"); // Objects can be linked    (0x4)
    _store_mask(attrs, SFGAO_STORAGE           , rv, L"storage"); // supports BindToObject(IID_IStorage)
    _store_mask(attrs, SFGAO_CANRENAME         , rv, L"canrename"); // Objects can be renamed
    _store_mask(attrs, SFGAO_CANDELETE         , rv, L"candelete"); // Objects can be deleted
    _store_mask(attrs, SFGAO_HASPROPSHEET      , rv, L"haspropsheet"); // Objects have property sheets
    _store_mask(attrs, SFGAO_DROPTARGET        , rv, L"droptarget"); // Objects are drop target
    _store_mask(attrs, SFGAO_CAPABILITYMASK    , rv, L"capabilitymask"); 
    _store_mask(attrs, SFGAO_SYSTEM            , rv, L"system"); // System object
    _store_mask(attrs, SFGAO_ENCRYPTED         , rv, L"encrypted"); // Object is encrypted (use alt color)
    _store_mask(attrs, SFGAO_ISSLOW            , rv, L"isslow"); // 'Slow' object
    _store_mask(attrs, SFGAO_GHOSTED           , rv, L"ghosted"); // Ghosted icon
    _store_mask(attrs, SFGAO_LINK              , rv, L"link"); // Shortcut (link)
    _store_mask(attrs, SFGAO_SHARE             , rv, L"share"); // Shared
    _store_mask(attrs, SFGAO_READONLY          , rv, L"readonly"); // Read-only
    _store_mask(attrs, SFGAO_HIDDEN            , rv, L"hidden"); // Hidden object
    _store_mask(attrs, SFGAO_DISPLAYATTRMASK   , rv, L"displayattrmask"); 
    _store_mask(attrs, SFGAO_FILESYSANCESTOR   , rv, L"filesysancestor"); // May contain children with SFGAO_FILESYSTEM
    _store_mask(attrs, SFGAO_FOLDER            , rv, L"folder"); // Support BindToObject(IID_IShellFolder)
    _store_mask(attrs, SFGAO_FILESYSTEM        , rv, L"filesystem"); // Is a win32 file system object (file/folder/root)
    _store_mask(attrs, SFGAO_HASSUBFOLDER      , rv, L"hassubfolder"); // May contain children with SFGAO_FOLDER (may be slow)
    _store_mask(attrs, SFGAO_CONTENTSMASK      , rv, L"contentsmask"); 
    _store_mask(attrs, SFGAO_VALIDATE          , rv, L"validate"); // Invalidate cached information (may be slow)
    _store_mask(attrs, SFGAO_REMOVABLE         , rv, L"removable"); // Is this removeable media?
    _store_mask(attrs, SFGAO_COMPRESSED        , rv, L"compressed"); // Object is compressed (use alt color)
    _store_mask(attrs, SFGAO_BROWSABLE         , rv, L"browsable"); // Supports IShellFolder, but only implements CreateViewObject() (non-folder view)
    _store_mask(attrs, SFGAO_NONENUMERATED     , rv, L"nonenumerated"); // Is a non-enumerated object (should be hidden)
    _store_mask(attrs, SFGAO_NEWCONTENT        , rv, L"newcontent"); // Should show bold in explorer tree
    _store_mask(attrs, SFGAO_CANMONIKER        , rv, L"canmoniker"); // Obsolete
    _store_mask(attrs, SFGAO_HASSTORAGE        , rv, L"hasstorage"); // Obsolete
    _store_mask(attrs, SFGAO_STREAM            , rv, L"stream"); // Supports BindToObject(IID_IStream)
    _store_mask(attrs, SFGAO_STORAGEANCESTOR   , rv, L"storageancestor"); // May contain children with SFGAO_STORAGE or SFGAO_STREAM
    _store_mask(attrs, SFGAO_STORAGECAPMASK    , rv, L"storagecapmask"); // For determining storage capabilities, ie for open/save semantics
    _store_mask(attrs, SFGAO_PKEYSFGAOMASK     , rv, L"pkeysfgaomask"); // Attributes that are masked out for PKEY_SFGAOFlags because they are considered to cause slow calculations or lack context (SFGAO_VALIDATE | SFGAO_ISSLOW | SFGAO_HASSUBFOLDER and others)

    return rv;
}

#undef _store_mask
