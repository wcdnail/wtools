#pragma once

#include <atlapp.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlframe.h>
#include <atlcrack.h>

namespace PText
{
   class FrameWnd: WTL::CFrameWindowImpl<FrameWnd, ATL::CWindow>
   {
   private:
      typedef WTL::CFrameWindowImpl<FrameWnd, ATL::CWindow> Super;

   public:
      FrameWnd();
      ~FrameWnd();

      using Super::Create;
      using Super::ShowWindow;

   private:
      friend class Super;

      BEGIN_MSG_MAP_EX(FrameWnd)
         MSG_WM_CREATE(OnCreate)
         MSG_WM_DESTROY(OnDestroy)
         MSG_WM_ERASEBKGND(OnEraseBkgnd)
      END_MSG_MAP()

      int OnCreate(LPCREATESTRUCT);
      void OnDestroy();
      BOOL OnEraseBkgnd(CDCHandle dc);

   private:
      FrameWnd(FrameWnd const&);
      FrameWnd& operator = (FrameWnd const&);
   };

   inline FrameWnd::FrameWnd()
      : Super()
   {}

   inline FrameWnd::~FrameWnd()
   {}

   inline int FrameWnd::OnCreate(LPCREATESTRUCT)
   {
      return 0;
   }

   inline void FrameWnd::OnDestroy()
   {
      ::PostQuitMessage(0);
      SetMsgHandled(FALSE);
   }

   inline BOOL FrameWnd::OnEraseBkgnd(CDCHandle dc)
   {
      CRect rc;
      GetClientRect(rc);
      dc.FillSolidRect(rc, 0);
      return 1;
   }
}
