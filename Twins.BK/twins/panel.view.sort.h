#pragma once

#include "panel.view.item.h"
#include <boost/noncopyable.hpp>
#include <vector>

namespace Twins
{
    struct FSort: boost::noncopyable
    {
        typedef std::vector<int> IndexVector;

        FSort();
        ~FSort();

        int GetIndex(int n) const;
        void CreateIndex(int column, bool ascending, FItemVector const& items);
        int FindIndexByName(FItemVector const& items, std::wstring const& name) const;

    private:
        IndexVector Index;
    };
}
