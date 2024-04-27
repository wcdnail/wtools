#pragma once

#include "file.list.h"
#include "panel.view.item.h"
#include <reflection/ref.std.h>
#include <boost/noncopyable.hpp>
#include <vector>
#include <string>
#include <atltypes.h>
#include <atlgdi.h>
#include <atltime.h>

namespace Twins
{
    class PanelHeader;
    struct FSort;

    class PanelColumns: boost::noncopyable
    {
    public:
        typedef std::vector<int> UserColumns;
        typedef Ref::Container<UserColumns> RefUserColumns;

        enum 
        { 
            IdName = 0,
            IdExt,
            IdSize,
            IdDate,
            IdAttribs,
            IdShellType,
            MaxCount
        };

        PanelColumns();
        ~PanelColumns();

        void OnCreate(PanelHeader& header);
        void OnDestroy(PanelHeader& header);
        bool OnContextMenu(PanelHeader& header, CPoint const& pn);
        bool DoSort(int id, PanelHeader& header, FSort& sorter, FItemVector const& items);

        void ResetColumnsToDefault();
        static void ResetWidthsToDefault();

        int iHot;
        bool Ascending[MaxCount];
        bool Used[MaxCount];

        static int Widths[MaxCount];

    private:
        typedef std::vector<PanelHeader*> Headers;

        CMenu Popup;
        
        static unsigned NameId[MaxCount];
        static void AddColumn(PanelHeader& header, int id);
        static Headers AssignedHeaders;

        int GetIndex(int id) const;
        void OnResizeDone(int, int);
        void RebuildPopup(int cmdbase);
        void ResetHeader(PanelHeader& header) const;
    };
}
