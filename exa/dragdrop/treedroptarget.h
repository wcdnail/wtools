/**************************************************************************
   THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY OF
   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
   PARTICULAR PURPOSE.
   Author: Leon Finker  1/2001
**************************************************************************/
#ifndef __TREEDROPTARGET_H__
#define __TREEDROPTARGET_H__

class CTreeDropTarget : public CIDropTarget
{
public:
	CTreeDropTarget(HWND hTargetWnd):CIDropTarget(hTargetWnd){}	
	
	virtual bool OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium, DWORD *pdwEffect)
	{
		if(pFmtEtc->cfFormat == CF_TEXT && medium.tymed == TYMED_HGLOBAL)
		{
			TCHAR* pStr = (TCHAR*)GlobalLock(medium.hGlobal);
			if(pStr != NULL)
			{
				TVINSERTSTRUCT tvins;
				tvins.hParent = TVI_ROOT;
				tvins.hInsertAfter = TVI_LAST;
				TVITEM tvit={0};
				tvit.mask = TVIF_TEXT;
				tvit.pszText = pStr;
				tvins.item = tvit;
				TreeView_InsertItem(m_hTargetWnd, &tvins);
			}
			GlobalUnlock(medium.hGlobal);
		}
		if(pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL)
		{
			HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
			if(hDrop != NULL)
			{
				TCHAR szFileName[MAX_PATH];

				UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); 
				for(UINT i = 0; i < cFiles; ++i)
				{
				DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
				TVINSERTSTRUCT tvins;
				tvins.hParent = TVI_ROOT;
				tvins.hInsertAfter = TVI_LAST;
				TVITEM tvit={0};
				tvit.mask = TVIF_TEXT;
				tvit.pszText = szFileName;
				tvins.item = tvit;
				TreeView_InsertItem(m_hTargetWnd, &tvins);
				}  
				//DragFinish(hDrop); // base class calls ReleaseStgMedium
			}
			GlobalUnlock(medium.hGlobal);
		}
		TreeView_SelectDropTarget(m_hTargetWnd, NULL);
		return true; //let base free the medium
	}

	virtual HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ DWORD __RPC_FAR *pdwEffect)
	{
		TVHITTESTINFO hit;
		hit.pt = (POINT&)pt;
		ScreenToClient(m_hTargetWnd,&hit.pt);
		hit.flags = TVHT_ONITEM;
		HTREEITEM hItem = TreeView_HitTest(m_hTargetWnd,&hit);
		if(hItem != NULL)
		{
			TreeView_SelectDropTarget(m_hTargetWnd, hItem);
		}
		return CIDropTarget::DragOver(grfKeyState, pt, pdwEffect);
	}
	virtual HRESULT STDMETHODCALLTYPE DragLeave(void)
	{
		TreeView_SelectDropTarget(m_hTargetWnd, NULL);
		return CIDropTarget::DragLeave();
	}

};

#endif //__TREEDROPTARGET_H__